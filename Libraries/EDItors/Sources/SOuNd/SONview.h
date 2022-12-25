/*$T SONview.h GC! 1.081 02/18/03 11:15:56 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SONVIEW_H__
#define __SONVIEW_H__
#include "BASe/BAStypes.h"

#pragma once
#ifdef ACTIVE_EDITORS
#define ESON_C_InstDisplaySound				0x00000001
#define ESON_C_InstDisplayState				0x00000002
#define ESON_C_InstDisplayVol				0x00000004
#define ESON_C_InstDisplayFlags				0x00000008
#define ESON_C_InstDisplayExtFlags			0x00000010
#define ESON_C_InstDisplayTrack				0x00000020
#define ESON_C_InstDisplayChannel			0x00000040
#define ESON_C_InstDisplayFrequency			0x00000080
#define ESON_C_InstDisplayFaderGroup		0x00000100

#define ESON_C_InstDisplayOnlyMdF			0x00000200
#define ESON_C_InstDisplayOnlyNonMdF		0x00000400
#define ESON_C_InstDisplayOnlyPlaying		0x00000800
#define ESON_C_InstDisplayOnlyStream		0x00001000
#define ESON_C_InstDisplayOnlyNonStream		0x00002000
#define ESON_C_InstDisplayOnlyReset			0x00004000
#define ESON_C_InstDisplayOnlyOnTrack		0x00008000
#define ESON_C_InstDisplayOnlyNonOnTrack	0x00010000
#define ESON_C_InstDisplayOnlyPlayingLoop	0x00020000
#define ESON_C_InstDisplayPrefetch			0x00040000
#define ESON_C_InstDisplayReverseFilter		0x80000000

class			EGRO_cl_Frame;
class			EOUT_cl_Frame;
class			F3D_cl_View;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

typedef struct	ESON_tdst_ViewInstanceList_
{
	int i_MaxNumber;
	int i_MaxSNumber;
	int i_MaxPlayingNumber;
	int i_MaxPlayingSNumber;
} ESON_tdst_ViewInstanceList;

typedef struct	ESON_tdst_ViewInstanceDisplay_
{
	int i_Size;
	struct SND_tdst_SoundInstance_ *ax_Data;
} ESON_tdst_ViewInstanceDisplay;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class ESON_cl_View : public CFormView
{
	DECLARE_DYNCREATE(ESON_cl_View)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	ESON_cl_View(ESON_cl_Frame *);
	ESON_cl_View(void);
	~ESON_cl_View();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	ESON_cl_Frame					*mpo_Editor;
	int								mi_Pane;
	int								mi_FirstUpdate;
	ESON_tdst_ViewInstanceList		mst_InstanceStat;

	CImageList						mo_ImageList;
	
	ESON_tdst_ViewInstanceDisplay	mst_CurrInstanceDisplay;
	ESON_tdst_ViewInstanceDisplay	mst_PrevInstanceDisplay;
	BOOL							mb_RefreshInstanceDisplay;
	BOOL							mb_ForceRefreshInstanceDisplay;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT &, CWnd *, UINT nID);

	void	FirstUpdateControls(void);
	void	UpdateControls(void);
	void	FillListSound(void);
	void	FillListInstance(void);
	void	FillListBank(void);
	void	FillListInsert(void);
	void	FillListGroup(void);

	void	UpdateInstanceDisplay(void);
	BOOL	b_InstanceIsFiltered(struct SND_tdst_SoundInstance_ *, int * pi_playingmode = NULL);

	int		i_GetCurSel(UINT);
	void	SetCurSel(UINT, int, BOOL _b_Data = FALSE);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Message headers
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL			PreTranslateMessage(MSG *);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnChangeView(NMHDR *, LRESULT *);
	
	afx_msg void	OnSelSound(NMHDR *, LRESULT *);
	afx_msg void	OnSelBank(NMHDR *, LRESULT *);
	afx_msg void	OnSelInstance(NMHDR *, LRESULT *);

	
	afx_msg void	OnColumnClickSound(NMHDR *, LRESULT *);
	afx_msg void	OnColumnClickBank(NMHDR *, LRESULT *);
	afx_msg void	OnColumnClickInstance(NMHDR *, LRESULT *);
	
	afx_msg void	OnRightClickBank(NMHDR *, LRESULT *);
	afx_msg void	OnRightClickSound(NMHDR *, LRESULT *);
	afx_msg void	OnRightClickInstance(NMHDR *, LRESULT *);
	afx_msg void	OnRightClickInsert(NMHDR *, LRESULT *);
	afx_msg void	OnRightClickGroup(NMHDR *, LRESULT *);
	
	afx_msg void	OnParentNotify(UINT message, LONG lParam);
    
	afx_msg void    OnSoloOffButton(void);
    afx_msg void    OnMuteOffButton(void);

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */

#endif /* __SONVIEW_H */
