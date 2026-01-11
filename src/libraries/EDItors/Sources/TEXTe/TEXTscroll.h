/*$T TEXTscroll.h GC 1.138 09/07/04 14:41:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "EDIbaseframe.h"
#include "Res/Res.h"

/*$4
 ***********************************************************************************************************************
    CONSTANTS
 ***********************************************************************************************************************
 */

#define ETEXT_COLOR_RankBack	0
#define ETEXT_COLOR_RankLight	1
#define ETEXT_COLOR_RankDark	2
#define ETEXT_COLOR_Col0		3
#define ETEXT_COLOR_Col1		4
#define ETEXT_COLOR_ColSort0	5
#define ETEXT_COLOR_ColSort1	6
#define ETEXT_COLOR_ColFind0	7
#define ETEXT_COLOR_ColFind1	8
#define ETEXT_COLOR_ColFindCur	9

#define ETEXT_COLOR_Nb			10

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class	ETEXT_cl_Frame;
class ETEXT_cl_Scroll :
	public CScrollView
{
	DECLARE_DYNCREATE(ETEXT_cl_Scroll)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	ETEXT_cl_Scroll(void);
	~ETEXT_cl_Scroll(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	ETEXT_cl_Frame	*mpo_Parent;
	CSize			mo_Size;
	CSize			mo_SizeFnt;
	CString			mo_Content;
	POSITION		mx_PosContent;
	BOOL			mb_InMenu;

	int				mi_NumSel;

	ULONG			mul_BackColor;
	ULONG			maul_Color[2][ETEXT_COLOR_Nb];
	CPen			mo_MoveHeaderPen;
	CPen			mo_DarkPen;
	CPen			mo_LightPen;
	CBrush			mo_SelBrush;
	CPoint			mao_Arrow[6];

	int				mi_Capture;
	int				mi_CaptureState;
	CPoint			mo_CapturePt;
	CRect			mo_CaptureRect;

	int				Column_Nb;
	int				Column[100];
	int				Column_X[101];

	int				Tirette_Line;
	int				Tirette_StartLine;
	int				Tirette_EndLine;
	int				Tirette_ColumnType;

	BOOL			mb_RankMoving;

	int				Sel_Down;
	int				Sel_DownLine;
	int				Sel_DownColumn;
	int				Sel_BlockStart;
	int				Sel_BlockEnd;

	int				mi_HitTest;
	int				mi_HitTest_Line;
	int				mi_HitTest_Column;
	int				mi_HitTest_SizeColumn;
	int				mi_HitTest_Tirette;

	CEdit			mo_Edit;
	BOOL			mb_EditContent;
	int				mi_Edit_Column;
	int				mi_Edit_Line;
	int				mi_Edit_LineNumber;
	BOOL			mi_Edit_Multiline;

	int				mi_MoveHeader_Column;
	int				mi_MoveHeader_Offset;
	int				mi_MoveHeader_Size;
	int				mi_MoveHeader_Drop;

	int				mi_OrderColumn;
	BOOL			mi_OrderLesserToGreater;

	int				mi_LastEntrySel;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	int		HitTest(CPoint);
	void	OnDraw(class CDC *);
	BOOL	OnScroll(UINT, UINT, BOOL bDoScroll = TRUE);
	BOOL	SetEditPosSize(CPoint, int = -1, BOOL = FALSE);
	void	UpdateScroll(void);
	void	DeleteCur(void);
	BOOL	PreTranslateMessage(MSG *);

	void	TextChange(void);
	void	ShowEdit(BOOL);
	void	EditNextPrevious(BOOL, BOOL);
	void	EditCel(void);
	void	MoveRank(int , int , int );

	void	ComputeOrder(void);
	void	ComputeCelRect(CRect &, int, int, BOOL);
	void	ComputeSizes(void);
	void	EnsureVisible(int, int);

	BOOL	UpdateSel(int, int, int, BOOL);
	BOOL	MigratePickRank(void);
	void	DoTirette(void);

	BOOL	b_CanCloseWithKey(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonUp(UINT, CPoint);
	afx_msg void	OnLButtonDblClk(UINT, CPoint);
	afx_msg void	OnRButtonDown(UINT, CPoint);
	afx_msg void	OnRButtonUp(UINT, CPoint);
	afx_msg void	OnMouseMove(UINT, CPoint);
	afx_msg void	OnEditChange(void);
	afx_msg void	OnLooseFocus(void);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg BOOL	OnEraseBkgnd(CDC *);
	afx_msg BOOL	OnMouseWheel(UINT, SHORT, CPoint);
	afx_msg void	OnKillFocus(CWnd *);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
