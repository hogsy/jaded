/*$T SNDbank.c GC 1.138 12/04/03 17:58:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGread.h"

#include "BASe/MEMory/MEM.h"

#include "../Main/WinEditors/Sources/EDIpaths.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"

#include "SouND/Sources/SNDbank.h"
#include "SouND/Sources/SNDdialog.h"
#include "SouND/Sources/SNDambience.h"
#include "SouND/Sources/SNDmusic.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "SouND/Sources/SNDmodifier.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDmacros.h"
#include "SouND/Sources/SNDstream.h"

#include "Editors/Sources/SOuNd/SONutil.h"

#ifdef ACTIVE_EDITORS
#include "LINks/LINKtoed.h"
#include "EDItors/Sources/SOuNd/SONmsg.h"
#endif
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "INOut/INO.h"

#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEN.h"

#ifdef _GAMECUBE
extern int L_strnicmp(const char *pz1, const char *pz2, int size);
#endif

/*$4
 ***********************************************************************************************************************
    MACROS
 ***********************************************************************************************************************
 */

#define M_CheckGaoForMerge(_pst_GOSrc) \
	{ \
		if(!_pst_GOSrc) \
		{ \
			ERR_X_Warning(0, "[SND(merge-bank)] the gao is null", NULL); \
			return; \
		} \
		if(!OBJ_b_TestIdentityFlag(_pst_GOSrc, OBJ_C_IdentityFlag_Sound)) \
		{ \
			ERR_X_Warning(0, "[SND(merge-bank)] the gao has no sound identity flag", _pst_GOSrc->sz_Name); \
			return; \
		} \
		if(!_pst_GOSrc->pst_Extended) \
		{ \
			ERR_X_Warning(0, "[SND(merge-bank)] the gao has no extented struct", _pst_GOSrc->sz_Name); \
			return; \
		} \
		if(!_pst_GOSrc->pst_Extended->pst_Sound) \
		{ \
			ERR_X_Warning(0, "[SND(merge-bank)] the gao has no sound struct", _pst_GOSrc->sz_Name); \
			return; \
		} \
		if(!_pst_GOSrc->pst_Extended->pst_Sound->st_Bank.pi_Bank) \
		{ \
			ERR_X_Warning(0, "[SND(merge-bank)] the gao has no sound bank", _pst_GOSrc->sz_Name); \
			return; \
		} \
	}

/*$4
 ***********************************************************************************************************************
    EXTERN/STATIC
 ***********************************************************************************************************************
 */

extern int	TEXT_i_GetLang(void);
extern void BIG_ReadNoSeek(ULONG _ul_Pos, void *_p_Buffer, ULONG _ul_Length);
static void SND_MetaBankReloadAll(void);

/*$4
 ***********************************************************************************************************************
    FONCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG SND_ul_CallbackUnknownBank(ULONG _ul_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ret;
	BIG_tdst_GroupElem	st_Elem;
	ULONG				ul_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!LOA_IsBinaryData())
	{
		BIG_ReadNoSeek(_ul_Pos + 4, (void *) &st_Elem, sizeof(BIG_tdst_GroupElem));
		ul_Type = st_Elem.ul_Type;
	}

#ifdef ACTIVE_EDITORS
	if(LOA_IsBinarizing())
	{
		/*~~~~~~~~~~~~*/
		char	*pc_Buf;
		/*~~~~~~~~~~~~*/

		pc_Buf = (char *) &st_Elem.ul_Type;
		LOA_FetchBuffer(0);
		LOA_ReadCharArray(&pc_Buf, NULL, sizeof(ULONG));
	}
#endif
	if(LOA_IsBinaryData())
	{
		/*~~~~~~~~~~~~*/
		char	*pc_Buf;
		/*~~~~~~~~~~~~*/

		pc_Buf = LOA_FetchBuffer(sizeof(ULONG));
		LOA_ReadCharArray(&pc_Buf, (char *) &ul_Type, sizeof(ULONG));
	}

	if(L_strnicmp((char *) &ul_Type, EDI_Csz_ExtSoundBank, 4) == 0)
		ret = SND_ul_CallbackLoadMetabank(_ul_Pos);
	else
		ret = SND_ul_CallbackLoadBank(_ul_Pos);

	return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_FreeMainStruct(OBJ_tdst_GameObject *_pst_GO)
{
	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Sound)) return;
	if(!SND_M_GoodPointer(_pst_GO->pst_Extended)) return;
	if(!SND_M_GoodPointer(_pst_GO->pst_Extended->pst_Sound)) return;

	SND_MainFreeUnion(_pst_GO->pst_Extended->pst_Sound);
	_pst_GO->pst_Extended->pst_Sound = NULL;
}

void SND_MainFreeUnion(SND_tdun_Main *_pMain)
{
	if(_pMain->st_Bank.ul_Flags & SND_Cte_Bank)
		SND_BankUnload(&_pMain->st_Bank);
	else if(_pMain->st_Bank.ul_Flags & SND_Cte_Metabank)
		SND_MetabankUnload(&_pMain->st_Metabank);
	else
    {
        if(--_pMain->st_Bank.ul_UserCount) return;
        MEM_Free(_pMain);
    }
}

SND_tdst_Bank* SND_p_MainGetBank(SND_tdun_Main *_pMain)
{
	if(_pMain->st_Bank.ul_Flags & SND_Cte_Bank)
		return &_pMain->st_Bank;
	else if(_pMain->st_Bank.ul_Flags & SND_Cte_Metabank)
		return _pMain->st_Metabank.pst_Curr;
	else
		return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#define L_EXT(d, c, b, a)	((a<<24) | (b<<16) | (c<<8) | (d<<0))

ULONG SND_ul_CallbackLoadBank(ULONG _ul_FilePos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ulSize;
	SND_tdst_Bank		*pBank;
	char				*pBuff;
	int					i;
	BIG_tdst_GroupElem	stGroup;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1- check yet loaded ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pBank = (SND_tdst_Bank *) LOA_ul_SearchAddress(_ul_FilePos);
	if((ULONG) pBank != -1)
	{
		pBank->ul_UserCount++;
		return(ULONG) pBank;
	}

	/*$1- read file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pBuff = BIG_pc_ReadFileTmp(_ul_FilePos, &ulSize);
	if(!ulSize) return 0;

	/*$1- alloc space ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pBank = (SND_tdst_Bank *) MEM_p_Alloc(sizeof(SND_tdst_Bank));
	L_memset(pBank, 0, sizeof(SND_tdst_Bank));

	/*$1- fill in + make ref ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pBank->ul_UserCount = 1;
	pBank->ul_Flags = SND_Cte_Bank;
	pBank->ul_FileKey = LOA_ul_GetCurrentKey();
	pBank->i_SoundNb = ulSize / sizeof(BIG_tdst_GroupElem);

	pBank->pi_Bank = (int *)MEM_p_Alloc(pBank->i_SoundNb * sizeof(int));
	L_memset(pBank->pi_Bank, 0, pBank->i_SoundNb * sizeof(int));
	for(i = 0; i < pBank->i_SoundNb; ++i)
	{
		stGroup.ul_Key = LOA_ReadULong(&pBuff);
		stGroup.ul_Type = LOA_ReadULong(&pBuff);

		if
		(
			(stGroup.ul_Key == BIG_C_InvalidKey)
		||	(stGroup.ul_Key == 0)
		||	(BIG_ul_SearchKeyToPos(stGroup.ul_Key) == -1)
		)
		{

#ifdef ACTIVE_EDITORS
            if( (stGroup.ul_Key != BIG_C_InvalidKey) &&	(stGroup.ul_Key != 0) )
            {
                char log[256];
                sprintf(log, "[SND] the sound bank %s [%x] contains a bad key : %x (rank %d)", BIG_NameFile(BIG_ul_SearchKeyToFat(pBank->ul_FileKey)), pBank->ul_FileKey, stGroup.ul_Key, i);
                ERR_X_Warning(BIG_ul_SearchKeyToPos(stGroup.ul_Key) != -1, log, NULL);
            }
#endif

			pBank->pi_Bank[i] = -1;
		}
		else
		{
			switch(stGroup.ul_Type)
			{
				case L_EXT('.','w','a','d'):	
				pBank->pi_Bank[i] = SND_l_AddDialog(stGroup.ul_Key);	break;
				
				case L_EXT('.','w','a','m'):		
				pBank->pi_Bank[i] = SND_l_AddMusic(stGroup.ul_Key);		break;
				
				case L_EXT('.','w','a','a'):	
				pBank->pi_Bank[i] = SND_l_AddAmbience(stGroup.ul_Key);	break;
				
				case L_EXT('.','s','m','d'):		
				pBank->pi_Bank[i] = SND_l_AddSModifier(stGroup.ul_Key);	break;
				
				case L_EXT('.','w','a','c'):	
				pBank->pi_Bank[i] = SND_l_AddLoadingSound(stGroup.ul_Key);		break;
				
				case L_EXT('.','w','a','v'):		
				pBank->pi_Bank[i] = SND_l_AddSound(stGroup.ul_Key);		break;
				
				default:
					pBank->pi_Bank[i] = -1;
					SND_EdiRedWarningMsgKey
					(
						"SND_ul_CallbackLoadBank",
						"Bad file type in the sound bank :",
						pBank->ul_FileKey
					);
					break;
			}			
		}
	}

	ESON_BankRegister((SND_tdun_Main*)pBank);
	return(ULONG) pBank;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_BankUnload(SND_tdst_Bank *pBank)
{
	/*~~*/
	int i;
	/*~~*/

	if(!pBank) return;
	if(--pBank->ul_UserCount) return;

	if(pBank->pi_SaveBank)
	{
		SND_BankRestore(pBank);
	}

	LOA_DeleteAddress(pBank);
	ESON_BankDelete((SND_tdun_Main*)pBank);

	for(i = 0; i < pBank->i_SoundNb; i++)
	{
		SND_DeleteByIndex(pBank->pi_Bank[i]);
		pBank->pi_Bank[i] = -1;
	}

	if(pBank->pi_Bank) MEM_Free(pBank->pi_Bank);
	pBank->pi_Bank = NULL;

	MEM_Free(pBank);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_MergeBank
(
	OBJ_tdst_GameObject *_pst_GODst,
	OBJ_tdst_GameObject *_pst_GOSrc,
	int					_i_Begin,
	int					_i_End,
	int					_i_Overwrite
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Bank		*pst_Src, *pst_Dst;
	int					i;
	SND_tdst_OneSound	*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_Begin < 0)
	{
		ERR_X_Warning(0, "[SND(merge-bank)] index is negative", NULL);
		return;
	}

	M_CheckGaoForMerge(_pst_GOSrc);
	M_CheckGaoForMerge(_pst_GODst);

	if((_pst_GOSrc->pst_Extended->pst_Sound->st_Bank.ul_Flags & SND_Cte_Bank) == 0) return;
	if((_pst_GODst->pst_Extended->pst_Sound->st_Bank.ul_Flags & SND_Cte_Bank) == 0) return;

	pst_Src = &_pst_GOSrc->pst_Extended->pst_Sound->st_Bank;
	pst_Dst = &_pst_GODst->pst_Extended->pst_Sound->st_Bank;

	if(_i_End == -1)
	{
		_i_End = (pst_Src->i_SoundNb > pst_Dst->i_SoundNb) ? pst_Dst->i_SoundNb : pst_Src->i_SoundNb;
		_i_End--;	/* index go from 0 to n-1 */
	}

	if(_i_End >= pst_Src->i_SoundNb)
	{
		ERR_X_Warning(0, "[SND(merge-bank)] End index is too big", NULL);
		return;
	}

	if(_i_End >= pst_Dst->i_SoundNb)
	{
		ERR_X_Warning(0, "[SND(merge-bank)] End index is too big", NULL);
		return;
	}

	if(!pst_Dst->pi_SaveBank)
	{
		/* 1st merge */
		pst_Dst->pi_SaveBank = pst_Dst->pi_Bank;
		pst_Dst->pi_Bank = (int *)MEM_p_Alloc(pst_Dst->i_SoundNb * sizeof(int));
		L_memcpy(pst_Dst->pi_Bank, pst_Dst->pi_SaveBank, pst_Dst->i_SoundNb * sizeof(int));
	}

	for(i = _i_Begin; i <= _i_End; i++)
	{
		if(pst_Dst->pi_Bank[i] == pst_Src->pi_Bank[i]) continue;
		if(pst_Src->pi_Bank[i] == -1) continue;

		if((pst_Dst->pi_Bank[i] == -1) || _i_Overwrite)
		{
			/* add one user */
			pst_Sound = SND_gst_Params.dst_Sound + pst_Src->pi_Bank[i];
			if(pst_Sound->ul_Flags & SND_Cul_DSF_Used) pst_Sound->ul_CptUsed++;

			/* free previous result of merge */
			if(pst_Dst->pi_Bank[i] != pst_Dst->pi_SaveBank[i]) SND_DeleteByIndex(pst_Dst->pi_Bank[i]);

			/* do merge */
			pst_Dst->pi_Bank[i] = pst_Src->pi_Bank[i];
		}
	}

#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~*/
		ULONG	ul_Fat;
		ULONG	ul_Key;
		/*~~~~~~~~~~~*/

		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Dst);
		if(ul_Key != BIG_C_InvalidIndex)
		{
			ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
			if(ul_Fat != -1)
			{
				LINK_SendMessageToEditors(ESON_MESSAGE_REFRESH_BANK, ul_Fat, 0);
			}
		}
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_RestoreGaoBank(OBJ_tdst_GameObject *_pst_GO)
{
	M_CheckGaoForMerge(_pst_GO);
	if((_pst_GO->pst_Extended->pst_Sound->st_Bank.ul_Flags & SND_Cte_Bank) == 0) return;
	SND_BankRestore(&_pst_GO->pst_Extended->pst_Sound->st_Bank);
}

void SND_BankRestore(SND_tdst_Bank	*pst_Bank)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_Bank->pi_SaveBank)
	{
		for(i = 0; i < pst_Bank->i_SoundNb; i++)
		{
			if(pst_Bank->pi_Bank[i] == pst_Bank->pi_SaveBank[i]) continue;
			if(pst_Bank->pi_Bank[i] == -1) continue;
			SND_DeleteByIndex(pst_Bank->pi_Bank[i]);
		}

		MEM_Free(pst_Bank->pi_Bank);
		pst_Bank->pi_Bank = pst_Bank->pi_SaveBank;
		pst_Bank->pi_SaveBank = NULL;
#ifdef ACTIVE_EDITORS
		{
			/*~~~~~~~~~~~*/
			ULONG	ul_Fat;
			ULONG	ul_Key;
			/*~~~~~~~~~~~*/

			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Bank);
			if(ul_Key != BIG_C_InvalidIndex)
			{
				ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
				if(ul_Fat != -1)
				{
					LINK_SendMessageToEditors(ESON_MESSAGE_REFRESH_BANK, ul_Fat, 0);
				}
			}
		}
#endif
	}
}

/*$4
 ***********************************************************************************************************************
    META BANK
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG SND_ul_CallbackLoadMetabank(ULONG _ul_FilePos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ulSize;
	SND_tdst_Metabank	*pMetabank;
	char				*pBuff;
	int					i;
	BIG_tdst_GroupElem	stGroup;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1- check yet loaded ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pMetabank = (SND_tdst_Metabank *) LOA_ul_SearchAddress(_ul_FilePos);
	if((ULONG) pMetabank != -1)
	{
		pMetabank->ul_UserCount++;
		return(ULONG) pMetabank;
	}

	/*$1- read file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pBuff = BIG_pc_ReadFileTmp(_ul_FilePos, &ulSize);

	/*$1- alloc space ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pMetabank = (SND_tdst_Metabank	*)MEM_p_Alloc(sizeof(SND_tdst_Metabank));
	L_memset(pMetabank, 0, sizeof(SND_tdst_Metabank));

	/*$1- fill in + make ref ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pMetabank->ul_UserCount = 1;
	pMetabank->ul_Flags = SND_Cte_Metabank;
	pMetabank->ul_FileKey = LOA_ul_GetCurrentKey();

	pMetabank->dst_BankKey = (ULONG *) MEM_p_Alloc(INO_e_MaxLangNb * sizeof(ULONG));
	L_memset(pMetabank->dst_BankKey, -1, INO_e_MaxLangNb * sizeof(ULONG));

	for(i = 0; i < (int) (ulSize / sizeof(BIG_tdst_GroupElem)); i++)
	{
		stGroup.ul_Key = LOA_ReadULong(&pBuff);
		stGroup.ul_Type = LOA_ReadULong(&pBuff);

		if
		(
			(stGroup.ul_Key == BIG_C_InvalidKey)
		||	(stGroup.ul_Key == 0)
		||	(BIG_ul_SearchKeyToPos(stGroup.ul_Key) == -1)
		)
		{
			pMetabank->dst_BankKey[i] = -1;
		}
		else
		{
			pMetabank->dst_BankKey[i] = stGroup.ul_Key;
		}
	}

	i = TEXT_i_GetLang();
	if(pMetabank->dst_BankKey[i] == -1)
	{
		SND_EdiRedWarningMsgKey
		(
			"SND_ul_CallbackLoadMetaBank",
			"The metabank contains no translation for the current language:",
			pMetabank->ul_FileKey
		);
	}
	else
	{
		LOA_MakeFileRef
		(
			pMetabank->dst_BankKey[i],
			(ULONG *) &pMetabank->pst_Curr,
			SND_ul_CallbackLoadBank,
			LOA_C_MustExists|LOA_C_HasUserCounter
		);
	}

	/*$1- register the metabank pointer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < SND_gst_Params.l_MetabankNumber; i++)
	{
		if(SND_gst_Params.dp_Metabank[i] == NULL) continue;
	}

	if(i == SND_gst_Params.l_MetabankNumber)
	{
		SND_gst_Params.l_MetabankNumber += 20;
		SND_gst_Params.dp_Metabank = (SND_tdst_Metabank	**)MEM_p_Realloc
			(
				SND_gst_Params.dp_Metabank,
				SND_gst_Params.l_MetabankNumber * sizeof(SND_tdst_Metabank *)
			);
		L_memset(&SND_gst_Params.dp_Metabank[i], 0, 20 * sizeof(SND_tdst_Metabank *));
	}

	SND_gst_Params.dp_Metabank[i] = pMetabank;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	return(ULONG) pMetabank;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_MetabankUnload(SND_tdst_Metabank *pMetabank)
{
	/*~~*/
	int i;
	/*~~*/

	if(!pMetabank) return;
	if(--pMetabank->ul_UserCount) return;

	/*$1- delete the pointer ref ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LOA_DeleteAddress(pMetabank);

	for(i = 0; i < SND_gst_Params.l_MetabankNumber; i++)
	{
		if(SND_gst_Params.dp_Metabank[i] != pMetabank) continue;
		SND_gst_Params.dp_Metabank[i] = NULL;
		break;
	}

	/*$1- free all memory ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pMetabank->pst_Curr) SND_BankUnload(pMetabank->pst_Curr);
	pMetabank->pst_Curr = NULL;

	if(pMetabank->dst_BankKey) MEM_Free(pMetabank->dst_BankKey);
	pMetabank->dst_BankKey = NULL;

	MEM_Free(pMetabank);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_UpdateLang(void)
{
	SND_StreamPrefetchFlushAll();
	SND_MetaBankReloadAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_MetaBankReloadAll(void)
{
	/*~~~~~*/
	int i, j;
	/*~~~~~*/

	if(!SND_gst_Params.l_Available) return;

	j = TEXT_i_GetLang();
	for(i = 0; i < SND_gst_Params.l_MetabankNumber; i++)
	{
		if(!SND_gst_Params.dp_Metabank[i]) continue;

		SND_BankUnload(SND_gst_Params.dp_Metabank[i]->pst_Curr);
		SND_gst_Params.dp_Metabank[i]->pst_Curr = NULL;

		if(SND_gst_Params.dp_Metabank[i]->dst_BankKey[j] == -1)
		{
			SND_EdiRedWarningMsgKey
			(
				"SND_MetaBankReloadAll",
				"The metabank has no translation for the selected language",
				SND_gst_Params.dp_Metabank[i]->ul_FileKey
			);
		}
		else
		{
			LOA_MakeFileRef
			(
				SND_gst_Params.dp_Metabank[i]->dst_BankKey[j],
				(ULONG *) &SND_gst_Params.dp_Metabank[i]->pst_Curr,
				SND_ul_CallbackLoadBank,
				LOA_C_MustExists|LOA_C_HasUserCounter
			);
			LOA_Resolve();
		}
	}
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
