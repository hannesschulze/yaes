#pragma once

#import "controller-adapter.hh"

#import <GameController/GameController.h>

NS_ASSUME_NONNULL_BEGIN

@interface ControllerAdapterGamepad : NSObject <ControllerAdapter>

- (instancetype)initWithProfile:(GCExtendedGamepad*)profile;
- (instancetype)init NS_UNAVAILABLE;

@property (readonly) GCExtendedGamepad* profile;

@end

NS_ASSUME_NONNULL_END