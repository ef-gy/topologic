//
//  OpenGLRenderer.m
//  Topologic
//
//  Created by Magnus Deininger on 06/07/2012.
//  Copyright (c) 2012, 2013 Magnus Deininger. All rights reserved.
//

#import "OpenGLRenderer.h"
#import "AppDelegate.h"

@interface OpenGLRenderer () {
}
@end

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
    [(AppDelegate*)[NSApp delegate] state]->width  = [self bounds].size.width;
    [(AppDelegate*)[NSApp delegate] state]->height = [self bounds].size.height;

    if ([(AppDelegate*)[NSApp delegate] state]->model)
    {
        [(AppDelegate*)[NSApp delegate] state]->model->render(true);
    }

    [[self openGLContext] flushBuffer];
}

- (BOOL) isOpaque
{
    return YES;
}

- (void) mouseDragged:(NSEvent*)event
{
    [(AppDelegate*)[NSApp delegate] state]->interpretDrag([event deltaX], [event deltaY], 0);
    [self setNeedsDisplay:YES];
}

- (void) magnifyWithEvent:(NSEvent *)event
{
    [(AppDelegate*)[NSApp delegate] state]->interpretDrag(0, 0, [event magnification]*50.);
    [self setNeedsDisplay:YES];
}

@end
