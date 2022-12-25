/*$T DIACOLOR_dlg.cpp GC!1.52 12/02/99 15:50:33 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIACOLOR_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKtoed.h"
#include "DIAlogs/DIAname_dlg.h"

#include "EDItors/Sources/OUTput/OUTframe.h"
#include "EDIeditors_infos.h"
#include "SOFT/SOFTcolor.h"

BEGIN_MESSAGE_MAP(EDIA_cl_ColorDialog, EDIA_cl_BaseDialog)
    ON_WM_MOUSEMOVE()
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_ERASEBKGND()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

#define GOURAUDDISCRET  50

static int gai_StockColorControl[ 10 ] = 
		{ 
			IDC_STATIC_STOCKCOLOR0,
			IDC_STATIC_STOCKCOLOR1,
			IDC_STATIC_STOCKCOLOR2,
			IDC_STATIC_STOCKCOLOR3,
			IDC_STATIC_STOCKCOLOR4,
			IDC_STATIC_STOCKCOLOR5,
			IDC_STATIC_STOCKCOLOR6,
			IDC_STATIC_STOCKCOLOR7,
			IDC_STATIC_STOCKCOLOR8,
			IDC_STATIC_STOCKCOLOR9
		};

/*
 ===================================================================================================
 ===================================================================================================
 */
EDIA_cl_ColorDialog::EDIA_cl_ColorDialog
(
    char            *_psz_Title,
    unsigned long   *ColorExt,
    void (*RefrechClbk) (unsigned long, unsigned long),
    unsigned long USRPARAM1,
    unsigned long USRPARAM2
) :
    EDIA_cl_BaseDialog(IDD_COLORCHOOSER)
{
    mi_Edit = -1;
	mb_IsInit = FALSE;
    IsClipped = 0;
    mo_Title = _psz_Title;
    OLDColor = *ColorExt;
    Color = ColorExt;
    ul_PreviousPaintColor = *Color;
    UpdateColorCoefs();
    OriginalSpe = Spectrum;
    OriginalSat = Saturation;
    UpdateColor();
    RefrechCallback = RefrechClbk;
    UserParam1 = USRPARAM1;
    UserParam2 = USRPARAM2;
    Changing = 0;
	mb_PickOn = FALSE;
    OldRECT1.top = OldRECT1.right = OldRECT1.bottom = OldRECT1.left = 0;
    MouseButtonIsDown = 0;
    
    mpul_ColorStock = NULL;
    /* specific editor */
    {
		EOUT_cl_Frame	*po_Out;
		po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
		if (po_Out)
			mpul_ColorStock = po_Out->mst_Ini.ul_ColorStock;
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
EDIA_cl_ColorDialog::~EDIA_cl_ColorDialog(void)
{
    o_Bmp.DeleteObject();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDIA_cl_ColorDialog::OnInitDialog(void)
{
    SetWindowText(mo_Title);
    o_Bmp.LoadBitmap(IDB_BITMAP3);
    CenterWindow(AfxGetMainWnd());
	mb_IsInit = TRUE;
	UpdateColor();
    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDIA_cl_ColorDialog::PreTranslateMessage(MSG *pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        OnKeyDown(pMsg->wParam, 0, 0);
    }
    return 0;
}


/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
ULONG u4_Interpol2PackedColor_KEEP_ALPHA1(ULONG ulP1, ULONG ulP2, float fZClipLocalCoef)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   RetValue, Interpoler;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ulP2 &= 0xffffff;
    if(fZClipLocalCoef >= 0.98f)
        return ulP2 | (ulP1 & 0xff000000);;
    if(fZClipLocalCoef <= 0.02f) return ulP1;
    *((float *) &Interpoler) = fZClipLocalCoef + 32768.0f + 16384.0f;
    RetValue = (Interpoler & 128) ? (ulP2 & 0xfefefe) >> 1 : (ulP1 & 0xfefefe) >> 1;
    RetValue += (Interpoler & 64) ? (ulP2 & 0xfcfcfc) >> 2 : (ulP1 & 0xfcfcfc) >> 2;
    RetValue += (Interpoler & 32) ? (ulP2 & 0xf8f8f8) >> 3 : (ulP1 & 0xf8f8f8) >> 3;
    RetValue += (Interpoler & 16) ? (ulP2 & 0xf0f0f0) >> 4 : (ulP1 & 0xf0f0f0) >> 4;
    RetValue += (Interpoler & 8) ? (ulP2 & 0xe0e0e0) >> 5 : (ulP1 & 0xe0e0e0) >> 5;
    RetValue += (Interpoler & 4) ? (ulP2 & 0xc0c0c0) >> 6 : (ulP1 & 0xc0c0c0) >> 6;
    RetValue += (Interpoler & 2) ? (ulP2 & 0x808080) >> 7 : (ulP1 & 0x808080) >> 7;
    return RetValue | (ulP1 & 0xff000000);
}

#define SWITCHMAX(A, B) \
    { \
        unsigned long   SWAP; \
        SWAP = A; \
        A = B; \
        B = SWAP; \
    }

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDIA_cl_ColorDialog::IF_IS_IN(int ITEM)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    RECT    LocalRect;
    POINT   MousePoint;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GetCursorPos(&MousePoint);
    GetDlgItem(ITEM)->GetWindowRect(&LocalRect);
    if
    (
        (MousePoint.y >= LocalRect.top) &&
        (MousePoint.y <= LocalRect.bottom) &&
        (MousePoint.x >= LocalRect.left) &&
        (MousePoint.x <= LocalRect.right)
    ) return TRUE;
    return FALSE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_ColorDialog::UpdateColorCoefs(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    double          r, g, b;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    
    r = (*Color & 0xff) * 1.0 / 255.0;
    g = ((*Color & 0xff00) >> 8) * 1.0 / 255.0;
    b = ((*Color & 0xff0000) >> 16) * 1.0 / 255.0;
    
    RGB_to_HLS( r, g, b, &Spectrum, &Luminosity, &Saturation);
    return;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_ColorDialog::UpdateColor(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    double          r, g, b;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    HLS_to_RGB( Spectrum, Luminosity, Saturation, &r, &g, &b );

    *Color &= 0xFF000000;
    *Color |= ((ULONG) (b * 255.0f)) << 16;
    *Color |= ((ULONG) (g * 255.0f)) << 8;
    *Color |= ((ULONG) (r * 255.0f));
    ul_PreviousPaintColor = *Color;

    HLS_to_RGB( Spectrum, .5f, Saturation, &r, &g, &b );
    Zoro = *Color & 0xFF000000;
    Zoro |= ((ULONG) (b * 255.0f)) << 16;
    Zoro |= ((ULONG) (g * 255.0f)) << 8;
    Zoro |= ((ULONG) (r * 255.0f));
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_ColorDialog::UpdateZoro(void)
{
	Zoro = *Color;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_ColorDialog::OnKeyDown(UINT _ui_Char, UINT _ui_Rep, UINT _ui_Flags )
{
    CPoint pt;
    int     i_Edit, i_Decal, i_Value, i_Change;

    if (mb_PickOn || MouseButtonIsDown ) 
    {
        EDIA_cl_BaseDialog::OnKeyDown( _ui_Char, _ui_Rep, _ui_Flags );
        return;
    }

    /* copy color into clipboard */
    if (GetAsyncKeyState(VK_CONTROL) < 0) 
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        HANDLE  hText;
        LPVOID  pvText;
        HANDLE  hRes;
		char	*sz_Text;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if (_ui_Char == 'C' )
	    {
            if(OpenClipboard())
		    {
			    if(EmptyClipboard())
				{
					hText = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, 11);
					if(hText != NULL)
					{
						pvText = GlobalLock(hText);
						if(pvText != NULL)
						{
							sprintf( (char *) pvText, "0x%08X" , *Color );
							hRes = SetClipboardData(CF_TEXT, hText);
							if(hRes == NULL) return;
							GlobalUnlock(hText);
						}
					}
				}
				CloseClipboard();
			}
		}
		else if (_ui_Char == 'V' )
	    {
            if(OpenClipboard())
		    {
			    hText = GetClipboardData(CF_TEXT);
				if(hText != NULL)
				{
					pvText = GlobalLock(hText);

					if(pvText != NULL)
					{
						sz_Text = (char *) pvText;
						if ( (*sz_Text == '0') && ( (sz_Text[ 1 ] == 'x') || (sz_Text[ 1 ] == 'X') ) )
						{
							sscanf( (char *) pvText, "0x%08X" , Color );
							UpdateColorCoefs();
							if(RefrechCallback)
								RefrechCallback(UserParam1, UserParam2);
							Invalidate();
							UpdateWindow();
						}
						GlobalUnlock(hText);
					}
				}
				CloseClipboard();
			}
		}
    }
    
    
    
    i_Change = 0;
    i_Edit = -1;

    if ( mi_Edit == -1)
    {
        if ( IF_IS_IN(IDC_INFORGBA_RGBARGBA) )
            i_Edit = 0;
        else  if ( IF_IS_IN(IDC_INFORGBA_RGBARGBA2) )
            i_Edit = 1;
        else  if ( IF_IS_IN(IDC_INFORGBA_RGBARGBA3) )
            i_Edit = 2;
        else if ( IF_IS_IN( IDC_ALPHAISYOURNAME ) )
            i_Edit = 3;
		GetCursorPos( &mo_PtEdit );
    }
    else 
        i_Edit = mi_Edit;

    if (i_Edit != -1)
    {
        i_Decal = i_Edit * 8;
        if (mi_Edit == -1)
            i_Value = 0;
        else
            i_Value = ((*Color >> (i_Decal)) & 0xFF);

        if ( ( _ui_Char >= VK_NUMPAD0 ) && (_ui_Char <= VK_NUMPAD9) )
            _ui_Char = _ui_Char - VK_NUMPAD0 + '0';

        if (isdigit( _ui_Char ))
        {
            i_Value = (i_Value * 10) + _ui_Char - '0';
            if (i_Value > 255) i_Value = 255;
            i_Change = 1;
        }
        else if (mi_Edit != -1)
        {
            if (_ui_Char == VK_BACK)
            {
                i_Value /= 10;
                i_Change = 1;
            }
            else if ( (_ui_Char == VK_RETURN) || (_ui_Char == VK_TAB ) )
            {
                mi_Edit = -1;
                return;
            }
        }

        mi_Edit = i_Edit;

        if (i_Change)
        {
            *Color &= ~(0xFF << i_Decal );
            *Color |= i_Value << i_Decal;
            UpdateColorCoefs();

            if(RefrechCallback)
                RefrechCallback(UserParam1, UserParam2);

            Invalidate();
            UpdateWindow();
        }
    }

    EDIA_cl_BaseDialog::OnKeyDown( _ui_Char, _ui_Rep, _ui_Flags );
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_ColorDialog::OnMouseMove(UINT, CPoint )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    RECT            LocalRect;
    POINT           MousePoint;
	CRect			o_Rect;
    unsigned long   REDRAW,Counter,TItem,TheAnd,SaveCo;
	CDC				*pDC;
	COLORREF		col;
    float           f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (mi_Edit != -1)
	{
		GetCursorPos(&MousePoint);
		if ( (lAbs( MousePoint.x - mo_PtEdit.x) > 2 ) || (lAbs( MousePoint.y - mo_PtEdit.y) > 1 ) )
			mi_Edit = -1;
	}

	SaveCo = 0;
	if(mb_PickOn)
	{	
		GetCursorPos(&MousePoint);
		pDC = GetDesktopWindow()->GetWindowDC();
		col = pDC->GetPixel(MousePoint.x, MousePoint.y);
		ReleaseDC(pDC);
		*Color &= 0xff000000;
		*Color |= col;
		REDRAW = 1;
		MouseButtonIsDown = 0;
		UpdateColorCoefs();
		::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PICK));
		goto zap;
	}

    REDRAW = 0;
    if(MouseButtonIsDown)
    {
        GetCursorPos(&MousePoint);
        if(IF_IS_IN(IDC_ALPHAISYOURNAME))
        {
            GetDlgItem(IDC_ALPHAISYOURNAME)->GetWindowRect(&LocalRect);
            LocalRect.right -= 1;
            if(!IsClipped)
            {
                GetClipCursor(&OldClip);
                ClipCursor(&LocalRect);
                IsClipped = 1;
            }

            f = (float) (MousePoint.x - LocalRect.left) / (float) (LocalRect.right - LocalRect.left - 2);
            f = (f < 0) ? 0 : (f>1) ? 1 : f;
            *Color &= 0xffffff;
            *Color |= ((unsigned long)(255.0f * f)) << 24;
            REDRAW = 1;
        }
		for (Counter = 0 ; Counter < 3 ; Counter++)
		{
			switch (Counter)
			{
			case 0: TItem = IDC_INFORGBA_RGBARGBA; TheAnd = 0; break;
			case 1: TItem = IDC_INFORGBA_RGBARGBA2;TheAnd = 8; break;
			case 2: TItem = IDC_INFORGBA_RGBARGBA3;TheAnd = 16; break;
			}
			if(IF_IS_IN(TItem))
			{
				GetDlgItem(TItem)->GetWindowRect(&LocalRect);
                LocalRect.right -= 1;
				if(!IsClipped)
				{
					GetClipCursor(&OldClip);
					ClipCursor(&LocalRect);
					IsClipped = 1;
				}

                f = (float) (MousePoint.x - LocalRect.left) / (float) (LocalRect.right - LocalRect.left - 2);
                f = (f < 0) ? 0 : (f>1) ? 1 : f;
				*Color &= ~(0xff << TheAnd);
				*Color |= ((unsigned long) (255.0f * f )) << TheAnd;
				SaveCo = *Color;
				UpdateColorCoefs();
				*Color = SaveCo ;
				REDRAW = 1;
				SaveCo = 1;
			}
		}

        if(IF_IS_IN(IDC_COLODETISHERE))
        {
            GetDlgItem(IDC_COLODETISHERE)->GetWindowRect(&LocalRect);
            if(!IsClipped)
            {
                GetClipCursor(&OldClip);
                ClipCursor(&LocalRect);
                IsClipped = 1;
            }

            Spectrum = (float) (MousePoint.x - LocalRect.left) / (float)
                (LocalRect.right - LocalRect.left);
            Saturation = (float) (MousePoint.y - LocalRect.top) / (float)
                (LocalRect.bottom - LocalRect.top);
            REDRAW = 1;
        }

        if(IF_IS_IN(IDC_LUMINOSITYHERE))
        {
            GetDlgItem(IDC_LUMINOSITYHERE)->GetWindowRect(&LocalRect);
            if(!IsClipped)
            {
                GetClipCursor(&OldClip);
                ClipCursor(&LocalRect);
                IsClipped = 1;
            }

            Luminosity = 1.0f - (float) (MousePoint.y - LocalRect.top) / (float)
                (LocalRect.bottom - LocalRect.top);
            REDRAW = 1;
        }

        if(IF_IS_IN(IDC_COLORISHERE))
        {
            GetDlgItem(IDC_COLORISHERE)->GetWindowRect(&LocalRect);
            ClipCursor(&OldClip);
            PostMessage(WM_CLOSE);
            if(RefrechCallback)
                RefrechCallback(UserParam1, UserParam2);
            return;
        }

        if(IF_IS_IN(IDC_COLORISHERECANCEL))
        {
            GetDlgItem(IDC_COLORISHERECANCEL)->GetWindowRect(&LocalRect);
            *Color = OLDColor;
            ClipCursor(&OldClip);
            PostMessage(WM_CLOSE);
            if(RefrechCallback)
                RefrechCallback(UserParam1, UserParam2);
            return;
        }

        if(IF_IS_IN(IDC_PICKCOLOR))
        {
			if(MouseButtonIsDown)
			{
				mul_OldColPick = *Color;
				mb_PickOn = TRUE;
				SetCapture();
				return;
			}
        }
        
        if (mpul_ColorStock)
		{
			for ( Counter = 0; Counter < 10; Counter++)
			{
				if (IF_IS_IN( gai_StockColorControl[ Counter ] ) )
				{
					*Color = mpul_ColorStock[ Counter ];
					UpdateColorCoefs();
					SaveCo = 1;
					REDRAW = 1;
					break;
				}
			}
		}
    }
    else if(IsClipped)
    {
        IsClipped = 0;
        ClipCursor(&OldClip);
    }

    Changing = 1/* IsClipped */;
    if(REDRAW)
    {
zap:
        if (!SaveCo) UpdateColor();
        if(RefrechCallback)
            RefrechCallback(UserParam1, UserParam2);

        Invalidate();
        UpdateWindow();
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_ColorDialog::OnLButtonDown(UINT, CPoint)
{
	MouseButtonIsDown = 1;
	OnMouseMove(0, 0);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_ColorDialog::OnLButtonUp(UINT, CPoint)
{
	if(mb_PickOn)
	{
		ReleaseCapture();
		mb_PickOn = FALSE;
	}
	else
	{
		MouseButtonIsDown = 0;
		OnMouseMove(0, 0);
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_ColorDialog::OnRButtonDown(UINT, CPoint)
{
	if(mb_PickOn)
	{
		ReleaseCapture();
		mb_PickOn = FALSE;
		*Color = mul_OldColPick;
		UpdateColorCoefs();
        UpdateColor();
        if(RefrechCallback)
            RefrechCallback(UserParam1, UserParam2);
        Invalidate();
		UpdateWindow();
	}
	else
	{
		int					Counter, TItem, TheAnd, f;
		int					REDRAW = 0;
		ULONG				SaveCo = 0;
		EDIA_cl_NameDialog	o_Dlg("Enter Color");

		for (Counter = 0 ; Counter < 4 ; Counter++)
		{
			switch (Counter)
			{
			case 0: TItem = IDC_INFORGBA_RGBARGBA; TheAnd = 0; break;
			case 1: TItem = IDC_INFORGBA_RGBARGBA2;TheAnd = 8; break;
			case 2: TItem = IDC_INFORGBA_RGBARGBA3;TheAnd = 16; break;
			case 3: TItem = IDC_ALPHAISYOURNAME; TheAnd = 24; break;
			}
			if(IF_IS_IN(TItem))
			{
				o_Dlg.mo_Name.Format("%d", (*Color >> TheAnd) & 0xFF);
				if(o_Dlg.DoModal() == IDOK)
				{
					f = L_atoi((char *) (LPCSTR) o_Dlg.mo_Name);
					if(f < 0) f = 0;
					if(f > 255) f = 255;
					*Color &= ~(0xff << TheAnd);
					*Color |= (f << TheAnd);
					SaveCo = *Color;
					UpdateColorCoefs();
					*Color = SaveCo ;
					REDRAW = 1;
					SaveCo = 1;
					goto ok;
				}
			}
		}

ok:
		if(REDRAW)
		{
			if (!SaveCo) UpdateColor();
			if(RefrechCallback)
				RefrechCallback(UserParam1, UserParam2);

			Invalidate();
			UpdateWindow();
		}
		else if (mpul_ColorStock)
		{
			for ( Counter = 0; Counter < 10; Counter++)
			{
				if (IF_IS_IN( gai_StockColorControl[ Counter ] ) )
				{
					mpul_ColorStock[ Counter ] = *Color;
					Invalidate();
					break;
				}
			}
		}
	}
}



/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_ColorDialog::OnPaint(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CDC             *PDC;
    HBRUSH          brh;
    HBRUSH          black;
    RECT            LocalRect, ColorRect2;
    PAINTSTRUCT     ps;
    float           Interp, inc;
    unsigned long   GouraudC, CurrentColor,Counter;
    CBrush          *Black2;
	char TextRGBA[32];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    
    if ( ul_PreviousPaintColor != *Color )
    {
		ul_PreviousPaintColor = *Color;
		UpdateColorCoefs();
		UpdateZoro();
	}

    BeginPaint(&ps);

	/* Erase bkgnd */
	GetDlgItem(IDC_ALPHAISYOURNAME)->GetWindowRect(&LocalRect);
	ScreenToClient(&LocalRect);
	ExcludeClipRect(ps.hdc, LocalRect.left - 1, LocalRect.top - 1, LocalRect.right + 1, LocalRect.bottom);

	GetDlgItem(IDC_INFORGBA_RGBARGBA)->GetWindowRect(&LocalRect);
	ScreenToClient(&LocalRect);
	ExcludeClipRect(ps.hdc, LocalRect.left, LocalRect.top, LocalRect.right, LocalRect.bottom);

	GetDlgItem(IDC_COLODETISHERE)->GetWindowRect(&LocalRect);
	ScreenToClient(&LocalRect);
	ExcludeClipRect(ps.hdc, LocalRect.left - 1, LocalRect.top - 1, LocalRect.right + 1, LocalRect.bottom);

	GetDlgItem(IDC_LUMINOSITYHERE)->GetWindowRect(&LocalRect);
	ScreenToClient(&LocalRect);
	ExcludeClipRect(ps.hdc, LocalRect.left - 1, LocalRect.top - 1, LocalRect.right + 1, LocalRect.bottom);

	GetDlgItem(IDC_COLORISHERE)->GetWindowRect(&LocalRect);
	ScreenToClient(&LocalRect);
	ExcludeClipRect(ps.hdc, LocalRect.left - 1, LocalRect.top - 1, LocalRect.right + 1, LocalRect.bottom);

	GetDlgItem(IDC_COLORISHERECANCEL)->GetWindowRect(&LocalRect);
	ScreenToClient(&LocalRect);
	ExcludeClipRect(ps.hdc, LocalRect.left - 1, LocalRect.top - 1, LocalRect.right + 1, LocalRect.bottom);

	GetDlgItem(IDC_INFORGBA_RGBARGBA)->GetWindowRect(&LocalRect);
	ScreenToClient(&LocalRect);
	ExcludeClipRect(ps.hdc, LocalRect.left - 1, LocalRect.top - 1, LocalRect.right + 1, LocalRect.bottom);

	GetDlgItem(IDC_INFORGBA_RGBARGBA2)->GetWindowRect(&LocalRect);
	ScreenToClient(&LocalRect);
	ExcludeClipRect(ps.hdc, LocalRect.left - 1, LocalRect.top - 1, LocalRect.right + 1, LocalRect.bottom);

	GetDlgItem(IDC_INFORGBA_RGBARGBA3)->GetWindowRect(&LocalRect);
	ScreenToClient(&LocalRect);
	ExcludeClipRect(ps.hdc, LocalRect.left - 1, LocalRect.top - 1, LocalRect.right + 1, LocalRect.bottom);



    brh = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	GetClientRect(&LocalRect);
    FillRect(ps.hdc, &LocalRect, brh);
    DeleteObject(brh);


	SelectClipRgn(ps.hdc, NULL);

	/* The rest */
    black = CreateSolidBrush(0);

    GetDlgItem(IDC_ALPHAISYOURNAME)->GetWindowRect(&LocalRect);
    ScreenToClient(&LocalRect);
    brh = CreateSolidBrush(0x00800000);
    FillRect(ps.hdc, &LocalRect, brh);
    DeleteObject(brh);
    brh = CreateSolidBrush(0);
    FrameRect(ps.hdc, &LocalRect, brh);
    SetBkColor(ps.hdc, 0x00800000);
    SetTextColor(ps.hdc, 0xffffff);
    SelectObject(ps.hdc, M_MF()->mo_Fnt);
	sprintf(TextRGBA , "ALPHA:%4d  (%x)", (*Color >> 24) & 0xFF, (*Color >> 24) & 0xFF);

    DrawText(ps.hdc, TextRGBA, strlen(TextRGBA), &LocalRect, DT_CENTER | DT_VCENTER);
    DeleteObject(brh);
    Interp = (float) (*(Color) >> 24) / 255.0f;
    LocalRect.right = LocalRect.left + (int) ((float) (LocalRect.right - LocalRect.left) * Interp);
    InvertRect(ps.hdc, &LocalRect);

	/* Frame alpha */
    GetDlgItem(IDC_ALPHAISYOURNAME)->GetWindowRect(&LocalRect);
    ScreenToClient(&LocalRect);
	LocalRect.left--;
	LocalRect.top--;
	LocalRect.right++;
    PDC = GetDC();
    PDC->Draw3dRect(&LocalRect, 0, GetSysColor(COLOR_3DHILIGHT));
    ReleaseDC(PDC);

	for (Counter = 0 ; Counter < 3 ; Counter++)
	{
		int SaveLeft,Decal;
		switch (Counter)
		{
		case 0:GetDlgItem(IDC_INFORGBA_RGBARGBA)->GetWindowRect(&LocalRect);	Decal = 0; break;
		case 1:GetDlgItem(IDC_INFORGBA_RGBARGBA2)->GetWindowRect(&LocalRect);	Decal = 8; break;
		case 2:GetDlgItem(IDC_INFORGBA_RGBARGBA3)->GetWindowRect(&LocalRect);	Decal =16; break;
		}
		ScreenToClient(&LocalRect);
		brh = CreateSolidBrush(0x9f << Decal);
		Interp = (float) ((*Color >> Decal) & 0xff) / 255.0f;
		SaveLeft = LocalRect.right ;
		LocalRect.right = LocalRect.left + (int) ((float) (LocalRect.right - LocalRect.left) * Interp);
		FillRect(ps.hdc, &LocalRect, brh);
		LocalRect.right  = SaveLeft ;
		DeleteObject(brh);


		brh = CreateSolidBrush(0x3f << Decal);
		SaveLeft = LocalRect.left ;
		LocalRect.left = LocalRect.left + (int) ((float) (LocalRect.right - LocalRect.left) * Interp);
		FillRect(ps.hdc, &LocalRect, brh);
		LocalRect.left = SaveLeft ;
		DeleteObject(brh);

		SetTextColor(ps.hdc, 0xffffff);
		SetBkMode(ps.hdc, TRANSPARENT);
		SelectObject(ps.hdc, M_MF()->mo_Fnt);
		sprintf(TextRGBA , "%4d  (%x)", (*Color >> Decal) & 0xFF, (*Color >> Decal) & 0xFF);
		DrawText(ps.hdc, TextRGBA , strlen(TextRGBA) , &LocalRect, DT_CENTER | DT_VCENTER);

		LocalRect.left--;
		LocalRect.top--;
		LocalRect.right++;
		LocalRect.bottom++;
	    PDC = GetDC();
	    PDC->Draw3dRect(&LocalRect, 0, GetSysColor(COLOR_3DHILIGHT));
	    ReleaseDC(PDC);

	}


    GetDlgItem(IDC_COLORISHERE)->GetWindowRect(&LocalRect);
    ScreenToClient(&LocalRect);
    brh = CreateSolidBrush(*Color & 0xffffff);
    FillRect(ps.hdc, &LocalRect, brh);
    SetBkColor(ps.hdc, *Color & 0xffffff);
    SetTextColor(ps.hdc, (((*Color & 0xfefefe) + 0x808080)) & 0xfefefe);
    LocalRect.top += 10;
    SelectObject(ps.hdc, M_MF()->mo_Fnt);
    DrawText(ps.hdc, "Ok", 2, &LocalRect, DT_CENTER | DT_VCENTER);
    LocalRect.top -= 10;
    DeleteObject(brh);

    GetDlgItem(IDC_COLORISHERE)->GetWindowRect(&LocalRect);
    ScreenToClient(&LocalRect);
	LocalRect.left--;
	LocalRect.top--;
	LocalRect.right++;
    PDC = GetDC();
    PDC->Draw3dRect(&LocalRect, GetSysColor(COLOR_3DHILIGHT), 0);
    ReleaseDC(PDC);

    GetDlgItem(IDC_COLORISHERECANCEL)->GetWindowRect(&LocalRect);
    ScreenToClient(&LocalRect);
    brh = CreateSolidBrush(OLDColor & 0xffffff);
    FillRect(ps.hdc, &LocalRect, brh);
    SetBkColor(ps.hdc, OLDColor & 0xffffff);
    SetTextColor(ps.hdc, (((OLDColor & 0xfefefe) + 0x808080)) & 0xfefefe);
    LocalRect.top += 10;
    SelectObject(ps.hdc, M_MF()->mo_Fnt);
    DrawText(ps.hdc, "Cancel", 6, &LocalRect, DT_CENTER | DT_VCENTER);
    LocalRect.top -= 10;
    DeleteObject(brh);

    GetDlgItem(IDC_COLORISHERECANCEL)->GetWindowRect(&LocalRect);
    ScreenToClient(&LocalRect);
	LocalRect.left--;
	LocalRect.top--;
	LocalRect.right++;
    PDC = GetDC();
    PDC->Draw3dRect(&LocalRect, GetSysColor(COLOR_3DHILIGHT), 0);
    ReleaseDC(PDC);

    Interp = 0.0f;
    GetDlgItem(IDC_LUMINOSITYHERE)->GetWindowRect(&LocalRect);
    ScreenToClient(&LocalRect);
    ColorRect2 = LocalRect;
    inc = 1.0f / (float) GOURAUDDISCRET;
    for(GouraudC = 0; GouraudC < GOURAUDDISCRET; GouraudC++)
    {
        ColorRect2.top = LocalRect.top + (long)
            (
                ((float) GouraudC / (float) GOURAUDDISCRET) * (float)
                    (LocalRect.bottom - LocalRect.top)
            );
        ColorRect2.bottom = LocalRect.top + (long)
            (
                ((float) (GouraudC + 1) / (float) GOURAUDDISCRET) * (float)
                    (LocalRect.bottom - LocalRect.top)
            );
        Interp = 1.0f - Interp;
        if(Interp > 0.5f)
            CurrentColor = u4_Interpol2PackedColor_KEEP_ALPHA1( Zoro, 0xFFFFFF, 2.0f * (Interp - 0.5f) );
        else
            CurrentColor = u4_Interpol2PackedColor_KEEP_ALPHA1(Zoro, 0, 2.0f * (0.5f - Interp));

        Interp = 1.0f - Interp;
        brh = CreateSolidBrush(CurrentColor & 0xffffff);
        FillRect(ps.hdc, &ColorRect2, brh);
        DeleteObject(brh);
        Interp += inc;
    }

    GetDlgItem(IDC_LUMINOSITYHERE)->GetWindowRect(&LocalRect);
    ScreenToClient(&LocalRect);
    LocalRect.left--;
    LocalRect.top--;
    LocalRect.right++;
    LocalRect.bottom++;
    Luminosity = 1.0f - Luminosity;
    ColorRect2.top = LocalRect.top + (long) (Luminosity * (float) (LocalRect.bottom - LocalRect.top));
    ColorRect2.bottom = ColorRect2.top;
    Luminosity = 1.0f - Luminosity;
	ColorRect2.right += 4;
	ColorRect2.left -= 4;
	ColorRect2.bottom += 2;

	/* Frame lum */
    GetDlgItem(IDC_LUMINOSITYHERE)->GetWindowRect(&LocalRect);
    ScreenToClient(&LocalRect);
	LocalRect.left--;
	LocalRect.top--;
	LocalRect.right++;
    PDC = GetDC();
    PDC->Draw3dRect(&LocalRect, 0, GetSysColor(COLOR_3DHILIGHT));
    ReleaseDC(PDC);
    InvertRect(ps.hdc, &ColorRect2);

	/* Frame color */
    GetDlgItem(IDC_COLODETISHERE)->GetWindowRect(&LocalRect);
    ScreenToClient(&LocalRect);
	LocalRect.left--;
	LocalRect.top--;
	LocalRect.right++;
    PDC = GetDC();
    PDC->Draw3dRect(&LocalRect, 0, GetSysColor(COLOR_3DHILIGHT));
    ReleaseDC(PDC);

    /* Draw mark */
    PDC = GetDC();
    GetDlgItem(IDC_COLODETISHERE)->GetWindowRect(&LocalRect);
    ScreenToClient(&LocalRect);
    PDC->DrawState
        (
            CPoint(LocalRect.left, LocalRect.top),
            CSize(LocalRect.right - LocalRect.left, LocalRect.bottom - LocalRect.top),
            &o_Bmp,
            DST_BITMAP
        );
    GetDlgItem(IDC_COLODETISHERE)->GetWindowRect(&LocalRect);
    ScreenToClient(&LocalRect);
	
    ColorRect2 = LocalRect;
    ColorRect2.left = LocalRect.left + (long)
        (OriginalSpe * (float) (LocalRect.right - LocalRect.left));
    ColorRect2.right = ColorRect2.left + 2;
    ColorRect2.left -= 2;
    ColorRect2.top = LocalRect.top + (long)
        (OriginalSat * (float) (LocalRect.bottom - LocalRect.top));
    ColorRect2.bottom = ColorRect2.top + 2;
    ColorRect2.top -= 2;
    Black2 = new CBrush((COLORREF) 0x0000);
    PDC->FrameRect(&ColorRect2, Black2);

    ColorRect2 = LocalRect;
    ColorRect2.left = LocalRect.left + (long)
        (Spectrum * (float) (LocalRect.right - LocalRect.left));
    ColorRect2.right = ColorRect2.left + 4;
    ColorRect2.left -= 4;
    ColorRect2.top = LocalRect.top + (long)
        (Saturation * (float) (LocalRect.bottom - LocalRect.top));
    ColorRect2.bottom = ColorRect2.top + 4;
    ColorRect2.top -= 4;
    PDC->FrameRect(&ColorRect2, Black2);
    
    /* le stock de couleur */
    if ( mpul_ColorStock )
    {
		ULONG	ul_Color, alpha;
		RECT	st_Rect;
		int		i, bottom;
		
		for (i = 0; i < 10; i++)
		{
			ul_Color = mpul_ColorStock[ i ];
			GetDlgItem( gai_StockColorControl[ i ] )->GetWindowRect( &st_Rect );
			ScreenToClient( &st_Rect );
			bottom = st_Rect.bottom;
			st_Rect.bottom = st_Rect.top + (int) ((float) (bottom - st_Rect.top) * 0.75);
			PDC->FillSolidRect( &st_Rect, ul_Color & 0xFFFFFF );
			st_Rect.top = st_Rect.bottom;
			st_Rect.bottom = bottom;
			alpha = (ul_Color & 0xFF000000) >> 24;
			ul_Color = alpha | (alpha << 8) | (alpha << 16 );
			PDC->FillSolidRect( &st_Rect, ul_Color & 0xFFFFFF );
		}
    }
    
    delete(Black2);
    ReleaseDC(PDC);
    DeleteObject(black);

    EndPaint(&ps);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDIA_cl_ColorDialog::OnEraseBkgnd(CDC *pdc)
{
    return TRUE;
}


#endif /* ACTIVE_EDITORS */
