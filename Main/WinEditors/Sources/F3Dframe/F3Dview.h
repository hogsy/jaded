/*$T F3Dview.h GC 1.134 04/22/04 13:45:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"
#include "ENGine/Sources/ENGmsg.h"
#include "EDImainframe.h"
#include "EDIundo.h"
#include "GRObject/GROedit.h"
#include "BASe/BAStypes.h"
#include "F3Dframe/F3Dview_postit.h"
#include "SELection/SELection.h"
#include "EDItors/Sources/PreFaB/PFBdata.h"


/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#define F3D_StdView_Top					0
#define F3D_StdView_Bottom				1
#define F3D_StdView_Left				2
#define F3D_StdView_Right				3
#define F3D_StdView_Front				4
#define F3D_StdView_Back				5
#define F3D_StdView_Origin				6

#define F3D_Action_CameraOn				1
#define F3D_Action_CameraOn1			2
#define F3D_Action_CameraOn2			3
#define F3D_Action_ChangeConstraint		7
#define F3D_Action_AddWaypoint			10
#define F3D_Action_Link					11
#define F3D_Action_AddInvisible			12
#define F3D_Action_HierarchicalLink		13

#ifdef JADEFUSION
#define F3D_SubObject_Vertex			0
#define F3D_SubObject_Edge				1
#define F3D_SubObject_Face				2

#define F3D_SubObject_Edge_Null			0
#define F3D_SubObject_Edge_Turn			1
#define F3D_SubObject_Edge_Cut			2
#endif

#define F3D_Duplicate_Geometry			0x00000001
#define F3D_Duplicate_Light				0x00000002
#define F3D_Duplicate_Material			0x00000004
#define F3D_Duplicate_PAG				0x00000008
#define F3D_Duplicate_WhenInMyLevel		0x00000010
#define F3D_Duplicate_WhenInOtherLevel	0x00000020
#define F3D_Duplicate_WhenInMyBank		0x00000040
#define F3D_Duplicate_WhenInOtherBank	0x00000080
#define F3D_Duplicate_AllLocation		0x000000F0
#define F3D_Duplicate_SubMatOfMultiMat	0x00000100
#ifdef JADEFUSION
#define F3D_Duplicate_Material_KeepInst	0x00000200
#endif

#define F3D_Duplicate_All				0xFFFFFFFF

#ifdef JADEFUSION
#define F3D_UPDATE_MTL					0
#define F3D_UPDATE_FLAGS				1
#define F3D_UPDATE_LRL					2
#define F3D_UPDATE_GRO_RLI				3
#define F3D_UPDATE_NUM					4
#endif
/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */

#define M_F3D_World		(mst_WinHandles.pst_World)
#define M_F3D_Sel		(mst_WinHandles.pst_World->pst_Selection)
#define M_F3D_DD		(mst_WinHandles.pst_DisplayData)
#define M_F3D_PB		(mst_WinHandles.pst_DisplayData->pst_PickingBuffer)
#define M_F3D_EditOpt	(mst_WinHandles.pst_DisplayData->pst_EditOptions)
#define M_F3D_Helpers	(mst_WinHandles.pst_DisplayData->pst_Helpers)

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

extern void		F3D_BeginSubObjectMode(OBJ_tdst_GameObject *);
extern void		F3D_EndSubObjectMode(OBJ_tdst_GameObject *);
extern BOOL		F3D_b_SubObject_Sel(struct SEL_tdst_SelectedItem_ *, ULONG, ULONG);
#ifdef JADEFUSION
extern void		F3D_ResetGaoClipboard(void);
#endif
/*$4
 ***********************************************************************************************************************
    globals
 ***********************************************************************************************************************
 */

extern ULONG	F3D_ul_SubObject_SelType;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

typedef struct	F3D_tdst_LinkStruct_
{
	POINT						st_Pos;
	struct OBJ_tdst_GameObject_ *pst_GO;
} F3D_tdst_LinkStruct;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern BOOL F3D_gb_PickMustRefresh;
extern BOOL OUT_b_ComputeSelectedBoudingVolume(void *, LONG, ULONG, ULONG);
#ifdef JADEFUSION
extern BOOL	F3D_b_Selection_DuplicateGro(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Flags, ULONG p2);
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class		EDI_cl_BaseFrame;
class		EDIA_cl_SelectionDialog;
class		EDIA_cl_ToolBoxDialog;
class		EDIA_cl_AnimDialog;
class		EDIA_cl_BackgroundImage;
class		F3D_cl_Frame;
class		EDIA_cl_CreateGeometry;
class		EDIA_cl_GameMaterial;
class		EDIA_cl_GaoInfo;
#ifdef JADEFUSION
class		EDIA_cl_CheckWorld;
class		EDIA_cl_ReplaceDialog;
class		EDIA_cl_CubeMapGenDialog;
class		EDIA_cl_LightRejectDialog;
class		EDIA_cl_ScreenshotSeqBuilderDlg;
#endif
class       EDIA_cl_GroVertexPos;
class       EDIA_cl_BrickMapper;

class F3D_cl_View :
	public CWnd
{
	DECLARE_DYNCREATE(F3D_cl_View)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	F3D_cl_View(void);
	~F3D_cl_View(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	F3D_cl_Frame			*mpo_Frame;
	MAI_tdst_WinHandles		mst_WinHandles;
	EDI_cl_BaseFrame		*mpo_AssociatedEditor;
	char					msz_AssociatedMadFile[_MAX_PATH];
	char					msz_ExportDir[_MAX_PATH];
	EDI_cl_UndoManager		mo_UndoManager;
	EDI_cl_UndoModif		*mpo_CurrentModif;
	BOOL					mb_AutoZoomExtendOnRotate;
	EDIA_cl_SelectionDialog *mpo_SelectionDialog;
	EDIA_cl_ToolBoxDialog	*mpo_ToolBoxDialog;
#ifdef JADEFUSION
	EDIA_cl_LightRejectDialog	*mpo_LightRejectDialog;
#endif
	EDIA_cl_AnimDialog		*mpo_AnimDialog;
	EDIA_cl_BackgroundImage *mpo_BackgroundImageDialog;
	EDIA_cl_CreateGeometry	*mpo_CreateGeometryDialog;
	EDIA_cl_GameMaterial	*mpo_GameMaterialDialog;
	EDIA_cl_GaoInfo			*mpo_GaoInfoDialog;
    EDIA_cl_GroVertexPos    *mpo_VertexPos;
#ifdef JADEFUSION
	EDIA_cl_CheckWorld		*mpo_CheckWorldDialog;
	EDIA_cl_ReplaceDialog	*mpo_ReplaceDialog;
	EDIA_cl_CubeMapGenDialog *mpo_CubeMapGenDialog;
	EDIA_cl_ScreenshotSeqBuilderDlg *mpo_ScreenshotSeqBuilderDialog;
#endif
	EDIA_cl_BrickMapper		*mpo_BrickMapper;
	BOOL					mb_SelectOn;
	BOOL					mb_DuplicateOn;
	BOOL					mb_ToolBoxOn;
	BOOL					mb_AnimOn;
	BOOL					mb_EditHierarchy;
	BOOL					mb_SelectionChange;
	BOOL					mb_LightRejectOn;

	int						mi_EditedCurve;
	struct WOR_tdst_Portal_ *mpst_EditedPortal;
	int						mi_FreezeHelper;

	BOOL					mb_LastClickInRightView;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	int							mi_MousePosX, mi_MousePosY;
	float						mf_PickedDepth;
	float						mf_OriginalAlpha;
	float						mf_Zintegration;
	BOOL						mb_BeginModePane;
	BOOL						mb_BeginModeRotate;
	BOOL						mb_Capture;
	BOOL						mb_ZoomOn;
	BOOL						mb_SelBoxOn;
	BOOL						mb_CanRefresh;
	CRect						mo_ZoomRect;
	ULONG						mst_LastViewpoint;
	LONG						ml_MouseWheelFactor;
	BOOL						mb_ConstantZoomSpeed;
	int							mi_ZoomSpeed;
	BOOL						mb_WaitForLButton;
	BOOL						mb_WaitForMButton;
	BOOL						mb_WaitForRButton;
	BOOL						mb_LButtonDblclk;
	BOOL						mb_DepthPickEnable;
	BOOL						mb_LinkOn;
	BOOL						mb_SubObject_LinkOn;
	struct OBJ_tdst_GameObject_ *mpst_LockCenter_GAO;
	int							mi_LockCenter_Point;
	BOOL						mb_HierarchicalLink;
	MATH_tdst_Vector			mst_Move;
	F3D_tdst_LinkStruct			mast_LinkNodes[1000];
	int							mi_NumLinks;
	struct OBJ_tdst_GameObject_ *mpst_LockPickObj;
	struct OBJ_tdst_GameObject_ *mpst_BonePickObj;
	BOOL						mb_LockPickObj;
	CPoint						mo_WhenLButtonDown;
	BOOL						mb_PaintMode;
	BOOL						mb_GridEdit;
	int							mi_CurCamera;
	BOOL						mb_CanSelSomeSubObject;
	int							mi_BackgroundImageMove;
	BOOL						mb_WorkWithGRORLI; // Do we work with the GRO RLI ? This boolean must not be TRUE if there is no RLI in the GRO.
	CList<F3D_tdst_PostIt, F3D_tdst_PostIt> mo_ListPostIt;


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	LRESULT								 DefWindowProc(UINT, WPARAM, LPARAM);
	void							  Refresh(void);
	void							  DisplayObjectName(POINT, BOOL = FALSE);
	struct OBJ_tdst_GameObject_				*pst_Pick(POINT);
	WAY_tdst_Link							*pst_PickLink(POINT, struct OBJ_tdst_GameObject_ **);
	ULONG							   ul_PickExt(void);
	BOOL							  b_PickDepth(MATH_tdst_Vector *, float *);
	BOOL							  b_PickOozRect(MATH_tdst_Vector *, MATH_tdst_Vector *, float *);
	BOOL							  b_PickOozRectPond(MATH_tdst_Vector *, MATH_tdst_Vector *, float *);
	BOOL							  IsActionValidate(UINT);
	void							  Redo(void);
	void							  Undo(void);
	BOOL							  b_RelevantChange(void);
	void							  Dialogs_BackgroundImage_SwapDisplay(void);
	void							  Selection_Anim(void);
	BOOL							  PickPoint3D(MATH_tdst_Vector *, struct OBJ_tdst_GameObject_ ** = NULL);

	void							  Selection_Hide(void);
	void							  Selection_Display(void);
	void							  Selection_HideAll(BOOL);
	void							  Selection_HideUnsel(void);
	void							  Selection_UnhideAll(BOOL);
	void							  Selection_Dump(void);
	void							  RenameObj(struct OBJ_tdst_GameObject_ *, char *);
	void							  Rename(void);
	void							  ChangeEditFlags(BOOL, ULONG, ULONG);

	void							  TestLockCam(void);
	void							  EndZoom(CPoint, BOOL _b_CanZoom = TRUE);
	void							  RotateCameraAroundTarget(MATH_tdst_Vector *);
	void							  MoveCamera(MATH_tdst_Vector *, char);
	BOOL							  Viewpoint_b_MoveToCamera(LONG);
	void							  Viewpoint_SetStandard(LONG);
	char							  *Viewpoint_sz_GetName(LONG);
	void							  Viewpoint_Next(void);
	void							  CameraAsObj(void);
	void							  ObjAsCamera(void);
	void							  CreateCamAtCam(void);
	void							  ZoomOnObject(CAM_tdst_Camera *, MATH_tdst_Vector *, MATH_tdst_Vector *);
	void							  ZoomExtendAll(CAM_tdst_Camera *, struct WOR_tdst_World_ *);
	void							  ZoomExtendSelected(CAM_tdst_Camera *, struct WOR_tdst_World_ *);
	void							  ZoomOnObj(OBJ_tdst_GameObject *);
	void							  Cam_FocalZoom(CPoint _o_Pt);
#ifdef JADEFUSION
	void							  SetCameraMatrix(MATH_tdst_Matrix* _pst_Matrix);
#endif
	void							  LogUnCollidable(void);
	void							  SaveWorld(void);
	void							  NewWorld(void);
	void							  ExportWorldToMad(char *, BOOL);
	void							  DropMaterial(EDI_tdst_DragDrop *);

	void							  DropGameObject(EDI_tdst_DragDrop *,BOOL);
	void							  DropPrefab(EDI_tdst_DragDrop *);
	void							  BuildGroupFromPrefab(OBJ_tdst_Group *pst_Group, OBJ_tdst_Prefab *_pst_Prefab, int _i_Index, OBJ_tdst_GameObject *_pst_GO);
	void							  DropPrefabExt(ULONG, CPoint, BOOL _b_First = TRUE, BOOL = TRUE);
	void							  DropObjectModel(EDI_tdst_DragDrop *);
	void							  DropAIModel(EDI_tdst_DragDrop *);
	void							  DropActionKit(EDI_tdst_DragDrop *);
	void							  DropSoundBank(EDI_tdst_DragDrop *);
	void							  DropSoundMetaBank(EDI_tdst_DragDrop *);
	void							  DropMorphData(EDI_tdst_DragDrop *);
	void							  DropRLIData(EDI_tdst_DragDrop *);

	void							  OneGameObjectChanged(BIG_INDEX, ULONG, INT);
	void							  OneGraphicObjectChanged(BIG_INDEX, ULONG, INT);
	void							  OneGraphicLightChanged(BIG_INDEX, ULONG, INT);
	void							  OneGraphicMaterialChanged(BIG_INDEX, ULONG, INT);

	ULONG							   ul_PickSelBox(POINT);

	void							  Optimize_Omni(void);

#ifdef JADEFUSION
    void                              CheckXMPConsistency(void);

	void							  ImportGRO_MTLassociation(char* _sz_Path, CRect* _po_Rect, char* _sz_PrefixeList = NULL);
	void							  ExportGRO_MTLassociation(char* _sz_Path, char* _sz_PrefixeList = NULL);
	void							  AssignMaterial(OBJ_tdst_GameObject* _pst_GO, BIG_KEY _ul_Key);
#endif

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    GameObject
	 -------------------------------------------------------------------------------------------------------------------
	 */

	void							  Selection_GAO_Move(MATH_tdst_Vector *);
	void							  Selection_GAO_Rotate(MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *);
	void							  Selection_GAO_ResetRotation(void);
	void							  Selection_Scale(MATH_tdst_Vector *);
	void							  Selection_GAO_ResetScale(void);

	void							  Selection_GAO_Teleport(BOOL);
	void							  Selection_GAO_DropToFloor(void);
	void							  Selection_GAO_CopyMatrix(void);
	void							  Selection_GAO_ResetXForm(void);
	void							  GAO_ResetXForm( OBJ_tdst_GameObject * );
	void							  Selection_GAO_ComputeBonePivot(void);

	void							  Selection_GAO_SetInitialPosition(void);
	void							  Selection_GAO_RestorePosition(void);
	void							  Selection_GAO_SelectAll(void);
	void							  Selection_GAO_ViewGizmo(void);
	void							  Selection_GAO_HideGizmo(void);
	void							  Selection_GAO_ToggleGizmo(void);

	struct OBJ_tdst_GameObject_		  *GAO_CreateInvisible(CPoint);
	void							  Selection_GAO_Destroy(void);
	void							  Selection_GAO_Duplicate(MATH_tdst_Vector *, BOOL,OBJ_tdst_GameObject ***,OBJ_tdst_GameObject ***,int *);
	void							  Selection_CLONE_Duplicate();
	void							  Selection_GAO_OffsetObjects(void);
	BOOL							  Selection_GAO_CopyProperties(BOOL _b_PasteGaoProperties, CRect* _po_Rect);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Zone & BV ( in F3Dview_zonebv )
	 -------------------------------------------------------------------------------------------------------------------
	 */

	void							  Selection_BV_Move(MATH_tdst_Vector *);
	void							  Selection_BV_Scale(MATH_tdst_Vector *);

	void							  Selection_Zone_Move(MATH_tdst_Vector *);
	void							  Selection_Zone_Scale(MATH_tdst_Vector *);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	ODE ( in F3Dview_ode )
	-------------------------------------------------------------------------------------------------------------------
	*/
#ifdef ODE_INSIDE
	void							  Selection_ODE_Move(MATH_tdst_Vector *);
	void							  Selection_ODE_Scale(MATH_tdst_Vector *);
	void							  Selection_ODE_Rotate(MATH_tdst_Vector *, MATH_tdst_Vector *, MATH_tdst_Vector *);
#endif//ODE_INSIDE

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Zones (in F3Dview_col)
	 -------------------------------------------------------------------------------------------------------------------
	 */

	void							  Zone_SetIndex(void);
	void							  Zone_Rename(void);
	void							  Zone_Delete(void);
	void							  Zone_Activate(void);
	void							  Zone_Desactivate(void);
	void							  Zone_CheckBV(void);
	void							  Zone_ComputeBV(void);
	void							  Zone_SwapToSpecific(void);
	void							  Zone_RestoreColSet(void);
	void							  Zone_CreateColSet(void);
	void							  Zone_CreateCob(void);
	void							  Zone_UpdateCob(BOOL);
	void							  Zone_UpdateOK3(void);
	void							  Zone_SelectObject(void);
	void							  Zone_CreateGro(void);
	void							  Zone_DestroyGameMaterial(void);

	void							  DropColSet(EDI_tdst_DragDrop *);
	void							  DropCob(EDI_tdst_DragDrop *);
	void							  DropZone(EDI_tdst_DragDrop *);
	void							  DropGameMaterial(EDI_tdst_DragDrop *);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Networks
	 -------------------------------------------------------------------------------------------------------------------
	 */

	struct OBJ_tdst_GameObject_				*CreateWaypoint(CPoint, char *_sz_Name = NULL);
	void							  LinkObjects(UINT, CPoint, BOOL);
	void							  CreateNetwork(OBJ_tdst_GameObject **);
	void							  DisplayNet(void);
	void							  SelectNetGAO(OBJ_tdst_GameObject *);
	void							  RemoveNet(BOOL);
	//void							  RemoveNetAndWaypoints(void);
	void							  RemoveLinks(void);
	void							  SelectNetRec(struct OBJ_tdst_GameObject_ *, BOOL _b_Net = TRUE);
	void							  SelectNet(void);
	void							  SelectLinks(void);
	void							  SetRoot(void);
	void							  CreateNetworkFromSelection(void);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Groups
	 -------------------------------------------------------------------------------------------------------------------
	 */

	void							  DropObjGroup(EDI_tdst_DragDrop *);
	void							  SelectGroup(BIG_INDEX, BOOL = TRUE);
	void							  Selection_Group(void);
	void							  Selection_GroupByName(char *, BOOL = TRUE);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Helper functions
	 -------------------------------------------------------------------------------------------------------------------
	 */

	BOOL							  Helper_b_IsOneUnderMouse(void);
	BOOL							  Helper_b_IsInUse(void);
	void							  Helper_Use(void);
	void							  Helper_EndUse(void);
	BOOL							  Helper_b_IsInTranslateMode(void);
	void							  Helper_SetAxis(LONG);
	BOOL							  Helper_b_IsCurrentAxis(LONG);
	BOOL							  Helper_b_AcceptAxis(LONG);
	void							  Helper_SetConstraint(LONG);
	BOOL							  Helper_b_IsCurrentConstraint(LONG);
	LONG							  Helper_l_GetDescription(char *);
	int								 Helper_i_Pick(CPoint, BOOL);
	void							  Helper_ResetFlags(void);
	void							  Helper_Reset(void);
	void							  Helper_ChangeMode(BOOL);
	void							  Helper_Treat(CPoint);
	BOOL							  Helper_b_ComputeMatrix(void);
	void							  Helper_Render(void);

	void							  Helper_AddCurve
									(
										OBJ_tdst_GameObject *,
										struct EVE_tdst_Data_ *,
										struct EVE_tdst_Track_ *
									);
	void							  Helper_DelCurve(struct EVE_tdst_Data_ *, struct EVE_tdst_Track_ *);
	void							  Helper_ReplaceCurve
									(
										OBJ_tdst_GameObject *,
										struct EVE_tdst_Data_ *,
										struct EVE_tdst_Data_ *
									);
	void							  Helper_SetInterPerSeconds(float);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Animation
	 -------------------------------------------------------------------------------------------------------------------
	 */

	void							  DropShape(EDI_tdst_DragDrop *);
	void							  CreateEvents(void);
	void							  DeleteEvents(void);
	void							  DropTrackList(EDI_tdst_DragDrop *);
	void							  CreateAnimation(void);
	void							  PopupAnimation(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *);
	void							  ActionAnimation
									(
										ULONG,
										ULONG = 0,
										OBJ_tdst_GameObject * = NULL,
										OBJ_tdst_GameObject * = NULL
									);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Gro functions (in file F3Dview_gro)
	 -------------------------------------------------------------------------------------------------------------------
	 */

	void							  CreateGeometry(void);
	void							  CreateColMap(void);
	void							  GameMaterial(void);
	void							  Selection_CleanGeo(void);
	void							  Selection_SnapGeo(void);
	void							  Selection_DuplicateGro(ULONG);
	void							  Selection_ChangeVisuFromGeoToLOD(OBJ_tdst_GameObject *);
	void							  Selection_ChangeVisuFromLODToGeo(OBJ_tdst_GameObject *);
	void							  Selection_Create2DTextVisuel(void);
	void							  Selection_CreateParticleGeneratorVisuel(void);
	void							  Selection_ComputeNormals(void);
	void							  Selection_CenterUVs(void);
	void							  Selection_CenterUVsWithMaterial(void);
	void							  Selection_SetUVs4Text(void);
	void							  Selection_AttachFace(OBJ_tdst_GameObject *);

	void							  GRO_Drop(EDI_tdst_DragDrop *);

	void							  LIGHT_ChangeOmniNearFar(LIGHT_tdst_Light *, float, float);
	void							  LIGHT_ChangeSpotAlpha(LIGHT_tdst_Light *, float, float, BOOL);
	void							  LIGHT_ChangeSpotNearFar(LIGHT_tdst_Light *, float, float);
	void							  LIGHT_ChangeShadowAlpha(LIGHT_tdst_Light *, float, float, BOOL);
	void							  LIGHT_ChangeShadowNearFar(LIGHT_tdst_Light *, float, float);

	void							  LIGHT_Drop(EDI_tdst_DragDrop *);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Sub object functions (in F3Dview_subobject.h)
	 -------------------------------------------------------------------------------------------------------------------
	 */

	void							  Selection_BeginSubObjectMode(void);
	void							  Selection_EndSubObjectMode(void);
	BOOL							  Selection_b_IsInSubObjectMode(BOOL _b_TestSelData = FALSE);
#ifdef JADEFUSION
	BOOL							  Selection_b_IsInSubObjectEdgeMode(void);
	int								  Selection_i_GetSubObjectMode(void);
	int								  Selection_i_GetSubObjectEdgeMode(void);
	void							  Selection_SwitchSubObjectMode(void);
	void							  Selection_SwitchSubObjectEdgeMode(void);
#endif
	void							  Selection_ColorVertices(ULONG, float, BOOL);
	void							  Selection_PickVertexColor(void);
	BOOL							  Selection_b_SubObject_VertexWeld(POINT *);
	void							  Selection_SubObject_VertexWeldThresh( float );
	void							  Selection_SubObject_DelVertex(void);
	void							  Selection_SubObject_VertexSym(void);
	void							  Selection_SubObject_PivotCenter( int, MATH_tdst_Vector * );
	void							  Selection_TurnEdge(void);
	void							  Selection_CutEdge(POINT *);
	void							  Selection_EdgeCutter(POINT *, POINT *);
	void							  Selection_ExtrudeEdge(void);
	void							  Selection_SubObject_FlipNormals(BOOL _b_CanUndo, BOOL _b_Locked);
	void							  Selection_SubObject_ChangeId(int);
	void							  Selection_SubObject_DelFace(void);
	void							  Selection_SubObject_DetachFace(void);
	void							  Selection_SubObject_AfterDetachFace(MATH_tdst_Matrix *);
	void							  Selection_SubObject_DuplicateFace(void);
	void							  Selection_SubObject_ExtrudeFace(void);
	void							  Selection_SubObject_SelFaceExt(void);
	void							  Selection_SubObject_InvertSel(void);
	BOOL							  Selection_b_SubObject_CutFace( struct OBJ_tdst_GameObject_ *, ULONG, POINT *, BOOL = FALSE );
	void							  Selection_SubObject_FaceOrder(void);
	void							  Selection_SubObjectTreat(POINT *);
	BOOL							  Selection_b_SubObject_TreatAnyway(POINT *);
	BOOL							  Selection_b_SubObjectTreat_OnLButtonUp(POINT *);
	void							  Selection_SubObjectMove(MATH_tdst_Vector *, POINT *, BOOL, BOOL);
	void							  Selection_SubObjectRotate(MATH_tdst_Vector *, BOOL);
	void							  Selection_SubObjectScale(MATH_tdst_Vector *);
	void							  Selection_SubObjectTeleport( BOOL );
	void							  Selection_SubObject_CenterOfGravity(MATH_tdst_Vector *);
	void							  Selection_SubObject_StartMovingVertex(int);

	void							  Selection_SubObject_CannotSelAll(void);
	BOOL							  Selection_b_SubObject_Pick(MATH_tdst_Vector *, char);
	BOOL							  Selection_b_SubObject_PickBox(MATH_tdst_Vector *, MATH_tdst_Vector *);
	void							  Selection_SubObject_Sel(char, char);
	void							  Selection_SubObject_SelForEdge(void);
	void							  Selection_SubObject_SelForFace(void);

	void							  Selection_SubObject_VertexPos(void);
    void                              Selection_SubObject_CreateVertexPos(void);
    void                              Selection_SubObject_DestroyVertexPos(void);

	void							  Selection_SubObject_Link(POINT *, BOOL, BOOL);

	void							  Selection_SubObject_HideFace( BOOL );
	void							  Selection_SubObject_UnHideFace(void);

#ifdef JADEFUSION
    void                              Selection_XenonRefresh(void);
    void                              Selection_XenonPostOperation(struct SEL_tdst_Selection_* _pst_Sel, BOOL _b_Pack, BOOL _b_QuickUpdate);
#endif

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Selection functions
	 -------------------------------------------------------------------------------------------------------------------
	 */

	void							  SpeedSelect(void);
	void							  Selection_Change(void);
#ifdef JADEFUSION
	void							  Selection_DuplicateCob(int, OBJ_tdst_GameObject * = NULL);
#else
	void							  Selection_DuplicateCob(int = -1);
#endif
	BOOL							  Selection_b_IsLocked(void);
	void							  Selection_WithDialog(BOOL = TRUE);
	void							  Selection_ToolBox(BOOL = TRUE);
	OBJ_tdst_GameObject						*Selection_pst_GetFirstObject(void);
#ifdef JADEFUSION
	void							  Selection_LightReject(BOOL = TRUE);
#endif
	BOOL							  Selection_b_Prefab
									(
										struct OBJ_tdst_GameObject_ *,
										ULONG,
										BOOL = TRUE,
										BOOL = TRUE
									);
	void							  Selection_Object(struct OBJ_tdst_GameObject_ *, ULONG, BOOL = TRUE, BOOL = TRUE);
	void							  Selection_Link(WAY_tdst_Link *, ULONG);
	void							  Selection_Zone(void *, ULONG, ULONG);
	void							  Selection_Unselect(void *, LONG, BOOL = TRUE);
	BOOL							  Selection_b_Treat(void *, LONG, LONG, BOOL, BOOL = FALSE);
	void							  ForceSelectObject(void *, BOOL _b_Desel = TRUE);
	void							  Selection_EndBox(CPoint, BOOL, BOOL, BOOL);
#ifdef JADEFUSION
	void                              Selection_HighlightedObjects(void);
#endif

	BOOL							  Selection_b_IsSelected(void *, LONG);
	struct SEL_tdst_SelectedItem_			*Selection_pst_RetrieveItem(void *, LONG);

	void							  Grid_Create(int);
	void							  Grid_Destroy(int);
	void							  Grid_EditSwap(void);
	BOOL							  Grid_b_InEdit(void);
	void							  Grid_Paint(struct GRID_tdst_World_ *, int, int, BOOL);
	BOOL							  Grid_b_Paint(int, int, BOOL);
	void							  Grid_ResetSel(void);
	void							  Grid_CopySel(struct WOR_tdst_World_ *);
	void							  Grid_PasteSel(struct WOR_tdst_World_ *, int, int);
	void							  Grid_SetWidth(int, int);
	void							  Grid_SetHeight(int, int);
	void							  Pick_PointW2H(MATH_tdst_Vector *, POINT *);
	void							  Pick_PointH2PB(POINT *, MATH_tdst_Vector *);
	LONG							  Pick_l_UnderPoint(POINT *, LONG, LONG);
	LONG							  Pick_l_InBox(POINT *, POINT *, LONG);
	struct SOFT_tdst_PickingBuffer_Pixel_	*Pick_pst_GetFirst(ULONG, LONG);
	struct SOFT_tdst_PickingBuffer_Pixel_	*Pick_pst_GetNext(ULONG, LONG, struct SOFT_tdst_PickingBuffer_Pixel_ *);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    RLI functions
	 -------------------------------------------------------------------------------------------------------------------
	 */
    void	RadiosityLightmap(void);
	void	RLITool(void);
	void	ComputeRLI(ULONG, BOOL, BOOL, BOOL);
	void	Selection_ResetRLI(ULONG, ULONG, BOOL);
	void	Selection_DestroyRLI(ULONG, BOOL);
	void	Selection_CopyLocal2Global(ULONG, BOOL);
	void	Selection_CopyGlobal2Local(ULONG, BOOL);
	void	Selection_RLIComputeAlpha2Color(BOOL);
	void	Selection_RLILock(BOOL);
	void	Selection_RLIUnlock(BOOL);
	void	Selection_RLIInvertAlpha(void);
	void	Selection_RLIAddColor(ULONG, ULONG, BOOL);

    /*$2
    -------------------------------------------------------------------------------------------------------------------
    Lightmaps functions
    -------------------------------------------------------------------------------------------------------------------
    */
    static void	LightMapProgressCallback(float _percent, char* _msg, void* _this);
    void		ComputeLightmaps(SEL_tdst_Selection* pSelection, bool _bNoDialog);
    void		DestroyLightmaps(void);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Brick mapper functions
	 -------------------------------------------------------------------------------------------------------------------
	 */
	void	BrickMapper(void);
	SEL_tdst_SelectedItem *GetFirstSel();
	SEL_tdst_SelectedItem *GetNextSel(SEL_tdst_SelectedItem *p_Sel);
	void Replace(void *_p_Old, void *_p_New);
	OBJ_tdst_Prefab *BrickLoad(BIG_INDEX h_File);
	void BrickUpdate(OBJ_tdst_GameObject *p_GO);
	void BrickDetach(OBJ_tdst_GameObject *p_GO);
	void BrickMapperClose();
	void BrickMapperSave(OBJ_tdst_GameObject *pst_GO);


	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Wow display functions
	 -------------------------------------------------------------------------------------------------------------------
	 */
	void DisplayWow(void);
	//static void sWowDisplayRefresh(void *, void *_p_Sel, void *_p_Data, long);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    curve functions
	 -------------------------------------------------------------------------------------------------------------------
	 */

	void	Curve_Move(MATH_tdst_Vector *);
	void	Curve_Rotate(MATH_tdst_Vector *, MATH_tdst_Vector *);
	BOOL	Curve_b_GetData(void);
	BOOL	Curve_b_ComputeHelperMatrix(MATH_tdst_Matrix *);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    portal functions
	 -------------------------------------------------------------------------------------------------------------------
	 */

	void	Portal_Move(MATH_tdst_Vector *);
	void	Portal_Rotate(MATH_tdst_Vector *, MATH_tdst_Vector *);
	void	Portal_Scale(MATH_tdst_Vector *);
	BOOL	Portal_b_ComputeHelperMatrix(MATH_tdst_Matrix *);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    postit functions
	 -------------------------------------------------------------------------------------------------------------------
	 */

	void	KillPostIt(void);
	void	PostItPath(char *, BOOL = FALSE);
	void	LoadPostIt(void);
	BOOL	PostItFindName(char *, char *);
	void	CreatePostIt(void);
	void	DestroyPostIt(void);
	BOOL	PostItPickMouse(void);
	void	PostItRefresh(F3D_tdst_PostIt *);

    /*$2
    -------------------------------------------------------------------------------------------------------------------
        Modifier
    -------------------------------------------------------------------------------------------------------------------
    */
    void    MDF_CreateSoftBody();

    /*$2
    -------------------------------------------------------------------------------------------------------------------
    Tangent Space
    -------------------------------------------------------------------------------------------------------------------
    */
    void    XeComputeTangentSpaceSmoothing(ULONG _ul_Mask);

    /*$2
    -------------------------------------------------------------------------------------------------------------------
    Fix RLI
    -------------------------------------------------------------------------------------------------------------------
    */
    void    XeSelectionFixRLI(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnMouseMove(UINT, CPoint);
	afx_msg BOOL	OnMouseWheel(UINT, short, CPoint);
	afx_msg void	OnMButtonDown(UINT, CPoint);
	afx_msg void	OnMButtonUp(UINT, CPoint);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonUp(UINT, CPoint);
	afx_msg void	OnLButtonDblClk(UINT, CPoint);
	afx_msg void	OnRButtonDown(UINT, CPoint);
	afx_msg BOOL	OnSetCursor(CWnd *, UINT, UINT);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
