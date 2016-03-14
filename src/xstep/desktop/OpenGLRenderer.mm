/**\ingroup frontend-cocoa
 * \file
 * \brief OpenGL renderer view implementation
 *
 * Implements the OpenGLRenderer class, which is derived from an NSOpenGLView
 * and provides the basis for the OSX frontend.
 *
 * \copyright
 * This file is part of the Topologic project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic
 * \see Project Source Code: https://github.com/ef-gy/topologic
 * \see Licence Terms: https://github.com/ef-gy/topologic/blob/master/COPYING
 */

#import "OpenGLRenderer.h"
#import "OSXAppDelegate.h"

@implementation OpenGLRenderer

- (id) initWithFrame: (NSRect) frame
{
  NSOpenGLPixelFormatAttribute attrs[] =
  {
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFADepthSize, 24,
    // Must specify the 3.2 Core Profile to use OpenGL 3.2
    NSOpenGLPFAOpenGLProfile,
    NSOpenGLProfileVersion3_2Core,
    0
  };
  
  NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
  
  if (!pf)
  {
    NSLog(@"No OpenGL pixel format");
  }
  
  return self = [super initWithFrame:frame pixelFormat:pf];
}

- (void)prepareOpenGL
{
  [super prepareOpenGL];
  
  [[self openGLContext] makeCurrentContext];
  
  glEnable (GL_BLEND);
  
  glDisable(GL_CULL_FACE);

  GLint swapInt = 1;
  [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
}

- (void) drawRect:(NSRect)dirtyRect
{
  [[self openGLContext] makeCurrentContext];
  [(OSXAppDelegate*)[NSApp delegate] state]->width  = [self bounds].size.width;
  [(OSXAppDelegate*)[NSApp delegate] state]->height = [self bounds].size.height;
  
  if ([(OSXAppDelegate*)[NSApp delegate] state]->model)
  {
    [(OSXAppDelegate*)[NSApp delegate] state]->model->opengl(true);
  }
  
  [[self openGLContext] flushBuffer];
}

- (BOOL) isOpaque
{
  return YES;
}

- (void) mouseDragged:(NSEvent*)event
{
  [(OSXAppDelegate*)[NSApp delegate] state]->interpretDrag([event deltaX], [event deltaY], 0);
  [self setNeedsDisplay:YES];
}

- (void) magnifyWithEvent:(NSEvent *)event
{
  [(OSXAppDelegate*)[NSApp delegate] state]->interpretDrag(0, 0, [event magnification]*50.);
  [self setNeedsDisplay:YES];
}

@end
