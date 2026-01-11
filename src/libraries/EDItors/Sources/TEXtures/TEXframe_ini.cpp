/*$T TEXframe_ini.cpp GC!1.26 04/22/99 14:38:49 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/ERRors/ERRasser.h"
#include "TEXscroll.h"
#include "TEXframe.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
void ETEX_cl_Frame::ReinitIni(void)
{
    EDI_cl_BaseFrame::ReinitIni();
    mst_Ini.i_Res = 0;
    mst_Ini.l_ShowFlag = 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void ETEX_cl_Frame::LoadIni(void)
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
void ETEX_cl_Frame::TreatIni(void)
{
    mpo_ScrollView->mi_Res = mst_Ini.i_Res;
    mpo_ScrollView->mb_AlphaShow = mst_Ini.b_AlphaShow;
    mpo_ScrollView->mb_ForceRes = mst_Ini.b_ForceRes;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void ETEX_cl_Frame::SaveIni(void)
{
_Try_
    EDI_cl_BaseFrame::BaseSaveIni(mst_Def.asz_Name, (UCHAR *) &mst_Ini, sizeof(mst_Ini));
_Catch_
_End_
}

#endif /* ACTIVE_EDITORS */
