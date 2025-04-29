/*$T TEXTframe_ini.cpp GC 1.138 03/17/04 16:53:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/ERRors/ERRasser.h"
#include "TEXTscroll.h"
#include "TEXTframe.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGfat.h"

char	*ETEXT_gasz_ColName[] =
{
	"Empty",
	"Rank",
	"Entry",
	"Text",
	"EntryKey",
	"SndFileKey",
	"CharacterKey",
	"TxgFileKey",
	"TxlFileKey",
	"LabelAuto",
    "Priority",
    "FacialExpression",
    "Lips",
    "Anim",
	"UserData",
	"Masked"
};


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::ReinitIni(void)
{
	/*~~*/
	int i;
	/*~~*/

	EDI_cl_BaseFrame::ReinitIni();

	mst_Ini.i_Version = 0x55005505;
	mst_Ini.i_LeftPaneCX = 150;
	mst_Ini.i_LeftPaneCY1 = 150;
	mst_Ini.i_LeftPaneCY2 = 150;
#ifdef JADEFUSION
	mst_Ini.i_OptionFlags = ETEXT_Cte_UseStandardLang | ETEXT_Cte_SyncAuto; // | ETEXT_Cte_KeepAllTxt;
#else
	mst_Ini.i_OptionFlags = ETEXT_Cte_UseStandardLang | ETEXT_Cte_SyncAuto | ETEXT_Cte_KeepAllTxt;
#endif	 
    L_memset(mst_Ini.ac_Reserve, 0, sizeof(mst_Ini.ac_Reserve));
    L_memset(mst_Ini.ac_UserColUsed, 0, sizeof(mst_Ini.ac_UserColUsed));
    
    for (i = 0; i < COLMAX; i++)
	{
		mst_Ini.ai_ColSize[ i ] = (i + 1 > en_Character ) ? -100 : 100;
		mst_Ini.ai_ColOrder[ i ] = i;
		if ( i + 1 < en_UserData )
			L_strcpy( mst_Ini.asz_ColName[ i ], ETEXT_gasz_ColName[ i + 1 ] );
		else
			sprintf( mst_Ini.asz_ColName[ i ], "UserData %d", en_Rank + i - en_UserData );
	}
	
	mst_Ini.i_NbCharacterKey = 0;
	mst_Ini.i_NbSndFileKey = 0;

    ResetEntryColumnInfo();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::LoadIni(void)
{
_Try_
    EDI_cl_BaseFrame::BaseLoadIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
    TreatIni();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::TreatIni(void)
{
	if(mst_Ini.i_Version < 0x55005504)
	{
		mst_Ini.i_Version = 0x55005504;
		ReinitIni();
		SaveIni();
	}
	
	if(mst_Ini.i_Version != 0x55005505	)
	{
		mst_Ini.i_Version = 0x55005505;
		mst_Ini.i_OrderColumn = 0;
	}

    SetEntryColumnInfo();

	mo_Splitter0.SetColumnInfo(0, mst_Ini.i_LeftPaneCX, 0);
	mo_Splitter2.SetRowInfo(0, mst_Ini.i_LeftPaneCY2, 0);

	mo_Splitter0.RecalcLayout();
	mo_Splitter2.RecalcLayout();
	mpo_Scroll->mi_OrderColumn = mst_Ini.i_OrderColumn;

	ULONG ul_Index = BIG_ul_SearchFileExt(EDI_Csz_Ini, "TextAlias.ini");
	if(ul_Index != BIG_C_InvalidIndex)
	{
		ULONG ul_Size;
		char*pc,*pz1,*pz2;
		int iCol, iVal;
		char	*ptr;

		//max_EnumUserDefinition
		pz1 = pz2 = pc = BIG_pc_ReadFileTmp(BIG_PosFile(ul_Index), &ul_Size );

		while(1)
		{
			while((*pz2 != '@') && *pz2) pz2++;
			*pz2++ = 0;

			iCol  = e_GetColTypeFromName(pz1);
			if(
				(iCol !=en_Priority) &&
				(iCol !=en_FacialExp) &&
				(iCol !=en_Lips		) &&
				(iCol !=en_Anims	) 
				)
			{
				// exit
				break;
			}

			if( ! max_EnumUserDefinition[iCol].st_NameToValue.gran )
			{
				BAS_binit(&max_EnumUserDefinition[iCol].st_NameToValue, 10);
				BAS_binit(&max_EnumUserDefinition[iCol].st_ValueToName, 10);
			}

			pz1 = pz2;
			while((*pz2 != '@') && *pz2) pz2++;
			*pz2++ = 0;
			
			sscanf(pz1, "%d", &iVal);

			
			pz1 = pz2;
			while((*pz2 != '@') && *pz2) pz2++;
			*pz2++ = 0;

			ptr = new char[L_strlen(pz1)+1];
			L_strcpy(ptr, pz1);
			
			BAS_binsert((ULONG)iVal, (ULONG)ptr, &max_EnumUserDefinition[iCol].st_ValueToName);
			BAS_binsert((ULONG)ptr, (ULONG)iVal, &max_EnumUserDefinition[iCol].st_NameToValue);

			
			if(*pz2++ != '@') break;
			
			if(*pz2++ != 0x0d) break;
			
			if(*pz2++ != 0x0a) break;

			pz1 = pz2;
		}
	}
#ifdef JADEFUSION
	mst_Ini.i_OptionFlags &= ~ETEXT_Cte_KeepAllTxt;
#else
	mst_Ini.i_OptionFlags |= ETEXT_Cte_KeepAllTxt;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::SaveIni(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i_Tmp;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GetEntryColumnInfo();
    SetEntryColumnInfo();
	OnFileChange();

	/* get splitter sizes */
	mst_Ini.i_Version = 0x55005505;
	mst_Ini.i_OrderColumn = mpo_Scroll->mi_OrderColumn;
	mo_Splitter0.GetColumnInfo(0, mst_Ini.i_LeftPaneCX, i_Tmp);
	mo_Splitter2.GetRowInfo(0, mst_Ini.i_LeftPaneCY2, i_Tmp);

	
_Try_ 
    EDI_cl_BaseFrame::BaseSaveIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::GetEntryColumnInfo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i;
    BOOL        ab_Masked[COLMAX];
    BOOL        ab_Empty[COLMAX];
    en_ColContent ae_Content[COLMAX];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* reset col usage */
	for(i = 0; i < COLMAX; i++)
	{
        ab_Masked[i] = mst_Ini.ae_ContentCols[i] & en_Masked;
        ab_Empty[i] = mst_Ini.ae_ContentCols[i] == en_Empty;
        ae_Content[i] = mst_Ini.ae_ContentCols[i];

		mst_Ini.ae_ContentCols[i] = en_Empty;
		L_strcpy(mst_Ini.ai_NameCols[i], ETEXT_gasz_ColName[en_Empty]);

        mst_Ini.ac_UserColUsed[i] = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::ResetEntryColumnInfo(void)
{
    int i;
    for(i = 0; i < COLMAX; i++)
	{
		mst_Ini.ai_SizeCols[i] = 100;
		mst_Ini.ae_ContentCols[i] = en_Empty;
		L_strcpy(mst_Ini.ai_NameCols[i], ETEXT_gasz_ColName[en_Empty]);
        mst_Ini.ac_UserColUsed[i] = 0;
	}

	i = 0;
	mst_Ini.ae_ContentCols[i++] = ETEXT_cl_Frame::en_Rank;
	mst_Ini.ae_ContentCols[i++] = ETEXT_cl_Frame::en_ID;
	mst_Ini.ae_ContentCols[i++] = ETEXT_cl_Frame::en_Preview;
	mst_Ini.ae_ContentCols[i++] = ETEXT_cl_Frame::en_IDKey;

	i = 0;
	L_strcpy(mst_Ini.ai_NameCols[i], ETEXT_gasz_ColName[mst_Ini.ae_ContentCols[i]]);
	i++;
	L_strcpy(mst_Ini.ai_NameCols[i], ETEXT_gasz_ColName[mst_Ini.ae_ContentCols[i]]);
	i++;
	L_strcpy(mst_Ini.ai_NameCols[i], ETEXT_gasz_ColName[mst_Ini.ae_ContentCols[i]]);
	i++;
	L_strcpy(mst_Ini.ai_NameCols[i], ETEXT_gasz_ColName[mst_Ini.ae_ContentCols[i]]);
	i++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::SetEntryColumnInfo(void)
{
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
ETEXT_cl_Frame::en_ColContent ETEXT_cl_Frame::e_GetColTypeFromName(char *p)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = en_Empty; i < en_UserData; i++)
	{
		if(!L_strcmp(p, ETEXT_gasz_ColName[i])) return en_ColContent(i);
	}

	return en_UserData;
}

/*
 =======================================================================================================================
	assume that the char pointer parameter is a pointer on a array of char with COLMAX element £
	flags :
		non user column on		1
		user column on			2
 =======================================================================================================================
 */
#define Column_GetUsed_nonuser	1
#define Column_GetUsed_user		2

int ETEXT_cl_Frame::Column_GetUsed( BOOL *_pb_UsedCol, int _i_Flags )
{
	int i;
	
	/* column */
	L_memset(_pb_UsedCol, 0, COLMAX * sizeof( BOOL ) );
	
	if ( _i_Flags & Column_GetUsed_nonuser )
	{
		for(i = 0; i < en_UserData; i++) 
			_pb_UsedCol[i] = 1;
	}
		
	if ( _i_Flags & Column_GetUsed_user )
	{
		i = en_UserData;
		while(i < COLMAX)
		{
			if ( (mst_Ini.ai_ColSize[i] <= 0) &&	!L_strnicmp(mst_Ini.asz_ColName[i], "UserData ", 9) ) 
				break;
			_pb_UsedCol[i++] = 1;
		}
		if(i < COLMAX) _pb_UsedCol[i++] = 1;
	}
	return i;
}

#endif /* ACTIVE_EDITORS */
