/*$T MDFmodifier_GEO.h GC! 1.081 11/14/00 12:12:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_GEO_H__
#define __MDFMODIFIER_GEO_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Modifier const
 ***********************************************************************************************************************
 */

/* Wave your body modifier flags */
#define GEO_C_OTCF_X				0x00000001
#define GEO_C_OTCF_Y				0x00000002
#define GEO_C_OTCF_Z				0x00000004
#define GEO_C_OTCF_Planar			0x00000008
#define GEO_C_OTCF_UseAlphaOfRLI	0x00000010
#define GEO_C_OTCF_InvertAlpha		0x00000020

/* symetrie modifier flags */
#define GEO_C_Symetrie_X			0x00000001
#define GEO_C_Symetrie_Y			0x00000002
#define GEO_C_Symetrie_Z			0x00000004

/* crush modifier flags */
#define GEO_C_Crush_X			0x00000001
#define GEO_C_Crush_Y			0x00000002
#define GEO_C_Crush_Z			0x00000004
#define GEO_C_Crush_NegAxis		0x00000008

/* RLI Carte flags */
#define GEO_C_RLIOp_None		0
#define GEO_C_RLIOp_Set			1
#define GEO_C_RLIOp_Add			2
#define GEO_C_RLIOp_Sub			3

/* General for GEO modifiers */
#define GEO_C_ModifierFlags_CanBeApplyInGeom	0x00010000
#define GEO_C_ModifierFlags_ApplyInGeom			0x00020000

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

typedef struct	GEO_tdst_ModifierSnap_OnePoint_
{
	ULONG				ul_IndexSrc;
	ULONG				ul_IndexTgt;
	MATH_tdst_Vector	st_Point;
} GEO_tdst_ModifierSnap_OnePoint;

typedef struct	GEO_tdst_ModifierSnap_
{
	void							*p_GameObject;
	ULONG							ul_NbPoints;
	GEO_tdst_ModifierSnap_OnePoint	*pst_Point;
} GEO_tdst_ModifierSnap;

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ondule Ton corps
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct	GEO_tdst_ModifierOnduleTonCorps_
{
	ULONG	ul_Flags;
	float	f_Angle;
	float	f_Amplitude;
	float	f_Factor;
	float	f_Delta;
} GEO_tdst_ModifierOnduleTonCorps;

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Vertex Field of Force
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
typedef struct	GEO_tdst_ModifierPertub_
{
	ULONG	ul_Flags;
} GEO_tdst_ModifierPerturb;


/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Symétrie
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct  GEO_tdst_ModifierSymetrie_
{
    ULONG               ul_Flags;
    float               f_Offset;
} GEO_tdst_ModifierSymetrie;


/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Morphing
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#ifdef JADEFUSION
typedef struct  GEO_tdst_ModifierMorphing_TangentSpaceBasis_
{
    LONG                    l_NbQuaternions;
    LONG                    *dl_Index;
    MATH_tdst_Quaternion    *dst_Quaternions;
} GEO_tdst_ModifierMorphing_TangentSpaceBasis;
#endif

typedef struct	GEO_tdst_ModifierMorphing_Data_
{
	LONG				l_NbVector;
	LONG				*dl_Index;
	MATH_tdst_Vector	*dst_Vector;

#ifdef JADEFUSION
    LONG                    l_NbQuaternions;
    LONG                    *dl_IndexQuat;
    MATH_tdst_Quaternion    *dst_Quaternions;
#endif

#ifdef ACTIVE_EDITORS
	char				sz_Name[64];
#endif
} GEO_tdst_ModifierMorphing_Data;

typedef struct	GEO_tdst_ModifierMorphing_Channel_
{
	LONG	l_NbData;
	LONG	*dl_DataIndex;
	float	f_Blend;
	float	f_ChannelBlend;

#ifdef JADEFUSION
    GEO_tdst_ModifierMorphing_Data_ * dst_CurrMorphChannel1;
    GEO_tdst_ModifierMorphing_Data_ * dst_CurrMorphChannel2;
    float	                          f_CurrChannelBlend;
#endif

#ifdef ACTIVE_EDITORS
	char	sz_Name[64];
#endif
#ifdef USE_DOUBLE_RENDERING	
	float	f_Blend_0;
	float	f_ChannelBlend_0;
	float	f_Blend_1;
	float	f_ChannelBlend_1;
#endif
} GEO_tdst_ModifierMorphing_Channel;

typedef struct	GEO_tdst_ModifierMorphing_
{
	ULONG								ul_Flags;
	LONG								l_NbMorphData;
	GEO_tdst_ModifierMorphing_Data		*dst_MorphData;
	LONG								l_NbChannel;
	GEO_tdst_ModifierMorphing_Channel	*dst_MorphChannel;
	LONG								l_NbPoints;
#ifdef ACTIVE_EDITORS
	struct OBJ_tdst_GameObject_			*pst_ForEditionOnly;
	ULONG								ul_Wnd;
#endif
} GEO_tdst_ModifierMorphing;

#ifdef JADEFUSION
struct GEO_tdst_ModifierMorphing_ComputingBuffersElement
{
    MATH_tdst_Vector  dst_Normals[SOFT_Cul_ComputingBufferSize];
    MATH_tdst_Vector  dst_Tangents[SOFT_Cul_ComputingBufferSize];
};

class GEO_tdst_ModifierMorphing_ComputingBuffers
{
public:
    GEO_tdst_ModifierMorphing_ComputingBuffers(){}
  
    GEO_Vertex *    GetNormals      ( );
    GEO_Vertex *    GetTangents     ( );
    
public:
    MATH_tdst_Quaternion    dst_Quaternions[SOFT_Cul_ComputingBufferSize];
    BYTE                    dst_NbrRotations[SOFT_Cul_ComputingBufferSize];
    MATH_tdst_Vector        dst_Normals[SOFT_Cul_ComputingBufferSize];
    MATH_tdst_Vector        dst_Tangents[SOFT_Cul_ComputingBufferSize];
};

extern GEO_tdst_ModifierMorphing_ComputingBuffers g_oMorphComputingBuffers;
#endif

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Transformed points
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
typedef struct GEO_tdst_ModifierStoreTransformedPoint_
{
    ULONG       ul_NbPoints;
    int         i_PointsAreValid;
    GEO_Vertex  *dst_Points;
} GEO_tdst_ModifierStoreTransformedPoint;

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Crush points
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
typedef struct GEO_tdst_ModifierCrush_
{
	ULONG	ul_Flags;
	float	f_Min;
	float	f_Max;
	float	f_LevelInit;
	float	f_Level;
} GEO_tdst_ModifierCrush;

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    RLI Carte
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
typedef struct GEO_tdst_ModifierRLICarte_
{
	UCHAR uc_Flags;
	UCHAR uc_Op;
	UCHAR uc_InternalFlags;
	UCHAR uc_Dummy2;
	ULONG aul_ColorInit[64];
	ULONG aul_Color[64];
	ULONG ul_NbPoints;
	UCHAR *duc_PtGroup;
	ULONG *dul_Colors;
} GEO_tdst_ModifierRLICarte;


/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern BOOL							GEO_Modifier_CanBeApply(struct OBJ_tdst_GameObject_ *, char *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void							GEO_ModifierSnap_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void							GEO_ModifierSnap_Destroy(MDF_tdst_Modifier *);
extern void							GEO_ModifierSnap_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void							GEO_ModifierSnap_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern ULONG						GEO_ul_ModifierSnap_Load(MDF_tdst_Modifier *, char *);
extern void							GEO_ModifierSnap_Save(MDF_tdst_Modifier *);
extern void							GEO_ModifierSnap_ComputeWithTresh
									(
										struct OBJ_tdst_GameObject_ *,
										struct OBJ_tdst_GameObject_ *,
										float,
										MDF_tdst_Modifier *
									);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void							GEO_ModifierOnduleTonCorps_Create
									(
										struct OBJ_tdst_GameObject_ *,
										MDF_tdst_Modifier *,
										void *
									);
extern void							GEO_ModifierOnduleTonCorps_Destroy(MDF_tdst_Modifier *);
extern void							GEO_ModifierOnduleTonCorps_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void							GEO_ModifierOnduleTonCorps_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern ULONG						GEO_ul_ModifierOnduleTonCorps_Load(MDF_tdst_Modifier *, char *);
extern void							GEO_ModifierOnduleTonCorps_Save(MDF_tdst_Modifier *);
extern int							GEO_ModifierOnduleTonCorps_Copy(MDF_tdst_Modifier *, MDF_tdst_Modifier *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void GEO_ModifierPerturb_Create(OBJ_tdst_GameObject *, MDF_tdst_Modifier *, void *);
extern void GEO_ModifierPerturb_Destroy(MDF_tdst_Modifier *);
extern void GEO_ModifierPerturb_Apply(MDF_tdst_Modifier	*, GEO_tdst_Object * );
extern void GEO_ModifierPerturb_Unapply( MDF_tdst_Modifier *, GEO_tdst_Object *);
extern ULONG GEO_ul_ModifierPerturb_Load(MDF_tdst_Modifier *, char *);
extern void GEO_ModifierPerturb_Save(MDF_tdst_Modifier *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void							GEO_ModifierSymetrie_Create
									(
										struct OBJ_tdst_GameObject_ *,
										MDF_tdst_Modifier *,
										void *
									);
extern void							GEO_ModifierSymetrie_Destroy(MDF_tdst_Modifier *);
extern void							GEO_ModifierSymetrie_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void							GEO_ModifierSymetrie_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern ULONG						GEO_ul_ModifierSymetrie_Load(MDF_tdst_Modifier *, char *);
extern void							GEO_ModifierSymetrie_Save(MDF_tdst_Modifier *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void							GEO_ModifierMorphing_Create
									(
										struct OBJ_tdst_GameObject_ *,
										MDF_tdst_Modifier *,
										void *
									);
									
									
#ifdef USE_DOUBLE_RENDERING	
void 								GEO_ModifierMorphing_Interpolate
									(
										struct MDF_tdst_Modifier_ * , 
										u_int mode , 
										float fInterpolatedValue
									);
#endif
									
extern void							GEO_ModifierMorphing_Destroy(MDF_tdst_Modifier *);
extern void							GEO_ModifierMorphing_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void							GEO_ModifierMorphing_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern ULONG						GEO_ul_ModifierMorphing_Load(MDF_tdst_Modifier *, char *);
extern void							GEO_ModifierMorphing_Save(MDF_tdst_Modifier *);
extern void                         GEO_ModifierMorphing_Swap( GEO_tdst_ModifierMorphing *, int , int , BOOL );
extern void                         GEO_ModifierMorphing_Move( GEO_tdst_ModifierMorphing *, int , int , BOOL );
extern LONG							GEO_l_ModifierMorphing_AddData
									(
										MDF_tdst_Modifier *,
										struct OBJ_tdst_GameObject_ *,
										LONG, BOOL, BOOL
									);
extern LONG							GEO_l_ModifierMorphing_AddChannel(MDF_tdst_Modifier *);
extern LONG							GEO_l_ModifierMorphing_AddChannelData(MDF_tdst_Modifier *, int);
extern GEO_tdst_ModifierMorphing	*GEO_pst_ModifierMorphing_Get(struct OBJ_tdst_GameObject_ *);
extern void                         GEO_ModifierMorphing_UpdateWhenGeoChange( GEO_tdst_ModifierMorphing *, struct WOR_tdst_Update_RLI_ *, LONG );
extern GEO_tdst_ModifierMorphing_Data *GEO_ModifierMorphing_DuplicateData( GEO_tdst_ModifierMorphing * );
extern void                         GEO_ModifierMorphing_DestroyData( GEO_tdst_ModifierMorphing_Data *, LONG );
extern void							GEO_ModifierMorphing_GetVertexMorphVector( GEO_tdst_ModifierMorphing *, long, MATH_tdst_Vector * );
#ifdef JADEFUSION
extern BOOL							GEO_ModifierMorphing_HasDummyChannels( GEO_tdst_ModifierMorphing * );
#endif
extern BOOL							GEO_b_ModifierMorphing_CheckDummyChannels( GEO_tdst_ModifierMorphing * );
extern void							GEO_ModifierMorphing_DeleteDummyChannels( GEO_tdst_ModifierMorphing * );
extern void							GEO_ModifierMorphing_CreateDummyChannels( GEO_tdst_ModifierMorphing * );

#ifdef JADEFUSION
//xenon
extern void                         GEO_ModifierMorphing_RecomputeAllTangentSpaceData( MDF_tdst_Modifier * );
#endif
/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void							GEO_ModifierSTP_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void							GEO_ModifierSTP_Destroy(MDF_tdst_Modifier *);
extern void							GEO_ModifierSTP_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void							GEO_ModifierSTP_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern ULONG						GEO_ul_ModifierSTP_Load(MDF_tdst_Modifier *, char *);
extern void							GEO_ModifierSTP_Save(MDF_tdst_Modifier *);


/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void							GEO_ModifierCrush_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void							GEO_ModifierCrush_Destroy(MDF_tdst_Modifier *);
extern void							GEO_ModifierCrush_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void							GEO_ModifierCrush_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern ULONG						GEO_ul_ModifierCrush_Load(MDF_tdst_Modifier *, char *);
extern void							GEO_ModifierCrush_Reinit(MDF_tdst_Modifier *);
extern void							GEO_ModifierCrush_Save(MDF_tdst_Modifier *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void							GEO_ModifierRLICarte_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void							GEO_ModifierRLICarte_Destroy(MDF_tdst_Modifier *);
extern void							GEO_ModifierRLICarte_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void							GEO_ModifierRLICarte_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern ULONG						GEO_ul_ModifierRLICarte_Load(MDF_tdst_Modifier *, char *);
extern void							GEO_ModifierRLICarte_Reinit(MDF_tdst_Modifier *);
extern void							GEO_ModifierRLICarte_Save(MDF_tdst_Modifier *);

extern void                         GEO_ModifierRLICarte_UpdateWhenGeoChange( MDF_tdst_Modifier *, struct WOR_tdst_Update_RLI_ *, LONG );


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GEOMODIFIER_H */
