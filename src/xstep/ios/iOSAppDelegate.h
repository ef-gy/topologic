/**\file
 * \brief iOS Application Delegate Header
 *
 * Contains the interface for the UIApplicationDelegate subclass for the iOS
 * frontend.
 *
 * \copyright
 * Created by Magnus Deininger on 11/10/2013.
 * Copyright (c) 2013 ef.gy. All rights reserved.
 */

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#define glGenVertexArrays(i, j)    glGenVertexArraysOES(i, j)
#define glBindVertexArray(i)       glBindVertexArrayOES(i)
#define glDeleteVertexArrays(i, j) glDeleteVertexArraysOES(i, j)

#include <topologic/gl.h>

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

@property (readonly) topologic::state<topologic::GLFP,MAXDEPTH> *state;
@property (strong, nonatomic) UIWindow *window;

- (void) updateModel;
- (void) updateModelParameters;
- (void) reconfigure;

@end
