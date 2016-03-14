/**\ingroup frontend-cocoa-touch
 * \file
 * \brief iOS view controller implementation
 *
 * Implements the view controller for the iOS frontend.
 *
 * \copyright
 * This file is part of the Topologic project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic
 * \see Project Source Code: https://github.com/ef-gy/topologic
 * \see Licence Terms: https://github.com/ef-gy/topologic/blob/master/COPYING
 */

#import "ViewController.h"

#include "iOSAppDelegate.h"

@implementation ViewController

- (void)viewDidLoad
{
  [super viewDidLoad];
  
  self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
  
  if (!self.context) {
    NSLog(@"Failed to create ES context");
  }
  
  GLKView *view = (GLKView *)self.view;
  view.context = self.context;
  view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
  
  [self setupGL];
}

- (void)dealloc
{
  if ([EAGLContext currentContext] == self.context) {
    [EAGLContext setCurrentContext:nil];
  }
}

- (void)setupGL
{
  [EAGLContext setCurrentContext:self.context];
  
  glEnable (GL_BLEND);
  
  glDisable(GL_CULL_FACE);
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
  [EAGLContext setCurrentContext:self.context];
  
  [(iOSAppDelegate*)[[UIApplication sharedApplication] delegate] state]->width  = rect.size.width  * [[UIScreen mainScreen] scale];
  [(iOSAppDelegate*)[[UIApplication sharedApplication] delegate] state]->height = rect.size.height * [[UIScreen mainScreen] scale];
  
  if ([(iOSAppDelegate*)[[UIApplication sharedApplication] delegate] state]->model)
  {
    [(iOSAppDelegate*)[[UIApplication sharedApplication] delegate] state]->model->opengl(true);
  }
  else
  {
    std::cerr << "no model to draw!\n";
  }
}

- (IBAction)handlePan:(UIPanGestureRecognizer *)recognizer
{
  CGPoint translation = [recognizer translationInView:self.view];
  
  [(iOSAppDelegate*)[[UIApplication sharedApplication] delegate] state]->setActive([recognizer numberOfTouches]+2);
  [(iOSAppDelegate*)[[UIApplication sharedApplication] delegate] state]->interpretDrag(translation.x,translation.y,0);
  
  [recognizer setTranslation:CGPointMake(0, 0) inView:self.view];
}

- (IBAction)handlePinch:(UIPinchGestureRecognizer *)recognizer
{
  [(iOSAppDelegate*)[[UIApplication sharedApplication] delegate] state]->setActive([recognizer numberOfTouches]+1);
  [(iOSAppDelegate*)[[UIApplication sharedApplication] delegate] state]->scale(recognizer.scale);
  
  recognizer.scale = 1;
}

@end
