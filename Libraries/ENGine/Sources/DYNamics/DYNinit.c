/*$T DYNinit.c GC!1.71 02/07/00 11:28:06 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/* Aim: Initialisations */
#include "Precomp.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "ENGine/Sources/DYNamics/DYNaccess.h"
#include "ENGine/Sources/DYNamics/DYNsolid.h"
#include "ENGine/Sources/DYNamics/DYNcol.h"
#include "ENGine/Sources/DYNamics/DYNConstraint.h"
#include "BASe/BAStypes.h"

#ifdef ACTIVE_EDITORS
#include "EDIpaths.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

extern ULONG		LOA_ul_FileTypeSize[40];

/*
 =======================================================================================================================
    Aim:    Create the dynamic struture (without initializing)
 =======================================================================================================================
 */
DYN_tdst_Dyna *DYN_pst_Dyna_Create(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    DYN_tdst_Dyna   *pst_Dyna;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Allocate the main dynamics structure */
    pst_Dyna = (DYN_tdst_Dyna *) MEM_p_Alloc(sizeof(DYN_tdst_Dyna));
	LOA_ul_FileTypeSize[37] += sizeof(DYN_tdst_Dyna);
    L_memset(pst_Dyna, 0, sizeof(DYN_tdst_Dyna));

#ifdef ACTIVE_EDITORS
    pst_Dyna->sz_Name = NULL;
#endif
    return(pst_Dyna);
}

/*
 =======================================================================================================================
    Aim:    Initialises the dynamics structure (already allocated)

    In:     _pst_Position       Pointer to the position (vector of the game object structure)
 =======================================================================================================================
 */
void DYN_Dyna_Init
(
    DYN_tdst_Dyna       *_pst_Dyna,
    LONG                _ul_DynFlags,
    float               _f_Mass,
    float               _f_SpeedLimitHoriz,
    float               _f_SpeedLimitVert,
    MATH_tdst_Vector    *_pst_InitSpeed,
    MATH_tdst_Vector    *_pst_Position
)
{
    /* DYN_SetDynFlags(_pst_Dyna, _ul_DynFlags); */
    DYN_SetMass(_pst_Dyna, _f_Mass);
    DYN_SetSpeedVector(_pst_Dyna, _pst_InitSpeed);
    DYN_SetPositionPointer(_pst_Dyna, _pst_Position);
    DYN_SetSpeedLimitHoriz(_pst_Dyna, _f_SpeedLimitHoriz);
    DYN_SetSpeedLimitVert(_pst_Dyna, _f_SpeedLimitVert);

    /*
     * The flags are changed, and everything allocated/desallocated, depending on what
     * the flags were before...
     */
    DYN_ChangeDynFlags(_pst_Dyna, _ul_DynFlags, _pst_Dyna->ul_DynFlags);

	_pst_Dyna->ul_DynFlags &= ~DYN_C_OptimizeColEnable;
	_pst_Dyna->ul_DynFlags &= ~DYN_C_OptimizeColDisable;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DYN_Dyna_SetName(DYN_tdst_Dyna *_pst_Dyna, char *_sz_Name)
{
    if(_sz_Name == NULL)
        _pst_Dyna->sz_Name = NULL;
    else
        _pst_Dyna->sz_Name = L_strcpy((char*)MEM_p_Alloc(L_strlen(_sz_Name) + 1), _sz_Name);
}

/*
 =======================================================================================================================
    Aim:    Register a dynamics struct with a given Name

    In:     _b_SetName  Set it to TRUE if you want the name of the dynamic to be changed
 =======================================================================================================================
 */
void DYN_Dyna_RegisterWithName(DYN_tdst_Dyna *_pst_Dyna, char *_sz_Name, int _b_SetName)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char        asz_Tmp[255];
    char        asz_Path[BIG_C_MaxLenPath];
    static int  i_NameNumber = 0;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_sz_Name == NULL)
    {
        sprintf(asz_Tmp, "DynaUnnamed%i", i_NameNumber);
        L_strcat(asz_Tmp, EDI_Csz_ExtDyna);
        i_NameNumber++;
    }
    else
        strcpy(asz_Tmp, _sz_Name);

    WOR_GetSubPath(WOR_gpst_WorldToLoadIn, EDI_Csz_Path_Dyna, asz_Path);
    LINK_RegisterPointer(_pst_Dyna, LINK_C_ENG_GO_Dyna, asz_Tmp, asz_Path);
    DYN_Dyna_SetName(_pst_Dyna, asz_Tmp);
}

#endif

/*
 =======================================================================================================================
    Aim:    Create a DYN_tdst_Forces structure (without initialising)
 =======================================================================================================================
 */
DYN_tdst_Forces *DYN_pst_Forces_Create(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    DYN_tdst_Forces *pst_Forces;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Allocate the main dynamics structure */
    pst_Forces = (DYN_tdst_Forces *) MEM_p_Alloc(sizeof(DYN_tdst_Forces));
    L_memset(pst_Forces, 0, sizeof(DYN_tdst_Forces));

    return(pst_Forces);
}

/*
 =======================================================================================================================
    Aim:    Initializes the fields of the DYN_tdst_Forces struture

    Note:   If the given pointer to vectors is nul, the vector is not changed.
 =======================================================================================================================
 */
void DYN_Forces_Init
(
    DYN_tdst_Forces     *_pst_Forces,
    MATH_tdst_Vector    *_pst_Gravity,
    MATH_tdst_Vector    *_pst_Traction,
    MATH_tdst_Vector    *_pst_Stream,
    float               _f_K
)
{
    if(_pst_Gravity) MATH_CopyVector(&_pst_Forces->st_Gravity, _pst_Gravity);
    if(_pst_Traction) MATH_CopyVector(&_pst_Forces->st_Traction, _pst_Traction);
    if(_pst_Stream) MATH_CopyVector(&_pst_Forces->st_Stream, _pst_Stream);
    _pst_Forces->st_K.x = _f_K;
    _pst_Forces->st_K.y = 0;
    _pst_Forces->st_K.z = 0;

    /* For the moment... */
    _pst_Forces->pst_MoreForces = NULL;
}

/*
 =======================================================================================================================
    Aim:    Remove a DYN_tdst_Forces structure
 =======================================================================================================================
 */
void DYN_Forces_Close(DYN_tdst_Forces *_pst_Forces)
{
    MEM_Free(_pst_Forces);
}

/*
 =======================================================================================================================
    Aim:    Remove the dynamics structure
 =======================================================================================================================
 */
void DYN_Dyna_Close(DYN_tdst_Dyna *_pst_Dyna)
{
#ifdef ACTIVE_EDITORS
    if(_pst_Dyna->sz_Name) MEM_Free(_pst_Dyna->sz_Name);
    LINK_DelRegisterPointer(_pst_Dyna);
#endif
    if(DYN_ul_GetDynFlags(_pst_Dyna) & DYN_C_BasicForces)
    {
        DYN_Forces_Close(_pst_Dyna->pst_Forces);
    }

    if(DYN_ul_GetDynFlags(_pst_Dyna) & DYN_C_Solid)
    {
        DYN_SolidClose(_pst_Dyna->pst_Solid);
    }

    if(DYN_ul_GetDynFlags(_pst_Dyna) & DYN_C_Col)
    {
        DYN_ColClose(_pst_Dyna->pst_Col);
    }

    if(DYN_ul_GetDynFlags(_pst_Dyna) & DYN_C_Constraint)
    {
        DYN_ConstraintClose(_pst_Dyna->pst_Constraint);
    }

    MEM_Free(_pst_Dyna);
}

/*
 =======================================================================================================================
    Aim:    Reinit the dynamics structure £

    Note:   Mass is initilaized to 1 £
            Speed Limit to infinit £
            Friction to 1 £
            Speed and all forces to 0 £
            Friction Vector Flag set to 0 £
            Rotation speed and torque are set to 0 £
            Slide set to 1, Rebound set to 0 £
            SlideHorizCosAngle and ReboundHorizCosAngle set to 1 £
            Desallocate the Dynamics if allocated
 =======================================================================================================================
 */
void DYN_Reinit(DYN_tdst_Dyna *_pst_Dyna)
{
    DYN_SetMass(_pst_Dyna, Cf_One);
    DYN_SetSpeedVector(_pst_Dyna, &MATH_gst_NulVector);
    DYN_SetSpeedLimitHoriz(_pst_Dyna, Cf_Infinit);
    DYN_SetSpeedLimitVert(_pst_Dyna, Cf_Infinit);
	DYN_SetMaxStepSize(_pst_Dyna, -1.0f);

    DYN_ClearFlags(_pst_Dyna, DYN_C_VectorFriction + DYN_C_AutoOrient);

    if(DYN_ul_TestDynFlags(_pst_Dyna, DYN_C_BasicForces))
    {
        DYN_Forces_Init(_pst_Dyna->pst_Forces, &MATH_gst_NulVector, &MATH_gst_NulVector, &MATH_gst_NulVector, Cf_One);
    }

    if(DYN_ul_TestDynFlags(_pst_Dyna, DYN_C_Solid))
    {
        DYN_SolidReinit(DYN_pst_SolidGet(_pst_Dyna));
    }

    if(DYN_ul_TestDynFlags(_pst_Dyna, DYN_C_Col))
    {
        DYN_ColReinit(DYN_pst_ColGet(_pst_Dyna));
    }

    if(DYN_ul_TestDynFlags(_pst_Dyna, DYN_C_Constraint))
    {
        DYN_ConstraintReinit(DYN_pst_ConstraintGet(_pst_Dyna));
    }

	_pst_Dyna->ul_DynFlags &= ~DYN_C_OptimizeColEnable;
	_pst_Dyna->ul_DynFlags &= ~DYN_C_OptimizeColDisable;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
