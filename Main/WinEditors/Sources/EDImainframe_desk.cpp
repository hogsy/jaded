/*$T EDImainframe_desk.cpp GC 1.134 06/10/04 09:36:20 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "EDImainframe.h"
#include "EDImainframe_act.h"
#include "Res/Res.h"
#include "EDIapp.h"
#include "EDIstrings.h"
#include "EDIerrid.h"
#include "DIAlogs/DIAmessage_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDIpaths.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGmsg.h"
#include "ENGine/Sources/ENGloop.h"
#include "LINKs/LINKmsg.h"
#include "EDItors/Sources/MENu/MENframe.h"
#include "EDItors/Sources/MENu/MENmenu.h"
#include "EDIeditors_infos.h"

#include "../Main/Shared/FileSystem/FileSystem.h"

/*
 =======================================================================================================================
    Aim: First init a desktop with default values
 =======================================================================================================================
 */
void EDI_cl_MainFrame::InitDesktop(EDI_tdst_Desktop *_pst_Desk)
{
	/*~~~~~*/
	int i, j;
	/*~~~~~*/

	/* Views */
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		for(j = 0; j < EDI_C_MaxEditors; j++)
		{
			_pst_Desk->ast_View[i].ast_Editors[j].i_Type = -1;
			_pst_Desk->ast_View[i].ast_Editors[j].i_Cpt = 0;
			_pst_Desk->ast_View[i].ast_Editors[j].po_Editor = NULL;
		}

		_pst_Desk->ast_View[i].b_TopFill = FALSE;
		_pst_Desk->ast_View[i].i_NumEditors = 0;
		_pst_Desk->ast_View[i].i_CurrentSelScreen = 0;
	}

	/* Splitters */
	mst_Desktop.i_SplitterCx = 100;
	mst_Desktop.i_SplitterCx1 = 100;
	mst_Desktop.i_SplitterCy1 = 100;
	mst_Desktop.i_SplitterCy12 = 100;
	mst_Desktop.i_SplitterCy2 = 100;
	mst_Desktop.i_SplitterCy22 = 100;
	mst_Desktop.i_FSSplitterCx = 100;
	mst_Desktop.i_FSSplitterCx1 = 100;
	mst_Desktop.i_FSSplitterCy1 = 100;
	mst_Desktop.i_FSSplitterCy12 = 100;
	mst_Desktop.i_FSSplitterCy2 = 100;
	mst_Desktop.i_FSSplitterCy22 = 100;
	mst_Desktop.b_FSLeftFrameOn = FALSE;
	mst_Desktop.b_FSMainBarOn = FALSE;
	mst_Desktop.b_VeryMaximized = FALSE;
	mst_Desktop.b_HrzOrient = FALSE;
	mst_Desktop.b_ShortMode = FALSE;
}

/*
 =======================================================================================================================
    Aim: To load a given desktop in a EDI_tdst_Desktop struct. In: _psz_Name Name of the desktop to load (folder in
    desktop path). _pst_Desk Address where the content of the desktop file will be loaded.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::LoadDesktop(char *_psz_Name, EDI_tdst_Desktop *_pst_Desk)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_File;
	ULONG		ul_Len;
	CString		o_Name;
	char		asz_Name[255];
#ifdef JADEFUSION
	char		*pc_Buf;
	BIG_INDEX	ul_Index;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf(asz_Name, "Loading desktop %s", _psz_Name);
	if(EDI_gpo_EnterWnd)
		EDI_gpo_EnterWnd->DisplayMessage(asz_Name);
	else
	{
		LINK_gb_CanLog = FALSE;
		LINK_PrintStatusMsg(asz_Name);
		LINK_gb_CanLog = TRUE;
	}

	/* Name of desktop (folder) */
	o_Name = EDI_Csz_Ini_Desktop;
	o_Name += "/";
	o_Name += _psz_Name;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Reinit view informations of desktop
	 -------------------------------------------------------------------------------------------------------------------
	 */

	InitDesktop(_pst_Desk);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Load desktop
	 -------------------------------------------------------------------------------------------------------------------
	 */

	o_Name += "/" EDI_Csz_NameDesktop;

	std::vector< uint8_t > buf;
	if ( jaded::filesystem.ReadFileByName( ( LPCSTR ) o_Name, &buf ) )
	{
		memcpy( _pst_Desk, buf.data(), buf.size() );

		InitDesktop( _pst_Desk );
	}

#ifdef JADEFUSION
	/* Load Features Mgr infos */
	ul_Index = BIG_ul_SearchFileExt((char *) (LPCSTR) o_Name, EDI_MTL_FEATURE_INI_FILENAME);
	if(ul_Index != BIG_C_InvalidIndex)
	{
_Try_
		pc_Buf = BIG_pc_ReadFileTmp(BIG_PosFile(ul_Index), NULL);
		for(int i = 0; i < EDI_MTL_MAX_FEATURES; i++)
			ab_MtlFeatures[i] = LOA_ReadInt(&pc_Buf);
_Catch_
_End_
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::DesktopTreat(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDI_cl_BaseView		*po_View;
	EDI_cl_BaseFrame	*po_Frame;
	int					i, iSel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Orientation of splitters */
	if
	(
		(mst_Desktop.b_HrzOrient && (mo_Splitter1.GetRowCount() == 2))
	||	(!mst_Desktop.b_HrzOrient && (mo_Splitter1.GetRowCount() == 1))
	)
	{
		mo_Splitter.ToggleOrientation();
		mo_Splitter1.ToggleOrientation();
		mo_Splitter2.ToggleOrientation();
	}

	/* Update size of first pane */
	if(mo_Splitter1.GetRowCount() == 2)
	{
		mo_Splitter.SetColumnInfo(0, mst_Desktop.i_SplitterCx, 0);
		mo_Splitter.SetColumnInfo(1, mst_Desktop.i_SplitterCx1, 0);
		mo_Splitter1.SetRowInfo(0, mst_Desktop.i_SplitterCy1, 0);
		mo_Splitter1.SetRowInfo(1, mst_Desktop.i_SplitterCy12, 0);
		mo_Splitter2.SetRowInfo(0, mst_Desktop.i_SplitterCy2, 0);
		mo_Splitter2.SetRowInfo(1, mst_Desktop.i_SplitterCy22, 0);
	}
	else
	{
		mo_Splitter.SetRowInfo(0, mst_Desktop.i_SplitterCx, 0);
		mo_Splitter.SetRowInfo(1, mst_Desktop.i_SplitterCx1, 0);
		mo_Splitter1.SetColumnInfo(0, mst_Desktop.i_SplitterCy1, 0);
		mo_Splitter1.SetColumnInfo(1, mst_Desktop.i_SplitterCy12, 0);
		mo_Splitter2.SetColumnInfo(0, mst_Desktop.i_SplitterCy2, 0);
		mo_Splitter2.SetColumnInfo(1, mst_Desktop.i_SplitterCy22, 0);
	}

	/* Select tabs */
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		po_View = po_GetViewWithNum(i);
		iSel = mst_Desktop.ast_View[i].i_CurrentSelScreen;
		po_Frame = mst_Desktop.ast_View[i].ast_Editors[iSel].po_Editor;
		if(po_Frame && po_Frame->mi_NumPane < po_View->GetItemCount())
		{
			/* Save current selection (for changepane to disactivate current editor) */
			mst_Desktop.ast_View[i].i_CurrentSelScreen = po_View->i_NumEditFromPane(po_View->GetCurFocus());
			po_View->SetCurSel(po_Frame->mi_NumPane);
			po_View->OnChangePane(NULL, NULL);
		}

		po_View->mb_TabFill = mst_Desktop.ast_View[i].b_TopFill;
	}

	/* Update all views */
	RecalcLayout();
	mo_Splitter.RecalcLayout();
	mo_Splitter1.RecalcLayout();
	mo_Splitter2.RecalcLayout();
}

/*
 =======================================================================================================================
    Aim: Call to save actual desktop. The desktop name (i.e. the corresponding folder) is retreive from
    "mst_Ini.asz_CurrentDeskName".
 =======================================================================================================================
 */
void EDI_cl_MainFrame::SaveCurrentDesktop(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	EDI_cl_BaseView *po_View;
	CString			o_Name;
	int				i, ix_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(BIG_Handle() == NULL) return;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Normal save of desktop
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/* Name of desktop (folder) */
	o_Name = EDI_Csz_Ini_Desktop;
	o_Name += "/";
	o_Name += mst_Ini.asz_CurrentDeskName;

	/* Save init information of all views */
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		po_View = po_GetViewWithNum(i);
		mst_Desktop.ast_View[i].i_CurrentSelScreen = po_View->i_NumEditFromPane(po_View->GetCurSel());
		mst_Desktop.ast_View[i].b_TopFill = po_View->mb_TabFill;
	}

#ifdef JADEFUSION
	/* Save Features Mgr infos */
	SAV_Begin( (char *) (LPCSTR)o_Name, EDI_MTL_FEATURE_INI_FILENAME );
	SAV_Buffer(&ab_MtlFeatures, sizeof(ab_MtlFeatures) );
	SAV_ul_End();
#endif

	/* Panes : Vertical orientation */
	if(mo_Splitter1.GetRowCount() == 2)
	{
		mo_Splitter.GetColumnInfo(0, mst_Desktop.i_SplitterCx, ix_Temp);
		if(ix_Temp < 0) mst_Desktop.i_SplitterCx = ix_Temp;

		mo_Splitter.GetColumnInfo(1, mst_Desktop.i_SplitterCx1, ix_Temp);
		if(ix_Temp < 0) mst_Desktop.i_SplitterCx1 = ix_Temp;

		mo_Splitter1.GetRowInfo(0, mst_Desktop.i_SplitterCy1, ix_Temp);
		if(ix_Temp < 0) mst_Desktop.i_SplitterCy1 = ix_Temp;

		mo_Splitter1.GetRowInfo(1, mst_Desktop.i_SplitterCy12, ix_Temp);
		if(ix_Temp < 0) mst_Desktop.i_SplitterCy12 = ix_Temp;

		mo_Splitter2.GetRowInfo(0, mst_Desktop.i_SplitterCy2, ix_Temp);
		if(ix_Temp < 0) mst_Desktop.i_SplitterCy2 = ix_Temp;

		mo_Splitter2.GetRowInfo(1, mst_Desktop.i_SplitterCy22, ix_Temp);
		if(ix_Temp < 0) mst_Desktop.i_SplitterCy22 = ix_Temp;
	}

	/* Panes : Horizontal orientation */
	else
	{
		mo_Splitter.GetRowInfo(0, mst_Desktop.i_SplitterCx, ix_Temp);
		if(ix_Temp < 0) mst_Desktop.i_SplitterCx = ix_Temp;

		mo_Splitter.GetRowInfo(1, mst_Desktop.i_SplitterCx1, ix_Temp);
		if(ix_Temp < 0) mst_Desktop.i_SplitterCx1 = ix_Temp;

		mo_Splitter1.GetColumnInfo(0, mst_Desktop.i_SplitterCy1, ix_Temp);
		if(ix_Temp < 0) mst_Desktop.i_SplitterCy1 = ix_Temp;

		mo_Splitter1.GetColumnInfo(1, mst_Desktop.i_SplitterCy12, ix_Temp);
		if(ix_Temp < 0) mst_Desktop.i_SplitterCy12 = ix_Temp;

		mo_Splitter2.GetColumnInfo(0, mst_Desktop.i_SplitterCy2, ix_Temp);
		if(ix_Temp < 0) mst_Desktop.i_SplitterCy2 = ix_Temp;

		mo_Splitter2.GetColumnInfo(1, mst_Desktop.i_SplitterCy22, ix_Temp);
		if(ix_Temp < 0) mst_Desktop.i_SplitterCy22 = ix_Temp;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Load desktop
	 -------------------------------------------------------------------------------------------------------------------
	 */
#if 0
	if(M_MF()->mst_ExternIni.b_SynchroEditorsData)
	{
		FILE	*f;
		char	asz_Name[512];

		strcpy(asz_Name, EDI_go_TheApp.m_pszHelpFilePath);
		*strrchr(asz_Name, '\\') = 0;
		strcat(asz_Name, "/EditorDatas/Ini/Desktop/");
		strcat(asz_Name, mst_Ini.asz_CurrentDeskName);
		strcat(asz_Name, "/");
		CreateDirectory(asz_Name, NULL);
		strcat(asz_Name, EDI_Csz_NameDesktop);
		f = fopen(asz_Name, "wb");
		if(f)
		{
			fwrite(&mst_Desktop, sizeof(EDI_tdst_Desktop), 1, f);
			fclose(f);
		}
	}
#endif

	/* Really save */
	BIG_ul_CreateDir((char *) (LPCSTR) o_Name);

	/* No key creation for desktop files */
	_Try_	
	BIG_UpdateFileFromBuffer
		(
			(char *) (LPCSTR) o_Name,
			EDI_Csz_NameDesktop,
			&mst_Desktop,
			sizeof(EDI_tdst_Desktop)
		);
	_Catch_ 
	_End_
}

/*
 =======================================================================================================================
    Aim: Call when the user clicked on a desktop. This will load the selected desktop and change it. In: _psz_Name Name
    of the desktop to load (this is a directory in desktop path).
 =======================================================================================================================
 */
void EDI_cl_MainFrame::ChangeDesktop(char *_psz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDI_tdst_Desktop	st_Temp, st_Union;
	int					i, j, k, l;
	tdst_PresentEditor	*po_NewEditor, *po_ActEditor;
	EDI_cl_BaseFrame	*po_Frame, *po_Editor;
	BOOL				b_Found;
	EDI_cl_BaseView		*po_View;
	CRect				o_Rect;
	int					i_Pane;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_MF()->LockDisplay(&mo_BigSplitter);
	AfxGetApp()->DoWaitCursor(1);

	/* Force desactive current editor */
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		po_View = po_GetViewWithNum(i);
		i_Pane = po_View->GetCurFocus();
		if(i_Pane != -1)
		{
			i_Pane = po_View->i_NumEditFromPane(i_Pane);
			po_Editor = M_CurEd(i, i_Pane).po_Editor;
			if(po_Editor) po_View->DisactivateEditor(po_Editor);
		}
	}

	/* Change current desktop */
	L_strcpy(mst_Ini.asz_CurrentDeskName, _psz_Name);

	/* Load desktop in a temporary buffer */
	LoadDesktop(_psz_Name, &st_Temp);

	/* Save infos in current desktop */
	L_memcpy(&st_Union, &st_Temp, sizeof(EDI_tdst_Desktop));
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		L_memset(&st_Union.ast_View[i].ast_Editors, 0, sizeof(st_Union.ast_View[i].ast_Editors));
		st_Union.ast_View[i].i_NumEditors = 0;
	}

	/* Treat editors */
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		for(j = 0; j < st_Temp.ast_View[i].i_NumEditors; j++)
		{
			po_NewEditor = &(st_Temp.ast_View[i].ast_Editors[j]);
			po_ActEditor = &(mst_Desktop.ast_View[i].ast_Editors[j]);

			/* Is the the same editor at the same place ? */
			if((po_NewEditor->i_Type == po_ActEditor->i_Type) && (po_NewEditor->i_Cpt == po_ActEditor->i_Cpt))
			{
				L_memcpy(&st_Union.ast_View[i].ast_Editors[j], po_ActEditor, sizeof(tdst_PresentEditor));
				po_ActEditor->po_Editor = NULL;
				st_Union.ast_View[i].i_NumEditors++;
				continue;
			}

			/* Find an empty editor in loaded desktop. Save it without reset original place */
			if((po_NewEditor->i_Type == -1) && (po_NewEditor->po_Editor == NULL))
			{
				L_memcpy(&st_Union.ast_View[i].ast_Editors[j], po_NewEditor, sizeof(tdst_PresentEditor));
				st_Union.ast_View[i].i_NumEditors++;
				continue;
			}

			/* It's not the same editor. Search in actual list the corresponding editor */
			b_Found = FALSE;
			for(k = 0; k < EDI_C_MaxViews; k++)
			{
				for(l = 0; l < mst_Desktop.ast_View[k].i_NumEditors; l++)
				{
					if
					(
						(po_NewEditor->i_Type == mst_Desktop.ast_View[k].ast_Editors[l].i_Type)
					&&	(po_NewEditor->i_Cpt == mst_Desktop.ast_View[k].ast_Editors[l].i_Cpt)
					)
					{
						b_Found = TRUE;
						goto zap;
					}
				}
			}

			/* We have found the editor in current desktop. Move it */
zap:
			if(b_Found)
			{
				L_memcpy
				(
					&st_Union.ast_View[i].ast_Editors[j],
					&mst_Desktop.ast_View[k].ast_Editors[l],
					sizeof(tdst_PresentEditor)
				);

				/* The original editor in original view must be deleted */
				mst_Desktop.ast_View[k].ast_Editors[l].po_Editor = NULL;

				/* Change fields cause of move */
				po_Frame = st_Union.ast_View[i].ast_Editors[j].po_Editor;
				if(po_Frame)
				{
					po_Frame->mi_NumPane = i;
					po_Frame->mst_BaseIni.b_IsVisible = TRUE;
					st_Union.ast_View[i].i_NumEditors++;
					po_View = po_GetViewWithNum(i);
					po_Frame->mpo_MyView = po_View;
					po_Frame->SetParent(po_View);
					if(po_Frame->mpo_Menu) po_Frame->mpo_Menu->SetParent(po_View);
				}

				continue;
			}

			/* We have not found it. Mark to create later. */
			L_memcpy
			(
				&st_Union.ast_View[i].ast_Editors[j],
				&st_Temp.ast_View[i].ast_Editors[j],
				sizeof(tdst_PresentEditor)
			);
			st_Union.ast_View[i].ast_Editors[j].po_Editor = NULL;
			st_Union.ast_View[i].i_NumEditors++;
		}
	}

	/*
	 * Copy in union all the editors that are currently present, and that are not in
	 * loaded desktop.
	 */
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		for(j = 0; j < mst_Desktop.ast_View[i].i_NumEditors; j++)
		{
			po_NewEditor = &(st_Union.ast_View[i].ast_Editors[j]);
			po_ActEditor = &(mst_Desktop.ast_View[i].ast_Editors[j]);
			if(po_ActEditor->po_Editor)
			{
				/* Search the first free place (if one) */
				for(k = 0; k < st_Union.ast_View[i].i_NumEditors; k++)
				{
					if(st_Union.ast_View[i].ast_Editors[i].i_Type == -1) break;
				}

				if(k == st_Union.ast_View[i].i_NumEditors) st_Union.ast_View[i].i_NumEditors++;
				L_memcpy(&st_Union.ast_View[i].ast_Editors[k], po_ActEditor, sizeof(tdst_PresentEditor));

				/* Force the control to be hidden */
				st_Union.ast_View[i].ast_Editors[k].po_Editor->mst_BaseIni.b_IsVisible = FALSE;
			}
		}
	}

	/* Copy union to current desktop */
	L_memcpy(&mst_Desktop, &st_Union, sizeof(EDI_tdst_Desktop));

	/*
	 * Create all editors that were in loaded desktop, but not in current one. This
	 * kind of editor has a type != -1, but has a po_Editor to NULL.
	 */
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		po_View = po_GetViewWithNum(i);
		for(j = 0; j < mst_Desktop.ast_View[i].i_NumEditors; j++)
		{
			po_NewEditor = &mst_Desktop.ast_View[i].ast_Editors[j];
			if(!po_NewEditor->po_Editor)
			{
				if(po_NewEditor->i_Type != -1)
				{
					/* Add new one */
					po_Frame = po_View->po_AddOneEditor(po_NewEditor->i_Type, po_NewEditor->i_Cpt, j);
					if(po_Frame) po_Frame->OpenProject();
				}
			}
		}
	}

	/* Load ini for desktop */
	for(i = 0; i < EDI_C_MaxViews; i++) po_GetViewWithNum(i)->LoadIniDesktop();

	/* Update all views. We force each editor to be "hide". */
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		for(j = 0; j < mst_Desktop.ast_View[i].i_NumEditors; j++)
		{
			po_Frame = mst_Desktop.ast_View[i].ast_Editors[j].po_Editor;
			if(po_Frame)
			{
				po_Frame->mb_IsPresent = FALSE; /* To force create of tabctrl pane */
				po_Frame->mi_NumPane = -1;		/* View update function will set this */
				po_Frame->ShowWindow(SW_HIDE);	/* Force frame editor to be hide */
			}
		}

		po_View = po_GetViewWithNum(i);
		po_View->DeleteAllItems();				/* Delete all current tabctrl panes */
		po_View->ActivateDisactivateEditors();	/* Create all good panes */

		/* Restore current sel screen cause function above change it */
		mst_Desktop.ast_View[i].i_CurrentSelScreen = st_Temp.ast_View[i].i_CurrentSelScreen;
	}

	/* Treat ini for each editor */
	for(i = 0; i < EDI_C_MaxViews; i++) po_GetViewWithNum(i)->TreatIni();

	/* Treat infos of desktop (position of splitters and toolbar) */
	DesktopTreat();

	/* Force resize view */
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		ENG_gb_GlobalLock = TRUE;
		po_GetViewWithNum(i)->GetWindowRect(&o_Rect);
		po_GetViewWithNum(i)->GetParent()->ScreenToClient(&o_Rect);
		o_Rect.right--;
		po_GetViewWithNum(i)->MoveWindow(&o_Rect);
		o_Rect.right++;
		po_GetViewWithNum(i)->MoveWindow(&o_Rect);
		ENG_gb_GlobalLock = FALSE;
	}

	M_MF()->UnlockDisplay(&mo_BigSplitter);		/* Refresh all */
	AfxGetApp()->DoWaitCursor(-1);
}

/*
 =======================================================================================================================
    Aim: Call when the user choose the action to create a new folder. Request (with a dialog) a name, and save the new
    desktop.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::NewDesktop(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog(EDI_STR_Csz_TitleNewDesktop, BIG_C_MaxLenName - 1);
	CString				o_Name;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_Dialog.DoModal() == IDOK)
	{
		/* Check if the name is a valid bigfile one (cause it's a folder) */
		if(BIG_b_CheckName((char *) (LPCSTR) o_Dialog.mo_Name))
		{
			/* Is the dir already here ? If yes, error */
			o_Name = EDI_Csz_Ini_Desktop;
			o_Name += "/";
			o_Name += o_Dialog.mo_Name;
			if(BIG_ul_SearchDir((char *) (LPCSTR) o_Name) != BIG_C_InvalidIndex)
			{
				ERR_X_ForceError(EDI_ERR_Csz_DeskAlreadyHere, (char *) (LPCSTR) o_Dialog.mo_Name);
				return;
			}

			/* Else save the current desktop with the new name */
			L_strcpy(mst_Ini.asz_CurrentDeskName, (char *) (LPCSTR) o_Dialog.mo_Name);
			SaveCurrentDesktop();

			/* Refresh menu */
			mpo_MenuFrame->FillList();
		}
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
void EDI_cl_MainFrame::OpenWorkspace(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	int				i, j, k;
	EMEN_cl_Frame	*po_Menu;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	/* Load all the .ini */
	LoadIni();

	/* Load current desktop */
	LoadDesktop(mst_Ini.asz_CurrentDeskName, &mst_Desktop);

	/* Check if there are some instances of editors */
	if
	(
		(M_CurNumEd(mpo_MenuView->mi_NumPane) == 0)
	&&	(M_CurNumEd(mpo_LeftView->mi_NumPane) == 0)
	&&	(M_CurNumEd(mpo_RightView->mi_NumPane) == 0)
	&&	(M_CurNumEd(mpo_CornerView->mi_NumPane) == 0)
	&&	(M_CurNumEd(mpo_BottomView->mi_NumPane) == 0)
	)
	{
		for(i = 0; i < mi_TotalEditors; i++)
			M_CurEd(mpo_LeftView->mi_NumPane, i).i_Type = mast_ListOfEditors[i].i_Type;

		M_CurNumEd(mpo_LeftView->mi_NumPane) = mi_TotalEditors;
	}

	/* Is there a new editor ? */
	for(i = 0; i < M_MF()->mi_TotalEditors; i++)
	{
		for(j = 0; j < EDI_C_MaxViews; j++)
		{
			for(k = 0; k < M_CurNumEd(j); k++)
			{
				if(M_MF()->mast_ListOfEditors[i].i_Type == M_CurEd(j, k).i_Type) goto l_Ok;
			}
		}

		/* Find a free space in the current view table */		
		for(k = 0; k < EDI_C_MaxEditors; k++)
		{
			if(M_CurEd(mpo_LeftView->mi_NumPane, k).i_Type == -1)
			{
				M_CurEd(mpo_LeftView->mi_NumPane, k).i_Type = mast_ListOfEditors[i].i_Type;					
				goto l_Ok;
			}
		}		

		M_CurEd(mpo_LeftView->mi_NumPane, M_CurNumEd(mpo_LeftView->mi_NumPane)).i_Type = mast_ListOfEditors[i].i_Type;
		M_CurNumEd(mpo_LeftView->mi_NumPane)++;

l_Ok: ;
	}

	/* Create editors */
	for(i = 0; i < EDI_C_MaxViews; i++) po_GetViewWithNum(i)->CreateMainEditors();
	for(i = 0; i < EDI_C_MaxViews; i++) po_GetViewWithNum(i)->CreateDuplicateEditors();

	/* Inform views (to create their editor) */
	for(i = 0; i < EDI_C_MaxViews; i++) po_GetViewWithNum(i)->OpenProject();

	/* Create global menu */
	mpo_MenuFrame = NULL;
	po_Menu = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_MENU, mst_Ini.i_CountMenu);
	if(!po_Menu) po_Menu = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_MENU, 0);
	if(po_Menu) mpo_MenuFrame = po_Menu->po_AddMenu(EDI_STR_Csz_MenuGlobal, mpo_Actions, NULL, 0);

	/* Create top menu if b_TopMenu is TRUE */
	if(mst_Ini.b_TopMenu && mpo_MenuFrame) mpo_MenuFrame->CreateTopMenu(NULL);

	/* Action set */
	ForceActionConfig(mst_Ini.i_ActionSet);

	/* Treat current desktop */
	DesktopTreat();

	/* Show main window splitter */
	if(BIG_Handle()) mo_BigSplitter.ShowWindow(SW_SHOW);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_MainFrame::b_CloseWorkspace(BOOL _b_Update)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	int				i;
	EMEN_cl_Frame	*po_Menu;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	/* Accept editors to close the current project. */
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		if(po_GetViewWithNum(i)->b_AcceptToCloseProject() == FALSE) return FALSE;
	}

	/* Save workspace ? */
	if(BIG_Handle() && mst_Ini.mst_Options.b_SaveWhenExit) SaveWorkspace();

	/* Remove action list from left frame */
	if(mpo_MenuFrame) mpo_MenuFrame->DestroyWindow();
	mpo_MenuFrame = NULL;

	/* Remove current menu of all editors MEN */
	for(i = 0; i < EDI_C_MaxDuplicate + 1; i++)
	{
		po_Menu = (EMEN_cl_Frame *) po_GetEditorByType(EDI_IDEDIT_MENU, i);
		if(!po_Menu) po_Menu = (EMEN_cl_Frame *) po_GetEditorByType(EDI_IDEDIT_MENU, 0);
		if(po_Menu) po_Menu->mpo_CurrentExpand = NULL;
	}

	if(_b_Update)
	{
		/* Hide main splitter */
		mo_BigSplitter.ShowWindow(SW_HIDE);
	}

	/* If there's a top menu for mainframe, delete it */
	if(mpo_MenuBar)
	{
		mpo_Menu->mpo_MenuFrame = NULL;
		mpo_Menu->DestroyWindow();
		mpo_Menu = NULL;
		mpo_MenuBar->DestroyWindow();
		delete mpo_MenuBar;
		mpo_MenuBar = NULL;
	}

	/* Is there a project ? */
	if(mst_ExternIni.asz_DataBigFileName[0] != '\0')
	{
		/* Inform editors */
		for(i = 0; i < EDI_C_MaxViews; i++) po_GetViewWithNum(i)->CloseProject();
	}

	return TRUE;
}

/*
 =======================================================================================================================
    Aim: Call to save all the ini files (save workspace).
 =======================================================================================================================
 */
void EDI_cl_MainFrame::SaveWorkspace(void)
{
	if(mst_Desktop.b_VeryMaximized) return;

	AfxGetApp()->DoWaitCursor(1);

	_Try_

	/* Ini files */
	if(BIG_gst.h_CLibFileHandle != NULL)
	{
		SaveIni();
		for(int i = 0; i < EDI_C_MaxViews; i++) po_GetViewWithNum(i)->SaveIni();
	}

	/* Current desktop */
	SaveCurrentDesktop();

	/* Print message */
	LINK_gb_CanLog = FALSE;
	SynchronizeEditorsData(FALSE);
	LINK_gb_CanLog = TRUE;
	LINK_PrintStatusMsg(EDI_STR_Csz_SaveWorkspace);

	_Catch_ _End_ AfxGetApp()->DoWaitCursor(-1);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim: When a pane is maximised, all no maximised panes have no menu frame on the left (the menu frame is destroyed
    when maximising). This function restore all menu frames when the pane is restore (when equalised panes, or when
    toggle maximised mode).
 =======================================================================================================================
 */
void EDI_cl_MainFrame::RestoreMenuFrames(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	EDI_cl_BaseView		*po_View;
	EDI_cl_BaseFrame	*po_Editor;
	EMEN_cl_Frame		*po_Menu;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpo_MaxView == NULL) return;

	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		po_View = po_GetViewWithNum(i);
		if(po_View != mpo_MaxView)
		{
			/* Get top level editor in view. If not, pass */
			po_Editor = po_View->po_GetActivatedEditor();
			if(po_Editor)
			{
				po_Editor->mpo_MenuFrame = NULL;
				if(po_Editor->b_HasMenu())
				{
					po_Menu = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType
						(
							EDI_IDEDIT_MENU,
							po_Editor->mst_BaseIni.i_CountMenu
						);
					if(!po_Menu)
					{
						po_Menu = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_MENU, 0);
					}

					if(po_Menu)
					{
						po_Editor->mpo_MenuFrame = po_Menu->po_AddMenu
							(
								po_Editor->mst_Def.asz_Name,
								po_Editor->mpo_Actions,
								po_Editor,
								i + 1,
								FALSE
							);

						/* Create top menu if b_TopMenu is TRUE */
						if(po_Editor->mpo_Menu) po_Editor->mpo_Menu->mpo_MenuFrame = po_Editor->mpo_MenuFrame;
					}
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OnDesktopEqualize(void)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	int		cx, cy;
	/*~~~~~~~~~~~*/

	LockDisplay(&mo_BigSplitter);

	RestoreMenuFrames();
	mpo_MaxView = NULL;
	mo_Splitter.GetClientRect(&o_Rect);
	cx = o_Rect.right / 2;
	cy = o_Rect.bottom / 2;

	/* Vertical orientation */
	if(mo_Splitter1.GetRowCount() == 2)
	{
		mo_Splitter.SetColumnInfo(0, cx, 0);
		mo_Splitter.SetColumnInfo(1, 0, 0);
		mo_Splitter1.SetRowInfo(0, cy, 0);
		mo_Splitter1.SetRowInfo(1, 0, 0);
		mo_Splitter2.SetRowInfo(0, cy, 0);
		mo_Splitter2.SetRowInfo(1, 0, 0);
	}

	/* Horizontal orientation */
	else
	{
		mo_Splitter.SetRowInfo(0, cy, 0);
		mo_Splitter.SetRowInfo(1, 0, 0);
		mo_Splitter1.SetColumnInfo(0, cx, 0);
		mo_Splitter1.SetColumnInfo(1, 0, 0);
		mo_Splitter2.SetColumnInfo(0, cx, 0);
		mo_Splitter2.SetColumnInfo(1, 0, 0);
	}

	/* Redraw all panes */
	mo_Splitter.RecalcLayout();
	mo_Splitter1.RecalcLayout();
	mo_Splitter2.RecalcLayout();
	UnlockDisplay(&mo_BigSplitter);
}

/*
 =======================================================================================================================
    Aim: Expand panes to fill empty ones.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OnDesktopFill(void)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	int		cx, cy;
	/*~~~~~~~~~~~*/

	/* No process if a pane is currently maximised */
	if(mpo_MaxView) return;

	mo_Splitter.GetClientRect(&o_Rect);
	cx = o_Rect.right;
	cy = o_Rect.bottom;

	LockDisplay(&mo_BigSplitter);

	/* Vertical orientation */
	if(mo_Splitter1.GetRowCount() == 2)
	{
		if(mpo_LeftView->GetItemCount() == 0)
		{
			if(mpo_CornerView->GetItemCount() == 0)
				mo_Splitter.SetColumnInfo(0, -1, -1);
			else
				mo_Splitter1.SetRowInfo(0, -1, -1);
		}
		else if(mpo_CornerView->GetItemCount() == 0)
		{
			if(mpo_LeftView->GetItemCount() == 0)
				mo_Splitter.SetColumnInfo(0, -1, -1);
			else
			{
				mo_Splitter1.SetRowInfo(0, cy, 0);
				mo_Splitter1.SetRowInfo(1, -1, -1);
			}
		}

		if(mpo_RightView->GetItemCount() == 0)
		{
			if(mpo_BottomView->GetItemCount() == 0)
			{
				mo_Splitter.SetColumnInfo(0, cx, 0);
				mo_Splitter.SetColumnInfo(1, -1, -1);
			}
			else
				mo_Splitter2.SetRowInfo(0, -1, -1);
		}
		else if(mpo_BottomView->GetItemCount() == 0)
		{
			if(mpo_RightView->GetItemCount() == 0)
			{
				mo_Splitter.SetColumnInfo(0, cx, 0);
				mo_Splitter.SetColumnInfo(1, -1, -1);
			}
			else
			{
				mo_Splitter2.SetRowInfo(0, cy, 0);
				mo_Splitter2.SetRowInfo(1, -1, -1);
			}
		}
	}

	/* Horizontal orientation */
	else
	{
		if(mpo_LeftView->GetItemCount() == 0)
		{
			if(mpo_CornerView->GetItemCount() == 0)
				mo_Splitter.SetRowInfo(0, -1, -1);
			else
				mo_Splitter1.SetColumnInfo(0, -1, -1);
		}
		else if(mpo_CornerView->GetItemCount() == 0)
		{
			if(mpo_LeftView->GetItemCount() == 0)
				mo_Splitter.SetRowInfo(0, -1, -1);
			else
			{
				mo_Splitter1.SetColumnInfo(0, cx, 0);
				mo_Splitter1.SetColumnInfo(1, -1, -1);
			}
		}

		if(mpo_RightView->GetItemCount() == 0)
		{
			if(mpo_BottomView->GetItemCount() == 0)
			{
				mo_Splitter.SetRowInfo(0, cy, 0);
				mo_Splitter.SetRowInfo(1, -1, -1);
			}
			else
				mo_Splitter2.SetColumnInfo(0, -1, -1);
		}
		else if(mpo_BottomView->GetItemCount() == 0)
		{
			if(mpo_RightView->GetItemCount() == 0)
			{
				mo_Splitter.SetRowInfo(0, cy, 0);
				mo_Splitter.SetRowInfo(1, -1, -1);
			}
			else
			{
				mo_Splitter2.SetColumnInfo(0, cx, 0);
				mo_Splitter2.SetColumnInfo(1, -1, -1);
			}
		}
	}

	/* Redraw all panes */
	mo_Splitter.RecalcLayout();
	mo_Splitter1.RecalcLayout();
	mo_Splitter2.RecalcLayout();
	UnlockDisplay(&mo_BigSplitter);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::ForceMaximised(EDI_cl_BaseView *_po_View)
{
	/*~~~~~~~~~~~~~~~~*/
	CRect	o_Rect;
	int		cx, cy, min;
	/*~~~~~~~~~~~~~~~~*/

	/*$F	int					i;
    EDI_cl_BaseFrame    *po_Editor;
    EDI_cl_BaseView     *po_View;
    EMEN_cl_Frame       *po_Menu;*/
	mo_Splitter.GetClientRect(&o_Rect);
	cx = o_Rect.right;
	cy = o_Rect.bottom;

	/* Remember maximised view */
	mpo_MaxView = _po_View;

	/* Delete left menu that do not correspond to maximised view */

	/*$F    for(i = 0; i < EDI_C_MaxViews; i++)
    {
        po_View = po_GetViewWithNum(i);
        if((_po_View != po_View) && (po_View != M_MF()->mpo_MenuView) && !po_View->mb_Floating)
        {
            po_Editor = po_View->po_GetActivatedEditor();
            if(po_Editor)
            {
                if(po_Editor->mpo_MenuFrame)
                {
                    po_Menu = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType
                        (
                            EDI_IDEDIT_MENU,
                            po_Editor->mst_BaseIni.i_CountMenu
                        );
					if(!po_Menu)
					{
						po_Menu = (EMEN_cl_Frame *) M_MF()->po_GetEditorByType
							(
								EDI_IDEDIT_MENU,
								0
							);
					}
                    po_Menu->RemoveMenu(po_Editor->mpo_MenuFrame);
                }
            }
        }
    }*/

	/* Save "restore" size */
	if(mo_Splitter1.GetRowCount() == 2)
	{
		mo_Splitter.GetColumnInfo(0, mst_Desktop.i_FSSplitterCx, min);
		if(min < 0) mst_Desktop.i_FSSplitterCx = min;
		mo_Splitter.GetColumnInfo(1, mst_Desktop.i_FSSplitterCx1, min);
		if(min < 0) mst_Desktop.i_FSSplitterCx1 = min;
		mo_Splitter1.GetRowInfo(0, mst_Desktop.i_FSSplitterCy1, min);
		if(min < 0) mst_Desktop.i_FSSplitterCy1 = min;
		mo_Splitter1.GetRowInfo(1, mst_Desktop.i_FSSplitterCy12, min);
		if(min < 0) mst_Desktop.i_FSSplitterCy12 = min;
		mo_Splitter2.GetRowInfo(0, mst_Desktop.i_FSSplitterCy2, min);
		if(min < 0) mst_Desktop.i_FSSplitterCy2 = min;
		mo_Splitter2.GetRowInfo(1, mst_Desktop.i_FSSplitterCy22, min);
		if(min < 0) mst_Desktop.i_FSSplitterCy22 = min;
	}
	else
	{
		mo_Splitter.GetRowInfo(0, mst_Desktop.i_FSSplitterCx, min);
		if(min < 0) mst_Desktop.i_FSSplitterCx = min;
		mo_Splitter.GetRowInfo(1, mst_Desktop.i_FSSplitterCx1, min);
		if(min < 0) mst_Desktop.i_FSSplitterCx1 = min;
		mo_Splitter1.GetColumnInfo(0, mst_Desktop.i_FSSplitterCy1, min);
		if(min < 0) mst_Desktop.i_FSSplitterCy1 = min;
		mo_Splitter1.GetColumnInfo(1, mst_Desktop.i_FSSplitterCy12, min);
		if(min < 0) mst_Desktop.i_FSSplitterCy12 = min;
		mo_Splitter2.GetColumnInfo(0, mst_Desktop.i_FSSplitterCy2, min);
		if(min < 0) mst_Desktop.i_FSSplitterCy2 = min;
		mo_Splitter2.GetColumnInfo(1, mst_Desktop.i_FSSplitterCy22, min);
		if(min < 0) mst_Desktop.i_FSSplitterCy22 = min;
	}

	/* Upper left */
	if(_po_View == mo_Splitter1.GetPane(0, 0))
	{
		if(mo_Splitter1.GetRowCount() == 2)
		{
			mo_Splitter.SetColumnInfo(0, cx, 0);
			mo_Splitter.SetColumnInfo(1, -1, -1);
			mo_Splitter1.SetRowInfo(0, cy, 0);
			mo_Splitter1.SetRowInfo(1, -1, -1);
		}
		else
		{
			mo_Splitter.SetRowInfo(0, cx, 0);
			mo_Splitter.SetRowInfo(1, -1, -1);
			mo_Splitter1.SetColumnInfo(0, cy, 0);
			mo_Splitter1.SetColumnInfo(1, -1, -1);
		}
	}

	/* Upper right */
	else if
		(
			((mo_Splitter1.GetRowCount() == 2) && (_po_View == mo_Splitter1.GetPane(1, 0)))
		||	((mo_Splitter1.GetRowCount() == 1) && (_po_View == mo_Splitter1.GetPane(0, 1)))
		)
	{
		if(mo_Splitter1.GetRowCount() == 2)
		{
			mo_Splitter.SetColumnInfo(0, cx, 0);
			mo_Splitter.SetColumnInfo(1, -1, -1);
			mo_Splitter1.SetRowInfo(0, -1, -1);
		}
		else
		{
			mo_Splitter.SetRowInfo(0, cx, 0);
			mo_Splitter.SetRowInfo(1, -1, -1);
			mo_Splitter1.SetColumnInfo(0, -1, -1);
		}
	}

	/* Bottom left */
	else if(_po_View == mo_Splitter2.GetPane(0, 0))
	{
		if(mo_Splitter1.GetRowCount() == 2)
		{
			mo_Splitter.SetColumnInfo(0, -1, -1);
			mo_Splitter2.SetRowInfo(0, cy, 0);
			mo_Splitter2.SetRowInfo(1, -1, -1);
		}
		else
		{
			mo_Splitter.SetRowInfo(0, -1, -1);
			mo_Splitter2.SetColumnInfo(0, cy, 0);
			mo_Splitter2.SetColumnInfo(1, -1, -1);
		}
	}

	/* Bottom right */
	else if
		(
			((mo_Splitter2.GetRowCount() == 2) && (_po_View == mo_Splitter2.GetPane(1, 0)))
		||	((mo_Splitter2.GetRowCount() == 1) && (_po_View == mo_Splitter2.GetPane(0, 1)))
		)
	{
		if(mo_Splitter1.GetRowCount() == 2)
		{
			mo_Splitter.SetColumnInfo(0, -1, -1);
			mo_Splitter2.SetRowInfo(0, -1, -1);
		}
		else
		{
			mo_Splitter.SetRowInfo(0, -1, -1);
			mo_Splitter2.SetColumnInfo(0, -1, -1);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::SwapMaximised(EDI_cl_BaseView *_po_View)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					cx1, cx2, cy11, cy12, cy21, cy22, min;
	CRect				o_Rect;
	MINMAXINFO			st;
	CWnd				*po_Focus;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Focus = GetFocus();

	/* Force full maximised if shift key pressed */
	if(GetAsyncKeyState(VK_SHIFT) < 0) mst_Desktop.b_VeryMaximized = TRUE;

	ENG_gb_GlobalLock = TRUE;

	/* Vertical horientation */
	if(mo_Splitter1.GetRowCount() == 2)
	{
		mo_Splitter.GetColumnInfo(0, min, cx1);
		mo_Splitter.GetColumnInfo(1, min, cx2);
		mo_Splitter1.GetRowInfo(0, min, cy11);
		mo_Splitter1.GetRowInfo(1, min, cy12);
		mo_Splitter2.GetRowInfo(0, min, cy21);
		mo_Splitter2.GetRowInfo(1, min, cy22);
	}

	/* Horizontal orientation */
	else
	{
		mo_Splitter.GetRowInfo(0, min, cx1);
		mo_Splitter.GetRowInfo(1, min, cx2);
		mo_Splitter1.GetColumnInfo(0, min, cy11);
		mo_Splitter1.GetColumnInfo(1, min, cy12);
		mo_Splitter2.GetColumnInfo(0, min, cy21);
		mo_Splitter2.GetColumnInfo(1, min, cy22);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Force maximised
	 -------------------------------------------------------------------------------------------------------------------
	 */

	static LONG oldStyle = 0;

	if(!mpo_MaxView)
	{
		/* Save left frame visible state */
		mst_Desktop.b_FSLeftFrameOn = mst_Ini.b_LeftFrameVisible;
		mst_Desktop.b_FSIsZoomed = 2;

		if(mst_Desktop.b_VeryMaximized)
		{
			/* If full maximised, hide left frame if necessary */
			if(mst_Desktop.b_VeryMaximized && mst_Ini.b_LeftFrameVisible) OnAction(EDI_ACTION_TOGGLEMENU);
		}

		ENG_gb_GlobalLock = FALSE;
		ForceMaximised(_po_View);

		if(mst_Desktop.b_VeryMaximized)
		{
			//mpo_MenuBar->HideCaret();

			st.ptMaxTrackSize.x = st.ptMaxSize.x = GetSystemMetrics( SM_CXSCREEN );
			st.ptMaxTrackSize.y = st.ptMaxSize.y = GetSystemMetrics( SM_CYSCREEN );

			LONG lStyle = GetWindowLong( m_hWnd, GWL_STYLE );
			oldStyle = lStyle;
			lStyle &= ~( WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU );
			SetWindowLong( m_hWnd, GWL_STYLE, lStyle );
			::SetWindowPos( m_hWnd, HWND_TOPMOST, 0, 0, st.ptMaxTrackSize.x, st.ptMaxTrackSize.y, SWP_FRAMECHANGED );

			BringWindowToTop();
		}
	}
	else
	{
		if(mst_Desktop.b_VeryMaximized)
		{
			mst_Desktop.b_VeryMaximized = FALSE;
			
			ShowWindow(SW_RESTORE);

			SetWindowLong( m_hWnd, GWL_STYLE, oldStyle );
			::SetWindowPos( m_hWnd, NULL, 
				mst_Desktop.o_FSPos.top, 
				mst_Desktop.o_FSPos.left,
				mst_Desktop.o_FSPos.right, 
				mst_Desktop.o_FSPos.bottom, 
				SWP_FRAMECHANGED );
		}

		RestoreMenuFrames();

		/* Restore left menu state */
		if(mst_Desktop.b_FSLeftFrameOn != mst_Ini.b_LeftFrameVisible) OnAction(EDI_ACTION_TOGGLEMENU);

		/* Restore size of each panes */
		if(mo_Splitter1.GetRowCount() == 2)
		{
			mo_Splitter.SetColumnInfo(0, mst_Desktop.i_FSSplitterCx, 0);
			mo_Splitter.SetColumnInfo(1, mst_Desktop.i_FSSplitterCx1, 0);
			mo_Splitter1.SetRowInfo(0, mst_Desktop.i_FSSplitterCy1, 0);
			mo_Splitter1.SetRowInfo(1, mst_Desktop.i_FSSplitterCy12, 0);
			mo_Splitter2.SetRowInfo(0, mst_Desktop.i_FSSplitterCy2, 0);
			mo_Splitter2.SetRowInfo(1, mst_Desktop.i_FSSplitterCy22, 0);
		}
		else
		{
			mo_Splitter.SetRowInfo(0, mst_Desktop.i_FSSplitterCx, 0);
			mo_Splitter.SetRowInfo(1, mst_Desktop.i_FSSplitterCx1, 0);
			mo_Splitter1.SetColumnInfo(0, mst_Desktop.i_FSSplitterCy1, 0);
			mo_Splitter1.SetColumnInfo(1, mst_Desktop.i_FSSplitterCy12, 0);
			mo_Splitter2.SetColumnInfo(0, mst_Desktop.i_FSSplitterCy2, 0);
			mo_Splitter2.SetColumnInfo(1, mst_Desktop.i_FSSplitterCy22, 0);
		}

		mpo_MaxView = NULL;
	}

	/* Redraw all panes */
	ENG_gb_GlobalLock = FALSE;
	mo_Splitter.RecalcLayout();
	mo_Splitter1.RecalcLayout();
	mo_Splitter2.RecalcLayout();

	if(po_Focus) po_Focus->SetFocus();
}
#endif /* ACTIVE_EDITORS */
