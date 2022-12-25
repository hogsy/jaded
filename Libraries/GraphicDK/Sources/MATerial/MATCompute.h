// MATCompute.h

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __MATCOMPUTE_H__
#define __MATCOMPUTE_H__




#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

// ***************************************************************************************************
//    Functions
// ***************************************************************************************************

void MAT_ComputeUV (	GEO_tdst_Object						*pst_Obj,
						GEO_tdst_ElementIndexedTriangles	*pst_Element,
						MAT_tdst_MTLevel					*pst_MLTTXLVL
						);

void MAT_ComputeDeductedAlpha(GEO_tdst_Object *pst_Obj, MAT_tdst_MTLevel *pst_MLTTXLVL);
typedef struct MAT_FuncTableNode_
{
	void (* Func)(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles *pst_Element,ULONG CurrentAddFlag,ULONG StoreUV);
} MAT_FuncTableNode;

#if defined(PSX2_TARGET)
void Gsp_SetUVMatrix_Identity();
void Gsp_SetUVMatrix(struct MAT_tdst_Decompressed_UVMatrix_ *TDST_ummAT);
_inline_ BOOL MAT_VUISIdentity_i(MAT_tdst_MTLevel *p_tdstLevel)
{
	if (p_tdstLevel->ScaleSPeedPosU == 0) return TRUE;
	return FALSE;
}

_inline_ void MAT_ComputeUV_NoStore
(
	GEO_tdst_Object						*pst_Obj,
	GEO_tdst_ElementIndexedTriangles	*pst_Element,
	MAT_tdst_MTLevel					*pst_MLTTXLVL,
	GDI_tdst_DisplayData				*pst_CurDD
)
{
	extern MAT_FuncTableNode FuncTable[];
	pst_CurDD->ul_DisplayInfo &= ~GDI_Cul_DI_FaceMap;
	FuncTable[MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags)].Func(pst_CurDD , pst_Obj,pst_Element,pst_MLTTXLVL->s_AditionalFlags,0);
	{
		if(!MAT_VUISIdentity_i(pst_MLTTXLVL))
		{
			MAT_tdst_Decompressed_UVMatrix	TDST_ummAT  __attribute__((aligned (8)));
			MAT_VUDecompress(pst_MLTTXLVL, &TDST_ummAT);
			Gsp_SetUVMatrix(&TDST_ummAT);
		} else Gsp_SetUVMatrix_Identity();
	}
}
#endif	// defined(PSX2_TARGET)

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


#endif /* __MATCOMPUTE_H__ */