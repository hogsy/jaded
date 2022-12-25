/*$T DIAtoolbox_goview.h GC! 1.081 04/21/00 14:49:30 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __DIATOOLBOX_CURVEVIEW_H__
#define __DIATOOLBOX_CURVEVIEW_H__
#include "BASe/BAStypes.h"

#pragma once
#ifdef ACTIVE_EDITORS
class	EDIA_cl_ToolBoxDialog;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EDIA_cl_ToolBox_CurveView : public CFormView
{
	DECLARE_DYNCREATE(EDIA_cl_ToolBox_CurveView)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_ToolBox_CurveView(EDIA_cl_ToolBoxDialog *);
	EDIA_cl_ToolBox_CurveView(void);
	~EDIA_cl_ToolBox_CurveView();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_ToolBoxDialog	*mpo_ToolBox;
    int                     mi_Capture;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	Create(LPCTSTR, LPCTSTR, DWORD, const RECT &, CWnd *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Message headers
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    afx_msg void    OnPaint(void);
    afx_msg void    OnLButtonDown(UINT, CPoint);
    afx_msg void    OnLButtonUp(UINT, CPoint);
    afx_msg void    OnMouseMove(UINT, CPoint);
    afx_msg void    OnDGCheckClick(UINT);
    afx_msg void    OnUseGFCheckClicked(void);
    afx_msg void    OnButtonNbFrameCenterClicked( void );

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */

#endif /* __DIATOOLBOX_CURVEVIEW_H__ */
