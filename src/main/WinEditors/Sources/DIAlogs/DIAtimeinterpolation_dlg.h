/*$T DIAtimeinterpolation_dlg.h GC! 1.081 09/11/00 09:40:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "BASe/BAStypes.h"

class	F3D_cl_View;

typedef struct EDIA_tdst_EditKey_
{
    float   tx, ty;
    int     x, y;
    char    sel;
} EDIA_tdst_EditKey;

typedef struct EDIA_tdst_HistoryList_
{
    long                l_NbKey;
    EDIA_tdst_EditKey   *pst_Key;
    int                 i_SelRect;
    CRect               o_SelRect;

    struct EDIA_tdst_HistoryList_ *pst_Prev;
    struct EDIA_tdst_HistoryList_ *pst_Next;
} EDIA_tdst_HistoryList;

#define C_EditedCurve   0
#define C_LoadedCurve   1
#define C_Copy          2


class EDIA_cl_TimeInter : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_TimeInter( struct EVE_tdst_Event_ *);
	~EDIA_cl_TimeInter(void);

    long                ml_NbKey[3];
    EDIA_tdst_EditKey   *mpst_Curve[3];

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    int                     mi_WindowHeight;
    int                     mi_WindowWidthBig, mi_WindowWidthSmall;

	CRect	                mo_DrawRect[2];
    CRect                   mo_CurveRect[2];
    CRect                   mo_XRect[2], mo_YRect[2];
	int		                mi_CurveWidth[2];
    int		                mi_CurveHeight[2];
    float                   mf_Time2Width[2];
    float                   mf_Time2Height[2];
    CRect                   mo_SelRect[2];
    int                     mi_SelRect[2];

    int                     mi_PickedRect;
    int                     mi_TraceRect;


    struct EVE_tdst_Event_ *mpst_Evt;

    int                     mi_Pickable, mi_Picked, mi_PickedCoord;
    CPoint                  o_InsertPoint;
    CPoint                  mo_MousePos;

    RECT                    mst_OldClipCursor;

    EDIA_tdst_HistoryList   *mpst_History;
    EDIA_tdst_HistoryList   *mpst_HistoryRoot;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);
    BOOL    PreTranslateMessage( MSG *);
	void	OnOK(void);
	void	OnCancel(void);
    void    OnKeyDown( UINT nChar );

    void    AllocData( int, int );
    void    GetEventData( void );
    void    SetEventData( void );
    void    GetPickable( CPoint );
    void    AdjustKey( int );
    void    SortKey(int);
    void    ScaleKey( CSize &, int, int );
    void    SelectPointInRect( int );    
    void    ListRefresh( void );
    BOOL    SomethingSel( EDIA_tdst_EditKey *, int );
    int     SelCount( EDIA_tdst_EditKey *, int );
    void    SelAll( EDIA_tdst_EditKey *, int );
    void    SelNone( EDIA_tdst_EditKey *, int );
    void    SelDelete();
    void    Copy();
    void    OnCopy( int );
    void    OnHFlip();
    void    OnVFlip();

    void    HistPush();
    void    HistPop();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnPaint(void);
    afx_msg void	OnMouseMove(UINT, CPoint);
    afx_msg void    OnLButtonDown( UINT, CPoint );
    afx_msg void    OnLButtonUp( UINT, CPoint );
    afx_msg void    OnLButtonDblClk( UINT, CPoint );
    afx_msg void    OnRButtonDown( UINT, CPoint );
    
    afx_msg void    OnButtonSave( void );
    afx_msg void    OnButtonLoad( void );
    afx_msg void    OnButtonLoadClose( void );
    afx_msg void    OnListFileChange( void );

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
