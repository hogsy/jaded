/*$T DX8request.c GC! 1.081 01/17/02 10:15:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include <D3D8.h>

#ifdef PSX2_TARGET
#include "PS2GL.h"
#include "PS2GL_Redefine.h"
#else
#if !defined(_XBOX) && !defined(_XENON)
#include "ddraw.h"
#endif
#endif
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "DX8init.h"
#include "DX8request.h"

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BAStypes.h"
#include "TEXture/TEXfile.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEXstruct.h"
#include "SOFT/SOFTcolor.h"

#include "DX8debugfct.h"

#if defined PSX2_TARGET && defined __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Full screen mode
 ***********************************************************************************************************************
 */

extern HINSTANCE	MAI_gh_MainInstance;

LONG				DX8_l_FullScreenMode = 0;
RECT				DX8_gst_WindowPos;
HWND				DX8_gh_FullScreenWnd;
HWND				DX8_gh_WindowedHwndSave;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_SetFogParams(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_FogParams *_pst_Fog)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static int				sFM[3] = { D3DFOG_LINEAR, D3DFOG_EXP, D3DFOG_EXP2 };
	DX8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = DX8_M_SD(_pst_DD);

	if(_pst_Fog->c_Flag & SOFT_C_FogActive)
	{
		pst_SD->ul_FogColor = DX8_M_ConvertColor(_pst_Fog->ul_Color);
		pst_SD->ul_FogColorBlack = 0;
		pst_SD->ul_FogColorOn2 = COLOR_ul_Mul(pst_SD->ul_FogColor, 0.5f);

		DX8_M_RenderState(pst_SD, D3DRS_FOGENABLE, TRUE);
		DX8_M_RenderState(pst_SD, D3DRS_FOGTABLEMODE, sFM[_pst_Fog->c_Mode]);
		DX8_M_RenderState(pst_SD, D3DRS_FOGDENSITY, *(ULONG *) &_pst_Fog->f_Density);
		pst_SD->ulFogState = 1;


		DX8_M_RenderState(pst_SD, D3DRS_FOGCOLOR, pst_SD->ul_FogColor);
		DX8_M_RenderState(pst_SD, D3DRS_FOGSTART, *(ULONG *) &_pst_Fog->f_Start);
		DX8_M_RenderState(pst_SD, D3DRS_FOGEND, *(ULONG *) &_pst_Fog->f_End);
	}
	else
	{
		pst_SD->ulFogState = 0;
		DX8_M_RenderState(pst_SD, D3DRS_FOGENABLE, FALSE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_DrawProjectedTriangle(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Vertex *_pst_TV)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_Draw2DTriangle(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Vertex *_pst_TV)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a point
 =======================================================================================================================
 */
void DX8_DrawPoint(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_DrawPointMin(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
}

/*
 =======================================================================================================================
    Aim:    draw a point with a given size
 =======================================================================================================================
 */
void DX8_DrawPoint_Size(GDI_tdst_DisplayData *_pst_DD, void **peewee)
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
void DX8_DrawPointEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawPointEx *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ulBlendingMode;
	DX8VertexFormat			*pVertexBuffer;
	float					fSize;
	DX8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = DX8_M_SD(_pst_DD);
#ifdef JADEFUSION
	pst_SD->m_VertexBuffer->/*lpVtbl->*/Lock
#else
		pst_SD->m_VertexBuffer->lpVtbl->Lock
#endif
			(
#ifndef JADEFUSION
			pst_SD->m_VertexBuffer,
#endif
			0,
			1 * sizeof(DX8VertexFormat),
			(unsigned char **) &pVertexBuffer,
			D3DLOCK_DISCARD
		);

	*(MATH_tdst_Vector *) &pVertexBuffer[0] = *_pst_Data->A;
	pVertexBuffer[0].Color = DX8_M_ConvertColor(_pst_Data->ul_Color);

#ifdef JADEFUSION
	pst_SD->m_VertexBuffer->/*lpVtbl->*/Unlock(/*pst_SD->m_VertexBuffer*/);
#else
	pst_SD->m_VertexBuffer->lpVtbl->Unlock(pst_SD->m_VertexBuffer);
#endif

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	DX8_SetTextureBlending((ULONG) - 1, ulBlendingMode);
	fSize = _pst_Data->f_Size * 1.0f;

	DX8_M_RenderState(pst_SD, D3DRS_POINTSPRITEENABLE, TRUE);
	DX8_M_RenderState(pst_SD, D3DRS_POINTSCALEENABLE, TRUE);
	DX8_M_RenderState(pst_SD, D3DRS_POINTSIZE, FtoDW(100.0f));
	DX8_M_RenderState(pst_SD, D3DRS_POINTSIZE_MIN, FtoDW(4.00f));
	DX8_M_RenderState(pst_SD, D3DRS_POINTSIZE_MAX, FtoDW(5.00f));
	DX8_M_RenderState(pst_SD, D3DRS_POINTSCALE_A, FtoDW(0.00f));
	DX8_M_RenderState(pst_SD, D3DRS_POINTSCALE_B, FtoDW(0.00f));
	DX8_M_RenderState(pst_SD, D3DRS_POINTSCALE_C, FtoDW(1.00f));

#ifdef JADEFUSION
	pst_SD->mp_D3DDevice->/*lpVtbl->*/SetStreamSource
#else
		pst_SD->mp_D3DDevice->lpVtbl->SetStreamSource
#endif
		(
#ifndef JADEFUSION
		pst_SD->mp_D3DDevice,
#endif
	0,
			pst_SD->m_VertexBuffer,
			sizeof(DX8VertexFormat)
		);
#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/SetVertexShader(/*pst_SD->mp_D3DDevice,*/ D3DFVF_VERTEXF);
	pst_SD->mp_D3DDevice->/*lpVtbl->*/DrawPrimitive(/*pst_SD->mp_D3DDevice,*/ D3DPT_POINTLIST, 0, 1);
#else
	pst_SD->mp_D3DDevice->lpVtbl->SetVertexShader(pst_SD->mp_D3DDevice, D3DFVF_VERTEXF);
	pst_SD->mp_D3DDevice->lpVtbl->DrawPrimitive(pst_SD->mp_D3DDevice, D3DPT_POINTLIST, 0, 1);
#endif
}

/*
 =======================================================================================================================
    Aim:    draw a point with a given size and color
 =======================================================================================================================
 */
void DX8_PointSize(GDI_tdst_DisplayData *_pst_DD, float _f_Size)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a line
 =======================================================================================================================
 */
void DX8_DrawLine(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector **v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ulBlendingMode;
	DX8VertexFormat			*pVertexBuffer;
	DX8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = DX8_M_SD(_pst_DD);

#ifdef JADEFUSION
	pst_SD->m_VertexBuffer->/*lpVtbl->*/Lock
#else
		pst_SD->m_VertexBuffer->lpVtbl->Lock
#endif
	(
#ifndef JADEFUSION
			pst_SD->m_VertexBuffer,
#endif			
			0,
			2 * sizeof(DX8VertexFormat),
			(unsigned char **) &pVertexBuffer,
			D3DLOCK_DISCARD
		);
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
	DX8_SetTextureBlending((ULONG) - 1, ulBlendingMode);
#ifdef JADEFUSION
	pst_SD->mp_D3DDevice->/*lpVtbl->*/SetStreamSource
#else
		pst_SD->mp_D3DDevice->lpVtbl->SetStreamSource
#endif
	(
#ifndef JADEFUSION
			pst_SD->mp_D3DDevice,
#endif			
			0,
			pst_SD->m_VertexBuffer,
			sizeof(DX8VertexFormat)
		);
#ifdef JADEFUSION
	pst_SD->mp_D3DDevice->/*lpVtbl->*/SetVertexShader(/*pst_SD->mp_D3DDevice,*/ D3DFVF_VERTEXF);
	pst_SD->mp_D3DDevice->/*lpVtbl->*/DrawPrimitive(/*pst_SD->mp_D3DDevice,*/ D3DPT_LINELIST, 0, 1);
#else
	pst_SD->mp_D3DDevice->lpVtbl->SetVertexShader(pst_SD->mp_D3DDevice, D3DFVF_VERTEXF);
	pst_SD->mp_D3DDevice->lpVtbl->DrawPrimitive(pst_SD->mp_D3DDevice, D3DPT_LINELIST, 0, 1);
#endif
}

/*
 =======================================================================================================================
    Aim:    Draw a line with width and color given
 =======================================================================================================================
 */
void DX8_DrawLineEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawLineEx *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ulBlendingMode;
	DX8VertexFormat			*pVertexBuffer;
	DX8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = DX8_M_SD(_pst_DD);

#ifdef JADEFUSION
	pst_SD->m_VertexBuffer->/*lpVtbl->*/Lock
#else
		pst_SD->m_VertexBuffer->lpVtbl->Lock
#endif
			(
#ifndef JADEFUSION
			pst_SD->m_VertexBuffer,
#endif
			0,
			2 * sizeof(DX8VertexFormat),
			(unsigned char **) &pVertexBuffer,
			D3DLOCK_DISCARD
		);

	*(MATH_tdst_Vector *) &pVertexBuffer[0] = *_pst_Data->A;
	*(MATH_tdst_Vector *) &pVertexBuffer[1] = *_pst_Data->B;

	pVertexBuffer[0].Color = DX8_M_ConvertColor(_pst_Data->ul_Color);
	pVertexBuffer[1].Color = DX8_M_ConvertColor(_pst_Data->ul_Color);

#ifdef JADEFUSION
	pst_SD->m_VertexBuffer->/*lpVtbl->*/Unlock(/*pst_SD->m_VertexBuffer*/);
#else
	pst_SD->m_VertexBuffer->lpVtbl->Unlock(pst_SD->m_VertexBuffer);
#endif
	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	DX8_SetTextureBlending((ULONG) - 1, ulBlendingMode);

#ifdef JADEFUSION
	pst_SD->mp_D3DDevice->/*lpVtbl->*/SetStreamSource
#else
		pst_SD->mp_D3DDevice->lpVtbl->SetStreamSource
#endif
			(
#ifndef JADEFUSION
			pst_SD->mp_D3DDevice,
#endif
			0,
			pst_SD->m_VertexBuffer,
			sizeof(DX8VertexFormat)
		);
#ifdef JADEFUSION
	pst_SD->mp_D3DDevice->/*lpVtbl->*/SetVertexShader(/*pst_SD->mp_D3DDevice,*/ D3DFVF_VERTEXF);
	pst_SD->mp_D3DDevice->/*lpVtbl->*/DrawPrimitive(/*pst_SD->mp_D3DDevice,*/ D3DPT_LINELIST, 0, 1);
#else
	pst_SD->mp_D3DDevice->lpVtbl->SetVertexShader(pst_SD->mp_D3DDevice, D3DFVF_VERTEXF);
	pst_SD->mp_D3DDevice->lpVtbl->DrawPrimitive(pst_SD->mp_D3DDevice, D3DPT_LINELIST, 0, 1);
#endif
}

/*
 =======================================================================================================================
    Aim:    Draw a line with width and color given
 =======================================================================================================================
 */
void DX8_LineSize(GDI_tdst_DisplayData *_pst_DD, float _f_Size)
{
}

/*
 =======================================================================================================================
    Aim:    Draw a quad
 =======================================================================================================================
 */
void DX8_DrawQuad(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector **v)
{
}

static PointSprite = 0;

/*
 =======================================================================================================================
    Aim:    Draw a Sprite
 =======================================================================================================================
 */
void DX8_DrawSprite(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DX8_tdst_SpecificData	*pst_SD;
	static float			tf_UV[5] = { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f };
	ULONG					ul_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = DX8_M_SD(_pst_DD);
	if(pst_SD->pst_LockedVertex == NULL) return;

    if ( pst_SD->ul_LVLeft == 0)
        return;

	ul_Color = DX8_M_ConvertColor(*(ULONG *) &v[4].x);

    if ( PointSprite )
    {
        *(GEO_Vertex *) pst_SD->pst_LockedVertex = v[0];
	    pst_SD->pst_LockedVertex->Color = ul_Color;
	    pst_SD->pst_LockedVertex++;
        pst_SD->ul_LVLeft --;
    }
    else
    {
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
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_BeforeDrawSprite(GDI_tdst_DisplayData *_pst_DD, ULONG _ul_NbSprites)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ulBlendingMode;
	DX8_tdst_SpecificData	*pst_SD;
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

	pst_SD = DX8_M_SD(_pst_DD);

#ifdef ACTIVE_EDITORS
	if(!(_pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired))
	{
		DX8_RS_LineWidth(&pst_SD->st_RS, 1.0f);
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
		DX8_SetTextureBlending((ULONG) - 1, ulBlendingMode);
	}

	DX8_RS_DrawWired(pst_SD, !(_pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired));
#else
	DX8_RS_DrawWired(pst_SD, 0);
#endif
	DX8_RS_CullFaceInverted(pst_SD, 0);
	//DX8_RS_CullFace(pst_SD, 0);

	if(!pst_SD->RenderScene)
	{
		pst_SD->RenderScene = TRUE;
#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/BeginScene(/*pst_SD->mp_D3DDevice*/);
#else
		pst_SD->mp_D3DDevice->lpVtbl->BeginScene(pst_SD->mp_D3DDevice);
#endif
	}

    if ( PointSprite )
    {
        pst_SD->ul_LVLeft = pst_SD->ul_LockedVertex = _ul_NbSprites;
#ifdef JADEFUSION
		pst_SD->m_VertexBuffer->/*lpVtbl->*/Lock
#else
			pst_SD->m_VertexBuffer->lpVtbl->Lock
#endif
				(
#ifndef JADEFUSION
				pst_SD->m_VertexBuffer,
#endif			    
				0,
			    _ul_NbSprites * sizeof(DX8VertexFormat),
			    (unsigned char **) &pst_SD->pst_LockedVertex,
			    D3DLOCK_DISCARD
		    );
    }
    else
    {
	    pst_SD->ul_LVLeft = pst_SD->ul_LockedVertex = _ul_NbSprites * 2;
#ifdef JADEFUSION
		pst_SD->m_VertexBuffer->/*lpVtbl->*/Lock
#else
		pst_SD->m_VertexBuffer->lpVtbl->Lock
#endif
			(
#ifndef JADEFUSION
			pst_SD->m_VertexBuffer,
#endif
				0,
			    _ul_NbSprites * sizeof(DX8VertexFormat) * 6,
			    (unsigned char **) &pst_SD->pst_LockedVertex,
			    D3DLOCK_DISCARD
		    );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_AfterDrawSprite(GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DX8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = DX8_M_SD(_pst_DD);
	if(pst_SD->pst_LockedVertex == NULL) return;

#ifdef JADEFUSION
	pst_SD->m_VertexBuffer->/*lpVtbl->*/Unlock(/*pst_SD->m_VertexBuffer*/);
	pst_SD->mp_D3DDevice->/*lpVtbl->*/SetStreamSource
#else
	pst_SD->m_VertexBuffer->lpVtbl->Unlock(pst_SD->m_VertexBuffer);
	pst_SD->mp_D3DDevice->lpVtbl->SetStreamSource
#endif
		(
#ifndef JADEFUSION
			pst_SD->mp_D3DDevice,
#endif			
			0,
			pst_SD->m_VertexBuffer,
			sizeof(DX8VertexFormat)
		);
#ifdef JADEFUSION
	pst_SD->mp_D3DDevice->/*lpVtbl->*/SetVertexShader(/*pst_SD->mp_D3DDevice,*/ D3DFVF_VERTEXF);
#else
	pst_SD->mp_D3DDevice->lpVtbl->SetVertexShader(pst_SD->mp_D3DDevice, D3DFVF_VERTEXF);
#endif
    if ( PointSprite )
    {
        DX8_M_RenderState(pst_SD, D3DRS_POINTSPRITEENABLE, TRUE);
	    DX8_M_RenderState(pst_SD, D3DRS_POINTSCALEENABLE, TRUE);
	    DX8_M_RenderState(pst_SD, D3DRS_POINTSIZE,     FtoDW(0.08f) );
	    DX8_M_RenderState(pst_SD, D3DRS_POINTSIZE_MIN, FtoDW(0.00f));
        DX8_M_RenderState(pst_SD, D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
        DX8_M_RenderState(pst_SD, D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
        DX8_M_RenderState(pst_SD, D3DRS_POINTSCALE_C,  FtoDW(1.00f) );

#ifdef JADEFUSION
        pst_SD->mp_D3DDevice->/*lpVtbl->*/DrawPrimitive(/*pst_SD->mp_D3DDevice,*/ D3DPT_POINTLIST, 0, pst_SD->ul_LockedVertex);
#else
		pst_SD->mp_D3DDevice->lpVtbl->DrawPrimitive(pst_SD->mp_D3DDevice, D3DPT_POINTLIST, 0, pst_SD->ul_LockedVertex);
#endif	
	}
    else
    {
#ifdef JADEFUSION
		pst_SD->mp_D3DDevice->/*lpVtbl->*/DrawPrimitive(/*pst_SD->mp_D3DDevice,*/ D3DPT_TRIANGLELIST, 0, pst_SD->ul_LockedVertex);
#else
		pst_SD->mp_D3DDevice->lpVtbl->DrawPrimitive(pst_SD->mp_D3DDevice, D3DPT_TRIANGLELIST, 0, pst_SD->ul_LockedVertex);
#endif
	}
    pst_SD->pst_LockedVertex = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_ReloadTexture(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_ReloadTextureParams *_pst_RTP)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_ReloadTexture2(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_ReloadTextureParams *_pst_RTP)
{
}

extern float	GFXF_f_depth;
extern ULONG	GFXF_ul_Color;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_ReadPixel(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *_pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	D3DLOCKED_RECT			LockedR;
	DX8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = DX8_M_SD(_pst_DD);

#ifdef JADEFUSION
	if(D3D_OK == pst_SD->mp_D3DDevice->/*lpVtbl->*/GetDepthStencilSurface(/*pst_SD->mp_D3DDevice,*/ &pst_SD->pZBuffer_SAVE))
	{
		if(D3D_OK == pst_SD->pZBuffer_SAVE->/*lpVtbl->*/LockRect(/*pst_SD->pZBuffer_SAVE,*/ &LockedR, NULL, D3DLOCK_READONLY))
		{
			pst_SD->pZBuffer_SAVE->/*lpVtbl->*/UnlockRect(/*pst_SD->pZBuffer_SAVE*/);
		}

		pst_SD->pZBuffer_SAVE->/*lpVtbl->*/Release(/*pst_SD->pZBuffer_SAVE*/);
	}
#else
	if(D3D_OK == pst_SD->mp_D3DDevice->lpVtbl->GetDepthStencilSurface(pst_SD->mp_D3DDevice, &pst_SD->pZBuffer_SAVE))
	{
		if(D3D_OK == pst_SD->pZBuffer_SAVE->lpVtbl->LockRect(pst_SD->pZBuffer_SAVE, &LockedR, NULL, D3DLOCK_READONLY))
		{
			pst_SD->pZBuffer_SAVE->lpVtbl->UnlockRect(pst_SD->pZBuffer_SAVE);
		}

		pst_SD->pZBuffer_SAVE->lpVtbl->Release(pst_SD->pZBuffer_SAVE);
	}
#endif
	pst_SD->pZBuffer_SAVE = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_ReadScreen(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_RWPixels *d)
{
}

/*
 =======================================================================================================================
    Aim:    Treat miscelleanous request ( see request constant in GDInterface.h file )
 =======================================================================================================================
 */
LONG DX8_l_Request(ULONG _ul_Request, ULONG _ul_Data)
{
	/*$off*/
	switch(_ul_Request)
	{
	case GDI_Cul_Request_SetFogParams:				DX8_SetFogParams(GDI_gpst_CurDD, (SOFT_tdst_FogParams *) _ul_Data); break;
	case GDI_Cul_Request_DrawSoftEllipse:			DX8_DrawEllipse(GDI_gpst_CurDD, (SOFT_tdst_Ellipse *) _ul_Data); break;
	case GDI_Cul_Request_DrawSoftArrow:				DX8_DrawArrow(GDI_gpst_CurDD, (SOFT_tdst_Arrow *) _ul_Data); break;
	case GDI_Cul_Request_DrawSoftSquare:			DX8_DrawSquare(GDI_gpst_CurDD, (SOFT_tdst_Square *) _ul_Data); break;
	case GDI_Cul_Request_DrawTransformedTriangle:	DX8_DrawProjectedTriangle(GDI_gpst_CurDD, (SOFT_tdst_Vertex *) _ul_Data); break;
	case GDI_Cul_Request_DepthTest:					DX8_RS_DepthTest( DX8_M_SD( GDI_gpst_CurDD ), _ul_Data ); break;
	case GDI_Cul_Request_DepthFunc:					DX8_RS_DepthFunc( DX8_M_SD( GDI_gpst_CurDD ), _ul_Data ? D3DCMP_LESSEQUAL : D3DCMP_ALWAYS); break;
	case GDI_Cul_Request_DrawPoint:					DX8_DrawPoint( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
	case GDI_Cul_Request_DrawPointMin:				DX8_DrawPointMin( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
	case GDI_Cul_Request_DrawLine:					DX8_DrawLine( GDI_gpst_CurDD, (MATH_tdst_Vector **) _ul_Data ); break;
	case GDI_Cul_Request_DrawTriangle:				break;
	case GDI_Cul_Request_DrawQuad:					DX8_DrawQuad( GDI_gpst_CurDD, (MATH_tdst_Vector **) _ul_Data ); break;
	case GDI_Cul_Request_ReloadTexture:				DX8_ReloadTexture( GDI_gpst_CurDD, (GDI_tdst_Request_ReloadTextureParams *) _ul_Data); break;
	case GDI_Cul_Request_ReloadTexture2:			DX8_ReloadTexture2( GDI_gpst_CurDD, (GDI_tdst_Request_ReloadTextureParams *) _ul_Data); break;
	case GDI_Cul_Request_SizeOfPoints:													DX8_PointSize( GDI_gpst_CurDD, *(float *) &_ul_Data ); break;
	case GDI_Cul_Request_SizeOfLine:				DX8_LineSize(GDI_gpst_CurDD, *(float *) &_ul_Data ); break;
	case GDI_Cul_Request_DrawLineEx:				DX8_DrawLineEx(GDI_gpst_CurDD, (GDI_tdst_Request_DrawLineEx *) _ul_Data); break;
	case GDI_Cul_Request_DrawPointEx:				DX8_DrawPointEx(GDI_gpst_CurDD, (GDI_tdst_Request_DrawPointEx *)_ul_Data ); break;
	case GDI_Cul_Request_DrawPointSize:				DX8_DrawPoint_Size(GDI_gpst_CurDD, (void  **)_ul_Data);break;
	case GDI_Cul_Request_BeforeDrawSprite:			DX8_BeforeDrawSprite(GDI_gpst_CurDD, _ul_Data);break;
	case GDI_Cul_Request_DrawSprite:				DX8_DrawSprite(GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data );break;
	case GDI_Cul_Request_DrawSpriteUV:				DX8_DrawSprite(GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data );break;
	case GDI_Cul_Request_AfterDrawSprite:			DX8_AfterDrawSprite(GDI_gpst_CurDD);break;
	case GDI_Cul_Request_ReloadPalette:				/*DX8_ReloadPalette( GDI_gpst_CurDD, (short) _ul_Data ); */break;
	case GDI_Cul_Request_Draw2DTriangle:			DX8_Draw2DTriangle(GDI_gpst_CurDD, (SOFT_tdst_Vertex *) _ul_Data); break;
	case GDI_Cul_Request_ReadScreen:				DX8_ReadScreen( GDI_gpst_CurDD, (GDI_tdst_Request_RWPixels *) _ul_Data ); break;
	case GDI_Cul_Request_EnableFog:					DX8_RS_Fogged( DX8_M_SD(GDI_gpst_CurDD), _ul_Data ); break;
	case GDI_Cul_Request_NumberOfTextures:			return ( DX8_M_SD( GDI_gpst_CurDD )->l_NumberOfTextures != (LONG) _ul_Data );
	case GDI_Cul_Request_ReadPixel:					DX8_ReadPixel( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
	case GDI_Cul_Request_Enable:					break;
	case GDI_Cul_Request_Disable:					break;
	case GDI_Cul_Request_PolygonOffset:				break;
	case GDI_Cul_Request_LoadInterfaceTex:			break;
	case GDI_Cul_Request_UnloadInterfaceTex:		break;
	case GDI_Cul_Request_GetInterfaceTexBuffer:		break;
	case GDI_Cul_Request_TextureUnloadCompare:		break;
	case GDI_Cul_Request_Display169BlackBand:		break;
	}
	/*$on*/
	return 0;
}

#if defined PSX2_TARGET && defined __cplusplus
}
#endif
