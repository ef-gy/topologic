/**\file
 * \brief OSX Application Delegate implementation
 *
 * Contains the implementation for the NSApplicationDelegate subclass of the OSX
 * frontend.
 *
 * \copyright
 * Copyright (c) 2012-2013, Topologic Project Members
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

typedef efgy::math::primitive<double> FP;

static topologic::state<topologic::GLFP,MAXDEPTH> topologicState;
static topologic::xml xml;

@implementation OSXAppDelegate

@synthesize state;

- (topologic::state<topologic::GLFP,MAXDEPTH> *)state
{
    return &topologicState;
}

@synthesize outputTemplates;
@synthesize openGL;
@synthesize subtypeSegmentedControl;

@synthesize drawerVisible;
@synthesize modelViewDrawerVisible;
@synthesize drawerMode;

@synthesize activeCamera;
@synthesize activeCameraType;

@synthesize colourBackground;
@synthesize colourWire;
@synthesize colourSurface;
@synthesize colourLighting;

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

@synthesize selectedModelName;

@synthesize model;
@synthesize modelDepth;
@synthesize renderDepth;

@synthesize polarRadius;
@synthesize polarPrecision;

@synthesize IFSIterations;
@synthesize IFSFunctions;
@synthesize IFSSeed;
@synthesize FlameVariants;
@synthesize IFSPreRotate;
@synthesize IFSPostRotate;

@synthesize models;

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

- (void)updateCamera
{
    switch (activeCameraType)
    {
        case 0:
            [self setCameraActiveCoordinate0Label:@"X"];
            [self setCameraActiveCoordinate1Label:@"Y"];
            [self setCameraActiveCoordinate2Label:@"Z"];
            [self setCameraActiveCoordinate3Label:@"W"];
            [self setCameraActiveCoordinate4Label:@"V"];
            [self setCameraActiveCoordinate5Label:@"U"];
            [self setCameraActiveCoordinate6Label:@"T"];
            break;
        case 1:
            [self setCameraActiveCoordinate0Label:@"r"];
            [self setCameraActiveCoordinate1Label:@"θ₁"];
            [self setCameraActiveCoordinate2Label:@"θ₂"];
            [self setCameraActiveCoordinate3Label:@"θ₃"];
            [self setCameraActiveCoordinate4Label:@"θ₄"];
            [self setCameraActiveCoordinate5Label:@"θ₅"];
            [self setCameraActiveCoordinate6Label:@"θ₆"];
            break;
    }

    switch (activeCamera)
    {
        case 4:
            [self setCameraActiveCoordinate6Enabled:YES];
        case 3:
            [self setCameraActiveCoordinate5Enabled:YES];
        case 2:
            [self setCameraActiveCoordinate4Enabled:YES];
        case 1:
            [self setCameraActiveCoordinate3Enabled:YES];
        case 0:
            [self setCameraActiveCoordinate2Enabled:YES];
            [self setCameraActiveCoordinate1Enabled:YES];
            [self setCameraActiveCoordinate0Enabled:YES];
    }

    switch (activeCamera)
    {
        case 0:
            [self setCameraActiveCoordinate3Enabled:NO];
        case 1:
            [self setCameraActiveCoordinate4Enabled:NO];
        case 2:
            [self setCameraActiveCoordinate5Enabled:NO];
        case 3:
            [self setCameraActiveCoordinate6Enabled:NO];
    }

    [self willChangeValueForKey:@"cameraActiveCoordinate0"];
    [self willChangeValueForKey:@"cameraActiveCoordinate1"];
    [self willChangeValueForKey:@"cameraActiveCoordinate2"];
    [self willChangeValueForKey:@"cameraActiveCoordinate3"];
    [self willChangeValueForKey:@"cameraActiveCoordinate4"];
    [self willChangeValueForKey:@"cameraActiveCoordinate5"];
    [self willChangeValueForKey:@"cameraActiveCoordinate6"];

    topologicState.setActive((unsigned int)(activeCamera+3));

    [self didChangeValueForKey:@"cameraActiveCoordinate0"];
    [self didChangeValueForKey:@"cameraActiveCoordinate1"];
    [self didChangeValueForKey:@"cameraActiveCoordinate2"];
    [self didChangeValueForKey:@"cameraActiveCoordinate3"];
    [self didChangeValueForKey:@"cameraActiveCoordinate4"];
    [self didChangeValueForKey:@"cameraActiveCoordinate5"];
    [self didChangeValueForKey:@"cameraActiveCoordinate6"];
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

- (void)dealloc
{
    [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
}

- (NSInteger)drawerMode
{
    return drawerMode;
}

- (void)setDrawerMode:(NSInteger)newDrawerMode
{
    drawerMode = newDrawerMode;

    switch (drawerMode)
    {
        case 0:
            [self setDrawerVisible:NO];
            [self setModelViewDrawerVisible:NO];
            break;
        case 1:
            [self setDrawerVisible:YES];
            [self setModelViewDrawerVisible:NO];
            break;
        case 2:
            [self setDrawerVisible:NO];
            [self setModelViewDrawerVisible:YES];
            break;
    }
}

- (BOOL)drawerVisible
{
    return drawerVisible;
}

- (void)setDrawerVisible:(BOOL)visibility
{
    if (visibility != drawerVisible)
    {
        drawerVisible = visibility;
        if (visibility)
        {
            [self setDrawerMode:1];
        }
        else
        {
            [self setDrawerMode:(modelViewDrawerVisible ? 2 : 0)];
        }
    }
}

- (BOOL)modelViewDrawerVisible
{
    return modelViewDrawerVisible;
}

- (void)setModelViewDrawerVisible:(BOOL)visibility
{
    if (visibility != modelViewDrawerVisible)
    {
        modelViewDrawerVisible = visibility;
        if (visibility)
        {
            [self setDrawerMode:2];
        }
        else
        {
            [self setDrawerMode:(drawerVisible ? 1 : 0)];
        }
    }
}

- (void)awakeFromNib
{
    [models removeAllItems];
    [models addItemWithTitle:@"3-simplex"];
    [models addItemWithTitle:@"4-simplex"];
    [models addItemWithTitle:@"5-simplex"];
    [models addItemWithTitle:@"6-simplex"];
    [models addItemWithTitle:@"7-simplex"];
    [[models menu] addItem:[NSMenuItem separatorItem]];
    [models addItemWithTitle:@"2-plane"];
    [[models menu] addItem:[NSMenuItem separatorItem]];
    [models addItemWithTitle:@"3-cube"];
    [models addItemWithTitle:@"4-cube"];
    [models addItemWithTitle:@"5-cube"];
    [models addItemWithTitle:@"6-cube"];
    [models addItemWithTitle:@"7-cube"];
    [[models menu] addItem:[NSMenuItem separatorItem]];
    [models addItemWithTitle:@"2-sphere"];
    [models addItemWithTitle:@"3-sphere"];
//    [[models menu] addItem:[NSMenuItem separatorItem]];
//    [models addItemWithTitle:@"3-torus"];
    [[models menu] addItem:[NSMenuItem separatorItem]];
    [models addItemWithTitle:@"2-moebius-strip"];
    [models addItemWithTitle:@"2-klein-bagel"];
    [[models menu] addItem:[NSMenuItem separatorItem]];
    [models addItemWithTitle:@"2-sierpinski-gasket"];
    [models addItemWithTitle:@"3-sierpinski-gasket"];
    [models addItemWithTitle:@"4-sierpinski-gasket"];
    [models addItemWithTitle:@"5-sierpinski-gasket"];
    [models addItemWithTitle:@"6-sierpinski-gasket"];
    [models addItemWithTitle:@"7-sierpinski-gasket"];
    [[models menu] addItem:[NSMenuItem separatorItem]];
    [models addItemWithTitle:@"2-sierpinski-carpet"];
    [models addItemWithTitle:@"3-sierpinski-carpet"];
    [[models menu] addItem:[NSMenuItem separatorItem]];
    [models addItemWithTitle:@"2-random-affine-ifs"];
    [models addItemWithTitle:@"3-random-affine-ifs"];
    [models addItemWithTitle:@"4-random-affine-ifs"];
    [models addItemWithTitle:@"5-random-affine-ifs"];
    [models addItemWithTitle:@"6-random-affine-ifs"];
    [models addItemWithTitle:@"7-random-affine-ifs"];
    [[models menu] addItem:[NSMenuItem separatorItem]];
    [models addItemWithTitle:@"2-random-flame"];
    [models addItemWithTitle:@"3-random-flame"];
    [models addItemWithTitle:@"4-random-flame"];
    [models addItemWithTitle:@"5-random-flame"];
    [models addItemWithTitle:@"6-random-flame"];
    [models addItemWithTitle:@"7-random-flame"];

    [self setActiveCamera:0];
    [self setActiveCameraType:1];

    [self setDrawerMode:1];

    [NSColor setIgnoresAlpha:NO];

    [self setColourBackground:[NSColor colorWithDeviceRed:0.45 green:0.45 blue:0.65 alpha:1]];
    [self setColourWire:[NSColor colorWithDeviceRed:1 green:1 blue:1 alpha:1]];
    [self setColourSurface:[NSColor colorWithDeviceRed:1 green:1 blue:1 alpha:0.1]];
    [self setColourLighting:[NSColor colorWithDeviceRed:1 green:1 blue:1 alpha:1]];

    [self updateCamera];

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

-(double)clampAngle:(double)a
{
    while (a > M_PI)
    {
        a -= 2*M_PI;
    }
    while (a < -M_PI)
    {
        a += 2*M_PI;
    }

    return a;
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

- (void)setPolarPrecision:(double)value
{
    if (double(topologicState.parameter.polarPrecision) != value)
    {
        topologicState.parameter.polarPrecision = value;
        [self updateModelParameters];
    }
}

- (double)polarPrecision
{
    return topologicState.parameter.polarPrecision;
}

- (void)setPolarRadius:(double)value
{
    if (double(topologicState.parameter.polarRadius) != value)
    {
        topologicState.parameter.polarRadius = value;
        [self updateModelParameters];
    }
}

- (double)polarRadius
{
    return topologicState.parameter.polarRadius;
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
    [self willChangeValueForKey:@"colourLighting"];

    [self willChangeValueForKey:@"cameraActiveCoordinate0"];
    [self willChangeValueForKey:@"cameraActiveCoordinate1"];
    [self willChangeValueForKey:@"cameraActiveCoordinate2"];
    [self willChangeValueForKey:@"cameraActiveCoordinate3"];
    [self willChangeValueForKey:@"cameraActiveCoordinate4"];
    [self willChangeValueForKey:@"cameraActiveCoordinate5"];
    [self willChangeValueForKey:@"cameraActiveCoordinate6"];

    [self willChangeValueForKey:@"polarPrecision"];
    [self willChangeValueForKey:@"polarRadius"];
    [self willChangeValueForKey:@"IFSIterations"];

    [self willChangeValueForKey:@"activeCameraType"];

    topologic::parse (topologicState, p);
    
    [self didChangeValueForKey:@"colourBackground"];
    [self didChangeValueForKey:@"colourWire"];
    [self didChangeValueForKey:@"colourSurface"];
    [self didChangeValueForKey:@"colourLighting"];

    [self didChangeValueForKey:@"cameraActiveCoordinate0"];
    [self didChangeValueForKey:@"cameraActiveCoordinate1"];
    [self didChangeValueForKey:@"cameraActiveCoordinate2"];
    [self didChangeValueForKey:@"cameraActiveCoordinate3"];
    [self didChangeValueForKey:@"cameraActiveCoordinate4"];
    [self didChangeValueForKey:@"cameraActiveCoordinate5"];
    [self didChangeValueForKey:@"cameraActiveCoordinate6"];

    [self didChangeValueForKey:@"polarPrecision"];
    [self didChangeValueForKey:@"polarRadius"];
    [self didChangeValueForKey:@"IFSIterations"];

    [self didChangeValueForKey:@"activeCameraType"];

    if (   topologic::parseModel<topologic::GLFP,MAXDEPTH,topologic::render::opengl> (topologicState, p)
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

        topologic::setModelWithTypeString
            <topologic::GLFP,MAXDEPTH,MAXDEPTH,topologic::render::svg>
            ([[model lowercaseString] UTF8String],
             topologicState,
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

    topologic::setModelWithTypeString
        <topologic::GLFP,MAXDEPTH,MAXDEPTH,topologic::render::opengl>
        ([[model lowercaseString] UTF8String],
         topologicState,
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
