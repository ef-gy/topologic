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

@interface OSXAppDelegate : NSObject <NSApplicationDelegate>
{
    OpenGLRenderer IBOutlet *openGL;
    NSSegmentedControl IBOutlet *subtypeSegmentedControl;

    BOOL drawerVisible;
    BOOL modelViewDrawerVisible;

    NSInteger drawerMode;
    NSInteger activeCamera;
    NSInteger activeCameraType;

    NSColor *colourBackground;
    NSColor *colourWire;
    NSColor *colourSurface;
    NSColor *colourLighting;

    double cameraActiveCoordinate0;
    double cameraActiveCoordinate1;
    double cameraActiveCoordinate2;
    double cameraActiveCoordinate3;
    double cameraActiveCoordinate4;
    double cameraActiveCoordinate5;
    double cameraActiveCoordinate6;

    bool cameraActiveCoordinate0Enabled;
    bool cameraActiveCoordinate1Enabled;
    bool cameraActiveCoordinate2Enabled;
    bool cameraActiveCoordinate3Enabled;
    bool cameraActiveCoordinate4Enabled;
    bool cameraActiveCoordinate5Enabled;
    bool cameraActiveCoordinate6Enabled;

    NSString *cameraActiveCoordinate0Label;
    NSString *cameraActiveCoordinate1Label;
    NSString *cameraActiveCoordinate2Label;
    NSString *cameraActiveCoordinate3Label;
    NSString *cameraActiveCoordinate4Label;
    NSString *cameraActiveCoordinate5Label;
    NSString *cameraActiveCoordinate6Label;

    NSString *selectedModelName;

    NSString *model;
    NSInteger modelDepth;
    NSInteger renderDepth;

    NSPopUpButton IBOutlet *models;
}

@property (readonly) topologic::state<GLfloat,MAXDEPTH> *state;

@property (assign) NSMutableArray *outputTemplates;
@property (assign) IBOutlet NSWindow *window;

@property (retain,readwrite) OpenGLRenderer *openGL;
@property (retain,readwrite) NSSegmentedControl *subtypeSegmentedControl;

@property (readwrite) BOOL fractalFlameColouring;
@property (readwrite) BOOL drawerVisible;
@property (readwrite) BOOL modelViewDrawerVisible;

@property (readwrite) NSInteger drawerMode;
@property (readwrite) NSInteger activeCamera;
@property (readwrite) NSInteger activeCameraType;

@property (retain,readwrite) NSColor *colourBackground;
@property (retain,readwrite) NSColor *colourWire;
@property (retain,readwrite) NSColor *colourSurface;
@property (retain,readwrite) NSColor *colourLighting;

@property (readwrite) double cameraActiveCoordinate0;
@property (readwrite) double cameraActiveCoordinate1;
@property (readwrite) double cameraActiveCoordinate2;
@property (readwrite) double cameraActiveCoordinate3;
@property (readwrite) double cameraActiveCoordinate4;
@property (readwrite) double cameraActiveCoordinate5;
@property (readwrite) double cameraActiveCoordinate6;

@property (readwrite) bool cameraActiveCoordinate0Enabled;
@property (readwrite) bool cameraActiveCoordinate1Enabled;
@property (readwrite) bool cameraActiveCoordinate2Enabled;
@property (readwrite) bool cameraActiveCoordinate3Enabled;
@property (readwrite) bool cameraActiveCoordinate4Enabled;
@property (readwrite) bool cameraActiveCoordinate5Enabled;
@property (readwrite) bool cameraActiveCoordinate6Enabled;

@property (retain,readwrite) NSString *cameraActiveCoordinate0Label;
@property (retain,readwrite) NSString *cameraActiveCoordinate1Label;
@property (retain,readwrite) NSString *cameraActiveCoordinate2Label;
@property (retain,readwrite) NSString *cameraActiveCoordinate3Label;
@property (retain,readwrite) NSString *cameraActiveCoordinate4Label;
@property (retain,readwrite) NSString *cameraActiveCoordinate5Label;
@property (retain,readwrite) NSString *cameraActiveCoordinate6Label;

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
