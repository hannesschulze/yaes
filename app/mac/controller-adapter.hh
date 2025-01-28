#pragma once

#import "nes/util/button-mask.hh"
#import <Foundation/Foundation.h>

/// Adapts inputs from a GCPhysicalInputProfile to a NES button mask.
@protocol ControllerAdapter <NSObject>

- (nes::button_mask)readPressedButtons;

@end