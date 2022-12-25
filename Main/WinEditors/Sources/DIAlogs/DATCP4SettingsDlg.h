/*$T DIAname_dlg.h GC! 1.078 03/16/00 10:29:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include <string>
#include <list>

#define P4_DEFAULT_PASSWORD "jadep4api"
#define P4_VERSIONROOT "version/"

class DAT_CP4SettingsDlg : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	DAT_CP4SettingsDlg (const char* _pszServer,
						const char* _pszDepot,
						const char* _pszVersion, 
						const char* _pszHost,
						const char* _pszUser,
						const char* _pszPassword,
						CWnd* _pParentWnd );


public:
	afx_msg void 	OnOK (void);
	afx_msg BOOL 	OnInitDialog (void);

	void GetP4Port		(char* _pszP4Port)		{ strcpy(_pszP4Port,m_strP4Port.c_str()); }
	void GetP4Depot		(char* _pszP4Depot)		{ strcpy(_pszP4Depot,m_strP4Depot.c_str()); }
	void GetBfVersion	(char* _pszBfVersion)	{ strcpy(_pszBfVersion,m_strBFVersion.c_str()); }
	void GetP4Username	(char* _pszP4Username)	{ strcpy(_pszP4Username,m_strP4User.c_str()); }
	void GetP4Password	(char* _pszP4Password)	{ strcpy(_pszP4Password,m_strP4Password.c_str()); }
	void GetP4Host		(char* _pszP4Host)		{ strcpy(_pszP4Host,m_strP4Host.c_str()); }
	




private:
	afx_msg void	OnP4ServerChanged();
	afx_msg void	OnP4DepotsChanged();
	afx_msg void	OnClearSettings();
	afx_msg void	OnP4UsernameChanged();
	afx_msg void	OnP4VersionChanged();
	
	BOOL			GetBigFileVersion	(const char* _pszP4Depot,std::list<std::string>& _lstVersion);
	BOOL			GetP4Hosts			(const char* _pszVersion, std::list<std::string>& _lstHosts);
	void			UpdateDepots();

	std::string m_strP4User;
	std::string m_strP4Host;
	std::string m_strP4Password;

	std::string m_strBFVersion;
	std::string m_strP4Depot;
	std::string m_strP4Port;

	CComboBox* m_pP4Server;
	CListBox*  m_pP4Depots;
	CListBox*  m_pP4Versions;

	DECLARE_MESSAGE_MAP()
};


#endif /* ACTIVE_EDITORS */
