/**\file
 * \brief Basic CLI frontend
 *
 * Contains the basic Topologic CLI frontend, which is limited to processing
 * SVG files. It does demonstrate rather succinctly how to use the library,
 * however, and creating SVGs from the command line is always a neat feature.
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

#if !defined(TOPOLOGIC_CLI_H)
#define TOPOLOGIC_CLI_H

/**!\brief Disable OpenGL renderer
 *
 * OpenGL requires the created code to be linked to the system's OpenGL
 * libraries; this macro disables OpenGL support, and in doing so it allows the
 * resulting programme to be used in environments that do not support OpenGL.
 */
#define NO_OPENGL

#include <topologic/arguments.h>

#if !defined(MAXDEPTH)
/**\!brief Maximum render depth
 *
 * This macro is used by some of the frontends to determine the maximum render
 * depth supported by a frontend. The default value is '7', which is plenty for
 * most applications - increasing this value will increase the size of the
 * generated code, so it may be desirable to decrease this value in
 * environments with tighter constraints.
 */
#define MAXDEPTH 7
#endif

namespace topologic
{
    /**\brief Default floating point type
     *
     * This is an alias for a default floating point type that is compatible
     * with the SVG renderer. Note the use of efgy::math::primitive: this is
     * necessary, as some parts of libefgy's code expect to get a proper class
     * to work with.
     */
    typedef efgy::math::primitive<double> FP;

    /**\brief Default CLI frontend main function
     *
     * Main function for a typical CLI-/SVG-only frontend. This is part of the
     * library code so that it's easy to reuse where applicable.
     *
     * \tparam FP Floating point data type to use; something like topologic::FP
     *
     * \param[in] argc The number of arguments that are being passed in argv.
     * \param[in] argv The actual argument vector. The first element must be
     *                 the name the programme was called as, the remainder are
     *                 command line flags.
     *
     * \returns 0 if the function ran correctly, nonzero otherwise.
     */
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
