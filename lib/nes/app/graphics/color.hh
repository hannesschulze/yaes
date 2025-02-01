#pragma once

#include "nes/common/types.hh"

namespace nes::app
{
	/// One of the predefined colors in the color palette used for the UI.
	enum class color : u8
	{
		transparent,          ///< No color.
		background_primary,   ///< The primary background color.
		background_secondary, ///< A darker version of the background color.
		foreground_primary,   ///< The primary foreground color.
		foreground_secondary, ///< A more subtle version of the foreground color.
		accent_primary,       ///< The primary accent color.
		accent_secondary,     ///< A darker version of the accent color.
		fixed_white,          ///< Just white.
		fixed_gray,           ///< Just gray.
		fixed_black,          ///< Just black.
	};
} // namespace nes::app