/*$T OGLrequest.c GC! 1.081 03/12/04 10:57:01 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#include "ddraw.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "OGLinit.h"
#include "OGLtex.h"
#include "OGLrequest.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BAStypes.h"
#include "TEXture/TEXfile.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEXstruct.h"

#include "OGLdebugfct.h"

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

LONG				OGL_l_FullScreenMode = 0;
RECT				OGL_gst_WindowPos;
HWND				OGL_gh_FullScreenWnd;
HWND				OGL_gh_WindowedHwndSave;


void OGL_SetColorFor2x(GLubyte *Color)
{
	GLubyte ColorRGBA[4];
	if (!GDI_gpst_CurDD->GlobalMul2X) 
	{
		glColor4ubv(Color);
		return;
	}
	ColorRGBA[0] = Color[0]>>1;
	ColorRGBA[1] = Color[1]>>1;
	ColorRGBA[2] = Color[2]>>1;
	ColorRGBA[3] = Color[3];
	glColor4ubv((GLubyte *)ColorRGBA);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_SetFogParams(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_FogParams *_pst_Fog)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static int				OGL_sai_FogMode[3] = { GL_LINEAR, GL_EXP, GL_EXP2 };
	OGL_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = (OGL_tdst_SpecificData *) _pst_DD->pv_SpecificData;

	if(_pst_Fog->c_Flag & SOFT_C_FogActive)
	{
		pst_SD->fFogColor[0] = (_pst_Fog->ul_Color & 0xFF) / 255.0f;
		pst_SD->fFogColor[1] = ((_pst_Fog->ul_Color >> 8) & 0xFF) / 255.0f;
		pst_SD->fFogColor[2] = ((_pst_Fog->ul_Color >> 16) & 0xFF) / 255.0f;
		pst_SD->fFogColor[3] = ((_pst_Fog->ul_Color >> 24) & 0xFF) / 255.0f;
		pst_SD->fFogBlack[0] = pst_SD->fFogBlack[1] = pst_SD->fFogBlack[2] = pst_SD->fFogBlack[3] = 0.0f;
		pst_SD->fFogColorOn2[0] = pst_SD->fFogColor[0] / 2.0f;
		pst_SD->fFogColorOn2[1] = pst_SD->fFogColor[1] / 2.0f;
		pst_SD->fFogColorOn2[2] = pst_SD->fFogColor[2] / 2.0f;
		pst_SD->fFogColorOn2[3] = pst_SD->fFogColor[3] / 2.0f;

		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, OGL_sai_FogMode[_pst_Fog->c_Mode]);
		glFogf(GL_FOG_DENSITY, _pst_Fog->f_Density);

		pst_SD->ulFogState = 1;

		glFogfv(GL_FOG_COLOR, pst_SD->fFogColor);
		glFogf(GL_FOG_START, _pst_Fog->f_Start);
		glFogf(GL_FOG_END, _pst_Fog->f_End);
	}
	else
	{
		pst_SD->ulFogState = 0;
		glDisable(GL_FOG);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_Draw2DTriangle(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Vertex *_pst_TV)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ulBlendingMode;
	/*~~~~~~~~~~~~~~~~~~~*/

	ulBlendingMode = 0;

	if((_pst_TV->color >> 24) == 0xFF)
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	else
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Alpha);
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);

	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin(GL_TRIANGLES);

	glTexCoord2fv(&_pst_TV[0].u);
	OGL_SetColorFor2x((GLubyte *) &_pst_TV[0].color);
	glVertex3fv((float *) &_pst_TV[0]);

	glTexCoord2fv(&_pst_TV[1].u);
	OGL_SetColorFor2x((GLubyte *) &_pst_TV[1].color);
	glVertex3fv((float *) &_pst_TV[1]);

	glTexCoord2fv(&_pst_TV[2].u);
	OGL_SetColorFor2x((GLubyte *) &_pst_TV[2].color);
	glVertex3fv((float *) &_pst_TV[2]);

	glEnd();
}

/*
 =======================================================================================================================
    Aim:    Draw a point
 =======================================================================================================================
 */
void OGL_DrawPoint(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ulBlendingMode;
	/*~~~~~~~~~~~~~~~~~~~*/

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);

	OGL_RS_PointSize(OGL_M_RS(_pst_DD), 5.0f);
	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin(GL_POINTS);
	OGL_SetColorFor2x((GLubyte *) &_pst_DD->ul_ColorConstant);
	glVertex3fv((float *) v);
	glEnd();
	OGL_RS_PointSize(OGL_M_RS(_pst_DD), 1.0f);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_DrawPointMin(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ulBlendingMode;
	/*~~~~~~~~~~~~~~~~~~~*/

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);

	OGL_RS_PointSize(OGL_M_RS(_pst_DD), 3.0f);
	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin(GL_POINTS);
	OGL_SetColorFor2x((GLubyte *) &_pst_DD->ul_ColorConstant);
	glVertex3fv((float *) v);
	glEnd();
	OGL_RS_PointSize(OGL_M_RS(_pst_DD), 1.0f);
}

/*
 =======================================================================================================================
    Aim:    draw a point with a given size
 =======================================================================================================================
 */
void OGL_DrawPoint_Size(GDI_tdst_DisplayData *_pst_DD, void **peewee)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ulBlendingMode;
	MATH_tdst_Vector	*v;
	float				*Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	v = (MATH_tdst_Vector *) peewee[0];
	Size = (float *) peewee[1];

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);

	OGL_RS_PointSize(OGL_M_RS(_pst_DD), *Size);
	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin(GL_POINTS);
	OGL_SetColorFor2x((GLubyte *) &_pst_DD->ul_ColorConstant);
	glVertex3fv((float *) v);
	glEnd();
	OGL_RS_PointSize(OGL_M_RS(_pst_DD), 1.0f);
}

/*
 =======================================================================================================================
    Aim:    draw a point with a given size and color
 =======================================================================================================================
 */
void OGL_DrawPointEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawPointEx *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ulBlendingMode;
	/*~~~~~~~~~~~~~~~~~~~*/

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);

	OGL_RS_PointSize(OGL_M_RS(_pst_DD), _pst_Data->f_Size);
	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin(GL_POINTS);
	OGL_SetColorFor2x((GLubyte *) &_pst_Data->ul_Color);
	glVertex3fv((float *) _pst_Data->A);
	glEnd();
	OGL_RS_PointSize(OGL_M_RS(_pst_DD), 1.0f);
}

/*
 =======================================================================================================================
    Aim:    draw a point with a given size and color
 =======================================================================================================================
 */
void OGL_PointSize(GDI_tdst_DisplayData *_pst_DD, float _f_Size)
{
	if(*(LONG *) &_f_Size == -1)
		OGL_RS_PointSize(OGL_M_RS(_pst_DD), 1.0f);
	else
		OGL_RS_PointSize(OGL_M_RS(_pst_DD), _f_Size);
}

/*
 =======================================================================================================================
    Aim:    Draw a line
 =======================================================================================================================
 */
void OGL_DrawLine(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector **v)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ulBlendingMode;
	/*~~~~~~~~~~~~~~~~~~~*/

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);

	OGL_RS_LineWidth(OGL_M_RS(_pst_DD), 1.0f);
	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin(GL_LINES);
	OGL_SetColorFor2x((GLubyte *) &_pst_DD->ul_ColorConstant);
	glVertex3fv((float *) v[0]);
	glVertex3fv((float *) v[1]);
	glEnd();
	OGL_RS_LineWidth(OGL_M_RS(_pst_DD), 1.0f);
}

/*
 =======================================================================================================================
    Aim:    Draw a line with width and color given
 =======================================================================================================================
 */

void OGL_DrawLineEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawLineEx *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ulBlendingMode;
	MATH_tdst_Vector	X, Y, Z, V[4];
	float				f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	if (_pst_Data->ul_Flags & GDI_Request_DrawLine_NoZWrite)
	{
		MAT_SET_FLAG(ulBlendingMode, MAT_Cul_Flag_NoZWrite);
	}
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);

	OGL_RS_LineWidth(OGL_M_RS(_pst_DD), _pst_Data->f_Width);
	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin(GL_LINES);
	OGL_SetColorFor2x((GLubyte *) &_pst_Data->ul_Color);
	glVertex3fv((float *) _pst_Data->A);
	glVertex3fv((float *) _pst_Data->B);
	glEnd();
	OGL_RS_LineWidth(OGL_M_RS(_pst_DD), 1.0f);

	if(_pst_Data->ul_Flags & GDI_Request_DrawLine_Arrow)
	{
		/* finding axe */
		MATH_SubVector(&Z, _pst_Data->B, _pst_Data->A);
		if(!MATH_b_NulVector(&Z))
		{
			if(fabs(Z.x) < fabs(Z.y))
				(fAbs(Z.x) < fAbs(Z.z)) ? MATH_InitVector(&X, 1, 0, 0) : MATH_InitVector(&X, 0, 0, 1);
			else
				(fAbs(Z.y) < fAbs(Z.z)) ? MATH_InitVector(&X, 0, 1, 0) : MATH_InitVector(&X, 0, 0, 1);

			MATH_CrossProduct(&Y, &Z, &X);
			MATH_NormalizeVector(&Y, &Y);
			MATH_CrossProduct(&X, &Y, &Z);
			MATH_NormalizeVector(&X, &X);

			f = MATH_f_NormVector(&Z);
			MATH_ScaleEqualVector(&Z, fInv(f));
			f /= 10;
			if(f > 1) f = 1;
			MATH_ScaleEqualVector(&Z, -f);
			MATH_AddEqualVector(&Z, _pst_Data->B);
			MATH_ScaleEqualVector(&X, f / 2);
			MATH_ScaleEqualVector(&Y, f / 2);

			MATH_AddVector(V, &Z, &X);
			MATH_AddVector(V + 1, &Z, &Y);
			MATH_SubVector(V + 2, &Z, &X);
			MATH_SubVector(V + 3, &Z, &Y);

			OGL_RS_DrawWired(OGL_M_RS(_pst_DD), 0);
			GDI_gpst_CurDD->profilingInformation.numBatches++;
			glBegin(GL_TRIANGLE_FAN);
			glVertex3fv((float *) _pst_Data->B);
			glVertex3fv((float *) V);
			glVertex3fv((float *) (V + 1));
			glVertex3fv((float *) (V + 2));
			glVertex3fv((float *) (V + 3));
			glVertex3fv((float *) V);
			glEnd();
			
			GDI_gpst_CurDD->profilingInformation.numBatches++;
			glBegin(GL_TRIANGLE_FAN);
			glVertex3fv((float *) V);
			glVertex3fv((float *) (V + 3));
			glVertex3fv((float *) (V + 2));
			glVertex3fv((float *) (V + 1));
			glEnd();
		}
	}
	
	//if(_pst_Data->ul_Flags & GDI_Request_DrawLine_Text)
	/*{
		MATH_AddVector( &X, _pst_Data->A, _pst_Data->B );
		MATH_ScaleEqualVector( &X, 0.5f );
		
	}
	*/
}

/*
 =======================================================================================================================
    Aim:    Draw a line with width and color given
 =======================================================================================================================
 */
void OGL_LineSize(GDI_tdst_DisplayData *_pst_DD, float _f_Size)
{
	if(*(LONG *) &_f_Size == -1)
		OGL_RS_LineWidth(OGL_M_RS(_pst_DD), 1.0F);
	else
		OGL_RS_LineWidth(OGL_M_RS(_pst_DD), _f_Size);
}

/*
 =======================================================================================================================
    Aim:    Draw a quad
 =======================================================================================================================
 */
void OGL_DrawQuad(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector **v)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ulBlendingMode;
	/*~~~~~~~~~~~~~~~~~~~*/

	ulBlendingMode = 0;
	if((_pst_DD->ul_ColorConstant & 0xFF000000) != 0xFF000000)
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Alpha);
	else
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);
	OGL_RS_CullFace(&OGL_M_SD(_pst_DD)->st_RS, 0);

	OGL_RS_LineWidth(OGL_M_RS(_pst_DD), 1.0f);

	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin(GL_QUADS);
	OGL_SetColorFor2x((GLubyte *) &_pst_DD->ul_ColorConstant);
	glVertex3fv((float *) v[0]);
	glVertex3fv((float *) v[1]);
	glVertex3fv((float *) v[2]);
	glVertex3fv((float *) v[3]);
	glEnd();
}

void OGL_PushZBuffer(GDI_tdst_DisplayData *_pst_DD, float Z)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ulBlendingMode;
	MATH_tdst_Matrix ViewMatrix;
	MATH_tdst_Vector	QUAD[4];
	u32 ColorRGBA;
	/*~~~~~~~~~~~~~~~~~~~*/

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
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);

	glDepthFunc(GL_GEQUAL);
	OGL_RS_CullFace(&OGL_M_SD(_pst_DD)->st_RS, 0);
	ColorRGBA = 0;

	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin(GL_QUADS);
	glColor4ubv((GLubyte *)&ColorRGBA);
	glVertex3fv((float *) &QUAD[0]);
	glVertex3fv((float *) &QUAD[1]);
	glVertex3fv((float *) &QUAD[2]);
	glVertex3fv((float *) &QUAD[3]);
	glEnd();
	glDepthFunc(GL_LEQUAL);
}

/*
 =======================================================================================================================
    Aim:    Draw a quad
 =======================================================================================================================
 */
void OGL_DrawQuadEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawQuad *quad )
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ulBlendingMode;
	MATH_tdst_Vector	A, B, u, v, n;
	float				fn;
	int					i;
	GDI_tdst_Request_DrawLineEx	st_Arrow;
	/*~~~~~~~~~~~~~~~~~~~*/
	
	if( quad->ul_Flags & GDI_Request_DrawQuad_Normal )
	{
		MATH_InitVectorToZero( &A );
		for (i = 0;i < 4; i++)
			MATH_AddEqualVector( &A, quad->V[ i ] );
		MATH_ScaleEqualVector( &A, 0.25f );
		
		MATH_SubVector( &u, quad->V[ 1 ], quad->V[ 0 ] ); 
		MATH_SubVector( &v, quad->V[ 3 ], quad->V[ 0 ] );
		
		MATH_CrossProduct( &n, &u, &v );
		fn = MATH_f_NormVector( &n );
		
		if ( fn != 0 )
		{
			MATH_ScaleEqualVector( &n , 1.0f / fn );
			MATH_AddVector( &B, &A, &n );
			st_Arrow.A = &A;
			st_Arrow.B = &B;
			st_Arrow.f_Width = quad->f_LineWidth;
			st_Arrow.ul_Color = ~quad->ul_EdgeColor;
			st_Arrow.ul_Flags = GDI_Request_DrawVector_Arrow;
			OGL_DrawLineEx( _pst_DD, &st_Arrow );
		}
	}

	if(!(quad->ul_Flags & GDI_Request_DrawQuad_OnlyLine))
	{
		ulBlendingMode = 0;
		if((quad->ul_SolidColor & 0xFF000000) != 0xFF000000)
			MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Alpha);
		else
			MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
		OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);
	
		OGL_RS_CullFace(&OGL_M_SD(_pst_DD)->st_RS, (quad->ul_Flags & GDI_Request_DrawQuad_CullFace) ? 1 : 0);
		OGL_RS_DrawWired(OGL_M_RS(_pst_DD), 0);

		OGL_RS_LineWidth(OGL_M_RS(_pst_DD), 1.0f);

		GDI_gpst_CurDD->profilingInformation.numBatches++;
		glBegin(GL_QUADS);
		OGL_SetColorFor2x((GLubyte *) &quad->ul_SolidColor);
		glVertex3fv((float *) quad->V[0]);
		glVertex3fv((float *) quad->V[1]);
		glVertex3fv((float *) quad->V[2]);
		glVertex3fv((float *) quad->V[3]);
		glEnd();
	}
	
	if(quad->ul_Flags & (GDI_Request_DrawQuad_OnlyLine | GDI_Request_DrawQuad_Line))
	{
		ulBlendingMode = 0;
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
		OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);
		OGL_RS_LineWidth(OGL_M_RS(_pst_DD), quad->f_LineWidth);

		GDI_gpst_CurDD->profilingInformation.numBatches++;
		glBegin(GL_LINES);
		OGL_SetColorFor2x((GLubyte *) &quad->ul_EdgeColor);
		
		glVertex3fv((float *) quad->V[0]);
		glVertex3fv((float *) quad->V[1]);
		
		glVertex3fv((float *) quad->V[1]);
		glVertex3fv((float *) quad->V[2]);
		
		glVertex3fv((float *) quad->V[2]);
		glVertex3fv((float *) quad->V[3]);
		
		glVertex3fv((float *) quad->V[3]);
		glVertex3fv((float *) quad->V[0]);
		glEnd();
	}
}

/*
 =======================================================================================================================
    Aim:    Draw a Sprite
 =======================================================================================================================
 */
void OGL_DrawSprite(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static float	tf_UV[5] = { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	OGL_SetColorFor2x((GLubyte *) &v[4].x);
	glTexCoord2fv(&tf_UV[3]);	/* 00 */
	glVertex3fv((float *) &v[0]);
	glTexCoord2fv(&tf_UV[2]);	/* 10 */
	glVertex3fv((float *) &v[1]);
	glTexCoord2fv(&tf_UV[1]);	/* 11 */
	glVertex3fv((float *) &v[2]);
	glTexCoord2fv(&tf_UV[0]);	/* 01 */
	glVertex3fv((float *) &v[3]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_BeforeDrawSprite(GDI_tdst_DisplayData *_pst_DD)
{
#ifdef ACTIVE_EDITORS
	if(!(_pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired))
	{
		/*~~~~~~~~~~~~~~~~~~~*/
		ULONG	ulBlendingMode;
		/*~~~~~~~~~~~~~~~~~~~*/

		ulBlendingMode = 0;

		OGL_RS_LineWidth(OGL_M_RS(_pst_DD), 1.0f);
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
		OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);
	}

	OGL_RS_DrawWired(OGL_M_RS(_pst_DD), !(_pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired));
#else
	OGL_RS_DrawWired(OGL_M_RS(_pst_DD), 0);
#endif
	OGL_RS_CullFaceInverted(OGL_M_RS(_pst_DD), 0);

	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin(GL_QUADS);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_AfterDrawSprite(GDI_tdst_DisplayData *_pst_DD)
{
	glEnd();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_DrawBox(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawBox *_pst_Box)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG						ul_SaveColor;
	ULONG						ulBlendingMode;
	GDI_tdst_Request_DrawLineEx st_Line;
	MATH_tdst_Vector			V[8], *Quad[4];
	int							i;
	/*$off*/
	char						aac_Edge[ 12 ][ 2 ] = {{0,1},{0,2},{0,4},{3,1},{3,2},{3,7},{5,4},{5,7},{5,1},{6,4},{6,7},{6,2} };
	char						aac_Face[ 6 ][ 4 ] = {{0,1,3,2},{1,3,7,5},{0,1,5,4},{3,2,6,7},{0,2,6,4},{4,5,7,6} };
	/*$on*/
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < 8; i++)
	{
		V[i].x = ((i & 1) ? _pst_Box->pst_Max : _pst_Box->pst_Min)->x;
		V[i].y = ((i & 2) ? _pst_Box->pst_Max : _pst_Box->pst_Min)->y;
		V[i].z = ((i & 4) ? _pst_Box->pst_Max : _pst_Box->pst_Min)->z;

		MATH_TransformVector(V + i, _pst_Box->pst_M, V + i);
		MATH_AddEqualVector(V + i, _pst_Box->pst_Center);
	}

	st_Line.f_Width = _pst_Box->f_Width;
	st_Line.ul_Color = _pst_Box->ul_EdgeColor;
	for(i = 0; i < 12; i++)
	{
		st_Line.A = V + aac_Edge[i][0];
		st_Line.B = V + aac_Edge[i][1];
		OGL_DrawLineEx(_pst_DD, &st_Line);
	}

	ulBlendingMode = 0;
	if((_pst_Box->ul_SolidColor & 0xFF000000) != 0xFF000000)
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Alpha);
	else
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);

	OGL_RS_CullFace(OGL_M_RS(_pst_DD), (_pst_Box->ul_Flags & GDI_Request_DrawBox_CullFace ? 1 : 0));
	OGL_RS_DrawWired(OGL_M_RS(_pst_DD), 0);
	OGL_RS_DepthMask(OGL_M_RS(_pst_DD), (_pst_Box->ul_Flags & GDI_Request_DrawBox_NoZWrite ? 0 : 1));

	ul_SaveColor = _pst_DD->ul_ColorConstant;
	_pst_DD->ul_ColorConstant = _pst_Box->ul_SolidColor;
	for(i = 0; i < 6; i++)
	{
		Quad[0] = V + aac_Face[i][0];
		Quad[1] = V + aac_Face[i][1];
		Quad[2] = V + aac_Face[i][2];
		Quad[3] = V + aac_Face[i][3];

		GDI_gpst_CurDD->profilingInformation.numBatches++;
		glBegin(GL_QUADS);
		OGL_SetColorFor2x((GLubyte *) &_pst_Box->ul_SolidColor);
		glVertex3fv((float *) Quad[0]);
		glVertex3fv((float *) Quad[1]);
		glVertex3fv((float *) Quad[2]);
		glVertex3fv((float *) Quad[3]);
		glEnd();
	}
	_pst_DD->ul_ColorConstant = ul_SaveColor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_DrawSphere(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawSphere *_pst_Sphere)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define SPHERE_SEG	12
	int			l, dl, i, j, k, NbV, NbT, NbSlice;
	float		R, z, d, angle, dangle;
	GEO_Vertex	*V, *VAlloc;
	ULONG		ulBlendingMode;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	NbSlice = SPHERE_SEG >> 1;
	NbV = 2 + (SPHERE_SEG) * (NbSlice - 1);
	NbT = 2 * (SPHERE_SEG) * (NbSlice - 1);

	V = VAlloc = (MATH_tdst_Vector*)L_malloc(NbV * sizeof(GEO_Vertex));

	/*$1- vertex ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dangle = Cf_2Pi / SPHERE_SEG;
	R = _pst_Sphere->f_Radius;

	MATH_InitVector(V, 0, 0, R);
	MATH_AddEqualVector(V, _pst_Sphere->pst_Center);
	V++;

	for(k = 0; k < NbSlice - 1; k++)
	{
		angle = Cf_PiBy2 - ((k + 1) * Cf_Pi / NbSlice);
		z = R * fSin(angle);
		d = R * fCos(angle);

		angle = 0;
		for(i = 0; i < SPHERE_SEG; i++, angle -= dangle)
		{
			MATH_InitVector(V, d * fSin(angle), d * fCos(angle), z);
			MATH_AddEqualVector(V, _pst_Sphere->pst_Center);
			V++;
		}
	}

	MATH_InitVector(V, 0, 0, -R);
	MATH_AddEqualVector(V, _pst_Sphere->pst_Center);
	V++;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    Triangles
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	ulBlendingMode = 0;
	if((_pst_Sphere->ul_SolidColor & 0xFF000000) != 0xFF000000)
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Alpha);
	else
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);
	OGL_RS_CullFace(OGL_M_RS(_pst_DD), 1);
	OGL_RS_DrawWired(OGL_M_RS(_pst_DD), 0);

	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin(GL_TRIANGLES);
	OGL_SetColorFor2x((GLubyte *) &_pst_Sphere->ul_SolidColor);

	V = VAlloc;

	/* top triangles */
	for(i = 0; i < SPHERE_SEG; i++)
	{
		glVertex3fv((float *) (V + 0));
		glVertex3fv((float *) (V + (i + 1)));
		glVertex3fv((float *) (V + (1 + ((1 + i) % (SPHERE_SEG)))));
	}

	/* slice triangles */
	l = 1;
	dl = SPHERE_SEG;
	for(k = 0; k < NbSlice - 2; k++)
	{
		for(i = 0; i < dl; i++)
		{
			j = ((1 + i) % SPHERE_SEG);

			glVertex3fv((float *) (V + (i + l)));
			glVertex3fv((float *) (V + (i + l + dl)));
			glVertex3fv((float *) (V + (j + l + dl)));

			glVertex3fv((float *) (V + (i + l)));
			glVertex3fv((float *) (V + (j + l + dl)));
			glVertex3fv((float *) (V + (j + l)));
		}

		l += dl;
	}

	/* bottom triangles */
	for(i = 0; i < SPHERE_SEG; i++)
	{
		glVertex3fv((float *) (V + (NbV - 1)));
		glVertex3fv((float *) (V + (((i + 1) % SPHERE_SEG) + l)));
		glVertex3fv((float *) (V + (i + l)));
	}

	glEnd();

	L_free(VAlloc);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_DrawCone(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawCone *_pst_Cone)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define CONE_SEG	12
	int					i;
	float				R, d, angle, dangle;
	ULONG				ulBlendingMode;
	MATH_tdst_Vector	V[CONE_SEG], X, Y, Z;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* finding axe */
	MATH_CopyVector(&X, _pst_Cone->pst_Axe);
	if(MATH_b_NulVector(&X) || (_pst_Cone->f_Alpha == 0)) return;

	if(fabs(X.x) < fabs(X.y))
	{
		if(fAbs(X.x) < fAbs(X.z))
			MATH_InitVector(&Y, 1, 0, 0);
		else
			MATH_InitVector(&Y, 0, 0, 1);
	}
	else
	{
		if(fAbs(X.y) < fAbs(X.z))
			MATH_InitVector(&Y, 0, 1, 0);
		else
			MATH_InitVector(&Y, 0, 0, 1);
	}

	MATH_CrossProduct(&Z, &X, &Y);
	MATH_NormalizeVector(&Z, &Z);
	MATH_CrossProduct(&Y, &Z, &X);
	MATH_NormalizeVector(&Y, &Y);

	d = MATH_f_NormVector(&X);
	R = fCos(_pst_Cone->f_Alpha);
	if(R < 0)
	{
		MATH_NegEqualVector(&X);
		R = -R;
	}

	R = d * fSin(_pst_Cone->f_Alpha) / ((R > 0.02f) ? R : 0.02f);
	MATH_MulEqualVector(&Y, R);
	MATH_MulEqualVector(&Z, R);

	angle = 0;
	dangle = Cf_2Pi / SPHERE_SEG;
	for(i = 0; i < CONE_SEG; i++)
	{
		MATH_MulVector(&V[i], &Y, fCos(angle));
		MATH_AddScaleVector(&V[i], &V[i], &Z, fSin(angle));
		MATH_AddEqualVector(&V[i], &X);
		MATH_AddEqualVector(&V[i], _pst_Cone->pst_Pos);
		angle += dangle;
	}

	/* draw triangle */
	ulBlendingMode = 0;
	if((_pst_Cone->ul_SolidColor & 0xFF000000) != 0xFF000000)
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Alpha);
	else
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);
	OGL_RS_CullFace(OGL_M_RS(_pst_DD), 0);
	OGL_RS_DrawWired(OGL_M_RS(_pst_DD), 0);

	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin(GL_TRIANGLE_FAN);
	OGL_SetColorFor2x((GLubyte *) &_pst_Cone->ul_SolidColor);
	glVertex3fv((float *) _pst_Cone->pst_Pos);
	for(i = 0; i < CONE_SEG; i++) glVertex3fv((float *) &V[i]);
	glVertex3fv((float *) V);
	glEnd();

	/* draw line */
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);

	OGL_RS_LineWidth(OGL_M_RS(_pst_DD), _pst_Cone->f_LineWidth);
	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin(GL_LINES);
	OGL_SetColorFor2x((GLubyte *) &_pst_Cone->ul_EdgeColor);

	for(i = 0; i < CONE_SEG; i++)
	{
		glVertex3fv((float *) _pst_Cone->pst_Pos);
		glVertex3fv((float *) &V[i]);

		glVertex3fv((float *) &V[i]);
		glVertex3fv((float *) &V[(i + 1) % CONE_SEG]);
	}

	glEnd();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_DrawCylinder(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawCylinder *_pst_Cyl)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define CYL_SEG 12
#define CYL_CAP 4
	int					cap, i, j, k, index;
	float				d, alpha, cosalpha, sinalpha, angle;
	ULONG				ulBlendingMode;
	MATH_tdst_Vector	V[(CYL_SEG * CYL_CAP + 1) * 2], X, Y, Z, Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Cyl->ul_Flags & GDI_Request_DrawCylinder_Capsule)
		cap = CYL_CAP;
	else
		cap = 1;

	/* finding axe */
	MATH_CopyVector(&Z, _pst_Cyl->pst_Axe);
	if(MATH_b_NulVector(&Z)) return;

	if(fabs(Z.x) < fabs(Z.y))
	{
		if(fAbs(Z.x) < fAbs(Z.z))
			MATH_InitVector(&X, 1, 0, 0);
		else
			MATH_InitVector(&X, 0, 0, 1);
	}
	else
	{
		if(fAbs(Z.y) < fAbs(Z.z))
			MATH_InitVector(&X, 0, 1, 0);
		else
			MATH_InitVector(&X, 0, 0, 1);
	}

	MATH_CrossProduct(&Y, &Z, &X);
	MATH_NormalizeVector(&Y, &Y);
	d = MATH_f_NormVector(&Z);
	MATH_NormalizeVector(&Z, &Z);
	MATH_CrossProduct(&X, &Y, &Z);

	/* bottom */
	MATH_InitVector(&V[0], 0, 0, cap == 1 ? 0 : -_pst_Cyl->f_Radius);
	for(i = 0, index = 1; i < cap; i++)
	{
		alpha = (cap - i - 1) * (Cf_PiBy2 / cap);
		sinalpha = -fSin(alpha) * _pst_Cyl->f_Radius;
		cosalpha = fCos(alpha) * _pst_Cyl->f_Radius;
		for(j = 0; j < CYL_SEG; j++, index++)
		{
			angle = j * (Cf_2Pi / SPHERE_SEG);
			MATH_InitVector(&V[index], cosalpha * fCos(angle), cosalpha * fSin(angle), sinalpha);
		}
	}

	k = index - CYL_SEG;
	for(i = 0; i < cap; i++)
	{
		for(j = 0; j < CYL_SEG; j++, index++, k++)
		{
			MATH_CopyVector(&V[index], &V[k]);
			V[index].z = d - V[index].z;
		}

		k -= CYL_SEG * 2;
	}

	MATH_InitVector(&V[index], 0, 0, (cap == 1 ? 0 : _pst_Cyl->f_Radius) + d);

	for(i = 0; i < 2 * ((CYL_SEG * cap) + 1); i++)
	{
		MATH_ScaleVector(&Temp, &X, V[i].x);
		MATH_AddScaleVector(&Temp, &Temp, &Y, V[i].y);
		MATH_AddScaleVector(&Temp, &Temp, &Z, V[i].z);
		MATH_AddVector(&V[i], &Temp, _pst_Cyl->pst_Pos);
	}

	/* draw triangle */
	if(!(_pst_Cyl->ul_Flags & GDI_Request_DrawCylinder_OnlyLine))
	{
		ulBlendingMode = 0;
		if((_pst_Cyl->ul_SolidColor & 0xFF000000) != 0xFF000000)
			MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Alpha);
		else
			MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
		OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);
		OGL_RS_CullFace(OGL_M_RS(_pst_DD), (_pst_Cyl->ul_Flags & GDI_Request_DrawCylinder_CullFace ? 1 : 0));
		OGL_RS_DrawWired(OGL_M_RS(_pst_DD), 0);
		OGL_RS_DepthMask(OGL_M_RS(_pst_DD), (_pst_Cyl->ul_Flags & GDI_Request_DrawCylinder_NoZWrite ? 0 : 1));

		GDI_gpst_CurDD->profilingInformation.numBatches++;
		glBegin(GL_TRIANGLE_FAN);
		OGL_SetColorFor2x((GLubyte *) &_pst_Cyl->ul_SolidColor);
		glVertex3fv((float *) &V[0]);
		glVertex3fv((float *) &V[1]);
		for(i = CYL_SEG; i > 0; i--) glVertex3fv((float *) &V[i]);
		glEnd();

		GDI_gpst_CurDD->profilingInformation.numBatches++;
		glBegin(GL_TRIANGLES);
		for(i = 0; i < (2 * cap) - 1; i++)
		{
			j = 1 + (i * CYL_SEG);
			k = j + CYL_SEG;
			for(index = 1; index < CYL_SEG; index++, k++, j++)
			{
				glVertex3fv((float *) &V[j]);
				glVertex3fv((float *) &V[j + 1]);
				glVertex3fv((float *) &V[k]);

				glVertex3fv((float *) &V[j + 1]);
				glVertex3fv((float *) &V[k + 1]);
				glVertex3fv((float *) &V[k]);
			}

			glVertex3fv((float *) &V[j]);
			glVertex3fv((float *) &V[j - CYL_SEG + 1]);
			glVertex3fv((float *) &V[k]);

			glVertex3fv((float *) &V[j - CYL_SEG + 1]);
			glVertex3fv((float *) &V[k - CYL_SEG + 1]);
			glVertex3fv((float *) &V[k]);
		}

		glEnd();

		GDI_gpst_CurDD->profilingInformation.numBatches++;
		glBegin(GL_TRIANGLE_FAN);
		j = ((CYL_SEG * cap + 1) * 2) - 1;
		glVertex3fv((float *) &V[j]);
		k = j - CYL_SEG;
		for(i = 0; i < CYL_SEG; i++, k++) glVertex3fv((float *) &V[k]);
		glVertex3fv((float *) &V[j - CYL_SEG]);
		glEnd();
	}

	/* draw line */
	if(_pst_Cyl->ul_Flags & (GDI_Request_DrawCylinder_OnlyLine | GDI_Request_DrawCylinder_Line))
	{
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
		OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);
		OGL_RS_LineWidth(OGL_M_RS(_pst_DD), _pst_Cyl->f_LineWidth);
		GDI_gpst_CurDD->profilingInformation.numBatches++;
		glBegin(GL_LINES);
		OGL_SetColorFor2x((GLubyte *) &_pst_Cyl->ul_EdgeColor);

		j = ((CYL_SEG * cap + 1) * 2) - 1;
		k = j - CYL_SEG;
		for(i = 0; i < CYL_SEG; i++)
		{
			glVertex3fv((float *) &V[0]);
			glVertex3fv((float *) &V[i + 1]);

			glVertex3fv((float *) &V[j]);
			glVertex3fv((float *) &V[k + i]);
		}

		for(i = 0; i < 2 * cap; i++)
		{
			k = 1 + (i * CYL_SEG);
			for(j = 0; j < CYL_SEG - 1; j++)
			{
				glVertex3fv((float *) &V[k + j]);
				glVertex3fv((float *) &V[k + j + 1]);
			}

			glVertex3fv((float *) &V[k]);
			glVertex3fv((float *) &V[k + CYL_SEG - 1]);
		}

		for(i = 0; i < (2 * cap) - 1; i++)
		{
			k = 1 + (i * CYL_SEG);
			for(j = 0; j < CYL_SEG; j++)
			{
				glVertex3fv((float *) &V[k + j]);
				glVertex3fv((float *) &V[k + j + CYL_SEG]);
			}
		}

		glEnd();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_ReloadTexture(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_ReloadTextureParams *_pst_RTP)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OGL_tdst_SpecificData	*pst_SD;
	TEX_tdst_Data			*pst_Tex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = OGL_M_SD(_pst_DD);
	pst_Tex = &TEX_gst_GlobalList.dst_Texture[_pst_RTP->w_Texture];
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pst_SD->dul_Texture[_pst_RTP->w_Texture]);
	glTexSubImage2D
	(
		GL_TEXTURE_2D,
		0,
		0,
		0,
		pst_Tex->w_Width,
		pst_Tex->w_Height,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		_pst_RTP->pc_Data
	);
}

/*
 =======================================================================================================================
    Function used for reloading only RAW field of an indexed texture (PSX2_TARGET)
 =======================================================================================================================
 */
void OGL_ReloadTexture2(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_ReloadTextureParams *_pst_RTP)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OGL_tdst_SpecificData	*pst_SD;
	TEX_tdst_Data			*pst_Tex;
	SHORT					wRealIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = OGL_M_SD(_pst_DD);
	pst_Tex = &TEX_gst_GlobalList.dst_Texture[_pst_RTP->w_Texture];
	wRealIndex = _pst_RTP->w_Texture;
	if(pst_Tex->uw_Flags & TEX_uw_RawPal)
	{
		pst_Tex = &TEX_gst_GlobalList.dst_Texture[pst_Tex->w_Height];
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pst_SD->dul_Texture[wRealIndex]);
	glTexSubImage2D
	(
		GL_TEXTURE_2D,
		0,
		0,
		0,
		pst_Tex->w_Width,
		pst_Tex->w_Height,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		_pst_RTP->pc_Data
	);	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_ReloadPalette(GDI_tdst_DisplayData *_pst_DD, short _w_Palette)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OGL_tdst_SpecificData		*pst_SD;
	TEX_tdst_Data				*pst_Tex;
	OGL_tdst_UpdatablePalette	*pst_Data;
	int							i;
	UCHAR						uc_ColorMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = OGL_M_SD(_pst_DD);

	glGetBooleanv(GL_MAP_COLOR, &uc_ColorMap);
	if(!uc_ColorMap) glPixelTransferi(GL_MAP_COLOR, TRUE);

	OGL_Texture_SetPalette(_w_Palette);

	/* loop through updatable palette data */
	pst_Data = pst_SD->dst_UdatePalette;
	for(i = 0; i < pst_SD->l_NumberOfUpdatablePalettes; i++, pst_Data++)
	{
		if(pst_Data->w_Palette == _w_Palette)
		{
			pst_Tex = &TEX_gst_GlobalList.dst_Texture[pst_Data->w_Texture];
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, pst_SD->dul_Texture[pst_Data->w_Texture]);
			glTexSubImage2D
			(
				GL_TEXTURE_2D,
				0,
				0,
				0,
				pst_Data->w_Width,
				pst_Data->w_Height,
				GL_COLOR_INDEX,
				GL_UNSIGNED_BYTE,
				pst_Data->p_Raw
			);
		}
	}

	if(!uc_ColorMap) glPixelTransferi(GL_MAP_COLOR, FALSE);
}

extern float	GFXF_f_depth;
extern ULONG	GFXF_ul_Color;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_ReadPixel(GDI_tdst_DisplayData *_pst_DD, MATH_tdst_Vector *_pst_Pos)
{
	/*~~~~~*/
	int x, y;
	/*~~~~~*/

	x = (int) _pst_Pos->x;
	y = (int) _pst_Pos->y;

	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &GFXF_ul_Color);
	glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &GFXF_f_depth);

	if(GFXF_f_depth == 1.0f)
		GFXF_f_depth = Cf_Infinit;
	else
		GFXF_f_depth = 1.0f / (20.0f * (1.0f - GFXF_f_depth));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_ReadScreen(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_RWPixels *d)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_SaveBuffer, i_DrawBuffer;
	ULONG					ulBlendingMode;
	OGL_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ulBlendingMode = 0;

	pst_SD = OGL_M_SD(_pst_DD);

	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);

	if(d->c_Write == 1)
	{
		/* glRasterPos2f( (float) d->x / 100.0f, (float) d->y / 100.0f ); */
		if(d->c_Buffer == 2) /* depth buffer */
			return;

		glGetIntegerv(GL_DRAW_BUFFER, &i_SaveBuffer);
		glDrawBuffer(d->c_Buffer ? GL_FRONT_LEFT : GL_BACK);
		glDrawPixels(d->w, d->h, GL_BGR_EXT, GL_UNSIGNED_BYTE, d->p_Bitmap);
		glDrawBuffer(i_SaveBuffer);
	}
	else if(d->c_Write == 0)
	{
		if(d->c_Buffer == 2)
		{
			glReadPixels(d->x, d->y, d->w, d->h, GL_DEPTH_COMPONENT, GL_FLOAT, d->p_Bitmap);
			return;
		}
		else
		{
			glGetIntegerv(GL_READ_BUFFER, &i_SaveBuffer);
			glReadBuffer(d->c_Buffer ? GL_FRONT_LEFT : GL_BACK_LEFT);
			glReadPixels(d->x, d->y, d->w, d->h, GL_BGR_EXT, GL_UNSIGNED_BYTE, d->p_Bitmap);
			glReadBuffer(i_SaveBuffer);
		}
	}
	else
	{
		glPixelTransferf(GL_RED_BIAS, d->f_Bias[0]);
		glPixelTransferf(GL_GREEN_BIAS, d->f_Bias[1]);
		glPixelTransferf(GL_BLUE_BIAS, d->f_Bias[2]);

		OGL_RS_DepthTest(&pst_SD->st_RS, 0);

		glGetIntegerv(GL_READ_BUFFER, &i_SaveBuffer);
		glGetIntegerv(GL_DRAW_BUFFER, &i_DrawBuffer);
		glReadBuffer(d->c_Buffer ? GL_FRONT : GL_BACK);
		glDrawBuffer((d->c_Buffer >> 4) ? GL_FRONT : GL_BACK);
		glCopyPixels(d->x, d->y, d->w, d->h, GL_COLOR);
		glReadBuffer(i_SaveBuffer);
		glDrawBuffer(i_DrawBuffer);

		OGL_RS_DepthTest(&pst_SD->st_RS, 1);

		glPixelTransferf(GL_RED_BIAS, 0);
		glPixelTransferf(GL_GREEN_BIAS, 0);
		glPixelTransferf(GL_BLUE_BIAS, 0);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OGL_Display169BlackBand(GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ulBlendingMode;
	MATH_tdst_Vector	V;
	float				x0, x1, y0, y1, y2, y3;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	x0 = -1;
	x1 = 1;
	y0 = -1;
	y1 = -.75;
	y2 = 0.75;
	y3 = 1;

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	OGL_SetTextureBlending((ULONG) - 1, ulBlendingMode);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float *) &MATH_gst_IdentityMatrix);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float *) &MATH_gst_IdentityMatrix);
	glOrtho(-1, 1, 1, -1, -10000, 10000);

	OGL_RS_DepthTest(&OGL_M_SD(_pst_DD)->st_RS, 0);
	OGL_RS_CullFace(&OGL_M_SD(_pst_DD)->st_RS, 0);
	OGL_RS_DrawWired(OGL_M_RS(_pst_DD), 0);

	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin(GL_QUADS);

	glColor3ub(0, 0, 0);

	MATH_InitVector(&V, x0, y0, 1);
	glVertex3fv((float *) &V);
	MATH_InitVector(&V, x1, y0, 1);
	glVertex3fv((float *) &V);
	MATH_InitVector(&V, x1, y1, 1);
	glVertex3fv((float *) &V);
	MATH_InitVector(&V, x0, y1, 1);
	glVertex3fv((float *) &V);

	MATH_InitVector(&V, x0, y2, 1);
	glVertex3fv((float *) &V);
	MATH_InitVector(&V, x1, y2, 1);
	glVertex3fv((float *) &V);
	MATH_InitVector(&V, x1, y3, 1);
	glVertex3fv((float *) &V);
	MATH_InitVector(&V, x0, y3, 1);
	glVertex3fv((float *) &V);

	glEnd();
}

/* pour test Bump */
TEX_tdst_Data		*OGLBump_TexData;
TEX_tdst_File_Desc	*OGLBump_Tex;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG OGL_l_Bump(GDI_tdst_DisplayData *_pst_DD, int _i_Tex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OGL_tdst_SpecificData	*pst_SD;
	OGL_tdst_BumpTexture	*pst_Bump;
	int						count;
	UCHAR					*puc_Buf;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = OGL_M_SD(_pst_DD);

	if(pst_SD->l_NbBumpTex == pst_SD->l_MaxBumpTex)
	{
		pst_SD->l_MaxBumpTex += OGL_C_BumpTexGran;
		pst_SD->dst_BumpTex = (OGL_tdst_BumpTexture *) MEM_p_Realloc
			(
				pst_SD->dst_BumpTex,
				pst_SD->l_MaxBumpTex * sizeof(OGL_tdst_BumpTexture)
			);
	}

	pst_Bump = pst_SD->dst_BumpTex + pst_SD->l_NbBumpTex;

	count = OGLBump_Tex->uw_Height * OGLBump_Tex->uw_Width * (OGLBump_Tex->uc_FinalBPP >> 3);
	puc_Buf = (UCHAR*)OGLBump_Tex->p_Bitmap;
	while(count--)
	{
		*puc_Buf++ >>= 1;
	}

	glGenTextures(1, (GLuint*)&pst_Bump->ul_Bump);
	glBindTexture(GL_TEXTURE_2D, pst_Bump->ul_Bump);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexImage2D
	(
		GL_TEXTURE_2D,
		0,
		4,
		OGLBump_Tex->uw_Width,
		OGLBump_Tex->uw_Height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		OGLBump_Tex->p_Bitmap
	);

	count = OGLBump_Tex->uw_Height * OGLBump_Tex->uw_Width * (OGLBump_Tex->uc_FinalBPP >> 3);
	puc_Buf = (UCHAR*)OGLBump_Tex->p_Bitmap;
	while(count--)
	{
		if(count & 3) *puc_Buf = (0xFF - (*puc_Buf << 1)) >> 1;
		puc_Buf++;
	}

	glGenTextures(1, (GLuint*)&pst_Bump->ul_InvBump);
	glBindTexture(GL_TEXTURE_2D, pst_Bump->ul_InvBump);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexImage2D
	(
		GL_TEXTURE_2D,
		0,
		4,
		OGLBump_Tex->uw_Width,
		OGLBump_Tex->uw_Height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		OGLBump_Tex->p_Bitmap
	);

	pst_SD->dul_Texture[_i_Tex] = OGL_C_BumpTexFlag + pst_SD->l_NbBumpTex++;

	return 1;
}

/*
 =======================================================================================================================
    Aim:    Treat miscelleanous request ( see request constant in GDInterface.h file )
 =======================================================================================================================
 */
static int polygon_fill;
static int polygon_line;
static int polygon_point;
static float polygon_factor;
static float polygon_unit;


LONG OGL_l_Request(ULONG _ul_Request, ULONG _ul_Data)
{
	GDI_gpst_CurDD->profilingInformation.numRequests++;

	/*$off*/
	switch(_ul_Request)
	{
	case GDI_Cul_Request_SetFogParams:				OGL_SetFogParams(GDI_gpst_CurDD, (SOFT_tdst_FogParams *) _ul_Data); break;
	case GDI_Cul_Request_DrawSoftEllipse:			OGL_DrawEllipse(GDI_gpst_CurDD, (SOFT_tdst_Ellipse *) _ul_Data); break;
	case GDI_Cul_Request_DrawSoftArrow:				OGL_DrawArrow(GDI_gpst_CurDD, (SOFT_tdst_Arrow *) _ul_Data); break;
	case GDI_Cul_Request_DrawSoftSquare:			OGL_DrawSquare(GDI_gpst_CurDD, (SOFT_tdst_Square *) _ul_Data); break;
	case GDI_Cul_Request_DepthTest:					OGL_RS_DepthTest( OGL_M_RS( GDI_gpst_CurDD ), _ul_Data ); break;
	case GDI_Cul_Request_DepthFunc:					OGL_RS_DepthFunc( OGL_M_RS( GDI_gpst_CurDD ), _ul_Data ? GL_LEQUAL : GL_ALWAYS ); break;
	case GDI_Cul_Request_DrawPoint:					OGL_DrawPoint( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
	case GDI_Cul_Request_DrawLine:					OGL_DrawLine( GDI_gpst_CurDD, (MATH_tdst_Vector **) _ul_Data ); break;
	case GDI_Cul_Request_DrawQuad:					OGL_DrawQuad( GDI_gpst_CurDD, (MATH_tdst_Vector **) _ul_Data ); break;
	case GDI_Cul_Request_ReloadTexture:				OGL_ReloadTexture( GDI_gpst_CurDD, (GDI_tdst_Request_ReloadTextureParams *) _ul_Data); break;
	case GDI_Cul_Request_ReloadTexture2:			OGL_ReloadTexture2( GDI_gpst_CurDD, (GDI_tdst_Request_ReloadTextureParams *) _ul_Data); break;
	case GDI_Cul_Request_SizeOfPoints:				OGL_PointSize( GDI_gpst_CurDD, *(float *) &_ul_Data ); break;
	case GDI_Cul_Request_SizeOfLine:				OGL_LineSize(GDI_gpst_CurDD, *(float *) &_ul_Data ); break;
	case GDI_Cul_Request_DrawLineEx:				OGL_DrawLineEx(GDI_gpst_CurDD, (GDI_tdst_Request_DrawLineEx *) _ul_Data); break;
	case GDI_Cul_Request_DrawPointEx:				OGL_DrawPointEx(GDI_gpst_CurDD, (GDI_tdst_Request_DrawPointEx *)_ul_Data ); break;
	case GDI_Cul_Request_DrawPointSize:				OGL_DrawPoint_Size(GDI_gpst_CurDD, (void  **)_ul_Data);break;
	case GDI_Cul_Request_BeforeDrawSprite:			OGL_BeforeDrawSprite(GDI_gpst_CurDD);break;
	case GDI_Cul_Request_DrawSprite:				OGL_DrawSprite(GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data );break;
	case GDI_Cul_Request_DrawSpriteUV:				OGL_DrawSprite(GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data );break;
	case GDI_Cul_Request_AfterDrawSprite:			OGL_AfterDrawSprite(GDI_gpst_CurDD);break;
	case GDI_Cul_Request_DrawPointMin:				OGL_DrawPointMin( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
	case GDI_Cul_Request_ReloadPalette:				OGL_ReloadPalette( GDI_gpst_CurDD, (short) _ul_Data ); break;
	case GDI_Cul_Request_Draw2DTriangle:			OGL_Draw2DTriangle(GDI_gpst_CurDD, (SOFT_tdst_Vertex *) _ul_Data); break;
	case GDI_Cul_Request_ReadScreen:				OGL_ReadScreen( GDI_gpst_CurDD, (GDI_tdst_Request_RWPixels *) _ul_Data ); break;
	case GDI_Cul_Request_EnableFog:					OGL_RS_Fogged( OGL_M_RS(GDI_gpst_CurDD), _ul_Data ); break;
	case GDI_Cul_Request_NumberOfTextures:			return ( OGL_M_SD( GDI_gpst_CurDD )->l_NumberOfTextures != (LONG) _ul_Data );
	case GDI_Cul_Request_ReadPixel:					OGL_ReadPixel( GDI_gpst_CurDD, (MATH_tdst_Vector *) _ul_Data ); break;
	case GDI_Cul_Request_Enable:					glEnable( _ul_Data ); break;
	case GDI_Cul_Request_Disable:					glDisable( _ul_Data ); break;
	case GDI_Cul_Request_PolygonOffset:				glPolygonOffset( *(float *) _ul_Data, *(((float *) _ul_Data) + 1) ); 			break;
	case GDI_Cul_Request_LoadInterfaceTex:			OGL_Texture_LoadInterfaceTex( GDI_gpst_CurDD ); break;
	case GDI_Cul_Request_UnloadInterfaceTex:		OGL_Texture_UnloadInterfaceTex( GDI_gpst_CurDD ); break;
	case GDI_Cul_Request_GetInterfaceTexBuffer:		return (LONG) OGL_p_Texture_GetInterfaceTexBuffer( GDI_gpst_CurDD, _ul_Data ); break;
	case GDI_Cul_Request_TextureUnloadCompare:		OGL_Texture_UnLoadCompare(); break;
	case GDI_Cul_Request_Display169BlackBand:		OGL_Display169BlackBand( GDI_gpst_CurDD ); break;
	case GDI_Cul_Request_DrawBox:					OGL_DrawBox( GDI_gpst_CurDD, (GDI_tdst_Request_DrawBox *) _ul_Data ); break;
	case GDI_Cul_Request_DrawSphere:				OGL_DrawSphere( GDI_gpst_CurDD, (GDI_tdst_Request_DrawSphere *) _ul_Data ); break;
	case GDI_Cul_Request_DrawCone:					OGL_DrawCone( GDI_gpst_CurDD, (GDI_tdst_Request_DrawCone *) _ul_Data ); break;
	case GDI_Cul_Request_DrawCylinder:				OGL_DrawCylinder( GDI_gpst_CurDD, (GDI_tdst_Request_DrawCylinder *) _ul_Data ); break;
	case GDI_Cul_Request_DrawQuadEx:				OGL_DrawQuadEx( GDI_gpst_CurDD, (GDI_tdst_Request_DrawQuad *) _ul_Data ); break;
	case GDI_Cul_Request_PolygonOffsetSave:
				polygon_fill = glIsEnabled( GL_POLYGON_OFFSET_FILL );
				polygon_line = glIsEnabled( GL_POLYGON_OFFSET_LINE );
				polygon_point = glIsEnabled( GL_POLYGON_OFFSET_POINT );
				glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &polygon_factor);
				glGetFloatv(GL_POLYGON_OFFSET_UNITS, &polygon_unit);
			   break;
	case GDI_Cul_Request_PolygonOffsetRestore:
				polygon_fill ? glEnable( GL_POLYGON_OFFSET_FILL ) : glDisable( GL_POLYGON_OFFSET_FILL );
				polygon_line ? glEnable( GL_POLYGON_OFFSET_LINE ) : glDisable( GL_POLYGON_OFFSET_LINE );
				polygon_point ? glEnable( GL_POLYGON_OFFSET_POINT ) : glDisable( GL_POLYGON_OFFSET_POINT );
				glPolygonOffset( polygon_factor, polygon_unit );
				break;

	/* pour test bump */
	case 1000: OGLBump_TexData = (TEX_tdst_Data *) _ul_Data; break;
	case 1001: OGLBump_Tex = (TEX_tdst_File_Desc *) _ul_Data; break;
	case 1002: return OGL_l_Bump( GDI_gpst_CurDD, _ul_Data ); break;

	case GDI_Cul_Request_PushZBuffer:
		OGL_PushZBuffer(GDI_gpst_CurDD, *(float*)&_ul_Data);
		break;

	}
	/*$on*/
	return 0;
}

#if defined PSX2_TARGET && defined __cplusplus
}
#endif
