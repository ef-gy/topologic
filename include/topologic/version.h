/**\file
 * \brief Version information
 *
 * Contains Topologic's version number and assorted pieces of documentation
 * that didn't quite fit in anywhere else.
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
 * \see Project Source Code: https://github.com/ef-gy/topologic.git
 */

#if !defined(TOPOLOGIC_VERSION_H)
#define TOPOLOGIC_VERSION_H

/**\dir include/topologic
 * \brief Topologic template code
 *
 * Like libefgy, Topologic is mostly just a set of C++ template headers; as
 * such, most of the code is contained within the headers themselves, and these
 * headers are located in the include/topologic directory. And just like
 * libefgy, if you intend to create your own frontend then all you need to do
 * is copy or symlink this directory in your own source code repository.
 */

/**\defgroup topologic-frontend Topologic Frontends
 * \brief Programmes that produce actual output
 *
 * At its core, Topologic is more of a library than a single programme. The
 * distribution archives do, however, contain several "frontends" to this
 * library, each with unique capabilities, ranging from very simple command
 * line frontends to fairly complex WebGL clients.
 */

/**\brief Library namespace for the Topologic project
 *
 * This namespace contains all of the classes and functions that make up the
 * actual backend code of Topologic. The frontends then only really need to set
 * up an OpenGL context (if they do render to OpenGL) and handle user I/O.
 */
namespace topologic
{
    /**\brief Library Version
     *
     * This is the library's version number. Topologic uses a very simple,
     * single-integer versioning scheme: whenever a new version is released,
     * this number is increased by one and used until the next release.
     */
    static const unsigned int version = 9;
};

#endif
