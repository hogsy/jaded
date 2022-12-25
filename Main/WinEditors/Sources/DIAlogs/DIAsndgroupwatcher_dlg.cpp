/*$T DIAsndgroupwatcher_dlg.cpp GC 1.138 07/29/05 18:09:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAsndgroupwatcher_dlg.h"
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
#include "Sound/Sources/SNDvolume.h"

#include "BASE/MEMory/MEM.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_SndGrpWatcher, EDIA_cl_BaseDialog)
	ON_BN_CLICKED(IDC_BTN_NONE0, OnButtonNone)
	ON_BN_CLICKED(IDC_BTN_ALL0, OnButtonAll)
	ON_BN_CLICKED(IDC_BTN_INV0, OnButtonInv)
	ON_BN_CLICKED(IDC_OK, OnButtonOk)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnChangeGrp)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

UINT	SND_gaui_GrpWatchCheckId[] =
{
	IDC_CHECK100,
	IDC_CHECK101,
	IDC_CHECK102,
	IDC_CHECK103,
	IDC_CHECK104,
	IDC_CHECK105,
	IDC_CHECK106,
	IDC_CHECK107,
	IDC_CHECK108,
	IDC_CHECK109,
	IDC_CHECK110,
	IDC_CHECK111,
	IDC_CHECK112,
	IDC_CHECK113,
	IDC_CHECK114,
	IDC_CHECK115,
	IDC_CHECK116,
	IDC_CHECK117,
	IDC_CHECK118,
	IDC_CHECK119,
	IDC_CHECK120,
	IDC_CHECK121,
	IDC_CHECK122,
	IDC_CHECK123,
	IDC_CHECK124,
	IDC_CHECK125,
	IDC_CHECK126,
	IDC_CHECK127,
	IDC_CHECK128,
	IDC_CHECK129,
	IDC_CHECK130,
	IDC_CHECK131,
	-1
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SndGrpWatcher::EDIA_cl_SndGrpWatcher(void) :
	EDIA_cl_BaseDialog(ESON_IDD_GRPWATCH)
{
	mi_ReturnOk = 0;
	mi_CurrentGrp = 0;
	L_memset(mai_CheckState, 0, SND_e_GrpNumber * 32 * sizeof(int));
	L_memset(mai_CheckVol, 0, 32*sizeof(float));
	L_memset(mai_AiWatch, 0, 32*sizeof(float));


#ifdef _DEBUG
	for(int i = 0; i < SND_e_GrpNumber; i++)
	{
		for(int j = 0; j < 32; j++) mai_CheckState[i][j] = (SND_gdst_Group[i].ul_WatchRq & (1 << j)) ? 1 : 0;
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SndGrpWatcher::~EDIA_cl_SndGrpWatcher(void)
{
#ifdef _DEBUG
	if(mi_ReturnOk)
	{
		for(int i = 0; i < SND_e_GrpNumber; i++)
		{
			for(int j = 0; j < 32; j++) 
			{
				if(mai_CheckState[i][j])
					SND_gdst_Group[i].ul_WatchRq |= (1 << j);
				else
					SND_gdst_Group[i].ul_WatchRq &= ~(1 << j);
			}

			if(mai_CheckVol[i])
				SND_gdst_Group[i].b_VolWatch = TRUE;
			else
				SND_gdst_Group[i].b_VolWatch = FALSE;
		}
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndGrpWatcher::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);

	for(int i = 0; i < 32; i++)
	{
		DDX_Check(pDX, SND_gaui_GrpWatchCheckId[i], mai_CheckState[mi_CurrentGrp][i]);
	}
	DDX_Check(pDX, IDC_CHECKVOL, mai_CheckVol[mi_CurrentGrp]);
	DDX_Check(pDX, IDC_CHECK2, mai_AiWatch[mi_CurrentGrp]);
	DDX_CBIndex(pDX, IDC_COMBO1, mi_CurrentGrp);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndGrpWatcher::OnButtonAll(void)
{
	UpdateData();
	for(int i = 0; i < 32; i++)
	{
		mai_CheckState[mi_CurrentGrp][i] = 1;
		((CButton *) GetDlgItem(SND_gaui_GrpWatchCheckId[i]))->SetCheck(mai_CheckState[mi_CurrentGrp][i]);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndGrpWatcher::OnButtonNone(void)
{
	UpdateData();
	for(int i = 0; i < 32; i++)
	{
		mai_CheckState[mi_CurrentGrp][i] = 0;
		((CButton *) GetDlgItem(SND_gaui_GrpWatchCheckId[i]))->SetCheck(mai_CheckState[mi_CurrentGrp][i]);
	}
}

void EDIA_cl_SndGrpWatcher::OnChangeGrp(void)
{
	UpdateData(TRUE);
	UpdateData(FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndGrpWatcher::OnButtonInv(void)
{
	UpdateData();
	for(int i = 0; i < 32; i++)
	{
		mai_CheckState[mi_CurrentGrp][i] = mai_CheckState[mi_CurrentGrp][i] ? 0 : 1;
		((CButton *) GetDlgItem(SND_gaui_GrpWatchCheckId[i]))->SetCheck(mai_CheckState[mi_CurrentGrp][i]);
	}
}

void EDIA_cl_SndGrpWatcher::OnButtonOk(void)
{
	mi_ReturnOk = 1;
	OnOK();
}
/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */
