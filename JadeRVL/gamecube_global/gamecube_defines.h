//------------------------------------------------------------------------
//
// (C) Copyright 2000 Ubisoft
//
// Author       Dany Joannette
// Date         01 March 2001
//
//
//------------------------------------------------------------------------

#ifndef GAMECUBERVL_DEFINES_H
#define GAMECUBERVL_DEFINES_H

#define ENABLE_ERR_MSG

#define _RVL
#define _GC2REVO
#define _GAMECUBE
#define HW2 
#define BUG_CLR_LOAD_DLY 
#define BUG_XF_STALL
#define TRK_INTEGRATION 
#define MARLIN DI 
#define EPPC 
#define ORCA 
#define GEKKO 
#define GX_REV 2 
#define GXFIFO_SERIAL 
#define __HWSIM 0 

#ifndef _DEBUG
//# define _FINAL_
#else // _DEBUG
# define GC_ENABLE_SOUND_PROFILER
#endif // _DEBUG

#define NO_BINK
#define GC2RVL_NO_ARAM

#include "gamecube_utils.h"

#define LOA_ALIGNED_LOAD

#endif // GAMECUBERVL_DEFINES_H
