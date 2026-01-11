#include "Precomp.h"
#include "MATHs/MATH_MEM.h"
#include "BASe/MEMory/MEM.h"

/**
 * XBox and PC want the matrices to be aligned at 16 bytes boundaries
 * in order to employ SSE/MMX instruction set.
 *
 * PlayStation2 and GameCube doesn't need this, so they get an unaligned
 * version.
 */

#if defined( _XBOX ) || defined( _PC_RETAIL ) || defined( _XENON )
#  define ALIGNED_MATRIX
#endif


#if defined( ALIGNED_MATRIX )

MATH_tdst_Matrix*
MATH_Matrix_Alloc(void)
{
    return (MATH_tdst_Matrix*)MEM_p_AllocAlign( sizeof( MATH_tdst_Matrix ), 16 );
}

MATH_tdst_Matrix*
MATH_Matrix_AllocArray( unsigned int count )
{
    return (MATH_tdst_Matrix*)MEM_p_AllocAlign( count*sizeof( MATH_tdst_Matrix ), 16 );
}

MATH_tdst_Matrix*
MATH_Matrix_ReallocArray( MATH_tdst_Matrix* array, unsigned int count )
{
    return (MATH_tdst_Matrix*)MEM_p_ReallocAlign( array, count*sizeof( MATH_tdst_Matrix ), 16 );
}

void
MATH_Matrix_Free( MATH_tdst_Matrix* matrix )
{
    MEM_FreeAlign( matrix );
}

void
MATH_Matrix_FreeArray( MATH_tdst_Matrix* matrixArray )
{
    MEM_FreeAlign( matrixArray );
}


#else

MATH_tdst_Matrix*
MATH_Matrix_Alloc(void)
{
    return (MATH_tdst_Matrix*)MEM_p_Alloc( sizeof( MATH_tdst_Matrix ) );
}

MATH_tdst_Matrix*
MATH_Matrix_AllocArray( unsigned int count )
{
    return (MATH_tdst_Matrix*)MEM_p_Alloc( count*sizeof( MATH_tdst_Matrix ) );
}

MATH_tdst_Matrix*
MATH_Matrix_ReallocArray( MATH_tdst_Matrix* array, unsigned int count )
{
    return (MATH_tdst_Matrix*)MEM_p_Realloc( array, count*sizeof( MATH_tdst_Matrix ) );
}

void
MATH_Matrix_Free( MATH_tdst_Matrix* matrix )
{
    MEM_Free( matrix );
}

void
MATH_Matrix_FreeArray( MATH_tdst_Matrix* matrixArray )
{
    MEM_Free( matrixArray );
}
#endif
