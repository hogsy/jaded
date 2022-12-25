/*$T Gx8debugfct.c GC! 1.081 01/16/02 16:51:28 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Gx8debugfct.h"
#include "Gx8color.h"
#include <D3D8.h>

#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Gx8init.h"
#include "Gx8renderstate.h"

/* external functions */
extern ULONG	LIGHT_ul_Interpol2Colors(ULONG, ULONG, float);

/*
 =======================================================================================================================
    Macros
 =======================================================================================================================
 */
#define Gx8_M_GetEllipsePoint(alpha) \
	{ \
		MATH_ScaleVector(&A, &_pst_Ellipse->st_A, fOptCos(alpha)); \
		MATH_ScaleVector(&B, &_pst_Ellipse->st_B, fOptSin(alpha)); \
		MATH_AddVector(&N, &A, &B); \
		MATH_AddEqualVector(&N, &C); \
	}

#define Gx8_M_Send3Vertex(a, b, c) \
	{ \
		ULONG	_color_; \
		_color_ = Gx8_M_ConvertColor(CurrentColor); \
 \
		*(MATH_tdst_Vector *) pVertexBuffer = *(a); \
		(pVertexBuffer++)->Color = _color_; \
		*(MATH_tdst_Vector *) pVertexBuffer = *(b); \
		(pVertexBuffer++)->Color = _color_; \
		*(MATH_tdst_Vector *) pVertexBuffer = *(c); \
		(pVertexBuffer++)->Color = _color_; \
	}

#define Gx8_M_SendColorAnd3Vertex(a, b, c, color) \
	{ \
		ULONG	_color_; \
		_color_ = Gx8_M_ConvertColor(CurrentColor); \
 \
		*(MATH_tdst_Vector *) pVertexBuffer = *(a); \
		CurrentColor = (pVertexBuffer++)->Color = _color_; \
		*(MATH_tdst_Vector *) pVertexBuffer = *(b); \
		(pVertexBuffer++)->Color = _color_; \
		*(MATH_tdst_Vector *) pVertexBuffer = *(c); \
		(pVertexBuffer++)->Color = _color_; \
	}

#define Gx8_M_DrawLightedTriangle(pt1, pt2, pt3) \
	{ \
		MATH_SubVector(&a, (pt2), (pt1)); \
		MATH_SubVector(&b, (pt3), (pt1)); \
		MATH_CrossProduct(&n, &a, &b); \
		MATH_NormalizeVector(&n, &n); \
		if((t = MATH_f_DotProduct(&light, &n)) <= 0) \
		{ \
			CurrentColor = c = LIGHT_ul_Interpol2Colors(0, ul_Color, -t); \
			c = Gx8_M_ConvertColor(c); \
			*(MATH_tdst_Vector *) pVertexBuffer = *(pt1); \
			(pVertexBuffer++)->Color = c; \
			*(MATH_tdst_Vector *) pVertexBuffer = *(pt2); \
			(pVertexBuffer++)->Color = c; \
			*(MATH_tdst_Vector *) pVertexBuffer = *(pt3); \
			(pVertexBuffer++)->Color = c; \
		} \
	}

/*
 =======================================================================================================================
    Aim:    Draw an ellipse
 =======================================================================================================================
 */
void Gx8_DrawEllipse(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Ellipse *_pst_Ellipse)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float					f_Alpha;
	ULONG					ulBlendingMode;
	MATH_tdst_Vector		A, B, C, M, N;
	ULONG					c, c0, c1;
	Gx8VertexFormat			*pVertexBuffer, *pVertexBufferSave;
	Gx8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = GX8_M_SD(_pst_DD);

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode, 0);

	Gx8_RS_DepthFunc(GX8_M_SD(_pst_DD), D3DCMP_ALWAYS);
	MATH_CopyVector(&C, (MATH_tdst_Vector *) &_pst_Ellipse->st_Center);
	c1 = _pst_Ellipse->st_Center.color;
	c0 = ((c1 & 0xFEFEFEFE) >> 1);

	/* Draw ellipse */
	Gx8_RS_LineWidth(GX8_M_RS(_pst_DD), 1.0f);

	Gx8_vBeginRenderUsingTheBigVertexBuffer(30, (unsigned char **) &pVertexBuffer);

	pVertexBufferSave = pVertexBuffer;
	Gx8_M_GetEllipsePoint(Cf_2Pi - .1f);
	MATH_CopyVector(&M, &N);

	for(f_Alpha = 0; f_Alpha <= Cf_2Pi; f_Alpha += 0.1f)
	{
		Gx8_M_GetEllipsePoint(f_Alpha);

		if((_pst_Ellipse->l_Flag & 1) || ((M.z <= C.z) && (N.z <= C.z)))
		{
			c = LIGHT_ul_Interpol2Colors(c0, c1, ((C.z - M.z) + .1f) * 5);
			c = Gx8_M_ConvertColor(c);
			pVertexBuffer->Color = c;
			*(MATH_tdst_Vector *) (pVertexBuffer++) = M;
			c = LIGHT_ul_Interpol2Colors(c0, c1, ((C.z - N.z) + .1f) * 5);
			c = Gx8_M_ConvertColor(c);
			pVertexBuffer->Color = c;
			*(MATH_tdst_Vector *) (pVertexBuffer++) = N;
		}

		MATH_CopyVector(&M, &N);
	}

    Gx8_vEndRenderUsingTheBigVertexBuffer((pVertexBuffer - pVertexBufferSave) / 2, D3DPT_LINELIST);

	if(!(_pst_Ellipse->l_Flag & 4))
	{
		/* Draw alpha start point */
		Gx8_RS_PointSize(GX8_M_RS(_pst_DD), 10.0f);
        Gx8_vBeginRenderUsingTheBigVertexBuffer(30, (unsigned char **) &pVertexBuffer);
		pVertexBufferSave = pVertexBuffer;

		pVertexBuffer->Color = Gx8_M_ConvertColor(0x808080FF);
		if(_pst_Ellipse->l_Flag & 1)
		{
			Gx8_M_GetEllipsePoint(_pst_Ellipse->f_AlphaStart);
			*(MATH_tdst_Vector *) (pVertexBuffer++) = N;
		}

		pVertexBuffer->Color = 0xFFFFFFFF;
		if(_pst_Ellipse->l_Flag & 2)
		{
			Gx8_M_GetEllipsePoint(_pst_Ellipse->f_AlphaEnd);
			*(MATH_tdst_Vector *) (pVertexBuffer++) = N;
		}

#pragma message("~~~~~~~~~~~~~~~~~~~~~~~ Unlock but don't draw ?!? ~~~~~~~~~~~~~~~~~~~~~~~")
		IDirect3DVertexBuffer8_Unlock(pst_SD->m_stBigVBData.m_VertexBuffer);
	}

	/* Reset render parameters */
	Gx8_RS_DrawWired(GX8_M_SD(_pst_DD), 0);
	Gx8_RS_LineWidth(GX8_M_RS(_pst_DD), 1.0f);
	Gx8_RS_PointSize(GX8_M_RS(_pst_DD), 1.0f);
	_pst_DD->LastDrawMask |= GDI_Cul_DM_NotWired;
	_pst_DD->ul_CurrentDrawMask |= GDI_Cul_DM_NotWired;



}

/*
 =======================================================================================================================
    Aim:    Draw an arrow
 =======================================================================================================================
 */
void Gx8_DrawArrow(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Arrow *_pst_Arrow)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		a, b, b1, i, j, k, light, n;
	MATH_tdst_Vector		pt[8];
	float					f_Scale, t;
	ULONG					ulBlendingMode, ul_Color, c=0;
	Gx8VertexFormat			*pVertexBuffer, *pVertexBufferSave;
	ULONG					CurrentColor;
	Gx8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = (Gx8_tdst_SpecificData *) _pst_DD->pv_SpecificData;

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode, 0);

	Gx8_RS_DepthFunc(GX8_M_SD(_pst_DD), D3DCMP_ALWAYS);
	Gx8_RS_DrawWired(GX8_M_SD(_pst_DD), 0);
	Gx8_RS_LineWidth(GX8_M_RS(_pst_DD), 1.0f);
    Gx8_RS_CullFace(GX8_M_SD(_pst_DD), GX8_C_CULLFACE_NORMAL);

	ul_Color = _pst_Arrow->st_Start.color;

	MATH_CopyVector(&a, (MATH_tdst_Vector *) &_pst_Arrow->st_Start);
	MATH_InitVector(&light, 0, 0, 1);

	if(_pst_Arrow->l_Flag & SOFT_Cul_Arrow_UseFactor)
		MATH_ScaleVector(&b, &_pst_Arrow->st_Axis, _pst_Arrow->f_Move);
	else
		MATH_CopyVector(&b, &_pst_Arrow->st_Axis);

	MATH_AddEqualVector(&b, &a);

	if(!(_pst_Arrow->l_Flag & SOFT_Cul_Arrow_DrawNotLine))
	{
        Gx8_vBeginRenderUsingTheBigVertexBuffer(30, (unsigned char **) &pVertexBuffer);
		pVertexBufferSave = pVertexBuffer;

		if(_pst_Arrow->l_Flag & SOFT_Cul_Arrow_ForSpot)
		{
			MATH_AddVector(&b1, &b, &_pst_Arrow->st_Up);
			c = Gx8_M_ConvertColor(0xFFFF0000);
			pVertexBuffer->Color = c;
			*(MATH_tdst_Vector *) (pVertexBuffer++) = a;
			pVertexBuffer->Color = c;
			*(MATH_tdst_Vector *) (pVertexBuffer++) = b1;
			MATH_SubVector(&b1, &b, &_pst_Arrow->st_Up);
			pVertexBuffer->Color = c;
			*(MATH_tdst_Vector *) (pVertexBuffer++) = a;
			pVertexBuffer->Color = c;
			*(MATH_tdst_Vector *) (pVertexBuffer++) = b1;
		}
		else
		{
			c = Gx8_M_ConvertColor(ul_Color);
			pVertexBuffer->Color = c;
			*(MATH_tdst_Vector *) (pVertexBuffer++) = a;
			pVertexBuffer->Color = c;
			*(MATH_tdst_Vector *) (pVertexBuffer++) = b;
		}

        Gx8_vEndRenderUsingTheBigVertexBuffer((pVertexBuffer - pVertexBufferSave) / 2, D3DPT_LINELIST);
	}

	MATH_CopyVector(&i, &_pst_Arrow->st_Axis);
	if(_pst_Arrow->l_Flag & SOFT_Cul_Arrow_ForSpot)
		f_Scale = 0.1f;
	else
		f_Scale = MATH_f_NormVector(&i) / 10.0f;
	MATH_NormalizeVector(&i, &i);
	MATH_CopyVector(&k, &_pst_Arrow->st_Up);
	MATH_NormalizeVector(&k, &k);
	MATH_CrossProduct(&j, &k, &i);

	MATH_ScaleEqualVector(&i, f_Scale);
	MATH_ScaleEqualVector(&j, f_Scale);
	MATH_ScaleEqualVector(&k, f_Scale);

	MATH_SubVector(&pt[0], &b, &i);
	MATH_AddEqualVector(&pt[0], &j);
	MATH_AddEqualVector(&pt[0], &k);
	MATH_SubVector(&pt[1], &pt[0], &j);
	MATH_SubEqualVector(&pt[1], &j);
	MATH_SubVector(&pt[2], &pt[1], &k);
	MATH_SubEqualVector(&pt[2], &k);
	MATH_AddVector(&pt[3], &pt[2], &j);
	MATH_AddEqualVector(&pt[3], &j);

    CurrentColor = c;
	if(_pst_Arrow->l_Flag & SOFT_Cul_Arrow_Cube)
	{
		MATH_ScaleEqualVector(&i, 2);
		MATH_AddVector(&pt[4], &pt[0], &i);
		MATH_AddVector(&pt[5], &pt[1], &i);
		MATH_AddVector(&pt[6], &pt[2], &i);
		MATH_AddVector(&pt[7], &pt[3], &i);

        Gx8_vBeginRenderUsingTheBigVertexBuffer(30, (unsigned char **) &pVertexBuffer);
		pVertexBufferSave = pVertexBuffer;
		Gx8_M_DrawLightedTriangle(pt + 0, pt + 2, pt + 1);
		Gx8_M_Send3Vertex(pt + 0, pt + 3, pt + 2);
		Gx8_M_DrawLightedTriangle(pt + 0, pt + 1, pt + 4);
		Gx8_M_Send3Vertex(pt + 1, pt + 5, pt + 4);
		Gx8_M_DrawLightedTriangle(pt + 1, pt + 2, pt + 5);
		Gx8_M_Send3Vertex(pt + 2, pt + 6, pt + 5);
		Gx8_M_DrawLightedTriangle(pt + 2, pt + 3, pt + 6);
		Gx8_M_Send3Vertex(pt + 3, pt + 7, pt + 6);
		Gx8_M_DrawLightedTriangle(pt + 3, pt + 0, pt + 7);
		Gx8_M_Send3Vertex(pt + 0, pt + 4, pt + 7);
		Gx8_M_DrawLightedTriangle(pt + 4, pt + 5, pt + 6);
		Gx8_M_Send3Vertex(pt + 4, pt + 6, pt + 7);

        Gx8_vEndRenderUsingTheBigVertexBuffer((pVertexBuffer - pVertexBufferSave) / 3, D3DPT_TRIANGLELIST);
	}
	else
	{
		MATH_AddVector(&pt[4], &b, &i);

        Gx8_vBeginRenderUsingTheBigVertexBuffer(30, (unsigned char **) &pVertexBuffer);
		pVertexBufferSave = pVertexBuffer;

		Gx8_M_DrawLightedTriangle(pt + 0, pt + 1, pt + 4);
		Gx8_M_DrawLightedTriangle(pt + 1, pt + 2, pt + 4);
		Gx8_M_DrawLightedTriangle(pt + 2, pt + 3, pt + 4);
		Gx8_M_DrawLightedTriangle(pt + 3, pt + 0, pt + 4);
		Gx8_M_DrawLightedTriangle(pt + 0, pt + 2, pt + 1);
		Gx8_M_Send3Vertex(pt + 0, pt + 3, pt + 2);

        Gx8_vEndRenderUsingTheBigVertexBuffer((pVertexBuffer - pVertexBufferSave) / 3, D3DPT_TRIANGLELIST);
	}

	Gx8_RS_DrawWired(GX8_M_SD(_pst_DD), 0);
	Gx8_RS_LineWidth(GX8_M_RS(_pst_DD), 1.0f);
	_pst_DD->LastDrawMask |= GDI_Cul_DM_NotWired;
	_pst_DD->ul_CurrentDrawMask |= GDI_Cul_DM_NotWired;

}

/*
 =======================================================================================================================
    Aim:    Draw a square
 =======================================================================================================================
 */
void Gx8_DrawSquare(GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Square *_pst_Square)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		p, ast_t[2], ast_Sqr[4], a;
	ULONG					ul_Color, ulBlendingMode;
	float					f_Scale;
	Gx8VertexFormat			*pVertexBuffer, *pVertexBufferSave;
	ULONG					CurrentColor;
	Gx8_tdst_SpecificData	*pst_SD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SD = (Gx8_tdst_SpecificData *) _pst_DD->pv_SpecificData;

	MATH_CopyVector(&p, (MATH_tdst_Vector *) &_pst_Square->st_Pos);
	f_Scale = MATH_f_NormVector(&_pst_Square->st_A) * .15f;
	MATH_InitVector(&ast_Sqr[0], p.x - f_Scale, p.y - f_Scale, p.z);
	MATH_InitVector(&ast_Sqr[1], p.x - f_Scale, p.y + f_Scale, p.z);
	MATH_InitVector(&ast_Sqr[2], p.x + f_Scale, p.y + f_Scale, p.z);
	MATH_InitVector(&ast_Sqr[3], p.x + f_Scale, p.y - f_Scale, p.z);

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Alpha);
	Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode, 0);

	Gx8_RS_DepthFunc(GX8_M_SD(_pst_DD), D3DCMP_ALWAYS);
	Gx8_RS_DrawWired(GX8_M_SD(_pst_DD), 0);
    Gx8_RS_CullFace(GX8_M_SD(_pst_DD), GX8_C_CULLFACE_NORMAL);

    Gx8_vBeginRenderUsingTheBigVertexBuffer(30, (unsigned char **) &pVertexBuffer);
	pVertexBufferSave = pVertexBuffer;
	_pst_Square->st_Pos.color &= 0x80FFFFFF;
	ul_Color = Gx8_M_ConvertColor(_pst_Square->st_Pos.color);
	pVertexBuffer->Color = ul_Color;
	*(MATH_tdst_Vector *) (pVertexBuffer++) = *(ast_Sqr + 0);
	pVertexBuffer->Color = ul_Color;
	*(MATH_tdst_Vector *) (pVertexBuffer++) = *(ast_Sqr + 1);
	pVertexBuffer->Color = ul_Color;
	*(MATH_tdst_Vector *) (pVertexBuffer++) = *(ast_Sqr + 2);
	pVertexBuffer->Color = ul_Color;
	*(MATH_tdst_Vector *) (pVertexBuffer++) = *(ast_Sqr + 3);

    Gx8_vEndRenderUsingTheBigVertexBuffer(2, D3DPT_TRIANGLEFAN);

	ulBlendingMode = 0;
	MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Copy);
	Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode, 0);

	Gx8_RS_DepthFunc(GX8_M_SD(_pst_DD), D3DCMP_ALWAYS);
	Gx8_RS_DrawWired(GX8_M_SD(_pst_DD), 1);
	Gx8_RS_LineWidth(GX8_M_RS(_pst_DD), 1.0f);
	ul_Color = 0xFF000000;

    Gx8_vBeginRenderUsingTheBigVertexBuffer(30, (unsigned char **) &pVertexBuffer);
	pVertexBufferSave = pVertexBuffer;

	_pst_Square->st_Pos.color &= 0x80FFFFFF;
	ul_Color = Gx8_M_ConvertColor(ul_Color);
	pVertexBuffer->Color = ul_Color;
	*(MATH_tdst_Vector *) (pVertexBuffer++) = *(ast_Sqr + 0);
	pVertexBuffer->Color = ul_Color;
	*(MATH_tdst_Vector *) (pVertexBuffer++) = *(ast_Sqr + 1);
	pVertexBuffer->Color = ul_Color;
	*(MATH_tdst_Vector *) (pVertexBuffer++) = *(ast_Sqr + 2);
	pVertexBuffer->Color = ul_Color;
	*(MATH_tdst_Vector *) (pVertexBuffer++) = *(ast_Sqr + 3);

    Gx8_vEndRenderUsingTheBigVertexBuffer(2, D3DPT_TRIANGLEFAN);

	Gx8_RS_DrawWired(GX8_M_SD(_pst_DD), 0);

	if(_pst_Square->l_Flag & 2)
	{
		MATH_ScaleVector(&a, &_pst_Square->st_A, 0.8f);
		MATH_AddVector(&ast_t[0], &p, &a);
		MATH_ScaleVector(&a, &_pst_Square->st_B, 0.8f);
		MATH_AddVector(&ast_t[1], &p, &a);

		ulBlendingMode = 0;
		MAT_SET_Blending(ulBlendingMode, MAT_Cc_Op_Alpha);
    	Gx8_SetTextureBlending((ULONG) - 1, ulBlendingMode, 0);

		Gx8_RS_DepthFunc(GX8_M_SD(_pst_DD), D3DCMP_ALWAYS);
		pVertexBufferSave = pVertexBuffer;

		CurrentColor = 0xffff0080;
		Gx8_M_Send3Vertex(&p, ast_t + 0, ast_t + 1);
		Gx8_M_Send3Vertex(&p, ast_t + 1, ast_t + 0);

#pragma message("~~~~~~~~~~~~~ Unlock when VB is not locked ?!? ~~~~~~~~~~~~~~~")
        Gx8_vEndRenderUsingTheBigVertexBuffer((pVertexBuffer - pVertexBufferSave) / 3, D3DPT_TRIANGLELIST);
	}

	Gx8_RS_DrawWired(GX8_M_SD(_pst_DD), 0);
	Gx8_RS_LineWidth(GX8_M_RS(_pst_DD), 1.0f);
	_pst_DD->LastDrawMask |= GDI_Cul_DM_NotWired;
	_pst_DD->ul_CurrentDrawMask |= GDI_Cul_DM_NotWired;


}
