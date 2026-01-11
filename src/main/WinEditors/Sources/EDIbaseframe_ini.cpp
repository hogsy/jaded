/*$T EDIbaseframe_ini.cpp GC!1.40 07/08/99 15:26:04 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "EDIbaseframe.h"
#include "EDImainframe.h"
#include "EDIaction.h"
#include "EDItors/Sources/MENu/MENin.h"
#include "EDItors/Sources/MENu/MENmenu.h"
#include "EDItors/Sources/MENu/MENinmenu.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "BASe/CLIbrary/CLImem.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseFrame::BaseLoadIni
(
    char    *_psz_IniFile,
    UCHAR   *_puc_IniStruct,
    int     _i_SizeOfIniStruct
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    UCHAR   *puc_AllIni;
	BOOL	bErr;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    puc_AllIni = new UCHAR[sizeof(mst_BaseIni) + _i_SizeOfIniStruct + 1024];
    ERR_X_Error(puc_AllIni != NULL, L_ERR_Csz_NotEnoughMemory, NULL);

    /*$2
     -----------------------------------------------------------------------------------------------
        Concat inis
     -----------------------------------------------------------------------------------------------
     */

    L_memcpy(puc_AllIni, &mst_BaseIni, sizeof(mst_BaseIni));
    if(_puc_IniStruct)
        L_memcpy(puc_AllIni + sizeof(mst_BaseIni), _puc_IniStruct, _i_SizeOfIniStruct);

    M_MF()->BaseLoadIni(_psz_IniFile, puc_AllIni, sizeof(mst_BaseIni) + _i_SizeOfIniStruct);

    /*$2
     -----------------------------------------------------------------------------------------------
        Load in current desktop
     -----------------------------------------------------------------------------------------------
     */

	bErr = FALSE;
_Try_
    BaseLoadIniDesktop(_psz_IniFile, _puc_IniStruct, _i_SizeOfIniStruct);
_Catch_
	bErr = TRUE;
_End_

    /*$2
     -----------------------------------------------------------------------------------------------
        Fragment inis
     -----------------------------------------------------------------------------------------------
     */
    if(!bErr || (GetAsyncKeyState(VK_CONTROL) >= 0))
    {
        L_memcpy(&mst_BaseIni, puc_AllIni, sizeof(mst_BaseIni));
        if(_puc_IniStruct)
            L_memcpy(_puc_IniStruct, puc_AllIni + sizeof(mst_BaseIni), _i_SizeOfIniStruct);
    }

    delete puc_AllIni;

    /* Display/hide the dialog bar (if there's one) */
    if(mpo_DialogBar && mst_BaseIni.b_DialogBarVisible == FALSE)
        mpo_DialogBar->ShowWindow(SW_HIDE);

    /* Top menu */
    if(mpo_MenuFrame)
    {
        if(mpo_Menu && !mst_BaseIni.b_TopMenu)
            mpo_MenuFrame->DestroyTopMenu(this);
        else if(mst_BaseIni.b_TopMenu && !mpo_Menu)
		{
            mpo_MenuFrame->CreateTopMenu(this, TRUE);
		}
    }
}

/*
 ===================================================================================================
    Aim:    Read an editor IA file in a temporary buffer. Treat desktop base ini values, and return
            a pointer on the buffer. Editors can use that pointer to store in their ini struct some
            value linked with desktops.
 ===================================================================================================
 */
char *EDI_cl_BaseFrame::BaseLoadIniDesktop
(
    char    *_psz_IniFile,
    UCHAR   *_puc_IniStruct,
    int     _i_SizeOfIniStruct
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CString     o_Name;
    char        asz_PathIni[BIG_C_MaxLenPath];
    BIG_INDEX   ul_Index;
    ULONG       ul_Size;
    UCHAR       *puc_TempBuf;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_Name = _psz_IniFile;
	o_Name += EDI_Csz_ExtIni;
    L_strcpy(asz_PathIni, EDI_Csz_Ini_Desktop);
    L_strcat(asz_PathIni, "/");
    L_strcat(asz_PathIni, M_MF()->mst_Ini.asz_CurrentDeskName);
    ul_Index = BIG_ul_SearchFileExt(asz_PathIni, (char *) (LPCSTR) o_Name);
    if(ul_Index != BIG_C_InvalidIndex)
    {
        puc_TempBuf = (UCHAR *) BIG_pc_ReadFileTmp(BIG_PosFile(ul_Index), &ul_Size);

        /* Is visible */
        L_memcpy
        (
            &mst_BaseIni.b_IsVisible,
            puc_TempBuf + ((char *) &mst_BaseIni.b_IsVisible - (char *) &mst_BaseIni),
            sizeof(BOOL)
        );

        /* Links */
        L_memcpy
        (
            &mst_BaseIni.ab_LinkTo,
            puc_TempBuf + ((char *) mst_BaseIni.ab_LinkTo - (char *) &mst_BaseIni),
            sizeof(mst_BaseIni.ab_LinkTo)
        );
        return (char *) puc_TempBuf + sizeof(mst_BaseIni);
    }
    else 
        return NULL;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseFrame::BaseSaveIni
(
    char    *_psz_IniFile,
    UCHAR   *_puc_IniStruct,
    int     _i_SizeOfIniStruct
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    UCHAR   *puc_AllIni;
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Concat inis */
    puc_AllIni = new UCHAR[sizeof(mst_BaseIni) + _i_SizeOfIniStruct];

	/* Menu top or left */
	if(mpo_Menu)
	{
		mst_BaseIni.mb_LeftMode = mpo_Menu->mb_LeftMode;
	}

    /* Topmenu toolbar pos */
    if(mpo_Menu && mpo_Menu->mpo_InMenu)
    {
		if(mst_BaseIni.mb_LeftMode)
		{
			mpo_Menu->mpo_InMenu->GetWindowRect(&o_Rect);
			mpo_Menu->mpo_InMenu->ScreenToClient(&o_Rect);
			mst_BaseIni.i_TopInMenu = o_Rect.bottom - o_Rect.top + 1;
		}
		else
		{
			mpo_Menu->mpo_InMenu->GetWindowRect(&o_Rect);
			mpo_Menu->mpo_InMenu->ScreenToClient(&o_Rect);
			mst_BaseIni.i_TopInMenu = o_Rect.right - o_Rect.left + 1;
		}
    }

    L_memcpy(puc_AllIni, &mst_BaseIni, sizeof(mst_BaseIni));
    if(_puc_IniStruct)
        L_memcpy(puc_AllIni + sizeof(mst_BaseIni), _puc_IniStruct, _i_SizeOfIniStruct);

    M_MF()->BaseSaveIni(_psz_IniFile, puc_AllIni, sizeof(mst_BaseIni) + _i_SizeOfIniStruct);

    delete puc_AllIni;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseFrame::ReinitIni(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    mst_BaseIni.b_EngineRefresh = FALSE;
    mst_BaseIni.b_EditorRefresh = FALSE;
    mst_BaseIni.b_DialogBarVisible = TRUE;
    mst_BaseIni.b_IsVisible = TRUE;
    mst_BaseIni.i_CountMenu = 0;
    mst_BaseIni.b_TopMenu = TRUE;
    mst_BaseIni.i_TopInMenu = 50;
	mst_BaseIni.mb_LeftMode = FALSE;
	mst_BaseIni.YSplitMenu = 200;
	mst_BaseIni.ISplitMenu = 0;

    for(i = 0; i < EDI_C_MaxHistory; i++)
        mst_BaseIni.aul_History[i] = BIG_C_InvalidIndex;

    /* No links */
    for(i = 0; i < EDI_C_MaxEditors; i++)
    {
        for(int j = 0; j < EDI_C_MaxDuplicate; j++)
            mst_BaseIni.ab_LinkTo[i][j] = FALSE;
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseFrame::LoadIni(void)
{
_Try_
    BaseLoadIni(mst_Def.asz_Name, NULL, 0);
_Catch_
_End_
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseFrame::LoadIniDesktop(void)
{
_Try_
    BaseLoadIniDesktop(mst_Def.asz_Name, NULL, 0);
_Catch_
_End_
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_BaseFrame::SaveIni(void)
{
_Try_
    BaseSaveIni(mst_Def.asz_Name, NULL, 0);
_Catch_
_End_
}

#endif /* ACTIVE_EDITORS */
