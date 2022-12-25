/*$T SOFTPickingBuffer.h GC! 1.081 06/05/00 10:06:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#ifdef ACTIVE_EDITORS
#include "MAD_loadSave/Sources/MAD_Struct_V0.h"
#include "MAD_rasterize/Sources/MAD_Rasterize.h"
#endif
#endif
#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "CAMera/CAMera.h"

#ifndef __PICKINGBUFFER_H__
#define __PICKINGBUFFER_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constant
 ***********************************************************************************************************************
 */

#define SOFT_Cul_PBF_DontUseIt			0x01
#define SOFT_Cul_PBF_BoxSelection		0x02
#define SOFT_Cul_PBF_PickWhileRender	0x04
#define SOFT_Cul_PBF_WindowSelection	0x08
#define SOFT_Cul_PBF_BackfaceRejection	0x10
#define SOFT_Cul_PBF_Iso                0x20

#define SOFT_Cuc_PBQF_All				0
#define SOFT_Cuc_PBQF_GameObject		1
#define SOFT_Cuc_PBQF_Link				2
#define SOFT_Cuc_PBQF_Zone				4
#define SOFT_Cuc_PBQF_HieLink			10			/* Do not put 8 (Picking Input/Output Constant Problem) */
#define SOFT_Cuc_PBQF_PostIt			11
#define SOFT_Cul_PBQF_TypeMask			0x000000FF

#define SOFT_Cul_PBQF_ElementMask		0x0000FF00
#define SOFT_Cul_PBQF_ElementShift		8
#define SOFT_Cul_PBQF_TriangleMask		0xFFFF0000
#define SOFT_Cul_PBQF_TriangleShift		16

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

/*
 -----------------------------------------------------------------------------------------------------------------------
    Picking buffer pixel data
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	SOFT_tdst_PickingBuffer_Pixel_
{
	ULONG	ul_SelMask;
	ULONG	ul_Value;
	ULONG	ul_ValueExt;
	float	f_Ooz;
} SOFT_tdst_PickingBuffer_Pixel;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Picking buffer result
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	SOFT_tdst_PickingBuffer_Query_
{
	/* Query data */
	MATH_tdst_Vector				st_Point1;
	MATH_tdst_Vector				st_Point2;
	LONG							l_Tolerance;
	ULONG							l_Filter;

	/* Result data */
	SOFT_tdst_PickingBuffer_Pixel	st_First;
	SOFT_tdst_PickingBuffer_Pixel	*dst_List;
	LONG							l_Number;
	LONG							l_NumberMax;
} SOFT_tdst_PickingBuffer_Query;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Picking buffer data
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	SOFT_tdst_PickingBuffer_
{
	LONG							l_Width;
	LONG							l_Height;

	ULONG							ul_Flags;
	ULONG							ul_UserFlags;

	SOFT_tdst_PickingBuffer_Pixel	*dst_Pixel;
#ifdef ACTIVE_EDITORS
	MAD_R_Clipping_Info				st_ClipInfo;
#endif
	float							f_XScreen;
	float							f_YScreen;
	float							f_Radius;
	float							f_Radius2;

	SOFT_tdst_PickingBuffer_Query	st_Query;
	LONG							l_CurrentValue;
	LONG							l_CurrentValueExt;
	LONG							l_Added;

} SOFT_tdst_PickingBuffer;

/*$4
 ***********************************************************************************************************************
    Picking buffer functions
 ***********************************************************************************************************************
 */

void						SOFT_PickingBuffer_QueryInit(SOFT_tdst_PickingBuffer_Query *);
void						SOFT_PickingBuffer_QueryClose(SOFT_tdst_PickingBuffer_Query *);
void						SOFT_PickingBuffer_QueryReinit(SOFT_tdst_PickingBuffer_Query *);
void						SOFT_PickingBuffer_QueryAddInList(SOFT_tdst_PickingBuffer_Query *, ULONG, ULONG);

void						SOFT_PickingBuffer_Init(SOFT_tdst_PickingBuffer *, LONG, LONG);
void						SOFT_PickingBuffer_Reinit(SOFT_tdst_PickingBuffer *, LONG, LONG);
void						SOFT_PickingBuffer_Close(SOFT_tdst_PickingBuffer *);

void						SOFT_ResetClipWindow(SOFT_tdst_PickingBuffer *);
void						SOFT_SetClipWindow(SOFT_tdst_PickingBuffer *, int, int, int, int);
void						SOFT_PickingBuffer_SetSelWindow(SOFT_tdst_PickingBuffer *);
void						SOFT_PickingBuffer_BeginPickingWhileRender(SOFT_tdst_PickingBuffer *, ULONG, ULONG);
void						SOFT_PickingBuffer_EndPickingWhileRender(SOFT_tdst_PickingBuffer *);
void						SOFT_PickingBuffer_SetMatrix(SOFT_tdst_PickingBuffer *, CAM_tdst_Camera *);

void						SOFT_PickingBuffer_Clear(SOFT_tdst_PickingBuffer *);
void						SOFT_PickingBuffer_Render
							(
								SOFT_tdst_PickingBuffer *,
								MATH_tdst_Vector **,
								ULONG,
								ULONG,
								ULONG
							);

BOOL						SOFT_b_PickingBuffer_PickExt(SOFT_tdst_PickingBuffer *, SOFT_tdst_PickingBuffer_Query *);
BOOL						SOFT_b_PickingBuffer_GetOoZ(SOFT_tdst_PickingBuffer *, MATH_tdst_Vector *, float *);
BOOL						SOFT_b_PickingBuffer_GetMinOoZRect
							(
								SOFT_tdst_PickingBuffer *,
								MATH_tdst_Vector *,
								MATH_tdst_Vector *,
								float *
							);
BOOL						SOFT_b_PickingBuffer_GetMinOoZRectPond
							(
								SOFT_tdst_PickingBuffer *,
								MATH_tdst_Vector *,
								MATH_tdst_Vector *,
								float *
							);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __PICKINGBUFFER_H__ */
