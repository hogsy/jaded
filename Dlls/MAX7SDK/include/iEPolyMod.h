/**********************************************************************
 *<
	FILE: iEPolyMod.h

	DESCRIPTION:   Interface for Edit Poly modifier.

	CREATED BY: Steve Anderson

	HISTORY: created March 2004

 *>	Copyright (c) 2004, All Rights Reserved.
 **********************************************************************/

#ifndef ___EDIT_POLY_MOD_INTERFACE___
#define ___EDIT_POLY_MOD_INTERFACE___

#include "iFnPub.h"

#define EPOLY_MOD_INTERFACE Interface_ID(0x405e217d, 0x49c2abc)
#define GetEPolyModInterface(cd) (EPolyMod *)(cd)->GetInterface(EPOLY_MOD_INTERFACE)

// Editable Poly Selection Levels
enum ePolyModSelLevel { EPM_SL_OBJECT, EPM_SL_VERTEX, EPM_SL_EDGE, 
	EPM_SL_BORDER, EPM_SL_FACE, EPM_SL_ELEMENT, EPM_SL_CURRENT };

// Relate EPolyMod selection levels to MNMesh selection levels:
const int meshSelLevel[] = { MNM_SL_OBJECT, MNM_SL_VERTEX, MNM_SL_EDGE, MNM_SL_EDGE,
	MNM_SL_FACE, MNM_SL_FACE, MNM_SL_CURRENT };

// Edit Poly Parameters:
enum epolyModParameters {
	// Main dialog parameters:
	epm_animation_mode, epm_show_cage,

	// Selection Parameters:
	epm_by_vertex, epm_ignore_backfacing,
	// Whether or not we're using pipeline selection in Edit Poly:
	epm_stack_selection,

	// Soft selection parameters:
	epm_ss_use, epm_ss_edist_use, epm_ss_edist,
	epm_ss_affect_back, epm_ss_falloff, epm_ss_pinch, epm_ss_bubble,

	// Animate-mode dialog parameters and operation settings:
	epm_current_operation,

	// Worldspace to common Objectspace transform.  Used in combination with the local mod context tms,
	// this can give us relative WS-to-OS transforms for each modifier instance.
	epm_world_to_object_transform,

	// Edit Geom dialog parameters and operation settings:
	epm_constrain_type, epm_split,
	epm_ms_smoothness, epm_ms_sep_smooth, epm_ms_sep_mat,
	epm_tess_type, epm_tess_tension,
	epm_bridge_taper, epm_bridge_bias,
	// Whether to align to view (0) or construction plane (1) in ep_op_align
	epm_align_type,
	// Parameters describing the current align plane.
	epm_align_plane_normal, epm_align_plane_offset,
	epm_delete_isolated_verts,

	// Vertex operation settings:
	epm_weld_vertex_threshold, epm_chamfer_vertex,
	epm_extrude_vertex_width, epm_extrude_vertex_height,

	// Edge operation settings:
	epm_connect_edge_segments, epm_weld_edge_threshold,
	epm_extrude_edge_height, epm_extrude_edge_width, epm_chamfer_edge,

	// Face operation settings:
	epm_extrude_face_type, epm_extrude_face_height,
	epm_bevel_type, epm_bevel_height, epm_bevel_outline,
	epm_inset_type, epm_inset, epm_outline,
	epm_hinge_angle, epm_hinge_base, epm_hinge_dir, epm_hinge_segments,
	epm_extrude_spline_node, epm_extrude_spline_segments,
	epm_extrude_spline_taper, epm_extrude_spline_taper_curve,
	epm_extrude_spline_twist, epm_extrude_spline_rotation,
	epm_extrude_spline_align,

	epm_autosmooth_threshold, epm_smooth_group_set, epm_smooth_group_clear,
	epm_material, epm_material_selby, epm_material_selby_clear,
	epm_smoother_selby, epm_smoother_selby_clear,

	epm_clone_to_element, epm_bridge_segments, epm_bridge_smooth_thresh,
	// Target 1 and target 2 contain either edge or face indices, in 1-based indexing.
	// Value of 0 indicates not yet set.
	epm_bridge_target_1, epm_bridge_target_2,
	epm_bridge_twist_1, epm_bridge_twist_2,
	epm_bridge_selected,
	epm_preserve_maps,
	epm_relax_amount, epm_relax_iters, epm_relax_hold_boundary, epm_relax_hold_outer,
	epm_select_by_angle, epm_select_angle,

	// Paint tools
	epm_ss_lock,
	epm_paintsel_value, epm_paintsel_size, epm_paintsel_strength,
	epm_paintdeform_value, epm_paintdeform_size, epm_paintdeform_strength,
	epm_paintdeform_axis,
};

// Edit Poly Operations:
enum epolyModButtonOp { ep_op_null,
	ep_op_hide_vertex, ep_op_hide_face,
	ep_op_unhide_vertex, ep_op_unhide_face,
	ep_op_hide_unsel_vertex, ep_op_hide_unsel_face,
	ep_op_ns_copy, ep_op_ns_paste, ep_op_cap,
	ep_op_delete_vertex, ep_op_delete_face, ep_op_detach_vertex, ep_op_detach_face,
	ep_op_split, ep_op_break, ep_op_collapse_vertex, ep_op_collapse_edge,
	ep_op_collapse_face, ep_op_reset_plane, ep_op_slice, ep_op_slice_face,
	ep_op_weld_vertex, ep_op_weld_edge, ep_op_create_shape, ep_op_make_planar, ep_op_remove_iso_verts,
	ep_op_meshsmooth, ep_op_tessellate, ep_op_retriangulate,
	ep_op_flip_face, ep_op_flip_element, ep_op_bevel, ep_op_chamfer_vertex,
	ep_op_chamfer_edge, ep_op_cut,
	ep_op_sel_grow, ep_op_sel_shrink, ep_op_inset, ep_op_extrude_vertex,
	ep_op_extrude_edge, ep_op_extrude_face,
	ep_op_extrude_along_spline, ep_op_connect_edge, ep_op_connect_vertex,
	ep_op_hinge_from_edge, ep_op_select_ring, ep_op_select_loop,
	ep_op_remove_iso_map_verts, ep_op_remove_vertex, ep_op_remove_edge, ep_op_outline,
	ep_op_toggle_shaded_faces,
	ep_op_transform, ep_op_delete_edge, ep_op_unused,
	ep_op_create, ep_op_align,
	ep_op_attach, ep_op_target_weld_vertex, ep_op_target_weld_edge,
	ep_op_edit_triangulation, ep_op_create_edge,
	ep_op_clone_vertex, ep_op_clone_edge, ep_op_clone_face,
	ep_op_insert_vertex_edge, ep_op_insert_vertex_face,
	ep_op_autosmooth, ep_op_smooth, ep_op_set_material,
	ep_op_select_by_smooth, ep_op_select_by_material, ep_op_get_stack_selection,
	ep_op_bridge_border, ep_op_bridge_polygon, ep_op_make_planar_x, ep_op_make_planar_y,
	ep_op_make_planar_z, ep_op_turn_edge, ep_op_relax, ep_op_paint_deform
};

// Edit Poly Command modes:
enum epolyModCommandMode { ep_mode_create_vertex, ep_mode_create_edge,
	ep_mode_create_face, ep_mode_divide_edge, ep_mode_divide_face,
	ep_mode_extrude_vertex, ep_mode_extrude_edge, ep_mode_extrude_face,
	ep_mode_chamfer_vertex, ep_mode_chamfer_edge, ep_mode_bevel,
	ep_mode_sliceplane, ep_mode_cut, ep_mode_weld, ep_mode_edit_tri,
	ep_mode_inset_face, ep_mode_quickslice, ep_mode_hinge_from_edge,
	ep_mode_pick_hinge, ep_mode_outline, ep_mode_bridge_border,
	ep_mode_bridge_polygon, ep_mode_pick_bridge_1, ep_mode_pick_bridge_2,
	ep_mode_turn_edge
};

// Edit Poly Pick modes:
enum epolyModPickMode { ep_mode_attach, ep_mode_pick_shape };

enum epolyModMethods {
	epmod_get_sel_level, epmod_get_mn_sel_level, epmod_set_sel_level,
	epmod_get_operation, epmod_set_operation, epmod_local_data_changed,
	epmod_refresh_screen, epmod_commit, epmod_commit_unless_animating,
	epmod_commit_and_repeat, epmod_cancel_operation, epmod_convert_selection, epmod_get_selection,
	epmod_set_selection, epmod_select, epmod_set_primary_node,
	epmod_toggle_command_mode, epmod_enter_command_mode, epmod_enter_pick_mode,
	epmod_get_command_mode, epmod_get_pick_mode,
	epmod_popup_dialog, epmod_button_op, epmod_repeat_last, epmod_in_slice_mode,
	epmod_reset_slice_plane, epmod_get_slice_plane_tm, epmod_get_slice_plane, epmod_set_slice_plane,
	epmod_create_vertex, epmod_create_face, epmod_create_edge, epmod_set_diagonal,
	epmod_cut, epmod_target_weld_vertex, epmod_target_weld_edge, epmod_attach_node,
	epmod_attach_nodes, epmod_detach_to_object, epmod_create_shape,
	epmod_bridge_borders, epmod_bridge_polygons, epmod_set_bridge_node, epmod_get_bridge_node,
	epmod_ready_to_bridge_selected, epmod_get_preserve_map, epmod_set_preserve_map,
	epmod_turn_diagonal,

	// Access to information about mesh:
	epmod_get_num_vertices, epmod_get_vertex, epmod_get_vertex_face_count, epmod_get_vertex_face,
	epmod_get_vertex_edge_count, epmod_get_vertex_edge, epmod_get_num_edges, epmod_get_edge_vertex,
	epmod_get_edge_face, epmod_get_num_faces, epmod_get_face_degree, epmod_get_face_vertex,
	epmod_get_face_edge, epmod_get_face_diagonal, epmod_get_face_material, epmod_get_face_smoothing_group,
	epmod_get_num_map_channels, epmod_get_map_channel_active,
	epmod_get_num_map_vertices, epmod_get_map_vertex, epmod_get_map_face_vertex,

	epmod_move_selection, epmod_move_slicer, epmod_rotate_selection, epmod_rotate_slicer,
	epmod_scale_selection, epmod_scale_slicer,

	epmod_list_operations, epmod_set_hinge_edge, epmod_get_hinge_edge
};

class EPolyMod : public FPMixinInterface {
public:
	BEGIN_FUNCTION_MAP
		FN_0(epmod_get_sel_level, TYPE_ENUM, GetEPolySelLevel);
		FN_0(epmod_get_mn_sel_level, TYPE_ENUM, GetMNSelLevel);
		VFN_1(epmod_set_sel_level, SetEPolySelLevel, TYPE_ENUM);

		FN_3(epmod_convert_selection, TYPE_INT, EpModConvertSelection, TYPE_ENUM, TYPE_ENUM, TYPE_bool);

		FN_0(epmod_get_operation, TYPE_ENUM, GetPolyOperationID);
		VFN_1(epmod_set_operation, EpModSetOperation, TYPE_ENUM);
		VFN_1(epmod_popup_dialog, EpModPopupDialog, TYPE_ENUM);
		VFN_1(epmod_button_op, EpModButtonOp, TYPE_ENUM);
		VFN_0(epmod_repeat_last, EpModRepeatLast);

		VFNT_0(epmod_commit, EpModCommit);
		VFNT_0(epmod_commit_unless_animating, EpModCommitUnlessAnimating);
		VFNT_0(epmod_commit_and_repeat, EpModCommitAndRepeat);
		VFN_0(epmod_cancel_operation, EpModCancel);

		FN_2(epmod_get_selection, TYPE_BITARRAY, EpModGetSelection, TYPE_ENUM, TYPE_INODE);
		FN_3(epmod_set_selection, TYPE_bool, EpModSetSelection, TYPE_ENUM, TYPE_BITARRAY_BR, TYPE_INODE);
		FN_5(epmod_select, TYPE_bool, EpModSelect, TYPE_ENUM, TYPE_BITARRAY_BR, TYPE_bool, TYPE_bool, TYPE_INODE);

		VFN_1(epmod_set_primary_node, EpModSetPrimaryNode, TYPE_INODE);

		VFN_1(epmod_toggle_command_mode, EpModToggleCommandMode, TYPE_ENUM);
		VFN_1(epmod_enter_command_mode, EpModEnterCommandMode, TYPE_ENUM);
		FN_0(epmod_get_command_mode, TYPE_ENUM, EpModGetCommandMode);
		VFN_1(epmod_enter_pick_mode, EpModEnterPickMode, TYPE_ENUM);
		FN_0(epmod_get_pick_mode, TYPE_ENUM, EpModGetPickMode);

		// Transforms:
		VFNT_4(epmod_move_selection, EpModMoveSelection, TYPE_POINT3_BR, TYPE_MATRIX3_BR, TYPE_MATRIX3_BR, TYPE_BOOL);
		VFNT_4(epmod_rotate_selection, EpModRotateSelection, TYPE_QUAT_BR, TYPE_MATRIX3_BR, TYPE_MATRIX3_BR, TYPE_BOOL);
		VFNT_4(epmod_scale_selection, EpModScaleSelection, TYPE_POINT3_BR, TYPE_MATRIX3_BR, TYPE_MATRIX3_BR, TYPE_BOOL);

		VFNT_3(epmod_move_slicer, EpModMoveSlicePlane, TYPE_POINT3_BR, TYPE_MATRIX3_BR, TYPE_MATRIX3_BR);
		VFNT_4(epmod_rotate_slicer, EpModRotateSlicePlane, TYPE_QUAT_BR, TYPE_MATRIX3_BR, TYPE_MATRIX3_BR, TYPE_BOOL);
		VFNT_4(epmod_scale_slicer, EpModScaleSlicePlane, TYPE_POINT3_BR, TYPE_MATRIX3_BR, TYPE_MATRIX3_BR, TYPE_BOOL);

		// Access to slice plane:
		FN_0(epmod_in_slice_mode, TYPE_bool, EpInSliceMode);
		VFN_0(epmod_reset_slice_plane, EpResetSlicePlane);
		FNT_0(epmod_get_slice_plane_tm, TYPE_MATRIX3_BV, EpGetSlicePlaneTM);
		VFNT_2(epmod_get_slice_plane, EpGetSlicePlane, TYPE_POINT3_BR, TYPE_POINT3_BR);
		VFNT_2(epmod_set_slice_plane, EpSetSlicePlane, TYPE_POINT3_BR, TYPE_POINT3_BR);

		FN_2(epmod_create_vertex, TYPE_INDEX, EpModCreateVertex, TYPE_POINT3, TYPE_INODE);
		FN_2(epmod_create_face, TYPE_INDEX, EpModCreateFace, TYPE_INDEX_TAB, TYPE_INODE);
		FN_3(epmod_create_edge, TYPE_INDEX, EpModCreateEdge, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);
		VFN_3(epmod_set_diagonal, EpModSetDiagonal, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);
		VFN_5(epmod_cut, EpModCut, TYPE_ENUM, TYPE_INDEX, TYPE_POINT3, TYPE_POINT3, TYPE_INODE);
		VFN_3(epmod_target_weld_vertex, EpModWeldVerts, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);
		VFN_3(epmod_target_weld_edge, EpModWeldEdges, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);

		VFNT_2(epmod_attach_node, EpModAttach, TYPE_INODE, TYPE_INODE);
		VFNT_2(epmod_attach_nodes, EpModMultiAttach, TYPE_INODE_TAB_BR, TYPE_INODE);

		VFNT_1(epmod_detach_to_object, EpModDetachToObject, TYPE_TSTR_BR);
		VFNT_1(epmod_create_shape, EpModCreateShape, TYPE_TSTR_BR);

		VFN_3(epmod_set_hinge_edge, EpModSetHingeEdge, TYPE_INDEX, TYPE_MATRIX3, TYPE_INODE);
		FN_1 (epmod_get_hinge_edge, TYPE_INDEX, EpModGetHingeEdge, TYPE_INODE);
		VFN_3(epmod_bridge_borders, EpModBridgeBorders, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);
		VFN_3(epmod_bridge_polygons, EpModBridgePolygons, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);
		FN_0(epmod_get_bridge_node, TYPE_INODE, EpModGetBridgeNode);
		VFN_1(epmod_set_bridge_node, EpModSetBridgeNode, TYPE_INODE);
		FN_0(epmod_ready_to_bridge_selected, TYPE_bool, EpModReadyToBridgeSelected);

		FN_1(epmod_get_preserve_map, TYPE_bool, EpModGetPreserveMap, TYPE_INT);
		VFN_2(epmod_set_preserve_map, EpModSetPreserveMap, TYPE_INT, TYPE_bool);

		//VFN_2(epmod_turn_edge, EpModTurnEdge, TYPE_INDEX, TYPE_INODE);
		VFN_3(epmod_turn_diagonal, EpModTurnDiagonal, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);

		// Mesh access methods:
		FN_1 (epmod_get_num_vertices, TYPE_INT, EpMeshGetNumVertices, TYPE_INODE);
		FN_2 (epmod_get_vertex, TYPE_POINT3_BV, EpMeshGetVertex, TYPE_INDEX, TYPE_INODE);
		FN_2 (epmod_get_vertex_face_count, TYPE_INT, EpMeshGetVertexFaceCount, TYPE_INDEX, TYPE_INODE);
		FN_3 (epmod_get_vertex_face, TYPE_INDEX, EpMeshGetVertexFace, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);
		FN_2 (epmod_get_vertex_edge_count, TYPE_INT, EpMeshGetVertexEdgeCount, TYPE_INDEX, TYPE_INODE);
		FN_3 (epmod_get_vertex_edge, TYPE_INDEX, EpMeshGetVertexEdge, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);

		FN_1 (epmod_get_num_edges, TYPE_INT, EpMeshGetNumEdges, TYPE_INODE);
		FN_3 (epmod_get_edge_vertex, TYPE_INDEX, EpMeshGetEdgeVertex, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);
		FN_3 (epmod_get_edge_face, TYPE_INDEX, EpMeshGetEdgeFace, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);

		FN_1 (epmod_get_num_faces, TYPE_INT, EpMeshGetNumFaces, TYPE_INODE);
		FN_2 (epmod_get_face_degree, TYPE_INT, EpMeshGetFaceDegree, TYPE_INDEX, TYPE_INODE);
		FN_3 (epmod_get_face_vertex, TYPE_INDEX, EpMeshGetFaceVertex, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);
		FN_3 (epmod_get_face_edge, TYPE_INDEX, EpMeshGetFaceEdge, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);
		FN_4 (epmod_get_face_diagonal, TYPE_INDEX, EpMeshGetFaceDiagonal, TYPE_INDEX, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);
		FN_2 (epmod_get_face_material, TYPE_INDEX, EpMeshGetFaceMaterial, TYPE_INDEX, TYPE_INODE);
		FN_2 (epmod_get_face_smoothing_group, TYPE_DWORD, EpMeshGetFaceSmoothingGroup, TYPE_INDEX, TYPE_INODE);

		FN_1 (epmod_get_num_map_channels, TYPE_INT, EpMeshGetNumMapChannels, TYPE_INODE);
		FN_2 (epmod_get_map_channel_active, TYPE_bool, EpMeshGetMapChannelActive, TYPE_INT, TYPE_INODE);
		FN_2 (epmod_get_num_map_vertices, TYPE_INT, EpMeshGetNumMapVertices, TYPE_INT, TYPE_INODE);
		FN_3 (epmod_get_map_vertex, TYPE_POINT3_BV, EpMeshGetMapVertex, TYPE_INT, TYPE_INDEX, TYPE_INODE);
		FN_4 (epmod_get_map_face_vertex, TYPE_INDEX, EpMeshGetMapFaceVertex, TYPE_INT, TYPE_INDEX, TYPE_INDEX, TYPE_INODE);

		VFN_1 (epmod_list_operations, EpModListOperations, TYPE_INODE);
	END_FUNCTION_MAP

	FPInterfaceDesc *GetDesc ();

	virtual int GetEPolySelLevel()=0;
	virtual int GetMNSelLevel()=0;
	virtual void SetEPolySelLevel (int sl)=0;

	virtual int EpModConvertSelection (int epSelLevelFrom, int epSelLevelTo, bool requireAll) { return 0; }

	virtual int GetPolyOperationID () { return ep_op_null; }
	virtual IParamBlock2 *getParamBlock () { return NULL; }
	virtual MapBitArray GetPreserveMapSettings () const { return MapBitArray(true, false); }
	virtual void SetPreserveMapSettings (const MapBitArray & mapSettings) { }
	virtual void EpModSetPreserveMap (int mapChannel, bool preserve) { }
	virtual bool EpModGetPreserveMap (int mapChannel) { return (mapChannel>0); }

	virtual void InvalidateDistanceCache () { }
	virtual void InvalidateSoftSelectionCache () { }

	virtual void EpModSetOperation (int opcode) { }
	virtual void EpModPopupDialog (int opcode) { EpModSetOperation (opcode); EpModShowOperationDialog(); }
	virtual void EpModButtonOp (int opcode) { }
	virtual void EpModLocalDataChanged (DWORD parts) { }
	virtual void EpModRefreshScreen () { }

	virtual void EpModCommitUnlessAnimating (TimeValue t) { }
	virtual void EpModCommit (TimeValue t) { }
	virtual void EpModCommitAndRepeat (TimeValue t) { }
	virtual void EpModCancel () { }

	virtual void EpModToggleCommandMode (int mode) { }
	virtual void EpModEnterCommandMode (int mode) { }
	virtual void EpModEnterPickMode (int mode) { }
	virtual int EpModGetCommandMode () { return -1; }
	virtual int EpModGetPickMode () { return -1; }

	// Selection access.
	virtual BitArray *EpModGetSelection (int meshSelLevel, INode *pNode=NULL) { return NULL; }
	virtual bool EpModSetSelection (int meshSelLevel, BitArray & selection, INode *pNode=NULL) { return false; }
	virtual bool EpModSelect (int meshSelLevel, BitArray & selection, bool invert=false, bool select=true, INode *pNode=NULL) { return false; }

	virtual void SetHitLevelOverride (DWORD hlo) { }
	virtual void ClearHitLevelOverride () { }
	virtual DWORD GetHitLevelOverride () { return 0x0; }
	virtual DWORD CurrentHitLevel (int *selByVert=NULL) { return 0x0; }

	virtual void SetHitTestResult (bool ignoreNew=false) { }
	virtual void ClearHitTestResult () { }

	virtual void SetDisplayLevelOverride (DWORD dlo) { }
	virtual void ClearDisplayLevelOverride () { }
	virtual DWORD GetDisplayLevelOverride () { return 0x0; }

	// Also used by command modes where needed:
	virtual void ForceIgnoreBackfacing (bool force) { }
	virtual bool GetForceIgnoreBackfacing () { return false; }

	// Transforms.
	virtual void EpModMoveSelection (Point3 &val, Matrix3 &partm, Matrix3 &tmAxis, BOOL localOrigin, TimeValue t) { }
	virtual void EpModRotateSelection (Quat &val, Matrix3 &partm, Matrix3 &tmAxis, BOOL localOrigin, TimeValue t) { }
	virtual void EpModScaleSelection (Point3 &val, Matrix3 &partm, Matrix3 &tmAxis, BOOL localOrigin, TimeValue t) { }

	virtual void EpModMoveSlicePlane (Point3 &val, Matrix3 &partm, Matrix3 &tmAxis, TimeValue t) { }
	virtual void EpModRotateSlicePlane (Quat &val, Matrix3 &partm, Matrix3 &tmAxis, BOOL localOrigin, TimeValue t) { }
	virtual void EpModScaleSlicePlane (Point3 &val, Matrix3 &partm, Matrix3 &tmAxis, BOOL localOrigin, TimeValue t) { }

	// Slice plane accessors:
	virtual void EpResetSlicePlane () { }
	virtual Matrix3 EpGetSlicePlaneTM (TimeValue t) { return Matrix3(true); }
	virtual void EpGetSlicePlane (Point3 & planeNormal, Point3 & planeCenter, TimeValue t) { }
	virtual void EpSetSlicePlane (Point3 & planeNormal, Point3 & planeCenter, TimeValue t) { }
	virtual bool EpInSliceMode () { return false; }
	virtual bool EpInSlice () { return false; }

	virtual Modifier *GetModifier ()=0;
	// Will return NULL if this EditPoly not currently being modified.
	virtual IObjParam *EpModGetIP() { return NULL; }

	virtual HWND GetDlgHandle(int paramPanelID) { return NULL; }

	/// <summary>
	/// Updates the Align normal and offset based on the current viewport.
	/// If there's no viewport available, method does nothing.
	/// </summary>
	/// <returns>true for parameters updated, false otherwise.</returns>
	virtual void UpdateAlignParameters (TimeValue t) { }
	virtual void UpdateCache(TimeValue t) { }

	virtual bool EpModShowOperationDialog () { return false; }
	virtual bool EpModShowingOperationDialog () { return false; }
	virtual void EpModCloseOperationDialog () { }

	virtual void EpModRepeatLast () { }

	// Methods related to one particular node:

	// Get or set the "Primary" node, which is used by default when node-specific commands are given.
	virtual INode *EpModGetPrimaryNode () { return NULL; }
	virtual void EpModSetPrimaryNode (INode *node) { }

	// In all of these methods, if node is left at NULL, the primary node is used.
	// (This is especially useful if you have an Edit Poly modifier applied to only one node,
	// you can just forget about the node argument.)
	// If no primary is previously established, one is picked to be primary.
	virtual Matrix3 EpModGetNodeTM (TimeValue t, INode *node=NULL) { return Matrix3(1); }

	virtual int EpModCreateVertex (Point3 p, INode *pNode=NULL) { return 0; }
	virtual int EpModCreateFace (Tab<int> *vertex, INode *pNode=NULL) { return 0; }
	virtual int EpModCreateEdge (int v1, int v2, INode *pNode=NULL) { return 0; }
	virtual void EpModSetDiagonal (int v1, int v2, INode *pNode=NULL) { }
	virtual void EpModCut (int startLevel, int startIndex, Point3 startPoint, Point3 normal, INode *pNode=NULL) { }
	virtual void EpModSetCutEnd (Point3 endPoint, INode *pNode=NULL) { }
	virtual int EpModGetLastCutEnd () { return -1; }
	virtual void EpModClearLastCutEnd () { }
	virtual void EpModCutCancel (INode *pNode=NULL) { }
	virtual void EpModDivideEdge (int edge, float prop, INode *pNode=NULL) { }
	virtual void EpModDivideFace (int face, Tab<float> *bary, INode *pNode=NULL) { }
	virtual void EpModWeldVerts (int v1, int v2, INode *pNode=NULL) { }
	virtual void EpModWeldEdges (int e1, int e2, INode *pNode=NULL) { }

	virtual void EpModAttach (INode *node, INode *pNode=NULL, TimeValue t=0) { }
	virtual void EpModMultiAttach (Tab<INode *> & nodeTab, INode *pNode=NULL, TimeValue t=0) { }

	virtual void EpModDetachToObject (TSTR & newObjectName, TimeValue t) { }
	virtual void EpModCreateShape (TSTR & shapeObjectName, TimeValue t) { }

	virtual void EpModSetHingeEdge (int edge, Matrix3 modContextTM, INode *pNode=NULL) { }
	virtual int EpModGetHingeEdge (INode *pNode) { return -1; }
	virtual void EpModBridgeBorders (int edge1, int edge2, INode *pNode=NULL) { }
	virtual void EpModBridgePolygons (int face1, int face2, INode *pNode=NULL) { }
	virtual void EpModSetBridgeNode (INode *pNode) { }
	virtual INode *EpModGetBridgeNode () { return NULL; }
	virtual bool EpModReadyToBridgeSelected () { return false; }

	//virtual void EpModTurnEdge (int edge, INode *pNode=NULL) { }
	virtual void EpModTurnDiagonal (int face, int diag, INode *pNode=NULL) { }

	virtual void EpModListOperations (INode *pNode=NULL) { }

	virtual MNMesh *EpModGetMesh (INode *pNode=NULL) { return NULL; }
	virtual MNMesh *EpModGetOutputMesh (INode *pNode=NULL) { return NULL; }

	// Access to information about each mesh.
	// (This is always based on the mesh _before_ any current operation.)
	virtual int EpMeshGetNumVertices (INode *pNode=NULL) { return 0; }
	virtual Point3 EpMeshGetVertex (int vertIndex, INode *pNode=NULL) { return Point3(0,0,0); }
	virtual int EpMeshGetVertexFaceCount (int vertIndex, INode *pNode=NULL) { return 0; }
	virtual int EpMeshGetVertexFace (int vertIndex, int whichFace, INode *pNode=NULL) { return 0; }
	virtual int EpMeshGetVertexEdgeCount (int vertIndex, INode *pNode=NULL) { return 0; }
	virtual int EpMeshGetVertexEdge (int vertIndex, int whichEdge, INode *pNode=NULL) { return 0; }

	virtual int EpMeshGetNumEdges (INode *pNode=NULL) { return 0; }
	virtual int EpMeshGetEdgeVertex (int edgeIndex, int end, INode *pNode=NULL) { return 0; }
	virtual int EpMeshGetEdgeFace (int edgeIndex, int side, INode *pNode=NULL) { return 0; }

	virtual int EpMeshGetNumFaces(INode *pNode=NULL) { return 0; }
	virtual int EpMeshGetFaceDegree (int faceIndex, INode *pNode=NULL) { return 0; }
	virtual int EpMeshGetFaceVertex (int faceIndex, int corner, INode *pNode=NULL) { return 0; }
	virtual int EpMeshGetFaceEdge (int faceIndex, int side, INode *pNode=NULL) { return 0; }
	virtual int EpMeshGetFaceDiagonal (int faceIndex, int diagonal, int end, INode *pNode=NULL) { return 0; }
	virtual int EpMeshGetFaceMaterial (int faceIndex, INode *pNode=NULL) { return 0; }
	virtual DWORD EpMeshGetFaceSmoothingGroup (int faceIndex, INode *pNode=NULL) { return 0; }

	virtual int EpMeshGetNumMapChannels (INode *pNode=NULL) { return 0; }
	virtual bool EpMeshGetMapChannelActive (int mapChannel, INode *pNode=NULL) { return false; }
	virtual int EpMeshGetNumMapVertices (int mapChannel, INode *pNode=NULL) { return 0; }
	virtual UVVert EpMeshGetMapVertex (int mapChannel, int vertIndex, INode *pNode=NULL) { return UVVert(0,0,0); }
	virtual int EpMeshGetMapFaceVertex (int mapChannel, int faceIndex, int corner, INode *pNode=NULL) { return 0; }
};

// Controls whether or not "inherently" nonanimatable operations are available in Animation mode.
#define EDIT_POLY_DISABLE_IN_ANIMATE

#endif
