//
//  ViewController.h
//  Topologic-iOS
//
//  Created by Magnus Deininger on 11/10/2013.
//  Copyright (c) 2013 ef.gy. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

@interface ViewController : GLKViewController

- (IBAction)handlePan:(UIPanGestureRecognizer *)recognizer;
- (IBAction)handlePinch:(UIPinchGestureRecognizer *)recognizer;

@end
