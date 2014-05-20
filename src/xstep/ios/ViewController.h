/**\ingroup frontend-cocoa-touch
 *
 * \{
 */

/**\file
 * \brief iOS view controller interface
 *
 * Declares the view controller class used by the iOS frontend, which is in turn
 * based on a GLKViewController.
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
 * \see Project Source Code: https://github.com/ef-gy/topologic
 */

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

/**\brief iOS view controller
 *
 * Controls the applications main OpenGL ES context and issues render commands
 * whenever they're needed.
 */
@interface ViewController : GLKViewController

/**\brief OpenGL ES context
 *
 * This is the currently active OpenGL ES context.
 */
@property (strong, nonatomic) EAGLContext *context;

/**\brief OpenGL ES initialisation
 *
 * Initialises some of the OpenGL context parameters.
 */
- (void)setupGL;

/**\brief Touch pan event handler
 *
 * Called by the runtime to indicate that the user has panned the OpenGL ES
 * view; the pan gesture rotates the current model, the number of fingers used
 * to pan determine which dimension the model is rotated in.
 *
 * \param[in] recognizer Describes how the view was panned.
 */
- (IBAction)handlePan:(UIPanGestureRecognizer *)recognizer;

/**\brief Touch pinch event handler
 *
 * Called by the runtime to indicate that the user has pinched the OpenGL ES
 * view; the pinch gestures zooms in or out of the model.
 *
 * \param[in] recognizer Describes how the view was pinched.
 */
- (IBAction)handlePinch:(UIPinchGestureRecognizer *)recognizer;

@end

/** \} */
