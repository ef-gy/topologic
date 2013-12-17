//
//  ViewController.m
//  Topologic-iOS
//
//  Created by Magnus Deininger on 11/10/2013.
//  Copyright (c) 2013 ef.gy. All rights reserved.
//

#import "ViewController.h"

#include "iOSAppDelegate.h"

@interface ViewController () {
}

@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;

- (void)setupGL;
- (void)tearDownGL;
@end

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
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];

    glEnable (GL_BLEND);
    
    glEnable(GL_CULL_FACE);
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    [EAGLContext setCurrentContext:self.context];

    [(iOSAppDelegate*)[[UIApplication sharedApplication] delegate] state]->width  = rect.size.width  * [[UIScreen mainScreen] scale];
    [(iOSAppDelegate*)[[UIApplication sharedApplication] delegate] state]->height = rect.size.height * [[UIScreen mainScreen] scale];

    if ([(iOSAppDelegate*)[[UIApplication sharedApplication] delegate] state]->model)
    {
        [(iOSAppDelegate*)[[UIApplication sharedApplication] delegate] state]->model->render(true);
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
