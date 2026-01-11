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

class EDIA_cl_Texture : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_Texture( ULONG, int );
	~EDIA_cl_Texture(void);


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    ULONG                   mul_TextureIndex;
	int						mi_Slot;
    BOOL                    mb_TexAlpha;
    BOOL                    mb_TexIndex;
    ULONG                   mul_RawIndex;
    ULONG                   mul_TgaIndex;

    CWnd                    *mpo_ColorButton[ 5 ];

    unsigned char           *mpc_Raw;
    HBITMAP					mh_BitmapRGB, mh_BitmapA;
    CDC                     *mpo_DC[2];
	int						mi_BmpWidth, mi_BmpHeight;
    int                     mi_X, mi_Y, mi_Zoom;
    int                     mi_CurX, mi_CurY, mi_CurRect;
    CRect                   mo_DrawRect[3];
    CRect                   mo_RealDrawRect[2];
    CRect                   *mpo_DrawRect[2];
    BOOL                    mb_ShowRGB, mb_ShowA;

    TEX_tdst_Palette        mst_Palette;
    HBITMAP					mh_PaletteRGB, mh_PaletteA;
    int						mi_PalWidth, mi_PalHeight;
    CRect                   mo_PalDrawRect;
    BOOL                    mb_PalShowAlpha;

    CRect                   mo_RGBARect[4];
    HBITMAP                 mh_RGBABmp[4];
    CDC                     *mpo_RGBADC[4];
    ULONG                   ul_RGBAColor;
    
    int                     mi_Capture;
    CPoint					mo_CapturePt;

    ULONG                   mul_Color[2];
    ULONG                   mul_ColorInv[2];
    int                     mi_ColorIndex[ 2 ];
    int                     mi_CurColor;
    CRect                   mo_ColorRect[ 2 ];

    int                     mi_NbColor;

    CRect                   mo_SelRect;

    int                     mi_PickedRect;
    int                     mi_Pickable, mi_Picked;
    
    CPoint                  mo_MousePos;
    CRect                   mst_RectSel;

    CPoint                  mo_PaintPreviousPos;
    int                     mi_PaintPreviousX, mi_PaintPreviousY;

    int                     mi_StartEdit, mi_ValueEdit, mi_CurValueEdit;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);
    BOOL    PreTranslateMessage( MSG *);

    void    UpdateTexture();
    void    DeleteBitmap();

    int     WheelZoom( int );

	void	OnOK(void);
	void	OnCancel(void);
    void    OnKeyDown( UINT , UINT , UINT );

    void    ComputeDrawRect( void );
    void    PickPalColor( CPoint, int );
    void    PickColor( CPoint, int );
    void    Paint( CPoint, int, int, BOOL );
    int     Point2Color( CPoint, int *, int * );
    void    Index2LineCol( int , int *, int *);
    void    UpdateRGBAValue( void );
    void    UpdateCoord( CPoint );

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
    afx_msg void    OnRButtonDown( UINT, CPoint );
    afx_msg void    OnRButtonUp( UINT, CPoint );
    afx_msg void	OnGetMinMaxInfo(MINMAXINFO FAR *);
    afx_msg void	OnSize(UINT, int, int);
    afx_msg BOOL	OnEraseBkgnd(CDC *);
    afx_msg void    OnSwapColor( void );
    afx_msg void    OnCheck_PaletteAlpha( void );
    afx_msg void    OnCheck_PaletteRGB( void );
    afx_msg void    OnCheck_A( void );
    afx_msg void    OnCheck_RGB( void );
	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
