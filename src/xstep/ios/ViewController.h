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
 * This file is part of the Topologic project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic
 * \see Project Source Code: https://github.com/ef-gy/topologic
 * \see Licence Terms: https://github.com/ef-gy/topologic/blob/master/COPYING
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
