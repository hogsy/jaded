/*$T iopBAStypes.h GC! 1.097 03/20/02 09:03:54 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP
#ifndef __iopBAStypes_h__
#define __iopBAStypes_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define FALSE	(0)
#define TRUE	(~FALSE)

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

/*$2- basics ---------------------------------------------------------------------------------------------------------*/

typedef char				long8;
typedef short				long16;
typedef int					long32;
typedef long long			long64;

typedef char				int8;
typedef short				int16;
typedef int					int32;
typedef long long			int64;

typedef char				CHAR;
typedef short				SHORT;
typedef int					INT, LONG;

typedef unsigned char		u_long8;
typedef unsigned short		u_long16;
typedef unsigned int		u_long32;
typedef unsigned long long	u_long64;

typedef unsigned char		u_int8;
typedef unsigned short		u_int16;
typedef unsigned int		u_int32;
typedef unsigned long long	u_int64;

typedef unsigned char		UCHAR;
typedef unsigned short		USHORT;
typedef unsigned int		UINT, ULONG;

typedef int					BOOL;
typedef unsigned int        BIG_KEY;

typedef struct  MATH_tdst_Vector_
{
    float   x;
    float   y;
    float   z;
} MATH_tdst_Vector;

typedef struct MATH_tdst_Matrix_
{
    float               Ix, Iy, Iz, Sx; /* I,J,K sont les vecteurs qui definissent les axes locaux
                                         * dans le repere global. */
    float               Jx, Jy, Jz, Sy; /* S le vecteur scale */
    float               Kx, Ky, Kz, Sz;
    MATH_tdst_Vector    T;              /* T pour Translation */
    float               w;
    LONG                lType;
}MATH_tdst_Matrix;

//$2-
#define _inline_ inline
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __iopBAStypes_h__ */
#endif /* PSX2_IOP */
