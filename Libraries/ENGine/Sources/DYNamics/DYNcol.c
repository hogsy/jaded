/*$T DYNcol.c GC!1.52 01/05/00 11:45:39 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Handles the functions related to solid objects
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "ENGine/Sources/DYNamics/DYNinit.h"
#include "ENGine/Sources/DYNamics/DYNaccess.h"
#include "ENGine/Sources/DYNamics/DYNcol.h"
#include "MATHs/MATH.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

/*
 ===================================================================================================
    Aim:    Create the col structure
 ===================================================================================================
 */
DYN_tdst_Col *DYN_pst_ColCreate(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    DYN_tdst_Col    *pst_Col;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Col = (DYN_tdst_Col *) MEM_p_Alloc(sizeof(DYN_tdst_Col));

    /* L_memset(pst_Col, 0, sizeof(DYN_tdst_Col)); */
    return(pst_Col);
}

/*
 ===================================================================================================
    Aim:    Init the col structure
 ===================================================================================================
 */
void    DYN_ColInit
        (
            DYN_tdst_Col    *_pst_Col,
            float           _f_Rebound,
            float           _f_Slide,
            float           _f_ReboundHorizCosAngle,
            float           _f_SlideHorizCosAngle
        )
{
    DYN_ReboundHorizCosAngleSet(_pst_Col, _f_ReboundHorizCosAngle);
    DYN_SlideHorizCosAngleSet(_pst_Col, _f_SlideHorizCosAngle);
    DYN_ReboundSet(_pst_Col, _f_Rebound);
    DYN_SlideSet(_pst_Col, _f_Slide);
}

/*
 ===================================================================================================
    Aim:    Reinit the solid structure with default values
 ===================================================================================================
 */
void DYN_ColReinit(DYN_tdst_Col *_pst_Col)
{
    DYN_ColInit
    (
        _pst_Col,
        DYN_C_DefaultRebound,
        DYN_C_DefaultSlide,
        DYN_C_DefaultReboundHorizCosAngle,
        DYN_C_DefaultSlideHorizCosAngle
    );
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void DYN_ColClose(DYN_tdst_Col *_pst_Col)
{
    MEM_Free(_pst_Col);
}


#if defined(PSX2_TARGET) && defined(__cplusplus)
} 
#endif
