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
@property (strong, nonatomic) UIWindow *window;

- (void) updateModel;
- (void) updateModelParameters;
- (void) reconfigure;

@end
