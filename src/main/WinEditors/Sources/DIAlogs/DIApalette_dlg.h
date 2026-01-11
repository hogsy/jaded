/*$T DIApalette_dlg.h GC! 1.081 09/11/00 09:40:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "BASe/BAStypes.h"
#include "TEXture/TEXstruct.h"

class	F3D_cl_View;

#define C_EditedCurve   0
#define C_LoadedCurve   1
#define C_Copy          2

#define C_DisplayR      1
#define C_DisplayG      2
#define C_DisplayB      4
#define C_DisplayA      8
#define C_DisplayColor  0x10
#define C_DisplayAll    0x1F

class EDIA_cl_Palette : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_Palette( ULONG );
	~EDIA_cl_Palette(void);


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    ULONG                   mul_Index;
    TEX_tdst_Palette        mst_Palette;
    ULONG                   mul_Color[ 256 ];
    char                    mc_Sel[ 256 ];
    int                     mi_NbColor;
    BOOL                    mb_Alpha;

    int                     mi_DisplayComponent;

    CRect                   mo_DrawRect;
    CRect                   mo_CaseRect;
    CSize                   mo_Case;
    int                     mi_NbPerLine;
	
    CRect                   mo_SelRect;

    int                     mi_PickedRect;
    int                     mi_Pickable, mi_Picked;
    int                     mi_Picked4Gradient;
    int                     mi_Picked4Invert;
    int                     mi_Picked4DeductAlpha;
    
    CPoint                  mo_MousePos;

    ULONG                   mul_Mask;
    ULONG                   mul_InvMask;

    ULONG                   mul_SaveColor;
    
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

    void    ComputeDrawRect( void );
    int     Point2Color( CPoint, int *, int * );
    void    Index2LineCol( int , int *, int *);
    void    SetPickedColor(int, int);
    void    UpdateRGBAValue( void );
    void    UpdateControl( void );

    void    GetMask();
    void    ComputeGradient( int, int );
    void    ComputeInvert( int, int );
    void    ComputeDeductAlpha( int, int );
    
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
    afx_msg void	OnGetMinMaxInfo(MINMAXINFO FAR *);
    afx_msg void	OnSize(UINT, int, int);
    afx_msg BOOL	OnEraseBkgnd(CDC *);
    afx_msg void    OnChangeDisplayComponent( void );
    afx_msg void    OnSwapAlphaChannel( void );
    afx_msg void    OnSwapColorNumber( void );
    
	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
