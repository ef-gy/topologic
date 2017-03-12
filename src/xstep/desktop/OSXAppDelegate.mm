/**\ingroup frontend-cocoa
 * \file
 * \brief OSX Application Delegate implementation
 *
 * Contains the implementation for the NSApplicationDelegate subclass of the OSX
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

@synthesize activeCamera;
@synthesize activeCameraType;

@synthesize colourBackground;
@synthesize colourWire;
@synthesize colourSurface;

@synthesize window = _window;

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
@synthesize updateFractalFlameColours;

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

- (void)updateCamera
{
  topologicState.setActive((std::size_t)(activeCamera));
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

- (void)setUpBaseModels
{
  using namespace efgy::geometry;

  [baseModels removeAllItems];

  std::set<const char*> bmods;
  for (const char *b : with<GLfloat,functor::models,MAXDEPTH>(bmods,"*",0,0))
  {
    [baseModels addItemWithTitle:@(b)];
  }
}

- (void)setUpFormats
{
  using namespace efgy::geometry;

  [formats removeAllItems];

  std::set<const char*> form;
  for (const char *f : with<GLfloat,functor::formats,MAXDEPTH>(form,"*","cube",0,0))
  {
    [formats addItemWithTitle:@(f)];
  }
}

- (void)setUpModelDepths
{
  using namespace efgy::geometry;
  
  [modelDepths setSegmentCount:0];

  std::set<std::size_t> dep;
  int i = 0;
  with<GLfloat,functor::modelDimensions,MAXDEPTH>(dep,"*",0,0);
  [modelDepths setSegmentCount:dep.size()];
  for (std::size_t d : dep)
  {
    std::string label = std::to_string(d) + "D";
    [[modelDepths cell] setTag:d forSegment:i];
    [modelDepths setLabel:@(label.c_str()) forSegment:i];
    i++;
  }
}

- (void)setUpRenderDepths
{
  using namespace efgy::geometry;

  [renderDepths setSegmentCount:0];

  std::set<std::size_t> rdep;
  int i = 0;
  with<GLfloat,functor::renderDimensions,MAXDEPTH>(rdep,"*",0,0);
  [renderDepths setSegmentCount:rdep.size()];
  for (std::size_t d : rdep)
  {
    std::string label = std::to_string(d) + "D";
    [[renderDepths cell] setTag:d forSegment:i];
    [renderDepths setLabel:@(label.c_str()) forSegment:i];
    i++;
  }
}

- (void)setUpCameraDepths
{
  using namespace efgy::geometry;

  [cameraDepths setSegmentCount:0];

  std::set<std::size_t> rdep;
  int i = 0;
  with<GLfloat,functor::renderDimensions,MAXDEPTH>(rdep,"*",0,0);
  [cameraDepths setSegmentCount:rdep.size()];
  for (std::size_t d : rdep)
  {
    std::string label = std::to_string(d) + "D";
    [[cameraDepths cell] setTag:d forSegment:i];
    [cameraDepths setLabel:@(label.c_str()) forSegment:i];
    i++;
  }
}

- (void)updateAvailableModelDepths
{
  using namespace efgy::geometry;

  std::set<std::size_t> dep;
  with<GLfloat,functor::modelDimensions,MAXDEPTH>(
      dep, [model UTF8String], 0, 0);

  auto mdim = [self modelDepth];
  bool needsReset = false;

  for (std::size_t i = 0; i < [modelDepths segmentCount]; i++)
  {
    auto dim = [[modelDepths cell] tagForSegment:i];
    bool enabled = (dep.find((std::size_t)dim) != dep.end());
    
    [modelDepths setEnabled:enabled forSegment:i];

    needsReset = needsReset || (!enabled && (mdim == dim));
  }

  if (needsReset) {
    for (std::size_t i = 0; i < [modelDepths segmentCount]; i++) {
      if ([modelDepths isEnabledForSegment:i]) {
        [self setModelDepth:[[modelDepths cell] tagForSegment:i]];
        break;
      }
    }
  }

  [self updateAvailableRenderDepths];
}

- (void)updateAvailableRenderDepths
{
  using namespace efgy::geometry;

  std::set<std::size_t> dep;
  with<GLfloat,functor::renderDimensions,MAXDEPTH>(
      dep, [model UTF8String],(std::size_t)[self modelDepth],0);

  auto mdim = [self renderDepth];
  bool needsReset = false;

  for (std::size_t i = 0; i < [renderDepths segmentCount]; i++)
  {
    auto dim = [[renderDepths cell] tagForSegment:i];
    bool enabled = (dep.find((std::size_t)dim) != dep.end());

    [renderDepths setEnabled:enabled forSegment:i];

    needsReset = needsReset || (!enabled && (mdim == dim));
  }

  if (needsReset) {
    for (std::size_t i = 0; i < [renderDepths segmentCount]; i++) {
      if ([renderDepths isEnabledForSegment:i]) {
        [self setRenderDepth:[[renderDepths cell] tagForSegment:i]];
        break;
      }
    }
  }

  [self updateAvailableCameraDepths];
}

- (void)updateAvailableCameraDepths
{
  auto rd = [self renderDepth];

  auto mdim = [self activeCamera];
  bool needsReset = false;

  for (std::size_t i = 0; i < [cameraDepths segmentCount]; i++)
  {
    auto dim = [[cameraDepths cell] tagForSegment:i];
    bool enabled = false;
    if (rd == 2) {
      enabled = dim == 2;
    } else {
      enabled = (dim > 2) && (dim <= rd);
    }

    [cameraDepths setEnabled:enabled forSegment:i];

    needsReset = needsReset || (!enabled && (mdim == dim));
  }
  
  if (needsReset) {
    for (std::size_t i = 0; i < [cameraDepths segmentCount]; i++) {
      if ([cameraDepths isEnabledForSegment:i]) {
        [self setActiveCamera:[[cameraDepths cell] tagForSegment:i]];
        break;
      }
    }
  }
}

- (void)awakeFromNib
{
  using namespace efgy::geometry;

  [cameraDepths setSegmentCount:0];

  [self setUpBaseModels];
  [self setUpFormats];
  [self setUpModelDepths];
  [self setUpRenderDepths];
  [self setUpCameraDepths];

  [self setActiveCamera:3];
  [self setActiveCameraType:1];

  [NSColor setIgnoresAlpha:NO];
  
  [self setColourBackground:[NSColor colorWithDeviceRed:1 green:1 blue:1 alpha:1]];
  [self setColourWire:[NSColor colorWithDeviceRed:0 green:0 blue:0 alpha:0.8]];
  [self setColourSurface:[NSColor colorWithDeviceRed:0.5 green:0.5 blue:0.5 alpha:0.2]];
  
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
  if (topologicState.fractalFlameColouring)
  {
    [self setUpdateFractalFlameColours:true];
  }
  [openGL setNeedsDisplay:YES];
}

- (BOOL)fractalFlameColouring
{
  return topologicState.fractalFlameColouring;
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
    if (modelDepth < 1)
    {
      modelDepth = 1;
    }
    if (modelDepth > MAXDEPTH)
    {
      modelDepth = MAXDEPTH;
    }
  }
  if (!topologicState.model || (topologicState.model->depth != depth))
  {
    [self updateModel];
  }

  [self updateAvailableRenderDepths];
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
    if (renderDepth < 2)
    {
      renderDepth = 2;
    }
    if (renderDepth > MAXDEPTH)
    {
      renderDepth = MAXDEPTH;
    }
  }
  if (!topologicState.model || (topologicState.model->renderDepth != depth))
  {
    [self updateModel];
  }

  [self updateAvailableCameraDepths];
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
  if (!topologicState.model || (std::string(topologicState.model->id) != [model UTF8String]))
  {
    [self updateModel];
  }

  [self updateAvailableModelDepths];
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
  if (!topologicState.model || (std::string(topologicState.model->formatID) != [format UTF8String]))
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
  efgy::json::value<> v;
  
  topologic::xml::parser p(s, path);
  if (!p.valid)
  {
    s >> v;
  }
  
  [self willChangeValueForKey:@"colourBackground"];
  [self willChangeValueForKey:@"colourWire"];
  [self willChangeValueForKey:@"colourSurface"];

  [self willChangeValueForKey:@"precision"];
  [self willChangeValueForKey:@"radius"];
  [self willChangeValueForKey:@"IFSIterations"];
  
  [self willChangeValueForKey:@"activeCameraType"];
  
  if (p.valid)
  {
    topologic::parse (topologicState, p);
  }
  else
  {
    topologic::parse (topologicState, v);
  }
  
  [self didChangeValueForKey:@"colourBackground"];
  [self didChangeValueForKey:@"colourWire"];
  [self didChangeValueForKey:@"colourSurface"];
  
  [self didChangeValueForKey:@"precision"];
  [self didChangeValueForKey:@"radius"];
  [self didChangeValueForKey:@"IFSIterations"];
  
  [self didChangeValueForKey:@"activeCameraType"];
  
  if (p.valid)
  {
    topologic::parseModel<GLfloat,MAXDEPTH,topologic::updateModel> (topologicState, p);
  }
  else
  {
    topologic::parseModel<GLfloat,MAXDEPTH,topologic::updateModel> (topologicState, v);
  }
  
  if (topologicState.model)
  {
    [self willChangeValueForKey:@"model"];
    [self willChangeValueForKey:@"modelDepth"];
    [self willChangeValueForKey:@"renderDepth"];
    [self willChangeValueForKey:@"selectedModelName"];
    
    std::string  mo = topologicState.model->id;
    std::size_t md = topologicState.model->depth;
    std::size_t rd = topologicState.model->renderDepth;
    
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
  [self openURL:[NSURL fileURLWithPath:fullPath]];
  
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
  [openPanel setAllowedFileTypes:[NSArray arrayWithObjects:@"public.svg-image", @"public.json", nil]];
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
  [savePanel setAllowedFileTypes:[NSArray arrayWithObjects:@"public.svg-image", @"public.json", nil]];
  [savePanel setNameFieldStringValue:[self selectedModelName]];
  [savePanel setAllowsOtherFileTypes:NO];
  [savePanel setExtensionHidden:NO];
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
  
  CFStringRef UTI = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, (CFStringRef) [fileName pathExtension], NULL);
  
  if ([(NSString*)UTI isEqualToString:@"public.svg-image"])
  {
    if (topologicState.model)
    {
      topologicState.model->update = true;
      topologicState.model->svg(os, true);
    }
  }
  else if ([(NSString*)UTI isEqualToString:@"public.json"])
  {
    os << efgy::json::tag() << topologicState;
  }
  
  fb.close();
}

- (IBAction)randomFlameColours:(id)sender
{
  topologicState.opengl.setColourMap(topologicState.parameter.colourMap);
  [openGL setNeedsDisplay:YES];
}

- (void) updateModel
{
  [self willChangeValueForKey:@"selectedModelName"];
  
  efgy::geometry::with<GLfloat,topologic::updateModel,MAXDEPTH>
  (topologicState,
   [[format lowercaseString] UTF8String],
   [[model lowercaseString] UTF8String],
   (const std::size_t)modelDepth,
   (const std::size_t)renderDepth);

  [self didChangeValueForKey:@"selectedModelName"];
  [openGL setNeedsDisplay:YES];

  [self updateAvailableModelDepths];
}

- (void) updateModelParameters
{
  if (topologicState.model)
  {
    topologicState.model->update = true;
    [openGL setNeedsDisplay:YES];
  }
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
  return YES;
}

- (IBAction)goToWebsite:(id)sender
{
  [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@(topologic::website)]];
}

- (IBAction)goToRepository:(id)sender
{
  [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@(topologic::repository)]];
}

- (IBAction)openInBrowser:(id)sender
{
  std::ostringstream json("");
  std::ostringstream service("");
  
  json << efgy::json::tag() << topologicState;
  
  NSString *unescaped = @(json.str().c_str());
  NSString *escaped = [unescaped stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLPathAllowedCharacterSet]];
  
  service << topologic::service << ":" << [escaped UTF8String];
  
  [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@(service.str().c_str())]];
}

- (IBAction)updateOpenGLView:(id)sender
{
  [openGL setNeedsDisplay:YES];
}

@end
