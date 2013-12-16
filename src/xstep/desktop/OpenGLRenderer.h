//
//  OpenGLRenderer.h
//  Topologic
//
//  Created by Magnus Deininger on 06/07/2012.
//  Copyright (c) 2012, 2013 Magnus Deininger. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <GLKit/GLKit.h>

@interface OpenGLRenderer : NSOpenGLView
{
}

- (void)prepareOpenGL;

- (void) drawRect:(NSRect)dirtyRect;
- (BOOL) isOpaque;
- (void) mouseDragged:(NSEvent*)event;
- (void) magnifyWithEvent:(NSEvent *)event;

@end
