#include "render.h"
#include "light.h"
#include "random.h"

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render()
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++) 
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++) 
        {
            //32 sub sample loop
            float epsilon = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

            Ray cameraRay = this->scene.camera.generateRay(x, y, epsilon);
            Interaction si = this->scene.rayIntersect(cameraRay);

            Vector3f color(0.f, 0.f, 0.f);
            
            if (si.didIntersect)
            {

                for (auto& light : this->scene.lights) 
                {
                    if (light.type == LightType::DIRECTIONAL_LIGHT) 
                    {
                        Vector3f origin = si.p + 0.001f * si.n;
                        Vector3f dir = (light.direction);

                        //shadow ray    
                        Ray shadowRay(origin, Normalize(dir));

                        Interaction shadowSi = this->scene.rayIntersect(shadowRay);

                        if (!shadowSi.didIntersect)
                        {
                            //Vector3f c(1.f, 1.f, 1.f);
                            Vector3f c;

                            if(this->scene.interpol)
                            {
                                if(si.diffuse)
                                {
                                    c = scene.surfaces[si.surface].diffuseTexture.bilinearFetch(si.uv_intersect);
                                }
                                else
                                {
                                    c = scene.surfaces[si.surface].diffuse;
                                }
                            }
                            else
                            {
                                if(si.diffuse)
                                {
                                    c = scene.surfaces[si.surface].diffuseTexture.nearestNeighbourFetch(si.uv_intersect);
                                }
                                else
                                {
                                    c = scene.surfaces[si.surface].diffuse;
                                }
                            }


                            auto cost = std::abs(Dot(si.n, dir));

                            color += light.radiance*(c/M_PI)*cost;
                            
                        }
                    }
                    else if(light.type == LightType::POINT_LIGHT)
                    {
                        Vector3f origin = si.p + 0.001f * si.n;

                        Vector3f dir = (light.location - si.p);

                        //shadow ray    
                        Ray shadowRay(origin, Normalize(dir));

                        Interaction shadowSi = this->scene.rayIntersect(shadowRay);

                        float r = std::abs(Dot(dir, dir));

                        // r = std::sqrt(r);

                        if ( shadowSi.t >= std::sqrt(r))
                        {
                            //Vector3f c(1.f, 1.f, 1.f);
                            Vector3f c;
                            if(this->scene.interpol)
                            {
                                if(si.diffuse)
                                {
                                    c = scene.surfaces[si.surface].diffuseTexture.bilinearFetch(si.uv_intersect);
                                }
                                else
                                {
                                    c = scene.surfaces[si.surface].diffuse;
                                }
                            }
                            else
                            {
                                if(si.diffuse)
                                {
                                    c = scene.surfaces[si.surface].diffuseTexture.nearestNeighbourFetch(si.uv_intersect);
                                }
                                else
                                {
                                    c = scene.surfaces[si.surface].diffuse;
                                }
                            }

                            auto cost = std::abs(Dot(si.n, Normalize(dir)));

                            color += light.radiance*(c/(M_PI*r))*cost;
                        }
                    }
                }

                this->outputImage.writePixelColor(color, x, y);
            }   
            else
            {
                this->outputImage.writePixelColor(Vector3f(0.f, 0.f, 0.f), x, y);
            }
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        std::cerr << "Usage: ./render <scene_config> <out_path> <interpolation_type>";
        return 1;
    }
    Scene scene(argv[1]);

    scene.interpol = std::stoi(argv[3]);

    Integrator rayTracer(scene);
    auto renderTime = rayTracer.render();
    
    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}
