/**\ingroup frontend-cocoa-touch
 *
 * \{
 */

/**\file
 * \brief iOS Application Delegate header
 *
 * Contains the interface for the UIApplicationDelegate subclass of the iOS
 * frontend.
 *
 * \copyright
 * Created by Magnus Deininger on 11/10/2013.
 * Copyright (c) 2012-2014, Topologic Project Members
 */

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#define glGenVertexArrays(i, j)    glGenVertexArraysOES(i, j)
#define glBindVertexArray(i)       glBindVertexArrayOES(i)
#define glDeleteVertexArrays(i, j) glDeleteVertexArraysOES(i, j)

#include <topologic/arguments.h>

#if !defined(MAXDEPTH)
/**\brief Maximum render depth
 *
 * This macro is used by some of the frontends to determine the maximum render
 * depth supported by a frontend. The default value is '7', which is plenty for
 * most applications - increasing this value will increase the size of the
 * generated code, so it may be desirable to decrease this value in
 * environments with tighter constraints.
 */
#define MAXDEPTH 7
#endif

/**\brief iOS Application Delegate
 *
 * This is the application delegate class for the Topologic/iOS frontend. It
 * takes care of programme setup tasks, handles the Topologic state object and
 * keeps track of application setting chanes.
 */
@interface iOSAppDelegate : UIResponder <UIApplicationDelegate>
{
}

/**\brief Topologic state object
 *
 * The OpenGL renderer in particular needs direct access to the state object to
 * make it draw meshes to the current OpenGL context, which is why this property
 * is available via the Application Delegate to access that state object.
 */
@property (readonly) topologic::state<GLfloat,MAXDEPTH> *state;

/**\brief Application window
 *
 * Used to access the application's window.
 */
@property (strong, nonatomic) UIWindow *window;

/**\copydoc OSXAppDelegate::updateModel */
- (void) updateModel;

/**\copydoc OSXAppDelegate::updateModelParameters */
- (void) updateModelParameters;

/**\brief Reconfigure with user defaults
 *
 * Uses the stored user defaults to set the current application settings.
 */
- (void) reconfigure;

@end

/** \} */