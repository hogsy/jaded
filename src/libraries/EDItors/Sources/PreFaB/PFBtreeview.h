/*$T PFBtreeview.h GC! 1.081 01/30/04 10:12:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __PFBTREEVIEW_H__
#define __PFBTREEVIEW_H__
#include "BASe/BAStypes.h"

#pragma once
#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EPFB_cl_TreeView : public CFormView
{
	DECLARE_DYNCREATE(EPFB_cl_TreeView)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EPFB_cl_TreeView(EPFB_cl_Frame *);
	EPFB_cl_TreeView(void);
	~EPFB_cl_TreeView();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EPFB_cl_Frame	*mpo_Editor;
	
	HTREEITEM		mh_DDItem;
	CPoint			mo_DDPt;
	
	ULONG			mul_RootDir;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void		Fill(ULONG, HTREEITEM _h_Parent = TVI_ROOT);
	void		Refresh( void );
	
	void		Rename( HTREEITEM );
	void		Remove( HTREEITEM );
	void		CreateDir( HTREEITEM );
	void		CreateEmptyPrefab( HTREEITEM );
	
	BOOL		AddPrefab(ULONG, ULONG, BOOL);
	BOOL		SelDir(ULONG _ul_Dir);
	BOOL		SelFile( ULONG _ul_File );
	
	HTREEITEM	FindDir(ULONG, HTREEITEM _h_Parent = TVI_ROOT);
	HTREEITEM	FindFile(ULONG, ULONG *, HTREEITEM _h_Parent = TVI_ROOT);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Message headers
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL			Create
					(
						LPCTSTR lpszClassName,
						LPCTSTR lpszWindowName,
						DWORD	dwStyle,
						const RECT &,
						CWnd *,
						UINT nID
					);
	BOOL			PreTranslateMessage(MSG *);

	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnPrefabTreeSelChanged(NMHDR *, LRESULT *);

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */

#endif /* __PFBTREEVIEW_H */
