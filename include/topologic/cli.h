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

#if !defined(TOPOLOGIC_CLI_H)
#define TOPOLOGIC_CLI_H

#define NO_OPENGL
#include <topologic/arguments.h>

#if !defined(MAXDEPTH)
#define MAXDEPTH 7
#endif

namespace topologic
{
    typedef efgy::math::primitive<double> FP;

    template<typename FP>
    int cli (int argc, char* argv[])
    {
        try
        {
            state<FP,MAXDEPTH> topologicState;

            if (!parseArguments (topologicState, argc, argv, outSVG))
            {
                return 1;
            }

            if (!topologicState.model)
            {
                std::cerr << "error: no model to render\n";
            }
            else
            {
                std::cout << topologicState.model->render(true).str();
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
};

#endif
