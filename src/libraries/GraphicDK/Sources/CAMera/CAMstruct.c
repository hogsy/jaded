/*$T CAMstruct.c GC!1.55 12/28/99 12:47:00 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifndef PSX2_TARGET
#ifdef ACTIVE_EDITORS
#include "MAD_loadSave/Sources/MAD_Struct_V0.h"
#include "MAD_mem/Sources/MAD_mem.h"
#endif
#endif

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "GDInterface/GDInterface.h"
#include "CAMera/CAMstruct.h"
#include "CAMera/CAMera.h"
#include "SOFT/SOFTlinear.h"

#include "GEOmetric/GEODebugObject.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C" {
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CAM_tdst_CameraObject *CAM_pst_Create(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CAM_tdst_CameraObject   *pst_Cam;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    pst_Cam = (CAM_tdst_CameraObject *) MEM_p_Alloc(sizeof(CAM_tdst_CameraObject));
    L_memset(pst_Cam, 0, sizeof(CAM_tdst_CameraObject));
    GRO_Struct_Init(&pst_Cam->st_Id, GRO_Camera);
    return pst_Cam;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_Free(CAM_tdst_CameraObject *_pst_Cam)
{
    if(_pst_Cam == NULL) return;
    if(_pst_Cam->st_Id.i->ul_Type != GRO_Camera) return;
    if(_pst_Cam->st_Id.l_Ref > 0) return;
    GRO_Struct_Free(&_pst_Cam->st_Id);
    LOA_DeleteAddress(_pst_Cam);
    MEM_Free(_pst_Cam);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *CAM_p_CreateFromBuffer(GRO_tdst_Struct *_pst_Id, char **ppc_Buffer, WOR_tdst_World *_pst_World)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CAM_tdst_CameraObject   *pst_Cam;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Cam = CAM_pst_Create();
    //GRO_Struct_SetName(&pst_Cam->st_Id, _pst_Id->sz_Name);

    /* Get structure content */
    pst_Cam->ul_Flags = LOA_ReadULong(ppc_Buffer);
    pst_Cam->f_NearPlane = LOA_ReadFloat(ppc_Buffer);
    pst_Cam->f_FarPlane = LOA_ReadFloat(ppc_Buffer);
    pst_Cam->f_FieldOfVision = LOA_ReadFloat(ppc_Buffer);

    return pst_Cam;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG CAM_l_SaveInBuffer(CAM_tdst_CameraObject *_pst_Cam, void *p_Unused)
{
#ifdef ACTIVE_EDITORS
    GRO_Struct_Save(&_pst_Cam->st_Id);

    SAV_Buffer(&_pst_Cam->ul_Flags, 4);
    SAV_Buffer(&_pst_Cam->f_NearPlane, 4);
    SAV_Buffer(&_pst_Cam->f_FarPlane, 4);
    SAV_Buffer(&_pst_Cam->f_FieldOfVision, 4);

#endif
    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
void *CAM_p_CreateFromMad(void *p_MadCamera)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CAM_tdst_CameraObject   *pst_Cam;
    MAD_Camera              *pst_MadCam;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_MadCam = (MAD_Camera *) p_MadCamera;

    pst_Cam = CAM_pst_Create();
    GRO_Struct_SetName(&pst_Cam->st_Id, pst_MadCam->ID.Name);

    switch(pst_MadCam->CameraType)
    {
    case MAD_e_OrthoCamera:
        pst_Cam->ul_Flags = CAM_Cul_Flags_Isometric;
        break;
    case MAD_e_PerspCamera:
        pst_Cam->ul_Flags = CAM_Cul_Flags_Perspective;
        break;
    }

    pst_Cam->f_NearPlane = pst_MadCam->PerspInf.Znear;
    pst_Cam->f_FarPlane = pst_MadCam->PerspInf.Zfar;
    pst_Cam->f_FieldOfVision = pst_MadCam->PerspInf.Fov;

    return pst_Cam;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAD_Camera *CAM_p_ToMad(CAM_tdst_CameraObject *_pst_Cam, WOR_tdst_World *_pst_World)
{
#ifdef ACTIVE_EDITORS

    /*~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Camera  *pst_MadCam;
    //char        *psz_Ext;
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/

    MAD_MALLOC(MAD_Camera, pst_MadCam, 1);
    pst_MadCam->ID.IDType = ID_MAD_Camera;
    pst_MadCam->ID.SizeOfThisOne = sizeof(MAD_Camera);
    //if((psz_Ext = strrchr(_pst_Cam->st_Id.sz_Name, '.'))) *psz_Ext = 0;
    strcpy(pst_MadCam->ID.Name, GRO_sz_Struct_GetName( &_pst_Cam->st_Id ) );
    //if(psz_Ext) *psz_Ext = '.';
    if(_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
        pst_MadCam->CameraType = MAD_e_PerspCamera;
    else
        pst_MadCam->CameraType = MAD_e_OrthoCamera;

    pst_MadCam->PerspInf.Fov = _pst_Cam->f_FieldOfVision;
    pst_MadCam->PerspInf.Znear = _pst_Cam->f_NearPlane;
    pst_MadCam->PerspInf.Zfar = _pst_Cam->f_FarPlane;

    return pst_MadCam;
#endif
    return NULL;
}


#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG CAM_l_HasSomethingToRender
(
    GRO_tdst_Visu           *_pst_Visu,
    GEO_tdst_Object         **ppst_PickableObject
)
{
#ifdef ACTIVE_EDITORS
    *ppst_PickableObject = GEO_pst_DebugObject_Get(GEO_DebugObject_Camera);
#endif /* ACTIVE_EDITORS */
    return GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_DisplayCamera;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_Render(OBJ_tdst_GameObject *_pst_Node)
{
#ifdef ACTIVE_EDITORS
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG       C, DM;
    MATH_tdst_Matrix    *M;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    C = (_pst_Node->ul_EditorFlags & OBJ_C_EditFlags_Selected) ? 1 : 0;
    C = GEO_ul_DebugObject_GetColor(GEO_DebugObject_Camera, C);
    GEO_DebugObject_SetLight(C);
    M = OBJ_pst_GetAbsoluteMatrix(_pst_Node);
    DM = GDI_Cul_DM_All - GDI_Cul_DM_UseAmbient - GDI_Cul_DM_Fogged - GDI_Cul_DM_MaterialColor;
    GEO_DebugObject_Draw(GDI_gpst_CurDD, GEO_DebugObject_Camera, DM, _pst_Node->ul_EditorFlags, M);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_Init(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~*/
    GRO_tdst_Interface  *i;
    /*~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Init light object interface */
    i = &GRO_gast_Interface[GRO_Camera];
#ifdef JADEFUSION
	i->pfnp_CreateFromBuffer = (void *(__cdecl *)(GRO_tdst_Struct *,char ** ,void *))CAM_p_CreateFromBuffer;
    i->pfn_Destroy = (void (__cdecl *)(void *))CAM_Free;
    i->pfnl_HasSomethingToRender = (LONG (__cdecl *)(void *,void *))CAM_l_HasSomethingToRender;
#else 
	i->pfnp_CreateFromBuffer = CAM_p_CreateFromBuffer;
    i->pfn_Destroy = CAM_Free;
    i->pfnl_HasSomethingToRender = CAM_l_HasSomethingToRender;
#endif

#ifdef ACTIVE_EDITORS
#ifdef JADEFUSION
	i->pfnl_SaveInBuffer = (LONG (__cdecl *)(void *,void *))CAM_l_SaveInBuffer;
    i->pfnp_CreateFromMad = CAM_p_CreateFromMad;
    i->pfnp_ToMad = (void* (__cdecl *)(void *,void *))CAM_p_ToMad;
    i->pfnl_PushSpecialMatrix = (LONG (__cdecl *)(void *))GRO_PushSpecialMatrixForProportionnal;
    i->pfn_Render = (void (__cdecl *)(void *))CAM_Render;
#else
	i->pfnl_SaveInBuffer = CAM_l_SaveInBuffer;
    i->pfnp_CreateFromMad = CAM_p_CreateFromMad;
    i->pfnp_ToMad = CAM_p_ToMad;
    i->pfnl_PushSpecialMatrix = GRO_PushSpecialMatrixForProportionnal;
    i->pfn_Render = CAM_Render;
#endif
#endif

    /* Init global camera */
    CAM_gst_IsoNoClip.ul_Flags = CAM_Cul_Flags_Isometric;
    CAM_gst_IsoNoClip.f_NearPlane = 1.0f;
    CAM_gst_IsoNoClip.f_FarPlane = 0.0f;
    CAM_gst_IsoNoClip.f_YoverX = 1.0f;

    CAM_gst_IsoNoClip.f_IsoFactor = 80.0f;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
