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
/**\brief Maximum render depth
 *
 * This macro is used by some of the frontends to determine the maximum render
 * depth supported by a frontend. The default value is '7', which is plenty for
 * most applications - increasing this value will increase the size of the
 * generated code, so it may be desirable to decrease this value in
 * environments with tighter constraints.
 */
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
    /**\brief OpenGL renderer instance
     *
     * Local variable that holds a pointer to the OpenGL renderer; accessed
     * though the openGL property.
     */
    OpenGLRenderer IBOutlet *openGL;

    NSInteger drawerMode;
    NSInteger activeCamera;

/* the next three are probably not necessary and should be removed */
    NSColor *colourBackground;
    NSColor *colourWire;
    NSColor *colourSurface;

    NSString *model;
    NSInteger modelDepth;
    NSInteger renderDepth;

    /**\brief Model list instance
     *
     * This is where the XIB reference for the list of models is stored.
     */
    NSPopUpButton IBOutlet *models;
}

/**\brief Application window
 *
 * Bound to the application's window so that it can be modified at runtime.
 */
@property (assign) IBOutlet NSWindow *window;

/**\brief List of models
 *
 * This is the complete list of available modules; The list is populated on the
 * fly and this instance is bound with Interface Builder.
 */
@property (retain,readwrite) IBOutlet NSPopUpButton *models;

/**\brief The OpenGL context
 *
 * This bound to the OpenGL render wrapper on programme startup, so that the
 * delegate can tell the OpenGL context that it needs to draw itself anew.
 */
@property (retain,readwrite) OpenGLRenderer *openGL;

/**\brief Is the settings/camera controls drawer visible?
 *
 * This property is 'YES' if the settings and camera controls drawer is
 * currently visible and 'NO' otherwise.
 */
@property (readonly) BOOL drawerVisible;

/**\brief Is the model/parameters drawer visible?
 *
 * This property is 'YES' if the model and model parameters drawer is currently
 * visible and 'NO' otherwise.
 */
@property (readonly) BOOL modelViewDrawerVisible;

/**\brief Drawer mode
 *
 * Combined version of drawerVisible and modelViewDrawerVisible; The possible
 * values here are:
 *
 *   - 0 when no drawers are visible.
 *   - 1 when drawerVisible is 'YES'.
 *   - 2 when modelViewDrawerVisible is 'YES'.
 *
 * Unlike the two boolean properties, this one can be set.
 */
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

/**\brief Topologic state object
 *
 * Allows access to the generic C++ state object instance for the application.
 */
@property (readonly) topologic::state<GLfloat,MAXDEPTH> *state;

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

/**\brief (Intended) model depth
 *
 * Contains the (intended) model depth. The model depth is the '4' in '4-cube'.
 */
@property (readwrite) NSInteger modelDepth;

/**\brief (Intended) render depth
 *
 * Contains the depth at which content should be rendered, i.e. how many
 * coordinates a single (real) vector ought to have. This is different than the
 * model depth, because it might be desirable to view models at a higher depth
 * than would be strictly necessary - for example, one might want to view a
 * 2-sphere in 4D to observe how it can be turned inside-out with rotations.
 */
@property (readwrite) NSInteger renderDepth;

/**\brief (Intended) model name
 *
 * This is the 'name' of the model, e.g. 'cube' for a square, a cube, a
 * tesseract, etc.
 */
@property (retain,readwrite) NSString *model;

/**\brief Current qualified model name
 *
 * This is the qualified name of the current model, e.g. '4-cube' for a
 * tesseract. This is only updated when a new model is actually set.
 */
@property (retain,readwrite) NSString *selectedModelName;

/** \} */

/**\defgroup osx-data-bindings-parameters Model parameters
 * \brief Model parameters
 *
 * Groups the various model parameters together that change the appearance of a
 * model, but do not contribute to selecting an actual model class to create the
 * model's mesh.
 *
 * \{
 */

/**\brief Model radius
 *
 * The radius parameter of things like spheres. Also used with klein bagels and
 * moebius strips. Recently this parameter also determines the size of cubes and
 * planes, and by extension also the size of IFS elements, which use cubes or
 * planes as their base object to deform.
 */
@property (readwrite) double polarRadius;

/**\brief Precision
 *
 * This used to only control the precision of smooth surfaces by specifying the
 * number of subdivisions for spheres, moebius strips and klein bagels, but now
 * also controls the number of subdivisons of planes (and thus indirectly the
 * fidelity of random flame geometry, as these make use of the planes).
 */
@property (readwrite) double polarPrecision;

/**\brief Number of IFS iterations
 *
 * The number of iterations that an IFS is run for. The actual number of
 * iterations may be lower than this, if running another iteration would exceed
 * the OpenGL vertex limit.
 */
@property (readwrite) NSInteger IFSIterations;

/**\brief Number of distinct IFS functions
 *
 * This is the number of functions to generate when creating random IFS
 * geometry.
 */
@property (readwrite) NSInteger IFSFunctions;

/**\brief IFS PRNG seed
 *
 * The seed value to use when generating random IFS geometry. This parameter
 * makes it possible for random IFS geometry to be found again, if the seed is
 * known.
 */
@property (readwrite) NSInteger IFSSeed;

/**\brief Number of distinct flame variants to use
 *
 * Controls the number of distinct flame variants that have a weight other than
 * zero when generating random fractal flame IFSs.
 */
@property (readwrite) NSInteger FlameVariants;

/**\brief Allow pre-translation rotations
 *
 * When generating random IFS-based models, this flag controls whether a random
 * rotation is applied to the IFS functions before their translation is applied.
 */
@property (readwrite) BOOL IFSPreRotate;

/**\brief Allow post-translation rotations
 *
 * When generating random IFS-based models, this flag controls whether a random
 * rotation is applied to the IFS functions after their translation is applied.
 */
@property (readwrite) BOOL IFSPostRotate;

/**\brief Use fractal flame colouring algorithm
 *
 * When set to 'YES', the OpenGL renderer will apply an approximation of the
 * fractal flame colouring algorithm to the output. This only produces
 * reasonable output with IFS-based models, as it requires index metadata that
 * is not present with other models.
 *
 * The SVG renderer ignores this flag.
 */
@property (readwrite) BOOL fractalFlameColouring;

/** \} */

/**\defgroup osx-data-bindings-colours Colours
 * \brief Render colours
 *
 * These colours are used in the process of rendering the models with OpenGL or
 * when generating an SVG export.
 *
 * \{
 */

/**\brief Background colour
 *
 * This is the background colour for the OpenGL canvas. Accesses the state
 * object directly on queries.
 */
@property (retain,readwrite) NSColor *colourBackground;

/**\brief Wireframe colour
 *
 * This is the wireframe colour for the OpenGL canvas. Accesses the state object
 * directly on queries.
 */
@property (retain,readwrite) NSColor *colourWire;

/**\brief Surface colour
 *
 * This is the surface colour for the OpenGL canvas. Accesses the state object
 * directly on queries.
 *
 * \note This really should be something translucent, because higher-dimensional
 *       geometry would be sort-of see-through from a higher-dimensional
 *       perspective.
 */
@property (retain,readwrite) NSColor *colourSurface;

/** \} */

/** \} */

/**\brief Open file
 *
 * Opens a file on the file system. The file should be an SVG with Topologic
 * metadata, in which case this metadata is read from the file and applied to
 * the active programme instance.
 *
 * \param[in] fullPath The file to open.
 *
 * \bug This function has quite possibly not been implemented.
 */
- (BOOL)openFile:(NSString*)fullPath;

/**\brief Open URL
 *
 * Called when the file referenced by the given URL should be opened. The file
 * should be an SVG with Topologic metadata, in which case this metadata is read
 * from the file and applied to the active programme instance.
 *
 * \param[in] url The URL to open.
 */
- (void)openURL:(NSURL*)url;

/**\brief Open file dialog
 *
 * Shows a dialog to open a file, then if one is selected it is read in and
 * parsed as an SVG file with Topologic metadata.
 *
 * \param[in] sender Part of the Interface Builder action interface; ignored.
 */
- (IBAction)openDocument:(id)sender;

/**\brief Save file dialog
 *
 * Shows a dialog to choose a file to save to. If a target is chosen, the
 * programme creates an SVG with Topologic metadata that can be read back in.
 * Saving to SVGs does not support all of the features supported by the OpenGL
 * renderer.
 *
 * \param[in] sender Part of the Interface Builder action interface; ignored.
 */
- (IBAction)saveDocumentAs:(id)sender;

/**\brief Reset coordinates to 1
 *
 * Reset all coordinates of the currently active camera to '1'.
 *
 * \param[in] sender Part of the Interface Builder action interface; ignored.
 */
- (IBAction)reset1:(id)sender;

/**\brief Reset coordinates to PI/2
 *
 * Reset all coordinates of the currently active camera to 'PI/2'.
 *
 * \param[in] sender Part of the Interface Builder action interface; ignored.
 */
- (IBAction)resetPIHalf:(id)sender;

/**\brief Generate random flame colours
 *
 * Reset the colour map for the fractal flame colouring algorithm.
 *
 * \param[in] sender Part of the Interface Builder action interface; ignored.
 */
- (IBAction)randomFlameColours:(id)sender;

/**\brief Cocoa callback: do close application
 *
 * Tells Cocoa that we really do want to exit the applciation if the last window
 * is closed. The default is to keep the application running.
 */
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication;

/**\brief Go to website
 *
 * Opens a browser window and jumps to Topologic's web site, which may contain
 * information about updates or links to other goodies.
 *
 * \param[in] sender Part of the Interface Builder action interface; ignored.
 */
- (IBAction)goToWebsite:(id)sender;

/**\brief Update OpenGL view
 *
 * Used to tell the OpenGL view that it should draw itself anew.
 *
 * \param[in] sender Part of the Interface Builder action interface; ignored.
 */
- (IBAction)updateOpenGLView:(id)sender;

/**\brief Save file
 *
 * Renders an SVG with Topologic metadata embedded to the specified file.
 *
 * \param[in] fileName Where to put the new SVG to.
 */
- (void) saveFile:(NSString*)fileName;

/**\brief Update model
 *
 * Called after model parameters are changed. This will tell Topologic to use
 * its model factory to recreate a new model with the currently intended
 * parameters.
 */
- (void) updateModel;

/**\brief Recalculate model
 *
 * Like 'updateModel', but the model is merely told to recalculate its mesh.
 * This should be called after changing model parameters.
 */
- (void) updateModelParameters;

@end
