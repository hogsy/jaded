// Gx8buffer.c


#include <xtl.h>
#include <D3D8.h>
#include "Gx8buffer.h"
#include <Gx8/Gx8Init.h>
#include "Gx8VertexBuffer.h"
#include "ANImation/ANIstruct.h"
#include "OBJects/OBJaccess.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "BASe/MEMory/MEM.h"
#include <assert.h>


extern Gx8_tdst_SpecificData	* p_gGx8SpecificData;
extern BOOL Normalmap;

	// FRANCY: detect number and size of added UV buffers
int		numberOfAddUVbuffer = 0;
int		numberOfAddedUVBytes = 0;

/************************************************************************************************************************
    Constants
 ************************************************************************************************************************/

#define		MAX_UV_BUFFER_NUMBER	50
//#define USE_TANGENTE

/************************************************************************************************************************
    Private Function declaration
 ************************************************************************************************************************/

void	Gx8_GenerateVB( WOR_tdst_World *pWorld );
void	Gx8_GenerateVBForGameObjectIfNeeded( OBJ_tdst_GameObject *pst_GO );
void	Gx8_GenerateVBForObject( GEO_tdst_Object *pObject );


/************************************************************************************************************************
    Public Function
 ************************************************************************************************************************/

/////////////
void	Gx8_PrepareVB( WOR_tdst_World *pWorld )
{
		// 
	Gx8_GenerateVB( pWorld );
}


/////////////
void	Gx8_ReleaseVBForObject( GEO_tdst_Object *pObject )
{
	if ( pObject->pVB_Position )
	{
        Gx8_VertexBuffer_Free( pObject->pVB_Position );
        /* MAX
		IDirect3DVertexBuffer8_Release( pObject->pVB_Position );
        */
		pObject->pVB_Position = NULL;
	}
	if ( pObject->pVB_Normal )
	{
        Gx8_VertexBuffer_Free( pObject->pVB_Normal );
        /* MAX
		IDirect3DVertexBuffer8_Release( pObject->pVB_Normal );
        */
		pObject->pVB_Normal = NULL;
	}
	if ( pObject->pVB_Tangente )
	{
        Gx8_VertexBuffer_Free( pObject->pVB_Tangente );
        /* MAX
		IDirect3DVertexBuffer8_Release( pObject->pVB_Normal );
        */
		pObject->pVB_Tangente = NULL;
	}
/*	if ( pObject->pVB_Binormales )
	{
        Gx8_VertexBuffer_Free( pObject->pVB_Binormales );
        
		//IDirect3DVertexBuffer8_Release( pObject->pVB_Normal );
        
		pObject->pVB_Binormales = NULL;
	}*/

	if ( pObject->pVB_Color )
	{
        Gx8_VertexBuffer_Free( pObject->pVB_Color );
        /* MAX
		IDirect3DVertexBuffer8_Release( pObject->pVB_Color );
        */
		pObject->pVB_Color = NULL;
	}
	if ( pObject->pVBlist_UV )
	{
		ULONG i;

			// FRANCY: detect number and size of added UV buffers
		numberOfAddUVbuffer -= pObject->UVbufferNumber - 1;
		numberOfAddedUVBytes -= ( pObject->UVbufferNumber - 1 ) * ( pObject->l_NbPoints * 8 );
		for ( i = 0; i < pObject->UVbufferNumber; i++ )
		{
			if ( pObject->pVBlist_UV[i] )
			{
                Gx8_VertexBuffer_Free( pObject->pVBlist_UV[i] );
                /* MAX
				IDirect3DVertexBuffer8_Release( pObject->pVBlist_UV[i] );
                */
				pObject->pVBlist_UV[i] = NULL;
			}
		}
		MEM_Free( pObject->pVBlist_UV );
		pObject->pVBlist_UV = NULL;
	}

	if ( pObject->pIB )
	{
		IDirect3DIndexBuffer8_Release( pObject->pIB );
		pObject->pIB = NULL;
	}
}

/************************************************************************************************************************
    Private Functions
 ************************************************************************************************************************/

////////////////////
void	Gx8_GenerateVB( WOR_tdst_World *pWorld )
{
	TAB_tdst_PFelem *pst_PFElem, *pst_PFLastElem;

	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&pWorld->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&pWorld->st_AllWorldObjects);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		OBJ_tdst_GameObject *pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;

		Gx8_GenerateVBForGameObjectIfNeeded(pst_GO);

        if ((OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims)) &&
            (pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton != NULL))
        {
            TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	        pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
	        pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
	        for(; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++)
	        {
                OBJ_tdst_GameObject *pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

				Gx8_GenerateVBForGameObjectIfNeeded(pst_BoneGO);
            }
        }
	}
}

////////////////////
void	Gx8_GenerateVBForGameObjectIfNeeded( OBJ_tdst_GameObject *pst_GO )
{
	if ( OBJ_b_TestIdentityFlag( pst_GO, OBJ_C_IdentityFlag_Visu ) )
	{
		if ( pst_GO->pst_Base->pst_Visu->pst_Object ) 
		{
			switch ( pst_GO->pst_Base->pst_Visu->pst_Object->i->ul_Type )
			{
			case GRO_Geometric :
				{
					GEO_tdst_Object *pObject = (GEO_tdst_Object *)pst_GO->pst_Base->pst_Visu->pst_Object;

					if ( ( pObject->l_NbElements ) && ( pObject->dst_Element ) && ( pObject->l_NbPoints ) )
						Gx8_GenerateVBForObject( pObject );

				} break;

			case GRO_GeoStaticLOD :
				{
					int i;
					GEO_tdst_StaticLOD *p_stLOD = (GEO_tdst_StaticLOD *)pst_GO->pst_Base->pst_Visu->pst_Object;

					for( i = 0; i < p_stLOD->uc_NbLOD; i++)
					{
						GEO_tdst_Object *pObject = (GEO_tdst_Object *)p_stLOD->dpst_Id[i];

						if ( ( pObject->l_NbElements ) && ( pObject->dst_Element ) && ( pObject->l_NbPoints ) )
							Gx8_GenerateVBForObject( pObject );
					}
				} break;
			default:
				break;
			}
		}
		// else this object will be used by GFX to render generated geometries.
	}
}

////////////////////
void
Gx8_GenerateVBForObject( GEO_tdst_Object *pObject )
{
	Gx8_tdst_SpecificData	* pSD;
	DWORD	FVF;
	UINT	stride;
	void	* pData;

	// To be sure that Normals have been allocated and calculated !
	GEO_UseNormals(pObject);

	// get SpecificData
	pSD = p_gGx8SpecificData;

		// position VB
		if ( !pObject->pVB_Position )
		{
			FVF = D3DFVF_XYZ;
			stride = D3DXGetFVFVertexSize( FVF );

			// create position VB
			pObject->pVB_Position = Gx8_VertexBuffer_Allocate( pObject->l_NbPoints, stride ); 
			assert( pObject->pVB_Position != NULL );
			/* MAX
			if ( FAILED( IDirect3DDevice8_CreateVertexBuffer( pSD->mp_D3DDevice,
																pObject->l_NbPoints * stride,
																D3DUSAGE_WRITEONLY,
																FVF,
																D3DPOOL_DEFAULT,
																&pObject->pVB_Position ) ) )
			{
				assert(FALSE);
			}
			*/
	        
			// fill position VB
			IDirect3DVertexBuffer8_Lock( pObject->pVB_Position, 0, 0, (void *) &pData, 0 );
			memcpy( pData, pObject->dst_Point, pObject->l_NbPoints * stride );
			IDirect3DVertexBuffer8_Unlock( pObject->pVB_Position );
		}

		// Normals VB
		if ( !pObject->pVB_Normal )
		{
			FVF = D3DFVF_XYZ;
			stride = D3DXGetFVFVertexSize( FVF );

			// create position VB
			pObject->pVB_Normal= Gx8_VertexBuffer_Allocate( pObject->l_NbPoints, stride ); 
			assert( pObject->pVB_Position != NULL );

			// fill position VB
			IDirect3DVertexBuffer8_Lock( pObject->pVB_Normal, 0, 0, (void *) &pData, 0 );
			memcpy( pData, pObject->dst_PointNormal, pObject->l_NbPoints * stride );
			IDirect3DVertexBuffer8_Unlock( pObject->pVB_Normal );
		}
if (Normalmap)
{
		// Tangente VB
		if ( !pObject->pVB_Tangente )
		{
			FVF = D3DFVF_XYZRHW;
			stride = D3DXGetFVFVertexSize( FVF );

			// create position VB
			pObject->pVB_Tangente= Gx8_VertexBuffer_Allocate( pObject->l_NbPoints, stride ); 
			assert( pObject->pVB_Tangente != NULL );


			/* MAX
			if ( FAILED( IDirect3DDevice8_CreateVertexBuffer( pSD->mp_D3DDevice,
																pObject->l_NbPoints * stride,
																D3DUSAGE_WRITEONLY,
																FVF,
																D3DPOOL_DEFAULT,
																&pObject->pVB_Normal ) ) )
			{
				assert(FALSE);
			}
	*/
			// fill Tangente VB
			IDirect3DVertexBuffer8_Lock( pObject->pVB_Tangente, 0, 0, (void *) &pData, 0 );
			memcpy( pData, pObject->CotangantesU, pObject->l_NbPoints * stride );
			IDirect3DVertexBuffer8_Unlock( pObject->pVB_Tangente );

		}
/*		if ( !pObject->pVB_Binormales )
		{
			FVF = D3DFVF_XYZ;
			stride = D3DXGetFVFVertexSize( FVF );

			pObject->pVB_Binormales= Gx8_VertexBuffer_Allocate( pObject->l_NbPoints, stride ); 
			assert( pObject->pVB_Binormales != NULL );

			// fill Binormale VB
			IDirect3DVertexBuffer8_Lock( pObject->pVB_Binormales, 0, 0, (void *) &pData, 0 );
			memcpy( pData, pObject->Binormales, pObject->l_NbPoints * stride );
			IDirect3DVertexBuffer8_Unlock( pObject->pVB_Binormales );

		}*/
}

	// UV VBs
	if ( ( !pObject->pVBlist_UV ) && ( pObject->l_NbUVs ) )
	{
		int		xElem;
		void	* pData[ MAX_UV_BUFFER_NUMBER ];		// Francy: I can find something better...

		// alloc VBlist with size 1 (only 1 element)
		pObject->UVbufferNumber = 1;
		pObject->pVBlist_UV = MEM_p_Alloc( sizeof(IDirect3DVertexBuffer8 * ) * pObject->UVbufferNumber );

		// create first UV VB
		FVF = D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2;
		stride = D3DXGetFVFVertexSize( FVF );
        pObject->pVBlist_UV[0] = Gx8_VertexBuffer_Allocate( pObject->l_NbPoints, stride ); 
        assert( pObject->pVB_Position != NULL );
        /* MAX
		IDirect3DDevice8_CreateVertexBuffer( pSD->mp_D3DDevice,
											pObject->l_NbPoints * stride,
											D3DUSAGE_WRITEONLY,
											FVF,
											D3DPOOL_DEFAULT,
											&pObject->pVBlist_UV[0] );
        */
		// lock first UV VB
		IDirect3DVertexBuffer8_Lock( pObject->pVBlist_UV[0], 0, 0, (void *) &pData[0], 0 );

		// fill UV VB with MAX FLOAT ( = free/uninitialized )
		{
			float	* pDest = (float *) pData[0];
			float	* pEnd = pDest + ( pObject->l_NbPoints * 2 );

			for ( ; pDest < pEnd; pDest++ )
				* pDest = FLT_MAX;	// free/uninitialized 
		}

		// fill UV VB with UV of every strip
		for ( xElem = 0; xElem < pObject->l_NbElements; xElem++)
		{
			GEO_tdst_ElementIndexedTriangles	* pElement = &pObject->dst_Element[xElem];

			// check that strips are present
			if ( pElement->pst_StripData )
			{
				ULONG	UVbufferIndex;
				ULONG	stripCounter;
				ULONG	vertexCounter;
				GEO_tdst_StripData	* pStripData = pElement->pst_StripData;
				
				// browse every strip
				for ( stripCounter = 0; stripCounter < pStripData->ulStripNumber; stripCounter++)
				{
					GEO_tdst_OneStrip	* pOneStrip = &pStripData->pStripList[ stripCounter ];
					BOOL	foundCompatibleUVbuffer = false;

					// search a compatible UV buffer
					for ( UVbufferIndex = 0; UVbufferIndex < pObject->UVbufferNumber; UVbufferIndex++)
					{
						Gx8VertexComponent_UV	* pUVData = ( Gx8VertexComponent_UV * ) pData[ UVbufferIndex ];
						foundCompatibleUVbuffer = true;

						// check that current UV buffer is compatible with current strip
						for ( vertexCounter = 0; vertexCounter < pOneStrip->ulVertexNumber; vertexCounter++)
						{
							UINT vertexIndex = pOneStrip->pMinVertexDataList[ vertexCounter ].auw_Index;
							UINT uvIndex = pOneStrip->pMinVertexDataList[ vertexCounter ].auw_UV;
							GEO_tdst_UV	* pUV = &pObject->dst_UV[ uvIndex ];

							if ( ( pUVData[ vertexIndex ].U != FLT_MAX ) && ( pUVData[ vertexIndex ].U != pUV->fU ) )
							{
								foundCompatibleUVbuffer = false;
								break;
							}
							if ( ( pUVData[ vertexIndex ].V != FLT_MAX ) && ( pUVData[ vertexIndex ].V != pUV->fV ) )
							{
								foundCompatibleUVbuffer = false;
								break;
							}
						}

						// exit current loop if UVbuffer found
						if ( foundCompatibleUVbuffer )
							break;
					}

					if ( foundCompatibleUVbuffer )
					{
						Gx8VertexComponent_UV	* pUVData = ( Gx8VertexComponent_UV * ) pData[ UVbufferIndex ];

							// fill current UV buffer with current strip UV
						for ( vertexCounter = 0; vertexCounter < pOneStrip->ulVertexNumber; vertexCounter++)
						{
							UINT vertexIndex = pOneStrip->pMinVertexDataList[ vertexCounter ].auw_Index;
							UINT uvIndex = pOneStrip->pMinVertexDataList[ vertexCounter ].auw_UV;
							GEO_tdst_UV	* pUV = &pObject->dst_UV[ uvIndex ];

 							pUVData[ vertexIndex ].U = pUV->fU;
							pUVData[ vertexIndex ].V = pUV->fV;
						}

						pOneStrip->UVbufferIndex = UVbufferIndex;
					}
					else
					{
						// realloc VBlist with size 1 (only 1 element)
						pObject->UVbufferNumber ++;
						pObject->pVBlist_UV = MEM_p_Realloc( pObject->pVBlist_UV, sizeof(IDirect3DVertexBuffer8 * ) * pObject->UVbufferNumber );
						UVbufferIndex = pObject->UVbufferNumber - 1;
						if ( UVbufferIndex >= MAX_UV_BUFFER_NUMBER )
							assert(FALSE);		// size of auxiliary array pData is MAX_UV_BUFFER_NUMBER

						// create UV VB
						FVF = D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2;
						stride = D3DXGetFVFVertexSize( FVF );
                        pObject->pVBlist_UV[ UVbufferIndex ] = Gx8_VertexBuffer_Allocate( pObject->l_NbPoints, stride );
                        assert( pObject->pVBlist_UV[ UVbufferIndex ] != NULL );
                        /* MAX
						IDirect3DDevice8_CreateVertexBuffer( pSD->mp_D3DDevice,
															pObject->l_NbPoints * stride,
															D3DUSAGE_WRITEONLY,
															FVF,
															D3DPOOL_DEFAULT,
															&pObject->pVBlist_UV[ UVbufferIndex ] );
                        */
						// lock added UV VB
						IDirect3DVertexBuffer8_Lock( pObject->pVBlist_UV[ UVbufferIndex ], 0, 0, (void *) &pData[ UVbufferIndex ], 0 );

						// fill UV VB with MAX FLOAT ( = free/uninitialized )
						{
							float	* pDest = (float *) pData[ UVbufferIndex ];
							float	* pEnd = pDest + ( pObject->l_NbPoints * 2 );

							for ( ; pDest < pEnd; pDest++ )
								* pDest = FLT_MAX;	// free/uninitialized 
						}

						{
							Gx8VertexComponent_UV	* pUVData = ( Gx8VertexComponent_UV * ) pData[ UVbufferIndex ];

								// fill current UV buffer with current strip UV
							for ( vertexCounter = 0; vertexCounter < pOneStrip->ulVertexNumber; vertexCounter++)
							{
								UINT vertexIndex = pOneStrip->pMinVertexDataList[ vertexCounter ].auw_Index;
								UINT uvIndex = pOneStrip->pMinVertexDataList[ vertexCounter ].auw_UV;
								GEO_tdst_UV	* pUV = &pObject->dst_UV[ uvIndex ];

 								pUVData[ vertexIndex ].U = pUV->fU;
								pUVData[ vertexIndex ].V = pUV->fV;
							}

							pOneStrip->UVbufferIndex = UVbufferIndex;
						}
					}
				}
			}
		}

		{
			ULONG	UVbufferIndex;

			// unlock every UV VB
			for ( UVbufferIndex = 0; UVbufferIndex < pObject->UVbufferNumber; UVbufferIndex++ )
				IDirect3DVertexBuffer8_Unlock( pObject->pVBlist_UV[ UVbufferIndex ] );

			// FRANCY: detect number and size of added UV buffers
			if ( pObject->UVbufferNumber > 0 )
			{
				numberOfAddUVbuffer += pObject->UVbufferNumber - 1;
				numberOfAddedUVBytes += ( pObject->UVbufferNumber - 1 ) * ( pObject->l_NbPoints * stride );
			}
		}		

	}

	// IB
	if ( !pObject->pIB )
	{
		int		xElem;
		UINT	size = 0;

		// compute Index Buffer size
		for ( xElem = 0; xElem < pObject->l_NbElements; xElem++)
		{
			GEO_tdst_ElementIndexedTriangles	* pElement = &pObject->dst_Element[xElem];

			// check that strips are present
			if ( pElement->pst_StripData )
			{
				UINT	stripCounter;
				UINT	currUVbuffer;
				UINT	drawStripCounter = 0;
				GEO_tdst_StripData	* pStripData = pElement->pst_StripData;
				
				for ( currUVbuffer = 0; currUVbuffer < pObject->UVbufferNumber; currUVbuffer++)
				{
					UINT	startingIndex = size;

					for ( stripCounter = 0; stripCounter < pStripData->ulStripNumber; stripCounter++)
					{
						GEO_tdst_OneStrip	* pOneStrip = &pStripData->pStripList[ stripCounter ];

						// consider only strips associated to current UV Buffer
						if ( pOneStrip->UVbufferIndex != currUVbuffer )
							continue;

						// Add degenerate triangle (before every strip but the the first)
						if ( startingIndex != size )
						{
							size += 2;

							// add a third point in order to have every strip starting from odd vertex index (even triangles use reversed culling)
							if ( (size - startingIndex) & 1 )
								size ++;
						}

						// increase IB size
						size += pStripData->pStripList[ stripCounter ].ulVertexNumber;
					}
					
					if ( size - startingIndex > 0 )
					{
						// one Draw Strip per UV buffer with more than 0 vertices
						drawStripCounter ++;	
					}
				}

				// save the number of draw strip for current element
				pStripData->drawStripNumber = drawStripCounter;
				assert( pStripData->drawStripNumber <= pObject->UVbufferNumber );
			}
		}

		if ( size > 0 )
		{
			UINT	currIndex = 0;
			USHORT	* pDataIB;
			
			// generate IB
			if ( FAILED( IDirect3DDevice8_CreateIndexBuffer( pSD->mp_D3DDevice,
																size * sizeof(USHORT),		// number of indexes * sizeof(USHORT)
																D3DUSAGE_WRITEONLY,
																D3DFMT_INDEX16,
																D3DPOOL_DEFAULT,
																&pObject->pIB ) ) )
			{
				assert(FALSE);
			}
			
			// fill IB
			IDirect3DIndexBuffer8_Lock( pObject->pIB, 0, 0, (void *) &pDataIB, 0 );
			for ( xElem = 0; xElem < pObject->l_NbElements; xElem++)
			{
				GEO_tdst_ElementIndexedTriangles	* pElement = &pObject->dst_Element[xElem];

				// check that strips are present
				if ( pElement->pst_StripData )
				{
					UINT	stripCounter;
					UINT	vertexCounter;
					UINT	currUVbuffer;
					UINT	currDrawStrip = 0;
					GEO_tdst_StripData	* pStripData = pElement->pst_StripData;

					// Alloc Draw Strip List
					assert( pStripData->drawStripNumber > 0 );
					assert( pStripData->drawStripNumber <= 50 );
					pStripData->pDrawStripList = MEM_p_Alloc( sizeof(GEO_tdst_DrawStrip) * pStripData->drawStripNumber );

					for ( currUVbuffer = 0; ( currUVbuffer < pObject->UVbufferNumber ) && ( currDrawStrip < pStripData->drawStripNumber ); currUVbuffer++)
					{
						// save starting index in IB for current DrawStrip
						pStripData->pDrawStripList[currDrawStrip].startingIndex = currIndex;

                        for ( stripCounter = 0; stripCounter < pStripData->ulStripNumber; stripCounter++)
						{
							GEO_tdst_OneStrip	* pOneStrip = &pStripData->pStripList[ stripCounter ];
							
							assert( pOneStrip->UVbufferIndex < pObject->UVbufferNumber );
							// consider only strips associated to current UV Buffer
							if ( pOneStrip->UVbufferIndex != currUVbuffer )
								continue;

							if ( pStripData->pDrawStripList[currDrawStrip].startingIndex == currIndex )
							{
								// save UV buffer index in Draw Strip only once ( the first strip )
								pStripData->pDrawStripList[currDrawStrip].UVbufferIndex = currUVbuffer;
							}
							else	// Add degenerate triangle (before every strip but the the first)
							{
								// duplicate last vertex of previous strip
								*( pDataIB ) = *( pDataIB - 1 );
								pDataIB ++;
								// duplicate first vertex of current strip
								*( pDataIB ) = pOneStrip->pMinVertexDataList[ 0 ].auw_Index;
								pDataIB ++;
								currIndex += 2;

								// add a third point in order to have every strip starting from odd vertex index (even triangles use reversed culling)
								if ( (currIndex - pStripData->pDrawStripList[currDrawStrip].startingIndex) & 1 )
								{
									// duplicate first vertex of current strip
									*( pDataIB ) = pOneStrip->pMinVertexDataList[ 0 ].auw_Index;
									pDataIB ++;
									currIndex ++;
								}
							}

							// Add every vertex of current strip
							for ( vertexCounter = 0; vertexCounter < pOneStrip->ulVertexNumber; vertexCounter++)
							{
								*( pDataIB ) = pOneStrip->pMinVertexDataList[ vertexCounter ].auw_Index;
								pDataIB ++;
							}

							// increase index in IB
							currIndex += pOneStrip->ulVertexNumber;	
							assert( currIndex <= size );
						}

						// if vertex number for current DrawStrip > 0
						if ( currIndex - pStripData->pDrawStripList[currDrawStrip].startingIndex > 0 )
						{
							// save vertex number for current DrawStrip
							pStripData->pDrawStripList[currDrawStrip].vertexNumber = currIndex - pStripData->pDrawStripList[currDrawStrip].startingIndex;
							// increase current Draw Strip
							currDrawStrip ++;
						}
					}
					assert( currDrawStrip == pStripData->drawStripNumber );
				}
			}
			IDirect3DIndexBuffer8_Unlock( pObject->pIB );
			assert( currIndex == size );
		}			
	}



/*
	// check if object has colors
	if ( pObject->dul_PointColors )
	{
		// create color VB
		FVF = D3DFVF_DIFFUSE;
		stride = D3DXGetFVFVertexSize( FVF );
		IDirect3DDevice8_CreateVertexBuffer( pSD->mp_D3DDevice,
											pObject->l_NbPoints * stride,
											D3DUSAGE_WRITEONLY,
											FVF,
											D3DPOOL_DEFAULT,
											&pObject->pVB_Color,
											NULL );
		// fill color VB
		IDirect3DVertexBuffer8_Lock( pObject->pVB_Color, 0, 0, &pData, 0 );
		memcpy( pData, pObject->dul_PointColors, pObject->l_NbPoints * stride );
//		memset( pData, 150, pObject->l_NbPoints * stride );
		IDirect3DVertexBuffer8_Unlock( pObject->pVB_Color );
	}
*/

}