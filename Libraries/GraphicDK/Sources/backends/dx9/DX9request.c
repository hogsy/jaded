// Dx9request.c


#include "Dx9request.h"
#include "Dx9renderstate.h"
#include "GDInterface/GDInterface.h"
#include "d3dx9tex.h"
#include "TEXture/TEXstruct.h"

/************************************************************************************************************************
    Private Function declaration
 ************************************************************************************************************************/

void	Dx9_BeforeDrawSprite( ULONG numberOfSprites );
void	Dx9_DrawSprite( MATH_tdst_Vector *v );
void	Dx9_AfterDrawSprite( void );
void	Dx9_ReadScreen( GDI_tdst_Request_RWPixels *d );
void	Dx9_ReloadTexture( GDI_tdst_Request_ReloadTextureParams *pRTP );


/************************************************************************************************************************
    Public Function
 ************************************************************************************************************************/

LONG	Dx9_l_Request( ULONG request, ULONG data )
{
	switch(request)
	{
	case GDI_Cul_Request_BeforeDrawSprite:			
		Dx9_BeforeDrawSprite( data );
		break;

	case GDI_Cul_Request_DrawSprite:				
	case GDI_Cul_Request_DrawSpriteUV:				
		Dx9_DrawSprite( (MATH_tdst_Vector *) data );
		break;

	case GDI_Cul_Request_AfterDrawSprite:			
		Dx9_AfterDrawSprite( );
		break;

	case GDI_Cul_Request_ReadScreen:				
		Dx9_ReadScreen( (GDI_tdst_Request_RWPixels *) data ); 
		break;

	case GDI_Cul_Request_ReloadTexture:				
		Dx9_ReloadTexture( (GDI_tdst_Request_ReloadTextureParams *) data); 
		break;

/*
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
	case GDI_Cul_Request_ReloadTexture2:			DX8_ReloadTexture2( GDI_gpst_CurDD, (GDI_tdst_Request_ReloadTextureParams *) _ul_Data); break;
	case GDI_Cul_Request_SizeOfPoints:													DX8_PointSize( GDI_gpst_CurDD, *(float *) &_ul_Data ); break;
	case GDI_Cul_Request_SizeOfLine:				DX8_LineSize(GDI_gpst_CurDD, *(float *) &_ul_Data ); break;
	case GDI_Cul_Request_DrawLineEx:				DX8_DrawLineEx(GDI_gpst_CurDD, (GDI_tdst_Request_DrawLineEx *) _ul_Data); break;
	case GDI_Cul_Request_DrawPointEx:				DX8_DrawPointEx(GDI_gpst_CurDD, (GDI_tdst_Request_DrawPointEx *)_ul_Data ); break;
	case GDI_Cul_Request_DrawPointSize:				DX8_DrawPoint_Size(GDI_gpst_CurDD, (void  **)_ul_Data);break;
	case GDI_Cul_Request_ReloadPalette:				break; // DX8_ReloadPalette( GDI_gpst_CurDD, (short) _ul_Data ); 
	case GDI_Cul_Request_Draw2DTriangle:			DX8_Draw2DTriangle(GDI_gpst_CurDD, (SOFT_tdst_Vertex *) _ul_Data); break;
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
*/
	}

	return 0;
}


/************************************************************************************************************************
    Private Functions
 ************************************************************************************************************************/

////////////////////
void	Dx9_BeforeDrawSprite( ULONG numberOfSprites )
{
	ULONG					ulBlendingMode;

    if( numberOfSprites == 0 ) 
		return;

	ulBlendingMode = 0;

	Dx9_RS_FillMode( D3DFILL_SOLID );
	Dx9_RS_CullMode( D3DCULL_CW );

	Dx9_BeginScene( );

	gDx9SpecificData.numberOfSpritePrimitiveToFill = gDx9SpecificData.numberOfSpritePrimitive = numberOfSprites * 2;

	IDirect3DVertexBuffer9_Lock( gDx9SpecificData.m_VertexBuffer, 
								 0, 
								 numberOfSprites * sizeof(Dx9VertexFormat) * 6, 
								 (unsigned char **) &gDx9SpecificData.pLockedVertex, 
								 D3DLOCK_DISCARD );
}

////////////////////
void	Dx9_DrawSprite( MATH_tdst_Vector *v )
{
	static float			tf_UV[5] = { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f };
	ULONG					ul_Color;

	if( gDx9SpecificData.pLockedVertex == NULL ) 
		return;

    if ( gDx9SpecificData.numberOfSpritePrimitiveToFill == 0)
        return;

	ul_Color = Dx9_M_ConvertColor(*(ULONG *) &v[4].x);

	*(GEO_Vertex *) gDx9SpecificData.pLockedVertex = v[0];
	gDx9SpecificData.pLockedVertex->Color = ul_Color;
	gDx9SpecificData.pLockedVertex->fU = tf_UV[3];
	gDx9SpecificData.pLockedVertex->fV = tf_UV[4];
	gDx9SpecificData.pLockedVertex++;

	*(GEO_Vertex *) gDx9SpecificData.pLockedVertex = v[1];
	gDx9SpecificData.pLockedVertex->Color = ul_Color;
	gDx9SpecificData.pLockedVertex->fU = tf_UV[2];
	gDx9SpecificData.pLockedVertex->fV = tf_UV[3];
	gDx9SpecificData.pLockedVertex++;

	*(GEO_Vertex *) gDx9SpecificData.pLockedVertex = v[2];
	gDx9SpecificData.pLockedVertex->Color = ul_Color;
	gDx9SpecificData.pLockedVertex->fU = tf_UV[1];
	gDx9SpecificData.pLockedVertex->fV = tf_UV[2];
	gDx9SpecificData.pLockedVertex++;

	*(GEO_Vertex *) gDx9SpecificData.pLockedVertex = v[0];
	gDx9SpecificData.pLockedVertex->Color = ul_Color;
	gDx9SpecificData.pLockedVertex->fU = tf_UV[3];
	gDx9SpecificData.pLockedVertex->fV = tf_UV[4];
	gDx9SpecificData.pLockedVertex++;

	*(GEO_Vertex *) gDx9SpecificData.pLockedVertex = v[2];
	gDx9SpecificData.pLockedVertex->Color = ul_Color;
	gDx9SpecificData.pLockedVertex->fU = tf_UV[1];
	gDx9SpecificData.pLockedVertex->fV = tf_UV[2];
	gDx9SpecificData.pLockedVertex++;

	*(GEO_Vertex *) gDx9SpecificData.pLockedVertex = v[3];
	gDx9SpecificData.pLockedVertex->Color = ul_Color;
	gDx9SpecificData.pLockedVertex->fU = tf_UV[0];
	gDx9SpecificData.pLockedVertex->fV = tf_UV[1];
	gDx9SpecificData.pLockedVertex++;

    gDx9SpecificData.numberOfSpritePrimitiveToFill -= 2;
}

////////////////////
void	Dx9_AfterDrawSprite( void )
{
	if(gDx9SpecificData.pLockedVertex == NULL) 
		return;

	IDirect3DVertexBuffer9_Unlock( gDx9SpecificData.m_VertexBuffer );
	IDirect3DDevice9_SetStreamSource( gDx9SpecificData.pD3DDevice, 0, gDx9SpecificData.m_VertexBuffer, 0, sizeof(Dx9VertexFormat) );
	IDirect3DDevice9_SetFVF( gDx9SpecificData.pD3DDevice, D3DFVF_VERTEXF );

    IDirect3DDevice9_DrawPrimitive(gDx9SpecificData.pD3DDevice, D3DPT_TRIANGLELIST, 0, gDx9SpecificData.numberOfSpritePrimitive);

    gDx9SpecificData.pLockedVertex = NULL;

}

////////////////////
void	Dx9_ReadScreen( GDI_tdst_Request_RWPixels *d )
{
	if (d->c_Write == 0)
    {
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		D3DLOCKED_RECT stLockedRect;
		unsigned char *p_cBitPtr, *p_cDestPtr, *p_cBitPtr2;
		int iLine, iColumn;
		RECT stRect;
		IDirect3DSurface9 *pRenderSurface;
		IDirect3DSurface9 *pSnapSurface;
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        if (d->c_Buffer == 2)
            return;

		stRect.left = d->x;
		stRect.right = d->x + d->w;
		stRect.top = d->y;
		stRect.bottom = d->y + d->h;

		// stop rendering...
		Dx9_EndScene();

		// Get render surface
		IDirect3DDevice9_GetRenderTarget( gDx9SpecificData.pD3DDevice, 0, &pRenderSurface);

		// Create snap surface
		IDirect3DDevice9_CreateOffscreenPlainSurface( gDx9SpecificData.pD3DDevice, 
										640, 480, gDx9SpecificData.AdapterFormat, 
										D3DPOOL_SYSTEMMEM, 
										&pSnapSurface, NULL );

		// Copy render surface to snap surface
		IDirect3DDevice9_GetRenderTargetData( gDx9SpecificData.pD3DDevice, 
											pRenderSurface, pSnapSurface );

		// Lock render surface
		IDirect3DSurface9_LockRect( pSnapSurface, &stLockedRect, &stRect, 0);

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
		IDirect3DSurface9_UnlockRect( pSnapSurface);

		// release resources
		IDirect3DSurface9_Release( pSnapSurface);
		IDirect3DSurface9_Release( pRenderSurface);

		// ... rebegin rendering
		Dx9_BeginScene();

    }
}

////////////////////
void	Dx9_ReloadTexture( GDI_tdst_Request_ReloadTextureParams *pRTP )
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	TEX_tdst_Data			*pst_Tex;
    HRESULT                 hr;
	IDirect3DTexture9		*Dx9_TextureHard;
    IDirect3DSurface9	    *Dx9_Surface;
	RECT					stSrcRect;
	ULONG					*pul_Buffer;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	pst_Tex = &TEX_gst_GlobalList.dst_Texture[pRTP->w_Texture];

	if ( !gDx9SpecificData.TextureList[pRTP->w_Texture] )
		hr = IDirect3DDevice9_CreateTexture( gDx9SpecificData.pD3DDevice, 
										pst_Tex->w_Width, pst_Tex->w_Height, 
										1, 0, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, 
										&Dx9_TextureHard, NULL );
	else
		Dx9_TextureHard = (IDirect3DTexture9 *) gDx9SpecificData.TextureList[pRTP->w_Texture];

	pul_Buffer = (ULONG *)pRTP->pc_Data;

	IDirect3DTexture9_GetSurfaceLevel(Dx9_TextureHard, 0, &Dx9_Surface);

	stSrcRect.left = stSrcRect.top = 0;
	stSrcRect.right = pst_Tex->w_Width;
	stSrcRect.bottom = pst_Tex->w_Height;

	D3DXLoadSurfaceFromMemory( Dx9_Surface, NULL, NULL, pul_Buffer, 
						D3DFMT_A8R8G8B8, pst_Tex->w_Width*4, NULL, 
						&stSrcRect, D3DX_FILTER_POINT, 0);

	D3DXFilterTexture( (LPDIRECT3DBASETEXTURE9) Dx9_TextureHard, NULL, 0, D3DX_FILTER_LINEAR);

	IDirect3DSurface9_Release( Dx9_Surface );
	(IDirect3DTexture9 *) gDx9SpecificData.TextureList[pRTP->w_Texture] = Dx9_TextureHard;
}

