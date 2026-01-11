// ------------------------------------------------------------------------------------------------
// File   : GEOXenonPack.h
// Date   : 2005-07-12
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_GEOXENONPACK_H
#define GUARD_GEOXENONPACK_H

#if defined(_XENON_RENDER)

// ------------------------------------------------------------------------------------------------
// DEFINITIONS
// ------------------------------------------------------------------------------------------------
#define XENON_MAX_WEIGHT 4

#define XENON_VB_OBJECT_VALIDATE_MAGIC 0xfedcba98

// ------------------------------------------------------------------------------------------------
// CLASSES
// ------------------------------------------------------------------------------------------------

// class used to reprensent a skin vertex
struct VertexSkinning
{
    // ----- class types -----
    // ----- attributes -----
    float m_afWeight[XENON_MAX_WEIGHT];
    BYTE  m_abyMatrixIndex[XENON_MAX_WEIGHT];

    // ----- methods -----
    VertexSkinning();
    void Add(int iMatrixIndex, float fWeight);
};

// class used to help generate Xenon friendly skinned mesh
class XePackSkinning
{
public:
    // ----- methods -----
    XePackSkinning(GEO_tdst_Object* poGeo);
    ~XePackSkinning();

    // ----- attributes -----
    int m_iVertexCount;
    VertexSkinning* m_poVertex;

    // Maximum nbr of non-zero weight in this mesh
    int m_iMaxNbrOfWeight;
    int m_iMaxMatrixIndex;
};

// ------------------------------------------------------------------------------------------------
// FUNCTIONS
// ------------------------------------------------------------------------------------------------

BOOL GEO_b_LoadGameObjectPack(OBJ_tdst_GameObject* _pst_GO);

void GEO_CleanGameObjectPack(OBJ_tdst_GameObject* _pst_GO);
void GEO_PackGameObject(OBJ_tdst_GameObject* _pst_GO, BOOL _b_Force = TRUE);
BOOL GEO_b_IsSkinned(OBJ_tdst_GameObject* _pst_GO, GEO_tdst_Object* _pst_Geo);

ULONG GEO_ul_PackDataLoadCallback(ULONG _ul_PosFile);

#if defined(ACTIVE_EDITORS)
void GEO_PackGameObjectEd(OBJ_tdst_GameObject* _pst_GO, BOOL _b_UpdateOnly = FALSE);
void GEO_PackGameObjectLOD(OBJ_tdst_GameObject* _pst_GO, UCHAR _uc_Level);

void GEO_PackGameObjectArray(OBJ_tdst_GameObject** _ppst_Objects, ULONG _ul_Count);
#endif

#endif // #if defined(_XENON_RENDER)

#endif // #ifdef GUARD_GEOXENONPACK_H
