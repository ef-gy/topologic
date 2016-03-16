//
//  Topologic_Screen_SaverView.h
//  Topologic Screen Saver
//
//  Created by Magnus Deininger on 3/15/16.
//  Copyright © 2016 ef.gy. All rights reserved.
//

#import <ScreenSaver/ScreenSaver.h>
#import <Cocoa/Cocoa.h>
#import <GLKit/GLKit.h>

#include <topologic/arguments.h>

#define MAXDEPTH 7

@interface Topologic_Screen_SaverView : ScreenSaverView
{
  NSOpenGLView *glView;
  
  topologic::state<GLfloat, MAXDEPTH> *state;
}

@end
