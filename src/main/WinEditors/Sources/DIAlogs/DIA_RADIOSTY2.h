/*$T DIA_RADIOSTY2.h GC!1.52 12/02/99 14:10:24 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"

#include "BASe/BAStypes.h"

class EDIA_cl_radiosity2 : public EDIA_cl_BaseDialog
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDIA_cl_radiosity2
    (
        char            *_psz_Title,
        ULONG   *Color,
        void (*RefrechCallback) (ULONG, ULONG),
        ULONG USRPARAM1,
        ULONG USRPARAM2
    );
    ~EDIA_cl_radiosity2(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    CString         mo_Name;
    CString         mo_Title;
    int             mi_MaxLen;
    BOOL            mb_Pass;
	BOOL            mb_IsInit;
    ULONG           *Color;
    ULONG           OLDColor;
	COLORREF		mul_OldColPick;
    ULONG           Zoro;
    ULONG           Changing;

	CPoint			mo_PtEdit;
    
    double          Spectrum;
    double          Luminosity;
    double          Saturation;
    double          OriginalSpe;
    double          OriginalSat;
    double          OriginalLum;
    
    RECT            OldRECT1;
	BOOL			mb_PickOn;
	CBitmap			o_Bmp;
    RECT            OldClip;
    int             IsClipped;
    int             MouseButtonIsDown;
    int             mi_Edit;

    ULONG           UserParam1;
    ULONG           UserParam2;

    void (*RefrechCallback) (ULONG USRPARAM1, ULONG USRPARAM2);

/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void    UpdateColor(void);
    void    UpdateColorCoefs(void);
    BOOL    OnInitDialog(void);
    BOOL    IF_IS_IN(int);

    BOOL    PreTranslateMessage(MSG *);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg void    OnMouseMove(UINT, CPoint);
    afx_msg void    OnPaint(void);
    afx_msg void    OnLButtonDown(UINT, CPoint);
    afx_msg void    OnLButtonUp(UINT, CPoint);
    afx_msg void    OnRButtonDown(UINT, CPoint);
	afx_msg BOOL	OnEraseBkgnd(CDC *);
    afx_msg void    OnKeyDown( UINT, UINT, UINT );
    DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
