#ifndef XEUTILS_HEADER
#define XEUTILS_HEADER

#include <vector>
#include "XeVertexBuffer.h"
#include "SDK/Sources/BASe/ERRors/ERR.h"
#include "GraphicDK/Sources/MATerial/MATstruct.h"
#include "XeSharedDefines.h"

#if defined(ACTIVE_EDITORS)
extern CHAR EDI_gsz_StartupPath[];
#define gsz_StartupPath EDI_gsz_StartupPath
#endif

#if defined(PCWIN_TOOL)
extern CHAR ENG_gsz_StartupPath[];
#define gsz_StartupPath ENG_gsz_StartupPath
#endif

#define XE_CUBE_MAP_MASK            0x08000000

#define XE_VS_CUSTOM_FEATURE_COUNT 15
#define XE_PS_CUSTOM_FEATURE_COUNT 15

#define XE_GENERIC_FILENAME			"Generic_filename"
#define XE_SCREENSHOT_DIR_NAME		"d:\\Screenshot"
#define XE_PROFILING_DIR_NAME		"d:\\Profiling"

// ULONG128 data type
class ULONG128Holder
{
public:

    inline bool operator!= (const ULONG128Holder& rhs) const { return ((rhs.part1 != part1) || (rhs.part2 != part2)); }
    inline bool operator== (const ULONG128Holder& rhs) const { return ((rhs.part1 == part1) && (rhs.part2 == part2)); }
    inline bool operator<  (const ULONG128Holder& rhs) const { return ((part1 < rhs.part1) ? true : ((part1 > rhs.part1) ? false : (part2 < rhs.part2 )));}

    ULONG64 part1;
    ULONG64 part2;
};

class ULONG128 : public ULONG128Holder
{
public:

    inline  ULONG128()                               { part1 = 0;          part2 = 0;          }
    inline  ULONG128(ULONG64 _part1, ULONG64 _part2) { part1 = _part1;     part2 = _part2;     }
    inline  ULONG128(ULONG64 _part)                  { part1 = 0;          part2 = _part;      }
    inline  ULONG128(const ULONG128 &_rhs)           { part1 = _rhs.part1; part2 = _rhs.part2; }
    inline ~ULONG128() {}

    ULONG128& operator= (const ULONG128& _rhs) { part1 = _rhs.part1; part2 = _rhs.part2; return *this; }
    ULONG128& operator= (const ULONG64&  _rhs) { part1 = 0;          part2 = _rhs;       return *this; }

    inline operator ULONG64() const { return part2; }
};

// Abstract Data Types
typedef std::vector<IDirect3DBaseTexture9 *> XeTextureContainer;

// Utility functions
BOOL XeLoadFile( const CHAR* strFileName, VOID** ppFileData, DWORD* pdwFileSize = NULL);
void XeUnloadFile( VOID* pFileData );
void DrawRectangleEx(float _fX1, float _fY1,
                     float _fX2, float _fY2,
                     float _fU1, float _fV1,
                     float _fU2, float _fV2,
                     DWORD _dwColorTL,
                     DWORD _dwColorTR,
                     DWORD _dwColorBL,
                     DWORD _dwColorBR,
                     float _fAngle,
                     DWORD _dwBlendingFlags,
                     DWORD TextureID);

float Gaussian1D(float x, float sigma);
float GetGaussianDistribution( float x, float y, float rho );
void GetGaussianOffsets( bool bHorizontal, D3DXVECTOR2 vViewportTexelSize,
                        D3DXVECTOR4* vSampleOffsetsAndWeights );
void GetGaussianOffsets7x7( BOOL bHorizontal, FLOAT fViewportTexelSize,
                           D3DXVECTOR4* vSampleOffsetsAndWeights );

float XeDistanceCameraToBV( D3DXVECTOR3 * _vCamPos, XeMesh * _poMesh, D3DXMATRIX * pWorldMatrix );


// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline static ULONG ConvertColorSource(ULONG _ul_JadeColorSource)
{
    switch (_ul_JadeColorSource)
    {
    case MAT_Cc_ColorOp_Diffuse:        return COLORSOURCE_DIFFUSE;
    case MAT_Cc_ColorOp_Specular:       return COLORSOURCE_DIFFUSE;
    case MAT_Cc_ColorOp_Disable:        return COLORSOURCE_CONSTANT;
    case MAT_Cc_ColorOp_RLI:            return COLORSOURCE_VERTEX_COLOR;
    case MAT_Cc_ColorOp_FullLight:      return COLORSOURCE_FULL_LIGHT;
    case MAT_Cc_ColorOp_InvertDiffuse:  return COLORSOURCE_INVERSE_DIFFUSE;
    case MAT_Cc_ColorOp_Diffuse2X:      return COLORSOURCE_DIFFUSE2X;
    case MAT_Cc_ColorOp_SpecularColor:  return COLORSOURCE_DIFFUSE;
    case MAT_Cc_ColorOp_DiffuseColor:   return COLORSOURCE_DIFFUSE;
    case MAT_Cc_ColorOp_ConstantColor:  return COLORSOURCE_CONSTANT;
    default:                            return COLORSOURCE_VERTEX_COLOR;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline static ULONG ConvertUVSource(ULONG _ul_JadeUVSource)
{
    switch (_ul_JadeUVSource)
    {
    case MAT_Cc_UV_Object1:     return UVSOURCE_OBJECT;
    case MAT_Cc_UV_Chrome:      return UVSOURCE_CHROME;
    case MAT_Cc_UV_Planar_GZMO: return UVSOURCE_PLANAR_GIZMO;
	case MAT_Cc_UV_DF_GZMO:		return UVSOURCE_DIFFUSEPHALOFF;
    default:                    return UVSOURCE_OBJECT;
    }
}

// Macros

#define VS_INVALID_HANDLE 0xffffffff

#ifndef SAFE_FREE
#define SAFE_FREE(_x_)     \
	if ((_x_) != NULL)       \
{                        \
	MEM_Free(_x_);        \
	(_x_) = NULL;        \
}
#endif // #ifndef SAFE_DELETE

#ifndef SAFE_DELETE
#define SAFE_DELETE(_x_)     \
    if ((_x_) != NULL)       \
    {                        \
        delete (_x_);        \
        (_x_) = NULL;        \
    }
#endif // #ifndef SAFE_DELETE

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(_x_)    \
    if ((_x_) != NULL)            \
    {                             \
        delete [] (_x_);          \
        (_x_) = NULL;             \
    }
#endif // #ifndef SAFE_DELETE_ARRAY

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(_x_)    \
    if ((_x_) != NULL)       \
    {                        \
        (_x_)->Release();    \
        (_x_) = NULL;        \
    }
#endif // #ifndef SAFE_RELEASE

// convert ABGR color to ARGB
#define XeConvertColor(a) (a & 0xff00ff00) | ((a & 0xff) << 16) | ((a & 0xff0000) >> 16)

// Attenuate a color for Mul2X
#define XeAttenuateColor(a) (((a) & 0xfefefefe) >> 1)

// For converting a FLOAT to a DWORD (useful for SetRenderState() calls)
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

// force a value to a specific alignment - assumes POW2 alignments
inline DWORD AlignTo(DWORD dwValue, DWORD dwAlignment)
{
    return (dwValue + (dwAlignment - 1) & ~(dwAlignment - 1));
}

inline ULONG XeMakeDEC3N(FLOAT* pVtx)
{
    int iX = (int)(pVtx[0] * 511.0f);
    int iY = (int)(pVtx[1] * 511.0f);
    int iZ = (int)(pVtx[2] * 511.0f);

    return (iX & 0x3FF) | ((iY & 0x3FF) << 10) | ((iZ & 0x3FF) << 20);
}

inline void XeExpandDEC3N(FLOAT* _pBuffer, ULONG _ulVal)
{
    int iX =  _ulVal        & 0x3FF;
    int iY = (_ulVal >> 10) & 0x3FF;
    int iZ = (_ulVal >> 20) & 0x3FF;

    *_pBuffer = (FLOAT)iX / 511.0f; ++_pBuffer;
    *_pBuffer = (FLOAT)iY / 511.0f; ++_pBuffer;
    *_pBuffer = (FLOAT)iZ / 511.0f; ++_pBuffer;
}

inline void XeMakeSHORT4N(SHORT* pOut, FLOAT* pIn)
{
    pOut[0] = (SHORT)(pIn[0] * 32767.0f);
    pOut[1] = (SHORT)(pIn[1] * 32767.0f);
    pOut[2] = (SHORT)(pIn[2] * 32767.0f);
    pOut[3] = (SHORT)(pIn[3] * 32767.0f);
}

#if defined(_XENON_RENDER_PC)

void XeOnValidationFailed(const CHAR* _str);

#define XeValidate(_expr, _str)                 if (!(_expr)) { XeOnValidationFailed(_str); }
#define XeValidateRet(_expr, _retVal, _str)     if (!(_expr)) { XeOnValidationFailed(_str); return _retVal; }
#define XeValidateErr(_expr, _retVal, _str)     XeValidateRet(_expr, _retVal, _str)

#else // -> !defined(_XENON_RENDER_PC)

#define XeValidate(_expr, _str)                 ERR_X_Assert(_expr)
#define XeValidateRet(_expr, _retVal, _str)     if (!(_expr)) { ERR_X_Assert(_expr) return _retVal; }
#define XeValidateErr(_expr, _retVal, _str)     if (!(_expr)) { ERR_X_Error(FALSE, _str, NULL); return _retVal; }

#endif // defined(_XENON_RENDER_PC)

// ------------------------------------------------------------------------------------------------
// TYPES
// ------------------------------------------------------------------------------------------------

enum XeVertexComponents
{
    XEVC_POSITION               = 0x00000001, // float[3]
    XEVC_POSITIONT              = 0x00000002, // float[4]
    XEVC_BLENDWEIGHT4           = 0x00000004, // float[4]
    XEVC_BLENDINDICES           = 0x00000008, // ULONG
    XEVC_NORMAL                 = 0x00000010, // float[3]
    XEVC_TANGENT                = 0x00000020, // float[4]
    XEVC_COLOR0                 = 0x00000100, // ULONG
    XEVC_COLOR1                 = 0x00000200, // ULONG
    XEVC_COLOR2                 = 0x00000400, // ULONG
    XEVC_COLOR3                 = 0x00000800, // ULONG
    XEVC_TEXCOORD0              = 0x00010000, // float[2]
    XEVC_TEXCOORD1              = 0x00020000, // float[2]
    XEVC_TEXCOORD2              = 0x00040000, // float[2]
    XEVC_TEXCOORD3              = 0x00080000, // float[2]
    XEVC_TEXCOORD0_FLOAT4       = 0x00100000, // float[4]
    XEVC_TEXCOORD1_FLOAT4       = 0x00200000, // float[4]
    XEVC_TEXCOORD2_FLOAT4       = 0x00400000, // float[4]
    XEVC_TEXCOORD3_FLOAT4       = 0x00800000, // float[4]

    // Compressed types
    XEVC_BLENDWEIGHT4_SHORT4N   = 0x01000004, // SHORT[4] (SHORT4N)
    XEVC_NORMAL_DEC3N           = 0x02000010, // ULONG    (DEC3N)
    XEVC_TANGENT_SHORT4N        = 0x04000020, // SHORT[4] (SHORT4N)

    XEVC_BLENDWEIGHT3_DEC3N     = 0x10000004, // ULONG (DEC3N) // spg2
    XEVC_TANGENT3_DEC3N         = 0x20000020, // ULONG (DEC3N) // spg2

    XEVC_UNDEFINED              = 0x80000000, // Used for dynamic meshes

    XEVC_COMPESSION_MASK        = 0xFF000000, // Compressed format mask

    XEVC_FORCE_DWORD            = 0xffffffff
};

// ------------------------------------------------------------------------------------------------
// STRUCTURES
// ------------------------------------------------------------------------------------------------

struct XeVertexStream
{
    ULONG                   ulComponents;
    XeBuffer                *pBuffer;
};

typedef union {
    struct {
        INT x;
        INT y;
        INT z;
        INT w;
    };
    INT component[4];
} VECTOR4INT;

typedef union {
    struct {
        BOOL x;
        BOOL y;
        BOOL z;
        BOOL w;
    };
    BOOL component[4];
} VECTOR4BOOL;

typedef union {
    struct {
        FLOAT x;
        FLOAT y;
        FLOAT z;
        FLOAT w;
    };
    FLOAT component[4];
} VECTOR4FLOAT;

typedef struct _XESKINNINGMATRIX
{
    VECTOR4FLOAT _m[4];

} XeSkinningMatrix;

typedef struct SimpleVertex_
{
    float x,y,z,rhw;
    DWORD Color;
    float u1,v1;

} SimpleVertex;

typedef struct SimpleVertexOnly_
{
    float x,y,z,rhw;

} SimpleVertexOnly;

typedef struct SimpleVertexNoColor_
{
    float x,y,z,rhw;
    float u1,v1;

} SimpleVertexNoColor;

#define MAX_BONES_PER_VERTEX 4
typedef struct FurOffsetVertex_
{
    FLOAT fX, fY, fZ;
    FLOAT afWeight[MAX_BONES_PER_VERTEX];
    BYTE  abyIndex[MAX_BONES_PER_VERTEX];
    FLOAT fTangentX, fTangentY, fTangentZ;
    FLOAT fBinormalX, fBinormalY, fBinormalZ;
    FLOAT fOffsetX, fOffsetY;
} FurOffsetVertex;

struct XeLightConsts
{
    VECTOR4FLOAT consts[4];
};

// Lighting

struct XeLightSet
{
    union
    {
        struct
        {
            ULONG ulDirLightCount;
            ULONG ulOmniLightCount;
            ULONG ulSpotLightCount;
            ULONG ulCylSpotLightCount;
        };
        INT ulLightCounts[4];
    };

    ULONG ulDirLightIndices[VS_MAX_LIGHTS];
    ULONG ulOmniLightIndices[VS_MAX_LIGHTS];
    ULONG ulSpotLightIndices[VS_MAX_LIGHTS];
    ULONG ulCylSpotLightIndices[VS_MAX_LIGHTS];

    inline void    Clear               ( ) { ulDirLightCount = ulOmniLightCount = ulSpotLightCount = ulCylSpotLightCount = 0; }
    inline ULONG   GetTotalLightCount  ( ) { return ulDirLightCount + ulOmniLightCount + ulSpotLightCount + ulCylSpotLightCount; }
    inline ULONG   GetDirLightCount    ( ) { return ulDirLightCount; }
    inline ULONG   GetOmniLightCount   ( ) { return ulOmniLightCount; }
    inline ULONG   GetSpotLightCount   ( ) { return ulSpotLightCount; }
    inline ULONG   GetCylSpotLightCount( ) { return ulCylSpotLightCount; }
    inline INT    *GetLightCountArray  ( ) { return ulLightCounts; }  
};

// Only first 4 fields are uploaded to shader
typedef struct _XELIGHT
{
    VECTOR4FLOAT position;
    VECTOR4FLOAT direction;
    VECTOR4FLOAT color;
    VECTOR4FLOAT params;
    FLOAT        RLIBlendingScale;
    FLOAT        RLIBlendingOffset;
    FLOAT        DiffuseMultiplier;
    FLOAT        SpecularMultiplier;
    FLOAT        ActorDiffusePonderator;
    FLOAT        ActorSpecularPonderator;
    void*        JadeLight;
    BOOL         IsRimLight;
    BOOL         IsLM;
    BOOL         IsInverted;
} XeLight;

#define XELIGHT_VECTOR_COUNT    4

typedef struct _XESHADOWLIGHT
{
    void*           JadeLight;
    ULONG           LightType;
    ULONG           ShadowTextureID;
    float           ShadowNear;
    float           ShadowFar;
    float           Umbra;      // aka Hotspot (3DSMax), Alpha (Jade)
    float           Penumbra;   // aka Falloff (3DSMax), BigAlpha (Jade)
    VECTOR4FLOAT    Color;      // Shadow color, alpha = fade
    D3DXMATRIX      View;
    D3DXMATRIX      Projection[2];
    D3DXMATRIX      InvCameraProj;
    int             NumIterations;
    float           FilterSize;
    float           ZOffset;
    float           HiResShadowFOV;
    DWORD           CookieTexureHandle;
    BOOL            ForceStaticReceiver;

    D3DXVECTOR3     ClipPlanes[4];
    float           CylinderRadius;
    float           Priority;   // Currently distance from viewer
} XeShadowLight;

struct XeSkinningSet
{
    ULONG ulSkinningMatrixIndex;
    ULONG ulSkinningMatrixCount;
};

// Wave Your Body parameters to be sent to vertex shader
typedef struct _XEWYBPARAMS
{
    VECTOR4FLOAT    Axes[2];       // X, Y, Z
    VECTOR4FLOAT    Conditions[2]; // Planar, UseAlpha, InverAlpha
    VECTOR4FLOAT    Values[2];     // Amplitude, Angle, Factor, ApplyFur
} XeWYBParams;

// Symmetry modifier parameters
typedef struct _XESYMMETRYPARAMS
{
    ULONG ulAxis;
    FLOAT fOffset;
} XeSymmetryParams;

// Transposed column-major matrix 
struct Float4x3
{
    VECTOR4FLOAT    m_vColumns[3];
};
void XeConvertToD3DMatrix( D3DXMATRIX & _to, const Float4x3 & _from );
void XeConvertToFloat4x3( Float4x3 & _to, const D3DXMATRIX & _from );

struct Float4x2
{
    VECTOR4FLOAT    m_vColumns[2];

};
void XeConvertToD3DMatrix( D3DXMATRIX & _to, const Float4x2 & _from );
void XeConvertToFloat4x2( Float4x2 & _to, const D3DXMATRIX & _from );

// ------------------------------------------------------------------------------------------------
// FUNCTIONS
// ------------------------------------------------------------------------------------------------

inline void XeSetD3DXMatrix(D3DXMATRIX* _pst_Mat, 
                            FLOAT _f11, FLOAT _f12, FLOAT _f13, FLOAT _f14,
                            FLOAT _f21, FLOAT _f22, FLOAT _f23, FLOAT _f24,
                            FLOAT _f31, FLOAT _f32, FLOAT _f33, FLOAT _f34,
                            FLOAT _f41, FLOAT _f42, FLOAT _f43, FLOAT _f44)
{
    ERR_X_Assert(_pst_Mat != NULL);

    _pst_Mat->_11 = _f11; _pst_Mat->_12 = _f12; _pst_Mat->_13 = _f13; _pst_Mat->_14 = _f14;
    _pst_Mat->_21 = _f21; _pst_Mat->_22 = _f22; _pst_Mat->_23 = _f23; _pst_Mat->_24 = _f24;
    _pst_Mat->_31 = _f31; _pst_Mat->_32 = _f32; _pst_Mat->_33 = _f33; _pst_Mat->_34 = _f34;
    _pst_Mat->_41 = _f41; _pst_Mat->_42 = _f42; _pst_Mat->_43 = _f43; _pst_Mat->_44 = _f44;
}

// Convert from ABGR to RGBA
inline void XeJadeColorToV4F(VECTOR4FLOAT* _p_Vec, ULONG _ul_Val)
{
    ERR_X_Assert(_p_Vec != NULL);

    _p_Vec->x = (FLOAT)(_ul_Val & 0xff) / 255.0f;
    _p_Vec->y = (FLOAT)((_ul_Val >> 8) & 0xff) / 255.0f;
    _p_Vec->z = (FLOAT)((_ul_Val >> 16) & 0xff) / 255.0f;
    _p_Vec->w = (FLOAT)((_ul_Val >> 24) & 0xff) / 255.0f;
}

ULONG GetVertexSize(ULONG _ulComponents);

inline void XePrintMatrix(const D3DXMATRIX* _poMtx)
{
    char s[256];
    OutputDebugString("\n-------------------------------------------\n");
    sprintf(s, "% 10.3f % 10.3f % 10.3f % 10.3f\n", _poMtx->_11, _poMtx->_12, _poMtx->_13, _poMtx->_14);
    OutputDebugString(s);
    sprintf(s, "% 10.3f % 10.3f % 10.3f % 10.3f\n", _poMtx->_21, _poMtx->_22, _poMtx->_23, _poMtx->_24);
    OutputDebugString(s);
    sprintf(s, "% 10.3f % 10.3f % 10.3f % 10.3f\n", _poMtx->_31, _poMtx->_32, _poMtx->_33, _poMtx->_34);
    OutputDebugString(s);
    sprintf(s, "% 10.3f % 10.3f % 10.3f % 10.3f\n", _poMtx->_41, _poMtx->_42, _poMtx->_43, _poMtx->_44);
    OutputDebugString(s);
}

//------------------------------------
// CLASS : XeGrowableArray
//------------------------------------
template < typename T >
class XeGrowableArray
{
    //----------------------------
    // Construction / Destruction
    //----------------------------
public:
    XeGrowableArray                 ( ULONG _ulInitialSize );
    ~XeGrowableArray                ( );

    //----------------------------
    // Data access
    //----------------------------
    T *                 At          ( ULONG _ulIndex );
    ULONG               GetCount    ( );                 

    //----------------------------
    // Operations
    //----------------------------
    T *                 Push        ( );
    void                Clear       ( );
    void                SetCount    ( ULONG _ulCount );

    //----------------------------
    // Helpers
    //----------------------------
private:
    void                Grow        ( );

    //----------------------------
    // Attributes
    //----------------------------
private:
    T *                 m_array;
    ULONG               m_ulCount;
    ULONG               m_ulCurrentSize;
};

//--------------------------------------
// XeGrowableArray::XeGrowableArray
//--------------------------------------
template < typename T >
XeGrowableArray< T >::XeGrowableArray( ULONG _ulInitialSize ) :
m_array             ( NULL ),
m_ulCount           ( 0 ),
m_ulCurrentSize     ( 0 )   
{
    ERR_X_Assert( _ulInitialSize > 0 );
    m_array = new T[_ulInitialSize];
    memset( m_array, 0, _ulInitialSize * sizeof(T) );

    m_ulCurrentSize = _ulInitialSize;
}

//--------------------------------------
// XeGrowableArray::~XeGrowableArray
//--------------------------------------
template < typename T >
XeGrowableArray< T >::~XeGrowableArray( )
{
    if( m_array != NULL )
    {
        delete [] m_array;
        m_array = NULL;
    }
}

//--------------------------------------
// XeGrowableArray::At
//--------------------------------------
template < typename T >
inline T *      
XeGrowableArray< T >::At( ULONG _ulIndex )
{
    ERR_X_Assert( _ulIndex < m_ulCount );
    return &m_array[_ulIndex];
}

//--------------------------------------
// XeGrowableArray::GetCount
//--------------------------------------
template < typename T >
inline ULONG          
XeGrowableArray< T >::GetCount( )
{
    return m_ulCount;
}

//--------------------------------------
// XeGrowableArray::Push
//--------------------------------------
template < typename T >
inline T *            
XeGrowableArray< T >::Push( )
{
    if( m_ulCount == m_ulCurrentSize )
    {
        Grow();
    }

    ERR_X_Assert( m_ulCount < m_ulCurrentSize );
    return &m_array[m_ulCount++];
}

//--------------------------------------
// XeGrowableArray::Grow
//--------------------------------------
template < typename T >
void          
XeGrowableArray< T >::Grow( )
{
    // We need to allocate a bigger array
    T * newArray = new T[ 2 * m_ulCurrentSize ];

    // Copy from one to the other
    memcpy( newArray, m_array, m_ulCurrentSize * sizeof(T) );
    memset( (newArray + m_ulCurrentSize), 0, m_ulCurrentSize * sizeof(T) );

    // Dealloc previous array
    delete [] m_array;

    m_array = newArray;
    m_ulCurrentSize *= 2;
}

//--------------------------------------
// XeGrowableArray::SetCount
//--------------------------------------
template < typename T >
inline void           
XeGrowableArray< T >::SetCount( ULONG _ulCount )
{
    // NOTE: Array can only shrink
    ERR_X_Assert( _ulCount <= m_ulCount );
    m_ulCount = _ulCount;
}

//--------------------------------------
// XeGrowableArray::Clear
//--------------------------------------
template < typename T >
inline void       
XeGrowableArray< T >::Clear( )
{
    m_ulCount = 0;
}

#ifdef ACTIVE_EDITORS
//--------------------------------------
// CLASS : XeLightUsageLogger
//--------------------------------------
class XeLightUsageLogger
{
    //----------------------------
    // Types
    //----------------------------
    enum eState
    {
        eState_ReadyForGao = 0,
        eState_InGao,
        eState_InElement,
    };

    //----------------------------
    // Construction / Destruction
    //----------------------------
public:
    XeLightUsageLogger              ( );
    ~XeLightUsageLogger             ( );

    //----------------------------
    // Data access
    //----------------------------
    bool            IsLogging       ( );

    //----------------------------
    // Operations
    //----------------------------
    void            BeginLog        ( const char * _szLogPath, const char * _szWorldName );
    void            EndLog          ( );

    void            BeginGAO        ( const char * _szObjectName );
    void            BeginElement    ( ULONG _ulElementIndex );
    void            AddLight        ( const char * _szLightName );
    void            EndElement      ( );
    void            EndGAO          ( );

    //----------------------------
    // Helpers
    //----------------------------
private:
    void            Close           ( );
    void            Write           ( const char * _szLine );

    //----------------------------
    // Attributes 
    //----------------------------
private:
    HANDLE          m_handle;
    ULONG           m_ulCurrentLightIndex;
    eState          m_state;
};

//------------------------------------------
// XeLightUsageLogger::IsLogging
//------------------------------------------
inline bool    
XeLightUsageLogger::IsLogging( )
{
    return (m_handle != 0);
}


extern XeLightUsageLogger g_oXeLightUsageLogger;

#endif // ACTIVE_EDITORS

#if defined(ACTIVE_EDITORS)
void XeEdModifyColor(VECTOR4FLOAT* _pColor, ULONG _ulQMFRFlags);
#endif

void GetScreenShotDirName(char* _sz_ScreenshotDirName);
void   GetScreenShotFileName(char* _sz_ScreenshotFileName, D3DXIMAGE_FILEFORMAT _FileFormat);

void GetProfilingDirName(char* _sz_ProfilingDirName);
void   GetProfilingFileName(char* _sz_ProfilingFileName);

void   GetGenericFileName(char* _sz_FileName);

BOOL XeCreateDirectory(char* _szDirName);

typedef enum
{
    XeRT_FIRSTLIST      = 0,                // For the "for" loops

    // *************************************************************************
    // *** When modifying this list, please update XeRenderer::GetListName() ***
    // *************************************************************************

    XeRT_OPAQUE_NO_FOG = XeRT_FIRSTLIST,    // Opaque elements not included in the z prepass
    XeRT_OPAQUE,                            // No "special" treatment
    XeRT_TRANSPARENT,                       // Transparent objects, Z-sorted
    XeRT_WATER,                             // Water patches
    XeRT_INTERFACE,                         // 2D UI objects, not sorted
    XeRT_REFLECTED,                         // Reflected objects, water rendering only for now
    XeRT_HEAT_SHIMMER_QUADS,                // Z-Tested heat shimmer quads used to create mask
    XeRT_SPG2,                              // spg2
    XeRT_SPG2OPAQUE,                        // spg2 meshes (plants, etc..)
    XeRT_SPG2TRANSPARENT,                   // spg2 transparent (fire)
    XeRT_TRANSPARENT_GLOW,                  // glowing transparent 
    XeRT_FUR,                               // fur
    XeRT_ZOVERWRITE,                        // objects that need to be drawn after all others and ignore the Z value from those objects
    XeRT_ZOVERWRITESPG2OPAQUE,              // SPG2 that are opaque that need to be drawn after all others and ignore the Z value from those objects
    XeRT_ZOVERWRITESPG2TRANSPARENT,         // SPG2 using alpha that need to be drawn after all others and ignore the Z value from those objects

#if defined(ACTIVE_EDITORS)
    XeRT_HELPERS,                           // Helper objects, editor only
#endif

    XeRT_NUMTYPES
} eXeRENDERLISTTYPE;

typedef struct _XeUIRectangle
{
    FLOAT fX1; FLOAT fY1; FLOAT fX2; FLOAT fY2;
    FLOAT fU1; FLOAT fV1; FLOAT fU2; FLOAT fV2;
    ULONG ulColorTL; ULONG ulColorTR; ULONG ulColorBL; ULONG ulColorBR;
    FLOAT fAngle; ULONG ulBlendingFlags; ULONG ulTextureID;

} XeUIRectangle;

#endif // XEUTILS_HEADER
