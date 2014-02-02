/**\ingroup frontend-cocoa
 * \file
 * \brief OSX Application Delegate implementation
 *
 * Contains the implementation for the NSApplicationDelegate subclass of the OSX
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

#import "OSXAppDelegate.h"

/**\brief Global state object
 *
 * Topologic programme state instance for this programme. Has to be global,
 * because we need to provide C-like accessors for emscripten/JavaScript
 * callers.
 */
static topologic::state<GLfloat,MAXDEPTH> topologicState;

/**\brief XML wrapper
 *
 * Used to parse SVG files with Topologic metadata and to manipulate the state
 * object accordingly. There only ever needs to be one of these objects.
 */
static topologic::xml xml;

@implementation OSXAppDelegate

@synthesize state;

- (topologic::state<GLfloat,MAXDEPTH> *)state
{
    return &topologicState;
}

@synthesize openGL;

@synthesize drawerVisible;
@synthesize modelViewDrawerVisible;
@synthesize drawerMode;

@synthesize activeCamera;
@synthesize activeCameraType;

@synthesize colourBackground;
@synthesize colourWire;
@synthesize colourSurface;

@synthesize window = _window;

@synthesize cameraActiveCoordinate0;
@synthesize cameraActiveCoordinate1;
@synthesize cameraActiveCoordinate2;
@synthesize cameraActiveCoordinate3;
@synthesize cameraActiveCoordinate4;
@synthesize cameraActiveCoordinate5;
@synthesize cameraActiveCoordinate6;

@synthesize cameraActiveCoordinate0Enabled;
@synthesize cameraActiveCoordinate1Enabled;
@synthesize cameraActiveCoordinate2Enabled;
@synthesize cameraActiveCoordinate3Enabled;
@synthesize cameraActiveCoordinate4Enabled;
@synthesize cameraActiveCoordinate5Enabled;
@synthesize cameraActiveCoordinate6Enabled;

@synthesize cameraActiveCoordinate0Label;
@synthesize cameraActiveCoordinate1Label;
@synthesize cameraActiveCoordinate2Label;
@synthesize cameraActiveCoordinate3Label;
@synthesize cameraActiveCoordinate4Label;
@synthesize cameraActiveCoordinate5Label;
@synthesize cameraActiveCoordinate6Label;

@synthesize model;
@synthesize format;
@synthesize modelDepth;
@synthesize renderDepth;

@synthesize radius;
@synthesize radius2;
@synthesize constant;
@synthesize precision;

@synthesize IFSIterations;
@synthesize IFSFunctions;
@synthesize IFSSeed;
@synthesize FlameVariants;
@synthesize IFSPreRotate;
@synthesize IFSPostRotate;

@synthesize models;
@synthesize formats;

- (void)setColourBackground:(NSColor *)colour
{
    topologicState.background.red   = [colour redComponent];
    topologicState.background.green = [colour greenComponent];
    topologicState.background.blue  = [colour blueComponent];
    topologicState.background.alpha = [colour alphaComponent];
}

- (NSColor *)colourBackground
{
    return [NSColor colorWithDeviceRed:topologicState.background.red green:topologicState.background.green blue:topologicState.background.blue alpha:topologicState.background.alpha];
}

- (void)setColourWire:(NSColor *)colour
{
    topologicState.wireframe.red   = [colour redComponent];
    topologicState.wireframe.green = [colour greenComponent];
    topologicState.wireframe.blue  = [colour blueComponent];
    topologicState.wireframe.alpha = [colour alphaComponent];
}

- (NSColor *)colourWire
{
    return [NSColor colorWithDeviceRed:topologicState.wireframe.red green:topologicState.wireframe.green blue:topologicState.wireframe.blue alpha:topologicState.wireframe.alpha];
}

- (void)setColourSurface:(NSColor *)colour
{
    topologicState.surface.red   = [colour redComponent];
    topologicState.surface.green = [colour greenComponent];
    topologicState.surface.blue  = [colour blueComponent];
    topologicState.surface.alpha = [colour alphaComponent];
}

- (NSColor *)colourSurface
{
    return [NSColor colorWithDeviceRed:topologicState.surface.red green:topologicState.surface.green blue:topologicState.surface.blue alpha:topologicState.surface.alpha];
}

- (BOOL)cameraActiveCoordinate0Enabled
{
    return activeCamera >= 0;
}

- (BOOL)cameraActiveCoordinate1Enabled
{
    return activeCamera >= 0;
}

- (BOOL)cameraActiveCoordinate2Enabled
{
    return activeCamera >= 0;
}

- (BOOL)cameraActiveCoordinate3Enabled
{
    return activeCamera >= 1;
}

- (BOOL)cameraActiveCoordinate4Enabled
{
    return activeCamera >= 2;
}

- (BOOL)cameraActiveCoordinate5Enabled
{
    return activeCamera >= 3;
}

- (BOOL)cameraActiveCoordinate6Enabled
{
    return activeCamera >= 4;
}

- (NSString *)cameraActiveCoordinate0Label
{
    return activeCameraType == 0 ? @"X" : @"r";
}

- (NSString *)cameraActiveCoordinate1Label
{
    return activeCameraType == 0 ? @"Y" : @"\u03b8\u2081";
}

- (NSString *)cameraActiveCoordinate2Label
{
    return activeCameraType == 0 ? @"Z" : @"\u03b8\u2082";
}

- (NSString *)cameraActiveCoordinate3Label
{
    return activeCameraType == 0 ? @"W" : @"\u03b8\u2083";
}

- (NSString *)cameraActiveCoordinate4Label
{
    return activeCameraType == 0 ? @"V" : @"\u03b8\u2084";
}

- (NSString *)cameraActiveCoordinate5Label
{
    return activeCameraType == 0 ? @"U" : @"\u03b8\u2085";
}

- (NSString *)cameraActiveCoordinate6Label
{
    return activeCameraType == 0 ? @"T" : @"\u03b8\u2086";
}

- (void)updateCamera
{
    [self willChangeValueForKey:@"cameraActiveCoordinate0Enabled"];
    [self willChangeValueForKey:@"cameraActiveCoordinate1Enabled"];
    [self willChangeValueForKey:@"cameraActiveCoordinate2Enabled"];
    [self willChangeValueForKey:@"cameraActiveCoordinate3Enabled"];
    [self willChangeValueForKey:@"cameraActiveCoordinate4Enabled"];
    [self willChangeValueForKey:@"cameraActiveCoordinate5Enabled"];
    [self willChangeValueForKey:@"cameraActiveCoordinate6Enabled"];

    [self willChangeValueForKey:@"cameraActiveCoordinate0Label"];
    [self willChangeValueForKey:@"cameraActiveCoordinate1Label"];
    [self willChangeValueForKey:@"cameraActiveCoordinate2Label"];
    [self willChangeValueForKey:@"cameraActiveCoordinate3Label"];
    [self willChangeValueForKey:@"cameraActiveCoordinate4Label"];
    [self willChangeValueForKey:@"cameraActiveCoordinate5Label"];
    [self willChangeValueForKey:@"cameraActiveCoordinate6Label"];
    
    [self willChangeValueForKey:@"cameraActiveCoordinate0"];
    [self willChangeValueForKey:@"cameraActiveCoordinate1"];
    [self willChangeValueForKey:@"cameraActiveCoordinate2"];
    [self willChangeValueForKey:@"cameraActiveCoordinate3"];
    [self willChangeValueForKey:@"cameraActiveCoordinate4"];
    [self willChangeValueForKey:@"cameraActiveCoordinate5"];
    [self willChangeValueForKey:@"cameraActiveCoordinate6"];

    topologicState.setActive((unsigned int)(activeCamera+3));

    [self didChangeValueForKey:@"cameraActiveCoordinate6"];
    [self didChangeValueForKey:@"cameraActiveCoordinate5"];
    [self didChangeValueForKey:@"cameraActiveCoordinate4"];
    [self didChangeValueForKey:@"cameraActiveCoordinate3"];
    [self didChangeValueForKey:@"cameraActiveCoordinate2"];
    [self didChangeValueForKey:@"cameraActiveCoordinate1"];
    [self didChangeValueForKey:@"cameraActiveCoordinate0"];

    [self didChangeValueForKey:@"cameraActiveCoordinate6Label"];
    [self didChangeValueForKey:@"cameraActiveCoordinate5Label"];
    [self didChangeValueForKey:@"cameraActiveCoordinate4Label"];
    [self didChangeValueForKey:@"cameraActiveCoordinate3Label"];
    [self didChangeValueForKey:@"cameraActiveCoordinate2Label"];
    [self didChangeValueForKey:@"cameraActiveCoordinate1Label"];
    [self didChangeValueForKey:@"cameraActiveCoordinate0Label"];
    
    [self didChangeValueForKey:@"cameraActiveCoordinate6Enabled"];
    [self didChangeValueForKey:@"cameraActiveCoordinate5Enabled"];
    [self didChangeValueForKey:@"cameraActiveCoordinate4Enabled"];
    [self didChangeValueForKey:@"cameraActiveCoordinate3Enabled"];
    [self didChangeValueForKey:@"cameraActiveCoordinate2Enabled"];
    [self didChangeValueForKey:@"cameraActiveCoordinate1Enabled"];
    [self didChangeValueForKey:@"cameraActiveCoordinate0Enabled"];
}

- (void)translateCartesianToPolar
{
    topologicState.translateCartesianToPolar();
    topologicState.polarCoordinates = true;
}

- (void)translatePolarToCartesian
{
    topologicState.translatePolarToCartesian();
    topologicState.polarCoordinates = false;
}

- (NSInteger)drawerMode
{
    return drawerMode;
}

- (void)setDrawerMode:(NSInteger)newDrawerMode
{
    [self willChangeValueForKey:@"drawerVisible"];
    [self willChangeValueForKey:@"modelViewDrawerVisible"];

    drawerMode = newDrawerMode;

    [self didChangeValueForKey:@"modelViewDrawerVisible"];
    [self didChangeValueForKey:@"drawerVisible"];
}

- (BOOL)drawerVisible
{
    return drawerMode == 1;
}

- (BOOL)modelViewDrawerVisible
{
    return drawerMode == 2;
}

- (void)awakeFromNib
{
    [models removeAllItems];
    [formats removeAllItems];

    std::set<std::string> mod;
    int li = 2;
    int i;
    for (const std::string &m : efgy::geometry::with<GLfloat,efgy::geometry::functor::modelsWithDepth,MAXDEPTH>(mod,"*",0,0))
    {
        std::istringstream dm (m);
        dm >> i;
        if (i != li)
        {
            li = i;
            [[models menu] addItem:[NSMenuItem separatorItem]];
        }
        
        [models addItemWithTitle:@(m.c_str())];
    }

    std::set<const char*> form;
    for (const char *f : efgy::geometry::with<GLfloat,efgy::geometry::functor::formats,MAXDEPTH>(form,"*","cube",0,0))
    {
        [formats addItemWithTitle:@(f)];
    }

    [self setActiveCamera:0];
    [self setActiveCameraType:1];

    [self setDrawerMode:1];

    [NSColor setIgnoresAlpha:NO];

    [self setColourBackground:[NSColor colorWithDeviceRed:1 green:1 blue:1 alpha:1]];
    [self setColourWire:[NSColor colorWithDeviceRed:0 green:0 blue:0 alpha:0.8]];
    [self setColourSurface:[NSColor colorWithDeviceRed:0 green:0 blue:0 alpha:0.5]];

    [self updateCamera];

    format = @"cartesian";

    [self setModel:@"cube"];
    [self setModelDepth:4];
    [self setRenderDepth:4];

    [self updateModel];
}

- (void)setFractalFlameColouring:(BOOL)value
{
    topologicState.fractalFlameColouring = value;
    [openGL setNeedsDisplay:YES];
}

- (BOOL)fractalFlameColouring
{
    return topologicState.fractalFlameColouring;
}

-(void)setSelectedModelName:(NSString *)value
{
    if (![[self selectedModelName] isEqualToString:value])
    {
        [self setModelDepth:[value integerValue]];
        [self setModel:[value substringFromIndex:[value rangeOfString:@"-"].location+1]];
    }
}

-(NSString *)selectedModelName
{
    if (topologicState.model)
    {
        return @(topologicState.model->name().c_str());
    }
    else
    {
        return @"4-cube";
    }
}

-(void)setModelDepth:(NSInteger)depth
{
    if (modelDepth != depth)
    {
        modelDepth = depth;
        if (modelDepth < 2)
        {
            modelDepth = 2;
        }
        if (modelDepth > MAXDEPTH)
        {
            modelDepth = MAXDEPTH;
        }
    }
    if (!topologicState.model || (topologicState.model->depth() != depth))
    {
        [self updateModel];
    }
}

- (NSInteger)modelDepth
{
    return modelDepth;
}

-(void)setRenderDepth:(NSInteger)depth
{
    if (renderDepth != depth)
    {
        renderDepth = depth;
        if (renderDepth < 3)
        {
            renderDepth = 3;
        }
        if (renderDepth > MAXDEPTH)
        {
            renderDepth = MAXDEPTH;
        }
    }
    if (!topologicState.model || (topologicState.model->renderDepth() != depth))
    {
        [self updateModel];
    }
}

- (NSInteger)renderDepth
{
    return renderDepth;
}

- (void)setModel:(NSString *)value
{
    if (![model isEqualToString:value])
    {
        model = value;
    }
    if (!topologicState.model || (std::string(topologicState.model->id()) != [model UTF8String]))
    {
        [self updateModel];
    }
}

- (NSString *)model
{
    return model;
}

- (void)setFormat:(NSString *)value
{
    if (![format isEqualToString:value])
    {
        format = value;
    }
    if (!topologicState.model || (std::string(topologicState.model->formatID()) != [format UTF8String]))
    {
        [self updateModel];
    }
}

- (NSString *)format
{
    return format;
}

-(void)setActiveCamera:(NSInteger)newValue
{
    if (activeCamera != newValue)
    {
        activeCamera = newValue;
        [self updateCamera];
    }
}

-(NSInteger)activeCamera
{
    return activeCamera;
}

-(void)setActiveCameraType:(NSInteger)newValue
{
    if ((activeCameraType == 0) && (newValue == 1))
    {
        [self translateCartesianToPolar];
    }
    else if ((activeCameraType == 1) && (newValue == 0))
    {
        [self translatePolarToCartesian];
    }
    activeCameraType = newValue;
    [self updateCamera];
}

-(NSInteger)activeCameraType
{
    return topologicState.polarCoordinates ? 1 : 0;
}

-(void)setCameraActiveCoordinate0:(double)newValue
{
    topologicState.setFromCoordinate(0, newValue);
    [openGL setNeedsDisplay:YES];
}

-(double)cameraActiveCoordinate0
{
    return topologicState.getFromCoordinate(0);
}

-(void)setCameraActiveCoordinate1:(double)newValue
{
    topologicState.setFromCoordinate(1, newValue);
    [openGL setNeedsDisplay:YES];
}

-(double)cameraActiveCoordinate1
{
    return topologicState.getFromCoordinate(1);
}

-(void)setCameraActiveCoordinate2:(double)newValue
{
    topologicState.setFromCoordinate(2, newValue);
    [openGL setNeedsDisplay:YES];
}

-(double)cameraActiveCoordinate2
{
    return topologicState.getFromCoordinate(2);
}

-(void)setCameraActiveCoordinate3:(double)newValue
{
    topologicState.setFromCoordinate(3, newValue);
    [openGL setNeedsDisplay:YES];
}

-(double)cameraActiveCoordinate3
{
    return topologicState.getFromCoordinate(3);
}

-(void)setCameraActiveCoordinate4:(double)newValue
{
    topologicState.setFromCoordinate(4, newValue);
    [openGL setNeedsDisplay:YES];
}

-(double)cameraActiveCoordinate4
{
    return topologicState.getFromCoordinate(4);
}

-(void)setCameraActiveCoordinate5:(double)newValue
{
    topologicState.setFromCoordinate(5, newValue);
    [openGL setNeedsDisplay:YES];
}

-(double)cameraActiveCoordinate5
{
    return topologicState.getFromCoordinate(5);
}

-(void)setCameraActiveCoordinate6:(double)newValue
{
    topologicState.setFromCoordinate(6, newValue);
    [openGL setNeedsDisplay:YES];
}

-(double)cameraActiveCoordinate6
{
    return topologicState.getFromCoordinate(6);
}

- (void)setIFSIterations:(NSInteger)value
{
    if (topologicState.parameter.iterations != (unsigned int)value)
    {
        topologicState.parameter.iterations = (unsigned int)value;
        [self updateModelParameters];
    }
}

- (NSInteger)IFSIterations
{
    return topologicState.parameter.iterations;
}

- (void)setIFSFunctions:(NSInteger)value
{
    if (topologicState.parameter.functions != (unsigned int)value)
    {
        topologicState.parameter.functions = (unsigned int)value;
        [self updateModelParameters];
    }
}

- (NSInteger)IFSFunctions
{
    return topologicState.parameter.functions;
}

- (void)setIFSSeed:(NSInteger)value
{
    if (topologicState.parameter.seed != (unsigned int)value)
    {
        topologicState.parameter.seed = (unsigned int)value;
        [self updateModelParameters];
    }
}

- (NSInteger)IFSSeed
{
    return topologicState.parameter.seed;
}

- (void)setFlameVariants:(NSInteger)value
{
    if (topologicState.parameter.flameCoefficients != (unsigned int)value)
    {
        topologicState.parameter.flameCoefficients = (unsigned int)value;
        [self updateModelParameters];
    }
}

- (NSInteger)FlameVariants
{
    return topologicState.parameter.flameCoefficients;
}

- (void)setIFSPreRotate:(BOOL)value
{
    if (topologicState.parameter.preRotate != (bool)value)
    {
        topologicState.parameter.preRotate = (bool)value;
        [self updateModelParameters];
    }
}

- (BOOL)IFSPreRotate
{
    return topologicState.parameter.preRotate;
}

- (void)setIFSPostRotate:(BOOL)value
{
    if (topologicState.parameter.postRotate != (bool)value)
    {
        topologicState.parameter.postRotate = (bool)value;
        [self updateModelParameters];
    }
}

- (BOOL)IFSPostRotate
{
    return topologicState.parameter.postRotate;
}

- (void)setPrecision:(double)value
{
    if (double(topologicState.parameter.precision) != value)
    {
        topologicState.parameter.precision = value;
        [self updateModelParameters];
    }
}

- (double)precision
{
    return topologicState.parameter.precision;
}

- (void)setRadius:(double)value
{
    if (double(topologicState.parameter.radius) != value)
    {
        topologicState.parameter.radius = value;
        [self updateModelParameters];
    }
}

- (double)radius
{
    return topologicState.parameter.radius;
}

- (void)setRadius2:(double)value
{
    if (double(topologicState.parameter.radius2) != value)
    {
        topologicState.parameter.radius2 = value;
        [self updateModelParameters];
    }
}

- (double)radius2
{
    return topologicState.parameter.radius2;
}

- (void)setConstant:(double)value
{
    if (double(topologicState.parameter.constant) != value)
    {
        topologicState.parameter.constant = value;
        [self updateModelParameters];
    }
}

- (double)constant
{
    return topologicState.parameter.constant;
}

- (void)openURL:(NSURL*)url
{
    [[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL:url];

    NSStringEncoding encoding;
    NSError *error = 0;
    NSString *contents = [[[NSString alloc] initWithContentsOfURL:url usedEncoding:&encoding error:&error] autorelease];

    if (!contents)
    {
        return;
    }

    std::string s([contents UTF8String]);
    std::string path([[url absoluteString] UTF8String]);

    topologic::xml::parser p(s, path);

    [self willChangeValueForKey:@"colourBackground"];
    [self willChangeValueForKey:@"colourWire"];
    [self willChangeValueForKey:@"colourSurface"];

    [self willChangeValueForKey:@"cameraActiveCoordinate0"];
    [self willChangeValueForKey:@"cameraActiveCoordinate1"];
    [self willChangeValueForKey:@"cameraActiveCoordinate2"];
    [self willChangeValueForKey:@"cameraActiveCoordinate3"];
    [self willChangeValueForKey:@"cameraActiveCoordinate4"];
    [self willChangeValueForKey:@"cameraActiveCoordinate5"];
    [self willChangeValueForKey:@"cameraActiveCoordinate6"];

    [self willChangeValueForKey:@"precision"];
    [self willChangeValueForKey:@"radius"];
    [self willChangeValueForKey:@"IFSIterations"];

    [self willChangeValueForKey:@"activeCameraType"];

    topologic::parse (topologicState, p);
    
    [self didChangeValueForKey:@"colourBackground"];
    [self didChangeValueForKey:@"colourWire"];
    [self didChangeValueForKey:@"colourSurface"];

    [self didChangeValueForKey:@"cameraActiveCoordinate0"];
    [self didChangeValueForKey:@"cameraActiveCoordinate1"];
    [self didChangeValueForKey:@"cameraActiveCoordinate2"];
    [self didChangeValueForKey:@"cameraActiveCoordinate3"];
    [self didChangeValueForKey:@"cameraActiveCoordinate4"];
    [self didChangeValueForKey:@"cameraActiveCoordinate5"];
    [self didChangeValueForKey:@"cameraActiveCoordinate6"];

    [self didChangeValueForKey:@"precision"];
    [self didChangeValueForKey:@"radius"];
    [self didChangeValueForKey:@"IFSIterations"];

    [self didChangeValueForKey:@"activeCameraType"];

    if (   topologic::parseModel<GLfloat,MAXDEPTH,topologic::updateModelOpenGL> (topologicState, p)
        && topologicState.model)
    {
        [self willChangeValueForKey:@"model"];
        [self willChangeValueForKey:@"modelDepth"];
        [self willChangeValueForKey:@"renderDepth"];
        [self willChangeValueForKey:@"selectedModelName"];

        std::string  mo = topologicState.model->id();
        unsigned int md = topologicState.model->depth();
        unsigned int rd = topologicState.model->renderDepth();

        model       = @(mo.c_str());
        modelDepth  = md;
        renderDepth = rd;

        [self didChangeValueForKey:@"model"];
        [self didChangeValueForKey:@"modelDepth"];
        [self didChangeValueForKey:@"renderDepth"];
        [self didChangeValueForKey:@"selectedModelName"];
    }

    [openGL setNeedsDisplay:YES];

    if (error)
    {
        [[NSAlert alertWithError:error] runModal];
    }
}

- (BOOL)openFile:(NSString*)fullPath
{
    return YES;
}

-(BOOL)application:(NSApplication *)sender openFile:(NSString *)filename
{
    [self openURL:[NSURL fileURLWithPath:filename]];

    return YES;
}

- (IBAction)openDocument:(id)sender
{
    NSOpenPanel *openPanel = [NSOpenPanel openPanel];
    [openPanel setAllowedFileTypes:[NSArray arrayWithObject:@"public.svg-image"]];
    [openPanel beginSheetModalForWindow:_window completionHandler:^(NSInteger result)
     {
         if (result == NSFileHandlingPanelOKButton)
         {
             [self openURL:[openPanel URL]];
         }
     }];
}

- (IBAction)saveDocumentAs:(id)sender
{
    NSSavePanel * savePanel = [NSSavePanel savePanel];
    [savePanel setAllowedFileTypes:[NSArray arrayWithObject:@"public.svg-image"]];
    [savePanel setNameFieldStringValue:[self selectedModelName]];
    [savePanel beginSheetModalForWindow:_window completionHandler:^(NSInteger result)
    {
        if (result == NSFileHandlingPanelOKButton)
        {
            [[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL:[savePanel URL]];
            [self saveFile:[savePanel filename]];
        }
    }];
}

- (void) saveFile:(NSString*)fileName
{
    const char *cFileName = [fileName cStringUsingEncoding:NSUTF8StringEncoding];
    
    std::filebuf fb;
    fb.open (cFileName,std::ios::out);
    
    std::ostream os(&fb);

    if (topologicState.model)
    {
        std::shared_ptr<topologic::render::base<true>> m = topologicState.model;
        topologicState.model = 0;

        efgy::geometry::with<GLfloat,topologic::updateModelSVG,MAXDEPTH>
            (topologicState,
             [[model lowercaseString] UTF8String],
             (const unsigned int)modelDepth,
             (const unsigned int)renderDepth);

        if (topologicState.model)
        {
            os << topologicState.model->render(true).str();
        }

        topologicState.model = m;
    }

    fb.close();

    [self updateModel];
}

- (IBAction)reset1:(id)sender
{
    if (activeCameraType == 0)
    {
        [self setCameraActiveCoordinate0:1];
    }
    [self setCameraActiveCoordinate1:1];
    [self setCameraActiveCoordinate2:1];
    [self setCameraActiveCoordinate3:1];
    [self setCameraActiveCoordinate4:1];
    [self setCameraActiveCoordinate5:1];
    [self setCameraActiveCoordinate6:1];
}

- (IBAction)resetPIHalf:(id)sender
{
    if (activeCameraType == 0)
    {
        [self setCameraActiveCoordinate0:M_PI_2];
    }
    [self setCameraActiveCoordinate1:M_PI_2];
    [self setCameraActiveCoordinate2:M_PI_2];
    [self setCameraActiveCoordinate3:M_PI_2];
    [self setCameraActiveCoordinate4:M_PI_2];
    [self setCameraActiveCoordinate5:M_PI_2];
    [self setCameraActiveCoordinate6:M_PI_2];
}

- (IBAction)randomFlameColours:(id)sender
{
    topologicState.opengl.setColourMap();
    [openGL setNeedsDisplay:YES];
}

- (void) updateModel
{
    [self willChangeValueForKey:@"selectedModelName"];

    efgy::geometry::with<GLfloat,topologic::updateModelOpenGL,MAXDEPTH>
        (topologicState,
         [[format lowercaseString] UTF8String],
         [[model lowercaseString] UTF8String],
         (const unsigned int)modelDepth,
         (const unsigned int)renderDepth);

    [models selectItemWithTitle:[self selectedModelName]];

    [self didChangeValueForKey:@"selectedModelName"];
    [openGL setNeedsDisplay:YES];
}

- (void) updateModelParameters
{
    if (topologicState.model)
    {
        topologicState.model->update();
        [openGL setNeedsDisplay:YES];
    }
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
    return YES;
}

- (IBAction)goToWebsite:(id)sender
{
	[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://ef.gy/topologic"]];
}

- (IBAction)updateOpenGLView:(id)sender
{
    [openGL setNeedsDisplay:YES];
}

@end
