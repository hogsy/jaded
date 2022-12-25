/*$T SOFTMatrixStack.c GC!1.52 10/11/99 14:57:50 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "SOFT/SOFTMatrixStack.h"


#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif
/*
 ===================================================================================================
 ===================================================================================================
 */
#ifdef JADEFUSION
LONG SOFT_l_MatrixStack_Create(SOFT_tdst_MatrixStack *_pst_MS)
{
    if(_pst_MS == NULL) 
    	return 0;
    _pst_MS->uw_NumberMaxOfMatrix = (unsigned short) MATRIX_STACK_DEPTH;
    SOFT_MatrixStack_Reset(_pst_MS, NULL);
    return MATRIX_STACK_DEPTH;
}
#else
LONG SOFT_l_MatrixStack_Create(SOFT_tdst_MatrixStack *_pst_MS, LONG _l_NumberOfMatrix)
{
    if((_l_NumberOfMatrix <= 0) || (_pst_MS == NULL)) return 0;

    _pst_MS->dst_Matrix = (MATH_tdst_Matrix *)
        L_malloc(_l_NumberOfMatrix * sizeof(MATH_tdst_Matrix));
    if(_pst_MS->dst_Matrix == NULL) _l_NumberOfMatrix = 0;

    _pst_MS->uw_NumberMaxOfMatrix = (unsigned short) _l_NumberOfMatrix;
    SOFT_MatrixStack_Reset(_pst_MS, NULL);
    return _l_NumberOfMatrix;
}
#endif
/*
 ===================================================================================================
 ===================================================================================================
 */
void SOFT_MatrixStack_Destroy(SOFT_tdst_MatrixStack *_pst_MS)
{
#ifndef JADEFUSION
	if(_pst_MS->dst_Matrix)
        L_free(_pst_MS->dst_Matrix);
#endif 
	_pst_MS->uw_NumberMaxOfMatrix = 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void SOFT_MatrixStack_Reset(SOFT_tdst_MatrixStack *_pst_MS, MATH_tdst_Matrix *_pst_Matrix)
{
    if(_pst_MS == NULL) return;
    if(_pst_MS->uw_NumberMaxOfMatrix == 0) return;

    if(_pst_Matrix != NULL)
        MATH_CopyMatrix(_pst_MS->dst_Matrix, _pst_Matrix);
    else
        MATH_SetIdentityMatrix(_pst_MS->dst_Matrix);

    _pst_MS->uw_IndexOfCurrentMatrix = 0;
    _pst_MS->pst_CurrentMatrix = _pst_MS->dst_Matrix;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
LONG SOFT_l_MatrixStack_Push(SOFT_tdst_MatrixStack *_pst_MS, MATH_tdst_Matrix *_pst_Matrix)
{
    if(_pst_MS->uw_IndexOfCurrentMatrix + 2 == _pst_MS->uw_NumberMaxOfMatrix) return 0;

    MATH_MulMatrixMatrix(_pst_MS->pst_CurrentMatrix + 1, _pst_Matrix, _pst_MS->pst_CurrentMatrix);
    _pst_MS->pst_CurrentMatrix++;
    _pst_MS->pst_CurrentMatrix->w = 1.0f;// MATRIX W!
    return ++_pst_MS->uw_IndexOfCurrentMatrix;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
LONG SOFT_l_MatrixStack_Pop(SOFT_tdst_MatrixStack *_pst_MS)
{
    if(!_pst_MS->uw_IndexOfCurrentMatrix) return 0;
    _pst_MS->pst_CurrentMatrix--;
    return _pst_MS->uw_IndexOfCurrentMatrix--;
}


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
