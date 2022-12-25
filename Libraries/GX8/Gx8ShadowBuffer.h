#ifndef _GX8_SHADOW_BUFFER_
#define _GX8_SHADOW_BUFFER_


//Standard GX8 INCLUDE
#include "Precomp.h"
#include <D3D8.h>
#include <D3DX8.h>
#include "Gx8init.h"

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/BENch/BENch.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "Gx8renderstate.h"


#define MAX_NUM_OBJECTS_PER_LIGHT 100

//Structures/////////////////////////////////////////////////////

struct LIGHT_tdst_Light;






//The shadow buffer....keep texture and light matrix
typedef struct _ShadowBuffer
{
	IDirect3DTexture8 *pShadowTexture;
	MATH_tdst_Matrix lightMatrix;
	void  *p_Light;
	D3DXMATRIX lightProjection;

	//Tell what object are in this light 
	void * objectInShadowForLight[MAX_NUM_OBJECTS_PER_LIGHT];
	int NumObjects;

}ShadowBuffer;


//Utility structure used to save device settings
typedef struct _DeviSett
{
	//Texture coord transform
	D3DMATRIX	texMatrix;
	DWORD       textCoordGeneration;
	DWORD       textCorrdTransform;

	//Wrapping
	DWORD		uTextIndex;
	DWORD       vTexIndex;

	//Filtering
	DWORD		magFilter;
	DWORD       minFilter;


	DWORD	    ColoOP;
	DWORD       CARG1;
	DWORD       CARG2;

	DWORD	    AlphaOP;
	DWORD       AARG1;
	DWORD       AARG2;


	DWORD      alphaBlend;
	DWORD      alphaSource;
	DWORD      alphaDest;

	DWORD      culling;


	
} DeviceSettings;


/////////////////////////////////////////////////////////////////

//Global device settings
DeviceSettings devSettings;




//Init the system////////////////////////////////////////////////
void Gx8_InitShadowBufferSystem(void);
/////////////////////////////////////////////////////////////////


//Low level DX resource creation functions
IDirect3DTexture8 * Gx8_CreateShadowBufferTexture(int widht,int height);
void  Gx8_CreateSBDiffuseTexture(int widht,int height,IDirect3DTexture8 **pColor,IDirect3DTexture8 **pDepth);

//Inteface with shadow buffer system 
int  Gx8_GetAvailableShadowBufferNumber(void);
void Gx8_ResetShadowBuffer(void);
int  Gx8_SetShadowBufferAsTarget(MATH_tdst_Matrix *pMatrix,void *pst_Light,float fov);
void Gx8_ClearSurface(void);
void Gx8_PrepareDeviceForSBRendering(void);
void Gx8_ReleaseDeviceForSBRendering(void);

void Gx8_CreateProjectionForSB(ShadowBuffer *pShadow,float FOV);


void Gx8_PrepareDeviceForSBDiffuseRendering(void);
void Gx8_ReleaseDeviceForSBDiffuseRendering(void);
int Gx8_GetShadowBufferFromLightPointer(void *);

//Prepare (and set in DX) texture coord matrix for shadow buffer texture 
void Gx8_ShadowBufferPrepareMatrix(MATH_tdst_Matrix *pObjectToCamera);


//Save and restore device settings
void SaveDeviceSettings(void);
void RestoreDeviceSettings(void);



//Prepare the rendering matrix for "diffuse rendering"
void Gx8_PrepareMatrixDiffuseRendering(void);
void Gx8_ReleaseMatrixDiffuseRendering(void);
void Gx8_SetDiffuseComponent(DWORD blendOperator);
void Gx8_ResetDiffuseComponent(void);
void Gx8_ResetDiffuseComponent(void);



//Interface functions
int Gx8_SBSetTarget(MATH_tdst_Matrix *pMatrix,void	*pst_Light,float fov);
int Gx8_SetShadowBufferFormEngineLight(void *pLight);
void Gx8_ResetShadowBufferDeviceSettings(void);

void Gx8_SetBias(int level);



void SetCastingShadows(bool);
bool GetCastingShadows(void);


void Gx8_SaveObjectInCurrentShadowBuffer(void *pGameObject);

//TO BE OPTIMIZED
bool Gx8_IsObjectVisibleByCurrentShadowBuffer(void *pGameObject);
bool Gx8_IsObjectVisibleByShadowBuffer(void *pGameObject);


bool Gx8_IsDiffuseInTexture(void);
void Gx8_SetDiffuseInTextureRendering(bool isInDiffuse);


#endif
