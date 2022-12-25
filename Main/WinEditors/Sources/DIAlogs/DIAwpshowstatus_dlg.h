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

class EDIA_cl_WPShowStatus : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_WPShowStatus(ULONG *, ULONG *);
	~EDIA_cl_WPShowStatus(void);


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    ULONG                   mul_TextureKey;
    ULONG                   mul_TextureIndex;
	int						mi_Slot;
    BOOL                    mb_TexAlpha;
    BOOL                    mb_TexIndex;
    ULONG                   mul_RawIndex;
    ULONG                   mul_TgaIndex;

    unsigned char           *mpc_Raw;
    HBITMAP					mh_BitmapRGB;
    CDC                     *mpo_DC;
	int						mi_BmpWidth, mi_BmpHeight;
    CRect                   mo_DrawRect;
    BOOL                    mb_ShowRGB, mb_ShowA;

    TEX_tdst_Palette        mst_Palette;
    
    char					c_SaveValue[ 65 ];
    char					c_Value[ 65 ];
    
    ULONG					*mpul_Flags;
    ULONG					*mpul_Flags2;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog( void );
    BOOL    PreTranslateMessage( MSG *);

	void	UpdateTableFromFlags();
	void	UpdateFlagsFromTable();
    void    UpdateTexture();
    void    DeleteBitmap();

	void	OnOK(void);
	void	OnCancel(void);

    void    ComputeDrawRect( void );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnPaint(void);
    afx_msg void    OnLButtonDown( UINT, CPoint );
    afx_msg void	OnAll(void);
    afx_msg void	OnNone(void);
	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
