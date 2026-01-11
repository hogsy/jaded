/*$T PROdefs.c GC!1.68 01/04/00 12:36:10 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "PROdefs.h"

#ifdef RASTERS_ON
#include "TIMer/TIMdefs.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

PRO_tdst_TrameRaster    *_PRO_gpst_FirstTrameRaster = NULL;

/*$4
 ***********************************************************************************************************************
    PUBLIC PART
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    To first initialise a raster. Set name and category.
 =======================================================================================================================
 */
void PRO_FirstInitTrameRaster
(
    PRO_tdst_TrameRaster    *_pst_Raster,
    char                    *_psz_Category,
    char                    *_psz_SubCategory,
    char                    *_psz_Name,
    PRO_tde_Type            _e_Type,
    ULONG                   _ul_UserData
)
{
    L_memset(_pst_Raster, 0, sizeof(PRO_tdst_TrameRaster));
    _pst_Raster->e_Type = _e_Type;
    _pst_Raster->psz_Category = _psz_Category;
    _pst_Raster->psz_SubCategory = _psz_SubCategory;
    _pst_Raster->psz_Name = _psz_Name;
    _pst_Raster->ul_UserData = _ul_UserData;

    /* Link to list */
    if(_PRO_gpst_FirstTrameRaster == NULL)
    {
        _PRO_gpst_FirstTrameRaster = _pst_Raster;
        _pst_Raster->pst_NextRaster = NULL;
        _pst_Raster->pst_PrevRaster = NULL;
    }
    else
    {
        _PRO_gpst_FirstTrameRaster->pst_PrevRaster = _pst_Raster;
        _pst_Raster->pst_NextRaster = _PRO_gpst_FirstTrameRaster;
        _pst_Raster->pst_PrevRaster = NULL;
        _PRO_gpst_FirstTrameRaster = _pst_Raster;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PRO_KillRaster(PRO_tdst_TrameRaster *_pst_Raster)
{
	/* Already killed ? */
	if(!_pst_Raster->pst_PrevRaster && !_pst_Raster->pst_NextRaster) return;

    /* Unlink raster of list */
    if(_pst_Raster->pst_PrevRaster)
        _pst_Raster->pst_PrevRaster->pst_NextRaster = _pst_Raster->pst_NextRaster;
    else
        _PRO_gpst_FirstTrameRaster = _pst_Raster->pst_NextRaster;
    if(_pst_Raster->pst_NextRaster) _pst_Raster->pst_NextRaster->pst_PrevRaster = _pst_Raster->pst_PrevRaster;

	_pst_Raster->pst_PrevRaster = _pst_Raster->pst_NextRaster = NULL;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PRO_ForceDisplayRasters(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    register PRO_tdst_TrameRaster   *pst_Current;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Current = _PRO_gpst_FirstTrameRaster;
    while(pst_Current)
    {
        pst_Current->b_NeedDisplay = TRUE;
        pst_Current = pst_Current->pst_NextRaster;
    }
}

#endif /* RASTERS_ON */
