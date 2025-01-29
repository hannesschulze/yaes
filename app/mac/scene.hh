#pragma once

#import "controller-adapter.hh"

#import <SpriteKit/SpriteKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface Scene : SKScene

- (instancetype)initWithFilePath:(NSString*)filePath NS_DESIGNATED_INITIALIZER;

+ (instancetype)sceneWithSize:(CGSize)size NS_UNAVAILABLE;
- (instancetype)initWithSize:(CGSize)size NS_UNAVAILABLE;
- (instancetype)initWithCoder:(NSCoder*)coder NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;

@property (nonatomic, nullable) id<ControllerAdapter> primaryController;
@property (nonatomic, nullable) id<ControllerAdapter> secondaryController;

@end

NS_ASSUME_NONNULL_END