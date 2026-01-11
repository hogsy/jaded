/*$T COLreport.h GC! 1.081 02/06/01 10:13:08 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __COL_REPORT__
#define __COL_REPORT__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* _cplusplcus */

#include "MATHs/MATH.h"
#include "COLconst.h"

extern BOOL		COL_gb_SkipODEReports;


BOOL						COL_ComputeReport
							(
								MATH_tdst_Vector *,
								MATH_tdst_Vector *,
								MATH_tdst_Vector *,
								MATH_tdst_Vector *,
								MATH_tdst_Vector *,
								ULONG *,
								ULONG,
								ULONG,
								struct COL_tdst_GlobalVars_ *
							);
void						COL_ReportCollision
							(
								MATH_tdst_Vector *,
								MATH_tdst_Vector *,
								MATH_tdst_Vector *,
								ULONG,
								ULONG,
								ULONG,
								struct COL_tdst_GlobalVars_ *
							);
void						COL_ComputeReports(struct COL_tdst_GlobalVars_ *, ULONG);

BOOL						COL_Report_CollideType(struct OBJ_tdst_GameObject_ *, struct WOR_tdst_World_ *, ULONG);
BOOL						COL_Report_HasGMat(struct OBJ_tdst_GameObject_ *, struct WOR_tdst_World_ *, ULONG);;

OBJ_tdst_GameObject			*COL_Report_GetBestWallGOInDirection
							(
								struct OBJ_tdst_GameObject_ *,
								struct WOR_tdst_World_ *,
								MATH_tdst_Vector *,
								float,
								ULONG *
							);

OBJ_tdst_GameObject			*COL_Report_GetBestWallGOInDirectionSpecial
							(
								struct OBJ_tdst_GameObject_ *,
								struct WOR_tdst_World_ *,
								MATH_tdst_Vector *,
								float,
								ULONG *
							);

void						COL_Report_GetNormal(struct OBJ_tdst_GameObject_ *, struct WOR_tdst_World_ *, MATH_tdst_Vector *, ULONG);
int							COL_Report_GetFlags(struct OBJ_tdst_GameObject_ *, struct WOR_tdst_World_ *, ULONG);
MATH_tdst_Vector			*COL_Report_GetCollidedPoint
							(
								struct OBJ_tdst_GameObject_ *,
								struct WOR_tdst_World_ *,
								ULONG
							);
MATH_tdst_Vector			*COL_Report_GMat_GetCollidedPoint
							(
								struct OBJ_tdst_GameObject_ *,
								struct WOR_tdst_World_ *,
								ULONG,
								ULONG *
							);
struct OBJ_tdst_GameObject_ *COL_Report_GetCOBGameObject
							(
								struct OBJ_tdst_GameObject_ *,
								struct WOR_tdst_World_ *,
								ULONG
							);
ULONG						COL_Report_GetZDMGameObjectList
							(
								struct OBJ_tdst_GameObject_ *,
								struct WOR_tdst_World_ *,
								ULONG *,
								MATH_tdst_Vector *,
								ULONG
							);
ULONG						COL_Report_GetAllReportsNormal
							(
								struct OBJ_tdst_GameObject_ *,
								struct WOR_tdst_World_ *,
								ULONG *,
								ULONG
							);

ULONG						COL_Report_GetNumberOfReports
							(
								struct OBJ_tdst_GameObject_ *,
								struct WOR_tdst_World_ *,
								ULONG
							);

#ifdef JADEFUSION
void						COL_Report_ComputeResultingNormal
#else
ULONG						COL_Report_ComputeResultingNormal
#endif
							(
								struct OBJ_tdst_GameObject_ *,
								struct WOR_tdst_World_ *,
								ULONG,
								MATH_tdst_Vector *
							);

ULONG						COL_Report_GetNumberOfReportsAndRanks
							(
								struct OBJ_tdst_GameObject_ *, 
								struct WOR_tdst_World_ *, 
								ULONG, 
								ULONG, 
								ULONG *
							);

BOOL						COL_Report_b_ObjectsAreColliding
							(
								struct OBJ_tdst_GameObject_ *, 
								struct OBJ_tdst_GameObject_ *, 								
								struct WOR_tdst_World_ *
							);

ULONG						COL_Report_ul_BestReportGet
							(
								struct OBJ_tdst_GameObject_ *,
								struct WOR_tdst_World_ *,
								ULONG,
								ULONG				
							);

ULONG						COL_Report_ul_GMatReportGet
							(
								struct OBJ_tdst_GameObject_ *,
								struct WOR_tdst_World_ *,
								ULONG				
							);


ULONG						COL_Report_ul_SlipperyEdgeNormalsGet
							(
								struct OBJ_tdst_GameObject_ *, 
								struct WOR_tdst_World_ *, 
								MATH_tdst_Vector *
							);




ULONG						COL_GameMaterial_GetFlags(struct OBJ_tdst_GameObject_ *, struct WOR_tdst_World_ *, ULONG);
float						COL_GameMaterial_GetSlide(struct OBJ_tdst_GameObject_ *, struct WOR_tdst_World_ *, ULONG);
float						COL_GameMaterial_GetRebound(struct OBJ_tdst_GameObject_ *, struct WOR_tdst_World_ *, ULONG);
UCHAR						COL_GameMaterial_GetSound(struct OBJ_tdst_GameObject_ *, struct WOR_tdst_World_ *, ULONG);
UCHAR						COL_GetDesign(struct OBJ_tdst_GameObject_ *, struct WOR_tdst_World_ *, ULONG);
USHORT						COL_GameMaterial_GetID(struct OBJ_tdst_GameObject_ *, struct WOR_tdst_World_ *, ULONG);
ULONG						COL_GameMaterial_GetODESound(struct OBJ_tdst_GameObject_ *, struct WOR_tdst_World_ *, ULONG *, ULONG);




#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* _cplusplcus */

#endif /* __COL_REPORT_ */
