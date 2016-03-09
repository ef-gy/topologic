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
 * This file is part of the Topologic project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic
 * \see Project Source Code: https://github.com/ef-gy/topologic
 * \see Licence Terms: https://github.com/ef-gy/topologic/blob/master/COPYING
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
