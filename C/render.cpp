#include "render.h"

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render()
{
    auto startTime = std::chrono::high_resolution_clock::now();
    std::cout << "light count: " << this->scene.lights.size() << std::endl;

    for (int x = 0; x < this->scene.imageResolution.x; x++) {
        for (int y = 0; y < this->scene.imageResolution.y; y++) {

            Vector3f result(0, 0, 0);

            // std::cout << "x: " << x << std::endl; 1024, 1016
            // std::cout << "y: " << y << std::endl;


            for(int i = 0; i < this->scene.spp; i++)
            {
                float epsilon = next_float();
                Ray cameraRay = this->scene.camera.generateRay(x, y, epsilon);
                Interaction si = this->scene.rayIntersect(cameraRay);

                if (si.didIntersect) 
                {
                    //check light intersect and add the emission 
                    
                    Vector3f radiance;
                    LightSample ls;

                    for (Light &light : this->scene.lights) 
                    {
                        if(light.type != LightType::AREA_LIGHT)
                        {
                            std::tie(radiance, ls) = light.sample(&si);

                            Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                            Interaction siShadow = this->scene.rayIntersect(shadowRay);

                            if (!siShadow.didIntersect || siShadow.t > ls.d) {
                                result += si.bsdf->eval(&si, si.toLocal(ls.wo))  * radiance * std::abs(Dot(si.n, ls.wo));
                            }               
                        }
                    }

                    //area light intersection
                    Vector3f loc;
                    if(this->scene.sampling == 0)
                    {
                        loc = SampleHemisphereUniform();

                        Vector3f glob = si.toWorld(loc);

                        Ray shadowRay(si.p + 1e-3f * si.n, glob);

                        Interaction siShadow = this->scene.rayIntersect(shadowRay);

                        Interaction lightIntersect = this->scene.rayEmitterIntersect(shadowRay);

                        if (siShadow.t > lightIntersect.t && lightIntersect.didIntersect) 
                        {
                            result +=  2*M_PI * si.bsdf->eval(&si, si.toLocal(glob))  * lightIntersect.emissiveColor * std::abs(Dot(si.n, glob));
                        }
                    }
                    else if(this->scene.sampling == 1)
                    {
                        loc = SampleHemisphereCosine();

                        Vector3f glob = si.toWorld(loc);

                        Ray shadowRay(si.p + 1e-3f * si.n, glob);

                        Interaction siShadow = this->scene.rayIntersect(shadowRay);

                        Interaction lightIntersect = this->scene.rayEmitterIntersect(shadowRay);

                        if (siShadow.t > lightIntersect.t && lightIntersect.didIntersect) 
                        {
                            result +=  2*M_PI * si.bsdf->eval(&si, si.toLocal(glob))  * lightIntersect.emissiveColor * std::abs(Dot(si.n, glob));
                        }
                    }
                    else if(this->scene.sampling == 2)
                    {
                        //importance sampling
                        //choose a light at random to sample from
                        //call sample to get a sample from that light
                        
                        float rand = next_float();

                        int lightIndex = rand * this->scene.lights.size();
                        if(lightIndex < this->scene.lights.size())  
                        {
                            std::tie(radiance, ls) = this->scene.lights[lightIndex].sample(&si);

                            Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                            Interaction siShadow = this->scene.rayIntersect(shadowRay);

                            if (!siShadow.didIntersect || siShadow.t > ls.d) {
                                result += si.bsdf->eval(&si, si.toLocal(ls.wo))  * radiance * this->scene.lights.size() * std::abs(Dot(si.n, ls.wo));
                            }    
                        }
                    }
                }
                for (Light &light : this->scene.lights) 
                {
                    Interaction li = light.intersectLight(&cameraRay);
                    if (li.didIntersect) 
                    {
                        result += li.emissiveColor;
                    }
                }
            }

            //MC average

            result = result / this->scene.spp;

            this->outputImage.writePixelColor(result, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char **argv)
{
    if (argc != 5) {
        std::cerr << "Usage: ./render <scene_config> <out_path> <num_samples> <sampling_strategy>";
        return 1;
    }
    Scene scene(argv[1]);

    // for(auto light: scene.lights)
    // {
    //     std::cout << "Light: " << light.type << std::endl;
    //     std::cout << "Radiance: " << light.radiance.x << light.radiance.y << light.radiance.z  << std::endl;
    //     std::cout << "center: " << light.center.x << light.center.y << light.center.z << std::endl;
    //     std::cout << "vx: " << light.vx.x << light.vx.y << light.vx.z << std::endl;
    //     std::cout << "vy: " << light.vy.x << light.vy.y << light.vy.z << std::endl;

    // }

    int spp = atoi(argv[3]);
    scene.spp = spp;

    int sampling = atoi(argv[4]);
    scene.sampling = sampling;

    Integrator rayTracer(scene);

    auto renderTime = rayTracer.render();
    
    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}

Vector3f SampleHemisphereUniform() 
{
    float r1 = next_float();
    float r2 = next_float();

    float theta = 2 * M_PI * r1;
    float phi = acos(r2);

    float x = sin(phi) * cos(theta);
    float y = sin(phi) * sin(theta);
    float z = cos(phi);

    Vector3f d(x, y, z);

    return d;
}

Vector3f SampleHemisphereCosine()
{
    float r1 = next_float();
    float r2 = next_float();

    float theta = 2 * M_PI * r1;
    float r = sqrt(r2);


    float x =  r* cos(theta);
    float y =  r * sin(theta);
    float z =  sqrt(std::max(0.0f, 1 - r2));

    Vector3f d(x, y, z);

    return d;
}
