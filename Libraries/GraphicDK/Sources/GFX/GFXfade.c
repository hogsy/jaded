/*$T GFXline.c GC! 1.081 09/19/00 09:14:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "GDInterface/GDInterface.h"
#include "GFX/GFX.h"
#include "GFX/GFXfade.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTcolor.h"
#include "SOFT/SOFTstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"

#ifdef _GAMECUBE
#include "GXI_GC/GXI_init.h"
#endif // _GAMECUBE

#ifdef _XENON_RENDER
#include "XenonGraphics/XeVertexShaderManager.h"
#include "XenonGraphics/XeContextManager.h"
#endif

#ifdef PSX2_TARGET

/* mamagouille */
#include "PSX2debug.h"
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Fade Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Fade_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Fade *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Fade *) MEM_p_Alloc(sizeof(GFX_tdst_Fade));
    L_memset( pst_Data, 0, sizeof(GFX_tdst_Fade) );
#ifdef JADEFUSION
	pst_Data->f_Depth = 100.0f * 0.055f;
#else
	pst_Data->f_Depth = 0.055f;
#endif
	return (void *) pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Fade_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Fade       *pst_Data;
    GEO_Vertex          *V;
    ULONG               ul_Color, DM;
    float               time, x1, y1, x2, y2;
    int                 i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
#ifdef _GAMECUBE
	float vp[GX_VIEWPORT_SZ];
#endif // _GAMECUBE


	pst_Data = (GFX_tdst_Fade *) p_Data;

    time = pst_Data->f_Time[5];
    i = 0;
    while ( time > pst_Data->f_Time[i] )
    {
        time -= pst_Data->f_Time[i];
        i++;
        if (i == 5)
            return 0;
    }

#ifdef _GAMECUBE
	GXGetViewportv( vp );
	GXSetViewport(0, 0, (f32)FRAME_BUFFER_WIDTH , (f32)FRAME_BUFFER_HEIGTH, 0.0f, 1.0f);
#endif // _GAMECUBE


    switch (i)
    {
    case 0 : time = 0; break;
    case 1 : time /= pst_Data->f_Time[1]; break;
    case 2 : time = 1; break;
    case 3 : time = 1 - (time / pst_Data->f_Time[3]); break;
    case 4 : time = 0; break;
    }

    ul_Color = LIGHT_ul_Interpol2Colors( pst_Data->ul_ColorStart, pst_Data->ul_ColorEnd, time );
    pst_Data->f_Time[5] += TIM_gf_dt;

	GFX_NeedGeom(4, 4, 2, 1);

    /* set UV */
    GFX_gpst_Geo->dst_UV[0].fU = 0;
    GFX_gpst_Geo->dst_UV[0].fV = 0;
    GFX_gpst_Geo->dst_UV[1].fU = 1;
    GFX_gpst_Geo->dst_UV[1].fV = 0;
    GFX_gpst_Geo->dst_UV[2].fU = 1;
    GFX_gpst_Geo->dst_UV[2].fV = 1;
    GFX_gpst_Geo->dst_UV[3].fU = 0;
    GFX_gpst_Geo->dst_UV[3].fV = 1;

    /* set vertex */
    V = GFX_gpst_Geo->dst_Point;

	x1 = (float) GDI_gpst_CurDD->st_Camera.l_ViewportRealLeft;
	x2 = x1 + GDI_gpst_CurDD->st_Camera.f_Width;

	x1 -= 100;
	x2 += 100;

#ifdef PSX2_TARGET
	y1 = (float) GDI_gpst_CurDD->st_Camera.l_ViewportRealTop-15;
    y2 = y1 + GDI_gpst_CurDD->st_Camera.f_Height+40;
#elif defined(_XBOX)//temporaire
    y1 = (float) GDI_gpst_CurDD->st_Camera.l_ViewportRealTop-15;
    y2 = y1 + GDI_gpst_CurDD->st_Camera.f_Height+40;
#else 
	y1 = (float) GDI_gpst_CurDD->st_Camera.l_ViewportRealTop;
    y2 = y1 + GDI_gpst_CurDD->st_Camera.f_Height;
#endif

	MATH_InitVector( V, x1, y1, pst_Data->f_Depth );
    MATH_InitVector( V + 1, x2, y1, pst_Data->f_Depth );
    MATH_InitVector( V + 2, x2, y2, pst_Data->f_Depth );
    MATH_InitVector( V + 3, x1, y2, pst_Data->f_Depth );

    for (i = 0; i< 4; i++)
	{
		CAM_AssignCameraToDevice
		(
			&GDI_gpst_CurDD->st_ScreenFormat,
			&GDI_gpst_CurDD->st_Camera,
			GDI_gpst_CurDD->st_Device.l_Width,
			GDI_gpst_CurDD->st_Device.l_Height
		);
		GDI_gpst_CurDD->st_GDI.pfnv_SetProjectionMatrix(&GDI_gpst_CurDD->st_Camera);
#ifdef _XENON_RENDER

    D3DXMATRIX * pProjection = g_pXeContextManagerEngine->GetCurrentProjectionMatrix();

    float x = 1.0f / pProjection->_11;
    float y = 1.0f / pProjection->_22;

    MATH_InitVector( V + 0, -x,  y,  1.0f );
    MATH_InitVector( V + 1,  x,  y,  1.0f );
    MATH_InitVector( V + 2,  x, -y,  1.0f );
    MATH_InitVector( V + 3, -x, -y,  1.0f  );
    
    // Transform to world space
        MATH_TransformVertex( V + i, &GDI_gpst_CurDD->st_Camera.st_Matrix, V+i );
#else 
		CAM_2Dto3D(&GDI_gpst_CurDD->st_Camera, VCast(&V[i]), VCast(&V[i]));
#endif
	}
    /* set triangles */
#ifdef JADEFUSION //POPOWARNING ??
	GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[0] = 0;
	GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[1] = 2;
	GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[2] = 1;
    GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[0] = 0;
    GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[1] = 2;
    GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[2] = 1;
	GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[0] = 3;
	GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[1] = 2;
	GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[2] = 0;
    GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[0] = 3;
    GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[1] = 2;
    GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[2] = 0;
#else
	GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[0] = 0;
	GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[1] = 1;
	GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[2] = 2;
    GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[0] = 0;
    GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[1] = 1;
    GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[2] = 2;
	GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[0] = 2;
	GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[1] = 3;
	GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[2] = 0;
    GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[0] = 2;
    GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[1] = 3;
    GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[2] = 0;
#endif
    GFX_gpst_Geo->dul_PointColors[1] = ul_Color;
    GFX_gpst_Geo->dul_PointColors[2] = ul_Color;
    GFX_gpst_Geo->dul_PointColors[3] = ul_Color;
    GFX_gpst_Geo->dul_PointColors[4] = ul_Color;

    M_GFX_CheckGeom();

    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
#ifdef JADEFUSION
	GDI_gpst_CurDD->ul_CurrentDrawMask &= ~( GDI_Cul_DM_ReflectOnWater | GDI_Cul_DM_UseTexture | GDI_Cul_DM_TestBackFace | GDI_Cul_DM_UseAmbient | GDI_Cul_DM_Lighted | GDI_Cul_DM_ZTest );
#else
	GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_UseAmbient | GDI_Cul_DM_Lighted | GDI_Cul_DM_ZTest);
#endif
	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
    GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
    
#ifdef _GAMECUBE
	GXSetViewportv( vp );
#endif // _GAMECUBE
    
    return 1;
}

/*$4
 ***********************************************************************************************************************
    Border fade Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_BorderFade_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Fade *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Fade *) MEM_p_Alloc(sizeof(GFX_tdst_BorderFade));
    L_memset( pst_Data, 0, sizeof(GFX_tdst_BorderFade) );
	return (void *) pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_BorderFade_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_BorderFade         *pst_Data;
    GEO_Vertex                  *V;
    ULONG                       ul_Color, DM;
    float                       f_Factor;
    float                       x1, y1, x2, y2;
    float                       x3, x4, y3, y4;
    float                       xl[6], yl[6];
    int                         i, j, k;
    static float                depth = 0.1f;
    GEO_tdst_IndexedTriangle    *T;
    static int                  ai_Index[4][4] = { {0,8,11,3},{8,9,5,4},{9,1,2,10},{7,6,10,11} };
    static int                  ai_IndexLine[4][3] = { {0,1,3},{1,2,3},{0,3,5}, {3,4,5} };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_BorderFade *) p_Data;

    x1 = (float) GDI_gpst_CurDD->st_Camera.l_ViewportRealLeft;
    x2 = x1 + GDI_gpst_CurDD->st_Camera.f_Width;
    y1 = (float) GDI_gpst_CurDD->st_Camera.l_ViewportRealTop;
    y2 = y1 + GDI_gpst_CurDD->st_Camera.f_Height;

    x3 = pst_Data->x1;
    x4 = pst_Data->x2;
    y3 = pst_Data->y1;
    y4 = pst_Data->y2;

    if (pst_Data->ul_Flags & GFX_C_BorderFade_Format)
    {
        if (pst_Data->ul_Flags & GFX_C_BorderFade_Ratio)
        {
            x3 = x1 + pst_Data->x1 * (x2 - x1);
            x4 = x1 + pst_Data->x2 * (x2 - x1);
            y3 = y1 + pst_Data->y1 * (y2 - y1);
            y4 = y1 + pst_Data->y2 * (y2 - y1);
        }

        if (pst_Data->ul_Flags & GFX_C_BorderFade_Center)
        {
            x4 = (x1 + x2 + x3) / 2;
            x3 = (x1 + x2 - x3) / 2;
            y4 = (y1 + y2 + y3) / 2;
            y3 = (y1 + y2 - y3) / 2;
        }

        pst_Data->x1 = x3;
        pst_Data->x2 = x4;
        pst_Data->y1 = y3;
        pst_Data->y2 = y4;

        pst_Data->ul_Flags &= ~GFX_C_BorderFade_Format;
    }

    if (pst_Data->ul_Flags & GFX_C_BorderFade_Out)
    {
        pst_Data->f_TimeCur += TIM_gf_dt;
        if (pst_Data->f_TimeCur >= pst_Data->f_TimeOut)
            return 0;
        f_Factor = 1.0f - (pst_Data->f_TimeCur / pst_Data->f_TimeOut);
    }
    else if (pst_Data->ul_Flags & GFX_C_BorderFade_In)
    {
        pst_Data->f_TimeCur += TIM_gf_dt;
        if (pst_Data->f_TimeCur < pst_Data->f_TimeWait)
            return 1;
        else if (pst_Data->f_TimeCur < pst_Data->f_TimeWait + pst_Data->f_TimeIn )
            f_Factor = (pst_Data->f_TimeCur - pst_Data->f_TimeWait) / pst_Data->f_TimeIn;
        else
        {
            pst_Data->ul_Flags -= GFX_C_BorderFade_In;
            pst_Data->f_TimeCur = 0;
            f_Factor = 1;
        }
    }
    else
    {
        pst_Data->f_TimeCur = 0;
        f_Factor = 1;
    }

    if (f_Factor != 1)
    {
        x3 = x1 + f_Factor * (x3 - x1);
        x4 = x2 + f_Factor * (x4 - x2);
        y3 = y1 + f_Factor * (y3 - y1);
        y4 = y2 + f_Factor * (y4 - y2);
    }

    ul_Color = pst_Data->ul_Color;

	GFX_NeedGeom(12, 4, 8, 1);

    /* set UV */
    GFX_gpst_Geo->dst_UV[0].fU = 0;
    GFX_gpst_Geo->dst_UV[0].fV = 0;
    GFX_gpst_Geo->dst_UV[1].fU = 1;
    GFX_gpst_Geo->dst_UV[1].fV = 0;
    GFX_gpst_Geo->dst_UV[2].fU = 1;
    GFX_gpst_Geo->dst_UV[2].fV = 1;
    GFX_gpst_Geo->dst_UV[3].fU = 0;
    GFX_gpst_Geo->dst_UV[3].fV = 1;

    /* set vertex */
    V = GFX_gpst_Geo->dst_Point;
    MATH_InitVector( V, x1, y1, depth );
    MATH_InitVector( V + 1, x2, y1, depth );
    MATH_InitVector( V + 2, x2, y2, depth );
    MATH_InitVector( V + 3, x1, y2, depth );

    MATH_InitVector( V + 4, x3, y3, depth );
    MATH_InitVector( V + 5, x4, y3, depth );
    MATH_InitVector( V + 6, x4, y4, depth );
    MATH_InitVector( V + 7, x3, y4, depth );

    MATH_InitVector( V + 8, x3, y1, depth );
    MATH_InitVector( V + 9, x4, y1, depth );
    MATH_InitVector( V + 10, x4, y2, depth );
    MATH_InitVector( V + 11, x3, y2, depth );

    for (i = 0; i < 12; i++)
    {
        CAM_2Dto3D( &GDI_gpst_CurDD->st_Camera, VCast(&V[i]), VCast(&V[i]) );
        GFX_gpst_Geo->dul_PointColors[i + 1] = ul_Color;
    }

    /* set triangles */
    T = GFX_gpst_Geo->dst_Element->dst_Triangle;
    for ( i = 0; i < 4; i++)
    {
	    T->auw_Index[0] = ai_Index[i][0];
	    T->auw_Index[1] = ai_Index[i][1];
	    T->auw_Index[2] = ai_Index[i][2];
        T->auw_UV[0] = 0;
        T->auw_UV[1] = 1;
        T->auw_UV[2] = 2;
        T++;

	    T->auw_Index[0] = ai_Index[i][2];
	    T->auw_Index[1] = ai_Index[i][3];
	    T->auw_Index[2] = ai_Index[i][0];
        T->auw_UV[0] = 2;
        T->auw_UV[1] = 3;
        T->auw_UV[2] = 0;
        T++;
    }

    M_GFX_CheckGeom();

    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_UseAmbient | GDI_Cul_DM_Lighted );
	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
    GDI_gpst_CurDD->ul_CurrentDrawMask = DM;

    if (pst_Data->ul_Flags & GFX_C_BorderFade_Line)
    {
        GFX_NeedGeom(24, 4, 16, 1);

        xl[0] = x3 + pst_Data->dx;
        xl[1] = xl[0] + pst_Data->w;
        xl[2] = xl[0] + pst_Data->sx;
        xl[5] = x4 - pst_Data->dx;
        xl[4] = xl[5] - pst_Data->w;
        xl[3] = xl[5] - pst_Data->sx;

        yl[0] = y3 + pst_Data->dy;
        yl[1] = yl[0] + pst_Data->w;
        yl[2] = yl[0] + pst_Data->sy;
        yl[5] = y4 - pst_Data->dy;
        yl[4] = yl[5] - pst_Data->w;
        yl[3] = yl[5] - pst_Data->sy;

        V = GFX_gpst_Geo->dst_Point;
        MATH_InitVector( V, xl[0], yl[0], depth );
        MATH_InitVector( V + 1, xl[2], yl[0], depth );
        MATH_InitVector( V + 2, xl[2], yl[1], depth );
        MATH_InitVector( V + 3, xl[1], yl[1], depth );
        MATH_InitVector( V + 4, xl[1], yl[2], depth );
        MATH_InitVector( V + 5, xl[0], yl[2], depth );

        MATH_InitVector( V + 6, xl[5], yl[0], depth );
        MATH_InitVector( V + 7, xl[3], yl[0], depth );
        MATH_InitVector( V + 8, xl[3], yl[1], depth );
        MATH_InitVector( V + 9, xl[4], yl[1], depth );
        MATH_InitVector( V + 10, xl[4], yl[2], depth );
        MATH_InitVector( V + 11, xl[5], yl[2], depth );

        MATH_InitVector( V + 12, xl[5], yl[5], depth );
        MATH_InitVector( V + 13, xl[3], yl[5], depth );
        MATH_InitVector( V + 14, xl[3], yl[4], depth );
        MATH_InitVector( V + 15, xl[4], yl[4], depth );
        MATH_InitVector( V + 16, xl[4], yl[3], depth );
        MATH_InitVector( V + 17, xl[5], yl[3], depth );

        MATH_InitVector( V + 18, xl[0], yl[5], depth );
        MATH_InitVector( V + 19, xl[2], yl[5], depth );
        MATH_InitVector( V + 20, xl[2], yl[4], depth );
        MATH_InitVector( V + 21, xl[1], yl[4], depth );
        MATH_InitVector( V + 22, xl[1], yl[3], depth );
        MATH_InitVector( V + 23, xl[0], yl[3], depth );

        for (i = 0; i < 24; i++)
        {
            CAM_2Dto3D( &GDI_gpst_CurDD->st_Camera, VCast(&V[i]), VCast(&V[i]) );
            GFX_gpst_Geo->dul_PointColors[i + 1] = pst_Data->ul_ColorLine;
        } 

        /* set triangles */
        T = GFX_gpst_Geo->dst_Element->dst_Triangle;
        for (k = 0, i = 0; i < 4; i++, k += 6)
        {
            for (j = 0; j < 4; j++ )
            {
                T->auw_Index[0] = k + ai_IndexLine[j][0];
	            T->auw_Index[1] = k + ai_IndexLine[j][1];
	            T->auw_Index[2] = k + ai_IndexLine[j][2];
                T++;
            }
        }

        M_GFX_CheckGeom();
        
        DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
        GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_UseAmbient | GDI_Cul_DM_Lighted );
	    GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
	    GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
        GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
    }

    return 1;
}

/*$4
 ***********************************************************************************************************************
    Diaphragm Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Diaphragm_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Diaphragm *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Diaphragm *) MEM_p_Alloc(sizeof(GFX_tdst_Diaphragm));
    L_memset( pst_Data, 0, sizeof(GFX_tdst_Diaphragm) );
    MATH_InitVector( &pst_Data->st_Center, 0.5, 0.5, 0 );
	return (void *) pst_Data;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Diaphragm_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Diaphragm          *pst_Data;
    GEO_Vertex                  *V, *W;
    ULONG                       ul_Color, DM;
    float                       x1, y1, x2, y2;
    float                       x3, x4, y3, y4, xm, ym;
    float                       d, da, time;
    int                         i, j, k;
    static float                depth = 0.055f;
    GEO_tdst_IndexedTriangle    *T;
    static int                  ai_Index[4][4] = { {0,8,11,3},{8,9,5,4},{9,1,2,10},{7,6,10,11} };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Diaphragm *) p_Data;

    pst_Data->f_Time[4] += TIM_gf_dt;

    time = pst_Data->f_Time[4];
    i = 0;
    while ( time > pst_Data->f_Time[i] )
    {
        time -= pst_Data->f_Time[i];
        i++;
        if (i == 4)
            return 0;
    }

    switch (i)
    {
    case 0 : time = 0; break;
    case 1 : time /= pst_Data->f_Time[1]; break;
    case 2 : time = 1; break;
    case 3 : time = 1 - (time / pst_Data->f_Time[3]); break;
    }

    x1 = (float) GDI_gpst_CurDD->st_Camera.l_ViewportRealLeft;
    x2 = x1 + GDI_gpst_CurDD->st_Camera.f_Width;
    y1 = (float) GDI_gpst_CurDD->st_Camera.l_ViewportRealTop;
    y2 = y1 + GDI_gpst_CurDD->st_Camera.f_Height;

    ul_Color = pst_Data->ul_Color;


    if (time >= 0.99f)
        GFX_NeedGeom(4, 4, 2, 1);
    else
        GFX_NeedGeom(12, 4, 12, 1);

    /* set UV */
    GFX_gpst_Geo->dst_UV[0].fU = 0;
    GFX_gpst_Geo->dst_UV[0].fV = 0;
    GFX_gpst_Geo->dst_UV[1].fU = 1;
    GFX_gpst_Geo->dst_UV[1].fV = 0;
    GFX_gpst_Geo->dst_UV[2].fU = 1;
    GFX_gpst_Geo->dst_UV[2].fV = 1;
    GFX_gpst_Geo->dst_UV[3].fU = 0;
    GFX_gpst_Geo->dst_UV[3].fV = 1;

    xm = pst_Data->st_Center.x * ( x1 + x2 );
    ym = pst_Data->st_Center.y * ( y1 + y2 );

    V = GFX_gpst_Geo->dst_Point;
    MATH_InitVector( V + 0, x1, y1, depth );
    MATH_InitVector( V + 1, x2, y1, depth );
    MATH_InitVector( V + 2, x1, y2, depth );
    MATH_InitVector( V + 3, x2, y2, depth );
    
    for (i = 0; i < 4; i++)
    {
        CAM_2Dto3D( &GDI_gpst_CurDD->st_Camera, VCast(&V[i]), VCast(&V[i]) );
        GFX_gpst_Geo->dul_PointColors[i + 1] = ul_Color;
    }

    if (time >= 0.99f)
    {
        T = GFX_gpst_Geo->dst_Element->dst_Triangle;

        T->auw_Index[0] = 2;
        T->auw_Index[1] = 1;
	    T->auw_Index[2] = 0;
        T->auw_UV[0] = 0;
        T->auw_UV[1] = 1;
        T->auw_UV[2] = 2;
        T++;

        T->auw_Index[0] = 2;
	    T->auw_Index[1] = 3;
	    T->auw_Index[2] = 1;
        T->auw_UV[0] = 2;
        T->auw_UV[1] = 3;
        T->auw_UV[2] = 1;
    }
    else
    {
        x3 = ((xm - x1) < (x2 - xm)) ? (x2 - xm) : (xm - x1);
        y3 = ((ym - y1) < (y2 - ym)) ? (y2 - ym) : (ym - y1);
        d = (1 - time) * fOptSqrt(x3*x3 + y3*y3);
        da = (Cf_PiBy4 / 2) + (time * Cf_Pi);

        V = GFX_gpst_Geo->dst_Point + 4;
        for (i = 0; i < 8; i++)
        {
            //x3 = d * fOptCos( i * Cf_PiBy4 + (Cf_PiBy4 / 2) );
            //y3 = d * fOptSin( i * Cf_PiBy4 + (Cf_PiBy4 / 2) );
            x3 = d * fOptCos( i * Cf_PiBy4 + da );
            y3 = d * fOptSin( i * Cf_PiBy4 + da );
            MATH_InitVector( V, x3, y3, depth );
            V++;
        }

        V = GFX_gpst_Geo->dst_Point + 11;
        W = GFX_gpst_Geo->dst_Point + 4;
        T = GFX_gpst_Geo->dst_Element->dst_Triangle;
        for (i = 4; i < 12; i++)
        {
            j = ((V->x > 0) ? 1 : 0) | ((V->y > 0) ? 2 : 0);
            k = ((W->x > 0) ? 1 : 0) | ((W->y > 0) ? 2 : 0);
            if (j != k)
            {
                T->auw_Index[0] = (V - GFX_gpst_Geo->dst_Point);
	            T->auw_Index[1] = j;
	            T->auw_Index[2] = k;
                T->auw_UV[0] = 0;
                T->auw_UV[1] = 1;
                T->auw_UV[2] = 2;
                T++;
            }
            T->auw_Index[0] = (V - GFX_gpst_Geo->dst_Point);
	        T->auw_Index[1] = k;
	        T->auw_Index[2] = (W - GFX_gpst_Geo->dst_Point);
            T->auw_UV[0] = 0;
            T->auw_UV[1] = 1;
            T->auw_UV[2] = 2;
            T++;

            V = W++;
        }

        V = GFX_gpst_Geo->dst_Point;
        for (i = 4; i < 12; i++)
        {
            V[i].x += xm;
            V[i].y += ym;
            CAM_2Dto3D( &GDI_gpst_CurDD->st_Camera, VCast(&V[i]), VCast(&V[i]) );
            GFX_gpst_Geo->dul_PointColors[i + 1] = ul_Color;
        }
    }

    M_GFX_CheckGeom();

    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_UseAmbient | GDI_Cul_DM_Lighted );
	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
    GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
    return 1;

    GFX_NeedGeom(32, 4, 16, 1);

    /* set UV */
    GFX_gpst_Geo->dst_UV[0].fU = 0;
    GFX_gpst_Geo->dst_UV[0].fV = 0;
    GFX_gpst_Geo->dst_UV[1].fU = 1;
    GFX_gpst_Geo->dst_UV[1].fV = 0;
    GFX_gpst_Geo->dst_UV[2].fU = 1;
    GFX_gpst_Geo->dst_UV[2].fV = 1;
    GFX_gpst_Geo->dst_UV[3].fU = 0;
    GFX_gpst_Geo->dst_UV[3].fV = 1;

    xm = 0.5f * ( x1 + x2 );
    ym = 0.5f * ( y1 + y2 );
    //xm = pst_Data->st_Center.x * ( x1 + x2 );
    //ym = pst_Data->st_Center.y * ( y1 + y2 );
    d = 0.5f * fOptSqrt( (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) );
    

    V = GFX_gpst_Geo->dst_Point;
    T = GFX_gpst_Geo->dst_Element->dst_Triangle;
    for (i = 0; i < 8; i++)
    {
        x3 = d * fOptCos( i * Cf_PiBy4);
        y3 = d * fOptSin( i * Cf_PiBy4);

        x4 = d * fOptCos( i * Cf_PiBy4 - time * Cf_PiBy2 );
        y4 = d * fOptSin( i * Cf_PiBy4 - time * Cf_PiBy2 );

        MATH_InitVector( V, xm + x3, ym + y3, depth );
        MATH_InitVector( V + 1, xm + x3 + x4, ym + y3 + y4, depth );
        MATH_InitVector( V + 2, xm + x3 + x4 + y4, ym + y3 + y4 - x4, depth );
        MATH_InitVector( V + 3, xm + x3 + y4, ym + y3 - x4, depth );
        V+=4;

        T->auw_Index[0] = i * 4 + 0;
	    T->auw_Index[1] = i * 4 + 1;
	    T->auw_Index[2] = i * 4 + 2;
        T->auw_UV[0] = 0;
        T->auw_UV[1] = 1;
        T->auw_UV[2] = 2;
        T++;

	    T->auw_Index[0] = i * 4 + 2;
	    T->auw_Index[1] = i * 4 + 3;
	    T->auw_Index[2] = i * 4 + 0;
        T->auw_UV[0] = 2;
        T->auw_UV[1] = 3;
        T->auw_UV[2] = 0;
        T++;
    }

    V = GFX_gpst_Geo->dst_Point;
    for (i = 0; i < 32; i++)
    {
        CAM_2Dto3D( &GDI_gpst_CurDD->st_Camera, VCast(&V[i]), VCast(&V[i]) );
        GFX_gpst_Geo->dul_PointColors[i + 1] = ul_Color;
    }

    M_GFX_CheckGeom();
    
    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_UseAmbient | GDI_Cul_DM_Lighted );
	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
    GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
    return 1;
}


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

