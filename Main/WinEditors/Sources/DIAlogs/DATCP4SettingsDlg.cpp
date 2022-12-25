//------------------------------------------------------------------------------
// Filename   :DATCP4SettingsDlg.cpp
/// \author    YCharbonneau
/// \date      18/03/2005
/// \par       Description: Implementation of DATCP4SettingsDlg
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DATCP4SettingsDlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles\LOAding\LOAdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "F3Dframe/F3Dview.h"
#include "EDIpaths.h"
#include "EDImainframe.h"
#include "Res/Res.h"

#include "DATaControl/DATCPerforce.h"
#include "BIGfiles\STReams\STReamaccess.h"



//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------

#define P4_PROD_PORT "srvperforce-mtl:4666" 
#define P4_PROD_RESOLVED_PORT_MONTPELLIER "10.5.0.2:1666"
#define P4_PROD_RESOLVED_PORT_MONTREUIL "10.0.0.2:1668"


//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(DAT_CP4SettingsDlg, EDIA_cl_BaseDialog)
	ON_CBN_SELCHANGE	(IDC_P4SERVER,OnP4ServerChanged)
	ON_BN_CLICKED		(IDC_P4REFRESHDEPOTS,OnP4ServerChanged)
	ON_CBN_SELCHANGE	(IDC_P4DEPOTS,OnP4DepotsChanged)
	ON_CBN_SELCHANGE	(IDC_P4USERNAME,OnP4UsernameChanged)
	ON_BN_CLICKED		(IDC_P4REFRESHVERSION,OnP4DepotsChanged)
	ON_CBN_SELCHANGE	(IDC_P4DEPOTVERSION,OnP4VersionChanged)
	ON_BN_CLICKED		(IDC_P4CLEARSTREAM,OnClearSettings)
END_MESSAGE_MAP()

//------------------------------------------------------------
//   DAT_CP4SettingsDlg::DAT_CP4SettingsDlg(const char* _pszServer,
/// \author    YCharbonneau
/// \date      02/07/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
DAT_CP4SettingsDlg::DAT_CP4SettingsDlg(const char* _pszServer,
									   const char* _pszDepot,
									   const char* _pszVersion,
									   const char* _pszHost,
									   const char* _pszUser,
									   const char* _pszPassword,
									   CWnd* _pParentWnd ) :
EDIA_cl_BaseDialog(IDD_P4SETTINGS,_pParentWnd),
m_strP4Port(_pszServer),
m_strP4Depot(_pszDepot),
m_strBFVersion(_pszVersion),
m_strP4User(_pszUser),
m_strP4Host(_pszHost),
m_strP4Password(_pszPassword)
{
	
}

//------------------------------------------------------------
//   BOOL DAT_CP4SettingsDlg::OnInitDialog()
/// \author    YCharbonneau
/// \date      02/07/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL DAT_CP4SettingsDlg::OnInitDialog()
{
	EDIA_cl_BaseDialog::OnInitDialog();

	CComboBox* pP4Server	= (CComboBox*)	GetDlgItem(IDC_P4SERVER);
	CComboBox* pP4Versions	= (CComboBox* )	GetDlgItem(IDC_P4DEPOTVERSION);

	pP4Server->ResetContent();
	pP4Server->AddString(P4_PROD_PORT);
	pP4Server->AddString(P4_PROD_RESOLVED_PORT_MONTPELLIER);
    pP4Server->AddString(P4_PROD_RESOLVED_PORT_MONTREUIL);

	((CButton*)GetDlgItem(IDC_P4SAVEDEPOT))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_P4SAVEVERSION))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_P4SAVEUSERNAME))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_P4SAVEPASSWORD))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_P4SAVEPORT))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_P4SAVEHOST))->SetCheck(FALSE);

	if ( m_strP4Port == P4_PROD_PORT ) 
	{
		pP4Server->SetCurSel(0);
	}
    else if ( m_strP4Port == P4_PROD_RESOLVED_PORT_MONTPELLIER ) 
	{
		pP4Server->SetCurSel(1);
	}
    else if ( m_strP4Port == P4_PROD_RESOLVED_PORT_MONTREUIL ) 
	{
		pP4Server->SetCurSel(2);
	}
	else if ( m_strP4Port != "" ) 
	{
		static std::string strDefaultPort = m_strP4Port;
		pP4Server->AddString(strDefaultPort.c_str());
		pP4Server->SetCurSel(3);
	}
	
	if ( m_strP4Port != "" )
	{
		OnP4ServerChanged();
	}

	return TRUE;
}

//------------------------------------------------------------
//   void DAT_CP4SettingsDlg::OnP4ServerChanged()
/// \author    YCharbonneau
/// \date      02/07/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CP4SettingsDlg::OnP4ServerChanged()
{
	CComboBox* pP4Server	= (CComboBox*)	GetDlgItem(IDC_P4SERVER);
	CComboBox* pP4Depots	= (CComboBox*)	GetDlgItem(IDC_P4DEPOTS);

	pP4Depots->ResetContent();

	CString strP4Server; 
	if ( pP4Server->GetCurSel() >  CB_ERR )
	{
		pP4Server->GetLBText(pP4Server->GetCurSel(),strP4Server);
	}
	else 
	{
		((CEdit*)pP4Server->GetWindow(GW_CHILD))->GetWindowText(strP4Server);
	}

	if ( strP4Server != "" ) 
	{

		DAT_CPerforce::GetInstance()->Initialize(	strP4Server.GetBuffer(),
			m_strP4Host.c_str(),
			m_strP4User.c_str(),
			m_strP4Password.c_str()
			);

		if ( DAT_CPerforce::GetInstance()->P4Connect() ) 
		{
			CComboBox* pP4Username	= (CComboBox*)	GetDlgItem(IDC_P4USERNAME);
			CEdit* pP4Password	= (CEdit*)	GetDlgItem(IDC_P4PASSWORD);
			pP4Username->ResetContent();

			std::list<std::string> lstUsers;
			DAT_CPerforce::GetInstance()->P4Users(lstUsers);

			for ( std::list<std::string>::iterator iter = lstUsers.begin();
				iter != lstUsers.end(); iter ++ ) 
			{
				int i = pP4Username->AddString((*iter).c_str());
				if ( m_strP4User == (*iter) ) 
				{
					pP4Username->SetCurSel(	i );
					if ( m_strP4Password == "" ) 
					{
						m_strP4Password = m_strP4User + P4_DEFAULT_PASSWORD; 
					}
					pP4Password->SetWindowText(m_strP4Password.c_str());
				}
			}

			m_strP4Port = strP4Server.GetBuffer();
			UpdateDepots();
			DAT_CPerforce::GetInstance()->P4Disconnect();
		}
		else 
		{
			m_strP4Port = "";

			CComboBox* pP4Version	= (CComboBox*)	GetDlgItem(IDC_P4DEPOTVERSION);
			pP4Version->ResetContent();
			CComboBox* pP4Depot	= (CComboBox*)	GetDlgItem(IDC_P4DEPOTS);
			pP4Depot->ResetContent();

			MessageBox("Cannot connect to server, check server name and port","ERROR",MB_OK);
		}
	}
}

void DAT_CP4SettingsDlg::OnP4VersionChanged()
{
	CComboBox* pP4Version	= (CComboBox*)	GetDlgItem(IDC_P4DEPOTVERSION);
	CString strP4Version; 
	if ( pP4Version->GetCurSel() >  CB_ERR )
	{
		pP4Version->GetLBText(pP4Version->GetCurSel(),strP4Version);
	}
	else 
	{
		((CEdit*)pP4Version->GetWindow(GW_CHILD))->GetWindowText(strP4Version);
	}

	m_strBFVersion = strP4Version.GetBuffer();

	if (  m_strBFVersion != ""  ) 
	{
		pP4Version->SelectString(0,m_strBFVersion.c_str());
		static std::list<std::string> s_lstHosts;
		s_lstHosts.clear();
		GetP4Hosts(m_strBFVersion.c_str(),s_lstHosts);

		CComboBox* pP4Hosts		= (CComboBox*)	GetDlgItem(IDC_P4HOST);
		pP4Hosts->ResetContent();

		for ( std::list<std::string>::iterator iterHost = s_lstHosts.begin();
			iterHost != s_lstHosts.end(); iterHost ++ ) 
		{
			int i = pP4Hosts->AddString((*iterHost).c_str());
			if ( m_strP4Host == (*iterHost) ) 
			{
				pP4Hosts->SetCurSel(i);
			}
		}
	}
}

void DAT_CP4SettingsDlg::OnP4UsernameChanged()
{
	CComboBox* pP4Username	= (CComboBox*)	GetDlgItem(IDC_P4USERNAME);
	CComboBox* pP4UserPassword	= (CComboBox*)	GetDlgItem(IDC_P4PASSWORD);

	CString strP4Username; 
	if ( pP4Username->GetCurSel() >  CB_ERR )
	{
		pP4Username->GetLBText(pP4Username->GetCurSel(),strP4Username);
	}
	else 
	{
		((CEdit*)pP4Username->GetWindow(GW_CHILD))->GetWindowText(strP4Username);
	}
	m_strP4User = strP4Username;
	m_strP4Password = m_strP4User + P4_DEFAULT_PASSWORD; 
	pP4UserPassword->SetWindowText(m_strP4Password.c_str());
}

//------------------------------------------------------------
//   void DAT_CP4SettingsDlg::OnP4DepotsChanged()
/// \author    YCharbonneau
/// \date      02/07/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CP4SettingsDlg::OnP4DepotsChanged()
{

	CComboBox* pP4Depots	= (CComboBox*)	GetDlgItem(IDC_P4DEPOTS);

	CString strP4Depot; 
	if ( pP4Depots->GetCurSel() >  CB_ERR )
	{
		pP4Depots->GetLBText(pP4Depots->GetCurSel(),strP4Depot);
	}
	else 
		return;


	if ( strP4Depot != "" ) 
	{
		// getting the versions
		static std::list<std::string> s_lstVersion;
		s_lstVersion.clear();
		GetBigFileVersion(strP4Depot.GetBuffer(),s_lstVersion);
		

		m_strP4Depot = strP4Depot.GetBuffer();
		
		CComboBox* pP4Version	= (CComboBox*)	GetDlgItem(IDC_P4DEPOTVERSION);
		pP4Version->ResetContent();

		for ( std::list<std::string>::iterator iter = s_lstVersion.begin();
			iter != s_lstVersion.end(); iter ++ ) 
		{
			pP4Version->AddString((*iter).c_str());
		}
		OnP4VersionChanged();
	}
	else 
	{
		m_strP4Depot = "";
	}
}

//------------------------------------------------------------
//   void DAT_CP4SettingsDlg::OnClearSettings()
/// \author    YCharbonneau
/// \date      02/07/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CP4SettingsDlg::OnClearSettings()
{
	SetStreamByName(BIG_gst.asz_Name ,STREAM_P4DEPOT	,""); 
	SetStreamByName(BIG_gst.asz_Name ,STREAM_BFVERSION	,""); 
	SetStreamByName(BIG_gst.asz_Name ,STREAM_BFVERSION	,""); 
	SetStreamByName(BIG_gst.asz_Name ,STREAM_P4USER		,""); 
	SetStreamByName(BIG_gst.asz_Name ,STREAM_P4PASSWORD	,""); 
	SetStreamByName(BIG_gst.asz_Name ,STREAM_P4PORT		,""); 
	SetStreamByName(BIG_gst.asz_Name ,STREAM_P4HOST		,""); 
}

//------------------------------------------------------------
//   BOOL DAT_CP4SettingsDlg::GetBigFileVersion(const char* _pszP4Depot,std::list<std::string>& _lstVersion)
/// \author    YCharbonneau
/// \date      02/07/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL DAT_CP4SettingsDlg::GetBigFileVersion(const char* _pszP4Depot,std::list<std::string>& _lstVersion)
{
	// client view has not been set wet.
	DAT_CPerforce Perforce;
	if ( Perforce.P4Connect() )
	{
		std::list<std::string> lstDirs;

		std::string strDir = "//";
		strDir += _pszP4Depot ;
		strDir += "/";
		strDir += P4_VERSIONROOT;
		strDir += "*";
		Perforce.P4Dirs(strDir.c_str(),lstDirs);

		///*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		///*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~«~~~~~~~~~~~~*/

		STL::list<STL::string>::iterator iter= lstDirs.begin();
		UINT ui = 0;

		for ( iter; iter != lstDirs.end(); iter++ ) 
		{
			char* pszVersion = const_cast<char*>((*iter).c_str()) ;
			pszVersion += strDir.length() - strlen("*") ;
			_lstVersion.push_back(pszVersion);
			ui++;
		}
		Perforce.P4Disconnect();
		return TRUE;
	}
	return FALSE;

}

//------------------------------------------------------------
//   BOOL DAT_CP4SettingsDlg::GetP4Hosts(const char* _pszVersion, std::list<std::string>& _lstHosts)
/// \author    YCharbonneau
/// \date      02/07/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL DAT_CP4SettingsDlg::GetP4Hosts(const char* _pszVersion, std::list<std::string>& _lstHosts)
{
	DAT_CPerforce Perforce;
	if ( Perforce.P4Connect() )
	{
		std::list<std::string> lstHosts;
		Perforce.P4Clients(lstHosts);

		STL::list<STL::string>::iterator iter= lstHosts.begin();

		for ( iter; iter != lstHosts.end(); iter++ ) 
		{
			char* pszHost = const_cast<char*>((*iter).c_str()) ;
			char * pVersion = strstr(pszHost,_pszVersion);
			if ( pVersion ) 
			{
				pVersion -= 1				;
				pVersion[0] = 0;
				_lstHosts.push_back(pszHost);
			}
		}
		Perforce.P4Disconnect();
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------
//   void DAT_CP4SettingsDlg::UpdateDepots()
/// \author    YCharbonneau
/// \date      02/07/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CP4SettingsDlg::UpdateDepots()
{
	static std::vector<std::string> s_vDepot;
	s_vDepot.clear();
	DAT_CPerforce::GetInstance()->P4Depots(s_vDepot);

	CComboBox*  pP4Depots	= (CComboBox* )	GetDlgItem(IDC_P4DEPOTS);
	pP4Depots->ResetContent();

	for ( UINT ui = 0 ; ui < s_vDepot.size() ; ui ++ ) 
	{
		pP4Depots->AddString(s_vDepot[ui].c_str());
	}
	if ( m_strP4Depot != "" ) 
	{
		pP4Depots->SelectString(0,m_strP4Depot.c_str());
		OnP4DepotsChanged();
	}

}

//------------------------------------------------------------
//   void DAT_CP4SettingsDlg::OnOK()
/// \author    YCharbonneau
/// \date      02/07/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CP4SettingsDlg::OnOK()
{

	CComboBox* pP4Host	= (CComboBox*)	GetDlgItem(IDC_P4HOST);
	CString strP4Host; 
	if ( pP4Host->GetCurSel() >  CB_ERR )
	{
		pP4Host->GetLBText(pP4Host->GetCurSel(),strP4Host);
	}
	else 
	{
		((CEdit*)pP4Host->GetWindow(GW_CHILD))->GetWindowText(strP4Host);
	}
	m_strP4Host = strP4Host.GetBuffer();

	if ( m_strP4Port	!= "" && 
		 m_strP4Depot	!= "" &&
		 m_strBFVersion != "" )
	{
		if ( ((CButton*)GetDlgItem(IDC_P4SAVEDEPOT))->GetCheck() ) 
		{
			SetStreamByName(BIG_gst.asz_Name ,STREAM_P4DEPOT	,m_strP4Depot.c_str()); 
		}
		if ( ((CButton*)GetDlgItem(IDC_P4SAVEVERSION))->GetCheck() ) 
		{
			SetStreamByName(BIG_gst.asz_Name ,STREAM_BFVERSION	,m_strBFVersion.c_str()	); 
		}
		if ( ((CButton*)GetDlgItem(IDC_P4SAVEUSERNAME))->GetCheck() ) 
		{
			SetStreamByName(BIG_gst.asz_Name ,STREAM_P4USER		,m_strP4User.c_str()	); 
		}
		if ( ((CButton*)GetDlgItem(IDC_P4SAVEPASSWORD))->GetCheck() ) 
		{
			SetStreamByName(BIG_gst.asz_Name ,STREAM_P4PASSWORD	,m_strP4Password.c_str()); 
		}
		if ( ((CButton*)GetDlgItem(IDC_P4SAVEPORT))->GetCheck() ) 
		{
			SetStreamByName(BIG_gst.asz_Name ,STREAM_P4PORT		,m_strP4Port.c_str()); 
		}
		if ( ((CButton*)GetDlgItem(IDC_P4SAVEHOST))->GetCheck() ) 
		{
			SetStreamByName(BIG_gst.asz_Name ,STREAM_P4HOST	,m_strP4Host.c_str()); 
		}

		EDIA_cl_BaseDialog::OnOK();
	}
	else
	{
		MessageBox("Bigfile setup not completed. Select a valid server, depot and version or select work disconnected.","ERROR",MB_OK);
	}
}



//------------------------------------------------------------------------------

#endif /* ACTIVE_EDITORS */
