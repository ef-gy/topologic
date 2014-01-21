/**\ingroup frontend-cocoa-touch
 * \file
 * \brief iOS view controller implementation
 *
 * Implements the view controller for the iOS frontend.
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
    
    glEnable(GL_CULL_FACE);
}

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
