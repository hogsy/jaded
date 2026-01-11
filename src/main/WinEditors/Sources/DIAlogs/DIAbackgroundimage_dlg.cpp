/*$T DIAbackgroundimage_dlg.cpp GC! 1.081 05/09/00 14:12:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbackgroundimage_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "EDIpaths.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "F3DFrame/F3Dview.h"
#include "SOFT/SOFTbackgroundImage.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINks/LINKtoed.h"
#include "TEXture/TEXfile.h"

BEGIN_MESSAGE_MAP(EDIA_cl_BackgroundImage, EDIA_cl_BaseDialog)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
    ON_BN_CLICKED(IDC_BUTTON_SAVE, OnSaveClick)
    ON_BN_CLICKED(IDC_BUTTON_LOAD, OnLoadClick)
	ON_BN_CLICKED(IDC_BUTTON_UPDATEDATA, OnUpdateDataClick)
	ON_BN_CLICKED(IDC_BUTTON_FIRSTPICTURE, OnFirstPictureClick)
	ON_BN_CLICKED(IDC_BUTTON_LASTPICTURE, OnLastPictureClick)
	ON_BN_CLICKED(IDC_BUTTON_MPEG, OnMpegClick)
	ON_BN_CLICKED(IDC_CHECK_VISIBLE, OnVisibleClick)
	ON_BN_CLICKED(IDC_CHECK_FREEZE, OnFreezeClick)
    ON_BN_CLICKED(IDC_CHECK_MPEGWB, OnMpegBWClick )
	ON_BN_CLICKED(IDC_CHECK_OUTOFBF, OnOutOfBFClick)
    ON_BN_CLICKED(IDC_RADIO_MPEG, OnRadioMpeg)
    ON_BN_CLICKED(IDC_RADIO_PICTURELIST, OnRadioPictureList)
    ON_CBN_KILLFOCUS(IDC_COMBO_ANIMFREQ, OnAnimFreqChange)
    ON_CBN_KILLFOCUS(IDC_COMBO_FILMFREQ, OnFilmFreqChange)
    ON_CBN_SELCHANGE(IDC_COMBO_ANIMFREQ, OnAnimFreqSelChange)
    ON_CBN_SELCHANGE(IDC_COMBO_FILMFREQ, OnFilmFreqSelChange)
    ON_EN_CHANGE( IDC_EDIT_DEPTH, OnEditDepth )
    ON_EN_KILLFOCUS(IDC_EDIT_DEPTH, OnUpdateDepth )
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_BackgroundImage::EDIA_cl_BackgroundImage(F3D_cl_View *_po_View) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_BACKGROUNDIMAGE)
{
	mpo_View = _po_View;
	mpst_BI = _po_View->mst_WinHandles.pst_DisplayData->pst_BackgroundImage;
	mpst_LocalBI = (SOFT_tdst_BackgroundImage *) L_malloc(sizeof(SOFT_tdst_BackgroundImage));
	L_memcpy(mpst_LocalBI, mpst_BI, sizeof(SOFT_tdst_BackgroundImage));
	mi_MoveOrSize = 0;
	mi_DepthMove = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_BackgroundImage::~EDIA_cl_BackgroundImage(void)
{
	L_free(mpst_LocalBI);
	if(mpo_View) mpo_View->mpo_BackgroundImageDialog = NULL;
} 

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_BackgroundImage::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	CButton     *o_Button;
	CRect	    o_Rect;
    CComboBox   *o_Combo;
    int         i;
    char        sz_Value[ 32];
	/*~~~~~~~~~~~~~~~~~~~*/

	if(mpst_BI->ul_Flags & SOFT_C_BI_Mpeg)
		o_Button = (CButton *) GetDlgItem(IDC_RADIO_MPEG);
	else
		o_Button = (CButton *) GetDlgItem(IDC_RADIO_PICTURELIST);
	o_Button->SetCheck(1);

	GetDlgItem(IDC_STATIC_SIZEANDPOS)->GetWindowRect(&o_Rect);
	mi_WindowWidth = o_Rect.Width();
	mi_WindowHeight = o_Rect.Height();

    /* fill anim frequency combo */
    o_Combo = (CComboBox *) GetDlgItem( IDC_COMBO_ANIMFREQ );
    i = o_Combo->AddString( "15" );  o_Combo->SetItemData(i, 15 );
    i = o_Combo->AddString( "25" );  o_Combo->SetItemData(i, 25 );
    i = o_Combo->AddString( "30" );  o_Combo->SetItemData(i, 30 );
    i = o_Combo->AddString( "60" );  o_Combo->SetItemData(i, 60 );

    /* fill film frequency combo */
    o_Combo = (CComboBox *) GetDlgItem( IDC_COMBO_FILMFREQ );
    i = o_Combo->AddString( "15" );  o_Combo->SetItemData(i, 15 );
    i = o_Combo->AddString( "25" );  o_Combo->SetItemData(i, 25 );
    i = o_Combo->AddString( "30" );  o_Combo->SetItemData(i, 30 );
    i = o_Combo->AddString( "60" );  o_Combo->SetItemData(i, 60 );

    GetDlgItem( IDC_EDIT_NAME )->SetWindowText( mpst_BI->sz_Name);

    sprintf( sz_Value, "%.4f", mpst_BI->depth );
    GetDlgItem( IDC_EDIT_DEPTH )->SetWindowText( sz_Value ); 

    UpdateFreq( 0 );
    UpdateData();

    CenterWindow();

	return TRUE;
}

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EDIA_cl_BackgroundImage::ul_GetPicture(char *sz_Title, char *sz_FileName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File(sz_Title, 3, TRUE, (mpst_LocalBI->ul_Flags & SOFT_C_BI_NotInBF) ? FALSE : TRUE, NULL);
	CString				o_Temp;
	ULONG				ul_Index;
	char				*psz_Ext;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		o_File.GetItem(o_File.mo_File, 1, o_Temp);
		if(BIG_b_CheckName((char *) (LPCTSTR) o_Temp) == FALSE) return BIG_C_InvalidKey;

		if(strstr(sz_Title, "mpeg"))
		{
			psz_Ext = strrchr((char *) (LPCTSTR) o_Temp, '.');
			if(!psz_Ext) return BIG_C_InvalidKey;
			if(L_stricmp(psz_Ext + 1, "mpg") && L_stricmp(psz_Ext + 1, "mpeg")) return BIG_C_InvalidKey;
		}
		else
		{
			if(!TEX_l_File_IsFormatSupported((char *) (LPCTSTR) o_Temp, -1)) return BIG_C_InvalidKey;
		}

		if(mpst_LocalBI->ul_Flags & SOFT_C_BI_NotInBF)
		{
            strcpy( sz_FileName, o_File.masz_FullPath);
            if ( sz_FileName[strlen(sz_FileName) - 1] != '/' ) strcat( sz_FileName, "/" );
            strcat( sz_FileName, (char *) (LPCTSTR) o_Temp );
			return 0;
		}
		else
		{
			ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCTSTR) o_Temp);
			if(ul_Index == BIG_C_InvalidIndex) return BIG_C_InvalidKey;
			return BIG_FileKey(ul_Index);
		}
	}

	return BIG_C_InvalidKey;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::UpdateData(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Index;
	char	sz_File[260];
	/*~~~~~~~~~~~~~~~~~*/

	((CButton *) GetDlgItem(IDC_CHECK_OUTOFBF))->SetCheck((mpst_LocalBI->ul_Flags & SOFT_C_BI_NotInBF) ? 1 : 0);
	((CButton *) GetDlgItem(IDC_CHECK_VISIBLE))->SetCheck((mpst_BI->ul_Flags & SOFT_C_BI_Visible) ? 1 : 0);
	((CButton *) GetDlgItem(IDC_CHECK_FREEZE))->SetCheck((mpst_BI->ul_Flags & SOFT_C_BI_Freeze) ? 1 : 0);
    ((CButton *) GetDlgItem(IDC_CHECK_MPEGWB))->SetCheck((mpst_BI->ul_Flags & SOFT_C_BI_MPEG_BlackAndWhite) ? 1 : 0);

	if(mpst_LocalBI->ul_Flags & SOFT_C_BI_NotInBF)
	{
		sprintf(sz_File, mpst_LocalBI->sz_PLFile, mpst_LocalBI->i_FirstIndex);
		GetDlgItem(IDC_EDIT_FIRSTPICTURE)->SetWindowText(sz_File);
		sprintf(sz_File, mpst_LocalBI->sz_PLFile, mpst_LocalBI->i_LastIndex);
		GetDlgItem(IDC_EDIT_LASTPICTURE)->SetWindowText(sz_File);
		GetDlgItem(IDC_EDIT_MPEG)->SetWindowText(mpst_LocalBI->sz_MpegFile);
	}
	else
	{
		if(mpst_LocalBI->ul_PLKey == BIG_C_InvalidKey)
		{
			GetDlgItem(IDC_EDIT_FIRSTPICTURE)->SetWindowText("<none>");
		}
		else
		{
			ul_Index = BIG_ul_SearchKeyToFat(mpst_LocalBI->ul_PLKey);
			if(ul_Index == BIG_C_InvalidIndex)
			{
				mpst_LocalBI->ul_PLKey = BIG_C_InvalidKey;
				UpdateData();
				return;
			}

			GetDlgItem(IDC_EDIT_FIRSTPICTURE)->SetWindowText(BIG_NameFile(ul_Index));
		}

		if(mpst_LocalBI->ul_PLLastKey == BIG_C_InvalidKey)
		{
			GetDlgItem(IDC_EDIT_LASTPICTURE)->SetWindowText("<none>");
		}
		else
		{
			ul_Index = BIG_ul_SearchKeyToFat(mpst_LocalBI->ul_PLLastKey);
			if(ul_Index == BIG_C_InvalidIndex)
			{
				mpst_LocalBI->ul_PLLastKey = BIG_C_InvalidKey;
				UpdateData();
				return;
			}

			GetDlgItem(IDC_EDIT_LASTPICTURE)->SetWindowText(BIG_NameFile(ul_Index));
		}

		if(mpst_LocalBI->ul_MpegKey  == BIG_C_InvalidKey)
		{
			GetDlgItem(IDC_EDIT_MPEG)->SetWindowText("<none>");
		}
		else
		{
			ul_Index = BIG_ul_SearchKeyToFat(mpst_LocalBI->ul_MpegKey);
			if(ul_Index == BIG_C_InvalidIndex)
			{
				mpst_LocalBI->ul_MpegKey = BIG_C_InvalidKey;
				UpdateData();
				return;
			}

			GetDlgItem(IDC_EDIT_MPEG)->SetWindowText(BIG_NameFile(ul_Index));
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::RectBI2W(void)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	int		w, h;
	/*~~~~~~~~~~~*/

	GetDlgItem(IDC_STATIC_SIZEANDPOS)->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);

	w = o_Rect.Width();
	h = o_Rect.Height();

	mo_SizeAndPos.left = o_Rect.left + (int) (mpst_BI->x * w);
	mo_SizeAndPos.bottom = o_Rect.top + (int) ((1.0f - mpst_BI->y) * h);
	mo_SizeAndPos.right = o_Rect.left + (int) ((mpst_BI->x + mpst_BI->w) * w);
	mo_SizeAndPos.top = o_Rect.top + (int) ((1.0f - (mpst_BI->y + mpst_BI->h)) * h);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::UpdateFreq( BOOL _b_RatioChange )
{
    char sz_Value[32];

    if (_b_RatioChange )
        mpst_BI->w_FilmFreq = (short) (mpst_BI->w_AnimFreq * mpst_BI->f_FreqRatio);
    else
        mpst_BI->f_FreqRatio = (float) mpst_BI->w_FilmFreq / (float) mpst_BI->w_AnimFreq;

    GetDlgItem( IDC_COMBO_ANIMFREQ )->SetWindowText( _itoa( mpst_BI->w_AnimFreq, sz_Value, 10 ) );
    GetDlgItem( IDC_COMBO_FILMFREQ )->SetWindowText( _itoa( mpst_BI->w_FilmFreq, sz_Value, 10 ) );
    sprintf( sz_Value, "%.4f", mpst_BI->f_FreqRatio );
    GetDlgItem( IDC_EDIT_FREQRATIO )->SetWindowText( sz_Value );
}

/*$4
 ***********************************************************************************************************************
    message handlers
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnMouseMove(UINT nFlags, CPoint o_Pt)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	float	dx, dy;
	/*~~~~~~~~~~~*/

	if(mi_MoveOrSize)
	{
		dx = ((float) o_Pt.x - mo_PtWhenLBDown.x) / mi_WindowWidth;
		dy = ((float) mo_PtWhenLBDown.y - o_Pt.y) / mi_WindowHeight;

        SOFT_BackgroundImage_MoveSize( mpst_BI, mi_MoveOrSize, dx, dy, !(nFlags & MK_SHIFT) );

		GetDlgItem(IDC_STATIC_SIZEANDPOS)->GetWindowRect(&o_Rect);
		ScreenToClient(&o_Rect);
		o_Rect.InflateRect(2, 2);
		InvalidateRect(&o_Rect);

		mo_PtWhenLBDown = o_Pt;

        LINK_Refresh();
	}
	/*$F
    else if(mi_DepthMove)
	{
        mpst_BI->z = (float) (o_Pt.y - mo_DepthRect.bottom) / ((float) mo_DepthRect.top - mo_DepthRect.bottom);
		InvalidateRect(&mo_DepthRect);

        LINK_Refresh();
	}
    */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnLButtonDown(UINT nFlags, CPoint o_Pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect, o_Rect2;
	int		dx1, dx2, dy1, dy2;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_STATIC_SIZEANDPOS)->GetWindowRect(&o_Rect);
	o_Rect2 = o_Rect;
	ScreenToClient(&o_Rect2);
	o_Rect2.InflateRect(2, 2);

	mo_SizeAndPos.InflateRect(2, 2);

	if(mo_SizeAndPos.PtInRect(o_Pt) && o_Rect2.PtInRect(o_Pt))
	{
		dx1 = abs(o_Pt.x - mo_SizeAndPos.left);
		dx2 = abs(o_Pt.x - mo_SizeAndPos.right);
		dy1 = abs(o_Pt.y - mo_SizeAndPos.top);
		dy2 = abs(o_Pt.y - mo_SizeAndPos.bottom);

		if(dy1 < 4)
			mi_MoveOrSize = (dx1 < 4) ? 2 : ((dx2 < 4) ? 4 : 3);
		else if(dx2 < 4)
			mi_MoveOrSize = (dy1 < 4) ? 4 : ((dy2 < 4) ? 6 : 5);
		else if(dy2 < 4)
			mi_MoveOrSize = (dx2 < 4) ? 6 : ((dx1 < 4) ? 8 : 7);
		else if(dx1 < 4)
			mi_MoveOrSize = (dy2 < 4) ? 8 : ((dy1 < 4) ? 2 : 9);
		else
			mi_MoveOrSize = 1;
	}
	/*$F
    else if(mo_DepthRect.PtInRect(o_Pt))
	{
		o_Rect2 = mo_DepthRect;
		ClientToScreen(o_Rect2);
        mpst_BI->z = (o_Pt.y - mo_DepthRect.bottom) / ((float) mo_DepthRect.top - mo_DepthRect.bottom);
		InvalidateRect(&mo_DepthRect);
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
		GetClipCursor(&mst_OldClipCursor);
		ClipCursor((LPRECT) & o_Rect2);
		SetCapture();
		mi_DepthMove = 1;
	}
    */

	mo_SizeAndPos.InflateRect(-2, -2);

	if(mi_MoveOrSize)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		static char *sgasz_Cursor[10] =
		{
			0,
			IDC_SIZEALL,
			IDC_SIZENWSE,
			IDC_SIZENS,
			IDC_SIZENESW,
			IDC_SIZEWE,
			IDC_SIZENWSE,
			IDC_SIZENS,
			IDC_SIZENESW,
			IDC_SIZEWE
		};
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		::SetCursor(AfxGetApp()->LoadStandardCursor(sgasz_Cursor[mi_MoveOrSize]));
		GetClipCursor(&mst_OldClipCursor);
		ClipCursor((LPRECT) & o_Rect);
		SetCapture();
		mo_PtWhenLBDown = o_Pt;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnLButtonUp(UINT , CPoint)
{
	if(mi_MoveOrSize || mi_DepthMove)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		ReleaseCapture();
		ClipCursor(&mst_OldClipCursor);
		mi_MoveOrSize = 0;
		mi_DepthMove = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnLButtonDblClk( UINT nFlags, CPoint pt )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect;
    float   w, h;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_STATIC_SIZEANDPOS)->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);

	if( o_Rect.PtInRect(pt) )
	{
        w = (float) o_Rect.Width();
        h = (float) o_Rect.Height();

        if ( (pt.y - o_Rect.top) < (h / 3) )
            mpst_BI->y = 1 - mpst_BI->h;
        else if ( (pt.y - o_Rect.top) < (2 * h / 3) )
            mpst_BI->y = 0.5f - (mpst_BI->h / 2);
        else
            mpst_BI->y = 0;

        if ( (pt.x - o_Rect.left) < (w / 3) )
            mpst_BI->x = 0;
        else if ( (pt.x - o_Rect.left) < (2 * w / 3) )
            mpst_BI->x = 0.5f - (mpst_BI->w / 2);
        else
            mpst_BI->x = 1 - ( mpst_BI->w );

        o_Rect.InflateRect(2, 2);
		InvalidateRect(&o_Rect);
        LINK_Refresh();
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnRButtonDown(UINT nFlags, CPoint o_Pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect;
	float   f;
    BOOL    b_NoEdge;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_STATIC_SIZEANDPOS)->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	o_Rect.InflateRect(2, 2);

	mo_SizeAndPos.InflateRect(2, 2);

    if( o_Rect.PtInRect(o_Pt) )
    {
        b_NoEdge = TRUE;
	    if(mo_SizeAndPos.PtInRect(o_Pt))
	    {
            b_NoEdge = FALSE;
		    if ( abs(o_Pt.x - mo_SizeAndPos.left) < 4)
            {
                f = mpst_BI->h * mpst_BI->ImageRatio;
                mpst_BI->x += mpst_BI->w - f;
                mpst_BI->w = f;
            }
            else if ( abs(o_Pt.x - mo_SizeAndPos.right) < 4 )
            {
                f = mpst_BI->h * mpst_BI->ImageRatio;
                mpst_BI->w = f;
            }
		    else if ( abs(o_Pt.y - mo_SizeAndPos.top) < 4 )
            {
                f = mpst_BI->w / mpst_BI->ImageRatio;
                mpst_BI->h = f;
            }
            else if ( abs(o_Pt.y - mo_SizeAndPos.bottom) < 4 )
            {
                f = mpst_BI->w / mpst_BI->ImageRatio;
                mpst_BI->y += mpst_BI->h - f;
                mpst_BI->h = f;
            }
            else
                b_NoEdge = TRUE;
        }

        if (b_NoEdge)
        {
        }
        else
        {
            o_Rect.InflateRect(2, 2);
		    InvalidateRect(&o_Rect);
            LINK_Refresh();
	    }

    }

	mo_SizeAndPos.InflateRect(-2, -2);

}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect;
	CDC		*pDC;
	int		x1, x2, y1, y2;
	CPen	o_RedPen, o_BluePen, o_BlackPen;
	void	*p_OldPen;
	CRgn	o_Region;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnPaint();

	pDC = GetDC();
	o_RedPen.CreatePen(PS_SOLID, 1, RGB(0xFF, 0, 0));
	o_BluePen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0xFF));
    o_BlackPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	RectBI2W();

	GetDlgItem(IDC_STATIC_SIZEANDPOS)->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);

    p_OldPen = pDC->SelectObject(o_BlackPen);
    pDC->MoveTo(o_Rect.left, o_Rect.top);
	pDC->LineTo(o_Rect.right, o_Rect.top);
	pDC->LineTo(o_Rect.right, o_Rect.bottom );
	pDC->LineTo(o_Rect.left, o_Rect.bottom);
	pDC->LineTo(o_Rect.left, o_Rect.top);

    o_Rect.InflateRect(2, 2);
	o_Region.CreateRectRgn(o_Rect.left, o_Rect.top, o_Rect.right, o_Rect.bottom);
	pDC->SelectClipRgn(&o_Region);

	x1 = mo_SizeAndPos.left;
	y1 = mo_SizeAndPos.bottom;
	x2 = mo_SizeAndPos.right;
	y2 = mo_SizeAndPos.top;

	pDC->SelectObject(o_BluePen);
	pDC->MoveTo(x1, y1);
	pDC->LineTo(x2, y1);
	pDC->LineTo(x2, y2);
	pDC->LineTo(x1, y2);
	pDC->LineTo(x1, y1);

	pDC->SelectObject(o_RedPen);
	pDC->MoveTo(x1, y1 - 3);
	pDC->LineTo(x1, y1);
	pDC->LineTo(x1 + 3, y1);
	pDC->MoveTo(x1, y2 + 3);
	pDC->LineTo(x1, y2);
	pDC->LineTo(x1 + 3, y2);
	pDC->MoveTo(x2, y1 - 3);
	pDC->LineTo(x2, y1);
	pDC->LineTo(x2 - 3, y1);
	pDC->MoveTo(x2, y2 + 3);
	pDC->LineTo(x2, y2);
	pDC->LineTo(x2 - 3, y2);

	pDC->SelectClipRgn(NULL);

	/*$F 
    o_Rect.DeflateRect(-20, 2);
	o_Rect.right = o_Rect.left + 16;
	mo_DepthRect = o_Rect;

	mo_DepthPoint.x = o_Rect.CenterPoint().x;
    mo_DepthPoint.y = o_Rect.bottom + (int) ((o_Rect.top - o_Rect.bottom) * (mpst_BI->z));

	pDC->SelectObject(o_BluePen);
	pDC->MoveTo(o_Rect.left, o_Rect.top);
	pDC->LineTo(mo_DepthPoint.x, o_Rect.bottom);
	pDC->LineTo(o_Rect.right, o_Rect.top);
	pDC->LineTo(o_Rect.left, o_Rect.top);

	pDC->SelectObject(o_RedPen);
	pDC->MoveTo(o_Rect.left, mo_DepthPoint.y);
	pDC->LineTo(o_Rect.right, mo_DepthPoint.y);
    */

	pDC->SelectObject(p_OldPen);
	DeleteObject(o_RedPen);
	DeleteObject(o_BluePen);
    DeleteObject(o_BlackPen);
	ReleaseDC(pDC);
	o_Region.DeleteObject();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnSaveClick(void)
{
    GetDlgItem(IDC_EDIT_NAME)->GetWindowText( mpst_BI->sz_Name, 63 );
    OnUpdateDataClick();
    SOFT_BackgroundImage_Save( mpst_BI );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnLoadClick(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDIA_cl_FileDialog	o_File("choose bid file", 3, TRUE, TRUE, NULL);
    CString				o_Temp;
    char				*psz_Ext;
    ULONG				ul_Index;
    CRect               o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_Temp = CString(EDI_Csz_Path_DisplayData) + CString("/BackgroundImageData");
    if(o_File.DoModal() == IDOK)
    {
        o_File.GetItem(o_File.mo_File, 1, o_Temp);
        if(BIG_b_CheckName((char *) (LPCTSTR) o_Temp) == FALSE) return;
        psz_Ext = strrchr((char *) (LPCTSTR) o_Temp, '.');
		if(!psz_Ext) return;
		if (L_stricmp(psz_Ext + 1, "bid") ) return;
        ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCTSTR) o_Temp);
		if(ul_Index == BIG_C_InvalidIndex) return;

        SOFT_BackgroundImage_Load( mpst_BI, BIG_FileKey(ul_Index));
        
        GetDlgItem( IDC_EDIT_NAME )->SetWindowText( mpst_BI->sz_Name);
        L_memcpy(mpst_LocalBI, mpst_BI, sizeof(SOFT_tdst_BackgroundImage));
        
        ((CButton *) GetDlgItem(IDC_RADIO_MPEG))->SetCheck( (mpst_BI->ul_Flags & SOFT_C_BI_Mpeg) ? 1 : 0 );
        ((CButton *) GetDlgItem(IDC_RADIO_PICTURELIST))->SetCheck( (mpst_BI->ul_Flags & SOFT_C_BI_Mpeg) ? 0 : 1 );
	    
        UpdateFreq( 0 );
        UpdateData();
        
        GetDlgItem(IDC_STATIC_SIZEANDPOS)->GetWindowRect(&o_Rect);
		ScreenToClient(&o_Rect);
		o_Rect.InflateRect(2, 2);
		InvalidateRect(&o_Rect);
        /*$F
        InvalidateRect(&mo_DepthRect);
        */
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnUpdateDataClick(void)
{
	/*~~~~~~~~~~~~~*/
	BOOL	b_Reload;
	/*~~~~~~~~~~~~~*/

	b_Reload = FALSE;

    if ( (mpst_LocalBI->ul_Flags & SOFT_C_BI_NotInBF) ^ (mpst_BI->ul_Flags & SOFT_C_BI_NotInBF) )
    {
        b_Reload = TRUE;
        mpst_BI->ul_Flags &= ~SOFT_C_BI_NotInBF;
        mpst_BI->ul_Flags |= mpst_LocalBI->ul_Flags & SOFT_C_BI_NotInBF;
    }
    
	if(((CButton *) GetDlgItem(IDC_RADIO_PICTURELIST))->GetCheck())
	{
        if (mpst_BI->ul_Flags & SOFT_C_BI_Mpeg)
        {
            b_Reload = TRUE;
		    mpst_BI->ul_Flags &= ~SOFT_C_BI_Mpeg;
        }
		
        if(mpst_BI->ul_Flags & SOFT_C_BI_NotInBF)
        {
            if (strcmp( mpst_BI->sz_PLFile, mpst_LocalBI->sz_PLFile) )
            {
                b_Reload = TRUE;
                strcpy( mpst_BI->sz_PLFile, mpst_LocalBI->sz_PLFile);
            }

            if (mpst_BI->i_FirstIndex != mpst_LocalBI->i_FirstIndex)
            {
                b_Reload = TRUE;
                mpst_BI->i_FirstIndex = mpst_LocalBI->i_FirstIndex;
            }

            if (mpst_BI->i_LastIndex != mpst_LocalBI->i_LastIndex)
            {
                b_Reload = TRUE;
                mpst_BI->i_LastIndex = mpst_LocalBI->i_LastIndex;
            }

        }
        else
        {
            if(mpst_BI->ul_PLKey != mpst_LocalBI->ul_PLKey)
		    {
			    b_Reload = TRUE;
			    mpst_BI->ul_PLKey = mpst_LocalBI->ul_PLKey;
		    }

		    if(mpst_BI->ul_PLLastKey != mpst_LocalBI->ul_PLLastKey)
		    {
			    b_Reload = TRUE;
			    mpst_BI->ul_PLLastKey = mpst_LocalBI->ul_PLLastKey;
		    }
        }
	}
	else
	{
        if ((mpst_BI->ul_Flags & SOFT_C_BI_Mpeg) == 0)
        {
            b_Reload = TRUE;
		    mpst_BI->ul_Flags |= SOFT_C_BI_Mpeg;
        }

		if(mpst_BI->ul_Flags & SOFT_C_BI_NotInBF)
		{
			if(strcmp(mpst_BI->sz_MpegFile, mpst_LocalBI->sz_MpegFile))
			{
				b_Reload = TRUE;
				strcpy(mpst_BI->sz_MpegFile, mpst_LocalBI->sz_MpegFile);
			}
		}
		else
		{
			if(mpst_BI->ul_MpegKey != mpst_LocalBI->ul_MpegKey)
			{
				b_Reload = TRUE;
				mpst_BI->ul_MpegKey = mpst_LocalBI->ul_MpegKey;
			}
		}
	}

	if(b_Reload) mpst_BI->ul_Flags |= SOFT_C_BI_Reload;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnFirstPictureClick(void)
{
	/*~~~~~~~~~~~*/
	ULONG	ul_Key;
    char    sz_File[260];
    char    *psz_Ext, *psz_Number;
    int     i_Digit;
	/*~~~~~~~~~~~*/

	ul_Key = ul_GetPicture("Choose first picture", sz_File);
    if ( ul_Key == BIG_C_InvalidKey) return;
    if (mpst_LocalBI->ul_Flags & SOFT_C_BI_NotInBF)
    {
        psz_Ext = strrchr( sz_File, '.' );
        if (!psz_Ext) return;
        psz_Number = psz_Ext - 1;
        while ( isdigit( *(--psz_Number) ) );
        psz_Number++;
        if (psz_Number == psz_Ext) return;
        i_Digit = (psz_Ext - psz_Number);
        strncpy( mpst_LocalBI->sz_PLFile, sz_File, psz_Number - sz_File );
        mpst_LocalBI->sz_PLFile[psz_Number - sz_File + 0] = '%';
        mpst_LocalBI->sz_PLFile[psz_Number - sz_File + 1] = '0';
        mpst_LocalBI->sz_PLFile[psz_Number - sz_File + 2] = '0' + i_Digit;
        mpst_LocalBI->sz_PLFile[psz_Number - sz_File + 3] = 'd';
        strcpy( &mpst_LocalBI->sz_PLFile[psz_Number - sz_File + 4], psz_Ext);
        *psz_Ext = 0;
        mpst_LocalBI->i_FirstIndex = mpst_LocalBI->i_LastIndex = atoi( psz_Number );
    }
    else
    {
	    mpst_LocalBI->ul_PLKey = ul_Key;
    }
	UpdateData();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnLastPictureClick(void)
{
	/*~~~~~~~~~~~*/
	ULONG	ul_Key;
    char    sz_File[260];
    char    sz_Template[260];
    char    *psz_Ext, *psz_Number;
    int     i_Digit;
	/*~~~~~~~~~~~*/

	ul_Key = ul_GetPicture("Choose last picture", sz_File);
    if ( ul_Key == BIG_C_InvalidKey) return;
    if (mpst_LocalBI->ul_Flags & SOFT_C_BI_NotInBF)
    {
        psz_Ext = strrchr( sz_File, '.' );
        if (!psz_Ext) return;
        psz_Number = psz_Ext - 1;
        while ( isdigit( *(--psz_Number) ) );
        psz_Number++;
        if (psz_Number == psz_Ext) return;
        i_Digit = (psz_Ext - psz_Number);
        strncpy( sz_Template, sz_File, psz_Number - sz_File );
        sz_Template[psz_Number - sz_File + 0] = '%';
        sz_Template[psz_Number - sz_File + 1] = '0';
        sz_Template[psz_Number - sz_File + 2] = '0' + i_Digit;
        sz_Template[psz_Number - sz_File + 3] = 'd';
        strcpy( &sz_Template[psz_Number - sz_File + 4], psz_Ext);
        if (L_stricmp( sz_Template, mpst_LocalBI->sz_PLFile)) return;
        
        *psz_Ext = 0;
        mpst_LocalBI->i_LastIndex = atoi( psz_Number );
        if (mpst_LocalBI->i_LastIndex < mpst_LocalBI->i_FirstIndex)
            mpst_LocalBI->i_LastIndex = mpst_LocalBI->i_FirstIndex;
    }
    else
    {
	    mpst_LocalBI->ul_PLLastKey = ul_Key;
    }
	UpdateData();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnMpegClick(void)
{
	/*~~~~~~~~~~~*/
	ULONG	ul_Key;
	/*~~~~~~~~~~~*/

	ul_Key = ul_GetPicture("Choose mpeg file", mpst_LocalBI->sz_MpegFile );
    if ( ul_Key == BIG_C_InvalidKey) return;
    if ( !(mpst_LocalBI->ul_Flags & SOFT_C_BI_NotInBF) )
    	mpst_LocalBI->ul_MpegKey = ul_Key;
	UpdateData();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnRadioMpeg(void)
{
    ((CButton *) GetDlgItem( IDC_RADIO_PICTURELIST ))->SetCheck( ((CButton *) GetDlgItem( IDC_RADIO_MPEG ))->GetCheck() ? 0 : 1 );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnRadioPictureList(void)
{
    ((CButton *) GetDlgItem( IDC_RADIO_MPEG ))->SetCheck( ((CButton *) GetDlgItem( IDC_RADIO_PICTURELIST ))->GetCheck() ? 0 : 1 );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnVisibleClick(void)
{
	if(((CButton *) GetDlgItem(IDC_CHECK_VISIBLE))->GetCheck())
		mpst_BI->ul_Flags |= SOFT_C_BI_Visible;
	else
		mpst_BI->ul_Flags &= ~SOFT_C_BI_Visible;

    LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnFreezeClick(void)
{
	if(((CButton *) GetDlgItem(IDC_CHECK_FREEZE))->GetCheck())
		mpst_BI->ul_Flags |= SOFT_C_BI_Freeze;
	else
		mpst_BI->ul_Flags &= ~SOFT_C_BI_Freeze;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnMpegBWClick(void)
{
	if(((CButton *) GetDlgItem(IDC_CHECK_MPEGWB))->GetCheck())
		mpst_BI->ul_Flags |= SOFT_C_BI_MPEG_BlackAndWhite;
	else
		mpst_BI->ul_Flags &= ~SOFT_C_BI_MPEG_BlackAndWhite;

    LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnFilmFreqChange( void )
{
    char sz_Value[32];

    GetDlgItem( IDC_COMBO_FILMFREQ )->GetWindowText( sz_Value, 31 );
    mpst_BI->w_FilmFreq = atoi( sz_Value );
    if (mpst_BI->w_FilmFreq == 0)
        mpst_BI->w_FilmFreq = 60;
    UpdateFreq( 0 );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnFilmFreqSelChange( void )
{
    int i;

    i = ((CComboBox *) GetDlgItem( IDC_COMBO_FILMFREQ ))->GetCurSel();
    mpst_BI->w_FilmFreq = (short) ((CComboBox *) GetDlgItem( IDC_COMBO_FILMFREQ ))->GetItemData( i );
    UpdateFreq( 0 );
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnAnimFreqChange( void )
{
    char sz_Value[32];

    GetDlgItem( IDC_COMBO_ANIMFREQ )->GetWindowText( sz_Value, 31 );
    mpst_BI->w_AnimFreq = atoi( sz_Value );
    if (mpst_BI->w_AnimFreq == 0)
        mpst_BI->w_AnimFreq = 60;
    UpdateFreq( 0 );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnAnimFreqSelChange( void )
{
    int i;

    i = ((CComboBox *) GetDlgItem( IDC_COMBO_ANIMFREQ ))->GetCurSel();
    mpst_BI->w_AnimFreq = (short) ((CComboBox *) GetDlgItem( IDC_COMBO_ANIMFREQ ))->GetItemData( i );
    UpdateFreq( 0 );
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnOutOfBFClick(void)
{
	if(((CButton *) GetDlgItem(IDC_CHECK_OUTOFBF))->GetCheck())
		mpst_LocalBI->ul_Flags |= SOFT_C_BI_NotInBF;
	else
		mpst_LocalBI->ul_Flags &= ~SOFT_C_BI_NotInBF;
	UpdateData();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnEditDepth()
{
    char sz_Value[32];

    GetDlgItem( IDC_EDIT_DEPTH )->GetWindowText( sz_Value, 31 ); 
    mpst_BI->depth = (float) atof( sz_Value );
    LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BackgroundImage::OnUpdateDepth()
{
    char sz_Value[32];
    GetDlgItem( IDC_EDIT_DEPTH )->GetWindowText( sz_Value, 31 ); 
    mpst_BI->depth = (float) atof( sz_Value );
    sprintf( sz_Value, "%.4f", mpst_BI->depth );
    GetDlgItem( IDC_EDIT_DEPTH )->SetWindowText( sz_Value ); 
    LINK_Refresh();
}


#endif /* ACTIVE_EDITORS */
