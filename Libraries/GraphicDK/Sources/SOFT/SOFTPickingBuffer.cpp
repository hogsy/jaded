/*$T SOFTPickingBuffer.c GC! 1.081 06/05/00 10:49:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "SOFT/SOFTPickingBuffer.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEO_SKIN.h"
#include "GDInterface/GDInterface.h"
#include "BASe/MEMory/MEM.h"

#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    Private functions prototypes
 ***********************************************************************************************************************
 */

void							SOFT_PickingBuffer_Project(MAD_R_Vertex *, MAD_R_Vertex *, MAD_R_Clipping_Info *);
void							SOFT_PickingBuffer_Iso_Project(MAD_R_Vertex *, MAD_R_Vertex *, MAD_R_Clipping_Info *);
void							SOFT_PickingBuffer_Rasterize(ULONG Y, MAD_R_Raster *, MAD_R_Vertex *);
void							SOFT_PickingBuffer_RasterizeAndPickWindow(ULONG Y, MAD_R_Raster *, MAD_R_Vertex *);
void							SOFT_PickingBuffer_RasterizeAndPickCross(ULONG Y, MAD_R_Raster *, MAD_R_Vertex *);
void							SOFT_PickingBuffer_Iso_Rasterize(ULONG Y, MAD_R_Raster *, MAD_R_Vertex *);
void							SOFT_PickingBuffer_Iso_RasterizeAndPickWindow(ULONG Y, MAD_R_Raster *, MAD_R_Vertex *);
void							SOFT_PickingBuffer_Iso_RasterizeAndPickCross(ULONG Y, MAD_R_Raster *, MAD_R_Vertex *);


/*$4
 ***********************************************************************************************************************
    Globals var
 ***********************************************************************************************************************
 */

static SOFT_tdst_PickingBuffer	*SOFT_sgpst_CurrentPB;
static ULONG					SOFT_sgul_CurrentValue;
static ULONG					SOFT_sgul_CurrentValueExt;
static ULONG					SOFT_sgul_Added;
BOOL							SOFT_gb_NoDepthTest = 0;
BOOL							SOFT_gb_WindowSel = 0;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_QueryInit(SOFT_tdst_PickingBuffer_Query *_pst_PBQ)
{
	L_memset(_pst_PBQ, 0, sizeof(SOFT_tdst_PickingBuffer_Query));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_QueryClose(SOFT_tdst_PickingBuffer_Query *_pst_PBQ)
{
	if(_pst_PBQ->l_NumberMax)
	{
		L_free(_pst_PBQ->dst_List);
		_pst_PBQ->l_NumberMax = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_QueryReinit(SOFT_tdst_PickingBuffer_Query *_pst_PBQ)
{
	_pst_PBQ->l_Number = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SOFT_l_PickingBuffer_QueryAddInList
(
	SOFT_tdst_PickingBuffer_Query	*_pst_PBQ,
	ULONG							_ul_Value,
	ULONG							_ul_ValueExt,
	float							_f_Ooz
)
{
	/*~~~~~~~~~~~*/
	LONG	l_Size;
	/*~~~~~~~~~~~*/

	if(_pst_PBQ->l_Filter && (_pst_PBQ->l_Filter != (_ul_ValueExt & SOFT_Cul_PBQF_TypeMask))) return -1;

	if(_pst_PBQ->l_Number == _pst_PBQ->l_NumberMax)
	{
		if(_pst_PBQ->l_NumberMax == 0)
		{
			l_Size = sizeof(SOFT_tdst_PickingBuffer_Pixel) * 64;
			_pst_PBQ->dst_List = (SOFT_tdst_PickingBuffer_Pixel*)L_malloc(l_Size);
		}
		else
		{
			l_Size = sizeof(SOFT_tdst_PickingBuffer_Pixel) * (64 + _pst_PBQ->l_NumberMax);
			_pst_PBQ->dst_List = (SOFT_tdst_PickingBuffer_Pixel*)L_realloc(_pst_PBQ->dst_List, l_Size);
		}

		_pst_PBQ->l_NumberMax += 64;
	}

	_pst_PBQ->dst_List[_pst_PBQ->l_Number].ul_Value = _ul_Value;
	_pst_PBQ->dst_List[_pst_PBQ->l_Number].ul_ValueExt = _ul_ValueExt;
	_pst_PBQ->dst_List[_pst_PBQ->l_Number].f_Ooz = _f_Ooz;
	return _pst_PBQ->l_Number++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_Init(SOFT_tdst_PickingBuffer *_pst_PB, LONG _l_Width, LONG _l_Height)
{
	_pst_PB->l_Width = _l_Width;
	_pst_PB->l_Height = _l_Height;
	_pst_PB->ul_UserFlags = 0;
	_pst_PB->dst_Pixel = (SOFT_tdst_PickingBuffer_Pixel *) L_malloc(_l_Width * _l_Height * sizeof(SOFT_tdst_PickingBuffer_Pixel));
	SOFT_ResetClipWindow(_pst_PB);
	SOFT_PickingBuffer_Clear(_pst_PB);

	_pst_PB->st_ClipInfo.NmbrOfSclrToInt = 0;
	_pst_PB->st_ClipInfo.Force2sidedFLAG = 0;
	_pst_PB->st_ClipInfo.InvertBCKFCFlag = 0;
	_pst_PB->st_ClipInfo.MAD_DrawLineCLBK = NULL;
	_pst_PB->st_ClipInfo.RasterizeCallback = SOFT_PickingBuffer_Rasterize;
	_pst_PB->st_ClipInfo.ProjectVertexCLBK = SOFT_PickingBuffer_Project;
	_pst_PB->st_ClipInfo.MAD_PlotCLBK = NULL;

	SOFT_PickingBuffer_QueryInit(&_pst_PB->st_Query);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_Reinit(SOFT_tdst_PickingBuffer *_pst_PB, LONG _l_Width, LONG _l_Height)
{
	if(_pst_PB->l_Width != _l_Width || _pst_PB->l_Height != _l_Height)
	{
		SOFT_PickingBuffer_Close(_pst_PB);
		SOFT_PickingBuffer_Init(_pst_PB, _l_Width, _l_Height);
		SOFT_PickingBuffer_QueryReinit(&_pst_PB->st_Query);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_Close(SOFT_tdst_PickingBuffer *_pst_PB)
{
	if(_pst_PB->dst_Pixel) L_free(_pst_PB->dst_Pixel);
	_pst_PB->dst_Pixel = NULL;
	SOFT_PickingBuffer_QueryClose(&_pst_PB->st_Query);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_Clear(SOFT_tdst_PickingBuffer *_pst_PB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	*pul_Buffer, *pul_Last, *pul_EndLine;
	ULONG	ul_PixelSize, ul_LineSize, ul_PixelIndex, ul_Pitch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_PixelIndex = (LONG) (_pst_PB->st_ClipInfo.YMin * _pst_PB->l_Width + _pst_PB->st_ClipInfo.XMin);
	pul_Buffer = (ULONG *) (_pst_PB->dst_Pixel + ul_PixelIndex);
	ul_PixelIndex = (LONG) (_pst_PB->st_ClipInfo.YMax * _pst_PB->l_Width + _pst_PB->st_ClipInfo.XMax);
	pul_Last = (ULONG *) (_pst_PB->dst_Pixel + ul_PixelIndex + 1);

	ul_PixelSize = sizeof(SOFT_tdst_PickingBuffer_Pixel) / sizeof(LONG);
	ul_LineSize = ((LONG) _pst_PB->st_ClipInfo.XMax - (LONG) _pst_PB->st_ClipInfo.XMin + 1) * ul_PixelSize;
	ul_Pitch = (_pst_PB->l_Width * ul_PixelSize) - ul_LineSize;

	while(pul_Buffer < pul_Last)
	{
		pul_EndLine = pul_Buffer + ul_LineSize;
		while(pul_Buffer < pul_EndLine) *pul_Buffer++ = 0;
		pul_Buffer += ul_Pitch;
	}

	SOFT_PickingBuffer_QueryReinit(&_pst_PB->st_Query);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_ResetClipWindow(SOFT_tdst_PickingBuffer *_pst_PB)
{
	_pst_PB->st_ClipInfo.XMin = 0;
	_pst_PB->st_ClipInfo.XMax = (float) _pst_PB->l_Width - 1;
	_pst_PB->st_ClipInfo.YMin = 0;
	_pst_PB->st_ClipInfo.YMax = (float) _pst_PB->l_Height - 1;
	_pst_PB->st_ClipInfo.ZMin = .001f;
	_pst_PB->st_ClipInfo.ZMax = Cf_Infinit;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_SetClipWindow(SOFT_tdst_PickingBuffer *_pst_PB, int _i_Left, int _i_Top, int _i_Right, int _i_Bottom)
{
	_pst_PB->st_ClipInfo.XMin = (float) ((_i_Left < 0) ? 0 : _i_Left);
	_pst_PB->st_ClipInfo.XMax = (float) ((_i_Right > _pst_PB->l_Width - 1) ? _pst_PB->l_Width - 1 : _i_Right);
	_pst_PB->st_ClipInfo.YMin = (float) ((_i_Top < 0) ? 0 : _i_Top);
	_pst_PB->st_ClipInfo.YMax = (float) ((_i_Bottom > _pst_PB->l_Height - 1) ? _pst_PB->l_Height - 1 : _i_Bottom);
	_pst_PB->st_ClipInfo.ZMin = .001f;
	_pst_PB->st_ClipInfo.ZMax = Cf_Infinit;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_SetSelWindow(SOFT_tdst_PickingBuffer *_pst_PB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	SOFT_tdst_PickingBuffer_Pixel	*p, *p_LineEnd, *p_Last;
	LONG							l_LineCount, l_Pitch;
	int								i_X1, i_Y1, i_X2, i_Y2;
	int								i_X0, i_Y0, i_X, i_Y;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(_pst_PB->ul_Flags & SOFT_Cul_PBF_PickWhileRender)) return;
	pst_Query = &_pst_PB->st_Query;
	if(!pst_Query) return;

	i_X1 = (int) (pst_Query->st_Point1.x * _pst_PB->l_Width);
	i_Y1 = (int) (pst_Query->st_Point1.y * _pst_PB->l_Height);

	if(_pst_PB->ul_Flags & SOFT_Cul_PBF_BoxSelection)
	{
		i_X2 = (int) (pst_Query->st_Point2.x * _pst_PB->l_Width);
		i_Y2 = (int) (pst_Query->st_Point2.y * _pst_PB->l_Height);
	}
	else
	{
		if(pst_Query->l_Tolerance)
		{
			i_X1 = (int) (pst_Query->st_Point1.x * _pst_PB->l_Width);
			i_X2 = i_X1 + pst_Query->l_Tolerance;
			i_X1 -= pst_Query->l_Tolerance;
			i_Y1 = (int) (pst_Query->st_Point1.y * _pst_PB->l_Height);
			i_Y2 = i_Y1 + pst_Query->l_Tolerance;
			i_Y1 -= pst_Query->l_Tolerance;
		}
		else
		{
			i_X2 = i_X1;
			i_Y2 = i_Y1;
		}
	}

	i_X0 = (i_X1 + i_X2) >> 1;
	i_Y0 = (i_Y1 + i_Y2) >> 1;

	if(i_X1 < (int) _pst_PB->st_ClipInfo.XMin) i_X1 = (int) _pst_PB->st_ClipInfo.XMin;
	if(i_X2 > (int) _pst_PB->st_ClipInfo.XMax) i_X2 = (int) _pst_PB->st_ClipInfo.XMax;
	if(i_Y1 < (int) _pst_PB->st_ClipInfo.YMin) i_Y1 = (int) _pst_PB->st_ClipInfo.YMin;
	if(i_Y2 > (int) _pst_PB->st_ClipInfo.YMax) i_Y2 = (int) _pst_PB->st_ClipInfo.YMax;

	p = _pst_PB->dst_Pixel + i_Y1 * _pst_PB->l_Width + i_X1;
	l_LineCount = i_X2 - i_X1 + 1;
	l_Pitch = _pst_PB->l_Width - l_LineCount;
	p_Last = p + (i_Y2 - i_Y1 + 1) * _pst_PB->l_Width;
	i_Y = i_Y1 - i_Y0;

	while(p < p_Last)
	{
		p_LineEnd = p + l_LineCount;
		i_X = i_X1 - i_X0;
		while(p < p_LineEnd)
		{
			p->ul_SelMask = 0xFFFFFFFF - (i_Y * i_Y) - (i_X * i_X);
			p++;
			i_X--;
		}

		p += l_Pitch;
		i_Y--;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_BeginPickingWhileRender(SOFT_tdst_PickingBuffer *_pst_PB, ULONG _ul_Value, ULONG _ul_ValueExt)
{
	_pst_PB->l_Added = -1;
    if ( !(_pst_PB->ul_Flags & SOFT_Cul_PBF_Iso) )
    {
	    if(_pst_PB->ul_Flags & SOFT_Cul_PBF_WindowSelection)
		    _pst_PB->st_ClipInfo.RasterizeCallback = SOFT_PickingBuffer_RasterizeAndPickWindow;
	    else
		    _pst_PB->st_ClipInfo.RasterizeCallback = SOFT_PickingBuffer_RasterizeAndPickCross;
    }
    else
    {
        if(_pst_PB->ul_Flags & SOFT_Cul_PBF_WindowSelection)
		    _pst_PB->st_ClipInfo.RasterizeCallback = SOFT_PickingBuffer_Iso_RasterizeAndPickWindow;
	    else
		    _pst_PB->st_ClipInfo.RasterizeCallback = SOFT_PickingBuffer_Iso_RasterizeAndPickCross;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_EndPickingWhileRender(SOFT_tdst_PickingBuffer *_pst_PB)
{
	_pst_PB->st_ClipInfo.RasterizeCallback = SOFT_PickingBuffer_Rasterize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL SOFT_b_PickingBuffer_PickExt(SOFT_tdst_PickingBuffer *_pst_PB, SOFT_tdst_PickingBuffer_Query *_pst_PBQ)
{
	/*~~~~~~~~~*/
	LONG	x, y;
	/*~~~~~~~~~*/

	if(_pst_PB->dst_Pixel == NULL) return 0;

	x = (ULONG) (_pst_PBQ->st_Point1.x * _pst_PB->l_Width);
	y = (ULONG) (_pst_PBQ->st_Point1.y * _pst_PB->l_Height);

	if((ULONG) x >= (ULONG) _pst_PB->l_Width) return 0;
	if((ULONG) y >= (ULONG) _pst_PB->l_Height) return 0;

	if(_pst_PBQ->l_Tolerance)
	{
	}

	_pst_PBQ->st_First = _pst_PB->dst_Pixel[_pst_PB->l_Width * y + x];

	x = _pst_PBQ->l_Filter & SOFT_Cul_PBQF_TypeMask;
	if(x)
	{
		y = _pst_PBQ->st_First.ul_ValueExt & SOFT_Cul_PBQF_TypeMask;
		if(x != y) return FALSE;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL SOFT_b_PickingBuffer_GetOoZ(SOFT_tdst_PickingBuffer *_pst_PB, MATH_tdst_Vector *_pst_Point, float *_pf_Ooz)
{
	/*~~~~~~~~~*/
	ULONG	x, y;
	/*~~~~~~~~~*/

	if(_pst_PB->dst_Pixel == NULL) return FALSE;

	x = (ULONG) (_pst_Point->x * _pst_PB->l_Width);
	y = (ULONG) (_pst_Point->y * _pst_PB->l_Height);

	if((x >= (ULONG) _pst_PB->l_Width) || (y >= (ULONG) _pst_PB->l_Height)) return FALSE;

	*_pf_Ooz = _pst_PB->dst_Pixel[_pst_PB->l_Width * y + x].f_Ooz;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL SOFT_b_PickingBuffer_GetMinOoZRect
(
	SOFT_tdst_PickingBuffer *_pst_PB,
	MATH_tdst_Vector		*_pst_PointTopLeft,
	MATH_tdst_Vector		*_pst_PointBottomRight,
	float					*_pf_Ooz
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG	x1, y1, x2, y2, i, j;
	float	fMinZ;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_PB->dst_Pixel == NULL) return FALSE;

	x1 = (LONG) (_pst_PointTopLeft->x * _pst_PB->l_Width);
	y1 = (LONG) (_pst_PointTopLeft->y * _pst_PB->l_Height);
	x2 = (LONG) (_pst_PointBottomRight->x * _pst_PB->l_Width);
	y2 = (LONG) (_pst_PointBottomRight->y * _pst_PB->l_Height);

	if(x1 >= (LONG) _pst_PB->l_Width) x1 = _pst_PB->l_Width - 1;
	if(x2 >= (LONG) _pst_PB->l_Width) x2 = _pst_PB->l_Width - 1;
	if(y1 >= (LONG) _pst_PB->l_Height) y1 = _pst_PB->l_Height - 1;
	if(y2 >= (LONG) _pst_PB->l_Height) y2 = _pst_PB->l_Height - 1;

	if(x2 < 0) x2 = 0;
	if(x1 < 0) x2 = 0;
	if(y2 < 0) y2 = 0;
	if(y1 < 0) y1 = 0;

	fMinZ = 0;
	for(i = y1; i >= y2; i--)
	{
		for(j = x1; j <= x2; j++)
		{
			*_pf_Ooz = _pst_PB->dst_Pixel[_pst_PB->l_Width * i + j].f_Ooz;
			if(*_pf_Ooz && (*_pf_Ooz > fMinZ)) fMinZ = *_pf_Ooz;
		}
	}

	*_pf_Ooz = fMinZ;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL SOFT_b_PickingBuffer_GetMinOoZRectPond
(
	SOFT_tdst_PickingBuffer *_pst_PB,
	MATH_tdst_Vector		*_pst_PointTopLeft,
	MATH_tdst_Vector		*_pst_PointBottomRight,
	float					*_pf_Ooz
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG	x1, y1, x2, y2, i, j;
	float	fMinZ, f1;
	int		cpt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_PB->dst_Pixel == NULL) return FALSE;

	x1 = (LONG) (_pst_PointTopLeft->x * _pst_PB->l_Width);
	y1 = (LONG) (_pst_PointTopLeft->y * _pst_PB->l_Height);
	x2 = (LONG) (_pst_PointBottomRight->x * _pst_PB->l_Width);
	y2 = (LONG) (_pst_PointBottomRight->y * _pst_PB->l_Height);

	if(x1 >= (LONG) _pst_PB->l_Width) x1 = _pst_PB->l_Width - 1;
	if(x2 >= (LONG) _pst_PB->l_Width) x2 = _pst_PB->l_Width - 1;
	if(y1 >= (LONG) _pst_PB->l_Height) y1 = _pst_PB->l_Height - 1;
	if(y2 >= (LONG) _pst_PB->l_Height) y2 = _pst_PB->l_Height - 1;

	if(x2 < 0) x2 = 0;
	if(x1 < 0) x2 = 0;
	if(y2 < 0) y2 = 0;
	if(y1 < 0) y1 = 0;

	cpt = 0;
	fMinZ = 0;
	for(i = y1; i >= y2; i -= 1)
	{
		for(j = x1; j <= x2; j += 1)
		{
			*_pf_Ooz = _pst_PB->dst_Pixel[_pst_PB->l_Width * i + j].f_Ooz;
			f1 = *_pf_Ooz;
			if(f1)
			{
				fMinZ += f1;
				cpt++;
			}
		}
	}

	*_pf_Ooz = fMinZ / cpt;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_SetMatrix(SOFT_tdst_PickingBuffer *_pst_PB, CAM_tdst_Camera *_pst_Cam)
{
	/*~~~~~~~~~~~~~~~~*/
	RADCAM	*pst_MadCam;
	/*~~~~~~~~~~~~~~~~*/

	pst_MadCam = &_pst_PB->st_ClipInfo.ProjectCam;
	pst_MadCam->PX = _pst_Cam->f_CenterX;
	pst_MadCam->PY = _pst_Cam->f_CenterY;
	pst_MadCam->CX = _pst_Cam->f_FactorX;
	pst_MadCam->CY = _pst_Cam->f_FactorY;
	pst_MadCam->Object = (MAD_NodeID *) _pst_Cam;

	if(_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
    {
        _pst_PB->ul_Flags &= ~SOFT_Cul_PBF_Iso;
		_pst_PB->st_ClipInfo.ProjectVertexCLBK = SOFT_PickingBuffer_Project;
        _pst_PB->st_ClipInfo.RasterizeCallback = SOFT_PickingBuffer_Rasterize;
        _pst_PB->st_ClipInfo.ClipFlags = 0;
    }
	else
	{
		/*
		 * pst_MadCam->CX /= (_pst_Cam->f_IsoFactor * 2); pst_MadCam->CY /=
		 * (_pst_Cam->f_IsoFactor * 2);
		 */
        _pst_PB->ul_Flags |= SOFT_Cul_PBF_Iso;
		_pst_PB->st_ClipInfo.ProjectVertexCLBK = SOFT_PickingBuffer_Iso_Project;
        _pst_PB->st_ClipInfo.RasterizeCallback = SOFT_PickingBuffer_Iso_Rasterize;
        _pst_PB->st_ClipInfo.ClipFlags = MAD_RASTERIZE_NoClipZ;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_Render
(
	SOFT_tdst_PickingBuffer *_pst_PB,
	MATH_tdst_Vector		**_ppst_Triangle,
	ULONG					_ul_Value,
	ULONG					_ul_ValueExt,
	ULONG					ulDrawMask
)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	MAD_R_Vertex	P3[3];
	float			f_Dummy;
	/*~~~~~~~~~~~~~~~~~~~~*/

	SOFT_sgul_CurrentValue = _ul_Value;
	SOFT_sgul_CurrentValueExt = _ul_ValueExt;
	SOFT_sgpst_CurrentPB = _pst_PB;
	_pst_PB->st_ClipInfo.InvertBCKFCFlag = (ulDrawMask & GDI_Cul_DM_NotInvertBF) ^ GDI_Cul_DM_NotInvertBF;
	_pst_PB->st_ClipInfo.Force2sidedFLAG = (ulDrawMask & GDI_Cul_DM_TestBackFace) ^ GDI_Cul_DM_TestBackFace;

	P3[0] = *(MAD_R_Vertex *) _ppst_Triangle[0];
	P3[1] = *(MAD_R_Vertex *) _ppst_Triangle[1];
	P3[2] = *(MAD_R_Vertex *) _ppst_Triangle[2];

	MAD_Rasterize_TriQuad3D(P3, &_pst_PB->st_ClipInfo, &f_Dummy);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_Project(MAD_R_Vertex *P3D, MAD_R_Vertex *P2D, MAD_R_Clipping_Info *C)
{
	P2D->OoZ = fOptInv(P3D->Z);
	P2D->X = P3D->X * P2D->OoZ * C->ProjectCam.CX + C->ProjectCam.PX;
	P2D->Y = C->ProjectCam.PY - P3D->Y * P2D->OoZ * C->ProjectCam.CY;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_Iso_Project(MAD_R_Vertex *P3D, MAD_R_Vertex *P2D, MAD_R_Clipping_Info *C)
{
	P2D->OoZ = fOptInv(P3D->Z);
   	P2D->X = P3D->X * C->ProjectCam.CX + C->ProjectCam.PX;
	P2D->Y = C->ProjectCam.PY - P3D->Y * C->ProjectCam.CY;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_Rasterize(ULONG Y, MAD_R_Raster *R, MAD_R_Vertex *PXInc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG							Xcounter, End;
	float							Interpol;
	SOFT_tdst_PickingBuffer_Pixel	*pst_First, *pst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((R->XEnd - R->P.X) > 0)
	{
		Xcounter = lFloatToLongOpt(R->P.X);
		End = lFloatToLongOpt(R->XEnd);

		pst_First = SOFT_sgpst_CurrentPB->dst_Pixel + Y * SOFT_sgpst_CurrentPB->l_Width + Xcounter;
		pst_Last = pst_First + End - Xcounter;

		Interpol = R->P.OoZ;

		while(pst_First < pst_Last)
		{
			if(pst_First->f_Ooz < Interpol)
			{
 				pst_First->ul_Value = SOFT_sgul_CurrentValue;
				pst_First->ul_ValueExt = SOFT_sgul_CurrentValueExt;
				pst_First->f_Ooz = Interpol;
			}

			Interpol += PXInc->OoZ;
			pst_First++;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_RasterizeAndPickWindow(ULONG Y, MAD_R_Raster *R, MAD_R_Vertex *PXInc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG							Xcounter, End;
	float							Interpol;
	SOFT_tdst_PickingBuffer_Pixel	*pst_First, *pst_Last, *pst_Pixel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((R->XEnd - R->P.X) > 0)
	{
		Xcounter = lFloatToLongOpt(R->P.X);
		End = lFloatToLongOpt(R->XEnd);

		pst_First = SOFT_sgpst_CurrentPB->dst_Pixel + Y * SOFT_sgpst_CurrentPB->l_Width + Xcounter;
		pst_Last = pst_First + End - Xcounter;

		Interpol = R->P.OoZ;

		while(pst_First < pst_Last)
		{
			if(SOFT_sgpst_CurrentPB->l_Added != -2)
			{
				if(pst_First->ul_SelMask)
				{
					if((SOFT_sgpst_CurrentPB->l_Added == -1))
					{
						SOFT_sgpst_CurrentPB->l_Added = SOFT_l_PickingBuffer_QueryAddInList
							(
								&SOFT_sgpst_CurrentPB->st_Query,
								SOFT_sgul_CurrentValue,
								SOFT_sgul_CurrentValueExt,
								Interpol
							);
					}
					else
					{
						pst_Pixel = SOFT_sgpst_CurrentPB->st_Query.dst_List + SOFT_sgpst_CurrentPB->l_Added;
                        if( (pst_Pixel->f_Ooz < Interpol) || SOFT_gb_NoDepthTest)
                        {
                            pst_Pixel->f_Ooz = Interpol;
                            pst_Pixel->ul_ValueExt = SOFT_sgul_CurrentValueExt;
                        }
					}
				}
				else
				{
					if((SOFT_sgpst_CurrentPB->l_Added != -1)) SOFT_sgpst_CurrentPB->st_Query.l_Number--;
					SOFT_sgpst_CurrentPB->l_Added = -2;
				}
			}

			if(pst_First->f_Ooz < Interpol || SOFT_gb_NoDepthTest)
			{
				pst_First->ul_Value = SOFT_sgul_CurrentValue;
				pst_First->ul_ValueExt = SOFT_sgul_CurrentValueExt;
				pst_First->f_Ooz = Interpol;
			}

			Interpol += PXInc->OoZ;
			pst_First++;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_RasterizeAndPickCross(ULONG Y, MAD_R_Raster *R, MAD_R_Vertex *PXInc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG							Xcounter, End;
	float							Interpol;
	SOFT_tdst_PickingBuffer_Pixel	*pst_First, *pst_Last, *pst_Pixel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((R->XEnd - R->P.X) > 0)
	{
		Xcounter = lFloatToLongOpt(R->P.X);
		End = lFloatToLongOpt(R->XEnd);

		pst_First = SOFT_sgpst_CurrentPB->dst_Pixel + Y * SOFT_sgpst_CurrentPB->l_Width + Xcounter;
		pst_Last = pst_First + End - Xcounter;

		Interpol = R->P.OoZ;

		while(pst_First < pst_Last)
		{
			if(pst_First->ul_SelMask)
			{
				if((SOFT_sgpst_CurrentPB->l_Added == -1))
				{
					if(SOFT_gb_NoDepthTest && !SOFT_gb_WindowSel) SOFT_PickingBuffer_QueryReinit(&SOFT_sgpst_CurrentPB->st_Query);
					SOFT_sgpst_CurrentPB->l_Added = SOFT_l_PickingBuffer_QueryAddInList
						(
							&SOFT_sgpst_CurrentPB->st_Query,
							SOFT_sgul_CurrentValue,
							SOFT_sgul_CurrentValueExt,
							Interpol
						);
				}
				else
				{
					pst_Pixel = SOFT_sgpst_CurrentPB->st_Query.dst_List + SOFT_sgpst_CurrentPB->l_Added;
					if( (pst_Pixel->f_Ooz < Interpol) || SOFT_gb_NoDepthTest)
                    {
                        pst_Pixel->f_Ooz = Interpol;
                        pst_Pixel->ul_ValueExt = SOFT_sgul_CurrentValueExt;
                    }
				}
			}

			if( (pst_First->f_Ooz < Interpol) || SOFT_gb_NoDepthTest)
			{
				pst_First->ul_Value = SOFT_sgul_CurrentValue;
				pst_First->ul_ValueExt = SOFT_sgul_CurrentValueExt;
				pst_First->f_Ooz = Interpol;
			}

			Interpol += PXInc->OoZ;
			pst_First++;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_Iso_Rasterize(ULONG Y, MAD_R_Raster *R, MAD_R_Vertex *PXInc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG							Xcounter, End;
	float							Interpol;
	SOFT_tdst_PickingBuffer_Pixel	*pst_First, *pst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((R->XEnd - R->P.X) > 0)
	{
		Xcounter = lFloatToLongOpt(R->P.X);
		End = lFloatToLongOpt(R->XEnd);

		pst_First = SOFT_sgpst_CurrentPB->dst_Pixel + Y * SOFT_sgpst_CurrentPB->l_Width + Xcounter;
		pst_Last = pst_First + End - Xcounter;

		Interpol = R->P.OoZ;

		while(pst_First < pst_Last)
		{
			if( fOptInv( pst_First->f_Ooz ) > fOptInv( Interpol) )
			{
 				pst_First->ul_Value = SOFT_sgul_CurrentValue;
				pst_First->ul_ValueExt = SOFT_sgul_CurrentValueExt;
				pst_First->f_Ooz = Interpol;
			}

			Interpol += PXInc->OoZ;
			pst_First++;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_Iso_RasterizeAndPickWindow(ULONG Y, MAD_R_Raster *R, MAD_R_Vertex *PXInc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG							Xcounter, End;
	float							Interpol;
	SOFT_tdst_PickingBuffer_Pixel	*pst_First, *pst_Last, *pst_Pixel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((R->XEnd - R->P.X) > 0)
	{
		Xcounter = lFloatToLongOpt(R->P.X);
		End = lFloatToLongOpt(R->XEnd);

		pst_First = SOFT_sgpst_CurrentPB->dst_Pixel + Y * SOFT_sgpst_CurrentPB->l_Width + Xcounter;
		pst_Last = pst_First + End - Xcounter;

		Interpol = R->P.OoZ;

		while(pst_First < pst_Last)
		{
			if(SOFT_sgpst_CurrentPB->l_Added != -2)
			{
				if(pst_First->ul_SelMask)
				{
					if((SOFT_sgpst_CurrentPB->l_Added == -1))
					{
						SOFT_sgpst_CurrentPB->l_Added = SOFT_l_PickingBuffer_QueryAddInList
							(
								&SOFT_sgpst_CurrentPB->st_Query,
								SOFT_sgul_CurrentValue,
								SOFT_sgul_CurrentValueExt,
								Interpol
							);
					}
					else
					{
						pst_Pixel = SOFT_sgpst_CurrentPB->st_Query.dst_List + SOFT_sgpst_CurrentPB->l_Added;
                        if( (fOptInv( pst_Pixel->f_Ooz ) > fOptInv(Interpol) ) || SOFT_gb_NoDepthTest)
                        {
                            pst_Pixel->f_Ooz = Interpol;
                            pst_Pixel->ul_ValueExt = SOFT_sgul_CurrentValueExt;
                        }
					}
				}
				else
				{
					if((SOFT_sgpst_CurrentPB->l_Added != -1)) SOFT_sgpst_CurrentPB->st_Query.l_Number--;
					SOFT_sgpst_CurrentPB->l_Added = -2;
				}
			}

			if( (fOptInv( pst_Pixel->f_Ooz ) > fOptInv(Interpol) ) || SOFT_gb_NoDepthTest)
			{
				pst_First->ul_Value = SOFT_sgul_CurrentValue;
				pst_First->ul_ValueExt = SOFT_sgul_CurrentValueExt;
				pst_First->f_Ooz = Interpol;
			}

			Interpol += PXInc->OoZ;
			pst_First++;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_PickingBuffer_Iso_RasterizeAndPickCross(ULONG Y, MAD_R_Raster *R, MAD_R_Vertex *PXInc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG							Xcounter, End;
	float							Interpol;
	SOFT_tdst_PickingBuffer_Pixel	*pst_First, *pst_Last, *pst_Pixel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((R->XEnd - R->P.X) > 0)
	{
		Xcounter = lFloatToLongOpt(R->P.X);
		End = lFloatToLongOpt(R->XEnd);

		pst_First = SOFT_sgpst_CurrentPB->dst_Pixel + Y * SOFT_sgpst_CurrentPB->l_Width + Xcounter;
		pst_Last = pst_First + End - Xcounter;

		Interpol = R->P.OoZ;

		while(pst_First < pst_Last)
		{
			if(pst_First->ul_SelMask)
			{
				if((SOFT_sgpst_CurrentPB->l_Added == -1))
				{
					if(SOFT_gb_NoDepthTest && !SOFT_gb_WindowSel) SOFT_PickingBuffer_QueryReinit(&SOFT_sgpst_CurrentPB->st_Query);
					SOFT_sgpst_CurrentPB->l_Added = SOFT_l_PickingBuffer_QueryAddInList
						(
							&SOFT_sgpst_CurrentPB->st_Query,
							SOFT_sgul_CurrentValue,
							SOFT_sgul_CurrentValueExt,
							Interpol
						);
				}
				else
				{
					pst_Pixel = SOFT_sgpst_CurrentPB->st_Query.dst_List + SOFT_sgpst_CurrentPB->l_Added;
					if( (fOptInv( pst_Pixel->f_Ooz ) > fOptInv(Interpol) ) || SOFT_gb_NoDepthTest)
                    {
                        pst_Pixel->f_Ooz = Interpol;
                        pst_Pixel->ul_ValueExt = SOFT_sgul_CurrentValueExt;
                    }
				}
			}

			if( (fOptInv( pst_Pixel->f_Ooz ) > fOptInv(Interpol) ) || SOFT_gb_NoDepthTest)
			{
				pst_First->ul_Value = SOFT_sgul_CurrentValue;
				pst_First->ul_ValueExt = SOFT_sgul_CurrentValueExt;
				pst_First->f_Ooz = Interpol;
			}

			Interpol += PXInc->OoZ;
			pst_First++;
		}
	}
}
#endif
