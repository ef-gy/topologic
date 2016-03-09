/**\ingroup frontend-cocoa-touch
 * \file
 * \brief iOS Application Delegate implementation
 *
 * Contains the implementation of the UIApplicationDelegate subclass for the
 * iOS frontend.
 *
 * \copyright
 * This file is part of the Topologic project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic
 * \see Project Source Code: https://github.com/ef-gy/topologic
 * \see Licence Terms: https://github.com/ef-gy/topologic/blob/master/COPYING
 */

#import "iOSAppDelegate.h"

/**\brief Global state object
 *
 * Topologic programme state instance for this programme. Has to be global,
 * because we need to provide C-like accessors for emscripten/JavaScript
 * callers.
 */
static topologic::state<GLfloat,MAXDEPTH> topologicState;

@implementation iOSAppDelegate

@synthesize state;

- (topologic::state<GLfloat,MAXDEPTH> *)state
{
    return &topologicState;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // Override point for customization after application launch.
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(settingDidChange:) name:@"kAppSettingChanged" object:nil];

    [self registerDefaultsFromSettingsBundle];

    [self reconfigure];

    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

- (void)registerDefaultsFromSettingsBundle
{
    NSString *settingsBundle = [[NSBundle mainBundle] pathForResource:@"InAppSettings" ofType:@"bundle"];
    if(!settingsBundle)
    {
        NSLog(@"Could not find InAppSettings.bundle");
        return;
    }

    NSDictionary *settings = [NSDictionary dictionaryWithContentsOfFile:[settingsBundle stringByAppendingPathComponent:@"Root.plist"]];
    NSArray *preferences = [settings objectForKey:@"PreferenceSpecifiers"];

    NSMutableDictionary *defaultsToRegister = [[NSMutableDictionary alloc] initWithCapacity:[preferences count]];
    for (NSDictionary *prefSpecification in preferences)
    {
        NSString *key = [prefSpecification objectForKey:@"Key"];
        if (   key
            && [prefSpecification objectForKey:@"DefaultValue"])
        {
            [defaultsToRegister setObject:[prefSpecification objectForKey:@"DefaultValue"] forKey:key];
        }
    }

    [[NSUserDefaults standardUserDefaults] registerDefaults:defaultsToRegister];
}

- (void) updateModel
{
    NSString *model = [[NSUserDefaults standardUserDefaults] stringForKey:@"model"];
    NSString *format = [[NSUserDefaults standardUserDefaults] stringForKey:@"format"];

    if (!model || !format)
    {
        return;
    }

    if ([[NSUserDefaults standardUserDefaults] integerForKey:@"depth"] < 2)
    {
        [[NSUserDefaults standardUserDefaults] setInteger:2 forKey:@"depth"];
    }
    else if ([[NSUserDefaults standardUserDefaults] integerForKey:@"depth"] > MAXDEPTH)
    {
        [[NSUserDefaults standardUserDefaults] setInteger:MAXDEPTH forKey:@"depth"];
    }

    if ([[NSUserDefaults standardUserDefaults] integerForKey:@"renderDepth"] < 3)
    {
        [[NSUserDefaults standardUserDefaults] setInteger:3 forKey:@"renderDepth"];
    }
    else if ([[NSUserDefaults standardUserDefaults] integerForKey:@"renderDepth"] > MAXDEPTH)
    {
        [[NSUserDefaults standardUserDefaults] setInteger:MAXDEPTH forKey:@"renderDepth"];
    }

    if (   [model isEqual:@"plane"]
        || [model isEqual:@"moebius-strip"]
        || [model isEqual:@"klein-bagel"]
        || [model isEqual:@"klein-bottle"]
        || [model isEqual:@"klein-torus"])
    {
        if ([[NSUserDefaults standardUserDefaults] integerForKey:@"depth"] != 2)
        {
            [[NSUserDefaults standardUserDefaults] setInteger:2 forKey:@"depth"];
        }
    }
    else if (   [model isEqual:@"sphere"])
    {
        if ([[NSUserDefaults standardUserDefaults] integerForKey:@"depth"] >= MAXDEPTH)
        {
            [[NSUserDefaults standardUserDefaults] setInteger:(MAXDEPTH-1) forKey:@"depth"];
        }

        if (  [[NSUserDefaults standardUserDefaults] integerForKey:@"renderDepth"]
            <=[[NSUserDefaults standardUserDefaults] integerForKey:@"depth"])
        {
            [[NSUserDefaults standardUserDefaults] setInteger:([[NSUserDefaults standardUserDefaults] integerForKey:@"renderDepth"]-1) forKey:@"depth"];
        }
    }
    else
    {
        if (  [[NSUserDefaults standardUserDefaults] integerForKey:@"renderDepth"]
            < [[NSUserDefaults standardUserDefaults] integerForKey:@"depth"])
        {
            [[NSUserDefaults standardUserDefaults] setInteger:[[NSUserDefaults standardUserDefaults] integerForKey:@"depth"] forKey:@"renderDepth"];
        }
    }

    efgy::geometry::with<GLfloat,topologic::updateModel,MAXDEPTH>
        (topologicState,
         [format UTF8String],
         [model UTF8String],
         (const unsigned int)[[NSUserDefaults standardUserDefaults] integerForKey:@"depth"],
         (const unsigned int)[[NSUserDefaults standardUserDefaults] integerForKey:@"renderDepth"]);
}

- (void) updateModelParameters
{
    if (topologicState.model)
    {
        topologicState.model->update = true;
    }
}

- (void)settingDidChange:(NSNotification*)notification
{
    [self reconfigureWithIndex:notification.object];
}

- (void) reconfigureWithIndex:(NSString*)index
{
    if ([index isEqual:@"radius"])
    {
        topologicState.parameter.radius = [[NSUserDefaults standardUserDefaults] floatForKey:@"radius"];
        [self updateModelParameters];
    }
    else if ([index isEqual:@"minorRadius"])
    {
        topologicState.parameter.radius2 = [[NSUserDefaults standardUserDefaults] floatForKey:@"minorRadius"];
        [self updateModelParameters];
    }
    else if ([index isEqual:@"constant"])
    {
        topologicState.parameter.constant = [[NSUserDefaults standardUserDefaults] floatForKey:@"constant"];
        [self updateModelParameters];
    }
    else if ([index isEqual:@"precision"])
    {
        topologicState.parameter.precision = [[NSUserDefaults standardUserDefaults] floatForKey:@"precision"];
        [self updateModelParameters];
    }
    else if ([index isEqual:@"limit"])
    {
        topologicState.parameter.vertexLimit = [[NSUserDefaults standardUserDefaults] integerForKey:@"limit"];
        [self updateModelParameters];
    }
    else if ([index isEqual:@"seed"])
    {
        topologicState.parameter.seed = [[NSUserDefaults standardUserDefaults] integerForKey:@"seed"];
        [[NSUserDefaults standardUserDefaults] setInteger:topologicState.parameter.seed forKey:@"seed"];
        [self updateModelParameters];
    }
    else if ([index isEqual:@"functions"])
    {
        topologicState.parameter.functions = [[NSUserDefaults standardUserDefaults] integerForKey:@"functions"];
        [[NSUserDefaults standardUserDefaults] setInteger:topologicState.parameter.functions forKey:@"functions"];
        [self updateModelParameters];
    }
    else if ([index isEqual:@"iterations"])
    {
        topologicState.parameter.iterations = [[NSUserDefaults standardUserDefaults] integerForKey:@"iterations"];
        [[NSUserDefaults standardUserDefaults] setInteger:topologicState.parameter.iterations forKey:@"iterations"];
        [self updateModelParameters];
    }
    else if ([index isEqual:@"variations"])
    {
        topologicState.parameter.flameCoefficients = [[NSUserDefaults standardUserDefaults] integerForKey:@"variations"];
        [[NSUserDefaults standardUserDefaults] setInteger:topologicState.parameter.flameCoefficients forKey:@"variations"];
        [self updateModelParameters];
    }
    else if ([index isEqual:@"fractalFlameColouring"])
    {
        topologicState.fractalFlameColouring = [[NSUserDefaults standardUserDefaults] boolForKey:@"fractalFlameColouring"];
        if (topologicState.fractalFlameColouring)
        {
            topologicState.opengl.setColourMap();
        }
        [self updateModelParameters];
    }
    else if ([index isEqual:@"colour"])
    {
        switch ([[NSUserDefaults standardUserDefaults] integerForKey:@"colour"])
        {
            case 0:
                topologicState.background.red   = 0.45;
                topologicState.background.green = 0.45;
                topologicState.background.blue  = 0.65;
                topologicState.background.alpha = 1;
                
                topologicState.wireframe.red    = 1;
                topologicState.wireframe.green  = 1;
                topologicState.wireframe.blue   = 1;
                topologicState.wireframe.alpha  = 1;
                
                topologicState.surface.red      = 1;
                topologicState.surface.green    = 1;
                topologicState.surface.blue     = 1;
                topologicState.surface.alpha    = 0.1;
                break;

            case 1:
                topologicState.background.red   = 1;
                topologicState.background.green = 1;
                topologicState.background.blue  = 1;
                topologicState.background.alpha = 1;
                
                topologicState.wireframe.red    = 0;
                topologicState.wireframe.green  = 0;
                topologicState.wireframe.blue   = 0;
                topologicState.wireframe.alpha  = 0.8;
                
                topologicState.surface.red      = 0.5;
                topologicState.surface.green    = 0.5;
                topologicState.surface.blue     = 0.5;
                topologicState.surface.alpha    = 0.2;
                break;

            case 2:
                topologicState.background.red   = 0;
                topologicState.background.green = 0;
                topologicState.background.blue  = 0;
                topologicState.background.alpha = 1;
                
                topologicState.wireframe.red    = 0;
                topologicState.wireframe.green  = 1;
                topologicState.wireframe.blue   = 0;
                topologicState.wireframe.alpha  = 0.8;
                
                topologicState.surface.red      = 0;
                topologicState.surface.green    = 1;
                topologicState.surface.blue     = 0;
                topologicState.surface.alpha    = 0.2;
                break;
        }
    }
    else if ([index isEqual:@"preRotate"])
    {
        topologicState.parameter.preRotate = [[NSUserDefaults standardUserDefaults] boolForKey:@"preRotate"];
        [self updateModelParameters];
    }
    else if ([index isEqual:@"postRotate"])
    {
        topologicState.parameter.postRotate = [[NSUserDefaults standardUserDefaults] boolForKey:@"postRotate"];
        [self updateModelParameters];
    }
    else if ([index isEqual:@"model"])
    {
        [self updateModel];
    }
    else if ([index isEqual:@"format"])
    {
        [self updateModel];
    }
    else if ([index isEqual:@"depth"])
    {
        if (   [[[NSUserDefaults standardUserDefaults] stringForKey:@"model"] isEqual:@"sphere"]
            && (   [[NSUserDefaults standardUserDefaults] integerForKey:@"renderDepth"]
                <= [[NSUserDefaults standardUserDefaults] integerForKey:@"depth"]))
        {
            [[NSUserDefaults standardUserDefaults] setInteger:([[NSUserDefaults standardUserDefaults] integerForKey:@"depth"]+1) forKey:@"renderDepth"];
        }
        [self updateModel];
    }
    else if ([index isEqual:@"renderDepth"])
    {
        if (   [[[NSUserDefaults standardUserDefaults] stringForKey:@"model"] isEqual:@"sphere"]
            && (   [[NSUserDefaults standardUserDefaults] integerForKey:@"renderDepth"]
                <= [[NSUserDefaults standardUserDefaults] integerForKey:@"depth"]))
        {
            [[NSUserDefaults standardUserDefaults] setInteger:([[NSUserDefaults standardUserDefaults] integerForKey:@"renderDepth"]-1) forKey:@"depth"];
        }
        [self updateModel];
    }
}

- (void) reconfigure
{
    [self reconfigureWithIndex:@"radius"];
    [self reconfigureWithIndex:@"minorRadius"];
    [self reconfigureWithIndex:@"constant"];
    [self reconfigureWithIndex:@"precision"];
    [self reconfigureWithIndex:@"limit"];
    [self reconfigureWithIndex:@"seed"];
    [self reconfigureWithIndex:@"functions"];
    [self reconfigureWithIndex:@"iterations"];
    [self reconfigureWithIndex:@"variations"];
    [self reconfigureWithIndex:@"fractalFlameColouring"];
    [self reconfigureWithIndex:@"colour"];
    [self reconfigureWithIndex:@"preRotate"];
    [self reconfigureWithIndex:@"postRotate"];
    [self reconfigureWithIndex:@"model"];
}

@end
