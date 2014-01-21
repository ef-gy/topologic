/**\ingroup frontend-cocoa
 * \file
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

/**\ingroup frontend-cocoa
 * \brief OSX application delegate
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
@property (retain,readwrite) IBOutlet NSPopUpButton *models;

@property (retain,readwrite) OpenGLRenderer *openGL;

@property (readwrite) BOOL fractalFlameColouring;
@property (readonly) BOOL drawerVisible;
@property (readonly) BOOL modelViewDrawerVisible;

@property (readwrite) NSInteger drawerMode;

/**\ingroup frontend-cocoa
 * \defgroup osx-data-bindings Data bindings
 * \brief Properties for XIB bindings
 *
 * Properties that make it easier to access parts of the programme state with
 * Xcode's Interface Builder bindings. A lot of these could be simple functions,
 * and usually they really only delegate to functions of the state object, but
 * binding them to XIB components means we have to provide a simple property for
 * them and we can't use functions that take an argument.
 *
 * All of these properties are exposed by the application delegate,
 * \ref OSXAppDelegate.
 *
 * \{
 */

/**\defgroup osx-data-bindings-camera Camera position
 * \brief Properties related to the current camera state
 *
 * These properties describe the current camera state and allow manipulation of
 * the camera's position.
 *
 * \{
 */

/**\brief Active camera
 *
 * Used to query and set which camera should be manipulated with the UI controls
 * right now. For example, setting this to 0 will allow you to control the 3D
 * camera, setting this to 1 will allow you to control the 4D camera and so on.
 */
@property (readwrite) NSInteger activeCamera;

/**\brief Coordinate system for active camera
 *
 * Used to query and set the currently active camera coordinate system. Possible
 * values are:
 *
 *  - 0 for cartesian coordinates
 *  - 1 for polar coordinates
 */
@property (readwrite) NSInteger activeCameraType;

/**\brief Active camera coordinate 0
 *
 * Used to query and set the currently active camera's 0th coordinate. This
 * really should be a function that just takes the coordinate as input, but then
 * Data bindings in the Interface Builder won't work.
 */
@property (readwrite) double cameraActiveCoordinate0;

/**\brief Active camera coordinate 1
 *
 * Used to query and set the currently active camera's 1st coordinate. This
 * really should be a function that just takes the coordinate as input, but then
 * Data bindings in the Interface Builder won't work.
 */
@property (readwrite) double cameraActiveCoordinate1;

/**\brief Active camera coordinate 2
 *
 * Used to query and set the currently active camera's 2nd coordinate. This
 * really should be a function that just takes the coordinate as input, but then
 * Data bindings in the Interface Builder won't work.
 */
@property (readwrite) double cameraActiveCoordinate2;

/**\brief Active camera coordinate 3
 *
 * Used to query and set the currently active camera's 3rd coordinate. This
 * really should be a function that just takes the coordinate as input, but then
 * Data bindings in the Interface Builder won't work.
 */
@property (readwrite) double cameraActiveCoordinate3;

/**\brief Active camera coordinate 4
 *
 * Used to query and set the currently active camera's 4th coordinate. This
 * really should be a function that just takes the coordinate as input, but then
 * Data bindings in the Interface Builder won't work.
 */
@property (readwrite) double cameraActiveCoordinate4;

/**\brief Active camera coordinate 5
 *
 * Used to query and set the currently active camera's 5th coordinate. This
 * really should be a function that just takes the coordinate as input, but then
 * Data bindings in the Interface Builder won't work.
 */
@property (readwrite) double cameraActiveCoordinate5;

/**\brief Active camera coordinate 6
 *
 * Used to query and set the currently active camera's 5th coordinate. This
 * really should be a function that just takes the coordinate as input, but then
 * Data bindings in the Interface Builder won't work.
 */
@property (readwrite) double cameraActiveCoordinate6;

/**\brief Camera coordinate input 0 enabled?
 *
 * 'YES' when the 0th camera coordinate should currently be editable. This
 * should probably be a single function instead of seven, but then we couldn't
 * access the properties using Interface Builder.
 */
@property (readonly) BOOL cameraActiveCoordinate0Enabled;

/**\brief Camera coordinate input 1 enabled?
 *
 * 'YES' when the 1st camera coordinate should currently be editable. This
 * should probably be a single function instead of seven, but then we couldn't
 * access the properties using Interface Builder.
 */
@property (readonly) BOOL cameraActiveCoordinate1Enabled;

/**\brief Camera coordinate input 2 enabled?
 *
 * 'YES' when the 2nd camera coordinate should currently be editable. This
 * should probably be a single function instead of seven, but then we couldn't
 * access the properties using Interface Builder.
 */
@property (readonly) BOOL cameraActiveCoordinate2Enabled;

/**\brief Camera coordinate input 3 enabled?
 *
 * 'YES' when the 3rd camera coordinate should currently be editable. This
 * should probably be a single function instead of seven, but then we couldn't
 * access the properties using Interface Builder.
 */
@property (readonly) BOOL cameraActiveCoordinate3Enabled;

/**\brief Camera coordinate input 4 enabled?
 *
 * 'YES' when the 4th camera coordinate should currently be editable. This
 * should probably be a single function instead of seven, but then we couldn't
 * access the properties using Interface Builder.
 */
@property (readonly) BOOL cameraActiveCoordinate4Enabled;

/**\brief Camera coordinate input 5 enabled?
 *
 * 'YES' when the 5th camera coordinate should currently be editable. This
 * should probably be a single function instead of seven, but then we couldn't
 * access the properties using Interface Builder.
 */
@property (readonly) BOOL cameraActiveCoordinate5Enabled;

/**\brief Camera coordinate input 6 enabled?
 *
 * 'YES' when the 6th camera coordinate should currently be editable. This
 * should probably be a single function instead of seven, but then we couldn't
 * access the properties using Interface Builder.
 */
@property (readonly) BOOL cameraActiveCoordinate6Enabled;

/**\brief UI label text for the 0th coordinate
 *
 * The label for the 0th coordinate that should be displayed next to its slider
 * in the interface. It'd be nice if this could be a single function, but then
 * Interface Builder bindings would no longer work.
 */
@property (retain,readonly) NSString *cameraActiveCoordinate0Label;

/**\brief UI label text for the 1st coordinate
 *
 * The label for the 1st coordinate that should be displayed next to its slider
 * in the interface. It'd be nice if this could be a single function, but then
 * Interface Builder bindings would no longer work.
 */
@property (retain,readonly) NSString *cameraActiveCoordinate1Label;

/**\brief UI label text for the 2nd coordinate
 *
 * The label for the 2nd coordinate that should be displayed next to its slider
 * in the interface. It'd be nice if this could be a single function, but then
 * Interface Builder bindings would no longer work.
 */
@property (retain,readonly) NSString *cameraActiveCoordinate2Label;

/**\brief UI label text for the 3rd coordinate
 *
 * The label for the 3rd coordinate that should be displayed next to its slider
 * in the interface. It'd be nice if this could be a single function, but then
 * Interface Builder bindings would no longer work.
 */
@property (retain,readonly) NSString *cameraActiveCoordinate3Label;

/**\brief UI label text for the 4th coordinate
 *
 * The label for the 4th coordinate that should be displayed next to its slider
 * in the interface. It'd be nice if this could be a single function, but then
 * Interface Builder bindings would no longer work.
 */
@property (retain,readonly) NSString *cameraActiveCoordinate4Label;

/**\brief UI label text for the 5th coordinate
 *
 * The label for the 5th coordinate that should be displayed next to its slider
 * in the interface. It'd be nice if this could be a single function, but then
 * Interface Builder bindings would no longer work.
 */
@property (retain,readonly) NSString *cameraActiveCoordinate5Label;

/**\brief UI label text for the 6th coordinate
 *
 * The label for the 6th coordinate that should be displayed next to its slider
 * in the interface. It'd be nice if this could be a single function, but then
 * Interface Builder bindings would no longer work.
 */
@property (retain,readonly) NSString *cameraActiveCoordinate6Label;

/** \} */

/**\defgroup osx-data-bindings-model Model selection
 * \brief Model name and dimensions
 *
 * Used to set the intended model parameters and query the currently active
 * ones; not all of these properties represent the current state, some retain
 * the intended target state instead.
 *
 * \{
 */

@property (readwrite) NSInteger modelDepth;
@property (readwrite) NSInteger renderDepth;
@property (retain,readwrite) NSString *model;
@property (retain,readwrite) NSString *selectedModelName;

/** \} */

/**\defgroup osx-data-bindings-parameters Model parameters
 * \brief Model parameters
 *
 * \{
 */

@property (readwrite) double polarRadius;
@property (readwrite) double polarPrecision;
@property (readwrite) NSInteger IFSIterations;
@property (readwrite) NSInteger IFSFunctions;
@property (readwrite) NSInteger IFSSeed;
@property (readwrite) NSInteger FlameVariants;
@property (readwrite) BOOL IFSPreRotate;
@property (readwrite) BOOL IFSPostRotate;

/** \} */

/**\defgroup osx-data-bindings-colours Colours
 * \brief Render colours
 *
 * These colours are used in the process of rendering the models with OpenGL or
 * when generating an SVG export.
 *
 * \{
 */

@property (retain,readwrite) NSColor *colourBackground;
@property (retain,readwrite) NSColor *colourWire;
@property (retain,readwrite) NSColor *colourSurface;

/** \} */

/** \} */

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
