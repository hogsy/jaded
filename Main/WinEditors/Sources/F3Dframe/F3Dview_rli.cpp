/*$T F3Dview_rli.cpp GC 1.129 09/14/01 17:20:53 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#ifdef JADEFUSION
#include "selection/selection.h"
#include "engine/sources/world/worstruct.h"
#include "DIAlogs/DIALightmaps.h"
#include "texture/texfile.h"
#include "texture/texstruct.h"
#include "RADLM/RADLM.h"
#endif
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKtoed.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "EDImsg.h"
#include "EDIpaths.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "SOFT/SOFThelper.h"
#include "GEOmetric/GEOstaticlod.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJsave.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIaccess.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dview_undo.h"
#include "F3Dframe/F3Dstrings.h"
#include "AIinterp/Sources/Events/EVEinit.h"
#include "SouND/Sources/SND.h"
#include "EDItors/Sources/EVEnts/EVEmsg.h"
#include "DIAlogs/DIAvector_dlg.h"
#include "DIAlogs/DIArli_dlg.h"

#ifdef JADEFUSION
#include "LIGHT/LIGHTmapstruct.h"
#include "LIGHT/LIGHTmapcompute.h"
#include "LIGHT/LIGHTmap.h"
#include "DIAlogs/DIA_UPDATE_dlg.h"
#endif

#if defined(_XENON_RENDER)
#include "GraphicDK/Sources/GEOmetric/GEOXenonPack.h"
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
BOOL F3D_b_RLILocked( OBJ_tdst_GameObject *_pst_GO )
{
    if ( (_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_RLILocked) || !(_pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_UnlockedRLI) )
    {
        char sz_Text[256];
        sprintf( sz_Text, "RLI locked on (%s)", _pst_GO->sz_Name ? _pst_GO->sz_Name : "unknow" );
        LINK_PrintStatusMsg( sz_Text );
        return TRUE;
    }
    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_ResetRLI(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Target, ULONG _ul_Mask)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	char				asz_Name[512];
	GEO_tdst_Object		*pst_Object, *pst_Obj2;
	GEO_tdst_StaticLOD	*pst_LOD;
	unsigned long		*pul_RLI, *pul_Last;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) return TRUE;

    if (F3D_b_RLILocked( pst_GO )) return TRUE;

	/* reset local RLI */
	if((_ul_Target & DIARLI_tgt_GAO) && (pst_GO->pst_Base->pst_Visu->dul_VertexColors))
	{
		pul_RLI = pst_GO->pst_Base->pst_Visu->dul_VertexColors;
		pul_Last = pul_RLI + (pul_RLI[0] + 1);
		pul_RLI++;
		for(; pul_RLI < pul_Last; pul_RLI++) *pul_RLI &= _ul_Mask;
		sprintf(asz_Name, "Reset RLI %s", pst_GO->sz_Name);
		LINK_PrintStatusMsg(asz_Name);
	}

	/* reset global RLI */
	if(_ul_Target & DIARLI_tgt_GRO)
	{
		pst_Object = (GEO_tdst_Object *) pst_GO->pst_Base->pst_Visu->pst_Object;
		if(!pst_Object) return TRUE;

		/* Geometry ? */
		if(pst_Object->st_Id.i->ul_Type == GRO_Geometric)
		{
			if(pst_Object->dul_PointColors)
			{
				GEO_ResetRLI(pst_Object, _ul_Mask);
#if defined(_XENON_RENDER)
                GEO_ResetXenonMesh(pst_GO, pst_GO->pst_Base->pst_Visu, (GEO_tdst_Object*)pst_GO->pst_Base->pst_Visu->pst_Object, FALSE, FALSE, FALSE);
#endif
				sprintf(asz_Name, "Reset RLI of geometry %s", GRO_sz_Struct_GetName(&pst_Object->st_Id));
				LINK_PrintStatusMsg(asz_Name);
			}
		}

		/* LOD ? */
		else if(pst_Object->st_Id.i->ul_Type == GRO_GeoStaticLOD)
		{
			pst_LOD = (GEO_tdst_StaticLOD *) pst_Object;
			for(i = 0; i < pst_LOD->uc_NbLOD; i++)
			{
				pst_Obj2 = (GEO_tdst_Object *) pst_LOD->dpst_Id[i];
				if((pst_Obj2) && (pst_Obj2->st_Id.i->ul_Type == GRO_Geometric) && (pst_Obj2->dul_PointColors))
				{
					GEO_ResetRLI(pst_Obj2, _ul_Mask);
					sprintf(asz_Name, "Reset RLI of geometry %s", GRO_sz_Struct_GetName(&pst_Obj2->st_Id));
					LINK_PrintStatusMsg(asz_Name);
				}
			}
		}
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_ResetRLI(ULONG _ul_Target, ULONG _ul_Mask, BOOL _b_Sel)
{
	if(mst_WinHandles.pst_World == NULL) return;
    if ( _b_Sel )
	    SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_ResetRLI, _ul_Target, ~_ul_Mask);
    else
        SEL_EnumWorldGao( mst_WinHandles.pst_World, F3D_b_ResetRLI, _ul_Target, ~_ul_Mask);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_DestroyRLI(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Target, ULONG ul2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	char				asz_Name[512];
	GEO_tdst_Object		*pst_Object, *pst_Obj2;
	GEO_tdst_StaticLOD	*pst_LOD;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) return TRUE;

    if ( (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_RLILocked) || !(pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_UnlockedRLI) )
    {
        char sz_Text[256];
        sprintf( sz_Text, "RLI locked on (%s)", pst_GO->sz_Name ? pst_GO->sz_Name : "unknow" );
        LINK_PrintStatusMsg( sz_Text );
        return TRUE;
    }

	/* destroy local RLI */
	if((_ul_Target & DIARLI_tgt_GAO) && (pst_GO->pst_Base->pst_Visu->dul_VertexColors))
	{
		OBJ_VertexColor_Free( pst_GO );
#if defined(_XENON_RENDER)
        if (pst_GO->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_Geometric)
        {
            GEO_ResetXenonMesh(pst_GO, pst_GO->pst_Base->pst_Visu, (GEO_tdst_Object*)pst_GO->pst_Base->pst_Visu->pst_Object, FALSE, FALSE, FALSE);
        }
#endif
		//MEM_Free(pst_GO->pst_Base->pst_Visu->dul_VertexColors);
		//pst_GO->pst_Base->pst_Visu->dul_VertexColors = NULL;
		sprintf(asz_Name, "Destroy RLI %s", pst_GO->sz_Name);
		LINK_PrintStatusMsg(asz_Name);
	}

	/* destroy global RLI */
	if(_ul_Target & DIARLI_tgt_GRO)
	{
		pst_Object = (GEO_tdst_Object *) pst_GO->pst_Base->pst_Visu->pst_Object;
		if(!pst_Object) return TRUE;

		/* geometry ? */
		if(pst_Object->st_Id.i->ul_Type == GRO_Geometric)
		{
			if(pst_Object->dul_PointColors)
			{
				GEO_DestroyRLI(pst_Object);
#if defined(_XENON_RENDER)
                GEO_ResetXenonMesh(pst_GO, pst_GO->pst_Base->pst_Visu, (GEO_tdst_Object*)pst_GO->pst_Base->pst_Visu->pst_Object, FALSE, FALSE, FALSE);
#endif
				sprintf(asz_Name, "Reset RLI of geometry %s", GRO_sz_Struct_GetName(&pst_Object->st_Id));
				LINK_PrintStatusMsg(asz_Name);
			}
		}

		/* LOD ? */
		else if(pst_Object->st_Id.i->ul_Type == GRO_GeoStaticLOD)
		{
			pst_LOD = (GEO_tdst_StaticLOD *) pst_Object;
			for(i = 0; i < pst_LOD->uc_NbLOD; i++)
			{
				pst_Obj2 = (GEO_tdst_Object *) pst_LOD->dpst_Id[i];
				if((pst_Obj2) && (pst_Obj2->st_Id.i->ul_Type == GRO_Geometric) && (pst_Obj2->dul_PointColors))
				{
					GEO_DestroyRLI(pst_Obj2);
					sprintf(asz_Name, "Reset RLI of geometry %s", GRO_sz_Struct_GetName(&pst_Obj2->st_Id));
					LINK_PrintStatusMsg(asz_Name);
				}
			}
		}
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_DestroyRLI(ULONG _ul_Target, BOOL _b_Sel )
{
	if(mst_WinHandles.pst_World == NULL) return;
    if ( _b_Sel )
	    SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_DestroyRLI, _ul_Target, 0);
    else
        SEL_EnumWorldGao( mst_WinHandles.pst_World, F3D_b_DestroyRLI, _ul_Target, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_RLIComputeAlpha2Color(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Target, ULONG ul2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	char				asz_Name[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) return TRUE;

	/* compute alpha to color */
	if (pst_GO->pst_Base->pst_Visu->dul_VertexColors)
	{
        GEO_RLIAlphaToColor( pst_GO->pst_Base->pst_Visu->dul_VertexColors );
		sprintf(asz_Name, "Alpha to color for %s", pst_GO->sz_Name);
        LINK_PrintStatusMsg(asz_Name);
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_RLIComputeAlpha2Color( BOOL _b_Sel )
{
	if(mst_WinHandles.pst_World == NULL) return;
    if ( _b_Sel )
	    SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_RLIComputeAlpha2Color, 0, 0);
    else
        SEL_EnumWorldGao( mst_WinHandles.pst_World, F3D_b_RLIComputeAlpha2Color, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_CopyLocal2Global(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Target, ULONG _ul_Mask)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Object;
	unsigned long		*pul_RLITgt, *pul_RLITgtLast, *pul_RLISrc;
	ULONG				ul_NotMask, ul_Nb;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) return TRUE;

	pst_Object = (GEO_tdst_Object *) pst_GO->pst_Base->pst_Visu->pst_Object;
	if(!pst_Object) return TRUE;

    if (pst_Object->st_Id.i->ul_Type == GRO_GeoStaticLOD )
    {
        GEO_tdst_StaticLOD *pst_LOD;
        pst_LOD = (GEO_tdst_StaticLOD *) pst_Object;
        if ( pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ForceLOD )
            ul_Nb = pst_GO->ul_ForceLODIndex % pst_LOD->uc_NbLOD;
        else
            ul_Nb = pst_GO->ul_ForceLODIndex = GEO_l_StaticLOD_GetIndex( pst_LOD, pst_LOD->uc_Distance);
        pst_Object = (GEO_tdst_Object *) pst_LOD->dpst_Id[ ul_Nb ];
    }

    if(pst_Object->st_Id.i->ul_Type != GRO_Geometric) return TRUE;
	ul_Nb = ((GEO_tdst_Object *) pst_Object)->l_NbPoints;

	/* Source RLI = local RLI */
	pul_RLISrc = pst_GO->pst_Base->pst_Visu->dul_VertexColors;
	if(pul_RLISrc == NULL) return TRUE;
	if(*pul_RLISrc != ul_Nb)
	{
		pul_RLISrc = OBJ_VertexColor_Realloc( pst_GO, ul_Nb );
		/*
		pst_GO->pst_Base->pst_Visu->dul_VertexColors = pul_RLISrc = (ULONG *) MEM_p_Realloc
			(
				pul_RLISrc,
				(ul_Nb + 1) * 4
			);
		*pul_RLISrc = ul_Nb;
		*/
	}

	if(((GEO_tdst_Object *) pst_Object)->dul_PointColors)
	{
		if(((GEO_tdst_Object *) pst_Object)->dul_PointColors[0] != ul_Nb)
		{
			MEM_Free(((GEO_tdst_Object *) pst_Object)->dul_PointColors);
			((GEO_tdst_Object *) pst_Object)->dul_PointColors = NULL;
		}
	}

	if(((GEO_tdst_Object *) pst_Object)->dul_PointColors == NULL)
	{
		((GEO_tdst_Object *) pst_Object)->dul_PointColors = (ULONG *) MEM_p_Alloc((ul_Nb + 1) * 4);
		((GEO_tdst_Object *) pst_Object)->dul_PointColors[0] = ul_Nb;
		L_memset(((GEO_tdst_Object *) pst_Object)->dul_PointColors + 1, 0, ul_Nb * 4);
	}

	/* Target = local RLI */
	pul_RLITgt = ((GEO_tdst_Object *) pst_Object)->dul_PointColors;

	pul_RLITgt++;
	pul_RLISrc++;
	pul_RLITgtLast = pul_RLITgt + ul_Nb;

	ul_NotMask = ~_ul_Mask;

	for(; pul_RLITgt < pul_RLITgtLast; pul_RLITgt++, pul_RLISrc++)
	{
		*pul_RLITgt &= ul_NotMask;
		*pul_RLITgt |= (*pul_RLISrc) & _ul_Mask;
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_CopyLocal2Global(ULONG _ul_Mask, BOOL _b_Sel)
{
	if(mst_WinHandles.pst_World == NULL) return;
    if (_b_Sel)
	    SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_CopyLocal2Global, 0, _ul_Mask);
    else
        SEL_EnumWorldGao( mst_WinHandles.pst_World, F3D_b_CopyLocal2Global, 0, _ul_Mask);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_CopyGlobal2Local(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Target, ULONG _ul_Mask)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Object;
	unsigned long		*pul_RLITgt, *pul_RLITgtLast, *pul_RLISrc;
	ULONG				ul_NotMask, ul_Nb;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) return TRUE;

    if ( (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_RLILocked) || !(pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_UnlockedRLI) )
    {
        char sz_Text[256];
        sprintf( sz_Text, "RLI locked on (%s)", pst_GO->sz_Name ? pst_GO->sz_Name : "unknow" );
        LINK_PrintStatusMsg( sz_Text );
        return TRUE;
    }

	pst_Object = (GEO_tdst_Object *) pst_GO->pst_Base->pst_Visu->pst_Object;
	if(!pst_Object) return TRUE;
	if(pst_Object->st_Id.i->ul_Type != GRO_Geometric) return TRUE;

	/* Source RLI = global RLI */
	pul_RLISrc = ((GEO_tdst_Object *) pst_Object)->dul_PointColors;
	if(pul_RLISrc == NULL) return TRUE;
	ul_Nb = ((GEO_tdst_Object *) pst_Object)->l_NbPoints;
	if(*pul_RLISrc != ul_Nb)
	{
		((GEO_tdst_Object *) pst_Object)->dul_PointColors = pul_RLISrc = (ULONG *) MEM_p_Realloc
			(
				pul_RLISrc,
				(ul_Nb + 1) * 4
			);
		*pul_RLISrc = ul_Nb;
	}

	if(pst_GO->pst_Base->pst_Visu->dul_VertexColors)
	{
		if(pst_GO->pst_Base->pst_Visu->dul_VertexColors[0] != ul_Nb)
			OBJ_VertexColor_Realloc( pst_GO, ul_Nb );
	}
	else
	{
		pst_GO->pst_Base->pst_Visu->dul_VertexColors = (ULONG *) MEM_p_Alloc((ul_Nb + 1) * 4);
		pst_GO->pst_Base->pst_Visu->dul_VertexColors[0] = ul_Nb;
		L_memset(pst_GO->pst_Base->pst_Visu->dul_VertexColors + 1, 0, ul_Nb * 4);
	}

	/* Target = local RLI */
	pul_RLITgt = pst_GO->pst_Base->pst_Visu->dul_VertexColors;

	pul_RLITgt++;
	pul_RLISrc++;
	pul_RLITgtLast = pul_RLITgt + ul_Nb;

	ul_NotMask = ~_ul_Mask;

	for(; pul_RLITgt < pul_RLITgtLast; pul_RLITgt++, pul_RLISrc++)
	{
		*pul_RLITgt &= ul_NotMask;
		*pul_RLITgt |= (*pul_RLISrc) & _ul_Mask;
	}

	return TRUE;
}
/**/
void F3D_cl_View::Selection_CopyGlobal2Local(ULONG _ul_Mask, BOOL _b_Sel )
{
	if(mst_WinHandles.pst_World == NULL) return;
    if (_b_Sel)
	    SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_CopyGlobal2Local, 0, _ul_Mask);
    else
        SEL_EnumWorldGao( mst_WinHandles.pst_World, F3D_b_CopyGlobal2Local, 0, _ul_Mask);

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::ComputeRLI(ULONG _ul_Mask, BOOL _b_Ask, BOOL _b_Sel, BOOL _b_LightSel )
{
	/*~~*/
	int i;
	/*~~*/

    i = _b_Ask ? (M_MF()->MessageBox("Destroy old RLI ?", "Jade", MB_YESNOCANCEL)) : IDYES;
	LIGHT_gl_ComputeRLI = (i == IDYES) ? 3 : (i == IDNO) ? 1 : 0;
    if (_b_Sel) LIGHT_gl_ComputeRLI |= 4;
    if (_b_LightSel) LIGHT_gl_ComputeRLI |= 8;
	
    if(LIGHT_gl_ComputeRLI & 1)
	{
		LIGHT_gul_ComputeRLIMask = _ul_Mask;
		if(LIGHT_gl_ComputeRLI & 2) ChangeEditFlags(0, OBJ_C_EditFlags_RLINotReset, 0);
		AfxGetApp()->DoWaitCursor(1);
		LINK_Refresh();
		LIGHT_gl_ComputeRLI = 0;
		AfxGetApp()->DoWaitCursor(-1);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_RLILock(SEL_tdst_SelectedItem *_pst_Sel, ULONG ul1, ULONG ul2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) return TRUE;

    pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_RLILocked;
    pst_GO->pst_Base->pst_Visu->ul_DrawMask &=  ~GDI_Cul_DM_UnlockedRLI;
	return TRUE;
}
/**/
void F3D_cl_View::Selection_RLILock( BOOL _b_Sel )
{
	if(mst_WinHandles.pst_World == NULL) return;
    if ( _b_Sel )
	    SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_RLILock, 0, 0);
    else
        SEL_EnumWorldGao( mst_WinHandles.pst_World, F3D_b_RLILock, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_RLIUnlock(SEL_tdst_SelectedItem *_pst_Sel, ULONG ul1, ULONG ul2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) return TRUE;

    pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_RLILocked;
    pst_GO->pst_Base->pst_Visu->ul_DrawMask |= GDI_Cul_DM_UnlockedRLI;
	return TRUE;
}
/**/
void F3D_cl_View::Selection_RLIUnlock( BOOL _b_Sel )
{
	if(mst_WinHandles.pst_World == NULL) return;
    if ( _b_Sel )
	    SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_RLIUnlock, 0, 0);
    else
        SEL_EnumWorldGao( mst_WinHandles.pst_World, F3D_b_RLIUnlock, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_RLIInvertAlpha(SEL_tdst_SelectedItem *_pst_Sel, ULONG ul1, ULONG ul2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) return TRUE;
    if (!pst_GO->pst_Base->pst_Visu->dul_VertexColors) return TRUE;

    GEO_RLIInvertAlpha( pst_GO->pst_Base->pst_Visu->dul_VertexColors );
	return TRUE;
}
/**/
void F3D_cl_View::Selection_RLIInvertAlpha( void )
{
	if(mst_WinHandles.pst_World == NULL) return;
    SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_RLIInvertAlpha, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_RLIAddColor(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Target, ULONG _ul_Color)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
    ULONG               *pul_RLI;
    GEO_tdst_Object     *pst_Object, *pst_Obj2;
    GEO_tdst_StaticLOD  *pst_LOD;
    int                 i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) return TRUE;

    if (F3D_b_RLILocked( pst_GO )) return TRUE;

	/* reset local RLI */
	if((_ul_Target & DIARLI_tgt_GAO) && (pst_GO->pst_Base->pst_Visu->dul_VertexColors))
	{
		pul_RLI = pst_GO->pst_Base->pst_Visu->dul_VertexColors;
        LIGHT_ColorAdd( _ul_Color, pul_RLI+1, pul_RLI[0], pul_RLI+1);
	}

	/* reset global RLI */
	if(_ul_Target & DIARLI_tgt_GRO)
	{
		pst_Object = (GEO_tdst_Object *) pst_GO->pst_Base->pst_Visu->pst_Object;
		if(!pst_Object) return TRUE;

		/* Geometry ? */
		if(pst_Object->st_Id.i->ul_Type == GRO_Geometric)
		{
            pul_RLI = pst_Object->dul_PointColors;
            LIGHT_ColorAdd( _ul_Color, pul_RLI+1, pul_RLI[0], pul_RLI+1);
		}
		/* LOD ? */
		else if(pst_Object->st_Id.i->ul_Type == GRO_GeoStaticLOD)
		{
			pst_LOD = (GEO_tdst_StaticLOD *) pst_Object;
			for(i = 0; i < pst_LOD->uc_NbLOD; i++)
			{
				pst_Obj2 = (GEO_tdst_Object *) pst_LOD->dpst_Id[i];
				if((pst_Obj2) && (pst_Obj2->st_Id.i->ul_Type == GRO_Geometric) && (pst_Obj2->dul_PointColors))
				{
                    pul_RLI = pst_Obj2->dul_PointColors;
                    LIGHT_ColorAdd( _ul_Color, pul_RLI+1, pul_RLI[0], pul_RLI+1);
				}
			}
		}
	}
	return TRUE;
}
/**/
void F3D_cl_View::Selection_RLIAddColor( ULONG ul_Target, ULONG ul_Color, BOOL _b_Sel )
{
	if(mst_WinHandles.pst_World == NULL) return;
    if ( _b_Sel )
	    SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_RLIAddColor, ul_Target, ul_Color );
    else
        SEL_EnumWorldGao( mst_WinHandles.pst_World, F3D_b_RLIUnlock, ul_Target, ul_Color );
	LINK_Refresh();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::RLITool(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_RLIDialog	o_RLI( this );
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_RLI.DoModal();

	/*
    switch(o_RLI.mul_Operation)
	{
	case DIARLI_op_compute:			    ComputeRLI(o_RLI.mul_Mask, TRUE, o_RLI.mb_ObjectSel, o_RLI.mb_LightSel); break;
	case DIARLI_op_destroycompute:	    ComputeRLI(o_RLI.mul_Mask, FALSE, o_RLI.mb_ObjectSel, o_RLI.mb_LightSel); break;
	case DIARLI_op_copylg:			    Selection_CopyLocal2Global(o_RLI.mul_Mask, o_RLI.mb_ObjectSel); break;
	case DIARLI_op_copygl:			    Selection_CopyGlobal2Local(o_RLI.mul_Mask, o_RLI.mb_ObjectSel); break;
	case DIARLI_op_reset:			    Selection_ResetRLI(o_RLI.mul_Target, o_RLI.mul_Mask, o_RLI.mb_ObjectSel); break;
	case DIARLI_op_destroy:			    Selection_DestroyRLI(o_RLI.mul_Target, o_RLI.mb_ObjectSel); break;
    case DIARLI_op_computealphatocolor:	Selection_RLIComputeAlpha2Color(o_RLI.mb_ObjectSel); break;
    case DIARLI_op_lock:                Selection_RLILock(o_RLI.mb_ObjectSel); break;
    case DIARLI_op_unlock:              Selection_RLIUnlock(o_RLI.mb_ObjectSel); break;
    case DIARLI_op_invertalpha:         Selection_RLIInvertAlpha(); break;
	}
    */
}
#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/

static EDIA_cl_UPDATEDialog *po_Dial;

void F3D_cl_View::LightMapProgressCallback(float _percent, char* _msg, void* _this)
{
    // TODO DOM: update the view so the user knows it's not crashed
    po_Dial->OnRefreshBarText(_percent, _msg);
}

//extern ULONG	LIGHT_g_NbOfLightMappedObjects;

void F3D_cl_View::DestroyLightmaps(void)
{
    WOR_tdst_World*	pWorld = mst_WinHandles.pst_DisplayData->pst_World;
    TAB_tdst_PFelem		*pst_CurrentElem;
    TAB_tdst_PFelem		*pst_EndElem;
    OBJ_tdst_GameObject *pst_Object;

    //LIGHT_g_NbOfLightMappedObjects = 0;

    // loop on all objects of the worls and kill the lightmaps

    pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&pWorld->st_AllWorldObjects);
    pst_EndElem = TAB_pst_PFtable_GetLastElem(&pWorld->st_AllWorldObjects);

    // loop on all the game objects
    for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
    {
        pst_Object = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

        if(TAB_b_IsAHole(pst_Object)) 
            continue;

        LIGHT_Lightmaps_DestroyGAOLightmaps(pst_Object);
    }
}

void F3D_cl_View::ComputeLightmaps(SEL_tdst_Selection* pSelection, bool _bNoDialog)
{
    static tdst_LightmapsComputationOptions options;
    static bool firstTime = true;

    EDIA_cl_LightmapsDialog o_LMDialog;

    if (firstTime)
    {
        options.computePS2Lightmaps = false;
        options.computeShadows = true;
        options.doSuperSampling = false;
        options.superSamplingFactor = 2.0f;
        options.shadowMapMode = true;
        options.texelPerMeter = 2.0f;
        options.shadowOpacity = 1.0f;
        options.fixBackFaceBug = false;
        firstTime = false;
    }

    memcpy(&o_LMDialog.mOptions, &options, sizeof(tdst_LightmapsComputationOptions));	

    if (_bNoDialog || o_LMDialog.DoModal() == IDOK)
    { 

        po_Dial = new EDIA_cl_UPDATEDialog((char *) "");
        po_Dial->DoModeless();

        // read the options        
        AfxGetApp()->DoWaitCursor(1);

        if (pSelection && (SEL_l_CountItem(pSelection, SEL_C_SIF_Object)))
        {
            LIGHT_ComputeAllLightmaps(mst_WinHandles.pst_DisplayData->pst_World, ((EOUT_cl_Frame*)(mpo_Frame->mpo_AssociatedEditor))->mul_CurrentWorld, &o_LMDialog.mOptions,(LMProgressCallback)&F3D_cl_View::LightMapProgressCallback, this, pSelection);			
        }
        else
        {
            LIGHT_ComputeAllLightmaps(mst_WinHandles.pst_DisplayData->pst_World, ((EOUT_cl_Frame*)(mpo_Frame->mpo_AssociatedEditor))->mul_CurrentWorld, &o_LMDialog.mOptions,(LMProgressCallback)&F3D_cl_View::LightMapProgressCallback, this, NULL);
        }
        AfxGetApp()->DoWaitCursor(-1);

        memcpy(&options, &o_LMDialog.mOptions, sizeof(tdst_LightmapsComputationOptions));	

        delete(po_Dial);
    }
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void F3D_cl_View::RadiosityLightmap(void)
{
    /*
    BOOL _b_Sel = TRUE;

    if(mst_WinHandles.pst_World == NULL) return;
    if (_b_Sel)
        SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_CopyLocal2Global, 0, _ul_Mask);
    //else
    //    SEL_EnumWorldGao( mst_WinHandles.pst_World, F3D_b_CopyLocal2Global, 0, _ul_Mask);
    LINK_Refresh();

    SEL_tdst_SelectedItem	*_pst_Sel;

    _pst_Sel = _pst_Sel->pst_FirstItem;
    while(_pst_Sel != NULL)
    {
        if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
        {
            OBJ_tdst_GameObject *pst_GO;
            GEO_tdst_Object		*pst_Object;

            pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
            if(!pst_GO) 
                continue;
            if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) 
                continue;

            pst_Object = (GEO_tdst_Object *) pst_GO->pst_Base->pst_Visu->pst_Object;
            if(!pst_Object) 
                continue;

            if (pst_Object->st_Id.i->ul_Type == GRO_GeoStaticLOD )
            {
                GEO_tdst_StaticLOD *pst_LOD;
                pst_LOD = (GEO_tdst_StaticLOD *) pst_Object;
                if ( pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ForceLOD )
                    ul_Nb = pst_GO->ul_ForceLODIndex % pst_LOD->uc_NbLOD;
                else
                    ul_Nb = pst_GO->ul_ForceLODIndex = GEO_l_StaticLOD_GetIndex( pst_LOD, pst_LOD->uc_Distance);
                pst_Object = (GEO_tdst_Object *) pst_LOD->dpst_Id[ ul_Nb ];
            }

            if(pst_Object->st_Id.i->ul_Type != GRO_Geometric) 
                continue;

            RADLM_Compute(pst_Object);
        }

        _pst_Sel = _pst_Sel->pst_Next;
    }
    */
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void F3D_cl_View::XeSelectionFixRLI(void)
{
#if defined(_XENON_RENDER)
    SEL_tdst_SelectedItem* pst_Item;

    pst_Item = mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
    while(pst_Item != NULL)
    {
        if(pst_Item->l_Flag & SEL_C_SIF_Object)
        {
            OBJ_tdst_GameObject* pst_GO  = (OBJ_tdst_GameObject*)pst_Item->p_Content;

            if (OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu) && 
                pst_GO->pst_Base                                        && 
                pst_GO->pst_Base->pst_Visu                              && 
                pst_GO->pst_Base->pst_Visu->dul_VertexColors            && 
                pst_GO->pst_Base->pst_Visu->pst_Object)
            {
                LONG l_NbPoints = -1;

                GRO_tdst_Struct* pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Object;

                if (pst_Gro->i->ul_Type == GRO_Geometric)
                {
                    l_NbPoints = ((GEO_tdst_Object*)pst_Gro)->l_NbPoints;
                }
                else if (pst_Gro->i->ul_Type == GRO_GeoStaticLOD)
                {
                    GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Gro;

                    if ((pst_LOD->uc_NbLOD > 0) && pst_LOD->dpst_Id[0] &&
                        pst_LOD->dpst_Id[0]->i->ul_Type == GRO_Geometric)
                    {
                        l_NbPoints = ((GEO_tdst_Object*)pst_LOD->dpst_Id[0])->l_NbPoints;
                    }
                }

                if ((l_NbPoints > 0) && (pst_GO->pst_Base->pst_Visu->dul_VertexColors[0] != l_NbPoints))
                {
                    OBJ_VertexColor_Realloc(pst_GO, (ULONG)l_NbPoints);

                    GEO_PackGameObject(pst_GO);
                }
            }
        }

        pst_Item = pst_Item->pst_Next;
    }
#endif
}
#endif

#endif
