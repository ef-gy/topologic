/*
 * This file is part of the Topologic CLI project.
*/

/*
 * Copyright (c) 2012-2013, ef.gy Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#ifdef __APPLE__
#include <GLUT/glut.h>          /* Open GL Util    APPLE */
#else
#include <GL/glut.h>            /* Open GL Util    OpenGL*/
#endif

#include <topologic/glut.h>

#if !defined(MAXDEPTH)
#define MAXDEPTH 7
#endif

GLint windowWidth, windowHeight;

GLdouble mouseX, mouseY;
GLdouble lastMouseX, lastMouseY;

bool mouseLeft, mouseRight;

topologic::state<topologic::GLFP,MAXDEPTH> topologicState;

void displayCall(void)
{
    static int lastFrameTime = 0;

    if (lastFrameTime == 0)
    {
        lastFrameTime = glutGet(GLUT_ELAPSED_TIME);
    }

    int now = glutGet(GLUT_ELAPSED_TIME);
    int elapsedMilliseconds = now - lastFrameTime;
    float elapsedTime = elapsedMilliseconds / 1000.0f;
    lastFrameTime = now;

    static double position = 0;

    position += elapsedTime;

    if (topologicState.model)
    {
        (*topologicState.model)(true);
    }

    glutSwapBuffers();
}

void reshape(GLint width, GLint height)
{
    glViewport(0, 0, width, height);
    glShadeModel (GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0f);

    glEnable (GL_BLEND);
    glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable (GL_COLOR_MATERIAL) ;
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LIGHT0);

    windowWidth  = width;
    windowHeight = height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#if defined(GL3D)
    gluPerspective(45.0f, (double)windowWidth / (double)windowHeight, 0.5f, 500.0f);
#else
    glOrtho (1.8, -1.8, -1, 1, 0, 200);
    glDisable(GL_DEPTH_TEST);
#endif

    glMatrixMode(GL_MODULATE);
}

void idle(void)
{
    if (mouseLeft || mouseRight)
    {
        glutPostRedisplay();
    }
}

void processMouse(int x, int y)
{
    mouseX = x;
    mouseY = y;
}

void processMouseButton(int button, int state, int x, int y)
{
    processMouse(x,y);

    switch (button)
    {
        case 0:
            mouseLeft = (state == 0);
            break;
        case 2:
            mouseRight = (state == 0);
            break;
    }

    if (mouseLeft || mouseRight)
    {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }
}

int main (int argc, char* argv[])
{
    try
    {
        if (!topologic::parseArguments (topologicState, argc, argv, topologic::outGL))
        {
            return 1;
        }

        if (!topologicState.model)
        {
            std::cerr << "error: no model to render\n";
        }
        else
        {
            glutInit(&argc, argv);
            glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
            glutInitWindowSize(1280, 720);
            glutCreateWindow("Topologic/GLUT");
            glutDisplayFunc (displayCall);
            glutIdleFunc(idle);
            glutReshapeFunc (reshape);
            glutMouseFunc(processMouseButton);
            glutMotionFunc(processMouse);
            glutPassiveMotionFunc(processMouse);
            glutFullScreen();
            glutMainLoop();
            //std::cout << (*topologicState.model)(true).str();
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown Exception\n";
        return 1;
    }

    return 0;
}
