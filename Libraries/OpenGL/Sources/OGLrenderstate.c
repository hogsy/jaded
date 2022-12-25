/*$T OGLrenderstate.c GC!1.71 03/06/00 15:23:24 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/* Aim: OpenGL render state */
#include "Precomp.h"

#ifdef PSX2_TARGET
#include "PS2GL.h"
#include "PS2GL_Redefine.h"

#else
#include <gl/gl.h>
#endif



#include "OGLinit.h"
#include "BASe/BAStypes.h"
#include "TEXture/TEXstruct.h"


#if defined PSX2_TARGET && defined __cplusplus
    extern "C" {
#endif


/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_RS_Init(OGL_tdst_RenderState *_pst_RS)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int             i_Params[2];
    unsigned char   uc_Param;
    float           f_Param;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    glGetIntegerv(GL_POLYGON_MODE, i_Params);
    _pst_RS->c_Wired = (i_Params[1] == GL_LINE) ? 1 : 0;
    _pst_RS->c_CullFace = glIsEnabled(GL_CULL_FACE);
    _pst_RS->l_LastTexture = -1;
    glGetIntegerv(GL_DEPTH_FUNC, i_Params);
    _pst_RS->l_DepthFunc = i_Params[0];
    glGetBooleanv(GL_DEPTH_WRITEMASK, &uc_Param);
    _pst_RS->c_DepthMask = uc_Param;
    glGetFloatv(GL_LINE_WIDTH, &f_Param);
    _pst_RS->f_LineWidth = f_Param;
    glGetFloatv(GL_POINT_SIZE, &f_Param);
    _pst_RS->f_PointSize = f_Param;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_RS_DrawWired(OGL_tdst_RenderState *_pst_RS, LONG _l_Wired)
{
    if(_l_Wired)
    {
        if(!_pst_RS->c_Wired)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            _pst_RS->c_Wired = 1;
        }
    }
    else
    {
        if(_pst_RS->c_Wired)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            _pst_RS->c_Wired = 0;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_RS_CullFace(OGL_tdst_RenderState *_pst_RS, LONG _l_CullFace)
{
    if(_l_CullFace)
    {
        if(!_pst_RS->c_CullFace)
        {
            glEnable(GL_CULL_FACE);
            _pst_RS->c_CullFace = 1;
        }
    }
    else
    {
        if(_pst_RS->c_CullFace)
        {
            glDisable(GL_CULL_FACE);
            _pst_RS->c_CullFace = 0;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_RS_CullFaceInverted(OGL_tdst_RenderState *_pst_RS, LONG _l_CullFaceInverted)
{
    if(_pst_RS->c_CullFace)
    {
        if(_l_CullFaceInverted)
        {
            if(!_pst_RS->c_CullFaceInverted)
            {
                glCullFace(GL_FRONT);
                _pst_RS->c_CullFaceInverted = 1;
            }
        }
        else if(_pst_RS->c_CullFaceInverted)
        {
            glCullFace(GL_BACK);
            _pst_RS->c_CullFaceInverted = 0;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_RS_Fogged(OGL_tdst_RenderState *_pst_RS, LONG _l_FogOn)
{
    if(_l_FogOn)
    {
        //if(!_pst_RS->c_Fogged)
        {
            glEnable(GL_FOG);
            _pst_RS->c_Fogged = 1;
        }
    }
    else
    {
        //if(_pst_RS->c_Fogged)
        {
            glDisable(GL_FOG);
            _pst_RS->c_Fogged = 0;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_RS_DepthFunc(OGL_tdst_RenderState *_pst_RS, LONG _l_Func)
{
    if(_pst_RS->l_DepthFunc != _l_Func)
    {
        glDepthFunc(_l_Func);
        _pst_RS->l_DepthFunc = _l_Func;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_RS_DepthTest(OGL_tdst_RenderState *_pst_RS, LONG _l_Test)
{
    if (_l_Test)
        glEnable( GL_DEPTH_TEST );
    else
        glDisable( GL_DEPTH_TEST );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_RS_DepthMask(OGL_tdst_RenderState *_pst_RS, LONG _l_DepthMask)
{
    if(_l_DepthMask)
    {
        if(!_pst_RS->c_DepthMask)
        {
            glDepthMask(GL_TRUE);
            _pst_RS->c_DepthMask = 1;
        }
    }
    else
    {
        if(_pst_RS->c_DepthMask)
        {
            glDepthMask(GL_FALSE);
            _pst_RS->c_DepthMask = 0;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_RS_SetPalette( LONG _l_Palette )
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG OGL_RS_UseTexture(OGL_tdst_SpecificData *_pst_SD, LONG _l_Texture)
{
    TEX_tdst_Data *pst_Tex;

    if(_l_Texture != _pst_SD->st_RS.l_LastTexture)
    {
		if(_l_Texture == -1)
		{
			glDisable(GL_TEXTURE_2D);
		}
		else
		{
			if (_l_Texture & 0x80000000)
			{
				extern void OGL_ShadowSelect(u32 TexNum);
				// Use a shadowTexturte
				OGL_ShadowSelect(_l_Texture);
			} else
			{
				if(_pst_SD->dul_Texture)
				{
					if (_pst_SD->dul_Texture[ _l_Texture ] == -1 )
						glDisable(GL_TEXTURE_2D);
					else
					{
						if (_pst_SD->dul_Texture[ _l_Texture ] == 0 )
						{
							pst_Tex = &TEX_gst_GlobalList.dst_Texture[ _l_Texture ];
							/* raw + pal */
							if (pst_Tex->uw_Flags & TEX_uw_RawPal)
								_l_Texture = pst_Tex->w_Height;
							/* animated texture */
							else if (pst_Tex->uw_Flags & TEX_uw_Ani)
							{
								_l_Texture = pst_Tex->w_Height;
								if (_l_Texture == -1)
									glDisable( GL_TEXTURE_2D );
								else if (_pst_SD->dul_Texture[ _l_Texture ] == 0 )
								{
									pst_Tex = &TEX_gst_GlobalList.dst_Texture[ _l_Texture ];
									if (pst_Tex->uw_Flags & TEX_uw_RawPal)
										_l_Texture = pst_Tex->w_Height;
								}
							}
						}

						/*$F test bump
						if (_pst_SD->dul_Texture[ _l_Texture ] & OGL_C_BumpTexFlag)
						{
							glEnable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D, _pst_SD->dst_BumpTex[ _pst_SD->dul_Texture[ _l_Texture ] & 0xFFFF ].ul_Bump );
						}
						else
						{
						*/
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, _pst_SD->dul_Texture[_l_Texture]);
					}

				}
				else
				{
					glDisable(GL_TEXTURE_2D);
				}
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
void OGL_RS_LineWidth(OGL_tdst_RenderState *_pst_RS, float _f_Width)
{
    if(_f_Width != _pst_RS->f_LineWidth)
    {
        glLineWidth(_f_Width);
        _pst_RS->f_LineWidth = _f_Width;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_RS_PointSize(OGL_tdst_RenderState *_pst_RS, float _f_Size)
{
    if(_f_Size != _pst_RS->f_PointSize)
    {
        glPointSize(_f_Size);
        _pst_RS->f_PointSize = _f_Size;
    }
}

#if defined PSX2_TARGET && defined __cplusplus
    }
#endif
