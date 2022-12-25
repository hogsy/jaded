
#include "GX8water.h"
//DECOMMENT IT IF YOU DON'T WANT THE WATER
/*#ifndef _NO_WATER_
	#define _NO_WATER_
#endif
  */

//VERY DIRTY...TO BE CHANGED
#include "GDInterface\GDInterface.h"
#include "BASe\BENch\BENch.h"
#include "MATHS\math.h"

#include "Texture\Tex_Mesh_Internal.h"

//XBOX HELPER LIBRARY
#include <xgraphics.h>

//////////////////////REFERENCE TO GLOBAL DEVICE AND OTHER ENGINE STUFF ////////////////////////////
extern Gx8_tdst_SpecificData	*p_gGx8SpecificData;
extern GDI_tdst_DisplayData *GDI_gpst_CurDD;
extern OBJ_tdst_GameObject ** AI_gpst_MainActors;
extern float GlobalWaterZ;
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////REFLECTION TEXTURE SIZE AND VIEWPORT SIZE///////////////////////////////////////////
#define TEXTURE_SIZE 256
#define ALPHA_TEXTURE_SIZE 128
#define TEXTURE_REF_SIZE 128
#define TEXTURE_ALPHA_SIZE 7
#define SCR_W 640
#define SCR_H 480
#define ALPHA_CLIPPING_Z		50.0f
////////////////////////////////////////////////////////////////////////////////////////////////////

#define DX_SetRenderState(a, b)		IDirect3DDevice8_SetRenderState(p_gGx8SpecificData->mp_D3DDevice, a, b)
#define DX_GetRenderTarget(a)		IDirect3DDevice8_GetRenderTarget(p_gGx8SpecificData->mp_D3DDevice, a)
#define DX_GetDepthStencilSurface(a) IDirect3DDevice8_GetDepthStencilSurface(p_gGx8SpecificData->mp_D3DDevice, a)
#define DX_SetRenderTarget(a, b)	IDirect3DDevice8_SetRenderTarget(p_gGx8SpecificData->mp_D3DDevice, a, b)
#define DX_SetViewport(a)			IDirect3DDevice8_SetViewport(p_gGx8SpecificData->mp_D3DDevice, a)
#define DX_GetViewport(a)			IDirect3DDevice8_GetViewport(p_gGx8SpecificData->mp_D3DDevice, a)
#define DX_GetRenderState(a, b)		IDirect3DDevice8_GetRenderState(p_gGx8SpecificData->mp_D3DDevice, a, b)
#define DX_GetTransform(a, b)		IDirect3DDevice8_GetTransform(p_gGx8SpecificData->mp_D3DDevice, a, b)
#define DX_SetTexture(a, b)			IDirect3DDevice8_SetTexture(p_gGx8SpecificData->mp_D3DDevice, a, b)
#define DX_DrawIndexedPrimitive(a, b, c, d, e) IDirect3DDevice8_DrawIndexedPrimitive(p_gGx8SpecificData->mp_D3DDevice, a, b, c, d, e)
#define DX_DrawPrimitive(a, b, c)	IDirect3DDevice8_DrawPrimitive(p_gGx8SpecificData->mp_D3DDevice, a, b, c)
#define DX_SetTextureStageState(a, b, c) IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, a, b, c)
#define DX_CreateTexture(a, b, c, d, e, f, g) IDirect3DDevice8_CreateTexture(p_gGx8SpecificData->mp_D3DDevice, a, b, c, d, e, f, g)
#define DX_SetTransform(a, b)		IDirect3DDevice8_SetTransform(p_gGx8SpecificData->mp_D3DDevice, a, b)
#define DX_Clear(a, b, c, d, e, f)	IDirect3DDevice8_Clear(p_gGx8SpecificData->mp_D3DDevice, a, b, c, d, e, f)
#define DX_Begin(a)					IDirect3DDevice8_Begin(p_gGx8SpecificData->mp_D3DDevice, a)
#define DX_SetVertexData4f(a, b, c, d, e) IDirect3DDevice8_SetVertexData4f(p_gGx8SpecificData->mp_D3DDevice, a, b, c, d, e)
#define DX_SetVertexData2f(a, b, c)	IDirect3DDevice8_SetVertexData2f(p_gGx8SpecificData->mp_D3DDevice, a, b, c)
#define DX_ReleaseSurface(s)		if((s) == 0) {} else { IDirect3DSurface8_Release(s); (s) = 0; }

//////////////////////GLOBAL TEXTURES///////////////////////////////////////////////////////////////

//Texture containing water reflection
IDirect3DTexture8 * waterReflectionTexture = NULL;
IDirect3DSurface8 * pWaterReflectionSurface = NULL;
//Depth buffer of water reflection
IDirect3DTexture8 * depthReflectionTexture = NULL;
IDirect3DSurface8 * pDepthReflectionSurface = NULL;

// pointer to Device's surfaces (backbuffer, depth)
IDirect3DSurface8 * pDeviceBackBuffer = 0;
IDirect3DSurface8 * pDeviceDepthBuffer = 0;

//Texture used to clip "stuff" with the water plane
IDirect3DTexture8 * alphaTexture = NULL;

#if defined( DRAW_SUN_LIGHT )
//Alpha mask used for highlihgt over the water (sun)
IDirect3DTexture8 * sunAlphaTexture = NULL;

//A copy of the reflection texture (used as source for sun's highlights)
IDirect3DTexture8 * waterReflectionCopyTexture = NULL;

//ALPHA MASK TEXTURE (USED TO DECOUPLE ENVIRONMENT REFLECTION BY SUN REFLECTION)
IDirect3DTexture8 * sunReflectionAlphaMask = NULL;
#endif

// View Matrix used for Reflection
//	MATH_tdst_Matrix g_ReflectionMatrix_View;


////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////GLOBAL...///////////////////////////////////////////////////////////
DWORD textAddressU;
DWORD textAddressV;
DWORD waterColor;
////////////////////////////////////////////////////////////////////////////////////////////////////

//Global that tell if a map has the water or not////////////////////////////////////////////////////
extern void *pst_GlobalsWaterParams;
////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////TRANSFORM FLOAT TO DWORD FOR DIRECTX FUNCTIONS ///////////////////////////////////
__inline DWORD Gx8_FloatToDW(float f)
{
	return *((DWORD*)&f);
}
////////////////////////////////////////////////////////////////////////////////////////////////////

//Color buffer and depth buffer
static IDirect3DSurface8 *gs_pOldSurface;
static IDirect3DSurface8 *gs_pOldDepthSurface;
static D3DMATRIX gs_DXOBJECT;
static DWORD gs_cullMode;
static DWORD gs_ZWriteEnable;
static D3DVIEWPORT8 OldViewData;

void Gx8_PrepareDrawElementReflectedInTexture(void)
{
	
#ifndef _NO_WATER_
	//viewport data
	D3DVIEWPORT8 viewData = { 0, 0, TEXTURE_SIZE, TEXTURE_SIZE, 0.0f, 1.0f };
return;
	ERR_X_Assert(pst_GlobalsWaterParams);

	_GSP_BeginRaster(48);

	//Save the back buffer
	DX_GetRenderTarget(&gs_pOldSurface);

	//save the depth buffer
	DX_GetDepthStencilSurface(&gs_pOldDepthSurface);

	//Get the viewport size
	DX_GetViewport(&OldViewData);

	//Set the global texture (and depth) as render target
	DX_SetRenderTarget(pWaterReflectionSurface, pDepthReflectionSurface);

	//Set the viewport size and CCW culling
	DX_SetViewport(&viewData);
	DX_GetRenderState(D3DRS_CULLMODE, &gs_cullMode);
	if(gs_cullMode == D3DCULL_CW)
		DX_SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	else if(gs_cullMode == D3DCULL_CCW)
		DX_SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	//SAVE DEVICE MATRIX
	DX_GetTransform(D3DTS_WORLD, &gs_DXOBJECT);

	//Get ZWrite stae
	DX_GetRenderState(D3DRS_ZWRITEENABLE, &gs_ZWriteEnable);

	//Dont draw the sky in Z buffer
	if(Gx8_IsSky())
	{
		DX_SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}
	else
	{
		DX_SetTexture(1, (IDirect3DBaseTexture8*)alphaTexture);
	}

	//Set the matrix to reflect the object about water plane
	Gx8_SetReflectionMatrix(GDI_gpst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix);
#endif
}

//Draw in the global reflection texture the flipped element  
void Gx8_DrawElementReflectedInTexture(int VertCount, int IndexOffset, int NumPrimitive)
{
#ifndef _NO_WATER_
	//Draw the object
return; //yoann
	DX_DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, VertCount,  IndexOffset, NumPrimitive);
#endif
}

void Gx8_UnprepareDrawElementReflectedInTexture(void)
{
#ifndef _NO_WATER_
	//If it was sky...enable zwriting
	return;
	if(Gx8_IsSky())
	{
		DX_SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
	else
	{
		DX_SetTexture(1, NULL);
		//DX_SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE); 
	}

	//Reset the render target
	DX_SetRenderTarget(gs_pOldSurface, gs_pOldDepthSurface);

	//TiZ NOTE:
	//do not use DX_SetViewport(&p_gGx8SpecificData->V8), because it refer
	//to the main viewport, tipically it's the fullscreen.
	//Instead get the viewport in Gx8_PrepareDrawElementReflectedInTexture and
	//use the saved value to restore the right viewport. 
	//Using this will draw all the skinned object in the right way!
	DX_SetViewport(&OldViewData);

	//////////VERY IMPORTANT...RELEASE THE SURFACES//////////////////////////////////////////////////
	DX_ReleaseSurface(gs_pOldSurface);
	DX_ReleaseSurface(gs_pOldDepthSurface);

	//RETURN TO PREVIOUS MATRIX
	DX_SetTransform(D3DTS_WORLD, &gs_DXOBJECT);

	// restore cullMode
//	if ( gs_cullMode != D3DCULL_NONE )
		DX_SetRenderState(D3DRS_CULLMODE, gs_cullMode);

	DX_SetRenderState(D3DRS_ZWRITEENABLE, gs_ZWriteEnable);


	//End of performance counting
	_GSP_EndRaster(48);	
#endif
}

//Create global reflection texture (color and depth surfaces)
void Gx8_CreateGlobalTexture(void)
{

	//Create color (reflection) texture
	DX_CreateTexture(TEXTURE_SIZE, 	//Width
					 TEXTURE_SIZE, 	//Height
					 1,   //Just 1 mip
					 0,   //Not used
					 D3DFMT_A8R8G8B8, //Color format
					 0, //Not used
					 &waterReflectionTexture //Destination pointer
					);

	//Create depht texture
	DX_CreateTexture(TEXTURE_SIZE, 	//Width
					 TEXTURE_SIZE, 	//Height
					 1,   //Just 1 mip
					 0,   //Not used
					 D3DFMT_LIN_D24S8, //Depth
					 0, //Not used
					 &depthReflectionTexture //Destination pointer
					);

	ERR_X_Assert(waterReflectionTexture);
	ERR_X_Assert(depthReflectionTexture);

	//Get texture surface
	IDirect3DTexture8_GetSurfaceLevel(waterReflectionTexture, 0, &pWaterReflectionSurface);

	//Get depth surface
	IDirect3DTexture8_GetSurfaceLevel(depthReflectionTexture, 0, &pDepthReflectionSurface);
#if defined( DRAW_SUN_LIGHT )
	//CREATE SUN REFLECTION MAP
	DX_CreateTexture(TEXTURE_SIZE, 	//Width
					 TEXTURE_SIZE, 	//Height
					 1,   //Just 1 mip
					 0,   //Not used
					 D3DFMT_A8R8G8B8, //Color format
					 0, //Not used
					 &sunReflectionAlphaMask //Destination pointer
					);
#endif
}


//Set the reflection matrix for the obeject in the device........
void Gx8_SetReflectionMatrix(MATH_tdst_Matrix* _pst_Matrix)
{
	// World Matrix used for Reflection
	MATH_tdst_Matrix g_ReflectionMatrix_World;

	static float fEcrasement = 1.0f;
	static float fScale = 1.0f;
	
	static float fXval = 0.f;
	static float fZval = 0.f;
	static float fYval = 0.f;

	MATH_MakeOGLMatrix(&g_ReflectionMatrix_World, _pst_Matrix);
	g_ReflectionMatrix_World.Iz= -g_ReflectionMatrix_World.Iz* fScale;
	g_ReflectionMatrix_World.Jz= -g_ReflectionMatrix_World.Jz* fScale;
	g_ReflectionMatrix_World.Kz= -g_ReflectionMatrix_World.Kz* fScale;
	g_ReflectionMatrix_World.T.x -=fXval;
	g_ReflectionMatrix_World.T.y -=fYval;
	g_ReflectionMatrix_World.T.z -=(1.0f + fEcrasement) * (g_ReflectionMatrix_World.T.z - GlobalWaterZ - fZval);
		
	DX_SetTransform(D3DTS_WORLD, (const struct _D3DMATRIX *)&g_ReflectionMatrix_World);
}


// Start reflection 
void Gx8_BeginReflectionDraw(void)
{
#ifdef _NO_WATER_
#else
	#define M_ConvertColor(a) (a & 0xff00ff00) | ((a & 0xff) << 16) | ((a & 0xff0000) >> 16)

	static bool alphaTextureCreated=false;
	static bool globTextureCreated=false;
	static bool backTextureCreated=false;
	static float	ZOffset=0.5f;

	//DATA
	float determinante;

	D3DMATRIX viewMatrix;
	D3DMATRIX viewInvMatrix;
	D3DMATRIX rot90Matrix;
	D3DMATRIX traMatrix;
	MATH_tdst_Matrix tempMatrix;
	WTR_Generator_Struct *pWTRStruct = pst_GlobalsWaterParams;

// Texture 1 Matrix used for Reflection UV
	D3DMATRIX g_ReflectionMatrix_UV;
return;
	//Return if this map hasn't the water
	if(!pst_GlobalsWaterParams)
	{
//TiZ: WHY????		DX_SetTextureStageState(1, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE);

		return;
	}
	_GSP_BeginRaster(48);	

	//COMPUTE THE MATRIX
	MATH_InvertMatrix(&tempMatrix, &GDI_gpst_CurDD->st_Camera.st_Matrix);
	tempMatrix.w=1.0f;
	tempMatrix.Sx=tempMatrix.Sy=tempMatrix.Sz=0.0f;

	//Invert the matrix
	D3DXMatrixInverse(&viewInvMatrix, &determinante, ( const struct _D3DMATRIX *) &tempMatrix); 

	//Rotation matrix (since the world is rotated)
	D3DXMatrixIdentity(&rot90Matrix);
	D3DXMatrixRotationX(&rot90Matrix, ((90.0f*3.1415926f)/180.0f));
 
	//Translation matrix
	D3DXMatrixIdentity(&traMatrix);
	traMatrix._42 = ZOffset + GlobalWaterZ;

	//Concatenate the matrix
	D3DXMatrixMultiply(&viewMatrix, &viewInvMatrix, &rot90Matrix);
	D3DXMatrixMultiply(&g_ReflectionMatrix_UV, &viewMatrix, &traMatrix);

	//Set this texture matrix for the requested texture stage
	DX_SetTransform(D3DTS_TEXTURE1, &g_ReflectionMatrix_UV);

	//Set DX matrix management
	DX_SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	DX_SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	DX_SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	DX_SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	DX_SetTextureStageState(1, D3DTSS_ALPHAKILL, D3DTALPHAKILL_ENABLE);
	DX_SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	DX_SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	DX_SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	DX_SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	DX_SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	DX_SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);

	//Create (if needed) the ALPHA TEXTURE used for clipping
	if(!alphaTextureCreated)
	{	
		alphaTextureCreated=true;
		Gx8_CreateAlphaTexture(&alphaTexture, 0);
#if defined( DRAW_SUN_LIGHT )
		Gx8_CreateSunAlphaTexture(&sunAlphaTexture);
#endif
	}

	//Create (if needed) global reflection texture and depth buffer
	if(!globTextureCreated)
	{
		globTextureCreated=true;
		Gx8_CreateGlobalTexture();
	}

	//Save the back buffer
	DX_GetRenderTarget(&pDeviceBackBuffer);

	//save the depth buffer
	DX_GetDepthStencilSurface(&pDeviceDepthBuffer);

	//Set the global texture (and depth) as render target
	DX_SetRenderTarget(pWaterReflectionSurface, pDepthReflectionSurface);

	waterColor=(DWORD)((255<<24)|(174<<16)|(203<<8)|(226));
	waterColor =(DWORD)M_ConvertColor((pWTRStruct->pColorTable[256]));
	//CLEAR the color/depth
	DX_Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET|D3DCLEAR_STENCIL, waterColor, 1, 0);

	//Return to device color/depth buffer
	DX_SetRenderTarget(pDeviceBackBuffer, pDeviceDepthBuffer);

	DX_SetViewport(&p_gGx8SpecificData->V8);

	DX_ReleaseSurface(pDeviceBackBuffer);
	DX_ReleaseSurface(pDeviceDepthBuffer);

	_GSP_EndRaster(48);	
#endif
}


void Gx8_EndReflectionDraw(void)
{
return;
#ifdef _NO_WATER_
#else

	if(!pst_GlobalsWaterParams)
		return;

	_GSP_BeginRaster(48);	
	_GSP_EndRaster(48);

#endif

}


//Create the alpha texture (used for water level clipping)
void Gx8_CreateAlphaTexture(IDirect3DTexture8 **theTexture, int direction)
{
	BYTE *pImage;
	D3DLOCKED_RECT theRect;
	int counter;

	DX_CreateTexture(ALPHA_TEXTURE_SIZE, 
					 ALPHA_TEXTURE_SIZE, 
					 1, 
					 0, 
					 D3DFMT_A8, 
					 0, 
					 theTexture
					);

	//Lock the surface
	IDirect3DTexture8_LockRect(*theTexture, 0, &theRect, NULL, NULL);

	pImage=theRect.pBits;

	for(counter=0;counter<ALPHA_TEXTURE_SIZE;++counter)
	{
		//Write a line (transparent or sold)
		if(counter<ALPHA_TEXTURE_SIZE/2)
		{
			if(!direction)
				memset(pImage, 0, ALPHA_TEXTURE_SIZE);
			else
				memset(pImage, 255, ALPHA_TEXTURE_SIZE);

		}
		else
		{
			if(!direction)
				memset(pImage, 255, ALPHA_TEXTURE_SIZE);
			else
				memset(pImage, 0, ALPHA_TEXTURE_SIZE);

		}

		{

//			memset(pImage, (255), ALPHA_TEXTURE_SIZE);
		}
		pImage+=theRect.Pitch;
			
	}

/*	pImage=theRect.pBits;

	for(counter=0;counter<ALPHA_TEXTURE_SIZE;++counter)
	{
		//Write a line (transparent or sold)
		if(counter<ALPHA_TEXTURE_SIZE/2)
		{
//HACK			if(!direction)
				memset(pImage, 0, ALPHA_TEXTURE_SIZE);
//HACK			else
//HACK				memset(pImage, 255, ALPHA_TEXTURE_SIZE);

		}
		else
		{
//HACK			if(!direction)
				memset(pImage, (255*128/counter)/2, ALPHA_TEXTURE_SIZE);
//HACK			else
//HACK				memset(pImage, 0, ALPHA_TEXTURE_SIZE);

		}
		pImage+=theRect.Pitch;
			
	}
*/

	IDirect3DTexture8_UnlockRect(*theTexture, 0);
}

#if defined( DRAW_SUN_LIGHT )
//Draw on water reflection the sky lights (for highlights)
void Gx8_DrawSkyLight(void)
{
return;
#ifdef _NO_WATER_
#else
	
	LIGHT_tdst_List *_pst_LightList;
	OBJ_tdst_GameObject			**ppst_LightNode, **ppst_Last;
	LIGHT_tdst_Light *LIGHT_gpst_Cur;

	_pst_LightList = &GDI_gpst_CurDD->st_LightList;
    ppst_LightNode = _pst_LightList->dpst_Light;
	ppst_Last = ppst_LightNode + _pst_LightList->ul_Current;

	//PREPARE ALPHA MASK
	Gx8_PrepareAlphaMaskTexture();

	while(ppst_LightNode < ppst_Last)
	{
		LIGHT_gpst_Cur = (LIGHT_tdst_Light *) (*ppst_LightNode)->pst_Extended->pst_Light;
        {
            //Find "sun/moon" lights
			if(LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_OceanSpecular)
			{

				//Get the light billboard
				MATH_tdst_Vector p4Points[4];
				Gx8_GetSkyLightPoints(p4Points, &(*ppst_LightNode)->pst_GlobalMatrix->T, &GDI_gpst_CurDD->st_Camera.st_Matrix.T);

				//EFFECTIVELY DRAW THE LIGHT
				Gx8_DrawLight(p4Points);

				//DRAW THE LIGHT IN THE ALPHA MASK
				Gx8_DrawLightInAlphaMask(p4Points);
			}
		}
		ppst_LightNode++;
	}
#endif
}

//Effectively draw light on water reflection
void Gx8_DrawLight(MATH_tdst_Vector *p4Points)
{
	//Pipeline matrix
	D3DMATRIX DXVIEW;
	D3DMATRIX DXOBJECT;

	//Color buffers
	IDirect3DSurface8 *pSurface;
	IDirect3DSurface8 *pOldSurface;

	//Depth buffers
	IDirect3DSurface8 *pDepthSurface;
	IDirect3DSurface8 *pOldDepthSurface;

	//Draw light mask
	D3DVIEWPORT8 viewData = { 0, 0, TEXTURE_SIZE, TEXTURE_SIZE, 0.0f, 1.0f };

	//Get texture surface
	IDirect3DTexture8_GetSurfaceLevel(waterReflectionTexture, 0, &pSurface);

	//Get depth surface
	IDirect3DTexture8_GetSurfaceLevel(depthReflectionTexture, 0, &pDepthSurface);

	//Save the back buffer
	DX_GetRenderTarget(&pOldSurface);

	//save the depth buffer
	DX_GetDepthStencilSurface(&pOldDepthSurface);

	//Set the global texture (and depth) as render target
	DX_SetRenderTarget(pSurface, pDepthSurface);

	//SAVE DEVICE MATRIX
	DX_GetTransform(D3DTS_VIEW, &DXVIEW);

	DX_GetTransform(D3DTS_WORLD, &DXOBJECT);

	//BLENDING FOR WATER SUN REFLECTIONs//////////////////////////////////////////////////////
	//Alpha
	DX_SetTexture(0, (IDirect3DBaseTexture8 *)sunAlphaTexture);
	
	//Reflection
	DX_SetTexture(1, (IDirect3DBaseTexture8 *)waterReflectionTexture);


	//STAGE 0
	DX_SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1); 
	DX_SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	DX_SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1); 
	DX_SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);


	//STAGE 1
	DX_SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1); 
	DX_SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	DX_SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1); 
	DX_SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);

	////////////ALPHA BLENDING....USE ADD TO SIMULATE SUN REFLECTION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	DX_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	
	//ADD TO SIMULATE REFLECTION'S SPOT
	DX_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	
	DX_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	DX_SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	DX_SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	DX_SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	DX_SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////PREPARE TEXTURE COORD GENERATION
	Gx8_PrepareTextureCoordForLights();

	//Set the viewport size and NO CULLING
	DX_SetViewport(&viewData);

	DX_SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	DX_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	DX_Begin(D3DPT_TRIANGLELIST);
	
	//First triangle
	DX_SetVertexData4f(D3DVSDE_DIFFUSE, 1.0f, 0.0f, 0.0f, 1.0f);
	DX_SetVertexData2f(D3DVSDE_TEXCOORD0, 0.0f, 1.0f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, p4Points[0].x, p4Points[0].y, p4Points[0].z, 1.0f);

	DX_SetVertexData4f(D3DVSDE_DIFFUSE, 1.0f, 0.0f, 0.0f, 1.0f);
	DX_SetVertexData2f(D3DVSDE_TEXCOORD0, 1.0f, 0.0f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, p4Points[2].x, p4Points[2].y, p4Points[2].z, 1.0f);

	DX_SetVertexData4f(D3DVSDE_DIFFUSE, 1.0f, 0.0f, 0.0f, 1.0f);
	DX_SetVertexData2f(D3DVSDE_TEXCOORD0, 0.0f, 0.0f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, p4Points[3].x, p4Points[3].y, p4Points[3].z, 1.0f);

	//Second triangle
	DX_SetVertexData4f(D3DVSDE_DIFFUSE, 1.0f, 0.0f, 0.0f, 1.0f);
	DX_SetVertexData2f(D3DVSDE_TEXCOORD0, 1.0f, 0.0f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, p4Points[2].x, p4Points[2].y, p4Points[2].z, 1.0f);

	DX_SetVertexData4f(D3DVSDE_DIFFUSE, 1.0f, 0.0f, 0.0f, 1.0f);
	DX_SetVertexData2f(D3DVSDE_TEXCOORD0, 0.0f, 1.0f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, p4Points[0].x, p4Points[0].y, p4Points[0].z, 1.0f);

	DX_SetVertexData4f(D3DVSDE_DIFFUSE, 1.0f, 0.0f, 0.0f, 1.0f);
	DX_SetVertexData2f(D3DVSDE_TEXCOORD0, 1.0f, 1.0f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, p4Points[1].x, p4Points[1].y, p4Points[1].z, 1.0f);

	IDirect3DDevice8_End(p_gGx8SpecificData->mp_D3DDevice);

	///RESET 
	//Reset the render target
	DX_SetRenderTarget(pOldSurface, pOldDepthSurface);

	DX_SetViewport(&p_gGx8SpecificData->V8);

	//////////VERY IMPORTANT...RELEASE THE SURFACES//////////////////////////////////////////////////
	DX_ReleaseSurface(pSurface);
	DX_ReleaseSurface(pOldSurface);
	DX_ReleaseSurface(pDepthSurface);
	DX_ReleaseSurface(pOldDepthSurface);
	//////////////////////////////////////////////////////////////////////////////////////////////////	

	//RETURN TO PREVIOUS MATRIX
	DX_SetTransform(D3DTS_VIEW, &DXVIEW);

	DX_SetTransform(D3DTS_WORLD, &DXOBJECT);

	DX_SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE); 

	DX_SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

	DX_SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
  
	//Release texture coord generation
	Gx8_ReleaseTextureCoordForLights();

	DX_SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	DX_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	//NO MORE ALPHA BLENDING
	DX_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);


	DX_SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE); 

	DX_SetTexture(1, NULL);
}




//Effectively draw light on water reflection ALPHA MASK
void Gx8_DrawLightInAlphaMask(MATH_tdst_Vector *p4Points)
{
	//Pipeline matrix
	D3DMATRIX DXVIEW;
	D3DMATRIX DXOBJECT;

	//Color buffers
	IDirect3DSurface8 *pSurface;
	IDirect3DSurface8 *pOldSurface;

	//Depth buffers
	IDirect3DSurface8 *pDepthSurface;
	IDirect3DSurface8 *pOldDepthSurface;

	//Draw light mask
	D3DVIEWPORT8 viewData = { 0, 0, TEXTURE_SIZE, TEXTURE_SIZE, 0.0f, 1.0f };


	//Get texture surface
	IDirect3DTexture8_GetSurfaceLevel(sunReflectionAlphaMask, 0, &pSurface);

	//Get depth surface
	IDirect3DTexture8_GetSurfaceLevel(depthReflectionTexture, 0, &pDepthSurface);

	//Save the back buffer
	DX_GetRenderTarget(&pOldSurface);

	//save the depth buffer
	DX_GetDepthStencilSurface(&pOldDepthSurface);

	//Set the global texture (and depth) as render target
	DX_SetRenderTarget(pSurface, pDepthSurface);

	//SAVE DEVICE MATRIX
	DX_GetTransform(D3DTS_VIEW, &DXVIEW);

	DX_GetTransform(D3DTS_WORLD, &DXOBJECT);

	//BLENDING FOR WATER SUN REFLECTIONs//////////////////////////////////////////////////////
	//Alpha
	DX_SetTexture(0, (IDirect3DBaseTexture8 *)sunAlphaTexture);

	//STAGE...DRAW SIMPLY THE ALPHA
	DX_SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1); 
	DX_SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	DX_SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1); 
	DX_SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);


	////////////ALPHA BLENDING....USE ADD TO SIMMULATE SUN REFLECTION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	DX_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	DX_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	DX_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	DX_SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	DX_SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//Set the viewport size and NO CULLING
	DX_SetViewport(&viewData);

	DX_SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	DX_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	DX_Begin(D3DPT_TRIANGLELIST);
	
	//First triangle
	DX_SetVertexData4f(D3DVSDE_DIFFUSE, 1.0f, 0.0f, 0.0f, 1.0f);
	DX_SetVertexData2f(D3DVSDE_TEXCOORD0, 0.0f, 1.0f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, p4Points[0].x, p4Points[0].y, p4Points[0].z, 1.0f);

	DX_SetVertexData4f(D3DVSDE_DIFFUSE, 1.0f, 0.0f, 0.0f, 1.0f);
	DX_SetVertexData2f(D3DVSDE_TEXCOORD0, 1.0f, 0.0f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, p4Points[2].x, p4Points[2].y, p4Points[2].z, 1.0f);

	DX_SetVertexData4f(D3DVSDE_DIFFUSE, 1.0f, 0.0f, 0.0f, 1.0f);
	DX_SetVertexData2f(D3DVSDE_TEXCOORD0, 0.0f, 0.0f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, p4Points[3].x, p4Points[3].y, p4Points[3].z, 1.0f);

	//Second triangle
	DX_SetVertexData4f(D3DVSDE_DIFFUSE, 1.0f, 0.0f, 0.0f, 1.0f);
	DX_SetVertexData2f(D3DVSDE_TEXCOORD0, 1.0f, 0.0f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, p4Points[2].x, p4Points[2].y, p4Points[2].z, 1.0f);

	DX_SetVertexData4f(D3DVSDE_DIFFUSE, 1.0f, 0.0f, 0.0f, 1.0f);
	DX_SetVertexData2f(D3DVSDE_TEXCOORD0, 0.0f, 1.0f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, p4Points[0].x, p4Points[0].y, p4Points[0].z, 1.0f);

	DX_SetVertexData4f(D3DVSDE_DIFFUSE, 1.0f, 0.0f, 0.0f, 1.0f);
	DX_SetVertexData2f(D3DVSDE_TEXCOORD0, 1.0f, 1.0f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, p4Points[1].x, p4Points[1].y, p4Points[1].z, 1.0f);

	IDirect3DDevice8_End(p_gGx8SpecificData->mp_D3DDevice);

	///RESET 
	//Reset the render target
	DX_SetRenderTarget(pOldSurface, pOldDepthSurface);

	DX_SetViewport(&p_gGx8SpecificData->V8);

	//////////VERY IMPORTANT...RELEASE THE SURFACES//////////////////////////////////////////////////
	DX_ReleaseSurface(pSurface);
	DX_ReleaseSurface(pOldSurface);
	DX_ReleaseSurface(pDepthSurface);
	DX_ReleaseSurface(pOldDepthSurface);
	//////////////////////////////////////////////////////////////////////////////////////////////////	


	//RETURN TO PREVIOUS MATRIX
	DX_SetTransform(D3DTS_VIEW, &DXVIEW);

	DX_SetTransform(D3DTS_WORLD, &DXOBJECT);
	DX_SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE); 
	DX_SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	DX_SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	DX_SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	DX_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	//NO MORE ALPHA BLENDING
	DX_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	DX_SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE); 
	DX_SetTexture(1, NULL);
}

#endif

//Create light's sprite
void Gx8_GetSkyLightPoints(MATH_tdst_Vector *p4Points, MATH_tdst_Vector *pLPos, MATH_tdst_Vector *pCPos)
{
	int counter;
	MATH_tdst_Vector Verticale, Horizontale, H, V;
	float Distance;
	MATH_InitVector(&Verticale, 0.0, 0.0, 1.0);
	MATH_SubVector(&Horizontale, pCPos, pLPos);
	Distance = MATH_f_NormVector(&Horizontale);
	MATH_CrossProduct(&H, &Horizontale, &Verticale);
	MATH_CrossProduct(&V, &H, &Horizontale);
	MATH_NormalizeVector(&H, &H);
	MATH_NormalizeVector(&V, &V);
	MATH_ScaleVector(&H, &H, Distance *1.0f);
	MATH_ScaleVector(&V, &V, Distance *1.0f);
	MATH_SubVector(p4Points+0, pLPos, &H);
	MATH_AddVector(p4Points+1, pLPos, &H);
	MATH_AddVector(p4Points+2, pLPos, &H);
	MATH_SubVector(p4Points+3, pLPos, &H);

	MATH_AddVector(p4Points+0, p4Points+0, &V);
	MATH_AddVector(p4Points+1, p4Points+1, &V);
	MATH_SubVector(p4Points+2, p4Points+2, &V);
	MATH_SubVector(p4Points+3, p4Points+3, &V);

	for(counter=0;counter<4;++counter)
	{
		p4Points[counter].z= -p4Points[counter].z;
	}
}


//Create sky light's alpha mask texture
void Gx8_CreateSunAlphaTexture(IDirect3DTexture8 **theTexture)
{
	BYTE *pImage;
	BYTE *pNoSwizImage;
	BYTE *SwizImage;
	BYTE *temp;
	D3DLOCKED_RECT theRect;
	int counter;

	DX_CreateTexture(TEXTURE_SIZE, 
					 TEXTURE_SIZE, 
					 1, 
					 0, 
					 D3DFMT_A8, 
					 0, 
					 theTexture);

	//Lock the surface
	IDirect3DTexture8_LockRect(*theTexture, 0, &theRect, NULL, NULL);

	pImage=theRect.pBits;

	pNoSwizImage=malloc(TEXTURE_SIZE*TEXTURE_SIZE);
	SwizImage=malloc(TEXTURE_SIZE*TEXTURE_SIZE);
	
	//Do the job
	Gx8_ComputeAlphaSunMap(pNoSwizImage, TEXTURE_ALPHA_SIZE, TEXTURE_SIZE);

	//Swizle the image
	XGSwizzleRect(pNoSwizImage, 
				  TEXTURE_SIZE, 
				  0, //RECT
				  SwizImage, 
				  TEXTURE_SIZE, 
				  TEXTURE_SIZE, 
				  0, //point
				  1);

	temp=SwizImage;
	//Copy the image in the texture

	for(counter=0;counter<TEXTURE_SIZE;++counter)
	{
		memcpy(pImage, SwizImage, TEXTURE_SIZE);

		//Go ahead with pointer
		pImage+=theRect.Pitch;
		SwizImage+=TEXTURE_SIZE;
	}

	IDirect3DTexture8_UnlockRect(*theTexture, 0);

	//Free memory
	free(pNoSwizImage);
	free(temp);
}


#if defined( DRAW_SUN_LIGHT )
//Copy the reflection texture (used as source for sky light rendering)
void Gx8_CopyReflectionTextureForSun(void)
{
	D3DLOCKED_RECT RectSource;
	D3DLOCKED_RECT RectDest;
	
	//Lock the surface
	IDirect3DTexture8_LockRect(waterReflectionTexture, 0, &RectSource, NULL, NULL);

	//Lock the surface
	IDirect3DTexture8_LockRect(waterReflectionCopyTexture, 0, &RectDest, NULL, NULL);

	//Copy memory
	memcpy(RectDest.pBits, RectSource.pBits, RectDest.Pitch*TEXTURE_SIZE);

	IDirect3DTexture8_UnlockRect(waterReflectionTexture, 0);
	IDirect3DTexture8_UnlockRect(waterReflectionCopyTexture, 0);
}
#endif

//Prepare texture coordinates generation for sky light's sprites
void Gx8_PrepareTextureCoordForLights(void)
{
	D3DMATRIX projMatrix;
	D3DMATRIX mulMAtrix;
	D3DMATRIX finMAtrix;

	//Get projection matrix from pipeline
	DX_GetTransform(D3DTS_PROJECTION, &projMatrix);

	projMatrix._22= -projMatrix._22;
	
	
	//Scale and translation matrix
	memset(&mulMAtrix, 0, sizeof(mulMAtrix));

	mulMAtrix._11=0.5f;
	mulMAtrix._22=0.5f;
	mulMAtrix._33=1.0f;
	mulMAtrix._44=1.0f;

	//Translate
	mulMAtrix._41= 0.5f;
	mulMAtrix._42= 0.5f;
	mulMAtrix._43= 0.0f;
	
    D3DXMatrixMultiply(&finMAtrix, &projMatrix, &mulMAtrix);
	
	//Set this texture matrix for the requested texture stage
	DX_SetTransform(D3DTS_TEXTURE1, &finMAtrix);
	DX_SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	DX_SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 | D3DTTFF_PROJECTED);
}

void Gx8_ReleaseTextureCoordForLights(void)
{
	//RELEASE TEXTURE MATRIX TRANSFORM
	DX_SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	DX_SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
}

////////////HELP FUNCTIONS/////////////////////////////
#ifndef _NO_WATER_
//Tell if the current object is the sky
__inline bool Gx8_IsSky(void)
{
/*	if(GDI_gpst_CurDD->pst_CurrentGameObject->pst_Base && GDI_gpst_CurDD->pst_CurrentGameObject->pst_Base->pst_Visu)
	{
		if(GDI_gpst_CurDD->pst_CurrentGameObject->pst_Base->pst_Visu->c_DisplayOrder > 1)
			return true;
	}*/
	return false;
}
#endif

unsigned char Gx8_ComputeAlphaValue(int x, int y , float Half)
{
	float Dista2Ctr, DX, DY;
	DX = (float)x - Half;
	DY = (float)y - Half;
	DX /= Half;
	DY /= Half;
	Dista2Ctr = 1.0f - fSqrt(DX * DX + DY * DY);
	if (Dista2Ctr < 0.0f) return 0;
	
	return (unsigned char)(255.0f * Dista2Ctr);
}

void Gx8_ComputeAlphaSunMap(unsigned char *pDST, int SizePo2, int stride)
{
	float Half;
	int YT, XT;

	Half = (float)(1 << (SizePo2 - 1));

	for (YT = 0 ; YT < (1 << SizePo2) ; YT ++)
	{
		for (XT = 0 ; XT < (1 << SizePo2) ; XT ++)
		{
			 (*(pDST+(XT+YT*stride))) =Gx8_ComputeAlphaValue(XT, YT, Half);
		}
	}
}

float Gx8_CalcDistanceFromCamera(void)
{
	//Return the squared distance between camera and current object
	MATH_tdst_Vector cameraPos;
	MATH_tdst_Vector objectPos;
	MATH_tdst_Vector distVector;

	cameraPos=GDI_gpst_CurDD->st_Camera.st_Matrix.T;
	objectPos=GDI_gpst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix->T;

	MATH_SubVector(&distVector, &objectPos, &cameraPos);

	return MATH_f_DotProduct(&distVector, &distVector);
}

void Gx8_BlueImage(void)
{

	D3DMATRIX DXVIEW;
	D3DMATRIX DXOBJECT;
	D3DMATRIX DXPROJECTION;
	D3DMATRIX TEMPMATRIX;

	//Color buffers
	IDirect3DSurface8 *pSurface;
	IDirect3DSurface8 *pOldSurface;

	//Depth buffers
	IDirect3DSurface8 *pDepthSurface;
	IDirect3DSurface8 *pOldDepthSurface;


	//WATER COLOR
	float r=0;
	float g=10.0f/255.0f;
	float b=50.0f/255.0f;


	//Get texture surface
	IDirect3DTexture8_GetSurfaceLevel(waterReflectionTexture, 0, &pSurface);

	//Get depth surface
	IDirect3DTexture8_GetSurfaceLevel(depthReflectionTexture, 0, &pDepthSurface);


	//Save the back buffer
	DX_GetRenderTarget(&pOldSurface);

	//save the depth buffer
	DX_GetDepthStencilSurface(&pOldDepthSurface);

	//Set the global texture (and depth) as render target
	DX_SetRenderTarget(pSurface, pDepthSurface);
	DX_GetTransform(D3DTS_VIEW, &DXVIEW);
	DX_GetTransform(D3DTS_WORLD, &DXOBJECT);	
	DX_GetTransform(D3DTS_PROJECTION, &DXPROJECTION);	
	D3DXMatrixIdentity(&TEMPMATRIX);
	DX_SetTransform(D3DTS_VIEW, &TEMPMATRIX);
	DX_SetTransform(D3DTS_WORLD, &TEMPMATRIX);	
	DX_SetTransform(D3DTS_PROJECTION, &TEMPMATRIX);	
	DX_SetTextureStageState(0, D3DTSS_COLOROP, D3DTSS_COLORARG1); 
	DX_SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	DX_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	
	//ADD TO SIMULATE REFLECTION'S SPOT
	DX_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	
	DX_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	DX_SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	DX_SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

	DX_Begin(D3DPT_TRIANGLELIST);
	
	//Firs triangle
	DX_SetVertexData4f(D3DVSDE_DIFFUSE, r, g, b, 0.5f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, -1.0f, -1.0f, 1.0f, 1.0f);

	DX_SetVertexData4f(D3DVSDE_DIFFUSE, r, g, b, 0.5f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, 1.0f, 1.0f,  1.0f, 1.0f);

	DX_SetVertexData4f(D3DVSDE_DIFFUSE, r, g, b, 0.5f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, -1.0f, 1.0f, 1.0f, 1.0f);

	//Second triangle
	DX_SetVertexData4f(D3DVSDE_DIFFUSE, r, g, b, 0.5f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, -1.0f, -1.0f,  1.0f, 1.0f);

	DX_SetVertexData4f(D3DVSDE_DIFFUSE, r, g, b, 0.5f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, 1.0f, -1.0f,  1.0f, 1.0f);

	DX_SetVertexData4f(D3DVSDE_DIFFUSE, r, g, b, 0.5f);
	DX_SetVertexData4f(D3DVSDE_VERTEX, 1.0f, 1.0f, 1.0f, 1.0f);
	

	IDirect3DDevice8_End(p_gGx8SpecificData->mp_D3DDevice);

	DX_SetTransform(D3DTS_VIEW, &DXVIEW);
	DX_SetTransform(D3DTS_WORLD, &DXOBJECT);	
	DX_SetTransform(D3DTS_PROJECTION, &DXPROJECTION);	
	DX_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	DX_SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	DX_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	///RESET 
	//Reset the render target
	DX_SetRenderTarget(pOldSurface, pOldDepthSurface);
	DX_SetViewport(&p_gGx8SpecificData->V8);
}

#if defined( DRAW_SUN_LIGHT )

void Gx8_PrepareAlphaMaskTexture(void)
{
	//Color buffers
	IDirect3DSurface8 *pSurface;
	IDirect3DSurface8 *pOldSurface;

	//Depth buffers
	IDirect3DSurface8 *pDepthSurface;
	IDirect3DSurface8 *pOldDepthSurface;

	//Get texture surface
	IDirect3DTexture8_GetSurfaceLevel(sunReflectionAlphaMask, 0, &pSurface);

	//Get depth surface
	IDirect3DTexture8_GetSurfaceLevel(depthReflectionTexture, 0, &pDepthSurface);

	//Save the back buffer
	DX_GetRenderTarget(&pOldSurface);

	//save the depth buffer
	DX_GetDepthStencilSurface(&pOldDepthSurface);

	//Set the global texture (and depth) as render target
	DX_SetRenderTarget(pSurface, pDepthSurface);

	DX_Clear(0, NULL, D3DCLEAR_TARGET, 0, 1, 0);

	//Set the global texture (and depth) as render target
	DX_SetRenderTarget(pOldSurface, pOldDepthSurface);

	DX_SetViewport(&p_gGx8SpecificData->V8);

	DX_ReleaseSurface(pSurface);
	DX_ReleaseSurface(pOldSurface);
	DX_ReleaseSurface(pDepthSurface);
	DX_ReleaseSurface(pOldDepthSurface);
}
#endif

bool CheckForMainActor(void)
{
	return (AI_gpst_MainActors[0]==GetFather(GDI_gpst_CurDD->pst_CurrentGameObject));
}

OBJ_tdst_GameObject *GetFather(OBJ_tdst_GameObject *p_stSon )
{
	if (((p_stSon->ul_IdentityFlags) & OBJ_C_IdentityFlag_Hierarchy) && p_stSon->pst_Base && p_stSon->pst_Base->pst_Hierarchy && (p_stSon->pst_Base->pst_Hierarchy->pst_Father))
	{
		return GetFather(p_stSon->pst_Base->pst_Hierarchy->pst_Father);
	}
	else
	{
		return p_stSon;
	}
}











