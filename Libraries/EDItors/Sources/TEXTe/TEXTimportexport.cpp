/*$T TEXTframe.cpp GC 1.138 03/29/04 11:33:35 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
/**/
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
/**/
#include "TEXTframe.h"
#include "TEXTscroll.h"
/**/
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGgroup.h"
/**/
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
/**/
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "STRing/STRstruct.h"
/**/
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
/**/
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAlist_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAlang_dlg.h"
#include "DIAlogs/DIAtextfile_dlg.h"
#include "DIAlogs/CCheckList.h"
/**/
#include "BASe/MEMory/MEM.h"
/**/
#include "INOut/INO.h"
#include "Res/res.h"

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

void			TEXT_SetTextID(TEXT_tdst_OneText *_pt_Text, int ID, char *pz);
int				TEXT_i_GetEntryIndex(TEXT_tdst_OneText *pText, int key);
/**/
extern char		*ETEXT_gasz_ColName[];
/**/
typedef struct	ETEXT_Import_SpecialString_
{
	char			*sz_Code;
	unsigned char	uc_Char;
} ETEXT_Import_SpecialString;

ETEXT_Import_SpecialString ETEX_gast_SpecialString[] = 
{
	{ "nbsp", ' ' },
	{ "quot", '"' },
	{ "amp", '&' },
	{ "lt", '<' },
	{ "gt", '>' },
	{ "copy", 169 },
	{ "reg", 174 },
	{ "Agrave", 192 },
	{ "Aacute", 193 },
	{ "Acirc", 194 },
	{ "Atilde", 195 },
	{ "Auml", 196 },
	{ "Aring", 197 },
	{ "AElig", 198 },
	{ "Ccedil", 199 },
	{ "Egrave", 200 },
	{ "Eacute",201 },
	{ "Ecirc", 202 },
	{ "Euml", 203 },
	{ "Igrave", 204 },
	{ "Iacute", 205 },
	{ "Icirc", 206 },
	{ "Iuml", 207 },
	{ "ETH", 208 },
	{ "Ntilde", 209 },
	{ "Ograve", 210 },
	{ "Oacture", 211 },
	{ "Ocirc", 212 },
	{ "Octile", 213 },
	{ "Ouml", 214 },
	{ "times", 215 },
	{ "Oslash", 216 },
	{ "Ugrave", 217 },
	{ "Uacute", 218 },
	{ "Ucirc", 219 },
	{ "Uuml", 220 },
	{ "Yacute", 221 },
	{ "szlig", 223 },
	{ "agrave", 224 },
	{ "aacute", 225 },
	{ "acirc", 226 },
	{ "atilde", 227 },
	{ "auml", 228 },
	{ "aring", 229 },
	{ "aelig", 230 },
	{ "ccedil", 231 },
	{ "egrave", 232 },
	{ "eacute", 233 },
	{ "ecirc", 234 },
	{ "euml", 235 },
	{ "igrave", 236 },
	{ "iacute", 237 },
	{ "icirc", 238 },
	{ "iuml", 239 },
	{ "eth", 240 },
	{ "ntilde", 241 },
	{ "ograve", 242 },
	{ "oacute", 243 },
	{ "ocirc", 244 },
	{ "otilde", 245 },
	{ "ouml", 246 },
	{ "oslash", 248 },
	{ "ugrave", 249 },
	{ "uacute", 250 },
	{ "ucirc", 251 },
	{ "uuml", 252 },
	{ "yacute",253 },
	{ "yum", 255 },
	{ "", 0 }
};

#define	SPAN_UNKNOW			0
#define SPAN_SPACERUN		1
#define	SPAN_DISPLAYNONE	2


/*$4
 ***********************************************************************************************************************
	Store old file data before importation
 ***********************************************************************************************************************
 */
 typedef struct ETEXT_tdst_ImportData_
 {
	ULONG	ul_Key;
	char	*sz_Text[ INO_e_MaxLangNb ];
 } ETEXT_tdst_ImportData;
 /**/
 ETEXT_tdst_ImportData *ETEX_gpst_ImportData = NULL;
 int					ETEX_i_ImportData_Nb;
 int					ETEX_i_ImportData_Max;
 /**/
 int ETEX_ImportData_Add( ULONG _ul_Key ) 
 {
	if ( !ETEX_gpst_ImportData )
	{
		ETEX_gpst_ImportData = (ETEXT_tdst_ImportData *) L_malloc( sizeof( ETEXT_tdst_ImportData ) * 1000 );
		ETEX_i_ImportData_Nb = 0;
		ETEX_i_ImportData_Max = 1000;
	}
	else if (ETEX_i_ImportData_Nb == ETEX_i_ImportData_Max )
	{
		ETEX_i_ImportData_Max += 1000;
		ETEX_gpst_ImportData = (ETEXT_tdst_ImportData *) L_realloc( ETEX_gpst_ImportData, sizeof( ETEXT_tdst_ImportData ) * ETEX_i_ImportData_Max );
	}
	
	L_memset( &ETEX_gpst_ImportData[ETEX_i_ImportData_Nb], 0, sizeof( ETEXT_tdst_ImportData ) );
	ETEX_gpst_ImportData[ETEX_i_ImportData_Nb].ul_Key = _ul_Key;
	return ETEX_i_ImportData_Nb++;
 }
 /**/
 void ETEX_ImportData_SetText( int _i_Index, int _i_Lang, char *_sz_Text )
 {
	ETEX_gpst_ImportData[ _i_Index ].sz_Text[ _i_Lang ] = _sz_Text;
 }
 /**/
 int ETEX_ImportData_FindKey( ULONG _ul_Key )
 {
	int i;
	for (i = 0; i < ETEX_i_ImportData_Nb; i++)
	{
		if ( ETEX_gpst_ImportData[ i ].ul_Key == _ul_Key )
			return i;
	}
	return -1;
 }
 /**/
 void ETEX_ImportData_Delete( void )
 {
	int i,j;
	for (i = 0; i < ETEX_i_ImportData_Nb; i++)
	{
		for (j = 0; j < INO_e_MaxLangNb; j++)
			if ( ETEX_gpst_ImportData[i].sz_Text[j] )
				L_free(ETEX_gpst_ImportData[i].sz_Text[j]);
	}
	L_free( ETEX_gpst_ImportData );
	ETEX_gpst_ImportData = NULL;
 }
  

/*$4
 ***********************************************************************************************************************
	EXPORT
 ***********************************************************************************************************************
 */
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEXT_cl_Frame::i_UserChooseLanguageList(BOOL *ab_Lang)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString				oStr, o;
	EDIA_cl_ListDialog	o_ListDlg( EDIA_List_SelectMultipleLangage );
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(ab_Lang, 0, INO_e_MaxLangNb * sizeof(BOOL));
	o_ListDlg.mpi_Sel = ab_Lang;
	o_ListDlg.mi_SelNb = INO_e_MaxLangNb;
	
	return o_ListDlg.DoModal();
}


/*
 =======================================================================================================================
	load txl key store in a langage bank, assume _pul_Txl is an array of a minimum of INO_e_MaxLangNb ULONG£
	return TRUE if read succeed _pul_Txl contains index of txl file.
 =======================================================================================================================
 */
BOOL ETEXT_cl_Frame::Bank_Load( ULONG _ul_Index, ULONG *_pul_Txl )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ulSize, i;
	BIG_tdst_GroupElem	*pGroup;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if( _ul_Index == BIG_C_InvalidIndex)
		return FALSE;
	pGroup = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmp(BIG_PosFile(_ul_Index), (ULONG *) &ulSize);
	if (!pGroup)
		return FALSE;
		
	L_memset( _pul_Txl, 0, sizeof(ULONG) * INO_e_MaxLangNb );
	
	for(i = 0; i < ulSize / sizeof(BIG_tdst_GroupElem); i++, pGroup++)
	{
		if((pGroup->ul_Key == BIG_C_InvalidKey) || (pGroup->ul_Key == 0))
		{
			_pul_Txl[ i ] = BIG_C_InvalidIndex;
			continue;
		}
		
		if ( (i >= 0) && (i < INO_e_MaxLangNb) )
			_pul_Txl[ i ] = BIG_ul_SearchKeyToFat(pGroup->ul_Key);
	}
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#define M_OnExportAllHtml_AddColumn( type, lang )\
	{ ColumnType[ ColumnNb ] = type;\
	ColumnLang[ ColumnNb++ ] = lang; }
	
void ETEXT_cl_Frame::OnExportHtml( char *_sz_FileName )
{
	ULONG	ul_Txl, ul_Adr;
	ULONG	aul_Txl[ INO_e_MaxLangNb ];
	BOOL	ab_TxlLoaded[ INO_e_MaxLangNb ], b_Compare;
	int		i, j, lang, ColumnNb, ColumnType[ 200 ], ColumnLang[ 200 ], txl, idx;
	USHORT	*puw_CT, *puw_InvertCT;
	UCHAR	*puc_Char;
	char	sz_Val[ 16 ];
	FILE	*hpFile;
	CString	o, o2, o_Unicode;
	TEXT_tdst_OneText *pt_Ref, *pt_Cur, *pt_Save, *pt_Temp;
	char	sz_Text[ 100 ];
	
	/* save */
	pt_Save = mpt_EditText;
	mpt_EditText = NULL;
	
	/*$1 - import file to compare texte */
	b_Compare = FALSE;
	hpFile = fopen( _sz_FileName, "rt" );
	if ( hpFile )
	{
		fclose( hpFile );
		b_Compare = TRUE;
		OnImportHtml( _sz_FileName, NULL, TRUE );
	}
	
	/*$1- compute columns */
	ColumnNb = 0;
	M_OnExportAllHtml_AddColumn( en_TxgFileKey, -1 );
	for (i = 0; i < mi_LangNb; i++)
		M_OnExportAllHtml_AddColumn( en_TxlFileKey, i );
	M_OnExportAllHtml_AddColumn( en_IDKey, -1);
	
	if (b_Compare)
		M_OnExportAllHtml_AddColumn( 1000, -1);
	
	for ( i = 0; i < COLMAX; i++)
	{
		j = mst_Ini.ai_ColOrder[ i ];
		if (mst_Ini.ai_ColSize[ j ] <= 0)
			continue;
		j++;
		if (j == en_TxgFileKey || j == en_TxlFileKey || j == en_IDKey )
			continue;
			
		if((j == en_Preview) || (j == en_SndFileKey))
		{
			for (lang = 0; lang < mi_LangNb; lang++)
				M_OnExportAllHtml_AddColumn( j, lang );
		}
		else
			M_OnExportAllHtml_AddColumn( j, -1 );
	}
	
	/*$1- open file */
	hpFile = fopen( _sz_FileName, "wt" );
	if(!hpFile) 
	{
		o = CString( "Can't open file " ) + CString( _sz_FileName );
		o += ("\ncheck if not already open and retry" );
		MessageBox( LPCTSTR( o ), "Error", MB_OK | MB_ICONEXCLAMATION );
		return;
	}

	/* Unicode */
	if (mb_Unicode)
	{
		for (i = 0; i < ColumnNb; i++)
		{
			if ( ( ColumnType[ i ] == en_Preview ) && (ColumnLang[ i ] != -1) )
			{
				CharTable_Import( ColumnLang[ i ] );
				puw_CT = mapuw_CharTable[ ColumnLang[ i ] ];
				puw_InvertCT = (USHORT* ) L_malloc( 5000 * sizeof (USHORT ) );
				L_memset( puw_InvertCT, 0, 5000 * sizeof (USHORT ) );
				for (j = 0; j < 65536; j++)
				{
					if (*puw_CT)
						puw_InvertCT[ *puw_CT ] = j;
					puw_CT++;
				}
				L_free( mapuw_CharTable[ ColumnLang[ i ] ] );
				mapuw_CharTable[ ColumnLang[ i ] ] = puw_InvertCT;
			}
		}
	}
	
	/*$1- header ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	fprintf( hpFile, ETEXT_export_head );
	
	/*$1- name of columns ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	fprintf( hpFile, "<tr>\n" );
	for (j = 0; j < ColumnNb; j++)
	{
		if (ColumnType[ j ] == 1000 )
			fprintf( hpFile, "<td>compare</td>\n" );
		else if (ColumnLang[ j ] == -1 )
			fprintf( hpFile, "<td>%s</td>\n", mst_Ini.asz_ColName[ ColumnType[ j ] - 1 ] );
		else
			fprintf( hpFile, "<td>%s (%s)</td>\n", mst_Ini.asz_ColName[ ColumnType[ j ] - 1 ], INO_dst_LanguageName[ mai_Lang[ ColumnLang[ j ] ] ].az_ShortName );
	}
	fprintf( hpFile, "</tr>\n" );
	
	
	for( txl = 0; txl < mi_TxgNb; txl++)
	{
		/* find a langage bank, load it */
		if (! Bank_Load( mpul_TxgIndex[ txl ], aul_Txl ) )
			continue;
			
		/* load all text used */
		pt_Ref = NULL;
		for (i = 0; i < mi_LangNb; i++)
		{
			ul_Txl = aul_Txl[ mai_Lang[ i ]  ];
			if ( ( ul_Txl == BIG_C_InvalidIndex ) || (ul_Txl == 0) )
				continue;
			
			ul_Adr = LOA_ul_SearchAddress( BIG_PosFile( ul_Txl ) );
			ab_TxlLoaded[ mai_Lang[ i ] ] = FALSE;
			if ( ( ul_Adr == 0xFFFFFFFF ) || (ul_Adr == 0) )
			{
				ul_Adr = TEXT_ul_CallbackLoadTxl(BIG_PosFile( ul_Txl ), BIG_FileKey( ul_Txl ));
                TEXT_gp_CallbackLoadTxlForSound = (TEXT_tdst_OneText*)ul_Adr ;
                TEXT_ul_CallbackLoadTxlForSound(BIG_PosFile( ul_Txl ), BIG_FileKey( ul_Txl ));
                TEXT_gp_CallbackLoadTxlForSound = NULL;
				LOA_Resolve();

				if (ul_Adr)
					ab_TxlLoaded[ mai_Lang[ i ] ] = TRUE;
			}
			aul_Txl[ mai_Lang[ i ] ] = ul_Adr;
			if ( !pt_Ref )
				pt_Ref = (TEXT_tdst_OneText	*) ul_Adr;
		}
		if (!pt_Ref) continue;

		pt_Cur = pt_Ref;
		/*$1- contents ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		for(i = 0; i < (int) pt_Ref->ul_Num; i++)
		{
			fprintf( hpFile,"<tr>\n" );
		
			for (j = 0; j < ColumnNb; j++)
			{
				fprintf( hpFile, "<td>" );
			
				pt_Cur = (ColumnLang[ j ] == -1) ? pt_Ref : (TEXT_tdst_OneText	*) aul_Txl[ mai_Lang[ ColumnLang[j] ] ];
				if ( (pt_Cur == NULL) || ((ULONG) pt_Cur == 0xFFFFFFFF) )
				{
					if (ColumnType[ j ] == en_Preview)
						o = CString( "" );
					else
						o = CString( "0xFFFFFFFF" );
				}
				else
				{
					if ( ColumnType[ j ] == 1000 )
					{
						idx = ETEX_ImportData_FindKey( pt_Cur->pst_Ids[ i ].ul_IdKey );
						if ( idx == -1 )
							o = CString( "new" );
						else
						{
							o = CString( "" );
							for (lang = 0; lang < mi_LangNb; lang++)
							{
								if ( !ETEX_gpst_ImportData[idx].sz_Text[ mai_Lang[ lang ] ] ) continue;
								if ( !(pt_Temp = (TEXT_tdst_OneText *) aul_Txl[ mai_Lang[ lang ] ]) ) continue;
								if ( pt_Temp->pst_Ids[ i ].i_Offset == -1)
									o2 = CString( "" );
								else
								{
									o2 = CString( pt_Temp->psz_Text + pt_Temp->pst_Ids[ i ].i_Offset );
									o2.Replace( "…", "..." );
								}
								//if ( L_strcmp( ETEX_gpst_ImportData[idx].sz_Text[mai_Lang[ lang ]], pt_Temp->psz_Text + pt_Temp->pst_Ids[ i ].i_Offset ) )
								if ( L_strcmp( ETEX_gpst_ImportData[idx].sz_Text[mai_Lang[ lang ]], (char *) (LPCTSTR) o2 ) )
									o += CString("M") + CString( INO_dst_LanguageName[ mai_Lang[ lang] ].az_ShortName );
							}
						}
					}
					else if (ColumnType[ j ] == en_TxgFileKey)
					{
						sprintf( sz_Text, "0x%08X", BIG_FileKey( mpul_TxgIndex[ txl ] ));
						o = CString( sz_Text );
					}
					else
					{
						mpt_EditText = pt_Cur;
						o = CEL_GetText( i, ColumnType[ j ] );
						mpt_EditText = NULL;
					}
				}
			
				if ( (ColumnType[ j ] == en_Preview) && mb_ExportNoFormatCode)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					char	*p = new char[o.GetLength() + 1];
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					L_strcpy(p, o);
					STR_DelFormatString(p);
					o = CString(p);
					delete[] p;
				}

				o.Replace("&", "&amp;" );
				o.Replace("""", "&quot;" );
				o.Replace("<", "&lt;" );
				o.Replace(">", "&gt;" );
				o.Replace("\n", "<br>" );
				o.Replace(" ", "&nbsp;" );
				if (o == CString(""))
					o = CString( "&nbsp" );

				if ( (ColumnType[ j ] == en_Preview) && mb_Unicode)
				{
					puw_CT = mapuw_CharTable[ ColumnLang[ j ] ];
					puc_Char = (UCHAR *)( LPCTSTR ) o;
					o_Unicode = "";
					while ( *puc_Char)
					{
						if ( puw_CT[ *puc_Char ] <= 255 )
							o_Unicode += TCHAR( puw_CT[ *puc_Char ] );
						else
						{
							o_Unicode += "&#";
							o_Unicode += L_itoa( puw_CT[ *puc_Char ], sz_Val, 10 );
							o_Unicode += ";";
						}
						puc_Char++;
					}
					o = o_Unicode;
				}
				
				fprintf( hpFile, "%s", (char *) (LPCTSTR) o );
				fprintf( hpFile, "</td>\n" );
			}
			fprintf( hpFile, "</tr>\n" );
		}
		
		/*$1- free loaded text */
		for (i = 0; i < mi_LangNb; i++)
		{
			pt_Cur = (TEXT_tdst_OneText	*) aul_Txl[ mai_Lang[ i ]  ];
			if ( (pt_Cur == NULL) || ((ULONG) pt_Cur == 0xFFFFFFFF) )
				continue;
			if ( !ab_TxlLoaded[ mai_Lang[ i ] ] ) continue;
			TEXT_Free( pt_Cur );
		}
	}
	
	/*$1- tail ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	fprintf( hpFile, ETEXT_export_tail );

	/*$1- close ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	fclose( hpFile );
	
	/* restore */
	mpt_EditText = pt_Save;
	
	/* fere ressource for comparaison */
	if ( b_Compare ) 
		ETEX_ImportData_Delete();

	/* free ressource for unicode conversion */
	if (mb_Unicode)
	{
		for (i = 0; i < ColumnNb; i++)
		{
			if ( ( ColumnType[ i ] == en_Preview ) && (ColumnLang[ i ] != -1) )
			{
				puw_CT = mapuw_CharTable[ ColumnLang[ j ] ];
				if (puw_CT)
				{
					L_free( puw_CT );
					mapuw_CharTable[ ColumnLang[ j ] ] = NULL;
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnExportFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ulBankFat, ulFileFat;
	CString oStr;
	CString o1stCol, o, htmlname;
	EDIA_cl_Lang		o_Lang;
	int					i;
	EDIA_cl_NameDialog	o_Name("Enter the language list");
	BOOL				ab_Lang[INO_e_MaxLangNb];
	CString				o_Txl, oHeader, o_Html;
	CStringArray		o_ExportResult;
	CStringArray		o_TxlList;
	FILE				*hpFile;
    CList<ULONG, ULONG> o_SavBankList;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(i_UserChooseLanguageList(ab_Lang) != IDOK) return;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	
	ulBankFat = ul_GetCurBank();
	ulFileFat = ul_GetCurFile();

	if(ulBankFat == BIG_C_InvalidIndex) return;
	if(ulFileFat == BIG_C_InvalidIndex) return;
	
	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    computing file name
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */
	o = CString("Jade_ExportFile");
	mi_LangNb = 0;
	for(i = 0; i < INO_e_MaxLangNb; i++)
	{
		if(ab_Lang[i]) 
		{
			o += CString("_") + CString(INO_dst_LanguageName[i].az_ShortName);
			mai_Lang[ mi_LangNb++ ] = i;
		}
	}

	o += CString('_') + CString(BIG_NameFile(ulBankFat));
	//o += BIG_NameFile(ulFileFat);
	o.Replace(".", "_");
	o_Html = o + CString( ".htm" );
	
	/*$1- open file to test if not already opened */
	if ( _access( (char *) (LPCTSTR) o_Html, 0 ) != -1)
	{
		hpFile = fopen( (char *) (LPCTSTR) o_Html, "at" ); 
		if ( !hpFile )
		{
			o = CString( "Can't open file " ) + CString( (char *) (LPCTSTR) o_Html );
			o += ("\ncheck if not already open and retry" );
			MessageBox( LPCTSTR( o ), "Error", MB_OK | MB_ICONEXCLAMATION );
			return;
		}
		fclose( hpFile );
	}

	/*$1- get a save configuration ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	//OnFileSave();
	mi_SynchroLang = INO_e_French;
	OnSynchronize();
	mi_SynchroLang = INO_e_Dummy;
	GetEntryColumnInfo();
	SetEntryColumnInfo();
	//OnFileChange();

	/*$1- do export ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	mpul_TxgIndex = &ulBankFat;
	mi_TxgNb = 1;

	OnExportHtml( (char *) (LPCTSTR) o_Html );
	return;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnExportAll(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_Index;
	EDIA_cl_Lang		o_Lang;
	int					i;
	EDIA_cl_NameDialog	o_Name("Enter the language list");
	BOOL				ab_Lang[INO_e_MaxLangNb];
	CString				o, oStr, o_Txl, oHeader, o_Html;
	CStringArray		o_ExportResult;
	CStringArray		o_TxlList;
	FILE				*hpFile;
    CList<ULONG, ULONG> o_SavBankList;
    ULONG				ul_SavFile;
    ULONG				ul_SavBank;
    int					i_SavEntry;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(i_UserChooseLanguageList(ab_Lang) != IDOK) return;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	
	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    computing file name
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */
	o = CString("Jade_ExportAll");
	mi_LangNb = 0;
	for(i = 0; i < INO_e_MaxLangNb; i++)
	{
		if(ab_Lang[i]) 
		{
			o += CString("_") + CString(INO_dst_LanguageName[i].az_ShortName);
			mai_Lang[ mi_LangNb++ ] = i;
		}
	}
	o_Html = o + CString( ".htm" );
	
	/*$1- open file to test if not already opened */
	if ( _access( (char *) (LPCTSTR) o_Html, 0 ) != -1)
	{
		hpFile = fopen( (char *) (LPCTSTR) o_Html, "at" ); 
		if ( !hpFile )
		{
			o = CString( "Can't open file " ) + CString( (char *) (LPCTSTR) o_Html );
			o += ("\ncheck if not already open and retry" );
			MessageBox( LPCTSTR( o ), "Error", MB_OK | MB_ICONEXCLAMATION );
			return;
		}
		fclose( hpFile );
	}
	
	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    testing file opening
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */
    
    /*$1 save the current config */
    o_SavBankList.RemoveAll();
    ul_SavFile = ul_GetCurFile();
    ul_SavBank = ul_GetCurBank();
    i_SavEntry = -1;
    
    for(i=0; i<mpo_ListBank->GetCount(); i++)
    {
        o_SavBankList.AddTail(mpo_ListBank->GetItemData(i));
    }
	OnCloseAll();
	LINK_PrintStatusMsg("Exporting all files...");

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    get a safe config
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */
	mi_SynchroLang = INO_e_French;
	OnSynchronizeAll();
	mi_SynchroLang = INO_e_Dummy;
	GetEntryColumnInfo();
	SetEntryColumnInfo();

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    open file
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */
	mi_TxgNb = 0;
	mi_TxgMax = 100;
	mpul_TxgIndex = (ULONG *) L_malloc( sizeof( ULONG ) * mi_TxgMax );
	
	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(BIG_FileKey(ul_Index) == BIG_C_InvalidKey) continue;
		if(!BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtTextLang)) continue;
		
		if (mi_TxgNb == mi_TxgMax)
		{
			mi_TxgMax+= 100;
			mpul_TxgIndex = (ULONG *) L_realloc( mpul_TxgIndex, mi_TxgMax * sizeof( ULONG ) );
		}
		mpul_TxgIndex[ mi_TxgNb++ ] = ul_Index;
	}
	
	if (mi_TxgNb == 0)
	{
		LINK_PrintStatusMsg( "No txg file found" );
		L_free( mpul_TxgIndex );
		return;
	}
	
	OnExportHtml( (char *) (LPCTSTR) o_Html );
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::CFileReadLine(CFile *poFile, CString &o)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		az_Buff[4096], *pz;
	UINT		uiSize;
	ULONGLONG	ullPos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o = CString("");
	ullPos = poFile->GetPosition();

	do
	{
		uiSize = poFile->Read(az_Buff, 4096);
		if(!uiSize) return;

		pz = L_strchr(az_Buff, '\x0a');
		if(pz)
		{
			*pz++ = 0;
		}

		o += CString(az_Buff);
	} while(!pz && uiSize);

	if(pz)
	{
		ullPos = ullPos + o.GetLength() + 1 + 1;
		poFile->Seek(ullPos, CFile::begin);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::GetHeader(CString &o_Line, en_ColContent *ae_Usage, INO_tden_LanguageId *ae_LangId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	CString o_Word;
	int		i, col, j, k, lang;
	char	*pz;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	lang = i = col = 0;
	do
	{
		o_Word = o_Line.Tokenize("\t", i);
		if(i < 0) break;

		ae_Usage[col] = e_GetColTypeFromName(o_Word.GetBuffer(o_Word.GetLength() + 1));

		if(o_Word.Find(ETEXT_gasz_ColName[en_SndFileKey], 0) == 0)
		{
			ae_Usage[col] = en_SndFileKey;
		}
		else if(o_Word.Find(ETEXT_gasz_ColName[en_Preview], 0) == 0)
		{
			ae_Usage[col] = en_Preview;
			j = o_Word.Find("(", 0);
			pz = o_Word.GetBuffer(o_Word.GetLength() + 1);
			pz += j + 1;
			for(k = 0; k < INO_e_MaxLangNb; k++)
			{
				if(INO_dst_LanguageName[k].az_ShortName[0] != pz[0]) continue;
				if(INO_dst_LanguageName[k].az_ShortName[1] != pz[1]) continue;
				ae_LangId[lang++] = (INO_tden_LanguageId) k;
				break;
			}

			o_Word.ReleaseBuffer();
		}
		else if(ae_Usage[col] == en_UserData)
		{
			j = o_Word.Find("-", 0);
			pz = o_Word.GetBuffer(o_Word.GetLength() + 1);
			pz[j] = 0;
			sscanf(pz, "%d", &j);
			o_Word.ReleaseBuffer();
			*(int *) &ae_Usage[col] = (int) en_UserData + j;
		}

		col++;
	} while(i >= 0);
}

/*$4
 ***********************************************************************************************************************
	IMPORT
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::GetLineData
(
	CString				&o_Line,
	en_ColContent		*ae_Usage,
	INO_tden_LanguageId *ae_LangId,
	ULONG				&ulTxgFat,
	ULONG				*aulTxlFat,
	CStringArray		&o_EntryList,
	CUIntArray			&o_EntryKeyList,
	CStringArray		*ao_TextList,
	CStringArray		*ao_UserDataList
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		i, col, txl, lang;
	CString o_Word;
	char	*pz;
	UINT	ul = -1;
	BOOL	ab_Col[COLMAX * INO_e_MaxLangNb];
	BOOL	ab_Preview[INO_e_MaxLangNb];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ulTxgFat = -1;
	for(i = 0; i < COLMAX * INO_e_MaxLangNb; i++) ab_Col[i] = FALSE;
	for(i = 0; i < INO_e_MaxLangNb; i++) ab_Preview[i] = FALSE;

	for(lang = txl = i = col = 0; i >= 0; col++)
	{
		o_Word = o_Line.Tokenize("\t", i);
		if(i < 0) break;

		ERR_X_Assert((int) ae_Usage[col] < COLMAX);
		ERR_X_Assert((int) ae_Usage[col] >= 0);

		if(ab_Col[ae_Usage[col]]) continue;
		ab_Col[ae_Usage[col]] = TRUE;

		switch(ae_Usage[col])
		{
		case en_TxgFileKey:
			pz = o_Word.GetBuffer(o_Word.GetLength() + 1);
			sscanf(pz, "0x%x", &ulTxgFat);
			ulTxgFat = BIG_ul_SearchKeyToFat(ulTxgFat);
			o_Word.ReleaseBuffer();
			break;

		case en_TxlFileKey:
			pz = o_Word.GetBuffer(o_Word.GetLength() + 1);
			sscanf(pz, "0x%x", &aulTxlFat[txl]);
			aulTxlFat[txl] = BIG_ul_SearchKeyToFat(aulTxlFat[txl]);
			o_Word.ReleaseBuffer();
			txl++;
			break;

		case en_IDKey:
			pz = o_Word.GetBuffer(o_Word.GetLength() + 1);
			sscanf(pz, "0x%x", &ul);
			o_Word.ReleaseBuffer();
			o_EntryKeyList.Add(ul);
			break;

		case en_ID:
			o_EntryList.Add(o_Word);
			break;

		case en_Preview:
			ERR_X_Warning(ae_LangId[lang] != INO_e_Dummy, "error", NULL);

			if(ab_Preview[ae_LangId[lang]]) break;
			ao_TextList[ae_LangId[lang]].Add(o_Word);
			ab_Preview[ae_LangId[lang]] = TRUE;

			ab_Col[ae_Usage[lang]] = FALSE;
			lang++;
			break;

		default:
			if((en_UserData <= ae_Usage[col]) && (ae_Usage[col] < en_MaxNb))
			{
				ao_UserDataList[ae_Usage[col]].Add(o_Word);
				break;
			}
			break;
		}
	}
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
#define HTMLTOKEN_Comment	-2
#define HTMLTOKEN_Unknow	-1
#define HTMLTOKEN_EOF		0
#define HTMLTOKEN_TABLE		1
#define HTMLTOKEN_EOTABLE	2
#define HTMLTOKEN_TR		3
#define HTMLTOKEN_EOTR		4
#define HTMLTOKEN_TD		5
#define HTMLTOKEN_EOTD		6
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */
char ETEXT_cl_Frame::OnImportHtml_GetChar( int _i_Token )
{
	if ( mst_Import.pc_BufCur == mst_Import.pc_BufEnd )
	{
		mst_Import.pc_BufEnd = mst_Import.pc_BufStart + fread( mst_Import.pc_BufStart, 1, 2048, mst_Import.ph_File);
		mst_Import.pc_BufCur = mst_Import.pc_BufStart;
		if (mst_Import.pc_BufEnd == mst_Import.pc_BufStart)
		{
			mst_Import.c_Char = 0;
			return 0;
		}
	}
	mst_Import.c_Char = *mst_Import.pc_BufCur++;
	
	if( _i_Token && mpc_TokenContent && (mpc_TokenContentCur < mpc_TokenContentEnd) )
		*mpc_TokenContentCur++ = mst_Import.c_Char;

	return mst_Import.c_Char;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEXT_cl_Frame::OnImportHtml_GetToken( void )
{
	static char	sz_Colspan[] = "colspan=";
	char sz_Token[ 256 ], *sz_Cur, sz_Com[ 4 ];
	int	 end, i_Token, i_colspan;
	
	
	do
	{
		OnImportHtml_GetChar( 1 );
		
	} while (( mst_Import.c_Char != 0 ) && (mst_Import.c_Char != '<'));
	if (mst_Import.c_Char == 0)
		return HTMLTOKEN_EOF;
		
	sz_Cur = sz_Token;
	end = 1;
	
	OnImportHtml_GetChar( 1 );
	
	if (mst_Import.c_Char != '/' )
	{
		end = 0;
		*sz_Cur++ = mst_Import.c_Char;
	}

	/* special for comment */	
	if ( mst_Import.c_Char == '!' )
	{
		OnImportHtml_GetChar( 1 );
		if (mst_Import.c_Char != '-') goto OnImportHtml_GetToken_EndComment;
		OnImportHtml_GetChar( 1 );
		if (mst_Import.c_Char != '-') goto OnImportHtml_GetToken_EndComment;
		L_strcpy( sz_Com, "   " );
		if( mpc_TokenContent && (mpc_TokenContentCur < mpc_TokenContentEnd) )
			*mpc_TokenContentCur++ = '>';
		
		while( 1 )
		{
			OnImportHtml_GetChar( 0 );
			if (mst_Import.c_Char == 0)
				return HTMLTOKEN_EOF;
			
			sz_Com[ 0 ] = sz_Com[ 1 ];
			sz_Com[ 1 ] = sz_Com[ 2 ];
			sz_Com[ 2 ] = mst_Import.c_Char;
			
			if ( !L_strcmp( sz_Com, "-->") )
					return HTMLTOKEN_Unknow;
			
		}
	}
	
OnImportHtml_GetToken_EndComment:

	while (1)
	{
		OnImportHtml_GetChar( 1 );

		if (mst_Import.c_Char == 0)
			return HTMLTOKEN_EOF;
		mst_Import.c_Char = tolower( mst_Import.c_Char );
		if (( mst_Import.c_Char < 'a') || (mst_Import.c_Char > 'z') )
		{
			*sz_Cur = 0;
			break;
		}
		*sz_Cur++ = mst_Import.c_Char;
	}
	
	i_Token = HTMLTOKEN_Unknow;
	if (sz_Token[ 0 ] == '!')
		return HTMLTOKEN_Comment;
	if (sz_Token[ 0 ] == 't')
	{
		switch( sz_Token[ 1 ] )
		{
			case 'a': 
				if (L_strcmp( sz_Token + 2, "ble") == 0) 
					i_Token = HTMLTOKEN_TABLE + end;
				break;
			case 'd':
				if (sz_Token[2] == 0)
					i_Token = HTMLTOKEN_TD + end;
				break;
			case 'r':
				if (sz_Token[2] == 0)
					i_Token = HTMLTOKEN_TR + end;
				break;
		}	
	}
	
	i_colspan = 0;
	mi_TokenParam = 0;
	
	while ( mst_Import.c_Char != '>' )
	{
		OnImportHtml_GetChar( 1 );

		if ( (i_Token == HTMLTOKEN_TD) && (i_colspan != -1) )
		{
			if ( sz_Colspan[ i_colspan ] == 0 )
			{
				if (isdigit( mst_Import.c_Char ))
					mi_TokenParam = (mi_TokenParam * 10) + (mst_Import.c_Char - '0');
				else 
					i_colspan = -1;
			}
			else if (mst_Import.c_Char == sz_Colspan[ i_colspan ] )
				i_colspan++;
			else
				i_colspan = 0;
		}
		if (mst_Import.c_Char == 0)
			return HTMLTOKEN_EOF;
	}
	
	return i_Token;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int OnImportHtml_ParseContent_GetSpan( char *_sz_Content )
{
	char	*sz_Cur;

	sz_Cur = _sz_Content + 4;
	while ( (*sz_Cur == ' ') || (*sz_Cur == 0xA) || (*sz_Cur == 0xD) ) sz_Cur++;
	if ( !*sz_Cur ) 
		return SPAN_UNKNOW;

	if ( !L_strcmp( sz_Cur, "style='mso-spacerun:yes'" ) )
		return SPAN_SPACERUN;
	if ( !L_strcmp( sz_Cur, "style='display:none'" ) )
		return SPAN_DISPLAYNONE;

	return SPAN_UNKNOW;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int OnImportHtml_GetNextToken( char *sz_Content, char *sz_Token )
{
	char *sz_Cur, *sz_Start;

	sz_Cur = sz_Content;
	*sz_Token = 0;
	while ( (*sz_Cur != '<') && *sz_Cur )
		sz_Cur++;
	if (!*sz_Cur) return 0;
	sz_Start = sz_Cur + 1;
	while (*sz_Cur && (*sz_Cur != '>') )
		sz_Cur++;
	if (!*sz_Cur) return 0;
	*sz_Cur = 0;
	L_strcpy( sz_Token, sz_Start );
	*sz_Cur = '>';
	return sz_Cur - sz_Content + 1;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnImportHtml_ParseContent( char *sz_Content )
{
	char	*sz_Cur, *sz_Copy, c_Cur, *sz_End;
	char	sz_NextToken[ 1024 ];
	USHORT	uw_Ascii;
	char	i_RC, i_Nb, i_SpecialCode, i_Span, i_DisplayNone, i_SpaceRun;
	
	i_RC = 0;
	sz_Copy = sz_Cur = sz_Content;
	i_DisplayNone = 0;
	i_SpaceRun = 0;
	
	while( *sz_Cur != 0 )
	{
		c_Cur = *sz_Cur++;
		switch (c_Cur)
		{
			case '\r':
			case '\n':
				while (*sz_Cur == ' ') sz_Cur++;
				i_RC = 2;
				break;
			case '\t':
				break;
			case '&':
				sz_End = sz_Cur;
				while ( *sz_End && *sz_End != ';' && *sz_End != ' ') sz_End++;
				if ( !*sz_End )
					goto OnImportHtml_ParseContent_End;
				*sz_End = 0;
				for( i_SpecialCode = 0; ; i_SpecialCode++ )
				{
					if ( !ETEX_gast_SpecialString[ i_SpecialCode ].uc_Char )
						break;
					if (!L_strcmp( sz_Cur, ETEX_gast_SpecialString[ i_SpecialCode ].sz_Code ))
					{
						if (i_RC) *sz_Copy++ = ' ';
						*sz_Copy++ = ETEX_gast_SpecialString[ i_SpecialCode ].uc_Char;
						break;
					}
				}
				if ( !ETEX_gast_SpecialString[ i_SpecialCode ].uc_Char && (*sz_Cur == '#') )
				{
					if (i_RC) *sz_Copy++ = ' ';
					sz_Cur++;
					uw_Ascii = 0;
					while ( isdigit( *sz_Cur) )
						uw_Ascii = (uw_Ascii * 10) + (*sz_Cur++ -'0');
					*sz_Copy++ = ((uw_Ascii >= 32)&& (uw_Ascii < 256)) ? (uw_Ascii & 0xFF) : 32; 
				}
				sz_Cur = sz_End + 1;
				break;
			case '<':
				sz_End = sz_Cur;
				while ( *sz_End && *sz_End != '>') sz_End++;
				if ( !*sz_End )
					goto OnImportHtml_ParseContent_End;
				*sz_End = 0;
				if ( !L_strcmp( sz_Cur, "/span" ) )
				{
					i_DisplayNone = 0;
					i_SpaceRun = 0;
					sz_Cur = sz_End + 1;
				}
				else if ( !L_strnicmp( sz_Cur, "span", 4 ) )
				{
					i_Span = OnImportHtml_ParseContent_GetSpan( sz_Cur );
					switch (i_Span )
					{
					case SPAN_SPACERUN:		i_SpaceRun = 1; break;
					case SPAN_DISPLAYNONE:	i_DisplayNone = 1; break;
					}
					sz_Cur = sz_End + 1;
				}
				if ( !L_strcmp( sz_Cur, "br" ) )
				{
					sz_Cur = sz_End + 1;
					while ((*sz_Cur == ' ') || (*sz_Cur == '\n' ) ) sz_Cur++;
					*sz_Copy++ = '\n';
				}
				else
				{
					i_Nb = OnImportHtml_GetNextToken( sz_End + 1, sz_NextToken );
					if ((*sz_NextToken == '/') && !L_strcmp( sz_NextToken + 1, sz_Cur) )
						sz_Cur = sz_End + 1 + i_Nb;
					else
						sz_Cur = sz_End + 1;
				}
				break;
			default:
				if (i_RC) *sz_Copy++ = ' ';
				if (i_SpaceRun && ((unsigned char) c_Cur == 0xA0) )
					c_Cur = ' ';
				*sz_Copy++ = c_Cur;
		}
		if (--i_RC < 0) i_RC = 0;
	}
OnImportHtml_ParseContent_End:
	*sz_Copy = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnImportHtml_ParseContent_Unicode( char *sz_Content, USHORT *uw_Unicode )
{
	USHORT		*uw_Cur;
	char		*sz_Cur, c_Cur, *sz_End;
	char		i_RC;
	int			i_SpecialCode, i_Span, i_DisplayNone, i_SpaceRun;
	
	i_RC = 0;
	sz_Cur = sz_Content;
	uw_Cur = uw_Unicode;
	i_DisplayNone = 0;
	i_SpaceRun = 0;
	
	while( *sz_Cur != 0 )
	{
		c_Cur = *sz_Cur++;
		switch (c_Cur)
		{
			case '\r':
			case '\n':
				while (*sz_Cur == ' ') sz_Cur++;
				i_RC = 2;
				break;
			case '\t':
				break;
			case '&':
				sz_End = sz_Cur;
				while ( *sz_End && *sz_End != ';' && *sz_End != ' ') sz_End++;
				if ( !*sz_End )
					goto OnImportHtml_ParseContent_Unicode_End;
				*sz_End = 0;
				for( i_SpecialCode = 0; ; i_SpecialCode++ )
				{
					if ( !ETEX_gast_SpecialString[ i_SpecialCode ].uc_Char )
						break;
					if (!L_strcmp( sz_Cur, ETEX_gast_SpecialString[ i_SpecialCode ].sz_Code ))
					{
						if ( !i_DisplayNone )
						{
							if (i_RC && !i_SpaceRun) *uw_Cur++ = ' ';
							*uw_Cur++ = ETEX_gast_SpecialString[ i_SpecialCode ].uc_Char;
						}	
						break;
					}
				}
				if ( !ETEX_gast_SpecialString[ i_SpecialCode ].uc_Char && (*sz_Cur == '#') )
				{
					if (!i_DisplayNone)
					{
						if (i_RC && !i_SpaceRun) *uw_Cur++ = ' ';
						sz_Cur++;
						*uw_Cur = 0;
						while ( isdigit(*sz_Cur) )
							*uw_Cur = (*uw_Cur * 10) + (*sz_Cur++ -'0');
						uw_Cur++;
					}
					else
					{
						sz_Cur++;
						while ( isdigit(*sz_Cur) ) sz_Cur++;
					}
				}
				sz_Cur = sz_End + 1;
				break;
			case '<':
				sz_End = sz_Cur;
				while ( *sz_End && *sz_End != '>') sz_End++;
				if ( !*sz_End )
					goto OnImportHtml_ParseContent_Unicode_End;
				*sz_End = 0;
				
				if ( !L_strcmp( sz_Cur, "/span" ) )
				{
					i_DisplayNone = 0;
					sz_Cur = sz_End + 1;
				}
				else if ( !L_strnicmp( sz_Cur, "span", 4 ) )
				{
					i_Span = OnImportHtml_ParseContent_GetSpan( sz_Cur );
					switch (i_Span )
					{
					case SPAN_SPACERUN:		i_SpaceRun = 1; break;
					case SPAN_DISPLAYNONE:	i_DisplayNone = 1; break;
					}
					sz_Cur = sz_End + 1;
				}
				else if ( !L_strcmp( sz_Cur, "br" ) )
				{
					sz_Cur = sz_End + 1;
					while ((*sz_Cur == ' ') || (*sz_Cur == '\n' ) ) sz_Cur++;
					if (!i_DisplayNone)
						*uw_Cur++ = '\n';
				}
				else
					sz_Cur = sz_End + 1;
				break;
			default:
				if (!i_DisplayNone)
				{
					if (i_RC && !i_SpaceRun) *uw_Cur++ = ' ';
					*uw_Cur++ = (UCHAR) c_Cur;
				}
		}
		if (--i_RC < 0) i_RC = 0;
	}
OnImportHtml_ParseContent_Unicode_End:
	*uw_Cur = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEXT_cl_Frame::OnImportHtml_GetLine( char *asz_Content[], USHORT *auw_Content[], int _i_NbContent, int _i_SizeContent )
{
	int		i_Line, i, i_Cur, i_Comment, i_colspan;
	
	for (i = 0; i < _i_NbContent; i++)
		*asz_Content[ i ] = 0;
	i_Cur = 0;
	i_Line = 0;
	i_Comment = 0;
	mpc_TokenContent = NULL;
		
	while ( 1 )
	{
		i = OnImportHtml_GetToken();
		
		if (i_Comment)
		{
			if (i == HTMLTOKEN_Comment)
				i_Comment = 0;
			else if (i == HTMLTOKEN_EOF)
				return -1;
		}
		else
		{
			switch ( i ) 
			{
				case HTMLTOKEN_EOF : 
					return -1;
					
				case HTMLTOKEN_Comment:
					i_Comment = 1;
					break;
				
				case HTMLTOKEN_TR:
					if (i_Line) return -1;
					i_Line = 1;
					break;
				
				case HTMLTOKEN_EOTR:
					if (!i_Line) return -1;
					return i_Cur;
					
				case HTMLTOKEN_TD:
					if (!i_Line) return -1;
					i_colspan = mi_TokenParam;
					if (mi_TokenParam == 0)
					{
						if (i_Cur < _i_NbContent)
						{
							mpc_TokenContent = mpc_TokenContentCur = asz_Content[ i_Cur ];
							mpc_TokenContentEnd = asz_Content[ i_Cur ] + _i_SizeContent;
						}
						else
							mpc_TokenContent = mpc_TokenContentCur = NULL;
					}
					break;
					
				case HTMLTOKEN_EOTD:
					if (!i_Line) return -1;
					if (i_colspan)
					{
						i_Cur += i_colspan;
					}
					else 
					{
						if ( mpc_TokenContent ) 
						{
							mpc_TokenContentCur -= 5;
							*mpc_TokenContentCur = 0;
							if (mb_Unicode && auw_Content && auw_Content[ i_Cur ] )
							{
								if (i_Cur < _i_NbContent )
									OnImportHtml_ParseContent_Unicode(mpc_TokenContent, auw_Content[ i_Cur ] );
							}
							else
								OnImportHtml_ParseContent( mpc_TokenContent );
							mpc_TokenContent = NULL;
						}
						i_Cur++;
					}
					break;
			}
		}
	}
	
	return i_Cur;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnImportHtml( char *sz_FileName, BOOL *_pb_Column, BOOL _b_ForExport )
{

	int					i, j, k ,idx, ColNb, ColCur, ColError, LineNb, LineError, Lang;
	char				*asz_Content[ 100 ], *sz_Lang;
	USHORT				*auw_Content[ 100 ], *puw_Cur, *puw_CT;
	char				sz_Msg[ 512 ];
	int					ColType[ 200 ], ColLang[ 200 ], ColImport[ 200 ];
	ULONG				ul_LineBank, ul_CurBank, ul_TxlRefIndex, ul_Key;
	ULONG				aul_TxlIndex[ INO_e_MaxLangNb ];
	TEXT_tdst_OneText	*apst_TxlAddr[ INO_e_MaxLangNb ], *pst_TxlRef, *pst_SaveEdit;
	
	pst_SaveEdit = mpt_EditText;
	
	mst_Import.ph_File = fopen( sz_FileName, "rt" );
	if ( !mst_Import.ph_File )
	{
		CString o_Error;
		o_Error = CString( "Cannot open file : ") + CString( sz_FileName );
		o_Error += "\ncheck if not already used and retry";
		MessageBox( LPCTSTR( o_Error ), "Error", MB_OK | MB_ICONEXCLAMATION );
		return;
	}
	
	/* init, alloc data */
	for (i = 0; i < 100; i++)
	{
		asz_Content[ i ] = (char *) L_malloc( 2048 );
		auw_Content[ i ] = NULL;
	}
	mst_Import.pc_BufStart = (char *) L_malloc( 2048 );
	mst_Import.pc_BufEnd = mst_Import.pc_BufCur = mst_Import.pc_BufStart;
		
	/* try to get header */
	LINK_PrintStatusMsg( "ImportText -> reading header" );
	ColNb = OnImportHtml_GetLine( asz_Content, NULL, 100, 2048 );
	ColError = 0;
	for (i = 0; i < ColNb; i++)
	{
		/* langage */
		Lang = -1;
		if (strlen( asz_Content[ i ]) == 0 )
		{
			sprintf( sz_Msg, "Column %d has no name : ignored", i );
			LINK_PrintStatusMsg( sz_Msg );
			ColType[ i ] = -1;
			continue;
		}
		else if (strlen( asz_Content[ i ] ) > 5)
		{
			sz_Lang = asz_Content[ i ] + strlen( asz_Content[ i ] ) - 5;
			if ( ( sz_Lang[0] == ' ' ) && (sz_Lang[1] == '(') && (sz_Lang[4] == ')' ) )
			{
				for ( k = 0; k <  INO_e_MaxLangNb; k++)
				{
					if ( !_strnicmp( sz_Lang + 2, INO_dst_LanguageName[k].az_ShortName, 2 ) )
					{
						Lang = k;
						*sz_Lang = 0;
						break;
					}
				}
			}
		}
		/* type */
		if ( !L_strcmp(	asz_Content[ i ], "compare" ) )
		{
			ColType[ i ] = 1000;
			ColLang[ i ] = -1;
		}
		else
		{
			for (j = 0; j < COLMAX; j++ )
			{
				if ( !L_strcmp( asz_Content[ i ], mst_Ini.asz_ColName[ j ] ) )
				{
					ColType[ i ] = j + 1;
					ColLang[ i ] = Lang;
					sprintf( sz_Msg, "Column %d recognized : type %d (%s) %s", i, j, mst_Ini.asz_ColName[ j ], Lang == -1 ? "" : INO_dst_LanguageName[Lang].az_LongName );
					LINK_PrintStatusMsg( sz_Msg );
					break;
				}
			}
			if ( j == COLMAX )
			{
				sprintf( sz_Msg, "Column not recognized %d (%s)", i, asz_Content[ i ] );
				LINK_PrintStatusMsg( sz_Msg );
				ColError = 1;
			}
		}
	}
	
	if ( ColError )
	{
		LINK_PrintStatusMsg( "Error while trying to get column type" );
		goto OnImportHtml_End;
	}
	
	/* check order of column */
	if (ColType[ 0 ] != en_TxgFileKey )
	{
		LINK_PrintStatusMsg( "Error : first column must be txg file key" );
		goto OnImportHtml_End;
	}
	
	mi_LangNb = 0;
	i = 1;
	while ( ColType[ i ] == en_TxlFileKey )
	{
		mai_Lang[ mi_LangNb++ ] = ColLang[ i ];
		if (mi_LangNb >= INO_e_MaxLangNb)
		{
			LINK_PrintStatusMsg( "Error : too many langage" );
			goto OnImportHtml_End;
		}
		i++;
	}
	if (mi_LangNb == 0)
		LINK_PrintStatusMsg( "Error : no txl file key column found" );
	
	if (ColType[ i ] != en_IDKey)
	{
		LINK_PrintStatusMsg( "Error : column after txl must be entry key" );
		goto OnImportHtml_End;
	}
	
	for ( ; i < ColNb; i++)
	{
		if (ColType[ i ] == -1) continue;
		if (ColType[ i ] == en_TxlFileKey )
		{
			LINK_PrintStatusMsg( "Error : txl column have to follow txg column" );
			goto OnImportHtml_End;
		}
		if (ColLang[ i ] != -1)
		{
			for (j = 0; j < mi_LangNb; j++)
				if (ColLang[ i ] == mai_Lang[ j ] )
					break;
			if (j == mi_LangNb )
			{
				LINK_PrintStatusMsg( "Error : find langage in some column but not in txl column" );
				goto OnImportHtml_End;
			}
		}
	}
	/* No Error continue */

	/* Ask for column to import */
	if (!_b_ForExport && _pb_Column )
	{
		CCheckList			o_Check;
		CRect				o_Rect;
		int					Col, ColIndex[ 200 ], ColBool[ 200 ];

		L_memset( ColImport, 0, sizeof( ColImport ) );
		L_memset( ColBool, 0, sizeof( ColImport ) );
		Col = 0;

		for (i = 0; i < ColNb; i++)
		{
			if (ColType[ i ] >= 1000) continue;
			
			j = ColType[ i ] - 1;
			Lang = ColLang[ i ];
			sprintf( sz_Msg, "%s %s", mst_Ini.asz_ColName[ j ], Lang == -1 ? "" : INO_dst_LanguageName[ Lang ].az_LongName );
			if ( ( ColType[ i ] == en_Preview ) && (ColLang[ i ] != -1) )
			{
				o_Check.AddString( sz_Msg );
				ColBool[ Col ] = 1;
				ColIndex[ Col ] = i;
				Col++;
			}
			else if ( ColType[ i ] >= en_UserData )
			{
				o_Check.AddString( sz_Msg );
				ColIndex[ Col ] = i;
				Col++;
			}
		}

		o_Check.SetArrayBool( ColBool );

		mpo_MyView->GetWindowRect(&o_Rect);
		o_Rect.left += (o_Rect.Width() / 2) - 100;
		o_Rect.top += (o_Rect.Height() / 2) - 100;
		o_Check.Do( &o_Rect, 200, this, 20, "column to import");

		for (i = 0; i < Col; i++)
			ColImport[ ColIndex[ i ] ] = ColBool[ i ];
	
		for (i = 0; i < ColNb; i++)
			if (ColImport[ i ] == 1) break;
		if (i == ColNb)
			goto OnImportHtml_End;
	}

	
	/* Unicode */
	if (mb_Unicode)
	{
		for (i = 0; i < ColNb; i++)
		{
			if ( ( ColType[ i ] == en_Preview ) && (ColLang[ i ] != -1) )
			{
				if (ColImport[ i ])
				{
					CharTable_Import( ColLang[ i ] );
					if ( mapuw_CharTable[ ColLang[ i ] ] )
						auw_Content[ i ] = (USHORT *) L_malloc( 2048 * sizeof( USHORT ) );
				}
			}
		}
	}
	
	LineNb = 0;
	LineError = 0;
	ul_CurBank = BIG_C_InvalidKey;
	while ( 1 )
	{
		ColError = OnImportHtml_GetLine( asz_Content, auw_Content, 100, 2048 );
		if ( ColError == -1 )
		{
			if (!_b_ForExport)
			{
				if (ul_CurBank != BIG_C_InvalidKey)
				{
					for (i = 0; i < INO_e_MaxLangNb; i++)
					{
						if ( !apst_TxlAddr[ i ]  ) continue;
						SaveFile( apst_TxlAddr[ i ] );
						TEXT_Free( apst_TxlAddr[ i ] );
						if ( i != 0 )
							Synchronize( aul_TxlIndex[ i ], ul_TxlRefIndex);
					}
				}
			}
			sprintf( sz_Msg, "End Of Parsing, %d line of data found, %d errors", LineNb, LineError );
			LINK_PrintStatusMsg( sz_Msg );
			goto OnImportHtml_End;
		}
		LineNb++;
		if ( ColError != ColNb )
		{
			sprintf( sz_Msg, "line[%d] : different number of column", LineNb );
			LINK_PrintStatusMsg( sz_Msg );
			LineError++;
			continue;
		}
		
		/* get bank */
		sscanf( asz_Content[ 0 ], "0x%x", &ul_Key );
		ul_LineBank = BIG_ul_SearchKeyToFat( ul_Key );
		if (ul_LineBank == BIG_C_InvalidIndex )
		{
			sprintf( sz_Msg, "line[%d] : bad bank key (%08X)", LineNb, ul_Key );
			LINK_PrintStatusMsg( sz_Msg );
			LineError++;
			continue;
		}
		
		if (!_b_ForExport)
		{
			if (ul_LineBank != ul_CurBank)
			{
				if (ul_CurBank != BIG_C_InvalidKey)
				{
					for (i = 0; i < INO_e_MaxLangNb; i++)
					{
						if ( !apst_TxlAddr[ i ]  ) continue;
						SaveFile( apst_TxlAddr[ i ] );
						TEXT_Free( apst_TxlAddr[ i ] );
						if ( i != 0 )
							Synchronize( aul_TxlIndex[ i ], ul_TxlRefIndex);
					}
				}
				ul_CurBank = ul_LineBank;
				Bank_Load( ul_LineBank, aul_TxlIndex );
				L_memset( apst_TxlAddr, 0, sizeof( apst_TxlAddr ) );
				ul_TxlRefIndex = BIG_C_InvalidIndex;
			}
		}
		
		/* get used txl */
		for ( ColCur = 1, i = 0; i < mi_LangNb; i++, ColCur++ )
		{
			sscanf( asz_Content[ ColCur ], "0x%x", &ul_Key );
			if (!_b_ForExport)
			{
				if (ul_Key != BIG_FileKey( aul_TxlIndex[ ColLang[ ColCur ] ] ) ) 
				{
					sprintf( sz_Msg, "line[%d] : bad txl key (%08X)", LineNb, ul_Key );
					LINK_PrintStatusMsg( sz_Msg );
					LineError++;
					break;
				}
				if ( apst_TxlAddr[ ColLang[ ColCur ] ] == NULL )
				{
					ul_Key = aul_TxlIndex[ ColLang[ ColCur ] ];
					apst_TxlAddr[ ColLang[ ColCur ] ] = (TEXT_tdst_OneText *) TEXT_ul_CallbackLoadTxl(BIG_PosFile( ul_Key ), BIG_FileKey( ul_Key ));

                    TEXT_gp_CallbackLoadTxlForSound = apst_TxlAddr[ ColLang[ ColCur ] ];
                    TEXT_ul_CallbackLoadTxlForSound(BIG_PosFile( ul_Key ), BIG_FileKey( ul_Key ));
                    TEXT_gp_CallbackLoadTxlForSound = NULL;

                    if(ul_TxlRefIndex == BIG_C_InvalidIndex)
					{
						ul_TxlRefIndex = aul_TxlIndex[ ColLang[ ColCur ] ];
						pst_TxlRef = apst_TxlAddr[ ColLang[ ColCur ] ];
					}
				}
			}
		}
		if (i != mi_LangNb) 
			continue;
		
	
		/* get entry key */
		sscanf( asz_Content[ ColCur ], "0x%x", &ul_Key );
		
		if ( _b_ForExport )
		{
			char *sz_NewContent, *src, *tgt;
			
			idx = ETEX_ImportData_Add( ul_Key );
			for ( ; ColCur < ColNb; ColCur++ )
			{
				if ( ColType[ ColCur ] != en_Preview ) continue;
				if( ColLang[ ColCur ] == -1 ) continue;
				if ( !L_strchr( asz_Content[ ColCur ], '…' ) )
				{
					sz_NewContent = (char *) L_malloc( strlen(asz_Content[ ColCur ]) + 1 );
					L_strcpy( sz_NewContent, asz_Content[ ColCur ] );
				}
				else
				{
					sz_NewContent = (char *) L_malloc( strlen(asz_Content[ ColCur ]) * 3 );
					src = asz_Content[ ColCur ];
					tgt = sz_NewContent;
					do
					{
						if (*src == '…')
						{
							*tgt++ = '.';
							*tgt++ = '.';
							*tgt++ = '.';
						}
						else
							*tgt++ = *src;
					} while (*src++);
				}
				ETEX_ImportData_SetText( idx, ColLang[ ColCur ], sz_NewContent );
			}
		}
		else
		{	
			/* get entry id in txl file */
			idx = TEXT_i_GetEntryIndex(pst_TxlRef, ul_Key );
			if ( (idx < 0) || (idx >= (int)pst_TxlRef->ul_Num) )
			{
				sprintf( sz_Msg, "line[%d] : bad id key (%08X)", LineNb, ul_Key );
				LINK_PrintStatusMsg( sz_Msg );
				LineError++;
				continue;
			}
			/* get other column */
			for ( ; ColCur < ColNb; ColCur++ )
			{
				if (ColType[ ColCur ] == 1000 ) continue;
				//if ( !_pb_Column[ ColType[ ColCur ] ] ) continue;
				if ( !ColImport[ ColCur ] ) continue;
				
				
				if ( ColType[ ColCur ] == en_Preview )
				{
					if ( (ColLang[ ColCur ] == -1) || !apst_TxlAddr[ ColLang[ ColCur ] ])
						break;
						
					if ( mb_Unicode && auw_Content[ ColCur ] && auw_Content[ ColCur ][0] )
					{
						char *sz_NewContent, *tgt;
						int	n, r;
						//tgt = sz_NewContent = (char *) L_malloc( strlen(asz_Content[ ColCur ]) * 3 );
						tgt = sz_NewContent = (char *) L_malloc( wcslen((const wchar_t*)auw_Content[ ColCur ])*2+2+2);
						sz_NewContent[wcslen((wchar_t *)auw_Content[ ColCur ])*2+2]=0;
						sz_NewContent[wcslen((wchar_t *)auw_Content[ ColCur ])*2+2+1]=0;

						
						puw_CT = mapuw_CharTable[ ColLang[ ColCur ] ];
						puw_Cur = auw_Content[ ColCur ];
						while ( *puw_Cur )
						{
							if ( puw_CT[ *puw_Cur ] & 0xFF00 )
								break;
							puw_Cur++;
						}
						/* some unicode character => convert all string */
						if ( *puw_Cur )
						{
							//ADDED 07112005
							puw_Cur = auw_Content[ ColCur ];
							// ENDADDED
							*tgt++ = '\\';
							*tgt++ = 'u';
										
							while ( *puw_Cur)
							{
								if (*puw_Cur == '\\')
								{
									*tgt++ = '\\';
									puw_Cur++;
									if ( (*puw_Cur == 'n') || (*puw_Cur == '-') )
										*tgt++ = *puw_Cur++ & 0xFF;
									else if (*puw_Cur == 'd')
									{
										*tgt++ = *puw_Cur++ & 0xFF;
										*tgt++ = *puw_Cur++ & 0xFF;
									}
									else 
									{
										while ( 1 )
										{
											*tgt++ = *puw_Cur & 0xFF;
											if (*puw_Cur == 0) 
												break;
											if (*puw_Cur++ == '\\')
												break;
										}
									}
								}
								else
								{
									n = puw_CT[ *puw_Cur++ ];
									r = n % 200;
									n = (n - r) / 200;
									*tgt++ = n+32;
									*tgt++ = r+32;
								}
							}
							*tgt = 0;
						}
						/* no unicode character */
						else
						{
							puw_Cur = auw_Content[ ColCur ];
							while ( *puw_Cur )
							{
								*tgt = (char) puw_CT[ *puw_Cur ];
								puw_Cur++;
								tgt++;
							}
							*tgt = 0;
						}
						TEXT_SetTextID(apst_TxlAddr[ ColLang[ ColCur ] ], idx, sz_NewContent );
						*auw_Content[ ColCur ] = 0;
						L_free( sz_NewContent );
					}
					/* cas particulier pour les '...' */
					else if (L_strchr( asz_Content[ ColCur ], '…' ) )
					{
						char *sz_NewContent, *src, *tgt;
						sz_NewContent = (char *) L_malloc( strlen(asz_Content[ ColCur ]) * 3 );
						src = asz_Content[ ColCur ];
						tgt = sz_NewContent;
						do
						{
							if (*src == '…')
							{
								*tgt++ = '.';
								*tgt++ = '.';
								*tgt++ = '.';
							}
							else
								*tgt++ = *src;
						} while (*src++);
						TEXT_SetTextID(apst_TxlAddr[ ColLang[ ColCur ] ], idx, sz_NewContent );
						L_free( sz_NewContent );
					}
					/* cas normal */
					else
						TEXT_SetTextID(apst_TxlAddr[ ColLang[ ColCur ] ], idx, asz_Content[ ColCur ] );
				}
				else if ( ColType[ ColCur ] >= en_UserData )
				{
					mpt_EditText = pst_TxlRef;
					CEL_SetString( idx, ColType[ ColCur ], asz_Content[ ColCur ] );
					mpt_EditText = pst_SaveEdit;
				}
			}
		}
	}
	
	/* this is the end */
OnImportHtml_End:
	fclose( mst_Import.ph_File );
	L_free( mst_Import.pc_BufStart );
	for (i = 0; i < 100; i++)
	{
		L_free( asz_Content[ i ] );
		if (auw_Content[ i ]) L_free( auw_Content[ i ] );
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnImport(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File("Choose the file to import", 3, 0, 0, NULL, "*.htm");
	CCheckList			o_Check;
	CString				o_file;
	CUIntArray			o_EntryKeyList;
	CStringArray		o_EntryList;
	CStringArray		ao_UserDataList[COLMAX];
	CStringArray		ao_TextList[INO_e_MaxLangNb];
	CString				o_Line;
    CList<ULONG, ULONG> o_SavBankList;
    int					i;
    char				*sz_File, *sz_Ext;
    BOOL				b_Column[ COLMAX ];
    CRect				o_Rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() != IDOK) return;
	
    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    o_SavBankList.RemoveAll();
    for(i=0; i<mpo_ListBank->GetCount(); i++)
    {
        o_SavBankList.AddTail(mpo_ListBank->GetItemData(i));
    }
	OnCloseAll();

	
	o_file = CString(o_File.masz_FullPath) + CString("/") + o_File.mo_File.MakeLower();
	o_file.Replace(".htm/", ".htm");
	
	sz_File = (char *) (LPCTSTR) o_file;
	sz_Ext = strrchr( sz_File, '.' );
	if ( ( sz_Ext ) && !L_strcmp( sz_Ext + 1, "htm") )
	{
		/*
		Column_GetUsed( b_Column, 2 );
		
		b_Column[ en_UserData - 1 ] = 1;		
		o_Check.AddString( "text" );
		
		for (i = en_UserData; i < COLMAX; i++)
		{
			if ( !b_Column[ i ] ) break;
			b_Column[ i ] = 0;
			o_Check.AddString( mst_Ini.asz_ColName[ i - 1 ] );
		}
			
		o_Check.SetArrayBool( &b_Column[ en_UserData - 1 ] );
		mpo_MyView->GetWindowRect(&o_Rect);
		o_Rect.left += (o_Rect.Width() / 2) - 100;
		o_Rect.top += (o_Rect.Height() / 2) - 100;
		o_Check.Do( &o_Rect, 200, this, 20, "column to import");
		
		b_Column[ en_Preview ] = b_Column[ en_UserData - 1 ];
		b_Column[ en_UserData - 1 ] = 0;
		*/
			
		OnImportHtml( sz_File, b_Column, FALSE );
		return;
	}
			
	MessageBox( "Choose htm file", "Bad file type", MB_ICONERROR | MB_OK );
    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::CharTable_Import( int _i_Lang ) 
{
	USHORT				*puw;
	int					i, index;
	EDIA_cl_FileDialog	o_File("Choose htm file with correspondante table", 3, 1 );
	CString				o_Temp;
	char				sz_Line[ 1024 ], *psz_Ext;
	char				*asz_Content[ 10 ];
	char				*sz_Cur;
	USHORT				*auw_Unicode[ 10 ], uw_CharUC[ 2000 ];
	//ADDED 07112005
	USHORT				*puw_UC;
	// ENDADDED
	
	if(o_File.DoModal() != IDOK)
		return;
		
	o_File.GetItem(o_File.mo_File, 1, o_Temp);
	psz_Ext = strrchr((char *) (LPCTSTR) o_Temp, '.');
	if ( ( !psz_Ext ) || L_stricmp(psz_Ext + 1, "htm") )
	{
		M_MF()->MessageBox("Bad file type", "Error", MB_OK);
		return;
	}
	
	/* save context */
	L_memcpy( &mst_ImportSave, &mst_Import, sizeof( mst_Import ) );
	puw = NULL;
	index = 0;
	
	sprintf( sz_Line, "%s\\%s", o_File.masz_FullPath, (char *) (LPCTSTR) o_Temp );
	mst_Import.ph_File = L_fopen(sz_Line, "rt" );
	if (!mst_Import.ph_File )
	{
		M_MF()->MessageBox("Can't open file", "Error", MB_OK);
		goto TEXT_GenerateCharTable_error;
	}
	
	CharTable_Free( _i_Lang );

	// alloc
	puw = (USHORT *) L_malloc( 0x10000 * sizeof( USHORT) );
	// init  
	L_memset( puw, 0, 0x10000 * sizeof( USHORT) );
	for (i = 0; i < 255; i++)
		puw[ i ] = i;
		
	mst_Import.pc_BufStart = (char *) L_malloc( 2048 );
	mst_Import.pc_BufEnd = mst_Import.pc_BufCur = mst_Import.pc_BufStart;
	for (i = 0; i < 10; i++)
		asz_Content[ i ] = (char *) L_malloc( 2000 );
	L_memset( auw_Unicode, 0, sizeof( auw_Unicode ) );
	auw_Unicode[ 1 ] = uw_CharUC;
		
	while ( 1 )
	{
		index++;
		i = OnImportHtml_GetLine( asz_Content, auw_Unicode, 10, 2000 );
		if ( i == -1 )
			break;
			
		sz_Cur = asz_Content[ 0 ];
		index = 0;
		//ADDED 07112005
		while ( *sz_Cur == ' ') sz_Cur++;
		// ENDADDED
		while ( *sz_Cur )
		{
			if ( isdigit( *sz_Cur ) )
				index = (index * 10) + (*sz_Cur - '0');
			else
			{	
				index = 0;
				break;
			}
			sz_Cur++;
		}
		
		if ( (index >= 32) && (index < 4000) )
		{
			//ADDED 07112005
			puw_UC = uw_CharUC;
			while ( *puw_UC == ' ' ) puw_UC++;
			//ENDADDED
			//MODIFIED 07112005
			if (*puw_UC > 255 )
				puw[ *puw_UC ] = index;
			//ENDMODIFY
		}
	}
	
	mapuw_CharTable[ _i_Lang ] = puw;
	goto TEXT_GenerateCharTable_end;
	
TEXT_GenerateCharTable_error:
	if (puw) L_free( puw );
	sprintf ( sz_Line, "Bad file content ( Line %d )", index );
	M_MF()->MessageBox("Bad file content", "Error", MB_OK);

TEXT_GenerateCharTable_end:
	L_fclose( mst_Import.ph_File );
	L_free( mst_Import.pc_BufStart );
	for (i = 0; i < 10; i++)
		L_free( asz_Content[ i ] );
	/* restore import parameters */
	L_memcpy( &mst_Import, &mst_ImportSave, sizeof( mst_Import ) );
	return;

	// for a manual coding correspondance table comment all previous line of code and use 
	// code below (modify it following your data )
#if 0 
	// alloc
	puw = (USHORT *) L_malloc( 0x10000 * sizeof( USHORT) );

	// init  
	L_memset( puw, 0, 0x10000 * sizeof( USHORT) );
	for (i = 0; i < 0x100; i++)
		puw[ i ] = i;

	// special character
	puw[ 0x3041 ] = 300;
	puw[ 0x3042 ] = 301;
	puw[ 0x3043 ] = 302;
	puw[ 0x3044 ] = 303;
	puw[ 0x3045 ] = 304;
	puw[ 0x3046 ] = 305;
	puw[ 0x3047 ] = 306;
	puw[ 0x3048 ] = 307;
	puw[ 0x3049 ] = 308;
	puw[ 0x304A ] = 309;
	puw[ 0x304B ] = 310;
	puw[ 0x304C ] = 311;
	puw[ 0x304D ] = 312;
	puw[ 0x304E ] = 313;
	puw[ 0x304F ] = 314;
	puw[ 0x3050 ] = 315;

	return puw;
#endif
}
/**/
void ETEXT_cl_Frame::CharTable_FreeAll()
{
	int		i;

	for (i = 0; i < INO_e_MaxLangNb; i++)
	{
		if ( mapuw_CharTable[i] )
			L_free( mapuw_CharTable[i] );
		mapuw_CharTable[i]  = NULL;
	}
}
/**/
void ETEXT_cl_Frame::CharTable_Free( int _i_Lang )
{
	if ( mapuw_CharTable[ _i_Lang ] )
		L_free( mapuw_CharTable[ _i_Lang ] );
	mapuw_CharTable[ _i_Lang ]  = NULL;
}
//INO_dst_LanguageName[i].az_LongName

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */

