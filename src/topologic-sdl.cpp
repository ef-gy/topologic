/**\file
 * \ingroup topologic-frontend
 * \brief Topologic/SDL frontend
 *
 * This is a basic SDL+OpenGL frontend for Topologic, designed for use with
 * emscripten targeted at WebGL.
 *
 * \copyright
 * Copyright (c) 2012-2013, Topologic Project Members
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

#if defined(EMSCRIPTEN)
#include <emscripten.h>
#endif

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"

#include <iostream>
#include <topologic/gl.h>

#if !defined(MAXDEPTH)
#define MAXDEPTH 7
#endif

static topologic::state<topologic::GLFP,MAXDEPTH> topologicState;

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

static bool buttonDown = false;
static bool doRender = true;

void forceRedraw(void)
{
    doRender = true;
}

void setFlameParameters(int variants)
{
    topologicState.parameter.flameCoefficients = variants;
}

void resetColourMap(void)
{
    topologicState.opengl.setColourMap();
    doRender = true;
}

void setViewportSize(int width, int height)
{
    topologicState.width = width;
    topologicState.height = height;
    doRender = true;
}

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

/**\brief Topologic/SDL main function
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

int setRadius(double radius)
{
    topologicState.topologic::state<topologic::GLFP,2>::parameter.polarRadius = topologic::GLFP(radius);
    return 0;
}

int setPrecision(double precision)
{
    topologicState.topologic::state<topologic::GLFP,2>::parameter.polarPrecision = topologic::GLFP(precision);
    return 0;
}

int updateModel(char *smodel, int dim, int rdim)
{
    topologic::setModelWithTypeString<topologic::GLFP,MAXDEPTH,MAXDEPTH,topologic::render::opengl> (std::string(smodel), topologicState, dim, rdim);

    return 0;
}

int updateProjection()
{
    topologicState.topologic::state<topologic::GLFP,MAXDEPTH>::updateMatrix();

    return 0;
}

int interpretDrag (double x, double y, double z)
{
    topologicState.interpretDrag(topologic::GLFP(x),topologic::GLFP(y),topologic::GLFP(z));
    return 0;
}

int setActiveDimension(int dim)
{
    topologicState.setActive(dim);
    return 0;
}

void setIFSParameters(int iterations, int seed, int functions, bool preRotate, bool postRotate)
{
    topologicState.parameter.iterations = iterations;
    topologicState.parameter.seed       = seed;
    topologicState.parameter.functions  = functions;
    topologicState.parameter.preRotate  = preRotate;
    topologicState.parameter.postRotate = postRotate;

//    std::cerr << "(" << iterations << ", " << seed << ", " << functions << ", " << preRotate << ", " << postRotate << ")\n";
}

void setFlameColouring(bool flameColouring)
{
    topologicState.fractalFlameColouring = flameColouring;
}

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
