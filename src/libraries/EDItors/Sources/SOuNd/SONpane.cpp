/*$T SONpane.cpp GC! 1.081 02/07/03 18:21:55 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "SouND/sources/SND.h"
#include "SouND/sources/SNDwave.h"
#include "SouND/Sources/SNDconv_xboxadpcm.h"

#include "SONpane.h"
#include "SONframe.h"
#include "SONview.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"
#include "EDImainframe.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "EDImsg.h"
#include "EDIpaths.h"
#include "BIGfiles/LOAding/LOAdefs.h"

#include "Res/Res.h"

#ifdef JADEFUSION
extern float SND_gf_AutoVolumeOff;
#else
extern "C" float SND_gf_AutoVolumeOff;
#endif

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

#define BLUE1		0x00FDA699
#define RED			0x000000FF
#define GREEN		0x0000FF00
#define BLUE		0x00FF0000
#define ORANGE		0x000080FF
#define VIOLET		0x00FF00FF
#define DARKGREY	0x007f7f7f

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(ESON_cl_Pane, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ESON_cl_Pane::ESON_cl_Pane(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ESON_cl_Pane::~ESON_cl_Pane(void)
{
}

#define COLMDL  0x009F0000
#define COLINS  0x0000009F
#define COLPOS	0x00009F00
#define COLNOCO 0x000080FF
#define COLCOE  0x0000009F

/*
 =======================================================================================================================
 =======================================================================================================================
 */
HBRUSH ESON_cl_Pane::CtlColor(CDC *pDC, UINT nCtlColor)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static HBRUSH	hbrred = 0;
	static HBRUSH	hbrgreen = 0;
	static HBRUSH	hbrblue = 0;
	static HBRUSH	hbrblue1 = 0;
	static HBRUSH	hbrorange = 0;
	static HBRUSH	hbrviolet = 0;
    static HBRUSH   hbrmodel = 0;
    static HBRUSH   hbrins = 0;
    static HBRUSH   hbrnocheckout = 0;
    static HBRUSH   hbrcheckoutext = 0;
    static HBRUSH   hbrcheckoutme = 0;
	HBRUSH			hbr;
    char            asz_CurrentOwner[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	hbr = 0;
	if(!hbrred)
	{
		hbrred = CreateSolidBrush(RED);
		hbrgreen = CreateSolidBrush(GREEN);
		hbrblue = CreateSolidBrush(BLUE);
		hbrblue1 = CreateSolidBrush(BLUE1);
		hbrorange = CreateSolidBrush(ORANGE);
		hbrviolet = CreateSolidBrush(VIOLET);
        hbrmodel = CreateSolidBrush(COLMDL);
        hbrins = CreateSolidBrush(COLINS);
        hbrnocheckout = CreateSolidBrush(COLNOCO);
        hbrcheckoutext = CreateSolidBrush(COLCOE);
        hbrcheckoutme = CreateSolidBrush(COLPOS);
	}

	if(!SND_gst_Params.l_Available)
	{
		hbr = hbrred;
		pDC->SetBkColor(RED);
		pDC->SetTextColor(0x00FFFFFF);
	}
	else
	{
		switch(mi_NumPane)
		{
		case 0:
			if(SND_gf_AutoVolumeOff == 0.0f)
			{
				hbr = hbrred;
				pDC->SetBkColor(RED);
				pDC->SetTextColor(0x00FFFFFF);
			}
			break;
		case 1:
            if(M_MF()->mst_Ini.b_LinkControlON)
            {
                if(mpo_Frame->mst_SModifierDesc.ul_Fat != BIG_C_InvalidIndex)
                {
                    L_strcpy(asz_CurrentOwner, BIG_P4OwnerName(mpo_Frame->mst_SModifierDesc.ul_Fat));
					if(!L_strnicmp(asz_CurrentOwner, "_nobody_", 8)) *asz_CurrentOwner = 0;
                    if(*asz_CurrentOwner)
                    {
                        if(L_strcmpi(asz_CurrentOwner, M_MF()->mst_Ini.asz_CurrentUserName))
                        {
                            hbr = hbrcheckoutext;
                            pDC->SetBkColor(COLCOE);
                            pDC->SetTextColor(0x00FFFFFF);
                        }
					    else
					    {
                            hbr = hbrcheckoutme;
                            pDC->SetBkColor(COLPOS);
                            pDC->SetTextColor(0x00FFFFFF);
					    }
                    }
                    else
                    {
                        hbr = hbrnocheckout;
                        pDC->SetBkColor(COLNOCO);
                        pDC->SetTextColor(0x00FFFFFF);
                    }
                }
            }

			break;
		default: break;
		}
	}

	return hbr;
}

#ifdef JADEFUSION
extern float SND_f_RamRasterGetUsedRatio(void);
#else
extern "C" float SND_f_RamRasterGetUsedRatio(void);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Pane::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC				*pdc;
	CRect			o_Rect;
	COLORREF		col;
	static HBRUSH	hbrred = 0;
	static HBRUSH	hbrgreen = 0;
	static HBRUSH	hbrblue = 0;
	static HBRUSH	hbrblue1 = 0;
	static HBRUSH	hbrorange = 0;
	static HBRUSH	hbrviolet = 0;
	DWORD			dwPlayCursor;
	HRESULT			hr;
	int				x, y, X, Y;
	ULONG			play;
	static ULONG	ul_LastKey = 0;
    char        asz_CurrentOwner[256];
	float			f;
	CString o;
	/*~~~~~~*/

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CEdit::OnPaint();

	if(!hbrred)
	{
		hbrred = CreateSolidBrush(RED);
		hbrgreen = CreateSolidBrush(GREEN);
		hbrblue = CreateSolidBrush(BLUE);
		hbrblue1 = CreateSolidBrush(BLUE1);
		hbrorange = CreateSolidBrush(ORANGE);
		hbrviolet = CreateSolidBrush(VIOLET);
	}

	col = 0;
	if(!SND_gst_Params.l_Available)
	{
		col = RED;
	}
	else
	{
		switch(mi_NumPane)
		{
		case 1:
            if(M_MF()->mst_Ini.b_LinkControlON)
            {
                if(mpo_Frame->mst_SModifierDesc.ul_Fat != BIG_C_InvalidIndex)
                {
                    col = 0;
                    L_strcpy(asz_CurrentOwner, BIG_P4OwnerName(mpo_Frame->mst_SModifierDesc.ul_Fat));
					if(!L_strnicmp(asz_CurrentOwner, "_nobody_", 8)) *asz_CurrentOwner = 0;
                    if(*asz_CurrentOwner)
                    {
                        if(L_strcmpi(asz_CurrentOwner, M_MF()->mst_Ini.asz_CurrentUserName))
                            col = COLCOE;
					    else
                            col = COLPOS;
                    }
                    else
                        col = COLNOCO;
                    if(col) break;
                    return;
                }
            }
            return;

        case 2:
			pdc = GetDC();
			GetClientRect(&o_Rect);
			f = SND_f_RamRasterGetUsedRatio();
		    o.Format("%.1f %%", 100.0f*f);
			o_Rect.right = (int)((float)o_Rect.right * f);
			if(f < 0.3f)
			{
				pdc->SelectObject(hbrgreen);
			}
			else if(f < 0.8f)
			{
				pdc->SelectObject(hbrblue1);
			}
			else if(f < 0.9f)
			{
				pdc->SelectObject(hbrorange);
			}
			else
			{
				pdc->SelectObject(hbrred);
			}
            
            pdc->Rectangle(&o_Rect);
			pdc->SetBkMode(TRANSPARENT);
			pdc->TextOut(5,0,o);
            ReleaseDC(pdc);
			return;

		case 3:
			if(mpo_Frame->mst_SDesc.po_SoundBuffer)
			{
				pdc = GetDC();
				GetClientRect(&o_Rect);
				
					x = 0;
					y = o_Rect.Height() / 2;
					pdc->MoveTo(x, y);
					x = o_Rect.Width();
					pdc->LineTo(x, y);

					x = o_Rect.Width() / 4;
					y = o_Rect.Height() / 2 - o_Rect.Height() / 4;
					pdc->MoveTo(x, y);
					y += o_Rect.Height() / 2;
					pdc->LineTo(x, y);

					x = o_Rect.Width() / 2;
					y = o_Rect.Height() / 2 - o_Rect.Height() / 4;
					pdc->MoveTo(x, y);
					y += o_Rect.Height() / 2;
					pdc->LineTo(x, y);

					x = o_Rect.Width() / 2 + o_Rect.Width() / 4;
					y = o_Rect.Height() / 2 - o_Rect.Height() / 4;
					pdc->MoveTo(x, y);
					y += o_Rect.Height() / 2;
					pdc->LineTo(x, y);
				

				hr = IDirectSoundBuffer8_GetCurrentPosition(mpo_Frame->mst_SDesc.po_SoundBuffer, &dwPlayCursor, NULL);
				if(hr == DS_OK)
				{
					/*~~~~~~*/
					float	f;
					/*~~~~~~*/

                    if(mpo_Frame->mst_SDesc.pWave->wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
                        f = (float) dwPlayCursor / (float) SND_ui_GetDecompressedSize(mpo_Frame->mst_SDesc.pWave->ul_DataSize);
                    else
                        f = (float) dwPlayCursor / (float) mpo_Frame->mst_SDesc.pWave->ul_DataSize;
								
					f *= (float) o_Rect.Width();
					x = (int) f;
					y = 0;
					X = x + 5;
					Y = y + o_Rect.Height();
					pdc->SelectObject(hbrgreen);
					pdc->Rectangle(x, y, X, Y);

					if(mpo_Frame->mui_TimerId)
					{
						IDirectSoundBuffer8_GetStatus(mpo_Frame->mst_SDesc.po_SoundBuffer, &play);
						if(!(play & DSBSTATUS_PLAYING))
						{
							mpo_Frame->KillTimer(mpo_Frame->mui_TimerId);
							mpo_Frame->mui_TimerId = 0;
						}
					}
				}

				ReleaseDC(pdc);
			}

			return;

		default: return;
		}
	}

	pdc = GetDC();
	GetClientRect(&o_Rect);
	pdc->Draw3dRect
		(
			&o_Rect,
			M_MF()->u4_Interpol2PackedColor(col, 0x00FFFFFF, 0.5f),
			M_MF()->u4_Interpol2PackedColor(col, 0, 0.5f)
		);
	ReleaseDC(pdc);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Pane::OnMouseMove(UINT flags, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~*/
	DWORD	dwNewPosition;
	CRect	o_Rect;
	HRESULT hr;
	float	f;
	char az[64];
	/*~~~~~~~~~~~~~~~~~~*/
	
	CEdit::OnMouseMove(flags,pt);

	if(mi_NumPane!=3) return;
	if(!mpo_Frame->mst_SDesc.po_SoundBuffer) return;

	::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_SPEC2));

	GetClientRect(&o_Rect);

	if(flags & MK_LBUTTON)
	{
		f = (float) pt.x / (float) o_Rect.Width();
		f = f * (float) mpo_Frame->mst_SDesc.pWave->ul_DataSize;
		dwNewPosition = (DWORD) f;
		hr = IDirectSoundBuffer8_SetCurrentPosition(mpo_Frame->mst_SDesc.po_SoundBuffer, dwNewPosition);

		f = mpo_Frame->mst_SDesc.f_Time * (f / (float) mpo_Frame->mst_SDesc.pWave->ul_DataSize);
		sprintf(az, "%.1f sec", f);
		mpo_Frame->mpo_PaneSound->SetWindowText(az);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Pane::OnLButtonDown(UINT flags, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~*/
	DWORD	dwNewPosition;
	CRect	o_Rect;
	ULONG	play;
	HRESULT hr;
	float	f;
	/*~~~~~~~~~~~~~~~~~~*/

	CEdit::OnLButtonDown(flags,pt);

	if(mi_NumPane!=3) return;
	if(!mpo_Frame->mst_SDesc.po_SoundBuffer) return;

	GetClientRect(&o_Rect);

	if(flags & MK_LBUTTON)
	{
		IDirectSoundBuffer8_GetStatus(mpo_Frame->mst_SDesc.po_SoundBuffer, &play);
		if(!(play & DSBSTATUS_PLAYING))
		{
			f = (float) pt.x / (float) o_Rect.Width();
			f = f * (float) mpo_Frame->mst_SDesc.pWave->ul_DataSize;
			dwNewPosition = (DWORD) f;
			hr = IDirectSoundBuffer8_SetCurrentPosition(mpo_Frame->mst_SDesc.po_SoundBuffer, dwNewPosition);
			mpo_Frame->Sound_Pause();
		}
	}
}

void ESON_cl_Pane::OnLButtonUp(UINT flags, CPoint pt)
{
	CEdit::OnLButtonUp(flags,pt);

	if(mi_NumPane!=3) return;
	if(!mpo_Frame->mst_SDesc.po_SoundBuffer) return;
	mpo_Frame->mpo_PaneSound->SetWindowText("");
}

#endif /* ACTIVE_EDITORS */
