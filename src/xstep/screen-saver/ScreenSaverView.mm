//
//  Topologic_Screen_SaverView.m
//  Topologic Screen Saver
//
//  Created by Magnus Deininger on 3/15/16.
//  Copyright © 2016 ef.gy. All rights reserved.
//

#import "ScreenSaverView.h"

@implementation Topologic_Screen_SaverView

- (instancetype)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
  NSLog( @"initWithFrame..." );

  self = [super initWithFrame:frame isPreview:isPreview];

  if (self) {
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

    glView = [[NSOpenGLView alloc] initWithFrame:NSZeroRect pixelFormat:pf];
    
    if (!glView)
    {
      NSLog( @"Couldn't initialize OpenGL view." );
      return nil;
    }
    
    [self addSubview:glView];
    [self setUpOpenGL];

    state = new topologic::state<GLfloat, MAXDEPTH>();

    std::srand(std::time(0));

    state->parameter.radius = 10;
    state->parameter.precision = 30;
    state->parameter.iterations = 2;
    state->parameter.functions = 3;
    state->parameter.seed = std::rand();
    state->fractalFlameColouring = true;

    efgy::geometry::with<GLfloat,topologic::updateModel,MAXDEPTH>(
        *state, "cartesian", "random-flame", 4, 4);

    [self setAnimationTimeInterval:1/30.0];
  }
  
  NSLog( @"initWithFrame done" );

  return self;
}

- (void)setUpOpenGL
{
  NSLog( @"setUpOpenGL..." );

  [[glView openGLContext] makeCurrentContext];

  glEnable (GL_BLEND);
  glDisable(GL_CULL_FACE);
  
  GLint swapInt = 1;
  [[glView openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
}

- (void)dealloc
{
  NSLog( @"dealloc..." );

  [glView removeFromSuperview];

  delete state;
  state = 0;
}

- (void)setFrameSize:(NSSize)newSize
{
  NSLog( @"setFrameSize..." );

  [super setFrameSize:newSize];
  [glView setFrameSize:newSize];
  
  [[glView openGLContext] makeCurrentContext];

  // forward the new size to Topologic here
  state->width  = newSize.width;
  state->height = newSize.height;

  [[glView openGLContext] update];
}

- (void)startAnimation
{
  NSLog( @"startAnimation..." );

  [super startAnimation];
}

- (void)stopAnimation
{
  NSLog( @"stopAnimation..." );

  [super stopAnimation];
}

- (void)doDraw
{
  [[glView openGLContext] makeCurrentContext];

  if (state->model)
  {
    state->model->opengl(true);
  }
  
  [[glView openGLContext] flushBuffer];
}

- (void)drawRect:(NSRect)rect
{
  NSLog( @"drawRect..." );

  [super drawRect:rect];

  [self doDraw];
}

- (void)animateOneFrame
{
  state->setActive(4);
  state->interpretDrag(-2, 0, 0);
  state->setActive(3);
  state->interpretDrag(0, 1, 0);

  // this isn't working for some reason...
  [self setNeedsDisplay:YES];

  [self doDraw];

  return;
}

- (BOOL)hasConfigureSheet
{
  return NO;
}

- (NSWindow*)configureSheet
{
  return nil;
}

@end
