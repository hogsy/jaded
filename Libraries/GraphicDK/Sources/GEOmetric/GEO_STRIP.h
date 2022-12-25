/*$T GEO_STRIP.h GC! 1.081 04/25/00 14:14:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __GEO_STRIP_H__
#define __GEO_STRIP_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#ifdef __GEO_STRIP_C__
#define EXTERN
#else
#define EXTERN	extern
#endif

/*
 * define 
 */
#define GEO_C_Strip_DataValid           0x00000001
#define GEO_C_Strip_DisplayStrip        0x00000002
#define GEO_C_Strip_StatData            0x00000004
#define GEO_C_Strip_UseSymetry          0x00000008
#define GEO_C_Strip_UseDirect           0x00000010


/* 
 * public variables 
 */

/* 
 * public prototypes 
 */
EXTERN BOOL	GEO_STRIP_Compute(struct OBJ_tdst_GameObject_ *pst_GAO,struct GEO_tdst_Object_ *, ULONG *p_AdditionalVertexColor,BOOL, void (*SetpercentOfJob)(float));
EXTERN void GEO_STRIP_Delete(struct GEO_tdst_Object_ *);
EXTERN void GEO_STRIP_SetFlag(LONG __command, struct GEO_tdst_Object_ *__pst_Object);
EXTERN void GEO_STRIP_UnSetFlag(LONG __command, struct GEO_tdst_Object_ *__pst_Object);
EXTERN void GEO_STRIP_ComputeStat(struct GEO_tdst_Object_ *__pst_Object, char *__asz);


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GEO_STRIP_H__ */
