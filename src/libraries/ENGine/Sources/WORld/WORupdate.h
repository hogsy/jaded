/*$T WORupdate.h GC! 1.081 04/25/00 12:31:56 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Aim: Main functions of the world module */
#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __WORUPDATE_H__
#define __WORUPDATE_H__

#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    constants
 ***********************************************************************************************************************
 */

#define WOR_Update_RLI_Add						1
#define WOR_Update_RLI_AddSome					2
#define WOR_Update_RLI_Del						3
#define WOR_Update_RLI_DelSome					4    
#define WOR_Update_RLI_AddSomeCenter			5
#define WOR_Update_RLI_ScaleVertices			6
#define WOR_Update_RLI_RotateVertices			7
#define WOR_Update_RLI_ScaleVerticesComplex		8
#define WOR_Update_RLI_OpMask       0xFF

#define WOR_Update_RLI_Barycentre   0x100

/*$4
 ***********************************************************************************************************************
    structures
 ***********************************************************************************************************************
 */

typedef struct	WOR_tdst_Update_RLI_
{
	void	*p_Geo;
	LONG	l_Op;
	LONG	l_Ind0, l_Ind1, l_Ind2;
	float	f_Blend, f0, f1, f2;
    LONG    l_OldNbPoints;
    LONG    l_NbAdded;
    ULONG   *aul_AddedRLI;
    struct  GEO_tdst_ObjectPonderation_ *p_AddedSKN_Objectponderation;
    OBJ_tdst_GameObject *pst_GoSrc;
    OBJ_tdst_GameObject *pst_GoDst;
} WOR_tdst_Update_RLI;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

void	WOR_World_Update_GroPointerChange(WOR_tdst_World *, void *, void *);
void	WOR_World_Update_RLI(WOR_tdst_World *, WOR_tdst_Update_RLI *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __WORUPDATE_H__ */ 
 