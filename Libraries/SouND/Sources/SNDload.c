/*$T SNDload.c GC 1.138 12/05/03 11:45:45 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

/*$1- general ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BAStypes.h"
#include "LINks/LINKmsg.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/IMPort/IMPbase.h"
#include "ENGine/Sources/DYNamics/DYNaccess.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "AIinterp/Sources/AIengine.h"
#include "../Main/WinEditors/Sources/EDIpaths.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEN.h"
#include "INOut/INO.h"

/*$1- sound module headers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"

#include "SouND/Sources/SNDerrid.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SNDdirect.h"
#include "SouND/Sources/SNDdialog.h"
#include "SouND/Sources/SNDmusic.h"
#include "SouND/Sources/SNDAmbience.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SNDmodifier.h"
#include "SouND/Sources/SNDrasters.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDmacros.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDdebug.h"
#include "SouND/Sources/SNDbank.h"

#ifdef ACTIVE_EDITORS
#include "SouND/Sources/SNDconv.h"
#include "LINks/LINKtoed.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "EDItors/Sources/SOuNd/SONmsg.h"
#endif
#include "EDItors/Sources/SOuNd/SONutil.h"

#ifdef PSX2_TARGET
#include "IOP/RPC_Manager.h"
#endif
#ifdef _GAMECUBE
int L_strnicmp(const char *pz1, const char *pz2, int size);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int L_strnicmp(const char *pz1, const char *pz2, int size)
{
	/*~~~~~~*/
	int		i;
	char	d;
	/*~~~~~~*/

	if(!pz1) return -1;
	if(!pz2) return 1;

	for(i = 0; i < size; i++)
	{
		d = L_toupper(pz1[i]) - L_toupper(pz2[i]);

		if(d) return d;
	}

	return 0;
}
#endif

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SND_l_AddSound(BIG_KEY _ul_WavKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				l_Size, l_Index;
	SND_tdst_OneSound	*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- activation -------------------------------------------------------------------------------------------------*/

	if(!SND_gst_Params.l_Available) return -1;
	if((_ul_WavKey == 0) || (_ul_WavKey == BIG_C_InvalidKey)) return -1;
	
	ESON_SetLoadedFile(_ul_WavKey);

	/*$2- Test extension and existence -------------------------------------------------------------------------------*/
	
	ESON_M_ReturnIfFileIsTruncated(_ul_WavKey, -1);
	l_Size = SND_l_GetSoundIndex(_ul_WavKey);
	if(l_Size != -1)
	{
		SND_gst_Params.dst_Sound[l_Size].ul_CptUsed++;
		return l_Size;
	}

	/*$2- Add sound to list ------------------------------------------------------------------------------------------*/

	l_Index = SND_i_AllocSound();
	pst_Sound = SND_gst_Params.dst_Sound + l_Index;

	pst_Sound->ul_FileKey = _ul_WavKey;
	pst_Sound->ul_Flags = SND_Cul_DSF_Used ;
	pst_Sound->ul_ExtFlags = 0;
	pst_Sound->ul_CptUsed = 1;
	pst_Sound->pst_Wave = NULL;
	pst_Sound->pst_DSB = NULL;
	pst_Sound->pst_LI = &SND_gpst_Interface[ISound];
	pst_Sound->pst_SModifier = NULL;

	LOA_MakeFileRef
	(
		_ul_WavKey,
		(ULONG *) &pst_Sound->pst_Wave,
		SND_ul_WaveCallback,
		LOA_C_MustExists|LOA_C_HasUserCounter
	);

	SND_gst_Params.l_Unloaded++;
	return(l_Index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_DeleteByIndex(LONG l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_OneSound	*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!SND_gst_Params.l_Available) return;

    SND_M_GetSoundOrReturn(l_Index, pst_Sound, ;);
	if(--pst_Sound->ul_CptUsed) return;

	SND_DetachSound(l_Index);
	if(SND_b_IsCurrWacSound(l_Index)) return;

	if(pst_Sound->pst_SModifier)
	{
		SND_SModifierDeleteAll(&pst_Sound->pst_SModifier, FALSE);
	}
	else
	{
		if((pst_Sound->ul_Flags & SND_Cul_SF_LoadingSound) == 0)
		{
			pst_Sound->pst_LI->pfv_SndBuffStop(pst_Sound->pst_DSB);
			pst_Sound->pst_LI->pfv_SndBuffRelease(pst_Sound->pst_DSB);
		}
	}

	pst_Sound->ul_FileKey = -1;
	pst_Sound->ul_Flags = 0;
	pst_Sound->ul_CptUsed = 0;
	pst_Sound->pst_DSB = NULL;
	pst_Sound->pst_LI = NULL;
	pst_Sound->pst_SModifier = NULL;
	if(pst_Sound->pst_Wave) SND_WaveUnload(pst_Sound->pst_Wave);
	pst_Sound->pst_Wave = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_DetachSound(LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	LONG					l_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;

	_SND_DebugSpy(SND_gst_Params.dst_Sound[_l_Index].ul_FileKey, SND_DetachSound);

	for(l_Index = 0; l_Index < SND_gst_Params.l_InstanceNumber; l_Index++)
	{
		pst_SI = SND_gst_Params.dst_Instance + l_Index;

		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;
		if(pst_SI->l_Sound != _l_Index) continue;

		if(SND_b_IsCurrWacInstance(l_Index))
		{
			SND_DetachWacInstance();
			continue;
		}

		pst_SI->pst_LI->pfv_SndBuffStop(pst_SI->pst_DSB);
		SND_FreeInstance(l_Index);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SND_l_LoadData(BIG_KEY _ul_WavKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_WaveData		st_WaveFile;
	SND_tdst_SoundBuffer	*pst_SoundBuffer;
	LONG					l_Index;
	SND_tdst_OneSound		*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- activation -------------------------------------------------------------------------------------------------*/

	if(!SND_gst_Params.l_Available) return -1;

	/*$2- Test extension and existence -------------------------------------------------------------------------------*/

	ESON_M_ReturnIfFileIsTruncated(_ul_WavKey, -1);
	l_Index = SND_l_GetSoundIndex(_ul_WavKey);
	if(l_Index == -1) return -1;

	pst_Sound = SND_gst_Params.dst_Sound + l_Index;
	if(pst_Sound->ul_Flags & SND_Cul_OSF_Loaded) return l_Index;
    if(pst_Sound->ul_Flags & (SND_Cul_SF_Dialog|SND_Cul_SF_Music|SND_Cul_SF_Ambience|SND_Cul_SF_LoadingSound|SND_Cul_SF_SModifier)) return -1;

	/*$2- create the sound buffer ------------------------------------------------------------------------------------*/

	st_WaveFile.st_WaveFmtx.wFormatTag = pst_Sound->pst_Wave->wFormatTag;
	st_WaveFile.st_WaveFmtx.nChannels = pst_Sound->pst_Wave->wChannels;
	st_WaveFile.st_WaveFmtx.nSamplesPerSec = pst_Sound->pst_Wave->dwSamplesPerSec;
	st_WaveFile.st_WaveFmtx.nAvgBytesPerSec = pst_Sound->pst_Wave->dwAvgBytesPerSec;
	st_WaveFile.st_WaveFmtx.nBlockAlign = pst_Sound->pst_Wave->wBlockAlign;
	st_WaveFile.st_WaveFmtx.wBitsPerSample = pst_Sound->pst_Wave->wBitsPerSample;
	st_WaveFile.st_WaveFmtx.cbSize = pst_Sound->pst_Wave->cbSize;
	st_WaveFile.i_Size = pst_Sound->pst_Wave->ul_DataSize;
#ifdef JADEFUSION
	st_WaveFile.pbData = (BYTE *) pst_Sound->pst_Wave->ul_DataPosition;
#else
	st_WaveFile.pbData = (void *) pst_Sound->pst_Wave->ul_DataPosition;
#endif
	SND_M_RasterRegisterFile(0);

#if defined(_XENON)
	// Extension to high->low-level sound engine interface, so voices can display their keys (for debugging purposes)

	xeSND_SndBuffSetKey(NULL, _ul_WavKey, pst_Sound->pst_Wave->cSoundBFIndex);
#endif

	pst_SoundBuffer = SND_gpst_Interface[ISound].pfp_SndBuffCreate
		(
			SND_gst_Params.pst_SpecificD,
			&st_WaveFile,
			pst_Sound->pst_Wave->ul_DataPosition
		);
	if(!pst_SoundBuffer) return -1;

	/*$2- register it ------------------------------------------------------------------------------------------------*/

	pst_Sound->ul_Flags |= SND_Cul_OSF_Loaded ;
	pst_Sound->pst_DSB = pst_SoundBuffer;

	return(l_Index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_LoadAllMapSounds(void)
{
	LONG                l_Index;
	SND_tdst_OneSound   *pst_Sound;
    ULONG               ul_Mask;
	
	if(!SND_gst_Params.l_Available)  return;
	
    SND_InsertVarFreeAll();

	SND_gst_Params.ul_Flags &= ~SND_Cte_Freeze3DVol;
	SND_gst_Params.ul_Flags &= ~SND_Cte_FreezeDynPan;

    if(SND_gst_Params.l_Unloaded) 
    {
        ul_Mask = SND_Cul_DSF_Used|SND_Cul_OSF_Loaded|SND_Cul_SF_Dialog|SND_Cul_SF_Music|SND_Cul_SF_Ambience|SND_Cul_SF_LoadingSound|SND_Cul_SF_SModifier;

        LOA_BeginSpeedMode(0xFF400000);
	    for(l_Index=0;l_Index<SND_gst_Params.l_SoundNumber; l_Index++)
	    {
		    pst_Sound = SND_gst_Params.dst_Sound + l_Index;
		    if((pst_Sound->ul_Flags & ul_Mask) != SND_Cul_DSF_Used) continue;
    		
		    SND_l_LoadData(pst_Sound->ul_FileKey);
	    }
	    LOA_EndSpeedMode();
    }
	SND_gst_Params.l_Unloaded = 0;

	SND_ResolveStaticPrefetchForSpecialMdF(); 
	SND_StreamResolveSyncPrefetch();	

	//TODO prévoir une attente synchrone fin de tous les chargements
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_DetachObject(void *_p_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	LONG					l_Index;
	OBJ_tdst_GameObject		*pst_GAGAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	pst_GAGAO = (OBJ_tdst_GameObject *) _p_GO;

#ifdef ACTIVE_EDITORS
	ESON_NotifyGaoIsDeleted((OBJ_tdst_GameObject*)_p_GO);
#endif
	if(!OBJ_b_TestIdentityFlag(pst_GAGAO, OBJ_C_IdentityFlag_Sound)) return;

	for(l_Index = 0; l_Index < SND_gst_Params.l_InstanceNumber; l_Index++)
	{
		pst_SI = SND_gst_Params.dst_Instance + l_Index;

		if((pst_SI->ul_Flags & SND_Cul_DSF_Used) == 0) continue;

		if(pst_SI->p_GameObject == _p_GO)
		{
			if(!OBJ_b_TestIdentityFlag(pst_GAGAO, OBJ_C_IdentityFlag_Sound)) return;
			if(!SND_M_GoodPointer(pst_GAGAO->pst_Extended)) return;
			if(!SND_M_GoodPointer(pst_GAGAO->pst_Extended->pst_Sound)) return;

			pst_SI->pst_LI->pfv_SndBuffStop(pst_SI->pst_DSB);

			SND_FreeInstance(l_Index);
		}
	}

	if
	(
		pst_GAGAO->pst_Extended
	&&	pst_GAGAO->pst_Extended->pst_Sound
	&&	(pst_GAGAO->pst_Extended->pst_Sound->st_Bank.ul_Flags & SND_Cte_Bank)
	)
	{
		if
		(
			(pst_GAGAO->pst_Extended->pst_Sound->st_Bank.ul_UserCount == 1)
		&&	pst_GAGAO->pst_Extended->pst_Sound->st_Bank.pi_SaveBank
		) SND_RestoreGaoBank(pst_GAGAO);
		
	}
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
