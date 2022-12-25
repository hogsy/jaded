/*$T DIAtoolbox_groview.h GC! 1.081 04/26/00 12:37:40 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __DIATOOLBOX_GROVIEW_H__
#define __DIATOOLBOX_GROVIEW_H__
#include "BASe/BAStypes.h"

#ifdef ACTIVE_EDITORS
class	EDIA_cl_ToolBoxDialog;
class   EDIA_cl_UVMapperDialog;
#ifdef JADEFUSION
class	EDIA_cl_XeMergeFlags;
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class	EGRO_cl_Frame;
class	EOUT_cl_Frame;
class	F3D_cl_View;
class   EDIA_cl_SKN_Dialog;
class	EDIA_cl_RLIAdjustDialog;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EDIA_cl_ToolBox_GROView : public CFormView
{
	DECLARE_DYNCREATE(EDIA_cl_ToolBox_GROView)

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	EDIA_cl_ToolBox_GROView(EDIA_cl_ToolBoxDialog *);
	EDIA_cl_ToolBox_GROView(void);
	~EDIA_cl_ToolBox_GROView();

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	unsigned char					uc_SavedLOD;
	EDIA_cl_ToolBoxDialog			*mpo_ToolBox;
	struct GRO_tdst_EditOptions_	*mpst_EditOption;
	BOOL							b_LButtonIsDown;
	BOOL							b_RButtonIsDown;
	BOOL							b_MouseIsClipped;
	BOOL							b_IsSpinnigMRM;
	BOOL							b_IsSpinnigMRM_Min;
    ULONG                           mi_MovedMRM_LOD_Cross;
	float							MRM_SpinInitNumberOfPoints;
	float							MRM_SpinInitNumberOfPointsSave;
	float							MRM_SpinInitNumberOfPointsMax;
	RECT							st_MouseClipSave;
	ULONG							ul_DisplayedColor;
    BOOL                            mb_Update;
    BOOL                            mb_Init;
    int                             mi_Pick;

    struct GEO_tdst_StaticLOD_      *mpst_LOD;
    int                             mi_LODCapture;
    int                             mi_LODDistCapture;
    int                             mi_LOD;
    int                             mi_LODTo;
    int                             mi_LODDrop;
    CPoint                          mo_LODPt;

    EDIA_cl_SKN_Dialog              *mpst_SkinDialog;
    POINT                           mst_SkinDialogPos;

    EDIA_cl_UVMapperDialog          *mpo_UVMapper;
    EDIA_cl_RLIAdjustDialog			*mpo_AdjustRLI;
#ifdef JADEFUSION
	EDIA_cl_XeMergeFlags			*mpo_XeMergeFlags;
#endif
    CBitmap                         mao_Cadenas[2];

	int SmoothSelection;

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	BOOL							Create(LPCTSTR, LPCTSTR, DWORD, const RECT &, CWnd *);
	void							UpdateControls( void );
    void                            UpdateControls_LOD( void );

#ifdef JADEFUSION
	void							Update_MergePS2Data( void );
	void							Remove_MergePS2Data( void );
#endif

    BOOL                            b_CanDrop(POINT);
    void                            DropGro( BIG_INDEX );
    void                            DropGao( BIG_INDEX );
    void                            UpdateNumberOfSubSel( void );

    int                             IsLODButton( HWND );
    int                             IsInLODDistCtrl( void );
    void                            LOD_InsertGeo( int, struct GRO_tdst_Struct_ *);
    void                            LOD_Clean( void );
    void                            UpdateFaceId( int );

	void							EndCurTool(void);
	void							BeginCurTool(void);

	void							Refresh(void);
	void							OneTrameEnding(void);

    void							CloseSkinEditor(BOOL);
    void							DestroySkinEditor(void);

    void                            PickRLIFromAnotherObject( void );
	void                            PickUVFromAnotherObject( void );
    void							AdjustRLI_ExternalClose( void );

/*
 -----------------------------------------------------------------------------------------------------------------------
    Message headers
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	BOOL							PreTranslateMessage(MSG *);
#ifdef JADEFUSION
	afx_msg void                    OnButton_Refresh(void);
#endif
	afx_msg void					OnCheck_ColorMode( void );
	afx_msg void					OnCheck_SubObjectMode(void);
    afx_msg void					OnButton_InvertSel(void);
    afx_msg void					OnCheck_Snap(void);
    afx_msg void					OnCheck_VisibleSel(void);
    afx_msg void					OnCheck_Pivot(void);
    afx_msg void					OnCheck_MovesUpdateUV(void);
	afx_msg void					OnCheck_MovesUpdateMorph(void);
    afx_msg void					OnButton_NonMagnifold(void);
	afx_msg void					OnButton_WarningScale(void);
	afx_msg void					OnButton_WarningColmap(void);
    afx_msg void					OnCheck_UV(void);
	afx_msg void					OnRadio_VertexMode(void);
	afx_msg void					OnCheck_PaintVertex(void);
	afx_msg void					On_EditSkin(void);
    afx_msg void					On_DestroySkin( void );
	afx_msg void					On_Adjust4Symetrie(void);
	afx_msg void					OnCheck_PaintVertexAlpha(void);
	afx_msg void					OnCheck_SmoothSel(void);
	afx_msg void					OnButton_PaintSel(void);
	afx_msg void					OnCheck_PickVertexColor(void);
	afx_msg void					OnButton_VAdjustRLI( void );
    afx_msg void					OnCheck_WeldVertex(void);
    afx_msg void					OnButton_VWeldThresh(void);
    afx_msg void					OnEdit_WeldThresh( void );
    afx_msg void					OnButton_VSym(void);
    afx_msg void					OnCheck_VBuildFace(void);
    afx_msg void                    OnCheck_VShowEdge(void);
	afx_msg void					OnRadio_EdgeMode(void);
	afx_msg void					OnCheck_TurnEdge(void);
	afx_msg void					OnCheck_CutEdge(void);
    afx_msg void                    OnCheck_ECutter(void);
	afx_msg void					OnRadio_FaceMode(void);
    afx_msg void					OnButton_FSelRejection(void);
    afx_msg void					OnButton_FlipNormals(void);
    afx_msg void					OnButton_ChangeId(void);
    afx_msg void					OnCheck_LockId(void);
    afx_msg void					OnButton_SelFaceExt(void);
    afx_msg void					OnButton_DetachFace(void);
    afx_msg void					OnButton_AttachFace(void);
    afx_msg void					OnButton_HideFace(void);
    afx_msg void					OnButton_UnHideFace(void);
    afx_msg void					OnEdit_Id(void);
    afx_msg void					OnCheck_FaceCut(void);
	afx_msg void					OnButton_VertexColor(void);
	afx_msg void					OnRadio_NoMRMMode(void);
	afx_msg void					OnRadio_MRMCurveMode(void);
	afx_msg void					OnRadio_MRMLODMode(void);
	afx_msg void					OnDrawItem(int, LPDRAWITEMSTRUCT);
	afx_msg void					OnSize(UINT, int, int);
	afx_msg void					OnLButtonDown(UINT, CPoint);
	afx_msg void					OnLButtonUp(UINT, CPoint);
	afx_msg void					OnRButtonDown(UINT, CPoint);
	afx_msg void					OnRButtonUp(UINT, CPoint);
	afx_msg void					OnMouseMove(UINT, CPoint);
	afx_msg void					OnButton_Strip(void);
	afx_msg void					OnButton_UnStrip(void);
	afx_msg void					OnCheck_ShowStrip(void);
	afx_msg void					OnCheck_ShowStripStat(void);
	afx_msg void					OnREboot(void);
	afx_msg void					OnREboot2(void);
	afx_msg void					OnAdjustRad(void);
	afx_msg void					OnSRR(void);
    afx_msg void					OnButton_LODTransform( void );
    afx_msg void	                OnButton_LOD( UINT );
    afx_msg void	                OnButton_LockLOD( UINT );
    afx_msg void                    On_SliderMRMDistance_Released(NMHDR * pNotifyStruct, LRESULT * result);
    afx_msg void                    On_SliderMRMLODNb_Released(NMHDR * pNotifyStruct, LRESULT * result);
	afx_msg void					OnRadio_GAOsRLIMode(void);
	afx_msg void					OnRadio_GROsRLIMode(void);

#ifdef JADEFUSION
	afx_msg void					OnXeMergeFlags();

#if defined(_XENON_RENDER)
    afx_msg void                    OnButton_XenonProcess(void);
    afx_msg void                    OnButton_XenonRevert(void);
    afx_msg void                    OnButton_XenonEditCreate(void);
    afx_msg void                    OnButton_XenonEditRemove(void);
    afx_msg void                    OnButton_XenonFixEdges(void);

    afx_msg void                    OnButton_XenonLOD0(void);
    afx_msg void                    OnButton_XenonLOD1(void);
    afx_msg void                    OnButton_XenonLOD2(void);
    afx_msg void                    OnButton_XenonLOD3(void);
    afx_msg void                    OnButton_XenonLOD4(void);
    afx_msg void                    OnButton_XenonLOD5(void);

    void XenonPickLOD(UCHAR _uc_Level);
#endif
    BOOL                            IsAtLeastOneGAOSelected(void);

#endif

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */

#endif /* __DIATOOLBOX_GROVIEW_H__ */
