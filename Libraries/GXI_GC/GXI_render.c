
#include "GEOmetric/GEOobject.h"
#include "GXI_init.h"
#include "GXI_render.h"
#include "GXI_displaylist.h"
#include "GXI_vertexspace.h"
#include "GXI_tex.h"
#include "GXI_font.h"
#include "GXI_shadows.h"
#include "GXI_renderstate.h"
#include "GXI_bench.h"
#include "BASe/MEMory/MEM.h"
#include "MATerial/MATstruct.h"
#include "GXI_dbg.h"

#include "GEOmetric/GEO_STRIP.h"
#include "GEOmetric/GEO_SKIN.h"
#include "ENGine/Sources/WORld/WORinit.h"

#define LAST_MODIF

#include "BAse/BENch/BENch.h"

BOOL g_bUVGenFromPos=FALSE;
BOOL g_bUVGenFromNrm=FALSE;
extern BOOL GDI_gb_WaveSprite;
#ifdef USE_HARDWARE_LIGHTS
BOOL g_bLightOff=FALSE;
#endif
#ifndef USE_FULL_SCENE_SHADOWS
extern BOOL g_bRenderingShadows;
#endif

Mtx  g_MtxWaterRefClipMatrix;

#ifndef DONT_USE_ALPHA_DEST
#define GX_GXLoadTexMtxImm 		GXLoadTexMtxImm
#define GX_GXSetTexCoordGen2 	GXSetTexCoordGen2
#else   
typedef struct tdstSaveLastLevel_
{
	GXTexMtx 		MatrixNumber;
	GXTexMtxType 	MatrixType;
	Mtx 			mView;
	
	GXTexMtx 		MatrixNumber2;
	GXTexMtxType 	MatrixType2;
	Mtx 			mView2;
	
	// SetTetureCoord2 Parametters
	GXTexGenType 	P0;
	GXTexGenSrc 	P1;
	GXTexMtx 		P2;
	GXBool 			P3;
	GXPTTexMtx 		P4;
	
	GXTexObj 		*LastTextObj;
} tdstSaveLastLevel;

tdstSaveLastLevel g_sdt_SaveLastLevel;

void GX_GXLoadTexMtxImm(Mtx *mView , GXTexMtx MatrixNumber ,  GXTexMtxType MatrixType)
{
	if (GX_TEXMTX0 == MatrixNumber)
	{
		L_memcpy(&g_sdt_SaveLastLevel . mView , mView , sizeof (Mtx));
		g_sdt_SaveLastLevel . MatrixNumber = MatrixNumber;
		g_sdt_SaveLastLevel . MatrixType = MatrixType;
	} else
	if (GX_PTTEXMTX0 == MatrixNumber)
	{
		L_memcpy(&g_sdt_SaveLastLevel . mView2 , mView , sizeof (Mtx));
		g_sdt_SaveLastLevel . MatrixNumber2 = MatrixNumber;
		g_sdt_SaveLastLevel . MatrixType2 = MatrixType;
	}
	GXLoadTexMtxImm(mView, MatrixNumber ,  MatrixType);
}

void GX_GXSetTexCoordGen2(GXTexCoordID TC0, GXTexGenType P0 , GXTexGenSrc P1, GXTexMtx P2 ,GXBool P3, GXPTTexMtx P4)
{
	g_sdt_SaveLastLevel.P0 = P0;
	g_sdt_SaveLastLevel.P1 = P1;
	g_sdt_SaveLastLevel.P2 = P2;
	g_sdt_SaveLastLevel.P3 = P3;
	g_sdt_SaveLastLevel.P4 = P4;
	GXSetTexCoordGen2(TC0, P0, P1, P2 ,P3, P4);
}

#endif // !DONT_USE_ALPHA_DEST

#ifdef USE_MY_TEX_REGION
	extern GXTexRegion  MyTexRegions[8]; // cache regions
#endif

u8* gpucGXI_TmpDLBuff = NULL;
u32 guGXI_TmpDLBuffSize = 0;
#ifdef ENABLE_ERR_MSG
int TmpDLBuffLock = 0;
#endif // ENABLE_ERR_MSG

void GXI_FreeTmpDLBuff()
{
	#ifdef ENABLE_ERR_MSG
	ERR_X_Assert(TmpDLBuffLock==0);
	#endif // ENABLE_ERR_MSG
	
	if(guGXI_TmpDLBuffSize)
	{
		MEM_FreeAlign(gpucGXI_TmpDLBuff);
		guGXI_TmpDLBuffSize = 0;
		gpucGXI_TmpDLBuff = NULL;
	}
}

u8* GXI_LockTmpDLBuff(u32 _size)
{
	#ifdef ENABLE_ERR_MSG
	ERR_X_Assert(TmpDLBuffLock==0);
	#endif // ENABLE_ERR_MSG
	
	if(guGXI_TmpDLBuffSize<_size)
	{
		GXI_FreeTmpDLBuff();
		gpucGXI_TmpDLBuff = (u8*)MEM_p_AllocAlign(_size,32);
		guGXI_TmpDLBuffSize = _size;
	}

	#ifdef ENABLE_ERR_MSG
	TmpDLBuffLock = 1;	
	#endif // ENABLE_ERR_MSG	
	return gpucGXI_TmpDLBuff;
}

void GXI_UnLockTmpDLBuff()
{
	#ifdef ENABLE_ERR_MSG
	ERR_X_Assert(TmpDLBuffLock);
	TmpDLBuffLock = 0;
	#endif // ENABLE_ERR_MSG
}

LONG GXI_l_DrawElementIndexedTrianglesUsingDL(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints);
	
LONG GXI_l_DrawElementIndexedTrianglesDirectly
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints);
	
LONG GXI_l_CloneDrawElementIndexedTriangles
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	MATH_tdst_Vector					*_pst_Normal,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints);

inline
BOOL GXI_b_IsStripped(GEO_tdst_ElementIndexedTriangles	*_pst_Element)
{
	ERR_X_Assert(_pst_Element!=NULL);
	if(_pst_Element->pst_StripData != NULL)
	{
		if(_pst_Element->pst_StripData->ulFlag & GEO_C_Strip_DataValid)
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
}

extern void MAT_UV_COMPUTE_GetMAtrix(GDI_tdst_DisplayData	*pst_CurDD,OBJ_tdst_GameObject *_pst_GO , ULONG CurrentAddFlag , MATH_tdst_Matrix	*p_MatrixDSt);
void GXI_Compute_Planar_UV(GEO_tdst_Object	*pst_Obj, GEO_tdst_ElementIndexedTriangles *pst_Element, u32 CurrentAddFlag)
{
	Mtx mtx;
	MATH_tdst_Matrix matrix;
	
	if(GXI_Global_ACCESS(bSendingObjectList))
	{
		GX_GXLoadTexMtxImm(GXI_Global_ACCESS(pcurrent_object_list_item)->texmatrix_2, GX_TEXMTX0,  GX_MTX2x4);	
		return;
	}
	
	L_memset(&mtx, 0, sizeof(Mtx));
		
	if(MAT_GET_MatrixFrom(CurrentAddFlag) == MAT_CC_OBJECT)
	{
		switch(MAT_GET_XYZ(CurrentAddFlag))
		{
			case MAT_CC_X:
				mtx[0][1] = 1.0f;
				mtx[1][2] = 1.0f;
			break;
					
			case MAT_CC_Y:
				mtx[0][0] = 1.0f;
				mtx[1][2] = 1.0f;
			break;

			case MAT_CC_Z:
				mtx[0][0] = 1.0f;
				mtx[1][1] = 1.0f;
			break;

			case MAT_CC_XYZ:
				mtx[0][0] = Cf_Sqrt2 * 0.5f;
				mtx[0][1] = -Cf_Sqrt2 * 0.5f;
				mtx[1][0] = -Cf_InvSqrt3;
				mtx[1][1] = -Cf_InvSqrt3;
				mtx[1][2] = Cf_InvSqrt3;
			break;
		}
	}
	else
	{
		MAT_UV_COMPUTE_GetMAtrix(GDI_gpst_CurDD , (OBJ_tdst_GameObject*)pst_Obj,  CurrentAddFlag , &matrix);
		switch(MAT_GET_XYZ(CurrentAddFlag))
		{
			case MAT_CC_X:
/*			
				mtx[0][0] = matrix.Iy;
				mtx[0][1] = matrix.Jy;
				mtx[0][2] = matrix.Ky;
				mtx[0][3] = matrix.T.y;			
				mtx[1][0] = matrix.Iz;
				mtx[1][1] = matrix.Jz;
				mtx[1][2] = matrix.Kz;					
				mtx[1][3] = matrix.T.z;		
*/				

				mtx[0][0] = matrix.Jx;
				mtx[0][1] = matrix.Jy;
				mtx[0][2] = matrix.Jz;
				mtx[0][3] = matrix.T.y;			
				mtx[1][0] = matrix.Kx;
				mtx[1][1] = matrix.Ky;
				mtx[1][2] = matrix.Kz;				
				mtx[1][3] = matrix.T.z;			
					
			break;
	
			case MAT_CC_Y:
				mtx[0][0] = matrix.Ix;
				mtx[0][1] = matrix.Iy;
				mtx[0][2] = matrix.Iz;
				mtx[0][3] = matrix.T.x;			
				mtx[1][0] = matrix.Kx;
				mtx[1][1] = matrix.Ky;
				mtx[1][2] = matrix.Kz;				
				mtx[1][3] = matrix.T.z;			
			break;

			case MAT_CC_Z:
				mtx[0][0] = matrix.Ix;
				mtx[0][1] = matrix.Iy;
				mtx[0][2] = matrix.Iz;
				mtx[0][3] = matrix.T.x;			
				mtx[1][0] = matrix.Jx;
				mtx[1][1] = matrix.Jy;
				mtx[1][2] = matrix.Jz;				
				mtx[1][3] = matrix.T.y;			
			break;

			case MAT_CC_XYZ: 
				mtx[0][0] = Cf_Sqrt2 * 0.5f;
				mtx[0][1] = -Cf_Sqrt2 * 0.5f;
				mtx[1][0] = -Cf_InvSqrt3;
				mtx[1][1] = -Cf_InvSqrt3;
				mtx[1][2] = Cf_InvSqrt3;
					
				MTXConcat(mtx, *(Mtx*)&matrix, mtx);
			break;
		}
	}

	if(GXI_Global_ACCESS(bPutObjectInObjectList))
	{
		MTXCopy(mtx, GXI_Global_ACCESS(current_object_list_item).texmatrix_2);
		return;
	}
				
	GX_GXLoadTexMtxImm(mtx, GX_TEXMTX0,  GX_MTX2x4);	
}

void GXI_SetTextureMatrix(u8 _u8Type, GEO_tdst_Object *pst_Obj, GEO_tdst_ElementIndexedTriangles *pst_Element, u32 CurrentAddFlag, MAT_tdst_Decompressed_UVMatrix *_uvMatrix)
{
	Mtx mtx;
	static u8 last = 0;

	BOOL bUseUVMatrix = (_uvMatrix!=NULL);
	
	g_bUVGenFromPos = FALSE;
	g_bUVGenFromNrm = FALSE;
	
	// set texgen to previously setted texgen
	if(_u8Type==5) //MAT_UV_Compute_Previous,
	{
		switch(last)
		{
			case 1: goto lastchrome1;
			case 2: goto lastchrome2;
			case 3: goto lastplanar1;
			case 4: goto lastplanar2;
			case 5: goto lastdefault1;
			case 6: goto lastdefault2;
		}
		
		return;
	}
		
	if(GXI_Global_ACCESS(bSendingObjectList))
	{
		if(GXI_Global_ACCESS(pcurrent_object_list_item)->texmatrix_useUvMatrix)
		{
			MTXCopy(GXI_Global_ACCESS(pcurrent_object_list_item)->texmatrix_1, mtx);
			bUseUVMatrix = TRUE;
		}
	}
	else
	{
		// uv rotation and translation
		if(_uvMatrix) 
		{
		 	mtx[0][0] = _uvMatrix->UVMatrix[0];
		 	mtx[0][1] = _uvMatrix->UVMatrix[2];
		 	mtx[0][2] = 0.0f;
		 	mtx[0][3] = _uvMatrix->AddU;
		 	
		 	mtx[1][0] = _uvMatrix->UVMatrix[1];
		 	mtx[1][1] = _uvMatrix->UVMatrix[3];
		 	mtx[1][2] = 0.0f;
		 	mtx[1][3] = _uvMatrix->AddV;
		 	
		 	mtx[2][0] = 0.0f;
		 	mtx[2][1] = 0.0f;
		 	mtx[2][2] = 0.0f;
		 	mtx[2][3] = 1.0f;
		 	
			if(GXI_Global_ACCESS(bPutObjectInObjectList))
			{
				GXI_Global_ACCESS(current_object_list_item).texmatrix_useUvMatrix = TRUE;
				MTXCopy(mtx, GXI_Global_ACCESS(current_object_list_item).texmatrix_1);
			}
		}
		else
		{
			if(GXI_Global_ACCESS(bPutObjectInObjectList))
				GXI_Global_ACCESS(current_object_list_item).texmatrix_useUvMatrix = FALSE;
		}
	}
	
	if(GXI_Global_ACCESS(bPutObjectInObjectList))
	{
		GXI_Global_ACCESS(current_object_list_item).texmatrix_type = _u8Type;
	}
	
	// other uv manipulations			
	switch(_u8Type)
	{
		case 2: //MAT_UV_Compute_Chrome,
			{
		    	Mtx mtxFinal;		    	
		    	
		    	GEO_UseNormals(pst_Obj);
				
				if(GXI_Global_ACCESS(bSendingObjectList))
				{
					MTXCopy(GXI_Global_ACCESS(pcurrent_object_list_item)->texmatrix_2, mtxFinal);
				}
				else
				{
					mtxFinal[0][0] = -GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Ix * 0.5f;
					mtxFinal[0][1] = -GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Jx * 0.5f;
					mtxFinal[0][2] = -GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Kx * 0.5f;
					mtxFinal[1][0] = -GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Iy * 0.5f;
					mtxFinal[1][1] = -GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Jy * 0.5f;
					mtxFinal[1][2] = -GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Ky * 0.5f;
					
					if (!(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Symetric))
					{
						mtxFinal[0][0] = -mtxFinal[0][0];
						mtxFinal[1][0] = -mtxFinal[1][0];
					}
					
					mtxFinal[0][3] = 0.5f;
					mtxFinal[1][3] = 0.5f;
				}
								
				if(GXI_Global_ACCESS(bPutObjectInObjectList))
				{
					MTXCopy(mtxFinal, GXI_Global_ACCESS(current_object_list_item).texmatrix_2);
					if(GXI_Global_ACCESS(bPutObjectInObjectList))
					{
						g_bUVGenFromNrm = TRUE;
						return;
					}
						
				}
				
				GX_GXLoadTexMtxImm(mtxFinal, GX_TEXMTX0,  GX_MTX2x4);
				
				if(	!bUseUVMatrix )
				{
					last = 1;
lastchrome1:		GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_NRM, GX_TEXMTX0 ,GX_FALSE, GX_PTIDENTITY);					
					g_bUVGenFromNrm = TRUE;
					return;
				}

				last = 2;
				GX_GXLoadTexMtxImm(mtx, GX_PTTEXMTX0, GX_MTX3x4);
lastchrome2:	GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_NRM, GX_TEXMTX0, GX_FALSE, GX_PTTEXMTX0);
				g_bUVGenFromNrm = TRUE;
				return;
			}
		break;
		
		case 6: //MAT_UV_Compute_PLANAR_GIZMO_NOSTORE,
				GXI_Compute_Planar_UV((GEO_tdst_Object*)GDI_gpst_CurDD->pst_CurrentGameObject, pst_Element, CurrentAddFlag);
				
				if(GXI_Global_ACCESS(bPutObjectInObjectList))
				{
					g_bUVGenFromPos = TRUE;
					return;
				}
				
				if(	!bUseUVMatrix )
				{
					last = 3;
lastplanar1:		GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_POS, GX_TEXMTX0 ,GX_FALSE, GX_PTIDENTITY);
					g_bUVGenFromPos = TRUE;
					return;
				}

				last = 4;
				GX_GXLoadTexMtxImm(mtx, GX_PTTEXMTX0, GX_MTX3x4);
lastplanar2:	GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_POS, GX_TEXMTX0, GX_FALSE, GX_PTTEXMTX0);
				g_bUVGenFromPos = TRUE;
				return;
		break;
		
		case 3: //MAT_UV_Compute_DF_GIZMO, used?
		case 4: //MAT_UV_Compute_PHONG_GIZMO, used?
		case 7: //MAT_UV_Compute_FACE_MAP, used?	
		case 8: //MAT_UV_Compute_FOGZZ, used?


		case 0: //MAT_UV_Compute_OBJ1,
		case 1: //MAT_UV_Compute_OBJ2,		
		case 9: //MAT_UV_Compute_Default
		default:	
				if(GXI_Global_ACCESS(bPutObjectInObjectList))
					return;
				
				if(!bUseUVMatrix)
				{
					last = 5;
lastdefault1:		GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY ,GX_FALSE, GX_PTIDENTITY);
					return;
				}
				
				last = 6;
				GX_GXLoadTexMtxImm(mtx, GX_TEXMTX0,  GX_MTX2x4);
lastdefault2:	GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0 ,GX_FALSE, GX_PTIDENTITY);
				return;
		break;
	}
}

/*static float GXI_fFogEndCoef = 1.f;
static float GXI_fFogNearCoef = 0.05f;
static float GXI_fFogStartCoef = 1.f;
static float GXI_fFogFarCoef = 1.f;
static BOOL  GXI_bDivStart = FALSE;
static BOOL  GXI_bDivEnd = FALSE;
static BOOL  GXI_bDivNear = FALSE;
static BOOL  GXI_bDivFar = FALSE;
static BOOL  GXI_bMulStart = FALSE;
static BOOL  GXI_bMulEnd = FALSE;
static BOOL  GXI_bMulNear = FALSE;
static BOOL  GXI_bMulFar = FALSE;*/

void GXI_set_fog(BOOL _enable)
{
	if(GXI_Global_ACCESS(bUseMegaFogOveride))
	{
		if(!!GXI_Global_ACCESS(FogParams).FogForceDisable)
#ifdef GIGSCALE0		
			GXSetFog(GXI_Global_ACCESS(MegaFogParams).FogType, GXI_Global_ACCESS(MegaFogParams).FogStart*GIGSCALE0, GXI_Global_ACCESS(MegaFogParams).FogEnd*GIGSCALE0, (GDI_gpst_CurDD->st_Camera.f_NearPlane/20.0f)*GIGSCALE0, max((GDI_gpst_CurDD->st_Camera.f_NearPlane/20.0f)*GIGSCALE0+0.1f, GDI_gpst_CurDD->st_Camera.f_FarPlane*GXI_Global_ACCESS(MegaFogParams).FogDensity), GXI_Global_ACCESS(MegaFogParams).FogColor); 
#else
			GXSetFog(GXI_Global_ACCESS(MegaFogParams).FogType, GXI_Global_ACCESS(MegaFogParams).FogStart, GXI_Global_ACCESS(MegaFogParams).FogEnd, GDI_gpst_CurDD->st_Camera.f_NearPlane, max(GDI_gpst_CurDD->st_Camera.f_NearPlane+0.1f, GDI_gpst_CurDD->st_Camera.f_FarPlane*GXI_Global_ACCESS(MegaFogParams).FogDensity*2), GXI_Global_ACCESS(MegaFogParams).FogColor); 
#endif			
	}
	else
	{
		if(_enable && GXI_Global_ACCESS(FogParams).FogDensity)// && !GXI_Global_ACCESS(FogParams).FogForceDisable)
		{
#ifdef GIGSCALE0		
			GXSetFog(GXI_Global_ACCESS(FogParams).FogType, GXI_Global_ACCESS(FogParams).FogStart, GXI_Global_ACCESS(FogParams).FogEnd*GIGSCALE0*0.70f, (GDI_gpst_CurDD->st_Camera.f_NearPlane), max((GDI_gpst_CurDD->st_Camera.f_NearPlane)+0.1f, (GDI_gpst_CurDD->st_Camera.f_FarPlane*GIGSCALE0)*GXI_Global_ACCESS(FogParams).FogDensity), GXI_Global_ACCESS(FogParams).FogForceDisable ? GX_BLACK : GXI_Global_ACCESS(FogParams).FogColor); 
#else
//			GXSetFog(GXI_Global_ACCESS(FogParams).FogType, GXI_Global_ACCESS(FogParams).FogStart/20.0f, GXI_Global_ACCESS(FogParams).FogEnd/20.0f, (GDI_gpst_CurDD->st_Camera.f_NearPlane/20.0f), max((GDI_gpst_CurDD->st_Camera.f_NearPlane/20.0f)+0.1f, (GDI_gpst_CurDD->st_Camera.f_FarPlane200.0f)*GXI_Global_ACCESS(FogParams).FogDensity/**GXI_Global_ACCESS(FogParams).FogDensity*5.0f*/), GXI_Global_ACCESS(FogParams).FogColor); 
//			GXSetFog(GXI_Global_ACCESS(FogParams).FogType, GXI_Global_ACCESS(FogParams).FogStart, GXI_Global_ACCESS(FogParams).FogEnd/1.5f, (GDI_gpst_CurDD->st_Camera.f_NearPlane/20.0f), max((GDI_gpst_CurDD->st_Camera.f_NearPlane/20.0f)+0.1f, (GDI_gpst_CurDD->st_Camera.f_FarPlane/20.0f)*(1.0f+GXI_Global_ACCESS(FogParams).FogDensity)), GXI_Global_ACCESS(FogParams).FogColor); 
			GXColor color;
			if (GXI_Global_ACCESS(FogParams).FogForceDisable)
				color = GX_BLACK;
			else
			{
				color = GXI_Global_ACCESS(FogParams).FogColor;
				// Div fog color by 2 because of mul2X
				if (g_bUseHWMul2x)
				{
					color.r = color.r >> 1;
					color.g = color.g >> 1;
					color.b = color.b >> 1;
				}
			}

			GXSetFog(	GXI_Global_ACCESS(FogParams).FogType, 
						GXI_Global_ACCESS(FogParams).FogStart, 
						GXI_Global_ACCESS(FogParams).FogEnd, 
						GDI_gpst_CurDD->st_Camera.f_NearPlane*0.05f, 
						GDI_gpst_CurDD->st_Camera.f_FarPlane, 
						color);
						
			/*float fFogStart = GXI_Global_ACCESS(FogParams).FogStart*GXI_fFogStartCoef;
			float fFogEnd = GXI_Global_ACCESS(FogParams).FogEnd*GXI_fFogEndCoef;
			float fFogNear = GDI_gpst_CurDD->st_Camera.f_NearPlane*GXI_fFogNearCoef;
			float fFogFar = GDI_gpst_CurDD->st_Camera.f_FarPlane*GXI_fFogFarCoef;
			
			if (GXI_bDivStart) fFogStart /= GXI_Global_ACCESS(MegaFogParams).FogDensity;
			if (GXI_bDivEnd) fFogEnd /= GXI_Global_ACCESS(MegaFogParams).FogDensity;
			if (GXI_bDivNear) fFogNear /= GXI_Global_ACCESS(MegaFogParams).FogDensity;
			if (GXI_bDivFar) fFogFar /= GXI_Global_ACCESS(MegaFogParams).FogDensity;
			if (GXI_bMulStart) fFogStart *= GXI_Global_ACCESS(MegaFogParams).FogDensity;
			if (GXI_bMulEnd) fFogEnd *= GXI_Global_ACCESS(MegaFogParams).FogDensity;
			if (GXI_bMulNear) fFogNear *= GXI_Global_ACCESS(MegaFogParams).FogDensity;
			if (GXI_bMulFar) fFogFar *= GXI_Global_ACCESS(MegaFogParams).FogDensity;
			
			GXSetFog(	GXI_Global_ACCESS(FogParams).FogType, 
						fFogStart, 
						fFogEnd, // On fait reculer le fog pour les faibles densités.
						fFogNear,
						fFogFar, 
						GXI_Global_ACCESS(FogParams).FogForceDisable ? GX_BLACK : GXI_Global_ACCESS(FogParams).FogColor); 
						*/
#endif			
		}
		else
		{
			GXSetFog( GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, GX_BLACK );
		}
	}
}

void GXI_set_draw_vertex_properties(GXAttrType _bUsePos, GXAttrType _bUseNrm, GXAttrType _bUseClr, GXAttrType _bUseTex)
{
	GX_GXSetVtxDesc(GX_VA_POS,   _bUsePos);
	GX_GXSetVtxDesc(GX_VA_NRM,   _bUseNrm);
	GX_GXSetVtxDesc(GX_VA_CLR0,  _bUseClr);
	GX_GXSetVtxDesc(GX_VA_TEX0,  _bUseTex);
}
extern u32 g_ShadowPass;

BOOL GXI_bDivColorStage1 = FALSE;
void GXI_set_color_generation_parameters(BOOL _bUseClr, BOOL _bUseTex)
{
	if (g_ShadowPass) return;
	
	GX_GXSetNumTexGens((_bUseTex ? 1 : 0));
	if(GXI_Global_ACCESS(LightMask) == GX_LIGHT_NULL || !(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Lighted) || g_bLightOff)
	{
		// No light
		GX_GXSetNumChans((_bUseClr ?  1 : 0));
		// color_tev0 = (color_tex ou 1) * color_vtx. 
		// alpha_tev0 = (alpha_tex ou 1) * alpha_vtx. 
		GX_GXSetNumTevStages(1);
		GX_GXSetTevOrder(GX_TEVSTAGE0, (_bUseTex ? GX_TEXCOORD0 : GX_TEXCOORD_NULL), (_bUseTex ? GX_TEXMAP0 : GX_TEXMAP_NULL), _bUseClr ? GX_COLOR0A0 : GX_COLOR_NULL);	
		GX_GXSetTevOp(GX_TEVSTAGE0, (_bUseTex && _bUseClr) ? GX_MODULATE : (_bUseClr ? GX_PASSCLR : GX_REPLACE));
	}
	else
	{
		// Use light
		GX_GXSetNumChans((_bUseClr ? 2 : 0));  
		   
		// The second stage is used for light
		GX_GXSetNumTevStages(2);//(GXI_Global_ACCESS(LightMask) != GX_LIGHT_NULL) ? 2 : 1);

		// Stage 0 : the rasterized color is the vertex color (see GXI_prepare_to_draw_material)
		// vertex color is set by MAT_DrawIndexedSprites_MT. It always exists (_bUseClr is always true).
		// It comes either from the rli, the real vertex color, the ambiant color, or by default is set to 0.
		// color_tev0 = (color_tex ou 1) * color_vtx. 
		// alpha_tev0 = (alpha_tex ou 1) * alpha_vtx. 
		GX_GXSetTevOrder(GX_TEVSTAGE0, (_bUseTex ? GX_TEXCOORD0 : GX_TEXCOORD_NULL), (_bUseTex ? GX_TEXMAP0 : GX_TEXMAP_NULL), _bUseClr ? GX_COLOR0A0 : GX_COLOR_NULL);	
		GX_GXSetTevOp(GX_TEVSTAGE0, (_bUseTex && _bUseClr) ? GX_MODULATE : (_bUseClr ? GX_PASSCLR : GX_REPLACE));
				
		// Stage 1 : the rasterized color is ambiant + lights (see GXI_prepare_to_draw_material)
		// ColorRas = Color1 = color from light and ambiant (cf GX_GXSetChanCtrl(  GX_COLOR1A1,...) lower)
		GX_GXSetTevOrder(GX_TEVSTAGE1, (_bUseTex ? GX_TEXCOORD0 : GX_TEXCOORD_NULL), (_bUseTex ? GX_TEXMAP0 : GX_TEXMAP_NULL), _bUseClr ? GX_COLOR1A1 : GX_COLOR_NULL);

		// color_tev1 = (color_tev0 + ((color_tex ou 1) * color_light_plus_ambiant) ) * scale;
		// Combinaison des 2 stages :
		// color_tev1 = (color_tex ou 1) * ((color_vtx ou 1) + (color_light_plus_ambiant)) * scale
		GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, _bUseTex ? GX_CC_TEXC : GX_CC_ONE, GX_CC_RASC, GX_CC_CPREV);
		GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO,GX_CS_DIVIDE_2, 1, GX_TEVPREV);
//		GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, /*GXI_bDivColorStage1 ? GX_CS_DIVIDE_2 : */GX_CS_SCALE_1, 1, GX_TEVPREV);
		
		// alpha_tev1 = alpha_tev0;
		GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
		GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);
		
		GX_GXSetTevOpDirty();
	}
}

void GXI_prepare_to_draw_material(GXColorSrc _MatSrc)
{
#ifdef USE_HARDWARE_LIGHTS
	GXColor Amb = {0,0,0,255};
	if(GXI_Global_ACCESS(LightMask) == GX_LIGHT_NULL || !(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Lighted) || g_bLightOff)
	{
		GX_GXSetChanCtrl(  GX_COLOR0A0,
		    			   GX_DISABLE,		// enable Channel
		    			   GX_SRC_VTX,		// amb source
		    			   _MatSrc,		// mat source
		    			   GX_LIGHT_NULL,	// light mask
		    			   GX_DF_CLAMP,		// diffuse function
		    			   GX_AF_NONE );	// atten   function
		    			   
    }
	else
	{
		if(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_UseAmbient)
		{
			u32 ul_Ambient;
			
			if (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_DontUseAmbient2)
				ul_Ambient = GDI_gpst_CurDD->pst_World->ul_AmbientColor;
			else
				ul_Ambient = GDI_gpst_CurDD->pst_World->ul_AmbientColor2;
				
			Amb.r = ul_Ambient & 0x000000FF;
			Amb.g = ul_Ambient>>8 & 0x000000FF;
			Amb.b = ul_Ambient>>16 & 0x000000FF;
		}
		    			       			   
	    GX_GXSetChanMatColor(GX_COLOR1A1, GX_WHITE);
	    GX_GXSetChanAmbColor(GX_COLOR1A1, Amb);

	    //_GXSetChanMatColor(GX_COLOR0A0, GX_BLACK);
	    //_GXSetChanAmbColor(GX_COLOR0A0, GX_BLACK);

		// RASC 0 is vertex color
		// RASC 1 is ambiant + lights
		
		GX_GXSetChanCtrl(  GX_COLOR0A0,
    			   GX_DISABLE,		// enable Channel
    			   GX_SRC_VTX,		// amb source
    			   _MatSrc,		// mat source
    			   GX_LIGHT_NULL,	// light mask
    			   GX_DF_NONE,		// diffuse function
    			   GX_AF_NONE );	// atten   function

		GX_GXSetChanCtrl(  GX_COLOR1A1,
    			   GX_ENABLE,		// enable Channel
    			   GX_SRC_REG,		// amb source
    			   GX_SRC_REG,		// mat source
    			   GXI_Global_ACCESS(LightMask),	// light mask
    			   GX_DF_CLAMP,		// diffuse function
    			   GX_AF_SPOT );	// atten   function

/*		GX_GXSetChanCtrl( GX_COLOR0,
	    			   GX_DISABLE,		// enable Channel    			   
	    			   GX_SRC_REG,		// amb source
	    			   GX_SRC_VTX,		// mat source
	    			   GX_LIGHT_NULL,	// light mask
	    			   GX_DF_CLAMP,		// diffuse function
	    			   GX_AF_NONE );	// atten   function

	    GX_GXSetChanCtrl( GX_ALPHA0,
	    			   GX_DISABLE,		// enable Channel    			   
	    			   GX_SRC_VTX,		// amb source
	    			   GX_SRC_VTX,		// mat source
	    			   GX_LIGHT_NULL,	// light mask
	    			   GX_DF_CLAMP,		// diffuse function
	    			   GX_AF_NONE );	// atten   function

		GX_GXSetChanCtrl( GX_COLOR1,
	    			   GX_ENABLE,		// enable Channel    			   
	    			   GX_SRC_REG,		// amb source
	    			   GX_SRC_REG,		// mat source
	    			   GXI_Global_ACCESS(LightMask),	// light mask
	    			   GX_DF_CLAMP,		// diffuse function
	    			   GX_AF_NONE );	// atten   function
	    	
		GX_GXSetChanCtrl( GX_ALPHA1,
	    			   GX_DISABLE,		// enable Channel    			   
	    			   GX_SRC_VTX,		// amb source
	    			   GX_SRC_VTX,		// mat source
	    			   GX_LIGHT_NULL,	// light mask
	    			   GX_DF_CLAMP,		// diffuse function
	    			   GX_AF_NONE );	// atten   function//*/
	}
#else
	GX_GXSetChanCtrl(  GX_COLOR0A0,
	    			   GX_DISABLE,		// enable Channel
	    			   GX_SRC_VTX,		// amb source
	    			   GX_SRC_VTX,		// mat source
	    			   GX_LIGHT_NULL,	// light mask
	    			   GX_DF_CLAMP,		// diffuse function
	    			   GX_AF_NONE );	// atten   function
#endif    			   
}

//		GXColor1u32(((ulGXISetCol & 0x000000ff)<<24) & 0xff000000 | ((ulGXISetCol & 0x0000ff00)<<8) & 0x00ff0000); \
//		GXColor1u32(((ulGXISetCol & 0x00ff0000)<<8) & 0xff000000 | ((ulGXISetCol & 0xff000000)>>8) & 0x00ff0000); \

#define GXI_SetCurrentColorRGBA(a) \
	if(pst_Color) \
	{ \
		ulGXISetCol = pst_Color[a] | ulGXISetCol_Or; \
		ulGXISetCol ^= ulGXISetCol_XOr; \
		if(pst_Alpha) \
		{ \
			ulGXISetCol &= 0x00ffffff; \
			ulGXISetCol |= pst_Alpha[a]; \
		} \
	} \
	else if(pst_Alpha) \
	{ \
		ulGXISetCol &= 0x00ffffff; \
		ulGXISetCol |= pst_Alpha[a]; \
	}

#define GXI_SendCurrentColor() \
	GXColor1u32((ulGXISetCol & 0x000000ff)<<24 | (ulGXISetCol & 0x0000ff00)<<8 | (ulGXISetCol & 0x00ff0000)>>8 | (ulGXISetCol & 0xff000000)>>24);

#define GXI_SetColorRGBA(a) \
	GXI_SetCurrentColorRGBA(a) \
	GXI_SendCurrentColor()
	

#define GXI_ConvertColor(color) (((color) & 0x000000ff)<<24 | ((color) & 0x0000ff00)<<8 | ((color) & 0x00ff0000)>>8 | ((color) & 0xff000000)>>24)


#define GXI_SetVertexXYZ(P,N)\
	if ((P) && (N))\
		if (pst_Color){\
			u32Tmp = (ulGXISetCol)>>4*6;\
			offsetN2 = offsetN*u32Tmp / 256.0f;\
			MATH_MulAddScalarVector( &vecTmp, N, P, offsetN2 ); \
			GXPosition3f32(*vecTmp,vecTmp[1],vecTmp[2]);\
		}else{\
			MATH_MulAddScalarVector( &vecTmp, N, P, offsetN ); \
			GXPosition3f32(*vecTmp,vecTmp[1],vecTmp[2]);\
		}

#define GXI_SetVertexUV(uv)\
		{\
			GXTexCoord2f32(\
			((float*) (uv))[0] + offSetU,\
			((float*) (uv))[1] + offSetV);\
		}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GXI_l_DrawElementIndexedTrianglesShadows
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_IndexedTriangle	*t, *tend;
	BOOL						bStrip;
//	GEO_tdst_OneStrip			*pStrip, *pStripEnd;
	u32							ReturnValue;
	f32 vp[GX_VIEWPORT_SZ];
	f32 pm[GX_PROJECTION_SZ];
	u32 oldScissorXOrig, oldScissorYOrig, oldScissorWidth, oldScissorHeight;
	extern BOOL	g_bIMustGetBack;
	extern BOOL	g_bIMustCLear;
	extern s32 g_currentShadowNum2Set;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GXI_EndRaster(3);
	
	GXI_BeginRaster(3);

	
    GX_GXSetColorUpdate(GX_TRUE);
	GX_GXSetAlphaUpdate(GX_FALSE);
	

	// save the context
	GXGetScissor(&oldScissorXOrig, &oldScissorYOrig, &oldScissorWidth, &oldScissorHeight);		
	GXGetViewportv(vp);
	GXGetProjectionv(pm);

	GXSetProjection(GXI_Global_ACCESS(orthogonal2D_matrix), GX_ORTHOGRAPHIC);					
	GXSetViewport(512.0f, (float)(FrameBufferHeight - SHADOW_RESOLUTION * 2), SHADOW_RESOLUTION*2, SHADOW_RESOLUTION*2, 0.0f, GX_MAX_Z24);

	t = _pst_Element->dst_Triangle;
	tend = t + _pst_Element->l_NbTriangles;

	if(_pst_Element->pst_StripData != NULL)
	{
		if(_pst_Element->pst_StripData->ulFlag & GEO_C_Strip_DataValid)
			bStrip = TRUE;
		else
			bStrip = FALSE;
	}
	else
		bStrip = FALSE;

	GX_GXSetCullMode(GX_CULL_NONE);
	
	GXI_set_fog(FALSE);
	
	GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);	
	
	GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP);//copy
	
	GX_GXSetChanCtrl(  GX_COLOR0A0,
				   GX_DISABLE,		// enable Channel
				   GX_SRC_REG,		// amb source
				   GX_SRC_REG,		// mat source
				   GX_LIGHT_NULL,		// light mask
				   GX_DF_CLAMP,		// diffuse function
				   GX_AF_NONE );	// atten   function

	GX_GXSetChanAmbColor( GX_COLOR0A0, GX_BLACK );
	GX_GXSetChanMatColor( GX_COLOR0A0, GX_WHITE );	

	// multi pass shadow, draw previous shadow texture for this shadow
	if(g_bIMustGetBack || g_bIMustCLear)
	{
		extern GXTexObj g_ShadowTextures[];
		float fSize;
/*
#ifndef USE_FULL_SCENE_SHADOWS      
#ifdef USE_MY_TEX_REGION
		GXInvalidateTexRegion(&MyTexRegions[0]);
#else
   		GXInvalidateTexAll(); 
#endif
#endif    */
		GXLoadPosMtxImm(GXI_Global_ACCESS(identity_projection_matrix), GX_PNMTX0);
		
		GXSetScissor(512.0f, (float)(FrameBufferHeight - SHADOW_RESOLUTION * 2), (SHADOW_RESOLUTION * 2), (SHADOW_RESOLUTION * 2));
		GXI_set_draw_vertex_properties(GX_DIRECT, GX_NONE, GX_NONE, GX_DIRECT);

		GX_GXSetNumChans(1);     
		GX_GXSetNumTexGens(1);
		GX_GXSetNumTevStages(1);
		if (g_bIMustCLear)
		{
			GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
			GX_GXSetChanAmbColor( GX_COLOR0A0, GX_BLACK );
			GX_GXSetChanMatColor( GX_COLOR0A0, GX_BLACK );	
			fSize = 2.0f;
		} else
		{
			GXInvalidateTexAll(  ); 					
			GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);	
			GX_GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
			GX_GXLoadTexObj(&g_ShadowTextures[g_currentShadowNum2Set], GX_TEXMAP0);
			fSize = 1.0f;
		}

		GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY ,GX_FALSE, GX_PTIDENTITY);
		GX_GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_NEVER, 0);
		GX_GXSetZCompLoc(GX_FALSE);
				
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(-fSize, -fSize, 0.0f);
			GXTexCoord2f32(0,0);
			GXPosition3f32(fSize, -fSize, 0.0f);
			GXTexCoord2f32(1,0);
			GXPosition3f32(fSize, fSize, 0.0f);
			GXTexCoord2f32(1,1);
			GXPosition3f32(-fSize, fSize, 0.0f);
			GXTexCoord2f32(0,1);
		GXEnd();		
		GX_GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
		g_bIMustGetBack = g_bIMustCLear = FALSE;
	}//*/

	GXSetScissor(512.0f + 4.0f, 4.0f + (float)(FrameBufferHeight - SHADOW_RESOLUTION * 2), (SHADOW_RESOLUTION * 2)-8.0f, (SHADOW_RESOLUTION * 2)-8.0f);
	
	// DJ_TEMP	
	GX_GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_NEVER, 0);
	GX_GXSetZCompLoc(GX_TRUE);


	GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(0);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);	

	GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	GX_GXSetChanAmbColor( GX_COLOR0A0, GX_WHITE );
	GX_GXSetChanMatColor( GX_COLOR0A0, GX_WHITE );	

	GXI_set_draw_vertex_properties(GX_DIRECT, GX_NONE, GX_NONE, GX_NONE);
	
	// current modelview matrix is set in the rendershadow fonction 
	GXLoadPosMtxImm(*(Mtx*)&GXI_Global_ACCESS(current_modelview_matrix), GX_PNMTX0);
	
	if(_pst_Element->dl)
		ReturnValue = GXI_l_DrawElementIndexedTrianglesUsingDL(_pst_Element,_pst_Point,_pst_UV,ulnumberOfPoints);
	else
		ReturnValue = GXI_l_DrawElementIndexedTrianglesDirectly(_pst_Element,_pst_Point,_pst_UV,ulnumberOfPoints);		

	
	
	// revert to the saved context
	GXSetProjectionv(pm);
	GXSetScissor(oldScissorXOrig, oldScissorYOrig, oldScissorWidth, oldScissorHeight);		
	GXSetViewportv(vp);	
	
	return ReturnValue;
} 


LONG GXI_l_DrawElementIndexedTrianglesDirectly
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_IndexedTriangle	*t, *tend;
	u32							*pst_Color;
	u32							*pst_Alpha;
	GEO_Vertex					*pst_Normals;
	u32							ulGXISetCol_Or;
	u32							ulGXISetCol_XOr;
	u32							ulGXISetCol;
	u32							i;
#ifdef D_USE_AFTERFX_MASK	
	BOOL bAfterFXMaskUpdate = FALSE;	
	f32  vp[GX_VIEWPORT_SZ];
	u32  oldScissorXOrig, oldScissorYOrig, oldScissorWidth, oldScissorHeight;
#endif	
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	

	ulGXISetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
	ulGXISetCol_Or = GXI_Global_ACCESS(ulColorOr);
	
	t = _pst_Element->dst_Triangle;
	tend = t + _pst_Element->l_NbTriangles;

	pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
	pst_Alpha = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentAlphaField;


#ifdef USE_SOFT_UV_MATRICES
#ifndef _FINAL_
	if(!g_bUseHardwareTextureMatrices)
#endif	
		GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY ,GX_FALSE, GX_PTIDENTITY);
#endif	
	
	pst_Normals = GDI_gpst_CurDD->pst_CurrentGeo->dst_PointNormal;

#ifdef USE_HARDWARE_LIGHTS
	if(!pst_Normals)
		g_bLightOff = TRUE;
#endif

	if(!g_bUVGenFromNrm 
#ifdef USE_HARDWARE_LIGHTS
	&& g_bLightOff
#endif
	)
		pst_Normals = NULL;


	if (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_TestBackFace)
	{
		if (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotInvertBF)
			GX_GXSetCullMode(GX_CULL_FRONT);
		else
			GX_GXSetCullMode(GX_CULL_BACK);
	} 
	else
	{
		GX_GXSetCullMode(GX_CULL_NONE);
	}

	GX_GXSetZMode((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest) ? GX_ENABLE : GX_DISABLE, (GXCompare)-1, -1);
	
	GXI_set_fog((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged)!=0);
	
	GXI_set_draw_vertex_properties(GX_DIRECT, pst_Normals ? GX_DIRECT : GX_NONE, GX_DIRECT, (GDI_gpst_CurDD->LastTextureUsed!=-1 && _pst_UV && !(g_bUVGenFromPos||g_bUVGenFromNrm)) ? GX_DIRECT : GX_NONE);
	
	GXI_prepare_to_draw_material(GX_SRC_VTX);

	GXI_set_color_generation_parameters(TRUE, (GDI_gpst_CurDD->LastTextureUsed!=-1));

	if(!pst_Color)
	{
		//return; // DJ_TEMP
		ulGXISetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulGXISetCol_Or;
	    ulGXISetCol ^= ulGXISetCol_XOr;
	}
#ifdef D_USE_AFTERFX_MASK	
	else
	{
		if((pst_Color[0]>>24)==1)
		{
			bAfterFXMaskUpdate = TRUE;
			
			// save the context
			GXGetScissor(&oldScissorXOrig, &oldScissorYOrig, &oldScissorWidth, &oldScissorHeight);		
			GXGetViewportv(vp);
			
			GXSetViewport(512.0f, 128.0f, 128.0f, 128.0f, 0.0f, GX_MAX_Z24);
			GXSetScissor(512.0f, 128.0f, 128.0f, 128.0f);
		}
	}
#endif	

	
	//---------------------------------------------------------------
	// indexed triangles mode
	//---------------------------------------------------------------
	{
       	GXBegin(GX_TRIANGLES, GX_VTXFMT0, _pst_Element->l_NbTriangles*3);

		while(t < tend)
		{     
			
			for(i=0;i<3;i++)
			{   
	        	GXPosition3f32(_pst_Point[t->auw_Index[i]].x, _pst_Point[t->auw_Index[i]].y, _pst_Point[t->auw_Index[i]].z);
	
				if(pst_Normals)
				{
		        	GXNormal3f32(pst_Normals[t->auw_Index[i]].x, pst_Normals[t->auw_Index[i]].y, pst_Normals[t->auw_Index[i]].z);
				}		        	
	   	    	
	   	    	GXI_SetColorRGBA(t->auw_Index[i]);
	   	    	
	   	    	if(GDI_gpst_CurDD->LastTextureUsed!=-1 && _pst_UV && !(g_bUVGenFromPos||g_bUVGenFromNrm))
	   	    	{
	   	    		if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
	   	    			GXTexCoord2f32(_pst_UV[t->auw_Index[i]].fU, _pst_UV[t->auw_Index[i]].fV);
	   	    		else
	   	    			GXTexCoord2f32(_pst_UV[t->auw_UV[i]].fU, _pst_UV[t->auw_UV[i]].fV);
	   	    	}
			}

   	    	t++;
   	    }
   	    	
   	    GXEnd();
	}

	g_bUVGenFromPos = FALSE;
	g_bUVGenFromNrm = FALSE;
	
#ifndef _FINAL_
	if(g_bShowNormals)
		GXI_DBG_DrawObjectNormals(_pst_Element,_pst_Point,ulnumberOfPoints);
		
	if(g_bShowSkinElements)
		GXI_DBG_ShowSkinElements(_pst_Element,_pst_Point,ulnumberOfPoints);
		
	if(g_bShowVertexColors)
		GXI_DBG_ShowVertexColors(_pst_Element,_pst_Point,ulnumberOfPoints);
#endif	
	
	return _pst_Element->l_NbTriangles;
}

_inline_ void SwapAndStore(register u32 *pAddress,register u32 Value)
{
	register u32 zero = 0;

	asm
	{
		stwbrx 	Value,pAddress,zero
	}
}
void GXI_v_MemCpy_WGP(u32 *pDest , u32 *pSRC , u32 ulNumber)
{
	register u32 *pPipe;
	register u32 ulGXISetCol;
	pPipe = (u32 *)GXRedirectWriteGatherPipe( (void *)pDest );
	{
		u32 *pSRCLST;
		pSRCLST = pSRC + ulNumber;
		while (pSRC < pSRCLST) *pPipe = *(pSRC++);
	}
	ulGXISetCol = 32;
	while (ulGXISetCol--) *(unsigned char *)pPipe = 0;
	GXRestoreWriteGatherPipe ( );
}
void GXI_ComputeRedirectBuffer(u16 *aus_RedirectBuffer,u16 *pus_ListOfUsedIndex , u32 ul_NumberOfUsedIndex)
{
	register u16 Counter;
	register u16 *pus_ListOfUsedIndexLast;
	Counter = 0;
	pus_ListOfUsedIndexLast = pus_ListOfUsedIndex + ul_NumberOfUsedIndex;
	while(pus_ListOfUsedIndex < pus_ListOfUsedIndexLast)
		aus_RedirectBuffer[*(pus_ListOfUsedIndex++)] = Counter++;
}

void GXI_v_CopyColors_UI(u32 *pColorsSRC , u32 *pAlphasSRC , u32 *pColorsDST , u32 cOR , u32 cXOR , u16 *p_UsedIndexes , u32 ulNumber)
{
	register u32 ulGXISetCol;
	u32 *pPipe;
	pPipe = (u32 *)GXRedirectWriteGatherPipe( (void *)pColorsDST );
	if(pAlphasSRC) 
	{
		u16 *p_UsedIndexesLST;
		p_UsedIndexesLST = p_UsedIndexes + ulNumber;
		while (p_UsedIndexes < p_UsedIndexesLST) 
		{ 
			ulGXISetCol = pColorsSRC[*p_UsedIndexes] | cOR; 
			ulGXISetCol ^= cXOR; 
			ulGXISetCol &= 0x00ffffff; 
			ulGXISetCol |= pAlphasSRC[*(p_UsedIndexes++)]; 
			SwapAndStore(pPipe,ulGXISetCol);
		} 
	}
	else
	{
		u16 *p_UsedIndexesLST;
		p_UsedIndexesLST = p_UsedIndexes + ulNumber;
		while (p_UsedIndexes < p_UsedIndexesLST) 
			SwapAndStore(pPipe,(pColorsSRC[*(p_UsedIndexes++)] | cOR) ^ cXOR);
	}//*/
	ulGXISetCol = 32;
	while (ulGXISetCol--) *(unsigned char *)pPipe = 0;
	GXRestoreWriteGatherPipe ( );
//	DCFlushRange(pColorsDST, ulNumber*4);	
}
void GXI_v_CopyColors(u32 *pColorsSRC , u32 *pAlphasSRC , u32 *pColorsDST , u32 cOR , u32 cXOR , u32 ulNumber)
{
	register u32 ulGXISetCol;
	u32 *pPipe;
	pPipe = (u32 *)GXRedirectWriteGatherPipe( (void *)pColorsDST );
	if(pAlphasSRC) 
	{
		u32 *pSRC,*pALF,*pSRCLST;
		pSRC = pColorsSRC;
		pALF = pAlphasSRC;
		pSRCLST = pSRC + ulNumber;
		while (pSRC < pSRCLST)
		{ 
			ulGXISetCol = *pSRC | cOR; 
			ulGXISetCol ^= cXOR; 
			ulGXISetCol &= 0x00ffffff; 
			ulGXISetCol |= *pALF; 
			SwapAndStore(pPipe,ulGXISetCol);
			pALF++;
			pSRC++;
		} 
	}
	else
	{
		u32 *pSRC,*pSRCLST;
		pSRC = pColorsSRC;
		pSRCLST = pSRC + ulNumber;
		while (pSRC < pSRCLST) 
			SwapAndStore(pPipe,(*(pSRC++) | cOR) ^ cXOR);
	}//*/
	ulGXISetCol = 32;
	while (ulGXISetCol--) *(unsigned char *)pPipe = 0;
	GXRestoreWriteGatherPipe ( );
//	DCFlushRange(pColorsDST, ulNumber*4);	
}

void GXI_FlushLocalGeom()
{
	GXI_Global_ACCESS(vtx_array_LOCAL) = NULL;
	GXI_Global_ACCESS(vtx_array_LOCAL_NRM) = NULL;
}

extern int GXI_StopComputeRefectionDuring2D;
#ifdef DONT_USE_ALPHA_DEST

// METHODE A -> Render Alpha in a specific buffer
// METHODE B -> Reuse Last valide multi-texture stage. HOT


#define ALPHA_TEXTURE_RESOLUTION 128
static u32 ulGXI_PreviousAlphaMode = 0;
void GXI_SetPreviousAlphaMode(u32 Mode)
{
	// 0 -> Nothing , 1 -> Save , 2 -> Use
	ulGXI_PreviousAlphaMode = Mode;
}

void GXI_RenderAlphaInShadfowBuffer(GEO_tdst_ElementIndexedTriangles	*_pst_Element)
{
	static GXTexMtx 		MatrixNumber;
	static GXTexMtxType 	MatrixType;
	static Mtx 				mView;
	
	static GXTexMtx 		MatrixNumber2;
	static GXTexMtxType 	MatrixType2;
	static Mtx 				mView2;
	
	static u32 				NumTevStage;
	static u32 				NumTexGen;
	static GXTexObj 		*LastTextObj;
	// SetTetureCoord2 Parametters
	static GXTexGenType 	P0;
	static GXTexGenSrc 		P1;
	static GXTexMtx 		P2;
	static GXBool 			P3;
	static GXPTTexMtx 		P4;
	
	if ((ulGXI_PreviousAlphaMode) && (!g_ShadowPass)) 
	{
		GX_GXSetNumTexGens(g_GXI_HW_States.u8NumTexGens + 1);
		GX_GXSetNumTevStages(g_GXI_HW_States.u8NumTevStages + 1);
		
		if (P2 == GX_TEXMTX0) 
			P2 = GX_TEXMTX2;

		if (P2 == GX_TEXMTX2)
			GXLoadTexMtxImm(&mView, P2 ,  MatrixType);

		if (P4 == GX_PTTEXMTX0)
		{
			GXLoadTexMtxImm(&mView2, GX_PTTEXMTX1 ,  MatrixType2);
			GXSetTexCoordGen2(GX_TEXCOORD0 + g_GXI_HW_States . u8NumTexGens , P0 , P1 , P2 , P3  , GX_PTTEXMTX1 );
		} else
			GXSetTexCoordGen2(GX_TEXCOORD0 + g_GXI_HW_States . u8NumTexGens , P0 , P1 , P2 , P3 , P4 );
		
		GXLoadTexObj(LastTextObj, GX_TEXMAP2);
		GX_GXSetTevOrder	(GX_TEVSTAGE0 + g_GXI_HW_States . u8NumTevStages , GX_TEXCOORD0 + g_GXI_HW_States.u8NumTexGens, GX_TEXMAP2, GX_COLOR0A0);
		
		GXSetTevColorOp (GX_TEVSTAGE0 + g_GXI_HW_States . u8NumTevStages , GX_TEV_ADD , GX_TB_ZERO , GX_CS_SCALE_1 , GX_TRUE , GX_TEVPREV );
		GXSetTevAlphaOp (GX_TEVSTAGE0 + g_GXI_HW_States . u8NumTevStages , GX_TEV_ADD , GX_TB_ZERO , GX_CS_SCALE_1 , GX_TRUE , GX_TEVPREV );
		// a * (1 - c) + b * c + d 
		GXSetTevColorIn (GX_TEVSTAGE0 + g_GXI_HW_States . u8NumTevStages , GX_CC_ZERO , GX_CC_ZERO , GX_CC_ZERO , GX_CC_CPREV );
		GX_GXSetTevOpDirty();
		
#ifndef LAST_MODIF	
		if ((_pst_Element->dl_size == 0xF0000060) &&
			(_pst_Element->l_MaterialId == 4) &&
			(_pst_Element->l_NbTriangles == 0) &&
			(_pst_Element->ul_NumberOfUsedIndex == 9)) /// Banner detection !! Final slaughter
			GXSetTevAlphaIn (GX_TEVSTAGE0 + g_GXI_HW_States . u8NumTevStages , GX_CA_ZERO , GX_CA_TEXA , GX_CA_RASA , GX_CA_ZERO );	
		else
			GXSetTevAlphaIn (GX_TEVSTAGE0 + g_GXI_HW_States . u8NumTevStages , GX_CA_TEXA , GX_CA_ZERO , GX_CA_ZERO , GX_CA_ZERO );//*/
#else
		GXSetTevAlphaIn (GX_TEVSTAGE0 + g_GXI_HW_States . u8NumTevStages , GX_CA_ZERO , GX_CA_TEXA , GX_CA_RASA , GX_CA_ZERO );
#endif		
	} else
	{ // Save All
		if (!g_GXI_HW_States.AlphaUpdate) return;
		if (g_GXI_HW_States.bmType != GX_BM_NONE) return;
		
		L_memcpy(&mView , &g_sdt_SaveLastLevel . mView , sizeof (Mtx));
		MatrixNumber = g_sdt_SaveLastLevel . MatrixNumber;
		MatrixType = g_sdt_SaveLastLevel . MatrixType;
		L_memcpy(&mView2 , &g_sdt_SaveLastLevel . mView2 , sizeof (Mtx));
		MatrixNumber2 = g_sdt_SaveLastLevel . MatrixNumber2;
		MatrixType2 = g_sdt_SaveLastLevel . MatrixType2;
		
		P0 = g_sdt_SaveLastLevel . P0;
		P1 = g_sdt_SaveLastLevel . P1;
		P2 = g_sdt_SaveLastLevel . P2;
		P3 = g_sdt_SaveLastLevel . P3;
		P4 = g_sdt_SaveLastLevel . P4;
		LastTextObj = g_sdt_SaveLastLevel . LastTextObj;
	}
}

void GXI_UsePreviousAlpha()
{
}

void GXI_EndUsePreviousAlpha()
{
	GX_GXSetNumTevStages(g_GXI_HW_States.u8NumTevStages);
	GX_GXSetNumTexGens(g_GXI_HW_States.u8NumTexGens);
//	GXInvalidateTexAll(  ); 
}


#endif
//u32 MustReturn = 0;
LONG GXI_l_DrawElementIndexedTrianglesUsingDL
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints
)
 {
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_IndexedTriangle	*t, *tend;
	u32							*pst_Color;
	u32							*pst_Alpha;
	GEO_Vertex					*pst_Normals;
	u32							ulGXISetCol_Or;
	u32							ulGXISetCol_XOr;
	u32							ulGXISetCol;
	u32							*col;	
	static u32 					SaveLastColor_Or;
	static u32 					SaveLastColor_Xor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	_GSP_BeginRaster(44);//Setup
	ulGXISetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
	ulGXISetCol_Or = GXI_Global_ACCESS(ulColorOr);

	
#ifdef LAST_MODIF	
	if (!g_ShadowPass)
	{
		if (!ulGXI_PreviousAlphaMode)
		{
			if ((g_GXI_HW_States.AlphaUpdate) && (g_GXI_HW_States.bmType == GX_BM_NONE))
			{
				SaveLastColor_Xor = ulGXISetCol_XOr;
				SaveLastColor_Or  = ulGXISetCol_Or ;
			}
		} else
		{
			// Override alpha field
			ulGXISetCol_XOr &= 0xffffff;
			ulGXISetCol_Or &= 0xffffff ;
			ulGXISetCol_XOr |= SaveLastColor_Xor & 0xff000000;
			ulGXISetCol_Or |= SaveLastColor_Or  & 0xff000000;
		}		
	}
#endif	
	t = _pst_Element->dst_Triangle;
	tend = t + _pst_Element->l_NbTriangles;

	pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
	pst_Alpha = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentAlphaField;

	pst_Normals = GDI_gpst_CurDD->pst_CurrentGeo->dst_PointNormal;

#ifdef USE_HARDWARE_LIGHTS
	if(!pst_Normals)
		g_bLightOff = TRUE;
#endif

	GXI_Global_ACCESS(current_object_list_item).draw_mask = GDI_gpst_CurDD->ul_CurrentDrawMask;
	GXI_Global_ACCESS(current_object_list_item).tex = GDI_gpst_CurDD->LastTextureUsed;
	
	// set arrays ---------------------------------------------------
	if(_pst_Point==GDI_gpst_CurDD->pst_CurrentGeo->dst_Point)
		GXI_Global_ACCESS(current_object_list_item).vtx_array = (ULONG*)_pst_Point;
	else
	{
		if (GXI_Global_ACCESS(vtx_array_LOCAL))
		{
			GXI_Global_ACCESS(current_object_list_item).vtx_array = GXI_Global_ACCESS(vtx_array_LOCAL);
		} else
		{
			GXI_Global_ACCESS(current_object_list_item).vtx_array = (ULONG*)GXI_GetVertexSpace(ulnumberOfPoints*12);
			GXI_Global_ACCESS(vtx_array_LOCAL) = GXI_Global_ACCESS(current_object_list_item).vtx_array;
			GXI_v_MemCpy_WGP(GXI_Global_ACCESS(current_object_list_item).vtx_array , _pst_Point , ulnumberOfPoints*3);
		}
	}	//*/
	GXI_Global_ACCESS(current_object_list_item).vtx_step = 12;
	
	if(!g_bUVGenFromNrm
#ifdef USE_HARDWARE_LIGHTS
	 && g_bLightOff
#endif	 
	 )
		pst_Normals = NULL;
	
	if(pst_Normals)
	{
		if(_pst_Point!=GDI_gpst_CurDD->pst_CurrentGeo->dst_Point)
		{
			if (GXI_Global_ACCESS(vtx_array_LOCAL_NRM))
			{
				GXI_Global_ACCESS(current_object_list_item).nrm_array = GXI_Global_ACCESS(vtx_array_LOCAL_NRM);
			} else
			{
				GXI_Global_ACCESS(current_object_list_item).nrm_array = (ULONG*)GXI_GetVertexSpace(ulnumberOfPoints*12);
				GXI_Global_ACCESS(vtx_array_LOCAL_NRM) = GXI_Global_ACCESS(current_object_list_item).nrm_array;
				GXI_v_MemCpy_WGP(GXI_Global_ACCESS(current_object_list_item).nrm_array , pst_Normals , ulnumberOfPoints*3);
			}
			GXI_Global_ACCESS(current_object_list_item).nrm_step = 12;
		} else//*/
		{
			GXI_Global_ACCESS(current_object_list_item).nrm_array = (ULONG*)pst_Normals;	
			GXI_Global_ACCESS(current_object_list_item).nrm_step = 12;
		}
	}
	else
	{
		GXI_Global_ACCESS(current_object_list_item).nrm_array = (ULONG*)_pst_Point;
		GXI_Global_ACCESS(current_object_list_item).nrm_step = 0;
	}
	

	if(GDI_gpst_CurDD->LastTextureUsed!=-1 && _pst_UV && !(g_bUVGenFromPos||g_bUVGenFromNrm))
	{
		GXI_Global_ACCESS(current_object_list_item).uv_array = (ULONG*)_pst_UV;
//		DCFlushRangeNoSync(GXI_Global_ACCESS(current_object_list_item).uv_array, ulnumberOfPoints*8);
		GXI_Global_ACCESS(current_object_list_item).uv_step = 8;
	}
	else
	{
		GXI_Global_ACCESS(current_object_list_item).uv_array = (ULONG*)_pst_Point;
		GXI_Global_ACCESS(current_object_list_item).uv_step = 0;
	}

	if(!pst_Color)
	{
		col = (u32*)GXI_GetVertexSpace(4);
		ulGXISetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulGXISetCol_Or;
	    ulGXISetCol ^= ulGXISetCol_XOr;
		SwapAndStore(col,ulGXISetCol);
	    DCFlushRangeNoSync(col, 4);		
	    GXI_Global_ACCESS(current_object_list_item).col_step = 0;
	}
	else
	{
		if (_pst_Element->ul_NumberOfUsedIndex)
		{
			GXI_ComputeRedirectBuffer(GDI_gpst_CurDD->pst_ComputingBuffers->aus_RedirectBuffer,_pst_Element->pus_ListOfUsedIndex , _pst_Element->ul_NumberOfUsedIndex);
			col = (u32*)GXI_GetVertexSpace(_pst_Element->ul_NumberOfUsedIndex*4);
			GXI_v_CopyColors_UI(pst_Color , pst_Alpha , col , ulGXISetCol_Or , ulGXISetCol_XOr , _pst_Element->pus_ListOfUsedIndex , _pst_Element->ul_NumberOfUsedIndex);
		} else
		{
			col = (u32*)GXI_GetVertexSpace(ulnumberOfPoints*4);
			GXI_v_CopyColors(pst_Color , pst_Alpha , col , ulGXISetCol_Or , ulGXISetCol_XOr , ulnumberOfPoints);
		}
		GXI_Global_ACCESS(current_object_list_item).col_step = 4;
	}
	
	GXI_Global_ACCESS(current_object_list_item).col_array = col;

	// create object displaylist if inexistant -------------------------------------
		
	_GSP_EndRaster(44);//Setup
	
	// send object immediatly ------------------------------------------------------
	
	{
		GXAttrType IDX_PosNorm , IDX_Color , IDX_Tex; 
		IDX_PosNorm = IDX_Color = IDX_Tex = GX_INDEX16;
		if (_pst_Element->dl_size & 0x80000000) IDX_PosNorm	 = GX_INDEX8;
		if (_pst_Element->dl_size & 0x40000000) IDX_Color	 = GX_INDEX8;
		if (_pst_Element->dl_size & 0x20000000) IDX_Tex		 = GX_INDEX8;
		GXI_set_draw_vertex_properties(IDX_PosNorm, IDX_PosNorm, IDX_Color, IDX_Tex);
	}

	if(!g_bRenderingShadows)
	{
		GXI_prepare_to_draw_material(GX_SRC_VTX);
		GXI_set_color_generation_parameters(TRUE, (GXI_Global_ACCESS(current_object_list_item).tex!=-1));
		GX_GXSetZMode((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest) ? GX_ENABLE : GX_DISABLE, (GXCompare)-1, -1);
		
		if((GXI_Global_ACCESS(current_object_list_item).draw_mask & GDI_Cul_DM_Fogged)!=0)
			GXI_set_fog(TRUE);
		else 
			GXI_set_fog(FALSE);
	}
			
	GX_GXSetArray(GX_VA_POS,  GXI_Global_ACCESS(current_object_list_item).vtx_array, GXI_Global_ACCESS(current_object_list_item).vtx_step);
	GX_GXSetArray(GX_VA_NRM,  GXI_Global_ACCESS(current_object_list_item).nrm_array, GXI_Global_ACCESS(current_object_list_item).nrm_step);
	GX_GXSetArray(GX_VA_CLR0, GXI_Global_ACCESS(current_object_list_item).col_array, GXI_Global_ACCESS(current_object_list_item).col_step);
	GX_GXSetArray(GX_VA_TEX0, GXI_Global_ACCESS(current_object_list_item).uv_array,  GXI_Global_ACCESS(current_object_list_item).uv_step );
	
#ifdef DONT_USE_ALPHA_DEST    
	GXI_RenderAlphaInShadfowBuffer(_pst_Element);
#endif	
	if(!GXI_StopComputeRefectionDuring2D && !g_bRenderingShadows)
	{
		if (GXI_Global_ACCESS(current_object_list_item).draw_mask & GDI_Cul_DM_TestBackFace)
		{
			if (GXI_Global_ACCESS(current_object_list_item).draw_mask & GDI_Cul_DM_NotInvertBF)
				GX_GXSetCullMode(GX_CULL_FRONT);
			else
				GX_GXSetCullMode(GX_CULL_BACK);
		} 
		else
		{
			GX_GXSetCullMode(GX_CULL_NONE);
		}
		GXI_ReverseWorldAndCallDisplayListASecondTime(_pst_Element);
	} else
		GXCallDisplayList(_pst_Element->dl, _pst_Element->dl_size & 0x0fffffff); // Normal	mode

#ifdef DONT_USE_ALPHA_DEST    
	if (ulGXI_PreviousAlphaMode && (!g_ShadowPass)) 
		GXI_EndUsePreviousAlpha();
#endif
	
	g_bUVGenFromPos = FALSE;	
	g_bUVGenFromNrm = FALSE;
	
	return _pst_Element->l_NbTriangles;
}

LONG GXI_l_CloneDrawElementIndexedTriangles
(
 GEO_tdst_ElementIndexedTriangles	*_pst_Element,
 GEO_Vertex					        *_pst_Point,
 MATH_tdst_Vector					*_pst_Normal,
 GEO_tdst_UV						*_pst_UV,
 ULONG								ulnumberOfPoints
 )
{
	/*
		Simple version.
	*/
	OBJ_tdst_GameObject			*_pst_GO;	
	OBJ_tdst_GameObject			*_pst_GOChaine;
	MATH_tdst_Matrix *p_MvObj,st_Current;
	BOOL bUseDL = _pst_Element->dl != NULL;
	int nLoops = 0;
	static u8* TmpDL = NULL;
	const u32 cTmpDLMaxSize = 256*1024;
	u32 TmpDLSize;

	if(!TmpDL)
	{
		TmpDL = (u8*)MEM_p_AllocAlign(cTmpDLMaxSize,64);
		DCInvalidateRange((void*)TmpDL,cTmpDLMaxSize);
	}

	_pst_GO = GDI_gpst_CurDD_SPR.pst_CurrentGameObject;

	if( _pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled )
		return 0;

	ERR_X_Assert(_pst_GO!=NULL);
	_pst_GOChaine = _pst_GO;
	
	MATH_SetIdentityMatrix(&st_Current);
	p_MvObj = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix-1;	
	
	ERR_X_Assert(_pst_Element != NULL);

	do
	{		
		if( _pst_GOChaine->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled )
		{	
			_pst_GOChaine = _pst_GOChaine->p_CloneNextGao;
			continue;
		}

		if(nLoops) //Viex matrix should be set for the first render.	
		{
			MATH_MulMatrixMatrix(&st_Current,_pst_GOChaine->pst_GlobalMatrix,p_MvObj);
			GDI_SetViewMatrix((*GDI_gpst_CurDD),&st_Current);			
		}

		if(bUseDL)
		{
			if(nLoops)
			{
			    //GX_GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
			    //GX_GXSetCullMode(GX_CULL_NONE);
				GXCallDisplayList(_pst_Element->dl, _pst_Element->dl_size & 0x0fffffff);
			}
			else
				GXI_l_DrawElementIndexedTrianglesUsingDL(_pst_Element,_pst_Point,_pst_UV, ulnumberOfPoints );
		}
		else // (bUseDL)
		{
			if(nLoops)
			{
				GXCallDisplayList(TmpDL,TmpDLSize);
			}
			else
			{
				GXBeginDisplayList((void*)TmpDL,cTmpDLMaxSize);
				GXI_l_DrawElementIndexedTrianglesDirectly(_pst_Element,_pst_Point,_pst_UV, ulnumberOfPoints );
				TmpDLSize = GXEndDisplayList();
				DCFlushRange((void*)TmpDL,TmpDLSize);
				GXCallDisplayList(TmpDL,TmpDLSize);
			}
		}

		_pst_GOChaine->ulMoreCloneInfo |= 1; // <- Has Been Drawn
					
		nLoops++;
		// Next game object.
		_pst_GOChaine = _pst_GOChaine->p_CloneNextGao;
	}while (_pst_GOChaine && (_pst_GOChaine != _pst_GO));

	return _pst_Element->l_NbTriangles * nLoops;
}

#ifndef _FINAL_
extern u32 nbMaxTriangles;
#endif

extern int					NbrFurLayers ;
extern float				OffsetNormal;
extern float				OffsetU ;
extern float				OffsetV ;
extern int					FurInvertAlpha;
extern int					DontDrawFirstLayer;
extern GXI_HW_States 		g_GXI_HW_States;

/* Fur */
#define FUR_USE_DL_TEST

#ifdef FUR_USE_DL_TEST
void GXI_ShiftDrawElementStrip(GEO_tdst_ElementIndexedTriangles*,GEO_Vertex*,MATH_tdst_Vector*,GEO_tdst_UV*,ULONG,u32*,u32*,BOOL,BOOL);
void GXI_ShiftDrawElementTrilist(GEO_tdst_ElementIndexedTriangles*,GEO_Vertex*,MATH_tdst_Vector*,GEO_tdst_UV*,ULONG,u32*,u32*,BOOL,BOOL);

u32 FurDL_AllocSize = 0;
typedef struct FurDL_
{
	struct FurDL *pNext;
	struct FurDL *pPred;	
	GEO_tdst_ElementIndexedTriangles *pst_Element;
	int NbrFurLayers;
	OSTick lastUsedTick;
	u32 uDLSize;
	u8  pucDL[] __attribute__((aligned(32)));
} FurDL;

FurDL *gpstFurDLlist = NULL;

void FreeFurDLlist()
{
	FurDL *p;
	while(gpstFurDLlist)
	{
		p = gpstFurDLlist->pNext;
		MEM_FreeAlign(gpstFurDLlist);
		gpstFurDLlist = p;
	}
	FurDL_AllocSize = 0;
}

/* Free element, return next element */
inline
FurDL* FurDL_RemoveEl(FurDL* _ele)
{
	FurDL *n, *p;
	ERR_X_Assert(_ele!=NULL);
	p = _ele->pPred;
	n = _ele->pNext;
	
	/* Relink */
	if(p)
		p->pNext = n;
	if(n)
		n->pPred = p;

	/* Check begin of the list */
	if(_ele == gpstFurDLlist)
		gpstFurDLlist = n;
	
	/* Free mem */
	FurDL_AllocSize	-= _ele->uDLSize + sizeof(FurDL);
	MEM_FreeAlign(_ele);
	
	return n;
}

FurDL * FindOrCreateDL
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	MATH_tdst_Vector					*_pst_Normal,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints,
	u32									*pst_Color,
	u32									*pst_Alpha,	
	BOOL 								bSendNorms,
	BOOL 								bUseUV
)
{
	#define TempDLBuffSize (256*1024)
	FurDL *ret = gpstFurDLlist;
	FurDL *last = ret;
	OSTick curTick = OSGetTick();
	u32 s;	
	u8 *TempDL;
	u32 TempDLSize;
	
	// Search element in buffers.
	while(ret)
	{
		s = OSTicksToSeconds(OSDiffTick(curTick,ret->lastUsedTick));
		if(ret->pst_Element == _pst_Element)
		{// found
			if(ret->NbrFurLayers == NbrFurLayers)
			{
				ret->lastUsedTick = curTick;
				return ret; // good
			}
			break; // discarded.
		}
		
		if( s >= 3 )
		{
			// remove old cache data.
			ret = FurDL_RemoveEl(ret);
		}
		else
		{
			last = ret;
			ret = ret->pNext;
		}
	}
	
	if(FurDL_AllocSize>128*1024)
		return NULL;
		
	TempDL = GXI_LockTmpDLBuff(TempDLBuffSize);
	DCInvalidateRange((void*)TempDL, TempDLBuffSize);	
	
	GXBeginDisplayList((void*)TempDL,TempDLBuffSize);	
	if(GXI_b_IsStripped(_pst_Element))
		GXI_ShiftDrawElementStrip(_pst_Element,_pst_Point,_pst_Normal,_pst_UV,ulnumberOfPoints,pst_Color,
			   				pst_Alpha, bSendNorms, bUseUV);	
	else
		GXI_ShiftDrawElementTrilist(_pst_Element,_pst_Point,_pst_Normal,_pst_UV,ulnumberOfPoints,pst_Color,
			   				pst_Alpha, bSendNorms, bUseUV);		
	TempDLSize = GXEndDisplayList();

	if(ret) // element found but discarded
		FurDL_RemoveEl(ret);
	
	if(TempDLSize > 32)
	{
		FurDL_AllocSize += TempDLSize + sizeof(FurDL);
		ret = (FurDL*)MEM_p_AllocAlignMem(TempDLSize+sizeof(FurDL),32,&MEM_gst_GCMemoryInfo);
		memset(ret, 0, sizeof(FurDL));
		ret->uDLSize = TempDLSize;
		memcpy(ret->pucDL,TempDL,TempDLSize);
		DCFlushRange((void*)ret->pucDL,ret->uDLSize);
	}
	else
	{
		ret = MEM_p_Alloc(sizeof(FurDL));
		FurDL_AllocSize += sizeof(FurDL);
		memset(ret,0,sizeof(FurDL));
	}

	// Link list	
	if(gpstFurDLlist)
	{
		ERR_X_Assert(last!=NULL);
		last->pNext = ret;
		ret->pPred = last;
	}
	else
		gpstFurDLlist = ret; // first element

	ret->pst_Element = _pst_Element;
	ret->NbrFurLayers = NbrFurLayers;
	ret->lastUsedTick = curTick;
	
	GXI_UnLockTmpDLBuff();
	return ret;
}
#endif // FUR_USE_DL_TEST

void GXI_ShiftDrawElementStrip
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	MATH_tdst_Vector					*_pst_Normal,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints,
	u32									*pst_Color,
	u32									*pst_Alpha,	
	BOOL 								bSendNorms,
	BOOL 								bUseUV
)
{
	u32							ulGXISetCol_Or;
	u32							ulGXISetCol_XOr;
	u32							ulGXISetCol;
	u32							layer,i;
	u32							u32Tmp;	
	USHORT						auw_Index, auw_UV;
	float 						*pTempPoint,*pTempNormal;
	float						offsetN, offSetU, offSetV,offsetN2;
	float						vecTmp[3];
	GEO_tdst_OneStrip			*pStrip, *pStripEnd;
	GXAttrType					gxIndexType = GX_INDEX16;

	if(ulnumberOfPoints < 256)
		gxIndexType = GX_INDEX8;
			
	ERR_X_Assert(_pst_Element);
	ERR_X_Assert(_pst_Normal);
	if(bUseUV)
	{
		ERR_X_Assert(_pst_UV);
	}
	
	ulGXISetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
	ulGXISetCol_XOr ^= 0xff000000;
	ulGXISetCol_Or = GXI_Global_ACCESS(ulColorOr);
	
	if(!pst_Color)
	{
		ulGXISetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulGXISetCol_Or;
	    ulGXISetCol ^= ulGXISetCol_XOr;
	}	
			
	for (layer = DontDrawFirstLayer; layer < NbrFurLayers ; layer++)
	{
		u8		uThreshold;
		
		offsetN = OffsetNormal * layer;
		offSetU = OffsetU * layer;
		offSetV = OffsetV * layer;
		
		// Set alpha threshold
		// out_reg = d + ((a op b) ? c : 0);
		// For alpha, filter with threshold : out_reg = (alpha > threshold ? 1 : 0)
		uThreshold = (u8)((255 * layer)/NbrFurLayers);

		GX_GXSetAlphaCompare(
			GX_GEQUAL, //GXCompare comp0,
			uThreshold, //u8 ref0,
			GX_AOP_AND, //GXAlphaOp op,
			GX_ALWAYS, //GXCompare comp1,
			0); //u8 ref1 );		

		pStrip = _pst_Element->pst_StripData->pStripList;
		pStripEnd = pStrip + _pst_Element->pst_StripData->ulStripNumber;

		for(; pStrip < pStripEnd; pStrip++)
		{			
			ERR_X_Assert(pStrip->pMinVertexDataList);
			if(!bUseUV)
			{
				GXBegin(GX_TRIANGLESTRIP,GX_VTXFMT0,pStrip->ulVertexNumber);
				for(i = 0; i < pStrip->ulVertexNumber; i++)
				{
					auw_Index = pStrip->pMinVertexDataList[i].auw_Index;
					pTempPoint = (float *) &_pst_Point[auw_Index];
					pTempNormal = (float *) &_pst_Normal[auw_Index];						
					GXI_SetCurrentColorRGBA(auw_Index);
					GXI_SetVertexXYZ(pTempPoint,pTempNormal);
		        	if(bSendNorms)
		        	{
		        		if(gxIndexType == GX_INDEX16)
		        		 	GXNormal1x16((u16)auw_Index);
						else
		        		 	GXNormal1x8((u8)auw_Index);
		        	}
					GXI_SendCurrentColor();
				}
				GXEnd();
			}			
			else if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_FaceMap)
			{
				ERR_X_ForceError("pas de chance, on n'a pas facemap + strip en magasin", NULL);
			}
			else
			{
				GXBegin(GX_TRIANGLESTRIP,GX_VTXFMT0,pStrip->ulVertexNumber);
				for(i = 0; i < pStrip->ulVertexNumber; i++)
				{
					auw_Index = pStrip->pMinVertexDataList[i].auw_Index;
					if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)					
						auw_UV = auw_Index;
					else
						auw_UV = pStrip->pMinVertexDataList[i].auw_UV;
					pTempPoint = (float *) &_pst_Point[auw_Index];
					pTempNormal = (float *) &_pst_Normal[auw_Index];
					GXI_SetCurrentColorRGBA(auw_Index);
					GXI_SetVertexXYZ(pTempPoint,pTempNormal);
		        	if(bSendNorms)
		        	{
		        		if(gxIndexType == GX_INDEX16)
		        		 	GXNormal1x16((u16)auw_Index);
		        		 else
		        		 	GXNormal1x8((u8)auw_Index);
		        	}
					GXI_SendCurrentColor();
					if(gxIndexType == GX_INDEX16)
						GXTexCoord1x16((u16)auw_UV);
					else
						GXTexCoord1x8((u8)auw_UV);					
				}
				GXEnd();
			}			
		}
		
		if(layer == DontDrawFirstLayer)
			GX_GXSetZMode((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest) ? GX_ENABLE : GX_DISABLE, 
						  (GXCompare)-1, FALSE);		
	}
}

void GXI_ShiftDrawElementTrilist
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	MATH_tdst_Vector					*_pst_Normal,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints,
	u32									*pst_Color,
	u32									*pst_Alpha,	
	BOOL 								bSendNorms,
	BOOL 								bUseUV
)
{
	GEO_tdst_IndexedTriangle	*t, *tend;
	u32							ulGXISetCol_Or;
	u32							ulGXISetCol_XOr;
	u32							ulGXISetCol;
	u32							layer,i;
	u32							u32Tmp;	
	USHORT						auw_Index;
	float 						*pTempPoint,*pTempNormal;
	float						offsetN, offSetU, offSetV,offsetN2;
	float						vecTmp[3];
	GXAttrType					gxIndexType = GX_INDEX16;

	if(ulnumberOfPoints < 256)
		gxIndexType = GX_INDEX8;
			
	ERR_X_Assert(_pst_Element);
	if(bSendNorms)
	{
		ERR_X_Assert(_pst_Normal);
	}
	if(bUseUV)
	{
		ERR_X_Assert(_pst_UV);
	}
	
	ulGXISetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
	ulGXISetCol_XOr ^= 0xff000000;
	ulGXISetCol_Or = GXI_Global_ACCESS(ulColorOr);
	
	if(!pst_Color)
	{
		ulGXISetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulGXISetCol_Or;
	    ulGXISetCol ^= ulGXISetCol_XOr;
	}	
	
	t = _pst_Element->dst_Triangle;
	tend = t + _pst_Element->l_NbTriangles;
	
	// If no strip and no triangles, return.
	if (!_pst_Element->dst_Triangle)
		return;	
	//---------------------------------------------------------------
	// indexed triangles mode
	//---------------------------------------------------------------
	
	for (layer = DontDrawFirstLayer; layer < NbrFurLayers ; layer++)
	{
		u8		uThreshold;
		
		offsetN = OffsetNormal * layer;
		offSetU = OffsetU * layer;
		offSetV = OffsetV * layer;
		t = _pst_Element->dst_Triangle;
		
		// Set alpha threshold
		// out_reg = d + ((a op b) ? c : 0);
		// For alpha, filter with threshold : out_reg = (alpha > threshold ? 1 : 0)
		uThreshold = (u8)((255 * layer)/NbrFurLayers);
		
		GX_GXSetAlphaCompare(
			GX_GEQUAL, //GXCompare comp0,
			uThreshold, //u8 ref0,
			GX_AOP_AND, //GXAlphaOp op,
			GX_ALWAYS, //GXCompare comp1,
			0); //u8 ref1 );							
	
       	GXBegin(GX_TRIANGLES, GX_VTXFMT0, _pst_Element->l_NbTriangles*3);

		while(t < tend)
		{     
			for(i=0;i<3;i++)
			{   
				auw_Index = t->auw_Index[i];
				pTempPoint = (float *) &_pst_Point[auw_Index];
				pTempNormal = (float *) &_pst_Normal[auw_Index];
				GXI_SetCurrentColorRGBA(auw_Index);
				GXI_SetVertexXYZ(pTempPoint,pTempNormal);							
				if(bSendNorms)
	        		if(gxIndexType == GX_INDEX16)
	        		 	GXNormal1x16((u16)auw_Index);
					else
	        		 	GXNormal1x8((u8)auw_Index);
	   	    	GXI_SendCurrentColor();
	   	    	
	   	    	if(bUseUV)
	   	    	{
	   	    		if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
		   	    		auw_Index = i;
					if(gxIndexType == GX_INDEX16)
						GXTexCoord1x16((u16)t->auw_UV[i]);
					else
						GXTexCoord1x8((u8)t->auw_UV[i]);
	   	    	}
			}
   	    	t++;
   	    }
   	    	
   	    GXEnd();
   	    
   	    if(layer == DontDrawFirstLayer)
			GX_GXSetZMode((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest) ? GX_ENABLE : GX_DISABLE, 
						   (GXCompare)-1, FALSE);
	}	
}

LONG GXI_l_ShiftDrawElementIndexedTriangles
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	MATH_tdst_Vector					*_pst_Normal,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	u32							*pst_Color;
	u32							*pst_Alpha;
	GEO_Vertex					*pst_Normals;
	BOOL 						bUseUV,bSendNorms;
	GXAttrType					gxIndexType = GX_INDEX16;
#ifdef FUR_USE_DL_TEST
	FurDL *pDL = NULL;
#endif // FUR_USE_DL_TEST
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	

#if (defined(_GAMECUBE) && !defined(_FINAL_))
	if(NoFUR)
		return 0;
#endif
	if(g_ShadowPass)
		return 0;

	if(!NbrFurLayers)
		return 0;
		
	if(ulnumberOfPoints < 256)
		gxIndexType = GX_INDEX8;		

#ifdef USE_SOFT_UV_MATRICES
#ifndef _FINAL_
	if(!g_bUseHardwareTextureMatrices)
#endif	
		GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY ,GX_FALSE, GX_PTIDENTITY);
#endif	
	
	pst_Normals = _pst_Normal;
	if(!pst_Normals)
	{
		GEO_UseNormals(GDI_gpst_CurDD->pst_CurrentGeo);	
		pst_Normals = GDI_gpst_CurDD->pst_CurrentGeo->dst_PointNormal;
	}
	
	bSendNorms = !g_bLightOff;
	
	ERR_X_Assert(pst_Normals); // We need the normals for the fur.*/

	pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
	pst_Alpha = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentAlphaField;	
		
	if (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_TestBackFace)
	{
		if (GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_NotInvertBF)
			GX_GXSetCullMode(GX_CULL_FRONT);
		else
			GX_GXSetCullMode(GX_CULL_BACK);
	} 
	else
	{
		GX_GXSetCullMode(GX_CULL_NONE);
	}
	
	GXI_set_fog((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged)!=0);
	
	bUseUV = (GDI_gpst_CurDD->LastTextureUsed!=-1 && _pst_UV && !(g_bUVGenFromPos||g_bUVGenFromNrm));
	
	GX_GXSetArray(GX_VA_NRM, pst_Normals, 12);
	GX_GXSetArray(GX_VA_TEX0,_pst_UV, 8);
	
	GX_GXSetVtxDesc(GX_VA_POS,   GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_NRM,   bSendNorms ? gxIndexType : GX_NONE);
	GX_GXSetVtxDesc(GX_VA_CLR0,  GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_TEX0,  bUseUV ? gxIndexType : GX_NONE);
	
	GXI_prepare_to_draw_material(GX_SRC_VTX);
	GXI_set_color_generation_parameters(TRUE, (GDI_gpst_CurDD->LastTextureUsed!=-1));		

	GX_GXSetZCompLoc(GX_FALSE);	
	GX_GXSetZMode((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest) ? GX_ENABLE : GX_DISABLE,
                  GX_LEQUAL, TRUE);

	if(GDI_gpst_CurDD->pst_CurrentGameObject->uc_VisiCoeff < 50)
		NbrFurLayers = DontDrawFirstLayer+1;
		
#ifdef FUR_USE_DL_TEST
	//if( (ulnumberOfPoints*NbrFurLayers) > 512 )
		pDL = FindOrCreateDL(_pst_Element,_pst_Point,pst_Normals,_pst_UV,ulnumberOfPoints,pst_Color,
				   				pst_Alpha, bSendNorms, bUseUV);
	if(pDL)
	{
		if(pDL->pucDL && pDL->uDLSize)
			GXCallDisplayList(pDL->pucDL, pDL->uDLSize);
	} else
#endif // FUR_USE_DL_TEST
	{
		if (GXI_b_IsStripped(_pst_Element))
			GXI_ShiftDrawElementStrip(_pst_Element,_pst_Point,pst_Normals,_pst_UV,ulnumberOfPoints,pst_Color,
				   				pst_Alpha, bSendNorms, bUseUV);	
		else
			GXI_ShiftDrawElementTrilist(_pst_Element,_pst_Point,pst_Normals,_pst_UV,ulnumberOfPoints,pst_Color,
				   				pst_Alpha, bSendNorms, bUseUV);
	}

	g_bUVGenFromPos = FALSE;
	g_bUVGenFromNrm = FALSE;
	
#ifndef _FINAL_
	if(g_bShowNormals)
		GXI_DBG_DrawObjectNormals(_pst_Element,_pst_Point,ulnumberOfPoints);
		
	if(g_bShowSkinElements)
		GXI_DBG_ShowSkinElements(_pst_Element,_pst_Point,ulnumberOfPoints);
		
	if(g_bShowVertexColors)
		GXI_DBG_ShowVertexColors(_pst_Element,_pst_Point,ulnumberOfPoints);
#endif	
	
	return _pst_Element->l_NbTriangles;
}




LONG GXI_l_DrawElementIndexedTriangles
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	MATH_tdst_Vector					*_pst_Normal,
	GEO_tdst_UV							*_pst_UV,
	ULONG								ulnumberOfPoints
)
{
	ULONG ReturnValue;
	ReturnValue = 0xc0de2020;

	GXI_BeginRaster(4);
	
#ifdef USE_HARDWARE_LIGHTS
	// if lighting was removed for some special reason (ie : shadows) or there is no lighting
	if(!GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField || g_ShadowPass || g_bRenderingShadows || !(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Lighted))
		g_bLightOff = TRUE;
	else
		g_bLightOff = FALSE;
#endif 

#ifndef USE_FULL_SCENE_SHADOWS
	if(g_bRenderingShadows)
	{
		ReturnValue = GXI_l_DrawElementIndexedTrianglesShadows(_pst_Element,_pst_Point,_pst_UV,ulnumberOfPoints);
		goto EndDEIT;
	}
#endif		

#ifdef USE_SOFT_UV_MATRICES
#ifndef _FINAL_
	if(g_bUseHardwareTextureMatrices)
#endif	
#endif

	
	if (renderState_Shfited) // Shifted rendering cannot use display lists.
		ReturnValue = GXI_l_ShiftDrawElementIndexedTriangles(_pst_Element , _pst_Point , _pst_Normal, _pst_UV , ulnumberOfPoints);
	else
	{
		/*if(renderState_Cloned)
			ReturnValue = GXI_l_CloneDrawElementIndexedTriangles(_pst_Element , _pst_Point , _pst_Normal, _pst_UV , ulnumberOfPoints);
		else*/
		if(_pst_Element->dl) // send object using a displaylist
			ReturnValue = GXI_l_DrawElementIndexedTrianglesUsingDL(_pst_Element,_pst_Point,_pst_UV,ulnumberOfPoints);
		else // send object directly to viewport
			ReturnValue = GXI_l_DrawElementIndexedTrianglesDirectly(_pst_Element,_pst_Point,_pst_UV,ulnumberOfPoints);
	}	
		
EndDEIT:
	GXI_EndRaster(4);
	
#ifdef GC_BENCHGRAPH
	GXI_Global_ACCESS(ulNumberOfTri) += _pst_Element->l_NbTriangles;
#endif

#ifdef Active_CloneListe
	if( renderState_Cloned )
		GDI_gpst_CurDD_SPR.pst_CurrentGameObject->ulMoreCloneInfo |= 1;
#endif // Active_CloneListe		

	return ReturnValue;
}



/*
 =======================================================================================================================
    Aim:    Setup blending mode
 =======================================================================================================================
 */
void GXI_SetTextureBlending(ULONG _l_Texture, ULONG BM , u32 AF)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					Flag;
	ULONG					Delta;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//if(!GXI_Global_ACCESS(bSendingObjectList))
	_l_Texture = GXI_RS_UseTexture(_l_Texture);

	GDI_gpst_CurDD->LastTextureUsed = _l_Texture;

	Flag = MAT_GET_FLAG(BM);

	if(Flag & MAT_Cul_Flag_UseLocalAlpha)
		GXI_Global_ACCESS(ulColorOr) = 0xff000000;
	else
		GXI_Global_ACCESS(ulColorOr) = 0x00000000;

	if(GXI_Global_ACCESS(FogParams).FogActive)
	{
		switch(MAT_GET_Blending(BM))
		{
		case MAT_Cc_Op_Copy:
        case MAT_Cc_Op_Glow:
		case MAT_Cc_Op_Alpha:
		case MAT_Cc_Op_AlphaDest:
			GXI_Global_ACCESS(FogParams).FogForceDisable = FALSE;
			break;
		case MAT_Cc_Op_AlphaPremult:
		case MAT_Cc_Op_AlphaDestPremult:
		case MAT_Cc_Op_Add:
		case MAT_Cc_Op_Sub:
		case MAT_Cc_Op_PSX2ShadowSpecific:
			GXI_Global_ACCESS(FogParams).FogForceDisable = TRUE;
			break;
		}
	}

	Delta = 0;
	
	if((s32)_l_Texture >= 0)//!= (ULONG) - 1)
	{
		if(GXI_Global_ACCESS(dul_TextureDeltaBlend))
		{
			Delta = GXI_Global_ACCESS(dul_TextureDeltaBlend)[_l_Texture] ^ BM; 
			if(!GXI_Global_ACCESS(dul_TextureDeltaBlend)[_l_Texture]) // first time init 
				Delta = -1;
		}
		else 
			Delta = -1;

        if(Delta == -1 || Delta & MAT_Cul_Flag_TileU || Delta & MAT_Cul_Flag_TileV)
		{
			GXInitTexObjWrapMode(&GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[_l_Texture].obj,
								  ((Flag & MAT_Cul_Flag_TileU) ? GX_REPEAT : GX_CLAMP),
								  ((Flag & MAT_Cul_Flag_TileV) ? GX_REPEAT : GX_CLAMP));
		}

		{
			if(GXI_Global_ACCESS(dul_TextureDeltaBlend))
			{
				GXI_Global_ACCESS(dul_TextureDeltaBlend)[_l_Texture] = BM;
			}
		}
	}

	Delta = GDI_gpst_CurDD->LastBlendingMode ^ BM;

	{
		if(Flag & MAT_Cul_Flag_AlphaTest)
		{
			if(Flag & MAT_Cul_Flag_InvertAlpha)
				GX_GXSetAlphaCompare(GX_LESS, (u8) MAT_GET_AlphaTresh(BM), GX_AOP_OR, GX_NEVER, 0);
			else
				GX_GXSetAlphaCompare(GX_GREATER, (u8) MAT_GET_AlphaTresh(BM), GX_AOP_OR, GX_NEVER, 0);
			GX_GXSetZCompLoc(GX_FALSE);
		}
		else
		{
			GX_GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
			GX_GXSetZCompLoc(GX_TRUE);
		}			
	}

	GX_GXSetColorUpdate((Flag & MAT_Cul_Flag_HideColor) ? GX_FALSE : GX_TRUE);
	GX_GXSetAlphaUpdate((Flag & MAT_Cul_Flag_HideAlpha) ? GX_FALSE : GX_TRUE);

	
#ifdef DONT_USE_ALPHA_DEST    	
	GXI_SetPreviousAlphaMode(0);
#endif	
	GX_GXSetZMode((GXBool)-1, (Flag & MAT_Cul_Flag_ZEqual) ? GX_EQUAL : GX_LEQUAL, (Flag & MAT_Cul_Flag_NoZWrite) ? GX_DISABLE : GX_ENABLE);
	
	{
		switch(MAT_GET_Blending(BM))
		{
		case MAT_Cc_Op_Copy:
        case MAT_Cc_Op_Glow:
			GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP); // copy
			break;
			
		case MAT_Cc_Op_Alpha:
			if(Flag & MAT_Cul_Flag_InvertAlpha)
				GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_INVSRCALPHA, GX_BL_SRCALPHA, GX_LO_NOOP); //(1-alpha)*src+alpha*dst
			else
				GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP); //alpha*src+(1-alpha)*dst
			break;
			
		case MAT_Cc_Op_AlphaPremult:
			if(Flag & MAT_Cul_Flag_InvertAlpha)
				GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_SRCALPHA, GX_LO_NOOP); //src+(1-alpha)*dst
			else
				GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_INVSRCALPHA, GX_LO_NOOP); //src+alpha*dst
			break;
			
#ifdef DONT_USE_ALPHA_DEST    
		case MAT_Cc_Op_AlphaDest:
			GXI_SetPreviousAlphaMode(2);
			if(Flag & MAT_Cul_Flag_InvertAlpha)
				GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_INVSRCALPHA, GX_BL_SRCALPHA, GX_LO_NOOP); //(1-alpha)*src+alpha*dst
			else
				GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP); //alpha*src+(1-alpha)*dst
			break;
			
		case MAT_Cc_Op_AlphaDestPremult:
			GXI_SetPreviousAlphaMode(2);
			if(Flag & MAT_Cul_Flag_InvertAlpha)
				GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_INVSRCALPHA, GX_BL_ONE, GX_LO_NOOP); //src+(1-alpha)*dst
			else
				GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_NOOP); //src+alpha*dst
			break;
#else
		case MAT_Cc_Op_AlphaDest:
			if(Flag & MAT_Cul_Flag_InvertAlpha)
				GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_INVDSTALPHA, GX_BL_DSTALPHA, GX_LO_NOOP); //(1-dsta)*src+dsta*dst
			else
				GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_DSTALPHA, GX_BL_INVDSTALPHA, GX_LO_NOOP); //dsta*src+(1-dsta)*dst
			break;
			
		case MAT_Cc_Op_AlphaDestPremult:
			if(Flag & MAT_Cul_Flag_InvertAlpha)
				GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_INVDSTALPHA, GX_BL_ONE, GX_LO_NOOP); //dsta*src+(1-dsta)*dst
			else
				GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_DSTALPHA, GX_BL_ONE, GX_LO_NOOP); //dsta*src+dst
			break;
#endif			
		case MAT_Cc_Op_Add:
			GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
			break;
			
		case MAT_Cc_Op_Sub:
			GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_ZERO, GX_BL_INVSRCCLR, GX_LO_NOOP);
			break;
			
		case MAT_Cc_Op_PSX2ShadowSpecific:
			GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_ZERO, GX_BL_INVSRCCLR, GX_LO_NOOP);
			break;
		}
	}

/*	GX_GXSetColorUpdate(GX_TRUE);
	GX_GXSetAlphaUpdate(GX_TRUE);
	GX_GXSetZMode((GXBool)-1, GX_LEQUAL, GX_ENABLE);
	GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP); // copy//*/
	GDI_gpst_CurDD->LastBlendingMode = BM;
	
	if((s32)_l_Texture >= 0)
	{
#ifdef DONT_USE_ALPHA_DEST	
		g_sdt_SaveLastLevel . LastTextObj = &GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[_l_Texture].obj;
#endif		
		GX_GXLoadTexObj(&GXI_Global_ACCESS(p_MyTextureDescriptorsAligned)[_l_Texture].obj, GX_TEXMAP0);
	}
}

extern GXI_HW_States g_GXI_HW_States;

LONG GXI_l_DrawElementIndexedSprite
(
	GEO_tdst_ElementIndexedSprite		*_pst_Element,
	GEO_Vertex					        *_pst_Point,
	ULONG								ulnumberOfPoints
)
{
	MATH_tdst_Vector XCam , YCam , *p_point;
	MATH_tdst_Vector        Sprite[4];
	float Size,fCounter;
	GEO_tdst_IndexedSprite	*p_Frst,*p_Last;
	ULONG						ulGXISetCol_Or;
	ULONG						ulGXISetCol_XOr , ulGXISetCol;
	ULONG						*pst_Color;
	ULONG						*pst_Alpha;
	s32 count;

	
#ifdef USE_HARDWARE_LIGHTS 
	g_bLightOff = TRUE;
#endif

	pst_Color = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentColorField;
	pst_Alpha = GDI_gpst_CurDD->pst_ComputingBuffers->CurrentAlphaField;


		
	ulGXISetCol_Or = GXI_Global_ACCESS(ulColorOr);
	ulGXISetCol_XOr = GDI_gpst_CurDD->pst_ComputingBuffers->ulColorXor;
	
	XCam . x = -GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Ix * _pst_Element->fGlobalSize * _pst_Element->fGlobalRatio;
	XCam . y = -GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Jx * _pst_Element->fGlobalSize * _pst_Element->fGlobalRatio;
	XCam . z = -GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Kx * _pst_Element->fGlobalSize * _pst_Element->fGlobalRatio;
	YCam . x = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Iy * _pst_Element->fGlobalSize;
	YCam . y = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Jy * _pst_Element->fGlobalSize;
	YCam . z = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Ky * _pst_Element->fGlobalSize;
	
	p_Frst = _pst_Element->dst_Sprite;
	p_Last = p_Frst + _pst_Element->l_NbSprites;
	
	if(!pst_Color)
	{
		ulGXISetCol = GDI_gpst_CurDD->pst_ComputingBuffers->ul_Ambient | ulGXISetCol_Or;
	    ulGXISetCol ^= ulGXISetCol_XOr;
	}
	
	g_bColorDiv2X = TRUE;
	g_bAlphaDiv2X = TRUE;

	GX_GXSetCullMode(GX_CULL_NONE);
	
	GXI_set_draw_vertex_properties(GX_DIRECT, GX_NONE, GX_DIRECT, GX_DIRECT);
	
	GXI_prepare_to_draw_material(GX_SRC_VTX);
	GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY ,GX_FALSE, GX_PTIDENTITY);
	GXI_set_color_generation_parameters(TRUE, TRUE);

	GX_GXSetZMode((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_ZTest) ? GX_ENABLE : GX_DISABLE, (GXCompare)-1, -1);
	
	GXI_set_fog((GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_Fogged)!=0);
	
	count = (u32)p_Last-(u32)p_Frst;
	
	if(count <= 0) return 0;
	
	fCounter = 0.0f;
	
	
   	GXBegin(GX_QUADS, GX_VTXFMT0, count);
	
	if (p_Frst->auw_Index == 0xC0DE) // GFX Signal of mega-flux (no indexes) , points ares SOFT_tdst_AVertexes with w = size
	{
		SOFT_tdst_AVertex *pS,*pSE;
		
		pS = (SOFT_tdst_AVertex *)_pst_Point;
		pS += p_Frst [1].auw_Index;
		pSE = pS + _pst_Element->l_NbSprites;
		if(pst_Color) pst_Color +=  p_Frst [1].auw_Index;
		if(pst_Alpha) pst_Alpha +=  p_Frst [1].auw_Index;
		
		while (pS < pSE)
		{
			fCounter += 1.0f;
			if(pst_Color) 
			{ 
				ulGXISetCol = *pst_Color | ulGXISetCol_Or; 
				ulGXISetCol ^= ulGXISetCol_XOr; 
				if(pst_Alpha) 
				{ 
					ulGXISetCol &= 0x00ffffff; 
					ulGXISetCol |= pst_Alpha[p_Frst->auw_Index]; 
				} 
				pst_Color++;
			} 
			else if(pst_Alpha) 
			{ 
				ulGXISetCol &= 0x00ffffff; 
				ulGXISetCol |= pst_Alpha[p_Frst->auw_Index]; 
			}

			Size = pS->w;

			Sprite[0] . x = pS->x  + (- XCam.x - YCam.x) * Size;
			Sprite[0] . y = pS->y  + (- XCam.y - YCam.y) * Size;
			Sprite[0] . z = pS->z  + (- XCam.z - YCam.z) * Size;
			Sprite[1] . x = pS->x  + (+ XCam.x - YCam.x) * Size;
			Sprite[1] . y = pS->y  + (+ XCam.y - YCam.y) * Size;
			Sprite[1] . z = pS->z  + (+ XCam.z - YCam.z) * Size;
			Sprite[2] . x = pS->x  + (+ XCam.x + YCam.x) * Size;
			Sprite[2] . y = pS->y  + (+ XCam.y + YCam.y) * Size;
			Sprite[2] . z = pS->z  + (+ XCam.z + YCam.z) * Size;
			Sprite[3] . x = pS->x  + (- XCam.x + YCam.x) * Size;
			Sprite[3] . y = pS->y  + (- XCam.y + YCam.y) * Size;
			Sprite[3] . z = pS->z  + (- XCam.z + YCam.z) * Size;

			if(GDI_gb_WaveSprite)
			{
				extern float TIM_gf_MainClockReal;
				float ff, ff1;
				ff = fOptSin(fCounter);
				ff1 = fOptSin(TIM_gf_MainClockReal * ff * 2.5f) * ff * 0.2f;
				Sprite[0] . x += ff1;
				Sprite[1] . x += ff1;
			}

			GXPosition3f32(Sprite[0].x, Sprite[0].y, Sprite[0].z);
			GXColor1u32((ulGXISetCol & 0x000000ff)<<24 | (ulGXISetCol & 0x0000ff00)<<8 | (ulGXISetCol & 0x00ff0000)>>8 | (ulGXISetCol & 0xff000000)>>24);
			GXTexCoord2f32(0.0f, 1.0f);
			
		   	GXPosition3f32(Sprite[1].x, Sprite[1].y, Sprite[1].z);
			GXColor1u32((ulGXISetCol & 0x000000ff)<<24 | (ulGXISetCol & 0x0000ff00)<<8 | (ulGXISetCol & 0x00ff0000)>>8 | (ulGXISetCol & 0xff000000)>>24);
			GXTexCoord2f32(1.0f, 1.0f);

		   	GXPosition3f32(Sprite[2].x, Sprite[2].y, Sprite[2].z);
			GXColor1u32((ulGXISetCol & 0x000000ff)<<24 | (ulGXISetCol & 0x0000ff00)<<8 | (ulGXISetCol & 0x00ff0000)>>8 | (ulGXISetCol & 0xff000000)>>24);
			GXTexCoord2f32(1.0f, 0.0f);

		   	GXPosition3f32(Sprite[3].x, Sprite[3].y, Sprite[3].z);
			GXColor1u32((ulGXISetCol & 0x000000ff)<<24 | (ulGXISetCol & 0x0000ff00)<<8 | (ulGXISetCol & 0x00ff0000)>>8 | (ulGXISetCol & 0xff000000)>>24);
			GXTexCoord2f32(0.0f, 0.0f);
			
			pS++;
		}
	} 
	else
	{
		while (p_Frst < p_Last)
		{
			fCounter += 1.0f;
			p_point = _pst_Point + p_Frst->auw_Index;
			Size = *(float *)p_Frst;

			Sprite[0] . x = p_point->x  + (- XCam.x - YCam.x) * Size;
			Sprite[0] . y = p_point->y  + (- XCam.y - YCam.y) * Size;
			Sprite[0] . z = p_point->z  + (- XCam.z - YCam.z) * Size;
			Sprite[1] . x = p_point->x  + (+ XCam.x - YCam.x) * Size;
			Sprite[1] . y = p_point->y  + (+ XCam.y - YCam.y) * Size;
			Sprite[1] . z = p_point->z  + (+ XCam.z - YCam.z) * Size;
			Sprite[2] . x = p_point->x  + (+ XCam.x + YCam.x) * Size;
			Sprite[2] . y = p_point->y  + (+ XCam.y + YCam.y) * Size;
			Sprite[2] . z = p_point->z  + (+ XCam.z + YCam.z) * Size;
			Sprite[3] . x = p_point->x  + (- XCam.x + YCam.x) * Size;
			Sprite[3] . y = p_point->y  + (- XCam.y + YCam.y) * Size;
			Sprite[3] . z = p_point->z  + (- XCam.z + YCam.z) * Size;

			if(GDI_gb_WaveSprite)
			{
				extern float TIM_gf_MainClockReal;
				float ff, ff1;
				ff = fOptSin(fCounter);
				ff1 = fOptSin(TIM_gf_MainClockReal * ff * 2.5f) * ff * 0.2f;
				Sprite[0] . x += ff1;
				Sprite[1] . x += ff1;
			}

		   	GXPosition3f32(Sprite[0].x, Sprite[0].y, Sprite[0].z);
			GXI_SetColorRGBA(p_Frst->auw_Index);
			GXTexCoord2f32(0.0f, 1.0f);
			
		   	GXPosition3f32(Sprite[1].x, Sprite[1].y, Sprite[1].z);
			GXI_SetColorRGBA(p_Frst->auw_Index);
			GXTexCoord2f32(1.0f, 1.0f);

		   	GXPosition3f32(Sprite[2].x, Sprite[2].y, Sprite[2].z);
			GXI_SetColorRGBA(p_Frst->auw_Index);
			GXTexCoord2f32(1.0f, 0.0f);

		   	GXPosition3f32(Sprite[3].x, Sprite[3].y, Sprite[3].z);
			GXI_SetColorRGBA(p_Frst->auw_Index);
			GXTexCoord2f32(0.0f, 0.0f);
			p_Frst++;
		}
	}
	
	GXEnd();
	
	g_bColorDiv2X = FALSE;
	g_bAlphaDiv2X = FALSE;

	return 0;
}
void GXI_DepthBlur(int OnOff , float ZStart ,float ZEnd)
{
}
void GXI_DrawRectangle_Z(f32 fx, f32 fy, f32 fw, f32 fh, u32 C , u32 T , u32 WriteZ , f32 ZValue)
{
	Mtx44 	m1;
	Mtx 	m2;
	f32 pm[GX_PROJECTION_SZ];
	
	GXGetProjectionv(pm);
	//MTXOrtho(m1,1.0f,0.0f,0.0f,1.0f,0.0f,GX_MAX_Z24);
	MTXOrtho(m1,1.0f,0.0f,0.0f,1.0f,0.0f,1.0f);
	GXSetProjection(m1, GX_ORTHOGRAPHIC);
	MTXIdentity(m2);
	GXLoadPosMtxImm(m2, GX_PNMTX0);		
	
	GX_GXSetCullMode(GX_CULL_NONE);
	GX_GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_NRM,  GX_NONE);
	GX_GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_GXSetVtxDesc(GX_VA_TEX0, GX_NONE);
	GX_GXSetVtxDesc(GX_VA_TEX1, GX_NONE);
	GX_GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
	if (WriteZ)
	{
		GX_GXSetZMode(GX_ENABLE, GX_ALWAYS, GX_ENABLE);
	}
	
	
		
	GX_GXSetChanCtrl(  GX_COLOR0A0,
    			   GX_DISABLE,		// enable Channel
    			   GX_SRC_VTX,		// amb source
    			   GX_SRC_VTX,		// mat source
    			   GX_LIGHT_NULL,	// light mask
    			   GX_DF_CLAMP,		// diffuse function
    			   GX_AF_NONE );	// atten   function
	    			   
	GX_GXSetAlphaCompare(GX_NEVER, 0, GX_AOP_OR, GX_ALWAYS, 0);
	GX_GXSetZCompLoc(GX_FALSE);
#ifdef DONT_USE_ALPHA_DEST    	
	GXI_SetPreviousAlphaMode(0);
#endif	

	switch(T)
	{
	case MAT_Cc_Op_Copy:
		GX_GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP); // copy
		break;
		
	case MAT_Cc_Op_Alpha:
		GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP); //alpha*src+(1-alpha)*dst
		break;
		
	case MAT_Cc_Op_AlphaPremult:
		GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_INVSRCALPHA, GX_LO_NOOP); //src+(1-alpha)*dst
		break;
		
	case MAT_Cc_Op_AlphaDest:
		GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_DSTALPHA, GX_BL_INVDSTALPHA, GX_LO_NOOP); //dsta*src+(1-dsta)*dst
		break;
		
	case MAT_Cc_Op_AlphaDestPremult:
		GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_DSTALPHA, GX_BL_ONE, GX_LO_NOOP); //dsta*src+dst
		break;
		
	case MAT_Cc_Op_Add:
		GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_NOOP);
		break;
		
	case MAT_Cc_Op_Sub:
		GX_GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_INV);
		break;
	}
		
	GX_GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY ,GX_FALSE, GX_PTIDENTITY);
	GX_GXSetNumChans(1);     
	GX_GXSetNumTexGens(0);
	GX_GXSetNumTevStages(1);
	GX_GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);	
	GX_GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		
   	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			
	   	GXPosition3f32(fx, fy, ZValue);
    	GXColor1u32(GXI_ColorABGR2RGBA(C));

	   	GXPosition3f32(fx, fy+fh, ZValue);
    	GXColor1u32(GXI_ColorABGR2RGBA(C));

	   	GXPosition3f32(fx+fw, fy+fh, ZValue);
    	GXColor1u32(GXI_ColorABGR2RGBA(C));

	   	GXPosition3f32(fx+fw, fy, ZValue);
    	GXColor1u32(GXI_ColorABGR2RGBA(C));

    GXEnd();
    
    GXSetProjectionv(pm);
}

void GXI_DrawRectangle(f32 fx, f32 fy, f32 fw, f32 fh, u32 C , u32 T)
{
	GXI_DrawRectangle_Z(fx,  fy,  fw,  fh,  C ,  T , 0 , 0.0f);
}

void GXI_DrawShadowedRectangle(f32 x, f32 y, f32 w, f32 h, u32 C)
{
     GXI_DrawRectangle(x + 0.001f,y - 0.003f, w, h, 0xff202020, MAT_Cc_Op_Copy);
     GXI_DrawRectangle(x,y,w,h,C,MAT_Cc_Op_Copy);
}

void GXI_v_FreeRenderBuff()
{
	#ifdef FUR_USE_DL_TEST
	FreeFurDLlist();
	#endif // FUR_USE_DL_TEST
}