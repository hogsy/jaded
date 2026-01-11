/*$T GEOstaticLOD.h GC!1.40 09/08/99 15:10:07 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GEOSTATIC_H__
#define __GEOSTATIC_H__



#include "BASe/BAStypes.h"
#include "GRObject/GROstruct.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 ---------------------------------------------------------------------------------------------------
    Structure
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  GEO_tdst_StaticLOD_
{
    GRO_tdst_Struct st_Id;
    UCHAR           uc_NbLOD;
    UCHAR           uc_Distance;
    UCHAR           auc_EndDistance[6];
    GRO_tdst_Struct *dpst_Id[6];
} GEO_tdst_StaticLOD;

/*
 ===================================================================================================
    Functions
 ===================================================================================================
 */
void                GEO_StaticLOD_AllocContent(GEO_tdst_StaticLOD *);
GEO_tdst_StaticLOD  *GEO_pst_StaticLOD_Create(UCHAR);
void                GEO_StaticLOD_Free(GEO_tdst_StaticLOD *);
void                GEO_StaticLOD_Add(GEO_tdst_StaticLOD *, GRO_tdst_Struct *, UCHAR );
void                GEO_StaticLOD_Replace(GEO_tdst_StaticLOD *, GRO_tdst_Struct *, LONG );
void                GEO_StaticLOD_Delete(GEO_tdst_StaticLOD *, LONG );
LONG                GEO_l_StaticLOD_GetIndex(GEO_tdst_StaticLOD *, UCHAR );
GRO_tdst_Struct     *GEO_pst_StaticLOD_GetLOD(GEO_tdst_StaticLOD *, UCHAR );
void                GEO_StaticLOD_Init(void);

#ifdef ACTIVE_EDITORS
BOOL				GEO_StaticLOD_HasRLI(GEO_tdst_StaticLOD *);
#endif


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __GEOSTATIC_H__ */