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

#if !defined(TOPOLOGIC_ARGUMENTS_H)
#define TOPOLOGIC_ARGUMENTS_H

#include <iostream>
#include <fstream>

#include <ef.gy/primitive.h>
#include <ef.gy/polytope.h>
#include <ef.gy/parametric.h>

#include <topologic/state.h>
#include <topologic/parse.h>
#include <topologic/version.h>

namespace topologic
{
    template<typename FP, unsigned int dim>
    bool parseArguments (state<FP, dim> &topologicState, int argc, char* argv[])
    {
        try
        {
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
                                  << argv[0] << " will render an SVG of a specified geometric primitive to stdout.\n\n"
                                     "Options\n"
                                     "  --help               Show this help text, then exit.\n"
                                     "  --version            Show version information, then exit.\n"
                                     "\n"
                                     "  --depth N            Set model depth to N\n"
                                     "  --render-depth N     Set render depth to N\n"
                                     "  --model M            Set model to M (cube, sphere, ...)\n"
                                     "  --precision F        Set model precision to F\n"
                                     "  --multiplier F       Set model export precision multiplier to F\n"
                                     "  --radius F           Set model radius parameter to F\n"
                                     "\n"
                                     "  --background R G B A Set background colour to R G B A (use values between 0 and 1)\n"
                                     "  --wireframe R G B A  Set wireframe colour to R G B A (use values between 0 and 1)\n"
                                     "  --surface R G B A    Set surface colour to R G B A (use values between 0 and 1)\n"
                                     "\n"
                                     "  --polar              Use/manipulate polar coordinates (default)\n"
                                     "  --cartesian          Use/manipulate cartesian coordinates\n"
                                     "  --from D X Y ...     Set the from-point of the D-to-D-1 projection to X Y ...\n"
                                     "\n"
                                     "  --id-prefix S        Use the prefix S for any IDs defined in the SVG\n"
                                     "\n"
                                     "See the man page of this programme for further details; e.g. run:\n"
                                     "man topologic\n\n";
                        return 0;
                    }
                    else if (arg == "--version")
                    {
                        std::cout << "Topologic CLI; Version " << version << "\n"
                                     "Maximum render depth of this binary is " << dim << " dimensions.\n"
                                     "Supported models: axe-graph, simplex, cube, sphere, moebius-strip, klein-bagel.\n";
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
                    else if (arg == "--precision")
                    {
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::parameter.polarPrecision = FP(dv);
                        }
                    }
                    else if (arg == "--radius")
                    {
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::parameter.polarRadius = FP(dv);
                        }
                    }
                    else if (arg == "--multiplier")
                    {
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::exportMultiplier = FP(dv);
                        }
                    }
                    else if (arg == "--background")
                    {
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::background.red = FP(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::background.green = FP(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::background.blue = FP(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::background.alpha = FP(dv);
                        }
                    }
                    else if (arg == "--surface")
                    {
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::surface.red = FP(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::surface.green = FP(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::surface.blue = FP(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::surface.alpha = FP(dv);
                        }
                    }
                    else if (arg == "--wireframe")
                    {
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::wireframe.red = FP(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::wireframe.green = FP(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::wireframe.blue = FP(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<FP,2>::wireframe.alpha = FP(dv);
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
                    else if (arg == "--id-prefix")
                    {
                        i++;
                        if (i < argc)
                        {
                            topologicState.state<FP,2>::idPrefix = std::string(argv[i]);
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
        }
        catch (std::exception &e)
        {
            std::cerr << "Exception: " << e.what() << "\n";
            return false;
        }
        catch (...)
        {
            std::cerr << "Unknown Exception\n";
            return false;
        }

        return true;
    }
};

#endif
