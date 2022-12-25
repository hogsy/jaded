// ****************************************
// Visual Desktop Windowing Manager
//
// Used for King Kong Xenon Debugging
//
// By Alexandre David (January 2005)
// ****************************************

#ifndef __Desktop_Native_h__
#define __Desktop_Native_h__

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "BASe/MEMory/MEM.h"

// ***********************************************************************************************************************

namespace Desktop
{

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

const TEngineResID	kiEngineResID_Texture_Native			= 0x620021a7; // King Kong 14
const TFloat		kfFont_Scale_X_Native					= kfFont_AspectRatio;
const TFloat		kfFont_Scale_Y_Native					= 1.0f;

// ***********************************************************************************************************************

const TEngineResID	kiEngineResID_Texture_DesktopText		= 0xa5001716; // POP5 Font
const TFloat		kfFont_Scale_X_DesktopText				= 4.0f;
const TFloat		kfFont_Scale_Y_DesktopText				= 2.5f;

// ***********************************************************************************************************************

const TEngineResID	kiEngineResID_Texture_DesktopGlyphs		= 0xa5001714;
const TFloat		kfFont_Scale_X_DesktopGlyphs			= 2.0f;
const TFloat		kfFont_Scale_Y_DesktopGlyphs			= 4.0f;

// ***********************************************************************************************************************

} // Namespace

// ***********************************************************************************************************************

#endif // __Desktop_Native_h__
