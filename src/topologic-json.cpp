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

topologic::state<FP,5> topologicState;

extern "C"
{
    int setRadius(double precision);
    int setPrecision(double precision);
    int updateModel(char *smodel, int dim, int rdim);
    int updateProjection(void);
    const char *getProjection(void);
    int mouseDrag (double, double);
}

int setRadius(double radius)
{
    topologicState.topologic::state<FP,2>::parameter.polarRadius = FP(radius);
    return 0;
}

int setPrecision(double precision)
{
    topologicState.topologic::state<FP,2>::parameter.polarPrecision = FP(precision);
    return 0;
}

int updateModel(char *smodel, int dim, int rdim)
{
    topologic::parseModelWithTypeStringParameters<FP,5,topologic::renderJSON> (topologicState, std::string(smodel), dim, rdim);

    return 0;
}

int updateProjection()
{
    topologicState.topologic::state<FP,5>::updateMatrix();

    return 0;
}

static std::string currentJSON("[ 'wireframe' ]");

const char *getProjection()
{
    if (topologicState.model)
    {
        currentJSON = (*topologicState.model)().str();
    }

    return currentJSON.c_str();
}

int mouseDrag (double x, double y)
{
    x /= 1000.;
    y /= 1000.;

    topologicState.topologic::state<FP,3>::fromp.data[1] += x;
    topologicState.topologic::state<FP,3>::fromp.data[2] += y;

    return 0;
}
