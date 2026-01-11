/*$T DIAsndinsert_dlg.h GC 1.138 11/28/03 11:50:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "BASe/BAStypes.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define M_CurrentCurv	0
#define M_ClipboardCurv 1
#define M_NbCurv		2

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

typedef enum	EDIA_en_EdSndInsertSelMode
{
	en_SelNone				= 0,
	en_SelXaxis				= 1,
	en_SelYaxis				= 2,
	en_SelCurvePoints		= 3,
	en_SelTopLeftCorner		= 4,
	en_SelTopEdge			= 8,
	en_SelTopRightCorner	= 12,
	en_SelRightEdge			= 16,
	en_SelBottomRightCorner = 20,
	en_SelBottomEdge		= 24,
	en_SelBottomLeftCorner	= 28,
	en_SelLeftEdge			= 32
} EDIA_tden_EdSndInsertSelMode;

typedef struct	EDIA_tdst_EdSndInsertKey_
{
	float	tx, ty;
	int		x, y;
	char	sel;
} EDIA_tdst_EdSndInsertKey;

typedef struct	EDIA_tdst_HistoryInsertList_
{
	long								l_NbKey;
	EDIA_tdst_EdSndInsertKey			*pst_Key;
	BOOL								b_SelRect;
	CRect								o_SelRect;

	struct EDIA_tdst_HistoryInsertList_ *pst_Prev;
	struct EDIA_tdst_HistoryInsertList_ *pst_Next;
} EDIA_tdst_HistoryInsertList;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EDIA_cl_SndInsert :
	public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_SndInsert(struct SND_tdst_Insert_ *pIns = NULL);
	~EDIA_cl_SndInsert(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
    BOOL                            mb_ForceExtentionFade;

public:
	long							ml_NbKey[M_NbCurv];
	EDIA_tdst_EdSndInsertKey		*mpst_Curve[M_NbCurv];
	CRect							mo_DrawRect;
	CRect							mo_CurveRect;
	CRect							mo_XRect, mo_YRect;
	float							mf_Time2Width;
	float							mf_Time2Height;
	CRect							mo_SelRect;
	BOOL							mb_SelRect;
	BOOL							mb_PickedRect;
	BOOL							mb_TraceRect;
	int								mi_Pickable, mi_Picked;
	EDIA_tden_EdSndInsertSelMode	me_PickedCoord;
	CPoint							o_InsertPoint;
	CPoint							mo_MousePos;
	RECT							mst_OldClipCursor;
	EDIA_tdst_HistoryInsertList		*mpst_History;
	EDIA_tdst_HistoryInsertList		*mpst_HistoryRoot;

	int								mi_TypeX;
	int								mi_TypeY;
	ULONG							mul_GaoKey;
	float							mf_MinX;
	float							mf_MaxX;
	float							mf_MinY;
	float							mf_MaxY;
    BOOL                            mb_Cycling;
    BOOL                            mb_ActiveOnPlay;

	struct SND_tdst_Insert_			*mp_Insert;
	BOOL							mb_IsModified;
	BOOL							mb_IsLoadedByMyself;
	BOOL							mb_WndInitDone;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE .
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);
	BOOL	PreTranslateMessage(MSG *);
	void	DoDataExchange(CDataExchange *);
	void	OnKeyDown(UINT nChar);	

/*
 -----------------------------------------------------------------------------------------------------------------------
    PRIVATE MEMBERS
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	void	SetIsModified(BOOL b_Modified = TRUE);
	void	Paste(void);
	void	Copy(void);

	void	AllocData(int, int);
	void	LoadData(void);
	void	SaveData(void);

	void	GetPickable(CPoint);
	void	AdjustKey(int);
	void	SortKey(int);
	void	ScaleKey(CSize &, int, int);
	void	SelectPointInRect(void);
	BOOL	SomethingSel(EDIA_tdst_EdSndInsertKey *, int);
	int		SelCount(EDIA_tdst_EdSndInsertKey *, int);
	void	SelAll(EDIA_tdst_EdSndInsertKey *, int);
	void	SelNone(EDIA_tdst_EdSndInsertKey *, int);
	void	SelDelete(void);
	void	SelDeleteSpecial(void);
	void	HistPush(void);
	void	HistPop(void);
	void	EnableXaxisAndGao(BOOL, BOOL);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnPaint(void);
	afx_msg void	OnMouseMove(UINT, CPoint);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonUp(UINT, CPoint);
	afx_msg void	OnRButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonDblClk(UINT, CPoint);
	afx_msg void	OnButtonSave(void);
	afx_msg void	OnButtonSaveAs(void);
	afx_msg void	OnButtonOpen(void);
	afx_msg void	OnButtonClose(void);
	afx_msg void	OnButtonNew(void);
	afx_msg void	OnChangeTypeX(void);
	afx_msg void	OnChangeTypeY(void);
	afx_msg void	OnButtonGao(void);
	afx_msg void	OnSetModif(void);
	afx_msg void	OnChangeMinMax(void);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
