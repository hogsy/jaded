/*$T F3Dview_undo.cpp GC! 1.081 05/23/01 15:56:49 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "F3Dframe/F3Dview_undo.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKtoed.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "SOFT/SOFThelper.h"
#include "SELection/SELection.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEOobjectaccess.h"
#include "GEOmetric/GEO_SKIN.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#include "ENGine/Sources/Objects/OBJconst.h"
#include "ENGine/Sources/Objects/OBJorient.h"
#include "ENGine/Sources/Objects/OBJsave.h"
#include "ENGine/Sources/Objects/OBJinit.h"
#include "ENGine/Sources/Objects/OBJload.h"
#include "ENGine/Sources/Objects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJBoundingvolume.h"
#include "ENGine/Sources/Objects/OBJslowaccess.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "ENGine/Sources/GRId/GRI_compute.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/sources/WORld/WORupdate.h"
#include "ENGine/sources/WORld/WORuniverse.h"
#include "ENGine/Sources/WAYs/WAYsave.h"
#include "ENGine/Sources/WAYs/WAYinit.h"
#include "ENGine/Sources/WAYs/WAY.h"
#include "ENGine/Sources/INTersection/INTSnP.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"
#include "DIAlogs/DIA_SKN_dlg.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAtoolbox_groview.h"

#include "BASe/MEMory/MEM.h"

#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    Selection enumeration functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_Undo_AdressToKey(SEL_tdst_Selection *_pst_Sel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_CurSel;
	BIG_KEY					ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurSel = _pst_Sel->pst_FirstItem;
	while(pst_CurSel)
	{
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_CurSel->p_Content);

		if(ul_Key != BIG_C_InvalidKey)  
        {
            pst_CurSel->p_Content = (void *) ul_Key;
            pst_CurSel->l_Flag |= SEL_C_SIF_ContentIsAKey;
        }
		pst_CurSel = pst_CurSel->pst_Next;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *F3D_Undo_OneKeyToAdress(ULONG ul_Value)
{
	if(ul_Value == BIG_C_InvalidKey) return NULL;

	ul_Value = BIG_ul_SearchKeyToFat(ul_Value);
	if(ul_Value == BIG_C_InvalidIndex) return NULL;

	ul_Value = LOA_ul_SearchAddress(BIG_PosFile(ul_Value));
	ERR_X_Assert(ul_Value != BIG_C_InvalidIndex);

	return (void *) ul_Value;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_Undo_KeyToAdress(SEL_tdst_Selection *_pst_Sel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_CurSel;
	unsigned long			ul_Value;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurSel = _pst_Sel->pst_FirstItem;
	while(pst_CurSel)
	{
        if (pst_CurSel->l_Flag & SEL_C_SIF_ContentIsAKey)
        {
		    ul_Value = (BIG_KEY) pst_CurSel->p_Content;
		    pst_CurSel->p_Content = F3D_Undo_OneKeyToAdress(ul_Value );
            pst_CurSel->l_Flag &= ~SEL_C_SIF_ContentIsAKey;
        }
		pst_CurSel = pst_CurSel->pst_Next;
	}
}

/*$4
 ***********************************************************************************************************************
    Class F3D_cl_Undo_HelperModif
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNAMIC(F3D_cl_Undo_HelperModif, CObject)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_Undo_HelperModif_SaveData(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_cl_Undo_HelperModif *po_Undo;
	OBJ_tdst_GameObject		*pst_GO;
	F3D_HelperModif_Data	*pst_Data;
	GEO_tdst_Object			*pst_Geo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;


	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			if(!((COL_tdst_Cob *) _pst_Sel->p_Content)) 
			{
				MessageBox(NULL, "No Pointer-Key link. UNDO cannot work", TEXT("SAVE and retry"), MB_OK | MB_ICONWARNING | MB_TASKMODAL | MB_SETFOREGROUND);
				return TRUE;
			}

			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			pst_Geo = (GEO_tdst_Object *)((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob;
		}
	}


	po_Undo = (F3D_cl_Undo_HelperModif *) _ul_Param1;
	pst_Data = &po_Undo->mdst_Data[po_Undo->mi_Count++];

	pst_Data->dst_Point = NULL;

	if(!pst_GO) return TRUE;

	if(OBJ_b_IsChild(pst_GO))
		MATH_CopyMatrix(&pst_Data->st_Matrix, &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
	else
		MATH_CopyMatrix(&pst_Data->st_Matrix, OBJ_pst_GetAbsoluteMatrix(pst_GO));

	if(po_Undo->mb_SavePoint)
	{
		if(pst_Geo && (pst_Geo->st_Id.i->ul_Type == GRO_Geometric))
		{
			pst_Data->dst_Point = (GEO_Vertex *) MEM_p_Alloc(pst_Geo->l_NbPoints * sizeof(GEO_Vertex));
			L_memcpy(pst_Data->dst_Point, pst_Geo->dst_Point, pst_Geo->l_NbPoints * sizeof(GEO_Vertex));
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_Undo_HelperModif_Swap(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_cl_Undo_HelperModif *po_Undo;
	OBJ_tdst_GameObject		*pst_GO;
	F3D_HelperModif_Data	*pst_Data;
	MATH_tdst_Matrix		*pst_Matrix, st_Matrix;
	GEO_Vertex				*pul_Save;
	GEO_tdst_Object			*pst_Geo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
		pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			if(!((COL_tdst_Cob *) _pst_Sel->p_Content)) 
			{
				MessageBox(NULL, "No Pointer-Key link. UNDO cannot work", TEXT("SAVE and retry"), MB_OK | MB_ICONWARNING | MB_TASKMODAL | MB_SETFOREGROUND);
				return TRUE;
			}

			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			pst_Geo = (GEO_tdst_Object *)((COL_tdst_Cob *) _pst_Sel->p_Content)->p_GeoCob;
		}
	}

	po_Undo = (F3D_cl_Undo_HelperModif *) _ul_Param1;
	pst_Data = &po_Undo->mdst_Data[po_Undo->mi_Count++];

	if(!pst_GO) return TRUE;

	/* matrix */
	if(OBJ_b_IsChild(pst_GO))
		pst_Matrix = &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix;
	else
		pst_Matrix = OBJ_pst_GetAbsoluteMatrix(pst_GO);
	MATH_CopyMatrix(&st_Matrix, &pst_Data->st_Matrix);
	MATH_CopyMatrix(&pst_Data->st_Matrix, pst_Matrix);
	MATH_CopyMatrix(pst_Matrix, &st_Matrix);

	/* vertex */
	if(pst_Data->dst_Point)
	{
		if(pst_Geo && (pst_Geo->st_Id.i->ul_Type == GRO_Geometric))
		{
			pul_Save = pst_Geo->dst_Point;
			pst_Geo->dst_Point = pst_Data->dst_Point;
			pst_Data->dst_Point = pul_Save;
		}

		GEO_SubObject_BuildFaceData(pst_Geo);
        OBJ_ComputeBV( pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_HelperModif::F3D_cl_Undo_HelperModif(F3D_cl_View *_pst_View, BOOL _b_SavePt, BOOL _b_Locked) :
	EDI_cl_UndoModif(_b_Locked)
{
	mpst_View = _pst_View;
	mb_SavePoint = _b_SavePt;

	SEL_Duplicate(&mst_Selection, _pst_View->mst_WinHandles.pst_World->pst_Selection);
	mi_Number = SEL_l_CountItem(&mst_Selection, SEL_C_SIF_Object | SEL_C_SIF_Cob);
	if(mi_Number)
		mdst_Data = (F3D_HelperModif_Data *) L_malloc(mi_Number * sizeof(F3D_HelperModif_Data));
	else
		mdst_Data = NULL;

	mi_Count = 0;
	SEL_EnumItem(&mst_Selection, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Undo_HelperModif_SaveData, (ULONG) this, 0);
	F3D_Undo_AdressToKey(&mst_Selection);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_HelperModif::~F3D_cl_Undo_HelperModif(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(mdst_Data)
	{
		for(i = 0; i < mi_Number; i++)
		{
			if(mdst_Data[i].dst_Point) MEM_Free(mdst_Data[i].dst_Point);
		}

		L_free(mdst_Data);
	}

	SEL_Close(&mst_Selection);
}

/*
 =======================================================================================================================
    void F3D_cl_Undo_HelperModif::SetDescription(char *_sz_Desc) { if(strlen(_sz_Desc) > 120) _sz_Desc[120] = 0;
    sprintf(msz_Description, "[undo] %s", _sz_Desc); }
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_HelperModif::b_Do(void)
{
	F3D_Undo_KeyToAdress(&mst_Selection);
	mi_Count = 0;
	SEL_EnumItem(&mst_Selection, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Undo_HelperModif_Swap, (ULONG) this, 0);
	F3D_Undo_AdressToKey(&mst_Selection);

	/* LINK_PrintStatusMsg(msz_Description + 7); */
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_HelperModif::b_Undo(void)
{
	F3D_Undo_KeyToAdress(&mst_Selection);
	mi_Count = 0;
	SEL_EnumItem(&mst_Selection, SEL_C_SIF_Object | SEL_C_SIF_Cob, F3D_b_Undo_HelperModif_Swap, (ULONG) this, 0);
	F3D_Undo_AdressToKey(&mst_Selection);

	/* LINK_PrintStatusMsg(msz_Description); */
	return TRUE;
}

/*$4
 ***********************************************************************************************************************
    Class F3D_cl_Undo_Selection
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNAMIC(F3D_cl_Undo_Selection, CObject)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_Undo_Select(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	F3D_cl_View			*pst_View;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	pst_View = (F3D_cl_View *) _ul_Param1;
	if(pst_View)
		pst_View->Selection_Object(pst_GO, SEL_C_SIF_Object, TRUE, FALSE);
	else
		pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_Selected;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_Undo_Unselect(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	F3D_cl_View			*pst_View;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	pst_View = (F3D_cl_View *) _ul_Param1;
	if(pst_View)
		pst_View->Selection_Unselect(pst_GO, SEL_C_SIF_Object, FALSE);
	else
		pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_Selected;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_Selection::F3D_cl_Undo_Selection(F3D_cl_View *_pst_View, BOOL _b_Locked) :
	EDI_cl_UndoModif(_b_Locked)
{
	mpst_View = _pst_View;
	SEL_Duplicate(&mst_Selection, _pst_View->mst_WinHandles.pst_World->pst_Selection);
	F3D_Undo_AdressToKey(&mst_Selection);
	SEL_DelItems(&mst_Selection, (void *) BIG_C_InvalidIndex, SEL_C_SIF_Object);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_Selection::~F3D_cl_Undo_Selection(void)
{
	SEL_Close(&mst_Selection);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_Undo_Selection::SwapSelection(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_SwapItem, *pst_Old, *pst_New;
	int						i_NbNew, i_New;
	char					*dc_New;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_NbNew = SEL_l_CountItem(&mst_Selection, SEL_C_SIF_All);
	dc_New = (char *) L_malloc(i_NbNew);
	L_memset(dc_New, 1, i_NbNew);

	/* first parse all current selection element and look through new selection */

	/* if we find them, we leave them else they are unselected */
	pst_Old = mpst_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
	while(pst_Old)
	{
		pst_New = mst_Selection.pst_FirstItem;
		i_New = 0;
		while(pst_New)
		{
			if(pst_New->p_Content == pst_Old->p_Content)
			{
				/* an old element is still selected do nothing */
				dc_New[i_New] = 0;
				break;
			}

			pst_New = pst_New->pst_Next;
			i_New++;
		}

		/* if element is not found in new selection it is unselected */
		if(!pst_New) F3D_b_Undo_Unselect(pst_Old, (ULONG) mpst_View, 0);

		pst_Old = pst_Old->pst_Next;
	}

	/* parse new list and select all element that weren't in previous selection */
	pst_New = mst_Selection.pst_FirstItem;
	i_New = 0;
	while(pst_New)
	{
		if(dc_New[i_New]) F3D_b_Undo_Select(pst_New, (ULONG) mpst_View, 0);

		pst_New = pst_New->pst_Next;
		i_New++;
	}

	L_free(dc_New);

	pst_SwapItem = mst_Selection.pst_FirstItem;
	mst_Selection.pst_FirstItem = mpst_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
	mpst_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem = pst_SwapItem;

	pst_SwapItem = mst_Selection.pst_LastItem;
	mst_Selection.pst_LastItem = mpst_View->mst_WinHandles.pst_World->pst_Selection->pst_LastItem;
	mpst_View->mst_WinHandles.pst_World->pst_Selection->pst_LastItem = pst_SwapItem;

	mpst_View->mb_SelectionChange = TRUE;
	mpst_View->Selection_Change();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_Selection::b_Do(void)
{
	F3D_Undo_KeyToAdress(&mst_Selection);
	SwapSelection();
	F3D_Undo_AdressToKey(&mst_Selection);
	SEL_DelItems(&mst_Selection, (void *) BIG_C_InvalidIndex, SEL_C_SIF_Object);
	LINK_PrintStatusMsg("Select");
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_Selection::b_Undo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION			x_Prev;
	EDI_cl_UndoModif	*po_Prev;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	F3D_Undo_KeyToAdress(&mst_Selection);
	SwapSelection();
	F3D_Undo_AdressToKey(&mst_Selection);
	SEL_DelItems(&mst_Selection, (void *) BIG_C_InvalidIndex, SEL_C_SIF_Object);
	LINK_PrintStatusMsg("[undo] Select");

	/* restablish eventually vertex selection */
	if(mpst_View->mo_UndoManager.mx_PrevPosition)
	{
		x_Prev = mpst_View->mo_UndoManager.mx_PrevPosition;
		mpst_View->mo_UndoManager.mo_ListOfModifs.GetPrev(x_Prev);
		if(x_Prev)
		{
			po_Prev = mpst_View->mo_UndoManager.mo_ListOfModifs.GetAt(x_Prev);

			if(po_Prev->IsKindOf(RUNTIME_CLASS(F3D_cl_Undo_SelSubObject))) po_Prev->b_Do();
		}
	}

	return TRUE;
}

/*$4
 ***********************************************************************************************************************
    Class F3D_cl_Undo_Destruction
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNAMIC(F3D_cl_Undo_Destruction, CObject)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_Undo_GetPosition(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Matrix	**ppst_Matrix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	ppst_Matrix = (MATH_tdst_Matrix **) _ul_Param1;

	if(!pst_GO) return TRUE;

	if(OBJ_b_IsChild(pst_GO))
		MATH_CopyMatrix((*ppst_Matrix)++, &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
	else
		MATH_CopyMatrix((*ppst_Matrix)++, OBJ_pst_GetAbsoluteMatrix(pst_GO));

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_Destruction::F3D_cl_Undo_Destruction(F3D_cl_View *_pst_View, BOOL _b_Locked) :
	EDI_cl_UndoModif(_b_Locked)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	long				l_Number;
	MATH_tdst_Matrix	*pst_Matrix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mpst_View = _pst_View;
	SEL_Duplicate(&mst_Selection, _pst_View->mst_WinHandles.pst_World->pst_Selection);

	l_Number = SEL_l_CountItem(&mst_Selection, SEL_C_SIF_Object);
	if(l_Number)
	{
		mdst_Matrix = (MATH_tdst_Matrix *) MEM_p_Alloc(l_Number * sizeof(MATH_tdst_Matrix));
		pst_Matrix = mdst_Matrix;
		SEL_EnumItem(&mst_Selection, SEL_C_SIF_Object, F3D_b_Undo_GetPosition, (ULONG) & pst_Matrix, 0);
	}
	else
		mdst_Matrix = NULL;

	l_Number = SEL_l_CountItem(&mst_Selection, SEL_C_SIF_Link);
	if(l_Number)
		mdul_LinkDest = (ULONG *) MEM_p_Alloc(l_Number * sizeof(ULONG));
	else
		mdul_LinkDest = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_Destruction::~F3D_cl_Undo_Destruction(void)
{
	if(mdst_Matrix) MEM_Free(mdst_Matrix);
	if(mdul_LinkDest) MEM_Free(mdul_LinkDest);
	SEL_Close(&mst_Selection);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_Destruction::b_Do(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		*pst_GO, *pst_DestGO;
	WOR_tdst_World			*pst_World;
	SEL_tdst_SelectedItem	*pst_CurSel;
	BIG_KEY					ul_Key;
	char					sz_Text[256];
	WAY_tdst_LinkList		*pst_List;
	WAY_tdst_Link			*pst_Link;
	WAY_tdst_Struct			*pst_Links;
	unsigned long			*pul_GODest;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_PrintStatusMsg("Delete Game objects :");

	pst_World = mpst_View->mst_WinHandles.pst_World;
	pst_CurSel = mst_Selection.pst_FirstItem;
	pul_GODest = mdul_LinkDest;

	while(pst_CurSel)
	{
		if(pst_CurSel->l_Flag & SEL_C_SIF_Link)
		{
			pst_Link = (WAY_tdst_Link *) pst_CurSel->p_Content;
			pst_GO = (OBJ_tdst_GameObject *) pst_CurSel->ul_User;
			pst_List = WAY_pst_SearchOneLinkList(pst_GO, pst_Link);
			if(!pst_List) break;
			if(pst_List->pst_Network && pst_List->pst_Network->ul_Flags & WAY_C_HasBeenMerge)
			{
				ERR_X_Warning(0, "Can't delete one link cause network has been merge", NULL);
				pst_CurSel = pst_CurSel->pst_Next;
				continue;
			}

			ERR_X_Assert(pst_List);

			pst_Links = (WAY_tdst_Struct *) pst_GO->pst_Extended->pst_Links;

			if(pst_List->ul_Num == 1)
				pst_CurSel->p_Content = (void *) - 1;
			else
				pst_CurSel->p_Content = (void *) (pst_List - pst_Links->pst_AllLinks);
			pst_CurSel->ul_User = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
			*pul_GODest++ = LOA_ul_SearchKeyWithAddress((ULONG) pst_Link->pst_Next);

			WAY_DelOneLink(pst_GO, pst_List, pst_Link->pst_Next);
			WAY_CheckObjectInNetwork(pst_GO);
			WAY_CheckObjectInNetwork(pst_Link->pst_Next);
			pst_CurSel = pst_CurSel->pst_Next;
			mpst_View->Selection_Unselect(pst_Link, SEL_C_SIF_Link);

			/* Check objects */
			WAY_CheckObjectInNetwork(pst_GO);
			ERR_gb_Warning = FALSE;
		}
		else
		{
			pst_CurSel = pst_CurSel->pst_Next;
		}
	}

	pst_CurSel = mst_Selection.pst_FirstItem;
	while(pst_CurSel)
	{
		if(pst_CurSel->l_Flag & SEL_C_SIF_HieLink)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_CurSel->p_Content;
			pst_DestGO = pst_GO->pst_Base->pst_Hierarchy->pst_Father;

			if
			(
				(pst_GO && pst_GO->c_FixFlags & OBJ_C_HasBeenMerge)
			&&	(pst_DestGO && pst_DestGO->c_FixFlags & OBJ_C_HasBeenMerge)
			)
			{
				ERR_X_Warning(0, "Can't delete one hiearchical link cause it has been merge", NULL);
				pst_CurSel = pst_CurSel->pst_Next;
				continue;
			}

			pst_CurSel->ul_User = LOA_ul_SearchKeyWithAddress((ULONG) pst_DestGO);
			pst_CurSel->p_Content = (void *) LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);

			OBJ_ChangeIdentityFlags
			(
				pst_GO,
				pst_GO->ul_IdentityFlags &~OBJ_C_IdentityFlag_Hierarchy,
				pst_GO->ul_IdentityFlags
			);
			pst_DestGO->ul_StatusAndControlFlags &= ~OBJ_C_StatusFlag_HasChild;

			pst_CurSel = pst_CurSel->pst_Next;
			mpst_View->Selection_Unselect(pst_GO, SEL_C_SIF_HieLink);
		}
		else
		{
			pst_CurSel = pst_CurSel->pst_Next;
		}
	}

	pst_CurSel = mst_Selection.pst_FirstItem;
	while(pst_CurSel)
	{
		if(pst_CurSel->l_Flag & SEL_C_SIF_Object)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_CurSel->p_Content;
			if(pst_GO->c_FixFlags & OBJ_C_HasBeenMerge)
			{
				sprintf(sz_Text, "Can't delete object %s cause it has been merge", pst_GO->sz_Name);
				ERR_X_Warning(0, sz_Text, NULL);
				pst_CurSel = pst_CurSel->pst_Next;
				continue;
			}

			sprintf(sz_Text, ".    %s", pst_GO->sz_Name);
			LINK_PrintStatusMsg(sz_Text);

			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
			pst_CurSel->p_Content = (void *) ul_Key;

			OBJ_WhenDestroyAnObject(pst_World, pst_GO);
			WOR_World_DetachObject(pst_World, pst_GO);
			INT_SnP_DetachObject(pst_GO, pst_World);
			OBJ_ul_GameObject_Save(pst_World, pst_GO, NULL);
			OBJ_GameObject_Remove(pst_GO, 1);
		}

		pst_CurSel = pst_CurSel->pst_Next;
	}

	SEL_Close(pst_World->pst_Selection);
	SEL_DelItems(&mst_Selection, (void *) BIG_C_InvalidIndex, SEL_C_SIF_Object);

	LINK_UpdatePointers();
	LINK_Refresh();
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_Destruction::b_Undo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		*pst_GO, *pst_DestGO;
	WOR_tdst_World			*pst_World;
	SEL_tdst_SelectedItem	*pst_CurSel;
	BIG_KEY					ul_Key;
	BIG_INDEX				ul_Fat;
	char					sz_Text[128];
	MATH_tdst_Matrix		*pst_Matrix, M, M1;
	ULONG					*pul_LinkDest;
	WAY_tdst_LinkList		*pst_List;
	WAY_tdst_Link			*pst_Link;
	WAY_tdst_Struct			*pst_Links;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_PrintStatusMsg("[Undo] delete game objects :");

	pst_World = mpst_View->mst_WinHandles.pst_World;
	pst_CurSel = mst_Selection.pst_FirstItem;
	pst_Matrix = mdst_Matrix;
	pul_LinkDest = mdul_LinkDest;

	while(pst_CurSel)
	{
		if(pst_CurSel->l_Flag & SEL_C_SIF_Object)
		{
			ul_Key = (BIG_KEY) pst_CurSel->p_Content;
			ERR_X_Assert(ul_Key != BIG_C_InvalidIndex);

			ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
			if(ul_Fat == BIG_C_InvalidIndex)
			{
				pst_CurSel = pst_CurSel->pst_Next;
				continue;
			}

			pst_GO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(ul_Fat));
			ERR_X_Assert(pst_GO == (OBJ_tdst_GameObject *) BIG_C_InvalidIndex);

			/* Load the game object */
			WOR_gpst_WorldToLoadIn = pst_World;
			LOA_MakeFileRef(ul_Key, (ULONG *) &pst_GO, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
			LOA_Resolve();
			ERR_X_Assert(pst_GO != NULL);
			WOR_World_JustAfterLoadObject(pst_World, pst_GO, TRUE, TRUE);

			pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_Selected;
			MATH_CopyMatrix(OBJ_pst_GetAbsoluteMatrix(pst_GO), pst_Matrix);

			sprintf(sz_Text, ".    %s", pst_GO->sz_Name);
			LINK_PrintStatusMsg(sz_Text);

			pst_CurSel->p_Content = (void *) pst_GO;
			pst_Matrix++;
		}

		pst_CurSel = pst_CurSel->pst_Next;
	}

	pst_CurSel = mst_Selection.pst_FirstItem;
	while(pst_CurSel)
	{
		if(pst_CurSel->l_Flag & SEL_C_SIF_HieLink)
		{
			ul_Key = (BIG_KEY) pst_CurSel->p_Content;
			ERR_X_Assert(ul_Key != BIG_C_InvalidIndex);
			ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
			if(ul_Fat == BIG_C_InvalidIndex)
			{
				pst_CurSel = pst_CurSel->pst_Next;
				continue;
			}

			pst_GO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(ul_Fat));
			ERR_X_Assert(pst_GO != (OBJ_tdst_GameObject *) BIG_C_InvalidIndex);

			ul_Key = (BIG_KEY) pst_CurSel->ul_User;
			ERR_X_Assert(ul_Key != BIG_C_InvalidIndex);
			ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
			ERR_X_Assert(ul_Fat != BIG_C_InvalidIndex);
			pst_DestGO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(ul_Fat));
			ERR_X_Assert(pst_DestGO != (OBJ_tdst_GameObject *) BIG_C_InvalidIndex);

			ul_Key = pst_GO->ul_IdentityFlags;
			OBJ_ChangeIdentityFlags(pst_GO, ul_Key | OBJ_C_IdentityFlag_Hierarchy, ul_Key);
			pst_DestGO->ul_StatusAndControlFlags |= OBJ_C_StatusFlag_HasChild;
			pst_GO->pst_Base->pst_Hierarchy->pst_Father = pst_DestGO;
			MATH_InvertMatrix(&M, OBJ_pst_GetAbsoluteMatrix(pst_DestGO));
			pst_Matrix = OBJ_pst_GetAbsoluteMatrix(pst_GO);
			MATH_MulMatrixMatrix(&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix, pst_Matrix, &M);
			pst_CurSel->p_Content = (void *) pst_GO;
			pst_CurSel->ul_User = 0;

			/* Restore initial pos */
			if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_HasInitialPos)
			{
				MATH_InvertMatrix(&M, OBJ_pst_GetAbsoluteMatrix(pst_DestGO));
				MATH_CopyMatrix(&M1, OBJ_pst_GetInitialAbsoluteMatrix(pst_GO));
				MATH_MulMatrixMatrix(OBJ_pst_GetInitialAbsoluteMatrix(pst_GO), &M1, &M);
			}
		}

		pst_CurSel = pst_CurSel->pst_Next;
	}

	pst_CurSel = mst_Selection.pst_FirstItem;
	while(pst_CurSel)
	{
		if(pst_CurSel->l_Flag & SEL_C_SIF_Link)
		{
			ul_Key = (BIG_KEY) pst_CurSel->ul_User;
			ERR_X_Assert(ul_Key != BIG_C_InvalidIndex);
			ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
			if(ul_Fat == BIG_C_InvalidIndex)
			{
				pst_CurSel = pst_CurSel->pst_Next;
				continue;
			}

			pst_GO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(ul_Fat));
			ERR_X_Assert(pst_GO != (OBJ_tdst_GameObject *) BIG_C_InvalidIndex);

			ul_Key = (BIG_KEY) * pul_LinkDest;
			ERR_X_Assert(ul_Key != BIG_C_InvalidIndex);
			ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
			ERR_X_Assert(ul_Fat != BIG_C_InvalidIndex);
			pst_DestGO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(ul_Fat));
			ERR_X_Assert(pst_DestGO != (OBJ_tdst_GameObject *) BIG_C_InvalidIndex);

			pst_Links = (WAY_tdst_Struct *) pst_GO->pst_Extended->pst_Links;
			if(!pst_Links)
			{
			}

			if(pst_Links)
			{
				if((long) pst_CurSel->p_Content == -1)
				{
					pst_List = pst_Links->pst_AllLinks;
				}
				else
				{
					pst_List = pst_Links->pst_AllLinks + (long) pst_CurSel->p_Content;
				}
			}

			pst_Link = WAY_pst_AddOneLink(pst_GO, pst_List, pst_DestGO);
			pst_CurSel->p_Content = (void *) pst_Link;
			pst_CurSel->ul_User = (ULONG) pst_GO;
			pul_LinkDest++;

			/* Check objects */
			WAY_CheckObjectInNetwork(pst_GO);
			ERR_gb_Warning = FALSE;
		}

		pst_CurSel = pst_CurSel->pst_Next;
	}

	SEL_Duplicate(pst_World->pst_Selection, &mst_Selection);

	LINK_Refresh();
	LINK_UpdatePointers();
	return TRUE;
}

/*$4
 ***********************************************************************************************************************
    Class F3D_cl_Undo_UVMoving
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNAMIC(F3D_cl_Undo_UVMoving, CObject)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_UVMoving::F3D_cl_Undo_UVMoving
(
	F3D_cl_View		*_pst_View,
    int             _i_NbObj, 
	GEO_tdst_Object **_ppst_Obj,
	GEO_tdst_UV		**_ppst_UV,
	BOOL			_b_Locked
) :
	EDI_cl_UndoModif(_b_Locked)
{
    int obj;

	mpst_View = _pst_View;
    mi_NbObj = _i_NbObj;
    for (obj = 0; obj < mi_NbObj; obj++)
    {
	    mul_ObjKey[obj] = LOA_ul_SearchKeyWithAddress((ULONG) _ppst_Obj[obj]);
	    mpst_UV[obj] = _ppst_UV[obj];
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_UVMoving::~F3D_cl_Undo_UVMoving(void)
{
    int obj;

    for (obj = 0; obj < mi_NbObj; obj++)
	    if(mpst_UV[obj]) MEM_Free(mpst_UV[obj]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_UVMoving::b_Do(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
    int             obj;
	GEO_tdst_Object *pst_Obj;
	GEO_tdst_UV		*pst_UV;
	/*~~~~~~~~~~~~~~~~~~~~~*/

    for (obj = 0; obj < mi_NbObj; obj++)
    {
	    if(!mpst_UV[ obj ]) continue;

	    pst_Obj = (GEO_tdst_Object *) F3D_Undo_OneKeyToAdress(mul_ObjKey[ obj ]);
	    if(pst_Obj)
	    {
		    pst_UV = pst_Obj->dst_UV;
		    pst_Obj->dst_UV = mpst_UV[ obj ];
		    mpst_UV[ obj ] = pst_UV;
		    LINK_PrintStatusMsg("UV position change");

            if (pst_Obj->pst_SubObject)
	    	    if(pst_Obj->pst_SubObject->pfn_UVMapper_Update)
		    	    pst_Obj->pst_SubObject->pfn_UVMapper_Update(pst_Obj->pst_SubObject->ul_UVMapper_Param);
        }
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_UVMoving::b_Undo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
    int             obj;
	GEO_tdst_Object *pst_Obj;
	GEO_tdst_UV		*pst_UV;
	/*~~~~~~~~~~~~~~~~~~~~~*/

    for (obj = 0; obj < mi_NbObj; obj++)
    {
	    if(!mpst_UV[ obj ]) continue;

	    pst_Obj = (GEO_tdst_Object *) F3D_Undo_OneKeyToAdress(mul_ObjKey[ obj ]);
	    if(pst_Obj)
	    {
		    pst_UV = pst_Obj->dst_UV;
		    pst_Obj->dst_UV = mpst_UV[ obj ];
		    mpst_UV[ obj ] = pst_UV;
		    LINK_PrintStatusMsg("[Undo] UV position change");

            if (pst_Obj->pst_SubObject)
		        if(pst_Obj->pst_SubObject->pfn_UVMapper_Update)
			        pst_Obj->pst_SubObject->pfn_UVMapper_Update(pst_Obj->pst_SubObject->ul_UVMapper_Param);
	    }
    }

	return TRUE;
}

/*$4
 ***********************************************************************************************************************
    Class F3D_cl_Undo_TurnEdge
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNAMIC(F3D_cl_Undo_TurnEdge, CObject)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_TurnEdge::F3D_cl_Undo_TurnEdge
(
	F3D_cl_View						*_pst_View,
	GEO_tdst_Object					*_pst_Obj,
	GEO_tdst_SubObject_ETurnData	*_pst_Data
) :
	EDI_cl_UndoModif(TRUE)
{
	mpst_View = _pst_View;
	mpst_Obj = _pst_Obj;
	mai_Old[0] = _pst_Data->aul_Old[0];
	mai_Old[1] = _pst_Data->aul_Old[1];
	mai_New[0] = _pst_Data->aul_New[0];
	mai_New[1] = _pst_Data->aul_New[1];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_TurnEdge::~F3D_cl_Undo_TurnEdge(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_TurnEdge::b_Do(void)
{
	/*~~~~~~~*/
	int i_Edge;
	/*~~~~~~~*/

	if(mpst_Obj->pst_SubObject)
	{
		i_Edge = GEO_i_SubObject_EFind(mpst_Obj, mai_Old[0], mai_Old[1]);
		if(i_Edge != -1) GEO_SubObject_ETurn(mpst_Obj, mpst_Obj->pst_SubObject->dst_Edge + i_Edge, NULL);
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_TurnEdge::b_Undo(void)
{
	/*~~~~~~~*/
	int i_Edge;
	/*~~~~~~~*/

	if(mpst_Obj->pst_SubObject)
	{
		i_Edge = GEO_i_SubObject_EFind(mpst_Obj, mai_New[0], mai_New[1]);
		if(i_Edge != -1) GEO_SubObject_ETurn(mpst_Obj, mpst_Obj->pst_SubObject->dst_Edge + i_Edge, NULL);
	}

	return TRUE;
}

/*$4
 ***********************************************************************************************************************
    Class F3D_cl_Undo_FlipFace
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNAMIC(F3D_cl_Undo_FlipFace, CObject)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_FlipFace::F3D_cl_Undo_FlipFace(F3D_cl_View *_pst_View, BOOL _b_Locked) :
	EDI_cl_UndoModif(_b_Locked)
{
	mpst_View = _pst_View;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_FlipFace::~F3D_cl_Undo_FlipFace(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_FlipFace::b_Do(void)
{
	mpst_View->Selection_SubObject_FlipNormals(FALSE, FALSE );
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_FlipFace::b_Undo(void)
{
	return b_Do();
}

/*$4
 ***********************************************************************************************************************
    Class F3D_cl_Undo_CutEdge
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNAMIC(F3D_cl_Undo_CutEdge, CObject)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_CutEdge::F3D_cl_Undo_CutEdge
(
	F3D_cl_View					*_pst_View,
	GEO_tdst_Object				*_pst_Obj,
	GEO_tdst_SubObject_ECutData *_pst_Data
) :
	EDI_cl_UndoModif(TRUE)
{
	mpst_View = _pst_View;
	mpst_Obj = _pst_Obj;
	L_memcpy(&mst_CutData, _pst_Data, sizeof(GEO_tdst_SubObject_ECutData));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_CutEdge::~F3D_cl_Undo_CutEdge(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_CutEdge::b_Do(void)
{
	/*~~~~~~~*/
	int i_Edge;
	/*~~~~~~~*/

	if(mpst_Obj->pst_SubObject)
	{
		i_Edge = GEO_i_SubObject_EFind(mpst_Obj, mst_CutData.al_EdgeVertex[0], mst_CutData.al_EdgeVertex[1]);
		if(i_Edge != -1)
		{
			GEO_i_SubObject_ECut(mpst_Obj, mpst_Obj->pst_SubObject->dst_Edge + i_Edge, mst_CutData.f_Cut, &mst_CutData);
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_CutEdge::b_Undo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int									i_Edge, i, j, i_To, i_From;
	GEO_tdst_SubObject_Edge				*pst_Edge;
	GEO_tdst_IndexedTriangle			*pst_Tri[2];
	int									i_DelTri[2];
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	WOR_tdst_Update_RLI					st_UpdateRLIData;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_DelTri[0] = i_DelTri[1] = -1;

	if(mpst_Obj->pst_SubObject)
	{
		for(i = 0; i < 2; i++)
		{
			i_Edge = GEO_i_SubObject_EFind(mpst_Obj, mst_CutData.al_OppVertex[i], mst_CutData.l_AddVertex);
			if(i_Edge == -1) continue;
			pst_Edge = mpst_Obj->pst_SubObject->dst_Edge + i_Edge;
			pst_Tri[0] = GEO_SubObject_GetTriangleFromEdgeIndex(mpst_Obj, pst_Edge->ul_Triangle[0]);
			pst_Tri[1] = GEO_SubObject_GetTriangleFromEdgeIndex(mpst_Obj, pst_Edge->ul_Triangle[1]);

			for(j = 0; j < 3; j++)
			{
				if(pst_Tri[0]->auw_Index[j] == mst_CutData.l_AddVertex) i_To = j;
				if
				(
					(pst_Tri[1]->auw_Index[j] == mst_CutData.al_EdgeVertex[0])
				||	(pst_Tri[1]->auw_Index[j] == mst_CutData.al_EdgeVertex[1])
				) i_From = j;
			}

			/* restore triangle 0 */
			pst_Tri[0]->auw_Index[i_To] = pst_Tri[1]->auw_Index[i_From];
			if(mpst_Obj->dst_UV) pst_Tri[0]->auw_UV[i_To] = pst_Tri[1]->auw_UV[i_From];

			/* save triangle 1 index to delete it */
			i_DelTri[i] = pst_Edge->ul_Triangle[1];
		}

		/* delete triangles */
		if(i_DelTri[0] < i_DelTri[1])
		{
			j = i_DelTri[0];
			i_DelTri[0] = i_DelTri[1];
			i_DelTri[1] = j;
		}

		for(i = 0; i < 2; i++)
		{
			if(i_DelTri[i] == -1) continue;
			pst_Element = mpst_Obj->dst_Element + ((i_DelTri[i] >> 16) & 0xFFF);
			GEO_DelTriangle(pst_Element, i_DelTri[i] & 0xFFFF);
		}

        /* update RLI */
		st_UpdateRLIData.p_Geo = mpst_Obj;
		st_UpdateRLIData.l_Op = WOR_Update_RLI_Del;
		st_UpdateRLIData.l_Ind0 = mst_CutData.l_AddVertex;
		st_UpdateRLIData.l_Ind1 = mpst_Obj->l_NbPoints + 2;
		WOR_Universe_Update_RLI(&st_UpdateRLIData);

        /* update eventually skin */
        GEO_SKN_UpdateAfterVertexEdition( mpst_Obj, &st_UpdateRLIData);

        /* delete added point */
		GEO_DelPoint(mpst_Obj, mst_CutData.l_AddVertex);

		/* delete added UV */
		GEO_DelUV(mpst_Obj, mst_CutData.al_AddUV[0]);
		GEO_DelUV(mpst_Obj, mst_CutData.al_AddUV[1]);

		/* recompute SubObject Data */
		mpst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
		GEO_SubObject_BuildEdgeData(mpst_Obj);
		GEO_SubObject_BuildFaceData(mpst_Obj);
	}

	return TRUE;
}

/*$4
 ***********************************************************************************************************************
    Class F3D_cl_Undo_SelSubObject
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNAMIC(F3D_cl_Undo_SelSubObject, CObject)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_SelSubObject::F3D_cl_Undo_SelSubObject(F3D_cl_View *_pst_View, int _i_Type, BOOL _b_Locked) :
	EDI_cl_UndoModif(_b_Locked)
{
	mpst_View = _pst_View;
	mpst_Data = NULL;
	mi_Type = _i_Type;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_SelSubObject::~F3D_cl_Undo_SelSubObject(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_tdst_SubObjectSel	*pst_Next;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	while(mpst_Data)
	{
		pst_Next = mpst_Data->pst_Next;
		if(mpst_Data->l_NewNumber) L_free(mpst_Data->duw_NewSel);
		if(mpst_Data->l_OldNumber) L_free(mpst_Data->duw_OldSel);
		L_free(mpst_Data);
		mpst_Data = pst_Next;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_SelSubObject::b_Do(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_tdst_SubObjectSel	*pst_Struct;
	GEO_tdst_Object			*pst_Geo;
	ULONG					ul_Value;
	char					*pc_Sel;
	LONG					l_Number;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_View->Selection_b_IsInSubObjectMode()) mpst_View->Selection_BeginSubObjectMode();

	F3D_ul_SubObject_SelType = mi_Type;
	for(pst_Struct = mpst_Data; pst_Struct; pst_Struct = pst_Struct->pst_Next)
	{
		if((ULONG) pst_Struct->pst_Obj == BIG_C_InvalidKey) continue;
		ul_Value = BIG_ul_SearchKeyToFat((ULONG) pst_Struct->pst_Obj);
		if(ul_Value == BIG_C_InvalidIndex) continue;
		pst_Geo = (GEO_tdst_Object *) LOA_ul_SearchAddress(BIG_PosFile(ul_Value));
		if(!pst_Geo) continue;

		pc_Sel = GEO_pc_SubObject_GetSelData(pst_Geo, mi_Type, &l_Number);
		if(!pc_Sel) continue;

		for(i = 0; i < l_Number; i++) pc_Sel[i] = 0;

		for(i = 0; i < pst_Struct->l_NewNumber; i++) pc_Sel[pst_Struct->duw_NewSel[i]] = 1;

		F3D_ul_SubObject_SelType = mi_Type;
		F3D_b_SubObject_Sel((SEL_tdst_SelectedItem *) pst_Geo, 0, 0);
		GEO_SubObject_HideUpdate( pst_Geo, TRUE );
	}

	LINK_PrintStatusMsg("Select (Sub object)");
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_SelSubObject::b_Undo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_tdst_SubObjectSel	*pst_Struct;
	GEO_tdst_Object			*pst_Geo;
	ULONG					ul_Value;
	char					*pc_Sel;
	LONG					l_Number;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_View->Selection_b_IsInSubObjectMode()) mpst_View->Selection_BeginSubObjectMode();

	F3D_ul_SubObject_SelType = mi_Type;
	for(pst_Struct = mpst_Data; pst_Struct; pst_Struct = pst_Struct->pst_Next)
	{
		if((ULONG) pst_Struct->pst_Obj == BIG_C_InvalidKey) continue;
		ul_Value = BIG_ul_SearchKeyToFat((ULONG) pst_Struct->pst_Obj);
		if(ul_Value == BIG_C_InvalidIndex) continue;
		pst_Geo = (GEO_tdst_Object *) LOA_ul_SearchAddress(BIG_PosFile(ul_Value));
		if(!pst_Geo) continue;

		pc_Sel = GEO_pc_SubObject_GetSelData(pst_Geo, mi_Type, &l_Number);
		if(!pc_Sel) continue;

		for(i = 0; i < l_Number; i++) pc_Sel[i] = 0;

		for(i = 0; i < pst_Struct->l_OldNumber; i++) pc_Sel[pst_Struct->duw_OldSel[i]] = 1;

		F3D_ul_SubObject_SelType = mi_Type;
		F3D_b_SubObject_Sel((SEL_tdst_SelectedItem *) pst_Geo, 0, 0);
		GEO_SubObject_HideUpdate( pst_Geo, TRUE );
	}

	LINK_PrintStatusMsg("[undo] Select (Sub object)");
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_tdst_SubObjectSel *F3D_cl_Undo_SelSubObject::pst_GetStruct(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_tdst_SubObjectSel	*pst_Struct;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Struct = mpst_Data;
	while(pst_Struct)
	{
		if(pst_Struct->pst_Obj == _pst_Obj) return pst_Struct;
		pst_Struct = pst_Struct->pst_Next;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_Undo_SelSubObject::BeginAddObj(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_tdst_SubObjectSel	*pst_Struct;
	char					*pc_Sel;
	LONG					l_Number, l_Count;
	int						i;
	USHORT					*duw_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Struct = pst_GetStruct(_pst_Obj);
	if(pst_Struct) return;

	pc_Sel = GEO_pc_SubObject_GetSelData(_pst_Obj, mi_Type, &l_Number);

	/* count number and get index of selection */
	l_Count = 0;
	duw_Sel = (USHORT *) L_malloc(2 * l_Number);
	for(i = 0; i < l_Number; i++)
		if(pc_Sel[i] & 1) duw_Sel[l_Count++] = i;

	if(l_Count == 0)
	{
		L_free(duw_Sel);
		duw_Sel = NULL;
	}
	else if(l_Count != l_Number) duw_Sel = (USHORT *) L_realloc(duw_Sel, 2 * l_Count);

	pst_Struct = (F3D_tdst_SubObjectSel *) L_malloc(sizeof(F3D_tdst_SubObjectSel));
	pst_Struct->pst_Obj = _pst_Obj;
	pst_Struct->pst_Next = NULL;
	pst_Struct->l_OldNumber = l_Count;
	pst_Struct->duw_OldSel = duw_Sel;

	/* add struct in list */
	pst_Struct->pst_Next = mpst_Data;
	mpst_Data = pst_Struct;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_Undo_SelSubObject::EndAddObj(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_tdst_SubObjectSel	*pst_Struct, *pst_Prev;
	ULONG					ul_Key;
	USHORT					*duw_Sel;
	char					*pc_Sel;
	LONG					l_Number, l_Count;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Struct = pst_GetStruct(_pst_Obj);
	if(!pst_Struct) return;

	pc_Sel = GEO_pc_SubObject_GetSelData(_pst_Obj, mi_Type, &l_Number);

	/* count number and get index of selection */
	l_Count = 0;
	duw_Sel = (USHORT *) L_malloc(2 * l_Number);
	for(i = 0; i < l_Number; i++)
		if(pc_Sel[i] & 1) duw_Sel[l_Count++] = i;

	if((l_Count == 0) && (pst_Struct->l_OldNumber == 0))
	{
		if(mpst_Data == pst_Struct)
		{
			mpst_Data = pst_Struct->pst_Next;
		}
		else
		{
			pst_Prev = mpst_Data;
			while(pst_Prev->pst_Next != pst_Struct) pst_Prev = pst_Prev->pst_Next;
			pst_Prev->pst_Next = pst_Struct->pst_Next;
		}

		L_free(pst_Struct);
		L_free(duw_Sel);
		return;
	}

	if(l_Count == 0)
	{
		L_free(duw_Sel);
		duw_Sel = NULL;
	}
	else if(l_Count != l_Number) duw_Sel = (USHORT *) L_realloc(duw_Sel, 2 * l_Count);

	pst_Struct->l_NewNumber = l_Count;
	pst_Struct->duw_NewSel = duw_Sel;

	ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Struct->pst_Obj);
	pst_Struct->pst_Obj = (GEO_tdst_Object *) ul_Key;
}

/*$4
 ***********************************************************************************************************************
    Class F3D_cl_Undo_GeoModif
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNAMIC(F3D_cl_Undo_GeoModif, CObject)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
F3D_cl_Undo_GeoModif::F3D_cl_Undo_GeoModif(F3D_cl_View *_pst_View, GEO_tdst_Object *_pst_Obj, OBJ_tdst_GameObject *_pst_Gao, BOOL _b_Locked) :
	EDI_cl_UndoModif(_b_Locked), mpst_GO(_pst_Gao)
#else
		F3D_cl_Undo_GeoModif::F3D_cl_Undo_GeoModif(F3D_cl_View *_pst_View, GEO_tdst_Object *_pst_Obj, OBJ_tdst_GameObject *_pst_Gao, BOOL _b_Locked) :
	EDI_cl_UndoModif(_b_Locked)
#endif
{
    ULONG                       *pul_RLI;
	GEO_tdst_Object	            *pst_Geo;
    GEO_tdst_ModifierMorphing	*pst_Morph;

	mpst_View = _pst_View;
	mul_ObjKey = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Obj);

	// ColMap Edition Detection.
	if((ULONG) _pst_Obj == (ULONG) _pst_Gao)
		pst_Geo = (GEO_tdst_Object *) ((COL_tdst_Base *)_pst_Gao->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->p_GeoCob;
	else
		pst_Geo = _pst_Obj;

	mpst_Obj = (GEO_tdst_Object *) pst_Geo->st_Id.i->pfnp_Duplicate(pst_Geo, NULL, NULL, 0);
	mpc_Sel = (char *) L_malloc(pst_Geo->l_NbPoints);
	if(pst_Geo->pst_SubObject) L_memcpy(mpc_Sel, pst_Geo->pst_SubObject->dc_VSel, pst_Geo->l_NbPoints);

    if (_pst_Gao && _pst_Gao->pst_Base && _pst_Gao->pst_Base->pst_Visu && _pst_Gao->pst_Base->pst_Visu->dul_VertexColors)
        pul_RLI = _pst_Gao->pst_Base->pst_Visu->dul_VertexColors;
    else
        pul_RLI = NULL;
    pst_Morph = GEO_pst_ModifierMorphing_Get( _pst_Gao );

    mpul_GaoRLI = NULL; 
    mpst_GaoMorphData = NULL;
    ml_GaoNbMorphData = 0;

    
    if (pul_RLI || pst_Morph)
    {
        mul_GaoKey = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Gao);
        if (pul_RLI)
        {
            mpul_GaoRLI = (ULONG *) MEM_p_Alloc( (pul_RLI[0] + 1) * 4 );
            L_memcpy(mpul_GaoRLI, pul_RLI, (pul_RLI[0] + 1) * 4 );
        }
        if (pst_Morph)
        {
            mpst_GaoMorphData = GEO_ModifierMorphing_DuplicateData( pst_Morph );
            ml_GaoNbMorphData = pst_Morph->l_NbMorphData;
        }
    }
    else
    {
		// ColMap Edition
		if(_pst_Gao && ((ULONG) _pst_Obj == (ULONG) _pst_Gao))
			mul_GaoKey = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Gao);
		else
			mul_GaoKey = BIG_C_InvalidKey;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_GeoModif::~F3D_cl_Undo_GeoModif(void)
{
	if(mpst_Obj) mpst_Obj->st_Id.i->pfn_Destroy(mpst_Obj);
	if(mpc_Sel) L_free(mpc_Sel);
    if(mpul_GaoRLI) MEM_Free( mpul_GaoRLI );
    if(mpst_GaoMorphData) GEO_ModifierMorphing_DestroyData( (GEO_tdst_ModifierMorphing_Data *) mpst_GaoMorphData, ml_GaoNbMorphData );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_GeoModif::b_Do(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_Gao;
	GEO_tdst_Object *pst_Obj;
	GEO_tdst_Object st_Obj;
	char			*pc_Save;
    LONG            l_ID;
    ULONG           *pul_Save, ul_Save;
    GEO_tdst_ModifierMorphing	*pst_Morph;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_Obj) return FALSE;

	pst_Obj = (GEO_tdst_Object *) F3D_Undo_OneKeyToAdress(mul_ObjKey);
	if(!pst_Obj)
	{
		return FALSE;
	}

	// ColMap Edition Detection
	if(mul_ObjKey == mul_GaoKey)
	{
		pst_Gao = (OBJ_tdst_GameObject *) F3D_Undo_OneKeyToAdress( mul_GaoKey );
		if(pst_Gao && pst_Gao->pst_Extended && pst_Gao->pst_Extended->pst_Col && ((COL_tdst_Base *) pst_Gao->pst_Extended->pst_Col)->pst_ColMap)
		{
			pst_Obj = (GEO_tdst_Object *)((COL_tdst_Base *) pst_Gao->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->p_GeoCob;
		}
	}

    l_ID = sizeof( GRO_tdst_Struct );
	L_memcpy(&st_Obj, ((char*) pst_Obj) + l_ID, sizeof(GEO_tdst_Object) - l_ID);
	L_memcpy(((char*) pst_Obj) + l_ID, ((char *) mpst_Obj) + l_ID, sizeof(GEO_tdst_Object) - l_ID);
	L_memcpy(((char *) mpst_Obj) + l_ID, &st_Obj, sizeof(GEO_tdst_Object) - l_ID);
	pst_Obj->pst_SubObject = mpst_Obj->pst_SubObject;
	mpst_Obj->pst_SubObject = NULL;

	if(pst_Obj->pst_SubObject)
	{
		pc_Save = pst_Obj->pst_SubObject->dc_VSel;
		pst_Obj->pst_SubObject->dc_VSel = mpc_Sel;
		mpc_Sel = pc_Save;

		if(pst_Obj->pst_SubObject->pfn_UVMapper_Update)
			pst_Obj->pst_SubObject->pfn_UVMapper_Update(pst_Obj->pst_SubObject->ul_UVMapper_Param);

        
        
        GEO_SubObject_BuildFaceData(pst_Obj);
	    GEO_SubObject_BuildEdgeData(pst_Obj);
	    GEO_SubObject_VUpdateESel(pst_Obj);
	    GEO_SubObject_VUpdateFSel(pst_Obj);
        GEO_SubObject_AllocBuffers(pst_Obj);
        GEO_SubObject_HideUpdate( pst_Obj, TRUE );
	}

    pst_Gao = (OBJ_tdst_GameObject *) F3D_Undo_OneKeyToAdress( mul_GaoKey );
    if (pst_Gao && pst_Gao->pst_Base && pst_Gao->pst_Base->pst_Visu)
    {
        pul_Save = pst_Gao->pst_Base->pst_Visu->dul_VertexColors;
        OBJ_VertexColor_ChangePointer( pst_Gao, mpul_GaoRLI, 0 );
        mpul_GaoRLI = pul_Save;

        OBJ_ComputeBV( pst_Gao, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);
    }
    if ( mpst_GaoMorphData && (pst_Morph = GEO_pst_ModifierMorphing_Get( pst_Gao )) )
    {
        pul_Save = (ULONG *) pst_Morph->dst_MorphData;
        pst_Morph->dst_MorphData = (GEO_tdst_ModifierMorphing_Data *) mpst_GaoMorphData;
        mpst_GaoMorphData = (void *) pul_Save;

        ul_Save = pst_Morph->l_NbMorphData; 
        pst_Morph->l_NbMorphData = ml_GaoNbMorphData;
        ml_GaoNbMorphData = ul_Save;
    }

    if ( (mpst_View->mpo_ToolBoxDialog) && GEO_SKN_IsSkinned( pst_Obj ) )
    {
        /*~~~~~~~~~~~~~~~~~~~~~*/
        EDIA_cl_SKN_Dialog	*ClSKN;
        EDIA_cl_ToolBox_GROView *po_Tool;
        extern void DIA_SKN_SelctionCLNK(GEO_tdst_Object *, void *);
        /*~~~~~~~~~~~~~~~~~~~~~*/

        pst_Obj->p_SKN_Objectponderation->ClassPtr = NULL;
        pst_Obj->p_SKN_Objectponderation->SelectionCLBK = NULL; 

        if (mpst_View->mpo_ToolBoxDialog)
        {
            po_Tool = (EDIA_cl_ToolBox_GROView *) mpst_View->mpo_ToolBoxDialog->mpo_ToolView[0];
            if ( (po_Tool) && (po_Tool->mpst_SkinDialog) && (po_Tool->mpst_SkinDialog->mpst_Skn_Gro[0] == pst_Obj) )
            {
                pst_Obj->p_SKN_Objectponderation->ClassPtr = po_Tool->mpst_SkinDialog;
                pst_Obj->p_SKN_Objectponderation->SelectionCLBK = DIA_SKN_SelctionCLNK;
            }
        }

        if ( pst_Obj->p_SKN_Objectponderation->ClassPtr )
        {
            ClSKN = (EDIA_cl_SKN_Dialog *) pst_Obj->p_SKN_Objectponderation->ClassPtr;
            GEO_SKN_UpdateSkinSelection( pst_Obj );
        }
    }


	// ColMap Edition Detection
	if(mul_ObjKey == mul_GaoKey)
	{
		pst_Gao = (OBJ_tdst_GameObject *) F3D_Undo_OneKeyToAdress( mul_GaoKey );
		if(pst_Gao && pst_Gao->pst_Extended && pst_Gao->pst_Extended->pst_Col && ((COL_tdst_Base *) pst_Gao->pst_Extended->pst_Col)->pst_ColMap)
		{
			if(((COL_tdst_Base *) pst_Gao->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0])
			{
				((COL_tdst_Base *) pst_Gao->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uc_Flag |= COL_C_Cob_Updated;
				COL_SynchronizeCob( ((COL_tdst_Base *) pst_Gao->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0], FALSE,FALSE);
			}
		}
	}

#if defined(_XENON_RENDER)
    if (mpst_GO && mpst_GO->pst_Base && mpst_GO->pst_Base->pst_Visu)
    {
        GEO_ResetXenonMesh(mpst_GO, mpst_GO->pst_Base->pst_Visu, pst_Obj, FALSE, FALSE, FALSE);
    }
#endif

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_GeoModif::b_Undo(void)
{
	return b_Do();
}

/*$4
 ***********************************************************************************************************************
    Class F3D_cl_Undo_GeoModif
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNAMIC(F3D_cl_Undo_MultipleGeoModif, CObject)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_Undo_BuildGeoModif(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_cl_Undo_MultipleGeoModif	*pcl_Undo;
	OBJ_tdst_GameObject				*pst_GO;
	GEO_tdst_Object					*pst_Geo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pcl_Undo = (F3D_cl_Undo_MultipleGeoModif *) _ul_Param1;

	pst_Geo = NULL;

	if(_pst_Sel->l_Flag & SEL_C_SIF_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
        pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	}
	else
	{
		if(_pst_Sel->l_Flag & SEL_C_SIF_Cob)
		{
			if(!((COL_tdst_Cob *) _pst_Sel->p_Content)) 
			{
				MessageBox(NULL, "No Pointer-Key link. UNDO cannot work", TEXT("SAVE and retry"), MB_OK | MB_ICONWARNING | MB_TASKMODAL | MB_SETFOREGROUND);
				return TRUE;
			}

			pst_GO = ((COL_tdst_Cob *) _pst_Sel->p_Content)->pst_GO;
			pst_Geo = (GEO_tdst_Object *) pst_GO;
		}
		else
			return TRUE;
	}


	if(pst_Geo)
		pcl_Undo->mdpcl_GeoModif[pcl_Undo->ml_Count++] = new F3D_cl_Undo_GeoModif(pcl_Undo->mpst_View, pst_Geo, pst_GO );
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_MultipleGeoModif::F3D_cl_Undo_MultipleGeoModif(F3D_cl_View *_pst_View, BOOL _b_Locked) :
	EDI_cl_UndoModif(_b_Locked)
{
	mpst_View = _pst_View;

	ml_Number = SEL_l_CountItem(_pst_View->mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object | SEL_C_SIF_Cob);
	if(!ml_Number)
	{
		mdpcl_GeoModif = NULL;
		return;
	}

	mdpcl_GeoModif = (F3D_cl_Undo_GeoModif **) L_malloc(ml_Number * sizeof(F3D_cl_Undo_GeoModif *));
	L_memset(mdpcl_GeoModif, 0, ml_Number * sizeof(F3D_cl_Undo_GeoModif *));

	ml_Count = 0;
	SEL_EnumItem
	(
		_pst_View->mst_WinHandles.pst_World->pst_Selection,
		SEL_C_SIF_Object | SEL_C_SIF_Cob,
		F3D_b_Undo_BuildGeoModif,
		(ULONG) this,
		0
	);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_MultipleGeoModif::~F3D_cl_Undo_MultipleGeoModif(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(mdpcl_GeoModif)
	{
		for(i = 0; i < ml_Number; i++)
		{
			if(mdpcl_GeoModif[i]) delete(mdpcl_GeoModif[i]);
		}

		L_free(mdpcl_GeoModif);
		mdpcl_GeoModif = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_MultipleGeoModif::b_Do(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(mdpcl_GeoModif)
	{
		for(i = 0; i < ml_Number; i++)
		{
			if(mdpcl_GeoModif[i]) mdpcl_GeoModif[i]->b_Do();
		}
	}

	LINK_PrintStatusMsg("Geometry change");
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Undo_MultipleGeoModif::b_Undo(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(mdpcl_GeoModif)
	{
		for(i = 0; i < ml_Number; i++)
		{
			if(mdpcl_GeoModif[i]) mdpcl_GeoModif[i]->b_Do();
		}
	}

#if defined(_XENON_RENDER)
    mpst_View->Selection_XenonRefresh();
#endif

	LINK_PrintStatusMsg("[Undo] Geometry change");
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *F3D_cl_Undo_MultipleGeoModif::GetOldGeo( GEO_tdst_Object *_pst_Obj, void **morph )
{
	/*~~*/
	int i;
    ULONG   ul_Key;
	/*~~*/

    if(!mdpcl_GeoModif) return NULL;

    ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Obj);
    if (ul_Key == BIG_C_InvalidKey) return NULL;
	
	for(i = 0; i < ml_Number; i++)
	{
		if( !mdpcl_GeoModif[i] ) continue;
        if (mdpcl_GeoModif[i]->mul_ObjKey == ul_Key)
		{
			*morph = mdpcl_GeoModif[i]->mpst_GaoMorphData;
            return mdpcl_GeoModif[i]->mpst_Obj;
		}
	}
    return NULL;
}

IMPLEMENT_DYNAMIC(F3D_cl_Undo_EditGrid, CObject)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_EditGrid::F3D_cl_Undo_EditGrid(F3D_cl_View *_pst_View, GRID_tdst_World *pst_Grid, int XMin, int YMin, int XMax, int YMax, char *ac_NewCapa)
{
	/*~~~~~~~~~~~~*/
	int i, j, k, t;
	/*~~~~~~~~~~~~*/

	mpst_Grid = pst_Grid;
	mc_PreviousCapa = NULL;
	mc_NewCapa = NULL;

	if(!pst_Grid || !ac_NewCapa) return;

	mc_PreviousCapa = (char *) MEM_p_Alloc((XMax - XMin + 1) * (YMax - YMin + 1));
	mc_NewCapa = (char *) MEM_p_Alloc((XMax - XMin + 1) * (YMax - YMin + 1));

	L_memcpy(mc_NewCapa, ac_NewCapa, (XMax - XMin + 1) * (YMax - YMin + 1));

	k = 0;
	for(i = XMin; i <= XMax; i++)
	{
		for(j = YMin; j <= YMax; j++)
		{
			t = (int) pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup;
			t = t * j + i;

			mc_PreviousCapa[k++] = pst_Grid->pst_EditArray[t].c_Capacities;
		}
	}

	mi_XMin = XMin;
	mi_XMax = XMax;
	mi_YMin = YMin;
	mi_YMax = YMax;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Undo_EditGrid::~F3D_cl_Undo_EditGrid()
{

	if(!mpst_Grid) return;

	if(mc_PreviousCapa)
	{
		MEM_Free(mc_PreviousCapa);
		mc_PreviousCapa = NULL;
	}

	if(mc_NewCapa)
	{
		MEM_Free(mc_NewCapa);
		mc_NewCapa = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int F3D_cl_Undo_EditGrid::b_Do()
{
	/*~~~~~~~~~~~~*/
	int i, j, k, t;
	int	x1, y1;
	/*~~~~~~~~~~~~*/

	k = 0;
	for(i = mi_XMin; i <= mi_XMax; i++)
	{
		for(j = mi_YMin; j <= mi_YMax; j++)
		{
			/* Paint real time array */
			x1 = i;
			y1 = j;
			if(GRI_ConvertTo2DReal(mpst_Grid, &x1, &y1))
			{
				t = (int) mpst_Grid->w_NumRealGroupsX * mpst_Grid->c_SizeGroup;
				t = t * y1 + x1;
				mpst_Grid->pst_RealArray[t].c_Capacities = (char) mc_NewCapa[k];
			}

			/* Editor array */
			t = (int) mpst_Grid->w_NumGroupsX * mpst_Grid->c_SizeGroup;
			t = t * j + i;

			mpst_Grid->pst_EditArray[t].c_Capacities = mc_NewCapa[k++];
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int F3D_cl_Undo_EditGrid::b_Undo()
{
	/*~~~~~~~~~~~~*/
	int i, j, k, t;
	int	x1, y1;
	/*~~~~~~~~~~~~*/

	k = 0;
	for(i = mi_XMin; i <= mi_XMax; i++)
	{
		for(j = mi_YMin; j <= mi_YMax; j++)
		{
			/* Paint real time array */
			x1 = i;
			y1 = j;
			if(GRI_ConvertTo2DReal(mpst_Grid, &x1, &y1))
			{
				t = (int) mpst_Grid->w_NumRealGroupsX * mpst_Grid->c_SizeGroup;
				t = t * y1 + x1;
				mpst_Grid->pst_RealArray[t].c_Capacities = (char) mc_PreviousCapa[k];
			}

			/* Editor array */
			t = (int) mpst_Grid->w_NumGroupsX * mpst_Grid->c_SizeGroup;
			t = t * j + i;

			mpst_Grid->pst_EditArray[t].c_Capacities = mc_PreviousCapa[k++];
		}
	}

	return TRUE;
}


#endif
