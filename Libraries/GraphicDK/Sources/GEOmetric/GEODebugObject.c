/*$T GEODebugObject.c GC! 1.075 03/07/00 09:56:28 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS   /* This file is empty when not compiled with editors */

#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGfat.h"
#include "../Main/WinEditors/Sources/EDIpaths.h"
#include "GEOmetric/GEODebugObject.h"
#include "GEOmetric/GEODebugObjectdef.h"
#include "GEOmetric/GEOobjectaccess.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "LINKs/LINKtoed.h"

#ifdef JADEFUSION
#include "BASe/MEMory/MEM.h"
#endif
/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */

#define M_GetData(prefix) \
    { \
        p = prefix##_gl_NbPoints; \
        t = prefix##_gl_NbTriangles; \
        vertices = prefix##_gast_Point; \
        triangles = prefix##_gast_Triangle; \
    }

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

GEO_tdst_Object     *GEO_gpst_DebugObject[GEO_DebugObject_Number];
BOOL                GEO_gb_DebugObjectInit = 0;
float               GEO_f_DebugObject_Scale = 1.0f;

short               GEO_gw_DebugObject_Texture = -1;

/* Light */
OBJ_tdst_GameObject *GEO_gpst_DebugObject_GOLight;
LIGHT_tdst_Light    GEO_gst_DebugObject_Light;
LIGHT_tdst_List     GEO_gst_DebugObject_LightList;

/*$off*/
ULONG       GEO_gaaul_DebugObject_Color[GEO_DebugObject_Number][2] =
{
    { 0xFF0000FF, 0xFF00FFFF }, /* GEO_DebugObject_Sphere */
    { 0xFF0000FF, 0xFF00FFFF }, /* GEO_DebugObject_Box */
    { 0xFFFF0000, 0xFF00FFFF }, /* GEO_DebugObject_LightOmni */
    { 0xFFFF0000, 0xFF00FFFF }, /* GEO_DebugObject_LightSpot */
    { 0xFF50FF50, 0xFF00FFFF }, /* GEO_DebugObject_Camera */
    { 0xFFFF0000, 0xFF00FFFF }, /* GEO_DebugObject_Arrow */
    { 0xFF0000FF, 0xFF00FFFF }, /* GEO_DebugObject_Torus */
    { 0xFF0000FF, 0xFF00FFFF }, /* GEO_DebugObject_Waypoint */
    { 0xFF0000FF, 0xFF00FFFF }, /* GEO_DebugObject_Question */
    { 0xFF0000FF, 0xFF00FFFF }, /* GEO_DebugObject_SystemAxis */
    { 0xFFFFFF00, 0xFF00FFFF }, /* GEO_DebugObject_LinkArrow */
    { 0xFFFFFF00, 0xFF00FFFF }, /* GEO_DebugObject_Cylinder */
    { 0xFFFFFF00, 0xFF00FFFF }, /* GEO_DebugObject_Cone */
    { 0xFF0000FF, 0xFF00FFFF }, /* GEO_DebugObject_Gizmo */
#ifdef JADEFUSION
	{ 0xFF0080FF, 0xFF00FFFF }, /* GEO_DebugObject_Portal */
#endif
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible00 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible01 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible02 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible03 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible04 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible05 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible06 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible07 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible08 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible09 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible0A */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible0B */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible0C */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible0D */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible0E */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible0F */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible10 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible11 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible12 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible13 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible14 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible15 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible16 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible17 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible18 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible19 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible1A */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible1B */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible1C */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible1D */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible1E */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible1F */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible20 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible21 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible22 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible23 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible24 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible25 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible26 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible27 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible28 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible29 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible2A */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible2B */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible2C */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible2D */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible2E */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible2F */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible30 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible31 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible32 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible33 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible34 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible35 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible36 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible37 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible38 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible39 */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible3A */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible3B */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible3C */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible3D */
    { 0xFF808080, 0xFFFFFFFF }, /* GEO_DebugObject_Invisible3E */
    { 0xFF808080, 0xFFFFFFFF }  /* GEO_DebugObject_Invisible3F */
};
/*$on*/

/*$4
 ***********************************************************************************************************************
    Private functions prototypes
 ***********************************************************************************************************************
 */

GEO_tdst_Object     *GEO_pst_DebugObject_CreateSphere(LONG, LONG, float);
GEO_tdst_Object     *GEO_pst_DebugObject_CreateBox(void);
GEO_tdst_Object     *GEO_pst_DebugObject_CreateCamera(void);
GEO_tdst_Object     *GEO_pst_DebugObject_CreateTorus(float, float, LONG, LONG);
GEO_tdst_Object     *GEO_pst_DebugObject_CreateArrow(void);
GEO_tdst_Object     *GEO_pst_DebugObject_CreateQuestion(void);
GEO_tdst_Object     *GEO_pst_DebugObject_CreateSystemAxis(void);
GEO_tdst_Object     *GEO_pst_DebugObject_CreateInvisible(int);
GEO_tdst_Object     *GEO_pst_DebugObject_CreateCone(void);
GEO_tdst_Object     *GEO_pst_DebugObject_CreateSpot(void);
GEO_tdst_Object     *GEO_pst_DebugObject_CreateWaypoint(void);
GEO_tdst_Object     *GEO_pst_DebugObject_CreateLinkArrow(void);
GEO_tdst_Object     *GEO_pst_DebugObject_CreateCylinder(void);
#ifdef JADEFUSION
GEO_tdst_Object     *GEO_pst_DebugObject_CreatePortal(void);
#endif
/*$4
 ***********************************************************************************************************************
    Public function
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DebugObject_Create(void)
{
    /*~~~~~~~~~~~~*/
    ULONG   ul_Flag;
    /*~~~~~~~~~~~~*/

    if(GEO_gb_DebugObjectInit) return;

    GEO_gb_DebugObjectInit = 1;

    GEO_gpst_DebugObject[GEO_DebugObject_Sphere] = GEO_pst_DebugObject_CreateSphere(8, 8, 1.0f);
    GEO_gpst_DebugObject[GEO_DebugObject_Box] = GEO_pst_DebugObject_CreateBox();
    GEO_gpst_DebugObject[GEO_DebugObject_LightOmni] = GEO_pst_DebugObject_CreateSphere(3, 4, 0.5f);
    GEO_gpst_DebugObject[GEO_DebugObject_LightSpot] = GEO_pst_DebugObject_CreateSpot();
    GEO_gpst_DebugObject[GEO_DebugObject_Camera] = GEO_pst_DebugObject_CreateCamera();
    GEO_gpst_DebugObject[GEO_DebugObject_Arrow] = GEO_pst_DebugObject_CreateArrow();
    GEO_gpst_DebugObject[GEO_DebugObject_Torus] = GEO_pst_DebugObject_CreateTorus(1.0f, .02f, 20, 3);
    GEO_gpst_DebugObject[GEO_DebugObject_Waypoint] = GEO_pst_DebugObject_CreateWaypoint();
    GEO_gpst_DebugObject[GEO_DebugObject_Question] = GEO_pst_DebugObject_CreateQuestion();
    GEO_gpst_DebugObject[GEO_DebugObject_SystemAxis] = GEO_pst_DebugObject_CreateSystemAxis();
    GEO_gpst_DebugObject[GEO_DebugObject_LinkArrow] = GEO_pst_DebugObject_CreateLinkArrow();
    GEO_gpst_DebugObject[GEO_DebugObject_Cylinder] = GEO_pst_DebugObject_CreateCylinder();
    GEO_gpst_DebugObject[GEO_DebugObject_Cone] = GEO_pst_DebugObject_CreateCone();
    GEO_gpst_DebugObject[GEO_DebugObject_Gizmo] = GEO_pst_DebugObject_CreateSphere(3, 4, 0.01f);
#ifdef JADEFUSION
    GEO_gpst_DebugObject[GEO_DebugObject_Portal] = GEO_pst_DebugObject_CreatePortal();
#endif
    for(ul_Flag = 0; ul_Flag <= GEO_DebugObject_LastInvisible - GEO_DebugObject_Invisible; ul_Flag++)
    {
        GEO_gpst_DebugObject[GEO_DebugObject_Invisible + ul_Flag] = GEO_pst_DebugObject_CreateInvisible(ul_Flag);
    }

    /* Light */
    ul_Flag = OBJ_C_IdentityFlag_ExtendedObject;
    ul_Flag |= OBJ_C_IdentityFlag_Lights;
    GEO_gpst_DebugObject_GOLight = OBJ_GameObject_Create(ul_Flag);
    GEO_gst_DebugObject_Light.st_Id.i = &GRO_gast_Interface[GRO_Light];
    GEO_gst_DebugObject_Light.ul_Flags = LIGHT_Cul_LF_Omni;
    GEO_gst_DebugObject_Light.ul_Flags |= LIGHT_Cul_LF_UseColor;
    GEO_gst_DebugObject_Light.ul_Flags |= LIGHT_Cul_LF_UseAlpha;
    GEO_gst_DebugObject_Light.ul_Flags |= LIGHT_Cul_LF_RealTimeOnDynam;
    GEO_gst_DebugObject_Light.ul_Flags |= LIGHT_Cul_LF_RealTimeOnNonDynam;
    GEO_gst_DebugObject_Light.st_Omni.f_Near = 10000.0f;
    GEO_gst_DebugObject_Light.st_Omni.f_Far = 10000.0f;
    GEO_gpst_DebugObject_GOLight->pst_Extended->pst_Light = &GEO_gst_DebugObject_Light.st_Id;

    LIGHT_List_Init(&GEO_gst_DebugObject_LightList, 2);
    LIGHT_List_AddLight(&GEO_gst_DebugObject_LightList, GEO_gpst_DebugObject_GOLight);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DebugObject_Destroy(void)
{
    /*~~~~~~~~~~~~*/
    LONG    lObject;
    /*~~~~~~~~~~~~*/

    if(GEO_gb_DebugObjectInit == 0) return;

    for(lObject = 0; lObject < GEO_DebugObject_Number; lObject++)
    {
        GEO_Free(GEO_gpst_DebugObject[lObject]);
    }

    GEO_gb_DebugObjectInit = 0;
    GEO_gpst_DebugObject_GOLight->pst_Extended->pst_Light = NULL;
    OBJ_GameObject_Remove(GEO_gpst_DebugObject_GOLight, 1);
    LIGHT_List_Close(&GEO_gst_DebugObject_LightList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DebugObject_AddTexture( void ) 
{
    /*~~~~~~~~~~~~*/
    ULONG ul_Key;
    /*~~~~~~~~~~~~*/

    ul_Key = BIG_ul_SearchFileExt(EDI_Csz_Path_DisplayData, "DebugObjectTexture.tga" );
    if (ul_Key != BIG_C_InvalidIndex)
        GEO_gw_DebugObject_Texture = TEX_w_List_AddTexture( &TEX_gst_GlobalList, BIG_FileKey( ul_Key ), 1 );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DebugObject_UsedTexture( char *_pc_UsedIndex )
{
    if (GEO_gw_DebugObject_Texture != -1)
        _pc_UsedIndex[GEO_gw_DebugObject_Texture] = 1;
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObject_Get(LONG _l_Object)
{
    if((ULONG) _l_Object >= GEO_DebugObject_Number) return NULL;

    if(GEO_gb_DebugObjectInit == 0) GEO_DebugObject_Create();

    return GEO_gpst_DebugObject[_l_Object];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DebugObject_Draw
(
    GDI_tdst_DisplayData    *_pst_DD,
    LONG                    _l_Object,
    ULONG                   _ul_DrawMask,
    ULONG                   _ul_NodeFlags,
    MATH_tdst_Matrix        *_pst_NodeMatrix
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject st_GO;
    OBJ_tdst_Base       st_Base;
    GRO_tdst_Visu       st_Visu;
    ULONG               ul_SaveDI;
    ULONG               ul_SaveDM;
    LIGHT_tdst_List     st_SaveLightList;
    LONG                gl_SaveComputeRLI;
    int					i_SaveSpeedDraw;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&st_GO, 0, sizeof(OBJ_tdst_GameObject));
#ifdef JADEFUSION
	L_memset(&st_Visu, 0, sizeof(GRO_tdst_Visu));
#endif
	st_GO.pst_GlobalMatrix = _pst_NodeMatrix;
    st_GO.pst_Base = &st_Base;
    st_Base.pst_Visu = &st_Visu;

    st_Visu.pst_Object = (GRO_tdst_Struct *) GEO_pst_DebugObject_Get(_l_Object);
    st_Visu.pst_Material = NULL;
    st_Visu.dul_VertexColors = NULL;
    st_GO.ul_EditorFlags = _ul_NodeFlags;
    ul_SaveDM = _pst_DD->ul_CurrentDrawMask;
    ul_SaveDI = _pst_DD->ul_DisplayInfo;
    gl_SaveComputeRLI = LIGHT_gl_ComputeRLI;
    LIGHT_gl_ComputeRLI = 0;
    i_SaveSpeedDraw = LINK_gi_SpeedDraw;

    if(GEO_gst_DebugObject_Light.ul_Flags & LIGHT_Cul_LF_Active)
    {
        /* Save light list */
        L_memcpy(&st_SaveLightList, &_pst_DD->st_LightList, sizeof(LIGHT_tdst_List));
        L_memcpy(&_pst_DD->st_LightList, &GEO_gst_DebugObject_LightList, sizeof(LIGHT_tdst_List));
        LINK_gi_SpeedDraw = 0;
    }

    if (_pst_DD->ul_DrawMask & GDI_Cul_DM_UseTexture)
        if ( ((GEO_tdst_Object *) st_Visu.pst_Object)->l_NbUVs != 0)
        {
            MAT_gst_DefaultSingleMaterial.pst_FirstLevel = &MAT_gst_DefaultMatMTLevel;
            MAT_gst_DefaultMatMTLevel.s_TextureId = GEO_gw_DebugObject_Texture;
        }

	_pst_DD->ul_CurrentDrawMask = _ul_DrawMask;
    _pst_DD->ul_DisplayInfo |= GDI_Cul_DI_DontSortObject;
	_pst_DD->pst_CurrentMat = &MAT_gst_DefaultSingleMaterial;
    
#if defined(_XENON_RENDER)
    // Do not use textures when the texture coordinates are not available
    if (((GEO_tdst_Object *) st_Visu.pst_Object)->l_NbUVs == 0)
    {
        _pst_DD->ul_CurrentDrawMask &= ~GDI_Cul_DM_UseTexture;
    }

    _pst_DD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_Fogged | GDI_Cul_DM_EmitShadowBuffer | GDI_Cul_DM_ReceiveShadowBuffer);

    BOOL bSharedIdentityMatrix = FALSE;
    if (st_GO.pst_GlobalMatrix == NULL)
    {
        st_GO.pst_GlobalMatrix = &MATH_gst_IdentityMatrix;
        bSharedIdentityMatrix  = TRUE;
    }

    // Use software lighting
    LIGHT_gb_DisableXenonSend = TRUE;
#endif	
	st_Visu.pst_Object->i->pfn_Render(&st_GO);

#ifndef JADEFUSION
    MAT_gst_DefaultSingleMaterial.pst_FirstLevel = NULL;
#endif
#if defined(_XENON_RENDER)

    LIGHT_gb_DisableXenonSend = FALSE;

    if (bSharedIdentityMatrix)
    {
        st_GO.pst_GlobalMatrix = NULL;
    }

    if (st_Visu.p_XeElements != NULL)
    {
        MEM_Free(st_Visu.p_XeElements);
        st_Visu.p_XeElements = NULL;
    }
#endif
    _pst_DD->ul_DisplayInfo = ul_SaveDI;
    _pst_DD->ul_CurrentDrawMask = ul_SaveDM;
    LIGHT_gl_ComputeRLI = gl_SaveComputeRLI;
    LINK_gi_SpeedDraw = i_SaveSpeedDraw;
    if(GEO_gst_DebugObject_Light.ul_Flags & LIGHT_Cul_LF_Active)
    {
        L_memcpy(&_pst_DD->st_LightList, &st_SaveLightList, sizeof(LIGHT_tdst_List));
        GEO_gst_DebugObject_Light.ul_Flags &= ~LIGHT_Cul_LF_Active;
    }

#if defined(_XENON_RENDER)
    _pst_DD->pst_CurrentMat = NULL;
#endif

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DebugObject_PushSphereMatrix(SOFT_tdst_MatrixStack *_pst_MS, MATH_tdst_Vector *_pst_Pos, float _f_Radius)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Matrix    st_SphereMat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MATH_SetIdentityMatrix(&st_SphereMat);
    MATH_SetScaleType(&st_SphereMat);
    st_SphereMat.Sx = st_SphereMat.Sy = st_SphereMat.Sz = _f_Radius;
    MATH_SetTranslation(&st_SphereMat, _pst_Pos);

    SOFT_l_MatrixStack_Push(_pst_MS, &st_SphereMat);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DebugObject_PushBoxMatrix
(
    SOFT_tdst_MatrixStack   *_pst_MS,
    MATH_tdst_Vector        *_pst_Pos1,
    MATH_tdst_Vector        *_pst_Pos2
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Matrix    st_BoxMat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MATH_SetIdentityMatrix(&st_BoxMat);
    MATH_SetScaleType(&st_BoxMat);

    st_BoxMat.Sx = _pst_Pos2->x - _pst_Pos1->x;
    if (st_BoxMat.Sx < 0.001f) st_BoxMat.Sx = 0.001f;
    st_BoxMat.Sy = _pst_Pos2->y - _pst_Pos1->y;
    if (st_BoxMat.Sy < 0.001f) st_BoxMat.Sy = 0.001f;
    st_BoxMat.Sz = _pst_Pos2->z - _pst_Pos1->z;
    if (st_BoxMat.Sz < 0.001f ) st_BoxMat.Sz = 0.001f;

    MATH_SetTranslation(&st_BoxMat, _pst_Pos1);

    SOFT_l_MatrixStack_Push(_pst_MS, &st_BoxMat);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DebugObject_PushCylinderMatrix
(
    SOFT_tdst_MatrixStack   *_pst_MS,
    MATH_tdst_Vector        *_pst_Center,
    float                   _f_Height,
    float                   _f_Radius
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Matrix    st_CylMat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MATH_SetIdentityMatrix(&st_CylMat);
    MATH_SetScaleType(&st_CylMat);

    /* The Default Cylinder is 2 meters high and has a 1 meter Radius. */
    st_CylMat.Sx = _f_Radius / 2;
    st_CylMat.Sy = _f_Radius / 2;
    st_CylMat.Sz = _f_Height / 2;

    MATH_SetTranslation(&st_CylMat, _pst_Center);

    SOFT_l_MatrixStack_Push(_pst_MS, &st_CylMat);
}

void GEO_DebugObject_PushConeMatrix
(
    SOFT_tdst_MatrixStack   *_pst_MS,
    MATH_tdst_Vector        *_pst_Trans,
    float                   _f_Height,
    float                   _f_Radius
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Matrix    st_CylMat;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MATH_SetIdentityMatrix(&st_CylMat);
    MATH_SetScaleType(&st_CylMat);

    /* The Default Cylinder is 2 meters high and has a 1 meter Radius. */
    st_CylMat.Sx = _f_Radius/2.0f;
    st_CylMat.Sy = _f_Height/2.0f;
    st_CylMat.Sz = _f_Radius/2.0f;

    MATH_SetTranslation(&st_CylMat, _pst_Trans);

    SOFT_l_MatrixStack_Push(_pst_MS, &st_CylMat);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DebugObject_PopMatrix(SOFT_tdst_MatrixStack *_pst_MS)
{
    SOFT_l_MatrixStack_Pop(_pst_MS);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DebugObject_SetLight(ULONG _ul_Color)
{
    if(GEO_gb_DebugObjectInit == 0) GEO_DebugObject_Create();

    GEO_gst_DebugObject_Light.ul_Color = _ul_Color;
    GEO_gst_DebugObject_Light.ul_Flags |= LIGHT_Cul_LF_Active;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DebugObject_SetLightMatrix(GDI_tdst_DisplayData *_pst_DD)
{
    if(!GEO_gb_DebugObjectInit) return;

    CAM_SetObjectMatrixFromCam(GEO_gpst_DebugObject_GOLight->pst_GlobalMatrix, &_pst_DD->st_Camera.st_Matrix);
    MATH_AddScaleVector
        ( 
            &GEO_gpst_DebugObject_GOLight->pst_GlobalMatrix->T,
            &GEO_gpst_DebugObject_GOLight->pst_GlobalMatrix->T,
            (MATH_tdst_Vector *) &GEO_gpst_DebugObject_GOLight->pst_GlobalMatrix->Jx,
            10
        );
        
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_DebugObject_GetColor(LONG _l_Object, LONG _l_Index)
{
    return GEO_gaaul_DebugObject_Color[_l_Object][_l_Index];
}

/*$5
 #######################################################################################################################
    Private function
 #######################################################################################################################
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObjectCreateGeneric
(
    LONG                    _l_NbPoints,
    LONG                    _l_NbTriangles,
    MATH_tdst_Vector        *_pst_Point,
    SOFT_tdst_ThreeIndex    *_pst_Triangle
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_Object                     *pst_Obj;
    GEO_tdst_ElementIndexedTriangles    *pst_Element;
    LONG                                l;
    GEO_tdst_IndexedTriangle            st_Triangle;
    float                               fX, fY, fZ;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Obj = GEO_pst_Create(_l_NbPoints, 0, 1, 0);

    for(l = 0; l < _l_NbPoints; l++)
    {
        fX = _pst_Point[l].x * 2 * GEO_f_DebugObject_Scale;
        fY = _pst_Point[l].y * 2 * GEO_f_DebugObject_Scale;
        fZ = _pst_Point[l].z * 2 * GEO_f_DebugObject_Scale;

        GEO_SetPoint(pst_Obj, l, fX, fY, fZ, 0.0f, 0.0f, 0.0f);
    }

    GEO_SetElement(pst_Obj, 0, NULL, _l_NbTriangles);
    pst_Element = pst_Obj->dst_Element;
    L_memset(&st_Triangle, 0, sizeof(GEO_tdst_IndexedTriangle));
    for(l = 0; l < _l_NbTriangles; l++)
    {
        st_Triangle.auw_Index[0] = (unsigned short) _pst_Triangle[l].l_Index[0];
        st_Triangle.auw_Index[1] = (unsigned short) _pst_Triangle[l].l_Index[1];
        st_Triangle.auw_Index[2] = (unsigned short) _pst_Triangle[l].l_Index[2];
        GEO_SetTriangle(pst_Element, l, &st_Triangle);
    }

    GEO_ComputeNormals(pst_Obj);
    return pst_Obj;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObjectCreateGenericWithUV
(
    LONG                    _l_NbPoints,
    LONG                    _l_NbUVs,
    LONG                    _l_NbTriangles,
    MATH_tdst_Vector        *_pst_Point,
    GEO_tdst_UV             *_pst_UV,
    SOFT_tdst_TriangleIndex *_pst_Triangle,
    int                     _i_Texture
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_Object                     *pst_Obj;
    GEO_tdst_ElementIndexedTriangles    *pst_Element;
    LONG                                l;
    GEO_tdst_IndexedTriangle            st_Triangle;
    float                               fX, fY, fZ;
    float                               fU, fV;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Obj = GEO_pst_Create(_l_NbPoints, _l_NbUVs, 1, 0);

    for(l = 0; l < _l_NbPoints; l++)
    {
        fX = _pst_Point[l].x * 2 * GEO_f_DebugObject_Scale;
        fY = _pst_Point[l].y * 2 * GEO_f_DebugObject_Scale;
        fZ = _pst_Point[l].z * 2 * GEO_f_DebugObject_Scale;

        GEO_SetPoint(pst_Obj, l, fX, fY, fZ, 0.0f, 0.0f, 0.0f);
    }

    fU = ((float) (_i_Texture % 8)) / 8.0f;
    fV = ((float) (7 - (_i_Texture / 8))) / 8.0f;
    for(l = 0; l < _l_NbUVs; l++)
    {
        GEO_SetUV( pst_Obj, l, (_pst_UV[l].fU / 8.0f) + fU, (_pst_UV[l].fV / 8.0f) + fV);
    }

    GEO_SetElement(pst_Obj, 0, NULL, _l_NbTriangles);
    pst_Element = pst_Obj->dst_Element;
    L_memset(&st_Triangle, 0, sizeof(GEO_tdst_IndexedTriangle));
    for(l = 0; l < _l_NbTriangles; l++)
    {
        L_memcpy( &st_Triangle, _pst_Triangle + l, sizeof( SOFT_tdst_TriangleIndex ) );
        GEO_SetTriangle(pst_Element, l, &st_Triangle);
    }

    GEO_ComputeNormals(pst_Obj);
    return pst_Obj;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObject_CreateCamera(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_Object *pst_Obj;
    /*~~~~~~~~~~~~~~~~~~~~~*/

    GEO_f_DebugObject_Scale = 0.5f;
    pst_Obj =  GEO_pst_DebugObjectCreateGeneric
        (
            Camera_gl_NbPoints,
            Camera_gl_NbTriangles,
            Camera_gast_Point,
            Camera_gast_Triangle
        );
    GEO_f_DebugObject_Scale = 1.0f;
    return pst_Obj;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObject_CreateQuestion(void)
{
    return GEO_pst_DebugObjectCreateGeneric
        (
            Question_gl_NbPoints,
            Question_gl_NbTriangles,
            Question_gast_Point,
            Question_gast_Triangle
        );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObject_CreateSystemAxis(void)
{
    return GEO_pst_DebugObjectCreateGeneric
        (
            SystemAxis_gl_NbPoints,
            SystemAxis_gl_NbTriangles,
            SystemAxis_gast_Point,
            SystemAxis_gast_Triangle
        );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObject_CreateInvisible(int i)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG                    p, t;
    MATH_tdst_Vector        *vertices;
    SOFT_tdst_ThreeIndex    *triangles;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (GEO_gw_DebugObject_Texture != -1)
    {
        return GEO_pst_DebugObjectCreateGenericWithUV
            (
                Waypoint_gl_NbPoints,
                Waypoint_gl_NbUVs,
                Waypoint_gl_NbTriangles,
                Waypoint_gast_Point,
                Waypoint_gast_UV,
                Waypoint_gast_Triangle,
                1 + i
            );
    }

/*$off*/
    switch(i)
    {
    case 0:        M_GetData(Invisible);         break;
    case 1:        M_GetData(Invisible1);        break;
    case 2:        M_GetData(Invisible2);        break;
    case 3:        M_GetData(Invisible3);        break;
    case 4:        M_GetData(Invisible4);        break;
    case 5:        M_GetData(Invisible5);        break;
    case 6:        M_GetData(Invisible6);        break;
    case 7:        M_GetData(Invisible7);        break;
    case 8:        M_GetData(Invisible8);        break;
    case 9:        M_GetData(Invisible9);        break;
    case 10:       M_GetData(InvisibleA);        break;
    case 11:       M_GetData(InvisibleB);        break;
    case 12:       M_GetData(InvisibleC);        break;
    case 13:       M_GetData(InvisibleD);        break;
    case 14:       M_GetData(InvisibleE);        break;
    case 15:       M_GetData(InvisibleF);        break;
    }
    /*$on*/
    return GEO_pst_DebugObjectCreateGeneric(p, t, vertices, triangles);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObject_CreateArrow(void)
{
    return GEO_pst_DebugObjectCreateGeneric
        (
            Arrow_gl_NbPoints,
            Arrow_gl_NbTriangles,
            Arrow_gast_Point,
            Arrow_gast_Triangle
        );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObject_CreateCone(void)
{
    return GEO_pst_DebugObjectCreateGeneric(Cone_gl_NbPoints, Cone_gl_NbTriangles, Cone_gast_Point, Cone_gast_Triangle);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObject_CreateSpot(void)
{
    return GEO_pst_DebugObjectCreateGeneric(Spot_gl_NbPoints, Spot_gl_NbTriangles, Spot_gast_Point, Spot_gast_Triangle);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObject_CreateWaypoint(void)
{
    return GEO_pst_DebugObjectCreateGenericWithUV
        (
            Waypoint_gl_NbPoints,
            Waypoint_gl_NbUVs,
            Waypoint_gl_NbTriangles,
            Waypoint_gast_Point,
            Waypoint_gast_UV,
            Waypoint_gast_Triangle,
            0
        );

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObject_CreateLinkArrow(void)
{
    return GEO_pst_DebugObjectCreateGeneric
        (
            LinkArrow_gl_NbPoints,
            LinkArrow_gl_NbTriangles,
            LinkArrow_gast_Point,
            LinkArrow_gast_Triangle
        );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObject_CreateCylinder(void)
{
    return GEO_pst_DebugObjectCreateGeneric
        (
            Cylinder_gl_NbPoints,
            Cylinder_gl_NbTriangles,
            Cylinder_gast_Point,
            Cylinder_gast_Triangle
        );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObject_CreateSphere(LONG _l_Lat, LONG _l_Long, float _f_Radius)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_Object                     *pst_Sphere;
    GEO_tdst_ElementIndexedTriangles    *pst_Element;
    LONG                                lLatitude, lLongitude;
    LONG                                lNbPoints, lPoint, lNbTriangles, lTriangle;
    float                               fTheta, fDTheta, fAlpha, fDAlpha;
    float                               fX, fY, fZ, fNZ;
    GEO_tdst_IndexedTriangle            st_Triangle1, st_Triangle2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    lNbPoints = _l_Long * (_l_Lat - 2) + 2;
#ifdef JADEFUSION
	ERR_X_Assert( lNbPoints <= 65535u );
#else 
	ERR_X_Assert( lNbPoints <= UINT16_MAX );
#endif
	pst_Sphere = GEO_pst_Create(lNbPoints, 0, 1, 0);

    /* Set the 2 pole */
    GEO_SetPoint(pst_Sphere, 0, 0.0f, 0.0f, _f_Radius, 0.0f, 0.0f, 0.0f);
    GEO_SetPoint(pst_Sphere, lNbPoints - 1, 0.0f, 0.0f, -_f_Radius, 0.0f, 0.0f, 0.0f);

    /* Set other points */
    fDTheta = Cf_Pi / (_l_Lat - 1);
    fDAlpha = Cf_2Pi / _l_Long;
    lPoint = 1;

    for(lLatitude = 1, fTheta = fDTheta; lLatitude < _l_Lat - 1; lLatitude++, fTheta += fDTheta)
    {
        fZ = _f_Radius * fOptCos(fTheta);
        fNZ = fOptSin(fTheta);

        for(lLongitude = 0, fAlpha = 0; lLongitude < _l_Long; lLongitude++, fAlpha += fDAlpha)
        {
            fX = _f_Radius * fNZ * fOptCos(fAlpha);
            fY = _f_Radius * fNZ * fOptSin(fAlpha);
            GEO_SetPoint(pst_Sphere, lPoint++, fX, fY, fZ, 0.0f, 0.0f, 0.0f);
        }
    }

    /* Set triangles */
    lNbTriangles = 2 * _l_Long * (_l_Lat - 2);
    GEO_SetElement(pst_Sphere, 0, NULL, lNbTriangles);
    pst_Element = pst_Sphere->dst_Element;

    /* Set triangle of pole */
    L_memset(&st_Triangle1, 0, sizeof(GEO_tdst_IndexedTriangle));
    st_Triangle1.auw_Index[1] = (unsigned short) _l_Long;
    st_Triangle1.auw_Index[2] = 1;
    L_memset(&st_Triangle2, 0, sizeof(GEO_tdst_IndexedTriangle));
    st_Triangle2.auw_Index[0] = (unsigned short) lNbPoints - 1;
    st_Triangle2.auw_Index[2] = (unsigned short) lNbPoints - 2;
    st_Triangle2.auw_Index[1] = (unsigned short) (lNbPoints - 1 - _l_Long);

    for(lPoint = 0; lPoint < _l_Long; lPoint++)
    {
        GEO_SetTriangle(pst_Element, lPoint, &st_Triangle1);
        st_Triangle1.auw_Index[1] = st_Triangle1.auw_Index[2];
        st_Triangle1.auw_Index[2]++;
        GEO_SetTriangle(pst_Element, lNbTriangles - _l_Long + lPoint, &st_Triangle2);
        st_Triangle2.auw_Index[2] = st_Triangle2.auw_Index[1];
        st_Triangle2.auw_Index[1]++;
    }

    /* Set triangle for other latitude */
    lTriangle = _l_Long;
    for(lLatitude = 1; lLatitude < _l_Lat - 2; lLatitude++)
    {
        st_Triangle1.auw_Index[1] = (unsigned short) (lLatitude * _l_Long);
        st_Triangle1.auw_Index[0] = (unsigned short) ((lLatitude - 1) * _l_Long + 1);
        st_Triangle2.auw_Index[0] = (unsigned short) (st_Triangle1.auw_Index[1] + _l_Long);
        st_Triangle2.auw_Index[1] = (unsigned short) (st_Triangle1.auw_Index[0] + _l_Long);

        for(lLongitude = 0; lLongitude < _l_Long; lLongitude++)
        {
            st_Triangle1.auw_Index[2] = st_Triangle2.auw_Index[0];
            GEO_SetTriangle(pst_Element, lTriangle++, &st_Triangle1);
            st_Triangle2.auw_Index[2] = st_Triangle1.auw_Index[0];
            GEO_SetTriangle(pst_Element, lTriangle++, &st_Triangle2);
            st_Triangle1.auw_Index[1] = st_Triangle1.auw_Index[0];
            st_Triangle1.auw_Index[0]++;
            st_Triangle2.auw_Index[0] = st_Triangle2.auw_Index[1];
            st_Triangle2.auw_Index[1]++;
        }
    }

    GEO_ComputeNormals(pst_Sphere);
    return pst_Sphere;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObject_CreateBox(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_Object                     *pst_Box;
    GEO_tdst_ElementIndexedTriangles    *pst_Element;
    LONG                                lNbPoints, lPoint, lNbTriangles;
    GEO_tdst_IndexedTriangle            st_Triangle;
    LONG                                x, y, z;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    lNbPoints = 8;
    pst_Box = GEO_pst_Create(lNbPoints, 0, 1, 0);

    /* Point of cube */
    lPoint = 0;
    for(y = 0; y < 2; y++)
    {
        for(z = 0; z < 2; z++)
            for(x = 0; x < 2; x++) GEO_SetPoint(pst_Box, lPoint++, (float) x, 1.0f - y, (float) z, 0.0f, 0.0f, 0.0f);
    }

    /* Set triangles */
    lNbTriangles = 12;
    GEO_SetElement(pst_Box, 0, NULL, lNbTriangles);
    pst_Element = pst_Box->dst_Element;

    L_memset(&st_Triangle, 0, sizeof(GEO_tdst_IndexedTriangle));

    for(x = 0; x < 12; x++)
    {
        st_Triangle.auw_Index[0] = (unsigned short) Box_gast_Triangle[x].l_Index[0];
        st_Triangle.auw_Index[1] = (unsigned short) Box_gast_Triangle[x].l_Index[1];
        st_Triangle.auw_Index[2] = (unsigned short) Box_gast_Triangle[x].l_Index[2];

        GEO_SetTriangle(pst_Element, x, &st_Triangle);
    }

    GEO_ComputeNormals(pst_Box);
    return pst_Box;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_Object *GEO_pst_DebugObject_CreateTorus(float r1, float r2, LONG segments, LONG sides)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_Object                     *pst_Torus;
    GEO_tdst_ElementIndexedTriangles    *pst_Element;
    LONG                                lNbPoints, lPoint, lNbTriangles;
    float                               fTheta, fDTheta, fAlpha, fDAlpha;
    GEO_tdst_IndexedTriangle            st_Triangle;
    MATH_tdst_Vector                    st_Pt, st_Pt1;
    LONG                                i, j;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    lNbPoints = segments * sides;
    pst_Torus = GEO_pst_Create(lNbPoints, 0, 1, 0);

    /* Compute the points */
    fAlpha = 0.0;
    fDAlpha = Cf_2Pi / segments;
    fDTheta = Cf_2Pi / sides;
    lPoint = 0;
    for(i = 0; i < segments; i++, fAlpha += fDAlpha)
    {
        st_Pt1.x = fOptCos(fAlpha);
        st_Pt1.y = fOptSin(fAlpha);
        st_Pt1.z = 0;

        fTheta = 0.0;
        for(j = 0; j < sides; j++, fTheta += fDTheta)
        {
            st_Pt.x = st_Pt1.x * (r1 + r2 * fOptCos(fTheta));
            st_Pt.y = st_Pt1.y * (r1 + r2 * fOptCos(fTheta));
            st_Pt.z = r2 * fOptSin(fTheta);

            GEO_SetPoint(pst_Torus, lPoint++, st_Pt.x, st_Pt.y, st_Pt.z, 0.0f, 0.0f, 0.0f);
        }
    }

    /* Set triangles */
    lNbTriangles = sides * segments * 2;
    GEO_SetElement(pst_Torus, 0, NULL, lNbTriangles);
    pst_Element = pst_Torus->dst_Element;

    for(lPoint = 0, i = 0; i < segments; i++)
    {
        for(j = 0; j < sides; j++)
        {
            st_Triangle.auw_Index[0] = (unsigned short) (i * sides + j);
            st_Triangle.auw_Index[2] = (unsigned short) ((st_Triangle.auw_Index[0] + sides) % lNbPoints);
            st_Triangle.auw_Index[1] = (unsigned short) (st_Triangle.auw_Index[2] + 1 - ((j == sides - 1) ? sides : 0));
            GEO_SetTriangle(pst_Element, lPoint++, &st_Triangle);

            st_Triangle.auw_Index[2] = st_Triangle.auw_Index[1];
            st_Triangle.auw_Index[1] = (unsigned short) (st_Triangle.auw_Index[0] + 1 - ((j == sides - 1) ? sides : 0));
            GEO_SetTriangle(pst_Element, lPoint++, &st_Triangle);
        }
    }

    GEO_ComputeNormals(pst_Torus);
    return pst_Torus;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
#ifdef JADEFUSION
GEO_tdst_Object *GEO_pst_DebugObject_CreatePortal(void)
{
    return GEO_pst_DebugObjectCreateGeneric
        (
        Portal_gl_NbPoints,
        Portal_gl_NbTriangles,
        Portal_gast_Point,
        Portal_gast_Triangle
        );
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_Dump( GEO_tdst_Object *O )
{
    L_FILE x_File;
    int     i, j, NbT;
    GEO_Vertex *V;
    GEO_tdst_UV *UV;
    GEO_tdst_IndexedTriangle *T;

    x_File = L_fopen( "X:/geodump.txt", "wt" );
    if (!x_File) return;

    /* dump general data */
    fprintf( x_File, "Vertex number   = %d\n", O->l_NbPoints );
    fprintf( x_File, "UV number       = %d\n", O->l_NbUVs );
    fprintf( x_File, "Element number  = %d\n", O->l_NbElements );
    fprintf( x_File, "Triangle number :\n" );
    for ( NbT = 0, i = 0; i < O->l_NbElements; i++ )
    {
        fprintf( x_File, "     Element  %d = %d\n", i, O->dst_Element[i].l_NbTriangles );
        NbT += O->dst_Element[i].l_NbTriangles;
    }
    fprintf( x_File, "          Total = %d\n", NbT );

    fprintf( x_File, "\n================== Vertex ================\n" );
    V = O->dst_Point;
    for (i = 0; i < O->l_NbPoints; i++, V++)
        fprintf( x_File, "%4d  (%4.3f, %4.3f, %4.3f)\n", i, V->x, V->y, V->z );

    fprintf( x_File, "\n================== UV ================\n" );
    UV = O->dst_UV;
    for (i = 0; i < O->l_NbUVs; i++, UV++)
        fprintf( x_File, "%4d  (%2.4f, %2.4f)\n", i, UV->fU, UV->fV );

    for (i = 0; i < O->l_NbElements; i++)
    {
        fprintf( x_File, "\n================== Faces, element %d [%d]================\n", i, O->dst_Element[i].l_MaterialId);
        T = O->dst_Element[i].dst_Triangle;
        for (j = 0; j < O->dst_Element[i].l_NbTriangles; j++, T++)
            fprintf( x_File, "%4d  V(%3d, %3d, %3d) UV(%3d, %3d, %3d)\n", j, T->auw_Index[0], T->auw_Index[1], T->auw_Index[2], T->auw_UV[0], T->auw_UV[1], T->auw_UV[2] );
    }
    L_fclose( x_File );
}

#endif /* ACTIVE_EDITORS */
