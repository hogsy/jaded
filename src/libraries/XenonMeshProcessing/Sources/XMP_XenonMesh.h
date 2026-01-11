// ------------------------------------------------------------------------------------------------
// File   : XMP_XenonMesh.h
// Date   : 2005-05-31
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_XMP_XENONMESH_H
#define GUARD_XMP_XENONMESH_H

// ------------------------------------------------------------------------------------------------
// FUNCTIONS
// ------------------------------------------------------------------------------------------------
#if defined(ACTIVE_EDITORS) && defined(_XENON_RENDER)

void XMP_ProcessGameObject(WOR_tdst_World* _pst_World, OBJ_tdst_GameObject* _pst_GO);
void XMP_RevertGameObject(WOR_tdst_World* _pst_World, OBJ_tdst_GameObject* _pst_GO, BOOL _b_PromptUser = TRUE);
void XMP_CheckConsistency(WOR_tdst_World* _pst_World, OBJ_tdst_GameObject* _pst_GO);

void XMP_CreateEditableMesh(WOR_tdst_World* _pst_World, OBJ_tdst_GameObject* _pst_GO);
void XMP_RemoveEditableMesh(WOR_tdst_World* _pst_World, OBJ_tdst_GameObject* _pst_GO);

void XMP_FixGameObjectEdges(WOR_tdst_World* _pst_World, OBJ_tdst_GameObject* _pst_GO);

#endif // ACTIVE_EDITORS

#endif // #ifdef GUARD_XMP_XENONMESH_H
