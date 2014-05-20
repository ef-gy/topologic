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
