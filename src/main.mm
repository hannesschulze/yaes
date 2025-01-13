#import "scene.hh"

#import <Cocoa/Cocoa.h>
#import <SpriteKit/SpriteKit.h>

int main(int argc, char** argv)
{
    NSApplication* app = [NSApplication sharedApplication];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];

    NSMenuItem* quitItem = [[NSMenuItem alloc] init];
    [quitItem setTitle:@"Quit NES"];
    [quitItem setKeyEquivalent:@"q"];
    [quitItem setKeyEquivalentModifierMask:NSEventModifierFlagCommand];
    [quitItem setTarget:app];
    [quitItem setAction:@selector(terminate:)];

    NSMenu* menu = [[NSMenu alloc] init];
    [menu addItem:quitItem];

    NSMenuItem* item = [[NSMenuItem alloc] init];
    [item setSubmenu:menu];

    NSMenu* mainMenu = [[NSMenu alloc] init];
    [mainMenu addItem:item];
    [app setMainMenu:mainMenu];

    Scene* scene = [[Scene alloc] init];
    CGSize minSize = [scene size];


    SKView* view = [[SKView alloc] init];
    [view setIgnoresSiblingOrder:YES];
    [view setShowsFPS:YES];
    [view presentScene:scene];

    NSWindow* window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0., 0., minSize.width * 2, minSize.height * 2)
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