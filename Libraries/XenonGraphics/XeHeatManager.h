//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright 2005 Ubisoft
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __XEHEATMANAGER_H__INCLUDED
#define __XEHEATMANAGER_H__INCLUDED

#define MAX_HOTAIROBJECTS   512

#define NUMHEATGRIDSEGMENTS 6

// forward declarations
class XeIndexBuffer;
class XeRenderObject;
class XeMesh;      
class XeMaterial;  

struct HotAirObject
{
    MATH_tdst_Vector    Position;
    float               Width;
    float               Height;
    float               Range;
    float               ScrollSpeed;
    float               NoisePixelSize;
    float               Scroll;
    float               Intensity;
    float               BottomScale;
};

//////////////////////////////////////////////////////////////////////////
// HeatEffectManager

class HeatEffectManager
{
    //----------------------
    // Types
    //----------------------
public:
    struct ObjectInfo
    {
        ObjectInfo          ( ) : m_fDistanceScaling(1.0f) {}

        HotAirObject        m_object;
        float               m_fDistanceScaling;
        MATH_tdst_Vector    m_vCenterInCameraSpace;
        ULONG               m_ulProjectionMatrixIndex;
        FLOAT               m_fNoiseSizeU;
        FLOAT               m_fNoiseSizeV;
    };

    struct HotAirState
    {
        ObjectInfo          m_objectInfos[MAX_HOTAIROBJECTS];
        UINT				m_NumHotAirObjects;
        UINT                m_NumHotAirObjectsFromGrid;
    };

private:

    XeMaterial *        m_poMaskMaterial;
    XeIndexBuffer *	    m_poMaskIB;
    XeMesh *            m_poMaskMesh;

    XeRenderObject *    m_poOffsetRenderObject;
    XeMesh *            m_poOffsetMesh;
    XeMaterial *        m_poOffsetMaterial;
    XeIndexBuffer *	    m_pOffsetIB;

    XeRenderObject *    m_poBltRenderObject;
    XeMaterial *        m_poBltMaterial;

    float				m_fAlphaScale;

    HotAirState         m_oStateRender;
    HotAirState         m_oStateEngine;
  
public:
    HeatEffectManager                   ( );
    ~HeatEffectManager                  ( );

public:
    void        Init                    ( );
	void        Shutdown                ( );
    void        OnDeviceLost            ( );
    void        OnDeviceReset           ( );

    void        BackupState             ( ) { memcpy( &m_oStateRender, &m_oStateEngine, sizeof(HotAirState)); }
    void        Update                  ( ) { m_oStateEngine.m_NumHotAirObjects = m_oStateEngine.m_NumHotAirObjectsFromGrid = 0; }
    
    void        AddHotAirPoint          ( HotAirObject * pHotAirObject );
    void        AddHotAirPointToCache   ( HotAirObject * pHotAirObject, LONG ulProjectionMatrixIndex );

    void        RenderHotAirObjects     ( void );

private:
    void        InitOffsetsRenderTask   ( );
    void        InitMaskRenderTask      ( );
    void        InitIndices             ( XeIndexBuffer * pIndexBuffer, USHORT usSegments );

    ObjectInfo * SetupRender            ( HotAirObject * pHotAirObject, BOOL bFromGrid = FALSE, LONG ulProjectionMatrixIndex = -1 );

    void        RenderMaskQuad          ( ObjectInfo * pObjectInfo, BOOL bFromGrid = FALSE );
    void        RenderOffsetPass        ( );
    
    bool        ComputeDistanceScaling  ( ObjectInfo * pObjectInfo, HotAirObject * pHotAirObject, BOOL bPosInCameraSpace = FALSE );
    void        ComputeNoiseUVParams    ( float *            pfSizeU, 
                                          float *            pfSizeV,
                                          HotAirObject *     pHotAirObject );
    void        SortPlanes              ( );
    void        RenderOffsetsHelper     ( BOOL bAlphaOnly, BOOL bUseAlphaTexture );
};

extern HeatEffectManager g_oHeatManager;

#endif // __XEHEATMANAGER_H__INCLUDED