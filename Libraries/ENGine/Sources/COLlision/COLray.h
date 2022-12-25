/*$T COLray.h GC! 1.081 06/08/00 14:33:40 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __COLRAY_H__
#define __COLRAY_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

struct OBJ_tdst_GameObject_ *COL_RayBV
							(
								TAB_tdst_PFtable *,
								MATH_tdst_Vector *,
								MATH_tdst_Vector *,
								float,
								ULONG,
								ULONG,
								ULONG,
								BOOL
							);

struct OBJ_tdst_GameObject_ *COL_Visual_RayCast
							(
								struct WOR_tdst_World_ *,
								struct OBJ_tdst_GameObject_ *,
								MATH_tdst_Vector *,
								MATH_tdst_Vector *,
								float,
								ULONG,
								ULONG,
								ULONG,
								BOOL *,
								BOOL,
								BOOL
#ifdef JADEFUSION
								,BOOL _bIgnoreDrawState = FALSE
#endif
								);

struct OBJ_tdst_GameObject_ *COL_Visual_RayCast_OneObject
							(
								struct OBJ_tdst_GameObject_ *,
								MATH_tdst_Vector *,
								MATH_tdst_Vector *,
								float,
								ULONG,
								ULONG,
								ULONG,
								BOOL *,
								BOOL,
								BOOL,
								BOOL,
								BOOL
							);


struct OBJ_tdst_GameObject_ *COL_ColMap_RayCast_OneObject
							(
								OBJ_tdst_GameObject *,
								MATH_tdst_Vector *,
								MATH_tdst_Vector *,
								float,
								ULONG,
								ULONG,
								ULONG,
								BOOL*,
								BOOL,
								BOOL,
								BOOL,
								BOOL,
								BOOL,
								BOOL				
								);


struct OBJ_tdst_GameObject_	*COL_ColMap_RayCast
							(
								struct WOR_tdst_World_		*_pst_World,
								struct OBJ_tdst_GameObject_ *_pst_GO,
								MATH_tdst_Vector			*_pst_Orig,
								MATH_tdst_Vector			*_pst_Dir,
								float						_f_MaxDist,
								ULONG						_ul_Filter,
								ULONG						_ul_NoFilter,
								ULONG						_ul_ID,
								BOOL						*_pb_Visible,
								BOOL						_b_UseRaySkipFlag,
								BOOL						_b_SkipCrossable,
								BOOL						_b_SkipCamera,
								BOOL						_b_SkipFlagX,
								BOOL						_b_UserCrossable,
								BOOL						_b_UseBoneOBBox
							);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif //__COLRAY_H__
