/*$T GRI_compute.h GC! 1.100 03/27/01 11:49:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GRID_PRECOMP_H__
#define __GRID_PRECOMP_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* __cplusplus */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

#define COMPRESS_MASK	0x3F
#define OCCUP			0xC0
#define MASK_STATIC		0x0F
#define MASK_DYN		0xF0
#define MASK_STATDYN	0xFF

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

extern BOOL GRI_3Dto2D_Float(struct GRID_tdst_World_ *, MATH_tdst_Vector *, float *, float *);
extern BOOL GRI_3Dto2D(struct GRID_tdst_World_ *, MATH_tdst_Vector *, int *, int *);
extern BOOL GRI_ConvertTo2DReal(struct GRID_tdst_World_ *, int *, int *);

extern void GRI_SetCapa(struct GRID_tdst_World_ *, MATH_tdst_Vector *, char);
extern int  GRI_AddCapa(struct GRID_tdst_World_ *, MATH_tdst_Vector *, char, int *);
extern int	GRI_i_GetCapa(struct GRID_tdst_World_ *, MATH_tdst_Vector *);

extern BOOL GRID_3Dto2D(struct GRID_tdst_World_ *, MATH_tdst_Vector *, int *, int *);
extern int	GRID_ShortestPath(struct OBJ_tdst_GameObject_ *, struct GRID_tdst_World_ *, MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, int, float, int, int);
extern int	GRI_SmoothWay(struct GRID_tdst_World_ *, MATH_tdst_Vector *, int, float);
extern int	GRI_Surround(struct GRID_tdst_World_ *, MATH_tdst_Vector *, MATH_tdst_Vector *);

extern int	GRI_Compress(struct GRID_tdst_World_ *, struct GRID_tdst_Elem_ *, int, int, char *, int);
extern void GRI_UnCompress(struct GRID_tdst_World_ *, struct GRID_tdst_Elem_ *, int, int);

extern void GRI_CompressReal(struct GRID_tdst_World_ *, int, int);
extern void GRI_UnCompressReal(struct GRID_tdst_World_ *);
extern void GRID_SetRealCenter(struct GRID_tdst_World_ *, int, int);

extern void GRI_Destroy(struct GRID_tdst_World_ *);
extern void GRI_Reinit(struct GRID_tdst_World_ *);

extern int	GRI_TagBox(struct GRID_tdst_World_ *, MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *, int);

extern int	GRI_i_ComputeDest(struct GRID_tdst_World_ *, MATH_tdst_Vector *);

#ifdef ACTIVE_EDITORS
extern void GRI_UncompressEdit(struct GRID_tdst_World_ *);
extern int	GRI_UpdateCompress(struct GRID_tdst_World_ *);
extern void GRI_SetWidth(struct GRID_tdst_World_ *, int);
extern void GRI_SetHeight(struct GRID_tdst_World_ *, int);
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* __cplusplus */

#endif /* __GRID_PRECOMP_H__ */
