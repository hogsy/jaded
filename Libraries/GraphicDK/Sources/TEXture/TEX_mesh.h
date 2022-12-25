#ifndef _TEX_MESH_
#define _TEX_MESH_

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

typedef struct WATER_Export_Struct_
{
	ULONG				ABadCafe; // SpecialCode


	// !!! ORDER IS IMPORTANT !!!
	ULONG				Activate;
	ULONG				SkyCol;
	ULONG				BottomCol;
	ULONG				WaterCol;
	ULONG				FrustrumMesh_SX;
	ULONG				FrustrumMesh_SY;
	ULONG				ulWAveMapShift;
	ULONG				ulWAveMapShiftCHP;
	ULONG				CloudShadingEnabled;
	ULONG				ulTextureRef;
	ULONG				ulSymetryActive;

	float				ZFactor;
	float				fChoppyFactor ;
	float				HCoef ;
	float				SpeedFactor ;
	float				GlobalWaterZ;
	float				WindBalance ;
	float				P_Factor ;
	float				TextorigFactor;
	
	ULONG				ulLastValue;
	
	
}
WATER_Export_Struct;

void WTR_Mesh_Destroy();
void WTR_SetDefaulExp(WATER_Export_Struct *pExp);
void WTR_SetExportParams(WATER_Export_Struct *pExp);
void WTR_Get_A_DifVector(MATH_tdst_Vector *pSrc , MATH_tdst_Vector *pDst , ULONG Converg);
void WTR_Get_A_NrmVector(MATH_tdst_Vector *pSrc , MATH_tdst_Vector *pDst );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif	