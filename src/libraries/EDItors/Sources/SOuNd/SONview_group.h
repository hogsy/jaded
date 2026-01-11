/*$T SONview_group.h GC 1.138 03/04/04 15:00:03 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SONVIEW_GROUP_H__
#define __SONVIEW_GROUP_H__
#include "BASe/BAStypes.h"

#pragma once
#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class ESON_cl_ViewGroup :
	public CFormView
{
	DECLARE_DYNCREATE(ESON_cl_ViewGroup)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCTOR/DESTRUCTOR
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	ESON_cl_ViewGroup(ESON_cl_Frame *);
	ESON_cl_ViewGroup(void);
	~ESON_cl_ViewGroup(void);

/*
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTE
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	ESON_cl_Frame	*mpo_Editor;
    HICON           m_hIconSolo;
    HICON           m_hIconMute;
    HICON           m_hIconBypass;
    HICON           m_hIconSoloG;
    HICON           m_hIconMuteG;
    HICON           m_hIconBypassG;

/*
 -----------------------------------------------------------------------------------------------------------------------
    MEMBERS
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	void	UpdateControls(void);

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	void	OnBypassGrp(int);
	void	OnSoloGrp(int);
	void	OnMuteGrp(int);
	void	OnBypassUsrGrp(int);
	void	OnSoloUsrGrp(int);
	void	OnMuteUsrGrp(int);
	void	UpdateAllInstances(void);

/*
 -----------------------------------------------------------------------------------------------------------------------
    OVERLOAD
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
	afx_msg void	OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *);
	afx_msg void	OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *);
	afx_msg void	OnPaint(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnMasterBypass(void);
	afx_msg void	OnMasterSolo(void);
	afx_msg void	OnMasterMute(void);

	afx_msg void	OnBypassGrp1(void);
	afx_msg void	OnBypassGrp2(void);
	afx_msg void	OnBypassGrp3(void);
	afx_msg void	OnBypassGrp4(void);
	afx_msg void	OnBypassGrp5(void);
	afx_msg void	OnBypassGrp6(void);
	afx_msg void	OnBypassGrp7(void);
	afx_msg void	OnBypassGrp8(void);
	afx_msg void	OnBypassUsrMaster(void);
	afx_msg void	OnBypassGrp11(void);
	afx_msg void	OnBypassGrp12(void);
	afx_msg void	OnBypassGrp13(void);

	afx_msg void	OnMuteGrp1(void);
	afx_msg void	OnMuteGrp2(void);
	afx_msg void	OnMuteGrp3(void);
	afx_msg void	OnMuteGrp4(void);
	afx_msg void	OnMuteGrp5(void);
	afx_msg void	OnMuteGrp6(void);
	afx_msg void	OnMuteGrp7(void);
	afx_msg void	OnMuteGrp8(void);
	afx_msg void	OnMuteUsrMaster(void);
	afx_msg void	OnMuteGrp11(void);
	afx_msg void	OnMuteGrp12(void);
	afx_msg void	OnMuteGrp13(void);

	afx_msg void	OnSoloGrp1(void);
	afx_msg void	OnSoloGrp2(void);
	afx_msg void	OnSoloGrp3(void);
	afx_msg void	OnSoloGrp4(void);
	afx_msg void	OnSoloGrp5(void);
	afx_msg void	OnSoloGrp6(void);
	afx_msg void	OnSoloGrp7(void);
	afx_msg void	OnSoloGrp8(void);
	afx_msg void	OnSoloUsrMaster(void);
	afx_msg void	OnSoloGrp11(void);
	afx_msg void	OnSoloGrp12(void);
	afx_msg void	OnSoloGrp13(void);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
#endif
