/*$T GEOmodifier.h GC!1.71 02/04/00 11:49:42 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GEOMODIFIER_H__
#define __GEOMODIFIER_H__


#include "BASe/BAStypes.h"

#include "MATHs/MATHstruct.h"
#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Modifier const
 ***********************************************************************************************************************
 */

/* Modifier type */
#define GEO_C_Modifier_Snap             0
#define GEO_C_Modifier_OnduleTonCorps   1
#define GEO_C_Modifier_Shadow0          2
#define GEO_C_Modifier_LegLink          3

#define GEO_C_Modifier_Number           4

/* Modifier general flags */
#define GEO_C_ModifierFlags_CanBeApplyInGeom    0x01000000
#define GEO_C_ModifierFlags_ApplyInGeom         0x02000000

/* Wave your body modifier flags */
#define GEO_C_OTCF_X                0x00000001
#define GEO_C_OTCF_Y                0x00000002
#define GEO_C_OTCF_Z                0x00000004
#define GEO_C_OTCF_Planar           0x00000008
#define GEO_C_OTCF_UseAlphaOfRLI    0x00000010
#define GEO_C_OTCF_InvertAlpha      0x00000020

#define GEO_C_OTCF_NothingComputed  0x80000000

/*$4
 ***********************************************************************************************************************
    Modifier desc
 ***********************************************************************************************************************
 */

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Snap
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct  GEO_tdst_ModifierSnap_OnePoint_
{
    ULONG               ul_IndexSrc;
    ULONG               ul_IndexTgt;
    MATH_tdst_Vector    st_Point;
} GEO_tdst_ModifierSnap_OnePoint;

typedef struct  GEO_tdst_ModifierSnap_
{
    void                            *p_GameObject;
    ULONG                           ul_NbPoints;
    GEO_tdst_ModifierSnap_OnePoint  *pst_Point;
} GEO_tdst_ModifierSnap;

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ondule Ton corps
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct  GEO_tdst_ModifierOnduleTonCorps_
{
    ULONG               ul_Flags;
    float               f_Angle;
    float               f_Amplitude;
    float               f_Factor;
    float               f_Delta;
    ULONG               ul_NbPoints;
    MATH_tdst_Vector    *pst_Point;
} GEO_tdst_ModifierOnduleTonCorps;

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ondule Ton corps
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct  GEO_tdst_ModifierSymetrie_
{
    ULONG               ul_Flags;
    float               f_Offset;
} GEO_tdst_ModifierSymetrie;


/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Shadow level 0
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct  GEO_tdst_ModifierShadow0_
{
    void    *p_GameObject;
    float   f_Size;
    float   f_Dist;
} GEO_tdst_ModifierShadow0;

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Shadow level 0
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct  GEO_tdst_ModifierLegLink_
{
    void    *p_GAOA;
    void    *p_GAOB;
    void    *p_GAOC;
	float	f_AB;
	float	f_BC;
} GEO_tdst_ModifierLegLink;

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    General
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct  GEO_tdst_ModifierInterface_
{
    ULONG   ul_Type;
    void (*pfnv_Create) (struct GEO_tdst_Modifier_ *, void *);
    void (*pfnv_Destroy) (struct GEO_tdst_Modifier_ *);
    void (*pfnv_Apply) (struct GEO_tdst_Modifier_ *, struct GEO_tdst_Object_ *, struct GDI_tdst_DisplayData_ *);
    void (*pfnv_Unapply) (struct GEO_tdst_Modifier_ *, struct GEO_tdst_Object_ *, struct GDI_tdst_DisplayData_ *);
    void (*pfnv_ApplyInGeom) (struct GEO_tdst_Modifier_ *, struct GEO_tdst_Object_ *, struct GDI_tdst_DisplayData_ *);
    ULONG (*pfnul_Load) (struct GEO_tdst_Modifier_ *, char *);

#ifdef ACTIVE_EDITORS
    void (*pfnv_Save) (struct GEO_tdst_Modifier_ *);
#endif
}
GEO_tdst_ModifierInterface;

typedef struct  GEO_tdst_Modifier_
{
    GEO_tdst_ModifierInterface  *i;
    ULONG                       ul_Flags;
    void                        *p_Data;
    struct GEO_tdst_Modifier_   *pst_Next;
    struct GEO_tdst_Modifier_   *pst_Prev;

#ifdef ACTIVE_EDITORS
	struct OBJ_tdst_GameObject_ *pst_GO;
#endif
} GEO_tdst_Modifier;

/*$4
 ***********************************************************************************************************************
    Modifier globals
 ***********************************************************************************************************************
 */

extern GEO_tdst_ModifierInterface   GEO_gast_ModifierInterface[GEO_C_Modifier_Number];
#ifdef ACTIVE_EDITORS
extern char                         *GEO_gasz_ModifierName[GEO_C_Modifier_Number];
#endif

/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

/*$3
 =======================================================================================================================
    Snap
 =======================================================================================================================
 */

#ifdef ACTIVE_EDITORS
void                                GEO_ModifierSnap_ComputeWithTresh
                                    (
                                        struct OBJ_tdst_GameObject_ *,
                                        struct OBJ_tdst_GameObject_ *,
                                        float,
                                        GEO_tdst_Modifier *
                                    );
#endif /* ACTIVE_EDITORS */

/*$3
 =======================================================================================================================
    General
 =======================================================================================================================
 */

void                                GEO_Modifier_Init(void);
GEO_tdst_Modifier                   *GEO_pst_Modifier_Create(int, void *);
void                                GEO_Modifier_Destroy(GEO_tdst_Modifier *);
ULONG                               GEO_ul_Modifier_Load(GEO_tdst_Modifier **, char *, int, void *);

#ifdef ACTIVE_EDITORS
void                                GEO_Modifier_AddToGameObject(struct OBJ_tdst_GameObject_ *, GEO_tdst_Modifier *);
void                                GEO_Modifier_DelInGameObject(struct OBJ_tdst_GameObject_ *, GEO_tdst_Modifier *);

void                                GEO_Modifier_ReplaceInGameObject
                                    (
                                        struct OBJ_tdst_GameObject_ *,
                                        GEO_tdst_Modifier *,
                                        GEO_tdst_Modifier *
                                    );

void                                GEO_Modifier_Save(GEO_tdst_Modifier *, int);

#endif
#ifdef __cplusplus
}
#endif
#endif /* __GEOMODIFIER_H */
