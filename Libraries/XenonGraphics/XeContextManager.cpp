// ------------------------------------------------------------------------------------------------
// File   : XeContextManager.cpp
// Date   : 2005-09-13
// Author : Dany Joannette
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "Precomp.h"

#include "XeVertexShaderManager.h"
#include "XeContextManager.h"
#include "XeRenderObject.h"

// ------------------------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------------------------
XeContextManager g_oXeContextManager1;

XeContextManager *g_pXeContextManagerEngine = &g_oXeContextManager1;

#ifdef _XENON
XeContextManager g_oXeContextManager2;
XeContextManager *g_pXeContextManagerRender = &g_oXeContextManager2;
#else
XeContextManager *g_pXeContextManagerRender = &g_oXeContextManager1; // fake multicontext
#endif

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - XeContextManager
// ------------------------------------------------------------------------------------------------

XeContextManager::XeContextManager(void) : 
m_vecSkinningStack      ( VS_INITIAL_SKINNING_MATRIX_STACK_SIZE ),
m_vecWorldStack         ( VS_INITIAL_MATRIX_STACK_SIZE ),
m_vecWorldViewStack     ( VS_INITIAL_MATRIX_STACK_SIZE ),
m_vecProjectionStack    ( VS_INITIAL_MATRIX_STACK_SIZE ),
m_vecPlanarGizmoStack   ( VS_INITIAL_MATRIX_STACK_SIZE ),
m_vecDirLightStack      ( VS_INITIAL_LIGHT_STACK_SIZE ),
m_vecOmniLightStack     ( VS_INITIAL_LIGHT_STACK_SIZE ),
m_vecSpotLightStack     ( VS_INITIAL_LIGHT_STACK_SIZE ),
m_vecCylSpotLightStack  ( VS_INITIAL_LIGHT_STACK_SIZE ),
m_vecWYBParamsStack     ( VS_MAX_WYB_STACK_SIZE ),
m_vecSymmetryParamsStack( VS_MAX_SYMMETRY_STACK_SIZE )
{
    m_arRenderObjects = (XeRenderObject*)malloc(XERENDER_RENDEROBJECTPOOLSIZE*sizeof(XeRenderObject));

    m_apoDynamicMeshPool = NULL;
    m_ulCurrentDynMesh = 0;
    m_bDoublePassZOverwrite = FALSE;

    Xe_SPG2_ulNumberOfSpheres = 0;
}

XeContextManager::~XeContextManager(void)
{
#if !defined(XML_CONV_TOOL)
    ClearMatrixStacks();
#endif

    free(m_arRenderObjects);
    m_arRenderObjects = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::ReInit()
{
    D3DXMATRIX          oIdentity;
    D3DXMatrixIdentity(&oIdentity);

    // world
    m_vecWorldStack.Clear();
    PushWorldMatrix(&oIdentity);
    m_ulCurrentWorldIndex = 1;

    // world view
    m_vecWorldViewStack.Clear();
    PushWorldViewMatrix(&oIdentity);
    m_ulCurrentWorldViewIndex = 1;

    // projection
    m_vecProjectionStack.Clear();
    PushProjectionMatrix(&oIdentity);
    m_ulCurrentProjIndex = 1;

    // gizmo
    m_vecPlanarGizmoStack.Clear();
    PushPlanarGizmoMatrix(&oIdentity);
    m_ulCurrentPlanarGizmoIndex = 1;

    // skinning
    m_vecSkinningStack.Clear();

    // lighting
    m_vecDirLightStack.Clear();
    m_vecOmniLightStack.Clear();
    m_vecSpotLightStack.Clear();
    m_vecCylSpotLightStack.Clear();

    for (INT iLight = 0; iLight < VS_MAX_LIGHTS_PER_PASS; iLight++)
        m_aiPerPixelLightIndex[iLight] = 0;

    m_ulCurrentLightSet = m_ulLastActiveLightSet = 0;
    m_ulNbLightSets     = 1;
    for (ULONG ulLightSet = 0; ulLightSet < VS_MAX_LIGHTSET_STACK_SIZE; ++ulLightSet)
    {
        m_aoLightSets[ulLightSet].Clear();
    }

    // fur
    m_fFurOffsetScale = 1.0f;

    // render object
    m_pCurrentRenderObject = NULL;

    // dynamic meshes
    if(!m_apoDynamicMeshPool)
    {
        m_apoDynamicMeshPool = new XeMesh[MAX_DYNAMIC_MESH];

        // Create a dynamic vertex buffer for the dynamic meshes
        for(UINT uiMeshIdx = 0; uiMeshIdx < MAX_DYNAMIC_MESH; uiMeshIdx++)
        {
            XeBuffer* pBuffer = (XeBuffer*)g_XeBufferMgr.CreateDynVertexBuffer(FALSE);
            m_apoDynamicMeshPool[uiMeshIdx].AddStream(XEVC_UNDEFINED, pBuffer);
            m_apoDynamicMeshPool[uiMeshIdx].SetDynamic(TRUE);
        }
    }
    m_ulCurrentDynMesh = 0;

    // UI Rects
    m_avUIRectanglePool.reserve(XERENDER_MIN_UI_RECTANGLE);
    m_avUIRectanglePool.resize(0);

    // misc
    m_oDirtyFlags.DirtyAll();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::Shutdown()
{
    if (m_apoDynamicMeshPool != NULL)
    {
        delete [] m_apoDynamicMeshPool;
        m_apoDynamicMeshPool = NULL;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeContextManager::GetLight
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeLight* XeContextManager::GetLight(ULONG _ulLightSet, INT _iIndex, ULONG *_pulType)
{
    ERR_X_Assert(_ulLightSet < m_ulNbLightSets);
    ERR_X_Assert(_iIndex < VS_MAX_LIGHTS);

    XeLightSet * pLightSet = &m_aoLightSets[_ulLightSet];

    if (_iIndex >= 0)
    {
        if (_iIndex < (INT) pLightSet->ulDirLightCount)
        {
            // the light at this index is a direction light
            *_pulType = LIGHT_TYPE_DIRECT;
            return m_vecDirLightStack.At( pLightSet->ulDirLightIndices[_iIndex] );
        }
        _iIndex -= pLightSet->ulDirLightCount;

        if (_iIndex < (INT) pLightSet->ulOmniLightCount)
        {
            // the light at this index is an omni light
            XeLight *pLight = m_vecOmniLightStack.At( pLightSet->ulOmniLightIndices[_iIndex] );
            if(pLight->IsInverted)
                *_pulType = LIGHT_TYPE_OMNI_INVERTED;
            else
                *_pulType = LIGHT_TYPE_OMNI;
            return pLight;
        }
        _iIndex -= pLightSet->ulOmniLightCount;

        if (_iIndex < (INT)pLightSet->ulSpotLightCount)
        {
            // the light at this index is a spot light
            *_pulType = LIGHT_TYPE_SPOT;
            return m_vecSpotLightStack.At( pLightSet->ulSpotLightIndices[_iIndex] );
        }
        _iIndex -= pLightSet->ulSpotLightCount;

        if (_iIndex < (INT) pLightSet->ulCylSpotLightCount)
        {
            // the light at this index is a spot light
            *_pulType = LIGHT_TYPE_CYLINDRICAL_SPOT;
            return m_vecCylSpotLightStack.At( pLightSet->ulCylSpotLightIndices[_iIndex] );
        }
    }

    *_pulType = LIGHT_TYPE_NONE;
    return NULL;
}

XeRenderObject* XeContextManager::PushRenderObject(eXeRENDERLISTTYPE _list)
{
    XeRenderObject* pRO = &m_arRenderObjects[m_nbRenderObjects];
    m_nbRenderObjects++;
    AddRenderObjectToList(_list, pRO);
    return pRO;
}

void XeContextManager::AddRenderObjectToList(eXeRENDERLISTTYPE _list, XeRenderObject* _pRO)
{
    m_apRenderObjectLists[_list][m_nbRenderObjectsList[_list]] = _pRO;
    m_nbRenderObjectsList[_list]++;
}

XeMesh* XeContextManager::PushDynMesh()
{
    return &m_apoDynamicMeshPool[m_ulCurrentDynMesh++];
}
