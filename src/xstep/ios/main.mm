/**\ingroup topologic-frontend
 * \defgroup frontend-cocoa-touch iOS/Cocoa Touch frontend
 * \brief Native Topologic frontend for iOS
 *
 * The native frontend for iOS devices - iPods, iPhones, iPads. This is a
 * universal application with the user interface designed as an Xcode 5 story
 * board.
 *
 * \see https://itunes.apple.com/us/app/topologic/id737310913 - this frontend on
 *      the iOS App Store. If you like this project then it'd be totally rad if
 *      you considered buying it.
 *
 * \{
 */

/**\file
 * \brief Topologic/iOS main function
 *
 * Contains the default iOS/UIKit main function for the Topologic/iOS frontend.
 *
 * \copyright
 * This file is part of the Topologic project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic
 * \see Project Source Code: https://github.com/ef-gy/topologic
 * \see Licence Terms: https://github.com/ef-gy/topologic/blob/master/COPYING
 */

#import <UIKit/UIKit.h>

#import "iOSAppDelegate.h"

/**\brief iOS main function stub
 *
 * This function simply calls the UIApplicationMain() function with the correct
 * parameters to get the iOS application up and running.
 *
 * \param[in] argc The number of arguments in argv.
 * \param[in] argv The command line arguments for the programme.
 *
 * \returns Presumably zero if the application terminated properly.
 */
int main(int argc, char * argv[])
{
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([iOSAppDelegate class]));
    }
}

/** \} */
