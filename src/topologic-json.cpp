/*
 * This file is part of the Topologic/Web project.
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

#define NO_OPENGL
#include <topologic/arguments.h>

typedef efgy::math::primitive<double> FP;

topologic::state<FP,4> topologicState;

static double origin4i = M_PI_2;
static double origin4j = M_PI_2;
static double origin4k = M_PI_2;

static double origin3i = -1;
static double origin3j = 1;

extern "C"
{
    int updateModel(char *smodel, int dim, int rdim);
    int updateProjection(void);
    const char *getProjection(void);
    int addOrigin3 (int, int);
    int setOrigin3 (int, int);
    int addOrigin4 (int, int, int);
    int setOrigin4 (int, int, int);
}

int updateModel(char *smodel, int dim, int rdim)
{
    topologic::parseModelWithTypeStringParameters<FP,4,topologic::renderJSON> (topologicState, std::string(smodel), dim, rdim);

    return 0;
}

int updateProjection()
{
    topologicState.topologic::state<FP,2>::polarCoordinates = false;

    topologicState.topologic::state<FP,4>::from.data[0] = 2.5 * cos(origin4i);
    topologicState.topologic::state<FP,4>::from.data[1] = 2.5 * sin(origin4i) * cos (origin4j);
    topologicState.topologic::state<FP,4>::from.data[2] = 2.5 * sin(origin4i) * sin (origin4j) * cos (origin4k);
    topologicState.topologic::state<FP,4>::from.data[3] = 2.5 * sin(origin4i) * sin (origin4j) * sin (origin4k);

    topologicState.topologic::state<FP,3>::from.data[0] = 2 * cos(origin3i);
    topologicState.topologic::state<FP,3>::from.data[1] = 2 * sin(origin3i) * cos (origin3j);
    topologicState.topologic::state<FP,3>::from.data[2] = 2 * sin(origin3i) * sin (origin3j);

    topologicState.topologic::state<FP,4>::updateMatrix();

    return 0;
}

const char *getProjection()
{
    if (topologicState.model)
    {
        return (*topologicState.model)().str().c_str();
    }

    return "[ 'wireframe' ];";
}

int addOrigin3 (int i, int j)
{
    origin3i += (double)i / 1000.f;
    origin3j += (double)j / 1000.f;

    return 0;
}

int setOrigin3 (int i, int j)
{
    origin3i = (double)i / 1000.f;
    origin3j = (double)j / 1000.f;

    return 0;
}

int addOrigin4 (int i, int j, int k)
{
    origin4i += (double)i / 1000.f;
    origin4j += (double)j / 1000.f;
    origin4k += (double)k / 1000.f;

    return 0;
}

int setOrigin4 (int i, int j, int k)
{
    origin4i = (double)i / 1000.f;
    origin4j = (double)j / 1000.f;
    origin4k = (double)k / 1000.f;

    return 0;
}

