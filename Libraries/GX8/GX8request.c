/*$T GX8request.c GC! 1.081 01/17/02 10:15:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Gx8request.h"

#if defined PSX2_TARGET && defined __cplusplus
extern "C"
{
#endif

#include <xtl.h>
#include <D3D8.h>

#ifndef _XBOX
#include "ddraw.h"
#endif
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "Gx8init.h"
#include "Gx8renderstate.h"

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BAStypes.h"
#include "TEXture/TEXfile.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEXstruct.h"
#include "SOFT/SOFTcolor.h"

#include "Gx8debugfct.h"

#include "Gx8tex.h"
#include "Gx8color.h"

#include "BASe\BENch\Bench.h"
#include "RASter/Gx8_CheatFlags.h"

#ifdef _XBOX
#include <xgraphics.h>
#endif // _XBOX

#include "Gx8Light.h"
/*$4
 ***********************************************************************************************************************
    Full screen mode
 ***********************************************************************************************************************
 */

extern HINSTANCE	MAI_gh_MainInstance;

LONG				Gx8_l_FullScreenMode = 0;
RECT				Gx8_gst_WindowPos;
HWND				Gx8_gh_FullScreenWnd;
HWND				Gx8_gh_WindowedHwndSave;

bool				Gx8_Update_Video;
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_SetFogParams(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_FogParams *_pst_Fog)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static int				sFM[3] = { D3DFOG_LINEAR, D3DFOG_EXP, D3DFOG_EXP2 };
	Gx8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = GX8_M_SD(_pst_DD);

#ifdef GX8_BENCH
	if (!g_iNoFog)
	{
#endif
	if(_pst_Fog->c_Flag & SOFT_C_FogActive)
	{
//		pst_SD->ul_FogColor = Gx8_M_ConvertColor((_pst_Fog->ul_Color & 0xfefefefe)>>1);
		pst_SD->ul_FogColor = Gx8_M_ConvertColor((_pst_Fog->ul_Color));
		pst_SD->ul_FogColorBlack = 0;
		pst_SD->ul_FogColorOn2 = COLOR_ul_Mul(pst_SD->ul_FogColor, 0.5f);

		Gx8_M_RenderState(pst_SD, D3DRS_FOGTABLEMODE, sFM[_pst_Fog->c_Mode]);
		Gx8_M_RenderState(pst_SD, D3DRS_FOGDENSITY, *(ULONG *) &_pst_Fog->f_Density);
		pst_SD->ulFogState = 1;

		//Gx8_M_RenderState(pst_SD, D3DRS_FOGCOLOR, Gx8_ConvertChrominancePixel(pst_SD->ul_FogColor));
		Gx8_M_RenderState(pst_SD, D3DRS_FOGCOLOR, pst_SD->ul_FogColor);
		Gx8_M_RenderState(pst_SD, D3DRS_FOGSTART, *(ULONG *) &_pst_Fog->f_Start);
		Gx8_M_RenderState(pst_SD, D3DRS_FOGEND, *(ULONG *) &_pst_Fog->f_End);
        Gx8_RS_Fogged(pst_SD, TRUE);
	}
	else
	{
		pst_SD->ulFogState = 0;
        Gx8_RS_Fogged(pst_SD, FALSE);
	}
#ifdef GX8_BENCH
	}
	else
	{
		pst_SD->ulFogState = 0;
        Gx8_RS_Fogged(pst_SD, FALSE);
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_DrawProjectedTriangle(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Vertex *_pst_TV)
{

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_Draw2DTriangle(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Vertex *_pst_TV)
{

}

/*
 =======================================================================================================================
    Aim:    Draw a point
 =======================================================================================================================
 */
void Gx8_DrawPoint(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_DrawPointMin(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
}

/*
 =======================================================================================================================
    Aim:    draw a point with a given size
 =======================================================================================================================
 */
void Gx8_DrawPoint_Size(GDI_tdst_DisplayData *_pst_DD, void **peewee)
{
}

/*
 =======================================================================================================================
    Aim:    draw a point with a given size and color
 =======================================================================================================================
 */
DWORD FtoDW(float f)
{
	return *((DWORD *) &f);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_DrawPointEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawPointEx *_pst_Data)
{

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ULONG					ulBlendingMode;
	Gx8VertexFormat			*pVertexBuffer;
	float					fSize;
	Gx8_tdst_SpecificData	*pst_SD;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	pst_SD = GX8_M_SD(_pst_DD);

	Gx8_M_RenderState(pst_SD, D3DRS_POINTSPRITEENABLE, 1/*TRUE*/);
	Gx8_M_RenderState(pst_SD, D3DRS_POINTSCALEENABLE, 1/*TRUE*/);
	Gx8_M_RenderState(pst_SD, D3DRS_POINTSIZE, FtoDW(100.0f));
	Gx8_M_RenderState(pst_SD, D3DRS_POINTSIZE_MIN, FtoDW(4.00f));
	Gx8_M_RenderState(pst_SD, D3DRS_POINTSIZE_MAX, FtoDW(5.00f));
	Gx8_M_RenderState(pst_SD, D3DRS_POINTSCALE_A, FtoDW(0.00f));
	Gx8_M_RenderState(pst_SD, D3DRS_POINTSCALE_B, FtoDW(0.00f));
	Gx8_M_RenderState(pst_SD, D3DRS_POINTSCALE_C, FtoDW(1.00f));

    Gx8_vBeginRenderUsingTheBigVertexBuffer(1, (unsigned char **) &pVertexBuffer);

	*(MATH_tdst_Vector *) &pVertexBuffer[0] = *_pst_Data->A;
	pVertexBuffer[0].Color = Gx8_M_ConvertColor(_pst_Data->ul_Color);

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode, 0);
	fSize = _pst_Data->f_Size * 1.0f;

    Gx8_vEndRenderUsingTheBigVertexBuffer(1, D3DPT_POINTLIST);

}

/*
 =======================================================================================================================
    Aim:    draw a point with a given size and color
 =======================================================================================================================
 */
void Gx8_PointSize(GDI_tdst_DisplayData *_pst_DD, float _f_Size)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a line
 =======================================================================================================================
 */
void Gx8_DrawLine(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector **v)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ULONG					ulBlendingMode;
	Gx8VertexFormat			*pVertexBuffer;
	Gx8_tdst_SpecificData	*pst_SD;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	pst_SD = GX8_M_SD(_pst_DD);

    Gx8_vBeginRenderUsingTheBigVertexBuffer(2, (unsigned char **) &pVertexBuffer);
	pVertexBuffer[0].X = ((float *) v[0])[0];
	pVertexBuffer[0].Y = ((float *) v[0])[1];
	pVertexBuffer[0].Z = ((float *) v[0])[2];
	pVertexBuffer[0].Color = 0xffffffff;

	pVertexBuffer[1].X = ((float *) v[1])[0];
	pVertexBuffer[1].Y = ((float *) v[1])[1];
	pVertexBuffer[1].Z = ((float *) v[1])[2];
	pVertexBuffer[1].Color = 0xffffffff;

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode, 0);

    Gx8_vEndRenderUsingTheBigVertexBuffer(1, D3DPT_LINELIST);

}

/*
 =======================================================================================================================
    Aim:    Draw a line with width and color given
 =======================================================================================================================
 */
void Gx8_DrawLineEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawLineEx *_pst_Data)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ULONG					ulBlendingMode;
	Gx8VertexFormat			*pVertexBuffer;
	Gx8_tdst_SpecificData	*pst_SD;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	pst_SD = GX8_M_SD(_pst_DD);

    Gx8_vBeginRenderUsingTheBigVertexBuffer(2, (unsigned char **) &pVertexBuffer);

	*(MATH_tdst_Vector *) &pVertexBuffer[0] = *_pst_Data->A;
	*(MATH_tdst_Vector *) &pVertexBuffer[1] = *_pst_Data->B;

	pVertexBuffer[0].Color = Gx8_M_ConvertColor(_pst_Data->ul_Color);
	pVertexBuffer[1].Color = Gx8_M_ConvertColor(_pst_Data->ul_Color);


	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode, 0);

    Gx8_vEndRenderUsingTheBigVertexBuffer(1, D3DPT_LINELIST);

}

/*
 =======================================================================================================================
    Aim:    Draw a line with width and color given
 =======================================================================================================================
 */
void Gx8_LineSize(GDI_tdst_DisplayData *_pst_DD, float _f_Size)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a quad
 =======================================================================================================================
 */
void Gx8_DrawQuad(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector **v)
{
}

static PointSprite = 0;

/*
 =======================================================================================================================
    Aim:    Draw a Sprite
 =======================================================================================================================
 */
void Gx8_DrawSprite(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	Gx8_tdst_SpecificData	*pst_SD;
	static float			tf_UV[5] = { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f };
	ULONG					ul_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = GX8_M_SD(_pst_DD);
	if(pst_SD->pst_LockedVertex == NULL) return;

    if ( pst_SD->ul_LVLeft == 0)
        return;

	ul_Color = Gx8_M_ConvertColor(*(ULONG *) &v[4].x);

    if ( PointSprite )
    {
        *(GEO_Vertex *) pst_SD->pst_LockedVertex = v[0];
	    pst_SD->pst_LockedVertex->Color = ul_Color;
	    pst_SD->pst_LockedVertex++;
        pst_SD->ul_LVLeft --;
    }
    else
    {
#ifdef _XBOX
	    *(GEO_Vertex *) pst_SD->pst_LockedVertex = v[0];
	    pst_SD->pst_LockedVertex->Color = ul_Color;
	    pst_SD->pst_LockedVertex->fU = tf_UV[3];
	    pst_SD->pst_LockedVertex->fV = tf_UV[4];
	    pst_SD->pst_LockedVertex++;

	    *(GEO_Vertex *) pst_SD->pst_LockedVertex = v[1];
	    pst_SD->pst_LockedVertex->Color = ul_Color;
	    pst_SD->pst_LockedVertex->fU = tf_UV[2];
	    pst_SD->pst_LockedVertex->fV = tf_UV[3];
	    pst_SD->pst_LockedVertex++;

	    *(GEO_Vertex *) pst_SD->pst_LockedVertex = v[2];
	    pst_SD->pst_LockedVertex->Color = ul_Color;
	    pst_SD->pst_LockedVertex->fU = tf_UV[1];
	    pst_SD->pst_LockedVertex->fV = tf_UV[2];
	    pst_SD->pst_LockedVertex++;

	    *(GEO_Vertex *) pst_SD->pst_LockedVertex = v[3];
	    pst_SD->pst_LockedVertex->Color = ul_Color;
	    pst_SD->pst_LockedVertex->fU = tf_UV[0];
	    pst_SD->pst_LockedVertex->fV = tf_UV[1];
	    pst_SD->pst_LockedVertex++;

        pst_SD->ul_LVLeft--;
#else _XBOX
	    *(GEO_Vertex *) pst_SD->pst_LockedVertex = v[0];
	    pst_SD->pst_LockedVertex->Color = ul_Color;
	    pst_SD->pst_LockedVertex->fU = tf_UV[3];
	    pst_SD->pst_LockedVertex->fV = tf_UV[4];
	    pst_SD->pst_LockedVertex++;

	    *(GEO_Vertex *) pst_SD->pst_LockedVertex = v[1];
	    pst_SD->pst_LockedVertex->Color = ul_Color;
	    pst_SD->pst_LockedVertex->fU = tf_UV[2];
	    pst_SD->pst_LockedVertex->fV = tf_UV[3];
	    pst_SD->pst_LockedVertex++;

	    *(GEO_Vertex *) pst_SD->pst_LockedVertex = v[2];
	    pst_SD->pst_LockedVertex->Color = ul_Color;
	    pst_SD->pst_LockedVertex->fU = tf_UV[1];
	    pst_SD->pst_LockedVertex->fV = tf_UV[2];
	    pst_SD->pst_LockedVertex++;

	    *(GEO_Vertex *) pst_SD->pst_LockedVertex = v[0];
	    pst_SD->pst_LockedVertex->Color = ul_Color;
	    pst_SD->pst_LockedVertex->fU = tf_UV[3];
	    pst_SD->pst_LockedVertex->fV = tf_UV[4];
	    pst_SD->pst_LockedVertex++;

	    *(GEO_Vertex *) pst_SD->pst_LockedVertex = v[2];
	    pst_SD->pst_LockedVertex->Color = ul_Color;
	    pst_SD->pst_LockedVertex->fU = tf_UV[1];
	    pst_SD->pst_LockedVertex->fV = tf_UV[2];
	    pst_SD->pst_LockedVertex++;

	    *(GEO_Vertex *) pst_SD->pst_LockedVertex = v[3];
	    pst_SD->pst_LockedVertex->Color = ul_Color;
	    pst_SD->pst_LockedVertex->fU = tf_UV[0];
	    pst_SD->pst_LockedVertex->fV = tf_UV[1];
	    pst_SD->pst_LockedVertex++;

        pst_SD->ul_LVLeft -= 2;
#endif _XBOX
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_BeforeDrawSprite(GDI_tdst_DisplayData *_pst_DD, ULONG _ul_NbSprites)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ulBlendingMode;
	Gx8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ulBlendingMode = 0;


    /*
    if (GetAsyncKeyState( VK_SHIFT ) < 0)
    {
        PointSprite = 1 - PointSprite;
        while( GetAsyncKeyState( VK_SHIFT ) < 0);
    }
    */

    if(_ul_NbSprites == 0) return;

	pst_SD = GX8_M_SD(_pst_DD);

#ifdef ACTIVE_EDITORS
	if(!(_pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired))
	{
		Gx8_RS_LineWidth(&pst_SD->st_RS, 1.0f);
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
    	Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode, 0);
	}

	Gx8_RS_DrawWired(pst_SD, !(_pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired));
#else
	Gx8_RS_DrawWired(pst_SD, 0);
#endif
    Gx8_RS_CullFace(pst_SD, GX8_C_CULLFACE_NORMAL); // GX8_C_CULLFACE_OFF

    if ( PointSprite )
    {
        pst_SD->ul_LVLeft = pst_SD->ul_LockedVertex = _ul_NbSprites;
        Gx8_vBeginRenderUsingTheBigVertexBuffer(_ul_NbSprites, (unsigned char **) &pst_SD->pst_LockedVertex);
    }
    else
    {
#ifdef _XBOX
	    pst_SD->ul_LVLeft = pst_SD->ul_LockedVertex = _ul_NbSprites;
        Gx8_vBeginRenderUsingTheBigVertexBuffer(_ul_NbSprites * 4, (unsigned char **) &pst_SD->pst_LockedVertex);
#else _XBOX
	    pst_SD->ul_LVLeft = pst_SD->ul_LockedVertex = _ul_NbSprites * 2;
        Gx8_vBeginRenderUsingTheBigVertexBuffer(_ul_NbSprites * 6, (unsigned char **) &pst_SD->pst_LockedVertex);
#endif _XBOX
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_AfterDrawSprite(GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	Gx8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = GX8_M_SD(_pst_DD);
	if(pst_SD->pst_LockedVertex == NULL) return;

    if ( PointSprite )
    {
        Gx8_M_RenderState(pst_SD, D3DRS_POINTSPRITEENABLE, 1/*TRUE*/);
	    Gx8_M_RenderState(pst_SD, D3DRS_POINTSCALEENABLE, 1/*TRUE*/);
	    Gx8_M_RenderState(pst_SD, D3DRS_POINTSIZE,     FtoDW(0.08f) );
	    Gx8_M_RenderState(pst_SD, D3DRS_POINTSIZE_MIN, FtoDW(0.00f));
        Gx8_M_RenderState(pst_SD, D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
        Gx8_M_RenderState(pst_SD, D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
        Gx8_M_RenderState(pst_SD, D3DRS_POINTSCALE_C,  FtoDW(1.00f) );

        Gx8_vEndRenderUsingTheBigVertexBuffer(pst_SD->ul_LockedVertex, D3DPT_POINTLIST);
    }
    else
    {
#ifdef _XBOX
        Gx8_vEndRenderUsingTheBigVertexBuffer(pst_SD->ul_LockedVertex, D3DPT_QUADLIST);
#else _XBOX
        Gx8_vEndRenderUsingTheBigVertexBuffer(pst_SD->ul_LockedVertex, D3DPT_TRIANGLELIST);
#endif _XBOX
    }
    pst_SD->pst_LockedVertex = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_Display169BlackBand(GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	Gx8_tdst_SpecificData	*pst_SD;
	D3DRECT rects[2] = { { 0, 0, 640, 70 }, { 0, 410, 640, 480 } };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = GX8_M_SD(_pst_DD);

	IDirect3DDevice8_Clear( pst_SD->mp_D3DDevice, 
							2, 
							rects, 
							D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
							D3DCOLOR_ARGB( 255, 0, 0, 0 ),
							1.0f,
							0 );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void Gx8_ReloadTexture(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_ReloadTextureParams *_pst_RTP)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Gx8_tdst_SpecificData	*pst_SD;
	TEX_tdst_Data			*pst_Tex;
    HRESULT                 hr;
	IDirect3DTexture8		*Gx8_TextureHard;
    IDirect3DSurface8	    *Gx8_Surface;
	RECT					stSrcRect;
	ULONG					*pul_Buffer;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	pst_SD = GX8_M_SD(_pst_DD);
	pst_Tex = &TEX_gst_GlobalList.dst_Texture[_pst_RTP->w_Texture];

	if (pst_SD->dul_Texture[_pst_RTP->w_Texture]==NULL)
		hr = IDirect3DDevice8_CreateTexture (pst_SD->mp_D3DDevice, pst_Tex->w_Width, pst_Tex->w_Height, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &Gx8_TextureHard);
	else
		Gx8_TextureHard = (IDirect3DTexture8 *) pst_SD->dul_Texture[_pst_RTP->w_Texture];

	pul_Buffer = (ULONG *)_pst_RTP->pc_Data;

	IDirect3DTexture8_GetSurfaceLevel(Gx8_TextureHard, 0, &Gx8_Surface);

	stSrcRect.left = stSrcRect.top = 0;
	stSrcRect.right = pst_Tex->w_Width;
	stSrcRect.bottom = pst_Tex->w_Height;

#ifdef _XBOX
	if (Gx8_Update_Video && _pst_RTP->IsAlreadySwizzled != -1)
	{
		Gx8_PlayVideoInTexture(pst_SD, Gx8_TextureHard);
	}
	else
		D3DXLoadSurfaceFromMemory (Gx8_Surface, NULL, NULL, pul_Buffer, D3DFMT_LIN_A8R8G8B8, pst_Tex->w_Width*4, NULL, &stSrcRect, D3DX_FILTER_POINT, 0);
	
	Gx8_Update_Video=false;

	//D3DXFilterTexture (Gx8_TextureHard, NULL, 0, D3DX_FILTER_LINEAR);
#else
//	D3DXLoadSurfaceFromMemory (Gx8_Surface, NULL, NULL, pul_Buffer, D3DFMT_A8R8G8B8, pst_Tex->w_Width*4, NULL, &stSrcRect, D3DX_FILTER_POINT, 0);
//	D3DXFilterTexture (Gx8_TextureHard, NULL, 0, D3DX_FILTER_LINEAR);
#endif
	if(Gx8_Surface)
		IDirect3DSurface8_Release(Gx8_Surface);
	(IDirect3DTexture8 *) pst_SD->dul_Texture[_pst_RTP->w_Texture] = Gx8_TextureHard;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_ReloadTexture2(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_ReloadTextureParams *_pst_RTP)
{
}

extern float	GFXF_f_depth;
extern ULONG	GFXF_ul_Color;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_ReadPixel(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *_pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	D3DLOCKED_RECT			LockedR;
	RECT					Rect = {(LONG)_pst_Pos->x, (LONG)_pst_Pos->y, (LONG)_pst_Pos->x + 1, (LONG)_pst_Pos->y + 1};
	Gx8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = GX8_M_SD(_pst_DD);

	if(D3D_OK == IDirect3DDevice8_GetDepthStencilSurface(pst_SD->mp_D3DDevice, &pst_SD->pZBuffer_SAVE))
	{
		if(D3D_OK == IDirect3DSurface8_LockRect(pst_SD->pZBuffer_SAVE, &LockedR, &Rect, D3DLOCK_READONLY))
		{
			float f = 1.0f;
			if ( ( f > 0.9999999f ) || ( f < 0.0f ) )
				f = Cf_Infinit;
			else
				f = 1.0f / ( 20.0f * (1.0f - f) );

			GFXF_f_depth = f;
			IDirect3DSurface8_UnlockRect(pst_SD->pZBuffer_SAVE);
		}

		IDirect3DSurface8_Release(pst_SD->pZBuffer_SAVE);
	}

	pst_SD->pZBuffer_SAVE = NULL;
}

void Gx8_CopyBackBufferToSnapTexture( Gx8_tdst_SpecificData *pst_SD, IDirect3DSurface8 *snapSurface)
{
	IDirect3DSurface8 *pBBSurface;
    HRESULT hr;

    hr = IDirect3DDevice8_GetRenderTarget( pst_SD->mp_D3DDevice, &pBBSurface);
    ERR_X_Assert( SUCCEEDED( hr ));

    hr = IDirect3DDevice8_CopyRects( pst_SD->mp_D3DDevice,
                                     pBBSurface,
                                     NULL,
                                     1,
                                     snapSurface,
                                     NULL );
    ERR_X_Assert( SUCCEEDED( hr ));

	IDirect3DSurface8_Release(pBBSurface);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_PushZBuffer(GDI_tdst_DisplayData *_pst_DD, float Z)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ulBlendingMode;
	MATH_tdst_Matrix ViewMatrix;
	MATH_tdst_Vector	QUAD[4];
	u32 ColorRGBA;
	Gx8_tdst_SpecificData	*pst_SD;
	//int i_Zfunc;
	//int i_Cullmode;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	//return;
	pst_SD = GX8_M_SD(_pst_DD);

	Z = 2.0f;
	MATH_SetIdentityMatrix(&ViewMatrix);

	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix);

	MATH_AddScaleVector(&QUAD[3] , &GDI_gpst_CurDD->st_Camera.st_Matrix.T , MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , Z);

	Z *= 4.0f;

	MATH_AddScaleVector(&QUAD[0] , &QUAD[3] , MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , -Z);
	MATH_AddScaleVector(&QUAD[1] , &QUAD[3] , MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , -Z);
	MATH_AddScaleVector(&QUAD[2] , &QUAD[3] , MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , Z);
	MATH_AddScaleVector(&QUAD[3] , &QUAD[3] , MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , Z);

	MATH_AddScaleVector(&QUAD[0] , &QUAD[0] , MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , Z);
	MATH_AddScaleVector(&QUAD[1] , &QUAD[1] , MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , -Z);
	MATH_AddScaleVector(&QUAD[2] , &QUAD[2] , MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , -Z);
	MATH_AddScaleVector(&QUAD[3] , &QUAD[3] , MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix) , Z);

	ulBlendingMode = MAT_Cul_Flag_HideColor;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode,0);

	//Save state
	//IDirect3DDevice8_GetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZFUNC, &i_Zfunc);
	//IDirect3DDevice8_GetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, &i_Cullmode );
	
	IDirect3DDevice8_SetRenderState ( pst_SD->mp_D3DDevice, D3DRS_ZFUNC, D3DCMP_GREATEREQUAL );

	//Gx8_RS_CullFace(pst_SD->mp_D3DDevice, 0);
	IDirect3DDevice8_SetRenderState ( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, 0 );

	ColorRGBA = 0;
	
	{
		struct MonStream_ {
		float x,y,z;
		DWORD Color;
		} MonStream[4];
	
		HRESULT hr;

		UINT uiVStride;

		Gx8_vSetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		uiVStride = D3DXGetFVFVertexSize(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	    MonStream[0].x = QUAD[0].x;
		MonStream[0].y = QUAD[0].y;
		MonStream[0].z = QUAD[0].z;
		MonStream[0].Color = (DWORD)ColorRGBA;

		MonStream[1].x = QUAD[1].x;
		MonStream[1].y = QUAD[1].y;
		MonStream[1].z = QUAD[1].z;
		MonStream[1].Color = (DWORD)ColorRGBA;

		MonStream[2].x = QUAD[2].x;
		MonStream[2].y = QUAD[2].y;
		MonStream[2].z = QUAD[2].z;
		MonStream[2].Color = (DWORD)ColorRGBA;

		MonStream[3].x = QUAD[3].x;
		MonStream[3].y = QUAD[3].y;
		MonStream[3].z = QUAD[3].z;
		MonStream[3].Color = (DWORD)ColorRGBA;

        hr = IDirect3DDevice8_DrawPrimitiveUP( pst_SD->mp_D3DDevice, D3DPT_QUADLIST, 2, (void*)MonStream,uiVStride );
	
	}
	
	//IDirect3DDevice8_SetRenderState ( pst_SD->mp_D3DDevice, D3DRS_ZFUNC, i_Zfunc );
	//IDirect3DDevice8_SetRenderState ( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, i_Cullmode );
	//IDirect3DDevice8_SetRenderState ( pst_SD->mp_D3DDevice, D3DRS_ZFUNC, i_Cullmode );
	IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ZFUNC, D3DCMP_LESSEQUAL );

}
void Gx8_ReadScreen(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_RWPixels *d)
{
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ULONG ulBlendingMode;
	Gx8_tdst_SpecificData *pst_SD;
	HRESULT hr;
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	pst_SD = GX8_M_SD(_pst_DD);

	MAT_SET_Blending (ulBlendingMode, MAT_Cc_Op_Copy);
	Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode, 0);

	if (d->c_Write == 0)
    {
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		D3DLOCKED_RECT stLockedRect;
		unsigned char *p_cBitPtr, *p_cDestPtr, *p_cBitPtr2;
		int iLine, iColumn;
		RECT stRect;
		//IDirect3DTexture8 *pSnapTexture;
		IDirect3DSurface8 *pSnapSurface;
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        if (d->c_Buffer == 2)
            return;

		stRect.left = d->x;
		stRect.right = d->x + d->w;
		stRect.top = d->y;
		stRect.bottom = d->y + d->h;

		// end puis rebegin ?
		if(pst_SD->RenderScene)
			IDirect3DDevice8_EndScene(pst_SD->mp_D3DDevice);

		// create texture for snapshot 
//		IDirect3DDevice8_CreateTexture( pst_SD->mp_D3DDevice, 640, 480, 1, D3DUSAGE_RENDERTARGET, 
//										D3DFMT_LIN_A8R8G8B8, D3DPOOL_DEFAULT, &pSnapTexture);
		hr = IDirect3DTexture8_GetSurfaceLevel( pst_SD->pTmpFrameBufferTexture, 0, &pSnapSurface);
		ERR_X_Assert(hr==D3D_OK);

		// copy back buffer to SnapSurface
		Gx8_CopyBackBufferToSnapTexture( pst_SD, pSnapSurface);

		// Lock SnapSurface
		hr = IDirect3DSurface8_LockRect( pSnapSurface, &stLockedRect, &stRect, D3DLOCK_READONLY);
		ERR_X_Assert(hr==D3D_OK);

		// copy pixels into buffer
		p_cBitPtr = stLockedRect.pBits;
		p_cDestPtr = (unsigned char *) d->p_Bitmap;
		iLine = d->h;
		while (iLine--)
		{
			p_cBitPtr2 = p_cBitPtr;
			for (iColumn=0 ; iColumn<d->w ; iColumn++)
			{
				// copy rgb
				*p_cDestPtr = *p_cBitPtr2;
				p_cDestPtr ++; p_cBitPtr2 ++;
				*p_cDestPtr = *p_cBitPtr2;
				p_cDestPtr ++; p_cBitPtr2 ++;
				*p_cDestPtr = *p_cBitPtr2;
				p_cDestPtr ++; p_cBitPtr2 ++;
				// ignore alpha
				p_cBitPtr2 ++; 
			}
			// go to next line
			p_cBitPtr += stLockedRect.Pitch;
		}

		// Unlock SnapSurface
		hr = IDirect3DSurface8_UnlockRect( pSnapSurface);
		ERR_X_Assert(hr==D3D_OK);

		// release resources
		hr = IDirect3DSurface8_Release( pSnapSurface);
		ERR_X_Assert(hr==D3D_OK);
		//IDirect3DTexture8_Release( pSnapTexture);

		// rebegin...
		IDirect3DDevice8_BeginScene(pst_SD->mp_D3DDevice);

    }
}

/*
 =======================================================================================================================
    Aim:    Treat miscelleanous request ( see request constant in GDInterface.h file )
 =======================================================================================================================
 */
LONG Gx8_l_Request(ULONG _ul_Request, ULONG _ul_Data)
{
	/*$off*/
	switch(_ul_Request)
	{
	case GDI_Cul_Request_SetFogParams:				Gx8_SetFogParams(GDI_gpst_CurDD, (SOFT_tdst_FogParams *) _ul_Data); break;
	case GDI_Cul_Request_DrawSoftEllipse:			Gx8_DrawEllipse(GDI_gpst_CurDD, (SOFT_tdst_Ellipse *) _ul_Data); break;
	case GDI_Cul_Request_DrawSoftArrow:				Gx8_DrawArrow(GDI_gpst_CurDD, (SOFT_tdst_Arrow *) _ul_Data); break;
	case GDI_Cul_Request_DrawSoftSquare:			Gx8_DrawSquare(GDI_gpst_CurDD, (SOFT_tdst_Square *) _ul_Data); break;
	case GDI_Cul_Request_DrawTransformedTriangle:	Gx8_DrawProjectedTriangle(GDI_gpst_CurDD, (SOFT_tdst_Vertex *) _ul_Data); break;
	case GDI_Cul_Request_DepthTest:					Gx8_RS_DepthTest( GX8_M_SD( GDI_gpst_CurDD ), _ul_Data ); break;
	case GDI_Cul_Request_DepthFunc:					Gx8_RS_DepthFunc( GX8_M_SD( GDI_gpst_CurDD ), _ul_Data ? D3DCMP_LESSEQUAL : D3DCMP_ALWAYS); break;
	case GDI_Cul_Request_DrawPoint:					Gx8_DrawPoint( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
	case GDI_Cul_Request_DrawPointMin:				Gx8_DrawPointMin( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
	case GDI_Cul_Request_DrawLine:					Gx8_DrawLine( GDI_gpst_CurDD, (MATH_tdst_Vector **) _ul_Data ); break;
	case GDI_Cul_Request_DrawTriangle:				break;
	case GDI_Cul_Request_DrawQuad:					Gx8_DrawQuad( GDI_gpst_CurDD, (MATH_tdst_Vector **) _ul_Data ); break;
	case GDI_Cul_Request_ReloadTexture:				Gx8_ReloadTexture( GDI_gpst_CurDD, (GDI_tdst_Request_ReloadTextureParams *) _ul_Data); break;
	case GDI_Cul_Request_ReloadTexture2:			Gx8_ReloadTexture2( GDI_gpst_CurDD, (GDI_tdst_Request_ReloadTextureParams *) _ul_Data); break;
	case GDI_Cul_Request_SizeOfPoints:													Gx8_PointSize( GDI_gpst_CurDD, *(float *) &_ul_Data ); break;
	case GDI_Cul_Request_SizeOfLine:				Gx8_LineSize(GDI_gpst_CurDD, *(float *) &_ul_Data ); break;
	case GDI_Cul_Request_DrawLineEx:				Gx8_DrawLineEx(GDI_gpst_CurDD, (GDI_tdst_Request_DrawLineEx *) _ul_Data); break;
	case GDI_Cul_Request_DrawPointEx:				Gx8_DrawPointEx(GDI_gpst_CurDD, (GDI_tdst_Request_DrawPointEx *)_ul_Data ); break;
	case GDI_Cul_Request_DrawPointSize:				Gx8_DrawPoint_Size(GDI_gpst_CurDD, (void  **)_ul_Data);break;
	case GDI_Cul_Request_BeforeDrawSprite:			Gx8_BeforeDrawSprite(GDI_gpst_CurDD, _ul_Data);break;
	case GDI_Cul_Request_DrawSprite:				Gx8_DrawSprite(GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data );break;
	case GDI_Cul_Request_DrawSpriteUV:				Gx8_DrawSprite(GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data );break;
	case GDI_Cul_Request_AfterDrawSprite:			Gx8_AfterDrawSprite(GDI_gpst_CurDD);break;
	case GDI_Cul_Request_ReloadPalette:				Gx8_ReloadPalette( GDI_gpst_CurDD, (short) _ul_Data );	break;
	case GDI_Cul_Request_Draw2DTriangle:			Gx8_Draw2DTriangle(GDI_gpst_CurDD, (SOFT_tdst_Vertex *) _ul_Data); break;
	case GDI_Cul_Request_ReadScreen:				Gx8_ReadScreen( GDI_gpst_CurDD, (GDI_tdst_Request_RWPixels *) _ul_Data ); break;
	case GDI_Cul_Request_EnableFog:					Gx8_RS_Fogged( GX8_M_SD(GDI_gpst_CurDD), _ul_Data ); break;
	case GDI_Cul_Request_NumberOfTextures:			return ( GX8_M_SD( GDI_gpst_CurDD )->l_NumberOfTextures != (LONG) _ul_Data );
	case GDI_Cul_Request_ReadPixel:					Gx8_ReadPixel( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
	case GDI_Cul_Request_Enable:					break;
	case GDI_Cul_Request_Disable:					break;
	case GDI_Cul_Request_PolygonOffset:				break;
	case GDI_Cul_Request_LoadInterfaceTex:			break;
	case GDI_Cul_Request_UnloadInterfaceTex:		break;
	case GDI_Cul_Request_GetInterfaceTexBuffer:		break;
	case GDI_Cul_Request_TextureUnloadCompare:		break;
	case GDI_Cul_Request_Display169BlackBand:		Gx8_Display169BlackBand(GDI_gpst_CurDD); break;

	case GDI_Cul_Request_PushZBuffer:		Gx8_PushZBuffer(GDI_gpst_CurDD, *(float*)&_ul_Data);break;
	}
	/*$on*/
	return 0;
}

#if defined PSX2_TARGET && defined __cplusplus
}
#endif
