/*$T PERCheckframe.h GC 1.138 10/14/04 14:40:27 */


/*$6
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/


#pragma once
#ifdef ACTIVE_EDITORS
#include "EDIbaseframe.h"

#include "PERCheckframe_act.h"
#include "PERCheckTreeListHeaderCtrl.h"

#ifdef JADEFUSION
#include "EDIFileFilters.h"
#endif

class EPERCheckTop_cl_View;
class EPERCheckBottom_cl_View;

class EPERCheck_cl_Frame : public EDI_cl_BaseFrame
{
	DECLARE_DYNCREATE(EPERCheck_cl_Frame )

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	CONSTRUCT.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	EPERCheck_cl_Frame (void);
	~EPERCheck_cl_Frame (void);

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	ATTRIBUTES.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	CSplitterWnd*			mpo_Splitter;
	CSplitterWnd*			mpo_Splitter2;

	BOOL					m_bSplitterCreated;

#ifdef JADEFUSION
	EDI_cl_FileFilter		m_FileFilter;
#endif

	struct
	{
		int i_Version;
	} mst_Ini;

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	FUNCTIONS.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	
	void	OnCtrlPopup(CPoint, ePERCheckViewType eViewType=ePERCheckNone);
	void	OnAction(ULONG);

	BOOL	b_KnowsKey(USHORT);
	BOOL	b_CanActivate(void);
	BOOL	b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);

	int		i_OnMessage(ULONG, ULONG, ULONG);

	UINT	ui_OnActionState(ULONG);

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	INTERFACE.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	void	OpenProject(void);
	void	CloseProject(void);

	char	*psz_OnActionGetBase(void)	{ return EPERCheck_asz_ActionBase; };

	void	ReinitIni(void);
	void	LoadIni(void);
	void	TreatIni(void);
	void	SaveIni(void);

	void	RefreshDiff(BIG_INDEX DirIndex);
	void	ExpandDiff(BIG_INDEX DirIndex, BOOL bClear);
	
	void	RefreshNotInBF();
	void	ExpandNotInBF();
	
	void	RefreshNotInP4();
	void	ExpandNotInP4();

	void    RefreshDeleted();
	void    ExpandDeleted();

	EPERCheckTop_cl_View* GetTopLeftWindow()	{ return ((EPERCheckTop_cl_View*)mpo_Splitter->GetPane(0,0)) ; } ;
	EPERCheckTop_cl_View* GetTopRightWindow()	{ return ((EPERCheckTop_cl_View*)mpo_Splitter->GetPane(0,1)) ; } ;

	EPERCheckBottom_cl_View* GetBottomWindow() { return ((EPERCheckBottom_cl_View*)mpo_Splitter2->GetPane(1,0)) ; } ;

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	MESSAGE MAP.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnClose(void);

	afx_msg void	OnCheckFilterSame();
	afx_msg void	OnCheckFilterModified();
	afx_msg void	OnCheckFilterNew();
	afx_msg void	OnCheckFilterRenamed();
	afx_msg void	OnCheckFilterMoved();
	afx_msg void	OnCheckFilterDeleted();
	afx_msg void	OnCheckFilterNewP4();
#ifdef JADEFUSION
	afx_msg void	OnCheckFilterSettings();

	afx_msg void	OnClickFilterSettings();
#endif
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
