/*$T DIAfontdescriptor_dlg.h GC! 1.081 05/03/01 09:22:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "DIAlogs/DIAbase.h"

class			EDIA_cl_FontDescriptorDialog;

/*$4
 ***********************************************************************************************************************
    structure for data
 ***********************************************************************************************************************
 */

typedef struct	EDIA_tdst_Letter_
{
    int     u0, u1, v0, v1;
	int		x[2], y[2];
	char	c_Sel;
	char	c_Show;
	char	c_Visible;
	char	c_Valid;
	char	c_InvertX, c_InvertY, c_RotateCW, c_RotateCCW;
} EDIA_tdst_Letter;

/*$4
 ***********************************************************************************************************************
    class for subclassing listbox
 ***********************************************************************************************************************
 */

class CLetterListBox : public CListBox
{
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	class EDIA_cl_FontDescriptorDialog	*mpo_FontDlg;

	void								MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	void								DrawItem(LPDRAWITEMSTRUCT lpDIS);

	afx_msg BOOL						OnEraseBkgnd(CDC *);

	DECLARE_MESSAGE_MAP()
};

/*$4
 ***********************************************************************************************************************
    Dialog class
 ***********************************************************************************************************************
 */

class EDIA_cl_FontDescriptorDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_FontDescriptorDialog(ULONG);
	~EDIA_cl_FontDescriptorDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    DATA.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	long						mul_TextureIndex;
	struct STR_tdst_FontDesc_	*mpst_Font;

	CRect						mo_TexRect;

	HBITMAP						mh_BitmapRGB, mh_BitmapA;
	int							mi_BmpWidth, mi_BmpHeight;

    int                         mi_Zoom;
    int                         mi_X, mi_Y;

	int							mi_Capture;
	CPoint						mo_CapturePt;
	int							mi_Pickable;
	int							mi_PickType;
	CRect						mo_SelRect;
	int							mi_CurrentLetter;
    EDIA_tdst_Letter            mst_SelLetter;
    EDIA_tdst_Letter            mst_CopyLetter;
	BOOL						mb_Copy;

	int							mi_ZoomAllWithOrigin;

	int							mi_LetterNb;
	EDIA_tdst_Letter			*mpst_Letter;

	CLetterListBox				mo_LB;

	BOOL						mb_AutoRefresh;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    void    PointNormalize( EDIA_tdst_Letter * );
    void    Point2UV( int, int, int *, int *);
    void    UV2Point (int, int, int *, int *, char );

	void	UpdateUV(void);
	void	LetterSelNone(void);

	void	UpdateTexture(void);
	void	UpdateLetterList(void);
	void	UpdateCurrentLetter(int);

	int		WheelZoom(int);

	void	SelRect_Draw(void);
	void	SelRect_Sel(void);

	void	LetterDelete(void);

	void	DeleteBitmap(void);

	void    LoadLetterDesc(void);
    void    WriteLetterDesc( void );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);
    void	OnOK(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL			PreTranslateMessage(MSG *);

	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnGetMinMaxInfo(MINMAXINFO FAR *);
	afx_msg void	OnPaint(void);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonUp(UINT, CPoint);
	afx_msg void	OnLButtonDblClk(UINT, CPoint);
	afx_msg void	OnMButtonDown(UINT, CPoint);
	afx_msg void	OnMButtonUp(UINT, CPoint);
	afx_msg void	OnRButtonDown(UINT, CPoint);
	afx_msg void	OnMouseMove(UINT, CPoint);
	afx_msg void	OnRedraw(void);
	afx_msg void	OnShowAll(void);
	afx_msg void	OnZoomAll(void);
	afx_msg void	OnRefresh(void);
	afx_msg void	OnAutoRefresh(void);
	afx_msg void	UpdateShowStatus(void);
	afx_msg void	OnButton_GenerateDesc(void);
    afx_msg void    OnButton_CopyUpperToLower(void);
    afx_msg void    OnButton_CopyLowerToUpper(void);
    afx_msg void    OnButton_ApplyCharRange( void );
	afx_msg void    OnEditChange_CharRange( void );

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
