
/*$T GROimportfrommad.c GC!1.39 06/23/99 14:59:22 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "GRObject/GROimportfrommad.h"

/*$4
 ***************************************************************************************************
    Private Functions prototypes
 ***************************************************************************************************
 */

ULONG   GRO_ul_ConvertMadType(LONG);
ULONG   GRO_ul_ConvertMadMaterialType(LONG);


extern int GEO_gi_CreateFromMad_DoClean;

/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
GRO_tdst_Struct *GRO_pst_ImportFromMad(MAD_NodeID *_pst_MadObject, WOR_tdst_World *_pst_World, LONG _l_Flags)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GRO_tdst_Struct *pst_Struct;
    LONG            l_Index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (_l_Flags == GRO_C_Import_MadObject)
    {
        l_Index = GRO_ul_ConvertMadType(_pst_MadObject->IDType);
        if (l_Index == -1) return NULL;
        GEO_gi_CreateFromMad_DoClean = 1;
        pst_Struct = (GRO_tdst_Struct *) GRO_gast_Interface[l_Index].pfnp_CreateFromMad(_pst_MadObject);
        GEO_gi_CreateFromMad_DoClean = 0;
        return pst_Struct;
    }
    
    if (_l_Flags == GRO_C_Import_MadMaterial)
    {
        l_Index = GRO_ul_ConvertMadMaterialType(((MAD_MAT_MatID *) _pst_MadObject)->MaterialType);
        if (l_Index == -1) return NULL;
        pst_Struct = (GRO_tdst_Struct *) GRO_gast_Interface[l_Index].pfnp_CreateFromMad(_pst_MadObject);
        return pst_Struct;
    }

    return NULL;
}

/*$4
 ***************************************************************************************************
    Private Functions
 ***************************************************************************************************
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
ULONG GRO_ul_ConvertMadType(LONG _l_Type)
{
    /*$off*/
    static LONG sl_ConversionTable[15] =
    {
        GRO_Unknown,    /* ID_MAD_World */
        GRO_Unknown,    /* ID_MAD_Matrix */
        GRO_Unknown,    /* ID_MAD_Material */
        GRO_Unknown,    /* ID_MAD_Texture */
        GRO_Geometric,  /* ID_MAD_GeometricObject_V0 */
        GRO_Light,      /* ID_MAD_Light */
        GRO_Camera,     /* ID_MAD_Camera */
        GRO_Unknown,    /* ID_MAD_WorldNode */
        GRO_Unknown,    /* ID_MAD_TargetObject */
        GRO_Unknown,    /* ID_MAD_Shape */
        GRO_Waypoint,   /* ID_MAD_Dummy */
		GRO_Unknown,	/* ID_MAD_SubFile */
		GRO_Unknown,	/* ID_MAD_GeometricObjectAddOn */
		GRO_Unknown,	/* ID_MAD_GeometricObjectLOD */
		GRO_Geometric,	/* ID_MAD_SkinnedGeometricObject */
    };
    /*$on*/
    if((ULONG) _l_Type >= 15) return GRO_Unknown;
    return sl_ConversionTable[_l_Type];
}

/*
 ===================================================================================================
 ===================================================================================================
 */
ULONG GRO_ul_ConvertMadMaterialType(LONG _l_Type)
{
    /*$off*/
    static LONG sl_ConversionTable[3] =
    {
        GRO_Unknown,   /* ID_MAT_NullMat */
        //GRO_MaterialSingle, 
        GRO_MaterialMultiTexture, /* ID_MAT_Standard */
        GRO_MaterialMulti,  /*ID_MAT_MultiMaterial */
    };
    /*$on*/
    if((ULONG) _l_Type >= 3) return GRO_Unknown;
    return sl_ConversionTable[_l_Type];
}

#endif /* ACTIVE_EDITORS */
