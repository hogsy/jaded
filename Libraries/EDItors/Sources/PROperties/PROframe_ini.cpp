/*$T PROframe_ini.cpp GC!1.26 04/22/99 14:39:36 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "PROframe.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
void EPRO_cl_Frame::ReinitIni(void)
{
    EDI_cl_BaseFrame::ReinitIni();
    mst_BaseIni.b_EngineRefresh = TRUE;
    mst_Ini.i_CxCol1 = 200;
    mst_Ini.i_CxCol2 = 100;
	mst_Ini.i_DspGroup = 0xFFFFFFFF;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EPRO_cl_Frame::LoadIni(void)
{
_Try_
    EDI_cl_BaseFrame::BaseLoadIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
    TreatIni();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EPRO_cl_Frame::TreatIni(void)
{
    mpo_VarsView->SetColWidth(mst_Ini.i_CxCol1, mst_Ini.i_CxCol2);
	mpo_VarsView->mpo_ListBox->mi_DisplayGroup = mst_Ini.i_DspGroup;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EPRO_cl_Frame::SaveIni(void)
{
    mpo_VarsView->GetColWidth(mst_Ini.i_CxCol1, mst_Ini.i_CxCol2);
	mst_Ini.i_DspGroup = mpo_VarsView->mpo_ListBox->mi_DisplayGroup;

_Try_
    EDI_cl_BaseFrame::BaseSaveIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
}

#endif /* ACTIVE_EDITORS */
