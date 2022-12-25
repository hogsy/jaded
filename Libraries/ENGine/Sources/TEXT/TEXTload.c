/*$T TEXTload.c GC 1.138 07/02/04 09:11:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"

#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "TEXT.h"
#include "TEXTload.h"

#include "EDIpaths.h"

#include "INOut/INO.h"

#undef __FILE__
#define __FILE__ "TXTLoad.c"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern BOOL		LOA_gb_SpeedMode;
extern ULONG	LOA_ul_BinKey;
extern ULONG	WOR_gul_WorldTextSize;
extern BIG_KEY	*WOR_gaul_WorldText;
/**/
int				TEXT_gi_ChangedLang = 0;
int				TEXT_gi_ChangingLang = 0;
TEXT_tdst_OneText*TEXT_gp_CallbackLoadTxlForSound = NULL;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

static ULONG	TEXT_ul_CallbackLoadTxt(ULONG ul_Pos, BOOL b_SoundMode);
static ULONG	TEXT_ul_CallbackLoadTxtNoSound(ULONG ul_Pos);
static ULONG	TEXT_ul_CallbackLoadTxtSound(ULONG ul_Pos);
static ULONG	TEXT_ul_CallbackLoadTxg(ULONG ul_Pos);
static void	TEXT_EdiCheckFileType(ULONG ulKey);
static void	TEXT_FreeOneTextIndex(int i);
static int		TEXT_i_AllocOneTextIndex(void);

/**/
extern LONG		SND_l_AddDialog(BIG_KEY _ul_WavKey);
extern void	SND_UpdateLang(void);
extern void	SND_Delete(BIG_KEY);
extern void	STR_3DStringList_Reinit(void);
int			TEXT_i_GetLang(void);
int			TEXT_i_GetSndLang(void);
extern int	AI_EvalFunc_IoConsLangGet_C(void);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS

BOOL TEXT_gb_EdiLoad = FALSE;

ULONG TEXT_ul_CallbackLoadTxlEditor(ULONG _ul_PosFile, ULONG key)
{
    ULONG ul,ulSize,iiSize;
    TEXT_tdst_OneText	*pText, *pTextEdi;
    
    if(_ul_PosFile == -1) return 0;

    TEXT_gb_EdiLoad = TRUE;
    ul = TEXT_ul_CallbackLoadTxl(_ul_PosFile, key);

    TEXT_gp_CallbackLoadTxlForSound = (TEXT_tdst_OneText*)ul;
    TEXT_ul_CallbackLoadTxlForSound(_ul_PosFile, key);
    TEXT_gp_CallbackLoadTxlForSound = NULL;

    TEXT_gb_EdiLoad = FALSE;
    LOA_Resolve(); // purge sound makefileref

    if(!ul) return 0;
    
    pText = (TEXT_tdst_OneText	*)ul;
	pTextEdi = (TEXT_tdst_OneText *) MEM_p_Alloc(sizeof(TEXT_tdst_OneText));
	L_memcpy(pTextEdi, pText, sizeof(TEXT_tdst_OneText));
    
    ulSize = 0;
    if(pText->pst_Ids)
    {
        pTextEdi->pst_Ids = (TEXT_tdst_Id *) MEM_p_Alloc(pTextEdi->ul_Num * sizeof(TEXT_tdst_Id));
	    L_memcpy(pTextEdi->pst_Ids, pText->pst_Ids, pText->ul_Num * sizeof(TEXT_tdst_Id));

        for(ul=0; ul<pText->ul_Num; ul++)
        {
            if(pText->pst_Ids[ul].asz_Comment)
            {
                iiSize = pText->pst_Ids[ul].i_CommentOffset[ 31 ] + L_strlen( pText->pst_Ids[ul].asz_Comment + pText->pst_Ids[ul].i_CommentOffset[31] ) + 1;
                
                pTextEdi->pst_Ids[ul].asz_Comment = (char*)MEM_p_Alloc(iiSize);
                L_memcpy(pTextEdi->pst_Ids[ul].asz_Comment, pText->pst_Ids[ul].asz_Comment, iiSize);
            }

            if(pTextEdi->pst_Ids[ul].i_Offset != -1)
                ulSize += L_strlen(pText->psz_Text + pTextEdi->pst_Ids[ul].i_Offset) + 1;
        }
    }

    if(pText->psz_Text)
    {
        if(ulSize)
        {
            pTextEdi->psz_Text = (char *) MEM_p_Alloc(ulSize);
            L_memcpy(pTextEdi->psz_Text, pText->psz_Text, ulSize);
        }
        else
        {
            pTextEdi->psz_Text = (char *) MEM_p_Alloc(1);
            *pTextEdi->psz_Text = 0;
        }
    }

    pTextEdi->ul_FatherKey = BIG_C_InvalidKey;
    pTextEdi->ul_Inst = 1;

    TEXT_Free(pText);

    return (ULONG)pTextEdi;
}
#endif

ULONG TEXT_ul_CallbackLoadTxl(ULONG _ul_PosFile, ULONG key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CHAR				*pc_Buf;
	ULONG				ul_Size;
	TEXT_tdst_OneText	*pText;
	TEXT_tdst_Id		*p;
	ULONG				i;
	ULONG				ulComment;
	int					i_Priority;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_PosFile == -1) return 0;
	

#ifdef ACTIVE_EDITORS
    if(!TEXT_gb_EdiLoad)
#endif
    {
	    /* yet loaded ? */
	    i = LOA_ul_SearchAddress(_ul_PosFile);
	    if(i != -1)
	    {
            pText = (TEXT_tdst_OneText*)i;
		    pText->ul_Inst++;
		    return(ULONG) pText;
	    }
    }

	pText = (TEXT_tdst_OneText *) MEM_p_Alloc(sizeof(TEXT_tdst_OneText));
	L_memset(pText, 0, sizeof(TEXT_tdst_OneText));


	pc_Buf = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);
	pText->ul_Key = key;

	if(pc_Buf && ul_Size)
	{
		pText->ul_Num = LOA_ReadULong(&pc_Buf);
        ul_Size -= sizeof(ULONG);

        pText->pst_Ids = p = (TEXT_tdst_Id *) MEM_p_Alloc(pText->ul_Num * sizeof(TEXT_tdst_Id));
		L_memset(p, 0, pText->ul_Num * sizeof(TEXT_tdst_Id));

		for(i = 0; i < pText->ul_Num; i++, p++)
		{
            p->f_LastTime = 0.0f;

		    p->ul_IdKey = LOA_ReadULong(&pc_Buf);
            ul_Size -= sizeof(ULONG);

            LOA_ReadULong_Ed(&pc_Buf, NULL);
            p->ul_SoundKey = -1;
            if(!LOA_IsBinaryData()) ul_Size -= sizeof(ULONG);

			p->ul_ObjKey = LOA_ReadULong(&pc_Buf);
            ul_Size -= sizeof(ULONG);

			p->i_Offset = LOA_ReadInt(&pc_Buf);
            ul_Size -= sizeof(int);

	        i_Priority = LOA_ReadInt(&pc_Buf);
			ul_Size -= sizeof(int);

			p->us_Priority = (unsigned short)(0x0000FFFF & i_Priority);
			p->us_Version = (unsigned short)((0xFFFF0000 & i_Priority) >> 16);
			if(p->us_Version < TEXT_Cte_Version1)
			{
				p->c_FacialIdx = TEXT_e_ExpNormal;
				p->c_LipsIdx = TEXT_e_LipsNormal;
				p->c_AnimIdx = 0;
				p->c_DumIdx = 0;
			}
			else
			{
				p->c_FacialIdx = LOA_ReadChar(&pc_Buf);
				ul_Size -= sizeof(char);

				p->c_LipsIdx = LOA_ReadChar(&pc_Buf);
				ul_Size -= sizeof(char);
				
				p->c_AnimIdx = LOA_ReadChar(&pc_Buf);
				ul_Size -= sizeof(char);
				
				p->c_DumIdx = LOA_ReadChar(&pc_Buf);
				ul_Size -= sizeof(char);
			}

			ulComment = LOA_ReadULong(&pc_Buf);
            ul_Size -= sizeof(ULONG);

            LOA_ReadString_Ed(&pc_Buf, p->asz_Name, TEXT_MaxLenId);
            if(!LOA_IsBinaryData()) ul_Size -= TEXT_MaxLenId * sizeof(char);

#ifdef ACTIVE_EDITORS
   		    p->asz_Comment = (char*)MEM_p_Alloc(ulComment);
#endif

			LOA_ReadString_Ed(&pc_Buf, p->asz_Comment, ulComment);
            if(!LOA_IsBinaryData()) ul_Size -= ulComment * sizeof(char);
			
#ifdef ACTIVE_EDITORS
			if(!LOA_IsBinaryData())
            {
				char	*sz_Cur, *sz_Old, *sz_Last;
				int		commentindex;
                ULONG   ulComment2 = ulComment;
				/*------------------ transform old comment (with \t char) ---------------*/
    			if ( *p->asz_Comment == '\t' )
    			{
    				sz_Cur = p->asz_Comment;
    				sz_Old = sz_Cur + 1;
    				sz_Last = sz_Cur + ulComment2;
    				commentindex = 0;
    				while (sz_Old < sz_Last )
    				{
    					if (commentindex >= 0)
    					{
    						if (*sz_Old == '\t')
    						{
    							*sz_Cur++ = 0;
    							commentindex ++;
    							if (commentindex == 32)
    								break;
    						}
    						else
    							*sz_Cur++ = *sz_Old;
    					}
    					else if (*sz_Old == '\t')
    						commentindex++;
    					sz_Old++;
    				}
    				ulComment2 = sz_Cur - p->asz_Comment;
    			}
    			/*------------------ compute start of each comment ---------------*/
    			sz_Cur = sz_Old = p->asz_Comment;
    			sz_Last = sz_Cur + ulComment2;
    			commentindex = 0;
    			while (sz_Cur < sz_Last )
    			{
    				if (*sz_Cur == 0)
    				{
    					p->i_CommentOffset[ commentindex++ ] = sz_Old - p->asz_Comment;
    					sz_Old = sz_Cur + 1;
    				}
    				sz_Cur++;
    			}
    			/*------------------ add comment if not enough ---------------*/
    			if (commentindex < 32 )
    			{
     				p->asz_Comment = (char*)MEM_p_Realloc( p->asz_Comment, ulComment2 + 32 );
   				sz_Cur = p->asz_Comment + ulComment2;
    				while (commentindex < 32 )
    				{
    					p->i_CommentOffset[ commentindex++ ] = sz_Cur - p->asz_Comment;
    					*sz_Cur++ = 0;
    				}
	   			}
    		}
#endif

        }

        if(ul_Size)
        {
		    pText->psz_Text = (char *) MEM_p_Alloc(ul_Size);
		    LOA_ReadString(&pc_Buf, pText->psz_Text, ul_Size);
        }
        else
        {
		    pText->psz_Text = (char *) MEM_p_Alloc(1);
            *pText->psz_Text = 0;		    
        }
        
        /* temporaire */
		for(i = 0; i < ul_Size; i++)
		{
		    if(pText->psz_Text[i] == '\r') pText->psz_Text[i] = '\n';		    
        }
		/* fin temporaire */
	}

	pText->ul_Inst = 1;
    pText->ul_FatherKey = BIG_C_InvalidKey;
    

#ifdef ACTIVE_EDITORS
    if(!TEXT_gb_EdiLoad)
#endif
    {
        LOA_AddPosAddress(_ul_PosFile, pText);
    }

	return(ULONG) pText;
}

ULONG TEXT_ul_CallbackLoadTxlForSound(ULONG _ul_PosFile, ULONG key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CHAR				*pc_Buf;
	ULONG				ul_Size;
	TEXT_tdst_OneText	*pText;
	TEXT_tdst_Id		*p;
	ULONG				i;
	ULONG				ulComment;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_PosFile == -1) return 0;
	
    pText = TEXT_gp_CallbackLoadTxlForSound ;
	

    pc_Buf = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);
	
	if(pc_Buf && ul_Size)
	{
        LOA_ReadULong_Ed(&pc_Buf, NULL);
        if(!LOA_IsBinaryData()) ul_Size -= sizeof(ULONG);
        
        p = pText->pst_Ids;
		for(i = 0; i < pText->ul_Num; i++, p++)
		{
		    /*p->ul_IdKey = */
		    LOA_ReadULong_Ed(&pc_Buf, NULL);
            if(!LOA_IsBinaryData()) ul_Size -= sizeof(ULONG);

            p->ul_SoundKey = LOA_ReadULong(&pc_Buf);
            ul_Size -= sizeof(ULONG);

			/*p->ul_ObjKey = */
			LOA_ReadULong_Ed(&pc_Buf, NULL);
			if(!LOA_IsBinaryData()) ul_Size -= sizeof(ULONG);

			/*p->i_Offset = */
			LOA_ReadInt_Ed(&pc_Buf, NULL);
            if(!LOA_IsBinaryData()) ul_Size -= sizeof(int);

	        /*p->i_Priority = */
	        LOA_ReadInt_Ed(&pc_Buf, NULL);
            if(!LOA_IsBinaryData()) ul_Size -= sizeof(int);

			if(p->us_Version >= TEXT_Cte_Version1)
			{
				/* p->c_FacialIdx */
				LOA_ReadChar_Ed(&pc_Buf, NULL);
	            if(!LOA_IsBinaryData()) ul_Size -= sizeof(char);

				/*p->c_LipsIdx */
				LOA_ReadChar_Ed(&pc_Buf, NULL);
	            if(!LOA_IsBinaryData()) ul_Size -= sizeof(char);
				
				/*p->c_AnimIdx */
				LOA_ReadChar_Ed(&pc_Buf, NULL);
	            if(!LOA_IsBinaryData()) ul_Size -= sizeof(char);
				
				/*p->c_DumIdx */
				LOA_ReadChar_Ed(&pc_Buf, NULL);
	            if(!LOA_IsBinaryData()) ul_Size -= sizeof(char);
			}

			ulComment = LOA_ReadULong(&pc_Buf);
            ul_Size -= sizeof(ULONG);

            LOA_ReadString_Ed(&pc_Buf, NULL, TEXT_MaxLenId);
            if(!LOA_IsBinaryData()) ul_Size -= TEXT_MaxLenId * sizeof(char);

			LOA_ReadString_Ed(&pc_Buf, NULL, ulComment);
            if(!LOA_IsBinaryData()) ul_Size -= ulComment * sizeof(char);
			
			if(p->ul_SoundKey && (p->ul_SoundKey != BIG_C_InvalidKey))
			{
			    SND_l_AddDialog(p->ul_SoundKey);
            }
		}
		
        if(ul_Size)
        {
		    LOA_ReadString_Ed(&pc_Buf, NULL, ul_Size);
        }
	}

	return(ULONG) pText;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
static ULONG TEXT_ul_CallbackLoadTxg(ULONG ul_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CHAR		*pc_Buf, *pc_EndBuf;
	ULONG		ul_Size;
	int			j;
	ULONG		ul_Dum;
#ifdef ACTIVE_EDITORS
	char		asz_Msg[512];
	BIG_INDEX	ul_File;
#endif
	BIG_KEY		ul_TxlKey, ul_TxlBankKey, ul_TxlFirstKey;
    int         i_CurrentLanguage;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ERR_X_Warning(ul_Pos != -1, "One txg file key is unknown. Check all txt files.", NULL);

    if(ul_Pos == -1) return 0;

	/* Read the file that contains all the languages */
	pc_Buf = pc_EndBuf = BIG_pc_ReadFileTmp(ul_Pos, &ul_Size);
	pc_EndBuf += ul_Size;

	/* Scan for language */
    ul_TxlBankKey = ul_TxlFirstKey = 0;
    i_CurrentLanguage = TEXT_gp_CallbackLoadTxlForSound ? TEXT_gst_Global.i_CurrentSndLanguage:TEXT_gst_Global.i_CurrentLanguage;

	for(j = 0; pc_Buf < pc_EndBuf; j++)
	{
		ul_TxlKey = LOA_ReadULong(&pc_Buf); /* read group element key */
		ul_Dum = LOA_ReadULong(&pc_Buf);

		if(ul_TxlKey == 0) continue;
		if(ul_TxlKey == BIG_C_InvalidKey) continue;
		if(!ul_TxlFirstKey) ul_TxlFirstKey = ul_TxlKey;
		if(j == i_CurrentLanguage) ul_TxlBankKey = ul_TxlKey;
	}

	/* No language : Take first one */
	if(ul_TxlBankKey == 0)
	{
#ifdef ACTIVE_EDITORS
		ul_File = BIG_ul_SearchKeyToFat(LOA_ul_GetCurrentKey());
		sprintf(asz_Msg, "Language %d not defined for %s", i_CurrentLanguage, BIG_NameFile(ul_File));
		ERR_X_Warning(0, asz_Msg, NULL);
#endif
		ul_TxlBankKey = ul_TxlFirstKey;
	}

    if(TEXT_gp_CallbackLoadTxlForSound)
	    ul_Dum = TEXT_ul_CallbackLoadTxlForSound(BIG_ul_SearchKeyToPos(ul_TxlBankKey), ul_TxlBankKey);
    else
        ul_Dum = TEXT_ul_CallbackLoadTxl(BIG_ul_SearchKeyToPos(ul_TxlBankKey), ul_TxlBankKey);
    
    return ul_Dum;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void TEXT_EdiCheckFileType(ULONG ulKey)
{
#ifdef ACTIVE_EDITORS
	ulKey = BIG_ul_SearchKeyToFat(ulKey);
	ERR_X_Warning
	(
		((ulKey != BIG_C_InvalidIndex) && BIG_b_IsFileExtension(ulKey, EDI_Csz_ExtTextLang)),
		"[TEXT] txt file contains bad files (check that all txt files contain only txg reference) !",
		NULL
	);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static ULONG TEXT_ul_CallbackLoadTxtNoSound(ULONG ul_Pos)
{
    return TEXT_ul_CallbackLoadTxt(ul_Pos, FALSE);
}

static ULONG TEXT_ul_CallbackLoadTxtSound(ULONG ul_Pos)
{
    return TEXT_ul_CallbackLoadTxt(ul_Pos, TRUE);
}

static ULONG TEXT_ul_CallbackLoadTxt(ULONG ul_Pos, BOOL b_ModeSound)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CHAR				*pc_Buf;
	ULONG				ul_Size;
	ULONG				j;
	BIG_tdst_GroupElem	*dstGroup;
	int					iNewFile;
	TEXT_tdst_OneText	*pOneText;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Read the file that contains all the texts */
	pc_Buf = BIG_pc_ReadFileTmp(ul_Pos, &ul_Size);
	if(ul_Size)
	{
		dstGroup = (BIG_tdst_GroupElem *) MEM_p_AllocTmp(ul_Size);
		L_memset(dstGroup, 0, ul_Size);

		iNewFile = 0;
		ul_Size = ul_Size / sizeof(BIG_tdst_GroupElem);

		/*$1- read txt file contents (we have to release the TmpRead) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		for(j = 0; j < ul_Size; j++)
		{
			dstGroup[j].ul_Key = LOA_ReadULong(&pc_Buf);
			dstGroup[j].ul_Type = LOA_ReadULong(&pc_Buf);
		}

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		for(j = 0; j < ul_Size; j++)
		{
			/* skip bad keys */
			if(dstGroup[j].ul_Key == BIG_C_InvalidKey) continue;
			if(dstGroup[j].ul_Key == 0) continue;

			/* check file in editor mode only */
			TEXT_EdiCheckFileType(dstGroup[j].ul_Key);

			/* load file */
            if(b_ModeSound)
            {
                iNewFile = TEXT_i_GetOneTextIndex(dstGroup[j].ul_Key);
                if(iNewFile < 0) continue;

                TEXT_gp_CallbackLoadTxlForSound = TEXT_gst_Global.pst_AllTexts[iNewFile] ;
                TEXT_ul_CallbackLoadTxg(BIG_ul_SearchKeyToPos(dstGroup[j].ul_Key));            
                TEXT_gp_CallbackLoadTxlForSound = NULL;
            }
            else
            {
			    pOneText = (TEXT_tdst_OneText *) TEXT_ul_CallbackLoadTxg(BIG_ul_SearchKeyToPos(dstGroup[j].ul_Key));

			    /* if file is new, register it into the global structure */
			    if(pOneText && (pOneText->ul_Inst <= 1))
			    {
                    pOneText->ul_FatherKey = dstGroup[j].ul_Key;
				    iNewFile = TEXT_i_AllocOneTextIndex();
				    TEXT_gst_Global.pst_AllTexts[iNewFile] = pOneText;
			    }
            }
		}

		/*$1- load only new files ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MEM_Free(dstGroup);
	}

	return(ULONG) TEXT_gst_Global.pst_AllTexts;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXT_LoadWorldText(void)
{
	/*~~~~~~~~~~~*/
	BIG_KEY ul_Txt;
	ULONG	ul;
	extern unsigned int WOR_gul_WorldKey;
	int     lang;
	/*~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    force loading for all language (binarization)
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(LOA_gb_SpeedMode)
	{
		/*~~~~~~~*/
		int i, cur;
		/*~~~~~~~*/

		cur = TEXT_gst_Global.i_CurrentLanguage;
		TEXT_gi_ChangingLang = 1;
		for(i = 0; i < INO_e_MaxLangNb; i++)
		{
			if(!INO_b_LanguageIsPresent(i)) continue;

			TEXT_FreeAll();
			TEXT_gst_Global.i_CurrentLanguage = i;
			TEXT_gst_Global.i_CurrentSndLanguage = i;

			ul_Txt = 0xFD000000 | ((i + 1) << 19);
			ul_Txt |= LOA_ul_BinKey & 0x0007FFFF;
			LOA_BeginSpeedMode(ul_Txt);
			for(ul = 0; ul < WOR_gul_WorldTextSize; ul++)
			{
				if(WOR_gaul_WorldText[ul] != BIG_C_InvalidKey)
				{
					LOA_MakeFileRef
					(
						WOR_gaul_WorldText[ul],
						NULL,
						TEXT_ul_CallbackLoadTxtNoSound,
						LOA_C_MustExists | LOA_C_NotSavePtr
					);
				}
			}
			LOA_Resolve();
			LOA_EndSpeedMode();
			


			ul_Txt = 0xFE000000 | ((i + 1) << 19);
			ul_Txt |= LOA_ul_BinKey & 0x0007FFFF;
			LOA_BeginSpeedMode(ul_Txt);
			for(ul = 0; ul < WOR_gul_WorldTextSize; ul++)
			{
				if(WOR_gaul_WorldText[ul] != BIG_C_InvalidKey)
				{
					LOA_MakeFileRef
					(
						WOR_gaul_WorldText[ul],
						NULL,
						TEXT_ul_CallbackLoadTxtSound,
						LOA_C_MustExists | LOA_C_NotSavePtr
					);
				}
			}

			LOA_Resolve();
			LOA_EndSpeedMode();
			
		}

		TEXT_gi_ChangingLang = 0;
		TEXT_gst_Global.i_CurrentLanguage = cur;
		TEXT_gst_Global.i_CurrentSndLanguage = cur;
    }
    else
    {
#endif

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    load text for the current language
	 -------------------------------------------------------------------------------------------------------------------
	 */

#ifdef ACTIVE_EDITORS
    lang = TEXT_i_GetLang() ;
#else
    lang = (TEXT_gi_ChangedLang>1) ? TEXT_i_GetLang() : AI_EvalFunc_IoConsLangGet_C();
    if(!INO_b_LanguageIsPresent(lang)) lang = INO_e_English;
#endif    

	ul_Txt = 0xFD000000 | (WOR_gul_WorldKey&0x0007FFFF) | ((lang + 1) << 19);
	LOA_BeginSpeedMode(ul_Txt);
	for(ul = 0; ul < WOR_gul_WorldTextSize; ul++)
	{
		if(WOR_gaul_WorldText[ul] != BIG_C_InvalidKey)
		{
			LOA_MakeFileRef(WOR_gaul_WorldText[ul], NULL, TEXT_ul_CallbackLoadTxtNoSound, LOA_C_MustExists | LOA_C_NotSavePtr);
		}
	}
	LOA_Resolve();
	LOA_EndSpeedMode();


    lang = TEXT_i_GetSndLang();
    if(!INO_b_LanguageIsPresent(lang)) lang = INO_e_English;
    
	ul_Txt = 0xFE000000 | (WOR_gul_WorldKey&0x0007FFFF) | ((lang + 1) << 19);
	LOA_BeginSpeedMode(ul_Txt);
	for(ul = 0; ul < WOR_gul_WorldTextSize; ul++)
	{
		if(WOR_gaul_WorldText[ul] != BIG_C_InvalidKey)
		{
			LOA_MakeFileRef(WOR_gaul_WorldText[ul], NULL, TEXT_ul_CallbackLoadTxtSound, LOA_C_MustExists | LOA_C_NotSavePtr);
		}
	}
	LOA_Resolve();
	LOA_EndSpeedMode();


#ifdef ACTIVE_EDITORS
    }

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Check for validity of indexes:: for all string but the first one we look previous character, it must be a null
	    character
	 -------------------------------------------------------------------------------------------------------------------
	 */

	{
		/*~~~~~*/
		int i, j;
		/*~~~~~*/

		for(j = 0; j < TEXT_gst_Global.i_Num; j++)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			TEXT_tdst_OneText	*pst_Text;
			char				asz_Msg[512];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Text = TEXT_gst_Global.pst_AllTexts[j];
			if(pst_Text)
			{
				for(i = 1; pst_Text && (i < (int) pst_Text->ul_Num); i++)
				{
                    if(pst_Text->pst_Ids[i].i_Offset == -1) continue;

					if(pst_Text->psz_Text[pst_Text->pst_Ids[i].i_Offset - 1] != 0)
					{
						sprintf(asz_Msg, "Bad index in text file number %d [%x]", j, pst_Text->ul_Key);
						ERR_X_Warning(0, asz_Msg, NULL);
						break;
					}
				}
			}
		}
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXT_Free(TEXT_tdst_OneText *_pst_Text)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	if(!_pst_Text) return;

	_pst_Text->ul_Inst--;
	
	
	for(i = 0; i < _pst_Text->ul_Num; i++)
	{
#ifdef ACTIVE_EDITORS
	    if(_pst_Text->pst_Ids[i].asz_Comment) MEM_Free(_pst_Text->pst_Ids[i].asz_Comment);
		_pst_Text->pst_Ids[i].asz_Comment = NULL;
#endif
		if(_pst_Text->pst_Ids[i].ul_SoundKey && (_pst_Text->pst_Ids[i].ul_SoundKey != BIG_C_InvalidKey))
		{
		    SND_Delete(_pst_Text->pst_Ids[i].ul_SoundKey);
        }		
	}
    if(_pst_Text->ul_Inst) return;


	if(_pst_Text->pst_Ids) MEM_Free(_pst_Text->pst_Ids);
	if(_pst_Text->psz_Text) MEM_Free(_pst_Text->psz_Text);

	MEM_Free(_pst_Text);
	LOA_DeleteAddress(_pst_Text);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXT_FreeAll(void)
{
	/*~~*/
	int i,j,k;
	/*~~*/

	if(!TEXT_gi_ChangingLang)
	{
		/* delete the txt key list only if we are not changing the current language */
		if(WOR_gaul_WorldText) MEM_Free(WOR_gaul_WorldText);
		WOR_gaul_WorldText = NULL;
		WOR_gul_WorldTextSize = 0;
	}

	if(TEXT_gst_Global.pst_AllTexts)
	{
		for(i = 0; i < TEXT_gst_Global.i_Num; i++)
		{
			if(TEXT_gst_Global.pst_AllTexts[i])
			{
                k = TEXT_gst_Global.pst_AllTexts[i]->ul_Inst ;
                for(j=0; j<k; j++)
                    TEXT_Free(TEXT_gst_Global.pst_AllTexts[i]);
			}
		}

		LOA_DeleteAddress(TEXT_gst_Global.pst_AllTexts);
		MEM_Free(TEXT_gst_Global.pst_AllTexts);
	}

	TEXT_gst_Global.i_Num = 0;
	TEXT_gst_Global.pst_AllTexts = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXT_ChangeLang(int _i_Lang)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_KEY		ul_Txt;
	extern void PreLoadTexReset(void);
	ULONG		i;
	extern unsigned int WOR_gul_WorldKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!INO_b_LanguageIsPresent(_i_Lang)) _i_Lang = INO_e_English;

	TEXT_gi_ChangedLang++;
	TEXT_gi_ChangingLang = 1;

	PreLoadTexReset();
	TEXT_FreeAll();
	TEXT_gst_Global.i_CurrentLanguage = _i_Lang;

	ul_Txt = 0xFD000000 | (WOR_gul_WorldKey&0x0007FFFF) | ((TEXT_gst_Global.i_CurrentLanguage + 1) << 19);
	LOA_BeginSpeedMode(ul_Txt);
	for(i = 0; i < WOR_gul_WorldTextSize; i++)
	{
		if(WOR_gaul_WorldText[i] != BIG_C_InvalidKey)
		{
			LOA_MakeFileRef(WOR_gaul_WorldText[i], NULL, TEXT_ul_CallbackLoadTxtNoSound, LOA_C_MustExists | LOA_C_NotSavePtr);
		}
	}
	LOA_Resolve();
	LOA_EndSpeedMode();

#if defined(ACTIVE_EDITORS) || defined(_XENON)
    TEXT_gst_Global.i_CurrentSndLanguage = _i_Lang;
#endif
    
    if(!INO_b_LanguageIsPresent(TEXT_gst_Global.i_CurrentSndLanguage)) TEXT_gst_Global.i_CurrentSndLanguage = INO_e_English;

    ul_Txt = 0xFE000000 | (WOR_gul_WorldKey&0x0007FFFF) | ((TEXT_gst_Global.i_CurrentSndLanguage + 1) << 19);
	LOA_BeginSpeedMode(ul_Txt);
	for(i = 0; i < WOR_gul_WorldTextSize; i++)
	{
		if(WOR_gaul_WorldText[i] != BIG_C_InvalidKey)
		{
			LOA_MakeFileRef(WOR_gaul_WorldText[i], NULL, TEXT_ul_CallbackLoadTxtSound, LOA_C_MustExists | LOA_C_NotSavePtr);
		}
	}
	LOA_Resolve();
	LOA_EndSpeedMode();

    SND_UpdateLang();
	STR_3DStringList_Reinit();
	TEXT_gi_ChangingLang = 0;
}



void TEXT_ChangeSndLang(int _i_Lang)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_KEY		ul_Txt;
	extern void PreLoadTexReset(void);
	ULONG		i;
	extern unsigned int WOR_gul_WorldKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION 
	if(!INO_b_LanguageIsPresent(_i_Lang, TRUE)) _i_Lang = INO_e_English;
#else
	if(!INO_b_LanguageIsPresent(_i_Lang)) _i_Lang = INO_e_English;
#endif
	TEXT_gi_ChangedLang++;
	TEXT_gi_ChangingLang = 1;

	PreLoadTexReset();
	TEXT_FreeAll();
	

#ifdef ACTIVE_EDITORS
    TEXT_gst_Global.i_CurrentLanguage = _i_Lang;
#endif
    
    if(!INO_b_LanguageIsPresent(TEXT_gst_Global.i_CurrentLanguage)) TEXT_gst_Global.i_CurrentLanguage = INO_e_English;
    

    ul_Txt = 0xFD000000 | (WOR_gul_WorldKey&0x0007FFFF) | ((TEXT_gst_Global.i_CurrentLanguage + 1) << 19);
	LOA_BeginSpeedMode(ul_Txt);
	for(i = 0; i < WOR_gul_WorldTextSize; i++)
	{
		if(WOR_gaul_WorldText[i] != BIG_C_InvalidKey)
		{
			LOA_MakeFileRef(WOR_gaul_WorldText[i], NULL, TEXT_ul_CallbackLoadTxtNoSound, LOA_C_MustExists | LOA_C_NotSavePtr);
		}
	}
	LOA_Resolve();
	LOA_EndSpeedMode();


	TEXT_gst_Global.i_CurrentSndLanguage = _i_Lang;

    ul_Txt = 0xFE000000 | (WOR_gul_WorldKey&0x0007FFFF) | ((TEXT_gst_Global.i_CurrentSndLanguage + 1) << 19);
	LOA_BeginSpeedMode(ul_Txt);
	for(i = 0; i < WOR_gul_WorldTextSize; i++)
	{
		if(WOR_gaul_WorldText[i] != BIG_C_InvalidKey)
		{
			LOA_MakeFileRef(WOR_gaul_WorldText[i], NULL, TEXT_ul_CallbackLoadTxtSound, LOA_C_MustExists | LOA_C_NotSavePtr);
		}
	}
	LOA_Resolve();
	LOA_EndSpeedMode();


    SND_UpdateLang();
	STR_3DStringList_Reinit();
	TEXT_gi_ChangingLang = 0;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int TEXT_i_GetLang(void)
{
	return TEXT_gst_Global.i_CurrentLanguage;
}

int TEXT_i_GetSndLang(void)
{
	return TEXT_gst_Global.i_CurrentSndLanguage;
}

/*
 =======================================================================================================================
 get one text struct index from the txg key
 =======================================================================================================================
 */
int TEXT_i_GetOneTextIndex(ULONG ulKey)
{
	/*~~*/
	int i;
	/*~~*/

    if(ulKey == BIG_C_InvalidKey) return -1;

	for(i = 0; i < TEXT_gst_Global.i_Num; i++)
	{
        if(!TEXT_gst_Global.pst_AllTexts[i]) continue;
		if(TEXT_gst_Global.pst_AllTexts[i]->ul_FatherKey == ulKey) return i;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int TEXT_i_GetEntryIndex(TEXT_tdst_OneText *pText, int key)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	if(!pText) return -1;
	if(!pText->ul_Num) return -1;

	for(i = 0; i < pText->ul_Num; i++)
	{
		if(pText->pst_Ids[i].ul_IdKey == key) return (int) i;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void TEXT_FreeOneTextIndex(int i)
{
	if(i < TEXT_gst_Global.i_Num) TEXT_gst_Global.pst_AllTexts[i] = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int TEXT_i_AllocOneTextIndex(void)
{
	/*~~~~~~~~~~~~~~*/
	int iNewFile = 16;
	int iFree;
	/*~~~~~~~~~~~~~~*/

	/* search a free space */
	for(iFree = 0; iFree < TEXT_gst_Global.i_Num; iFree++)
	{
		if(TEXT_gst_Global.pst_AllTexts[iFree] == NULL) return iFree;
	}

	/* alloc space */
	if(TEXT_gst_Global.pst_AllTexts)
	{
		TEXT_gst_Global.pst_AllTexts = (TEXT_tdst_OneText **) MEM_p_Realloc
			(
				TEXT_gst_Global.pst_AllTexts,
				(TEXT_gst_Global.i_Num + iNewFile) * sizeof(TEXT_tdst_OneText *)
			);
		L_memset(&TEXT_gst_Global.pst_AllTexts[TEXT_gst_Global.i_Num], 0, iNewFile * sizeof(TEXT_tdst_OneText *));
	}
	else
	{
		TEXT_gst_Global.pst_AllTexts = (TEXT_tdst_OneText **) MEM_p_Alloc(iNewFile * sizeof(TEXT_tdst_OneText *));
		L_memset(&TEXT_gst_Global.pst_AllTexts[0], 0, iNewFile * sizeof(TEXT_tdst_OneText *));
	}

	TEXT_gst_Global.i_Num += iNewFile;
	return iFree;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
