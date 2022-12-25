/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "GDInterface/GDIrasters.h" 
#include "GX8init.h"
#include "GX8tex.h"
#include "GEOmetric/GEO_STRIP.h"
#include "xgraphics.h" //temporaire !!

#define UseVertexBuffer

/*$4
 ***********************************************************************************************************************
    Private and external function prototype
 ***********************************************************************************************************************
 */

extern void LOA_BeginSpeedMode(BIG_KEY _ul_Key);
extern void LOA_EndSpeedMode(void);
extern BOOL GDI_gb_WaveSprite;

BOOL		OGL_gb_DispStrip;
BOOL		OGL_gb_DispLOD;


/*$4
 ***********************************************************************************************************************
    Public Function
 ***********************************************************************************************************************
 */
extern int					NbrFurLayers ;
extern float				OffsetNormal;
extern float				OffsetU ;
extern float				OffsetV ;
extern int					FurInvertAlpha;


//#ifndef UseVertexBuffer
#define Gx8_M_ConvertColor(a) (a & 0xff00ff00) | ((a & 0xff) << 16) | ((a & 0xff0000) >> 16)


#define GX8_SetColorRGBA(a) \
	if(pst_Color) \
	{ \
		ulOGLSetCol = pst_Color[a] | ulOGLSetCol_Or; \
		ulOGLSetCol ^= ulOGLSetCol_XOr; \
		if(pst_Alpha) \
		{ \
			ulOGLSetCol &= 0x00ffffff; \
			ulOGLSetCol |= pst_Alpha[a]; \
		} \
		IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol)); \
 	} \
	else if(pst_Alpha) \
	{ \
		ulOGLSetCol &= 0x00ffffff; \
		ulOGLSetCol |= pst_Alpha[a]; \
		IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol)); \
 	}
/*
#define GX8_SetColorRGBA(a) \
	{ \
		ulOGLSetCol = pst_Color[a] | ulOGLSetCol_Or; \
		ulOGLSetCol ^= ulOGLSetCol_XOr; \
			ulOGLSetCol &= 0x00ffffff; \
			ulOGLSetCol |= pst_Alpha[a]; \
		IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol)); \
 	} \
*/
#define GX8_SetVertexXYZ(P,N)\
	if ((P) && (N))\
		if (pst_Color){\
			ulOGLSetCol = (ulOGLSetCol)>>4*6;\
			offsetN2 = offsetN*ulOGLSetCol / 256.0f;\
			tempXYZ[0] = ((float*)(P))[0] + offsetN2*((float*)(N))[0];\
			tempXYZ[1] = ((float*)(P))[1] + offsetN2*((float*)(N))[1];\
			tempXYZ[2] = ((float*)(P))[2] + offsetN2*((float*)(N))[2];\
			IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, (float*)P[0], P[1], P[2], 1.0f );\
		}else{\
			tempXYZ[0] = ((float*)(P))[0] + offsetN*((float*)(N))[0];\
			tempXYZ[1] = ((float*)(P))[1] + offsetN*((float*)(N))[1];\
			tempXYZ[2] = ((float*)(P))[2] + offsetN*((float*)(N))[2];\
			IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, (float*)P[0], P[1], P[2], 1.0f );\
		}



#define GX8_SetVertexUV(uv)\
		{\
			tempXYZ[0] = ((float*) (uv))[0] + offSetU;\
			tempXYZ[1] = ((float*) (uv))[1] + offSetV;\
			glTexCoord2fv(tempXYZ);\
		}
//#endif

#define GX8_TestHideTriangle(t)


//#ifndef UseVertexBuffer
// rendering without vertex buffer
LONG GX8_l_ShiftDrawElementIndexedTriangles // STRIP VERSION
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	MATH_tdst_Vector					*_pst_Normal,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints
)
{
	Gx8_tdst_SpecificData		*pst_SD;
	GEO_tdst_IndexedTriangle	*t, *tend;
	ULONG						*pst_Color;
	ULONG						*pst_Alpha;
	ULONG						ulDrwMskDelta;
	ULONG						ulOGLSetCol_Or;
	ULONG						ulOGLSetCol_XOr;
	ULONG						ulOGLSetCol;
	BOOL						bStrip;
	USHORT						auw_Index, auw_UV;
	GEO_tdst_OneStrip			*pStrip, *pStripEnd;
	float						offsetN, offSetU, offSetV;//, offsetN2;
	int							in;
	BOOL						i_Alphatest;
	int							i_Alphafunc;
	ULONG						f_Alpharef;




 	pst_SD = (Gx8_tdst_SpecificData *) GDI_gpst_CurDD->pv_SpecificData;
	t = _pst_Element->dst_Triangle;
	tend = t + _pst_Element->l_NbTriangles;

	ulOGLSetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
	ulOGLSetCol_XOr ^= 0xff000000;

	ulDrwMskDelta = GDI_gpst_CurDD->LastDrawMask ^ GDI_gpst_CurDD->ul_CurrentDrawMask;
	pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
	pst_Alpha = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentAlphaField;
	ulOGLSetCol_Or = pst_SD->ulColorOr;

	Gx8_RS_DrawWired(&pst_SD->st_RS, !(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired));

	Gx8_RS_DepthTest( &pst_SD->st_RS, GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest );
	Gx8_RS_Fogged(&pst_SD->st_RS, ((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged) && (pst_SD->ulFogState)));
	GDI_gpst_CurDD->LastDrawMask = GDI_gpst_CurDD->ul_CurrentDrawMask;
	//Gx8_vSetVertexShader( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

	//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_NONE );  //Force culling

	// save alpha test context
	IDirect3DDevice8_GetRenderState((D3DDevice*)pst_SD, D3DRS_ALPHATESTENABLE ,&i_Alphatest);
	if ( i_Alphatest )	
	{
	IDirect3DDevice8_GetRenderState((D3DDevice*)pst_SD, D3DRS_ALPHAFUNC ,&i_Alphafunc);
	IDirect3DDevice8_GetRenderState((D3DDevice*)pst_SD, D3DRS_ALPHAREF ,&f_Alpharef);
	}


    //IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
    //IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
 
	if(_pst_Element->pst_StripData != NULL)
	{
		if(_pst_Element->pst_StripData->ulFlag & GEO_C_Strip_DataValid)
			bStrip = TRUE;
		else
			bStrip = FALSE;
	}
	else
		bStrip = FALSE;

	for (in = 0; in < NbrFurLayers ; in++)// NbrFurLayers
	{
		DWORD popo= (DWORD)(( (float)(in)/NbrFurLayers)*255);
		Gx8_M_RenderState(pst_SD, D3DRS_ALPHATESTENABLE, TRUE );
		Gx8_M_RenderState(pst_SD, D3DRS_ALPHAREF, popo);

//		Gx8_M_RenderState(pst_SD, D3DRS_ALPHAREF, (((float)(i))/(float)NbrFurLayers)*255);
		Gx8_M_RenderState(pst_SD, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );//alenvers ou a l'endroit ? ??

		offsetN = OffsetNormal * in;
		offSetU = OffsetU * in;
		offSetV = OffsetV * in;
		t = _pst_Element->dst_Triangle;

		if( bStrip )
		{
			// *** draw strip *** //
			ULONG	i;
			//ULONG	ulEdgeColor;
			//~~~~~~~~~~~~~~~~

			if  (( (_pst_Element->pst_StripData->ulFlag & GEO_C_Strip_DisplayStrip)) && ((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired)))
			{
				//TO DO:
			}
			else 
			{
				pStrip = _pst_Element->pst_StripData->pStripList;
				pStripEnd = pStrip + _pst_Element->pst_StripData->ulStripNumber;

				for(; pStrip < pStripEnd; pStrip++)
				{
					IDirect3DDevice8_Begin( pst_SD->mp_D3DDevice, D3DPT_TRIANGLESTRIP );
					
					//PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_Begin);

					if(pst_Color == NULL)
					{
						ulOGLSetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulOGLSetCol_Or;
						ulOGLSetCol ^= ulOGLSetCol_XOr;
						IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol));
					}

					if(!_pst_UV)
					{
						for(i = 0; i < pStrip->ulVertexNumber; i++)
						{
							auw_Index = pStrip->pMinVertexDataList[i].auw_Index;
  							
							IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, auw_Index );
 							IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX, _pst_Point[auw_Index].x, _pst_Point[auw_Index].y, _pst_Point[auw_Index].z, 1.0f );
							GX8_SetColorRGBA(auw_Index);
						}
					}
					else if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
					{
						for(i = 0; i < pStrip->ulVertexNumber; i++)
						{
							float nx,ny,nz;
							D3DXVECTOR3 pos;

							auw_Index = pStrip->pMinVertexDataList[i].auw_Index;

							GX8_SetColorRGBA(auw_Index);
 							IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, (float)_pst_UV[auw_UV].fU , (float)_pst_UV[auw_UV].fV );
			
							//ulOGLSetCol = (ulOGLSetCol)>>4*6;
							//offsetN2 = offsetN*ulOGLSetCol / 256.0f;

							nx=_pst_Normal[auw_Index].x;
							ny=_pst_Normal[auw_Index].y;
							nz=_pst_Normal[auw_Index].z;

							pos.x=_pst_Point[auw_Index].x + offsetN*nx;
							pos.y=_pst_Point[auw_Index].y + offsetN*ny;
							pos.z=_pst_Point[auw_Index].z + offsetN*nz;
							
							IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX,pos.x, pos.y, pos.z, 1.0f );
						}
					}
					else if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_FaceMap)
					{
						ERR_X_ForceError("pas de chance, on n'a pas facemap + strip en magasin", NULL);
					}
					else
					{
						for(i = 0; i < pStrip->ulVertexNumber; i++)
						{
							float nx,ny,nz;
							D3DXVECTOR3 pos;
							
							auw_Index = pStrip->pMinVertexDataList[i].auw_Index;
							auw_UV = pStrip->pMinVertexDataList[i].auw_UV;

							GX8_SetColorRGBA(auw_Index);
							//ulOGLSetCol = pst_Color[auw_Index] | ulOGLSetCol_Or; 
							//ulOGLSetCol ^= ulOGLSetCol_XOr;
	/*						if(pst_Alpha)
							{
								ulOGLSetCol &= 0x00ffffff;
								ulOGLSetCol |= pst_Alpha[auw_Index];
							}*/
							//IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(ulOGLSetCol)); \

/*
							if ( pst_Color )
							{
							ulOGLSetCol = (ulOGLSetCol)>>4*6;
							offsetN2 = offsetN*ulOGLSetCol / 256.0f;

							nx=_pst_Normal[auw_Index].x;
							ny=_pst_Normal[auw_Index].y;
							nz=_pst_Normal[auw_Index].z;

							pos.x=_pst_Point[auw_Index].x + offsetN2*nx;
							pos.y=_pst_Point[auw_Index].y + offsetN2*ny;
							pos.z=_pst_Point[auw_Index].z + offsetN2*nz;

							}
							else
	*/						{
							nx=_pst_Normal[auw_Index].x;
							ny=_pst_Normal[auw_Index].y;
							nz=_pst_Normal[auw_Index].z;

							pos.x=_pst_Point[auw_Index].x + offsetN*nx;
							pos.y=_pst_Point[auw_Index].y + offsetN*ny;
							pos.z=_pst_Point[auw_Index].z + offsetN*nz;
							}
							//IDirect3DDevice8_SetVertexDataColor( pst_SD->mp_D3DDevice, D3DVSDE_DIFFUSE, Gx8_M_ConvertColor(auw_Index));

							IDirect3DDevice8_SetVertexData2f( pst_SD->mp_D3DDevice, D3DVSDE_TEXCOORD0, (float)_pst_UV[auw_UV].fU , (float)_pst_UV[auw_UV].fV );

							IDirect3DDevice8_SetVertexData4f( pst_SD->mp_D3DDevice, D3DVSDE_VERTEX,pos.x, pos.y, pos.z, 1.0f );
							
						}
					}

					//PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_GL_End);

					IDirect3DDevice8_End( pst_SD->mp_D3DDevice );
				}
			}
		}
//		glDisable(GL_BLEND);
//		glBlendFunc(GL_ONE, GL_ZERO);
	}
	//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE );
	
	// restore test context
	if ( i_Alphatest )
	{
		Gx8_M_RenderState(pst_SD, D3DRS_ALPHATESTENABLE, TRUE );
		Gx8_M_RenderState(pst_SD, D3DRS_ALPHAFUNC, i_Alphafunc );
		Gx8_M_RenderState(pst_SD, D3DRS_ALPHAREF, f_Alpharef );
	}
	else
	{
		Gx8_M_RenderState(pst_SD, D3DRS_ALPHATESTENABLE, FALSE );
		Gx8_M_RenderState(pst_SD, D3DRS_ALPHAFUNC, D3DCMP_ALWAYS );
		//desac glDisable(GL_ALPHA_TEST);	
		//desac glAlphaFunc(GL_ALWAYS, 0 );
	}

	// fin //
	return _pst_Element->l_NbTriangles*6;
}

//#else
// --- Fur Test with vertex shader ---

LONG GX8_l_ShiftDrawElementIndexedTrianglesVB // STRIP VERSION
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	MATH_tdst_Vector					*_pst_Normal,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//    GEO_tdst_IndexedTriangle    *t, *tend;
    ULONG                       *pst_Color, *pst_Alpha;
    ULONG                       TNum, ulOGLSetCol, ulOGLSetCol_XOr, ulOGLSetCol_Or, ulAmbient;
    Gx8_tdst_SpecificData   *pst_SD = GX8_M_SD( GDI_gpst_CurDD );
    GEO_tdst_Object             * pObject;
	ULONG						ulDrwMskDelta;
	BOOL						i_Alphatest;
	int							i_Alphafunc;
	ULONG						f_Alpharef;

	float						offsetN;//,offsetN2;

    static bool bSingleColor_OPT=true;  //this will enable the trick of the single color for all the triangles
    static bool bMultipleColor_OPT=false;
   	//extern bTest;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    //_GSP_BeginRaster(45);

	TNum = _pst_Element->l_NbTriangles;
    if(!TNum) return 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////
    
	pObject = GDI_gpst_CurDD->pst_CurrentGeo;
    
    // ANTI-BUG for pre-submission version
    if (pObject == NULL) 
        return 0;

	ulDrwMskDelta = GDI_gpst_CurDD->LastDrawMask ^ GDI_gpst_CurDD->ul_CurrentDrawMask;
	pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
	pst_Alpha = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentAlphaField;
	ulOGLSetCol_Or = pst_SD->ulColorOr;
	Gx8_RS_DrawWired(&pst_SD->st_RS, !(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotWired));

	Gx8_RS_DepthTest( &pst_SD->st_RS, GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest );
	Gx8_RS_Fogged(&pst_SD->st_RS, ((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged) && (pst_SD->ulFogState)));
	GDI_gpst_CurDD->LastDrawMask = GDI_gpst_CurDD->ul_CurrentDrawMask;

	IDirect3DDevice8_GetRenderState((D3DDevice*)pst_SD, D3DRS_ALPHATESTENABLE ,&i_Alphatest);
	if ( i_Alphatest )	
	{
		IDirect3DDevice8_GetRenderState((D3DDevice*)pst_SD, D3DRS_ALPHAFUNC ,&i_Alphafunc);
		IDirect3DDevice8_GetRenderState((D3DDevice*)pst_SD, D3DRS_ALPHAREF ,&f_Alpharef);
	}
	
 //   IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
 //   IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice,0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	//ERR_X_Assert( pObject != NULL );


	//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_NONE );  //Force culling

	IDirect3DDevice8_SetRenderState ( pst_SD->mp_D3DDevice, D3DRS_ALPHATESTENABLE, TRUE );
	IDirect3DDevice8_SetRenderState ( pst_SD->mp_D3DDevice, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_CULLMODE, D3DCULL_NONE );  //Force culling


	ulOGLSetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
	ulOGLSetCol_XOr ^= 0xff000000;


	if ( ( pObject->pIB ) && ( _pst_Element->pst_StripData ) )
    {
        ULONG                       * pColorSource;
        ULONG                       * pColorData;
        Gx8VertexComponent_Position * pPointData;
        Gx8VertexComponent_Normal   * pNormalData;
        IDirect3DVertexBuffer8      tmpVBpos;
        IDirect3DVertexBuffer8      tmpVBNorm;
        IDirect3DVertexBuffer8      tmpVBcol;
        UINT    drawStripCounter;
        GEO_tdst_StripData  * pStripData = _pst_Element->pst_StripData;
        
        //LIGHT_TurnOffObjectLighting();// ?? ??

        // Prepare Position Stream
        if ( _pst_Point == pObject->dst_Point )
        {
            IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 0, pObject->pVB_Position, sizeof(Gx8VertexComponent_Position) );
        }
        else
        {
            if ( pst_SD->positionVB.pCurrGO != GDI_gpst_CurDD->pst_CurrentGameObject )
            {
                if ( pst_SD->positionVB.nextIndex + pObject->l_NbPoints > CIRCULAR_VB_SIZE )
                {
                    pst_SD->positionVB.currIndex = 0;
                    pst_SD->positionVB.nextIndex = pObject->l_NbPoints;
                }
                else
                {
                    pst_SD->positionVB.currIndex = pst_SD->positionVB.nextIndex;
                    pst_SD->positionVB.nextIndex += pObject->l_NbPoints;
                }
                pst_SD->positionVB.pCurrGO = GDI_gpst_CurDD->pst_CurrentGameObject;
                IDirect3DVertexBuffer8_Lock( pst_SD->positionVB.pVB, pst_SD->positionVB.currIndex * pst_SD->positionVB.stride, pObject->l_NbPoints * pst_SD->positionVB.stride, (BYTE **) &pPointData, D3DLOCK_NOOVERWRITE );   // D3DLOCK_NOOVERWRITE
                memcpy( pPointData, _pst_Point, pObject->l_NbPoints * sizeof(Gx8VertexComponent_Position) );
                IDirect3DVertexBuffer8_Unlock( pst_SD->positionVB.pVB );
            }
            else
            {
                IDirect3DVertexBuffer8_Lock( pst_SD->positionVB.pVB, pst_SD->positionVB.currIndex * pst_SD->positionVB.stride, pObject->l_NbPoints * pst_SD->positionVB.stride, (BYTE **) &pPointData, D3DLOCK_READONLY );  // D3DLOCK_NOOVERWRITE
                IDirect3DVertexBuffer8_Unlock( pst_SD->positionVB.pVB );
            }

            XGSetVertexBufferHeader( 0, 0, 0, 0, &tmpVBpos, 0 );
            IDirect3DVertexBuffer8_Register( &tmpVBpos, pPointData );

            if ( FAILED( 
				IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 0, &tmpVBpos, pst_SD->positionVB.stride ) ) )
            {
//                assert(FALSE); // failed setting position stream
            }
        }
        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 1, NULL, 0 );
 
        {
      
		      //ATTENTION...THE SAME CHECK AS FOR POSITION IS USED FOR NORMALS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            if ( _pst_Point == pObject->dst_Point )
            {
                IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 1, pObject->pVB_Normal, sizeof(Gx8VertexComponent_Normal) );
            }
	        else
            {
                if ( pst_SD->normalVB.pCurrGO != GDI_gpst_CurDD->pst_CurrentGameObject )
                {
                    if ( pst_SD->normalVB.nextIndex + pObject->l_NbPoints > CIRCULAR_VB_SIZE )
                    {
                        pst_SD->normalVB.currIndex = 0;
                        pst_SD->normalVB.nextIndex = pObject->l_NbPoints;
                    }
                    else
                    {
                        pst_SD->normalVB.currIndex = pst_SD->normalVB.nextIndex;
                        pst_SD->normalVB.nextIndex += pObject->l_NbPoints;
                    }
                    pst_SD->normalVB.pCurrGO = GDI_gpst_CurDD->pst_CurrentGameObject;
                    IDirect3DVertexBuffer8_Lock( pst_SD->normalVB.pVB, pst_SD->normalVB.currIndex * pst_SD->normalVB.stride, pObject->l_NbPoints * pst_SD->normalVB.stride, (BYTE **) &pNormalData, D3DLOCK_NOOVERWRITE );  // D3DLOCK_NOOVERWRITE
                

                    //To be check

                    memcpy( pNormalData, pObject->dst_PointNormal, pObject->l_NbPoints * sizeof(Gx8VertexComponent_Normal) );
                    IDirect3DVertexBuffer8_Unlock( pst_SD->normalVB.pVB );
                }
                else
                {
                    IDirect3DVertexBuffer8_Lock( pst_SD->normalVB.pVB, pst_SD->normalVB.currIndex * pst_SD->normalVB.stride, pObject->l_NbPoints * pst_SD->normalVB.stride, (BYTE **) &pNormalData, D3DLOCK_READONLY ); // D3DLOCK_NOOVERWRITE
                    IDirect3DVertexBuffer8_Unlock( pst_SD->normalVB.pVB );
                }

                XGSetVertexBufferHeader( 0, 0, 0, 0, &tmpVBNorm, 0 );
                IDirect3DVertexBuffer8_Register( &tmpVBNorm, pNormalData );

                if ( FAILED(
					IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 1, &tmpVBNorm, pst_SD->normalVB.stride ) ) )
                {
                    //assert(FALSE); // failed setting position stream
                }
            

            }//New object is used

        }//normal

        // Prepare Color Stream
        pColorSource = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
        //ulOGLSetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
        //ulOGLSetCol_Or = pst_SD->ulColorOr;
        ulAmbient = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient;

        if ( ( pst_SD->colorVB.pCurrGO != GDI_gpst_CurDD->pst_CurrentGameObject ) ||
            ( pst_SD->colorVB.pCurrSource != pColorSource ) ||
            ( pst_SD->colorVB.currData1 != ulOGLSetCol_XOr ) ||
            ( pst_SD->colorVB.currData2 != ulOGLSetCol_Or ) ||
            ( pst_SD->colorVB.ambient != ulAmbient ) )
        {

            if ( pst_SD->colorVB.nextIndex + pObject->l_NbPoints > CIRCULAR_VB_SIZE_COLOR )
            {
                pst_SD->colorVB.currIndex = 0;
                pst_SD->colorVB.nextIndex = pObject->l_NbPoints;
            }
            else
            {
                pst_SD->colorVB.currIndex = pst_SD->colorVB.nextIndex;
                pst_SD->colorVB.nextIndex += pObject->l_NbPoints;
            }
            pst_SD->colorVB.pCurrGO = GDI_gpst_CurDD->pst_CurrentGameObject;
            pst_SD->colorVB.pCurrSource = pColorSource;
            pst_SD->colorVB.currData1 = ulOGLSetCol_XOr;
            pst_SD->colorVB.currData2 = ulOGLSetCol_Or;
            pst_SD->colorVB.ambient = ulAmbient;

			if( !pColorSource )
            { 
				bSingleColor_OPT = false;
				//bSingleColor_OPT=bTest;

				if(bSingleColor_OPT)    //Go for the optimized one
                {   
                    //Note:
                    //since the color is the same for the whole element
                    //just set the diffuse to that color.
                    //This prevent copying all the colors to the array.
                    pColorData=0;


                    ulOGLSetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulOGLSetCol_Or;
                    ulOGLSetCol ^= ulOGLSetCol_XOr;
                    ulOGLSetCol = Gx8_M_ConvertColor(ulOGLSetCol);

                    IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice,D3DRS_TEXTUREFACTOR,ulOGLSetCol);
                    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
                    IDirect3DDevice8_SetTextureStageState( pst_SD->mp_D3DDevice, 0, D3DTSS_COLORARG0,D3DTA_TFACTOR);
                }
                else
                {
                    ULONG   * pDest;
                    ULONG   * pEnd;

                    IDirect3DVertexBuffer8_Lock( pst_SD->colorVB.pVB, pst_SD->colorVB.currIndex * pst_SD->colorVB.stride, pObject->l_NbPoints * pst_SD->colorVB.stride, (BYTE **)&pColorData, D3DLOCK_NOOVERWRITE );    // D3DLOCK_NOOVERWRITE

                    pDest = (ULONG *) pColorData;
                    pEnd = pDest + pObject->l_NbPoints;


                    ulOGLSetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulOGLSetCol_Or;
                    ulOGLSetCol ^= ulOGLSetCol_XOr;
                    ulOGLSetCol = Gx8_M_ConvertColor(ulOGLSetCol);

                    for ( ; pDest < pEnd; pDest++ )
                        * pDest = ulOGLSetCol;

                    IDirect3DVertexBuffer8_Unlock( pst_SD->colorVB.pVB );
                    
                }
            }
            else
			{
                {
                    ULONG   * pDest;
                    ULONG   * pSource = (ULONG *) pColorSource;
                    ULONG   * pEnd;
                    ULONG   col;

                    IDirect3DVertexBuffer8_Lock( pst_SD->colorVB.pVB, pst_SD->colorVB.currIndex * pst_SD->colorVB.stride, pObject->l_NbPoints * pst_SD->colorVB.stride, (BYTE **)&pColorData, D3DLOCK_NOOVERWRITE );    // D3DLOCK_NOOVERWRITE
                    pDest = (ULONG *) pColorData;
                    pEnd = pDest + pObject->l_NbPoints;

                    for ( ; pDest < pEnd; pDest++ )
                    {
                        col = ( ( ( * pSource ) | ulOGLSetCol_Or ) ^ ulOGLSetCol_XOr );
                        * pDest = Gx8_M_ConvertColor( col );
                        pSource++;
                    }
                    IDirect3DVertexBuffer8_Unlock( pst_SD->colorVB.pVB );
                } 
            }

        }
        else 
        {
            IDirect3DVertexBuffer8_Lock( pst_SD->colorVB.pVB, pst_SD->colorVB.currIndex * pst_SD->colorVB.stride, pObject->l_NbPoints * pst_SD->colorVB.stride, (BYTE **)&pColorData, D3DLOCK_READONLY );   // D3DLOCK_NOOVERWRITE
            IDirect3DVertexBuffer8_Unlock( pst_SD->colorVB.pVB );
        }


        if(pColorData)
        {
            XGSetVertexBufferHeader( 0, 0, 0, 0, &tmpVBcol, 0 );
            IDirect3DVertexBuffer8_Register( &tmpVBcol, pColorData );
        }

 		{
            if(!pColorData)
            {
                IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, NULL, 0 );
            }
            else
            {
                if ( FAILED( IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, &tmpVBcol, pst_SD->colorVB.stride ) ) )
                {
                    //assert(FALSE); // failed setting color stream
                }
            }

        }

        IDirect3DDevice8_SetIndices( pst_SD->mp_D3DDevice, pObject->pIB, 0 );

        for ( drawStripCounter = 0; drawStripCounter < pStripData->drawStripNumber; drawStripCounter++)
        {
            GEO_tdst_DrawStrip  * pDrawStrip = &pStripData->pDrawStripList[ drawStripCounter ];
            //float costant[4]={0.0f,0.0f,0.0f,0.0f};

/*
            //Alpha lit (to be check)
            if (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseLitAlphaShader)
            {


                // Set UV source stream and Vertex Declaration
                if ( pObject->pVBlist_UV )
                {
                    if(pObject->dst_PointNormal)
                    {
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 3, pObject->pVBlist_UV[ pDrawStrip->UVbufferIndex ], sizeof(Gx8VertexComponent_UV) );
                    
                    }
                    else
                    {
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, pObject->pVBlist_UV[ pDrawStrip->UVbufferIndex ], sizeof(Gx8VertexComponent_UV) );
                    
                    }
                }
                else
                {   
                    if(pObject->dst_PointNormal)
                    {
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 3, NULL, 0 );
                    }
                    else
                    {
                        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, NULL, 0 );
                    }
                }


                 Gx8_vUseLitAlphaVertexShader();


                //Carlone
                 IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice, 44, costant,1);

                ERR_X_Assert(pDrawStrip->vertexNumber>2);

                 IDirect3DDevice8_DrawIndexedPrimitive( pst_SD->mp_D3DDevice, 
                                                        D3DPT_TRIANGLESTRIP,
                                                        0,
                                                        pObject->l_NbPoints,
                                                        pDrawStrip->startingIndex,
                                                        pDrawStrip->vertexNumber - 2 );
            }
            else
 */           {


                // Set UV source stream and Vertex Declaration
                //if ( pObject->pVBlist_UV )
                {
					//Gx8_vSetVertexShader( pst_SD->pVertexShaderNormalWithUV );
	
				IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 3, pObject->pVBlist_UV[ pDrawStrip->UVbufferIndex ], sizeof(Gx8VertexComponent_UV) );
						
						//IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 3, NULL, 0 );
                    
                }
/*				else
                {
                         //IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 3, NULL, 0 );
                        //Gx8_vUseFurVertexShader();
						//Gx8_vSetVertexShader( pst_SD->pVertexShaderNormal );
                 }
*/

                //ERR_X_Assert(pDrawStrip->vertexNumber>2);

				//IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, NULL, 0 );
				{
				int i;
				//MATH_tdst_Matrix Matrix;
				//MATH_tdst_Matrix s_stMappingMatrice;	
				//MATH_MakeOGLMatrix(&Matrix, &s_stMappingMatrice);
				
				//IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice, 20, &s_stMappingMatrice, 4);
				//Gx8_vSetUVTransformationForShaders((D3DMATRIX *)&Matrix);
				
				Gx8_vUseFurVertexShader();
				//Gx8_vSetVertexShader( pst_SD->pVertexShaderNormalWithUV );

					for ( i = 0; i < NbrFurLayers ; i++ )//NbrFurLayers
					{
						D3DXVECTOR4 facto;
						//D3DCOLOR color;
						//DWORD popo;
						//color = 0x33ff0000;
						//popo=(DWORD)(( (float)(i)/NbrFurLayers)*255);
						//if (i==0) popo=10;
						offsetN = OffsetNormal * i;
						//Gx8_M_RenderState(pst_SD, D3DRS_ALPHAREF, popo);
						Gx8_M_RenderState(pst_SD, D3DRS_ALPHAREF, (DWORD)(( (float)(i)/NbrFurLayers)*255));
						//if (!i)	Gx8_M_RenderState(pst_SD, D3DRS_ALPHAREF, 0x5);
	
				/*		if (pst_Alpha)
						{
							//ulOGLSetCol =0;
							ulOGLSetCol = (ulOGLSetCol)>>4*6;
							offsetN2 = offsetN*ulOGLSetCol / 256.0f;
							facto.x = facto.y = facto .z =  facto.w = offsetN2;
							facto.w = 0;
						}
						else */
						{
							facto.x = facto.y = facto .z =  facto.w = (offsetN);//-OffsetNormal;
							facto.w = 0;
						}
						
						
						IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice, 15, &facto,1);
						//IDirect3DDevice8_SetVertexShaderConstant(pst_SD->mp_D3DDevice, 16, &color,1);

						IDirect3DDevice8_DrawIndexedPrimitive( pst_SD->mp_D3DDevice, 
																D3DPT_TRIANGLESTRIP,
																0,
																pObject->l_NbPoints,
																pDrawStrip->startingIndex,
																pDrawStrip->vertexNumber - 2 );

					}
				}
			}
            

        }
        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 0, NULL, 0 );
        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 1, NULL, 0 );
        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 2, NULL, 0 );
        IDirect3DDevice8_SetStreamSource( pst_SD->mp_D3DDevice, 3, NULL, 0 );
		
		if ( i_Alphatest )
		{
			Gx8_M_RenderState(pst_SD, D3DRS_ALPHATESTENABLE, TRUE );
			Gx8_M_RenderState(pst_SD, D3DRS_ALPHAFUNC, i_Alphafunc );
			Gx8_M_RenderState(pst_SD, D3DRS_ALPHAREF, f_Alpharef );
		}
		else
		{
			Gx8_M_RenderState(pst_SD, D3DRS_ALPHATESTENABLE, FALSE );
			Gx8_M_RenderState(pst_SD, D3DRS_ALPHAFUNC, D3DCMP_ALWAYS );
		}
	
		
		//IDirect3DDevice8_SetRenderState( pst_SD->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, FALSE );

        //_GSP_EndRaster(45);

        //return 0;
    }

    return _pst_Element->l_NbTriangles*6;
}
//#endif