/*$T sonview_smd.h GC 1.138 03/18/05 15:25:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SONVIEW_SMD_H__
#define __SONVIEW_SMD_H__
#include "BASe/BAStypes.h"

#pragma once
#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class ESON_cl_ViewSmd :
	public CFormView
{
	DECLARE_DYNCREATE(ESON_cl_ViewSmd)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCTOR/DESTRUCTOR
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	ESON_cl_ViewSmd(ESON_cl_Frame *);
	ESON_cl_ViewSmd(void);
	~ESON_cl_ViewSmd(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTE
 -----------------------------------------------------------------------------------------------------------------------
 */

private:
	BOOL	mb_InterfaceIsEnabled;

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	ESON_cl_Frame	*mpo_Editor;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MEMBER
 -----------------------------------------------------------------------------------------------------------------------
 */

private:
	void								UpdateExtendedPlayer(void);
	void								DisableInterface(void);
	void								EnableInterface(void);
	void								SignalChangeToEditor(void);
	struct SND_tdst_SModifierExtPlayer_ *GetExtPlayer(void);
	void								OnEditChange(int id, float *pf_Val, float f_Min, float f_Max);
	void								OnEditChange(int id, int *pf_Val, int f_Min, int f_Max);
	void								PlayListInit(void);
	void								PlayListSwapFile(int i1, int i2);
	void								InsertListInit(void);
	void								InsertListSwapFile(int i1, int i2);
	void								InitFreqFromFile(int);
	void								InitRandomFreq(void);
	void								DefaultRandomFreq(void);
	void								InitRandomVol(void);
	void								DefaultRandomVol(void);
	void								InitVol(void);
	void								InitPan(void);
	void								InitMinPan(void);
	void								InitFadeIn(void);
	void								InitFadeOut(void);
	void								InitDoppler(void);
	void								OnFind(void);
	void								OnFind2(ULONG);
	void								OnRButtonDownInsertList(void);
	void								OnRButtonDownPlayList(void);

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	void	UpdateSmdView(void);

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
	BOOL			PreTranslateMessage(MSG *);
	afx_msg void	OnPaint(void);
	afx_msg void	OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *);
	afx_msg void	OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnRButtonDown(UINT, CPoint);
	afx_msg void	OnDynPan(void);
	afx_msg void	OnNo3D(void);

	afx_msg void	OnSpheric(void);
	afx_msg void	OnAxial(void);
	afx_msg void	OnX(void);
	afx_msg void	OnY(void);
	afx_msg void	OnZ(void);

	afx_msg void	OnDoppler(void);

	afx_msg void	OnFadeIn(void);
	afx_msg void	OnFadeInChange(void);
	afx_msg void	OnFadeOut(void);
	afx_msg void	OnFadeOutChange(void);

	afx_msg void	OnChangeXNear(void);
	afx_msg void	OnChangeXMiddle(void);
	afx_msg void	OnChangeXFar(void);
	afx_msg void	OnChangeYNear(void);
	afx_msg void	OnChangeYMiddle(void);
	afx_msg void	OnChangeYFar(void);
	afx_msg void	OnChangeZNear(void);
	afx_msg void	OnChangeZMiddle(void);
	afx_msg void	OnChangeZFar(void);
	afx_msg void	OnChangeCylinder(void);

	afx_msg void	OnChangeMiddleFactor(void);
	afx_msg void	OnChangeFarFactor(void);
	afx_msg void	OnChangeDeltaFar(void);

	afx_msg void	OnChangeVolMin(void);
	afx_msg void	OnChangeVolMax(void);

	afx_msg void	OnChangeFreq(void);
	afx_msg void	OnChangeFreqMin(void);
	afx_msg void	OnChangeFreqMax(void);
	afx_msg void	OnChangeDoppler(void);

	afx_msg void	OnPlayListAdd(void);
	afx_msg void	OnPlayListInsertHole(void);
	afx_msg void	OnPlayListDel(void);
	afx_msg void	OnPlayListMoveUp(void);
	afx_msg void	OnPlayListMoveDown(void);

	afx_msg void	OnInsertListAdd(void);
	afx_msg void	OnInsertListDel(void);
	afx_msg void	OnInsertListMoveUp(void);
	afx_msg void	OnInsertListMoveDown(void);

	afx_msg void	OnPlaySequential(void);
	afx_msg void	OnPlayRandom(void);
	afx_msg void	OnStopOnLastHit(void);

	afx_msg void	OnGrp1(void);
	afx_msg void	OnGrp2(void);
	afx_msg void	OnGrp3(void);
	afx_msg void	OnGrp4(void);
	afx_msg void	OnGrp5(void);
	afx_msg void	OnGrp6(void);
	afx_msg void	OnGrpA(void);
	afx_msg void	OnGrpB(void);

	afx_msg void	OnFxAB(void);

	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
#endif /* __SONVIEW_H */
