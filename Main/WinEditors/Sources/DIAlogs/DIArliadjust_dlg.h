/*$T DIArli_dlg.h GC! 1.081 06/05/01 10:50:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class	F3D_cl_View;
class	EDIA_cl_ToolBox_GROView;
class	EDIA_cl_Palette;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
 typedef struct EDIA_tdst_RLIAdjust_
 {
	char	*pc_Sel;
	ULONG	*pul_RLI;
	ULONG	*dul_RLISave;
 } EDIA_tdst_RLIAdjust;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EDIA_cl_RLIAdjustDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_RLIAdjustDialog( F3D_cl_View *, EDIA_cl_ToolBox_GROView *, BOOL mb_SubObject );
	EDIA_cl_RLIAdjustDialog( EDIA_cl_Palette *, ULONG *, char *, int );
	
	BOOL    PreTranslateMessage( MSG *);
	void	OnCancel(void);
	void	Refresh( void );
	
	int		Value2Coord( int, int );
	int		Coord2Value( int, int );
	void	DisplayValues( void );
	void	ChangeMode( int );
	
	void	RLI_Save( void );
	void	RLI_Restore( void );
	void	RLI_Compute_HLS( void );
	void	RLI_Compute_HLS_Colorize( void );
	void	RLI_Compute_BC( void );
	void	RLI_Compute_AddRGB( void );
	void	RLI_Compute_MulRGB( void );
	void	RLI_Compute_Alpha( void );
	void	RLI_Compute( void );
	void	RLI_Free( void );
	ULONG	RLI_AdjustHLS( ULONG );
	ULONG	RLI_AdjustHLSColorize( ULONG );
	ULONG	RLI_AdjustRGBAdd( ULONG );
	ULONG	RLI_AdjustAlphaAdd( ULONG );
	ULONG	RLI_AdjustRGBMul( ULONG );

#ifdef JADEFUSION
    void XenonRefresh(void);
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    F3D_cl_View					*mpo_3DView;
    EDIA_cl_ToolBox_GROView		*mpo_ToolBox;
    EDIA_cl_Palette				*mpo_Palette;
    
    int					mb_SubObject;
    
    int					mi_Value[ 4 ];
    int					mi_SaveValue[ 4 ];
    int					mi_Min[ 4 ];
    int					mi_Max[ 4 ];
    CRect				mo_Rect[ 5 ];
    
    int					mi_Capture;
    
    int					mi_RLINb;
    int					mi_RLICur;
    EDIA_tdst_RLIAdjust *mpst_RLIAdjust;
    
    ULONG				*mpul_PaletteColor;
    char				*mpc_PaletteSel;
    int					mi_PaletteColorNb;
    ULONG				mul_PaletteSave[ 256 ];
    
    double				mdl, mds, mdh;
	double				mdr, mdg, mdb;
	double				mdalpha;

    int					mi_PreviewOnSlide;
    int					mi_HideSubObject;
    int					mi_Colorize;
    int					mi_CurMode;
    int					mi_Cafe;
    
    double				maf_Rlum[ 256 ];
    double				maf_Glum[ 256 ];
    double				maf_Blum[ 256 ];
    double				maf_Rfinal[ 256 ];
    double				maf_Gfinal[ 256 ];
    double				maf_Bfinal[ 256 ];
    
    

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

protected:
	afx_msg void	OnDestroy(void);
	afx_msg void	OnPaint(void);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonUp(UINT, CPoint);
	afx_msg void	OnRButtonDown(UINT, CPoint);
	afx_msg void	OnMouseMove(UINT, CPoint);
	afx_msg void	OnBt_PreviewOnSlide( void );
	afx_msg void	OnBt_HideSubObject( void );
	afx_msg void	OnBt_Colorize( void );
	afx_msg BOOL    OnMouseWheel(UINT, short, CPoint);
	afx_msg void	OnRadio_Mode1( void );
	afx_msg void	OnRadio_Mode2( void );
	afx_msg void	OnRadio_Mode3( void );
	afx_msg void	OnRadio_Mode4( void );
	
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
