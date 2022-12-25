/*$T DX8renderstate.c GC! 1.081 01/15/02 15:40:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Aim: OpenGL render state */
#include "Precomp.h"
#include <D3D8.h>

#include "DX8init.h"
#include "BASe/BAStypes.h"
#include "TEXture/TEXstruct.h"

#if defined PSX2_TARGET && defined __cplusplus
extern "C"
{
#endif
extern ULONG					LIGHT_ul_Interpol2Colors(ULONG, ULONG, float);

extern DX8_tdst_SpecificData	*p_gDX8SpecificData;

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_RS_Init(DX8_tdst_RenderState *_pst_RS)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_RS_DrawWired(DX8_tdst_SpecificData *_pst_SD, LONG _l_Wired)
{
	if(_l_Wired)
	{
		if (!_pst_SD->st_RS.c_Wired)
		{
#ifdef JADEFUSION
			_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetRenderState( /*_pst_SD->mp_D3DDevice,*/ D3DRS_FILLMODE, D3DFILL_WIREFRAME );
#else
			_pst_SD->mp_D3DDevice->lpVtbl->SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_FILLMODE, D3DFILL_WIREFRAME );
#endif
			_pst_SD->st_RS.c_Wired = 1;
		}
	}
	else
	{
		if(_pst_SD->st_RS.c_Wired)
		{
#ifdef JADEFUSION
			_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetRenderState( /*_pst_SD->mp_D3DDevice,*/ D3DRS_FILLMODE, D3DFILL_SOLID );
#else
			_pst_SD->mp_D3DDevice->lpVtbl->SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_FILLMODE, D3DFILL_SOLID );
#endif
			_pst_SD->st_RS.c_Wired = 0;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_RS_CullFace(DX8_tdst_SpecificData *_pst_SD, LONG _l_CullFace)
{
	if(_l_CullFace)
	{
		if(!_pst_SD->st_RS.c_CullFace)
		{
#ifdef JADEFUSION
			_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetRenderState( /*_pst_SD->mp_D3DDevice,*/ D3DRS_CULLMODE, D3DCULL_CCW );
#else
			 _pst_SD->mp_D3DDevice->lpVtbl->SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_CCW );
#endif
		   _pst_SD->st_RS.c_CullFace = 1;
            _pst_SD->st_RS.c_CullFaceInverted = 0;
		}
	}
	else
	{
		if(_pst_SD->st_RS.c_CullFace)
		{
#ifdef JADEFUSION
			_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetRenderState( /*_pst_SD->mp_D3DDevice,*/ D3DRS_CULLMODE, D3DCULL_NONE );
#else
			_pst_SD->mp_D3DDevice->lpVtbl->SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_NONE );
#endif
			_pst_SD->st_RS.c_CullFace = 0;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_RS_CullFaceInverted(DX8_tdst_SpecificData *_pst_SD, LONG _l_CullFaceInverted)
{
    if( _pst_SD->st_RS.c_CullFace )
    {
        if(_l_CullFaceInverted)
        {
            if(!_pst_SD->st_RS.c_CullFaceInverted)
            {
#ifdef JADEFUSION
				_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetRenderState( /*_pst_SD->mp_D3DDevice,*/ D3DRS_CULLMODE, D3DCULL_CW );
#else
				_pst_SD->mp_D3DDevice->lpVtbl->SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_CW );
#endif
				_pst_SD->st_RS.c_CullFaceInverted = 1;
            }
        }
        else if(_pst_SD->st_RS.c_CullFaceInverted)
        {
#ifdef JADEFUSION
			_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetRenderState( /*_pst_SD->mp_D3DDevice,*/ D3DRS_CULLMODE, D3DCULL_CCW );
#else
			_pst_SD->mp_D3DDevice->lpVtbl->SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_CCW );
#endif
			_pst_SD->st_RS.c_CullFaceInverted = 0;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_RS_Fogged(DX8_tdst_SpecificData *_pst_SD, LONG _l_FogOn)
{
    if(_l_FogOn)
    {
        if(!_pst_SD->st_RS.c_Fogged)
        {
#ifdef JADEFUSION
			_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetRenderState( /*_pst_SD->mp_D3DDevice,*/ D3DRS_FOGENABLE, TRUE );
#else
			_pst_SD->mp_D3DDevice->lpVtbl->SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_FOGENABLE, TRUE );
#endif
			_pst_SD->st_RS.c_Fogged = 1;
        }
    }
    else
    {
        if(_pst_SD->st_RS.c_Fogged)
        {
#ifdef JADEFUSION
			_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetRenderState( /*_pst_SD->mp_D3DDevice,*/ D3DRS_FOGENABLE, FALSE );
#else
			_pst_SD->mp_D3DDevice->lpVtbl->SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_FOGENABLE, FALSE );
#endif
			_pst_SD->st_RS.c_Fogged = 0;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_RS_DepthFunc(DX8_tdst_SpecificData *_pst_SD, LONG _l_Func)
{
    if(_pst_SD->st_RS.l_DepthFunc == _l_Func) return;

#ifdef JADEFUSION
	_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetRenderState( /*_pst_SD->mp_D3DDevice,*/ D3DRS_ZFUNC, _l_Func );
#else
	_pst_SD->mp_D3DDevice->lpVtbl->SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_ZFUNC, _l_Func );
#endif
	_pst_SD->st_RS.l_DepthFunc = _l_Func;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_RS_DepthTest(DX8_tdst_SpecificData *_pst_SD, LONG _l_Test)
{
 #ifdef JADEFUSION
	_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetRenderState( /*_pst_SD->mp_D3DDevice,*/ D3DRS_ZENABLE, _l_Test ? D3DZB_TRUE : D3DZB_FALSE  );
#else
	_pst_SD->mp_D3DDevice->lpVtbl->SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_ZENABLE, _l_Test ? D3DZB_TRUE : D3DZB_FALSE  );
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_RS_DepthMask(DX8_tdst_SpecificData *_pst_SD, LONG _l_DepthMask)
{
    if(_l_DepthMask)
    {
        if(!_pst_SD->st_RS.c_DepthMask)
        {
 #ifdef JADEFUSION
			_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetRenderState( /*_pst_SD->mp_D3DDevice,*/ D3DRS_ZWRITEENABLE, TRUE );
 #else
			_pst_SD->mp_D3DDevice->lpVtbl->SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, TRUE );
 #endif
			_pst_SD->st_RS.c_DepthMask = 1;
        }
    }
    else
    {
        if(_pst_SD->st_RS.c_DepthMask)
        {
#ifdef JADEFUSION
			_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetRenderState( /*_pst_SD->mp_D3DDevice,*/ D3DRS_ZWRITEENABLE, FALSE );
#else
			_pst_SD->mp_D3DDevice->lpVtbl->SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, FALSE );
#endif
			_pst_SD->st_RS.c_DepthMask = 0;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_RS_SetPalette(LONG _l_Palette)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG DX8_RS_UseTexture(DX8_tdst_SpecificData *_pst_SD, LONG _l_Texture)
{
    TEX_tdst_Data *pst_Tex;
    IDirect3DBaseTexture8 *T;

    if(_l_Texture != _pst_SD->st_RS.l_LastTexture)
    {
        if ( (_l_Texture == -1) || ( !_pst_SD->dul_Texture ) )
        {
 #ifdef JADEFUSION
			_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*_pst_SD->mp_D3DDevice,*/ 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		    _pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*_pst_SD->mp_D3DDevice,*/ 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		    _pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*_pst_SD->mp_D3DDevice,*/ 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		    _pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*_pst_SD->mp_D3DDevice,*/ 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
#else
			_pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		    _pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		    _pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		    _pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
#endif
		}
        else
        {
            /* shadow Texture ? */
            if(_l_Texture & 0x80000000)
		    {
                T = (IDirect3DBaseTexture8 *) _pst_SD->dul_SDW_Texture[_l_Texture &~0x80000000];
    		}
            else
            {
                if (_pst_SD->dul_Texture[ _l_Texture ] == 0 )
                {
                    pst_Tex = &TEX_gst_GlobalList.dst_Texture[ _l_Texture ];
                
                    /* raw + pal */
                    if (pst_Tex->uw_Flags & TEX_uw_RawPal)
                    {
                        _l_Texture = pst_Tex->w_Height;
#ifdef JADEFUSION
						_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetCurrentTexturePalette( /*_pst_SD->mp_D3DDevice,*/ pst_Tex->w_Width);
#else
						 _pst_SD->mp_D3DDevice->lpVtbl->SetCurrentTexturePalette( _pst_SD->mp_D3DDevice, pst_Tex->w_Width);
#endif
					}
                    /* animated texture */
                    else if (pst_Tex->uw_Flags & TEX_uw_Ani)
                    {
                        _l_Texture = pst_Tex->w_Height;
                        if (_l_Texture == -1)
                        {
                            return DX8_RS_UseTexture( _pst_SD, -1 );
                        }
                        else if (_pst_SD->dul_Texture[ _l_Texture ] == 0 )
                        {
                            pst_Tex = &TEX_gst_GlobalList.dst_Texture[ _l_Texture ];
                            if (pst_Tex->uw_Flags & TEX_uw_RawPal)
                            {
                                _l_Texture = pst_Tex->w_Height;
#ifdef JADEFUSION
								_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetCurrentTexturePalette( /*_pst_SD->mp_D3DDevice,*/ pst_Tex->w_Width);
#else
								_pst_SD->mp_D3DDevice->lpVtbl->SetCurrentTexturePalette( _pst_SD->mp_D3DDevice, pst_Tex->w_Width);
#endif
							}
                        }
                    }
                }

                T = (IDirect3DBaseTexture8 *) _pst_SD->dul_Texture[_l_Texture];
            }
#ifdef JADEFUSION           
             _pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTexture( /*_pst_SD->mp_D3DDevice,*/0, T );
#else
			 _pst_SD->mp_D3DDevice->lpVtbl->SetTexture( _pst_SD->mp_D3DDevice,0, T );
#endif
            if (_pst_SD->st_RS.l_LastTexture == -1)
            {
#ifdef JADEFUSION 
				_pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*_pst_SD->mp_D3DDevice,*/ 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		        _pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*_pst_SD->mp_D3DDevice,*/ 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		        _pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*_pst_SD->mp_D3DDevice,*/ 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		        _pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*_pst_SD->mp_D3DDevice,*/ 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		        _pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*_pst_SD->mp_D3DDevice,*/ 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		        _pst_SD->mp_D3DDevice->/*lpVtbl->*/SetTextureStageState(/*_pst_SD->mp_D3DDevice,*/ 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
#else
				_pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		        _pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		        _pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		        _pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		        _pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		        _pst_SD->mp_D3DDevice->lpVtbl->SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
#endif
			}
            
        }

        _pst_SD->st_RS.l_LastTexture = _l_Texture;
    }
    
    return _l_Texture;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_RS_LineWidth(DX8_tdst_RenderState *_pst_RS, float _f_Width)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DX8_RS_PointSize(DX8_tdst_RenderState *_pst_RS, float _f_Size)
{
}

#if defined PSX2_TARGET && defined __cplusplus
}
#endif
