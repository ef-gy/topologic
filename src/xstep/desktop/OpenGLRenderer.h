/**\ingroup frontend-cocoa
 *
 * \{
 */

/**\file
 * \brief OpenGL renderer view interface
 *
 * Declares the OpenGLRenderer Objective-C class, which is based on NSOpenGLView
 * and is used to keep track of an OpenGL context for the OSX Topologic
 * frontend.
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
#import <GLKit/GLKit.h>

/**\brief OpenGL render view
 *
 * This is the OpenGL view used in the OSX frontend. It initialises an OpenGL
 * context so that it can be used with libefgy/Topologic and redraws the scene
 * when it becomes necessary. It also listens to certain mouse events that allow
 * simple manipulations of the scene.
 */
@interface OpenGLRenderer : NSOpenGLView
{
}

/**\brief Prepare OpenGL context
 *
 * Sets some global parameters that libefgy's code expects to be set.
 */
- (void)prepareOpenGL;

/**\brief Redraw
 *
 * Called by the runtime to tell us that we need to redraw parts of the OpenGL
 * context. Whenever this function is called we simply redraw the whole scene.
 */
- (void) drawRect:(NSRect)dirtyRect;

/**\brief Opaque view?
 *
 * Called by the runtime to query if the view is 'opaque'. Our view is, so we
 * always return 'YES'.
 *
 * \returns Always 'YES'.
 */
- (BOOL) isOpaque;

/**\brief Runtime callback: mouse was dragged
 *
 * Called by the runtime to indicate that the mouse was dragged. Whenever we
 * receive this event, we rotate the current view around the origin based on the
 * event data.
 */
- (void) mouseDragged:(NSEvent*)event;

/**\brief Runtime callback: window should be magnified
 *
 * Called by the runtime when the view should be 'magnified'. On OSX this occurs
 * when a user uses a two-finger pinch gesture on either a touch pad or a mouse
 * that supports gesture recognition. In response to this event, the object is
 * zoomed in or out.
 */
- (void) magnifyWithEvent:(NSEvent *)event;

@end

/** \} */
