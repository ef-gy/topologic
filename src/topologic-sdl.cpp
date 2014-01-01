/**\file
 * \ingroup topologic-frontend
 * \brief Topologic/SDL frontend
 *
 * This is a basic SDL+OpenGL frontend for Topologic, designed for use with
 * emscripten targeted at WebGL.
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
 * \see Project Source Code: http://git.becquerel.org/jyujin/topologic.git
 */

/**\defgroup topologic-javascript-exports emscripten/JS Exported Functions
 * \brief Functions which emscripten will export when compiling to JavaScript
 *
 * Some functions are exported by emscripten so that they can be used by other
 * JavaScript code in the same JS context. These functions have an 'export "C"'
 * tag so they can be looked up without any name mangling issues and they're
 * grouped together in the documentation for convenience.
 */

#if defined(EMSCRIPTEN)
#include <emscripten.h>
#endif

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"

#include <iostream>
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

/**\brief Global state object
 *
 * Topologic programme state instance for this programme. Has to be global,
 * because we need to provide C-like accessors for emscripten/JavaScript
 * callers.
 */
static topologic::state<GLfloat,MAXDEPTH> topologicState;

// these functions are defined as 'extern "C"' to disable output name mangling;
// when compiling with emscripten these functions are exported so that they can
// be used in foreign JS code.
extern "C"
{
    void process(void);
    int setRadius(double);
    int setPrecision(double);
    int updateModel(char *, int, int);
    int updateProjection(void);
    int interpretDrag(double, double, double);
    int setActiveDimension(int);
    void forceRedraw(void);
    void setIFSParameters(int, int, int, bool, bool);
    void setFlameColouring(bool);
    void setColour(int, double, double, double, double);
    void setFlameParameters(int);
    void resetColourMap(void);
    void setViewportSize(int, int);
}

/**\brief Is a mouse button currently being?
 *
 * Set to 'true' whenever a mouse button has been pressed; reset to 'false'
 * when a button is no longer being pressed. Used to implement mouse 'drag'
 * input, which rotates the model in the currently active dimension.
 */
static bool buttonDown = false;

/**\brief Should the scene be rendered?
 *
 * Set to 'true' with the forceRedraw() function. The main loop will only draw
 * a new frame if this variable is true, so whenever you change any output
 * parameters you should call the forceRedraw() function to set this flag.
 *
 * The next time the main loop has completed drawing the scene it will reset
 * this flag to 'false'.
 */
static bool doRender = true;

/**\ingroup topologic-javascript-exports
 * \brief Render the scene
 *
 * Sets the 'doRender' variable to 'true' to make the main loop draw the
 * current scene.
 */
void forceRedraw(void)
{
    doRender = true;
}

/**\ingroup topologic-javascript-exports
 * \brief Update fractal flame parameter
 *
 * Modifies parameters of the global state object related to fractal flames.
 *
 * \param[in] variants The number of distinct, non-zero variant factors to use
 *                     when generating fractal flames.
 */
void setFlameParameters(int variants)
{
    topologicState.parameter.flameCoefficients = variants;
}

/**\ingroup topologic-javascript-exports
 * \brief Genere new colour map
 *
 * Creates a new colour map for the fractal flame rendering algorithm. The
 * colours in that map are chosen at random.
 */
void resetColourMap(void)
{
    topologicState.opengl.setColourMap();
    doRender = true;
}

/**\ingroup topologic-javascript-exports
 * \brief Resize viewport
 *
 * Call this function after resizing the viewport in the frontend; this is most
 * useful for the WebGL client, where SDL is not exactly in control of the
 * canvas size and the user might resize the browser window at any time.
 *
 * \param[in] width  The viewport's new width
 * \param[in] height The viewport's new height
 *
 * \bug The current WebGL client is not calling this function when the window
 *      is resized by the user. This results in pixelated output and/or an
 *      incorrect aspect ratio for most users.
 */
void setViewportSize(int width, int height)
{
    topologicState.width = width;
    topologicState.height = height;
    doRender = true;
}

/**\ingroup topologic-javascript-exports
 * \brief Main loop iteration
 *
 * Will try to process any pending SDL events in a loop and then return control
 * to the caller. This function is designed to be called directly in the
 * programme's main loop - either in main() itself, or, in an emscripten/JS
 * envionment, in whatever place emscripten_set_main_loop() saw fit to have it
 * run in.
 */
void process(void)
{
    const SDL_VideoInfo* info = SDL_GetVideoInfo();

    if (info)
    {
        topologicState.width  = info->current_w;
        topologicState.height = info->current_h;
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button)
                {
                    case 4:
                        topologicState.interpretDrag(0, 0, 30);
                        doRender = true;
                        break;
                    case 5:
                        topologicState.interpretDrag(0, 0, -30);
                        doRender = true;
                        break;
                    default:
                        buttonDown = true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                switch (event.button.button)
                {
                    case 4:
                    case 5:
                        break;
                    default:
                        buttonDown = false;
                }
                break;
            case SDL_MOUSEMOTION:
                if (buttonDown)
                {
                    topologicState.interpretDrag(event.motion.xrel, event.motion.yrel, 0);
                    doRender = true;
                }
                break;
        }
    }

    if (doRender)
    {
        doRender = false;

        if (topologicState.model)
        {
            topologicState.model->render(true);
        }

        SDL_GL_SwapBuffers();
    }
}

/**\ingroup topologic-javascript-exports
 * \brief Topologic/SDL main function
 *
 * Parses the command line arguments with topologic::parseArguments, then
 * initialises an OpenGL 3.2 context with SDL and commences rendering the
 * model described on the command line in a loop.
 *
 * This function will actually return rather swiftly if the code was compiled
 * with emscripten after setting the process() function as the main loop.
 *
 * \todo This function really needs some love and testing in environments other
 *       than emscripten/WebGL.
 *
 * \param[in] argc The number of arguments in the argv array.
 * \param[in] argv The actual command line arguments passed to the programme.
 *
 * \returns 0 on success, nonzero otherwise.
 */
int main(int argc, char *argv[])
{
    if (!topologic::parseArguments (topologicState, argc, argv, topologic::outGL))
    {
        return 1;
    }

    SDL_Surface *screen;

    // Slightly different SDL initialization
    if ( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
        std::cerr << "Unable to initialize SDL: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ); // *new*

    screen = SDL_SetVideoMode( 1280, 720, 16, SDL_OPENGL | SDL_RESIZABLE ); // *changed*
    if ( !screen ) {
        std::cerr << "Unable to set video mode: " << SDL_GetError() << "\n";
        return 1;
    }

    // Set the OpenGL state after creating the context with SDL_SetVideoMode

    glClearDepth(1.0f);

    glEnable (GL_BLEND);

    glEnable(GL_CULL_FACE);

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(process, 30, 0);
#else
    while (1)
    {
        process();
        SDL_Delay(50);
    }

    SDL_Quit();
#endif

    return 0;
}

/**\ingroup topologic-javascript-exports
 * \brief Set model radius
 *
 * Sets the polarRadius parameter used in some models.
 *
 * \param[in] radius The new value of the polarRadius parameter.
 *
 * \returns '0' if things went smoothly, nonzero otherwise. This method doesn't
 *          do anything that could fail, however, so it'll always return '0'.
 */
int setRadius(double radius)
{
    topologicState.topologic::state<GLfloat,2>::parameter.polarRadius = GLfloat(radius);
    return 0;
}

/**\ingroup topologic-javascript-exports
 * \brief Set model precision/quality
 *
 * Sets the polarPrecision parameter used in some models - this parameter
 * controls different aspects of different models, but in general higher values
 * result in better output quality.
 *
 * \param[in] precision The new value of the polarPrecision parameter.
 *
 * \returns '0' if things went smoothly, nonzero otherwise. This method doesn't
 *          do anything that could fail, however, so it'll always return '0'.
 */
int setPrecision(double precision)
{
    topologicState.topologic::state<GLfloat,2>::parameter.polarPrecision = GLfloat(precision);
    return 0;
}

/**\ingroup topologic-javascript-exports
 * \brief Update model settings
 *
 * This function is a wrapper for the efgy::geometry::with() function, which
 * allows you to choose a new model to be rendered.
 *
 * \returns 0 if your new settings didn't blow up the code; won't return if
 *          they did.
 */
int updateModel(char *smodel, int dim, int rdim)
{
    efgy::geometry::with<GLfloat,topologic::updateModelOpenGL,MAXDEPTH> (topologicState, smodel, dim, rdim);

    return 0;
}

/**\ingroup topologic-javascript-exports
 * \brief Update projection matrices
 *
 * Calls topologic::state::updateMatrix() on the global state object to update
 * the current projection matrices; use after updating any parameters that
 * are used to form the projection matrix, such as the from/to point or the
 * output window size.
 *
 * \returns '0' if things went smoothly, nonzero otherwise. This method doesn't
 *          do anything that could fail, however, so it'll always return '0'.
 */
int updateProjection(void)
{
    topologicState.topologic::state<GLfloat,MAXDEPTH>::updateMatrix();

    return 0;
}

/**\ingroup topologic-javascript-exports
 * \brief Interpret trackball/mouse drag events
 *
 * Call this functin after detecting mouse drag or scroll events. This is a
 * simple wrapper for topologic::state::interpretDrag(), so see that function's
 * documentation for further details.
 *
 * \returns '0' if things went smoothly, nonzero otherwise. This method doesn't
 *          do anything that could fail, however, so it'll always return '0'.
 */
int interpretDrag (double x, double y, double z)
{
    topologicState.interpretDrag(GLfloat(x),GLfloat(y),GLfloat(z));
    return 0;
}

/**\ingroup topologic-javascript-exports
 * \brief Set active dimension
 *
 * Sets the currently 'active' dimension. Each dimension of the global state
 * object has an 'active' flag, which determins whether certain operations,
 * such as interpreting a mouse drag event, apply to them; this function sets
 * that flag.
 *
 * \returns '0' if things went smoothly, nonzero otherwise. This method doesn't
 *          do anything that could fail, however, so it'll always return '0'.
 */
int setActiveDimension(int dim)
{
    topologicState.setActive(dim);
    return 0;
}

/**\ingroup topologic-javascript-exports
 * \brief Set IFS parameters
 *
 * Allows you to modify the model parameters related to (random) IFS creation
 * and rendition. Keep in mind that the fractal flames are also IFSs, so these
 * parameters also apply to fractal flames.
 *
 * \param[in] iterations The number of iterations to run an IFS when creating
 *                       the model.
 * \param[in] seed       Seed value for the mersenne twister used to create
 *                       random IFS functions.
 * \param[in] functions  Target number of functions to use with IFSs.
 * \param[in] preRotate  Whether to consider adding a pre-translation rotation
 *                       to random affine IFSs.
 * \param[in] postRotate Whether to consider adding a post-translation rotation
 *                       to random affine IFSs.
 */
void setIFSParameters(int iterations, int seed, int functions, bool preRotate, bool postRotate)
{
    topologicState.parameter.iterations = iterations;
    topologicState.parameter.seed       = seed;
    topologicState.parameter.functions  = functions;
    topologicState.parameter.preRotate  = preRotate;
    topologicState.parameter.postRotate = postRotate;
}

/**\ingroup topologic-javascript-exports
 * \brief Enable or disable fractal flame colouring
 *
 * Set the 'fractal flame colouring algorithm' flag of the global topologic
 * state object.
 *
 * \param[in] flameColouring Whether to enable the fractal flame colouring
 *                           algorithm.
 */
void setFlameColouring(bool flameColouring)
{
    topologicState.fractalFlameColouring = flameColouring;
}

/**\ingroup topologic-javascript-exports
 * \brief Set output colour
 *
 * Allows you to modify any of the global state's colour values. The 'colour'
 * parameter specifies which colour to set to (red, green, blue, alpha). Alpha
 * is used to provide transparency.
 *
 * The colour vector (red, green, blue, alpha) should have each component in
 * the range [0,1]. Setting the alpha component of the wireframe or surface
 * colour to 0 or less will disable rendering of wireframe lines or surfaces,
 * respectively.
 *
 * \param[in] colour Which colour to set. Accepted values are:
 *                     * 0: set the background colour
 *                     * 1: set the wireframe colour
 *                     * 2: set the surface colour
 * \param[in] red    Red component of the colour vector
 * \param[in] green  Green component of the colour vector
 * \param[in] blue   Blue component of the colour vector
 * \param[in] alpha  Alpha component of the colour vector
 */
void setColour(int colour, double red, double green, double blue, double alpha)
{
    switch (colour)
    {
        case 0:
            topologicState.background.red   = red;
            topologicState.background.green = green;
            topologicState.background.blue  = blue;
            topologicState.background.alpha = alpha;
            break;
        case 1:
            topologicState.wireframe.red   = red;
            topologicState.wireframe.green = green;
            topologicState.wireframe.blue  = blue;
            topologicState.wireframe.alpha = alpha;
            break;
        case 2:
            topologicState.surface.red   = red;
            topologicState.surface.green = green;
            topologicState.surface.blue  = blue;
            topologicState.surface.alpha = alpha;
            break;
    }
}
