#import "view-controller.hh"
#import <iostream>

#import <Cocoa/Cocoa.h>


int main()
{
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

	auto viewController = [[ViewController alloc] init];
	auto window = [NSWindow windowWithContentViewController:viewController];
    [window setTitle:@"NES"];
    [window setContentAspectRatio:[viewController minimumDisplaySize]];
	[window setContentMinSize:[viewController minimumDisplaySize]];
	[window setContentSize:[viewController preferredDisplaySize]];
    [window makeKeyAndOrderFront:nil];

    [app activateIgnoringOtherApps:YES];
    [app run];

    return 0;
}