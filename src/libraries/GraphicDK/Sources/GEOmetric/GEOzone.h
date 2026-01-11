/*$T GEOzone.h GC!1.5 11/03/99 10:33:37 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __GEO_ZONE__
#define __GEO_ZONE__

#ifndef PSX2_TARGET
#pragma once
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#define GEO_Cul_GraphicZDx		0x0
#define GEO_Cul_GraphicCob		0x1


/*
 ---------------------------------------------------------------------------------------------------
    Structure GraphicZone
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  GEO_tdst_GraphicZone_
{
    void				        *pv_Data;
    struct OBJ_tdst_GameObject_ *pst_GO;
	UCHAR						uc_Type;						
} GEO_tdst_GraphicZone;

/*
 ---------------------------------------------------------------------------------------------------
    Strucuture GraphicZones
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  GEO_tdst_GraphicZones_
{
    ULONG                   ul_Max;
    ULONG                   ul_Next;
    GEO_tdst_GraphicZone    *dpst_GraphicZone;
} GEO_tdst_GraphicZones;

/*
 ===================================================================================================
    Functions
 ===================================================================================================
 */
void                    GEO_Zone_Display(struct GDI_tdst_DisplayData_ *, struct OBJ_tdst_GameObject_ *);
void                    GEO_Zone_Init(GEO_tdst_GraphicZones *);
void                    GEO_Zone_Close(GEO_tdst_GraphicZones *);
void                    GEO_Zone_Clear(GEO_tdst_GraphicZones *);
GEO_tdst_GraphicZone    *GEO_Zone_GetNextZone(GEO_tdst_GraphicZones *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif //__GEO_ZONE__
