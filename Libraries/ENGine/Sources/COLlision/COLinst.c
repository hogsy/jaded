/*$T COLinst.c GC!1.5 11/26/99 11:25:40 */

/*$F GC Dependencies 11/22/99 */

#include "COLstruct.h"
#include "COLconst.h"
#include "COLaccess.h"
#include "COLedit.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/BAStypes.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGkey.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BASe/ERRors/ERRasser.h"

/*
 ===================================================================================================
    Aim:    This function updates the instance array of pointers to zone.
 ===================================================================================================
 */
void COL_ComputeArrayOfPointers(COL_tdst_Instance *_pst_Instance)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    COL_tdst_ColSet *pst_ColSet;
    COL_tdst_ZDx    *pst_CurrentZDx, *pst_LastZDx;
    COL_tdst_ZDx    **dpst_Instance_ZDx, **dpst_Instance_LastZDx;
    UCHAR           uc_Index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_Instance) return;

    pst_ColSet = _pst_Instance->pst_ColSet;

    /* If the instance has no ColSet and has also shared zones --> Error */
    ERR_X_Assert(!((!pst_ColSet) && _pst_Instance->uc_NbOfShared));

    /*
     * We may have just created a new instance. If it is the case, just the ColSet pointer is
     * valid in the Instance structure. We have to construct a valid instance.
     */
    if((pst_ColSet) && (!_pst_Instance->dpst_ZDx))
    {
        _pst_Instance->dpst_ZDx = (COL_tdst_ZDx **)
            MEM_p_Alloc(pst_ColSet->uc_NbOfZDx * sizeof(COL_tdst_ZDx *));
        L_memset(_pst_Instance->dpst_ZDx, 0, pst_ColSet->uc_NbOfZDx * sizeof(COL_tdst_ZDx *));
        _pst_Instance->uc_NbOfZDx = pst_ColSet->uc_NbOfZDx;
        _pst_Instance->uc_NbOfShared = pst_ColSet->uc_NbOfZDx;
    }

    /*
     * First, we go thru all the ColSet zones and put a pointer to each one in the instance
     * array.
     */
    if(pst_ColSet)
    {
        dpst_Instance_ZDx = _pst_Instance->dpst_ZDx;
        pst_CurrentZDx = pst_ColSet->past_ZDx;
        pst_LastZDx = pst_CurrentZDx + pst_ColSet->uc_NbOfZDx;

        for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++)
        {
            *dpst_Instance_ZDx = pst_CurrentZDx;
            dpst_Instance_ZDx++;
        }
    }

    /*
     * All the zone in the instance may not be the ColSet ones. For each zone, we check the
     * Specific flag and if we find one specific zone, we update the pointer.
     */
    if(_pst_Instance->past_Specific)
    {
        pst_CurrentZDx = _pst_Instance->past_Specific;
        pst_LastZDx = pst_CurrentZDx + _pst_Instance->uc_NbOfSpecific;

        dpst_Instance_ZDx = _pst_Instance->dpst_ZDx;
        dpst_Instance_LastZDx = _pst_Instance->dpst_ZDx + _pst_Instance->uc_NbOfZDx;

        for
        (
            uc_Index = 0;
            dpst_Instance_ZDx < dpst_Instance_LastZDx; dpst_Instance_ZDx++, uc_Index++
        )
        {
            if(COL_b_Instance_IsSpecific(_pst_Instance, uc_Index))
                *dpst_Instance_ZDx = pst_CurrentZDx++;
            else
                continue;
        }
    }
}

