// ------------------------------------------------------------------------------------------------
// File   : GEO_XenonMesh.cpp
// Date   : 2005-05-31
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
// Prevent STL for complaining all the time
#pragma warning(disable:4702)

#include "Precomp.h"

#if defined(ACTIVE_EDITORS) && defined(_XENON_RENDER)

#include <list>
#include <map>
#include <vector>
#include <algorithm>

#include <d3dx9.h>

#include "XenonGraphics/DevHelpers/XeBench.h"
#include "XenonGraphics/DevHelpers/XePerf.h"

#include "XenonGraphics/XeMaterial.h"
#include "XenonGraphics/XeRenderer.h"

#include "SDK/Sources/BIGfiles/BIGread.h"
#include "SDK/Sources/BIGfiles/LOAding/LOA.h"
#include "SDK/Sources/BIGfiles/LOAding/LOAdefs.h"
#include "SDK/Sources/BIGfiles/LOAding/LOAread.h"
#include "SDK/Sources/BIGfiles/SAVing/SAVdefs.h"
#include "SDK/Sources/BASe/BAScrc.h"

#include "ENGine/Sources/OBJects/OBJ.h"
#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENgine/Sources/INTersection/INTmain.h"

#include "GraphicDK/Sources/GRObject/GROstruct.h"
#include "GraphicDK/Sources/GEOmetric/GEOstaticLOD.h"
#include "GraphicDK/Sources/GEOmetric/GEO_SKIN.h"
#include "GraphicDK/Sources/GEOmetric/GEOobjectcomputing.h"
#include "GraphicDK/Sources/GEOmetric/GEOXenonPack.h"
#include "GraphicDK/Sources/TEXture/TEXstruct.h"
#include "GraphicDK/Sources/TEXture/TEXfile.h"
#include "GraphicDK/Sources/LIGHT/LIGHTmap.h"

#include "DIAlogs/DIA_UPDATE_dlg.h"

#include "XMP_XenonMesh.h"
#include "XMP_XenonMeshBuilder.h"

// ------------------------------------------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------------------------------------------
const CHAR XMP_GEO_SUFFIX[]  = "_XMP";
const CHAR XMP_GEO_SUFFIX2[] = "_XMP_LOD_XMP";
const CHAR XMP_LOD_SUFFIX[]  = "_XMP_LOD";
const CHAR XMP_XEM_SUFFIX[]  = "_XEM";

const ULONG XMP_PROCESSING_MASK = GRO_XMPF_EnableTessellation | GRO_XMPF_EnableDisplacement | GRO_XMPF_EnableChamfer;

const FLOAT XMP_SIMPLIFICATION_THRESHOLD = 0.005f;

const CHAR* XMP_LOG_INFO[XenonMeshPack::LL_COUNT] =
{
    "[XMP|Debug]",
    "[XMP|Info]",
    "[XMP|Warning]",
    "[XMP|Error]"
};

struct XMPObjectInfo
{
    OBJ_tdst_GameObject* pst_GO;
    GRO_tdst_Visu*       pst_Visu;
    GEO_tdst_Object*     pst_Geo;
};

typedef std::list<XMPObjectInfo> XMPObjectInfoList;

// ------------------------------------------------------------------------------------------------
// EXTERNAL FUNCTIONS
// ------------------------------------------------------------------------------------------------
extern ULONG  OBJ_ul_GameObjectRLICallback(ULONG _ul_FilePos);
extern ULONG  OBJ_ul_GameObject_SaveRLI(ULONG* _pul_RLI, char* _sz_Path, char* _sz_Name, int _i_Duplicate);
extern ULONG* OBJ_VertexColor_Realloc(OBJ_tdst_GameObject* _pst_GO, ULONG _ul_NewNumber);

extern void GEO_FreeContent(GEO_tdst_Object* _pst_Geo);

// ------------------------------------------------------------------------------------------------
// PRIVATE VARIABLES
// ------------------------------------------------------------------------------------------------
static EDIA_cl_UPDATEDialog* spo_UpdateDialog = NULL;
static FLOAT                 sf_Progress      = 0.0f;

// ------------------------------------------------------------------------------------------------
// FUNCTIONS
// ------------------------------------------------------------------------------------------------

static void XMP_ProcessGeometricObject(WOR_tdst_World*      _pst_World, 
                                       OBJ_tdst_GameObject* _pst_GO, 
                                       GRO_tdst_Visu*       _pst_Visu, 
                                       GEO_tdst_Object*     _pst_Geo);

static BOOL XMP_AddGEOToDatabase(XenonMeshPack::MeshBuilder* _pst_Database, 
                                 GRO_tdst_Visu*              _pst_Visu, 
                                 GEO_tdst_Object*            _pst_Geo,
                                 BOOL                        _b_SkipInformation);

static void XMP_AddGraphicObject(WOR_tdst_World*      _pst_World,
                                 OBJ_tdst_GameObject* _pst_GO,
                                 GRO_tdst_Visu*       _pst_Visu,
                                 GEO_tdst_Object*     _pst_NewGeo);

static GEO_tdst_Object* XMP_BuildGEOFromDatabase(XenonMeshPack::MeshBuilder* _pst_Database, GEO_tdst_Object* _pst_OldGeo, bool _b_IgnoreWeights);

static void XMP_BuildFileName(const CHAR* _psz_Path, 
                              const CHAR* _psz_BaseName, 
                              const CHAR* _psz_Suffix, 
                              CHAR*       _psz_FileName);

static MAT_tdst_MultiTexture* XMP_GetMultiTexture(MAT_tdst_Material* _pst_Material,
                                                  LONG               _l_MaterialId);

static UCHAR* XMP_ExtractHeightMap(XeMaterial* _pst_Material, LONG* _pl_Width, LONG* _pl_Height);
static UCHAR* XMP_ExtractHeightMapFromTEX(TEX_tdst_File_Desc* _pst_TexDesc, LONG* _pl_Width, LONG* _pl_Height);
static UCHAR* XMP_ExtractHeightMapD3DX(ULONG _ul_Key, LONG* _pl_Width, LONG* _pl_Height, bool _b_Flip);

static bool XMP_HasAlphaInFormat(D3DFORMAT _e_Format);

static void XMP_LogMessage(XenonMeshPack::LogLevel _e_LogLevel, CHAR* _sz_Msg);

static ULONG XMP_ComputeObjectCRC(OBJ_tdst_GameObject* _pst_GO);
static ULONG XMP_ComputeGeometryCRC(GEO_tdst_Object* _pst_Geo, ULONG* _pul_VertexColors, MAT_tdst_Material* _pst_Material);

static ULONG* XMP_LoadGameObjectDefaultRLI(OBJ_tdst_GameObject* _pst_GO, bool _b_AddAddress);

static void XMP_ProgressTick(void);

static bool XMP_IsXMPMesh(GRO_tdst_Struct* _pst_Gro);
static bool XMP_IsXEMMesh(GRO_tdst_Struct* _pst_Gro);

static bool  XMP_BuildRLIFileName(const CHAR* _psz_PrevRLIName, ULONG _ul_RLIKey, CHAR* _psz_NewName);
static ULONG XMP_ExtractRLIKey(const CHAR* _psz_RLIName);

static void XMP_FindNeighbors(XMPObjectInfoList*   _p_AllObjects, 
                              WOR_tdst_World*      _pst_World, 
                              OBJ_tdst_GameObject* _pst_GO, 
                              GRO_tdst_Visu*       _pst_Visu, 
                              GEO_tdst_Object*     _pst_Geo);

static void XMP_AddGEOToPointCloud(XenonMeshPack::PointCloud* _pst_PointCloud, 
                                   MATH_tdst_Matrix*          _pst_Matrix, 
                                   GEO_tdst_Object*           _pst_Geo);

static void XMP_UpdateGEOFromDatabase(XenonMeshPack::MeshBuilder* _pst_Database, 
                                      OBJ_tdst_GameObject*        _pst_GO, 
                                      GRO_tdst_Visu*              _pst_Visu, 
                                      GEO_tdst_Object*            _pst_Geo);

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

void XMP_ProcessGameObject(WOR_tdst_World* _pst_World, OBJ_tdst_GameObject* _pst_GO)
{
    DWORD dwStartTime = timeGetTime();

    if (_pst_GO)
    {
        CHAR sz_Msg[512];
        sprintf(sz_Msg, "XMP - Processing '%s'", _pst_GO->sz_Name ? _pst_GO->sz_Name : "<Invalid>");
        XMP_LogMessage(XenonMeshPack::LL_INFO, sz_Msg);
    }

    if (!GDI_b_IsXenonGraphics())
    {
        XMP_LogMessage(XenonMeshPack::LL_ERROR, "XMP - Mesh processing only available when using DirectX 9");
        return;
    }

    if ((_pst_World == NULL) || (_pst_GO == NULL))
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Invalid world or game object");
        return;
    }

    if (!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Game object must have a visu to be processed");
        return;
    }

    GRO_tdst_Visu* pst_Visu = _pst_GO->pst_Base->pst_Visu;
    if (pst_Visu->pst_Object == NULL)
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - No graphic object attached to the game object");
        return;
    }

    // Check if the object can or should be processed
    if ((pst_Visu->ul_XenonMeshProcessingFlags & XMP_PROCESSING_MASK) == 0)
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Game object is not marked for processing");
        return;
    }

    // Only supporting geometric objects
    GEO_tdst_Object* pst_Geo = NULL;
    if (pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
    {
        GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Visu->pst_Object;

        if ((pst_LOD->uc_NbLOD == 0) || (pst_LOD->uc_NbLOD >= 6))
        {
            XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Invalid LOD configuration");
            return;
        }

        for (ULONG i = 0; i < pst_LOD->uc_NbLOD; ++i)
        {
            if (pst_LOD->dpst_Id[i] && XMP_IsXMPMesh(pst_LOD->dpst_Id[i]))
            {
                XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Object can only be processed once");
                return;
            }
        }

        if ((pst_LOD->dpst_Id[0] == NULL) || (pst_LOD->dpst_Id[0]->i->ul_Type != GRO_Geometric))
        {
            XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Invalid LOD configuration");
            return;
        }

        pst_Geo = (GEO_tdst_Object*)pst_LOD->dpst_Id[0];
    }
    else if (pst_Visu->pst_Object->i->ul_Type == GRO_Geometric)
    {
        pst_Geo = (GEO_tdst_Object*)pst_Visu->pst_Object;
    }
    else
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Graphic object is not a geometric object");
        return;
    }

    // Do not process MRM objects
    if (pst_Geo->p_MRM_Levels != NULL)
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Unable to process multi-resolution meshes");
        return;
    }

    // Progress bar
    spo_UpdateDialog = new EDIA_cl_UPDATEDialog((CHAR*)"");
    sf_Progress      = 0.0f;
    spo_UpdateDialog->DoModeless();

    ULONG ul_CRC = XMP_ComputeObjectCRC(_pst_GO);

    XMP_ProcessGeometricObject(_pst_World, _pst_GO, pst_Visu, pst_Geo);

    // Save the CRC and reset consistency flag
    pst_Visu->ul_XenonMeshProcessingCRC    = ul_CRC;
	pst_Visu->ul_XenonMeshProcessingFlags &= ~GRO_XMPF_ConsistencyFailed;

    SAFE_DELETE(spo_UpdateDialog);

    ULONG ulPeakMemoryUsage = 0;
#if defined(_XMP_TRACK_MEMORY)
    // Make sure we freed all the memory we allocated
    ulPeakMemoryUsage = XenonMeshPack::XMP_DumpMemory();
#endif

    DWORD dwEndTime = timeGetTime();
    CHAR  szMsg[512];

    sprintf(szMsg, "XMP - Done (Time: %.2f seconds, Peak Memory Usage: %u bytes)", 
                   (FLOAT)(dwEndTime - dwStartTime) * 0.001f, ulPeakMemoryUsage);
    XMP_LogMessage(XenonMeshPack::LL_INFO, szMsg);
}

void XMP_RevertGameObject(WOR_tdst_World* _pst_World, OBJ_tdst_GameObject* _pst_GO, BOOL _b_PromptUser)
{
    if (_pst_GO)
    {
        CHAR sz_Msg[512];
        sprintf(sz_Msg, "XMP - Reverting '%s'", _pst_GO->sz_Name ? _pst_GO->sz_Name : "<Invalid>");
        XMP_LogMessage(XenonMeshPack::LL_INFO, sz_Msg);
    }

    if (!GDI_b_IsXenonGraphics())
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Mesh processing only available when using DirectX 9");
        return;
    }

    if ((_pst_World == NULL) || (_pst_GO == NULL))
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Invalid world or game object");
        return;
    }

    if (!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Game object must have a visu to be processed");
        return;
    }

    GRO_tdst_Visu* pst_Visu = _pst_GO->pst_Base->pst_Visu;
    if (pst_Visu->pst_Object == NULL)
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - No graphic object attached to the game object");
        return;
    }

    // We can only revert if the object was processed into a GeoStaticLOD
    if (pst_Visu->pst_Object->i->ul_Type != GRO_GeoStaticLOD)
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Graphic object is not a LOD object");
        return;
    }

    // Make sure we are really dealing with a StaticLOD created by our mesh processing
    GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Visu->pst_Object;
    if ((pst_LOD->st_Id.sz_Name == NULL) || (strstr(pst_LOD->st_Id.sz_Name, XMP_LOD_SUFFIX) == NULL))
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Graphic object was not created by Xenonify");
        return;
    }

    // Make sure we have at least 2 geometric objects in the GeoStaticLOD
    if ((pst_LOD->uc_NbLOD < 2) || 
        ((pst_LOD->dpst_Id[0] == NULL) || (pst_LOD->dpst_Id[0]->i->ul_Type != GRO_Geometric)) ||
        ((pst_LOD->dpst_Id[1] == NULL) || (pst_LOD->dpst_Id[1]->i->ul_Type != GRO_Geometric)))
    {
        XMP_LogMessage(XenonMeshPack::LL_ERROR, "XMP - Invalid LOD setup, unable to revert the changes");
        return;
    }

    if (!XMP_IsXMPMesh(pst_LOD->dpst_Id[0]))
    {
        XMP_LogMessage(XenonMeshPack::LL_ERROR, "XMP - Mesh on top of the LOD stack was not created by Xenonify");
        return;
    }

    GEO_tdst_Object* pst_MeshGeo = (GEO_tdst_Object*)pst_LOD->dpst_Id[0];
    GEO_tdst_Object* pst_OldGeo  = (GEO_tdst_Object*)pst_LOD->dpst_Id[1];

    // Make sure the first geometric object was created by us
    if ((pst_MeshGeo->st_Id.sz_Name == NULL) || (strstr(pst_MeshGeo->st_Id.sz_Name, XMP_GEO_SUFFIX) == NULL))
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Graphic object was not created by Xenonify");
        return;
    }

    // Make sure the RLI can be recovered...
    {
        ULONG* pul_RLI = XMP_LoadGameObjectDefaultRLI(_pst_GO, false);
        if (pul_RLI == NULL)
        {
            if (!_b_PromptUser)
            {
                CHAR szMsg[1024];
                sprintf(szMsg, "XMP - Unable to find the corresponding RLI for %s. " \
                               "Playstationize was NOT applied.", _pst_GO->sz_Name);
                ERR_X_Warning(0, szMsg, NULL);
                return;
            }

            CHAR sz_Msg[512];
            sprintf(sz_Msg, "Unable to find RLI associated with the object (%s), are you sure you want to Playstationize it anyway?",
                    _pst_GO->sz_Name);

            int iRes = ::MessageBox(NULL, sz_Msg, "XMP - Warning", MB_YESNO | MB_ICONWARNING | MB_APPLMODAL);
            if (iRes == IDNO)
                return;
        }

        if (pul_RLI != NULL)
        {
            MEM_Free(pul_RLI);
        }
    }

    // Clear the Xenon part of the meshes since we are going to change them
    GEO_ClearXenonMesh(pst_Visu, pst_MeshGeo);
    GEO_ClearXenonMesh(pst_Visu, pst_OldGeo);

    if (pst_Visu->dul_VertexColors)
    {
        // Remove the reference to the XMP RLI
        LOA_DeleteAddress(pst_Visu->dul_VertexColors);

        MEM_Free(pst_Visu->dul_VertexColors);
        pst_Visu->dul_VertexColors = NULL;
    }

    // Reload the original RLI if possible
    pst_Visu->dul_VertexColors = XMP_LoadGameObjectDefaultRLI(_pst_GO, true);

    // Remove the mesh geometric object from the graphics object table if necessary
    if (pst_MeshGeo->st_Id.l_Ref == 2)
    {
        LONG l_Index = TAB_ul_Ptable_GetElemIndexWithPointer(&_pst_World->st_GraphicObjectsTable, pst_MeshGeo);
        if (l_Index != (LONG)TAB_Cul_BadIndex)
        {
            TAB_Ptable_RemoveElemWithPointer(&_pst_World->st_GraphicObjectsTable, pst_MeshGeo);
            --pst_MeshGeo->st_Id.l_Ref;
            pst_MeshGeo->st_Id.i->pfn_Destroy(pst_MeshGeo);

            // Make sure we did not introduce any holes, that table doesn't like them
            TAB_Ptable_RemoveHoles(&_pst_World->st_GraphicObjectsTable);
        }
    }

    // Add the old geometric object from the graphic object table
    if (pst_LOD->uc_NbLOD == 2)
    {
        LONG l_Index = TAB_ul_Ptable_GetElemIndexWithPointer(&_pst_World->st_GraphicObjectsTable, pst_OldGeo);
        if (l_Index == (LONG)TAB_Cul_BadIndex)
        {
            TAB_Ptable_AddElemAndResize(&_pst_World->st_GraphicObjectsTable, pst_OldGeo);

            // Ref +1 -> Now referenced by the table
            ++pst_OldGeo->st_Id.l_Ref;
        }

        // Remove the LOD from the graphic object table if necessary
        if (pst_LOD->st_Id.l_Ref <= 2)
        {
            TAB_Ptable_RemoveElemWithPointer(&_pst_World->st_GraphicObjectsTable, pst_LOD);
            --pst_LOD->st_Id.l_Ref;
        }

        // Make sure we did not introduce any holes, that table doesn't like them
        TAB_Ptable_RemoveHoles(&_pst_World->st_GraphicObjectsTable);

        // Assign the old geometric object to the game object and remove any reference from the LOD
        pst_Visu->pst_Object = (GRO_tdst_Struct*)pst_OldGeo;
        pst_LOD->dpst_Id[1]  = NULL;
        pst_LOD->uc_NbLOD    = 1;

        // Ref -1 -> Not referenced in the GAO
        --pst_LOD->st_Id.l_Ref;
        pst_LOD->st_Id.i->pfn_Destroy(pst_LOD);

        // Allow the graphic object and RLI to be merged since we reverted to the orinal PS2 version
        _pst_GO->ul_XeMergeFlags &= ~(OBJ_C_Merge_KeepXe_KeyGRO | OBJ_C_Merge_KeepXe_KeyRLI);
    }
    else
    {
        // Update the LOD
        for (LONG i = 0; i < pst_LOD->uc_NbLOD - 1; ++i)
        {
            pst_LOD->dpst_Id[i] = pst_LOD->dpst_Id[i + 1];
        }
        --pst_LOD->uc_NbLOD;
    }

    // Invalidate the light maps
    LIGHT_Lightmaps_DestroyGAOLightmaps(_pst_GO);

    // Rebuild the Xenon mesh
    GEO_PackGameObject(_pst_GO);

    // Clear the CRC, key and reset the consistency flag
    pst_Visu->ul_XenonMeshProcessingCRC    = 0;
    pst_Visu->ul_XenonMeshProcessingRLIKey = BIG_C_InvalidKey;
	pst_Visu->ul_XenonMeshProcessingFlags &= ~GRO_XMPF_ConsistencyFailed;

    XMP_LogMessage(XenonMeshPack::LL_INFO, "XMP - Done");
}

void XMP_CheckConsistency(WOR_tdst_World* _pst_World, OBJ_tdst_GameObject* _pst_GO)
{
    _pst_World = _pst_World;
    _pst_GO    = _pst_GO;

    if (!GDI_b_IsXenonGraphics())
        return;

    if ((_pst_World == NULL) || (_pst_GO == NULL) || (_pst_GO->sz_Name == NULL))
        return;

    if (!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
        return;

    GRO_tdst_Visu* pst_Visu = _pst_GO->pst_Base->pst_Visu;
    if (pst_Visu->pst_Object == NULL)
        return;

	pst_Visu->ul_XenonMeshProcessingFlags &= ~GRO_XMPF_ConsistencyFailed;

    // We can only check if the object was processed into a GeoStaticLOD
    if (pst_Visu->pst_Object->i->ul_Type != GRO_GeoStaticLOD)
        return;

    // Make sure we are really dealing with a StaticLOD created by our mesh processing
    GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Visu->pst_Object;
    if ((pst_LOD->st_Id.sz_Name == NULL) || (strstr(pst_LOD->st_Id.sz_Name, XMP_LOD_SUFFIX) == NULL))
        return;

    if (pst_Visu->ul_XenonMeshProcessingCRC == 0)
        return;

    ULONG ul_CRC = XMP_ComputeObjectCRC(_pst_GO);

    if (ul_CRC != pst_Visu->ul_XenonMeshProcessingCRC)
    {
        // Warning message for the user and for the TestOnPC script
        CHAR szMsg[1024];
        sprintf(szMsg, "XMP - Original version of '%s' has changed. " \
                       "You should re-Xenonify and check-in the files. LOD key is [%08X]",
                       _pst_GO->sz_Name, LOA_ul_SearchKeyWithAddress((ULONG)pst_LOD));
        LINK_PrintStatusMsg(szMsg);

        XMP_LogMessage(XenonMeshPack::LL_DEBUG, "XMP - CheckConsistency() - Object has changed and must be reverted");

		pst_Visu->ul_XenonMeshProcessingFlags |= GRO_XMPF_ConsistencyFailed;
    }
    else
    {
        XMP_LogMessage(XenonMeshPack::LL_DEBUG, "XMP - CheckConsistency() - Object is still valid");
    }
}

void XMP_CreateEditableMesh(WOR_tdst_World* _pst_World, OBJ_tdst_GameObject* _pst_GO)
{
    CHAR sz_Message[512];
    CHAR sz_GroPath[512];
    CHAR sz_WorldPath[512];
    CHAR sz_BaseName[512];
    CHAR sz_GroName[512];
    CHAR sz_FileName[512];
    CHAR sz_RLIOldName[512];
    CHAR sz_RLIName[512];
    CHAR sz_RLIPath[512];

    L_memset(sz_GroPath,   0, BIG_C_MaxLenPath);
    L_memset(sz_BaseName,  0, BIG_C_MaxLenPath);
    L_memset(sz_GroName,   0, BIG_C_MaxLenPath);
    L_memset(sz_FileName,  0, BIG_C_MaxLenPath);
    L_memset(sz_WorldPath, 0, BIG_C_MaxLenPath);
    L_memset(sz_RLIPath,   0, BIG_C_MaxLenPath);

    if ((_pst_World == NULL) || (_pst_GO == NULL) || (_pst_GO->sz_Name == NULL))
        return;

    if (!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu) || (_pst_GO->pst_Base->pst_Visu == NULL))
        return;

    GRO_tdst_Visu* pst_Visu = _pst_GO->pst_Base->pst_Visu;
    if (pst_Visu->pst_Object == NULL)
        return;

    // Make sure we have a RLI file to work with
    ULONG ul_RLIKey   = BIG_C_InvalidKey;
    ULONG ul_RLIIndex = BIG_C_InvalidIndex;
    if (pst_Visu->dul_VertexColors != NULL)
    {
        ul_RLIKey   = LOA_ul_SearchKeyWithAddress((ULONG)pst_Visu->dul_VertexColors);
        ul_RLIIndex = BIG_ul_SearchKeyToFat(ul_RLIKey);
    }

    if ((ul_RLIKey == BIG_C_InvalidKey) || (ul_RLIIndex == BIG_C_InvalidIndex))
    {
        sprintf(sz_Message, "Unable to create Xenon Editable Mesh for %s, RLI is not present or can't be found", _pst_GO->sz_Name);
        XMP_LogMessage(XenonMeshPack::LL_ERROR, sz_Message);
        return;
    }

    strcpy(sz_RLIOldName, BIG_NameFile(ul_RLIIndex));
    if (!XMP_BuildRLIFileName(sz_RLIOldName, ul_RLIKey, sz_RLIName))
    {
        sprintf(sz_Message, "Unable to create Xenon Editable Mesh for %s, invalid RLI file name", _pst_GO->sz_Name);
        XMP_LogMessage(XenonMeshPack::LL_ERROR, sz_Message);
        return;
    }

    BIG_ComputeFullName(BIG_ParentFile(ul_RLIIndex), sz_RLIPath);

    // Get the path in which the new graphic object must be saved
    WOR_GetPath(_pst_World, sz_WorldPath);
    WOR_GetGroPath(_pst_World, sz_GroPath);

    if (pst_Visu->pst_Object->i->ul_Type == GRO_Geometric)
    {
        strcpy(sz_BaseName, GRO_sz_Struct_GetName(pst_Visu->pst_Object));
        sprintf(sz_GroName, "%s%s", sz_BaseName, XMP_LOD_SUFFIX);

        // Create the LOD and set the current Geo as the first and only level
        GEO_tdst_StaticLOD* pst_LOD = GEO_pst_StaticLOD_Create(1);
        pst_LOD->dpst_Id[0] = pst_Visu->pst_Object;
        pst_LOD->st_Id.i->pfn_AddRef(pst_LOD, 2);
        GRO_Struct_SetName(&pst_LOD->st_Id, sz_GroName);

        XMP_BuildFileName(sz_GroPath, sz_BaseName, XMP_LOD_SUFFIX, sz_FileName);

        // Save the LOD
        GRO_ul_Struct_FullSave(&pst_LOD->st_Id, sz_GroPath, sz_FileName, NULL);

        // Insert the LOD into the graphic object table
        TAB_Ptable_AddElemAndResize(&_pst_World->st_GraphicObjectsTable, pst_LOD);

        // Attach the LOD to the visu
        pst_Visu->pst_Object = (GRO_tdst_Struct*)pst_LOD;
    }

    if (pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
    {
        GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Visu->pst_Object;

        if (pst_LOD->uc_NbLOD >= 6)
        {
            sprintf(sz_Message, "Unable to create Xenon Editable Mesh for %s, LOD is full", _pst_GO->sz_Name);
            XMP_LogMessage(XenonMeshPack::LL_ERROR, sz_Message);
            return;
        }

        if ((pst_LOD->dpst_Id[0] == NULL) || (pst_LOD->dpst_Id[0]->i->ul_Type != GRO_Geometric))
        {
            sprintf(sz_Message, "Unable to create Xenon Editable Mesh for %s, invalid LOD configuration", _pst_GO->sz_Name);
            XMP_LogMessage(XenonMeshPack::LL_ERROR, sz_Message);
            return;
        }

        GEO_tdst_Object* pst_OldGeo = (GEO_tdst_Object*)pst_LOD->dpst_Id[0];

        if (XMP_IsXEMMesh(pst_LOD->dpst_Id[0]))
        {
            sprintf(sz_Message, "Unable to create Xenon Editable Mesh for %s, mesh is already editable", _pst_GO->sz_Name);
            XMP_LogMessage(XenonMeshPack::LL_WARNING, sz_Message);
            return;
        }

        // Clear the packing since we are going to change the first level
        GEO_CleanGameObjectPack(_pst_GO);

        // Unregister the RLI
        ULONG* pul_RLI = pst_Visu->dul_VertexColors;
        LOA_DeleteAddress(pst_Visu->dul_VertexColors);

        // Create the new RLI file
        pst_Visu->dul_VertexColors = (ULONG*)MEM_p_Alloc((*pul_RLI + 1) * sizeof(ULONG));
        L_memcpy(pst_Visu->dul_VertexColors, pul_RLI, (*pul_RLI + 1) * sizeof(ULONG));

        // Free the old RLI
        MEM_Free(pul_RLI);

        // Save the RLI file and add the address
        SAV_Begin(sz_RLIPath, sz_RLIName);
        ULONG ul_Temp = 0x80 | ('R' << 8) | ('L' << 16) | ('I' << 24);
        SAV_Buffer(&ul_Temp, 4 );
        SAV_Buffer(pst_Visu->dul_VertexColors, 4 * (pst_Visu->dul_VertexColors[0] + 1));
        ul_RLIIndex = SAV_ul_End();
        if (ul_RLIIndex != BIG_C_InvalidIndex)
        {
            LOA_AddAddress(ul_RLIIndex, _pst_GO->pst_Base->pst_Visu->dul_VertexColors);
        }

        // Find a name for the new graphic object
        strcpy(sz_BaseName, GRO_sz_Struct_GetName(&pst_OldGeo->st_Id));
        sprintf(sz_GroName, "%s%s", sz_BaseName, XMP_XEM_SUFFIX);

        // Duplicate the geometric object and add to the table
        GEO_tdst_Object* pst_NewGeo = (GEO_tdst_Object*)GEO_p_Duplicate(pst_OldGeo, NULL, NULL, 0);
        GRO_Struct_SetName(&pst_NewGeo->st_Id, sz_GroName);
        TAB_Ptable_AddElemAndResize(&_pst_World->st_GraphicObjectsTable, pst_NewGeo);
        pst_NewGeo->st_Id.i->pfn_AddRef(pst_NewGeo, 2);

        // Save the new graphic object
        XMP_BuildFileName(sz_GroPath, sz_BaseName, XMP_XEM_SUFFIX, sz_FileName);
        GRO_ul_Struct_FullSave(&pst_NewGeo->st_Id, sz_GroPath, sz_FileName, NULL);

        // Move all objects of the LOD down
        for (ULONG i = pst_LOD->uc_NbLOD; i > 0; --i)
        {
            pst_LOD->dpst_Id[i] = pst_LOD->dpst_Id[i - 1];
        }
        pst_LOD->dpst_Id[0] = (GRO_tdst_Struct*)pst_NewGeo;
        ++pst_LOD->uc_NbLOD;

        // Rebuild the Xenon mesh
        GEO_PackGameObject(_pst_GO);

        // Do not allow the object to be merged since we changed it
        _pst_GO->ul_XeMergeFlags |= (OBJ_C_Merge_KeepXe_KeyGRO | OBJ_C_Merge_KeepXe_KeyRLI);
    }
}

void XMP_RemoveEditableMesh(WOR_tdst_World* _pst_World, OBJ_tdst_GameObject* _pst_GO)
{
    CHAR sz_Message[512];
    CHAR sz_RLIName[512];

    if ((_pst_World == NULL) || (_pst_GO == NULL) || (_pst_GO->sz_Name == NULL))
        return;

    if (!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu) || (_pst_GO->pst_Base->pst_Visu == NULL))
        return;

    GRO_tdst_Visu* pst_Visu = _pst_GO->pst_Base->pst_Visu;
    if (pst_Visu->pst_Object == NULL)
        return;

    if (pst_Visu->dul_VertexColors == NULL)
        return;

    if (pst_Visu->pst_Object->i->ul_Type != GRO_GeoStaticLOD)
    {
        sprintf(sz_Message, "Unable to remove Xenon Editable Mesh for %s, not a LOD", _pst_GO->sz_Name);
        XMP_LogMessage(XenonMeshPack::LL_WARNING, sz_Message);
        return;
    }

    GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Visu->pst_Object;
    if (pst_LOD->uc_NbLOD < 2)
    {
        sprintf(sz_Message, "Unable to remove Xenon Editable Mesh for %s, invalid LOD setup", _pst_GO->sz_Name);
        XMP_LogMessage(XenonMeshPack::LL_WARNING, sz_Message);
        return;
    }

    if (pst_LOD->dpst_Id[0] == NULL)
    {
        sprintf(sz_Message, "Unable to remove Xenon Editable Mesh for %s, invalid LOD setup", _pst_GO->sz_Name);
        XMP_LogMessage(XenonMeshPack::LL_WARNING, sz_Message);
        return;
    }

    if (XMP_IsXEMMesh(pst_LOD->dpst_Id[0]))
    {
        ULONG ul_RLIIndex = LOA_ul_SearchIndexWithAddress((ULONG)pst_Visu->dul_VertexColors);
        if (ul_RLIIndex == BIG_C_InvalidIndex)
        {
            sprintf(sz_Message, "Unable to remove Xenon Editable Mesh for %s, RLI was not found", _pst_GO->sz_Name);
            XMP_LogMessage(XenonMeshPack::LL_WARNING, sz_Message);
            return;
        }

        strcpy(sz_RLIName, BIG_NameFile(ul_RLIIndex));
        ULONG ul_RLIKey = XMP_ExtractRLIKey(sz_RLIName);

        if (ul_RLIKey == BIG_C_InvalidKey)
        {
            sprintf(sz_Message, "Unable to remove Xenon Editable Mesh for %s, invalid RLI file name", _pst_GO->sz_Name);
            XMP_LogMessage(XenonMeshPack::LL_WARNING, sz_Message);
            return;
        }

        ul_RLIIndex = BIG_ul_SearchKeyToFat(ul_RLIKey);
        if (ul_RLIIndex == BIG_C_InvalidIndex)
        {
            sprintf(sz_Message, "Unable to remove Xenon Editable Mesh for %s, unable to find the original RLI file", _pst_GO->sz_Name);
            XMP_LogMessage(XenonMeshPack::LL_WARNING, sz_Message);
            return;
        }

        sprintf(sz_Message, "Are you sure you want to discard changes made to %s?", _pst_GO->sz_Name);
        int iRes = ::MessageBox(NULL, sz_Message, "XMP - Warning", MB_YESNO | MB_ICONWARNING | MB_APPLMODAL);
        if (iRes == IDNO)
            return;

        GEO_CleanGameObjectPack(_pst_GO);

        if (pst_LOD->uc_NbLOD == 2)
        {
            GRO_tdst_Struct* pst_NewGro = pst_LOD->dpst_Id[1];

            // GAO referencing that GRO now
            pst_NewGro->i->pfn_AddRef(pst_NewGro, 1);

            // GAO not referencing the LOD anymore
            pst_LOD->st_Id.i->pfn_AddRef(pst_LOD, -1);
            if (pst_LOD->st_Id.l_Ref == 1)
            {
                TAB_Ptable_RemoveElemWithPointer(&_pst_World->st_GraphicObjectsTable, pst_LOD);
                --pst_LOD->st_Id.l_Ref;
                pst_LOD->st_Id.i->pfn_Destroy(pst_LOD);

                TAB_Ptable_RemoveHoles(&_pst_World->st_GraphicObjectsTable);
            }

            pst_Visu->pst_Object = pst_NewGro;

            // Remove the merge flags since we reverted back to the original version
            _pst_GO->ul_XeMergeFlags &= ~(OBJ_C_Merge_KeepXe_KeyGRO | OBJ_C_Merge_KeepXe_KeyRLI);
        }
        else
        {
            GRO_tdst_Struct* pst_XEMGro = pst_LOD->dpst_Id[0];

            // Update the LOD
            for (LONG i = 0; i < pst_LOD->uc_NbLOD - 1; ++i)
            {
                pst_LOD->dpst_Id[i] = pst_LOD->dpst_Id[i + 1];
            }
            --pst_LOD->uc_NbLOD;

            // Not referenced by the LOD anymore
            pst_XEMGro->i->pfn_AddRef(pst_XEMGro, -1);

            // Only referenced by the Gro table?
            if (pst_XEMGro->l_Ref <= 1)
            {
                TAB_Ptable_RemoveElemWithPointer(&_pst_World->st_GraphicObjectsTable, pst_XEMGro);
                --pst_XEMGro->l_Ref;
                pst_XEMGro->i->pfn_Destroy(pst_XEMGro);

                TAB_Ptable_RemoveHoles(&_pst_World->st_GraphicObjectsTable);
            }
        }

        // Recover the RLI
        LOA_DeleteAddress(pst_Visu->dul_VertexColors);
        MEM_Free(pst_Visu->dul_VertexColors);
        pst_Visu->dul_VertexColors = (ULONG*)OBJ_ul_GameObjectRLICallback(BIG_PosFile(ul_RLIIndex));
        if (pst_Visu->dul_VertexColors)
        {
            LOA_AddAddress(ul_RLIIndex, pst_Visu->dul_VertexColors);
        }
    }
    else
    {
        sprintf(sz_Message, "Unable to remove Xenon Editable Mesh for %s, mesh is not editable", _pst_GO->sz_Name);
        XMP_LogMessage(XenonMeshPack::LL_WARNING, sz_Message);
    }
}

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION OF PRIVATE FUNCTIONS
// ------------------------------------------------------------------------------------------------

static void XMP_ProcessGeometricObject(WOR_tdst_World*      _pst_World, 
                                       OBJ_tdst_GameObject* _pst_GO, 
                                       GRO_tdst_Visu*       _pst_Visu, 
                                       GEO_tdst_Object*     _pst_Geo)
{
    XenonMeshPack::MeshBuilder oMeshBuilder;

    // Clear the Xenon part of the mesh since we are going to change it
    GEO_ClearXenonMesh(_pst_Visu, _pst_Geo);

    oMeshBuilder.SetOutputFunction(XMP_LogMessage);
    oMeshBuilder.SetProgressTickFunction(XMP_ProgressTick);

    // Enable safe mode processing for pathetic cases
    if (_pst_Visu->ul_XenonMeshProcessingFlags & GRO_XMPF_SafeMode)
    {
        oMeshBuilder.SetSafeMode(true);
    }

    XMP_LogMessage(XenonMeshPack::LL_INFO, "XMP - Preparing the mesh");

    // Remember the RLI key
    _pst_Visu->ul_XenonMeshProcessingRLIKey = LOA_ul_SearchKeyWithAddress((ULONG)_pst_Visu->dul_VertexColors);

    // Set the maximum vertex and face count since we must fit within the engine's limitations
    oMeshBuilder.SetMaximumVertexCount(SOFT_Cul_ComputingBufferSize);
    oMeshBuilder.SetMaximumFaceCount(65535 / 3);

    oMeshBuilder.BeginVertices();

    // Add the vertices and triangles to the mesh builder
    if (!XMP_AddGEOToDatabase(&oMeshBuilder, _pst_Visu, _pst_Geo, FALSE))
    {
        GEO_PackGameObject(_pst_GO);
        return;
    }

    oMeshBuilder.EndVertices();

    // Process the mesh

    XMP_LogMessage(XenonMeshPack::LL_INFO, "XMP - Chamfer");
    oMeshBuilder.Chamfer();

    XMP_LogMessage(XenonMeshPack::LL_INFO, "XMP - Tessellation");
    oMeshBuilder.Tessellate();

    XMP_LogMessage(XenonMeshPack::LL_INFO, "XMP - Displacement");
    oMeshBuilder.Displace();

    XMP_LogMessage(XenonMeshPack::LL_INFO, "XMP - Simplification");
    oMeshBuilder.Simplify(XMP_SIMPLIFICATION_THRESHOLD);

    // Build a geometric object with the new vertices and faces
    //  -> Vertex colors in the Visu will not be valid after this point since the number
    //     of vertices in the Geo has changed. We will need to move the color array from
    //     the Geo to the visu...
    XMP_LogMessage(XenonMeshPack::LL_INFO, "XMP - Building the final object");
    GEO_tdst_Object* pst_NewGeo = XMP_BuildGEOFromDatabase(&oMeshBuilder, _pst_Geo, (_pst_Visu->ul_XenonMeshProcessingFlags & GRO_XMPF_IgnoreWeights) != 0);
    if (pst_NewGeo == NULL)
        return;

    // Replace the current geometric object with a GeoStaticLOD
    XMP_AddGraphicObject(_pst_World, _pst_GO, _pst_Visu, pst_NewGeo);

    // Invalidate the light maps
    LIGHT_Lightmaps_DestroyGAOLightmaps(_pst_GO);

    // Create the Xenon mesh
    GEO_PackGameObject(_pst_GO);

    // Prevent the merge from changing the key of the .GRO and .RLI files of this game object
    // since we associated it with a LOD instead of a GEO.
    _pst_GO->ul_XeMergeFlags |= OBJ_C_Merge_KeepXe_KeyGRO | OBJ_C_Merge_KeepXe_KeyRLI;
}

static BOOL XMP_AddGEOToDatabase(XenonMeshPack::MeshBuilder* _pst_Database, 
                                 GRO_tdst_Visu*              _pst_Visu, 
                                 GEO_tdst_Object*            _pst_Geo,
                                 BOOL                        _b_SkipInformation)
{
    const ULONG MAX_WEIGHTS = 4;

    bool         ab_HasWeight[MAX_WEIGHTS] = { false, false, false, false };
    GEO_tdst_UV* pst_UVs    = NULL;
    ULONG*       pul_Colors = NULL;
    BOOL         b_WorkToDo = FALSE;

    if (_pst_Geo->l_NbElements <= 0)
    {
        XMP_LogMessage(XenonMeshPack::LL_ERROR, "XMP - Graphic object is empty, no processing will be done");
        return FALSE;
    }

    // We do not support skinned meshes
    if ((_pst_Geo->p_SKN_Objectponderation != NULL) && 
        ((_pst_Visu->ul_XenonMeshProcessingFlags & GRO_XMPF_IgnoreWeights) == 0))
    {
        if (_pst_Geo->p_SKN_Objectponderation->NumberPdrtLists > MAX_WEIGHTS)
        {
            XMP_LogMessage(XenonMeshPack::LL_ERROR, "XMP - Graphic object has too many weights, no processing will be done");
            return FALSE;
        }

        for (ULONG ul_MatrixCounter = 0; ul_MatrixCounter < _pst_Geo->p_SKN_Objectponderation->NumberPdrtLists; ++ul_MatrixCounter)
        {
            if (!MATH_b_EqMatrixWithEpsilon(&MATH_gst_IdentityMatrix, &_pst_Geo->p_SKN_Objectponderation->pp_PdrtLst[ul_MatrixCounter]->st_FlashedMatrix, 0.0001f))
            {
                XMP_LogMessage(XenonMeshPack::LL_ERROR, "XMP - Graphic object is skinned, no processing will be done");
                return FALSE;
            }
        }

        // Expand the skinning information - easier to work with
        GEO_SKN_Expand(_pst_Geo);

        for (ULONG i = 0; i < MAX_WEIGHTS; ++i)
        {
            ab_HasWeight[i] = (_pst_Geo->p_SKN_Objectponderation->NumberPdrtLists > i);
        }
    }

    // We at least need the points
    if (_pst_Geo->dst_Point == NULL)
    {
        XMP_LogMessage(XenonMeshPack::LL_ERROR, "XMP - Graphic object does not have any points, no processing will be done");
        return FALSE;
    }

    // Texture coordinates?
    if (_pst_Geo->dst_UV != NULL)
    {
        pst_UVs = _pst_Geo->dst_UV;
    }

    // Colors?
    if (_pst_Visu->dul_VertexColors != NULL)
    {
        pul_Colors = _pst_Visu->dul_VertexColors + 1;
    }
    else if (_pst_Geo->dul_PointColors != NULL)
    {
        pul_Colors = _pst_Geo->dul_PointColors + 1;
    }

    // Add all the elements
    for (LONG l_Element = 0; l_Element < _pst_Geo->l_NbElements; ++l_Element)
    {
        GEO_tdst_ElementIndexedTriangles* pst_Element = &_pst_Geo->dst_Element[l_Element];

        if (pst_Element->l_NbTriangles <= 0)
            continue;

        MAT_tdst_MultiTexture* pst_Material = XMP_GetMultiTexture((MAT_tdst_Material*)_pst_Visu->pst_Material, pst_Element->l_MaterialId);
        D3DXMATRIX stNMapTransform;
        UCHAR* pucHeightMap          = NULL;
        LONG   lHeightMapWidth       = 0;
        LONG   lHeightMapHeight      = 0;
        FLOAT  fDisplacementOffset   = 0.0f;
        FLOAT  fDisplacementMax      = 0.0f;
        FLOAT  fMaxTriangleArea      = 1.0f;
        FLOAT  fSmoothThreshold      = Cf_Pi;
        ULONG  ulProcessingFlags     = 0;
        FLOAT  fChamferThreshold     = -1.0f;
        FLOAT  fChamferLength        = 0.05f;
        FLOAT  fChamferWeldThreshold = 0.0005f;

        // Set the material information (identifier, map & transform)
        if (!_b_SkipInformation)
        {
            if ((pst_Material != NULL) && (pst_Material->pst_FirstLevel != NULL) &&
                (pst_Material->pst_FirstLevel->pst_XeMaterial != NULL))
            {
                XeMaterial*         pst_XeMaterial = pst_Material->pst_FirstLevel->pst_XeMaterial;
                MAT_tdst_XeMTLevel* pst_XeLevel    = pst_Material->pst_FirstLevel->pst_XeLevel;

                if (pst_XeLevel != NULL)
                {
                    // Fetch the height map
                    pucHeightMap = XMP_ExtractHeightMap(pst_XeMaterial, &lHeightMapWidth, &lHeightMapHeight);

                    // Displacement parameters
                    fDisplacementOffset = pst_XeLevel->f_DisplacementOffset;
                    fDisplacementMax    = pst_XeLevel->f_DisplacementHeight;

                    // Use the texture transform of the normal map
                    if (pst_XeLevel->ul_MeshProcessingFlags != 0)
                    {
                        pst_XeMaterial->GetTransform(XeMaterial::TEXTURE_NORMAL, &stNMapTransform);
                    }
                    else
                    {
                        D3DXMatrixIdentity(&stNMapTransform);
                    }

                    // Also make sure the element can be modified, check the object and material flags
                    if ((pst_XeLevel->ul_MeshProcessingFlags & GRO_XMPF_EnableTessellation) != 0)
                        ulProcessingFlags |= XenonMeshPack::PF_TESSELLATE;
                    if ((pst_XeLevel->ul_MeshProcessingFlags & GRO_XMPF_EnableDisplacement) != 0)
                        ulProcessingFlags |= XenonMeshPack::PF_DISPLACE;

                    // No displacement if there is no height map
                    if (pucHeightMap == NULL)
                    {
                        ulProcessingFlags &= ~XenonMeshPack::PF_DISPLACE;
                    }

                    // Tessellation area
                    fMaxTriangleArea = pst_XeLevel->f_TessellationArea;

                    // Auto smooth threshold
                    fSmoothThreshold = (Cf_Pi / 180.0f) * pst_XeLevel->f_SmoothThreshold;

                    // Chamfer
                    if ((pst_XeLevel->ul_MeshProcessingFlags & GRO_XMPF_EnableChamfer) != 0)
                        ulProcessingFlags |= XenonMeshPack::PF_CHAMFER;
                    fChamferThreshold     = fCos((Cf_Pi / 180.0f) * pst_XeLevel->f_ChamferThreshold);
                    fChamferLength        = pst_XeLevel->f_ChamferLength;
                    fChamferWeldThreshold = pst_XeLevel->f_ChamferWeldThreshold;
                }
            }
            else
            {
                D3DXMatrixIdentity(&stNMapTransform);
            }

            if ((_pst_Visu->ul_XenonMeshProcessingFlags & GRO_XMPF_EnableChamfer) == 0)
                ulProcessingFlags &= ~XenonMeshPack::PF_CHAMFER;
            if ((_pst_Visu->ul_XenonMeshProcessingFlags & GRO_XMPF_EnableTessellation) == 0)
                ulProcessingFlags &= ~XenonMeshPack::PF_TESSELLATE;
            if ((_pst_Visu->ul_XenonMeshProcessingFlags & GRO_XMPF_EnableDisplacement) == 0)
                ulProcessingFlags &= ~XenonMeshPack::PF_DISPLACE;

            b_WorkToDo |= (ulProcessingFlags != 0);
        }

        _pst_Database->SetMaterialInfo((BYTE)l_Element, pst_Element->l_MaterialId, ulProcessingFlags, 
                                        stNMapTransform, pucHeightMap, lHeightMapWidth, lHeightMapHeight, 
                                        fDisplacementOffset, fDisplacementMax, fMaxTriangleArea, fSmoothThreshold, 
                                        fChamferThreshold, fChamferLength, fChamferWeldThreshold);

        // Add all the triangles
        for (LONG l_Triangle = 0; l_Triangle < pst_Element->l_NbTriangles; ++l_Triangle)
        {
            GEO_tdst_IndexedTriangle* pst_Triangle = &pst_Element->dst_Triangle[l_Triangle];

            for (ULONG ul_Point = 0; ul_Point < 3; ++ul_Point)
            {
                USHORT uw_Index   = pst_Triangle->auw_Index[ul_Point];
                USHORT uw_UVIndex = pst_Triangle->auw_UV[ul_Point];

                _pst_Database->AddVertex(_pst_Geo->dst_Point[uw_Index].x, 
                                         _pst_Geo->dst_Point[uw_Index].y, 
                                         _pst_Geo->dst_Point[uw_Index].z, 
                                         ab_HasWeight[0] ? _pst_Geo->p_SKN_Objectponderation->pp_PdrtLst[0]->p_PdrtVrc_E[uw_Index].f_Ponderation : 0.0f, 
                                         ab_HasWeight[1] ? _pst_Geo->p_SKN_Objectponderation->pp_PdrtLst[1]->p_PdrtVrc_E[uw_Index].f_Ponderation : 0.0f, 
                                         ab_HasWeight[2] ? _pst_Geo->p_SKN_Objectponderation->pp_PdrtLst[2]->p_PdrtVrc_E[uw_Index].f_Ponderation : 0.0f, 
                                         ab_HasWeight[3] ? _pst_Geo->p_SKN_Objectponderation->pp_PdrtLst[3]->p_PdrtVrc_E[uw_Index].f_Ponderation : 0.0f, 
                                         pul_Colors ? pul_Colors[uw_Index]   : 0, 
                                         pst_UVs    ? pst_UVs[uw_UVIndex].fU : 0.0f, 
                                         pst_UVs    ? pst_UVs[uw_UVIndex].fV : 0.0f, 
                                         (BYTE)l_Element);
            }
        }
    }

    // Always succeed when we are skipping information
    if (_b_SkipInformation)
    {
        b_WorkToDo = TRUE;
    }

    if (!b_WorkToDo)
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Nothing to be done on that object, skipping it");
    }

    return b_WorkToDo;
}

static GEO_tdst_Object* XMP_BuildGEOFromDatabase(XenonMeshPack::MeshBuilder* _pst_Database, GEO_tdst_Object* _pst_OldGeo, bool _b_IgnoreWeights)
{
    const ULONG MAX_NUM_ELEMENTS = 256;

    XenonMeshPack::PackVertexArray   oVertices;
    XenonMeshPack::PackUVArray       oTexCoords;
    XenonMeshPack::PackTriangleArray oFaces;
    GEO_tdst_Object* pst_Geo;

    // Get the new mesh
    _pst_Database->GetPackedMesh(oVertices, oTexCoords, oFaces);
    ULONG ul_NbVertices  = oVertices.size();
    ULONG ul_NbTexCoords = oTexCoords.size();
    ULONG ul_NbFaces     = oFaces.size();
    if ((ul_NbVertices == 0) || (ul_NbTexCoords == 0) || (ul_NbFaces == 0))
    {
        XMP_LogMessage(XenonMeshPack::LL_ERROR, "XMP - No faces, vertices or texture coordinates...");
        return NULL;
    }

    // Find out the number of elements
    ULONG ul_NbElements = 1;
    for (ULONG ul_Face = 0; ul_Face < ul_NbFaces; ++ul_Face)
    {
        XenonMeshPack::PackTriangle* pst_Face = &oFaces[ul_Face];

        if (pst_Face->elementIndex >= ul_NbElements)
        {
            ul_NbElements = pst_Face->elementIndex + 1;
        }
    }

    // Create the geometric object and set the content
    pst_Geo = GEO_pst_Create(ul_NbVertices, ul_NbTexCoords, ul_NbElements, ul_NbVertices);
    if (pst_Geo == NULL)
    {
        XMP_LogMessage(XenonMeshPack::LL_ERROR, "XMP - Jade failed to create a geometric object");
        return NULL;
    }

    // Fill the points and colors arrays
    for (ULONG ul_Vertex = 0; ul_Vertex < ul_NbVertices; ++ul_Vertex)
    {
        XenonMeshPack::PackVertex* pst_Vertex = &oVertices[ul_Vertex];

        pst_Geo->dst_Point[ul_Vertex].x         = pst_Vertex->position.x;
        pst_Geo->dst_Point[ul_Vertex].y         = pst_Vertex->position.y;
        pst_Geo->dst_Point[ul_Vertex].z         = pst_Vertex->position.z;
        pst_Geo->dul_PointColors[ul_Vertex + 1] = pst_Vertex->color;
    }

    // Fill the texture coordinates array
    for (ULONG ul_TC = 0; ul_TC < ul_NbTexCoords; ++ul_TC)
    {
        XenonMeshPack::PackUV* pst_UV = &oTexCoords[ul_TC];

        pst_Geo->dst_UV[ul_TC].fU = pst_UV->texCoord.x;
        pst_Geo->dst_UV[ul_TC].fV = pst_UV->texCoord.y;
    }

    // Fill the elements
    for (ULONG ul_Element = 0; ul_Element < ul_NbElements; ++ul_Element)
    {
        GEO_tdst_ElementIndexedTriangles* pst_Element = &pst_Geo->dst_Element[ul_Element];

        // Find the number of triangles for that element
        pst_Element->l_NbTriangles = 0;
        for (ULONG ul_Face = 0; ul_Face < ul_NbFaces; ++ul_Face)
        {
            if (oFaces[ul_Face].elementIndex == ul_Element)
            {
                ++pst_Element->l_NbTriangles;
            }
        }

        pst_Element->l_MaterialId = _pst_Database->GetMaterialId((BYTE)ul_Element);
        GEO_AllocElementContent(pst_Element);

        // Fill the triangle list
        GEO_tdst_IndexedTriangle* pst_Triangle = pst_Element->dst_Triangle;
        for (ULONG ul_Face = 0; ul_Face < ul_NbFaces; ++ul_Face)
        {
            XenonMeshPack::PackTriangle* pst_Face = &oFaces[ul_Face];

            if (pst_Face->elementIndex == ul_Element)
            {
                pst_Triangle->auw_Index[0] = (USHORT)pst_Face->vertexIndices[0];
                pst_Triangle->auw_Index[1] = (USHORT)pst_Face->vertexIndices[1];
                pst_Triangle->auw_Index[2] = (USHORT)pst_Face->vertexIndices[2];

                pst_Triangle->auw_UV[0]    = (USHORT)pst_Face->uvIndices[0];
                pst_Triangle->auw_UV[1]    = (USHORT)pst_Face->uvIndices[1];
                pst_Triangle->auw_UV[2]    = (USHORT)pst_Face->uvIndices[2];
                ++pst_Triangle;
            }
        }
    }

    // Recover the skinning information
    if ((_pst_OldGeo->p_SKN_Objectponderation != NULL) && !_b_IgnoreWeights)
    {
        pst_Geo->p_SKN_Objectponderation = GEO_SKN_CreateObjPonderation(pst_Geo, _pst_OldGeo->p_SKN_Objectponderation->NumberPdrtLists, pst_Geo->l_NbPoints);

        GEO_SKN_Expand(pst_Geo);

        for (ULONG ul_Vertex = 0; ul_Vertex < ul_NbVertices; ++ul_Vertex)
        {
            XenonMeshPack::PackVertex* pst_Vertex = &oVertices[ul_Vertex];

            for (ULONG ul_Weight = 0; ul_Weight < pst_Geo->p_SKN_Objectponderation->NumberPdrtLists; ++ul_Weight)
            {
                pst_Geo->p_SKN_Objectponderation->pp_PdrtLst[ul_Weight]->p_PdrtVrc_E[ul_Vertex].f_Ponderation = pst_Vertex->weights[ul_Weight];
            }
        }
    }

    // Re-compute the normals
    GEO_ComputeNormals(pst_Geo);

    return pst_Geo;
}

static void XMP_AddGraphicObject(WOR_tdst_World*      _pst_World,
                                 OBJ_tdst_GameObject* _pst_GO,
                                 GRO_tdst_Visu*       _pst_Visu,
                                 GEO_tdst_Object*     _pst_NewGeo)
{
    GEO_tdst_Object*    pst_OldGeo = (GEO_tdst_Object*)_pst_Visu->pst_Object;
    GEO_tdst_StaticLOD* pst_LOD    = NULL;
    CHAR  sz_GroPath[BIG_C_MaxLenPath];
    CHAR  sz_WorldPath[BIG_C_MaxLenPath];
    CHAR  sz_BaseName[BIG_C_MaxLenPath];
    CHAR  sz_GeoName[BIG_C_MaxLenPath];
    CHAR  sz_FileName[BIG_C_MaxLenPath];

    L_memset(sz_GroPath,   0, BIG_C_MaxLenPath);
    L_memset(sz_BaseName,  0, BIG_C_MaxLenPath);
    L_memset(sz_GeoName,   0, BIG_C_MaxLenPath);
    L_memset(sz_FileName,  0, BIG_C_MaxLenPath);
    L_memset(sz_WorldPath, 0, BIG_C_MaxLenPath);

    // Get the path in which the new graphic object must be saved
    WOR_GetGroPath(_pst_World, sz_GroPath);

    // Build a name for the new object
    strcpy(sz_BaseName, GRO_sz_Struct_GetName((GRO_tdst_Struct*)pst_OldGeo));
    sprintf(sz_GeoName, "%s%s", sz_BaseName, XMP_GEO_SUFFIX);
    GRO_Struct_SetName(&_pst_NewGeo->st_Id, sz_GeoName);

    // Find a valid file name
    XMP_BuildFileName(sz_GroPath, sz_BaseName, XMP_GEO_SUFFIX, sz_FileName);

    // Set the RLI
    if (_pst_Visu->dul_VertexColors != NULL)
    {
        LOA_DeleteAddress(_pst_Visu->dul_VertexColors);

        MEM_Free(_pst_Visu->dul_VertexColors);
    }
    _pst_Visu->dul_VertexColors  = _pst_NewGeo->dul_PointColors;
    _pst_NewGeo->dul_PointColors = NULL;

    // Save the new geometric object
    GRO_ul_Struct_FullSave(&_pst_NewGeo->st_Id, sz_GroPath, sz_FileName, NULL);

    // Create a LOD mesh to hold both geometric objects
    if (_pst_Visu->pst_Object->i->ul_Type != GRO_GeoStaticLOD)
    {
        pst_LOD = GEO_pst_StaticLOD_Create(2);
        sprintf(sz_GeoName, "%s%s", sz_BaseName, XMP_LOD_SUFFIX);
        GRO_Struct_SetName(&pst_LOD->st_Id, sz_GeoName);

        // Always use level 0
        pst_LOD->auc_EndDistance[0] = 2;
        pst_LOD->auc_EndDistance[1] = 1;

        pst_LOD->st_Id.l_Ref     = 1; // One for the GAO
        pst_LOD->dpst_Id[0]      = (GRO_tdst_Struct*)_pst_NewGeo;
        _pst_NewGeo->st_Id.l_Ref = 1; // One for the GeoStaticLOD
        pst_LOD->dpst_Id[1]      = (GRO_tdst_Struct*)pst_OldGeo;

        // Add the LOD to the graphic objects table
        TAB_Ptable_AddElemAndResize(&_pst_World->st_GraphicObjectsTable, pst_LOD);
        ++pst_LOD->st_Id.l_Ref;

        // Add the new geometric object to the graphic objects table
        TAB_Ptable_AddElemAndResize(&_pst_World->st_GraphicObjectsTable, _pst_NewGeo);
        ++_pst_NewGeo->st_Id.l_Ref;

        // Find a file name to save the LOD
        XMP_BuildFileName(sz_GroPath, sz_BaseName, XMP_LOD_SUFFIX, sz_FileName);

        // Save the LOD in the BF
        GRO_ul_Struct_FullSave(&pst_LOD->st_Id, sz_GroPath, sz_FileName, NULL);
        (GRO_tdst_Struct*)_pst_Visu->pst_Object = (GRO_tdst_Struct*)pst_LOD;
    }
    else
    {
        pst_LOD = (GEO_tdst_StaticLOD*)_pst_Visu->pst_Object;

        // Move all objects of the LOD down
        for (ULONG i = pst_LOD->uc_NbLOD; i > 0; --i)
        {
            pst_LOD->dpst_Id[i] = pst_LOD->dpst_Id[i - 1];
        }
        pst_LOD->dpst_Id[0]      = (GRO_tdst_Struct*)_pst_NewGeo;
        _pst_NewGeo->st_Id.l_Ref = 1;
        ++pst_LOD->uc_NbLOD;

        // Add the new geometric object to the graphic objects table
        TAB_Ptable_AddElemAndResize(&_pst_World->st_GraphicObjectsTable, _pst_NewGeo);
        ++_pst_NewGeo->st_Id.l_Ref;
    }

    // Save the RLI file and associate the pointer so we can display it in the Gao information window
    {
        CHAR  sz_RLIName[BIG_C_MaxLenName + 8];
        CHAR* sz_Pos;

        // Set the right name and make sure it's not too long for the file system
        strcpy(sz_RLIName, _pst_GO->sz_Name);
        sz_Pos = strchr(sz_RLIName, '.');
        if (sz_Pos != NULL) *sz_Pos = '\0';
        strcat(sz_RLIName, "_XMP_LOD.");
        if (strlen(sz_RLIName) > (BIG_C_MaxLenName - 5))
        {
            sz_RLIName[BIG_C_MaxLenName - 5] = '.';
            sz_RLIName[BIG_C_MaxLenName - 4] = '\0';
        }

        WOR_GetPath(_pst_GO->pst_World, sz_WorldPath);

        ULONG ul_RLIKey   = OBJ_ul_GameObject_SaveRLI(_pst_GO->pst_Base->pst_Visu->dul_VertexColors, sz_WorldPath, sz_RLIName, FALSE);
        ULONG ul_RLIIndex = BIG_ul_SearchKeyToFat(ul_RLIKey);
        if (ul_RLIIndex != BIG_C_InvalidIndex)
        {
            LOA_AddAddress(ul_RLIIndex, _pst_GO->pst_Base->pst_Visu->dul_VertexColors);
        }
    }
}

static void XMP_BuildFileName(const CHAR* _psz_Path, 
                              const CHAR* _psz_BaseName, 
                              const CHAR* _psz_Suffix, 
                              CHAR*       _psz_FileName)
{
    CHAR  sz_BaseName[512];
    ULONG ul_EngineFile;
    ULONG ul_Counter;

    ULONG ul_SuffixExtSize = strlen(_psz_Suffix) + strlen(".gro");
    ULONG ul_BaseSize      = strlen(_psz_BaseName);

    // Make sure we'll never exceed the maximum length
    if (ul_BaseSize >= BIG_C_MaxLenName - 7 - ul_SuffixExtSize)
    {
        L_memset(sz_BaseName, 0, 512);
        strncpy(sz_BaseName, _psz_BaseName, BIG_C_MaxLenName - 7 - ul_SuffixExtSize);
    }
    else
    {
        strcpy(sz_BaseName, _psz_BaseName);
    }

    sprintf(_psz_FileName, "%s%s.gro", sz_BaseName, _psz_Suffix);
    ul_EngineFile = BIG_ul_SearchFileExt(_psz_Path, _psz_FileName);
    if (ul_EngineFile != BIG_C_InvalidIndex)
    {
        if (LOA_ul_SearchAddress(BIG_PosFile(ul_EngineFile)) == BIG_C_InvalidIndex)
        {
            ul_EngineFile = BIG_C_InvalidIndex;
        }
    }

    ul_Counter = 0;

    while (ul_EngineFile != BIG_C_InvalidIndex)
    {
        sprintf(_psz_FileName, "%s%s_%03d.gro", sz_BaseName, _psz_Suffix, ul_Counter++);

        ul_EngineFile = BIG_ul_SearchFileExt(_psz_Path, _psz_FileName);
        if (ul_EngineFile != BIG_C_InvalidIndex)
        {
            if (LOA_ul_SearchAddress(BIG_PosFile(ul_EngineFile)) == BIG_C_InvalidIndex)
            {
                ul_EngineFile = BIG_C_InvalidIndex;
            }
        }
    }
}

static MAT_tdst_MultiTexture* XMP_GetMultiTexture(MAT_tdst_Material* _pst_Material,
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

static UCHAR* XMP_ExtractHeightMap(XeMaterial* _pst_Material,  LONG* _pl_Width, LONG* _pl_Height)
{
    LONG l_NMapId = _pst_Material->GetTextureId(XeMaterial::TEXTURE_NORMAL);

    // No normal map == no displacement
    if (l_NMapId == MAT_Xe_InvalidTextureId)
        return NULL;

    ULONG ulKey = TEX_gst_GlobalList.dst_Texture[l_NMapId].ul_Key;
    TEX_tdst_File_Desc st_TextureDesc;
    L_memset(&st_TextureDesc, 0, sizeof(TEX_tdst_File_Desc));
    TEX_l_File_GetInfo(ulKey, &st_TextureDesc, TRUE);

    switch (st_TextureDesc.st_Params.uc_Type)
    {
        case TEX_FP_RawPalFile: return XMP_ExtractHeightMapFromTEX(&st_TextureDesc, _pl_Width, _pl_Height);
        case TEX_FP_TgaFile:    return XMP_ExtractHeightMapD3DX(ulKey, _pl_Width, _pl_Height, true);
        case TEX_FP_XeDDSFile:  return XMP_ExtractHeightMapD3DX(ulKey, _pl_Width, _pl_Height, false);
    }

    return NULL;
}

static UCHAR* XMP_ExtractHeightMapFromTEX(TEX_tdst_File_Desc* _pst_TexDesc, LONG* _pl_Width, LONG* _pl_Height)
{
    ULONG ul_Key = _pst_TexDesc->st_Tex.st_XeProperties.ul_OriginalTexture;

    // Use the original texture since we want the alpha channel and it may not be available in the
    // generated DDS
    if ((ul_Key == 0) || (ul_Key == BIG_C_InvalidKey))
        return NULL;

    TEX_tdst_File_Desc st_OriTexDesc;
    L_memset(&st_OriTexDesc, 0, sizeof(TEX_tdst_File_Desc));
    TEX_l_File_GetInfo(_pst_TexDesc->st_Tex.st_XeProperties.ul_OriginalTexture, &st_OriTexDesc, TRUE);

    if (st_OriTexDesc.st_Params.uc_Type != TEX_FP_TgaFile)
        return NULL;

    return XMP_ExtractHeightMapD3DX(ul_Key, _pl_Width, _pl_Height, true);
}

static UCHAR* XMP_ExtractHeightMapD3DX(ULONG _ul_Key, LONG* _pl_Width, LONG* _pl_Height, bool _b_Flip)
{
    IDirect3DTexture9* pst_Texture = NULL;
    D3DXIMAGE_INFO     stImageInfo;
    ULONG   ul_FileIndex  = BIG_ul_SearchKeyToFat(_ul_Key);
    ULONG   ul_FileLength = 0;
    CHAR*   pc_FileBuffer = NULL;
    CHAR*   pc_SrcBuffer  = NULL;
    UCHAR*  puc_HeightMap = NULL;
    HRESULT hr;

    if (ul_FileIndex == BIG_C_InvalidIndex)
        return NULL;

    pc_FileBuffer = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_FileIndex), &ul_FileLength);

    if ((pc_FileBuffer == NULL) || (ul_FileLength == 0))
        return NULL;

    ul_FileLength -= sizeof(TEX_tdst_File_Params);
    pc_SrcBuffer   = pc_FileBuffer;
    if (LOA_IsBinaryData())
        pc_SrcBuffer += sizeof(TEX_tdst_File_Params);

    hr = D3DXGetImageInfoFromFileInMemory(pc_SrcBuffer, ul_FileLength, &stImageInfo);
    if (FAILED(hr))
    {
        L_free(pc_FileBuffer);
        return FALSE;
    }

    // No alpha == no displacement
    if (!XMP_HasAlphaInFormat(stImageInfo.Format))
    {
        L_free(pc_FileBuffer);
        return FALSE;
    }

    hr = D3DXCreateTextureFromFileInMemoryEx(g_oXeRenderer.GetDevice(), pc_SrcBuffer, ul_FileLength, 
                                             D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8, 
                                             D3DPOOL_SCRATCH, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 
                                             0, NULL, NULL, &pst_Texture);

    L_free(pc_FileBuffer);

    if (FAILED(hr) || (pst_Texture == NULL))
        return FALSE;

    // Get the width and height of the texture
    D3DSURFACE_DESC st_LevelDesc;
    L_memset(&st_LevelDesc, 0, sizeof(D3DSURFACE_DESC));
    pst_Texture->GetLevelDesc(0, &st_LevelDesc);
    *_pl_Width  = st_LevelDesc.Width;
    *_pl_Height = st_LevelDesc.Height;

    // Create the heightmap using the alpha channel
    D3DLOCKED_RECT st_Lock;
    hr = pst_Texture->LockRect(0, &st_Lock, NULL, 0);
    if (SUCCEEDED(hr))
    {
        puc_HeightMap = (UCHAR*)XMP_ALLOC(st_LevelDesc.Width * st_LevelDesc.Height);

        ULONG* pul_SrcData = (ULONG*)st_Lock.pBits;
        UCHAR* puc_DestRow;

        for (ULONG j = 0; j < st_LevelDesc.Height; ++j)
        {
            if (_b_Flip)
            {
                puc_DestRow = &puc_HeightMap[(st_LevelDesc.Height - j - 1) * st_LevelDesc.Width];
            }
            else
            {
                puc_DestRow = &puc_HeightMap[j * st_LevelDesc.Width];
            }

            for (ULONG i = 0; i < st_LevelDesc.Width; ++i)
            {
                puc_DestRow[i] = (UCHAR)(pul_SrcData[i] >> 24);
            }

            pul_SrcData += (st_Lock.Pitch >> 2);
        }
    }

    SAFE_RELEASE(pst_Texture);

    return puc_HeightMap;
}

static bool XMP_HasAlphaInFormat(D3DFORMAT _e_Format)
{
    switch (_e_Format)
    {
        case D3DFMT_DXT3:
        case D3DFMT_DXT4:
        case D3DFMT_DXT5:
        case D3DFMT_A8R8G8B8:
        case D3DFMT_A4R4G4B4:
        case D3DFMT_A8:
            return true;
    }

    return false;
}

static void XMP_LogMessage(XenonMeshPack::LogLevel _e_LogLevel, CHAR* _sz_Msg)
{
    if (_e_LogLevel >= XenonMeshPack::LL_INFO)
    {
        if (_e_LogLevel == XenonMeshPack::LL_WARNING)
        {
            LINK_gul_ColorTxt = 0x000080ff;
        }
        else if (_e_LogLevel == XenonMeshPack::LL_ERROR)
        {
            LINK_gul_ColorTxt = 0x000000ff;
        }
        else
        {
            LINK_gul_ColorTxt = 0x00000000;
        }

        LINK_PrintStatusMsg(_sz_Msg);

        LINK_gul_ColorTxt = 0x00000000;
    }

    ERR_OutputDebugString("%s %s\n", XMP_LOG_INFO[_e_LogLevel], _sz_Msg);
}

static ULONG XMP_ComputeObjectCRC(OBJ_tdst_GameObject* _pst_GO)
{
    GRO_tdst_Visu*     pst_Visu     = _pst_GO->pst_Base->pst_Visu;
    GEO_tdst_Object*   pst_Geo      = NULL;
    MAT_tdst_Material* pst_Material = NULL;
    ULONG*             pul_Colors   = NULL;

    if ((pst_Visu->pst_Material == NULL) || (pst_Visu->pst_Object == NULL) || (pst_Visu->dul_VertexColors == NULL))
        return 0;

    if (pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
    {
        // LOD, first level is the PS2 version...

        CHAR                sz_Temp[512];
        GEO_tdst_StaticLOD* pst_LOD      = (GEO_tdst_StaticLOD*)pst_Visu->pst_Object;
        BIG_INDEX           ul_RLIIndex  = BIG_C_InvalidIndex;
        BIG_KEY             ul_RLIKey    = BIG_C_InvalidKey;

        if (pst_LOD->uc_NbLOD == 0)
            return 0;

        ul_RLIKey = LOA_ul_SearchKeyWithAddress((ULONG)pst_Visu->dul_VertexColors);
        if (ul_RLIKey != BIG_C_InvalidKey)
        {
            ul_RLIIndex = BIG_ul_SearchKeyToFat(ul_RLIKey);
        }

        for (ULONG i = 0; i < (ULONG)pst_LOD->uc_NbLOD - 1; ++i)
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

        if (ul_RLIIndex == BIG_C_InvalidIndex)
            return 0;

        if ((pst_LOD->dpst_Id[pst_LOD->uc_NbLOD - 1] == NULL) || (pst_LOD->dpst_Id[pst_LOD->uc_NbLOD - 1]->i->ul_Type != GRO_Geometric))
            return 0;

        pul_Colors = (ULONG*)OBJ_ul_GameObjectRLICallback(BIG_PosFile(ul_RLIIndex));

        ULONG ul_CRC = XMP_ComputeGeometryCRC((GEO_tdst_Object*)pst_LOD->dpst_Id[pst_LOD->uc_NbLOD - 1],
                                              pul_Colors, pst_Material);

        MEM_Free(pul_Colors);

        return ul_CRC;
    }
    else if (pst_Visu->pst_Object->i->ul_Type == GRO_Geometric)
    {
        // Only one mesh, so we'll assume this is the PS2 verion...

        pst_Geo = (GEO_tdst_Object*)pst_Visu->pst_Object;

        if (pst_Visu->dul_VertexColors != NULL)
            pul_Colors = pst_Visu->dul_VertexColors + 1;
        else
            pul_Colors = pst_Geo->dul_PointColors + 1;

        return XMP_ComputeGeometryCRC(pst_Geo, pst_Visu->dul_VertexColors, pst_Material);
    }

    return 0;
}

static ULONG XMP_ComputeGeometryCRC(GEO_tdst_Object* _pst_Geo, ULONG* _pul_VertexColors, MAT_tdst_Material* _pst_Material)
{
    std::vector<ULONG> aulCRCVector;

    // Add the vertex colors
    if (_pul_VertexColors != NULL)
    {
        ULONG ulNbColors = *_pul_VertexColors;

        if (ulNbColors > 0)
        {
            aulCRCVector.push_back(BAS_g_CRC32.Get_CRC(_pul_VertexColors, (ulNbColors + 1) * sizeof(ULONG)));
        }
    }

    // Add the number of points, uv, and elements
    aulCRCVector.push_back((ULONG)_pst_Geo->l_NbPoints);
    aulCRCVector.push_back((ULONG)_pst_Geo->l_NbElements);
    aulCRCVector.push_back((ULONG)_pst_Geo->l_NbUVs);

    // Add the points
    if ((_pst_Geo->l_NbPoints > 0) && (_pst_Geo->dst_Point != NULL))
    {
        aulCRCVector.push_back(BAS_g_CRC32.Get_CRC(_pst_Geo->dst_Point, _pst_Geo->l_NbPoints * sizeof(GEO_Vertex)));
    }

    // Add the point colors
    if (_pst_Geo->dul_PointColors != NULL)
    {
        ULONG ulNbColors = *_pst_Geo->dul_PointColors;

        if (ulNbColors > 0)
        {
            aulCRCVector.push_back(BAS_g_CRC32.Get_CRC(_pst_Geo->dul_PointColors, (ulNbColors + 1) * sizeof(ULONG)));
        }
    }

    // Skinning information
    if (_pst_Geo->p_SKN_Objectponderation != NULL)
    {
        GEO_SKN_Expand(_pst_Geo);

        aulCRCVector.push_back((ULONG)_pst_Geo->p_SKN_Objectponderation->NumberPdrtLists);

        // Weight lists
        for (ULONG ul_List = 0; ul_List < _pst_Geo->p_SKN_Objectponderation->NumberPdrtLists; ++ul_List)
        {
            aulCRCVector.push_back(BAS_g_CRC32.Get_CRC(_pst_Geo->p_SKN_Objectponderation->pp_PdrtLst[ul_List]->p_PdrtVrc_E, 
                                                       _pst_Geo->l_NbPoints * sizeof(GEO_tdst_ExpandedVertexPonderation)));
        }
    }

    // Add the texture coordinates
    if ((_pst_Geo->l_NbUVs > 0) && (_pst_Geo->dst_UV != NULL))
    {
        aulCRCVector.push_back(BAS_g_CRC32.Get_CRC(_pst_Geo->dst_UV, _pst_Geo->l_NbUVs * sizeof(GEO_tdst_UV)));
    }

    for (LONG l_Element = 0; l_Element < _pst_Geo->l_NbElements; ++l_Element)
    {
        GEO_tdst_ElementIndexedTriangles* pst_Element = &_pst_Geo->dst_Element[l_Element];

        // Number of triangles and material ID
        aulCRCVector.push_back((ULONG)pst_Element->l_NbTriangles);
        aulCRCVector.push_back((ULONG)pst_Element->l_MaterialId);

        // Transformation matrix of the normal map
        MAT_tdst_MultiTexture* pst_MT = XMP_GetMultiTexture(_pst_Material, pst_Element->l_MaterialId);
        if (pst_MT && pst_MT->pst_FirstLevel && pst_MT->pst_FirstLevel->pst_XeMaterial)
        {
            D3DXMATRIX mTransform;
            pst_MT->pst_FirstLevel->pst_XeMaterial->GetTransform(XeMaterial::TEXTURE_NORMAL, &mTransform);
            aulCRCVector.push_back(BAS_g_CRC32.Get_CRC(&mTransform, sizeof(D3DXMATRIX)));
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

    return BAS_g_CRC32.Get_CRC(&aulCRCVector[0], aulCRCVector.size() * sizeof(ULONG));
}

static ULONG* XMP_LoadGameObjectDefaultRLI(OBJ_tdst_GameObject* _pst_GO, bool _b_AddAddress)
{
    CHAR  sz_Path[512];
    CHAR  sz_FileName[512];
    CHAR* psz_Ext;

    // Load using the key if available
    if (OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu) && 
        (_pst_GO->pst_Base->pst_Visu->ul_XenonMeshProcessingRLIKey != 0) &&
        (_pst_GO->pst_Base->pst_Visu->ul_XenonMeshProcessingRLIKey != BIG_C_InvalidKey))
    {
        ULONG ul_FilePos = BIG_ul_SearchKeyToPos(_pst_GO->pst_Base->pst_Visu->ul_XenonMeshProcessingRLIKey);

        if (ul_FilePos != BIG_C_InvalidIndex)
        {
            ULONG* dul_Colors = (ULONG*)OBJ_ul_GameObjectRLICallback(ul_FilePos);

            if (_b_AddAddress)
                LOA_AddAddress(BIG_ul_SearchKeyToFat(_pst_GO->pst_Base->pst_Visu->ul_XenonMeshProcessingRLIKey), dul_Colors);

            return dul_Colors;
        }
    }

    if (_pst_GO->pst_World == NULL)
        return NULL;

    // Path
    WOR_GetGaoPath(_pst_GO->pst_World, sz_Path);
    strcat(sz_Path, " RLI");

    // RLI file name
    L_strcpy(sz_FileName, _pst_GO->sz_Name);
    psz_Ext = L_strrchr(sz_FileName, '.');
    if (psz_Ext) *psz_Ext = '\0';
    L_strcat(sz_FileName, ".rli");

    ULONG ul_FileIndex = BIG_ul_SearchFileExt(sz_Path, sz_FileName);
    if (ul_FileIndex == BIG_C_InvalidIndex)
        return NULL;

    ULONG* dul_VertexColors = (ULONG*)OBJ_ul_GameObjectRLICallback(BIG_PosFile(ul_FileIndex));

    if (_b_AddAddress)
        LOA_AddAddress(ul_FileIndex, dul_VertexColors);

    return dul_VertexColors;
}

static void XMP_ProgressTick(void)
{
  if (spo_UpdateDialog == NULL)
    return;

  spo_UpdateDialog->OnRefreshBar(sf_Progress);

  sf_Progress += 0.01f;
  if (sf_Progress >= 1.0f)
    sf_Progress = 0.0f;
}

static bool XMP_IsXMPMesh(GRO_tdst_Struct* _pst_Gro)
{
    CHAR* psz_Suffix = strstr(_pst_Gro->sz_Name, XMP_GEO_SUFFIX);
    ULONG ul_Len     = strlen(_pst_Gro->sz_Name);

    if (psz_Suffix == NULL)
        return false;

    while (strstr(psz_Suffix + 1, XMP_GEO_SUFFIX) != NULL)
    {
        psz_Suffix = strstr(psz_Suffix + 1, XMP_GEO_SUFFIX);
    }

    if ((psz_Suffix - _pst_Gro->sz_Name)  == (ul_Len - strlen(XMP_GEO_SUFFIX)))
        return true;

    // Make sure we catch the _XMP_xxx
    if ((psz_Suffix - _pst_Gro->sz_Name) == (ul_Len - strlen(XMP_GEO_SUFFIX) - 4))
    {
        if ((_pst_Gro->sz_Name[ul_Len - 4] == '_') &&
            isdigit(_pst_Gro->sz_Name[ul_Len - 3]) &&
            isdigit(_pst_Gro->sz_Name[ul_Len - 2]) &&
            isdigit(_pst_Gro->sz_Name[ul_Len - 1]))
            return true;
    }

    return false;
}

static bool XMP_IsXEMMesh(GRO_tdst_Struct* _pst_Gro)
{
    CHAR* psz_Suffix = strstr(_pst_Gro->sz_Name, XMP_XEM_SUFFIX);
    ULONG ul_Len     = strlen(_pst_Gro->sz_Name);

    if (psz_Suffix == NULL)
        return false;

    while (strstr(psz_Suffix + 1, XMP_XEM_SUFFIX) != NULL)
    {
        psz_Suffix = strstr(psz_Suffix + 1, XMP_XEM_SUFFIX);
    }

    if ((psz_Suffix - _pst_Gro->sz_Name)  == (ul_Len - strlen(XMP_XEM_SUFFIX)))
        return true;

    // Make sure we catch the _XEM_xxx
    if ((psz_Suffix - _pst_Gro->sz_Name) == (ul_Len - strlen(XMP_XEM_SUFFIX) - 4))
    {
        if ((_pst_Gro->sz_Name[ul_Len - 4] == '_') &&
            isdigit(_pst_Gro->sz_Name[ul_Len - 3]) &&
            isdigit(_pst_Gro->sz_Name[ul_Len - 2]) &&
            isdigit(_pst_Gro->sz_Name[ul_Len - 1]))
            return true;
    }

    return false;
}

static bool  XMP_BuildRLIFileName(const CHAR* _psz_PrevRLIName, ULONG _ul_RLIKey, CHAR* _psz_NewName)
{
    CHAR* psz_Ext;

    strcpy(_psz_NewName, _psz_PrevRLIName);

    psz_Ext = strrchr(_psz_NewName, '.');
    if (psz_Ext == NULL)
        return false;

    *psz_Ext = '\0';

    ULONG ul_Len = strlen(_psz_NewName);
    if ((ul_Len + 16) > BIG_C_MaxLenName)
    {
        psz_Ext  = &_psz_NewName[BIG_C_MaxLenName - 16];
        *psz_Ext = '\0';
    }

    sprintf(psz_Ext, "_0x%08x.rli", _ul_RLIKey);

    return true;
}

static ULONG XMP_ExtractRLIKey(const CHAR* _psz_RLIName)
{
    const CHAR* psz_Search = strstr(_psz_RLIName, "_0x");
    ULONG ul_Key     = BIG_C_InvalidKey;

    if (psz_Search == NULL)
        return BIG_C_InvalidKey;

    // Make sure we're at the last occurence
    while (strstr(psz_Search + 1, "_0x") != NULL)
    {
        psz_Search = strstr(psz_Search + 1, "_0x");
    }

    psz_Search += 3;

    if (sscanf(psz_Search, "%x", &ul_Key) != 1)
        return BIG_C_InvalidKey;

    return ul_Key;
}

void XMP_FixGameObjectEdges(WOR_tdst_World* _pst_World, OBJ_tdst_GameObject* _pst_GO)
{
    DWORD dwStartTime = timeGetTime();

    if (_pst_GO)
    {
        CHAR sz_Msg[512];
        sprintf(sz_Msg, "XMP - Removing T-Junctions of '%s'", _pst_GO->sz_Name ? _pst_GO->sz_Name : "<Invalid>");
        XMP_LogMessage(XenonMeshPack::LL_INFO, sz_Msg);
    }

    if (!GDI_b_IsXenonGraphics())
    {
        XMP_LogMessage(XenonMeshPack::LL_ERROR, "XMP - Mesh processing only available when using DirectX 9");
        return;
    }

    if ((_pst_World == NULL) || (_pst_GO == NULL))
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Invalid world or game object");
        return;
    }

    if (!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Game object must have a visu to be processed");
        return;
    }

    GRO_tdst_Visu* pst_Visu = _pst_GO->pst_Base->pst_Visu;
    if (pst_Visu->pst_Object == NULL)
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - No graphic object attached to the game object");
        return;
    }

    // Only applying fix on Xenon Editable Meshes
    if (pst_Visu->pst_Object->i->ul_Type != GRO_GeoStaticLOD)
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Graphic object is not a Xenon Editable Mesh");
        return;
    }

    // Only supporting geometric objects
    GEO_tdst_Object* pst_Geo = NULL;
    GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Visu->pst_Object;

    if (pst_LOD->uc_NbLOD == 0)
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Invalid LOD configuration");
        return;
    }

    if ((pst_LOD->dpst_Id[0] == NULL) || (pst_LOD->dpst_Id[0]->i->ul_Type != GRO_Geometric))
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Invalid LOD configuration");
        return;
    }

    if (!XMP_IsXEMMesh(pst_LOD->dpst_Id[0]))
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Edge fixing can only be applied to Xenon Editable Meshes");
        return;
    }

    pst_Geo = (GEO_tdst_Object*)pst_LOD->dpst_Id[0];

    // Do not process MRM objects
    if (pst_Geo->p_MRM_Levels != NULL)
    {
        XMP_LogMessage(XenonMeshPack::LL_WARNING, "XMP - Unable to process multi-resolution meshes");
        return;
    }

    // Progress bar
    spo_UpdateDialog = new EDIA_cl_UPDATEDialog((CHAR*)"");
    sf_Progress      = 0.0f;
    spo_UpdateDialog->DoModeless();

    {
        XMPObjectInfoList oNeighbors;

        // Find the neighbors
        XMP_FindNeighbors(&oNeighbors, _pst_World, _pst_GO, pst_Visu, pst_Geo);
        if (!oNeighbors.empty())
        {
            XenonMeshPack::MeshBuilder oMeshBuilder;
            XenonMeshPack::PointCloud  oPointCloud;

            // Clear the Xenon part of the mesh since we are going to change it
            GEO_ClearXenonMesh(pst_Visu, pst_Geo);

            oMeshBuilder.SetOutputFunction(XMP_LogMessage);
            oMeshBuilder.SetProgressTickFunction(XMP_ProgressTick);

            XMP_LogMessage(XenonMeshPack::LL_INFO, "XMP - Preparing the mesh");

            // Set the maximum vertex and face count since we must fit within the engine's limitations
            oMeshBuilder.SetMaximumVertexCount(SOFT_Cul_ComputingBufferSize);
            oMeshBuilder.SetMaximumFaceCount(65535 / 3);

            oMeshBuilder.BeginVertices();

            // Add the vertices and triangles to the mesh builder
            if (XMP_AddGEOToDatabase(&oMeshBuilder, pst_Visu, pst_Geo, TRUE))
            {
                oMeshBuilder.EndVertices();

                // Collect information about surrounding objects
                XMP_LogMessage(XenonMeshPack::LL_INFO, "XMP - Collecting edge information");
                XMPObjectInfoList::iterator itInfo = oNeighbors.begin();
                while (itInfo != oNeighbors.end())
                {
                    CHAR sz_Msg[256];
                    sprintf(sz_Msg, "XMP - Adding %s to the point cloud", 
                            itInfo->pst_GO->sz_Name ? itInfo->pst_GO->sz_Name : "<Unknown>");
                    XMP_LogMessage(XenonMeshPack::LL_DEBUG, sz_Msg);

                    MATH_tdst_Matrix st_InvGO;
                    MATH_tdst_Matrix st_Matrix;
                    MATH_InvertMatrix(&st_InvGO, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
                    MATH_MulMatrixMatrix(&st_Matrix, OBJ_pst_GetAbsoluteMatrix(itInfo->pst_GO), &st_InvGO);

                    XMP_AddGEOToPointCloud(&oPointCloud, &st_Matrix, itInfo->pst_Geo);

                    ++itInfo;
                }

                // Remove T-Junctions
                XMP_LogMessage(XenonMeshPack::LL_INFO, "XMP - Fixing T-junctions");
                oMeshBuilder.RemoveTJunctions(&oPointCloud);

                // Update the geometric object and colors
                XMP_LogMessage(XenonMeshPack::LL_INFO, "XMP - Updating the geometric object");
                XMP_UpdateGEOFromDatabase(&oMeshBuilder, _pst_GO, pst_Visu, pst_Geo);
            }

            // Create the Xenon mesh
            GEO_PackGameObject(_pst_GO);
        }
        else
        {
            XMP_LogMessage(XenonMeshPack::LL_INFO, "XMP - No neighbors found, unable to fix the edges");
        }
    }

    SAFE_DELETE(spo_UpdateDialog);

    ULONG ulPeakMemoryUsage = 0;
#if defined(_XMP_TRACK_MEMORY)
    // Make sure we freed all the memory we allocated
    ulPeakMemoryUsage = XenonMeshPack::XMP_DumpMemory();
#endif

    DWORD dwEndTime = timeGetTime();
    CHAR  szMsg[512];

    sprintf(szMsg, "XMP - T-Junction removal finished (Time: %.2f seconds, Peak Memory Usage: %u bytes)", 
                   (FLOAT)(dwEndTime - dwStartTime) * 0.001f, ulPeakMemoryUsage);
    XMP_LogMessage(XenonMeshPack::LL_INFO, szMsg);
}

static void XMP_FindNeighbors(XMPObjectInfoList*   _p_AllObjects, 
                              WOR_tdst_World*      _pst_World, 
                              OBJ_tdst_GameObject* _pst_GO, 
                              GRO_tdst_Visu*       _pst_Visu, 
                              GEO_tdst_Object*     _pst_Geo)
{
    TAB_tdst_PFelem* pst_Elem;
    TAB_tdst_PFelem* pst_LastElem;
    MATH_tdst_Vector st_ObjMin;
    MATH_tdst_Vector st_ObjMax;

    _p_AllObjects->clear();

    // Only consider boxes
    if (!OBJ_BV_IsAABBox(_pst_GO->pst_BV))
        return;

    // Bounding volume of the main game object
    MATH_AddVector(&st_ObjMax, OBJ_pst_BV_GetGMax(_pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));
    MATH_AddVector(&st_ObjMin, OBJ_pst_BV_GetGMin(_pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));

    // Make the box just a little bigger to make sure we don't discard a valid neighbor
    st_ObjMax.x += 0.01f; st_ObjMax.y += 0.01f; st_ObjMax.z += 0.01f;
    st_ObjMin.x -= 0.01f; st_ObjMin.y -= 0.01f; st_ObjMin.z -= 0.01f;

    pst_Elem     = _pst_World->st_AllWorldObjects.p_Table;
    pst_LastElem = _pst_World->st_AllWorldObjects.p_NextElem;
    for(; pst_Elem < pst_LastElem; pst_Elem++)
    {
        if (TAB_b_IsAHole(pst_Elem->p_Pointer))
            continue;

        OBJ_tdst_GameObject* pst_GO = (OBJ_tdst_GameObject*)pst_Elem->p_Pointer;

        if (pst_GO == _pst_GO)
            continue;

        if (!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
            continue;

        if (!pst_GO || !pst_GO->pst_Base || !pst_GO->pst_Base->pst_Visu)
            continue;

        GRO_tdst_Visu*   pst_Visu = pst_GO->pst_Base->pst_Visu;
        GEO_tdst_Object* pst_Geo  = NULL;

        if (pst_Visu->pst_Object == NULL)
            continue;

        if (pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
        {
            GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)pst_Visu->pst_Object;

            if ((pst_LOD->uc_NbLOD > 0)       && 
                (pst_LOD->dpst_Id[0] != NULL) && 
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
            continue;

        if (!OBJ_BV_IsAABBox(pst_GO))
            continue;

        MATH_tdst_Vector st_Min;
        MATH_tdst_Vector st_Max;

        MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(pst_GO));
        MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(pst_GO));

        if (!INT_AABBoxAABBox(&st_ObjMin, &st_ObjMax, &st_Min, &st_Max))
            continue;

        XMPObjectInfo oInfo;

        oInfo.pst_GO   = pst_GO;
        oInfo.pst_Visu = pst_Visu;
        oInfo.pst_Geo  = pst_Geo;

        _p_AllObjects->push_back(oInfo);
    }
}

static void XMP_AddGEOToPointCloud(XenonMeshPack::PointCloud* _pst_PointCloud, 
                                   MATH_tdst_Matrix*          _pst_Matrix, 
                                   GEO_tdst_Object*           _pst_Geo)
{
    LONG i;

    if (_pst_Geo->dst_Point == NULL)
        return;

    // Add the vertices
    for (i = 0; i < _pst_Geo->l_NbPoints; ++i)
    {
        MATH_tdst_Vector st_Pos;

        MATH_TransformVertex(&st_Pos, _pst_Matrix, &_pst_Geo->dst_Point[i]);

        _pst_PointCloud->AddVertex(st_Pos.x, st_Pos.y, st_Pos.z);
    }
}

static void XMP_UpdateGEOFromDatabase(XenonMeshPack::MeshBuilder* _pst_Database, 
                                      OBJ_tdst_GameObject*        _pst_GO, 
                                      GRO_tdst_Visu*              _pst_Visu, 
                                      GEO_tdst_Object*            _pst_Geo)
{
    XenonMeshPack::PackVertexArray   oVertices;
    XenonMeshPack::PackUVArray       oTexCoords;
    XenonMeshPack::PackTriangleArray oFaces;

    // Get the new mesh
    _pst_Database->GetPackedMesh(oVertices, oTexCoords, oFaces);
    ULONG ul_NbVertices  = oVertices.size();
    ULONG ul_NbTexCoords = oTexCoords.size();
    ULONG ul_NbFaces     = oFaces.size();
    if ((ul_NbVertices == 0) || (ul_NbTexCoords == 0) || (ul_NbFaces == 0))
    {
        XMP_LogMessage(XenonMeshPack::LL_ERROR, "XMP - No faces, vertices or texture coordinates...");
        return;
    }

    CHAR  sz_Name[256];
    ULONG ul_NbElements = _pst_Geo->l_NbElements;
    BOOL  b_Skin        = (_pst_Geo->p_SKN_Objectponderation != NULL);
    ULONG ul_NbMatrices = b_Skin ? _pst_Geo->p_SKN_Objectponderation->NumberPdrtLists : 0;

    strcpy(sz_Name, GRO_sz_Struct_GetName(&_pst_Geo->st_Id));

    // Reset the geometric object with the new number of points and texture coordinates
    GEO_FreeContent(_pst_Geo);
    _pst_Geo->l_NbElements = ul_NbElements;
    _pst_Geo->l_NbPoints   = ul_NbVertices;
    _pst_Geo->l_NbUVs      = ul_NbTexCoords;
    GEO_AllocContent(_pst_Geo);

    GRO_Struct_SetName(&_pst_Geo->st_Id, sz_Name);

    // Reset the color count in the visu
    OBJ_VertexColor_Realloc(_pst_GO, ul_NbVertices);

    // Fill the points and colors arrays
    for (ULONG ul_Vertex = 0; ul_Vertex < ul_NbVertices; ++ul_Vertex)
    {
        XenonMeshPack::PackVertex* pst_Vertex = &oVertices[ul_Vertex];

        _pst_Geo->dst_Point[ul_Vertex].x           = pst_Vertex->position.x;
        _pst_Geo->dst_Point[ul_Vertex].y           = pst_Vertex->position.y;
        _pst_Geo->dst_Point[ul_Vertex].z           = pst_Vertex->position.z;
        _pst_Visu->dul_VertexColors[ul_Vertex + 1] = pst_Vertex->color;
    }

    // Fill the texture coordinates array
    for (ULONG ul_TC = 0; ul_TC < ul_NbTexCoords; ++ul_TC)
    {
        XenonMeshPack::PackUV* pst_UV = &oTexCoords[ul_TC];

        _pst_Geo->dst_UV[ul_TC].fU = pst_UV->texCoord.x;
        _pst_Geo->dst_UV[ul_TC].fV = pst_UV->texCoord.y;
    }

    // Fill the elements
    for (ULONG ul_Element = 0; ul_Element < ul_NbElements; ++ul_Element)
    {
        GEO_tdst_ElementIndexedTriangles* pst_Element = &_pst_Geo->dst_Element[ul_Element];

        // Find the number of triangles for that element
        pst_Element->l_NbTriangles = 0;
        for (ULONG ul_Face = 0; ul_Face < ul_NbFaces; ++ul_Face)
        {
            if (oFaces[ul_Face].elementIndex == ul_Element)
            {
                ++pst_Element->l_NbTriangles;
            }
        }

        pst_Element->l_MaterialId = _pst_Database->GetMaterialId((BYTE)ul_Element);
        GEO_AllocElementContent(pst_Element);

        // Fill the triangle list
        GEO_tdst_IndexedTriangle* pst_Triangle = pst_Element->dst_Triangle;
        for (ULONG ul_Face = 0; ul_Face < ul_NbFaces; ++ul_Face)
        {
            XenonMeshPack::PackTriangle* pst_Face = &oFaces[ul_Face];

            if (pst_Face->elementIndex == ul_Element)
            {
                pst_Triangle->auw_Index[0] = (USHORT)pst_Face->vertexIndices[0];
                pst_Triangle->auw_Index[1] = (USHORT)pst_Face->vertexIndices[1];
                pst_Triangle->auw_Index[2] = (USHORT)pst_Face->vertexIndices[2];

                pst_Triangle->auw_UV[0]    = (USHORT)pst_Face->uvIndices[0];
                pst_Triangle->auw_UV[1]    = (USHORT)pst_Face->uvIndices[1];
                pst_Triangle->auw_UV[2]    = (USHORT)pst_Face->uvIndices[2];
                ++pst_Triangle;
            }
        }
    }

    // Recover the skinning information
    if (b_Skin)
    {
        _pst_Geo->p_SKN_Objectponderation = GEO_SKN_CreateObjPonderation(_pst_Geo, ul_NbMatrices, _pst_Geo->l_NbPoints);

        GEO_SKN_Expand(_pst_Geo);

        for (ULONG ul_Vertex = 0; ul_Vertex < ul_NbVertices; ++ul_Vertex)
        {
            XenonMeshPack::PackVertex* pst_Vertex = &oVertices[ul_Vertex];

            for (ULONG ul_Weight = 0; ul_Weight < _pst_Geo->p_SKN_Objectponderation->NumberPdrtLists; ++ul_Weight)
            {
                _pst_Geo->p_SKN_Objectponderation->pp_PdrtLst[ul_Weight]->p_PdrtVrc_E[ul_Vertex].f_Ponderation = pst_Vertex->weights[ul_Weight];
            }
        }
    }

    // Re-compute the normals
    GEO_ComputeNormals(_pst_Geo);
}

#endif // ACTIVE_EDITORS && _XENON_RENDER
