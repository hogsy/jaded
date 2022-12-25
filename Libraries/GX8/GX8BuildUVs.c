#include "GDInterface/GDInterface.h"
#include "Gx8BuildUVs.h"
#include <BASe/MEMory/MEM.h>
#include <assert.h>

#include "Gx8init.h"
#include "Gx8AddInfo.h"
#include "Gx8color.h"
#include "RASter/Gx8_CheatFlags.h"
#include "Gx8ShadowBuffer.h"
#include <d3dx8.h>


void *pCurrentGameObject;

// Various things that we'll have to take care of someday to improve performances and/or functionalities.
#pragma message("WARN: Gx8 engine does not use GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ulColorXor and ulColorOr")

extern Gx8_tdst_SpecificData	*p_gGx8SpecificData;

#if defined(ALIGNED_VERTEX)
#error The algorithm supposes that GEO_Vertex is defined as MATH_tdst_Vector. If you changed that you'll have to modify this file.
#endif

#define C_MAX_ELEMENTS_PER_OBJECT 20
#define C_lMaxVertexPerObject 10000

// STATIC FUNCTIONS
static int Gx8_BuildBuffersForElements (GEO_tdst_Object		*p_stObject,
                                        ULONG               *dul_VertexColors);

static void Gx8_BuildVBConstructionList(GEO_tdst_Object                  *p_stObject,
                                         GEO_tdst_ElementIndexedTriangles *p_stElement,
                                         unsigned short                   *p_stIndicesToFill);

static int Gx8_AddVB(GEO_tdst_Object	*p_stObject, 
	                 GEO_tdst_ElementIndexedTriangles *p_stElement,
					 ULONG				*dul_VertexColors,
					 int				iIndex,
					 int                *pSize);

bool GetCastingShadows(void);


/******************/
/* Implementation */
/******************/

int Gx8_ComputeVertexBufferFriendlyDataForGeometricObject(GEO_tdst_Object *p_stObject,
                                                            ULONG           *dul_VertexColors)
{
	if ( dul_VertexColors )
	    return Gx8_BuildBuffersForElements( p_stObject, dul_VertexColors + 1 );

    return Gx8_BuildBuffersForElements(p_stObject, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//__forceinline 
void Gx8_FillVertexBufferForElementInObject(GEO_tdst_Object						* p_stObject,
							                GEO_tdst_ElementIndexedTriangles	* p_stElement,
                                            ULONG								* dul_VertexColors,
                                            int									iIndex,
                                            ULONG								ulDisplayInfo)
{
    float						* p_fVBData;
    GEO_Vertex					* _pst_Point = NULL;
    GEO_tdst_UV					* _pst_UV = NULL;
    ULONG						* pst_Color = NULL;
	int							bUpdateColors = FALSE;
	ULONG						ulOGLSetCol;
    Gx8_tdstVBData				* pVertexBufferData = NULL;
    LPDIRECT3DVERTEXBUFFER8		pVertexBuffer = NULL;
    DWORD						dwLockingFlags;
	Gx8_tdstMultipleVBList		* pMultipleVBList;
	Gx8_tdstMultipleVB			* pMultipleVB;
	int							bFreeBufferFound = FALSE;
	unsigned int				iCurrentVBIndex;
	int							iFVF;
	int							iStride;


	bool shadowPresent;
	
	//CARLONE
	/*CARLONE INTEGRATION int							counter;
	void                        *pToGameObject=pCurrentGameObject;
	*/
	int size;

//	int lastGoodFind;

	//Carlone...number of this frame
	//int thisFrameNumber=GDI_gpst_CurDD->ul_RenderingCounter;

    if ( (!p_stObject) || (!p_stElement) || (!p_stElement->pst_Gx8Add) )
        return;

	dwLockingFlags = 0;

	// get Multiple VB 
	pMultipleVBList = p_stElement->pst_Gx8Add->pMultipleVBList;
	pMultipleVB = &pMultipleVBList->pMultipleVB[iIndex];




	//////////////////////////////////////CARLONE...TO BE CHANGED...FIND THE VERTEX BUFFER FROM THE GAME OBJECT/////////////////////////////

/*NOT WORKING..TO BE CHEKED

	lastGoodFind=-1;

	for(counter=0;counter<pMultipleVB->iVBNumber;++counter)
	{
		if(pToGameObject==pMultipleVB->pToGameObject[counter])
		{	
			if(!IDirect3DVertexBuffer8_IsBusy(pMultipleVB->pVB[counter]))
				break;
			else
				lastGoodFind=counter;

		}
	}

	if(counter>=pMultipleVB->iVBNumber)
	{
		int size;
		iCurrentVBIndex = Gx8_AddVB( p_stObject, dul_VertexColors, iIndex,&size );
		pMultipleVB->pToGameObject[iCurrentVBIndex]=pToGameObject;

		if(lastGoodFind >=0 )
		{
			//Copy information from vertex buffer in this one new
			Gx8_CopyVertexBuffer(pMultipleVB->pVB[iCurrentVBIndex],pMultipleVB->pVB[lastGoodFind],size);
		}
		else
		{
			//First vertex buffer...i have to recompute everything
			if ( ulDisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer )
				ulDisplayInfo = GDI_Cul_DI_UpdateUV | GDI_Cul_DI_UpdateRLI | GDI_Cul_DI_UseSpecialVertexBuffer;
			else
				ulDisplayInfo = GDI_Cul_DI_UpdateUV | GDI_Cul_DI_UpdateRLI | GDI_Cul_DI_UseSpecialVertexBuffer | GDI_Cul_DI_UseOriginalPoints;
	
		}
	}
	else
	{
		iCurrentVBIndex=counter;
	}

*/


	
/*CARLONE INTEGRATION	for(counter=0;counter<pMultipleVB->iVBNumber;++counter)
	{
		if(pToGameObject==pMultipleVB->pToGameObject[counter])
		{	
			//if(!IDirect3DVertexBuffer8_IsBusy( pMultipleVB->pVB[ counter ]))
				break;
		}
	}


	if(counter>=pMultipleVB->iVBNumber)
	{
		int size;
		iCurrentVBIndex = Gx8_AddVB( p_stObject, dul_VertexColors, iIndex,&size );
		pMultipleVB->pToGameObject[iCurrentVBIndex]=pToGameObject;

		//Copy information from vertex buffer in this one new
		Gx8_CopyVertexBuffer(pMultipleVB->pVB[iCurrentVBIndex],pMultipleVB->pVB[iCurrentVBIndex-1],size);
		
	}
	else
	{
		iCurrentVBIndex=counter;
	}


	
	pVertexBuffer = pMultipleVB->pVB[ iCurrentVBIndex ];
	bFreeBufferFound= TRUE;
*/



	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// set starting Index

	shadowPresent=false;
	shadowPresent=GetCastingShadows();

	if(shadowPresent)
	{

		iCurrentVBIndex = 0;
			// search a ready buffer
		while ( ( iCurrentVBIndex < pMultipleVB->iVBNumber ) && (! bFreeBufferFound) )
		{
			// get VB			
			pVertexBufferData = &pMultipleVB->pVBData[ iCurrentVBIndex ];

			if ( ( pVertexBufferData->iFrameNumber == GDI_gpst_CurDD->ul_RenderingCounter ) &&
				( pVertexBufferData->pToGameObject == pCurrentGameObject ) )
			{	// non-busy VB found
				bFreeBufferFound = TRUE;
			}
			else
			{	// increase Index
				iCurrentVBIndex ++;
			}
		}

		// found a ready buffer
		if ( bFreeBufferFound)
		{
			ulDisplayInfo &= ~GDI_Cul_DI_UseSpecialVertexBuffer;

			pVertexBuffer = pVertexBufferData->pVB;
		}
		else
		{

			// set starting Index
			iCurrentVBIndex = 0;
				///////////////////////////////////////////////////////////////////////////////////
			while ( ( iCurrentVBIndex < pMultipleVB->iVBNumber ) && (! bFreeBufferFound) )
			{
				// get VB			
				pVertexBufferData = &pMultipleVB->pVBData[ iCurrentVBIndex ];
				pVertexBuffer = pVertexBufferData->pVB;

				if ( ! IDirect3DVertexBuffer8_IsBusy( pVertexBuffer ) )
				{	// non-busy VB found
					bFreeBufferFound = TRUE;
				}
				else
				{	// increase Index
					iCurrentVBIndex ++;
				}
			}
			
			if (! bFreeBufferFound)
			{
				// Add a new VB
				// iCurrentVBIndex is the index of the new buffer
				iCurrentVBIndex = Gx8_AddVB( p_stObject, p_stElement, dul_VertexColors, iIndex,&size );
				// pMultipleVB has been reallocated: re-get it
				pMultipleVB = &pMultipleVBList->pMultipleVB[iIndex];
				// get VB			
				pVertexBuffer = pMultipleVB->pVBData[ iCurrentVBIndex ].pVB;
				// force full update of the new buffer
				if ( ulDisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer )
					ulDisplayInfo = GDI_Cul_DI_UpdateUV | GDI_Cul_DI_UpdateRLI | GDI_Cul_DI_UseSpecialVertexBuffer;
				else
					ulDisplayInfo = GDI_Cul_DI_UpdateUV | GDI_Cul_DI_UpdateRLI | GDI_Cul_DI_UseSpecialVertexBuffer | GDI_Cul_DI_UseOriginalPoints;
			}
			
			// save GameObject and current frame
			pMultipleVB->pVBData[ iCurrentVBIndex ].iFrameNumber = GDI_gpst_CurDD->ul_RenderingCounter;
			pMultipleVB->pVBData[ iCurrentVBIndex ].pToGameObject = pCurrentGameObject;
		}
	}
	else
	{
		{
			// set starting Index
			iCurrentVBIndex = 0;
				///////////////////////////////////////////////////////////////////////////////////
			while ( ( iCurrentVBIndex < pMultipleVB->iVBNumber ) && (! bFreeBufferFound) )
			{
				// get VB			
				pVertexBufferData = &pMultipleVB->pVBData[ iCurrentVBIndex ];
				pVertexBuffer = pVertexBufferData->pVB;

				if ( ! IDirect3DVertexBuffer8_IsBusy( pVertexBuffer ) )
				{	// non-busy VB found
					bFreeBufferFound = TRUE;
				}
				else
				{	// increase Index
					iCurrentVBIndex ++;
				}
			}
			
			if (! bFreeBufferFound)
			{
				// Add a new VB
				// iCurrentVBIndex is the index of the new buffer
				iCurrentVBIndex = Gx8_AddVB( p_stObject, p_stElement, dul_VertexColors, iIndex,&size );
				// pMultipleVB has been reallocated: re-get it
				pMultipleVB = &pMultipleVBList->pMultipleVB[iIndex];
				// get VB			
				pVertexBuffer = pMultipleVB->pVBData[ iCurrentVBIndex ].pVB;
				// force full update of the new buffer
				if ( ulDisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer )
					ulDisplayInfo = GDI_Cul_DI_UpdateUV | GDI_Cul_DI_UpdateRLI | GDI_Cul_DI_UseSpecialVertexBuffer;
				else
					ulDisplayInfo = GDI_Cul_DI_UpdateUV | GDI_Cul_DI_UpdateRLI | GDI_Cul_DI_UseSpecialVertexBuffer | GDI_Cul_DI_UseOriginalPoints;
			}
			
			// save GameObject and current frame
			pMultipleVB->pVBData[ iCurrentVBIndex ].iFrameNumber = GDI_gpst_CurDD->ul_RenderingCounter;
			pMultipleVB->pVBData[ iCurrentVBIndex ].pToGameObject = pCurrentGameObject;
		}
	}


	// set Indexes in order to have it when drawing
	pMultipleVB->iVBIndex = iCurrentVBIndex;
	pMultipleVBList->iMVBIndex = iIndex;

	// get FVF
	iFVF = pMultipleVB->iVertexShader;
	// get Stride
	iStride = (pMultipleVB->iVertexStride) / sizeof(float);


		// set Point source if needed
    if (ulDisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer)
    {
		if (ulDisplayInfo & GDI_Cul_DI_UseOriginalPoints)
		{
			_pst_Point = p_stObject->dst_Point;
		}
		else
		{
			_pst_Point = GDI_gpst_CurDD_SPR.p_Current_Vertex_List;
		}
    }

		// set color source if needed
    if (ulDisplayInfo & GDI_Cul_DI_UpdateRLI)
    {
		bUpdateColors = TRUE;

        if (dul_VertexColors)
        {
            pst_Color = dul_VertexColors;
        }
        else if (p_stObject->dul_PointColors)
        {
            pst_Color = p_stObject->dul_PointColors + 1;
        }
		else
		{
            pst_Color = NULL;
		}
	}

		// set UV source if needed
	if (ulDisplayInfo & GDI_Cul_DI_UpdateUV)
	{
	    _pst_UV = p_stObject->dst_UV;
	}

		// Lock VB
    IDirect3DVertexBuffer8_Lock(pVertexBuffer, 0, 0, (BYTE**)&p_fVBData, dwLockingFlags);

		// fill VB
		// Selective update: it could be useful if we update only changed data (normals, colors...)
	{
		int			i;
	    float		* p_fVBDataStart;

		p_fVBDataStart = p_fVBData;

				// Add vertex coords to vertex buffer
		if (_pst_Point)
		{
			int vertexNumber = p_stElement->pst_Gx8Add->uiVertexCount;
			for (i = 0; i < vertexNumber; i++)
			{
				int VertexIndex = p_stElement->pst_Gx8Add->d_stVBConstructionList[i].auw_VertexIndex;
				memcpy(p_fVBData, &(_pst_Point[ VertexIndex ]), 3*sizeof(float));
				p_fVBData += iStride;
			}
		}
		// Increase starting offset (Points are always present in VB)
		p_fVBDataStart += 3;

				// Add vertex normals to vertex buffer
		p_fVBData = p_fVBDataStart;
		if (_pst_Point)		// NOTE: Normals are updated only when Points are updated !
		{
			int vertexNumber = p_stElement->pst_Gx8Add->uiVertexCount;
			for (i = 0; i < vertexNumber; i++)
			{
				int VertexIndex = p_stElement->pst_Gx8Add->d_stVBConstructionList[i].auw_VertexIndex;
				memcpy(p_fVBData, &(p_stObject->dst_PointNormal[ VertexIndex ]), 3*sizeof(float));
				p_fVBData += iStride;
			}
		}
		// Increase starting offset if VB has Normals
		if ( iFVF & D3DFVF_NORMAL )
		{
			p_fVBDataStart += 3;
		}

		// Add vertex colors to vertex buffer
		if (bUpdateColors)
		{
			p_fVBData = p_fVBDataStart;
			if (pst_Color)
			{
				int vertexNumber = p_stElement->pst_Gx8Add->uiVertexCount;
				for (i = 0; i < vertexNumber; i++)
				{
					int VertexIndex = p_stElement->pst_Gx8Add->d_stVBConstructionList[i].auw_VertexIndex;
					ulOGLSetCol = pst_Color[VertexIndex];
					*(DWORD *)p_fVBData = Gx8_M_ConvertColor(ulOGLSetCol);
					p_fVBData += iStride;
				}
			}
			else
			{	// use black if no source color is defined
				DWORD black = Gx8_M_ConvertColor(0xff000000);
				int vertexNumber = p_stElement->pst_Gx8Add->uiVertexCount;
				for (i = 0; i < vertexNumber; i++)
				{
					*(DWORD *)p_fVBData = black;
					p_fVBData += iStride;
				}
			}
		}
		// Increase starting offset (Colors are always present in VB)
		p_fVBDataStart ++;

		// Add UV coords to vertex buffer
		if (p_stObject->l_NbUVs)
		{
			int vertexNumber = p_stElement->pst_Gx8Add->uiVertexCount;
			p_fVBData = p_fVBDataStart;
			for (i = 0; i < vertexNumber; i++)
			{
				int UVIndex = p_stElement->pst_Gx8Add->d_stVBConstructionList[i].auw_UVIndex;
				memcpy(p_fVBData, &(p_stObject->dst_UV[UVIndex]), 2*sizeof(float));
				p_fVBData += iStride;
			}
		}
	}

		// Unlock VB
    IDirect3DVertexBuffer8_Unlock(pVertexBuffer);

}

	// create and reset Gx8AddInfo
void Gx8_CreateAddInfo(GEO_tdst_Object	*p_stObject)
{
	int xElem;
    Gx8_tdstAddInfo *p_stAddInfo;

	// Init AddInfo for each element
	for (xElem=0; xElem<p_stObject->l_NbElements; xElem++)
	{
        GEO_tdst_ElementIndexedTriangles *p_stElement = &p_stObject->dst_Element[xElem];

		// Alloc AddInfo
		p_stAddInfo = MEM_p_Alloc(sizeof(Gx8_tdstAddInfo));
		assert(p_stAddInfo != NULL);
		memset(p_stAddInfo, 0, sizeof(Gx8_tdstAddInfo));
		// Set AddInfo
		p_stElement->pst_Gx8Add = p_stAddInfo;
	}
}

	// create and reset SpriteVBData only for the first element
void Gx8_CreateSpriteVBData(GEO_tdst_Object	*p_stObject)
{
    Gx8_tdstSpriteVBData *p_SVB;
    GEO_tdst_ElementIndexedTriangles *p_stElement = p_stObject->dst_Element;

	// check element & AddInfo
	assert( p_stElement );
	assert( p_stElement->pst_Gx8Add );

	// Alloc SpriteVBData
	p_SVB = MEM_p_Alloc( sizeof(Gx8_tdstSpriteVBData) * GX8_SPG_LEVEL_NUMBER );
	assert( p_SVB );
	memset( p_SVB, 0, sizeof(Gx8_tdstSpriteVBData) * GX8_SPG_LEVEL_NUMBER );

	// Set SpriteVBData
	p_stElement->pst_Gx8Add->pSpriteVBData = p_SVB;
}

	// init Gx8AddInfo with Index Buffers
void Gx8_InitIndexBuffer(GEO_tdst_Object	*p_stObject)
{
	int xElem;

	for (xElem=0; xElem<p_stObject->l_NbElements; xElem++)
	{
	    unsigned short *p_usIBData;
	    LPDIRECT3DINDEXBUFFER8  p_xIndexBuffer;
        GEO_tdst_ElementIndexedTriangles *p_stElement = &p_stObject->dst_Element[xElem];

		// Create Index Buffer (IB)
		IDirect3DDevice8_CreateIndexBuffer(p_gGx8SpecificData->mp_D3DDevice,
											p_stElement->l_NbTriangles * 3 * sizeof(unsigned short),
											D3DUSAGE_WRITEONLY,
											D3DFMT_INDEX16,
											D3DPOOL_MANAGED,
											&p_xIndexBuffer);

		// Fill IB and Gx8AddInfo 
		IDirect3DIndexBuffer8_Lock(p_xIndexBuffer, 0, 0, (BYTE**)&p_usIBData, 0);
        p_stElement->pst_Gx8Add->uiVertexOffset = 0;
        p_stElement->pst_Gx8Add->lpd3dibIndexBuffer = p_xIndexBuffer;
        p_stElement->pst_Gx8Add->uiIndexOffset = 0;
        p_stElement->pst_Gx8Add->iIsDynamic = FALSE;
    
        // Compute data needed for the VB creation, and fill the IB
        Gx8_BuildVBConstructionList(p_stObject, p_stElement, p_usIBData);

		// Unlock IB
	    IDirect3DIndexBuffer8_Unlock(p_xIndexBuffer);
		p_xIndexBuffer = NULL;
    }

/*
	int xElem;
    UINT ulTotalVerticesCount;
    UINT ulTotalFacesCount;
    LPDIRECT3DINDEXBUFFER8  p_xIndexBuffer;
    unsigned short *p_usIBData;

    // Count the needed sizes for the index buffer
    ulTotalFacesCount = 0;
	for (xElem=0; xElem<p_stObject->l_NbElements; xElem++)
	{
        ulTotalFacesCount += p_stObject->dst_Element[xElem].l_NbTriangles;
    }

	// Create Index Buffer (IB)
    IDirect3DDevice8_CreateIndexBuffer(p_gGx8SpecificData->mp_D3DDevice,
                                        2 * ulTotalFacesCount * 3,
                                        D3DUSAGE_WRITEONLY,
                                        D3DFMT_INDEX16,
                                        D3DPOOL_MANAGED,
                                        &p_xIndexBuffer);

	// Fill IB and Gx8AddInfo 
    IDirect3DIndexBuffer8_Lock(p_xIndexBuffer, 0, 0, (BYTE**)&p_usIBData, 0);
    ulTotalVerticesCount = 0;
    ulTotalFacesCount = 0;
	for (xElem=0; xElem<p_stObject->l_NbElements; xElem++)
	{
        GEO_tdst_ElementIndexedTriangles *p_stElement = &p_stObject->dst_Element[xElem];

        p_stElement->pst_Gx8Add->uiVertexOffset = ulTotalVerticesCount;
        p_stElement->pst_Gx8Add->lpd3dibIndexBuffer = p_xIndexBuffer;
        p_stElement->pst_Gx8Add->uiIndexOffset = 3 * ulTotalFacesCount;
        p_stElement->pst_Gx8Add->iIsDynamic = FALSE;
    
        // Compute data needed for the VB creation, and fill the IB
        Gx8_BuildVBConstructionList(p_stObject, p_stElement, p_usIBData);

		// increase counters
        ulTotalVerticesCount += p_stElement->pst_Gx8Add->uiVertexCount;
        ulTotalFacesCount += p_stElement->l_NbTriangles;
        p_usIBData += 3 * p_stElement->l_NbTriangles;
    }
    IDirect3DIndexBuffer8_Unlock(p_xIndexBuffer);
*/
}


	// add a VB to an existing Multiple VB
	// return the VB index
int Gx8_AddVB(GEO_tdst_Object	*p_stObject, 
               GEO_tdst_ElementIndexedTriangles *p_stElement,
			   ULONG			*dul_VertexColors,
			   int				iIndex, 
			   int              *pSize )
{
    LPDIRECT3DVERTEXBUFFER8		pVertexBuffer;
    D3DPOOL ePool;
    DWORD dwUsage;
    DWORD dwFVF;
    int iVertexStride;
	Gx8_tdstMultipleVB			* pMultipleVB;
	Gx8_tdstMultipleVBList		* pMultipleVBList;

	// get pMultipleVBList
	pMultipleVBList = p_stElement->pst_Gx8Add->pMultipleVBList;
	// make sure that pMultipleVB points to the Multiple VB to fill
	pMultipleVB = &pMultipleVBList->pMultipleVB[ iIndex ];


	// compute Usage and Pool
#if !defined(_XBOX)
    // PC versions would have to fill in Usage and Pool
    //if (Dynamic)  TODO
    if (1)
    {
        dwUsage = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
        ePool = D3DPOOL_DEFAULT;
    }
    else
#endif _XBOX
    {
        dwUsage = D3DUSAGE_WRITEONLY;
        ePool = D3DPOOL_MANAGED;
    }

	// calculate FVF
	// every VB has position and diffuse
	dwFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

	if (p_stObject->dst_PointNormal != NULL)
        dwFVF |= D3DFVF_NORMAL;

    if (p_stObject->l_NbUVs != 0)
        dwFVF |= D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(2);
    else
        dwFVF |= D3DFVF_TEX0;

	// calculate Stride
	iVertexStride = D3DXGetFVFVertexSize(dwFVF);

    // Create the Vertex Buffer (VB)
    if ( D3D_OK != IDirect3DDevice8_CreateVertexBuffer(p_gGx8SpecificData->mp_D3DDevice,
                                        p_stElement->pst_Gx8Add->uiVertexCount * iVertexStride,
                                        dwUsage,
                                        dwFVF,
                                        ePool,
                                        &pVertexBuffer) )
	{	// Out of contiguous memory (VB)
		assert( false );
	}


	//Output the size
	(*pSize)=(p_stElement->pst_Gx8Add->uiVertexCount * iVertexStride);

#pragma message("TODO: Gx8_BuildBuffersForElements: find something better than multiple reallocs...")

	// allocate (or reallocate) Vertex Buffers
	if ( pMultipleVB->iVBNumber == 0 )
	{
		pMultipleVB->iVBNumber = 1;
		pMultipleVB->pVBData = MEM_p_Alloc( sizeof(Gx8_tdstVBData) );
	}
	else
	{
		// check that Stride is the same than other VB
		assert( iVertexStride == pMultipleVB->iVertexStride);

		pMultipleVB->iVBNumber ++;

		if(pMultipleVB->iVBNumber>10)
		{
			int boom;
			boom=1000;
		}


		pMultipleVB->pVBData = MEM_p_Realloc( pMultipleVB->pVBData, pMultipleVB->iVBNumber * sizeof(Gx8_tdstVBData) );
	}
	// out of memory check
	assert(pMultipleVB != NULL);
	// set VB
	pMultipleVB->pVBData[ pMultipleVB->iVBNumber - 1 ].pVB = pVertexBuffer;
	// set Stride and Shader
	pMultipleVB->iVertexStride = iVertexStride;
	pMultipleVB->iVertexShader = dwFVF;

	return pMultipleVB->iVBNumber-1;
}


	// create and fill with the first element a Multiple VB
	// returns the index of the created Multiple VB
int Gx8_CreateMultipleVB(GEO_tdst_Object	*p_stObject, 
						 ULONG				*dul_VertexColors)
{
	int xElem;
    int iIndex = 0;
	Gx8_tdstMultipleVB			* pMultipleVB;
	Gx8_tdstMultipleVBList		* pMultipleVBList;
	int fakeSize;

	for (xElem=0; xElem<p_stObject->l_NbElements; xElem++)
	{
        GEO_tdst_ElementIndexedTriangles *p_stElement = &p_stObject->dst_Element[xElem];

		// allocate (or reallocate) pMultipleVBList if needed
		if ( !p_stElement->pst_Gx8Add->pMultipleVBList )
		{
			// alloc Multiple VB List
			pMultipleVBList = MEM_p_Alloc( sizeof( Gx8_tdstMultipleVBList ) );
			// out of memory check
			assert(pMultipleVBList != NULL);

			// compute size
			if (dul_VertexColors != NULL)
				pMultipleVBList->iMVBNumber = 2;		// allocate 2 elements (0 = GEO_tdst_Object, 1 = First Visu)
			else
				pMultipleVBList->iMVBNumber = 1;		// allocate only 1 element (0 = GEO_tdst_Object)
			// compute return value
			iIndex = pMultipleVBList->iMVBNumber - 1;
			// allocate
			pMultipleVBList->pMultipleVB = MEM_p_Alloc( pMultipleVBList->iMVBNumber * sizeof( Gx8_tdstMultipleVB ) );
			// out of memory check
			assert(pMultipleVBList->pMultipleVB != NULL);
			// reset allocated memory
			memset(pMultipleVBList->pMultipleVB, 0, pMultipleVBList->iMVBNumber * sizeof( Gx8_tdstMultipleVB ));
			
			p_stElement->pst_Gx8Add->pMultipleVBList = pMultipleVBList;
		}
		else
		{		// pMultipleVBList already allocated
			if (dul_VertexColors == NULL)
			{
				// Index 0 means the VB associated to GEO_tdst_Object->dulPointColors
				iIndex = 0;

				// return if object has yet a valid MultipleVB for Index 0 
				if ( p_stElement->pst_Gx8Add->pMultipleVBList->pMultipleVB[0].iVertexShader )
					return iIndex;
			}
			else
			{
				// get pMultipleVBList
				pMultipleVBList = p_stElement->pst_Gx8Add->pMultipleVBList;
				// get pMultipleVB
				pMultipleVB = pMultipleVBList->pMultipleVB;
				// increase MultipleVB List Size
				pMultipleVBList->iMVBNumber ++;
				// realloc
				pMultipleVB = MEM_p_Realloc( pMultipleVB, pMultipleVBList->iMVBNumber * sizeof( Gx8_tdstMultipleVB ) );
				// out of memory check
				assert(pMultipleVBList->pMultipleVB != NULL);
				// reset allocated memory
				memset(&pMultipleVB[ pMultipleVBList->iMVBNumber - 1 ], 0, sizeof( Gx8_tdstMultipleVB ));
				// compute return value
				iIndex = pMultipleVBList->iMVBNumber - 1;
				// set pMultipleVB
				pMultipleVBList->pMultipleVB = pMultipleVB;
			}
		}



		if ( p_stElement->pst_Gx8Add->uiVertexCount )
		{

			// create the first VB
			Gx8_AddVB( p_stObject, p_stElement, dul_VertexColors, iIndex,&fakeSize );

			// Fill the VB
			Gx8_FillVertexBufferForElementInObject(p_stObject, p_stElement, dul_VertexColors, iIndex, 
												GDI_Cul_DI_UpdateUV | GDI_Cul_DI_UpdateRLI | GDI_Cul_DI_UseSpecialVertexBuffer | GDI_Cul_DI_UseOriginalPoints);
		}
	}

	return iIndex;
}


	// returns Multiple VB index to use
int Gx8_BuildBuffersForElements (GEO_tdst_Object	*p_stObject,
                                   ULONG            *dul_VertexColors)
{
	// To be sure that Normals have been allocated and calculated !
	GEO_UseNormals(p_stObject);

    if ( ! p_stObject->dst_Element[0].pst_Gx8Add )
    {		// Create Gx8AddInfo if needed (for each element)
		Gx8_CreateAddInfo( p_stObject );
	}

	if (p_stObject->dst_Element[0].pst_Gx8Add->lpd3dibIndexBuffer == NULL)
    {		//	Init Index Buffer in Gx8AddInfo (for each element)
		Gx8_InitIndexBuffer( p_stObject );
	}

	return Gx8_CreateMultipleVB( p_stObject, dul_VertexColors );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned short MLT_tdusUsedUVListNodeID;
typedef struct MLT_tdstUsedUVListNode_
{
    unsigned short usVertexIndexInObject; // where's the vertex in the object
    unsigned short usUVIndex;             // where's the corresponding UV in the object
    unsigned short usVertexIndexInVB;     // where to store the constructed vertex in the VB
    MLT_tdusUsedUVListNodeID xNext;
} MLT_tdstUsedUVListNode;

#define C_lMaxVertexPerVertexBuffer C_lMaxVertexPerObject
static MLT_tdstUsedUVListNode a_stUsedUVListNodePool[C_lMaxVertexPerVertexBuffer];
static MLT_tdusUsedUVListNodeID gs_xNextFreeIDInUsedEVList;

#define C_usUnknownUsedUVListNodeID ((MLT_tdusUsedUVListNodeID)-1)

#define MLT_M_p_stGetUsedUVListNodeFromID(xID) (&a_stUsedUVListNodePool[xID])
#define MLT_M_p_stGetNextUsedUVListNodeID(xID) (a_stUsedUVListNodePool[xID].xNext)

__inline void MLT_vInitUsedUVList(void)
{
    gs_xNextFreeIDInUsedEVList = 0;
}

__inline MLT_tdusUsedUVListNodeID MLT_p_stAllocNewUsedUVListNode(unsigned short usVertexIndexInObject,
                                                                unsigned short usUVIndex,
                                                                unsigned short usVertexIndexInVB,
                                                                MLT_tdusUsedUVListNodeID xNext)
{
    MLT_tdusUsedUVListNodeID xNewID = gs_xNextFreeIDInUsedEVList;
    MLT_tdstUsedUVListNode *p_stNewNode = &a_stUsedUVListNodePool[xNewID];

    assert(gs_xNextFreeIDInUsedEVList < C_lMaxVertexPerVertexBuffer);
    assert(gs_xNextFreeIDInUsedEVList != C_usUnknownUsedUVListNodeID);
    gs_xNextFreeIDInUsedEVList++;

    p_stNewNode->usVertexIndexInObject = usVertexIndexInObject;
    p_stNewNode->usUVIndex = usUVIndex;
    p_stNewNode->usVertexIndexInVB = usVertexIndexInVB;
    p_stNewNode->xNext = xNext;

    return xNewID;
}

void Gx8_BuildVBConstructionList(GEO_tdst_Object                  *p_stObject,
                                  GEO_tdst_ElementIndexedTriangles *p_stElement,
                                  unsigned short                   *p_stIndicesToFill)
{
    MLT_tdusUsedUVListNodeID a_xUsedUVArray[C_lMaxVertexPerObject];
    LONG lIndex;
    UINT uiVertexCount;

    assert(p_stObject->l_NbPoints <= C_lMaxVertexPerObject);
    MLT_vInitUsedUVList();
    memset(a_xUsedUVArray, C_usUnknownUsedUVListNodeID, p_stObject->l_NbPoints * sizeof(MLT_tdusUsedUVListNodeID));

    // Compute the list of indices to put in the index buffer
    // and count the number of vertices needed in the VB.
    // We have to find out how many time a given vertex of the objects uses a specific UV.
    uiVertexCount = 0;
    for (lIndex = 0; lIndex < p_stElement->l_NbTriangles; lIndex++)
    {
        char i;
        for (i = 0; i < 3; i++)
        {
            MLT_tdusUsedUVListNodeID xCurrentNodeID = a_xUsedUVArray[p_stElement->dst_Triangle[lIndex].auw_Index[i]];
            while ((xCurrentNodeID != C_usUnknownUsedUVListNodeID) &&
                   (MLT_M_p_stGetUsedUVListNodeFromID(xCurrentNodeID)->usUVIndex != p_stElement->dst_Triangle[lIndex].auw_UV[i]))
            {
                xCurrentNodeID = MLT_M_p_stGetNextUsedUVListNodeID(xCurrentNodeID);
            }
            if (xCurrentNodeID == C_usUnknownUsedUVListNodeID)
            {
                a_xUsedUVArray[p_stElement->dst_Triangle[lIndex].auw_Index[i]] =
                    MLT_p_stAllocNewUsedUVListNode(p_stElement->dst_Triangle[lIndex].auw_Index[i],
                                                   p_stElement->dst_Triangle[lIndex].auw_UV[i],
                                                   (unsigned short)uiVertexCount,
                                                   a_xUsedUVArray[p_stElement->dst_Triangle[lIndex].auw_Index[i]]);
                p_stIndicesToFill[3*lIndex+i] = (unsigned short) uiVertexCount;
                uiVertexCount++;
            }
            else
            {
                // The case already occured
                p_stIndicesToFill[3*lIndex+i] = MLT_M_p_stGetUsedUVListNodeFromID(xCurrentNodeID)->usVertexIndexInVB;
            }
        }
    }

    p_stElement->pst_Gx8Add->uiVertexCount = uiVertexCount;
    // Compute d_stVBConstructionList
	if (uiVertexCount>0)
		p_stElement->pst_Gx8Add->d_stVBConstructionList = MEM_p_Alloc(uiVertexCount * sizeof(Gx8_tdstVBConstructionList));
	else
		p_stElement->pst_Gx8Add->d_stVBConstructionList = NULL;
    for (lIndex = 0; lIndex < p_stObject->l_NbPoints; lIndex++)
    {
        MLT_tdusUsedUVListNodeID xCurrentNodeID = a_xUsedUVArray[lIndex];
        while (xCurrentNodeID != C_usUnknownUsedUVListNodeID)
        {
            MLT_tdstUsedUVListNode *p_stCurrentNode = MLT_M_p_stGetUsedUVListNodeFromID(xCurrentNodeID);
            Gx8_tdstVBConstructionList *p_stCurrentVertexInfo = &p_stElement->pst_Gx8Add->d_stVBConstructionList[p_stCurrentNode->usVertexIndexInVB];
            p_stCurrentVertexInfo->auw_VertexIndex = p_stCurrentNode->usVertexIndexInObject;
            p_stCurrentVertexInfo->auw_UVIndex= p_stCurrentNode->usUVIndex;

            xCurrentNodeID = MLT_M_p_stGetNextUsedUVListNodeID(xCurrentNodeID);
        }
    }
}

void Gx8_UpdateModifiedObject(GEO_tdst_Object *p_stObject,
			                  GEO_tdst_ElementIndexedTriangles *p_stElement,
                              ULONG           *dul_VertexColors,
                              int			   iIndex,
                              ULONG            ulDisplayInfo)
{

    if ( (!p_stObject) || (!p_stElement) || (!p_stElement->pst_Gx8Add) )
        return;

// VERIFY THAT THE VERTEX FORMAT IS THE SAME USED FOR THE VB CREATION
#if defined ( _DEBUG )
	{
		DWORD dwFVF;
		int iVertexStride;
		
		// every VB has position and diffuse
		dwFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

		if (p_stObject->dst_PointNormal != NULL)
		{
			dwFVF |= D3DFVF_NORMAL;
		}

		if (p_stObject->l_NbUVs != 0)
		{
			dwFVF |= D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(2);
		}
		else
		{
			dwFVF |= D3DFVF_TEX0;
		}
		// calculate current FVF stride
		iVertexStride = D3DXGetFVFVertexSize(dwFVF);

		// verify that the FVF stride is not changed
		assert(p_stElement->pst_Gx8Add->pMultipleVBList->pMultipleVB[ iIndex ].iVertexStride == iVertexStride);
	}
#endif // _DEBUG

	Gx8_FillVertexBufferForElementInObject(p_stObject,
											p_stElement,
                                            dul_VertexColors,
                                            iIndex,
                                            ulDisplayInfo);


		// destroy Sprite VB data if it is no more valid 
	if ( ulDisplayInfo & ( GDI_Cul_DI_UseSpecialVertexBuffer | GDI_Cul_DI_UpdateRLI ) )
	{
		// release Sprite VB Data
		HRESULT hr;
		int i;
		Gx8_tdstSpriteVBData	* pSpriteVBData = p_stObject->dst_Element[0].pst_Gx8Add->pSpriteVBData;

			// set updated in current frame
		p_stObject->dst_Element[0].pst_Gx8Add->uiUpdateHistory |= 0x00000001;

		if ( pSpriteVBData )
		{
			// for each level
			for ( i = 0; i < GX8_SPG_LEVEL_NUMBER; i++)
			{
					// release Sprite VB
				hr = IDirect3DVertexBuffer8_Release( pSpriteVBData[i].pSPG_VB );
				assert (hr==0);
			}

			// free VB list
			MEM_Free( pSpriteVBData );
			p_stObject->dst_Element[0].pst_Gx8Add->pSpriteVBData = NULL;
		}
	}
	

}



void Gx8_SetCurrentGameObject(void *pNextGO)
{
	pCurrentGameObject=pNextGO;
}

void Gx8_CopyVertexBuffer(LPDIRECT3DVERTEXBUFFER8 pDest,LPDIRECT3DVERTEXBUFFER8 pSource,int Size)
{


	BYTE *pDataSource;
	BYTE *pDataDest;

	IDirect3DVertexBuffer8_Lock(pSource, 0, 0, (BYTE**)&pDataSource, 0);
	IDirect3DVertexBuffer8_Lock(pDest, 0, 0, (BYTE**)&pDataDest, 0);


	memcpy(pDataDest,pDataSource,Size);

	IDirect3DVertexBuffer8_Unlock(pSource);
	IDirect3DVertexBuffer8_Unlock(pDest);

}
