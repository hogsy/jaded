/*$T LINKstruct_reg.cpp GC! 1.081 06/30/00 09:48:58 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "VAVview/VAVlist.h"
#include "LINKstruct.h"
#include "LINKstruct_reg.h"
#include "LINKtoed.h"
#include "LINKmsg.h"

#include "GDInterface/GDInterface.h"
#include "GEOmetric/GEOobject.h"
#include "ENGine/Sources/OBJects/OBJ.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/DYNamics/DYNaccess.h"
#include "ENGine/Sources/DYNamics/DYNconst.h"
#include "ENGine/Sources/COLlision/COLedit.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#include "ENGine/Sources/COLlision/COLinit.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "ENGine/Sources/GRId/GRI_display.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GPG.h"
#include "GraphicDK/Sources/SOFT/SOFThelper.h"
#include "GraphicDK/Sources/GRObject/GROedit.h"
#include "GraphicDK/Sources/PArticleGenerator/PAGstruct.h"
#include "GraphicDK/Sources/TEXture/TEXprocedural.h"
#include "GraphicDK/Sources/TEXture/TEXanimated.h"
#include "GraphicDK/Sources/TEXture/TEX_MESH.h"
#include "MATHs/MATHmatrixBase.h"
#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"
#include "ENGine/Sources/OBJects/OBJgizmo.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "EDIpaths.h"
#ifdef JADEFUSION
#include "ENGine/Sources/MoDiFier/MDFmodifier_SoftBody.h"
#endif

static SPG_SpriteElementDescriptor  st_SPG_SpriteMapper;
static GPG_tdst_Geom				st_GPG_Geom;
static GPG_tdst_GeomLOD				st_GPG_GeomLOD;
static GDI_tdst_DisplayData			st_DisplayData;
static SOFT_tdst_Helpers			st_Helpers;
static GRO_tdst_EditOptions			st_EditOptions;
static OBJ_tdst_GameObject			st_GO;
static OBJ_tdst_Base				st_Base;
static OBJ_tdst_Extended			st_Extended;
static GRO_tdst_Visu				st_Visu;
static DYN_tdst_Dyna				st_Dyna;
static DYN_tdst_Forces				st_DynForces;
static AI_tdst_Message				st_Message;
static WOR_tdst_World				st_World;
static WAY_tdst_Link				st_Link;
static COL_tdst_ZDx					st_Zone;
static COL_tdst_Cob					st_Cob;
static COL_tdst_GameMat				st_GM;
static ANI_st_GameObjectAnim		st_GOAnim;
static ANI_tdst_Anim				st_Anim;
static COL_tdst_Base				st_ColBase;
static OBJ_tdst_Hierarchy			st_GOHierarchy;
static OBJ_tdst_AdditionalMatrix	st_GOAddMatrix;
static OBJ_tdst_Group				st_Group;
static OBJ_tdst_Trace				st_Trace;
static ACT_st_ActionItem			st_ActionItem;
static GRID_tdst_World				st_Grid;
static GRID_tdst_DisplayOptions		st_GridDO;
static DYN_tdst_Col					st_Col;
static DYN_tdst_Solid				st_Solid;
static OBJ_tdst_DesignStruct		st_DesignStruct;
static DYN_tdst_Constraint			st_Constraint;
static PAG_tdst_Struct				st_PAG;
static WATER_Export_Struct			st_OceanExp;
#ifdef JADEFUSION
static CSoftBodyPlaneConstraint     s_SoftBodyColPlane;
static GDI_tdst_HighlightColors		st_HighlightColors;
#endif
extern EVAV_cl_ListBox				*gpo_CurVavListBox;

#ifdef JADEFUSION
extern void COL_SortGameMaterial(COL_tdst_GameMatList *);
extern UINT SPG2_gb_Recompute;
#else
extern "C" void COL_SortGameMaterial(COL_tdst_GameMatList *);
extern "C" UINT SPG2_gb_Recompute;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_RefreshAction(void *, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*psel;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	psel = (EVAV_cl_ViewItem *) _po_Item;
	LINK_UpdatePointer((void *) psel->mi_Param7);
	LINK_UpdatePointers();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_GOChangeEditorFlag(void *, void *, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject				*pst_GO;
	struct GDI_tdst_DisplayData_	*pst_DD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) ((char *) p_Data - ((char *) &st_GO.ul_EditorFlags - (char *) &st_GO));

	if(*(LONG *) p_Data & OBJ_C_EditFlags_Trace)
	{
		if(!(l_OldData & OBJ_C_EditFlags_Trace))
		{
			pst_GO->pst_Trace = (OBJ_tdst_Trace *) MEM_p_Alloc(sizeof(OBJ_tdst_Trace));
			pst_GO->pst_Trace->dst_Pos = (MATH_tdst_Vector *) MEM_p_Alloc(sizeof(MATH_tdst_Vector) * 20);
			pst_GO->pst_Trace->l_Number = 20;
			pst_GO->pst_Trace->l_Delay = 0;
			pst_GO->pst_Trace->l_Counter = 0;
			pst_GO->pst_Trace->l_NbDisplay = -1;
			pst_GO->pst_Trace->f_ZOffset = 0.0f;
			pst_GO->pst_Trace->ul_Color = 0xFFFFFFFF;
		}
	}
	else
	{
		if(l_OldData & OBJ_C_EditFlags_Trace)
		{
			MEM_Free(pst_GO->pst_Trace->dst_Pos);
			MEM_Free(pst_GO->pst_Trace);
			pst_GO->pst_Trace = NULL;
		}
	}

	if
	(
		(pst_GO->pst_World)
	&&	(pst_GO->pst_World->pst_View)
	&&	(pst_GO->pst_World->pst_View->st_DisplayInfo.pst_DisplayDatas)
	)
	{
		pst_DD = (struct GDI_tdst_DisplayData_ *) pst_GO->pst_World->pst_View->st_DisplayInfo.pst_DisplayDatas;

		if(!(*(LONG *) p_Data & OBJ_C_EditFlags_ShowAltBV))
		{
			if(l_OldData & OBJ_C_EditFlags_ShowAltBV)
			{
				if(pst_DD->uc_EditBounding)
				{
					LINK_PrintStatusMsg("AABBox/Sphere Edition Mode");
					pst_DD->pst_Helpers->ul_Flags = SOFT_Cul_HF_NoMovers | SOFT_Cul_HF_SAObject;
				}
			}
		}
		else
		{
			if(!(l_OldData & OBJ_C_EditFlags_ShowAltBV))
			{
				if(pst_DD->uc_EditBounding)
				{
					LINK_PrintStatusMsg("OBBox Edition Mode");
					pst_DD->pst_Helpers->ul_Flags = SOFT_Cul_HF_NoMovers | SOFT_Cul_HF_SAObject;
				}
			}
		}
	}

	OBJ_Gizmo_Update(pst_GO);
	LINK_UpdatePointer(pst_GO);
	LINK_UpdatePointers();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_GOChangeTraceParameters(void *, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Trace		*pst_Trace;
	EVAV_cl_ViewItem	*psel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	psel = (EVAV_cl_ViewItem *) _po_Item;
	pst_Trace = (OBJ_tdst_Trace *) ((char *) p_Data - psel->mul_Offset);

	if(psel->mul_Offset == 4)
	{
		if(pst_Trace->l_Number <= 1) pst_Trace->l_Number = 1;
		pst_Trace->dst_Pos = (MATH_tdst_Vector *) MEM_p_Realloc
			(
				pst_Trace->dst_Pos,
				sizeof(MATH_tdst_Vector) * pst_Trace->l_Number
			);
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_Refresh3DEngine(void *, void *, void *, LONG)
{
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_Refresh3DEngineG1(void *, void *, void *, LONG)
{
	SPG2_gb_Recompute = 1;
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_RefreshOcean(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	ULONG *pStruct;
	pStruct = (ULONG *)p_Data;
	while (*pStruct != 0xABadCafe) pStruct --;
	WTR_SetExportParams((WATER_Export_Struct *)pStruct);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_Refresh3DEngineGrid(void *, void *, void *, LONG)
{
	GRI_gb_UpdateZ = TRUE;
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_Refresh3DEngineAndPointers(void *p_Owner, void *, void *, LONG)
{
	LINK_Refresh();
	LINK_UpdatePointers();
}

/*
 =======================================================================================================================
    Aim:    Function called when the identity flags are changed in the editors
 =======================================================================================================================
 */
void LINK_CallBack_ChangeIdentityFlags(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*
	 * P_Data is a pointer to the identity flags, so it is also a pointer to the
	 * gameobject (identity flags is the first field of the game object struct)
	 */
	OBJ_ChangeIdentityFlags((OBJ_tdst_GameObject *) p_Data, *(LONG *) p_Data, l_OldData);

	/* We update the display of the editors */
	LINK_UpdatePointer(p_Data);
	LINK_UpdatePointers();
	LINK_Refresh();
}

/*
 =======================================================================================================================
    Aim:    Function called when the identity flags are changed in the editors
 =======================================================================================================================
 */
void LINK_CallBack_ChangeLOD(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	OBJ_tdst_GameObject	*pst_GO;

	pst_GO = (OBJ_tdst_GameObject *) p_Data;

	LINK_Refresh();
}


/*
 =======================================================================================================================
    Aim:    Function called when the control flags are changed in the editors
 =======================================================================================================================
 */
void LINK_CallBack_ChangeControlFlags(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject st_GO, *pst_GO;
	USHORT				us_Offset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * We first obtain the GameObject pointer from p_Data which is a pointer to the
	 * Control Flags.
	 */
	us_Offset = (char *) &st_GO.ul_StatusAndControlFlags - (char *) &st_GO;
	pst_GO = (OBJ_tdst_GameObject *) ((char *) p_Data - us_Offset);

	OBJ_ChangeControlFlags(pst_GO, pst_GO->ul_StatusAndControlFlags, 0);

	if(pst_GO->pst_World)
	{
		pst_GO->pst_World->b_ForceActivationRefresh = TRUE;
		pst_GO->pst_World->b_ForceBVRefresh = TRUE;
	}

	LINK_UpdatePointer(pst_GO);
	LINK_UpdatePointers();
	LINK_Refresh();
}

/*
 =======================================================================================================================
    Aim:    Function called when the Dynamics flags are changed in the editors
 =======================================================================================================================
 */
void LINK_CallBack_ChangeDynamicsFlags(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*
	 * P_Data is a pointer to the dynamics flags, so it is also a pointer to the
	 * dynamics (dynamics flags is the first field of the DYN_tdst_Dyna struct)
	 */
	DYN_ChangeDynFlags((DYN_tdst_Dyna *) p_Data, *(LONG *) p_Data, l_OldData);
	LINK_UpdatePointers();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_ChangeDynamicValues(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* The mass was changed */
	if(!L_strcmpi(((EVAV_cl_ViewItem *) _po_Item)->masz_Name, DYN_Csz_MassString))
	{
		/* We find the beginning of the structure */
		pst_Dyna = (DYN_tdst_Dyna *) ((char *) p_Data - DYN_Cul_OffsetMass);
		DYN_SetMass(pst_Dyna, *(float *) p_Data);
	}

	/* The speed vector was changed */
	if(!L_strcmpi(((EVAV_cl_ViewItem *) _po_Item)->masz_Name, DYN_Csz_SpeedVectorString))
	{
		/* We find the beginning of the structure */
		pst_Dyna = (DYN_tdst_Dyna *) ((char *) p_Data - DYN_Cul_OffsetSpeedVector);
		DYN_SetSpeedVector(pst_Dyna, (MATH_tdst_Vector *) p_Data);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_ChangeZoneFlags(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ZDx	st_ZDx;
	USHORT			uw_Offset;
	COL_tdst_ZDx	*pst_ZDx;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* P_Data is a pointer to the Flags .. We want to get a pointer to the COL_tdst_ZDx */
	uw_Offset = (char *) &st_ZDx.uc_Flag - (char *) &st_ZDx;
	pst_ZDx = (COL_tdst_ZDx *) ((char *) p_Data - uw_Offset);
	COL_Zone_ChangeFlags(pst_ZDx, *(LONG *) p_Data, l_OldData);

	LINK_UpdatePointers();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_ChangeCobFlags(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cob	st_Cob;
	USHORT			uw_Offset;
	COL_tdst_Cob	*pst_Cob;
	/*~~~~~~~~~~~~~~~~~~~~~~*/


	/* If the Cob Flag that has been updated is not the Game Material Flag, we return; */
	if ( ((*(LONG *) p_Data) & COL_C_Cob_GameMat) == (l_OldData & COL_C_Cob_GameMat)) return;

	/* P_Data is a pointer to the Flags .. We want to get a pointer to the COL_tdst_Cob */
	uw_Offset = (char *) &st_Cob.uc_Flag - (char *) &st_Cob;
	pst_Cob = (COL_tdst_Cob *) ((char *) p_Data - uw_Offset);

	if((*(LONG *) p_Data) & COL_C_Cob_GameMat)
	{
		switch(pst_Cob->uc_Type)
		{
		case COL_C_Zone_Triangles:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				COL_tdst_IndexedTriangles			*pst_CobObj;
				COL_tdst_ElementIndexedTriangles	*pst_CobElement;
				ULONG								i;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_CobObj = pst_Cob->pst_TriangleCob;
				pst_CobElement = pst_CobObj->dst_Element;

				/* We create a GameMaterial for each Element. */
				pst_Cob->pst_GMatList = (COL_tdst_GameMatList *) MEM_p_Alloc(sizeof(COL_tdst_GameMatList));
				L_memset(pst_Cob->pst_GMatList, 0, sizeof(COL_tdst_GameMatList));
				pst_Cob->pst_GMatList->pst_GMat = (COL_tdst_GameMat *) MEM_p_Alloc(pst_CobObj->l_NbElements * sizeof(COL_tdst_GameMat));
				L_memset(pst_Cob->pst_GMatList->pst_GMat, 0, pst_CobObj->l_NbElements * sizeof(COL_tdst_GameMat));
				pst_Cob->pst_GMatList->pal_Id = (ULONG *) MEM_p_Alloc(pst_CobObj->l_NbElements * sizeof(ULONG));
				L_memset(pst_Cob->pst_GMatList->pal_Id, 0, pst_CobObj->l_NbElements * sizeof(ULONG));

				for(i = 0; i < pst_CobObj->l_NbElements; i++, pst_CobElement++)
				{
					pst_Cob->pst_GMatList->pal_Id[i] = pst_CobElement->l_MaterialId;
					pst_Cob->pst_GMatList->pst_GMat[i].ul_Color = 0xFF00FF00; /* Default Green Color */

				}

				pst_Cob->pst_GMatList->ul_GMat = pst_CobObj->l_NbElements;
			}
			break;

		default:
			pst_Cob->pst_GMatList = (COL_tdst_GameMatList *) MEM_p_Alloc(sizeof(COL_tdst_GameMatList));
			L_memset(pst_Cob->pst_GMatList, 0, sizeof(COL_tdst_GameMatList));
			pst_Cob->pst_GMatList->pst_GMat = (COL_tdst_GameMat *) MEM_p_Alloc(sizeof(COL_tdst_GameMat));
			L_memset(pst_Cob->pst_GMatList->pst_GMat, 0, sizeof(COL_tdst_GameMat));
			pst_Cob->pst_GMatList->pal_Id = (ULONG *) MEM_p_Alloc(sizeof(ULONG));
			L_memset(pst_Cob->pst_GMatList->pal_Id, 0, sizeof(ULONG));

			pst_Cob->pst_GMatList->pal_Id[0] = pst_Cob->pst_MathCob->l_MaterialId;
			pst_Cob->pst_GMatList->ul_GMat = 1;
			pst_Cob->pst_GMatList->pst_GMat [0].ul_Color = 0xFF00FF00; /* Default Green Color */
			break;
		}

		pst_Cob->uc_Flag |= COL_C_Cob_GameMat;
		pst_Cob->pst_GMatList->ul_NbOfInstances = 1;
	}
	else
		COL_FreeGameMaterial(pst_Cob);

	COL_GMat_UpdateFileName(pst_Cob);

	LINK_UpdatePointer(pst_Cob);
	LINK_UpdatePointers();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_ChangeCobID(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Mathematical	st_Math;
	USHORT					uw_Offset;
	COL_tdst_Mathematical	*pst_Math;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * P_Data is a pointer to the ID .. We want to get a pointer to the
	 * COL_tdst_Mathematical
	 */
	uw_Offset = (char *) &st_Math.l_MaterialId - (char *) &st_Math;
	pst_Math = (COL_tdst_Mathematical *) ((char *) p_Data - uw_Offset);

	LINK_UpdatePointer(pst_Math->pst_Cob);
	LINK_UpdatePointers();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_ChangeElementFlags(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cob						*pst_Cob;
	COL_tdst_ElementIndexedTriangles	st_Element;
	USHORT								uw_Offset;
	COL_tdst_ElementIndexedTriangles	*pst_Element;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * P_Data is a pointer to the Flags .. We want to get a pointer to the
	 * COL_tdst_ElementIndexedTriangles
	 */
	uw_Offset = (char *) &st_Element.uc_Flag - (char *) &st_Element;
	pst_Element = (COL_tdst_ElementIndexedTriangles *) ((char *) p_Data - uw_Offset);
	pst_Cob = pst_Element->pst_Cob;

	if((*(LONG *) p_Data) & COL_C_Cob_GameMat)
	{
		if(!pst_Cob->pst_GMatList)
		{
			pst_Cob->pst_GMatList = (COL_tdst_GameMatList *) MEM_p_Alloc(sizeof(COL_tdst_GameMatList));
			L_memset(pst_Cob->pst_GMatList, 0, sizeof(COL_tdst_GameMatList));
			pst_Cob->pst_GMatList->pst_GMat = (COL_tdst_GameMat *) MEM_p_Alloc(sizeof(COL_tdst_GameMat));
			L_memset(pst_Cob->pst_GMatList->pst_GMat, 0, sizeof(COL_tdst_GameMat));
			pst_Cob->pst_GMatList->pal_Id = (ULONG *) MEM_p_Alloc(sizeof(ULONG));
			pst_Cob->pst_GMatList->pal_Id[0] = pst_Element->l_MaterialId;

			pst_Cob->pst_GMatList->ul_GMat = 1;
			pst_Cob->pst_GMatList->ul_NbOfInstances = 1;
			pst_Cob->uc_Flag |= COL_C_Cob_GameMat;
		}
		else
		{
			pst_Cob->pst_GMatList->pst_GMat = (COL_tdst_GameMat *) MEM_p_Realloc
				(
					pst_Cob->pst_GMatList->pst_GMat,
					(pst_Cob->pst_GMatList->ul_GMat + 1) * sizeof(COL_tdst_GameMat)
				);

			/* Set to 0 the newly created Game Material. */
			L_memset(&pst_Cob->pst_GMatList->pst_GMat[pst_Cob->pst_GMatList->ul_GMat], 0, sizeof(COL_tdst_GameMat));
			pst_Cob->pst_GMatList->pal_Id = (ULONG *) MEM_p_Realloc
				(
					pst_Cob->pst_GMatList->pal_Id,
					(pst_Cob->pst_GMatList->ul_GMat + 1) * sizeof(ULONG)
				);
			pst_Cob->pst_GMatList->pal_Id[pst_Cob->pst_GMatList->ul_GMat] = pst_Element->l_MaterialId;
			pst_Cob->pst_GMatList->ul_GMat++;
			COL_SortGameMaterial(pst_Cob->pst_GMatList);
		}
	}
	else
	{
		if(pst_Cob->pst_GMatList)
		{
			if(pst_Cob->pst_GMatList->ul_GMat > 1)
				COL_GMat_RemoveAtIndex(pst_Cob, COL_ul_GMat_GetIndex(pst_Cob, pst_Element));
			else
				COL_FreeGameMaterial(pst_Cob);
		}
	}

	COL_GMat_UpdateFileName(pst_Cob);

	LINK_UpdatePointer(pst_Element->pst_Cob);
	LINK_UpdatePointers();
	LINK_Refresh();
}

/*
=======================================================================================================================
=======================================================================================================================
*/ 
void LINK_CallBack_Modifier_Refresh(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
    EVAV_cl_ViewItem	*po_Item;
    EVAV_cl_ViewItem	*po_Parent;
    MDF_tdst_Modifier   *pst_Mdf = NULL;

    po_Item = (EVAV_cl_ViewItem *) _po_Item;

    if(po_Item->mp_Data == p_Data)
    {
        //The display item is the one modified
        po_Parent = gpo_CurVavListBox->po_GetTypedParent(po_Item, LINK_C_MDF_Modifier);
        if(po_Parent) 
            pst_Mdf = *(MDF_tdst_Modifier **)(po_Parent->mp_Data);
    }
    else
        return;

    if(pst_Mdf)
    {
        MDF_Reinit(pst_Mdf->pst_GO);
        LINK_UpdatePointer(pst_Mdf->pst_GO);
    }

    LINK_UpdatePointers();
    //EDI_OUT_gl_ForceSetMode = 1;
    LINK_Refresh();
    //EDI_OUT_gl_ForceSetMode = 0;
}

#ifdef ODE_INSIDE
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_ChangeODEFlags(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_ODE						st_ODE;
	DYN_tdst_ODE						*pst_ODE;
	dMass								Mass;
	USHORT								uw_Offset;
	OBJ_tdst_GameObject					*pst_GO;
	WOR_tdst_World						*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * P_Data is a pointer to the Flags .. We want to get a pointer to the
	 * COL_tdst_ElementIndexedTriangles
	 */
	uw_Offset = (char *) &st_ODE.uc_Flags - (char *) &st_ODE;
	pst_ODE = (DYN_tdst_ODE *) ((char *) p_Data - uw_Offset);
	pst_GO = pst_ODE->pst_GO;

	if((*(LONG *) p_Data) & ODE_FLAGS_RIGIDBODY)
	{
		if(!pst_ODE->ode_id_body)
		{
			pst_ODE->ode_id_body = dBodyCreate(pst_GO->pst_World->ode_id_world);
			dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);

			dBodySetData(pst_ODE->ode_id_body, pst_GO);

			pst_ODE->f_LinearThres = 0.2f;
			pst_ODE->f_AngularThres = 0.2f;

			pst_ODE->uc_Flags |= ODE_FLAGS_RIGIDBODY;

			if(pst_ODE->uc_Flags & ODE_FLAGS_AUTODISABLE)
			{
				dBodySetAutoDisableFlag(pst_ODE->ode_id_body, 1);
				dBodySetAutoDisableLinearThreshold(pst_ODE->ode_id_body, pst_ODE->f_LinearThres);
				dBodySetAutoDisableAngularThreshold(pst_ODE->ode_id_body, pst_ODE->f_AngularThres);
				dBodySetAutoDisableSteps (pst_ODE->ode_id_body, 10);
			}
			else
				dBodySetAutoDisableFlag(pst_ODE->ode_id_body, 0);
		}
	}
	else
	{
		if(pst_ODE->ode_id_body)
		{
			dBodyDestroy(pst_ODE->ode_id_body);
			pst_ODE->ode_id_body = 0;
			pst_ODE->uc_Flags &= ~ODE_FLAGS_RIGIDBODY;
		}
	}

	if(pst_ODE->ode_id_body)
	{

		if((*(LONG *) p_Data) & ODE_FLAGS_AUTODISABLE)
		{
			if(!(l_OldData & ODE_FLAGS_AUTODISABLE))
			{
				dBodySetAutoDisableFlag(pst_ODE->ode_id_body, 1);
				dBodySetAutoDisableLinearThreshold(pst_ODE->ode_id_body, pst_ODE->f_LinearThres);
				dBodySetAutoDisableAngularThreshold(pst_ODE->ode_id_body, pst_ODE->f_AngularThres);
				dBodySetAutoDisableSteps (pst_ODE->ode_id_body, 10);
			}
		}

		if(!((*(LONG *) p_Data) & ODE_FLAGS_AUTODISABLE))
		{
			if(l_OldData & ODE_FLAGS_AUTODISABLE)
			{
				dBodySetAutoDisableFlag(pst_ODE->ode_id_body, 0);
			}
		}
	}


	if((*(LONG *) p_Data) & ODE_FLAGS_FORCEIMMOVABLE)
	{
		if(!(l_OldData & ODE_FLAGS_FORCEIMMOVABLE)  && (pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY))
		{
			dBodyDestroy(pst_ODE->ode_id_body);
			pst_ODE->ode_id_body = 0;

		}
	}

	if(!((*(LONG *) p_Data) & ODE_FLAGS_FORCEIMMOVABLE))
	{
		if((l_OldData & ODE_FLAGS_FORCEIMMOVABLE) && (pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY))
		{
			pst_World = WOR_World_GetWorldOfObject(pst_GO);

			pst_ODE->ode_id_body = dBodyCreate(pst_World->ode_id_world);

			dBodySetData(pst_ODE->ode_id_body, pst_GO);

			switch(pst_ODE->uc_Type)
			{
			case ODE_TYPE_SPHERE:
				{
					dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
					dMassSetSphereTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X);
					dBodySetMass(pst_ODE->ode_id_body, &Mass);
				}
				break;

			case ODE_TYPE_BOX:
				{
					dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
					dMassSetBoxTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);
					dBodySetMass(pst_ODE->ode_id_body, &Mass);
				}
				break;

			case ODE_TYPE_CYLINDER:
				{
					dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
					dMassSetCylinderTotal(&Mass, pst_ODE->mass_init, 2, pst_ODE->f_X, pst_ODE->f_Y);
					dBodySetMass(pst_ODE->ode_id_body, &Mass);
				}
				break;
			}

		}
	}

	if((*(LONG *) p_Data) & ODE_FLAGS_ENABLE)
	{
		if(!(l_OldData & ODE_FLAGS_ENABLE)  && (pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY))
		{
			dBodyEnable(pst_ODE->ode_id_body);
		}
	}

	if(!((*(LONG *) p_Data) & ODE_FLAGS_ENABLE))
	{
		if((l_OldData & ODE_FLAGS_ENABLE) && (pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY))
		{
			dBodyDisable(pst_ODE->ode_id_body);
		}
	}

	LINK_UpdatePointer(pst_GO);
	LINK_UpdatePointers();
	LINK_Refresh();
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_ChangeElementID(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cob						*pst_Cob;
	COL_tdst_ElementIndexedTriangles	st_Element;
	USHORT								uw_Offset;
	COL_tdst_ElementIndexedTriangles	*pst_Element;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * P_Data is a pointer to the ID .. We want to get a pointer to the
	 * COL_tdst_ElementIndexedTriangles
	 */
	uw_Offset = (char *) &st_Element.l_MaterialId - (char *) &st_Element;
	pst_Element = (COL_tdst_ElementIndexedTriangles *) ((char *) p_Data - uw_Offset);
	pst_Cob = pst_Element->pst_Cob;

	LINK_UpdatePointer(pst_Cob);
	LINK_UpdatePointers();
	LINK_Refresh();
}
#ifdef JADEFUSION
// =======================================================================================================================
//    Aim:    Function called when the misc flags are changed in the editors
// =======================================================================================================================
void LINK_CallBack_ChangeExtraFlags(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	USHORT				  us_Offset;

	us_Offset = (char *) &st_Extended.uw_ExtraFlags - (char *) &st_Extended;

	OBJ_tdst_Extended * pExtended = (OBJ_tdst_Extended *)((char *) p_Data - us_Offset);

	OBJ_ChangeExtraFlags(pExtended, pExtended->uw_ExtraFlags, (UCHAR)l_OldData);

	// We update the display of the editors
	LINK_UpdatePointers();
	LINK_Refresh();
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_Callback_SaveProcedural( void *, void *, void *, LONG )
{
    TEX_Procedural_Save();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_Callback_SaveAnimated( void *, void *, void *, LONG )
{
    TEX_Anim_Save();
}


#define MATH_Csz_OrientationI		"Orientation I"
#define MATH_Csz_OrientationJ		"Orientation J"
#define MATH_Csz_OrientationK		"Orientation K"
#define MATH_Csz_Translation		"Translation"
#define MATH_Csz_Rotation			"Rotation"
#define MATH_Csz_Scale				"Scale"
#define MATH_Csz_OrientedScale		"Oriented Scale"
#define MATH_Csz_AnyScale			"Any Scale"
#define MATH_Csz_ScaleX				"Scale X"
#define MATH_Csz_ScaleY				"Scale Y"
#define MATH_Csz_ScaleZ				"Scale Z"
#define MATH_Csz_Type				"Matrix Type"

#define MATH_Cul_OffsetOrientationJ 16
#define MATH_Cul_OffsetOrientationK 32
#define MATH_Cul_OffsetTranslation	48
#define MATH_Cul_OffsetType			64
#define MATH_Cul_OffsetScaleX		12
#define MATH_Cul_OffsetScaleY		28
#define MATH_Cul_OffsetScaleZ		44

#define M_NameEqual(N)				(!L_strcmpi(((EVAV_cl_ViewItem *) _po_Item)->masz_Name, (N)))

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_CallBack_ChangeMatrix(void *p_Owner, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*pst_Matrix;
	EVAV_cl_ViewItem	*po_Parent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Find the beginning of the structure depending on what is edited */
	if(M_NameEqual("Whole Rotation Matrix"))
	{
		pst_Matrix = (MATH_tdst_Matrix *) p_Data;
	}

	if(M_NameEqual(MATH_Csz_OrientationI))
	{
		pst_Matrix = (MATH_tdst_Matrix *) p_Data;

		/*
		 * We also make a rotation matrix without clearing the scale and translation, and
		 * without changing the direction of the X axis
		 */
		MATH_MakeRotationMatrix_KeepingXAxis(pst_Matrix, 0);
	}

	if(M_NameEqual(MATH_Csz_OrientationJ))
	{
		pst_Matrix = (MATH_tdst_Matrix *) ((char *) p_Data - MATH_Cul_OffsetOrientationJ);

		/*
		 * We also make a rotation matrix without clearing the scale and translation, and
		 * without changing the direction of the Y axis
		 */
		MATH_MakeRotationMatrix_KeepingYAxis(pst_Matrix, 0);
	}

	if(M_NameEqual(MATH_Csz_OrientationK))
	{
		pst_Matrix = (MATH_tdst_Matrix *) ((char *) p_Data - MATH_Cul_OffsetOrientationK);

		/*
		 * We also make a rotation matrix without clearing the scale and translation, and
		 * without changing the direction of the Z axis
		 */
		MATH_MakeRotationMatrix_KeepingYAxis(pst_Matrix, 0);
	}

	if(M_NameEqual(MATH_Csz_Translation))
		pst_Matrix = (MATH_tdst_Matrix *) ((char *) p_Data - MATH_Cul_OffsetTranslation);
	if(M_NameEqual(MATH_Csz_ScaleX)) pst_Matrix = (MATH_tdst_Matrix *) ((char *) p_Data - MATH_Cul_OffsetScaleX);
	if(M_NameEqual(MATH_Csz_ScaleY)) pst_Matrix = (MATH_tdst_Matrix *) ((char *) p_Data - MATH_Cul_OffsetScaleY);
	if(M_NameEqual(MATH_Csz_ScaleZ)) pst_Matrix = (MATH_tdst_Matrix *) ((char *) p_Data - MATH_Cul_OffsetScaleZ);

	/* Set the correct type of the matrix */
	MATH_SetCorrectType(pst_Matrix);

	/* In case the matrix is the global position, recompute local if necessary */
	po_Parent = gpo_CurVavListBox->po_GetParentGAO((EVAV_cl_ViewItem *) _po_Item);
	if(po_Parent) OBJ_ComputeLocalWhenHie((OBJ_tdst_GameObject *) po_Parent->mp_Data);

	LINK_Refresh();
}

#undef M_NameEqual

#ifdef JADEFUSION
void LINK_Callback_ChangeSoftBodyColPlane(void *, void *_po_Item, void *p_Data, LONG l_OldData)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CSoftBodyPlaneConstraint * p_ColPlane;
	MATH_tdst_Vector         * pst_Normal;
	OBJ_tdst_GameObject      * pst_Gao;
	EVAV_cl_ViewItem         * po_Item;
	EVAV_cl_ViewItem         * psel;
	EVAV_cl_ViewItem	     * po_Parent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Item = (EVAV_cl_ViewItem *) _po_Item;

	psel = (EVAV_cl_ViewItem *) _po_Item;
	p_ColPlane = (CSoftBodyPlaneConstraint *) ((char *) p_Data - psel->mul_Offset);

	pst_Normal = (MATH_tdst_Vector *)(&p_ColPlane->m_LocalPlane.X);
	MATH_NormalizeEqualAnyVector(pst_Normal);

	if(po_Item->mp_Data == p_Data)
	{
		//The display item is the one modified
		po_Parent = gpo_CurVavListBox->po_GetTypedParent(po_Item, LINK_C_ENG_GameObjectOriented);
		pst_Gao = (OBJ_tdst_GameObject *) po_Parent->mp_Data;
	}
}
#endif

/*
 =======================================================================================================================
    Aim:    Main function to register all engine structures.
 =======================================================================================================================
 */
void LINK_RegisterAllStructs(void)
{
	/* TEMPORARY STRUCTS MUST BE GLOBAL !!! */

/*$off*/

	/*
     * _i_Num The id of the structure. This id must be define in LINKstruct_reg.h 
     * _psz_StructName Name of the structure to register. 
     * _pfn_CB Callback associated with the structure 
     * 
     * 1- Name of the field 
     * 2- Type of the field 
     * 3- Offset from the beginning of the structure of the field 
     * 4- Flags 
     * 5- First param (depending of type) 
     * 6- Second param (depending of type) 
     * 7- Third param (depeinding of type) 
	 * 8- Sub group (for filtering). Must be 1 if not used
     * £
     * ... (repeat 1 to 7 for each field) 
     * 
     * -1 (end mark)
     */

	/*
	 * Flags :
	 *
	 * EVAV_ReadOnly		Value in field can't be change by user.
	 * EVAV_CanDelete		Field can be delete (in edition) by the user.
	 */
	/*
	 * Types for fields :
	 *
	 * EVAV_EVVIT_Separator	Param1 is the filter string "name1\0value1\0name2\0value2\0\0", Param2 and Param3 ignored
	 * EVAV_EVVIT_Pointer	Param1 is the registered type of the structure, Param2 and Param3 ignored.
	 * EVAV_EVVIT_SubStruct	Param1 is the registered type of the structure, Param2 and Param3 ignored.
	 * EVAV_EVVIT_Bool		Param1, Param2 and Param3 ignored (field must be a BOOL type)
	 * EVAV_EVVIT_Flags		Param1 = N° of the bit (first is 1), Param2 is size of field (1, 2, or 4 bytes), Param3 ignored
	 * EVAV_EVVIT_Int		Param1 = Min value, Param2 = Max value, Param3 = size of int (1, 2 or 4 bytes, neg value for unsigned value)
	 * EVAV_EVVIT_Hexa		Same as EVAV_EVVIT_Int, but display is in hexa
	 * EVAV_EVVIT_String	Param1, Param2 and Param3 ignored
	 * EVAV_EVVIT_Float		Param1 = Min value, Param2 = Max value, Param3 ignored
	 * EVAV_EVVIT_Vector	Param1, Param2 and Param3 ignored
	 * EVAV_EVVIT_Matrix	Param1, Param2 and Param3 ignored
	 * EVAV_EVVIT_Color		(COLORREF 0x00BBGGRR) Param1, Param2 and Param3 ignored
	 */

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_FlagsMatrix,
		"Matrix Flags",
		NULL,
		MATH_Csz_Translation,		EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "",
		MATH_Csz_Rotation,			EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "",
		MATH_Csz_Scale,				EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "",
		MATH_Csz_OrientedScale,		EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 4, 4, 0, 0, 0, "",
		MATH_Csz_AnyScale,			EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 5, 4, 0, 0, 0, "",
#ifdef JADEFUSION
		"Viewport size: X", 	EVAV_EVVIT_Float, (void *) ((char *) &st_DisplayData.st_Camera.f_Width - (char *)&st_DisplayData.st_Camera), EVAV_ReadOnly, 0.0f, 0.0f, 0, 0, 0, "Horizontal viewport size",
		"Viewport size: Y",		EVAV_EVVIT_Float, (void *) ((char *) &st_DisplayData.st_Camera.f_Height - (char *) &st_DisplayData.st_Camera), EVAV_ReadOnly, 0.0f, 0.0f, 0, 0, 0, "Vertical viewport size",
#endif
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_Matrix,
		"Matrix",
		LINK_CallBack_ChangeMatrix,
		MATH_Csz_OrientationI,		EVAV_EVVIT_Vector,		(void *) 0,		0, 0, 0, 0, 0, 0, "",
		MATH_Csz_OrientationJ,		EVAV_EVVIT_Vector,		(void *) 16, 0, 0, 0, 0, 0, 0, "",
		MATH_Csz_OrientationK,		EVAV_EVVIT_Vector,		(void *) 32, 0, 0, 0, 0, 0, 0, "",
		MATH_Csz_Translation,		EVAV_EVVIT_Vector,		(void *) 48, 0, 0, 0, 0, 0, 0, "",
		MATH_Csz_ScaleX,			EVAV_EVVIT_Float,		(void *) 12, 0, Cf_Zero, Cf_Infinit, 0, 0, 0, "",
		MATH_Csz_ScaleY,			EVAV_EVVIT_Float,		(void *) 28, 0, Cf_Zero, Cf_Infinit, 0, 0, 0, "",
		MATH_Csz_ScaleZ,			EVAV_EVVIT_Float,		(void *) 44, 0, Cf_Zero, Cf_Infinit, 0, 0, 0, "",
		MATH_Csz_Type,				EVAV_EVVIT_SubStruct,	(void *) 64, 0, LINK_C_FlagsMatrix, 0, 0, 0, 0, "",
		"Whole Rotation Matrix",	EVAV_EVVIT_Matrix,		(void *) 0, 0, 0, 0, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_StructCamera,
		"Camera",
		LINK_CallBack_Refresh3DEngine,
		"Perspective",			EVAV_EVVIT_Flags, (void *) ((char *) &st_DisplayData.st_Camera.ul_Flags - (char *) &st_DisplayData.st_Camera), 0, 0, 4, 0, 0, 0, "True = perspective , False = isometric",
        "Iso2",			        EVAV_EVVIT_Flags, (void *) ((char *) &st_DisplayData.st_Camera.ul_Flags - (char *) &st_DisplayData.st_Camera), 0, 8, 4, 0, 0, 0, "True = Iso 2, False = old iso",
		"Matrix",				EVAV_EVVIT_SubStruct, (void *) ((char *) &st_DisplayData.st_Camera.st_Matrix - (char *) &st_DisplayData.st_Camera ), 0, LINK_C_Matrix, 0, 0, 0, 0, "Position and orientation",
		"FoV",					EVAV_EVVIT_Float, (void *) ((char *) &st_DisplayData.st_Camera.f_FieldOfVision - (char *) &st_DisplayData.st_Camera ), EVAV_ReadOnly, 0.01f, 3.0f, 0, 0, 0, "Focale",
		"Ratio Y over X",		EVAV_EVVIT_Float, (void *) ((char *) &st_DisplayData.st_Camera.f_YoverX- (char *) &st_DisplayData.st_Camera ), 0, 0.1f, 3.0f, 0, 0, 0, "Pixel ratio",
		"Near plane",			EVAV_EVVIT_Float, (void *) ((char *) &st_DisplayData.st_Camera.f_NearPlane - (char *) &st_DisplayData.st_Camera ), 0, 0.0f, 0.0f, 0, 0, 0, "Near plane distance",
		"Far plane",			EVAV_EVVIT_Float, (void *) ((char *) &st_DisplayData.st_Camera.f_FarPlane - (char *) &st_DisplayData.st_Camera ), 0, 0.0f, 0.0f, 0, 0, 0, "Far plane distance",
		"Iso factor",			EVAV_EVVIT_Float, (void *) ((char *) &st_DisplayData.st_Camera.f_IsoFactor - (char *) &st_DisplayData.st_Camera ), 0, 0.0f, 1000.0f, 0, 0, 0, "Isometric factor",
		-1
	);


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_TextureManagerFlag,
		"Flags",
		LINK_CallBack_Refresh3DEngine,
		"Accept non square texture",        EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "True : accept non square texture",
		"Force reload",						EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "",
		"Use fast compression",				EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "True : use fast texture compression",
		"Use mipmap",						EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "True : enable mipmap",
		"Invert mipmap",					EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "True : invert color of successive mipmap level",
		"Dump bench",						EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "True : display information into log window",
        "Accept 24 bits textures",			EVAV_EVVIT_Flags, (void *) 0, 0, 9, 4, 0, 0, 0, "True : 24 bit per pixel (RGB) textures are accepted",
        "Accept 16 bits textures (1555)",	EVAV_EVVIT_Flags, (void *) 0, 0,10, 4, 0, 0, 0, "True : 16 bit per pixel (ARGB = 1555) textures are accepted",
        "Accept 16 bits textures (4444)",	EVAV_EVVIT_Flags, (void *) 0, 0,11, 4, 0, 0, 0, "True : 16 bit per pixel (ARGB = 4444) textures are accepted",
        "Accept 8 bits textures (palette)",	EVAV_EVVIT_Flags, (void *) 0, 0,12, 4, 0, 0, 0, "True : 8 bit per pixel are accepted, each value is an index in a table",
        "Accept 4 bits textures (palette)",	EVAV_EVVIT_Flags, (void *) 0, 0,13, 4, 0, 0, 0, "True : 4 bit per pixel are accepted, each value is an index in a table",
        "Accept palette with alpha values",	EVAV_EVVIT_Flags, (void *) 0, 0,14, 4, 0, 0, 0, "True : palette can have variable value for alpha component",
        "Several tex for same raw",	        EVAV_EVVIT_Flags, (void *) 0, 0,15, 4, 0, 0, 0, "True : more than one texture for same raw if different palette",
        "Swap RGB to BGR",                  EVAV_EVVIT_Flags, (void *) 0, 0,20, 4, 0, 0, 0, "True : Red and blue componants are swapped",
        "Store interface texture",          EVAV_EVVIT_Flags, (void *) 0, 0,21, 4, 0, 0, 0, "True : Interface texture are stored in Ram (not directly in vram)",
        "Load stored interface texture",    EVAV_EVVIT_Flags, (void *) 0, 0,22, 4, 0, 0, 0, "True : Interface texture are loaded each trame in VRAM",
		-1
	);


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_StructTextureManager,
		"Texture manager",
		LINK_CallBack_Refresh3DEngine,
		"Flags",				        EVAV_EVVIT_SubStruct,   (void *) ((char *) &st_DisplayData.st_TexManager.ul_Flags - (char *) &st_DisplayData.st_TexManager ), 0, LINK_C_TextureManagerFlag, 0, 0, 0, 0, "Options flags",
		"Max texture size",		        EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.l_MaxTextureSize - (char *) &st_DisplayData.st_TexManager), 0, 0, 0, 0, 0, 0, "Max height or width of texture (in pixels)",
        "Min texture size",		        EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.l_MinTextureSize - (char *) &st_DisplayData.st_TexManager), 0, 0, 0, 0, 0, 0, "Min height or width of texture (in pixels)",
		"VRAM Available",		        EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.l_VRamAvailable- (char *) &st_DisplayData.st_TexManager), 0, 0, 0, 0, 0, 0, "Max available memory for texture storage",
		"VRAM left",			        EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.l_VRamLeft - (char *) &st_DisplayData.st_TexManager), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Texture memory left after loading",
		"VRAM needed",		            EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.l_VRamNeeded - (char *) &st_DisplayData.st_TexManager), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Texture memory needed for all textures (not compressed)",
        "RAM Available",		        EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.l_RamAvailable- (char *) &st_DisplayData.st_TexManager), 0, 0, 0, 0, 0, 0, "Max available memory for texture storage",
		"RAM left",			            EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.l_RamLeft - (char *) &st_DisplayData.st_TexManager), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Texture memory left after loading",
		"RAM needed",		            EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.l_RamNeeded - (char *) &st_DisplayData.st_TexManager), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Texture memory needed for all textures (not compressed)",
		"Memory taken",			        EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.l_MemoryTakenByLoading - (char *) &st_DisplayData.st_TexManager), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Texture memory taken by all textures (compressed)",
        "Number of textures",	        EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.l_TextureNumber - (char *) &st_DisplayData.st_TexManager), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Total number of texture in global list",
        "Number of textures in VRAM",	EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.l_VRamTextureNumber - (char *) &st_DisplayData.st_TexManager), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Number of textures loaded in VRam",
        "Number in VRAM (4bpp)",        EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.al_TextureNumberByBpp[0]- (char *) &st_DisplayData.st_TexManager), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Number of 4 bpp textures loaded in VRam",
        "Number in VRAM (8bpp)",        EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.al_TextureNumberByBpp[1]- (char *) &st_DisplayData.st_TexManager), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Number of 8 bpp textures loaded in VRam",
        "Number in VRAM (16bpp)",       EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.al_TextureNumberByBpp[2]- (char *) &st_DisplayData.st_TexManager), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Number of 16 bpp textures loaded in VRam",
        "Number in VRAM (24bpp)",       EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.al_TextureNumberByBpp[3]- (char *) &st_DisplayData.st_TexManager), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Number of 24 bpp textures loaded in VRam",
        "Number in VRAM (32bpp)",       EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.al_TextureNumberByBpp[4]- (char *) &st_DisplayData.st_TexManager), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Number of 32 bpp textures loaded in VRam",
        "Number of textures in RAM",	EVAV_EVVIT_Int,			(void *) ((char *) &st_DisplayData.st_TexManager.l_RamTextureNumber - (char *) &st_DisplayData.st_TexManager), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Number of textures loaded in VRam",
		"Tex Slot 1",					EVAV_EVVIT_ConstInt,	(void *) ((char *) &st_DisplayData.st_TexManager.c_TexSlotOrder[0] - (char *)&st_DisplayData.st_TexManager), 0, 0, 0, 1, (int) "none\n255\nPS2\n0\nGameCube\n1\nXBox\n2\nPC\n3\n", 0, "Tex slot order",
		"Tex Slot 2",					EVAV_EVVIT_ConstInt,	(void *) ((char *) &st_DisplayData.st_TexManager.c_TexSlotOrder[1] - (char *)&st_DisplayData.st_TexManager), 0, 0, 0, 1, (int) "none\n255\nPS2\n0\nGameCube\n1\nXBox\n2\nPC\n3\n", 0, "Tex slot order",
		"Tex Slot 3",					EVAV_EVVIT_ConstInt,	(void *) ((char *) &st_DisplayData.st_TexManager.c_TexSlotOrder[2] - (char *)&st_DisplayData.st_TexManager), 0, 0, 0, 1, (int) "none\n255\nPS2\n0\nGameCube\n1\nXBox\n2\nPC\n3\n", 0, "Tex slot order",
		"Tex Slot 4",					EVAV_EVVIT_ConstInt,	(void *) ((char *) &st_DisplayData.st_TexManager.c_TexSlotOrder[3] - (char *)&st_DisplayData.st_TexManager), 0, 0, 0, 1, (int) "none\n255\nPS2\n0\nGameCube\n1\nXBox\n2\nPC\n3\n", 0, "Tex slot order",
		//"Force console",				EVAV_EVVIT_ConstInt,	(void *) ((char *) &st_DisplayData.st_TexManager.i_Console - (char *)&st_DisplayData.st_TexManager), 0, 0, 0, 0, (int) "PC\n0\nPS2\n1\nGameCube\n2\nXBox\n3\n", 0, "Console",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_FlagsDrawMask,
		"Draw Mask",
		LINK_CallBack_Refresh3DEngine,
		"Use Texture",			        EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "False : don't use textures",
		"Dont Force Color",		        EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "False : force vertex color (use display data forced color field)",
		"Dont Show B.V.",		        EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "False : show BV",
		"Draw It",				        EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "False : display not object",
		"Not Wire framed",		        EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "False : draw in wireframe mode",
		"Lighted",				        EVAV_EVVIT_Flags, (void *) 0, 0,27, 4, 0, 0, 0, "False : not lighted",
#ifdef JADEFUSION
        "Use map light settings",	    EVAV_EVVIT_Flags, (void *) 0, 0,26, 4, 0, 0, 0, "False : regular light settings used (actor ponderators are ignored)",
#endif
		"Computable vertex color",	    EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "False : vertex color are locked (not changed when computing RLI)",
		"Use vertex color",		        EVAV_EVVIT_Flags, (void *) 0, 0, 6, 4, 0, 0, 0, "False : do not use vertex color",
#ifdef JADEFUSION
		"Vertex color is dynamic",	    EVAV_EVVIT_Flags, (void *) 0, 0,28, 4, 0, 0, 0, "False : do not apply scale and offset to RLI",
#endif
		"Use material color",	        EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "False : do not use material color",
		"Use ambient color",	        EVAV_EVVIT_Flags, (void *) 0, 0, 8, 4, 0, 0, 0, "False : do not use ambient color (stored in world data)",
		"Dont Use ambient color 2",	    EVAV_EVVIT_Flags, (void *) 0, 0,22, 4, 0, 0, 0, "False : use normale ambient color",
		"Compute Specular ",	        EVAV_EVVIT_Flags, (void *) 0, 0,10, 4, 0, 0, 0, "False : do not compute specular light",
		"Test backface",		        EVAV_EVVIT_Flags, (void *) 0, 0,11, 4, 0, 0, 0, "False : no backface rejection",
		"Not Invert backface",	        EVAV_EVVIT_Flags, (void *) 0, 0,13, 4, 0, 0, 0, "False : frontface rejection",
		"Fogged",				        EVAV_EVVIT_Flags, (void *) 0, 0,14, 4, 0, 0, 0, "False : fog not applied",
		"Not Symetric",			        EVAV_EVVIT_Flags, (void *) 0, 0,15, 4, 0, 0, 0, "False : symetric object",
		"Receive dyn shadows",	        EVAV_EVVIT_Flags, (void *) 0, 0,16, 4, 0, 0, 0, "False : no dynamic shadow",
		"Activate skin",		        EVAV_EVVIT_Flags, (void *) 0, 0,17, 4, 0, 0, 0, "False : use not skin data",
#ifdef JADEFUSION
		"Receive shadow buffer",	    EVAV_EVVIT_Flags, (void *) 0, 0,18, 4, 0, 0, 0, "False : no dynamic shadow on this object",
		"Emit shadow buffer",			EVAV_EVVIT_Flags, (void *) 0, 0,19, 4, 0, 0, 0, "False : casts dynamic shadows",
		"Lightmap exclude from receivers", EVAV_EVVIT_Flags, (void *) 0, 0,10, 4, 0, 0, 0, "False : lightmap enabled for this object",
		"Lightmap exclude from casters",   EVAV_EVVIT_Flags, (void *) 0, 0,12, 4, 0, 0, 0, "False : consider object when computing lightmaps",
		"Use BV for light rejection",   EVAV_EVVIT_Flags, (void *) 0, 0,21, 4, 0, 0, 0, "False : will not use object BV to eliminate some light",
#else
		"Receive shadow buffer",	    EVAV_EVVIT_Flags, (void *) 0, 0,18, 4, 0, 0, 0, "",
#endif
		"Emit shadow buffer",			EVAV_EVVIT_Flags, (void *) 0, 0,19, 4, 0, 0, 0, "",
        "Use BV for light rejection",   EVAV_EVVIT_Flags, (void *) 0, 0,21, 4, 0, 0, 0, "True : use object BV to eliminate some light",
		"Not compute Normales",			EVAV_EVVIT_Flags, (void *) 0, 0,23, 4, 0, 0, 0, "False: Recompute normales",
        "Dont use RLI factor",			EVAV_EVVIT_Flags, (void *) 0, 0,25, 4, 0, 0, 0, "False: rli scale is used",
		"Dont attenuate light",			EVAV_EVVIT_Flags, (void *) 0, 0,30, 4, 0, 0, 0, "False: rli scale is used",
		"Reflect on water",				EVAV_EVVIT_Flags, (void *) 0, 0,31, 4, 0, 0, 0, "",
		"Dont Use Fog 2",				        EVAV_EVVIT_Flags, (void *) 0, 0,28, 4, 0, 0, 0, "True: 2nd Fog is used",
		"Sort this",					EVAV_EVVIT_Flags, (void *) 0, 0,29, 4, 0, 0, 0, "True: this object will not be sort",
		//"Do not Clip under water(PSX2 BUG)",	EVAV_EVVIT_Flags, (void *) 0, 0,26, 4, 0, 0, 0, "True : A little bit slower",
		"Dont Use AUTOClone",	EVAV_EVVIT_Flags, (void *) 0, 0,26, 4, 0, 0, 0, "False : do not use SUPER AUTO CLONAGE ATOMIQUE",
		-1
	);

	
	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ChannelFlag,
		"Active Channels",
		LINK_CallBack_Refresh3DEngine,
		"Chanel 0",				EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "",
		"Chanel 1",				EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "",
		"Chanel 2",				EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "",
		"Chanel 3",				EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "",
		"Chanel 4",				EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "",
		"Chanel 5",				EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "",
		"Chanel 6",				EVAV_EVVIT_Flags, (void *) 0, 0, 6, 4, 0, 0, 0, "",
		"Chanel 7",				EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "",
		"Chanel 8",				EVAV_EVVIT_Flags, (void *) 0, 0, 8, 4, 0, 0, 0, "",
		"Chanel 9",				EVAV_EVVIT_Flags, (void *) 0, 0, 9, 4, 0, 0, 0, "",
		"Chanel 10",				EVAV_EVVIT_Flags, (void *) 0, 0, 10, 4, 0, 0, 0, "",
		"Chanel 11",				EVAV_EVVIT_Flags, (void *) 0, 0, 11, 4, 0, 0, 0, "",
		"Chanel 12",				EVAV_EVVIT_Flags, (void *) 0, 0, 12, 4, 0, 0, 0, "",
		"Chanel 13",				EVAV_EVVIT_Flags, (void *) 0, 0, 13, 4, 0, 0, 0, "",
		"Chanel 14",				EVAV_EVVIT_Flags, (void *) 0, 0, 14, 4, 0, 0, 0, "",
		"Chanel 15",				EVAV_EVVIT_Flags, (void *) 0, 0, 15, 4, 0, 0, 0, "",
		-1
	);
	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_FlagsSelect32,
		"Sub material selector",
		LINK_CallBack_Refresh3DEngine,
		"Sub material 0",				EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 1",				EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 2",				EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 3",				EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 4",				EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 5",				EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 6",				EVAV_EVVIT_Flags, (void *) 0, 0, 6, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 7",				EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 8",				EVAV_EVVIT_Flags, (void *) 0, 0, 8, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 9",				EVAV_EVVIT_Flags, (void *) 0, 0, 9, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 10",				EVAV_EVVIT_Flags, (void *) 0, 0, 10, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 11",				EVAV_EVVIT_Flags, (void *) 0, 0, 11, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 12",				EVAV_EVVIT_Flags, (void *) 0, 0, 12, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 13",				EVAV_EVVIT_Flags, (void *) 0, 0, 13, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 14",				EVAV_EVVIT_Flags, (void *) 0, 0, 14, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 15",				EVAV_EVVIT_Flags, (void *) 0, 0, 15, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 16",				EVAV_EVVIT_Flags, (void *) 0, 0, 16, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 17",				EVAV_EVVIT_Flags, (void *) 0, 0, 17, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 18",				EVAV_EVVIT_Flags, (void *) 0, 0, 18, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 19",				EVAV_EVVIT_Flags, (void *) 0, 0, 19, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 20",				EVAV_EVVIT_Flags, (void *) 0, 0, 20, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 21",				EVAV_EVVIT_Flags, (void *) 0, 0, 21, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 22",				EVAV_EVVIT_Flags, (void *) 0, 0, 22, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 23",				EVAV_EVVIT_Flags, (void *) 0, 0, 23, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 24",				EVAV_EVVIT_Flags, (void *) 0, 0, 24, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 25",				EVAV_EVVIT_Flags, (void *) 0, 0, 25, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 26",				EVAV_EVVIT_Flags, (void *) 0, 0, 26, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 27",				EVAV_EVVIT_Flags, (void *) 0, 0, 27, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 28",				EVAV_EVVIT_Flags, (void *) 0, 0, 28, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 29",				EVAV_EVVIT_Flags, (void *) 0, 0, 29, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 30",				EVAV_EVVIT_Flags, (void *) 0, 0, 30, 4, 0, 0, 0, "False : don't use this material",
		"Sub material 31",				EVAV_EVVIT_Flags, (void *) 0, 0, 31, 4, 0, 0, 0, "False : don't use this material",
		-1
	);
	LINK_RegisterStructType
	(
		LINK_C_SPGFlags,
		"Sprite gen flags",
		LINK_CallBack_Refresh3DEngine,
		"Is transparent",				EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "modify this if you see nothing",
		"Sort sprites",					EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "set it to true if you want to sort sprites",
		"Use special fog",				EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "set it to true if you want a second", 
		"Special fog culling",			EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "set it to true if you don't want to display sprites farest from fog far", 
		"Move Grass",					EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "set it to true if you want spg to wave", 
		-1
	);
	
	LINK_RegisterStructType
	(
		LINK_C_SPG2Flags,
		"Sprite gen flags",
		LINK_CallBack_Refresh3DEngine,
		"Generate per vertex",			EVAV_EVVIT_Flags, (void *) 0, 0,18, 4, 0, 0, 0, "", 
		"Generate on surfface",			EVAV_EVVIT_Flags, (void *) 0, 0,19, 4, 0, 0, 0, "", 
		"Generate on Grid",				EVAV_EVVIT_Flags, (void *) 0, 0,25, 4, 0, 0, 0, "", 
		"Generate on IA Points",		EVAV_EVVIT_Flags, (void *) 0, 0,30, 4, 0, 0, 0, "", 
		"Vertex Alpha is Density",		EVAV_EVVIT_Flags, (void *) 0, 0, 6, 4, 0, 0, 0, "", 
		"Inverse Vertex Alpha",			EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "", 
		"Vertex Alpha is Size",			EVAV_EVVIT_Flags, (void *) 0, 0, 8, 4, 0, 0, 0, "", 
		"Debug:ShowLines",				EVAV_EVVIT_Flags, (void *) 0, 0, 9, 4, 0, 0, 0, "", 
		"Draw Hat",						EVAV_EVVIT_Flags, (void *) 0, 0,11, 4, 0, 0, 0, "", 
		"Draw X",						EVAV_EVVIT_Flags, (void *) 0, 0,12, 4, 0, 0, 0, "", 
		"Draw Y",						EVAV_EVVIT_Flags, (void *) 0, 0,13, 4, 0, 0, 0, "", 
		"Draw sprites",					EVAV_EVVIT_Flags, (void *) 0, 0,17, 4, 0, 0, 0, "", 
		"Preference Z",					EVAV_EVVIT_Flags, (void *) 0, 0,14, 4, 0, 0, 0, "", 
		"Preference XY",				EVAV_EVVIT_Flags, (void *) 0, 0,15, 4, 0, 0, 0, "", 
		"Only positive Z",				EVAV_EVVIT_Flags, (void *) 0, 0,16, 4, 0, 0, 0, "", 
		"Rotation Noise",				EVAV_EVVIT_Flags, (void *) 0, 0,20, 4, 0, 0, 0, "", 
		"Alpha draw (TEST ONLY !!!)",	EVAV_EVVIT_Flags, (void *) 0, 0,22, 4, 0, 0, 0, "TEST ONLY!!!", 
		"Sprite rotations",				EVAV_EVVIT_Flags, (void *) 0, 0,23, 4, 0, 0, 0, "", 
		"Don't use cache",				EVAV_EVVIT_Flags, (void *) 0, 0,24, 4, 0, 0, 0, "", 
		"Mode Fire",					EVAV_EVVIT_Flags, (void *) 0, 0,26, 4, 0, 0, 0, "", 
		"Use second Grid in burned mode",	EVAV_EVVIT_Flags, (void *) 0, 0,27, 4, 0, 0, 0, "", 
		"X Axis Is In lookat",			EVAV_EVVIT_Flags, (void *) 0, 0,28, 4, 0, 0, 0, "", 
		"Is lighted",       			EVAV_EVVIT_Flags, (void *) 0, 0,31, 4, 0, 0, 0, "", 		
		-1
	);
	
	LINK_RegisterStructType
	(
		LINK_C_SPG2Flags1,
		"Sprite gen flags (bis)",
		LINK_CallBack_Refresh3DEngineG1,
		"Damier",       				EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "", 		
		"Draw In BV",  					EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "", 		
		"Dont Generate",  				EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "", 		
		"Dont Display",  				EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "", 		
		"Damier 2",       				EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "", 		
		"Use RLI(vertex color) only",   EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "", 		
		"Use LOD",						EVAV_EVVIT_Flags, (void *) 0, 0, 6, 4, 0, 0, 0, "", 		
		"Use LOD2",						EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "", 		
#ifdef JADEFUSION
		"Heat Shimmer Enable",   		EVAV_EVVIT_Flags, (void *) 0, 0, 16, 4, 0, 0, 0, "", 	
#endif
		-1
	);


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_DisplayFlags,
		"Display Flags",
		LINK_CallBack_Refresh3DEngine,
		"Display Light",					EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "True : show light in 3D-view",
		"Display Camera",					EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "True : show camera in 3D-view",
		"Display Waypoint",					EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "True : show isolated waypoint in 3D-view",
		"Display network",					EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "True : show network in 3D-view",
		"Display Waypoint as BV",			EVAV_EVVIT_Flags, (void *) 0, 0, 6, 4, 0, 0, 0, "True : display waypoint BV instead of waypoint",
		"Display Invisible",				EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "True : display invisible object",
        "Display Visu without Gro",         EVAV_EVVIT_Flags, (void *) 0, 0,31, 4, 0, 0, 0, "True : display a question mark on object with visu but no gro",
		"Display Vertex Normals",			EVAV_EVVIT_Flags, (void *) 0, 0,10, 4, 0, 0, 0, "True : Show vertex normals",
		"Display Face Normals",				EVAV_EVVIT_Flags, (void *) 0, 0,23, 4, 0, 0, 0, "True : Show face normals",
		"Display normal only for selected", EVAV_EVVIT_Flags, (void *) 0, 0,18, 4, 0, 0, 0, "True : restrict normal display to selected vertices\nDisplay vertex normals flags have to be set",
        "Display AI Vectors"              , EVAV_EVVIT_Flags, (void *) 0, 0,24, 4, 0, 0, 0, "True : Display AI Vectors",  
        "Hide Interface"                  , EVAV_EVVIT_Flags, (void *) 0, 0,26, 4, 0, 0, 0, "True : Hide interface (text, fade ... ) ",  
        "Shade selected"                  , EVAV_EVVIT_Flags, (void *) 0, 0,27, 4, 0, 0, 0, "True : all not selected object are drawn in wireframe",  
        "Depth test when rendering"       , EVAV_EVVIT_Flags, (void *) 0, 0,29, 4, 0, 0, 0, "False : depth test are delayed just before the flip",  
		"Hide Shadows"					  , EVAV_EVVIT_Flags, (void *) 0, 0,25, 4, 0, 0, 0, "True : Dsiplay shadows",  
		-1
	);


    /*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_GFXShowMask,
		"GFX show mask",
		LINK_CallBack_Refresh3DEngine,
        "All",			EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "True : all gfx displayed",
		"Line",			EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "True : GFX of this type is displayed",
		"Lightning",	EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "True : GFX of this type is displayed",
		"Desintegrate",	EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "True : GFX of this type is displayed",
		"Flare",		EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "True : GFX of this type is displayed",
		"ComplexFlare",	EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "True : GFX of this type is displayed",
		"Smoke",		EVAV_EVVIT_Flags, (void *) 0, 0, 6, 4, 0, 0, 0, "True : GFX of this type is displayed",
		"Water",        EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "True : GFX of this type is displayed",
        "Speed",        EVAV_EVVIT_Flags, (void *) 0, 0, 8, 4, 0, 0, 0, "True : GFX of this type is displayed",
        "Spark",        EVAV_EVVIT_Flags, (void *) 0, 0, 9, 4, 0, 0, 0, "True : GFX of this type is displayed",
        "Water2",       EVAV_EVVIT_Flags, (void *) 0, 0,10, 4, 0, 0, 0, "True : GFX of this type is displayed",
        "Fade",         EVAV_EVVIT_Flags, (void *) 0, 0,11, 4, 0, 0, 0, "True : GFX of this type is displayed",
        "Profiler",     EVAV_EVVIT_Flags, (void *) 0, 0,12, 4, 0, 0, 0, "True : GFX of this type is displayed",
		"Explode",      EVAV_EVVIT_Flags, (void *) 0, 0,13, 4, 0, 0, 0, "True : GFX of this type is displayed",
        "BorderFade",   EVAV_EVVIT_Flags, (void *) 0, 0,14, 4, 0, 0, 0, "True : GFX of this type is displayed",
        "Diaphragm",    EVAV_EVVIT_Flags, (void *) 0, 0,15, 4, 0, 0, 0, "True : GFX of this type is displayed",
        "Carte",        EVAV_EVVIT_Flags, (void *) 0, 0,16, 4, 0, 0, 0, "True : GFX of this type is displayed",
        "Gao",          EVAV_EVVIT_Flags, (void *) 0, 0,17, 4, 0, 0, 0, "True : GFX of this type is displayed",
        "Sun",          EVAV_EVVIT_Flags, (void *) 0, 0,18, 4, 0, 0, 0, "True : GFX of this type is displayed",
        "String",       EVAV_EVVIT_Flags, (void *) 0, 0,19, 4, 0, 0, 0, "True : GFX of this type is displayed",
        "Ripple",       EVAV_EVVIT_Flags, (void *) 0, 0,20, 4, 0, 0, 0, "True : GFX of this type is displayed",
        "AnotherFlare", EVAV_EVVIT_Flags, (void *) 0, 0,21, 4, 0, 0, 0, "True : GFX of this type is displayed",
        -1
	);   

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_HelpersOptions,
		"Helpers options",
		LINK_CallBack_Refresh3DEngine,
		"All together",					EVAV_EVVIT_Flags, (char *) &st_Helpers.ul_Flags - (char *) &st_Helpers, 0, 12, 4, 0, 0, 0, "True to display all helpers together (translation + scale + rotation)",
		"Rotate around local center",	EVAV_EVVIT_Flags, (char *) &st_Helpers.ul_Flags - (char *) &st_Helpers, 0, 13, 4, 0, 0, 0, "True : when rotating multiselection all object rotate around it's local center",
		"Break rotation for hierarchy", EVAV_EVVIT_Flags, (char *) &st_Helpers.ul_Flags - (char *) &st_Helpers, 0, 15, 4, 0, 0, 0, "True : when rotating hierarchical linked object childs object doesn't move",
		"Hide while in use",			EVAV_EVVIT_Flags, (char *) &st_Helpers.ul_Flags - (char *) &st_Helpers, 0, 16, 4, 0, 0, 0, "True : When helper are in use, they disapear",
		-1
	);


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_StructDisplayData,
		"Display Data",
		LINK_CallBack_Refresh3DEngine,
		"Display Flags",			EVAV_EVVIT_SubStruct, (void *) ((char *) &st_DisplayData.ul_DisplayFlags - (char *) &st_DisplayData ), 0, LINK_C_DisplayFlags, 0, 0, 0, 0, "Display options",
        "GFX render mask",          EVAV_EVVIT_SubStruct, (void *) ((char *) &st_DisplayData.ul_GFXRenderMask - (char *) &st_DisplayData ), 0, LINK_C_GFXShowMask, 0, 0, 0, 0, "GFX render mask",
		"Draw Mask",				EVAV_EVVIT_SubStruct, (void *) ((char *) &st_DisplayData.ul_DrawMask - (char *) &st_DisplayData ), 0, LINK_C_FlagsDrawMask, 0, 0, 0, 0, "General draw mask (applied on all object)",
		"Light Atteruation",        EVAV_EVVIT_Float, (void *) ((char *) &st_DisplayData.f_LightAttenuation - (char *) &st_DisplayData), 0, 0.0f, 0.0f, 0, 0, 0, "attenuation factor for Light",
        "RLI factor",               EVAV_EVVIT_Float, (void *) ((char *) &st_DisplayData.f_RLIScale - (char *) &st_DisplayData), 0, 0.0f, 0.0f, 0, 0, 0, "multiplying factor for RLI",
        "RLI dest color",			EVAV_EVVIT_Color, (void *) ((char *) &st_DisplayData.ul_RLIColorDest - (char *) &st_DisplayData ), 0, 0, 0, 0, 0, 0, "Color blended with RLI (blend value is stored in RLI factor)",
        "Forced Color",				EVAV_EVVIT_Color, (void *) ((char *) &st_DisplayData.ul_ColorConstant - (char *) &st_DisplayData ), 0, 0, 0, 0, 0, 0, "Color used when vertex color is forced (see draww mask flags)",
		"Screen options",			EVAV_EVVIT_SubStruct, (void *) ((char *) &st_DisplayData.st_ScreenFormat - (char *) &st_DisplayData ), 0, LINK_C_GDI_ScreenFormat, 0, 0, 0, 0, "Screen options parameters",
		"Camera",					EVAV_EVVIT_SubStruct, (void *) ((char *) &st_DisplayData.st_Camera - (char *) &st_DisplayData ), 0, LINK_C_StructCamera, 0, 0, 0, 0, "Camera parameters",
		"World",					EVAV_EVVIT_Pointer, (void *) ((char *) &st_DisplayData.pst_World - (char *) &st_DisplayData ), 0, LINK_C_ENG_World, 0, 0, 0, 0, "World data",
		"Texture manager",			EVAV_EVVIT_SubStruct, (void *) ((char *) &st_DisplayData.st_TexManager - (char *) &st_DisplayData ), 0, LINK_C_StructTextureManager, 0, 0, 0, 0, "Texture manager parameters",
#ifdef JADEFUSION
		"Enable Color Correction",  EVAV_EVVIT_Bool, (void *) ((char *) &st_DisplayData.b_EnableColorCorrection - (char *) &st_DisplayData ), 0, 0, 0, 0, 0, 0, "",
#endif
		"Grid options",				EVAV_EVVIT_Pointer, (void *) ((char *) &st_DisplayData.pst_GridOptions - (char *) &st_DisplayData ), 0, LINK_C_ENG_GridDisplayOptions, 0, 0, 0, 0, "Grid display parameters",
		"Helper options",			EVAV_EVVIT_Pointer, (void *) ((char *) &st_DisplayData.pst_Helpers - (char *) &st_DisplayData ), 0, LINK_C_ENG_HelpersOptions, 0, 0, 0, 0, "Helpers parameters",
		"Sub object options",		EVAV_EVVIT_Pointer, (void *) ((char *) &st_DisplayData.pst_EditOptions- (char *) &st_DisplayData ), 0, LINK_C_GDI_EditOptions, 0, 0, 0, 0, "Sub object edition options",
        "Waypoint Size",            EVAV_EVVIT_Float, (void *) ((char *) &st_DisplayData.f_WPSize - (char *) &st_DisplayData), 0, 0.0f, 0.0f, 0, 0, 0, "minimal size of Waypoint (in meter)",
        "Waypoint flags",           EVAV_EVVIT_SubStruct, (void *) ((char *) &st_DisplayData.ul_WPFlags - (char *) &st_DisplayData ), 0, LINK_C_3DView_WPFlags, 0, 0, 0, 0, "Waypoint flags",
        "Ocean test",				EVAV_EVVIT_Pointer, (void *) ((char *) &st_DisplayData.pst_OceanExp - (char *) &st_DisplayData ), 0, LINK_C_GDI_OceanStruct, 0, 0, 0, 0, "Ocean Work",
        "AfterEffect in engine",	EVAV_EVVIT_Bool, (void *) ((char *) &st_DisplayData.ShowAEInEngine - (char *) &st_DisplayData ), 0, 0, 0, 0, 0, 0, "",
        "AfterEffect in editor",	EVAV_EVVIT_Bool, (void *) ((char *) &st_DisplayData.ShowAEEditor- (char *) &st_DisplayData ), 0, 0, 0, 0, 0, 0, "",
        "Global mul 2X",			EVAV_EVVIT_Bool, (void *) ((char *) &st_DisplayData.GlobalMul2X - (char *) &st_DisplayData ), 0, 0, 0, 0, 0, 0, "",
        "Global mul 2X factor",		EVAV_EVVIT_Float, (void *) ((char *) &st_DisplayData.GlobalMul2XFactor - (char *) &st_DisplayData), 0, 0.0f, 0.0f, 0, 0, 0, "0.0 -> no mul 2X \n1.0f normal mul 2X\n> 1.0f -> mul NX",
        "Show Alpha Buffer",		EVAV_EVVIT_Bool, (void *) ((char *) &st_DisplayData.ShowAlphaBuffer - (char *) &st_DisplayData ), 0, 0, 0, 0, 0, 0, "",
        "Clear Alpha Buffer in next load",		EVAV_EVVIT_Bool, (void *) ((char *) &st_DisplayData.ClearAlphaForGlowAtNextLoad - (char *) &st_DisplayData ), 0, 0, 0, 0, 0, 0, "",
        "Triangle alarme",			EVAV_EVVIT_Bool, (void *) ((char *) &st_DisplayData.DrawGraphicDebugInfo - (char *) &st_DisplayData ), 0, 0, 0, 0, 0, 0, "",
        "Triangle number alarme",	EVAV_EVVIT_Int		, (void *)((char *)&st_DisplayData.TRI_ALarm -(char *)&st_DisplayData)	, 0, 0, 0, 0, 0	, 0,"",
        "SPG2 number alarme",		EVAV_EVVIT_Int		, (void *)((char *)&st_DisplayData.SPG_ALarm -(char *)&st_DisplayData)	, 0, 0, 0, 0, 0	, 0,"",
        "Draw ratio alarme",		EVAV_EVVIT_Int		, (void *)((char *)&st_DisplayData.DRAW_ALarm -(char *)&st_DisplayData)	, 0, 0, 0, 0, 0	, 0,"",
        "Object number alarme",		EVAV_EVVIT_Int		, (void *)((char *)&st_DisplayData.OBJ_ALarm -(char *)&st_DisplayData)	, 0, 0, 0, 0, 0	, 0,"",
        "Small mesh display",		EVAV_EVVIT_Int		, (void *)((char *)&st_DisplayData.SMALL_ALarm -(char *)&st_DisplayData)	, 0, 0, 0, 0, 0	, 0,"",
		"Show histogramm",			EVAV_EVVIT_Bool, (void *)((char *)&st_DisplayData.ShowHistogramm -(char *)&st_DisplayData)	, 0, 0, 0, 0, 0	, 0,"",
#ifdef JADEFUSION
		"Light Culling Flags",	        EVAV_EVVIT_SubStruct, (void *) ((char *) &st_DisplayData.ul_LightCullingFlags - (char *) &st_DisplayData ), 0, LINK_C_LightCullingFlags, 0, 0, 0, 0, "Light culling flags",
		"Anti Aliasing Blur",	        EVAV_EVVIT_Bool, (void *) ((char *) &st_DisplayData.b_AntiAliasingBlur - (char *) &st_DisplayData ), 0, 0, 0, 0, 0, 0, "",
		"Draw WideScreen Bands",        EVAV_EVVIT_Bool, (void *) ((char *) &st_DisplayData.b_DrawWideScreenBands - (char *) &st_DisplayData ), 0, 0, 0, 0, 0, 0, "",
#endif
		"GlobalXInvert",			EVAV_EVVIT_Bool, (void *)((char *)&st_DisplayData.GlobalXInvert -(char *)&st_DisplayData)	, 0, 0, 0, 0, 0	, 0,"",
		"DisplayMemInfo",			EVAV_EVVIT_Bool, (void *)((char *)&st_DisplayData.DisplayMemInfo -(char *)&st_DisplayData)	, 0, 0, 0, 0, 0	, 0,"",
		"DisplayTriInfo",			EVAV_EVVIT_Bool, (void *)((char *)&st_DisplayData.DisplayTriInfo -(char *)&st_DisplayData)	, 0, 0, 0, 0, 0	, 0,"",
		"ColorCostIA",				EVAV_EVVIT_Bool, (void *)((char *)&st_DisplayData.ColorCostIA -(char *)&st_DisplayData)	, 0, 0, 0, 0, 0	, 0,"",
		"ColorCostIAThresh",		EVAV_EVVIT_Int, (void *)((char *)&st_DisplayData.ColorCostIAThresh -(char *)&st_DisplayData)	, 0, 0, 0, 0, 0	, 0,"",
#ifdef JADEFUSION 
		"Highlight Mode Colors",	EVAV_EVVIT_SubStruct, (void *) ((char *) &st_DisplayData.st_HighlightColors - (char *) &st_DisplayData ), 0, LINK_C_GDI_HighlightColors, 0, 0, 0, 0, "Highlight Colors",
#endif
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_GDK_StructId,
		"Struct ID",
		NULL,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_MDF_Modifier,
		"Modifier",
		NULL,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_GEO_ModifierOTCFlag,
		"WaVeYoUrBoDy Flags",
		NULL,
		"X axis",			EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "Local X axis is deformation axe or normal of deformation plane",
		"Y axis",			EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "Local Y axis is deformation axe or normal of deformation plane",
		"Z axis",			EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "Local Z axis is deformation axe or normal of deformation plane",
		"Planar",			EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "True : plane deformation, False : axis deformation",
		"Use alpha RLI",	EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "Deformation is blended by alpha value of vertex color",
		"Invert alpha",		EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "Deformation is blended by 1 - alpha value of vertex color",
		-1
	);

    /*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_GEO_ModifierSymetrieFlag,
		"Symetrie Flags",
		NULL,
		"X symetrie",	EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "Local X axis is normal of symetrie plane",
		"Y symetrie",	EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "Local Y axis is normal of symetrie plane",
		"Z symetri",	EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "Local Z axis is normal of symetrie plane",
		-1
	);
	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_GAO_ModifierSDWFlag,
		"Shadow generation flags",
		LINK_CallBack_Refresh3DEngine,
		"Activate",									EVAV_EVVIT_Flags, (void *) 0, 0,18, 4, 0, 0, 0, "ON / OFF",
		"One shadow each non-directional light ($)",EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "TRUE will generate one shadow for each spot & omni\nwitch have the flag \"emit real-time shadows\" set to TRUE",
		"One shadow each directional light",		EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "TRUE will generate one shadow for each directional\nwitch have the flag \"emit real-time shadows\" set to TRUE",
		"One Vertical shadow",						EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "Generate one vertical shadow",
		"One for object axis X",					EVAV_EVVIT_Flags, (void *) 0, 0,20, 4, 0, 0, 0, "Must be use with plane YZ.",
		"One for object axis Y",					EVAV_EVVIT_Flags, (void *) 0, 0,21, 4, 0, 0, 0, "Must be use with plane XZ.",
		"One for object axis Z",					EVAV_EVVIT_Flags, (void *) 0, 0,22, 4, 0, 0, 0, "Must be use with plane XY.",
		"Invert Dircetion",							EVAV_EVVIT_Flags, (void *) 0, 0,23, 4, 0, 0, 0, "MEGA-TRANS sueur Effect 2006\n",
		"Real-time shadow texture generation ($)",	EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "This is available only on DIRECTX8 & PSX2.",
		"Volumetric generation ($)",				EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 4, 4, 0, 0, 0, "Not yet implemented",
		"Apply on myself",							EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "",
		"Apply on dynamic",							EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "Emit shadow on caracters",
		"Apply on not dynamic",						EVAV_EVVIT_Flags, (void *) 0, 0, 8, 4, 0, 0, 0, "Emit shadow on world",
		"Eliminate if same father",					EVAV_EVVIT_Flags, (void *) 0, 0,12, 4, 0, 0, 0, "This flag permit to eliminate to shadowing objects included in the same anim",
		"Turn 90°",									EVAV_EVVIT_Flags, (void *) 0, 0,10, 4, 0, 0, 0, "Turn the texture of 90°",
		"Turn 180°",								EVAV_EVVIT_Flags, (void *) 0, 0,11, 4, 0, 0, 0, "Turn the texture of 90°",
		"Use normales",								EVAV_EVVIT_Flags, (void *) 0, 0,14, 4, 0, 0, 0, "Test the backface on destination shadowed object\nSame difference as PAINT LIGHT-CLASSIC LIGHT",
		"Use additional transparency",				EVAV_EVVIT_Flags, (void *) 0, 0,19, 4, 0, 0, 0, "Shadow will be additionned to destination\n (Normal mode is SUB)",
		"Always use shadow color",					EVAV_EVVIT_Flags, (void *) 0, 0,24, 4, 0, 0, 0, "...else it could use color of the light",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_GDK_LightFlag,
		"Light Flag",
		LINK_CallBack_Refresh3DEngine,
		"Active",					    EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "Light is active",
		"Paint",					    EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "Painting mode : when true don't take care of vertices normals",
#ifdef JADEFUSION
		"Extended Light",				EVAV_EVVIT_Flags, (void *) 0, 0,22, 4, 0, 0, 0, "True : Light will be used on Xenon only",
		"Extended Shadow Light",		EVAV_EVVIT_Flags, (void *) 0, 0,23, 4, 0, 0, 0, "True : Light is a shadow light used on Xenon only",
#endif
		"Use color",				    EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "Use color value",
		"Use alpha",				    EVAV_EVVIT_Flags, (void *) 0, 0, 6, 4, 0, 0, 0, "Use alpha value",
#ifdef JADEFUSION
		"Inverted (omni only)",		    EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "True : Omni is inverted",
#else
		"Absorb",					    EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "True : light is substractive, else light is additive",
#endif
		"Real time on dynam",		    EVAV_EVVIT_Flags, (void *) 0, 0, 8, 4, 0, 0, 0, "True : light is real time computed on dynamic object",
		"Real time on not dynam",	    EVAV_EVVIT_Flags, (void *) 0, 0, 9, 4, 0, 0, 0, "True : light is real time computed on not dynamic object",
		"RLI on dynam",				    EVAV_EVVIT_Flags, (void *) 0, 0,10, 4, 0, 0, 0, "True : light affect RLI of dynamic object when computing RLI",
		"RLI on not dynam",			    EVAV_EVVIT_Flags, (void *) 0, 0,11, 4, 0, 0, 0, "True : light affect RLI of not dynamic object when computing RLI",
		"Specular",					    EVAV_EVVIT_Flags, (void *) 0, 0,12, 4, 0, 0, 0, "True : light affect specular value",
		"RLI cast ray",				    EVAV_EVVIT_Flags, (void *) 0, 0,13, 4, 0, 0, 0, "True : when computing RLI we cast a ray to know if a vertex is lighted or not",
		"Emit Real-Time Shadows",	    EVAV_EVVIT_Flags, (void *) 0, 0,14, 4, 0, 0, 0, "True : Light emit real time shadow",
		"Cast Shadows",				    EVAV_EVVIT_Flags, (void *) 0, 0,15, 4, 0, 0, 0, "True : Light will cut object in radiosity computation",
		"Enable Penumbria",			    EVAV_EVVIT_Flags, (void *) 0, 0,19, 4, 0, 0, 0, "True : Shadows will be smooth \n (radius will be taken from near distance)",
        "Omni const (locally direct)",	EVAV_EVVIT_Flags, (void *) 0, 0,20, 4, 0, 0, 0, "True : omni direction is computed once per object (not once per vertex)",
		"Dont attenuate",				EVAV_EVVIT_Flags, (void *) 0, 0,21, 4, 0, 0, 0, "",
		"Exclusive light",				EVAV_EVVIT_Flags, (void *) 0, 0,22, 4, 0, 0, 0, "True : Progressivelly erase non-exclusives lights",
        "AddMat - Dont use normal mat",	EVAV_EVVIT_Flags, (void *) 0, 0,24, 4, 0, 0, 0, "True : normal material is ignored on object lighted by add material light",
        "AddMat - BV must be inside",	EVAV_EVVIT_Flags, (void *) 0, 0,25, 4, 0, 0, 0, "True : all BV of object must be inside of light BV",
        "Do not save Object reference",	EVAV_EVVIT_Flags, (void *) 0, 0,26, 4, 0, 0, 0, "True : objetc reference save is always NULL",
		"Bump",                         EVAV_EVVIT_Flags, (void *) 0, 0,28, 4, 0, 0, 0, "True : Bump Light",
        "Ocean specular",               EVAV_EVVIT_Flags, (void *) 0, 0,29, 4, 0, 0, 0, "True : This will enable the specular reflexion on the ocean",
#ifdef JADEFUSION
		"Spot is cylindrical",          EVAV_EVVIT_Flags, (void *) 0, 0,27, 4, 0, 0, 0, "True : cylindrical spot",
		"Lightmaps(shadow only)",       EVAV_EVVIT_Flags, (void *) 0, 0,30, 4, 0, 0, 0, "True : This ligth will be used when computing the ligthmaps",
		"Rim Light",                    EVAV_EVVIT_Flags, (void *) 0, 0,18, 4, 0, 0, 0, "True : This will enable rim light contribution from this light",
#else
		"Shadow buffer",                EVAV_EVVIT_Flags, (void *) 0, 0,30, 4, 0, 0, 0, "",
#endif
		"not replace Shadow buffer",    EVAV_EVVIT_Flags, (void *) 0, 0,31, 4, 0, 0, 0, "Set this flag to 1 if you want to \n desactivate this light when \n shadoaw buffer are enabled",
        -1
	);

    /*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_GDK_LightAddMaterialFlag,
		"Add material light flag",
		LINK_CallBack_Refresh3DEngine,
		"Dont use normal material",	EVAV_EVVIT_Flags, (void *) 0, 0, 0, 1, 0, 0, 0, "only added material will be used if this flag is set",
        "BV have to be inside",	EVAV_EVVIT_Flags, (void *) 0, 0, 1, 1, 0, 0, 0, "if set material will be added to object only if BV is inside Light BV",
		-1
	);
    /*---------------------------------------------------------------------------------------------------------------*/
    LINK_RegisterStructType
	(
		LINK_C_SPG_SpriteMapper,
		"Setting of Sprite Mapper",
		LINK_CallBack_Refresh3DEngine,
		"Submat num",		EVAV_EVVIT_Int		, (void *)((char *)&st_SPG_SpriteMapper.ulSubElementMaterialNumber -(char *)&st_SPG_SpriteMapper)	, 0, 0, 0, 0, 0	, 0,"",
		"Ratio Factor",		EVAV_EVVIT_Float	, (void *)((char *)&st_SPG_SpriteMapper.fRatioFactor -				(char *)&st_SPG_SpriteMapper) 	, 0, Cf_Zero, Cf_Infinit, 0, 0,  0,"",
		"Size Factor",		EVAV_EVVIT_Float	, (void *)((char *)&st_SPG_SpriteMapper.fSizeFactor -				(char *)&st_SPG_SpriteMapper)	, 0, Cf_Zero, Cf_Infinit, 0, 0	, 0,"",
		"Size NoIse Factor",EVAV_EVVIT_Float	, (void *)((char *)&st_SPG_SpriteMapper.fSizeNoiseFactor -			(char *)&st_SPG_SpriteMapper)	, 0, Cf_Zero, Cf_Infinit, 0, 0	, 0,"",
		-1
	);

    /*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_AI_Model,
		"AI Model",
		NULL,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_AI_Instance,
		"AI Instance",
		NULL,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_MSG_LiFo,
		"Message List",
		NULL,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_MSG_Struct,
		"Message",
		NULL,
		"Id",				EVAV_EVVIT_Int, (void *) ((char *) &st_Message.i_Id - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Sender",			EVAV_EVVIT_GO, (void *) ((char *) &st_Message.pst_Sender - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Object1",			EVAV_EVVIT_GO, (void *) ((char *) &st_Message.pst_GAO1 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Object2",			EVAV_EVVIT_GO, (void *) ((char *) &st_Message.pst_GAO2 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Object3",			EVAV_EVVIT_GO, (void *) ((char *) &st_Message.pst_GAO3 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Object4",			EVAV_EVVIT_GO, (void *) ((char *) &st_Message.pst_GAO4 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Object5",			EVAV_EVVIT_GO, (void *) ((char *) &st_Message.pst_GAO5 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Vec1",				EVAV_EVVIT_Vector, (void *) ((char *) &st_Message.st_Vec1 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Vec2",				EVAV_EVVIT_Vector, (void *) ((char *) &st_Message.st_Vec2 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Vec3",				EVAV_EVVIT_Vector, (void *) ((char *) &st_Message.st_Vec3 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Vec4",				EVAV_EVVIT_Vector, (void *) ((char *) &st_Message.st_Vec4 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Vec5",				EVAV_EVVIT_Vector, (void *) ((char *) &st_Message.st_Vec5 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Int1",				EVAV_EVVIT_Int, (void *) ((char *) &st_Message.i_Int1 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Int2",				EVAV_EVVIT_Int, (void *) ((char *) &st_Message.i_Int2 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Int3",				EVAV_EVVIT_Int, (void *) ((char *) &st_Message.i_Int3 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Int4",				EVAV_EVVIT_Int, (void *) ((char *) &st_Message.i_Int4 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		"Int5",				EVAV_EVVIT_Int, (void *) ((char *) &st_Message.i_Int5 - (char *) &st_Message), 0, 0, 0, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_World,
		"World",
		LINK_CallBack_Refresh3DEngine,
		"Nb Objects",		EVAV_EVVIT_Int,		(void *) ((char *) &st_World.ul_NbTotalGameObjects - (char *) &st_World), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Number of objects in world",
		"Nb Views",			EVAV_EVVIT_Int,		(void *) ((char *) &st_World.ul_NbViews - (char *) &st_World), EVAV_ReadOnly, 0, 0, 0, 0, 0, "Number of view in world",
		"LOD Cut",			EVAV_EVVIT_Int,		(void *) ((char *) &st_World.i_LODCut - (char *) &st_World), 0, 0, 0, 0, 0, 0, "LOD cut value",
		"Ambiant color",	EVAV_EVVIT_Color,	(void *) ((char *) &st_World.ul_AmbientColor - (char *) &st_World ), 0, 0, 0, 0, 0, 0, "Ambient color (to be used draw mask flag : use ambient color have to be set to true)",
		"Ambiant color 2",	EVAV_EVVIT_Color,	(void *) ((char *) &st_World.ul_AmbientColor2 - (char *) &st_World ), 0, 0, 0, 0, 0, 0, "Ambient color 2 (to be used draw mask flag : use ambient color have to be set to true)",
#ifdef _XENON_RENDER
        "Diffuse color multiplier",	        EVAV_EVVIT_Color,	(void *) ((char *) &st_World.ul_XeDiffuseColor - (char *) &st_World ), 0, 0, 0, 0, 0, 0, "Optional diffuse color that can multiply the material diffuse color",
        "Specular color multiplier",        EVAV_EVVIT_Color,	(void *) ((char *) &st_World.ul_XeSpecularColor - (char *) &st_World ), 0, 0, 0, 0, 0, 0, "Optional specular color that can multiply the material specular color",
        "Specular shininess multiplier",    EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeSpecularShiny - (char *) &st_World ), 0, 0.0f, 0.0f, 0, 0, 0, "Optional specular shininess multiplier",
        "Specular strength multiplier",     EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeSpecularStrength - (char *) &st_World ), 0, 0.0f, 0.0f, 0, 0, 0, "Optional specular strength multiplier",
		"Global RLI Scale",	EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeRLIScale - (char *) &st_World ), 0, 0.0f, 0.0f, 0, 0, 0, "Global RLI scale for this world",
		"Global RLI Offset",EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeRLIOffset - (char *) &st_World ), 0, 0.0f, 0.0f, 0, 0, 0, "Global RLI Offset for this world",
		"SPG2 Light",		EVAV_EVVIT_GO,		(void *) ((char *) &st_World.pSPG2Light - (char *) &st_World ), 0, 0, 0, 0, 0, 0, "SPG2 Directional light to use for this map",
		"Shadow Blur",      EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeGaussianStrength - (char *) &st_World ), 0, 0.0f, 0.01f, 0, 0, 0, "Gaussian blur strength",
        "Glow Luminosity Min", EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeGlowLuminosityMin - (char *) &st_World ), 0, 0.0f, 0.0f, 0, 0, 0, "Glow Luminosity Min",
        "Glow Luminosity Max", EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeGlowLuminosityMax - (char *) &st_World ), 0, 0.0f, 0.0f, 0, 0, 0, "Glow Luminosity Max",
		"Glow Intensity",   EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeGlowIntensity - (char *) &st_World ), 0, 0.0f, 0.0f, 0, 0, 0, "Glow intensity",
		"Glow Color",       EVAV_EVVIT_Color,	(void *) ((char *) &st_World.ul_XeGlowColor - (char *) &st_World ), 0, 0, 0, 0, 0, 0, "Glow Color for this world",
		"Glow Z-Near",      EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeGlowZNear - (char *) &st_World ), 0, 0.0f, 0.0f, 0, 0, 0, "Glow Z-Near",
		"Glow Z-Far",       EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeGlowZFar - (char *) &st_World ), 0, 0.0f, 0.0f, 0, 0, 0, "Glow Z-Far",
		"Rain Effect: Dry Diffuse Factor", EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeDryDiffuseFactor - (char *) &st_World ), 0, -100.0f, 100.0f, 0, 0, 0, "Dry Diffuse Factor (affects every light's diffuseFactor)",
		"Rain Effect: Wet Diffuse Factor", EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeWetDiffuseFactor - (char *) &st_World ), 0, -100.0f, 100.0f, 0, 0, 0, "Wet Diffuse Factor (affects every light's diffuseFactor)",
		"Rain Effect: Dry Specular Factor", EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeDrySpecularBoost - (char *) &st_World ), 0, -100.0f, 100.0f, 0, 0, 0, "Dry Specular Boost (affects every light's specularFactor)",
		"Rain Effect: Wet Specular Factor", EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeWetSpecularBoost - (char *) &st_World ), 0, -100.0f, 100.0f, 0, 0, 0, "Wet Specular Boost (affects every light's specularFactor)",
		"Rain Effect: Increase delay time", EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeRainEffectDelay - (char *) &st_World ), 0, 0.0f, 1000.0f, 0, 0, 0, "Rain Specular Delay (seconds until max specular/diffuse boost when raining)",
		"Rain Effect: Decrease delay time", EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeRainEffectDryDelay - (char *) &st_World ), 0, 0.0f, 1000.0f, 0, 0, 0, "Rain Specular Decay (seconds until min specular/diffuse boost when raining stops)",
        "God Ray Intensity", EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeGodRayIntensity - (char *) &st_World ), 0, 0.0f, 10.0f, 0, 0, 0, "God Ray Intensity",
        "God Ray Intensity Color",	EVAV_EVVIT_Color,	(void *) ((char *) &st_World.ul_XeGodRayIntensityColor - (char *) &st_World ), 0, 0, 0, 0, 0, 0, "God Ray Intensity Color",

        "Material LOD Detail Enable", EVAV_EVVIT_Bool,	(void *) ((char *) &st_World.b_XeMaterialLODDetailEnable - (char *) &st_World ), 0, 0, 0, 0, 0, 0, "Enable Material LOD Detail",
        "Material LOD Detail Near", EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeMaterialLODDetailNear - (char *) &st_World ), 0, 0.0f, 10000.0f, 0, 0, 0, "Material LOD Detail Near",
        "Material LOD Detail Far",	EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeMaterialLODDetailFar - (char *) &st_World ), 0, 0.0f, 10000.0f, 0, 0, 0, "Material LOD Detail Far",
        "Material LOD Enable", EVAV_EVVIT_Bool,	(void *) ((char *) &st_World.b_XeMaterialLODEnable - (char *) &st_World ), 0, 0, 0, 0, 0, 0, "Enable Material LOD",
        "Material LOD Near", EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeMaterialLODNear - (char *) &st_World ), 0, 0.0f, 10000.0f, 0, 0, 0, "Material LOD Near",
        "Material LOD Far",	EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeMaterialLODFar - (char *) &st_World ), 0, 0.0f, 10000.0f, 0, 0, 0, "Material LOD Far",
        "Saturation",	    EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeSaturation - (char *) &st_World ), 0, 0.0f, 1.0f, 0, 0, 0, "Saturation",
        "Brightness_R",	    EVAV_EVVIT_Float,	(void *) ((char *) &st_World.v_XeBrightness.x - (char *) &st_World ), 0, -10.0f, 10.0f, 0, 0, 0, "Brightness - Red",
        "Brightness_G",	    EVAV_EVVIT_Float,	(void *) ((char *) &st_World.v_XeBrightness.y - (char *) &st_World ), 0, -10.0f, 10.0f, 0, 0, 0, "Brightness - Green",
        "Brightness_B",	    EVAV_EVVIT_Float,	(void *) ((char *) &st_World.v_XeBrightness.z - (char *) &st_World ), 0, -10.0f, 10.0f, 0, 0, 0, "Brightness - Blue",
        "Contrast",	        EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeContrast - (char *) &st_World ), 0, -10.0f, 10.0f, 0, 0, 0, "Contrast",
#endif
		"Background color", EVAV_EVVIT_Color,	(void *) ((char *) &st_World.ul_BackgroundColor - (char *) &st_World ), 0, 0, 0, 0, 0, 0, "Background color",
#ifdef JADEFUSION
		"Mipmap LOD Bias",	EVAV_EVVIT_Float,	(void *) ((char *) &st_World.f_XeMipMapLODBias - (char *) &st_World ), 0, 0.0f, 0.0f, 0, 0, 0, "Global mipmap LOD bias for this world",
#endif
		"Grid (0)",			EVAV_EVVIT_Pointer, (void *) ((char *) &st_World.pst_Grid - (char *) &st_World ), 0, LINK_C_ENG_Grid, 0, 0, 0, 0, "Grid 0 parameters",
		"Grid (1)",			EVAV_EVVIT_Pointer, (void *) ((char *) &st_World.pst_Grid1 - (char *) &st_World ), 0, LINK_C_ENG_Grid, 0, 0, 0, 0, "Grid 1 parameters",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GameObjectTrace,
		"Trace",
		LINK_CallBack_GOChangeTraceParameters,
		"Nb saved pos",		EVAV_EVVIT_Int,		(void *) ((char *) &st_Trace.l_Number - (char *) &st_Trace), 0, 0, 0, 0, 0, 0, "Number of previous position saved",
		"Delay",			EVAV_EVVIT_Int,		(void *) ((char *) &st_Trace.l_Delay - (char *) &st_Trace), 0, 0, 0, 0, 0, 0, "Delay between two saved position",
		"Nb displayed pos", EVAV_EVVIT_Int,		(void *) ((char *) &st_Trace.l_NbDisplay - (char *) &st_Trace), 0, 0, 0, 0, 0, 0, "Delay between two saved position",
		"Z offset",			EVAV_EVVIT_Float,	(void *) ((char *) &st_Trace.f_ZOffset - (char *) &st_Trace), 0, 0.0f, 0.0f, 0, 0, 0, "Vertical offset for rendering point",
		"Color",			EVAV_EVVIT_Color,	(void *) ((char *) &st_Trace.ul_Color - (char *) &st_Trace ), 0, 0, 0, 0, 0, 0, "trace color",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GameObjectEditorInfos,
		"Editor Infos",
		NULL,
		"Flags",			EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_GO.ul_EditorFlags - (char *) &st_GO), 0, LINK_C_ENG_GO_EditorFlags, 0, 0, 0, 0, "Object editor flags",
		"Trace",			EVAV_EVVIT_Pointer,		(void *) ((char *) &st_GO.pst_Trace - (char*) &st_GO ), 0, LINK_C_ENG_GameObjectTrace, 0, 0, 0, 0, "Trace parameters",
		"WP texture index",	EVAV_EVVIT_Int,			(void *) ((char *) &st_GO.ul_InvisibleObjectIndex - (char *) &st_GO), 0, 0, 63, 0, 0, 0, "Index of invisible object : change display of object (used only if object is an invisible object)",
		"DontDisplayDebug", EVAV_EVVIT_Bool,		(void *) ((char *) &st_GO.b_DontDisplayDebug - (char *) &st_GO), 0, 0, 0, 0, 0, 0, "Do not display debug info in 3D view for this GAO",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_LOD,
		"LOD",
		NULL,
		"Visibility coeff", EVAV_EVVIT_Int,			(void *) ((char *) &st_GO.uc_VisiCoeff-(char*) &st_GO  ), 0, 0, 0, -1, 0, 0, "",
		"Visibility LOD",	EVAV_EVVIT_Int,			(void *) ((char *) &st_GO.uc_LOD_Vis - (char*) &st_GO  ), EVAV_ReadOnly, 0, 0, -1, 0, 0, "",
		"Activation LOD",	EVAV_EVVIT_Int,			(void *) ((char *) &st_GO.uc_LOD_Dist - (char*) &st_GO	), EVAV_ReadOnly, 0, 0, -1, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GameObjectOriented,
		"Game Object",
		NULL,
		"Name",						EVAV_EVVIT_String,		(void *) ((char *) &st_GO.sz_Name - (char *) &st_GO), EVAV_ReadOnly, 0, 0, 0, 0, 0, "",
		"Editor Infos",				EVAV_EVVIT_SubStruct,	(void *) 0, 0, LINK_C_ENG_GameObjectEditorInfos, 0, 0, 0, 0, "",
		"Identity",					EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_GO.ul_IdentityFlags - (char *) &st_GO), 0, LINK_C_ENG_GO_IdentityFlags, 0, 0, 0, 0, "",
		"Status & Control",			EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_GO.ul_StatusAndControlFlags- (char *) &st_GO), 0, LINK_C_ENG_GO_StatusAndControlFlags, 0, 0, 0, 0, "",
		"Type",						EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_GO.uc_DesignFlags - (char *) &st_GO), 0, LINK_C_ENG_GO_DesignFlags, 0, 0, 0, 0, "",
		"Fix",						EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_GO.c_FixFlags - (char *) &st_GO), 0, LINK_C_ENG_GO_FixFlags, 0, 0, 0, 0, "",
		"Bounding Volume",			EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_GO.pst_BV - (char*) &st_GO ), 0, LINK_C_StructBV, 0, 0, 0, 0, "",
		"Position / Orientation",	EVAV_EVVIT_Pointer,		(void *) ((char *) &st_GO.pst_GlobalMatrix - (char*) &st_GO ), 0, LINK_C_Matrix, 0, 0, 0, 0, "",
		"Base",						EVAV_EVVIT_Pointer,		(void *) ((char *) &st_GO.pst_Base - (char*) &st_GO ), 0, LINK_C_ENG_GO_Base, 0, 0, 0, 0, "",
		"Extended",					EVAV_EVVIT_Pointer,		(void *) ((char *) &st_GO.pst_Extended - (char*) &st_GO ), 0, LINK_C_ENG_GO_Extended, 0, 0, 0, 0, "",
		"LOD",						EVAV_EVVIT_SubStruct,	(void *) 0, 0, LINK_C_LOD, 0, 0, 0, 0, "",
		"LOD min value",			EVAV_EVVIT_Int,			(void *) ((char *) &st_GO.ucCullingVisibility - (char*) &st_GO ), 0, 0, 0, -1, 0, 0, "",
		-1
	);


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_IdentityFlags,
		"Identity",
		LINK_CallBack_ChangeIdentityFlags,
        //"Bone",				    EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "Object has complex animations",
		"Anims",				EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "Object has complex animations",
		"Dynam",				EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "Object has dynamic",
		"Light",				EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "Object is a light",
		"AI",					EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "Object has AI",
		"Events",				EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 25, 4, 0, 0, 0, "Object has event tracks",
		"Sound",				EVAV_EVVIT_Flags, (void *) 0, 0, 27, 4, 0, 0, 0, "Object with a sound bank",
		"Design Struct",		EVAV_EVVIT_Flags, (void *) 0, 0, 6, 4, 0, 0, 0, "Object has a designer struct",
		"Waypoint",				EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "Object is a waypoint",
		"Design Helper",		EVAV_EVVIT_Flags, (void *) 0, 0, 20, 4, 0, 0, 0, "Object is a design helper",
		"ColMap",				EVAV_EVVIT_Flags, (void *) 0, 0, 8, 4, 0, 0, 0, "Object has a colmap",
		"ZDM",					EVAV_EVVIT_Flags, (void *) 0, 0, 9, 4, 0, 0, 0, "Object has a ZDM",
		"ZDE",					EVAV_EVVIT_Flags, (void *) 0, 0, 10, 4, 0, 0, 0, "Object has a detection zone",
		"Draw at end (fps specific)",	EVAV_EVVIT_Flags, (void *) 0, 0, 11, 4, 0, 0, 0, "Nothing between camera and Object",
		"Base",					EVAV_EVVIT_Flags, (void *) 0, 0, 12, 4, 0, 0, 0, "Object has a base struct",
		"Extended",				EVAV_EVVIT_Flags, (void *) 0, 0, 13, 4, 0, 0, 0, "Object has an extended struct",
		"Visual",				EVAV_EVVIT_Flags, (void *) 0, 0, 14, 4, 0, 0, 0, "",
		"Msg",					EVAV_EVVIT_Flags, (void *) 0, 0, 15, 4, 0, 0, 0, "",
		"Init Pos",				EVAV_EVVIT_Flags, (void *) 0, 0, 16, 4, 0, 0, 0, "Object has an initial pos",
		"Generated",			EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 17, 4, 0, 0, 0, "Object has been generated by AI",
		"In Network",			EVAV_EVVIT_Flags, (void *) 0, 0, 18, 4, 0, 0, 0, "Object is in a network",
		"OBBox",				EVAV_EVVIT_Flags, (void *) 0, 0, 19, 4, 0, 0, 0, "",
		"Additional matrix",	EVAV_EVVIT_Flags, (void *) 0, 0, 21, 4, 0, 0, 0, "Object has gizmos (matrix)",
		"Additional matrix Ptr",EVAV_EVVIT_Flags, (void *) 0, 0, 24, 4, 0, 0, 0, "Object has gizmos (pointers to matrix)",
		"Hierarchy",			EVAV_EVVIT_Flags, (void *) 0, 0, 22, 4, 0, 0, 0, "Object has a hierarchy",
		"Group",				EVAV_EVVIT_Flags, (void *) 0, 0, 23, 4, 0, 0, 0, "",
		"ODE",					EVAV_EVVIT_Flags, (void *) 0, 0, 28, 4, 0, 0, 0, "Object visible in ODE solver",
		"No SPG2",				EVAV_EVVIT_Flags, (void *) 0, 0, 29, 4, 0, 0, 0, "No GRID SPG2 on this object",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_EditorFlags,
		"Editor flags",
		LINK_CallBack_GOChangeEditorFlag,
		"Selected",				EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 0, 4, 0, 0, 0, "Selected Object",
		"Show BV",				EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "Display BV",
		"Show OBBox",			EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "Display additional BV",
#ifdef _XENON_RENDER
		"Show Element BV",	    EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "Display per element BV",
#endif
		"Show trajectory",		EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "Display trace (old position)",
		"Show Gizmo",			EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "Display gizmo",
		"Force LOD",			EVAV_EVVIT_Flags, (void *) 0, 0,10, 4, 0, 0, 0, "Force LOD (use Forced LOD index to set LOD to display)",
		"Wired Cob",			EVAV_EVVIT_Flags, (void *) 0, 0,13, 4, 0, 0, 0, "Force Mathematical Cobs to be rendered in Wireframe",
		"Need SnP",				EVAV_EVVIT_Flags, (void *) 0, 0,17, 4, 0, 0, 0, "During the Game, we detect if the Object must be in SnP",
		"Display in 'R' Mode",	EVAV_EVVIT_Flags, (void *) 0, 0,18, 4, 0, 0, 0, "In ColMap Display, Force the Visu of the GO to be displayed",
		"Log Debug Info",		EVAV_EVVIT_Flags, (void *) 0, 0,19, 4, 0, 0, 0, "Trace",
        "RLI Locked",           EVAV_EVVIT_Flags, (void *) 0, 0,21, 4, 0, 0, 0, "RLI are locked",
        "Show slope",           EVAV_EVVIT_Flags, (void *) 0, 0,22, 4, 0, 0, 0, "draw wall / floor with different color",
		"Radiosity:Transparent",EVAV_EVVIT_Flags, (void *) 0, 0,28, 4, 0, 0, 0, "",
        "Radiosity:Don't cut me",  EVAV_EVVIT_Flags, (void *) 0, 0,29, 4, 0, 0, 0, "",
        "Radiosity:Don't emit penombria",  EVAV_EVVIT_Flags, (void *) 0, 0,30, 4, 0, 0, 0, "",
        "Radiosity:Don't emit shadow volume",  EVAV_EVVIT_Flags, (void *) 0, 0,31, 4, 0, 0, 0, "",


		"Radiosity:Channel 0",EVAV_EVVIT_Flags, (void *) 0, 0,23, 4, 0, 0, 0, "",
		"Radiosity:Channel 1",EVAV_EVVIT_Flags, (void *) 0, 0,24, 4, 0, 0, 0, "",
		"Radiosity:Channel 2",EVAV_EVVIT_Flags, (void *) 0, 0,25, 4, 0, 0, 0, "",
		"Radiosity:Channel 3",EVAV_EVVIT_Flags, (void *) 0, 0,26, 4, 0, 0, 0, "",
		"Radiosity:Channel 4",EVAV_EVVIT_Flags, (void *) 0, 0,27, 4, 0, 0, 0, "",
		"Radiosity:Channel 5",EVAV_EVVIT_Flags, (void *) 0, 0,14, 4, 0, 0, 0, "",
		"Radiosity:Channel 6",EVAV_EVVIT_Flags, (void *) 0, 0,7, 4, 0, 0, 0, "",

		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_DesignFlags,
		"Type",
		LINK_CallBack_Refresh3DEngine,
		"Friend",				EVAV_EVVIT_Flags, (void *) 0, 0, 0, 1, 0, 0, 0, "",
		"Enemy",				EVAV_EVVIT_Flags, (void *) 0, 0, 1, 1, 0, 0, 0, "",
		"Projectile",			EVAV_EVVIT_Flags, (void *) 0, 0, 2, 1, 0, 0, 0, "",
		"Custom1",				EVAV_EVVIT_Flags, (void *) 0, 0, 3, 1, 0, 0, 0, "",
		"Custom2",				EVAV_EVVIT_Flags, (void *) 0, 0, 4, 1, 0, 0, 0, "",
		"Dodge Camera",			EVAV_EVVIT_Flags, (void *) 0, 0, 5, 1, 0, 0, 0, "",
		"Space",				EVAV_EVVIT_Flags, (void *) 0, 0, 6, 1, 0, 0, 0, "",
		"Custom5",				EVAV_EVVIT_Flags, (void *) 0, 0, 7, 1, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_FixFlags,
		"Fix",
		LINK_CallBack_Refresh3DEngine,
		"b0 (init)",			EVAV_EVVIT_Flags, (void *) 0, 0, 2, 1, 0, 0, EVAV_Filter_Init, "",
		"b1 (init)",			EVAV_EVVIT_Flags, (void *) 0, 0, 3, 1, 0, 0, EVAV_Filter_Init, "",
		"b0",					EVAV_EVVIT_Flags, (void *) 0, 0, 0, 1, 0, 0, EVAV_Filter_Cur, "",
		"b1",					EVAV_EVVIT_Flags, (void *) 0, 0, 1, 1, 0, 0, EVAV_Filter_Cur, "",
		"Has Been Merge",		EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 4, 1, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_StatusAndControlFlags,
		"Status And Control",
		LINK_CallBack_ChangeControlFlags,
		"Active",					EVAV_EVVIT_Flags,		(void *) 0, EVAV_ReadOnly, 0, 4, 0, 0, 0, "",
		"Visible",					EVAV_EVVIT_Flags,		(void *) 0, EVAV_ReadOnly, 1, 4, 0, 0, 0, "",
		"Culled",					EVAV_EVVIT_Flags,		(void *) 0, EVAV_ReadOnly, 2, 4, 0, 0, 0, "",
		"Real Time Lightned",		EVAV_EVVIT_Flags,		(void *) 0, EVAV_ReadOnly, 3, 4, 0, 0, 0, "",
		"Detection List",			EVAV_EVVIT_Flags,		(void *) 0, EVAV_ReadOnly, 5, 4, 0, 0, 0, "",
		"Has Child",				EVAV_EVVIT_Flags,		(void *) 0, EVAV_ReadOnly, 6, 4, 0, 0, 0, "",
		"Custom Bits",				EVAV_EVVIT_SubStruct,	(void *) 0, 0, LINK_C_CustomBits, 0, 0, 0, 0, "",
		"Enable SnP",				EVAV_EVVIT_Flags,		(void *) 0, 0, 7, 4, 0, 0, 0, "",
		"Secto Reinit",				EVAV_EVVIT_Flags,		(void *) 0, 0, 8, 4, 0, 0, 0, "",
		"Secto Invisible",			EVAV_EVVIT_Flags,		(void *) 0, EVAV_ReadOnly, 16, 4, 0, 0, 0, "",
		"Secto Inactive",			EVAV_EVVIT_Flags,		(void *) 0, EVAV_ReadOnly, 17, 4, 0, 0, 0, "",
		"Force RTL",				EVAV_EVVIT_Flags,		(void *) 0, 0, 18, 4, 0, 0, 0, "",
		"Force No RTL",				EVAV_EVVIT_Flags,		(void *) 0, 0, 19, 4, 0, 0, 0, "",
		"Force Detection List",		EVAV_EVVIT_Flags,		(void *) 0, 0, 20, 4, 0, 0, 0, "",
		"Force Invisible (Init)",	EVAV_EVVIT_Flags,		(void *) 0, 0, 29, 4, 0, 0, EVAV_Filter_Init, "",
		"Force Inactive (Init)",	EVAV_EVVIT_Flags,		(void *) 0, 0, 30, 4, 0, 0, EVAV_Filter_Init, "",
		"Force Invisible",			EVAV_EVVIT_Flags,		(void *) 0, 0, 21, 4, 0, 0, EVAV_Filter_Cur, "",
		"Force Inactive",			EVAV_EVVIT_Flags,		(void *) 0, 0, 22, 4, 0, 0, EVAV_Filter_Cur, "",
		"Look At Camera",			EVAV_EVVIT_Flags,		(void *) 0, 0, 23, 4, 0, 0, 0, "",
		"Ray Insensitive",			EVAV_EVVIT_Flags,		(void *) 0, 0, 24, 4, 0, 0, 0, "",
		"Editable BV",				EVAV_EVVIT_Flags,		(void *) 0, 0, 25, 4, 0, 0, 0, "",
		"In Pause",					EVAV_EVVIT_Flags,		(void *) 0, 0, 26, 4, 0, 0, 0, "",
		"Anim Dont Touch",			EVAV_EVVIT_Flags,		(void *) 0, 0, 31, 4, 0, 0, 0, "",
		"Always Active",			EVAV_EVVIT_Flags,		(void *) 0, 0, 27, 4, 0, 0, 0, "",
		"Always Visible",			EVAV_EVVIT_Flags,		(void *) 0, 0, 28, 4, 0, 0, 0, "",
		-1
	);


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_ExtraFlags,
		"Extra Flags",
#ifdef JADEFUSION
		LINK_CallBack_ChangeExtraFlags,
#else
		NULL,
#endif
		"Optim Anim",				EVAV_EVVIT_Flags,		(void *) 0, 0, 0, 4, 0, 0, 0, "",
		"Optim Culled",				EVAV_EVVIT_Flags,		(void *) 0, 0, 2, 4, 0, 0, 0, "",
		"LOD dist AI",				EVAV_EVVIT_Flags,		(void *) 0, 0, 3, 4, 0, 0, 0, "",
		"COL Max Prio",				EVAV_EVVIT_Flags,		(void *) 0, 0, 4, 4, 0, 0, 0, "",
		"Optim Bone",				EVAV_EVVIT_Flags,		(void *) 0, 0, 5, 4, 0, 0, 0, "",
		"Optim Sect Acti Vis",		EVAV_EVVIT_Flags,		(void *) 0, 0, 6, 4, 0, 0, 0, "",
		"Skip Anim Translations",	EVAV_EVVIT_Flags,		(void *) 0, 0, 7, 4, 0, 0, 0, "",
		"Slow/Fast Off",			EVAV_EVVIT_Flags,		(void *) 0, 0, 8, 4, 0, 0, 0, "",
		"Skip Translations On Bone",EVAV_EVVIT_Flags,		(void *) 0, 0, 9, 4, 0, 0, 0, "",
		"Skip Rotations On Bone",	EVAV_EVVIT_Flags,		(void *) 0, 0, 10, 4, 0, 0, 0, "",
		"Vec Blend Rotate",			EVAV_EVVIT_Flags,		(void *) 0, 0, 11, 4, 0, 0, 0, "",
		"No Blend On Bone",			EVAV_EVVIT_Flags,		(void *) 0, 0, 12, 4, 0, 0, 0, "",
		"Always Play Anim",			EVAV_EVVIT_Flags,		(void *) 0, 0, 13, 4, 0, 0, 0, "",
		"Dont reinit pos",			EVAV_EVVIT_Flags,		(void *) 0, 0, 14, 4, 0, 0, 0, "",
		"Special Rec",				EVAV_EVVIT_Flags,		(void *) 0, 0, 15, 4, 0, 0, 0, "",
#ifdef JADEFUSION
		"Affected by dynamic wind",		EVAV_EVVIT_Flags,       (void *) 0, 0, 14, 4, 0, 0, 0, "",
		"Affected by static wind",      EVAV_EVVIT_Flags,       (void *) 0, 0, 15, 4, 0, 0, 0, "",
#endif
		-1
	);


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_Base,
		"Base",
		NULL,
		"Visuel",				EVAV_EVVIT_Pointer, (void *)((char *) &st_Base.pst_Visu - (char*) &st_Base ), 0, LINK_C_ENG_GO_Visu, 0, 0, 0, 0, "Visuel of object",
		"Dynamic",				EVAV_EVVIT_Pointer, (void *) ((char *) &st_Base.pst_Dyna - (char *) &st_Base), 0, LINK_C_ENG_GO_Dyna, 0, 0, 0, 0, "Dynamic description",
		"Hierarchy",			EVAV_EVVIT_Pointer, (void *) ((char *) &st_Base.pst_Hierarchy - (char *) &st_Base), 0, LINK_C_ENG_GO_BaseHierarchy, 0, 0, 0, 0, "Hierarchy description",
#ifdef ODE_INSIDE
		"ODE",					EVAV_EVVIT_Pointer, (void *) ((char *) &st_Base.pst_ODE - (char *) &st_Base), 0, LINK_C_ENG_ODE, 0, 0, 0, 0, "ODE",
#endif
		"Additional matrix",	EVAV_EVVIT_Pointer, (void *) ((char *) &st_Base.pst_AddMatrix - (char *) &st_Base), 0, LINK_C_ENG_GO_BaseAdditionalMatrix, 0, 0, 0, 0, "Gizmos description",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_BaseHierarchy,
		"Hierarchy",
		LINK_CallBack_Refresh3DEngine,
		"Father",		EVAV_EVVIT_GO, (void *)((char *) &st_GOHierarchy.pst_Father - (char*) &st_GOHierarchy ), 0, 0, 0, 0, 0, 0, "Father object",
		"Father (Init)",EVAV_EVVIT_GO, (void *)((char *) &st_GOHierarchy.pst_FatherInit - (char*) &st_GOHierarchy ), 0, 0, 0, 0, 0, EVAV_Filter_Init, "Father object",
		"Local Matrix", EVAV_EVVIT_SubStruct, (void *)((char *) &st_GOHierarchy.st_LocalMatrix - (char*) &st_GOHierarchy ), 0, LINK_C_Matrix, 0, 0, 0, 0, "Local position and rotation (in father object system axis)",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_BaseAdditionalMatrix,
		"Additional matrix",
		NULL,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_BaseAnim,
		"Base",
		NULL,
		"Animation(s)",			EVAV_EVVIT_Pointer, (void *)((char *) &st_Base.pst_GameObjectAnim - (char *) &st_Base ), 0, LINK_C_ENG_GameObjectAnim, 0, 0, 0, 0, "Complex animation description",
		"Dynamic",				EVAV_EVVIT_Pointer, (void *) ((char *) &st_Base.pst_Dyna - (char *) &st_Base), 0, LINK_C_ENG_GO_Dyna, 0, 0, 0, 0, "Dynamic description",
#ifdef ODE_INSIDE
		"ODE",					EVAV_EVVIT_Pointer, (void *) ((char *) &st_Base.pst_ODE - (char *) &st_Base), 0, LINK_C_ENG_ODE, 0, 0, 0, 0, "ODE",
#endif
		"Hierarchy",			EVAV_EVVIT_Pointer, (void *) ((char *) &st_Base.pst_Hierarchy - (char *) &st_Base), 0, LINK_C_ENG_GO_BaseHierarchy, 0, 0, 0, 0, "Hierarchy description",
		"Additional matrix",	EVAV_EVVIT_Pointer, (void *) ((char *) &st_Base.pst_AddMatrix - (char *) &st_Base), 0, LINK_C_ENG_GO_BaseAdditionalMatrix, 0, 0, 0, 0, "Gizmos description",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GameObjectAnim,
		"GO Anim",
		NULL,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_Anim,
		"Anim",
		NULL,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_BlendAnim,
		"Blend Anim",
		NULL,
		-1
	);


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_ActionKit,
		"Action kit",
		LINK_CallBack_RefreshAction,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_Action,
		"Action",
		LINK_CallBack_RefreshAction,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_AnimForAction,
		"Anim Action",
		NULL,
		"Anim",					EVAV_EVVIT_Anim,  0, 0, 1, 0, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_Skeleton,
		"Skeleton",
		NULL,
		"Skeleton",				EVAV_EVVIT_Skeleton, 0, 0, 1, 0, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_Skin,
		"Skin",
		NULL,
		"Skin",					EVAV_EVVIT_Skin, 0, 0, 1, 0, 0, 0, 0, "",
		-1
	);
	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_MBFlags,
		"Magic Box Flags",
		NULL,
		"Freeze",					EVAV_EVVIT_Flags, (void *) 0, 0, 2, 1, 0, 0, 0, "",
		"Use Last Frame Speed",		EVAV_EVVIT_Flags, (void *) 0, 0, 5, 1, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_AnimFlags,
		"Anim Flags",
		NULL,
		"Animation finished",			EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 0, 1, 0, 0, 0, "",
		"(Mode 0) Speed From Anim",		EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 1, 1, 0, 0, 0, "",
		"(Mode 1) Speed-Frequency",		EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 2, 1, 0, 0, 0, "",
		"(Mode 3) Speed From Dyna",		EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 3, 1, 0, 0, 0, "",
		-1
	);


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_BlendFlags,
		"Blend Flags",
		NULL,
		"Progressive",					EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 0, 2, 0, 0, 0, "",
		"Inv. Progressive",				EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 1, 2, 0, 0, 0, "",
		"Freeze Bones",					EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 2, 2, 0, 0, 0, "",
		"Freeze Magic Box",				EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 3, 2, 0, 0, 0, "",
		"Stock Matrixes",				EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 5, 2, 0, 0, 0, "",
		"Interpolate Magic Box",		EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 10, 2, 0, 0, 0, "",
		"AI Bones Control",				EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 5, 2, 0, 0, 0, "",
		-1
	);



	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_ActionItem,
		"Action Item",
		NULL,
//        "Anim",                 EVAV_EVVIT_Anim,        (void *) ((char *) &st_ActionItem.pst_Data - (char *) &st_ActionItem), 0, 0, 0, 0, 0, 0, "",
		"Repetition",			EVAV_EVVIT_Int,			(void *) ((char *) &st_ActionItem.uc_Repetition - (char *) &st_ActionItem), 0, 0, 0, 1, 0, 0, "",
		"Nb frames for blend",	EVAV_EVVIT_Int,			(void *) ((char *) &st_ActionItem.uc_NumberOfFrameForBlend - (char *) &st_ActionItem), 0, 0, 0, 1, 0, 0, "",
		"Anim frequency",		EVAV_EVVIT_Int,			(void *) ((char *) &st_ActionItem.uc_Frequency - (char *) &st_ActionItem), 0, 0, 0, 1, 0, 0, "",
		"Custom bits",			EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_ActionItem.uc_CustomBit - (char *) &st_ActionItem), 0, LINK_C_ENG_ActionItemCustomBit, 0, 0, 0, 0, "",
		"Design Flags",			EVAV_EVVIT_Int,			(void *) ((char *) &st_ActionItem.uw_DesignFlags - (char *) &st_ActionItem), 0, 0, 0, 2, 0, 0, "",
		"Flags",				EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_ActionItem.uc_Flag - (char *) &st_ActionItem), 0, LINK_C_ENG_ActionItemFlag, 0, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_ActionItemCustomBit,
		"Custom bits",
		NULL,
		"CB 0",					EVAV_EVVIT_Flags, (void *) 0, 0, 0, 1, 0, 0, 0, "",
		"CB 1",					EVAV_EVVIT_Flags, (void *) 0, 0, 1, 1, 0, 0, 0, "",
		"CB 2",					EVAV_EVVIT_Flags, (void *) 0, 0, 2, 1, 0, 0, 0, "",
		"CB 3",					EVAV_EVVIT_Flags, (void *) 0, 0, 3, 1, 0, 0, 0, "",
		"CB 4",					EVAV_EVVIT_Flags, (void *) 0, 0, 4, 1, 0, 0, 0, "",
		"CB 5",					EVAV_EVVIT_Flags, (void *) 0, 0, 5, 1, 0, 0, 0, "",
		"CB 6",					EVAV_EVVIT_Flags, (void *) 0, 0, 6, 1, 0, 0, 0, "",
		"CB 7",					EVAV_EVVIT_Flags, (void *) 0, 0, 7, 1, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_ActionItemFlag,
		"Flags",
		NULL,
		"Mode (bit 0)",					EVAV_EVVIT_Flags, (void *) 0, 0, 1, 1, 0, 0, 0, "",
		"Mode (bit 1)",					EVAV_EVVIT_Flags, (void *) 0, 0, 2, 1, 0, 0, 0, "",
		"Mode (bit 2)",					EVAV_EVVIT_Flags, (void *) 0, 0, 3, 1, 0, 0, 0, "",
		"Ignore gravity",				EVAV_EVVIT_Flags, (void *) 0, 0, 4, 1, 0, 0, 0, "",
		"Ignore traction",				EVAV_EVVIT_Flags, (void *) 0, 0, 5, 1, 0, 0, 0, "",
		"Ignore stream",				EVAV_EVVIT_Flags, (void *) 0, 0, 6, 1, 0, 0, 0, "",
		"Ignore vertical translation",	EVAV_EVVIT_Flags, (void *) 0, 0, 7, 1, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_Group,
		"Group",
		NULL,
		"Name",					EVAV_EVVIT_String,	(void *) ((char *) &st_Group.sz_Name - (char *) &st_Group), EVAV_ReadOnly, 0, 0, 0, 0, 0, "",
		"Used by(nb objects)",	EVAV_EVVIT_Int,		(void *) ((char *) &st_Group.ul_NbObjectsUsingMe - (char *) &st_Group), EVAV_ReadOnly, 0, 0, 0, 0, 0, "",
		"Flags"				,	EVAV_EVVIT_Hexa,	(void *) ((char *) &st_Group.ul_Flags - (char *) &st_Group), EVAV_ReadOnly, 0, 0, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_Secto,
		"Secto",
		NULL,
		"Secto 0",				EVAV_EVVIT_Int,		(void *) 0, 0, 0, 0, -1, 0, 0, "",
		"Secto 1",				EVAV_EVVIT_Int,		(void *) 1, 0, 0, 0, -1, 0, 0, "",
		"Secto 2",				EVAV_EVVIT_Int,		(void *) 2, 0, 0, 0, -1, 0, 0, "",
		"Secto 3",				EVAV_EVVIT_Int,		(void *) 3, 0, 0, 0, -1, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_Extended,
		"Extended",
		NULL,
		"AI",					EVAV_EVVIT_Pointer, (void *)((char *) &st_Extended.pst_Ai - (char*) &st_Extended ), 0, LINK_C_AI_Instance, 0, 0, 0, 0, "AI description",
		"COL",					EVAV_EVVIT_Pointer, (void *)((char *) &st_Extended.pst_Col - (char*) &st_Extended ), 0, LINK_C_COL_Base, 0, 0, 0, 0, "Collision description",
		"Messages",				EVAV_EVVIT_Pointer, (void *)((char *) &st_Extended.pst_Msg - (char*) &st_Extended ), 0, LINK_C_MSG_LiFo, 0, 0, 0, 0, "List of messages",
		"Design Struct",		EVAV_EVVIT_Pointer, (void *)((char *) &st_Extended.pst_Design - (char*) &st_Extended ), 0, LINK_C_ENG_DesignStruct, 0, 0, 0, 0, "Designer infos",
		"Light",				EVAV_EVVIT_Pointer, (void *)((char *) &st_Extended.pst_Light - (char*) &st_Extended ), 0, LINK_C_GDK_StructId,0, 0, 0, 0, "Light description",
		"Group",				EVAV_EVVIT_Pointer, (void *)((char *) &st_Extended.pst_Group - (char*) &st_Extended ), 0, LINK_C_ENG_GO_Group,0, 0, 0, 0, "Group description",
		"Modifiers",			EVAV_EVVIT_SubStruct, (void *) ((char *) &st_Extended.pst_Modifiers - (char *) &st_Extended), 0, LINK_C_MDF_Modifier, 0, 0, 0, 0, "List of modifiers",
		"Extra Flags",			EVAV_EVVIT_SubStruct, (void *) ((char *) &st_Extended.uw_ExtraFlags - (char *) &st_Extended), 0, LINK_C_ENG_GO_ExtraFlags, 2, -1, 0, 0, "Extra Flags",
		"Capacities (Init)",	EVAV_EVVIT_SubStruct, (void *)((char *) &st_Extended.uw_CapacitiesInit - (char*) &st_Extended ), 0, LINK_C_Capacities, 0, 0, 0, EVAV_Filter_Init, "Designer field (init value)",
		"Capacities",			EVAV_EVVIT_SubStruct, (void *)((char *) &st_Extended.uw_Capacities - (char*) &st_Extended ), 0, LINK_C_Capacities, 0, 0, 0, EVAV_Filter_Cur, "Designer filed (current value)",
		"Sound Bank",			EVAV_EVVIT_Pointer, (void *)((char *) &st_Extended.pst_Sound - (char*) &st_Extended ), 0, LINK_C_SND_Bank, 0, 0, 0, 0, "Sound description",
		"Secto",				EVAV_EVVIT_SubStruct, (void *)((char *) &st_Extended.auc_Sectos[0] - (char*) &st_Extended ), 0, LINK_C_Secto, 0, 0, 0, 0, "Secto",
		"AI Priority",			EVAV_EVVIT_Int,		(void *) ((char *) &st_Extended.uc_AiPrio - (char *) &st_Extended ), 0, 0, 0, -1, 0, 0, "AI priority",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_Visu,
		"Visual",
		LINK_CallBack_Refresh3DEngine,
		"Draw Mask",	EVAV_EVVIT_SubStruct, (void *) ((char *) &st_Visu.ul_DrawMask - (char *) &st_Visu), 0, LINK_C_FlagsDrawMask, 0, 0, 0, 0, "Object draw mask",
		"Object",		EVAV_EVVIT_Pointer, (void *) ((char *) &st_Visu.pst_Object - (char *) &st_Visu), 0, LINK_C_GDK_StructId, 0, 0, 0, 0, "Graphic object",
		"Material",		EVAV_EVVIT_Pointer, (void *) ((char *) &st_Visu.pst_Material - (char *) &st_Visu), 0, LINK_C_GDK_StructId, 0, 0, 0, 0, "Material",
        "Has RLI",		EVAV_EVVIT_Bool, (void *) ((char *) &st_Visu.dul_VertexColors - (char *) &st_Visu), EVAV_ReadOnly, LINK_C_GDK_StructId, 0, 0, 0, 0, "Object has RLI ?",
        "ZList Display Order",EVAV_EVVIT_Int, (void *) ((char *) &st_Visu.c_DisplayOrder - (char *) &st_Visu), 0, -3, 4, 1,0, 0, "ZListe order:\nLower is the first to be draw",
		"Light Attenuation",EVAV_EVVIT_Int, (void *) ((char *) &st_Visu.c_LightAttenuation - (char *) &st_Visu), 0, 0, 127, 1,0, 0, "light attenuation",
#ifdef JADEFUSION
		"Xenon Mesh Processing", EVAV_EVVIT_SubStruct, (void*) ((char*)&st_Visu.ul_XenonMeshProcessingFlags - (char*)&st_Visu), 0, LINK_C_XenonMeshProcessingFlags, 0, 0, 0, 0, "Xenon automatic mesh processing flags",
#endif
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_StructBV,
		"Bounding Volume",
		LINK_CallBack_Refresh3DEngine,
		"Type of BV",		EVAV_EVVIT_BV,	0, 0, 0, 0, 0, 0, 0, "",
		"Parameters",		EVAV_EVVIT_Pointer, 0, 0, LINK_C_ENG_BoundingVolume, 0, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_BoundingVolume,
		"Engine Bounding Volume",
		LINK_CallBack_Refresh3DEngine,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_ODE,
		"ODE",
		NULL,
		-1
	);

#ifdef ODE_INSIDE
	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ODEFlags,
		"ODE Flags",
		LINK_CallBack_ChangeODEFlags,
		"Active ODE",									EVAV_EVVIT_Flags, (void *) 0, 0, 2, 1, 0, 0, 0, "",
		"Geometric ColMap",								EVAV_EVVIT_Flags, (void *) 0, 0, 0, 1, 0, 0, 0, "",
		"Rigid Body (Moveable)",						EVAV_EVVIT_Flags, (void *) 0, 0, 1, 1, 0, 0, 0, "",
		"Auto Pause if motionless",						EVAV_EVVIT_Flags, (void *) 0, 0, 3, 1, 0, 0, 0, "",
		"Force Init Pause(RigidBody ONLY)",				EVAV_EVVIT_Flags, (void *) 0, 0, 4, 1, 0, 0, 0, "",
		"Force Immovable",								EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 5, 1, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_SurfaceFlags,
		"Surface Flags",
		NULL,
		"dContactMu2",									EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "",
		"dContactFDir1",								EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "",
		"dContactBounce",								EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "",
		"dContactSoftERP",								EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "",
		"dContactSoftCFM",								EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "",
		"dContactMotion1",								EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "",
		"dContactMotion2",								EVAV_EVVIT_Flags, (void *) 0, 0, 6, 4, 0, 0, 0, "",
		"dContactSlip1",								EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "",
		"dContactSlip2",								EVAV_EVVIT_Flags, (void *) 0, 0, 8, 4, 0, 0, 0, "",
		"dContactApprox1_1",							EVAV_EVVIT_Flags, (void *) 0, 0, 9, 4, 0, 0, 0, "",
		"dContactApprox1_2",							EVAV_EVVIT_Flags, (void *) 0, 0, 10, 4, 0, 0, 0, "",
		-1
	);

#endif


#ifdef ODE_INSIDE
	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_InternalBodyFlags,
		"ODE InternalFlags",
		LINK_CallBack_ChangeODEFlags,
		"Use finite Rotation",						EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 0, 1, 0, 0, 0, "",
		"Use finite Rotation along axis",			EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 1, 1, 0, 0, 0, "",
		"Paused",									EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 2, 1, 0, 0, 0, "",
		"No Gravity on Body",						EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 3, 1, 0, 0, 0, "",
//		"Enable Auto Disable",						EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 4, 1, 0, 0, 0, "",
		-1
	);
#endif


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_DYN_Flags,
		"Dynamics Flags",
		LINK_CallBack_ChangeDynamicsFlags,
		"No Forces",					EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 0, 4, 0, 0, 0, "",
		"One Constant Force",			EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "",
		"Basic Forces",					EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "",
		"Complex Forces",				EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 3, 4, 0, 0, 0, "",
		"Use precision",				EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 4, 4, 0, 0, 0, "",
		"Auto precision ",				EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 5, 4, 0, 0, 0, "",
		"Low precision",				EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 6, 4, 0, 0, 0, "",
		"Medium precision ",			EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 7, 4, 0, 0, 0, "",
		"High precision",				EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 8, 4, 0, 0, 0, "",
		"Auto Orient",					EVAV_EVVIT_Flags, (void *) 0, 0, 9, 4, 0, 0, 0, "",
		"Auto Orient Inertia 1",		EVAV_EVVIT_Flags, (void *) 0, 0, 10, 4, 0, 0, 0, "",
		"Auto Orient Inertia 2",		EVAV_EVVIT_Flags, (void *) 0, 0, 11, 4, 0, 0, 0, "",
		DYN_Csz_VectorFrictionString,	EVAV_EVVIT_Flags, (void *) 0, 0, 12, 4, 0, 0, 0, "",
		"Auto Orient Horizontal",		EVAV_EVVIT_Flags, (void *) 0, 0, 13, 4, 0, 0, 0, "",
		"Solid",						EVAV_EVVIT_Flags, (void *) 0, 0, 16, 4, 0, 0, 0, "",
		"Col",							EVAV_EVVIT_Flags, (void *) 0, 0, 17, 4, 0, 0, 0, "",
		"Ignore Gravity",				EVAV_EVVIT_Flags, (void *) 0, 0, 18, 4, 0, 0, 0, "",
		"Ignore Traction",				EVAV_EVVIT_Flags, (void *) 0, 0, 19, 4, 0, 0, 0, "",
		"Ignore Stream",				EVAV_EVVIT_Flags, (void *) 0, 0, 20, 4, 0, 0, 0, "",
		"Constrained",					EVAV_EVVIT_Flags, (void *) 0, 0, 21, 4, 0, 0, 0, "",
		-1
	);


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_Dyna,
		"Dynamic",
		LINK_CallBack_ChangeDynamicValues,
		"Dynamic Flags",			EVAV_EVVIT_SubStruct, (void*)0, 0, LINK_C_DYN_Flags, 0, 0, 0, 0, "",
		DYN_Csz_MassString,			EVAV_EVVIT_Float, (void *) ((char *) &st_Dyna.f_Mass - (char *) &st_Dyna ), 0, Cf_Epsilon, Cf_Infinit, 0, 0, 0, "",
		DYN_Csz_SpeedLimitHorizString,		EVAV_EVVIT_Float, (void *) ((char *) &st_Dyna.f_SpeedLimitHoriz - (char *) &st_Dyna ), 0, Cf_Epsilon, Cf_Infinit, 0, 0, 0, "",
		DYN_Csz_SpeedLimitVertString,	EVAV_EVVIT_Float, (void *) ((char *) &st_Dyna.f_SpeedLimitVert - (char *) &st_Dyna ), 0, Cf_Epsilon, Cf_Infinit, 0, 0, 0, "",
		DYN_Csz_SpeedVectorString,	EVAV_EVVIT_Vector, (void *) ((char *) &st_Dyna.st_SpeedVector - (char *) &st_Dyna ), 0, 0, 0, 0, 0, 0, "",
		DYN_Csz_SumOfForces,		EVAV_EVVIT_Vector, (void *) ((char *) &st_Dyna.st_F - (char *) &st_Dyna ), 0, 0, 0, 0, 0, 0, "",
		DYN_Csz_BasicForces,		EVAV_EVVIT_Pointer, (void *) ((char *) &st_Dyna.pst_Forces - (char *) &st_Dyna), 0, LINK_C_DYN_BasicForces, 0, 0, 0, 0, "",
		DYN_Csz_SolidString,		EVAV_EVVIT_Pointer, (void *) ((char *) &st_Dyna.pst_Solid - (char *) &st_Dyna), 0, LINK_C_DYN_Solid, 0, 0, 0, 0, "",
		DYN_Csz_ColString,			EVAV_EVVIT_Pointer, (void *) ((char *) &st_Dyna.pst_Col - (char *) &st_Dyna), 0, LINK_C_DYN_Col, 0, 0, 0, 0, "",
		DYN_Csz_ConstraintString,	EVAV_EVVIT_Pointer, (void *) ((char *) &st_Dyna.pst_Constraint - (char *) &st_Dyna), 0, LINK_C_DYN_Constraint, 0, 0, 0, 0, "",
		"Max Step Size",			EVAV_EVVIT_Float, (void *) ((char *) &st_Dyna.f_MaxStepSize - (char *) &st_Dyna), 0, 0.0f, 0.0f, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_DYN_BasicForces,
		DYN_Csz_BasicForces,
		NULL,
		DYN_Csz_GravityString,			EVAV_EVVIT_Vector, (void *) ((char *) &st_DynForces.st_Gravity - (char *) &st_DynForces ), 0, 0, 0, 0, 0, 0, "",
		DYN_Csz_TractionString,			EVAV_EVVIT_Vector, (void *) ((char *) &st_DynForces.st_Traction - (char *) &st_DynForces ), 0, 0, 0, 0, 0, 0, "",
		DYN_Csz_StreamString,			EVAV_EVVIT_Vector, (void *) ((char *) &st_DynForces.st_Stream - (char *) &st_DynForces ), 0, 0, 0, 0, 0, 0, "",
		DYN_Csz_VectorFrictionString,	EVAV_EVVIT_Vector, (void *) ((char *) &st_DynForces.st_K - (char *) &st_DynForces ), 0, 0, 0, 0, 0, 0, "",
		DYN_Csz_FrictionString,			EVAV_EVVIT_Float, (void *) ((char *) &st_DynForces.st_K - (char *) &st_DynForces ), 0, Cf_Epsilon, Cf_Infinit, 0, 0, 0, "",
		-1
	);


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_DYN_Solid,
		DYN_Csz_SolidString,
		NULL,
		"Angular Speed",		EVAV_EVVIT_Vector, (void *) ((char *) &st_Solid.st_w - (char *) &st_Solid ), 0, 0, 0, 0, 0, 0, "",
		"Torque",				EVAV_EVVIT_Vector, (void *) ((char *) &st_Solid.st_T - (char *) &st_Solid ), 0, 0, 0, 0, 0, 0, "",
		"Angular Speed Limit",	EVAV_EVVIT_Float, (void *) ((char *) &st_Solid.f_wLimit - (char *) &st_Solid ), 0, Cf_Epsilon, Cf_Infinit, 0, 0, 0, "",
		"Elastic Factor",		EVAV_EVVIT_Float, (void *) ((char *) &st_Solid.f_Factor- (char *) &st_Solid ), 0, Cf_Epsilon, Cf_Infinit, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_DYN_Col,
		DYN_Csz_ColString,
		NULL,
		DYN_Csz_SlideString,				EVAV_EVVIT_Float, (void *) ((char *) &st_Col.f_Slide - (char *) &st_Col), 0, Cf_Epsilon, Cf_Infinit, 0, 0, 0, "",
		DYN_Csz_ReboundString,				EVAV_EVVIT_Float, (void *) ((char *) &st_Col.f_Rebound - (char *) &st_Col), 0, Cf_Epsilon, Cf_Infinit, 0, 0, 0, "",
		DYN_Csz_SlideHorizCosAngleString,	EVAV_EVVIT_Float, (void *) ((char *) &st_Col.f_SlideHorizCosAngle - (char *) &st_Col), 0, Cf_Epsilon, Cf_Infinit, 0, 0, 0, "",
		DYN_Csz_ReboundHorizCosAngleString, EVAV_EVVIT_Float, (void *) ((char *) &st_Col.f_ReboundHorizCosAngle - (char *) &st_Col), 0, Cf_Epsilon, Cf_Infinit, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_DYN_Constraint,
		DYN_Csz_ConstraintString,
		NULL,
		"Point A",			EVAV_EVVIT_Vector,	(void *) ((char *) &st_Constraint.st_A - (char *) &st_Constraint ), 0, 0, 0, 0, 0, 0, "",
		"Point B",			EVAV_EVVIT_Vector,	(void *) ((char *) &st_Constraint.st_B - (char *) &st_Constraint ), 0, 0, 0, 0, 0, 0, "",
		"Point O",			EVAV_EVVIT_Vector,	(void *) ((char *) &st_Constraint.st_O - (char *) &st_Constraint ), 0, 0, 0, 0, 0, 0, "",
		"Radius",			EVAV_EVVIT_Float,	(void *) ((char *) &st_Constraint.f_r - (char *) &st_Constraint), 0, Cf_Epsilon, Cf_Infinit, 0, 0, 0, "",
		"Type",				EVAV_EVVIT_Int,		(void *) ((char *) &st_Constraint.c_ConstraintType - (char *) &st_Constraint ), 0, 0 , 0, -1, 0, 0, "",
		-1
	);


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_CustomBits,
		"Custom Bits",
		NULL,
		"Custom All",		EVAV_EVVIT_Int,			(void *) 1, 0, 0, 0, -1, 0, 0, "Sum of all bits",
		"Custom 0",			EVAV_EVVIT_Flags, (void *) 0, 0, 8, 4, 0, 0, 0, "Designer bit",
		"Custom 1",			EVAV_EVVIT_Flags, (void *) 0, 0, 9, 4, 0, 0, 0, "Designer bit",
		"Custom 2",			EVAV_EVVIT_Flags, (void *) 0, 0, 10, 4, 0, 0, 0, "Designer bit",
		"Custom 3",			EVAV_EVVIT_Flags, (void *) 0, 0, 11, 4, 0, 0, 0, "Designer bit",
		"Custom 4",			EVAV_EVVIT_Flags, (void *) 0, 0, 12, 4, 0, 0, 0, "Designer bit",
		"Custom 5",			EVAV_EVVIT_Flags, (void *) 0, 0, 13, 4, 0, 0, 0, "Designer bit",
		"Custom 6",			EVAV_EVVIT_Flags, (void *) 0, 0, 14, 4, 0, 0, 0, "Designer bit",
		"Custom 7",			EVAV_EVVIT_Flags, (void *) 0, 0, 15, 4, 0, 0, 0, "Designer bit",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_Capacities,
		"Capacities",
		NULL,
		"Capa All",			EVAV_EVVIT_Int,	  (void *) 0, 0, 0, 0, -4, 0, 0, "Sum of all bits",
		"Capa 0",			EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "Designer bit",
		"Capa 1",			EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "Designer bit",
		"Capa 2",			EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "Designer bit",
		"Capa 3",			EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "Designer bit",
		"Capa 4",			EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "Designer bit",
		"Capa 5",			EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "Designer bit",
		"Capa 6",			EVAV_EVVIT_Flags, (void *) 0, 0, 6, 4, 0, 0, 0, "Designer bit",
		"Capa 7",			EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "Designer bit",
		"Capa 8",			EVAV_EVVIT_Flags, (void *) 0, 0, 8, 4, 0, 0, 0, "Designer bit",
		"Capa 9",			EVAV_EVVIT_Flags, (void *) 0, 0, 9, 4, 0, 0, 0, "Designer bit",
		"Capa 10",			EVAV_EVVIT_Flags, (void *) 0, 0, 10, 4, 0, 0, 0, "Designer bit",
		"Capa 11",			EVAV_EVVIT_Flags, (void *) 0, 0, 11, 4, 0, 0, 0, "Designer bit",
		"Capa 12",			EVAV_EVVIT_Flags, (void *) 0, 0, 12, 4, 0, 0, 0, "Designer bit",
		"Capa 13",			EVAV_EVVIT_Flags, (void *) 0, 0, 13, 4, 0, 0, 0, "Designer bit",
		"Capa 14",			EVAV_EVVIT_Flags, (void *) 0, 0, 14, 4, 0, 0, 0, "Designer bit",
		"Capa 15",			EVAV_EVVIT_Flags, (void *) 0, 0, 15, 4, 0, 0, 0, "Designer bit",
		"Capa 16",			EVAV_EVVIT_Flags, (void *) 0, 0, 16, 4, 0, 0, 0, "Designer bit",
		"Capa 17",			EVAV_EVVIT_Flags, (void *) 0, 0, 17, 4, 0, 0, 0, "Designer bit",
		"Capa 18",			EVAV_EVVIT_Flags, (void *) 0, 0, 18, 4, 0, 0, 0, "Designer bit",
		"Capa 19",			EVAV_EVVIT_Flags, (void *) 0, 0, 19, 4, 0, 0, 0, "Designer bit",
		"Capa 20",			EVAV_EVVIT_Flags, (void *) 0, 0, 20, 4, 0, 0, 0, "Designer bit",
		"Capa 21",			EVAV_EVVIT_Flags, (void *) 0, 0, 21, 4, 0, 0, 0, "Designer bit",
		"Capa 22",			EVAV_EVVIT_Flags, (void *) 0, 0, 22, 4, 0, 0, 0, "Designer bit",
		"Capa 23",			EVAV_EVVIT_Flags, (void *) 0, 0, 23, 4, 0, 0, 0, "Designer bit",
		"Capa 24",			EVAV_EVVIT_Flags, (void *) 0, 0, 24, 4, 0, 0, 0, "Designer bit",
		"Capa 25",			EVAV_EVVIT_Flags, (void *) 0, 0, 25, 4, 0, 0, 0, "Designer bit",
		"Capa 26",			EVAV_EVVIT_Flags, (void *) 0, 0, 26, 4, 0, 0, 0, "Designer bit",
		"Capa 27",			EVAV_EVVIT_Flags, (void *) 0, 0, 27, 4, 0, 0, 0, "Designer bit",
		"Capa 28",			EVAV_EVVIT_Flags, (void *) 0, 0, 28, 4, 0, 0, 0, "Designer bit",
		"Capa 29",			EVAV_EVVIT_Flags, (void *) 0, 0, 29, 4, 0, 0, 0, "Designer bit",
		"Capa 30",			EVAV_EVVIT_Flags, (void *) 0, 0, 30, 4, 0, 0, 0, "Designer bit",
		"Capa 31",			EVAV_EVVIT_Flags, (void *) 0, 0, 31, 4, 0, 0, 0, "Designer bit",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_NetLink,
		"Net Link",
		NULL,
		"Capacities Init",	EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_Link.uw_CapacitiesInit - (char *) &st_Link), 0, LINK_C_Capacities, 0, 0, 0, EVAV_Filter_Init, "Init value of link capacities",
		"Capacities",		EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_Link.uw_Capacities - (char *) &st_Link), 0, LINK_C_Capacities, 0, 0, 0, EVAV_Filter_Cur, "Current value of link capacities",
		"Design Init",		EVAV_EVVIT_Int,			(void *) ((char *) &st_Link.c_DesignInit - (char *) &st_Link), 0, 0, 0, 1, 0, EVAV_Filter_Init, "Init value of link user param",
		"Design",			EVAV_EVVIT_Int,			(void *) ((char *) &st_Link.c_Design - (char *) &st_Link), 0, 0, 0, 1, 0, EVAV_Filter_Cur, "Current value of link user param",
		"Pound",			EVAV_EVVIT_Float,		(void *) ((char *) &st_Link.f_Pound - (char *) &st_Link), EVAV_ReadOnly, 0.0f, 0.0f, 0, 0, 0, "Pound of link (usually distance)",
		-1
	);

	/*$F
	---------------------------------------------------------------------------------------------------

										COLLISION

	---------------------------------------------------------------------------------------------------
	*/

	LINK_RegisterStructType
	(
		LINK_C_ENG_Zone,
		"Engine Zone",
		LINK_CallBack_Refresh3DEngine,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/

	LINK_RegisterStructType
	(
		LINK_C_GameMaterialFlags,
		"Game Material",
		NULL,
		"Bit 0",					EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "",
		"Bit 1",					EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "",
		"Bit 2",					EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "",
		"Bit 3",					EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "",
		"Bit 4",					EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "",
		"Bit 5",					EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "",
		"Bit 6",					EVAV_EVVIT_Flags, (void *) 0, 0, 6, 4, 0, 0, 0, "",
		"Bit 7",					EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "",
		"Bit 8",					EVAV_EVVIT_Flags, (void *) 0, 0, 8, 4, 0, 0, 0, "",
		"Bit 9",					EVAV_EVVIT_Flags, (void *) 0, 0, 9, 4, 0, 0, 0, "",
		"Bit 10",					EVAV_EVVIT_Flags, (void *) 0, 0,10, 4, 0, 0, 0, "",
		"Bit 11",					EVAV_EVVIT_Flags, (void *) 0, 0,11, 4, 0, 0, 0, "",
		"Bit 12",					EVAV_EVVIT_Flags, (void *) 0, 0,12, 4, 0, 0, 0, "",
		"Bit 13",					EVAV_EVVIT_Flags, (void *) 0, 0,13, 4, 0, 0, 0, "",
		"Bit 14",					EVAV_EVVIT_Flags, (void *) 0, 0,14, 4, 0, 0, 0, "",
		"Bit 15",					EVAV_EVVIT_Flags, (void *) 0, 0,15, 4, 0, 0, 0, "",
		"Bit 16",					EVAV_EVVIT_Flags, (void *) 0, 0,16, 4, 0, 0, 0, "",
		"Bit 17",					EVAV_EVVIT_Flags, (void *) 0, 0,17, 4, 0, 0, 0, "",
		"Bit 18",					EVAV_EVVIT_Flags, (void *) 0, 0,18, 4, 0, 0, 0, "",
		"Bit 19",					EVAV_EVVIT_Flags, (void *) 0, 0,19, 4, 0, 0, 0, "",
		"Bit 20",					EVAV_EVVIT_Flags, (void *) 0, 0,20, 4, 0, 0, 0, "",
		"Bit 21",					EVAV_EVVIT_Flags, (void *) 0, 0,21, 4, 0, 0, 0, "",
		"Bit 22",					EVAV_EVVIT_Flags, (void *) 0, 0,22, 4, 0, 0, 0, "",
		"Bit 23",					EVAV_EVVIT_Flags, (void *) 0, 0,23, 4, 0, 0, 0, "",
		"Bit 24",					EVAV_EVVIT_Flags, (void *) 0, 0,24, 4, 0, 0, 0, "",
		"Bit 25",					EVAV_EVVIT_Flags, (void *) 0, 0,25, 4, 0, 0, 0, "",
		"Bit 26",					EVAV_EVVIT_Flags, (void *) 0, 0,26, 4, 0, 0, 0, "",
		"Bit 27",					EVAV_EVVIT_Flags, (void *) 0, 0,27, 4, 0, 0, 0, "",
		"Bit 28",					EVAV_EVVIT_Flags, (void *) 0, 0,28, 4, 0, 0, 0, "",
		"Bit 29",					EVAV_EVVIT_Flags, (void *) 0, 0,29, 4, 0, 0, 0, "",
		"Bit 30",					EVAV_EVVIT_Flags, (void *) 0, 0,30, 4, 0, 0, 0, "",
		"Bit 31",					EVAV_EVVIT_Flags, (void *) 0, 0,31, 4, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/

	LINK_RegisterStructType
	(
		LINK_C_CobFlags,
		"Cob Flags",
		LINK_CallBack_ChangeCobFlags,
		"Game Material",					EVAV_EVVIT_Flags, (void *) 0, 0, 0, 1, 0, 0, 0, "",
		"Camera Optimisation",				EVAV_EVVIT_Flags, (void *) 0, 0, 1, 1, 0, 0, 0, "",
		"Moving Cob",						EVAV_EVVIT_Flags, (void *) 0, 0, 2, 1, 0, 0, 0, "",
		"Wall Algo",						EVAV_EVVIT_Flags, (void *) 0, 0, 4, 1, 0, 0, 0, "",
		"Disable Col Optim",				EVAV_EVVIT_Flags, (void *) 0, 0, 5, 1, 0, 0, 0, "",
		"Real-Time Computation",			EVAV_EVVIT_Flags, (void *) 0, 0, 6, 1, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/

	LINK_RegisterStructType
	(
		LINK_C_ElementFlags,
		"Cob Flags",
		LINK_CallBack_ChangeElementFlags,
		"Game Material",					EVAV_EVVIT_Flags, (void *) 0, 0, 0, 2, 0, 0, 0, "",
		-1
	);



	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_GMStruct,
		"Game Material",
		LINK_CallBack_Refresh3DEngine,
		"Slide",			EVAV_EVVIT_Float,		(void *) ((char *) &st_GM.f_Slide - (char *) &st_GM ), 0, 0.0f, 1.0f, 0, 0, 0, "",
		"Rebound",			EVAV_EVVIT_Float,		(void *) ((char *) &st_GM.f_Rebound - (char *) &st_GM ), 0, 0.0f, 1.0f, 0, 0, 0, "",
		"Sound",			EVAV_EVVIT_Int,			(void *) ((char *) &st_GM.uc_Sound - (char *) &st_GM ), 0, 0 , 0, -1, 0, 0, "",
		"Custom",			EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_GM.ul_CustomBits - (char *) &st_GM), 0, LINK_C_GameMaterialFlags, 0, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/

	LINK_RegisterStructType
	(
		LINK_C_ENG_Cob,
		"Engine Cob",
		LINK_CallBack_Refresh3DEngine,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/

	LINK_RegisterStructType
	(
		LINK_C_ZoneFlags,
		"Zone Flags",
		LINK_CallBack_ChangeZoneFlags,
		"ZDM",					EVAV_EVVIT_Flags, (void *) ((char *) &st_Zone.uc_Flag - (char *) &st_Zone), 0, 0, 4, 0, 0, 0, "",
		"ZDE",					EVAV_EVVIT_Flags, (void *) ((char *) &st_Zone.uc_Flag - (char *) &st_Zone), 0, 2, 4, 0, 0, 0, "",
		"Scale insensitive",	EVAV_EVVIT_Flags, (void *) ((char *) &st_Zone.uc_Flag - (char *) &st_Zone), 0, 3, 4, 0, 0, 0, "",
		"Flag X",				EVAV_EVVIT_Flags, (void *) ((char *) &st_Zone.uc_Flag - (char *) &st_Zone), 0, 4, 4, 0, 0, 0, "",
		"Camera",				EVAV_EVVIT_Flags, (void *) ((char *) &st_Zone.uc_Flag - (char *) &st_Zone), 0, 7, 4, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/

	LINK_RegisterStructType
	(
		LINK_C_Zone,
		"Zone",
		NULL,
		"Type",					EVAV_EVVIT_Zone,  0, 0, 0, 0, 0, 0, 0, "",
		"Name",					EVAV_EVVIT_String, (void *) ((char *) &st_Zone.sz_Name - (char *) &st_Zone), 0, 0, 0, 0, 0, 0, "",
		"Zone Flags",			EVAV_EVVIT_SubStruct, (void *) 0, 0, LINK_C_ZoneFlags, 0, 0, 0, 0, "",
		"Design",				EVAV_EVVIT_Int, (void *) ((char *) &st_Zone.uc_Design - (char *) &st_Zone), 0, 0, 0, 1, 0, 0, "",
		"Shape",				EVAV_EVVIT_Pointer, (void *) ((char *) &st_Zone.pst_Itself - (char *) &st_Zone), 0, LINK_C_ENG_Zone, 0, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/

	LINK_RegisterStructType
	(
		LINK_C_Cob,
		"Collision Object",
		NULL,
		"Type",					EVAV_EVVIT_Cob,		0, 0, 0, 0, 0, 0, 0, "",
		"Game Material File",	EVAV_EVVIT_String, (void *) ((char *) &st_Cob.sz_GMatName - (char *) &st_Cob), EVAV_ReadOnly, 0, 0, 0, 0, 0, "",
		"Collision Object",		EVAV_EVVIT_Pointer, (void *) ((char *) &st_Cob.pst_Itself - (char *) &st_Cob), 0, LINK_C_ENG_Cob, 0, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	
    LINK_RegisterStructType
	(
		LINK_C_SND_Bank,
		"Sound Bank",
		NULL,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	
    LINK_RegisterStructType
	(
		LINK_C_SND_Sound,
		"Sound File",
		NULL,
		-1
	);

    /*---------------------------------------------------------------------------------------------------------------*/

	LINK_RegisterStructType
	(
		LINK_C_COL_Base,
		"COL Base",
		NULL,
		"Instance",				EVAV_EVVIT_Pointer, (void *) ((char *) &st_ColBase.pst_Instance - (char *) &st_ColBase), 0, LINK_C_COL_Instance, 0, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/

	LINK_RegisterStructType
	(
		LINK_C_COL_Detection,
		"COL Detection",
		NULL,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_COL_Instance,
		"COL Instance",
		NULL,

		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_COL_Crossable,
		"Crossable",
		NULL,
		"Custom 0",			EVAV_EVVIT_Flags, (void *) 0, 0, 0, 2, 0, 0, 0, "",
		"Custom 1",			EVAV_EVVIT_Flags, (void *) 0, 0, 1, 2, 0, 0, 0, "",
		"Custom 2",			EVAV_EVVIT_Flags, (void *) 0, 0, 2, 2, 0, 0, 0, "",
		"Custom 3",			EVAV_EVVIT_Flags, (void *) 0, 0, 3, 2, 0, 0, 0, "",
		"Custom 4",			EVAV_EVVIT_Flags, (void *) 0, 0, 4, 2, 0, 0, 0, "",
		"Custom 5",			EVAV_EVVIT_Flags, (void *) 0, 0, 5, 2, 0, 0, 0, "",
		"Custom 6",			EVAV_EVVIT_Flags, (void *) 0, 0, 6, 2, 0, 0, 0, "",
		"Custom 7",			EVAV_EVVIT_Flags, (void *) 0, 0, 7, 2, 0, 0, 0, "",
		"Custom 8",			EVAV_EVVIT_Flags, (void *) 0, 0, 8, 2, 0, 0, 0, "",
		"Custom 9",			EVAV_EVVIT_Flags, (void *) 0, 0, 9, 2, 0, 0, 0, "",
		"Custom 10",		EVAV_EVVIT_Flags, (void *) 0, 0, 10, 2, 0, 0, 0, "",
		"Custom 11",		EVAV_EVVIT_Flags, (void *) 0, 0, 11, 2, 0, 0, 0, "",
		"Custom 12",		EVAV_EVVIT_Flags, (void *) 0, 0, 12, 2, 0, 0, 0, "",
		"Custom 13",		EVAV_EVVIT_Flags, (void *) 0, 0, 13, 2, 0, 0, 0, "",
		"Custom 14",		EVAV_EVVIT_Flags, (void *) 0, 0, 14, 2, 0, 0, 0, "",
		"Custom 15",		EVAV_EVVIT_Flags, (void *) 0, 0, 15, 2, 0, 0, 0, "",
		-1
	);


	/*$F
	---------------------------------------------------------------------------------------------------

											GRID

	---------------------------------------------------------------------------------------------------
	*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_Grid,
		"Grid",
		LINK_CallBack_Refresh3DEngine,
		"Coord X Grid",		EVAV_EVVIT_Float,		(void *) ((char *) &st_Grid.f_MinXTotal - (char *) &st_Grid), EVAV_ReadOnly, 0.0f, 0.0f, 0, 0, 0, "",
		"Coord Y Grid",		EVAV_EVVIT_Float,		(void *) ((char *) &st_Grid.f_MinYTotal - (char *) &st_Grid), EVAV_ReadOnly, 0.0f, 0.0f, 0, 0, 0, "",
		"Coord X Real",		EVAV_EVVIT_Float,		(void *) ((char *) &st_Grid.f_MinXReal - (char *) &st_Grid), EVAV_ReadOnly, 0.0f, 0.0f, 0, 0, 0, "",
		"Coord Y Real",		EVAV_EVVIT_Float,		(void *) ((char *) &st_Grid.f_MinYReal - (char *) &st_Grid), EVAV_ReadOnly, 0.0f, 0.0f, 0, 0, 0, "",
		"Num Groups X",		EVAV_EVVIT_Int,			(void *) ((char *) &st_Grid.w_NumGroupsX - (char *) &st_Grid), EVAV_ReadOnly, 0, 0, 2, 0, 0, "",
		"Num Groups Y",		EVAV_EVVIT_Int,			(void *) ((char *) &st_Grid.w_NumGroupsY - (char *) &st_Grid), EVAV_ReadOnly, 0, 0, 2, 0, 0, "",
		"Num Real X",		EVAV_EVVIT_Int,			(void *) ((char *) &st_Grid.w_NumRealGroupsX - (char *) &st_Grid), EVAV_ReadOnly, 0, 0, 2, 0, 0, "",
		"Num Real Y",		EVAV_EVVIT_Int,			(void *) ((char *) &st_Grid.w_NumRealGroupsY - (char *) &st_Grid), EVAV_ReadOnly, 0, 0, 2, 0, 0, "",
		"X Real Group",		EVAV_EVVIT_Int,			(void *) ((char *) &st_Grid.w_XRealGroup - (char *) &st_Grid), EVAV_ReadOnly, 0, 0, 2, 0, 0, "",
		"Y Real Group",		EVAV_EVVIT_Int,			(void *) ((char *) &st_Grid.w_YRealGroup - (char *) &st_Grid), EVAV_ReadOnly, 0, 0, 2, 0, 0, "",
		"Size Group",		EVAV_EVVIT_Int,			(void *) ((char *) &st_Grid.c_SizeGroup - (char *) &st_Grid), EVAV_ReadOnly, 0, 0, 1, 0, 0, "",
		"Dummy Object",		EVAV_EVVIT_Pointer,		(void *) ((char *) &st_Grid.p_GameObject - (char *) &st_Grid), 0, LINK_C_ENG_GameObjectOriented, 0, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GridDisplayOptions,
		"Grid display options",
		LINK_CallBack_Refresh3DEngineGrid,
		"Flags",			EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_GridDO.ul_Flags - (char *) &st_GridDO), 0, LINK_C_ENG_GridDisplayOptionsFlags, 0, 0, 0, 0, "",
		"Coord Z Grid",		EVAV_EVVIT_Float,		(void *) ((char *) &st_GridDO.f_Z- (char *) &st_GridDO), 0, 0.0f, 0.0f, 0, 0, 0, "Z position of the grid (editor info, CTRL+W to save)",
		"Lock Z on",		EVAV_EVVIT_Key,			(void *) ((char *) &st_GridDO.ul_KeyForLockedZ - (char *) &st_GridDO), 0, 0, 0, 0, 0, 0, "To lock Z position of the grid to an object",
		-1
	);


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GridDisplayOptionsFlags,
		"Grid display options",
		LINK_CallBack_Refresh3DEngine,
		"Show",				EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "",
		"Show only real",	EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "",
		"Show content",		EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "",
		"Lock Z on Gao",	EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_SpriteGen,
		"Sprite generator options",
        NULL,
        -1
    );

    /*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_SpriteGenFlags,
		"Sprite gen flags",
        LINK_CallBack_Refresh3DEngine, 
        "Disapear with Transparency",	EVAV_EVVIT_Flags, (void *) 0, 0, 0, 2, 0, 0, 0, "",
		"Disapear with size"		,	EVAV_EVVIT_Flags, (void *) 0, 0, 1, 2, 0, 0, 0, "",
		"Disapear with Density"		,	EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 2, 2, 0, 0, 0, "",
		"Vertex Alpha Is Size"		,	EVAV_EVVIT_Flags, (void *) 0, 0, 3, 2, 0, 0, 0, "",
		"Vertex Alpha Is Transparency", EVAV_EVVIT_Flags, (void *) 0, 0, 4, 2, 0, 0, 0, "",
		"Vertex Alpha Is Density"	,	EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 5, 2, 0, 0, 0, "",
		"Size factor is real size " ,	EVAV_EVVIT_Flags, (void *) 0, 0, 6, 2, 0, 0, 0, "",
		"Sort sprites"				,	EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 7, 2, 0, 0, 0, "",
		"Table Mapped"				,	EVAV_EVVIT_Flags, (void *) 0, 0, 8, 2, 0, 0, 0, "",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_GO_DesDesFlags,
		"Capacities",
		NULL,
		"Capa All",			EVAV_EVVIT_Int,	  (void *) 0, 0, 0, 0, -4, 0, 0, "Sum of all bits",
		"Capa 0",			EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "Designer bit",
		"Capa 1",			EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "Designer bit",
		"Capa 2",			EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "Designer bit",
		"Capa 3",			EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "Designer bit",
		"Capa 4",			EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "Designer bit",
		"Capa 5",			EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "Designer bit",
		"Capa 6",			EVAV_EVVIT_Flags, (void *) 0, 0, 6, 4, 0, 0, 0, "Designer bit",
		"Capa 7",			EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "Designer bit",
		"Capa 8",			EVAV_EVVIT_Flags, (void *) 0, 0, 8, 4, 0, 0, 0, "Designer bit",
		"Capa 9",			EVAV_EVVIT_Flags, (void *) 0, 0, 9, 4, 0, 0, 0, "Designer bit",
		"Capa 10",			EVAV_EVVIT_Flags, (void *) 0, 0, 10, 4, 0, 0, 0, "Designer bit",
		"Capa 11",			EVAV_EVVIT_Flags, (void *) 0, 0, 11, 4, 0, 0, 0, "Designer bit",
		"Capa 12",			EVAV_EVVIT_Flags, (void *) 0, 0, 12, 4, 0, 0, 0, "Designer bit",
		"Capa 13",			EVAV_EVVIT_Flags, (void *) 0, 0, 13, 4, 0, 0, 0, "Designer bit",
		"Capa 14",			EVAV_EVVIT_Flags, (void *) 0, 0, 14, 4, 0, 0, 0, "Designer bit",
		"Capa 15",			EVAV_EVVIT_Flags, (void *) 0, 0, 15, 4, 0, 0, 0, "Designer bit",
		"Capa 16",			EVAV_EVVIT_Flags, (void *) 0, 0, 16, 4, 0, 0, 0, "Designer bit",
		"Capa 17",			EVAV_EVVIT_Flags, (void *) 0, 0, 17, 4, 0, 0, 0, "Designer bit",
		"Capa 18",			EVAV_EVVIT_Flags, (void *) 0, 0, 18, 4, 0, 0, 0, "Designer bit",
		"Capa 19",			EVAV_EVVIT_Flags, (void *) 0, 0, 19, 4, 0, 0, 0, "Designer bit",
		"Capa 20",			EVAV_EVVIT_Flags, (void *) 0, 0, 20, 4, 0, 0, 0, "Designer bit",
		"Capa 21",			EVAV_EVVIT_Flags, (void *) 0, 0, 21, 4, 0, 0, 0, "Designer bit",
		"Capa 22",			EVAV_EVVIT_Flags, (void *) 0, 0, 22, 4, 0, 0, 0, "Designer bit",
		"Capa 23",			EVAV_EVVIT_Flags, (void *) 0, 0, 23, 4, 0, 0, 0, "Designer bit",
		"Capa 24",			EVAV_EVVIT_Flags, (void *) 0, 0, 24, 4, 0, 0, 0, "Designer bit",
		"Capa 25",			EVAV_EVVIT_Flags, (void *) 0, 0, 25, 4, 0, 0, 0, "Designer bit",
		"Capa 26",			EVAV_EVVIT_Flags, (void *) 0, 0, 26, 4, 0, 0, 0, "Designer bit",
		"Capa 27",			EVAV_EVVIT_Flags, (void *) 0, 0, 27, 4, 0, 0, 0, "Designer bit",
		"Capa 28",			EVAV_EVVIT_Flags, (void *) 0, 0, 28, 4, 0, 0, 0, "Designer bit",
		"Capa 29",			EVAV_EVVIT_Flags, (void *) 0, 0, 29, 4, 0, 0, 0, "Designer bit",
		"Capa 30",			EVAV_EVVIT_Flags, (void *) 0, 0, 30, 4, 0, 0, 0, "Designer bit",
		"Capa 31",			EVAV_EVVIT_Flags, (void *) 0, 0, 31, 4, 0, 0, 0, "Designer bit",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_DesignStruct,
		"Design Structure",
		NULL,
		"flags",				EVAV_EVVIT_SubStruct,		(void *)((char *)&st_DesignStruct.flags - (char *)&st_DesignStruct), 0, LINK_C_ENG_GO_DesDesFlags, 0, 0, 0, 0, "",
		"int 1",				EVAV_EVVIT_Int,		(void *)((char *)&st_DesignStruct.i1 - (char *)&st_DesignStruct), 0, 0, 0, 0, 0, 0, "",
		"int 1 (c1)",			EVAV_EVVIT_Int,		(void *)(((char *)&st_DesignStruct.i1) + 0 - (char *)&st_DesignStruct), 0, 0, 0, 1, 0, 0, "",
		"int 1 (c2)",			EVAV_EVVIT_Int,		(void *)(((char *)&st_DesignStruct.i1) + 1 - (char *)&st_DesignStruct), 0, 0, 0, 1, 0, 0, "",
		"int 1 (c3)",			EVAV_EVVIT_Int,		(void *)(((char *)&st_DesignStruct.i1) + 2 - (char *)&st_DesignStruct), 0, 0, 0, 1, 0, 0, "",
		"int 1 (c4)",			EVAV_EVVIT_Int,		(void *)(((char *)&st_DesignStruct.i1) + 3 - (char *)&st_DesignStruct), 0, 0, 0, 1, 0, 0, "",
		"int 2",				EVAV_EVVIT_Int,		(void *)((char *)&st_DesignStruct.i2 - (char *)&st_DesignStruct), 0, 0, 0, 0, 0, 0, "",
		"int 2 (c1)",			EVAV_EVVIT_Int,		(void *)(((char *)&st_DesignStruct.i2) + 0 - (char *)&st_DesignStruct), 0, 0, 0, 1, 0, 0, "",
		"int 2 (c2)",			EVAV_EVVIT_Int,		(void *)(((char *)&st_DesignStruct.i2) + 1 - (char *)&st_DesignStruct), 0, 0, 0, 1, 0, 0, "",
		"int 2 (c3)",			EVAV_EVVIT_Int,		(void *)(((char *)&st_DesignStruct.i2) + 2 - (char *)&st_DesignStruct), 0, 0, 0, 1, 0, 0, "",
		"int 2 (c4)",			EVAV_EVVIT_Int,		(void *)(((char *)&st_DesignStruct.i2) + 3 - (char *)&st_DesignStruct), 0, 0, 0, 1, 0, 0, "",
		"int 3",				EVAV_EVVIT_Int,		(void *)((char *)&st_DesignStruct.i3 - (char *)&st_DesignStruct), 0, 0, 0, 0, 0, 0, "",
        "int 3 (c1)",			EVAV_EVVIT_Int,		(void *)(((char *)&st_DesignStruct.i3) + 0 - (char *)&st_DesignStruct), 0, 0, 0, 1, 0, 0, "",
		"int 3 (c2)",			EVAV_EVVIT_Int,		(void *)(((char *)&st_DesignStruct.i3) + 1 - (char *)&st_DesignStruct), 0, 0, 0, 1, 0, 0, "",
		"int 3 (c3)",			EVAV_EVVIT_Int,		(void *)(((char *)&st_DesignStruct.i3) + 2 - (char *)&st_DesignStruct), 0, 0, 0, 1, 0, 0, "",
		"int 3 (c4)",			EVAV_EVVIT_Int,		(void *)(((char *)&st_DesignStruct.i3) + 3 - (char *)&st_DesignStruct), 0, 0, 0, 1, 0, 0, "",
		"float 1",				EVAV_EVVIT_Float,	(void *)((char *)&st_DesignStruct.f1 - (char *)&st_DesignStruct), 0, 0.0f, 0.0f, 0, 0, 0, "",
		"float 2",				EVAV_EVVIT_Float,	(void *)((char *)&st_DesignStruct.f2 - (char *)&st_DesignStruct), 0, 0.0f, 0.0f, 0, 0, 0, "",
		"vector 1",				EVAV_EVVIT_Vector,	(void *)((char *)&st_DesignStruct.st_Vec1 - (char *)&st_DesignStruct), 0, 0, 0, 0, 0, 0, "",
		"vector 2",				EVAV_EVVIT_Vector,	(void *)((char *)&st_DesignStruct.st_Vec2 - (char *)&st_DesignStruct), 0, 0, 0, 0, 0, 0, "",
		"perso 1",				EVAV_EVVIT_GO,		(void *)((char *)&st_DesignStruct.pst_Perso1 - (char *)&st_DesignStruct), 0, 0, 0, 0, 0, 0, "",
		"perso 2",				EVAV_EVVIT_GO,		(void *)((char *)&st_DesignStruct.pst_Perso2 - (char *)&st_DesignStruct), 0, 0, 0, 0, 0, 0, "",
		"net 1",				EVAV_EVVIT_NetWork, (void *)((char *)&st_DesignStruct.pst_Net1 - (char *)&st_DesignStruct), 0, 0, 0, 0, 0, 0, "",
		"net 2",				EVAV_EVVIT_NetWork, (void *)((char *)&st_DesignStruct.pst_Net2 - (char *)&st_DesignStruct), 0, 0, 0, 0, 0, 0, "",
		"text 1",				EVAV_EVVIT_Text,	(void *)((char *)&st_DesignStruct.st_Text1 - (char *)&st_DesignStruct), 0, 0, 0, 0, 0, 0, "",
		"text 2",				EVAV_EVVIT_Text,	(void *)((char *)&st_DesignStruct.st_Text2 - (char *)&st_DesignStruct), 0, 0, 0, 0, 0, 0, "",
		"Des Flags Init",		EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_DesignStruct.ui_DesignFlagsInit - (char *) &st_DesignStruct), 0, LINK_C_ENG_GO_DesDesFlags, 0, 0, 0, EVAV_Filter_Init, "",	
		"Des Flags",			EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_DesignStruct.ui_DesignFlags - (char *) &st_DesignStruct), 0, LINK_C_ENG_GO_DesDesFlags, 0, 0, 0, EVAV_Filter_Cur, "",	
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_GDI_ScreenFormat,
		"Screen format",
		LINK_CallBack_Refresh3DEngine,
		"Flags",				EVAV_EVVIT_SubStruct,	(void *) ((char *) &st_DisplayData.st_ScreenFormat.ul_Flags- (char *) &st_DisplayData.st_ScreenFormat), 0, LINK_C_GDI_ScreenFormatFlags, 0, 0, 0, 0, "",
		"Pixel Y/X",			EVAV_EVVIT_Float,	(void *)((char *)&st_DisplayData.st_ScreenFormat.f_PixelYoverX - (char *)&st_DisplayData.st_ScreenFormat), 0, 0.0f, 0.0f, 0, 0, 0, "Pixel ratio (height/width)",
		"Custom screen Y/X",	EVAV_EVVIT_Float,	(void *)((char *)&st_DisplayData.st_ScreenFormat.f_ScreenYoverX - (char *)&st_DisplayData.st_ScreenFormat), 0, 0.0f, 0.0f, 0, 0, 0, "Custom screen ratio (height / width)",
		"Common screen Y/X",	EVAV_EVVIT_ConstInt, (void *)((char *)&st_DisplayData.st_ScreenFormat.l_ScreenRatioConst - (char *)&st_DisplayData.st_ScreenFormat), 0, 0, 0, 0, (int) "User\n0\n1:1\n1\n4:3\n2\n16:9\n3\n", 0, "Common screen ratio (1:1, 4:3, 16:9)\nset to user to use custom ratio",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_GDI_ScreenFormatFlags,
		"Screen format flags",
		LINK_CallBack_Refresh3DEngine,
		"Crop to window",	EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "All viewport is visible",
		"Occupy all",		EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "Occupy all window (viewport can be larger than window)",
		"Y is reference",	EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "Y axis is the reference axis\nWhat is displayed in height is always the same according to the focale)",
        "16/9 black band", 	EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "Display black band to reduce screen to 16/9",
		-1
	);
	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_GDI_OceanStruct,
		"Ocean wave",
		LINK_CallBack_RefreshOcean,
		"Activated",			EVAV_EVVIT_Bool,	(void *) ((char *) &st_OceanExp.Activate		- (char *) &st_OceanExp ), 0, 0, 0, 0, 0, 0, "Enable Disable Ocean",
        "Sky Color", 			EVAV_EVVIT_Color,	(void *) ((char *) &st_OceanExp.SkyCol			- (char *) &st_OceanExp ), 0, 0, 0, 0, 0, 0,			"IA WTR_SetL int Parram n° 0",
        "Bottom Color", 		EVAV_EVVIT_Color,	(void *) ((char *) &st_OceanExp.BottomCol		- (char *) &st_OceanExp ), 0, 0, 0, 0, 0, 0,			"IA WTR_SetL int Parram n° 1",
        "Water Color", 			EVAV_EVVIT_Color,	(void *) ((char *) &st_OceanExp.WaterCol		- (char *) &st_OceanExp ), 0, 0, 0, 0, 0, 0,			"IA WTR_SetL int Parram n° 2",
		"Frustrum Mesh Size X",	EVAV_EVVIT_Int,		(void *) ((char *) &st_OceanExp.FrustrumMesh_SX - (char *) &st_OceanExp ), 0,16, 512, 0, 0, 0,			"IA WTR_SetL int Parram n° 3",
		"Frustrum Mesh Size Y",	EVAV_EVVIT_Int,		(void *) ((char *) &st_OceanExp.FrustrumMesh_SY - (char *) &st_OceanExp ), 0,16, 512, 0, 0, 0,			"IA WTR_SetL int Parram n° 4",
        "Spectrum power of 2", 	EVAV_EVVIT_Int,		(void *) ((char *) &st_OceanExp.ulWAveMapShift	- (char *) &st_OceanExp ), 0,4, 10, 0, 0, 0,			"IA WTR_SetL int Parram n° 5",
        "Choppy Spectrum power of 2", 	EVAV_EVVIT_Int,		(void *) ((char *) &st_OceanExp.ulWAveMapShiftCHP - (char *) &st_OceanExp ), 0,4, 10, 0, 0, 0,	"IA WTR_SetL int Parram n° 6",
		"Cloud shading Activated",EVAV_EVVIT_Bool,	(void *) ((char *) &st_OceanExp.CloudShadingEnabled- (char *) &st_OceanExp ), 0, 0, 0, 0, 0, 0,			"IA WTR_SetL int Parram n° 7",
		"Texture env Ref",		EVAV_EVVIT_Int,	(void *) ((char *) &st_OceanExp.ulTextureRef - (char *) &st_OceanExp ), 0, 0, 0, 0, 0, 0,			"IA WTR_SetL int Parram n° 8",

        "Waves Height", 		EVAV_EVVIT_Float,	(void *) ((char *) &st_OceanExp.ZFactor			- (char *) &st_OceanExp ), 0,-10.0, +10.0, 0, 0, 0,		"IA WTR_SetF Float Parram n° 0",
        "Choppy", 				EVAV_EVVIT_Float,	(void *) ((char *) &st_OceanExp.fChoppyFactor	- (char *) &st_OceanExp ), 0,-10.0, +10.0, 0, 0, 0,		"IA WTR_SetF Float Parram n° 1",
        "Tiling", 				EVAV_EVVIT_Float,	(void *) ((char *) &st_OceanExp.HCoef			- (char *) &st_OceanExp ), 0,0.0001f, 4.0f, 0, 0, 0,	"IA WTR_SetF Float Parram n° 2",
        "Speed", 				EVAV_EVVIT_Float,	(void *) ((char *) &st_OceanExp.SpeedFactor		- (char *) &st_OceanExp ), 0,-1000.0, +1000.0, 0, 0, 0, "IA WTR_SetF Float Parram n° 3",
        "Water Height", 		EVAV_EVVIT_Float,	(void *) ((char *) &st_OceanExp.GlobalWaterZ	- (char *) &st_OceanExp ), 0,-1000.0, +1000.0, 0, 0, 0, "IA WTR_SetF Float Parram n° 4",
        "Wave dir", 			EVAV_EVVIT_Float,	(void *) ((char *) &st_OceanExp.WindBalance		- (char *) &st_OceanExp ), 0,0.0, +100.0, 0, 0, 0,		"IA WTR_SetF Float Parram n° 5",
        "Wave Prop", 			EVAV_EVVIT_Float,	(void *) ((char *) &st_OceanExp.P_Factor		- (char *) &st_OceanExp ), 0,0.0, +100.0, 0, 0, 0,		"IA WTR_SetF Float Parram n° 6",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_GDI_EditOptions,
		"Sub object options",
		NULL,
        "Wired Color (selected)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_WiredColorSel - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used for wired mode with selected object",
        "Wired Color (not selected)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_WiredColor - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used for wired mode with non-selected object",
		"Color of vertex (main, not picked, not pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_VertexColor[0] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a vertex that is not selected and not selectable",
		"Color of vertex (main, picked, not pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_VertexColor[1] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a vertex that is selected but not selectable",
		"Color of vertex (main, not picked, pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_VertexColor[2] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a vertex that is not selected but selectable",
		"Color of vertex (main, picked, pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_VertexColor[3] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a vertex that is selected and selectable",
        "Color of vertex (second, not picked, not pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_VertexColor[4] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a vertex that is not selected and not selectable",
		"Color of vertex (second, picked, not pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_VertexColor[5] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a vertex that is selected but not selectable",
		"Color of vertex (second, not picked, pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_VertexColor[6] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a vertex that is not selected but selectable",
		"Color of vertex (second, picked, pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_VertexColor[7] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a vertex that is selected and selectable",
        "Vertex point size", EVAV_EVVIT_Float, (void *)((char *)&st_EditOptions.f_VertexSize - (char *)&st_EditOptions), 0, 0.0f, 0.0f, 0, 0, 0, "size in pixel of representation of a vertex",
		"Color of edge (main, not picked, not pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_EdgeColor[0] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display an edge that is not selected and not selectable",
		"Color of edge (main, picked, not pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_EdgeColor[1] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display an edge that is selected but not selectable",
		"Color of edge (main, not picked, pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_EdgeColor[2] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display an edge that is not selected but selectable",
		"Color of edge (main, picked, pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_EdgeColor[3] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display an edge that is selected and selectable",
        "Color of edge (second, not picked, not pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_EdgeColor[4] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display an edge that is not selected and not selectable",
		"Color of edge (second, picked, not pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_EdgeColor[5] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display an edge that is selected but not selectable",
		"Color of edge (second, not picked, pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_EdgeColor[6] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display an edge that is not selected but selectable",
		"Color of edge (second, picked, pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_EdgeColor[7] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display an edge that is selected and selectable",
        "Edge line size", EVAV_EVVIT_Float, (void *)((char *)&st_EditOptions.f_EdgeSize - (char *)&st_EditOptions), 0, 0.0f, 0.0f, 0, 0, 0, "width in pixel of lines representating edges",
        "Color of faces (main, not picked, not pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceColor[0] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a face that is not selected and not selectable",
		"Color of faces (main, picked, not pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceColor[1] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a face that is selected but not selectable",
		"Color of faces (main, not picked, pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceColor[2] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a face that is not selected but selectable",
		"Color of faces (main, picked, pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceColor[3] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a face that is selected and selectable",
        "Color of faces (second, not picked, not pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceColor[4] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a face that is not selected and not selectable",
		"Color of faces (second, picked, not pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceColor[5] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a face that is selected but not selectable",
		"Color of faces (second, not picked, pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceColor[6] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a face that is not selected but selectable",
		"Color of faces (second, picked, pickable)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceColor[7] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a face that is selected and selectable",
        "Face point size", EVAV_EVVIT_Float, (void *)((char *)&st_EditOptions.f_FaceSize - (char *)&st_EditOptions), 0, 0.0f, 0.0f, 0, 0, 0, "size in pixel of representation of a face",
        "Color of normals (main, not picked)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_NormalColor[0] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a normal of a selected vertex",
		"Color of normals (main, picked)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_NormalColor[1] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a normal of a not selected vertex",
        "Color of normals (second, not picked)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_NormalColor[2] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a normal of a selected vertex",
		"Color of normals (second, picked)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_NormalColor[3] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a normal of a not selected vertex",
        "Normal line size", EVAV_EVVIT_Float, (void *)((char *)&st_EditOptions.f_NormalSize - (char *)&st_EditOptions), 0, 0.0f, 0.0f, 0, 0, 0, "Width in pixel of line representating normals",
		"Normals length (in meter)", EVAV_EVVIT_Float, (void *)((char *)&st_EditOptions.f_ScaleForNormals - (char *)&st_EditOptions), 0, 0.0f, 0.0f, 0, 0, 0, "length of displayed normals",
        "Morph vector (selected)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_MorphVectorColor[1] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to draw morph vector of selected vertices",
        "Morph vector (not selected)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_MorphVectorColor[0] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to draw morph vector of unselected vertices",
        "UVMapper(1) - Point", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_UVMapperColor[0][0] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a point (not sel) in UV mapper",
        "UVMapper(1) - Point sel", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_UVMapperColor[0][1] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a point (sel) in UV mapper",
        "UVMapper(1) - Line1", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_UVMapperColor[0][2] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color1 used to display a line in UV mapper",
        "UVMapper(1) - Line2", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_UVMapperColor[0][3] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display a line (doted) in UV mapper",
        "UVMapper(1) - Grid", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_UVMapperColor[0][4] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display grid in UV mapper",
        "UVMapper(1) - Helper1", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_UVMapperColor[0][5] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color1 used to display helper in UV mapper",
        "UVMapper(1) - Helper2", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_UVMapperColor[0][6] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display helper in UV mapper",
        "UVMapper(2) - Point", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_UVMapperColor[1][0] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a point (not sel) in UV mapper",
        "UVMapper(2) - Point sel", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_UVMapperColor[1][1] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display a point (sel) in UV mapper",
        "UVMapper(2) - Line1", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_UVMapperColor[1][2] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color1 used to display a line in UV mapper",
        "UVMapper(2) - Line2", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_UVMapperColor[1][3] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display a line (doted) in UV mapper",
        "UVMapper(2) - Grid", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_UVMapperColor[1][4] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color used to display grid in UV mapper",
        "UVMapper(2) - Helper1", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_UVMapperColor[1][5] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color1 used to display helper in UV mapper",
        "UVMapper(2) - Helper2", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_UVMapperColor[1][6] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display helper in UV mapper",
        "FaceIDColor(0)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[0] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(1)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[1] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(2)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[2] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(3)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[3] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(4)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[4] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(5)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[5] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(6)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[6] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(7)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[7] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(8)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[8] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(9)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[9] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(10)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[10] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(11)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[11] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(12)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[12] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(13)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[13] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(14)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[14] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(15)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[15] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(16)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[16] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(17)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[17] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(18)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[18] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(19)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[19] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(20)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[20] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(21)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[21] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(22)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[22] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(23)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[23] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(24)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[24] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(25)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[25] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(26)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[26] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(27)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[27] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(28)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[28] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(29)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[29] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(30)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[30] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
        "FaceIDColor(31)", EVAV_EVVIT_Color, (void *) ((char *) &st_EditOptions.ul_FaceIDColor[31] - (char *) &st_EditOptions ), 0, 0, 0, 0, 0, 0, "Color2 used to display face in color mode face ID",
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_TrackList,
		"Track List",
		NULL,
		-1
	);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_TEX_Procedural,
		"Procedural Texture",
		LINK_Callback_SaveProcedural,
		-1
	);

    /*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_TEX_ProceduralFlag,
		"Procedural texture flags",
        LINK_Callback_SaveProcedural,
        "Interface texture"         ,	EVAV_EVVIT_Flags, (void *) 0, 0, 0, 2, 0, 0, 0, "texture is stored in RAM and real time loaded (if supported)",
		-1
	);

    /*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_TEX_Animated,
		"Animated Texture",
		LINK_Callback_SaveAnimated,
		-1
	);



	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ENG_ParticuleGeneratorFlags,
		"Zone Flags",
		NULL,
		"Active",				EVAV_EVVIT_Flags, (void *) 0, 0, 6, 4, 0, 0, 0, "",
		"Freeze",				EVAV_EVVIT_Flags, (void *) 0, 0, 7, 4, 0, 0, 0, "",
		"Transparent",			EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "",
		"Color RGB = A",		EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "",
		"Size Y = X",			EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "",
		"Death decrease alpha", EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "",
		"Death decrease size",	EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "",
		"Death increase size",	EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "",
		"Birth increase alpha", EVAV_EVVIT_Flags, (void *) 0, 0, 8, 4, 0, 0, 0, "",
		"Birth increase size",	EVAV_EVVIT_Flags, (void *) 0, 0, 9, 4, 0, 0, 0, "",
		"Use Z min",			EVAV_EVVIT_Flags, (void *) 0, 0, 10, 4, 0, 0, 0, "",
		"Use Z max",			EVAV_EVVIT_Flags, (void *) 0, 0, 11, 4, 0, 0, 0, "",
		"Use Rotation",			EVAV_EVVIT_Flags, (void *) 0, 0, 12, 4, 0, 0, 0, "",
		"Use Friction",			EVAV_EVVIT_Flags, (void *) 0, 0, 13, 4, 0, 0, 0, "",
		"Acc Speed",			EVAV_EVVIT_Flags, (void *) 0, 0, 15, 4, 0, 0, 0, "",
		"Do not sort particles",EVAV_EVVIT_Flags, (void *) 0, 0, 16, 4, 0, 0, 0, "",
		-1
	);

    /*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_MDF_LegLinkFlag,
		"LegLink orientation Flags",
		NULL,
		"X axis",			EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "X axis is used to determine orientation of leg",
		"Y axis",			EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "Y axis is used to determine orientation of leg",
		"Z axis",			EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "Z axis is used to determine orientation of leg",
		"Neg dir",			EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "Negate orientation axis",
		"Group Process",	EVAV_EVVIT_Flags, (void *) 0, 0, 4, 4, 0, 0, 0, "Get objects from group",
		"Compute Length",	EVAV_EVVIT_Flags, (void *) 0, 0, 5, 4, 0, 0, 0, "Compute bones length from init pos",
		-1
	);

    /*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_3DView_WPFlags,
		"Waypoint flags",
		NULL,
		"Hide all when moving",		EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "WP are hidden when moving something",
		"Hide select when moving",	EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "All selected WP are hidden while moving",
		-1
	);

    /*---------------------------------------------------------------------------------------------------------------*/
    LINK_RegisterStructType
    (
		LINK_C_GEO_EditorFlags,
		"Geometry editor flags",
		NULL,
		"Geometry for 3DText",		EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "Object are not cleaned after edition loading",
		-1
	);



	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_Animation_LOD,
		"Animation LOD",
		NULL,
		"Freeze Skeleton",		EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 0, 4, 0, 0, 0, "Selected Object",
		"Pseudo 30Htz",			EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 1, 4, 0, 0, 0, "Display BV",
		"Skip Translations",	EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 2, 4, 0, 0, 0, "Display additional BV",
		"Skip Extremity bones",	EVAV_EVVIT_Flags, (void *) 0, EVAV_ReadOnly, 3, 4, 0, 0, 0, "Display trace (old position)",
		-1
	);


	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ModifierCrushFlags,
		"Modifier Crush Flags",
		NULL,
		"X axis",			EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "Local X axis is deformation axe",
		"Y axis",			EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "Local Y axis is deformation axe",
		"Z axis",			EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "Local Z axis is deformation axe",
		"Neg Axis",			EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "True : Negative axis",
		-1
	);
	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ModifierFurFlags,
		"Modifier Fur/OutLine Flags",
		NULL,
		"Use OutLine",						EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "Out Line draw with Polygon",
		"Use Constant",						EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "Constant Size of Line",
		"Don't draw first layer",			EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "",
		-1
	);
	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
	(
		LINK_C_ModifierCloneFlags,
		"Modifier Clone Flags",
		NULL,
		"Light BV Reject",			EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "",
		"Don't Frustum Cull Reject",EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "",
		"Show clone",				EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "",
		-1
	);
    /*---------------------------------------------------------------------------------------------------------------*/
    LINK_RegisterStructType
	(
		LINK_C_GPG_Geom,
		"Setting of geom",
		LINK_CallBack_Refresh3DEngine,
		"Zoom Min",			EVAV_EVVIT_Float	, (void *)((char *)&st_GPG_Geom.f_ZoomMin -							(char *)&st_GPG_Geom)			, 0, Cf_Zero, Cf_Infinit, 0, 0	, 0,"",
		"Zoom Max",			EVAV_EVVIT_Float	, (void *)((char *)&st_GPG_Geom.f_ZoomMax -							(char *)&st_GPG_Geom)			, 0, Cf_Zero, Cf_Infinit, 0, 0	, 0,"",
		"Perturb Factor",	EVAV_EVVIT_Float	, (void *)((char *)&st_GPG_Geom.f_PertFactor -						(char *)&st_GPG_Geom)			, 0, Cf_Zero, Cf_Infinit, 0, 0	, 0,"",
		"Rand Vertex",		EVAV_EVVIT_Float	, (void *)((char *)&st_GPG_Geom.f_RandV -							(char *)&st_GPG_Geom)			, 0, Cf_Zero, Cf_Infinit, 0, 0	, 0,"",
		"Rand Pos",			EVAV_EVVIT_Float	, (void *)((char *)&st_GPG_Geom.f_RandP -							(char *)&st_GPG_Geom)			, 0, Cf_Zero, Cf_Infinit, 0, 0	, 0,"",
		"Dont Perturb",		EVAV_EVVIT_Flags,		(void *)((char *)&st_GPG_Geom.ul_Flags -						(char *)&st_GPG_Geom),			0, 0, 4, 0, 0, 0, "",
		"LOD 0",			EVAV_EVVIT_SubStruct,	(void *)((char *)&st_GPG_Geom.t_LOD[0] -						(char *)&st_GPG_Geom)			, 0, LINK_C_GPG_GeomLOD, 0, 0, 0, 0, "",
		"LOD 1",			EVAV_EVVIT_SubStruct,	(void *)((char *)&st_GPG_Geom.t_LOD[1] -						(char *)&st_GPG_Geom)			, 0, LINK_C_GPG_GeomLOD, 0, 0, 0, 0, "",
		"LOD 2",			EVAV_EVVIT_SubStruct,	(void *)((char *)&st_GPG_Geom.t_LOD[2] -						(char *)&st_GPG_Geom)			, 0, LINK_C_GPG_GeomLOD, 0, 0, 0, 0, "",
		-1
	);

    LINK_RegisterStructType
	(
		LINK_C_GPG_GeomLOD,
		"Setting of geom LOD",
		LINK_CallBack_Refresh3DEngine,
		"Geometry",			EVAV_EVVIT_Geom		, (void *)((char *)&st_GPG_GeomLOD.pt_Geom -						(char *)&st_GPG_GeomLOD)		, 0, 0, 0, 0, 0	, 0,"",
		"LOD",				EVAV_EVVIT_Float	, (void *)((char *)&st_GPG_GeomLOD.f_LOD -							(char *)&st_GPG_GeomLOD)		, 0, Cf_Zero, Cf_Infinit, 0, 0	, 0,"",
		-1
	);
#ifdef JADEFUSION
	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
		(
		LINK_C_LightShaftFlags,
		"Light Shaft Flags",
		LINK_CallBack_Refresh3DEngine,
		"Enable Noise",             EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "",
		"Enable 2nd Noise Texture", EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "",
		"Disable Fog",              EVAV_EVVIT_Flags, (void *) 0, 0, 3, 4, 0, 0, 0, "",
		//            "Blabla",           EVAV_EVVIT_Flags, (void *) 0, 0, BitIndex, 4, 0, 0, 0, "",
		-1
		);

	/*---------------------------------------------------------------------------------------------------------------*/
	LINK_RegisterStructType
		(
		LINK_C_LightCullingFlags,
		"Light culling flags",
		LINK_CallBack_Refresh3DEngine,
		"Cull lights per element",			        EVAV_EVVIT_Flags, (void *) 0, 0, 0, 4, 0, 0, 0, "True : Cull lights per element",
		"Cull spot lights with frustum (GAO)",      EVAV_EVVIT_Flags, (void *) 0, 0, 1, 4, 0, 0, 0, "True : Use frustum to cull spot lights from GAO light set",
		"Cull spot lights with frustum (Element)",  EVAV_EVVIT_Flags, (void *) 0, 0, 2, 4, 0, 0, 0, "True : Use frustum to cull spot lights from element light set",
		-1
		);

	LINK_RegisterStructType
		(
		LINK_C_MDF_SoftBodyColPlane,
		"Collision plane",
		LINK_Callback_ChangeSoftBodyColPlane,
		"Normal",	EVAV_EVVIT_Vector, (void *)((char *)(&(s_SoftBodyColPlane.m_LocalPlane.X)) - (char *)&s_SoftBodyColPlane), 0, 0, 0, 0, 0, 0, "",
		"W",		EVAV_EVVIT_Float,  (void *)((char *)(&(s_SoftBodyColPlane.m_LocalPlane.W)) - (char *)&s_SoftBodyColPlane), 0, -Cf_Infinit, Cf_Infinit, 0, 0, 0, "",
		-1
		);

	/*---------------------------------------------------------------------------------------------------------------*/
    LINK_RegisterStructType
        (
        LINK_C_XenonMeshProcessingFlags,
        "Xenon Mesh Processing Flags",
        LINK_CallBack_Refresh3DEngine,
        "Enable Tessellation", EVAV_EVVIT_Flags,    (void*)0, 0, 0, 4, 0, 0, 0, "",
        "Enable Displacement", EVAV_EVVIT_Flags,    (void*)0, 0, 1, 4, 0, 0, 0, "",
        "Enable Chamfer",      EVAV_EVVIT_Flags,    (void*)0, 0, 2, 4, 0, 0, 0, "",
        "Ignore Weights",      EVAV_EVVIT_Flags,    (void*)0, 0, 3, 4, 0, 0, 0, "",
        "Safe Mode",           EVAV_EVVIT_Flags,    (void*)0, 0, 4, 4, 0, 0, 0, "",
        "Smoothing Group",     EVAV_EVVIT_ConstInt, (void*)2, 0, 0, 0, 1, (int)"None\n0\nGroup 1\n1\nGroup 2\n2\nGroup 3\n3\nGroup 4\n4\n", 0, "Smoothing Group",
        -1
        );

	LINK_RegisterStructType
		(
		LINK_C_GDI_HighlightColors,
		"Highlight Colors",
		LINK_CallBack_Refresh3DEngine,
		"Color #1", EVAV_EVVIT_Color, (void *) ((char *) &st_HighlightColors.ul_Colors[0] - (char *) &st_HighlightColors), 0, 0, 0, 0, 0, 0, "Color used for Error",
		"Color #2", EVAV_EVVIT_Color, (void *) ((char *) &st_HighlightColors.ul_Colors[1] - (char *) &st_HighlightColors), 0, 0, 0, 0, 0, 0, "Color used for Warning",
		"Color #3", EVAV_EVVIT_Color, (void *) ((char *) &st_HighlightColors.ul_Colors[2] - (char *) &st_HighlightColors), 0, 0, 0, 0, 0, 0, "Additional Color",
		"Color #4", EVAV_EVVIT_Color, (void *) ((char *) &st_HighlightColors.ul_Colors[3] - (char *) &st_HighlightColors), 0, 0, 0, 0, 0, 0, "Additional Color",
		"Color #5", EVAV_EVVIT_Color, (void *) ((char *) &st_HighlightColors.ul_Colors[4] - (char *) &st_HighlightColors), 0, 0, 0, 0, 0, 0, "Additional Color",
		-1
		);
#endif

	/*$on*/
}

#endif /* ACTIVE_EDITORS */
