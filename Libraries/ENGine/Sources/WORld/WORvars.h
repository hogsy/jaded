/*$T WORvars.h GC!1.41 08/02/99 13:19:22 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Variables of the world module
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/* The universe of the game */
extern WOR_tdst_Universe    WOR_gst_Universe;

/* The Table of worlds */
extern WOR_tdst_World       WOR_gst_World[WOR_C_MaxNbWorlds];

/* The world curently handled by the engine */
extern WOR_tdst_World       *WOR_gpst_CurrentWorld;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
