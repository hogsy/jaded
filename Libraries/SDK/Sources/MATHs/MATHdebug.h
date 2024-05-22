/*$T MATHdebug.h GC!1.32 05/07/99 10:52:11 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef MathsDebug_h
#define MathsDebug_h


#ifdef _DEBUG

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

void MATH_CheckFloat(float *, char *);
void MATH_CheckVector(MATH_tdst_Vector *, const char *);
void MATH_CheckMatrix(MATH_tdst_Matrix *, char *);
void MATH_CheckQuaternion(MATH_tdst_Quaternion *, char *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


#else // _DEBUG

#define MATH_CheckFloat(x,y) 
#define MATH_CheckVector(x,y) 
#define MATH_CheckMatrix(x,y) 
#define MATH_CheckQuaternion(x,y)

#endif // _DEBUG

#endif // MathsDebug_h
 