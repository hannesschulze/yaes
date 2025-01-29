#pragma once

#import "nes/sys/types/button-mask.hh"

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/// Adapts inputs from a GCPhysicalInputProfile to a NES button mask.
@protocol ControllerAdapter <NSObject>

- (nes::button_mask)readPressedButtons;

@end

NS_ASSUME_NONNULL_END