/*$T F3DView_subobject.cpp GC! 1.081 06/11/01 10:01:02 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "EDIpaths.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dview_undo.h"
#include "LINKs/LINKtoed.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJBoundingvolume.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#include "ENGine/Sources/COLlision/COLedit.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEO_MRM.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "GEOmetric/GEO_SKIN.h"
#include "GEOmetric/GEO_STRIP.h"
#include "SOFT/SOFThelper.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "DIAlogs/DIA_UPDATE_dlg.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAtoolbox_groview.h"
#include "DIAlogs/DIAshape_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAgrovertexpos_dlg.h"

#ifndef JADEFUSION
extern "C"
{
#endif
	extern SOFT_tdst_PickingBuffer *pPickForZCull;
	extern void GEO_CreateGeoFromCob(OBJ_tdst_GameObject *, GEO_tdst_Object *, COL_tdst_Cob *);
#ifndef JADEFUSION
}
#endif

#ifdef JADEFUSION
#include "GEOmetric/GEOXenonPack.h"
#endif
/*$4
 ***********************************************************************************************************************
    structures
 ***********************************************************************************************************************
 */

typedef struct	F3D_SubObject_PickData_
{
	char				c_Sel;
    char                c_Backface;
	float				f_Dist;
	int					i_Sel;
	OBJ_tdst_GameObject	*pst_GAO;
	GEO_tdst_Object		*pst_Obj;
	MATH_tdst_Vector	P0, P1;
	int					i_Id;
} F3D_SubObject_PickData;

/*$4
 ***********************************************************************************************************************
    globals
 ***********************************************************************************************************************
 */

ULONG								F3D_ul_SubObject_SelType;
static EDIA_cl_UPDATEDialog			*F3D_ul_MRMDial;
static BOOL							F3D_sb_VertexWeldResult;
static BOOL							F3D_sb_WindowSel;
static GEO_tdst_Object				*F3D_spst_EdgeOpObj;
static GEO_tdst_SubObject_ETurnData F3D_sst_EdgeTurnedData;
static GEO_tdst_SubObject_ECutData	F3D_sst_EdgeCutData;
static int							F3D_si_UpdateMorph;
#ifdef JADEFUSION
extern void COL_OK3_Build(void *_p_Data, BOOL _b_ColMap, BOOL _b_ReBuild);
#else
extern "C" void COL_OK3_Build(void *, BOOL, BOOL);
#endif
extern	BOOL F3D_b_MoveBVCameraAxis(SEL_tdst_SelectedItem *, ULONG, ULONG);

static int	F3D_i_LockedId;

/*$5
 #######################################################################################################################
    Sub object mode
 #######################################################################################################################
 */

/*
 =======================================================================================================================
    Aim:    Swap to sub object selection / edition mode
 =======================================================================================================================
 */
void F3D_BeginSubObjectMode(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_Object *pst_Geo;
	float			f_MRMExp, f_MRMMin;
    BOOL            b_Stripped;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_NoSubObjectMode) return;

	pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(_pst_GO);
	if(!pst_Geo) return;

	if(GEO_MRM_ul_IsMrmObject(pst_Geo))
	{
		GEO_MRM_GetCurve(pst_Geo, &f_MRMExp, &f_MRMMin);
		GEO_MRM_Destroy(pst_Geo);
	}
	else
	{
		*(LONG *) &f_MRMExp = -1;
		*(LONG *) &f_MRMMin = -1;
	}

    b_Stripped = ( pst_Geo->ulStripFlag & GEO_C_Strip_DataValid);
    if (b_Stripped)
	{
		LINK_PrintStatusMsg( "[WARNING] Some geometry are Stripped, It could be long when exiting the sub object mode" );
        GEO_STRIP_Delete( pst_Geo );
	}

	GEO_SubObject_Create(pst_Geo);

	pst_Geo->pst_SubObject->af_MRMParams[0] = f_MRMExp;
	pst_Geo->pst_SubObject->af_MRMParams[1] = f_MRMMin;
    pst_Geo->pst_SubObject->b_Stripped = b_Stripped;
}
/**/
BOOL F3D_b_Selection_BeginSubObjectMode(SEL_tdst_SelectedItem *_pst_Sel, ULONG, ULONG)
{
	F3D_BeginSubObjectMode((OBJ_tdst_GameObject *) _pst_Sel->p_Content);
	return TRUE;
}
/**/
void F3D_cl_View::Selection_BeginSubObjectMode(void)
{
	if(Selection_b_IsInSubObjectMode()) return;

    /* interdit le passage en édition sous objet si on est déja en édition de BV */
    if ( M_F3D_DD->uc_EditBounding )
    {
        M_MF()->MessageBox("Can't edit geometry while editing BV", "Warning", MB_OK | MB_ICONSTOP);
        return;
    }

	/* interdit le passage en édition sous objet si on est déja en édition de ODE */
	if ( M_F3D_DD->uc_EditODE )
	{
		M_MF()->MessageBox("Can't edit geometry while editing ODE", "Warning", MB_OK | MB_ICONSTOP);
		return;
	}
    /* interdit le passage en édition sous objet si on est création de géométrie */
    if (mpo_CreateGeometryDialog)
    {
        M_MF()->MessageBox("Can't edit geometry while creating new geometry", "Warning", MB_OK | MB_ICONSTOP);
        return;
    }


	M_F3D_EditOpt->ul_Flags |= GRO_Cul_EOF_SubObject;
	if(!(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Mode)) M_F3D_EditOpt->ul_Flags |= GRO_Cul_EOF_Vertex;

	if(mst_WinHandles.pst_World)
	{
		SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_Selection_BeginSubObjectMode, 0, 0);
#ifdef JADEFUSION
		Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif
		LINK_Refresh();
	}

	if(mpo_ToolBoxDialog)
	{
		((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->UpdateControls();
	}

}

/*
 =======================================================================================================================
    Aim:    Swap to normal object selection/edition mode
 =======================================================================================================================
 */
ULONG F3D_ul_MRMSetBarreState(float Position, char *Text)
{
	return F3D_ul_MRMDial->OnRefreshBarText(Position, Text);
}
/**/
void F3D_StripSetBarreState(float Position)
{
	F3D_ul_MRMDial->OnRefreshBarText(Position, "Compute Strip");
}
/**/
void F3D_EndSubObjectMode(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_Object *pst_Geo;
	float			f_MRMMin, f_MRMExp;
    BOOL            b_Stripped;
    ULONG           *pul_Colors;
	ULONG			ul_SaveEditorsFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(_pst_GO);
	if(!pst_Geo || !pst_Geo->pst_SubObject) return;

	f_MRMExp = pst_Geo->pst_SubObject->af_MRMParams[0];
	f_MRMMin = pst_Geo->pst_SubObject->af_MRMParams[1];
    b_Stripped = pst_Geo->pst_SubObject->b_Stripped;

	GEO_SubObject_Free(pst_Geo);

	if(*(LONG *) &f_MRMExp != -1)
	{
		F3D_ul_MRMDial = new EDIA_cl_UPDATEDialog((char *) "");
		F3D_ul_MRMDial->DoModeless();
		GEO_MRM_Compute(_pst_GO, pst_Geo, TRUE,F3D_ul_MRMSetBarreState);
		delete(F3D_ul_MRMDial);
		GEO_MRM_SetCurve(pst_Geo, f_MRMExp, f_MRMMin);
	}

    if (b_Stripped)
    {
		ul_SaveEditorsFlags = pst_Geo->ul_EditorFlags;
        pul_Colors = NULL;
        if ( (_pst_GO->pst_Base) && (_pst_GO->pst_Base->pst_Visu) )
            pul_Colors = _pst_GO->pst_Base->pst_Visu->dul_VertexColors;
		F3D_ul_MRMDial = new EDIA_cl_UPDATEDialog((char *) "");
		F3D_ul_MRMDial->DoModeless();
#ifdef JADEFUSION
#if defined(_XENON_RENDER)
        if ((_pst_GO->pst_Base) && (_pst_GO->pst_Base->pst_Visu))
        {
            GEO_ClearXenonMesh(_pst_GO->pst_Base->pst_Visu, pst_Geo, FALSE, FALSE);
        }
#endif
#endif
        GEO_STRIP_Compute( _pst_GO , pst_Geo, pul_Colors, FALSE, F3D_StripSetBarreState );
		delete(F3D_ul_MRMDial);
		pst_Geo->ul_EditorFlags = ul_SaveEditorsFlags;
    }
}
/**/
BOOL F3D_b_Selection_EndSubObjectMode(SEL_tdst_SelectedItem *_pst_Sel, ULONG, ULONG)
{
	F3D_EndSubObjectMode((OBJ_tdst_GameObject *) _pst_Sel->p_Content);
	return TRUE;
}
/**/
void F3D_cl_View::Selection_EndSubObjectMode(void)
{
	if(!Selection_b_IsInSubObjectMode()) return;

	M_F3D_EditOpt->ul_Flags &= ~GRO_Cul_EOF_SubObject;

	if(mst_WinHandles.pst_World)
	{
		SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_Selection_EndSubObjectMode, 0, 0);
#ifdef JADEFUSION
		Selection_XenonPostOperation(M_F3D_Sel, TRUE, FALSE);
#endif
		LINK_Refresh();
	}

	if(mpo_ToolBoxDialog)
	{
		((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->UpdateControls();
	}

    /* si on était en mode scale et axe non local, retourne en local */
    if(M_F3D_Helpers->ul_Flags & SOFT_Cul_HF_Scale) 
        Helper_SetAxis( SOFT_Cul_HF_SAObject );

    Selection_SubObject_DestroyVertexPos();
}

/*
 =======================================================================================================================
    Aim:    To know if we are in sub object mode
 =======================================================================================================================
 */
BOOL F3D_b_Selection_IsSelSubObject(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	LONG				*pl_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	pl_Sel = (LONG *) _ul_Param1;

	*pl_Sel = GEO_l_SubObject_GetNbPickedVertices(pst_Obj);

	if(*pl_Sel) return FALSE;

	return TRUE;
}
/**/
BOOL F3D_cl_View::Selection_b_IsInSubObjectMode(BOOL _b_TestSelData)
{
	/*~~~~~~~~~~*/
	LONG	l_Sel;
	/*~~~~~~~~~~*/

	if(!mst_WinHandles.pst_World) return FALSE;
	if(!(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_SubObject)) return FALSE;

	if(!_b_TestSelData) return TRUE;

	l_Sel = 0;
	if(mst_WinHandles.pst_World)
		SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_IsSelSubObject, (int) & l_Sel, 0);

	return(l_Sel != 0);
}
#ifdef JADEFUSION
/*
 =======================================================================================================================
Aim:    Get the Current SubObject Mode
=======================================================================================================================
*/

BOOL F3D_cl_View::Selection_b_IsInSubObjectEdgeMode()
{
	if(!EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_SubObjShortCut))
		return FALSE;

	if(!mst_WinHandles.pst_World) return FALSE;
	if(!(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_SubObject)) return FALSE;
	if(!(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Edge)) return FALSE;

    return TRUE;
}

/*
=======================================================================================================================
Aim:    Get the Current SubObject Mode
=======================================================================================================================
*/

int F3D_cl_View::Selection_i_GetSubObjectMode()
{
	if(!EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_SubObjShortCut))
		return -1;

	if(Selection_b_IsInSubObjectMode())
	{
		if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Vertex)
			return 0;
		if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Edge)
			return 1;
		if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Face)
			return 2;
	}
	return -1;
}

/*
=======================================================================================================================
Aim:    Switch between different SubObject Mode
=======================================================================================================================
*/

void F3D_cl_View::Selection_SwitchSubObjectMode()
{
	if(!EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_SubObjShortCut))
		return;

	switch(Selection_i_GetSubObjectMode())
	{
	case F3D_SubObject_Vertex:
		((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnRadio_EdgeMode();
		break;
	case F3D_SubObject_Edge:
		((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnRadio_FaceMode();
		break;
	case F3D_SubObject_Face:
		((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnRadio_VertexMode();
		break;
	default:
		break;
	}
}

/*
=======================================================================================================================
Aim:    Get the Current SubObject - Edge Mode
=======================================================================================================================
*/

int F3D_cl_View::Selection_i_GetSubObjectEdgeMode()
{
	if(!EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_SubObjShortCut))
		return -1;

	if(Selection_b_IsInSubObjectEdgeMode())
	{
		if(M_F3D_EditOpt->i_EdgeTool == 0)
			return 0;
		if(M_F3D_EditOpt->i_EdgeTool == GRO_i_EOT_EdgeTurn)
			return 1;
		if(M_F3D_EditOpt->i_EdgeTool == GRO_i_EOT_EdgeCut)
			return 2;
	}
	return -1;
}

/*
=======================================================================================================================
Aim:    Switch between SubObject - Edge Mode
=======================================================================================================================
*/

void F3D_cl_View::Selection_SwitchSubObjectEdgeMode()
{
	if(!EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_SubObjShortCut))
		return;

	switch(Selection_i_GetSubObjectEdgeMode())
	{
	case F3D_SubObject_Edge_Null:
		((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnCheck_TurnEdge();
		break;
	case F3D_SubObject_Edge_Turn:
		((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnCheck_CutEdge();
		break;
	case F3D_SubObject_Edge_Cut:
		((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnCheck_CutEdge();
		break;
	default:
		break;
	}
}

#endif
/*
 =======================================================================================================================
    Aim:    Color all selected vertices
 =======================================================================================================================
 */
// If there is RLI in the GRO, it will be modified. If there isn't, the function will modify
// the RLI of the GAO
BOOL F3D_b_Selection_ColorGROVertices(SEL_tdst_SelectedItem *_pst_Sel, ULONG ul_Color, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	if(pst_Obj)
	{
		if (pst_Obj->dul_PointColors)
			GEO_SubObject_ColorVertices(pst_Obj, &pst_Obj->dul_PointColors, ul_Color, *(float *) p2);
		else //if (pst_GO->pst_Base->pst_Visu->dul_VertexColors)
			GEO_SubObject_ColorVertices(pst_Obj, &pst_GO->pst_Base->pst_Visu->dul_VertexColors, ul_Color, *(float *) p2);
	}

	return TRUE;
}
// Same thing for the alpha component
BOOL F3D_b_Selection_ColorAlphaGROVertices(SEL_tdst_SelectedItem *_pst_Sel, ULONG ul_Color, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	if(pst_Obj)
	{
		if (pst_Obj->dul_PointColors)
			GEO_SubObject_ColorAlphaVertices
			(
				pst_Obj,
				&pst_Obj->dul_PointColors,
				ul_Color,
				*(float *) p2
			);
		else // if (pst_GO->pst_Base->pst_Visu->dul_VertexColors)
			GEO_SubObject_ColorAlphaVertices(
				pst_Obj,
				&pst_GO->pst_Base->pst_Visu->dul_VertexColors,
				ul_Color,
				*(float *) p2
			);
	}

	return TRUE;
}
// If there is RLI in the GAO, it will be modified. If there isn't, the function will modify
// the RLI of the GRO
BOOL F3D_b_Selection_ColorGAOVertices(SEL_tdst_SelectedItem *_pst_Sel, ULONG ul_Color, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	if(pst_Obj)
	{
		if (pst_GO->pst_Base->pst_Visu->dul_VertexColors
			|| (!pst_GO->pst_Base->pst_Visu->dul_VertexColors && !pst_Obj->dul_PointColors))
			GEO_SubObject_ColorVertices(pst_Obj, &pst_GO->pst_Base->pst_Visu->dul_VertexColors, ul_Color, *(float *) p2);
		else if (pst_Obj->dul_PointColors)
			GEO_SubObject_ColorVertices(pst_Obj, &pst_Obj->dul_PointColors                    , ul_Color, *(float *) p2);
	}

	return TRUE;
}
// Same thing for the alpha component
BOOL F3D_b_Selection_ColorAlphaGAOVertices(SEL_tdst_SelectedItem *_pst_Sel, ULONG ul_Color, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	if(pst_Obj)
	{
		if (pst_GO->pst_Base->pst_Visu->dul_VertexColors
			|| (!pst_GO->pst_Base->pst_Visu->dul_VertexColors && !pst_Obj->dul_PointColors))
			GEO_SubObject_ColorAlphaVertices(
				pst_Obj,
				&pst_GO->pst_Base->pst_Visu->dul_VertexColors,
				ul_Color,
				*(float *) p2
			);
		else if (pst_Obj->dul_PointColors)
			GEO_SubObject_ColorAlphaVertices
			(
				pst_Obj,
				&pst_Obj->dul_PointColors,
				ul_Color,
				*(float *) p2
			);
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_ColorVertices(ULONG ul_Color, float _f_Ratio, BOOL _b_Alpha)
{
	if(!Selection_b_IsInSubObjectMode()) return;
	if(_b_Alpha)
	{
		if (mb_WorkWithGRORLI)
			SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_Selection_ColorAlphaGROVertices, ul_Color, (ULONG) & _f_Ratio);
		else
			SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_Selection_ColorAlphaGAOVertices, ul_Color, (ULONG) & _f_Ratio);
	}
	else
		if (mb_WorkWithGRORLI)
			SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_Selection_ColorGROVertices, ul_Color, (ULONG) & _f_Ratio);
		else	
			SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_Selection_ColorGAOVertices, ul_Color, (ULONG) & _f_Ratio);
#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, TRUE);
#endif
	LINK_Refresh();
}

/*
 =======================================================================================================================
    Aim:    Color all selected vertices
 =======================================================================================================================
 */
BOOL F3D_b_Selection_PickGAOVertexColor(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_ValuePtr, ULONG _ul_ResultPtr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	unsigned long		*pul_RLI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*(ULONG *) _ul_ResultPtr = 0;
	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	if(pst_Obj)
	{
		pul_RLI = pst_GO->pst_Base->pst_Visu->dul_VertexColors;
		if(GEO_l_SubObject_PickVertexColor(pst_Obj, pul_RLI, (ULONG *) _ul_ValuePtr))
		{
			*(ULONG *) _ul_ResultPtr = 1;
			return FALSE;
		}
	}

	return TRUE;
}
BOOL F3D_b_Selection_PickGROVertexColor(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_ValuePtr, ULONG _ul_ResultPtr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	unsigned long		*pul_RLI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*(ULONG *) _ul_ResultPtr = 0;
	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	if(pst_Obj)
	{
		pul_RLI = pst_Obj->dul_PointColors;
		if(GEO_l_SubObject_PickVertexColor(pst_Obj, pul_RLI, (ULONG *) _ul_ValuePtr))
		{
			*(ULONG *) _ul_ResultPtr = 1;
			return FALSE;
		}
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_PickVertexColor(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned long	ul_Result, ul_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!Selection_b_IsInSubObjectMode()) return;
	if (mpo_ToolBoxDialog->mpo_View->mb_WorkWithGRORLI)
		SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_Selection_PickGROVertexColor, (ULONG) & ul_Color, (ULONG) & ul_Result);
	else
		SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_Selection_PickGAOVertexColor, (ULONG) & ul_Color, (ULONG) & ul_Result);
	if(ul_Result)
	{
		M_F3D_EditOpt->ul_VertexPaintColor = ul_Color;
		if(mpo_ToolBoxDialog) ((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->UpdateControls();
	}
}

/*
 =======================================================================================================================
    Aim:    WeldVertex
 =======================================================================================================================
 */
BOOL F3D_b_Selection_VertexWeld(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	BOOL				b_ColMap;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i = -1;

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		b_ColMap = FALSE;
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			b_ColMap = TRUE;

			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);

		}
		else
			return TRUE;
	}

	if(pst_Obj)
	{
		/* pick a vertex */
		i = GEO_i_SubObject_VPick(pst_Obj, (MATH_tdst_Vector *) p2, 0, pst_GO);
		if(i != -1)
		{
			GEO_SubObject_VWeld(pst_Obj, i);
			F3D_sb_VertexWeldResult = TRUE;
		}
	}

	if(b_ColMap) COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);
    
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
        OBJ_ComputeBV( pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);

	/* Rebuild Visual OK3 */
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		COL_OK3_Build(pst_Obj, FALSE, TRUE);

	return TRUE;
}
/**/
BOOL F3D_cl_View::Selection_b_SubObject_VertexWeld(POINT *_pst_Pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	BOOL				b_Locked;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!Selection_b_IsInSubObjectMode()) return FALSE;

	b_Locked = mpo_CurrentModif != NULL;
	if(b_Locked) mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);

	mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this, b_Locked);
	mpo_CurrentModif->SetDesc("Weld vertex");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	F3D_sb_VertexWeldResult = FALSE;
	v.x = (float) _pst_Pt->x;
	v.y = (float) M_F3D_DD->pst_PickingBuffer->l_Height - _pst_Pt->y;

	SEL_EnumItem
	(
		M_F3D_Sel,
		SEL_C_SIF_Object | SEL_C_SIF_Cob,
		F3D_b_Selection_VertexWeld,
		(ULONG) M_F3D_DD->pst_PickingBuffer,
		(ULONG) & v
	);
#ifdef JADEFUSION
	Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif
	return F3D_sb_VertexWeldResult;
}

/*
 =======================================================================================================================
    Aim:    weld selected vertex with a threshold
 =======================================================================================================================
 */
BOOL F3D_b_Selection_VertexWeldThresh(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	BOOL				b_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		b_ColMap = FALSE;
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			b_ColMap = TRUE;
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	if(pst_Obj) 
        GEO_i_SubObject_VWeldThresh(pst_Obj, *(float *) p1 );

	if(b_ColMap) COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);

    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
        OBJ_ComputeBV( pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);

	/* Rebuild Visual OK3 */
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		COL_OK3_Build(pst_Obj, FALSE, TRUE);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_VertexWeldThresh(float _f_Thresh)
{
	if(!Selection_b_IsInSubObjectMode()) return;

	mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this);
	mpo_CurrentModif->SetDesc("Weld with threshold");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;
	
	if (_f_Thresh <= 0)
		_f_Thresh = M_F3D_EditOpt->f_VertexWeldThresh;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_VertexWeldThresh, (ULONG) &_f_Thresh, 0 );

#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif	
	LINK_Refresh();
}

/*
 =======================================================================================================================
    Aim:    DelVertex
 =======================================================================================================================
 */
BOOL F3D_b_Selection_DelVertex(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	BOOL				b_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		b_ColMap = FALSE;
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			b_ColMap = TRUE;
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	GEO_SubObject_VDelSel(pst_Obj, NULL);

	if(b_ColMap) COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);

    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
        OBJ_ComputeBV( pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);

	/* Rebuild Visual OK3 */
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		COL_OK3_Build(pst_Obj, FALSE, TRUE);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_DelVertex(void)
{
	if(!Selection_b_IsInSubObjectMode()) return;

	mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this);
	mpo_CurrentModif->SetDesc("Delete points");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_DelVertex, 0, 0);
#ifdef JADEFUSION
	Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif
}

/*
 =======================================================================================================================
    Aim:    weld selected vertex with a threshold
 =======================================================================================================================
 */
BOOL F3D_b_Selection_VertexSym(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	MATH_tdst_Matrix	M;
	MATH_tdst_Vector	localcenter, localaxe;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	if (p2 == -1)
	{
		GEO_SubObject_VSym(pst_Obj, p1 );
	}
	else
	{
		MATH_SetIdentityMatrix( &M );
		MATH_InvertMatrix(&M, OBJ_pst_GetAbsoluteMatrix(pst_GO));
		if ( p1 )
		{
			MATH_TransformVertex(&localcenter, &M, (MATH_tdst_Vector *) p1 );
			p1 = (ULONG) &localcenter;
		}
		if ( p2 )
		{
			MATH_TransformVectorNoScale( &localaxe, &M, (MATH_tdst_Vector *) p2 );
			p2 = (ULONG ) &localaxe;
		}
		GEO_SubObject_VSymAll( pst_Obj, (GEO_Vertex *) &p1, (MATH_tdst_Vector *) p2 );
	}


    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
        OBJ_ComputeBV( pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);
		/* Rebuild Visual OK3 */
		COL_OK3_Build(pst_Obj, FALSE, TRUE);
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_VertexSym(void)
{
    int					axe;
	MATH_tdst_Vector	center, *C, *V;

	if(!Selection_b_IsInSubObjectMode()) return;

	mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this);
	mpo_CurrentModif->SetDesc("Vertex symetrie");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;


	MATH_CopyVector(&center, &M_F3D_Helpers->st_GlobalMatrix.T);

    axe = M_F3D_EditOpt->i_SymetrieOp;
	if (axe < GRO_Cul_EOSYM_PivotHelper_X )
		SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_VertexSym, axe, 0xFFFFFFFF );
	else
	{
		MATH_CopyVector(&center, &M_F3D_Helpers->st_GlobalMatrix.T);
		switch( axe )
		{
		case GRO_Cul_EOSYM_PivotHelper_X:	V = MATH_pst_GetXAxis( &M_F3D_Helpers->st_MatrixGlobalAxis ); C = NULL;	break;
		case GRO_Cul_EOSYM_PivotHelper_Y:	V = MATH_pst_GetYAxis( &M_F3D_Helpers->st_MatrixGlobalAxis ); C = NULL;	break;
		case GRO_Cul_EOSYM_PivotHelper_Z:	V = MATH_pst_GetZAxis( &M_F3D_Helpers->st_MatrixGlobalAxis ); C = NULL;	break;
		case GRO_Cul_EOSYM_HelperHelper_X:	V = MATH_pst_GetXAxis( &M_F3D_Helpers->st_MatrixGlobalAxis ); C = &center;	break;
		case GRO_Cul_EOSYM_HelperHelper_Y:	V = MATH_pst_GetYAxis( &M_F3D_Helpers->st_MatrixGlobalAxis ); C = &center;	break;
		case GRO_Cul_EOSYM_HelperHelper_Z:	V = MATH_pst_GetZAxis( &M_F3D_Helpers->st_MatrixGlobalAxis ); C = &center;	break;
		case GRO_Cul_EOSYM_Pivot_O:			V = NULL; C = NULL; break;
		case GRO_Cul_EOSYM_Helper_O :		V = NULL; C = &center;
		default: V = NULL;
		}
		SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_VertexSym, (ULONG) &center, (ULONG) V );
	}
#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif
	LINK_Refresh();
}

/*
 =======================================================================================================================
    Aim:    compute pivot so it will be centered on object£
    p1 = type de centre :	0 center bouding volume£
							1 gravity center £
							2 given pos £
	p2 = NULL pour type 0 & 1, pour type 2 = position globale £
 =======================================================================================================================
 */
BOOL F3D_b_Selection_SubObjectPivotCenter(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	MATH_tdst_Matrix	M;
	MATH_tdst_Vector	*v, v_global, v_local;
	int					i_type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	i_type = (int) p1;
	v = (MATH_tdst_Vector *) p2;

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}
	
	if ( i_type == 2 )
	{
		MATH_SubVector( &v_global, v, OBJ_pst_GetAbsolutePosition( pst_GO ) );
		MATH_InvertMatrix(&M, OBJ_pst_GetAbsoluteMatrix(pst_GO));
	    MATH_TransformVectorNoScale(&v_local, &M, &v_global);
		if(MATH_b_TestScaleType(&M))
		{
			v_local.x *= M.Sx;
			v_local.y *= M.Sy;
			v_local.z *= M.Sz;
		}
	}
	else
	{
		GEO_l_SubObject_ComputePickedVerticesCenter( pst_Obj, &v_local, (i_type == 0) ? -3 : -2 );
		MATH_TransformVector( &v_global, OBJ_pst_GetAbsoluteMatrix( pst_GO ), &v_local );
	}
	
	MATH_AddEqualVector(OBJ_pst_GetAbsolutePosition(pst_GO), &v_global);
	OBJ_ComputeLocalWhenHie(pst_GO);
	MATH_NegEqualVector(&v_local);
	GEO_SubObject_MoveVertices(pst_Obj, &v_local, 1);

	/*
	 * If we are editing the Pivot of one GameObject, we have to also move to vertices
	 * of its Triangles-based Cob if it has one.
	 */
	if( _pst_Sel->l_Flag & SEL_C_SIF_Object )
	{
		if (pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)
		{
			/*~~~~~~~~~~~~~~~~~~~~~*/
			COL_tdst_Cob	*pst_Cob;
			/*~~~~~~~~~~~~~~~~~~~~~*/

			pst_Cob = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0];

			if((pst_Cob) && (pst_Cob->uc_Type == COL_C_Zone_Triangles))
			{
				if(!pst_Cob->p_GeoCob)
				{
					GEO_tdst_Object			*pst_GeoCob;

					pst_Cob->p_GeoCob = MEM_p_Alloc(sizeof(GEO_tdst_Object));
					pst_GeoCob = (GEO_tdst_Object *) pst_Cob->p_GeoCob;

					/* Fill the st_GeoCob structure with info needed to see the Cob. */
					L_memset(pst_GeoCob, 0, sizeof(GEO_tdst_Object));

					pst_GeoCob->st_Id.i = &GRO_gast_Interface[GRO_Geometric];
					GEO_CreateGeoFromCob(pst_GO, pst_GeoCob, pst_Cob);
				}

				if(!(((GEO_tdst_Object *) pst_Cob->p_GeoCob)->pst_SubObject))
					GEO_SubObject_Create((GEO_tdst_Object *) pst_Cob->p_GeoCob);
				GEO_SubObject_MoveVertices((GEO_tdst_Object *) pst_Cob->p_GeoCob, &v_local, 1 );
				pst_Cob->uc_Flag |= COL_C_Cob_Updated;
			}
		}
		
		if ( OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_EditableBV) )
		{
			F3D_b_MoveBVCameraAxis( _pst_Sel, (ULONG) &v_global, 0);
		}
	}

    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
        OBJ_ComputeBV( pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_PivotCenter( int i_Type, MATH_tdst_Vector *V )
{
	if(!Selection_b_IsInSubObjectMode()) return;

	mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this);
	mpo_CurrentModif->SetDesc("Center pivot");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_SubObjectPivotCenter, (ULONG) i_Type, (ULONG) V );
#ifdef JADEFUSION
	Selection_XenonPostOperation(M_F3D_Sel, FALSE, TRUE);
#endif
}

/*
 =======================================================================================================================
    Aim:    change vertex position
 =======================================================================================================================
 */
BOOL F3D_b_Selection_VertexPos(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

    ((EDIA_cl_GroVertexPos *) p1)->AddData( pst_GO, pst_Obj);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_CreateVertexPos(void)
{
    ERR_X_Assert(!mpo_VertexPos);
    mpo_VertexPos = new EDIA_cl_GroVertexPos(this);
}
/**/
void F3D_cl_View::Selection_SubObject_DestroyVertexPos(void)
{
    if (mpo_VertexPos)
    {
        EDIA_cl_GroVertexPos *pTemp = mpo_VertexPos;
        mpo_VertexPos = NULL;

        if (pTemp->mb_Global)
            M_F3D_EditOpt->ul_Flags |= GRO_Cul_EOF_VertexPosGlobal;
        else
            M_F3D_EditOpt->ul_Flags &= ~GRO_Cul_EOF_VertexPosGlobal;

        if (pTemp->mb_Move)
            M_F3D_EditOpt->ul_Flags |= GRO_Cul_EOF_VertexPosMove;
        else
            M_F3D_EditOpt->ul_Flags &= ~GRO_Cul_EOF_VertexPosMove;

        pTemp->DestroyWindow();
    }
}
/**/
void F3D_cl_View::Selection_SubObject_VertexPos(void)
{
    BOOL bCreateVertexPos = !mpo_VertexPos;

    if(!Selection_b_IsInSubObjectMode()) return;

    if (bCreateVertexPos)
        Selection_SubObject_CreateVertexPos();
    else
    {
        mpo_VertexPos->Destroy();
        mpo_VertexPos->Create();
    }

    MATH_CopyVector( &mpo_VertexPos->mst_Pivot, &M_F3D_Helpers->st_GlobalMatrix.T );
    if (bCreateVertexPos)
    {
        mpo_VertexPos->mb_Global = (M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_VertexPosGlobal) ? TRUE : FALSE;
        mpo_VertexPos->mb_Move = (M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_VertexPosMove) ? TRUE : FALSE;
    }

    SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_VertexPos, (ULONG) mpo_VertexPos, 0);

    if (mpo_VertexPos->mi_NbData == 0)
    {
        M_MF()->MessageBox( "Select vertices first", "Error", MB_OK );
        return;
    }

    mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this);
	mpo_CurrentModif->SetDesc("Set vertex position");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

    Selection_SubObject_StartMovingVertex( M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_VertexMoveUpdateUV );

    if (bCreateVertexPos)
        mpo_VertexPos->DoModeless();
    else
        mpo_VertexPos->OnInitDialog();

    /*if ( DIA_VertexPos.DoModal() == IDCANCEL )
    {
        mo_UndoManager.b_Undo();
        LINK_Refresh();
    }*/
}

/*
 =======================================================================================================================
    Aim:    Turn Edge
 =======================================================================================================================
 */
BOOL F3D_b_Selection_TurnEdge(SEL_tdst_SelectedItem *_pst_Sel, ULONG, ULONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	if(pst_Obj)
	{
		if(GEO_i_SubObject_ETurnSel(pst_Obj, &F3D_sst_EdgeTurnedData))
		{
			/* Update Cob */
			if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
				COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);


			F3D_spst_EdgeOpObj = pst_Obj;
			return FALSE;
		}
	}

	/* Rebuild Visual OK3 */
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		COL_OK3_Build(pst_Obj, FALSE, TRUE);

	/* Update Cob */
	if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);


	return TRUE;
}
/**/
void F3D_cl_View::Selection_TurnEdge(void)
{
	if(!Selection_b_IsInSubObjectMode()) return;
	F3D_spst_EdgeOpObj = NULL;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_TurnEdge, 0, 0);
#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif
	if(F3D_spst_EdgeOpObj)
	{
		mpo_CurrentModif = new F3D_cl_Undo_TurnEdge(this, F3D_spst_EdgeOpObj, &F3D_sst_EdgeTurnedData);
		mpo_CurrentModif->SetDesc("Turn edge");
		mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
		mpo_CurrentModif = NULL;
	}
}

/*
 =======================================================================================================================
    Aim:    Cut Edge
 =======================================================================================================================
 */
BOOL F3D_b_Selection_CutEdge(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		*pst_GO;
	GEO_tdst_Object			*pst_Obj;
	GDI_tdst_DisplayData	*pst_DD;
	BOOL					b_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		b_ColMap = FALSE;
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			b_ColMap = TRUE;
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	pst_DD = (GDI_tdst_DisplayData *) p2;
	pst_DD->pst_CurrentGameObject = pst_GO;

	if(pst_Obj)
	{
		if(GEO_i_SubObject_ECutSel((GDI_tdst_DisplayData *) p2, pst_Obj, (MATH_tdst_Vector *) p1, &F3D_sst_EdgeCutData))
		{
			F3D_spst_EdgeOpObj = pst_Obj;

			if(b_ColMap) COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);

			return FALSE;
		}

	/* Rebuild Visual OK3 */
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		COL_OK3_Build(pst_Obj, FALSE, TRUE);
	}

	if(b_ColMap) COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_CutEdge(POINT *_pst_Pt)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~*/

	if(!Selection_b_IsInSubObjectMode()) return;

	v.x = (float) _pst_Pt->x;
	v.y = (float) _pst_Pt->y;

	F3D_spst_EdgeOpObj = NULL;
    F3D_sst_EdgeCutData.i_Divide = ( M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_EdgeDivide) ? 1 : 0;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_CutEdge, (ULONG) & v, (ULONG) M_F3D_DD);
#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif
	if(F3D_spst_EdgeOpObj)
	{
		mpo_CurrentModif = new F3D_cl_Undo_CutEdge(this, F3D_spst_EdgeOpObj, &F3D_sst_EdgeCutData);
		mpo_CurrentModif->SetDesc("Cut edge");
		mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
		mpo_CurrentModif = NULL;
	}
}

/*
 =======================================================================================================================
    Aim:    Cut Edge with cutter
 =======================================================================================================================
 */
BOOL F3D_b_Selection_EdgeCutter(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		*pst_GO;
	GEO_tdst_Object			*pst_Obj;
	GDI_tdst_DisplayData	*pst_DD;
    MATH_tdst_Vector        *V;
	BOOL					b_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		b_ColMap = FALSE;
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			b_ColMap = TRUE;
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	pst_DD = (GDI_tdst_DisplayData *) p1;
	pst_DD->pst_CurrentGameObject = pst_GO;

	if(!pst_Obj) return TRUE;

    V = (MATH_tdst_Vector *) p2;

	GEO_i_SubObject_ECutter( pst_DD, pst_Obj, V, V + 1, V + 2 );

	if(b_ColMap) COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);

	/* Rebuild Visual OK3 */
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		COL_OK3_Build(pst_Obj, FALSE, TRUE);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_EdgeCutter(POINT *_pst_Pt0, POINT *_pst_Pt1)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	A, B, C, Norm;
    MATH_tdst_Vector    V[3];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!Selection_b_IsInSubObjectMode()) return;
    if ( ( _pst_Pt0->x == _pst_Pt1->x ) && (_pst_Pt0->y == _pst_Pt1->y) ) return;

    _pst_Pt0->y = M_F3D_DD->st_Device.l_Height - _pst_Pt0->y;
    _pst_Pt1->y = M_F3D_DD->st_Device.l_Height - _pst_Pt1->y;

    mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif( this, FALSE );
    mpo_CurrentModif->SetDesc("Edge cutter");
    mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
    mpo_CurrentModif = NULL;

    MATH_InitVector(&Norm, (float) _pst_Pt0->x, (float) _pst_Pt0->y, 1);
	CAM_2Dto3DCamera(&M_F3D_DD->st_Camera, &A, &Norm);
    MATH_CopyVector( &V[0], &A );
    MATH_InitVector(&Norm, (float) _pst_Pt0->x, (float) _pst_Pt0->y, 2);
	CAM_2Dto3DCamera(&M_F3D_DD->st_Camera, &B, &Norm);
    MATH_SubEqualVector( &B, &A );
    MATH_InitVector(&Norm, (float) _pst_Pt1->x, (float) _pst_Pt1->y, 1);
	CAM_2Dto3DCamera(&M_F3D_DD->st_Camera, &C, &Norm);
    MATH_CopyVector( &V[1], &C );
    MATH_SubEqualVector( &C, &A );
    MATH_CrossProduct( &V[2], &B, &C );

    SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_EdgeCutter, (ULONG) M_F3D_DD, (ULONG) V );
#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif
}

/*
 =======================================================================================================================
    Aim:    Extrude selected edges
 =======================================================================================================================
 */
BOOL F3D_b_Selection_ExtrudeEdge(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	BOOL				b_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		b_ColMap = FALSE;
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			b_ColMap = TRUE;
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	if(pst_Obj) GEO_SubObject_EExtrude(pst_Obj, 0);

	if(b_ColMap) COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);

    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
        OBJ_ComputeBV( pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_ExtrudeEdge(void)
{
	if(!Selection_b_IsInSubObjectMode()) return;

	mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this);
	mpo_CurrentModif->SetDesc("Extrude edge");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_ExtrudeEdge, 0, (ULONG) M_F3D_DD);
#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif
}

/*
 =======================================================================================================================
    Aim:    Flip normals of selected triangles
 =======================================================================================================================
 */
BOOL F3D_b_Selection_FlipNormals(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	if(pst_Obj) GEO_SubObject_FlipNormals(pst_Obj);

	/* Rebuild Visual OK3 */
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		COL_OK3_Build(pst_Obj, FALSE, TRUE);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_FlipNormals(BOOL _b_CanUndo, BOOL _b_Locked)
{
	if(!Selection_b_IsInSubObjectMode()) return;

	if(_b_CanUndo)
	{
		mpo_CurrentModif = new F3D_cl_Undo_FlipFace(this, _b_Locked);
		mpo_CurrentModif->SetDesc("Flip face");
		mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
		mpo_CurrentModif = NULL;
	}

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_FlipNormals, 0, 0);
#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif
}


/*
 =======================================================================================================================
    Aim:    change id of selected triangles
 =======================================================================================================================
 */
BOOL F3D_b_Selection_ChangeId(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	BOOL				b_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		b_ColMap = FALSE;
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			b_ColMap = TRUE;
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;

				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	if(pst_Obj) GEO_SubObject_ChangeID(pst_Obj, p1, b_ColMap);
	if(b_ColMap) 
	{
		COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);
		COL_UpdateCameraNumberOfFaces(((COL_tdst_Cob *) _pst_Sel->p_Content));
	}

	/* Rebuild Visual OK3 */
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		COL_OK3_Build(pst_Obj, FALSE, TRUE);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_ChangeId(int _i_Id)
{
	if(!Selection_b_IsInSubObjectMode()) return;

	mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this);
	mpo_CurrentModif->SetDesc("Change id");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_ChangeId, (ULONG) _i_Id, 0);
#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif
}

/*
 =======================================================================================================================
    Aim:    DelFace
 =======================================================================================================================
 */
BOOL F3D_b_Selection_DelFace(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	BOOL				b_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		b_ColMap = FALSE;
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			b_ColMap = TRUE;
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	GEO_SubObject_FDelSel(pst_Obj, TRUE);

	if(b_ColMap) COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);

	/* Rebuild Visual OK3 */
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		COL_OK3_Build(pst_Obj, FALSE, TRUE);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_DelFace(void)
{
	if(!Selection_b_IsInSubObjectMode()) return;

	mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this);
	mpo_CurrentModif->SetDesc("Del face");
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_DelFace, 0, 0);
#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;
}

/*
 =======================================================================================================================
    Aim:    DelFace
 =======================================================================================================================
 */
BOOL F3D_b_Selection_HideFace(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
#ifdef JADEFUSION
	GRO_tdst_Visu       *pst_Visu = NULL;
#endif
	BOOL				b_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		b_ColMap = FALSE;
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);

#ifdef JADEFUSION
        if (pst_GO && pst_GO->pst_Base)
        {
            pst_Visu = pst_GO->pst_Base->pst_Visu;
        }
#endif
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			b_ColMap = TRUE;
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

#ifdef JADEFUSION
	GEO_SubObject_HideSelectedFaces(pst_Obj, pst_Visu, p1, p2);
#else
	GEO_SubObject_HideSelectedFaces(pst_Obj, p1, p2);
#endif
	if(b_ColMap) COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);

	/* Rebuild Visual OK3 */
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		COL_OK3_Build(pst_Obj, FALSE, TRUE);
	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_HideFace(BOOL _b_Unsel)
{
	if(!Selection_b_IsInSubObjectMode()) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_HideFace, TRUE, _b_Unsel);
	Invalidate();
}
/**/
void F3D_cl_View::Selection_SubObject_UnHideFace(void)
{
	if(!Selection_b_IsInSubObjectMode()) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_HideFace, FALSE, 0);
	Invalidate();
}

/*
 =======================================================================================================================
    Aim:    Select face extended
 =======================================================================================================================
 */
BOOL F3D_b_Selection_SelFaceExt(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	if (!pst_Obj) 
		return TRUE;
		
	switch (p1)
	{
	case GRO_Cul_EOF_FaceSelExt:
		GEO_i_SubObject_FSelectExt(pst_Obj, F3D_i_LockedId);
		break;
	case GRO_Cul_EOF_FaceSelNear:
		GEO_i_SubObject_FSelectNear(pst_Obj, 0, F3D_i_LockedId );
		break;
	case GRO_Cul_EOF_FaceSelExtUV:
		GEO_i_SubObject_FSelectExtUV(pst_Obj );
		break;
	case GRO_Cul_EOF_FaceSelNearEdge:
		GEO_i_SubObject_FSelectNear(pst_Obj, 1, F3D_i_LockedId );
		break;
	}

    if(pst_Obj->pst_SubObject->pfn_UVMapper_Update)
	    pst_Obj->pst_SubObject->pfn_UVMapper_Update(pst_Obj->pst_SubObject->ul_UVMapper_Param);

	/* Rebuild Visual OK3 */
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		COL_OK3_Build(pst_Obj, FALSE, TRUE);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_SelFaceExt(void)
{
	if(!Selection_b_IsInSubObjectMode()) return;

	mpo_CurrentModif = new F3D_cl_Undo_Selection(this);
	mpo_CurrentModif->SetDesc("Extended selection of face");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;
	
	F3D_i_LockedId = (M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_FaceLockId) ? M_F3D_EditOpt->l_FaceId : -1;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_SelFaceExt, M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_FaceSelMask, 0);
}

/*
 =======================================================================================================================
    Aim:    invert selection
 =======================================================================================================================
 */
BOOL F3D_b_Selection_InvertSel(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

    GEO_SubObject_InvertSel(pst_Obj, p1);
    LINK_Refresh();
	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_InvertSel(void)
{
	if(!Selection_b_IsInSubObjectMode()) return;

	mpo_CurrentModif = new F3D_cl_Undo_Selection(this);
	mpo_CurrentModif->SetDesc("invert selection");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

    SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_InvertSel, M_F3D_EditOpt->ul_Flags, 0);
}

/*
 =======================================================================================================================
    Attach à l'objet couramment sélectionné toutes les faces sélectionnées ne lui appartenant pas.
 =======================================================================================================================
 */
void F3D_cl_View::Selection_AttachFace( OBJ_tdst_GameObject *_pst_GO )
{
    OBJ_tdst_GameObject     *G1, *G2;
    SEL_tdst_SelectedItem   *pst_Sel;
    GEO_tdst_Object         *Geo1, *Geo2;
    MATH_tdst_Matrix        I, M;
	BOOL					b_Cob;
	COL_tdst_Cob			*pst_Cob;
    
	b_Cob = FALSE;
    pst_Sel = SEL_pst_GetFirst( M_F3D_Sel, SEL_C_SIF_Object);
    if (!pst_Sel)
	{
		pst_Sel = SEL_pst_GetFirst( M_F3D_Sel, SEL_C_SIF_Cob);
		if (!pst_Sel)
			return;

		b_Cob = TRUE;

		pst_Cob = (COL_tdst_Cob *) pst_Sel->p_Content;

		G1 = pst_Cob->pst_GO;
//		if (G1 != _pst_GO) return;

		if(pst_Cob->p_GeoCob)
		{
			pst_Cob->uc_Flag |= COL_C_Cob_Updated;
			Geo1 = (GEO_tdst_Object *) (pst_Cob->p_GeoCob);
		}
		else
			Geo1 = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(G1);

		if ( !Geo1 ) return;

	}
	else
	{
    
		G1 = (OBJ_tdst_GameObject *) pst_Sel->p_Content;
		if (G1 != _pst_GO) return;
		Geo1 = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo( G1 );
		if ( !Geo1 ) return;
	}

    MATH_SetIdentityMatrix( &I );
    MATH_InvertMatrix( &I, OBJ_pst_GetAbsoluteMatrix( G1 ) );

    mpo_CurrentModif = new F3D_cl_Undo_GeoModif(this, Geo1);
	mpo_CurrentModif->SetDesc("reorder vertex index");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

    while (1)
    {
		if(b_Cob)
		{
	        pst_Sel = SEL_pst_GetNext( M_F3D_Sel, pst_Sel, SEL_C_SIF_Cob );
			if (!pst_Sel) break;

			G2 = ((COL_tdst_Cob *) pst_Sel->p_Content)->pst_GO;

			if(((COL_tdst_Cob *) pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				Geo2 = (GEO_tdst_Object *) (((COL_tdst_Cob *) pst_Sel->p_Content)->p_GeoCob);
			}
			else
				Geo2 = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(G2);

			if ( !Geo2 ) break;

		}
		else
		{
	        pst_Sel = SEL_pst_GetNext( M_F3D_Sel, pst_Sel, SEL_C_SIF_Object );
			if (!pst_Sel) break;

	        G2 = (OBJ_tdst_GameObject *) pst_Sel->p_Content;
		    Geo2 = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo( G2 );

		}


        if ( !Geo2 ) continue;

        MATH_MulMatrixMatrix( &M, OBJ_pst_GetAbsoluteMatrix( G2 ), &I );
        GEO_SubObject_FAttach( Geo1, Geo2, G2->pst_Base->pst_Visu->dul_VertexColors, &M , G1,G2);
    }

	if(b_Cob) COL_SynchronizeCob(pst_Cob, FALSE,FALSE);

    OBJ_ComputeBV( G1, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);
    LINK_Refresh();
}

/*
 =======================================================================================================================
    Aim:    Detach faces
 =======================================================================================================================
 */
BOOL F3D_b_Selection_DetachFace(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	BOOL				b_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		b_ColMap = FALSE;
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			b_ColMap = TRUE;
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

    if (p1)
        *((MATH_tdst_Matrix **) p1) = OBJ_pst_GetAbsoluteMatrix( pst_GO );

	GEO_SubObject_FDetach(pst_Obj);

	if(b_ColMap) COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);

    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
        OBJ_ComputeBV( pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);

	/* Rebuild Visual OK3 */
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		COL_OK3_Build(pst_Obj, FALSE, TRUE);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_DetachFace(void)
{
    MATH_tdst_Matrix *pst_Matrix;

	if(!Selection_b_IsInSubObjectMode()) return;

	mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this);
	mpo_CurrentModif->SetDesc("Detach face");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_DetachFace, (ULONG) &pst_Matrix, 0);

	Selection_SubObject_AfterDetachFace( pst_Matrix );
#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif
}

/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    after detach faces : create new geometry ??
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	F3D_SubObject_Count_
{
	int					i_NbVertex, i_CurVertex;
	int					i_NbUV, i_CurUV;
	int					i_NbElement;
	int					i_Id[256];
	int					i_NbTriangle[256];
	int					i_CurTriangle[256];
	MATH_tdst_Matrix	st_Invert;
    MAT_tdst_Material   *pst_Mat;
    int                 i_RLI;
    ULONG               *pul_RLI;
	ULONG				ul_DrawMask;
} F3D_SubObject_Count;
/**/
BOOL F3D_b_Selection_AfterDetachFace(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			*pst_GO;
	GEO_tdst_Object				*pst_Obj;
	int							i, j;
	char						*pc_FSel;
	GEO_tdst_IndexedTriangle	*pst_Triangle;
	int							i_ElemIndex;
	F3D_SubObject_Count			*pst_Data;
	char						*V, *UV;
	int							i_NbFace;
	BOOL						b_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		b_ColMap = FALSE;
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			b_ColMap = TRUE;
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	if(!pst_Obj || !pst_Obj->pst_SubObject) return TRUE;

	pst_Data = (F3D_SubObject_Count *) p1;

    if (pst_Data->pst_Mat == NULL)
        pst_Data->pst_Mat = (MAT_tdst_Material *) OBJ_p_GetGrm( pst_GO );

	V = (char *) L_malloc(pst_Obj->l_NbPoints);
	L_memset(V, 0, pst_Obj->l_NbPoints);
	if(pst_Obj->l_NbUVs)
	{
		UV = (char *) L_malloc(pst_Obj->l_NbUVs);
		L_memset(UV, 0, pst_Obj->l_NbUVs);
	}
	else
		UV = NULL;

	pc_FSel = pst_Obj->pst_SubObject->dc_FSel;
	for(i = 0; i < pst_Obj->l_NbElements; i++)
	{
		i_NbFace = 0;
		pst_Triangle = pst_Obj->dst_Element[i].dst_Triangle;
		for(j = 0; j < pst_Obj->dst_Element[i].l_NbTriangles; j++, pc_FSel++, pst_Triangle++)
		{
			if(*pc_FSel & 1)
			{
				i_NbFace++;
				V[pst_Triangle->auw_Index[0]] = 1;
				V[pst_Triangle->auw_Index[1]] = 1;
				V[pst_Triangle->auw_Index[2]] = 1;

				if(UV)
				{
					UV[pst_Triangle->auw_UV[0]] = 1;
					UV[pst_Triangle->auw_UV[1]] = 1;
					UV[pst_Triangle->auw_UV[2]] = 1;
				}
			}
		}

		if(i_NbFace)
		{
			for(i_ElemIndex = 0; i_ElemIndex < pst_Data->i_NbElement; i_ElemIndex++)
			{
				if(pst_Data->i_Id[i_ElemIndex] == pst_Obj->dst_Element[i].l_MaterialId) break;
			}

			if(i_ElemIndex == pst_Data->i_NbElement)
			{
				pst_Data->i_Id[i_ElemIndex] = pst_Obj->dst_Element[i].l_MaterialId;
				pst_Data->i_NbElement++;
			}

			pst_Data->i_NbTriangle[i_ElemIndex] += i_NbFace;
		}
	}

	for(i = 0; i < pst_Obj->l_NbPoints; i++)
		if(V[i]) pst_Data->i_NbVertex++;

	if(UV)
	{
		for(i = 0; i < pst_Obj->l_NbUVs; i++)
			if(UV[i]) pst_Data->i_NbUV++;
	}

	L_free(V);
	if(UV) L_free(UV);

	if(b_ColMap) COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);

	return TRUE;
}
/**/
BOOL F3D_b_Selection_AfterDetachFace_Create(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			*pst_GO;
	GEO_tdst_Object				*pst_NewGeo, *pst_Obj;
	int							i, j;
	char						*pc_FSel;
	GEO_tdst_IndexedTriangle	*pst_Triangle, *T;
	int							i_ElemIndex;
	F3D_SubObject_Count			*pst_Data;
	int							*V, *UV;
	MATH_tdst_Matrix			M;
	BOOL						b_ColMap, b_RLI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_RLI = FALSE;
	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		b_ColMap = FALSE;
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			b_ColMap = TRUE;
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	if(!pst_Obj || !pst_Obj->pst_SubObject) return TRUE;

	pst_Data = (F3D_SubObject_Count *) p1;
	pst_NewGeo = (GEO_tdst_Object *) p2;

	// update draw mask
	pst_Data->ul_DrawMask |= pst_GO->pst_Base->pst_Visu->ul_DrawMask;

    if ( pst_GO->pst_Base->pst_Visu->dul_VertexColors )
    {
        b_RLI = TRUE;
        pst_Data->i_RLI = 1;
    }

	V = (int *) L_malloc(pst_Obj->l_NbPoints * 4);
	L_memset(V, 0, pst_Obj->l_NbPoints * 4);
	if ( pst_Obj->l_NbUVs )
	{
		UV = (int *) L_malloc(pst_Obj->l_NbUVs * 4);
		L_memset(UV, 0, pst_Obj->l_NbUVs * 4);
	}
	else 
		UV = NULL;

	pc_FSel = pst_Obj->pst_SubObject->dc_FSel;
	for(i = 0; i < pst_Obj->l_NbElements; i++)
	{
		pst_Triangle = pst_Obj->dst_Element[i].dst_Triangle;
		for(j = 0; j < pst_Obj->dst_Element[i].l_NbTriangles; j++, pc_FSel++, pst_Triangle++)
		{
			if(*pc_FSel & 1)
			{
				V[pst_Triangle->auw_Index[0]] = 1;
				V[pst_Triangle->auw_Index[1]] = 1;
				V[pst_Triangle->auw_Index[2]] = 1;
				if ( UV )
				{
					UV[pst_Triangle->auw_UV[0]] = 1;
					UV[pst_Triangle->auw_UV[1]] = 1;
					UV[pst_Triangle->auw_UV[2]] = 1;
				}
			}
		}
	}

	/* ajout des points */
	MATH_MulMatrixMatrix(&M, OBJ_pst_GetAbsoluteMatrix(pst_GO), &pst_Data->st_Invert);
	for(i = 0; i < pst_Obj->l_NbPoints; i++)
	{
		if(V[i] == 0) continue;
		V[i] = pst_Data->i_CurVertex++;
		MATH_TransformVertex(VCast(pst_NewGeo->dst_Point + V[i]), &M, VCast(pst_Obj->dst_Point + i));

        if ( b_RLI )
            pst_Data->pul_RLI[ V[i] + 1 ] = pst_GO->pst_Base->pst_Visu->dul_VertexColors[ i + 1 ];
	}

	/* ajout des UVs */
	for(i = 0; i < pst_Obj->l_NbUVs; i++)
	{
		if(UV[i] == 0) continue;
		UV[i] = pst_Data->i_CurUV++;
		pst_NewGeo->dst_UV[UV[i]].fU = pst_Obj->dst_UV[i].fU;
		pst_NewGeo->dst_UV[UV[i]].fV = pst_Obj->dst_UV[i].fV;
	}

	/* ajout des triangles */
	pc_FSel = pst_Obj->pst_SubObject->dc_FSel;
	for(i = 0; i < pst_Obj->l_NbElements; i++)
	{
		/* search good element */
		for(i_ElemIndex = 0; i_ElemIndex < pst_Data->i_NbElement; i_ElemIndex++)
		{
			if(pst_Data->i_Id[i_ElemIndex] == pst_Obj->dst_Element[i].l_MaterialId) break;
		}

		if(i_ElemIndex == pst_Data->i_NbElement) 
        {
            pc_FSel += pst_Obj->dst_Element[i].l_NbTriangles;
            continue;
        }

		pst_Triangle = pst_Obj->dst_Element[i].dst_Triangle;
		for(j = 0; j < pst_Obj->dst_Element[i].l_NbTriangles; j++, pc_FSel++, pst_Triangle++)
		{
			if(!(*pc_FSel & 1)) continue;

			T = pst_NewGeo->dst_Element[i_ElemIndex].dst_Triangle + pst_Data->i_CurTriangle[i_ElemIndex]++;
			T->auw_Index[0] = V[pst_Triangle->auw_Index[0]];
			T->auw_Index[1] = V[pst_Triangle->auw_Index[1]];
			T->auw_Index[2] = V[pst_Triangle->auw_Index[2]];
			if ( UV )
			{
				T->auw_UV[0] = UV[pst_Triangle->auw_UV[0]];
				T->auw_UV[1] = UV[pst_Triangle->auw_UV[1]];
				T->auw_UV[2] = UV[pst_Triangle->auw_UV[2]];
			}
			else 
				T->auw_UV[0] = T->auw_UV[1] = T->auw_UV[2] = 0;
		}
	}

	L_free(V);
	if ( UV ) L_free(UV);

	if(b_ColMap) COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_AfterDetachFace( MATH_tdst_Matrix *_pst_Matrix )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_SubObject_Count st_Data;
	RECT				st_Rect;
	GEO_tdst_Object		*pst_Geo;
	int					i;
	CString				o_Name;
	char				*psz_Ext, sz_Name[BIG_C_MaxLenName], sz_Path[ BIG_C_MaxLenPath ];
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Index, ul_Flags, ul_OldFlags;
    EDIA_cl_FileDialog	*po_FileDlg;
	void				*pst_Cob;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&st_Data, 0, sizeof(F3D_SubObject_Count));
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_AfterDetachFace, (ULONG) & st_Data, 0);

	pst_Cob = SEL_pst_GetFirstItem(M_F3D_Sel, SEL_C_SIF_Cob);

	/* Propose New Geometry in Gemoetric Edition, not ColMap */
	if(!pst_Cob && st_Data.i_NbVertex)
	{
		if(M_MF()->MessageBox("Create new geometry ?", "Detach face", MB_YESNO) == IDYES)
		{
			ul_Index = LOA_ul_SearchIndexWithAddress( (ULONG) mst_WinHandles.pst_World );
            BIG_ComputeFullName( BIG_ParentFile( ul_Index ), sz_Path );
            L_strcat( sz_Path, "/" );
            L_strcat( sz_Path, EDI_Csz_Path_GraphicObject );
            BIG_ul_CreateDir( sz_Path );

            po_FileDlg = new EDIA_cl_FileDialog("Choose gro file", 3, FALSE, TRUE, sz_Path, "*" EDI_Csz_ExtGraphicObject);

			if(po_FileDlg->DoModal() != IDOK) 
            {
                delete po_FileDlg;
                return;
            }

			po_FileDlg->GetItem(po_FileDlg->mo_File, 1, o_Name);
			L_strcpy(sz_Name, (char *) (LPCTSTR) o_Name);
            L_strcpy(sz_Path, po_FileDlg->masz_FullPath );
            delete po_FileDlg;

			/* check name */
			if(BIG_b_CheckName(sz_Name) == FALSE)
				return;

			/* check extension */
			psz_Ext = strrchr(sz_Name, '.');
			if(!psz_Ext)
				L_strcat(sz_Name, EDI_Csz_ExtGraphicObject);
			else if(L_stricmp(psz_Ext + 1, "gro"))
			{
				M_MF()->MessageBox("Bad file extension !!", "Error", MB_OK | MB_ICONSTOP);
				return;
			}

			/* check for existence */
			ul_Index = BIG_ul_SearchFileExt(sz_Path, sz_Name);
			if(ul_Index != BIG_C_InvalidIndex)
			{
				M_MF()->MessageBox("File already exist !!", "Warning", MB_OK | MB_ICONSTOP);
				return;
			}

			/* create waypoint */
			GetClientRect(&st_Rect);
			pst_GO = CreateWaypoint(CPoint(st_Rect.right / 2, st_Rect.bottom / 2), sz_Name );
            MATH_CopyMatrix( OBJ_pst_GetAbsoluteMatrix(pst_GO ), _pst_Matrix );
            

			/* create geometry */
			pst_Geo = GEO_pst_Create(st_Data.i_NbVertex, st_Data.i_NbUV, st_Data.i_NbElement, 0);
			GRO_Struct_SetName(&pst_Geo->st_Id, sz_Name);
			for(i = 0; i < st_Data.i_NbElement; i++)
			{
				L_memset(&pst_Geo->dst_Element[i], 0, sizeof(GEO_tdst_ElementIndexedTriangles));
				pst_Geo->dst_Element[i].l_NbTriangles = st_Data.i_NbTriangle[i];
				pst_Geo->dst_Element[i].l_MaterialId = st_Data.i_Id[i];
				GEO_AllocElementContent(&pst_Geo->dst_Element[i]);
			}

            /* create buffer for RLI */
            st_Data.pul_RLI = (ULONG *) MEM_p_Alloc( 4 * (st_Data.i_NbVertex + 1 ) );
            L_memset( st_Data.pul_RLI, 0, 4 * (st_Data.i_NbVertex + 1 ) );
            st_Data.pul_RLI[ 0 ] = st_Data.i_NbVertex;
            st_Data.i_RLI = 0;

			/* set init draw mask */
			st_Data.ul_DrawMask = 0;

			MATH_SetIdentityMatrix(&st_Data.st_Invert);
			MATH_InvertMatrix(&st_Data.st_Invert, OBJ_pst_GetAbsoluteMatrix(pst_GO));

			SEL_EnumItem
			(
				M_F3D_Sel,
				SEL_C_SIF_Object | SEL_C_SIF_Cob,
				F3D_b_Selection_AfterDetachFace_Create,
				(ULONG) & st_Data,
				(ULONG) pst_Geo
			);

			/* recompute normals */
			GEO_ComputeNormals( pst_Geo );

			/* save  geometry */
			GRO_ul_Struct_FullSave((GRO_tdst_Struct *) pst_Geo, sz_Path, sz_Name, &TEX_gst_GlobalList);

			/* attach geometry to Gameobject and world */
			ul_OldFlags = pst_GO->ul_IdentityFlags;
			ul_Flags = ul_OldFlags | OBJ_C_IdentityFlag_BaseObject | OBJ_C_IdentityFlag_Visu;
            ul_Flags &= ~OBJ_C_IdentityFlag_Waypoints;
			OBJ_ChangeIdentityFlags(pst_GO, ul_Flags, ul_OldFlags);

			/* set RLI */
            if (st_Data.i_RLI)
                pst_GO->pst_Base->pst_Visu->dul_VertexColors = st_Data.pul_RLI;
            else
                MEM_Free( st_Data.pul_RLI );

			/* set visu */
			pst_GO->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct *) pst_Geo;
			pst_GO->pst_Base->pst_Visu->ul_DrawMask = st_Data.ul_DrawMask;

			/* add created object into world table */
			TAB_Ptable_AddElemAndResize(&mst_WinHandles.pst_World->st_GraphicObjectsTable, pst_Geo);
			pst_Geo->st_Id.l_Ref += 2;

            /* recompute BV */
            OBJ_ComputeBV(pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_AABBox);

			/* set material */
            if (st_Data.pst_Mat)
            {
                pst_GO->pst_Base->pst_Visu->pst_Material = (GRO_tdst_Struct *) st_Data.pst_Mat;
                st_Data.pst_Mat->st_Id.l_Ref++;
            }

			/* delete selected triangles */
            Selection_SubObject_DelFace();
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Duplicate selected faces
 =======================================================================================================================
 */
BOOL F3D_b_Selection_DuplicateFace(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	BOOL				b_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		b_ColMap = FALSE;
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			b_ColMap = TRUE;
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	if(pst_Obj) GEO_SubObject_FDuplicate(pst_Obj);

	if(b_ColMap) COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);

    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
        OBJ_ComputeBV( pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);

	/* Rebuild Visual OK3 */
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		COL_OK3_Build(pst_Obj, FALSE, TRUE);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_DuplicateFace(void)
{
	if(!Selection_b_IsInSubObjectMode()) return;

	mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this);
	mpo_CurrentModif->SetDesc("Copy face");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_DuplicateFace, 0, (ULONG) M_F3D_DD);

#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif
}

/*
 =======================================================================================================================
    Aim:    Extrude selected faces
 =======================================================================================================================
 */
BOOL F3D_b_Selection_ExtrudeFace(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	BOOL				b_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		b_ColMap = FALSE;
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			b_ColMap = TRUE;
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	if(pst_Obj) GEO_SubObject_FExtrude(pst_Obj);

	if(b_ColMap) COL_SynchronizeCob((COL_tdst_Cob *) _pst_Sel->p_Content, FALSE,FALSE);

    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
        OBJ_ComputeBV( pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);

	/* Rebuild Visual OK3 */
    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		COL_OK3_Build(pst_Obj, FALSE, TRUE);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_ExtrudeFace(void)
{
	if(!Selection_b_IsInSubObjectMode()) return;

	mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this);
	mpo_CurrentModif->SetDesc("Extrude face");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_ExtrudeFace, 0, (ULONG) M_F3D_DD);
#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, FALSE);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::Selection_b_SubObject_CutFace
(
	OBJ_tdst_GameObject *_pst_GO,
	ULONG				_ul_ElemFace,
	POINT				*pst_Point,
	BOOL				_b_ColMap
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_Object		*pst_Obj;
	int					e, t;
	MATH_tdst_Vector	*A, B, C;
	MATH_tdst_Matrix	*M;
	int					res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_b_ColMap)
	{
		if
		(
			_pst_GO
		&&	_pst_GO->pst_Extended
		&&	_pst_GO->pst_Extended->pst_Col
		&&	((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap
		)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/
			COL_tdst_ColMap *pst_ColMap;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;

			if((pst_ColMap->dpst_Cob[0]) && (pst_ColMap->dpst_Cob[0]->uc_Type == COL_C_Zone_Triangles))
			{
				if(pst_ColMap->dpst_Cob[0]->p_GeoCob)
				{
					pst_ColMap->dpst_Cob[0]->uc_Flag |= COL_C_Cob_Updated;
					pst_Obj = (GEO_tdst_Object *) (pst_ColMap->dpst_Cob[0]->p_GeoCob);
				}
				else
					return FALSE;
			}
			else
				return FALSE;
		}
		else
			return FALSE;
	}
	else
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(_pst_GO);
	if(!pst_Obj) return FALSE;
	if(!pst_Obj->pst_SubObject) return FALSE;

	e = (_ul_ElemFace & SOFT_Cul_PBQF_ElementMask) >> SOFT_Cul_PBQF_ElementShift;
	t = (_ul_ElemFace & SOFT_Cul_PBQF_TriangleMask) >> SOFT_Cul_PBQF_TriangleShift;
	M = _pst_GO->pst_GlobalMatrix;
	A = &M_F3D_DD->st_Camera.st_Matrix.T;
	MATH_InitVector(&C, (float) pst_Point->x, (float) pst_Point->y, 1);
	CAM_2Dto3D(&M_F3D_DD->st_Camera, &B, &C);

	mpo_CurrentModif = new F3D_cl_Undo_GeoModif(this, pst_Obj);
	mpo_CurrentModif->SetDesc("Cut face");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;

	res = GEO_i_SubObject_FCut(pst_Obj, M, A, &B, e, t);;

	if(_b_ColMap) 
	{
		COL_SynchronizeCob(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0], FALSE,FALSE);
		COL_UpdateCameraNumberOfFaces(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]);
	}

	return res;
}

/*
 =======================================================================================================================
    Aim:    change order of a face
 =======================================================================================================================
 */
void F3D_cl_View::Selection_SubObject_FaceOrder()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			*pst_GO;
	GEO_tdst_Object				*pst_Obj;
	int							i, face, elem, newface;
	EDIA_cl_NameDialog			o_Order("");
	SEL_tdst_SelectedItem		*pst_Sel;
	char						sz_Text[ 64 ];
	GEO_tdst_IndexedTriangle	st_Triangle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!Selection_b_IsInSubObjectMode()) return;

	pst_Sel = M_F3D_Sel->pst_FirstItem;

	if ( pst_Sel->pst_Next )
	{
		M_MF()->MessageBox( "Select only one geometry", "Error", MB_OK );
		return;
	}

	if( !(pst_Sel->l_Flag & SEL_C_SIF_Object) )
	{
		M_MF()->MessageBox( "Select one geometry", "Error", MB_OK );
		return;
	}

	pst_GO = (OBJ_tdst_GameObject *) pst_Sel->p_Content;
	pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);

	if (!pst_Obj) 
	{
		M_MF()->MessageBox( "Select a geometry", "Error", MB_OK );
		return;
	}

	i = GEO_l_SubObject_FNbSel( pst_Obj );
	if (i != 1 )
	{
		M_MF()->MessageBox( "Select only one face", "Error", MB_OK );
		return;
	}

	// find sel face
	for ( face = 0; face < (int) pst_Obj->pst_SubObject->ul_NbFaces; face++ )
	{
		if (pst_Obj->pst_SubObject->dc_FSel[ face ] & 1)
			break;
	}

	if (face == (int) pst_Obj->pst_SubObject->ul_NbFaces )
		return;

	elem = pst_Obj->pst_SubObject->dst_Faces[ face ].uw_Element;
	face = pst_Obj->pst_SubObject->dst_Faces[ face ].uw_Index;

	sprintf( sz_Text, "order face (elem = %d, face = %d)", elem, face );
	o_Order.mo_Title = sz_Text;
	if ( o_Order.DoModal() != IDOK ) return;

	newface = atoi( (char *) (LPCTSTR) o_Order.mo_Name );
	if (newface >= pst_Obj->dst_Element[ elem ].l_NbTriangles )
	{
		sprintf( sz_Text, "Index of face out of limit (%d)", pst_Obj->dst_Element[ elem ].l_NbTriangles - 1);
		M_MF()->MessageBox( sz_Text, "Error", MB_OK );
		return;
	}

	L_memcpy ( &st_Triangle, &pst_Obj->dst_Element[ elem ].dst_Triangle[ face ], sizeof(GEO_tdst_IndexedTriangle) );
	L_memcpy ( &pst_Obj->dst_Element[ elem ].dst_Triangle[ face ], &pst_Obj->dst_Element[ elem ].dst_Triangle[ newface ], sizeof(GEO_tdst_IndexedTriangle) );
	L_memcpy ( &pst_Obj->dst_Element[ elem ].dst_Triangle[ newface ], &st_Triangle, sizeof(GEO_tdst_IndexedTriangle) );
}

/*
 =======================================================================================================================
    traite les outils sous objet qui ne demande pas d'avoir des sous objets sélectionnables 
        ex : cut face, edge cutter )
 =======================================================================================================================
 */
int F3D_cl_View::Selection_b_SubObject_TreatAnyway(POINT *pst_Point)
{
    SOFT_tdst_PickingBuffer			*pst_PB;
    SOFT_tdst_PickingBuffer_Query	*pst_Query;
    SOFT_tdst_PickingBuffer_Pixel	*pst_Pixel;
    GEO_tdst_GraphicZone	        *pst_Zone;

    if((M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Edge) && (M_F3D_EditOpt->i_EdgeTool == GRO_i_EOT_EdgeCutter))
    {
        mb_SubObject_LinkOn = TRUE;
        Selection_SubObject_Link( pst_Point, TRUE, FALSE );
        return TRUE;
    }

    if((M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Face) && (M_F3D_EditOpt->i_FaceTool == GRO_i_EOT_FaceCut))
    {
		if(Pick_l_UnderPoint(pst_Point, SOFT_Cuc_PBQF_GameObject, 0))
		{
			pst_PB = mst_WinHandles.pst_DisplayData->pst_PickingBuffer;
			pst_Query = &pst_PB->st_Query;
			pst_Pixel = pst_Query->dst_List;
			if
			(
				Selection_b_SubObject_CutFace
				(
					(OBJ_tdst_GameObject *) pst_Pixel->ul_Value,
					pst_Pixel->ul_ValueExt,
					pst_Point,
					FALSE
				)
			)
			{
				LINK_Refresh();
				return TRUE;
			}
		}
		else if(Pick_l_UnderPoint(pst_Point, SOFT_Cuc_PBQF_Zone, 0))
        {
		    pst_PB = mst_WinHandles.pst_DisplayData->pst_PickingBuffer;
			pst_Query = &pst_PB->st_Query;
			pst_Pixel = pst_Query->dst_List;

			pst_Zone = mst_WinHandles.pst_DisplayData->st_DisplayedZones.dpst_GraphicZone + (ULONG) pst_Pixel->ul_Value;

			if(pst_Zone->uc_Type == GEO_Cul_GraphicCob)
			{
			    if(Selection_b_SubObject_CutFace(pst_Zone->pst_GO, pst_Pixel->ul_ValueExt, pst_Point, TRUE))
				{
				    LINK_Refresh();
					return TRUE;
                }
            }
		}
	}
    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_SubObjectTreat(POINT *pst_Point)
{
	if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Vertex)
	{
		switch(M_F3D_EditOpt->i_VertexTool)
		{
		case GRO_i_EOT_VertexPaint:
			Selection_ColorVertices(M_F3D_EditOpt->ul_VertexPaintColor, M_F3D_EditOpt->f_VertexBlendColor, FALSE);
			break;
		case GRO_i_EOT_VertexPaintAlpha:
			Selection_ColorVertices(M_F3D_EditOpt->ul_VertexPaintColor, M_F3D_EditOpt->f_VertexBlendColor, TRUE);
			break;
		case GRO_i_EOT_VertexPickColor:
			Selection_PickVertexColor();
			break;
		case GRO_i_EOT_VertexWeld:
			break;
        case GRO_i_EOT_VertexBuildFace:
            mb_SubObject_LinkOn = TRUE;
            Selection_SubObject_Link( pst_Point, TRUE, FALSE );
            break;
		}
	}

	else if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Edge)
	{
		switch(M_F3D_EditOpt->i_EdgeTool)
		{
		case GRO_i_EOT_EdgeCut:
			Selection_CutEdge(pst_Point);
			break;
		case GRO_i_EOT_EdgeTurn:
			Selection_TurnEdge();
			break;
		}
	}

	else if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Face)
	{
        /*
		switch(M_F3D_EditOpt->i_FaceTool)
		{
	    }
        */
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::Selection_b_SubObjectTreat_OnLButtonUp(POINT *_pst_Pt)
{
	if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Vertex)
	{
		switch(M_F3D_EditOpt->i_VertexTool)
		{
		case GRO_i_EOT_VertexWeld:
			if(Selection_b_SubObject_VertexWeld(_pst_Pt))
			{
				delete mpo_CurrentModif;
				mpo_CurrentModif = NULL;
				return 1;
			}
			break;
		}
	}
    else if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Edge)
	{
		switch(M_F3D_EditOpt->i_EdgeTool)
		{
        case GRO_i_EOT_EdgeCutter:
            break;
        }
    }
	
	return 0;
}

/*
 =======================================================================================================================
    Constantes pour le picking des faces
 =======================================================================================================================
 */
static SOFT_tdst_PickingBuffer *F3D_spst_PB;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_SubObject_VPick(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		*v;
	float					f;
	OBJ_tdst_GameObject		*pst_GO;
	GEO_tdst_Object			*pst_Obj;
	F3D_SubObject_PickData	*pst_Data;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	v = (MATH_tdst_Vector *) p1;
	pst_Data = (F3D_SubObject_PickData *) p2;

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}


	i = GEO_i_SubObject_VPick(pst_Obj, v, pst_Data->c_Sel, pst_GO);

	if(i == -1) return TRUE;

	f = pst_Obj->pst_SubObject->dst_2D[i].z;
	if(fOptInv(f) < fOptInv(pst_Data->f_Dist))
	{
		pst_Data->f_Dist = f;
		pst_Data->i_Sel = i;
		pst_Data->pst_Obj = pst_Obj;
		pst_Data->pst_GAO = pst_GO;
	}

	return TRUE;
}
/**/
BOOL F3D_b_SubObject_EPick(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		*v;
	float					f;
	OBJ_tdst_GameObject		*pst_GO;
	GEO_tdst_Object			*pst_Obj;
	F3D_SubObject_PickData	*pst_Data;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	v = (MATH_tdst_Vector *) p1;
	pst_Data = (F3D_SubObject_PickData *) p2;

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	f = pst_Data->f_Dist;

	i = GEO_i_SubObject_EPick(pst_Obj, &pst_Data->P0, &pst_Data->P1, &pst_Data->f_Dist, (_pst_Sel->l_Flag & SEL_C_SIF_Cob) ? pst_GO : NULL);
	

	if ( (i!=-1) && (fOptInv(f) > fOptInv(pst_Data->f_Dist)) )
	{
		pst_Data->i_Sel = i;
		pst_Data->pst_Obj = pst_Obj;
	}
	else
		pst_Data->f_Dist = f;

	return TRUE;
}
/**/
BOOL F3D_b_SubObject_FPick(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		*v;
	float					f;
	OBJ_tdst_GameObject		*pst_GO;
	GEO_tdst_Object			*pst_Obj;
	F3D_SubObject_PickData	*pst_Data;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	v = (MATH_tdst_Vector *) p1;
	pst_Data = (F3D_SubObject_PickData *) p2;

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

    /*$F
    if (F3D_spst_PB)
        i = GEO_i_SubObject_FPickBoxWithPB(pst_Obj, v, NULL, F3D_i_LockedId, pst_GO, F3D_spst_PB);
    else
    */
    	i = GEO_i_SubObject_FPick(pst_Obj, v, pst_Data->c_Sel, pst_Data->i_Id, pst_Data->c_Backface );

	if(i == -1) return TRUE;

	f = pst_Obj->pst_SubObject->dst_2D[i].z;
	if(fOptInv(f) < fOptInv(pst_Data->f_Dist))
	{
		pst_Data->f_Dist = f;
		pst_Data->i_Sel = i;
		pst_Data->pst_Obj = pst_Obj;
	}

	return TRUE;
}
/**/
BOOL F3D_cl_View::Selection_b_SubObject_Pick(MATH_tdst_Vector *v, char _c_Sel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_SubObject_PickData	st_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!Selection_b_IsInSubObjectMode()) return FALSE;

	st_Data.f_Dist = 0.00000001f;
	st_Data.i_Sel = -1;
	st_Data.c_Sel = _c_Sel;

	if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Vertex)
	{
		if (!(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_SelectVisible))
		if ((mst_WinHandles.pst_DisplayData->ul_WiredMode & 3) != 2)
		{
			if(mst_WinHandles.pst_World != NULL)
			{
				MATH_tdst_Vector	vL;
				POINT				pointJJ;
				GetCursorPos(&pointJJ);
				ScreenToClient(&pointJJ);
				vL.x = (float) pointJJ.x / mst_WinHandles.pst_DisplayData->st_Device.l_Width;
				vL.y = 1.0f - ((float) pointJJ.y / mst_WinHandles.pst_DisplayData->st_Device.l_Height);
				b_PickDepth(&vL, &vL.z);
			}
			pPickForZCull = mst_WinHandles.pst_DisplayData->pst_PickingBuffer;
		};//*/
		SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_SubObject_VPick, (ULONG) v, (ULONG) & st_Data);
		pPickForZCull  = NULL;
		if(st_Data.i_Sel == -1) 
			return FALSE;
		st_Data.pst_Obj->pst_SubObject->dc_VSel[st_Data.i_Sel] |= 2;
		
		mpst_LockCenter_GAO = st_Data.pst_GAO;
		mi_LockCenter_Point = st_Data.i_Sel;
	}
	else if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Edge)
	{
		MATH_InitVector(&st_Data.P0, (float) v->x, (float) v->y, 1);

		/* CAM_2Dto3D( &M_F3D_DD->st_Camera, &st_Data.P1, &st_Data.P0); */
		CAM_2Dto3DCamera(&M_F3D_DD->st_Camera, &st_Data.P1, &st_Data.P0);

		/* MATH_CopyVector( &st_Data.P0, &M_F3D_DD->st_Camera.st_Matrix.T ); */
        if(M_F3D_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
		    MATH_InitVectorToZero(&st_Data.P0);
        else
            MATH_SubVector( &st_Data.P0, &st_Data.P1, &MATH_gst_BaseVectorK );

		SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_SubObject_EPick, (ULONG) v, (ULONG) & st_Data);
		if(st_Data.i_Sel == -1) return FALSE;
		st_Data.pst_Obj->pst_SubObject->dc_ESel[st_Data.i_Sel] |= 2;
	}
	else if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Face)
	{
        /*$F ca plante : trop de refresh
        F3D_spst_PB = NULL;
        if ( M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_FacePickingBufferTest ) 
        {
            POINT pt1, pt2;

            F3D_spst_PB = mst_WinHandles.pst_DisplayData->pst_PickingBuffer;
            pt1.x = (int) v->x - 1;
            pt1.y = (F3D_spst_PB->l_Height - (int) v->y) - 1;
            pt2.x = (int) v->x + 1;
            pt2.y = (F3D_spst_PB->l_Height - (int) v->y) + 1;

            Pick_l_InBox( &pt1, &pt2, 0 );
        }
        */

        st_Data.c_Backface = (M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_FaceBackfaceTest ) ? 1 : 0;
		st_Data.i_Id = (M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_FaceLockId) ? M_F3D_EditOpt->l_FaceId : -1;
		SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_SubObject_FPick, (ULONG) v, (ULONG) & st_Data);
		if(st_Data.i_Sel == -1) return FALSE;
		st_Data.pst_Obj->pst_SubObject->dc_FSel[st_Data.i_Sel] |= 2;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

BOOL F3D_b_SubObject_PickBox(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	**v;
	int					*n;
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
    char                c_Backface;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	v = (MATH_tdst_Vector **) p1;
	n = (int *) p2;

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
		} 
		else
			return TRUE;
	}

	if(F3D_ul_SubObject_SelType & GRO_Cul_EOF_Vertex)
		*n += GEO_i_SubObject_VPickBox(pst_Obj, v[0], v[1], (_pst_Sel->l_Flag & SEL_C_SIF_Cob) ? pst_GO : NULL);
	else if(F3D_ul_SubObject_SelType & GRO_Cul_EOF_Edge)
		*n += GEO_i_SubObject_EPickBox(pst_Obj, v[0], v[1], F3D_sb_WindowSel ? 1 : 0, (_pst_Sel->l_Flag & SEL_C_SIF_Cob) ? pst_GO : NULL);
	else if(F3D_ul_SubObject_SelType & GRO_Cul_EOF_Face)
    {
        if (F3D_spst_PB)
            *n += GEO_i_SubObject_FPickBoxWithPB(pst_Obj, v[0], v[1], F3D_i_LockedId, pst_GO, F3D_spst_PB);
        else
        {
            c_Backface = (F3D_ul_SubObject_SelType & GRO_Cul_EOF_FaceBackfaceTest) ? 1 : 0;
		    *n += GEO_i_SubObject_FPickBox(pst_Obj, v[0], v[1], F3D_i_LockedId, c_Backface, (_pst_Sel->l_Flag & SEL_C_SIF_Cob) ? pst_GO : NULL );
        }
    }
	return TRUE;
}
/**/
BOOL F3D_cl_View::Selection_b_SubObject_PickBox(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	float				f;
	MATH_tdst_Vector	*v[2];
	int					n;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!Selection_b_IsInSubObjectMode()) return FALSE;
	v[0] = A;
	v[1] = B;
	if(A->x > B->x)
	{
		f = A->x;
		A->x = B->x;
		B->x = f;
	}

	if(A->y > B->y)
	{
		f = A->y;
		A->y = B->y;
		B->y = f;
	}

    F3D_spst_PB = NULL;
    if ( (M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Face) && (M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_FacePickingBufferTest) ) 
    {
        POINT pt1, pt2;

        F3D_spst_PB = mst_WinHandles.pst_DisplayData->pst_PickingBuffer;
        pt1.x = (int) A->x;
        pt1.y = F3D_spst_PB->l_Height - (int) B->y;
        pt2.x = (int) B->x;
        pt2.y = F3D_spst_PB->l_Height - (int) A->y;

        Pick_l_InBox( &pt1, &pt2, 0 );
    }

	F3D_ul_SubObject_SelType = M_F3D_EditOpt->ul_Flags;
	F3D_sb_WindowSel = (M_F3D_PB->ul_UserFlags & SOFT_Cul_PBF_WindowSelection) == SOFT_Cul_PBF_WindowSelection;
	n = 0;
	F3D_i_LockedId = (M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_FaceLockId) ? M_F3D_EditOpt->l_FaceId : -1;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_SubObject_PickBox, (ULONG) v, (ULONG) & n);
	return(n != 0);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_SubObject_CannotSelAll(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	GEO_SubObject_CannotSelAll(pst_Obj);
	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_CannotSelAll(void)
{
	if(!Selection_b_IsInSubObjectMode()) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_SubObject_CannotSelAll, 0, 0);
}

static F3D_cl_Undo_SelSubObject *F3D_spo_CurrentModif;
static int						F3D_i_IDInfo;
static GRO_tdst_Struct			*F3D_pst_GroForIDInfo;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#pragma optimize ("", off)

BOOL F3D_b_SubObject_Sel(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	char				*pc_Sel, *pc_Last;
	LONG				l_Number;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!p1 && !p2)
	{
		pst_Obj = (GEO_tdst_Object *) _pst_Sel;
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		{
			pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
			pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
		{
			if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
			{
				pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
				if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
					pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
				else
					pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
			}
			else
				return TRUE;
		}

		if(!pst_Obj || !pst_Obj->pst_SubObject) return TRUE;

		if(F3D_spo_CurrentModif) F3D_spo_CurrentModif->BeginAddObj(pst_Obj);

		pc_Sel = GEO_pc_SubObject_GetSelData(pst_Obj, F3D_ul_SubObject_SelType, &l_Number);
		pc_Last = pc_Sel + l_Number;

		if((p1 == 0) && (p2 == 1))
		{
			for(; pc_Sel < pc_Last; pc_Sel++) *pc_Sel = (*pc_Sel & 0x82) | ((*pc_Sel & 2) >> 1);
		}
		else if((p1 == 1) && (p2 == 1))
		{
			for(; pc_Sel < pc_Last; pc_Sel++) *pc_Sel |= ((*pc_Sel & 2) >> 1);
		}
		else if((p1 == 1) && (p2 == 0))
		{
			for(; pc_Sel < pc_Last; pc_Sel++)
				if((*pc_Sel & 3) == 3) *pc_Sel = (*pc_Sel & 0x80) | 2;
		}
	}

	if(F3D_ul_SubObject_SelType & GRO_Cul_EOF_Edge)
	{
        GEO_SubObject_EUpdateVSel( pst_Obj );
	}

	if(F3D_ul_SubObject_SelType & GRO_Cul_EOF_Face)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		int							i;
		GEO_tdst_SubObject_Face		*pst_Face;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(F3D_pst_GroForIDInfo == (GRO_tdst_Struct *) pst_Obj)
		{
			pc_Sel = pst_Obj->pst_SubObject->dc_FSel;
			pc_Last = pc_Sel + pst_Obj->pst_SubObject->ul_NbFaces;
			for(i = 0; pc_Sel < pc_Last; pc_Sel++, i++)
			{
				if(*pc_Sel & 3)
				{
					pst_Face = pst_Obj->pst_SubObject->dst_Faces + i;
					F3D_i_IDInfo = pst_Obj->dst_Element[pst_Face->uw_Element].l_MaterialId;
                    break;
				}
			}
		}

        GEO_SubObject_FUpdateVSel( pst_Obj );

		/* inform eventually mapper that face has changed */
		if(pst_Obj->pst_SubObject->pfn_UVMapper_Update)
			pst_Obj->pst_SubObject->pfn_UVMapper_Update(pst_Obj->pst_SubObject->ul_UVMapper_Param);
	}

	GEO_SKN_UpdateSkinSelection(pst_Obj);

	if(F3D_spo_CurrentModif) F3D_spo_CurrentModif->EndAddObj(pst_Obj);

	return TRUE;
}

#pragma optimize ("", on)

/**/
void F3D_cl_View::Selection_SubObject_Sel(char _c_Sel, char _c_CanSel)
{
	if(!Selection_b_IsInSubObjectMode()) return;
	F3D_ul_SubObject_SelType = M_F3D_EditOpt->ul_Flags;
	mpo_CurrentModif = F3D_spo_CurrentModif = new F3D_cl_Undo_SelSubObject(this, F3D_ul_SubObject_SelType);
	mpo_CurrentModif->SetDesc("Sub object selection");

	F3D_i_IDInfo = -1;
	F3D_pst_GroForIDInfo = (mpo_ToolBoxDialog) ? mpo_ToolBoxDialog->mpst_Gro : NULL;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_SubObject_Sel, (ULONG) _c_Sel, (ULONG) _c_CanSel);
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = F3D_spo_CurrentModif = NULL;

	if(F3D_i_IDInfo != -1)
	{
		if(mpo_ToolBoxDialog) mpo_ToolBoxDialog->UpdateFaceId(F3D_i_IDInfo);
		M_F3D_EditOpt->l_FaceId = F3D_i_IDInfo;
	}

}

/*
 =======================================================================================================================
    Aim:    swap from one mode to another so select vertex according to mode change
 =======================================================================================================================
 */
BOOL F3D_b_SubObject_SelForEdge(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	GEO_SubObject_VUpdateESel(pst_Obj);
	GEO_SubObject_EUpdateVSel(pst_Obj);
	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_SelForEdge(void)
{
	if(!Selection_b_IsInSubObjectMode()) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object, F3D_b_SubObject_SelForEdge, 0, 0);
}

/*
 =======================================================================================================================
    Aim:    swap from one mode to another so select vertex according to mode change
 =======================================================================================================================
 */
BOOL F3D_b_SubObject_SelForFace(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	GEO_SubObject_VUpdateFSel(pst_Obj);
	GEO_SubObject_FUpdateVSel(pst_Obj);
	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_SelForFace(void)
{
	if(!Selection_b_IsInSubObjectMode()) return;
	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_SubObject_SelForFace, 0, 0);
}

/*
 =======================================================================================================================
    Aim:    prepare moving vertex
 =======================================================================================================================
 */
BOOL F3D_b_Selection_SubObject_StartMovingVertex(SEL_tdst_SelectedItem *_pst_Sel, ULONG _i_UpdUV, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !(_pst_Sel->l_Flag & SEL_C_SIF_Object) ) return TRUE;
	
	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);

    GEO_SubObject_InitVMoves( pst_Obj, _i_UpdUV );
	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_StartMovingVertex( int _i_UpdUV )
{
	if(!Selection_b_IsInSubObjectMode()) return;
    SEL_EnumItem( M_F3D_Sel, SEL_C_SIF_Object, F3D_b_Selection_SubObject_StartMovingVertex, _i_UpdUV, 0 );
}

/*
 =======================================================================================================================
    Aim:    Move all selected vertices
 =======================================================================================================================
 */

static MATH_tdst_Vector F3D_b_Selection_SubObjectMove_Move;
/**/
BOOL F3D_b_Selection_SubObjectMove(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			*pst_GO;
	GEO_tdst_Object				*pst_Obj;
	MATH_tdst_Matrix			M;
	MATH_tdst_Vector			*v, v_local; //, v_morph;
    BOOL						b_Pivot, b_Pos;
	//GEO_tdst_ModifierMorphing	*pst_Morph;
	//int							i_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_Pivot = (p2 & 1) != 0;
    b_Pos = (p2 & 2) != 0;

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	v = (MATH_tdst_Vector *) p1;
    MATH_InvertMatrix(&M, OBJ_pst_GetAbsoluteMatrix(pst_GO));

	if (b_Pos && pst_Obj->pst_SubObject->pst_MorphData)
	{
		b_Pos = 0;
		v = &F3D_b_Selection_SubObjectMove_Move;
	}

    if ( b_Pos )
    {
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			MATH_tdst_Matrix	M2;

			MATH_CopyMatrix(&M2, pst_GO->pst_GlobalMatrix);
			MATH_ClearScale(&M2, 1);
			MATH_InvertMatrix(&M, &M2);

			MATH_TransformVertex(&v_local, &M, v);
		}
		else
			MATH_TransformVertex(&v_local, &M, v);

		/*
		if (pst_Morph = GEO_pst_ModifierMorphing_Get( pst_GO))
		{
			if ( !GEO_l_SubObject_GetNbPickedVerticesAndOneIndex(pst_Obj, &i_Index ) )
				return TRUE;
			GEO_ModifierMorphing_GetVertexMorphVector( pst_Morph, i_Index, &v_morph );
			MATH_SubEqualVector( &v_local, &v_morph );
			GEO_SubObject_MoveVertices(pst_Obj, &v_local, (char) b_Pivot);
			return TRUE;
		}
		*/
		if ( GEO_l_SubObject_VSet( pst_Obj, &v_local ) )
			return FALSE;
		else
			return TRUE;
    }

    MATH_TransformVectorNoScale(&v_local, &M, v);
	if(MATH_b_TestScaleType(&M) && (_pst_Sel->l_Flag & SEL_C_SIF_Object))
	{
		v_local.x *= M.Sx;
		v_local.y *= M.Sy;
		v_local.z *= M.Sz;
	}

	if(b_Pivot)
	{
		MATH_NegEqualVector(&v_local);
		MATH_AddEqualVector(OBJ_pst_GetAbsolutePosition(pst_GO), v);
		OBJ_ComputeLocalWhenHie(pst_GO);
	}

	GEO_SubObject_MoveVertices(pst_Obj, &v_local, (char) b_Pivot);

	/*
	 * If we are editing the Pivot of one GameObject, we have to also move to vertices
	 * of its Triangles-based Cob if it has one.
	 */
	if(b_Pivot && (_pst_Sel->l_Flag & SEL_C_SIF_Object) )
	{
		if (pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)
		{
			/*~~~~~~~~~~~~~~~~~~~~~*/
			COL_tdst_Cob	*pst_Cob;
			/*~~~~~~~~~~~~~~~~~~~~~*/

			pst_Cob = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0];

			if((pst_Cob) && (pst_Cob->uc_Type == COL_C_Zone_Triangles))
			{
				if(!pst_Cob->p_GeoCob)
				{
					GEO_tdst_Object			*pst_GeoCob;

					pst_Cob->p_GeoCob = MEM_p_Alloc(sizeof(GEO_tdst_Object));
					pst_GeoCob = (GEO_tdst_Object *) pst_Cob->p_GeoCob;

					/* Fill the st_GeoCob structure with info needed to see the Cob. */
					L_memset(pst_GeoCob, 0, sizeof(GEO_tdst_Object));

					pst_GeoCob->st_Id.i = &GRO_gast_Interface[GRO_Geometric];
					GEO_CreateGeoFromCob(pst_GO, pst_GeoCob, pst_Cob);
				}

				if(!(((GEO_tdst_Object *) pst_Cob->p_GeoCob)->pst_SubObject))
					GEO_SubObject_Create((GEO_tdst_Object *) pst_Cob->p_GeoCob);
				GEO_SubObject_MoveVertices((GEO_tdst_Object *) pst_Cob->p_GeoCob, &v_local, (char) b_Pivot);
				pst_Cob->uc_Flag |= COL_C_Cob_Updated;
			}
		}
		
		if ( OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_EditableBV) )
		{
			F3D_b_MoveBVCameraAxis( _pst_Sel, (ULONG) v, 0);
		}
	}

    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
        OBJ_ComputeBV( pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObjectMove(MATH_tdst_Vector *_pst_Move, POINT *_pst_Pt, BOOL _b_Pivot, BOOL _b_3DWorld )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*M;
	MATH_tdst_Vector	v, center;
	char				axe;
    BOOL                b_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!Selection_b_IsInSubObjectMode()) return;

	if ( _b_3DWorld )
		MATH_CopyVector( &v, _pst_Move );
	else
	{
		M = &M_F3D_DD->st_Camera.st_Matrix;
		MATH_TransformVector(&v, M, _pst_Move);
	}
		
	b_Pos = FALSE;

	if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_SnapVertex || GetAsyncKeyState('S') < 0)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		F3D_SubObject_PickData	st_Data;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		center.x = (float) _pst_Pt->x;
		center.y = (float) M_F3D_PB->l_Height - _pst_Pt->y;

		if (!(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_SelectVisible))
		if ((mst_WinHandles.pst_DisplayData->ul_WiredMode & 3) != 2)
		{
			if(mst_WinHandles.pst_World != NULL)
			{
				MATH_tdst_Vector	vL;
				vL.x = (float) center.x / mst_WinHandles.pst_DisplayData->st_Device.l_Width;
				vL.y = (float) center.y / mst_WinHandles.pst_DisplayData->st_Device.l_Height;
				b_PickDepth(&vL, &vL.z);
			}
			pPickForZCull = mst_WinHandles.pst_DisplayData->pst_PickingBuffer;
		};//*/
		st_Data.f_Dist = 0;
		st_Data.c_Sel = 0;
		st_Data.i_Sel = -1;
		SEL_EnumItem
		(
			M_F3D_Sel,
			SEL_C_SIF_Object | SEL_C_SIF_Cob,
			F3D_b_SubObject_VPick,
			(ULONG) & center,
			(ULONG) & st_Data
		);
		pPickForZCull = NULL;

		if(st_Data.i_Sel != -1)
		{
			MATH_CopyVector(&center, &M_F3D_Helpers->st_GlobalMatrix.T);
			MATH_TransformVertex(&v, M, st_Data.pst_Obj->pst_SubObject->dst_3D + st_Data.i_Sel);

			axe = SOFT_c_Helper_GetUsedSystemAxis(M_F3D_Helpers);
			if(axe == SOFT_Cul_HF_SAWorld)
			{
				axe = SOFT_c_Helper_GetUsedAxis(M_F3D_Helpers);
				if(!(axe & 1)) v.x = M_F3D_Helpers->st_StartPos.x;
				if(!(axe & 2)) v.y = M_F3D_Helpers->st_StartPos.y;
				if(!(axe & 4)) v.z = M_F3D_Helpers->st_StartPos.z;
			}

            if (M_F3D_Helpers->l_NbVertices == 1)
			{
                b_Pos = 1;
				MATH_SubVector( &F3D_b_Selection_SubObjectMove_Move, &v, &center );
			}
            else
                MATH_SubEqualVector(&v, &center);
		}
	}

	/* snap to grid */
	else if (M_F3D_Helpers->ul_Flags & SOFT_Cul_HF_SnapGrid)
	{
		MATH_TransformVertex(&center, M, &M_F3D_Helpers->st_Center);
		MATH_AddEqualVector(&v, &center);

		
		/*if ((mst_WinHandles.pst_DisplayData->pst_Helpers->l_Picked == SOFT_Cl_Helpers_MoveGlobalX) ||
			(mst_WinHandles.pst_DisplayData->pst_Helpers->l_Picked == SOFT_Cl_Helpers_MoveLocalX))
			v.x = MATH_f_FloatModulo(v.x, M_F3D_Helpers->f_GridSize);
		else
		if ((mst_WinHandles.pst_DisplayData->pst_Helpers->l_Picked == SOFT_Cl_Helpers_MoveGlobalY)||
			(mst_WinHandles.pst_DisplayData->pst_Helpers->l_Picked == SOFT_Cl_Helpers_MoveLocalY))
			v.y = MATH_f_FloatModulo(v.y, M_F3D_Helpers->f_GridSize);
		else
		if ((mst_WinHandles.pst_DisplayData->pst_Helpers->l_Picked == SOFT_Cl_Helpers_MoveGlobalZ)||
			(mst_WinHandles.pst_DisplayData->pst_Helpers->l_Picked == SOFT_Cl_Helpers_MoveLocalZ))
			v.z = MATH_f_FloatModulo(v.z, M_F3D_Helpers->f_GridSize);
		else*/
		{
            if (M_F3D_Helpers->f_GridSizeXY)
            {
    			v.x = MATH_f_FloatModulo(v.x, M_F3D_Helpers->f_GridSizeXY);
	    		v.y = MATH_f_FloatModulo(v.y, M_F3D_Helpers->f_GridSizeXY);
            }
            if (M_F3D_Helpers->f_GridSizeZ) 
    			v.z = MATH_f_FloatModulo(v.z, M_F3D_Helpers->f_GridSizeZ);
		}
		MATH_SubEqualVector(&v, &center);
	}

	SEL_EnumItem
	(
		M_F3D_Sel,
		SEL_C_SIF_Object | SEL_C_SIF_Cob,
		F3D_b_Selection_SubObjectMove,
		(ULONG) & v,
        (_b_Pivot ? 1 : 0) | (b_Pos ? 2 : 0)
	);
#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, TRUE);
#endif
	LINK_Refresh();
}

/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    teleport selected vertices
 -----------------------------------------------------------------------------------------------------------------------
 */
void F3D_cl_View::Selection_SubObjectTeleport(BOOL b_Center)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POINT				st_Pt;
	GDI_tdst_Device		*pst_Dev;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_WinHandles.pst_World == NULL) return;
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;

	if(b_Center)
	{
		v.x = v.y = 0.5f;
		st_Pt.x = pst_Dev->l_Width / 2;
		st_Pt.y = pst_Dev->l_Height / 2;
	}
	else
	{
		GetCursorPos(&st_Pt);
		ScreenToClient(&st_Pt);

		if((unsigned long) st_Pt.x > (unsigned long) pst_Dev->l_Width) return;
		if((unsigned long) st_Pt.y > (unsigned long) pst_Dev->l_Height) return;

		v.x = (float) st_Pt.x / (float) pst_Dev->l_Width;
		v.y = 1.0f - ((float) st_Pt.y / (float) pst_Dev->l_Height);
	}

	b_PickDepth(&v, &v.z);

	v.x = (float) st_Pt.x;
	v.y = (float) st_Pt.y;
	CAM_2Dto3D(&mst_WinHandles.pst_DisplayData->st_Camera, &v, &v);
	
	MATH_SubVector( &v , &v, &M_F3D_Helpers->st_GlobalMatrix.T );
	
	mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this, FALSE);
	mpo_CurrentModif->SetDesc("Weld vertex");
	mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
	mpo_CurrentModif = NULL;
	
	Selection_SubObjectMove( &v, &st_Pt, FALSE, TRUE );
}

/*
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    Rotate all selected vertices
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	F3D_tdst_RotateParams_
{
	MATH_tdst_Vector	st_Axe;
	MATH_tdst_Vector	st_Center;
	float				f_Angle;
} F3D_tdst_RotateParams;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_Selection_SubObjectRotate(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		*pst_GO;
	GEO_tdst_Object			*pst_Obj;
	MATH_tdst_Matrix		Local, Rotate, *M;
	MATH_tdst_Vector		st_LocalAxis, st_LocalCenter;
	F3D_tdst_RotateParams	*pst_Params;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	pst_Params = (F3D_tdst_RotateParams *) p1;

	/*
	if(MATH_b_TestScaleType(M))
	{
		MATH_InvertMatrix(&Local, M);
		MATH_NegVector(&Local.T, &M->T);
		MATH_TransformVectorNoScale(&Local.T, &Local, &Local.T);
		MATH_TransformVertexNoScale(&v_local, &Local, MATH_pst_GetTranslation(World));
		if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		{
			v_local.x *= Local.Sx;
			v_local.y *= Local.Sy;
			v_local.z *= Local.Sz;
		}
	}
	else
	{
		MATH_InvertMatrix(&Local, M);
		MATH_TransformVertexNoScale(&v_local, &Local, MATH_pst_GetTranslation(World));
	}
	*/


	M = OBJ_pst_GetAbsoluteMatrix(pst_GO);

	if(MATH_b_TestScaleType(M))
	{
		MATH_InvertMatrix(&Local, M);
		MATH_NegVector(&Local.T, &M->T);
		MATH_TransformVectorNoScale(&Local.T, &Local, &Local.T);

		MATH_TransformVectorNoScale(&st_LocalAxis, &Local, &pst_Params->st_Axe);
		MATH_NormalizeVector(&st_LocalAxis, &st_LocalAxis);
		MATH_TransformVertexNoScale(&st_LocalCenter, &Local, &pst_Params->st_Center);

	}
	else
	{
		MATH_InvertMatrix(&Local, M);

		MATH_TransformVector(&st_LocalAxis, &Local, &pst_Params->st_Axe);
		MATH_NormalizeVector(&st_LocalAxis, &st_LocalAxis);
		MATH_TransformVertex(&st_LocalCenter, &Local, &pst_Params->st_Center);

	}


	MATH_MakeRotationMatrix_AxisAngle(&Rotate, &st_LocalAxis, pst_Params->f_Angle, NULL, 1);
	MATH_SetTranslation(&Rotate, &st_LocalCenter);

	if(p2)
	{
		MATH_CopyMatrix(&Local, OBJ_pst_GetAbsoluteMatrix(pst_GO));
		MATH_MulMatrixMatrix(OBJ_pst_GetAbsoluteMatrix(pst_GO), &Rotate, &Local);

		MATH_SetIdentityMatrix(&Local);
		MATH_InvertMatrix(&Local, &Rotate);
		MATH_CopyMatrix(&Rotate, &Local);

		OBJ_ComputeLocalWhenHie(pst_GO);
	}

	GEO_SubObject_RotateVertices(pst_Obj, &Rotate, (char) p2, F3D_si_UpdateMorph );

	/*
	 * If we are editing the Pivot of one GameObject, we have to also move to vertices
	 * of its Triangles-based Cob if it has one.
	 */
	if(p2 && (_pst_Sel->l_Flag & SEL_C_SIF_Object) && (pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap))
	{
		/*~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Cob	*pst_Cob;
		/*~~~~~~~~~~~~~~~~~~~~~*/

		pst_Cob = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0];

		if((pst_Cob) && (pst_Cob->uc_Type == COL_C_Zone_Triangles))
		{
			if(!pst_Cob->p_GeoCob)
			{
				GEO_tdst_Object			*pst_GeoCob;

				pst_Cob->p_GeoCob = MEM_p_Alloc(sizeof(GEO_tdst_Object));
				pst_GeoCob = (GEO_tdst_Object *) pst_Cob->p_GeoCob;

				/* Fill the st_GeoCob structure with info needed to see the Cob. */
				L_memset(pst_GeoCob, 0, sizeof(GEO_tdst_Object));

				pst_GeoCob->st_Id.i = &GRO_gast_Interface[GRO_Geometric];
				GEO_CreateGeoFromCob(pst_GO, pst_GeoCob, pst_Cob);
			}

			if(!(((GEO_tdst_Object *) pst_Cob->p_GeoCob)->pst_SubObject))
				GEO_SubObject_Create((GEO_tdst_Object *) pst_Cob->p_GeoCob);
			GEO_SubObject_RotateVertices((GEO_tdst_Object *) pst_Cob->p_GeoCob, &Rotate, (char) p2, 0);
			pst_Cob->uc_Flag |= COL_C_Cob_Updated;
		}
	}

    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
        OBJ_ComputeBV( pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObjectRotate(MATH_tdst_Vector *_pst_Move, BOOL _b_Pivot)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_tdst_RotateParams	st_Params;
	CAM_tdst_Camera			*pst_Camera;
	MATH_tdst_Vector		st_Axe;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!Selection_b_IsInSubObjectMode()) return;

	st_Params.f_Angle = _pst_Move->x;
	if(st_Params.f_Angle == 0)
	{
		st_Params.f_Angle = _pst_Move->y;
		if(st_Params.f_Angle == 0)
		{
			st_Params.f_Angle = _pst_Move->z;
			if(st_Params.f_Angle == 0) return;
		}
	}

	SOFT_Helpers_GetAxe(M_F3D_Helpers, M_F3D_Helpers->l_Pickable, &st_Axe);
	pst_Camera = (CAM_tdst_Camera *) (&M_F3D_DD->st_Camera);
	MATH_TransformVector(&st_Params.st_Axe, &pst_Camera->st_Matrix, &st_Axe);
	MATH_NormalizeVector(&st_Params.st_Axe, &st_Params.st_Axe);
	MATH_TransformVertex(&st_Params.st_Center, &pst_Camera->st_Matrix, &M_F3D_Helpers->st_Center);

	F3D_si_UpdateMorph = ( M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_VertexMoveUpdateMorph ) ? 1 : 0;

	SEL_EnumItem(M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_SubObjectRotate, (ULONG) & st_Params, _b_Pivot);
#ifdef JADEFUSION
	Selection_XenonPostOperation(M_F3D_Sel, FALSE, TRUE);
#endif
	LINK_Refresh();
}

/*
 =======================================================================================================================
    Aim:    Scale all selected vertices
 =======================================================================================================================
 */
static  float                           F3D_f_ScaleNorm;
static  F3D_cl_Undo_MultipleGeoModif    *F3D_spcl_Undo;

BOOL F3D_b_Selection_SubObjectScale(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj, *pst_Old;
	MATH_tdst_Matrix	*M, Local, *World;
	MATH_tdst_Vector	*v, v_local;
	void				*morph;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	v = (MATH_tdst_Vector *) p1;
	World = (MATH_tdst_Matrix *) p2;

	M = OBJ_pst_GetAbsoluteMatrix(pst_GO);
	if(MATH_b_TestScaleType(M))
	{
		MATH_InvertMatrix(&Local, M);
		MATH_NegVector(&Local.T, &M->T);
		MATH_TransformVectorNoScale(&Local.T, &Local, &Local.T);
		MATH_TransformVertexNoScale(&v_local, &Local, MATH_pst_GetTranslation(World));
		if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
		{
			v_local.x *= Local.Sx;
			v_local.y *= Local.Sy;
			v_local.z *= Local.Sz;
		}
	}
	else
	{
		MATH_InvertMatrix(&Local, M);
		MATH_TransformVertexNoScale(&v_local, &Local, MATH_pst_GetTranslation(World));
	}

    {
        char sz_Text[ 256 ];
        sprintf( sz_Text, "(%.3f, %.3f, %.3f) %.3f", v->x, v->y, v->z, F3D_f_ScaleNorm );
        LINK_PrintStatusMsg( sz_Text );
    }

	morph = NULL;
    pst_Old = ( F3D_spcl_Undo ) ? F3D_spcl_Undo->GetOldGeo( pst_Obj, &morph ) : NULL;
    GEO_SubObject_ScaleVertices(pst_Obj, &v_local, v, F3D_f_ScaleNorm, pst_Old ? pst_Old->dst_Point : NULL, morph, F3D_si_UpdateMorph );

    if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
        OBJ_ComputeBV( pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObjectScale(MATH_tdst_Vector *_pst_Move)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	Camera, World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!Selection_b_IsInSubObjectMode()) return;

    if ( Helper_b_IsCurrentAxis( SOFT_Cul_HF_SAObject ) || (M_F3D_Helpers->l_Pickable == SOFT_Cl_Helpers_ScaleLocalXYZ ) )
    {
        F3D_spcl_Undo = NULL;
        F3D_f_ScaleNorm = 0;
    }
    else
    {
        F3D_f_ScaleNorm = MATH_f_NormVector( _pst_Move );
        if ( !mpo_CurrentModif ) return;
        F3D_spcl_Undo = (F3D_cl_Undo_MultipleGeoModif *) mpo_CurrentModif;
        if (F3D_f_ScaleNorm == 0) return;
    }

	/* Build rotation matrix */
	MATH_CopyMatrix(&Camera, &mst_WinHandles.pst_DisplayData->pst_Helpers->st_Matrix);
	MATH_SetTranslation(&Camera, &mst_WinHandles.pst_DisplayData->pst_Helpers->st_Center);
	MATH_MulMatrixMatrix(&World, &Camera, &mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix);

	F3D_si_UpdateMorph = ( M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_VertexMoveUpdateMorph ) ? 1 : 0;

	SEL_EnumItem
	(
		M_F3D_Sel,
		SEL_C_SIF_Object | SEL_C_SIF_Cob,
		F3D_b_Selection_SubObjectScale,
		(ULONG) _pst_Move,
		(ULONG) & World
		);
#ifdef JADEFUSION
    Selection_XenonPostOperation(M_F3D_Sel, FALSE, TRUE);
#endif
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_SubObject_CenterOfGravity(SEL_tdst_SelectedItem *_pst_Sel, ULONG l1, ULONG l2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				*pl_Number, ln;
	MATH_tdst_Vector	*c;
	MATH_tdst_Vector	lc;
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	c = (MATH_tdst_Vector *) l1;
	pl_Number = (LONG *) l2;

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	ln = GEO_l_SubObject_ComputePickedVerticesCenter(pst_Obj, &lc, -1);

	if(ln)
	{
		/* Transform center into global system coordinates */
		if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
			MATH_TransformVertex(&lc, OBJ_pst_GetAbsoluteMatrix(pst_GO), &lc);
		else
			MATH_TransformVertexNoScale(&lc, OBJ_pst_GetAbsoluteMatrix(pst_GO), &lc);

		MATH_ScaleEqualVector(&lc, (float) ln);
		MATH_ScaleEqualVector(c, (float) * pl_Number);
		MATH_AddEqualVector(c, &lc);
		*pl_Number += ln;
		MATH_ScaleEqualVector(c, 1.0f / ((float) * pl_Number));
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_SubObject_CenterOfGravity(MATH_tdst_Vector *C)
{
	/*~~~~~~~~~~~~~*/
	LONG	l_Number, ln;
	GEO_tdst_Object *pst_Obj;
	MATH_tdst_Vector	AlternativeCenter;
	/*~~~~~~~~~~~~~*/
	
	/* check if center is locked */
    GDI_gpst_CurDD = M_F3D_DD;
	MATH_InitVectorToZero(C);
	l_Number = 0;
	SEL_EnumItem
	(
		M_F3D_Sel,
		SEL_C_SIF_Object | SEL_C_SIF_Cob,
		F3D_b_SubObject_CenterOfGravity,
		(ULONG) C,
		(ULONG) & l_Number
	);
	M_F3D_Helpers->l_NbVertices = l_Number;
	
	if ( M_F3D_Helpers->b_CenterLock_On )
	{
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(M_F3D_Helpers->pst_CenterLock_GAO);
		if ( !pst_Obj->pst_SubObject || !pst_Obj->pst_SubObject->dc_VSel || !(pst_Obj->pst_SubObject->dc_VSel[ M_F3D_Helpers->i_CenterLock_Point ] & 1) )
			M_F3D_Helpers->b_CenterLock_On = 0;
		else
		{
			ln = GEO_l_SubObject_ComputePickedVerticesCenter(pst_Obj, &AlternativeCenter, M_F3D_Helpers->i_CenterLock_Point );
			if (ln != 1)
				M_F3D_Helpers->b_CenterLock_On = 0;
			else
				MATH_TransformVertex( C, OBJ_pst_GetAbsoluteMatrix( M_F3D_Helpers->pst_CenterLock_GAO ), &AlternativeCenter);	
		}
	}


    if (l_Number == 1)
    {
    }
}


/*
 =======================================================================================================================
    Manage sub object tools that need a line (build face / edge cutter)
 =======================================================================================================================
 */
typedef struct F3D_SubObject_BuildFaceData_
{
    GEO_tdst_Object *pst_Geo;
    int             i_Nb;
    int             i_Index[3];
    int             i_PickIndex;
    POINT           st_Pos[3];
} F3D_SubObject_BuildFaceData;
/**/
BOOL F3D_b_Selection_PickAVertex(SEL_tdst_SelectedItem *_pst_Sel, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	int					i;
    F3D_SubObject_BuildFaceData *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i = -1;

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob)
			{
				((COL_tdst_Cob *) _pst_Sel->p_Content)->uc_Flag |= COL_C_Cob_Updated;
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob);
			}
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		}
		else
			return TRUE;
	}

	if(!pst_Obj) return TRUE;

    /* pick a vertex */
	i = GEO_i_SubObject_VPick(pst_Obj, (MATH_tdst_Vector *) p2, 1, pst_GO);
    if (i == -1) return TRUE;
    
    pst_Data = (F3D_SubObject_BuildFaceData *) p1;
    pst_Data->pst_Geo = pst_Obj;
    pst_Data->i_PickIndex = i;
    return FALSE;
}
/**/
void F3D_cl_View::Selection_SubObject_Link( POINT *_pst_Point, BOOL b_Begin, BOOL b_Pick )
{
    static F3D_SubObject_BuildFaceData  ast_BF;
    static POINT                        sst_StartPos;
    CDC								    *pDC;
    CPen							    o_Pen, *poldpen;
    MATH_tdst_Vector                    v;
    int                                 x1, y1, x2, y2, z;

    if (!mb_SubObject_LinkOn) return;

    if ( (M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Edge) && (M_F3D_EditOpt->i_EdgeTool == GRO_i_EOT_EdgeCutter) )
    {
        if (b_Begin)
        {
            ast_BF.st_Pos[0] = *_pst_Point;
            ast_BF.st_Pos[1] = *_pst_Point;
            ast_BF.i_Nb = 1;
            return;
        }

        if (b_Pick)
        {
            Selection_EdgeCutter( &ast_BF.st_Pos[0], &ast_BF.st_Pos[1] );
            mb_SubObject_LinkOn = FALSE;
            LINK_Refresh();
            return;
        }
    }
    else if ( (M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Vertex) && (M_F3D_EditOpt->i_VertexTool == GRO_i_EOT_VertexBuildFace) )
    {
        /* point for picking */
        v.x = (float) _pst_Point->x;
        v.y = (float) M_F3D_DD->pst_PickingBuffer->l_Height - _pst_Point->y;

        /* depart */
        if (b_Begin)
        {
            L_memset( &ast_BF, 0, sizeof( F3D_SubObject_BuildFaceData ) );
            SEL_EnumItem( M_F3D_Sel, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Selection_PickAVertex, (ULONG) &ast_BF, (ULONG) &v );
            if (ast_BF.pst_Geo == NULL)
                mb_SubObject_LinkOn = FALSE;
            ast_BF.i_Index[0] = ast_BF.i_PickIndex;
            ast_BF.st_Pos[0] = *_pst_Point;
            ast_BF.st_Pos[1] = *_pst_Point;
            ast_BF.i_Nb = 1;
            return;
        }
    
        /* new point */
        if (b_Pick)
        {
            ast_BF.i_PickIndex = GEO_i_SubObject_VPick(ast_BF.pst_Geo, &v, 0, NULL);
            if ( ast_BF.i_PickIndex != -1)
            {
                ast_BF.st_Pos[ ast_BF.i_Nb ] = *_pst_Point;
                ast_BF.i_Index[ ast_BF.i_Nb++ ] = ast_BF.i_PickIndex;
                ast_BF.st_Pos[ ast_BF.i_Nb ] = *_pst_Point;
                if (ast_BF.i_Nb == 3)
                {
                    mpo_CurrentModif = new F3D_cl_Undo_GeoModif(this, ast_BF.pst_Geo);
	                mpo_CurrentModif->SetDesc("Build face");

                    x1 = ast_BF.st_Pos[ 1 ].x - ast_BF.st_Pos[ 0 ].x;
                    y1 = ast_BF.st_Pos[ 1 ].y - ast_BF.st_Pos[ 0 ].y;
                    x2 = ast_BF.st_Pos[ 2 ].x - ast_BF.st_Pos[ 0 ].x;
                    y2 = ast_BF.st_Pos[ 2 ].y - ast_BF.st_Pos[ 0 ].y;
                    z = x1 * y2 - x2 * y1;
                    x1 = ast_BF.i_Index[0];
                    y1 = ast_BF.i_Index[ (z < 0) ? 2 : 1];
                    y2 = ast_BF.i_Index[ (z < 0) ? 1 : 2];

                    x1 = ast_BF.i_Index[0];
                    y1 = ast_BF.i_Index[1];
                    y2 = ast_BF.i_Index[2];

                    if ( GEO_i_SubObject_VBuildFace( ast_BF.pst_Geo, x1, y1, y2, M_F3D_EditOpt->l_FaceId ) )
                        mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
                    else
                        delete mpo_CurrentModif;

                    mpo_CurrentModif = NULL;

                    mb_SubObject_LinkOn = FALSE;
                    LINK_Refresh();
                    return;
                }
            }
        }
    }

    pDC = GetDC();
#ifdef JADEFUSION
	o_Pen.CreatePen(PS_SOLID, 1, GetSysColor(0x00ffffff));
#else
	o_Pen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_ACTIVECAPTION));
#endif
	poldpen = pDC->SelectObject(&o_Pen);
	pDC->SetROP2(R2_XORPEN);

    pDC->MoveTo( ast_BF.st_Pos[ ast_BF.i_Nb - 1 ] );
	pDC->LineTo( ast_BF.st_Pos[ ast_BF.i_Nb ] );

    ast_BF.st_Pos[ ast_BF.i_Nb ] = *_pst_Point;

    pDC->MoveTo( ast_BF.st_Pos[ ast_BF.i_Nb - 1 ] );
	pDC->LineTo( ast_BF.st_Pos[ ast_BF.i_Nb ] );
    ReleaseDC( pDC );
}
#ifdef JADEFUSION
void F3D_cl_View::Selection_XenonRefresh(void)
{
    if ((mst_WinHandles.pst_World != NULL) && (mst_WinHandles.pst_World->pst_Selection != NULL))
    {
        Selection_XenonPostOperation(M_F3D_Sel, TRUE, FALSE);
    }
}

void F3D_cl_View::Selection_XenonPostOperation(SEL_tdst_Selection* _pst_Sel, BOOL _b_Pack, BOOL _b_QuickUpdate)
{
#if defined(_XENON_RENDER)
    SEL_tdst_SelectedItem* pst_Item;

    pst_Item = _pst_Sel->pst_FirstItem;
    while(pst_Item != NULL)
    {
        if(pst_Item->l_Flag & SEL_C_SIF_Object)
        {
            OBJ_tdst_GameObject* pst_GO  = (OBJ_tdst_GameObject*)pst_Item->p_Content;

            if (_b_Pack)
            {
                GEO_PackGameObject(pst_GO);
            }
            else
            {
                GEO_PackGameObjectEd(pst_GO, _b_QuickUpdate);
            }
        }

        pst_Item = pst_Item->pst_Next;
    }
#endif
}

#endif
#endif /* ACTIVE_EDITORS */

