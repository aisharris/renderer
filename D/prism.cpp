    #include <iostream>
    #include <stdio.h>
    #include <GL/glew.h>
    #include <GL/freeglut.h>
    // #include "math_3d.h"

    GLuint VBO;

    void display();
    void reshape(int width, int height);
    void keyboard(unsigned char key, int x, int y);
    static void CreateVertexBuffer();
    static void RenderSceneCB();
    static void CompileShaders();

    int main(int argc, char** argv) {
        
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
        glutInitWindowSize(1920, 1080);
        glutCreateWindow("OpenGL Prisms Project");

        GLenum err = glewInit();
        if (err != GLEW_OK) {
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
            return -1;
        }

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        // Additional initialization code

        CreateVertexBuffer();

        CompileShaders();

        glutDisplayFunc(RenderSceneCB);
        // glutReshapeFunc(reshape);
        // glutKeyboardFunc(keyboard);

        glutMainLoop();

        return 0;
    }

    static void CompileShaders()
    {
        GLuint ShaderProgram = glCreateProgram();

        if (ShaderProgram == 0)
        {
            fprintf(stderr, "Error creating shader program\n");
            exit(1);
        }

        std::string vs, fs;

        if(!ReadFile(pVSFileName, vs))
        {
            exit(1);
        }
    }

    static void RenderSceneCB()
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glEnableVertexAttribArray(0);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //3 for a 3d vector, type is float, normalize false, stride- number of bytes to the next vertex(0 for single attr), 0 offset

        //starts pipeline, gpu starts reading buffer, passes shaders and goes to rasterizer.
        glDrawArrays(GL_TRIANGLES, 0, 3);//primimtve type, start index, number of vertices to process

        glDisableVertexAttribArray(0);

        glutSwapBuffers();       

    }

    static void CreateVertexBuffer()
    {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);
        glCullFace(GL_FRONT);
        Vector3f Vertices[3];
        Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
        Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
        Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    }

    void display() {
        // Your rendering code here
        static GLclampf c = 0.0f;
        glClearColor(c, c, c, c);

        c+= 1.0f/256.0f;

        if(c >= 1.0f)
        {
            c = 0.0f;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glutPostRedisplay();
        glutSwapBuffers();
    }

    void reshape(int width, int height) {
        // Your reshape code here
        glViewport(0, 0, width, height);
    }

    void keyboard(unsigned char key, int x, int y) {
        // Your keyboard input handling code here
    }

