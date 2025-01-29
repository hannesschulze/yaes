#pragma once

#import "controller-adapter.hh"

#import <GameController/GameController.h>

NS_ASSUME_NONNULL_BEGIN

@interface ControllerAdapterKeyboard : NSObject <ControllerAdapter>

@property (readonly, nullable) GCKeyboard* keyboard;

@end

NS_ASSUME_NONNULL_END