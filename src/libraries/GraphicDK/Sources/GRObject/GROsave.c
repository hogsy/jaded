/*$T GROsave.c GC!1.39 06/29/99 13:17:41 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "GRObject/GROsave.h"
#include "GRObject/GROstruct.h"
#include "GEOmetric/GEOload.h"

/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
void GRO_Struct_SaveTable(TAB_tdst_Ptable *_pst_Table)
{
#ifdef ACTIVE_EDITORS
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   ul_Value;
    GRO_tdst_Struct **ppst_First, **ppst_Last;
    char            sz_Path[ BIG_C_MaxLenPath ];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ppst_First = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(_pst_Table);
    ppst_Last = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(_pst_Table);

    for(; ppst_First <= ppst_Last; ppst_First++)
    {
        if( TAB_b_IsAHole(*ppst_First)) continue;

        ul_Value = LOA_ul_SearchKeyWithAddress((ULONG) * ppst_First);
        if (ul_Value != BIG_C_InvalidKey)
        {
            ul_Value = BIG_ul_SearchKeyToFat( ul_Value );
            if (ul_Value != BIG_C_InvalidIndex )
            {
                BIG_ComputeFullName(BIG_ParentFile(ul_Value), sz_Path);
                SAV_Begin(sz_Path, BIG_NameFile(ul_Value));
                (*ppst_First)->i->pfnl_SaveInBuffer( (*ppst_First), &TEX_gst_GlobalList );
                SAV_ul_End();
            }
        }
    }
#endif
}
