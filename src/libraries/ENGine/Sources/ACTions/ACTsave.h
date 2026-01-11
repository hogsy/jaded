/*$T ACTsave.h GC!1.71 02/24/00 14:25:18 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __ACTSAVE_H__
#define __ACTSAVE_H__

#pragma once
#include "BASe/BAStypes.h"

#include "ENGine/Sources/ACTions/ACTstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern void		ACT_LoadSetAction(ACT_st_ActionKit *, BIG_INDEX, int);
extern void		ACT_NewAction(ACT_st_ActionKit *, char *, int);
extern ULONG    ACT_ul_SaveAction(ACT_st_Action *_pst_Action);
extern ULONG    ACT_ul_SaveActionKit(ACT_st_ActionKit *_pst_ActionKit);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
