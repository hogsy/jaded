#ifndef __GXI_DEF_H__
#define __GXI_DEF_H__

#include <dolphin.h>

#include "GXI_GC/GXI_hwstates.h"

typedef struct {

	GXTexObj		obj;
	GXTexFmt		formatTEX;	
	GXCITexFmt		formatCI;
	
	GXTexWrapMode   wrap_s;
	GXTexWrapMode   wrap_t;
	u16				W;
	u16				H;
	BOOL			bUseMipMap;
	u32 			paletteNum;
	void*			bitmapData;
}	GXI_tdst_TextureDesc;

typedef struct {
	GXTlutObj		obj;
	void*			paletteData;
	u32				paletteName;
}	GXI_tdst_PaletteDesc;

typedef struct 
{
	f32 Depth_Blur_ZStart;
	f32 Depth_Blur_ZEnd;
	f32 Depth_Blur_Near;
	f32 Depth_Blur_Factor;
	f32 Contrast;
	f32 Brighness;
	f32 ZoomSmoothFactor;
	u32 ZSDirIsValidated;
	MATH_tdst_Vector ZSDir;
	f32 SpinSmoothFactor;
	f32 MotionBlurFactor;
	f32 RemanaceFactor;
	f32 BlurFactor;
	f32 EmbossFactor;
	f32 BWFactor;
	f32 CB_Spectre;
	f32 CB_Intensity;
	f32 CB_PhotoFilterEffect;
	f32 CB_Pastel;
	f32 fBorderBrightnessFactor;
	f32	ZoomSmoothRemanenceFactor;
	u32 BorderColor;
	MATH_tdst_Vector ZoomSmoothRemanenceLightDirection;
	u32 AA22BlurMode;
	
	f32 fWR_ZHeight;
	f32 fWR_ZDepth;
	f32 fWR_ZPlane;
	
} GXI_AfterEffectParams;


#define GC_Status_AE_BorderBrightness	0x00000001  // previously GC_Status_FogOn
#define GC_Status_Setup_On 				0x00000002
#define GC_Status_Raster_On 			0x00000004
#define GC_Status_AE_ZS_Remanence		0x00000008	// previously GC_Status_SVS
#define GC_Status_PAL_On 				0x00000010
#define GC_Status_AE_BLUR	 			0x00000020
#define GC_Status_TNumCounterOn 		0x00000040	// Unused
#define GC_Status_SetRenderStateOn 		0x00000080
#define GC_Status_AE_CB_On 				0x00000100
#define GC_Status_AE_DB 				0x00000200
#define GC_Status_AE_MB 				0x00000400
#define GC_Status_AE_SM 				0x00000800
#define GC_Status_Raster_Details_On 	0x00001000
#define GC_Status_AE_EMB 				0x00002000
#define GC_Status_AE_SMSM				0x00004000
#define GC_Status_AE_SHOWSDW			0x00008000  // Unused ?
#define GC_Status_GetZBufferBack		0x00010000	// Unused
#define GC_Status_ZBufferBackValid		0x00020000	// Unused
#define GC_Status_AE_SHOWZBUF			0x00040000	// Unused ?
#define GC_Status_AE_ZS					0x00080000
#define GC_Status_AE_REMANANCE			0x00100000
#define GC_Status_VAA_CRTS				0x00200000	// Unused
#define GC_Status_Show_Depth			0x00400000	// Unused
#define GC_Status_AE_DBN				0x00800000
#define GC_Status_AE_BlurTest44			0x01000000
#define GC_Status_AE_ModeCompare		0x02000000	// Unused ?
#define GC_Status_AE_RS					0x04000000
#define GC_Status_AE_BW					0x08000000
#define GC_Status_Raster_Prop_On 		0x10000000
#define GC_Status_AE_ShowTexture		0x20000000
#define GC_Status_Console_On 			0x40000000
#define GC_Status_Strips_On 			0x80000000	// Unused

#define USE_HARDWARE_LIGHTS
#define GC_BENCHGRAPH

#ifdef USE_HARDWARE_LIGHTS
#define MAX_HARDWARE_LIGHTS 7
#endif



#define FLOAT f32

typedef struct _tdstFogParams
{
	GXColor					FogColor;
	f32						FogStart;
	f32						FogEnd;
	f32						FogDensity;
	GXFogType				FogType;
	BOOL					FogActive;
	BOOL					FogForceDisable;	
} tdstFogParams;

#include "GXI_GC/GXI_objectlist.h"

typedef struct {

	// utility
	Mtx 					current_modelview_matrix;
	Mtx44					identity_projection_matrix;
	Mtx44					orthogonal2D_matrix;
	
	// global status
	BOOL					bRendering2D;
							
	// object list management
	BOOL					bPutObjectInObjectList;
	GXI_ObjectList			current_object_list_item;
	GXI_ObjectList			*pcurrent_object_list_item;
	u32  	   				*vtx_array_LOCAL;
	u32  	   				*vtx_array_LOCAL_NRM;

	BOOL					bSendingObjectList;
	BOOL					bFirstReadPixelThisFrame; // special case for sending objectlist in z only mode
	
	// texture handling
	u32 					ulNumberOfTextures;
	u32						ulNumberOfPalettes;
	GXI_tdst_TextureDesc 	*p_MyTextureDescriptorsAligned; // array of texture info
	GXI_tdst_PaletteDesc	*p_MyPalletteDescriptorAligned; // array of palette info

	// texture animations
	Mtx						TexMatrix;
	
	// afterFX
	GXI_AfterEffectParams   *p_AE_Params;
	float					fFocale,fScreenRatio; 	
	
	// menu variables
	f32 					Xsize;
	f32 					Ysize;
	u32 					Morfling;
	f32 					fCurrentFrquency;
	
	// color manipulations
	u32 					ulColorOr;
	
	// optimisations
	u32						*dul_TextureDeltaBlend;
	
	// setup menu
	u32						Status;
	
	// Raster Mode
	u32						RasterMode;

	// fog
	tdstFogParams			FogParams;
	
	BOOL					bUseMegaFogOveride;
	tdstFogParams			MegaFogParams;
	
	// lighting
#ifdef USE_HARDWARE_LIGHTS	
	GXLightID				LightMask;
	GXLightObj				LightObjects[MAX_HARDWARE_LIGHTS];
#endif	
#ifdef GC_BENCHGRAPH
	u32						ulDisplayTnum;
	u32						ulNumberOfTri;
	u32						ulVertxArraySize;
	u32						ulFIFOSize;
	u32						ulGEOMEM;
	u32						ulTEXMEM;
	u32						ulDisplayLST;
#endif
	
}	GXI_BigStruct ATTRIBUTE_ALIGN(32);


#endif //GXI_DEF_H
