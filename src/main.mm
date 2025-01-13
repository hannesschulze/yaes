#import <Cocoa/Cocoa.h>
#import "view-controller.hh"

int main(int argc, char** argv)
{
    NSApplication* app = [NSApplication sharedApplication];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];

    NSMenuItem *quitItem = [[NSMenuItem alloc] init];
    [quitItem setTitle:@"Quit NES"];
    [quitItem setKeyEquivalent:@"q"];
    [quitItem setKeyEquivalentModifierMask:NSEventModifierFlagCommand];
    [quitItem setTarget:app];
    [quitItem setAction:@selector(terminate:)];

    NSMenu *menu = [[NSMenu alloc] init];
    [menu addItem:quitItem];

    NSMenuItem *item = [[NSMenuItem alloc] init];
    [item setSubmenu:menu];

    NSMenu *mainMenu = [[NSMenu alloc] init];
    [mainMenu addItem:item];
    [app setMainMenu:mainMenu];

    NSWindow *window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0., 0., 640., 480.)
                                                   styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                                                              NSWindowMiniaturizeButton | NSWindowStyleMaskResizable)
                                                     backing:NSBackingStoreBuffered
                                                       defer:false];
    [window setTitle:@"NES"];
    [window setContentViewController:[[ViewController alloc] init]];
    [window setContentSize:NSMakeSize(640., 480.)];
    [window makeKeyAndOrderFront:nil];

    [app activateIgnoringOtherApps:YES];
    [app run];

    return 0;
}