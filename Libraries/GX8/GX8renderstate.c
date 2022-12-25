/*$T Gx8renderstate.c GC! 1.081 01/15/02 15:40:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Gx8renderstate.h"

/* Aim: OpenGL render state */
#include <xtl.h>
#include <D3D8.h>

#include "Gx8init.h"
#include "BASe/BAStypes.h"
#include "TEXture/TEXstruct.h"
#include "GDInterface/GDInterface.h"
#include "RASter/Gx8_CheatFlags.h"

extern ULONG					LIGHT_ul_Interpol2Colors(ULONG, ULONG, float);

extern Gx8_tdst_SpecificData	*p_gGx8SpecificData;
extern BOOL RenderInNormal;
extern D3DPalette *g_aPal[];
extern BOOL Normalmap;
extern BOOL RenderInNormal;
/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */
#define USE_TANGENTE
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_RS_Init(Gx8_tdst_SpecificData *_pst_SD)
{
    _pst_SD->st_RS.l_LastTexture = NULL;
    IDirect3DDevice8_SetRenderState(_pst_SD->mp_D3DDevice, D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    _pst_SD->st_RS.l_DepthFunc = D3DCMP_LESSEQUAL;
    //_pst_RS->f_LineWidth;
    //_pst_RS->f_PointSize;
	IDirect3DDevice8_SetRenderState(_pst_SD->mp_D3DDevice, D3DRS_FILLMODE, D3DFILL_SOLID);
	_pst_SD->st_RS.c_Wired = 0;
    IDirect3DDevice8_SetRenderState(_pst_SD->mp_D3DDevice, D3DRS_ZWRITEENABLE, 1/*TRUE*/);
    _pst_SD->st_RS.c_DepthMask = 1;
    IDirect3DDevice8_SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_LIGHTING, FALSE);
    _pst_SD->st_RS.c_Lighting = 0;
    IDirect3DDevice8_SetRenderState(_pst_SD->mp_D3DDevice, D3DRS_FOGENABLE, 1/*TRUE*/);
    _pst_SD->st_RS.c_Fogged = 1;
    IDirect3DDevice8_SetRenderState(_pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE);
    IDirect3DDevice8_SetRenderState(_pst_SD->mp_D3DDevice, D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
    IDirect3DDevice8_SetRenderState(_pst_SD->mp_D3DDevice, D3DRS_SRCBLEND, D3DBLEND_ONE);
    IDirect3DDevice8_SetRenderState(_pst_SD->mp_D3DDevice, D3DRS_DESTBLEND, D3DBLEND_ZERO);
    IDirect3DDevice8_SetRenderState( _pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_CCW);
    _pst_SD->st_RS.e_CullFace = GX8_C_CULLFACE_NORMAL;
#ifdef _XBOX
    IDirect3DDevice8_SetRenderState(_pst_SD->mp_D3DDevice, D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL);
#endif

    IDirect3DDevice8_SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
    IDirect3DDevice8_SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
    _pst_SD->st_RS.dwCurrentVertexShader = 0xFFFFFFFF;
	_pst_SD->st_RS.dwCurrentPixelShader = 0xFFFFFFFF;

    IDirect3DDevice8_SetRenderState(_pst_SD->mp_D3DDevice, D3DRS_NORMALIZENORMALS, 1/*TRUE*/);

    // default values
    // take the per-vertex diffuse (RLI) as an emmissive color, so that we can use RLI's and HW light at the same time
    // This does not work on all PC cards, so in that case we have to switch to SW vertex processing...
#if (!defined(_XBOX)) && defined(_DEBUG)
    if (!(_pst_SD->m_d3dCaps.VertexProcessingCaps & D3DVTXPCAPS_MATERIALSOURCE7))
    {
        OutputDebugString("TODO: RLI+lights management when device doesn't support selectable vertex color sources.\n"); // see comments above
    }
#endif (!defined(_XBOX)) && defined(_DEBUG)
    IDirect3DDevice8_SetRenderState(_pst_SD->mp_D3DDevice, D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL); // default was D3DMCS_COLOR1
    IDirect3DDevice8_SetRenderState(_pst_SD->mp_D3DDevice, D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
    IDirect3DDevice8_SetRenderState(_pst_SD->mp_D3DDevice, D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL); // beware of the DX8 doc bug...
    IDirect3DDevice8_SetRenderState(_pst_SD->mp_D3DDevice, D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1); // default was D3DMCS_MATERIAL

	Gx8_vSetNormalMaterial();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_RS_SetPalette(LONG _l_Palette)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG Gx8_RS_UseTexture(Gx8_tdst_SpecificData *_pst_SD, LONG _l_Texture)
{
    TEX_tdst_Data *pst_Tex;
    IDirect3DBaseTexture8 *T;

	Gx8_SetUVStageOff();

    if(_l_Texture != _pst_SD->st_RS.l_LastTexture)
    {
        if ( (_l_Texture == -1) || ( !_pst_SD->dul_Texture ) )
        {
            IDirect3DDevice8_SetTexture( _pst_SD->mp_D3DDevice,0, NULL );
			/*
            IDirect3DDevice8_SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		    IDirect3DDevice8_SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		    IDirect3DDevice8_SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		    IDirect3DDevice8_SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
             */
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
                
                    // raw + pal
                    if (pst_Tex->uw_Flags & TEX_uw_RawPal)
                    {
						if (pst_Tex->w_Width>=0)// ca parait dingue, mais y'a des -1 ???!!!
						{
	                        _l_Texture = pst_Tex->w_Height;
	                        IDirect3DDevice8_SetPalette (_pst_SD->mp_D3DDevice, 0, g_aPal[pst_Tex->w_Width]);
						}
                    }

                    /* animated texture */
                    /*else*/ if (pst_Tex->uw_Flags & TEX_uw_Ani)
                    {
                        _l_Texture = pst_Tex->w_Height;
                        if (_l_Texture == -1)
                        {
                            return Gx8_RS_UseTexture( _pst_SD, -1 );
                        }
                        else if (_pst_SD->dul_Texture[ _l_Texture ] == 0 )
                        {
                            pst_Tex = &TEX_gst_GlobalList.dst_Texture[ _l_Texture ];
                            if (pst_Tex->uw_Flags & TEX_uw_RawPal)
                            {
								if (pst_Tex->w_Width>=0)// ca parait dingue, mais y'a des -1 ???!!!
								{
									_l_Texture = pst_Tex->w_Height;
									IDirect3DDevice8_SetPalette (_pst_SD->mp_D3DDevice, 0, g_aPal[pst_Tex->w_Width]);
								}
							}
                        }
                    }
                }

                T = (IDirect3DBaseTexture8 *) _pst_SD->dul_Texture[_l_Texture];
            }
            
            IDirect3DDevice8_SetTexture( _pst_SD->mp_D3DDevice,0, T );
			
		}

        _pst_SD->st_RS.l_LastTexture = _l_Texture;
    }
    
    return _l_Texture;
}

LONG Gx8_RS_UseTextureNormal(Gx8_tdst_SpecificData *_pst_SD, LONG _l_Texture)
{
    TEX_tdst_Data *pst_Tex;
    IDirect3DBaseTexture8 *T;

	//Gx8_SetUVStageOff();

    if(_l_Texture != _pst_SD->st_RS.l_LastTexture)
    {
        if ( (_l_Texture == -1) || ( !_pst_SD->dul_Texture ) )
        {
            IDirect3DDevice8_SetTexture( _pst_SD->mp_D3DDevice,1, NULL );
			/*
            IDirect3DDevice8_SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		    IDirect3DDevice8_SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		    IDirect3DDevice8_SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		    IDirect3DDevice8_SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
             */
        }
        else
        {
               if (_pst_SD->dul_Texture[ _l_Texture ] == 0 )
                {
                    pst_Tex = &TEX_gst_GlobalList.dst_Texture[ _l_Texture ];
                }

                T = (IDirect3DBaseTexture8 *) _pst_SD->dul_Texture[_l_Texture];
            
            IDirect3DDevice8_SetTexture( _pst_SD->mp_D3DDevice,1, T );
			
		}

        _pst_SD->st_RS.l_LastTexture = _l_Texture;
    }
    
    return _l_Texture;
}



LONG Gx8_RS_UseTextureMT(Gx8_tdst_SpecificData *_pst_SD, LONG _l_Texture, UINT stage)
{
    TEX_tdst_Data *pst_Tex;
    IDirect3DBaseTexture8 *T;

	Gx8_SetUVStageOff();

    if(_l_Texture != _pst_SD->st_RS.l_LastTexture)
    {
        if ( (_l_Texture == -1) || ( !_pst_SD->dul_Texture ) )
        {
            IDirect3DDevice8_SetTexture( _pst_SD->mp_D3DDevice,stage, NULL );
			/*
            IDirect3DDevice8_SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		    IDirect3DDevice8_SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		    IDirect3DDevice8_SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		    IDirect3DDevice8_SetTextureStageState(_pst_SD->mp_D3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
             */
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
                
                    // raw + pal
                    if (pst_Tex->uw_Flags & TEX_uw_RawPal)
                    {
						if (pst_Tex->w_Width>=0)// ca parait dingue, mais y'a des -1 ???!!!
						{
	                        _l_Texture = pst_Tex->w_Height;
	                        IDirect3DDevice8_SetPalette (_pst_SD->mp_D3DDevice, stage, g_aPal[pst_Tex->w_Width]);
						}
                    }

                    /* animated texture */
                    /*else*/ if (pst_Tex->uw_Flags & TEX_uw_Ani)
                    {
                        _l_Texture = pst_Tex->w_Height;
                        if (_l_Texture == -1)
                        {
                            return Gx8_RS_UseTextureMT( _pst_SD, -1, stage );
                        }
                        else if (_pst_SD->dul_Texture[ _l_Texture ] == 0 )
                        {
                            pst_Tex = &TEX_gst_GlobalList.dst_Texture[ _l_Texture ];
                            if (pst_Tex->uw_Flags & TEX_uw_RawPal)
                            {
								if (pst_Tex->w_Width>=0)// ca parait dingue, mais y'a des -1 ???!!!
								{
									_l_Texture = pst_Tex->w_Height;
									IDirect3DDevice8_SetPalette (_pst_SD->mp_D3DDevice, stage, g_aPal[pst_Tex->w_Width]);
								}
							}
                        }
                    }
                }

                T = (IDirect3DBaseTexture8 *) _pst_SD->dul_Texture[_l_Texture];
            }
            
            IDirect3DDevice8_SetTexture( _pst_SD->mp_D3DDevice,stage, T );
        }

        _pst_SD->st_RS.l_LastTexture = _l_Texture;
    }
    
    return _l_Texture;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_RS_LineWidth(Gx8_tdst_RenderState *_pst_RS, float _f_Width)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Gx8_RS_PointSize(Gx8_tdst_RenderState *_pst_RS, float _f_Size)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void Gx8_SetRastersRenderState (void)
{
	Gx8_tdst_SpecificData *pst_SD = GX8_M_SD( GDI_gpst_CurDD );

    GDI_gpst_CurDD->LastBlendingMode |= MAT_Cul_Flag_AlphaTest;
    Gx8_M_RenderState(pst_SD, D3DRS_ALPHATESTENABLE, TRUE);
    GDI_gpst_CurDD->LastBlendingMode &= ~MAT_Cul_Flag_InvertAlpha;
    Gx8_M_RenderState(pst_SD, D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    MAT_SET_Blending(GDI_gpst_CurDD->LastBlendingMode, MAT_Cc_Op_Copy);
    Gx8_M_RenderState(pst_SD, D3DRS_ALPHABLENDENABLE, FALSE);

	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	IDirect3DDevice8_SetTextureStageState(pst_SD->mp_D3DDevice, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	IDirect3DDevice8_SetRenderState(pst_SD->mp_D3DDevice, D3DRS_ALPHAREF, (DWORD)0);
}

void Gx8_SetUVStageMatrix(D3DMATRIX *pstMatrix)
{
	IDirect3DDevice8_SetTransform(p_gGx8SpecificData->mp_D3DDevice,D3DTS_TEXTURE0, pstMatrix);
if (Normalmap && RenderInNormal)
{
	//yo
	IDirect3DDevice8_SetTransform(p_gGx8SpecificData->mp_D3DDevice,D3DTS_TEXTURE1, pstMatrix);
}
}

static Gx8_tdeUVGenerationType gs_eUVType = Gx8_eUVOff;
void Gx8_SetUVChromeStage(void)
{
	
	if (gs_eUVType != Gx8_eUVChrome)
	{
		gs_eUVType = Gx8_eUVChrome;
		IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
		IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	}
	
}

void Gx8_SetUVNormalStage(void)
{
	
	if (gs_eUVType != Gx8_eUVNormal)
	{
		gs_eUVType = Gx8_eUVNormal;
		IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
		IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
//yo
#ifdef USE_TANGENTE
		if ( RenderInNormal )
		{
			IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
			IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		}
#endif
	}
	
}

void Gx8_SetUVStageOff(void)
{
	
	if (gs_eUVType != Gx8_eUVOff)
	{
		gs_eUVType = Gx8_eUVOff;
		IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXCOORDINDEX, 0);
		IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	}
	
}

void Gx8_SetUVRotationStage(void)
{
	
	if (gs_eUVType != Gx8_eUVRotation)
	{
		gs_eUVType = Gx8_eUVRotation;
		IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
		IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	}
	
}

Gx8_tdeUVGenerationType Gx8_fneGetCurrentUVState(void)
{
	return gs_eUVType;
}

