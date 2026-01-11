/*$T WAYpoint.c GC!1.55 12/28/99 14:09:18 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTHelper.h"
#include "WAYpoint/WAYpoint.h"
#include "WAYpoint/WAYlink.h"
#include "BASe/BAStypes.h"
#include "MATerial/MATSingle.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif


/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

GRO_tdst_Struct WAY_gst_GroStruct;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *WAY_p_CreateFromMad(void *p)
{
    return &WAY_gst_GroStruct;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_Free(GRO_tdst_Struct *_pst_Struct)
{
    return;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG WAY_l_HasSomethingToRender
(
    GRO_tdst_Visu           *_pst_Visu,
    GEO_tdst_Object         **ppst_PickableObject
)
{
	int		i_Index;
    if(!(GDI_gpst_CurDD->ul_DisplayFlags & (GDI_Cul_DF_DisplayWaypoint | GDI_Cul_DF_DisplayWaypointInNetwork))) return 0;
    if ( GDI_gpst_CurDD->pst_Helpers->l_Picked != -1 )
    {
        if (GDI_gpst_CurDD->ul_WPFlags & 1) return 0;
        if ( (GDI_gpst_CurDD->ul_WPFlags & 2) && (GDI_gpst_CurDD->pst_CurrentGameObject->ul_EditorFlags & OBJ_C_EditFlags_Selected) ) return 0;
    }

    /* Display bounding volume of waypoint */
    if(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_WaypointBoundingVolume)
    {
        if(OBJ_BV_IsSphere(GDI_gpst_CurDD->pst_CurrentGameObject->pst_BV))
            *ppst_PickableObject = GEO_pst_DebugObject_Get(GEO_DebugObject_Sphere);
        else
            *ppst_PickableObject = GEO_pst_DebugObject_Get(GEO_DebugObject_Box);
    }

    /* Display waypoint */
    else
    {
		i_Index = GDI_gpst_CurDD->pst_CurrentGameObject->ul_InvisibleObjectIndex;
		if ( (i_Index == 0 ) || (i_Index > GEO_DebugObject_LastInvisible - GEO_DebugObject_Invisible))
			*ppst_PickableObject = GEO_pst_DebugObject_Get(GEO_DebugObject_Waypoint);
		else
		{
			if( !GDI_WPShowStatusOn( GDI_gpst_CurDD, i_Index ) ) 
				return FALSE;
			*ppst_PickableObject = GEO_pst_DebugObject_Get(GEO_DebugObject_Invisible + i_Index);
		}
    }
    return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_Render(OBJ_tdst_GameObject *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG				C, DM, DI;
    LONG                l_Object;
    int					i_Index;
    MATH_tdst_Matrix    *M;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    DI = GDI_gpst_CurDD->ul_DisplayInfo; 

    if(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_WaypointBoundingVolume)
    {
        /* Display bounding volume of waypoint */
        if(OBJ_BV_IsSphere(_pst_Node->pst_BV))
            l_Object = GEO_DebugObject_Sphere;
        else
            l_Object = GEO_DebugObject_Box;
        GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_RenderingTransparency;
        
        MAT_gst_DefaultMatMTLevel.ul_Flags = (MAT_Cc_Op_Add << MAT_Cc_Blending_SHIFT);
        MAT_gst_DefaultSingleMaterial.pst_FirstLevel = &MAT_gst_DefaultMatMTLevel;
        MAT_gst_DefaultMatMTLevel.s_TextureId = -1;
    }
    else
    {
        /* Display waypoint */
        l_Object = GEO_DebugObject_Waypoint;
        i_Index = GDI_gpst_CurDD->pst_CurrentGameObject->ul_InvisibleObjectIndex;
		if ( (i_Index == 0 ) || (i_Index > GEO_DebugObject_LastInvisible - GEO_DebugObject_Invisible))
			l_Object = GEO_DebugObject_Waypoint;
		else
		{
			if( !GDI_WPShowStatusOn( GDI_gpst_CurDD, i_Index ) ) 
				return;
			l_Object = GEO_DebugObject_Invisible + i_Index;
		}
    }

    C = (_pst_Node->ul_EditorFlags & OBJ_C_EditFlags_Selected) ? 1 : 0;
    C = GEO_ul_DebugObject_GetColor(l_Object, C);
    GEO_DebugObject_SetLight(C);

    /* Display waypoint */
    M = OBJ_pst_GetAbsoluteMatrix(_pst_Node);

    
	DM = GDI_Cul_DM_All - GDI_Cul_DM_UseAmbient - GDI_Cul_DM_Fogged - GDI_Cul_DM_MaterialColor;
    GEO_DebugObject_Draw(GDI_gpst_CurDD, l_Object, DM, _pst_Node->ul_EditorFlags, M);

    MAT_gst_DefaultMatMTLevel.ul_Flags = 0;
    GDI_gpst_CurDD->ul_DisplayInfo = DI;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG WAY_PushSpecialMatrix(OBJ_tdst_GameObject *_pst_Node)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    st_Pos;
    MATH_tdst_Matrix    Scale;
    float               f_Scale;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_WaypointBoundingVolume)
    {
        if(OBJ_BV_IsSphere(_pst_Node->pst_BV))
        {
            /* Display waypoint */
            GEO_DebugObject_PushSphereMatrix
            (
                &GDI_gpst_CurDD->st_MatrixStack,
                &MATH_gst_NulVector,
                OBJ_f_BV_GetRadius(_pst_Node->pst_BV)
            );
        }
        else
        {
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            MATH_tdst_Vector    st_Center, st_Max, st_Min;
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

            /*
             * The object matrix has already been pushed ... We must pop
             * it because the AABBox is already in the Global coordinate
             * system.
             */
            SOFT_l_MatrixStack_Pop(&GDI_gpst_CurDD->st_MatrixStack);

            MATH_InitVector(&st_Center, 0.0f, 0.0f, 0.0f);

            /* We add a Dummy Identity Matrix. */
            GEO_DebugObject_PushSphereMatrix(&GDI_gpst_CurDD->st_MatrixStack, &st_Center, 1.0f);

            MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(_pst_Node->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_Node));
            MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(_pst_Node->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_Node));

            /* Display waypoint */
            GEO_DebugObject_PushBoxMatrix(&GDI_gpst_CurDD->st_MatrixStack, &st_Min, &st_Max);
        }

        return 1;
    }
    
    if( GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_Proportionnal )
    {
        f_Scale = 0.1f * fNormalTan( GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2);
        MATH_TransformVertex(&st_Pos, GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix, &MATH_gst_NulVector);
        MATH_SetIdentityMatrix(&Scale);
        if(st_Pos.z > 0)
        {
            f_Scale *= st_Pos.z;
            if (f_Scale < GDI_gpst_CurDD->f_WPSize) f_Scale = GDI_gpst_CurDD->f_WPSize;
        }
    }
    else
    {
        f_Scale = GDI_gpst_CurDD->f_WPSize;
    }

    if (f_Scale != 1.0f)
    {
        MATH_SetIdentityMatrix(&Scale);
        MATH_SetZoom(&Scale, f_Scale);
        SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, &Scale);
        return 1;
    }
    
    return 0;
    
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WAY_Init(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~*/
    GRO_tdst_Interface  *i;
    /*~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Init light object interface */
    i = &GRO_gast_Interface[GRO_Waypoint];

#if defined (__cplusplus)
    i->pfn_Destroy = (void (__cdecl *)(void *))WAY_Free;
#else
	i->pfn_Destroy = WAY_Free;
#endif

#ifdef ACTIVE_EDITORS
#if defined (__cplusplus)
    i->pfnl_HasSomethingToRender = (LONG (__cdecl *)(void *,void *))WAY_l_HasSomethingToRender;
    i->pfn_Render = (void (__cdecl *)(void *))WAY_Render;
    i->pfnl_PushSpecialMatrix = (LONG (__cdecl *)(void *))WAY_PushSpecialMatrix;
#else
	i->pfnl_HasSomethingToRender = WAY_l_HasSomethingToRender;
    i->pfn_Render = WAY_Render;
    i->pfnl_PushSpecialMatrix = WAY_PushSpecialMatrix;
#endif
	i->pfnp_CreateFromMad = WAY_p_CreateFromMad;
#endif
    WAY_gst_GroStruct.i = i;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
