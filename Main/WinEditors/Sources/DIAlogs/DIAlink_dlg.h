/*$T DIAlink_dlg.h GC! 1.098 10/19/00 18:05:35 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIfile.h"
#include "DIAlogs/DIAbase.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define EDI_C_MaxLenPassWord	16
class EDIA_cl_LinkControlPassWord : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_LinkControlPassWord(BOOL _b_CurrentEnable = TRUE);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	CString mo_CurrentPassWord;
	CString mo_PassWord;
	CString mo_Title;
	BOOL	mb_CurrentEnable;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	virtual void	DoDataExchange(CDataExchange *);
	virtual void	OnOK(void);
};

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EDIA_cl_LinkControlDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_LinkControlDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	mb_Open;
	int		mi_LinkControlON;
	CString mo_ProjectName;
	BOOL	mb_CanSetUserName;
	int		mi_CanWrite;

	CString mo_UserName;
	CString mo_UserPassWord;
	char	masz_LinkControlFileVSS[L_MAX_PATH];
	char	masz_LinkControlFileREF[L_MAX_PATH];
	char	masz_CurrentVSSFile[L_MAX_PATH];

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	CanSetUserName(void);
	void	EnableContent(BOOL);
	void	RefreshLockList(void);
	void	EnableAdmin(BOOL);

	BOOL	b_CheckPath(char *);
	BOOL	b_CheckUserName(char *);

	void	DeleteUser(char *, BOOL);
	BOOL	b_AddUser(char *, char *, int *, BOOL);
	BOOL	b_GetUser(char *, char *, int *, BOOL);
	BOOL	b_CheckCurrentUser(void);

	void	GetUserSelected(char *);
	BOOL	b_CreateIfNotExists(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	virtual void	DoDataExchange(CDataExchange *);
	virtual BOOL	OnInitDialog(void);
	virtual void	OnOK(void);
	virtual void	OnCancel(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

protected:
	afx_msg void	OnCheck(void);
	afx_msg void	OnBrowse(void);
	afx_msg void	OnPassWord(void);
	afx_msg void	OnShare(void);
	afx_msg void	OnAddUser(void);
	afx_msg void	OnDeleteUser(void);
	afx_msg void	OnEraseLock(void);
	afx_msg void	OnCheckR(void);
	afx_msg void	OnExit(void);
	afx_msg void	OnKillRights(void);

	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
