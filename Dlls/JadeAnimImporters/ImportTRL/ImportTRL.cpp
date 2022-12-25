#include "StdAfx.h"
#include "ImportTRL.h"
#include "GetParamDlg.h"

ClassDesc2* GetImportTRLDesc() 
{
	return &gImportTRLDesc;
}

// Sigleton Design pattern
CImportTRL* CImportTRL::_instance = NULL;

CImportTRL& CImportTRL::Instance()
{
	if (!_instance)
		_instance = new CImportTRL;
	return *_instance;
}

// Constructor
CImportTRL::CImportTRL()
{
}

// Destructor 
CImportTRL::~CImportTRL()
{
	_instance = NULL;
}

// Definition for 3DSMax Virtual functions
int CImportTRL::ExtCount()
{
	return 1;
}

// Extension to search for the import plugins
const TCHAR* CImportTRL::Ext( int i )
{
	switch (i)
	{
	case 0:
		return _T("TRL");
	default:
		return _T("");
	}
}

// Long description of plugins
const TCHAR* CImportTRL::LongDesc()
{
	return _T("Import Ubi Soft TRL animation files for Jade engine");
}

// Caption used in file selector
const TCHAR* CImportTRL::ShortDesc()
{
	return _T("Jade Anim TRL files");
}

// Author name (Yes! it's usefull when someone at Ubi try to understand this module!)
const TCHAR* CImportTRL::AuthorName()
{
	return _T("Jean-Sylvain Sormany");
}

// Legal stuff
const TCHAR* CImportTRL::CopyrightMessage()
{
	return _T("No Copyright");
}

// Not used
const TCHAR* CImportTRL::OtherMessage1()
{
	return _T("");
}

// Not used
const TCHAR* CImportTRL::OtherMessage2()
{
	return _T("");
}

// Current version
unsigned int CImportTRL::Version()
{
	return 0;
}

// Normally we should fill the window with comments on plugins
void CImportTRL::ShowAbout(HWND hWnd)
{
}

// Main function of the plugins, call when the file is selected
int CImportTRL::DoImport( const TCHAR *name, ImpInterface *ii,Interface *i, BOOL suppressPrompts )
{
	_log.Print( _T("Ubi Import TRL v. %s\r\n"), C_szVersionString );
	_log.Print( _T("\r\nStart importing file %s.\r\n"), name );

	// To remove when the importer will work...
	_log.Print(_T("Warning : Beta version of TRL importer!\r\n          Doesn't import TRL exported from Jade!\r\n"));

	// Init Member variables
	_ip = i;
	GetFileNameAndDirectory (name);

	//open dialog box to display infos
	// Get the nodes
	
	if (!GetNodes(suppressPrompts!=0))
	{
		_log.Print( _T("\r\nImport canceled.\r\n") );
	}
	else if(!NumNodes())
	{
		_log.PrintError(_T("Cannot find a suitable bones group or camera to generate a TRL.\r\n"));
	}
	
	else
	{
		// Attach pelvis to magic box
		_MagicBoxNode->AttachChild( _linkObjects.nodes.at(0).GetINode() );

		// Real import TRL
		SetTRLToMAX(name);
	}
	

	_log.Print( _T("\r\nEnd.\r\n") );

	if (!suppressPrompts)
	{
		if (_log.HasErrors())
			MessageBox( i->GetMAXHWnd(), _T("An error occurred. Please read the log."), _T("Ubi Import TRL"), MB_OK|MB_ICONERROR );
		else if (_log.HasWarnings())
			MessageBox( i->GetMAXHWnd(), _T("There are warnings. Please read the log."), _T("Ubi Import TRL"), MB_OK|MB_ICONWARNING );

		DialogBoxParam( hInstance, MAKEINTRESOURCE(IDD_LOG), i->GetMAXHWnd(), LogDlgProc, (LPARAM) NULL);
	}
	else
	{
		SaveLog(TRUE);
	}
	
	if (suppressPrompts && _log.HasErrors())
	{
		return FALSE;
	}
	
	return TRUE;
}

// Get the name of the file and the directory from the complete name (w path)
void CImportTRL::GetFileNameAndDirectory (const TCHAR *name)
{
	_tcscpy( _actorDir, name );
	TCHAR* p_temp = _tcsrchr( _actorDir, _T('\\') );
	if (p_temp)
	{
		// Get anim name
		_tcscpy( _animName, p_temp+1 );
		*_tcsrchr( _animName, _T('.') ) = 0;

		*p_temp = 0;

		// Get the actor name from the directory name
		p_temp = _tcsrchr( _actorDir , _T('\\') );
		if (p_temp)
		{
			_tcscpy( _actorName, p_temp + 1 );
		}

		_tcscat( _actorDir, _T("\\") );
	}
}

// Function that write a LOG file
void CImportTRL::SaveLog( BOOL bSuppressPrompts ) const
{
	TCHAR filename[MAX_PATH*2];
	_stprintf( filename, _T("%s%s_trl.log"), _actorDir, _animName );

	if (!bSuppressPrompts)
	{
		OPENFILENAME ofn;
		ofn.lStructSize =       sizeof(OPENFILENAME);
		ofn.hwndOwner =         _ip->GetMAXHWnd();
		ofn.hInstance =         NULL;
		ofn.lpstrFilter =       _T("Log files (*.log;*.txt)\0*.log;*.txt\0");
		ofn.lpstrCustomFilter = NULL;
		ofn.nMaxCustFilter =    0;
		ofn.nFilterIndex =      1;
		ofn.lpstrFile =         filename;
		ofn.nMaxFile =          MAX_PATH*2;
		ofn.lpstrFileTitle =    NULL;
		ofn.nMaxFileTitle =     0;
		ofn.lpstrInitialDir =   NULL;
		ofn.lpstrTitle =        NULL;
		ofn.Flags =             OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY;
		ofn.nFileOffset =       0;
		ofn.nFileExtension =    0;
		ofn.lpstrDefExt =       _T("log");
		ofn.lCustData =         0;
		ofn.lpfnHook =          NULL;
		ofn.lpTemplateName =    NULL;
		if (!GetSaveFileName( &ofn ))
			return;
	}

	FILE* file = _tfopen( filename, _T("wt") );
	if (!file)
	{
		if (!bSuppressPrompts)
			MessageBox( _ip->GetMAXHWnd(), _T("Error creating file"), NULL, 0 );
		return;
	}

	fputs( _log.GetBuffer(), file );
	fclose( file );
}

// Callback function call for controlling child window of LOG
BOOL CALLBACK CImportTRL::LogDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) 
	{
	case WM_INITDIALOG:
		{
			SetWindowText( GetDlgItem( hDlg, IDC_LOG ), _instance->_log.GetBuffer() );

			// Update the dialog title with the current version number
			TCHAR title[1024];
			_stprintf(title, _T("Jade Anim Importers v. %s - TRL - Log"), C_szVersionString );
			SetWindowText(hDlg, title);

			CenterWindow(hDlg,GetParent(hDlg));
			SetFocus(hDlg); // For some reason this was necessary.  DS-3/4/96
			return FALSE;
		}
	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case IDOK: 
			EndDialog(hDlg, 1);
			return TRUE;

		case IDC_SAVE_TO_FILE:
			_instance->SaveLog( FALSE );
			return TRUE;
		}
	}
	return FALSE;
}

// Get the Nodes 
bool CImportTRL::GetNodes(bool bSuppressPrompts)
{
	_linkObjects.nodes.clear();
	_linkObjects.objects.clear();

	CJadeBonesGroup jbg(*_ip, CJadeBonesGroup::EJadeBonesGroupUtility);
	CMagicBoxHitByNameDlgCallback mbhbncb(jbg.Group(), *_ip);

	// First check if there are more than one node named "Magic Box" in the scene
	if (mbhbncb.CountMagicBoxesInScene()>1)
	{
		_log.PrintError(_T("There are more than one node named \"Magic Box\" in the scene.\r\nYou have to rename them."));
		return false;
	}

	_MagicBoxNode = mbhbncb.GetMagicBoxNode();

	if (bSuppressPrompts && !_MagicBoxNode)
	{
		_log.PrintError(_T("There are no \"Magic Box\" in the scene."));
		return false;
	}

	// Try to load the group from a gas file
	TCHAR szPath[MAX_PATH];
	TCHAR szTemp[MAX_PATH] = _T("");
	WIN32_FIND_DATA stFindData;
	HANDLE h;

	// First search in the default directory, if we are using a standard hierarchy
	_tcscpy( szPath, GetActorDir() );
	_tcslwr( szPath );
	TCHAR* pChar = _tcsstr( szPath, _T("\\anims\\") );
	if (pChar)
	{
		_tcscpy( pChar, _T("\\Model\\") );
		_stprintf( szTemp, _T("%s*.gas"), szPath );
		h = FindFirstFile( szTemp, &stFindData );
		if (h!=INVALID_HANDLE_VALUE )
		{
			_stprintf( szTemp, _T("%s%s"), szPath, stFindData.cFileName );
			jbg.ProcessGas(szTemp, &_log);

			FindClose(h);
		}
	}

	if (jbg.Group().empty())
	{
		// Otherwise, search in the same directory
		_stprintf( szTemp, _T("%s*.gas"), GetActorDir() );

		h = FindFirstFile( szTemp, &stFindData );
		if (h!=INVALID_HANDLE_VALUE )
		{
			_stprintf( szTemp, _T("%s%s"), GetActorDir(), stFindData.cFileName );
			jbg.ProcessGas(szTemp, &_log);

			FindClose(h);
		}
	}

	if (!bSuppressPrompts)
	{
		if (jbg.DoModal()!=1)
			return false;
		CGetParamDlg dlg(*_ip);
#ifndef AUTO_SKIP_TRANS
		mb_SkipTranslations = dlg.DoModal();
#else
		mb_SkipTranslations = 1;
#endif
	}

	jbg.LogGroup(_log);

	if (!jbg.ValidateGroup(&_log))
		return false;

	if (jbg.Group().empty())
		return true;

	const int numObjects = jbg.Group().size() + 1;
	_linkObjects.nodes.reserve(numObjects);
	_linkObjects.objects.reserve(numObjects);

	MATH_tdst_Vector st_LocalPos = {0.0f, 0.0f, 0.0f};
	_linkObjects.objects.push_back( CBankObject( C_az_MagicBoxName, ANI_C_MagicBoxBone, ANI_Cuc_ThisIsAMainBone, st_LocalPos));

	int i=0;
	for (CJadeBoneList::const_iterator itBone=jbg.Group().begin(); itBone!=jbg.Group().end(); ++itBone)
	{
		int parent = ANI_Cuc_ThisIsAMainBone;
		MATH_SetNulVector(&st_LocalPos);

		if (itBone->GetParent()!=-1 && jbg.IsInitLocalPosValid())
		{
			parent = itBone->GetParent()+1;
			const float* initLocalPos = itBone->GetInitLocalPos();
			MATH_InitVector(&st_LocalPos, initLocalPos[0], initLocalPos[1], initLocalPos[2]);
		}

		_linkObjects.objects.push_back( CBankObject( itBone->GetNode().GetName(), itBone->GetIndex()+1, parent, st_LocalPos));
		if (itBone->GetIndex()<jbg.GetNumAnimatables())
		{
			if(!itBone->IsNotAnimated() || GetbImportNonAnimatableBones())
			{
				_linkObjects.nodes.push_back(CNodeInfo( i++, &itBone->GetNode(), TRUE ));
				_linkObjects.objects.back().SetNode(&_linkObjects.nodes.back());
			}
		}
	}

	while (!_MagicBoxNode)
	{
		if (!_ip->DoHitByNameDialog(&mbhbncb))
			return false;
	}

	_linkObjects.nodes.push_back(CNodeInfo( i, _MagicBoxNode, TRUE ));
	_linkObjects.objects[0].SetNode(&_linkObjects.nodes.back());
	_log.Print(_T("Magic Box: %s\r\n"), _MagicBoxNode->GetName());
	_animParams.bIsInitLocalPosValid = jbg.IsInitLocalPosValid();
	_MaxNode = NumNodes();

#ifdef LOG_DEBUG
	_log.Print(_T("MAX Info : \r\n"));
	_log.Print(_T("Bank Name : %s\r\n"),_linkObjects.szBankName);
	
	_log.Print(_T("Contenu des Nodes : \r\n"));
	for (int a =0; a < NumNodes();a++)
	{
		_log.Print(_T("Node %i : %s\r\n"), a, (_linkObjects.nodes.at(a)).GetName());
	}

	_log.Print(_T("Contenu des Objets : \r\n"));
	for (int a =0; a < _linkObjects.objects.size();a++)
	{
		_log.Print(_T("Object %i : %s\r\n"), a, (_linkObjects.objects.at(a)).GetName());		
	}
#endif

	return true;
}


// Set Animation To Bone
BOOL CImportTRL::SetTRLToMAX( const TCHAR *name )
{
	FILE*  f_TRLFile;
	USHORT uw_NumTracks, uw_GlobFlags;

	f_TRLFile = fopen(name,"rb");
	if (f_TRLFile == NULL)
	{
		_log.PrintError(_T("TRL file is not valid (ERROR OPENING)!\r\n"));
		return FALSE;
	}

	fread(&uw_NumTracks, 1, sizeof(USHORT), f_TRLFile);
	fread(&uw_GlobFlags, 1, sizeof(USHORT), f_TRLFile);

#ifdef LOG_DEBUG
	_log.Print(_T("# of track and Global Flag : (%i,%i)\r\n"),uw_NumTracks,uw_GlobFlags);
#endif
    _ip->ProgressStart(_T("Reading Tracks"), TRUE, fnimp, 0);

	
	AnimateOn();

	for (int i = 1; i <= uw_NumTracks; i++)
	{
#ifdef LOG_DEBUG
		_log.Print(_T("===========================================\r\n"));
		_log.Print(_T("Track #%i : "),i);
#endif
		
        _ip->ProgressUpdate(i*100/uw_NumTracks);

		if (TrackRead(f_TRLFile) == FALSE)
		{
			AnimateOff();
			_ip->ProgressEnd();
			
			if (fclose(f_TRLFile))
			{
				_log.PrintError(_T("ERROR CLOSING FILE!\r\n"));		
			}
			return FALSE;
		}
	}

	AnimateOff();

	_ip->ProgressEnd();

	if (fclose(f_TRLFile))
	{
		_log.PrintError(_T("ERROR CLOSING FILE!\r\n"));
		return FALSE;
	}

	return TRUE;
}

// Read a track
int CImportTRL::TrackRead(FILE*  f_TRLFile)
{
	int		i_NumOfEvent;
	ULONG	ul_DataLength, ul_Flags;
	USHORT	uw_Gizmo, uw_TrackFlags;
	int		iCurrentFrame = 0;

	if (feof(f_TRLFile))
	{
		_log.PrintError(_T("\nERROR!! UNEXPECTED OEF!\r\n"));
		return FALSE;
	}

	fread(&uw_TrackFlags, 1, sizeof(USHORT), f_TRLFile);
	fread(&uw_Gizmo, 1, sizeof(USHORT), f_TRLFile);
	fread(&ul_DataLength, 1, sizeof(ULONG), f_TRLFile);
	fread(&ul_Flags, 1, sizeof(ULONG), f_TRLFile);
	fread(&i_NumOfEvent, 1, sizeof(int), f_TRLFile);


#ifdef LOG_DEBUG
	INode *TransfoNode;
	if (uw_Gizmo != C_TRL_uw_MagicBox)
		TransfoNode = _linkObjects.nodes.at(uw_Gizmo).GetINode();
	else
		TransfoNode = _MagicBoxNode;
	_log.Print(_T( "%s (%i,%i,%i,%i,%i)\r\n" ),
		TransfoNode->GetName(),
		uw_TrackFlags,
		uw_Gizmo,
		ul_DataLength,
		ul_Flags,
		i_NumOfEvent);
#endif

	if (uw_Gizmo > _MaxNode && uw_Gizmo != USHORT(-1))
	{
		_log.PrintError(_T("\nERROR!! The TRL describle movements on more bone than there is in MAX file!\r\n"));
		_log.PrintError(_T("There is %i bones in Max file\r\n"), _MaxNode+1);
		_log.PrintError(_T("The TRL must be for another model...\r\n"));
		return FALSE;
	}

	for (int j = 1; j <= i_NumOfEvent; j++)
	{
#ifdef LOG_DEBUG
		_log.Print(_T("\r\nEvent %i : "),j);
#endif

		if (EventRead(f_TRLFile, j, uw_Gizmo, &iCurrentFrame) == FALSE)
		{
			return FALSE;
		}
	}

	return TRUE;
}

// Read an event
int CImportTRL::EventRead(FILE*  f_TRLFile, int EventNum, int uw_Gizmo, int* iCurrentFrame )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ul_Num;
	USHORT					uw_NumFrames, uw_Size, uw_Type;
	MATH_tdst_Quaternion	st_RotQ;
	MATH_tdst_Vector		st_Transl;
	AI_tdst_Node			ast_AINodes[7];
	short					wFlags, wNumTrack, wNumEvent;
	int						iCurrentTime;
	Point3					CurrentTransfoData;
	Quat					TransformQuat;
	INode					*TransfoNode;
	Control					*TransfoCont;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (uw_Gizmo == C_TRL_uw_MagicBox)
		// Magic Box case
		TransfoNode = _MagicBoxNode;
	else
		TransfoNode = _linkObjects.nodes.at(uw_Gizmo).GetINode();

	fread(&uw_NumFrames, 1, sizeof(USHORT), f_TRLFile);
#ifdef LOG_DEBUG
	_log.Print(_T("(%i,"),uw_NumFrames);
#endif

	iCurrentTime =  TICK_PER_FRAME * (int)(*iCurrentFrame);

	// Number of frame should be OK on the first set of Data...
	if (_MaxFrame < uw_NumFrames)
	{
		_MaxFrame = uw_NumFrames;
		_ip->SetAnimRange(Interval(0,uw_NumFrames * TICK_PER_FRAME));
	}

	fread(&wFlags, 1, sizeof(short), f_TRLFile);

	if (feof(f_TRLFile))
	{
		_log.PrintError(_T("\r\nERROR!! UNEXPECTED OEF!\r\n"));
		return FALSE;
	}

	if (wFlags & EVE_C_EventFlag_InterpolationKey)
	{			
#ifdef LOG_DEBUG
		_log.Print(_T("%i)\r\n"),wFlags);
#endif

		if (wFlags & EVE_C_EventFlag_Symetric)
		{
			// Ignore this type of event
			fread(&wNumTrack, 1, sizeof(short), f_TRLFile);
			fread(&wNumEvent, 1, sizeof(short), f_TRLFile);
#ifdef LOG_DEBUG
			_log.Print(_T("# of track : %i, # of events : %i \r\n"),wNumTrack,wNumEvent);			
#endif
		}
		else
		{
			fread(&uw_Size, 1, sizeof(USHORT), f_TRLFile);
			fread(&uw_Type, 1, sizeof(USHORT), f_TRLFile);
#ifdef LOG_DEBUG
			_log.Print(_T("Size and Type : (%i,%i)\r\n"),uw_Size,uw_Type);			
#endif

			if (uw_Type & EVE_InterKeyType_Rotation_Quaternion)
			{
				// Quaternion Reading
				fread(&st_RotQ, 1, sizeof(MATH_tdst_Quaternion), f_TRLFile);
#ifdef LOG_DEBUG
				_log.Print(_T("Quat : (%f,%f,%f,%f)\r\n"), st_RotQ.x, st_RotQ.y, st_RotQ.z, st_RotQ.w);
#endif
				// Transformation Quaternion will be :
				TransformQuat.Set(st_RotQ.x, st_RotQ.y, st_RotQ.z, -st_RotQ.w);	

#ifndef OUTPUT_TRL_ONLY
				// Apply transfo
				TransfoCont = TransfoNode->GetTMController()->GetRotationController();
				// We want linear interpolation
				if (TransfoCont->ClassID() != Class_ID(LININTERP_ROTATION_CLASS_ID, 0))
				{
					DWORD f = TransfoCont->GetInheritanceFlags();
					TransfoCont = (Control*)CreateInstance(CTRL_ROTATION_CLASS_ID, Class_ID(LININTERP_ROTATION_CLASS_ID, 0));
					// The following method takes care of deleting the old controller
					TransfoNode->GetTMController()->SetRotationController(TransfoCont);
					TransfoCont->SetInheritanceFlags(f, TRUE);
				}
				// If bone = pelvis, change transformation so that it's relative to magic box
				if (!uw_Gizmo)
				{
					Matrix3 relativeTransfo;
					TransformQuat.MakeMatrix(relativeTransfo);
					// We assume that the Magic Box's track has already been read and keyed
					relativeTransfo *= Inverse(_MagicBoxNode->GetNodeTM(iCurrentTime));
					TransformQuat = Quat( relativeTransfo );
				}
				TransfoCont->SetValue(iCurrentTime, &TransformQuat);
#endif
			}
			else 
			{									
				// Translation vector reading
				fread(&st_Transl, 1, sizeof(MATH_tdst_Vector), f_TRLFile);
#ifdef LOG_DEBUG
				_log.Print(_T("Trans : (%f,%f,%f)\r\n"), st_Transl.x, st_Transl.y, st_Transl.z);
#endif
				
				// Skip translation ? Never skip translations on magic box and pelvis
				if (!mb_SkipTranslations || !uw_Gizmo || (uw_Gizmo == C_TRL_uw_MagicBox))
				{
					// Transform MATH_tdst_Vector into Point3
					CurrentTransfoData.Set(st_Transl.x, st_Transl.y, st_Transl.z);
					// Don't forget convertion factor
					CurrentTransfoData *= CONVERTION_FACTOR;

#ifndef OUTPUT_TRL_ONLY
					// Apply transfo
					TransfoCont = TransfoNode->GetTMController()->GetPositionController();
					// We want linear interpolation
					if (TransfoCont->ClassID() != Class_ID(LININTERP_POSITION_CLASS_ID, 0))
					{
						DWORD f = TransfoCont->GetInheritanceFlags();
						TransfoCont = (Control*)CreateInstance(CTRL_POSITION_CLASS_ID, Class_ID(LININTERP_POSITION_CLASS_ID, 0));
						// The following method takes care of deleting the old controller
						TransfoNode->GetTMController()->SetPositionController(TransfoCont);
						TransfoCont->SetInheritanceFlags(f, TRUE);
					}
					// If bone = pelvis, change transformation so that it's relative to magic box
					if (!uw_Gizmo)
					{
						Matrix3 relativeTransfo;
						relativeTransfo.IdentityMatrix();
						relativeTransfo.SetRow(3, CurrentTransfoData);
						// We assume that the Magic Box's track has already been read and keyed
						relativeTransfo *= Inverse(_MagicBoxNode->GetNodeTM(iCurrentTime));
						CurrentTransfoData = relativeTransfo.GetRow(3);
					}
					TransfoCont->SetValue(iCurrentTime, &CurrentTransfoData);
#endif
				}
			}
		}
	}			
	// This data is ignore for now... (No info necessary for the animation)
	else if (wFlags & EVE_C_EventFlag_AIFunction)
	{
#ifdef LOG_DEBUG
		_log.Print(_T("%i)\r\n"),wFlags);
#endif 

		fread(&wFlags, 1, sizeof(short), f_TRLFile);
		fread(&ul_Num, 1, sizeof(ULONG), f_TRLFile);

		if (ul_Num > 7)
		{
			_log.PrintError(_T("Data Corruption\r\n"));
			return FALSE;
		}

		for (ULONG i = 0; i < ul_Num; i++)
		{
			fread(&(ast_AINodes[i]), sizeof(AI_tdst_Node), 1, f_TRLFile);
		}

#ifdef LOG_DEBUG
		_log.Print(_T("Specific flag is %i \r\n"),wFlags);
		_log.Print(_T("Num is %i \r\n"),ul_Num);
		_log.Print(_T("Passing the AI function structure... \r\n"));
#endif
	}

	(*iCurrentFrame) = (*iCurrentFrame) + uw_NumFrames;

	return TRUE;
}

