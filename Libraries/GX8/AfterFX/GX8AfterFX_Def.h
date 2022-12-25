#ifndef _AFTERFX_DEF_H_
#define _AFTERFX_DEF_H_

#include "GX8/Gx8init.h"

#define GSP_Status_AE_BW				0x00000001
#define GSP_Status_AE_WATEREFFECT		0x00000002
#define GSP_Status_AE_CB_On 			0x00000004
#define GSP_Status_AE_DB 				0x00000008
#define GSP_Status_AE_MB 				0x00000010
#define GSP_Status_AE_SM 				0x00000020
#define GSP_Status_AE_EMB 				0x00000040
#define GSP_Status_AE_SMSM				0x00000080
#define GSP_Status_AE_SHOWSDW			0x00000100
#define GSP_Status_AE_SHOWZBUF			0x00000200
#define GSP_Status_AE_ZS				0x00000400
#define GSP_Status_AE_REMANANCE			0x00000800
#define GSP_Status_AE_DBN				0x00001000
#define GSP_Status_AE_BlurTest44		0x00002000
#define GSP_Status_AE_ModeCompare		0x00004000
#define GSP_Status_AE_RS				0x00008000
#define GSP_Status_AE_ShowTexture		0x00010000
#define GSP_Status_AE_NOAE				0x00020000
#define GSP_Status_AE_FADE				0x00040000
#define GSP_Status_AE_Border			0x00080000//Yoann Border Brightness
#define GSP_Status_AE_ZSR				0x00100000//Yoann Zoom Smooth Remanence

#define GSP_MAX_AE 32

typedef struct 
{
	float Contrast;
	float Brighness;
	float ZoomSmoothFactor;
	float SpinSmoothFactor;
	float MotionBlurFactor;
	float RemananceFactor;
	float BlurFactor;
	float Depth_Blur_ZStart;
	float Depth_Blur_ZEnd;
	float Depth_Blur_Near;
	float Depth_Blur_Factor;
	float BWFactor;
	float CB_Spectre;
	float CB_Intensity;
	float CB_PhotoFilterEffect;
	float CB_Pastel;
	float fadeTime;
	//Rajou Yoann
	float WarpFactor;
	float ZoomSmoothRemananceFactor;
	float SharpenFactor	;
	float BorderBrightNess;
	float FoggyBlur;
	float Glow;
	MATH_tdst_Vector ZoomSmoothRemananceLightDirection;
	MATH_tdst_Vector ZSDir;
	u32				ZSDirIsValidated;

	u32	  BorderColor;
	float PerfectGow_Factor;
	float PerfectGow_Thresh;
	float PerfectGow_Radius;	
/*
	float EmbossFactor;
	float fDiaphragmStrength;
	u_int AA22BlurMode;
	/* water effect */
/*	float fWR_ZHeight;
	float fWR_ZDepth;
	float fWR_ZPlane;
	float fWR_TransparencyMax;
	float fWR_SinusEffect;
	float fWR_SinusEffectBalance;
	float fWR_DarkFactor;
	int   ulWR_ZOrder;
	/* glow effect */
/*	float fG_Glowintensity;
	float fG_GlowMotionBlur;
	u_int ulG_GlowMode;*/
		
} GSP_AfterEffectParams;


void Gsp_AE(void);

void AE_ManageAETexturesAllocation(Gx8_tdst_SpecificData *_pst_SD);
void AE_InitAETexturesPool(Gx8_tdst_SpecificData *pst_SD);
void AE_ResetAETexturesPool(Gx8_tdst_SpecificData *pst_SD);

#endif