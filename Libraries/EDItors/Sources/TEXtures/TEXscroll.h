/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#ifdef ACTIVE_EDITORS

#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "BIGfiles/BIGdefs.h"
#include "EDIbaseframe.h"
#include "Res/Res.h"
#include "TEXture/TEXfile.h"
#include "TEXutil.h"



/*
 ---------------------------------------------------------------------------------------------------
    Description of a bitmap
 ---------------------------------------------------------------------------------------------------
 */
/*
#define MAIEDITEX_C_INVALID	0
#define MAIEDITEX_C_BMP		1
#define MAIEDITEX_C_TGA		2
#define MAIEDITEX_C_JPG		3
#define MAIEDITEX_C_PALETTE	4
#define MAIEDITEX_C_RAW		5
#define MAIEDITEX_C_TEX		6

typedef struct  MAIEDITEX_tdst_Bitmap_
{
    HBITMAP             h_Bitmap;
    HBITMAP             h_BitmapAlpha;
    BITMAPINFOHEADER    st_Header;
	BOOL				b_Compress;
} MAIEDITEX_tdst_Bitmap;

typedef struct MAIEDITEX_tdst_BitmapDes_
{
    int                     i_Type;
    BIG_INDEX               ul_FatFile;
    CRect                   o_Rect;
    MAIEDITEX_tdst_Bitmap   st_Bmp;
	TEX_tdst_File_Tex		st_Tex;
	ULONG					ul_PaletteFlags;
} MAIEDITEX_tdst_BitmapDes;

typedef struct MAIEDITEX_tdst_SelBitmapDes_
{
    int                     i_Type;
    BIG_INDEX               ul_FatFile;
    CRect                   o_Rect;
    MAIEDITEX_tdst_Bitmap   st_Bmp;
	TEX_tdst_File_Tex		st_Tex;
	ULONG					ul_PaletteFlags;

	char				c_Quality;
	short				uw_Flags;
	ULONG				ul_Color;
	ULONG				ul_FontDescKey;
} MAIEDITEX_tdst_SelBitmapDes;
*/
/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

#define OFFSET  4
class   ETEX_cl_Frame;
class ETEX_cl_InsideScroll : public CScrollView
{
    DECLARE_DYNCREATE(ETEX_cl_InsideScroll)

/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    ETEX_cl_InsideScroll(void);
    ~ETEX_cl_InsideScroll();

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    CList<MAIEDITEX_tdst_BitmapDes *, MAIEDITEX_tdst_BitmapDes *>   mo_BmpList;
    CSize                                                           mo_Size;
    BOOL                                                            mb_AlphaShow;
    BOOL                                                            mb_ForceRes;
    CDialogBar                                                      *mpo_DialogBar;
    ETEX_cl_Frame                                                   *mpo_Parent;
    int                                                             mi_Res;
    BIG_INDEX                                                       mul_DirIndex;
    BIG_INDEX                                                       mul_Selected;
    int                                                             mi_InfoHeight;
	MAIEDITEX_tdst_SelBitmapDes										mst_CurDes;

    HBITMAP                                                         mh_MipmapOnOff[2];
    HBITMAP                                                         mh_MipmapColorOnOff[2];
    HBITMAP                                                         mh_MipmapAlphaOnOff[2];
    HBITMAP                                                         mh_MipmapKeepBorderOnOff[2];
    HBITMAP                                                         mh_FontOnOff[2];
    HBITMAP                                                         mh_UpdatablePal[2];
    HBITMAP                                                         mh_InterfaceOnOff[2];
    HBITMAP                                                         mh_BumpMap[2];

    UINT                                                            mui_MipmapOnOff[2];
    UINT                                                            mui_MipmapColorOnOff[2];
    UINT                                                            mui_MipmapAlphaOnOff[2];


    CBrush                                                          *mpo_SelBrush;
    CBrush                                                          *mpo_UnselBrush;
    CBrush                                                          *mpo_ColorBoderBrush;
    CBrush                                                          *mpo_BackBrush;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void    OnDraw(class CDC *);
    int     mi_ResToWidth(void);
    BOOL    OnScroll(UINT, UINT, BOOL);

    int     i_GetInfoHeight();
    void    DisplayInfo( CDC *, RECT *, MAIEDITEX_tdst_BitmapDes *);
    BOOL    b_ClickOnInfo( MAIEDITEX_tdst_BitmapDes *, CPoint, LONG );

    void    SaveTex( MAIEDITEX_tdst_BitmapDes * );
	void	CreateTga( MAIEDITEX_tdst_BitmapDes *, int, int );
	void	ImportAlphaMask( MAIEDITEX_tdst_BitmapDes *, int );

	void	UpdateInfo( void );

    MAIEDITEX_tdst_BitmapDes *	GetDesUnderPoint( CPoint * );
	POSITION					GetPosUnderPoint( CPoint * );
/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg void    OnRButtonDown(UINT, CPoint);
    afx_msg void    OnLButtonDown(UINT, CPoint);
    afx_msg void    OnLButtonUp(UINT, CPoint);
    afx_msg void    OnMouseMove(UINT, CPoint);
    afx_msg void    OnLButtonDblClk(UINT, CPoint);
    afx_msg int     OnCreate( LPCREATESTRUCT );
    afx_msg void    OnKeyDown( UINT, UINT, UINT );

    DECLARE_MESSAGE_MAP()
};
#endif  /* ACTIVE_EDITORS */
