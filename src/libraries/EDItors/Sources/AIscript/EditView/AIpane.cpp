/*$T AIpane.cpp GC!1.71 02/18/00 10:23:22 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"

#include "EDImainframe.h"
#include "EDImsg.h"
#include "EDIpaths.h"

#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINKs/LINKtoed.h"
#include "LINks/LINKmsg.h"

#include "../AIframe.h"
#include "AIpane.h"
#include "AIview.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"

#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDItors/Sources/OUTput/OUTframe.h"

#include "EDItors/Sources/PERForce/PERCDataCtrl.h"
#include "EDItors/Sources/PERForce/PERmsg.h"





/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

#define COLMDL  0x009F0000
#define COLINS  0x0000009F
#define COLPOS	0x00009F00
#define COLNOCO 0x000080FF
#define COLCOE  0x0000009F

extern BOOL EDI_gb_NoUpdateVSS;
extern BOOL	EDI_gb_CheckInDel;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EAI_cl_Pane, CEdit)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_RBUTTONDOWN()
    ON_WM_MBUTTONDOWN()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EAI_cl_Pane::EAI_cl_Pane(int _i_Num)
{
    mi_NumPane = _i_Num;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EAI_cl_Pane::~EAI_cl_Pane(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
HBRUSH EAI_cl_Pane::CtlColor(CDC *pDC, UINT nCtlColor)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    static HBRUSH   hbrmodel = 0;
    static HBRUSH   hbrins = 0;
    static HBRUSH   hbrnocheckout = 0;
    static HBRUSH   hbrcheckoutext = 0;
    static HBRUSH   hbrcheckoutme = 0;
    HBRUSH          hbr;
    char            asz_CurrentOwner[256];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    hbr = 0;
    if(!hbrmodel)
    {
        hbrmodel = CreateSolidBrush(COLMDL);
        hbrins = CreateSolidBrush(COLINS);
        hbrnocheckout = CreateSolidBrush(COLNOCO);
        hbrcheckoutext = CreateSolidBrush(COLCOE);
        hbrcheckoutme = CreateSolidBrush(COLPOS);
    }

    /* Model. Checkout or not */
    if(mi_NumPane == 1)
    {
        //if(M_MF()->mst_Ini.b_LinkControlON)
        {
            if(mpo_Frame->mul_CurrentEditFile != BIG_C_InvalidIndex)
            {
                L_strcpy(asz_CurrentOwner, BIG_P4OwnerName(mpo_Frame->mul_CurrentEditFile));
				if(!L_strnicmp(asz_CurrentOwner, "_nobody_", 8)) *asz_CurrentOwner = 0;
                if(*asz_CurrentOwner)
                {
                    if(L_strcmpi(asz_CurrentOwner, M_MF()->mst_Ini.asz_CurrentUserName))
                    {
                        hbr = hbrcheckoutext;
                        pDC->SetBkColor(COLCOE);
                        pDC->SetTextColor(0x00FFFFFF);
                    }
					else
					{
                        hbr = hbrcheckoutme;
                        pDC->SetBkColor(COLPOS);
                        pDC->SetTextColor(0x00FFFFFF);
					}
                }
                else
                {
                    hbr = hbrnocheckout;
                    pDC->SetBkColor(COLNOCO);
                    pDC->SetTextColor(0x00FFFFFF);
                }
            }
        }
    }

    /* Info. Display model or instance */
    else if(mi_NumPane == 3)
    {
        if(mpo_Frame->mpst_Model)
        {
            if(mpo_Frame->mpst_Instance)
            {
                hbr = hbrins;
                pDC->SetBkColor(COLINS);
            }
            else
            {
                hbr = hbrmodel;
                pDC->SetBkColor(COLMDL);
            }

            pDC->SetTextColor(0x00FFFFFF);
        }
    }

    return hbr;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Pane::OnPaint(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CDC         *pdc;
    CRect       o_Rect;
    COLORREF    col;
    char        asz_CurrentOwner[256];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    CEdit::OnPaint();

    /* Checkout pane */
    if(mi_NumPane == 1)
    {
        //if(M_MF()->mst_Ini.b_LinkControlON)
        {
            if(mpo_Frame->mul_CurrentEditFile != BIG_C_InvalidIndex)
            {
                col = 0;
                L_strcpy(asz_CurrentOwner, BIG_P4OwnerName(mpo_Frame->mul_CurrentEditFile));
				if(!L_strnicmp(asz_CurrentOwner, "_nobody_", 8)) *asz_CurrentOwner = 0;
                if(*asz_CurrentOwner)
                {
                    if(L_strcmpi(asz_CurrentOwner, M_MF()->mst_Ini.asz_CurrentUserName))
                        col = COLCOE;
					else
                        col = COLPOS;
                }
                else
                    col = COLNOCO;
                if(col)
                {
                    pdc = GetDC();
                    GetClientRect(&o_Rect);
                    pdc->Draw3dRect
                        (
                            &o_Rect,
                            M_MF()->u4_Interpol2PackedColor(col, 0x00FFFFFF, 0.5f),
                            M_MF()->u4_Interpol2PackedColor(col, 0, 0.5f)
                        );
                    ReleaseDC(pdc);
                }
            }
        }
    }

    /* Model/instance pane */
    else if(mi_NumPane == 3)
    {
        if(mpo_Frame->mpst_Model)
        {
            if(mpo_Frame->mpst_Instance)
                col = COLINS;
            else
                col = COLMDL;
            pdc = GetDC();
            GetClientRect(&o_Rect);
            pdc->Draw3dRect
                (
                    &o_Rect,
                    M_MF()->u4_Interpol2PackedColor(col, 0x00FFFFFF, 0.5f),
                    M_MF()->u4_Interpol2PackedColor(col, 0, 0.5f)
                );
            ReleaseDC(pdc);
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern BOOL EDI_gb_BatchModeWithoutPerforce;

void EAI_cl_Pane::OnRButtonDown(UINT, CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EMEN_cl_SubMenu     o_Menu(FALSE);
    int                 i_Res;
    BIG_INDEX           ul_File;
    TAB_tdst_PFelem     *pst_CurrentElemW;
    TAB_tdst_PFelem     *pst_EndElemW;
    WOR_tdst_World      *pst_World;
    AI_tdst_Instance    *pst_Instance;
    TAB_tdst_PFtable    *pst_AIEOT;
    TAB_tdst_PFelem     *pst_CurrentElem;
    TAB_tdst_PFelem     *pst_EndElem;
    OBJ_tdst_GameObject *pst_GAO;
    CString             o_Name;
    BOOL                b_First, b_Disable;
	OBJ_tdst_GameObject	*pst_GO;
	EOUT_cl_Frame		*po_3DV;
	EDI_cl_Action		*po_Act;
	char				asz_Path[BIG_C_MaxLenPath];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    enum EAIRightButtonSecondPane
    {
        EAIRightButtonSecondPane_Nothing,
        EAIRightButtonSecondPane_ShowInBrowser,
        EAIRightButtonSecondPane_GetLatestVersion,
        EAIRightButtonSecondPane_CheckOut,
        EAIRightButtonSecondPane_CheckIn,
        EAIRightButtonSecondPane_CheckInOut,
        EAIRightButtonSecondPane_UndoCheckOut
    };

	/* Drag & Drop */
    if(EDI_gst_DragDrop.b_BeginDragDrop) 
	{
		M_MF()->CancelDragDrop();
		return;
	}
	
	if ( mi_NumPane == 2 )
	{
		if(mpo_Frame->mul_CurrentEditModel == BIG_C_InvalidIndex)
            return;
			
		ClientToScreen(&pt);
        M_MF()->InitPopupMenuAction(mpo_Frame, &o_Menu);
        M_MF()->AddPopupMenuAction( mpo_Frame, &o_Menu, EAIRightButtonSecondPane_ShowInBrowser, TRUE, "Show in browser" );
        
        b_Disable = 0;
        if ( !M_MF()->mst_Ini.b_LinkControlON ) b_Disable = 1;
        
        M_MF()->AddPopupMenuAction( mpo_Frame, &o_Menu, EAIRightButtonSecondPane_Nothing, TRUE, "" );

        if (M_MF()->mst_Ini.b_LinkControlON && EDI_gb_BatchModeWithoutPerforce)
        {
    		M_MF()->AddPopupMenuAction( mpo_Frame, &o_Menu, EAIRightButtonSecondPane_GetLatestVersion, TRUE, "Get latest version" );
    		M_MF()->AddPopupMenuAction( mpo_Frame, &o_Menu, EAIRightButtonSecondPane_CheckOut, TRUE, "Check out" );
            po_Act = (EDI_cl_Action *) M_MF()->mo_TrackPopupMenuList.GetAt( M_MF()->mo_TrackPopupMenuActionList.GetTailPosition() );
            po_Act->mb_Disabled = b_Disable;

            M_MF()->AddPopupMenuAction( mpo_Frame, &o_Menu, EAIRightButtonSecondPane_CheckIn, TRUE, "Check in" );
            po_Act = (EDI_cl_Action *) M_MF()->mo_TrackPopupMenuList.GetAt( M_MF()->mo_TrackPopupMenuActionList.GetTailPosition() );
            po_Act->mb_Disabled = b_Disable;

            M_MF()->AddPopupMenuAction( mpo_Frame, &o_Menu, EAIRightButtonSecondPane_CheckInOut, TRUE, "Check in out" );
            po_Act = (EDI_cl_Action *) M_MF()->mo_TrackPopupMenuList.GetAt( M_MF()->mo_TrackPopupMenuActionList.GetTailPosition() );
            po_Act->mb_Disabled = b_Disable;

            M_MF()->AddPopupMenuAction( mpo_Frame, &o_Menu, EAIRightButtonSecondPane_UndoCheckOut, TRUE, "Undo check out" );
            po_Act = (EDI_cl_Action *) M_MF()->mo_TrackPopupMenuList.GetAt( M_MF()->mo_TrackPopupMenuActionList.GetTailPosition() );
            po_Act->mb_Disabled = b_Disable;
        }
		
		i_Res = M_MF()->TrackPopupMenuAction(mpo_Frame, pt, &o_Menu);
        /* Treat result */
        if(i_Res && (i_Res != -1)) 
		{
			if ( i_Res == EAIRightButtonSecondPane_ShowInBrowser )
			{   
				EBRO_cl_Frame   *po_Browser;
				po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
				po_Browser->i_OnMessage( EDI_MESSAGE_SELFILE, BIG_ParentFile( mpo_Frame->mul_CurrentEditFile ), mpo_Frame->mul_CurrentEditFile );
			}
			else
			{
				PER_CDataCtrlEmulator PerforceDataTree; 
				PerforceDataTree.AddIndex(mpo_Frame->mul_CurrentEditFile);
				
				EAI_cl_Frame* pAiFrame =(EAI_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_AI, 0);
				pAiFrame->mb_P4CloseWorld = FALSE;

				BIG_ComputeFullName(BIG_ParentFile(mpo_Frame->mul_CurrentEditFile), asz_Path);
				switch( i_Res )
				{
				case EAIRightButtonSecondPane_GetLatestVersion:
					AfxGetApp()->DoWaitCursor(1);
					mpo_Frame->OnSave();
                    M_MF()->GetLatestVersionFile(asz_Path, BIG_NameFile(mpo_Frame->mul_CurrentEditFile));
					EDI_gb_NoUpdateVSS = TRUE;
					mpo_Frame->UpdateOwner();
					LINK_gb_RefreshOnlyFiles = TRUE;
					M_MF()->FatHasChanged();
					AfxGetApp()->DoWaitCursor(-1);
					break;
				case EAIRightButtonSecondPane_CheckOut:
					AfxGetApp()->DoWaitCursor(1);
					mpo_Frame->OnSave();
					M_MF()->CheckOutFile(asz_Path, BIG_NameFile(mpo_Frame->mul_CurrentEditFile));
					EDI_gb_NoUpdateVSS = TRUE;
					mpo_Frame->UpdateOwner();
					LINK_gb_RefreshOnlyFiles = TRUE;
					M_MF()->FatHasChanged();
					AfxGetApp()->DoWaitCursor(-1);
					break;
				case EAIRightButtonSecondPane_CheckIn:
					AfxGetApp()->DoWaitCursor(1);
					mpo_Frame->OnSave();
					EDI_gb_CheckInDel = FALSE;
					M_MF()->CheckInFile(asz_Path, BIG_NameFile(mpo_Frame->mul_CurrentEditFile));
					EDI_gb_NoUpdateVSS = TRUE;
					mpo_Frame->UpdateOwner();
					LINK_gb_RefreshOnlyFiles = TRUE;
					AfxGetApp()->DoWaitCursor(-1);
					break;
				case EAIRightButtonSecondPane_CheckInOut:
					AfxGetApp()->DoWaitCursor(1);
					mpo_Frame->OnSave();
					EDI_gb_CheckInDel = FALSE;
					M_MF()->CheckInFileNoOut(asz_Path, BIG_NameFile(mpo_Frame->mul_CurrentEditFile));
					EDI_gb_NoUpdateVSS = TRUE;
					mpo_Frame->UpdateOwner();
					LINK_gb_RefreshOnlyFiles = TRUE;
					AfxGetApp()->DoWaitCursor(-1);
					break;
				
				case EAIRightButtonSecondPane_UndoCheckOut:
					M_MF()->UndoCheckOutFile(asz_Path, BIG_NameFile(mpo_Frame->mul_CurrentEditFile));
					EDI_gb_NoUpdateVSS = TRUE;
					LINK_gb_RefreshOnlyFiles = TRUE;
					break;
					

				case EAI_ACTION_P4_EDIT:
					{
						LINK_SendMessageToEditors(EPER_MESSAGE_EDIT_SELECTED, ((ULONG)&PerforceDataTree),0);
					}
					break;
				case EAI_ACTION_P4_REVERT:
					{
						LINK_SendMessageToEditors(EPER_MESSAGE_REVERT_SELECTED, ((ULONG)&PerforceDataTree),0);
					}
					break;
				case EAI_ACTION_P4_SYNC		:
					{
						LINK_SendMessageToEditors(EPER_MESSAGE_SYNC_SELECTED, ((ULONG)&PerforceDataTree),0);
					}
					break;
				case EAI_ACTION_P4_FORCESYNC	:
					{
						LINK_SendMessageToEditors(EPER_MESSAGE_FORCESYNC_SELECTED, ((ULONG)&PerforceDataTree),0);
					}
					break;
				case EAI_ACTION_P4_ADD		:
					{
						LINK_SendMessageToEditors(EPER_MESSAGE_ADD_SELECTED, ((ULONG)&PerforceDataTree),0);
					}
					break;
				case EAI_ACTION_P4_DELETE		:
					{
						LINK_SendMessageToEditors(EPER_MESSAGE_DELETE_SELECTED, ((ULONG)&PerforceDataTree),0);
					}
					break;
				case EAI_ACTION_P4_SUBMIT		:
					{
						LINK_SendMessageToEditors(EPER_MESSAGE_SUBMIT_SELECTED, ((ULONG)&PerforceDataTree),0);
					}
					break;
				case EAI_ACTION_P4_SUBMITEDIT	:
					{
						LINK_SendMessageToEditors(EPER_MESSAGE_SUBMITEDIT_SELECTED, ((ULONG)&PerforceDataTree),0);
					}
					break;
				case EAI_ACTION_P4_DIFF		:
					{
						LINK_SendMessageToEditors(EPER_MESSAGE_DIFF_SELECTED, ((ULONG)&PerforceDataTree),0);
					}
					break;
				case EAI_ACTION_P4_SHOWHISTORY:
					{
						LINK_SendMessageToEditors(EPER_MESSAGE_SHOWHISTORY_SELECTED, ((ULONG)&PerforceDataTree),0);
					}
					break;
				}
				pAiFrame->mb_P4CloseWorld = TRUE;
				pAiFrame->RefreshDialogBar();
				pAiFrame->DisplayPaneNames();
				
				LINK_Refresh();
			}
		}
	}

    if(mi_NumPane == 3)
    {
        if(mpo_Frame->mul_CurrentEditModel == BIG_C_InvalidIndex)
            return;
        ClientToScreen(&pt);
        M_MF()->InitPopupMenuAction(mpo_Frame, &o_Menu);

        /* Model */
        if(mpo_Frame->mpst_Instance)
        {
			if(mpo_Frame->mpst_Instance->pst_GameObject)
			{
				M_MF()->AddPopupMenuAction
					(
						mpo_Frame,
						&o_Menu,
						1,
						TRUE,
						((OBJ_tdst_GameObject *) mpo_Frame->mpst_Instance->pst_GameObject)->sz_Name
					);
			}
            M_MF()->AddPopupMenuAction
                (
                    mpo_Frame,
                    &o_Menu,
                    0,
                    TRUE,
                    NULL
                );

            ul_File = LOA_ul_SearchIndexWithAddress((ULONG) mpo_Frame->mpst_Instance->pst_Model);
            ERR_X_Assert(ul_File != BIG_C_InvalidIndex);
            M_MF()->AddPopupMenuAction
                (
                    mpo_Frame,
                    &o_Menu,
                    ul_File,
                    TRUE,
                    BIG_NameFile(mpo_Frame->mul_CurrentEditModel)
                );
        }

        /* List of instance of the model (in all worlds) */
        pst_CurrentElemW = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
        pst_EndElemW = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
        for(pst_CurrentElemW; pst_CurrentElemW <= pst_EndElemW; pst_CurrentElemW++)
        {
            pst_World = (WOR_tdst_World *) pst_CurrentElemW->p_Pointer;
            b_First = TRUE;
            if(!TAB_b_IsAHole(pst_World))
            {
                pst_AIEOT = &(pst_World->st_AllWorldObjects);
                pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
                pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
                for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
                {
                    pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
                    if(TAB_b_IsAHole(pst_GAO)) continue;

                    if((pst_GAO->pst_Extended) && (pst_GAO->pst_Extended->pst_Ai))
                    {
                        pst_Instance = (AI_tdst_Instance *) pst_GAO->pst_Extended->pst_Ai;
                        if(pst_Instance->pst_Model == mpo_Frame->mpst_Model)
                        {
                            ul_File = LOA_ul_SearchKeyWithAddress((ULONG) pst_Instance);
                            if(ul_File != BIG_C_InvalidIndex)
                            {
                                ul_File = BIG_ul_SearchKeyToFat(ul_File);
                                if(ul_File != mpo_Frame->mul_CurrentEditInstance)
                                {
                                    /* Separator with the name of the world */
                                    if(b_First)
                                    {
                                        M_MF()->AddPopupMenuAction(mpo_Frame, &o_Menu, 0, TRUE, pst_World->sz_Name);
                                        b_First = FALSE;
                                    }

                                    /* Instance */
                                    M_MF()->AddPopupMenuAction
                                        (
                                            mpo_Frame,
                                            &o_Menu,
                                            ul_File,
                                            TRUE,
                                            BIG_NameFile(ul_File)
                                        );
                                }
                            }
                        }
                    }
                }
            }
        }

        /* Popup */
        i_Res = M_MF()->TrackPopupMenuAction(mpo_Frame, pt, &o_Menu);

        /* Treat result */
        if(i_Res && (i_Res != -1)) 
		{
			if(i_Res == 1)
			{
				pst_GO = (OBJ_tdst_GameObject *) mpo_Frame->mpst_Instance->pst_GameObject;
				po_3DV = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
				po_3DV->mpo_EngineFrame->mpo_DisplayView->ForceSelectObject(pst_GO);
				po_3DV->mpo_EngineFrame->mpo_DisplayView->ZoomExtendSelected
					(
						&po_3DV->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData->st_Camera,
						po_3DV->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World
					);
				LINK_Refresh();
			}
			else
			{
				mpo_Frame->i_OnMessage(EDI_MESSAGE_SELFILE, 0, i_Res);
			}
		}
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Pane::OnMButtonDown(UINT, CPoint)
{
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Pane::OnLButtonDown(UINT, CPoint pt)
{
	if(mpo_Frame->mul_CurrentEngineModel != BIG_C_InvalidIndex)
	{
		if(mpo_Frame->mpst_Instance)
			EDI_gst_DragDrop.i_Param2 = (int) mpo_Frame->mpst_Instance;
		else
			EDI_gst_DragDrop.i_Param2 = (int) mpo_Frame->mpst_Model;
		if(EDI_gst_DragDrop.i_Param2)
		{
	        ClientToScreen(&pt);
			M_MF()->BeginDragDrop(pt, this, mpo_Frame, EDI_DD_Data);
		}
		else if(mpo_Frame->mul_CurrentEditModel != BIG_C_InvalidIndex)
		{
			EDI_gst_DragDrop.ul_FatDir = BIG_ParentFile(mpo_Frame->mul_CurrentEditModel);
			EDI_gst_DragDrop.ul_FatFile = mpo_Frame->mul_CurrentEditModel;
	        ClientToScreen(&pt);
			M_MF()->BeginDragDrop(pt, this, mpo_Frame, EDI_DD_File);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Pane::OnLButtonUp(UINT, CPoint pt)
{
    if(EDI_gst_DragDrop.b_BeginDragDrop) M_MF()->EndDragDrop(pt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Pane::OnMouseMove(UINT, CPoint pt)
{
    M_MF()->b_MoveDragDrop(pt);
}

#endif /* ACTIVE_EDITORS */
