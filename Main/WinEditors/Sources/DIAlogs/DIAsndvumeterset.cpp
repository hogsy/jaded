/*$T DIAsndvumeter.cpp GC! 1.081 10/22/02 14:38:20 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAsndvumeterset.h"
#include "Res/Res.h"
#include "VAVview/VAVlist.h"
#include "LINks/LINKtoed.h"
#include "EDImainframe.h"
#include "EDIeditors_infos.h"
#include "ENGine/Sources/ENGvars.h"

#include "SouND/sources/SND.h"
#include "SouND/sources/SNDwave.h"
#include "SouND/sources/SNDtrack.h"
#include "Editors/Sources/SOuNd/SONframe.h"
#include "Editors/Sources/SOuNd/SONvumeter.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_SndVumeterSetDialog, EDIA_cl_BaseDialog)
	ON_COMMAND(IDOK, OnOk)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SndVumeterSetDialog::EDIA_cl_SndVumeterSetDialog(void) :
	EDIA_cl_BaseDialog(ESON_IDD_VUMETERSET)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SndVumeterSetDialog::~EDIA_cl_SndVumeterSetDialog(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndVumeterSetDialog::OnOk(void)
{
    EDIA_cl_BaseDialog::OnOK();
}

void EDIA_cl_SndVumeterSetDialog::DoDataExchange(CDataExchange *pDX)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	asz_Temp[512];
	CString o_String;
	/*~~~~~~~~~~~~~~~~~~*/

	LINK_gb_CanRefresh = FALSE;
	CDialog::DoDataExchange(pDX);
	if(!pDX->m_bSaveAndValidate)
	{
		sprintf(asz_Temp, "%d", mi_Falloff);
		DDX_Text(pDX, ESON_IDC_FALLOFF, CString(asz_Temp));

		sprintf(asz_Temp, "%d", mi_PeakTTL);
		DDX_Text(pDX, ESON_IDC_PEAKTTL, CString(asz_Temp));

		sprintf(asz_Temp, "%d", mi_Scale);
		DDX_Text(pDX, ESON_IDC_SCALE, CString(asz_Temp));

		sprintf(asz_Temp, "%d", mi_MaxScale);
		DDX_Text(pDX, ESON_IDC_MAXSCALE, CString(asz_Temp));
	}
	else
	{
		DDX_Text(pDX, ESON_IDC_FALLOFF, o_String);
		sscanf((char *) (LPCSTR) o_String, "%d", &mi_Falloff);

		DDX_Text(pDX, ESON_IDC_PEAKTTL, o_String);
		sscanf((char *) (LPCSTR) o_String, "%d", &mi_PeakTTL);

		DDX_Text(pDX, ESON_IDC_SCALE, o_String);
		sscanf((char *) (LPCSTR) o_String, "%d", &mi_Scale);

		DDX_Text(pDX, ESON_IDC_MAXSCALE, o_String);
		sscanf((char *) (LPCSTR) o_String, "%d", &mi_MaxScale);
    }

	LINK_gb_CanRefresh = TRUE;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */

