// MATCompute.c PCretail-specific


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

#include "MATerial/MATSingle.h"
#include "MATerial/MATmulti.h"
#include "MATerial/MATSprite.h"

#include "GRObject/GROstruct.h"
#include "GEOmetric/GEO_MRM.h"
#include "SOFT/SOFTlinear.h"

#include "TIMer/TIMdefs.h"

#include "BASe/BENch/BENch.h"

#include "Dx9/Dx9uv.h"
#include "d3dx9math.h"


extern MAT_tdst_MultiTexture    MAT_gst_DefaultSingleMaterial;
static float			        MAT_C_f_5Bits = 32768.0f * 8.0f + 16384.0f * 8.0f;
// static float			        MAT_C_f_7Bits = 32768.0f * 2.0f + 16384.0f * 2.0f;
static float			        MAT_C_f_8Bits = 32768.0f + 16384.0f;
static float			        MAT_C_f_10Bits = 8192.0f + 4096.0f;
// static float			        MAT_C_f_12Bits = 2048.0f + 1024.0f;

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
int g_MultipleVBIndex = 0;


MATH_tdst_Matrix s_stMappingMatrice;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG	MAT_DdctAlph_Normal[128];
ULONG	MAT_DdctAlph_Symetric[128];
ULONG	MAT_DdctAlph_Negative[128];





///////////////////
void MAT_ComputeDeductedAlpha(GEO_tdst_Object *pst_Obj, MAT_tdst_MTLevel *pst_MLTTXLVL)
{
	// empty because is not used anymore
		}

///////////////////
void MAT_SetScale(ULONG *ul_ScaleSPeedPosU, float Scale)
{
	ULONG	ulCalc;

	ILAF(ulCalc) = Scale;
	*ul_ScaleSPeedPosU &= ~(MAT_Cc_ScaleMask | MAT_Cc_ScaleSign);
	*ul_ScaleSPeedPosU |= ulCalc & (MAT_Cc_ScaleMask | MAT_Cc_ScaleSign);
	return;
}

///////////////////
void MAT_SetSpeed(ULONG *ul_ScaleSPeedPosU, float Speed)
{
	ULONG	ulCalc;

	ILAF(ulCalc) = Speed;
	*ul_ScaleSPeedPosU &= ~(MAT_Cc_SpeedMask | MAT_Cc_SpeedSign);
	*ul_ScaleSPeedPosU |= (ulCalc >> 16) & (MAT_Cc_SpeedMask | MAT_Cc_SpeedSign);
}

///////////////////
void MAT_SetPos(ULONG *ul_ScaleSPeedPosU, float Pos)
{
	MAT_SetSpeed(ul_ScaleSPeedPosU, Pos);
}

///////////////////
void MAT_SetRotation(ULONG *ul_ScaleSPeedPosU, ULONG *ul_ScaleSPeedPosV, float Alpha)
{
	ULONG	ulCalc;

	*ul_ScaleSPeedPosU &= ~0x00010001;
	*ul_ScaleSPeedPosV &= ~0x00010001;

	ILAF(ulCalc) = Alpha + MAT_C_f_8Bits;
	*ul_ScaleSPeedPosU |= (ulCalc & 0x80) << 9;
	*ul_ScaleSPeedPosU |= (ulCalc & 0x40) >> 6;
	*ul_ScaleSPeedPosV |= (ulCalc & 0x20) << 11;
	*ul_ScaleSPeedPosV |= (ulCalc & 0x10) >> 4;
}

///////////////////
float MAT_GetRotation(ULONG ul_ScaleSPeedPosU, ULONG ul_ScaleSPeedPosV)
{
	float	fCalc;

	IFAL(fCalc) = IFAL(MAT_C_f_8Bits);
	IFAL(fCalc) |= (ul_ScaleSPeedPosU & 0x00010000) >> 9;
	IFAL(fCalc) |= (ul_ScaleSPeedPosU & 0x00000001) << 6;
	IFAL(fCalc) |= (ul_ScaleSPeedPosV & 0x00010000) >> 11;
	IFAL(fCalc) |= (ul_ScaleSPeedPosV & 0x00000001) << 4;
	return(fCalc - MAT_C_f_8Bits);
}

///////////////////
_inline_ ULONG MAT_IsRotate(ULONG ul_ScaleSPeedPosU, ULONG ul_ScaleSPeedPosV)
{
	return (ul_ScaleSPeedPosU | ul_ScaleSPeedPosV | 0x00010001);
}

///////////////////
ULONG MAT_GetRotation_INT(ULONG ul_ScaleSPeedPosU, ULONG ul_ScaleSPeedPosV)
{
	ULONG	Value;

	Value = (ul_ScaleSPeedPosU & 0x00010000) >> 9;
	Value |= (ul_ScaleSPeedPosU & 0x00000001) << 6;
	Value |= (ul_ScaleSPeedPosV & 0x00010000) >> 11;
	Value |= (ul_ScaleSPeedPosV & 0x00000001) << 4;
	return((Value >> 4) & 0xf);
}

// =======================================================================================================================
//    -> SCALE is between -16..+16 it is coede in the 9 first bit of ScaleSPeedPos with 1 bit of sign, 4 bit integer, 4
//    bits float.
// =======================================================================================================================
float MAT_GetScale(ULONG ul_ScaleSPeedPosU)
{
	float	fCalc;

	if(ul_ScaleSPeedPosU == 0) 
		return 1.0f;

	if(ul_ScaleSPeedPosU == 0x08001000) 
		return 1.0f;	/* Keep old identity value compatible */
		
	IFAL(fCalc) = ul_ScaleSPeedPosU;
	IFAL(fCalc) &= (MAT_Cc_ScaleMask | MAT_Cc_ScaleSign);
	return fCalc;
}

// =======================================================================================================================
//    -> SCALE is between -16..+16 it is coede in the 9 first bit of ScaleSPeedPos with 1 bit of sign, 4 bit integer, 4
//    bits float.
// =======================================================================================================================
_inline_ ULONG MAT_GetScale_INT(ULONG ul_ScaleSPeedPosU)
{
	return 	ul_ScaleSPeedPosU & (MAT_Cc_ScaleMask | MAT_Cc_ScaleSign);
}

// =======================================================================================================================
//    -> SPEED is between -512..+512 (in percentage per SECONDE (%/s)) it is coede in the 15 first bit of ScaleSPeedPos
//    with 1 bit of sign, 10 bit integer, 4 bits float.
// =======================================================================================================================
float MAT_GetSpeed(ULONG ul_ScaleSPeedPosU)
{
	float	fCalc;

	if((ul_ScaleSPeedPosU) == 0) 
		return 0.0f;

	if(ul_ScaleSPeedPosU == 0x08001000) 
		return 0.0f;	// Keep old identity value compatible

	IFAL(fCalc) = ul_ScaleSPeedPosU << (LONG) 16;
	return fCalc;
}
#define MAT_GetPos MAT_GetSpeed
#define MAT_GetPos_i MAT_GetSpeed

// =======================================================================================================================
//    -> POS is between 0,0..1,0 (in percentage) it is coded between 13 to 24 bits (8 bits)of ScaleSPeedPos
// =======================================================================================================================
void MAT_VUDecompressSpeed(MAT_tdst_MTLevel *p_tdstCompressedUV, MAT_tdst_Decompressed_UVMatrix *p_tdstDecompressedUV)
{
	// U
	if(p_tdstCompressedUV->ul_Flags & MAT_Cc_Flag_UDynamicTransEnable)
	{
		p_tdstDecompressedUV->AddU = TIM_gf_MainClock * p_tdstDecompressedUV->AddU;
		p_tdstDecompressedUV->AddU += MAT_C_f_10Bits;
		IFAL(p_tdstDecompressedUV->AddU) = IFAL(MAT_C_f_10Bits) | (IFAL(p_tdstDecompressedUV->AddU) & 0x3ff);
		p_tdstDecompressedUV->AddU -= MAT_C_f_10Bits;
	}

	// V
	if(p_tdstCompressedUV->ul_Flags & MAT_Cc_Flag_VDynamicTransEnable)
	{
		p_tdstDecompressedUV->AddV = TIM_gf_MainClock * p_tdstDecompressedUV->AddV;
		p_tdstDecompressedUV->AddV += MAT_C_f_10Bits;
		IFAL(p_tdstDecompressedUV->AddV) = IFAL(MAT_C_f_10Bits) | (IFAL(p_tdstDecompressedUV->AddV) & 0x3ff);
		p_tdstDecompressedUV->AddV -= MAT_C_f_10Bits;
	}
}
 

// =======================================================================================================================
//    This function will return 1 if compressedUV is != of identity
// =======================================================================================================================
BOOL MAT_VUISIdentity(MAT_tdst_MTLevel *p_tdstLevel)
{
	if((p_tdstLevel->ScaleSPeedPosU == MAT_Cc_Identity) && (p_tdstLevel->ScaleSPeedPosV == MAT_Cc_Identity))
		return TRUE;
	if((p_tdstLevel->ScaleSPeedPosU == 0) || (p_tdstLevel->ScaleSPeedPosV == 0)) return TRUE;
	return FALSE;
}

///////////////////
_inline_ BOOL MAT_VUISIdentity_i(MAT_tdst_MTLevel *p_tdstLevel)
{
	if((p_tdstLevel->ScaleSPeedPosU == MAT_Cc_Identity) && (p_tdstLevel->ScaleSPeedPosV == MAT_Cc_Identity))
		return TRUE;
	if((p_tdstLevel->ScaleSPeedPosU == 0) || (p_tdstLevel->ScaleSPeedPosV == 0)) return TRUE;
	return FALSE;
}

///////////////////
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

///////////////////
void MAT_VUDecompress(MAT_tdst_MTLevel *p_tdstCompressedUV, MAT_tdst_Decompressed_UVMatrix *p_tdstDecompressedUV)
{
	register ULONG	*pMatrix;

	pMatrix = (ULONG *)p_tdstDecompressedUV->UVMatrix;
	pMatrix[0] = MAT_GetScale_INT(p_tdstCompressedUV->ScaleSPeedPosU);
	pMatrix[3] = MAT_GetScale_INT(p_tdstCompressedUV->ScaleSPeedPosV);
	pMatrix[1] = pMatrix[2] = 0;

	// U
	p_tdstDecompressedUV->AddU = MAT_GetPos_i(p_tdstCompressedUV->ScaleSPeedPosU) * p_tdstDecompressedUV->UVMatrix[0];
	// V
	p_tdstDecompressedUV->AddV = MAT_GetPos_i(p_tdstCompressedUV->ScaleSPeedPosV) * p_tdstDecompressedUV->UVMatrix[3];

	if(p_tdstCompressedUV->ul_Flags & (MAT_Cc_Flag_UDynamicTransEnable|MAT_Cc_Flag_VDynamicTransEnable)) MAT_VUDecompressSpeed(p_tdstCompressedUV, p_tdstDecompressedUV);

	// Rotation
	if (MAT_IsRotate(p_tdstCompressedUV->ScaleSPeedPosU, p_tdstCompressedUV->ScaleSPeedPosV)) MAT_ComputeRotationMatrix( p_tdstCompressedUV , p_tdstDecompressedUV);
}

///////////////////
void MAT_UV_Compute_OBJ1(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
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
}

///////////////////
void MAT_UV_Compute_OBJ2(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
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
		}
		
///////////////////
void MAT_UV_Compute_Previous(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
	if(pst_CurDD->pst_ComputingBuffers->Current != NULL)	
        return;
	MAT_UV_Compute_OBJ1(pst_CurDD,pst_Obj,pst_Element,CurrentAddFlag,StoreUV);
}

///////////////////
void MAT_UV_Compute_Default(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
	pst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_UseOneUVPerPoint;
	pst_CurDD->pst_ComputingBuffers->Current = (SOFT_tdst_UV *) pst_Obj->dst_UV;
//	Dx9_SetUVStageOff();
}

///////////////////
void MAT_UV_Compute_Chrome(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
		// setting the hardware UV computation
		Dx9_SetUVChromeStage();

        // Vertex shaders compute UVs starting from either :
        // - object UVs
        // - object-space coordinates
        // - object-space normals
        // whereas the FFP starts uses :
        // - object UVs
        // - camera-space coordinates
        // - camera-space normals
        // so we have to treat both cases differently, by tweaking the matrices we provide...
        if (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseLitAlphaShader)
        {
            MATH_tdst_Vector T = { 0.5f, 0.5f, 0.0f };
            MATH_GetRotationMatrix(&s_stMappingMatrice, pst_CurDD->st_MatrixStack.pst_CurrentMatrix);
            MATH_SetTranslation(&s_stMappingMatrice, &T);
            MATH_SetZoom(&s_stMappingMatrice, -0.5f);
		    if (!(pst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Symetric))
		    {
			    s_stMappingMatrice.Ix = -s_stMappingMatrice.Ix;
			    s_stMappingMatrice.Iy = -s_stMappingMatrice.Iy;
		    }
        }
        else
        {
		    MATH_tdst_Vector VU,VV,VW,VT,VS ;
		    if (!(pst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Symetric))
			    MATH_InitVector(&VU , 0.5f , 0.f , 0.f);
		    else
			    MATH_InitVector(&VU , -0.5f , 0.f , 0.f);
		    MATH_InitVector(&VV , 0.f , -0.5f , 0.f);
		    MATH_InitVector(&VW , 0.f , 0.f , -0.5f);
		    MATH_InitVector(&VT , 0.5f , 0.5f , 0.0f);
		    MATH_InitVector(&VS , 1.f , 1.f , 1.f);
		    MATH_SetMatrixWithType(&s_stMappingMatrice,&VU,&VV,&VW,&VT,&VS,0.f,MATH_Ci_Rotation);
        }

		pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
		pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
}

///////////////////
void MAT_UV_Compute_PLANAR_GIZMO(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
        const BOOL bNotUsingVertexShader = ((GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseLitAlphaShader) == 0);
		MATH_tdst_Matrix	Matrix ONLY_PSX2_ALIGNED(16);
		MATH_tdst_Matrix	CameraToObjectMatrix;
		MATH_tdst_Vector	CamPos;
		GEO_Vertex	        *VSrc, *VSrcLast;
		SOFT_tdst_UV		*DstUV;
		VSrc = pst_CurDD->p_Current_Vertex_List;
		VSrcLast = VSrc + pst_Obj->l_NbPoints;
		DstUV = pst_CurDD->pst_ComputingBuffers->ast_UV;
		pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
		pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;

        // Vertex shaders compute UVs starting from either :
        // - object UVs
        // - object-space coordinates
        // - object-space normals
        // whereas the FFP starts uses :
        // - object UVs
        // - camera-space coordinates
        // - camera-space normals
        // so we have to treat both cases differently, by tweaking the matrices we provide...

		Dx9_SetUVNormalStage();

        if (bNotUsingVertexShader)
        {
		    // calcul de la matrice de transformation de camera vers objet
		    if (MATH_b_TestScaleType(pst_CurDD->st_Camera.pst_ObjectToCameraMatrix))
		    {
			    MATH_tdst_Matrix	Matrix3;
			    MATH_MakeOGLMatrix(&Matrix3,pst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
			    CameraToObjectMatrix.lType = MATH_Ci_Rotation;
			    D3DXMatrixInverse((D3DXMATRIX*)&CameraToObjectMatrix,NULL,(CONST D3DXMATRIX*)&Matrix3);
		    }
		    else
            {
			    MATH_InvertMatrix(&CameraToObjectMatrix,pst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
            }
        }

		if(MAT_GET_MatrixFrom(CurrentAddFlag) == MAT_CC_OBJECT)
		{
			switch(MAT_GET_XYZ(CurrentAddFlag))
			{
			case MAT_CC_X:
				{
					MATH_tdst_Vector VU,VV,VW,VT,VS ;
					MATH_InitVector(&VU , 0.f , 0.f , 1.f);
					MATH_InitVector(&VV , 1.f , 0.f , 0.f);
					MATH_InitVector(&VW , 0.f , 1.f , 0.f);
					MATH_InitVector(&VT , 0.f , 0.f , 0.0f);
					MATH_InitVector(&VS , 1.f , 1.f , 1.f);

                    if (bNotUsingVertexShader)
                    {
					    MATH_tdst_Matrix stM;
					    MATH_SetMatrixWithType(&stM,&VU,&VV,&VW,&VT,&VS,0.f,MATH_Ci_Rotation);
    					MATH_MulMatrixMatrix(&s_stMappingMatrice,&CameraToObjectMatrix,&stM);
                    }
                    else
                    {
					    MATH_SetMatrixWithType(&s_stMappingMatrice,&VU,&VV,&VW,&VT,&VS,0.f,MATH_Ci_Rotation);
                    }
				}
				return;
			case MAT_CC_Y:
				{
					MATH_tdst_Vector VU,VV,VW,VT,VS ;
					MATH_InitVector(&VU , 1.f , 0.f , 0.f);
					MATH_InitVector(&VV , 0.f , 0.f , 1.f);
					MATH_InitVector(&VW , 0.f , 1.f , 0.f);
					MATH_InitVector(&VT , 0.f , 0.f , 0.0f);
					MATH_InitVector(&VS , 1.f , 1.f , 1.f);

                    if (bNotUsingVertexShader)
                    {
					    MATH_tdst_Matrix stM;
					    MATH_SetMatrixWithType(&stM,&VU,&VV,&VW,&VT,&VS,0.f,MATH_Ci_Rotation);
    					MATH_MulMatrixMatrix(&s_stMappingMatrice,&CameraToObjectMatrix,&stM);
                    }
                    else
                    {
					    MATH_SetMatrixWithType(&s_stMappingMatrice,&VU,&VV,&VW,&VT,&VS,0.f,MATH_Ci_Rotation);
                    }
				}
				return;
			case MAT_CC_Z:
				// get the inverse camera transform matrix
                if (bNotUsingVertexShader)
                {
				    s_stMappingMatrice = CameraToObjectMatrix;
                }
                else
                {
				    s_stMappingMatrice = MATH_gst_IdentityMatrix;
                }
				return;
			case MAT_CC_XYZ:
				{
					{
						MATH_tdst_Vector VU,VV,VW,VT,VS ;
						MATH_InitVector(&VU , Cf_Sqrt2 * 0.5f ,-Cf_InvSqrt3, 0.408f);
						MATH_InitVector(&VV ,  -Cf_Sqrt2 * 0.5f , -Cf_InvSqrt3 , 0.408f);
						MATH_InitVector(&VW , 0.f , Cf_InvSqrt3 , 0.816f);
						MATH_InitVector(&VT , 0.f , 0.f , 0.0f);
						MATH_InitVector(&VS , 1.f , 1.f , 1.f);

                        if (bNotUsingVertexShader)
                        {
						    MATH_tdst_Matrix stM;
						    MATH_SetMatrixWithType(&stM,&VU,&VV,&VW,&VT,&VS,0.f,MATH_Ci_Rotation);
    					    MATH_MulMatrixMatrix(&s_stMappingMatrice,&CameraToObjectMatrix,&stM);
                        }
                        else
                        {
						    MATH_SetMatrixWithType(&s_stMappingMatrice,&VU,&VV,&VW,&VT,&VS,0.f,MATH_Ci_Rotation);
                        }
					}
					return;
				}
			}
			return;
		}
		else
	// if (MAT_GET_MatrixFrom(pst_MLTTXLVL->s_AditionalFlags) == MAT_CC_WORLD)
		{
			if ((MAT_GET_MatrixFrom(CurrentAddFlag) == MAT_CC_GIZMO) &&	
				(pst_CurDD->pst_CurrentGameObject->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix))
			{
				LONG	GizmoNum;

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
				// Camera system
					MATH_MakeOGLMatrix(&Matrix, pst_CurDD->st_MatrixStack.pst_CurrentMatrix);
					Coef = pst_CurDD->st_Camera.f_FactorX / pst_CurDD->st_Camera.f_CenterX;
					MATH_ScaleEqualVector(MATH_pst_GetXAxis(&Matrix),Coef);
					MATH_ScaleEqualVector(MATH_pst_GetYAxis(&Matrix),Coef);
					MATH_ScaleEqualVector(MATH_pst_GetZAxis(&Matrix),Coef);

					CamPos = Matrix.T;
					MATH_ScaleEqualVector(&CamPos,Coef);
				} else
				{
				// World system
					MATH_MakeOGLMatrix(&Matrix, pst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix);
					CamPos = Matrix.T;
				} 
			}

			switch(MAT_GET_XYZ(CurrentAddFlag))
			{
			case MAT_CC_X:
				{
					MATH_tdst_Vector VU,VV,VW,VT,VS ;
					MATH_tdst_Matrix stM;
					MATH_InitVector(&VU , 0.f , 1.f , 0.f);
					MATH_InitVector(&VV , 0.f , 0.f , 1.f);
					MATH_InitVector(&VW , 1.f , 0.f , 0.f);
					MATH_InitVector(&VT , 0.f , 0.f , 0.0f);
					MATH_InitVector(&VS , 1.f , 1.f , 1.f);
					MATH_SetMatrixWithType(&stM,&VU,&VV,&VW,&VT,&VS,0.f,MATH_Ci_Rotation);

                    if (bNotUsingVertexShader)
                    {
					    MATH_tdst_Matrix	Matrix3;
					    MATH_MulMatrixMatrix(&Matrix3,&Matrix,&stM);
					    MATH_MulMatrixMatrix(&s_stMappingMatrice,&CameraToObjectMatrix,&Matrix3);
                    }
                    else
                    {
					    MATH_MulMatrixMatrix(&s_stMappingMatrice,&Matrix,&stM);
                    }
				}
				break;
			case MAT_CC_Y:
				{
					MATH_tdst_Vector VU,VV,VW,VT,VS ;
					MATH_tdst_Matrix stM;
					MATH_InitVector(&VU , 1.f , 0.f , 0.f);
					MATH_InitVector(&VV , 0.f , 0.f , 1.f);
					MATH_InitVector(&VW , 0.f , 1.f , 0.f);
					MATH_InitVector(&VT , 0.f , 0.f , 0.0f);
					MATH_InitVector(&VS , 1.f , 1.f , 1.f);
					MATH_SetMatrixWithType(&stM,&VU,&VV,&VW,&VT,&VS,0.f,MATH_Ci_Rotation);

                    if (bNotUsingVertexShader)
                    {
					    MATH_tdst_Matrix	Matrix3;
					    MATH_MulMatrixMatrix(&Matrix3,&Matrix,&stM);
					    MATH_MulMatrixMatrix(&s_stMappingMatrice,&CameraToObjectMatrix,&Matrix3);
                    }
                    else
                    {
					    MATH_MulMatrixMatrix(&s_stMappingMatrice,&Matrix,&stM);
                    }
				}
				break;
			case MAT_CC_Z:
                if (bNotUsingVertexShader)
				{
					MATH_MulMatrixMatrix(&s_stMappingMatrice,&CameraToObjectMatrix,&Matrix);
				}
                else
                {
                    s_stMappingMatrice = Matrix;
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
	
					{
						MATH_tdst_Vector VU,VV,VW,VT,VS ;
						MATH_tdst_Matrix stM;
						MATH_InitVector(&VU , Cf_Sqrt2 * 0.5f ,-Cf_InvSqrt3, 0.408f);
						MATH_InitVector(&VV ,  -Cf_Sqrt2 * 0.5f , -Cf_InvSqrt3 , 0.408f);
						MATH_InitVector(&VW , 0.f , Cf_InvSqrt3 , 0.816f);
//					MATH_InitVector(&VU , Cf_Sqrt2 * 0.5f , -Cf_Sqrt2 * 0.5f, 0.f);
//					MATH_InitVector(&VV , -Cf_InvSqrt3 , -Cf_InvSqrt3 , Cf_InvSqrt3);
//					MATH_InitVector(&VW , 0.408f , 0.408f , 0.816f);
						MATH_InitVector(&VT , 0.f , 0.f , 0.0f);
						MATH_InitVector(&VS , 1.f , 1.f , 1.f);
						MATH_SetMatrixWithType(&stM,&VU,&VV,&VW,&VT,&VS,0.f,MATH_Ci_Rotation);

                        if (bNotUsingVertexShader)
                        {
						    MATH_tdst_Matrix	Matrix3;
						    MATH_MulMatrixMatrix(&Matrix3,&Matrix,&stM);
						    MATH_MulMatrixMatrix(&s_stMappingMatrice,&CameraToObjectMatrix,&Matrix3);
                        }
                        else
                        {
						    MATH_MulMatrixMatrix(&s_stMappingMatrice,&Matrix,&stM);
                        }
					}
				}
				break;
			}
		}
		pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
		pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;	
}

///////////////////
void MAT_UV_COMPUTE_GetMAtrix(GDI_tdst_DisplayData	*pst_CurDD,OBJ_tdst_GameObject *_pst_GO , ULONG CurrentAddFlag , MATH_tdst_Matrix	*p_MatrixDSt)
{
	if ((MAT_GET_MatrixFrom(CurrentAddFlag) == MAT_CC_GIZMO) &&	(pst_CurDD->pst_CurrentGameObject->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix))
	{
		LONG	GizmoNum;

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
			// Camera system
			MATH_MakeOGLMatrix(p_MatrixDSt, pst_CurDD->st_MatrixStack.pst_CurrentMatrix);
			Coef = pst_CurDD->st_Camera.f_FactorX / pst_CurDD->st_Camera.f_CenterX;
			MATH_ScaleEqualVector(MATH_pst_GetXAxis(p_MatrixDSt),Coef);
			MATH_ScaleEqualVector(MATH_pst_GetYAxis(p_MatrixDSt),Coef);
			MATH_ScaleEqualVector(MATH_pst_GetZAxis(p_MatrixDSt),Coef);
			MATH_ScaleEqualVector(&p_MatrixDSt->T,Coef);
		} else
		{
			// World system
			MATH_MakeOGLMatrix(p_MatrixDSt, _pst_GO->pst_GlobalMatrix);
		} 
	}
	MATH_TranspEq33Matrix(p_MatrixDSt);
}
#define MAT_UV_Compute_PLANAR_GIZMO_NOSTORE MAT_UV_Compute_PLANAR_GIZMO

///////////////////
void MAT_UV_Compute_FACE_MAP(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
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
}

///////////////////
void MAT_UV_Compute_FOGZZ(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
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

	//	This mapping is computed as follow:
	//		U is the distance to camera projected on the ground
	//		V is the World Z 
	MATH_MakeOGLMatrix(&Matrix, pst_CurDD->pst_CurrentGameObject->pst_GlobalMatrix);

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
}

///////////////////
void MAT_UV_Compute_WaterHole(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,MAT_tdst_MTLevel	*pst_MLTTXLVL , ULONG StoreUV)
{
	GEO_Vertex	        *VSrc, *VSrcLast;
	SOFT_tdst_UV		*DstUV;
	MATH_tdst_Matrix	Matrix;
	MATH_tdst_Vector	CamPos;	
	float ScaleU,ScaleV;
	pst_CurDD->pst_ComputingBuffers->Current = pst_CurDD->pst_ComputingBuffers->ast_UV;
	pst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseOneUVPerPoint;
	ScaleU = 1.0f  / MAT_GetScale(pst_MLTTXLVL->ScaleSPeedPosU) * 0.5f;
	ScaleV = 1.0f  / MAT_GetScale(pst_MLTTXLVL->ScaleSPeedPosV) * 0.5f;

	//	This mapping is computed as folow:
	//		U is the distance to camera 
	//		V is the World Z 
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
}

///////////////////
void MAT_UV_Compute_DF_GIZMO(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
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
}

///////////////////
void MAT_UV_Compute_PHONG_GIZMO(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV)
{
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
}

///////////////////
typedef struct MAT_FuncTableNode_
{
	void (* Func)(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles *pst_Element,ULONG CurrentAddFlag,ULONG StoreUV);
} MAT_FuncTableNode;

///////////////////
static MAT_FuncTableNode FuncTable[] = 
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

///////////////////
void MAT_ComputeUV // And Store Them
(
	GEO_tdst_Object						*pst_Obj,
	GEO_tdst_ElementIndexedTriangles	*pst_Element,
	MAT_tdst_MTLevel					*pst_MLTTXLVL
)
{
	ULONG				CurrentUVMode;
	ULONG				CurrentAddFlag;

    PRO_StartTrameRaster(&GDI_gpst_CurDD_SPR.pst_Raster->st_Temp1);

	CurrentUVMode = MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags);
	CurrentAddFlag = pst_MLTTXLVL->s_AditionalFlags;

	Dx9_SetUVStageOff();

	GDI_gpst_CurDD_SPR.ul_DisplayInfo &= ~GDI_Cul_DI_FaceMap;
	FuncTable[CurrentUVMode].Func(&GDI_gpst_CurDD_SPR , pst_Obj,pst_Element,CurrentAddFlag,1);

	if(!GDI_gpst_CurDD_SPR.pst_ComputingBuffers->Current) 
	{
		if (Dx9_GetCurrentUVState() != Dx9_eUVOff)
		{
			MATH_tdst_Matrix Matrix;
			MATH_MakeOGLMatrix(&Matrix, &s_stMappingMatrice);
//            if (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseLitAlphaShader)
//            {
//                Dx9_vSetUVTransformationForShaders((D3DMATRIX *)&Matrix);
//            }
//            else
            {
			    Dx9_SetUVStageMatrix((D3DMATRIX *)&Matrix);
            }
		}
		return;
	}

	{
		MATH_tdst_Matrix Matrix;

		if(!MAT_VUISIdentity_i(pst_MLTTXLVL))
		{
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			MAT_tdst_Decompressed_UVMatrix	TDST_ummAT;
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			MATH_tdst_Matrix stM;
			MATH_tdst_Vector VU,VV,VW,VT,VS ;
	
			MAT_VUDecompress(pst_MLTTXLVL, &TDST_ummAT);

			MATH_InitVector(&VU , TDST_ummAT.UVMatrix[0] , TDST_ummAT.UVMatrix[1] , 0.f);
			MATH_InitVector(&VV , TDST_ummAT.UVMatrix[2] , TDST_ummAT.UVMatrix[3] , 0.f);
			MATH_InitVector(&VW , 0.f , 0.f , 1.f);
			MATH_InitVector(&VT , TDST_ummAT.AddU , TDST_ummAT.AddV , 0.0f);
			MATH_InitVector(&VS , 1.f , 1.f , 1.f);
			MATH_SetMatrixWithType(&stM,&VU,&VV,&VW,&VT,&VS,0.f,MATH_Ci_Rotation|MATH_Ci_Translation);

			if (Dx9_GetCurrentUVState() != Dx9_eUVOff)
			{
				MATH_MulMatrixMatrix(&Matrix,&s_stMappingMatrice,&stM);	
				MATH_MakeOGLMatrix(&Matrix, &Matrix);
			}
			else
			{
				Dx9_SetUVRotationStage();
				MATH_MakeOGLMatrix(&Matrix, &stM);
				Matrix.Kx = Matrix.T.x;
				Matrix.Ky = Matrix.T.y;
			}

			// setting the transform matrix
//            if (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseLitAlphaShader)
//            {
//                Dx9_vSetUVTransformationForShaders((D3DMATRIX *)&Matrix);
//            }
//            else
            {
			    Dx9_SetUVStageMatrix((D3DMATRIX *)&Matrix);
            }
		}
		else
		{
			if (Dx9_GetCurrentUVState() != Dx9_eUVOff)
			{
				MATH_MakeOGLMatrix(&Matrix, &s_stMappingMatrice);
//                if (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseLitAlphaShader)
//                {
//                    Dx9_vSetUVTransformationForShaders((D3DMATRIX *)&Matrix);
//                }
//                else
                {
			        Dx9_SetUVStageMatrix((D3DMATRIX *)&Matrix);
                }
			}
			else
			{
				Dx9_SetUVStageOff();
				}
			}
	}
   PRO_StopTrameRaster(&GDI_gpst_CurDD_SPR.pst_Raster->st_Temp1);
   
}

