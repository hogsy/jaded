/*$T F3Dview.cpp GC! 1.081 05/02/00 12:29:21 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDIapp.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dview_undo.h"
#include "F3Dframe/F3Dstrings.h"
#include "CAMera/CAMera.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKmsg.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "INOut/INOkeyboard.h"
#include "LINKs/LINKtoed.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/ENGcall.h"
#include "ENGine/Sources/WORld/WORrender.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/EOT/EOTmain.h"
#include "GEOmetric/GEOdebugobject.h"
#include "SOFT/SOFThelper.h"
#include "SOFT/SOFTbackgroundimage.h"
#include "TIMer/TIMdefs.h"
#include "GDInterface/GDIrasters.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "DIAlogs/DIAselection_dlg.h"
#include "DIAlogs/DIAanim_dlg.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAshape_dlg.h"
#include "DIAlogs/DIAbackgroundImage_dlg.h"
#include "DIAlogs/DIAcreategeometry_dlg.h"
#include "DIAlogs/DIAgamematerial_dlg.h"
#include "DIAlogs/DIAtoolbox_groview.h"
#include "DIAlogs/DIAgrovertexpos_dlg.h"
#include "DIAlogs/DIAbrickmapper_dlg.h"
#include "SouND/sources/SND.h"
#include "TEXture/TEXprocedural.h"
#include "TEXture/TEXanimated.h"
#include "AIinterp/Sources/AIengine.h"

#ifdef JADEFUSION
#include "DIAlogs/DIAlightrej_dlg.h"
#include "DIAlogs/DIAcubemapgen_dlg.h"
#include "DIAlogs/DIAScreenshotSeqBuilder_dlg.h"
#endif
/*$4
 ***********************************************************************************************************************
    MESSAGE MAP.
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNCREATE(F3D_cl_View, CWnd)
BEGIN_MESSAGE_MAP(F3D_cl_View, CWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    Constructor/destructor
 ***********************************************************************************************************************
 */
#ifdef JADEFUSION
extern ULONG g_ul_BIG_SNAPSHOT_COUNTER;
extern BOOL GRI_gb_Edit;
#else
extern "C" ULONG g_ul_BIG_SNAPSHOT_COUNTER;
extern "C" BOOL GRI_gb_Edit;
#endif
/*
 =======================================================================================================================
    Aim:    Construct F3D_cl_View object
 =======================================================================================================================
 */
F3D_cl_View::F3D_cl_View(void)
{
	L_memset(&mst_WinHandles, 0, sizeof(MAI_tdst_WinHandles));
	mb_Capture = FALSE;

	mpst_BonePickObj = NULL;
	ml_MouseWheelFactor = 20;
	mb_ConstantZoomSpeed = FALSE;
	mi_ZoomSpeed = 1;
	mpo_AssociatedEditor = NULL;
	mb_ZoomOn = FALSE;
	mb_ToolBoxOn = FALSE;
	mb_SelBoxOn = FALSE;
	mb_AnimOn = FALSE;
	*msz_AssociatedMadFile = 0;

	char tmp[ MAX_PATH ];
	getcwd( tmp, sizeof( tmp ) );
	snprintf( msz_ExportDir, sizeof( msz_ExportDir ), "%s/exports", tmp );

	mi_FreezeHelper = -1;

	mb_BeginModePane = FALSE;
	mb_BeginModeRotate = FALSE;
	mb_WaitForLButton = FALSE;
	mb_WaitForMButton = FALSE;
	mb_WaitForRButton = FALSE;
    mb_LButtonDblclk = FALSE;
	mb_AutoZoomExtendOnRotate = FALSE;
	mi_BackgroundImageMove = 0;

	mb_LockPickObj = FALSE;
	mpst_LockPickObj = NULL;

	MATH_InitVectorToZero(&mst_Move);
    mpo_CurrentModif = NULL;

	mb_LinkOn = FALSE;
    mb_SubObject_LinkOn = FALSE;
	mi_NumLinks = 0;
	mb_CanRefresh = TRUE;
	mpo_SelectionDialog = NULL;
	mpo_ToolBoxDialog = NULL;
	mpo_AnimDialog = NULL;
	mpo_BackgroundImageDialog = NULL;
    mpo_CreateGeometryDialog = NULL;
    mpo_GameMaterialDialog = NULL;
    mpo_GaoInfoDialog = NULL;
    mpo_VertexPos = NULL;
	mpo_BrickMapper = NULL;
    mb_GridEdit = FALSE;
	GRI_gb_Edit = FALSE;

	mi_CurCamera = -1;
	mb_SelectOn = FALSE;
	mb_DuplicateOn = FALSE;

    mb_EditHierarchy = FALSE;
	mi_EditedCurve = -1;
	mpst_EditedPortal = NULL;
    mb_SelectionChange = FALSE;
    mb_CanSelSomeSubObject = FALSE;

	mb_WorkWithGRORLI = FALSE;

	mb_LastClickInRightView = FALSE;

#ifdef JADEFUSION
	mb_LightRejectOn = FALSE;
	mpo_LightRejectDialog = NULL;
	mpo_CheckWorldDialog = NULL;
	mpo_ReplaceDialog = NULL;
	mpo_CubeMapGenDialog = NULL;
	mpo_ScreenshotSeqBuilderDialog = NULL;

#endif

}

/*
 =======================================================================================================================
    Aim:    Finish F3D_cl_View class
 =======================================================================================================================
 */
F3D_cl_View::~F3D_cl_View(void)
{
	mst_WinHandles.pst_World = NULL;

	if(mpo_SelectionDialog)
	{
		mpo_SelectionDialog->DestroyWindow();
		delete mpo_SelectionDialog;
		mpo_SelectionDialog = NULL;
	}

	if(mpo_ToolBoxDialog)
	{
		mpo_ToolBoxDialog->DestroyWindow();
		delete mpo_ToolBoxDialog;
		mpo_ToolBoxDialog = NULL;
	}

#ifdef JADEFUSION
	// NB: Added Light Rejection dialog - from POP5 Jade Editor	
	if(mpo_LightRejectDialog)
	{
		mpo_LightRejectDialog->DestroyWindow();
		delete mpo_LightRejectDialog;
		mpo_LightRejectDialog = NULL;
	}
#endif

	if(mpo_AnimDialog)
	{
		mpo_AnimDialog->DestroyWindow();
		delete mpo_AnimDialog;
		mpo_AnimDialog = NULL;
	}

    if (mpo_BackgroundImageDialog)
    {
        mpo_BackgroundImageDialog->mpo_View = NULL;
    }

    if (mpo_CreateGeometryDialog)
    {
        mpo_CreateGeometryDialog->DestroyWindow();
        delete mpo_CreateGeometryDialog;
        mpo_CreateGeometryDialog = NULL;
    }

    if (mpo_GameMaterialDialog)
    {
        mpo_GameMaterialDialog->DestroyWindow();
        delete mpo_GameMaterialDialog;
        mpo_GameMaterialDialog = NULL;
    }

#ifdef JADEFUSION
	if(mpo_CubeMapGenDialog)
	{
		mpo_CubeMapGenDialog->DestroyWindow();
		delete mpo_CubeMapGenDialog;
		mpo_CubeMapGenDialog = NULL;
	}

	if(mpo_ScreenshotSeqBuilderDialog)
	{
		mpo_ScreenshotSeqBuilderDialog->DestroyWindow();
		delete mpo_ScreenshotSeqBuilderDialog;
		mpo_ScreenshotSeqBuilderDialog = NULL;
	}
#endif

	if (mpo_BrickMapper)
    {
        mpo_BrickMapper->DestroyWindow();
        delete mpo_BrickMapper;
        mpo_BrickMapper = NULL;
    }

    Selection_SubObject_DestroyVertexPos();
}

/*$4
 ***********************************************************************************************************************
    Main functions:: message loop and refresh
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Window callback
 =======================================================================================================================
 */
LRESULT F3D_cl_View::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~*/
	LRESULT result;
	/*~~~~~~~~~~~*/

	if(MAI_b_TreatDisplayWndMessages(m_hWnd, &mst_WinHandles, message, wParam, lParam, &result)) return result;

	/* Init rasters */
	if(message == WM_CREATE)
	{
        char *pc_Cur;
#ifdef RASTERS_ON
		GDI_Rasters_Init(mst_WinHandles.pst_DisplayData->pst_Raster, mpo_AssociatedEditor->mst_Def.asz_Name);
#endif
        L_strcpy(M_F3D_DD->sz_SnapshotName, EDI_go_TheApp.m_pszHelpFilePath);
        pc_Cur = strrchr( M_F3D_DD->sz_SnapshotName, '/' );
        if (pc_Cur == NULL)
            pc_Cur = strrchr( M_F3D_DD->sz_SnapshotName, '\\' );
        if (pc_Cur == NULL)
            L_strcpy(M_F3D_DD->sz_SnapshotName, "Capture");
        else
        {
            strcpy( pc_Cur + 1, "Capture" );
            CreateDirectory( M_F3D_DD->sz_SnapshotName, NULL );
            strcpy( pc_Cur + 8, "\\Capture" );
        }
	}

	return CWnd::DefWindowProc(message, wParam, lParam);
}

/*
 =======================================================================================================================
    Aim:    Update 3D display
 =======================================================================================================================
 */
void F3D_cl_View::Refresh(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ulDM;
	WOR_tdst_World	*pst_World;
	EOUT_cl_Frame	*po_Out;
	extern BOOL		LINK_gb_EditRefresh;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!LINK_gb_CanRefresh) return;
	if(!mb_CanRefresh || mb_LinkOn || mb_SubObject_LinkOn) mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DoNotRender;

	/* Get out editor */
	po_Out = (EOUT_cl_Frame *) mpo_AssociatedEditor;
	if(!mst_WinHandles.pst_DisplayData->h_Wnd) return;

Redraw_Label:

	if((!mb_ZoomOn) && (!mb_SelBoxOn))
	{
#ifndef _XENON_RENDER
		if(!LINK_gb_EditRefresh) GDI_AfterDisplay(mst_WinHandles.pst_DisplayData);
#endif
		GDI_BeforeDisplay(mst_WinHandles.pst_DisplayData);
        SOFT_Helpers_Reset(M_F3D_Helpers);

		pst_World = mst_WinHandles.pst_DisplayData->pst_World;
		if(pst_World)
		{
			/* Associate a display data with the view */
			if(po_Out)
			{
				pst_World->pst_View[po_Out->mi_NumEdit].st_DisplayInfo.pst_DisplayDatas = mst_WinHandles.pst_DisplayData;
			}
			else
			{
				pst_World->pst_View[0].st_DisplayInfo.pst_DisplayDatas = mst_WinHandles.pst_DisplayData;
			}
			if (pst_World->b_IsSplitScreen)
				pst_World->pst_View[pst_World->i_SplitViewIdx].st_DisplayInfo.pst_DisplayDatas = mst_WinHandles.pst_DisplayData;

			if(EDI_go_TheApp.mb_RunEngine)
			{
				mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_UpdateTrace;
			}
			else
			{
				mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_UpdateTrace;
			}

            SOFT_BackgroundImage_Update(mst_WinHandles.pst_DisplayData->pst_BackgroundImage, mst_WinHandles.pst_DisplayData);

			if((pst_World == mst_WinHandles.pst_World) || !EDI_go_TheApp.mb_RunEngine)
			{
				if (pst_World->b_IsSplitScreen)
					pst_World->pst_View[pst_World->i_SplitViewIdx].pfnv_ViewPointModificator(&pst_World->pst_View[pst_World->i_SplitViewIdx]);
				if(po_Out)
				{
					/* We make the view follow its father. */
					pst_World->pst_View[po_Out->mi_NumEdit].pfnv_ViewPointModificator(&pst_World->pst_View[po_Out->mi_NumEdit]);
					WOR_World_MakeObjectsVisible(pst_World);
				}
				else
				{
					pst_World->pst_View[0].pfnv_ViewPointModificator(&pst_World->pst_View[0]);
					WOR_World_MakeObjectsVisible(pst_World);
				}

				if(!AI_gb_ExitByBreak || AI_gb_ErrorWhenBreak)
				{
					WOR_World_ActivateObjects(pst_World);
					EOT_SetOfEOT_Build(&pst_World->st_EOT, &pst_World->st_ActivObjects);
				}

				/* Sinon ca clear le Z buffer */
				if(M_F3D_Helpers->pv_Prefab)
				{
					ulDM = mst_WinHandles.pst_DisplayData->ul_DisplayFlags;
					mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_HideInterface;
				}

				CreatePostIt();
				WOR_Render(pst_World, mst_WinHandles.pst_DisplayData);
				DestroyPostIt();

				if(M_F3D_Helpers->pv_Prefab)
				{
					mst_WinHandles.pst_DisplayData->ul_DisplayFlags = ulDM;
				}
			}
			else
			{
				CreatePostIt();
				WOR_Render(pst_World, mst_WinHandles.pst_DisplayData);
				DestroyPostIt();
			}
		}

		Helper_Render();
#ifndef _XENON_RENDER
		if(LINK_gb_EditRefresh)
#endif
			GDI_AfterDisplay(mst_WinHandles.pst_DisplayData);

        /* procedural texture */
        TEX_Procedural_Update( mst_WinHandles.pst_DisplayData );
        /* animated texture */
        TEX_Anim_Update( mst_WinHandles.pst_DisplayData );

        if (mpo_ToolBoxDialog && Selection_b_IsInSubObjectMode() )
            ((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->UpdateNumberOfSubSel();
	}

	if ((g_ul_BIG_SNAPSHOT_COUNTER) && (!(g_ul_BIG_SNAPSHOT_COUNTER & 0x0f)))
	{
		g_ul_BIG_SNAPSHOT_COUNTER = 0;
		goto Redraw_Label;
	}
	if (g_ul_BIG_SNAPSHOT_COUNTER)
	{
		g_ul_BIG_SNAPSHOT_COUNTER--;
		goto Redraw_Label;
	}

	mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_DoNotRender;
}

/*$4
 ***********************************************************************************************************************
    Edition mode
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim return TRUE is given mode is validated
 =======================================================================================================================
 */
BOOL F3D_cl_View::IsActionValidate(UINT _ui_ID)
{
	switch(_ui_ID)
	{
	case F3D_Action_CameraOn:
		if(GetAsyncKeyState(VK_LMENU) < 0) return TRUE;
		if(GetAsyncKeyState(VK_SPACE) < 0) return TRUE;
		break;

	case F3D_Action_CameraOn1:
		if(GetAsyncKeyState(VK_LMENU) < 0) return TRUE;
		break;

	case F3D_Action_CameraOn2:
		if(GetAsyncKeyState(VK_SPACE) < 0) return TRUE;
		break;

	case F3D_Action_ChangeConstraint:
		if(GetAsyncKeyState('W') < 0) return TRUE;
		break;

	case F3D_Action_AddWaypoint:
		if((GetAsyncKeyState(VK_SHIFT) < 0) && (GetAsyncKeyState(VK_CONTROL) >= 0)) return TRUE;
		break;

	case F3D_Action_AddInvisible:
		if(GetAsyncKeyState(VK_RMENU) < 0) return TRUE;
		break;

	case F3D_Action_Link:
		if((GetAsyncKeyState(VK_SHIFT) < 0) && (GetAsyncKeyState(VK_CONTROL)) < 0)
		{
			if(!mb_EditHierarchy) return TRUE;
		}
		break;

	case F3D_Action_HierarchicalLink:
		if((GetAsyncKeyState(VK_SHIFT) < 0) && (GetAsyncKeyState(VK_CONTROL)) < 0)
		{
			if(mb_EditHierarchy) return TRUE;
		}
		break;
	}

	return FALSE;
}

/*$4
 ***********************************************************************************************************************
    Do / undo
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Redo undone action
 =======================================================================================================================
 */
void F3D_cl_View::Redo(void)
{
	mo_UndoManager.b_Redo();
	LINK_Refresh();
}

/*
 =======================================================================================================================
    Aim:    Undo action
 =======================================================================================================================
 */
void F3D_cl_View::Undo(void)
{
	mo_UndoManager.b_Undo();
	LINK_Refresh();
}

/*
 =======================================================================================================================
    Aim:    say if world has been changed. look at undo list (swap selection undo)
 =======================================================================================================================
 */
BOOL F3D_cl_View::b_RelevantChange(void)
{
    POSITION            x_Pos;
    EDI_cl_UndoModif    *po_Modif;
    BOOL                b_NotRelevant;
    int                 i;

    i = mo_UndoManager.mi_MarkedIndex;
    while ( i < mo_UndoManager.mi_CurrentIndex)
    {
        x_Pos = mo_UndoManager.mo_ListOfModifs.FindIndex( i );
        if (x_Pos)
        {
            po_Modif = mo_UndoManager.mo_ListOfModifs.GetAt( x_Pos );
            b_NotRelevant = po_Modif->IsKindOf(RUNTIME_CLASS( F3D_cl_Undo_Selection ) );
            if ( !b_NotRelevant ) b_NotRelevant = po_Modif->IsKindOf(RUNTIME_CLASS( F3D_cl_Undo_SelSubObject ) );
            if ( !b_NotRelevant ) return TRUE;
        }
        i++;
    }
    return FALSE;
}

/*$4
 ***********************************************************************************************************************
    dialogs
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Dialogs_BackgroundImage_SwapDisplay(void)
{
	if(mpo_BackgroundImageDialog)
	{
		delete mpo_BackgroundImageDialog;
		mpo_BackgroundImageDialog = NULL;
	}
	else
	{
		mpo_BackgroundImageDialog = new EDIA_cl_BackgroundImage(this);
        mpo_BackgroundImageDialog->DoModeless();
	}
}

#endif /* ACTIVE_EDITORS */
