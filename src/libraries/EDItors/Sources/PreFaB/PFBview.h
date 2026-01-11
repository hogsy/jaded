/*$T SONview.h GC! 1.081 02/18/03 11:15:56 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __PFBVIEW_H__
#define __PFBVIEW_H__
#include "BASe/BAStypes.h"

#pragma once
#ifdef ACTIVE_EDITORS


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EPFB_cl_View : public CFormView
{
	DECLARE_DYNCREATE(EPFB_cl_View)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EPFB_cl_View(EPFB_cl_Frame *);
	EPFB_cl_View();
	~EPFB_cl_View();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EPFB_cl_Frame	*mpo_Editor;
	
	int				mi_DisplayComment;
	int mi_DisplayPreview{ 0 };
	
	ULONG			mul_PreviewIndex;

	int mi_Preview{ 0 };
	HBITMAP mh_PreviewBitmap{ nullptr };
	int mi_PreviewWidth{ 2 };
	int mi_PreviewHeight{ 2 };

/*$2
 -----------------------------------------------------------------------------------------------------------------------
	function
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	void Display( OBJ_tdst_Prefab *);
	void CreatePreview( OBJ_tdst_Prefab *, BOOL );
	void DisplayPreview();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
	overwritten functions
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	BOOL	Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT &, CWnd *, UINT nID);


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Message headers
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL			PreTranslateMessage(MSG *);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnPaint( void );
	afx_msg void    OnCommentChange( void );
	afx_msg void    OnButtonType( void );
	afx_msg void    OnButtonOffset( void );
	afx_msg void    OnButtonSave( void );
	afx_msg void    OnButtonComment( void );
	afx_msg void    OnButtonPreview( void );
	afx_msg void    OnRButtonDown( UINT, CPoint );
	afx_msg void    ButtonPreview( void );
	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */

#endif /* __PFBVIEW_H */
