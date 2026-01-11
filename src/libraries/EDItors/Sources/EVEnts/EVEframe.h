/*$T EVEframe.h GC! 1.086 07/07/00 14:56:35 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "EDIbaseframe.h"
#include "EVEtrack.h"
#include "EVEframe_act.h"
#include "AIinterp/Sources/EVents/EVEstruct.h"
#include "DIAlogs/DIAmorphedit_dlg.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
#define EEVE_MaxMark	500
#define EEVE_SizeMark	64
typedef struct EEVE_tt_TRE_
{
	UINT	ui_NumMark;
	char	az_Marks[EEVE_MaxMark][EEVE_SizeMark];
	UINT	ui_FrameMark[EEVE_MaxMark];
} EEVE_tt_Tre;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define EEVE_C_OptionTimeSec	1
#define EEVE_C_OptionTimeFrame	2

#define EEVE_C_UnitMode160		1
#define EEVE_C_UnitModeAuto		2
#define EEVE_C_UnitModeUser		4

#define EEVE_C_SnapMode160		1
#define EEVE_C_SnapModeAuto		2
#define EEVE_C_SnapModeUser		4
#define EEVE_C_SnapModeTracks	8

#define EEVE_C_CopyAll				0
#define EEVE_C_CopyOnlyRotation		1
#define EEVE_C_CopyOnlyTranslation	2

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

typedef struct	EEVE_tdst_CopyEvent_
{
	EVE_tdst_Event	*pst_Event; /* Copy of event */
	int				i_NumTrack; /* Relative pos of track */
	int				i_NumEvent; /* Number of Event in the Track*/
} EEVE_tdst_CopyEvent;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class			EEVE_cl_Scroll;
class			EEVE_cl_Inside;
class			EOUT_cl_Frame;
class			EDIA_cl_MorphEditDialog;

class EEVE_cl_Frame : public EDI_cl_BaseFrame
{
	DECLARE_DYNCREATE(EEVE_cl_Frame)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EEVE_cl_Frame(void);
	~EEVE_cl_Frame(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MEMBERS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	struct
	{
		int		i_SplitterWidth;
		float	f_Factor;
		float	f_DivFactor;
		float	f_ValFactor;
		int		i_Inter;
		int		i_YBar;
		int		i_TimeDisplay;
		int		i_ForceSnap;
		int		i_DisplayHidden;
		int		i_XLeft;
		float	f_RealUnit;
		int		i_UnitMode;
		int		i_AllEventsVis;
		int		i_AutoAlign;
		int		i_SnapMode;
		float	f_SnapUnit;
		int		i_DisplayTrackName;
		int		i_PlayHz;
		int		i_LinkAction;
		BOOL	b_AnimMode;
		BOOL	b_Lock;
		BOOL	b_RunSel;
		int		a[5];
		float	f_WidthFixed;
		COLORREF	ac_ColIA[2000];
	} mst_Ini;

	/**/
	CFont											mo_Fnt;
	CSplitterWnd									*mpo_MainSplitter;
	CList<EEVE_cl_Track *, EEVE_cl_Track *>			mo_ListTracks;
	struct EVE_tdst_ListTracks_						*mpst_ListTracks;
	BIG_INDEX										mul_ListTracks;
	struct OBJ_tdst_GameObject_						*mpst_GAO;
	BIG_INDEX										mul_GAO;
	struct EVE_tdst_Data_							*mpst_Data;
	EVAV_cl_View									*mpo_VarsView;
	EVAV_tdst_VarsViewStruct						mst_VarsViewStruct;
	CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>	mo_ListItems;
	EEVE_cl_Event									*mpo_LastSelected;
	BOOL											mb_Play;
	int												mi_NumCycles;
	int												mi_PlayContract;
	BOOL											mb_LockUpdate;

	int												mi_NumberOfLinkedOutputEditors;
	EOUT_cl_Frame									*mapo_LinkedOutputEditor[EDI_C_MaxDuplicate];
	CList<EVE_tdst_Data *, EVE_tdst_Data *>			mo_ListUndo;
	int												mi_CurrentUndo;
	BOOL											mb_AnimMode;

	BIG_INDEX										mul_Tre;
	EEVE_tt_Tre										mt_Tre;
	
	EDIA_cl_MorphEditDialog							*mo_MorphEditor;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void			SetLinkedOutputEditors(void);

	void			WhenChanged(void);
	void			CloseAll(BOOL _b_Refresh = TRUE);
	void			SetListTracks(struct EVE_tdst_ListTracks_ *);
	void			SetGAO(OBJ_tdst_GameObject *, BOOL = TRUE, BOOL = FALSE);
	EEVE_cl_Track	*AddTrack(struct EVE_tdst_Track_ *);

	int				EventToIndex(EEVE_cl_Track *, EEVE_cl_Event *);
	void			OnNewTrack(void);
	void			OnRunAll(BOOL, BOOL = FALSE, BOOL = FALSE);
	void			PopupEvent(EEVE_cl_Track *, EEVE_cl_Event *);
	void			SetFactor(BOOL = TRUE, BOOL = TRUE);
	void			SetInter(int);
	void			OnDeleteTrack(EEVE_cl_Track *);
	void			InvertTracks(int, int);
	void			MoveTrack(BOOL);

	void			CloseCopy(void);
	void			CloseCopyTracks(void);
	void			CopyOneEvent(EVE_tdst_Track *, int, EVE_tdst_Event *, BOOL = FALSE, BOOL = FALSE, BOOL = FALSE, EEVE_tdst_CopyEvent * = NULL);
	void			Copy(void);
	void			CopySelectedTracks(void);

	void			Paste(EEVE_cl_Track *, EEVE_cl_Event *, BOOL, BOOL = FALSE, BOOL = FALSE, BOOL = FALSE);
	void			PasteFirstSel(BOOL, BOOL = FALSE);
	void			PasteFirstSelAbsolute(void);
	void			PasteCopiedTracks(int);

	void			ForceRefresh(void);
	void			ForceRefreshReal(void);
	void			UnselectAll(BOOL = TRUE);
	void			SelectCurrentEvents(BOOL = FALSE);
	void			SelectInRect(CRect *, BOOL = FALSE, BOOL = FALSE);
	void			MoveReal(int);
	void			MoveRunningBox(EEVE_cl_Track *, float, BOOL = FALSE, BOOL = TRUE);
	void			Align(int);
	void			DeleteOneEvent(EVE_tdst_Track *, int, BOOL = TRUE);
	void			DeleteSelected(BOOL = TRUE);
	void			SplitSelected(void);
	void			SetDelaySelected(float);
	void			SplitOneEvent(EVE_tdst_Track *, EVE_tdst_Params *, int, float, float);
	void			SplitComputeWidth
					(
						EVE_tdst_Track *,
						EVE_tdst_Params *,
						int,
						CPoint,
						EEVE_cl_Event *,
						float *,
						float *,
						BOOL
					);
	void			SplitKey(CPoint);
	void			RecomputeWidthFromCur(void);

	void			WhenScroll(void);

	void			OptimizeAnim(void);
	void			RoundTo(void);
	void			Force0(BOOL);
	void			Force1(void);
	void			GotoOrigin(void);
	void			GotoEnd(void);
	void			Play(void);
	void			Expand(int);
	void			SaveForUndo(void);
	void			Undo(void);
	void			Redo(void);
	void			CloseUndo();
	void			SwapAllVis(void);

	void			CenterToRealTime(void);
	int				CenterSel(void);

	EDIA_cl_MorphEditDialog		*OpenMorphEditor( struct MDF_tdst_Modifier_ *, OBJ_tdst_GameObject * );
	void						CloseMorphEditor();

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	void			DisplayHideCurveForGAO(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *, BOOL);
	void			SplitEventForGAO(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *, BOOL, BOOL = TRUE);
	void			DelEventForGAO(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *, BOOL);
	int				AddFrameAll(EVE_tdst_Track * = NULL, int = 0, BOOL = FALSE);
	void			DuplicateFrameAll(EVE_tdst_Track * = NULL);
	void			DelKeyFrame(BOOL = TRUE);
	void			AddKey(void);
	void			DelKey(BOOL = FALSE);
	void			ShowAllCurves(BOOL = TRUE);
	void			DelAllCurves(void);
	void			MoveKeyLeftForGAO(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *, BOOL);
	void			MoveKeyRightForGAO(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *, BOOL);
	void			Recompute(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *, BOOL);
	void			RecomputeTrack(OBJ_tdst_GameObject *, EVE_tdst_Track *);
	void			SelectAllBones(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *);
	void			AddRemoveIK(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *);
	void			SwitchTrans(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *);
	void			AddTimeAll(BOOL);
	void			ComputeZoom(int, BOOL = TRUE);
	void			AlignEventLeft(void);
	void			LoadTre(void);
	void			SaveTre(BOOL = FALSE);

private:
	void			ChooseDialogForSoundPlayer(EEVE_cl_Track *_po_Track, EEVE_cl_Event *, EVE_tdst_Event *);
	void			SetSizeOfSoundEvent(EEVE_cl_Track *_po_Track, EEVE_cl_Event *, int);
	void			ResetSizeOfSoundPlayer(EEVE_cl_Track *_po_Track, EEVE_cl_Event *);
	void			ResetSizeOfRequestPlayDialog(EEVE_cl_Track *_po_Track, EEVE_cl_Event *);
	void			ChooseDialogForEvent(EEVE_cl_Track *, EEVE_cl_Event *, EVE_tdst_Event *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    INTERFACE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	char	*psz_OnActionGetBase(void)	{ return EEVE_asz_ActionBase; };
	void	OnAction(ULONG);
	BOOL	b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);
	UINT	ui_OnActionState(ULONG);
	void	ReinitIni(void);
	void	LoadIni(void);
	void	TreatIni(void);
	void	SaveIni(void);
	int		i_OnMessage(ULONG, ULONG, ULONG);
	BOOL	b_KnowsKey(USHORT);
	void	OneTrameEnding(void);
	void	OnRealIdle(void);
	void	AfterEngine(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg void	OnSize(UINT, int, int);
	DECLARE_MESSAGE_MAP()
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void SpeedSolidRect(CDC *pdc, CRect *_po_Rect, COLORREF col)
{
	/*~~~~~~~~~~~*/
	HBRUSH	hbr, h;
	/*~~~~~~~~~~~*/

	hbr = CreateSolidBrush(col);
	h = (HBRUSH) pdc->SelectObject(hbr);
	pdc->PatBlt
		(
			_po_Rect->left,
			_po_Rect->top,
			_po_Rect->right - _po_Rect->left + 1,
			_po_Rect->bottom - _po_Rect->top + 1,
			PATCOPY
		);
	DeleteObject(hbr);
	pdc->SelectObject(h);
}

#endif /* ACTIVE_EDITORS */
