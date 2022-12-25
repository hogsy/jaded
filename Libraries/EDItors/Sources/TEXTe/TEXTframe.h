/*$T TEXTframe.h GC 1.138 01/11/05 15:18:54 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "EDIbaseframe.h"
#include "TEXTframe_act.h"
#include "TEXTscroll.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "INOut/INO.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define COLMAX	100
#define SIZEMAX 64

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define ETEXT_Cte_UseStandardLang	0x00000001
#define ETEXT_Cte_SyncAuto			0x00000002
#define ETEXT_Cte_KeepEmptyTxt		0x00000004
#define ETEXT_Cte_KeepAllTxt		0x00000008

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define ETEXT_Content_None		0
#define ETEXT_Content_Int		1
#define ETEXT_Content_String	2

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
typedef struct ETEXT_tdst_ParseHtml_
{
	FILE				*ph_File;
	char				*pc_BufStart;
	char				*pc_BufEnd;
	char				*pc_BufCur;
	char				c_Char;
} ETEXT_tdst_ParseHtml;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
typedef struct ETEXT_tdst_EnumUserDefinition_ 
{
	BAS_tdst_barray st_ValueToName;
	BAS_tdst_barray st_NameToValue;
} ETEXT_tdst_EnumUserDefinition;

class ETEXT_cl_Frame : public EDI_cl_BaseFrame
{
	DECLARE_DYNCREATE(ETEXT_cl_Frame)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	ETEXT_cl_Frame(void);
	~ETEXT_cl_Frame(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	CSplitterWnd		mo_Splitter0;

	CSplitterWnd		mo_Splitter2;

	CListBox			*mpo_ListBank;
	CListBox			*mpo_ListFile;

	int					mi_SortCol;

	ETEXT_cl_Scroll		*mpo_Scroll;

	BOOL				mb_FileIsModified;
	BOOL				mb_NeedSync;

	TEXT_tdst_OneText	*mpt_EditText;
	
	TEXT_tdst_AllText	BufferBank;
	ULONG				ul_BufferBankIdx;

	BOOL				mb_FindSomething;
	int					mi_FindLine, mi_FindCol;

	BOOL				mb_ExportNoFormatCode;
	int					mi_SynchroLang;

	BOOL				mi_ForceSynchroEntry;
	TEXT_tdst_Id		mst_ForceSynchroEntry;

	/* importing variables */
	ETEXT_tdst_ParseHtml	mst_Import;
	ETEXT_tdst_ParseHtml	mst_ImportSave;
	
	//FILE				*mph_ImportFile;
	//char				*mpc_ImportBufStart, *mpc_ImportBufEnd, *mpc_ImportBufCur, mc_ImportChar;
	char				*mpc_TokenContent, *mpc_TokenContentCur, *mpc_TokenContentEnd;
	int					mi_TokenParam;
	USHORT				*mapuw_CharTable[ INO_e_MaxLangNb ];
	BOOL				mb_Unicode;

	/* exporting variable */
	int					mi_LangNb, mai_Lang[INO_e_MaxLangNb];
	int					mi_TxgNb, mi_TxgMax;
	ULONG				*mpul_TxgIndex;

	/* new entry default value */
	char				msz_NewEntry_Name[TEXT_MaxLenId];
	int					mi_NewEntry_Number;

	enum en_ColContent
	{
		en_Empty		= 0x0000,
		en_Rank			= 0x0001,
		en_ID			= 0x0002,
		en_Preview		= 0x0003,
		en_IDKey		= 0x0004,
		en_SndFileKey	= 0x0005,
		en_Character	= 0x0006,
		en_TxgFileKey	= 0x0007,
		en_TxlFileKey	= 0x0008,
		en_LabelAuto	= 0x0009,
		en_Priority		= 0x000A,
		en_FacialExp	= 0x000B,
		en_Lips			= 0x000C,
		en_Anims		= 0x000D,
		en_UserData		= 0x000E,
		en_MaxNb		= COLMAX,
		en_Masked		= 0x8000,
		en_Dummy		= 0xFFFFFFFF
	};
	struct
	{
		int				i_Version;
		int				i_LeftPaneCX;
		int				i_LeftPaneCY1;
		int				i_LeftPaneCY2;
		int				ai_SizeCols[COLMAX];
		en_ColContent	ae_ContentCols[COLMAX];
		char			ai_NameCols[COLMAX][SIZEMAX];
		char			ac_UserColUsed[en_MaxNb];
		int				i_OptionFlags;

		int				ai_ColSize[COLMAX];
		char			asz_ColName[COLMAX][SIZEMAX];
		int				ai_ColOrder[COLMAX];

		int				i_NbSndFileKey;
		ULONG			ai_SndFileKey[10];

		int				i_NbCharacterKey;
		ULONG			ai_CharacterKey[10];
		int				i_OrderColumn;

		char			ac_Reserve[248];
	} mst_Ini;

	ETEXT_tdst_EnumUserDefinition	max_EnumUserDefinition[en_UserData];
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

private:
	en_ColContent	e_GetColTypeFromName(char *);
	void			ResetEntryColumnInfo(void);
	/**/
	int				i_GetSelNb(void);
	ULONG			ul_GetCurBank(int *p = NULL);
	ULONG			ul_GetCurFile(int *p = NULL);
	/**/
	void			BankPopup(void);
	void			FilePopup(void);
	/**/
	void			Synchronize(ULONG ulFatDest, ULONG ulFatRef);
	void			SaveFile(struct TEXT_tdst_OneText_ *pText);
	/**/
	int				i_UserChooseLanguageList(BOOL *ab_Lang);
	/**/
	void			CFileReadLine(CFile *poFile, CString &o);
	void			GetHeader(CString &o_Line, en_ColContent *ae_Usage, enum INO_tden_LanguageId_ *);
	void			GetLineData
					(
						CString &,
						en_ColContent *,
						enum INO_tden_LanguageId_ *,
						ULONG &,
						ULONG *,
						CStringArray &,
						CUIntArray &,
						CStringArray *,
						CStringArray *
					);

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	BOOL	b_HasUserValue(en_ColContent);
	int		i_GetUserValueNb(en_ColContent);
	ULONG	ul_GetUserValue(en_ColContent, int);
	char*	p_GetUserName(en_ColContent, int);
	char*	p_GetUserValueToName(en_ColContent, ULONG);
	ULONG	ul_GetUserNameToValue(en_ColContent, char*);

	void	SetFileModified(BOOL = TRUE, ULONG = -1);
	ULONG	GetSndFileAuto(char *);
	void	EditValidate(int, int);
	void	OnDisplayAllFilesLoaded(void);
	void	OnReportNoSound(void);

	void	EntryPopup(void);
	void	SetEntryColumnInfo(void);
	void	GetEntryColumnInfo(void);
	int		Column_GetUsed(BOOL *, int);

	void	OnBankOpen(ULONG = -1);
	void	OnBankNew(void);
	void	OnBankNoLangNew(void);
	void	OnBankClose(void);
	void	OnBankChange(void);

	void	OnFileClose(void);
	void	OnFileNew(void);
	void	OnFileDelete(void);
	void	OnFileSave(void);
	void	OnFileOpen(ULONG ulFat);
	void	OnFileGetSndFileAuto(void);
	void	OnFileGetPrefixAuto(BOOL b_OnlySelected);
	void	FileChange(BIG_INDEX ulFat);

	void	OnEntryNew(BOOL, BOOL b_hole = FALSE);
	void	OnEntryDelete(void);
	void	OnEntryCopy(void);
	void	OnEntryCut(void);
	void	OnEntryPaste(void);

	void	CompressComment(TEXT_tdst_OneText *);
	void	OnCloseAll(void);
	void	OnSynchronize(void);
	void	OnSynchronizeAll(void);

	void	OnFind(void);
	void	OnFindSaleChar(void);
	void	OnReplaceSaleChar( void );
	void	OnFindNext(int);

	void	OnExportFile(void);
	void	OnExportAll(void);
	void	OnImport(void);
	void	OnCheckAndFixAll(void);
	void	OnReportSoundFile(void);

	char	OnImportHtml_GetChar( int );
	int		OnImportHtml_GetToken(void);
	void	OnImportHtml_ParseContent(char *);
	void	OnImportHtml_ParseContent_Unicode( char *, USHORT *);
	int		OnImportHtml_GetLine(char *[], USHORT *[], int, int );
	void	OnImportHtml(char *, BOOL *, BOOL);
	
	void	CharTable_Import( int );
	void	CharTable_FreeAll();
	void	CharTable_Free( int );

	BOOL	Bank_Load(ULONG, ULONG *);
	void	OnExportHtml(char *);

	/**/
	char	*CEL_GetText(int, int);
	int		CEL_GetContent(int, int, ULONG *);
	int		CEL_SetString(int, int, char *);
	int		CEL_SetInt(int, int, ULONG);

	/* fonction for history key */
	void	CheckKeyHistory(int *, ULONG *);
	void	AddKeyInHistory(int *, ULONG *, ULONG);

	void	CheckSndFileHistory(void);
	void	AddSndFileInHistory(ULONG);
	void	CheckCharacterHistory(void);
	void	AddCharacterInHistory(ULONG);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    INTERFACE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	ReinitIni(void);
	void	LoadIni(void);
	void	TreatIni(void);
	void	SaveIni(void);
	/**/
	char	*psz_OnActionGetBase(void)	{ return ETEXT_asz_ActionBase; };
	void	OnAction(ULONG);
	BOOL	b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);
	UINT	ui_OnActionState(ULONG);
	int		i_OnMessage(ULONG, ULONG, ULONG);
	BOOL	b_KnowsKey(USHORT);
	void	AfterEngine(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL			PreTranslateMessage(MSG *);
	afx_msg int		OnCreate(LPCREATESTRUCT);
	/**/
	afx_msg void	OnBankSelChange(void);
	afx_msg void	OnFileChange(void);

	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
