/**\ingroup frontend-cocoa
 * \file
 * \brief OSX Application Delegate interface
 *
 * Contains the interface for the NSApplicationDelegate subclass of the OSX
 * frontend.
 *
 * \copyright
 * This file is part of the Topologic project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic
 * \see Project Source Code: https://github.com/ef-gy/topologic
 * \see Licence Terms: https://github.com/ef-gy/topologic/blob/master/COPYING
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

    NSInteger activeCamera;
    
    NSString *model;
    NSString *format;
    NSInteger modelDepth;
    NSInteger renderDepth;
    
    IBOutlet NSPopUpButton *baseModels;
    
    /**\brief Coordinate format list instance
     *
     * This is where the XIB reference for the list of supported coordinate
     * formats is stored.
     */
    IBOutlet NSPopUpButton *formats;
    
    IBOutlet NSSegmentedControl *modelDepths;
    IBOutlet NSSegmentedControl *renderDepths;
    IBOutlet NSSegmentedControl *cameraDepths;
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

/**\brief List of coordinate formats
 *
 * This is the complete list of supported coordinate formats; The list is
 * populated on the fly and this instance is bound with Interface Builder.
 */
@property (retain,readwrite) IBOutlet NSPopUpButton *formats;

/**\brief The OpenGL context
 *
 * This bound to the OpenGL render wrapper on programme startup, so that the
 * delegate can tell the OpenGL context that it needs to draw itself anew.
 */
@property (retain,readwrite) OpenGLRenderer *openGL;

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

/**\brief Current coordinate format
 *
 * Contains the current coordinate format's name, e.g. 'cartesian'.
 */
@property (retain,readwrite) NSString *format;

/**\brief Current qualified model name
 *
 * This is the qualified name of the current model, e.g. '4-cube' for a
 * tesseract. This is only updated when a new model is actually set.
 */
@property (retain,readonly) NSString *selectedModelName;

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

/**\brief Major model radius
 *
 * The radius parameter of things like spheres. Also used with klein bagels and
 * moebius strips. Recently this parameter also determines the size of cubes and
 * planes, and by extension also the size of IFS elements, which use cubes or
 * planes as their base object to deform.
 *
 * For models such as tori, which take two radius parameters, this determines
 * the major radius of the model.
 */
@property (readwrite) double radius;

/**\brief Minor model radius
 *
 * For models such as tori, which take several radii, this defines the minor
 * radius of the model.
 */
@property (readwrite) double radius2;

/**\brief Constant for formulae
 *
 * Used in formulae like the one for klein bottles, where it determines some
 * parts of the resulting geometry.
 */
@property (readwrite) double constant;

/**\brief Precision
 *
 * This used to only control the precision of smooth surfaces by specifying the
 * number of subdivisions for spheres, moebius strips and klein bagels, but now
 * also controls the number of subdivisons of planes (and thus indirectly the
 * fidelity of random flame geometry, as these make use of the planes).
 */
@property (readwrite) double precision;

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
 * Opens a file on the file system. The file should be an SVG or JSON with
 * Topologic metadata, in which case this metadata is read from the file and
 * applied to the active programme instance.
 *
 * \param[in] fullPath The file to open.
 *
 * \note This is just a wrapper for openURL.
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

/**\brief Go to repository
 *
 * Opens a browser window and jumps to Topologic's source code repository. Part
 * of the 'help' menu.
 *
 * \param[in] sender Part of the Interface Builder action interface; ignored.
 */
- (IBAction)goToRepository:(id)sender;

/**\brief Open in browser
 *
 * Opens a browser window with the web version of Topologic using the current
 * settings.
 *
 * \param[in] sender Part of the Interface Builder action interface; ignored.
 */
- (IBAction)openInBrowser:(id)sender;

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
