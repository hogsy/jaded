/*$T ENGcall.h GC!1.32 05/20/99 10:47:32 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#include "GDInterface/GDInterface.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Fonctions.
 ***************************************************************************************************
 */

void    ENG_OneWorldEngineCall(WOR_tdst_World *);
void    ENG_EngineCall(void);
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
