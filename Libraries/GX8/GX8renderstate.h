/*$T Gx8renderstate.h GC!1.52 11/23/99 12:22:14 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    OpenGL initialization / close / flip / restore
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __GX8RENDERSTATE_H__
#define __GX8RENDERSTATE_H__

#ifndef PSX2_TARGET
#pragma once
#endif

#include "BASe/BAStypes.h"
#include "Gx8init.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Structure
 ***************************************************************************************************
 */

typedef enum
{
    Gx8_eUVOff, // directly taken from the vertex UV (no transformation)
    Gx8_eUVNormal, // computed from the camera-space positions
    Gx8_eUVChrome, // computed from the camera-space normals
    Gx8_eUVRotation // computed from the vertex UV
} Gx8_tdeUVGenerationType;

/*
 ---------------------------------------------------------------------------------------------------
    Specific data for OpenGL Device
 ---------------------------------------------------------------------------------------------------
 */
/*$4
 ***************************************************************************************************
    Function (bodies for the inline functions are at the end of this file)
 ***************************************************************************************************
 */

void            Gx8_RS_Init(Gx8_tdst_SpecificData *);

__inline void   Gx8_RS_DrawWired(Gx8_tdst_SpecificData *, LONG);
__inline void   Gx8_RS_CullFace(Gx8_tdst_SpecificData *, Gx8_tdeCullFaceType);
__inline void   Gx8_RS_Fogged(struct Gx8_tdst_SpecificData_ *, LONG);
__inline void   Gx8_RS_DepthFunc( struct Gx8_tdst_SpecificData_ *, LONG);
__inline void   Gx8_RS_DepthTest(struct Gx8_tdst_SpecificData_ *, LONG);
__inline void   Gx8_RS_DepthMask(struct Gx8_tdst_SpecificData_ *, LONG);
__inline void   Gx8_RS_Lighting(struct Gx8_tdst_SpecificData_ *, LONG);
LONG            Gx8_RS_UseTexture(struct Gx8_tdst_SpecificData_ *, LONG);

void            Gx8_RS_LineWidth(Gx8_tdst_RenderState *, float);
void            Gx8_RS_PointSize(Gx8_tdst_RenderState *, float);

__inline void   Gx8_vSetVertexShader(DWORD dwVertexShader);
__inline void   Gx8_vSetPixelShader(DWORD dwPixelShader);

void            Gx8_SetRastersRenderState (void);

void                    Gx8_SetUVStageMatrix(D3DMATRIX *pstMatrix);
void                    Gx8_SetUVChromeStage(void);
void                    Gx8_SetUVNormalStage(void);
void                    Gx8_SetUVStageOff(void);
void                    Gx8_SetUVRotationStage(void);
Gx8_tdeUVGenerationType Gx8_fneGetCurrentUVState(void);

/*
 ***************************************************************************************************
    Implementation of inline functions
 ***************************************************************************************************
 */
__inline void Gx8_RS_DrawWired(Gx8_tdst_SpecificData *_pst_SD, LONG _l_Wired)
{
	if(_l_Wired)
	{
		if (!_pst_SD->st_RS.c_Wired)
		{
			IDirect3DDevice8_SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			_pst_SD->st_RS.c_Wired = 1;
		}
	}
	else
	{
		if(_pst_SD->st_RS.c_Wired)
		{
			IDirect3DDevice8_SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_FILLMODE, D3DFILL_SOLID );
			_pst_SD->st_RS.c_Wired = 0;
		}
	}
}

__inline void Gx8_RS_CullFace(Gx8_tdst_SpecificData *_pst_SD, Gx8_tdeCullFaceType _e_CullFace)
{
    if (_pst_SD->st_RS.e_CullFace != _e_CullFace)
    {
        IDirect3DDevice8_SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_CULLMODE, _e_CullFace );
        _pst_SD->st_RS.e_CullFace = _e_CullFace;
    }
}

__inline void Gx8_RS_Fogged(Gx8_tdst_SpecificData *_pst_SD, LONG _l_FogOn)
{
	if(_l_FogOn)
    {
        if(!_pst_SD->st_RS.c_Fogged)
        {
            IDirect3DDevice8_SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_FOGENABLE, 1/*TRUE*/ );
            _pst_SD->st_RS.c_Fogged = 1;
        }
    }
    else
    {
        if(_pst_SD->st_RS.c_Fogged)
        {
            IDirect3DDevice8_SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_FOGENABLE, FALSE );
            _pst_SD->st_RS.c_Fogged = 0;
        }
    }
}

__inline void Gx8_RS_DepthFunc(Gx8_tdst_SpecificData *_pst_SD, LONG _l_Func)
{
    if(_pst_SD->st_RS.l_DepthFunc != _l_Func)
    {
        IDirect3DDevice8_SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_ZFUNC, _l_Func );
        _pst_SD->st_RS.l_DepthFunc = _l_Func;
    }
}

__inline void Gx8_RS_DepthTest(Gx8_tdst_SpecificData *_pst_SD, LONG _l_Test)
{
    IDirect3DDevice8_SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_ZENABLE, _l_Test ? D3DZB_TRUE : D3DZB_FALSE  );
}

__inline void Gx8_RS_DepthMask(Gx8_tdst_SpecificData *_pst_SD, LONG _l_DepthMask)
{
    if(_l_DepthMask)
    {
        if(!_pst_SD->st_RS.c_DepthMask)
        {
            IDirect3DDevice8_SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 1/*TRUE*/ );
            _pst_SD->st_RS.c_DepthMask = 1;
        }
    }
    else
    {
        if(_pst_SD->st_RS.c_DepthMask)
        {
            IDirect3DDevice8_SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, FALSE );
            _pst_SD->st_RS.c_DepthMask = 0;
        }
    }
}

__inline void Gx8_RS_Lighting(Gx8_tdst_SpecificData *_pst_SD, LONG _l_LightingOn)
{
    if (_l_LightingOn)
    {
        if(!_pst_SD->st_RS.c_Lighting)
        {
            IDirect3DDevice8_SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_LIGHTING, 1/*TRUE*/ );
            _pst_SD->st_RS.c_Lighting = 1;
        }
    }
    else
    {
        if(_pst_SD->st_RS.c_Lighting)
        {
            IDirect3DDevice8_SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE );
            _pst_SD->st_RS.c_Lighting = 0;
        }
    }
}

__inline void    Gx8_vSetVertexShader(DWORD dwVertexShader)
{
    extern Gx8_tdst_SpecificData	*p_gGx8SpecificData;

    if (p_gGx8SpecificData->st_RS.dwCurrentVertexShader != dwVertexShader)
    {
        IDirect3DDevice8_SetVertexShader(p_gGx8SpecificData->mp_D3DDevice, dwVertexShader);
        p_gGx8SpecificData->st_RS.dwCurrentVertexShader = dwVertexShader;
    }
}
__inline void    Gx8_vSetPixelShader(DWORD dwPixelShader)
{
    extern Gx8_tdst_SpecificData	*p_gGx8SpecificData;

    if (p_gGx8SpecificData->st_RS.dwCurrentPixelShader != dwPixelShader)
    {
        IDirect3DDevice8_SetPixelShader(p_gGx8SpecificData->mp_D3DDevice, dwPixelShader);
        p_gGx8SpecificData->st_RS.dwCurrentPixelShader = dwPixelShader;
    }
}

#ifdef __cplusplus
}
#endif
#endif /* __GX8RENDERSTATE_H */
