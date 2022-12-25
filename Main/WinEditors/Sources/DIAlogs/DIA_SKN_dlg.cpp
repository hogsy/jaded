/*$T DIA_SKN_dlg.cpp GC! 1.081 05/16/00 16:17:46 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "Engine/sources/OBJects/OBJgizmo.h"
#include "Engine/sources/OBJects/OBJslowaccess.h"
#include "Engine/sources/OBJects/OBJorient.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "GEOmetric/GEOobjectaccess.h"
#include "GEOmetric/GEODebugObject.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEO_MRM.h"
#include "GEOmetric/GEO_SKIN.h"
#include "GEOmetric/GEOstaticLOD.h"

#include "SOFT/SOFTpickingbuffer.h"
#include "DIAlogs/DIA_SKN_dlg.h"
#include "DIAlogs/DIAtoolbox_groview.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "LINks/LINKmsg.h"
#include "LINks/LINKtoed.h"
#include "LINks/LINKstruct.h"

extern void EDI_Tooltip_DisplayMessage(char *, ULONG ulSpeed = 200);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_SKN_Dialog, EDIA_cl_BaseDialog)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_DESTROY()
END_MESSAGE_MAP()
#define ITM_Pshed	1

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

static char DIASKN_FunctionNames[][32] =
{
	"PAINT",
	"Smooth",
	"R<->G",
	"Rst Gzm",
	"ADD",
	"Contrast",
	"G->R<-B",
	"Flsh All",
	"SUB",
	"Sel. Red",
	"<-R->",
	"UNDO",
	"Get Sym X",
	"Nearest",
	"tresh Min",
	"REDO",
	"\n\nL",
	"180°",
	"Pick",
    ""
};

static char DIASKN_FunctionInfo[][200] =
{
	"Select RED component of current color.\nRight button 100% - left button adjust.\nMouse wheele for change gizmo",
	"Select GREEN component of current color.\nRight button 100% - left button adjust.\nMouse wheele for change gizmo",
	"Select BLUE component of current color.\nRight button 100% - left button adjust.\nMouse wheele for change gizmo",
	"Select STRAIGH.\nLeft button for adjust.",
	"Paint will affect the 3 colors\non the selected vertices",
	"Smooth the selected vertices\nUse straight for adjust",
	"Exchange Red Gizmo with green gizmo\non selected vertices",
	"Restore gizmo",
	"Add the current color\nto selected vertices",
	"Contrast the selected vertices",
	"Insert Red gizmo between blue and\ngreen gizmo on selection\nUse straight for adjust",
	"Flash All gizmo",
	"Substract the current color\nto selected vertices.\nSub with a gizmo with 100% will remove it",
	"This will select the red vertice.\nUse straight for the threshold.\nCtrl will keep other selection",
	"Expand Red gizmo on selection.\nUse straight for adjust",
	"UNDO.\n Tu veut un dessin? ",
	"Affect to selected vertices\nthe ponderation symetrised\nfrom unselected vertices.\n!!THIS WILL ALSO FLASH ALL MATRIXES.\n!!THIS CAN ALSO TAKE A LONG TIME.\nBones must be aligned along Z Axis.",
	"Affect to selected vertices\nthe ponderation computed from\nthe nearest bone.\n!!THIS WILL ALSO FLASH ALL MATRIXES",
	"This will erase ponderation with weight less than behind straight bar.\n This will be limited to 49.9%",
	"REDO.\n ",
	"LOCK GIZMO.\nGizmo are not update\n when selection change....",
	"Rotate red flashed gizmo of 180°\naround bone",
    "Pick ponderation from another skin\nPush button, move mouse above object to pick in associated 3DView\nand release mouse button",
	""
};

#define MOVERECT(rect, a, b) \
	(rect)->top += b; \
	(rect)->right += a; \
	(rect)->left += a; \
	(rect)->bottom += b;
#define INFLATERECT(rect, a, b) \
	(rect)->top += b; \
	(rect)->right -= a; \
	(rect)->left += a; \
	(rect)->bottom -= b;
#define DIA_SKN_MinValue			0.0001f
#define ITEMFILLCOLOR()				GetSysColor(COLOR_3DFACE)
#define ITEMLINECOLOR()				ITEMFILLCOLOR() /* GetSysColor(COLOR_3DFACE) - ((GetSysColor(COLOR_3DFACE) &
													 * 0xfcfcfcfc) >> 2) */
#define ITEMTEXTCOLOR()				((GetSysColor(COLOR_3DFACE) & 0xf0f0f0f0) >> 2)
#define ITEMTEXTCOLORDisable()		(((GetSysColor(COLOR_3DFACE) & 0xf0f0f0f0) >> 1) +  ITEMTEXTCOLOR())
#define ITEMTEXTGIZMOCOLOR()		0xcfcf
#define ITEMTEXTGIZMOCOLOR_HILI()	0xffffff
#define ITEMGIZMOLINECOLOR()		GetSysColor(COLOR_3DFACE)
#define ITEMHELPCOLOR()				0xffff

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DIA_SKN_SelctionCLNK(GEO_tdst_Object *pst_Object, void *ClassPtr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_SKN_Dialog	*ClSKN;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	ClSKN = (EDIA_cl_SKN_Dialog *) ClassPtr;
	ClSKN->UpdateSelection();
	ClSKN->UpdateWindow();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG IsInList(ULONG NewIndex, ULONG *p_3Index)
{
	/*~~~~~~~~~~~~*/
	ULONG	Counter;
	/*~~~~~~~~~~~~*/

	Counter = 3;
	while(Counter--)
		if(p_3Index[Counter] == NewIndex) return Counter;
	return 0xFFFFFFFF;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SKN_Dialog::UpdateSelection(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	LONG	Counter, Counter2;
	ULONG	t3Gizs[6];
	float	t3Values[6];
	RECT	LocalRect;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	NumberOfSelectedPoints = 0;
	t3Gizs[3] = Gizmo[0].GizmoIndex;
	t3Gizs[4] = Gizmo[1].GizmoIndex;
	t3Gizs[5] = Gizmo[2].GizmoIndex;
	WasUpdateBySel = 1;
	for(ULONG ulNOG = 0; ulNOG < ul_NumberOfGEOM; ulNOG++)
	{
		GEO_SKN_SetNumberOfMatrix(mpst_Skn_Gro[ulNOG], GetGizmoNumber());
		NumberOfSelectedPoints += GEO_SKN_GetInfoAboutSelected
			(
				mpst_Skn_Gro[ulNOG],
				t3Gizs,
				t3Values,
				SKN_Cul_UseSubSel
			);
	}

	if(GizmoAreLocked)
	{
		Gizmo[0].fValue = t3Values[3];
		Gizmo[1].fValue = t3Values[4];
		Gizmo[2].fValue = t3Values[5];
		Counter = 3;
		while(Counter--)
		{
			if(Gizmo[Counter].fValue < DIA_SKN_MinValue) Gizmo[Counter].fValue = DIA_SKN_MinValue;
			GetDlgItem(Gizmo[Counter].Item)->GetWindowRect(&LocalRect);
			ScreenToClient(&LocalRect);
			InvalidateRect(&LocalRect);
		}
	}
	else
	{
		Counter = 3;
		while(Counter--)
		{
			if(IsInList(Gizmo[Counter].GizmoIndex, t3Gizs) != 0xFFFFFFFF)
			{
				/* Actualize Value */
				Gizmo[Counter].fValue = t3Values[IsInList(Gizmo[Counter].GizmoIndex, t3Gizs)];

				/* Erase from new list */
				t3Gizs[IsInList(Gizmo[Counter].GizmoIndex, t3Gizs)] = 0xFFFFFFFF;
			}
			else
				/* Ready for new Gizmo */
				Gizmo[Counter].GizmoIndex |= 0x80000000;
		}

		/* Existant Gizmo are reactualized (chinese head broker 1) */
		Counter = 3;
		Counter2 = 0;
		while(Counter--)
		{
			if(Gizmo[Counter].GizmoIndex & 0x80000000)
			{
				while(t3Gizs[Counter2] == 0xFFFFFFFF) Counter2++;
				if(t3Values[Counter2] >= DIA_SKN_MinValue)
				{
					Gizmo[Counter].fValue = t3Values[Counter2];
					Gizmo[Counter].GizmoIndex = t3Gizs[Counter2];
				}
				else
				{
					Gizmo[Counter].fValue = DIA_SKN_MinValue;
					Gizmo[Counter].GizmoIndex &= 0x7fffffff;
				}

				Counter2++;
			}

			GetDlgItem(Gizmo[Counter].Item)->GetWindowRect(&LocalRect);
			ScreenToClient(&LocalRect);
			InvalidateRect(&LocalRect);
		}
	}

	/* New Gizmo are inserted (chinese head broker 2) */
	if(mpst_Skn_Gro)
	{
		for(ULONG ulNOG = 0; ulNOG < ul_NumberOfGEOM; ulNOG++)
		{
			GEO_SKN_SetNumberOfMatrix(mpst_Skn_Gro[ulNOG], GetGizmoNumber());
			GEO_SKN_Skin2Colors(mpst_Skn_Gro[ulNOG], Gizmo[0].GizmoIndex, Gizmo[1].GizmoIndex, Gizmo[2].GizmoIndex, 0);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SKN_Dialog::EDIA_cl_SKN_Dialog
(
	char			*_psz_Title,
	unsigned long	*ColorExt,
	void (*RefrechClbk) (unsigned long, unsigned long),
	unsigned long USRPARAM1,
	unsigned long USRPARAM2,
	struct OBJ_tdst_GameObject_ *mpst_Gao,
	struct GRO_tdst_Struct_ *mpst_Gro
) :
	EDIA_cl_BaseDialog(IDD_SKN_DLG)
{
	/*~~~~~~~~~~~~*/
	ULONG	Counter;
	/*~~~~~~~~~~~~*/

	mo_Title = "Edit sking of " + (CString) _psz_Title;
	NumberOfSelectedPoints = 0;
	UnderMouseItem = 0;
	UnderMouseItemIndex = 0;
	bLocked = 0;
	bMousebIsDown = 0;
	bRMousebIsDown = 0;
	TimeDBCLK = 0;
	GizmoAreLocked = 0;
	mpst_Skn_Gao = mpst_Gao;
	ul_NumberOfGEOM = 1;
	mpst_Skn_Gro[0] = (GEO_tdst_Object *) mpst_Gro;
	if (mpst_Gro->i->ul_Type == GRO_GeoStaticLOD)
			mpst_Skn_Gro[0] = (GEO_tdst_Object *)((GEO_tdst_StaticLOD *)mpst_Gro)->dpst_Id[0];

	Gizmo[0].fValue = 0.5f;
	Gizmo[1].fValue = 0.6f;
	Gizmo[2].fValue = 0.5f;
	Gizmo[3].fValue = 1.f;
	Gizmo[0].GizmoIndex = 0;
	Gizmo[1].GizmoIndex = 0;
	Gizmo[2].GizmoIndex = 0;
	Gizmo[3].GizmoIndex = 8;
	Gizmo[0].Item = IDC_GIZMORED;
	Gizmo[1].Item = IDC_GIZMOGREEN;
	Gizmo[2].Item = IDC_GIZMOBLUE;
	Gizmo[3].Item = IDC_GIZMOBLEND;
	AllItemsNum = 0;
	AllItem[AllItemsNum++] = IDC_GIZMORED;
	AllItem[AllItemsNum++] = IDC_GIZMOGREEN;
	AllItem[AllItemsNum++] = IDC_GIZMOBLUE;
	AllItem[AllItemsNum++] = IDC_GIZMOBLEND;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH2;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH3;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH4;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH5;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH6;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH7;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH8;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH9;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH10;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH11;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH12;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH17;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH18;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH19;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH20;
	AllItem[AllItemsNum] = IDC_SKN_LOCKED;
	GizmoAreLockedIndex = AllItemsNum++;
	AllItem[AllItemsNum++] = IDC_SKN_SMOOTH13;
    AllItem[AllItemsNum++] = IDC_SKN_SMOOTH14;
	Counter = AllItemsNum;
	while(Counter--)
	{
		AllItemStates[Counter] = 0;
	}

	if(mpst_Gro)
	{
		GEO_tdst_Object *p_Geom;
		p_Geom = (GEO_tdst_Object *)mpst_Gro;
		if (mpst_Gro->i->ul_Type == GRO_GeoStaticLOD)
			p_Geom = (GEO_tdst_Object *)((GEO_tdst_StaticLOD *)mpst_Gro)->dpst_Id[0];

		GEO_SKN_SetNumberOfMatrix((GEO_tdst_Object *) p_Geom, GetGizmoNumber());
		GEO_SKN_Skin2Colors
		(
			(GEO_tdst_Object *) p_Geom,
			Gizmo[0].GizmoIndex,
			Gizmo[1].GizmoIndex,
			Gizmo[2].GizmoIndex,
			0
		);
	}

	Gizmo[0].GizmoIndex = 0;
	Gizmo[1].GizmoIndex = 1 % GetGizmoNumber();
	Gizmo[2].GizmoIndex = 2 % GetGizmoNumber();

    mpo_GroView = NULL;
}

/*
 =======================================================================================================================
    GIZMO IO
 =======================================================================================================================
 */
LONG EDIA_cl_SKN_Dialog::GetGizmoNumber(void)
{
	if(!(mpst_Skn_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)) return 0;

	return mpst_Skn_Gao->pst_Base->pst_AddMatrix->l_Number;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *EDIA_cl_SKN_Dialog::GetGizmoName(ULONG Num)
{
	/*~~~~~~~~~~~~~~~~*/
	ULONG	ul_MatrixID;
    OBJ_tdst_GameObject st_Obj;
	/*~~~~~~~~~~~~~~~~*/

	if(!(mpst_Skn_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)) return "";

    L_memcpy( &st_Obj, mpst_Skn_Gao, sizeof( OBJ_tdst_GameObject ) );

	if(mpst_Skn_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
	{
        if (Num >= (ULONG) mpst_Skn_Gao->pst_Base->pst_AddMatrix->l_Number)
            return "";
        if( !mpst_Skn_Gao->pst_Base->pst_AddMatrix->dst_GizmoPtr[Num].pst_GO )
            return "";

		ul_MatrixID = mpst_Skn_Gao->pst_Base->pst_AddMatrix->dst_GizmoPtr[Num].l_MatrixId;
		if(ul_MatrixID == 0xffffffff)
			return mpst_Skn_Gao->pst_Base->pst_AddMatrix->dst_GizmoPtr[Num].pst_GO->sz_Name;
		else
		{
			if(mpst_Skn_Gao->pst_Base->pst_AddMatrix->dst_GizmoPtr[Num].pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix) 
			{
				if (mpst_Skn_Gao->pst_Base->pst_AddMatrix->dst_GizmoPtr[Num].pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer) 
				    return mpst_Skn_Gao->pst_Base->pst_AddMatrix->dst_GizmoPtr[Num].pst_GO->sz_Name;
				else
				    return mpst_Skn_Gao->pst_Base->pst_AddMatrix->dst_GizmoPtr[Num].pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[ul_MatrixID].sz_Name;
			}
			else
			{
				return mpst_Skn_Gao->pst_Base->pst_AddMatrix->dst_GizmoPtr[Num].pst_GO->sz_Name;
			}
        }
	}
	else
		return mpst_Skn_Gao->pst_Base->pst_AddMatrix->dst_Gizmo[Num].sz_Name;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SKN_Dialog::~EDIA_cl_SKN_Dialog(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SKN_Dialog::ComputeCurrentColor(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float	ColorNorlzd[3], Norm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CurrentColor = 0;
	ColorNorlzd[0] = Gizmo[0].fValue;
	ColorNorlzd[1] = Gizmo[1].fValue;
	ColorNorlzd[2] = Gizmo[2].fValue;
	Norm = ColorNorlzd[0] * ColorNorlzd[0] + ColorNorlzd[1] * ColorNorlzd[1] + ColorNorlzd[2] * ColorNorlzd[2];
	Norm = (float) sqrt(Norm);
	Norm = 1.0f / Norm;
	ColorNorlzd[0] *= Norm;
	ColorNorlzd[1] *= Norm;
	ColorNorlzd[2] *= Norm;

	CurrentColor |= (ULONG) (ColorNorlzd[0] * 255.0f) << 0;
	CurrentColor |= (ULONG) (ColorNorlzd[1] * 255.0f) << 8;
	CurrentColor |= (ULONG) (ColorNorlzd[2] * 255.0f) << 16;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EDIA_cl_SKN_Dialog::GetItemFromPoint(void)
{
	/*~~~~~~~~~~~~*/
	ULONG	Counter;
	/*~~~~~~~~~~~~*/

	if(bLocked) return UnderMouseItem;
	UnderMouseItem = 0;
	UnderMouseItemIndex = 0;
	for(Counter = 0; Counter < AllItemsNum; Counter++)
	{
		if(IF_IS_IN(AllItem[Counter]))
		{
			UnderMouseItem = AllItem[Counter];
			UnderMouseItemIndex = Counter;
		}
	}

	return UnderMouseItem;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SKN_Dialog::IF_IS_IN(int ITEM)
{
	/*~~~~~~~~~~~~~~~*/
	RECT	LocalRect;
	POINT	MousePoint;
	/*~~~~~~~~~~~~~~~*/

	GetCursorPos(&MousePoint);
	if(ITEM != -1)
		GetDlgItem(ITEM)->GetWindowRect(&LocalRect);
	else
	{
		GetClientRect(&LocalRect);
		ClientToScreen(&LocalRect);
	}

	if
	(
		(MousePoint.y >= LocalRect.top)
	&&	(MousePoint.y <= LocalRect.bottom)
	&&	(MousePoint.x >= LocalRect.left)
	&&	(MousePoint.x <= LocalRect.right)
	) return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SKN_Dialog::OnInitDialog(void)
{
	SetWindowText(mo_Title);

    if (mst_Position.x == 0x7FFFFFFF)
	    CenterWindow(AfxGetMainWnd());
    else
        SetWindowPos( NULL, mst_Position.x, mst_Position.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE );

	for(ULONG ulNOG = 0; ulNOG < ul_NumberOfGEOM; ulNOG++)
	{
		if(mpst_Skn_Gro[ulNOG])
		{
			(mpst_Skn_Gro[ulNOG])->p_SKN_Objectponderation->ClassPtr = this;
			(mpst_Skn_Gro[ulNOG])->p_SKN_Objectponderation->SelectionCLBK = DIA_SKN_SelctionCLNK;
			UpdateSelection();
			GEO_SKN_Skin2Colors(mpst_Skn_Gro[ulNOG], Gizmo[0].GizmoIndex, Gizmo[1].GizmoIndex, Gizmo[2].GizmoIndex, 0);
			LINK_Refresh();
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SKN_Dialog::OnMouseMove(UINT, CPoint)
{
	/*~~~~~~~~~~~~~~~~~~*/
	ULONG	Item;
	RECT	LocalRect;
	ULONG	Counter, Pass;
	POINT	MousePoint;
	/*~~~~~~~~~~~~~~~~~~*/

	Pass = 0;
	SetCapture();
	SetFocus();
	if(bMousebIsDown || bRMousebIsDown)
	{
		GetCursorPos(&MousePoint);
		for(Counter = 0; Counter < 4; Counter++)
		{
			if(bRMousebIsDown && (Counter != 3))
			{
				if(Gizmo[Counter].fValue != 1.0) Gizmo[Counter].fValue = DIA_SKN_MinValue;
			}

			if(UnderMouseItem == Gizmo[Counter].Item)
			{
				if(bRMousebIsDown && (Counter != 3))
				{
				}
				else
				{
					GetDlgItem(UnderMouseItem)->GetWindowRect(&LocalRect);
					Gizmo[Counter].fValue = (float) (MousePoint.x - LocalRect.left) / (float) (LocalRect.right - LocalRect.left);
					if(Gizmo[Counter].fValue < DIA_SKN_MinValue) Gizmo[Counter].fValue = DIA_SKN_MinValue;
					if(Gizmo[Counter].fValue > 1.0f) Gizmo[Counter].fValue = 1.0f;
					if(NumberOfSelectedPoints == 1)
					{
						/*~~~~~~~~~~~~~*/
						ULONG	RGB[3];
						float	Value[3];
						/*~~~~~~~~~~~~~*/

						RGB[0] = Gizmo[0].GizmoIndex;
						RGB[1] = Gizmo[1].GizmoIndex;
						RGB[2] = Gizmo[2].GizmoIndex;
						Value[0] = Gizmo[0].fValue;
						Value[1] = Gizmo[1].fValue;
						Value[2] = Gizmo[2].fValue;

						/* GEO_SKN_Push(mpst_Skn_Gro ); */
						for(ULONG ulNOG = 0; ulNOG < ul_NumberOfGEOM; ulNOG++)
						{
							GEO_SKN_SetNumberOfMatrix(mpst_Skn_Gro[ulNOG], GetGizmoNumber());
							GEO_SKN_Paint(mpst_Skn_Gro[ulNOG], RGB, Value, SKN_Cul_UseSubSel);
							GEO_SKN_Normalize(mpst_Skn_Gro[ulNOG], 0);
							GEO_SKN_Skin2Colors
							(
								mpst_Skn_Gro[ulNOG],
								Gizmo[0].GizmoIndex,
								Gizmo[1].GizmoIndex,
								Gizmo[2].GizmoIndex,
								0
							);
							LINK_Refresh();
						}
					}

					Pass = 1;
				}
			}

			GetDlgItem(Gizmo[Counter].Item)->GetWindowRect(&LocalRect);
			ScreenToClient(&LocalRect);
			InvalidateRect(&LocalRect);
		}
	}

	if(!Pass)
	{
		Item = UnderMouseItem;
		GetItemFromPoint();
		if(Item != UnderMouseItem)
		{
			if(Item)
			{
				GetDlgItem(Item)->GetWindowRect(&LocalRect);
				ScreenToClient(&LocalRect);
				InvalidateRect(&LocalRect);
			}
		}
	}

	if(UnderMouseItem)
	{
		GetDlgItem(UnderMouseItem)->GetWindowRect(&LocalRect);
		ScreenToClient(&LocalRect);
		InvalidateRect(&LocalRect);
	}

	UpdateWindow();

	if(!IF_IS_IN(-1) && !bLocked)
	{
		bMousebIsDown = 0;
		bRMousebIsDown = 0;
		ReleaseCapture();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SKN_Dialog::Command(ULONG ul_CommandNum)
{
	for(ULONG ulNOG = 0; ulNOG < ul_NumberOfGEOM; ulNOG++)
	{
		GEO_SKN_SetNumberOfMatrix(mpst_Skn_Gro[ulNOG], GetGizmoNumber());
		if((ul_CommandNum == 0) || (ul_CommandNum == 4) || (ul_CommandNum == 8))
		{
			/*~~~~~~~~~~~~~*/
			ULONG	RGB[3];
			float	Value[3];
			/*~~~~~~~~~~~~~*/

			RGB[0] = Gizmo[0].GizmoIndex;
			RGB[1] = Gizmo[1].GizmoIndex;
			RGB[2] = Gizmo[2].GizmoIndex;
			Value[0] = Gizmo[0].fValue;
			Value[1] = Gizmo[1].fValue;
			Value[2] = Gizmo[2].fValue;
			GEO_SKN_Push(mpst_Skn_Gro[ulNOG]);
			if(ul_CommandNum == 4)
				GEO_SKN_Paint_ADD(mpst_Skn_Gro[ulNOG], RGB, Value, SKN_Cul_UseSubSel);
			else if(ul_CommandNum == 8)
				GEO_SKN_Paint_SUB(mpst_Skn_Gro[ulNOG], RGB, Value, SKN_Cul_UseSubSel);
			else
				GEO_SKN_Paint(mpst_Skn_Gro[ulNOG], RGB, Value, SKN_Cul_UseSubSel);
			GEO_SKN_Normalize(mpst_Skn_Gro[ulNOG], 0);
			GEO_SKN_Skin2Colors(mpst_Skn_Gro[ulNOG], Gizmo[0].GizmoIndex, Gizmo[1].GizmoIndex, Gizmo[2].GizmoIndex, 0);
		}

		if(ul_CommandNum == 6)
		{
			GEO_SKN_InsertGizmo
			(
				mpst_Skn_Gro[ulNOG],
				Gizmo[0].GizmoIndex,
				Gizmo[2].GizmoIndex,
				Gizmo[1].GizmoIndex,
				Gizmo[3].fValue,
				SKN_Cul_UseSubSel
			);
			GEO_SKN_Normalize(mpst_Skn_Gro[ulNOG], 0);
			UpdateSelection();
		}

		if(ul_CommandNum == 10)
		{
			GEO_SKN_ExpandGizmo(mpst_Skn_Gro[ulNOG], Gizmo[0].GizmoIndex, Gizmo[3].fValue, SKN_Cul_UseSubSel);
			GEO_SKN_Normalize(mpst_Skn_Gro[ulNOG], 0);
			UpdateSelection();
		}

		if(ul_CommandNum == 9)
		{
			GEO_SKN_Push(mpst_Skn_Gro[ulNOG]);
			GEO_SKN_SelectGizmo
			(
				mpst_Skn_Gro[ulNOG],
				Gizmo[0].GizmoIndex,
				(Gizmo[3].fValue * 0.9f) + 0.05f,
				GetAsyncKeyState(VK_CONTROL)
			);
            UpdateSelection();
		}

		if(ul_CommandNum == 1)
		{
			GEO_SKN_Smooth(mpst_Skn_Gro[ulNOG], Gizmo[3].fValue, SKN_Cul_UseSubSel);
			GEO_SKN_Normalize(mpst_Skn_Gro[ulNOG], 0);
			UpdateSelection();
		}

		if(ul_CommandNum == 2)
		{
			GEO_SKN_Push(mpst_Skn_Gro[ulNOG]);
			GEO_SKN_SwitchGizmo(mpst_Skn_Gro[ulNOG], Gizmo[0].GizmoIndex, Gizmo[1].GizmoIndex, SKN_Cul_UseSubSel);
			GEO_SKN_Normalize(mpst_Skn_Gro[ulNOG], 0);
			UpdateSelection();
		}

		if(ul_CommandNum == 5)
		{
			GEO_SKN_Push(mpst_Skn_Gro[ulNOG]);
			GEO_SKN_Contrast(mpst_Skn_Gro[ulNOG], 2.0f, SKN_Cul_UseSubSel);
			GEO_SKN_Normalize(mpst_Skn_Gro[ulNOG], 0);
			GEO_SKN_Skin2Colors(mpst_Skn_Gro[ulNOG], Gizmo[0].GizmoIndex, Gizmo[1].GizmoIndex, Gizmo[2].GizmoIndex, 0);
		}

		if(ul_CommandNum == 11) /* UNDO */
		{
			GEO_SKN_Pop(mpst_Skn_Gro[ulNOG]);
			GEO_SKN_Skin2Colors(mpst_Skn_Gro[ulNOG], Gizmo[0].GizmoIndex, Gizmo[1].GizmoIndex, Gizmo[2].GizmoIndex, 0);
		}

		if(ul_CommandNum == 15) /* REDO */
		{
			GEO_SKN_REDO(mpst_Skn_Gro[ulNOG]);
			GEO_SKN_Skin2Colors(mpst_Skn_Gro[ulNOG], Gizmo[0].GizmoIndex, Gizmo[1].GizmoIndex, Gizmo[2].GizmoIndex, 0);
		}

		if(ul_CommandNum == 14) /* Tresh Limit min */
		{
			GEO_SKN_Push(mpst_Skn_Gro[ulNOG]);
			GEO_SKN_ForceLimitMin(mpst_Skn_Gro[ulNOG], Gizmo[3].fValue,SKN_Cul_UseSubSel);
			GEO_SKN_Normalize(mpst_Skn_Gro[ulNOG], 0);
			GEO_SKN_Skin2Colors(mpst_Skn_Gro[ulNOG], Gizmo[0].GizmoIndex, Gizmo[1].GizmoIndex, Gizmo[2].GizmoIndex, 0);
			UpdateSelection();
		}

		if(ul_CommandNum == 3)
		{
			if(mpst_Skn_Gro)
			{
				/*~~~~~~~~~~~~~~~~~~*/
				ULONG	MatrixCounter;
				/*~~~~~~~~~~~~~~~~~~*/

				MatrixCounter = GetGizmoNumber();
				while(MatrixCounter--)
				{
					if((mpst_Skn_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix) && !(mpst_Skn_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer))
						MATH_SetFullIdentityType(&mpst_Skn_Gao->pst_Base->pst_AddMatrix->dst_Gizmo[MatrixCounter].st_Matrix);
					GEO_SKN_RestoreGizmo(mpst_Skn_Gao, mpst_Skn_Gro[ulNOG], MatrixCounter);
				}

				OBJ_Gizmo_ForceEditionPos(mpst_Skn_Gao);
				if(mpst_Skn_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)
				{
					if((mpst_Skn_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)&&(mpst_Skn_Gao->pst_Base->pst_AddMatrix->dst_GizmoPtr[0].pst_GO))
						OBJ_Gizmo_Update_LOAD(mpst_Skn_Gao->pst_Base->pst_AddMatrix->dst_GizmoPtr[0].pst_GO);
				}
			}
		}

		if ((ul_CommandNum == 7) || (ul_CommandNum == 12) || (ul_CommandNum == 13))
		{
			GEO_SKN_Push(mpst_Skn_Gro[ulNOG]);
			GEO_SKN_SetNumberOfMatrix(mpst_Skn_Gro[ulNOG], GetGizmoNumber());
			if(mpst_Skn_Gro)
			{
				/*~~~~~~~~~~~~~~~~~~*/
				ULONG	MatrixCounter;
				/*~~~~~~~~~~~~~~~~~~*/

				MatrixCounter = GetGizmoNumber();
				while(MatrixCounter--) GEO_SKN_FlashGizmo(mpst_Skn_Gao, mpst_Skn_Gro[ulNOG], MatrixCounter);
			}
			if (ul_CommandNum == 12) /* SYM X */
			{
				GEO_SKN_Symetrise_X(mpst_Skn_Gao, mpst_Skn_Gro[ulNOG], SKN_Cul_UseSubSel);
			} 
            else if (ul_CommandNum == 13) /* Deduct Proxy */
			{
				GEO_SKN_Deduct_Proxy(mpst_Skn_Gao, mpst_Skn_Gro[ulNOG], SKN_Cul_UseSubSel);
			}
            UpdateSelection();

		}


		if(ul_CommandNum == GizmoAreLockedIndex - 4) GizmoAreLocked ^= 1;
		if(ul_CommandNum == GizmoAreLockedIndex - 3)
		{
			GEO_SKN_Push(mpst_Skn_Gro[ulNOG]);
			GEO_SKN_R_180_Gizmo(mpst_Skn_Gao, mpst_Skn_Gro[ulNOG], Gizmo[0].GizmoIndex);
            UpdateSelection();
		}

        if (ul_CommandNum == 22)
        {
            ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PICK));
        }
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SKN_Dialog::OnLButtonDown(UINT, CPoint)
{
	/*~~~~~~~~~~~~~~*/
	RECT	LocalRect;
	ULONG	Counter;
	/*~~~~~~~~~~~~~~*/

	GetItemFromPoint();

	if(UnderMouseItem)
	{
		ItemDBCLK = 0;
		for(Counter = 0; Counter < 4; Counter++)
		{
			if(UnderMouseItem == Gizmo[Counter].Item)
			{
				bMousebIsDown = 1;
				ItemDBCLK = UnderMouseItem;
				GetDlgItem(UnderMouseItem)->GetWindowRect(&LocalRect);
				GetClipCursor(&OldClip);
				ClipCursor(&LocalRect);
			}
		}

		for(Counter = 4; Counter < AllItemsNum; Counter++)
		{
			if(UnderMouseItem == AllItem[Counter])
			{
				AllItemStates[Counter] |= ITM_Pshed;
				bMousebIsDown = 1;
				bLocked = 1;
				Command(Counter - 4);
			}
		}

		OnMouseMove(0, (CPoint)0);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SKN_Dialog::OnLButtonUp(UINT _ui_Flags, CPoint pt)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	Counter;
	RECT	LocalRect;
    BOOL    b_Pick;
    char    c_Sel;
	/*~~~~~~~~~~~~~~*/

    b_Pick = FALSE;
	if(bMousebIsDown)
	{
		ClipCursor(&OldClip);
		for(Counter = 4; Counter < AllItemsNum; Counter++)
		{
			if(AllItemStates[Counter] & ITM_Pshed)
			{
				GetDlgItem(AllItem[Counter])->GetWindowRect(&LocalRect);
				ScreenToClient(&LocalRect);
				InvalidateRect(&LocalRect);
                if (Counter == 22)
                    b_Pick = TRUE;
			}

			AllItemStates[Counter] &= ~ITM_Pshed;
		}

		bMousebIsDown = 0;
		bLocked = 0;
		UpdateWindow();
	}

	if(!IF_IS_IN(-1))
	{
		bMousebIsDown = 0;
		bRMousebIsDown = 0;
		ReleaseCapture();
	}

    if (b_Pick)
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        F3D_cl_View                     *po_3DView;
        POINT                           st_Pt;
        OBJ_tdst_GameObject             *pst_GO[32];
        GEO_tdst_Object                 *pst_Gro[32];
        LONG                            l_Number, l_Index;
        SOFT_tdst_PickingBuffer_Pixel   *pst_Pick;
        MATH_tdst_Matrix                *MDst, *MSrc;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

        GetCursorPos( &st_Pt );
        if (!mpo_GroView) return;
        po_3DView = mpo_GroView->mpo_ToolBox->mpo_View;
        po_3DView->ScreenToClient( &st_Pt );
        if (!po_3DView->Pick_l_UnderPoint( &st_Pt, SOFT_Cuc_PBQF_GameObject, 0 )) return;

        pst_Pick = po_3DView->Pick_pst_GetFirst(SOFT_Cuc_PBQF_GameObject, -1);
        l_Number = 0;
        while (pst_Pick)
        {
            pst_GO[l_Number] = (OBJ_tdst_GameObject *) pst_Pick->ul_Value;
	        pst_Gro[l_Number] = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo( pst_GO[l_Number] );
            if ( ( pst_Gro[l_Number]) && ( GEO_SKN_IsSkinned( pst_Gro[l_Number] )) )
                l_Number++;
            if (l_Number == 32) break;
            pst_Pick = po_3DView->Pick_pst_GetNext( SOFT_Cuc_PBQF_GameObject, -1, pst_Pick);
        }
        
        if (l_Number == 0) 
            return;

        if (l_Number > 1)
        {
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            EDIA_cl_NameDialogCombo	o_Dialog("Skin selection");
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            
            for (l_Index = 0;  l_Index < l_Number; l_Index++)
                o_Dialog.AddItem( pst_GO[l_Index]->sz_Name, l_Index );

            if(o_Dialog.DoModal() != IDOK) return;
	
            l_Index = o_Dialog.mi_CurSelData;
            if ( (l_Index < 0) || (l_Index >= l_Number) ) return;
        }
        else
            l_Index = 0;

        /*$F
        pst_GO = (OBJ_tdst_GameObject *) po_3DView->Pick_pst_GetFirst(SOFT_Cuc_PBQF_GameObject, -1)->ul_Value;
	    pst_Gro = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo( pst_GO );
        if (!pst_Gro) return;
        if (!GEO_SKN_IsSkinned( pst_Gro )) return;
        */

        if ( _ui_Flags & MK_SHIFT)
        {
            MDst = OBJ_pst_GetAbsoluteMatrix( mpst_Skn_Gao );
            MSrc = OBJ_pst_GetAbsoluteMatrix( pst_GO[l_Index] );
        }
        else
            MDst = MSrc = NULL;

        GEO_SKN_Push(mpst_Skn_Gro[0]);

        l_Number = 0;
        for (Counter = 0; Counter < (ULONG) mpst_Skn_Gro[0]->l_NbPoints; Counter++)
            l_Number += (mpst_Skn_Gro[0]->pst_SubObject->dc_VSel[Counter] & 1);

        c_Sel = ( (l_Number) && (l_Number != pst_Gro[l_Index]->l_NbPoints)) ? 1 : 0;
        GEO_SKN_AdaptToAnotherSkin( mpst_Skn_Gao, pst_GO[l_Index], mpst_Skn_Gro[0], pst_Gro[l_Index], c_Sel, MDst, MSrc );
        GEO_SKN_Skin2Colors(mpst_Skn_Gro[0], Gizmo[0].GizmoIndex, Gizmo[1].GizmoIndex, Gizmo[2].GizmoIndex, 0);
        LINK_Refresh();
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SKN_Dialog::OnRButtonDown(UINT, CPoint)
{
	/*~~~~~~~~~~~~*/
	ULONG	Counter;
	/*~~~~~~~~~~~~*/

	GetItemFromPoint();


	if(UnderMouseItem)
	{
		for(Counter = 0; Counter < 3; Counter++)
		{
			if(UnderMouseItem == Gizmo[Counter].Item)
			{
				if(Gizmo[Counter].fValue == 1.0f)
					Gizmo[Counter].fValue = DIA_SKN_MinValue;
				else
					Gizmo[Counter].fValue = 1.0f;
				bRMousebIsDown = 1;
				OnMouseMove(0, 0);
			}
		}
	}

	if(!IF_IS_IN(-1))
	{
		bMousebIsDown = 0;
		bRMousebIsDown = 0;
		ReleaseCapture();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SKN_Dialog::OnRButtonUp(UINT, CPoint)
{
	bRMousebIsDown = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SKN_Dialog::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC				*PDC;
	HBRUSH			brh;
	RECT			LocalRect, SaveRect;
	PAINTSTRUCT		ps;
	unsigned long	Counter;
	ULONG			UndoRedoState;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	BeginPaint(&ps);

	UndoRedoState = GEO_SKN_Get_UnRe_Sate(mpst_Skn_Gro[0]);

	if(GizmoAreLocked)
		AllItemStates[GizmoAreLockedIndex] |= ITM_Pshed;
	else
		AllItemStates[GizmoAreLockedIndex] &= ~ITM_Pshed;

	/* Erase bkgnd */
	for(Counter = 0; Counter < AllItemsNum; Counter++)
	{
		GetDlgItem(AllItem[Counter])->GetWindowRect(&LocalRect);
		ScreenToClient(&LocalRect);
		ExcludeClipRect(ps.hdc, LocalRect.left - 1, LocalRect.top - 1, LocalRect.right + 1, LocalRect.bottom);
	}

	brh = CreateSolidBrush(ITEMFILLCOLOR());
	GetClientRect(&LocalRect);
	FillRect(ps.hdc, &LocalRect, brh);
	DeleteObject(brh);
	SelectClipRgn(ps.hdc, NULL);

	if(UnderMouseItem != 0) EDI_Tooltip_DisplayMessage(DIASKN_FunctionInfo[UnderMouseItemIndex], 800);

	for(Counter = 0; Counter < 4; Counter++)
	{
		/*~~~~~~~~*/
		float	Pos;
		/*~~~~~~~~*/

		/* Draw frame */
		GetDlgItem(Gizmo[Counter].Item)->GetWindowRect(&LocalRect);
		ScreenToClient(&LocalRect);
		SaveRect = LocalRect;
		Pos = Gizmo[Counter].fValue;
		LocalRect.left = LocalRect.left + (int) ((float) (LocalRect.right - LocalRect.left) * Pos);
		brh = CreateSolidBrush(0x3f << (Counter << 3));
		FillRect(ps.hdc, &LocalRect, brh);
		DeleteObject(brh);
		LocalRect.right = LocalRect.left;
		LocalRect.left -= 1;
		brh = CreateSolidBrush(ITEMGIZMOLINECOLOR());
		FillRect(ps.hdc, &LocalRect, brh);
		DeleteObject(brh);

		SaveRect.right = LocalRect.left;
		if(Counter == 3)
			brh = CreateSolidBrush(0x5f5f5f);
		else
			brh = CreateSolidBrush(0x9f << (Counter << 3));
		FillRect(ps.hdc, &SaveRect, brh);
		DeleteObject(brh);

		/* Draw text */
		GetDlgItem(Gizmo[Counter].Item)->GetWindowRect(&LocalRect);
		ScreenToClient(&LocalRect);
		LocalRect.top += 3;
		SetTextColor(ps.hdc, 0);
		SetBkMode(ps.hdc, TRANSPARENT);
		if(UnderMouseItem == Gizmo[Counter].Item)
			SetTextColor(ps.hdc, ITEMTEXTGIZMOCOLOR_HILI());
		else
			SetTextColor(ps.hdc, ITEMTEXTGIZMOCOLOR());
		{
			/*~~~~~~~~~~~~~~~~~~~*/
			char	GizmText[1024];
			/*~~~~~~~~~~~~~~~~~~~*/

			LocalRect.left += 5;
			LocalRect.right -= 5;
			switch(Counter)
			{
			case 0:
				sprintf(GizmText, "R:%d-%s", Gizmo[Counter].GizmoIndex, GetGizmoName(Gizmo[Counter].GizmoIndex));
				break;
			case 1:
				sprintf(GizmText, "G:%d-%s", Gizmo[Counter].GizmoIndex, GetGizmoName(Gizmo[Counter].GizmoIndex));
				break;
			case 2:
				sprintf(GizmText, "B:%d-%s", Gizmo[Counter].GizmoIndex, GetGizmoName(Gizmo[Counter].GizmoIndex));
				break;
			case 3:
				sprintf(GizmText, "Straigth");
				break;
			}

			DrawText(ps.hdc, GizmText, strlen(GizmText), &LocalRect, DT_LEFT | DT_VCENTER);
			if(Counter != 3)
			{
				 if (Gizmo[Counter].fValue == DIA_SKN_MinValue) 
                     sprintf(GizmText , "%.0f " ,0.0f ); 
                 else 
                     sprintf(GizmText , "%.0f " , 100.0f * Gizmo[Counter].fValue / (Gizmo[0].fValue + Gizmo[1].fValue + Gizmo[2].fValue));
			}
			else
				sprintf(GizmText, "%.0f %%", 100.0f * Gizmo[Counter].fValue);
			DrawText(ps.hdc, GizmText, strlen(GizmText), &LocalRect, DT_RIGHT | DT_VCENTER);
		}
	}

	for(Counter = 4; Counter < AllItemsNum; Counter++)
	{
		GetDlgItem(AllItem[Counter])->GetWindowRect(&LocalRect);
		ScreenToClient(&LocalRect);
		brh = CreateSolidBrush(ITEMFILLCOLOR());
		FillRect(ps.hdc, &LocalRect, brh);
		DeleteObject(brh);

		GetDlgItem(AllItem[Counter])->GetWindowRect(&LocalRect);
		ScreenToClient(&LocalRect);
		if(Counter < GizmoAreLockedIndex) LocalRect.top += 3;
		SetTextColor(ps.hdc, 0);
		SetBkMode(ps.hdc, TRANSPARENT);
		if(AllItemStates[Counter] & ITM_Pshed)
			SetTextColor(ps.hdc, 0xffff);
		else if(Counter > GizmoAreLockedIndex)
			SetTextColor(ps.hdc, 0xff);
		else
		{
			if ((Counter == 15) && (!(UndoRedoState & 1))) /* UNDO */
			{
				SetTextColor(ps.hdc, ITEMTEXTCOLORDisable());
			} else
			if ((Counter == 19) && (!(UndoRedoState & 2))) /* REDO */
			{
				SetTextColor(ps.hdc, ITEMTEXTCOLORDisable());
			} else
			SetTextColor(ps.hdc, ITEMTEXTCOLOR());
		}

		DrawText
		(
			ps.hdc,
			DIASKN_FunctionNames[Counter - 4],
			strlen(DIASKN_FunctionNames[Counter - 4]),
			&LocalRect,
			DT_CENTER | DT_VCENTER
		);
	}

	PDC = GetDC();
	for(Counter = 0; Counter < AllItemsNum; Counter++)
	{
		GetDlgItem(AllItem[Counter])->GetWindowRect(&LocalRect);
		ScreenToClient(&LocalRect);
		INFLATERECT(&LocalRect, -1, -1);
		if(AllItemStates[Counter] & ITM_Pshed)
			PDC->Draw3dRect(&LocalRect, 0, GetSysColor(COLOR_3DHILIGHT));
		else
		{
			if (UnderMouseItem == AllItem[Counter])
			{
				PDC->Draw3dRect(&LocalRect, GetSysColor(COLOR_3DHILIGHT), 0);
			}
			else
				PDC->Draw3dRect(&LocalRect, ITEMLINECOLOR(), ITEMLINECOLOR());
		}
	}

	ReleaseDC(PDC);

	EndPaint(&ps);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SKN_Dialog::OnDestroy(void)
{
    EDIA_cl_ToolBox_GROView *po_GroView;

	for(ULONG ulNOG = 0; ulNOG < ul_NumberOfGEOM; ulNOG++)
	{
		if(mpst_Skn_Gro[ulNOG])
		{
			if(mpst_Skn_Gro[ulNOG]->st_Id.l_Ref)
			{
				if((mpst_Skn_Gro[ulNOG])->p_SKN_Objectponderation)
				{
					(mpst_Skn_Gro[ulNOG])->p_SKN_Objectponderation->ClassPtr = NULL;
					(mpst_Skn_Gro[ulNOG])->p_SKN_Objectponderation->SelectionCLBK = NULL;
				}
			}
		}
	}

	ClipCursor(NULL);
	ReleaseCapture();

    po_GroView = mpo_GroView;
    if (po_GroView)
        po_GroView->CloseSkinEditor( TRUE );

    LINK_Refresh();

    EDIA_cl_BaseDialog::OnDestroy();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SKN_Dialog::OnEraseBkgnd(CDC *pdc)
{
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SKN_Dialog::OnMouseWheel(UINT fFlags, short zDelta, CPoint point)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	Counter, LastGizmoI;
	LONG	Delta;
	RECT	LocalRect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(UnderMouseItem)
	{
		for(Counter = 0; Counter < 3; Counter++)
		{
			if(UnderMouseItem == Gizmo[Counter].Item)
			{
				LastGizmoI = Gizmo[Counter].GizmoIndex;
				if(zDelta < 0.0f)
				{
					for (Delta = Gizmo[Counter].GizmoIndex + 1; Delta < GetGizmoNumber() ; Delta++)
					{
						if ((Gizmo[(Counter + 1) % 3].GizmoIndex != Delta) &&
							(Gizmo[(Counter + 2) % 3].GizmoIndex != Delta))
						{
							Gizmo[Counter].GizmoIndex = Delta;
							Delta = GetGizmoNumber();
						}
					}
				}
				else
				{
					if (Gizmo[Counter].GizmoIndex != 0)
					{
						for (Delta = Gizmo[Counter].GizmoIndex - 1; Delta >= 0 ; Delta--)
						{
							if ((Gizmo[(Counter + 1) % 3].GizmoIndex != Delta) &&
								(Gizmo[(Counter + 2) % 3].GizmoIndex != Delta))
							{
								Gizmo[Counter].GizmoIndex = Delta;
								Delta = -1;
							}
						}
					}
				}

/*
				if(zDelta < 0.0f)
					Delta = 1;
				else
					Delta = -1;
				if(Gizmo[Counter].GizmoIndex + Delta < 0) Delta = 0;
				if(Gizmo[Counter].GizmoIndex + Delta >= GetGizmoNumber()) Delta = 0;
				if((Gizmo[Counter].GizmoIndex + Delta) == Gizmo[(Counter + 1) % 3].GizmoIndex)
				{
					Gizmo[(Counter + 1) % 3].GizmoIndex -= Delta;
				}
				else if((Gizmo[Counter].GizmoIndex + Delta) == Gizmo[(Counter + 2) % 3].GizmoIndex)
				{
					Gizmo[(Counter + 2) % 3].GizmoIndex -= Delta;
				}

				Gizmo[Counter].GizmoIndex += Delta;
*/
				GetDlgItem(Gizmo[Counter].Item)->GetWindowRect(&LocalRect);
				ScreenToClient(&LocalRect);
				InvalidateRect(&LocalRect);
			}
		}

		if(mpst_Skn_Gro)
		{
			for(ULONG ulNOG = 0; ulNOG < ul_NumberOfGEOM; ulNOG++)
			{
				GEO_SKN_SetNumberOfMatrix(mpst_Skn_Gro[ulNOG], GetGizmoNumber());
				GEO_SKN_Skin2Colors
				(
					mpst_Skn_Gro[ulNOG],
					Gizmo[0].GizmoIndex,
					Gizmo[1].GizmoIndex,
					Gizmo[2].GizmoIndex,
					0
				);
			}
		}

		UpdateWindow();
		LINK_Refresh();
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_SKN_Dialog::i_OnMessage(ULONG, ULONG, ULONG)
{
	/*~~*/
	int p;
	/*~~*/

	if
	(
		(LINK_gx_PointersJustDeleted.Lookup(mpst_Skn_Gao, (void * &) p))
	||	(LINK_gx_PointersJustDeleted.Lookup(mpst_Skn_Gao->pst_World, (void * &) p))
	)
	{
		EndDialog(0);
	}

	return 1;
};

#endif /* ACTIVE_EDITORS */
