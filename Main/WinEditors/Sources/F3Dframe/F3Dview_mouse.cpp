/*$T F3Dview_mouse.cpp GC 1.134 04/22/04 13:47:32 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dview_undo.h"
#include "LINKs/LINKtoed.h"
#include "ENGine/Sources/ENGcall.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "DIAlogs/DIAgamematerial_dlg.h"
#include "DIAlogs/DIAgamematerial_inside.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "EDItors/Sources/EVEnts/EVEmsg.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAtoolbox_groview.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "SOFT/SOFThelper.h"
#include "SOFT/SOFTbackgroundimage.h"
#include "SELection/SELection.h"

#include "EDItors/Sources/OUTput/OUTframe.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/WORld/WORaccess.h"

/* TEST PREFAB */
#include "EDItors/Sources/PreFab/PFBframe.h"

#ifndef JADEFUSION
extern "C"
{
#endif
	extern BOOL ENG_gb_EngineRunning;
	extern SOFT_tdst_PickingBuffer *pPickForZCull;
	// -- Bug: modif temporaire --
	extern MATH_tdst_Vector mst_MoveBugLocal;
	// ---------------------------
#ifndef JADEFUSION
}
#endif
#define RECT(pDC, o) \
	{ \
		pDC->MoveTo(o.left, o.top); \
		pDC->LineTo(o.right, o.top); \
		pDC->LineTo(o.right, o.bottom); \
		pDC->LineTo(o.left, o.bottom); \
		pDC->LineTo(o.left, o.top); \
	}
extern void				EDI_Tooltip_DisplayMessage(char *, ULONG ulSpeed = 200);
extern EDI_cl_BaseFrame *gpo_AssociatedEditor;
#ifdef JADEFUSION
extern BOOL			SOFT_gb_ForceStartPicking;
#else
extern "C" BOOL			SOFT_gb_ForceStartPicking;
#endif
extern BOOL				LINK_gb_EditRefresh;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message)
{
	if(mb_GridEdit && GetAsyncKeyState('P') < 0)
	{
		::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PICK));
		return TRUE;
	}

	if(mb_GridEdit || M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_SubObject)
	{
		::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_SPEC));
		return TRUE;
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
#ifdef JADEFUSION
extern UCHAR *	guc_CapaSel;
extern UCHAR *	guc_CapaSel1;
extern int		gi_XCursor;
extern int		gi_YCursor;
#else
extern "C" UCHAR *	guc_CapaSel;
extern "C" UCHAR *	guc_CapaSel1;
extern "C" int		gi_XCursor;
extern "C" int		gi_YCursor;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::OnMouseMove(UINT nFlags, CPoint point)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	MATH_tdst_Vector	v1;
	RECT				st_Rect;
	MSG					msg;
	CDC					*pDC;
	CPoint				point1;
	CPen				o_Pen;
	CPen				*poldpen;
	BOOL				bClip;
	char				c_Refresh;
	float				fMemo;
	CRect				o_Win;
	int					iCXScreen, iCYScreen;
	EOUT_cl_Frame		*po_Out;
	float				f_Pick;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	nFlags = 0;
	if(GetAsyncKeyState(VK_LBUTTON) < 0) nFlags |= MK_LBUTTON;
	if(GetAsyncKeyState(VK_RBUTTON) < 0) nFlags |= MK_RBUTTON;
	if(GetAsyncKeyState(VK_MBUTTON) < 0) nFlags |= MK_MBUTTON;
	if(GetAsyncKeyState(VK_SHIFT) < 0) nFlags |= MK_SHIFT;
	if(GetAsyncKeyState(VK_CONTROL) < 0) nFlags |= MK_CONTROL;

	/* Curseur spécial */
	CWnd::OnMouseMove(nFlags, point);
	if(mst_WinHandles.pst_World == NULL)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		EDI_Tooltip_DisplayMessage
		(
			"No world loaded\nDRAG & DROP here a .WOR or a .WOL from the browser\nOr click with right button on the 3D view menu bar",
			5000
		);
		return;
	}

	/* Background image move / size */
	if(mi_BackgroundImageMove)
	{
		v.x = (point.x - mo_WhenLButtonDown.x) / M_F3D_DD->st_Camera.f_Width;
		v.y = (mo_WhenLButtonDown.y - point.y) / M_F3D_DD->st_Camera.f_Height;
		SOFT_BackgroundImage_MoveSize
		(
			M_F3D_DD->pst_BackgroundImage,
			mi_BackgroundImageMove,
			v.x,
			v.y,
			!(nFlags & MK_SHIFT)
		);
		mo_WhenLButtonDown = point;
		LINK_Refresh();
		return;
	}

	/* using static BV ? */
	mst_WinHandles.pst_DisplayData->uc_StaticBV = (GetAsyncKeyState('B') < 0);

	// JFP: Replacing the fabs call with abs, those ARE integers...
	if((abs(point.x - mo_WhenLButtonDown.x) > 4) || (abs(point.y - mo_WhenLButtonDown.y) > 4))
		mb_DepthPickEnable = FALSE;

	/* D & d mode */
	if(EDI_gst_DragDrop.b_BeginDragDrop)
	{
		if(nFlags & MK_LBUTTON) M_MF()->b_MoveDragDrop(point);
		return;
	}

	/* Link mode */
	if(mb_LinkOn)
	{
		DisplayObjectName(point, TRUE);
		LinkObjects(nFlags, point, FALSE);
		while(PeekMessage(&msg, GetSafeHwnd(), WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE));
		return;
	}

	/* sub object link mode */
	if(mb_SubObject_LinkOn)
	{
		Selection_SubObject_Link((POINT *) &point, FALSE, FALSE);
		while(PeekMessage(&msg, GetSafeHwnd(), WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE));
		return;
	}

	/* Grid */
	if(mb_GridEdit && (guc_CapaSel || guc_CapaSel1) && (nFlags & MK_SHIFT))
	{
		gi_XCursor = point.x;
		gi_YCursor = point.y;
		LINK_Refresh();
	}
	else
	{
		if((gi_XCursor != -1) || (gi_YCursor != -1))
		{
			gi_XCursor = -1;
			gi_YCursor = -1;
			LINK_Refresh();
		}
	}



	/* PREFAB */
	c_Refresh = 0;
	if(!Selection_b_IsInSubObjectMode() && (GetAsyncKeyState('P') < 0))
	{
		/*~~~~~~~~~~~~~~~~~~~~*/
		EPFB_cl_Frame	*po_PFB;
		char			c_Show;
		/*~~~~~~~~~~~~~~~~~~~~*/

		po_PFB = (EPFB_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_PREFAB, 0);
		c_Show = po_PFB->mst_Ini.i_DisplayPoint ? 1 : 0;
		c_Show += po_PFB->mst_Ini.i_DisplayBV ? 2 : 0;
		if(po_PFB && po_PFB->mpst_Prefab && c_Show)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	v;
			GDI_tdst_Device		*pst_Dev;
			CAM_tdst_Camera		*pst_Cam;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

			/* Pick to know pos */
			pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
			v.x = (float) point.x / (float) pst_Dev->l_Width;
			v.y = 1.0f - ((float) point.y / (float) pst_Dev->l_Height);
			b_PickDepth(&v, &v.z);
			pst_Cam = &mst_WinHandles.pst_DisplayData->st_Camera;
			v.x = (float) point.x;
			v.y = (float) point.y;
			CAM_2Dto3D(pst_Cam, &v, &v);

			MATH_CopyVector(&M_F3D_Helpers->st_PrefabPos, &v);
			po_PFB->mpst_Prefab->c_Show = c_Show;
			M_F3D_Helpers->pv_Prefab = (void *) po_PFB->mpst_Prefab;

			c_Refresh = 1;
		}
	}

	/* Weld + Snap when "X" is pressed, and sub object is active */
	{
		static ULONG LastWasOn = 0;
		static ULONG SavedVertexTool;
		static ULONG SavedSnapMode;
		static ULONG SubobjectMode;
		if	(Selection_b_IsInSubObjectMode())
		{
			if ( !mpo_ToolBoxDialog )
				Selection_ToolBox();
				
			if	((GetAsyncKeyState('X') < 0) && (!LastWasOn))
			{
				SubobjectMode = M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Mode;
				if (SubobjectMode != GRO_Cul_EOF_Vertex)
					((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnRadio_VertexMode();

				SavedSnapMode = M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_SnapVertex;
				SavedVertexTool = M_F3D_EditOpt->i_VertexTool;
				if (!(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_SnapVertex))
					((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnCheck_Snap();
				if (M_F3D_EditOpt->i_VertexTool != GRO_i_EOT_VertexWeld)
					((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnCheck_WeldVertex();//*/
				LastWasOn = 1;
			}

			if	((GetAsyncKeyState('X') >= 0) && (LastWasOn))
			{
				if (SubobjectMode == GRO_Cul_EOF_Edge)
					((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnRadio_EdgeMode();
				else
				if (SubobjectMode == GRO_Cul_EOF_Face)
					((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnRadio_FaceMode();

				if (!SavedSnapMode)
					((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnCheck_Snap();
				((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->EndCurTool();
				M_F3D_EditOpt->i_VertexTool = SavedVertexTool;
				((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->BeginCurTool();
				((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->UpdateControls();
				LastWasOn = 0;
			} 
		} else LastWasOn = 0;
	}

	/* Smooth Sell when "S" is pressed, and sub object is active */
	if (mpo_ToolBoxDialog != NULL)
	{
		static ULONG LastWasOn2 = 0;
		static ULONG LastWasOn3 = 0;
		static ULONG LastWasOn4 = 0;
		static ULONG SubobjectMode;
		if	(Selection_b_IsInSubObjectMode())
		{
			if ((GetAsyncKeyState(VK_CONTROL) >= 0) && (GetAsyncKeyState(VK_SHIFT) >= 0))
			{
				if	((GetAsyncKeyState('B') < 0) && (!LastWasOn3))
				{
					SubobjectMode = M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Mode;
					if (SubobjectMode == GRO_Cul_EOF_Vertex)
						((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnRadio_EdgeMode();
					else if (SubobjectMode == GRO_Cul_EOF_Edge)
						((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnRadio_FaceMode();
					else if (SubobjectMode == GRO_Cul_EOF_Face)
						((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnRadio_VertexMode();
					LastWasOn3 = 1;
				}
				if	(GetAsyncKeyState('B') >= 0)
				{
					LastWasOn3 = 0;
				}
				if	((GetAsyncKeyState('C') < 0) && (!LastWasOn4))
				{
					SubobjectMode = M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Mode;
					if (SubobjectMode == GRO_Cul_EOF_Edge)
					{
						if(M_F3D_EditOpt->i_EdgeTool == GRO_i_EOT_EdgeTurn)
							((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnCheck_CutEdge();
						else
							((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnCheck_TurnEdge();
						LastWasOn4 = 1;
					}
				}
				if	(GetAsyncKeyState('C') >= 0)
				{
					LastWasOn4 = 0;
				}
				if	((GetAsyncKeyState('S') < 0) && (!LastWasOn2))
				{
					SubobjectMode = M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Mode;
					if (SubobjectMode == GRO_Cul_EOF_Vertex)
						((EDIA_cl_ToolBox_GROView *) mpo_ToolBoxDialog->mpo_ToolView[0])->OnCheck_SmoothSel();
					LastWasOn2 = 1;
				}

				if	(GetAsyncKeyState('S') >= 0)
				{
					LastWasOn2 = 0;
				} 
			}
		} 
		else 
		{
			LastWasOn4 = 0;
			LastWasOn3 = 0;
			LastWasOn2 = 0;
		}
	}

	if(!c_Refresh)
	{
		if(M_F3D_Helpers->pv_Prefab)
		{
			M_F3D_Helpers->pv_Prefab = NULL;
			c_Refresh = 1;
		}
	}

	if(c_Refresh)
	{
		LINK_gb_EditRefresh = TRUE;
		Refresh();
		LINK_gb_EditRefresh = FALSE;
		return;
	}

	/* Tooltip : Display object name under mouse */
	if
	(
		!mb_ZoomOn
	&&	!mb_SelBoxOn
	&&	!IsActionValidate(F3D_Action_CameraOn)
	&&	!Helper_b_IsOneUnderMouse()
	&&	!Selection_b_IsInSubObjectMode()
	)
	{
		DisplayObjectName(point);
	}

	if(ENG_gb_EngineRunning) EDI_Tooltip_DisplayMessage("", 1);

	/* End of zoom */
	if(mb_ZoomOn)
	{
		if(!(IsActionValidate(F3D_Action_CameraOn)) || !(nFlags & MK_MBUTTON))
		{
			EndZoom(point);
			mb_WaitForMButton = TRUE;
		}
		else if((nFlags & MK_LBUTTON) || (nFlags & MK_RBUTTON))
		{
			EndZoom(point, FALSE);
			if(nFlags & MK_LBUTTON) mb_WaitForLButton = TRUE;
			if(nFlags & MK_RBUTTON) mb_WaitForRButton = TRUE;
			mb_WaitForMButton = TRUE;
		}
	}

	/* Wait for release buttons */
	if(mb_WaitForLButton)
	{
		if(nFlags & MK_LBUTTON)
			nFlags &= ~MK_LBUTTON;
		else
			mb_WaitForLButton = FALSE;
	}

	if(mb_WaitForMButton)
	{
		if(nFlags & MK_MBUTTON)
			nFlags &= ~MK_MBUTTON;
		else
			mb_WaitForMButton = FALSE;
	}

	if(mb_WaitForRButton)
	{
		if(nFlags & MK_RBUTTON)
			nFlags &= ~MK_RBUTTON;
		else
			mb_WaitForRButton = FALSE;
	}

	point1 = point;
	GetWindowRect(&st_Rect);
	ClientToScreen(&point);
	if(IsActionValidate(F3D_Action_CameraOn) || (Helper_b_IsInUse()))
	{
		if(!mb_Capture)
		{
			SetCapture();
			mb_Capture = TRUE;
			mi_MousePosX = point.x;
			mi_MousePosY = point.y;
			return;
		}
	}
	else
	{
		if(mb_Capture)
		{
			ReleaseCapture();
			mb_Capture = FALSE;
		}

		if((!mb_SelBoxOn) && (!(nFlags & MK_MBUTTON)))
		{
			c_Refresh = 0;

			if(Selection_b_IsInSubObjectMode())
			{

				/* cannot sel any sub object */
				if(mb_CanSelSomeSubObject)
				{
					mb_CanSelSomeSubObject = FALSE;
					Selection_SubObject_CannotSelAll();
					c_Refresh = 1;
				}

				/* is there a point under mouse */
				v.x = (float) point1.x;
				v.y = (float) M_F3D_PB->l_Height - point1.y;


				if (!(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_SelectVisible))
				if ((mst_WinHandles.pst_DisplayData->ul_WiredMode & 3) != 2)
				{
					if(mst_WinHandles.pst_World != NULL)
					{
						MATH_tdst_Vector	vL;
						vL.x = (float) point1.x / mst_WinHandles.pst_DisplayData->st_Device.l_Width;
						vL.y = 1.0f - ((float) point1.y / mst_WinHandles.pst_DisplayData->st_Device.l_Height);
						b_PickDepth(&vL, &vL.z);
					}
					pPickForZCull = mst_WinHandles.pst_DisplayData->pst_PickingBuffer;
				};


				if(Selection_b_SubObject_Pick(&v, 1))
				{
					c_Refresh = 1;
					mb_CanSelSomeSubObject = TRUE;
					if((nFlags & MK_LBUTTON) && (M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_ToolWhileMouseMove))
					{
						Selection_SubObject_Sel(0, 1);
						Selection_SubObjectTreat(&point1);
					}
				}

				pPickForZCull = NULL;
			}

			c_Refresh |= Helper_b_IsOneUnderMouse();
			Helper_i_Pick(point1, 1);
			if((c_Refresh) || (Helper_b_IsOneUnderMouse()))
			{
				LINK_gb_EditRefresh = TRUE;
				Refresh();
				LINK_gb_EditRefresh = FALSE;
				return;
			}
		}
	}

	if(IsActionValidate(F3D_Action_CameraOn) && (!Helper_b_IsInUse()))
	{
		if(!(nFlags & MK_LBUTTON)) mb_BeginModePane = FALSE;
		if(!(nFlags & MK_RBUTTON)) mb_BeginModeRotate = FALSE;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Perspective
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(IsActionValidate(F3D_Action_CameraOn1) && IsActionValidate(F3D_Action_CameraOn2))
		{
			if((nFlags & MK_LBUTTON) && (nFlags & MK_RBUTTON) && (!Helper_b_IsInUse()))
			{
				if(mst_WinHandles.pst_DisplayData->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
				{
					Cam_FocalZoom(point);

					/* Get out editor */
					po_Out = (EOUT_cl_Frame *) mpo_AssociatedEditor;
				}
			}
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Pane
		 ---------------------------------------------------------------------------------------------------------------
		 */

		else if((nFlags & MK_LBUTTON) && !(nFlags & MK_RBUTTON) && (!Helper_b_IsInUse()))
		{
			mb_AutoZoomExtendOnRotate = FALSE;
			TestLockCam();
			if(!mb_BeginModePane)
			{
				ScreenToClient(&point);
				v.x = ((float) point.x / mst_WinHandles.pst_DisplayData->st_Device.l_Width);
				v.y = 1.0f - ((float) point.y / mst_WinHandles.pst_DisplayData->st_Device.l_Height);
				b_PickDepth(&v, &mf_PickedDepth);
				mb_BeginModePane = TRUE;
				ClientToScreen(&point);
			}

			::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_CAMPANE));
			v.x = (float) (mi_MousePosX - point.x);
			v.y = (float) (mi_MousePosY - point.y);
			if(mst_WinHandles.pst_DisplayData->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
			{
				v.x *= mf_PickedDepth / mst_WinHandles.pst_DisplayData->st_Camera.f_FactorX;
				v.y *= mf_PickedDepth / mst_WinHandles.pst_DisplayData->st_Camera.f_FactorY;
				v.z = 0;
			}
			else
			{
				v.x /= mst_WinHandles.pst_DisplayData->st_Camera.f_FactorX;
				v.y /= mst_WinHandles.pst_DisplayData->st_Camera.f_FactorY;
				v.z = 0;
			}

			CAM_Move(&mst_WinHandles.pst_DisplayData->st_Camera, &v);
			LINK_Refresh();
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Camera rotate
		 ---------------------------------------------------------------------------------------------------------------
		 */

		else if((nFlags & MK_RBUTTON) && (!Helper_b_IsInUse()))
		{
			TestLockCam();
			if(GetAsyncKeyState(VK_LBUTTON) < 0)
			{
				v.z = (float) (mi_MousePosY - point.y) / 100.0f;
				v.y = 0;
				v.x = 0;

				ScreenToClient(&point);
				v1.x = ((float) point.x / mst_WinHandles.pst_DisplayData->st_Device.l_Width);
				v1.y = 1.0f - ((float) point.y / mst_WinHandles.pst_DisplayData->st_Device.l_Height);
				b_PickDepth(&v1, &f_Pick);
				if(f_Pick < 1.0) v.z /= (10 - f_Pick * 9);
				ClientToScreen(&point);
			}
			else
			{
				v.y = (float) (mi_MousePosY - point.y);
				v.z = 0;
				v.x = (float) (point.x - mi_MousePosX);
			}

			/* Doom rotate */
			if(IsActionValidate(F3D_Action_CameraOn1) && !IsActionValidate(F3D_Action_CameraOn2))
			{
				mb_AutoZoomExtendOnRotate = FALSE;
				::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_CAMTGT));
				v.x /= 100.0f;
				v.y /= 100.0f;
				v.x = -v.x;
				if(v.x)
					MATH_RotateMatrix_AroundGlobalZAxis(&mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix, v.x);
				if(v.y) MATH_RotateMatrix_AroundLocalXAxis(&mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix, v.y);
				LINK_Refresh();
			}

			/* Target rotate */
			else if(IsActionValidate(F3D_Action_CameraOn2))
			{
				::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_CAMTGT));
				fMemo = v.z;
				v.z = 0;
   				RotateCameraAroundTarget(&v);
				if(fMemo)
				{
					MATH_CopyVector(&v, &mst_WinHandles.pst_DisplayData->st_Camera.st_Target);
					MATH_SubVector(&v, &v, &mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix.T);
					MATH_NormalizeVector(&v, &v);
					MATH_ScaleVector(&v, &v, fMemo);
					MATH_VectorGlobalToLocal(&v, &mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix, &v);

					if(mst_WinHandles.pst_DisplayData->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
						CAM_Move(&mst_WinHandles.pst_DisplayData->st_Camera, &v);
					else
					{
						MoveCamera(&v, 1);
					}
				}

				if(mb_AutoZoomExtendOnRotate)
				{
					ZoomExtendSelected
					(
						&mst_WinHandles.pst_DisplayData->st_Camera,
						mst_WinHandles.pst_DisplayData->pst_World
					);
				}

				LINK_Refresh();
			}
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Zoom
		 ---------------------------------------------------------------------------------------------------------------
		 */

		else if((nFlags & MK_MBUTTON) && (!Helper_b_IsInUse()))
		{
			TestLockCam();

			/* Constant zoom */
			if(IsActionValidate(F3D_Action_CameraOn1))
			{
				::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_CAMZOOM1));
				v.x = v.y = 0;
				v.z = ((float) mi_MousePosY - point.y) * 0.5f;
				CAM_Move(&mst_WinHandles.pst_DisplayData->st_Camera, &v);
				LINK_Refresh();
			}

			/* Rect zoom */
			else
			{
				if(!mb_ZoomOn)
				{
					mb_ZoomOn = TRUE;
					mo_ZoomRect.left = point1.x;
					mo_ZoomRect.top = point1.y;
					mo_ZoomRect.right = point1.x;
					mo_ZoomRect.bottom = point1.y;
				}

				::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_CAMZOOM));
				pDC = GetDC();
				pDC->SetROP2(R2_XORPEN);
#ifdef JADEFUSION
				o_Pen.CreatePen(PS_SOLID, 1, 0x00ffffff);
#else
				o_Pen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_ACTIVECAPTION));
#endif
				poldpen = pDC->SelectObject(&o_Pen);
				RECT(pDC, mo_ZoomRect);
				mo_ZoomRect.right = point1.x;
				mo_ZoomRect.bottom = point1.y;
				RECT(pDC, mo_ZoomRect);
				pDC->SelectObject(poldpen);
				DeleteObject(&o_Pen);
				ReleaseDC(pDC);
			}
		}
		else
		{
			::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_CAM));
		}
	}
	else
	{
		mb_BeginModePane = FALSE;
		mb_BeginModeRotate = FALSE;
		mf_PickedDepth = 0.0f;

		if(Helper_b_IsInUse())
		{
            if (nFlags & MK_LBUTTON)
    			Helper_Treat(point1);
		}
		else
		{
			if(nFlags & MK_MBUTTON)
			{
				if(!mb_SelBoxOn)
				{
					SetCapture();
					mb_Capture = TRUE;
					mb_SelBoxOn = TRUE;
					mo_ZoomRect.left = point1.x;
					mo_ZoomRect.top = point1.y;
					mo_ZoomRect.right = point1.x;
					mo_ZoomRect.bottom = point1.y;
				}

				pDC = GetDC();
				pDC->SetROP2(R2_XORPEN);
#ifdef JADEFUSION
				o_Pen.CreatePen(PS_SOLID, 1, 0x00ffffff);
#else
				o_Pen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_ACTIVECAPTION));
#endif
				poldpen = pDC->SelectObject(&o_Pen);
				RECT(pDC, mo_ZoomRect);
				mo_ZoomRect.right = point1.x;
				mo_ZoomRect.bottom = point1.y;
				RECT(pDC, mo_ZoomRect);
				pDC->SelectObject(poldpen);
				DeleteObject(&o_Pen);
				ReleaseDC(pDC);
			}
			else
			{
				if(mb_SelBoxOn)
				{
					ReleaseCapture();
					mb_Capture = FALSE;
					Selection_EndBox(point, TRUE, (nFlags & MK_CONTROL), (nFlags & MK_SHIFT));
				}
			}
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Window clipping for mouse
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(mb_Capture && !Helper_b_IsInUse())
	{
		bClip = FALSE;
		o_Win = st_Rect;
		iCXScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		iCYScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		if(st_Rect.left < GetSystemMetrics(SM_XVIRTUALSCREEN)) st_Rect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
		if(st_Rect.top < GetSystemMetrics(SM_YVIRTUALSCREEN)) st_Rect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
		if(st_Rect.right > iCXScreen) st_Rect.right = iCXScreen - 1;
		if(st_Rect.bottom > iCYScreen) st_Rect.bottom = iCYScreen - 1;
		if(!mb_ZoomOn && (point.x < st_Rect.left + 2))
		{
			point.x = st_Rect.right - 2;
			SetCursorPos(point.x, point.y);
			bClip = TRUE;
		}
		else if(!mb_ZoomOn && (point.x > st_Rect.right - 2))
		{
			point.x = st_Rect.left + 2;
			SetCursorPos(point.x, point.y);
			bClip = TRUE;
		}

		if(!mb_ZoomOn && (point.y < st_Rect.top + 2))
		{
			point.y = st_Rect.bottom - 2;
			SetCursorPos(point.x, point.y);
			bClip = TRUE;
		}
		else if(!mb_ZoomOn && (point.y > st_Rect.bottom - 2))
		{
			point.y = st_Rect.top + 2;
			SetCursorPos(point.x, point.y);
			bClip = TRUE;
		}

		/* Delete mouse move */
		if(bClip)
		{
			while(PeekMessage(&msg, GetSafeHwnd(), WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE));
		}

		mi_MousePosX = point.x;
		mi_MousePosY = point.y;
	}

	/* Grid edit */
	if(mb_GridEdit)
	{
		if(nFlags & MK_LBUTTON)
		{
			if(Grid_b_Paint(point1.x, point1.y, 0)) LINK_Refresh();
		}
		else if(nFlags & MK_RBUTTON)
		{
			if(Grid_b_Paint(point1.x, point1.y, 1)) LINK_Refresh();
		}

		return;
	}
}

/*
 =======================================================================================================================
    Aim: If we have previously selected a Gizmo, we dont want to enable the Depth Picking.
 =======================================================================================================================
 */
BOOL F3D_DisableDepthPickingIfNeeded(SEL_tdst_Selection *_pst_Sel, BOOL *_pb_EnableDepthPick)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(SEL_b_IsEmpty(_pst_Sel)) return FALSE;
	if(SEL_l_CountItem(_pst_Sel, SEL_C_SIF_All) > 1) return FALSE;
	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(_pst_Sel, SEL_C_SIF_Object);
	if
	(
		pst_GO
	&&	(
			(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)
		||	(
				(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
			&&	(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowGizmo)
			)
		)
	)
	{
		*_pb_EnableDepthPick = FALSE;
		return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::OnLButtonDown(UINT nFlags, CPoint point)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL							bRefresh, b_ForceRefresh;
	struct WOR_tdst_World_			*pst_World;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	SOFT_tdst_PickingBuffer			*pst_PB;
	SOFT_tdst_PickingBuffer_Pixel	*pst_Pixel;
	long							l_Type, l_Param, l_Tolerance;
	UCHAR							uc_Type;
	void							*p_Data;
	BOOL							b_Xor, b_Neg;
	int								i;
	OBJ_tdst_GameObject				*pst_GO;
	OBJ_tdst_GameObject				*pst_Father;
	bool							b_SplitScreen = false;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	/* Check for world */
    pst_World = mst_WinHandles.pst_DisplayData->pst_World;
	if(!pst_World)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		goto endOnLButtonDown;
	}

	if (pst_World->b_IsSplitScreen) {
		CRect   o_Rect;

		WOR_View_UnsplitView (pst_World);

		// We take into account clicks in the left view only but we register click in the right view
		mpo_Frame->mpo_Parent->GetClientRect(o_Rect);
		mb_LastClickInRightView = (point.x > o_Rect.Width() / 2);
		point.x *= 2;
		point.y = 2*point.y - o_Rect.Height()/2;
		b_SplitScreen = true;
	}

	mpst_BonePickObj = NULL;
	if(mpo_AssociatedEditor) mpo_AssociatedEditor->GetParent()->SetFocus();

	mo_WhenLButtonDown = point;
	CWnd::OnLButtonDown(nFlags, point);

	/* Exit if camera mode */
	if(IsActionValidate(F3D_Action_CameraOn)) goto endOnLButtonDown;

	/* New picking */
	b_Neg = (nFlags & MK_SHIFT);
	b_Xor = (nFlags & MK_CONTROL) || b_Neg;

	Grid_ResetSel();

	if(!Selection_b_IsInSubObjectMode())
	{
		/* test add prefab */
		if(GetAsyncKeyState('P') < 0)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EPFB_cl_Frame	*po_PFBeditor;
			EOUT_cl_Frame	*po_Out;
			/*~~~~~~~~~~~~~~~~~~~~*/

			po_Out = (EOUT_cl_Frame *) mpo_AssociatedEditor;
			po_PFBeditor = (EPFB_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_PREFAB, 0);
			if((po_PFBeditor) && (po_PFBeditor->mul_FileIndex != BIG_C_InvalidIndex))
			{
				/*~~~~~~~~~~~~~~~~~~~*/
				/* Prefab_ul_Save( po_PFBeditor->mpst_Prefab ); */
				EDI_tdst_DragDrop	DD;
				/*~~~~~~~~~~~~~~~~~~~*/

				DD.o_Pt = point;
				ClientToScreen(&DD.o_Pt);
				DD.ul_FatFile = po_PFBeditor->mul_FileIndex;
				DropPrefab(&DD);
				LINK_Refresh();
			}

			// "Select prefab" coche a la creation d'un nouveau prefab
			po_Out->mst_Ini.ul_SelFlags |= EOUT_SelFlags_Prefab;
			
			goto endOnLButtonDown;
		};

		/* Test add waypoint mode */
		if(IsActionValidate(F3D_Action_AddWaypoint))
		{
			if(mpo_AssociatedEditor)
			{
				mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DisplayWaypoint;
				CreateWaypoint(point);
				////Deselect object selected
				////SEL_EnumItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object, F3D_b_Undo_Unselect, (ULONG) this, 0);
				//SEL_tdst_SelectedItem* pst_GO = mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
				//if(pst_GO)
				//	Selection_Unselect(pst_GO->p_Content, SEL_C_SIF_Object);

				////select new WP and rename it
				//Selection_Object(CreateWaypoint(point), SEL_C_SIF_Object);
				//Rename();
				LINK_Refresh();
			}

			goto endOnLButtonDown;
		}

		/* Test add invisible object mode */
		if(IsActionValidate(F3D_Action_AddInvisible))
		{
			if(mpo_AssociatedEditor)
			{
				mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_DisplayInvisible;
				GAO_CreateInvisible(point);
				LINK_Refresh();
			}

			goto endOnLButtonDown;
		}

		/* Test link objects mode */
		if(IsActionValidate(F3D_Action_Link))
		{
			mb_HierarchicalLink = FALSE;
			LinkObjects(MK_LBUTTON, point, TRUE);
			goto endOnLButtonDown;
		}

		/* Test hierarchical link objects mode */
		if(IsActionValidate(F3D_Action_HierarchicalLink))
		{
			mb_HierarchicalLink = TRUE;
			LinkObjects(MK_LBUTTON, point, TRUE);
			goto endOnLButtonDown;
		}

		/* Test for grid picking */
		if(mb_GridEdit)
		{
			if(Grid_b_Paint(point.x, point.y, 0)) LINK_Refresh();
			goto endOnLButtonDown;
		}
	}

	/* Check if helper is active */
	if(Helper_b_IsOneUnderMouse())
	{
		if(Selection_b_IsInSubObjectMode(TRUE))
		{
			Helper_Use();

			if
			(
				(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Edge)
			&&	(Helper_b_IsInTranslateMode())
			&&	(GetAsyncKeyState('P') < 0)
			) Selection_ExtrudeEdge();
			else if
				(
					(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Face)
				&&	(Helper_b_IsInTranslateMode())
				&&	(GetAsyncKeyState('P') < 0)
				)
				Selection_SubObject_ExtrudeFace();
			else if
				(
					(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Face)
				&&	(Helper_b_IsInTranslateMode())
				&&	(GetAsyncKeyState('O') < 0)
				)
				Selection_SubObject_DuplicateFace();
			else
			{
				/* mpo_CurrentModif = new F3D_cl_Undo_HelperModif(this, TRUE); */
				mpo_CurrentModif = new F3D_cl_Undo_MultipleGeoModif(this);
				mpo_CurrentModif->SetDesc("Move vertex");
				Selection_SubObject_StartMovingVertex(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_VertexMoveUpdateUV);
			}
		}
		else
		{
			if((!(nFlags & MK_CONTROL)) && (GetAsyncKeyState('O') < 0) && (Helper_b_IsInTranslateMode()))
			{
				mb_DuplicateOn = TRUE;
				Selection_GAO_Duplicate(NULL, TRUE,NULL,NULL,NULL);
			}
			// -- DuplicateClone --
			if((!(nFlags & MK_CONTROL)) && (GetAsyncKeyState('L') < 0) && (Helper_b_IsInTranslateMode()))
			{
				Selection_CLONE_Duplicate();
			}
			
			if(mi_FreezeHelper != -1)
			{
				SOFT_gb_ForceStartPicking = TRUE;
				Helper_i_Pick(point, 0);
				SOFT_gb_ForceStartPicking = FALSE;
			}

			Helper_Use();

			if
			(
				(M_F3D_Helpers->l_Picked >= SOFT_Cl_Helpers_Skeleton1)
			&&	(M_F3D_Helpers->l_Picked <= SOFT_Cl_Helpers_Skeleton4)
			)
			{
				pst_GO = M_F3D_Helpers->ast_Skeleton[M_F3D_Helpers->l_Picked - SOFT_Cl_Helpers_Skeleton1].pst_GO;
				l_Param = M_F3D_Helpers->ast_Skeleton[M_F3D_Helpers->l_Picked - SOFT_Cl_Helpers_Skeleton1].l_GizmoPicked;

				if(pst_GO && pst_GO->pst_Base && pst_GO->pst_Base->pst_AddMatrix)
				{
					if(l_Param < pst_GO->pst_Base->pst_AddMatrix->l_Number)
					{
						pst_GO = pst_GO->pst_Base->pst_AddMatrix->dpst_EditionGO[l_Param];
						mpst_BonePickObj = pst_GO;
						if(Selection_b_Treat(pst_GO, 0, SEL_C_SIF_Object, b_Xor)) LINK_Refresh();
					}
				}

				M_F3D_Helpers->l_Picked = -1;
			}
			else
			{
				mpo_CurrentModif = new F3D_cl_Undo_HelperModif(this, FALSE);
				mpo_CurrentModif->SetDesc("Move object");
			}
		}

		goto endOnLButtonDown;
	}

	mi_EditedCurve = -1;
	if(mpst_EditedPortal)
	{
		mpst_EditedPortal->uw_Flags &= ~WOR_CF_PortalPicked;
		mpst_EditedPortal = NULL;
	}

	/* Test if selection is locked */
	if(Selection_b_IsLocked()) goto endOnLButtonDown;

	if(Selection_b_IsInSubObjectMode())
	{
		if(!(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_ToolAcceptMultiple)) b_Xor = b_Neg = FALSE;

		l_Tolerance = 4;

		if(Selection_b_SubObject_TreatAnyway(&point)) goto endOnLButtonDown;

		if(mb_CanSelSomeSubObject)
		{
			if(mb_LButtonDblclk)
			{
				if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Vertex)
				{
					Selection_SubObject_VertexPos();
					goto endOnLButtonDown;
				}

				if(M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Face)
				{
					Selection_SubObject_FaceOrder();
					goto endOnLButtonDown;
				}
			}

			if((nFlags & MK_SHIFT) && (nFlags & MK_CONTROL))
			{
				if
				(
					M_F3D_Helpers->b_CenterLock_On
				&&	(mpst_LockCenter_GAO == M_F3D_Helpers->pst_CenterLock_GAO)
				&&	(mi_LockCenter_Point == M_F3D_Helpers->i_CenterLock_Point)
				) M_F3D_Helpers->b_CenterLock_On = 0;
				else
				{
					M_F3D_Helpers->b_CenterLock_On = 1;
					M_F3D_Helpers->pst_CenterLock_GAO = mpst_LockCenter_GAO;
					M_F3D_Helpers->i_CenterLock_Point = mi_LockCenter_Point;
				}
			}
			else
			{
				Selection_SubObject_Sel(b_Xor ? 1 : 0, b_Neg ? 0 : 1);
				Selection_SubObjectTreat(&point);
				if((M_F3D_EditOpt->ul_Flags & GRO_Cul_EOF_Vertex) && mpo_VertexPos)
					Selection_SubObject_VertexPos();
			}

			LINK_Refresh();
			goto endOnLButtonDown;
		}
		else
		{
			if(M_F3D_Helpers->b_CenterLock_On)
			{
				M_F3D_Helpers->b_CenterLock_On = 0;
				LINK_Refresh();
				goto endOnLButtonDown;
			}
		}
	}
	else
	{
		l_Tolerance = 0;
	}

	if(mst_WinHandles.pst_DisplayData->uc_DrawPostIt && PostItPickMouse()) goto endOnLButtonDown;

	b_ForceRefresh = F3D_DisableDepthPickingIfNeeded(M_F3D_Sel, &mb_DepthPickEnable);
	if(Pick_l_UnderPoint(&point, SOFT_Cuc_PBQF_All, l_Tolerance) == 0)
	{
		if(Selection_b_IsInSubObjectMode()) goto endOnLButtonDown;
		bRefresh = Selection_b_Treat(NULL, 0, 0, b_Xor);
		mi_BackgroundImageMove = SOFT_i_BackgroundImage_Pick(M_F3D_DD->pst_BackgroundImage, point.x, point.y, M_F3D_DD);

		if(mpo_GameMaterialDialog && mpo_GameMaterialDialog->mpo_View->Link3D_On)
			mpo_GameMaterialDialog->mpo_View->UpdateGameMaterial( NULL, 0);

	}
	else
	{
		pst_PB = mst_WinHandles.pst_DisplayData->pst_PickingBuffer;
		pst_Query = &pst_PB->st_Query;

		if(b_Xor)
		{
			pst_Pixel = pst_Query->dst_List;
			for(i = 0; i < pst_Query->l_Number; i++)
			{
				if(pst_Query->dst_List[i].ul_SelMask == 1)
				{
					pst_Pixel = pst_Query->dst_List + i;
					break;
				}
			}
		}
		else
		{
			pst_Pixel = pst_Query->dst_List;
			if(mb_DepthPickEnable)
			{
				for(i = 0; i < pst_Query->l_Number - 1; i++)
				{
					if(pst_Query->dst_List[i].ul_SelMask == 1)
					{
						pst_Pixel = pst_Query->dst_List + i + 1;
						break;
					}
				}
			}
		}

		mb_DepthPickEnable = TRUE;

		l_Type = pst_Pixel->ul_ValueExt & SOFT_Cul_PBQF_TypeMask;
		switch(l_Type)
		{
		case SOFT_Cuc_PBQF_GameObject:
			p_Data = (void *) pst_Pixel->ul_Value;
			pst_GO = (OBJ_tdst_GameObject *) p_Data;

			if(Selection_b_IsInSubObjectMode() && (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected)) goto endOnLButtonDown;

			if
			(
				(SEL_l_CountItem(M_F3D_Sel, SEL_C_SIF_All) == 1)
			&&	ANI_b_IsGizmoAnim(pst_GO, &pst_Father)
			&&	SEL_RetrieveItem(M_F3D_Sel, p_Data)
			)
			{
				M_MF()->SendMessageToLinks
					(
						mpo_AssociatedEditor,
						EEVE_MESSAGE_SELECTALLBONES,
						(ULONG) pst_Father,
						(ULONG) pst_GO
					);
			}
			else
			{
				l_Param = pst_Pixel->ul_ValueExt;
				bRefresh = Selection_b_Treat(p_Data, l_Param, SEL_C_SIF_Object, b_Xor);

				/* Drag drop */
				EDI_gst_DragDrop.ul_FatDir = BIG_C_InvalidIndex;
				EDI_gst_DragDrop.ul_FatFile = BIG_C_InvalidIndex;
				EDI_gst_DragDrop.i_Param2 = (int) p_Data;
				M_MF()->BeginDragDrop(point, this, mpo_AssociatedEditor, EDI_DD_Data);
				ClientToScreen(&point);
				EDI_gst_DragDrop.o_OutRect = CRect(point.x - 32, point.y - 32, point.x + 32, point.y + 32);
			}
			break;

		case SOFT_Cuc_PBQF_Link:
			p_Data = (void *) ((WAY_tdst_GraphicLink *) pst_Pixel->ul_Value)->pst_Link;
			l_Param = (long) ((WAY_tdst_GraphicLink *) pst_Pixel->ul_Value)->pst_Origin;
			bRefresh = Selection_b_Treat(p_Data, l_Param, SEL_C_SIF_Link, b_Xor);
			break;

		case SOFT_Cuc_PBQF_HieLink:
			p_Data = (void *) pst_Pixel->ul_Value;
			l_Param = 0;
			bRefresh = Selection_b_Treat(p_Data, l_Param, SEL_C_SIF_HieLink, b_Xor);
			break;

		case SOFT_Cuc_PBQF_Zone:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				GEO_tdst_GraphicZone	*pst_GZ;
				int						e;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_GZ = mst_WinHandles.pst_DisplayData->st_DisplayedZones.dpst_GraphicZone + (ULONG) ((GEO_tdst_GraphicZone *) pst_Pixel->ul_Value);
				p_Data = (void *) pst_GZ->pv_Data;
				l_Param = (long) pst_GZ->pst_GO;
				uc_Type = pst_GZ->uc_Type;

				switch(uc_Type)
				{
				case GEO_Cul_GraphicZDx:
					bRefresh = Selection_b_Treat(p_Data, l_Param, SEL_C_SIF_ZDx, b_Xor);

					/* Drag drop */
					EDI_gst_DragDrop.ul_FatDir = BIG_C_InvalidIndex;
					EDI_gst_DragDrop.ul_FatFile = BIG_C_InvalidIndex;
					EDI_gst_DragDrop.i_Param2 = (int) p_Data;
					M_MF()->BeginDragDrop(point, this, mpo_AssociatedEditor, EDI_DD_Zone);
					ClientToScreen(&point);
					EDI_gst_DragDrop.o_OutRect = CRect(point.x - 32, point.y - 32, point.x + 32, point.y + 32);
					if(mpo_GameMaterialDialog && mpo_GameMaterialDialog->mpo_View->Link3D_On)
						mpo_GameMaterialDialog->mpo_View->UpdateGameMaterial( NULL, 0);
					break;

				case GEO_Cul_GraphicCob:
					e = (pst_Pixel->ul_ValueExt & SOFT_Cul_PBQF_ElementMask) >> SOFT_Cul_PBQF_ElementShift;
					((COL_tdst_Cob *) p_Data)->ul_EditedElement = e;
					if(mpo_GameMaterialDialog && mpo_GameMaterialDialog->mpo_View->Link3D_On)
					{
						if(((COL_tdst_Cob *) p_Data)->uc_Type == COL_C_Zone_Triangles)
						{
							mpo_GameMaterialDialog->mpo_View->UpdateGameMaterial
								(
									((COL_tdst_Cob *) p_Data)->pst_GMatList,
									((COL_tdst_Cob *) p_Data)->pst_TriangleCob->dst_Element[e].l_MaterialId
								);
						}
						else
						{
							mpo_GameMaterialDialog->mpo_View->UpdateGameMaterial
								(
									((COL_tdst_Cob *) p_Data)->pst_GMatList,
									((COL_tdst_Cob *) p_Data)->pst_MathCob->l_MaterialId
								);
						}
					}

					bRefresh = Selection_b_Treat(p_Data, l_Param, SEL_C_SIF_Cob, b_Xor);
					break;

				default:
					if(mpo_GameMaterialDialog && mpo_GameMaterialDialog->mpo_View->Link3D_On)
						mpo_GameMaterialDialog->mpo_View->UpdateGameMaterial( NULL, 0);
					break;

				}
			}
			break;
		}
		if(l_Type != SOFT_Cuc_PBQF_Zone)
		{
			if(mpo_GameMaterialDialog && mpo_GameMaterialDialog->mpo_View->Link3D_On)
           				mpo_GameMaterialDialog->mpo_View->UpdateGameMaterial( NULL, 0);
		}

	}


	
	
	if (b_SplitScreen) {
		WOR_View_SplitView (pst_World);
	}
	/* Refresh eventually display and pointers */
	if(bRefresh || b_ForceRefresh)
	{
		LINK_Refresh();
		LINK_UpdatePointers();
		if(mpo_AssociatedEditor) mpo_AssociatedEditor->RefreshMenu();
		if(mpo_AssociatedEditor) mpo_AssociatedEditor->SetFocus();
	}
	return;

endOnLButtonDown:
	if (b_SplitScreen) {
		WOR_View_SplitView (pst_World);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	mb_LButtonDblclk = TRUE;
	OnLButtonDown(nFlags, point);
	mb_LButtonDblclk = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::OnLButtonUp(UINT nFlags, CPoint point)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char							sz_Text[100];
	BOOL							b_Refresh;
	struct WOR_tdst_World_			*pst_World;
	bool							b_SplitScreen = false;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Check for world */
	pst_World = mst_WinHandles.pst_DisplayData->pst_World;
	if(!pst_World)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		goto endOnLButtonUp;
	}
	if (pst_World->b_IsSplitScreen) {
		CRect   o_Rect;

		WOR_View_UnsplitView (pst_World);

		// We take into account clicks in the left view only
		mpo_Frame->mpo_Parent->GetClientRect(o_Rect);
		point.x *= 2;
		point.y = 2*point.y - o_Rect.Height()/2;
		b_SplitScreen = true;
	}

	CWnd::OnLButtonUp(nFlags, point);

	mb_PaintMode = FALSE;

	/* background image */
	if(mi_BackgroundImageMove)
	{
		mi_BackgroundImageMove = 0;
		goto endOnLButtonUp;
	}

	/* Link mode */
	if(mb_LinkOn)
	{
		LinkObjects(nFlags, point, FALSE);
		goto endOnLButtonUp;
	}

	if(mb_SubObject_LinkOn)
	{
		mb_SubObject_LinkOn = FALSE;
		goto endOnLButtonUp;
	}

	/* Helper mode */
	if(Helper_b_IsInUse())
	{
		if(Selection_b_IsInSubObjectMode())
		{
			if(!Selection_b_SubObjectTreat_OnLButtonUp(&point))
			{
				if(mpo_CurrentModif) mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
			}
#if defined(_XENON_RENDER)
            Selection_XenonPostOperation(M_F3D_Sel, FALSE, TRUE);//popoverif
#endif

		}
		else if(Helper_l_GetDescription(sz_Text))
		{
			mpo_CurrentModif->SetDesc(sz_Text);
			mo_UndoManager.b_AskFor(mpo_CurrentModif, FALSE);
		}
		else
			delete mpo_CurrentModif;
		mpo_CurrentModif = NULL;
		MATH_InitVectorToZero(&mst_Move);
		// -- Bug: modif temporaire --
		MATH_InitVectorToZero(&mst_MoveBugLocal);
		// ---------------------------
		Helper_EndUse();

		/*
		if(mb_DuplicateOn)
		{
			mb_DuplicateOn = FALSE;
			Selection_GAO_Duplicate(NULL, TRUE,NULL,NULL,NULL);
		}
		*/

		LINK_Refresh();
		goto endOnLButtonUp;
	}

	/* End of a drag & drop operation */
	if(EDI_gst_DragDrop.b_BeginDragDrop)
	{
		b_Refresh = EDI_gst_DragDrop.b_CanDragDrop;
		M_MF()->EndDragDrop(point);
		if(b_Refresh) LINK_Refresh();
	}

endOnLButtonUp:
	if (b_SplitScreen) {
		WOR_View_SplitView (pst_World);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::OnRButtonDown(UINT nFlags, CPoint point)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char							sz_Text[100];
	struct WOR_tdst_World_			*pst_World;
	bool							b_SplitScreen = false;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Check for world */
	pst_World = mst_WinHandles.pst_DisplayData->pst_World;
	if(!pst_World)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		goto endOnRButtonDown;
	}
	if (pst_World->b_IsSplitScreen) {
		CRect   o_Rect;

		WOR_View_UnsplitView (pst_World);

		// We take into account clicks in the left view only
		mpo_Frame->mpo_Parent->GetClientRect(o_Rect);
		point.x *= 2;
		point.y = 2*point.y - o_Rect.Height()/2;
		b_SplitScreen = true;
	}

	if(mpo_AssociatedEditor) mpo_AssociatedEditor->GetParent()->SetFocus();
	CWnd::OnRButtonDown(nFlags, point);
	if(mst_WinHandles.pst_World == NULL) goto endOnRButtonDown;

	/* End of zoom */
	if(mb_ZoomOn)
	{
		EndZoom(point, FALSE);
		mb_WaitForRButton = TRUE;
		mb_WaitForMButton = TRUE;
		goto endOnRButtonDown;
	}

	/* End of zoom */
	if(mb_SelBoxOn)
	{
		Selection_EndBox(point, FALSE, FALSE, FALSE);
		mb_WaitForRButton = TRUE;
		mb_WaitForLButton = TRUE;
		goto endOnRButtonDown;
	}

	/* End of network creation */
	if(mb_LinkOn)
	{
		LinkObjects(MK_LBUTTON | MK_RBUTTON, point, FALSE);
		goto endOnRButtonDown;
	}

	if(mb_SubObject_LinkOn)
	{
		Selection_SubObject_Link((POINT *) &point, FALSE, TRUE);
		goto endOnRButtonDown;
	}

	/* End of selection move/rotate */
	if(mb_Capture && Helper_b_IsInUse())
	{
		if(!Selection_b_IsInSubObjectMode()) Helper_l_GetDescription(sz_Text);

		if(mpo_CurrentModif)
		{
			mpo_CurrentModif->b_Undo();
			delete mpo_CurrentModif;
			mpo_CurrentModif = NULL;
		}

		MATH_InitVectorToZero(&mst_Move);
		// -- Bug: modif temporaire --
		MATH_InitVectorToZero(&mst_MoveBugLocal);
		// ---------------------------
		mb_WaitForLButton = TRUE;
		Helper_Reset();
		LINK_Refresh();
		goto endOnRButtonDown;
	}

	/* Exit if moving camera */
	if(IsActionValidate(F3D_Action_CameraOn)) goto endOnRButtonDown;

	/* Cancel drag & drop */
	if(EDI_gst_DragDrop.b_BeginDragDrop)
	{
		M_MF()->CancelDragDrop();
		goto endOnRButtonDown;
	}

	/* Grid edit */
	if(mb_GridEdit)
	{
		if(Grid_b_Paint(point.x, point.y, 1))
		{
			LINK_Refresh();
		}

		goto endOnRButtonDown;
	}

	/* Else transmit RButton info to parent editor */
	if(mpo_AssociatedEditor)
	{
		ClientToScreen(&point);
		mpo_AssociatedEditor->ScreenToClient(&point);
		mpo_AssociatedEditor->SendMessage(WM_RBUTTONDOWN, nFlags, point.x + (point.y << 16));
	}

endOnRButtonDown:
	if (b_SplitScreen) {
		WOR_View_SplitView (pst_World);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::OnMButtonDown(UINT nFlags, CPoint point)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	RECT				st_Rect;
	MATH_tdst_Vector	v;
	struct WOR_tdst_World_			*pst_World;
	bool							b_SplitScreen = false;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Check for world */
	pst_World = mst_WinHandles.pst_DisplayData->pst_World;
	if(!pst_World)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		goto endOnMButtonDown;
	}
	if (pst_World->b_IsSplitScreen) {
		CRect   o_Rect;

		WOR_View_UnsplitView (pst_World);

		// We take into account clicks in the left view only
		mpo_Frame->mpo_Parent->GetClientRect(o_Rect);
		point.x *= 2;
		point.y = 2*point.y - o_Rect.Height()/2;
		b_SplitScreen = true;
	}

	CWnd::OnMButtonDown(nFlags, point);
	if(mst_WinHandles.pst_World == NULL)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		goto endOnMButtonDown;
	}

	/* End of network creation */
	if(mb_LinkOn)
	{
		LinkObjects(MK_LBUTTON | MK_MBUTTON, point, FALSE);
		goto endOnMButtonDown;
	}

	if(!Selection_b_IsInSubObjectMode() && (GetAsyncKeyState('P') < 0))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EPFB_cl_Frame	*po_PFBeditor;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		po_PFBeditor = (EPFB_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_PREFAB, 0);
		if(po_PFBeditor && po_PFBeditor->mpst_Prefab)
		{
			po_PFBeditor->mpst_Prefab->f_Rotate = 0;
			LINK_Refresh();
			goto endOnMButtonDown;
		}
	}

	/* Center pick point */
	if(IsActionValidate(F3D_Action_CameraOn1))
	{
		TestLockCam();
		GetClientRect(&st_Rect);
		st_Rect.right >>= 1;
		st_Rect.bottom >>= 1;
		v.x = ((float) point.x / mst_WinHandles.pst_DisplayData->st_Device.l_Width);
		v.y = 1.0f - ((float) point.y / mst_WinHandles.pst_DisplayData->st_Device.l_Height);
		b_PickDepth(&v, &mf_PickedDepth);
		v.x = (float) point.x - st_Rect.right;
		v.y = (float) point.y - st_Rect.bottom;

		/* MODIF_Leroy 11/5/00 */
		if(mst_WinHandles.pst_DisplayData->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
		{
			v.x *= mf_PickedDepth / mst_WinHandles.pst_DisplayData->st_Camera.f_FactorX;
			v.y *= mf_PickedDepth / mst_WinHandles.pst_DisplayData->st_Camera.f_FactorY;
			v.z = 0;
		}
		else
		{
			v.x /= mst_WinHandles.pst_DisplayData->st_Camera.f_FactorX;
			v.y /= mst_WinHandles.pst_DisplayData->st_Camera.f_FactorY;
			v.z = 0;
		}

		CAM_Move(&mst_WinHandles.pst_DisplayData->st_Camera, &v);
		LINK_Refresh();
	}

	/* Test for grid picking */
	if(mb_GridEdit)
	{
		/*~~~~~~~~~~~~~~*/
		MSG		msg;
		CPoint	pt, pt1;
		int		tmp, i, j;
		int		what;
		/*~~~~~~~~~~~~~~*/

		GetCursorPos(&pt1);
		ScreenToClient(&pt1);
		what = 0;
		if(GetAsyncKeyState(VK_CONTROL) < 0) what = 1;
		while(1)
		{
			GetMessage(&msg, 0, 0, 0);
			if(msg.message == WM_MBUTTONUP) break;
			if(msg.message != WM_MOUSEMOVE) continue;
			GetCursorPos(&pt);
			ScreenToClient(&pt);

			if(pt.x < pt1.x)
			{
				tmp = pt.x;
				pt.x = pt1.x;
				pt1.x = tmp;
			}

			if(pt.y < pt1.y)
			{
				tmp = pt.y;
				pt.y = pt1.y;
				pt1.y = tmp;
			}

			for(i = pt1.x; i < pt.x; i++)
			{
				for(j = pt1.y; j < pt.y; j++)
				{
					Grid_b_Paint(i, j, what);
				}
			}

			LINK_Refresh();
		}

		goto endOnMButtonDown;
	}

endOnMButtonDown:
	if (b_SplitScreen) {
		WOR_View_SplitView (pst_World);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::OnMButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnMButtonUp(nFlags, point);
	if(mst_WinHandles.pst_DisplayData->uc_EngineCamera) return;
	if(mst_WinHandles.pst_DisplayData->uc_EditorCamera) return;
	if(mb_ZoomOn) EndZoom(point);
	if(mb_SelBoxOn) Selection_EndBox(point, TRUE, (nFlags & MK_CONTROL), (nFlags & MK_SHIFT));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	RECT				st_Rect;
	EOUT_cl_Frame		*po_Out;
	float				f_Rot;
	struct WOR_tdst_World_			*pst_World;
	bool							b_SplitScreen = false;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Check for world */
	pst_World = mst_WinHandles.pst_DisplayData->pst_World;
	if(!pst_World)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		goto endOnMouseWheel;
	}
	if (pst_World->b_IsSplitScreen) {
		CRect   o_Rect;

		WOR_View_UnsplitView (pst_World);

		// We take into account clicks in the left view only
		mpo_Frame->mpo_Parent->GetClientRect(o_Rect);
		pt.x *= 2;
		pt.y = 2*pt.y - o_Rect.Height()/2;
		b_SplitScreen = true;
	}

	if(!Selection_b_IsInSubObjectMode() && (GetAsyncKeyState('P') < 0))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EPFB_cl_Frame	*po_PFBeditor;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		po_PFBeditor = (EPFB_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_PREFAB, 0);
		if(po_PFBeditor && po_PFBeditor->mpst_Prefab)
		{
			if(M_F3D_Helpers->ul_Flags & SOFT_Cul_HF_SnapAngle)
				f_Rot = M_F3D_Helpers->f_AngleSnap;
			else
				f_Rot = Cf_PiBy8;
			po_PFBeditor->mpst_Prefab->f_Rotate += (zDelta < 0) ? -f_Rot : f_Rot;
			//LINK_Refresh();
			goto endOnMouseWheel;
		}
	}

	/* Camera move */
	if(IsActionValidate(F3D_Action_CameraOn))
	{
		if(mpo_AssociatedEditor) mpo_AssociatedEditor->GetParent()->SetFocus();

		TestLockCam();
		
		if (
				(GetAsyncKeyState( VK_SHIFT ) < 0 )
				&& (M_F3D_Helpers->st_Center.z > 0.1)
				&& (M_F3D_Helpers->st_Center2D.x > 0)
				&& (M_F3D_Helpers->st_Center2D.x < mst_WinHandles.pst_DisplayData->st_Device.l_Width)
				&& (M_F3D_Helpers->st_Center2D.y > 0)
				&& (M_F3D_Helpers->st_Center2D.y < mst_WinHandles.pst_DisplayData->st_Device.l_Height)
			)
		{
			MATH_CopyVector( &v, &M_F3D_Helpers->st_Center2D );
			pt.x = (LONG) v.x;
			pt.y = mst_WinHandles.pst_DisplayData->st_Device.l_Height - (LONG) v.y;
			v.z = M_F3D_Helpers->st_Center.z;
			if( v.z < 1.0f ) v.z = 1.0f;
			if (zDelta < 0) *(unsigned long *) &v.z |= 0x80000000;
			v.z *= (float) ml_MouseWheelFactor / 120.0f;
		}
		else
		{
			v.x = (float) pt.x / mst_WinHandles.pst_DisplayData->st_Device.l_Width;
			v.y = 1.0f - ((float) pt.y / mst_WinHandles.pst_DisplayData->st_Device.l_Height);
			if(b_PickDepth(&v, &v.z) && (v.z < 1.0f)) v.z = 1.0f;
			if(zDelta < 0) * (unsigned long *) &v.z |= 0x80000000;
			v.z *= (float) ml_MouseWheelFactor / 120.0f;
			if(GetAsyncKeyState(VK_MENU) >= 0)
			{
				if(v.z > 0 && v.z < 0.5) v.z = 1.0;
				if(v.z < 0 && v.z > -0.5) v.z = -1.0;
			}
		}

		if(mb_ConstantZoomSpeed) {
			if(v.z > 0) v.z =  (float)mi_ZoomSpeed;
			else        v.z = -(float)mi_ZoomSpeed;
		}

		GetClientRect(&st_Rect);
		v.x = (float) (pt.x - (st_Rect.right >> 1));
		v.y = (float) (pt.y - (st_Rect.bottom >> 1));

  		po_Out = (EOUT_cl_Frame *) mpo_AssociatedEditor;
		if(po_Out->mst_Ini.f_CamWheelFactor < 0.05f) po_Out->mst_Ini.f_CamWheelFactor = 0.05f;
		MATH_ScaleVector(&v, &v, po_Out->mst_Ini.f_CamWheelFactor);

		MoveCamera(&v, 0);
		ClientToScreen(&pt);
		mi_MousePosX = pt.x;
		mi_MousePosY = pt.y;
	}

	/* Object move */
	else
	{
		if
		(
			(!Selection_pst_GetFirstObject())
		&&	(!SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_ZDx))
		&&	(!SEL_pst_GetFirstItem(mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Cob))
		&&	!mpst_EditedPortal
		) goto endOnMouseWheel;

		if(GetAsyncKeyState(VK_SHIFT) < 0)
		{
			SEL_Circle(mst_WinHandles.pst_World->pst_Selection, (zDelta > 0) ? 1 : 0);
			//LINK_Refresh();
		}
		else
			Helper_ChangeMode(zDelta > 0);
	}

endOnMouseWheel:
	if (b_SplitScreen) {
		WOR_View_SplitView (pst_World);
	}
	LINK_Refresh();
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_View::PickPoint3D(MATH_tdst_Vector *v, OBJ_tdst_GameObject **pobj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MSG					msg;
	CPoint				pt;
	MATH_tdst_Vector	vpos;
	BOOL				ok;
	GDI_tdst_Device		*pst_Dev;
	CAM_tdst_Camera		*pst_Cam;
	CRect				rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Picking position */
	ok = FALSE;
	SetCapture();
	::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PICK));
	while(1)
	{
		::GetMessage(&msg, NULL, 0, 0);
		if(msg.message == WM_RBUTTONDOWN)
		{
			ReleaseCapture();
			return FALSE;
		}

		if(msg.message == WM_LBUTTONDOWN)
		{
			/* Pick to know pos */
			GetCursorPos(&pt);
			GetClientRect(&rect);
			ScreenToClient(&pt);
			if(!rect.PtInRect(pt)) break;
			pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
			vpos.x = (float) pt.x / (float) pst_Dev->l_Width;
			vpos.y = 1.0f - ((float) pt.y / (float) pst_Dev->l_Height);
			b_PickDepth(&vpos, &vpos.z);
			pst_Cam = &mst_WinHandles.pst_DisplayData->st_Camera;
			vpos.x = (float) pt.x;
			vpos.y = (float) pt.y;
			CAM_2Dto3D(pst_Cam, &vpos, &vpos);
			MATH_CopyVector(v, &vpos);
			ok = TRUE;
			if(pobj) *pobj = 0;
			break;
		}
	}

	ReleaseCapture();
	return TRUE;
}
#endif /* ACTIVE_EDITORS */
