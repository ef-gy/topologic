/**\file
 * \brief OpenGL renderer view implementation
 *
 * Implements the OpenGLRenderer class, which is derived from an NSOpenGLView
 * and provides the basis for the OSX frontend.
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
 * \see Project Source Code: http://git.becquerel.org/jyujin/topologic.git
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
	
	NSOpenGLPixelFormat *pf = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attrs] autorelease];
	
	if (!pf)
	{
		NSLog(@"No OpenGL pixel format");
	}

	return self = [super initWithFrame:frame pixelFormat: [pf autorelease]];
}

- (void)prepareOpenGL
{
	[super prepareOpenGL];

    [[self openGLContext] makeCurrentContext];

    glEnable (GL_BLEND);

    glEnable(GL_CULL_FACE);

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
        [(OSXAppDelegate*)[NSApp delegate] state]->model->render(true);
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
