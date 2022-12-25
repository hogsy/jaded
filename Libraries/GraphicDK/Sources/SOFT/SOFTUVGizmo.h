/*$T SOFTUVGizmo.h GC! 1.081 08/21/01 09:59:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef ACTIVE_EDITORS
#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __SOFTUVGIZMOCOLOR_H__
#define __SOFTUVGIZMOCOLOR_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    constant
 ***********************************************************************************************************************
 */

/* type of UVGizmo */
#define SOFT_Cl_UVGizmo_Planar      1
#define SOFT_Cl_UVGizmo_Cylindre    2
#define SOFT_Cl_UVGizmo_Box         3

/* type of alignement */
#define SOFT_Cl_UVGizmo_X           0
#define SOFT_Cl_UVGizmo_Y           1
#define SOFT_Cl_UVGizmo_Z           2


/*$4
 ***********************************************************************************************************************
    structures
 ***********************************************************************************************************************
 */

typedef struct	SOFT_tdst_UVGizmo_
{
	struct OBJ_tdst_GameObject_	*pst_GO;
    MATH_tdst_Matrix            st_Matrix;

    char                        c_On;
	char						c_Type;
	char						c_XYZ;

	char                        c_Dummy;

	float						x, y, z;
	float						u, v;

	ULONG						ul_EditorDlg;
} SOFT_tdst_UVGizmo;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void	SOFT_UVGizmo_Init( SOFT_tdst_UVGizmo *);
void    SOFT_UVGizmo_Set( SOFT_tdst_UVGizmo *, OBJ_tdst_GameObject *, int );
void    SOFT_UVGizmo_Reset( SOFT_tdst_UVGizmo * );
void	SOFT_UVGizmo_Draw( SOFT_tdst_UVGizmo *);

void    SOFT_UVGizmo_Apply(SOFT_tdst_UVGizmo *);

void    SOFT_UVGizmo_Rotate( SOFT_tdst_UVGizmo *, MATH_tdst_Matrix *, MATH_tdst_Vector *, MATH_tdst_Vector * );
void    SOFT_UVGizmo_Move( SOFT_tdst_UVGizmo *, MATH_tdst_Matrix *, MATH_tdst_Vector * );
void    SOFT_UVGizmo_Scale(SOFT_tdst_UVGizmo *, MATH_tdst_Vector * );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SOFTUVGIZMOCOLOR_H__ */

#endif /* ACTIVE_EDITORS */
