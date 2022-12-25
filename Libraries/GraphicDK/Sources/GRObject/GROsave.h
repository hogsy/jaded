/*$T GROsave.h GC!1.39 06/29/99 11:51:46 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GROSAVE_H__
#define __GROSAVE_H__

#include "TABles/TABles.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */

void    GRO_Struct_SaveTable(TAB_tdst_Ptable *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __GROSAVE_H__ */