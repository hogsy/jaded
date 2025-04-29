/*$T F3DView_Selection.cpp GC 1.139 04/15/04 13:45:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDIapp.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dstrings.h"
#include "F3Dframe/F3Dview_undo.h"
#include "CAMera/CAMera.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "GEOmetric/GEOsubobject.h"
#include "LIGHT/LIGHTstruct.h"
#include "SOFT/SOFThelper.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "SELection/SELection.h"
#include "Res/Res.h"
#include "BASe/MEMory/MEM.h"
#include "EDIpaths.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "DIAlogs/DIAselection_dlg.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAshape_dlg.h"
#include "DIAlogs/DIAlist_dlg.h"
#include "DIAlogs/DIAanim_dlg.h"
#include "DIAlogs/DIAcreategeometry_dlg.h"
#include "DIAlogs/DIAcreategeometry_inside.h"
#include "DIAlogs/DIAgaoinfo_dlg.h"
#include "DIAlogs/DIAgrovertexpos_dlg.h"
#include "DIAlogs/DIAbrickmapper_dlg.h"
#include "F3Dframe/F3Dframe.h"

extern BOOL SOFT_gb_WindowSel;

#ifdef JADEFUSION
#include "DIAlogs/DIAreplace_dlg.h"
#include "DIAlogs/DIAlightrej_dlg.h"
#include "DIAlogs/DIAcubemapgen_dlg.h"

extern SOFT_tdst_PickingBuffer *pPickForZCull;
#else
extern SOFT_tdst_PickingBuffer *pPickForZCull;
#endif
/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */

#define RECT(pDC, o) \
	{ \
		pDC->MoveTo(o.left, o.top); \
		pDC->LineTo(o.right, o.top); \
		pDC->LineTo(o.right, o.bottom); \
		pDC->LineTo(o.left, o.bottom); \
		pDC->LineTo(o.left, o.top); \
	}

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::Selection_b_IsLocked(void)
{
	return M_F3D_Sel->l_Flag & SEL_C_SF_Locked;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *F3D_cl_View::Selection_pst_GetFirstObject(void)
{
	/*~~~~~~~*/
	void	*p;
	/*~~~~~~~*/

	if(!mst_WinHandles.pst_World) return NULL;
	p = SEL_pst_GetFirstItem(M_F3D_Sel, SEL_C_SIF_Object);
	return (OBJ_tdst_GameObject *) p;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::Selection_b_Prefab(OBJ_tdst_GameObject *_pst_Obj, ULONG _ul_Param, BOOL _b_Msg, BOOL _b_ModifSel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
#ifdef JADEFUSION
	int					i_index;
	CString				str_Name, str_PFBName;
#else
	char				sz_Key[11];
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mst_WinHandles.pst_World) return FALSE;
	if(!_pst_Obj->sz_Name) return FALSE;
	if((_pst_Obj->sz_Name[0] != '[') || (_pst_Obj->sz_Name[9] != ']')) return FALSE;

#ifdef JADEFUSION
	//get PreFab name
	str_Name = _pst_Obj->sz_Name;
	i_index = str_Name.Find('@');
	if(i_index == -1) 
	{
		i_index = str_Name.Find(']');
		if(i_index == -1)
			return FALSE;
	}
	str_PFBName = str_Name.Left(i_index);
#else
	L_memcpy(sz_Key, _pst_Obj->sz_Name, 10);
#endif

	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	while(pst_PFElem <= pst_PFLastElem)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		pst_PFElem++;
		if(TAB_b_IsAHole(pst_GO)) continue;
#ifdef JADEFUSION
		//get object name
		str_Name = pst_GO->sz_Name;
		i_index = str_Name.Find('@');
		if(i_index == -1)
		{
			i_index = str_Name.Find(']');
			if(i_index == -1) continue;
		}

		if(pst_GO->sz_Name && !str_PFBName.Compare(str_Name.Left(i_index)))
#else
		if(pst_GO->sz_Name && !L_memcmp(sz_Key, pst_GO->sz_Name, 10))
#endif
			Selection_Object(pst_GO, _ul_Param, _b_Msg, _b_ModifSel);
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_Object(OBJ_tdst_GameObject *_pst_Obj, ULONG _ul_Param, BOOL _b_Msg, BOOL _b_ModifSel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_Selection		*pst_Sel;
	long					l_SelItemCount;
	unsigned long			ul_Msg;
	GRO_tdst_Struct			*pst_Gro;
	SEL_tdst_SelectedItem	*pst_SelItem;
	BOOL					b_SelPrefab;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (!_pst_Obj ) return;
	if(!mst_WinHandles.pst_World) return;
	pst_Sel = M_F3D_Sel;

	if(((EOUT_cl_Frame *) mpo_AssociatedEditor)->mst_Ini.ul_SelFlags & EOUT_SelFlags_Prefab)
	{
		((EOUT_cl_Frame *) mpo_AssociatedEditor)->mst_Ini.ul_SelFlags &= ~EOUT_SelFlags_Prefab;
		b_SelPrefab = Selection_b_Prefab(_pst_Obj, _ul_Param, _b_Msg, _b_ModifSel);
		((EOUT_cl_Frame *) mpo_AssociatedEditor)->mst_Ini.ul_SelFlags |= EOUT_SelFlags_Prefab;
		if(b_SelPrefab) return;
	}

	if(SEL_RetrieveItem(pst_Sel, _pst_Obj)) return;

	/* Delete all Cobs already selected */
	if(SEL_l_CountItem(pst_Sel, SEL_C_SIF_Cob))
	{
		pst_Sel->l_Flag &= ~SEL_C_SF_Editable;
		while(pst_SelItem = pst_Sel->pst_FirstItem)
		{
			Selection_Unselect(pst_SelItem->p_Content, pst_SelItem->l_Flag);
			mb_SelectionChange = TRUE;
		}
	}

	if(_b_ModifSel) SEL_pst_AddItem(pst_Sel, _pst_Obj, SEL_C_SIF_Object);

	_pst_Obj->ul_EditorFlags |= OBJ_C_EditFlags_Selected;

	if(mst_WinHandles.pst_DisplayData->uc_EditODE)
	{
		if(_pst_Obj->pst_Base && _pst_Obj->pst_Base->pst_ODE && _pst_Obj->pst_Base->pst_ODE->uc_Type == 2 /*ODE_TYPE_BOX*/)
			LINK_PrintStatusMsg("ODE Edition Mode");
		else
			LINK_PrintStatusMsg("ODE Edition Mode: The selected object doesn't have a ODE Box.");
	}

	if(mst_WinHandles.pst_DisplayData->uc_EditBounding)
	{
		if(!(_pst_Obj->ul_EditorFlags & OBJ_C_EditFlags_ShowAltBV))
		{
			LINK_PrintStatusMsg("AABBox/Sphere Edition Mode");
		}
		else
		{
			LINK_PrintStatusMsg("OBBox Edition Mode");
		}
	}

    // New GAO -> close dialog
    Selection_SubObject_DestroyVertexPos();

	if(Selection_b_IsInSubObjectMode())
	{
		if(!(_pst_Obj->ul_EditorFlags & OBJ_C_EditFlags_NoSubObjectMode)) F3D_BeginSubObjectMode(_pst_Obj);
	}

	l_SelItemCount = SEL_l_CountItem(pst_Sel, SEL_C_SIF_All);

	/* some special code for camera and camera mode */
	pst_Gro = (GRO_tdst_Struct *) OBJ_p_GetGro(_pst_Obj);
	if(pst_Gro && pst_Gro->i->ul_Type == GRO_Camera)
	{
		if(M_F3D_DD->uc_LockObjectToCam)
		{
			M_F3D_DD->pst_EditorCamObject = _pst_Obj;
			CAM_SetCameraMatrix(&M_F3D_DD->st_Camera, OBJ_pst_GetAbsoluteMatrix(_pst_Obj));
		}
		else if(M_F3D_DD->pst_EditorCamObject == NULL)
		{
			M_F3D_DD->pst_EditorCamObject = _pst_Obj;
		}
	}
	else
	{
		M_F3D_DD->uc_LockObjectToCam = 0;
	}

	/* Inform linked editors about selection */
	if(_b_Msg)
	{
		if(mpo_AssociatedEditor)
		{
			/* Add data, or replace it ? */
			ul_Msg = (l_SelItemCount > 1) ? EDI_MESSAGE_ADDSELDATA : EDI_MESSAGE_SELDATA;
			M_MF()->SendMessageToLinks(mpo_AssociatedEditor, ul_Msg, _ul_Param, (ULONG) _pst_Obj);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_Link(WAY_tdst_Link *_pst_Link, ULONG _ul_Param)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_SelItem;
	long					l_SelItemCount;
	unsigned long			ul_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mst_WinHandles.pst_World) return;
	pst_SelItem = SEL_pst_AddItem(M_F3D_Sel, _pst_Link, SEL_C_SIF_Link);
	pst_SelItem->ul_User = _ul_Param;

	l_SelItemCount = SEL_l_CountItem(M_F3D_Sel, SEL_C_SIF_All);
	ul_Msg = (l_SelItemCount > 1) ? EDI_MESSAGE_ADDSELDATA : EDI_MESSAGE_SELDATA;
	M_MF()->SendMessageToLinks(mpo_AssociatedEditor, ul_Msg, 0, (ULONG) _pst_Link);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_Zone(void *_pv_Data, ULONG _ul_Param, ULONG _ul_Flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_Selection		*pst_Sel;
	SEL_tdst_SelectedItem	*pst_SelItem;
	long					l_SelItemCount;
	unsigned long			ul_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mst_WinHandles.pst_World) return;
	pst_Sel = M_F3D_Sel;

	if(SEL_RetrieveItem(pst_Sel, _pv_Data)) return;

	pst_SelItem = SEL_pst_AddItem(pst_Sel, _pv_Data, _ul_Flags);
	pst_SelItem->ul_User = _ul_Param;

	if(_ul_Flags & SEL_C_SIF_ZDx)
		((COL_tdst_ZDx *) _pv_Data)->pst_GO = (OBJ_tdst_GameObject *) _ul_Param;
	else
	{
		/*
		 * GEO_SubObject_UnselAll( (GEO_tdst_Object *) ((COL_tdst_Cob *)
		 * _pv_Data)->p_GeoCob);
		 */
		((COL_tdst_Cob *) _pv_Data)->pst_GO = (OBJ_tdst_GameObject *) _ul_Param;
	}

	l_SelItemCount = SEL_l_CountItem(pst_Sel, SEL_C_SIF_All);
	ul_Msg = (l_SelItemCount > 1) ? EDI_MESSAGE_ADDSELDATA : EDI_MESSAGE_SELDATA;
	M_MF()->SendMessageToLinks(mpo_AssociatedEditor, ul_Msg, 0, (ULONG) _pv_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_Unselect(void *p, long l_Flag, BOOL _b_ModifSel)
{
	if(!mst_WinHandles.pst_World) return;
	if(!p) return;
	if(_b_ModifSel) SEL_DelItem(M_F3D_Sel, p);

	M_MF()->SendMessageToLinks(mpo_AssociatedEditor, EDI_MESSAGE_DELSELDATA, 0, (ULONG) p);

	if(l_Flag & SEL_C_SIF_Object)
	{
		if(Selection_b_IsInSubObjectMode()) F3D_EndSubObjectMode((OBJ_tdst_GameObject *) p);
		((OBJ_tdst_GameObject *) p)->ul_EditorFlags &= ~OBJ_C_EditFlags_Selected;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::Selection_b_IsSelected(void *p, long _l_Flag)
{
    if (!p) return FALSE;

	if(_l_Flag & SEL_C_SIF_Object)
	{
		return((OBJ_tdst_GameObject *) p)->ul_EditorFlags & OBJ_C_EditFlags_Selected;
	}
	else if(_l_Flag & SEL_C_SIF_Link)
	{
		return(SEL_RetrieveItem(M_F3D_Sel, ((WAY_tdst_GraphicLink *) p)->pst_Link) != NULL);
	}
	else if(_l_Flag & (SEL_C_SIF_ZDx | SEL_C_SIF_Cob))
	{
		return
			(
				SEL_RetrieveItem
				(
					M_F3D_Sel,
					(
						mst_WinHandles.pst_DisplayData->st_DisplayedZones.dpst_GraphicZone + (ULONG)
							((GEO_tdst_GraphicZone *) p)
					)->pv_Data
				) != NULL
			);
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SEL_tdst_SelectedItem *F3D_cl_View::Selection_pst_RetrieveItem(void *p, long _l_Flag)
{
	if(_l_Flag & SEL_C_SIF_Link)
		p = ((WAY_tdst_GraphicLink *) p)->pst_Link;
	else if(_l_Flag & (SEL_C_SIF_ZDx | SEL_C_SIF_Cob))
		p = ((GEO_tdst_GraphicZone *) p)->pv_Data;

	return(SEL_RetrieveItem(M_F3D_Sel, p));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_EndBox(CPoint pt, BOOL _b_CanSel, BOOL _b_Add, BOOL _b_Sub)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC								*pDC;
	CPen							o_Pen;
	CPen							*poldpen;
	CRect							o_Rect;
	SOFT_tdst_PickingBuffer			*pst_PB;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	void							*p;
	SEL_tdst_SelectedItem			*pst_Sel, *pst_Next;
	long							l_Value;
	unsigned long					ul_Ext;
	WAY_tdst_GraphicLink			*pst_GLink;
	GEO_tdst_GraphicZone			*pst_Zone;
	MATH_tdst_Vector				A, B;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDC = GetDC();
	pDC->SetROP2(R2_XORPEN);
#ifdef JADEFUSION
	o_Pen.CreatePen(PS_SOLID, 1, GetSysColor(0x00ffffff));
#else
	o_Pen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_ACTIVECAPTION));
#endif
	poldpen = pDC->SelectObject(&o_Pen);
	RECT(pDC, mo_ZoomRect);
	pDC->SelectObject(poldpen);
	DeleteObject(&o_Pen);
	ReleaseDC(pDC);
	mb_SelBoxOn = FALSE;
	if(!_b_CanSel) return;

	if(Selection_b_IsInSubObjectMode())
	{
		A.x = (float) mo_ZoomRect.left;
		B.x = (float) mo_ZoomRect.right;
		A.y = (float) M_F3D_PB->l_Height - mo_ZoomRect.top;
		B.y = (float) M_F3D_PB->l_Height - mo_ZoomRect.bottom;

		/* Philippe -> Add culling for selection begin */
		if (!(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_SelectVisible))
		if ((mst_WinHandles.pst_DisplayData->ul_WiredMode & 3) != 2)
		{
			
			pPickForZCull = mst_WinHandles.pst_DisplayData->pst_PickingBuffer;
			SOFT_SetClipWindow(mst_WinHandles.pst_DisplayData->pst_PickingBuffer, mo_ZoomRect.left, M_F3D_PB->l_Height - mo_ZoomRect.bottom, mo_ZoomRect.right, M_F3D_PB->l_Height - mo_ZoomRect.top);
			SOFT_ResetClipWindow(mst_WinHandles.pst_DisplayData->pst_PickingBuffer);
			mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DoNotRender;
			mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_UsePickingBuffer;
			SOFT_gb_WindowSel = TRUE;
			Refresh();
			SOFT_gb_WindowSel = FALSE;
			mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_UsePickingBuffer;
			SOFT_ResetClipWindow(mst_WinHandles.pst_DisplayData->pst_PickingBuffer);
		}
		/* Philippe -> Add culling for selection end */

		if(Selection_b_SubObject_PickBox(&A, &B))
		{
			Selection_SubObject_Sel(_b_Add | _b_Sub ? 1 : 0, _b_Sub ? 0 : 1);
			Selection_SubObject_CannotSelAll();
			LINK_Refresh();
			pPickForZCull = NULL;
			return;
		}
		pPickForZCull = NULL;
	}

	if(_b_Sub) _b_Add = TRUE;

	l_Value = Pick_l_InBox((POINT *) &mo_ZoomRect.left, (POINT *) &mo_ZoomRect.right, SOFT_Cuc_PBQF_All);

	pst_PB = mst_WinHandles.pst_DisplayData->pst_PickingBuffer;
	pst_Query = &pst_PB->st_Query;

	if(l_Value)
	{
		mpo_CurrentModif = new F3D_cl_Undo_Selection(this);
		mpo_CurrentModif->SetDesc("Selection with box");
		mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
		mpo_CurrentModif = NULL;

		if(!_b_Add)
		{
			pst_Sel = M_F3D_Sel->pst_FirstItem;
			while(pst_Sel)
			{
				pst_Sel->l_Flag |= SEL_C_SIF_Delete;
				pst_Sel = pst_Sel->pst_Next;
			}

			for(l_Value = 0; l_Value < pst_Query->l_Number; l_Value++)
			{
				p = (void *) pst_Query->dst_List[l_Value].ul_Value;
				ul_Ext = pst_Query->dst_List[l_Value].ul_ValueExt;
				pst_Sel = NULL;

				switch(ul_Ext & SOFT_Cul_PBQF_TypeMask)
				{
				case SOFT_Cuc_PBQF_GameObject:
					if(pst_Sel = Selection_pst_RetrieveItem(p, SEL_C_SIF_Object))
						pst_Sel->l_Flag &= ~SEL_C_SIF_Delete;
					break;

				case SOFT_Cuc_PBQF_Link:
					if(pst_Sel = Selection_pst_RetrieveItem(p, SEL_C_SIF_Link)) pst_Sel->l_Flag &= ~SEL_C_SIF_Delete;
					break;

				case SOFT_Cuc_PBQF_Zone:
					pst_Zone = mst_WinHandles.pst_DisplayData->st_DisplayedZones.dpst_GraphicZone + (ULONG) p;
					switch(pst_Zone->uc_Type)
					{
					case GEO_Cul_GraphicZDx:
						if(pst_Sel = Selection_pst_RetrieveItem(pst_Zone, SEL_C_SIF_ZDx))
							pst_Sel->l_Flag &= ~SEL_C_SIF_Delete;
						break;

					case GEO_Cul_GraphicCob:
						if(pst_Sel = Selection_pst_RetrieveItem(pst_Zone, SEL_C_SIF_Cob))
							pst_Sel->l_Flag &= ~SEL_C_SIF_Delete;
						break;
					}
					break;
				}

				if(pst_Sel) pst_Query->dst_List[l_Value].ul_Value = 0;
			}

			pst_Sel = M_F3D_Sel->pst_FirstItem;
			while(pst_Sel)
			{
				pst_Next = pst_Sel->pst_Next;
				if(pst_Sel->l_Flag & SEL_C_SIF_Delete)
				{
					mb_SelectionChange = TRUE;
					Selection_Unselect(pst_Sel->p_Content, pst_Sel->l_Flag);
				}

				pst_Sel = pst_Next;
			}
		}

		for(l_Value = 0; l_Value < pst_Query->l_Number; l_Value++)
		{
			p = (void *) pst_Query->dst_List[l_Value].ul_Value;
			if(p == NULL) continue;
			ul_Ext = pst_Query->dst_List[l_Value].ul_ValueExt;

			switch(ul_Ext & SOFT_Cul_PBQF_TypeMask)
			{
			case SOFT_Cuc_PBQF_GameObject:
				Selection_Object((OBJ_tdst_GameObject *) p, ul_Ext);
				mb_SelectionChange = TRUE;
				break;

			case SOFT_Cuc_PBQF_Link:
				pst_GLink = (WAY_tdst_GraphicLink *) p;
				Selection_Link(pst_GLink->pst_Link, (ULONG) pst_GLink->pst_Origin);
				mb_SelectionChange = TRUE;
				break;

			case SOFT_Cuc_PBQF_Zone:
				pst_Zone = mst_WinHandles.pst_DisplayData->st_DisplayedZones.dpst_GraphicZone + (ULONG) p;
				switch(pst_Zone->uc_Type)
				{
				case GEO_Cul_GraphicZDx:
					Selection_Zone(pst_Zone->pv_Data, (ULONG) pst_Zone->pst_GO, SEL_C_SIF_ZDx);
					mb_SelectionChange = TRUE;
					break;

				case GEO_Cul_GraphicCob:
					Selection_Zone(pst_Zone->pv_Data, (ULONG) pst_Zone->pst_GO, SEL_C_SIF_Cob);
					mb_SelectionChange = TRUE;
					break;
				}
				break;
			}
		}
	}

	Selection_Change();

	if(mpo_AssociatedEditor) mpo_AssociatedEditor->RefreshMenu();
	LINK_Refresh();
}
#ifdef JADEFUSION
extern BOOL EDI_gb_ComputeMap;
extern BOOL LOA_gb_SpeedMode;
extern BOOL	EDI_gb_SlashC ;
#else
extern "C" BOOL EDI_gb_ComputeMap;
extern "C" BOOL LOA_gb_SpeedMode;
extern "C" BOOL	EDI_gb_SlashC ;
#endif
extern BOOL EDI_gb_SlashL;

#ifdef JADEFUSION
/*
 =======================================================================================================================
 =======================================================================================================================
 */

void F3D_cl_View::Selection_HighlightedObjects(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_Selection		*pst_Sel;
	TAB_tdst_PFelem			*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject		*pst_GO;
	ULONG					ul_Count;
	char					sz_Msg[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mst_WinHandles.pst_World) return;
	pst_Sel = M_F3D_Sel;

	ul_Count = 0;
	
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&mst_WinHandles.pst_World->st_AllWorldObjects);

	//loop trought all world objects to clear Highlight flag
	while(pst_PFElem <= pst_PFLastElem)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		pst_PFElem++;
		if(TAB_b_IsAHole(pst_GO)) continue;

		pst_GO->ul_AdditionalFlags &= ~OBJ_C_EdAddFlags_Highlighted;
	}

	//render one frame
	LINK_Refresh();

	//clear actual objects selection
	SEL_DelItems(pst_Sel, (void *) BIG_C_InvalidIndex, SEL_C_SIF_Object);

	//select all highlighted objects
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
	while(pst_PFElem <= pst_PFLastElem)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		pst_PFElem++;
		if(TAB_b_IsAHole(pst_GO)) continue;

		if(pst_GO->ul_AdditionalFlags & OBJ_C_EdAddFlags_Highlighted)
		{
			Selection_Object(pst_GO, SEL_C_SIF_Object);
			++ul_Count;
		}
	}

	if(ul_Count)
	{
		sprintf(sz_Msg, "%d highlighted objects selected.", ul_Count);
		LINK_PrintStatusMsg(sz_Msg);
	}
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */

void F3D_cl_View::Selection_WithDialog(BOOL _b_CanSize)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	BOOL	b_Show;
	BOOL	b_Size;
	/*~~~~~~~~~~~*/

	if(EDI_gb_ComputeMap || LOA_gb_SpeedMode || EDI_gb_SlashC || EDI_gb_SlashL)
	{
		return;
	}

	b_Size = TRUE;
	M_MF()->LockDisplay(GetParent());
	if(!mpo_SelectionDialog)
	{
		mpo_SelectionDialog = new EDIA_cl_SelectionDialog(this);
		mpo_SelectionDialog->Create(MAKEINTRESOURCE(DIALOGS_IDD_SELECTION), GetParent());
		if(mpo_Frame->mb_SelMinimize) mpo_SelectionDialog->ScrollWindow(-WIDTH_MINSCRSEL, 0);
		mb_SelectOn = FALSE;
	}

	b_Show = FALSE;
	if(mb_SelectOn)
	{
		mpo_SelectionDialog->ShowWindow(SW_HIDE);
		if(_b_CanSize)
		{
			GetParent()->GetClientRect(&o_Rect);
			MoveWindow(&o_Rect);
			GetParent()->SetFocus();
		}

		mb_SelectOn = FALSE;

		/* Anim toolbox */
		if(mpo_AnimDialog && mb_AnimOn)
		{
			GetClientRect(&o_Rect);
			mpo_AnimDialog->MoveWindow(o_Rect.left + 5, o_Rect.bottom - 50, o_Rect.Width() - 10, 40);
		}
	}
	else
	{
		if(mb_ToolBoxOn)
		{
			Selection_ToolBox(FALSE);
			b_Size = FALSE;
		}

		mpo_SelectionDialog->UpdateSelectionButton();
		mpo_SelectionDialog->UpdateList();
		mpo_SelectionDialog->ShowWindow(SW_SHOW);
		mb_SelectOn = TRUE;
		GetParent()->SetFocus();
		b_Show = TRUE;
	}

	if(b_Show)
	{
		if(!b_Size) ENG_gb_GlobalLock = TRUE;
		GetParent()->SendMessage(WM_SIZE, 0, 0);
		mpo_SelectionDialog->ShowWindow(SW_SHOW);
	}

	M_MF()->UnlockDisplay(GetParent());
	ENG_gb_GlobalLock = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_ToolBox(BOOL _b_CanSize)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	BOOL	b_Show;
	BOOL	b_Size;
	/*~~~~~~~~~~~*/

	if(EDI_gb_ComputeMap || LOA_gb_SpeedMode)
	{
		return;
	}

	b_Size = TRUE;
	M_MF()->LockDisplay(GetParent());
	if(!mpo_ToolBoxDialog)
	{
		mpo_ToolBoxDialog = new EDIA_cl_ToolBoxDialog(this);
		mpo_ToolBoxDialog->Create(MAKEINTRESOURCE(DIALOGS_IDD_TOOLBOX), GetParent());
		if(mpo_Frame->mb_SelMinimize) mpo_ToolBoxDialog->ScrollWindow(-WIDTH_MINSCRSEL, 0);
		mb_ToolBoxOn = FALSE;
		if(LINK_gb_AllRefreshEnable) mpo_ToolBoxDialog->SelectGaoFromPtr(Selection_pst_GetFirstObject());
	}

	b_Show = FALSE;
	if(mb_ToolBoxOn)
	{
		mpo_ToolBoxDialog->ShowWindow(SW_HIDE);
		if(_b_CanSize)
		{
			GetParent()->GetClientRect(&o_Rect);
			MoveWindow(&o_Rect);
			GetParent()->SetFocus();
		}

		/* Anim toolbox */
		if(mpo_AnimDialog && mb_AnimOn)
		{
			GetClientRect(&o_Rect);
			mpo_AnimDialog->MoveWindow(o_Rect.left + 5, o_Rect.bottom - 50, o_Rect.Width() - 10, 40);
		}

		mb_ToolBoxOn = FALSE;
	}
	else
	{
		if(mb_SelectOn)
		{
			Selection_WithDialog(FALSE);
			b_Size = FALSE;
		}

		mpo_ToolBoxDialog->UpdateSelectionButton();
		mpo_ToolBoxDialog->FirstDraw();
		mpo_ToolBoxDialog->ShowWindow(SW_SHOW);
		mb_ToolBoxOn = TRUE;
		GetParent()->SetFocus();
		b_Show = TRUE;
	}

	if(b_Show)
	{
		if(!b_Size) ENG_gb_GlobalLock = TRUE;
		GetParent()->SendMessage(WM_SIZE, 0, 0);
		mpo_ToolBoxDialog->ShowWindow(SW_SHOW);
	}

	M_MF()->UnlockDisplay(GetParent());
	ENG_gb_GlobalLock = FALSE;
}
#ifdef JADEFUSION
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_LightReject(BOOL _b_CanSize)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	BOOL	b_Show;
	BOOL	b_Size;
	/*~~~~~~~~~~~*/

	b_Size = TRUE;
	M_MF()->LockDisplay(GetParent());
	if(!mpo_LightRejectDialog)
	{
		mpo_LightRejectDialog = new EDIA_cl_LightRejectDialog(this);
		mpo_LightRejectDialog->Create(MAKEINTRESOURCE(DIALOGS_IDD_LIGHTREJECT), GetParent());
		mb_LightRejectOn = FALSE;
	}

	b_Show = FALSE;
	if(mb_LightRejectOn)
	{
		mpo_LightRejectDialog->ShowWindow(SW_HIDE);
		if(_b_CanSize)
		{
			GetParent()->GetClientRect(&o_Rect);
			MoveWindow(&o_Rect);
			GetParent()->SetFocus();
		}

		mb_LightRejectOn = FALSE;
	}
	else
	{
		mpo_LightRejectDialog->ShowWindow(SW_SHOW);
		mb_LightRejectOn = TRUE;
		GetParent()->SetFocus();
		b_Show = TRUE;
	}

	if(b_Show)
	{
		mpo_LightRejectDialog->ShowWindow(SW_SHOW);
	}

	GetParent()->SendMessage(WM_SIZE, 0, 0);

	M_MF()->UnlockDisplay(GetParent());
	ENG_gb_GlobalLock = FALSE;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::Selection_b_Treat(void *p, long _l_Param, long _l_Flag, BOOL _b_Xor, BOOL _b_Force)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_Selection		*pst_Sel;
	SEL_tdst_SelectedItem	*pst_SelItem;
	void					*p_Del, *p_Add;
	BOOL					b_DelAll;
	unsigned long			ul_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * blindage si selection (selection suite à un F6 par exemple ) en cours de
	 * modification
	 */

	/*$F a revoir car empeche les delete 
    if ( mpo_CurrentModif )
    {
        mpo_CurrentModif->b_Undo();
		delete mpo_CurrentModif;
		mpo_CurrentModif = NULL;
        MATH_InitVectorToZero(&mst_Move);
		Helper_Reset();
    }
    */
	if(!mst_WinHandles.pst_World) return FALSE;
	pst_Sel = M_F3D_Sel;
	b_DelAll = 0;

	if(_b_Xor)
	{
		b_DelAll = 0;

		if(p && (SEL_RetrieveItem(pst_Sel, p)))
		{
			if(_b_Force) return TRUE;
			p_Del = p;
			p_Add = NULL;
		}
		else
		{
			p_Del = NULL;
			p_Add = p;
		}
	}
	else
	{
		b_DelAll = SEL_b_IsEmpty(pst_Sel) ? 0 : 1;
		p_Del = NULL;
		p_Add = p;
	}

	/* Add undo for selection operation */
	if(p_Del || b_DelAll || p_Add)
	{
		mpo_CurrentModif = new F3D_cl_Undo_Selection(this);
		mpo_CurrentModif->SetDesc("Selection");
		mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
		mpo_CurrentModif = NULL;
	}
	else
	{
		return FALSE;
	}

	/* Delete eventually an object */
	ul_Msg = EDI_MESSAGE_DELSELDATA;

	if(p_Del)
	{
		pst_Sel->l_Flag &= ~SEL_C_SF_Editable;
		Selection_Unselect(p_Del, _l_Flag);
		mb_SelectionChange = TRUE;
	}

	/* Delete all object already selected */
	if(b_DelAll)
	{
		pst_Sel->l_Flag &= ~SEL_C_SF_Editable;
		while(pst_SelItem = pst_Sel->pst_FirstItem)
		{
			Selection_Unselect(pst_SelItem->p_Content, pst_SelItem->l_Flag);
			mb_SelectionChange = TRUE;
		}
	}

	/* Add object */
	if(p_Add)
	{
		if(_l_Flag & SEL_C_SIF_Link)
		{
			Selection_Link((WAY_tdst_Link *) p_Add, _l_Param);
			mb_SelectionChange = TRUE;
		}
		else if(_l_Flag & (SEL_C_SIF_ZDx | SEL_C_SIF_Cob))
		{
			Selection_Zone(p_Add, _l_Param, _l_Flag);
			mb_SelectionChange = TRUE;
		}
		else if(_l_Flag & SEL_C_SIF_Object)
		{
			Selection_Object((OBJ_tdst_GameObject *) p_Add, _l_Param);
			mb_SelectionChange = TRUE;
		}
		else if(_l_Flag & SEL_C_SIF_HieLink)
		{
			SEL_pst_AddItem(M_F3D_Sel, p_Add, SEL_C_SIF_HieLink);
			mb_SelectionChange = TRUE;
		}
	}

	Selection_Change();

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Selection_Change(void)
{
	if(!mb_SelectionChange) return;

	if(LINK_gb_AllRefreshEnable)
	{
		if(mpo_ToolBoxDialog) mpo_ToolBoxDialog->SelectGaoFromPtr(Selection_pst_GetFirstObject());
		if(mpo_CreateGeometryDialog) mpo_CreateGeometryDialog->mpo_View->Update_GameObject();
		if(mpo_GaoInfoDialog) mpo_GaoInfoDialog->ChangeGao(Selection_pst_GetFirstObject());
#ifdef JADEFUSION
		if(mpo_ReplaceDialog) mpo_ReplaceDialog->UpdateTrees();
		if(mpo_CubeMapGenDialog) mpo_CubeMapGenDialog->ChangeGao(Selection_pst_GetFirstObject());
#endif
		if(mpo_BrickMapper)
		{
			mpo_BrickMapper->FillAmbience();
			mpo_BrickMapper->FillAlternative();
		}
	}

	mb_SelectionChange = FALSE;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::SpeedSelect(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_ListDialog	o_Dlg(EDIA_List_SpeedSelectGAO);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_Dlg.mpo_View = this;
	o_Dlg.mpst_World = mst_WinHandles.pst_World;
	o_Dlg.DoModal();
	LINK_Refresh();
}
#endif /* ACTIVE_EDITORS */
