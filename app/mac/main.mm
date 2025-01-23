#import "scene.hh"
#import <iostream>

#import <Cocoa/Cocoa.h>
#import <SpriteKit/SpriteKit.h>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <rom-path>" << std::endl;
        return 1;
    }

    auto filePath = [NSString stringWithUTF8String:argv[1]];

    auto app = [NSApplication sharedApplication];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];

    auto quitItem = [[NSMenuItem alloc] init];
    [quitItem setTitle:@"Quit NES"];
    [quitItem setKeyEquivalent:@"q"];
    [quitItem setKeyEquivalentModifierMask:NSEventModifierFlagCommand];
    [quitItem setTarget:app];
    [quitItem setAction:@selector(terminate:)];

    auto menu = [[NSMenu alloc] init];
    [menu addItem:quitItem];

    auto item = [[NSMenuItem alloc] init];
    [item setSubmenu:menu];

    auto mainMenu = [[NSMenu alloc] init];
    [mainMenu addItem:item];
    [app setMainMenu:mainMenu];

    auto scene = [[Scene alloc] initWithFilePath:filePath];
    auto minSize = [scene size];

    auto view = [[SKView alloc] init];
    [view setIgnoresSiblingOrder:YES];
    [view setShowsFPS:YES];
    [view presentScene:scene];

    auto window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0., 0., minSize.width * 2, minSize.height * 2)
                                              styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                                                         NSWindowMiniaturizeButton | NSWindowStyleMaskResizable)
                                                backing:NSBackingStoreBuffered
                                                  defer:false];
    [window setTitle:@"NES"];
    [window setContentAspectRatio:minSize];
    [window setContentMinSize:minSize];
    [window setContentView:view];
    [window makeKeyAndOrderFront:nil];

    [app activateIgnoringOtherApps:YES];
    [app run];

    return 0;
}