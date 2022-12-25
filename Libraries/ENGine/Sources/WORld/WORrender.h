/*$T WORrender.h GC!1.39 06/28/99 10:46:06 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __WORRENDER_H__
#define __WORRENDER_H__


#include "GDInterface/GDInterface.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */

void    WOR_Render(WOR_tdst_World *, GDI_tdst_DisplayData *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __WORRENDER_H__ */