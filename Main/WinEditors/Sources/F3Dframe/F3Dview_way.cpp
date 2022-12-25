/*$T F3Dview_way.cpp GC! 1.100 08/31/01 12:54:55 */


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
#include "LINKs/LINKmsg.h"
#include "TABles/TABles.h"

#include "CAMera/CAMera.h"

#include "EDIapp.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "LINKs/LINKtoed.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJsave.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORexporttomad.h"
#include "ENGine/Sources/WAYs/WAYsave.h"
#include "ENGine/Sources/WAYs/WAYinit.h"
#include "ENGine/Sources/WAYs/WAY.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "GEOmetric/GEOload.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dview_undo.h"
#include "F3Dframe/F3Dstrings.h"
#include "GEOmetric/GEOdebugobject.h"
#include "SOFT/SOFThelper.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAchecklist_dlg.h"
#include "VAVview/VAVview.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "ENGine/Sources/INTersection/INTSnp.h"

#include "EDIstrings.h"
#include "EDIpaths.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define NM()	ERR_X_Warning(0, "Action can't be done cause network has been merged", NULL)

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *F3D_cl_View::CreateWaypoint(CPoint point, char *_sz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Link		*pst_Link, *pst_Link1;
	WAY_tdst_LinkList	*pst_LinkList, *pst_LinkList1;
	OBJ_tdst_GameObject *pst_GO, *pst_NewGO;
	ULONG				ul_Identity;
	WOR_tdst_World		*pst_World;
	GDI_tdst_Device		*pst_Dev;
	MATH_tdst_Vector	v;
	CAM_tdst_Camera		*pst_Cam;
	char				asz_Name[BIG_C_MaxLenName];
	char				asz_Path[BIG_C_MaxLenPath];
	BIG_INDEX			ul_Key, ul_Index;
	static int			i_NameNumber;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = mst_WinHandles.pst_World;
	if(!pst_World) return NULL;

	/* Register and set a default name */
#ifdef JADEFUSION
	if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_Create_WP))
#endif
	{
	WOR_GetGaoPath(pst_World, asz_Path);
	}
	/* Pick a link ? (for later) */
	pst_Link = pst_PickLink(point, &pst_NewGO);

#ifdef JADEFUSION
	if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_Create_WP))
#else
	if(0)
#endif
	{
    if (!_sz_Name)
    {
        EDIA_cl_NameDialogCompletion  o_Dialog(pst_World, DLG_WAYPOINT);

        if(o_Dialog.DoModal() == IDOK)
        {
            if(L_strlen((LPSTR)(LPCSTR)o_Dialog.m_strName) > 0)
            {
                ERR_X_Error(L_strlen((LPSTR)(LPCSTR)o_Dialog.m_strName) <= BIG_C_MaxRealLenName, "Invalid Name. Too Long. (Restricted to 60 character max)", NULL);

                if(BIG_b_CheckName((LPSTR)(LPCSTR) o_Dialog.m_strName) == FALSE) 
                {
                    return NULL;
                }

                L_strcpy(asz_Name, (LPSTR)(LPCSTR) o_Dialog.m_strName);
                L_strcat(asz_Name, EDI_Csz_ExtGameObject);
                
                ul_Index = BIG_ul_SearchDir(asz_Path);
                if (ul_Index == BIG_C_InvalidIndex)
                {
                    BIG_ul_CreateDir(asz_Path);
                    ul_Index = BIG_ul_SearchDir(asz_Path);
                }

                ul_Index = BIG_ul_SearchFileInDirRec(ul_Index, asz_Name);
                if (ul_Index != BIG_C_InvalidIndex)
                {
                    BIG_BuildNewName(asz_Path, asz_Name, EDI_Csz_ExtGameObject);
                }
            }
            else 
            {
                L_strcpy(asz_Name, "Unnamed");
                BIG_BuildNewName(asz_Path, asz_Name, EDI_Csz_ExtGameObject);
            }
        }
        else
        {
            return NULL;
        }

    }
    else
    {
        strcpy(asz_Name, _sz_Name);
        BIG_BuildNewName(asz_Path, asz_Name, EDI_Csz_ExtGameObject);
    }
	}

	ul_Identity = OBJ_C_IdentityFlag_Waypoints;
	mpst_LockPickObj = pst_GO = OBJ_GameObject_Create(ul_Identity);

	OBJ_ComputeBV(pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_AABBox);

	/* Add object at the drag point */
	pst_GO->pst_World = pst_World;
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	v.x = (float) point.x / (float) pst_Dev->l_Width;
	v.y = 1.0f - ((float) point.y / (float) pst_Dev->l_Height);
	b_PickDepth(&v, &v.z);

	pst_Cam = &mst_WinHandles.pst_DisplayData->st_Camera;
	v.x = (float) point.x;
	v.y = (float) point.y;
	CAM_2Dto3D(pst_Cam, &v, &v);
	OBJ_SetInitialAbsolutePosition(pst_GO, &v);
	OBJ_RestoreInitialPos(pst_GO);

#ifdef JADEFUSION
	if(!EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_Create_WP))
#else	
	if(1)
#endif
	{
		/* Register and set a default name */
		WOR_GetGaoPath(pst_World, asz_Path);
		strcpy(asz_Name, _sz_Name ? _sz_Name : "Unnamed");
		BIG_BuildNewName(asz_Path, asz_Name, EDI_Csz_ExtGameObject);
	}

	WOR_gpst_WorldToLoadIn = pst_World;
	OBJ_GameObject_RegisterWithName(pst_GO, BIG_C_InvalidIndex, asz_Name, OBJ_GameObject_RegSetName);

	ul_Key = OBJ_ul_GameObject_Save(pst_World, pst_GO, asz_Path);
	LOA_AddAddress(ul_Key, pst_GO);

	WOR_World_JustAfterLoadObject(pst_World, pst_GO, TRUE, TRUE);

	/* Insert object on a link ? */
	if(pst_Link)
	{
		/* Search current link list */
		pst_LinkList = WAY_pst_SearchOneLinkList(pst_NewGO, pst_Link);
		if(pst_LinkList->pst_Network && pst_LinkList->pst_Network->ul_Flags & WAY_C_HasBeenMerge)
		{
			NM();
			return pst_GO;
		}

		WAY_pst_CreateStruct(mst_WinHandles.pst_World, pst_GO);

		pst_LinkList1 = WAY_pst_AddOneLinkList(pst_GO, pst_LinkList->pst_Network);
		pst_Link1 = WAY_pst_AddOneLink(pst_GO, pst_LinkList1, pst_Link->pst_Next);
		L_memcpy(pst_Link1, pst_Link, sizeof(WAY_tdst_Link));

		/* Net of current link is current object */
		pst_LinkList = WAY_pst_SearchLinkList(pst_Link->pst_Next, pst_LinkList->pst_Network);
		pst_Link->pst_Next = pst_GO;

		/* Reverse link ? */
		if(pst_LinkList)
		{
			pst_Link = WAY_pst_SearchOneLinkInList(pst_LinkList, pst_NewGO);
			if(pst_Link)
			{
				pst_Link1 = WAY_pst_AddOneLink(pst_GO, pst_LinkList1, pst_Link->pst_Next);
				L_memcpy(pst_Link1, pst_Link, sizeof(WAY_tdst_Link));
				pst_Link->pst_Next = pst_GO;
			}
		}
	}

	return pst_GO;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::LinkObjects(UINT nFlags, CPoint point, BOOL _b_First)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject				*p_Sel;
	int								i;
	CDC								*pDC;
	CPen							o_Pen, *poldpen;
	SOFT_tdst_PickingBuffer_Pixel	*pst_Pixel;
	ULONG							ul_IF;
	MATH_tdst_Matrix				M, M1, *pst_Local;
	char							az[512];
	MATH_tdst_Vector				v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Do no pick because DisplayObjectName as already done one */
	pst_Pixel = Pick_pst_GetFirst(SOFT_Cuc_PBQF_GameObject, -1);
	if(pst_Pixel)
		p_Sel = (OBJ_tdst_GameObject *) pst_Pixel->ul_Value;
	else
		p_Sel = NULL;

	if(!p_Sel && _b_First) return;
	if(!mb_LinkOn) LINK_Refresh();
	mb_LinkOn = TRUE;

	pDC = GetDC();
#ifdef JADEFUSION
	o_Pen.CreatePen(PS_SOLID, 1, GetSysColor(0x00ffffff));
#else
	o_Pen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_ACTIVECAPTION));
#endif
	poldpen = pDC->SelectObject(&o_Pen);
	pDC->SetROP2(R2_XORPEN);

	if(_b_First)
	{
		mi_NumLinks = 1;
		mast_LinkNodes[0].st_Pos = point;
		mast_LinkNodes[0].pst_GO = p_Sel;
		mast_LinkNodes[1].st_Pos.x = point.x + 1;
		mast_LinkNodes[1].st_Pos.y = point.y + 1;
		pDC->MoveTo(mast_LinkNodes[0].st_Pos);
		pDC->LineTo(mast_LinkNodes[1].st_Pos);
	}

	if(p_Sel && (mi_NumLinks == 1))
	{
		MATH_SubVector(&v, &p_Sel->pst_GlobalMatrix->T, &mast_LinkNodes[0].pst_GO->pst_GlobalMatrix->T);
		sprintf(az, "Distance between objects = %f", MATH_f_NormVector(&v));
		M_MF()->mo_Status.SetPaneText(0, az);
	}

	/* Erase end link, and redisplay it */
	pDC->MoveTo(mast_LinkNodes[mi_NumLinks - 1].st_Pos);
	pDC->LineTo(mast_LinkNodes[mi_NumLinks].st_Pos);

	mast_LinkNodes[mi_NumLinks].st_Pos.x = point.x + 1;
	mast_LinkNodes[mi_NumLinks].st_Pos.y = point.y + 1;
	pDC->MoveTo(mast_LinkNodes[mi_NumLinks - 1].st_Pos);
	pDC->LineTo(mast_LinkNodes[mi_NumLinks].st_Pos);

	/* Have we reach a new object ? */
	if(!(nFlags & MK_RBUTTON) && !(nFlags & MK_MBUTTON)) mb_WaitForRButton = FALSE;

	if(p_Sel)
	{
		if
		(
			((nFlags & MK_RBUTTON) && !mb_WaitForRButton)
		||	((nFlags & MK_MBUTTON) && !mb_WaitForRButton)
		)
		{
			mb_WaitForRButton = TRUE;
			if(p_Sel != mast_LinkNodes[mi_NumLinks - 1].pst_GO)
			{
				mast_LinkNodes[mi_NumLinks].pst_GO = p_Sel;
				mi_NumLinks++;
				mast_LinkNodes[mi_NumLinks].st_Pos.x = point.x + 1;
				mast_LinkNodes[mi_NumLinks].st_Pos.y = point.y + 1;
				pDC->MoveTo(mast_LinkNodes[mi_NumLinks - 1].st_Pos);
				pDC->LineTo(mast_LinkNodes[mi_NumLinks].st_Pos);

				if(nFlags & MK_MBUTTON)
				{
					mast_LinkNodes[mi_NumLinks].pst_GO = mast_LinkNodes[mi_NumLinks - 2].pst_GO;
					mast_LinkNodes[mi_NumLinks].st_Pos.x = mast_LinkNodes[mi_NumLinks - 2].st_Pos.x;
					mast_LinkNodes[mi_NumLinks].st_Pos.y = mast_LinkNodes[mi_NumLinks - 2].st_Pos.y;
					mi_NumLinks++;
					mast_LinkNodes[mi_NumLinks].pst_GO = p_Sel;
					mast_LinkNodes[mi_NumLinks].st_Pos.x = point.x + 1;
					mast_LinkNodes[mi_NumLinks].st_Pos.y = point.y + 1;
					mi_NumLinks++;
				}
			}
			else if(mi_NumLinks > 1)
			{
				pDC->MoveTo(mast_LinkNodes[mi_NumLinks - 1].st_Pos);
				pDC->LineTo(mast_LinkNodes[mi_NumLinks].st_Pos);
				mi_NumLinks--;
				pDC->MoveTo(mast_LinkNodes[mi_NumLinks - 1].st_Pos);
				pDC->LineTo(mast_LinkNodes[mi_NumLinks].st_Pos);
				mast_LinkNodes[mi_NumLinks].st_Pos.x = point.x + 1;
				mast_LinkNodes[mi_NumLinks].st_Pos.y = point.y + 1;
				pDC->MoveTo(mast_LinkNodes[mi_NumLinks - 1].st_Pos);
				pDC->LineTo(mast_LinkNodes[mi_NumLinks].st_Pos);
			}
		}
	}

	if(mb_HierarchicalLink)
	{
		if((mi_NumLinks == 2) || (!(nFlags & MK_LBUTTON)) || (!IsActionValidate(F3D_Action_HierarchicalLink)))
		{
			if(mi_NumLinks == 2)
			{
				/* link only if that don't create a cycle */
				p_Sel = mast_LinkNodes[0].pst_GO;
				while(p_Sel->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
				{
					if(p_Sel->pst_Base->pst_Hierarchy->pst_FatherInit == mast_LinkNodes[1].pst_GO)
					{
						mi_NumLinks = 0;
						break;
					}

					p_Sel = p_Sel->pst_Base->pst_Hierarchy->pst_FatherInit;
					if(p_Sel == NULL) break;
				}

				/* link */
				if(mi_NumLinks)
				{
					p_Sel = mast_LinkNodes[1].pst_GO;
					if(p_Sel->c_FixFlags & OBJ_C_HasBeenMerge)
					{
						sprintf(az, "Link %s on a merged object", p_Sel->sz_Name);
						ERR_X_Warning(0, az, NULL);
					}
					else if(mast_LinkNodes[0].pst_GO->c_FixFlags & OBJ_C_HasBeenMerge)
					{
						sprintf(az, "Link %s on a merged object", mast_LinkNodes[0].pst_GO->sz_Name);
						ERR_X_Warning(0, az, NULL);
					}
//					else
					{
						ul_IF = p_Sel->ul_IdentityFlags;
						OBJ_ChangeIdentityFlags(p_Sel, ul_IF | OBJ_C_IdentityFlag_Hierarchy, ul_IF);
						mast_LinkNodes[0].pst_GO->ul_StatusAndControlFlags |= OBJ_C_StatusFlag_HasChild;
						p_Sel->pst_Base->pst_Hierarchy->pst_FatherInit = mast_LinkNodes[0].pst_GO;
						p_Sel->pst_Base->pst_Hierarchy->pst_Father = mast_LinkNodes[0].pst_GO;
						MATH_SetIdentityMatrix(&M);
						MATH_InvertMatrix(&M, OBJ_pst_GetAbsoluteMatrix(mast_LinkNodes[0].pst_GO));
						pst_Local = &p_Sel->pst_Base->pst_Hierarchy->st_LocalMatrix;
						MATH_SetIdentityMatrix(pst_Local);
						MATH_MulMatrixMatrix(pst_Local, OBJ_pst_GetAbsoluteMatrix(p_Sel), &M);

						/* Initial pos must now be relative to father */
						if(p_Sel->ul_IdentityFlags & OBJ_C_IdentityFlag_HasInitialPos)
						{
							MATH_InvertMatrix(&M, OBJ_pst_GetAbsoluteMatrix(mast_LinkNodes[0].pst_GO));
							MATH_MulMatrixMatrix(&M1, OBJ_pst_GetInitialAbsoluteMatrix(p_Sel), &M);
							MATH_CopyMatrix(OBJ_pst_GetInitialAbsoluteMatrix(p_Sel), &M1);
						}

						/* Flash pos must now be relative to father */
						if(p_Sel->ul_IdentityFlags & OBJ_C_IdentityFlag_FlashMatrix) ANI_ApplyHierarchyOnFlash(p_Sel);

						mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DisplayHierarchy;
					}
				}
			}

			mb_LinkOn = FALSE;
			mi_NumLinks = 0;
			LINK_Refresh();
		}
	}
	else
	{
		if(!(nFlags & MK_LBUTTON) || (!IsActionValidate(F3D_Action_Link)))
		{
			/* Erase all links */
			for(i = 0; i < mi_NumLinks; i++)
			{
				pDC->MoveTo(mast_LinkNodes[i].st_Pos);
				pDC->LineTo(mast_LinkNodes[i + 1].st_Pos);
			}

			if(mi_NumLinks > 1) CreateNetwork(NULL);
			mb_LinkOn = FALSE;
			LINK_Refresh();
			mi_NumLinks = 0;
		}
	}

	pDC->SelectObject(poldpen);
	DeleteObject(&o_Pen);
	ReleaseDC(pDC);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::CreateNetwork(OBJ_tdst_GameObject **pp_SrcNodes)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i, k;
	OBJ_tdst_GameObject		*pst_GO;
	WAY_tdst_Struct			*pst_Links;
	WAY_tdst_LinkList		*pst_List;
	WAY_tdst_LinkList		*pst_List1;
	int						i_NumNet;
	WAY_tdst_Network		*pst_Net;
	EDIA_cl_NameDialogCombo o_Dialog(F3D_STR_Csz_NetName);
	WAY_tdst_Network		*apst_Present[100];
	ULONG					j;
	BIG_INDEX				ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mi_NumLinks) return;

	/* Search if one object is already in a network */
	i_NumNet = 0;
	for(i = 0; i < mi_NumLinks; i++)
	{
		pst_GO = mast_LinkNodes[i].pst_GO;
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Links))
		{
			pst_Links = (WAY_tdst_Struct *) pst_GO->pst_Extended->pst_Links;
			ERR_X_Assert(pst_Links);
			if(pst_Links->ul_Num)
			{
				for(j = 0; j < pst_Links->ul_Num; j++)
				{
					pst_List = &pst_Links->pst_AllLinks[j];

					/* Search it network has already been encountered */
					for(k = 0; k < i_NumNet; k++)
					{
						if(apst_Present[k] == pst_List->pst_Network) break;
					}

					/* No, add it (if it is currently visible) */
					if(k == i_NumNet)
					{
						if((pst_List->pst_Network->ul_Flags & WAY_C_DisplayNet) && (pst_List->pst_Network->pst_Root))
						{
							apst_Present[i_NumNet] = pst_List->pst_Network;
							i_NumNet++;
						}
					}
				}
			}
		}
	}

	if(i_NumNet == 1 && apst_Present[0] && apst_Present[0]->ul_Flags & WAY_C_HasBeenMerge)
	{
		NM();
		return;
	}

	/* No network yet. Create a new one */
_Try_
	if(!i_NumNet || (i_NumNet > 1))
	{
        // If there are waypoints belonging to 2 networks,
        // ask if we want to merge the networks or create a new network.
        if (i_NumNet == 2)
        {
            char sMsg[256];
            BIG_INDEX ul_Index0 = LOA_ul_SearchIndexWithAddress((ULONG) apst_Present[0]);
            BIG_INDEX ul_Index1 = LOA_ul_SearchIndexWithAddress((ULONG) apst_Present[1]);
			ERR_X_Assert(ul_Index0 != BIG_C_InvalidIndex);
			ERR_X_Assert(ul_Index1 != BIG_C_InvalidIndex);
            sprintf(sMsg,"Do you want to merge %s into %s ?",BIG_NameFile(ul_Index1),BIG_NameFile(ul_Index0));
            if (M_MF()->MessageBox(sMsg, "Merge", MB_YESNO | MB_ICONQUESTION) == IDYES)
            {
                sprintf(sMsg,"Do you want to destroy %s ?",BIG_NameFile(ul_Index1));
                if ((M_MF()->MessageBox(sMsg, "Destroy", MB_YESNO | MB_ICONQUESTION) != IDYES))
                {
                    // TODO : copy network src 
                }

                // If a link belongs to src network, make it belong to dst network.
                WAY_tdst_Network *pst_DstNet = apst_Present[0];
                WAY_tdst_Network *pst_SrcNet = apst_Present[1];

                TAB_tdst_PFelem			*pst_PFElem = TAB_pst_PFtable_GetFirstElem(&mst_WinHandles.pst_World->st_AllWorldObjects);
                TAB_tdst_PFelem			*pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&mst_WinHandles.pst_World->st_AllWorldObjects);

                while(pst_PFElem <= pst_PFLastElem)
                {
                    OBJ_tdst_GameObject		*pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
                    WAY_tdst_Struct *pst_Struct;

                    pst_PFElem++;
                    if(TAB_b_IsAHole(pst_GO)) continue;
                    if(!pst_GO->pst_Extended) continue;
                    if(!pst_GO->pst_Extended->pst_Links) continue;
                    pst_Struct = (WAY_tdst_Struct *) pst_GO->pst_Extended->pst_Links;
                    if(pst_Struct->ul_Num == 0) continue;

                    for(i = 0; i < pst_Struct->ul_Num; i++)
                    {
                        if (pst_Struct->pst_AllLinks[i].pst_Network == pst_SrcNet) 
                            pst_Struct->pst_AllLinks[i].pst_Network = pst_DstNet;
                    }
                }

                // Destroy src network.
                WAY_DelOneNetwork(mst_WinHandles.pst_World, pst_SrcNet,FALSE,TRUE);
                goto checkobjects;
            }
        }

        {
		/* Get network name */
            while(1)
            {
                /* Add all networks */
                if(mst_WinHandles.pst_World->pst_AllNetworks)
                {
                    for(j = 0; j < mst_WinHandles.pst_World->pst_AllNetworks->ul_Num; j++)
                    {
                        ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mst_WinHandles.pst_World->pst_AllNetworks->ppst_AllNetworks[j]);
                        ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
                        o_Dialog.AddItem(BIG_NameFile(ul_Index));
                    }
                }

                if(o_Dialog.DoModal() == IDCANCEL) _Return_(;);
                if(BIG_b_CheckName((char *) (LPCSTR) o_Dialog.mo_Name) == FALSE) _Return_(;);
                pst_Net = WAY_pst_SearchNetwork
                    (
                    mst_WinHandles.pst_World,
                    (char *) (LPCSTR) o_Dialog.mo_Name,
                    mast_LinkNodes[0].pst_GO
                    );
                if(!pst_Net) break;
                if(M_MF()->MessageBox(F3D_STR_Csz_NetAlreadyExists, EDI_STR_Csz_Title, MB_YESNO) == IDYES) goto addtonet;
            }

            pst_Net = WAY_pst_CreateNetwork
                (
                mst_WinHandles.pst_World,
                (char *) (LPCSTR) o_Dialog.mo_Name,
                mast_LinkNodes[0].pst_GO
                );
                
        }
	}

addtonet:
    if (pp_SrcNodes)
    {
        // Links are copied from pp_SrcNodes to dst nodes
        for(i = 0; i < mi_NumLinks; i++)
        {
        	OBJ_tdst_GameObject	*pst_SrcNode = pp_SrcNodes[i];
            OBJ_tdst_GameObject	*pst_DstNode = mast_LinkNodes[i].pst_GO;
            OBJ_tdst_GameObject	*pst_NextSrcNode;
            
            WAY_pst_CreateStruct(mst_WinHandles.pst_World, pst_DstNode);
            pst_List = WAY_pst_AddOneLinkList(pst_DstNode, pst_Net);

            ERR_X_Assert(pst_DstNode && pst_DstNode->pst_Extended && pst_DstNode->pst_Extended->pst_Links);
            WAY_tdst_Struct *pst_DstStruct = (WAY_tdst_Struct *) pst_DstNode->pst_Extended->pst_Links;

            ERR_X_Assert(pst_SrcNode && pst_SrcNode->pst_Extended && pst_SrcNode->pst_Extended->pst_Links);
            WAY_tdst_Struct *pst_SrcStruct = (WAY_tdst_Struct *) pst_SrcNode->pst_Extended->pst_Links;

            // Loop on link lists
            for(j = 0; j < pst_SrcStruct->ul_Num; j++)
            {
                WAY_tdst_LinkList   *pstSrcLinkList = pst_SrcStruct->pst_AllLinks +j;

                // Loop on links
                for(k = 0; k < pstSrcLinkList->ul_Num; k++)
                {
                    WAY_tdst_Link *pstSrcLink = pstSrcLinkList->pst_Links +k;
                    pst_NextSrcNode = pstSrcLink->pst_Next;

                    // Search pst_NextSrcNode in pp_SrcNodes
                    int l;
                    for(l = 0; l < mi_NumLinks; l++)
                    {
                        if (pst_NextSrcNode == pp_SrcNodes[l])
                            break;
                    }
                    
                    if (l < mi_NumLinks)
                    {
                        // Found next node, create link with copied nodes (mast_LinkNodes)
                        WAY_tdst_Link *pstDstLink = WAY_pst_AddOneLink(pst_DstNode, pst_List, mast_LinkNodes[l].pst_GO);

                        // Copy link data.
                        pstDstLink->c_Design = pstSrcLink->c_Design;
                        pstDstLink->c_DesignInit = pstSrcLink->c_DesignInit;
                        pstDstLink->f_Pound = pstSrcLink->f_Pound;
                        pstDstLink->uw_Capacities = pstSrcLink->uw_Capacities;
                        pstDstLink->uw_CapacitiesInit = pstSrcLink->uw_CapacitiesInit;
                        pstDstLink->uw_Design2 = pstSrcLink->uw_Design2;
                    }
                }
            }
        }    
    }
    else
    {
        /* Only one network. Add links to it */
        if(i_NumNet == 1)
        {
            pst_Net = apst_Present[0];
        }

        /* Add all links in the given network */
        for(i = 0; i < mi_NumLinks - 1; i++)
        {
            pst_GO = mast_LinkNodes[i].pst_GO;
            WAY_pst_CreateStruct(mst_WinHandles.pst_World, pst_GO);
            pst_List = WAY_pst_AddOneLinkList(mast_LinkNodes[i].pst_GO, pst_Net);

            /* Force dest to have the link list, even if no link */
            WAY_pst_CreateStruct(mst_WinHandles.pst_World, mast_LinkNodes[i + 1].pst_GO);
            pst_List1 = WAY_pst_AddOneLinkList(mast_LinkNodes[i + 1].pst_GO, pst_Net);
            WAY_pst_AddOneLink(mast_LinkNodes[i].pst_GO, pst_List, mast_LinkNodes[i + 1].pst_GO);
        }
    }

checkobjects:
	/* Check objects */
	for(i = 0; i < mi_NumLinks - 1; i++)
	{
		WAY_CheckObjectInNetwork(mast_LinkNodes[i].pst_GO);
		ERR_gb_Warning = FALSE;
	}

	if(mpo_AssociatedEditor) mpo_AssociatedEditor->RefreshMenu();

_Catch_
_End_
	mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DisplayWaypointInNetwork;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void sRefresh(void *, void *_p_Sel, void *_p_Data, long)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem		*p_Sel;
	WAY_tdst_AllNetworks	*pst_AllNet;
	BOOL					*p_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_Sel = (EVAV_cl_ViewItem *) _p_Sel;
	p_Data = (BOOL *) _p_Data;
	pst_AllNet = (WAY_tdst_AllNetworks *) p_Sel->mi_Param1;

	if(*p_Data)
		pst_AllNet->ppst_AllNetworks[p_Sel->mi_Param2]->ul_Flags |= WAY_C_DisplayNet;
	else
		pst_AllNet->ppst_AllNetworks[p_Sel->mi_Param2]->ul_Flags &= ~WAY_C_DisplayNet;

	LINK_Refresh();
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DisplayNet(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_CheckListDialog *po_Dialog;
	BOOL					*pb_Displ;
	ULONG					i;
	WAY_tdst_AllNetworks	*pst_AllNet;
	BIG_INDEX				ul_Index;
	char					az[1024];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Dialog = new EDIA_cl_CheckListDialog(F3D_STR_Csz_NetDisplayTitle, CPoint(0, 0));
	pst_AllNet = mst_WinHandles.pst_World->pst_AllNetworks;
	pb_Displ = new BOOL[pst_AllNet->ul_Num];

	for(i = 0; i < pst_AllNet->ul_Num; i++)
	{
		/* Is network visible ? */
		pb_Displ[i] = FALSE;
		if(pst_AllNet->ppst_AllNetworks[i]->ul_Flags & WAY_C_DisplayNet) pb_Displ[i] = TRUE;

		/* Compute network name */
		ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) pst_AllNet->ppst_AllNetworks[i]);
		ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
		ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
		ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

		/* Add one bool item */
		if(pst_AllNet->ppst_AllNetworks[i]->ul_Flags & WAY_C_HasBeenMerge)
			sprintf(az, "%s  (MERGE)", BIG_NameFile(ul_Index));
		else
			sprintf(az, "%s", BIG_NameFile(ul_Index));
		po_Dialog->AddItem(az, EVAV_EVVIT_Bool, &pb_Displ[i], EVAV_None, (int) pst_AllNet, (int) i);
	}

	po_Dialog->mpo_VarsView->SetChangeCallback(sRefresh);
	po_Dialog->DoModal();

	delete pb_Displ;
	LINK_Refresh();
}




/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::RemoveNet(BOOL _bRemoveWaypoints)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_CheckListDialog *po_Dialog;
	BOOL					*pb_Displ;
	ULONG					i, j;
	WAY_tdst_AllNetworks	*pst_AllNet;
	BIG_INDEX				ul_Index;
	char					az[1024];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Dialog = new EDIA_cl_CheckListDialog(F3D_STR_Csz_NetDelTitle, CPoint(0, 0));
	pst_AllNet = mst_WinHandles.pst_World->pst_AllNetworks;
	pb_Displ = new BOOL[pst_AllNet->ul_Num];

	for(i = 0; i < pst_AllNet->ul_Num; i++)
	{
		pb_Displ[i] = FALSE;

		/* Compute network name */
		ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) pst_AllNet->ppst_AllNetworks[i]);
		ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
		ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
		ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

		if(pst_AllNet->ppst_AllNetworks[i]->ul_Flags & WAY_C_HasBeenMerge)
			sprintf(az, "%s  (MERGE - CAN'T DELETE)", BIG_NameFile(ul_Index));
		else
			sprintf(az, "%s", BIG_NameFile(ul_Index));

		/* Add one bool item */
		po_Dialog->AddItem(az, EVAV_EVVIT_Bool, &pb_Displ[i]);
	}

	po_Dialog->DoModal();

	/* Check all networks to delete */
	for(i = 0, j = 0; i < pst_AllNet->ul_Num; i++, j++)
	{
		if(pb_Displ[j])
		{
			if(!(pst_AllNet->ppst_AllNetworks[i]->ul_Flags & WAY_C_HasBeenMerge))
			{
				WAY_DelOneNetwork(mst_WinHandles.pst_World, pst_AllNet->ppst_AllNetworks[i],_bRemoveWaypoints,FALSE);
				i--;
			}
			else
			{
				ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) pst_AllNet->ppst_AllNetworks[i]);
				ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
				sprintf
				(
					az,
					"Can't delete network %s cause it is merged. You must edit original world to delete it",
					BIG_NameFile(ul_Index)
				);
				ERR_X_Warning(0, az, NULL);
			}
		}
	}

    // Refresh dialog box that displays all networks. 

	delete pb_Displ;
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::RemoveLinks(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_CheckListDialog *po_Dialog;
	BOOL					*pb_Displ;
	ULONG					i, j, k, ul_Count;
	BIG_INDEX				ul_Index, ul_Index1;
	OBJ_tdst_GameObject		*pst_GO;
	WAY_tdst_Struct			*pst_Struct;
	char					asz_Name[1024];
	BOOL					b_MustShift;
	BOOL					b_Warn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);
	if(!pst_GO) return;
	if(!pst_GO->pst_Extended) return;
	if(!pst_GO->pst_Extended->pst_Links) return;
	pst_Struct = (WAY_tdst_Struct *) pst_GO->pst_Extended->pst_Links;
	if(!pst_Struct->ul_Num) return;

	/* Count number of links */
	b_Warn = FALSE;
	ul_Count = 0;
	for(i = 0; i < pst_Struct->ul_Num; i++) ul_Count += pst_Struct->pst_AllLinks[i].ul_Num;

	/* To receive result */
	po_Dialog = new EDIA_cl_CheckListDialog(F3D_STR_Csz_LinksDelTitle, CPoint(0, 0));
	pb_Displ = new BOOL[ul_Count];

	/* Add all links in list */
	ul_Count = 0;
	k = 0;
	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		/* Compute network name */
		ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) pst_Struct->pst_AllLinks[i].pst_Network);
		ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
		ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
		ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

		for(j = 0; j < pst_Struct->pst_AllLinks[i].ul_Num; j++)
		{
			pb_Displ[k++] = FALSE;

			/* Name of linked object */
			ul_Index1 = LOA_ul_SearchKeyWithAddress((ULONG) pst_Struct->pst_AllLinks[i].pst_Links[j].pst_Next);
			ERR_X_Assert(ul_Index1 != BIG_C_InvalidIndex);
			ul_Index1 = BIG_ul_SearchKeyToFat(ul_Index1);
			ERR_X_Assert(ul_Index1 != BIG_C_InvalidIndex);

			sprintf(asz_Name, "(%s) => %s", BIG_NameFile(ul_Index), BIG_NameFile(ul_Index1));
			if(pst_Struct->pst_AllLinks[i].pst_Network && pst_Struct->pst_AllLinks[i].pst_Network->ul_Flags & WAY_C_HasBeenMerge)
				L_strcat(asz_Name, " (MERGE - CAN'T DELETE)");
			po_Dialog->AddItem(asz_Name, EVAV_EVVIT_Bool, &pb_Displ[ul_Count]);
			ul_Count++;
		}
	}

	po_Dialog->DoModal();

	/* Delete deselected links */
	ul_Count = 0;
	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		b_MustShift = FALSE;
		for(j = 0; j < pst_Struct->pst_AllLinks[i].ul_Num; j++)
		{
			if(pb_Displ[ul_Count])
			{
				if(pst_Struct->pst_AllLinks[i].pst_Network && pst_Struct->pst_AllLinks[i].pst_Network->ul_Flags & WAY_C_HasBeenMerge)
				{
					NM();
					b_Warn = TRUE;
				}
				else
				{
					if(pst_Struct->pst_AllLinks[i].ul_Num == 1) b_MustShift = TRUE;
					WAY_DelOneLink(pst_GO, &pst_Struct->pst_AllLinks[i], pst_Struct->pst_AllLinks[i].pst_Links[j].pst_Next);
					WAY_CheckObjectInNetwork(pst_GO);
					WAY_CheckObjectInNetwork(pst_Struct->pst_AllLinks[i].pst_Links[j].pst_Next);
					j--;
				}
			}

			ul_Count++;

			if(b_MustShift)
			{
				i--;
				break;
			}
		}

		/* Just in case we have deleted the last link of the object */
		if(!pst_GO->pst_Extended->pst_Links) break;
	}

	delete pb_Displ;

	/* Check objects */
	WAY_CheckObjectInNetwork(pst_GO);
	ERR_gb_Warning = b_Warn;

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::SelectNetRec(OBJ_tdst_GameObject *_pst_GO, BOOL _b_Net)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Struct *pst_Struct;
	ULONG			i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(BAS_bsearch((ULONG) _pst_GO, &WAY_gst_Seen) != -1) return;
	BAS_binsert((ULONG) _pst_GO, (ULONG) _pst_GO, &WAY_gst_Seen);
	if(_b_Net) ForceSelectObject(_pst_GO, FALSE);
	if(!_pst_GO) return;
	if(!_pst_GO->pst_Extended) return;
	if(!_pst_GO->pst_Extended->pst_Links) return;
	pst_Struct = (WAY_tdst_Struct *) _pst_GO->pst_Extended->pst_Links;
	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		for(j = 0; j < pst_Struct->pst_AllLinks[i].ul_Num; j++)
		{
			if(!_b_Net)
			{
				Selection_b_Treat
				(
					&pst_Struct->pst_AllLinks[i].pst_Links[j],
					(long) _pst_GO,
					SEL_C_SIF_Link,
					TRUE,
					TRUE
				);
			}

			SelectNetRec(pst_Struct->pst_AllLinks[i].pst_Links[j].pst_Next, _b_Net);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::SelectNet(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);
	if(!pst_GO) return;
	SelectNetGAO(pst_GO);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::SelectNetGAO(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			i;
	WAY_tdst_Struct *pst_Struct;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	WAY_gst_Seen.num = 0;
	if(!pst_GO->pst_Extended) return;
	if(!pst_GO->pst_Extended->pst_Links) return;
	pst_Struct = (WAY_tdst_Struct *) pst_GO->pst_Extended->pst_Links;
	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		SelectNetRec(pst_Struct->pst_AllLinks[i].pst_Network->pst_Root, TRUE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::SelectLinks(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	WAY_tdst_Struct		*pst_Struct;
	ULONG				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);
	if(!pst_GO) return;
	WAY_gst_Seen.num = 0;

	if(!pst_GO->pst_Extended) return;
	if(!pst_GO->pst_Extended->pst_Links) return;
	pst_Struct = (WAY_tdst_Struct *) pst_GO->pst_Extended->pst_Links;

	Selection_b_Treat(NULL, 0, 0, 0);
	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		SelectNetRec(pst_Struct->pst_AllLinks[i].pst_Network->pst_Root, FALSE);
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::SetRoot(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	WAY_tdst_Struct		*pst_Struct;
	ULONG				i;
	char				asz_Msg[512];
	BIG_INDEX			ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);
	if(!pst_GO) return;
	if(!pst_GO->pst_Extended) return;
	if(!pst_GO->pst_Extended->pst_Links) return;
	pst_Struct = (WAY_tdst_Struct *) pst_GO->pst_Extended->pst_Links;

	for(i = 0; i < pst_Struct->ul_Num; i++)
	{
		if(pst_Struct->pst_AllLinks[i].pst_Network && pst_Struct->pst_AllLinks[i].pst_Network->pst_Root)
		{
			if(pst_Struct->pst_AllLinks[i].pst_Network->ul_Flags & WAY_C_HasBeenMerge)
			{
				NM();
				continue;
			}
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Struct->pst_AllLinks[i].pst_Network);
			ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
			sprintf(asz_Msg, "Set Root for \"%s\" ?", BIG_NameFile(ul_Index));
			if(M_MF()->MessageBox(asz_Msg, "Please confirm", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				pst_Struct->pst_AllLinks[i].pst_Network->pst_Root = pst_GO;
			}
		}
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::CreateNetworkFromSelection(void)
{
    int i;
    OBJ_tdst_GameObject **pp_SrcNodes,**pp_DstNodes;

    // Duplicate selected waypoints
    Selection_GAO_Duplicate(NULL,TRUE,&pp_SrcNodes,&pp_DstNodes,&mi_NumLinks);

    // Build mast_LinkNodes before call of CreateNetwork.
    for (i=0; i<mi_NumLinks; i++)
        mast_LinkNodes[i].pst_GO = pp_DstNodes[i];

    // Create network on copied nodes using links from src nodes.
    CreateNetwork(pp_SrcNodes);

    L_free(pp_SrcNodes);
    L_free(pp_DstNodes);
}

#endif
