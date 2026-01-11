/*$T DIAjoystick_dlg.h GC! 1.081 06/17/00 14:41:02 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "BASe/BAStypes.h"

class	F3D_cl_View;

class EDIA_cl_Joystick : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_Joystick(void);
	~EDIA_cl_Joystick(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    UINT    mui_Timer;

	CRect	mo_JoyPos;

    int     mi_WindowWidth;
    int     mi_WindowHeight;
    int     mi_OX;
    int     mi_OY;
    float   mf_RatioX;
    float   mf_RatioY;

    int     mi_SetRect;
    int     mi_Calibrate;
    int     mi_Counter;

    LONG    mal_OldValues[2][8];
	int		mi_CurPad;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);
    void    OnOK(void);
    void    OnCancel(void);

    void    UpdateCalibrateText( void );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnPaint(void);
    afx_msg void	OnTimer(UINT);
    afx_msg void	OnButton_SetRect0(void);
    afx_msg void	OnButton_SetRect1(void);
    afx_msg void	OnButton_Calibrate(void);
	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
