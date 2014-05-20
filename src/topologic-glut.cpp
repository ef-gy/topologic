/**\ingroup topologic-frontend
 * \defgroup frontend-glut GLUT frontend
 * \brief Topologic frontend using GLUT
 *
 * More of an example than a proper frontend, this is the GLUT version of
 * Topologic. This version doesn't provide any options to change the model at
 * runtime, so you have to use the same kind of command line parameters that
 * the CLI frontend uses. After launching, the programme will try to get an
 * OpenGL 3.2 full-screen context and allows you to manipulate the rendering
 * by dragging the mouse.
 *
 * \{
 */

/**\file
 * \brief Topologic/GLUT frontend
 *
 * Implements a simple Topologic frontend using GLUT and an OpenGL 3.2 context.
 *
 * \copyright
 * Copyright (c) 2012-2014, Topologic Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic
 * \see Project Source Code: https://github.com/ef-gy/topologic.git
 */

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>          /* Open GL Util APPLE */
#else
#include <GL/glew.h>
#include <GL/glut.h>            /* FreeGLUT (hopefully) */
#endif

#include <topologic/arguments.h>

#if !defined(MAXDEPTH)
/**\brief Maximum render depth
 *
 * This macro is used by some of the frontends to determine the maximum render
 * depth supported by a frontend. The default value is '7', which is plenty for
 * most applications - increasing this value will increase the size of the
 * generated code, so it may be desirable to decrease this value in
 * environments with tighter constraints.
 */
#define MAXDEPTH 7
#endif

/**\brief Current X position of cursor
 *
 * Contains the current X position of the cursor in screen coordinates.
 */
static GLfloat mouseX;

/**\brief Current Y position of cursor
 *
 * Contains the current Y position of the cursor in screen coordinates.
 */
static GLfloat mouseY;

/**\brief X position of cursor at time of last processing
 *
 * Contains the X coordinate of the cursor, in screen coordinates, at the last
 * point in time the programme tried to figure out if the cursor was dragged
 * over the screen.
 */
static GLfloat lastMouseX;

/**\brief Y position of cursor at time of last processing
 *
 * Contains the Y coordinate of the cursor, in screen coordinates, at the last
 * point in time the programme tried to figure out if the cursor was dragged
 * over the screen.
 */
static GLfloat lastMouseY;

/**\brief Is the left mouse button currently down?
 *
 * Set to 'true' when the left mouse button is pressed and then reset once this
 * button is released.
 */
static bool mouseLeft;

/**\brief Is the right mouse button currently down?
 *
 * Set to 'true' when the right mouse button is pressed and then reset once
 * this button is released.
 */
static bool mouseRight;

/**\brief Is the 'shift' button currently down?
 *
 * Set to 'true' when the 'shift' modifier key on the keyboard is pressed;
 * reset to 'false' when the 'shift' key is released.
 */
static bool shiftActive;

/**\brief Global state object
 *
 * Topologic programme state instance for this programme. Has to be global,
 * because we need to provide C-like accessors for emscripten/JavaScript
 * callers.
 */
static topologic::state<GLfloat,MAXDEPTH> topologicState;

/**\brief GLUT display callback
 *
 * Called by GLUT whenever it thinks the current scene should be drawn anew.
 */
static void displayCall(void)
{
    if (topologicState.model)
    {
        topologicState.model->opengl(true);
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

/**\brief GLUT reshape callback
 *
 * Called by GLUT whenever the output window is resized; since the GLUT
 * frontend requests a full screen window this should only happen once at
 * programme startup.
 *
 * \param[in] width  The width, in pixels, of the output viewport
 * \param[in] height The height, in pixels, of the output viewport
 */
static void reshape(GLint width, GLint height)
{
    glClearDepth(1.0f);

    glEnable (GL_BLEND);

    glEnable(GL_CULL_FACE);

    topologicState.width  = width;
    topologicState.height = height;
}

/**\brief GLUT mouse processing callback
 *
 * Called by GLUT whenever the mouse moves; used to find out if the mouse has
 * been dragged or if the mouse has been moved with the shift key down.
 *
 * \param[in] x Current X coordinate of mouse cursor
 * \param[in] y Current Y coordinate of mouse cursor
 */
static void processMouse(int x, int y)
{
    mouseX = x;
    mouseY = y;

    if (mouseLeft || mouseRight)
    {
        double xd = (mouseX - lastMouseX);
        double yd = (mouseY - lastMouseY);

        if (shiftActive)
        {
            topologicState.interpretDrag(0,0,xd+-yd);
        }
        else
        {
            topologicState.interpretDrag(xd,yd,0);
        }

        lastMouseX = mouseX;
        lastMouseY = mouseY;

        glutPostRedisplay();
    }
}

/**\brief GLUT mouse button callback
 *
 * Called by GLUT whenever any of the mouse buttons' state changes, e.g. when
 * the user clicks somewhere or releases a button after dragging the cursor.
 *
 * \param[in] button Which button changed state
 * \param[in] state  The new state of the button
 * \param[in] x      Current X coordinate of mouse cursor
 * \param[in] y      Current Y coordinate of mouse cursor
 */
static void processMouseButton(int button, int state, int x, int y)
{
    shiftActive = (glutGetModifiers() & GLUT_ACTIVE_SHIFT);

    processMouse(x,y);

    switch (button)
    {
        case 0:
            mouseLeft = (state == 0);
            break;
        case 2:
            mouseRight = (state == 0);
            break;
        case 3:
            if (state == 0)
            {
                topologicState.interpretDrag(0,0,30);
            }
            break;
        case 4:
            if (state == 0)
            {
                topologicState.interpretDrag(0,0,-30);
            }
            break;
    }

    if (mouseLeft || mouseRight)
    {
        lastMouseX = mouseX;
        lastMouseY = mouseY;

        glutPostRedisplay();
    }
}

/**\brief GLUT keyboard callback
 *
 * Called by GLUT whenever a key on the keyboard is pressed. Used to determine
 * which dimension in the state object should currently be set to 'active'.
 *
 * \param[in] key The key that was pressed
 * \param[in] x   Current X coordinate of mouse cursor
 * \param[in] y   Current Y coordinate of mouse cursor
 */
static void processKeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case '1': topologicState.setActive ( 3); break;
        case '2': topologicState.setActive ( 4); break;
        case '3': topologicState.setActive ( 5); break;
        case '4': topologicState.setActive ( 6); break;
        case '5': topologicState.setActive ( 7); break;
        case '6': topologicState.setActive ( 8); break;
        case '7': topologicState.setActive ( 9); break;
        case '8': topologicState.setActive (10); break;
        case '9': topologicState.setActive (11); break;
        case '0': topologicState.setActive (12); break;
        case 'r':
            topologicState.realign();
            glutPostRedisplay();
            break;
    }

    glutPostRedisplay();
}

/**\brief Topologic/GLUT main function
 *
 * Parses the command line arguments with topologic::parseArguments, then
 * initialises an OpenGL 3.2 context with GLUT and commences rendering the
 * model described on the command line in a loop.
 *
 * \param[in] argc The number of arguments in the argv array.
 * \param[in] argv The actual command line arguments passed to the programme.
 *
 * \returns 0 on success, nonzero otherwise.
 */
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
            glutInitWindowSize(1280, 720);

#ifdef __APPLE__
	        glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
#if defined(GLUT_CORE_PROFILE)
	        glutInitContextVersion(3, 2);
	        glutInitContextProfile(GLUT_CORE_PROFILE);
#endif
	        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
            glutCreateWindow("Topologic/GLUT");
#ifndef __APPLE__
            glewInit();
#endif

            glutDisplayFunc (displayCall);
            glutReshapeFunc (reshape);
            glutMouseFunc(processMouseButton);
            glutMotionFunc(processMouse);
            glutPassiveMotionFunc(processMouse);
            glutKeyboardFunc(processKeyboard);
            glutFullScreen();

            glutMainLoop();
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

/** \} */
