#pragma once

#import <Cocoa/Cocoa.h>
#import <GameController/GameController.h>

@interface ViewController : GCEventViewController

- (instancetype)initWithFilePath:(NSString*)filePath NS_DESIGNATED_INITIALIZER;

- (instancetype)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil NS_UNAVAILABLE;
- (instancetype)initWithCoder:(NSCoder*)coder NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

@property (readonly) NSSize minimumDisplaySize;
@property (readonly) NSSize preferredDisplaySize;

@end