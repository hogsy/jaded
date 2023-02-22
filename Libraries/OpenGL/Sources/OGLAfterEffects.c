/*$T OGLrenderstate.c GC!1.71 03/06/00 15:23:24 */
	
/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/* Aim: OpenGL render state */
#include "Precomp.h"

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "GDInterface/GDIrasters.h"
#include "OGLinit.h"
#include "OGLtex.h"
#include "GEOmetric/GEO_STRIP.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2.h"
#include "SOFT/SOFTstruct.h"
#include "SOFT/SOFTlinear.h"

#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORrender.h"


#define MINUSSULUS 0.0015f

	extern void OGL_DrawText(int X,int Y,char *P_String,long Color,float Alpha,float SizeFactor);

static int FBTexture[10] = {0xC0DE2004,0xC0DE2004,0xC0DE2004,0xC0DE2004,0xC0DE2004,0xC0DE2004,0xC0DE2004,0xC0DE2004};

static float SKYZ = -0.999999f;


#if defined PSX2_TARGET && defined __cplusplus
    extern "C" {
#endif
void OGL_AE_Radapt();

#define OLEA_Status_FogOn				0x00000001
#define OLEA_Status_Setup_On 			0x00000002
#define OLEA_Status_Raster_On 		0x00000004
#define OLEA_Status_SVS	 			0x00000008
#define OLEA_Status_PAL_On 			0x00000010
#define OLEA_Status_AE_BLUR	 		0x00000020
#define OLEA_Status_TNumCounterOn 	0x00000040
#define OLEA_Status_SetRenderStateOn 	0x00000080
#define OLEA_Status_AE_CB_On 			0x00000100
#define OLEA_Status_AE_DB 			0x00000200
#define OLEA_Status_AE_MB 			0x00000400
#define OLEA_Status_AE_SM 			0x00000800
#define OLEA_Status_Raster_Details_On 0x00001000
#define OLEA_Status_AE_EMB 			0x00002000
#define OLEA_Status_AE_SMSM			0x00004000
#define OLEA_Status_AE_SHOWSDW		0x00008000
#define OLEA_Status_GetZBufferBack	0x00010000
#define OLEA_Status_ZBufferBackValid	0x00020000
#define OLEA_Status_AE_SHOWZBUF		0x00040000
#define OLEA_Status_AE_ZS				0x00080000
#define OLEA_Status_AE_REMANANCE		0x00100000
#define OLEA_Status_VAA_CRTS			0x00200000
#define OLEA_Status_Show_Depth		0x00400000
#define OLEA_Status_AE_DBN			0x00800000
#define OLEA_Status_AE_BlurTest44		0x01000000
#define OLEA_Status_AE_ModeCompare	0x02000000
#define OLEA_Status_AE_RS				0x04000000
#define OLEA_Status_AE_BW				0x08000000
#define OLEA_Status_Raster_Prop_On 	0x10000000
#define OLEA_Status_AE_ShowTexture	0x20000000
#define OLEA_Status_Console_On 		0x40000000
#define OLEA_Status_Strips_On 		0x80000000

typedef struct 
{
	float Depth_Blur_ZStart;
	float Depth_Blur_ZEnd;
	float Depth_Blur_Near;
	float Depth_Blur_Factor;
	float Contrast;
	float Brighness;
	float ZoomSmoothFactor;
	float SharpenFactor;
	float ZoomSmoothRemananceFactor;
	float BorderBrightNess;
	u32	  BorderColor;
	MATH_tdst_Vector ZoomSmoothRemananceLightDirection;
	MATH_tdst_Vector ZSDir;
	u32				ZSDirIsValidated;
	float SpinSmoothFactor;
	float MotionBlurFactor;
	float MotionSmoothFactor;
	float RemanaceFactor;
	float BlurFactor;
	float EmbossFactor;
	float BWFactor;
	float CB_Spectre;
	float CB_Intensity;
	float CB_PhotoFilterEffect;
	float CB_Pastel;
	float fDiaphragmStrength;
	float WarpFactor;
	float FoggyBlur;
	float Glow;
	ULONG AA22BlurMode;
	
	float fWR_ZHeight;
	float fWR_ZDepth;
	float fWR_ZPlane;


	float PerfectGow_Factor;
	float PerfectGow_Thresh;
	float PerfectGow_Radius;
	
	ULONG Status;
} OLAE_AfterEffectParams;

OLAE_AfterEffectParams OGL_AllAfterEffects;

void OGL_AE_ResetAll()
{
//	float Depth_Blur_ZStart;
//	float Depth_Blur_ZEnd;
//	float Depth_Blur_Near;
//	float Depth_Blur_Factor;
	OGL_AllAfterEffects.Contrast = 0.5f;
	OGL_AllAfterEffects.Brighness = 0.5f;
	OGL_AllAfterEffects.ZoomSmoothFactor=0;
	OGL_AllAfterEffects.SpinSmoothFactor=0;
	OGL_AllAfterEffects.MotionBlurFactor=0;
	OGL_AllAfterEffects.RemanaceFactor=0;
	OGL_AllAfterEffects.BlurFactor=0;
	OGL_AllAfterEffects.EmbossFactor=0;
	OGL_AllAfterEffects.BWFactor=0;
	OGL_AllAfterEffects.CB_Spectre=0;
	OGL_AllAfterEffects.CB_Intensity=0;
	OGL_AllAfterEffects.CB_PhotoFilterEffect=0;
	OGL_AllAfterEffects.CB_Pastel=0;
	OGL_AllAfterEffects.fDiaphragmStrength=0;
	OGL_AllAfterEffects.WarpFactor=0;
	OGL_AllAfterEffects.AA22BlurMode=0;
	
	OGL_AllAfterEffects.fWR_ZHeight=0;
	OGL_AllAfterEffects.fWR_ZDepth=0;
	OGL_AllAfterEffects.fWR_ZPlane=0;
	
	OGL_AllAfterEffects.Status = 0;
} OLAE_AfterEffectParams;

#if defined(_XENON_RENDER_PC)
float Gsp_AE_MASTER_GET_P1_OGL(ULONG AENum , ULONG P1 )
#else
float Gsp_AE_MASTER_GET_P1(ULONG AENum , ULONG P1 )
#endif
{
	switch (AENum)
	{
		case 0: // Depth Bluring
			switch (P1)
			{
				case 0: return (OGL_AllAfterEffects.Depth_Blur_ZStart);
				case 1: return (OGL_AllAfterEffects.Depth_Blur_ZEnd);
				case 2: return (OGL_AllAfterEffects.Depth_Blur_Factor);
			}
		case 1: // Motion blur
			return (OGL_AllAfterEffects.MotionBlurFactor);
		case 2: // screen Smooth
			return (OGL_AllAfterEffects.BlurFactor);
		case 3: // Emboss
			return (OGL_AllAfterEffects.EmbossFactor);
		case 4: // Motion smooth
			return 0.0f;
		case 5: // Zoom smooth
			return (OGL_AllAfterEffects.ZoomSmoothFactor);
		case 6: // Rotate smooth
			return (OGL_AllAfterEffects.SpinSmoothFactor);
		case 7: // Remanance
			return (OGL_AllAfterEffects.RemanaceFactor);
		case 8: // Brightness
			return (OGL_AllAfterEffects.Brighness);
		case 9: // Contrast
			return (OGL_AllAfterEffects.Contrast);
		case 10: // AA
			return (float)(OGL_AllAfterEffects.AA22BlurMode);
		case 11: // BW
			return (OGL_AllAfterEffects.BWFactor);
		case 12: // Color balance
			switch (P1)
			{
				case 0: return (OGL_AllAfterEffects.CB_Intensity);
				case 1: return (OGL_AllAfterEffects.CB_Spectre);
				case 2: return (OGL_AllAfterEffects.CB_PhotoFilterEffect);
			}
			return 0.0f;
		case 13: // fOG CORRECTION
			switch (P1)
			{
				case 0: return 0;//(GXI_Global_ACCESS(fFogCorrector));
				case 1: return 0;//(GXI_Global_ACCESS(fFogCorrectorFar));
			}
			return 0.0f;
			break;
		case 14: // Depth blur near
			return (OGL_AllAfterEffects.Depth_Blur_Near);
			break;
		case 16: // blur
			return (OGL_AllAfterEffects.BlurFactor);
			break;
		case 17: // Warp
			return (OGL_AllAfterEffects.WarpFactor);
			break;
	}
	return 0.0f;
}
#if defined(_XENON_RENDER_PC)
ULONG Gsp_AE_MASTER_GET_ONOFF_OGL(ULONG AENum )
#else
ULONG Gsp_AE_MASTER_GET_ONOFF(ULONG AENum )
#endif
{
	switch (AENum)
	{
		case 0: // Depth Bluring
			return (OGL_AllAfterEffects.Status & OLEA_Status_AE_DB);
		case 1: // Motion blur
			return (OGL_AllAfterEffects.Status & OLEA_Status_AE_MB);
		case 2: // screen Smooth
			return (OGL_AllAfterEffects.Status & OLEA_Status_AE_SM);
		case 3: // Emboss
			return (OGL_AllAfterEffects.Status & OLEA_Status_AE_EMB);
		case 4: // Motion smooth
			return (OGL_AllAfterEffects.Status & OLEA_Status_AE_SMSM);
		case 5: // Zoom smooth
			return (OGL_AllAfterEffects.Status & OLEA_Status_AE_ZS);
		case 6: // Rotate smooth
			return (OGL_AllAfterEffects.Status & OLEA_Status_AE_RS);
		case 7: // Remanance
			return (OGL_AllAfterEffects.Status & OLEA_Status_AE_REMANANCE);
		case 10: // AA
			return (OGL_AllAfterEffects.Status & OLEA_Status_AE_BlurTest44);
		case 11: // BW
			return (OGL_AllAfterEffects.Status & OLEA_Status_AE_BW);
		case 12: // Color balance
			return (OGL_AllAfterEffects.Status & OLEA_Status_AE_CB_On);
		case 13: // fOG CORRECTION
			break;
		case 14: // Depth Blur nrear
			return (OGL_AllAfterEffects.Status & OLEA_Status_AE_DBN);
			break;
			
	}
	return 0;
}

#if defined(_XENON_RENDER_PC)
void Gsp_AE_MASTER_OGL(ULONG AENum , ULONG OnOff , ULONG P1 , float Pf1)
#else
void Gsp_AE_MASTER(ULONG AENum , ULONG OnOff , ULONG P1 , float Pf1)
#endif
{
	switch (AENum)
	{
		case 0: // Depth Bluring
			OGL_AllAfterEffects.Status &= ~ OLEA_Status_AE_DB;
			if (OnOff) OGL_AllAfterEffects.Status |= OLEA_Status_AE_DB;
			switch (P1)
			{
				case 0: OGL_AllAfterEffects.Depth_Blur_ZStart = Pf1;	break;
				case 1: OGL_AllAfterEffects.Depth_Blur_ZEnd = Pf1;		break;
				case 2: OGL_AllAfterEffects.Depth_Blur_Factor = Pf1;	break;
			}
			break;
		case 1: // Motion blur
			OGL_AllAfterEffects.Status &= ~ OLEA_Status_AE_MB;
			if (OnOff && (Pf1 > 0.05f)) OGL_AllAfterEffects.Status |= OLEA_Status_AE_MB;
			OGL_AllAfterEffects.MotionBlurFactor = Pf1;
			break;
		case 2: // screen Smooth
			OGL_AllAfterEffects.Status &= ~ OLEA_Status_AE_SM;
			if (OnOff) OGL_AllAfterEffects.Status |= OLEA_Status_AE_SM;
			OGL_AllAfterEffects.BlurFactor = Pf1;
			break;
		case 3: // Emboss
			OGL_AllAfterEffects.Status &= ~ OLEA_Status_AE_EMB;
			if (OnOff) OGL_AllAfterEffects.Status |= OLEA_Status_AE_EMB;
			OGL_AllAfterEffects.EmbossFactor = Pf1;
			break;
		case 4: // Motion smooth
			OGL_AllAfterEffects.MotionSmoothFactor = Pf1;
			OGL_AllAfterEffects.Status &= ~ OLEA_Status_AE_SMSM;
			if (OnOff) OGL_AllAfterEffects.Status |= OLEA_Status_AE_SMSM;
			break;
		case 5: // Zoom smooth
			OGL_AllAfterEffects.Status &= ~ OLEA_Status_AE_ZS;
			switch (P1)
			{
				case 0: OGL_AllAfterEffects.ZoomSmoothFactor = Pf1;	break;
				case 1: OGL_AllAfterEffects.ZSDir.x = Pf1;OGL_AllAfterEffects.ZSDirIsValidated = 1;	break;
				case 2: OGL_AllAfterEffects.ZSDir.y = Pf1;OGL_AllAfterEffects.ZSDirIsValidated = 1;	break;
				case 3: OGL_AllAfterEffects.ZSDir.z = Pf1;OGL_AllAfterEffects.ZSDirIsValidated = 1;	break;
			}

			break;
		case 6: // Rotate smooth
			OGL_AllAfterEffects.Status &= ~ OLEA_Status_AE_RS;
			if (Pf1 > 0.01f)
			{
				if (OnOff) OGL_AllAfterEffects.Status |= OLEA_Status_AE_RS;
			}
			OGL_AllAfterEffects.SpinSmoothFactor = Pf1;
			break;
		case 7: // Remanance
			OGL_AllAfterEffects.Status &= ~ OLEA_Status_AE_REMANANCE;
			if (OnOff) OGL_AllAfterEffects.Status |= OLEA_Status_AE_REMANANCE;
			OGL_AllAfterEffects.RemanaceFactor = Pf1;
			break;
		case 8: // Brightness
			OGL_AllAfterEffects.Brighness = Pf1;
			break;
		case 9: // Contrast
			OGL_AllAfterEffects.Contrast  = Pf1;
			break;
		case 10: // AA
			OGL_AllAfterEffects.Status &= ~ OLEA_Status_AE_BlurTest44;
			if (OnOff) OGL_AllAfterEffects.Status |= OLEA_Status_AE_BlurTest44;
			OGL_AllAfterEffects.AA22BlurMode  = (unsigned int)Pf1;
			break;
		case 11: // BW
			OGL_AllAfterEffects.Status &= ~ OLEA_Status_AE_BW;
			if (OnOff) OGL_AllAfterEffects.Status |= OLEA_Status_AE_BW;
			OGL_AllAfterEffects.BWFactor  = Pf1;
			break;
		case 12: // Color balance
			switch (P1)
			{
				case 0: OGL_AllAfterEffects.CB_Intensity = Pf1;	break;
				case 1: OGL_AllAfterEffects.CB_Spectre = Pf1;		break;
				case 2: OGL_AllAfterEffects.CB_PhotoFilterEffect = Pf1;	break;
			}
			OGL_AllAfterEffects.Status &= ~ OLEA_Status_AE_CB_On;
			if (OnOff) OGL_AllAfterEffects.Status |= OLEA_Status_AE_CB_On;
			break;
		case 13: // fOG CORRECTION
			switch (P1)
			{
				case 0: break;//GXI_Global_ACCESS(fFogCorrector)		 = Pf1;	break;
				case 1: break;//GXI_Global_ACCESS(fFogCorrectorFar) 	 = Pf1;	break;
			}
			break;
		case 14: // Depth Blur nrear
			OGL_AllAfterEffects.Status &= ~ OLEA_Status_AE_DBN;
			if (OnOff) OGL_AllAfterEffects.Status |= OLEA_Status_AE_DBN;
			OGL_AllAfterEffects.Depth_Blur_Near = Pf1;
			break;
		case 15: // Fade
//			GXI_AE_FADE_PUSH(Pf1);
			break;
		case 16: // Blur
			OGL_AllAfterEffects.Status &= ~ OLEA_Status_AE_BLUR;
			if (OnOff) OGL_AllAfterEffects.Status |= OLEA_Status_AE_BLUR;
			OGL_AllAfterEffects.BlurFactor = Pf1;
			break;
		case 17: // Warp
			OGL_AllAfterEffects.WarpFactor = Pf1;
			if (!OnOff) OGL_AllAfterEffects.WarpFactor = 0.0f;
			break;
		case 18: // ZoomSmoothRemanance 
			if (!OnOff) OGL_AllAfterEffects.ZoomSmoothRemananceFactor = 0.0f;
			switch (P1)
			{
				case 0: OGL_AllAfterEffects.ZoomSmoothRemananceFactor = Pf1;	break;
				case 1: OGL_AllAfterEffects.ZoomSmoothRemananceLightDirection.x = Pf1;		break;
				case 2: OGL_AllAfterEffects.ZoomSmoothRemananceLightDirection.y = Pf1;		break;
				case 3: OGL_AllAfterEffects.ZoomSmoothRemananceLightDirection.z = Pf1;		break;
			}
			break;
		case 19: // Sharpen
			OGL_AllAfterEffects.SharpenFactor = Pf1;
			break;

		case 20: // Border brightness
			OGL_AllAfterEffects.BorderBrightNess = Pf1;
			break;

		case 21: // FoggyBlur
			OGL_AllAfterEffects.FoggyBlur = Pf1;
			break;

		case 22: // Glow
			OGL_AllAfterEffects.Glow = Pf1;
			break;

		case 23: // BorderColor 
			OGL_AllAfterEffects.BorderColor = P1 & 0xffffff;
			break;

		case 24: // PerfectGlow 
			switch (P1)
			{
				case 0: OGL_AllAfterEffects.PerfectGow_Factor = Pf1;break;
				case 1: OGL_AllAfterEffects.PerfectGow_Thresh = Pf1;break;
				case 2: OGL_AllAfterEffects.PerfectGow_Radius = Pf1;break;
			}
			break;
	}
}
static GLbitfield  OGL_AE_ATTRIB_SAVE =
(//GL_CURRENT_BIT|
 GL_POINT_BIT|
 GL_LINE_BIT|
 GL_POLYGON_BIT|
 GL_POLYGON_STIPPLE_BIT|
 //GL_PIXEL_MODE_BIT|
 GL_LIGHTING_BIT|
 GL_FOG_BIT|
 GL_DEPTH_BUFFER_BIT|
 //GL_ACCUM_BUFFER_BIT|
 GL_STENCIL_BUFFER_BIT|
 GL_VIEWPORT_BIT|
 GL_TRANSFORM_BIT|
 GL_ENABLE_BIT|
 GL_COLOR_BUFFER_BIT|
 GL_HINT_BIT|
 //GL_EVAL_BIT|
 //GL_LIST_BIT|
 //GL_TEXTURE_BIT|
 GL_SCISSOR_BIT);

void OGL_ES_PushState()
{
	OGL_CALL( glPushAttrib(OGL_AE_ATTRIB_SAVE) );
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable( GL_ALPHA_TEST );
	//glDisable( GL_SAMPLE_ALPHA_TO_COVERAGE );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );
	glDepthMask(GL_FALSE);

	OGL_CALL( glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_FALSE) );
}

void OGL_ES_PopState()
{
	OGL_CALL( glPopAttrib() );
}


static u32 BIGESTLARGOR = 512;
static u32 BIGESTHAUTOR = 512;
void OGL_AE_SetViewport(float *W , float *H)
{
	float Viewport[4];
	glGetFloatv(GL_VIEWPORT  ,  Viewport);
	*W = (Viewport[2])/(float)BIGESTLARGOR;
	*H = (Viewport[3])/(float)BIGESTHAUTOR;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, *W, 0.0, *H, -1.0, 1.0);
	
}


void OGL_AE_ValidateTexture(int *TexHan,int Mode,int ScreenSize,int ScreenSize_V)
{
	u32 Dummy;
	if((*TexHan == 0xC0DE2004) || (!glAreTexturesResident(1,(GLuint* )TexHan,(GLboolean* )&Dummy )))
	{
		int Viewporti[4];
		OGL_CALL( glGetIntegerv(GL_VIEWPORT  ,  Viewporti) );
		OGL_CALL( glGenTextures(1, (GLuint*)TexHan) );
		OGL_CALL( glBindTexture(GL_TEXTURE_2D, *TexHan) );
		OGL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP) );
		OGL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP) );

		OGL_CALL( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
		OGL_CALL( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) );
		OGL_CALL( glEnable(GL_TEXTURE_2D) );
		OGL_CALL( glCopyTexImage2D(GL_TEXTURE_2D, 0 , Mode, Viewporti[0] , Viewporti[1] , ScreenSize, ScreenSize_V, 0) );
	}
}

void OGL_AE_CopyTexture(int *TexHan)
{
	int Viewporti[4];
	int XT,YT;
	glGetIntegerv(GL_VIEWPORT  ,  Viewporti);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, *TexHan );

	XT = YT = 0;

	if (Viewporti[0] < 0) {XT = -Viewporti[0] ; Viewporti[0] = 0;};
	if (Viewporti[1] < 0) {YT = -Viewporti[1] ; Viewporti[1] = 0;};
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0 ,  XT , YT , Viewporti[0] , Viewporti[1] , Viewporti[2] , Viewporti[3] );
}
void OGL_AE_CopySubTexture(int *TexHan,int x ,int y,int sx,int sy)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, *TexHan );
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0 ,  0 , 0 , x , y , sx , sy );
}

void OGL_2dSendZ(float X,float Y,float Z)
{
	glTexCoord2f(X,Y);
	glVertex3f (X,Y,Z);
}
void OGL_2dSend(float X,float Y,float U,float V)
{
	glTexCoord2f(U,V);
	glVertex3f (X,Y,0.0f);
}
void OGL_DrawQUADZ(float W,float H,float Z,ULONG Color)
{
	glBegin(GL_POLYGON); 
	glColor4ubv((GLubyte *) &Color);
	OGL_2dSendZ(0,0,Z);
	OGL_2dSendZ(W,0,Z);
	OGL_2dSendZ(W,H,Z);
	OGL_2dSendZ(0,H,Z);
	glEnd( );
}
void OGL_DrawQUAD_FH(float W,float H,ULONG Color)
{
	glBegin(GL_POLYGON); 
	glColor4ubv((GLubyte *) &Color);
	glTexCoord2f(0,0);
	glVertex3f (W,0,0);
	glTexCoord2f(W,0);
	glVertex3f (0,0,0);
	glTexCoord2f(W,H);
	glVertex3f (0,H,0);
	glTexCoord2f(0,H);
	glVertex3f (W,H,0);
	glEnd( );
}
float CosA,SinA;
void OGL_DrawDisc(float W,float H,float Radius,u32 Color)
{
	u32 Counter;
	float X,Y,SWAP;
#define DISC_DISCREET 12
	CosA = fCos(3.1415927f * 2.0f / DISC_DISCREET);
	SinA = fSin(3.1415927f * 2.0f / DISC_DISCREET);
	Counter = DISC_DISCREET + 1;
	X = Radius;
	Y = 0.0f;
	glBegin(GL_TRIANGLE_FAN); 
	glColor4ubv((GLubyte *) &Color);
	OGL_2dSend(W,H,W,H);
	Color = 0xffffffff;
	glColor4ubv((GLubyte *) &Color);

	while (Counter--)
	{
		OGL_2dSend(W + X * 0.5f ,H + Y ,W - X * 4.0f,H - Y * 4.0f);
		SWAP = X * CosA + Y * SinA;
		Y = - X * SinA + Y * CosA;
		X =  SWAP;
	}
	glEnd( );
}

void OGL_DrawQUAD(float W,float H,ULONG Color)
{
	OGL_DrawQUADZ(W,H,0.0f,Color);
}

void OGL_2dSend_E(float X,float Y , float E )
{
	glTexCoord2f(X + E,Y + E);
	glVertex3f (X,Y,0.0);
}
void OGL_DrawQUAD_E(float W,float H,float E , ULONG Color)
{
	glBegin(GL_POLYGON); 
	glColor4ubv((GLubyte *) &Color);
	OGL_2dSend_E(0,0,E);
	OGL_2dSend_E(W,0,E);
	OGL_2dSend_E(W,H,E);
	OGL_2dSend_E(0,H,E);
	glEnd( );
}

void OGL_2dSend_WRAP(float X,float Y,float W, float H, float Factor)
{
	float XSave,YSave,Radius;
	float AX,AY;
	glTexCoord2f(X,Y);

	XSave = X ; 
	YSave = Y;
	X -= W / 2.0f;
	Y -= H / 2.0f;
	X *= 2.0f/ W;
	Y *= 2.0f/ H;
	AX = fAbs(X);
	AY = fAbs(Y);
	if (AX < AY) AX = AY;
#define SQUARE_BLENDER 0.5f
	Radius = fOptSqrt(X * X + Y * Y) * (1.0f-  SQUARE_BLENDER) + (AX) * SQUARE_BLENDER;
	if (Radius >= 1.0f)
		glVertex3f (XSave,YSave,0.0);
	else
	{
		X += (X * Radius - X) * Factor;
		Y += (Y * Radius - Y) * Factor;
		X *= W / 2.0f;
		Y *= H / 2.0f;
		X += W / 2.0f;
		Y += H / 2.0f;
		glVertex3f (X,Y,0.0);
	}
}

void OGL_DrawQUAD_BOX(float W0,float H0,float W1,float H1,float W,float H, ULONG Color , float Factor)
{
	glBegin(GL_POLYGON); 
	glColor4ubv((GLubyte *) &Color);
	OGL_2dSend_WRAP(W0,H0,W,H,Factor);
	OGL_2dSend_WRAP(W1,H0,W,H,Factor);
	OGL_2dSend_WRAP(W1,H1,W,H,Factor);
	OGL_2dSend_WRAP(W0,H1,W,H,Factor);
	glEnd( );
}

void OGL_DrawQUAD_FBOX(float W0,float H0,float W1,float H1,float WT,float HT, ULONG Color )
{
	glBegin(GL_POLYGON); 
	glColor4ubv((GLubyte *) &Color);
	glTexCoord2f(0,0);
	glVertex3f (W0,H0,0.0);
	glTexCoord2f(WT,0);
	glVertex3f (W1,H0,0.0);
	glTexCoord2f(WT,HT);
	glVertex3f (W1,H1,0.0);
	glTexCoord2f(0,HT);
	glVertex3f (W0,H1,0.0);
	glEnd( );
}



void OGL_DrawQUAD_Rotate(float W,float H,ULONG Color,float Factor)
{
	float XK,YK;
	XK = fCos(Factor);
	YK = fSin(Factor);
	W *= 0.5f;
	H *= 0.5f;
	glBegin(GL_POLYGON); 
	glColor4ubv((GLubyte *) &Color);
	glTexCoord2f(0,0);
	glVertex3f (W + (-XK * W - YK * H),H + (YK * W - XK * H),0.0f);
	glTexCoord2f(W * 2.0f,0);
	glVertex3f (W + (XK * W - YK * H),H + (-YK * W - XK * H),0.0f);
	glTexCoord2f(W * 2.0f,H * 2.0f);
	glVertex3f (W + (XK * W + YK * H),H + (-YK * W + XK * H),0.0f);
	glTexCoord2f(0,H * 2.0f);
	glVertex3f (W + (-XK * W + YK * H),H + (YK * W + XK * H),0.0f);
	glEnd( );
}
void OGL_DrawVertexZoom(float X,float Y,float CX,float CY,float Factor)
{
	X = (X - CX) * (Factor+1.0f) + CX;
	Y = (Y - CY) * (Factor+1.0f) + CY;
	glVertex3f (X,Y,0);
}
void OGL_DrawQUAD_Zoom(float W,float H,float XC,float YC,ULONG Color,float Factor)
{
	glBegin(GL_POLYGON); 
	glColor4ubv((GLubyte *) &Color);
	glTexCoord2f(0,0);
	OGL_DrawVertexZoom(0,0,XC,YC,Factor);
	glTexCoord2f(W,0);
	OGL_DrawVertexZoom(W,0,XC,YC,Factor);
	glTexCoord2f(W,H);
	OGL_DrawVertexZoom(W,H,XC,YC,Factor);
	glTexCoord2f(0,H);
	OGL_DrawVertexZoom(0,H,XC,YC,Factor);
	glEnd( );
}
void OGL_DrawQUAD_MS(float W,float H,ULONG Color,float FactorX,float FactorY)
{
	float XK,YK;
	XK = W * FactorX;
	YK = H * FactorY;	
	glBegin(GL_POLYGON); 
	glColor4ubv((GLubyte *) &Color);
	glTexCoord2f(0,0);
	glVertex3f (XK,YK,0.0);
	glTexCoord2f(W,0);
	glVertex3f (W+XK,YK,0.0);
	glTexCoord2f(W,H);
	glVertex3f (W+XK,H+YK,0.0);
	glTexCoord2f(0,H);
	glVertex3f (XK,H+YK,0.0);
	glEnd( );
}

void OGL_BlackWhite(float Factor)
{
	OGL_AE_ValidateTexture(&FBTexture[5], GL_LUMINANCE,BIGESTLARGOR,BIGESTHAUTOR);
	OGL_AE_CopyTexture(&FBTexture[5] );
	{
		int Color;
		float W,H ;
		if (Factor > 1.0f) Factor = 1.0f;
		if (Factor < 0.0f) Factor = 0.0f;
		OGL_AE_SetViewport(&W , &H);
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
		glBindTexture(GL_TEXTURE_2D, FBTexture[5]);
		Color = 0xffffff | ((int)(Factor * 255.0f) << 24);
		OGL_DrawQUAD(W,H,Color);
	}
}
void OGL_FlipH()
{
	OGL_AE_ValidateTexture(&FBTexture[0],GL_RGB ,BIGESTLARGOR,BIGESTHAUTOR);
	OGL_AE_CopyTexture(&FBTexture[0] );
	{
		int Color;
		float W,H ;
		OGL_AE_SetViewport(&W , &H);
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, FBTexture[0]);
		Color = 0xffffff;
		OGL_DrawQUAD_FH(W,H,Color);
	}
}
extern ULONG LIGHT_ul_Interpol2Colors(ULONG ulP1, ULONG ulP2, float fZClipLocalCoef);
#define COMPUTE_COLOR_BB(XXX,YYY)\
{\
	float X,Y;\
	X = XXX;\
	Y = YYY;\
	X -= W / 2.0f;\
	Y -= H / 2.0f;\
	X *= 2.0f/ W;\
	Y *= 2.0f/ H;\
	Y *= W/ H;\
	X = 1.0f - (X * X + Y * Y) * Factor;\
	if (X < 0) X = 0;\
	if (X > 1) X = 1;\
	Color = OGL_AllAfterEffects.BorderColor | ((u32)(X * 255.0f)<<24);\
	glColor4ubv((GLubyte *) &Color);\
	OGL_2dSendZ(XXX,YYY,0);\
}

void OGL_BorderBrightness(float Factor)
{
	int Color;
	float W,H ;
	float Wi, Hi;
	float Wb, Hb;
	int CounterX,CounterY;

	OGL_AE_SetViewport(&W,&H);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA ,GL_SRC_ALPHA );
#define BB_Divider 16
	Wi = W / (float)BB_Divider ;
	Hi = H / (float)BB_Divider ;
	Wb = 0.0f;
	CounterX = BB_Divider ;
	while(CounterX--)
	{
		CounterY = BB_Divider ;
		Hb = 0.0f;
		while(CounterY--)
		{
			glBegin(GL_POLYGON); 
			COMPUTE_COLOR_BB(Wb,Hb);
			COMPUTE_COLOR_BB(Wb+Wi,Hb);
			COMPUTE_COLOR_BB(Wb+Wi,Hb+Hi);
			COMPUTE_COLOR_BB(Wb,Hb+Hi);
			glEnd(); 

			Hb += Hi ;
		}
		Wb += Wi ;
	}
}


void OGL_WRAP(float Factor)
{
	OGL_AE_ValidateTexture(&FBTexture[0],GL_RGB ,BIGESTLARGOR,BIGESTHAUTOR);
	OGL_AE_CopyTexture(&FBTexture[0]);

	{
		int Color;
		float W,H ;

		OGL_AE_SetViewport(&W,&H);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, FBTexture[0]);
		

		glDisable(GL_BLEND);
		Color = 0xffffff;
		{
			float Wi, Hi;
			float Wb, Hb;
			float CounterX,CounterY;
#define Divider 16
			Wi = W / (float)Divider;
			Hi = H / (float)Divider;
			Wb = 0.0f;
			CounterX = Divider;
			while(CounterX--)
			{
				CounterY = Divider;
				Hb = 0.0f;
				while(CounterY--)
				{
					OGL_DrawQUAD_BOX(Wb,Hb,Wb + Wi,Hb+ Hi,W,H,Color,Factor);
					Hb += Hi ;
				}
				Wb += Wi ;
			}
		}
	}
}

void OGL_DigitalZoom(float Factor)
{
	int Color;
	float W,H,XCenter,YCenter;
	OGL_AE_ValidateTexture(&FBTexture[0], GL_RGB,BIGESTLARGOR,BIGESTHAUTOR);
	OGL_AE_CopyTexture(&FBTexture[0]);
	OGL_AE_SetViewport(&W , &H);
	XCenter = W * .5f;
	YCenter = H * .5f;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, FBTexture[0]);
	glDisable(GL_BLEND);
	Color = 0x00ffffff;
	OGL_DrawQUAD_Zoom(W,H,XCenter,YCenter,Color,Factor);
}

void OGL_ZoomSmooth(float Factor,float XCenter , float YCenter , int RotMode)
{
	int Viewporti[4];
	float W,H,ConvoleRevolution ;
	OGL_AE_ValidateTexture(&FBTexture[0], GL_RGB,BIGESTLARGOR,BIGESTHAUTOR);
	glGetIntegerv(GL_VIEWPORT  ,  Viewporti);
	OGL_AE_SetViewport(&W , &H);
	ConvoleRevolution = 0.666666f;
	XCenter += 1.f;
	YCenter += 1.f;
	XCenter *= W * .5f;
	YCenter *= H * .5f;

	while (fAbs(Factor) > MINUSSULUS)
	{
		int Color;
		OGL_AE_CopyTexture(&FBTexture[0]);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, FBTexture[0]);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
		if (RotMode == 2)
		{
			Color = 0x00ffffff;
			OGL_DrawQUAD_Zoom(W,H,XCenter,YCenter,Color,0.0f);
			Color = 0x00ffffff;
			Color |= ((int)(ConvoleRevolution * 255.0f) << 24);
			OGL_DrawQUAD_Zoom(W,H,XCenter,YCenter,Color,Factor * 0.1f);
			ConvoleRevolution = (ConvoleRevolution - 0.5f) * 0.5f + 0.5f;
		} else
		if (RotMode == 1)
		{
			Color = 0x00ffffff;
			OGL_DrawQUAD_Rotate(W,H,Color,-Factor);
			Color = 0x80ffffff;
			OGL_DrawQUAD_Rotate(W,H,Color,Factor );
		} else
		{
			Color = 0x00ffffff;
			OGL_DrawQUAD_Zoom(W,H,XCenter,YCenter,Color,-Factor * 0.1f);
			Color = 0x80ffffff;
			OGL_DrawQUAD_Zoom(W,H,XCenter,YCenter,Color,Factor * 0.1f);
		} 

		Factor *= 0.5f;
	}
}

void OGL_MotionSmooth(float FactorX,float FactorY)
{
	int Viewporti[4];
	float W,H ;
	OGL_AE_ValidateTexture(&FBTexture[0], GL_RGB, BIGESTLARGOR, BIGESTHAUTOR );
	glGetIntegerv(GL_VIEWPORT  ,  Viewporti);
	OGL_AE_SetViewport(&W , &H);
	while (FactorX * FactorX + FactorY * FactorY > MINUSSULUS*MINUSSULUS)
	{
		int Color;
		OGL_AE_CopyTexture(&FBTexture[0]);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, FBTexture[0]);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
		Color = 0x00ffffff;
		
		(W,H,Color,-FactorX,-FactorY);
		Color = 0x80ffffff;
		OGL_DrawQUAD_MS(W,H,Color,FactorX,FactorY);

		FactorX*= 0.5f;
		FactorY*= 0.5f;
	}
}
void OGL_BigBlur(float Factor,int HQ)
{
	int Viewporti[4];
	float W,H,ConvoleRevolution ;
	OGL_AE_ValidateTexture(&FBTexture[0], GL_RGB, BIGESTLARGOR, BIGESTHAUTOR );
	glGetIntegerv(GL_VIEWPORT  ,  Viewporti);
	OGL_AE_SetViewport(&W , &H);
	ConvoleRevolution = 0.75f;
	while (	Factor > MINUSSULUS)
	{
		int Color;
		float FactorL;
		OGL_AE_CopyTexture(&FBTexture[0]);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, FBTexture[0]);
		glEnable(GL_BLEND);
		glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
		FactorL = 1.0f;

		Color = 0x00ffffff;
		Color |= ((int)((1.0f / FactorL) * 255.0f) << 24);
		FactorL += 1.0f;
		OGL_DrawQUAD_MS(W,H,Color,-Factor,0);
		Color = 0x00ffffff;
		Color |= ((int)((1.0f / FactorL) * 255.0f) << 24);
		FactorL += 1.0f;
		OGL_DrawQUAD_MS(W,H,Color,0,Factor);
		Color = 0x00ffffff;
		Color |= ((int)((1.0f / FactorL) * 255.0f) << 24);
		FactorL += 1.0f;
		OGL_DrawQUAD_MS(W,H,Color,Factor,0);
		Color = 0x00ffffff;
		Color |= ((int)((1.0f / FactorL) * 255.0f) << 24);
		FactorL += 1.0f;
		OGL_DrawQUAD_MS(W,H,Color,0,-Factor);

/*		Color = 0x00ffffff;
		Color |= ((int)((1.0f / FactorL) * 255.0f) << 24);
		FactorL += 1.0f;
		OGL_DrawQUAD_MS(W,H,Color,-Factor,Factor);
		Color = 0x00ffffff;
		Color |= ((int)((1.0f / FactorL) * 255.0f) << 24);
		FactorL += 1.0f;
		OGL_DrawQUAD_MS(W,H,Color,-Factor,-Factor);
		Color = 0x00ffffff;
		Color |= ((int)((1.0f / FactorL) * 255.0f) << 24);
		FactorL += 1.0f;
		OGL_DrawQUAD_MS(W,H,Color,Factor,-Factor);
		Color = 0x00ffffff;
		Color |= ((int)((1.0f / FactorL) * 255.0f) << 24);
		FactorL += 1.0f;
		OGL_DrawQUAD_MS(W,H,Color,Factor,Factor);//*/
		if (HQ)
		{
			Color = 0x00ffffff;
			Color |= ((int)(ConvoleRevolution * 255.0f) << 24);
			OGL_DrawQUAD_MS(W,H,Color,0,0);
			ConvoleRevolution = (ConvoleRevolution - 0.5f) * 0.5f + 0.5f;
		}
		Factor*= 0.5f;
	}
}

float OGL_GetPhase(float fColorSpctr)
{
	if (fColorSpctr > 1.0f) fColorSpctr-= 1.0f;
	if (fColorSpctr > 0.5f) fColorSpctr = 1.0f - fColorSpctr;
	if (fColorSpctr < 0.1666666f) return 1.0f;
	if (fColorSpctr > 0.3333333f) return 0.0f;
	fColorSpctr = fColorSpctr - 0.166666666f;
	fColorSpctr *= 6.0f;
	return 1.0f - fColorSpctr;
}

void OGL_AE_ColorBalance(float CB_I , float CB_S)
{
	// ColorBalance 
	if (CB_I != 0.0f)
	{
		float W,H ;
		float Red,Green,Blue;
		unsigned char Color[4];
		ULONG ColorUL;
		//u32 OverTwo;
		Red   = CB_I * (1.0f - OGL_GetPhase(CB_S)) + (1.0f - CB_I);
		Green = CB_I * (1.0f - OGL_GetPhase(CB_S - 0.3333333f + 1.0f )) + (1.0f - CB_I);
		Blue  = CB_I * (1.0f - OGL_GetPhase(CB_S - 0.6666666f + 1.0f)) + (1.0f - CB_I);
		Color[0] = (unsigned char)(Red*255.0f);
		Color[1] = (unsigned char)(Green*255.0f);
		Color[2] = (unsigned char)(Blue*255.0f);
		ColorUL = Color[0] | (Color[1]<<8) | (Color[2]<<16) | (0x80<<24);
		glDisable(GL_TEXTURE_2D);
		OGL_AE_SetViewport(&W , &H);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		OGL_DrawQUAD(W,H,ColorUL);
	}

}
void OGL_ShowAlphaBuffer()
{
	float W,H ;
//	int Color;
	OGL_BlackWhite(1.0f);//*/

	OGL_AE_SetViewport(&W , &H);
/*	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	OGL_DrawQUADZ(W,H,SKYZ,0xff000000);
	glDisable(GL_DEPTH_TEST);*/
	glDisable(GL_TEXTURE_2D);
	glColorMask(GL_TRUE , GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_ALPHA , GL_ONE_MINUS_DST_ALPHA );
	OGL_DrawQUAD(W,H,0xff);//*/

	glColorMask(GL_TRUE , GL_TRUE , GL_TRUE , GL_FALSE);
}




#define HistogrammSizeX 256
#define HistogrammSizeY 128
void OGL_ShowZBuffer()
{


	float W,H;
	static float fdAdd = -0.001f; 
	static u32 Bloc = 1;

	
	glPixelStorei(GL_UNPACK_SWAP_BYTES , TRUE);


	glPixelZoom(0.5f,  0.5f);

	OGL_AE_SetViewport(&W , &H);
	OGL_AE_ValidateTexture(&FBTexture[5], GL_DEPTH_COMPONENT,BIGESTLARGOR,BIGESTHAUTOR);
	OGL_AE_CopyTexture(&FBTexture[5] );
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, FBTexture[5]);
	glDisable(GL_BLEND);
	glDisable(GL_BLEND);
	OGL_DrawQUAD(W,H,0x80808080);
	glPixelStorei(GL_UNPACK_SWAP_BYTES , FALSE);
}//*/
#ifdef ACTIVE_EDITORS
u8 HistogrammMap[HistogrammSizeX*HistogrammSizeY*4];
void OGL_ShowHistogramm()
{
	u32 i_SaveBuffer;
	u32 i_SaveBuffer2;
	float Viewport[4];
	u32 HistogrammeGraphe[256*4],Counter,MaxValue,AverageValue,BigSumm;
	u32 HistogrammeGraphe2[256*4];
	glGetFloatv(GL_VIEWPORT  ,  Viewport);

	glGetIntegerv(GL_READ_BUFFER, (GLint* )&i_SaveBuffer);
	glReadBuffer(GL_BACK);
	glGetIntegerv(GL_DRAW_BUFFER, (GLint* )&i_SaveBuffer2);
	glDrawBuffer(GL_BACK);

	OGL_AE_ValidateTexture(&FBTexture[7],GL_RGB ,BIGESTLARGOR,BIGESTHAUTOR);
	OGL_AE_CopyTexture(&FBTexture[7] );

	
	glPixelTransferi(GL_MAP_COLOR , GL_FALSE);

	glPixelTransferf(GL_RED_BIAS, 0);
	glPixelTransferf(GL_GREEN_BIAS, 0);
	glPixelTransferf(GL_BLUE_BIAS, 0);
	glPixelTransferf(GL_RED_SCALE, 1);
	glPixelTransferf(GL_GREEN_SCALE, 1);
	glPixelTransferf(GL_BLUE_SCALE, 1);

	{
		int Color;
		float W,H ,CoefNorm;
		u8 *pColor,*pColorLast;
		OGL_AE_SetViewport(&W , &H);
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, FBTexture[7]);
		Color = 0xFFffffff;
		OGL_DrawQUAD_FBOX(0,0,HistogrammSizeX / (float)BIGESTLARGOR,HistogrammSizeY / (float)BIGESTHAUTOR,W,H, Color );
		glDisable(GL_TEXTURE_2D);
#define HIST_CompNum 3
 
		glRasterPos2f(0,0); 
		glPixelZoom(1.f,  1.f);
	    glReadPixels(0,0,HistogrammSizeX , HistogrammSizeY , GL_RGB , GL_UNSIGNED_BYTE,HistogrammMap);

		memset(HistogrammeGraphe , 0 , 256*4*4);

		pColor = HistogrammMap;
		pColorLast = pColor + HistogrammSizeX * HistogrammSizeY * HIST_CompNum;
		AverageValue = 0;
		BigSumm=0;
		while (pColor < pColorLast) 
		{
			AverageValue += *(pColor+0);
			AverageValue += *(pColor+1);
			AverageValue += *(pColor+2);
			if (*(pColor+0)) BigSumm++;
			if (*(pColor+1)) BigSumm++;
			if (*(pColor+2)) BigSumm++;
			
			HistogrammeGraphe[((u32)*(pColor++)) * HIST_CompNum + 0] ++;
			HistogrammeGraphe[((u32)*(pColor++)) * HIST_CompNum + 1] ++;
			HistogrammeGraphe[((u32)*(pColor++)) * HIST_CompNum + 2] ++;
		}
		MaxValue = 0;
		if (BigSumm)
		AverageValue /= BigSumm;

		HistogrammeGraphe[0] = 0;
		HistogrammeGraphe[1] = 0;
		HistogrammeGraphe[2] = 0;

		Counter = 256*HIST_CompNum - 3;
		while (Counter --)
		{
			HistogrammeGraphe2[Counter] = (HistogrammeGraphe[Counter] + HistogrammeGraphe[Counter+3])>>1;
		}
		Counter = 256*HIST_CompNum - 3;
		while (Counter --)
		{
			HistogrammeGraphe[Counter] = HistogrammeGraphe2[Counter];
		}
		Counter = 256*HIST_CompNum;
		while (Counter --)
		{
			MaxValue += HistogrammeGraphe[Counter];
		}
		MaxValue >>= 4;
		MaxValue /= 3;
		memset(HistogrammMap , 200, HistogrammSizeX*HistogrammSizeY*3);
		CoefNorm = (float)HistogrammSizeY / (float)MaxValue;
		Counter = 256 * HIST_CompNum;
		while (Counter --)
		{
			HistogrammeGraphe[Counter ] = (u32)(CoefNorm * (float)HistogrammeGraphe[Counter ]);
			if (HistogrammeGraphe[Counter ] > HistogrammSizeY-1) HistogrammeGraphe[Counter ] = HistogrammSizeY-1;
			HistogrammMap[Counter + HistogrammeGraphe[Counter ] * HistogrammSizeX * HIST_CompNum] = 0;
		}
		
		pColor = HistogrammMap;
		pColorLast = pColor + HistogrammSizeX * HistogrammSizeY * HIST_CompNum;
		pColor += HistogrammSizeX*HIST_CompNum;
		while (pColor < pColorLast) *(pColor++) &= *(pColor-HistogrammSizeX*HIST_CompNum);

		pColor = HistogrammMap;
		pColorLast = pColor + HistogrammSizeX * HistogrammSizeY * HIST_CompNum;
		while (pColor < pColorLast) 
		{
			*(AverageValue * 3 + pColor) = 0xff;
			*(AverageValue * 3 + pColor + 1) = 0xff;
			*(AverageValue * 3 + pColor + 2) = 0xff;

			*(64*3 + pColor) =	0x7f;
			*(64*3 + pColor + 1) = 0x7f;
			*(64*3 + pColor + 2) = 0;

			*(128*3 + pColor) =		0xff;
			*(128*3 + pColor + 1) = 0xff;
			*(128*3 + pColor + 2) = 0;

			*(192*3 + pColor) = 0x7f;
			*(192*3 + pColor + 1) = 0x7f;
			*(192*3 + pColor + 2) = 0;

			pColor += HIST_CompNum * HistogrammSizeX;
		}

/*		memset(HistogrammMap , 0 , 256*128*4);
		memset(HistogrammMap , 0xff , 256*128*1);*/

		//*/
		glDrawPixels(HistogrammSizeX , HistogrammSizeY , GL_RGB , GL_UNSIGNED_BYTE ,HistogrammMap);

	}
	glReadBuffer(i_SaveBuffer);
	glDrawBuffer(i_SaveBuffer2);
	{
		char string[1024];
		AverageValue = (u32)(((float)AverageValue / 255.0f) * 100.0f);
		sprintf(string,"Average luminosity = %d%%", AverageValue  );
		if (AverageValue < 15)
			OGL_DrawText(10,(int)(Viewport[3] - HistogrammSizeY - 20),string,0x4040ff,1.0f,1.0f);
		else
			OGL_DrawText(10,(int)(Viewport[3] - HistogrammSizeY - 20),string,0x808080,1.0f,1.0f);
	}
}//*/
#endif

void OGL_AE_BrightnessContrast(float Contrast , float Brighness )
{
	float SavedBr;
	// Brightness
	SavedBr = Brighness ;
	if (Brighness != 0.5f)
	{
		float W,H ;
		ULONG Color,Invert;
		Brighness -= 0.5f;
		Brighness *= 2.0f;
		Invert = 0;
		if (Brighness < 0.0f) 
		{
			Brighness = -Brighness ;
			Invert = 1;
		}
		if (Brighness > 1.0f) Brighness = 1.0f;
		glDisable(GL_TEXTURE_2D);
		OGL_AE_SetViewport(&W , &H);
		glEnable(GL_BLEND);
		if (Invert)
		{
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
			Color = 0xffffff;
			OGL_DrawQUAD(W,H,Color);
		}
		glBlendFunc(GL_ONE, GL_ONE);
		Color = (ULONG)(Brighness * 255.0f);
		Color |= Color << 8;
		Color |= Color << 16;
		OGL_DrawQUAD(W,H,Color);
		if (Invert)
		{
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
			Color = 0xffffff;
			OGL_DrawQUAD(W,H,Color);
		}
	}
	
	// Contrast 
	if (Contrast != 0.5f)
	{
		float W,H ;
		ULONG Color;

		Contrast -= 0.5f;
		Contrast *= 2.0f;
		if (Contrast > 0.0f)
		{
			while (Contrast > 0.0f)
			{
				Color = (ULONG)(Contrast * 255.0f);
				if (Color > 255) Color = 255;
				Color |= Color << 8;
				Color |= Color << 16;
				glDisable(GL_TEXTURE_2D);
				OGL_AE_SetViewport(&W , &H);
				glEnable(GL_BLEND);
				glBlendFunc(GL_DST_COLOR, GL_ONE);
				OGL_DrawQUAD(W,H,Color);
				Contrast -= 1.0f;
				if (Contrast > 2.0f) Contrast = 2.0f;
			}
		} else
		{
			Contrast = -Contrast ;
			if (Contrast > 1.0f) Contrast = 1.0f;
			SavedBr *= 255.0f;
			if (SavedBr < 0.0f) SavedBr = 0.0f;
			if (SavedBr > 255.0f) SavedBr = 255.0f;
			Color = (int)SavedBr;
			Color |= (Color<<8) | (Color<<16);
			Color |= (ULONG)(Contrast * 255.0f)<<24;
			glDisable(GL_TEXTURE_2D);
			OGL_AE_SetViewport(&W , &H);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			OGL_DrawQUAD(W,H,Color);
		}
	}
}
void OGL_MotionSmoothAuto(float Factor)
{
	float XX,YY;
	static MATH_tdst_Vector		MotionSmoothPoint; 
	float fCurrentFocale = 1.0f / fNormalTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2);		
	MATH_TransformVector(&MotionSmoothPoint, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , &MotionSmoothPoint);
	MotionSmoothPoint . x = ((fCurrentFocale) * MotionSmoothPoint . x) / MotionSmoothPoint . z;
	MotionSmoothPoint . y = ((fCurrentFocale) * MotionSmoothPoint . y) / MotionSmoothPoint . z;
	XX = (0.125f * GDI_gpst_CurDD->st_Device.l_Width * MotionSmoothPoint . x);
	YY = -(0.25f * GDI_gpst_CurDD->st_Device.l_Height * MotionSmoothPoint . y);
	Factor *= 0.01f;
	OGL_MotionSmooth(XX*Factor , YY * Factor);	
	MotionSmoothPoint.x = GDI_gpst_CurDD->st_Camera.st_InverseMatrix.Iz;
	MotionSmoothPoint.y = GDI_gpst_CurDD->st_Camera.st_InverseMatrix.Jz;
	MotionSmoothPoint.z = GDI_gpst_CurDD->st_Camera.st_InverseMatrix.Kz;
}
int LastMotionBlur = 0;
void OGL_MotionBlur(float Factor)
{
	int Color;
	float W,H ;
	OGL_AE_ValidateTexture(&FBTexture[7],GL_RGB ,BIGESTLARGOR,BIGESTHAUTOR);
	if (LastMotionBlur == 0)
	{
		OGL_AE_SetViewport(&W , &H);
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
		glBindTexture(GL_TEXTURE_2D, FBTexture[7]);
		Color = 0xffffff | ((int)(Factor * 255.0f) << 24);
		OGL_DrawQUAD(W,H,Color);
	}
	OGL_AE_CopyTexture(&FBTexture[7]);
	LastMotionBlur = 0;
}

#define MAX_SPALSH 64

SOFT_tdst_AVertex	FuckingHell[MAX_SPALSH];
u32					SplashNumber = 0;

void AE_Splash_AddOne(MATH_tdst_Vector *p2DPosPlusSize , u32 Color )
{
	if (SplashNumber == MAX_SPALSH) return;
	*(MATH_tdst_Vector *)&FuckingHell[SplashNumber] = *p2DPosPlusSize ;
	FuckingHell[SplashNumber++].c = Color;
}

void OGL_DrawSpah()
{
	float W,H;
	OGL_AE_ValidateTexture(&FBTexture[0],GL_RGB ,BIGESTLARGOR,BIGESTHAUTOR);
	OGL_AE_CopyTexture(&FBTexture[0]);
	OGL_AE_SetViewport(&W , &H);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	//glBlendFunc(GL_ONE_MINUS_SRC_ALPHA , GL_SRC_ALPHA );
	glBindTexture(GL_TEXTURE_2D, FBTexture[0]);
	while (SplashNumber  --)
	{
		OGL_DrawDisc(W * FuckingHell[SplashNumber] . x,H * (1.0f - FuckingHell[SplashNumber] . y),FuckingHell[SplashNumber] . z , FuckingHell[SplashNumber] . c);
	}
	// MECA
	SplashNumber = 0;
	
}

void OGL_ZooSmoothAndCenter(float Factor,MATH_tdst_Vector *ZSDir)
{
	float W,H,X,Y,fCurrentFocale ;
	MATH_tdst_Vector		MotionSmoothPoint; 

	OGL_AE_SetViewport(&W , &H);
	MATH_NormalizeVector(ZSDir,ZSDir);
	fCurrentFocale = 1.0f / fNormalTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2);		
	MATH_TransformVector(&MotionSmoothPoint, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , ZSDir);
	MotionSmoothPoint . x = ((fCurrentFocale) * MotionSmoothPoint . x) / MotionSmoothPoint . z;
	MotionSmoothPoint . y = ((fCurrentFocale) * MotionSmoothPoint . y) / MotionSmoothPoint . z;
	X = (MotionSmoothPoint . x);
	Y = -(MotionSmoothPoint . y);

	Factor *= MotionSmoothPoint . z * MotionSmoothPoint . z;

	if (MotionSmoothPoint . z > 0.f)
		OGL_ZoomSmooth(Factor,X,Y,2);
}


void OGL_ZooSmoothRemanance(float Factor,MATH_tdst_Vector *LigthDir)
{
	float W,H,X,Y,fCurrentFocale ;
	int Color;
	MATH_tdst_Vector		MotionSmoothPoint; 

	OGL_AE_SetViewport(&W , &H);

	fCurrentFocale = 1.0f / fNormalTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2);		
	MATH_TransformVector(&MotionSmoothPoint, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , LigthDir);
	MotionSmoothPoint . x = ((fCurrentFocale) * MotionSmoothPoint . x) / MotionSmoothPoint . z;
	MotionSmoothPoint . y = ((fCurrentFocale) * MotionSmoothPoint . y) / MotionSmoothPoint . z;
	X = (MotionSmoothPoint . x);
	Y = -(MotionSmoothPoint . y);
/*	Factor *= -MotionSmoothPoint . z;
	if (Factor < 0) return;
*/
	OGL_AE_ValidateTexture(&FBTexture[6], GL_RGB,BIGESTLARGOR,BIGESTHAUTOR);
	OGL_AE_CopyTexture(&FBTexture[6] );

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GEQUAL);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	OGL_DrawQUADZ(W,H,SKYZ,0);
	glDisable(GL_DEPTH_TEST);

	{
		u32 BorderColor;
		BorderColor = OGL_AllAfterEffects.BorderColor;
		OGL_AllAfterEffects.BorderColor = 0;
		OGL_BorderBrightness(1.0f);
		OGL_AllAfterEffects.BorderColor = BorderColor ;
	}

	if (MotionSmoothPoint . z < 0.f)
		OGL_ZoomSmooth(4.0f,X,Y,2);
	else
		OGL_ZoomSmooth(-4.0f,X,Y,2);

	Color = (ULONG)(Factor * 255.0f);
	Color |= Color << 8;
	Color |= Color << 16;
	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_DST_COLOR, GL_ONE);
	OGL_DrawQUAD(W,H,Color);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_ONE, GL_SRC_ALPHA );
	glBindTexture(GL_TEXTURE_2D, FBTexture[6]);
	Color = 0xffffff | ((int)(Factor * 255.0f) << 24);
	OGL_DrawQUAD(W,H,Color);

}







void OGL_PerfectGlow(float Factor,float Thresh,float radius)
{
	int invert;
	float W,H,FactorSave ;
	int Color;
	FactorSave = Factor;
	invert = 0;
	if (Factor == 0.0f) return;
	OGL_AE_SetViewport(&W , &H);
	OGL_AE_ValidateTexture(&FBTexture[6], GL_RGB,BIGESTLARGOR,BIGESTHAUTOR);
	/* 1 Save */
	OGL_AE_CopyTexture(&FBTexture[6] );
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	/* 2 BW */
	OGL_BlackWhite(1.0f);
	/* 3 Sub */
    /* 3a Invert dest */
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR , GL_ZERO );
	Color = 0xffffff;
	OGL_DrawQUAD(W,H,Color);
    /* 3b Add dest */
	glBlendFunc(GL_ONE , GL_ONE );
	Color = (u32)(Thresh * 255.0f);
	Color |= (Color << 8) | (Color << 16);
	Color ^= 0xffffff ;
	OGL_DrawQUAD(W,H,Color);
    /* 3c Invert dest */
	glBlendFunc(GL_ONE_MINUS_DST_COLOR , GL_ZERO );
	Color = 0xffffff;
	OGL_DrawQUAD(W,H,Color);

		

	/* 4 Blur */
	OGL_BigBlur(radius * 0.02f ,0);
	/* 5 Add saved 2x */

	OGL_AE_ValidateTexture(&FBTexture[7], GL_RGB,BIGESTLARGOR,BIGESTHAUTOR);
	OGL_AE_CopyTexture(&FBTexture[7] );

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, FBTexture[6]);
	glBlendFunc(GL_ONE,GL_SRC_ALPHA );

	Color = (u32)(Factor * 255.f) << 24;
	Color |= 0xffffff;
	OGL_DrawQUAD(W,H,Color);

	glBindTexture(GL_TEXTURE_2D, FBTexture[7]);
	glBlendFunc(GL_SRC_ALPHA , GL_ONE);
	OGL_DrawQUAD(W,H,Color);
	OGL_DrawQUAD(W,H,Color);
	OGL_DrawQUAD(W,H,Color);

}










void OGL_Remanance(float Factor)
{
	int invert;
	float W,H ;
	int Color;
	invert = 0;
	OGL_AE_SetViewport(&W , &H);
	if (Factor < 0.0f)
	{
		invert = 1;
		Factor = -Factor ;
	}
	if (Factor > 1.0f) Factor = 1.0f;
	if (invert)
	{
		glEnable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
		Color = 0xffffff;
		OGL_DrawQUAD(W,H,Color);
	} 
	OGL_AE_ValidateTexture(&FBTexture[6], GL_RGB,BIGESTLARGOR,BIGESTHAUTOR);
	OGL_AE_CopyTexture(&FBTexture[6] );

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	OGL_DrawQUADZ(W,H,SKYZ,0xFF000000);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);

	if (!invert)
	{

		glEnable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_ZERO, GL_DST_ALPHA);
		Color = 0;
		OGL_DrawQUAD(W,H,Color);//*/
	} 
//*/
	if (invert)
	{
		glEnable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
		Color = 0xffffff;
		OGL_DrawQUAD(W,H,Color);
		OGL_BigBlur(0.02f,0);

	} else
	{
		OGL_BigBlur(0.02f,0);
/*		glEnable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_ZERO, GL_ONE_MINUS_DST_ALPHA);
		Color = 0;
		OGL_DrawQUAD(W,H,Color);//*/
	}
	{
		Color = (ULONG)(Factor * 255.0f);
		Color |= Color << 8;
		Color |= Color << 16;
		glEnable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_DST_COLOR, GL_ONE);
		OGL_DrawQUAD(W,H,Color);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, FBTexture[6]);

/*		glBlendFunc(GL_DST_ALPHA, GL_ONE);
		Color = 0xffffff | ((int)(Factor * 255.0f) << 24);
		OGL_DrawQUAD(W,H,Color);
//*/
		glBlendFunc(GL_ONE, GL_SRC_ALPHA );
		Color = 0xffffff | ((int)(Factor * 255.0f) << 24);
		OGL_DrawQUAD(W,H,Color);

	}
	if (invert)
	{
		glEnable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
		Color = 0xffffff;
		OGL_DrawQUAD(W,H,Color);
	}
}
void OGL_Glow(float Factor)
{
	int invert;
	float W,H,FactorSave ;
	int Color;
	FactorSave = Factor;
	invert = 0;
	OGL_AE_SetViewport(&W , &H);
	OGL_AE_ValidateTexture(&FBTexture[6], GL_RGB,BIGESTLARGOR,BIGESTHAUTOR);
	OGL_AE_CopyTexture(&FBTexture[6] );
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);

	/* 1) Mul dst with alpha dest */
	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_ZERO, GL_DST_ALPHA);
	Color = 0;
	OGL_DrawQUAD(W,H,Color);//*/

	/* 2) Multiply dest by Factor */
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, FBTexture[6]);
	glBlendFunc(GL_DST_ALPHA, GL_ONE);
	if (Factor > 1.0f)
	{
		while (Factor > 1.0f)
		{
			OGL_DrawQUAD(W,H,0xffffff);
			Factor -= 1.0f;
		}
		glBlendFunc(GL_DST_COLOR, GL_ONE);
		Color = (ULONG)(Factor * 255.0f);
		Color |= Color << 8;
		Color |= Color << 16;
		OGL_DrawQUAD(W,H,Color);
	} else
	{
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		Color = (ULONG)(Factor * 255.0f);
		Color |= Color << 8;
		Color |= Color << 16;
		OGL_DrawQUAD(W,H,Color);
	}

	/* 3) blur dst */
	OGL_BigBlur(0.01f,0);


	/* 4) Erase Blur on original */
	glBlendFunc(GL_ZERO, GL_ONE_MINUS_DST_ALPHA);
	OGL_DrawQUAD(W,H,0xffffff);

    /* 5) Add original */
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, FBTexture[6]);
	glBlendFunc(GL_ONE, GL_ONE);
	OGL_DrawQUAD(W,H,0xffffff);

	glBlendFunc(GL_DST_ALPHA , GL_ONE);
	while (FactorSave > 1.0f)
	{
		OGL_DrawQUAD(W,H,0xffffff);
		FactorSave -= 1.0f;
	}
	Color = 0xffffff | ((int)(Factor * 255.0f) << 24);
	OGL_DrawQUAD(W,H,Color);//*/
	glBlendFunc(GL_ONE, GL_ONE);
}


void OGL_FoggyBlur(float Factor)
{
	int invert;
	float W,H ;
//	int Color;
	invert = 0;
	OGL_AE_SetViewport(&W , &H);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	OGL_DrawQUADZ(W,H,SKYZ,0xFF000000);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);

	OGL_AE_ValidateTexture(&FBTexture[6], GL_RGB,BIGESTLARGOR,BIGESTHAUTOR);
	OGL_AE_CopyTexture(&FBTexture[6] );


	
	OGL_BigBlur(0.05f,0);
	glBindTexture(GL_TEXTURE_2D, FBTexture[6]);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA , GL_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	Factor *= 255.0f;
	OGL_DrawQUAD(W,H,0xffffff | (((int)Factor)<<24));

}

void OGL_NearBlur(float Factor)
{
	int invert;
	float W,H ,SDFSDF;
//	int Color;
	invert = 0;
	OGL_AE_SetViewport(&W , &H);
	OGL_AE_ValidateTexture(&FBTexture[6], GL_RGB,BIGESTLARGOR,BIGESTHAUTOR);
	OGL_AE_CopyTexture(&FBTexture[6] );
	
	SDFSDF = fAbs(Factor);
	SDFSDF = 1.0f - SDFSDF;
	SDFSDF *=SDFSDF;
	SDFSDF *=SDFSDF;
	SDFSDF *=SDFSDF;
	SDFSDF *=SDFSDF;
	SDFSDF = 1.0f - SDFSDF;

	OGL_BigBlur(0.0020f * SDFSDF,0);
	
	glBindTexture(GL_TEXTURE_2D, FBTexture[6]);

	glEnable(GL_DEPTH_TEST);
	if (Factor < 0.0)
		glDepthFunc(GL_LEQUAL);
	else
		glDepthFunc(GL_GEQUAL);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	OGL_DrawQUADZ(W,H,SKYZ + 0.15f,0xffffffff);
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);

}
extern u32 JADE_HorizontalSymetry;
#ifdef ACTIVE_EDITORS
int BackIsClear,FrontIsClear;
#endif
void AFTEREFFX_GlowFromZList()
{
	MATH_tdst_Matrix					st_SavedProjection;
	MATH_tdst_Matrix					st_SavedMV;

#ifdef ACTIVE_EDITORS
	if ((ENG_gb_EngineRunning) && (!GDI_gpst_CurDD->ShowAEInEngine)) return;
	if ((!ENG_gb_EngineRunning) && (!GDI_gpst_CurDD->ShowAEEditor)) return;
	if (GDI_gpst_CurDD->ul_DisplayFlags & (GDI_Cul_DF_ShowCOB|GDI_Cul_DF_DoNotRender)) return;
	if (!BackIsClear) return;
#endif

	glMatrixMode(GL_PROJECTION);
	glGetFloatv(GL_PROJECTION_MATRIX,(GLfloat *) &st_SavedProjection);
	glMatrixMode(GL_MODELVIEW);
	glGetFloatv(GL_PROJECTION_MATRIX,(GLfloat *) &st_SavedMV);


	OGL_ES_PushState();

	if (OGL_AllAfterEffects.Glow)	OGL_Glow(OGL_AllAfterEffects.Glow);

	OGL_ES_PopState();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat *) &st_SavedProjection);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat *) &st_SavedMV);

}

void OGL_AE_Before2D()
{
#ifdef ACTIVE_EDITORS
	extern BOOL ENG_gb_EngineRunning;
	extern u32 GetSPG2CachePercent();
	if (!BackIsClear) return;

	/* FogMax Effect */
/*	{
		OGL_ES_PushState();
		OGL_ShowZBuffer();

		OGL_ES_PopState();
	}
//*/



	


	BackIsClear = 0;
#ifdef ACTIVE_EDITORS
	{
	extern float NotRunningTimer;
	NotRunningTimer += 1.0f / 30.0f;
	}
#endif
#endif

	if (GDI_gpst_CurDD->ul_DisplayFlags & (GDI_Cul_DF_ShowCOB|GDI_Cul_DF_DoNotRender)) return;

	{
		float Viewport[4];
		glGetFloatv(GL_VIEWPORT  ,  Viewport);
		if (GetUpperPowerOf2((u32)Viewport[2]) > (s32)BIGESTLARGOR) 
		{
			BIGESTLARGOR = (u32)GetUpperPowerOf2((u32)Viewport[2]);
			OGL_AE_Radapt();
		}
		if (GetUpperPowerOf2((u32)Viewport[3]) > (s32)BIGESTHAUTOR) 
		{
			BIGESTHAUTOR = GetUpperPowerOf2((u32)Viewport[3]);
			OGL_AE_Radapt();
		}

	}
	

	if (GDI_gpst_CurDD->GlobalMul2X)		
	{
		OGL_ES_PushState();
		OGL_AE_BrightnessContrast(0.5f + GDI_gpst_CurDD->GlobalMul2XFactor * 0.5f, 0.5f);
		OGL_ES_PopState();
	}
	if (GDI_gpst_CurDD->ShowAlphaBuffer)	
	{
		OGL_ES_PushState();
		OGL_ShowAlphaBuffer();
		OGL_ES_PopState();
	}


#ifdef ACTIVE_EDITORS
	if ((ENG_gb_EngineRunning) && (!GDI_gpst_CurDD->ShowAEInEngine)) goto AE_END;
	if ((!ENG_gb_EngineRunning) && (!GDI_gpst_CurDD->ShowAEEditor)) goto AE_END;
#endif

	OGL_ES_PushState();
/*
	{
		static float Alpha = 0.0f;
		OGL_NearBlur(fCos(Alpha) );
		OGL_NearBlur(fCos(Alpha) );
		Alpha += 0.125f;
	}//*/
//	OGL_FoggyBlur(0.6f)	;
#if 1
	if (SplashNumber) OGL_DrawSpah();
	if (OGL_AllAfterEffects.MotionBlurFactor)			OGL_MotionBlur(OGL_AllAfterEffects.MotionBlurFactor);
	else	LastMotionBlur = 1;
	if (OGL_AllAfterEffects.MotionSmoothFactor)			OGL_MotionSmoothAuto(OGL_AllAfterEffects.MotionSmoothFactor);
	if (OGL_AllAfterEffects.BWFactor)					OGL_BlackWhite(OGL_AllAfterEffects.BWFactor);//*/
//	if (OGL_AllAfterEffects.SharpenFactor)				OGL_Remanance(-OGL_AllAfterEffects.SharpenFactor);//*/
	if (OGL_AllAfterEffects.FoggyBlur)					OGL_FoggyBlur(OGL_AllAfterEffects.FoggyBlur);
	
	OGL_AE_ColorBalance(OGL_AllAfterEffects.CB_Intensity, OGL_AllAfterEffects.CB_Spectre);
	OGL_AE_BrightnessContrast(OGL_AllAfterEffects.Contrast/* + fRand(0.0f,0.1f)*/, OGL_AllAfterEffects.Brighness);
	//OGL_AllAfterEffects.BorderBrightNess = 0.5f;
	if (OGL_AllAfterEffects.BorderBrightNess)			OGL_BorderBrightness(OGL_AllAfterEffects.BorderBrightNess);
	if (OGL_AllAfterEffects.ZoomSmoothRemananceFactor)	OGL_ZooSmoothRemanance(OGL_AllAfterEffects.ZoomSmoothRemananceFactor , &OGL_AllAfterEffects.ZoomSmoothRemananceLightDirection);
	if (OGL_AllAfterEffects.RemanaceFactor)				OGL_Remanance(OGL_AllAfterEffects.RemanaceFactor);
	//if (OGL_AllAfterEffects.Glow)						OGL_Glow(OGL_AllAfterEffects.Glow);

	if (OGL_AllAfterEffects.BlurFactor)					OGL_BigBlur(OGL_AllAfterEffects.BlurFactor * 0.02f,0);
	if (OGL_AllAfterEffects.WarpFactor)					OGL_WRAP(OGL_AllAfterEffects.WarpFactor);

	if (OGL_AllAfterEffects.ZoomSmoothFactor)			
	{
		if (OGL_AllAfterEffects.ZSDirIsValidated)
			OGL_ZooSmoothAndCenter(OGL_AllAfterEffects.ZoomSmoothFactor * 0.2f ,&OGL_AllAfterEffects.ZSDir);
		else
			OGL_ZoomSmooth(OGL_AllAfterEffects.ZoomSmoothFactor * 0.2f ,0,0, 0);//*/
		OGL_AllAfterEffects.ZSDirIsValidated = 0;
	}
	if (OGL_AllAfterEffects.SpinSmoothFactor)			OGL_ZoomSmooth(OGL_AllAfterEffects.SpinSmoothFactor * 0.1f ,0,0, 1);//*/

	if (OGL_AllAfterEffects.PerfectGow_Factor)			OGL_PerfectGlow(OGL_AllAfterEffects.PerfectGow_Factor,OGL_AllAfterEffects.PerfectGow_Thresh,OGL_AllAfterEffects.PerfectGow_Radius);//*/
	

#endif

	OGL_ES_PopState();

AE_END:
	JADE_HorizontalSymetry = GDI_gpst_CurDD->GlobalXInvert;
#ifdef ACTIVE_EDITORS
	if (!ENG_gb_EngineRunning) JADE_HorizontalSymetry = 0;
#endif
	if (JADE_HorizontalSymetry)		
	{
		OGL_ES_PushState();
		OGL_FlipH();
		OGL_ES_PopState();
	}
}

void OGL_DisplayDebugPerObject(WOR_tdst_World *_pst_World)
{
#ifdef ACTIVE_EDITORS
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;
	OBJ_tdst_GameObject *pst_GO;
	float StatMax,StatMin;
	float fCurrentFocale = 1.0f / fNormalTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2);		
	static u32 LasTime = 0;
	u32 DeltaTime,ulNumberOfSel,TotalEngineTickNumber;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DeltaTime = timeGetTime() - LasTime;
	LasTime = timeGetTime();
	DeltaTime *= 255;
	DeltaTime /= 2000;
	ulNumberOfSel = 0;
	if (!DeltaTime) DeltaTime = 1;
	if (DeltaTime > 255) DeltaTime = 255;
	StatMax = -10000000000000000.0f;
	StatMin = 10000000000000000.0f;
	pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
	for(; pst_Elem <= pst_LastElem; pst_Elem++)
	{
		/* get and test game object */
		pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		if ((pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected) == 0)
			pst_GO->DrawInfoName = -1;
		else
		{
			if (pst_GO->DrawInfoName & 0xffffff00)
				pst_GO->DrawInfoName = 255;
			else
			{
				pst_GO->DrawInfoName-=DeltaTime;
				if (pst_GO->DrawInfoName & 0xffffff00)
					pst_GO->DrawInfoName = 0;
			}
		}
		if ((pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected) && (!(pst_GO->DrawInfoName & 0xffffff00)))
		{
			ulNumberOfSel++;
		}
		if (GDI_gpst_CurDD->DisplayTriInfo && pst_GO->NumberOfTris && (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected))
		{
			float STAT;
			MATH_tdst_Vector		MotionSmoothPoint; 
			MATH_TransformVertex(&MotionSmoothPoint, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , &pst_GO->pst_GlobalMatrix->T);
			if (MotionSmoothPoint . z > 0.0f)
			{
				STAT = (float)pst_GO->NumberOfTris / (float)(pst_GO->uc_LOD_Vis + 1);
				if (StatMax < STAT) StatMax = STAT;
				if (StatMin > STAT) StatMin = STAT;
			}
		}

	}
	pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
	for(; pst_Elem <= pst_LastElem; pst_Elem++)
	{
		float XX,YY,TEX_SIZE;
		extern void OGL_DrawText_NoRecenter(int X,int Y,char *P_String,long Color,float Alpha,float SizeFactor);
		MATH_tdst_Vector		MotionSmoothPoint; 
		MATH_tdst_Vector _pst_Dest;
		u8 String[100];
		/* get and test game object */
		TEX_SIZE = 1.0f;
		pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		MATH_TransformVertex(&MotionSmoothPoint, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , &pst_GO->pst_GlobalMatrix->T);
		if (MotionSmoothPoint . z > 0.0f)
		{
			extern void SOFT_Project(MATH_tdst_Vector *_pst_Dest, MATH_tdst_Vector *_pst_Src, LONG _l_Number, CAM_tdst_Camera *_pst_Cam);
			SOFT_Project(&_pst_Dest, &MotionSmoothPoint, 1, &GDI_gpst_CurDD->st_Camera);
			XX = _pst_Dest.x - GDI_gpst_CurDD->st_Device.Vx;//*/;
			YY = _pst_Dest.y - GDI_gpst_CurDD->st_Device.Vy;//*/
			YY -= 16 * TEX_SIZE;
		} else
		{
			if (MotionSmoothPoint.x > 0.0f)
				XX = 10000.0f;
			else
				XX = -10000.0f;

			if (MotionSmoothPoint.y > 0.0f)
				YY = -10000.0f;
			else
				YY = 10000.0f;

		}
		if (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected) 
		{
			if (((!(pst_GO->DrawInfoName & 0xffffff00)) && pst_GO->DrawInfoName) && (ulNumberOfSel < 20))
			{
				s32 LX,LY;

				sprintf((char* )String,"%s",pst_GO->sz_Name);
#ifdef JADEFUSION
				LX = XX-((float)((L_strlen((char* )String) * 8)>>1)*TEX_SIZE);
				LY = YY;
				if (LX < -GDI_gpst_CurDD->st_Device.Vx) 
					LX = -GDI_gpst_CurDD->st_Device.Vx;
				if (LX < 0.0f) 
					LX = 0.0f;
				if (LX + L_strlen((char* )String) * 8.f * TEX_SIZE > GDI_gpst_CurDD->st_Device.Vx + GDI_gpst_CurDD->st_Device.Vw) 
					LX = GDI_gpst_CurDD->st_Device.Vx + GDI_gpst_CurDD->st_Device.Vw - L_strlen((char* )String) * 8.f * TEX_SIZE;
				if (LY < -GDI_gpst_CurDD->st_Device.Vy) 
					LY = -GDI_gpst_CurDD->st_Device.Vy;
				if (LY < 0.0f) 
					LY = 0.0f;
				if (LY > GDI_gpst_CurDD->st_Device.Vy + GDI_gpst_CurDD->st_Device.Vh - GDI_gpst_CurDD->st_Camera.l_ViewportRealTop - 16.f * TEX_SIZE) 
					LY = GDI_gpst_CurDD->st_Device.Vy + GDI_gpst_CurDD->st_Device.Vh - GDI_gpst_CurDD->st_Camera.l_ViewportRealTop - 16.f * TEX_SIZE;
				OGL_DrawText_NoRecenter(LX,LY,(char* )String,0x80ffff,3.0f * (float)pst_GO->DrawInfoName / 255.0f,TEX_SIZE);
#else
				LX = (s32)(XX-((float)((L_strlen(String) * 8)>>1)*TEX_SIZE));
				LY = (s32)YY;
				if (LX < -GDI_gpst_CurDD->st_Device.Vx) 
					LX = -GDI_gpst_CurDD->st_Device.Vx;
				if (LX < (s32)0.0f) 
					LX = (s32)0.0f;
				if (LX + L_strlen(String) * 8.f * TEX_SIZE > GDI_gpst_CurDD->st_Device.Vx + GDI_gpst_CurDD->st_Device.Vw) 
					LX = (s32)(GDI_gpst_CurDD->st_Device.Vx + GDI_gpst_CurDD->st_Device.Vw - L_strlen(String) * 8.f * TEX_SIZE);
				if (LY < -GDI_gpst_CurDD->st_Device.Vy) 
					LY = -GDI_gpst_CurDD->st_Device.Vy;
				if (LY < (s32)0.0f) 
					LY = (s32)0.0f;
				if (LY > (s32)(GDI_gpst_CurDD->st_Device.Vy + GDI_gpst_CurDD->st_Device.Vh - GDI_gpst_CurDD->st_Camera.l_ViewportRealTop - 16.f * TEX_SIZE)) 
					LY = (s32)(GDI_gpst_CurDD->st_Device.Vy + GDI_gpst_CurDD->st_Device.Vh - GDI_gpst_CurDD->st_Camera.l_ViewportRealTop - 16.f * TEX_SIZE);
				OGL_DrawText_NoRecenter(LX,LY,String,0x80ffff,3.0f * (float)pst_GO->DrawInfoName / 255.0f,TEX_SIZE);
#endif
				YY -= 16 * TEX_SIZE;
			}
			if (GDI_gpst_CurDD->DisplayTriInfo && pst_GO->NumberOfTris)
			{
				/* Project cord.*/
				u32 ColorAdd;
				s32 LX,LY;

				sprintf((char* )String,"%d",pst_GO->NumberOfTris);

#ifdef JADEFUSION
				LX = XX-((float)((L_strlen((char* )String) * 8)>>1)*TEX_SIZE);
				LY = YY;
				if (LX < -GDI_gpst_CurDD->st_Device.Vx) 
					LX = -GDI_gpst_CurDD->st_Device.Vx;
				if (LX < 0.0f) 
					LX = 0.0f;
				if (LX + L_strlen((char* )String) * 8.f * TEX_SIZE > GDI_gpst_CurDD->st_Device.Vx + GDI_gpst_CurDD->st_Device.Vw) 
					LX = GDI_gpst_CurDD->st_Device.Vx + GDI_gpst_CurDD->st_Device.Vw - L_strlen((char* )String) * 8.f * TEX_SIZE;
				if (LY < -GDI_gpst_CurDD->st_Device.Vy) 
					LY = -GDI_gpst_CurDD->st_Device.Vy;
				if (LY < 0.0f) 
					LY = 0.0f;
				if (LY > GDI_gpst_CurDD->st_Device.Vy + GDI_gpst_CurDD->st_Device.Vh - GDI_gpst_CurDD->st_Camera.l_ViewportRealTop - 16.f * TEX_SIZE) 
					LY = GDI_gpst_CurDD->st_Device.Vy + GDI_gpst_CurDD->st_Device.Vh - GDI_gpst_CurDD->st_Camera.l_ViewportRealTop - 16.f * TEX_SIZE;
#else
				LX = (s32)(XX-((float)((L_strlen(String) * 8)>>1)*TEX_SIZE));
				LY = (s32)(YY);
				if (LX < -GDI_gpst_CurDD->st_Device.Vx) 
					LX = -GDI_gpst_CurDD->st_Device.Vx;
				if (LX < 0) 
					LX = 0;
				if (LX + L_strlen(String) * 8.f * TEX_SIZE > GDI_gpst_CurDD->st_Device.Vx + GDI_gpst_CurDD->st_Device.Vw) 
					LX = (s32)(GDI_gpst_CurDD->st_Device.Vx + GDI_gpst_CurDD->st_Device.Vw - L_strlen(String) * 8.f * TEX_SIZE);
				if (LY < (s32)(-GDI_gpst_CurDD->st_Device.Vy)) 
					LY = (s32)(-GDI_gpst_CurDD->st_Device.Vy);
				if (LY < 0) 
					LY = 0;
				if (LY > (s32)(GDI_gpst_CurDD->st_Device.Vy + GDI_gpst_CurDD->st_Device.Vh - GDI_gpst_CurDD->st_Camera.l_ViewportRealTop - 16.f * TEX_SIZE)) 
					LY = (s32)(GDI_gpst_CurDD->st_Device.Vy + GDI_gpst_CurDD->st_Device.Vh - GDI_gpst_CurDD->st_Camera.l_ViewportRealTop - 16.f * TEX_SIZE);
#endif

				ColorAdd = (u32)(((pst_GO->NumberOfTris / (pst_GO->uc_LOD_Vis + 1)) - StatMin) / (StatMax - StatMin) * 255.0f);
				if (ColorAdd > 255) ColorAdd = 255;
				ColorAdd |= ColorAdd << 8;
				ColorAdd |= 0x800000;
				ColorAdd ^= 0x00ff00;
				OGL_DrawText_NoRecenter(LX,LY,(char* )String,ColorAdd,1.0f,TEX_SIZE);
				pst_GO->NumberOfTris = 0;
			}
		}
	}		
	if (GDI_gpst_CurDD->ColorCostIA)
	{
		TotalEngineTickNumber = 0;
		pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_EOT.st_AI);
		pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_EOT.st_AI);
		for(; pst_Elem <= pst_LastElem; pst_Elem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO)) continue;
			TotalEngineTickNumber += pst_GO->EngineTicksNumber;
		}

		pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_EOT.st_AI);
		pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_EOT.st_AI);
		for(; pst_Elem <= pst_LastElem; pst_Elem++)
		{
			float XX,YY,TEX_SIZE;
			extern void OGL_DrawText_NoRecenter(int X,int Y,char *P_String,long Color,float Alpha,float SizeFactor);
			MATH_tdst_Vector		MotionSmoothPoint; 
			MATH_tdst_Vector _pst_Dest;
			u8 String[100];
			/* get and test game object */
			TEX_SIZE = 1.0f;
			pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO)) continue;
			MATH_TransformVertex(&MotionSmoothPoint, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , &pst_GO->pst_GlobalMatrix->T);
			if (MotionSmoothPoint . z > 0.0f)
			{
				extern void SOFT_Project(MATH_tdst_Vector *_pst_Dest, MATH_tdst_Vector *_pst_Src, LONG _l_Number, CAM_tdst_Camera *_pst_Cam);
				SOFT_Project(&_pst_Dest, &MotionSmoothPoint, 1, &GDI_gpst_CurDD->st_Camera);
				XX = _pst_Dest.x - GDI_gpst_CurDD->st_Device.Vx;//*/;
				YY = _pst_Dest.y - GDI_gpst_CurDD->st_Device.Vy;//*/
				YY -= 16 * TEX_SIZE;
			} else
			{
				if (MotionSmoothPoint.x > 0.0f)
					XX = 10000.0f;
				else
					XX = -10000.0f;

				if (MotionSmoothPoint.y > 0.0f)
					YY = -10000.0f;
				else
					YY = 10000.0f;

			}
			if (pst_GO->EngineTicksNumber)
			{
				s32 LX,LY;
				u32 Percent;

				Percent = (int)(100.0f * (float)pst_GO->EngineTicksNumber/(float)TotalEngineTickNumber);
				if (Percent > 100) Percent = 0;
				if (Percent > GDI_gpst_CurDD->ColorCostIAThresh)
					sprintf((char* )String,"%d%%->%s",Percent,pst_GO->sz_Name);
				else
					sprintf((char* )String,"%d%%",Percent);
				Percent *= 255;
				Percent /= 100;

				if (Percent > 1)

				{ 
#ifdef JADEFUSION
					LX = (s32)(XX-((float)((L_strlen((char* )String) * 8)>>1)*TEX_SIZE));
#else
					LX = (s32)(XX-((float)((L_strlen(String) * 0))*TEX_SIZE));
#endif
					LY = (s32)YY;
					if (LX < -GDI_gpst_CurDD->st_Device.Vx) 
						LX = -GDI_gpst_CurDD->st_Device.Vx;
					if (LX < (s32)0.0f) 
						LX = (s32)0.0f;
#ifdef JADEFUSION
					if (LX + L_strlen((char* )String) * 8.f * TEX_SIZE > GDI_gpst_CurDD->st_Device.Vx + GDI_gpst_CurDD->st_Device.Vw) 
						LX = (s32)(GDI_gpst_CurDD->st_Device.Vx + GDI_gpst_CurDD->st_Device.Vw - L_strlen((char* )String) * 8.f * TEX_SIZE);
#else
					if (LX + L_strlen(String) * 8.f * TEX_SIZE > GDI_gpst_CurDD->st_Device.Vx + GDI_gpst_CurDD->st_Device.Vw) 
						LX = (s32)(GDI_gpst_CurDD->st_Device.Vx + GDI_gpst_CurDD->st_Device.Vw - L_strlen(String) * 8.f * TEX_SIZE);
#endif
					if (LY < -GDI_gpst_CurDD->st_Device.Vy) 
						LY = -GDI_gpst_CurDD->st_Device.Vy;
					if (LY < (s32)0.0f) 
						LY = (s32)0.0f;
					if (LY > (s32)(GDI_gpst_CurDD->st_Device.Vy + GDI_gpst_CurDD->st_Device.Vh - GDI_gpst_CurDD->st_Camera.l_ViewportRealTop - 16.f * TEX_SIZE)) 
						LY = (s32)(GDI_gpst_CurDD->st_Device.Vy + GDI_gpst_CurDD->st_Device.Vh - GDI_gpst_CurDD->st_Camera.l_ViewportRealTop - 16.f * TEX_SIZE);
					OGL_DrawText_NoRecenter(LX,LY,(char* )String,(lMax(255 - Percent * 6,0)<<16) + (lMax(255 - Percent * 6,0)<<8) + (lMin(Percent * 6,255)<<0),3.0f * (float)pst_GO->DrawInfoName / 255.0f,TEX_SIZE);
					YY -= 16 * TEX_SIZE;
				}
			}
			pst_GO->EngineTicksNumber = 0;
		}
	}

#endif
}


void OGL_AE_DebugInfo(WOR_tdst_World *_pst_World)
{
#define GHOST_Blender 0.5f * fMax(TIM_gf_dt, 1.0f/ 30.0f)
#define ALERTE_SIZE 1.0f	
    
    char string[1024];
	u32 CachePercent,YPos;
	extern u32 Stats_ulNumberOfTRiangles;
    extern u32 Stats_ulCallToDrawNb;
	extern ULONG SPG2_PrimitivCounter ;
	extern float WOR_TimeSinceLastWorldLoad;
	extern float TIM_gf_MainClockReal;
	static u32 TreshTri;
	static u32 TreshSPG2;
	static u32 TreshDraw;
	static u32 TreshObj;
	static float Alpha_TRIS = 0.0f;
	static float Alpha_SPG2 = 0.0f;
	static float Alpha_DRAW = 0.0f;
	static float Alpha_OBJ = 0.0f;
	float AlphaMEM;
#ifdef ACTIVE_EDITORS
	static float SupraDT = 0.0;
#endif
	YPos = 10;

#ifdef ACTIVE_EDITORS
#ifdef JADEFUSION
	extern ULONG GetSPG2CachePercent();
#endif
	if (GDI_gpst_CurDD->ShowHistogramm)	
	{
		OGL_ES_PushState();
		GDI_gpst_CurDD->st_ScreenFormat.ul_Flags &= ~GDI_Cul_SFF_OccupyAll;
		GDI_gpst_CurDD->st_ScreenFormat.ul_Flags |= GDI_Cul_SFF_CropToWindow;
		OGL_ShowHistogramm();
		OGL_ES_PopState();
	}

	
	if (GDI_gpst_CurDD->DrawGraphicDebugInfo)
	{
		u32 TRI_ALarm = GDI_gpst_CurDD->TRI_ALarm;
		u32 DRAW_ALarm = GDI_gpst_CurDD->DRAW_ALarm;
		u32 SPG_ALarm = GDI_gpst_CurDD->SPG_ALarm;
		u32 OBJ_ALarm = GDI_gpst_CurDD->OBJ_ALarm;
        
        // Triangle nb alarm
		if (Stats_ulNumberOfTRiangles > TreshTri)
		{
			Alpha_TRIS += 4.0f * GHOST_Blender;
			TreshTri = TRI_ALarm;
		}
		else
		{
			Alpha_TRIS -= GHOST_Blender;
			TreshTri = TRI_ALarm + (TRI_ALarm >> 3) ;
		}
		if (Alpha_TRIS > 0.01f)
		{
			sprintf(string,"Num of tris:%d", Stats_ulNumberOfTRiangles );
			if (Stats_ulNumberOfTRiangles > TRI_ALarm + (TRI_ALarm >> 2))
				OGL_DrawText(10,YPos,string,0x8080ff,Alpha_TRIS,ALERTE_SIZE);
			else
				OGL_DrawText(10,YPos,string,0xffffff,Alpha_TRIS,ALERTE_SIZE);
			YPos += (int)(16.0 * ALERTE_SIZE);
		}
		Alpha_TRIS = fMax(Alpha_TRIS , 0.0f);
		Alpha_TRIS = fMin(Alpha_TRIS , 1.0f);

        // Object nb alarm
        if (Stats_ulCallToDrawNb > TreshObj)
		{
			Alpha_OBJ += 4.0f * GHOST_Blender;
			TreshObj = OBJ_ALarm;
		}
		else
		{
			Alpha_OBJ -= GHOST_Blender;
			TreshObj = OBJ_ALarm + (OBJ_ALarm >> 3) ;
		}
		if (Alpha_OBJ > 0.01f)
		{
			sprintf(string,"Num of obj:%d", Stats_ulCallToDrawNb );
			if (Stats_ulCallToDrawNb > OBJ_ALarm + (OBJ_ALarm >> 2))
				OGL_DrawText(10,YPos,string,0x8080ff,Alpha_OBJ,ALERTE_SIZE);
			else
				OGL_DrawText(10,YPos,string,0xffffff,Alpha_OBJ,ALERTE_SIZE);
			YPos += (int)(16.0 * ALERTE_SIZE);
		}
		Alpha_OBJ = fMax(Alpha_OBJ , 0.0f);
		Alpha_OBJ = fMin(Alpha_OBJ , 1.0f);

        // SPG2 alarm
		CachePercent = GetSPG2CachePercent();
		if (SPG2_PrimitivCounter < 100) CachePercent = 0;
		if (!CachePercent && SPG2_PrimitivCounter) CachePercent = 1;
		if ((CachePercent > 95) || (SPG2_PrimitivCounter > SPG_ALarm))
			Alpha_SPG2 += 4.0f * GHOST_Blender;
		else
			Alpha_SPG2 -= GHOST_Blender;

		if (Alpha_SPG2 > 0.01f)
		{
			sprintf(string,"Num of SPG2:%d(%d%%)", SPG2_PrimitivCounter , CachePercent);
			if ( (CachePercent >= 98) || (SPG2_PrimitivCounter > (SPG_ALarm) + (SPG_ALarm >> 2)) )
				OGL_DrawText(10,YPos,string,0x8080ff , Alpha_SPG2,ALERTE_SIZE);
			else
				OGL_DrawText(10,YPos,string,0xffffff , Alpha_SPG2,ALERTE_SIZE);
			YPos += (int)(16.0 * ALERTE_SIZE);
		} 
		Alpha_SPG2 = fMax(Alpha_SPG2 , 0.0f);
		Alpha_SPG2 = fMin(Alpha_SPG2 , 1.0f);

        // Draw ratio alarm
		if (Stats_ulNumberOfTRiangles < TreshDraw * Stats_ulCallToDrawNb )
		{
			Alpha_DRAW += 4.0f * GHOST_Blender;
			TreshDraw = DRAW_ALarm;
		}
		else
		{
			Alpha_DRAW -= GHOST_Blender;
			TreshDraw = DRAW_ALarm + (DRAW_ALarm >> 3) ;
		}
		if ((Alpha_DRAW > 0.01f) && (Stats_ulCallToDrawNb>0))
		{
			sprintf(string,"Triangles per draw :%d", (Stats_ulNumberOfTRiangles/Stats_ulCallToDrawNb) );
			if (Stats_ulNumberOfTRiangles < (DRAW_ALarm + (DRAW_ALarm >> 2))*Stats_ulCallToDrawNb)
				OGL_DrawText(10,YPos,string,0x8080ff,Alpha_DRAW,ALERTE_SIZE);
			else
				OGL_DrawText(10,YPos,string,0xffffff,Alpha_DRAW,ALERTE_SIZE);
			YPos += (int)(16.0 * ALERTE_SIZE);
		}
		Alpha_DRAW = fMax(Alpha_DRAW , 0.0f);
		Alpha_DRAW = fMin(Alpha_DRAW , 1.0f);

		Stats_ulNumberOfTRiangles = 0;
        Stats_ulCallToDrawNb = 0;
	}
#endif
#ifdef MEM_SPY
	//GDI_gpst_CurDD->DisplayMemInfo = 1;
#ifdef ACTIVE_EDITORS
	if (WOR_TimeSinceLastWorldLoad == 0.0f) SupraDT = 0;
	WOR_TimeSinceLastWorldLoad = 0.00001f;
	SupraDT += 1.0f / 30.0f;
#endif	
	if (GDI_gpst_CurDD->DisplayMemInfo)
		AlphaMEM = 1.0f;
	else
#ifdef ACTIVE_EDITORS
		AlphaMEM = 2.0f - (SupraDT - WOR_TimeSinceLastWorldLoad);
#else
		AlphaMEM = 0.0f;//10.0f - (TIM_gf_MainClock - WOR_TimeSinceLastWorldLoad);
#endif
	if (AlphaMEM > 1.0f) AlphaMEM = 1.0f;
	if (AlphaMEM > 0.0f)
	{
		//extern u32 MEM_ulSummaryNb;
		extern	MEM_stSummaryElement *MEM_SortSummaryTable[MEM_ulSummaryTableSize];
		extern void MEM_vBuildSummary();
		u32 CurrentNum,Summ;
		CurrentNum = MEM_ulSummaryNb;
		MEM_vBuildSummary();
		if (MEM_ulSummaryNb != -1)
		{
			char string_MO[1024];
			Summ  = 0;
			for (CurrentNum = 0 ; CurrentNum < MEM_ulSummaryNb ; CurrentNum++) Summ += MEM_SortSummaryTable[CurrentNum]->ulAllocSize;
			sprintf(string_MO,"%2d.%03dMo ", Summ >> 20 , (((Summ >> 10) & 1023) * 999) / 1024 );
			sprintf(string,"TOTAL:%s", string_MO );
			OGL_DrawText(10,YPos,string,0xffffffff, 1.0f,1.0f);
			YPos += (int)(16.0 * 1.0f);

			for (CurrentNum = 0 ; CurrentNum < MEM_ulSummaryNb ; CurrentNum++)
			{
				if (MEM_SortSummaryTable[CurrentNum]->ulAllocSize > 128 * 1024)
				{
					u32 color;
					sprintf(string_MO,"%2d.%03dMo ", MEM_SortSummaryTable[CurrentNum]->ulAllocSize >> 20 , (((MEM_SortSummaryTable[CurrentNum]->ulAllocSize >> 10) & 1023) * 999) / 1024 );

					color  = (MEM_SortSummaryTable[CurrentNum]->ulKey & (0x3 << 4)) << (24 - 2 - 4);
					color |= (MEM_SortSummaryTable[CurrentNum]->ulKey & (0x3 << 2)) << (16 - 2 - 2);
					color |= (MEM_SortSummaryTable[CurrentNum]->ulKey & (0x3 << 0)) << (8 - 2);
					color |= 0x3f3f3f;

					sprintf(string,"%s:%s", MEM_SortSummaryTable[CurrentNum]->sHeader , string_MO );
					OGL_DrawText(10,YPos,string,color , AlphaMEM,1.0f);
					YPos += (int)(16.0 * 1.0f);
				}
			}
		}
	}
#endif
	OGL_DisplayDebugPerObject(_pst_World);
}

void OGL_AE_Reinit()
{
}

void OGL_AE_BeforeFlip()
{
#ifdef ACTIVE_EDITORS
	int SWAP;
	SWAP = BackIsClear;
	BackIsClear = FrontIsClear;
	FrontIsClear = SWAP ;
#endif
}

void OGL_AE_BeforeClear()
{
#ifdef ACTIVE_EDITORS
	BackIsClear = 1;
#endif
}



void OGL_AE_Radapt()
{
	extern u32 ul_OpenglFontTexture;
	extern void OGL_InitAllShadows();
	int Counter = 10;
	OGL_AE_ResetAll();
	while(Counter--)
	{
		if(FBTexture[Counter] != 0xC0DE2004)
		{
#ifdef JADEFUSION
			glDeleteTextures(1, (const GLuint*)&FBTexture[Counter]);
#else
			glDeleteTextures(1, &FBTexture[Counter]);
#endif
			FBTexture[Counter] = 0xC0DE2004;
		}
	}
	if (ul_OpenglFontTexture != -1)
	{
		glDeleteTextures(1, &ul_OpenglFontTexture);
		ul_OpenglFontTexture = -1; 
	}
	OGL_InitAllShadows();
}

#if defined PSX2_TARGET && defined __cplusplus
    }
#endif
