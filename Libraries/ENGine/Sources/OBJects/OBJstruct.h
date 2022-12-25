/*$T OBJstruct.h GC! 1.081 04/20/00 10:04:28 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifdef JADEFUSION
#define CURRENT_GAO_VERSION     7//POPOWARNING
#endif

#ifndef __OBJ_STRUCT__
#define __OBJ_STRUCT__
#include "BASe/BAStypes.h"

#include "GRObject/GROstruct.h"
#include "ENGine/Sources/DYNamics/DYNstruct.h"
#include "ENGine/Sources/OBJects/OBJgrp.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"

#ifdef JADEFUSION
#ifdef	ACTIVE_EDITORS
#include "XenonGraphics/XeSharedDefines.h"
#endif	//ACTIVE_EDITORS
#endif

struct ANI_st_GameObjectAnim_;

#ifdef JADEFUSION
class CCurrentWind;
#endif

#define Active_CloneListe

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#ifndef PSX2_TARGET
#pragma once
#endif
#ifdef ACTIVE_EDITORS

/*
 -----------------------------------------------------------------------------------------------------------------------
    Object position historic
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	OBJ_tdst_Trace_
{
	MATH_tdst_Vector	*dst_Pos;
	LONG				l_Number;
	LONG				l_Delay;
	LONG				l_Counter;
	LONG				l_NbDisplay;
	float				f_ZOffset;
	ULONG				ul_Color;
} OBJ_tdst_Trace;

#endif /* ACTIVE_EDITORS */

/*
 -----------------------------------------------------------------------------------------------------------------------
    Definition of the Bounding volume structures.
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	OBJ_tdst_SingleBV_
{
#ifdef ACTIVE_EDITORS
	struct OBJ_tdst_GameObject_ *pst_GO;
#endif
	MATH_tdst_Vector			st_GMin;
	MATH_tdst_Vector			st_GMax;
	MATH_tdst_Vector			st_GMinIn;
	MATH_tdst_Vector			st_GMaxIn;

} OBJ_tdst_SingleBV;

typedef struct	OBJ_tdst_ComplexBV_
{
#ifdef ACTIVE_EDITORS
	struct OBJ_tdst_GameObject_ *pst_GO;
#endif
	MATH_tdst_Vector			st_GMin;
	MATH_tdst_Vector			st_GMax;
	MATH_tdst_Vector			st_GMinIn;
	MATH_tdst_Vector			st_GMaxIn;

	MATH_tdst_Vector			st_LMin;
	MATH_tdst_Vector			st_LMax;
} OBJ_tdst_ComplexBV;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    Definition of a data referencer

    Note:   A data referencer has a pointer to loaded data and 32 bits of information to locate the unloaded data on
            the disk (or in the cache system) £
            £
            If the bit 0 of the p_LoadedData pointer is not 0, it meens that the pointer is not valid, so the data is
            not loaded. In that case, the 31 left bits can be used for more information about the unloaded data
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	OBJ_tdst_DataRef_
{
	union
	{
		void	*p_LoadedData;
		ULONG	ul_UnloadedDataExt;
	};
	ULONG	ul_UnloadedData;
} OBJ_tdst_DataRef;

/*
 =======================================================================================================================
    Aim:    Returns 1 if a data is loaded into memory
 =======================================================================================================================
 */
_inline_ LONG OBJ_b_DataRef_IsLoaded(OBJ_tdst_DataRef *_pst_DataRef)
{
	return(!(_pst_DataRef->ul_UnloadedDataExt & 1));
}

/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    Definition of a game object hierarchy
 -----------------------------------------------------------------------------------------------------------------------
 */
#if defined(_XBOX) || defined(_M_X86) || defined(_XENON)
typedef struct	OBJ_tdst_Hierarchy_
{
	__declspec(align(16)) MATH_tdst_Matrix			st_LocalMatrix;
	struct OBJ_tdst_GameObject_ *pst_FatherInit;
	struct OBJ_tdst_GameObject_ *pst_Father;
#ifdef USE_DOUBLE_RENDERING
	MATH_tdst_Matrix			st_LocalMatrix_A;
	MATH_tdst_Matrix			st_LocalMatrix_B;
#endif	
}__declspec(align(16)) OBJ_tdst_Hierarchy;
#else
typedef struct	OBJ_tdst_Hierarchy_
{
	struct OBJ_tdst_GameObject_ *pst_FatherInit;
	struct OBJ_tdst_GameObject_ *pst_Father;
	MATH_tdst_Matrix			st_LocalMatrix;
#ifdef USE_DOUBLE_RENDERING
	MATH_tdst_Matrix			st_LocalMatrix_A;
	MATH_tdst_Matrix			st_LocalMatrix_B;
#endif	
}OBJ_tdst_Hierarchy;
#endif
/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    Definition of game object additional matrix
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	OBJ_tdst_Gizmo_
{
	MATH_tdst_Matrix	st_Matrix;
#ifdef ACTIVE_EDITORS
	char				sz_Name[OBJ_C_Gizmo_MaxLenName];
#endif
} OBJ_tdst_Gizmo;

typedef struct	OBJ_tdst_GizmoPtr_
{
	MATH_tdst_Matrix			*pst_Matrix;
	struct OBJ_tdst_GameObject_ *pst_GO;	/* GO that contains gizmo referenced by above pointer */
	LONG						l_MatrixId; /* index of gizmo or -1 for main GO matrix */
} OBJ_tdst_GizmoPtr;

typedef struct	OBJ_tdst_AdditionalMatrix_
{
#ifdef ACTIVE_EDITORS
	struct OBJ_tdst_GameObject_ *pst_GO;
	struct OBJ_tdst_GameObject_ **dpst_EditionGO;
	ULONG						ul_GrpIndex;	/* For Skin Edition */
#endif
	LONG						l_Number;
	union
	{
		OBJ_tdst_Gizmo		*dst_Gizmo;
		OBJ_tdst_GizmoPtr	*dst_GizmoPtr;
	};
} OBJ_tdst_AdditionalMatrix;

/*
 -----------------------------------------------------------------------------------------------------------------------
			ODE Structure
 -----------------------------------------------------------------------------------------------------------------------
 */
#define ODE_INSIDE


#ifdef ODE_INSIDE
#include "ode_jade.h"
#endif

/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    Definition of a game object base data
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	OBJ_tdst_Base_
{
	union
	{
		GRO_tdst_Visu					*pst_Visu;				/* A game object can have either a */
		struct ANI_st_GameObjectAnim_	*pst_GameObjectAnim;	/* Visual or an animation */
	};
#ifdef ODE_INSIDE
	DYN_tdst_Dyna				*pst_Dyna;
	DYN_tdst_ODE				*pst_ODE;
#else
	DYN_tdst_Dyna				*pst_Dyna;
#endif

	OBJ_tdst_Hierarchy			*pst_Hierarchy;
	OBJ_tdst_AdditionalMatrix	*pst_AddMatrix;
    GRO_tdst_Struct             *pst_AddMaterial;
} OBJ_tdst_Base;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Design structure
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	OBJ_tdst_DesignStruct_
{
	int							flags;
	int							i1;
	int							i2;
	float						f1;
	float						f2;
	MATH_tdst_Vector			st_Vec1;
	MATH_tdst_Vector			st_Vec2;
	struct OBJ_tdst_GameObject_ *pst_Perso1;
	struct OBJ_tdst_GameObject_ *pst_Perso2;
	struct WAY_tdst_Network_	*pst_Net1;
	struct WAY_tdst_Network_	*pst_Net2;
	TEXT_tdst_Eval				st_Text1;
	TEXT_tdst_Eval				st_Text2;
	unsigned int				ui_DesignFlags;
	unsigned int				ui_DesignFlagsInit;
	int							i3;
} OBJ_tdst_DesignStruct;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    Definition of a game object extended data
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	OBJ_tdst_Extended_
{
	void						*pst_Col;
	struct AI_tdst_Instance_	*pst_Ai;
	OBJ_tdst_DesignStruct		*pst_Design;
	OBJ_tdst_Group				*pst_Group;
	GRO_tdst_Struct				*pst_Light;
	void						*pst_Msg;
	void						*pst_Links;
	struct MDF_tdst_Modifier_	*pst_Modifiers;
	struct EVE_tdst_Data_		*pst_Events;
	union SND_tdun_Main_		*pst_Sound;
	ULONG						uw_Capacities;
	ULONG						uw_CapacitiesInit;
	UCHAR						uc_AiPrio;	/* AI priority (offset = +32) */
	UCHAR						uc_Blank;
	USHORT						uw_ExtraFlags;
	UCHAR						auc_Sectos[4];
#ifdef JADEFUSION
	CCurrentWind                *po_CurrentWind;
#endif
} OBJ_tdst_Extended;

#ifdef Active_CloneListe
/* Clone Liste chaine*/
/*typedef struct	CloneListeGao_
{
	struct OBJ_tdst_GameObject_* p_Gao;
	struct CloneListeGao_ *p_Next;
}CloneListeGao;*/

typedef struct	LightCloneListeGao_
{
	struct OBJ_tdst_GameObject_* p_Gao;
	struct LightCloneListeGao_ *p_Next;
}LightCloneListeGao;

#endif
/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    Definition of a game object

    Note:   Identity flags: these flags tell what kind of object it is $£
            they are used by the ENG module to know which object to put in which engine objects table (EOT)
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	OBJ_tdst_GameObject_
{
	ULONG	ul_IdentityFlags;			/* Identity flags of the game object */
	ULONG	ul_StatusAndControlFlags;	/* Status and Control flags of the Game Object */
	UCHAR	uc_Secto;					/* Compteur invisibilité gao */
	UCHAR	uc_VisiCoeff;
	USHORT	us_SnP_Ref;					/* Indexes needed for the Collision Detection Algorithm */

	MATH_tdst_Matrix		*pst_GlobalMatrix;			/* Position & orientation of the object */

	void					*pst_BV;
	OBJ_tdst_Base			*pst_Base;
	OBJ_tdst_Extended		*pst_Extended;

	UCHAR					uc_LOD_Vis;					/* % of screen width 255 >= full screen, 0 < 1 pixel */
	UCHAR					uc_LOD_Dist;				/* Index in the distances table */
	UCHAR					uc_DesignFlags;				/* Design Flags */
	char					c_FixFlags;					/* Flags for fix */

/*#ifndef JADEFUSION
#ifdef _DEBUG
	char					*sz_Name;
#endif
#else
#ifndef _FINAL
	char					*sz_Name;
#endif
#endif*/
#if (defined JADEFUSION && !defined _FINAL) || (!defined JADEFUSION && defined _DEBUG)
char					*sz_Name;
#endif

#ifdef ACTIVE_EDITORS
	ULONG					ul_EditorFlags;
	ULONG					ul_AdditionalFlags;			/* local flags, never save these flags */
	struct WOR_tdst_World_	*pst_World;					/* World where there's the object */
	OBJ_tdst_Trace			*pst_Trace;
	ULONG					ul_ObjectModel;
	ULONG					ul_InvisibleObjectIndex;	/* index of invisible object to be displayed */
	ULONG					ul_ForceLODIndex;           /* index to force LOD displayed */
	ULONG					ul_User3;
	ULONG					ul_Sector;					/* Index of Sector of Object */
	ULONG					ul_PrefabKey;				/* key of prefab if one was used to create object */
	ULONG					ul_PrefabObjKey;			/* key of object of prefab if one was used to create object */
    BOOL                    b_DontDisplayDebug;         /* don't display debug info for this GAO.*/

#ifdef JADEFUSION
	struct OBJ_tdst_GameObject_	*apst_IsLightBy[VS_MAX_LIGHTS];	/* array of pointer to all the lights that's affect the GAO */
	ULONG					ul_XeMergeFlags;			// -NOTE- Added a bitfield to know if this GAO's properties 
#endif
#endif


#ifdef _XBOX

	//Tell if a game object as been already drawn
	int playSkinning;
	
	//Tell if a game object is in light or not
	int IsInLight;

#endif


#ifdef PSX2_TARGET
	struct GSP_GO_DataCache_ *DataCacheOptimization;
#ifndef _FINAL_
	u8		LastGlobalsTick;
	u8		LastDrawTick;
	u8		LastEngineTick;
	u32		DrawTick;
	u32		EngineTick;
	u32		NumberOfTris;
#endif	
#endif
#ifdef ACTIVE_EDITORS
	u32		NumberOfTris;
	u32		DrawInfoName;
	u32		EngineTicksNumber;
#endif

	ULONG	ul_MyKey;

#ifdef Active_CloneListe
	//CloneListeGao			*CloneList;// pointeur Clone liste
	struct OBJ_tdst_GameObject_*	p_CloneNextGao;
	LightCloneListeGao		*CloneLightList;
	ULONG					ulMoreCloneInfo; // Philippe add for LOD
#endif
	u8						ucCullingVisibility;

}OBJ_tdst_GameObject;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __OBJ_STRUCT__ */

