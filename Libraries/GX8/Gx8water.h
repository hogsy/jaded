////////////////////////////////////////////////////////////////////////////////////////////////////
/////X-BOX SPECIFIC CODE FOR WATER REFLECTION MAPPING //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _GX8WATER_
#define _GX8WATER_

#include "Precomp.h"

#include <D3D8.h>
#include <D3DX8.h>

#include "Gx8init.h"


//Begin Reflection draw
void Gx8_BeginReflectionDraw(void);
void Gx8_EndReflectionDraw(void);

//Create a globla reflection texture
void Gx8_CreateGlobalTexture(void);

//Draw in the global reflection texture the flipped element  
void Gx8_PrepareDrawElementReflectedInTexture(void);
void Gx8_DrawElementReflectedInTexture(int VertCount,int IndexOffset,int NumPrimitive);
void Gx8_UnprepareDrawElementReflectedInTexture(void);

//Set the reflection matrix for the obeject in the device........
void Gx8_SetReflectionMatrix(MATH_tdst_Matrix* _pst_Matrix);


//Create alpha texture
void Gx8_CreateAlphaTexture(IDirect3DTexture8 ** theTexture,int direction);
void Gx8_CreateSunAlphaTexture(IDirect3DTexture8 ** theTexture);

//Prepare texture matrpingTestureix for alpha clipping
void Gx8_SetupAlphaClippingTestureMatrix(void);

//Prepare texture coord generation system and texture stage settings
void Gx8_PrepareForAlphaClipping(IDirect3DTexture8 * theTexture);
void Gx8_ResetFromAlphaClipping(void);

//Calculate distance from object to camera.....
float Gx8_CalcDistanceFromCamera(void);


//Draw light (sun and moon)
void Gx8_DrawSkyLight(void);
void Gx8_DrawLight(MATH_tdst_Vector *p4Points);
void Gx8_DrawLightInAlphaMask(MATH_tdst_Vector *p4Points);
void Gx8_PrepareAlphaMaskTexture(void);

//Get the light's billboard
void Gx8_GetSkyLightPoints(MATH_tdst_Vector *p4Points,MATH_tdst_Vector *pLPos, MATH_tdst_Vector *pCPos);
bool Gx8_IsSky(void);

unsigned char Gx8_ComputeAlphaValue(int x,int y  , float Half);
void Gx8_ComputeAlphaSunMap(unsigned char *pDST ,int SizePo2,int stride);

//TO BE OPTMIZED!!!!!!!!!!
void Gx8_CopyReflectionTextureForSun(void);

//Texture coordinates generation for lights
void Gx8_PrepareTextureCoordForLights(void);
void Gx8_ReleaseTextureCoordForLights(void);

void Gx8_BlueImage(void);
DWORD Gx8_FloatToDW(float);

//Tell if the current actor is the main actor....
bool CheckForMainActor(void);
OBJ_tdst_GameObject * GetFather(OBJ_tdst_GameObject *p_stSon );

#endif