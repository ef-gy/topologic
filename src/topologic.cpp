/**\ingroup topologic-frontend
 * \defgroup frontend-cli CLI frontend
 * \brief Basic command line Topologic frontend
 *
 * This is the very simplest frontend for Topologic, which doesn't do any OpenGL
 * magic at all but rather only provides a scriptable command line interface
 * that produces SVGs on stdout.
 *
 * \{
 */

/**\file
 * \brief Topologic/CLI frontend
 *
 * This is the simplest possible Topologic frontend: a command line application
 * that renders SVG files to stdout.
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

#include <topologic/cli.h>

/**\brief Topologic/CLI main function
 *
 * This is really just a stub that calls the topologic::cli function, which
 * contains the actual logic for the Topologic/CLI frontend.
 *
 * \param[in] argc The number of arguments in the argv array.
 * \param[in] argv The actual command line arguments passed to the programme.
 *
 * \returns 0 on success, nonzero otherwise.
 */
int main (int argc, char* argv[])
{
    return topologic::cli<double> (argc, argv);
}

/** \} */
