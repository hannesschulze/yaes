#pragma once

#import <SpriteKit/SpriteKit.h>

@interface Scene : SKScene

- (instancetype)init NS_DESIGNATED_INITIALIZER;

+ (instancetype)sceneWithSize:(CGSize)size NS_UNAVAILABLE;
- (instancetype)initWithSize:(CGSize)size NS_UNAVAILABLE;

@end