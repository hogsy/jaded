/*$T MDFmodifier_GEO.c GC! 1.081 06/09/00 09:00:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
 
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_SPG.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "SOFT/SOFTstruct.h"
#include "SOFT/SOFTzlist.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h" 
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"

#include "BASe/BENch/BENch.h"

#ifdef PSX2_TARGET
#include "MainPsx2/Sources/PSX2debug.h"
#define _ReadLong(_a)   ReadLong((char*)_a)
#define _ReadFloat(_a)   ReadFloat((char*)_a)
#else
#define _ReadLong(_a)    *(ULONG*)_a
#define _ReadFloat(_a)   *(float*)_a
#endif

#if defined(_XBOX) 
#include <assert.h>
#include "Gx8/Gx8AddInfo.h"
#include "Gx8/Gx8init.h"
#include "Gx8/Gx8BuildUVs.h"
#endif


static float GST_BIGFUCK = 0.1f;
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

#define SPG_Nb_Max_SpritePerPass 256
#define SPG_RandomNumber SPG_Nb_Max_SpritePerPass - 1
float f_RandomTable[SPG_RandomNumber + 1];
unsigned char ulRandomPos = 0;
unsigned char ulRandomInit = 0;
static GEO_tdst_IndexedSprite			SpritesIDX[SPG_Nb_Max_SpritePerPass] ONLY_PSX2_ALIGNED(16);
static GEO_tdst_ElementIndexedSprite	stIndxSp[4];
static GEO_tdst_Object			*pst_CurrentObj;
static MAT_tdst_Material *		pst_CurrentMat;
static unsigned short FIndx = 0;

extern BOOL GDI_gb_WaveSprite;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

void SPG_InitModifier(SPG_tdst_Modifier *pst_SPG)
{
	pst_SPG->f_GlobalRatio = 1.0f;
	pst_SPG->f_GlobalSize = 0.01f;
	pst_SPG->bk_MaterialID = (ULONG)-1;
	pst_SPG->p_Material = NULL;
	pst_SPG->ulMaxDepth	= 2;
	pst_SPG->f_Noise	= 0.0f;
	*(ULONG *)&pst_SPG->p_Material = (ULONG)-1;
	pst_SPG->ulSubMaterialMask = (ULONG)-1;
	L_memset(&pst_SPG->stSpriteMapper , 0 , sizeof(SPG_SpriteMapper) * 4);
	pst_SPG->f_Extraction = 0.0f;
	pst_SPG->f_ThresholdMin = 0.0f;
	pst_SPG->fLODCorrectionFactor = 1.0f;
	pst_SPG->stSED[0].fRatioFactor = 1.0f;
	pst_SPG->stSED[0].fSizeFactor = 1.0f;
	pst_SPG->stSED[0].fSizeNoiseFactor = 0.0f;
	pst_SPG->stSED[0].ulSubElementMaterialNumber = 0;
	pst_SPG->stSED[1] = pst_SPG->stSED[0];
	pst_SPG->stSED[2] = pst_SPG->stSED[0];
	pst_SPG->stSED[3] = pst_SPG->stSED[0];
	pst_SPG->ulFlags = SPG_IsMaterialTransparent;
	
#ifdef ACTIVE_EDITORS
		pst_SPG->ulCodeKey = 0xC0DE2002;
#endif
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */

void SPG_Modifier_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SPG_tdst_Modifier *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(SPG_tdst_Modifier));
	pst_Data = (SPG_tdst_Modifier *) _pst_Mod->p_Data;
	L_memset(pst_Data , 0 , sizeof(SPG_tdst_Modifier));
	
	if (!ulRandomInit)
	{
		ULONG Counter;
		ulRandomInit = 1;
		Counter = SPG_RandomNumber + 1;
		while (Counter--) f_RandomTable[Counter] = (((float) (rand() & 255) / 256.0f) - 0.5f);
	}


	if(!p_Data)
	{
		SPG_InitModifier(pst_Data);
	}
	else
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(SPG_tdst_Modifier));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SPG_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SPG_tdst_Modifier *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (SPG_tdst_Modifier *) _pst_Mod->p_Data;
	MEM_Free(pst_Data);
}
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */

void SPG_Modifier_Apply
(
	MDF_tdst_Modifier		*_pst_Mod,
	GEO_tdst_Object			*_pst_Obj
)
{
#if defined(_XENON_RENDER)
    if (_pst_Mod && _pst_Mod->pst_GO && OBJ_b_TestIdentityFlag(_pst_Mod->pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        _pst_Mod->pst_GO->pst_Base->pst_Visu->ucFlag |= GRO_VISU_FLAG_HIDE_MESH;
    }
#endif
}
_inline_ unsigned short SPG_CreateNewPoint(GEO_Vertex *P1)
{
	return FIndx++;
}
static GEO_Vertex *gvCurrentStack;
static GEO_Vertex gvStack[SPG_Nb_Max_SpritePerPass] ONLY_PSX2_ALIGNED(16);
static ULONG *gvCOLORStack;
static ULONG gvBase;
#ifdef ACTIVE_EDITORS
	static ULONG gsulNumberofgeneratedSprites = 0;
#endif

float gs_SpecialFogNear;
float gs_SpecialFogFar;
ULONG gs_SpecialFogCOLOR ;
GEO_Vertex gs_SpecialFogAxis;
GEO_Vertex gs_SpecialFogCulledPoint;
float gs_SpecialFogDistance;
extern ULONG LIGHT_ul_Interpol2Colors(ULONG ulP1, ULONG ulP2, float fZClipLocalCoef);
#ifdef PSX2_TARGET
void SPG_ComputeSpriteFog(SOFT_tdst_AVertex *pPoints , ULONG *pColors , ULONG ulNumber)
{
	float fFactor;
	while (ulNumber--)
	{
		fFactor = MATH_f_DotProduct(VCast(pPoints) , &gs_SpecialFogAxis) - gs_SpecialFogDistance;
		if (fFactor >= 1.0f) pPoints->c = gs_SpecialFogCOLOR;
		else
			if (fFactor > 0.0f)
				pPoints->c = LIGHT_ul_Interpol2Colors(pPoints->c , gs_SpecialFogCOLOR , fFactor);
		pColors++;
		pPoints++;
	}
}
void SPG_ComputeSpriteFog_Cull(SOFT_tdst_AVertex *pPoints , ULONG *pColors , ULONG ulNumber)
{
	float fFactor;
	while (ulNumber--)
	{
		fFactor = MATH_f_DotProduct(VCast(pPoints) , &gs_SpecialFogAxis) - gs_SpecialFogDistance;
		if (fFactor >= 1.0f)	*pPoints = gs_SpecialFogCulledPoint;
			else
				if (fFactor > 0.0f)
					pPoints->c = LIGHT_ul_Interpol2Colors(pPoints->c , gs_SpecialFogCOLOR , fFactor);
		pPoints++;
	}
}
#else
void SPG_ComputeSpriteFog(GEO_Vertex *pPoints , ULONG *pColors , ULONG ulNumber)
{
	float fFactor;
	while (ulNumber--)
	{
		fFactor = MATH_f_DotProduct(VCast(pPoints) , &gs_SpecialFogAxis) - gs_SpecialFogDistance;
		*pColors = LIGHT_ul_Interpol2Colors(*pColors , gs_SpecialFogCOLOR , fFactor);
		pColors++;
		pPoints++;
	}
}
void SPG_ComputeSpriteFog_Cull(GEO_Vertex *pPoints , ULONG *pColors , ULONG ulNumber)
{
	float fFactor;
	while (ulNumber--)
	{
		fFactor = MATH_f_DotProduct(VCast(pPoints) , &gs_SpecialFogAxis) - gs_SpecialFogDistance;
		if (fFactor >= 1.0f)	*pPoints = gs_SpecialFogCulledPoint;
			else
				if (fFactor > 0.0f)
					*pColors = LIGHT_ul_Interpol2Colors(*pColors , gs_SpecialFogCOLOR , fFactor);
		pColors++;
		pPoints++;
	}
}
#endif
void SPG_Draw(ULONG ulNumber)
{
	GEO_Vertex *pSaveVL;
	pSaveVL = GDI_gpst_CurDD->p_Current_Vertex_List ;
	GDI_gpst_CurDD->p_Current_Vertex_List = gvCurrentStack;
	if (gs_SpecialFogCOLOR) 
	{
		if (gs_SpecialFogCOLOR & 0x100)
			SPG_ComputeSpriteFog_Cull(gvStack , gvCOLORStack - ulNumber, ulNumber );
		else
			SPG_ComputeSpriteFog(gvStack , gvCOLORStack - ulNumber, ulNumber );
	}
	MAT_DrawIndexedSprites(pst_CurrentObj,pst_CurrentMat,&stIndxSp[0]);
	MAT_DrawIndexedSprites(pst_CurrentObj,pst_CurrentMat,&stIndxSp[1]);
	MAT_DrawIndexedSprites(pst_CurrentObj,pst_CurrentMat,&stIndxSp[2]);
	MAT_DrawIndexedSprites(pst_CurrentObj,pst_CurrentMat,&stIndxSp[3]);
	gvCOLORStack -= ulNumber;
	GDI_gpst_CurDD->p_Current_Vertex_List = pSaveVL;
	gvBase = 0;

#ifdef ACTIVE_EDITORS
	gsulNumberofgeneratedSprites += stIndxSp[0].l_NbSprites + stIndxSp[1].l_NbSprites + stIndxSp[2].l_NbSprites + stIndxSp[3].l_NbSprites;
#endif

}
SOFT_tdst_AVertex SPG_Noise[2];

#ifndef PSX2_TARGET
void SPG_SetNoise(SOFT_tdst_AVertex *P1,SOFT_tdst_AVertex *P2 , float fNoise , SOFT_tdst_AVertex *SaveNoise)
{
	MATH_ScaleVector((MATH_tdst_Vector *)&SPG_Noise[0] , (MATH_tdst_Vector *)P1 , fNoise);
	MATH_ScaleVector((MATH_tdst_Vector *)&SPG_Noise[1] , (MATH_tdst_Vector *)P2 , fNoise);
}
#else
asm void SPG_SetNoise(SOFT_tdst_AVertex *P1,SOFT_tdst_AVertex *P2 , float fNoise , SOFT_tdst_AVertex *SaveNoise)
{
    .set noreorder
	lqc2 	$vf28,0(a0)
	mfc1	t0,$f12
	lqc2 	$vf29,0(a1)
	qmtc2	t0,$vf10
	vmulx	$vf28,$vf28,$vf10x
	vmulx	$vf29,$vf29,$vf10x
	sqc2	$vf28,0x00(a2)
	jr 	ra
	sqc2	$vf29,0x10(a2)
}
#endif	

typedef struct	SPG_tdst_DepthVar_
{
	float fThreshold;
	float fNoiseFacotr;
	ULONG Depth;
	ULONG Mask;
} SPG_tdst_DepthVar;
typedef struct SPG_tdst_RasterizeColor_
{
	short	R,G,B,A;
} SPG_tdst_RasterizeColor;
typedef struct SPG_tdst_RasterizeVertex_
{
	SOFT_tdst_AVertex		P3D;
	SPG_tdst_RasterizeColor	C3D;
} SPG_tdst_RasterizeVertex;

_inline_ void SPG_AddRVertex(SPG_tdst_RasterizeVertex *pDst, SPG_tdst_RasterizeVertex *pB)
{
#ifndef PSX2_TARGET
	pDst->C3D.A += pB->C3D.A;
	pDst->C3D.B += pB->C3D.B;
	pDst->C3D.G += pB->C3D.G;
	pDst->C3D.R += pB->C3D.R;
	pDst->P3D.x += pB->P3D.x;
	pDst->P3D.y += pB->P3D.y;
	pDst->P3D.z += pB->P3D.z;
#else
	asm  __volatile__ ("
		.set noreorder
		lqc2	$vf10,0(pDst)
		ld		t0,0x10(pDst)
		lqc2	$vf11,0(pB)
		ld		t1,0x10(pB)
		vadd	$vf10,$vf10,$vf11
		paddsh	t0,t0,t1
		sqc2	$vf10,0(pDst)
		sd		t0,0x10(pDst)
		.set reorder
	");
#endif	
}
_inline_ void SPG_SubRV(SPG_tdst_RasterizeVertex *pDst, SPG_tdst_RasterizeVertex *pA, SPG_tdst_RasterizeVertex *pB)
{
#ifndef PSX2_TARGET
	pDst->C3D.A = pA->C3D.A - pB->C3D.A;
	pDst->C3D.B = pA->C3D.B - pB->C3D.B;
	pDst->C3D.G = pA->C3D.G - pB->C3D.G;
	pDst->C3D.R = pA->C3D.R - pB->C3D.R;
	pDst->P3D.x = pA->P3D.x - pB->P3D.x;
	pDst->P3D.y = pA->P3D.y - pB->P3D.y;
	pDst->P3D.z = pA->P3D.z - pB->P3D.z;
#else
	asm __volatile__("
		.set noreorder
		lqc2	$vf10,0(pA)
		ld		t0,0x10(pA)
		lqc2	$vf11,0(pB)
		ld		t1,0x10(pB)
		vsub	$vf10,$vf10,$vf11
		psubsh	t0,t0,t1
		sqc2	$vf10,0(pDst)
		sd		t0,0x10(pDst)
		.set reorder
	");
#endif	
}
#ifdef _XENON
_inline_ short Xe_ShiftShort( short value, ULONG Shift )
{
    LONG v = value;
    v >>= Shift;
    return (short)v;
}
#endif

_inline_ void SPG_ShiftRV(SPG_tdst_RasterizeVertex *pDst , ULONG Shift)
{

#ifdef _XENON
	float Diveder;
  	// must be a better way ... need the sign of the short to be replicated during shift (algeabric) but the shift is done on a word so the low word component (32bit) does not contain the signed value at bit 31 (it is at bit 15 since we use a short)
	pDst->C3D.A = Xe_ShiftShort(pDst->C3D.A, Shift);
	pDst->C3D.B = Xe_ShiftShort(pDst->C3D.B, Shift);
	pDst->C3D.G = Xe_ShiftShort(pDst->C3D.G, Shift);
	pDst->C3D.R = Xe_ShiftShort(pDst->C3D.R, Shift);

	Diveder = 1.0f;
	*(ULONG*)&Diveder -= Shift << 23;
	pDst->P3D.x *= Diveder;
	pDst->P3D.y *= Diveder;
	pDst->P3D.z *= Diveder;
#elif !defined(PSX2_TARGET)
	float Diveder;
	pDst->C3D.A >>= Shift;
	pDst->C3D.B >>= Shift;
	pDst->C3D.G >>= Shift;
	pDst->C3D.R >>= Shift;
	Diveder = 1.0f;
	*(ULONG*)&Diveder -= Shift << 23;
	pDst->P3D.x *= Diveder;
	pDst->P3D.y *= Diveder;
	pDst->P3D.z *= Diveder;
#else
	register ULONG Temp1,Temp2,Temp3,Temp4,Temp5;
	asm __volatile__("
		.set noreorder
		lqc2	$vf13 , 0(pDst)
		lq		Temp3 , 0x10(pDst)
		lui    	Temp1 ,	16256
		sll		Temp5 , Shift , 23
		andi	Temp2 , Shift , 4
		psrah	Temp4 , Temp3 , 4
		movn	Temp3 , Temp4 , Temp2

		sub 	Temp1 , Temp1 , Temp5
		qmtc2	Temp1 , $vf14
		
		andi	Temp2 , Shift , 2
		psrah	Temp4 , Temp3 , 2
		movn	Temp3 , Temp4 , Temp2

		vmulx	$vf13 , $vf13 , $vf14x
		
		andi	Temp2 , Shift , 1
		psrah	Temp4 , Temp3 , 1
		movn	Temp3 , Temp4 , Temp2
		
		
		sqc2	$vf13 , 0(pDst)
		sq		Temp3 , 0x10(pDst)
		.set reorder
	");//*/
#endif	
}
_inline_ void SPG_CreateRV(SPG_tdst_RasterizeVertex *pDst , GEO_Vertex *P1 , ULONG Color)
{
#ifndef PSX2_TARGET
	pDst->C3D.A = (short)((Color & 0xff000000) >> 17);
	pDst->C3D.B = (short)((Color & 0x00ff0000) >> 9);
	pDst->C3D.G = (short)((Color & 0x0000ff00) >> 1);
	pDst->C3D.R = (short)((Color & 0x000000ff) << 7);
	pDst->P3D.x = P1->x;
	pDst->P3D.y = P1->y;
	pDst->P3D.z = P1->z;
#else
	asm __volatile__("
		.set noreorder
		lq		t0,0(P1)
		pextlb	Color,Color,$0
		psrlw	Color,Color,1
		sq		t0,0(pDst)
		sd		Color,0x10(pDst)
		.set reorder
	");
#endif	
}
_inline_ void SPG_CopyRV(SPG_tdst_RasterizeVertex *pDst , SPG_tdst_RasterizeVertex *pSrc)
{
#ifndef PSX2_TARGET
	*pDst = *pSrc;
#else
	asm __volatile__("
		.set noreorder
		lq		t0,0x00(pSrc)
		ld		t1,0x10(pSrc)
		sq		t0,0x00(pDst)
		sd		t1,0x10(pDst)
		.set reorder
	");
#endif	
}
_inline_ ULONG SPG_PackColor(SPG_tdst_RasterizeVertex *P1)
{
#ifndef PSX2_TARGET
	return  ((P1->C3D.A >> 7) << 24) | ((P1->C3D.B >> 7) << 16) | ((P1->C3D.G >> 7) << 8) | ((P1->C3D.R >> 7) << 0);
#else	
	ULONG ReturnVal;
	asm __volatile__("
		.set noreorder
		ld		t0,0x10(P1)
		psrlh	t0,t0,7
		ppacb	ReturnVal,$0,t0
		.set reorder
	");
	return ReturnVal;
#endif
}
_inline_ void SPG_AddRVertexAndPackToDest(GEO_Vertex *p_dst , SPG_tdst_RasterizeVertex *POS,SPG_tdst_RasterizeVertex *INC , float *RandomPtr)
{
#ifndef PSX2_TARGET
		SPG_AddRVertex(POS, INC);
		MATH_AddScaleVector((MATH_tdst_Vector *)p_dst , (MATH_tdst_Vector *)&POS->P3D	, (MATH_tdst_Vector *)&SPG_Noise[0] , *RandomPtr);
		MATH_AddScaleVector((MATH_tdst_Vector *)p_dst , (MATH_tdst_Vector *)p_dst		, (MATH_tdst_Vector *)&SPG_Noise[1] , *(RandomPtr + 1));
		*(gvCOLORStack++) = SPG_PackColor(POS);
#else
	asm __volatile__("
		.set noreorder
		vadd	$vf10,$vf10,$vf11
		lw		t6,0x0(RandomPtr)
		lw		t5,0x4(RandomPtr)
		qmtc2	t6,$vf15
		qmtc2	t5,$vf16
		paddsh	t8,t8,t9
		vmulax	$acc,$vf28,$vf15x
		vmaddax $acc,$vf29,$vf16x
		vmaddw 	$vf12,$vf10,$vf00w
		psrlh	t7,t8,7
		ppacb	t7,$0,t7
		sqc2	$vf12,0(p_dst)
		sw		t7,0x0c(p_dst)
		.set reorder
	");
#endif	
}
_inline_ void SPG_PopSerialSpg(SPG_tdst_RasterizeVertex *POS,SPG_tdst_RasterizeVertex *INC , SOFT_tdst_AVertex *SaveNoise)
{
#ifdef PSX2_TARGET
	asm __volatile__("
		.set noreorder
		lqc2	$vf10,0(POS)
		ld		t8,0x10(POS)
		lqc2	$vf11,0(INC)
		ld		t9,0x10(INC)
		lqc2	$vf28,0x00(SaveNoise)
		lqc2	$vf29,0x10(SaveNoise)
		.set reorder
	");
#endif	
}
_inline_ void SPG_PushSerialSpg(SPG_tdst_RasterizeVertex *POS,SPG_tdst_RasterizeVertex *INC , SOFT_tdst_AVertex *SaveNoise)
{
#ifdef PSX2_TARGET
	asm __volatile__("
		.set noreorder
		sqc2	$vf10,0(POS)
		sd		t8,0x10(POS)
		sqc2	$vf11,0(INC)
		sd		t9,0x10(INC)
		sqc2	$vf28,0x00(SaveNoise)
		sqc2	$vf29,0x10(SaveNoise)

		.set reorder
	");
#endif	
}

void SPG_SerialSpriter(SPG_tdst_RasterizeVertex *POS,SPG_tdst_RasterizeVertex *INC,ULONG NUM ,SOFT_tdst_AVertex *SaveNoise)
{
	register GEO_Vertex *p_StackPtr,*p_StackPtrLast,*p_StackPtrVeryLast;
	register float *p_RandomTable;
	p_StackPtr = gvStack + gvBase;
	p_StackPtrLast = gvStack + SPG_Nb_Max_SpritePerPass;
	p_StackPtrVeryLast = p_StackPtr + NUM;
	p_RandomTable = f_RandomTable + gvBase;
	SPG_PopSerialSpg(POS,INC,SaveNoise);
	while (p_StackPtrVeryLast > p_StackPtrLast)
	{
		while (p_StackPtr < p_StackPtrLast)
		{
			SPG_AddRVertexAndPackToDest(p_StackPtr++ , POS,INC , p_RandomTable++);
		}
		SPG_PushSerialSpg(POS,INC,SaveNoise);
		SPG_Draw(SPG_Nb_Max_SpritePerPass);
		SPG_PopSerialSpg(POS,INC,SaveNoise);
		p_StackPtr-=SPG_Nb_Max_SpritePerPass;
		p_StackPtrVeryLast-=SPG_Nb_Max_SpritePerPass;
		p_RandomTable -= SPG_Nb_Max_SpritePerPass;
	}
	while (p_StackPtr < p_StackPtrVeryLast)
	{
		SPG_AddRVertexAndPackToDest(p_StackPtr++ , POS,INC , p_RandomTable++);
	}
	gvBase = p_StackPtr - gvStack;//*/
}

float GET_SqrLenght(SPG_tdst_RasterizeVertex *D)
{
	return D->P3D.x * D->P3D.x + D->P3D.y * D->P3D.y + D->P3D.z * D->P3D.z;
}

_inline_ ULONG SPG_GetDepth(ULONG OriginalDepth ,float Threshold , SPG_tdst_RasterizeVertex *D1 , SPG_tdst_RasterizeVertex *D2)
{	
	float fD1 ;
	ULONG ulComputedDepth ;
	ulComputedDepth  = 0;
	fD1 = fMin(GET_SqrLenght(D1),GET_SqrLenght(D2));
	while ((fD1 > Threshold) && (ulComputedDepth < OriginalDepth))
	{
		fD1 *= 0.25f;
		ulComputedDepth++;
	}
	return ulComputedDepth;
}

void SPG_TransformTrisToSprites(GEO_Vertex *P1 , GEO_Vertex *P2 , GEO_Vertex *P3 , ULONG *p_3Colors, SPG_tdst_DepthVar *p_DV )
{
	SOFT_tdst_AVertex 		SaveNoise[2] ONLY_PSX2_ALIGNED(16);
	SPG_tdst_RasterizeVertex V12,V13,PA,PB,PC , PT1 , PT2 ONLY_PSX2_ALIGNED(16);
	register ULONG Counter,Counter12,ulComputedDepth;

	SPG_CreateRV(&PA,  P1 , p_3Colors[0]);
	SPG_CreateRV(&PB,  P2 , p_3Colors[1]);
	SPG_CreateRV(&PC,  P3 , p_3Colors[2]);
	SPG_SubRV(&V12, &PB, &PA);
	SPG_SubRV(&V13, &PC, &PA);
	if (p_DV->fThreshold) ulComputedDepth = SPG_GetDepth(p_DV->Depth , p_DV->fThreshold , &V12 , &V13);
	else ulComputedDepth = p_DV->Depth;
	if (!ulComputedDepth) return;
	SPG_ShiftRV(&V12 , ulComputedDepth);
	SPG_ShiftRV(&V13 , ulComputedDepth);
	Counter12 = 1<<ulComputedDepth;
	Counter12--;
	SPG_SetNoise(&V12.P3D,&V13.P3D,p_DV->fNoiseFacotr , SaveNoise);
	SPG_CopyRV(&PT1 , &PA);
	Counter = Counter12;
	while (Counter --)
	{
		SPG_AddRVertex(&PT1, &V12);
		SPG_CopyRV(&PT2 , &PT1);
		SPG_SerialSpriter(&PT2,&V13,Counter , SaveNoise);
	}
	if (p_DV->Mask & 1)
	{
		SPG_CopyRV(&PT1 , &PA);
		SPG_SerialSpriter(&PT1,&V12,Counter12 , SaveNoise);
	}
	if (p_DV->Mask & 2)
	{
		SPG_CopyRV(&PT1 , &PB);
		SPG_SubRV(&V12, &PC, &PB);
		SPG_ShiftRV(&V12, ulComputedDepth);
		SPG_SerialSpriter(&PT1,&V12,Counter12  , SaveNoise);
	}
	if (p_DV->Mask & 4)
	{
		SPG_CopyRV(&PT1 , &PA);
		SPG_SubRV(&V12, &PC, &PA);
		SPG_ShiftRV(&V12, ulComputedDepth);
		SPG_SerialSpriter(&PT1,&V12,Counter12  , SaveNoise);
	}//*/
}
#ifdef USE_DOUBLE_RENDERING	
void SPG_Modifier_Interpolate(struct MDF_tdst_Modifier_ * p_Mod, u_int mode , float fInterpolatedValue)
{
	/* MODE : 
			101 -> 	Must copy current in S0  				// Camera Cut Mode 
			000 -> 	Must copy S0 in S1 , and Current in S0	// First I frame
			0XX ->  Must blend S1 -> S0 with interpolvalue	// I Frame
			100 -> 	Must copy S0 in current					// K frame
	*/
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SPG_tdst_Modifier *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	pst_Data = (SPG_tdst_Modifier *) p_Mod->p_Data;

	if (*(ULONG *)&pst_Data->f_Noise_Out) pst_Data->f_Noise = pst_Data->f_Noise_Out;
	*(ULONG *)&pst_Data->f_Noise_Out = 0;
	if (!(*(ULONG *)&pst_Data->f_Noise_0 & 1)) mode = 101;
	if (mode < 100)
	{
		if (mode == 0)		
		{
			pst_Data->f_Noise_1 = pst_Data->f_Noise_0;
			pst_Data->f_Noise_0 = pst_Data->f_Noise;
		}
		pst_Data->f_Noise = pst_Data->f_Noise_1 * (1.0f - fInterpolatedValue) + pst_Data->f_Noise_0 * (fInterpolatedValue);
	} 
	else
		if (mode == 100) // K 1
		{
			pst_Data->f_Noise = pst_Data->f_Noise_0;
		} else
			if (mode == 101) // K 2 (camera cut)
			{
				pst_Data->f_Noise_1 = pst_Data->f_Noise_0 = pst_Data->f_Noise;
			} 
	*(ULONG *)&pst_Data->f_Noise_0 |= 1;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SPG_Modifier_Unapply
(
	MDF_tdst_Modifier		*_pst_Mod,
	GEO_tdst_Object			*_pst_Obj
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SPG_tdst_Modifier *pst_Data;
    GEO_Vertex                      *pst_Point;
	ULONG							Counter2 , *pColor;
	float 							fCorrectionFacor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (SPG_tdst_Modifier *) _pst_Mod->p_Data;
	
#ifdef ACTIVE_EDITORS
	pst_Data->ulNumberofUsedTriangles = pst_Data->ulNumberofgeneratedSprites = gsulNumberofgeneratedSprites = 0;
#endif


	fCorrectionFacor = ((float)_pst_Mod->pst_GO->uc_LOD_Vis) * pst_Data -> fLODCorrectionFactor * 4.0F / 256.0f;

	fCorrectionFacor = fMax(0.0f , fMin(fCorrectionFacor , 1.0f));
	fCorrectionFacor *= fCorrectionFacor;

	if (fCorrectionFacor < 0.5f) return;
	
#ifdef PSX2_TARGET
#ifdef USE_GO_DATA
	{
		{
			if (_pst_Mod->pst_GO)
			{
				if (_pst_Mod->pst_GO->DataCacheOptimization)
				{
					extern void GSP_Free_DC_DATA(void*);
					GSP_Free_DC_DATA((void  *)_pst_Mod->pst_GO->DataCacheOptimization);
					_pst_Mod->pst_GO->DataCacheOptimization = NULL;
				}
			}
		}
	}
#endif
#endif //*/				
	

	if (pst_Data->ulFlags & SPG_IsMaterialTransparent)
	{
		if (!(GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_RenderingTransparency))
		{
			/* Add object in ZList */
			SOFT_AddCurrentObjectInZList(&GDI_gpst_CurDD_SPR,_pst_Mod->pst_GO);
			return;
		}
	} else if (GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_RenderingTransparency) return;

	if ((ULONG)pst_Data->p_Material == (ULONG)-1)
	{
		pst_Data->p_Material = NULL;
#if 0 // TEMPORAIRE : FAIT PLANTER EN BINARISATION
		if (pst_Data->bk_MaterialID != (ULONG)-1)
		{
			LOA_MakeFileRef(pst_Data->bk_MaterialID,(ULONG *) &pst_Data->p_Material , GEO_ul_Load_ObjectCallback, LOA_C_MustExists);
			LOA_Resolve();
		}
#endif
	}

_GSP_BeginRaster(42);
	pst_Point = GDI_gpst_CurDD->p_Current_Vertex_List;

	pst_CurrentObj = _pst_Obj;
	pst_CurrentMat = (MAT_tdst_Material *)_pst_Mod->pst_GO->pst_Base->pst_Visu->pst_Material;

	ulRandomPos = 0;

#if defined(_XBOX)

	if ( ( _pst_Obj->dst_Element ) && 
		 ( _pst_Obj->dst_Element[0].pst_Gx8Add ) && 
		 ( _pst_Obj->dst_Element[0].pst_Gx8Add->pSpriteVBData ) &&
		 ( ( _pst_Obj->dst_Element[0].pst_Gx8Add->uiUpdateHistory & 0x00000001 ) == 0x00000000 ) )	// it has NOT been updated this frame
	{
			// draw Sprite VB
		Gx8_DrawSpriteVB( _pst_Obj, pst_CurrentMat );
		return;
	}
	else
	{
		assert( _pst_Obj->dst_Element );

		if ( ( _pst_Obj->dst_Element[0].pst_Gx8Add ) &&
			( _pst_Obj->dst_Element[0].pst_Gx8Add->uiUpdateHistory & 0x00000003 ) != 0x00000003 )	// it has NOT been updated in both the last 2 frames
		{
				// Begin Sprite VB recording
			Gx8_BeginSpriteVBCreation();
		}
	}

#endif	// defined(_XBOX)

	if (pst_Data->p_Material )
	{
		pst_CurrentMat = pst_Data->p_Material;
	} else pst_CurrentMat = (MAT_tdst_Material *)_pst_Mod->pst_GO->pst_Base->pst_Visu->pst_Material;

	
	stIndxSp[0].fGlobalRatio	= pst_Data->f_GlobalRatio;
 	stIndxSp[0].fGlobalSize	= pst_Data->f_GlobalSize;
	stIndxSp[0].l_NbSprites	= lMin(_pst_Obj->l_NbPoints , SPG_Nb_Max_SpritePerPass);
	stIndxSp[0].l_MaterialId	= 0;
	stIndxSp[0].dst_Sprite		= SpritesIDX;
	Counter2 				= _pst_Obj->l_NbPoints;
	pst_Data->ulMaxDepth	= lMin(pst_Data->ulMaxDepth , 8);
	pst_Data->ulMaxDepth	= lMax(pst_Data->ulMaxDepth , 0);
	/* Draw sprite per point */
	while (Counter2)
	{
		ULONG Counter;
		Counter = lMin(Counter2 , SPG_Nb_Max_SpritePerPass);
		Counter2 -= Counter;
		stIndxSp[0].l_NbSprites = Counter;
		while (Counter--) 
		{
			*(float *)&SpritesIDX[Counter] = 1.0f;
			SpritesIDX[Counter].auw_Index = (unsigned short)(Counter + Counter2);
		}
//		MAT_DrawIndexedSprites(pst_CurrentObj ,pst_CurrentMat  ,stIndxSp);
	}
	/* Init sprite list  */
	stIndxSp[0].l_NbSprites	= stIndxSp[1].l_NbSprites	= stIndxSp[2].l_NbSprites	= stIndxSp[3].l_NbSprites	= SPG_Nb_Max_SpritePerPass >> 2;
	stIndxSp[0].l_MaterialId	= pst_Data->stSED[0].ulSubElementMaterialNumber & 0xff;
	stIndxSp[1].l_MaterialId	= pst_Data->stSED[1].ulSubElementMaterialNumber & 0xff;
	stIndxSp[2].l_MaterialId	= pst_Data->stSED[2].ulSubElementMaterialNumber & 0xff;
	stIndxSp[3].l_MaterialId	= pst_Data->stSED[3].ulSubElementMaterialNumber & 0xff;
#ifdef PSX2_TARGET
	if (MATH_b_TestScaleType(GDI_gpst_CurDD ->pst_CurrentGameObject->pst_GlobalMatrix))
	{
		fCorrectionFacor *= ( GDI_gpst_CurDD ->pst_CurrentGameObject->pst_GlobalMatrix->Sx + GDI_gpst_CurDD ->pst_CurrentGameObject->pst_GlobalMatrix->Sy + GDI_gpst_CurDD ->pst_CurrentGameObject->pst_GlobalMatrix->Sz ) * 0.33333f; 
	}
	
#endif
	
	stIndxSp[0].fGlobalSize = pst_Data->f_GlobalSize * pst_Data->stSED[0].fSizeFactor * fCorrectionFacor;
	stIndxSp[1].fGlobalSize = pst_Data->f_GlobalSize * pst_Data->stSED[1].fSizeFactor * fCorrectionFacor;
	stIndxSp[2].fGlobalSize = pst_Data->f_GlobalSize * pst_Data->stSED[2].fSizeFactor * fCorrectionFacor;
	stIndxSp[3].fGlobalSize = pst_Data->f_GlobalSize * pst_Data->stSED[3].fSizeFactor * fCorrectionFacor;

	stIndxSp[0].fGlobalRatio = pst_Data->f_GlobalRatio * pst_Data->stSED[0].fRatioFactor;
	stIndxSp[1].fGlobalRatio = pst_Data->f_GlobalRatio * pst_Data->stSED[1].fRatioFactor;
	stIndxSp[2].fGlobalRatio = pst_Data->f_GlobalRatio * pst_Data->stSED[2].fRatioFactor;
	stIndxSp[3].fGlobalRatio = pst_Data->f_GlobalRatio * pst_Data->stSED[3].fRatioFactor;

	stIndxSp[1].dst_Sprite		= stIndxSp[0].dst_Sprite + (SPG_Nb_Max_SpritePerPass >> 2);
	stIndxSp[2].dst_Sprite		= stIndxSp[1].dst_Sprite + (SPG_Nb_Max_SpritePerPass >> 2);
	stIndxSp[3].dst_Sprite		= stIndxSp[2].dst_Sprite + (SPG_Nb_Max_SpritePerPass >> 2);


	Counter2= SPG_Nb_Max_SpritePerPass;
	while (Counter2--) 
	{
		*(float *)&SpritesIDX[Counter2] = 1.0f;
	}
	Counter2= SPG_Nb_Max_SpritePerPass >> 2;
	while (Counter2--) 
	{
		*(float *)&SpritesIDX[Counter2 + 0 * (SPG_Nb_Max_SpritePerPass >> 2)] -= pst_Data->stSED[0].fSizeNoiseFactor * f_RandomTable[Counter2 & SPG_RandomNumber];
		*(float *)&SpritesIDX[Counter2 + 1 * (SPG_Nb_Max_SpritePerPass >> 2)] -= pst_Data->stSED[1].fSizeNoiseFactor * f_RandomTable[Counter2 & SPG_RandomNumber];
		*(float *)&SpritesIDX[Counter2 + 2 * (SPG_Nb_Max_SpritePerPass >> 2)] -= pst_Data->stSED[2].fSizeNoiseFactor * f_RandomTable[Counter2 & SPG_RandomNumber];
		*(float *)&SpritesIDX[Counter2 + 3 * (SPG_Nb_Max_SpritePerPass >> 2)] -= pst_Data->stSED[3].fSizeNoiseFactor * f_RandomTable[Counter2 & SPG_RandomNumber];
		SpritesIDX[Counter2 + 0 * (SPG_Nb_Max_SpritePerPass >> 2)].auw_Index = (unsigned short)((Counter2 << 2) + 0 + _pst_Obj->l_NbPoints);
		SpritesIDX[Counter2 + 1 * (SPG_Nb_Max_SpritePerPass >> 2)].auw_Index = (unsigned short)((Counter2 << 2) + 1 + _pst_Obj->l_NbPoints);
		SpritesIDX[Counter2 + 2 * (SPG_Nb_Max_SpritePerPass >> 2)].auw_Index = (unsigned short)((Counter2 << 2) + 2 + _pst_Obj->l_NbPoints);
		SpritesIDX[Counter2 + 3 * (SPG_Nb_Max_SpritePerPass >> 2)].auw_Index = (unsigned short)((Counter2 << 2) + 3 + _pst_Obj->l_NbPoints);
	}

//	L_memset(GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors , 0 , sizeof(ULONG) * SOFT_Cul_ComputingBufferSize);
	gvCurrentStack = gvStack - _pst_Obj->l_NbPoints;
#ifdef _XENON_RENDER
	ULONG *pOldColorBuffer = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;
	GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors = GDI_gpst_CurDD->pst_ComputingBuffers->staticComputedColors;
#endif
	gvCOLORStack = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors + _pst_Obj->l_NbPoints;
	pColor = GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors;

	if (pst_Data->ulFlags & SPG_GrassMove) GDI_gb_WaveSprite = TRUE;

		// special fog
	if (pst_Data->ulFlags & SPG_SpecialFog)
	{
		gs_SpecialFogCOLOR =	pst_Data->ulSpecialFogColor | 1;
		gs_SpecialFogCOLOR &= ~0x100;
		if (pst_Data->ulFlags & SPG_FogCulling) gs_SpecialFogCOLOR |= 0x100;
		gs_SpecialFogNear =		pst_Data->fSpecialFogNear;
		gs_SpecialFogFar =		pst_Data->fSpecialFogFar;
		if (gs_SpecialFogFar == gs_SpecialFogNear) gs_SpecialFogFar = gs_SpecialFogNear + 0.1f;
		gs_SpecialFogFar = 1.0f / (gs_SpecialFogFar - gs_SpecialFogNear);
		gs_SpecialFogAxis.x = gs_SpecialFogFar * GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Iz;
		gs_SpecialFogAxis.y = gs_SpecialFogFar * GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Jz;
		gs_SpecialFogAxis.z = gs_SpecialFogFar * GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Kz;
		gs_SpecialFogCulledPoint.x = gs_SpecialFogCulledPoint.y = gs_SpecialFogCulledPoint.z = 100000.0f; // ça c'est du culling!.
		gs_SpecialFogDistance = gs_SpecialFogFar * (gs_SpecialFogNear-GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->T.z);
	} 
	else
	if (pst_Data->ulFlags & SPG_IsMaterialTransparent)	
	{

		/* Philippe -> Force fog near */
		gs_SpecialFogCOLOR =	pst_Data->ulSpecialFogColor | 1;
		gs_SpecialFogCOLOR |= 0x100;
		gs_SpecialFogNear =		pst_Data->f_GlobalSize * 3;
		gs_SpecialFogFar =		pst_Data->f_GlobalSize;
		
		gs_SpecialFogFar = 1.0f / (gs_SpecialFogFar - gs_SpecialFogNear);
		gs_SpecialFogAxis.x = gs_SpecialFogFar * GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Iz;
		gs_SpecialFogAxis.y = gs_SpecialFogFar * GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Jz;
		gs_SpecialFogAxis.z = gs_SpecialFogFar * GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->Kz;
		gs_SpecialFogCulledPoint.x = gs_SpecialFogCulledPoint.y = gs_SpecialFogCulledPoint.z = 100000.0f; // ça c'est du culling!.
		gs_SpecialFogDistance = gs_SpecialFogFar * (gs_SpecialFogNear-GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix->T.z);
	}
	else gs_SpecialFogCOLOR = 0;
	/* Then parse triangles */
	if (pst_Data->ulMaxDepth)
	{
		GEO_tdst_ElementIndexedTriangles	*p_stInd3,*p_stInd3L;
		SPG_tdst_DepthVar					stDV;
		p_stInd3 = _pst_Obj->dst_Element;
		p_stInd3L = p_stInd3 + _pst_Obj->l_NbElements;
		FIndx	= 10000;
		gvBase = 0;
		stDV .Depth = pst_Data->ulMaxDepth;
		stDV .fNoiseFacotr = pst_Data->f_Noise;
 		stDV .fThreshold = pst_Data->f_ThresholdMin * pst_Data->f_ThresholdMin;
		GEO_UseNormals(_pst_Obj);
 		

		while (p_stInd3 < p_stInd3L)
		{
			if ((1 << p_stInd3->l_MaterialId) & pst_Data->ulSubMaterialMask)
			{
#ifdef _GAMECUBE
				if(p_stInd3->dl)
				{
					u16     	p1, p2, p3;
					BOOL    	bflip;
					u8		*Stream,*StreamLast;
					u32 		Pitch , Mode8;
					Stream = p_stInd3->dl;
					StreamLast = Stream + (p_stInd3->dl_size & 0x0fffffff);
					Pitch = 8;
					Mode8 = 0;
					if (p_stInd3->dl_size & 0x80000000) {Pitch -= 2;Mode8 = 1;};
					if (p_stInd3->dl_size & 0x40000000) Pitch -= 1;
					if (p_stInd3->dl_size & 0x20000000) Pitch -= 1;
					while (Stream < StreamLast)
					{
						{
							u16 lNbVertex;
							u8		*StreamStripLast;
							
							Stream++; // Flags
							lNbVertex = *(u16 *)Stream;	// Number Of Vertexes
							Stream += 2; 				// Number Of Vertexes
						    bflip=FALSE;
							StreamStripLast = Stream + lNbVertex * Pitch;
							if (Mode8)
							{
							    p1 = (u32)*(u8 *)Stream;Stream += Pitch;
							    p2 = (u32)*(u8 *)Stream;Stream += Pitch;
							} else
							{
							    p1 = (u32)*(u16*)Stream;Stream += Pitch;
							    p2 = (u32)*(u16*)Stream;Stream += Pitch;
							}
							while (Stream < StreamStripLast) // Parse VertexIndexes
							{
								GEO_Vertex	stStartingPoints[3] ONLY_PSX2_ALIGNED(16);
								ULONG 		Color3[3];
								
								if (Mode8)  p3 = (u32)*(u8 *)Stream;
								else 		p3 = (u32)*(u16*)Stream;
							
								Color3[0] = pColor[p1];
								Color3[1] = pColor[p2];
								Color3[2] = pColor[p3];
								stDV.Mask= 0;
								if (p1 > p2) stDV.Mask |= 1;
								if (p2 > p3) stDV.Mask |= 2;
								if (p3 > p1) stDV.Mask |= 4;
								MATH_AddScaleVector(stStartingPoints + 0 , pst_Point + p1 , _pst_Obj->dst_PointNormal + p1 , pst_Data->f_Extraction);
								MATH_AddScaleVector(stStartingPoints + 1 , pst_Point + p2 , _pst_Obj->dst_PointNormal + p2 , pst_Data->f_Extraction);
								MATH_AddScaleVector(stStartingPoints + 2 , pst_Point + p3 , _pst_Obj->dst_PointNormal + p3 , pst_Data->f_Extraction);//*/
								SPG_TransformTrisToSprites(	&stStartingPoints[0],&stStartingPoints[1],&stStartingPoints[2],Color3 ,&stDV  );
								p1 = p2;
								p2 = p3;
								bflip =!bflip;
								
								Stream += Pitch;
							}
						}
						while ((*Stream == 0) && (Stream < StreamLast)) Stream++; // Skip DX_NOP
					}
				}
				else
#endif
				{
					GEO_tdst_IndexedTriangle	*p_Triangle, *p_TriangleL;
#ifdef PSX2_TARGET
					GEO_I_Need_The_Triangles_Begin_Element(p_stInd3,0);
#endif					
					p_Triangle = p_stInd3->dst_Triangle;
					p_TriangleL = p_Triangle + p_stInd3->l_NbTriangles;
					while (p_Triangle < p_TriangleL)
					{
						GEO_Vertex							stStartingPoints[3] ONLY_PSX2_ALIGNED(16);
						ULONG Color3[3];
	#ifdef ACTIVE_EDITORS
						pst_Data->ulNumberofUsedTriangles ++;
	#endif
						Color3[0] = pColor[p_Triangle ->auw_Index[0]];
						Color3[1] = pColor[p_Triangle ->auw_Index[1]];
						Color3[2] = pColor[p_Triangle ->auw_Index[2]];
						stDV.Mask= 0;
						if (p_Triangle ->auw_Index[0] > p_Triangle ->auw_Index[1]) stDV.Mask |= 1;
						if (p_Triangle ->auw_Index[1] > p_Triangle ->auw_Index[2]) stDV.Mask |= 2;
						if (p_Triangle ->auw_Index[2] > p_Triangle ->auw_Index[0]) stDV.Mask |= 4;
						MATH_AddScaleVector(stStartingPoints + 0 , pst_Point + p_Triangle ->auw_Index[0] , _pst_Obj->dst_PointNormal + p_Triangle ->auw_Index[0] , pst_Data->f_Extraction);
						MATH_AddScaleVector(stStartingPoints + 1 , pst_Point + p_Triangle ->auw_Index[1] , _pst_Obj->dst_PointNormal + p_Triangle ->auw_Index[1] , pst_Data->f_Extraction);
						MATH_AddScaleVector(stStartingPoints + 2 , pst_Point + p_Triangle ->auw_Index[2] , _pst_Obj->dst_PointNormal + p_Triangle ->auw_Index[2] , pst_Data->f_Extraction);//*/
						SPG_TransformTrisToSprites(	&stStartingPoints[0],
													&stStartingPoints[1],
													&stStartingPoints[2],
													Color3 ,
													&stDV  );
						p_Triangle ++;
					}
#ifdef PSX2_TARGET
					GEO_I_Need_The_Triangles_End_Element(p_stInd3,0);
#endif					
					
				}
			}
			p_stInd3++;
		}
		
 		
		
		if (gvBase)
		{
			GEO_Vertex *pSaveVL; 
			stIndxSp[0].l_NbSprites = gvBase >> 2;
			stIndxSp[1].l_NbSprites = gvBase >> 2;
			stIndxSp[2].l_NbSprites = gvBase >> 2;
			stIndxSp[3].l_NbSprites = gvBase >> 2;
			gvBase &= 3;
			while (gvBase--) stIndxSp[gvBase].l_NbSprites++;
			pSaveVL = GDI_gpst_CurDD->p_Current_Vertex_List;
			GDI_gpst_CurDD->p_Current_Vertex_List = gvCurrentStack;
			{
				ULONG ulNumber;
				ulNumber = stIndxSp[0].l_NbSprites + stIndxSp[1].l_NbSprites + stIndxSp[2].l_NbSprites + stIndxSp[3].l_NbSprites;
				if (gs_SpecialFogCOLOR) 
				{
					if (gs_SpecialFogCOLOR & 0x100)
						SPG_ComputeSpriteFog_Cull(gvStack , gvCOLORStack - ulNumber, ulNumber );
					else
						SPG_ComputeSpriteFog(gvStack , gvCOLORStack - ulNumber, ulNumber );
				}
			}


#ifdef ACTIVE_EDITORS
			gsulNumberofgeneratedSprites += stIndxSp[0].l_NbSprites + stIndxSp[1].l_NbSprites + stIndxSp[2].l_NbSprites + stIndxSp[3].l_NbSprites;
#endif
			if (stIndxSp[0].l_NbSprites) MAT_DrawIndexedSprites(pst_CurrentObj,pst_CurrentMat,&stIndxSp[0]);
			if (stIndxSp[1].l_NbSprites) MAT_DrawIndexedSprites(pst_CurrentObj,pst_CurrentMat,&stIndxSp[1]);
			if (stIndxSp[2].l_NbSprites) MAT_DrawIndexedSprites(pst_CurrentObj,pst_CurrentMat,&stIndxSp[2]);
			if (stIndxSp[3].l_NbSprites) MAT_DrawIndexedSprites(pst_CurrentObj,pst_CurrentMat,&stIndxSp[3]);
			GDI_gpst_CurDD->p_Current_Vertex_List = pSaveVL;
		}
	}

	GDI_gb_WaveSprite = FALSE;
#ifdef ACTIVE_EDITORS
	pst_Data->ulNumberofgeneratedSprites = gsulNumberofgeneratedSprites;
#endif

#if defined(_XBOX)

	if ( _pst_Obj->dst_Element[0].pst_Gx8Add )
	{
			// shift history
		_pst_Obj->dst_Element[0].pst_Gx8Add->uiUpdateHistory = _pst_Obj->dst_Element[0].pst_Gx8Add->uiUpdateHistory << 1;

		if ( Gx8_CheckSpriteVBCreation() )
		{
			if ( ! _pst_Obj->dst_Element[0].pst_Gx8Add )
			{		// Create Gx8AddInfo if needed (for each element)
				Gx8_CreateAddInfo( _pst_Obj );
			}

			if ( ! _pst_Obj->dst_Element[0].pst_Gx8Add->pSpriteVBData )
			{		// Create SpriteVBData if needed (only for the first element)
				Gx8_CreateSpriteVBData( _pst_Obj );
			}

				// End Sprite VB recording
			Gx8_EndSpriteVBCreation( _pst_Obj, stIndxSp );

				// Draw Sprite VB
	//		Gx8_DrawSpriteVB( _pst_Obj, pst_CurrentMat );
		}
	}
#endif	// defined(_XBOX)

#ifdef _XENON_RENDER
	GDI_gpst_CurDD->pst_ComputingBuffers->ComputedColors = pOldColorBuffer;
#endif

_GSP_EndRaster(42);
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void _LOA_ReadSpriteMapper(CHAR ** _ppcBuffer, SPG_SpriteMapper * _Data, LOA_eBinFlags _eBinFlags)
{
	SKIP_IF_NULL_DATA(*_ppcBuffer, _Data, sizeof(SPG_SpriteMapper));
	M_OPT_LOAD_MEMCPY(*_ppcBuffer, _Data, SPG_SpriteMapper);

	/* Global Position */
	_LOA_ReadUChar(_ppcBuffer, &_Data->UShift, _eBinFlags);
	_LOA_ReadUChar(_ppcBuffer, &_Data->Uadd, _eBinFlags);
	_LOA_ReadUChar(_ppcBuffer, &_Data->VShift, _eBinFlags);
	_LOA_ReadUChar(_ppcBuffer, &_Data->Vadd, _eBinFlags);
	/* Sub-image description */
	_LOA_ReadUChar(_ppcBuffer, &_Data->USubFrameShift, _eBinFlags);
	_LOA_ReadUChar(_ppcBuffer, &_Data->VSubFrameShift, _eBinFlags);
	_LOA_ReadUChar(_ppcBuffer, &_Data->AnimationFrameDT, _eBinFlags);
	_LOA_ReadUChar(_ppcBuffer, &_Data->AnimOffset, _eBinFlags);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */

ULONG SPG_ul_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SPG_tdst_Modifier				*pst_Data;
	ULONG							ul_Size;
	ULONG							ul_Version;
	unsigned char					*pc_Cur;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	pc_Cur = (unsigned char*)_pc_Buffer;
	pst_Data = (SPG_tdst_Modifier *) _pst_Mod->p_Data;
   /* Read Size */
	ul_Size = LOA_ReadLong((CHAR**)&pc_Cur);
   /* Read version */
	ul_Version = LOA_ReadLong((CHAR**)&pc_Cur);
	SPG_InitModifier(pst_Data);
	/* Read fSize */
	pst_Data->f_GlobalSize = LOA_ReadFloat((CHAR**)&pc_Cur);
	/* Read ratio */
	pst_Data->f_GlobalRatio = LOA_ReadFloat((CHAR**)&pc_Cur);
	/* Read Material */
	*(ULONG *)&pst_Data->bk_MaterialID = LOA_ReadULong((CHAR**)&pc_Cur);
	*(ULONG *)&pst_Data->bk_MaterialID = -1;	// BINARISATION
	if (ul_Version > 0)
	{
		/* read Extraction */
		pst_Data->f_Extraction= LOA_ReadFloat((CHAR**)&pc_Cur);
		/* read Threshold */
		pst_Data->f_ThresholdMin= LOA_ReadFloat((CHAR**)&pc_Cur);
		/* read Depth */
		pst_Data->ulMaxDepth= LOA_ReadULong((CHAR**)&pc_Cur);
		if (ul_Version > 1)
		{
			/* read Depth */
			pst_Data->ulSubMaterialMask= LOA_ReadULong((CHAR**)&pc_Cur);

			if (ul_Version > 2)
			{
				/* read Noise */
				pst_Data->f_Noise = LOA_ReadFloat((CHAR**)&pc_Cur);
				if (ul_Version > 3)
				{
					ULONG i;
					for(i = 0; i < 4; ++i)
					{
						LOA_ReadSpriteMapper((CHAR**)&pc_Cur, pst_Data->stSpriteMapper+i);
					}
					if (ul_Version > 4)
					{
						for(i = 0; i < 4; ++i)
						{
							pst_Data->stSED[i].ulSubElementMaterialNumber = LOA_ReadULong((CHAR**)&pc_Cur);
							pst_Data->stSED[i].fSizeFactor = LOA_ReadFloat((CHAR**)&pc_Cur);
							pst_Data->stSED[i].fSizeNoiseFactor = LOA_ReadFloat((CHAR**)&pc_Cur);
							pst_Data->stSED[i].fRatioFactor = LOA_ReadFloat((CHAR**)&pc_Cur);
#ifdef ACTIVE_EDITORS
							if (_isnan(pst_Data->stSED[i].fSizeFactor)) pst_Data->stSED[i].fSizeFactor = 0.0f;
							if (_isnan(pst_Data->stSED[i].fSizeNoiseFactor)) pst_Data->stSED[i].fSizeNoiseFactor = 0.0f;
							if (_isnan(pst_Data->stSED[i].fRatioFactor)) pst_Data->stSED[i].fRatioFactor = 0.0f;
#endif							

						}

						if (ul_Version > 5)
						{
							/* read LODCF */
							pst_Data->fLODCorrectionFactor = LOA_ReadFloat((CHAR**)&pc_Cur);
							if (ul_Version > 6)
							{
								/* read flags */
								pst_Data->ulFlags = LOA_ReadULong((CHAR**)&pc_Cur);
								if (ul_Version > 7)
								{
									/* read fog */
									pst_Data->fSpecialFogNear = LOA_ReadFloat((CHAR**)&pc_Cur);
									pst_Data->fSpecialFogFar = LOA_ReadFloat((CHAR**)&pc_Cur);
									pst_Data->ulSpecialFogColor = LOA_ReadULong((CHAR**)&pc_Cur);
								}
							}
						}
					} 
				} 
			} 
		} 
	}

	*(ULONG*)&pst_Data->p_Material = (ULONG)-1;


#ifdef ACTIVE_EDITORS
	pst_Data->ulCodeKey = 0xC0DE2002;
#endif

//#ifndef ACTIVE_EDITORS
#if 0 // TEMPORAIRE : FAIT PLANTER LA BINARISATION
	if ((ULONG)pst_Data->p_Material == (ULONG)-1)
	{
		pst_Data->p_Material = NULL;
		if (pst_Data->bk_MaterialID != (ULONG)-1)
		{
			LOA_MakeFileRef(pst_Data->bk_MaterialID,(ULONG *) &pst_Data->p_Material , GEO_ul_Load_ObjectCallback, LOA_C_MustExists);
			LOA_Resolve();
		}
	}
#endif

	return (ULONG)(pc_Cur - (unsigned char*)_pc_Buffer);
//	return pc_Cur - _pc_Buffer ;

}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SPG_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SPG_tdst_Modifier *pst_Data;
	ULONG							ulSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



	pst_Data = (SPG_tdst_Modifier *) _pst_Mod->p_Data;
   /* Save Size */
	ulSize = 4/* Version */ + 4/* size */ + 4/* Ratio */ + 4/* LODCF*/ + 4 /* Material */ + 4 /* thresh */ + 4 /* extraction */ + 4 /* Depth */ + 4 /* SMM */+ 4 /* Noise*/ + sizeof(SPG_SpriteMapper) * 4 + sizeof(SPG_SpriteElementDescriptor) * 4 + 4 /* fLODCorrectionFactor */ + 4 /*Flags*/ + 12/*fog*/;
	SAV_Buffer(&ulSize, 4);
   /* Save version */
	ulSize = 8;
	SAV_Buffer(&ulSize, 4);
	/* Save fSize */
	SAV_Buffer(&pst_Data->f_GlobalSize, 4);
	/* Save ratio */
	SAV_Buffer(&pst_Data->f_GlobalRatio, 4);
	/* Save Material */
	SAV_Buffer(&pst_Data->bk_MaterialID, 4);
	/* Save Extraction */
	SAV_Buffer(&pst_Data->f_Extraction, 4);
	/* Save Threshold */
	SAV_Buffer(&pst_Data->f_ThresholdMin, 4);
	/* Save Depth */
	SAV_Buffer(&pst_Data->ulMaxDepth, 4);
	/* Save SMM */
	SAV_Buffer(&pst_Data->ulSubMaterialMask, 4);
	/* Save Noise */
	SAV_Buffer(&pst_Data->f_Noise, 4);
	/* Save SpriteMapper */
	SAV_Buffer(&pst_Data->stSpriteMapper, sizeof(SPG_SpriteMapper) * 4);
	/* Save SED */
	SAV_Buffer(&pst_Data->stSED, sizeof(SPG_SpriteElementDescriptor) * 4);
	/* Save LODCF */
	SAV_Buffer(&pst_Data->fLODCorrectionFactor, 4);
	/* Save flags */
	SAV_Buffer(&pst_Data->ulFlags, 4);
	/* Save fog */
	SAV_Buffer(&pst_Data->fSpecialFogNear, 4);
	SAV_Buffer(&pst_Data->fSpecialFogFar, 4);
	SAV_Buffer(&pst_Data->ulSpecialFogColor, 4);
}

#endif


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
