/*$T MDFModifier_WATER3D.c GC! 1.081 06/09/00 09:00:51 */


/*$6
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "GDInterface/GDInterface.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_WATER3D.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEO_STRIP.h"

#include "BASe/BENch/BENch.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "GEOmetric/GEO_SKIN.h"

#ifdef _XENON_RENDER
#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeShader.h"
#include "XenonGraphics/XeSharedDefines.h"
#include "XenonGraphics/XeWaterManager.h"
#include "XenonGraphics/XeUtils.h"

#ifdef VIDEOCONSOLE_ENABLE
extern TBool NoWaterTurbulance;
#endif

#endif

#if defined(ACTIVE_EDITORS) || defined(_XENON_RENDER)
void MAT_DrawIndexedTriangle_MT(GDI_tdst_DisplayData				*pst_CurDD,	GEO_tdst_Object						*pst_Obj,	MAT_tdst_Material					*_pst_Material,	GEO_tdst_ElementIndexedTriangles	*pst_Element);
#endif

#define  MATH_Max(a, b)	(((a) > (b)) ? (a) : (b))
#define  MATH_Min(a, b)	(((a) < (b)) ? (a) : (b))

FLOAT WATER3D_fsec;

MATH_tdst_Vector        gpst_WaterNormal[(MAX_PATCH_SIZE*MAX_PATCH_SIZE)];	

FLOAT					TotalForceZ[(MAX_PATCH_SIZE*MAX_PATCH_SIZE)]; // max patch size

#ifdef ACTIVE_EDITORS
ULONG g_iWaterMemoryMax = 0;
#endif

#ifndef _FINAL
ULONG g_iWaterMemory=0;
#endif

#if defined(XML_CONV_TOOL)
extern ULONG gGaoWater3DMdfVersion;
MATH_tdst_Matrix matDummyMatrix;
#endif

// if USE_WATER3D_MEMORY is defined, use 128k for all the water being displayed (always the same amount o memory this way... no fluctuations)
#define	WATER3D_MEMORY_SIZE	(128*1024)

#ifdef USE_WATER3D_MEMORY

	INT						*gpst_WaterMemory = NULL;
	INT						*gpst_WaterMemory_Current = NULL;

	#ifndef _FINAL
	#endif

	void WATER3D_DefragmentMemory()
	{
		// defragmentation
		gpst_WaterMemory_Current = gpst_WaterMemory;

		while(((ULONG)gpst_WaterMemory_Current+fAbs(*gpst_WaterMemory_Current)+4) < ((ULONG)gpst_WaterMemory+WATER3D_MEMORY_SIZE))
		{
			if(*gpst_WaterMemory_Current>0 && gpst_WaterMemory_Current[((*gpst_WaterMemory_Current)>>2)+1]>0)
			{
				*gpst_WaterMemory_Current += gpst_WaterMemory_Current[((*gpst_WaterMemory_Current)>>2)+1]+4;
	#ifndef _FINAL_
				g_iWaterMemory -= 4;
	#endif				
			}
			else
				gpst_WaterMemory_Current += ((abs(*gpst_WaterMemory_Current)>>2) + 1);
		}

		gpst_WaterMemory_Current = gpst_WaterMemory;
	}

	void * WATER3D_Alloc(INT spaceNeeded)
	{	
		INT spaceTaken = ((((spaceNeeded-1)>>2)+1)<<2); // 4 bytes roundup
		
		if (((ULONG)gpst_WaterMemory_Current+spaceTaken) >= ((ULONG)gpst_WaterMemory+WATER3D_MEMORY_SIZE))
		{
			gpst_WaterMemory_Current = gpst_WaterMemory;
		}

		for(int i=0; i<2; i++)
		{
			while((((ULONG)gpst_WaterMemory_Current+spaceTaken+4) < ((ULONG)gpst_WaterMemory+WATER3D_MEMORY_SIZE)))
			{
				if(*gpst_WaterMemory_Current >= spaceTaken)
				{
					if(*gpst_WaterMemory_Current > spaceTaken)
					{
	#ifndef _FINAL_
						g_iWaterMemory -= 4; // no new block but 4 bytes are added later so remove...
	#endif							
						gpst_WaterMemory_Current[(spaceTaken>>2)+1] = *gpst_WaterMemory_Current - spaceTaken - 4;
					}
					*gpst_WaterMemory_Current = -spaceTaken;
					
	#ifndef _FINAL_
					g_iWaterMemory += spaceTaken+4;
	#endif			
					
					return((void*)&gpst_WaterMemory_Current[1]);
				}
				else
				{
					gpst_WaterMemory_Current += ((abs((*gpst_WaterMemory_Current))>>2) + 1);
				}
			}

			if(i==1)
				return NULL;

			WATER3D_DefragmentMemory();
		}
		
		return NULL;
	}

	void WATER3D_Free(void *pos)
	{
		if(!pos)
			return;

		gpst_WaterMemory_Current = (INT*)pos;
		gpst_WaterMemory_Current--;

		ERR_X_Assert((gpst_WaterMemory_Current>=gpst_WaterMemory) && (gpst_WaterMemory_Current<(gpst_WaterMemory+WATER3D_MEMORY_SIZE)));

		if(*gpst_WaterMemory_Current>0)
		{
			ERR_X_Warning(0, "WATER3D memory already freed", NULL);
		}
		else
		{
			*gpst_WaterMemory_Current = -(*gpst_WaterMemory_Current);
	#ifndef _FINAL_
			g_iWaterMemory -= *gpst_WaterMemory_Current;
	#endif	
		}
	}

	void WATER3D_PatchFree(WATER3D_tdst_Patches *p_Patch)
	{
		if(((ULONG)p_Patch->VZ > (ULONG)gpst_WaterMemory) && ((ULONG)p_Patch->VZ < ((ULONG)gpst_WaterMemory + WATER3D_MEMORY_SIZE)))
			WATER3D_Free(p_Patch->VZ);
		else if(p_Patch->VZ)
			MEM_FreeAlign(p_Patch->VZ);

		if(((ULONG)p_Patch->SZ > (ULONG)gpst_WaterMemory) && ((ULONG)p_Patch->SZ < ((ULONG)gpst_WaterMemory + WATER3D_MEMORY_SIZE)))
			WATER3D_Free(p_Patch->SZ);
		else if(p_Patch->SZ)
			MEM_FreeAlign(p_Patch->SZ);
		
		p_Patch->VZ = NULL;
		p_Patch->SZ = NULL;
	}

	void WATER3D_PatchAlloc(WATER3D_tdst_Modifier *p_Water3D, WATER3D_tdst_Patches *p_Patch)
	{
		if(!gpst_WaterMemory)
		{
			gpst_WaterMemory_Current = gpst_WaterMemory = (INT*)MEM_p_Alloc(WATER3D_MEMORY_SIZE);
			*gpst_WaterMemory = WATER3D_MEMORY_SIZE-4;
		}

		if(!p_Patch->SZ)
		{
			// velocity field alloc
			p_Patch->VZ = (void*)WATER3D_Alloc(p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT));
			if(!p_Patch->VZ) 
			{
				p_Patch->VZ = (void*)MEM_p_AllocAlign(p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT), 32);
				ERR_X_Warning("Water memory exceeded\n", NULL, NULL);
			}

			// position field alloc
			p_Patch->SZ = (void*)WATER3D_Alloc(p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT));
			if(!p_Patch->SZ)
			{
				p_Patch->SZ = (void*)MEM_p_AllocAlign(p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT), 32);
				ERR_X_Warning("Water memory exceeded\n", NULL, NULL);
			}


			memset(p_Patch->VZ, 0, p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT));
			memset(p_Patch->SZ, 0, p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT));
		}
	}

#endif

WATER3D_tdst_Modifier 	*p_FirstActiveWater3D = NULL;
WATER3D_tdst_Modifier 	*pCur3DWater=NULL;
WATER3D_tdst_Patches	*pCur3DWaterPatch=NULL;

// do not compute not display patches that are not in the field of view
BOOL OBJ_CullingOBBox( MATH_tdst_Vector	*_pst_LMin, MATH_tdst_Vector	*_pst_LMax,	MATH_tdst_Matrix	*_pst_GlobalObject,	CAM_tdst_Camera *_pst_Cam);

/*
=======================================================================================================================
=======================================================================================================================
*/
void WATER3D_CreateObject(WATER3D_tdst_Modifier *p_Water3D, WATER3D_tdst_Patches *p_Patch)
{	
#if !defined(XML_CONV_TOOL)
	OBJ_tdst_GameObject *_pst_GO = p_Water3D->p_GO;

	memset(&p_Patch->GEO_Object,0, sizeof(p_Patch->GEO_Object));

	GRO_Struct_Init(&p_Patch->GEO_Object.st_Id, GRO_Geometric);

//#ifdef ACTIVE_EDITORS
//	p_Patch->GEO_Object.flags.UseNormalsInEngine = 1;
//#endif	

	p_Patch->GEO_Object . l_NbPoints			= (p_Patch->GridWidth-2)*(p_Patch->GridHeight-2);
	p_Patch->pGEO_Object_Normals = (GEO_Vertex *) gpst_WaterNormal;
	
	p_Patch->GEO_Object . dst_Point			= p_Patch->pGEO_Object_Points;
	p_Patch->GEO_Object . dst_PointNormal	= (MATH_tdst_Vector*)p_Patch->pGEO_Object_Normals;
	p_Patch->GEO_Object . l_NbUVs			= p_Patch->GEO_Object . l_NbPoints;
	p_Patch->GEO_Object . dst_UV				= p_Patch->pGEO_Object_UVs;
	p_Patch->GEO_Object .l_NbElements		= 1;
	p_Patch->GEO_Object .dst_Element			= &p_Patch->GEO_Element;
	p_Patch->GEO_Object .ulStripFlag			= GEO_C_Strip_DataValid;

	memset(&p_Patch->GEO_Element, 0 , sizeof( GEO_tdst_ElementIndexedTriangles ));
	p_Patch->GEO_Element . dst_Triangle = NULL;
	p_Patch->GEO_Element . l_NbTriangles = 0;
	p_Patch->GEO_Element . l_MaterialId  = ((GEO_tdst_Object*)_pst_GO->pst_Base->pst_Visu->pst_Object)->dst_Element[0].l_MaterialId;

#ifdef _XENON_RENDER
    if (GDI_b_IsXenonGraphics())
    {
        p_Patch->pGEO_Object_Points = NULL;
        p_Patch->pGEO_Object_UVs = NULL;

        // create a mesh container
        XeMesh *poMesh = new XeMesh;
        p_Patch->m_poWaterMesh = poMesh;

        poMesh->ClearAllStreams();
        poMesh->AddStream(XEVC_POSITION | XEVC_TEXCOORD0, 0, NULL, p_Patch->GEO_Object.l_NbPoints);
        poMesh->AddStream(XEVC_NORMAL | XEVC_TANGENT, 1, NULL, p_Patch->GEO_Object.l_NbPoints);

        struct WaterVertex {
            FLOAT x,y,z;
            FLOAT u,v;
        };

        WaterVertex *pVertex = (WaterVertex*) poMesh->GetStream(0)->pBuffer->Lock(p_Patch->GEO_Object.l_NbPoints, 5*sizeof(FLOAT));

        FLOAT fPatchRangeX = (p_Patch->fMaxX - p_Patch->fMinX);
        FLOAT fPatchRangeY = (p_Patch->fMaxY - p_Patch->fMinY);
        FLOAT fWaterRangeX = (p_Water3D->fMaxX - p_Water3D->fMinX);
        FLOAT fWaterRangeY = (p_Water3D->fMaxY - p_Water3D->fMinY);

        // create geometry
        for ( int i = 0, k = 0; i < p_Patch->GridHeight-2; i++ )
        {
		    for ( int j = 0; j < p_Patch->GridWidth-2; j++, k++ )
            {
                pVertex->x = p_Patch->fMinX + fPatchRangeX*((FLOAT)j/(p_Patch->GridWidth-3));
                pVertex->y = p_Patch->fMinY + fPatchRangeY*((FLOAT)i/(p_Patch->GridHeight-3));
                pVertex->z = 0.0f;
                pVertex->u = (pVertex->x - p_Water3D->fMinX) / fWaterRangeX;
                pVertex->v = (pVertex->y - p_Water3D->fMinY) / fWaterRangeY;
                pVertex++;
            }
	    }

        poMesh->GetStream(0)->pBuffer->Unlock();

        int iRowCount = p_Patch->GridHeight-3;
        int iColCount = p_Patch->GridWidth-3;
        int iIndexCount = iRowCount * iColCount * 2 * 3;
        int iTopLine, iBottomLine;
 
        XeIndexBuffer *pIndexBuffer = g_XeBufferMgr.CreateIndexBuffer(iIndexCount);
        WORD* pwIndex = (WORD*) pIndexBuffer->Lock(iIndexCount);



        for (int iCurRow = 0; iCurRow < iRowCount; iCurRow++)
        {
            iTopLine = (iColCount+1) * iCurRow;
            iBottomLine = iTopLine + (iColCount+1);

            for (int iCurCol = 0; iCurCol < iColCount; iCurCol++, iTopLine++, iBottomLine++)
            {
                *pwIndex = (WORD) iTopLine; pwIndex++;
                *pwIndex = (WORD) iBottomLine; pwIndex++;
                *pwIndex = (WORD) (iBottomLine+1); pwIndex++;

                *pwIndex = (WORD) iTopLine; pwIndex++;
                *pwIndex = (WORD) (iBottomLine+1); pwIndex++;
                *pwIndex = (WORD) (iTopLine+1); pwIndex++;
            }
        }

        pIndexBuffer->Unlock();
        pIndexBuffer->SetFaceCount(iRowCount * iColCount * 2);
        poMesh->SetIndices(pIndexBuffer);

        // SC: The index buffer now belongs to the mesh...
        pIndexBuffer->Release();

        // create a mesh container for static mesh
        poMesh = new XeMesh;
        p_Patch->m_poStaticWaterMesh = poMesh;

        poMesh->ClearAllStreams();
        poMesh->AddStream(XEVC_POSITION | XEVC_NORMAL | XEVC_TANGENT | XEVC_TEXCOORD0, 0, NULL, 6);

        struct StaticWaterVertex {
            FLOAT x,y,z;
            FLOAT u,v;
            FLOAT Nx,Ny,Nz;
            FLOAT Tx,Ty,Tz,RealZ;
        };

        StaticWaterVertex *pStaticVertex = (StaticWaterVertex*) poMesh->GetStream(0)->pBuffer->Lock(6, 12*sizeof(FLOAT));

        // add 6 vertices for static patch optimization
        
        // Top Left
        pStaticVertex->x = p_Patch->fMinX;
        pStaticVertex->y = p_Patch->fMinY;
        pStaticVertex->z = p_Patch->fZMinMin;
        pStaticVertex->u = (pStaticVertex->x - p_Water3D->fMinX) / fWaterRangeX;
        pStaticVertex->v = (pStaticVertex->y - p_Water3D->fMinY) / fWaterRangeY;
        pStaticVertex->Nx = 0.0f;
        pStaticVertex->Ny = 0.0f;
        pStaticVertex->Nz = 1.0f;
        pStaticVertex->RealZ = p_Patch->fZMinMin;
        pStaticVertex->Tx = 1.0f;
        pStaticVertex->Ty = 0.0f;
        pStaticVertex->Tz = 0.0f;
        pStaticVertex++;

        // Bottom Left
        pStaticVertex->x = p_Patch->fMinX;
        pStaticVertex->y = p_Patch->fMaxY;
        pStaticVertex->z = p_Patch->fZMinMax;
        pStaticVertex->u = (pStaticVertex->x - p_Water3D->fMinX) / fWaterRangeX;
        pStaticVertex->v = (pStaticVertex->y - p_Water3D->fMinY) / fWaterRangeY;
        pStaticVertex->Nx = 0.0f;
        pStaticVertex->Ny = 0.0f;
        pStaticVertex->Nz = 1.0f;
        pStaticVertex->RealZ = p_Patch->fZMinMax;
        pStaticVertex->Tx = 1.0f;
        pStaticVertex->Ty = 0.0f;
        pStaticVertex->Tz = 0.0f;
        pStaticVertex++;

        // Bottom Right
        pStaticVertex->x = p_Patch->fMaxX;
        pStaticVertex->y = p_Patch->fMaxY;
        pStaticVertex->z = p_Patch->fZMaxMax;
        pStaticVertex->u = (pStaticVertex->x - p_Water3D->fMinX) / fWaterRangeX;
        pStaticVertex->v = (pStaticVertex->y - p_Water3D->fMinY) / fWaterRangeY;
        pStaticVertex->Nx = 0.0f;
        pStaticVertex->Ny = 0.0f;
        pStaticVertex->Nz = 1.0f;
        pStaticVertex->RealZ = p_Patch->fZMaxMax;
        pStaticVertex->Tx = 1.0f;
        pStaticVertex->Ty = 0.0f;
        pStaticVertex->Tz = 0.0f;
        pStaticVertex++;        

        // Top Left
        pStaticVertex->x = p_Patch->fMinX;
        pStaticVertex->y = p_Patch->fMinY;
        pStaticVertex->z = p_Patch->fZMinMin;
        pStaticVertex->u = (pStaticVertex->x - p_Water3D->fMinX) / fWaterRangeX;
        pStaticVertex->v = (pStaticVertex->y - p_Water3D->fMinY) / fWaterRangeY;
        pStaticVertex->Nx = 0.0f;
        pStaticVertex->Ny = 0.0f;
        pStaticVertex->Nz = 1.0f;
        pStaticVertex->RealZ = p_Patch->fZMinMin;
        pStaticVertex->Tx = 1.0f;
        pStaticVertex->Ty = 0.0f;
        pStaticVertex->Tz = 0.0f;
        pStaticVertex++;

        // Bottom Right
        pStaticVertex->x = p_Patch->fMaxX;
        pStaticVertex->y = p_Patch->fMaxY;
        pStaticVertex->z = p_Patch->fZMaxMax;
        pStaticVertex->u = (pStaticVertex->x - p_Water3D->fMinX) / fWaterRangeX;
        pStaticVertex->v = (pStaticVertex->y - p_Water3D->fMinY) / fWaterRangeY;
        pStaticVertex->Nx = 0.0f;
        pStaticVertex->Ny = 0.0f;
        pStaticVertex->Nz = 1.0f;
        pStaticVertex->RealZ = p_Patch->fZMaxMax;
        pStaticVertex->Tx = 1.0f;
        pStaticVertex->Ty = 0.0f;
        pStaticVertex->Tz = 0.0f;
        pStaticVertex++;        

        // Top Right
        pStaticVertex->x = p_Patch->fMaxX;
        pStaticVertex->y = p_Patch->fMinY;
        pStaticVertex->z = p_Patch->fZMaxMin;
        pStaticVertex->u = (pStaticVertex->x - p_Water3D->fMinX) / fWaterRangeX;
        pStaticVertex->v = (pStaticVertex->y - p_Water3D->fMinY) / fWaterRangeY;
        pStaticVertex->Nx = 0.0f;
        pStaticVertex->Ny = 0.0f;
        pStaticVertex->Nz = 1.0f;
        pStaticVertex->RealZ = p_Patch->fZMaxMin;
        pStaticVertex->Tx = 1.0f;
        pStaticVertex->Ty = 0.0f;
        pStaticVertex->Tz = 0.0f;

        poMesh->GetStream(0)->pBuffer->Unlock();

        poMesh->SetIndices(NULL);
    }
    else
  #endif
    {
	    p_Patch->pGEO_Object_Points = (GEO_Vertex *) MEM_p_AllocAlign(sizeof(GEO_Vertex)*p_Patch->GEO_Object.l_NbPoints, 32);
	    p_Patch->GEO_Object . dst_Point			= p_Patch->pGEO_Object_Points;
	    p_Patch->pGEO_Object_UVs = (GEO_tdst_UV *) MEM_p_AllocAlign(sizeof(GEO_tdst_UV)*p_Patch->GEO_Object.l_NbPoints, 32);
	    p_Patch->GEO_Object . dst_UV				= p_Patch->pGEO_Object_UVs;

        // create geometry
        for ( int i = 0, k = 0; i < p_Patch->GridHeight-2; i++ )
        {
            for ( int j = 0; j < p_Patch->GridWidth-2; j++, k++ )
            {
                p_Patch->GEO_Object . dst_Point[k].x = p_Patch->fMinX + (p_Patch->fMaxX - p_Patch->fMinX)*((FLOAT)j/(p_Patch->GridWidth-3));
                p_Patch->GEO_Object . dst_Point[k].y = p_Patch->fMinY + (p_Patch->fMaxY - p_Patch->fMinY)*((FLOAT)i/(p_Patch->GridHeight-3));
                p_Patch->GEO_Object . dst_Point[k].z = 0.0f;

                p_Patch->pGEO_Object_UVs[k].fU = (FLOAT)j/(p_Patch->GridWidth-3);
                p_Patch->pGEO_Object_UVs[k].fV = (FLOAT)i/(p_Patch->GridWidth-3);
            }
        }

        // create strips
	    {
		    // allocate space for strip data 
		    p_Patch->GEO_Element . pst_StripData = (GEO_tdst_StripData *) MEM_p_Alloc(sizeof(GEO_tdst_StripData));
		    p_Patch->GEO_Element . pst_StripData->ulFlag = GEO_C_Strip_DataValid;
		    p_Patch->GEO_Element . pst_StripData->ulStripNumber = p_Patch->GridHeight-3;
		    p_Patch->GEO_Element . pst_StripData->pStripList = (GEO_tdst_OneStrip *) MEM_p_Alloc(sizeof(GEO_tdst_OneStrip) * p_Patch->GEO_Element . pst_StripData->ulStripNumber);

		    GEO_tdst_OneStrip					*pStrip, *pLastStrip;
		    pStrip = p_Patch->GEO_Element . pst_StripData->pStripList;
		    pLastStrip = pStrip + p_Patch->GEO_Element . pst_StripData->ulStripNumber;
    		
		    ULONG								ul_Count;
    #ifndef ACTIVE_EDITORS
		    ULONG								ul_Count1;
    #endif
		    int k=0;
    		
		    ul_Count = 0;
		    for(; pStrip < pLastStrip; pStrip++)
		    {
			    /* size of the strip */
			    pStrip->ulVertexNumber = (p_Patch->GridWidth-2)<<1;
			    ul_Count += pStrip->ulVertexNumber;

			    /* vertex data */
    #ifndef ACTIVE_EDITORS
			    pStrip->pMinVertexDataList = (GEO_tdst_MinVertexData *) MEM_p_AllocFromEnd(sizeof(GEO_tdst_MinVertexData) * pStrip->ulVertexNumber);
    #else
			    pStrip->pMinVertexDataList = (GEO_tdst_MinVertexData *) MEM_p_Alloc(sizeof(GEO_tdst_MinVertexData) * pStrip->ulVertexNumber);
    #endif
			    GEO_tdst_MinVertexData * pp = pStrip->pMinVertexDataList;
			    for(int i = 0; (ULONG)i < pStrip->ulVertexNumber; i+=2, ++pp)
			    {
				    pp->auw_Index = k+(p_Patch->GridWidth-2);
				    pp->auw_UV = k+(p_Patch->GridWidth-2);
				    pp++;
				    pp->auw_Index = k;
				    pp->auw_UV = k;
				    k++;
			    }
		    }

    #ifndef ACTIVE_EDITORS
		    p_Patch->GEO_Element . pst_StripData->pStripDatas = (GEO_tdst_MinVertexData *) MEM_p_Alloc(sizeof(GEO_tdst_MinVertexData) * ul_Count);
		    pLastStrip = p_Patch->GEO_Element . pst_StripData->pStripList;
		    pStrip = pLastStrip + p_Patch->GEO_Element . pst_StripData->ulStripNumber - 1;
		    ul_Count1 = 0;
		    for(; pStrip >= pLastStrip; pStrip--)
		    {
			    ul_Count1 += pStrip->ulVertexNumber;
			    L_memcpy(p_Patch->GEO_Element . pst_StripData->pStripDatas + ul_Count - ul_Count1, pStrip->pMinVertexDataList, sizeof(GEO_tdst_MinVertexData) * pStrip->ulVertexNumber);
			    MEM_FreeFromEnd(pStrip->pMinVertexDataList);
			    pStrip->pMinVertexDataList = p_Patch->GEO_Element . pst_StripData->pStripDatas + ul_Count - ul_Count1;
		    }
    #endif
	    }
    }
#endif // XML_CONV_TOOL
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void WATER3D_FreeObject(WATER3D_tdst_Modifier *p_Water3D)
{
	for(int i=0; i<p_Water3D->PatchSizeY; i++)
	{
		for(int j=0; j<p_Water3D->PatchSizeX; j++)
		{
			if(!p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]) 
				continue;

          #ifdef _XENON_RENDER
            if (!GDI_b_IsXenonGraphics())
          #endif
            {
			#ifdef ACTIVE_EDITORS
				GEO_tdst_OneStrip					*pStrip, *pLastStrip;
				pStrip = p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]->GEO_Element.pst_StripData->pStripList;
				pLastStrip = pStrip + p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]->GEO_Element.pst_StripData->ulStripNumber;

				for(; pStrip < pLastStrip; pStrip++)
				{
					MEM_Free(pStrip->pMinVertexDataList);
				}
			#else
				MEM_Free(p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]->GEO_Element.pst_StripData->pStripDatas);
			#endif

				MEM_Free(p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]->GEO_Element.pst_StripData->pStripList);
				MEM_Free(p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]->GEO_Element.pst_StripData);
				MEM_FreeAlign(p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]->pGEO_Object_Points);
				MEM_FreeAlign(p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]->pGEO_Object_UVs);
            }

			MEM_SafeFree(p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]);

		}
	}

	MEM_SafeFree(p_Water3D->PatchGrid);
	p_Water3D->PatchGrid = NULL;
}



/*
=======================================================================================================================
=======================================================================================================================
*/
void WATER3D_Modifier_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	WATER3D_tdst_Modifier *pst_Data;

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(WATER3D_tdst_Modifier));

	pst_Data = (WATER3D_tdst_Modifier *) _pst_Mod->p_Data;
	
	if(!p_Data)
	{
			L_memset(pst_Data , 0 , sizeof(WATER3D_tdst_Modifier));
#ifdef ACTIVE_EDITORS
			pst_Data->ulCodeKey = 0xC0DE2002; 
#endif
			// default values
			pst_Data->Density = 700;
			pst_Data->fDamping = 0.975f;
			pst_Data->fPropagationSpeed = 5.0f;
			pst_Data->fPerturbanceAmplitudeModifier = 1.0f;
			pst_Data->fImpactForceAttenuation = 0.8f;
			pst_Data->fTurbulanceAmplitude = -0.150f;
			pst_Data->lTurbulanceFactor = 0;
			pst_Data->fRadius = 10.0f;
			pst_Data->fDampingOutsideRadius = 0.9f;
			pst_Data->bTurbulanceOffIfOutsideRadius = TRUE;
			pst_Data->bWaterChrome=0;
            pst_Data->fRefractionIntensity = 1.0f;
            pst_Data->fReflectionIntensity = 1.0f;
            pst_Data->fWaterDensity = 0.5f;
            pst_Data->fBaseMapOpacity = 0.5f;
            pst_Data->fMossMapOpacity = 0.5f;
            pst_Data->fFogIntensity = 1.0f;
    }
	else
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(WATER3D_tdst_Modifier));
        pst_Data->p_NextActiveWater3D = NULL;   // make sure it will be inserted in the active water object list
    }
	
	pst_Data->bActive = FALSE;
	pst_Data->p_GO = _pst_GO;
	pst_Data->pst_Mod = _pst_Mod;

	pst_Data->PatchGrid = NULL;

#ifdef ACTIVE_EDITORS
	pst_Data->ulCodeKey = 0xC0DE2001;
	pst_Data->iNbPoly = 0;
#endif
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void WATER3D_Modifier_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
#ifdef ACTIVE_EDITORS
	g_iWaterMemoryMax = 0;
#endif
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void WATER3D_Modifier_Reset( WATER3D_tdst_Modifier *p_Water3D)
{ 
#if !defined(XML_CONV_TOOL)
	if (p_Water3D->bActive)
	{
		for(int i=0; i<p_Water3D->PatchSizeY; i++)
		{
			for(int j=0; j<p_Water3D->PatchSizeX; j++)
			{
				if(!p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]) 
					continue;

#ifdef USE_WATER3D_MEMORY
				WATER3D_PatchFree(p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]);
#else
				MEM_FreeAlign(p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]->SZ);
				MEM_FreeAlign(p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]->VZ);
#endif

              #ifdef _XENON_RENDER
                THREAD_SAFE_MESH_DELETE(p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]->m_poWaterMesh);
                THREAD_SAFE_MESH_DELETE(p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]->m_poStaticWaterMesh);

                if (p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]->m_ulPatchInfoId != -1)
                {
                    g_oWaterManager.UnregisterWaterPatch(p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]);
                    p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]->m_ulPatchInfoId = -1;
                }
              #endif
			}
		}

		WATER3D_FreeObject(p_Water3D);

		p_Water3D->bActive = FALSE;
	}
#endif
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void WATER3D_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	WATER3D_tdst_Modifier *pst_Data;
	WATER3D_tdst_Modifier **pp_Parser;
	
	pst_Data = (WATER3D_tdst_Modifier *) _pst_Mod->p_Data;	
	pp_Parser = &p_FirstActiveWater3D;

	while (*pp_Parser)
	{
		if (*pp_Parser == pst_Data)
			(*pp_Parser) = (*pp_Parser)->p_NextActiveWater3D;
		else
			pp_Parser = &(*pp_Parser)->p_NextActiveWater3D;
	}

	WATER3D_Modifier_Reset(pst_Data);
		
    MEM_Free(pst_Data);
	
#ifdef USE_WATER3D_MEMORY		
	if(gpst_WaterMemory)
	{
		WATER3D_DefragmentMemory();
		if(*gpst_WaterMemory==(WATER3D_MEMORY_SIZE-4)) // no alloc left
		{
			MEM_Free(gpst_WaterMemory);
			gpst_WaterMemory_Current = gpst_WaterMemory = NULL;
	#ifndef _FINAL_
			g_iWaterMemory = 0;
	#endif			
		}	
	}
#endif
}

#if !defined(_FINAL_) && defined(ACTIVE_EDITORS)

void WATER3D_Modifier_MemoryCompute(WATER3D_tdst_Modifier *p_Water3D)
{
	WATER3D_tdst_Patches *p_Patch;
	
	if (!p_Water3D->bActive)
		return;
	
	for(int i=0; i<p_Water3D->PatchSizeY; i++)
	{
		for(int j=0; j<p_Water3D->PatchSizeX; j++)
		{
			if((p_Patch=p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j])!=NULL)
			{
				if(!p_Patch->ulFlag)
					g_iWaterMemory += p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT)*2;
			}
		}
	}
}

#endif //#if !defined(_FINAL_) && defined(ACTIVE_EDITORS)



/*
=======================================================================================================================
=======================================================================================================================
*/
void WATER3D_Modifier_Disturb(OBJ_tdst_GameObject *pGO, MATH_tdst_Vector *pDisturberArray, FLOAT *pDisturberValues, INT nbDisturber, FLOAT fZOffset, BOOL bFloatOnWater, MATH_tdst_Vector *_pvMin, MATH_tdst_Vector *_pvMax)
{ 
#if !defined(XML_CONV_TOOL)
	WATER3D_tdst_Modifier *p_Water3D;
	p_Water3D = p_FirstActiveWater3D;
	int currentDisturber;
	int i,j;
	MATH_tdst_Vector	colPos;
	MATH_tdst_Matrix	invGlobalMatrix;

	if(!nbDisturber)
		return;

	while (p_Water3D && p_Water3D->bActive)
	{
		MATH_InvertMatrix(&invGlobalMatrix, p_Water3D->p_GO->pst_GlobalMatrix);

		for(currentDisturber=0; currentDisturber<nbDisturber; currentDisturber++)
		{
			WATER3D_tdst_Patches * p_Patch;

			for(i=p_Water3D->PatchSizeY-1; i>=0; i--)
			{
				for(j=0; j<p_Water3D->PatchSizeX; j++)
				{
					if(!p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]) 
						continue;						

					p_Patch = p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j];

#if defined(_XENON)
					__dcbt(j*128, p_Water3D->PatchGrid[i * p_Water3D->PatchSizeX + (j + 1)]);
#endif

					FLOAT fMinX, fMaxX, fMinY, fMaxY;

#if defined(_XENON)
					fMinX = fMin(p_Patch->fMinXGlobal, p_Patch->fMaxXGlobal);
					fMaxX = fMax(p_Patch->fMinXGlobal, p_Patch->fMaxXGlobal);

					fMinY = fMin(p_Patch->fMinYGlobal, p_Patch->fMaxYGlobal);
					fMaxY = fMax(p_Patch->fMinYGlobal, p_Patch->fMaxYGlobal);
#else
                    if (p_Patch->fMinXGlobal < p_Patch->fMaxXGlobal)
                    {
                        fMinX = p_Patch->fMinXGlobal;
                        fMaxX = p_Patch->fMaxXGlobal;
                    }
                    else
                    {
                        fMinX = p_Patch->fMaxXGlobal;
                        fMaxX = p_Patch->fMinXGlobal;
                    }

                    if (p_Patch->fMinYGlobal < p_Patch->fMaxYGlobal)
                    {
                        fMinY = p_Patch->fMinYGlobal;
                        fMaxY = p_Patch->fMaxYGlobal;
                    }
                    else
                    {
                        fMinY = p_Patch->fMaxYGlobal;
                        fMaxY = p_Patch->fMinYGlobal;
                    }
#endif
					if( pDisturberArray[currentDisturber].x > fMinX && 
                        pDisturberArray[currentDisturber].x < fMaxX && 
                        pDisturberArray[currentDisturber].y > fMinY && 
                        pDisturberArray[currentDisturber].y < fMaxY)
					{
						// get position of possibly colliding gameobject
						MATH_TransformVertex(&colPos, &invGlobalMatrix, &pDisturberArray[currentDisturber]);

						if(bFloatOnWater)
						{
							int X= (int) ((colPos.x-p_Patch->fMinX)/(p_Patch->fMaxX-p_Patch->fMinX)*(p_Patch->GridWidth-3)+1);
							int Y= (int) ((colPos.y-p_Patch->fMinY)/(p_Patch->fMaxY-p_Patch->fMinY)*(p_Patch->GridHeight-3)+1);
							
							// if patch is hidden, there is no computation done and the SZ may be null. take the z of the patch instead
							if(p_Patch->SZ)
								colPos.z = ((FLOAT*)p_Patch->SZ)[X+(Y*p_Patch->GridWidth)];
							else
								colPos.z = p_Patch->fZMinMin;
								
							MATH_TransformVertex(&pDisturberArray[currentDisturber], p_Water3D->p_GO->pst_GlobalMatrix, &colPos);
							
							if(_pvMin)
								MATH_AddVector(_pvMin, &p_Water3D->p_GO->pst_GlobalMatrix->T, OBJ_pst_BV_GetGMin(p_Water3D->p_GO->pst_BV));
							if(_pvMax)
								MATH_AddVector(_pvMax, &p_Water3D->p_GO->pst_GlobalMatrix->T, OBJ_pst_BV_GetGMax(p_Water3D->p_GO->pst_BV));
						}

						if(p_Patch->nCurrentDisturbance<MAX_DISTURBANCE)
						{	
							if(colPos.z + fZOffset <= p_Patch->fZMinMin)// && colPos.z >= p_Water3D->fZ-p_Water3D->fHeight)
							{
#if defined(_XENON)
								colPos.z = fMin(1.0f, fMax(-1.0f, pDisturberValues[currentDisturber])) * p_Water3D->fImpactForceAttenuation;
#else
								colPos.z = MATH_Min(1.0f, MATH_Max(-1.0f, pDisturberValues[currentDisturber]))*p_Water3D->fImpactForceAttenuation; // disturbance force
#endif

								pDisturberValues[currentDisturber] += 1000.0f; // let disturber know that it has actually perturbed something

								int disturb = p_Patch->nCurrentDisturbance;
								while(disturb--)
								{
									if((colPos.x == p_Patch->vDisturbances[disturb].x) && (colPos.y == p_Patch->vDisturbances[disturb].y))
										break;
								}

								if(disturb<0) // no disturbance already at this X,Y
								{
									MATH_CopyVector(&p_Patch->vDisturbances[p_Patch->nCurrentDisturbance], &colPos);							
									p_Patch->nCurrentDisturbance++;
								}
								else
								{
#if defined(_XENON)
									p_Patch->vDisturbances[disturb].z = fMin(1.0f, fMax(-1.0f, p_Patch->vDisturbances[disturb].z + colPos.z)) * p_Water3D->fImpactForceAttenuation;
#else
									p_Patch->vDisturbances[disturb].z = MATH_Min(1.0f, MATH_Max(-1.0f, p_Patch->vDisturbances[disturb].z + colPos.z))*p_Water3D->fImpactForceAttenuation;
#endif
								}

								p_Patch->ulFlag = 0; // disturbance : recompute patch
							}
						}
					}
				}
			}
		}

		p_Water3D = p_Water3D->p_NextActiveWater3D;
	}
#endif // XML_CONV_TOOL
}

int WATER3D_Modifier_ComputePatch(WATER3D_tdst_Modifier *p_Water3D, WATER3D_tdst_Patches *p_Patch)
{
	INT						i,j;
	BOOL					bOutsideRadius = FALSE;
	FLOAT					fDistance;
	
	if((p_Patch->ulFlag & WATER3D_Static) && fAbs(p_Water3D->lTurbulanceFactor*p_Water3D->fTurbulanceAmplitude)<0.0001f && !p_Patch->nCurrentDisturbance)
	{
#ifdef USE_WATER3D_MEMORY
		WATER3D_PatchFree(p_Patch);
#endif
		return 0;
	}
	
	// radius check

	if((p_Water3D->fRadius>0.0f) || (p_Water3D->fRadiusCut>0.0f))
	{
		MATH_tdst_Vector center;
		center.x = (p_Patch->fMaxX + p_Patch->fMinX)*0.5f;
		center.y = (p_Patch->fMaxY + p_Patch->fMinY)*0.5f;
		center.z = (p_Patch->fZMaxMax + p_Patch->fZMinMin)*0.5f;
		
		MATH_TransformVertex(&center, GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix, &center);
		
		fDistance = MATH_f_NormVector(&center);
		
		if((p_Water3D->fRadiusCut>0.0f) && (fDistance>p_Water3D->fRadiusCut))
		{
			p_Patch->ulFlag |= WATER3D_Static;

#ifdef USE_WATER3D_MEMORY
			WATER3D_PatchFree(p_Patch);
#else
			memset(p_Patch->VZ, 0, p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT));
			memset(p_Patch->SZ, 0, p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT));
#endif
			return 0;
		}
		
		if((p_Water3D->fRadius>0.0f) && (fDistance>p_Water3D->fRadius))
			bOutsideRadius = TRUE;
	}
	
#ifdef USE_WATER3D_MEMORY
	WATER3D_PatchAlloc(p_Water3D, p_Patch);
#endif
	
	// compute resulting forces----------------------------------------------------------------------------------------------

	if(    (p_Water3D->p_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Visible)
      #ifdef VIDEOCONSOLE_ENABLE
        && (!NoWaterTurbulance)
      #endif
        ) // if GO is visible
	{
		FLOAT	dz;

		FLOAT *VZ = (FLOAT*)p_Patch->VZ;
		FLOAT *SZ = (FLOAT*)p_Patch->SZ;
		
		FLOAT *pFZ = TotalForceZ + p_Patch->GridWidth;
		FLOAT *pFZPreviousRow = TotalForceZ;
		FLOAT *pFZNextRow = TotalForceZ + p_Patch->GridWidth;
		FLOAT *pSZ = SZ;
		FLOAT *pSZPreviousRow = SZ;
		FLOAT *pSZNextRow = SZ + p_Patch->GridWidth;

		// For each grid nodes, compute the force
		
		// clear force for current row
		memset(pFZNextRow, 0, (p_Patch->GridWidth)*sizeof(FLOAT));

		p_Patch->fDisturbanceMax = 0.0f;

		// first row
		for ( j = 0; j < (p_Patch->GridWidth-1); j++)
		{
			// Distance between C-S vertices
			*pFZNextRow -= (*pSZNextRow) - (*pSZ);

			// Distance between C-SE vertices
			*(pFZNextRow + 1) = (*pSZ) - *(pSZNextRow + 1);

			pFZNextRow++;
			pSZ++; pSZNextRow++;
		}

		// skip last column
		pFZNextRow++;
		pSZ++; pSZNextRow++;

		for ( i = 1; i < (p_Patch->GridHeight - 1); i++ )
		{	
			// Clear first force in the next row
			*pFZNextRow = 0.0f;
			
			for ( j = 0; j < (p_Patch->GridWidth - 1); j++)
			{
				register float FZAbs;
				// Distance between C-E vertices
				dz = (*(pSZ + 1) - (*pSZ));
				*pFZ += dz;
				*(pFZ+1) -= dz;

				// Distance between C-S vertices
				dz = (*pSZNextRow) - (*pSZ);
				*pFZ += dz;
				*pFZNextRow -= dz;

				// Distance between C-SE vertices
				dz = (*(pSZNextRow + 1) - (*pSZ));
				*pFZ += dz;
				*(pFZNextRow + 1) = -dz;

				// Distance between C-NE vertices
				dz = (*(pSZPreviousRow + 1) - (*pSZ));
				*pFZ += dz;
				*(pFZPreviousRow + 1) -= dz;

				FZAbs=fAbs(*pFZ);
				
#if defined(_XENON)
				if(j>0)
				{
					p_Patch->fDisturbanceMax = fMax(p_Patch->fDisturbanceMax, FZAbs);
				}
#else
				if(j>0 && p_Patch->fDisturbanceMax<FZAbs) 
					p_Patch->fDisturbanceMax=FZAbs;
#endif

				pFZ++; pFZNextRow++; pFZPreviousRow++;
				pSZ++; pSZNextRow++; pSZPreviousRow++;
			}

			// skip last column
			pFZ++; pFZNextRow++; pFZPreviousRow++;
			pSZ++; pSZNextRow++; pSZPreviousRow++;
		} 

		// add external forces (disturbance) ------------------------------------------------------------------------------------------

		// apply turbulance

		// compute the turbulance factor depending on framerate
		LONG lTurbulanceFactor = p_Water3D->lTurbulanceFactor;	
		p_Patch->fLastTurbulance += WATER3D_fsec;

		/*int iTurbulanceAdjust=0;*/

#define IDEAL_DT	0.16666f
		int iTurbulanceAdjust = (int)(p_Patch->fLastTurbulance / IDEAL_DT);
		p_Patch->fLastTurbulance -= (FLOAT)iTurbulanceAdjust * IDEAL_DT;

		/*while (p_Patch->fLastTurbulance> IDEAL_DT)		
		{
			iTurbulanceAdjust++;
			p_Patch->fLastTurbulance -= IDEAL_DT;
		}*/

		lTurbulanceFactor *= (bOutsideRadius && p_Water3D->bTurbulanceOffIfOutsideRadius) ? 0 : iTurbulanceAdjust;

		while(lTurbulanceFactor>0)
		{
			USHORT X,Y;
			FLOAT fVal;

			X = (USHORT)fRand(0, (float) (p_Patch->GridWidth-2));
			Y = (USHORT)fRand(0, (float) (p_Patch->GridHeight-2));
			fVal = (FLOAT)p_Patch->TurbulanceGuide*p_Water3D->fTurbulanceAmplitude;

			TotalForceZ[X + 1 + ((Y + 1) * p_Patch->GridWidth)] += fVal;
			TotalForceZ[X + (Y * p_Patch->GridWidth)] += fVal;
			TotalForceZ[X + ((Y + 1) * p_Patch->GridWidth)] += fVal;
			TotalForceZ[X + 1 + (Y * p_Patch->GridWidth)] += fVal;
			
			fVal=fAbs(fVal);
			
#if defined(_XENON)
			p_Patch->fDisturbanceMax = fMax(p_Patch->fDisturbanceMax, fVal);
#else
			if(p_Patch->fDisturbanceMax<fVal)
				p_Patch->fDisturbanceMax = fVal;
#endif

			p_Patch->TurbulanceGuide = -p_Patch->TurbulanceGuide;
			lTurbulanceFactor--;
		}

		// apply disturbances ---------------------------------------------------------------------------------------------------------

		while( p_Patch->nCurrentDisturbance--)
		{
			FLOAT fVal;
			USHORT X,Y;

#if defined(_XENON)
			if(*(int*)&(p_Patch->vDisturbances[p_Patch->nCurrentDisturbance].z) == 0) continue;
#else
			if(p_Patch->vDisturbances[p_Patch->nCurrentDisturbance].z == 0.0f) continue;
#endif

			X= (USHORT) ((p_Patch->vDisturbances[p_Patch->nCurrentDisturbance].x-p_Patch->fMinX)/(p_Patch->fMaxX-p_Patch->fMinX)*(p_Patch->GridWidth-3)+1);
			Y= (USHORT) ((p_Patch->vDisturbances[p_Patch->nCurrentDisturbance].y-p_Patch->fMinY)/(p_Patch->fMaxY-p_Patch->fMinY)*(p_Patch->GridHeight-3)+1);

			// Clamp X and Y Values (Patch For Negative Unsigned Wrap-Around)

			if (Y >= (MAX_PATCH_SIZE - 1))
			{
				Y = 0;
			}

            if (X >= (MAX_PATCH_SIZE - 1))
            {
                X = 0;
            }

            fVal = p_Patch->vDisturbances[p_Patch->nCurrentDisturbance].z*p_Water3D->fPerturbanceAmplitudeModifier;//*FLOAT_SCALE;

			TotalForceZ[X + 1 + ((Y + 1) * p_Patch->GridWidth)] += fVal;
			TotalForceZ[X + (Y * p_Patch->GridWidth)] += fVal;
			TotalForceZ[X + ((Y + 1) * p_Patch->GridWidth)] += fVal;
			TotalForceZ[X + 1 + (Y * p_Patch->GridWidth)] += fVal;

			fVal=fAbs(fVal);

#if defined(_XENON)
			p_Patch->fDisturbanceMax = fMax(p_Patch->fDisturbanceMax, fVal);
#else
			if(p_Patch->fDisturbanceMax<fVal)
				p_Patch->fDisturbanceMax = fVal;
#endif
		}

		p_Patch->nCurrentDisturbance = 0;
		
		// Update velocity ad position of grid nodes ------------------------------------------------------------------------------------
		FLOAT ffactor = WATER3D_fsec*p_Water3D->fPropagationSpeed;
		FLOAT fDamping = bOutsideRadius ? (p_Water3D->fDampingOutsideRadius*(p_Water3D->fRadius/fDistance)) : p_Water3D->fDamping; 

		pFZ = &TotalForceZ[p_Patch->GridWidth];
		pSZ = &SZ[p_Patch->GridWidth];
		FLOAT *pVZ = &VZ[p_Patch->GridWidth];
		
		FLOAT fPosDamping=1.0f;
		
		if(bOutsideRadius)
		{
			fPosDamping = (p_Patch->fDisturbanceMax < 0.001f) ? fDamping : (p_Patch->fDisturbanceMax < 0.0005f ? (1.0f - (0.0005f - p_Patch->fDisturbanceMax)*2000.0f) : 1.0f);
			if(p_Water3D->fRadiusCut>0.0f)
			{
				fPosDamping *= 1.0f - ((fDistance - p_Water3D->fRadius)/(p_Water3D->fRadiusCut - p_Water3D->fRadius))*0.5f;
			}
		}
		else
			fPosDamping = (p_Patch->fDisturbanceMax < 0.0001f ? (1.0f - (0.0001f - p_Patch->fDisturbanceMax)*10000.0f) : (p_Patch->fDisturbanceMax < 0.001f ? fDamping : 1.0f));

		BOOL  bUseDampingPos = (fPosDamping!=1.0f);

		if( (p_Patch->fDisturbanceMax > 0.00001f) && !(p_Patch->ulFlag & WATER3D_Culled))
		{
			for ( i = 1; i < (p_Patch->GridHeight-1); i++ )
			{			
				pFZ++;	pSZ++;	pVZ++; // skip first item of row

				for ( j = 1; j < (p_Patch->GridWidth-1); j++)
				{
					// propagation
					
					*pVZ += (*pFZ)*ffactor;    				
					*pSZ += (*pVZ)*ffactor;
		
					// damping
					
					*pVZ *= fDamping;

					if(bUseDampingPos)
						*pSZ *= fPosDamping;

					pFZ++;	pSZ++;	pVZ++;
				}

				pFZ++;	pSZ++;	pVZ++; // skip last item of row
			} 

			p_Patch->ulFlag &= ~WATER3D_Static;
		}
		else
		{
			p_Patch->ulFlag |= WATER3D_Static;

			memset(SZ, 0, p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT));
			memset(VZ, 0, p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT));
		}
	}
	
	return 0;
}

#ifndef _FINAL
float assertfloat(float val)
{
	if ((val>10000.0f) || (val<-10000.0f))
	{
		return 0.0f;
	}
	return val;
}
#endif

void WATER3D_Modifier_ComputePatches(WATER3D_tdst_Modifier *p_Water3D)
{
#if !defined(XML_CONV_TOOL)
	if(!p_Water3D) return;
	if(p_Water3D->bUpdated || p_Water3D->bDrawn) return;	


	WATER3D_fsec = 0.033333f;//M_ConvertMicroSecTofSec(g_stTimeInfo.s64Curren tDt);
	
	//useless test with this assignation
	//if(WATER3D_fsec>0.033334f) // safeguard for low framerates
	//	WATER3D_fsec = 0.033334f;


	p_Water3D->bUpdated = TRUE;

	int i,j,k;
	float PosCurrent, PosNeighbour;

	WATER3D_tdst_Patches *p_Neighbour, *p_Current;
	int iIndex;
	
	MATH_tdst_Vector	st_LMin, st_LMax;

	// compute current state of water
	for(i=0; i<p_Water3D->PatchSizeY; i++)
	{
		iIndex = i*p_Water3D->PatchSizeX;
		
		for(j=0; j<p_Water3D->PatchSizeX; j++, iIndex++)
		{
			
			if((p_Current=p_Water3D->PatchGrid[iIndex])==NULL)
				continue;

			// patch culling test

			st_LMin.x = p_Current->fMinX;
			st_LMin.y = p_Current->fMinY;
			st_LMin.z = p_Current->fZMinMin;

			st_LMax.x = p_Current->fMaxX;
			st_LMax.y = p_Current->fMaxY;
			st_LMax.z = p_Current->fZMaxMax;

			if( OBJ_CullingOBBox
					(
						&st_LMin,
						&st_LMax,
						p_Water3D->p_GO->pst_GlobalMatrix,
						&GDI_gpst_CurDD->st_Camera
					)
					)
			{
				p_Current->ulFlag |= WATER3D_Culled;
				if(p_Current->ulFlag & WATER3D_Static)
				{
#ifdef USE_WATER3D_MEMORY
					WATER3D_PatchFree(p_Current);
#endif
					continue;
				}
			}
			else
				p_Current->ulFlag &= ~WATER3D_Culled;

			WATER3D_Modifier_ComputePatch(p_Water3D,	p_Water3D->PatchGrid[iIndex]);
		}
	}	
	
	// adjust border positions
	for(i=p_Water3D->PatchSizeY-1; i>=0; i--)
	{
		for(j=0; j<p_Water3D->PatchSizeX; j++)
		{
			if((p_Current=p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j])==NULL || (p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j]->ulFlag & WATER3D_Culled))
				continue;

			if(i>0 && (p_Neighbour=p_Water3D->PatchGrid[(i-1)*p_Water3D->PatchSizeX+j])!=NULL) // up
			{
				if(!(p_Neighbour->ulFlag & WATER3D_Culled) && (!(p_Current->ulFlag & WATER3D_Static) || !(p_Neighbour->ulFlag & WATER3D_Static)))
				{
					if(p_Current->ulFlag & WATER3D_Static)
					{
						for (k = 1; k < p_Neighbour->GridWidth-1; k++ )
						{
							((FLOAT*)p_Neighbour->SZ)[(p_Neighbour->GridHeight-2)*p_Neighbour->GridWidth + k] = 0.0f;
						}
					}
					else if(p_Neighbour->ulFlag & WATER3D_Static)
					{
						for (k = 1; k < p_Current->GridWidth-1; k++ )
						{
							((FLOAT*)p_Current->SZ)[k+p_Current->GridWidth] = 0.0f;
						}
					}
					else
					{
						for (k = 1; k < p_Neighbour->GridWidth-1; k++ )
						{
							((FLOAT*)p_Neighbour->SZ)[(p_Neighbour->GridHeight-2)*p_Neighbour->GridWidth + k] = ((FLOAT*)p_Current->SZ)[k+p_Current->GridWidth];
						}
					}
				}
			}
			if(j<p_Water3D->PatchSizeX-1 && (p_Neighbour=p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j+1])!=NULL) // right
			{
				if(!(p_Neighbour->ulFlag & WATER3D_Culled) && (!(p_Current->ulFlag & WATER3D_Static) || (!(p_Neighbour->ulFlag & WATER3D_Static))))
				{
					if(p_Current->ulFlag & WATER3D_Static)
					{
						for (k = 1; k < p_Neighbour->GridHeight-1; k++ )
						{
							((FLOAT*)p_Neighbour->SZ)[k*p_Neighbour->GridWidth+1] = 0.0f;
						}
					}
					else if(p_Neighbour->ulFlag & WATER3D_Static)
					{
						for (k = 1; k < p_Current->GridHeight-1; k++ )
						{
							((FLOAT*)p_Current->SZ)[(k+1)*p_Current->GridWidth-2] = 0.0f;
						}
					}
					else
					{
						for (k = 1; k < p_Neighbour->GridHeight-1; k++ )
						{
							((FLOAT*)p_Neighbour->SZ)[k*p_Neighbour->GridWidth+1] = ((FLOAT*)p_Current->SZ)[(k+1)*p_Current->GridWidth-2];
						}
					}
				}
			}
		}
	}

	// fix water boundaries

	FLOAT *pCurrentSZ;
	WATER3D_tdst_Patches **p_PatchGrid1, **p_PatchGrid2;

	// top & bottom

	p_PatchGrid1 = p_Water3D->PatchGrid;
	p_PatchGrid2 = p_Water3D->PatchGrid+(p_Water3D->PatchSizeX*(p_Water3D->PatchSizeY-1));
	for(j=0; j<p_Water3D->PatchSizeX; j++, p_PatchGrid1++, p_PatchGrid2++)
	{
		p_Current = *p_PatchGrid1;
		p_Neighbour = *p_PatchGrid2;

		// top
		if(p_Current && !(p_Current->ulFlag & (WATER3D_Static | WATER3D_Culled)))
		{
			pCurrentSZ = (FLOAT*)p_Current->SZ; // first row of patch
			for ( k = 0; k < p_Current->GridWidth; k++ )
				*pCurrentSZ++ = 0.0f; // up = 0
		}

		// bottom
		if(p_Neighbour && !(p_Neighbour->ulFlag & (WATER3D_Static | WATER3D_Culled)))
		{
			pCurrentSZ = ((FLOAT*)p_Neighbour->SZ)+(p_Neighbour->GridWidth*(p_Neighbour->GridHeight-1)); // last row of patch
			for ( k = 0; k < p_Neighbour->GridWidth; k++ )
				*pCurrentSZ++ = 0.0f; // down = 0
		}
	}

	// left & right

	p_PatchGrid1 = p_Water3D->PatchGrid;
	p_PatchGrid2 = p_Water3D->PatchGrid+(p_Water3D->PatchSizeX-1);
	for(j=0; j<p_Water3D->PatchSizeY; j++, p_PatchGrid1+=p_Water3D->PatchSizeX, p_PatchGrid2+=p_Water3D->PatchSizeX)
	{
		p_Current = *p_PatchGrid1;
		p_Neighbour = *p_PatchGrid2;

		// left
		if(p_Current && !(p_Current->ulFlag & (WATER3D_Static | WATER3D_Culled)))
		{
			pCurrentSZ = (FLOAT*)p_Current->SZ; // first column of patch
			for ( k = 0; k < p_Current->GridHeight; k++, pCurrentSZ+= p_Current->GridWidth)
				*pCurrentSZ = 0.0f; // left = 0
		}

		// right
		if(p_Neighbour && !(p_Neighbour->ulFlag & (WATER3D_Static | WATER3D_Culled)))
		{
			pCurrentSZ = ((FLOAT*)p_Neighbour->SZ)+(p_Neighbour->GridWidth-1); // last column of patch
			for ( k = 0; k < p_Neighbour->GridHeight; k++, pCurrentSZ+= p_Neighbour->GridWidth)
				*pCurrentSZ = 0.0f; // right = 0
		}
	}	

	// propagate forces for next frame
	
	BOOL bZeroNeighbourBorder;
	for(i=p_Water3D->PatchSizeY-1; i>=0; i--)
	{
		for(j=0; j<p_Water3D->PatchSizeX; j++)
		{
			if(((p_Current = p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j])==NULL) || (p_Current->ulFlag & WATER3D_Culled)) 
				bZeroNeighbourBorder = TRUE;
			else 
				bZeroNeighbourBorder = FALSE;

			// propagation

			if(i>0 && (p_Neighbour = p_Water3D->PatchGrid[(i-1)*p_Water3D->PatchSizeX+j])!=NULL) // up - down
			{
				if((p_Neighbour->ulFlag & WATER3D_Culled) && !bZeroNeighbourBorder && !(p_Current->ulFlag & WATER3D_Static))
				{
					for ( k = 0; k < p_Current->GridWidth; k++ )
					{
						((FLOAT*)p_Current->SZ)[k] *= 0.9f;
					}
				}
				else if(bZeroNeighbourBorder)
				{
					if(!(p_Neighbour->ulFlag & WATER3D_Static))
					{
						for ( k = 0; k < p_Neighbour->GridWidth; k++ )
						{
							((FLOAT*)p_Neighbour->SZ)[(p_Neighbour->GridHeight-1)*p_Neighbour->GridWidth + k] = 0.0f;
						}
					}
				}
				else if(!(p_Current->ulFlag & WATER3D_Static) || !(p_Neighbour->ulFlag & WATER3D_Static ))
				{
					// inter patch force propagation
					
					if(p_Current->ulFlag & WATER3D_Static)
					{
						for ( k = 0; k < p_Neighbour->GridWidth; k++ )
						{
							// up = neighbour down
							PosNeighbour = ((FLOAT*)p_Neighbour->SZ)[(p_Neighbour->GridHeight-3)*p_Neighbour->GridWidth + k];
							if(fAbs(PosNeighbour) > 0.001f ) 
							{
								if(p_Current->ulFlag & WATER3D_Static)
								{
									p_Current->ulFlag &= ~WATER3D_Static;
#ifdef USE_WATER3D_MEMORY									
									WATER3D_PatchAlloc(p_Water3D, p_Current);
#endif									
								}
								// PosNeighbour=assertfloat(PosNeighbour);
								((FLOAT*)p_Current->SZ)[k] = PosNeighbour;
							}
							else
							{
								((FLOAT*)p_Neighbour->SZ)[(p_Neighbour->GridHeight-1)*p_Neighbour->GridWidth + k] = 0.0f;
								((FLOAT*)p_Neighbour->SZ)[(p_Neighbour->GridHeight-3)*p_Neighbour->GridWidth + k] = 0.0f;
							}
						}
					}
					else if(p_Neighbour->ulFlag & WATER3D_Static)
					{
						for ( k = 0; k < p_Current->GridWidth; k++ )
						{
							// up = neighbour down
							PosCurrent = ((FLOAT*)p_Current->SZ)[p_Current->GridWidth*2 + k];
							if(fAbs(PosCurrent) > 0.001f) 
							{
								if(p_Neighbour->ulFlag & WATER3D_Static)
								{
									p_Neighbour->ulFlag &= ~WATER3D_Static;
#ifdef USE_WATER3D_MEMORY									
									WATER3D_PatchAlloc(p_Water3D, p_Neighbour);
#endif									
								}
								((FLOAT*)p_Neighbour->SZ)[(p_Neighbour->GridHeight-1)*p_Neighbour->GridWidth + k] = PosCurrent;
							}
							else
							{
								((FLOAT*)p_Current->SZ)[p_Current->GridWidth*2 + k] = 0.0f;
								((FLOAT*)p_Current->SZ)[k] = 0.0f;
							}
						}
					}
					else for ( k = 0; k < p_Current->GridWidth; k++ )
					{
						// up = neighbour down
						PosNeighbour = ((FLOAT*)p_Neighbour->SZ)[(p_Neighbour->GridHeight-3)*p_Neighbour->GridWidth + k];
						PosCurrent = ((FLOAT*)p_Current->SZ)[p_Current->GridWidth*2 + k];

						// PosNeighbour=assertfloat(PosNeighbour);
						// PosCurrent=assertfloat(PosCurrent);
						
						((FLOAT*)p_Current->SZ)[k] = PosNeighbour;
						((FLOAT*)p_Neighbour->SZ)[(p_Neighbour->GridHeight-1)*p_Neighbour->GridWidth + k] = PosCurrent;
					}
				}
			}
			
			if(j<p_Water3D->PatchSizeX-1 && (p_Neighbour = p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j+1])!=NULL) // right - left
			{
				if((p_Neighbour->ulFlag & WATER3D_Culled) && !bZeroNeighbourBorder && !(p_Current->ulFlag & WATER3D_Static))
				{
					for ( k = 0; k < p_Current->GridHeight; k++ )
					{
						((FLOAT*)p_Current->SZ)[(k+1)*p_Current->GridWidth - 1] *= 0.9f;
					}
				}
				else if(bZeroNeighbourBorder)
				{
					if(!(p_Neighbour->ulFlag & WATER3D_Static))
					{
						for ( k = 0; k < p_Neighbour->GridHeight; k++ )
						{
							((FLOAT*)p_Neighbour->SZ)[k*p_Neighbour->GridWidth] = 0.0f;
						}
					}
				}
				else if(!(p_Current->ulFlag & WATER3D_Static) || !(p_Neighbour->ulFlag & WATER3D_Static))
				{
					// inter patch force propagation
					
					if(p_Current->ulFlag & WATER3D_Static)
					{
						for (k = 0; k < p_Current->GridHeight; k++ )
						{
							PosNeighbour = ((FLOAT*)p_Neighbour->SZ)[k*p_Neighbour->GridWidth + 2];
							// PosNeighbour=assertfloat(PosNeighbour);
							if(fAbs(PosNeighbour) > 0.001f) 
							{
								if(p_Current->ulFlag & WATER3D_Static)
								{
									p_Current->ulFlag &= ~WATER3D_Static;
#ifdef USE_WATER3D_MEMORY									
									WATER3D_PatchAlloc(p_Water3D, p_Current);
#endif									
								}
								((FLOAT*)p_Current->SZ)[(k+1)*p_Current->GridWidth - 1] = PosNeighbour;
							}
							else
							{
								((FLOAT*)p_Neighbour->SZ)[k*p_Neighbour->GridWidth + 2] = 0.0f;
								((FLOAT*)p_Neighbour->SZ)[k*p_Neighbour->GridWidth] = 0.0f;
							}
						}
					}
					else if(p_Neighbour->ulFlag & WATER3D_Static)
					{
						for (k = 0; k < p_Current->GridHeight; k++ )
						{
							PosCurrent = ((FLOAT*)p_Current->SZ)[(k+1)*p_Current->GridWidth - 3];
							// PosCurrent=assertfloat(PosCurrent);
							if(fAbs(PosCurrent) > 0.001f) 
							{
								if(p_Neighbour->ulFlag & WATER3D_Static)
								{
									p_Neighbour->ulFlag &= ~WATER3D_Static;
#ifdef USE_WATER3D_MEMORY									
									WATER3D_PatchAlloc(p_Water3D, p_Neighbour);
#endif									
								}
								((FLOAT*)p_Neighbour->SZ)[k*p_Neighbour->GridWidth] = PosCurrent;
							}
							else
							{
								((FLOAT*)p_Current->SZ)[(k+1)*p_Current->GridWidth - 3] = 0.0f;
								((FLOAT*)p_Current->SZ)[(k+1)*p_Current->GridWidth - 1] = 0.0f;
							}
						}
					}					
					else for (k = 0; k < p_Current->GridHeight; k++ )
					{
						// right = neighbour left
						PosNeighbour = ((FLOAT*)p_Neighbour->SZ)[k*p_Neighbour->GridWidth + 2];
						PosCurrent = ((FLOAT*)p_Current->SZ)[(k+1)*p_Current->GridWidth - 3];
						
						// PosNeighbour=assertfloat(PosNeighbour);
						// PosCurrent=assertfloat(PosCurrent);
						
						((FLOAT*)p_Current->SZ)[(k+1)*p_Current->GridWidth - 1] = PosNeighbour;
						((FLOAT*)p_Neighbour->SZ)[k*p_Neighbour->GridWidth] = PosCurrent;
					}
				}
			}
		}
	}
#endif //XML_CONV_TOOL
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void WATER3D_Modifier_Apply( MDF_tdst_Modifier	*_pst_Mod,	GEO_tdst_Object	*_pst_Obj )
{
#if !defined(XML_CONV_TOOL)
	WATER3D_tdst_Modifier *p_Water3D;
	p_Water3D = (WATER3D_tdst_Modifier *) _pst_Mod->p_Data;
	WATER3D_tdst_Patches  *p_Patch;
	
	// Activate Water3D 
	if (!p_Water3D->bActive)
	{
		_pst_Mod->pst_GO->pst_Base->pst_Visu->ucFlag|=GRO_VISU_FLAG_WATERFX;

		// add to active 3d water list
		if(!p_Water3D->p_NextActiveWater3D)
		{
			WATER3D_tdst_Modifier *p_CurWater3D = p_FirstActiveWater3D;
			BOOL bFound = FALSE;
			
			while(p_CurWater3D && !bFound) 
			{
				if(p_CurWater3D==p_Water3D)
					bFound = TRUE;
				p_CurWater3D = p_CurWater3D->p_NextActiveWater3D;
			}
			
			if(!bFound)
			{					
				p_Water3D->p_NextActiveWater3D = p_FirstActiveWater3D;
				p_FirstActiveWater3D = p_Water3D;
			}
		}
		
		// find water patch dimensions
		
		MATH_tdst_Vector	stScale;
		MATH_GetScale(&stScale, p_Water3D->p_GO->pst_GlobalMatrix);

		INT i, curX=0, curY=0;
		FLOAT UsedWidth=0, UsedHeight=0;

		LONG		l_NbPoints = _pst_Obj->l_NbPoints;
		GEO_Vertex  * dst_Point = _pst_Obj->dst_Point;
		ULONG* 		dul_VertexColors = p_Water3D->p_GO->pst_Base->pst_Visu->dul_VertexColors;

		p_Water3D->PatchSizeX = 0;
		p_Water3D->PatchSizeY = 0;
        MATH_tdst_Vector oCurAxis, oRefAxis;
        FLOAT fDot;
        
        p_Water3D->fMinX = _pst_Obj->dst_Point[0].x;
        p_Water3D->fMaxY = _pst_Obj->dst_Point[0].y;
        MATH_SubVector(&oRefAxis, &_pst_Obj->dst_Point[1], &_pst_Obj->dst_Point[0]);

        while (TRUE)
        {
            MATH_SubVector(&oCurAxis, &_pst_Obj->dst_Point[p_Water3D->PatchSizeX+1], &_pst_Obj->dst_Point[p_Water3D->PatchSizeX]);
            fDot = MATH_f_DotProduct(&oCurAxis, &oRefAxis);

            if (fDot < 0.0f)
                break;

            p_Water3D->fMaxX = _pst_Obj->dst_Point[p_Water3D->PatchSizeX+1].x;
            p_Water3D->PatchSizeX++;
        }

        if(!p_Water3D->PatchSizeX) 
		{
			return;
		}

        // find MaxY
        INT iCurRow = 0;
        INT iNextRow = (p_Water3D->PatchSizeX+1);
        MATH_SubVector(&oRefAxis, &_pst_Obj->dst_Point[iNextRow], &_pst_Obj->dst_Point[iCurRow]);

        while (TRUE)
        {
            MATH_SubVector(&oCurAxis, &_pst_Obj->dst_Point[iNextRow], &_pst_Obj->dst_Point[iCurRow]);
            fDot = MATH_f_DotProduct(&oCurAxis, &oRefAxis);

            if (fDot < 0.0f)
                break;

            p_Water3D->fMinY = _pst_Obj->dst_Point[(p_Water3D->PatchSizeY+1)*(p_Water3D->PatchSizeX+1)].y;
            p_Water3D->PatchSizeY++;

            iCurRow = iNextRow;
            iNextRow += (p_Water3D->PatchSizeX+1);

            // check for last row
            if (iNextRow >= _pst_Obj->l_NbPoints)
                break;
        }

		p_Water3D->PatchGrid = (WATER3D_tdst_Patches**)MEM_p_Alloc(sizeof(WATER3D_tdst_Patches*)*p_Water3D->PatchSizeX*p_Water3D->PatchSizeY);
		memset(p_Water3D->PatchGrid, 0, sizeof(WATER3D_tdst_Patches*)*p_Water3D->PatchSizeX*p_Water3D->PatchSizeY);

		p_Water3D->bActive = TRUE;

#ifdef ACTIVE_EDITORS
		INT j;
		p_Water3D->iNbPoly = 0;
#endif
		for(i=0; i<l_NbPoints; i++)
		{
			{
				if(curX==p_Water3D->PatchSizeX) // change row
				{
					curX=0;
					curY++;
					continue;
				}

				if(curY==p_Water3D->PatchSizeY) // end reached
					break;

				// if a patch is desired (alpha non zero)
				if(!dul_VertexColors || dul_VertexColors[i+1] & 0xFF000000)
				{
					// patch delimitations

					p_Patch = p_Water3D->PatchGrid[curY*p_Water3D->PatchSizeX+curX] = (WATER3D_tdst_Patches*)MEM_p_Alloc(sizeof(WATER3D_tdst_Patches));
                    
                    // X min/max
					p_Patch->fMinX = dst_Point[i].x; 
					p_Patch->fMaxX = dst_Point[i+1].x;
					p_Patch->fZMinMin = dst_Point[i].z;	
					p_Patch->fZMaxMin = dst_Point[i+1].z;	

					// Y min/max
					p_Patch->fMinY = dst_Point[i].y;
					p_Patch->fMaxY = dst_Point[i+p_Water3D->PatchSizeX+1].y;
					p_Patch->fZMinMax = dst_Point[i+p_Water3D->PatchSizeX+1].z;	
					p_Patch->fZMaxMax = dst_Point[i+p_Water3D->PatchSizeX+2].z;	

#ifdef _XENON_RENDER
                    // Init the patch info
                    p_Patch->m_ulPatchInfoId = g_oWaterManager.RegisterWaterPatch(p_Water3D, p_Patch);
                    ERR_X_Assert(p_Patch->m_ulPatchInfoId != -1);
#endif

                    // get min and max in global space for dirturber optimisations
					MATH_tdst_Vector	vPos;
					vPos.x = p_Patch->fMinX; vPos.y = p_Patch->fMinY; vPos.z = p_Patch->fZMinMin;					
					MATH_TransformVertex(&vPos, p_Water3D->p_GO->pst_GlobalMatrix, &vPos);
					p_Patch->fMinXGlobal = vPos.x; p_Patch->fMinYGlobal = vPos.y;					
					vPos.x = p_Patch->fMaxX; vPos.y = p_Patch->fMaxY; vPos.z = p_Patch->fZMaxMax;					
					MATH_TransformVertex(&vPos, p_Water3D->p_GO->pst_GlobalMatrix, &vPos);
					p_Patch->fMaxXGlobal = vPos.x; p_Patch->fMaxYGlobal = vPos.y;
					
					// forces equilibrium factor (will toggle 1, -1) for perturbances
					p_Patch->TurbulanceGuide = 1; 

					// some variables init
					p_Patch->fLastTurbulance = 0.0f;
					p_Patch->nCurrentDisturbance = 0;					
					p_Patch->ulFlag = WATER3D_Static;
					
					// grid : add 2 to each dimensions to use as a border computing buffer
					if(curY==0 || !p_Water3D->PatchGrid[(curY-1)*p_Water3D->PatchSizeX+curX])
					{
						p_Patch->GridWidth = (UCHAR) (2 + fAbs((p_Water3D->Density/100.0f)*(p_Patch->fMaxX-p_Patch->fMinX)*stScale.x));
						if(p_Patch->GridWidth<=3) 
							p_Patch->GridWidth = 4;
					}
					else // keep same width for entire column
					{
						p_Patch->GridWidth = p_Water3D->PatchGrid[(curY-1)*p_Water3D->PatchSizeX+curX]->GridWidth;
					}
					
					if(curX==0 || !p_Water3D->PatchGrid[curY*p_Water3D->PatchSizeX+curX-1])
					{
						p_Patch->GridHeight = (UCHAR) (2 + fAbs((p_Water3D->Density/100.0f)*(p_Patch->fMaxY-p_Patch->fMinY)*stScale.y));
						if(p_Patch->GridHeight<=3) 
							p_Patch->GridHeight = 4;
					}
					else // keep same height for entire line
					{
						p_Patch->GridHeight = p_Water3D->PatchGrid[curY*p_Water3D->PatchSizeX+curX-1]->GridHeight;
					}

					// static computing buffer max memory limiter
					while(p_Patch->GridHeight*p_Patch->GridWidth > (MAX_PATCH_SIZE*MAX_PATCH_SIZE)) // limit the memory taken by one patch or water surface
					{
						if(curX>0)
							p_Patch->GridWidth--;
						else if (curY>0)
							p_Patch->GridHeight--;
						else
						{
							p_Patch->GridWidth--;
							p_Patch->GridHeight--;
						}
						
						if(p_Patch->GridWidth<4) p_Patch->GridWidth = 4;
						if(p_Patch->GridHeight<4) p_Patch->GridHeight = 4;
					}
					
#ifdef PSX2_TARGET
					// VERY IMPORTANT !!!
					p_Patch->GridWidth = (p_Patch->GridWidth + 3) & (~3);
#endif
					

#ifdef USE_WATER3D_MEMORY
					p_Patch->VZ = NULL;
					p_Patch->SZ = NULL;
#else

					p_Patch->SZ = (void*)MEM_p_AllocAlign(p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT), 32);
					p_Patch->VZ = (void*)MEM_p_AllocAlign(p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT), 32);
					memset(p_Patch->VZ, 0, p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT));
					memset(p_Patch->SZ, 0, p_Patch->GridWidth*p_Patch->GridHeight*sizeof(FLOAT));
#endif

#ifdef ACTIVE_EDITORS
					p_Water3D->iNbPoly += (p_Patch->GridWidth-2)*(p_Patch->GridHeight-2)*2;
#endif

					// geo object creation for graphinc pipeline rendering
					WATER3D_CreateObject(p_Water3D, p_Patch);
				}

				curX++;
			}
		}

#ifdef ACTIVE_EDITORS

		// validate patch system
		WATER3D_tdst_Patches  *p_Patch2;
		for(i=0; i<p_Water3D->PatchSizeY; i++)
		{
			for(j=0; j<p_Water3D->PatchSizeX; j++)
			{
				// NULL when patch has been removed with vertex alpha
				if((p_Patch = p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j])==NULL)
					continue;

				if(j<p_Water3D->PatchSizeX-1) // right compare
				{
					if((p_Patch2=p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j+1])!=NULL)
					{
						ERR_X_Assert(fEqWithEpsilon(p_Patch->fMaxX, p_Patch2->fMinX, (float) 1E-3 ));
						ERR_X_Assert(fEqWithEpsilon(p_Patch->fMinY, p_Patch2->fMinY, (float) 1E-3 ));
						ERR_X_Assert(fEqWithEpsilon(p_Patch->fMaxY, p_Patch2->fMaxY, (float) 1E-3 ));
						ERR_X_Assert(p_Patch->fZMaxMin == p_Patch2->fZMinMin);
						ERR_X_Assert(p_Patch->fZMaxMax == p_Patch2->fZMinMax);
					}
					else
						continue;
				}
				
				if(j>0 ) // left compare
				{
					if((p_Patch2=p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j-1])!=NULL)
					{
						ERR_X_Assert(fEqWithEpsilon(p_Patch->fMinX, p_Patch2->fMaxX, (float) 1E-3 ));
						ERR_X_Assert(fEqWithEpsilon(p_Patch->fMinY, p_Patch2->fMinY, (float) 1E-3 ));
						ERR_X_Assert(fEqWithEpsilon(p_Patch->fMaxY, p_Patch2->fMaxY, (float) 1E-3 ));
						ERR_X_Assert(p_Patch->fZMinMin == p_Patch2->fZMaxMin);
						ERR_X_Assert(p_Patch->fZMinMax == p_Patch2->fZMaxMax);
					}
					else
						continue;
				}
  
				if(i<p_Water3D->PatchSizeY-1 ) // up compare
				{
					if((p_Patch2=p_Water3D->PatchGrid[(i+1)*p_Water3D->PatchSizeX+j])!=NULL)
					{
						ERR_X_Assert(fEqWithEpsilon(p_Patch->fMinX, p_Patch2->fMinX, (float) 1E-3 ));
						ERR_X_Assert(fEqWithEpsilon(p_Patch->fMaxX, p_Patch2->fMaxX, (float) 1E-3 ));
						ERR_X_Assert(fEqWithEpsilon(p_Patch->fMaxY, p_Patch2->fMinY, (float) 1E-3 ));
						ERR_X_Assert(p_Patch->fZMinMax == p_Patch2->fZMinMin);
						ERR_X_Assert(p_Patch->fZMaxMax == p_Patch2->fZMaxMin);
					}
					else
						continue;
				}

				if(i>0) // down compare
				{
					if((p_Patch2=p_Water3D->PatchGrid[(i-1)*p_Water3D->PatchSizeX+j])!=NULL)
					{
						ERR_X_Assert(fEqWithEpsilon(p_Patch->fMinX, p_Patch2->fMinX, (float) 1E-3 ));
						ERR_X_Assert(fEqWithEpsilon(p_Patch->fMaxX, p_Patch2->fMaxX, (float) 1E-3 ));
						ERR_X_Assert(fEqWithEpsilon(p_Patch->fMinY, p_Patch2->fMaxY, (float) 1E-3 ));
						ERR_X_Assert(p_Patch->fZMinMin == p_Patch2->fZMinMax);
						ERR_X_Assert(p_Patch->fZMaxMin == p_Patch2->fZMaxMax);
					}
					else
						continue;
				}
			}
		} 
#endif // ACTIVE_EDITORS
	}
	else // water is active ... update it and all adjacent patches
	{
		WATER3D_Modifier_ComputePatches(p_Water3D);	
	}
#endif //XML_CONV_TOOL
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void WATER3D_Modifier_Unapply( MDF_tdst_Modifier *_pst_Mod,	GEO_tdst_Object	*_pst_Obj )
{ 
}

/*
=======================================================================================================================
=======================================================================================================================
*/
ULONG WATER3D_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	WATER3D_tdst_Modifier			*pst_Data;
	ULONG							ul_Version;
	unsigned char					*pc_Cur;

	pc_Cur = (UCHAR*)_pc_Buffer;
	pst_Data = (WATER3D_tdst_Modifier *) _pst_Mod->p_Data;
	
	ul_Version = LOA_ReadLong((CHAR**)&pc_Cur);
#if defined(XML_CONV_TOOL)
	gGaoWater3DMdfVersion = ul_Version;
#endif
	
#if defined(XML_CONV_TOOL)
	LOA_ReadMatrix((CHAR**)&pc_Cur, &matDummyMatrix);
#else
	MATH_tdst_Matrix    FixChromeMatrix;
	LOA_ReadMatrix((CHAR**)&pc_Cur, &FixChromeMatrix);
#endif

	pst_Data->fDamping = LOA_ReadFloat((CHAR**)&pc_Cur);
	pst_Data->fPropagationSpeed = LOA_ReadFloat((CHAR**)&pc_Cur);
	pst_Data->fPerturbanceAmplitudeModifier = LOA_ReadFloat((CHAR**)&pc_Cur);
	pst_Data->fImpactForceAttenuation = LOA_ReadFloat((CHAR**)&pc_Cur);
	pst_Data->fTurbulanceAmplitude = LOA_ReadFloat((CHAR**)&pc_Cur);
	pst_Data->lTurbulanceFactor = LOA_ReadLong((CHAR**)&pc_Cur);
	pst_Data->Density = LOA_ReadInt((CHAR**)&pc_Cur);


	pst_Data->fRadius = LOA_ReadFloat((CHAR**)&pc_Cur);
	pst_Data->fDampingOutsideRadius = LOA_ReadFloat((CHAR**)&pc_Cur);
	pst_Data->bTurbulanceOffIfOutsideRadius = LOA_ReadULong((CHAR**)&pc_Cur);
	pst_Data->fRadiusCut = LOA_ReadFloat((CHAR**)&pc_Cur);
	
	if (ul_Version>=4)
	{
		pst_Data->bWaterChrome = LOA_ReadInt((CHAR**)&pc_Cur);
	}

	if(ul_Version<3)
	{
		pst_Data->fRadius = 0.0f;
		pst_Data->fDampingOutsideRadius = pst_Data->fDamping;
		pst_Data->bTurbulanceOffIfOutsideRadius = FALSE;
		pst_Data->fRadiusCut = 0.0f;
	}

    if (ul_Version >= 5)
    {
        pst_Data->fRefractionIntensity = LOA_ReadFloat((CHAR**)&pc_Cur);
        pst_Data->fReflectionIntensity = LOA_ReadFloat((CHAR**)&pc_Cur);
    }

    if (ul_Version >= 6)
    {
        pst_Data->fWaterDensity = LOA_ReadFloat((CHAR**)&pc_Cur);
    }

    if (ul_Version >= 7)
    {
        pst_Data->fBaseMapOpacity = LOA_ReadFloat((CHAR**)&pc_Cur);
        pst_Data->fMossMapOpacity = LOA_ReadFloat((CHAR**)&pc_Cur);
    }

    if (ul_Version >= 8)
    {
        pst_Data->fFogIntensity = LOA_ReadFloat((CHAR**)&pc_Cur);
    }
    else
    {
        pst_Data->fFogIntensity = 1.0f;
    }

#if defined(_XENON)
	// Clamp values right away, as the shader constants update is not going to check on Xenon

	pst_Data->fReflectionIntensity = min(pst_Data->fReflectionIntensity, 1.0f);
	pst_Data->fReflectionIntensity = max(pst_Data->fReflectionIntensity, 0.0f);

	pst_Data->fRefractionIntensity = min(pst_Data->fRefractionIntensity, 1.0f);
	pst_Data->fRefractionIntensity = max(pst_Data->fRefractionIntensity, 0.0f);

	pst_Data->fWaterDensity        = max(pst_Data->fWaterDensity, 0.0f);

	pst_Data->fBaseMapOpacity      = min(pst_Data->fBaseMapOpacity, 1.0f);
	pst_Data->fBaseMapOpacity      = max(pst_Data->fBaseMapOpacity, 0.0f);

	pst_Data->fFogIntensity        = max(pst_Data->fFogIntensity, 0.0f);
#endif

	pst_Data ->p_GO = _pst_Mod->pst_GO;

#ifdef ACTIVE_EDITORS
    pst_Data->ulCodeKey = 0xC0DE2001;
#endif

	return pc_Cur - (UCHAR*)_pc_Buffer;
}


#ifdef ACTIVE_EDITORS

/*
=======================================================================================================================
=======================================================================================================================
*/
void WATER3D_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
	WATER3D_tdst_Modifier			*pst_Data;
	ULONG							ul_Version;

	pst_Data = (WATER3D_tdst_Modifier *) _pst_Mod->p_Data;

	// Save version
#if defined(XML_CONV_TOOL)
	ul_Version = gGaoWater3DMdfVersion;
#else
	ul_Version = 8;
#endif
	SAV_Buffer(&ul_Version, 4);

#if defined(XML_CONV_TOOL)
	SAV_Buffer(&matDummyMatrix, sizeof(MATH_tdst_Matrix));
#else
	MATH_tdst_Matrix matDummy;
	memset(&matDummy, 0, sizeof(matDummy));
	SAV_Buffer(&matDummy, sizeof(MATH_tdst_Matrix));
#endif
		
	SAV_Buffer(&pst_Data->fDamping, sizeof(FLOAT));
	SAV_Buffer(&pst_Data->fPropagationSpeed, sizeof(FLOAT));
	SAV_Buffer(&pst_Data->fPerturbanceAmplitudeModifier, sizeof(FLOAT));
	SAV_Buffer(&pst_Data->fImpactForceAttenuation, sizeof(FLOAT));
	SAV_Buffer(&pst_Data->fTurbulanceAmplitude, sizeof(FLOAT));
	SAV_Buffer(&pst_Data->lTurbulanceFactor, sizeof(LONG));
	SAV_Buffer(&pst_Data->Density, sizeof(INT));

	SAV_Buffer(&pst_Data->fRadius, sizeof(FLOAT));
	SAV_Buffer(&pst_Data->fDampingOutsideRadius, sizeof(FLOAT));
	SAV_Buffer(&pst_Data->bTurbulanceOffIfOutsideRadius, sizeof(ULONG));
	SAV_Buffer(&pst_Data->fRadiusCut, sizeof(FLOAT));

    SAV_Buffer(&pst_Data->bWaterChrome, sizeof(INT));
	SAV_Buffer(&pst_Data->fRefractionIntensity, sizeof(FLOAT));
	SAV_Buffer(&pst_Data->fReflectionIntensity, sizeof(FLOAT));
	SAV_Buffer(&pst_Data->fWaterDensity, sizeof(FLOAT));
	SAV_Buffer(&pst_Data->fBaseMapOpacity, sizeof(FLOAT));
	SAV_Buffer(&pst_Data->fMossMapOpacity, sizeof(FLOAT));

    if (ul_Version >= 8)
    {
	    SAV_Buffer(&pst_Data->fFogIntensity, sizeof(FLOAT));
    }
}

#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		DISPLAY FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
=======================================================================================================================
=======================================================================================================================
*/
void WATER3D_BeforeDraw()
{ 
	WATER3D_tdst_Modifier *p_Water3D;
	p_Water3D = p_FirstActiveWater3D;

#ifdef ACTIVE_EDITORS
	g_iWaterMemory = 0;
#endif		

	while (p_Water3D)
	{	
#ifdef ACTIVE_EDITORS
		WATER3D_Modifier_MemoryCompute(p_Water3D);
#endif		
		p_Water3D->bDrawn = FALSE;
		p_Water3D->bUpdated = FALSE;
		p_Water3D = p_Water3D->p_NextActiveWater3D;
	}
	
#ifdef ACTIVE_EDITORS
	ERR_X_Warning(((g_iWaterMemory < WATER3D_MEMORY_SIZE)||(g_iWaterMemory<=g_iWaterMemoryMax)), "There is not enough memory for that much water. Try playing with the modifier Radius property to limit the memory taken\n", NULL);
	if(g_iWaterMemory > WATER3D_MEMORY_SIZE) // show message only once
		g_iWaterMemoryMax = -1;
#endif	
}


/*
=======================================================================================================================
=======================================================================================================================
*/
void WATER3D_ComputeNormals(WATER3D_tdst_Patches *p_Patch, FLOAT	*vert, MATH_tdst_Vector *normal	)
{
    int i;
    int j;
    int m = p_Patch->GridWidth;
    int n = p_Patch->GridHeight;

	MATH_tdst_Vector   *pNormals = normal;

	vert += m;

    for (i=1; i<n-1; i++)
    {
		vert++; // skip first

        for (j=1; j<m-1; j++)
        {			

        	pNormals->z = 1.0f;
         
        	pNormals->y = ((vert[-1])-(vert[1]))*4.0f; // w - e
        	pNormals->x = ((vert[m])-(vert[-m]))*4.0f; // s - n
        	pNormals++;

         	vert++;
       	}

		vert++; // skip last
	}
}


/*
=======================================================================================================================
=======================================================================================================================
*/
WATER3D_tdst_Modifier* WATER3D_ModifierFind(OBJ_tdst_GameObject *_pst_GO)
{
	WATER3D_tdst_Modifier	*p_Water3D;
	p_Water3D = p_FirstActiveWater3D;

	while (p_Water3D)
	{
		if(p_Water3D->p_GO == _pst_GO) break;
		p_Water3D = p_Water3D->p_NextActiveWater3D;
	}

	return p_Water3D;
}

void WATER3D_Modifier_DisplayPatch(OBJ_tdst_GameObject *_pst_GO, WATER3D_tdst_Modifier *p_Water3D, WATER3D_tdst_Patches * p_Patch, WATER3D_tdst_Patches * p_Left, WATER3D_tdst_Patches * p_Right, WATER3D_tdst_Patches * p_Up, WATER3D_tdst_Patches * p_Down)
{
#if !defined(XML_CONV_TOOL)
	INT						i,j;
	GEO_Vertex				*p_Src;
	MATH_tdst_Vector        *pst_Normal;	

	FLOAT *SZ = (FLOAT*)p_Patch->SZ;

	// set object to use when in editor mode (default) --> other platforms draw the water object directly...	
	GEO_tdst_Object *pst_Object = &p_Patch->GEO_Object;
	GDI_gpst_CurDD->pst_CurrentGeo = pst_Object;

	pCur3DWaterPatch = p_Patch;

	// get and init normals
	pst_Normal = pst_Object->dst_PointNormal;

	p_Src = pst_Object->dst_Point;
	GDI_gpst_CurDD->p_Current_Vertex_List = p_Src;

	// update vertex for object
	FLOAT	*pSZ = &SZ[p_Patch->GridWidth];
	p_Src = pst_Object->dst_Point;

	if ((p_Patch->ulFlag & WATER3D_Static) == 0)
    {
        FLOAT curZ, startZ;
	    FLOAT stepZX,stepZX1,stepZX2,stepZXX;
	    FLOAT stepZY;

	    startZ = curZ = p_Patch->fZMinMin;	
	    stepZX1 = ( p_Patch->fZMaxMin - p_Patch->fZMinMin ) / (p_Patch->GridWidth-3);
	    stepZX2 = ( p_Patch->fZMaxMax - p_Patch->fZMinMax ) / (p_Patch->GridWidth-3);
	    stepZY = ( p_Patch->fZMinMax - p_Patch->fZMinMin ) / (p_Patch->GridHeight-3);
	    stepZXX = (stepZX2 - stepZX1) / (p_Patch->GridHeight-3);
	    stepZX = stepZX1;

    #ifdef _XENON_RENDER
        if (GDI_b_IsXenonGraphics())
        {
            struct WaterVertex
            {
                FLOAT Nx,Ny,Nz;
                FLOAT Tx,Ty,Tz,z;
            };

            WaterVertex *pVertexBuffer = (WaterVertex*) p_Patch->m_poWaterMesh->GetStream(1)->pBuffer->Lock(p_Patch->GEO_Object.l_NbPoints, 7*sizeof(FLOAT), FALSE, TRUE);
            WaterVertex *pVertex = pVertexBuffer;

		    FLOAT *vert = SZ;

 		    vert += p_Patch->GridWidth;

 		    for (i = 1; i < p_Patch->GridHeight - 1; i++)
		    {
			    vert++; // skip first

			    pSZ++; // skip first			

			    curZ = startZ + stepZY*i;		
			    stepZX += stepZXX;				

			    for (j = 1; j < p_Patch->GridWidth - 1; j++, pVertex++, pSZ++)
			    {		
				    pVertex->z = (*pSZ)+curZ;	
				    curZ += stepZX;				

				    pVertex->Nx = ((vert[p_Patch->GridWidth])-(vert[-p_Patch->GridWidth]))*4.0f; // s - n
				    pVertex->Ny = ((vert[-1])-(vert[1]))*4.0f; // w - e
				    pVertex->Nz = 1.0f;

				    pVertex->Tx = 1.0f;
				    pVertex->Ty = 0.0f;
				    pVertex->Tz = -pVertex->Nx / pVertex->Nz;
                         
				    vert++;
			    }

			    vert++; // skip last

			    pSZ++; // skip last	
		    }

            p_Patch->m_poWaterMesh->GetStream(1)->pBuffer->Unlock();
        }
        else
    #endif
        {
            for ( i = 1; i < (p_Patch->GridHeight-1); i++)
            {
                pSZ++; // skip first

                curZ = startZ + stepZY*i;
                stepZX += stepZXX;
                for ( j = 1; j < (p_Patch->GridWidth-1); j++,p_Src++,pSZ++)
                {
                    p_Src->z = (*pSZ)+curZ;
                    curZ += stepZX;
                }

                pSZ++; // skip last
            }
        	
            WATER3D_ComputeNormals( p_Patch, SZ, pst_Object->dst_PointNormal);
        }
    }
		
	// draw geometry
	
	//MAT_DrawIndexedTriangle(pst_Object, (MAT_tdst_Material *) _pst_GO->pst_Base->pst_Visu->pst_Material, pst_Object->dst_Element);
	{
        MAT_tdst_Material	*_pst_Material = (MAT_tdst_Material *) _pst_GO->pst_Base->pst_Visu->pst_Material;

        if(_pst_Material  && (_pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti))
        {
            if(((MAT_tdst_Multi *)_pst_Material)->l_NumberOfSubMaterials == 0)
                _pst_Material = NULL;
            else
                _pst_Material = (MAT_tdst_Material*) ((MAT_tdst_Multi *)_pst_Material)->dpst_SubMaterial[lMin(pst_Object->dst_Element->l_MaterialId , ((MAT_tdst_Multi *)_pst_Material)->l_NumberOfSubMaterials - 1)];
        }

        if(_pst_Material == NULL)
        {
            _pst_Material = (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;
        }

#ifdef _XENON_RENDER
        if (GDI_b_IsXenonGraphics())
        {
            MAT_tdst_MultiTexture	*pst_MLTTX = (MAT_tdst_MultiTexture *) _pst_Material;
            MAT_tdst_MTLevel		*pst_MLTTXLVL = pst_MLTTX->pst_FirstLevel;

            if(_pst_Material == NULL) _pst_Material= (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;

            if (pst_MLTTXLVL != NULL)
            {
                while (pst_MLTTXLVL && (LONG)pst_MLTTXLVL != -1)
                {
                    if(!(pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_InActive))
                    {
                        XeMesh *poWaterMesh = (p_Patch->ulFlag & WATER3D_Static) ? p_Patch->m_poStaticWaterMesh : p_Patch->m_poWaterMesh;

                        g_oWaterManager.AddPatch(p_Patch->m_ulPatchInfoId, poWaterMesh);

                      #ifndef _XENON
                        // always update the modifier info in editor since it can change anytime via the interface
                        g_oWaterManager.UpdateModifierParams(p_Patch->m_ulPatchInfoId, p_Water3D);
                      #endif

                        g_oWaterManager.InvalidatePatchInfo(p_Patch->m_ulPatchInfoId);
                    }

                    pst_MLTTXLVL = pst_MLTTXLVL->pst_NextLevel;
                }
            }
        }
        else
        {            
            MAT_DrawIndexedTriangle_MT(&GDI_gpst_CurDD_SPR , pst_Object,_pst_Material,pst_Object->dst_Element);
        }
#elif defined(ACTIVE_EDITORS)
		MAT_DrawIndexedTriangle_MT(&GDI_gpst_CurDD_SPR , pst_Object,_pst_Material,pst_Object->dst_Element);	
#endif

		GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ulColorXor = 0;
	}
#endif //XML_CONV_TOOL
}

void WATER3D_Modifier_Display(OBJ_tdst_GameObject *_pst_GO)
{
#if !defined(XML_CONV_TOOL)
    WATER3D_tdst_Modifier	*p_Water3D;
	int						iIndex;
	int 					bSomeThingWasDrawn=0;
	
  #ifdef _XENON_RENDER
    g_oWaterManager.BeginAddPatch();
  #endif

    pCur3DWater = p_Water3D = WATER3D_ModifierFind(_pst_GO);

	if(!p_Water3D) return;

    if((pCur3DWater->pst_Mod->ul_Flags & MDF_C_Modifier_Inactive))
	{
		pCur3DWater->p_GO->pst_Base->pst_Visu->ucFlag &= ~GRO_VISU_FLAG_WATERFX;
		WATER3D_Modifier_Reset( pCur3DWater );
		return;
	}

	if(!p_Water3D->bUpdated || p_Water3D->bDrawn) return;
	
	p_Water3D->bDrawn = TRUE;

    WATER3D_tdst_Patches *pCurrentPatch = NULL;

	for(int i=0; i<p_Water3D->PatchSizeY; i++)
	{
		iIndex=i*p_Water3D->PatchSizeX;
		for(int j=0; j<p_Water3D->PatchSizeX; j++,iIndex++)
		{
			if(!p_Water3D->PatchGrid[iIndex] || (p_Water3D->PatchGrid[iIndex]->ulFlag & WATER3D_Culled)) 
				continue;
				
			bSomeThingWasDrawn=1;
			 
			WATER3D_Modifier_DisplayPatch(_pst_GO, p_Water3D,	p_Water3D->PatchGrid[iIndex], 
																(j>0 ? p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j-1] : NULL), 
																(j<p_Water3D->PatchSizeX-1 ? p_Water3D->PatchGrid[i*p_Water3D->PatchSizeX+j+1] : NULL), 
																(i>0 ? p_Water3D->PatchGrid[(i-1)*p_Water3D->PatchSizeX+j] : NULL), 
																(i<p_Water3D->PatchSizeY-1 ? p_Water3D->PatchGrid[(i+1)*p_Water3D->PatchSizeX+j] : NULL));

            pCurrentPatch = p_Water3D->PatchGrid[iIndex];
		}
    }

#ifdef _XENON_RENDER
    if (GDI_b_IsXenonGraphics() && pCurrentPatch)
    {
    	LONG l_MaterialId  = ((GEO_tdst_Object*)_pst_GO->pst_Base->pst_Visu->pst_Object)->dst_Element[0].l_MaterialId;
        MAT_tdst_Material	*_pst_Material = (MAT_tdst_Material *) _pst_GO->pst_Base->pst_Visu->pst_Material;

        if(_pst_Material  && (_pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti))
        {
            if(((MAT_tdst_Multi *)_pst_Material)->l_NumberOfSubMaterials == 0)
                _pst_Material = NULL;
            else
                _pst_Material = (MAT_tdst_Material*) ((MAT_tdst_Multi *)_pst_Material)->dpst_SubMaterial[lMin(l_MaterialId, ((MAT_tdst_Multi *)_pst_Material)->l_NumberOfSubMaterials - 1)];
        }

        if(_pst_Material == NULL)
        {
            _pst_Material = (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;
        }

        MAT_tdst_MultiTexture	*pst_MLTTX = (MAT_tdst_MultiTexture *) _pst_Material;
        MAT_tdst_MTLevel		*pst_MLTTXLVL = pst_MLTTX->pst_FirstLevel;

        if(_pst_Material == NULL) _pst_Material= (MAT_tdst_Material *) &MAT_gst_DefaultSingleMaterial;

        if (pst_MLTTXLVL != NULL)
        {
            while (pst_MLTTXLVL && (LONG)pst_MLTTXLVL != -1)
            {
                if(!(pst_MLTTXLVL->ul_Flags & MAT_Cul_Flag_InActive))
                {
                    g_oXeRenderer.QueueMeshForRender(   _pst_GO->pst_GlobalMatrix,
                                                        pCurrentPatch->m_poStaticWaterMesh, // use any mesh, will be changed anyway
                                                        pst_MLTTXLVL->pst_XeMaterial,
                                                        GDI_gpst_CurDD->ul_CurrentDrawMask,
                                                        -1,
                                                        XeRT_WATER,
                                                        XeRenderObject::TriangleList,
                                                        _pst_GO->uc_LOD_Vis,
                                                        0,
                                                        _pst_GO, 
                                                        XeRenderObject::Water,
                                                        0,
                                                        GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Lighted ? QMFR_LIGHTED : 0);
                }

                pst_MLTTXLVL = pst_MLTTXLVL->pst_NextLevel;
            }
        }
    }
  #endif

#endif
}



