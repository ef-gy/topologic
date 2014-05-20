/**\ingroup topologic-frontend
 * \defgroup frontend-cocoa OSX/Cocoa frontend
 * \brief Native Topologic frontend for OSX
 *
 * This is a native Topologic frontend using Cocoa for Mac OSX. Most of the code
 * is in the Topologic include files, so this is really just a fairly thin
 * Objective C wrapper around the C++ core.
 *
 * \see http://itunes.apple.com/us/app/topologic/id558300160 - this frontend on
 *      the Mac App Store. If you like this project then it'd be totally rad if
 *      you considered buying it.
 *
 * \{
 */

/**\file
 * \brief Topologic/OSX main function
 *
 * Contains the default OSX/Cocoa main function for the Topologic/OSX frontend.
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

#import <Cocoa/Cocoa.h>

/**\brief OSX main function stub
 *
 * This function simply calls the NSApplicationMain() function with the correct
 * parameters to get the OSX application up and running.
 *
 * \param[in] argc The number of arguments in argv.
 * \param[in] argv The command line arguments for the programme.
 *
 * \returns Presumably zero if the application terminated properly.
 */
int main(int argc, char *argv[])
{
    return NSApplicationMain(argc, (const char **)argv);
}

/** \} */
