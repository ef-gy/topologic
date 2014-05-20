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
