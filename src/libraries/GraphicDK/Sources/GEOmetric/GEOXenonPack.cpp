// ------------------------------------------------------------------------------------------------
// File   : GEOXenonPack.cpp
// Date   : 2005-07-12
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "Precomp.h"

#if defined(_XENON_RENDER)

#include <vector>

#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "SDK/Sources/BASe/BAScrc.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "TABles/TABles.h"
#include "GRObject/GROsave.h"
#include "MATerial/MATstruct.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJgrp.h"
#include "ENGine/Sources/OBJects/OBJgizmo.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/WORld/WORsecto.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/WAYs/WAYload.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "ENGine/Sources/WAYs/WAYinit.h"
#include "AIinterp/Sources/AIload.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/Events/EVEload.h"
#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKstruct.h"
#include "EDIpaths.h"
#include "ENGine/Sources/GRP/GRPload.h"
#include "ENGine/Sources/COLlision/COLload.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/ANImation/ANIplay.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/ACTions/ACTload.h"
#include "ENGine/Sources/ACTions/ACTinit.h"
#include "ENGine/Sources/ACTions/ACTcompute.h"
#include "ENGine/Sources/GRId/GRI_load.h"
#include "ENGine/Sources/GRId/GRI_compute.h"
#include "GraphicDK/Sources/SELection/SELection.h"
#include "GraphicDK/Sources/GDInterface/GDInterface.h"
#include "SouND/Sources/SND.h"
#include "BASe/BASarray.h"
#include "Engine/Sources/Wind/CurrentWind.h"

#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEN.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_XMEN.h"

#include "GraphicDK/Sources/GEOmetric/GEOobjectcomputing.h"
#include "GraphicDK/Sources/GEOmetric/GEO_SKIN.h"

#include "GEOXenonPack.h"

#include "XenonGraphics/XeMesh.h"
#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeVertexShaderManager.h"
#include "XenonGraphics/XeGDInterface.h"
#include "XenonGraphics/XeWaterManager.h"
#include "XenonGraphics/XeDynVertexBuffer.h"
#include "XenonGraphics/XeUtils.h"

// ------------------------------------------------------------------------------------------------
// DEFINITIONS
// ------------------------------------------------------------------------------------------------

// Define to enable pack tracking
#if !defined(_FINAL_)
//#define _XE_PACK_TRACK
#endif

// Define to use SxT instead of the normal (default)
#define _XE_PACK_USE_SXT

// ------------------------------------------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------------------------------------------
const D3DXVECTOR3 XENON_D3DVEC3_ZERO(0.0f, 0.0f, 0.0f);

const ULONG C_ul_PackVersion    = 2;
const CHAR  C_sz_PackDataPath[] = "ROOT/X360Pack/";

// SC: TODO: Be careful about that, Xenon can use more formats than the editor and we should not
//           limit map pre-processing only to what the editor supports

#if defined(_XE_COMPRESS_NORMALS)
const ULONG C_ul_NormalSize   = sizeof(ULONG);
const ULONG C_ul_NormalFormat = XEVC_NORMAL_DEC3N;
#else
const ULONG C_ul_NormalSize   = 3 * sizeof(FLOAT);
const ULONG C_ul_NormalFormat = XEVC_NORMAL;
#endif

#if defined(_XE_COMPRESS_TANGENTS)
const ULONG C_ul_TangentSize   = 4 * sizeof(SHORT);
const ULONG C_ul_TangentFormat = XEVC_TANGENT_SHORT4N;
#else
const ULONG C_ul_TangentSize   = (4 * sizeof(FLOAT));
const ULONG C_ul_TangentFormat = XEVC_TANGENT;
#endif

#if defined(_XE_COMPRESS_WEIGHTS)
const ULONG C_ul_BlendSize   = 4 * (sizeof(SHORT) + sizeof(UCHAR));
const ULONG C_ul_BlendFormat = XEVC_BLENDWEIGHT4_SHORT4N | XEVC_BLENDINDICES;
#else
const ULONG C_ul_BlendSize   = 4 * (sizeof(FLOAT) + sizeof(UCHAR));
const ULONG C_ul_BlendFormat = XEVC_BLENDWEIGHT4 | XEVC_BLENDINDICES;
#endif

#if defined(_XE_COMPRESS_TEXCOORD0)
#pragma error(_XE_COMRESS_TEXCOORD0 not properly implemented - Should use FLOAT16_2)
const ULONG C_ul_TexCoord0Size   = 2 * sizeof(FLOAT);
const ULONG C_ul_TexCoord0Format = XEVC_TEXCOORD0;
#else
const ULONG C_ul_TexCoord0Size   = 2 * sizeof(FLOAT);
const ULONG C_ul_TexCoord0Format = XEVC_TEXCOORD0;
#endif

#if defined(ACTIVE_EDITORS)
#pragma pack(push, 1)
struct DumbVertex
{
    D3DXVECTOR3 position;
    ULONG       color;
#if defined(_XE_COMPRESS_TEXCOORD0)
    D3DXVECTOR2 texCoord0;
#else
    D3DXVECTOR2 texCoord0;
#endif
#if defined(_XE_COMPRESS_WEIGHTS)
    SHORT       weights[4];
#else
    FLOAT       weights[4];
#endif
    ULONG       indices;
#if defined(_XE_COMPRESS_NORMALS)
    ULONG       normal;
#else
    D3DXVECTOR3 normal;
#endif
#if defined(_XE_COMPRESS_TANGENTS)
    SHORT       tangent[4];
#else
    D3DXVECTOR4 tangent;
#endif
};
#pragma pack(pop)

struct TangentSpaceInfo
{
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vNormal;
    D3DXVECTOR3 vTangent;
    D3DXVECTOR3 vBinormal;
    D3DXVECTOR3 vSxT;
    FLOAT       fBinormalDirection;
};

typedef std::vector<TangentSpaceInfo> TangentSpaceInfoArray;

#endif

// ------------------------------------------------------------------------------------------------
// EXTERNAL VARIABLES
// ------------------------------------------------------------------------------------------------

#if defined(ACTIVE_EDITORS)
extern BOOL EDI_gb_ComputeMap;
extern BOOL EDI_gb_XeQuickLoad;
#endif

extern ULONG OBJ_ul_GameObjectRLICallback(ULONG _ul_FilePos);

// ------------------------------------------------------------------------------------------------
// PRIVATE CLASSES
// ------------------------------------------------------------------------------------------------

class XeMeshPacker
{
    enum
    {
#if defined(ACTIVE_EDITORS)
        MAX_ELEMENTS  = 8192,
#else
        MAX_ELEMENTS  = 256,
#endif
        INVALID_MATCH = 0x7fffffff,
        INVALID_INDEX = 0x7fffffff,
    };

    struct Vertex
    {
        M_DeclareOperatorNewAndDelete();

        ULONG ulPositionIndex;
        ULONG ulBlendingIndex;
        ULONG ulColor;
        ULONG ulTexCoordsIndex;
        ULONG ulLMCoordsIndex;

        ULONG ulOriginalIndex;
        ULONG ulNextMatch;
    };
    typedef std::vector<Vertex> VertexArray;

    struct Position
    {
        M_DeclareOperatorNewAndDelete();

        D3DXVECTOR3 vPosition;
        D3DXVECTOR3 vNormal;
        ULONG       ulNextMatchIndex;
        ULONG       ulUsed;

        D3DXVECTOR3 vS;
        FLOAT       fBinormalDirection;
        D3DXVECTOR3 vT;
        D3DXVECTOR3 vSxT;
    };
    typedef std::vector<Position> PositionArray;

    struct BlendingInfo
    {
        M_DeclareOperatorNewAndDelete();

        FLOAT afWeights[XENON_MAX_WEIGHT];
        BYTE  abyIndices[XENON_MAX_WEIGHT];
    };
    typedef std::vector<BlendingInfo> BlendingInfoArray;

    typedef std::vector<ULONG>       ColorArray;
    typedef std::vector<D3DXVECTOR2> TexCoordArray;

    struct Triangle
    {
        M_DeclareOperatorNewAndDelete();

        USHORT vertexIndices[3];
    };
    typedef std::vector<Triangle> TriangleArray;

    struct ElementInfo
    {
        M_DeclareOperatorNewAndDelete();

        TriangleArray ao_Triangles;
        FLOAT         f_SmoothThreshold;
    };

public:

    M_DeclareOperatorNewAndDelete();

    XeMeshPacker(void);
    ~XeMeshPacker(void);

    void SetPositionSource(GEO_Vertex* _pPositions, ULONG _ulNbPositions);
    void SetBlendingSource(VertexSkinning* _pSkinningInfo, ULONG _ulCount);
    void SetColorSource(ULONG* _pColors);
    void SetTexCoordSource(GEO_tdst_UV* _pTexCoords, ULONG _ulCount);
    void SetLMCoordSource(FLOAT** _ppLMCoords);

    void SetSmoothThreshold(ULONG _ulElementIndex, FLOAT _fThreshold);

    void AddTriangle(ULONG _ulVertexIndex1, ULONG _ulTexCoordIndex1,
                     ULONG _ulVertexIndex2, ULONG _ulTexCoordIndex2,
                     ULONG _ulVertexIndex3, ULONG _ulTexCoordIndex3,
                     ULONG _ulElementIndex);

    void BuildMeshes(OBJ_tdst_GameObject* _pst_GO, GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo, ULONG _ul_MaxWeight, ULONG _ul_MaxBones, BOOL _b_Tangent, BOOL _b_Skinned, BOOL _b_LightMaps, BOOL _b_Fur);

#if defined(ACTIVE_EDITORS)
    inline void DisableSaving(void) { m_bDisableSaving = TRUE; }
    inline void EnableEditionMode(void) { m_bEditionMode = TRUE; }

    void SetEditionModeVertexCount(ULONG _ul_NbVertices);

    void BeginObject(void);
    void EndObject(void);

    void ExtractTangentSpace(TangentSpaceInfoArray* _pao_TangentSpace);
#endif

private:

    ULONG GetVertexSize(ULONG* _pul_Components, BOOL _b_NormalMap, BOOL _b_Skinned, BOOL _b_LightMaps, BOOL _b_Fur);

    ULONG AddVertex(ULONG _ulVertexIndex, ULONG _ulTexCoordIndex, D3DXVECTOR3 _vTriangleNormal, ULONG _ulElementIndex);

    ULONG PushPosition(ULONG _ulVertexIndex);
    ULONG PushBlending(ULONG _ulIndex);
    ULONG PushTexCoord(ULONG _ulTexCoordIndex);

    void SnapPositions(void);

    void ComputeTangentSpace(void);

    ULONG AddPosition(ULONG _ulVertexIndex, const D3DXVECTOR3& _vNormal, ULONG _ulElementIndex);
    ULONG AddBlending(ULONG _ulIndex);
    ULONG AddTexCoord(ULONG _ulIndex);
    ULONG AddLMCoord(ULONG _ulIndex, ULONG _ulElementIndex);

#if defined(ACTIVE_EDITORS)
    void PrepareTangentSpace(GEO_tdst_Object* _pst_Geo, GRO_tdst_Visu* _pst_Visu );
#endif

    void PrepareFurData(GRO_tdst_Visu* _pst_Visu, BOOL _b_Skin, BOOL _b_Tangent);

    void WritePadding(void);

    void WriteUChars(void* _pcBuffer, UCHAR* _pucValue, ULONG _ulCount);
    void WriteUByte4(void* _pcBuffer, UCHAR* _pucValue);
    void WriteShorts(void* _pcBuffer, SHORT* _pwValue,  ULONG _ulCount);
    void WriteULongs(void* _pcBuffer, ULONG* _pulValue, ULONG _ulCount);
    void WriteFloats(void* _pcBuffer, FLOAT* _pfValue,  ULONG _ulCount);

    void WritePosition(ULONG _ulIndex, UCHAR** _ppcBuffer);
    void WriteNormal(ULONG _ulIndex, UCHAR** _ppcBuffer, BOOL _b_NormalMap);
    void WriteTangent(ULONG _ulIndex, UCHAR** _ppcBuffer);
    void WriteBlend(ULONG _ulIndex, UCHAR** _ppcBuffer);
    void WriteColor(ULONG _ulColor, UCHAR** _ppcBuffer);
    void WriteTexCoord(ULONG _ulIndex, UCHAR** _ppcBuffer);
    void WriteLMCoord(ULONG _ulIndex, UCHAR** _ppcBuffer);

#if defined(ACTIVE_EDITORS)
    void BeginSave(OBJ_tdst_GameObject* _pst_GO, GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo);
    void EndSave(GRO_tdst_Visu* _pst_Visu);
#endif

private:

    GEO_Vertex*     m_pPositionSrc;
    VertexSkinning* m_pSkinningSrc;
    ULONG*          m_pColorSrc;
    GEO_tdst_UV*    m_pTexCoordSrc;
    FLOAT**         m_pLMCoordSrc;

    ULONG* m_dulPositionRemapTable;
    ULONG* m_dulBlendingRemapTable;
    ULONG* m_dulTexCoordRemapTable;

    VertexArray       m_aoVertices;
    PositionArray     m_aoPositions;
    BlendingInfoArray m_aoBlendingInfo;
    TexCoordArray     m_aoTexCoords;
    TexCoordArray     m_aoLMCoords;

    ElementInfo m_aoElements[MAX_ELEMENTS];

#if defined(ACTIVE_EDITORS)
    BOOL  m_bDisableSaving;
    BOOL  m_bEditionMode;
    BOOL  m_bCanSave;
    ULONG m_ulSaveBufferSize;
    ULONG m_ulObjectStartPosition;
    ULONG m_ulObjectStartElement;
    ULONG m_ulMaxElement;
#endif
};

class XePackReader
{
public:

    M_DeclareOperatorNewAndDelete();

    XePackReader(CHAR* _pc_Buffer, ULONG _ul_Size);
    ~XePackReader(void);

    USHORT ReadUShort(void);
    ULONG  ReadULong(void);
    FLOAT  ReadFloat(void);
    void   ReadArray(void* _p_Dest, ULONG _ul_Size);
    void   ReadVector(MATH_tdst_Vector* _pst_Vector);

    void SkipArray(ULONG _ul_Size);

    void UpdateAlignment(void);

private:

    CHAR* m_pcBuffer;
    ULONG m_ulOffset;
    ULONG m_ulSize;
};

// ------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
// ------------------------------------------------------------------------------------------------

void GEO_AllocXenonElements(GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo);
void GEO_FreeXenonMesh(OBJ_tdst_GameObject* _pst_GO, GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo);
void GEO_PackXenonMesh(OBJ_tdst_GameObject* _pst_GO, GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo, ULONG* _pul_Colors = NULL);

#if defined(ACTIVE_EDITORS)
void GEO_PackXenonMeshEd(OBJ_tdst_GameObject* _pst_GO, GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo, BOOL _b_UpdateOnly);
#endif

void GEO_CleanGeoStaticLODObjectPack(OBJ_tdst_GameObject* _pst_GO, GEO_tdst_StaticLOD* _pst_LOD);
void GEO_CleanAnimatedObjectPack(OBJ_tdst_GameObject* _pst_GO, ANI_st_GameObjectAnim* _pst_Anim);

BOOL GEO_b_IsNormalMapped(GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo);
BOOL GEO_b_IsLightMapped(GRO_tdst_Visu* _pst_Visu);
BOOL GEO_b_IsFurPresent(OBJ_tdst_GameObject* _pst_GO, GRO_tdst_Visu* _pst_Visu);

void GEO_ComputeElementAABB(GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo);

ULONG* GEO_pul_GetColors(GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo, ULONG* _pul_Colors = NULL);

MAT_tdst_MultiTexture* GEO_pst_GetMultiTexture(MAT_tdst_Material* _pst_Material, LONG _l_MaterialId);

BOOL GEO_b_CanPackObject(OBJ_tdst_GameObject* _pst_GO);
void GEO_PackGeoStaticLODObject(OBJ_tdst_GameObject* _pst_GO, GEO_tdst_StaticLOD* _pst_LOD);
void GEO_PackGeometricObject(OBJ_tdst_GameObject* _pst_GO, GEO_tdst_Object* _pst_Geo);

BOOL GEO_b_LoadPackedGameObject(OBJ_tdst_GameObject* _pst_GO, BOOL _b_SkipCRC);

BOOL GEO_b_LoadOneObject(GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo, CHAR* _pc_Buffer, ULONG _ul_Length, BOOL _b_SkipCRC);

ULONG GEO_ul_ComputeCRC(GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo);

#if defined(ACTIVE_EDITORS)
void GEO_ComputePackNameAndDir(BIG_INDEX _ulFileIndex, CHAR* _szPathName, CHAR* _szFileName);

D3DXVECTOR3 GEO_ReverseTransformVector(MATH_tdst_Matrix* _pst_Matrix, const D3DXVECTOR3* _pst_Vector);

TangentSpaceInfo* GEO_pst_GetBestTangentSpaceMatch(const D3DXVECTOR3* _pst_Pos, 
                                                   const D3DXVECTOR3* _pst_Normal);
#endif

// ------------------------------------------------------------------------------------------------
// PRIVATE VARIABLES
// ------------------------------------------------------------------------------------------------
#if defined(ACTIVE_EDITORS)
static TangentSpaceInfoArray* s_paoGlobalTangentSpace = NULL;
static MATH_tdst_Matrix       s_stObjectGlobalMatrix;
static MATH_tdst_Matrix       s_stObjectInvNormalTransform;
#endif

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

inline FLOAT SnapFloat(FLOAT _fValue)
{
    static FLOAT s_FloatThreshold = 4096.0f;

    BOOL  bSign    = (_fValue < 0.0f);
    LONG  lInteger = (ULONG)fmodf(fabsf(_fValue), 1.0f);
    LONG  lFrac    = (ULONG)((fabsf(_fValue) - (FLOAT)lInteger) * s_FloatThreshold);
    FLOAT fNewVal  = (FLOAT)lInteger;

    fNewVal += (FLOAT)lFrac / s_FloatThreshold;
    if (bSign)
        fNewVal = -fNewVal;

    return fNewVal;
}

inline bool IsPositionEqual(FLOAT _x1, FLOAT _y1, FLOAT _z1,
                            FLOAT _x2, FLOAT _y2, FLOAT _z2, FLOAT _epsilon)
{
    if (fabsf(_x1 - _x2) > _epsilon)
        return false;

    if (fabsf(_y1 - _y2) > _epsilon)
        return false;

    if (fabsf(_z1 - _z2) > _epsilon)
        return false;

    return true;
}

inline bool IsPositionEqual(const D3DXVECTOR3& _p1,
                            const D3DXVECTOR3& _p2,
                            FLOAT              _epsilon)
{
    return IsPositionEqual(_p1.x, _p1.y, _p1.z, _p2.x, _p2.y, _p2.z, _epsilon);
}

BOOL GEO_b_CanPackObject(OBJ_tdst_GameObject* _pst_GO)
{
    if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ExtendedObject))
    {
        // Parse the modifier to see if there is something preventing us from packing
        if (_pst_GO->pst_Extended->pst_Modifiers != NULL)
        {
            MDF_tdst_Modifier* pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;

            while (pst_Modifier != NULL)
            {
                // No packing for morphed objects
                if((pst_Modifier->i->ul_Type == MDF_C_Modifier_Morphing) &&
                   !(pst_Modifier->ul_Flags & MDF_C_Modifier_Inactive))
                {
                    return FALSE;
                }

                // No packing for RLI Carte
                if (pst_Modifier->i->ul_Type == MDF_C_Modifier_RLICarte)
                {
                    return FALSE;
                }

                pst_Modifier = pst_Modifier->pst_Next;
            }
        }
    }

    return TRUE;
}

BOOL GEO_b_HasInformationToPack(GEO_tdst_Object* _pst_Geo)
{
    if (_pst_Geo->l_NbPoints <= 0)
        return FALSE;

    if (_pst_Geo->l_NbElements <= 0)
        return FALSE;

    LONG l_NbTriangles = 0;

    for (LONG i = 0; i < _pst_Geo->l_NbElements; ++i)
    {
        l_NbTriangles += _pst_Geo->dst_Element[i].l_NbTriangles;
    }

    if (l_NbTriangles <= 0)
        return FALSE;

    return TRUE;
}

void GEO_PackGeometricObject(OBJ_tdst_GameObject* _pst_GO, GEO_tdst_Object* _pst_Geo)
{
    if (!GEO_b_HasInformationToPack(_pst_Geo))
        return;

    GEO_PackXenonMesh(_pst_GO, _pst_GO->pst_Base->pst_Visu, _pst_Geo);
}

void GEO_PackGeoStaticLODObject(OBJ_tdst_GameObject* _pst_GO, GEO_tdst_StaticLOD* _pst_LOD)
{
    // Empty
    if (_pst_LOD->uc_NbLOD == 0)
        return;

    // We are always only using the first level on Xenon
    if ((_pst_LOD->dpst_Id[0] == NULL) ||
        (_pst_LOD->dpst_Id[0]->i->ul_Type != GRO_Geometric))
        return;

    GEO_PackGeometricObject(_pst_GO, (GEO_tdst_Object*)_pst_LOD->dpst_Id[0]);
}

void GEO_PackGameObject(OBJ_tdst_GameObject* _pst_GO, BOOL _b_Force)
{
    if (!GDI_b_IsXenonGraphics())
        return;

    if (!GEO_b_CanPackObject(_pst_GO))
        return;

#if defined(ACTIVE_EDITORS)
    if (EDI_gb_XeQuickLoad)
    {
        // Quick loading the map for tests, use the editor packing since it is way faster
        GEO_PackGameObjectEd(_pst_GO);
        return;
    }
#endif

    // Load the existing packing if possible
    if (!_b_Force)
    {
        if (GEO_b_LoadPackedGameObject(_pst_GO, FALSE))
            return;

        if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
        {
            _pst_GO->pst_Base->pst_Visu->ul_PackedDataKey = BIG_C_InvalidKey;
        }
    }

    if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        GEO_CleanGameObjectPack(_pst_GO);

        GRO_tdst_Struct* pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Object;
        if (pst_Gro == NULL)
            return;

        switch (pst_Gro->i->ul_Type)
        {
            case GRO_Geometric:
                GEO_PackGeometricObject(_pst_GO, (GEO_tdst_Object*)pst_Gro);
                break;

            case GRO_GeoStaticLOD:
                GEO_PackGeoStaticLODObject(_pst_GO, (GEO_tdst_StaticLOD*)pst_Gro);
                break;
        }
    }
}

#if defined(ACTIVE_EDITORS)

void GEO_PackGameObjectEd(OBJ_tdst_GameObject* _pst_GO, BOOL _b_UpdateOnly)
{
    if (!GDI_b_IsXenonGraphics())
        return;

    if (!GEO_b_CanPackObject(_pst_GO))
    {
        if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ExtendedObject))
        {
            // Parse the modifier to see if there is something preventing us from packing
            if (_pst_GO->pst_Extended->pst_Modifiers != NULL)
            {
                MDF_tdst_Modifier* pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;

                while (pst_Modifier != NULL)
                {
                    // No packing for morphed objects
                    if((pst_Modifier->i->ul_Type == MDF_C_Modifier_Morphing) &&
                        !(pst_Modifier->ul_Flags & MDF_C_Modifier_Inactive))
                    {

                        GEO_tdst_Object * pst_Object = NULL;

                        if( _pst_GO->pst_Base && 
                            _pst_GO->pst_Base->pst_Visu &&
                            _pst_GO->pst_Base->pst_Visu->pst_Object )
                        {
                            // Recreate the morphed mesh
                            pst_Object = (GEO_tdst_Object*) _pst_GO->pst_Base->pst_Visu->pst_Object;
                            GEO_ClearXenonMesh( _pst_GO->pst_Base->pst_Visu, pst_Object, FALSE, TRUE );

                            pst_Object->b_CreateMorphedMesh = TRUE;
                            GEO_CreateMorphedXenonMesh( _pst_GO->pst_Base->pst_Visu, pst_Object );
                            return;
                        }
                    }
                    pst_Modifier = pst_Modifier->pst_Next;
                }
            }
        }
        return;
    }

    if (!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
        return;

    GRO_tdst_Visu* pst_Visu = _pst_GO->pst_Base->pst_Visu;

    if (pst_Visu->pst_Object == NULL)
        return;

    GEO_tdst_Object* pst_Geo = NULL;

    if (pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
    {
        GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Visu->pst_Object;

        if ((pst_LOD->uc_NbLOD > 0) && (pst_LOD->dpst_Id[0] != NULL) &&
            (pst_LOD->dpst_Id[0]->i->ul_Type == GRO_Geometric))
        {
            pst_Geo = (GEO_tdst_Object*)pst_LOD->dpst_Id[0];
        }
    }
    else if (pst_Visu->pst_Object->i->ul_Type == GRO_Geometric)
    {
        pst_Geo = (GEO_tdst_Object*)pst_Visu->pst_Object;
    }

    if (pst_Geo == NULL)
        return;

    GEO_PackXenonMeshEd(_pst_GO, pst_Visu, pst_Geo, _b_UpdateOnly);
}

void GEO_PackGameObjectLOD(OBJ_tdst_GameObject* _pst_GO, UCHAR _uc_Level)
{
    if (_uc_Level == 0)
    {
        GEO_PackGameObject(_pst_GO);
        return;
    }

    if (!GDI_b_IsXenonGraphics())
        return;

    if (!GEO_b_CanPackObject(_pst_GO))
        return;

    if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        GEO_CleanGameObjectPack(_pst_GO);

        GRO_tdst_Struct* pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Object;
        if (pst_Gro == NULL)
            return;

        if (pst_Gro->i->ul_Type == GRO_GeoStaticLOD)
        {
            GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Gro;

            if (_uc_Level >= pst_LOD->uc_NbLOD)
            {
                GEO_PackGameObject(_pst_GO);
                return;
            }

            if (strstr(pst_LOD->st_Id.sz_Name, "_XMP_LOD") == NULL)
            {
                GEO_PackGameObject(_pst_GO);
                return;
            }

            ULONG* dul_NewColors = NULL;
            ULONG  ul_RLIKey     = BIG_C_InvalidKey;
            ULONG  ul_RLIIndex   = BIG_C_InvalidIndex;

            // Find the vertex colors that must be used with the selected LOD
            if (_pst_GO->pst_Base->pst_Visu->dul_VertexColors != NULL)
            {
                CHAR  sz_Temp[512];

                ul_RLIKey = LOA_ul_SearchKeyWithAddress((ULONG)_pst_GO->pst_Base->pst_Visu->dul_VertexColors);
                if (ul_RLIKey != BIG_C_InvalidKey)
                {
                    ul_RLIIndex = BIG_ul_SearchKeyToFat(ul_RLIKey);
                }

                for (ULONG i = 0; i < (ULONG)_uc_Level; ++i)
                {
                    if (pst_LOD->dpst_Id[i])
                    {
                        if ((strstr(pst_LOD->dpst_Id[i]->sz_Name, "_XEM") - pst_LOD->dpst_Id[i]->sz_Name) == strlen(pst_LOD->dpst_Id[i]->sz_Name) - strlen("_XEM"))
                        {
                            // Editable mesh - Previous key is stored in the name of the RLI

                            if (ul_RLIIndex != BIG_C_InvalidKey)
                            {
                                strcpy(sz_Temp, BIG_NameFile(ul_RLIIndex));
                                CHAR* psz_Search = strstr(sz_Temp, "_0x");
                                if (psz_Search != NULL)
                                {
                                    psz_Search += 3;

                                    if (sscanf(psz_Search, "%x", &ul_RLIKey) != 1)
                                    {
                                        ul_RLIKey   = BIG_C_InvalidKey;
                                        ul_RLIIndex = BIG_C_InvalidIndex;
                                    }
                                    else
                                    {
                                        ul_RLIIndex = BIG_ul_SearchKeyToFat(ul_RLIKey);
                                    }
                                }
                            }
                        }
                        else if ((strstr(pst_LOD->dpst_Id[i]->sz_Name, "_XMP")         - pst_LOD->dpst_Id[i]->sz_Name) == (strlen(pst_LOD->dpst_Id[i]->sz_Name) - strlen("_XMP")) ||
                                 (strstr(pst_LOD->dpst_Id[i]->sz_Name, "_XMP_LOD_XMP") - pst_LOD->dpst_Id[i]->sz_Name) == (strlen(pst_LOD->dpst_Id[i]->sz_Name) - strlen("_XMP_LOD_XMP")))
                        {
                            // Xenonified Mesh - Previous key is stored in the visu

                            ul_RLIKey   = _pst_GO->pst_Base->pst_Visu->ul_XenonMeshProcessingRLIKey;
                            ul_RLIIndex = BIG_ul_SearchKeyToFat(ul_RLIKey);
                        }
                        else
                        {
                            // Unknown...

                            ul_RLIKey   = BIG_C_InvalidKey;
                            ul_RLIIndex = BIG_C_InvalidIndex;
                        }
                    }
                }
            }

            if (ul_RLIIndex != BIG_C_InvalidIndex)
            {
                dul_NewColors = (ULONG*)OBJ_ul_GameObjectRLICallback(BIG_PosFile(ul_RLIIndex));
            }

            if ((pst_LOD->dpst_Id[0]->i->ul_Type         == GRO_Geometric) && 
                (pst_LOD->dpst_Id[_uc_Level]->i->ul_Type == GRO_Geometric))
            {
                if (((GEO_tdst_Object*)pst_LOD->dpst_Id[0])->l_NbElements == ((GEO_tdst_Object*)pst_LOD->dpst_Id[_uc_Level])->l_NbElements)
                {
                    GEO_PackXenonMesh(_pst_GO, _pst_GO->pst_Base->pst_Visu, (GEO_tdst_Object*)pst_LOD->dpst_Id[_uc_Level], dul_NewColors);

                    // Trick the renderer into thinking we packed the same object
                    _pst_GO->pst_Base->pst_Visu->ul_VBObjectValidate = (ULONG)pst_LOD->dpst_Id[0];
                }
            }

            if (dul_NewColors)
            {
                MEM_Free(dul_NewColors);
            }
        }
    }
}
#endif

BOOL GEO_b_IsNormalMapped(GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo)
{
    if (_pst_Geo->dst_UV == NULL)
        return FALSE;

    if (_pst_Geo->dst_PointNormal == NULL)
        return FALSE;

#if defined(ACTIVE_EDITORS)
    BOOL bFoundOne = FALSE;

    // Parse the materials to find out if we must create the tangent space
    for (LONG i = 0; i < _pst_Geo->l_NbElements; ++i)
    {
        GEO_tdst_ElementIndexedTriangles* pst_Elem = &_pst_Geo->dst_Element[i];

        if (pst_Elem->l_NbTriangles <= 0)
            continue;

        MAT_tdst_MultiTexture* pst_Multi = GEO_pst_GetMultiTexture((MAT_tdst_Material*)_pst_Visu->pst_Material, pst_Elem->l_MaterialId);
        if (pst_Multi != NULL)
        {
            MAT_tdst_MTLevel* pst_Level = pst_Multi->pst_FirstLevel;

            while (pst_Level != NULL)
            {
                if (pst_Level->pst_XeLevel != NULL)
                {
                    if ((pst_Level->pst_XeLevel->l_NormalMapId  != MAT_Xe_InvalidTextureId) ||
                        (pst_Level->pst_XeLevel->l_DetailNMapId != MAT_Xe_InvalidTextureId))
                    {
                        bFoundOne = TRUE;
                        break;
                    }
                }

                pst_Level = pst_Level->pst_NextLevel;
            }
        }

        if (bFoundOne)
            break;
    }

    return bFoundOne;

#else

    return TRUE;

#endif
}

BOOL GEO_b_IsLightMapped(GRO_tdst_Visu* _pst_Visu)
{
    return (_pst_Visu->pp_st_LightmapCoords != NULL);
}

BOOL GEO_b_IsFurPresent(OBJ_tdst_GameObject* _pst_GO, GRO_tdst_Visu* _pst_Visu)
{
    // Sharing the same texture coordinates, lightmaps have priority
    if (GEO_b_IsLightMapped(_pst_Visu))
        return FALSE;

    if (!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ExtendedObject))
        return FALSE;

    MDF_tdst_Modifier* pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
    while (pst_Modifier != NULL)
    {
        if (pst_Modifier->i->ul_Type == MDF_C_Modifier_DYNFUR)
        {
            return TRUE;
        }

        pst_Modifier = pst_Modifier->pst_Next;
    }

    return FALSE;
}

BOOL GEO_b_IsSkinned(OBJ_tdst_GameObject* _pst_GO, GEO_tdst_Object* _pst_Geo)
{
    if (!GEO_SKN_IsSkinned(_pst_Geo))
        return FALSE;

    for (ULONG ul_MatrixCounter = 0; ul_MatrixCounter < _pst_Geo->p_SKN_Objectponderation->NumberPdrtLists; ++ul_MatrixCounter)
    {
        if(!MATH_b_EqMatrixWithEpsilon(&MATH_gst_IdentityMatrix, &_pst_Geo->p_SKN_Objectponderation->pp_PdrtLst[ul_MatrixCounter]->st_FlashedMatrix, Cf_EpsilonBig))
        {
            return TRUE;
        }
    }

    // All matrices are set to identity, if there is a SPG2 modifier, the skinning information is useless
    if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ExtendedObject))
    {
        MDF_tdst_Modifier* pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
        while (pst_Modifier != NULL)
        {
            if (pst_Modifier->i->ul_Type == MDF_C_Modifier_SpriteMapper2)
            {
                return FALSE;
            }

            pst_Modifier = pst_Modifier->pst_Next;
        }
    }

    return TRUE;
}

ULONG* GEO_pul_GetColors(GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo, ULONG* _pul_Colors)
{
#if !defined(_FINAL_)
    static ULONG s_aulBadColors[SOFT_Cul_ComputingBufferSize];
    static BOOL  s_bColorInitialized = FALSE;

    if (!s_bColorInitialized)
    {
        const ULONG C_aul_ColorCycle[6] = { 0x00ff00ff, 0x00ff00ff, 
                                            0x00ff00ff, 0x00ff00ff,
                                            0x00ff00ff, 0x00ff00ff };
        for (ULONG i = 0; i < SOFT_Cul_ComputingBufferSize; ++i)
        {
            s_aulBadColors[i] = C_aul_ColorCycle[i % 6];
        }

        s_bColorInitialized = TRUE;
    }
#endif

#if defined(ACTIVE_EDITORS)
    if (_pul_Colors != NULL)
    {
        if (*_pul_Colors >= (ULONG)_pst_Geo->l_NbPoints)
            return _pul_Colors + 1;
    }
#endif

    if (_pst_Visu->dul_VertexColors != NULL)
    {
#if !defined(_FINAL_)
        if (*_pst_Visu->dul_VertexColors < (ULONG)_pst_Geo->l_NbPoints)
            return s_aulBadColors;
#endif

        return _pst_Visu->dul_VertexColors + 1;
    }

    if (_pst_Geo->dul_PointColors != NULL)
    {
#if !defined(_FINAL_)
        if (*_pst_Geo->dul_PointColors < (ULONG)_pst_Geo->l_NbPoints)
            return s_aulBadColors;
#endif

        return _pst_Geo->dul_PointColors + 1;
    }

    return NULL;
}

void GEO_ComputeElementAABB(GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo)
{
    LONG i;
    LONG j;
    LONG k;

    for (i = 0; i < _pst_Geo->l_NbElements; ++i)
    {
        GEO_tdst_ElementIndexedTriangles* pst_Element = &_pst_Geo->dst_Element[i];

        MATH_InitVector(&_pst_Visu->p_XeElements[i].st_AABBMin,  1e30f,  1e30f,  1e30f);
        MATH_InitVector(&_pst_Visu->p_XeElements[i].st_AABBMax, -1e30f, -1e30f, -1e30f);

        GEO_tdst_IndexedTriangle* pst_Triangle = pst_Element->dst_Triangle;

        for (j = 0; j < pst_Element->l_NbTriangles; ++j, ++pst_Triangle)
        {
            for (k = 0; k < 3; ++k)
            {
                GEO_Vertex& vertex = _pst_Geo->dst_Point[pst_Triangle->auw_Index[k]];

                if (vertex.x < _pst_Visu->p_XeElements[i].st_AABBMin.x)
                    _pst_Visu->p_XeElements[i].st_AABBMin.x = vertex.x;
                if (vertex.x > _pst_Visu->p_XeElements[i].st_AABBMax.x)
                    _pst_Visu->p_XeElements[i].st_AABBMax.x = vertex.x;
                if (vertex.y < _pst_Visu->p_XeElements[i].st_AABBMin.y)
                    _pst_Visu->p_XeElements[i].st_AABBMin.y = vertex.y;
                if (vertex.y > _pst_Visu->p_XeElements[i].st_AABBMax.y)
                    _pst_Visu->p_XeElements[i].st_AABBMax.y = vertex.y;
                if (vertex.z < _pst_Visu->p_XeElements[i].st_AABBMin.z)
                    _pst_Visu->p_XeElements[i].st_AABBMin.z = vertex.z;
                if (vertex.z > _pst_Visu->p_XeElements[i].st_AABBMax.z)
                    _pst_Visu->p_XeElements[i].st_AABBMax.z = vertex.z;
            }
        }
    }
}

MAT_tdst_MultiTexture* GEO_pst_GetMultiTexture(MAT_tdst_Material* _pst_Material,
                                               LONG               _l_MaterialId)
{
    if (_pst_Material == NULL)
        return NULL;

    if (_pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti)
    {
        MAT_tdst_Multi* pst_Multi = (MAT_tdst_Multi*)_pst_Material;

        return (MAT_tdst_MultiTexture*)pst_Multi->dpst_SubMaterial[lMin(_l_MaterialId, pst_Multi->l_NumberOfSubMaterials - 1)];
    }
    else if (_pst_Material->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
    {
        return (MAT_tdst_MultiTexture*)_pst_Material;
    }

    return NULL;
}

void GEO_AllocXenonElements(GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo)
{
    _pst_Visu->l_NbXeElements = _pst_Geo->l_NbElements;
    _pst_Visu->p_XeElements   = (GRO_tdst_XeElement*)MEM_p_Alloc(_pst_Visu->l_NbXeElements * sizeof(GRO_tdst_XeElement));
    L_memset(_pst_Visu->p_XeElements, 0, _pst_Visu->l_NbXeElements * sizeof(GRO_tdst_XeElement));
}

void GEO_CleanGameObjectPack(OBJ_tdst_GameObject* _pst_GO)
{
    if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        GRO_tdst_Struct* pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Object;
        if (pst_Gro == NULL)
            return;

        switch (pst_Gro->i->ul_Type)
        {
            case GRO_Geometric:
                GEO_FreeXenonMesh(_pst_GO, _pst_GO->pst_Base->pst_Visu, (GEO_tdst_Object*)pst_Gro);
                break;

            case GRO_GeoStaticLOD:
                GEO_CleanGeoStaticLODObjectPack(_pst_GO, (GEO_tdst_StaticLOD*)pst_Gro);
                break;
        }
    }
    else if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
    {
        ANI_st_GameObjectAnim* pst_Anim = _pst_GO->pst_Base->pst_GameObjectAnim;
        if (pst_Anim == NULL)
            return;

        GEO_CleanAnimatedObjectPack(_pst_GO, pst_Anim);
    }
}

void GEO_CleanGeoStaticLODObjectPack(OBJ_tdst_GameObject* _pst_GO, GEO_tdst_StaticLOD* _pst_LOD)
{
    // Empty
    if (_pst_LOD->uc_NbLOD == 0)
        return;

    // We are always only using the first level on Xenon
    if ((_pst_LOD->dpst_Id[0] == NULL) ||
        (_pst_LOD->dpst_Id[0]->i->ul_Type != GRO_Geometric))
        return;

    GEO_FreeXenonMesh(_pst_GO, _pst_GO->pst_Base->pst_Visu, (GEO_tdst_Object*)_pst_LOD->dpst_Id[0]);
}

void GEO_CleanAnimatedObjectPack(OBJ_tdst_GameObject* _pst_GO, ANI_st_GameObjectAnim* _pst_Anim)
{
    if (_pst_Anim->pst_Skeleton == NULL)
        return;

    TAB_tdst_PFelem* pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(_pst_Anim->pst_Skeleton->pst_AllObjects);
    TAB_tdst_PFelem* pst_EndBone     = TAB_pst_PFtable_GetLastElem(_pst_Anim->pst_Skeleton->pst_AllObjects);

    for ( ; pst_CurrentBone <= pst_EndBone; ++pst_CurrentBone )
    {
        OBJ_tdst_GameObject* pst_BoneGO = (OBJ_tdst_GameObject*)pst_CurrentBone->p_Pointer;
        if (pst_BoneGO != NULL)
        {
            GEO_CleanGameObjectPack(pst_BoneGO);
        }
    }
}

void GEO_FreeXenonMesh(OBJ_tdst_GameObject* _pst_GO, GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo)
{
    LONG i;

    if (_pst_Visu->p_XeElements != NULL)
    {
        for (i = 0; i < _pst_Visu->l_NbXeElements; ++i)
        {
            if ( _pst_Visu->p_XeElements[i].pst_Mesh &&
                !_pst_Visu->p_XeElements[i].pst_Mesh->IsDynamic())
            {
                THREAD_SAFE_MESH_DELETE(_pst_Visu->p_XeElements[i].pst_Mesh);
            }
            _pst_Visu->p_XeElements[i].pst_Mesh = NULL;

            THREAD_SAFE_IB_RELEASE(_pst_Visu->p_XeElements[i].pst_IndexBuffer);

#if defined(ACTIVE_EDITORS)
            if (_pst_Visu->p_XeElements[i].puw_Indices)
            {
                MEM_Free(_pst_Visu->p_XeElements[i].puw_Indices);
                _pst_Visu->p_XeElements[i].puw_Indices = NULL;
            }
#endif
            if (_pst_Visu->p_XeElements[i].dst_TangentSpace != NULL)
            {
                MEM_Free(_pst_Visu->p_XeElements[i].dst_TangentSpace);
                _pst_Visu->p_XeElements[i].dst_TangentSpace = NULL;
            }
        }
    }
    _pst_Visu->l_NbXeElements = -1;

    THREAD_SAFE_VB_RELEASE(_pst_Visu->pVB_MorphStatic);
    THREAD_SAFE_VB_RELEASE(_pst_Visu->pVB_MorphDynamic);
    _pst_Geo->b_CreateMorphedMesh = FALSE;

    MEM_Free(_pst_Visu->p_XeElements);
    _pst_Visu->p_XeElements = NULL;

    THREAD_SAFE_VB_RELEASE(_pst_Visu->p_VBVertex);
    THREAD_SAFE_VB_RELEASE(_pst_Visu->p_VBFurOffsets);

    if (_pst_Visu->p_FurOffsetVertex != NULL)
    {
        MEM_Free(_pst_Visu->p_FurOffsetVertex);
        _pst_Visu->p_FurOffsetVertex = NULL;
    }

    _pst_Visu->ul_PackedDataKey = BIG_C_InvalidKey;
}

#if defined(ACTIVE_EDITORS)
void GEO_PackXenonMeshEd(OBJ_tdst_GameObject* _pst_GO, GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo, BOOL _b_UpdateOnly)
{
    if (!GDI_b_IsXenonGraphics())
        return;

    if ((_pst_GO == NULL) || (_pst_Visu == NULL) || (_pst_Geo == NULL))
        return;

    LONG l_NbVertices = 0;
    for (LONG i = 0; i < _pst_Geo->l_NbElements; ++i)
    {
        l_NbVertices += _pst_Geo->dst_Element[i].l_NbTriangles * 3;
    }

    if (l_NbVertices == 0)
        return;

    if ((l_NbVertices != _pst_Visu->l_VBVertexCount) || (_pst_Geo->l_NbElements != _pst_Visu->l_NbXeElements))
        _b_UpdateOnly = FALSE;

    // Clear the actual vertex and index buffers
    if (!_b_UpdateOnly)
        GEO_FreeXenonMesh(_pst_GO, _pst_Visu, _pst_Geo);

    // Nothing to do
    if (_pst_Geo->l_NbElements <= 0)
        return;

    if (!_b_UpdateOnly)
    {
        GEO_AllocXenonElements(_pst_Visu, _pst_Geo);

        // Compute the bounding boxes
        GEO_ComputeElementAABB(_pst_Visu, _pst_Geo);
    }

    BOOL bNormalMap = GEO_b_IsNormalMapped(_pst_Visu, _pst_Geo);
    BOOL bLightMap  = GEO_b_IsLightMapped(_pst_Visu);
    BOOL bFur       = GEO_b_IsFurPresent(_pst_GO, _pst_Visu);
    BOOL bSkinned   = GEO_b_IsSkinned(_pst_GO, _pst_Geo);

    XePackSkinning oSkinning(_pst_Geo);
    XeMeshPacker   oMeshPacker;

    if (!_b_UpdateOnly)
    {
        // NEVER SAVE THIS!
        oMeshPacker.DisableSaving();

        // Don't try anything clever with the data
        oMeshPacker.EnableEditionMode();
        oMeshPacker.SetEditionModeVertexCount((ULONG)l_NbVertices);

        oMeshPacker.SetPositionSource(_pst_Geo->dst_Point, (ULONG)_pst_Geo->l_NbPoints);
        oMeshPacker.SetBlendingSource(bSkinned ? oSkinning.m_poVertex : NULL, (ULONG)_pst_Geo->l_NbPoints);
        oMeshPacker.SetColorSource(GEO_pul_GetColors(_pst_Visu, _pst_Geo));
        oMeshPacker.SetTexCoordSource(_pst_Geo->dst_UV, (ULONG)_pst_Geo->l_NbUVs);
        oMeshPacker.SetLMCoordSource(bLightMap ? _pst_Visu->pp_st_LightmapCoords : NULL);

        for (LONG l_Element = 0; l_Element < _pst_Geo->l_NbElements; ++l_Element)
        {
            GEO_tdst_ElementIndexedTriangles* pst_Element = &_pst_Geo->dst_Element[l_Element];

            if (pst_Element->l_NbTriangles <= 0)
                continue;

            for (LONG l_Triangle = 0; l_Triangle < pst_Element->l_NbTriangles; ++l_Triangle)
            {
                GEO_tdst_IndexedTriangle* pst_Triangle = &pst_Element->dst_Triangle[l_Triangle];

                oMeshPacker.AddTriangle(pst_Triangle->auw_Index[0], pst_Triangle->auw_UV[0], 
                                        pst_Triangle->auw_Index[1], pst_Triangle->auw_UV[1],
                                        pst_Triangle->auw_Index[2], pst_Triangle->auw_UV[2],
                                        (ULONG)l_Element);
            }
        }

        if (bSkinned)
        {
            ++oSkinning.m_iMaxMatrixIndex;
        }

        // Build the D3D resources
        oMeshPacker.BuildMeshes(_pst_GO, _pst_Visu, _pst_Geo, oSkinning.m_iMaxNbrOfWeight, oSkinning.m_iMaxMatrixIndex,
                                bNormalMap, bSkinned, bLightMap, bFur);

        // Extra validation
        _pst_Visu->ul_VBObjectValidate = (ULONG)_pst_Geo;
    }
    else
    {
        DumbVertex* pVertices = (DumbVertex*)_pst_Visu->p_VBVertex->Lock(_pst_Visu->l_VBVertexCount, sizeof(DumbVertex));

        GEO_Vertex*  pst_Pos = _pst_Geo->dst_Point;
        GEO_tdst_UV* pst_UV  = _pst_Geo->dst_UV;
        ULONG* pul_Colors    = NULL;

        pul_Colors = GEO_pul_GetColors(_pst_Visu, _pst_Geo);

        // Only update the position, colors and tex coord
        for (LONG l_Element = 0; l_Element < _pst_Geo->l_NbElements; ++l_Element)
        {
            for (LONG l_Tri = 0; l_Tri < _pst_Geo->dst_Element[l_Element].l_NbTriangles; ++l_Tri)
            {
                GEO_tdst_IndexedTriangle* pst_Tri = &_pst_Geo->dst_Element[l_Element].dst_Triangle[l_Tri];

                for (LONG l = 0; l < 3; ++l)
                {
                    if (pst_Pos)
                    {
                        pVertices->position.x = pst_Pos[pst_Tri->auw_Index[l]].x;
                        pVertices->position.y = pst_Pos[pst_Tri->auw_Index[l]].y;
                        pVertices->position.z = pst_Pos[pst_Tri->auw_Index[l]].z;
                    }

                    if (pul_Colors)
                    {
                        pVertices->color = XeConvertColor(pul_Colors[pst_Tri->auw_Index[l]]);
                    }

                    if (pst_UV)
                    {
                        pVertices->texCoord0.x = pst_UV[pst_Tri->auw_UV[l]].fU;
                        pVertices->texCoord0.y = pst_UV[pst_Tri->auw_UV[l]].fV;
                    }

                    ++pVertices;
                }
            }
        }

        _pst_Visu->p_VBVertex->Unlock();
    }
}
#endif

void GEO_PackXenonMesh(OBJ_tdst_GameObject* _pst_GO, GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo, ULONG* _pul_Colors)
{
    if (!GDI_b_IsXenonGraphics())
        return;

    if ((_pst_GO == NULL) || (_pst_Visu == NULL) || (_pst_Geo == NULL))
        return;

#if defined(_XE_PACK_TRACK)
    if (_pst_Geo->l_NbElements > 0)
    {
        ERR_OutputDebugString("[Pack] Packing %s [0x%08x]\n", 
                            _pst_GO->sz_Name ? _pst_GO->sz_Name : "<Unknown>",
                            (ULONG)_pst_GO);
    }
#endif

    // Clear the actual vertex and index buffers
    GEO_FreeXenonMesh(_pst_GO, _pst_Visu, _pst_Geo);

    // Nothing to do
    if (_pst_Geo->l_NbElements <= 0)
        return;

    GEO_AllocXenonElements(_pst_Visu, _pst_Geo);

    // Compute the bounding boxes
    GEO_ComputeElementAABB(_pst_Visu, _pst_Geo);

    BOOL bNormalMap = GEO_b_IsNormalMapped(_pst_Visu, _pst_Geo);
    BOOL bLightMap  = GEO_b_IsLightMapped(_pst_Visu);
    BOOL bFur       = GEO_b_IsFurPresent(_pst_GO, _pst_Visu);
    BOOL bSkinned   = GEO_b_IsSkinned(_pst_GO, _pst_Geo);

#if defined(_XE_PACK_TRACK)
    LONG lOriVertexCount = 0;
#endif

    XePackSkinning oSkinning(_pst_Geo);
    XeMeshPacker   oMeshPacker;

#if defined(ACTIVE_EDITORS)
    if (_pul_Colors)
    {
        oMeshPacker.DisableSaving();
    }
#endif

    oMeshPacker.SetPositionSource(_pst_Geo->dst_Point, (ULONG)_pst_Geo->l_NbPoints);
    oMeshPacker.SetBlendingSource(bSkinned ? oSkinning.m_poVertex : NULL, (ULONG)_pst_Geo->l_NbPoints);
    oMeshPacker.SetColorSource(GEO_pul_GetColors(_pst_Visu, _pst_Geo, _pul_Colors));
    oMeshPacker.SetTexCoordSource(_pst_Geo->dst_UV, (ULONG)_pst_Geo->l_NbUVs);
    oMeshPacker.SetLMCoordSource(bLightMap ? _pst_Visu->pp_st_LightmapCoords : NULL);

    for (LONG l_Element = 0; l_Element < _pst_Geo->l_NbElements; ++l_Element)
    {
        GEO_tdst_ElementIndexedTriangles* pst_Element = &_pst_Geo->dst_Element[l_Element];

        if (pst_Element->l_NbTriangles <= 0)
            continue;

#if defined(_XE_PACK_TRACK)
        lOriVertexCount += 3 * pst_Element->l_NbTriangles;
#endif

        MAT_tdst_MultiTexture* pst_MT = GEO_pst_GetMultiTexture((MAT_tdst_Material*)_pst_Visu->pst_Material, pst_Element->l_MaterialId);
        if ((pst_MT != NULL) && (pst_MT->pst_FirstLevel != NULL) && (pst_MT->pst_FirstLevel->pst_XeLevel != NULL))
        {
            oMeshPacker.SetSmoothThreshold((ULONG)l_Element, pst_MT->pst_FirstLevel->pst_XeLevel->f_SmoothThreshold);
        }

        for (LONG l_Triangle = 0; l_Triangle < pst_Element->l_NbTriangles; ++l_Triangle)
        {
            GEO_tdst_IndexedTriangle* pst_Triangle = &pst_Element->dst_Triangle[l_Triangle];

            oMeshPacker.AddTriangle(pst_Triangle->auw_Index[0], pst_Triangle->auw_UV[0], 
                                    pst_Triangle->auw_Index[1], pst_Triangle->auw_UV[1],
                                    pst_Triangle->auw_Index[2], pst_Triangle->auw_UV[2],
                                    (ULONG)l_Element);
        }
    }

    if (bSkinned)
    {
        ++oSkinning.m_iMaxMatrixIndex;
    }

    // Build the D3D resources
    oMeshPacker.BuildMeshes(_pst_GO, _pst_Visu, _pst_Geo, oSkinning.m_iMaxNbrOfWeight, oSkinning.m_iMaxMatrixIndex,
                            bNormalMap, bSkinned, bLightMap, bFur);

    // Extra validation
    _pst_Visu->ul_VBObjectValidate = (ULONG)_pst_Geo;

#if defined(_XE_PACK_TRACK)
    ERR_OutputDebugString("[Pack] Object %s [0x%08x] went from %d to %d vertices\n", 
                          _pst_GO->sz_Name ? _pst_GO->sz_Name : "<Unknown>", (ULONG)_pst_GO,
                          lOriVertexCount, _pst_Visu->l_VBVertexCount);
#endif
}

GRO_tdst_Visu* GEO_pst_GetVisuFromKeyAddress(ULONG* _pul_KeyAddress)
{
    static ULONG ul_Delta = 0xffffffff;

    if (ul_Delta == 0xffffffff)
    {
        GRO_tdst_Visu stVisu;
        ul_Delta = (UCHAR*)&stVisu.ul_PackedDataKey - (UCHAR*)&stVisu;
    }

    return (GRO_tdst_Visu*)((UCHAR*)_pul_KeyAddress - ul_Delta);
}

ULONG GEO_ul_PackDataLoadCallback(ULONG _ul_PosFile)
{
    ULONG*         pul_Address = LOA_pul_GetCurrentAddress();
    GRO_tdst_Visu* pst_Visu    = GEO_pst_GetVisuFromKeyAddress(pul_Address);
    CHAR*  pc_Buffer;
    ULONG  ul_Size;

#if defined(_XE_PACK_TRACK)
    ERR_OutputDebugString("[Pack] GEO_ul_PackDataLoadCallback @ 0x%08x\n", _ul_PosFile);
#endif

    pc_Buffer = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);

    CHAR* pcBufferTemp = pc_Buffer;
    LOA_ReadCharArray(&pcBufferTemp, NULL, ul_Size);

    GEO_b_LoadOneObject(pst_Visu, NULL, pc_Buffer, ul_Size, TRUE);

    return LOA_ul_GetCurrentKey();
}

// This one is private and should parse the bones for animated objects
BOOL GEO_b_LoadPackedGameObject(OBJ_tdst_GameObject* _pst_GO, BOOL _b_SkipCRC)
{
    BOOL bSuccess = FALSE;

    if (!GEO_b_CanPackObject(_pst_GO))
        return FALSE;

    if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
    {
        ANI_st_GameObjectAnim* pst_Anim = _pst_GO->pst_Base->pst_GameObjectAnim;

        if ((pst_Anim == NULL) || (pst_Anim->pst_Skeleton == NULL))
            return FALSE;

        TAB_tdst_PFelem* pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Anim->pst_Skeleton->pst_AllObjects);
        TAB_tdst_PFelem* pst_EndBone     = TAB_pst_PFtable_GetLastElem(pst_Anim->pst_Skeleton->pst_AllObjects);

        for ( ; pst_CurrentBone <= pst_EndBone; ++pst_CurrentBone )
        {
            OBJ_tdst_GameObject* pst_BoneGO = (OBJ_tdst_GameObject*)pst_CurrentBone->p_Pointer;
            if (pst_BoneGO != NULL)
            {
                GEO_b_LoadPackedGameObject(pst_BoneGO, TRUE);
            }
        }
    }
    else if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        GRO_tdst_Visu*   pst_Visu = _pst_GO->pst_Base->pst_Visu;
        GEO_tdst_Object* pst_Geo  = NULL;

        if (pst_Visu->ul_VBObjectValidate == XENON_VB_OBJECT_VALIDATE_MAGIC)
        {
#if defined(_XE_PACK_TRACK)
            ERR_OutputDebugString("[Pack] Visu-Geo [0x%08x - 0x%08x] association for %s [0x%08x]\n", 
                (ULONG)pst_Visu, (ULONG)pst_Visu->pst_Object,
                _pst_GO->sz_Name ? _pst_GO->sz_Name : "<Unknown>",
                (ULONG)_pst_GO);
#endif

            // Object is already loaded and we just need to associate the visu with the current geo
            pst_Visu->ul_VBObjectValidate = (ULONG)pst_Visu->pst_Object;
            return TRUE;
        }

        if (pst_Visu->pst_Object == NULL)
            return FALSE;

        if (pst_Visu->pst_Object->i->ul_Type == GRO_Geometric)
        {
            pst_Geo = (GEO_tdst_Object*)pst_Visu->pst_Object;
        }
        else if (pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
        {
            GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Visu->pst_Object;

            if ((pst_LOD->dpst_Id[0] != NULL) &&
                (pst_LOD->dpst_Id[0]->i->ul_Type == GRO_Geometric))
            {
                pst_Geo = (GEO_tdst_Object*)pst_LOD->dpst_Id[0];
            }
        }

        if (pst_Geo == NULL)
            return FALSE;

        // Already set, use it
        if (pst_Visu->ul_VBObjectValidate == (ULONG)pst_Geo)
        {
            return TRUE;
        }

        // Load the object if possible
        if ((pst_Visu->ul_PackedDataKey != 0) &&
            (pst_Visu->ul_PackedDataKey != BIG_C_InvalidKey))
        {
            ULONG ulFilePos = BIG_ul_SearchKeyToPos(pst_Visu->ul_PackedDataKey);

#if defined(ACTIVE_EDITORS)
            // File was not found - There maybe be a valid file, but someone forgot to save the map... Let's find it!
            if (ulFilePos == BIG_C_InvalidKey)
            {
                BIG_INDEX ulGaoIndex = LOA_ul_SearchIndexWithAddress((ULONG)_pst_GO);
                if (ulGaoIndex != BIG_C_InvalidKey)
                {
                    CHAR szFileName[32];
                    CHAR szPath[1024];

                    GEO_ComputePackNameAndDir(ulGaoIndex, szPath, szFileName);

                    BIG_INDEX ulOldFileIndex = BIG_ul_SearchFileExt(szPath, szFileName);
                    if (ulOldFileIndex != BIG_C_InvalidIndex)
                    {
                        ulFilePos = BIG_PosFile(ulOldFileIndex);

                        pst_Visu->ul_PackedDataKey = BIG_FileKey(ulOldFileIndex);

#if defined(_XE_PACK_TRACK)
                        ERR_OutputDebugString("[Pack] Loading %s [0x%08x], packed data key was not matching, but data is present and will be used", 
                                              _pst_GO->sz_Name, (ULONG)_pst_GO);
#endif
                    }
                }
            }
#endif

            if (ulFilePos != BIG_C_InvalidKey)
            {
                ULONG ulLength;
                CHAR* pcBuffer = BIG_pc_ReadFileTmpMustFree(ulFilePos, &ulLength);

                if (pcBuffer != NULL)
                {
                    ULONG ulPackedDataKeyBK = pst_Visu->ul_PackedDataKey;

                    // Make sure we start on something clean
                    GEO_FreeXenonMesh(_pst_GO, pst_Visu, pst_Geo);

                    pst_Visu->ul_PackedDataKey = ulPackedDataKeyBK;

                    bSuccess = GEO_b_LoadOneObject(pst_Visu, pst_Geo, pcBuffer, ulLength, _b_SkipCRC);
#if defined(_XE_PACK_TRACK)
                    ERR_OutputDebugString("[Pack] Loading %s for %s [0x%08x]\n", 
                                          bSuccess ? "succeeded" : "failed", 
                                          _pst_GO->sz_Name ? _pst_GO->sz_Name : "<Unknown>",
                                          (ULONG)_pst_GO);
#endif

                    L_free(pcBuffer);
                }
            }
        }
    }

    return bSuccess;
}

BOOL GEO_b_LoadOneObject(GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo, CHAR* _pc_Buffer, ULONG _ul_Length, BOOL _b_SkipCRC)
{
    XePackReader oReader(_pc_Buffer, _ul_Length);
    ULONG i;

    ULONG ulVersion = oReader.ReadULong();

    // Only supporting version 2
    if (ulVersion != 2)
    {
#if defined(_XE_PACK_TRACK)
        ERR_OutputDebugString("[Pack] Version mismatch (%u)\n", ulVersion);
#endif
        return FALSE;
    }

    // CRC
    ULONG ulCRC = oReader.ReadULong();
    if (
        (_pst_Geo != NULL) 
        && !_b_SkipCRC
#if defined(ACTIVE_EDITORS)
        && !EDI_gb_XeQuickLoad
#endif
        )
    {
        ULONG ulCurCRC = GEO_ul_ComputeCRC(_pst_Visu, _pst_Geo);
        if (ulCRC != ulCurCRC)
        {
#if defined(_XE_PACK_TRACK)
            ERR_OutputDebugString("[Pack] CRC test failed\n");
#endif
            return FALSE;
        }
    }

    // Number of elements
    _pst_Visu->l_NbXeElements = (LONG)oReader.ReadULong();
    _pst_Visu->p_XeElements   = (GRO_tdst_XeElement*)MEM_p_Alloc(_pst_Visu->l_NbXeElements * sizeof(GRO_tdst_XeElement));
    L_memset(_pst_Visu->p_XeElements, 0, _pst_Visu->l_NbXeElements * sizeof(GRO_tdst_XeElement));

    // Vertex count, size and format
    _pst_Visu->l_VBVertexCount = (LONG)oReader.ReadULong();
    ULONG ulVertexFormat       = oReader.ReadULong();
    ULONG ulVertexStride       = oReader.ReadULong();
    ULONG ulMaxWeights         = oReader.ReadULong();
    ULONG ulMaxBones           = oReader.ReadULong();
    ULONG ulComponents         = ulVertexFormat;

#if defined(_XENON)
    // Create the vertex buffer
    _pst_Visu->p_VBVertex = (XeBuffer*)g_XeBufferMgr.CreateVertexBuffer(_pst_Visu->l_VBVertexCount, ulVertexStride, TRUE);
    UCHAR* pucBuffer      = (UCHAR*)_pst_Visu->p_VBVertex->Lock(_pst_Visu->l_VBVertexCount, ulVertexStride);

    // Load as-is
    oReader.ReadArray(pucBuffer, _pst_Visu->l_VBVertexCount * ulVertexStride);

    _pst_Visu->p_VBVertex->Unlock();
#else
    ULONG ulVertexStrideEd = ulVertexStride;

    // PC does not support DEC3N for normals, so make sure we are not using that format
    if ((ulVertexFormat & XEVC_NORMAL_DEC3N) == XEVC_NORMAL_DEC3N)
    {
        ulComponents     = (ulVertexFormat & ~XEVC_NORMAL_DEC3N) | XEVC_NORMAL;
        ulVertexStrideEd = ulVertexStrideEd + (3 * sizeof(FLOAT)) - sizeof(ULONG);
    }

    // Create the vertex buffer
    _pst_Visu->p_VBVertex = (XeBuffer*)g_XeBufferMgr.CreateVertexBuffer(_pst_Visu->l_VBVertexCount, ulVertexStrideEd, TRUE);
    UCHAR* pucBuffer      = (UCHAR*)_pst_Visu->p_VBVertex->Lock(_pst_Visu->l_VBVertexCount, ulVertexStrideEd);

    // Parse and load the vertex buffer
    for (i = 0; i < (ULONG)_pst_Visu->l_VBVertexCount; ++i)
    {
        // Position 3 float
        if (ulVertexFormat & XEVC_POSITION)
        {
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
        }

        // Color
        if (ulVertexFormat & XEVC_COLOR0)
        {
            *(ULONG*)pucBuffer = oReader.ReadULong(); pucBuffer += sizeof(ULONG);
        }

        // Texture coordinates
        if (ulVertexFormat & XEVC_TEXCOORD0)
        {
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
        }

        // Skinning
        if ((ulVertexFormat & XEVC_BLENDWEIGHT4_SHORT4N) == XEVC_BLENDWEIGHT4_SHORT4N)
        {
            // Weights
            *(USHORT*)pucBuffer = oReader.ReadUShort(); pucBuffer += sizeof(USHORT);
            *(USHORT*)pucBuffer = oReader.ReadUShort(); pucBuffer += sizeof(USHORT);
            *(USHORT*)pucBuffer = oReader.ReadUShort(); pucBuffer += sizeof(USHORT);
            *(USHORT*)pucBuffer = oReader.ReadUShort(); pucBuffer += sizeof(USHORT);

            // Indices
            *(ULONG*)pucBuffer = oReader.ReadULong(); pucBuffer += sizeof(ULONG);
        }
        else if (ulVertexFormat & XEVC_BLENDWEIGHT4)
        {
            // Weights
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);

            // Indices
            *(ULONG*)pucBuffer = oReader.ReadULong(); pucBuffer += sizeof(ULONG);
        }

        // Normal
        if ((ulVertexFormat & XEVC_NORMAL_DEC3N) == XEVC_NORMAL_DEC3N)
        {
            ULONG ulCompressedNormal = oReader.ReadULong();

            // SC: TODO: This function was never tested
            XeExpandDEC3N((FLOAT*)pucBuffer, ulCompressedNormal);
            pucBuffer += sizeof(ULONG);
        }
        else if (ulVertexFormat & XEVC_NORMAL)
        {
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
        }

        // Tangent
        if ((ulVertexFormat & XEVC_TANGENT_SHORT4N) == XEVC_TANGENT_SHORT4N)
        {
            *(USHORT*)pucBuffer = oReader.ReadUShort(); pucBuffer += sizeof(USHORT);
            *(USHORT*)pucBuffer = oReader.ReadUShort(); pucBuffer += sizeof(USHORT);
            *(USHORT*)pucBuffer = oReader.ReadUShort(); pucBuffer += sizeof(USHORT);
            *(USHORT*)pucBuffer = oReader.ReadUShort(); pucBuffer += sizeof(USHORT);
        }
        else if (ulVertexFormat & XEVC_TANGENT_SHORT4N)
        {
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
        }

        // Lightmap coordinates
        if (ulVertexFormat & XEVC_TEXCOORD1)
        {
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
            *(FLOAT*)pucBuffer = oReader.ReadFloat(); pucBuffer += sizeof(FLOAT);
        }
    }

    // Unlock the vertex buffer
    _pst_Visu->p_VBVertex->Unlock();
#endif

    oReader.UpdateAlignment();

    for (i = 0; i < (ULONG)_pst_Visu->l_NbXeElements; ++i)
    {
        // Index count
        ULONG ulNbIndices = oReader.ReadULong();

        if (ulNbIndices == 0)
            continue;

        // Element's bounding box
        oReader.ReadVector(&_pst_Visu->p_XeElements[i].st_AABBMin);
        oReader.ReadVector(&_pst_Visu->p_XeElements[i].st_AABBMax);

        // Create the mesh
        XeMesh* pst_Mesh                    = new XeMesh();
        _pst_Visu->p_XeElements[i].pst_Mesh = pst_Mesh;

        pst_Mesh->SetAxisAlignedBoundingVolume(_pst_Visu->p_XeElements[i].st_AABBMin, _pst_Visu->p_XeElements[i].st_AABBMax);

        pst_Mesh->AddStream(ulComponents, _pst_Visu->p_VBVertex);

        pst_Mesh->SetMaxWeights(ulMaxWeights);
        pst_Mesh->SetMaxBones(ulMaxBones);

        // Create the index buffer
        // Create the element's index buffers
        XeIndexBuffer* pst_IndexBuffer = (XeIndexBuffer*)g_XeBufferMgr.CreateIndexBuffer(ulNbIndices);
        _pst_Visu->p_XeElements[i].pst_IndexBuffer = pst_IndexBuffer;

#if defined(ACTIVE_EDITORS)
        USHORT* puw_IndicesEd = (USHORT*)MEM_p_Alloc(ulNbIndices * sizeof(USHORT));
        _pst_Visu->p_XeElements[i].puw_Indices = puw_IndicesEd;
#endif

        USHORT* puwIndices = (USHORT*)pst_IndexBuffer->Lock(ulNbIndices);

#if defined(_XENON)
        // Read the content as-is
        oReader.ReadArray(puwIndices, ulNbIndices * sizeof(USHORT));
#else
        // Read-swap the indices
        for (ULONG j = 0; j < ulNbIndices; ++j)
        {
            USHORT uwIndex   = oReader.ReadUShort();
            *puwIndices++    = uwIndex;
#if defined(ACTIVE_EDITORS)
            *puw_IndicesEd++ = uwIndex;
#endif
        }
#endif

        oReader.UpdateAlignment();

        pst_IndexBuffer->Unlock();

        // Attach the index buffer
        pst_Mesh->SetIndices(_pst_Visu->p_XeElements[i].pst_IndexBuffer);
    }

    // Fur information
    ULONG ulHasFur = oReader.ReadULong();
    if (ulHasFur != 0)
    {
        FurOffsetVertex* p_FurVertices = (FurOffsetVertex*)MEM_p_Alloc(_pst_Visu->l_VBVertexCount * sizeof(FurOffsetVertex));
        _pst_Visu->p_FurOffsetVertex   = p_FurVertices;

        L_memset(p_FurVertices, 0, _pst_Visu->l_VBVertexCount * sizeof(FurOffsetVertex));

        // Read the fur data
        for (ULONG j = 0; j < (ULONG)_pst_Visu->l_VBVertexCount; ++j, ++p_FurVertices)
        {
            p_FurVertices->fX = oReader.ReadFloat();
            p_FurVertices->fY = oReader.ReadFloat();
            p_FurVertices->fZ = oReader.ReadFloat();

            p_FurVertices->afWeight[0]       = oReader.ReadFloat();
            p_FurVertices->afWeight[1]       = oReader.ReadFloat();
            p_FurVertices->afWeight[2]       = oReader.ReadFloat();
            p_FurVertices->afWeight[3]       = oReader.ReadFloat();
            *(ULONG*)p_FurVertices->abyIndex = oReader.ReadULong();

            p_FurVertices->fTangentX = oReader.ReadFloat();
            p_FurVertices->fTangentY = oReader.ReadFloat();
            p_FurVertices->fTangentZ = oReader.ReadFloat();

            p_FurVertices->fBinormalX = oReader.ReadFloat();
            p_FurVertices->fBinormalY = oReader.ReadFloat();
            p_FurVertices->fBinormalZ = oReader.ReadFloat();
        }

        _pst_Visu->p_VBFurOffsets = (XeBuffer*)g_XeBufferMgr.CreateDynVertexBuffer();

        // Add to the streams
        for (i = 0; i < (ULONG)_pst_Visu->l_NbXeElements; ++i)
        {
            if (_pst_Visu->p_XeElements[i].pst_Mesh != NULL)
            {
                _pst_Visu->p_XeElements[i].pst_Mesh->AddStream(XEVC_TEXCOORD1, _pst_Visu->p_VBFurOffsets);
            }
        }
    }

    ULONG ulNbTangentPoints = oReader.ReadULong();
    if (ulNbTangentPoints != 0)
    {
#if defined(_XENON)
        oReader.SkipArray(ulNbTangentPoints * sizeof(GEO_tdst_TextureSpaceBasis));
#else
        // This array is for the editor so there is no need for endian swap
        GEO_tdst_TextureSpaceBasis* pst_TangentSpace = (GEO_tdst_TextureSpaceBasis*)MEM_p_Alloc(ulNbTangentPoints * sizeof(GEO_tdst_TextureSpaceBasis));
        oReader.ReadArray(pst_TangentSpace, ulNbTangentPoints * sizeof(GEO_tdst_TextureSpaceBasis));

        for (ULONG j = 0; j < (ULONG)_pst_Visu->l_NbXeElements; ++j)
        {
            _pst_Visu->p_XeElements[j].dst_TangentSpace = (GEO_tdst_TextureSpaceBasis*)MEM_p_Alloc(ulNbTangentPoints * sizeof(GEO_tdst_TextureSpaceBasis));
            L_memcpy(_pst_Visu->p_XeElements[j].dst_TangentSpace, pst_TangentSpace, ulNbTangentPoints * sizeof(GEO_tdst_TextureSpaceBasis));
        }

        MEM_Free(pst_TangentSpace);
#endif

        oReader.UpdateAlignment();
    }

    // Associate the geo to the visu (or mark for future association)
    if (_pst_Geo != NULL)
        _pst_Visu->ul_VBObjectValidate = (ULONG)_pst_Geo;
    else
        _pst_Visu->ul_VBObjectValidate = XENON_VB_OBJECT_VALIDATE_MAGIC;

    return TRUE;
}

ULONG GEO_ul_ComputeCRC(GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo)
{
    ULONG* pul_Colors = NULL;

    std::vector<ULONG> aulCRCVector;

    if (_pst_Visu->dul_VertexColors != NULL)
        pul_Colors = _pst_Visu->dul_VertexColors;
    else if (_pst_Geo->dul_PointColors != NULL)
        pul_Colors = _pst_Geo->dul_PointColors;

    // Add the vertex colors
    if (pul_Colors != NULL)
    {
        ULONG ulNbColors = *pul_Colors;

        if (ulNbColors > 0)
        {
            aulCRCVector.push_back(BAS_g_CRC32.ComputeCRC32LittleEndian(pul_Colors, ulNbColors + 1));
        }
    }

    // Add the number of points, uv, and elements
    aulCRCVector.push_back((ULONG)_pst_Geo->l_NbPoints);
    aulCRCVector.push_back((ULONG)_pst_Geo->l_NbElements);
    aulCRCVector.push_back((ULONG)_pst_Geo->l_NbUVs);

    // Add the points
    if ((_pst_Geo->l_NbPoints > 0) && (_pst_Geo->dst_Point != NULL))
    {
        aulCRCVector.push_back(BAS_g_CRC32.ComputeCRC32LittleEndian(_pst_Geo->dst_Point, _pst_Geo->l_NbPoints * sizeof(GEO_Vertex) / sizeof(ULONG)));
    }

    // Skinning information
    if (_pst_Geo->p_SKN_Objectponderation != NULL)
    {
#if defined(ACTIVE_EDITORS)
        GEO_SKN_Compress(_pst_Geo);
#endif

        aulCRCVector.push_back((ULONG)_pst_Geo->p_SKN_Objectponderation->NumberPdrtLists);

        // Weight lists
        for (ULONG ul_List = 0; ul_List < _pst_Geo->p_SKN_Objectponderation->NumberPdrtLists; ++ul_List)
        {
            aulCRCVector.push_back(BAS_g_CRC32.ComputeCRC32LittleEndian(_pst_Geo->p_SKN_Objectponderation->pp_PdrtLst[ul_List]->p_PdrtVrc_C, 
                                                                        _pst_Geo->p_SKN_Objectponderation->pp_PdrtLst[ul_List]->us_NumberOfPonderatedVertices * sizeof(GEO_tdst_CompressedVertexPonderation) / sizeof(ULONG)));
        }
    }

    // Add the texture coordinates
    if ((_pst_Geo->l_NbUVs > 0) && (_pst_Geo->dst_UV != NULL))
    {
        aulCRCVector.push_back(BAS_g_CRC32.ComputeCRC32LittleEndian(_pst_Geo->dst_UV, _pst_Geo->l_NbUVs * sizeof(GEO_tdst_UV) / sizeof(ULONG)));
    }

    for (LONG l_Element = 0; l_Element < _pst_Geo->l_NbElements; ++l_Element)
    {
        GEO_tdst_ElementIndexedTriangles* pst_Element = &_pst_Geo->dst_Element[l_Element];

        // Number of triangles and material ID
        aulCRCVector.push_back((ULONG)pst_Element->l_NbTriangles);
        aulCRCVector.push_back((ULONG)pst_Element->l_MaterialId);

        // Light map coordinates
        if ((pst_Element->l_NbTriangles) > 0 && (_pst_Visu->pp_st_LightmapCoords != NULL))
        {
            aulCRCVector.push_back(BAS_g_CRC32.ComputeCRC32LittleEndian(_pst_Visu->pp_st_LightmapCoords[l_Element], 
                                                                        3 * pst_Element->l_NbTriangles * sizeof(GEO_tdst_UV) / sizeof(ULONG)));
        }

        // Content of the element
        for (LONG l_Triangle = 0; l_Triangle < pst_Element->l_NbTriangles; ++l_Triangle)
        {
            GEO_tdst_IndexedTriangle* pst_Triangle = &pst_Element->dst_Triangle[l_Triangle];

            aulCRCVector.push_back((ULONG)pst_Triangle->auw_Index[0]);
            aulCRCVector.push_back((ULONG)pst_Triangle->auw_Index[1]);
            aulCRCVector.push_back((ULONG)pst_Triangle->auw_Index[2]);

            if (_pst_Geo->l_NbUVs > 0)
            {
                aulCRCVector.push_back((ULONG)pst_Triangle->auw_UV[0]);
                aulCRCVector.push_back((ULONG)pst_Triangle->auw_UV[1]);
                aulCRCVector.push_back((ULONG)pst_Triangle->auw_UV[2]);
            }
        }
    }

    return BAS_g_CRC32.ComputeCRC32LittleEndian(&aulCRCVector[0], aulCRCVector.size());
}

#if defined(ACTIVE_EDITORS)
void GEO_ComputePackNameAndDir(BIG_INDEX _ulFileIndex, CHAR* _szPathName, CHAR* _szFileName)
{
    CHAR szPathName[1024];
    CHAR szTempName[1024];

    BIG_ComputeFullName(BIG_ParentFile(_ulFileIndex), szPathName);
    strlwr(szPathName);

    sprintf(_szFileName, "0x%08x.xgo", BIG_FileKey(_ulFileIndex));

    // Remove the "ROOT/EngineDatas/" part
    const CHAR Csz_RootEngineDatas[] = "root/enginedatas/";
    if (strstr(szPathName, Csz_RootEngineDatas) == szPathName)
    {
        strcpy(szTempName, szPathName);
        strcpy(szPathName, szTempName + strlen(Csz_RootEngineDatas));
    }

    // Truncate "02 Modelisation Bank" to "02"
    const CHAR Csz_ModelisationBank[] = "02 modelisation bank";
    CHAR*      pst_Find;
    if ((pst_Find = strstr(szPathName, Csz_ModelisationBank)) != NULL)
    {
        strcpy(szTempName, szPathName);
        strcpy(pst_Find + 2, szTempName + (pst_Find - szPathName) + strlen(Csz_ModelisationBank));
    }

    // Truncate "06 Levels" to "06"
    const CHAR Csz_Levels[] = "06 levels";
    if ((pst_Find = strstr(szPathName, Csz_Levels)) != NULL)
    {
        strcpy(szTempName, szPathName);
        strcpy(pst_Find + 2, szTempName + (pst_Find - szPathName) + strlen(Csz_Levels));
    }

    // Remove the "/Game Objects" at the end
    const CHAR Csz_GameObject[] = "/game objects";
    if ((pst_Find = strstr(szPathName, Csz_GameObject)) != NULL)
    {
        *pst_Find = '\0';
    }

    strcpy(_szPathName, C_sz_PackDataPath);
    strcat(_szPathName, szPathName);
}
#endif

#if defined(ACTIVE_EDITORS)

void GEO_AddGEOToMeshPacker(XeMeshPacker* _pst_Packer, OBJ_tdst_GameObject* _pst_GO, GEO_tdst_Object* _pst_Geo)
{
    ERR_X_Assert(_pst_Packer != NULL);
    ERR_X_Assert(_pst_GO     != NULL);
    ERR_X_Assert(_pst_Geo    != NULL);

    if (_pst_Geo->l_NbPoints <= 0)
        return;

    // Compute the world space position so we'll be able to match vertices
    GEO_Vertex* pWorldVertices = (GEO_Vertex*)MEM_p_Alloc(_pst_Geo->l_NbPoints * sizeof(GEO_Vertex));
    for (LONG i = 0; i < _pst_Geo->l_NbPoints; ++i)
    {
        MATH_TransformVertex(&pWorldVertices[i], _pst_GO->pst_GlobalMatrix, &_pst_Geo->dst_Point[i]);
    }

    _pst_Packer->BeginObject();

    // Only need the points and texture coordinates since we only want the tangent space
    _pst_Packer->SetPositionSource(pWorldVertices, (ULONG)_pst_Geo->l_NbPoints);
    _pst_Packer->SetBlendingSource(NULL, 0);
    _pst_Packer->SetColorSource(NULL);
    _pst_Packer->SetTexCoordSource(_pst_Geo->dst_UV, (ULONG)_pst_Geo->l_NbUVs);
    _pst_Packer->SetLMCoordSource(NULL);

    for (LONG l_Element = 0; l_Element < _pst_Geo->l_NbElements; ++l_Element)
    {
        GEO_tdst_ElementIndexedTriangles* pst_Element = &_pst_Geo->dst_Element[l_Element];

        if (pst_Element->l_NbTriangles <= 0)
            continue;

        MAT_tdst_MultiTexture* pst_MT = GEO_pst_GetMultiTexture((MAT_tdst_Material*)_pst_GO->pst_Base->pst_Visu->pst_Material, pst_Element->l_MaterialId);
        if ((pst_MT != NULL) && (pst_MT->pst_FirstLevel != NULL) && (pst_MT->pst_FirstLevel->pst_XeLevel != NULL))
        {
            _pst_Packer->SetSmoothThreshold((ULONG)l_Element, pst_MT->pst_FirstLevel->pst_XeLevel->f_SmoothThreshold);
        }

        for (LONG l_Triangle = 0; l_Triangle < pst_Element->l_NbTriangles; ++l_Triangle)
        {
            GEO_tdst_IndexedTriangle* pst_Triangle = &pst_Element->dst_Triangle[l_Triangle];

            _pst_Packer->AddTriangle(pst_Triangle->auw_Index[0], pst_Triangle->auw_UV[0], 
                                     pst_Triangle->auw_Index[1], pst_Triangle->auw_UV[1],
                                     pst_Triangle->auw_Index[2], pst_Triangle->auw_UV[2],
                                     (ULONG)l_Element);
        }
    }

    _pst_Packer->EndObject();

    MEM_Free(pWorldVertices);
}

void GEO_AddGAOToMeshPacker(XeMeshPacker* _pst_Packer, OBJ_tdst_GameObject* _pst_GO)
{
    ERR_X_Assert(_pst_Packer != NULL);
    ERR_X_Assert(_pst_GO     != NULL);

    if (!GEO_b_CanPackObject(_pst_GO))
        return;

    if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        GEO_CleanGameObjectPack(_pst_GO);

        GRO_tdst_Struct* pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Object;
        if (pst_Gro == NULL)
            return;

        switch (pst_Gro->i->ul_Type)
        {
            case GRO_Geometric:
                GEO_AddGEOToMeshPacker(_pst_Packer, _pst_GO, (GEO_tdst_Object*)pst_Gro);
                break;

            case GRO_GeoStaticLOD:
                {
                    GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Gro;
                    if ((pst_LOD->uc_NbLOD                > 0)    &&
                        (pst_LOD->dpst_Id[0]             != NULL) &&
                        (pst_LOD->dpst_Id[0]->i->ul_Type == GRO_Geometric))
                    {
                        GEO_AddGEOToMeshPacker(_pst_Packer, _pst_GO, (GEO_tdst_Object*)pst_LOD->dpst_Id[0]);
                    }
                }
                break;
        }
    }
}

void GEO_PackGameObjectArray(OBJ_tdst_GameObject** _ppst_Objects, ULONG _ul_Count)
{
    ERR_X_Assert(_ppst_Objects != NULL);

    ULONG i;

    if (_ul_Count == 0)
        return;

    if (!GDI_b_IsXenonGraphics())
        return;

    TangentSpaceInfoArray aoTangentSpace;

    // Compute the global tangent space basis and store the result for future use
    {
        XeMeshPacker oMeshPacker;

        // Prepare the objects
        for (i = 0; i < _ul_Count; ++i)
        {
            GEO_AddGAOToMeshPacker(&oMeshPacker, _ppst_Objects[i]);
        }

        // Compute and extract the tangent space
        oMeshPacker.ExtractTangentSpace(&aoTangentSpace);
    }

    // Pack the objects
    {
        // Enable global tangent space
        s_paoGlobalTangentSpace = &aoTangentSpace;

        for (i = 0; i < _ul_Count; ++i)
        {
            MATH_tdst_Matrix st_TempMatrix;

            // World matrix
            s_stObjectGlobalMatrix = *_ppst_Objects[i]->pst_GlobalMatrix;

            // Normal inverse transform
            st_TempMatrix = *_ppst_Objects[i]->pst_GlobalMatrix;
            MATH_ClearAnyScaleType(&st_TempMatrix);
            MATH_InvertMatrix(&s_stObjectInvNormalTransform, &st_TempMatrix);

            GEO_PackGameObject(_ppst_Objects[i], TRUE);
        }

        // Disable global tangent space
        s_paoGlobalTangentSpace = NULL;
    }
}

D3DXVECTOR3 GEO_ReverseTransformVector(MATH_tdst_Matrix* _pst_Matrix, const D3DXVECTOR3* _pst_Vector)
{
    MATH_tdst_Vector vVec;
    MATH_tdst_Vector vXVec;

    vVec.x = _pst_Vector->x;
    vVec.y = _pst_Vector->y;
    vVec.z = _pst_Vector->z;

    MATH_TransformVector(&vXVec, _pst_Matrix, &vVec);

    return D3DXVECTOR3(vXVec.x, vXVec.y, vXVec.z);
}

TangentSpaceInfo* GEO_pst_GetBestTangentSpaceMatch(const D3DXVECTOR3* _pst_Pos, 
                                                   const D3DXVECTOR3* _pst_Normal)
{
    TangentSpaceInfo* pBestMatch    = NULL;
    FLOAT             fBestMatchDot = -2.0f;
    MATH_tdst_Vector  vObjPos;
    MATH_tdst_Vector  vWorldPos;
    D3DXVECTOR3       vWPos;
    D3DXVECTOR3       vWNormal;
    ULONG i;

    vObjPos.x = _pst_Pos->x;
    vObjPos.y = _pst_Pos->y;
    vObjPos.z = _pst_Pos->z;
    MATH_TransformVertex(&vWorldPos, &s_stObjectGlobalMatrix, &vObjPos);
    vWPos.x = vWorldPos.x;
    vWPos.y = vWorldPos.y;
    vWPos.z = vWorldPos.z;

    vWNormal = GEO_ReverseTransformVector(&s_stObjectInvNormalTransform, _pst_Normal);

    for (i = 0; i < s_paoGlobalTangentSpace->size(); ++i)
    {
        TangentSpaceInfo* pInfo = &(*s_paoGlobalTangentSpace)[i];

        if (IsPositionEqual(vWPos, pInfo->vPosition, 0.00001f))
        {
            FLOAT fDot = D3DXVec3Dot(&vWNormal, &pInfo->vNormal);
            if (fDot > fBestMatchDot)
            {
                pBestMatch    = pInfo;
                fBestMatchDot = fDot;
            }
        }
    }

    return pBestMatch;
}

#endif

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - VertexSkinning
// ------------------------------------------------------------------------------------------------

VertexSkinning::VertexSkinning()
{
    int i;

    for (i = 0; i < XENON_MAX_WEIGHT; i++)
    {
        m_abyMatrixIndex[i] = 0;
        m_afWeight[i] = 0.0f;
    }
}

void VertexSkinning::Add(int iMatrixIndex, float fWeight)
{
    int i, j, iSmallestWeightIndex = 0;
    float fSmallestWeight = 1000.0f;

    for (i = 0; i < XENON_MAX_WEIGHT; i++)
    {
        if (fWeight > m_afWeight[i])
        {
            // Found new weight to add.

            // find the smallest weight
            for (j = 0; j < XENON_MAX_WEIGHT; j++)
            {
                if (m_afWeight[j] < fSmallestWeight)
                {
                    fSmallestWeight = m_afWeight[j];
                    iSmallestWeightIndex = j;
                }
            }

            // replace smallest weight with new weight
            m_afWeight[iSmallestWeightIndex] = fWeight;
            
            ERR_X_Assert(iMatrixIndex < 256);
            m_abyMatrixIndex[iSmallestWeightIndex] = iMatrixIndex;

            return;
        }
    }

    return;
}

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - XePackSkinning
// ------------------------------------------------------------------------------------------------

XePackSkinning::XePackSkinning(GEO_tdst_Object* poGeo)
{
    m_iMaxMatrixIndex = 0;
    m_iVertexCount    = 0;
    m_iMaxNbrOfWeight = 0;

    if ((poGeo->p_SKN_Objectponderation == NULL) || 
        (poGeo->p_SKN_Objectponderation->NumberPdrtLists == 0) ||
        (poGeo->p_SKN_Objectponderation->pp_PdrtLst == NULL))
    {        
        m_poVertex = NULL;
        return;
    }

#if defined(ACTIVE_EDITORS)
    GEO_SKN_Compress(poGeo);
#endif

    int iMatrixCount = poGeo->p_SKN_Objectponderation->NumberPdrtLists;

    int iCurrentMatrix;
    int iCurrentWeight;

    m_iVertexCount = poGeo->l_NbPoints;
    m_poVertex     = new VertexSkinning[poGeo->l_NbPoints];

    // -- Build Skin information --
    // For each matrix
    for (iCurrentMatrix = 0; iCurrentMatrix < iMatrixCount; iCurrentMatrix++)
    {
        GEO_tdst_VertexPonderationList* poWeightList = poGeo->p_SKN_Objectponderation->pp_PdrtLst[iCurrentMatrix];

        for (iCurrentWeight = 0; iCurrentWeight < poWeightList->us_NumberOfPonderatedVertices; iCurrentWeight++)
        {
            int iCurrentIndex = poWeightList->p_PdrtVrc_C[iCurrentWeight].Index;

            m_poVertex[iCurrentIndex].Add(iCurrentMatrix, *(float *)&poWeightList->p_PdrtVrc_C[iCurrentWeight]);

            if (iCurrentMatrix > m_iMaxMatrixIndex)
            {
                m_iMaxMatrixIndex = iCurrentMatrix;
            }
        }
    }

    // -- Make sure the weight sum is 1 --
    // -- Find Max number of Weight
    for (int iCurVertex = 0; iCurVertex < m_iVertexCount; iCurVertex++)
    {
        float fWeightSum = 0.0f;
        INT   iWeightCount = 0;

        // add up all weights
        for (iCurrentWeight = 0; iCurrentWeight < XENON_MAX_WEIGHT; iCurrentWeight++)
        {
            float fCurrentWeight = m_poVertex[iCurVertex].m_afWeight[iCurrentWeight];
            fWeightSum += fCurrentWeight;

            if (fCurrentWeight > 0.0f)
            {
                iWeightCount++;
            }
        }

        if (fWeightSum > 0.0f)
        {
            if (iWeightCount > m_iMaxNbrOfWeight)
            {
                m_iMaxNbrOfWeight = iWeightCount;
            }

            // renormalize all weights
            for (iCurrentWeight = 0; iCurrentWeight < XENON_MAX_WEIGHT; iCurrentWeight++)
            {
                m_poVertex[iCurVertex].m_afWeight[iCurrentWeight] /= fWeightSum;
            }
        }
        else
        {
#if !defined(ACTIVE_EDITORS)
            // skinning with no weight
            ERR_X_Assert(FALSE);
#endif
        }

#ifdef _XENON
        // swap bytes for big endian
        BYTE byTemp = m_poVertex[iCurVertex].m_abyMatrixIndex[0];
        m_poVertex[iCurVertex].m_abyMatrixIndex[0] = m_poVertex[iCurVertex].m_abyMatrixIndex[3];
        m_poVertex[iCurVertex].m_abyMatrixIndex[3] = byTemp;
        byTemp = m_poVertex[iCurVertex].m_abyMatrixIndex[1];
        m_poVertex[iCurVertex].m_abyMatrixIndex[1] = m_poVertex[iCurVertex].m_abyMatrixIndex[2];
        m_poVertex[iCurVertex].m_abyMatrixIndex[2] = byTemp;
#endif
    }
}

XePackSkinning::~XePackSkinning()
{
    SAFE_DELETE(m_poVertex);
}


// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - XeMeshPacker
// ------------------------------------------------------------------------------------------------

XeMeshPacker::XeMeshPacker(void)
{
    m_pPositionSrc = NULL;
    m_pSkinningSrc = NULL;
    m_pColorSrc    = NULL;
    m_pTexCoordSrc = NULL;
    m_pLMCoordSrc  = NULL;

    for (ULONG i = 0; i < MAX_ELEMENTS; ++i)
    {
        m_aoElements[i].f_SmoothThreshold = Cf_PiBy180 * 180.0f;
    }

    m_dulPositionRemapTable = NULL;
    m_dulBlendingRemapTable = NULL;
    m_dulTexCoordRemapTable = NULL;

#if defined(ACTIVE_EDITORS)
    m_bDisableSaving        = FALSE;
    m_bEditionMode          = FALSE;
    m_bCanSave              = FALSE;
    m_ulSaveBufferSize      = 0;
    m_ulObjectStartPosition = 0;
    m_ulObjectStartElement  = 0;
    m_ulMaxElement          = 0;
#endif
}

XeMeshPacker::~XeMeshPacker(void)
{
    if (m_dulPositionRemapTable)
    {
        MEM_Free(m_dulPositionRemapTable);
        m_dulPositionRemapTable = NULL;
    }

    if (m_dulBlendingRemapTable)
    {
        MEM_Free(m_dulBlendingRemapTable);
        m_dulBlendingRemapTable = NULL;
    }

    if (m_dulTexCoordRemapTable)
    {
        MEM_Free(m_dulTexCoordRemapTable);
        m_dulTexCoordRemapTable = NULL;
    }
}

void XeMeshPacker::SetPositionSource(GEO_Vertex* _pPositions, ULONG _ulNbPositions)
{
    m_pPositionSrc = _pPositions;

#if defined(ACTIVE_EDITORS)
    if (m_bEditionMode)
        return;
#endif

    m_dulPositionRemapTable = (ULONG*)MEM_p_Alloc(_ulNbPositions * sizeof(ULONG));
    L_memset(m_dulPositionRemapTable, 0, _ulNbPositions * sizeof(ULONG));

    m_aoPositions.reserve(_ulNbPositions);

    // Add all the positions to build the remapping table
    for (ULONG i = 0; i < _ulNbPositions; ++i)
    {
        ULONG ulVertexIndex = PushPosition(i);

        m_dulPositionRemapTable[i] = ulVertexIndex;
    }
}

void XeMeshPacker::SetBlendingSource(VertexSkinning* _pSkinningInfo, ULONG _ulCount)
{
    m_pSkinningSrc = _pSkinningInfo;

#if defined(ACTIVE_EDITORS)
    if (m_bEditionMode)
        return;
#endif

    if (m_pSkinningSrc != NULL)
    {
        m_dulBlendingRemapTable = (ULONG*)MEM_p_Alloc(_ulCount * sizeof(ULONG));
        L_memset(m_dulBlendingRemapTable, 0, _ulCount * sizeof(ULONG));

        // Build the remapping table
        for (ULONG i = 0; i < _ulCount; ++i)
        {
            ULONG ulBlending = PushBlending(i);

            m_dulBlendingRemapTable[i] = ulBlending;
        }
    }
}

void XeMeshPacker::SetColorSource(ULONG* _pColors)
{
    m_pColorSrc = _pColors;
}

void XeMeshPacker::SetTexCoordSource(GEO_tdst_UV* _pTexCoords, ULONG _ulCount)
{
    m_pTexCoordSrc = _pTexCoords;

#if defined(ACTIVE_EDITORS)
    if (m_bEditionMode)
        return;
#endif

    if (m_pTexCoordSrc != NULL)
    {
        m_dulTexCoordRemapTable = (ULONG*)MEM_p_Alloc(_ulCount * sizeof(ULONG));
        L_memset(m_dulTexCoordRemapTable, 0, _ulCount * sizeof(ULONG));

        // Build the remapping table
        for (ULONG i = 0; i < _ulCount; ++i)
        {
            ULONG ulTexCoord = PushTexCoord(i);

            m_dulTexCoordRemapTable[i] = ulTexCoord;
        }
    }
}

void XeMeshPacker::SetLMCoordSource(FLOAT** _ppLMCoords)
{
    m_pLMCoordSrc = _ppLMCoords;
}

void XeMeshPacker::SetSmoothThreshold(ULONG _ulElementIndex, FLOAT _fThreshold)
{
#if defined(ACTIVE_EDITORS)
    _ulElementIndex += m_ulObjectStartElement;
    if (_ulElementIndex > m_ulMaxElement)
    {
        m_ulMaxElement = _ulElementIndex;
    }
#endif

    m_aoElements[_ulElementIndex].f_SmoothThreshold = Cf_PiBy180 * _fThreshold;
}

void XeMeshPacker::AddTriangle(ULONG _ulVertexIndex1, ULONG _ulTexCoordIndex1,
                               ULONG _ulVertexIndex2, ULONG _ulTexCoordIndex2,
                               ULONG _ulVertexIndex3, ULONG _ulTexCoordIndex3,
                               ULONG _ulElementIndex)
{
    if (m_pPositionSrc == NULL)
        return;

#if defined(ACTIVE_EDITORS)
    _ulElementIndex += m_ulObjectStartElement;
#endif

    D3DXVECTOR3 v1(m_pPositionSrc[_ulVertexIndex1].x, m_pPositionSrc[_ulVertexIndex1].y, m_pPositionSrc[_ulVertexIndex1].z);
    D3DXVECTOR3 v2(m_pPositionSrc[_ulVertexIndex2].x, m_pPositionSrc[_ulVertexIndex2].y, m_pPositionSrc[_ulVertexIndex2].z);
    D3DXVECTOR3 v3(m_pPositionSrc[_ulVertexIndex3].x, m_pPositionSrc[_ulVertexIndex3].y, m_pPositionSrc[_ulVertexIndex3].z);

    // Compute the triangle normal
    v2 -= v1;
    v3 -= v1;
    D3DXVec3Cross(&v1, &v2, &v3);
    D3DXVec3Normalize(&v1, &v1);

    Triangle triangle;
    triangle.vertexIndices[0] = (USHORT)AddVertex(_ulVertexIndex1, _ulTexCoordIndex1, v1, _ulElementIndex);
    triangle.vertexIndices[1] = (USHORT)AddVertex(_ulVertexIndex2, _ulTexCoordIndex2, v1, _ulElementIndex);
    triangle.vertexIndices[2] = (USHORT)AddVertex(_ulVertexIndex3, _ulTexCoordIndex3, v1, _ulElementIndex);

    m_aoElements[_ulElementIndex].ao_Triangles.push_back(triangle);
}

ULONG XeMeshPacker::AddVertex(ULONG _ulVertexIndex, ULONG _ulTexCoordIndex, D3DXVECTOR3 _vTriangleNormal, ULONG _ulElementIndex)
{
    Vertex vertex;

    vertex.ulPositionIndex  = AddPosition(_ulVertexIndex, _vTriangleNormal, _ulElementIndex);
    vertex.ulBlendingIndex  = AddBlending(_ulVertexIndex);
    vertex.ulColor          = (m_pColorSrc != NULL) ? m_pColorSrc[_ulVertexIndex] : 0;
    vertex.ulTexCoordsIndex = AddTexCoord(_ulTexCoordIndex);
    vertex.ulLMCoordsIndex  = AddLMCoord(_ulVertexIndex, _ulElementIndex);
    vertex.ulOriginalIndex  = _ulVertexIndex;

    ULONG ulNbVertices = m_aoVertices.size();
    ULONG i;

#if defined(ACTIVE_EDITORS)
    if (!m_bEditionMode)
#endif
    {
        for (i = 0; i < ulNbVertices; ++i)
        {
            Vertex* pVertex = &m_aoVertices[i];

            if (pVertex->ulPositionIndex != vertex.ulPositionIndex)
                continue;

            if (pVertex->ulBlendingIndex != vertex.ulBlendingIndex)
                continue;

            if (pVertex->ulColor != vertex.ulColor)
                continue;

            if (pVertex->ulTexCoordsIndex != vertex.ulTexCoordsIndex)
                continue;

            if (pVertex->ulLMCoordsIndex != vertex.ulLMCoordsIndex)
                continue;

            return i;
        }
    }

    m_aoVertices.push_back(vertex);

    return ulNbVertices;
}

ULONG XeMeshPacker::PushPosition(ULONG _ulVertexIndex)
{
    D3DXVECTOR3 pos(m_pPositionSrc[_ulVertexIndex].x, m_pPositionSrc[_ulVertexIndex].y, m_pPositionSrc[_ulVertexIndex].z);
    ULONG ulNbPos = m_aoPositions.size();

#if defined(ACTIVE_EDITORS)
    ULONG i;
    for (i = 0; i < m_ulObjectStartPosition; ++i)
    {
        if (IsPositionEqual(pos, m_aoPositions[i].vPosition, 0.00001f))
        {
            return i;
        }
    }
#endif

    Position position;
    position.vPosition          = pos;
    position.vNormal            = XENON_D3DVEC3_ZERO;
    position.ulNextMatchIndex   = INVALID_MATCH;
    position.ulUsed             = 0;
    position.vS                 = XENON_D3DVEC3_ZERO;
    position.fBinormalDirection = 1.0f;
    position.vT                 = XENON_D3DVEC3_ZERO;
    position.vSxT               = XENON_D3DVEC3_ZERO;
    m_aoPositions.push_back(position);

    return ulNbPos;
}

ULONG XeMeshPacker::PushBlending(ULONG _ulIndex)
{
    VertexSkinning* pSkinning = &m_pSkinningSrc[_ulIndex];
    ULONG ulNbBlend = m_aoBlendingInfo.size();
    ULONG i;

#if defined(ACTIVE_EDITORS)
    if (!m_bEditionMode)
#endif
    {
        for (i = 0; i < ulNbBlend; ++i)
        {
            BlendingInfo* pBlending = &m_aoBlendingInfo[i];

            if ((pBlending->abyIndices[0] != pSkinning->m_abyMatrixIndex[0]) ||
                (pBlending->abyIndices[1] != pSkinning->m_abyMatrixIndex[1]) ||
                (pBlending->abyIndices[2] != pSkinning->m_abyMatrixIndex[2]) ||
                (pBlending->abyIndices[3] != pSkinning->m_abyMatrixIndex[3]))
                continue;

            if ((fabsf(pBlending->afWeights[0] - pSkinning->m_afWeight[0]) > Cf_EpsilonBig) ||
                (fabsf(pBlending->afWeights[1] - pSkinning->m_afWeight[1]) > Cf_EpsilonBig) ||
                (fabsf(pBlending->afWeights[2] - pSkinning->m_afWeight[2]) > Cf_EpsilonBig) ||
                (fabsf(pBlending->afWeights[3] - pSkinning->m_afWeight[3]) > Cf_EpsilonBig))
                continue;

            return i;
        }
    }

    BlendingInfo oInfo;
    oInfo.afWeights[0]  = pSkinning->m_afWeight[0];
    oInfo.afWeights[1]  = pSkinning->m_afWeight[1];
    oInfo.afWeights[2]  = pSkinning->m_afWeight[2];
    oInfo.afWeights[3]  = pSkinning->m_afWeight[3];
    oInfo.abyIndices[0] = pSkinning->m_abyMatrixIndex[0];
    oInfo.abyIndices[1] = pSkinning->m_abyMatrixIndex[1];
    oInfo.abyIndices[2] = pSkinning->m_abyMatrixIndex[2];
    oInfo.abyIndices[3] = pSkinning->m_abyMatrixIndex[3];
    m_aoBlendingInfo.push_back(oInfo);

    return ulNbBlend;
}

ULONG XeMeshPacker::PushTexCoord(ULONG _ulTexCoordIndex)
{
    D3DXVECTOR2 oTC(m_pTexCoordSrc[_ulTexCoordIndex].fU, m_pTexCoordSrc[_ulTexCoordIndex].fV);
    ULONG ulNbTexCoords = m_aoTexCoords.size();
    ULONG i;

#if defined(ACTIVE_EDITORS)
    if (!m_bEditionMode)
#endif
    {
        for (i = 0; i < ulNbTexCoords; ++i)
        {
            D3DXVECTOR2* pTexCoord = &m_aoTexCoords[i];

            if ((fabsf(oTC.x - pTexCoord->x) > Cf_EpsilonBig) ||
                (fabsf(oTC.y - pTexCoord->y) > Cf_EpsilonBig))
                continue;

            return i;
        }
    }

    m_aoTexCoords.push_back(oTC);

    return ulNbTexCoords;
}

void XeMeshPacker::SnapPositions(void)
{
    ULONG ulNbPos = m_aoPositions.size();
    ULONG i;

    for (i = 0; i < ulNbPos; ++i)
    {
        Position* pPos = &m_aoPositions[i];

        pPos->vPosition.x = SnapFloat(pPos->vPosition.x);
        pPos->vPosition.y = SnapFloat(pPos->vPosition.y);
        pPos->vPosition.z = SnapFloat(pPos->vPosition.z);
    }
}

void XeMeshPacker::ComputeTangentSpace(void)
{
    const FLOAT SMALL_FLOAT = 1e-12f;

    ULONG        aulVertices[3];
    ULONG        aulPositions[3];
    D3DXVECTOR2* pUVs[3];
    D3DXVECTOR3  vEdge01;
    D3DXVECTOR3  vEdge02;
    D3DXVECTOR3  vCross;
    D3DXVECTOR3  vUnitS;
    D3DXVECTOR3  vUnitT;
    D3DXVECTOR3  vBasisS;
    D3DXVECTOR3  vBasisT;

#if defined(ACTIVE_EDITORS)
    // Use the global tangent space instead of recomputing?
    if (s_paoGlobalTangentSpace != NULL)
    {
        for (ULONG ulPosition = 0; ulPosition < m_aoPositions.size(); ++ulPosition)
        {
            Position*         pPos  = &m_aoPositions[ulPosition];
            TangentSpaceInfo* pInfo = GEO_pst_GetBestTangentSpaceMatch(&pPos->vPosition, &pPos->vNormal);

            if (pInfo != NULL)
            {
                pPos->vNormal            = GEO_ReverseTransformVector(&s_stObjectInvNormalTransform, &pInfo->vNormal);
                pPos->vT                 = GEO_ReverseTransformVector(&s_stObjectInvNormalTransform, &pInfo->vTangent);
                pPos->vS                 = GEO_ReverseTransformVector(&s_stObjectInvNormalTransform, &pInfo->vBinormal);
                pPos->vSxT               = GEO_ReverseTransformVector(&s_stObjectInvNormalTransform, &pInfo->vSxT);
                pPos->fBinormalDirection = pInfo->fBinormalDirection;
            }
        }

        return;
    }
#endif

    // Reset the match indices since we are going to use them to duplicate vertices with 
    // non-friendly texture coordinates
    for (ULONG ulPosition = 0; ulPosition < m_aoPositions.size(); ++ulPosition)
    {
        m_aoPositions[ulPosition].ulNextMatchIndex = INVALID_MATCH;
    }

    for (ULONG ulVertex = 0; ulVertex < m_aoVertices.size(); ++ulVertex)
    {
        m_aoVertices[ulVertex].ulNextMatch = INVALID_MATCH;
    }

    for (ULONG ulElement = 0; ulElement < MAX_ELEMENTS; ++ulElement)
    {
        ElementInfo* pElement      = &m_aoElements[ulElement];
        ULONG        ulNbTriangles = pElement->ao_Triangles.size();

        if (ulNbTriangles == 0)
            continue;

        for (ULONG ulTriangle = 0; ulTriangle < ulNbTriangles; ++ulTriangle)
        {
            Triangle*    pTriangle  = &pElement->ao_Triangles[ulTriangle];

            for (ULONG i = 0; i < 3; ++i)
            {
                aulVertices[i]  = pTriangle->vertexIndices[i];
                aulPositions[i] = m_aoVertices[aulVertices[i]].ulPositionIndex;
                pUVs[i]         = &m_aoTexCoords[m_aoVertices[aulVertices[i]].ulTexCoordsIndex];
            }

            vEdge01.y = pUVs[1]->x - pUVs[0]->x;
            vEdge01.z = pUVs[1]->y - pUVs[0]->y;

            vEdge02.y = pUVs[2]->x - pUVs[0]->x;
            vEdge02.z = pUVs[2]->y - pUVs[0]->y;

            D3DXVECTOR3 vS(0.0f, 0.0f, 0.0f);
            D3DXVECTOR3 vT(0.0f, 0.0f, 0.0f);

            // X, S, T
            vEdge01.x = m_aoPositions[aulPositions[1]].vPosition.x - m_aoPositions[aulPositions[0]].vPosition.x;
            vEdge02.x = m_aoPositions[aulPositions[2]].vPosition.x - m_aoPositions[aulPositions[0]].vPosition.x;

            D3DXVec3Cross(&vCross, &vEdge01, &vEdge02);
            if (fabs(vCross.x) > SMALL_FLOAT)
            {
                FLOAT fdSdX = -vCross.y / vCross.x;
                FLOAT fdTdX = -vCross.z / vCross.x;

                vS.x += fdSdX;
                vT.x += fdTdX;
            }

            // Y, S, T
            vEdge01.x = m_aoPositions[aulPositions[1]].vPosition.y - m_aoPositions[aulPositions[0]].vPosition.y;
            vEdge02.x = m_aoPositions[aulPositions[2]].vPosition.y - m_aoPositions[aulPositions[0]].vPosition.y;

            D3DXVec3Cross(&vCross, &vEdge01, &vEdge02);
            if (fabs(vCross.x) > SMALL_FLOAT)
            {
                FLOAT fdSdY = -vCross.y / vCross.x;
                FLOAT fdTdY = -vCross.z / vCross.x;

                vS.y += fdSdY;
                vT.y += fdTdY;
            }

            // Z, S, T
            vEdge01.x = m_aoPositions[aulPositions[1]].vPosition.z - m_aoPositions[aulPositions[0]].vPosition.z;
            vEdge02.x = m_aoPositions[aulPositions[2]].vPosition.z - m_aoPositions[aulPositions[0]].vPosition.z;

            D3DXVec3Cross(&vCross, &vEdge01, &vEdge02);
            if (fabs(vCross.x) > SMALL_FLOAT)
            {
                FLOAT fdSdZ = -vCross.y / vCross.x;
                FLOAT fdTdZ = -vCross.z / vCross.x;

                vS.z += fdSdZ;
                vT.z += fdTdZ;
            }

            D3DXVec3Normalize(&vUnitS, &vS);
            D3DXVec3Normalize(&vUnitT, &vT);

            for (ULONG i = 0; i < 3; ++i)
            {
                m_aoPositions[aulPositions[i]].vS += vS;
                m_aoPositions[aulPositions[i]].vT += vT;
            }

            // SC: Not working in all cases, but can be fixed if needed - Do NOT remove
/*
            for (ULONG i = 0; i < 3; ++i)
            {
                FLOAT fLengthS = D3DXVec3Length(&m_aoPositions[aulPositions[i]].vS);
                FLOAT fLengthT = D3DXVec3Length(&m_aoPositions[aulPositions[i]].vT);

                if ((fLengthS < SMALL_FLOAT) && (fLengthT < SMALL_FLOAT))
                {
                    m_aoPositions[aulPositions[i]].vS += vS;
                    m_aoPositions[aulPositions[i]].vT += vT;
                }
                else
                {
                    ULONG ulPrev   = aulPositions[i];
                    ULONG ulOriPos = aulPositions[i];
                    bool  bFound   = false;

                    while (!bFound)
                    {
                        vBasisS  = m_aoPositions[aulPositions[i]].vS;
                        vBasisS *= 1.0f / fLengthS;
                        vBasisT  = m_aoPositions[aulPositions[i]].vT;
                        vBasisT *= 1.0f / fLengthT;

                        if ((D3DXVec3Dot(&vBasisS, &vUnitS) > 0.0f) || 
                            (D3DXVec3Dot(&vBasisT, &vUnitT) > 0.0f))
                        {
                            bFound = true;
                        }
                        else
                        {
                            if (m_aoPositions[aulPositions[i]].ulNextMatchIndex != INVALID_MATCH)
                            {
                                ulPrev          = aulPositions[i];
                                aulPositions[i] = m_aoPositions[ulPrev].ulNextMatchIndex;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }

                    if (!bFound)
                    {
                        Position oPosition;
                        ULONG    ulIndex = m_aoPositions.size();

                        oPosition.vPosition          = m_aoPositions[ulPrev].vPosition;
                        oPosition.vNormal            = m_aoPositions[ulPrev].vNormal;
                        oPosition.ulNextMatchIndex   = INVALID_MATCH;
                        oPosition.ulUsed             = 1;
                        oPosition.vS                 = XENON_D3DVEC3_ZERO;
                        oPosition.fBinormalDirection = 1.0f;
                        oPosition.vT                 = XENON_D3DVEC3_ZERO;
                        oPosition.vSxT               = XENON_D3DVEC3_ZERO;
                        m_aoPositions.push_back(oPosition);

                        m_aoPositions[ulPrev].ulNextMatchIndex = ulIndex;
                        aulPositions[i]                        = ulIndex;
                    }

                    m_aoPositions[aulPositions[i]].vS += vS;
                    m_aoPositions[aulPositions[i]].vT += vT;

                    if (ulOriPos != aulPositions[i])
                    {
                        ULONG ulPrevVertex = aulVertices[i];

                        bFound = false;

                        while (!bFound)
                        {
                            if (m_aoVertices[aulVertices[i]].ulPositionIndex == aulPositions[i])
                            {
                                bFound = true;
                            }
                            else
                            {
                                ulPrevVertex   = aulVertices[i];
                                aulVertices[i] = m_aoVertices[aulVertices[i]].ulNextMatch;

                                if (aulVertices[i] == INVALID_MATCH)
                                {
                                    break;
                                }
                            }
                        }

                        if (!bFound)
                        {
                            aulVertices[i] = m_aoVertices.size();

                            Vertex oVertex;

                            oVertex.ulPositionIndex  = aulPositions[i];
                            oVertex.ulBlendingIndex  = m_aoVertices[ulPrevVertex].ulBlendingIndex;
                            oVertex.ulColor          = m_aoVertices[ulPrevVertex].ulColor;
                            oVertex.ulTexCoordsIndex = m_aoVertices[ulPrevVertex].ulTexCoordsIndex;
                            oVertex.ulLMCoordsIndex  = m_aoVertices[ulPrevVertex].ulLMCoordsIndex;
                            oVertex.ulOriginalIndex  = m_aoVertices[ulPrevVertex].ulOriginalIndex;
                            oVertex.ulNextMatch      = INVALID_MATCH;
                            m_aoVertices.push_back(oVertex);

                            m_aoVertices[ulPrevVertex].ulNextMatch = aulVertices[i];
                        }

                        pTriangle->vertexIndices[i] = (USHORT)aulVertices[i];
                    }
                }
            }
*/
        }
    }

    // Normalize all basis vectors and compute SxT + BinormalDirection
    for (ULONG ulPosition = 0; ulPosition < m_aoPositions.size(); ++ulPosition)
    {
        Position* pPosition = &m_aoPositions[ulPosition];

        D3DXVec3Normalize(&pPosition->vS, &pPosition->vS);
        D3DXVec3Normalize(&pPosition->vT, &pPosition->vT);

        D3DXVec3Cross(&pPosition->vSxT, &pPosition->vS, &pPosition->vT);
        D3DXVec3Normalize(&pPosition->vSxT, &pPosition->vSxT);

        if (D3DXVec3Dot(&pPosition->vSxT, &pPosition->vNormal) < 0.0f)
        {
            pPosition->vSxT               = -pPosition->vSxT;
            pPosition->fBinormalDirection = -1.0f;
        }
    }
}

ULONG XeMeshPacker::AddPosition(ULONG _ulVertexIndex, const D3DXVECTOR3& _vNormal, ULONG _ulElementIndex)
{
#if defined(ACTIVE_EDITORS)
    if (m_bEditionMode)
    {
        m_dulPositionRemapTable[_ulVertexIndex] = PushPosition(_ulVertexIndex);
    }
#endif

    ULONG     ulPos = m_dulPositionRemapTable[_ulVertexIndex];
    Position* pPos  = &m_aoPositions[ulPos];

    if (pPos->ulUsed)
    {
        FLOAT fDot;

        while (ulPos != INVALID_MATCH)
        {
            pPos = &m_aoPositions[ulPos];
            fDot = D3DXVec3Dot(&_vNormal, &pPos->vNormal);
            if (fDot > 1.0f)
            {
                fDot = 1.0f;
            }

            if (fAcos(fDot) <= m_aoElements[_ulElementIndex].f_SmoothThreshold)
            {
                return ulPos;
            }

            ulPos = pPos->ulNextMatchIndex;
        }

        ulPos = m_aoPositions.size();

        pPos->ulNextMatchIndex = ulPos;

        // Must create a new one
        Position position;
        position.vPosition          = pPos->vPosition;
        position.vNormal            = _vNormal;
        position.ulNextMatchIndex   = INVALID_MATCH;
        position.ulUsed             = 1;
        position.vS                 = XENON_D3DVEC3_ZERO;
        position.fBinormalDirection = 1.0f;
        position.vT                 = XENON_D3DVEC3_ZERO;
        position.vSxT               = XENON_D3DVEC3_ZERO;

        m_aoPositions.push_back(position);
    }
    else
    {
        pPos->ulUsed  = 1;
        pPos->vNormal = _vNormal;
    }

    return ulPos;
}

ULONG XeMeshPacker::AddBlending(ULONG _ulIndex)
{
    if (m_pSkinningSrc == NULL)
        return INVALID_INDEX;

#if defined(ACTIVE_EDITORS)
    if (m_bEditionMode)
    {
        m_dulBlendingRemapTable[_ulIndex] = PushBlending(_ulIndex);
    }
#endif

    return m_dulBlendingRemapTable[_ulIndex];
}

ULONG XeMeshPacker::AddTexCoord(ULONG _ulIndex)
{
    if (m_pTexCoordSrc == NULL)
        return INVALID_INDEX;

#if defined(ACTIVE_EDITORS)
    if (m_bEditionMode)
    {
        m_dulTexCoordRemapTable[_ulIndex] = PushTexCoord(_ulIndex);
    }
#endif

    return m_dulTexCoordRemapTable[_ulIndex];
}

ULONG XeMeshPacker::AddLMCoord(ULONG _ulIndex, ULONG _ulElementIndex)
{
    if (m_pLMCoordSrc == NULL)
        return INVALID_INDEX;

    D3DXVECTOR2 oLMCoord(m_pLMCoordSrc[_ulElementIndex][_ulIndex * 2], m_pLMCoordSrc[_ulElementIndex][_ulIndex * 2 + 1]);
    ULONG ulNbLMCoords = m_aoLMCoords.size();
    ULONG i;

#if defined(ACTIVE_EDITORS)
    if (!m_bEditionMode)
#endif
    {
        for (i = 0; i < ulNbLMCoords; ++i)
        {
            if ((fabsf(oLMCoord.x - m_aoLMCoords[i].x) > Cf_EpsilonBig) ||
                (fabsf(oLMCoord.y - m_aoLMCoords[i].y) > Cf_EpsilonBig))
                continue;

            return i;
        }
    }

    m_aoLMCoords.push_back(oLMCoord);

    return ulNbLMCoords;
}

ULONG XeMeshPacker::GetVertexSize(ULONG* _pul_Components, BOOL _b_Tangent, BOOL _b_Skinned, BOOL _b_LightMaps, BOOL _b_Fur)
{
    ULONG ulVertexStride = 0;
    ULONG ulComponents   = 0;

    *_pul_Components = 0;

    // Position
    ulVertexStride += 3 * sizeof(FLOAT);
    ulComponents   |= XEVC_POSITION;

    // Color
    ulVertexStride += sizeof(ULONG);
    ulComponents   |= XEVC_COLOR0;

    // Texture coordinates
    if (
        (m_pTexCoordSrc != NULL)
#if defined(ACTIVE_EDITORS)
        || m_bEditionMode
#endif
        )
    {
        ulVertexStride += C_ul_TexCoord0Size;
        ulComponents   |= C_ul_TexCoord0Format;
    }

    // Skinning
    if (
        (_b_Skinned && (m_pSkinningSrc != NULL))
#if defined(ACTIVE_EDITORS)
        || m_bEditionMode
#endif
       )
    {
        ulVertexStride += C_ul_BlendSize;
        ulComponents   |= C_ul_BlendFormat;
    }

    // Normal
    ulVertexStride += C_ul_NormalSize;
    ulComponents   |= C_ul_NormalFormat;

    // Tangent space
    if (
        _b_Tangent
#if defined(ACTIVE_EDITORS)
        || m_bEditionMode
#endif
       )
    {
        ulVertexStride += C_ul_TangentSize;
        ulComponents   |= C_ul_TangentFormat;
    }

    if (
        _b_LightMaps
#if defined(ACTIVE_EDITORS)
        && !m_bEditionMode
#endif
       )
    {
        // SC: TOADD: Could probably be compressed in FLOAT16_2

        ulVertexStride += 2 * sizeof(FLOAT);
        ulComponents   |= XEVC_TEXCOORD1;
    }

    *_pul_Components = ulComponents;

    return ulVertexStride;
}

void XeMeshPacker::BuildMeshes(OBJ_tdst_GameObject* _pst_GO, GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo, ULONG _ul_MaxWeight, ULONG _ul_MaxBones, BOOL _b_Tangent, BOOL _b_Skinned, BOOL _b_LightMaps, BOOL _b_Fur)
{
    ULONG  ulNbVertices = 0;
    ULONG  ulComponents = 0;
    ULONG  ulVertexSize = 0;
    ULONG  ulCurOffset  = 0;
    ULONG  i;
    ULONG  j;
    UCHAR* pucBuffer;

    // Compute the tangent space (if needed)
    if (
        _b_Tangent
#if defined(ACTIVE_EDITORS)
        && !m_bEditionMode
#endif
       )
    {
        ComputeTangentSpace();
    }

    // Favor vertex welding
    // SC: Removed because it can cause problems (especially on objects with rotations)
    //SnapPositions();

    ulNbVertices = m_aoVertices.size();
    ulVertexSize = GetVertexSize(&ulComponents, _b_Tangent, _b_Skinned, _b_LightMaps, _b_Fur);

    if (!_b_Skinned)
    {
        _ul_MaxWeight = 0;
        _ul_MaxBones  = 0;
    }

#if defined(ACTIVE_EDITORS)
    BeginSave(_pst_GO, _pst_Visu, _pst_Geo);

    // Number of elements
    WriteULongs(NULL, (ULONG*)&_pst_Geo->l_NbElements, 1);

    // Number of vertices
    WriteULongs(NULL, &ulNbVertices, 1);

    // Vertex format
    WriteULongs(NULL, &ulComponents, 1);

    // Vertex stride
    WriteULongs(NULL, &ulVertexSize, 1);

    // Max weights
    WriteULongs(NULL, &_ul_MaxWeight, 1);

    // Max bones
    WriteULongs(NULL, &_ul_MaxBones, 1);
#endif

    // Create and lock the vertex buffer
    _pst_Visu->p_VBVertex = (XeBuffer*)g_XeBufferMgr.CreateVertexBuffer(ulNbVertices, ulVertexSize, TRUE);
    pucBuffer = (UCHAR*)_pst_Visu->p_VBVertex->Lock(ulNbVertices, ulVertexSize);

#if defined(ACTIVE_EDITORS)
    // We may not fill all fields so let's start with a clean array
    if (m_bEditionMode)
    {
        memset(pucBuffer, 0, ulNbVertices * ulVertexSize);
    }
#endif

    for (i = 0; i < ulNbVertices; ++i)
    {
        Vertex& st_Vertex = m_aoVertices[i];

        // Position
        WritePosition(st_Vertex.ulPositionIndex, &pucBuffer);

        // Color
        WriteColor(st_Vertex.ulColor, &pucBuffer);

        // Texture coordinates
        if (m_pTexCoordSrc != NULL)
        {
            WriteTexCoord(st_Vertex.ulTexCoordsIndex, &pucBuffer);
        }
#if defined(ACTIVE_EDITORS)
        else if (m_bEditionMode)
        {
            pucBuffer += C_ul_TexCoord0Size;
        }
#endif

        // Skinning
        if (_b_Skinned && (m_pSkinningSrc != NULL))
        {
            WriteBlend(st_Vertex.ulBlendingIndex, &pucBuffer);
        }
#if defined(ACTIVE_EDITORS)
        else if (m_bEditionMode)
        {
            pucBuffer += C_ul_BlendSize;
        }
#endif

        // Normal
        WriteNormal(st_Vertex.ulPositionIndex, &pucBuffer, _b_Tangent);

        // Tangent space
        if (_b_Tangent)
        {
            WriteTangent(st_Vertex.ulPositionIndex, &pucBuffer);
        }
#if defined(ACTIVE_EDITORS)
        else if (m_bEditionMode)
        {
            pucBuffer += C_ul_TangentSize;
        }
#endif

        if (
            _b_LightMaps
#if defined(ACTIVE_EDITORS)
            && !m_bEditionMode
#endif
           )
        {
            WriteLMCoord(st_Vertex.ulLMCoordsIndex, &pucBuffer);
        }
    }

    // We're done, unlock the buffer
    _pst_Visu->p_VBVertex->Unlock();

    // Fix 32 bit alignment
    WritePadding();

    for (i = 0; i < (ULONG)_pst_Visu->l_NbXeElements; ++i)
    {
        ULONG ulNbTriangles = m_aoElements[i].ao_Triangles.size();

        // Number of indices
#if defined(ACTIVE_EDITORS)
        ULONG ulNbIndices = 3 * ulNbTriangles;
        WriteULongs(NULL, &ulNbIndices, 1);
#endif

        if (ulNbTriangles == 0)
            continue;

        XeMesh* pst_Mesh = new XeMesh();

        _pst_Visu->p_XeElements[i].pst_Mesh = pst_Mesh;

        pst_Mesh->SetAxisAlignedBoundingVolume(_pst_Visu->p_XeElements[i].st_AABBMin, _pst_Visu->p_XeElements[i].st_AABBMax);

        pst_Mesh->AddStream(ulComponents, _pst_Visu->p_VBVertex);

        pst_Mesh->SetMaxWeights(_ul_MaxWeight);
        pst_Mesh->SetMaxBones(_ul_MaxBones);

        // Bounding box
#if defined(ACTIVE_EDITORS)
        WriteFloats(NULL, &_pst_Visu->p_XeElements[i].st_AABBMin.x, 3);
        WriteFloats(NULL, &_pst_Visu->p_XeElements[i].st_AABBMax.x, 3);
#endif

        // Create the element's index buffers
        XeIndexBuffer* pst_IndexBuffer = (XeIndexBuffer*)g_XeBufferMgr.CreateIndexBuffer(ulNbTriangles * 3);
        _pst_Visu->p_XeElements[i].pst_IndexBuffer = pst_IndexBuffer;

#if defined(ACTIVE_EDITORS)
        USHORT* puw_IndicesEd = (USHORT*)MEM_p_Alloc(ulNbTriangles * 3 * sizeof(USHORT));
        _pst_Visu->p_XeElements[i].puw_Indices = puw_IndicesEd;
#endif

        USHORT* puwIndices = (USHORT*)pst_IndexBuffer->Lock(ulNbTriangles * 3);

        for (j = 0; j < ulNbTriangles; ++j)
        {
            WriteShorts(puwIndices, (SHORT*)&m_aoElements[i].ao_Triangles[j].vertexIndices[0], 3);
            puwIndices += 3;

#if defined(ACTIVE_EDITORS)
            // Keep a copy of the index buffer for the hide faces mode
            *puw_IndicesEd = m_aoElements[i].ao_Triangles[j].vertexIndices[0]; ++puw_IndicesEd;
            *puw_IndicesEd = m_aoElements[i].ao_Triangles[j].vertexIndices[1]; ++puw_IndicesEd;
            *puw_IndicesEd = m_aoElements[i].ao_Triangles[j].vertexIndices[2]; ++puw_IndicesEd;
#endif
        }

        pst_IndexBuffer->Unlock();

        // Fix 32 bit alignment
        WritePadding();

        // Attach the index buffer
        pst_Mesh->SetIndices(_pst_Visu->p_XeElements[i].pst_IndexBuffer);
    }
    _pst_Visu->l_VBVertexCount = ulNbVertices;

    if (_b_Fur)
    {
#if defined(ACTIVE_EDITORS)
        // Fur data is present
        ULONG ulHasFur = 1;
        WriteULongs(NULL, &ulHasFur, 1);
#endif

        PrepareFurData(_pst_Visu, _b_Skinned, _b_Tangent);
    }
#if defined(ACTIVE_EDITORS)
    else
    {
        // Fur data is NOT present
        ULONG ulHasFur = 0;
        WriteULongs(NULL, &ulHasFur, 1);
    }
#endif

#if defined(ACTIVE_EDITORS)
    // Prepare the tangent space data
    if (_b_Tangent && !EDI_gb_ComputeMap)
    {
        // Tangent space information is present
        ULONG ulNbTangentPoints = (ULONG)_pst_Geo->l_NbPoints;
        WriteULongs(NULL, &ulNbTangentPoints, 1);

        PrepareTangentSpace(_pst_Geo, _pst_Visu);

        // Fix 32 bit alignment
        if (ulNbTangentPoints > 1)
            WritePadding();
    }
    else
    {
        // Tangent space information is NOT present
        ULONG ulNbTangentPoints = 0;
        WriteULongs(NULL, &ulNbTangentPoints, 1);
    }
#endif

#if defined(ACTIVE_EDITORS)
    EndSave(_pst_Visu);
#endif
}

#if defined(ACTIVE_EDITORS)

void XeMeshPacker::SetEditionModeVertexCount(ULONG _ul_NbVertices)
{
    m_dulPositionRemapTable = (ULONG*)MEM_p_Alloc(_ul_NbVertices * sizeof(ULONG));
    m_dulBlendingRemapTable = (ULONG*)MEM_p_Alloc(_ul_NbVertices * sizeof(ULONG));
    m_dulTexCoordRemapTable = (ULONG*)MEM_p_Alloc(_ul_NbVertices * sizeof(ULONG));

    for (ULONG i = 0; i < _ul_NbVertices; ++i)
    {
        m_dulPositionRemapTable[i] = i;
        m_dulBlendingRemapTable[i] = i;
        m_dulTexCoordRemapTable[i] = i;
    }
}

void XeMeshPacker::BeginObject(void)
{
    m_ulObjectStartPosition = m_aoPositions.size();
    m_ulObjectStartElement  = m_ulMaxElement;
}

void XeMeshPacker::EndObject(void)
{
    if (m_dulPositionRemapTable)
    {
        MEM_Free(m_dulPositionRemapTable);
        m_dulPositionRemapTable = NULL;
    }

    if (m_dulBlendingRemapTable)
    {
        MEM_Free(m_dulBlendingRemapTable);
        m_dulBlendingRemapTable = NULL;
    }

    if (m_dulTexCoordRemapTable)
    {
        MEM_Free(m_dulTexCoordRemapTable);
        m_dulTexCoordRemapTable = NULL;
    }
}

void XeMeshPacker::ExtractTangentSpace(TangentSpaceInfoArray* _pao_TangentSpace)
{
    ComputeTangentSpace();

    ULONG ulNbPositions = m_aoPositions.size();
    ULONG i;

    for (i = 0; i < ulNbPositions; ++i)
    {
        TangentSpaceInfo oInfo;
        Position*        pPos = &m_aoPositions[i];

        oInfo.vPosition          = pPos->vPosition;
        oInfo.vNormal            = pPos->vNormal;
        oInfo.vTangent           = pPos->vT;
        oInfo.vBinormal          = pPos->vS;
        oInfo.vSxT               = pPos->vSxT;
        oInfo.fBinormalDirection = pPos->fBinormalDirection;
        _pao_TangentSpace->push_back(oInfo);
    }
}

void XeMeshPacker::PrepareTangentSpace(GEO_tdst_Object* _pst_Geo, GRO_tdst_Visu* _pst_Visu)
{
    ULONG ulNbVertices = m_aoVertices.size();
    ULONG i;

    GEO_tdst_TextureSpaceBasis* pst_TangentSpace = (GEO_tdst_TextureSpaceBasis*)MEM_p_Alloc(_pst_Geo->l_NbPoints * sizeof(GEO_tdst_TextureSpaceBasis));

    L_memset(pst_TangentSpace, 0, _pst_Geo->l_NbPoints * sizeof(GEO_tdst_TextureSpaceBasis));

    for (i = 0; i < ulNbVertices; ++i)
    {
        Vertex*   pVertex = &m_aoVertices[i];
        Position* pPos    = &m_aoPositions[pVertex->ulPositionIndex];

        GEO_tdst_TextureSpaceBasis* pst_Basis = &pst_TangentSpace[pVertex->ulOriginalIndex];

        if (!pst_Basis->Active)
        {
            MATH_InitVector(&pst_Basis->S,   pPos->vS.x,      pPos->vS.y,      pPos->vS.z);
            MATH_InitVector(&pst_Basis->T,   pPos->vT.x,      pPos->vT.y,      pPos->vT.z);
#if defined(_XE_PACK_USE_SXT)
            MATH_InitVector(&pst_Basis->SxT, pPos->vSxT.x, pPos->vSxT.y, pPos->vSxT.z);
#else
            MATH_InitVector(&pst_Basis->SxT, pPos->vNormal.x, pPos->vNormal.y, pPos->vNormal.z);
#endif

            pst_Basis->BinormalDirection = pPos->fBinormalDirection;
            pst_Basis->Active            = 1;
        }
    }

    for (LONG l_Element = 0; l_Element < _pst_Geo->l_NbElements; ++l_Element)
    {
        ERR_X_Assert(_pst_Visu->p_XeElements[l_Element].dst_TangentSpace == NULL);
        _pst_Visu->p_XeElements[l_Element].dst_TangentSpace = (GEO_tdst_TextureSpaceBasis*)MEM_GEO_p_Alloc(_pst_Geo->l_NbPoints * sizeof(GEO_tdst_TextureSpaceBasis));
        L_memcpy(_pst_Visu->p_XeElements[l_Element].dst_TangentSpace, pst_TangentSpace, _pst_Geo->l_NbPoints * sizeof(GEO_tdst_TextureSpaceBasis));
    }

    // Output the buffer as-is to the file (Editor only)
    WriteUChars(NULL, (UCHAR*)pst_TangentSpace, _pst_Geo->l_NbPoints * sizeof(GEO_tdst_TextureSpaceBasis));

    MEM_Free(pst_TangentSpace);
}
#endif

void XeMeshPacker::PrepareFurData(GRO_tdst_Visu* _pst_Visu, BOOL _b_Skin, BOOL _b_Tangent)
{
    FurOffsetVertex* p_FurVertices;
    ULONG ulNbVertices = m_aoVertices.size();
    ULONG i;

    p_FurVertices                = (FurOffsetVertex*)MEM_p_Alloc(ulNbVertices * sizeof(FurOffsetVertex));
    _pst_Visu->p_FurOffsetVertex = p_FurVertices;

    L_memset(p_FurVertices, 0, ulNbVertices * sizeof(FurOffsetVertex));

    for (i = 0; i < ulNbVertices; ++i, ++p_FurVertices)
    {
        Vertex*   pVertex = &m_aoVertices[i];
        Position* pPos    = &m_aoPositions[pVertex->ulPositionIndex];

        // Position
        p_FurVertices->fX = pPos->vPosition.x;
        p_FurVertices->fY = pPos->vPosition.y;
        p_FurVertices->fZ = pPos->vPosition.z;

        // Blending information
        if (_b_Skin)
        {
            BlendingInfo* pBlend = &m_aoBlendingInfo[pVertex->ulBlendingIndex];

            p_FurVertices->afWeight[0] = pBlend->afWeights[0];
            p_FurVertices->afWeight[1] = pBlend->afWeights[1];
            p_FurVertices->afWeight[2] = pBlend->afWeights[2];
            p_FurVertices->afWeight[3] = pBlend->afWeights[3];

            p_FurVertices->abyIndex[0] = pBlend->abyIndices[0];
            p_FurVertices->abyIndex[1] = pBlend->abyIndices[1];
            p_FurVertices->abyIndex[2] = pBlend->abyIndices[2];
            p_FurVertices->abyIndex[3] = pBlend->abyIndices[3];
        }

        // Tangent space
        if (_b_Tangent)
        {
            p_FurVertices->fTangentX = pPos->vS.x;
            p_FurVertices->fTangentY = pPos->vS.y;
            p_FurVertices->fTangentZ = pPos->vS.z;
 
            D3DXVECTOR3 vBinormal;
            D3DXVec3Cross(&vBinormal, &pPos->vS, &pPos->vNormal);

            if (pPos->fBinormalDirection > 0.0f)
            {
                p_FurVertices->fBinormalX = vBinormal.x;
                p_FurVertices->fBinormalY = vBinormal.y;
                p_FurVertices->fBinormalZ = vBinormal.z;
            }
            else
            {
                p_FurVertices->fBinormalX = -vBinormal.x;
                p_FurVertices->fBinormalY = -vBinormal.y;
                p_FurVertices->fBinormalZ = -vBinormal.z;
            }
        }

#if defined(ACTIVE_EDITORS)
        // Save the fur vertex
        WriteFloats(NULL, &p_FurVertices->fX,              3);
        WriteFloats(NULL,  p_FurVertices->afWeight,        4);
        WriteULongs(NULL, (ULONG*)p_FurVertices->abyIndex, 1);
        WriteFloats(NULL, &p_FurVertices->fTangentX,       3);
        WriteFloats(NULL, &p_FurVertices->fBinormalX,      3);
#endif
    }

    _pst_Visu->p_VBFurOffsets = (XeBuffer*)g_XeBufferMgr.CreateDynVertexBuffer();

    // Add to the streams
    for (i = 0; i < (ULONG)_pst_Visu->l_NbXeElements; ++i)
    {
        if (_pst_Visu->p_XeElements[i].pst_Mesh != NULL)
        {
            _pst_Visu->p_XeElements[i].pst_Mesh->AddStream(XEVC_TEXCOORD1, _pst_Visu->p_VBFurOffsets);
        }
    }
}

void XeMeshPacker::WritePadding(void)
{
#if defined(ACTIVE_EDITORS)
    if (!m_bCanSave)
        return;

    while ((m_ulSaveBufferSize & 0x3) != 0)
    {
        UCHAR ucTemp = 0xfa;

        SAV_Buffer(&ucTemp, sizeof(UCHAR));
        ++m_ulSaveBufferSize;
    }
#endif
}

void XeMeshPacker::WriteUChars(void* _pcBuffer, UCHAR* _pucValue, ULONG _ulCount)
{
    if (_pcBuffer)
    {
        L_memcpy(_pcBuffer, _pucValue, _ulCount * sizeof(UCHAR));
    }

#if defined(ACTIVE_EDITORS)
    if (!m_bCanSave)
        return;

    SAV_Buffer(_pucValue, _ulCount * sizeof(UCHAR));
    m_ulSaveBufferSize += _ulCount;
#endif
}

void XeMeshPacker::WriteUByte4(void* _pcBuffer, UCHAR* _pucValue)
{
    if (_pcBuffer)
    {
        L_memcpy(_pcBuffer, _pucValue, 4 * sizeof(UCHAR));
    }

#if defined(ACTIVE_EDITORS)
    if (!m_bCanSave)
        return;

    SAV_Buffer(&_pucValue[3], sizeof(UCHAR));
    SAV_Buffer(&_pucValue[2], sizeof(UCHAR));
    SAV_Buffer(&_pucValue[1], sizeof(UCHAR));
    SAV_Buffer(&_pucValue[0], sizeof(UCHAR));

    m_ulSaveBufferSize += 4 * sizeof(UCHAR);
#endif
}

void XeMeshPacker::WriteShorts(void* _pcBuffer, SHORT* _pwValue,  ULONG _ulCount)
{
    if (_pcBuffer)
    {
        L_memcpy(_pcBuffer, _pwValue, _ulCount * sizeof(SHORT));
    }

#if defined(ACTIVE_EDITORS)
    if (!m_bCanSave)
        return;

    for (ULONG i = 0; i < _ulCount; ++i)
    {
        USHORT wVal = (USHORT)_pwValue[i];

        SwapWord(&wVal);

        SAV_Buffer(&wVal, sizeof(SHORT));

        m_ulSaveBufferSize += sizeof(SHORT);
    }
#endif
}

void XeMeshPacker::WriteULongs(void* _pcBuffer, ULONG* _pulValue, ULONG _ulCount)
{
    if (_pcBuffer)
    {
        L_memcpy(_pcBuffer, _pulValue, _ulCount * sizeof(ULONG));
    }

#if defined(ACTIVE_EDITORS)
    if (!m_bCanSave)
        return;

    for (ULONG i = 0; i < _ulCount; ++i)
    {
        ULONG ulVal = _pulValue[i];

        SwapDWord(&ulVal);

        SAV_Buffer(&ulVal, sizeof(ULONG));

        m_ulSaveBufferSize += sizeof(ULONG);
    }
#endif
}

void XeMeshPacker::WriteFloats(void* _pcBuffer, FLOAT* _pfValue,  ULONG _ulCount)
{
    if (_pcBuffer)
    {
        L_memcpy(_pcBuffer, _pfValue, _ulCount * sizeof(FLOAT));
    }

#if defined(ACTIVE_EDITORS)
    if (!m_bCanSave)
        return;

    for (ULONG i = 0; i < _ulCount; ++i)
    {
        ULONG ulVal = *(ULONG*)&_pfValue[i];

        SwapDWord(&ulVal);

        SAV_Buffer(&ulVal, sizeof(ULONG));

        m_ulSaveBufferSize += sizeof(ULONG);
    }
#endif
}

void XeMeshPacker::WritePosition(ULONG _ulIndex, UCHAR** _ppcBuffer)
{
    WriteFloats(*_ppcBuffer, (FLOAT*)&m_aoPositions[_ulIndex].vPosition.x, 3);

    *_ppcBuffer += 3 * sizeof(FLOAT);
}

void XeMeshPacker::WriteNormal(ULONG _ulIndex, UCHAR** _ppcBuffer, BOOL _b_NormalMap)
{
    D3DXVECTOR3* pst_Normal = &m_aoPositions[_ulIndex].vNormal;

#if defined(_XE_PACK_USE_SXT)
    if (_b_NormalMap)
    {
        pst_Normal = &m_aoPositions[_ulIndex].vSxT;
    }
#endif

#if defined(_XE_COMPRESS_NORMALS)

    ULONG ulCompressedNormal = XeMakeDEC3N((FLOAT*)pst_Normal);
    WriteULongs(*_ppcBuffer, &ulCompressedNormal, 1);

#else

    WriteFloats(*_ppcBuffer, (FLOAT*)pst_Normal, 3);

#endif

    *_ppcBuffer += C_ul_NormalSize;
}

void XeMeshPacker::WriteTangent(ULONG _ulIndex, UCHAR** _ppcBuffer)
{
#if defined(_XE_COMPRESS_TANGENTS)

    SHORT asTangent[4];

    XeMakeSHORT4N(asTangent, (FLOAT*)&m_aoPositions[_ulIndex].vS);
    WriteShorts(*_ppcBuffer, asTangent, 4);

#else

    WriteFloats(*_ppcBuffer, (FLOAT*)&m_aoPositions[_ulIndex].vS.x, 4);

#endif

    *_ppcBuffer += C_ul_TangentSize;
}

void XeMeshPacker::WriteBlend(ULONG _ulIndex, UCHAR** _ppcBuffer)
{
#if defined(_XE_COMPRESS_WEIGHTS)

    SHORT asWeights[4];

    XeMakeSHORT4N(asWeights, m_aoBlendingInfo[_ulIndex].afWeights);
    WriteShorts(*_ppcBuffer, asWeights, 4);
    *_ppcBuffer += 4 * sizeof(SHORT);

#else

    WriteFloats(*_ppcBuffer, m_aoBlendingInfo[_ulIndex].afWeights, 4);
    *_ppcBuffer += 4 * sizeof(FLOAT);

#endif

    WriteUByte4(*_ppcBuffer, m_aoBlendingInfo[_ulIndex].abyIndices);
    *_ppcBuffer += sizeof(ULONG);
}

void XeMeshPacker::WriteColor(ULONG _ulColor, UCHAR** _ppcBuffer)
{
    ULONG ulColor = XeConvertColor(_ulColor);

    WriteULongs(*_ppcBuffer, &ulColor, 1);
    *_ppcBuffer += sizeof(ULONG);
}

void XeMeshPacker::WriteTexCoord(ULONG _ulIndex, UCHAR** _ppcBuffer)
{
#if defined(_XE_COMPRESS_TEXCOORD0)
#pragma error(_XE_COMRESS_TEXCOORD0 not properly implemented - Should use FLOAT16_2)
#else
    WriteFloats(*_ppcBuffer, (FLOAT*)&m_aoTexCoords[_ulIndex].x, 2);
#endif

    *_ppcBuffer += C_ul_TexCoord0Size;
}

void XeMeshPacker::WriteLMCoord(ULONG _ulIndex, UCHAR** _ppcBuffer)
{
    WriteFloats(*_ppcBuffer, (FLOAT*)&m_aoLMCoords[_ulIndex].x, 2);
    *_ppcBuffer += 2 * sizeof(FLOAT);
}

#if defined(ACTIVE_EDITORS)

void XeMeshPacker::BeginSave(OBJ_tdst_GameObject* _pst_GO, GRO_tdst_Visu* _pst_Visu, GEO_tdst_Object* _pst_Geo)
{
    BIG_INDEX ulGaoIndex = LOA_ul_SearchIndexWithAddress((ULONG)_pst_GO);

    m_bCanSave         = FALSE;
    m_ulSaveBufferSize = 0;

    if (m_bDisableSaving)
        return;

    if (ulGaoIndex != BIG_C_InvalidIndex)
    {
        CHAR szFileName[32];
        CHAR szPackPathName[1024];

        GEO_ComputePackNameAndDir(ulGaoIndex, szPackPathName, szFileName);

        SAV_Begin(szPackPathName, szFileName);

        m_bCanSave         = TRUE;
        m_ulSaveBufferSize = 0;

        // Do not pack anything in _main_fix since the map will not be binarized
        if (strstr(szPackPathName, "_main/_main_fix"))
        {
            m_bCanSave = FALSE;
        }

        // Do not write to the BF while we are binarizing... Pre-processing should already have done that
        if (LOA_IsBinarizing())
        {
            m_bCanSave = FALSE;
        }

        // Version
        ULONG ulVersion = C_ul_PackVersion;
        WriteULongs(NULL, &ulVersion, 1);

        // CRC
        ULONG ulCRC = GEO_ul_ComputeCRC(_pst_Visu, _pst_Geo);
        WriteULongs(NULL, &ulCRC, 1);
    }
}

void XeMeshPacker::EndSave(GRO_tdst_Visu* _pst_Visu)
{
    if (!m_bCanSave)
        return;

    BIG_INDEX ulIndex = SAV_ul_End();

    _pst_Visu->ul_PackedDataKey = BIG_FileKey(ulIndex);
    m_bCanSave = FALSE;
}

#endif

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - XePackReader
// ------------------------------------------------------------------------------------------------

XePackReader::XePackReader(CHAR* _pc_Buffer, ULONG _ul_Size)
{
    m_pcBuffer = _pc_Buffer;
    m_ulSize   = _ul_Size;
    m_ulOffset = 0;
}

XePackReader::~XePackReader(void)
{
}

USHORT XePackReader::ReadUShort(void)
{
    USHORT uwVal = *(USHORT*)&m_pcBuffer[m_ulOffset];

    m_ulOffset += sizeof(USHORT);

#if !defined(_XENON)
    SwapWord(&uwVal);
#endif

    return uwVal;
}

ULONG XePackReader::ReadULong(void)
{
    ULONG ulVal = *(ULONG*)&m_pcBuffer[m_ulOffset];

    m_ulOffset += sizeof(ULONG);

#if !defined(_XENON)
    SwapDWord(&ulVal);
#endif

    return ulVal;
}

FLOAT XePackReader::ReadFloat(void)
{
#if defined(_XENON)
	// Make sure we handle misaligned floats (in binarized data)
	ULONG ulVal = *(ULONG*)&m_pcBuffer[m_ulOffset];
    FLOAT fVal  = *(FLOAT*)&ulVal;

    m_ulOffset += sizeof(FLOAT);

    return fVal;
#else
    ULONG ulVal = *(ULONG*)&m_pcBuffer[m_ulOffset];

    m_ulOffset += sizeof(FLOAT);

    SwapDWord(&ulVal);

    return *(FLOAT*)&ulVal;
#endif
}

void XePackReader::ReadArray(void* _p_Dest, ULONG _ul_Size)
{
    L_memcpy(_p_Dest, &m_pcBuffer[m_ulOffset], _ul_Size);

    m_ulOffset += _ul_Size;
}

void XePackReader::ReadVector(MATH_tdst_Vector* _pst_Vector)
{
    _pst_Vector->x = ReadFloat();
    _pst_Vector->y = ReadFloat();
    _pst_Vector->z = ReadFloat();
}

void XePackReader::SkipArray(ULONG _ul_Size)
{
    m_ulOffset += _ul_Size;
}

void XePackReader::UpdateAlignment(void)
{
    if ((m_ulOffset & 0x3) != 0)
    {
        m_ulOffset &= 0xfffffffc;
        m_ulOffset += 0x00000004;
    }
}

#endif // #if defined(_XENON_RENDER)
