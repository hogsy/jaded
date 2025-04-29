/*$T F3DView_pick.cpp GC! 1.086 07/10/00 16:00:14 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDIapp.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dstrings.h"
#include "F3Dframe/F3Dview_undo.h"
#include "CAMera/CAMera.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "INOut/INOkeyboard.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/ENGcall.h"
#include "ENGine/Sources/WORld/WORrender.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "GEOmetric/GEOdebugobject.h"
#include "GEOmetric/GEOsubobject.h"
#include "LIGHT/LIGHTstruct.h"
#include "GraphicDK/Sources/CAMera/CAMstruct.h"
#include "SOFT/SOFThelper.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "SELection/SELection.h"
#include "Res/Res.h"
#include "BASe/MEMory/MEM.h"
#include "EDIpaths.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "MATHs/MATH.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "DIAlogs/DIAselection_dlg.h"

/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */

#define M_pst_PB()	(mst_WinHandles.pst_DisplayData->pst_PickingBuffer)
#define M_pst_PQ()	(&mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query)

extern BOOL SOFT_gb_WindowSel;
/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Pick_PointW2H(MATH_tdst_Vector *v, POINT *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_Device *pst_Dev;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	v->x = (float) p->x / pst_Dev->l_Width;
	v->y = 1.0f - ((float) p->y / pst_Dev->l_Height);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::Pick_PointH2PB(POINT *p, MATH_tdst_Vector *v)
{
	p->x = (int) (v->x * M_pst_PB()->l_Width);
	p->y = (int) (v->y * M_pst_PB()->l_Height);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
long F3D_cl_View::Pick_l_UnderPoint(POINT *_po_Point, long _l_Filter, long _l_Tolerance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CPoint							pt;
	SOFT_tdst_PickingBuffer_Pixel	st_SwapPixel, *Pixeli, *Pixelj;
	int								i, j;
	long							l_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mb_LockPickObj) return (ULONG) mpst_LockPickObj;

	Pick_PointW2H(&M_pst_PQ()->st_Point1, _po_Point);
	M_pst_PQ()->l_Filter = _l_Filter;
	M_pst_PQ()->l_Tolerance = _l_Tolerance;
	M_pst_PB()->ul_Flags = SOFT_Cul_PBF_PickWhileRender;

	Pick_PointH2PB(&pt, &M_pst_PQ()->st_Point1);
	SOFT_SetClipWindow(M_pst_PB(), pt.x - 10, pt.y - 10, pt.x + 10, pt.y + 10);

	mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DoNotRender;
	mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_UsePickingBuffer;
	Refresh();
	mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_UsePickingBuffer;

	SOFT_ResetClipWindow(M_pst_PB());

	/* Sort list of result by depth */
	Pixeli = M_pst_PQ()->dst_List;
	for(i = 0; i < M_pst_PQ()->l_Number; i++, Pixeli++)
	{
		l_Type = Pixeli->ul_ValueExt & SOFT_Cul_PBQF_TypeMask;
		Pixeli->ul_SelMask = Selection_b_IsSelected((void *) Pixeli->ul_Value, l_Type);

		Pixelj = M_pst_PQ()->dst_List;
		for(j = 0; j < i; j++, Pixelj++)
		{
			//if(Pixelj->f_Ooz < Pixeli->f_Ooz)
            if( fOptInv(Pixelj->f_Ooz) > fOptInv(Pixeli->f_Ooz) )
			{
				st_SwapPixel = *Pixeli;
				L_memmove(Pixelj + 1, Pixelj, (char *) Pixeli - (char *) Pixelj);
				*Pixelj = st_SwapPixel;
				break;
			}
		}
	}

	return M_pst_PQ()->l_Number;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
long F3D_cl_View::Pick_l_InBox(POINT *_po_Point1, POINT *_po_Point2, long _l_Filter)
{
	/*~~~~~~~~~~~~~*/
	CPoint	pt1, pt2;
	float	temp;
	/*~~~~~~~~~~~~~*/

	if(mb_LockPickObj) return (ULONG) mpst_LockPickObj;

	Pick_PointW2H(&M_pst_PQ()->st_Point1, _po_Point1);
	Pick_PointW2H(&M_pst_PQ()->st_Point2, _po_Point2);

	if(M_pst_PQ()->st_Point1.x > M_pst_PQ()->st_Point2.x)
	{
		temp = M_pst_PQ()->st_Point1.x;
		M_pst_PQ()->st_Point1.x = M_pst_PQ()->st_Point2.x;
		M_pst_PQ()->st_Point2.x = temp;
	}

	if(M_pst_PQ()->st_Point1.y > M_pst_PQ()->st_Point2.y)
	{
		temp = M_pst_PQ()->st_Point1.y;
		M_pst_PQ()->st_Point1.y = M_pst_PQ()->st_Point2.y;
		M_pst_PQ()->st_Point2.y = temp;
	}

	M_pst_PB()->ul_Flags = SOFT_Cul_PBF_BoxSelection;
	M_pst_PB()->ul_Flags |= SOFT_Cul_PBF_PickWhileRender;
	M_pst_PB()->ul_Flags |= M_pst_PB()->ul_UserFlags;

	M_pst_PQ()->l_Filter = _l_Filter;
	M_pst_PQ()->l_Tolerance = 0;

	Pick_PointH2PB(&pt1, &M_pst_PQ()->st_Point1);
	Pick_PointH2PB(&pt2, &M_pst_PQ()->st_Point2);
	SOFT_SetClipWindow(M_pst_PB(), pt1.x - 10, pt1.y - 10, pt2.x + 10, pt2.y + 10);

	mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DoNotRender;
	mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_UsePickingBuffer;
	SOFT_gb_WindowSel = TRUE;
	Refresh();
	SOFT_gb_WindowSel = FALSE;
	mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_UsePickingBuffer;

	SOFT_ResetClipWindow(M_pst_PB());

	return M_pst_PQ()->l_Number;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SOFT_tdst_PickingBuffer_Pixel *F3D_cl_View::Pick_pst_GetFirst(ULONG _ul_Filter, long _l_Selected)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_PickingBuffer_Pixel	*Pixel, *LastPixel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Pixel = M_pst_PQ()->dst_List;
	LastPixel = Pixel + M_pst_PQ()->l_Number;
	for(; Pixel < LastPixel; Pixel++)
	{
		if((_ul_Filter == 0) || ((Pixel->ul_ValueExt & SOFT_Cul_PBQF_TypeMask) == _ul_Filter))
		{
			if((_l_Selected == -1) || ((ULONG) _l_Selected == Pixel->ul_SelMask)) return Pixel;
		}
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SOFT_tdst_PickingBuffer_Pixel *F3D_cl_View::Pick_pst_GetNext(ULONG _ul_Filter, long _l_Selected, SOFT_tdst_PickingBuffer_Pixel *_pst_After )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SOFT_tdst_PickingBuffer_Pixel	*Pixel, *LastPixel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Pixel = M_pst_PQ()->dst_List;
	LastPixel = Pixel + M_pst_PQ()->l_Number;
    _pst_After++;

    if ( (_pst_After >= LastPixel) || (_pst_After < Pixel) ) return NULL;

    for(Pixel = _pst_After; Pixel < LastPixel; Pixel++)
	{
		if((_ul_Filter == 0) || ((Pixel->ul_ValueExt & SOFT_Cul_PBQF_TypeMask) == _ul_Filter))
		{
			if((_l_Selected == -1) || ((ULONG) _l_Selected == Pixel->ul_SelMask)) return Pixel;
		}
	}
	return NULL;
}


#endif /* ACTIVE_EDITORS */

