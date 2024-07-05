/*$T MATH_MEM.h */

#if !defined( MATH_MEM_H )
#  define MATH_MEM_H 

#pragma once

#include "MATHs/MATHstruct.h"

#ifdef __cplusplus
extern "C"
{
#	endif

	/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    MATH_MEM.h: Memory management for mathematics. Needed for alignment.
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

	/**
 * Call this function to allocate a single matrix.
 *
 * @return a pointer to the allocated matrix or NULL if there is no more memory.
 */
	MATH_tdst_Matrix *
	MATH_Matrix_Alloc( void );

	/**
 * Call this function to allocate an array of matrices.
 *
 * @contract count cannot be 0.
 * 
 * @param count the number of matrices to allocate.
 * @return a pointer to the allocated array.
 */
	MATH_tdst_Matrix *
	MATH_Matrix_AllocArray( unsigned int count );

	/**
 * Attemps to reallocate an array of matrices.
 *
 * @param array the pointer to the array to resize. Can be NULL.
 * @param count the new number of elements of the array.
 * @return a pointer to the resized array.
 */

	MATH_tdst_Matrix *
	MATH_Matrix_ReallocArray( MATH_tdst_Matrix *array, unsigned int count );

	/**
 * Frees one matrix allocated via MATH_Matrix_Alloc.
 *
 * @contract the passed pointer can be NULL.
 *
 * @param matrix a pointer as returned by MATH_Matrix_Alloc.
 */
	void
	MATH_Matrix_Free( MATH_tdst_Matrix *matrix );

	/**
 * Frees one matrix array allocated via MATH_Matrix_AllocArray.
 *
 * @contract the passed pointer can be NULL.
 *
 * @param matrixArray the pointer to the array as returned by
 *                    MATH_Matrix_AllocArray.
 */
	void
	MATH_Matrix_FreeArray( MATH_tdst_Matrix *matrixArray );

#ifdef __cplusplus
}
#endif


#endif
