/*$T WORstruct.h GC! 1.092 09/01/00 17:32:52 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$F
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Définition des structures liées à l'univers, au monde
	Structures
		WOR_tdst_Universe
		WOR_tdst_World
		WOR_tdst_View
		WOR_tdst_Activator
		WOR_tdst_Activation
		WOR_tdst_WorldDisplay
		WOR_tdst_Visibility
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef __WOR_STRUCT_H__
#define __WOR_STRUCT_H__

#ifdef __cplusplus
#include <utility>
#include <vector>
#endif

#ifndef PSX2_TARGET
#pragma once
#endif
#include "BASe/BAStypes.h"
#include "TABles/TABles.h"
#include "TEXture/TEXstruct.h"
#include "ENGine/Sources/WORld/WORconst.h"
#include "ENGine/Sources/WORld/WORsecto.h"
#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/EOT/EOTstruct.h"
#include "AIinterp/Sources/AIstruct.h"
#include "ENGine/Sources/WAYs/WAYstruct.h"
#include "ENGine/Sources/INTersection/INTstruct.h"
#include "TIMer/PROfiler/PROdefs.h"
#include "TIMer/TIM.h"

#ifdef ODE_INSIDE
#include "../Extern/ode/include/ode/ode.h"
#endif


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Different constant to reinit a world
 -----------------------------------------------------------------------------------------------------------------------
 */

#define UNI_Cuc_InitInPause		20
#define UNI_Cuc_TotalInit		15
#define UNI_Cuc_AfterLoadWorld	14
#define UNI_Cuc_ForDuplicate	13
#define UNI_Cuc_Reset			0xFE
#define UNI_Cuc_Ready			0xFF

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Constant for World structures version
 -----------------------------------------------------------------------------------------------------------------------
 */
#ifdef JADEFUSION
#define WOR_Cl_WorldStructVersion	13
#else
#define WOR_Cl_WorldStructVersion	4
#endif
/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim: Structure Universe Note: The universe is the higest level structure in the universe decription A universe is a
    set of worlds Description of the worlds flags: £
    Bit 0: Displayed/Undisplayed £
    Bit 1: Active/Inactive £
    Bit 2: Loaded/Unloaded £
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	WOR_tdst_Universe_
{
	AI_tdst_Instance	*pst_AI;			/* Global AI of universe */
	TAB_tdst_PFtable	st_WorldsTable;		/* The table of worlds: The pointers point to worlds and the flags are used
											 * * to tell if a world is displayed or not, active or not, loaded or
											 * not... */
	UCHAR				uc_CurrentStatus;	/* Running status */
	UCHAR				uc_Dummy[3];		/* Align */
} WOR_tdst_Universe;

/*
 -----------------------------------------------------------------------------------------------------------------------
    When on obj is killed, it can reset a long value somewhere
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	WOR_tdst_DelObj_
{
	OBJ_tdst_GameObject *pst_GAO;	/* Deleted GAO */
	void				*p_Data;	/* Data to reinit when GAO is killed */
} WOR_tdst_DelObj;

/*
 -----------------------------------------------------------------------------------------------------------------------
    When on obj is killed, it can reset a long value somewhere
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct WOR_tdst_ShowVector_
{
	char				c_Local;
	char				c_Vector;
	char				c_Remember;
	char				c_Dummy;
	
	MATH_tdst_Vector	*pst_Vector;
	OBJ_tdst_GameObject *pst_Gao;
	char				*sz_Name;
	MATH_tdst_Vector	st_RememberSrc[ 32 ];
	MATH_tdst_Vector	st_RememberTgt[ 32 ];
	ULONG				ul_Color;
} WOR_tdst_ShowVector;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim: Structure world Note: The world is a set of objects that can interact £
    All the game objects of a world (loaded or not, activ or not) are here in the st_AllWorldObjects table £
    st_AllWorldObjects is a PFtable of references (see OBJ_tdst_DataRef) £
    st_ActivObjects is a table of pointers and flags. The pointers point to the activ objects, the flags are a copy of
    the identity flags of the object. £
    The number of activ objects can be found in the st_ActivObjects itself.
 -----------------------------------------------------------------------------------------------------------------------
 */
#define WOR_Cte_DbgVectorRender	1000
#define WOR_Cte_DbgCircleRender	100
#define WOR_Cte_DbgGeoForm		100
#define WOR_Cte_DbgShowVector	32

#define WOR_Cte_DbgGFType_Sphere	0
#define WOR_Cte_DbgGFType_Circle	1
#define WOR_Cte_DbgGFType_Disque	2
#define WOR_Cte_DbgGFType_Square	3
#define WOR_Cte_DbgGFType_Box		4
#define WOR_Cte_DbgGFType_Triangle	5
#define WOR_Cte_DbgGFType_Cone2D	6
#define WOR_Cte_DbgGFType_Cone3D	7
#define WOR_Cte_DbgGFType_Cylinder	8
#define WOR_Cte_DbgGFType_2DRect	9


#ifdef ACTIVE_EDITORS
typedef struct WOR_SelectionGroup_
{
	unsigned char Name[256];
	unsigned long ulNumberOfKey;
	unsigned long *BigKeyListe;
} WOR_SelectionGroup;
#endif

#ifdef __cplusplus
typedef std::vector< OBJ_tdst_GameObject * > WOR_World_LightsVector;
#endif

struct WOR_World_LightsVectorCRef;
typedef struct WOR_World_LightsVectorCRef WOR_World_LightsVectorCRef; /**< Opaque reference to a WOR_World_LightsVector */

typedef struct	WOR_tdst_World_
{
	ULONG						ul_NbTotalGameObjects;					/* Total number of objects in the world */
	ULONG						ul_NbViews;								/* Number of world views */

	BOOL						b_ForceVisibilityRefresh;
	BOOL						b_ForceActivationRefresh;
	BOOL						b_ForceBVRefresh;
	BOOL						b_Dummy;


	TAB_tdst_PFtable			st_AllWorldObjects;						/* Table of all objects (loaded or not) */
	TAB_tdst_Ptable				st_GraphicObjectsTable;					/* Table of all Graphic object reference */
	TAB_tdst_Ptable				st_GraphicMaterialsTable;				/* Table of all graphic materials reference */
	INT_tdst_SnP				*pst_SnP;								/* The sweep and prune structure */

	struct WOR_tdst_View_		*pst_View;								/* The table of world views */
	struct WOR_tdst_View_		*pst_CurrentView;						/* Current view treated */

	TAB_tdst_PFtable			st_VisibleObjects;						/* Visible objects. Updated by the visibility system */
	TAB_tdst_PFtable			st_ActivObjects;						/* The table pointers on activ objects */

	WOR_World_LightsVectorCRef *st_Lights; /**< Objects that have a light (or are a light) */

	EOT_tdst_SetOfEOT			st_EOT;									/* All the engine objects tables of the world */

	ULONG						ul_AmbientColor;						/* Ambient color use for all world */
	ULONG						ul_BackgroundColor;						/* Background color use for all world */

	WAY_tdst_AllNetworks		*pst_AllNetworks;						/* All networks for that world */

	char						sz_Name[60];
	struct GRID_tdst_World_		*pst_Grid;								/* Grid associated with that world */
	struct GRID_tdst_World_		*pst_Grid1;								/* Grid associated with that world */
	ULONG						ul_AmbientColor2;
	MATH_tdst_Matrix			st_CameraPosSave;						/* Save position of camera */
	ULONG						ul_NbReports;							/* Number of Collisions reports */
	OBJ_tdst_GameObject			*apst_CrossObjects[COL_Cul_MaxCrossed]; /* Array of GameObjects crossed by the last Ray */
	ULONG						ul_NbCrossed;							/* Number of GameObjects crossed by the last Ray */
	COL_tdst_Report				ast_Reports[COL_Cul_MaxNbOfCollision];	/* Collisions reports */
	COL_tdst_RayInfo			st_RayInfo;								/* Info saved after last Ray cast. */
	ULONG						ul_ProcessCounterAI;					/* Counter for hierarchy */
	ULONG						ul_ProcessCounterDyn;					/* Counter for hierarchy */
	ULONG						ul_ProcessCounterHie;					/* Counter for hierarchy */

	struct GFX_tdst_List_		*pst_GFX;								/* graphic FX */
    struct GFX_tdst_List_		*pst_GFXInterface;						/* graphic FX for interface (drawn after a ZClear) */

	int							i_NumDelObj;							/* Number of deleted infos */
	WOR_tdst_DelObj				ast_DelObj[MAX_DEL_OBJ];				/* Deleted info */

	int							i_LODCut;								/* Cut LOD for render */

	BIG_KEY						h_WorldKey;								/* The key of the .wol/.wow */

	WOR_tdst_Secto				ast_AllSectos[WOR_C_MaxSecto];			/* Definition of all sectors */
	ULONG						ul_CurrentSector;						/* Secteur courant */
	MATH_tdst_Vector			st_SectorLastPos;						/* Last pos of sector reference */

#ifdef JADEFUSION
	OBJ_tdst_GameObject			*pSPG2Light;

    ULONG                       ul_XeDiffuseColor;                       /* Xenon global diffuse color */
    ULONG                       ul_XeSpecularColor;                      /* Xenon global specular color */
    float                       f_XeSpecularShiny;                       /* Xenon global specular shininess */
    float                       f_XeSpecularStrength;                    /* Xenon global specular strength */
	float                       f_XeMipMapLODBias;                      /* Xenon mipmap LOD bias */
	float                       f_XeRLIScale;                           /* Xenon RLI Scale */
	float                       f_XeRLIOffset;                          /* Xenon RLI Offset */
	float                       f_XeGaussianStrength;                   /* Xenon: Controls Gaussian blur on shadows */
	float						f_XeDryDiffuseFactor;
	float						f_XeWetDiffuseFactor;
	float						f_XeDrySpecularBoost;
	float						f_XeWetSpecularBoost;
	float						f_XeRainEffectDelay;
	float						f_XeRainEffectDryDelay;

    float                       f_XeGlowLuminosityMin;                  /* Xenon Glow Luminosity Min */
    float                       f_XeGlowLuminosityMax;                  /* Xenon Glow Luminosity Max */
    float                       f_XeGlowIntensity;                      /* Xenon Glow Intensity */
    ULONG                       ul_XeGlowColor;                         /* Xenon Glow Color */
    float                       f_XeGlowZNear;                          /* Xenon Glow Z Near */
    float                       f_XeGlowZFar;                           /* Xenon Glow Z Far */

    BOOL                        b_XeMaterialLODEnable;
    float                       f_XeMaterialLODNear;
    float                       f_XeMaterialLODFar;
    BOOL                        b_XeMaterialLODDetailEnable;
    float                       f_XeMaterialLODDetailNear;
    float                       f_XeMaterialLODDetailFar;

    float                       f_XeSaturation;
    MATH_tdst_Vector            v_XeBrightness;
    float                       f_XeContrast;

    float                       f_XeGodRayIntensity;
    ULONG                       ul_XeGodRayIntensityColor;
#endif

#ifdef ODE_INSIDE
	dWorldID					ode_id_world;							/* ID of the World for ODE */
	dSpaceID					ode_id_space;							
	dJointGroupID				ode_joint_dyna;

	dJointGroupID				ode_joint_ode;
	dJointGroupID				ode_joint_col_ode;
	dJointGroupID				ode_joint_col_jade;
	int							ode_contacts_num_jade;
	int							ode_contacts_num_ode;
	struct dContact				ode_contacts_jade[COL_Cul_MaxNbOfCollision];
	struct dContact				ode_contacts_ode[COL_Cul_MaxNbOfCollision];
#endif

#ifdef ACTIVE_EDITORS
	ULONG						ulNumberOfGroups;
	WOR_SelectionGroup			*pListOfGroups;

	/* For editors */
	ULONG						c_EditorReferences;						/* Number of editor that reference this world
																		 * * (always 0 if it's an engine world) */
	ULONG						c_HaveToBeSaved;
	struct SEL_tdst_Selection_	*pst_Selection;							/* List of selected object */
	/* Render Vectors ... */
	MATH_tdst_Vector			st_Origin[WOR_Cte_DbgVectorRender];
	MATH_tdst_Vector			st_Vector[WOR_Cte_DbgVectorRender];
	ULONG						aul_Color[WOR_Cte_DbgVectorRender];
    OBJ_tdst_GameObject *       ap_VectorGAO[WOR_Cte_DbgVectorRender];

	/* Render Circles ... */
	MATH_tdst_Vector			st_Center[WOR_Cte_DbgCircleRender];
	MATH_tdst_Vector			st_Normal[WOR_Cte_DbgCircleRender];
	float						af_Radius[WOR_Cte_DbgCircleRender];
	ULONG						aul_CircleColor[WOR_Cte_DbgCircleRender];
    OBJ_tdst_GameObject *       ap_CircleGAO[WOR_Cte_DbgCircleRender];
	
	/* Render other debug geo form */
	int							ai_GFType[ WOR_Cte_DbgGeoForm ];
	MATH_tdst_Vector			ast_GFPos[ WOR_Cte_DbgGeoForm ];
	MATH_tdst_Vector			ast_GFAxe1[ WOR_Cte_DbgGeoForm ];
	MATH_tdst_Vector			ast_GFAxe2[ WOR_Cte_DbgGeoForm ];
	MATH_tdst_Vector			ast_GFAxe3[ WOR_Cte_DbgGeoForm ];
	float						af_GFVal1[ WOR_Cte_DbgGeoForm ];
	float						af_GFVal2[ WOR_Cte_DbgGeoForm ];
	ULONG						aul_GFColor[ WOR_Cte_DbgGeoForm ];
    OBJ_tdst_GameObject *       ap_GeoFormGAO[WOR_Cte_DbgGeoForm];
	
	/* Show vector / pos */
	WOR_tdst_ShowVector			ast_ShowVector[ WOR_Cte_DbgShowVector ];

	ULONG						uc_Vector;
	ULONG						uc_Circle;
	ULONG						uc_GeoForm;
	ULONG						uc_ShowVector;

	BOOL						b_IsSplitScreen;
	int							i_SplitViewIdx;
#endif
} WOR_tdst_World;

/*$4
 ***********************************************************************************************************************
    Strutures linked to the World View: £
    WOR_tdst_View: the world view structure £
    WOR_tdst_Visibility: the visibility structure £
    WOR_tdst_WorldDisplay: the display structure of the world
 ***********************************************************************************************************************
 */

/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim: WOR_tdst_WorldDisplay: Structure that countains the display informations for the world view
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	WOR_tdst_WorldDisplay_
{
	void	*pst_DisplayDatas;	/* Display data associated with view */
	float	f_FieldOfVision;	/* View Fov */
	float	f_ViewportLeft;
	float	f_ViewportTop;
	float	f_ViewportWidth;
	float	f_ViewportHeight;
} WOR_tdst_WorldDisplay;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim: Structure View
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	WOR_tdst_View_
{
	MATH_tdst_Matrix		st_ViewPoint;						/* The (global) Matrix of the view */
	MATH_tdst_Matrix		st_OldViewPoint;					/* The old matrix of the View */
	OBJ_tdst_GameObject		*pst_Father;						/* Object that the activator follows */
	UCHAR					uc_Flags;							/* Flags for view status */
	WOR_tdst_WorldDisplay	st_DisplayInfo;						/* Informations about the display */

	/*
	 * Pointer to a function that copies (or modifies if wanted) the matrix from the
	 * father to the matrix of the activator (a view can follow a camera)
	 */
	void (*pfnv_ViewPointModificator) (struct WOR_tdst_View_ *);
}
WOR_tdst_View;

#ifdef ACTIVE_EDITORS
void WORGos_Load(WOR_tdst_World *_pst_Dest);
void WORGos_Save(WOR_tdst_World *_pst_Dest);
void WORGos_RemindGroup(WOR_tdst_World *_pst_World, ULONG ulNumber);
void WORGos_DefineGroup(WOR_tdst_World *_pst_World, char *GroupName);
void WORGos_DestroyGroup(WOR_tdst_World *_pst_World);
#endif

extern BOOL WOR_SectoObjVisible(struct WOR_tdst_World_ *, struct OBJ_tdst_GameObject_ *);
extern BOOL WOR_SectoObjActive(struct WOR_tdst_World_ *, struct OBJ_tdst_GameObject_ *);
extern void WOR_ComputeSector(struct WOR_tdst_World_ *);
extern void WOR_NoSectorVisAct(struct WOR_tdst_World_ *);
extern void WOR_SetCurrentSector(struct WOR_tdst_World_ *, int, BOOL);
extern void WOR_ValidatePortal(struct WOR_tdst_World_ *, WOR_tdst_Portal *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __WOR_STRUCT_H__ */
