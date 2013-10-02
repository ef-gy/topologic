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

#include <iostream>
#include <fstream>

#include <ef.gy/primitive.h>
#include <ef.gy/polytope.h>
#include <ef.gy/parametric.h>

#define NO_OPENGL
#include <topologic/state.h>
#include <topologic/parse.h>
#include <topologic/version.h>

#if !defined(MAXDEPTH)
#define MAXDEPTH 7
#endif

namespace topologic
{
    typedef efgy::math::primitive<double> FP;

    int cli (int argc, char* argv[])
    {
        try
        {
            state<FP,MAXDEPTH> topologicState;

            topologic::xml xml;
            unsigned int depth = 4, rdepth = 4;
            std::string model = "cube";

            if (argc > 1)
            {
                for (int i = 1; i < argc; i++)
                {
                    std::string arg = argv[i];

                    if (arg == "--help")
                    {
                        std::cout << "Usage:\n" << argv[0] << " [ options ] [ files ]\n\n"
                                  << argv[0] << " will render an SVG of a specific model to stdout.\n\n"
                                     "Options\n"
                                     "  --help           Show this help text, then exit.\n"
                                     "  --version        Show version information, then exit.\n"
                                     "\n"
                                     "  --depth N        Set model depth to N\n"
                                     "  --render-depth N Set render depth to N\n"
                                     "  --model M        Set model to M (cube, sphere, ...)\n"
                                     "\n"
                                     "  --polar          Use/manipulate polar coordinates (default)\n"
                                     "  --cartesian      Use/manipulate cartesian coordinates\n"
                                     "  --from D X Y ... Set the from-point of the D-to-D-1 projection to X Y ...\n"
                                     "\n"
                                     "See the man page of this programme for further details; e.g. run:\n"
                                     "man topologic\n\n";
                        return 0;
                    }
                    else if (arg == "--version")
                    {
                        std::cout << "Topologic CLI; Version " << version << "\n"
                                  << "Maximum render depth of this binary is " << MAXDEPTH << " dimensions.\n";
                        return 0;
                    }
                    else if (arg == "--model")
                    {
                        i++;
                        if (i < argc)
                        {
                            model = argv[i];
                        }
                    }
                    else if (arg == "--depth")
                    {
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            st >> depth;
                        }
                    }
                    else if (arg == "--render-depth")
                    {
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            st >> rdepth;
                        }
                    }
                    else if (arg == "--polar")
                    {
                        topologicState.state<FP,2>::polarCoordinates = true;
                    }
                    else if (arg == "--cartesian")
                    {
                        topologicState.state<FP,2>::polarCoordinates = false;
                    }
                    else if (arg == "--from")
                    {
                        i++;
                        if (i < argc)
                        {
                            int d = 0;
                            std::stringstream st (argv[i]);
                            st >> d;
                            st.clear();

                            for (int j = 0; j < d; j++)
                            {
                                double value;
                                i++;
                                if (i >= argc)
                                {
                                    break;
                                }
                                st.str(argv[i]);
                                st >> value;
                                st.clear();
                                if (topologicState.state<FP,2>::polarCoordinates)
                                {
                                    setPolar(topologicState, d, j, FP(value));
                                }
                                else
                                {
                                    setCartesian(topologicState, d, j, FP(value));
                                }
                            }
                        }
                    }
                    else try
                    {
                        std::ifstream in(arg);
                        std::istreambuf_iterator<char> eos;
                        std::string s(std::istreambuf_iterator<char>(in), eos);
    
                        xml::parser p = xml.parse(s, arg);
                        parse (topologicState, p);
                        parseModel (topologicState, p);
                        if (topologicState.model)
                        {
                            model = topologicState.model->id();
                            depth = topologicState.model->depth();
                            rdepth = topologicState.model->renderDepth();
                        }
                    }
                    catch (std::exception &e)
                    {
                        std::cerr << "Exception: " << e.what() << "\n";
                    }
                }
            }

            if (!topologicState.model)
            {
                parseModelWithTypeStringParameters (topologicState, model, depth, rdepth);
            }

            if (!topologicState.model)
            {
                std::cerr << "error: no model to render\n";
            }
            else
            {
                std::cout << (*topologicState.model)(true).str();
            }
        }
        catch (std::exception &e)
        {
            std::cerr << "Exception: " << e.what() << "\n";
        }
        catch (...)
        {
            std::cerr << "Unknown Exception\n";
        }

        return 0;
    }
};

#endif
