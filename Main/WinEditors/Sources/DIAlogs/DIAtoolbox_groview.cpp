/*$T DIAtoolbox_groview.cpp GC! 1.100 08/28/01 15:57:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "Res/Res.h"

#include "../dlls/MAD_loadsave/Sources/MAD_STRUCT_V0.h"
#include "../dlls/MAD_mem/Sources/MAD_mem.h"

#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAtoolbox_groview.h"
#include "DIAlogs/DIACOLOR_dlg.h"
#include "DIAlogs/DIA_UPDATE_dlg.h"
#include "DIAlogs/DIA_SKN_dlg.h"
#include "DIAlogs/DIAradiosity_dlg.h"
#include "DIAlogs/DIA_RADIOSTY2.h"
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAuvmapper_dlg.h"
#include "DIAlogs/DIAvector_dlg.h"
#include "DIAlogs/DIArliadjust_dlg.h"
#include "DIAlogs/DIA_SmoothSel_dlg.h"
#include "DIAlogs/DIAstockofcolor_dlg.h"

#ifdef JADEFUSION
#include "DIAlogs/DIAXeMergeFlags.h"
#endif

#include "Editors/Sources/OUTput/OUTframe.h"
#include "Editors/Sources/OUTput/OUTmsg.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"

#include "Engine/Sources/WORld/WORstruct.h"
#include "Engine/sources/OBJects/OBJslowaccess.h"
#include "Engine/sources/OBJects/OBJorient.h"

#include "GEOmetric/GEO_MRM.h"
#include "GEOmetric/GEO_SKIN.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "GEOmetric/GEO_STRIP.h"
#include "GEOmetric/GEO_LIGHTCUT.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEOload.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "SELection/SELection.h"

#ifdef JADEFUSION
#include "GEOmetric/GEOXenonPack.h"

#if defined(_XENON_RENDER)
#include "XenonMeshProcessing/Sources/XMP_XenonMesh.h"
#endif
#endif

IMPLEMENT_DYNCREATE(EDIA_cl_ToolBox_GROView, CFormView)
extern void					EDI_Tooltip_DisplayMessage(char *, ULONG ulSpeed = 200);
static EDIA_cl_UPDATEDialog *po_Dial = NULL;

static int LODCTRL_Name[] = { IDC_BUTTON_LOD0, IDC_BUTTON_LOD1, IDC_BUTTON_LOD2, IDC_BUTTON_LOD3, IDC_BUTTON_LOD4, IDC_BUTTON_LOD5 };
static int LODCTRL_Lock[] = { IDC_BUTTON_LOCKLOD0, IDC_BUTTON_LOCKLOD1, IDC_BUTTON_LOCKLOD2, IDC_BUTTON_LOCKLOD3, IDC_BUTTON_LOCKLOD4, IDC_BUTTON_LOCKLOD5 };
static int LODCTRL_Dist[] = { IDC_STATIC_LODDIST0, IDC_STATIC_LODDIST1, IDC_STATIC_LODDIST2, IDC_STATIC_LODDIST3, IDC_STATIC_LODDIST4, IDC_STATIC_LODDIST5 };

static char *gasz_FaceSelName[4] = { "SelExt", "SelNear", "SelUV", "SelN2" }; 
static char *gasz_ColorModeName[3] = { "Off", "FaceId", "Alpha" };
static char *gasz_SymOpShortName[GRO_Cul_EOSYM_Number] = { "Sym X", "Sym Y", "Sym Z", "Sym PX", "Sym PY", "Sym PZ", "Sym HX", "Sym HY", "Sym HZ", "Sym PC", "Sym HC"  };
static char *gasz_SymOpLongName[GRO_Cul_EOSYM_Number] = { "Sym (Pivot+Local) X", "Sym (Pivot+Local) Y", "Sym (Pivot + Local) Z", "Sym (Pivot + Helper) X", "Sym (Pivot  Helper) Y", "Sym (Pivot + Helper) Z", "Sym (Helper) X", "Sym (Helper) Y", "Sym (Herlper) Z", "Sym (Pivot) centrale", "Sym (helper) centrale"  };

#ifndef JADEFUSION
extern "C"  /* Philippe Vimont Code comme oun pork ! */
{
#endif
	extern ULONG SmoothSelIsActivate ;
	extern ULONG InvertSel ;
	extern float DistanceMax ;
#ifndef JADEFUSION
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG SetBarreState(float Position, char *Text)
{
    return (po_Dial ? po_Dial->OnRefreshBarText(Position, Text) : 0);
}
void SetBarreState2(float Position)
{
	static unsigned char Value;
	unsigned char NewValue;
	NewValue = (unsigned char )(Position *255.0f);
	if (NewValue != Value)
		po_Dial->OnRefreshBarText(Position, "Compute Strip");
	Value = NewValue;
}


/*$4
 ***********************************************************************************************************************
    Message map
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_ToolBox_GROView, CFormView)
	ON_BN_CLICKED(IDC_CHECK_COLORMODE, OnCheck_ColorMode )
	ON_BN_CLICKED(IDC_CHECK_SUBOBJECTMODE, OnCheck_SubObjectMode)
	ON_BN_CLICKED(IDC_CHECK_SNAP, OnCheck_Snap)
	ON_BN_CLICKED(IDC_CHECK_SELWITHZ, OnCheck_VisibleSel)
	ON_BN_CLICKED(IDC_CHECK_PIVOT, OnCheck_Pivot)
    ON_BN_CLICKED(IDC_BUTTON_INVERTSEL, OnButton_InvertSel )
    ON_BN_CLICKED(IDC_CHECK_MOVESUPDATEUV, OnCheck_MovesUpdateUV)
	ON_BN_CLICKED(IDC_CHECK_MOVESUPDATEMORPH, OnCheck_MovesUpdateMorph)
    ON_BN_CLICKED(IDC_BUTTON_NONMAGNIFOLD, OnButton_NonMagnifold )
	ON_BN_CLICKED(IDC_BUTTON_WARNINGSCALE, OnButton_WarningScale )
	ON_BN_CLICKED(IDC_BUTTON_WARNINGCOLMAP, OnButton_WarningColmap)
	ON_BN_CLICKED(IDC_CHECK_UV, OnCheck_UV)
	ON_BN_CLICKED(IDC_CHECK_EDITSKIN, On_EditSkin)
	ON_BN_CLICKED(IDC_BUTTON_DESTROYSKIN, On_DestroySkin)
	ON_BN_CLICKED(IDC_BUTTON_ADJ_SYM, On_Adjust4Symetrie)
	ON_BN_CLICKED(IDC_RADIO_VERTEX, OnRadio_VertexMode)
	ON_BN_CLICKED(IDC_CHECK_PAINTVERTEX, OnCheck_PaintVertex)
	ON_BN_CLICKED(IDC_CHECK_PAINTALPHAVERTEX, OnCheck_PaintVertexAlpha)
	ON_BN_CLICKED(IDC_CHECK_SmoothSel, OnCheck_SmoothSel)
	ON_BN_CLICKED(IDC_BUTTON_APPLYPAINTONSEL, OnButton_PaintSel)
	ON_BN_CLICKED(IDC_CHECK_PICKVERTEXCOLOR, OnCheck_PickVertexColor)
	ON_BN_CLICKED(IDC_CHECK_VERTEXADJUSTRLI, OnButton_VAdjustRLI )
	ON_BN_CLICKED(IDC_CHECK_WELDVERTEX, OnCheck_WeldVertex)
	ON_BN_CLICKED(IDC_BUTTON_WELDTHRESH, OnButton_VWeldThresh)
	ON_EN_KILLFOCUS(IDC_EDIT_WELDTHRESH, OnEdit_WeldThresh)
#ifdef JADEFUSION
	ON_BN_CLICKED(IDC_BUTTON_XEREFRESH, OnButton_Refresh)
#endif
    ON_BN_CLICKED(IDC_BUTTON_VSYM, OnButton_VSym)
	ON_BN_CLICKED(IDC_CHECK_VBUILDFACE, OnCheck_VBuildFace)
    ON_BN_CLICKED(IDC_CHECK_SHOWEDGE, OnCheck_VShowEdge)
	ON_BN_CLICKED(IDC_RADIO_EDGE, OnRadio_EdgeMode)
	ON_BN_CLICKED(IDC_CHECK_TURNEDGE, OnCheck_TurnEdge)
	ON_BN_CLICKED(IDC_CHECK_CUTEDGE, OnCheck_CutEdge)
	ON_BN_CLICKED(IDC_CHECK_ECUTTER, OnCheck_ECutter)
	ON_BN_CLICKED(IDC_RADIO_FACE, OnRadio_FaceMode)
    ON_BN_CLICKED(IDC_BUTTON_FSELREJECTION, OnButton_FSelRejection )
	ON_BN_CLICKED(IDC_BUTTON_FLIPNORMALS, OnButton_FlipNormals)
	ON_BN_CLICKED(IDC_BUTTON_FACEID, OnButton_ChangeId)
	ON_BN_CLICKED(IDC_CHECK_LOCKID, OnCheck_LockId)
	ON_BN_CLICKED(IDC_BUTTON_FSELEXT, OnButton_SelFaceExt)
	ON_BN_CLICKED(IDC_BUTTON_DETACHFACE, OnButton_DetachFace)
    ON_BN_CLICKED(IDC_BUTTON_FATTACH, OnButton_AttachFace)
	ON_BN_CLICKED(IDC_CHECK_FCUT, OnCheck_FaceCut)
	ON_EN_KILLFOCUS(IDC_EDIT_ID, OnEdit_Id)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnButton_VertexColor)
	ON_BN_CLICKED(IDC_RADIO_NO_MRM, OnRadio_NoMRMMode)
	ON_BN_CLICKED(IDC_RADIO_MRM_CURVE, OnRadio_MRMCurveMode)
	ON_BN_CLICKED(IDC_RADIO_MRM_LOD, OnRadio_MRMLODMode)
	ON_BN_CLICKED(IDC_BUTTON_STRIP, OnButton_Strip)
	ON_BN_CLICKED(IDC_BUTTON_UNSTRIP, OnButton_UnStrip)
	ON_BN_CLICKED(IDC_CHECK_SHOWSTRIP, OnCheck_ShowStrip)
	ON_BN_CLICKED(IDC_CHECK_STRIPSTAT, OnCheck_ShowStripStat)
	ON_BN_CLICKED(IDC_BUTTONREBOOT, OnREboot)
	ON_BN_CLICKED(IDC_BUTTONREBOOT2, OnREboot2)
	ON_BN_CLICKED(IDC_BUTTONADJUSTRAD, OnAdjustRad)
	ON_BN_CLICKED(IDC_CHECK_SHOWRADRES, OnSRR)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_MRM_DISTANCE, On_SliderMRMDistance_Released)
#ifdef JADEFUSION
	ON_BN_CLICKED(IDC_BUTTON_XEMERGEFLAGS, OnXeMergeFlags)
#endif
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_MRM_LOD_NB, On_SliderMRMLODNb_Released)
#if !defined(_XENON_RENDER)
	ON_COMMAND(IDC_CHECK_FHIDE, OnButton_HideFace)
	ON_COMMAND(IDC_CHECK_FUNHIDE, OnButton_UnHideFace)
#endif
    ON_BN_CLICKED(IDC_BUTTON_LODTRANSFORM, OnButton_LODTransform )
    ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_LOD0, IDC_BUTTON_LOD5, OnButton_LOD )
    ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_LOCKLOD0, IDC_BUTTON_LOCKLOD5, OnButton_LockLOD )
	ON_WM_DRAWITEM()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_RADIO_RLIGAO, OnRadio_GAOsRLIMode)
	ON_BN_CLICKED(IDC_RADIO_RLIGRO, OnRadio_GROsRLIMode)
#ifdef JADEFUSION
    ON_BN_CLICKED(IDC_EGRO_XE_PROCESS,     OnButton_XenonProcess)
    ON_BN_CLICKED(IDC_EGRO_XE_REVERT,      OnButton_XenonRevert)
    ON_BN_CLICKED(IDC_EGRO_XE_EDIT_CREATE, OnButton_XenonEditCreate)
    ON_BN_CLICKED(IDC_EGRO_XE_EDIT_REMOVE, OnButton_XenonEditRemove)
    ON_BN_CLICKED(IDC_EGRO_XE_FIXEDGES,    OnButton_XenonFixEdges)
    ON_BN_CLICKED(IDC_BUTTON_LOD0,         OnButton_XenonLOD0)
    ON_BN_CLICKED(IDC_BUTTON_LOD1,         OnButton_XenonLOD1)
    ON_BN_CLICKED(IDC_BUTTON_LOD2,         OnButton_XenonLOD2)
    ON_BN_CLICKED(IDC_BUTTON_LOD3,         OnButton_XenonLOD3)
    ON_BN_CLICKED(IDC_BUTTON_LOD4,         OnButton_XenonLOD4)
    ON_BN_CLICKED(IDC_BUTTON_LOD5,         OnButton_XenonLOD5)
#endif
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ToolBox_GROView::~EDIA_cl_ToolBox_GROView(void)
{
	mb_Update = FALSE;
	DestroySkinEditor();

	if(mpo_UVMapper) delete mpo_UVMapper;

    mao_Cadenas[0].DeleteObject();
    mao_Cadenas[1].DeleteObject();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ToolBox_GROView::EDIA_cl_ToolBox_GROView(EDIA_cl_ToolBoxDialog *_po_ToolBox) :
	CFormView(EGRO_IDD_EDITVIEW)
{
	mb_Update = TRUE;
	mb_Init = FALSE;
	mpo_ToolBox = _po_ToolBox;
	b_LButtonIsDown = 0;
	b_RButtonIsDown = 0;
	b_MouseIsClipped = 0;
	b_IsSpinnigMRM = 0;
	b_IsSpinnigMRM_Min = 0;
    mi_MovedMRM_LOD_Cross = -1;
	if(mpo_ToolBox) mpst_EditOption = mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_EditOptions;
	mpst_SkinDialog = NULL;
	mst_SkinDialogPos.x = 0x7FFFFFFF;
    mi_LODCapture = 0;
    mi_LODDistCapture = 0;

	mpo_UVMapper = NULL;
	mpo_AdjustRLI = NULL;
#ifdef JADEFUSION
	mpo_XeMergeFlags = NULL;
#endif
	mi_Pick = 0;

    mao_Cadenas[0].LoadBitmap(IDB_BITMAP_CADENASFERME);
	mao_Cadenas[1].LoadBitmap(IDB_BITMAP_CADENASOUVERT);

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ToolBox_GROView::EDIA_cl_ToolBox_GROView(void) :
	CFormView(EGRO_IDD_EDITVIEW)
{
	EDIA_cl_ToolBox_GROView(NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_ToolBox_GROView::Create
(
	LPCTSTR		lpszClassName,
	LPCTSTR		lpszWindowName,
	DWORD		dwStyle,
	const RECT	&rect,
	CWnd		*pParentWnd
)
{
	CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, 0, NULL);

    CSliderCtrl *po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MRM_DISTANCE);
	po_Slider->SetRange(1, 1000, TRUE);
	po_Slider->SetPos(1000);

    po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MRM_LOD_NB);
	po_Slider->SetRange(1, 5, TRUE);
	po_Slider->SetPos(1);

	UpdateControls();
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::UpdateControls(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	sz_Value[100];
    ULONG   eMRMState = GEO_ul_GetMRMState((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro);
    BOOL    b_Sub, b;
    int     i,iPointNb;
	u8		RLIloc;
    extern void EDIA_cl_ToolBoxDialog_Update(ULONG, GEO_tdst_Object * );
	/*~~~~~~~~~~~~~~~~~*/
	
	if(!mb_Init)
	{
		GetDlgItem(IDC_EDIT_ID)->SetWindowText(_itoa(mpst_EditOption->l_FaceId, sz_Value, 10));
		sprintf(sz_Value, "%.3f", mpst_EditOption->f_VertexWeldThresh);
		GetDlgItem(IDC_EDIT_WELDTHRESH)->SetWindowText(sz_Value);
		((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN_ID))->SetRange32(0, 1000);

		/*sprintf(sz_Value, "%.3f", mpst_EditOption->f_MRMLevels[0]);
		GetDlgItem(IDC_EDIT_MRM_LEVEL_1)->SetWindowText(sz_Value);
		sprintf(sz_Value, "%.3f", mpst_EditOption->f_MRMLevels[1]);
		GetDlgItem(IDC_EDIT_MRM_LEVEL_2)->SetWindowText(sz_Value);
		sprintf(sz_Value, "%.3f", mpst_EditOption->f_MRMLevels[2]);
		GetDlgItem(IDC_EDIT_MRM_LEVEL_3)->SetWindowText(sz_Value);
		sprintf(sz_Value, "%.3f", mpst_EditOption->f_MRMLevels[3]);
		GetDlgItem(IDC_EDIT_MRM_LEVEL_4)->SetWindowText(sz_Value);
		sprintf(sz_Value, "%.3f", mpst_EditOption->f_MRMLevels[4]);
		GetDlgItem(IDC_EDIT_MRM_LEVEL_5)->SetWindowText(sz_Value);*/

		mb_Init = TRUE;
	}
	
	if(!mb_Update) return;

	M_MF()->LockDisplay(this);
	mpst_EditOption = mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_EditOptions;
	BeginCurTool();

	/* gao name */
    GetDlgItem(IDC_STATIC_GAONAME)->SetWindowText((mpo_ToolBox->mpst_Gao) ? mpo_ToolBox->mpst_Gao->sz_Name : "No Current Selection");
    
    /* color mode button */
    ((CButton *) GetDlgItem(IDC_CHECK_COLORMODE))->SetCheck(mpst_EditOption->ul_Flags & GRO_Cul_EOF_ColorModeMask ? 1 : 0 );
    GetDlgItem(IDC_CHECK_COLORMODE)->SetWindowText( gasz_ColorModeName[ (mpst_EditOption->ul_Flags & GRO_Cul_EOF_ColorModeMask) >> GRO_Cul_EOF_ColorModeShift ] );

    /* sub object check box */
    b_Sub = mpst_EditOption->ul_Flags & GRO_Cul_EOF_SubObject;
	((CButton *) GetDlgItem(IDC_CHECK_SUBOBJECTMODE))->SetCheck( b_Sub );
    
    /* invert sel & snap & pivot */
    GetDlgItem(IDC_BUTTON_INVERTSEL)->EnableWindow( b_Sub );
    GetDlgItem(IDC_CHECK_SNAP)->EnableWindow( b_Sub );
    GetDlgItem(IDC_CHECK_SELWITHZ)->EnableWindow( b_Sub );
    GetDlgItem(IDC_CHECK_PIVOT)->EnableWindow( b_Sub );
    GetDlgItem(IDC_CHECK_MOVESUPDATEUV)->EnableWindow( b_Sub );
	GetDlgItem(IDC_CHECK_MOVESUPDATEMORPH)->EnableWindow( b_Sub );
    ((CButton *) GetDlgItem(IDC_CHECK_SNAP))->SetCheck(mpst_EditOption->ul_Flags & GRO_Cul_EOF_SnapVertex);
    ((CButton *) GetDlgItem(IDC_CHECK_SELWITHZ))->SetCheck(!(mpst_EditOption->ul_Flags & GRO_Cul_EOF_SelectVisible));
    ((CButton *) GetDlgItem(IDC_CHECK_PIVOT))->SetCheck(mpst_EditOption->ul_Flags & GRO_Cul_EOF_MovePivot);
    ((CButton *) GetDlgItem(IDC_CHECK_MOVESUPDATEUV))->SetCheck(mpst_EditOption->ul_Flags & GRO_Cul_EOF_VertexMoveUpdateUV);
	((CButton *) GetDlgItem(IDC_CHECK_MOVESUPDATEMORPH))->SetCheck(mpst_EditOption->ul_Flags & GRO_Cul_EOF_VertexMoveUpdateMorph);

    /* not magnifold object */
    b = FALSE; 
    if ( mpo_ToolBox->mpst_Gro && (mpo_ToolBox->mpst_Gro->i->ul_Type == GRO_Geometric) && ((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->pst_SubObject )
    {
        ((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->pst_SubObject->ul_Tool_Param = (ULONG) mpo_ToolBox;
        ((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->pst_SubObject->pfn_Tool_Update = EDIA_cl_ToolBoxDialog_Update;

        if ( b_Sub && (((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->pst_SubObject->ul_Flags & GEO_Cul_SOF_NonMagnifold) )
                b = TRUE;
    }
    GetDlgItem( IDC_BUTTON_NONMAGNIFOLD )->ShowWindow( b ? SW_SHOW : SW_HIDE );
    GetDlgItem( IDC_STATIC_NBSUBSEL )->ShowWindow( b ? SW_HIDE : SW_SHOW );

	/* display other warning : scale, colmap */
	if (!b && mpo_ToolBox->mpst_Gao)
	{
		b = MATH_b_TestScaleType( OBJ_pst_GetAbsoluteMatrix( mpo_ToolBox->mpst_Gao ));
		GetDlgItem( IDC_BUTTON_WARNINGSCALE )->ShowWindow( b ? SW_SHOW : SW_HIDE );

		b = mpo_ToolBox->mpst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap;
		GetDlgItem( IDC_BUTTON_WARNINGCOLMAP )->ShowWindow( b ? SW_SHOW : SW_HIDE );
	}
	else
	{
		GetDlgItem( IDC_BUTTON_WARNINGSCALE )->ShowWindow( SW_HIDE );
		GetDlgItem( IDC_BUTTON_WARNINGCOLMAP )->ShowWindow( SW_HIDE );
	}

    /* Vertex sub object controls */
	b = b_Sub && (mpst_EditOption->ul_Flags & GRO_Cul_EOF_Vertex);
    GetDlgItem(IDC_CHECK_PAINTVERTEX)->EnableWindow(b);
	GetDlgItem(IDC_CHECK_PAINTALPHAVERTEX)->EnableWindow(b);
	GetDlgItem(IDC_CHECK_SmoothSel)->EnableWindow(b);
	
	GetDlgItem(IDC_BUTTON_APPLYPAINTONSEL)->EnableWindow(b);
	GetDlgItem(IDC_CHECK_PICKVERTEXCOLOR)->EnableWindow(b);
	GetDlgItem(IDC_CHECK_WELDVERTEX)->EnableWindow(b);
	GetDlgItem(IDC_CHECK_VBUILDFACE)->EnableWindow(b);
    GetDlgItem(IDC_CHECK_SHOWEDGE)->EnableWindow(b);
	GetDlgItem(IDC_CHECK_VERTEXPICKRLI)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_CHOOSE_RLI_LOCATION)->EnableWindow(b);
	GetDlgItem(IDC_CHECK_VERTEXADJUSTRLI)->EnableWindow( SW_SHOW );
	GetDlgItem(IDC_BUTTON_WELDTHRESH)->EnableWindow(b);
	GetDlgItem(IDC_EDIT_WELDTHRESH)->EnableWindow(b);
    GetDlgItem(IDC_BUTTON_VSYM)->EnableWindow(b);
	i = mpst_EditOption->i_SymetrieOp;
	if ( (i < 0) || (i >= GRO_Cul_EOSYM_Number))
			i = mpst_EditOption->i_SymetrieOp = GRO_Cul_EOSYM_PivotLocal_X;
    GetDlgItem( IDC_BUTTON_VSYM )->SetWindowText( gasz_SymOpShortName[ i ] );

	/** "Work with RLI in ..." **/
	// Where is the RLI ?
	RLIloc = OBJ_i_RLIlocation(mpo_ToolBox->mpst_Gao);
	GetDlgItem(IDC_RADIO_RLIGAO)->EnableWindow(RLIloc & u8_RLIinGAO);
	GetDlgItem(IDC_RADIO_RLIGRO)->EnableWindow(RLIloc >= u8_RLIinGRO);
	// Check the right radio button
	if (mpo_ToolBox->mpo_View->mb_WorkWithGRORLI && (RLIloc < u8_RLIinGRO))
		mpo_ToolBox->mpo_View->mb_WorkWithGRORLI = FALSE;
	if ( !mpo_ToolBox->mpo_View->mb_WorkWithGRORLI && !(RLIloc & u8_RLIinGAO) && (RLIloc >= u8_RLIinGRO))
		mpo_ToolBox->mpo_View->mb_WorkWithGRORLI = TRUE;
	((CButton *) GetDlgItem(IDC_RADIO_RLIGRO))->SetCheck( mpo_ToolBox->mpo_View->mb_WorkWithGRORLI && (RLIloc >= u8_RLIinGRO));
	((CButton *) GetDlgItem(IDC_RADIO_RLIGAO))->SetCheck(!mpo_ToolBox->mpo_View->mb_WorkWithGRORLI && (RLIloc &  u8_RLIinGAO));

    ((CButton *) GetDlgItem(IDC_RADIO_VERTEX))->SetCheck(mpst_EditOption->ul_Flags & GRO_Cul_EOF_Vertex);
    ((CButton *) GetDlgItem(IDC_CHECK_PAINTVERTEX))->SetCheck(mpst_EditOption->i_VertexTool == GRO_i_EOT_VertexPaint);
    ((CButton *) GetDlgItem(IDC_CHECK_PAINTALPHAVERTEX))->SetCheck(mpst_EditOption->i_VertexTool == GRO_i_EOT_VertexPaintAlpha);
    ((CButton *) GetDlgItem(IDC_CHECK_SmoothSel))->SetCheck(SmoothSelIsActivate == 1);
	
    ((CButton *) GetDlgItem(IDC_CHECK_PICKVERTEXCOLOR))->SetCheck(mpst_EditOption->i_VertexTool == GRO_i_EOT_VertexPickColor);
    ((CButton *) GetDlgItem(IDC_CHECK_WELDVERTEX))->SetCheck(mpst_EditOption->i_VertexTool == GRO_i_EOT_VertexWeld);
    ((CButton *) GetDlgItem(IDC_CHECK_VBUILDFACE))->SetCheck(mpst_EditOption->i_VertexTool == GRO_i_EOT_VertexBuildFace);
    ((CButton *) GetDlgItem(IDC_CHECK_SHOWEDGE))->SetCheck(mpst_EditOption->ul_Flags & GRO_Cul_EOF_ShowEdge);

    /* edge sub ovject controls */
	b = b_Sub && (mpst_EditOption->ul_Flags & GRO_Cul_EOF_Edge);
    GetDlgItem(IDC_CHECK_TURNEDGE)->EnableWindow(b);
	GetDlgItem(IDC_CHECK_CUTEDGE)->EnableWindow(b);
#if defined(_XENON_RENDER)
    // Disable the edge cutter since it is unstable with the Xenon Renderer
	GetDlgItem(IDC_CHECK_ECUTTER)->EnableWindow(FALSE);
#else
    GetDlgItem(IDC_CHECK_ECUTTER)->EnableWindow(b);
#endif
	GetDlgItem(IDC_CHECK_CUTEDGE)->SetWindowText( (mpst_EditOption->ul_Flags & GRO_Cul_EOF_EdgeDivide) ? "DivBy2" : "Cut" );
	
    ((CButton *) GetDlgItem(IDC_RADIO_EDGE))->SetCheck(mpst_EditOption->ul_Flags & GRO_Cul_EOF_Edge);
    ((CButton *) GetDlgItem(IDC_CHECK_TURNEDGE))->SetCheck(mpst_EditOption->i_EdgeTool == GRO_i_EOT_EdgeTurn);
	((CButton *) GetDlgItem(IDC_CHECK_CUTEDGE))->SetCheck(mpst_EditOption->i_EdgeTool == GRO_i_EOT_EdgeCut);
	((CButton *) GetDlgItem(IDC_CHECK_ECUTTER))->SetCheck(mpst_EditOption->i_EdgeTool == GRO_i_EOT_EdgeCutter);

    /* face sub object controls */
	b = b_Sub && (mpst_EditOption->ul_Flags & GRO_Cul_EOF_Face);
    GetDlgItem(IDC_BUTTON_FSELREJECTION)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_FLIPNORMALS)->EnableWindow(b);
	GetDlgItem(IDC_CHECK_UV)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_FACEID)->EnableWindow(b);
	GetDlgItem(IDC_EDIT_ID)->EnableWindow(b);
	GetDlgItem(IDC_SPIN_ID)->EnableWindow(b);
	GetDlgItem(IDC_CHECK_LOCKID)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_FSELEXT)->EnableWindow(b);
    GetDlgItem(IDC_BUTTON_FSELEXT)->SetWindowText( gasz_FaceSelName[ (mpst_EditOption->ul_Flags & GRO_Cul_EOF_FaceSelMask) >> GRO_Cul_EOF_FaceSelShift ] );
	GetDlgItem(IDC_BUTTON_DETACHFACE)->EnableWindow(b);
    GetDlgItem(IDC_BUTTON_FATTACH)->EnableWindow(b);
    GetDlgItem(IDC_CHECK_FHIDE)->EnableWindow(b);	
    GetDlgItem(IDC_CHECK_FUNHIDE)->EnableWindow(b);	
    GetDlgItem(IDC_CHECK_FCUT)->EnableWindow(b);
    GetDlgItem(IDC_CHECK_FACEPICKUV)->EnableWindow(b);
	
    ((CButton *) GetDlgItem(IDC_RADIO_FACE))->SetCheck(mpst_EditOption->ul_Flags & GRO_Cul_EOF_Face);
    ((CButton *) GetDlgItem(IDC_CHECK_UV))->SetCheck(mpo_UVMapper != NULL);

	((CButton *) GetDlgItem(IDC_CHECK_LOCKID))->SetCheck(mpst_EditOption->ul_Flags & GRO_Cul_EOF_FaceLockId);
	((CButton *) GetDlgItem(IDC_CHECK_FCUT))->SetCheck(mpst_EditOption->i_FaceTool == GRO_i_EOT_FaceCut);

    /* number of faces */
	if ( b_Sub && (mpo_ToolBox->mpst_Gro) && (mpo_ToolBox->mpst_Gro->i->ul_Type == GRO_Geometric) && (((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->pst_SubObject) )
		GetDlgItem(IDC_STATIC_NBFACES)->SetWindowText(_itoa(((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->pst_SubObject->ul_NbFaces, sz_Value, 10));
    else
        GetDlgItem(IDC_STATIC_NBFACES)->SetWindowText("");

    /* face selection rejection type */
    if (mpst_EditOption->ul_Flags & GRO_Cul_EOF_FacePickingBufferTest)
        GetDlgItem( IDC_BUTTON_FSELREJECTION )->SetWindowText( "Sel (C)" );
    else if (mpst_EditOption->ul_Flags & GRO_Cul_EOF_FaceBackfaceTest)
        GetDlgItem( IDC_BUTTON_FSELREJECTION )->SetWindowText( "Sel (N)" );
    else
        GetDlgItem( IDC_BUTTON_FSELREJECTION )->SetWindowText( "Sel all" );

    /* skin */
    b = b_Sub && (mpo_ToolBox->mpst_Gro);
    GetDlgItem(IDC_CHECK_EDITSKIN)->EnableWindow( b );
    ((CButton *) GetDlgItem(IDC_CHECK_EDITSKIN))->SetCheck( mpst_EditOption->ul_Flags & GRO_Cul_EOF_SkinMode );

    b = b && GEO_SKN_IsSkinned((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro);
    GetDlgItem(IDC_CHECK_EDITSKIN)->SetWindowText( b ? "Edit" : "Create && Edit" );
    GetDlgItem(IDC_BUTTON_ADJ_SYM)->EnableWindow( b );
    GetDlgItem(IDC_BUTTON_DESTROYSKIN)->EnableWindow( b );

    /* strip control */
	if((mpo_ToolBox->mpst_Gro) && !b_Sub)
	{
		/* if stripped */
		if(((GEO_tdst_Object *) (mpo_ToolBox->mpst_Gro))->ulStripFlag & GEO_C_Strip_DataValid)
		{
			/* enable all objects */
			GetDlgItem(IDC_BUTTON_STRIP)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_UNSTRIP)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_SHOWSTRIP)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_STRIPSTAT)->EnableWindow(TRUE);
			GetDlgItem(IDC_STATIC_STRIPTEXT)->EnableWindow(TRUE);

			/* if display strip */
			if(((GEO_tdst_Object *) (mpo_ToolBox->mpst_Gro))->ulStripFlag & GEO_C_Strip_DisplayStrip)
				((CButton *) GetDlgItem(IDC_CHECK_SHOWSTRIP))->SetCheck(TRUE);
			else
				((CButton *) GetDlgItem(IDC_CHECK_SHOWSTRIP))->SetCheck(FALSE);

			/* if display stat */
			if(((GEO_tdst_Object *) (mpo_ToolBox->mpst_Gro))->ulStripFlag & GEO_C_Strip_StatData)
			{
				((CButton *) GetDlgItem(IDC_CHECK_STRIPSTAT))->SetCheck(TRUE);
			}
			else
			{
				((CButton *) GetDlgItem(IDC_CHECK_STRIPSTAT))->SetCheck(FALSE);
				GetDlgItem(IDC_STATIC_STRIPTEXT)->SetWindowText("");
			}
		}
		else
		{
			GetDlgItem(IDC_BUTTON_STRIP)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_UNSTRIP)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_SHOWSTRIP)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_STRIPSTAT)->EnableWindow(FALSE);
			GetDlgItem(IDC_STATIC_STRIPTEXT)->EnableWindow(FALSE);
			GetDlgItem(IDC_STATIC_STRIPTEXT)->SetWindowText("");
		}
	}
	else
	{
		GetDlgItem(IDC_BUTTON_STRIP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_UNSTRIP)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_SHOWSTRIP)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_STRIPSTAT)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_STRIPTEXT)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_STRIPTEXT)->SetWindowText("");
	}

    /* MRM */
    b = (!b_Sub) && mpo_ToolBox->mpst_Gao && mpo_ToolBox->mpst_Gro && (mpo_ToolBox->mpst_Gro->i->ul_Type == GRO_Geometric);

    // Switch Radio button for MRM.
    ((CButton *) GetDlgItem(IDC_RADIO_NO_MRM))->SetCheck(eMRMState == GEO_MRM_None);
    ((CButton *) GetDlgItem(IDC_RADIO_MRM_CURVE))->SetCheck(eMRMState == GEO_MRM_Curve);
    ((CButton *) GetDlgItem(IDC_RADIO_MRM_LOD))->SetCheck(eMRMState == GEO_MRM_LOD);

    GetDlgItem( IDC_EDIT_MRM_NUMPOINTS )->EnableWindow( b && (eMRMState != GEO_MRM_None) );
    GetDlgItem( IDC_SLIDER_MRM_DISTANCE  )->EnableWindow( b && (eMRMState == GEO_MRM_LOD) );
    GetDlgItem( IDC_SLIDER_MRM_LOD_NB  )->EnableWindow( b && (eMRMState == GEO_MRM_LOD) );

    if (b)
    {
        if (eMRMState == GEO_MRM_Curve)
            iPointNb = GEO_MRM_GetNumberOfPoints_Curved( (GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, GEO_MRM_GetFloatFromUC((ULONG) mpo_ToolBox->mpst_Gao->uc_LOD_Vis) );
        else if (eMRMState == GEO_MRM_LOD)
            iPointNb = GEO_MRM_GetNumberOfPoints_CurvedLevels((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro,GEO_MRM_GetFloatFromUC((ULONG) mpo_ToolBox->mpst_Gao->uc_LOD_Vis));
        else
            iPointNb = 0;
    }
    else
        iPointNb = 0;

#ifdef JADEFUSION
#if !defined(_GLV_KEEP_LAST)
    // SC: No need for adjust radiosity if we are not keeping the result after a compute
    GetDlgItem(IDC_BUTTONADJUSTRAD)->EnableWindow(FALSE);
#endif

#if !defined(_GLV_KEEP_KILT)
    // SC: No need for the show compute button since we are not keeping the debug mesh to draw
    GetDlgItem(IDC_CHECK_SHOWRADRES)->EnableWindow(FALSE);
#endif

    BOOL bEnableXMP = GDI_b_IsXenonGraphics() && !b_Sub && IsAtLeastOneGAOSelected();
    GetDlgItem(IDC_EGRO_XE_PROCESS)->EnableWindow(bEnableXMP);
    GetDlgItem(IDC_EGRO_XE_REVERT)->EnableWindow(bEnableXMP);
    GetDlgItem(IDC_EGRO_XE_EDIT_CREATE)->EnableWindow(bEnableXMP);
    GetDlgItem(IDC_EGRO_XE_EDIT_REMOVE)->EnableWindow(bEnableXMP);
    GetDlgItem(IDC_EGRO_XE_FIXEDGES)->EnableWindow(bEnableXMP);
#endif

    SetDlgItemInt( IDC_EDIT_MRM_NUMPOINTS, iPointNb );

    UpdateControls_LOD();
#ifdef JADEFUSION
	Update_MergePS2Data();
#endif
	M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::UpdateControls_LOD(void)
{
    OBJ_tdst_GameObject *GO;
    BOOL                b;
    int                 i, j;
    char                sz_Value[16];

    mpst_LOD = NULL;

    b = (mpst_EditOption->ul_Flags & GRO_Cul_EOF_SubObject) || (!mpo_ToolBox->mpst_Gro) || (!mpo_ToolBox->mpst_Gao);
    if (b || (mpo_ToolBox->mpst_Gro->i->ul_Type != GRO_GeoStaticLOD) )
    {
        GetDlgItem(IDC_BUTTON_LODTRANSFORM)->EnableWindow( FALSE );
        for ( i = 0; i < 6; i++)
        {
            GetDlgItem( LODCTRL_Name[i] )->SetWindowText( "" );
            GetDlgItem( LODCTRL_Name[i] )->EnableWindow( FALSE );
            GetDlgItem( LODCTRL_Lock[i] )->ShowWindow( SW_HIDE );
        }

        b = !b && (mpo_ToolBox->mpst_Gro->i->ul_Type == GRO_Geometric);
        if (b) GetDlgItem( IDC_BUTTON_LODTRANSFORM )->SetWindowText( "Transform to LOD" );
        GetDlgItem(IDC_BUTTON_LODTRANSFORM)->EnableWindow( b );
        return;
    }

    mpst_LOD = (GEO_tdst_StaticLOD *) mpo_ToolBox->mpst_Gro;
    LOD_Clean();

    GO = mpo_ToolBox->mpst_Gao;
    j = (GO->ul_EditorFlags & OBJ_C_EditFlags_ForceLOD) ? GO->ul_ForceLODIndex : -1;

    for (i = 0; i < mpst_LOD->uc_NbLOD; i++)
    {
        GetDlgItem( LODCTRL_Name[i] )->EnableWindow( TRUE );
        GetDlgItem( LODCTRL_Name[i] )->SetWindowText( mpst_LOD->dpst_Id[i] ? mpst_LOD->dpst_Id[i]->sz_Name : "Bad data" );
        GetDlgItem( LODCTRL_Lock[i] )->ShowWindow( SW_SHOW );
        ((CButton *) GetDlgItem( LODCTRL_Lock[i] ))->SetBitmap( (HBITMAP) mao_Cadenas[ (j == i) ? 0 : 1] );
        GetDlgItem( LODCTRL_Dist[i] )->ShowWindow( SW_SHOW );
        GetDlgItem( LODCTRL_Dist[i] )->SetWindowText( _itoa( mpst_LOD->auc_EndDistance[i], sz_Value, 10) );
    }

    for (; i < 6; i++)
    {
        GetDlgItem( LODCTRL_Name[i] )->EnableWindow( FALSE );
        GetDlgItem( LODCTRL_Name[i] )->SetWindowText( "" );
        GetDlgItem( LODCTRL_Lock[i] )->ShowWindow( SW_HIDE );
        GetDlgItem( LODCTRL_Dist[i] )->ShowWindow( SW_HIDE );
    }

    if (mpst_LOD->uc_NbLOD < 6)
        GetDlgItem( LODCTRL_Name[ mpst_LOD->uc_NbLOD ])->EnableWindow( TRUE );

    GetDlgItem(IDC_BUTTON_LODTRANSFORM)->EnableWindow( TRUE );
    GetDlgItem( IDC_BUTTON_LODTRANSFORM )->SetWindowText( "Transform to Geo" );
}
#ifdef JADEFUSION
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::Update_MergePS2Data()
{
	if ( mpo_XeMergeFlags )
	{
		mpo_XeMergeFlags->Refresh(mpo_ToolBox->mpst_Gao);
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ToolBox_GROView::Remove_MergePS2Data()
{
	if ( mpo_XeMergeFlags )
	{
		mpo_XeMergeFlags = NULL;
	}
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_ToolBox_GROView::b_CanDrop( POINT pt )
{
    CRect o_Rect;

    if (!mpst_LOD) return FALSE;

    for (mi_LODDrop = 0; mi_LODDrop < 6; mi_LODDrop++)
    {
        GetDlgItem( LODCTRL_Name[mi_LODDrop] )->GetWindowRect((LPRECT) & o_Rect);
        if (o_Rect.PtInRect( pt ) )
            return TRUE;
    }
    mi_LODDrop = -1;
    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::DropGro( BIG_INDEX _ul_Index )
{
    GRO_tdst_Struct *pst_Gro;

    if ((mpst_LOD == NULL) || (mi_LODDrop < 0) || (mi_LODDrop >= 6) ) return;

    pst_Gro = (GRO_tdst_Struct *) LOA_ul_SearchAddress( BIG_PosFile( _ul_Index ) ); 
    if ((ULONG) pst_Gro == (ULONG) -1)
    {
        LOA_MakeFileRef( BIG_FileKey(_ul_Index), (ULONG *) &pst_Gro, GEO_ul_Load_ObjectCallback, LOA_C_MustExists);
        LOA_Resolve();
    }
    LOD_InsertGeo( mi_LODDrop, pst_Gro );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::DropGao( BIG_INDEX _ul_Index )
{
    ULONG               ul_Pos;
    OBJ_tdst_GameObject *pst_GO;

    if ((mpst_LOD == NULL) || (mi_LODDrop < 0) || (mi_LODDrop >= 6) ) return;

    ul_Pos = BIG_PosFile( _ul_Index );
    if (ul_Pos == (ULONG) - 1) 
        return;

    pst_GO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress( BIG_PosFile( _ul_Index ) );
    if ( !pst_GO->pst_Base) return;
    if ( !pst_GO->pst_Base->pst_Visu ) return;

    LOD_InsertGeo( mi_LODDrop, pst_GO->pst_Base->pst_Visu->pst_Object );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::UpdateNumberOfSubSel( void )
{
	SEL_tdst_SelectedItem *pst_Sel;
    char    *pc_Sel, *pc_Last;
    LONG    l_Number;
    char    sz_Value[16];
    

	pst_Sel = SEL_pst_GetFirst(mpo_ToolBox->mpo_View->mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object | SEL_C_SIF_Cob);

	if(pst_Sel && (pst_Sel->l_Flag & SEL_C_SIF_Object))
		pc_Sel = GEO_pc_SubObject_GetSelData( (GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, mpst_EditOption->ul_Flags, &l_Number);
	else
	{
		if(pst_Sel && (pst_Sel->l_Flag & SEL_C_SIF_Cob))
		{
			OBJ_tdst_GameObject *pst_GO;
			GEO_tdst_Object		*pst_Obj;

			pst_GO = ((COL_tdst_Cob *) pst_Sel->p_Content)->pst_GO;
			if(((COL_tdst_Cob *) pst_Sel->p_Content)->p_GeoCob)
				pst_Obj = (GEO_tdst_Object *) (((COL_tdst_Cob *) pst_Sel->p_Content)->p_GeoCob);
			else
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);

			pc_Sel = GEO_pc_SubObject_GetSelData( (GEO_tdst_Object *) pst_Obj, mpst_EditOption->ul_Flags, &l_Number);
		}
		else
		{
			pc_Sel = NULL;
		}
	}

    if (!pc_Sel) 
        l_Number = 0;
    else
    {
        pc_Last = pc_Sel + l_Number;
        l_Number = 0;
        while (pc_Sel < pc_Last)
            if ( *pc_Sel++ & 1 ) l_Number++;
    }
    GetDlgItem( IDC_STATIC_NBSUBSEL )->SetWindowText( _itoa( l_Number, sz_Value, 10 ) );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_ToolBox_GROView::IsLODButton( HWND hWnd )
{
    int i;

    if (mpst_LOD == NULL) return -1;

    for (i = 0; i < 6; i++)
    {
        if ( !GetDlgItem( LODCTRL_Name[i] )->IsWindowEnabled() ) return -1;
        if ( hWnd == GetDlgItem( LODCTRL_Name[i] )->GetSafeHwnd() ) return i;
    }
    return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_ToolBox_GROView::IsInLODDistCtrl( void )
{
    CRect   o_Rect;
    POINT   st_Pt;
    int     i;

    if (mpst_LOD == NULL) return -1;

    GetCursorPos( &st_Pt );

    for (i = 0; i < 6; i++)
    {
        if ( !GetDlgItem( LODCTRL_Name[i] )->IsWindowEnabled() ) return -1;

        GetDlgItem( LODCTRL_Dist[i] )->GetWindowRect( (LPRECT) &o_Rect );
        if (o_Rect.PtInRect( st_Pt ) )
            return i;
    }
    return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::LOD_Clean( void )
{
    int i, j;

    if (mpst_LOD == NULL) return;

    for (i = 0, j = 0; i < mpst_LOD->uc_NbLOD; i++)
    {
        if (mpst_LOD->dpst_Id[i] == NULL) continue;
        if (i != j)
        {
            mpst_LOD->dpst_Id[j] = mpst_LOD->dpst_Id[i];
            mpst_LOD->auc_EndDistance[j] = mpst_LOD->auc_EndDistance[i];
        }
        j++;
    }

    mpst_LOD->uc_NbLOD = j;

    for (; j < 6; j++)
    {
        mpst_LOD->dpst_Id[j] = NULL;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::LOD_InsertGeo( int _i_LOD, GRO_tdst_Struct *_pst_Gro )
{
    TAB_tdst_Ptable	*pst_PTable;
    int             i;

    if (!mpst_LOD ) return;
    if ( (_i_LOD < 0) || (_i_LOD >= 6)) return;
    if ( (_pst_Gro == NULL) || ( (ULONG) _pst_Gro == (ULONG) -1) )return;
    if (_pst_Gro->i->ul_Type != GRO_Geometric ) return;

    for (i = 5; i > _i_LOD; i--)
    {
        mpst_LOD->dpst_Id[i] = mpst_LOD->dpst_Id[ i - 1];
    }
    mpst_LOD->dpst_Id[ _i_LOD ] = _pst_Gro;
    _pst_Gro->i->pfn_AddRef( _pst_Gro, 1 );

    pst_PTable = &((WOR_tdst_World *) mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_World)->st_GraphicObjectsTable;
    if(TAB_ul_Ptable_GetElemIndexWithPointer(pst_PTable, _pst_Gro) == TAB_Cul_BadIndex)
	{
	    TAB_Ptable_AddElemAndResize(pst_PTable, _pst_Gro);
		_pst_Gro->i->pfn_AddRef(_pst_Gro, 1);
    }

    mpst_LOD->uc_NbLOD++;

    LOD_Clean();
    UpdateControls_LOD();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::UpdateFaceId(int i)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	sz_Value[10];
	/*~~~~~~~~~~~~~~~~~*/

	if((mpst_EditOption->ul_Flags & GRO_Cul_EOF_FaceLockId) == 0)
		GetDlgItem(IDC_EDIT_ID)->SetWindowText(_itoa(i, sz_Value, 10));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::EndCurTool(void)
{
	mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_Tool;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::BeginCurTool(void)
{
	if(mpst_EditOption->i_VertexTool == GRO_i_EOT_NoTool)
	{
		mpst_EditOption->ul_Flags |= GRO_Cul_EOF_ToolAcceptMultiple;
		return;
	}

	if(mpst_EditOption->ul_Flags & GRO_Cul_EOF_Vertex)
	{
		switch(mpst_EditOption->i_VertexTool)
		{
		case GRO_i_EOT_VertexPaint:
		case GRO_i_EOT_VertexPaintAlpha:
		case GRO_i_EOT_VertexPickColor:
			mpst_EditOption->ul_Flags |= GRO_Cul_EOF_ToolWhileMouseMove | GRO_Cul_EOF_ToolAcceptMultiple;
			break;
		case GRO_i_EOT_VertexWeld:
			mpst_EditOption->ul_Flags |= GRO_Cul_EOF_ToolAcceptMultiple;
			break;
		}
	}
	else if(mpst_EditOption->ul_Flags & GRO_Cul_EOF_Edge)
	{
        if (mpst_EditOption->i_EdgeTool == 0)
            mpst_EditOption->ul_Flags |= GRO_Cul_EOF_ToolAcceptMultiple;
	}
	else if(mpst_EditOption->ul_Flags & GRO_Cul_EOF_Face)
	{
		mpst_EditOption->ul_Flags |= GRO_Cul_EOF_ToolAcceptMultiple;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::Refresh(void)
{
	if(!mpst_EditOption) return;
	if(mpst_EditOption->ul_VertexPaintColor != ul_DisplayedColor)
		GetDlgItem(IDC_BUTTON_COLOR)->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

/*$4
 ***********************************************************************************************************************
    Message functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_ToolBox_GROView::PreTranslateMessage(MSG *pMsg)
{
    char					sz_Value[10];
	int						i_Button, i_Res;
    POINT					pt;
    CRect					o_Rect;
    MATH_tdst_Vector		v_Pos;
    OBJ_tdst_GameObject		*pst_GO;
    SEL_tdst_SelectedItem	*pst_Sel;

	if(pMsg->hwnd == GetDlgItem(IDC_BUTTON_COLORBEND)->GetSafeHwnd())
	{
		if(pMsg->message == WM_MOUSEMOVE)
		{
			OnMouseMove(0, 0);
			return 1;
		}

		if(pMsg->message == WM_LBUTTONDOWN)
		{
			OnLButtonDown(0,0);
			return 1;
		}

		if(pMsg->message == WM_LBUTTONUP)
		{
			OnLButtonUp(0,0);
			return 1;
		}
	}
	else if(pMsg->hwnd == GetDlgItem(IDC_SPIN_ID)->GetSafeHwnd())
	{
		if(pMsg->message == WM_LBUTTONUP)
		{
			OnEdit_Id();
		}
	}
	else if(pMsg->hwnd == GetDlgItem(IDC_CHECK_VERTEXPICKRLI)->GetSafeHwnd())
	{
		if(pMsg->message == WM_LBUTTONDOWN)
		{
			((CButton *) GetDlgItem(IDC_CHECK_VERTEXPICKRLI))->SetCheck(1);
			mi_Pick = 1;
            ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PICK));
			SetCapture();
		}

		if(pMsg->message == WM_LBUTTONUP)
		{
			OnLButtonUp(0,0);
			return 1;
		}
	}
	else if(pMsg->hwnd == GetDlgItem(IDC_CHECK_FACEPICKUV)->GetSafeHwnd())
	{
		if(pMsg->message == WM_LBUTTONDOWN)
		{
			((CButton *) GetDlgItem(IDC_CHECK_FACEPICKUV))->SetCheck(1);
			mi_Pick = 2;
            ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PICK));
			SetCapture();
		}

		if(pMsg->message == WM_LBUTTONUP)
		{
			OnLButtonUp(0,0);
			return 1;
		}
	}
	/*else if ( (pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem( IDC_BUTTON_COLOR )->GetSafeHwnd() ) )
	{
		EDIA_cl_StockOfColorDialog *po_Dlg;
		CRect						o_Rect;
		
		GetDlgItem( IDC_BUTTON_COLOR )->GetWindowRect( &o_Rect );
		
		po_Dlg = new EDIA_cl_StockOfColorDialog( mpst_EditOption->ul_ColorStock, mpst_EditOption->ul_VertexPaintColor, o_Rect );
		if ( po_Dlg->DoModal() == IDOK )
		{
			mpst_EditOption->ul_VertexPaintColor = po_Dlg->mul_Res_Color;
			GetDlgItem(IDC_BUTTON_COLOR)->RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}

		delete po_Dlg;
		return 1;
	}*/
	else if ( (pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem( IDC_CHECK_PIVOT )->GetSafeHwnd() ) )
    {
		EMEN_cl_SubMenu o_Menu(FALSE);

        M_MF()->InitPopupMenuAction(NULL, &o_Menu);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Center pivot ( BV )", -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Center pivot ( Barycentre )", -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Set pivot pos", -1);
        
        GetCursorPos( &pt );
		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		switch(i_Res)
		{
        case 1: 
			mpo_ToolBox->mpo_View->mpo_AssociatedEditor->i_OnMessage(EOUT_MESSAGE_SUBOBJECT_PIVOTCENTER, 0, 0);
			LINK_Refresh();
            break;
        case 2: 
			mpo_ToolBox->mpo_View->mpo_AssociatedEditor->i_OnMessage(EOUT_MESSAGE_SUBOBJECT_PIVOTCENTER, 1, 0);
			LINK_Refresh();
            break;
        case 3: 
			MATH_InitVector( &v_Pos, 0, 0, 0);
			pst_Sel = SEL_pst_GetFirst(mpo_ToolBox->mpo_View->mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object );
			if ( pst_Sel && (pst_GO = (OBJ_tdst_GameObject *) pst_Sel->p_Content) )
			{
			    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				EDIA_cl_VectorDialog    o_Dlg;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				/* Click on button ? */
				MATH_CopyVector(&o_Dlg.mst_Vector, OBJ_pst_GetAbsolutePosition( pst_GO ) );
				if ( !o_Dlg.DoModal() ) return 1;
				MATH_CopyVector(&v_Pos, &o_Dlg.mst_Vector);
			}
			mpo_ToolBox->mpo_View->mpo_AssociatedEditor->i_OnMessage(EOUT_MESSAGE_SUBOBJECT_PIVOTCENTER, 2, (ULONG) &v_Pos  );
			LINK_Refresh();
            break;
        }
        return 1;
    }
    else if ( (pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem( IDC_CHECK_COLORMODE )->GetSafeHwnd() ) )
    {
		EMEN_cl_SubMenu o_Menu(FALSE);

        M_MF()->InitPopupMenuAction(NULL, &o_Menu);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "no color mode", -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, gasz_ColorModeName[ 1 ], -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, gasz_ColorModeName[ 2 ], -1);
        
        GetCursorPos( &pt );
		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		switch(i_Res)
		{
        case 1: 
			mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_ColorModeMask;
			((CButton *) GetDlgItem( IDC_CHECK_COLORMODE ))->SetCheck( 0 );
			GetDlgItem( IDC_CHECK_COLORMODE )->SetWindowText( gasz_ColorModeName[ 0 ] );
			LINK_Refresh();
            break;
        case 2: 
			mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_ColorModeMask;
			mpst_EditOption->ul_Flags |= GRO_Cul_EOF_ColorModeFaceID;
			((CButton *) GetDlgItem( IDC_CHECK_COLORMODE ))->SetCheck( 1 );
			GetDlgItem( IDC_CHECK_COLORMODE )->SetWindowText( gasz_ColorModeName[ 1 ] );
			LINK_Refresh();
            break;
        case 3: 
			mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_ColorModeMask;
			mpst_EditOption->ul_Flags |= GRO_Cul_EOF_ColorModeAlpha;
			((CButton *) GetDlgItem( IDC_CHECK_COLORMODE ))->SetCheck( 1 );
			GetDlgItem( IDC_CHECK_COLORMODE )->SetWindowText( gasz_ColorModeName[ 2 ] );
			LINK_Refresh();
            break;
        }
        return 1;
    }
    else if ( (pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem( IDC_BUTTON_VSYM)->GetSafeHwnd() ) )
    {
        EMEN_cl_SubMenu o_Menu(FALSE);

        M_MF()->InitPopupMenuAction(NULL, &o_Menu);
		for (i_Res = 0; i_Res < GRO_Cul_EOSYM_Number; i_Res++)
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, i_Res + 1, TRUE, gasz_SymOpLongName[ i_Res ], -1);
        
        GetCursorPos( &pt );
		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		if (i_Res > 0 )
		{
			mpst_EditOption->i_SymetrieOp = i_Res - 1;
			GetDlgItem( IDC_BUTTON_VSYM )->SetWindowText( gasz_SymOpShortName[ i_Res - 1 ] );
		}
        return 1;
    }
    else if ( (pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem( IDC_BUTTON_FSELREJECTION )->GetSafeHwnd() ) )
    {
        EMEN_cl_SubMenu o_Menu(FALSE);

        M_MF()->InitPopupMenuAction(NULL, &o_Menu);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "No rejection", -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Rejection by normals", -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Rejection by culling", -1);
        
        GetCursorPos( &pt );
		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		switch(i_Res)
		{
        case 1: 
            mpst_EditOption->ul_Flags &= ~(GRO_Cul_EOF_FaceBackfaceTest | GRO_Cul_EOF_FacePickingBufferTest);
            GetDlgItem( IDC_BUTTON_FSELREJECTION )->SetWindowText( "Sel all" );
            break;
        case 2:
            mpst_EditOption->ul_Flags &= ~(GRO_Cul_EOF_FaceBackfaceTest | GRO_Cul_EOF_FacePickingBufferTest);
            mpst_EditOption->ul_Flags |= GRO_Cul_EOF_FaceBackfaceTest;
            GetDlgItem( IDC_BUTTON_FSELREJECTION )->SetWindowText( "Sel (N)" );
            break;
        case 3:
            mpst_EditOption->ul_Flags &= ~(GRO_Cul_EOF_FaceBackfaceTest | GRO_Cul_EOF_FacePickingBufferTest);
            mpst_EditOption->ul_Flags |= GRO_Cul_EOF_FacePickingBufferTest;
            GetDlgItem( IDC_BUTTON_FSELREJECTION )->SetWindowText( "Sel (C)" );
            break;
        }
        return 1;
    }
    else if ( (pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem( IDC_BUTTON_FSELEXT)->GetSafeHwnd() ) )
    {
        EMEN_cl_SubMenu o_Menu(FALSE);

        M_MF()->InitPopupMenuAction(NULL, &o_Menu);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Select near", -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "Select near by edge", -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Select extend", -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Select extend by UV", -1);
        
        GetCursorPos( &pt );
		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		if ( ( i_Res >= 1) && ( i_Res <= 4) )
		{
			mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_FaceSelMask;
            mpst_EditOption->ul_Flags |= (i_Res - 1) << GRO_Cul_EOF_FaceSelShift;
			GetDlgItem(IDC_BUTTON_FSELEXT)->SetWindowText( gasz_FaceSelName[ (mpst_EditOption->ul_Flags & GRO_Cul_EOF_FaceSelMask) >> GRO_Cul_EOF_FaceSelShift ] );
		}
	    return 1;
    }
    else if ( (pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem( IDC_CHECK_FHIDE )->GetSafeHwnd() ) )
    {
        EMEN_cl_SubMenu o_Menu(FALSE);

        M_MF()->InitPopupMenuAction(NULL, &o_Menu);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Hide unselected", -1);
        
        GetCursorPos( &pt );
		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		if ( i_Res == 1)
		{
			mpo_ToolBox->mpo_View->Selection_SubObject_HideFace( TRUE );
		}
	    return 1;
    }
    else if ( (pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem( IDC_CHECK_CUTEDGE )->GetSafeHwnd() ) )
    {
        EMEN_cl_SubMenu o_Menu(FALSE);

        M_MF()->InitPopupMenuAction(NULL, &o_Menu);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Cut (mouse location)", -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Divide edge by 2", -1);
        
        GetCursorPos( &pt );
		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		switch(i_Res)
		{
        case 1: 
            mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_EdgeDivide;
            GetDlgItem( IDC_CHECK_CUTEDGE )->SetWindowText( "Cut" );
            break;
        case 2:
            mpst_EditOption->ul_Flags |= GRO_Cul_EOF_EdgeDivide;
            GetDlgItem( IDC_CHECK_CUTEDGE )->SetWindowText( "DivBy2" );
            break;
        }
        return 1;
    }
    else if ( (pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem( IDC_BUTTON_WELDTHRESH )->GetSafeHwnd() ) )
    {
        EMEN_cl_SubMenu o_Menu(FALSE);

        M_MF()->InitPopupMenuAction(NULL, &o_Menu);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Weld selected vertex", -1);
        
        GetCursorPos( &pt );
		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		switch(i_Res)
		{
        case 1: 
			mpo_ToolBox->mpo_View->Selection_SubObject_VertexWeldThresh( Cf_Infinit );
			LINK_Refresh();
            break;
        }
        return 1;
    }
    else if ( mi_LODCapture || ( (i_Button = IsLODButton( pMsg->hwnd )) != -1 ) )
    {
        GetCursorPos( &pt );
#ifdef JADEFUSION
		if (GDI_b_IsXenonGraphics() && ((GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0x8000))
            return 0;
#endif
        /*$F
        if (pMsg->message == WM_RBUTTONDOWN)
        {
            EMEN_cl_SubMenu o_Menu(FALSE);

            M_MF()->InitPopupMenuAction(NULL, &o_Menu);
            M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Set", -1);
            M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Insert", -1);
            if (mpst_LOD->uc_NbLOD > 1) M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Delete", -1);
            
		    i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		    switch(i_Res)
		    {
            case 3: 
                GEO_StaticLOD_Delete( (GEO_tdst_StaticLOD *) mpo_ToolBox->mpst_Gro, i_Button );
                UpdateControls();
                LINK_Refresh();
                break;
            }
            return 1;
        }
        else*/
        if (pMsg->message == WM_LBUTTONDOWN)
        {
            mi_LODCapture = 1;
            mi_LOD = i_Button;
            mo_LODPt = CPoint( pt );
            SetCapture();
            return 1;
        }
        else if (pMsg->message == WM_LBUTTONUP)
        {
            if (mi_LODCapture == 0) return 0;
            ReleaseCapture();
            ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

            if (mi_LODCapture == 1)
            {
            }
            else if (mi_LODCapture == 2)
            {
                if (mi_LODTo == -1)
                {
                    GEO_StaticLOD_Delete( (GEO_tdst_StaticLOD *) mpo_ToolBox->mpst_Gro, mi_LOD );
                    UpdateControls();
                    LINK_Refresh();
                }
                else if (mi_LOD != mi_LODTo)
                {   
                    GRO_tdst_Struct *pst_Gro;

#if defined(_XENON_RENDER)
                    GEO_CleanGameObjectPack(mpo_ToolBox->mpst_Gao);
#endif
                    pst_Gro = mpst_LOD->dpst_Id[ mi_LODTo ];
                    mpst_LOD->dpst_Id[ mi_LODTo ] = mpst_LOD->dpst_Id[ mi_LOD ];
                    mpst_LOD->dpst_Id[ mi_LOD ] = pst_Gro;

                    if (mpst_LOD->uc_NbLOD <= mi_LODTo)
                        mpst_LOD->uc_NbLOD = mi_LODTo + 1;

#if defined(_XENON_RENDER)
                    GEO_PackGameObject(mpo_ToolBox->mpst_Gao);
#endif
                    LOD_Clean();
                    UpdateControls();
                    LINK_Refresh();
                }
            }
            mi_LODCapture = 0;
            return 1;
        }
        else if (mi_LODCapture && (pMsg->message == WM_MOUSEMOVE))
        {
            if (mi_LOD == mpst_LOD->uc_NbLOD)
            {
                mi_LODCapture = 0;
                ReleaseCapture();
                return 1;
            }

            mi_LODCapture = 2;
            GetCursorPos(&pt);
            for (mi_LODTo = 0; mi_LODTo < 6; mi_LODTo++)
            {
                GetDlgItem( LODCTRL_Name[mi_LODTo] )->GetWindowRect((LPRECT) & o_Rect);
                if (o_Rect.PtInRect( pt) )
                {
                    ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGMOVE));
                    break;
                }
            }
            if (mi_LODTo==6)
            {
                mi_LODTo = -1;
                ::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGDEL));
            }
        }
        return 1;
    }
    else if ( ( pMsg->message == WM_RBUTTONDOWN) && ( (i_Button = IsInLODDistCtrl() ) != -1 ) )
    {
        mi_LODDistCapture = 1;
        mi_LOD = i_Button;
        mi_LODTo = mpst_LOD->auc_EndDistance[ mi_LOD ];
        SetCapture();
        GetCursorPos( &mo_LODPt );
        return 1;
    }
    else if (mi_LODDistCapture == 1)
    {
        if (pMsg->message == WM_MOUSEMOVE)
        {
            GetCursorPos( &pt );
            i_Res = mi_LODTo + ((pt.y - mo_LODPt.y) >> 1);
            mpst_LOD->auc_EndDistance[ mi_LOD ] = (i_Res <= 0) ? 0 : (i_Res > 255) ? 255 : (char) i_Res;
            GetDlgItem( LODCTRL_Dist[ mi_LOD ] )->SetWindowText( _itoa( mpst_LOD->auc_EndDistance[ mi_LOD ], sz_Value, 10 ) );

            for (i_Res = mi_LOD - 1; i_Res >= 0; i_Res--)
            {
                if (mpst_LOD->auc_EndDistance[ i_Res ] < mpst_LOD->auc_EndDistance[ i_Res + 1 ] )
                {
                    mpst_LOD->auc_EndDistance[ i_Res ] = mpst_LOD->auc_EndDistance[ i_Res + 1 ];
                    GetDlgItem( LODCTRL_Dist[ i_Res ] )->SetWindowText( _itoa( mpst_LOD->auc_EndDistance[ i_Res ], sz_Value, 10 ) );
                }
            }
            for (i_Res = mi_LOD + 1; i_Res < mpst_LOD->uc_NbLOD; i_Res++)
            {
                if (mpst_LOD->auc_EndDistance[ i_Res ] > mpst_LOD->auc_EndDistance[ i_Res - 1 ] )
                {
                    mpst_LOD->auc_EndDistance[ i_Res ] = mpst_LOD->auc_EndDistance[ i_Res - 1 ];
                    GetDlgItem( LODCTRL_Dist[ i_Res ] )->SetWindowText( _itoa( mpst_LOD->auc_EndDistance[ i_Res ], sz_Value, 10 ) );
                }
            }
        }
        else if (pMsg->message == WM_RBUTTONUP)
        {
            mi_LODDistCapture = 0;
            ReleaseCapture();
            LINK_Refresh();
        }
    }

	return CFormView::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_SubObjectMode(void)
{
	if(mpst_EditOption->ul_Flags & GRO_Cul_EOF_SubObject)
	{
		if(mpst_SkinDialog)
		{
            if (mpst_EditOption->ul_Flags & GRO_Cul_EOF_SkinMode)
				On_EditSkin();
			else
				mpst_SkinDialog = NULL;
		}

		mpo_ToolBox->mpo_View->Selection_EndSubObjectMode();
#ifdef JADEFUSION
		GetDlgItem( IDC_STATIC_NBSUBSEL )->SetWindowText("");
#endif
	}
	else
		mpo_ToolBox->mpo_View->Selection_BeginSubObjectMode();

	if(mpo_UVMapper) mpo_UVMapper->UpdateGao();

	UpdateControls();
	LINK_Refresh();
}
#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ToolBox_GROView::OnButton_Refresh(void)
{
     mpo_ToolBox->mpo_View->Selection_XenonRefresh();
     LINK_Refresh();
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_InvertSel(void)
{
    mpo_ToolBox->mpo_View->Selection_SubObject_InvertSel();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_ColorMode( void )
{
	int i_CM;
	
	i_CM = (mpst_EditOption->ul_Flags & GRO_Cul_EOF_ColorModeMask) >> GRO_Cul_EOF_ColorModeShift;
	i_CM = (i_CM + 1) % 3;
	mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_ColorModeMask;
	mpst_EditOption->ul_Flags |= i_CM << GRO_Cul_EOF_ColorModeShift;
	
	((CButton *) GetDlgItem( IDC_CHECK_COLORMODE ))->SetCheck( i_CM ? 1 : 0 );
	GetDlgItem(IDC_CHECK_COLORMODE)->SetWindowText( gasz_ColorModeName[ i_CM ] );
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_Snap(void)
{
	if(mpst_EditOption->ul_Flags & GRO_Cul_EOF_SnapVertex)
		mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_SnapVertex;
	else
		mpst_EditOption->ul_Flags |= GRO_Cul_EOF_SnapVertex;
	UpdateControls();
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_VisibleSel(void)
{
	if(mpst_EditOption->ul_Flags & GRO_Cul_EOF_SelectVisible)
		mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_SelectVisible;
	else
		mpst_EditOption->ul_Flags |= GRO_Cul_EOF_SelectVisible;
	UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_Pivot(void)
{
	if(mpst_EditOption->ul_Flags & GRO_Cul_EOF_MovePivot)
		mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_MovePivot;
	else
		mpst_EditOption->ul_Flags |= GRO_Cul_EOF_MovePivot;
	UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_MovesUpdateUV(void)
{
	if(mpst_EditOption->ul_Flags & GRO_Cul_EOF_VertexMoveUpdateUV)
		mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_VertexMoveUpdateUV;
	else
		mpst_EditOption->ul_Flags |= GRO_Cul_EOF_VertexMoveUpdateUV;
	UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_MovesUpdateMorph(void)
{
	if(mpst_EditOption->ul_Flags & GRO_Cul_EOF_VertexMoveUpdateMorph)
		mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_VertexMoveUpdateMorph;
	else
		mpst_EditOption->ul_Flags |= GRO_Cul_EOF_VertexMoveUpdateMorph;
	UpdateControls();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_NonMagnifold( void )
{
    EMEN_cl_SubMenu o_Menu(FALSE);
    int             i_Res;
    POINT           pt;

    M_MF()->InitPopupMenuAction(NULL, &o_Menu);
    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Select bad edges", -1);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Select face with bad edges", -1);

    GetCursorPos(&pt);
    i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
	switch(i_Res)
	{
	case 1:
        GEO_SubObject_ESelectBad( (GEO_tdst_Object *) mpo_ToolBox->mpst_Gro );
        OnRadio_EdgeMode();
		break;
    case 2:
        GEO_SubObject_FSelectBad( (GEO_tdst_Object *) mpo_ToolBox->mpst_Gro );
        OnRadio_FaceMode();
        break;
    }

}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_WarningScale( void )
{
	EMEN_cl_SubMenu			o_Menu(FALSE);
    int						i_Res;
    POINT					pt;
	OBJ_tdst_GameObject		*pst_GO;
	MATH_tdst_Matrix		st_Matrix;
	MATH_tdst_Vector		st_Pos;

	pst_GO = mpo_ToolBox->mpst_Gao;
	if (!pst_GO) return;

    M_MF()->InitPopupMenuAction(NULL, &o_Menu);
    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Reset Scale", -1);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Reset XForm", -1);

    GetCursorPos(&pt);
    i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
	switch(i_Res)
	{
	case 1:
		MATH_CopyMatrix(&st_Matrix, OBJ_pst_GetAbsoluteMatrix(pst_GO));
		MATH_CopyVector(&st_Pos, MATH_pst_GetTranslation(&st_Matrix));
		MATH_GetRotationMatrix(&st_Matrix, &st_Matrix);
		MATH_SetTranslation(&st_Matrix, &st_Pos);
		OBJ_SetAbsoluteMatrix(pst_GO, &st_Matrix);
		LINK_Refresh();
		break;
    case 2:
		mpo_ToolBox->mpo_View->GAO_ResetXForm( pst_GO );
        LINK_Refresh();
        break;
    }
}

	/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_WarningColmap( void )
{
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_UV(void)
{
	if(mpo_UVMapper)
		delete mpo_UVMapper;
	else
	{
		mpo_UVMapper = new EDIA_cl_UVMapperDialog(this);
		mpo_UVMapper->DoModeless();
	}

	UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnRadio_VertexMode(void)
{
	EndCurTool();
	mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_Mode;
	mpst_EditOption->ul_Flags |= GRO_Cul_EOF_Vertex;
	BeginCurTool();
	UpdateControls();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_PaintVertex(void)
{
	EndCurTool();
	if(mpst_EditOption->i_VertexTool == GRO_i_EOT_VertexPaint)
		mpst_EditOption->i_VertexTool = 0;
	else
		mpst_EditOption->i_VertexTool = GRO_i_EOT_VertexPaint;
	BeginCurTool();
	UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::On_Adjust4Symetrie(void)
{
	if((mpo_ToolBox->mpst_Gro) && (mpo_ToolBox->mpst_Gro->i->ul_Type == GRO_Geometric))
	{
		if(M_MF()->MessageBox("Really want to adjust for symetrie ?", "Jade", MB_YESNO) == IDYES)
		{
			GEO_AdjustForSymetrie((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::CloseSkinEditor(BOOL _b_Update)
{
	/*~~~~~~~~~~~~*/
	RECT	st_Rect;
	/*~~~~~~~~~~~~*/

	if(_b_Update) EndCurTool();
	if(mpst_SkinDialog) mpst_SkinDialog->GetWindowRect(&st_Rect);
	mst_SkinDialogPos.x = st_Rect.left;
	mst_SkinDialogPos.y = st_Rect.top;
	mpst_EditOption->i_VertexTool = 0;
	mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_SkinMode;

	if(_b_Update)
	{
		BeginCurTool();
		UpdateControls();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::DestroySkinEditor(void)
{
    if (mpst_EditOption->ul_Flags & GRO_Cul_EOF_SkinMode)
	{
		if(mpst_SkinDialog) delete(mpst_SkinDialog);
	}

	mpst_SkinDialog = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::On_EditSkin(void)
{
	EndCurTool();

    if (mpst_EditOption->ul_Flags & GRO_Cul_EOF_SkinMode)
	{
        mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_SkinMode;
		mpst_SkinDialog->DestroyWindow();
		mpst_SkinDialog = NULL;
        LINK_Refresh();
	}
	else
	{
		if(mpo_ToolBox->mpst_Gro)
		{
			if(mpo_ToolBox->mpst_Gao)
			{
				if(mpo_ToolBox->mpst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)
				{
					mpst_EditOption->ul_Flags |= GRO_Cul_EOF_SkinMode;
					mpst_SkinDialog = new EDIA_cl_SKN_Dialog
						(
							GRO_sz_Struct_GetName(mpo_ToolBox->mpst_Gro),
							0,
							0,
							0,
							(ULONG) 0,
							mpo_ToolBox->mpst_Gao,
							mpo_ToolBox->mpst_Gro
						);
					mpst_SkinDialog->mpo_GroView = this;
					mpst_SkinDialog->mst_Position = mst_SkinDialogPos;
					mpst_SkinDialog->DoModeless();
				}
				else
				{
					M_MF()->MessageBox
						(
							"The game object Must have gizmo\n before edit ponderation.",
							"Gizmo missing",
							MB_ICONINFORMATION | MB_OK
						);
				}
			}
		}
	}

	BeginCurTool();
	UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::On_DestroySkin(void)
{
	GEO_SKN_DestroyObjPonderation(((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->p_SKN_Objectponderation);
	((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->p_SKN_Objectponderation = NULL;

	if(mpst_EditOption->ul_Flags & GRO_Cul_EOF_SkinMode )
		On_EditSkin();
	else
		UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::PickRLIFromAnotherObject(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_cl_View						*po_3DView;
	POINT							st_Pt;
	OBJ_tdst_GameObject				*pst_GO[32];
	GEO_tdst_Object					*pst_Gro[32], *pst_GeoTgt, *pst_GeoSrc;
    GEO_tdst_StaticLOD              *pst_LOD;
	LONG							l_Number, l_Index, i;
	SOFT_tdst_PickingBuffer_Pixel	*pst_Pick;
	EDIA_cl_NameRLIDialogCombo		o_Dialog("Object selection", pst_GO);
	char							*sz_Ext;
    ULONG                           *pul_RLISrc, **ppul_RLITgt;
    BOOL                            b_Gro;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_Gro = (GetAsyncKeyState( VK_CONTROL ) < 0);

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	if(!mpo_ToolBox->mpst_Gro) return;
	if(!mpo_ToolBox->mpst_Gao) return;

	GetCursorPos(&st_Pt);
	po_3DView = mpo_ToolBox->mpo_View;
	po_3DView->ScreenToClient(&st_Pt);
	if(!po_3DView->Pick_l_UnderPoint(&st_Pt, SOFT_Cuc_PBQF_GameObject, 0)) return;

    /* get picked GAOs */
	pst_Pick = po_3DView->Pick_pst_GetFirst(SOFT_Cuc_PBQF_GameObject, -1);
	l_Number = 0;
	while(pst_Pick)
	{
		pst_GO[l_Number] = (OBJ_tdst_GameObject *) pst_Pick->ul_Value;
        pst_Gro[l_Number] = (GEO_tdst_Object *) OBJ_p_GetGro(pst_GO[l_Number]);
		if(pst_GO[l_Number] && pst_Gro[l_Number]) l_Number++;
		if(l_Number == 32) break;
		pst_Pick = po_3DView->Pick_pst_GetNext(SOFT_Cuc_PBQF_GameObject, -1, pst_Pick);
	}
    
	if(l_Number == 0) return; // No picked GAO

	//if(l_Number > 1)
	//{
		for(l_Index = 0; l_Index < l_Number; l_Index++) o_Dialog.AddItem(l_Index);

		sz_Ext = L_strrchr(pst_GO[0]->sz_Name, '.');
		if(sz_Ext) *sz_Ext = 0;
		o_Dialog.SetDefault(pst_GO[0]->sz_Name);
		if(sz_Ext) *sz_Ext = '.';
		if(o_Dialog.DoModal() != IDOK) return;

		l_Index = o_Dialog.mi_CurSelData;
		if((l_Index < 0) || (l_Index >= l_Number)) return;
	//}
	//else
	//	l_Index = 0;

    /* get source parameters */    
    pst_GeoSrc = pst_Gro[ l_Index ];
    pul_RLISrc = pst_GO[l_Index]->pst_Base->pst_Visu->dul_VertexColors;
	if ( o_Dialog.WorkWithGRORLI )
    {
        if (pst_GeoSrc->st_Id.i->ul_Type == GRO_GeoStaticLOD)
        {
            EDIA_cl_NameDialogCombo			o_DialogLOD("Choose LOD index");
            char                            sz_Text[ 16 ][ 256 ];

            pst_LOD = (GEO_tdst_StaticLOD *) pst_GeoSrc;
            l_Index = pst_GO[ l_Index ]->ul_ForceLODIndex % pst_LOD->uc_NbLOD;

            for (i = 0; i < pst_LOD->uc_NbLOD; i++)
            {
                if (i == 16) break;
                sprintf( sz_Text[i], "%d - %s", i, pst_LOD->dpst_Id[i] ? pst_LOD->dpst_Id[i]->sz_Name : "<empty>" );
                o_DialogLOD.AddItem( sz_Text[i], i );
            }
            o_DialogLOD.SetDefault( sz_Text[l_Index] );

            if ( o_DialogLOD.DoModal() != IDOK ) return;
            l_Index = o_DialogLOD.mi_CurSelData;

            pst_GeoSrc = (GEO_tdst_Object *) pst_LOD->dpst_Id[ l_Index ];
        }

        if (pst_GeoSrc && (pst_GeoSrc->st_Id.i->ul_Type == GRO_Geometric ) )
            pul_RLISrc = pst_GeoSrc->dul_PointColors;
    }
    else 
        pst_GeoSrc = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO[l_Index]);

    /* get target parameters */
    ppul_RLITgt = NULL;
	if ( b_Gro || mpo_ToolBox->mpo_View->mb_WorkWithGRORLI )
    {
        pst_GeoTgt = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo( mpo_ToolBox->mpst_Gao );
        if ( pst_GeoTgt && (pst_GeoTgt->st_Id.i->ul_Type == GRO_Geometric) )
            ppul_RLITgt = &pst_GeoTgt->dul_PointColors;
    }
    else
    {
        ppul_RLITgt = &mpo_ToolBox->mpst_Gao->pst_Base->pst_Visu->dul_VertexColors;
        pst_GeoTgt = (GEO_tdst_Object *) mpo_ToolBox->mpst_Gro;
        if ( !pst_GeoTgt || (pst_GeoTgt->st_Id.i->ul_Type != GRO_Geometric) )
            return;
    }

	GEO_PickRLI( pst_GeoTgt, ppul_RLITgt, pst_GeoSrc, pul_RLISrc, 0 );
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::PickUVFromAnotherObject(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	F3D_cl_View						*po_3DView;
	POINT							st_Pt;
	OBJ_tdst_GameObject				*pst_GO[32];
	GEO_tdst_Object					*pst_Gro[32], *pst_GeoTgt, *pst_GeoSrc;
    GEO_tdst_StaticLOD              *pst_LOD;
	LONG							l_Number, l_Index, i;
	SOFT_tdst_PickingBuffer_Pixel	*pst_Pick;
	EDIA_cl_NameDialogCombo			o_Dialog("Object selection");
	char							*sz_Ext;
    ULONG                           *pul_RLISrc, **ppul_RLITgt;
    BOOL                            b_Gro;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_Gro = (GetAsyncKeyState( VK_CONTROL ) < 0);

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	if(!mpo_ToolBox->mpst_Gro) return;
	if(!mpo_ToolBox->mpst_Gao) return;

	GetCursorPos(&st_Pt);
	po_3DView = mpo_ToolBox->mpo_View;
	po_3DView->ScreenToClient(&st_Pt);
	if(!po_3DView->Pick_l_UnderPoint(&st_Pt, SOFT_Cuc_PBQF_GameObject, 0)) return;

    /* get picked gao */
	pst_Pick = po_3DView->Pick_pst_GetFirst(SOFT_Cuc_PBQF_GameObject, -1);
	l_Number = 0;
	while(pst_Pick)
	{
		pst_GO[l_Number] = (OBJ_tdst_GameObject *) pst_Pick->ul_Value;
        pst_Gro[l_Number] = (GEO_tdst_Object *) OBJ_p_GetGro(pst_GO[l_Number]);
		if(pst_GO[l_Number] && pst_Gro[l_Number]) l_Number++;
		if(l_Number == 32) break;
		pst_Pick = po_3DView->Pick_pst_GetNext(SOFT_Cuc_PBQF_GameObject, -1, pst_Pick);
	}
    
	if(l_Number == 0) return;

	if(l_Number > 1)
	{
		for(l_Index = 0; l_Index < l_Number; l_Index++) o_Dialog.AddItem(pst_GO[l_Index]->sz_Name, l_Index);

		sz_Ext = L_strrchr(pst_GO[0]->sz_Name, '.');
		if(sz_Ext) *sz_Ext = 0;
		o_Dialog.SetDefault(pst_GO[0]->sz_Name);
		if(sz_Ext) *sz_Ext = '.';
		if(o_Dialog.DoModal() != IDOK) return;

		l_Index = o_Dialog.mi_CurSelData;
		if((l_Index < 0) || (l_Index >= l_Number)) return;
	}
	else
		l_Index = 0;

    /* get source parameters */    
    pst_GeoSrc = pst_Gro[ l_Index ];
    pul_RLISrc = pst_GO[l_Index]->pst_Base->pst_Visu->dul_VertexColors;
    if ( !pul_RLISrc && pst_GeoSrc )
    {
        if (pst_GeoSrc->st_Id.i->ul_Type == GRO_GeoStaticLOD)
        {
            EDIA_cl_NameDialogCombo			o_DialogLOD("Choose LOD index");
            char                            sz_Text[ 16 ][ 256 ];

            pst_LOD = (GEO_tdst_StaticLOD *) pst_GeoSrc;
            l_Index = pst_GO[ l_Index ]->ul_ForceLODIndex % pst_LOD->uc_NbLOD;

            for (i = 0; i < pst_LOD->uc_NbLOD; i++)
            {
                if (i == 16) break;
                sprintf( sz_Text[i], "%d - %s", i, pst_LOD->dpst_Id[i] ? pst_LOD->dpst_Id[i]->sz_Name : "<empty>" );
                o_DialogLOD.AddItem( sz_Text[i], i );
            }
            o_DialogLOD.SetDefault( sz_Text[l_Index] );

            if ( o_DialogLOD.DoModal() != IDOK ) return;
            l_Index = o_DialogLOD.mi_CurSelData;

            pst_GeoSrc = (GEO_tdst_Object *) pst_LOD->dpst_Id[ l_Index ];
        }

        if (pst_GeoSrc && (pst_GeoSrc->st_Id.i->ul_Type == GRO_Geometric ) )
            pul_RLISrc = pst_GeoSrc->dul_PointColors;
    }
    else 
        pst_GeoSrc = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO[l_Index]);

    /* get target parameters */
    ppul_RLITgt = NULL;
    if ( b_Gro )
    {
        pst_GeoTgt = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo( mpo_ToolBox->mpst_Gao );
        if ( pst_GeoTgt && (pst_GeoTgt->st_Id.i->ul_Type == GRO_Geometric) )
            ppul_RLITgt = &pst_GeoTgt->dul_PointColors;
    }
    else
    {
        ppul_RLITgt = &mpo_ToolBox->mpst_Gao->pst_Base->pst_Visu->dul_VertexColors;
        pst_GeoTgt = (GEO_tdst_Object *) mpo_ToolBox->mpst_Gro;
        if ( !pst_GeoTgt || (pst_GeoTgt->st_Id.i->ul_Type != GRO_Geometric) )
            return;
    }

	GEO_PickUV( pst_GeoTgt, pst_GeoSrc, 0 );
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_PaintVertexAlpha(void)
{
	EndCurTool();
	if(mpst_EditOption->i_VertexTool == GRO_i_EOT_VertexPaintAlpha)
		mpst_EditOption->i_VertexTool = 0;
	else
		mpst_EditOption->i_VertexTool = GRO_i_EOT_VertexPaintAlpha;
	BeginCurTool();
	UpdateControls();
}
void EDIA_cl_ToolBox_GROView::OnCheck_SmoothSel(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static EDIA_cl_SmoothSelectionDialog *po_Dial;
	CString				o_Title;
	ULONG				*pul_Value;
	static CRect				Rect;
	static	ULONG				Valid = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EndCurTool();
	if (SmoothSelIsActivate) 
	{
		po_Dial->GetWindowRect((LPRECT)&Rect);
		Valid = 1;
		po_Dial->DestroyWindow();
	} else

	{
		CWnd *KeepFocus;
		KeepFocus = GetFocus();
		o_Title = "Smooth selction ('S' key)";
		pul_Value = &mpst_EditOption->ul_VertexPaintColor;
		po_Dial = new EDIA_cl_SmoothSelectionDialog((char *) (LPCSTR) o_Title, pul_Value, NULL, (ULONG) this, 0);
		po_Dial->DoModeless();
		if (Valid)
			po_Dial->SetWindowPos(NULL,Rect.left,Rect.top,Rect.right - Rect.left,Rect.bottom - Rect.top,SWP_NOZORDER | SWP_NOSIZE );
		KeepFocus->SetFocus();
	}
	BeginCurTool();
	UpdateControls();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_PickVertexColor(void)
{
	EndCurTool();
	if(mpst_EditOption->i_VertexTool == GRO_i_EOT_VertexPickColor)
		mpst_EditOption->i_VertexTool = 0;
	else
		mpst_EditOption->i_VertexTool = GRO_i_EOT_VertexPickColor;
	BeginCurTool();
	UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 void EDIA_cl_ToolBox_GROView::OnButton_VAdjustRLI( void )
 {
	BOOL					b = (mpst_EditOption->ul_Flags & GRO_Cul_EOF_SubObject);
	/*
	if ( mpo_AdjustRLI )
	{
		mpo_AdjustRLI->DestroyWindow();
		delete mpo_AdjustRLI;
		mpo_AdjustRLI = NULL;
	}
	else
	{
		mpo_AdjustRLI = new EDIA_cl_RLIAdjustDialog( mpo_ToolBox->mpo_View, this, b ? 1 : 0 );
		mpo_AdjustRLI->DoModeless();
	}
	//((CButton *) GetDlgItem( IDC_CHECK_VERTEXADJUSTRLI ))->SetCheck( mpo_AdjustRLI ? 1 : 0 );
	*/

	EDIA_cl_RLIAdjustDialog dlg_RLIAdjust( mpo_ToolBox->mpo_View, this, b ? 1 : 0 );
	dlg_RLIAdjust.DoModal();
	((CButton *) GetDlgItem( IDC_CHECK_VERTEXADJUSTRLI ))->SetCheck( 0 );
 }
 
 /*
 =======================================================================================================================
 =======================================================================================================================
 */
 void EDIA_cl_ToolBox_GROView::AdjustRLI_ExternalClose( void )
 {
	mpo_AdjustRLI = NULL;
	((CButton *) GetDlgItem( IDC_CHECK_VERTEXADJUSTRLI ))->SetCheck( 0 );
 }

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_WeldVertex(void)
{
	EndCurTool();
	if(mpst_EditOption->i_VertexTool == GRO_i_EOT_VertexWeld)
		mpst_EditOption->i_VertexTool = 0;
	else
	{
		mpst_EditOption->i_VertexTool = GRO_i_EOT_VertexWeld;
	}

	BeginCurTool();
	UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_VWeldThresh(void)
{
	if(!(mpst_EditOption->ul_Flags & GRO_Cul_EOF_Vertex)) return;
	mpo_ToolBox->mpo_View->Selection_SubObject_VertexWeldThresh( -1 );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnEdit_WeldThresh(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	sz_Value[10];
	/*~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_EDIT_WELDTHRESH)->GetWindowText(sz_Value, 9);
	mpst_EditOption->f_VertexWeldThresh = (float) atof(sz_Value);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_VBuildFace(void)
{
	EndCurTool();
	if(mpst_EditOption->i_VertexTool == GRO_i_EOT_VertexBuildFace)
		mpst_EditOption->i_VertexTool = 0;
	else
		mpst_EditOption->i_VertexTool = GRO_i_EOT_VertexBuildFace;
	BeginCurTool();
	UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_VShowEdge(void)
{
    if ( mpst_EditOption->ul_Flags & GRO_Cul_EOF_ShowEdge)
        mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_ShowEdge;
    else
        mpst_EditOption->ul_Flags |= GRO_Cul_EOF_ShowEdge;

	UpdateControls();
    LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_VSym(void)
{
	if(!(mpst_EditOption->ul_Flags & GRO_Cul_EOF_Vertex)) return;
	mpo_ToolBox->mpo_View->Selection_SubObject_VertexSym();
	LINK_Refresh();
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnRadio_EdgeMode(void)
{
	EndCurTool();
	mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_Mode;
	mpst_EditOption->ul_Flags |= GRO_Cul_EOF_Edge;
	mpo_ToolBox->mpo_View->mpo_AssociatedEditor->i_OnMessage(EOUT_MESSAGE_SUBOBJECT_EUPDATEVSEL, 0, 0);
	BeginCurTool();
	UpdateControls();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnRadio_GAOsRLIMode(void)
{
	mpo_ToolBox->mpo_View->mb_WorkWithGRORLI = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnRadio_GROsRLIMode(void)
{
	mpo_ToolBox->mpo_View->mb_WorkWithGRORLI = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnRadio_NoMRMMode(void)
{
    if (GEO_ul_GetMRMState((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro) != GEO_MRM_None)
    {
        GEO_MRM_DestroyAll((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro);
	}

	UpdateControls();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnRadio_MRMCurveMode(void)
{
    ULONG ulMRMState = GEO_ul_GetMRMState((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro);

    if (ulMRMState == GEO_MRM_LOD)
        GEO_MRM_DeleteLevels((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro);

    if (ulMRMState != GEO_MRM_Curve)
    {
        po_Dial = new EDIA_cl_UPDATEDialog((char *) "");
        po_Dial->DoModeless();
        if (GEO_MRM_Compute(NULL, (GEO_tdst_Object *) mpo_ToolBox->mpst_Gro,TRUE, SetBarreState))
            GEO_MRM_SetCurve((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, 1.5f , 1.0f);
        delete(po_Dial);
        po_Dial = NULL;
    }

    UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnRadio_MRMLODMode(void)
{
    if (GEO_ul_GetMRMState((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro) != GEO_MRM_LOD)
    {
        float dfLevels[] = {.7f, .4f, .2f};
        float dfThresholds[] = {.7f, .4f, .2f};

        po_Dial = new EDIA_cl_UPDATEDialog((char *) "");
        po_Dial->DoModeless();

        ((CSliderCtrl *) GetDlgItem(IDC_SLIDER_MRM_LOD_NB))->SetPos(3);
        GEO_MRM_ComputeLevels(NULL,(GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, TRUE, 4,dfLevels,dfThresholds,SetBarreState);
        delete(po_Dial);
        po_Dial = NULL;
        UpdateControls();
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_TurnEdge(void)
{
	EndCurTool();
	if(mpst_EditOption->i_EdgeTool == GRO_i_EOT_EdgeTurn)
		mpst_EditOption->i_EdgeTool = 0;
	else
		mpst_EditOption->i_EdgeTool = GRO_i_EOT_EdgeTurn;
	BeginCurTool();
	UpdateControls();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_CutEdge(void)
{
	EndCurTool();
	if(mpst_EditOption->i_EdgeTool == GRO_i_EOT_EdgeCut)
		mpst_EditOption->i_EdgeTool = 0;
	else
		mpst_EditOption->i_EdgeTool = GRO_i_EOT_EdgeCut;
	BeginCurTool();
	UpdateControls();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_ECutter(void)
{
	EndCurTool();
	if(mpst_EditOption->i_EdgeTool == GRO_i_EOT_EdgeCutter)
		mpst_EditOption->i_EdgeTool = 0;
	else
		mpst_EditOption->i_EdgeTool = GRO_i_EOT_EdgeCutter;
	BeginCurTool();
	UpdateControls();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnRadio_FaceMode(void)
{
	EndCurTool();
	mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_Mode;
	mpst_EditOption->ul_Flags |= GRO_Cul_EOF_Face;
	mpo_ToolBox->mpo_View->mpo_AssociatedEditor->i_OnMessage(EOUT_MESSAGE_SUBOBJECT_FUPDATEVSEL, 0, 0);
	BeginCurTool();
	UpdateControls();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_FSelRejection( void )
{
    if (mpst_EditOption->ul_Flags & GRO_Cul_EOF_FacePickingBufferTest)
    {
        mpst_EditOption->ul_Flags &= ~(GRO_Cul_EOF_FaceBackfaceTest | GRO_Cul_EOF_FacePickingBufferTest);
        GetDlgItem( IDC_BUTTON_FSELREJECTION )->SetWindowText( "Sel all" );
    }
    else if (mpst_EditOption->ul_Flags & GRO_Cul_EOF_FaceBackfaceTest)
    {
        mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_FaceBackfaceTest;
        mpst_EditOption->ul_Flags |= GRO_Cul_EOF_FacePickingBufferTest;
        GetDlgItem( IDC_BUTTON_FSELREJECTION )->SetWindowText( "Sel (C)" );
    }
    else
    {
        mpst_EditOption->ul_Flags |= GRO_Cul_EOF_FaceBackfaceTest;
        GetDlgItem( IDC_BUTTON_FSELREJECTION )->SetWindowText( "Sel (N)" );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_FlipNormals(void)
{
    if(!(mpst_EditOption->ul_Flags & GRO_Cul_EOF_Face)) return;
	mpo_ToolBox->mpo_View->mpo_AssociatedEditor->i_OnMessage( EOUT_MESSAGE_SUBOBJECT_FLIPNORMALS, 0, 0 );
	LINK_Refresh();

    /* ca c'était quand le bouton flip était un check
    EndCurTool();
	if(mpst_EditOption->i_FaceTool == GRO_i_EOT_FaceFlipNormal)
		mpst_EditOption->i_FaceTool = 0;
	else
	{
		mpst_EditOption->i_FaceTool = GRO_i_EOT_FaceFlipNormal;
		mpo_ToolBox->mpo_View->mpo_AssociatedEditor->i_OnMessage(EOUT_MESSAGE_SUBOBJECT_FLIPNORMALS, 0, 0);
	}

	BeginCurTool();
	UpdateControls();
	LINK_Refresh();
    */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_SelFaceExt(void)
{
	if(!(mpst_EditOption->ul_Flags & GRO_Cul_EOF_Face)) return;
	mpo_ToolBox->mpo_View->mpo_AssociatedEditor->i_OnMessage(EOUT_MESSAGE_SUBOBJECT_FSELEXT, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_DetachFace(void)
{
	if(!(mpst_EditOption->ul_Flags & GRO_Cul_EOF_Face)) return;
	mpo_ToolBox->mpo_View->mpo_AssociatedEditor->i_OnMessage(EOUT_MESSAGE_SUBOBJECT_FDETACH, 0, 0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_AttachFace(void)
{
    mpo_ToolBox->mpo_View->Selection_AttachFace( mpo_ToolBox->mpst_Gao );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_HideFace(void)
{
    mpo_ToolBox->mpo_View->Selection_SubObject_HideFace( FALSE );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_UnHideFace(void)
{
    mpo_ToolBox->mpo_View->Selection_SubObject_UnHideFace();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_ChangeId(void)
{
	if(!(mpst_EditOption->ul_Flags & GRO_Cul_EOF_Face)) return;
	mpo_ToolBox->mpo_View->mpo_AssociatedEditor->i_OnMessage
		(
			EOUT_MESSAGE_SUBOBJECT_CHANGEID,
			mpst_EditOption->l_FaceId,
			0
		);
	LINK_Refresh();

	/*$F onvheck_changeid (si le bouton redevient un check) 
    EndCurTool();
    if(mpst_EditOption->i_FaceTool == GRO_i_EOT_FaceChangeId)
		mpst_EditOption->i_FaceTool = 0;
	else
    {
		mpst_EditOption->i_FaceTool = GRO_i_EOT_FaceChangeId;
        mpo_ToolBox->mpo_View->mpo_AssociatedEditor->i_OnMessage(EOUT_MESSAGE_SUBOBJECT_CHANGEID, mpst_EditOption->l_FaceId, 0);
    }
	BeginCurTool();
	UpdateControls();
	LINK_Refresh();
    */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_LockId(void)
{
	if(mpst_EditOption->ul_Flags & GRO_Cul_EOF_FaceLockId)
		mpst_EditOption->ul_Flags &= ~GRO_Cul_EOF_FaceLockId;
	else
		mpst_EditOption->ul_Flags |= GRO_Cul_EOF_FaceLockId;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnEdit_Id(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	sz_Value[10];
	/*~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_EDIT_ID)->GetWindowText(sz_Value, 9);
	mpst_EditOption->l_FaceId = atoi(sz_Value);

	/*$F si bouton Id redevinent un check
    if ( (mpst_EditOption->ul_Flags & GRO_Cul_EOF_Face) && (mpst_EditOption->i_FaceTool == GRO_i_EOT_FaceChangeId) ) 
    {
        mpo_ToolBox->mpo_View->mpo_AssociatedEditor->i_OnMessage(EOUT_MESSAGE_SUBOBJECT_CHANGEID, mpst_EditOption->l_FaceId, 0);
        LINK_Refresh();
    }
    */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_FaceCut(void)
{
	EndCurTool();

	if(mpst_EditOption->i_FaceTool == GRO_i_EOT_FaceCut)
		mpst_EditOption->i_FaceTool = 0;
	else
		mpst_EditOption->i_FaceTool = GRO_i_EOT_FaceCut;

	BeginCurTool();
	UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_PaintSel(void)
{
	/*~~~~~~~*/
	int i_Tool;
	/*~~~~~~~*/

	if(!(mpst_EditOption->ul_Flags & GRO_Cul_EOF_Vertex)) return;
	i_Tool = mpst_EditOption->i_VertexTool;
	if((i_Tool == GRO_i_EOT_VertexPaint) || (i_Tool == GRO_i_EOT_VertexPaintAlpha))
	{
		mpo_ToolBox->mpo_View->mpo_AssociatedEditor->i_OnMessage(EOUT_MESSAGE_SUBOBJECT_PAINTSEL, 0, 0);
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_ShowStrip(void)
{
	if(((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->ulStripFlag & GEO_C_Strip_DisplayStrip)
		GEO_STRIP_UnSetFlag(GEO_C_Strip_DisplayStrip, (GEO_tdst_Object *) mpo_ToolBox->mpst_Gro);
	else
		GEO_STRIP_SetFlag(GEO_C_Strip_DisplayStrip, (GEO_tdst_Object *) mpo_ToolBox->mpst_Gro);
	UpdateControls();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnCheck_ShowStripStat(void)
{
	/*~~~~~~~~~~~~~*/
	char	str[200];
	/*~~~~~~~~~~~~~*/

	if(((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->ulStripFlag & GEO_C_Strip_StatData)
		GEO_STRIP_UnSetFlag(GEO_C_Strip_StatData, (GEO_tdst_Object *) mpo_ToolBox->mpst_Gro);
	else
		GEO_STRIP_ComputeStat((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, str);

	GetDlgItem(IDC_STATIC_STRIPTEXT)->SetWindowText(str);

	UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_Strip(void)
{
	/*~~~~~~~~~~~~~*/
	char	str[200];
	/*~~~~~~~~~~~~~*/
	if(mpo_ToolBox->mpst_Gro) 
	{

		
		ULONG *pAdditionalVertexList;
		po_Dial = new EDIA_cl_UPDATEDialog((char *) "");
		po_Dial->DoModeless();
		pAdditionalVertexList = NULL;
		if ((mpo_ToolBox->mpst_Gao) && (mpo_ToolBox->mpst_Gao->pst_Base) && (mpo_ToolBox->mpst_Gao->pst_Base->pst_Visu))
			pAdditionalVertexList = mpo_ToolBox->mpst_Gao->pst_Base->pst_Visu->dul_VertexColors;

#if defined(_XENON_RENDER)
        if ((mpo_ToolBox->mpst_Gao->pst_Base) && (mpo_ToolBox->mpst_Gao->pst_Base->pst_Visu))
        {
            GEO_ClearXenonMesh(mpo_ToolBox->mpst_Gao->pst_Base->pst_Visu, (GEO_tdst_Object*)mpo_ToolBox->mpst_Gro, FALSE, FALSE);
        }
#endif
		GEO_STRIP_Compute(mpo_ToolBox->mpst_Gao , (GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, pAdditionalVertexList, TRUE  , SetBarreState2);

#if defined(_XENON_RENDER)
        if ((mpo_ToolBox->mpst_Gao->pst_Base) && (mpo_ToolBox->mpst_Gao->pst_Base->pst_Visu))
        {
            GEO_ResetXenonMesh(mpo_ToolBox->mpst_Gao, mpo_ToolBox->mpst_Gao->pst_Base->pst_Visu, (GEO_tdst_Object*)mpo_ToolBox->mpst_Gro, FALSE, TRUE, FALSE);
        }
#endif
		delete(po_Dial);
        po_Dial = NULL;
	
		GEO_STRIP_ComputeStat((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, str);

		GetDlgItem(IDC_STATIC_STRIPTEXT)->SetWindowText(str);
	}
	UpdateControls();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_UnStrip(void)
{
	if(mpo_ToolBox->mpst_Gro) GEO_STRIP_Delete((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro);
	UpdateControls();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_LODTransform()
{
    if (!mpo_ToolBox->mpst_Gao) return;
    if (!mpo_ToolBox->mpst_Gro) return;

    if (mpo_ToolBox->mpst_Gro->i->ul_Type == GRO_Geometric)
    {
        mpo_ToolBox->mpo_View->Selection_ChangeVisuFromGeoToLOD( mpo_ToolBox->mpst_Gao );
        mpo_ToolBox->SelectGaoFromPtr( mpo_ToolBox->mpst_Gao );
    }
    else if (mpo_ToolBox->mpst_Gro->i->ul_Type == GRO_GeoStaticLOD)
    {
        mpo_ToolBox->mpo_View->Selection_ChangeVisuFromLODToGeo( mpo_ToolBox->mpst_Gao );
        mpo_ToolBox->SelectGaoFromPtr( mpo_ToolBox->mpst_Gao );
    }
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_LOD(UINT _i_Num)
{
    UpdateControls_LOD();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_LockLOD(UINT _i_Num)
{
    if (!mpo_ToolBox->mpst_Gao) return;

    _i_Num -= IDC_BUTTON_LOCKLOD0;
    if (!(mpo_ToolBox->mpst_Gao->ul_EditorFlags & OBJ_C_EditFlags_ForceLOD))
    {
        mpo_ToolBox->mpst_Gao->ul_EditorFlags |= OBJ_C_EditFlags_ForceLOD;
        mpo_ToolBox->mpst_Gao->ul_ForceLODIndex = _i_Num;
    }
    else
    {
        if (mpo_ToolBox->mpst_Gao->ul_ForceLODIndex == (ULONG) _i_Num)
            mpo_ToolBox->mpst_Gao->ul_EditorFlags &= ~OBJ_C_EditFlags_ForceLOD;
        else
            mpo_ToolBox->mpst_Gao->ul_ForceLODIndex = _i_Num;
    }

    LINK_Refresh();
    UpdateControls_LOD();
}


static ULONG	ulRadiosityFlags = 1 | 4 | 8 | 256 | 128;
static ULONG	ulRadiosityQuality = 10;
static ULONG	ulBET = 6;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnREboot(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LIGHT_tdst_Light		stLight;
	EDIA_cl_RadiosityDialog *p_stRadDlg;
	ULONG					BackColor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_stRadDlg = new EDIA_cl_RadiosityDialog(&ulRadiosityFlags, &ulRadiosityQuality, &ulBET);
	p_stRadDlg->DoModal();
	if(ulRadiosityFlags & 1)
	{
		po_Dial = new EDIA_cl_UPDATEDialog((char *) "");
		po_Dial->DoModeless();

		stLight.st_Omni.f_Far = 10.0f;
		stLight.st_Omni.f_Near = 5.0f;
		stLight.ul_Color = 0xffffffff;
		BackColor = 0;
		if(ulRadiosityFlags & 128)
		{
			BackColor =
				((WOR_tdst_World *) mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_World)
					->ul_BackgroundColor;
		}

		if(ulRadiosityFlags & 4)
		{
			GLV_ComputeAnObject
			(
				mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_World,
				ulRadiosityFlags & 2,
				ulRadiosityQuality,
				ulRadiosityFlags & 16,
				-((float) ulBET / 8.0f) + 1.0f,
				ulRadiosityFlags & 32,
				ulRadiosityFlags & 8,
				ulRadiosityFlags & 64,
				BackColor,
				ulRadiosityFlags & 256,
				ulRadiosityFlags & 512,
				SetBarreState
			);
		}
		else
		{
			GLV_ComputeAnObject
			(
				mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_World,
				ulRadiosityFlags & 2,
				0,
				ulRadiosityFlags & 16,
				-((float) ulBET / 8.0f) + 1.0f,
				ulRadiosityFlags & 32,
				ulRadiosityFlags & 8,
				ulRadiosityFlags & 64,
				BackColor,
				ulRadiosityFlags & 256,
				ulRadiosityFlags & 512,
				SetBarreState
			);
		}

		delete(po_Dial);
        po_Dial = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnSRR(void)
{
	GLV_ShowTheKilt(((CButton *) GetDlgItem(IDC_CHECK_SHOWRADRES))->GetCheck());
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnREboot2(void)
{
	GLV_DestroyRadiosity(mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_World, 1);
#ifdef JADEFUSION
    // SC: Update the selection since the GEO has changed
    mpo_ToolBox->SelectGaoFromPtr(mpo_ToolBox->mpst_Gao);

    LINK_Refresh();
#endif
}
BOOL AdjustRadiosityMode = FALSE;
void EDIA_cl_ToolBox_GROView::OnAdjustRad(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LIGHT_tdst_Light		stLight;
	EDIA_cl_RadiosityDialog *p_stRadDlg;
	ULONG					BackColor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AdjustRadiosityMode = TRUE;
	p_stRadDlg = new EDIA_cl_RadiosityDialog(&ulRadiosityFlags, &ulRadiosityQuality, &ulBET);
	p_stRadDlg->DoModal();
	AdjustRadiosityMode = FALSE;
	if(ulRadiosityFlags & 1)
	{
		po_Dial = new EDIA_cl_UPDATEDialog((char *) "");
		po_Dial->DoModeless();

		stLight.st_Omni.f_Far = 10.0f;
		stLight.st_Omni.f_Near = 5.0f;
		stLight.ul_Color = 0xffffffff;
		BackColor = 0;
		if(ulRadiosityFlags & 128)
		{
			BackColor =
				((WOR_tdst_World *) mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_World)
					->ul_BackgroundColor;
		}

		if(ulRadiosityFlags & 4)
		{
			GLV_AdjustRadiosity
			(
				mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_World,
				ulRadiosityFlags & 2,
				ulRadiosityQuality,
				ulRadiosityFlags & 16,
				-((float) ulBET / 8.0f) + 1.0f,
				ulRadiosityFlags & 32,
				ulRadiosityFlags & 8,
				ulRadiosityFlags & 64,
				BackColor,
				ulRadiosityFlags & 256,
				ulRadiosityFlags & 512,
				SetBarreState
			);
		}
		else
		{
			GLV_AdjustRadiosity
			(
				mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_World,
				ulRadiosityFlags & 2,
				0,
				ulRadiosityFlags & 16,
				-((float) ulBET / 8.0f) + 1.0f,
				ulRadiosityFlags & 32,
				ulRadiosityFlags & 8,
				ulRadiosityFlags & 64,
				BackColor,
				ulRadiosityFlags & 256,
				ulRadiosityFlags & 512,
				SetBarreState
			);
		}

		delete(po_Dial);
        po_Dial = NULL;
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void HookProc(ULONG _o_View, ULONG)
{
	((EDIA_cl_ToolBox_GROView *) _o_View)->GetDlgItem(IDC_BUTTON_COLOR)->RedrawWindow
		(
			NULL,
			NULL,
			RDW_INVALIDATE | RDW_UPDATENOW
		);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnButton_VertexColor(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_ColorDialog *po_Dial;
	CString				o_Title;
	ULONG				*pul_Value;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_Title = "Pick a color for vertex paint";
	pul_Value = &mpst_EditOption->ul_VertexPaintColor;
	po_Dial = new EDIA_cl_ColorDialog((char *) (LPCSTR) o_Title, pul_Value, HookProc, (ULONG) this, 0);
	po_Dial->DoModeless();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CDC		*po_DC;
	ULONG	ul_Color;
	CRect	o_Rect, o_SubRect;
    CPen	o_Pen1, o_Pen2, o_Pen3, o_Pen4;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(nIDCtl == IDC_BUTTON_COLOR)
	{
		po_DC = CDC::FromHandle(lpDrawItemStruct->hDC);
		
		ul_DisplayedColor = mpst_EditOption->ul_VertexPaintColor;
		ul_Color = mpst_EditOption->ul_VertexPaintColor & 0xFFFFFF;

		::GetClientRect(lpDrawItemStruct->hwndItem, (LPRECT) & o_Rect);
		po_DC->FrameRect((LPRECT) & o_Rect, CBrush::FromHandle((HBRUSH) GetStockObject(BLACK_BRUSH)));
		o_Rect.DeflateRect(1, 1);
		o_SubRect = o_Rect;
		o_SubRect.right = o_Rect.left + (((o_Rect.right - o_Rect.left) * 3) / 4);
		po_DC->FillSolidRect((LPRECT) & o_SubRect, ul_Color);

		ul_Color = (mpst_EditOption->ul_VertexPaintColor & 0xFF000000) >> 8;
		ul_Color |= (ul_Color >> 8) | (ul_Color >> 16);
		o_Rect.left = o_SubRect.right;
		po_DC->FillSolidRect((LPRECT) & o_Rect, ul_Color);
	}
	else if(nIDCtl == IDC_BUTTON_COLORBEND)
	{
		po_DC = CDC::FromHandle(lpDrawItemStruct->hDC);

		::GetClientRect(lpDrawItemStruct->hwndItem, (LPRECT) & o_Rect);
		po_DC->FrameRect((LPRECT) & o_Rect, CBrush::FromHandle((HBRUSH) GetStockObject(BLACK_BRUSH)));
		o_Rect.DeflateRect(1, 1);

		o_SubRect = o_Rect;
		o_SubRect.right = o_Rect.left + (LONG) (mpst_EditOption->f_VertexBlendColor * (o_Rect.right - o_Rect.left));
		po_DC->FillSolidRect((LPRECT) & o_SubRect, 0xFFFFFF);
		o_Rect.left = o_SubRect.right;
		po_DC->FillSolidRect((LPRECT) & o_Rect, 0);
	}
	else if(nIDCtl == IDC_GRAPH)
	{
		if(mpo_ToolBox->mpst_Gao)
		{
			if((mpo_ToolBox->mpst_Gro) && (mpo_ToolBox->mpst_Gro->i->ul_Type == GRO_Geometric))
			{
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                void	*hOldPen;
                float	HorizCoef, VertiCoef, fMin;
                float	*p_CoefTable;
                ULONG	HPos, SaveRight,Counter;
                ULONG   eMRMState = GEO_ul_GetMRMState((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro);
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

                if (eMRMState != GEO_MRM_None)
                {
					po_DC = CDC::FromHandle(lpDrawItemStruct->hDC);

					o_Pen1.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
					o_Pen2.CreatePen(PS_SOLID, 2, RGB(0xff, 0, 0));
					o_Pen3.CreatePen(PS_SOLID, 2, RGB(0xff, 0xff, 0));
					o_Pen4.CreatePen(PS_SOLID, 3, RGB(0,0xff,0));

					::GetClientRect(lpDrawItemStruct->hwndItem, (LPRECT) & o_Rect);
					o_Rect.InflateRect(-1, -1);

					GEO_MRM_GetCurve((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, &HorizCoef, &fMin);

					SaveRight = o_Rect.left;
					o_Rect.left = o_Rect.left + (ULONG) ((float) (o_Rect.right - o_Rect.left) * (fMin));
					po_DC->FillSolidRect((LPRECT) & o_Rect, GetSysColor(COLOR_3DSHADOW));
					o_Rect.left = SaveRight;

					SaveRight = o_Rect.right;
					o_Rect.right = o_Rect.right + (ULONG) ((float) (o_Rect.left - o_Rect.right) * (1.0f - fMin));

					po_DC->FillSolidRect((LPRECT) & o_Rect, GetSysColor(COLOR_3DFACE));

					/* Draw vertical lines (grid) */
					HorizCoef = 0.0f;
					hOldPen = po_DC->SelectObject(&o_Pen1);
					for(Counter = 0; Counter < 8; Counter++)
					{
						HorizCoef += 1.0f / 8.0f;
						po_DC->MoveTo
							(
								(int) (o_Rect.left + (HorizCoef * (float) (o_Rect.right - o_Rect.left))),
								o_Rect.top
							);
						po_DC->LineTo
							(
								(int) (o_Rect.left + (HorizCoef * (float) (o_Rect.right - o_Rect.left))),
								o_Rect.bottom
							);
					}

					/* Draw distance */
					po_DC->SelectObject(&o_Pen3);
					HorizCoef = GEO_MRM_GetFloatFromUC((ULONG) mpo_ToolBox->mpst_Gao->uc_LOD_Vis);
                    if (((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->p_MRM_Levels)
                        HorizCoef *= ((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->p_MRM_Levels->f_DistanceCoef;
					HPos = o_Rect.left + 1 + (ULONG) ((float) (o_Rect.right - o_Rect.left - 3L) * HorizCoef);
					po_DC->MoveTo(HPos, o_Rect.bottom - 1);
					po_DC->LineTo(HPos, o_Rect.top - 1);

					/* Draw text */
					po_DC->SelectObject(M_MF()->mo_Fnt);
					po_DC->SetBkMode(TRANSPARENT);
					po_DC->SetTextColor(0 /* GetSysColor(COLOR_3DSHADOW) */ );
					po_DC->SetTextAlign(TA_CENTER);
					po_DC->ExtTextOut((o_Rect.left + o_Rect.right) >> 1, o_Rect.top + 3, 0, NULL, "Distance", 8, NULL);
					po_DC->SelectObject(M_MF()->mo_Fnt1);
					po_DC->ExtTextOut(o_Rect.left + 3, (o_Rect.bottom + o_Rect.top) >> 1, 0, NULL, "Quality", 7, NULL);

					/* Draw rect */
					o_Rect.InflateRect(1, 1);
					po_DC->Draw3dRect(&o_Rect, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
					DeleteObject(&o_Pen1);
					DeleteObject(&o_Pen2);
					DeleteObject(&o_Pen3);
                    po_DC->SelectObject(hOldPen);

                    if (eMRMState == GEO_MRM_Curve)
                    {
                        /* Draw curve */
                        po_DC->SelectObject(&o_Pen2);
                        p_CoefTable = GEO_gdf_MRMQualityCurve;
                        HorizCoef = 0.0f;
                        po_DC->MoveTo
                            (
                            (int) (o_Rect.left + (HorizCoef * (float) (o_Rect.right - o_Rect.left))),
                            (int) (o_Rect.top + ((1.0f) * (float) (o_Rect.bottom - o_Rect.top)))
                            );
                        for(Counter = 1; Counter < 8; Counter++)
                        {
                            HorizCoef += 1.0f / 8.0f;
                            p_CoefTable++;
                            po_DC->LineTo
                                (
                                (int) (o_Rect.left + (HorizCoef * (float) (o_Rect.right - o_Rect.left))),
                                (int) (o_Rect.top + ((1.0f - *p_CoefTable) * (float) (o_Rect.bottom - o_Rect.top)))
                                );
                        }

                        po_DC->LineTo(o_Rect.right, o_Rect.top);
                        po_DC->LineTo(SaveRight, o_Rect.top);

                        o_Rect.right = SaveRight;
                    }
                    else
                    {
                        // eMRMState == GEO_MRM_LOD

                        /* Draw green crosses (levels) */
                        po_DC->SelectObject(&o_Pen4);

                        for (Counter = 1; Counter < ((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->p_MRM_Levels->ul_LevelNb ; Counter++)
                        {
                            HorizCoef = ((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->p_MRM_Levels->f_Thresholds[Counter-1];
                            VertiCoef = ((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)->p_MRM_Levels->f_MRMLevels[Counter-1];

                            po_DC->MoveTo
                                (
                                (int) (o_Rect.left + (HorizCoef * (float) (o_Rect.right - o_Rect.left))),
                                (int) (o_Rect.bottom + 3 + VertiCoef * (o_Rect.top - o_Rect.bottom))
                                );
                            po_DC->LineTo
                                (
                                (int) (o_Rect.left + (HorizCoef * (float) (o_Rect.right - o_Rect.left))),
                                (int) (o_Rect.bottom - 3 + VertiCoef * (o_Rect.top - o_Rect.bottom))
                                );
                            po_DC->MoveTo
                                (
                                (int) (o_Rect.left + 3 +(HorizCoef * (float) (o_Rect.right - o_Rect.left))),
                                (int) (o_Rect.bottom +  VertiCoef * (o_Rect.top - o_Rect.bottom))
                                );
                            po_DC->LineTo
                                (
                                (int) (o_Rect.left - 3 + (HorizCoef * (float) (o_Rect.right - o_Rect.left))),
                                (int) (o_Rect.bottom + VertiCoef * (o_Rect.top - o_Rect.bottom))
                                );
                        }
                    }
                }
				else
				{
recmrm:
					po_DC = CDC::FromHandle(lpDrawItemStruct->hDC);
					::GetClientRect(lpDrawItemStruct->hwndItem, (LPRECT) & o_Rect);
					o_Rect.InflateRect(-1, -1);

					o_Rect.InflateRect(1, 1);
					po_DC->Draw3dRect(&o_Rect, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
				}
			}
			else
				goto recmrm;
		}
		else
			goto recmrm;
	}

	CFormView::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnSize(UINT nType, int cx, int cy)
{
	/* UpdateControls(); SetScrollSizes(MM_TEXT, CSize(0, 0)); */
	CFormView::OnSize(nType, cx, cy);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnLButtonDown(UINT, CPoint)
{
	/*~~~~~~~~~~~~~~~~~*/
	POINT	st_Pos;
	CRect	o_Rect;
	MSG		msg;
	CPoint	pt, old;
	int		x, y, xx, yy;
	int		min, max;
	/*~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&st_Pos);
	GetDlgItem(IDC_GRAPH)->GetWindowRect((LPRECT) & o_Rect);
	b_LButtonIsDown = 1;
	if(o_Rect.PtInRect(st_Pos))
	{
		if((mpo_ToolBox->mpst_Gro) && (mpo_ToolBox->mpst_Gro->i->ul_Type == GRO_Geometric))
		{
            ULONG eMRMState = GEO_ul_GetMRMState((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro);

			if (eMRMState == GEO_MRM_Curve)
			{
				b_IsSpinnigMRM = TRUE;
				SetCapture();
				OnMouseMove(0, 0);
				return;
			}
            else if (eMRMState == GEO_MRM_LOD)
			{
                // Choose cross close to cursor
                GEO_tdst_MRM_Levels *pMRMLevels = ((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro)->p_MRM_Levels;
                float fMouseX = (float) (st_Pos.x - o_Rect.left) / (float) (o_Rect.right - o_Rect.left); // 0 -> 1
                float fMouseY = 1.f - (float) (st_Pos.y - o_Rect.top) / (float) (o_Rect.bottom - o_Rect.top);  // 0 -> 1
                int i,iBestIndex;
                float fBestDistance = FLT_MAX;

                for (i=1; i<pMRMLevels->ul_LevelNb; i++)
                {
                    float fCrossX = pMRMLevels->f_Thresholds[i-1];
                    float fCrossY = pMRMLevels->f_MRMLevels[i-1];

                    float fDistance = fabsf(fCrossX-fMouseX) + fabsf(fCrossY-fMouseY);
                    if (fDistance < fBestDistance)
                    {
                        fBestDistance = fDistance;
                        iBestIndex = i;
                    }
                }

                if (fBestDistance < 0.1f)
                    mi_MovedMRM_LOD_Cross = iBestIndex;
                else
                {
                    if (mi_MovedMRM_LOD_Cross != -1)
                    {
                        GEO_tdst_Object *pGeom = (GEO_tdst_Object *) mpo_ToolBox->mpst_Gro;
                        GEO_MRM_ComputeLevels(NULL,pGeom, TRUE, 0,NULL,NULL,SetBarreState);
                    }

                    mi_MovedMRM_LOD_Cross = -1;
                }

				SetCapture();
				OnMouseMove(0, 0);
				return;
			}
		}
	}

	GetCursorPos(&old);
	GetCursorPos(&pt);

	GetDlgItem(IDC_BUTTON_COLORBEND)->GetWindowRect((LPRECT) & o_Rect);
	if(o_Rect.PtInRect(pt)) return;

	SetCapture();
	SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR1));
	while(1)
	{
		GetMessage(&msg, NULL, NULL, NULL);
		if(msg.message == WM_MOUSEMOVE)
		{
			GetCursorPos(&pt);
			x = old.x - pt.x;
			y = old.y - pt.y;
			xx = GetScrollPos(SB_HORZ);
			yy = GetScrollPos(SB_VERT);

			GetClientRect(&o_Rect);

			GetScrollRange(SB_HORZ, &min, &max);
			max -= o_Rect.Width();
			if(max <= 0) x = 0;
			if(xx + x > max) x = max - xx;
			if(xx + x < 0) x = -xx;

			GetScrollRange(SB_VERT, &min, &max);
			max -= o_Rect.Height();
			if(max <= 0) y = 0;
			if(yy + y > max) y = max - yy;
			if(yy + y < 0) y = -yy;

			if(x || y)
			{
				ScrollWindow(-x, -y);
				SetScrollPos(SB_HORZ, xx + x);
				SetScrollPos(SB_VERT, yy + y);
				M_MF()->LockDisplay(this);
				M_MF()->UnlockDisplay(this);
			}

			old = pt;
			continue;
		}

		if(msg.message == WM_LBUTTONUP) break;
	}

	ReleaseCapture();
	b_LButtonIsDown = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnRButtonUp(UINT, CPoint)
{
	b_RButtonIsDown = 0;
	OnMouseMove(0, 0);
	if(b_IsSpinnigMRM_Min)
	{
		b_IsSpinnigMRM_Min = FALSE;
		ReleaseCapture();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnRButtonDown(UINT, CPoint)
{
	/*~~~~~~~~~~~*/
	POINT	st_Pos;
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	GetCursorPos(&st_Pos);
	GetDlgItem(IDC_GRAPH)->GetWindowRect((LPRECT) & o_Rect);
	b_RButtonIsDown = 1;
	if(o_Rect.PtInRect(st_Pos))
	{
		if((mpo_ToolBox->mpst_Gro) && (mpo_ToolBox->mpst_Gro->i->ul_Type == GRO_Geometric))
		{
			if(GEO_MRM_ul_IsAnyMrmObject((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro))
			{
				b_IsSpinnigMRM_Min = TRUE;
				SetCapture();
			}
		}
	}

	OnMouseMove(0, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnLButtonUp(UINT, CPoint)
{
	b_LButtonIsDown = 0;
	OnMouseMove(0, 0);
	if(b_IsSpinnigMRM)
	{
		b_IsSpinnigMRM = FALSE;
		ReleaseCapture();
	}

	if(mi_Pick == 1)
	{
		ReleaseCapture();
		mi_Pick = 0;
		PickRLIFromAnotherObject();
		((CButton *) GetDlgItem(IDC_CHECK_VERTEXPICKRLI))->SetCheck(0);
        ::SetCursor(AfxGetApp()->LoadCursor(IDC_ARROW));
	}
	
	if(mi_Pick == 2)
	{
		ReleaseCapture();
		mi_Pick = 0;
		PickUVFromAnotherObject();
		((CButton *) GetDlgItem(IDC_CHECK_FACEPICKUV))->SetCheck(0);
        ::SetCursor(AfxGetApp()->LoadCursor(IDC_ARROW));
	}

    if (mi_MovedMRM_LOD_Cross != -1)
    {
        GEO_tdst_Object *pGeom = (GEO_tdst_Object *) mpo_ToolBox->mpst_Gro;

        GEO_MRM_ComputeLevels(NULL,pGeom, TRUE, 0,NULL,NULL,SetBarreState);
        mi_MovedMRM_LOD_Cross = -1;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OnMouseMove(UINT, CPoint Coord)
{
	/*~~~~~~~~~~~*/
	POINT	st_Pos;
	CRect	o_Rect;
    ULONG eMRMState = GEO_ul_GetMRMState((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro);
	/*~~~~~~~~~~~*/

    GetCursorPos(&st_Pos);
    GetDlgItem(IDC_GRAPH)->GetWindowRect((LPRECT) & o_Rect);

    if((!b_IsSpinnigMRM) && (!b_IsSpinnigMRM_Min))
	{
		b_RButtonIsDown = 1;
		if(o_Rect.PtInRect(st_Pos))
		{
			EDI_Tooltip_DisplayMessage
			(
				"MRM quality adjust\nMust set the flags \"Engine auto-refresh\" \nand \"Editor auto-refresh\" \nto see the yellow line moving",
				500
			);
		}
	}

    if (eMRMState == GEO_MRM_Curve)
    {
        if(b_IsSpinnigMRM)
        {
            /*~~~~~~~~~~~~*/
            float	NewExp;
            float	NewExp2;
            float	fMin;
            /*~~~~~~~~~~~~*/

            GEO_MRM_GetCurve((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, &NewExp, &fMin);
            if(!b_IsSpinnigMRM_Min)
                o_Rect.right = o_Rect.right + (ULONG) ((float) (o_Rect.left - o_Rect.right) * (1.0f - fMin));
            NewExp2 = (float) (st_Pos.x - o_Rect.left) / (float) (o_Rect.right - o_Rect.left);
            if(NewExp2 < 0.0f) NewExp2 = 0.0f;
            if(NewExp2 > 1.0f) NewExp2 = 1.0f;
            NewExp2 = 1.0f - NewExp2;
            NewExp = (float) (st_Pos.y - o_Rect.top) / (NewExp2 * (float) (o_Rect.bottom - o_Rect.top));
            GEO_MRM_SetCurve((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, NewExp, fMin);
            GEO_MRM_GetCurve((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, &NewExp, &fMin);
            UpdateControls();
            GEO_MRM_GetCurve((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, &NewExp, &fMin);
            LINK_Refresh();
            GEO_MRM_GetCurve((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, &NewExp, &fMin);
        }

        if(b_IsSpinnigMRM_Min)
        {
            /*~~~~~~~~~~~~*/
            float	NewExp;
            float	NewExp2;
            /*~~~~~~~~~~~~*/

            GEO_MRM_GetCurve((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, &NewExp, &NewExp2);
            NewExp2 = (float) (st_Pos.x - o_Rect.left) / (float) (o_Rect.right - o_Rect.left);
            if(NewExp2 < 0.0f) NewExp2 = 0.0f;
            if(NewExp2 > 1.0f) NewExp2 = 1.0f;
            GEO_MRM_SetCurve((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, NewExp, NewExp2);
            UpdateControls();
            LINK_Refresh();

            /* TreatSpinMRM(); */
        }
    }
    else if (GEO_MRM_LOD == eMRMState)
    {
        if (mi_MovedMRM_LOD_Cross != -1)
        {
            // Get movement limit for cross (in order to respect order)
            float fMinX,fMaxX,fMinY,fMaxY;
            GEO_tdst_MRM_Levels *pMRMLevels = ((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro)->p_MRM_Levels;
            float fMouseX = (float) (st_Pos.x - o_Rect.left) / (float) (o_Rect.right - o_Rect.left); // 0 -> 1
            float fMouseY = 1.f - (float) (st_Pos.y - o_Rect.top) / (float) (o_Rect.bottom - o_Rect.top);  // 0 -> 1

            if (mi_MovedMRM_LOD_Cross > 1)
            {
                fMaxX = pMRMLevels->f_Thresholds[mi_MovedMRM_LOD_Cross-2];
                fMaxY = pMRMLevels->f_MRMLevels[mi_MovedMRM_LOD_Cross-2];
            }
            else
            {
                fMaxX = 1.f;
                fMaxY = 1.f;
            }

            if (mi_MovedMRM_LOD_Cross < pMRMLevels->ul_LevelNb-1)
            {
                fMinX = pMRMLevels->f_Thresholds[mi_MovedMRM_LOD_Cross];
                fMinY = pMRMLevels->f_MRMLevels[mi_MovedMRM_LOD_Cross];
            }
            else
            {
                fMinX = 0.f;
                fMinY = 0.f;
            }

            // Move cross to new pos.
            pMRMLevels->f_Thresholds[mi_MovedMRM_LOD_Cross-1] = MATH_f_FloatLimit(fMouseX,fMinX,fMaxX);
            pMRMLevels->f_MRMLevels[mi_MovedMRM_LOD_Cross-1] = MATH_f_FloatLimit(fMouseY,fMinY,fMaxY);

            UpdateControls();
            LINK_Refresh();
        }
    }
    


	if(b_IsSpinnigMRM_Min || b_IsSpinnigMRM) return;

	if(b_LButtonIsDown)
	{
		GetCursorPos(&st_Pos);
		GetDlgItem(IDC_BUTTON_COLORBEND)->GetWindowRect((LPRECT) & o_Rect);
		if(o_Rect.PtInRect(st_Pos))
		{
			if(!b_MouseIsClipped)
			{
				GetClipCursor(&st_MouseClipSave);
				ClipCursor((LPRECT) & o_Rect);
				b_MouseIsClipped = 1;
			}

			mpst_EditOption->f_VertexBlendColor = (float) (st_Pos.x - o_Rect.left) / (float) (o_Rect.right - o_Rect.left);
			GetDlgItem(IDC_BUTTON_COLORBEND)->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
	}
	else if(b_MouseIsClipped)
	{
		b_MouseIsClipped = 0;
		ClipCursor(&st_MouseClipSave);
	}

    if (ChildWindowFromPoint( Coord ) == this)
        SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR1));
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
void EDIA_cl_ToolBox_GROView::On_SliderMRMDistance_Released(NMHDR * pNotifyStruct, LRESULT * result)
{ 
    ULONG eMRMState = GEO_ul_GetMRMState((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro);

    if (eMRMState == GEO_MRM_LOD)
    {
        CSliderCtrl *po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MRM_DISTANCE);
        po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MRM_DISTANCE);
        int Pos = po_Slider->GetPos();
        float fDistanceCoef = (float)Pos/1000.f;
        if (fDistanceCoef != ((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro)->p_MRM_Levels->f_DistanceCoef)
        {
            ((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro)->p_MRM_Levels->f_DistanceCoef = fDistanceCoef;
            UpdateControls();
            LINK_Refresh();
        }
    }
    //*result = 0;
}

void EDIA_cl_ToolBox_GROView::On_SliderMRMLODNb_Released(NMHDR * pNotifyStruct, LRESULT * result)
{ 
    ULONG eMRMState = GEO_ul_GetMRMState((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro);

    if (eMRMState == GEO_MRM_LOD)
    {
        CSliderCtrl *po_Slider = (CSliderCtrl *) GetDlgItem(IDC_SLIDER_MRM_LOD_NB);
        int LevelNb = po_Slider->GetPos()+1;
        if (LevelNb != ((GEO_tdst_Object *)mpo_ToolBox->mpst_Gro)->p_MRM_Levels->ul_LevelNb)
        {
            GEO_MRM_ComputeLevels(
                NULL,
                (GEO_tdst_Object *) mpo_ToolBox->mpst_Gro, 
                TRUE, 
                LevelNb,
                NULL ,
                NULL,
                SetBarreState);
            UpdateControls();
            LINK_Refresh();
        }
    }
    //*result = 0;
}

#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/

void EDIA_cl_ToolBox_GROView::OnXeMergeFlags(void)
{
	mpo_XeMergeFlags = new EDIA_cl_XeMergeFlags(this, mpo_ToolBox->mpst_Gao);
	mpo_XeMergeFlags->DoModeless();
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_GROView::OneTrameEnding(void)
{
	if (mpo_ToolBox->mpst_Gao)
	{
		if((mpo_ToolBox->mpst_Gro) && (mpo_ToolBox->mpst_Gro->i->ul_Type == GRO_Geometric))
		{
			if((GEO_MRM_ul_IsAnyMrmObject((GEO_tdst_Object *) mpo_ToolBox->mpst_Gro)))
			{
				if(uc_SavedLOD != mpo_ToolBox->mpst_Gao->uc_LOD_Vis)
				{
					UpdateControls();
					uc_SavedLOD = mpo_ToolBox->mpst_Gao->uc_LOD_Vis;
				}
			}
		}
	}
}

#ifdef JADEFUSION
#if defined(_XENON_RENDER)
void EDIA_cl_ToolBox_GROView::OnButton_XenonProcess(void)
{
    SEL_tdst_SelectedItem* pst_Item;

    pst_Item = mpo_ToolBox->mpo_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
    while (pst_Item != NULL)
    {
        if (pst_Item->l_Flag & SEL_C_SIF_Object)
        {
            OBJ_tdst_GameObject* pst_GO = (OBJ_tdst_GameObject*)pst_Item->p_Content;

            XMP_ProcessGameObject(mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_World, pst_GO);
        }

        pst_Item = pst_Item->pst_Next;
    }

    // Reset the selection
    mpo_ToolBox->SelectGaoFromPtr(NULL);

    LINK_Refresh();
}

void EDIA_cl_ToolBox_GROView::OnButton_XenonRevert(void)
{
    SEL_tdst_SelectedItem* pst_Item;

    pst_Item = mpo_ToolBox->mpo_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
    while (pst_Item != NULL)
    {
        if (pst_Item->l_Flag & SEL_C_SIF_Object)
        {
            OBJ_tdst_GameObject* pst_GO = (OBJ_tdst_GameObject*)pst_Item->p_Content;

            XMP_RevertGameObject(mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_World, pst_GO);
        }

        pst_Item = pst_Item->pst_Next;
    }

    // Reset the selection
    mpo_ToolBox->SelectGaoFromPtr(NULL);

    LINK_Refresh();
}

void EDIA_cl_ToolBox_GROView::OnButton_XenonEditCreate(void)
{
    SEL_tdst_SelectedItem* pst_Item;

    pst_Item = mpo_ToolBox->mpo_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
    while (pst_Item != NULL)
    {
        if (pst_Item->l_Flag & SEL_C_SIF_Object)
        {
            OBJ_tdst_GameObject* pst_GO = (OBJ_tdst_GameObject*)pst_Item->p_Content;

            XMP_CreateEditableMesh(mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_World, pst_GO);
        }

        pst_Item = pst_Item->pst_Next;
    }

    // Reset the selection
    mpo_ToolBox->SelectGaoFromPtr(NULL);

    LINK_Refresh();
}

void EDIA_cl_ToolBox_GROView::OnButton_XenonEditRemove(void)
{
    SEL_tdst_SelectedItem* pst_Item;

    pst_Item = mpo_ToolBox->mpo_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
    while (pst_Item != NULL)
    {
        if (pst_Item->l_Flag & SEL_C_SIF_Object)
        {
            OBJ_tdst_GameObject* pst_GO = (OBJ_tdst_GameObject*)pst_Item->p_Content;

            XMP_RemoveEditableMesh(mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_World, pst_GO);
        }

        pst_Item = pst_Item->pst_Next;
    }

    // Reset the selection
    mpo_ToolBox->SelectGaoFromPtr(NULL);

    LINK_Refresh();
}

void EDIA_cl_ToolBox_GROView::OnButton_XenonFixEdges(void)
{
    SEL_tdst_SelectedItem* pst_Item;

    pst_Item = mpo_ToolBox->mpo_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
    while (pst_Item != NULL)
    {
        if (pst_Item->l_Flag & SEL_C_SIF_Object)
        {
            OBJ_tdst_GameObject* pst_GO = (OBJ_tdst_GameObject*)pst_Item->p_Content;

            XMP_FixGameObjectEdges(mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_World, pst_GO);
        }

        pst_Item = pst_Item->pst_Next;
    }

    LINK_Refresh();
}

void EDIA_cl_ToolBox_GROView::OnButton_XenonLOD0(void)
{
    XenonPickLOD(0);
}

void EDIA_cl_ToolBox_GROView::OnButton_XenonLOD1(void)
{
    XenonPickLOD(1);
}

void EDIA_cl_ToolBox_GROView::OnButton_XenonLOD2(void)
{
    XenonPickLOD(2);
}

void EDIA_cl_ToolBox_GROView::OnButton_XenonLOD3(void)
{
    XenonPickLOD(3);
}

void EDIA_cl_ToolBox_GROView::OnButton_XenonLOD4(void)
{
    XenonPickLOD(4);
}

void EDIA_cl_ToolBox_GROView::OnButton_XenonLOD5(void)
{
    XenonPickLOD(5);
}

void EDIA_cl_ToolBox_GROView::XenonPickLOD(UCHAR _uc_Level)
{
    if (mpo_ToolBox && mpo_ToolBox->mpst_Gao)
    {
        GEO_PackGameObjectLOD(mpo_ToolBox->mpst_Gao, _uc_Level);

        LINK_Refresh();
    }
}

#endif

BOOL EDIA_cl_ToolBox_GROView::IsAtLeastOneGAOSelected(void)
{
    SEL_tdst_SelectedItem* pst_Item;

    if (!mpo_ToolBox ||
        !mpo_ToolBox->mpo_View ||
        !mpo_ToolBox->mpo_View->mst_WinHandles.pst_World ||
        !mpo_ToolBox->mpo_View->mst_WinHandles.pst_World->pst_Selection)
        return FALSE;

    pst_Item = mpo_ToolBox->mpo_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
    while (pst_Item != NULL)
    {
        if (pst_Item->l_Flag & SEL_C_SIF_Object)
        {
            return TRUE;
        }

        pst_Item = pst_Item->pst_Next;
    }

    return FALSE;
}
#endif

#endif /* ACTIVE_EDITORS */
