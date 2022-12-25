/**********************************************************************
 *<
	FILE: mad_util.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/
#include  <process.h>
#include "shlobj.h"
#include "mad_util.h"
#include "MAD_mem/Sources/MAD_mem.h"
#include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#include "ImportExportDef.h"

#include "iparamm2.h"
#include "iskin.h"
#include "SkinUtils.h"

// The unique ClassID
#define MAD_UTIL_CLASS_ID	Class_ID(0xca22745f, 0xcd102cf5)

class Mad_util : public UtilityObj {
	public:

		IUtil *iu;
		Interface *ip;
		HWND hPanel;
        char Filename[260];
        BOOL bCopyTextures;
	    char            TexturePATH[260];

		
		//Constructor/Destructor
		Mad_util();
		~Mad_util();

		void BeginEditParams(Interface *ip,IUtil *iu);
		void EndEditParams(Interface *ip,IUtil *iu);
		void DeleteThis() {}

		void Init(HWND hWnd);
		void Destroy(HWND hWnd);

		int		DoImportSkin ( const TCHAR *name );
private:
		INode	*GetNodeByName(char *_Name, INode *_RootNode);
};


static Mad_util theMad_util;

// This is the Class Descriptor for the Mad_util plug-in
class Mad_utilClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return &theMad_util;}
	const TCHAR *	ClassName() {return GetString(IDS_CLASS_NAME);}
	SClass_ID		SuperClassID() {return UTILITY_CLASS_ID;}
	Class_ID		ClassID() {return MAD_UTIL_CLASS_ID;}
	const TCHAR* 	Category() {return GetString(IDS_CATEGORY);}
	void			ResetClassParams (BOOL fileReset);
};

static Mad_utilClassDesc Mad_utilDesc;
ClassDesc* GetMad_utilDesc() {return &Mad_utilDesc;}

//TODO: Should implement this method to reset the plugin params when Max is reset
void Mad_utilClassDesc::ResetClassParams (BOOL fileReset) 
{

}
/*
 ===================================================================================================
Read a file choose by user
use like this
    char name [256];
    name = Default nema;
    Retrun 0 if failed
 ===================================================================================================
 */
unsigned long CDLG_USR_GetFileName_Save(char *Name, HWND MUM)
{
    OPENFILENAME ofn;
    char Convert[32];
    char DirectoryName[260];
    char FileName[260];
    char *FilterSRC;
    memset(Convert,0,32);
    FilterSRC = Name;
    sprintf(Convert , "MAD Export File|*.MAD" );
    FilterSRC = Convert;
    while (*FilterSRC)
    {
        if (*FilterSRC == '|')
        *FilterSRC = 0;
        FilterSRC++;
    } 
    *(FilterSRC+1) = 0;
    /* compute Name & dir */
    MAD_SeparateDirNameFromName(Name,DirectoryName,FileName);
    /* call windows fonction */
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = MUM;
    ofn.hInstance = NULL;
    ofn.lpstrFilter = Convert;
    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = FileName;
    ofn.nMaxFile = 256;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrInitialDir = DirectoryName;
    ofn.lpstrTitle = "Choose file to export";
    ofn.Flags = OFN_NOCHANGEDIR ;
    ofn.lpstrDefExt = NULL;
    if (!GetSaveFileName(&ofn))
        return 0;
	FilterSRC = FileName;
    while (*FilterSRC) FilterSRC++;
	if (*(FilterSRC-4) != '.') sprintf(Name , "%s.MAD" , FileName);
	else
		strcpy(Name,FileName);
    return 1;
}
/*
 ===================================================================================================
 function for enble - disable windows buttons
 ===================================================================================================
 */
void MAD_EnableWindow(HWND hwndDlg,unsigned long ED )
{
    DWORD dwStyle ;
    dwStyle = GetWindowLong(hwndDlg , GWL_STYLE);  
    if (!ED)
        dwStyle |= WS_DISABLED;
    else
        dwStyle &= ~WS_DISABLED;
    SetWindowLong(hwndDlg , GWL_STYLE, dwStyle );
    ShowWindow(hwndDlg,SW_HIDE );
    ShowWindow(hwndDlg,SW_SHOW );
}

int _stdcall fn_CBBrowse( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	lParam = lParam;
	if ( uMsg == BFFM_INITIALIZED)
	{
		SendMessage( hWnd, BFFM_SETSELECTION, 1, lpData );
	}
	return 0;
}

int fn_SH_bBrowseForFolder( /*HWND _p_oWnd, */char *_szTitle, char *_szPath )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//    HWND _p_oWnd;
	BROWSEINFO		stBI;
	ITEMIDLIST		*p_stItem;
	char			szDisplayName[ MAX_PATH ];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	stBI.hwndOwner = NULL;
	stBI.pidlRoot = NULL;
    stBI.pszDisplayName = szDisplayName;
    stBI.lpszTitle = _szTitle;
    stBI.ulFlags = 0;
	stBI.lpfn = (*_szPath) ? fn_CBBrowse : NULL;
	stBI.lParam = (LPARAM) _szPath;
	
	if ((p_stItem = SHBrowseForFolder( &stBI )) == NULL)
		return FALSE;

    SHGetPathFromIDList( p_stItem, _szPath );
    while ((*_szPath) != 0) _szPath++;
    *_szPath = 0x5c;
    *(_szPath + 1) = 0;

	return TRUE;
}

static BOOL CALLBACK Mad_utilDlgProc(
                                     HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char				FName[400];
	unsigned long		ExpFlags;

	ExpFlags = 0;

    switch (msg) {
    case WM_INITDIALOG:
        theMad_util.Init(hWnd);
		if (!theMad_util . Filename[0]) 
		strcpy(theMad_util . Filename,"X:\\QuickExport.MAD");
        if (theMad_util . Filename[0]) SetDlgItemText(hWnd , IDC_BUTTONSelectFile , theMad_util . Filename);
		MAD_EnableWindow(GetDlgItem(hWnd,IDC_BUTTONRADIOSITY),theMad_util . Filename[0]);
		MAD_EnableWindow(GetDlgItem(hWnd,IDC_BUTTONExport),theMad_util . Filename[0]);
		MAD_EnableWindow(GetDlgItem(hWnd,IDC_BUTTONIMPORT),theMad_util . Filename[0]);
		MAD_EnableWindow(GetDlgItem(hWnd,IDC_BUTTON2),theMad_util.bCopyTextures);

        if (theMad_util.bCopyTextures)
            CheckDlgButton( hWnd, IDC_COPYTEX , BST_CHECKED );
        else
            CheckDlgButton( hWnd, IDC_COPYTEX , BST_UNCHECKED );

		CheckDlgButton( hWnd, IDC_INCLUDETEXTURES,	1 );
		CheckDlgButton( hWnd, IDC_SKINEXPORT,		1 );
		CheckDlgButton( hWnd, IDC_MERGE,			0 );
		CheckDlgButton( hWnd, IDC_SETOPACITY,		1 );
		CheckDlgButton( hWnd, IDC_IMPORTMULTIMAT,	0 );
        break;
        
    case WM_DESTROY:
        theMad_util.Destroy(hWnd);
        break;
        
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDC_BUTTONSelectFile:
            CDLG_USR_GetFileName_Save(theMad_util . Filename, hWnd);
			if (theMad_util . Filename[0])
            SetDlgItemText(hWnd , IDC_BUTTONSelectFile , theMad_util . Filename);
			MAD_EnableWindow(GetDlgItem(hWnd,IDC_BUTTONRADIOSITY),theMad_util . Filename[0]);
			MAD_EnableWindow(GetDlgItem(hWnd,IDC_BUTTONExport),theMad_util . Filename[0]);
			MAD_EnableWindow(GetDlgItem(hWnd,IDC_BUTTONIMPORT),theMad_util . Filename[0]);
            break;

        case IDC_BUTTONIMPORT:
			{
				MadImportParam	st_TheImportParam;

				// if no file selected, ask for one
				if (!theMad_util.Filename[0])
				{
					CDLG_USR_GetFileName_Save(theMad_util . Filename, hWnd);
					SetDlgItemText(hWnd , IDC_BUTTONSelectFile , theMad_util . Filename);
				}

				// Set Parameters
				st_TheImportParam.mb_Merge = IsDlgButtonChecked(hWnd, IDC_MERGE);
				st_TheImportParam.mb_MergeTexture = IsDlgButtonChecked(hWnd, IDC_COPYTEX);
				if (theMad_util . TexturePATH[0] != 0)
					st_TheImportParam.msz_TexturePath = theMad_util.TexturePATH;
				else
					st_TheImportParam.msz_TexturePath = NULL;
				st_TheImportParam.mb_SetFullOpacity = IsDlgButtonChecked(hWnd, IDC_SETOPACITY);
				st_TheImportParam.mb_ImportMultiTexMat = IsDlgButtonChecked(hWnd, IDC_IMPORTMULTIMAT);

				if (theMad_util.ip->ImportFromFile( theMad_util.Filename, (BOOL) &st_TheImportParam))
					// The weights of the skin must be imported after "ImportFromFile" or it won't work
					// (don't ask why).
					theMad_util.DoImportSkin( theMad_util.Filename );
			}
            break;

        case IDC_BUTTONExport:
            if (!theMad_util . Filename[0])
            {
                CDLG_USR_GetFileName_Save(theMad_util . Filename, hWnd);
                SetDlgItemText(hWnd , IDC_BUTTONSelectFile , theMad_util . Filename);
            }
			if (!IsDlgButtonChecked(hWnd, IDC_SKINEXPORT))
				ExpFlags |= MADEXP_NOSKIN;
            if (IsDlgButtonChecked(hWnd, IDC_INCLUDETEXTURES)) 
                ExpFlags |= MADEXP_INCLUDETEX;
            else
                ExpFlags |= MADEXP_DONOTINCLUDETEX;
			theMad_util . ip -> ExportToFile(theMad_util . Filename, ExpFlags);
            break;

        case IDC_BUTTONExportSelOnly:
            if (!theMad_util . Filename[0])
            {
                CDLG_USR_GetFileName_Save(theMad_util . Filename, hWnd);
                SetDlgItemText(hWnd , IDC_BUTTONSelectFile , theMad_util . Filename);
            }
			ExpFlags |= MADEXP_SELONLY;
			if (!IsDlgButtonChecked(hWnd, IDC_SKINEXPORT))
				ExpFlags |= MADEXP_NOSKIN;
            if (IsDlgButtonChecked(hWnd, IDC_INCLUDETEXTURES)) 
                ExpFlags |= MADEXP_INCLUDETEX;
            else
                ExpFlags |= MADEXP_DONOTINCLUDETEX;
            theMad_util . ip -> ExportToFile(theMad_util . Filename, ExpFlags);
            break;

        case IDC_COPYTEX:
            theMad_util.bCopyTextures = !theMad_util.bCopyTextures;
			MAD_EnableWindow(GetDlgItem(hWnd,IDC_BUTTON2),theMad_util.bCopyTextures);
            break;

		case IDC_BUTTONRADIOSITY:
			if (!IsDlgButtonChecked(hWnd, IDC_SKINEXPORT))
				ExpFlags |= MADEXP_NOSKIN;
            if (IsDlgButtonChecked(hWnd, IDC_INCLUDETEXTURES)) 
                ExpFlags |= MADEXP_INCLUDETEX;
            else
                ExpFlags |= MADEXP_DONOTINCLUDETEX;
            theMad_util . ip -> ExportToFile(theMad_util . Filename, ExpFlags);
			theMad_util.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			sprintf (FName , "C:\\MAD_RAD\\mad_rad.exe %s" , theMad_util . Filename);
			WinExec( FName , SW_SHOW);
			break;

		case IDC_BUTTON2:
			fn_SH_bBrowseForFolder( "Choose an extract texture directory ", theMad_util . TexturePATH );
			if (theMad_util . TexturePATH[0])
			{
				SetDlgItemText(hWnd , IDC_BUTTON2 , theMad_util . TexturePATH);
			}
			break;
        }
        break;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MOUSEMOVE:
            theMad_util.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
            break;
            
        default:
            return FALSE;
    }
    return TRUE;
}



//--- Mad_util -------------------------------------------------------
Mad_util::Mad_util()
{
	iu = NULL;
	ip = NULL;	
	hPanel = NULL;
    Filename[0] = 0;
    bCopyTextures = FALSE;
	TexturePATH[0] = 0;
}

Mad_util::~Mad_util()
{

}

void Mad_util::BeginEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = iu;
	this->ip = ip;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		Mad_utilDlgProc,
		GetString(IDS_PARAMS),
		0);
}
	
void Mad_util::EndEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = NULL;
	this->ip = NULL;
	ip->DeleteRollupPage(hPanel);
	hPanel = NULL;
}

void Mad_util::Init(HWND hWnd)
{

}

void Mad_util::Destroy(HWND hWnd)
{

}

/*
 ===================================================================================================
	Return first child node (or grand child or grand grand child...) of _RootNode which is named
	_Name.
 ===================================================================================================
 */
INode *Mad_util::GetNodeByName(char *_Name, INode *_RootNode)
{
	INode *res;

	if (!strcmp(_RootNode->GetName(), _Name))
		return _RootNode;

	for(int c = 0; c < _RootNode->NumberOfChildren(); c++)
		if (res = GetNodeByName(_Name, _RootNode->GetChildNode(c)))
			return res;

	return NULL;
}


/*
 ===================================================================================================
	This function is just a bad answer to a problem I can't solve: I can't add weights to the
	skin during the import. So I created this function which import only skin weights and
	ise called after first import, once the skin context and all the stuff have
	been initialized by max.
 ===================================================================================================
 */
int Mad_util::DoImportSkin(const TCHAR *name)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter;
    Matrix3         NodeMAt;
    Color           Col;
	MAD_World		*MW;
	FILE			*pStream;
	INode           **AllINodes;
	bool			b_OneSkinImported = false;
    char			FileName[_MAX_PATH];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    /* Load MADWorld */
	sprintf(FileName, "%s", name);
    MW = MAD_Load(FileName,NULL,0);

    if(MW == NULL)
    {
        MessageBox
        (
            ip->GetMAXHWnd(),
            _T("This is not a valid .MAD format. "),
            _T(""),
            MB_OK | MB_ICONERROR | MB_TASKMODAL
        );
	    MAD_FREE();
	    ip->ProgressEnd();
        return 1;
    }
    ip->ProgressUpdate(10);

    /* Find nodes */

    MAD_MALLOC(INode *, AllINodes, MW->NumberOfHierarchieNodes);
    for(Counter = 0; Counter < MW->NumberOfHierarchieNodes; Counter++)
		AllINodes[Counter] = GetNodeByName(MW->Hierarchie[Counter].ID.Name, ip->GetRootNode());
    ip->ProgressUpdate(20);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         Import skin info
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    for(Counter = 0; Counter < MW->NumberOfHierarchieNodes; Counter++)
    {
		if (AllINodes[Counter] &&
			MW->AllObjects[MW->Hierarchie[Counter].Object]->IDType == ID_MAD_SkinnedGeometricObject)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			Modifier					*mod;
			ISkinImportData				*skin;
			MAD_SkinnedGeometricObject	*pst_Mad_SknObj;
			INode						*pst_BoneNode;
			MAD_PonderationList			*pst_PondList;
			INode						**apst_Bone;
			Tab<INode*>					tab_Bone;
			float						*af_Pond;
			Tab<float>					tab_Pond;
			float						f_Temp;
			int							i_NumVert, i_VertIdx;;
			unsigned char				*auc_NumPond;
			Matrix3						st_IdentityMat;
			bool						b_HaveAlreadyBeenWarned_Weights, b_HaveAlreadyBeenWarned_Vert;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			st_IdentityMat.IdentityMatrix();
			b_HaveAlreadyBeenWarned_Weights = false;
			b_HaveAlreadyBeenWarned_Vert    = false;

			// Check if there is a skin modifier
			mod = SkinUtils::GetSkin(AllINodes[Counter]);
			if (!mod) continue;

			// Set modifiers data
			skin = (ISkinImportData *) mod->GetInterface(I_SKINIMPORTDATA);
			if (!skin) continue;

			pst_Mad_SknObj = (MAD_SkinnedGeometricObject *)MW->AllObjects[MW->Hierarchie[Counter].Object];
			if (!pst_Mad_SknObj->us_NumberOfPonderationLists) continue;

			// Init arrays
			i_NumVert = pst_Mad_SknObj->pst_GeoObj->NumberOfPoints;
			apst_Bone   = (INode **)        malloc(MAD_IMPORT_MAX_BONE_PER_VERT * i_NumVert * sizeof(INode *));
			af_Pond     = (float *)         malloc(MAD_IMPORT_MAX_BONE_PER_VERT * i_NumVert * sizeof(float));
			auc_NumPond = (unsigned char *) malloc(    i_NumVert * sizeof(unsigned char));
			if (!apst_Bone || !af_Pond || !auc_NumPond) continue;
			memset(apst_Bone,   0, MAD_IMPORT_MAX_BONE_PER_VERT * i_NumVert * sizeof(INode *));
			memset(af_Pond,     0, MAD_IMPORT_MAX_BONE_PER_VERT * i_NumVert * sizeof(float));
			memset(auc_NumPond, 0,                                i_NumVert * sizeof(unsigned char));

			// Stores ponderations in arrays
			for (WORD us=0; us<pst_Mad_SknObj->us_NumberOfPonderationLists; us++)
			{
				pst_PondList = pst_Mad_SknObj->pst_PonderationList + us;
				if (pst_PondList->us_MatrixIdx < 0 || pst_PondList->us_MatrixIdx >= MW->NumberOfHierarchieNodes)
					continue;

				// Find node associated to this bone
				pst_BoneNode = AllINodes[pst_PondList->us_MatrixIdx];
				if (!pst_BoneNode)
					continue;

				// Update pond and bone arrays
				for (WORD us2 = 0; us2 < pst_PondList->us_NumberOfPonderatedVertices; us2++)
				{
					i_VertIdx = (int)pst_PondList->p_CmpPdrtVrt[us2].Index;
					if (auc_NumPond[i_VertIdx] >= MAD_IMPORT_MAX_BONE_PER_VERT)
					{
						if(!b_HaveAlreadyBeenWarned_Vert)
						{
							// Too much weights. Impossible.
							char    Warn[300]; /* */
							sprintf(Warn , "At least one vertex have weights on more than %d bones.\n Continue import.",
								MAD_IMPORT_MAX_BONE_PER_VERT);
							MessageBox(
								ip->GetMAXHWnd(),
								Warn,
								TEXT("Import warning"),
								MB_OK | MB_ICONWARNING | MB_TASKMODAL );
							b_HaveAlreadyBeenWarned_Vert = true;
						}
						continue;
					}
					apst_Bone[MAD_IMPORT_MAX_BONE_PER_VERT * i_VertIdx + auc_NumPond[i_VertIdx]] = pst_BoneNode;
					f_Temp = *((float *) &pst_PondList->p_CmpPdrtVrt[us2]);
					*(ULONG *)&f_Temp &= 0xffff0000;
					af_Pond[MAD_IMPORT_MAX_BONE_PER_VERT * i_VertIdx + auc_NumPond[i_VertIdx]] = f_Temp;
					auc_NumPond[i_VertIdx]++;
				}
			}

			for (i_VertIdx = 0; i_VertIdx < i_NumVert; i_VertIdx++)
			{
				if (!auc_NumPond[i_VertIdx])
					continue;

				tab_Bone.ZeroCount();
				tab_Pond.ZeroCount();
				// Add ponderations
				for (int i = 0; i < MAD_IMPORT_MAX_BONE_PER_VERT && i < auc_NumPond[i_VertIdx]; i++)
				{
					tab_Bone.Append( 1, apst_Bone + (MAD_IMPORT_MAX_BONE_PER_VERT * i_VertIdx + i) );
					tab_Pond.Append( 1, af_Pond   + (MAD_IMPORT_MAX_BONE_PER_VERT * i_VertIdx + i) );
				}
				if (!skin->AddWeights(AllINodes[Counter], i_VertIdx, tab_Bone, tab_Pond))
				{
					if (!b_HaveAlreadyBeenWarned_Weights)
					{
						char    Warn[300]; /* */
						sprintf(Warn , "Problem encountered while adding weights.");
						MessageBox(
							ip->GetMAXHWnd(),
							Warn,
							TEXT("Import warning"),
							MB_OK | MB_ICONWARNING | MB_TASKMODAL );
						b_HaveAlreadyBeenWarned_Weights = true;
					}
				}
			}

			mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
			mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_NUM_SUBOBJECTTYPES_CHANGED);
			AllINodes[Counter]->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
			ip->ForceCompleteRedraw();

			b_OneSkinImported = true;
		}
    }

    /* End */

	if (b_OneSkinImported)
	{
		char    text[300]; /* */
		sprintf(text , "Import skin completed.\nTry to unable \"Weight All Vertices\" option if you have any problem.");
		MessageBox(
			ip->GetMAXHWnd(),
			text,
			TEXT("Import completed"),
			MB_OK | MB_TASKMODAL );
	}

    MAD_FREE();
    return 1;
}

