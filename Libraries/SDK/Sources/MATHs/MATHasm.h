/*$T MATHasm.h GC!1.32 05/07/99 10:48:51 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Fichier inutilisé. (servirait éventuelement pour une version PC...)
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "MATHvars.h"
#include "MATHstruct.h"
#include "BASe/BASsys.h"

#ifndef PSX2_TARGET
/*
 ===================================================================================================
    Square root
 ===================================================================================================
 */
static _inline_ float MATH_f_OptSqrtAsm(float a)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   result;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    _asm
    {
        mov ebx, a
        mov eax, ebx

        and ebx, 0x7F800000
        add ebx, 0x3F800000
        shr ebx, 1
        and ebx, 0x7F800000

        and eax, 0x00FFE000
        shr eax, 11

        add ebx, dword ptr[MATH_gl_TableRacine + eax]
        mov dword ptr[result], ebx
    }

    return(result);
}

/* Zero compare */
#pragma warning(disable : 4035)

/*
 ===================================================================================================
 ===================================================================================================
 */
static _inline_ unsigned char __fastcall MATH_b_EqualZeroAsm(float A)
{
    __asm
    {
        mov ecx, A
        mov al, 0           /* Default return value is 0 (not equal to 0) */
        and ecx, 0x7FFFFFFF /* Very important: clear sign flag because negative zero also compares
                             * to zero... */
        cmp ecx, 1          /* Cf is set only if ecx=0 (we compare to 1, so cf is set if ecx is
                             * strictly smaller then 1) */
        adc al, 0           /* Add carry to al */
        /* No need of return, default returned vaue is in al */
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
static _inline_ unsigned char __fastcall MATH_b_DifferentZeroAsm(float A)
{
    __asm
    {
        mov ecx, A
        mov al, 1           /* Default return value is 0 (not equal to 0) */
        and ecx, 0x7FFFFFFF /* Very important: clear sign flag because negative zero also compares
                             * to zero... */
        cmp ecx, 1          /* Cf is set only if ecx=0 (we compare to 1, so cf is set if ecx is
                             * strictly smaller then 1) */
        sbb al, 0           /* Sub carry to al */
        /* No need of return, default returned vaue is in al */
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
static _inline_ unsigned char __fastcall MATH_b_GreaterZeroAsm(float A)
{
    __asm
    {
        mov ecx, A
        mov al, 0           /* Default is less or equal zero */
        sub ecx, 1
        cmp ecx, 0x7fffffff /* Key of the algorithm: cf is set if (A-1)<0x7fffffff */
        adc al, 0           /* Add cf to al */
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
static _inline_ unsigned char __fastcall MATH_b_LessEqualZeroAsm(float A)
{
    __asm
    {
        mov ecx, A
        mov al, 1           /* Default is less or equal zero */
        sub ecx, 1
        cmp ecx, 0x7fffffff /* Key of the algorithm: cf is set if (A-1)<0x7fffffff */
        sbb al, 0           /* Add cf to al */
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
static _inline_ unsigned char __fastcall MATH_b_LessZeroAsm(float A)
{
    __asm
    {
        mov al, 1
        cmp A, 0x80000001
        sbb al, 0
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
static _inline_ unsigned char __fastcall MATH_b_GreaterEqualZeroAsm(float A)
{
    __asm
    {
        mov al, 0
        cmp A, 0x80000001
        adc al, 0
    }
}

/*
 ===================================================================================================
    Dot product
 ===================================================================================================
 */
static _inline_ float MATH_f_DotProductAsm(MATH_tdst_Vector *A, MATH_tdst_Vector *B)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   Result;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    __asm
    {
        mov eax, A
        mov ebx, B
        fld dword ptr[eax]
        fmul dword ptr[ebx]
        fld dword ptr[eax + 4]
        fmul dword ptr[ebx + 4]
        fld dword ptr[eax + 8]
        fmul dword ptr[ebx + 8]
        fxch st(1)
        faddp st(2), st
        faddp st(1), st
        fstp[Result]
    }

    return Result;
}

/*
 ===================================================================================================
    Cross product
 ===================================================================================================
 */
static _inline_ void MATH_CrossProductAsm
(
    MATH_tdst_Vector  *Dest,
    MATH_tdst_Vector  *A,
    MATH_tdst_Vector  *B
)
{
    __asm
    {
        mov eax, A
        mov ebx, B
        mov edx, Dest

        fld dword ptr[eax + 4]
        fmul dword ptr[ebx + 8]
        fld dword ptr[eax + 8]
        fmul dword ptr[ebx]
        fld dword ptr[eax]
        fmul dword ptr[ebx + 4]
        fld dword ptr[eax + 8]
        fmul dword ptr[ebx + 4]
        fld dword ptr[eax]
        fmul dword ptr[ebx + 8]
        fld dword ptr[eax + 4]
        fmul dword ptr[ebx]
        fxch st(2)
        fsubp st(5), st
        fsubp st(3), st
        fsubp st(1), st
        fxch st(2)
        fstp dword ptr[edx]
        fstp dword ptr[edx + 4]
        fstp dword ptr[edx + 8]
    }
}

/*
 ===================================================================================================
    Vector*scalar multiplication
 ===================================================================================================
 */
static _inline_ float MATH_MulVectorAsm(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A, float f)
{
    __asm
    {
        mov eax, A
        mov ebx, Dest
        fld dword ptr[eax]
        fmul dword ptr[f]
        fld dword ptr[eax + 4]
        fmul dword ptr[f]
        fld dword ptr[eax + 8]
        fmul dword ptr[f]
        fxch st(2)
        fstp dword ptr[ebx]
        fstp dword ptr[ebx + 4]
        fstp dword ptr[ebx + 8]
    }
}

#else 
/* pas du tout optimise */
#include <math.h>

#define MATH_f_OptSqrtAsm(a) sqrtf(a)
#endif /* PSX2_TARGET */