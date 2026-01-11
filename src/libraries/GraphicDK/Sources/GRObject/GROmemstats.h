/*$T GROrender.h GC!1.52 11/18/99 11:31:02 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GROMEMSTATS_H__
#define __GROMEMSTATS_H__

#ifdef ACTIVE_EDITORS

#include "BASe/BAStypes.h"
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

void GRO_MemStat_Geo( TAB_tdst_Ptable *, void * );
void GRO_MemStat_Light( TAB_tdst_Ptable *, void * );
void GRO_MemStat_AllGro( TAB_tdst_Ptable * );
void GRO_MemStat_AllMat( TAB_tdst_Ptable * );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /*ACTIVE_EDITORS*/
#endif /*__GROMEMSTATS_H__*/
