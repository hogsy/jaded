/*$T DIAspeedact_dlg.cpp GC! 1.100 08/21/01 16:28:07 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAspeedact_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDIPaths.h"
#include "LINKs/LINKmsg.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "EDImainframe.h"
#include "DIAlogs/DIAfile_dlg.h"

char EDIA_cl_SpeedAct:: maz_Path[BIG_C_MaxLenPath] = "Root";
BOOL EDIA_cl_SpeedAct:: mb_Loaded = FALSE;
BOOL EDIA_cl_SpeedAct:: mb_NotLoaded = TRUE;
static int				i_CountFiles = 0;
static int				i_SizeFiles = 0;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/* Message map */
BEGIN_MESSAGE_MAP(EDIA_cl_SpeedAct, EDIA_cl_BaseDialog)
	ON_BN_CLICKED(IDDELETE, OnDelete)
	ON_BN_CLICKED(IDSTATS, OnStats)
	ON_BN_CLICKED(IDLOG, OnLog)
	ON_BN_CLICKED(IDNONE, OnNone)
	ON_BN_CLICKED(IDALL, OnAll)
	ON_BN_CLICKED(IDINVERT, OnInvert)
	ON_BN_CLICKED(IDPATH, OnPath)
	ON_BN_CLICKED(IDEXIT, OnExit)
	ON_BN_CLICKED(IDC_CHECKNOTLOADED, OnNotLoaded)
	ON_BN_CLICKED(IDC_CHECKLOADED, OnLoaded)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SpeedAct::EDIA_cl_SpeedAct(void) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_SPEEDDEL)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_SpeedAct::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~*/
	CListBox	*p_List;
	CWnd		*po_Wnd;
	CButton		*p;
	/*~~~~~~~~~~~~~~~~*/

	p = (CButton *) GetDlgItem(IDC_CHECKNOTLOADED);
	if(mb_NotLoaded) p->SetCheck(TRUE);
	p = (CButton *) GetDlgItem(IDC_CHECKLOADED);
	if(mb_Loaded) p->SetCheck(TRUE);

	po_Wnd = GetDlgItem(IDC_STATICPATH);
	po_Wnd->SetWindowText(maz_Path);

	p_List = (CListBox *) GetDlgItem(IDC_LIST1);
	p_List->SetColumnWidth(30);

	p_List->AddString(EDI_Csz_ExtIni);
	p_List->AddString(EDI_Csz_ExtIniDef);
	p_List->AddString(EDI_Csz_ExtLnk);
	p_List->AddString(EDI_Csz_ExtEdiAction);
	p_List->AddString(EDI_Csz_ExtVss);
	p_List->AddString(EDI_Csz_ExtTree);
	p_List->AddString(EDI_Csz_ExtAIEditorModel);
	p_List->AddString(EDI_Csz_ExtAIEditorVars);
	p_List->AddString(EDI_Csz_ExtAIEditorFct);
	p_List->AddString(EDI_Csz_ExtAIEditorDepend);
	p_List->AddString(EDI_Csz_ExtAIEngineModel);
	p_List->AddString(EDI_Csz_ExtAIEngineFct);
	p_List->AddString(EDI_Csz_ExtAIEngineVars);
	p_List->AddString(EDI_Csz_ExtAIEngineInstance);
	p_List->AddString(EDI_Csz_ExtCOLSetModel);
	p_List->AddString(EDI_Csz_ExtCOLInstance);
	p_List->AddString(EDI_Csz_ExtCOLObject);
	p_List->AddString(EDI_Csz_ExtCOLMap);
	p_List->AddString(EDI_Csz_ExtCOLGMAT);
	p_List->AddString(EDI_Csz_ExtGraphicObject);
	p_List->AddString(EDI_Csz_ExtGraphicMaterial);
	p_List->AddString(EDI_Csz_ExtGraphicLight);
	p_List->AddString(EDI_Csz_ExtTexture1);
	p_List->AddString(EDI_Csz_ExtTexture2);
	p_List->AddString(EDI_Csz_ExtTexture3);
	p_List->AddString(EDI_Csz_ExtGameObject);
	p_List->AddString(EDI_Csz_ExtGameObjects);
	p_List->AddString(EDI_Csz_ExtObjModels);
	p_List->AddString(EDI_Csz_ExtObjGroups);
	p_List->AddString(EDI_Csz_ExtObjGolGroups);
	p_List->AddString(EDI_Csz_ExtWorldText);
	p_List->AddString(EDI_Csz_ExtUniverse);
	p_List->AddString(EDI_Csz_ExtWorld);
	p_List->AddString(EDI_Csz_ExtWorldList);
	p_List->AddString(EDI_Csz_ExtGrpWorld);
	p_List->AddString(EDI_Csz_ExtTextLang);
	p_List->AddString(EDI_Csz_ExtTextFile);	
	p_List->AddString(EDI_Csz_ExtDyna);
	p_List->AddString(EDI_Csz_ExtAnimation);
	p_List->AddString(EDI_Csz_ExtSkeleton);
	p_List->AddString(EDI_Csz_ExtSkin);
	p_List->AddString(EDI_Csz_ExtAnimTbl);
	p_List->AddString(EDI_Csz_ExtShape);
	p_List->AddString(EDI_Csz_ExtAction);
	p_List->AddString(EDI_Csz_ExtActionKit);
	p_List->AddString(EDI_Csz_ExtNetWorld);
	p_List->AddString(EDI_Csz_ExtNetWay);
	p_List->AddString(EDI_Csz_ExtNetObject);
	p_List->AddString(EDI_Csz_ExtGridDef);
	p_List->AddString(EDI_Csz_ExtGridCompressed);
	p_List->AddString(EDI_Csz_ExtEventAllsTracks);
	p_List->AddString(EDI_Csz_ExtSoundMetaBank);
	p_List->AddString(EDI_Csz_ExtSoundBank);
	p_List->AddString(EDI_Csz_ExtSoundMusic);
	p_List->AddString(EDI_Csz_ExtSoundAmbience);
	p_List->AddString(EDI_Csz_ExtSoundDialog);
	p_List->AddString(EDI_Csz_ExtSoundFile);
	p_List->AddString(EDI_Csz_ExtLoadingSound);
	p_List->AddString(EDI_Csz_ExtSModifier);
	p_List->AddString(".raw");
	p_List->AddString(".pal");
	p_List->AddString(".tex");
#ifdef JADEFUSION
	p_List->AddString(".dds");
    p_List->AddString(".xgo");
#endif
	CenterWindow();
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SpeedAct::Stats(BIG_INDEX file, char *ext)
{
	/*~~~~~~~~~~~~~*/
	DIAstats	s;
	POSITION	pos;
	int			size;
	CString		o;
	/*~~~~~~~~~~~~~*/

	o = ext;
	o.MakeLower();
	ext = (char *) (LPCSTR) o;
	if(!mo_StatsRefs.Lookup(ext, (void * &) pos))
	{
		L_memset(&s, 0, sizeof(s));
		s.i_MaxSize = 0;
		s.i_MinSize = 0xFFFFFFFF;
		pos = mo_Stats.AddTail(s);
		mo_StatsRefs.SetAt(ext, pos);
	}

	s = mo_Stats.GetAt(pos);
	s.i_Count++;
	size = BIG_ul_GetLengthFile(BIG_PosFile(file));
	s.i_Size += size;
	i_SizeFiles += size;

	if((UINT) size > s.i_MaxSize)
	{
		s.i_MaxSize = size;
		s.ul_MaxSize = file;
	}

	if((UINT) size < s.i_MinSize)
	{
		s.i_MinSize = size;
		s.ul_MinSize = file;
	}

	mo_Stats.SetAt(pos, s);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SpeedAct::Trace(BIG_INDEX file)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	az[BIG_C_MaxLenPath];
	char	t[2048];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	BIG_ComputeFullName(BIG_ParentFile(file), az);
	sprintf(t, "%s ==> %s", az, BIG_NameFile(file));
	LINK_PrintStatusMsg(t);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SpeedAct::Delete(BIG_INDEX file)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	az[BIG_C_MaxLenPath];
	char	t[2048];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	BIG_ComputeFullName(BIG_ParentFile(file), az);
	sprintf(t, "%s ==> %s", az, BIG_NameFile(file));
	LINK_PrintStatusMsg(t);

	BIG_DelFile(az, BIG_NameFile(file));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SpeedAct::OnRec(CListBox *p_List, BIG_INDEX dir, int what)
{
	/*~~~~~~~~~~~~~*/
	BIG_INDEX	file;
	BIG_INDEX	next;
	char		*pz;
	int			i;
	/*~~~~~~~~~~~~~*/

	if(dir == BIG_C_InvalidIndex) return TRUE;

	file = BIG_SubDir(dir);
	while(file != BIG_C_InvalidIndex)
	{
		if(GetAsyncKeyState(VK_ESCAPE) < 0) return FALSE;
		if(!OnRec(p_List, file, what)) return FALSE;
		file = BIG_NextDir(file);
	}

	file = BIG_FirstFile(dir);
	while(file != BIG_C_InvalidIndex)
	{
		if(GetAsyncKeyState(VK_ESCAPE) < 0) return FALSE;
		next = BIG_NextFile(file);

		/* Correct extension ? */
		pz = L_strrchr(BIG_NameFile(file), '.');
		if(pz && ((i = p_List->FindString(-1, pz)) != LB_ERR))
		{
			if(p_List->GetSel(i))
			{
				if
				(
					((BIG_FileChanged(file) & EDI_FHC_Loaded) && mb_Loaded)
				||	(!((BIG_FileChanged(file) & EDI_FHC_Loaded)) && mb_NotLoaded)
				)
				{
					i_CountFiles++;
					switch(what)
					{
					case 0: Stats(file, pz); break;
					case 1: Trace(file); break;
					case 2: Delete(file); break;
					}
				}
			}
		}

		file = next;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SpeedAct::OnStats(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul;
	DIAstats	s;
	CString		ext;
	POSITION	pos, pos1;
	char		az[1024];
	/*~~~~~~~~~~~~~~~~~~*/

	mo_StatsRefs.RemoveAll();
	mo_Stats.RemoveAll();
	i_CountFiles = 0;
	i_SizeFiles = 0;

	LINK_PrintStatusMsg("Scan Files Statistics");
	LINK_PrintStatusMsg("---------------------");
	ul = BIG_ul_SearchDir(maz_Path);
	if(!OnRec((CListBox *) GetDlgItem(IDC_LIST1), ul, 0))
	{
		LINK_PrintStatusMsg("Operation aborted...");
		return;
	}

	pos = mo_StatsRefs.GetStartPosition();
	while(pos)
	{
		mo_StatsRefs.GetNextAssoc(pos, ext, (void * &) pos1);
		s = mo_Stats.GetAt(pos1);
		sprintf
		(
			az,
			"%s  ==> %d count, %d ko total size, %d ko max size (%s), %d ko min size (%s), %d ko average",
			(char *) (LPCSTR) ext,
			s.i_Count,
			s.i_Size / 1024L,
			s.i_MaxSize / 1024L,
			BIG_NameFile(s.ul_MaxSize),
			s.i_MinSize / 1024L,
			BIG_NameFile(s.ul_MinSize),
			(s.i_Size / s.i_Count) / 1024L
		);
		LINK_PrintStatusMsg(az);
	}

	sprintf(az, "%d files, %d ko total size", i_CountFiles, i_SizeFiles / 1024L);
	LINK_PrintStatusMsg(az);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SpeedAct::OnDelete(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul;
	char		az[1024];
	/*~~~~~~~~~~~~~~~~~*/

	if(M_MF()->MessageBox("Are you sure you want to delete files ?", "Please Comfirm", MB_YESNO) != IDYES) return;
	i_CountFiles = 0;
	LINK_PrintStatusMsg("Delete Files");
	LINK_PrintStatusMsg("------------");
	ul = BIG_ul_SearchDir(maz_Path);
	if(!OnRec((CListBox *) GetDlgItem(IDC_LIST1), ul, 2))
	{
		LINK_PrintStatusMsg("Operation aborted...");
		return;
	}

	sprintf(az, "%d files", i_CountFiles);
	LINK_PrintStatusMsg(az);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SpeedAct::OnLog(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul;
	char		az[1024];
	/*~~~~~~~~~~~~~~~~~*/

	i_CountFiles = 0;
	LINK_PrintStatusMsg("Scan Files Trace");
	LINK_PrintStatusMsg("----------------");
	ul = BIG_ul_SearchDir(maz_Path);
	if(!OnRec((CListBox *) GetDlgItem(IDC_LIST1), ul, 1))
	{
		LINK_PrintStatusMsg("Operation aborted...");
		return;
	}

	sprintf(az, "%d files", i_CountFiles);
	LINK_PrintStatusMsg(az);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SpeedAct::OnExit(void)
{
	EndDialog(IDCANCEL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SpeedAct::OnPath(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CWnd				*po_Wnd;
	EDIA_cl_FileDialog	o_File("Choose path", 2, FALSE, TRUE, maz_Path);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		L_strcpy(maz_Path, (char *) (LPCSTR) o_File.masz_FullPath);
		po_Wnd = GetDlgItem(IDC_STATICPATH);
		po_Wnd->SetWindowText(maz_Path);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SpeedAct::OnAll(void)
{
	/*~~~~~~~~~~~~~~~~*/
	CListBox	*p_List;
	int			j;
	/*~~~~~~~~~~~~~~~~*/

	p_List = (CListBox *) GetDlgItem(IDC_LIST1);
	p_List->SetRedraw(FALSE);
	j = p_List->GetTopIndex();
	for(int i = 0; i < p_List->GetCount(); i++) p_List->SetSel(i, TRUE);
	p_List->SetTopIndex(j);
	p_List->SetRedraw(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SpeedAct::OnNone(void)
{
	/*~~~~~~~~~~~~~~~~*/
	CListBox	*p_List;
	int			j;
	/*~~~~~~~~~~~~~~~~*/

	p_List = (CListBox *) GetDlgItem(IDC_LIST1);
	p_List->SetRedraw(FALSE);
	j = p_List->GetTopIndex();
	for(int i = 0; i < p_List->GetCount(); i++) p_List->SetSel(i, FALSE);
	p_List->SetTopIndex(j);
	p_List->SetRedraw(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SpeedAct::OnInvert(void)
{
	/*~~~~~~~~~~~~~~~~*/
	CListBox	*p_List;
	int			j;
	/*~~~~~~~~~~~~~~~~*/

	p_List = (CListBox *) GetDlgItem(IDC_LIST1);
	p_List->SetRedraw(FALSE);
	j = p_List->GetTopIndex();
	for(int i = 0; i < p_List->GetCount(); i++)
	{
		if(p_List->GetSel(i))
			p_List->SetSel(i, FALSE);
		else
			p_List->SetSel(i, TRUE);
	}

	p_List->SetTopIndex(j);
	p_List->SetRedraw(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SpeedAct::OnNotLoaded(void)
{
	/*~~~~~~~*/
	CButton *p;
	/*~~~~~~~*/

	p = (CButton *) GetDlgItem(IDC_CHECKNOTLOADED);
	if(p->GetCheck())
		mb_NotLoaded = TRUE;
	else
		mb_NotLoaded = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SpeedAct::OnLoaded(void)
{
	/*~~~~~~~*/
	CButton *p;
	/*~~~~~~~*/

	p = (CButton *) GetDlgItem(IDC_CHECKLOADED);
	if(p->GetCheck())
		mb_Loaded = TRUE;
	else
		mb_Loaded = FALSE;
}

#endif /* ACTIVE_EDITORS */
