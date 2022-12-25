#include "Gx8ShadowBuffer.h"


///////////////////////SPECIFIC SHADOW BUFFER DEFINES //////////////////////////////////////////////
#define MAX_SHADOW_BUFFER_TEXTURES 4

#define SHADOW_BUFFER_WIDHT  640//320 //640
#define SHADOW_BUFFER_HEIGHT 480//240 //480

#define SHADOW_BUFFER_DIFFUSE_TEXTURE_SIZE_W 640 //640
#define SHADOW_BUFFER_DIFFUSE_TEXTURE_SIZE_H 480 //480

////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////GLOBAL SHADOW BUFFER TEXTURES ////////////////////////////////////////////////
ShadowBuffer shadowTextures[MAX_SHADOW_BUFFER_TEXTURES];
bool usedShadowBuffer[MAX_SHADOW_BUFFER_TEXTURES];
////////////////////////////////////////////////////////////////////////////////////////////////////
   
/////////////////////GLOBAL SURFACES ///////////////////////////////////////////////////////////////
IDirect3DSurface8		*pSaveZBuffer;
IDirect3DSurface8       *pSaveRenderTarget;
////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////UNUSED TEXTURE...TO BE CHANGED/////////////////////////////////////////////////
IDirect3DTexture8 *pFakeColorTexture;
IDirect3DSurface8 *pFakeColorTextureSurface;
////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////// DIFFUSE SHADOW BUFFER /////////////////////////////////////////////////////////
IDirect3DTexture8 *pDiffuseShadowBuffer;
IDirect3DTexture8 *pDiffuseShadowBufferZ;
////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////// TELL IF CASTING SHADOW LIGHTS ARE PRESENT /////////////////////////////////////
bool castingShadow;
////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////REFERENCE TO GLOBAL DEVICE AND OTHER ENGINE STUFF ////////////////////////////
extern Gx8_tdst_SpecificData	*p_gGx8SpecificData;
extern GDI_tdst_DisplayData    *GDI_gpst_CurDD;
////////////////////////////////////////////////////////////////////////////////////////////////////


D3DXMATRIX *pCurrentProjection;
ShadowBuffer *pCurrentShadowBuffer;

//Init the system
void Gx8_InitShadowBufferSystem(void)
{

	int counter;
	for(counter=0;counter<MAX_SHADOW_BUFFER_TEXTURES;++counter)
	{
		shadowTextures[counter].pShadowTexture=Gx8_CreateShadowBufferTexture(SHADOW_BUFFER_WIDHT,SHADOW_BUFFER_HEIGHT);
		usedShadowBuffer[counter]=false;
	}

	//Create the fake texture...to be changed//Create the texture (NO LINEAR FORMAT)
	IDirect3DDevice8_CreateTexture
    (
            p_gGx8SpecificData->mp_D3DDevice,
			SHADOW_BUFFER_WIDHT,
			SHADOW_BUFFER_HEIGHT,
            1,
            D3DUSAGE_RENDERTARGET,
            D3DFMT_LIN_A8R8G8B8 ,
            D3DPOOL_DEFAULT,
            &pFakeColorTexture
     );

	//Get the surface for fake color texture
	IDirect3DTexture8_GetSurfaceLevel(pFakeColorTexture,
								      0,
									  &pFakeColorTextureSurface);


	//Create the shadow buffer system diffuse texture....
	Gx8_CreateSBDiffuseTexture(SHADOW_BUFFER_DIFFUSE_TEXTURE_SIZE_W,
													SHADOW_BUFFER_DIFFUSE_TEXTURE_SIZE_H,
													&pDiffuseShadowBuffer,
													&pDiffuseShadowBufferZ);


	//BY DEFAULT NO TEXTURE DIFFUESE AND NO TEXTURE MATRIX
	p_gGx8SpecificData->diffuseInTexture=false;
	p_gGx8SpecificData->needSBMatrix=false;

}

//Low level DX resource creation functions
IDirect3DTexture8 * Gx8_CreateShadowBufferTexture(int widht,int height)
{

	IDirect3DTexture8 * pTexture=0;
	
	//Create the texture (LINEAR FORMAT)
	IDirect3DDevice8_CreateTexture
    (
            p_gGx8SpecificData->mp_D3DDevice,
			widht,
			height,
            1,
            D3DUSAGE_RENDERTARGET,
            D3DFMT_LIN_D24S8,
            D3DPOOL_DEFAULT,
            &pTexture
     );


	
	return pTexture;
}

//Create the texture that will be used as 
void  Gx8_CreateSBDiffuseTexture(int widht,int height,IDirect3DTexture8 **pColor,IDirect3DTexture8 **pDepth)
{	
	//Create the texture (NO LINEAR FORMAT)
	IDirect3DDevice8_CreateTexture
    (
            p_gGx8SpecificData->mp_D3DDevice,
			widht,
			height,
            1,
            D3DUSAGE_RENDERTARGET,
			D3DFMT_LIN_A8R8G8B8 ,
			//D3DFMT_A8R8G8B8 ,
            D3DPOOL_DEFAULT,
            pColor
     );


	IDirect3DDevice8_CreateTexture
    (
            p_gGx8SpecificData->mp_D3DDevice,
			widht,
			height,
            1,
            D3DUSAGE_RENDERTARGET,
            D3DFMT_LIN_D24S8,
			//D3DFMT_D24S8,
            D3DPOOL_DEFAULT,
            pDepth
     );
	
}

//Inteface with shadow buffer system 
int Gx8_GetAvailableShadowBufferNumber(void)
{
	int counter;
	for(counter=0;counter<MAX_SHADOW_BUFFER_TEXTURES;++counter)
	{
		if(!usedShadowBuffer[counter])
		{
			return counter;
		}
	}

	//No more available textures
	return -1;
}


int Gx8_SetShadowBufferAsTarget(MATH_tdst_Matrix *pMatrix,void	*pst_Light,float fov)
{

	IDirect3DSurface8		*pShadowDepthSurface;


	//Get the avaliable shadow buffer
	int availableShadow;
	availableShadow=Gx8_GetAvailableShadowBufferNumber();

	if(availableShadow>=0)
	{
		pCurrentShadowBuffer= &shadowTextures[availableShadow];

		//To be changed...i don't like it here
		usedShadowBuffer[availableShadow]=true;

		//Save light matrix for this shadow buffer
		MATH_CopyMatrix(&shadowTextures[availableShadow].lightMatrix,pMatrix);

		//Save the pointer to light
		shadowTextures[availableShadow].p_Light=pst_Light;

		//Save the projection matrix
		fov*=1.2f;
		Gx8_CreateProjectionForSB(shadowTextures+availableShadow,fov);


		//Get available shadow buffer surface
		IDirect3DTexture8_GetSurfaceLevel(shadowTextures[availableShadow].pShadowTexture,
										  0,
										  &pShadowDepthSurface);


		//Set the shadow buffer as render targe (z-buffer)..the same color buffer as before (i don't need to write on color.)
		IDirect3DDevice8_SetRenderTarget( p_gGx8SpecificData->mp_D3DDevice,
										  pFakeColorTextureSurface,
										  pShadowDepthSurface);


		//Clear the surface (here we can use a mask to clear the shadow buffer)
		Gx8_ClearSurface();





		//Release not more use surface interfaces
		IDirect3DSurface8_Release(pShadowDepthSurface);
		pShadowDepthSurface=0;


		return 1;
	}
	else
	{
		return 0;
	}

}


void Gx8_ResetShadowBuffer(void)
{
	int counter;

	memset(usedShadowBuffer,0,sizeof(bool)*MAX_SHADOW_BUFFER_TEXTURES);


	for(counter=0;counter<MAX_SHADOW_BUFFER_TEXTURES;++counter)
	{
		memset(shadowTextures[counter].objectInShadowForLight,0,sizeof(void *)*MAX_NUM_OBJECTS_PER_LIGHT);
		shadowTextures[counter].NumObjects=0;
	}


}


//This one can be changed to give a "shape" to the light 
void Gx8_ClearSurface(void)
{

	IDirect3DDevice8_Clear( p_gGx8SpecificData->mp_D3DDevice, 
							0, 
							NULL, 
							D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
							0x00000000, 
							1.0f,
							0 );

}


void Gx8_PrepareDeviceForSBRendering(void)
{

	float fZOffset=4.0f;
	float fSlopeScale=2.0f;

	//All the shadow buffer available at the start
	Gx8_ResetShadowBuffer();

	//Save old surface (for z and color buffers)
	IDirect3DDevice8_GetRenderTarget(p_gGx8SpecificData->mp_D3DDevice, &pSaveRenderTarget );
	IDirect3DDevice8_GetDepthStencilSurface(p_gGx8SpecificData->mp_D3DDevice, &pSaveZBuffer );

	
	//Don't write in color buffer
	IDirect3DDevice8_SetRenderState( p_gGx8SpecificData->mp_D3DDevice,D3DRS_COLORWRITEENABLE, 0 );

	//Set poligon offset (for Z precision)
	IDirect3DDevice8_SetRenderState( p_gGx8SpecificData->mp_D3DDevice, D3DRS_SOLIDOFFSETENABLE, TRUE );
	IDirect3DDevice8_SetRenderState( p_gGx8SpecificData->mp_D3DDevice, D3DRS_POLYGONOFFSETZOFFSET, (*(DWORD*)&fZOffset) );
	IDirect3DDevice8_SetRenderState( p_gGx8SpecificData->mp_D3DDevice, D3DRS_POLYGONOFFSETZSLOPESCALE, (*(DWORD*)&fSlopeScale) );

}


void Gx8_ReleaseDeviceForSBRendering(void)
{

	//Restore old surfaces (and releases it)
	IDirect3DDevice8_SetRenderTarget( p_gGx8SpecificData->mp_D3DDevice,
									  pSaveRenderTarget,
									  pSaveZBuffer);
	//Release surfaces
	IDirect3DSurface8_Release(pSaveRenderTarget);
	pSaveRenderTarget=0;

	IDirect3DSurface8_Release(pSaveZBuffer);
	pSaveZBuffer=0;


	//Restore color writing 
	IDirect3DDevice8_SetRenderState( p_gGx8SpecificData->mp_D3DDevice,D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL );
	
	//No more poligon offset
	IDirect3DDevice8_SetRenderState( p_gGx8SpecificData->mp_D3DDevice, D3DRS_SOLIDOFFSETENABLE, FALSE );
}



/////////////////////////////////DIFFUSE RENDERING ////////////////////////////////////////////////////////////////////
void Gx8_PrepareDeviceForSBDiffuseRendering(void)
{

	IDirect3DSurface8		*pShadowDiffuse;
	IDirect3DSurface8		*pShadowDiffuseZ;



	IDirect3DDevice8_GetRenderTarget(p_gGx8SpecificData->mp_D3DDevice, &pSaveRenderTarget );
	IDirect3DDevice8_GetDepthStencilSurface(p_gGx8SpecificData->mp_D3DDevice, &pSaveZBuffer );


	
	IDirect3DTexture8_GetSurfaceLevel(pDiffuseShadowBuffer,
										  0,
										  &pShadowDiffuse);
		

	IDirect3DTexture8_GetSurfaceLevel(pDiffuseShadowBufferZ,
										  0,
										  &pShadowDiffuseZ);


	IDirect3DDevice8_SetRenderTarget( p_gGx8SpecificData->mp_D3DDevice,
									  pShadowDiffuse,
									  pShadowDiffuseZ);
	

	/*IDirect3DDevice8_SetRenderTarget( p_gGx8SpecificData->mp_D3DDevice,
									  pShadowDiffuse,
									  pSaveZBuffer);
	*/

	//Clean UP everthing
	IDirect3DDevice8_Clear( p_gGx8SpecificData->mp_D3DDevice, 
							0, 
							NULL, 
							D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
							0x00000000, 
							1.0f,
							0 );



	//Release no more used surfaces
	IDirect3DSurface8_Release(pShadowDiffuse);
	IDirect3DSurface8_Release(pShadowDiffuseZ);

	pShadowDiffuse=0;
	pShadowDiffuseZ=0;

}

void Gx8_ReleaseDeviceForSBDiffuseRendering(void)
{
	//Restore old surfaces (and releases it)
	IDirect3DDevice8_SetRenderTarget( p_gGx8SpecificData->mp_D3DDevice,
									  pSaveRenderTarget,
									  pSaveZBuffer);
	//Release surfaces
	IDirect3DSurface8_Release(pSaveRenderTarget);
	pSaveRenderTarget=0;

	IDirect3DSurface8_Release(pSaveZBuffer);
	pSaveZBuffer=0;

	//No more texture transformation by default
	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);

	

}


int Gx8_GetShadowBufferFromLightPointer(void *pPassedLight)
{
	int counter;
	for(counter=0;counter<MAX_SHADOW_BUFFER_TEXTURES;++counter)
	{
		if(usedShadowBuffer[counter])
		{
			if(shadowTextures[counter].p_Light==pPassedLight)
				return counter;
		}
	}

	//There is no shadow buffer available
	return -1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








/////INTERFACE///////////////////////////
int  Gx8_SBSetTarget(MATH_tdst_Matrix *pMatrix,void 	*pst_Light,float fov)
{

	if(Gx8_SetShadowBufferAsTarget(pMatrix,pst_Light,fov))
	{
		//EVERYTHING is OK
		return 1;
	}
	else
	{
		//Something doesn't work
		return 0;
	}

}


int Gx8_SetShadowBufferFormEngineLight(void *pLight)
{

	int theBuffer;
	MATH_tdst_Matrix mTmp;

	theBuffer=Gx8_GetShadowBufferFromLightPointer(pLight);

	if(theBuffer>=0)
	{
		//Save the current shadow buffer
		pCurrentShadowBuffer= &shadowTextures[theBuffer];

		//Set the shadow buffer texture in texture 0 (see...no more texture problems...shadow rendering performed on diffuse)
		IDirect3DDevice8_SetTexture( p_gGx8SpecificData->mp_D3DDevice,0,(D3DBaseTexture *) shadowTextures[theBuffer].pShadowTexture );

		//Prepare the device settings for shadow buffer rendering
		IDirect3DDevice8_SetRenderState( p_gGx8SpecificData->mp_D3DDevice, D3DRS_SHADOWFUNC, D3DCMP_GREATEREQUAL );


		//Set the light matrix in globla
		CAM_SetObjectMatrixFromCam( &mTmp, &shadowTextures[theBuffer].lightMatrix );
		MATH_InvertMatrix( &p_gGx8SpecificData->mSBTextureMatrix, &mTmp );


		//Save globally the projection matrix
		pCurrentProjection= & shadowTextures[theBuffer].lightProjection;

		
		
		
		//Clamping
		IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER );
		IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER );
		IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_BORDERCOLOR, 0xffffffff );

		//Filtering
		IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_MAGFILTER, D3DTEXF_GAUSSIANCUBIC  );
		IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_MINFILTER, D3DTEXF_GAUSSIANCUBIC  );
		
		//Texture coords generation 
		IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
		IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT4 | D3DTTFF_PROJECTED );



		return 1;
		

	}
	else
		return 0;
		

}


void Gx8_ResetShadowBufferDeviceSettings(void)
{
	IDirect3DDevice8_SetTexture( p_gGx8SpecificData->mp_D3DDevice, 0, NULL );
	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
	IDirect3DDevice8_SetRenderState( p_gGx8SpecificData->mp_D3DDevice, D3DRS_SHADOWFUNC, D3DCMP_NEVER );
}



//Prepare (and set in DX) texture coord matrix for shadow buffer texture 
void Gx8_ShadowBufferPrepareMatrix(MATH_tdst_Matrix *pObjectToCamera)
{
	
	D3DXMATRIX Projection;
	D3DXMATRIX matViewport;
	MATH_tdst_Matrix matInvObjectCamera;
	MATH_tdst_Matrix matObjectWorld;
	MATH_tdst_Matrix matCameraToWorld;
	MATH_tdst_Matrix matCameraToLight;
	MATH_tdst_Matrix matCameraToLightProjection;
	MATH_tdst_Matrix matWT;

	D3DXMatrixIdentity( &matViewport );

	matViewport._11 = SHADOW_BUFFER_WIDHT * 0.5f;
	matViewport._22 = -((SHADOW_BUFFER_HEIGHT-140) * 0.5f);
	matViewport._33 = D3DZ_MAX_D24S8;    // Maximum depth value possible for the D3DFMT_D24S8 depth buffer.
	//OLD..PROB.BUG matViewport._41 = SHADOW_BUFFER_WIDHT * 0.5f  + 0.5f;
	//OLD..PROB.BUG matViewport._42 = SHADOW_BUFFER_HEIGHT * 0.5f + 0.5f;
	matViewport._41 = SHADOW_BUFFER_WIDHT  * 0.5f;
	matViewport._42 = ((SHADOW_BUFFER_HEIGHT-140) * 0.5f) + 70.0f;
	
	

	/*matViewport._11 =  0.5f;
	matViewport._22 = -0.5f;
	matViewport._33 = D3DZ_MAX_D24S8;    // Maximum depth value possible for the D3DFMT_D24S8 depth buffer.
	matViewport._41 =1.0f;
	matViewport._42 =1.0f;
	*/


	IDirect3DDevice8_GetTransform(p_gGx8SpecificData->mp_D3DDevice,D3DTS_PROJECTION,&Projection);
	
	// build the matrix, the original coordinates are in camera space
	MATH_InvertMatrix(&matInvObjectCamera, pObjectToCamera);
	MATH_MulMatrixMatrix(&matObjectWorld, pObjectToCamera, &GDI_gpst_CurDD->st_Camera.st_Matrix);
	MATH_MulMatrixMatrix(&matCameraToWorld, &matInvObjectCamera, &matObjectWorld);
	MATH_MulMatrixMatrix(&matCameraToLight, &matCameraToWorld, &p_gGx8SpecificData->mSBTextureMatrix);
	MATH_MakeOGLMatrix(&matCameraToLight, &matCameraToLight);
	
	//OLD D3DXMatrixMultiply( (D3DMATRIX*)&matCameraToLightProjection, (D3DMATRIX*)&matCameraToLight, &Projection );
	D3DXMatrixMultiply( (D3DMATRIX*)&matCameraToLightProjection, (D3DMATRIX*)&matCameraToLight, pCurrentProjection );

	D3DXMatrixMultiply( (D3DMATRIX*)&matWT, (D3DMATRIX*)&matCameraToLightProjection, &matViewport );

	IDirect3DDevice8_SetTransform(p_gGx8SpecificData->mp_D3DDevice,
								  D3DTS_TEXTURE0,
								  (D3DMATRIX *) &matWT);

	
	

}

  


void SaveDeviceSettings(void)
{

	//Filtering
	IDirect3DDevice8_GetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_MAGFILTER, &devSettings.magFilter  );
	IDirect3DDevice8_GetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_MINFILTER, &devSettings.minFilter  );

	//UV addressing
	IDirect3DDevice8_GetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_ADDRESSU, &devSettings.uTextIndex );
	IDirect3DDevice8_GetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_ADDRESSV, &devSettings.vTexIndex );

	//The coordinates transformation 
	IDirect3DDevice8_GetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXCOORDINDEX, &devSettings.textCoordGeneration );
	IDirect3DDevice8_GetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, &devSettings.textCorrdTransform );
	IDirect3DDevice8_GetTransform(p_gGx8SpecificData->mp_D3DDevice,D3DTS_TEXTURE0,&devSettings.texMatrix);



	IDirect3DDevice8_GetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_COLOROP, &devSettings.ColoOP);
	IDirect3DDevice8_GetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_COLORARG1, &devSettings.CARG1);
	IDirect3DDevice8_GetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_COLORARG2, &devSettings.CARG2);
	IDirect3DDevice8_GetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_ALPHAOP, &devSettings.AlphaOP);
	IDirect3DDevice8_GetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_ALPHAARG1, &devSettings.AARG1);
	IDirect3DDevice8_GetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_ALPHAARG2, &devSettings.AARG2);


	//Alpha blending
	//IDirect3DDevice8_GetRenderState(p_gGx8SpecificData->mp_D3DDevice, D3DRS_CULLMODE, &devSettings.culling);
    /*IDirect3DDevice8_GetRenderState(p_gGx8SpecificData->mp_D3DDevice, D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
    */

	IDirect3DDevice8_GetRenderState(p_gGx8SpecificData->mp_D3DDevice, D3DRS_SRCBLEND, &devSettings.alphaSource);
    IDirect3DDevice8_GetRenderState(p_gGx8SpecificData->mp_D3DDevice, D3DRS_DESTBLEND,&devSettings.alphaDest);
	IDirect3DDevice8_GetRenderState(p_gGx8SpecificData->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, &devSettings.alphaBlend);


}

void RestoreDeviceSettings(void)
{
	//Filtering
	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_MAGFILTER, devSettings.magFilter  );
	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_MINFILTER, devSettings.minFilter  );

	//UV addressing
	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_ADDRESSU, devSettings.uTextIndex );
	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_ADDRESSV, devSettings.vTexIndex );

	//The coordinates transformation 
	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXCOORDINDEX, devSettings.textCoordGeneration );
	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, devSettings.textCorrdTransform );
	IDirect3DDevice8_SetTransform(p_gGx8SpecificData->mp_D3DDevice,D3DTS_TEXTURE0,&devSettings.texMatrix);

	IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_COLOROP, devSettings.ColoOP);
	IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_COLORARG1, devSettings.CARG1);
	IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_COLORARG2, devSettings.CARG2);
	IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_ALPHAOP, devSettings.AlphaOP);
	IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_ALPHAARG1, devSettings.AARG1);
	IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 0, D3DTSS_ALPHAARG2, devSettings.AARG2);

	
	IDirect3DDevice8_SetRenderState(p_gGx8SpecificData->mp_D3DDevice, D3DRS_SRCBLEND, devSettings.alphaSource);
    IDirect3DDevice8_SetRenderState(p_gGx8SpecificData->mp_D3DDevice, D3DRS_DESTBLEND,devSettings.alphaDest);
	IDirect3DDevice8_SetRenderState(p_gGx8SpecificData->mp_D3DDevice, D3DRS_ALPHABLENDENABLE, devSettings.alphaBlend);


	//IDirect3DDevice8_SetRenderState(p_gGx8SpecificData->mp_D3DDevice, D3DRS_CULLMODE, devSettings.culling);



}



//////DIFFUSE RENDERING


void Gx8_PrepareMatrixDiffuseRendering(void)
{

	D3DMATRIX projection;
	D3DMATRIX viewport;
	D3DMATRIX finalMat;
	

	p_gGx8SpecificData->diffuseInTexture=true;
	p_gGx8SpecificData->IsDiffuseActive=true;


	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT4 | D3DTTFF_PROJECTED );




	IDirect3DDevice8_GetTransform(p_gGx8SpecificData->mp_D3DDevice,
								  D3DTS_PROJECTION,
								  (D3DMATRIX *) &projection);



	D3DXMatrixIdentity( &viewport );



	viewport._11 = SHADOW_BUFFER_DIFFUSE_TEXTURE_SIZE_W * 0.5f;
	viewport._22 = -((SHADOW_BUFFER_DIFFUSE_TEXTURE_SIZE_H-140) * 0.5f);
	//viewport._33 = D3DZ_MAX_D24S8;    // Maximum depth value possible for the D3DFMT_D24S8 depth buffer.
	viewport._33 =1.0f;
	//OLD..PROB.BUG viewport._41 = SHADOW_BUFFER_DIFFUSE_TEXTURE_SIZE_W * 0.5f + 0.5f;
	//OLD..PROB.BUG viewport._42 = SHADOW_BUFFER_DIFFUSE_TEXTURE_SIZE_H * 0.5f + 0.5f;

	viewport._41 = SHADOW_BUFFER_DIFFUSE_TEXTURE_SIZE_W * 0.5f;
	viewport._42 = ((SHADOW_BUFFER_DIFFUSE_TEXTURE_SIZE_H-140) * 0.5f) + 70.0f;
	

	

/*
	viewport._11 = 0.5f;
	viewport._22 = -(0.5f);
	//viewport._33 = D3DZ_MAX_D24S8;    // Maximum depth value possible for the D3DFMT_D24S8 depth buffer.
	viewport._33 =1.0f;
	viewport._41 = 0.5f;
	viewport._42 = 0.5f;

*/
	
	D3DXMatrixMultiply( (D3DMATRIX*)&finalMat, (D3DMATRIX*)&projection, &viewport );



	IDirect3DDevice8_SetTransform(p_gGx8SpecificData->mp_D3DDevice,
								  D3DTS_TEXTURE1,
								  (D3DMATRIX *) &finalMat);


	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
	
	//Filtering
	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR  );
	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR  );

}


void Gx8_SetDiffuseComponent(DWORD blendOperator)
{
	IDirect3DDevice8_SetTexture( p_gGx8SpecificData->mp_D3DDevice,1,(D3DBaseTexture *)pDiffuseShadowBuffer);

	IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_COLOROP, blendOperator);
	IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_COLORARG2, D3DTA_TEXTURE);

	IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	
}

void Gx8_ResetDiffuseComponent(void)
{
	IDirect3DDevice8_SetTexture( p_gGx8SpecificData->mp_D3DDevice,1,NULL);
	IDirect3DDevice8_SetTextureStageState(p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
}





void Gx8_ReleaseMatrixDiffuseRendering(void)
{

	

	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
	IDirect3DDevice8_SetTextureStageState( p_gGx8SpecificData->mp_D3DDevice, 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	IDirect3DDevice8_SetTexture( p_gGx8SpecificData->mp_D3DDevice,1,NULL);

	Gx8_ResetDiffuseComponent();

	p_gGx8SpecificData->diffuseInTexture=false;
	p_gGx8SpecificData->IsDiffuseActive=false;

}

void SetCastingShadows(bool casting)
{
	castingShadow=casting;
}

bool GetCastingShadows(void)
{
	return castingShadow;
}

void Gx8_SetBias(int level)
{
	IDirect3DDevice8_SetRenderState(p_gGx8SpecificData->mp_D3DDevice,D3DRS_ZBIAS,level);
}

void Gx8_CreateProjectionForSB(ShadowBuffer *pShadow,float FOV)
{


		D3DXMATRIX projMatrix;
		float wp,hp,Q;
		float f_near,f_far;
		
		f_near = 0.05f;
		f_far = 500000.0f;


		//Set directX projection matrix
		hp=wp= 1.0f /(float)( tan(FOV/2.0f) );
		Q=f_far/(f_far-f_near);

		D3DXMatrixIdentity(&projMatrix);
		projMatrix._11=wp;
		projMatrix._22=hp;
		projMatrix._33=Q;
		projMatrix._34=1.0f;
		projMatrix._43=(-Q*f_near);

		pShadow->lightProjection=projMatrix;

		
}


void Gx8_SaveObjectInCurrentShadowBuffer(void *pGameObject)
{
	int position;
	position=pCurrentShadowBuffer->NumObjects;
	pCurrentShadowBuffer->objectInShadowForLight[position]=pGameObject;
	pCurrentShadowBuffer->NumObjects++;
}


bool Gx8_IsObjectVisibleByCurrentShadowBuffer(void *pGameObject)
{


	int counter;

	for(counter=0;counter<pCurrentShadowBuffer->NumObjects;++counter)
	{
		if(pGameObject==pCurrentShadowBuffer->objectInShadowForLight[counter])
			return true;
	}

	return false;
}

bool Gx8_IsObjectVisibleByShadowBuffer(void *pGameObject)
{
	int counter;

	ShadowBuffer * pTempShadowBuffer;
	pTempShadowBuffer=pCurrentShadowBuffer;

	for(counter=0;counter<MAX_SHADOW_BUFFER_TEXTURES;++counter)
	{
		pCurrentShadowBuffer=shadowTextures+counter;
		if(Gx8_IsObjectVisibleByCurrentShadowBuffer(pGameObject))
		{
			pCurrentShadowBuffer=pTempShadowBuffer;
			return true;
		}
	}

	pCurrentShadowBuffer=pTempShadowBuffer;
	return false;
}





bool Gx8_IsDiffuseInTexture(void)
{
	return p_gGx8SpecificData->IsDiffuseActive;
}


void Gx8_SetDiffuseInTextureRendering(bool isInDiffuse)
{
	p_gGx8SpecificData->diffuseInTexture=isInDiffuse;
}








