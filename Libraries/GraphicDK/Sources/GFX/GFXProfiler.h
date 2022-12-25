/*$T GFXsmoke.h GC! 1.081 09/21/00 11:59:44 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFXPROFILER_H__
#define __GFXPROFILER_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */
typedef struct	GFX_tdst_Profiler_Point_
{
	MATH_tdst_Vector ActualPos;
	MATH_tdst_Vector ActualSpeed;
	MATH_tdst_Vector ActualNormale;
	ULONG			 ActualColor;
	float			 fPointConstraintZMin;
} GFX_tdst_Profiler_Point;

typedef struct	GFX_tdst_Profiler_Point_Generator_
{
	MATH_tdst_Vector	Pos;
	MATH_tdst_Vector	initialSpeed;
	MATH_tdst_Vector	RandomPos;
	MATH_tdst_Vector	RandominitialSpeed;
	MATH_tdst_Vector	Force;
	float				PointConstraintZMin;
	ULONG				ulColorStart;
	ULONG				ulColorEnd;
}	GFX_tdst_Profiler_Point_Generator;

typedef struct	GFX_tdst_Profiler_Link_
{
	unsigned short A,B;
	unsigned short MaterialID;
	float		   fvA,fvB;
} GFX_tdst_Profiler_Link;

#define GFX_Profile_TextWater	0x0001

typedef struct	GFX_tdst_Profiler_
{
	float								fGravity;
	MATH_tdst_Vector					FrictionXYZ;
	ULONG								Valid;
	float								fSegmentSizeMin;
	USHORT								UTiler;
	USHORT								Flags;

	/* Generation Profile */
	ULONG								ulNumberOfPointsPerProfile;
	GFX_tdst_Profiler_Point_Generator	*p_PointGenerator;
	ULONG								ulNumberOfLinksPerProfile;
	GFX_tdst_Profiler_Link				*p_AllLinks;

	/* Profile state */
	ULONG								ulNumberOfProfiles;
	ULONG								ulCurrentProfile;			/* The profile where creation is		*/
	ULONG								*p_AllProfileStates;		/* Each Value corespond to each profile */
	GFX_tdst_Profiler_Point				*p_AllPoints;				/* Number = ulNumberOfProfiles * ulNumberOfPointsPerProfile */

	/* Mesh state */
	ULONG								ulNumberOfTriangles;
	ULONG								ulNumberOfPoints;
	ULONG								ulNumberOfUV;

	/* Actual 3 Vector Generation */
	MATH_tdst_Vector					GX,	GY,	GZ,	GT;
	ULONG								ulActualPointToSet;
	ULONG								ulActualLinkToSet;

	ULONG								ulAlphaSub;

	ULONG								bStateGoBack;

#ifdef USE_DOUBLE_RENDERING	
	MATH_tdst_Vector					GX1,	GY1,	GZ1,	GT1;
	MATH_tdst_Vector					GX2,	GY2,	GZ2,	GT2;
#endif
} GFX_tdst_Profiler;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void				*GFX_Profiler_Create(void);
void				GFX_Profiler_Destroy( void *);
int					GFX_i_Profiler_Render(void *);

void				GFX_Profiler_Seti( void *, int, int );

void				GFX_Profiler_Setf( void *, int, float );

void				GFX_Profiler_Setv( void *, int, MATH_tdst_Vector * );
MATH_tdst_Vector	*GFX_pst_Profiler_Getv( void *, int );


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFXPROFILER_H__ */
