// diasoundselection_dlg.cpp : implementation file
//

#include "Precomp.h"
#include "Res\res.h"
#include "diawindcurve_dlg.h"
#include "ENGine/Sources/Wind/noise.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// EDIA_cl_WindCurve dialog



EDIA_cl_WindCurve::EDIA_cl_WindCurve(FLOAT _fAmplitude, FLOAT _fShape, FLOAT _fFrequency, FLOAT _fSpeed, FLOAT _fMinForce, FLOAT _fMaxForce, CWnd* pParent /*=NULL*/)
	: EDIA_cl_BaseDialog(EDIA_cl_WindCurve::IDD), m_fAmplitude(_fAmplitude), m_fShape(_fShape),
      m_fFrequency(_fFrequency), m_fSpeed(_fSpeed), m_fMinForce(_fMinForce), m_fMaxForce(_fMaxForce)
{
}

BEGIN_MESSAGE_MAP(EDIA_cl_WindCurve, EDIA_cl_BaseDialog)
    ON_WM_PAINT()
    ON_WM_HSCROLL()
    ON_EN_KILLFOCUS(IDC_EDIT_AMPLITUDE, OnAmplitudeChange)
    ON_EN_KILLFOCUS(IDC_EDIT_SHAPE, OnShapeChange)
    ON_EN_KILLFOCUS(IDC_EDIT_FREQUENCY, OnFrequencyChange)
    ON_EN_KILLFOCUS(IDC_EDIT_SAMPLE, OnSpeedChange)
    ON_EN_KILLFOCUS(IDC_EDIT_MINFORCE, OnMinForceChange)
    ON_EN_KILLFOCUS(IDC_EDIT_MAXFORCE, OnMaxForceChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EDIA_cl_WindCurve message handlers

BOOL EDIA_cl_WindCurve::OnInitDialog() 
{
	EDIA_cl_BaseDialog::OnInitDialog();

    CSliderCtrl* pst_CoeffSlider;

    pst_CoeffSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_SHAPE));
    pst_CoeffSlider->SetRange(-100, 100);
    pst_CoeffSlider->SetPageSize(1);
    pst_CoeffSlider->SetTicFreq(1);
    pst_CoeffSlider->SetPos(0);
    //pst_CoeffSlider->SendMessage(TBM_SETTHUMBLENGTH, 12);

    pst_CoeffSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_FREQUENCY));
    pst_CoeffSlider->SetRange(0, 200);
    pst_CoeffSlider->SetPageSize(1);
    pst_CoeffSlider->SetTicFreq(1);
    pst_CoeffSlider->SetPos(0);

    pst_CoeffSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_AMPLITUDE));
    pst_CoeffSlider->SetRange(0, 100);
    pst_CoeffSlider->SetPageSize(1);
    pst_CoeffSlider->SetTicFreq(1);
    pst_CoeffSlider->SetPos(0);

    pst_CoeffSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_MINFORCE));
    pst_CoeffSlider->SetRange(0, 100);
    pst_CoeffSlider->SetPageSize(1);
    pst_CoeffSlider->SetTicFreq(1);
    pst_CoeffSlider->SetPos(0);

    pst_CoeffSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_MAXFORCE));
    pst_CoeffSlider->SetRange(0, 100);
    pst_CoeffSlider->SetPageSize(1);
    pst_CoeffSlider->SetTicFreq(1);
    pst_CoeffSlider->SetPos(0);

    pst_CoeffSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_SAMPLE));
    pst_CoeffSlider->SetRange(1, 1000);
    pst_CoeffSlider->SetPageSize(1);
    pst_CoeffSlider->SetTicFreq(1);
    pst_CoeffSlider->SetPos(1);

    pst_CoeffSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_SCALE));
    pst_CoeffSlider->SetRange(1, 60);
    pst_CoeffSlider->SetPageSize(1);
    pst_CoeffSlider->SetTicFreq(1);
    pst_CoeffSlider->SetPos(1);

    pst_CoeffSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_SCROLL));
    pst_CoeffSlider->SetRange(0, 1000);
    pst_CoeffSlider->SetPageSize(1);
    pst_CoeffSlider->SetTicFreq(1);
    pst_CoeffSlider->SetPos(0);

    m_fScale  = 1.0f;
    m_fScroll = 0.0f;

    SetFrequency(m_fFrequency);
    SetShape(m_fShape);
    SetAmplitude(m_fAmplitude);
    SetSpeed(m_fSpeed);
    SetScale(m_fScale);
    SetMinForce(m_fMinForce);
    SetMaxForce(m_fMaxForce);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void EDIA_cl_WindCurve::OnOK() 
{
	EDIA_cl_BaseDialog::OnOK();
}


void EDIA_cl_WindCurve::OnPaint(void)
{
    CPoint pt;
    ULONG i;
    FLOAT t;
    FLOAT v;
    int   iMinX, iMaxX;
    int   iMinY, iMaxY;

    EDIA_cl_BaseDialog::OnPaint();

    CDC * pDC = GetDC();

    // find the client area
    CRect rect;
    GetDlgItem(IDC_STATIC_CURVEBOX)->GetClientRect(rect);

    GetDlgItem(IDC_STATIC_CURVEBOX)->ClientToScreen(rect);
    ScreenToClient(rect);

    iMinX = rect.left;
    iMaxX = rect.right;
    iMinY = rect.top;
    iMaxY = rect.bottom;

    CPen penRed(PS_SOLID, 1, RGB(255, 0, 0));
    pDC->SelectObject(&penRed);

    pDC->MoveTo(iMinX, iMinY);
    pDC->LineTo(iMaxX, iMinY);
    pDC->MoveTo(iMinX, iMaxY);
    pDC->LineTo(iMaxX, iMaxY);

    for(t=0; t<=m_fScale; t+=1.0f)
    {
        pt.x = iMinX + (iMaxX - iMinX) * (t/m_fScale);
        pt.y = iMaxY;

        pDC->MoveTo(pt);

        pt.y = iMaxY - 5;
        pDC->LineTo(pt);
    }

    // draw with a thick blue pen
    CPen penBlue(PS_SOLID, 1, RGB(0, 0, 255));
    CPen* pOldPen = pDC->SelectObject(&penBlue);

    FLOAT fX  = m_fScale;
    FLOAT fDX = fX / (FLOAT)(iMaxX - iMinX);

    for(i=0, t=0; t<fX; t+=fDX, i++)
    {
        v = Swave((m_fScroll + m_fSpeed*t), m_fShape, m_fFrequency, m_fAmplitude);

        pt.x = iMinX + (iMaxX - iMinX) * (t/fX);
        pt.y = iMaxY + (iMinY - iMaxY) * v;

        if (i==0)
            pDC->MoveTo(pt);
        else
            pDC->LineTo(pt);
    }

    // Put back the old objects.
    pDC->SelectObject(pOldPen);
}

void EDIA_cl_WindCurve::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	EDIA_cl_BaseDialog::OnHScroll(nSBCode, nPos, pScrollBar);

    if (pScrollBar == GetDlgItem(IDC_SLIDER_SHAPE))
    {
        CSliderCtrl * pSliderCtrl = static_cast<CSliderCtrl*>(static_cast<CWnd*>(pScrollBar));
        const int iPos = pSliderCtrl->GetPos();
        m_fShape = static_cast<float>(iPos) / 100.0f;

        SetShape(m_fShape);

        Invalidate(TRUE);
    }
    else if (pScrollBar == GetDlgItem(IDC_SLIDER_FREQUENCY))
    {
        CSliderCtrl * pSliderCtrl = static_cast<CSliderCtrl*>(static_cast<CWnd*>(pScrollBar));
        const int iPos = pSliderCtrl->GetPos();
        m_fFrequency = static_cast<float>(iPos) / 100.0f;

        SetFrequency(m_fFrequency);

        Invalidate(TRUE);
    }
    else if (pScrollBar == GetDlgItem(IDC_SLIDER_AMPLITUDE))
    {
        CSliderCtrl * pSliderCtrl = static_cast<CSliderCtrl*>(static_cast<CWnd*>(pScrollBar));
        const int iPos = pSliderCtrl->GetPos();
        m_fAmplitude = static_cast<float>(iPos) / 100.0f;

        SetAmplitude(m_fAmplitude);

        Invalidate(TRUE);
    }
    else if (pScrollBar == GetDlgItem(IDC_SLIDER_SAMPLE))
    {
        CSliderCtrl * pSliderCtrl = static_cast<CSliderCtrl*>(static_cast<CWnd*>(pScrollBar));
        const int iPos = pSliderCtrl->GetPos();
        m_fSpeed = static_cast<float>(iPos) / 10.0f;

        SetSpeed(m_fSpeed);

        Invalidate(TRUE);
    }
    else if (pScrollBar == GetDlgItem(IDC_SLIDER_MINFORCE))
    {
        CSliderCtrl * pSliderCtrl = static_cast<CSliderCtrl*>(static_cast<CWnd*>(pScrollBar));
        const int iPos = pSliderCtrl->GetPos();
        m_fMinForce = static_cast<float>(iPos) / 100.0f;

        SetMinForce(m_fMinForce);

        Invalidate(TRUE);
    }
    else if (pScrollBar == GetDlgItem(IDC_SLIDER_MAXFORCE))
    {
        CSliderCtrl * pSliderCtrl = static_cast<CSliderCtrl*>(static_cast<CWnd*>(pScrollBar));
        const int iPos = pSliderCtrl->GetPos();
        m_fMaxForce = static_cast<float>(iPos) / 100.0f;

        SetMaxForce(m_fMaxForce);

        Invalidate(TRUE);
    }
    else if (pScrollBar == GetDlgItem(IDC_SLIDER_SCALE))
    {
        CSliderCtrl * pSliderCtrl = static_cast<CSliderCtrl*>(static_cast<CWnd*>(pScrollBar));
        const int iPos = pSliderCtrl->GetPos();

        SetScale(static_cast<float>(iPos));

        Invalidate(TRUE);
    }
    else if (pScrollBar == GetDlgItem(IDC_SLIDER_SCROLL))
    {
        CSliderCtrl * pSliderCtrl = static_cast<CSliderCtrl*>(static_cast<CWnd*>(pScrollBar));
        const int iPos = pSliderCtrl->GetPos();
        m_fScroll = static_cast<float>(iPos) / 10.0f;

        Invalidate(TRUE);
    }
}

void EDIA_cl_WindCurve::OnAmplitudeChange(void)
{
    FLOAT   f_Amplitude;
	CEdit	*pedit;
	CString o_Text;

	pedit = (CEdit *) GetDlgItem(IDC_EDIT_AMPLITUDE);
	pedit->GetWindowText(o_Text);

	f_Amplitude = static_cast<float>(L_atof(o_Text));

    SetAmplitude(f_Amplitude);

    Invalidate(TRUE);
}

void EDIA_cl_WindCurve::OnFrequencyChange(void)
{
    FLOAT   f_Frequency;
	CEdit	*pedit;
	CString o_Text;

	pedit = (CEdit *) GetDlgItem(IDC_EDIT_AMPLITUDE);
	pedit->GetWindowText(o_Text);

	f_Frequency = static_cast<float>(L_atof(o_Text));
    SetFrequency(f_Frequency);

    Invalidate(TRUE);
}

void EDIA_cl_WindCurve::OnShapeChange(void)
{
    FLOAT   f_Shape;
	CEdit	*pedit;
	CString o_Text;

	pedit = (CEdit *) GetDlgItem(IDC_EDIT_SHAPE);
	pedit->GetWindowText(o_Text);

	f_Shape = static_cast<float>(L_atof(o_Text));
    SetShape(f_Shape);

    Invalidate(TRUE);
}

void EDIA_cl_WindCurve::OnSpeedChange(void)
{
    FLOAT   f_Speed;
	CEdit	*pedit;
	CString o_Text;

	pedit = (CEdit *) GetDlgItem(IDC_EDIT_SAMPLE);
	pedit->GetWindowText(o_Text);

	f_Speed = static_cast<float>(L_atof(o_Text));
    SetSpeed(f_Speed);

    Invalidate(TRUE);
}

void EDIA_cl_WindCurve::OnMinForceChange(void)
{
    FLOAT   f_Force;
	CEdit	*pedit;
	CString o_Text;

	pedit = (CEdit *) GetDlgItem(IDC_EDIT_MINFORCE);
	pedit->GetWindowText(o_Text);

	f_Force = static_cast<float>(L_atof(o_Text)) / 100.0f;
    SetMinForce(f_Force);

    Invalidate(TRUE);
}

void EDIA_cl_WindCurve::OnMaxForceChange(void)
{
    FLOAT   f_Force;
	CEdit	*pedit;
	CString o_Text;

	pedit = (CEdit *) GetDlgItem(IDC_EDIT_MAXFORCE);
	pedit->GetWindowText(o_Text);

	f_Force = static_cast<float>(L_atof(o_Text)) / 100.0f;
    SetMaxForce(f_Force);

    Invalidate(TRUE);
}

void EDIA_cl_WindCurve::SetFrequency(FLOAT _f_Value)
{
    m_fFrequency = _f_Value;

	if(m_fFrequency < 0.0f)
        m_fFrequency = 0.0f;

	if(m_fFrequency > 2.0f)
        m_fFrequency = 2.0f;

    CSliderCtrl * pSliderCtrl = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_FREQUENCY);
    pSliderCtrl->SetPos((int)(m_fFrequency * 100));

    CEdit * pedit = (CEdit *) GetDlgItem(IDC_EDIT_FREQUENCY);
    char    szText[10];

    sprintf(szText, "%.2f", m_fFrequency);
    pedit->SetWindowText(szText);
}

void EDIA_cl_WindCurve::SetShape(FLOAT _f_Value)
{
    m_fShape = _f_Value;

	if(m_fShape < -1.0f)
        m_fShape = -1.0f;

	if(m_fShape > 1.0f)
        m_fShape = 1.0f;

    CSliderCtrl * pSliderCtrl = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_SHAPE);
    pSliderCtrl->SetPos((int)(m_fShape * 100));

    CEdit * pedit = (CEdit *) GetDlgItem(IDC_EDIT_SHAPE);
    char    szText[10];

    sprintf(szText, "%.2f", m_fShape);
    pedit->SetWindowText(szText);
}

void EDIA_cl_WindCurve::SetAmplitude(FLOAT _f_Value)
{
    m_fAmplitude = _f_Value;

	if(m_fAmplitude < 0.0f)
        m_fAmplitude = 0.0f;

	if(m_fAmplitude > 1.0f)
        m_fAmplitude = 1.0f;

    CSliderCtrl * pSliderCtrl = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_AMPLITUDE);
    pSliderCtrl->SetPos((int)(m_fAmplitude * 100));

    CEdit * pedit = (CEdit *) GetDlgItem(IDC_EDIT_AMPLITUDE);
    char    szText[10];

    sprintf(szText, "%.2f", m_fAmplitude);
    pedit->SetWindowText(szText);
}

void EDIA_cl_WindCurve::SetSpeed(FLOAT _f_Value)
{
    m_fSpeed = _f_Value;

    CSliderCtrl * pSliderCtrl = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_SAMPLE);
    pSliderCtrl->SetPos(m_fSpeed * 10.0f);

    CEdit * pedit = (CEdit *) GetDlgItem(IDC_EDIT_SAMPLE);
    char    szText[10];

    sprintf(szText, "%5.1f", m_fSpeed);
    pedit->SetWindowText(szText);
}

void EDIA_cl_WindCurve::SetMinForce(FLOAT _f_Value)
{
    m_fMinForce = _f_Value;

    if (m_fMinForce >= m_fMaxForce)
    {
        m_fMinForce = fMin(m_fMinForce, 0.99f);
        m_fMaxForce = fMin(m_fMinForce + 0.01f, 1.0f);

        SetMaxForce(m_fMaxForce);
    }

    CSliderCtrl * pSliderCtrl = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_MINFORCE);
    pSliderCtrl->SetPos(m_fMinForce * 100.0f);

    CEdit * pedit = (CEdit *) GetDlgItem(IDC_EDIT_MINFORCE);
    char    szText[10];

    sprintf(szText, "%3.0f", (m_fMinForce * 100.0f));
    pedit->SetWindowText(szText);
}

void EDIA_cl_WindCurve::SetMaxForce(FLOAT _f_Value)
{
    m_fMaxForce = _f_Value;

    if (m_fMinForce >= m_fMaxForce)
    {
        m_fMaxForce = fMax(m_fMaxForce, 0.01f);
        m_fMinForce = fMax(m_fMaxForce - 0.01f, 0.0f);

        SetMinForce(m_fMinForce);
    }

    CSliderCtrl * pSliderCtrl = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_MAXFORCE);
    pSliderCtrl->SetPos(m_fMaxForce * 100.0f);

    CEdit * pedit = (CEdit *) GetDlgItem(IDC_EDIT_MAXFORCE);
    char    szText[10];

    sprintf(szText, "%3.0f", (m_fMaxForce * 100.0f));
    pedit->SetWindowText(szText);
}

void EDIA_cl_WindCurve::SetScale(FLOAT _f_Value)
{
    m_fScale = _f_Value;

    char szText[128];
    sprintf(szText, "Time: %.0f sec.", m_fScale);
    GetDlgItem(IDC_STATIC_INTERVAL)->SetWindowText(szText);
}

BOOL EDIA_cl_WindCurve::PreTranslateMessage(MSG *pmsg)
{
	if(pmsg->message == WM_KEYDOWN)
	{
		switch(pmsg->wParam)
		{
		 case VK_RETURN:
            {
			    SetFocus();
                return TRUE;
            }
		}
	}

	return EDIA_cl_BaseDialog::PreTranslateMessage(pmsg);
}
