
/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BAStypes.h"

#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"

#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"

#ifndef PSX2_TARGET
#ifndef _GAMECUBE
#ifdef ACTIVE_EDITORS
#include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#endif
#endif
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#include "MATerial/MATSingle.h"
#include "MATerial/MATmulti.h"
#include "MATerial/MATSprite.h"
#include "MATerial/MATCompute.h"

#include "GRObject/GROstruct.h"
#include "GEOmetric/GEO_MRM.h"
#include "SOFT/SOFTlinear.h"

#include "TIMer/TIMdefs.h"

#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_UVTexWave.h"

#ifdef ACTIVE_EDITORS
#include "LINks/LINKtoed.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "GEOmetric/GEOsubobject.h"
#include "GRObject/GROrender.h"
#include "GRObject/GROedit.h"
#endif
#ifdef PSX2_TARGET
#include <eeregs.h>
#include <eestruct.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <libpc.h>
#include "GS_PS2/Gsp.h"
#ifdef GSP_PS2_BENCH
extern u_int NoGEODRAW;
#endif
#endif

#include "BASe/BENch/BENch.h"

#ifdef _GAMECUBE
#define USE_SOFT_UV_MATRICES
#include "GXI_GC/GXI_vertexspace.h"
#include "GXI_GC/GXI_def.h"
#endif
extern MAT_tdst_MultiTexture    MAT_gst_DefaultSingleMaterial;
static float			        MAT_C_f_5Bits = 32768.0f * 8.0f + 16384.0f * 8.0f;
/* static float			        MAT_C_f_7Bits = 32768.0f * 2.0f + 16384.0f * 2.0f; */
static float			        MAT_C_f_8Bits = 32768.0f + 16384.0f;
static float			        MAT_C_f_10Bits = 8192.0f + 4096.0f;
/* static float			        MAT_C_f_12Bits = 2048.0f + 1024.0f; */

static float			        MAT_SinTable[16] =
{
	0.0f,
	0.3826834f,
	0.7071067f,
	0.9238795f,
	1.0f,
	0.9238795f,
	0.7071067f,
	0.3826834f,
	0.0f,
	-0.3826834f,
	-0.7071067f,
	-0.9238795f,
	-1.0f,
	-0.9238795f,
	-0.7071067f,
	-0.3826834f
};

#define MAT_Sin(a)	MAT_SinTable[(a) & 0xf]
#define MAT_Cos(a)	MAT_SinTable[(a + 4) & 0xf]
#define IFAL		lInterpretFloatAsLong
#define ILAF		fInterpretLongAsFloat

extern OBJ_tdst_GameObject    *LIGHT_gpst_Bump;
BOOL GDI_gb_WaveSprite = FALSE;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG	MAT_DdctAlph_Normal[128];
ULONG	MAT_DdctAlph_Symetric[128];
ULONG	MAT_DdctAlph_Negative[128];

void MAT_VUDecompress(MAT_tdst_MTLevel *p_tdstCompressedUV, MAT_tdst_Decompressed_UVMatrix *p_tdstDecompressedUV);

void MAT_ComputeDeductedAlpha(GEO_tdst_Object *pst_Obj, MAT_tdst_MTLevel *pst_MLTTXLVL)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*VSrc, *VSrcLast;
	float				*fDst, Norm;
	ULONG				*ulDst;
	static ULONG		bFirst = 1;
	ULONG				*p_Table;
	ULONG				Counter;
	MATH_tdst_Matrix	Matrix;
	MATH_tdst_Vector	Local;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_GSP_BeginRaster(10);

	if
	(
		(MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags) != MAT_Cc_UV_Phong_GZMO)
	&&	(MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags) != MAT_Cc_UV_Planar_GZMO)
	)	return;


	if(bFirst)
	{
		bFirst = 0;
#define MAT_DDAL_BASE	5
		for(Counter = 0; Counter < MAT_DDAL_BASE; Counter++) MAT_DdctAlph_Symetric[Counter] = 0xff;

		for(Counter = 0; Counter < 16; Counter++)
			MAT_DdctAlph_Symetric[Counter + MAT_DDAL_BASE] = ((16 - Counter) << 5);

		for(Counter = 16 + MAT_DDAL_BASE; Counter < 32; Counter++) MAT_DdctAlph_Symetric[Counter] = 0;

		for(Counter = 0; Counter < 32; Counter++)
			MAT_DdctAlph_Symetric[Counter + 32] = MAT_DdctAlph_Symetric[32 - Counter];

		MAT_DdctAlph_Symetric[0] = 0xff;
		MAT_DdctAlph_Symetric[64] = 0xff;

		for(Counter = 0; Counter < 64; Counter++)
		{
			if(MAT_DdctAlph_Symetric[Counter] & 0xffffff00) MAT_DdctAlph_Symetric[Counter] = 0xff;
			MAT_DdctAlph_Symetric[Counter] <<= 24;
		}

		for(Counter = 65; Counter < 128; Counter++)
		{
			MAT_DdctAlph_Symetric[Counter] = MAT_DdctAlph_Symetric[64];
		}

		for(Counter = 0; Counter < 32; Counter++)
		{
			MAT_DdctAlph_Normal[Counter] = MAT_DdctAlph_Symetric[Counter];
			MAT_DdctAlph_Negative[Counter] = 0;
		}

		for(Counter = 32; Counter < 65; Counter++)
		{
			MAT_DdctAlph_Negative[Counter] = MAT_DdctAlph_Symetric[Counter];
			MAT_DdctAlph_Normal[Counter] = 0;
		}
	}

	p_Table = MAT_DdctAlph_Normal;
	if(MAT_GET_s_Flags(pst_MLTTXLVL->s_AditionalFlags) & MAT_Cul_sFlag_USeSymetric)
		p_Table = MAT_DdctAlph_Symetric;
	else if(MAT_GET_s_Flags(pst_MLTTXLVL->s_AditionalFlags) & MAT_Cul_sFlag_USeNegativ)
		p_Table = MAT_DdctAlph_Negative;

	GDI_gpst_CurDD_SPR.pst_ComputingBuffers->CurrentAlphaField = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ComputedAlpha;
	GEO_UseNormals(pst_Obj);
	VSrc = pst_Obj->dst_PointNormal;
	if(MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags) == MAT_Cc_UV_Phong_GZMO)
		VSrc = SOFT_pst_GetSpecularVectors();

	fDst = (float *) GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ComputedAlpha;
	ulDst = (ULONG *) fDst;
	VSrcLast = VSrc + pst_Obj->l_NbPoints;
	if(MAT_GET_MatrixFrom(pst_MLTTXLVL->s_AditionalFlags) == MAT_CC_OBJECT)
	{
		switch(MAT_GET_XYZ(pst_MLTTXLVL->s_AditionalFlags))
		{
		case MAT_CC_X:
			while(VSrc < VSrcLast)
			{
				*(fDst++) = ((VSrc++)->x * 0.9f + 1.0f) + MAT_C_f_5Bits;
				*(ulDst++) = p_Table[*(ulDst) & 0x7f];
			}
			break;
		case MAT_CC_Y:
			while(VSrc < VSrcLast)
			{
				*(fDst++) = (VSrc++)->y * 0.9f + 1.0f + MAT_C_f_5Bits;
				*(ulDst++) = p_Table[*(ulDst) & 0x7f];
			}
			break;
		case MAT_CC_XYZ:
		case MAT_CC_Z:
			while(VSrc < VSrcLast)
			{
				*(fDst++) = (VSrc++)->z * 0.9f + 1.0f + MAT_C_f_5Bits;
				*(ulDst++) = p_Table[*(ulDst) & 0x7f];
			}
			break;
		}
	}
	else if(MAT_GET_MatrixFrom(pst_MLTTXLVL->s_AditionalFlags) == MAT_CC_WORLD)
	{
		MATH_MakeOGLMatrix(&Matrix, GDI_gpst_CurDD_SPR.pst_CurrentGameObject->pst_GlobalMatrix);
		switch(MAT_GET_XYZ(pst_MLTTXLVL->s_AditionalFlags))
		{
		case MAT_CC_X:
			Local.x = Matrix.Ix;
			Local.y = Matrix.Jx;
			Local.z = Matrix.Kx;
			break;
		case MAT_CC_Y:
			Local.x = Matrix.Iy;
			Local.y = Matrix.Jy;
			Local.z = Matrix.Ky;
			break;
		case MAT_CC_XYZ:
		case MAT_CC_Z:
			Local.x = Matrix.Iz;
			Local.y = Matrix.Jz;
			Local.z = Matrix.Kz;
			break;
		}

		Norm = MATH_f_SqrNormVector(&Local);
		Norm = fOptInvSqrt(Norm);
		Local.x *= Norm;
		Local.y *= Norm;
		Local.z *= Norm;
		while(VSrc < VSrcLast)
		{
			*(fDst++) = (VSrc->x * Local.x + VSrc->y * Local.y + VSrc->z * Local.z) * 0.9f + 1.0f + MAT_C_f_5Bits;
			VSrc++;
			*(ulDst++) = p_Table[*(ulDst) & 0x7f];
		}
	}

	_GSP_EndRaster(10);	
}




/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_SetScale(ULONG *ul_ScaleSPeedPosU, float Scale)
{
	/*~~~~~~~~~~~*/
	ULONG	ulCalc;
	/*~~~~~~~~~~~*/

	ILAF(ulCalc) = Scale;
	*ul_ScaleSPeedPosU &= ~(MAT_Cc_ScaleMask | MAT_Cc_ScaleSign);
	*ul_ScaleSPeedPosU |= ulCalc & (MAT_Cc_ScaleMask | MAT_Cc_ScaleSign);
	return;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_SetSpeed(ULONG *ul_ScaleSPeedPosU, float Speed)
{
	/*~~~~~~~~~~~*/
	ULONG	ulCalc;
	/*~~~~~~~~~~~*/

	ILAF(ulCalc) = Speed;
	*ul_ScaleSPeedPosU &= ~(MAT_Cc_SpeedMask | MAT_Cc_SpeedSign);
	*ul_ScaleSPeedPosU |= (ulCalc >> 16) & (MAT_Cc_SpeedMask | MAT_Cc_SpeedSign);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_SetPos(ULONG *ul_ScaleSPeedPosU, float Pos)
{
	MAT_SetSpeed(ul_ScaleSPeedPosU, Pos);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_SetRotation(ULONG *ul_ScaleSPeedPosU, ULONG *ul_ScaleSPeedPosV, float Alpha)
{
	/*~~~~~~~~~~~*/
	ULONG	ulCalc;
	/*~~~~~~~~~~~*/

	*ul_ScaleSPeedPosU &= ~0x00010001;
	*ul_ScaleSPeedPosV &= ~0x00010001;

	ILAF(ulCalc) = Alpha + MAT_C_f_8Bits;
	*ul_ScaleSPeedPosU |= (ulCalc & 0x80) << 9;
	*ul_ScaleSPeedPosU |= (ulCalc & 0x40) >> 6;
	*ul_ScaleSPeedPosV |= (ulCalc & 0x20) << 11;
	*ul_ScaleSPeedPosV |= (ulCalc & 0x10) >> 4;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float MAT_GetRotation(ULONG ul_ScaleSPeedPosU, ULONG ul_ScaleSPeedPosV)
{
	/*~~~~~~~~~~*/
	float	fCalc;
	/*~~~~~~~~~~*/

	IFAL(fCalc) = IFAL(MAT_C_f_8Bits);
	IFAL(fCalc) |= (ul_ScaleSPeedPosU & 0x00010000) >> 9;
	IFAL(fCalc) |= (ul_ScaleSPeedPosU & 0x00000001) << 6;
	IFAL(fCalc) |= (ul_ScaleSPeedPosV & 0x00010000) >> 11;
	IFAL(fCalc) |= (ul_ScaleSPeedPosV & 0x00000001) << 4;
	return(fCalc - MAT_C_f_8Bits);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ ULONG MAT_IsRotate(ULONG ul_ScaleSPeedPosU, ULONG ul_ScaleSPeedPosV)
{
	return (ul_ScaleSPeedPosU | ul_ScaleSPeedPosV | 0x00010001);
}

ULONG MAT_GetRotation_INT(ULONG ul_ScaleSPeedPosU, ULONG ul_ScaleSPeedPosV)
{
	/*~~~~~~~~~~*/
	ULONG	Value;
	/*~~~~~~~~~~*/

	Value = (ul_ScaleSPeedPosU & 0x00010000) >> 9;
	Value |= (ul_ScaleSPeedPosU & 0x00000001) << 6;
	Value |= (ul_ScaleSPeedPosV & 0x00010000) >> 11;
	Value |= (ul_ScaleSPeedPosV & 0x00000001) << 4;
	return((Value >> 4) & 0xf);
}

/*
 =======================================================================================================================
    -> SCALE is between -16..+16 it is coede in the 9 first bit of ScaleSPeedPos with 1 bit of sign, 4 bit integer, 4
    bits float.
 =======================================================================================================================
 */
float MAT_GetScale(ULONG ul_ScaleSPeedPosU)
{
	/*~~~~~~~~~~*/
	float	fCalc;
	/*~~~~~~~~~~*/

#ifndef PSX2_TARGET
	if(ul_ScaleSPeedPosU == 0) 
		return 1.0f;

	if(ul_ScaleSPeedPosU == 0x08001000) 
		return 1.0f;	/* Keep old identity value compatible */
#endif
		
	IFAL(fCalc) = ul_ScaleSPeedPosU;
	IFAL(fCalc) &= (MAT_Cc_ScaleMask | MAT_Cc_ScaleSign);
	return fCalc;
}
/*
 =======================================================================================================================
    -> SCALE is between -16..+16 it is coede in the 9 first bit of ScaleSPeedPos with 1 bit of sign, 4 bit integer, 4
    bits float.
 =======================================================================================================================
 */
_inline_ ULONG MAT_GetScale_INT(ULONG ul_ScaleSPeedPosU)
{
	return 	ul_ScaleSPeedPosU & (MAT_Cc_ScaleMask | MAT_Cc_ScaleSign);
}
/*
 =======================================================================================================================
    -> SPEED is between -512..+512 (in percentage per SECONDE (%/s)) it is coede in the 15 first bit of ScaleSPeedPos
    with 1 bit of sign, 10 bit integer, 4 bits float.
 =======================================================================================================================
 */
#ifndef PSX2_TARGET
float MAT_GetSpeed(ULONG ul_ScaleSPeedPosU)
{
	/*~~~~~~~~~~*/
	float	fCalc;
	/*~~~~~~~~~~*/
	if((ul_ScaleSPeedPosU) == 0) 
		return 0.0f;

	if(ul_ScaleSPeedPosU == 0x08001000) 
		return 0.0f;	/* Keep old identity value compatible */

	IFAL(fCalc) = ul_ScaleSPeedPosU << (LONG) 16;
	return fCalc;
}
#define MAT_GetPos MAT_GetSpeed
#define MAT_GetPos_i MAT_GetSpeed
#else
float MAT_GetSpeed(ULONG ul_ScaleSPeedPosU)
{
	register float	fCalc;
	fCalc = ps2_i2f_Special(ul_ScaleSPeedPosU << 16);
	return fCalc;
}
_inline_ float MAT_GetSpeed_i(ULONG ul_ScaleSPeedPosU)
{
	register float	fCalc;
	fCalc = ps2_i2f_Special(ul_ScaleSPeedPosU << 16);
	return fCalc;
}
#define MAT_GetPos MAT_GetSpeed
#define MAT_GetPos_i MAT_GetSpeed_i
#endif
/*
 =======================================================================================================================
    -> POS is between 0,0..1,0 (in percentage) it is coded between 13 to 24 bits (8 bits)of ScaleSPeedPos
 =======================================================================================================================
 */
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
float NotRunningTimer = 0;
#endif
void MAT_VUDecompressSpeed(MAT_tdst_MTLevel *p_tdstCompressedUV, MAT_tdst_Decompressed_UVMatrix *p_tdstDecompressedUV)
{	

#ifdef ACTIVE_EDITORS
#define TIM_MAIN_CLOCK_4TS NotRunningTimer
#ifdef JADEFUSION
	extern BOOL ENG_gb_EngineRunning;
#else
	extern ULONG ENG_gb_EngineRunning;
#endif
	if (ENG_gb_EngineRunning) 
		NotRunningTimer = TIM_gf_MainClock;
#else
	extern float TIM_gf_MainClockForTextureScrolling;
#define TIM_MAIN_CLOCK_4TS TIM_gf_MainClockForTextureScrolling
#endif

		/* U */
		if(p_tdstCompressedUV->ul_Flags & MAT_Cc_Flag_UDynamicTransEnable)
		{
			p_tdstDecompressedUV->AddU = TIM_MAIN_CLOCK_4TS * p_tdstDecompressedUV->AddU;
			p_tdstDecompressedUV->AddU += MAT_C_f_10Bits;
			IFAL(p_tdstDecompressedUV->AddU) = IFAL(MAT_C_f_10Bits) | (IFAL(p_tdstDecompressedUV->AddU) & 0x3ff);
			p_tdstDecompressedUV->AddU -= MAT_C_f_10Bits;
		}

		/* V */
		if(p_tdstCompressedUV->ul_Flags & MAT_Cc_Flag_VDynamicTransEnable)
		{
			p_tdstDecompressedUV->AddV = TIM_MAIN_CLOCK_4TS * p_tdstDecompressedUV->AddV;
			p_tdstDecompressedUV->AddV += MAT_C_f_10Bits;
			IFAL(p_tdstDecompressedUV->AddV) = IFAL(MAT_C_f_10Bits) | (IFAL(p_tdstDecompressedUV->AddV) & 0x3ff);
			p_tdstDecompressedUV->AddV -= MAT_C_f_10Bits;
		}
}
 

/*
 =======================================================================================================================
    This function wiil return 1 if compressedUV is != of identiuty
 =======================================================================================================================
 */
BOOL MAT_VUISIdentity(MAT_tdst_MTLevel *p_tdstLevel)
{
#ifdef PSX2_TARGET
	if (p_tdstLevel->ScaleSPeedPosU == 0) return TRUE;
	return FALSE;
#else
	if((p_tdstLevel->ScaleSPeedPosU == MAT_Cc_Identity) && (p_tdstLevel->ScaleSPeedPosV == MAT_Cc_Identity))
		return TRUE;
	if((p_tdstLevel->ScaleSPeedPosU == 0) || (p_tdstLevel->ScaleSPeedPosV == 0)) return TRUE;
	return FALSE;
#endif	
}
#ifndef PSX2_TARGET
#ifdef JADEFUSION
BOOL MAT_VUISIdentity_i(MAT_tdst_MTLevel *p_tdstLevel)
#else
_inline_ BOOL MAT_VUISIdentity_i(MAT_tdst_MTLevel *p_tdstLevel)
#endif
{
#ifdef PSX2_TARGET
	if (p_tdstLevel->ScaleSPeedPosU == 0) return TRUE;
	return FALSE;
#else
	if((p_tdstLevel->ScaleSPeedPosU == MAT_Cc_Identity) && (p_tdstLevel->ScaleSPeedPosV == MAT_Cc_Identity))
		return TRUE;
	if((p_tdstLevel->ScaleSPeedPosU == 0) || (p_tdstLevel->ScaleSPeedPosV == 0)) return TRUE;
	return FALSE;
#endif	
}
#endif
void MAT_ComputeRotationMatrix(MAT_tdst_MTLevel *p_tdstCompressedUV , MAT_tdst_Decompressed_UVMatrix *p_tdstDecompressedUV)
{
	float fMatSin,fMatCos;
	float	UVMatrix[4];

	ULONG Alpha;
	Alpha = MAT_GetRotation_INT(p_tdstCompressedUV->ScaleSPeedPosU, p_tdstCompressedUV->ScaleSPeedPosV);
	fMatSin = MAT_Sin(Alpha);
	fMatCos = MAT_Cos(Alpha);
	*(ULONG *)&UVMatrix[0] = *(ULONG *)&p_tdstDecompressedUV->UVMatrix[0];
	*(ULONG *)&UVMatrix[1] = *(ULONG *)&p_tdstDecompressedUV->UVMatrix[1];
	*(ULONG *)&UVMatrix[2] = *(ULONG *)&p_tdstDecompressedUV->UVMatrix[2];
	*(ULONG *)&UVMatrix[3] = *(ULONG *)&p_tdstDecompressedUV->UVMatrix[3];
	p_tdstDecompressedUV->UVMatrix[0] = fMatCos * UVMatrix[0] - fMatSin * UVMatrix[2];
	p_tdstDecompressedUV->UVMatrix[1] = fMatCos * UVMatrix[1] - fMatSin * UVMatrix[3];
	p_tdstDecompressedUV->UVMatrix[2] = fMatSin * UVMatrix[0] + fMatCos * UVMatrix[2];
	p_tdstDecompressedUV->UVMatrix[3] = fMatSin * UVMatrix[1] + fMatCos * UVMatrix[3];
}

void MAT_VUDecompress(MAT_tdst_MTLevel *p_tdstCompressedUV, MAT_tdst_Decompressed_UVMatrix *p_tdstDecompressedUV)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
//	register ULONG	Alpha;
	register ULONG	*pMatrix;
	MAT_tdst_MultiTexture	*pst_MLTTX;
	MAT_tdst_MTLevel		*pst_MLTTXLVL;
	extern int NumberMat;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pMatrix = (ULONG *)p_tdstDecompressedUV->UVMatrix;
	pMatrix[0] = MAT_GetScale_INT(p_tdstCompressedUV->ScaleSPeedPosU);//*MAT_Sin(popo)/10;
	pMatrix[3] = MAT_GetScale_INT(p_tdstCompressedUV->ScaleSPeedPosV);//*MAT_Cos(popo)/10;
	pMatrix[1] = pMatrix[2] = 0;

	/* U */
	p_tdstDecompressedUV->AddU = MAT_GetPos_i(p_tdstCompressedUV->ScaleSPeedPosU) * p_tdstDecompressedUV->UVMatrix[0];
	//p_tdstDecompressedUV->AddU=0+MAT_Sin(popo)/10;
	/* V */
	p_tdstDecompressedUV->AddV = MAT_GetPos_i(p_tdstCompressedUV->ScaleSPeedPosV) * p_tdstDecompressedUV->UVMatrix[3];
	//p_tdstDecompressedUV->AddV=0+MAT_Cos(popo)/10;

	if(p_tdstCompressedUV->ul_Flags & (MAT_Cc_Flag_UDynamicTransEnable|MAT_Cc_Flag_VDynamicTransEnable)) MAT_VUDecompressSpeed(p_tdstCompressedUV, p_tdstDecompressedUV);

	/* Rotation */
	if (MAT_IsRotate(p_tdstCompressedUV->ScaleSPeedPosU, p_tdstCompressedUV->ScaleSPeedPosV)) MAT_ComputeRotationMatrix( p_tdstCompressedUV , p_tdstDecompressedUV);

	//GDI_gpst_CurDD_SPR.pst_ComputingBuffers->Current = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ast_UV;
	if ( UVWave )//&& NumberMat+1 == LevelUVWave )
	{
		MDF_tdst_Modifier		*pst_Modifier;
		OBJ_tdst_GameObject					*_pst_GO;
		UVTexWave_tdst_Modifier *p_UVTexWave;

	#ifdef ACTIVE_EDITORS
#define TIM_MAIN_CLOCK_4TS NotRunningTimer
#ifdef JADEFUSION
	extern BOOL ENG_gb_EngineRunning;
#else
	extern ULONG ENG_gb_EngineRunning;
#endif
	if (ENG_gb_EngineRunning) 
		NotRunningTimer = TIM_gf_MainClock;
#else
	extern float TIM_gf_MainClockForTextureScrolling;
#define TIM_MAIN_CLOCK_4TS TIM_gf_MainClockForTextureScrolling
#endif	
		_pst_GO = GDI_gpst_CurDD_SPR.pst_CurrentGameObject;
		pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;

		while (pst_Modifier)
		{	
			if ( pst_Modifier->i->ul_Type == MDF_C_Modifier_UVTexWave)
			{
				p_UVTexWave = (UVTexWave_tdst_Modifier*) pst_Modifier->p_Data;
				if (p_UVTexWave->MatLevel == NumberMat+1)
				{		
					p_tdstDecompressedUV->AddU = fSin(TIM_MAIN_CLOCK_4TS * p_UVTexWave->f_SpeedCoefU) * p_UVTexWave->f_AngleU;
					p_tdstDecompressedUV->AddU += MAT_C_f_10Bits;
					IFAL(p_tdstDecompressedUV->AddU) = IFAL(MAT_C_f_10Bits) | (IFAL(p_tdstDecompressedUV->AddU) & 0x3ff);
					p_tdstDecompressedUV->AddU -= MAT_C_f_10Bits;	
				
					p_tdstDecompressedUV->AddV = fCos(TIM_MAIN_CLOCK_4TS * p_UVTexWave->f_SpeedCoefV) * p_UVTexWave->f_AngleV;
					p_tdstDecompressedUV->AddV += MAT_C_f_10Bits;
					IFAL(p_tdstDecompressedUV->AddV) = IFAL(MAT_C_f_10Bits) | (IFAL(p_tdstDecompressedUV->AddV) & 0x3ff);
					p_tdstDecompressedUV->AddV -= MAT_C_f_10Bits;
				}
			}
			pst_Modifier = pst_Modifier->pst_Next;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
void MAT_UVMatrix_Invert( MAT_tdst_Decompressed_UVMatrix *_pst_Matrix, MAT_tdst_Decompressed_UVMatrix *_pst_Inv )
{
	float f, g;

	/* check */
	if (!_pst_Matrix || ! _pst_Inv) return;
	
	/* set inv to identity */
	L_memset( _pst_Inv, 0, sizeof( MAT_tdst_Decompressed_UVMatrix ) );
	_pst_Inv->UVMatrix[ 0 ] = _pst_Inv->UVMatrix[ 3 ] = 1.0f;

	/* compute determinant */
	f = _pst_Matrix->UVMatrix[0] * _pst_Matrix->UVMatrix[3];
	f -= _pst_Matrix->UVMatrix[2] * _pst_Matrix->UVMatrix[1];
	/* inversion impossible */
	if(f == 0) return;
	
	f = fInv(f);
	_pst_Inv->UVMatrix[0] = _pst_Matrix->UVMatrix[3] * f;
	_pst_Inv->UVMatrix[1] = -_pst_Matrix->UVMatrix[1] * f;
	_pst_Inv->UVMatrix[2] = -_pst_Matrix->UVMatrix[2] * f;
	_pst_Inv->UVMatrix[3] = _pst_Matrix->UVMatrix[0] * f;
		
	g = _pst_Matrix->UVMatrix[2] * _pst_Matrix->AddV - _pst_Matrix->UVMatrix[3] * _pst_Matrix->AddU;
	_pst_Inv->AddU = g * f;
	g = _pst_Matrix->UVMatrix[1] * _pst_Matrix->AddU - _pst_Matrix->UVMatrix[0] * _pst_Matrix->AddV;
	_pst_Inv->AddV = g * f;
}
#endif

void MAT_UV_Compute_OBJ1(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)

		SOFT_tdst_UV		*SrcUV;
		unsigned short		*UsdIndx, *LastUsdIndx;
		if(GEO_MRM_ul_IsMrmObject(pst_Obj))
		{
			pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
			pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
			if(!pst_Obj->dst_UV) return;
			SrcUV = ((SOFT_tdst_UV *) pst_Obj->dst_UV) + pst_Element->p_MrmElementAdditionalInfo->ul_One_UV_Per_Point_Per_Element_Base;
			UsdIndx = pst_Element->pus_ListOfUsedIndex;
			LastUsdIndx = UsdIndx + pst_Element->ul_NumberOfUsedIndex;
			while(UsdIndx < LastUsdIndx) pst_CurDD->pst_ComputingBuffers->Current[*(UsdIndx++)] = *(SrcUV++);
		}
		else
		{
			pst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UseOneUVPerPoint;
			pst_CurDD->pst_ComputingBuffers->Current = (SOFT_tdst_UV *) pst_Obj->dst_UV;
		}
#endif		//#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)
}
void MAT_UV_Compute_OBJ2(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)

		SOFT_tdst_UV		*SrcUV;
		unsigned short		*UsdIndx, *LastUsdIndx;
		if(!pst_Obj->dst_UV) return;
		if(GEO_MRM_ul_IsMrmObject(pst_Obj))
		{
			pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
			pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
			SrcUV = ((SOFT_tdst_UV *) pst_Obj->dst_UV) + pst_Element->p_MrmElementAdditionalInfo->ul_One_UV_Per_Point_Per_Element_Base;
			UsdIndx = pst_Element->pus_ListOfUsedIndex;
			LastUsdIndx = UsdIndx + pst_Element->ul_NumberOfUsedIndex;
			while(UsdIndx < LastUsdIndx) pst_CurDD->pst_ComputingBuffers->Current[*(UsdIndx++)] = *(SrcUV++);
		}
		else
		{
			if (pst_Obj->l_NbUVs >= pst_Obj->l_NbPoints)
				pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
			else
				pst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UseOneUVPerPoint;
			pst_CurDD->pst_ComputingBuffers->Current = (SOFT_tdst_UV *) pst_Obj->dst_UV;
		}
		
#endif //#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)		
}
void MAT_UV_Compute_Previous(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)

	if(pst_CurDD->pst_ComputingBuffers->Current != NULL)	
        return;
	MAT_UV_Compute_OBJ1(pst_CurDD,pst_Obj,pst_Element,CurrentAddFlag,StoreUV);
	
#endif // #if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)	
}

void MAT_UV_Compute_Default(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
	pst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UseOneUVPerPoint;
	pst_CurDD->pst_ComputingBuffers->Current = (SOFT_tdst_UV *) pst_Obj->dst_UV;
}
#ifdef PSX2_TARGET
/*
	 p_NormaleBase & NUM must be aligned on 4
*/
void MAT_PS2_AsmCChromeUV(MATH_tdst_Vector *p_NormaleBase,SOFT_tdst_UV *pDstUV,ULONG NUM,SOFT_tdst_AVertex *pCX,SOFT_tdst_AVertex *pCY)
{
#if 0
		while(NUM--)
		{
			pDstUV->u = 0.5f - pCX->x * p_NormaleBase->x - pCX->y * p_NormaleBase->y - pCX->z * p_NormaleBase->z;
			pDstUV->v = 0.5f - pCY->x * p_NormaleBase->x - pCY->y * p_NormaleBase->y - pCY->z * p_NormaleBase->z;
			p_NormaleBase++;
			pDstUV++;
		}
#else
	/* PARAMS : 
		a0 = p_NormaleBase 
		a1 = pDstUV
		A2 = NUM
		a3 = pCX
		t0 = pCY
	*/
	asm ("
		.set noreorder // Treat 4 by 4!

				beq 			a2,zero,S4LOOP_END
				addiu			a2 , a2 , 3
				addiu			a1 , a1 , -32
				lqc2			$vf01 , 0(a3)
				lqc2			$vf02 , 0(t0)
				vsub.xyz		$vf01 , $vf00 , $vf01
				vsub.xyz		$vf02 , $vf00 , $vf02
				srl				a2 , a2 , 2			// Align on 4
S4LOOP: 		lq				t0,0(a0)			/* t0 =  x1  z0   y0  x0  */
				lq				t1,16(a0)			/* t1 =  y2  x2   z1  y1  */
				lq				t2,32(a0)			/* t2 =  z3  y3   x3  z2  */
				prot3w			t3,t0				/* t3 = [x1  x0]  z0  y0  */
				prot3w			t4,t2				/* t4 =  z3  z2   y3  x3  */
				pextlw			t5,t1,t3			/* t5 = [z1  z0] [y1  y0] */
				pcpyud			t6,t1,t1			/* t6 =  y2  x2   y2  x2  */
				pextlw			t7,t4,t6			/* t7 = [y3  y2] [x3  x2] */
				pcpyud  		t2,t5,t4    		/* t2 = [z3  z2   z1  z0] */
				pcpyud  		t3,t3,t3			/* t3 = [x1  x0] [x1  x0] */
				pcpyld  		t5,t5,t5			/* t5 = [y1  y0] [y1  y0] */
				pcpyud  		t1,t5,t7			/* t1 = [y3  y2] [y1  y0] */
				pcpyld  		t0,t7,t3			/* t0 = [x3  x2] [x1  x0] */
				qmtc2   		t2,$vf12			/* Z */
				qmtc2   		t0,$vf10			/* X */
				qmtc2   		t1,$vf11    		/* Y */
				vmulax.xyzw 	ACC,$vf10,$vf01x
				addiu			a2 , a2 , -1
				vmadday.xyzw 	ACC,$vf11,$vf01y
				addiu			a0 , a0 , 48
				vmaddz.xyzw 	$vf20,$vf12,$vf01z
				addiu			a1 , a1 , 32
				vmulax.xyzw 	ACC,$vf10,$vf02x
				vmadday.xyzw 	ACC,$vf11,$vf02y
				vmaddz.xyzw 	$vf21,$vf12,$vf02z
				vaddw.xyzw		$vf20,$vf20,$vf01w
				vaddw.xyzw		$vf21,$vf21,$vf01w
				qmfc2.i 		t4,$vf20
				qmfc2   		t5,$vf21
				pextlw			t0,t5,t4
				pextuw			t1,t5,t4
				sq				t0,0(a1)
				bne 			a2,zero,S4LOOP
				sq				t1,16(a1)

S4LOOP_END:
		.set reorder
	");
#endif		
}
#endif

void MAT_UV_Compute_Chrome(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)

		MATH_tdst_Vector	VDst, *VNormal, *VNormalLast;
		SOFT_tdst_AVertex	CX ONLY_PSX2_ALIGNED(16);
		SOFT_tdst_AVertex	CY ONLY_PSX2_ALIGNED(16);
		SOFT_tdst_UV		*DstUV;
		unsigned short		*UsdIndx, *LastUsdIndx;
		CX .x = pst_CurDD->st_MatrixStack.pst_CurrentMatrix->Ix * 0.5f;
		CX .y = pst_CurDD->st_MatrixStack.pst_CurrentMatrix->Jx * 0.5f;
		CX .z = pst_CurDD->st_MatrixStack.pst_CurrentMatrix->Kx * 0.5f;
		CY .x = pst_CurDD->st_MatrixStack.pst_CurrentMatrix->Iy * 0.5f;
		CY .y = pst_CurDD->st_MatrixStack.pst_CurrentMatrix->Jy * 0.5f;
		CY .z = pst_CurDD->st_MatrixStack.pst_CurrentMatrix->Ky * 0.5f;
		if (!(pst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Symetric))
		{
			CX .x = -CX .x;
			CY .x = -CY .x;
		}
		CX.w = 0.5f;
		
		if(pst_Element -> pus_ListOfUsedIndex)
		{
#ifdef PSX2_TARGET
#define UsedIndexDiscPo2 2
			{
				LONG IndexCounter;
				LONG LastComputed;
				LONG ToCompute;
				GEO_UseNormals(pst_Obj);
				VNormal = pst_Obj->dst_PointNormal;
				DstUV = pst_CurDD->pst_ComputingBuffers->ast_UV;
				UsdIndx = pst_Element->pus_ListOfUsedIndex;
				IndexCounter = pst_Element->ul_NumberOfUsedIndex;
				LastComputed = 0;
				VNormal 		-= 1 << UsedIndexDiscPo2;
				DstUV 			-= 1 << UsedIndexDiscPo2;
				LastComputed 	-= 1 << UsedIndexDiscPo2;
				while (IndexCounter--)
				{
					ToCompute = ((LONG)*UsdIndx) & (0xffffffff << UsedIndexDiscPo2);
					if (LastComputed != ToCompute)
					{
					    LastComputed 	= ToCompute - LastComputed;
						VNormal 		+= LastComputed;
						DstUV 			+= LastComputed;
						LastComputed 	= ToCompute;
						MAT_PS2_AsmCChromeUV(VNormal,DstUV,1 << UsedIndexDiscPo2,&CX,&CY);
					}
					UsdIndx++;
				}
			}
#else			
			UsdIndx = pst_Element->pus_ListOfUsedIndex;
			LastUsdIndx = UsdIndx + pst_Element->ul_NumberOfUsedIndex;
			while(UsdIndx < LastUsdIndx)
			{
				GEO_UseNormals(pst_Obj);
				VNormal = pst_Obj->dst_PointNormal + *UsdIndx;
				DstUV = pst_CurDD->pst_ComputingBuffers->ast_UV + *UsdIndx;
				VDst.x = CX.x * VNormal->x + CX.y * VNormal->y + CX.z * VNormal->z;
				VDst.y = CY.x * VNormal->x + CY.y * VNormal->y + CY.z * VNormal->z;
				DstUV->u = 0.5f - VDst.x;
				DstUV->v = 0.5f - VDst.y;
				UsdIndx++;
			}
#endif			
		}
		else
		{
#ifdef PSX2_TARGET		
			GEO_UseNormals(pst_Obj);
			MAT_PS2_AsmCChromeUV(pst_Obj->dst_PointNormal,
							pst_CurDD->pst_ComputingBuffers->ast_UV,
							pst_Obj->l_NbPoints,&CX,&CY);
#else			
			GEO_UseNormals(pst_Obj);
			VNormal = pst_Obj->dst_PointNormal;
			VNormalLast = VNormal + pst_Obj->l_NbPoints;
			DstUV = pst_CurDD->pst_ComputingBuffers->ast_UV;
			while(VNormal < VNormalLast)
			{
				VDst.x = CX.x * VNormal->x + CX.y * VNormal->y + CX.z * VNormal->z;
				VDst.y = CY.x * VNormal->x + CY.y * VNormal->y + CY.z * VNormal->z;
				DstUV->u = 0.5f - VDst.x;
				DstUV->v = 0.5f - VDst.y;
				DstUV++;
				VNormal++;
			}
#endif
		}

		pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
		pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
		
#endif //#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)		
}

void MAT_UV_Compute_PLANAR_GIZMO(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)

		MATH_tdst_Matrix	Matrix ONLY_PSX2_ALIGNED(16);
		MATH_tdst_Vector	CamPos;
		GEO_Vertex	        *VSrc, *VSrcLast;
		SOFT_tdst_UV		*DstUV;
		VSrc = pst_CurDD->p_Current_Vertex_List;
		VSrcLast = VSrc + pst_Obj->l_NbPoints;
		DstUV = pst_CurDD->pst_ComputingBuffers->ast_UV;
		pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
		pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
		if(MAT_GET_MatrixFrom(CurrentAddFlag) == MAT_CC_OBJECT)
		{
			switch(MAT_GET_XYZ(CurrentAddFlag))
			{
			case MAT_CC_X:
#ifdef PSX2_TARGET
			if (!StoreUV)
				Gsp_SetPlanarProjectionMode(1) /* 0: Nothing , 1: OX , 2: OY , 3:OZ , 4:WP */;
			else
#endif			
				while(VSrc < VSrcLast)
				{
					DstUV->u = VSrc->y;
					(DstUV++)->v = (VSrc++)->z;
				}
				return;
			case MAT_CC_Y:
#ifdef PSX2_TARGET
			if (!StoreUV)
				Gsp_SetPlanarProjectionMode(2) /* 0: Nothing , 1: OX , 2: OY , 3:OZ , 4:WP */;
			else
#endif				
				while(VSrc < VSrcLast)
				{
					DstUV->u = VSrc->x;
					(DstUV++)->v = (VSrc++)->z;
				}
				return;
			case MAT_CC_Z:
#ifdef PSX2_TARGET
			if (!StoreUV)
				Gsp_SetPlanarProjectionMode(3) /* 0: Nothing , 1: OX , 2: OY , 3:OZ , 4:WP */;
			else
#endif				
				while(VSrc < VSrcLast)
				{
					DstUV->u = VSrc->x;
					(DstUV++)->v = (VSrc++)->y;
				}
				return;
			case MAT_CC_XYZ:
				{
					MATH_tdst_Vector stU,stV;
					MATH_InitVector(&stU , Cf_Sqrt2 * 0.5f , -Cf_Sqrt2 * 0.5f , 0.0f);
					MATH_InitVector(&stV , -Cf_InvSqrt3 , -Cf_InvSqrt3 , Cf_InvSqrt3);
#ifdef PSX2_TARGET
					if (!StoreUV)
						Gsp_SetPlanarProjectionUVMatrix(&stU,&stV,NULL);
					else
#endif				
						while(VSrc < VSrcLast)
						{
							DstUV->u = MATH_f_DotProduct(VCast(VSrc) , &stU);
							(DstUV++)->v = MATH_f_DotProduct(VCast(VSrc++), &stV);
						}
				}
				return;
			}
			return;
		}
		else
		/* if (MAT_GET_MatrixFrom(pst_MLTTXLVL->s_AditionalFlags) == MAT_CC_WORLD) */
		{
			if ((MAT_GET_MatrixFrom(CurrentAddFlag) == MAT_CC_GIZMO) &&	
				(pst_CurDD->pst_CurrentGameObject->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix))
			{
				/*~~~~~~~~~~~~~*/
				LONG	GizmoNum;
				/*~~~~~~~~~~~~~*/

				GizmoNum = MAT_GET_GizmoNumber(CurrentAddFlag);
				if(pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->l_Number > GizmoNum)
				{
					if (pst_CurDD->pst_CurrentGameObject->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
					{
						if(pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_GO)
						{
							if((!(pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)) || (pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer))
							{
								MATH_tdst_Matrix	Matrix2;
								MATH_InvertMatrix	(&Matrix, pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_GO->pst_GlobalMatrix );
								MATH_MulMatrixMatrix(&Matrix2 ,pst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix  , &Matrix );
								MATH_MakeOGLMatrix	(&Matrix, &Matrix2);
							}
							else
							{
								if(pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_Matrix)
								{
									MATH_InvertMatrix(&Matrix, pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_Matrix);
									MATH_MakeOGLMatrix(&Matrix, &Matrix);
								}
								else
								{
									MATH_InvertMatrix(&Matrix, &MATH_gst_IdentityMatrix);
									MATH_MakeOGLMatrix(&Matrix, &Matrix);
								}
							}
						}
						else
						{
							MATH_InvertMatrix(&Matrix, &MATH_gst_IdentityMatrix);
							MATH_MakeOGLMatrix(&Matrix, &Matrix);
						}
						CamPos = Matrix.T;
					}
					else
					{
						MATH_InvertMatrix(&Matrix,&pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_Gizmo[GizmoNum].st_Matrix);
						MATH_MakeOGLMatrix(&Matrix, &Matrix);
						CamPos = Matrix.T;
					}
				}
				else
				{
					MATH_MakeOGLMatrix(&Matrix, pst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix);
					CamPos = Matrix.T;
				}
			}
			else
			{
				if (MAT_GET_MatrixFrom(CurrentAddFlag) == MAT_CC_CAMERA)
				{
					float Coef;
					/* Camera system */
					MATH_MakeOGLMatrix(&Matrix, pst_CurDD->st_MatrixStack.pst_CurrentMatrix);
					Coef = pst_CurDD->st_Camera.f_FactorX / pst_CurDD->st_Camera.f_CenterX;
					MATH_ScaleEqualVector(MATH_pst_GetXAxis(&Matrix),Coef);
					MATH_ScaleEqualVector(MATH_pst_GetYAxis(&Matrix),Coef);
					MATH_ScaleEqualVector(MATH_pst_GetZAxis(&Matrix),Coef);

					CamPos = Matrix.T;
					MATH_ScaleEqualVector(&CamPos,Coef);
				} else
				{
					/* World system */
					MATH_MakeOGLMatrix(&Matrix, pst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix);
					CamPos = Matrix.T;
				} 
				if (!StoreUV)
				{
					VSrc = pst_CurDD->p_Current_Vertex_List;
					VSrcLast = VSrc + pst_Obj->l_NbPoints;
				}
			}

			switch(MAT_GET_XYZ(CurrentAddFlag))
			{
			case MAT_CC_X:
#ifdef PSX2_TARGET
			if (!StoreUV)
				{
					MATH_tdst_Vector VU,VV,VT ;
					MATH_InitVector(&VU , Matrix.Iy , Matrix.Jy , Matrix.Ky);
					MATH_InitVector(&VV , Matrix.Iz , Matrix.Jz , Matrix.Kz);
					MATH_InitVector(&VT , CamPos.y , CamPos.z , 0.0f);
					Gsp_SetPlanarProjectionUVMatrix(&VU,&VV,&VT);					
				}
			else
#endif			
				while(VSrc < VSrcLast)
				{
					DstUV->u = VSrc->x * Matrix.Iy + VSrc->y * Matrix.Jy + VSrc->z * Matrix.Ky + CamPos.y;
					(DstUV++)->v = VSrc->x * Matrix.Iz + VSrc->y * Matrix.Jz + VSrc->z * Matrix.Kz + CamPos.z;
					VSrc++;
				}
				break;
			case MAT_CC_Y:
#ifdef PSX2_TARGET
				if (!StoreUV)
				{
					MATH_tdst_Vector VU,VV,VT ;
					MATH_InitVector(&VU , Matrix.Ix , Matrix.Jx , Matrix.Kx);
					MATH_InitVector(&VV , Matrix.Iz , Matrix.Jz , Matrix.Kz);
					MATH_InitVector(&VT , CamPos.x , CamPos.z , 0.0f);
					Gsp_SetPlanarProjectionUVMatrix(&VU,&VV,&VT);					
				}
				else
#endif				
				while(VSrc < VSrcLast)
				{
					DstUV->u = VSrc->x * Matrix.Ix + VSrc->y * Matrix.Jx + VSrc->z * Matrix.Kx + CamPos.x;
					(DstUV++)->v = VSrc->x * Matrix.Iz + VSrc->y * Matrix.Jz + VSrc->z * Matrix.Kz + CamPos.z;
					VSrc++;
				}
				break;
			case MAT_CC_Z:
#ifdef PSX2_TARGET
				if (!StoreUV)
				{
					MATH_tdst_Vector VU,VV,VT ;
					MATH_InitVector(&VU , Matrix.Ix , Matrix.Jx , Matrix.Kx);
					MATH_InitVector(&VV , Matrix.Iy , Matrix.Jy , Matrix.Ky);
					MATH_InitVector(&VT , CamPos.x , CamPos.y , 0.0f);
					Gsp_SetPlanarProjectionUVMatrix(&VU,&VV,&VT);
				}
				else
#endif			
				while(VSrc < VSrcLast)
				{
					DstUV->u = VSrc->x * Matrix.Ix + VSrc->y * Matrix.Jx + VSrc->z * Matrix.Kx + CamPos.x;
					(DstUV++)->v = VSrc->x * Matrix.Iy + VSrc->y * Matrix.Jy + VSrc->z * Matrix.Ky + CamPos.y;
					VSrc++;
				}
				break;
			case MAT_CC_XYZ:
				{
					MATH_tdst_Vector stU,stV,stUC,stVC ;
					MATH_tdst_Vector A;
					MATH_InitVector(&stUC , Cf_Sqrt2 * 0.5f , -Cf_Sqrt2 * 0.5f , 0.0f);
					MATH_InitVector(&stVC , -Cf_InvSqrt3 , -Cf_InvSqrt3 , Cf_InvSqrt3);
					A.x = MATH_f_DotProduct(&stUC , &CamPos);
					A.y = MATH_f_DotProduct(&stVC , &CamPos);
					MATH_TranspEq33Matrix(&Matrix);
					stU.x = Matrix.Ix * stUC.x + Matrix.Jx * stUC.y + Matrix.Kx * stUC.z ;
					stU.y = Matrix.Iy * stUC.x + Matrix.Jy * stUC.y + Matrix.Ky * stUC.z ;
					stU.z = Matrix.Iz * stUC.x + Matrix.Jz * stUC.y + Matrix.Kz * stUC.z ;
					stV.x = Matrix.Ix * stVC.x + Matrix.Jx * stVC.y + Matrix.Kx * stVC.z ;
					stV.y = Matrix.Iy * stVC.x + Matrix.Jy * stVC.y + Matrix.Ky * stVC.z ;
					stV.z = Matrix.Iz * stVC.x + Matrix.Jz * stVC.y + Matrix.Kz * stVC.z ;
#ifdef PSX2_TARGET
					if (!StoreUV)
						Gsp_SetPlanarProjectionUVMatrix(&stU,&stV,&A);					
					else
#endif					
						while(VSrc < VSrcLast)
						{
							DstUV->u = MATH_f_DotProduct(VCast(VSrc) , &stU) + A.x;
							(DstUV++)->v = MATH_f_DotProduct(VCast(VSrc) , &stV) + A.y;
							VSrc++;
						}
				}
				break;
			}
		}
		pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
		pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
		
#endif // #if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)		
}
void MAT_UV_COMPUTE_GetMAtrix(GDI_tdst_DisplayData	*pst_CurDD,OBJ_tdst_GameObject *_pst_GO , ULONG CurrentAddFlag , MATH_tdst_Matrix	*p_MatrixDSt)
{
	if ((MAT_GET_MatrixFrom(CurrentAddFlag) == MAT_CC_GIZMO) &&	(pst_CurDD->pst_CurrentGameObject->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix))
	{
		/*~~~~~~~~~~~~~*/
		LONG	GizmoNum;
		/*~~~~~~~~~~~~~*/
		GizmoNum = MAT_GET_GizmoNumber(CurrentAddFlag);
		if(_pst_GO->pst_Base->pst_AddMatrix->l_Number > GizmoNum)
		{
			if (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
			{
				if(_pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_GO)
				{
					if((!(pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)) || (pst_CurDD->pst_CurrentGameObject->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer))
					{
						MATH_tdst_Matrix	Matrix2;
						MATH_InvertMatrix	(p_MatrixDSt, _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_GO->pst_GlobalMatrix );
						MATH_MulMatrixMatrix(&Matrix2 ,_pst_GO->pst_GlobalMatrix  , p_MatrixDSt );
						MATH_MakeOGLMatrix	(p_MatrixDSt, &Matrix2);
					}
					else
					{
						if(_pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_Matrix)
						{
							MATH_InvertMatrix(p_MatrixDSt, _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[GizmoNum].pst_Matrix);
							MATH_MakeOGLMatrix(p_MatrixDSt, p_MatrixDSt);
						}
						else
						{
							MATH_MakeOGLMatrix(p_MatrixDSt, &MATH_gst_IdentityMatrix);
						}
					}
				}
				else
				{
					MATH_MakeOGLMatrix(p_MatrixDSt, &MATH_gst_IdentityMatrix);
				}
			}
			else
			{
				MATH_InvertMatrix(p_MatrixDSt,&_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[GizmoNum].st_Matrix);
				MATH_MakeOGLMatrix(p_MatrixDSt, p_MatrixDSt);
			}
		}
		else
		{
			MATH_MakeOGLMatrix(p_MatrixDSt, _pst_GO->pst_GlobalMatrix);
		}
	}
	else
	{
		if (MAT_GET_MatrixFrom(CurrentAddFlag) == MAT_CC_CAMERA)
		{
			float Coef;
			/* Camera system */
			MATH_MakeOGLMatrix(p_MatrixDSt, pst_CurDD->st_MatrixStack.pst_CurrentMatrix);
			Coef = pst_CurDD->st_Camera.f_FactorX / pst_CurDD->st_Camera.f_CenterX;
			MATH_ScaleEqualVector(MATH_pst_GetXAxis(p_MatrixDSt),Coef);
			MATH_ScaleEqualVector(MATH_pst_GetYAxis(p_MatrixDSt),Coef);
			MATH_ScaleEqualVector(MATH_pst_GetZAxis(p_MatrixDSt),Coef);
			MATH_ScaleEqualVector(&p_MatrixDSt->T,Coef);
		} else
		{
			/* World system */
			MATH_MakeOGLMatrix(p_MatrixDSt, _pst_GO->pst_GlobalMatrix);
		} 
	}
	MATH_TranspEq33Matrix(p_MatrixDSt);
}
#ifndef PSX2_TARGET
#define MAT_UV_Compute_PLANAR_GIZMO_NOSTORE MAT_UV_Compute_PLANAR_GIZMO
#else
void MAT_UV_Compute_PLANAR_GIZMO_NOSTORE(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
		MATH_tdst_Matrix	Matrix ONLY_PSX2_ALIGNED(16);
		ULONG 				GetXYZ;
		Gsp_SetPlanarProjectionMode(1);
		if (StoreUV)
		{
			MAT_UV_Compute_PLANAR_GIZMO(pst_CurDD,pst_Obj,pst_Element, CurrentAddFlag ,  StoreUV);
			return;
		}
		pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
		GetXYZ = MAT_GET_XYZ(CurrentAddFlag);
		if(MAT_GET_MatrixFrom(CurrentAddFlag) == MAT_CC_OBJECT)
		{
			if (GetXYZ != MAT_CC_XYZ)
				Gsp_SetPlanarProjectionMode(GetXYZ + 1);
			else
			{
				MATH_tdst_Vector stU,stV;
				MATH_InitVector(&stU , Cf_Sqrt2 * 0.5f , -Cf_Sqrt2 * 0.5f , 0.0f);
				MATH_InitVector(&stV , -Cf_InvSqrt3 , -Cf_InvSqrt3 , Cf_InvSqrt3);
				Gsp_SetPlanarProjectionUVMatrix(&stU,&stV,NULL);
			}
		}
		else
		{
			MAT_UV_COMPUTE_GetMAtrix(pst_CurDD , pst_CurDD->pst_CurrentGameObject ,  CurrentAddFlag , &Matrix);
			switch(GetXYZ)
			{
			case MAT_CC_X:
				{
					MATH_tdst_Vector VT ;
					MATH_InitVector(&VT , Matrix.T.y , Matrix.T.z , 0.0f);
					Gsp_SetPlanarProjectionUVMatrix(MATH_pst_GetYAxis(&Matrix),MATH_pst_GetZAxis(&Matrix),&VT);
				}
				break;
			case MAT_CC_Y:
				{
					MATH_tdst_Vector VT ;
					MATH_InitVector(&VT , Matrix.T.x , Matrix.T.z , 0.0f);
					Gsp_SetPlanarProjectionUVMatrix(MATH_pst_GetXAxis(&Matrix),MATH_pst_GetZAxis(&Matrix),&VT);					
				}
				break;
			case MAT_CC_Z:
				{
					MATH_tdst_Vector VT ;
					MATH_InitVector(&VT , Matrix.T.x , Matrix.T.y , 0.0f);
					Gsp_SetPlanarProjectionUVMatrix(MATH_pst_GetXAxis(&Matrix),MATH_pst_GetYAxis(&Matrix),&VT);
				}
				break;
			case MAT_CC_XYZ:
				{
					MATH_tdst_Vector stU,stV,stUC,stVC ;
					MATH_tdst_Vector A;
					MATH_InitVector(&stUC , Cf_Sqrt2 * 0.5f , -Cf_Sqrt2 * 0.5f , 0.0f);
					MATH_InitVector(&stVC , -Cf_InvSqrt3 , -Cf_InvSqrt3 , Cf_InvSqrt3);
					A.x = MATH_f_DotProduct(&stUC , &Matrix.T);
					A.y = MATH_f_DotProduct(&stVC , &Matrix.T);
					stU.x = Matrix.Ix * stUC.x + Matrix.Jx * stUC.y + Matrix.Kx * stUC.z ;
					stU.y = Matrix.Iy * stUC.x + Matrix.Jy * stUC.y + Matrix.Ky * stUC.z ;
					stU.z = Matrix.Iz * stUC.x + Matrix.Jz * stUC.y + Matrix.Kz * stUC.z ;
					stV.x = Matrix.Ix * stVC.x + Matrix.Jx * stVC.y + Matrix.Kx * stVC.z ;
					stV.y = Matrix.Iy * stVC.x + Matrix.Jy * stVC.y + Matrix.Ky * stVC.z ;
					stV.z = Matrix.Iz * stVC.x + Matrix.Jz * stVC.y + Matrix.Kz * stVC.z ;
					Gsp_SetPlanarProjectionUVMatrix(&stU,&stV,&A);
				}
				break;
			}
		}
}
#endif

void MAT_UV_Compute_FACE_MAP(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)

		pst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UseOneUVPerPoint;
		pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_FaceMap;
		pst_CurDD->pst_ComputingBuffers->ast_UV[0].u = 0.0f;
		pst_CurDD->pst_ComputingBuffers->ast_UV[0].v = 0.0f;
		pst_CurDD->pst_ComputingBuffers->ast_UV[1].u = 0.0f;
		pst_CurDD->pst_ComputingBuffers->ast_UV[1].v = 1.0f;
		pst_CurDD->pst_ComputingBuffers->ast_UV[2].u = 1.0f;
		pst_CurDD->pst_ComputingBuffers->ast_UV[2].v = 0.0f;
		pst_CurDD->pst_ComputingBuffers->ast_UV[3].u = 1.0f;
		pst_CurDD->pst_ComputingBuffers->ast_UV[3].v = 1.0f;
		pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;

#endif 		
}

void MAT_UV_Compute_FOGZZ(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)

	GEO_Vertex	        *VSrc, *VSrcLast;
	SOFT_tdst_UV		*DstUV;
	MATH_tdst_Matrix	Matrix ONLY_PSX2_ALIGNED(16);
	MATH_tdst_Vector	CamPos;
	float CamPosDistPlane , Norm;
	MATH_tdst_Vector stCamDir ;
	CamPos.x = CamPos.y = CamPos.z = 0.0f;
	stCamDir.x = stCamDir.y = 0.0f; 
	stCamDir.z = 1.0f;
	MATH_InvertMatrix(&Matrix, pst_CurDD->st_MatrixStack.pst_CurrentMatrix);
	MATH_TransformVertex(&CamPos, &Matrix, &CamPos);
	MATH_TransformVector(&stCamDir, &Matrix, &stCamDir);
	Norm = MATH_f_SqrNormVector(&stCamDir);
	Norm = fOptInvSqrt(Norm);
	Norm *= Norm;
	stCamDir.x *= Norm;
	stCamDir.y *= Norm;
	stCamDir.z *= Norm;
	CamPosDistPlane = MATH_f_DotProduct(&stCamDir , &CamPos);

	pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
	pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
	/* 
		This mapping is computed as folow:
			U is the distance to camera projected on the ground
			V is the World Z 
	*/
	MATH_MakeOGLMatrix(&Matrix, pst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix);

#ifdef PSX2_TARGET
	if (!StoreUV)
	{
		MATH_tdst_Vector VU,VV,VT ;
		MATH_InitVector(&VU , stCamDir.x , stCamDir.y , stCamDir.z);
		MATH_InitVector(&VV , Matrix.Iz , Matrix.Jz , Matrix.Kz);
		MATH_InitVector(&VT , -CamPosDistPlane , Matrix.T.z ,  0.0f);
		Gsp_SetPlanarProjectionUVMatrix(&VU,&VV,&VT);
	}
	else 
#endif	
	if(pst_Element -> pus_ListOfUsedIndex)
	{
		unsigned short		*UsdIndx, *LastUsdIndx;
		UsdIndx = pst_Element->pus_ListOfUsedIndex;
		LastUsdIndx = UsdIndx + pst_Element->ul_NumberOfUsedIndex;
		while(UsdIndx < LastUsdIndx)
		{
			VSrc = pst_CurDD->p_Current_Vertex_List + *UsdIndx;
			DstUV = pst_CurDD->pst_ComputingBuffers->ast_UV + *UsdIndx;
			DstUV->v = VSrc->x * Matrix.Iz + VSrc->y * Matrix.Jz + VSrc->z * Matrix.Kz + Matrix.T.z;
			(DstUV++)->u = VSrc->x * stCamDir.x + VSrc->y * stCamDir.y + VSrc->z * stCamDir.z - CamPosDistPlane;
			UsdIndx++;
		}
	} else
	{
		VSrc = pst_CurDD->p_Current_Vertex_List;
		VSrcLast = VSrc + pst_Obj->l_NbPoints;
		DstUV = pst_CurDD->pst_ComputingBuffers->ast_UV;
		while(VSrc < VSrcLast)
		{
			DstUV->v = VSrc->x * Matrix.Iz + VSrc->y * Matrix.Jz + VSrc->z * Matrix.Kz + Matrix.T.z;
			(DstUV++)->u = VSrc->x * stCamDir.x + VSrc->y * stCamDir.y + VSrc->z * stCamDir.z - CamPosDistPlane;
			VSrc++;
		}
	}
	
#endif // #if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)	
}

void MAT_UV_Compute_WaterHole(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,MAT_tdst_MTLevel	*pst_MLTTXLVL , ULONG StoreUV)
{

#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)

	GEO_Vertex	        *VSrc, *VSrcLast;
	SOFT_tdst_UV		*DstUV;
	MATH_tdst_Matrix	Matrix;
	MATH_tdst_Vector	CamPos;	
	float ScaleU,ScaleV;
	pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
	pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
	ScaleU = 1.0f  / MAT_GetScale(pst_MLTTXLVL->ScaleSPeedPosU) * 0.5f;
	ScaleV = 1.0f  / MAT_GetScale(pst_MLTTXLVL->ScaleSPeedPosV) * 0.5f;
	/* 
		This mapping is computed as folow:
			U is the distance to camera 
			V is the World Z 
	*/
	VSrc = pst_CurDD->p_Current_Vertex_List;
	VSrcLast = VSrc + pst_Obj->l_NbPoints;
	DstUV = pst_CurDD->pst_ComputingBuffers->ast_UV;
	MATH_MakeOGLMatrix(&Matrix, pst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix);
	CamPos = pst_CurDD->st_Camera.st_Matrix.T;
	MATH_NegEqualVector(&CamPos);
	CamPos.x += Matrix.T.x;
	CamPos.y += Matrix.T.y;
	CamPos.z += Matrix.T.z;

	while(VSrc < VSrcLast)
	{
		float ZZZ;
		ZZZ = VSrc->x * Matrix.Iz + VSrc->y * Matrix.Jz + VSrc->z * Matrix.Kz + CamPos.z;
		ZZZ = fOptInv(ZZZ);
		DstUV->v = ((VSrc->x * Matrix.Ix + VSrc->y * Matrix.Jx + VSrc->z * Matrix.Kx + CamPos.x) * ZZZ) + ScaleV ;
		(DstUV++)->u = ((VSrc->x * Matrix.Iy + VSrc->y * Matrix.Jy + VSrc->z * Matrix.Ky +  CamPos.y) * ZZZ) + ScaleU ;
		VSrc++;
	}
	
#endif //#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)	
}

void MAT_UV_Compute_DF_GIZMO(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)

    MATH_tdst_Vector	*VNormal, *VNormalLast;
	SOFT_tdst_UV		*DstUV;
	MATH_tdst_Matrix	Matrix ONLY_PSX2_ALIGNED(16);
	MATH_tdst_Vector	CamPos, Local;
    GEO_Vertex          *Point3D;
	ULONG				*CurrentColor;
	float				Norm,Norm2;

	pst_CurDD->pst_ComputingBuffers->Current = (SOFT_tdst_UV *) pst_Obj->dst_UV;
	CamPos.x = CamPos.y = CamPos.z = 0.0f;
	MATH_InvertMatrix(&Matrix, pst_CurDD->st_MatrixStack.pst_CurrentMatrix);
	MATH_TransformVertex(&CamPos, &Matrix, &CamPos);
	GEO_UseNormals(pst_Obj);
	VNormal = pst_Obj->dst_PointNormal;
	VNormalLast = VNormal + pst_Obj->l_NbPoints;
	DstUV = pst_CurDD->pst_ComputingBuffers->ast_UV;
	Point3D = pst_Obj->dst_Point;
	CurrentColor = pst_CurDD->pst_ComputingBuffers->aul_Diffuse;
	while(VNormal < VNormalLast)
	{
    	MATH_SubVector(&Local, &CamPos, VCast(Point3D));
		Norm2 = Local.x * VNormal->x + Local.y * VNormal->y + Local.z * VNormal->z;
		Norm = MATH_f_SqrNormVector(&Local);
		DstUV->u = ((float) ((*CurrentColor >> 16) & 0xff) + ((*CurrentColor >> 8) & 0xff) + ((*CurrentColor) & 0xff)) / 768.0f;
		DstUV->v = Norm2 * fOptInvSqrt(Norm);
		DstUV->v = 1.0f - DstUV->v;
		DstUV->v = fMax(DstUV->v , 0.1f);
		DstUV->v = fMin(DstUV->v , 0.9f);
		DstUV++;
		VNormal++;
		Point3D++;
		CurrentColor++;
	}
	pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
	pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
	
#endif	//#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)
}

void MAT_UV_Compute_PHONG_GIZMO(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)

    MATH_tdst_Vector	*VNormal, *VNormalLast;
	MATH_tdst_Vector	Sunx, Suny;
	SOFT_tdst_UV		*DstUV;
	MATH_tdst_Matrix	Matrix ONLY_PSX2_ALIGNED(16);

		pst_CurDD->pst_ComputingBuffers->Current = (SOFT_tdst_UV *) pst_Obj->dst_UV;
		Sunx.x = Sunx.y = Sunx.z = 0.0f;
		Suny.x = Suny.y = Suny.z = 0.0f;
		switch(MAT_GET_XYZ(CurrentAddFlag))
		{
		case MAT_CC_X:
			Suny.z = Sunx.y = 1.0f;
			break;
		case MAT_CC_Y:
			Suny.z = Sunx.x = 1.0f;
			break;
		case MAT_CC_Z:
			Suny.y = Sunx.x = 1.0f;
			break;
		case MAT_CC_XYZ:
			Suny.y = Sunx.x = 1.0f;
			break;
		}

		MATH_InvertMatrix(&Matrix, pst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix);
		Matrix.T.x = Matrix.T.y = Matrix.T.z = 0.0f;
		MATH_TransformVertex(&Sunx, &Matrix, &Sunx);
		MATH_TransformVertex(&Suny, &Matrix, &Suny);
		MATH_ScaleVector(&Suny, &Suny, 2.0f);
		MATH_ScaleVector(&Sunx, &Sunx, 2.0f);

		VNormal = SOFT_pst_GetSpecularVectors();
		VNormalLast = VNormal + pst_Obj->l_NbPoints;
		DstUV = pst_CurDD->pst_ComputingBuffers->ast_UV;
		while(VNormal < VNormalLast)
		{
			DstUV->u = Sunx.x * VNormal->x + Sunx.y * VNormal->y + Sunx.z * VNormal->z;
			DstUV->v = Suny.x * VNormal->x + Suny.y * VNormal->y + Suny.z * VNormal->z;
			DstUV->u = DstUV->u + 0.5f;
			DstUV->v = DstUV->v + 0.5f;
			DstUV++;
			VNormal++;
		}
		pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
		pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
		
#endif //#if !defined(_GAMECUBE) || defined(USE_SOFT_UV_MATRICES)		
}


MAT_FuncTableNode FuncTable[] = 
{
	MAT_UV_Compute_OBJ1,
	MAT_UV_Compute_OBJ2,
	MAT_UV_Compute_Chrome,
	MAT_UV_Compute_DF_GIZMO,
	MAT_UV_Compute_PHONG_GIZMO,
	MAT_UV_Compute_Previous,
	MAT_UV_Compute_PLANAR_GIZMO_NOSTORE,
	MAT_UV_Compute_FACE_MAP,
	MAT_UV_Compute_FOGZZ,
	MAT_UV_Compute_Default/*MAT_UV_Compute_WaterHole*/,
	MAT_UV_Compute_Default,
	MAT_UV_Compute_Default,
	MAT_UV_Compute_Default,
	MAT_UV_Compute_Default,
	MAT_UV_Compute_Default,
	MAT_UV_Compute_Default
};
/*
 =======================================================================================================================
 =======================================================================================================================
 */

#ifdef _GAMECUBE // all texture generation are hardware
extern void GXI_SetTextureMatrix(u8 _u8Type, GEO_tdst_Object *pst_Obj, GEO_tdst_ElementIndexedTriangles *pst_Element, ULONG CurrentAddFlag, MAT_tdst_Decompressed_UVMatrix *_uvMatrix);
#include "GXI_GC/GXI_dbg.h"
#endif

void MAT_ComputeUV /* And Store Thems */
(
	GEO_tdst_Object						*pst_Obj,
	GEO_tdst_ElementIndexedTriangles	*pst_Element,
	MAT_tdst_MTLevel					*pst_MLTTXLVL
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				CurrentUVMode;
	ULONG				CurrentAddFlag;
#ifdef 	PSX2_TARGET
	static 	ULONG 			LastUVMode;
	static 	ULONG 			LastAddFlag;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef _GAMECUBE
	// special case of gensprite, compute UV array
	if (!((pst_MLTTXLVL->s_TextureId != -1) && (TEX_gst_GlobalList.dst_Texture[pst_MLTTXLVL->s_TextureId].uw_Flags & TEX_uw_IsSpriteGen)))
#if !defined(_FINAL_) && defined(USE_SOFT_UV_MATRICES)
	if(g_bUseHardwareTextureMatrices)
#endif
	{
		MAT_tdst_Decompressed_UVMatrix	TDST_ummAT;
			
		GDI_gpst_CurDD_SPR.ul_DisplayInfo &= ~GDI_Cul_DI_FaceMap;
			
		MAT_UV_Compute_Default(&GDI_gpst_CurDD_SPR , pst_Obj, pst_Element,pst_MLTTXLVL->s_AditionalFlags, 1);
			
		if(!MAT_VUISIdentity_i(pst_MLTTXLVL))
		{
			MAT_VUDecompress(pst_MLTTXLVL, &TDST_ummAT);
			GXI_SetTextureMatrix(MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags), pst_Obj, pst_Element, pst_MLTTXLVL->s_AditionalFlags, &TDST_ummAT);
		}
		else 
			GXI_SetTextureMatrix(MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags), pst_Obj, pst_Element, pst_MLTTXLVL->s_AditionalFlags, NULL);
			
		return;
	}
	
#endif


    PRO_StartTrameRaster(&GDI_gpst_CurDD_SPR.pst_Raster->st_Temp1);

	CurrentUVMode = MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags);
	CurrentAddFlag = pst_MLTTXLVL->s_AditionalFlags;
#ifdef 	PSX2_TARGET
	if (CurrentUVMode == MAT_Cc_UV_Previous)
	{
		CurrentUVMode = LastUVMode;
		CurrentAddFlag = LastAddFlag;
	}
	LastAddFlag = CurrentAddFlag;
	LastUVMode = CurrentUVMode;
#endif
	GDI_gpst_CurDD_SPR.ul_DisplayInfo &= ~GDI_Cul_DI_FaceMap;
	FuncTable[CurrentUVMode].Func(&GDI_gpst_CurDD_SPR , pst_Obj,pst_Element,CurrentAddFlag,1);

#ifdef 	PSX2_TARGET
	{
		Gsp_SetUVMatrix_Identity();
#else
        {
		if(!GDI_gpst_CurDD_SPR.pst_ComputingBuffers->Current) return;
#endif
		if(!MAT_VUISIdentity_i(pst_MLTTXLVL) || UVWave)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MAT_tdst_Decompressed_UVMatrix	TDST_ummAT;
			float							fSaveU;
			SOFT_tdst_UV		*SrcUV, *SrcUVLast, *DstUV;
			unsigned short		*UsdIndx, *LastUsdIndx;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			SrcUV = (SOFT_tdst_UV *) GDI_gpst_CurDD_SPR.pst_ComputingBuffers->Current;
			DstUV = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ast_UV;
			GDI_gpst_CurDD_SPR.pst_ComputingBuffers->Current = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ast_UV;

			/* Compute new UV */
			if(GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
				SrcUVLast = SrcUV + pst_Obj->l_NbPoints;
			else
				SrcUVLast = SrcUV + pst_Obj->l_NbUVs;
			MAT_VUDecompress(pst_MLTTXLVL, &TDST_ummAT);

			if (MAT_VUISIdentity_i(pst_MLTTXLVL) && UVWave )
			{
				TDST_ummAT.UVMatrix[0]=1;
				TDST_ummAT.UVMatrix[1]=0;
				TDST_ummAT.UVMatrix[2]=0;
				TDST_ummAT.UVMatrix[3]=1;
			}

			if((GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint) && (pst_Element->pus_ListOfUsedIndex))
			{
				UsdIndx = pst_Element->pus_ListOfUsedIndex;
				LastUsdIndx = UsdIndx + pst_Element->ul_NumberOfUsedIndex;
				while(UsdIndx < LastUsdIndx)
				{
					/* Turn & rotate UV */
					SrcUV = ((SOFT_tdst_UV *) GDI_gpst_CurDD_SPR.pst_ComputingBuffers->Current) + *UsdIndx;
					fSaveU = SrcUV->u;
					SrcUV->u = TDST_ummAT.UVMatrix[0] * fSaveU + TDST_ummAT.UVMatrix[2] * SrcUV->v + TDST_ummAT.AddU;
					SrcUV->v = TDST_ummAT.UVMatrix[1] * fSaveU + TDST_ummAT.UVMatrix[3] * SrcUV->v + TDST_ummAT.AddV;
					UsdIndx++;
				}
			}
			else
			{
				while(SrcUV < SrcUVLast)
				{
					/* Turn & rotate UV */
					fSaveU = SrcUV->u;
					DstUV->u = TDST_ummAT.UVMatrix[0] * fSaveU + TDST_ummAT.UVMatrix[2] * SrcUV->v + TDST_ummAT.AddU;
					DstUV->v = TDST_ummAT.UVMatrix[1] * fSaveU + TDST_ummAT.UVMatrix[3] * SrcUV->v + TDST_ummAT.AddV;
					SrcUV++;
					DstUV++;
				}
			}
		} 
	}

#ifdef ACTIVE_EDITORS_XX
	/* EMulate PSX2 GS big texuv BUG */ 
	if (!((MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags) == MAT_Cc_UV_Planar_GZMO) && (MAT_GET_MatrixFrom(pst_MLTTXLVL->s_AditionalFlags) == MAT_CC_WORLD)))	
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MAT_tdst_Decompressed_UVMatrix	TDST_ummAT;
		SOFT_tdst_UV		*SrcUV, *SrcUVLast, *DstUV;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		SrcUV = (SOFT_tdst_UV *) GDI_gpst_CurDD_SPR.pst_ComputingBuffers->Current;
		DstUV = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ast_UV;
		GDI_gpst_CurDD_SPR.pst_ComputingBuffers->Current = GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ast_UV;

		/* Compute new UV */
		if(GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
			SrcUVLast = SrcUV + pst_Obj->l_NbPoints;
		else
			SrcUVLast = SrcUV + pst_Obj->l_NbUVs;
		MAT_VUDecompress(pst_MLTTXLVL, &TDST_ummAT);
		while(SrcUV < SrcUVLast)
		{
			/* Turn & rotate UV */
			DstUV->u = fMax(fMin(SrcUV->u , 16.0f) , -16.0f);
			DstUV->v = fMax(fMin(SrcUV->v , 16.0f) , -16.0f);
			SrcUV++;
			DstUV++;
		}
	}
#endif
   PRO_StopTrameRaster(&GDI_gpst_CurDD_SPR.pst_Raster->st_Temp1);
}
#ifdef 	PSX2_TARGET
#endif	

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
