/*$T EDImainframe_ini.cpp GC! 1.100 06/15/01 14:53:34 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGexport.h"
#include "BIGfiles/minilzo.h"
#include "BASe/ERRors/ERRasser.h"
#include "EDIerrid.h"
#include "EDIpaths.h"
#include "EDImainframe.h"
#include "EDImainframe_act.h"
#include "BASe/CLIbrary/CLImem.h"
#include "LINKs/LINKstruct.h"
#include "BASe/BAStypes.h"
#include "BIGfiles/BIGdefs.h"
#include "EDIsplitter.h"
#include "EDIaction.h"
#include "EDItors/Sources/MENu/MENframe.h"
#include "EDItors/Sources/MENu/MENmenu.h"
#include "EDItors/Sources/MENu/MENinmenu.h"
#include "ENGine/Sources/ENGvars.h"
#include "INOut/INOjoystick.h"
#include "EDIapp.h"

#ifdef JADEFUSION
extern BOOL BIG_gb_IgnoreDateExt;
extern BOOL EDI_gb_NoVerbose;
#else
extern "C" BOOL BIG_gb_IgnoreDateExt;
extern "C" BOOL EDI_gb_NoVerbose;
#endif

/*$4
 ***********************************************************************************************************************
    Common functions used by all editors and views.
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Base function to load an ini file.

    In:     _psz_IniFile        Name of ini file to load.
            _puc_IniStruct      Structure to store the content of the file.
            _i_SizeOfIniStruct  Size of structure to be retreive from file.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::BaseLoadIni(char *_psz_IniFile, UCHAR *_puc_IniStruct, int _i_SizeOfIniStruct)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		ul_Size;
	BIG_INDEX	ul_Index;
	CString		o_Name;
	char		asz_Name[BIG_C_MaxLenPath];
	char		*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Read from disk */
#if 0
	if(M_MF()->mst_ExternIni.b_SynchroEditorsData)
	{
		FILE	*f;
		int		len;

		strcpy(asz_Name, EDI_go_TheApp.m_pszHelpFilePath);
		*strrchr(asz_Name, '\\') = 0;
		strcat(asz_Name, "/EditorDatas/Ini/");
		strcat(asz_Name, _psz_IniFile);
		strcat(asz_Name, EDI_Csz_ExtIni);
 		f = fopen(asz_Name, "rb");
		if(f)
		{
			fseek(f, 0, SEEK_END);
			len = ftell(f);
			rewind(f);
			fread(_puc_IniStruct, len, 1, f);
			fclose(f);
			return;
		}
	}
#endif

	/* Read the .ini if it exists. */
	o_Name = _psz_IniFile;
	o_Name += EDI_Csz_ExtIni;
	ul_Index = BIG_ul_SearchFileExt(EDI_Csz_Ini, (char *) (LPCSTR) o_Name);
	if(ul_Index != BIG_C_InvalidIndex)
	{
		ul_Size = BIG_ul_ReadFile(BIG_PosFile(ul_Index), _puc_IniStruct);
	}

	/*
	 * Else try to load the default file. We compute the default ini file with the
	 * file name, but without spaces (cause for an editor copy, the name has a space,
	 * and (n)
	 */
	else
	{
		/* Compute file name */
		L_strcpy(asz_Name, _psz_IniFile);
		psz_Temp = asz_Name;
		while(!L_isspace(*psz_Temp) && *psz_Temp) psz_Temp++;
		*psz_Temp = 0;
		o_Name = asz_Name;
		o_Name += EDI_Csz_ExtIniDef;
		o_Name += EDI_Csz_ExtIni;

		/* Try to load */
		ul_Index = BIG_ul_SearchFileExt(EDI_Csz_Ini, (char *) (LPCSTR) o_Name);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			ul_Size = BIG_ul_ReadFile(BIG_PosFile(ul_Index), _puc_IniStruct);
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Base function to save an ini file.

    In:     _psz_IniFile        Name of file to save.
            _puc_IniStruct      Structure to save.
            _i_SizeOfIniStruct  Size of structure (in bytes) to save.
 =======================================================================================================================
 */
void EDI_cl_MainFrame::BaseSaveIni(char *_psz_IniFile, UCHAR *_puc_IniStruct, int _i_SizeOfIniStruct)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString o_Name;
	char	asz_PathIni[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Write to disk */
#if 0
	if(M_MF()->mst_ExternIni.b_SynchroEditorsData)
	{
		FILE	*f, *f1;
		char	asz_Name[512];

		strcpy(asz_Name, EDI_go_TheApp.m_pszHelpFilePath);
		*strrchr(asz_Name, '\\') = 0;
		strcat(asz_Name, "/EditorDatas");
		CreateDirectory(asz_Name, NULL);
		strcat(asz_Name, "/Ini/");
		CreateDirectory(asz_Name, NULL);
		strcat(asz_Name, _psz_IniFile);
		strcat(asz_Name, EDI_Csz_ExtIni);
		f = fopen(asz_Name, "wb");
		if(f)
		{
			fwrite(_puc_IniStruct, _i_SizeOfIniStruct, 1, f);
			fclose(f);
		}

		strcpy(asz_Name, EDI_go_TheApp.m_pszHelpFilePath);
		*strrchr(asz_Name, '\\') = 0;
		strcat(asz_Name, "/EditorDatas/Ini/Desktop/");
		CreateDirectory(asz_Name, NULL);
		strcat(asz_Name, mst_Ini.asz_CurrentDeskName);
		strcat(asz_Name, "/");
		CreateDirectory(asz_Name, NULL);
		strcat(asz_Name, _psz_IniFile);
		strcat(asz_Name, EDI_Csz_ExtIni);
		f1 = fopen(asz_Name, "wb");
		if(f1)
		{
			fwrite(_puc_IniStruct, _i_SizeOfIniStruct, 1, f1);
			fclose(f1);
		}
	}
#endif

	BIG_ul_CreateDir(EDI_Csz_Ini);

	/* Compute file name */
	o_Name = _psz_IniFile;
	o_Name += EDI_Csz_ExtIni;

	/* Save in normal folder */
	BIG_UpdateFileFromBuffer(EDI_Csz_Ini, (char *) (LPCSTR) o_Name, _puc_IniStruct, _i_SizeOfIniStruct);

	/* Save in desktop folder */
	L_strcpy(asz_PathIni, EDI_Csz_Ini_Desktop);
	L_strcat(asz_PathIni, "/");
	L_strcat(asz_PathIni, mst_Ini.asz_CurrentDeskName);
	BIG_ul_CreateDir(asz_PathIni);
	BIG_UpdateFileFromBuffer(asz_PathIni, (char *) (LPCSTR) o_Name, _puc_IniStruct, _i_SizeOfIniStruct);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::ReinitIni(void)
{
	/* Current desktop */
	L_strcpy(mst_Ini.asz_CurrentDeskName, EDI_Csz_DefaultDesktop);

	/* Menu frame */
	mst_Ini.i_LeftFrameSize = 200;
	mst_Ini.b_LeftFrameVisible = TRUE;
	mst_Ini.i_CountMenu = 0;
	mst_Ini.b_TopMenu = TRUE;
	mst_Ini.i_TopInMenu = 50;
	mst_Ini.ui_HeightMenu = 14;
	mst_Ini.b_ActivateUniverse = FALSE;
	mst_Ini.b_IntAsHex = FALSE;
	mst_Ini.mi_JoyType = win32INO_l_Joystick_Mode = INO_Joy_PCMode;

	/* Action set (keyboard) */
	mst_Ini.i_ActionSet = 0;

	/* Link control */
	mst_Ini.b_LinkControlON = FALSE;
	mst_Ini.asz_CurrentVSSFile[0] = 0;
	mst_Ini.asz_CurrentRefFile[0] = 0;
	L_strcpy(mst_Ini.asz_CurrentUserName, (char *) (LPCSTR) EDI_Csz_AdminUserName);
	mst_Ini.asz_UserPassWord[0] = 0;
	mst_Ini.ui_NumOpen = 0;

	/* Options */
	mst_Ini.mst_Options.b_SaveWhenExit = FALSE;
	mst_Ini.mst_Options.b_AskExit = TRUE;
	mst_Ini.mst_Options.x_ColRunEngine = 0x000000FF;

	/* joystick */
	for(int i = 0; i < 2; i++)
	{
		win32INO_l_Joystick_YDownStart[i] = 42000;
		win32INO_l_Joystick_XRightStart[i] = 42000;
		win32INO_l_Joystick_YUpStart[i] = 22000;
		win32INO_l_Joystick_XLeftStart[i] = 22000;
		win32INO_l_Joystick_YDown[i] = 65536;
		win32INO_l_Joystick_XRight[i] = 65536;
		win32INO_l_Joystick_YUp[i] = 0;
		win32INO_l_Joystick_XLeft[i] = 0;
	}

#ifdef JADEFUSION
	//disable all features 
	for(int i = 0; i < EDI_MTL_MAX_FEATURES; i++)
		ab_MtlFeatures[i] = FALSE;
#endif

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::LoadIni(void)
{
_Try_
	ReinitIni();
	BaseLoadIni(EDI_Csz_FrameIniFile, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
	/* Check link control */
	if(mst_Ini.b_LinkControlON) OnLinkControlBeg();

	/* Kit names of engine struct */
	LINK_LoadKitNumbers();

	/* Read action list */
	mpo_Actions->DeleteAll();
	mpo_Actions->ParseBuffer(EDI_asz_ActionBase);
	mpo_Actions->ReadFile(EDI_Csz_FrameIniFile);

	if(mst_Ini.ui_HeightMenu < 14) mst_Ini.ui_HeightMenu = 14;
	if(mst_Ini.ui_HeightMenu > 24) mst_Ini.ui_HeightMenu = 14;

	/* Activate universe ? */
	ENG_gb_ActivateUniverse = mst_Ini.b_ActivateUniverse;

	/*$S
	 * Update views.
	 */

	/* Left frame */
	if(mst_Ini.b_LeftFrameVisible)
		mo_BigSplitter.SetColumnInfo(0, mst_Ini.i_LeftFrameSize, 0);
	else
		mo_BigSplitter.SetColumnInfo(0, -1, -1);

	/* joystick */
	if(mst_Ini.al_JoyCalibration[0] & 0x0FF00000 || mst_Ini.al_JoyCalibration1[0] & 0x0FF00000)
	{
		for(int i = 0; i < 2; i++)
		{
			win32INO_l_Joystick_YDownStart[i] = 42000;
			win32INO_l_Joystick_XRightStart[i] = 42000;
			win32INO_l_Joystick_YUpStart[i] = 22000;
			win32INO_l_Joystick_XLeftStart[i] = 22000;
			win32INO_l_Joystick_YDown[i] = 65536;
			win32INO_l_Joystick_XRight[i] = 65536;
			win32INO_l_Joystick_YUp[i] = 0;
			win32INO_l_Joystick_XLeft[i] = 0;
		}
	}
	else
	{
		win32INO_l_Joystick_YDownStart[0] = mst_Ini.al_JoyCalibration[0];
		win32INO_l_Joystick_XRightStart[0] = mst_Ini.al_JoyCalibration[1];
		win32INO_l_Joystick_YUpStart[0] = mst_Ini.al_JoyCalibration[2];
		win32INO_l_Joystick_XLeftStart[0] = mst_Ini.al_JoyCalibration[3];
		win32INO_l_Joystick_YDown[0] = mst_Ini.al_JoyCalibration[4];
		win32INO_l_Joystick_XRight[0] = mst_Ini.al_JoyCalibration[5];
		win32INO_l_Joystick_YUp[0] = mst_Ini.al_JoyCalibration[6];
		win32INO_l_Joystick_XLeft[0] = mst_Ini.al_JoyCalibration[7];

		win32INO_l_Joystick_YDownStart[1] = mst_Ini.al_JoyCalibration1[0];
		win32INO_l_Joystick_XRightStart[1] = mst_Ini.al_JoyCalibration1[1];
		win32INO_l_Joystick_YUpStart[1] = mst_Ini.al_JoyCalibration1[2];
		win32INO_l_Joystick_XLeftStart[1] = mst_Ini.al_JoyCalibration1[3];
		win32INO_l_Joystick_YDown[1] = mst_Ini.al_JoyCalibration1[4];
		win32INO_l_Joystick_XRight[1] = mst_Ini.al_JoyCalibration1[5];
		win32INO_l_Joystick_YUp[1] = mst_Ini.al_JoyCalibration1[6];
		win32INO_l_Joystick_XLeft[1] = mst_Ini.al_JoyCalibration1[7];
	}

	win32INO_l_Joystick_Mode = mst_Ini.mi_JoyType;

#ifdef JADEFUSION
	if(win32INO_l_Joystick_Mode > 2) win32INO_l_Joystick_Mode = mst_Ini.mi_JoyType = INO_Joy_XenonMode;
#else
	if(win32INO_l_Joystick_Mode > 1) win32INO_l_Joystick_Mode = mst_Ini.mi_JoyType = INO_Joy_PCMode;
#endif

	/* Update all views */
	RecalcLayout();
	mo_Splitter.RecalcLayout();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::SaveIni(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				ix_Temp1, ix_Temp;
	CRect			o_Rect;
	CControlBarInfo o_Info;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Kit names of engine struct */
	LINK_SaveKitNumbers();

	/* Topmenu toolbar pos */
	if(mpo_Menu && mpo_Menu->mpo_InMenu)
	{
		mpo_Menu->mpo_InMenu->GetWindowRect(&o_Rect);
		mpo_Menu->mpo_InMenu->ScreenToClient(&o_Rect);
		mst_Ini.i_TopInMenu = o_Rect.right - o_Rect.left + 1;
	}

	/* External ini file */
	SaveExternIni();

	/* Left menu */
	mo_BigSplitter.GetColumnInfo(0, ix_Temp1, ix_Temp);
	if(ix_Temp1 >= 0) mst_Ini.i_LeftFrameSize = ix_Temp1;

	/* joystick calibration */
	mst_Ini.al_JoyCalibration[0] = win32INO_l_Joystick_YDownStart[0];
	mst_Ini.al_JoyCalibration[1] = win32INO_l_Joystick_XRightStart[0];
	mst_Ini.al_JoyCalibration[2] = win32INO_l_Joystick_YUpStart[0];
	mst_Ini.al_JoyCalibration[3] = win32INO_l_Joystick_XLeftStart[0];
	mst_Ini.al_JoyCalibration[4] = win32INO_l_Joystick_YDown[0];
	mst_Ini.al_JoyCalibration[5] = win32INO_l_Joystick_XRight[0];
	mst_Ini.al_JoyCalibration[6] = win32INO_l_Joystick_YUp[0];
	mst_Ini.al_JoyCalibration[7] = win32INO_l_Joystick_XLeft[0];
	mst_Ini.al_JoyCalibration1[0] = win32INO_l_Joystick_YDownStart[1];
	mst_Ini.al_JoyCalibration1[1] = win32INO_l_Joystick_XRightStart[1];
	mst_Ini.al_JoyCalibration1[2] = win32INO_l_Joystick_YUpStart[1];
	mst_Ini.al_JoyCalibration1[3] = win32INO_l_Joystick_XLeftStart[1];
	mst_Ini.al_JoyCalibration1[4] = win32INO_l_Joystick_YDown[1];
	mst_Ini.al_JoyCalibration1[5] = win32INO_l_Joystick_XRight[1];
	mst_Ini.al_JoyCalibration1[6] = win32INO_l_Joystick_YUp[1];
	mst_Ini.al_JoyCalibration1[7] = win32INO_l_Joystick_XLeft[1];
	mst_Ini.mi_JoyType = win32INO_l_Joystick_Mode;

_Try_
	BaseSaveIni(EDI_Csz_FrameIniFile, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::SynchronizeEditorsDataImport(char *_pz_Dir)
{
	char				asz_Temp[1024];
	char				asz_BigName[1024];
	char				asz_BigName1[1024];
	struct L_finddata_t st_FileInfo;
	ULONG				ul_Handle;
	char				*pz_Dir1;

    // Some directories should not be read
    if (strstr(_pz_Dir,"Name Kits"))
        return;

	strcpy(asz_Temp, _pz_Dir);
	strcat(asz_Temp, "/*.*");
	ul_Handle = L_findfirst(asz_Temp, &st_FileInfo);
	if(ul_Handle != -1)
	{
		do
		{
			if(st_FileInfo.attrib & L_A_SUBDIR)
			{
				if(!L_strcmpi(st_FileInfo.name, ".")) continue;
				if(!L_strcmpi(st_FileInfo.name, "..")) continue;

				L_strcpy(asz_BigName, _pz_Dir);
				L_strcat(asz_BigName, "/");
				L_strcat(asz_BigName, st_FileInfo.name);
				SynchronizeEditorsDataImport(asz_BigName);
			}
			else
			{
				L_strcpy(asz_BigName, _pz_Dir);
				L_strcat(asz_BigName, "/");
				L_strcat(asz_BigName, st_FileInfo.name);

				pz_Dir1 = strstr(_pz_Dir, "EditorDatas");
				sprintf(asz_BigName1, "Root/%s", pz_Dir1);
				if(BIG_ul_SearchDir(asz_BigName1) != BIG_C_InvalidIndex)
					BIG_UpdateFileFromDisk(asz_BigName, asz_BigName1, st_FileInfo.name);
			}
		} while(L_findnext(ul_Handle, &st_FileInfo) != -1);
		L_findclose(ul_Handle);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::SynchronizeEditorsData(BOOL _b_Sens)
{
	char	az[1024];

	if(!mst_ExternIni.b_SynchroEditorsData) return;
	if(EDI_gb_NoVerbose) return;

	if(_b_Sens)
	{
		if(EDI_gpo_EnterWnd) 
			EDI_gpo_EnterWnd->DisplayMessage("Synchronizing EditorsData");
		else
			LINK_PrintStatusMsg("Synchronizing EditorsData");
		BIG_gb_IgnoreDateExt = TRUE;
_Try_
		strcpy(az, EDI_go_TheApp.m_pszHelpFilePath);
		*strrchr(az, '\\') = 0;
		strcat(az, "/EditorDatas");
		SynchronizeEditorsDataImport(az);
_Catch_
_End_
		if(EDI_gpo_EnterWnd) 
			EDI_gpo_EnterWnd->DisplayMessage("Synchronizing EditorsData : OK");
		else
			LINK_PrintStatusMsg("Synchronizing EditorsData : OK");
		ERR_gb_Warning = FALSE;
		BIG_gb_IgnoreDateExt = FALSE;
	}
	else
	{
		strcpy(az, EDI_go_TheApp.m_pszHelpFilePath);
		*strrchr(az, '\\') = 0;
		strcat(az, "/EditorDatas");
		BIG_ExportDirToDisk(az, "Root/EditorDatas");
	}
}

#endif /* ACTIVE_EDITORS */
