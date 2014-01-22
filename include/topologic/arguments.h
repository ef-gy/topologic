/**\file
 * \brief Command line argument parsing
 *
 * The different frontends tend to receive command line arguments and this
 * header provides the topologic::parseArguments function to parse said command
 * line arguments in a common way.
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

#if !defined(TOPOLOGIC_ARGUMENTS_H)
#define TOPOLOGIC_ARGUMENTS_H

#include <iostream>
#include <fstream>
#include <cmath>

#include <ef.gy/version.h>
#include <ef.gy/parametric.h>

#include <topologic/parse.h>
#include <topologic/version.h>

namespace topologic
{
    /**\brief Parse command line arguments
     *
     * A function template to parse C-style command line arguments, apply the
     * settings in those arguments to a topologic::state instance. This
     * function will also parse XML files that have been passed in as command
     * line arguments and set the model in the state object to match that
     * specified as command line arguments or in XML files.
     *
     * As usual, later options override earlier ones - that also applies to
     * settings in XML files. If the NOLIBRARIES macro is set then XML files
     * will not be processed.
     *
     * \tparam Q   Base data type as used in the topologic::state instance
     * \tparam dim Maximum render depth of the topologic::state instance
     *
     * \param[out] topologicState The topologic::state instance to populate
     * \param[in]  argc           Number of arguments in the argv array
     * \param[in]  argv           Command line argument array (c.f. main())
     * \param[in]  out            Output mode, e.g. topologic::outGL
     *
     * \returns 'true' if things went smoothly, 'false' otherwise.
     */
    template<typename Q, unsigned int dim>
    bool parseArguments (state<Q, dim> &topologicState, int argc, char* argv[], enum outputMode out)
    {
        try
        {
#if !defined (NOLIBRARIES)
            topologic::xml XML;
#endif
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
                                     "  --help                  Show this help text, then exit.\n"
                                     "  --version               Show version information, then exit.\n"
                                     "\n"
                                     "  --depth N               Set model depth to N\n"
                                     "  --render-depth N        Set render depth to N\n"
                                     "  --model M               Set model to M (cube, sphere, ...)\n"
                                     "\n"
                                     "  --iterations N          Set IFS iterations to N\n"
                                     "  --seed N                Set the RNG seed to N (e.g. for the random-affine-ifs model)\n"
                                     "  --functions N           Suggest that random IFSs should use N functions\n"
                                     "  --pre-rotation          Suggest that random IFSs should add a pre-translation rotation\n"
                                     "  --no-pre-rotation       Suggest that random IFSs should not add a pre-translation rotation\n"
                                     "  --post-rotation         Suggest that random IFSs should add a post-translation rotation\n"
                                     "  --no-post-rotation      Suggest that random IFSs should not add a post-translation rotation\n"
                                     "  --flame-variants N      Suggest that random fractal flames should use N variants\n"
                                     "\n"
                                     "  --precision F           Set model precision to F\n"
                                     "  --radius F              Set model radius parameter to F\n"
                                     "\n"
                                     "  --background R G B A    Set background colour to R G B A (use values between 0 and 1)\n"
                                     "  --wireframe R G B A     Set wireframe colour to R G B A (use values between 0 and 1)\n"
                                     "  --surface R G B A       Set surface colour to R G B A (use values between 0 and 1)\n"
                                     "\n"
                                     "  --polar                 Use/manipulate polar coordinates (default)\n"
                                     "  --cartesian             Use/manipulate cartesian coordinates\n"
                                     "  --from D X Y ...        Set the from-point of the D-to-D-1 projection to X Y ...\n"
                                     "  --transform D A B ...   Set the transformation matrix in D to A B ...\n"
                                     "\n"
                                     "  --regular-colours       Use regular colouring algorithm\n"
                                     "  --fractal-flame-colours Use fractal flame colouring algorithm (OpenGL only)\n"
                                     "\n"
                                     "See the man page of this programme for further details; e.g. run:\n"
                                     "man topologic\n\n";
                        return 0;
                    }
                    else if (arg == "--version")
                    {
                        std::cout << "Topologic/V" << version << "\n"
                                     "libefgy/V" << efgy::version << "\n"
                                     "Maximum render depth of this binary is " << dim << " dimensions.\n"
                                     "Supported models:";
                        std::set<const char *> models;
                        for (const char *m : efgy::geometry::with<Q,efgy::geometry::functor::models,dim>(models,"*",0,0))
                        {
                            std::cout << " " << m;
                        }
                        std::cout << "\n";
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
                    else if (arg == "--iterations")
                    {
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            st >> topologicState.state<Q,2>::parameter.iterations;
                        }
                    }
                    else if (arg == "--seed")
                    {
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            st >> topologicState.state<Q,2>::parameter.seed;
                        }
                    }
                    else if (arg == "--functions")
                    {
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            st >> topologicState.state<Q,2>::parameter.functions;
                        }
                    }
                    else if (arg == "--flame-variants")
                    {
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            st >> topologicState.state<Q,2>::parameter.flameCoefficients;
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
                            topologicState.state<Q,2>::parameter.polarPrecision = Q(dv);
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
                            topologicState.state<Q,2>::parameter.polarRadius = Q(dv);
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
                            topologicState.state<Q,2>::background.red = Q(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<Q,2>::background.green = Q(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<Q,2>::background.blue = Q(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<Q,2>::background.alpha = Q(dv);
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
                            topologicState.state<Q,2>::surface.red = Q(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<Q,2>::surface.green = Q(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<Q,2>::surface.blue = Q(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<Q,2>::surface.alpha = Q(dv);
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
                            topologicState.state<Q,2>::wireframe.red = Q(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<Q,2>::wireframe.green = Q(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<Q,2>::wireframe.blue = Q(dv);
                        }
                        i++;
                        if (i < argc)
                        {
                            std::stringstream st (argv[i]);
                            double dv;
                            st >> dv;
                            topologicState.state<Q,2>::wireframe.alpha = Q(dv);
                        }
                    }
                    else if (arg == "--polar")
                    {
                        topologicState.state<Q,2>::polarCoordinates = true;
                    }
                    else if (arg == "--cartesian")
                    {
                        topologicState.state<Q,2>::polarCoordinates = false;
                    }
                    else if (arg == "--pre-rotation")
                    {
                        topologicState.state<Q,2>::parameter.preRotate = true;
                    }
                    else if (arg == "--no-pre-rotation")
                    {
                        topologicState.state<Q,2>::parameter.preRotate = false;
                    }
                    else if (arg == "--post-rotation")
                    {
                        topologicState.state<Q,2>::parameter.postRotate = true;
                    }
                    else if (arg == "--no-post-rotation")
                    {
                        topologicState.state<Q,2>::parameter.postRotate = false;
                    }
                    else if (arg == "--regular-colours")
                    {
                        topologicState.state<Q,2>::fractalFlameColouring = false;
                    }
                    else if (arg == "--fractal-flame-colours")
                    {
                        topologicState.state<Q,2>::fractalFlameColouring = true;
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
                                topologicState.setFromCoordinate(j, Q(value), d);
                            }
                        }
                    }
                    else if (arg == "--transform")
                    {
                        i++;
                        if (i < argc)
                        {
                            int d = 0;
                            std::stringstream st (argv[i]);
                            st >> d;
                            st.clear();
                            
                            for (unsigned int x = 0; x <= d; x++)
                            {
                                for (unsigned int y = 0; y <= d; y++)
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
                                    setMatrixCell(topologicState, d, x, y, Q(value));
                                }
                                if (i >= argc)
                                {
                                    break;
                                }
                            }
                        }
                    }
#if !defined (NOLIBRARIES)
                    else try
                    {
                        std::ifstream in(argv[i]);
                        std::istreambuf_iterator<char> eos;
                        std::string s(std::istreambuf_iterator<char>(in), eos);
    
                        xml::parser p(s, arg);
                        parse (topologicState, p);
                        if (out == outSVG)
                        {
                            parseModel<Q,dim,updateModelSVG> (topologicState, p);
                        }
#if !defined(NO_OPENGL)
                        else if (out == outGL)
                        {
                            parseModel<Q,dim,updateModelOpenGL> (topologicState, p);
                        }
#endif
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
#endif
                }
            }

            if (!topologicState.model)
            {
                if (out == outSVG)
                {
                    efgy::geometry::with<Q,updateModelSVG,dim> (topologicState, model, depth, rdepth);
                }
#if !defined(NO_OPENGL)
                else if (out == outGL)
                {
                    efgy::geometry::with<Q,updateModelOpenGL,dim> (topologicState, model, depth, rdepth);
                }
#endif
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
