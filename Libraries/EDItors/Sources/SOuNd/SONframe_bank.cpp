/*$T SONframe_bank.cpp GC 1.138 06/20/03 10:54:58 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS

/*$2- editor ---------------------------------------------------------------------------------------------------------*/

#include "EDIpaths.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAsndtrans_dlg.h"
#include "DIAlogs/DIAfindfile_dlg.h"
#include "DIAlogs/DIA_UPDATE_dlg.h"
#include "BROwser/BROframe.h"
#include "EDImsg.h"

/*$2- engine ---------------------------------------------------------------------------------------------------------*/

#include "BIGfiles/BIGexport.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "WORld/WORvars.h"
#include "LINKs/LINKstruct_reg.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_GEN.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "SouND/sources/SND.h"
#include "SouND/sources/SNDmacros.h"
#include "SouND/sources/SNDwave.h"
#include "SouND/sources/SNDconst.h"
#include "SouND/sources/SNDtrack.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/sources/SNDmodifier.h"
#include "SouND/sources/SNDdialog.h"
#include "SouND/sources/SNDambience.h"
#include "SouND/sources/SNDmusic.h"
#include "SouND/sources/SNDloadingsound.h"
#include "SouND/sources/SNDconv_xboxadpcm.h"
#include "SouND/sources/SNDrasters.h"
#include "SouND/sources/SNDbank.h"

/*$2- sound editor ---------------------------------------------------------------------------------------------------*/

#include "SONframe.h"
#include "SONview.h"
#include "SONmsg.h"
#include "SONutil.h"

/*$4
 ***********************************************************************************************************************
    extern prototypes
 ***********************************************************************************************************************
 */

extern void		BRO_OrderGroupFile(ULONG _ul_Group);

#ifdef JADEFUSION
extern LONG SND_l_GetLoadingSound(void);
extern BOOL BIG_b_CheckUserAbortCommand(void);
#else
extern "C" LONG SND_l_GetLoadingSound(void);
extern "C" BOOL BIG_b_CheckUserAbortCommand(void);
#endif
/*$4
 ***********************************************************************************************************************
    extern variables
 ***********************************************************************************************************************
 */

EDIA_cl_SndTranslate	go_Dialog;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_CB_ModifyMicroPos(void *p1, void *p2, void *p_NewValue, LONG l_OldValue)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(*(LONG *) p_NewValue != l_OldValue)
	{
		pst_GO = *(OBJ_tdst_GameObject **) p_NewValue;
		SND_gst_Params.pst_RefForVol = pst_GO->pst_GlobalMatrix;
		SND_gst_Params.ul_Flags &= ~SND_Cte_Freeze3DVol;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::UpdateVarView_Bank(BOOL bForce)
{

    if(!bForce && (mst_SndBankDesc.ul_Fat == BIG_C_InvalidIndex)) return;

	M_MF()->LockDisplay(mpo_VarsView0);
	mpo_VarsView0->ResetList();

		if(SND_gst_Params.l_Available && (mst_SndBankDesc.ul_Fat != BIG_C_InvalidIndex))
		{
			mpo_VarsView0->AddItem
				(
					"Bank",
					EVAV_EVVIT_SubStruct,
					mst_SndBankDesc.pst_CurrentBank,
					EVAV_ReadOnly | EVAV_AutoExpand,
					LINK_C_SND_Bank,
					0,
					sizeof(SND_tdun_Main)
				);
		}
	mpo_VarsView0->SetItemList(&mo_ListItems0);
	M_MF()->UnlockDisplay(mpo_VarsView0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SoundBank_Close(void)
{
	if(mst_SndBankDesc.ul_Key == BIG_C_InvalidKey) return;

	SND_MainFreeUnion(mst_SndBankDesc.pst_CurrentBank);
	mst_SndBankDesc.ul_Key = BIG_C_InvalidKey;
	mst_SndBankDesc.ul_Fat = BIG_C_InvalidIndex;
	mst_SndBankDesc.pst_CurrentBank = NULL;
	UpdateVarView_Bank(TRUE);
    mpo_View->UpdateControls();
	RefreshMenu();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SoundBank_Modify(void)
{
	if(mst_SndBankDesc.ul_Key == BIG_C_InvalidKey) return;
	if(mst_SndBankDesc.ul_Fat == BIG_C_InvalidIndex) return;
	BRO_OrderGroupFile(mst_SndBankDesc.ul_Fat);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SoundBank_ForceReload(void)
{
	/*~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Save;
	/*~~~~~~~~~~~~~~~~*/

	ul_Save = mst_SndBankDesc.ul_Fat;
	SoundBank_Close();
	SoundBank_Set(ul_Save, TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SoundBank_Set(BIG_INDEX _ul_Fat, BOOL _b_Update)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Key;
	int		i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* check if loaded */
	ul_Key = LOA_ul_SearchAddress(BIG_PosFile(_ul_Fat));

	/* if it's the same do nothing */
	if(ul_Key == (ULONG) mst_SndBankDesc.pst_CurrentBank) return;

	/* else free the previous one */
	if(mst_SndBankDesc.ul_Key != BIG_C_InvalidKey) SND_MainFreeUnion(mst_SndBankDesc.pst_CurrentBank);

	/* get the new one */
	mst_SndBankDesc.ul_Fat = _ul_Fat;
	mst_SndBankDesc.ul_Key = BIG_FileKey(_ul_Fat);
	M_MF()->AddHistoryFile(this, mst_SndBankDesc.ul_Key);
	BIG_ComputeFullName(BIG_ParentFile(_ul_Fat), mst_SndBankDesc.sz_Path);

	/* if doesn't loaded */
	LOA_MakeFileRef
	(
		BIG_FileKey(_ul_Fat),
		(ULONG *) &mst_SndBankDesc.pst_CurrentBank,
		SND_ul_CallbackLoadBank,
		LOA_C_MustExists|LOA_C_HasUserCounter
	);
	LOA_Resolve();
	ESON_LoadAllSounds();
    UpdateVarView_Bank();		
	
	RefreshMenu();

	if(_b_Update)
	{
		mpo_View->FillListBank();
		mpo_View->FillListSound();
		for(i = 0; i < (int)SND_gul_MainRefListSize; i++)
		{
			if(SND_gap_MainRefList[i] == mst_SndBankDesc.pst_CurrentBank) break;
		}

		mpo_View->SetCurSel(IDC_LIST_BANK, i, TRUE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SoundBank_UserReport(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem			*pst_CurrentElemW;
	TAB_tdst_PFelem			*pst_EndElemW;
	WOR_tdst_World			*pst_World;
	TAB_tdst_PFtable		*pst_AWO;
	TAB_tdst_PFelem			*pst_CurrentElem;
	TAB_tdst_PFelem			*pst_EndElem;
	OBJ_tdst_GameObject		*pst_GAO;
	CList<ULONG, ULONG>		o_GaoList;
	POSITION				pos;
	char					az_msg[1024];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_SndBankDesc.ul_Fat == BIG_C_InvalidIndex) return;
	if(mst_SndBankDesc.ul_Key == BIG_C_InvalidKey) return;
	if(!mst_SndBankDesc.pst_CurrentBank) return;

	LINK_PrintStatusMsg("---------------------------------------------");
	LINK_PrintStatusMsg("Bank user report :");
	LINK_PrintStatusMsg("------------------");

	sprintf
	(
		az_msg,
		"Users of sound bank %s [%08x] are :",
		BIG_NameFile(mst_SndBankDesc.ul_Fat),
		mst_SndBankDesc.ul_Key
	);
	LINK_PrintStatusMsg(az_msg);

	/*$1- current universe ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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

			for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
			{
				pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
				if(TAB_b_IsAHole(pst_GAO)) continue;

				/*$1- register all gao that has one Sound Bank ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if(pst_GAO->pst_Extended && pst_GAO->pst_Extended->pst_Sound)
				{
					if(pst_GAO->pst_Extended->pst_Sound == mst_SndBankDesc.pst_CurrentBank)
						o_GaoList.AddTail((ULONG) pst_GAO);
				}

			}
		}
	}

	/*$1- then scan all bank ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pos = o_GaoList.GetHeadPosition();
	while(pos)
	{
		pst_GAO = (OBJ_tdst_GameObject *) o_GaoList.GetNext(pos);
		sprintf(az_msg, "[%08x] %s", LOA_ul_SearchKeyWithAddress((ULONG) pst_GAO), pst_GAO->sz_Name);
		LINK_PrintStatusMsg(az_msg);
	}

	LINK_PrintStatusMsg("[  none  ] sound editor");
	LINK_PrintStatusMsg("---------------------------------------------");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SoundBank_ContentsReport(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	SND_tdst_OneSound	*pst_Sound, *pst_SndModifier;
	char				az_msg[1024];
	SND_tdst_Bank*pBank;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_SndBankDesc.ul_Fat == BIG_C_InvalidIndex) return;
	if(mst_SndBankDesc.ul_Key == BIG_C_InvalidKey) return;
	if(!mst_SndBankDesc.pst_CurrentBank) return;

	LINK_PrintStatusMsg("---------------------------------------------");
	LINK_PrintStatusMsg("Bank contents report :");
	LINK_PrintStatusMsg("----------------------");

	pBank = SND_p_MainGetBank(mst_SndBankDesc.pst_CurrentBank);
	sprintf
	(
		az_msg,
		"Contents of sound bank %s [%08x] are :",
		BIG_NameFile(mst_SndBankDesc.ul_Fat),
		mst_SndBankDesc.ul_Key
	);
	LINK_PrintStatusMsg(az_msg);

	for(i = 0; i < pBank->i_SoundNb; i++)
	{
		if(pBank->pi_Bank[i] < 0) continue;

		pst_Sound = SND_gst_Params.dst_Sound + pBank->pi_Bank[i];
		pst_SndModifier = NULL;

		if((pst_Sound->ul_Flags & SND_Cul_DSF_Used) == 0) continue;


		if(pst_Sound->pst_SModifier && ((int) pst_Sound->pst_DSB >= 0))
		{
			pst_SndModifier = pst_Sound;
			pst_Sound = SND_gst_Params.dst_Sound + (int) pst_Sound->pst_DSB;
			if((pst_Sound->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
		}
		else
		{
			continue;
		}

		if(pst_Sound->ul_FileKey == -1)
		{
			ERR_X_Warning(0, "[SND(SoundReport)] Bad key in dst_Sound array", NULL);
		}
		else
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/
			unsigned int	ui_DataSize;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM)
				ui_DataSize = SND_ui_GetDecompressedSize(pst_Sound->pst_Wave->ul_DataSize);
			else
				ui_DataSize = pst_Sound->pst_Wave->ul_DataSize;

			if(pst_SndModifier)
			{
				sprintf
				(
					az_msg,
					"[%08x] %9d oct, %6d Hz, %2d Users, File %s (from %s)",
					pst_Sound->ul_FileKey,
					ui_DataSize,
					pst_Sound->pst_Wave->dwSamplesPerSec,
					pst_Sound->ul_CptUsed,
					BIG_NameFile(BIG_ul_SearchKeyToFat(pst_Sound->ul_FileKey)),
					BIG_NameFile(BIG_ul_SearchKeyToFat(pst_SndModifier->ul_FileKey))
				);
			}
			else
			{
				sprintf
				(
					az_msg,
					"[%08x] %9d oct, %6d Hz, %2d Users, File %s",
					pst_Sound->ul_FileKey,
					ui_DataSize,
					pst_Sound->pst_Wave->dwSamplesPerSec,
					pst_Sound->ul_CptUsed,
					BIG_NameFile(BIG_ul_SearchKeyToFat(pst_Sound->ul_FileKey))
				);
			}

			LINK_PrintStatusMsg(az_msg);
		}
	}

	LINK_PrintStatusMsg("---------------------------------------------");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::SoundBank_OnFindFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	EBRO_cl_Frame	*po_Browser;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_SndBankDesc.ul_Fat == BIG_C_InvalidIndex) return;

	po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
	po_Browser->mpo_MyView->IWantToBeActive(po_Browser);
	po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(mst_SndBankDesc.ul_Fat), mst_SndBankDesc.ul_Fat);
}

char	*asz_SnkSuffix[] = { "_FR", "_US", "_DE", "_ES", "_IT", "_NL", "_PO", "_FI", "_SE", "_NO", "_DK" };

char	asz_HlpPrefix[] = "FEAGINPLSOK";

char	*asz_Dir[] =
{
	"France",
	"US",
	"Germany",
	"Spain",
	"Italy",
	"Netherlands",
	"Poland",
	"Finland",
	"Sweden",
	"Norway",
	"Denmark"
};

#define C_ReferenceIdx	1

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static ULONG ul_FindFile(char *asz_NewSnkName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FindFileDialog	o_Find;
	static ULONG			ulfat = -1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ulfat == -1) ulfat = BIG_ul_SearchDir(EDI_Csz_Path_Audio);

	o_Find.mo_Name = CString(asz_NewSnkName);
	o_Find.mul_Dir = ulfat; /* BIG_Root(); */
	o_Find.mul_File = BIG_FirstFile(o_Find.mul_Dir);
	o_Find.mb_FindDir = FALSE;

	if(o_Find.RecurseFind(o_Find.mul_Dir, 0))
		return o_Find.mul_File;
	else
		return -1;
}


/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
