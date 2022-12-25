/*$T COLset.c GC!1.71 02/23/00 14:26:53 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"

#include "MATHs/MATH.h"

#include "COLstruct.h"
#include "COLconst.h"
#include "COLaccess.h"

#include "GraphicDK/Sources/GEOmetric/GEOobject.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
#include "OBJects/OBJorient.h"
//#include "MATHs/MATHmatrixTransform.h"
#include "COLedit.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_ColSet_AddSphere(COL_tdst_ZDx *_pst_ZDx, COL_tdst_ZDx *_pst_Orig, MATH_tdst_Vector *_pst_Pos)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    COL_tdst_Sphere *pst_Sphere;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    _pst_ZDx->p_Shape = (COL_tdst_Sphere *) MEM_p_Alloc(sizeof(COL_tdst_Sphere));
    pst_Sphere = (COL_tdst_Sphere *) _pst_ZDx->p_Shape;
    if(_pst_Orig)
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        COL_tdst_Sphere *pst_SphOrig;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        pst_SphOrig = (COL_tdst_Sphere *) _pst_Orig->p_Shape;
        pst_Sphere->f_Radius = pst_SphOrig->f_Radius;
        if(_pst_Pos)
        {
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            MATH_tdst_Matrix    st_InvAMatrix ONLY_PSX2_ALIGNED(16);
            MATH_tdst_Vector    st_ACS_Pos;
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

            /* The pos vector is given in the GCS. We must transform it in the ACS. */
            MATH_InvertMatrix(&st_InvAMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_Orig->pst_GO));
            MATH_TransformVector(&st_ACS_Pos, &st_InvAMatrix, _pst_Pos);
            MATH_CopyVector(&pst_Sphere->st_Center, &st_ACS_Pos);
        }
        else
            MATH_CopyVector(&pst_Sphere->st_Center, &pst_SphOrig->st_Center);
        _pst_ZDx->uc_Flag = _pst_Orig->uc_Flag;
        _pst_ZDx->uc_Type = COL_C_Zone_Sphere;
    }
    else
    {
        pst_Sphere->f_Radius = 1.0f;
        if(_pst_Pos)
            MATH_CopyVector(&pst_Sphere->st_Center, _pst_Pos);
        else
            MATH_InitVector(&pst_Sphere->st_Center, 0.0f, 0.0f, 0.0f);
        COL_Zone_CopyFlag(_pst_ZDx, COL_C_Zone_ZDM);
        _pst_ZDx->uc_Type = COL_C_Zone_Sphere;
    }

#ifdef ACTIVE_EDITORS

    /*$F--------- Dummy Name -------------- */
    _pst_ZDx->sz_Name = (char *) MEM_p_Alloc(30);
    _pst_ZDx->pst_ColSetZDx = NULL;
    strcpy(_pst_ZDx->sz_Name, "Zone");
    sprintf(_pst_ZDx->sz_Name + L_strlen(_pst_ZDx->sz_Name), "@%x", (ULONG) _pst_ZDx->p_Shape);

#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_ColSet_AddBox(COL_tdst_ZDx *_pst_ZDx, COL_tdst_ZDx *_pst_Orig, MATH_tdst_Vector *_pst_Pos)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    COL_tdst_Box    *pst_Box;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    _pst_ZDx->p_Shape = (COL_tdst_Box *) MEM_p_Alloc(sizeof(COL_tdst_Box));
    pst_Box = (COL_tdst_Box *) _pst_ZDx->p_Shape;
    if(_pst_Orig)
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        COL_tdst_Box    *pst_BoxOrig;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        pst_BoxOrig = (COL_tdst_Box *) _pst_Orig->p_Shape;
        MATH_CopyVector(&pst_Box->st_Min, &pst_BoxOrig->st_Min);
        MATH_CopyVector(&pst_Box->st_Max, &pst_BoxOrig->st_Max);
    }
    else
    {
        MATH_InitVector(&pst_Box->st_Min, -1.0f, -1.0f, -1.0f);
        MATH_InitVector(&pst_Box->st_Max, 1.0f, 1.0f, 1.0f);
    }

    COL_Zone_CopyFlag(_pst_ZDx, COL_C_Zone_ZDE);
    _pst_ZDx->uc_Type = COL_C_Zone_Box;

#ifdef ACTIVE_EDITORS

    /*$F--------- Dummy Name -------------- */
    _pst_ZDx->sz_Name = (char *) MEM_p_Alloc(30);
    _pst_ZDx->pst_ColSetZDx = NULL;
    strcpy(_pst_ZDx->sz_Name, "Zone");
    sprintf(_pst_ZDx->sz_Name + L_strlen(_pst_ZDx->sz_Name), "@%x", (ULONG) _pst_ZDx->p_Shape);

#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_ColSet_AddZDx(COL_tdst_ColSet *_pst_ColSet, USHORT _uw_Type, void *_pv_Data1, void *_pv_Data2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    COL_tdst_Instance   **dpst_Instance, **dpst_LastInstance;
    COL_tdst_ZDx        *pst_CurrentZDx, *pst_LastZDx;
    COL_tdst_ZDx        st_TempZDx;
    ULONG               ul_Offset, ul_OldPosition;
    UCHAR               uc_Cpt;
    char                asz_Path[BIG_C_MaxLenPath];
    BIG_KEY             ul_Key;
    BIG_INDEX           ul_Fat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*
     * First, we check if we are not going to have more than COL_Cul_MaxNbOfZDx zones
     * in the ColSet.
     */
    if(_pst_ColSet->uc_NbOfZDx == COL_Cul_MaxNbOfZDx)
    {
        ERR_X_ForceError("Sorry, Max number of zones reached", NULL);
        return;
    }
	if(_pv_Data1)
	{
		switch(_uw_Type)
		{
		case COL_C_Zone_Sphere:
			COL_ColSet_AddSphere(&st_TempZDx, (COL_tdst_ZDx *)_pv_Data1, NULL);
			break;

		case COL_C_Zone_Box:
			COL_ColSet_AddBox(&st_TempZDx, (COL_tdst_ZDx *)_pv_Data1, NULL);
			break;
		}

		st_TempZDx.pst_GO = ((COL_tdst_ZDx *)_pv_Data1)->pst_GO;
	}

    /*
     * We deregister all the ColSet's zones cause the realloc can change the place of
     * the array in memory.
     */
    if(_pst_ColSet->uc_NbOfZDx)
    {
        pst_CurrentZDx = _pst_ColSet->past_ZDx;
        pst_LastZDx = pst_CurrentZDx + _pst_ColSet->uc_NbOfZDx;
        for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++)
            LINK_DelRegisterPointer(pst_CurrentZDx);
    }

    if(!_pst_ColSet->uc_NbOfZDx)
        _pst_ColSet->past_ZDx = (COL_tdst_ZDx*)MEM_p_Alloc(sizeof(COL_tdst_ZDx));
    else
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        COL_tdst_ZDx    **dpst_ZDx, **dpst_LastZDx;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        ul_OldPosition = (ULONG) _pst_ColSet->past_ZDx;

        /* Realloc of the structure. */
        _pst_ColSet->past_ZDx = (COL_tdst_ZDx*)MEM_p_Realloc
            (
                _pst_ColSet->past_ZDx,
                (_pst_ColSet->uc_NbOfZDx + 1) * sizeof(COL_tdst_ZDx)
            );
        ul_Offset = (ULONG) _pst_ColSet->past_ZDx - ul_OldPosition;

        /*
         * We have just performed a Realloc of the ColSet array of zones. That means that
         * all the pointers that points to thoses zones in any of its instances array of
         * pointers are obsoletes. So we compute the "offset" of the realloc in order to
         * be able to recompute the real position of the pointers to those zones in
         * memory. Moreover, we have to update the pst_ColSetZDx of the specific zones.
         */
        dpst_Instance = _pst_ColSet->dpst_Instances;
        dpst_LastInstance = dpst_Instance + _pst_ColSet->uw_NbOfInstances;
        for(; dpst_Instance < dpst_LastInstance; dpst_Instance++)
        {
            dpst_ZDx = (*dpst_Instance)->dpst_ZDx;
            dpst_LastZDx = dpst_ZDx + (*dpst_Instance)->uc_NbOfZDx;
            for(uc_Cpt = 0; dpst_ZDx < dpst_LastZDx; dpst_ZDx++, uc_Cpt++)
            {
                /* If the current ZDx is Shared */
                if(!(COL_b_Instance_IsSpecific(*dpst_Instance, uc_Cpt)))
                {
                    ul_OldPosition = (ULONG) * dpst_ZDx;
                    *(dpst_ZDx) = (COL_tdst_ZDx *) (ul_OldPosition + ul_Offset);
                }
                else
                {
                    ul_OldPosition = (ULONG) (*dpst_ZDx)->pst_ColSetZDx;
                    (*dpst_ZDx)->pst_ColSetZDx = (COL_tdst_ZDx *) (ul_OldPosition + ul_Offset);
                }
            }
        }
    }

    /* We get the last ZDx that is the one to fill. */
    pst_CurrentZDx = (_pst_ColSet->past_ZDx + _pst_ColSet->uc_NbOfZDx);
    pst_CurrentZDx->uc_BoneIndex = 0xFF;
    pst_CurrentZDx->uc_Design = 0;
    pst_CurrentZDx->pst_Itself = pst_CurrentZDx;

	pst_CurrentZDx->uc_Flag = st_TempZDx.uc_Flag;

    switch(_uw_Type)
    {
    case COL_C_Zone_Sphere:
        COL_ColSet_AddSphere(pst_CurrentZDx, (COL_tdst_ZDx *) &st_TempZDx, (MATH_tdst_Vector *) _pv_Data2);
        break;

    case COL_C_Zone_Box:
        COL_ColSet_AddBox(pst_CurrentZDx, (COL_tdst_ZDx *) &st_TempZDx, (MATH_tdst_Vector *) _pv_Data2);
        break;
    }

    if(_pst_ColSet->pauc_AI_Indexes[_pst_ColSet->uc_NbOfZDx] == 0xFF)
        pst_CurrentZDx->uc_AI_Index = _pst_ColSet->uc_NbOfZDx;
    else
        pst_CurrentZDx->uc_AI_Index = 0xFF;
#ifdef ACTIVE_EDITORS
    if(_pst_ColSet->pauc_AI_Indexes[_pst_ColSet->uc_NbOfZDx] == 0xFF)
        _pst_ColSet->pauc_AI_Indexes[_pst_ColSet->uc_NbOfZDx] = _pst_ColSet->uc_NbOfZDx;
#endif
    /*
     * We have just add a new zone in the ColSet. We must reflect this to the
     * instances.
     */
    dpst_Instance = _pst_ColSet->dpst_Instances;
    dpst_LastInstance = dpst_Instance + _pst_ColSet->uw_NbOfInstances;

    for(; dpst_Instance < dpst_LastInstance; dpst_Instance++)
    {
        /* We realloc this instance's array of pointers. */
        (*dpst_Instance)->dpst_ZDx = (COL_tdst_ZDx **) MEM_p_Realloc
            (
                (*dpst_Instance)->dpst_ZDx,
                (_pst_ColSet->uc_NbOfZDx + 1) * sizeof(COL_tdst_ZDx **)
            );
        *((*dpst_Instance)->dpst_ZDx + _pst_ColSet->uc_NbOfZDx) = pst_CurrentZDx;
        (*dpst_Instance)->uc_NbOfZDx++;
        (*dpst_Instance)->uc_NbOfShared++;
        if(pst_CurrentZDx->uc_AI_Index != 0xFF) COL_Instance_SetActivationFlag(*dpst_Instance, _pst_ColSet->uc_NbOfZDx);
    }

    /* We increase the number of this ColSet's zones. */
    _pst_ColSet->uc_NbOfZDx++;

    /*
     * Get the BigFile path to the ColSet file. Needed to register properly the
     * pointer
     */
    ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_ColSet);
    ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);

    BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);

    /* We have to re-register all the previous zones + the new one. */
    pst_CurrentZDx = _pst_ColSet->past_ZDx;
    pst_LastZDx = pst_CurrentZDx + _pst_ColSet->uc_NbOfZDx;
    for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++)
        LINK_RegisterPointer(pst_CurrentZDx, LINK_C_Zone, BIG_NameFile(ul_Fat), asz_Path);


	if(_pv_Data1)
	{
		MEM_Free(st_TempZDx.p_Shape);
		MEM_Free(st_TempZDx.sz_Name);
	}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_ColSet_RemoveZDxWithIndex(COL_tdst_ColSet *_pst_ColSet, UCHAR _uc_Index)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    COL_tdst_Instance   *pst_Instance;
    COL_tdst_Instance   **dpst_Instance, **dpst_LastInstance;
    COL_tdst_ZDx        *pst_ZDx, *pst_CurrentZDx, *pst_LastZDx;
    UCHAR               uc_Specific, uc_Cpt;
    ULONG               ul_OldPosition;
    char                asz_Path[BIG_C_MaxLenPath];
    BIG_KEY             ul_Key;
    BIG_INDEX           ul_Fat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Gets the ZDx... */
    pst_ZDx = COL_pst_ColSet_GetZoneWithIndex(_pst_ColSet, _uc_Index);

    /*
     * We have to unregister all the ColSet zones because the memcpy will move those
     * zone pointers in memory.
     */
    pst_CurrentZDx = _pst_ColSet->past_ZDx;
    pst_LastZDx = pst_CurrentZDx + _pst_ColSet->uc_NbOfZDx;
    for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++)
        LINK_DelRegisterPointer(pst_CurrentZDx);

    /* Zone's Shape desallocation. */      
    MEM_Free((_pst_ColSet->past_ZDx + _uc_Index)->p_Shape);

	/* Zone's Name desallocation */
	if((_pst_ColSet->past_ZDx + _uc_Index)->sz_Name)
		MEM_Free((_pst_ColSet->past_ZDx + _uc_Index)->sz_Name);


    /* We move the following specific zones. */
    L_memcpy
    (
        _pst_ColSet->past_ZDx + _uc_Index,
        _pst_ColSet->past_ZDx + _uc_Index + 1,
        (_pst_ColSet->uc_NbOfZDx - _uc_Index - 1) * sizeof(COL_tdst_ZDx)
    );

    /*
     * We have just removed one ColSet zone. We have to loop thru all the instances of
     * this ColSet to update (at least)the instances's array of pointers.
     */
    dpst_Instance = _pst_ColSet->dpst_Instances;
    dpst_LastInstance = dpst_Instance + _pst_ColSet->uw_NbOfInstances;

    for(; dpst_Instance < dpst_LastInstance; dpst_Instance++)
    {
        pst_Instance = *dpst_Instance;

        /*
         * If the the removed ColSet zone was specific in the instance, we have to
         * unregister all the specific zones, realloc the specific array, update the
         * instance's specific BitField and register the zones.
         */
        if(COL_b_Instance_IsSpecific(pst_Instance, _uc_Index))
        {
            if(pst_Instance->uc_NbOfSpecific > 1)
            {
                uc_Specific = COL_uc_Instance_GetSpecificIndexWithZone
                    (
                        pst_Instance,
                        *(pst_Instance->dpst_ZDx + _uc_Index)
                    );

                /*
                 * If the instance has more than 1 specific zone, we have to deal with a memcpy.
                 */
                if(pst_Instance->uc_NbOfSpecific - uc_Specific - 1)
                {
                    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                    COL_tdst_ZDx    **dpst_ZDx, **dpst_LastZDx;
                    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

                    /*
                     * We are about to perform a memcpy in the instance's specific array of zones. We
                     * have to unregister the zones.
                     */
                    pst_CurrentZDx = pst_Instance->past_Specific;
                    pst_LastZDx = pst_CurrentZDx + pst_Instance->uc_NbOfSpecific;
                    for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++)
                        LINK_DelRegisterPointer(pst_CurrentZDx);

                    /* Memcpy */
                    L_memcpy
                    (
                        pst_Instance->past_Specific + uc_Specific,
                        pst_Instance->past_Specific + uc_Specific + 1,
                        (pst_Instance->uc_NbOfSpecific - uc_Specific - 1) * sizeof(COL_tdst_ZDx)
                    );

                    /*
                     * We have performed a memcpy in the Instance's specific array of zones. We have
                     * to loop thru all this instance's array of pointers to find the pointers that
                     * point to specific zones whose rank were "after" the deleted one to update them.
                     */
                    dpst_ZDx = pst_Instance->dpst_ZDx;
                    dpst_LastZDx = dpst_ZDx + pst_Instance->uc_NbOfZDx;
                    for(uc_Cpt = 0; dpst_ZDx < dpst_LastZDx; dpst_ZDx++, uc_Cpt++)
                    {
                        if(COL_b_Instance_IsSpecific(pst_Instance, uc_Cpt))
                        {
                            uc_Specific = COL_uc_Instance_GetSpecificIndexWithZone(pst_Instance, *dpst_ZDx);
                            if(uc_Specific < _uc_Index) continue;
                            ul_OldPosition = (ULONG) * dpst_ZDx;
                            *(dpst_ZDx) = (COL_tdst_ZDx *) (ul_OldPosition - sizeof(COL_tdst_ZDx));
                        }
                    }

                    /* Decrease this instance's number of specific zones. */
                    pst_Instance->uc_NbOfSpecific--;

                    /*
                     * We have to re-register all the previous specific zones except thed one.
                     */
                    ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Instance);
                    ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
                    BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);

                    pst_CurrentZDx = pst_Instance->past_Specific;
                    pst_LastZDx = pst_CurrentZDx + pst_Instance->uc_NbOfSpecific;
                    for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++)
                    {
                        LINK_RegisterPointer(pst_CurrentZDx, LINK_C_Zone, BIG_NameFile(ul_Fat), asz_Path);
                    }
                }
            }
            else
            {
                /* It was the last specific zone of the instance, we free the structure. */
                MEM_Free(pst_Instance->past_Specific);
                pst_Instance->uc_NbOfSpecific = 0;
            }
        }
        else
            pst_Instance->uc_NbOfShared--;

        /*
         * We removed the (_uc_Index)ieme zone of the ColSet. For each instance, we have
         * to update the pointers to SHARED ZONES that were after the deleted one.
         * Moreover, for each pointer to specific zones whose index is over the removed
         * ZDx, we have to update the pst_ColSetZDx pointer.
         */
        for(uc_Cpt = _uc_Index + 1; uc_Cpt < _pst_ColSet->uc_NbOfZDx; uc_Cpt++)
        {
            if(!(COL_b_Instance_IsSpecific(pst_Instance, uc_Cpt)))
            {
                ul_OldPosition = (ULONG) (*(pst_Instance->dpst_ZDx + uc_Cpt));
                *(pst_Instance->dpst_ZDx + uc_Cpt) = (COL_tdst_ZDx *) (ul_OldPosition - sizeof(COL_tdst_ZDx));
            }
            else
            {
                ul_OldPosition = (ULONG) (*(pst_Instance->dpst_ZDx + uc_Cpt))->pst_ColSetZDx;
                (*(pst_Instance->dpst_ZDx + uc_Cpt))->pst_ColSetZDx = (COL_tdst_ZDx *) (ul_OldPosition - sizeof(COL_tdst_ZDx));
            }
        }

        /* We update the instance's specific BitField */
        for(uc_Cpt = _uc_Index; uc_Cpt < _pst_ColSet->uc_NbOfZDx - 1; uc_Cpt++)
        {
            if(COL_b_Instance_IsSpecific(pst_Instance, (UCHAR) (uc_Cpt + 1)))
                COL_Instance_SetSpecificFlag(pst_Instance, uc_Cpt);
            else
                COL_Instance_ResetSpecificFlag(pst_Instance, uc_Cpt);
        }

        /*
         * Reset the Flag the "After last" to make a good init after another zone add.
         */
        COL_Instance_ResetSpecificFlag(pst_Instance, (UCHAR) (_pst_ColSet->uc_NbOfZDx - 1));

        /* Move the Zones */
        L_memcpy
        (
            pst_Instance->dpst_ZDx + _uc_Index,
            pst_Instance->dpst_ZDx + _uc_Index + 1,
            (_pst_ColSet->uc_NbOfZDx - _uc_Index - 1) * sizeof(COL_tdst_ZDx *)
        );

        if(_pst_ColSet->uc_NbOfZDx - 1)
        {
            /* We realloc this instance's array of pointers. */
            pst_Instance->dpst_ZDx = (COL_tdst_ZDx **) MEM_p_Realloc
                (
                    pst_Instance->dpst_ZDx,
                    (_pst_ColSet->uc_NbOfZDx - 1) * sizeof(COL_tdst_ZDx **)
                );
        }
        else
            MEM_Free(pst_Instance->dpst_ZDx);
        pst_Instance->uc_NbOfZDx--;
    }

    /* The number of zones decreases. */
    _pst_ColSet->uc_NbOfZDx--;

    /*
     * We have to re-register all the previous specific zones except the deleted one.
     */
    ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_ColSet);
    ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
    BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);

    pst_CurrentZDx = _pst_ColSet->past_ZDx;
    pst_LastZDx = pst_CurrentZDx + _pst_ColSet->uc_NbOfZDx;
    for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++)
        LINK_RegisterPointer(pst_CurrentZDx, LINK_C_Zone, BIG_NameFile(ul_Fat), asz_Path);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_ColSet_RemoveZDxWithZone(COL_tdst_ColSet *_pst_ColSet, COL_tdst_ZDx *_pst_ZDx)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    UCHAR           uc_Index;
    COL_tdst_ZDx    *pst_CurrentZDx, *pst_LastZDx;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    uc_Index = 0;

    pst_CurrentZDx = _pst_ColSet->past_ZDx;
    pst_LastZDx = _pst_ColSet->past_ZDx + _pst_ColSet->uc_NbOfZDx;

    for(; pst_CurrentZDx < pst_LastZDx; pst_CurrentZDx++, uc_Index++)
        if(pst_CurrentZDx == _pst_ZDx) break;

    if(pst_CurrentZDx == _pst_ZDx)
        COL_ColSet_RemoveZDxWithIndex(_pst_ColSet, uc_Index);
    else
        ERR_X_ForceError("Cannot remove this zone", NULL);
}

/*
 =======================================================================================================================
    Aim:    Remove a particular instance from the ColSet array of pointers to instance.
 =======================================================================================================================
 */
void COL_ColSet_RemoveInstance(COL_tdst_ColSet *_pst_ColSet, COL_tdst_Instance *_pst_Instance)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    COL_tdst_Instance   **dpst_Instance, **dpst_LastInstance;
    UCHAR               uc_Index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    dpst_Instance = _pst_ColSet->dpst_Instances;
    if(!dpst_Instance) return;
    dpst_LastInstance = dpst_Instance + _pst_ColSet->uw_NbOfInstances;
    for(uc_Index = 0; dpst_Instance < dpst_LastInstance; dpst_Instance++, uc_Index++)
    {
        if(*dpst_Instance == _pst_Instance)
        {
/*$off*/
            L_memcpy
            (
                _pst_ColSet->dpst_Instances + uc_Index,
                      _pst_ColSet->dpst_Instances + uc_Index + 1,
                (_pst_ColSet->uw_NbOfInstances - uc_Index - 1) * sizeof(COL_tdst_Instance *)
            );
            if(_pst_ColSet->uw_NbOfInstances - 1)
            {
#ifdef JADEFUSION
               _pst_ColSet->dpst_Instances = (COL_tdst_Instance_**)MEM_p_Realloc
				(
                        _pst_ColSet->dpst_Instances,
                        (_pst_ColSet->uw_NbOfInstances - 1) * sizeof(COL_tdst_Instance **)
                    );
#else
                _pst_ColSet->dpst_Instances = MEM_p_Realloc
				(
                        _pst_ColSet->dpst_Instances,
                        (_pst_ColSet->uw_NbOfInstances - 1) * sizeof(COL_tdst_Instance **)
                    );
#endif
            }
            else
                MEM_Free(_pst_ColSet->dpst_Instances);
/*$on*/
        }
    }
}

/*
 =======================================================================================================================
    Aim:    Update the ColSet array of pointers to instances that have this particular ColSet.
 =======================================================================================================================
 */
void COL_ColSet_AddInstance(COL_tdst_ColSet *_pst_ColSet, COL_tdst_Instance *_pst_Instance)
{
    if(!_pst_ColSet->uw_NbOfInstances)
    {
        _pst_ColSet->dpst_Instances = (COL_tdst_Instance **) MEM_p_Alloc(sizeof(COL_tdst_Instance *));
        *(_pst_ColSet->dpst_Instances) = _pst_Instance;
    }
    else
    {
        _pst_ColSet->dpst_Instances = (COL_tdst_Instance **) MEM_p_Realloc
            (
                _pst_ColSet->dpst_Instances,
                (_pst_ColSet->uw_NbOfInstances + 1) * sizeof(COL_tdst_Instance *)
            );
        *(_pst_ColSet->dpst_Instances + _pst_ColSet->uw_NbOfInstances) = _pst_Instance;
    }
}

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
