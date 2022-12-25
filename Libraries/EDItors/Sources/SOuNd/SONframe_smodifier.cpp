/*$T SONframe_smodifier.cpp GC 1.138 12/10/03 10:31:34 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS

/*$2- editors --------------------------------------------------------------------------------------------------------*/

#include "Res/Res.h"
#include "LINks/LINKtoed.h"
#include "LINks/LINKmsg.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "EDIstrings.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAsndgenplayer_dlg.h"
#include "BROwser/BROframe.h"
#include "EDImsg.h"

/*$2- engine ---------------------------------------------------------------------------------------------------------*/

#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "WORld/WORstruct.h"
#include "WORld/WORvars.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_GEN.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "SouND/sources/SND.h"
#include "SouND/sources/SNDwave.h"
#include "SouND/sources/SNDconst.h"
#include "SouND/sources/SNDmodifier.h"
#include "SouND/sources/SNDmusic.h"
#include "SouND/sources/SNDambience.h"
#include "SouND/sources/SNDdialog.h"
#include "SouND/sources/SNDloadingsound.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDbank.h"

#include "SONframe.h"
#include "SONview.h"
#include "SONview_smd.h"
#include "SONview_smdins.h"
#include "SONview_smdlist.h"
#include "SONstrings.h"
#include "SONmsg.h"
#include "SONpane.h"
#include "SONutil.h"

/*$4
 ***********************************************************************************************************************
    extern variables
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */
void ESON_cl_Frame::SModifier_DisplayName(void)
{
    CString name;
   	char				asz_CurrentOwner[256];

    if(mst_SModifierDesc.ul_Fat == BIG_C_InvalidIndex)
    {
        name = CString("");
    }
    else
    {
        if(mst_SModifierDesc.b_NeedSaving)
            name = CString(BIG_NameFile(mst_SModifierDesc.ul_Fat)) + CString("*");
        else
            name = CString(BIG_NameFile(mst_SModifierDesc.ul_Fat));
    	
        if(M_MF()->mst_Ini.b_LinkControlON)
        {
		    L_strcpy(asz_CurrentOwner, BIG_P4OwnerName(mst_SModifierDesc.ul_Fat));
   			if(!L_strnicmp(asz_CurrentOwner, "_nobody_", 8)) *asz_CurrentOwner = 0;
		    if(*asz_CurrentOwner)
			    name += CString(" (") + CString(asz_CurrentOwner) + CString(")");
        }
    }
        
    mpo_PaneSmd->SetWindowText(name);
	mpo_PaneSmd->Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_cl_Frame::b_SModifier_CanClose(void)
{
	/*~~~~~~~~~~*/
	int		i_Res;
	CString o_Str;
	/*~~~~~~~~~~*/

	if(mst_SModifierDesc.ul_Fat == BIG_C_InvalidIndex) return TRUE;
	if(mst_SModifierDesc.b_NeedSaving)
	{
		if(mst_SModifierDesc.b_AutoSave)
		{
			SModifier_SaveCurrent();
		}
		else
		{
			/* Ask user to close without saving */
			o_Str = "Save SModifier file ";
			o_Str += BIG_NameFile(mst_SModifierDesc.ul_Fat);
			o_Str += "\n";
			o_Str += "before continuing ?";
			i_Res = M_MF()->MessageBox
				(
					(char *) (LPCSTR) o_Str,
					EDI_STR_Csz_TitleConfirm,
					MB_ICONQUESTION | MB_YESNOCANCEL
				);
			if(i_Res == IDCANCEL) return FALSE;
			if(i_Res == IDYES) SModifier_SaveCurrent();
			mst_SModifierDesc.b_NeedSaving = FALSE;
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SModifier_SaveCurrent(BOOL b_refresh)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_OneSound	*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mst_SModifierDesc.b_NeedSaving) return;

	if
	(
		(mst_SModifierDesc.ul_Fat == BIG_C_InvalidIndex)
	||	(mst_SModifierDesc.ul_Key == BIG_C_InvalidKey)
	||	(mst_SModifierDesc.i_Index == -1)
	)
	{
		M_MF()->MessageBox("Fatal error can't save the current SModifier", "Bad News", MB_OK | MB_ICONSTOP);
		return;
	}

	/* unload */
	LOA_DeleteAddress((void *) LOA_ul_SearchAddress(BIG_PosFile(mst_SModifierDesc.ul_Fat)));

	/* Compute the file name and the path name and open the file */
	BIG_ComputeFullName(BIG_ParentFile(mst_SModifierDesc.ul_Fat), mst_SModifierDesc.sz_Path);

	/* create the file from the struct */
	pst_Sound = &SND_gst_Params.dst_Sound[mst_SModifierDesc.i_Index];
	if(!pst_Sound->pst_SModifier) return ;
	SND_SModifierSaveFromBuffer(mst_SModifierDesc.ul_Fat, pst_Sound->pst_SModifier);

	LINK_PrintStatusMsg("SModifier file Saved.");
	mst_SModifierDesc.b_NeedSaving = FALSE;
    
    if(b_refresh) SModifier_DisplayName();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SModifier_CloseCurrent(BOOL b_refresh)
{
	if(mst_SModifierDesc.ul_Fat != BIG_C_InvalidIndex)
	{
		SND_StopRq(mst_SModifierDesc.i_Instance);
		SND_DeleteByIndex(mst_SModifierDesc.i_Index);
	}

	mst_SModifierDesc.ul_Fat = BIG_C_InvalidIndex;
	mst_SModifierDesc.i_Index = -1;
	mst_SModifierDesc.i_Instance = -1;

	mst_SModifierDesc.ul_Key = BIG_C_InvalidKey;
	if(b_refresh)
	{
        mpo_ViewSmd->UpdateSmdView();
		mpo_PaneSmd->SetWindowText("");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SModifier_Open(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File("Choose File", 0, 0, 1, NULL, "*"EDI_Csz_ExtSModifier);
	CString				o_Temp;
	BIG_INDEX			ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		o_File.GetItem(o_File.mo_File, 0, o_Temp);
		ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
		if(ul_Index != BIG_C_InvalidIndex) SModifier_Set(ul_Index);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SModifier_Set(BIG_INDEX _ul_Fat, BOOL _b_Update)
{
	if(mst_SModifierDesc.ul_Fat != -1)
	{
		if(!b_SModifier_CanClose()) return;
		SModifier_CloseCurrent();
	}

	mst_SModifierDesc.ul_Fat = _ul_Fat;
	mst_SModifierDesc.ul_Key = BIG_FileKey(_ul_Fat);
	M_MF()->AddHistoryFile(this, mst_SModifierDesc.ul_Key);
	BIG_ComputeFullName(BIG_ParentFile(_ul_Fat), mst_SModifierDesc.sz_Path);

	/* check lenght */
	if(BIG_ul_GetLengthFile(BIG_PosFile(mst_SModifierDesc.ul_Fat)) == 0)
	{
		SND_SModifierCreateNewFile(mst_SModifierDesc.ul_Fat, NULL);
	}

	mst_SModifierDesc.i_Index = SND_l_AddSModifier(mst_SModifierDesc.ul_Key);
	LOA_Resolve();
	ESON_LoadAllSounds();

	if(_b_Update)
	{
		mpo_ViewSmd->UpdateSmdView();
	    SModifier_DisplayName();
		mpo_View->FillListSound();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SModifier_New(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog("Enter a new SModifier name");
	char				asz_Name[BIG_C_MaxLenPath];
	char				*psz_Name;
	BIG_INDEX			ul_New;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!b_SModifier_CanClose()) return;

	_Try_ if(o_Dialog.DoModal() == IDOK)
	{
		psz_Name = (char *) (LPCSTR) o_Dialog.mo_Name;
		if(BIG_b_CheckName(psz_Name) == FALSE) return;

		L_strcpy(asz_Name, psz_Name);
		L_strcat(asz_Name, EDI_Csz_ExtSModifier);
		if(BIG_ul_SearchFileExt(EDI_Csz_Path_Audio, asz_Name) != BIG_C_InvalidIndex)
		{
			ERR_X_ForceError("SModifier name already exists : ", psz_Name);
		}
		else
		{
			BIG_ul_CreateDir(EDI_Csz_Path_Audio);
			ul_New = BIG_ul_CreateFile(EDI_Csz_Path_Audio, asz_Name);

			/* Inform all editors */
			M_MF()->FatHasChanged();

			/* create empty file */
			SND_SModifierCreateNewFile(ul_New, NULL);

			/* Set created as current one */
			SModifier_Set(ul_New);
		}
	}

	_Catch_ _End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SModifier_CreateAss(BIG_INDEX _ul_SoundFileFat)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						asz_Path[1024];
	char						*psz_SoundFileName;
	char						*pz_Temp;
	char						asz_SMDName[256];
	BIG_INDEX					ul_New;
	int							i;
	int							i_SoundFile;
	SND_tdst_SModifierExtPlayer st_Player;
    BOOL                        b_Streamed;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	if(_ul_SoundFileFat == BIG_C_InvalidIndex) return;
	if(ESON_b_IsFileTruncated(BIG_FileKey(_ul_SoundFileFat))) return;

	/* get path and file name */
	BIG_ComputeFullName(BIG_ParentFile(_ul_SoundFileFat), asz_Path);
	psz_SoundFileName = BIG_NameFile(_ul_SoundFileFat);
	pz_Temp = L_strrchr(psz_SoundFileName, '.');
    L_memset(&st_Player, 0, sizeof(SND_tdst_SModifierExtPlayer));
    b_Streamed = FALSE;

	/* check file extension */
	if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundAmbience, 4))
    {
		i_SoundFile = SND_l_AddAmbience(BIG_FileKey(_ul_SoundFileFat));
        SND_M_SModifierSetGroup_st(st_Player, SND_Cte_SModifierGroupAmbience);
        b_Streamed = TRUE;
    }
	else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundMusic, 4))
    {
		i_SoundFile = SND_l_AddMusic(BIG_FileKey(_ul_SoundFileFat));
        SND_M_SModifierSetGroup_st(st_Player, SND_Cte_SModifierGroupMusic);
        b_Streamed = TRUE;
    }
	else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundDialog, 4))
    {
		i_SoundFile = SND_l_AddDialog(BIG_FileKey(_ul_SoundFileFat));
        SND_M_SModifierSetGroup_st(st_Player, SND_Cte_SModifierGroupDialog);
    }
	else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSoundFile, 4))
    {
		i_SoundFile = SND_l_AddSound(BIG_FileKey(_ul_SoundFileFat));
        SND_M_SModifierSetGroup_st(st_Player, SND_Cte_SModifierGroupSFX);
    }
	else if(!L_strnicmp(pz_Temp, EDI_Csz_ExtLoadingSound, 4))
    {
		i_SoundFile = SND_l_AddLoadingSound(BIG_FileKey(_ul_SoundFileFat));
        SND_M_SModifierSetGroup_st(st_Player, SND_Cte_SModifierGroupSFX);
    }
	else
    {
		i_SoundFile = -1;        
    }

	if(i_SoundFile == -1) return;
	LOA_Resolve();
    ESON_LoadAllSounds();

	/* make SModifier file name */
	for(pz_Temp = psz_SoundFileName, i = 0; *pz_Temp != '.'; pz_Temp++, i++)
	{
		asz_SMDName[i] = *pz_Temp;
	}

	asz_SMDName[i] = '\0';
	L_strcat(asz_SMDName, EDI_Csz_ExtSModifier);

	/* check existence */
	if(BIG_ul_SearchFileExt(asz_Path, asz_SMDName) != BIG_C_InvalidIndex)
	{
		ERR_X_ForceError("SModifier name already exists : ", asz_SMDName);
	}
	else
	{
		BIG_ul_CreateDir(asz_Path);
		ul_New = BIG_ul_CreateFile(asz_Path, asz_SMDName);

		/* Inform all editors */
		M_MF()->FatHasChanged();

		/* create empty file */
		/**/
		st_Player.ul_SndKey = -1;
		st_Player.ul_SndFlags = SND_gst_Params.dst_Sound[i_SoundFile].ul_Flags & SND_Cul_SF_MaskFlagsFromSoundToInstance;
        if(!b_Streamed)
        st_Player.ul_SndFlags |= SND_Cul_SF_DynPan|SND_Cul_SF_DynVolSpheric;
		st_Player.i_SndIndex = -1;
		/**/
		st_Player.ui_Version = SND_Cte_SModifierExtPlayerVersion;
		/**/
		st_Player.ui_SndExtFlags = SND_gst_Params.dst_Sound[i_SoundFile].ul_ExtFlags & SND_Cul_ESF_MaskDynExtFlags;
		st_Player.p_Template = NULL;
        /**/
        if(!b_Streamed)
		st_Player.ui_PlayerFlag = SND_Cte_SModifierFxA;
		/**/
		st_Player.f_DryVol = 1.0f;
        if(b_Streamed)
        {
            st_Player.f_DryVol_FactMin = 0.0f;
            st_Player.f_DryVol_FactMax = 0.0f;
        }
        else
        {
        st_Player.f_DryVol_FactMin = 0.9f;
        st_Player.f_DryVol_FactMax = 1.0f;
        }
        st_Player.f_FxVolLeft= 0.5f;
        st_Player.f_FxVolRight= 0.5f;
		/**/
		st_Player.ul_FadeInKey = -1;
		st_Player.pst_FadeIn = NULL;
		st_Player.ul_FadeOutKey = -1;
		st_Player.pst_FadeOut = NULL;
		/**/
		st_Player.i_Pan = 0;
		st_Player.i_Span = 0;
		st_Player.f_MinPan = 0.0f;
		/**/
		st_Player.f_FreqCoef = 1.f;
        if(b_Streamed)
        {
            st_Player.f_Freq_FactMin = 0.0f;
            st_Player.f_Freq_FactMax = 0.0f;
        }
        else
        {
        st_Player.f_Freq_FactMin = 0.95f;
        st_Player.f_Freq_FactMax = 1.0f;
        }
		st_Player.f_Doppler = 0.0f;
		/**/
		st_Player.af_Near[0] = 2.0f;
		st_Player.af_Near[1] = 2.0f;
		st_Player.af_Near[2] = 2.0f;
		st_Player.af_Far[0] = 12.0f;
		st_Player.af_Far[1] = 12.0f;
		st_Player.af_Far[2] = 12.0f;
		st_Player.af_MiddleBlend[0] = 0.33f;
		st_Player.af_MiddleBlend[1] = 0.33f;
		st_Player.af_MiddleBlend[2] = 0.33f;
		st_Player.f_FarCoeff = 0.0f;
		st_Player.f_MiddleCoeff = 0.33f;
		st_Player.f_CylinderHeight = 0.0f;
		/**/
		L_memset(st_Player.ac_Reserved, 0, SND_Cte_SModifierExtPlayerReservedSize*sizeof(char));
        /**/
        st_Player.st_PlayList.al_IdxList = new LONG[1];
        st_Player.st_PlayList.al_IdxList[0] = -1;

        st_Player.st_PlayList.aul_FlagList = new ULONG[1];
        st_Player.st_PlayList.aul_FlagList[0] = SND_gst_Params.dst_Sound[i_SoundFile].ul_Flags & SND_Cul_SF_MaskFlagsFromSoundToInstance;

        st_Player.st_PlayList.aul_KeyList = new ULONG[1];
        st_Player.st_PlayList.aul_KeyList[0] = BIG_FileKey(_ul_SoundFileFat);

        st_Player.st_PlayList.us_Idx = 0;
        st_Player.st_PlayList.us_Size = 1;


        if(!b_Streamed)
        SND_M_SModifierSetGroup(&st_Player, SND_Cte_SModifierGroupSFX);

		SND_SModifierCreateNewFile(ul_New, &st_Player);
        delete [] st_Player.st_PlayList.al_IdxList;
        delete [] st_Player.st_PlayList.aul_FlagList;
        delete [] st_Player.st_PlayList.aul_KeyList;
		
		/* unload sound file */
		SND_DeleteByIndex(i_SoundFile);

		/* Set created as current one */
		if(mst_Ini.i_Option & ESON_Cte_AutoOpen) SModifier_Set(ul_New);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SModifier_Stop(void)
{
	if(mst_SModifierDesc.i_Instance != -1)
	{
		SND_Release(mst_SModifierDesc.i_Instance);
		mst_SModifierDesc.i_Instance = -1;
		mpo_View->FillListInstance();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SModifier_Pause(void)
{
	if(SND_i_IsPlaying(mst_SModifierDesc.i_Instance))
		SModifier_Stop();
	else
		SModifier_Play();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SModifier_Play(BOOL _b_Looped)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance		*pst_SI;
	SND_tdst_SModifier			*pst_SMod;
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_SModifierDesc.i_Index == -1) return;
	SModifier_Stop();
	
    SND_Release(mst_SModifierDesc.i_Instance);
	mst_SModifierDesc.i_Instance = -1;

	if(mst_SModifierDesc.i_Instance == -1)
	{
		mst_SModifierDesc.i_Instance = SND_l_Request(NULL, mst_SModifierDesc.i_Index);
		if(mst_SModifierDesc.i_Instance < 0) return;
		SND_SetFlags(NULL, mst_SModifierDesc.i_Instance, SND_Cul_DSF_DestroyWhenFinished);
	}

	pst_SI = SND_gst_Params.dst_Instance + mst_SModifierDesc.i_Instance;
	pst_SI->ul_Flags &= ~(SND_Cul_SF_DynVolSpheric | SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis);
    pst_SI->ul_Flags &= ~(SND_Cul_SF_DynPan);
	pst_SI->ul_EdiFlags |= SND_EdiFlg_EdiUse;

    if(_b_Looped)
        SND_PlaySoundLooping(mst_SModifierDesc.i_Instance, -1);
	else
	    SND_PlaySound(mst_SModifierDesc.i_Instance);
    
	pst_SI = SND_gst_Params.dst_Instance + mst_SModifierDesc.i_Instance;
	if((pst_SMod = SND_pst_SModifierGet(pst_SI->pst_SModifier, SND_Cte_SModifierExtPlayer,0)) != NULL)
	{
		pst_ExtPlay = (SND_tdst_SModifierExtPlayer *) pst_SMod->pv_Data;
	    if(pst_ExtPlay->f_DryVol_FactMax + pst_ExtPlay->f_DryVol_FactMin)
            SND_SetVol(mst_SModifierDesc.i_Instance, pst_ExtPlay->st_Dyn.f_DryVol_Curr);
        else
            SND_SetVol(mst_SModifierDesc.i_Instance, pst_ExtPlay->f_DryVol);
	}

	mpo_View->UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SModifier_OnFindFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	EBRO_cl_Frame	*po_Browser;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_SModifierDesc.ul_Fat == BIG_C_InvalidIndex) return;

	po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
	po_Browser->mpo_MyView->IWantToBeActive(po_Browser);
	po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(mst_SModifierDesc.ul_Fat), mst_SModifierDesc.ul_Fat);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SModifier_UserReport(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem			*pst_CurrentElemW;
	TAB_tdst_PFelem			*pst_EndElemW;
	WOR_tdst_World			*pst_World;
	TAB_tdst_PFtable		*pst_AWO;
	TAB_tdst_PFelem			*pst_CurrentElem;
	TAB_tdst_PFelem			*pst_EndElem;
	OBJ_tdst_GameObject		*pst_GAO;
	MDF_tdst_Modifier		*pst_MdF;
	GEN_tdst_ModifierSound	*pst_SndMdF;
	char					az_msg[1024];
	int						i, j;
	SND_tdst_Bank			*pst_Bank;
	SND_tdst_OneSound		*pst_Sound;
	SND_tdst_SModifierExtPlayer*pst_ExtPlayer;
	ULONG					ul_Fat, ul_Key;
	int						nb;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_SModifierDesc.ul_Fat == BIG_C_InvalidIndex) return;

	AfxGetApp()->DoWaitCursor(1);
	ul_Fat = mst_SModifierDesc.ul_Fat;
	ul_Key = mst_SModifierDesc.ul_Key;

	LINK_PrintStatusMsg("---------------------------------------------");
	LINK_PrintStatusMsg("SModifier user report :");
	LINK_PrintStatusMsg("-----------------------");

	sprintf(az_msg, "Users of sound %s [%08x] are :", BIG_NameFile(ul_Fat), ul_Key);
	LINK_PrintStatusMsg(az_msg);

	/*$2- search in bank ---------------------------------------------------------------------------------------------*/

	for(i = 0; i < (int) SND_gul_MainRefListSize; i++)
	{
		pst_Bank = SND_p_MainGetBank(SND_gap_MainRefList[i]);
		nb = 0;

		if(pst_Bank->pi_SaveBank)
		{
			for(j = 0; j < pst_Bank->i_SoundNb; j++)
			{
				if(pst_Bank->pi_SaveBank[j] == -1) continue;
				if(SND_gst_Params.dst_Sound[pst_Bank->pi_SaveBank[j]].ul_FileKey == ul_Key)
				{
					nb++;
				}
			}
		}
		else
		{
			for(j = 0; j < pst_Bank->i_SoundNb; j++)
			{
				if(SND_gst_Params.dst_Sound[pst_Bank->pi_Bank[j]].ul_FileKey == ul_Key)
				{
					nb++;
				}
			}
		}

		if(nb)
		{
			sprintf
			(
				az_msg,
				"[%08x] x%d %s",
				pst_Bank->ul_FileKey,
				nb,
				BIG_NameFile(BIG_ul_SearchKeyToFat(pst_Bank->ul_FileKey))
			);
			LINK_PrintStatusMsg(az_msg);
		}
	}

	/*$2- search in MdF ----------------------------------------------------------------------------------------------*/

	pst_CurrentElemW = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
	pst_EndElemW = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
	for(pst_CurrentElemW; pst_CurrentElemW <= pst_EndElemW; pst_CurrentElemW++)
	{
		pst_World = (WOR_tdst_World *) pst_CurrentElemW->p_Pointer;

		/*$1- in each world ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(!TAB_b_IsAHole(pst_World))
		{
			pst_AWO = &(pst_World->st_AllWorldObjects);

			pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AWO);
			pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AWO);

			/*$1- register all gao that has one Sound Bank ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
			{
				pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
				if(TAB_b_IsAHole(pst_GAO)) continue;

				if((pst_GAO->pst_Extended) && (pst_GAO->pst_Extended->pst_Modifiers))
				{
					pst_MdF = pst_GAO->pst_Extended->pst_Modifiers;
					while(pst_MdF)
					{
						if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
						{
							pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
							if(pst_SndMdF->ui_FileKey == ul_Key)
							{
								sprintf
								(
									az_msg,
									"[%08x] %s (MdF #%d)",
									LOA_ul_SearchKeyWithAddress((ULONG) pst_GAO),
									pst_GAO->sz_Name,
									pst_SndMdF->ui_Id
								);
								LINK_PrintStatusMsg(az_msg);
							}
						}

						pst_MdF = pst_MdF->pst_Next;
					}
				}
			}
		}
	}

	/*$2- search in SModifier ----------------------------------------------------------------------------------------*/

	for(i = 0; i < SND_gst_Params.l_SoundNumber; i++)
	{
		pst_Sound = SND_gst_Params.dst_Sound + i;

		if((pst_Sound->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
		if((pst_Sound->ul_Flags & SND_Cul_SF_SModifier) == 0) continue;

		
		pst_ExtPlayer = (SND_tdst_SModifierExtPlayer*)(SND_pst_SModifierGet(pst_Sound->pst_SModifier, SND_Cte_SModifierExtPlayer,0))->pv_Data;

		if(pst_ExtPlayer)
		{			
			int nb = 0;

			for(unsigned short us=0; us<pst_ExtPlayer->st_PlayList.us_Size; us++)
			{
				if(pst_ExtPlayer->st_PlayList.aul_KeyList[us] == ul_Key)
				{
					nb++;
				}
			}
			
			if(nb)
			{
				sprintf
				(
					az_msg,
					"[%08x] x%d %s",
					pst_Sound->ul_FileKey,
					nb,
					BIG_NameFile(BIG_ul_SearchKeyToFat(pst_Sound->ul_FileKey))
				);
				LINK_PrintStatusMsg(az_msg);
			}
			
		}
	}

	LINK_PrintStatusMsg("[  none  ] sound editor");
	LINK_PrintStatusMsg("---------------------------------------------");

	AfxGetApp()->DoWaitCursor(-1);
}


/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
