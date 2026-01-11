/*$T DIACOLOR_dlg.h GC!1.52 12/02/99 14:10:24 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "BASe/BAStypes.h"


class EDIA_cl_UPDATEDialog : public EDIA_cl_BaseDialog
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDIA_cl_UPDATEDialog
    (
        char            *_psz_Title
    );
    ~EDIA_cl_UPDATEDialog(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */
#define DIA_UPDT_MAX_TEXT_LEN 256
public:
    char	        uc_Text[DIA_UPDT_MAX_TEXT_LEN];
	ULONG	ul_TextLenght;
	ULONG	ul_AccesCounter;
	float			CurrentPos;
	float			LastPos;
	ULONG			FirstTime;
	ULONG			RealFirstTime;
	ULONG			LastSeconds;
	BOOL			bIsCanceled;
/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    BOOL    OnInitDialog(void);
	BOOL	OnRefreshBarText(float Advance,char *Text);
	BOOL	OnRefreshBar(float Advance);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg void    OnPaint(void);
	afx_msg BOOL	OnEraseBkgnd(CDC *);
	afx_msg void OnButton_Cancel(void);
    DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
