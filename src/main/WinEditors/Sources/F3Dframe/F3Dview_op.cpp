/*$T F3Dview_op.cpp GC! 1.081 02/01/01 10:35:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "INOut/INOkeyboard.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "TABles/TABles.h"

#include "CAMera/CAMera.h"

#include "EDIapp.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "EDIpaths.h"
#include "LINKs/LINKtoed.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJsave.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORexporttomad.h"
#include "ENGine/Sources/WAYs/WAYinit.h"
#include "GEOmetric/GEOload.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "GEOmetric/GEO_MRM.h"
#include "GEOmetric/GEO_SKIN.h"
#include "F3Dframe/F3Dview.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINKs/LINKmsg.h"
#include "F3Dstrings.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "DIAlogs/DIAselection_dlg.h"
#include "DIAlogs/DIAtoolbox_dlg.h"

#ifdef JADEFUSION
#include "DIAlogs/DIAlightrej_dlg.h"
#endif

#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLconst.h"
extern void EDI_Tooltip_DisplayMessage(char *, ULONG ulSpeed = 200);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
#ifdef JADEFUSION
extern ULONG	EDI_OUT_gl_ForceSetMode;
extern ULONG	GRI_gf_Z;
#else
extern "C"	ULONG	EDI_OUT_gl_ForceSetMode;
extern "C"	ULONG	GRI_gf_Z;
#endif
BOOL				F3D_gb_PickMustRefresh = TRUE;
extern char			*F3D_STR_Csz_Zone_AI[16];

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DisplayObjectName(POINT point, BOOL _b_Status)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Link					*pst_Link;
	WAY_tdst_LinkList				*pst_LinkList;
	OBJ_tdst_GameObject				*pst_GO;
	char							asz_Path[BIG_C_MaxLenPath];
	long							t, e;
	BIG_INDEX						ul_Index;
	int								i;
	char							asz_Bin[33];
	SOFT_tdst_PickingBuffer_Pixel	*pst_Pixel;
	MATH_tdst_Vector				*v;
	GDI_tdst_Device					*pst_Dev;
	MATH_tdst_Vector				vpos;
	CAM_tdst_Camera					*pst_Cam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Shift must be pressed */
	if(GetAsyncKeyState(VK_SHIFT) >= 0) return;

	/* Control must NOT, be pressed */
	//if(GetAsyncKeyState(VK_CONTROL) < 0) return;

	if(!Pick_l_UnderPoint(&point, SOFT_Cuc_PBQF_All, 0))
	{
		if(!_b_Status) EDI_Tooltip_DisplayMessage("Nothing", 10);
		return;
	}

	pst_Pixel = Pick_pst_GetFirst(SOFT_Cuc_PBQF_All, -1);
	if(pst_Pixel == NULL) return;

	*asz_Path = 0;

	switch(pst_Pixel->ul_ValueExt & SOFT_Cul_PBQF_TypeMask)
	{
	case SOFT_Cuc_PBQF_Link:
		pst_GO = ((WAY_tdst_GraphicLink *) pst_Pixel->ul_Value)->pst_Origin;
		pst_Link = ((WAY_tdst_GraphicLink *) pst_Pixel->ul_Value)->pst_Link;
		pst_LinkList = WAY_pst_SearchOneLinkList(pst_GO, pst_Link);
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_LinkList->pst_Network);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			*asz_Bin = 0;
			for(i = 31; i >= 0; i--) L_strcat(asz_Bin, pst_Link->uw_Capacities & (1 << i) ? "1" : "0");
			sprintf
			(
				asz_Path,
				"%s\n%s => %s\nCapa : %s",
				BIG_NameFile(ul_Index),
				pst_GO->sz_Name,
				pst_Link->pst_Next->sz_Name,
				asz_Bin
			);
		}
		break;

	case SOFT_Cuc_PBQF_Zone:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			UCHAR					uc_Index;
			UCHAR					uc_Type;
			GEO_tdst_GraphicZone	*pst_GZ;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_GZ = mst_WinHandles.pst_DisplayData->st_DisplayedZones.dpst_GraphicZone + (ULONG) pst_Pixel->ul_Value;
			pst_GO = pst_GZ->pst_GO;
			uc_Type = pst_GZ->uc_Type;
			switch(uc_Type)
			{
			case GEO_Cul_GraphicZDx:
				uc_Index = ((COL_tdst_ZDx *) (pst_GZ->pv_Data))->uc_AI_Index;
				sprintf
				(
					asz_Path,
					"\"%s\" ",
					((COL_tdst_ZDx *) (pst_GZ->pv_Data))->sz_Name
				);
				if(uc_Index < COL_Cul_MaxNbOfZDx)
					sprintf(asz_Path + strlen(asz_Path), "[%s] ", F3D_STR_Csz_Zone_AI[uc_Index]);
				else
					sprintf(asz_Path + strlen(asz_Path), "[Error: %i] ", uc_Index);
				sprintf(asz_Path + strlen(asz_Path), "from object %s", pst_GO->sz_Name);

				break;

			case GEO_Cul_GraphicCob:
				{
					/*~~~~~~~~~~~~~~~~~~~~~*/
					COL_tdst_Cob	*pst_Cob;
					/*~~~~~~~~~~~~~~~~~~~~~*/

					pst_Cob = (COL_tdst_Cob *) pst_GZ->pv_Data;

					if(pst_Cob->uc_Type == COL_C_Zone_Triangles)
					{
						/*~~~~~~~~~~~~~~~~~~~~~~~~*/
						GEO_tdst_Object *pst_GeoCob;
						ULONG			ul_GMatID;
						/*~~~~~~~~~~~~~~~~~~~~~~~~*/

						e = (pst_Pixel->ul_ValueExt & SOFT_Cul_PBQF_ElementMask) >> SOFT_Cul_PBQF_ElementShift;
						t = (pst_Pixel->ul_ValueExt & SOFT_Cul_PBQF_TriangleMask) >> SOFT_Cul_PBQF_TriangleShift;

						if(!pst_Cob) return;
						pst_GeoCob = (GEO_tdst_Object *) pst_Cob->p_GeoCob;
						if(!pst_GeoCob) return;
						if(pst_Cob->pst_GMatList)
						{
							ul_GMatID = pst_Cob->pst_TriangleCob->dst_Element[e].l_MaterialId;
							sprintf
							(
								asz_Path,
								"ColMap of %s, Element:%u, Triangle: %u, GMat ID: %u",
								pst_GO->sz_Name,
								e,
								t,
								ul_GMatID
							);
						}
						else
							sprintf(asz_Path, "ColMap of %s, Element:%u, Triangle: %u", pst_GO->sz_Name, e, t);
					}
				}
				break;
			}
		}
		break;

	case SOFT_Cuc_PBQF_GameObject:
		{
			/*~~~~~~~~~~~~~~~~~~~~~*/
			GRO_tdst_Struct *pst_Gro;
			/*~~~~~~~~~~~~~~~~~~~~~*/

			pst_GO = (OBJ_tdst_GameObject *) pst_Pixel->ul_Value;
			e = (pst_Pixel->ul_ValueExt & SOFT_Cul_PBQF_ElementMask) >> SOFT_Cul_PBQF_ElementShift;
			t = (pst_Pixel->ul_ValueExt & SOFT_Cul_PBQF_TriangleMask) >> SOFT_Cul_PBQF_TriangleShift;
			v = OBJ_pst_GetAbsolutePosition(pst_GO);

			/* Pos 3D */
			pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
			vpos.x = (float) point.x / (float) pst_Dev->l_Width;
			vpos.y = 1.0f - ((float) point.y / (float) pst_Dev->l_Height);
			b_PickDepth(&vpos, &vpos.z);
			pst_Cam = &mst_WinHandles.pst_DisplayData->st_Camera;
			vpos.x = (float) point.x;
			vpos.y = (float) point.y;
			CAM_2Dto3D(pst_Cam, &vpos, &vpos);
			Pick_l_UnderPoint(&point, SOFT_Cuc_PBQF_All, 0);

			pst_Gro = NULL;
			if((ULONG) pst_GO != -1)
			{
				if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
					pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Object;
				if(pst_Gro)
					if(pst_Gro->i->ul_Type != GRO_Geometric) pst_Gro = NULL;
			}

			if(pst_Gro)
			{
				sprintf(asz_Path, "%s\n", pst_GO->sz_Name);
				sprintf(asz_Path + strlen(asz_Path), "Element: %d, Triangle: %d\n", e, t);

				if
				(
					(GEO_MRM_ul_IsMrmObject((GEO_tdst_Object *) pst_Gro))
				||	(GEO_SKN_IsSkinned((GEO_tdst_Object *) pst_Gro))
				)
				{
					sprintf(asz_Path + strlen(asz_Path), "Special features:");
					if(GEO_SKN_IsSkinned((GEO_tdst_Object *) pst_Gro))
						sprintf(asz_Path + strlen(asz_Path), "Skinned,");
					if(GEO_MRM_ul_IsMrmObject((GEO_tdst_Object *) pst_Gro))
						sprintf(asz_Path + strlen(asz_Path), "MRM,");
					sprintf(asz_Path + strlen(asz_Path), "\n");
				}
			}
			else
			{
				sprintf(asz_Path, "%s\n", pst_GO->sz_Name);
			}

			sprintf(asz_Path + strlen(asz_Path), "Object Pos : %.2f, %.2f, %.2f\nPicking Pos : %.2f, %.2f, %.2f\n", v->x, v->y, v->z, vpos.x, vpos.y, vpos.z);
		}
		break;
	}

	if(*asz_Path)
	{
		LINK_gb_UseSecond = TRUE;
		LINK_gb_CanLog = FALSE;
		if(_b_Status)
			LINK_PrintStatusMsg(asz_Path);
		else
			EDI_Tooltip_DisplayMessage(asz_Path, 1);
		LINK_gb_CanLog = TRUE;
		LINK_gb_UseSecond = FALSE;
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG F3D_cl_View::ul_PickSelBox(POINT point)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	CPoint	pt;
	ULONG	ul_Val;
	/*~~~~~~~~~~~*/

	if(mb_SelectOn && mpo_SelectionDialog)
	{
		mpo_SelectionDialog->GetWindowRect(&o_Rect);
		pt = point;
		ClientToScreen(&pt);
		if(o_Rect.PtInRect(pt))
		{
			ul_Val = mpo_SelectionDialog->ul_GAOFromPt(pt);
			if(!ul_Val) return 1;
			return ul_Val;
		}
	}

	if(mb_ToolBoxOn && mpo_ToolBoxDialog)
	{
		mpo_ToolBoxDialog->GetWindowRect(&o_Rect);
		pt = point;
		ClientToScreen(&pt);
		if(o_Rect.PtInRect(pt)) return 1;
	}

#ifdef JADEFUSION
	// NB: Added Light Rejection dialog - from POP5 Jade Editor	
	if(mb_LightRejectOn && mpo_LightRejectDialog)
	{
		mpo_LightRejectDialog->GetWindowRect(&o_Rect);
		pt = point;
		ClientToScreen(&pt);
		if(o_Rect.PtInRect(pt)) return 1;
	}
#endif
	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *F3D_cl_View::pst_Pick(POINT point)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	ULONG							ul_Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mb_LockPickObj) return mpst_LockPickObj;

	ul_Val = ul_PickSelBox(point);
	if(ul_Val && ul_Val != 1) return (OBJ_tdst_GameObject *) ul_Val;
	if(ul_Val) return NULL;

	pst_Query = &mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query;
	pst_Query->st_Point1.x = (float) point.x / mst_WinHandles.pst_DisplayData->st_Device.l_Width;
	pst_Query->st_Point1.y = 1.0f - ((float) point.y / mst_WinHandles.pst_DisplayData->st_Device.l_Height);
	pst_Query->l_Filter = SOFT_Cuc_PBQF_GameObject;
	pst_Query->l_Tolerance = 0;
	mst_WinHandles.pst_DisplayData->pst_PickingBuffer->ul_Flags = SOFT_Cul_PBF_PickWhileRender;
	return (OBJ_tdst_GameObject *) ul_PickExt();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
WAY_tdst_Link *F3D_cl_View::pst_PickLink(POINT point, OBJ_tdst_GameObject **_ppst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_GraphicLink			*pst_GL;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Query = &mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query;
	pst_Query->st_Point1.x = (float) point.x / mst_WinHandles.pst_DisplayData->st_Device.l_Width;
	pst_Query->st_Point1.y = 1.0f - ((float) point.y / mst_WinHandles.pst_DisplayData->st_Device.l_Height);
	pst_Query->l_Filter = SOFT_Cuc_PBQF_Link;
	pst_Query->l_Tolerance = 0;
	mst_WinHandles.pst_DisplayData->pst_PickingBuffer->ul_Flags = 0;
	pst_GL = (WAY_tdst_GraphicLink *) ul_PickExt();
	if(!pst_GL) return NULL;
	*_ppst_GO = pst_GL->pst_Origin;
	return pst_GL->pst_Link;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG F3D_cl_View::ul_PickExt(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_PickingBuffer			*pst_PB;
	ULONG							ul_Res;
	int								i_X, i_Y, i_X2, i_Y2;
	BOOL							b_Res;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	ULONG							ul_Val;
	POINT							pt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mb_LockPickObj) return (ULONG) mpst_LockPickObj;

	mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_UsePickingBuffer;
    mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DoNotRender;
	pst_PB = mst_WinHandles.pst_DisplayData->pst_PickingBuffer;
	pst_Query = &pst_PB->st_Query;

	i_X = (int) (pst_Query->st_Point1.x * pst_PB->l_Width);
	i_Y = (int) (pst_Query->st_Point1.y * pst_PB->l_Height);

	pt.x = i_X;
	pt.y = i_Y;
	ul_Val = ul_PickSelBox(pt);
	if(ul_Val && ul_Val != 1) return ul_Val;
	if(ul_Val) return 0;

	if(pst_PB->ul_Flags & SOFT_Cul_PBF_BoxSelection)
	{
		i_X2 = (int) (pst_Query->st_Point2.x * pst_PB->l_Width);
		i_Y2 = (int) (pst_Query->st_Point2.y * pst_PB->l_Height);
		SOFT_SetClipWindow(pst_PB, i_X - 10, i_Y - 10, i_X2 + 10, i_Y2 + 10);
	}
	else
	{
		SOFT_SetClipWindow(pst_PB, i_X - 10, i_Y - 10, i_X + 10, i_Y + 10);
	}

	if(F3D_gb_PickMustRefresh) Refresh();

	if(pst_PB->ul_Flags & SOFT_Cul_PBF_BoxSelection)
	{
		b_Res = pst_Query->l_Number != 0;
		ul_Res = 1;
	}
	else
	{
		b_Res = SOFT_b_PickingBuffer_PickExt(pst_PB, pst_Query);
		ul_Res = pst_Query->st_First.ul_Value;
	}

	mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_UsePickingBuffer;
    mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_DoNotRender;
	SOFT_ResetClipWindow(pst_PB);

	if(!b_Res) return 0;
	return ul_Res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::b_PickDepth(MATH_tdst_Vector *_pst_Pos, float *_pf_Z)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_PickingBuffer *pst_PB;
	BOOL					b_Res;
	int						i_X, i_Y;
	static float			last_Z = 5.0f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_UsePickingBuffer;
    mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DoNotRender;

	pst_PB = mst_WinHandles.pst_DisplayData->pst_PickingBuffer;
	pst_PB->ul_Flags = 0;
	i_X = (int) (_pst_Pos->x * pst_PB->l_Width);
	i_Y = (int) (_pst_Pos->y * pst_PB->l_Height);
	SOFT_SetClipWindow(pst_PB, i_X - 40, i_Y - 40, i_X + 40, i_Y + 40);
	if(F3D_gb_PickMustRefresh) Refresh();

	b_Res = SOFT_b_PickingBuffer_GetOoZ(pst_PB, _pst_Pos, _pf_Z);

	/* If nothing found under mouse set depth to a arbitary distance */
	if(!b_Res || (*_pf_Z == 0))
	{
		b_Res = 0;	/* Force bad result on null value */
		*_pf_Z = last_Z;
	}
	else
	{
		*_pf_Z = fOptInv(*_pf_Z);
		last_Z = *_pf_Z;
	}

	mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_UsePickingBuffer;
    mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_DoNotRender;
	SOFT_ResetClipWindow(pst_PB);
	return b_Res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::b_PickOozRect(MATH_tdst_Vector *_pst_Pos1, MATH_tdst_Vector *_pst_Pos2, float *_pf_Ooz)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_PickingBuffer *pst_PB;
	BOOL					b_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_UsePickingBuffer;
    mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DoNotRender;
	pst_PB = mst_WinHandles.pst_DisplayData->pst_PickingBuffer;
	SOFT_ResetClipWindow(pst_PB);
	if(F3D_gb_PickMustRefresh) Refresh();

	b_Res = SOFT_b_PickingBuffer_GetMinOoZRect(pst_PB, _pst_Pos1, _pst_Pos2, _pf_Ooz);

	mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_UsePickingBuffer;
    mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_DoNotRender;
	SOFT_ResetClipWindow(pst_PB);
	return b_Res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::b_PickOozRectPond(MATH_tdst_Vector *_pst_Pos1, MATH_tdst_Vector *_pst_Pos2, float *_pf_Ooz)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_PickingBuffer *pst_PB;
	BOOL					b_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_UsePickingBuffer;
    mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DoNotRender;
	pst_PB = mst_WinHandles.pst_DisplayData->pst_PickingBuffer;
	SOFT_ResetClipWindow(pst_PB);
	if(F3D_gb_PickMustRefresh) Refresh();

	b_Res = SOFT_b_PickingBuffer_GetMinOoZRectPond(pst_PB, _pst_Pos1, _pst_Pos2, _pf_Ooz);

	mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_UsePickingBuffer;
    mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_DoNotRender;
	SOFT_ResetClipWindow(pst_PB);
	return b_Res;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropMaterial(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject				*pst_GO;
	WOR_tdst_World					*pst_World;
	GRO_tdst_Struct					*pst_GRO, *pst_Mat;
	unsigned long					i, ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Find object where to drop material */
	ScreenToClient(&_pst_DragDrop->o_Pt);
	pst_GO = (OBJ_tdst_GameObject *) pst_Pick((POINT) _pst_DragDrop->o_Pt);
	if(!pst_GO) return;

	/* can not d&d if animation struct is allocated */
	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims)) 
	{
		ERR_X_Error(0, "Can't associate material cause object has an animation structure", NULL);
		return;
	}

    /* allocate visu structure if not allocated */
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) 
        OBJ_ChangeIdentityFlags( pst_GO, pst_GO->ul_IdentityFlags | OBJ_C_IdentityFlag_Visu, pst_GO->ul_IdentityFlags );

	/* Get dropped material */
	pst_World = mst_WinHandles.pst_World;
	pst_GRO = GEO_pst_Object_Load(_pst_DragDrop->ul_FatFile, pst_World);
	if(TAB_ul_Ptable_GetElemIndexWithPointer(&pst_World->st_GraphicMaterialsTable, pst_GRO) == TAB_Cul_BadIndex)
	{
		TAB_Ptable_AddElemAndResize(&pst_World->st_GraphicMaterialsTable, pst_GRO);
		pst_GRO->i->pfn_AddRef(pst_GRO, 1);
		EDI_OUT_gl_ForceSetMode = 1;
	}

	if(pst_GO->pst_Base->pst_Visu->pst_Material)
		pst_GO->pst_Base->pst_Visu->pst_Material->i->pfn_AddRef(pst_GO->pst_Base->pst_Visu->pst_Material, -1);

	pst_GO->pst_Base->pst_Visu->pst_Material = pst_GRO;
	pst_GRO->i->pfn_AddRef(pst_GRO, 1);

	if(pst_GRO->i->ul_Type == GRO_MaterialMulti)
	{
		for(i = 0; (long) i < ((MAT_tdst_Multi *) pst_GRO)->l_NumberOfSubMaterials; i++)
		{
			pst_Mat = (GRO_tdst_Struct *) ((MAT_tdst_Multi *) pst_GRO)->dpst_SubMaterial[i];
			if(!pst_Mat) continue;
			pst_Mat->i->pfn_AddRef(pst_Mat, 1);
			ul_Index = TAB_ul_Ptable_GetElemIndexWithPointer(&pst_World->st_GraphicMaterialsTable, pst_Mat);
			if(ul_Index == TAB_Cul_BadIndex)
			{
				TAB_Ptable_AddElemAndResize(&pst_World->st_GraphicMaterialsTable, pst_Mat);
				pst_Mat->i->pfn_AddRef(pst_Mat, 1);
				EDI_OUT_gl_ForceSetMode = 1;
			}
		}
	}

	LINK_Refresh();
	EDI_OUT_gl_ForceSetMode = 0;
}

#endif
