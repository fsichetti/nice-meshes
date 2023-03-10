#include "Browser.hpp"

#define GLUT_SCROLL_UP 3
#define GLUT_SCROLL_DOWN 4

namespace Browser {
    Trackball* trackball = nullptr;
    Mesh* mesh = nullptr;
    std::string outPath = "./";

    bool drawWireframe = false;
    bool drawUV = false;

    Shader* progSmooth; 
    Shader* progLines;
    bool mouseDragL = false;
    bool mouseDragR = false;
    int mouseLastX, mouseLastY;

    void init(char* name, Trackball* trb) {
        trackball = trb;

        // Init window
        int c = 1;
        const int wW = 1920, wH = 1080;
        glutInit(&c, &name);
        glutInitWindowPosition(0, 0);
        glutInitWindowSize(wW, wH);
        trackball->setDimensions(wW, wH); //TEMPORARY
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
        glutCreateWindow("Viewer");

        // Return control to application on window close for cleanup
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, 
            GLUT_ACTION_CONTINUE_EXECUTION);

        // Callbacks
        glutDisplayFunc(callbackDisplay);
        glutSpecialFunc(callbackSpecial);
        glutKeyboardFunc(callbackKeyboard);
        glutMouseFunc(callbackMouse);
        glutMotionFunc(callbackMotion);
        glutReshapeFunc(callbackReshape);
        glutCloseFunc(callbackClose);
        
        // OpenGL Settings
        glClearColor(.4, .45, .4, 0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_BACK);
        // glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Shader programs
        const std::string f = "shaders/";
        progSmooth = new Shader(f+"smoothV.glsl", f+"smoothF.glsl");
        progLines = new Shader(f+"simpleV.glsl", f+"linesG.glsl", f+"monochromeF.glsl");
    }

    void setMesh(Mesh* m) { mesh = m; }
    void setOutPath(std::string o) { outPath = o; }
    void launch() { glutMainLoop(); }
    
    void callbackDisplay() {
        // Clear buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto mat = trackball->mvpPtr();
        auto vDir = trackball->viewDirection();

        const float meshAlpha = 1.0;

        // Mesh
        glUseProgram(*progSmooth);
        glUniformMatrix4fv(0, 1, GL_TRUE, mat);     // mvp
        glUniform3f(1, vDir[0], vDir[1], vDir[2]);  // view direction
        glUniform1f(2, meshAlpha);                  // alpha
        glUniform1i(3, drawUV);
        mesh->draw();

        if (drawWireframe) {
            glUseProgram(*progLines);
            glUniformMatrix4fv(0, 1, GL_TRUE, mat); //mvp
            glUniform4f(2, 0, 0, 0, meshAlpha); // colour
            mesh->draw();
        }

        // Show
        glutSwapBuffers();
    }

    void callbackSpecial(int key, int x, int y) {
        // unused now
    }

    void callbackKeyboard(unsigned char key, int x, int y) {
        std::string n;
        switch (key) {
            // Reset trackball
            case 'r':
                trackball->reset();
                glutPostRedisplay();
                break;

            // Toggle wireframe
            case 'w':
                drawWireframe = !drawWireframe;
                glutPostRedisplay();
                break;

            // Toggle uv
            case 'u':
                drawUV = !drawUV;
                glutPostRedisplay();
                break;

            // Screenshot
            case 's':
                n = "./shot.png";
                screenshot(n);
                std::cout << "Screenshot taken (" + n << ")" << std::endl;
                break;
            
            // Export OBJ
            case 'o':
                n = outPath + mesh->name + ".obj";
                try { mesh->writeOBJ(n); }
                catch(std::exception &ex) {
                    std::cerr << "File " << n << ": " << ex.what() << std::endl;
                }
                std::cout << "Exported mesh (" << n << ")" << std::endl;
                break;

            // Export PLY
            case 'p':
                n = outPath + mesh->name + ".ply";
                try { mesh->writePLY(n); }
                catch(std::exception &ex) {
                    std::cerr << "File " << n << ": " << ex.what() << std::endl;
                }
                std::cout << "Exported mesh (" << n << ")" << std::endl;
                break;

            // Export OFF
            case 'f':
                n = outPath + mesh->name + ".off";
                try { mesh->writeOFF(n); }
                catch(std::exception &ex) {
                    std::cerr << "File " << n << ": " << ex.what() << std::endl;
                }
                std::cout << "Exported mesh (" << n << ")" << std::endl;
                break;
        }
    }

    void callbackMouse(int key, int state, int x, int y) {
        switch (key) {
            case GLUT_LEFT_BUTTON:
                if (state == GLUT_DOWN) {
                    mouseDragL = true;
                    mouseLastX = x;
                    mouseLastY = y;
                }
                else mouseDragL = false;
                break;
            case GLUT_RIGHT_BUTTON:
                if (state == GLUT_DOWN) {
                    mouseDragR = true;
                    mouseLastX = x;
                    mouseLastY = y;
                }
                else mouseDragR = false;
                break;
            case GLUT_SCROLL_UP:
                if (state == GLUT_DOWN) {
                    trackball->zoom(1);
                    glutPostRedisplay();
                }
                break;
            case GLUT_SCROLL_DOWN:
                if (state == GLUT_DOWN) {
                    trackball->zoom(-1);
                    glutPostRedisplay();
                }
                break;
        }
    }

    void callbackMotion(int x, int y) {
        if (mouseDragL || mouseDragR) {
            if (mouseDragL) trackball->dragged(mouseLastX, mouseLastY, x, y);
            if (mouseDragR) trackball->panning(mouseLastX, mouseLastY, x, y);
            glutPostRedisplay();
            mouseLastX = x;
            mouseLastY = y;
        }
    }

    void callbackReshape(int w, int h) {
        trackball->setDimensions(w, h);
        glViewport(0, 0, w, h);
    }

    void callbackClose() {
        delete progLines;
        delete progSmooth;
        mesh->deleteBuffers();
        glutLeaveMainLoop();
    }

    void screenshot(std::string filename) {
        uint w, h;
        trackball->getDimensions(w, h);

        // Read raw data
        GLubyte* pixels = new GLubyte[3 * w * h];
        glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, pixels);

        // Convert
        FIBITMAP* image = FreeImage_ConvertFromRawBits(
            pixels, w, h, 3 * w, 24, 
            0xFF0000, 0x00FF00, 0x0000FF, false
        );

        // Save
        FreeImage_Save(FIF_PNG, image, (filename).c_str(), 0);

        // Clean up
        FreeImage_Unload(image);
        delete [] pixels;
    }

    // void message(float x, float y, std::string msg) {
    //     glColor3f(1., 1., 1.); 
    //     glRasterPos2f(x, y);
    //     glutBitmapString(GLUT_BITMAP_8_BY_13, 
    //     reinterpret_cast<const unsigned char*>(msg.c_str()));
    //     glutPostRedisplay();
    // }
}