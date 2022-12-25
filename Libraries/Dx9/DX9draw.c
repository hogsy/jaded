// Dx9draw.c


#include "Dx9draw.h"
#include "Dx9renderstate.h"
#include "Dx9water.h"
#include "Dx9uv.h"
#include "GDInterface/GDInterface.h"
#include <assert.h>

extern void *pst_GlobalsWaterParams;

/************************************************************************************************************************
    Macros
 ************************************************************************************************************************/

#define Dx9_SetColorRGBA(a) \
	if(pst_Color) \
	{ \
		ulOGLSetCol = pst_Color[a] | ulOGLSetCol_Or; \
		ulOGLSetCol ^= ulOGLSetCol_XOr; \
		if(pst_Alpha) \
		{ \
			ulOGLSetCol &= 0x00ffffff; \
			ulOGLSetCol |= pst_Alpha[a]; \
		} \
		pVertexBuffer->Color = Dx9_M_ConvertColor(ulOGLSetCol); \
	} \
	else if(pst_Alpha) \
	{ \
		ulOGLSetCol &= 0x00ffffff; \
		ulOGLSetCol |= pst_Alpha[a]; \
		pVertexBuffer->Color = Dx9_M_ConvertColor(ulOGLSetCol); \
	}


/************************************************************************************************************************
    Public Function
 ************************************************************************************************************************/


/////////////
LONG	Dx9_l_DrawElementIndexedTriangles( GEO_tdst_ElementIndexedTriangles	* pElement,
										  GEO_Vertex						* pPoint,
										  GEO_tdst_UV						* pUV,
										  ULONG								numberOfPoints )
{
	GEO_tdst_Object				* pObject;
	ULONG						ulOGLSetCol, ulOGLSetCol_XOr, ulOGLSetCol_Or;

	if ( !pElement->l_NbTriangles ) 
		return 0;

	(void) numberOfPoints;

	Dx9_BeginScene( );

	pObject = GDI_gpst_CurDD->pst_CurrentGeo;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if ( ( pObject->pIB ) && ( pElement->pst_StripData ) )
	{
		ULONG						* pColorSource;
		ULONG						* pColorData;
		Dx9VertexComponent_Position	* pPointData;
		UINT	drawStripCounter;
		GEO_tdst_StripData	* pStripData = pElement->pst_StripData;

		// Prepare Position Stream
		if ( pPoint == pObject->dst_Point )
		{
			IDirect3DDevice9_SetStreamSource( gDx9SpecificData.pD3DDevice, 0, pObject->pVB_Position, 0, sizeof(Dx9VertexComponent_Position) );
		}
		else
		{
			if ( gDx9SpecificData.positionVB.pCurrGO != GDI_gpst_CurDD->pst_CurrentGameObject )
			{
				if ( gDx9SpecificData.positionVB.nextIndex + pObject->l_NbPoints > CIRCULAR_VB_SIZE )
				{
					gDx9SpecificData.positionVB.currIndex = 0;
					gDx9SpecificData.positionVB.nextIndex = pObject->l_NbPoints;
				}
				else
				{
					gDx9SpecificData.positionVB.currIndex = gDx9SpecificData.positionVB.nextIndex;
					gDx9SpecificData.positionVB.nextIndex += pObject->l_NbPoints;
				}
				gDx9SpecificData.positionVB.pCurrGO = GDI_gpst_CurDD->pst_CurrentGameObject;
				IDirect3DVertexBuffer9_Lock( gDx9SpecificData.positionVB.pVB, gDx9SpecificData.positionVB.currIndex * gDx9SpecificData.positionVB.stride, pObject->l_NbPoints * gDx9SpecificData.positionVB.stride, (BYTE **) &pPointData, D3DLOCK_NOOVERWRITE );	// D3DLOCK_NOOVERWRITE
				memcpy( pPointData, pPoint, pObject->l_NbPoints * sizeof(Dx9VertexComponent_Position) );
				IDirect3DVertexBuffer9_Unlock( gDx9SpecificData.positionVB.pVB );
			}
			if ( FAILED( IDirect3DDevice9_SetStreamSource( gDx9SpecificData.pD3DDevice, 0, gDx9SpecificData.positionVB.pVB, gDx9SpecificData.positionVB.currIndex * gDx9SpecificData.positionVB.stride, gDx9SpecificData.positionVB.stride ) ) )
			{
				assert(FALSE); // failed setting position stream
			}
		}

		// Prepare Color Stream
		pColorSource = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
		ulOGLSetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
		ulOGLSetCol_Or = gDx9SpecificData.ulColorOr;

		if ( ( gDx9SpecificData.colorVB.pCurrGO != GDI_gpst_CurDD->pst_CurrentGameObject ) ||
			( gDx9SpecificData.colorVB.pCurrSource != pColorSource ) ||
			( gDx9SpecificData.colorVB.currData1 != ulOGLSetCol_XOr ) ||
			( gDx9SpecificData.colorVB.currData2 != ulOGLSetCol_Or ) )
		{
			if ( gDx9SpecificData.colorVB.nextIndex + pObject->l_NbPoints > CIRCULAR_VB_SIZE )
			{
				gDx9SpecificData.colorVB.currIndex = 0;
				gDx9SpecificData.colorVB.nextIndex = pObject->l_NbPoints;
			}
			else
			{
				gDx9SpecificData.colorVB.currIndex = gDx9SpecificData.colorVB.nextIndex;
				gDx9SpecificData.colorVB.nextIndex += pObject->l_NbPoints;
			}
			gDx9SpecificData.colorVB.pCurrGO = GDI_gpst_CurDD->pst_CurrentGameObject;
			gDx9SpecificData.colorVB.pCurrSource = pColorSource;
			gDx9SpecificData.colorVB.currData1 = ulOGLSetCol_XOr;
			gDx9SpecificData.colorVB.currData2 = ulOGLSetCol_Or;
			IDirect3DVertexBuffer9_Lock( gDx9SpecificData.colorVB.pVB, gDx9SpecificData.colorVB.currIndex * gDx9SpecificData.colorVB.stride, pObject->l_NbPoints * gDx9SpecificData.colorVB.stride, (BYTE **)&pColorData, D3DLOCK_NOOVERWRITE );	// D3DLOCK_NOOVERWRITE

			if( !pColorSource )
			{
				ULONG	* pDest = (ULONG *) pColorData;
				ULONG	* pEnd = pDest + pObject->l_NbPoints;

				ulOGLSetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulOGLSetCol_Or;
				ulOGLSetCol ^= ulOGLSetCol_XOr;
				ulOGLSetCol = Dx9_M_ConvertColor(ulOGLSetCol);

				for ( ; pDest < pEnd; pDest++ )
					* pDest = ulOGLSetCol;
			}
			else
			{
				ULONG	* pDest = (ULONG *) pColorData;
				ULONG	* pSource = (ULONG *) pColorSource;
				ULONG	* pEnd = pDest + pObject->l_NbPoints;
				ULONG	col;

				for ( ; pDest < pEnd; pDest++ )
				{
					col = ( ( ( * pSource ) | ulOGLSetCol_Or ) ^ ulOGLSetCol_XOr );
					* pDest = Dx9_M_ConvertColor( col );
					pSource++;
				}
			}

			IDirect3DVertexBuffer9_Unlock( gDx9SpecificData.colorVB.pVB );
		}
		if ( FAILED( IDirect3DDevice9_SetStreamSource( gDx9SpecificData.pD3DDevice, 1, gDx9SpecificData.colorVB.pVB, gDx9SpecificData.colorVB.currIndex * gDx9SpecificData.colorVB.stride, gDx9SpecificData.colorVB.stride ) ) )
		{
			assert(FALSE); // failed setting color stream
		}

		IDirect3DDevice9_SetStreamSource( gDx9SpecificData.pD3DDevice, 3, NULL, 0, 0 );

		IDirect3DDevice9_SetIndices( gDx9SpecificData.pD3DDevice, pObject->pIB );

		for ( drawStripCounter = 0; drawStripCounter < pStripData->drawStripNumber; drawStripCounter++)
		{
			GEO_tdst_DrawStrip	* pDrawStrip = &pStripData->pDrawStripList[ drawStripCounter ];

			// Set UV source stream and Vertex Declaration
			if ( pObject->pVBlist_UV )
			{
				IDirect3DDevice9_SetStreamSource( gDx9SpecificData.pD3DDevice, 2, pObject->pVBlist_UV[ pDrawStrip->UVbufferIndex ], 0, sizeof(Dx9VertexComponent_UV) );
				IDirect3DDevice9_SetVertexDeclaration( gDx9SpecificData.pD3DDevice, gDx9SpecificData.pVertexDeclarationWithUV );
			}
			else
			{
				IDirect3DDevice9_SetStreamSource( gDx9SpecificData.pD3DDevice, 2, NULL, 0, 0 );
				IDirect3DDevice9_SetVertexDeclaration( gDx9SpecificData.pD3DDevice, gDx9SpecificData.pVertexDeclaration );
			}

			IDirect3DDevice9_DrawIndexedPrimitive( gDx9SpecificData.pD3DDevice, 
												   D3DPT_TRIANGLESTRIP,
												   0,
												   0,
												   pObject->l_NbPoints,
												   pDrawStrip->startingIndex,
												   pDrawStrip->vertexNumber - 2 );
		}

		if(pst_GlobalsWaterParams) // TODO: (optimization) add check for UI
		{
			Dx9_BeginDrawReflection();
			for ( drawStripCounter = 0; drawStripCounter < pStripData->drawStripNumber; drawStripCounter++)
			{
				GEO_tdst_DrawStrip	* pDrawStrip = &pStripData->pDrawStripList[ drawStripCounter ];

				// Set UV source stream and Vertex Declaration
				if ( pObject->pVBlist_UV )
				{
					IDirect3DDevice9_SetStreamSource( gDx9SpecificData.pD3DDevice, 2, pObject->pVBlist_UV[ pDrawStrip->UVbufferIndex ], 0, sizeof(Dx9VertexComponent_UV) );
					IDirect3DDevice9_SetVertexDeclaration( gDx9SpecificData.pD3DDevice, gDx9SpecificData.pVertexDeclarationWithUV );
				}
				else
				{
					IDirect3DDevice9_SetStreamSource( gDx9SpecificData.pD3DDevice, 2, NULL, 0, 0 );
					IDirect3DDevice9_SetVertexDeclaration( gDx9SpecificData.pD3DDevice, gDx9SpecificData.pVertexDeclaration );
				}

				IDirect3DDevice9_DrawIndexedPrimitive( gDx9SpecificData.pD3DDevice,
													   D3DPT_TRIANGLESTRIP,
													   0,
													   0,
													   pObject->l_NbPoints,
													   pDrawStrip->startingIndex,
													   pDrawStrip->vertexNumber - 2 );
			}
			Dx9_EndDrawReflection();
		}

		IDirect3DDevice9_SetStreamSource( gDx9SpecificData.pD3DDevice, 0, NULL, 0, 0 );
		IDirect3DDevice9_SetStreamSource( gDx9SpecificData.pD3DDevice, 1, NULL, 0, 0 );
		IDirect3DDevice9_SetStreamSource( gDx9SpecificData.pD3DDevice, 2, NULL, 0, 0 );
		IDirect3DDevice9_SetStreamSource( gDx9SpecificData.pD3DDevice, 3, NULL, 0, 0 );

		return 0;
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		Dx9VertexFormat				* pVertexBuffer;
		GEO_tdst_IndexedTriangle	* t, * tend;
		ULONG						* pst_Color, *pst_Alpha;
		ULONG						TNum, ulOGLSetCol, ulOGLSetCol_XOr, ulOGLSetCol_Or;
	    
		TNum = pElement->l_NbTriangles;
		if ( !TNum ) 
			return 0;

		IDirect3DVertexBuffer9_Lock( gDx9SpecificData.m_VertexBuffer, 0, TNum * sizeof(Dx9VertexFormat) * 3, (unsigned char **) &pVertexBuffer, D3DLOCK_DISCARD );

		ulOGLSetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
		pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
		pst_Alpha = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentAlphaField;
		ulOGLSetCol_Or = ((Dx9_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData)->ulColorOr;
		if(pst_Color == NULL)
		{
			ulOGLSetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulOGLSetCol_Or;
			ulOGLSetCol ^= ulOGLSetCol_XOr;
			ulOGLSetCol = Dx9_M_ConvertColor(ulOGLSetCol);
		}

		// Fill
		t = pElement->dst_Triangle;
		tend = pElement->dst_Triangle + TNum;
		if(pUV)
		{
			if(pst_Color)
			{
				if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
				{
					while(t < tend)
					{
						*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[0]];
						Dx9_SetColorRGBA(t->auw_Index[0]);
						pVertexBuffer->fU = pUV[t->auw_Index[0]].fU;
						pVertexBuffer->fV = pUV[t->auw_Index[0]].fV;
						pVertexBuffer++;

						*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[1]];
						Dx9_SetColorRGBA(t->auw_Index[1]);
						pVertexBuffer->fU = pUV[t->auw_Index[1]].fU;
						pVertexBuffer->fV = pUV[t->auw_Index[1]].fV;
						pVertexBuffer++;

						*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[2]];
						Dx9_SetColorRGBA(t->auw_Index[2]);
						pVertexBuffer->fU = pUV[t->auw_Index[2]].fU;
						pVertexBuffer->fV = pUV[t->auw_Index[2]].fV;
						pVertexBuffer++;

						t++;
					}
				}
				else
				{
					while(t < tend)
					{
						*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[0]];
						Dx9_SetColorRGBA(t->auw_Index[0]);
						pVertexBuffer->fU = pUV[t->auw_UV[0]].fU;
						pVertexBuffer->fV = pUV[t->auw_UV[0]].fV;
						pVertexBuffer++;

						*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[1]];
						Dx9_SetColorRGBA(t->auw_Index[1]);
						pVertexBuffer->fU = pUV[t->auw_UV[1]].fU;
						pVertexBuffer->fV = pUV[t->auw_UV[1]].fV;
						pVertexBuffer++;

						*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[2]];
						Dx9_SetColorRGBA(t->auw_Index[2]);
						pVertexBuffer->fU = pUV[t->auw_UV[2]].fU;
						pVertexBuffer->fV = pUV[t->auw_UV[2]].fV;
						pVertexBuffer++;
						t++;
					}
				}
			}
			else
			{
				if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
				{
					while(t < tend)
					{
						*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[0]];
						pVertexBuffer->Color = ulOGLSetCol;
						pVertexBuffer->fU = pUV[t->auw_Index[0]].fU;
						pVertexBuffer->fV = pUV[t->auw_Index[0]].fV;
						pVertexBuffer++;

						*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[1]];
						pVertexBuffer->Color = ulOGLSetCol;
						pVertexBuffer->fU = pUV[t->auw_Index[1]].fU;
						pVertexBuffer->fV = pUV[t->auw_Index[1]].fV;
						pVertexBuffer++;

						*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[2]];
						pVertexBuffer->Color = ulOGLSetCol;
						pVertexBuffer->fU = pUV[t->auw_Index[2]].fU;
						pVertexBuffer->fV = pUV[t->auw_Index[2]].fV;
						pVertexBuffer++;

						t++;
					}
				}
				else
				{
					while(t < tend)
					{
						*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[0]];
						pVertexBuffer->Color = ulOGLSetCol;
						pVertexBuffer->fU = pUV[t->auw_UV[0]].fU;
						pVertexBuffer->fV = pUV[t->auw_UV[0]].fV;
						pVertexBuffer++;

						*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[1]];
						pVertexBuffer->Color = ulOGLSetCol;
						pVertexBuffer->fU = pUV[t->auw_UV[1]].fU;
						pVertexBuffer->fV = pUV[t->auw_UV[1]].fV;
						pVertexBuffer++;

						*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[2]];
						pVertexBuffer->Color = ulOGLSetCol;
						pVertexBuffer->fU = pUV[t->auw_UV[2]].fU;
						pVertexBuffer->fV = pUV[t->auw_UV[2]].fV;
						pVertexBuffer++;

						t++;
					}
				}
			}
		}
		else
		{
			if(pst_Color)
			{
				while(t < tend)
				{
					*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[0]];
					Dx9_SetColorRGBA(t->auw_Index[0]);
					pVertexBuffer++;

					*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[1]];
					Dx9_SetColorRGBA(t->auw_Index[1]);
					pVertexBuffer++;

					*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[2]];
					Dx9_SetColorRGBA(t->auw_Index[2]);
					pVertexBuffer++;

					t++;
				}
			}
			else
			{
				while(t < tend)
				{
					*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[0]];
					pVertexBuffer->Color = ulOGLSetCol;
					pVertexBuffer++;

					*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[1]];
					pVertexBuffer->Color = ulOGLSetCol;
					pVertexBuffer++;

					*(GEO_Vertex *) pVertexBuffer = pPoint[t->auw_Index[2]];
					pVertexBuffer->Color = ulOGLSetCol;
					pVertexBuffer++;

					t++;
				}
			}
		}

		// End fill
		IDirect3DVertexBuffer9_Unlock( gDx9SpecificData.m_VertexBuffer);
		IDirect3DDevice9_SetStreamSource( gDx9SpecificData.pD3DDevice, 0, gDx9SpecificData.m_VertexBuffer, 0, sizeof(Dx9VertexFormat) );
		IDirect3DDevice9_SetFVF(gDx9SpecificData.pD3DDevice, D3DFVF_VERTEXF );
		IDirect3DDevice9_DrawPrimitive( gDx9SpecificData.pD3DDevice, D3DPT_TRIANGLELIST, 0, TNum );

		/*if(pst_GlobalsWaterParams)
		{
			Dx9_BeginDrawReflection();
			IDirect3DDevice9_DrawPrimitive( gDx9SpecificData.pD3DDevice, D3DPT_TRIANGLELIST, 0, TNum );
			Dx9_EndDrawReflection();
		}*/

		return 0;
	}
}



/////////////
LONG	Dx9_l_DrawElementIndexedSprites( GEO_tdst_ElementIndexedSprite	*pElement,
										GEO_Vertex						*pPoint,
										ULONG							numberOfPoints )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		XCam, YCam, *p_point;
	MATH_tdst_Vector		Sprite[5];
	float					Size;
	GDI_tdst_DisplayData	*pst_DD;
	GEO_tdst_IndexedSprite	*p_Frst, *p_Last;
    ULONG					ul_ColorOr, ul_ColorXor, ul_ColorSet;
	ULONG					*pst_Color;
	ULONG					*pst_Alpha;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	(void) numberOfPoints;

	pst_DD = GDI_gpst_CurDD;

	Dx9_SetUVStageOff();

	pst_Color = pst_DD->pst_ComputingBuffers->CurrentColorField;
	pst_Alpha = pst_DD->pst_ComputingBuffers->CurrentAlphaField;
	ul_ColorOr = gDx9SpecificData.ulColorOr;
    ul_ColorXor = pst_DD->pst_ComputingBuffers->ulColorXor;
	ul_ColorSet = pst_DD->pst_ComputingBuffers->ul_Ambient | ul_ColorOr;
	ul_ColorSet ^= ul_ColorXor;

	XCam.x = -pst_DD->st_MatrixStack.pst_CurrentMatrix->Ix * pElement->fGlobalSize * pElement->fGlobalRatio;
	XCam.y = -pst_DD->st_MatrixStack.pst_CurrentMatrix->Jx * pElement->fGlobalSize * pElement->fGlobalRatio;
	XCam.z = -pst_DD->st_MatrixStack.pst_CurrentMatrix->Kx * pElement->fGlobalSize * pElement->fGlobalRatio;
	YCam.x = pst_DD->st_MatrixStack.pst_CurrentMatrix->Iy * pElement->fGlobalSize;
	YCam.y = pst_DD->st_MatrixStack.pst_CurrentMatrix->Jy * pElement->fGlobalSize;
	YCam.z = pst_DD->st_MatrixStack.pst_CurrentMatrix->Ky * pElement->fGlobalSize;
	pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_BeforeDrawSprite, pElement->l_NbSprites);

	p_Frst = pElement->dst_Sprite;
	p_Last = p_Frst + pElement->l_NbSprites;
	while(p_Frst < p_Last)
	{
        /* calcul de la couleur du sprite */
        if( pst_Color ) 
		{ 
			ul_ColorSet = (pst_Color[p_Frst->auw_Index] | ul_ColorOr) ^ ul_ColorXor; 
			if(pst_Alpha) 
				ul_ColorSet = (ul_ColorSet & 0x00ffffff) | pst_Alpha[p_Frst->auw_Index]; 
		} 
		else if(pst_Alpha) 
			ul_ColorSet = (ul_ColorSet & 0x00ffffff) |pst_Alpha[p_Frst->auw_Index]; 

        *(ULONG *) &Sprite[4].x =  ul_ColorSet;
		
		p_point = pPoint + p_Frst->auw_Index;
		Size = *(float *) p_Frst;

		Sprite[0].x = p_point->x + (-XCam.x - YCam.x) * Size;
		Sprite[0].y = p_point->y + (-XCam.y - YCam.y) * Size;
		Sprite[0].z = p_point->z + (-XCam.z - YCam.z) * Size;
		Sprite[1].x = p_point->x + (+XCam.x - YCam.x) * Size;
		Sprite[1].y = p_point->y + (+XCam.y - YCam.y) * Size;
		Sprite[1].z = p_point->z + (+XCam.z - YCam.z) * Size;
		Sprite[2].x = p_point->x + (+XCam.x + YCam.x) * Size;
		Sprite[2].y = p_point->y + (+XCam.y + YCam.y) * Size;
		Sprite[2].z = p_point->z + (+XCam.z + YCam.z) * Size;
		Sprite[3].x = p_point->x + (-XCam.x + YCam.x) * Size;
		Sprite[3].y = p_point->y + (-XCam.y + YCam.y) * Size;
		Sprite[3].z = p_point->z + (-XCam.z + YCam.z) * Size;

		pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSpriteUV, (ULONG) Sprite);
		p_Frst++;
	}

	pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_AfterDrawSprite, 0);
	return 0;
}