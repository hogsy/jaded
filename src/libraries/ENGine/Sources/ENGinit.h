/*$T ENGinit.h GC!1.40 07/16/99 11:14:10 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

/*$4
 ***************************************************************************************************
    Fonctions.
 ***************************************************************************************************
 */

void    ENG_InitApplication(void);
void    ENG_CloseApplication(void);
void    ENG_InitEngine(void);
void    ENG_CloseEngine(void);
void    ENG_Init(void);
void    ENG_Desinit(void);

void	ENG_ReinitOneObject(OBJ_tdst_GameObject *, int);
void    ENG_InitOneWorld(WOR_tdst_World *);
void    ENG_ReinitOneWorld(WOR_tdst_World *, int);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
