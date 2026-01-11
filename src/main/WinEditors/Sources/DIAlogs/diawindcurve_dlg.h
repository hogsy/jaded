#if !defined(AFX_DIAWINDCURVE_DLG_H__B6AF618F_3F2F_44C4_A45C_D291FC99D6DF__INCLUDED_)
#define AFX_DIAWINDCURVE_DLG_H__B6AF618F_3F2F_44C4_A45C_D291FC99D6DF__INCLUDED_

#if _MSC_VER > 1000
#endif // _MSC_VER > 1000
// diaWINDCURVE_dlg.h : header file
//

#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"

#include "Res/Res.h"

/////////////////////////////////////////////////////////////////////////////
// EDIA_cl_WINDCURVE dialog

class EDIA_cl_WindCurve : public EDIA_cl_BaseDialog
{
// Construction
public:
	EDIA_cl_WindCurve(FLOAT _fAmplitude, FLOAT _fShape, FLOAT _fFrequency, FLOAT _fSpeed, FLOAT _fMinForce, FLOAT _fMaxForce, CWnd* pParent = NULL);   // standard constructor
    // Dialog Data

    enum { IDD = DIALOGS_IDD_WINDCURVE };

    FLOAT GetFrequency() { return m_fFrequency; }
    FLOAT GetShape()     { return m_fShape; }
    FLOAT GetAmplitude() { return m_fAmplitude; }
    FLOAT GetSpeed()     { return m_fSpeed; }
    FLOAT GetMinForce()  { return m_fMinForce; }
    FLOAT GetMaxForce()  { return m_fMaxForce; }

// Implementation
protected:

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual void OnOK();
    virtual BOOL PreTranslateMessage(MSG *);

    afx_msg void OnPaint(void);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

    afx_msg void OnAmplitudeChange(void);
    afx_msg void OnFrequencyChange(void);
    afx_msg void OnShapeChange(void);
    afx_msg void OnSpeedChange(void);
    afx_msg void OnMinForceChange(void);
    afx_msg void OnMaxForceChange(void);

    void SetFrequency(FLOAT _f_Value);
    void SetShape(FLOAT _f_Value);
    void SetAmplitude(FLOAT _f_Value);
    void SetSpeed(FLOAT _f_Value);
    void SetScale(FLOAT _f_Value);
    void SetMinForce(FLOAT _f_Value);
    void SetMaxForce(FLOAT _f_Value);

	DECLARE_MESSAGE_MAP()

private:

    FLOAT m_fFrequency;
    FLOAT m_fShape;
    FLOAT m_fAmplitude;
    FLOAT m_fSpeed;
    FLOAT m_fMinForce;
    FLOAT m_fMaxForce;

    FLOAT m_fScale;
    FLOAT m_fScroll;
};

#endif // #ifdef ACTIVE_EDITORS


#endif // !defined(AFX_DIAWINDCURVE_DLG_H__B6AF618F_3F2F_44C4_A45C_D291FC99D6DF__INCLUDED_)
