#pragma once

#import <SpriteKit/SpriteKit.h>
#import <GameController/GameController.h>

NS_ASSUME_NONNULL_BEGIN

@interface Scene : SKScene

+ (instancetype)sceneWithSize:(CGSize)size NS_UNAVAILABLE;
- (instancetype)initWithSize:(CGSize)size NS_UNAVAILABLE;
- (instancetype)initWithCoder:(NSCoder*)coder NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END