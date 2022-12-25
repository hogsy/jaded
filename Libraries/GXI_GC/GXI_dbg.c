
#include "GEOmetric/GEOobject.h"
#include "GXI_init.h"
#include "GXI_render.h"
#include "GXI_displaylist.h"
#include "GXI_vertexspace.h"
#include "GXI_tex.h"
#include "GXI_font.h"
#include "GXI_renderstate.h"
#include "GXI_bench.h"
#include "BASe/MEMory/MEM.h"
#include "MATerial/MATstruct.h"

#include "GEOmetric/GEO_STRIP.h"
#include "GEOmetric/GEO_SKIN.h"


BOOL g_bShowNormals=FALSE;
BOOL g_bShowSkinElements=FALSE;
BOOL g_bShowVertexColors=FALSE;
BOOL g_bShowLights=FALSE;
BOOL g_bUseHardwareTextureMatrices=TRUE;
BOOL g_bUseHardwareLights=TRUE;
BOOL g_bUseHWStates=FALSE; 
BOOL g_bUseHWMul2x=TRUE;


BOOL NoMulti=FALSE;
BOOL NoMATDRAW=FALSE;
BOOL NoSDW=FALSE;
BOOL NoSPR=FALSE;
BOOL NoGEODRAW=FALSE;
BOOL NoGODRAW=FALSE;
BOOL NoLIGH=FALSE;
BOOL NoGFX=FALSE;
BOOL NoZLST=FALSE;
BOOL NoSKN=FALSE;
BOOL NoADM=FALSE;
BOOL NoPAG=FALSE;
BOOL NoSTR=FALSE;
BOOL NoMDF=FALSE;
BOOL NoFUR=FALSE;
BOOL NoSPG2=FALSE;

void GXI_DBG_DrawVectorInWorldSpace(MATH_tdst_Vector *_pPointA,MATH_tdst_Vector *_pPointB, u32 _uColor)
{
	f32 old_projection[GX_PROJECTION_SZ];	

	GXGetProjectionv( old_projection );
    GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_ENABLE);
	GXSetLineWidth(20, GX_TO_ZERO);	
    
	GXLoadPosMtxImm(*(Mtx*)&GXI_Global_ACCESS(current_modelview_matrix), GX_PNMTX0);
    //MTXIdentity(mv);
    //GXLoadPosMtxImm(mv, GX_PNMTX0);
    
	GX_GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_CLR0,GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_NRM, GX_NONE);
    GX_GXSetVtxDesc(GX_VA_TEX0,GX_NONE);
#ifdef USE_PERPECTIVE_CORRECT_COLORS
    GX_GXSetVtxDesc(GX_VA_CLR1,GX_NONE);
#endif

    GX_GXSetCullMode(GX_CULL_NONE);
    
    GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(0);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	
	GX_GXSetChanCtrl(  GX_COLOR0A0,
    			   GX_DISABLE,		// enable Channel
    			   GX_SRC_VTX,		// amb source
    			   GX_SRC_VTX,		// mat source
    			   GX_LIGHT_NULL,	// light mask
    			   GX_DF_CLAMP,		// diffuse function
    			   GX_AF_NONE );	// atten   function
    
    GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
    
	GXBegin(GX_LINES, GX_VTXFMT0, 2);
	GXPosition3f32(_pPointA->x, _pPointA->y, _pPointA->z);
	GXColor1u32(_uColor);
	GXPosition3f32(_pPointB->x, _pPointB->y, _pPointB->z);
	GXColor1u32(_uColor);
	GXEnd();
	GXSetProjectionv( old_projection );
}

void GXI_DBG_DrawPointInWorldSpace(MATH_tdst_Vector *_pPoint,u32 color)
{
	f32 old_projection[GX_PROJECTION_SZ];
	
	GXGetProjectionv( old_projection );
	
    GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_ENABLE);
	GXSetPointSize(40, GX_TO_ZERO);
    
	GXLoadPosMtxImm(*(Mtx*)&GXI_Global_ACCESS(current_modelview_matrix), GX_PNMTX0);
    //MTXIdentity(mv);
    //GXLoadPosMtxImm(mv, GX_PNMTX0);
    
	GX_GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_CLR0,GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_NRM, GX_NONE);
    GX_GXSetVtxDesc(GX_VA_TEX0,GX_NONE);
#ifdef USE_PERPECTIVE_CORRECT_COLORS
    GX_GXSetVtxDesc(GX_VA_CLR1,GX_NONE);
#endif

    GX_GXSetCullMode(GX_CULL_NONE);
    
    GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(0);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	
	GX_GXSetChanCtrl(  GX_COLOR0A0,
    			   GX_DISABLE,		// enable Channel
    			   GX_SRC_VTX,		// amb source
    			   GX_SRC_VTX,		// mat source
    			   GX_LIGHT_NULL,	// light mask
    			   GX_DF_CLAMP,		// diffuse function
    			   GX_AF_NONE );	// atten   function
    
    GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
        
	GXBegin(GX_POINTS, GX_VTXFMT0, 1);
	GXPosition3f32(_pPoint->x, _pPoint->y, _pPoint->z);
	GXColor1u32(color);
	GXEnd();
	GXSetProjectionv( old_projection );
}

void GXI_DBG_DrawPointInViewSpace(MATH_tdst_Vector *_pPoint,u32 color)
{
	f32 old_projection[GX_PROJECTION_SZ];
	f32   mv[3][4];
	
	GXGetProjectionv( old_projection );
	
    GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_ENABLE);
	GXSetPointSize(40, GX_TO_ZERO);
    
//	GXLoadPosMtxImm(*(Mtx*)&GXI_Global_ACCESS(current_modelview_matrix), GX_PNMTX0);
    MTXIdentity(mv);
    GXLoadPosMtxImm(mv, GX_PNMTX0);
    
	GX_GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_CLR0,GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_NRM, GX_NONE);
    GX_GXSetVtxDesc(GX_VA_TEX0,GX_NONE);
#ifdef USE_PERPECTIVE_CORRECT_COLORS
    GX_GXSetVtxDesc(GX_VA_CLR1,GX_NONE);
#endif

    GX_GXSetCullMode(GX_CULL_NONE);
    
    GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(0);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	
	GX_GXSetChanCtrl(  GX_COLOR0A0,
    			   GX_DISABLE,		// enable Channel
    			   GX_SRC_VTX,		// amb source
    			   GX_SRC_VTX,		// mat source
    			   GX_LIGHT_NULL,	// light mask
    			   GX_DF_CLAMP,		// diffuse function
    			   GX_AF_NONE );	// atten   function
    
    GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
        
	GXBegin(GX_POINTS, GX_VTXFMT0, 1);
	GXPosition3f32(_pPoint->x, _pPoint->y, _pPoint->z);
	GXColor1u32(color);
	GXEnd();
	GXSetProjectionv( old_projection );
}

void GXI_DBG_DrawPointInOrthoSpace(MATH_tdst_Vector *_pPoint,u32 color)
{
	f32 old_projection[GX_PROJECTION_SZ];
	
	GXGetProjectionv( old_projection );
    GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_ENABLE);
	GXSetPointSize(40, GX_TO_ZERO);
    
    //MTXIdentity(mv);
    //GXLoadPosMtxImm(mv, GX_PNMTX0);
    
	GX_GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_CLR0,GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_NRM, GX_NONE);
    GX_GXSetVtxDesc(GX_VA_TEX0,GX_NONE);
#ifdef USE_PERPECTIVE_CORRECT_COLORS
    GX_GXSetVtxDesc(GX_VA_CLR1,GX_NONE);
#endif

    GX_GXSetCullMode(GX_CULL_NONE);
    
    GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(0);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	
	GX_GXSetChanCtrl(  GX_COLOR0A0,
    			   GX_DISABLE,		// enable Channel
    			   GX_SRC_VTX,		// amb source
    			   GX_SRC_VTX,		// mat source
    			   GX_LIGHT_NULL,	// light mask
    			   GX_DF_CLAMP,		// diffuse function
    			   GX_AF_NONE );	// atten   function
    			   
	GXSetProjection(GXI_Global_ACCESS(orthogonal2D_matrix), GX_ORTHOGRAPHIC);					
	GXLoadPosMtxImm(GXI_Global_ACCESS(identity_projection_matrix), GX_PNMTX0);
    
    GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
        
	GXBegin(GX_POINTS, GX_VTXFMT0, 1);
	GXPosition3f32(_pPoint->x, _pPoint->y, _pPoint->z);
	GXColor1u32(color);
	GXEnd();
	
	GXLoadPosMtxImm(*(Mtx*)&GXI_Global_ACCESS(current_modelview_matrix), GX_PNMTX0);
	GXSetProjectionv( old_projection );
}



void GXI_DBG_ShowVertexColors
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	ULONG								ulnumberOfPoints
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_IndexedTriangle	*t, *tend;
	GEO_Vertex					*pst_Normals;
	BOOL						bStrip;
	u16							auw_Index;
	GEO_tdst_OneStrip			*pStrip, *pStripEnd;
	u32							i;
	Vec *v1;
	u32 col1;
	u32							*pst_Color;
	u32							*pst_Alpha;
	u32							ulGXISetCol_Or;
	u32							ulGXISetCol_XOr;
	u32							ulGXISetCol;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ulGXISetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
	ulGXISetCol_Or = GXI_Global_ACCESS(ulColorOr);

	pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
	pst_Alpha = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentAlphaField;
	
	t = _pst_Element->dst_Triangle;
	tend = t + _pst_Element->l_NbTriangles;

	pst_Normals = GDI_gpst_CurDD->pst_CurrentGeo->dst_PointNormal;

	if(_pst_Element->pst_StripData != NULL)
	{
		if(_pst_Element->pst_StripData->ulFlag & GEO_C_Strip_DataValid)
			bStrip = TRUE;
		else
			bStrip = FALSE;
	}
	else
		bStrip = FALSE;

	GX_GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);

	GX_GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_CLR0,GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_NRM, GX_NONE);
    GX_GXSetVtxDesc(GX_VA_TEX0,GX_NONE);
#ifdef USE_PERPECTIVE_CORRECT_COLORS
    GX_GXSetVtxDesc(GX_VA_CLR1,GX_NONE);
#endif

    GX_GXSetCullMode(GX_CULL_NONE);
    
    GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(0);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	
	GX_GXSetChanCtrl(  GX_COLOR0A0,
    			   GX_DISABLE,		// enable Channel
    			   GX_SRC_VTX,		// amb source
    			   GX_SRC_VTX,		// mat source
    			   GX_LIGHT_NULL,	// light mask
    			   GX_DF_CLAMP,		// diffuse function
    			   GX_AF_NONE );	// atten   function
    
    GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
    			   
	GXSetFog( GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, GX_BLACK );
	
	col1 = 0xff88ffff;
	
	//---------------------------------------------------------------
	// indexed triangles mode
	//---------------------------------------------------------------
	if(bStrip == FALSE)
	{
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, _pst_Element->l_NbTriangles*3);

		while(t < tend)
		{     
			
			for(i=0;i<3;i++)
			{   
				v1 = (Vec*)&_pst_Point[t->auw_Index[i]];

				GXPosition3f32(v1->x, v1->y, v1->z);
				
					if(pst_Color) 
					{ 
						ulGXISetCol = pst_Color[t->auw_Index[i]] | ulGXISetCol_Or; 
						ulGXISetCol ^= ulGXISetCol_XOr; 
						if(pst_Alpha) 
						{ 
							ulGXISetCol &= 0x00ffffff; 
							ulGXISetCol |= pst_Alpha[t->auw_Index[i]]; 
						} 
					} 
					else if(pst_Alpha) 
					{ 
						ulGXISetCol &= 0x00ffffff; 
						ulGXISetCol |= pst_Alpha[t->auw_Index[i]]; 
					} 
					else
					{
						ulGXISetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulGXISetCol_Or;
					    ulGXISetCol ^= ulGXISetCol_XOr;
					}

					col1 = ((ulGXISetCol & 0x000000ff)<<24 | (ulGXISetCol & 0x0000ff00)<<8 | (ulGXISetCol & 0x00ff0000)>>8 | (ulGXISetCol & 0xff000000)>>24);

		        GXColor1u32(col1);
			}

   	    	t++;
   	    }
   	    	
   	    GXEnd();
	}

	//---------------------------------------------------------------
	// strip mode
	//---------------------------------------------------------------
	else 
	{
		pStrip = _pst_Element->pst_StripData->pStripList;
		pStripEnd = pStrip + _pst_Element->pst_StripData->ulStripNumber;

		for(; pStrip < pStripEnd; pStrip++)
		{
	       	GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, pStrip->ulVertexNumber);
		
				for(i = 0; i < pStrip->ulVertexNumber; i++)
				{
					auw_Index = pStrip->pMinVertexDataList[i].auw_Index;

					v1 = (Vec*)&_pst_Point[auw_Index];

					GXPosition3f32(v1->x, v1->y, v1->z);

						if(pst_Color) 
						{ 
							ulGXISetCol = pst_Color[t->auw_Index[i]] | ulGXISetCol_Or; 
							ulGXISetCol ^= ulGXISetCol_XOr; 
							if(pst_Alpha) 
							{ 
								ulGXISetCol &= 0x00ffffff; 
								ulGXISetCol |= pst_Alpha[t->auw_Index[i]]; 
							} 
						} 
						else if(pst_Alpha) 
						{ 
							ulGXISetCol &= 0x00ffffff; 
							ulGXISetCol |= pst_Alpha[t->auw_Index[i]]; 
						} 
						else
						{
							ulGXISetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulGXISetCol_Or;
						    ulGXISetCol ^= ulGXISetCol_XOr;
						}

						col1 = ((ulGXISetCol & 0x000000ff)<<24 | (ulGXISetCol & 0x0000ff00)<<8 | (ulGXISetCol & 0x00ff0000)>>8 | (ulGXISetCol & 0xff000000)>>24);
						
			        GXColor1u32(col1);
				}
				
			GXEnd();
		}
	}
}

void GXI_DBG_ShowSkinElements
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	ULONG								ulnumberOfPoints
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_IndexedTriangle	*t, *tend;
	GEO_Vertex					*pst_Normals;
	BOOL						bStrip;
	u16							auw_Index;
	GEO_tdst_OneStrip			*pStrip, *pStripEnd;
	u32							i;
	Vec *v1;
	u32 col1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!GEO_SKN_IsSkinned(GDI_gpst_CurDD->pst_CurrentGeo)) 
		return;
	
	t = _pst_Element->dst_Triangle;
	tend = t + _pst_Element->l_NbTriangles;

	pst_Normals = GDI_gpst_CurDD->pst_CurrentGeo->dst_PointNormal;

	if(_pst_Element->pst_StripData != NULL)
	{
		if(_pst_Element->pst_StripData->ulFlag & GEO_C_Strip_DataValid)
			bStrip = TRUE;
		else
			bStrip = FALSE;
	}
	else
		bStrip = FALSE;

	GX_GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);

	GX_GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_CLR0,GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_NRM, GX_NONE);
    GX_GXSetVtxDesc(GX_VA_TEX0,GX_NONE);
#ifdef USE_PERPECTIVE_CORRECT_COLORS
    GX_GXSetVtxDesc(GX_VA_CLR1,GX_NONE);
#endif

    GX_GXSetCullMode(GX_CULL_NONE);
    
    GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(0);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	
	GX_GXSetChanCtrl(  GX_COLOR0A0,
    			   GX_DISABLE,		// enable Channel
    			   GX_SRC_VTX,		// amb source
    			   GX_SRC_VTX,		// mat source
    			   GX_LIGHT_NULL,	// light mask
    			   GX_DF_CLAMP,		// diffuse function
    			   GX_AF_NONE );	// atten   function
    
    GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
    			   
	GXSetFog( GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, GX_BLACK );
	
	col1 = 0xff88ffff;
	
	//---------------------------------------------------------------
	// indexed triangles mode
	//---------------------------------------------------------------
	if(bStrip == FALSE)
	{
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, _pst_Element->l_NbTriangles*3);

		while(t < tend)
		{     
			
			for(i=0;i<3;i++)
			{   
				v1 = (Vec*)&_pst_Point[t->auw_Index[i]];

				GXPosition3f32(v1->x, v1->y, v1->z);
		        GXColor1u32(col1);
			}

   	    	t++;
   	    }
   	    	
   	    GXEnd();
	}

	//---------------------------------------------------------------
	// strip mode
	//---------------------------------------------------------------
	else 
	{
		pStrip = _pst_Element->pst_StripData->pStripList;
		pStripEnd = pStrip + _pst_Element->pst_StripData->ulStripNumber;

		for(; pStrip < pStripEnd; pStrip++)
		{
	       	GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, pStrip->ulVertexNumber);
		
				for(i = 0; i < pStrip->ulVertexNumber; i++)
				{
					auw_Index = pStrip->pMinVertexDataList[i].auw_Index;

					v1 = (Vec*)&_pst_Point[auw_Index];

					GXPosition3f32(v1->x, v1->y, v1->z);
			        GXColor1u32(col1);
				}
				
			GXEnd();
		}
	}
}


void GXI_DBG_DrawObjectNormals
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	ULONG								ulnumberOfPoints
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_IndexedTriangle	*t, *tend;
	GEO_Vertex					*pst_Normals;
	BOOL						bStrip;
	u16							auw_Index;
	GEO_tdst_OneStrip			*pStrip, *pStripEnd;
	u32							i;
	Vec *v1, v2, v3;
	u32 col1, col2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	t = _pst_Element->dst_Triangle;
	tend = t + _pst_Element->l_NbTriangles;

	pst_Normals = GDI_gpst_CurDD->pst_CurrentGeo->dst_PointNormal;

	if(_pst_Element->pst_StripData != NULL)
	{
		if(_pst_Element->pst_StripData->ulFlag & GEO_C_Strip_DataValid)
			bStrip = TRUE;
		else
			bStrip = FALSE;
	}
	else
		bStrip = FALSE;

	GX_GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);

	GX_GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_CLR0,GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_NRM, GX_NONE);
    GX_GXSetVtxDesc(GX_VA_TEX0,GX_NONE);
#ifdef USE_PERPECTIVE_CORRECT_COLORS
    GX_GXSetVtxDesc(GX_VA_CLR1,GX_NONE);
#endif

    GX_GXSetCullMode(GX_CULL_NONE);
    
    GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(0);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	
	GX_GXSetChanCtrl(  GX_COLOR0A0,
    			   GX_DISABLE,		// enable Channel
    			   GX_SRC_VTX,		// amb source
    			   GX_SRC_VTX,		// mat source
    			   GX_LIGHT_NULL,	// light mask
    			   GX_DF_CLAMP,		// diffuse function
    			   GX_AF_NONE );	// atten   function
    
    GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
    			   
	GXSetFog( GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, GX_BLACK );
	
	col1 = 0xffffffff;
	col2 = 0xffff00ff;
	
	//---------------------------------------------------------------
	// indexed triangles mode
	//---------------------------------------------------------------
	if(bStrip == FALSE)
	{
		GXBegin(GX_LINES, GX_VTXFMT0, _pst_Element->l_NbTriangles*3*2);

		while(t < tend)
		{     
			
			for(i=0;i<3;i++)
			{   
				v1 = (Vec*)&_pst_Point[t->auw_Index[i]];

				memcpy(&v3, (Vec*)&pst_Normals[t->auw_Index[i]], sizeof(Vec));
				VECScale(&v3, &v3, 0.10f);
				VECAdd(v1, &v3, &v2);

				GXPosition3f32(v1->x, v1->y, v1->z);
		        GXColor1u32(col1);
				GXPosition3f32(v2.x, v2.y, v2.z);
		        GXColor1u32(col2);
			}

   	    	t++;
   	    }
   	    	
   	    GXEnd();
	}

	//---------------------------------------------------------------
	// strip mode
	//---------------------------------------------------------------
	else 
	{
		pStrip = _pst_Element->pst_StripData->pStripList;
		pStripEnd = pStrip + _pst_Element->pst_StripData->ulStripNumber;

		for(; pStrip < pStripEnd; pStrip++)
		{
	       	GXBegin(GX_LINES, GX_VTXFMT0, pStrip->ulVertexNumber*2);
		
				for(i = 0; i < pStrip->ulVertexNumber; i++)
				{
					auw_Index = pStrip->pMinVertexDataList[i].auw_Index;


					v1 = (Vec*)&_pst_Point[auw_Index];

					memcpy(&v3, (Vec*)&pst_Normals[auw_Index], sizeof(Vec));
					VECScale(&v3, &v3, 0.10f);
					VECAdd(v1, &v3, &v2);
					
					GXPosition3f32(v1->x, v1->y, v1->z);
			        GXColor1u32(col1);
					GXPosition3f32(v2.x, v2.y, v2.z);
			        GXColor1u32(col2);
				}
				
			GXEnd();
		}
	}
}


void GXI_DBG_DrawLight(Vec *pos)
{
    //Mtx  mv;  // Modelview matrix.
    //Vec *pos;
    //CVector3 ldir;
    //LIGHT_tdst_Light *light;
    
   // u32 Red = 0xFF0000FF;
    u32 Yellow = 0xFFFF00FF;
    
	// Draw sphere where light is
	GXI_DBG_DrawPointInViewSpace(pos,Yellow); 
	    

   /* GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_ENABLE);
    
    // draw a light mark
    //osrSetCurrentMatrices((Mtx*)&osrGlobal->eye_matrix_4x4, GX_PNMTX0);
    //MTXInvXpose((Mtx*)&osrGlobal->eye_matrix_4x4, mv);
    //GXLoadNrmMtxImm(mv, GX_PNMTX0);
    MTXIdentity(mv);
    GXLoadPosMtxImm(mv, GX_PNMTX0);
    
	GX_GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_CLR0,GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_NRM, GX_NONE);
    GX_GXSetVtxDesc(GX_VA_TEX0,GX_NONE);
#ifdef USE_PERPECTIVE_CORRECT_COLORS
    GX_GXSetVtxDesc(GX_VA_CLR1,GX_NONE);
#endif

    GX_GXSetCullMode(GX_CULL_NONE);
    
    GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(0);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	
	GX_GXSetChanCtrl(  GX_COLOR0A0,
    			   GX_DISABLE,		// enable Channel
    			   GX_SRC_VTX,		// amb source
    			   GX_SRC_VTX,		// mat source
    			   GX_LIGHT_NULL,	// light mask
    			   GX_DF_CLAMP,		// diffuse function
    			   GX_AF_NONE );	// atten   function
    
    GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
    			   
    
  /*  for(i=0; i<(&GDI_gpst_CurDD->st_LightList)->ul_Current; i++)
    {
        if((light=((&GDI_gpst_CurDD->st_LightList)[i])))
        {
        	if(light->type == eLightType_Ambient)
			   	continue;
    
    		pos = (Vec*)&light->stMatrix.stTranslationVector;
    		pos.x = x;pos.y=y;pos.z=z;
    		light->stMatrix.stTransformMatrix.GetColumn(2, ldir);
    	*/	/*
			GXBegin(GX_LINES, GX_VTXFMT0, 6);
		        GXPosition3f32(pos->x, pos->y, 700.0f);
		        GXColor1u32((u32)Red);
		        GXPosition3f32(pos->x, pos->y, -700.0f);
		        GXColor1u32((u32)Red);

		        GXPosition3f32(pos->x, -480.0f, pos->z);
		        GXColor1u32((u32)Red);
		        GXPosition3f32(pos->x,  480.0f, pos->z);
		        GXColor1u32((u32)Red);

		        GXPosition3f32(-640.0f, pos->y, pos->z);
		        GXColor1u32((u32)Red);
		        GXPosition3f32( 640.0f, pos->y, pos->z);
		        GXColor1u32((u32)Red);

		        //GXPosition3f32(pos->x, pos->y, pos->z);
		        //GXColor1u32((u32)Yellow);
		        //GXPosition3f32(ldir(0), ldir(1), ldir(2));
		        //GXColor1u32((u32)Yellow);
		        
		    GXEnd();
		    
		    
	/*	}
	}*//*
	
	//GXDrawDone();
	GXLoadPosMtxImm(*(Mtx*)&GXI_Global_ACCESS(current_modelview_matrix), GX_PNMTX0);*/
}



void GXI_DBG_DrawVectorInWorldSpaceFromCamera(MATH_tdst_Vector *_pVector,float _fScale)
{
	f32 old_projection[GX_PROJECTION_SZ];	
	MATH_tdst_Vector PointA,PointB;
	MATH_tdst_Vector Camera,LookAt,Right,Down;
    float fCurrentFocale;

	GXGetProjectionv( old_projection );
    GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_ENABLE);
	GXSetLineWidth(20, GX_TO_ZERO);	
    
	GXLoadPosMtxImm(*(Mtx*)&GXI_Global_ACCESS(current_modelview_matrix), GX_PNMTX0);
    //MTXIdentity(mv);
    //GXLoadPosMtxImm(mv, GX_PNMTX0);
    
	GX_GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_CLR0,GX_DIRECT);
    GX_GXSetVtxDesc(GX_VA_NRM, GX_NONE);
    GX_GXSetVtxDesc(GX_VA_TEX0,GX_NONE);
#ifdef USE_PERPECTIVE_CORRECT_COLORS
    GX_GXSetVtxDesc(GX_VA_CLR1,GX_NONE);
#endif

    GX_GXSetCullMode(GX_CULL_NONE);
    
    GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(0);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	
	GX_GXSetChanCtrl(  GX_COLOR0A0,
    			   GX_DISABLE,		// enable Channel
    			   GX_SRC_VTX,		// amb source
    			   GX_SRC_VTX,		// mat source
    			   GX_LIGHT_NULL,	// light mask
    			   GX_DF_CLAMP,		// diffuse function
    			   GX_AF_NONE );	// atten   function
    
    GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
    
 	fCurrentFocale = 1.0f / fNormalTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2);
	
    MATH_CopyVector(&Camera,&(GDI_gpst_CurDD->st_Camera.st_Matrix.T));
    MATH_InitVector(&Right,GDI_gpst_CurDD->st_Camera.st_Matrix.Ix,GDI_gpst_CurDD->st_Camera.st_Matrix.Iy,GDI_gpst_CurDD->st_Camera.st_Matrix.Iz);
    MATH_InitVector(&Down,GDI_gpst_CurDD->st_Camera.st_Matrix.Jx,GDI_gpst_CurDD->st_Camera.st_Matrix.Jy,GDI_gpst_CurDD->st_Camera.st_Matrix.Jz);
    MATH_InitVector(&LookAt,GDI_gpst_CurDD->st_Camera.st_Matrix.Kx,GDI_gpst_CurDD->st_Camera.st_Matrix.Ky,GDI_gpst_CurDD->st_Camera.st_Matrix.Kz);
    
    MATH_AddScaleVector(&PointA,&Camera,&LookAt,fCurrentFocale);
    MATH_AddScaleVector(&PointB,&PointA,_pVector,_fScale);
    
	GXBegin(GX_LINES, GX_VTXFMT0, 2);
	GXPosition3f32(PointA.x, PointA.y, PointA.z);
	GXColor1u32(0xFF0000FF);
	GXPosition3f32(PointB.x, PointB.y, PointB.z);
	GXColor1u32(0xFF0000FF);
	GXEnd();
	GXSetProjectionv( old_projection );
}
