#ifndef SHADERMANAGER_HEADER
#define SHADERMANAGER_HEADER

#include "XeUtils.h"
#include <vector>

template<class ShaderInterface, ULONG FeatureCount, ULONG LogConstCount, ULONG PhyConstCount>
class XeShaderManager
{
public:
    union DirtyFlags {
        struct {
            DWORD bPredicateConst   : 1;
            DWORD bMappingConst     : 1;
            DWORD bUserConst        : 1;
            DWORD uReserved         : 29;
        };
        DWORD dwRawFlags;

        void Clear() { dwRawFlags = 0; }
        void DirtyAll() { dwRawFlags = 0xFFFFFFFF; }
    };

    XeShaderManager() {};
    ~XeShaderManager() {};
    virtual void ReInit() {};

    // shader management
    virtual BOOL LoadShader(const CHAR* _szShaderName, ULONG _Type) { return FALSE; };   // Load a shader from a text buffer, file, etc.
    virtual BOOL ReloadShaders(void) { return FALSE; };       // Forces a reload of the shaders (useful when modifying shaders at runtime)
    void         UnloadAll(void);           // Deallocates all currently loaded shaders

    // constant management
    void         ClearConstantPredicate();
    void         ClearConstantMapping();
    BOOL         SetLogicalConstant(ULONG _eType, VECTOR4FLOAT *_aoNewConstant, ULONG _ulSize);
    inline BOOL  IsLogConstantPresent(ULONG _eType) { return m_abLogConstantPresent[_eType]; }
    inline INT   GetLogConstantIndex(ULONG _eType);
    inline ULONG GetLogConstantSize(ULONG _eType) { return m_aulLogConstantSize[_eType]; }

    // Feature functions
    void         ClearAllFeatures();
    virtual void SetFeature(ULONG _eFeature, ULONG _ulValue);

    // Functions
    INT          FindPhySpace(ULONG _ulSize);
    void         UpdateConstantMapping(ULONG _eType, INT iNewIndex);
    BOOL         AddNewLogConstant(ULONG _eType, VECTOR4FLOAT *_aoNewConstant, ULONG _ulSize);

public:
    LPDIRECT3DDEVICE9   m_pD3DDevice;
    std::vector<ShaderInterface> m_vecLoadedShaders;

    // Members
    DirtyFlags          m_oDirtyFlags;

    // vertex shader constants
    BOOL                m_abLogConstantPresent[LogConstCount];
    ULONG               m_aulLogConstantSize[LogConstCount];
    VECTOR4BOOL         *m_poPredicate;
    VECTOR4INT          *m_poConstantMapping;
    XeConstantTable     *m_poCurPhyConstantTable;
    XeConstantTable     m_oPhyConstantTable;
    BOOL                m_abPhyConstantPresent[PhyConstCount];

    // Features
    ULONG               m_aulFeature[FeatureCount];
};

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
template<class ShaderInterface, ULONG FeatureCount, ULONG LogConstCount, ULONG PhyConstCount>
void XeShaderManager<ShaderInterface, FeatureCount, LogConstCount, PhyConstCount>::UnloadAll(void)
{
    for (ULONG ulCurShader = 0; ulCurShader < m_vecLoadedShaders.size(); ulCurShader++)
    {
        SAFE_RELEASE(m_vecLoadedShaders[ulCurShader]);
    }

    m_vecLoadedShaders.clear();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
template<class ShaderInterface, ULONG FeatureCount, ULONG LogConstCount, ULONG PhyConstCount>
void XeShaderManager<ShaderInterface, FeatureCount, LogConstCount, PhyConstCount>::ClearConstantPredicate()
{
    ZeroMemory(m_poPredicate, FeatureCount * sizeof(VECTOR4BOOL));
    m_oDirtyFlags.bPredicateConst = TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
template<class ShaderInterface, ULONG FeatureCount, ULONG LogConstCount, ULONG PhyConstCount>
void XeShaderManager<ShaderInterface, FeatureCount, LogConstCount, PhyConstCount>::ClearConstantMapping()
{
    ZeroMemory(m_abLogConstantPresent, LogConstCount * sizeof(BOOL));
    m_oDirtyFlags.bMappingConst = TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
template<class ShaderInterface, ULONG FeatureCount, ULONG LogConstCount, ULONG PhyConstCount>
BOOL XeShaderManager<ShaderInterface, FeatureCount, LogConstCount, PhyConstCount>::SetLogicalConstant(ULONG _eType, VECTOR4FLOAT *_aoNewConstant, ULONG _ulSize)
{
    if ((_eType >= LogConstCount) || 
        (_aoNewConstant == NULL)) 
    {
        return FALSE;
    }

    if (IsLogConstantPresent(_eType))
    {
        if (GetLogConstantSize(_eType) >= _ulSize)
        {
            // get the current mapping and copy over
            L_memcpy(   &m_poCurPhyConstantTable->afUser[GetLogConstantIndex(_eType)],
                _aoNewConstant,
                _ulSize * sizeof(VECTOR4FLOAT));

            // mark physical constant as occupied
            L_memset(   &m_abPhyConstantPresent[GetLogConstantIndex(_eType)], 
                0xFF, 
                _ulSize * sizeof(BOOL));

            if (GetLogConstantSize(_eType) > _ulSize)
            {
                // free up unused space
                L_memset(   &m_abPhyConstantPresent[GetLogConstantIndex(_eType) + _ulSize],
                    0, 
                    (GetLogConstantSize(_eType) - _ulSize) * sizeof(BOOL));
            }

            m_oDirtyFlags.bUserConst = TRUE;
        }
        else
        {
            // must relocate logical constant

            // release physical constant
            L_memset(   &m_abPhyConstantPresent[GetLogConstantIndex(_eType)],
                0, 
                _ulSize * sizeof(BOOL));

            // add constant
            AddNewLogConstant(_eType, _aoNewConstant, _ulSize);
        }
    }
    else
    {
        // add constant
        AddNewLogConstant(_eType, _aoNewConstant, _ulSize);
    }

    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : returns physical constant index
// ------------------------------------------------------------------------------------------------
template<class ShaderInterface, ULONG FeatureCount, ULONG LogConstCount, ULONG PhyConstCount>
INT XeShaderManager<ShaderInterface, FeatureCount, LogConstCount, PhyConstCount>::GetLogConstantIndex(ULONG _eType)
{
    ULONG ulPhysConstant = ((ULONG) _eType) / 4;
    ULONG ulComponent = ((ULONG) _eType) % 4;

    return m_poCurPhyConstantTable->aiMapping[ulPhysConstant].component[ulComponent];
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : -1 if hole not found
//          physical constant index of beginning of hole otherwise
// Descr. : 
// ------------------------------------------------------------------------------------------------
template<class ShaderInterface, ULONG FeatureCount, ULONG LogConstCount, ULONG PhyConstCount>
INT XeShaderManager<ShaderInterface, FeatureCount, LogConstCount, PhyConstCount>::FindPhySpace(ULONG _ulSize)
{
    INT   iHoleIndex = 0;
    INT   iHoleSize = 0;
    BOOL  bHoleFound = FALSE;
    INT   iCurPhyConst;

    for (iCurPhyConst = 0; iCurPhyConst < PhyConstCount; iCurPhyConst++)
    {
        // find hole
        if (m_abPhyConstantPresent[iCurPhyConst] == 0)
        {
            // hole found
            iHoleIndex = iCurPhyConst;
            iHoleSize  = 1;
            iCurPhyConst++;

            // find how big is the hole
            for (iCurPhyConst; (iCurPhyConst < (INT) PhyConstCount) && (iHoleSize < (INT) _ulSize); iCurPhyConst++)
            {
                if (m_abPhyConstantPresent[iCurPhyConst] == 0)
                {
                    // constant free
                    iHoleSize++;
                }
            }

            if (iHoleSize >= (INT) _ulSize)
            {
                // a big enough hole was found
                bHoleFound = TRUE;
                break;
            }
        }
    }

    if (bHoleFound)
        return iHoleIndex;
    else
        return -1;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
template<class ShaderInterface, ULONG FeatureCount, ULONG LogConstCount, ULONG PhyConstCount>
void XeShaderManager<ShaderInterface, FeatureCount, LogConstCount, PhyConstCount>::UpdateConstantMapping(ULONG _eType, INT iNewIndex)
{
    ULONG ulPhysConstant = ((ULONG) _eType) / 4;
    ULONG ulComponent = ((ULONG) _eType) % 4;

    m_poCurPhyConstantTable->aiMapping[ulPhysConstant].component[ulComponent] = iNewIndex;

    m_oDirtyFlags.bMappingConst = TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
template<class ShaderInterface, ULONG FeatureCount, ULONG LogConstCount, ULONG PhyConstCount>
BOOL XeShaderManager<ShaderInterface, FeatureCount, LogConstCount, PhyConstCount>::AddNewLogConstant(ULONG _eType, VECTOR4FLOAT *_aoNewConstant, ULONG _ulSize)
{
    // find new location
    INT iNewLocation = FindPhySpace(_ulSize);

    if (iNewLocation != -1)
    {
        // new location found

        // update mapping table
        UpdateConstantMapping(_eType, iNewLocation);

        // 
        L_memcpy(   &m_poCurPhyConstantTable->afUser[iNewLocation],  
            _aoNewConstant,
            _ulSize * sizeof(VECTOR4FLOAT));

        // mark physical constant as occupied
        L_memset(   &m_abPhyConstantPresent[iNewLocation], 
            0xFF, 
            _ulSize * sizeof(BOOL));

        // save constant size
        m_aulLogConstantSize[_eType] = _ulSize;

        m_oDirtyFlags.bUserConst = TRUE;

        return TRUE;
    }

    return FALSE;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
template<class ShaderInterface, ULONG FeatureCount, ULONG LogConstCount, ULONG PhyConstCount>
void XeShaderManager<ShaderInterface, FeatureCount, LogConstCount, PhyConstCount>::ClearAllFeatures()
{
    ZeroMemory(m_aulFeature, FeatureCount * sizeof(ULONG));
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
template<class ShaderInterface, ULONG FeatureCount, ULONG LogConstCount, ULONG PhyConstCount>
void XeShaderManager<ShaderInterface, FeatureCount, LogConstCount, PhyConstCount>::SetFeature(ULONG _eFeature, ULONG _ulValue)
{
    m_aulFeature[_eFeature] = _ulValue != 0 ? 1 : 0;
    m_oDirtyFlags.bPredicateConst = TRUE;
}

#endif