/*$T OGLdebugfct.c GC!1.52 10/28/99 12:00:45 */


/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "OGLdebugfct.h"
#include "OGLinit.h"

/*
 ===================================================================================================
    external functions
 ===================================================================================================
 */
extern ULONG LIGHT_ul_Interpol2Colors( ULONG, ULONG, float );

/*
 ===================================================================================================
    Macros
 ===================================================================================================
 */

void OGL_SetColorFor2xDBG( GLubyte *Color )
{
	GLubyte ColorRGBA[ 4 ];
	if ( !GDI_gpst_CurDD->GlobalMul2X )
	{
		glColor4ubv( Color );
		return;
	}
	ColorRGBA[ 0 ] = Color[ 0 ] >> 1;
	ColorRGBA[ 1 ] = Color[ 1 ] >> 1;
	ColorRGBA[ 2 ] = Color[ 2 ] >> 1;
	ColorRGBA[ 3 ] = Color[ 3 ];
	glColor4ubv( ( GLubyte * ) ColorRGBA );
}


#define OGL_M_GetEllipsePoint( alpha )                                 \
	{                                                                  \
		MATH_ScaleVector( &A, &_pst_Ellipse->st_A, fOptCos( alpha ) ); \
		MATH_ScaleVector( &B, &_pst_Ellipse->st_B, fOptSin( alpha ) ); \
		MATH_AddVector( &N, &A, &B );                                  \
		MATH_AddEqualVector( &N, &C );                                 \
	}

#define OGL_M_Send3Vertex( a, b, c )      \
	{                                     \
		glVertex3fv( ( float * ) ( a ) ); \
		glVertex3fv( ( float * ) ( b ) ); \
		glVertex3fv( ( float * ) ( c ) ); \
	}

#define OGL_M_SendColorAnd3Vertex( a, b, c, color )       \
	{                                                     \
		OGL_SetColorFor2xDBG( ( GLubyte * ) ( &color ) ); \
		glVertex3fv( ( float * ) ( a ) );                 \
		glVertex3fv( ( float * ) ( b ) );                 \
		glVertex3fv( ( float * ) ( c ) );                 \
	}

#define OGL_M_DrawLightedTriangle( pt1, pt2, pt3 )           \
	{                                                        \
		MATH_SubVector( &a, ( pt2 ), ( pt1 ) );              \
		MATH_SubVector( &b, ( pt3 ), ( pt1 ) );              \
		MATH_CrossProduct( &n, &a, &b );                     \
		MATH_NormalizeVector( &n, &n );                      \
		if ( ( t = MATH_f_DotProduct( &light, &n ) ) <= 0 )  \
		{                                                    \
			c = LIGHT_ul_Interpol2Colors( 0, ul_Color, -t ); \
			OGL_SetColorFor2xDBG( ( GLubyte * ) &c );        \
			glVertex3fv( ( float * ) ( pt1 ) );              \
			glVertex3fv( ( float * ) ( pt2 ) );              \
			glVertex3fv( ( float * ) ( pt3 ) );              \
		}                                                    \
	}


/*
 ===================================================================================================
    Aim:    Draw an ellipse
 ===================================================================================================
 */
void OGL_DrawEllipse( GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Ellipse *_pst_Ellipse )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float f_Alpha;
	ULONG ulBlendingMode;
	MATH_tdst_Vector A, B, C, M, N;
	ULONG c, c0, c1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ulBlendingMode = 0;
	MAT_SET_Blending( ulBlendingMode, MAT_Cc_Op_Copy );
	OGL_SetTextureBlending( ( ULONG ) -1, ulBlendingMode );

	if ( !( _pst_Ellipse->l_Flag & 8 ) ) OGL_RS_DepthFunc( OGL_M_RS( _pst_DD ), GL_ALWAYS );

	MATH_CopyVector( &C, ( MATH_tdst_Vector * ) &_pst_Ellipse->st_Center );
	c1 = _pst_Ellipse->st_Center.color;
	c0 = ( ( c1 & 0xFEFEFEFE ) >> 1 );

	/* Draw ellipse */
	OGL_RS_LineWidth( OGL_M_RS( _pst_DD ), 1.0f );
	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin( GL_LINES );

	OGL_M_GetEllipsePoint( Cf_2Pi - .1f );
	MATH_CopyVector( &M, &N );

	for ( f_Alpha = 0; f_Alpha <= Cf_2Pi; f_Alpha += 0.1f )
	{
		OGL_M_GetEllipsePoint( f_Alpha );

		if ( ( _pst_Ellipse->l_Flag & 1 ) || ( ( M.z <= C.z ) && ( N.z <= C.z ) ) )
		{
			c = LIGHT_ul_Interpol2Colors( c0, c1, ( ( C.z - M.z ) + .1f ) * 5 );
			OGL_SetColorFor2xDBG( ( GLubyte * ) &c );
			glVertex3fv( ( float * ) &M );
			c = LIGHT_ul_Interpol2Colors( c0, c1, ( ( C.z - N.z ) + .1f ) * 5 );
			OGL_SetColorFor2xDBG( ( GLubyte * ) &c );
			glVertex3fv( ( float * ) &N );
		}

		MATH_CopyVector( &M, &N );
	}

	glEnd();

	if ( !( _pst_Ellipse->l_Flag & 4 ) )
	{
		/* Draw alpha start point */
		OGL_RS_PointSize( OGL_M_RS( _pst_DD ), 10.0f );

		GDI_gpst_CurDD->profilingInformation.numBatches++;
		glBegin( GL_POINTS );
		if ( !GDI_gpst_CurDD->GlobalMul2X )
			glColor4ub( 0x80, 0x80, 0x80, 0xFF );
		else
			glColor4ub( 0x80 >> 1, 0x80 >> 1, 0x80 >> 1, 0xFF );
		if ( _pst_Ellipse->l_Flag & 1 )
		{
			OGL_M_GetEllipsePoint( _pst_Ellipse->f_AlphaStart );
			glVertex3fv( ( float * ) &N );
		}

		if ( !GDI_gpst_CurDD->GlobalMul2X )
			glColor4ub( 0xFF, 0xFF, 0xFF, 0xFF );
		else
			glColor4ub( 0xFF >> 1, 0xFF >> 1, 0xFF >> 1, 0xFF );
		if ( _pst_Ellipse->l_Flag & 2 )
		{
			OGL_M_GetEllipsePoint( _pst_Ellipse->f_AlphaEnd );
			glVertex3fv( ( float * ) &N );
		}

		glEnd();
	}

	/* Reset render parameters */
	OGL_RS_DrawWired( OGL_M_RS( _pst_DD ), 0 );
	OGL_RS_LineWidth( OGL_M_RS( _pst_DD ), 1.0f );
	OGL_RS_PointSize( OGL_M_RS( _pst_DD ), 1.0f );
	_pst_DD->LastDrawMask |= GDI_Cul_DM_NotWired;
	_pst_DD->ul_CurrentDrawMask |= GDI_Cul_DM_NotWired;
}

/*
 ===================================================================================================
    Aim:    Draw an arrow
 ===================================================================================================
 */
void OGL_DrawArrow( GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Arrow *_pst_Arrow )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector a, b, b1, i, j, k, light, n;
	MATH_tdst_Vector pt[ 8 ];
	float f_Scale, t;
	ULONG ulBlendingMode, ul_Color, c;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ulBlendingMode = 0;
	MAT_SET_Blending( ulBlendingMode, MAT_Cc_Op_Copy );
	OGL_SetTextureBlending( ( ULONG ) -1, ulBlendingMode );
	OGL_RS_DepthFunc( OGL_M_RS( _pst_DD ), GL_ALWAYS );
	OGL_RS_DrawWired( OGL_M_RS( _pst_DD ), 0 );
	OGL_RS_LineWidth( OGL_M_RS( _pst_DD ), 1.0f );
	OGL_RS_CullFace( OGL_M_RS( _pst_DD ), 1 );
	OGL_RS_CullFaceInverted( OGL_M_RS( _pst_DD ), 0 );

	ul_Color = _pst_Arrow->st_Start.color;

	MATH_CopyVector( &a, ( MATH_tdst_Vector * ) &_pst_Arrow->st_Start );
	MATH_InitVector( &light, 0, 0, 1 );

	if ( _pst_Arrow->l_Flag & SOFT_Cul_Arrow_UseFactor )
		MATH_ScaleVector( &b, &_pst_Arrow->st_Axis, _pst_Arrow->f_Move );
	else
		MATH_CopyVector( &b, &_pst_Arrow->st_Axis );

	MATH_AddEqualVector( &b, &a );

	if ( !( _pst_Arrow->l_Flag & SOFT_Cul_Arrow_DrawNotLine ) )
	{
		GDI_gpst_CurDD->profilingInformation.numBatches++;
		glBegin( GL_LINES );
		if ( _pst_Arrow->l_Flag & SOFT_Cul_Arrow_ForSpot )
		{
			MATH_AddVector( &b1, &b, &_pst_Arrow->st_Up );
			c = 0xFFFF0000;
			OGL_SetColorFor2xDBG( ( GLubyte * ) &c );
			glVertex3fv( ( float * ) &a );
			glVertex3fv( ( float * ) &b1 );
			MATH_SubVector( &b1, &b, &_pst_Arrow->st_Up );
			glVertex3fv( ( float * ) &a );
			glVertex3fv( ( float * ) &b1 );
		}
		else
		{
			OGL_SetColorFor2xDBG( ( GLubyte * ) &ul_Color );
			glVertex3fv( ( float * ) &a );
			glVertex3fv( ( float * ) &b );
		}
		glEnd();
	}

	MATH_CopyVector( &i, &_pst_Arrow->st_Axis );
	if ( _pst_Arrow->l_Flag & SOFT_Cul_Arrow_ForSpot )
		f_Scale = 0.1f;
	else
		f_Scale = MATH_f_NormVector( &i ) / 10.0f;
	MATH_NormalizeVector( &i, &i );
	MATH_CopyVector( &k, &_pst_Arrow->st_Up );
	MATH_NormalizeVector( &k, &k );
	MATH_CrossProduct( &j, &k, &i );

	MATH_ScaleEqualVector( &i, f_Scale );
	MATH_ScaleEqualVector( &j, f_Scale );
	MATH_ScaleEqualVector( &k, f_Scale );

	MATH_SubVector( &pt[ 0 ], &b, &i );
	MATH_AddEqualVector( &pt[ 0 ], &j );
	MATH_AddEqualVector( &pt[ 0 ], &k );
	MATH_SubVector( &pt[ 1 ], &pt[ 0 ], &j );
	MATH_SubEqualVector( &pt[ 1 ], &j );
	MATH_SubVector( &pt[ 2 ], &pt[ 1 ], &k );
	MATH_SubEqualVector( &pt[ 2 ], &k );
	MATH_AddVector( &pt[ 3 ], &pt[ 2 ], &j );
	MATH_AddEqualVector( &pt[ 3 ], &j );

	if ( _pst_Arrow->l_Flag & SOFT_Cul_Arrow_Cube )
	{
		MATH_ScaleEqualVector( &i, 2 );
		MATH_AddVector( &pt[ 4 ], &pt[ 0 ], &i );
		MATH_AddVector( &pt[ 5 ], &pt[ 1 ], &i );
		MATH_AddVector( &pt[ 6 ], &pt[ 2 ], &i );
		MATH_AddVector( &pt[ 7 ], &pt[ 3 ], &i );

		GDI_gpst_CurDD->profilingInformation.numBatches++;
		glBegin( GL_TRIANGLES );
		OGL_M_DrawLightedTriangle( pt + 0, pt + 2, pt + 1 );
		OGL_M_Send3Vertex( pt + 0, pt + 3, pt + 2 );
		OGL_M_DrawLightedTriangle( pt + 0, pt + 1, pt + 4 );
		OGL_M_Send3Vertex( pt + 1, pt + 5, pt + 4 );
		OGL_M_DrawLightedTriangle( pt + 1, pt + 2, pt + 5 );
		OGL_M_Send3Vertex( pt + 2, pt + 6, pt + 5 );
		OGL_M_DrawLightedTriangle( pt + 2, pt + 3, pt + 6 );
		OGL_M_Send3Vertex( pt + 3, pt + 7, pt + 6 );
		OGL_M_DrawLightedTriangle( pt + 3, pt + 0, pt + 7 );
		OGL_M_Send3Vertex( pt + 0, pt + 4, pt + 7 );
		OGL_M_DrawLightedTriangle( pt + 4, pt + 5, pt + 6 );
		OGL_M_Send3Vertex( pt + 4, pt + 6, pt + 7 );
		glEnd();
	}
	else
	{
		MATH_AddVector( &pt[ 4 ], &b, &i );
		GDI_gpst_CurDD->profilingInformation.numBatches++;
		glBegin( GL_TRIANGLES );
		OGL_M_DrawLightedTriangle( pt + 0, pt + 1, pt + 4 );
		OGL_M_DrawLightedTriangle( pt + 1, pt + 2, pt + 4 );
		OGL_M_DrawLightedTriangle( pt + 2, pt + 3, pt + 4 );
		OGL_M_DrawLightedTriangle( pt + 3, pt + 0, pt + 4 );
		OGL_M_DrawLightedTriangle( pt + 0, pt + 2, pt + 1 );
		OGL_M_Send3Vertex( pt + 0, pt + 3, pt + 2 );
		glEnd();
	}

	OGL_RS_DrawWired( OGL_M_RS( _pst_DD ), 0 );
	OGL_RS_LineWidth( OGL_M_RS( _pst_DD ), 1.0f );
	_pst_DD->LastDrawMask |= GDI_Cul_DM_NotWired;
	_pst_DD->ul_CurrentDrawMask |= GDI_Cul_DM_NotWired;
}

/*
 ===================================================================================================
    Aim:    Draw a square
 ===================================================================================================
 */
void OGL_DrawSquare( GDI_tdst_DisplayData *_pst_DD, SOFT_tdst_Square *_pst_Square )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector p, ast_t[ 2 ], ast_Sqr[ 4 ], a;
	ULONG ul_Color, ulBlendingMode;
	float f_Scale;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CopyVector( &p, ( MATH_tdst_Vector * ) &_pst_Square->st_Pos );
	f_Scale = MATH_f_NormVector( &_pst_Square->st_A ) * .15f;
	MATH_InitVector( &ast_Sqr[ 0 ], p.x - f_Scale, p.y - f_Scale, p.z );
	MATH_InitVector( &ast_Sqr[ 1 ], p.x - f_Scale, p.y + f_Scale, p.z );
	MATH_InitVector( &ast_Sqr[ 2 ], p.x + f_Scale, p.y + f_Scale, p.z );
	MATH_InitVector( &ast_Sqr[ 3 ], p.x + f_Scale, p.y - f_Scale, p.z );

	ulBlendingMode = 0;
	MAT_SET_Blending( ulBlendingMode, MAT_Cc_Op_Alpha );
	OGL_SetTextureBlending( ( ULONG ) -1, ulBlendingMode );
	OGL_RS_DepthFunc( OGL_M_RS( _pst_DD ), GL_ALWAYS );
	OGL_RS_DrawWired( OGL_M_RS( _pst_DD ), 0 );
	OGL_RS_CullFace( OGL_M_RS( _pst_DD ), 1 );
	OGL_RS_CullFaceInverted( OGL_M_RS( _pst_DD ), 0 );

	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin( GL_QUADS );
	_pst_Square->st_Pos.color &= 0x80FFFFFF;
	OGL_SetColorFor2xDBG( ( GLubyte * ) &_pst_Square->st_Pos.color );
	glVertex3fv( ( float * ) ( ast_Sqr + 0 ) );
	glVertex3fv( ( float * ) ( ast_Sqr + 1 ) );
	glVertex3fv( ( float * ) ( ast_Sqr + 2 ) );
	glVertex3fv( ( float * ) ( ast_Sqr + 3 ) );
	glEnd();

	ulBlendingMode = 0;
	MAT_SET_Blending( ulBlendingMode, MAT_Cc_Op_Copy );
	OGL_SetTextureBlending( ( ULONG ) -1, ulBlendingMode );
	OGL_RS_DepthFunc( OGL_M_RS( _pst_DD ), GL_ALWAYS );

	OGL_RS_DrawWired( OGL_M_RS( _pst_DD ), 1 );
	OGL_RS_LineWidth( OGL_M_RS( _pst_DD ), 1.0f );
	ul_Color = 0xFF000000;
	GDI_gpst_CurDD->profilingInformation.numBatches++;
	glBegin( GL_QUADS );
	OGL_SetColorFor2xDBG( ( GLubyte * ) &ul_Color );
	glVertex3fv( ( float * ) ( ast_Sqr + 0 ) );
	glVertex3fv( ( float * ) ( ast_Sqr + 1 ) );
	glVertex3fv( ( float * ) ( ast_Sqr + 2 ) );
	glVertex3fv( ( float * ) ( ast_Sqr + 3 ) );
	glEnd();

	OGL_RS_DrawWired( OGL_M_RS( _pst_DD ), 0 );

	if ( _pst_Square->l_Flag & 2 )
	{
		MATH_ScaleVector( &a, &_pst_Square->st_A, 0.8f );
		MATH_AddVector( &ast_t[ 0 ], &p, &a );
		MATH_ScaleVector( &a, &_pst_Square->st_B, 0.8f );
		MATH_AddVector( &ast_t[ 1 ], &p, &a );

		ulBlendingMode = 0;
		MAT_SET_Blending( ulBlendingMode, MAT_Cc_Op_Alpha );
		OGL_SetTextureBlending( ( ULONG ) -1, ulBlendingMode );
		OGL_RS_DepthFunc( OGL_M_RS( _pst_DD ), GL_ALWAYS );

		GDI_gpst_CurDD->profilingInformation.numBatches++;
		glBegin( GL_TRIANGLES );
		if ( !GDI_gpst_CurDD->GlobalMul2X )
			glColor4ub( 255, 255, 0, 64 );
		else
			glColor4ub( 255 >> 1, 255 >> 1, 0, 64 );
		OGL_M_Send3Vertex( &p, ast_t + 0, ast_t + 1 );
		OGL_M_Send3Vertex( &p, ast_t + 1, ast_t + 0 );
		glEnd();
	}

	OGL_RS_DrawWired( OGL_M_RS( _pst_DD ), 0 );
	OGL_RS_LineWidth( OGL_M_RS( _pst_DD ), 1.0f );
	_pst_DD->LastDrawMask |= GDI_Cul_DM_NotWired;
	_pst_DD->ul_CurrentDrawMask |= GDI_Cul_DM_NotWired;
}
