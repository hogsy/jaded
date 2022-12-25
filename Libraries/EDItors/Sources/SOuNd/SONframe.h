/*$T SONframe.h GC 1.138 04/05/04 14:05:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "BASE/CLIbrary/CLIfile.h"
#include "BIGfiles/BIGfat.h"
#include "EDIbaseframe.h"
#include "SONframe_act.h"
#include "SouND/Sources/SNDstruct.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define ESON_Cte_AutoSave					0x00000001
#define ESON_Cte_VolumeOff					0x00000002
#define ESON_Cte_AutoPlay					0x00000010
#define ESON_Cte_AutoOpen					0x00000020
#define ESON_Cte_CloseAllWhenDestroyWorld	0x00000040
#define ESON_Cte_EnableDebugLog				0x00000200
#define ESON_Cte_EnablePCM					0x00000400
#define ESON_Cte_DispEffVol					0x00000800
#define ESON_Cte_NoSynchro					0x00001000

/*$4
 ***********************************************************************************************************************
    structures
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	ESON_tdst_Ini_
{
	char			sz_IniVersion[16];

	char			sz_SoundEditor[256];
	char			sz_TempDir[256];

	LONG			l_ListSoundColumnWidth[10];
	LONG			l_ListInstanceColumnWidth[10];
	LONG			l_ListBankColumnWidth[10];

	int				i_SplitterWidth0;
	int				i_SplitterWidth1;
	int				i_SplitterWidth2;

	int				i_Option;

	int				i_FiltersSound;
	int				i_FilterInstance;

	/* sound */
	unsigned int	ui_SndPlayerFreq;
	float			f_SndPlayerVol;
	int				i_SndPlayerPan;

	/* smodifier */
	unsigned int	ui_SmdPlayerFreq;
	float			f_SmdPlayerVol;
	int				i_SmdPlayerPan;

	/* peakmeter */
	int				i_VuFalloff;
	int				i_VuPeakTTL;

	LONG			l_ListMuteColumnWidth[10];
	int				i_LastSplitter;
	char			c_TabId;
	char			ac_Dummy[1019];
} ESON_tdst_Ini;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	ESON_tdst_SoundDesc_
{
	float						f_PlayerVol;
	int							i_PlayerPan;
	unsigned int				ui_PlayerFreq;
	float						f_PlayerStartTime;
	IDirectSoundBuffer8			*po_SoundBuffer;
	BOOL						b_PlayLoop;
	BOOL						b_Paused;

	BIG_INDEX					ul_Fat;
	BIG_KEY						ul_Key;
	char						sz_Path[BIG_C_MaxLenPath];

	struct SND_tdst_WaveDesc_	*pWave;
	ULONG						ul_NumberOfSamples;
	float						f_Time;
} ESON_tdst_SoundDesc;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	ESON_tdst_SndBankDesc_
{
	char					sz_Path[BIG_C_MaxLenPath];
	BIG_INDEX				ul_Fat;
	BIG_KEY					ul_Key;
	union SND_tdun_Main_	*pst_CurrentBank;
} ESON_tdst_SndBankDesc;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	ESON_tdst_SModifierDesc_
{
	int			i_Index;
	char		sz_Path[BIG_C_MaxLenPath];
	BIG_INDEX	ul_Fat;
	BIG_KEY		ul_Key;
	BOOL		b_AutoSave;
	int			i_Instance;
	BOOL		b_NeedSaving;
} ESON_tdst_SModifierDesc;

/*$4
 ***********************************************************************************************************************
    class
 ***********************************************************************************************************************
 */

class	ESON_cl_View;
class	ESON_cl_ViewSmd;
class	ESON_cl_ViewGroup;
class	ESON_cl_Pane;
class	ESON_cl_VUmeter;

class ESON_cl_Frame :
	public EDI_cl_BaseFrame
{
	DECLARE_DYNCREATE(ESON_cl_Frame)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	ESON_cl_Frame(void);
	~ESON_cl_Frame(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	/* OPTIONS */
	ESON_tdst_Ini									mst_Ini;

	/* PANES */
	ESON_cl_Pane									*mpo_PaneList;
	ESON_cl_Pane									*mpo_PaneSmd;
	ESON_cl_Pane									*mpo_PaneGrp;
	ESON_cl_Pane									*mpo_PaneSound;

	/* VUMETER */
	ESON_cl_VUmeter									*mpo_VUmeter;

	/* SPLITTER VIEW */
	CSplitterWnd									*mpo_Splitter;
	CSplitterWnd									*mpo_Splitter2;

	/* SOUND VIEW */
	EVAV_cl_View									*mpo_VarsView0;
	EVAV_tdst_VarsViewStruct						mst_VarsViewStruct0;
	CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>	mo_ListItems0;

	/* BANK VIEW */
	EVAV_cl_View									*mpo_VarsView1;
	EVAV_tdst_VarsViewStruct						mst_VarsViewStruct1;
	CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>	mo_ListItems1;

	/* SMODIFIER VIEWS */
	ESON_cl_ViewSmd									*mpo_ViewSmd;

	/* GROUP VIEWS */
	ESON_cl_ViewGroup								*mpo_ViewGroup;

	/* LISTS VIEW */
	ESON_cl_View									*mpo_View;

	/* data */
	ESON_tdst_SoundDesc								mst_SDesc;
	ESON_tdst_SndBankDesc							mst_SndBankDesc;
	ESON_tdst_SModifierDesc							mst_SModifierDesc;
	int												mi_ListSplitterId;
	int												mi_SModifierSplitterId;
	int												mi_BankSplitterId;
	int												mi_SoundSplitterId;
	unsigned int									mui_TimerId;
	int												mi_FiltersGroup;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	Insert_OnOpen(ULONG _ulFat = -1);
	void	Insert_OnNew(void);

	void	OnVumeter(void);
	void	OnMixer(void);

	void	Sound_Set(BIG_INDEX, BOOL _b_Update = TRUE);
	void	Sound_Stop(void);
	void	Sound_Pause(void);
	void	Sound_Play(BOOL _b_Looped = FALSE);
	void	Sound_Close(void);
	void	Sound_SetSpy(BOOL _b_UseSelected);
	void	Sound_ResetSpy(void);
	void	Sound_UserReport(void);
	void	Sound_OnFindFile(void);
	void	Sound_FindReference(ULONG ulKey = -1);

	void	SoundBank_Set(BIG_INDEX, BOOL _b_Update = TRUE);
	void	SoundBank_Close(void);
	void	SoundBank_Modify(void);
	void	SoundBank_ForceReload(void);
	void	SoundBank_UserReport(void);
	void	SoundBank_ContentsReport(void);
	void	SoundBank_OnFindFile(void);

	BOOL	b_SModifier_CanClose(void);
	void	SModifier_SaveCurrent(BOOL b_refresh = TRUE);
	void	SModifier_CloseCurrent(BOOL b_refresh = TRUE);
	void	SModifier_Open(void);
	void	SModifier_Set(BIG_INDEX, BOOL _b_Update = TRUE);
	void	SModifier_New(void);
	void	SModifier_CreateAss(BIG_INDEX);
	void	SModifier_Stop(void);
	void	SModifier_Pause(void);
	void	SModifier_Play(BOOL _b_Looped = FALSE);
	void	SModifier_OnFindFile(void);
	void	SModifier_UserReport(void);
	void	SModifier_DisplayName(void);

	void	UpdateVarView_Bank(BOOL bForce = FALSE);
	void	UpdateVarView_Sound(BOOL bForce = FALSE);

	void	Instance_Set(void);
	void	Instance_Spy(void);
	void	Instance_OnEditSound(void);
	void	Instance_OnEditSmd(void);
	void	Instance_OnFindSound(void);
	void	Instance_OnFindSmd(void);
	void	Instance_OnFindGao(void);
	void	Instance_OnSetSoloGao(void);
	void	Instance_OnSetMuteGao(void);
	void	Instance_OnSetSolo(void);
	void	Instance_OnSetMute(void);
	BOOL	Instance_b_GaoIsSolo(void);
	BOOL	Instance_b_GaoIsMute(void);
	BOOL	Instance_b_IsSolo(void);
	BOOL	Instance_b_IsMute(void);
	void	Instance_AddWatch(void);

	void	MdF_GaoHasChanged(unsigned int lgao = 0);

	void	External_SetEditor(void);
	void	External_SetTempDir(void);
	void	External_Edit(void);
	void	External_Reload(void);

	void	Display_EqualSplit(void);
	void	SizeDialogBar(void);

	void	OnWorldDestruction(void);
	void	OnResetAutoOff(void);
	void	OnReinitEngine(void);
	void	OnCreateMTX(BIG_INDEX);
	void	OnRefreshRaster(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    INTERFACE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	b_CanDuplicate(void)			{ return FALSE; };
	BOOL	b_CanBeLinked(void)				{ return TRUE; };
	BOOL	b_CanBeLinkedToEngine(void)		{ return TRUE; };
	BOOL	b_CanActivate(void);
	char	*psz_OnActionGetBase(void)	{ return ESON_asz_ActionBase; };
	void	OnAction(ULONG);
	BOOL	b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);
	UINT	ui_OnActionState(ULONG);
	void	OnActionUI(ULONG, CString &, CString &);
	void	OnToolBarCommand(UINT);
	void	OnToolBarCommandUI(UINT, CCmdUI *);
	int		i_OnMessage(ULONG, ULONG, ULONG);
	void	ReinitIni(void);
	void	LoadIni(void);
	void	TreatIni(void);
	void	SaveIni(void);
	void	OneTrameEnding(void);
	void	RefreshDialogBar(void);
	BOOL	b_KnowsKey(USHORT);
	void	OnRealIdle(void);
	void	OnVolumeOff(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnTimer(UINT);
	DECLARE_MESSAGE_MAP()
};

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */
