/**\file
 * \brief OSX Application Delegate interface
 *
 * Contains the interface for the NSApplicationDelegate subclass of the OSX
 * frontend.
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

#import <Cocoa/Cocoa.h>
#import <GLKit/GLKit.h>

#import "OpenGLRenderer.h"

#include <topologic/arguments.h>

#if !defined(MAXDEPTH)
#define MAXDEPTH 7
#endif

/**\brief OSX application delegate
 *
 * Contains the application delegate for the OSX version of Topologic, which
 * provides access to the C++ state object and a rather large number of
 * properties for use with Xcode's data bindings.
 */
@interface OSXAppDelegate : NSObject <NSApplicationDelegate>
{
    OpenGLRenderer IBOutlet *openGL;

    NSInteger drawerMode;
    NSInteger activeCamera;

    NSColor *colourBackground;
    NSColor *colourWire;
    NSColor *colourSurface;

    NSString *model;
    NSInteger modelDepth;
    NSInteger renderDepth;

    NSPopUpButton IBOutlet *models;
}

@property (readonly) topologic::state<GLfloat,MAXDEPTH> *state;

@property (assign) IBOutlet NSWindow *window;

@property (retain,readwrite) OpenGLRenderer *openGL;

@property (readwrite) BOOL fractalFlameColouring;
@property (readonly) BOOL drawerVisible;
@property (readonly) BOOL modelViewDrawerVisible;

@property (readwrite) NSInteger drawerMode;
@property (readwrite) NSInteger activeCamera;
@property (readwrite) NSInteger activeCameraType;

@property (retain,readwrite) NSColor *colourBackground;
@property (retain,readwrite) NSColor *colourWire;
@property (retain,readwrite) NSColor *colourSurface;

@property (readwrite) double cameraActiveCoordinate0;
@property (readwrite) double cameraActiveCoordinate1;
@property (readwrite) double cameraActiveCoordinate2;
@property (readwrite) double cameraActiveCoordinate3;
@property (readwrite) double cameraActiveCoordinate4;
@property (readwrite) double cameraActiveCoordinate5;
@property (readwrite) double cameraActiveCoordinate6;

@property (readonly) BOOL cameraActiveCoordinate0Enabled;
@property (readonly) BOOL cameraActiveCoordinate1Enabled;
@property (readonly) BOOL cameraActiveCoordinate2Enabled;
@property (readonly) BOOL cameraActiveCoordinate3Enabled;
@property (readonly) BOOL cameraActiveCoordinate4Enabled;
@property (readonly) BOOL cameraActiveCoordinate5Enabled;
@property (readonly) BOOL cameraActiveCoordinate6Enabled;

@property (retain,readonly) NSString *cameraActiveCoordinate0Label;
@property (retain,readonly) NSString *cameraActiveCoordinate1Label;
@property (retain,readonly) NSString *cameraActiveCoordinate2Label;
@property (retain,readonly) NSString *cameraActiveCoordinate3Label;
@property (retain,readonly) NSString *cameraActiveCoordinate4Label;
@property (retain,readonly) NSString *cameraActiveCoordinate5Label;
@property (retain,readonly) NSString *cameraActiveCoordinate6Label;

@property (readwrite) NSInteger modelDepth;
@property (readwrite) NSInteger renderDepth;

@property (readwrite) double polarRadius;
@property (readwrite) double polarPrecision;
@property (readwrite) NSInteger IFSIterations;
@property (readwrite) NSInteger IFSFunctions;
@property (readwrite) NSInteger IFSSeed;
@property (readwrite) NSInteger FlameVariants;
@property (readwrite) BOOL IFSPreRotate;
@property (readwrite) BOOL IFSPostRotate;

@property (retain,readwrite) NSString *model;
@property (retain,readwrite) NSString *selectedModelName;

@property (retain,readwrite) IBOutlet NSPopUpButton *models;

- (BOOL)openFile:(NSString*)fullPath;
- (IBAction)openDocument:(id)sender;
- (IBAction)saveDocumentAs:(id)sender;
- (IBAction)reset1:(id)sender;
- (IBAction)resetPIHalf:(id)sender;
- (IBAction)randomFlameColours:(id)sender;
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication;
- (IBAction)goToWebsite:(id)sender;
- (IBAction)updateOpenGLView:(id)sender;
- (void) saveFile:(NSString*)fileName;
- (void) updateModel;
- (void) updateModelParameters;

@end
