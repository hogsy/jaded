/*$T ps2MATHopt.h GC! 1.081 07/11/00 12:23:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#if !defined(__ps2MATHOPT_H__) && defined(PSX2_TARGET)
#define __ps2MATHOPT_H__

#ifdef __cplusplus
extern "C" {
#endif
/*$4
 ***********************************************************************************************************************
    public macro
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    public variables
 ***********************************************************************************************************************
 */

#ifdef __ps2MATHOPT_C__
#define __EXTERN
#else
#define __EXTERN	extern
#endif

/*$4
 ***********************************************************************************************************************
    public variables
 ***********************************************************************************************************************
 */

__EXTERN u_long128	ps2MATH_gul128_Data1;
__EXTERN u_long128	ps2MATH_gul128_Data2;

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

void				ps2MATH_InitModule(void);

/*$4
 ***********************************************************************************************************************
    interface for asm functions
 ***********************************************************************************************************************
 */

extern float		asm_Blend(float _f1, float _f2, float _c);
extern LONG			asm_f2i(float a);
extern void			asm_AbsAlignedVector(MATH_tdst_Vector *, MATH_tdst_Vector *);
extern float		asm_fabs(float _f);
extern float		asm_Min(float a, float b);
extern float		asm_Max(float a, float b);
extern float		asm_fsqrt(float _f);
extern void			asm_vsqrt(float *);
extern float		asm_frsqrt(float _f1, float _f2);
extern float		asm_vrsqrt(float *, float _ff);
extern void			asm_MATH_GetRotationMatrix(MATH_tdst_Matrix *, MATH_tdst_Matrix *);
extern void			asm_MATH_GetRotationMatrix2(MATH_tdst_Matrix *, MATH_tdst_Matrix *);

#ifdef __cplusplus
}
#endif
#undef __EXTERN
#endif /* __ps2MATHopt_h__ */
