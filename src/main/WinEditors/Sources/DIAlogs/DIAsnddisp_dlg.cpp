/*$T DIAsnddisp_dlg.cpp GC 1.138 02/22/05 09:22:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAsnddisp_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAname_dlg.h"

#include "BASe/CLIbrary/CLIstr.h"

#include "EDIpaths.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDImainframe.h"
#include "EDIstrings.h"

#include "Res/Res.h"

#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"

#include "LINKs/LINKmsg.h"
#include "LINks/LINKtoed.h"

#include "Sound/Sources/SND.h"
#include "Sound/Sources/SNDinsert.h"

#include "BASE/MEMory/MEM.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_SndDisp, EDIA_cl_BaseDialog)
	ON_BN_CLICKED(ID_REINIT, OnButtonReinit)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

UINT	gaui_CheckId[] =
{
	IDC_CHECK27,	/* SND_Cte_EdiShowIdx_MdFPlayer */
	IDC_CHECK28,	/* SND_Cte_EdiShowIdx_MdFFx */
	IDC_CHECK29,	/* SND_Cte_EdiShowIdx_MdFWac */
	IDC_CHECK30,	/* SND_Cte_EdiShowIdx_MdFVol */
	IDC_CHECK1,		/* SND_Cte_EdiShowIdx_Micro */
	IDC_CHECK31,	/* SND_Cte_EdiShowIdx_Instance */
	IDC_CHECK32,	/* SND_Cte_EdiShowIdx_OnlySelection */
	IDC_CHECK33,	/* SND_Cte_EdiShowIdx_Near */
	IDC_CHECK35,	/* SND_Cte_EdiShowIdx_Far */
	IDC_CHECK34,	/* SND_Cte_EdiShowIdx_Middle */
	IDC_CHECK36,	/* SND_Cte_EdiShowIdx_Start */
	IDC_CHECK37		/* SND_Cte_EdiShowIdx_Stop */
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SndDisp::EDIA_cl_SndDisp(BOOL *_ab_InitState) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_DISPFILTER)
{
	/*~~*/
	int i;
	/*~~*/

	if(_ab_InitState)
	{
		for(i = 0; i < SND_Cte_EdiShowIdx_Nb; i++)
		{
			mab_ShowIt[i] = _ab_InitState[i];
		}
	}
	else
	{
		SetInitState();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SndDisp::~EDIA_cl_SndDisp(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndDisp::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK27,	mab_ShowIt[SND_Cte_EdiShowIdx_MdFPlayer]);
	DDX_Check(pDX, IDC_CHECK28,	mab_ShowIt[SND_Cte_EdiShowIdx_MdFFx]);
	DDX_Check(pDX, IDC_CHECK29,	mab_ShowIt[SND_Cte_EdiShowIdx_MdFWac]);
	DDX_Check(pDX, IDC_CHECK30,	mab_ShowIt[SND_Cte_EdiShowIdx_MdFVol]);
	DDX_Check(pDX, IDC_CHECK1,	mab_ShowIt[SND_Cte_EdiShowIdx_Micro]);
	DDX_Check(pDX, IDC_CHECK31,	mab_ShowIt[SND_Cte_EdiShowIdx_Instance]);
	DDX_Check(pDX, IDC_CHECK32,	mab_ShowIt[SND_Cte_EdiShowIdx_OnlySelection]);
	DDX_Check(pDX, IDC_CHECK33,	mab_ShowIt[SND_Cte_EdiShowIdx_Near]);
	DDX_Check(pDX, IDC_CHECK35,	mab_ShowIt[SND_Cte_EdiShowIdx_Far]);
	DDX_Check(pDX, IDC_CHECK34,	mab_ShowIt[SND_Cte_EdiShowIdx_Middle]);
	DDX_Check(pDX, IDC_CHECK36,	mab_ShowIt[SND_Cte_EdiShowIdx_Start]);
	DDX_Check(pDX, IDC_CHECK37,	mab_ShowIt[SND_Cte_EdiShowIdx_Stop]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndDisp::OnButtonReinit(void)
{
	/*~~*/
	int i;
	/*~~*/

	SetInitState();

	for(i = 0; i < SND_Cte_EdiShowIdx_Nb; i++)
	{
		((CButton *) GetDlgItem(gaui_CheckId[i]))->SetCheck(mab_ShowIt[i]);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndDisp::SetInitState(void)
{
	mab_ShowIt[SND_Cte_EdiShowIdx_MdFPlayer] = TRUE;
	mab_ShowIt[SND_Cte_EdiShowIdx_MdFFx] = FALSE;
	mab_ShowIt[SND_Cte_EdiShowIdx_MdFWac] = FALSE;
	mab_ShowIt[SND_Cte_EdiShowIdx_MdFVol] = FALSE;
	mab_ShowIt[SND_Cte_EdiShowIdx_Micro] = FALSE;
	mab_ShowIt[SND_Cte_EdiShowIdx_Instance] = FALSE;
	mab_ShowIt[SND_Cte_EdiShowIdx_OnlySelection] = TRUE;
	mab_ShowIt[SND_Cte_EdiShowIdx_Near] = TRUE;
	mab_ShowIt[SND_Cte_EdiShowIdx_Far] = TRUE;
	mab_ShowIt[SND_Cte_EdiShowIdx_Middle] = FALSE;
	mab_ShowIt[SND_Cte_EdiShowIdx_Start] = FALSE;
	mab_ShowIt[SND_Cte_EdiShowIdx_Stop] = FALSE;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */
