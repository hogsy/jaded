
#include "Precomp.h"
#include "XeMaterial.h"
#include "XeMesh.h"
#include "XeRenderObject.h"
#include "XeRenderer.h"
#include "XeUtils.h"
#include "XeVertexShaderManager.h"
#include "XeFXManager.h"
#include "XeRenderStateManager.h"

#if defined(ACTIVE_EDITORS)
extern BOOL EDI_gb_XeQuiet;
#include "GDInterface/GDInterface.h"
#endif

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
BOOL XeLoadFile( const CHAR* strFileName, VOID** ppFileData, DWORD* pdwFileSize )
{
    // Assert if NULL pointer

    if( pdwFileSize )
        *pdwFileSize = 0L;

    // Open the file for reading
    HANDLE hFile = CreateFile(  strFileName, GENERIC_READ, 0, NULL, 
                                OPEN_EXISTING, 0, NULL );

    if( INVALID_HANDLE_VALUE == hFile )
    {
        // Output Error
        return FALSE;
    }

    DWORD dwFileSize = GetFileSize( hFile, NULL );
    VOID* pFileData = malloc( dwFileSize );

    if( NULL == pFileData )
    {
        CloseHandle( hFile );
        
        // Output Error
        return FALSE;
    }

    DWORD dwBytesRead;
    ReadFile( hFile, pFileData, dwFileSize, &dwBytesRead, NULL );

    // Finished reading file
    CloseHandle( hFile ); 

    if( dwBytesRead != dwFileSize )
    {
        // Output Error
        return FALSE;
    }

    if( pdwFileSize )
        *pdwFileSize = dwFileSize;
    *ppFileData = pFileData;

    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeUnloadFile( VOID* pFileData )
{
    // Assert if NULL pointer
    
    free( pFileData );
}

//----------------------------------------------------------------------@FHB--
// DrawRectangleEx
// 
// Role   : Draws a quad with specified parameters
//          
// Params : _fX1 : Top left x coordinate
//          _fY1 : Top left y coordinate
//          _fX2 : Bottom right x coordinate
//          _fY2 : Bottom right y coordinate
//          _fU1 : Top left u coordinate
//          _fV1 : Top left v coordinate
//          _fU2 : Bottom right u coordinate
//          _fV2 : Bottom right v coordinate
//          _dwColorTL : Top left color
//          _dwColorTR : Top right color
//          _dwColorBL : Bottom left color
//          _dwColorBR : Bottom right color
//          _fAngle : Rotation angle (radians, positive is counter-clockwise)
//          _dwBlendingFlags : UNUSED, NOT YET IMPLEMENTED
//          TextureID : ID of texture to use on the quad
//          
// Return : void
// 
// Author : Eric Le
// Date   : 6 Jan 2005
//----------------------------------------------------------------------@FHE--
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
                     DWORD TextureID)
{
    g_oXeRenderer.AddUIRectangleRequest(_fX1, _fY1, _fX2, _fY2, _fU1, _fV1, _fU2, _fV2, 
                                        _dwColorTL, _dwColorTR, _dwColorBL,
                                        _dwColorBR, _fAngle, _dwBlendingFlags, TextureID);
}

ULONG GetVertexSize(ULONG _ulComponents)
{
    ULONG ulSize = 0;

    if (_ulComponents == XEVC_UNDEFINED)
        return 0;

    if (_ulComponents & XEVC_POSITIONT)
        ulSize += 4 * sizeof(float);
    else if (_ulComponents & XEVC_POSITION)
        ulSize += 3 * sizeof(float);

    if (_ulComponents & XEVC_BLENDWEIGHT4)
    {
        if ((_ulComponents & XEVC_BLENDWEIGHT4_SHORT4N) == XEVC_BLENDWEIGHT4_SHORT4N)
            ulSize += 4 * sizeof(SHORT);
        else if ((_ulComponents & XEVC_BLENDWEIGHT3_DEC3N) == XEVC_BLENDWEIGHT3_DEC3N)
            ulSize += sizeof(ULONG);
        else
            ulSize += 4 * sizeof(float);
    }

    if (_ulComponents & XEVC_BLENDINDICES)
        ulSize += 4 * sizeof(BYTE);

    if (_ulComponents & XEVC_NORMAL)
    {
        if ((_ulComponents & XEVC_NORMAL_DEC3N) == XEVC_NORMAL_DEC3N)
            ulSize += sizeof(ULONG);
        else 
            ulSize += 3 * sizeof(float);
    }

    if (_ulComponents & XEVC_TANGENT)
    {
        if ((_ulComponents & XEVC_TANGENT_SHORT4N) == XEVC_TANGENT_SHORT4N)
            ulSize += 4 * sizeof(SHORT);
        else if ((_ulComponents & XEVC_TANGENT3_DEC3N) == XEVC_TANGENT3_DEC3N)
            ulSize += sizeof(ULONG);
        else
            ulSize += 4 * sizeof(float);
    }

    if (_ulComponents & XEVC_COLOR0)
        ulSize += sizeof(ULONG);

    if (_ulComponents & XEVC_COLOR1)
        ulSize += sizeof(ULONG);

    if (_ulComponents & XEVC_COLOR2)
        ulSize += sizeof(ULONG);

    if (_ulComponents & XEVC_COLOR3)
        ulSize += sizeof(ULONG);

    if (_ulComponents & XEVC_TEXCOORD0_FLOAT4)
        ulSize += 4 * sizeof(float);
    else if (_ulComponents & XEVC_TEXCOORD0)
        ulSize += 2 * sizeof(float);

    if (_ulComponents & XEVC_TEXCOORD1_FLOAT4)
        ulSize += 4 * sizeof(float);
    else if (_ulComponents & XEVC_TEXCOORD1)
        ulSize += 2 * sizeof(float);

    if (_ulComponents & XEVC_TEXCOORD2_FLOAT4)
        ulSize += 4 * sizeof(float);
    else if (_ulComponents & XEVC_TEXCOORD2)
        ulSize += 2 * sizeof(float);

    if (_ulComponents & XEVC_TEXCOORD3_FLOAT4)
        ulSize += 4 * sizeof(float);
    else if (_ulComponents & XEVC_TEXCOORD3)
        ulSize += 2 * sizeof(float);

    return ulSize;
}

#if defined(_XENON_RENDER_PC)

void XeOnValidationFailed(const CHAR* _str)
{
#if defined(ACTIVE_EDITORS)
    if (EDI_gb_XeQuiet)
    {
        ERR_OutputDebugString("[Error] %s\n", _str);
        return;
    }
#endif

#if defined(_DEBUG)
    if (::MessageBox(NULL, _str, "Xenon - DX9 - Error", MB_OKCANCEL | MB_ICONERROR | MB_TASKMODAL) == IDCANCEL)
    {
        __asm int 03h;
    }
#else
    ::MessageBox(NULL, _str, "Xenon - DX9 - Error", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#endif
}

#endif

float GetGaussianDistribution( float x, float y, float rho )
{
    float g = 1.0f / sqrt( 2.0f * 3.141592654f * rho * rho );
    return g * exp( -(x * x + y * y) / (2 * rho * rho) );
}

// ------------------------------------------------------------------------------------------------
// Name   : Gaussian1D
// Params : x, sigma : Gaussian curve point and variance
// RetVal : Value
// Descr. : 1D Gaussian distribution solver
// ------------------------------------------------------------------------------------------------
float Gaussian1D(float x, float sigma)
{
    //         e^-((x^2) / (2 * sigma^2))
    // Coeff = --------------------------
    //            sqrt(2 * pi) * sigma

    float sqr2PiSigma = sqrt(6.28318531f) * sigma;
    float x2          = x * x;
    float twoSigma2   = 2.0f * (sigma * sigma);

    return (float) L_pow(2.7183f, -(x2 / twoSigma2)) / sqr2PiSigma;
}

void GetGaussianOffsets( bool bHorizontal, D3DXVECTOR2 vViewportTexelSize,
                        D3DXVECTOR4* vSampleOffsetsAndWeights )
{
    // Get the center texel offset and weight
    float fSampleWeight = 1.0f * GetGaussianDistribution( 0, 0, 2.0f );
    vSampleOffsetsAndWeights[0] = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, fSampleWeight );

    // Get the offsets and weights for the remaining taps
    static float mu = 0.0f;
    static float sigma = 2.31f;
    if( bHorizontal )
    {
        for( int i = 1; i < 15; i += 2 )
        {
            fSampleWeight = 2.0f * GetGaussianDistribution( float(i + 0), 0.0f, sigma );
            vSampleOffsetsAndWeights[i + 0] = D3DXVECTOR4(  i * vViewportTexelSize.x, 0.0f, 0.0f, fSampleWeight);

            fSampleWeight = 2.0f * GetGaussianDistribution( float(i + 1), 0.0f, sigma );
            vSampleOffsetsAndWeights[i + 1] = D3DXVECTOR4( -i * vViewportTexelSize.x, 0.0f, 0.0f, fSampleWeight);
        }
    }
    else
    {
        for( int i = 1; i < 15; i += 2 )
        {
            fSampleWeight = 2.0f * GetGaussianDistribution( 0.0f, float(i + 0), sigma );
            vSampleOffsetsAndWeights[i + 0] = D3DXVECTOR4( 0.0f,  i * vViewportTexelSize.y, 0.0f, fSampleWeight);

            fSampleWeight = 2.0f * GetGaussianDistribution( 0.0f, float(i + 1), sigma );
            vSampleOffsetsAndWeights[i + 1] = D3DXVECTOR4( 0.0f, -i * vViewportTexelSize.y, 0.0f, fSampleWeight);
        }
    }
}

void GetGaussianOffsets7x7( BOOL bHorizontal, FLOAT fViewportTexelSize,
                        D3DXVECTOR4* vSampleOffsetsAndWeights )
{
    // 7x7 Vertical Gaussian blur filter

    static float sigma = 2.0f;
    float f1 = Gaussian1D(1, sigma);
    float f2 = Gaussian1D(2, sigma);
    float f3 = Gaussian1D(3, sigma);

    if( bHorizontal )
    {
        vSampleOffsetsAndWeights[0] = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f - (f1+f2+f3)*2 );
        vSampleOffsetsAndWeights[1] = D3DXVECTOR4(  1 * fViewportTexelSize, 0.0f, 0.0f, f1);
        vSampleOffsetsAndWeights[2] = D3DXVECTOR4(  2 * fViewportTexelSize, 0.0f, 0.0f, f2);
        vSampleOffsetsAndWeights[3] = D3DXVECTOR4(  3 * fViewportTexelSize, 0.0f, 0.0f, f3);
    }
    else
    {
        vSampleOffsetsAndWeights[0] = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f - (f1+f2+f3)*2 );
        vSampleOffsetsAndWeights[1] = D3DXVECTOR4(  0.0f, 1 * fViewportTexelSize, 0.0f, f1);
        vSampleOffsetsAndWeights[2] = D3DXVECTOR4(  0.0f, 2 * fViewportTexelSize, 0.0f, f2);
        vSampleOffsetsAndWeights[3] = D3DXVECTOR4(  0.0f, 3 * fViewportTexelSize, 0.0f, f3);
    }
}

#define M_ReturnDistanceEdge(a,b)       { float x = a; float y = b; return fSqrt( x*x + y*y ); }
#define M_ReturnDistancePoint(a,b,c)    { float x = a; float y = b; float z = c; return fSqrt( x*x + y*y + z*z ); }

float 
XeDistanceCameraToBV( D3DXVECTOR3 * _vCamPos, XeMesh * _poMesh, D3DXMATRIX * pWorldMatrix )
{
    if( !_poMesh->HasBoundingVolume() )
    {
        return 0.0f;
    }

    // Compute global position of AABB 
    MATH_tdst_Vector vMin, vMax;
    _poMesh->GetAxisAlignedBoundingVolume( vMin, vMax );
    D3DXVec3Add( (D3DXVECTOR3*)&vMin, (D3DXVECTOR3*)&vMin, (D3DXVECTOR3*)&pWorldMatrix->_41 );
    D3DXVec3Add( (D3DXVECTOR3*)&vMax, (D3DXVECTOR3*)&vMax, (D3DXVECTOR3*)&pWorldMatrix->_41 );

    LONG lClassX(-1), lClassY(-1), lClassZ(-1);
    LONG nbIn = 0;

    // Classify point
    if( _vCamPos->x > vMin.x ) 
    {
        if( _vCamPos->x > vMax.x )
        {
            lClassX = 1;
        }
        else
        {
            lClassX = 0;
            ++nbIn;
        }
    }
    if( _vCamPos->y > vMin.y ) 
    {
        if( _vCamPos->y > vMax.y )
        {
            lClassY = 1;
        }
        else
        {
            lClassY = 0;
            ++nbIn;
        }
    }
    if( _vCamPos->z > vMin.z ) 
    {
        if( _vCamPos->z > vMax.z )
        {
            lClassZ = 1;
        }
        else
        {
            lClassZ = 0;
             ++nbIn;
        }
    }

    // Return distance
    if( nbIn == 3 ) return 0.0f;

    if( nbIn == 2 )
    {
        if( lClassX != 0 )
        {
            if( lClassX == -1 )
            {
                return (vMin.x - _vCamPos->x);
            }
            else
            {
                return (_vCamPos->x - vMax.x);
            }
        }
        if( lClassY != 0 )
        {
            if( lClassY == -1 )
            {
                return (vMin.y - _vCamPos->y);
            }
            else
            {
                return (_vCamPos->y - vMax.y);
            }
        }
        if( lClassZ != 0 )
        {
            if( lClassZ == -1 )
            {
                return (vMin.z - _vCamPos->z);
            }
            else
            {
                return (_vCamPos->z - vMax.z);
            }
        }
    }

    if( nbIn == 1 )
    {
        if( lClassX == 0 )
        {
            if( lClassY == -1 )
            {
                if( lClassZ == -1)
                    M_ReturnDistanceEdge( _vCamPos->y - vMin.y, _vCamPos->z - vMin.z )
                else
                    M_ReturnDistanceEdge( _vCamPos->y - vMin.y, _vCamPos->z - vMax.z )
            }
            else
            {
                if( lClassZ == -1)
                    M_ReturnDistanceEdge( _vCamPos->y - vMax.y, _vCamPos->z - vMin.z )
                else
                    M_ReturnDistanceEdge( _vCamPos->y - vMax.y, _vCamPos->z - vMax.z )
            }
        }
        if( lClassY == 0 )
        {
            if( lClassX == -1 )
            {
                if( lClassZ == -1)
                    M_ReturnDistanceEdge( _vCamPos->x - vMin.x, _vCamPos->z - vMin.z )
                else
                    M_ReturnDistanceEdge( _vCamPos->x - vMin.x, _vCamPos->z - vMax.z )
            }
            else
            {
                if( lClassZ == -1)
                    M_ReturnDistanceEdge( _vCamPos->x - vMax.x, _vCamPos->z - vMin.z )
                else
                    M_ReturnDistanceEdge( _vCamPos->x - vMax.x, _vCamPos->z - vMax.z )
            }
        }
        if( lClassZ == 0 )
        {
            if( lClassX == -1 )
            {
                if( lClassY == -1)
                    M_ReturnDistanceEdge( _vCamPos->x - vMin.x, _vCamPos->y - vMin.y )
                else
                    M_ReturnDistanceEdge( _vCamPos->x - vMin.x, _vCamPos->y - vMax.y )
            }
            else
            {
                if( lClassY == -1)
                    M_ReturnDistanceEdge( _vCamPos->x - vMax.x, _vCamPos->y - vMin.y )
                else
                    M_ReturnDistanceEdge( _vCamPos->x - vMax.x, _vCamPos->y - vMax.y )
            }
        }
    }

    if( nbIn == 0 )
    {
        if( lClassX == -1 )
        {
            if( lClassY == -1 )
            {
                if( lClassZ == -1)
                    M_ReturnDistancePoint( _vCamPos->x - vMin.x, _vCamPos->y - vMin.y, _vCamPos->z - vMin.z )
                else
                    M_ReturnDistancePoint( _vCamPos->x - vMin.x, _vCamPos->y - vMin.y, _vCamPos->z - vMax.z )
            }
            else
            {
                if( lClassZ == -1)
                    M_ReturnDistancePoint( _vCamPos->x - vMin.x, _vCamPos->y - vMax.y, _vCamPos->z - vMin.z )
                else
                    M_ReturnDistancePoint( _vCamPos->x - vMin.x, _vCamPos->y - vMax.y, _vCamPos->z - vMax.z )
            }
        }
        else
        {
            if( lClassY == -1 )
            {
                if( lClassZ == -1)
                    M_ReturnDistancePoint( _vCamPos->x - vMax.x, _vCamPos->y - vMin.y, _vCamPos->z - vMin.z )
                else
                    M_ReturnDistancePoint( _vCamPos->x - vMax.x, _vCamPos->y - vMin.y, _vCamPos->z - vMax.z )
            }
            else
            {
                if( lClassZ == -1)
                    M_ReturnDistancePoint( _vCamPos->x - vMax.x, _vCamPos->y - vMax.y, _vCamPos->z - vMin.z )
                else
                    M_ReturnDistancePoint( _vCamPos->x - vMax.x, _vCamPos->y - vMax.y, _vCamPos->z - vMax.z )
            }
        }
    }
    return 0.0f;
}

//-------------------------------------------------------
// XeConvertToD3DMatrix
//-------------------------------------------------------
void XeConvertToD3DMatrix( D3DXMATRIX & _to, const Float4x3 & _from )
{
    _to._11 = _from.m_vColumns[0].x;
    _to._21 = _from.m_vColumns[0].y;
    _to._31 = _from.m_vColumns[0].z;
    _to._41 = _from.m_vColumns[0].w;

    _to._12 = _from.m_vColumns[1].x;
    _to._22 = _from.m_vColumns[1].y;
    _to._32 = _from.m_vColumns[1].z;
    _to._42 = _from.m_vColumns[1].w;

    _to._13 = _from.m_vColumns[2].x;
    _to._23 = _from.m_vColumns[2].y;
    _to._33 = _from.m_vColumns[2].z;
    _to._43 = _from.m_vColumns[2].w;

    _to._14 = 0.0f;
    _to._24 = 0.0f;
    _to._34 = 0.0f;
    _to._44 = 1.0f;
}

//-------------------------------------------------------
// XeConvertToFloat4x3
//-------------------------------------------------------
void XeConvertToFloat4x3( Float4x3 & _to, const D3DXMATRIX & _from )
{
    _to.m_vColumns[0].x = _from._11; 
    _to.m_vColumns[0].y = _from._21;
    _to.m_vColumns[0].z = _from._31; 
    _to.m_vColumns[0].w = _from._41;

    _to.m_vColumns[1].x = _from._12; 
    _to.m_vColumns[1].y = _from._22;
    _to.m_vColumns[1].z = _from._32; 
    _to.m_vColumns[1].w = _from._42;

    _to.m_vColumns[2].x = _from._13; 
    _to.m_vColumns[2].y = _from._23;
    _to.m_vColumns[2].z = _from._33; 
    _to.m_vColumns[2].w = _from._43;
}

//-------------------------------------------------------
// XeConvertToD3DMatrix
//-------------------------------------------------------
void XeConvertToD3DMatrix( D3DXMATRIX & _to, const Float4x2 & _from )
{
    _to._11 = _from.m_vColumns[0].x;
    _to._21 = _from.m_vColumns[0].y;
    _to._31 = _from.m_vColumns[0].z;
    _to._41 = _from.m_vColumns[0].w;

    _to._12 = _from.m_vColumns[1].x;
    _to._22 = _from.m_vColumns[1].y;
    _to._32 = _from.m_vColumns[1].z;
    _to._42 = _from.m_vColumns[1].w;

    _to._13 = 0.0f;
    _to._23 = 0.0f;
    _to._33 = 1.0f;
    _to._43 = 0.0f;

    _to._14 = 0.0f;
    _to._24 = 0.0f;
    _to._34 = 0.0f;
    _to._44 = 1.0f;
}

//-------------------------------------------------------
// XeConvertToFloat4x2
//-------------------------------------------------------
void XeConvertToFloat4x2( Float4x2 & _to, const D3DXMATRIX & _from )
{
    _to.m_vColumns[0].x = _from._11; 
    _to.m_vColumns[0].y = _from._21;
    _to.m_vColumns[0].z = _from._31; 
    _to.m_vColumns[0].w = _from._41;

    _to.m_vColumns[1].x = _from._12; 
    _to.m_vColumns[1].y = _from._22;
    _to.m_vColumns[1].z = _from._32; 
    _to.m_vColumns[1].w = _from._42;
}


#ifdef ACTIVE_EDITORS
//------------------------------------------
// CLASS : XeLightUsageLogger
//------------------------------------------
XeLightUsageLogger g_oXeLightUsageLogger;

//------------------------------------------
// XeLightUsageLogger::XeLightUsageLogger
//------------------------------------------
XeLightUsageLogger::XeLightUsageLogger( ) :
m_handle                ( 0 ),
m_ulCurrentLightIndex   ( 0 ),
m_state                 ( eState_ReadyForGao )
{
}

//------------------------------------------
// XeLightUsageLogger::~XeLightUsageLogger
//------------------------------------------
XeLightUsageLogger::~XeLightUsageLogger( )
{
    Close();
}

//------------------------------------------
// XeLightUsageLogger::BeginLog
//------------------------------------------
void    
XeLightUsageLogger::BeginLog( const char * _szLogPath, const char * _szWorldName )
{
    ERR_X_Assert( m_handle == 0 );

    // Open the file for reading
    m_handle = CreateFile( _szLogPath, 
                           GENERIC_WRITE, 
                           0,
                           NULL, 
                           CREATE_ALWAYS, 
                           0,
                           NULL );

    ERR_X_Assert( m_handle != INVALID_HANDLE_VALUE );

    if( INVALID_HANDLE_VALUE == m_handle )
    {
        // Not logging
        m_handle = NULL;
        return;
    }

    // Header
    char buffer[256];
    sprintf( buffer, "Light Usage Log [%s]\n", _szWorldName );
    Write( buffer );
    Write( "---------------------------------------\n\n" );
 
    m_state = eState_ReadyForGao;
}

//------------------------------------------
// XeLightUsageLogger::EndLog
//------------------------------------------
void  
XeLightUsageLogger::EndLog( )
{
    Close( );
}

//------------------------------------------
// XeLightUsageLogger::BeginGAO
//------------------------------------------
void  
XeLightUsageLogger::BeginGAO( const char * _szObjectName )
{
    ERR_X_Assert( m_state == eState_ReadyForGao );
    m_ulCurrentLightIndex = 0;

    // Write gao name 
    char buffer[256];
    sprintf( buffer, "Game Object : %s\n", _szObjectName );
    Write( buffer );

    m_state = eState_InGao;
}

//------------------------------------------
// XeLightUsageLogger::BeginElement
//------------------------------------------
void     
XeLightUsageLogger::BeginElement( ULONG _ulElementIndex )
{
    ERR_X_Assert( m_state == eState_InGao );
    m_ulCurrentLightIndex = 0;

    // Write gao name 
    char buffer[256];
    sprintf( buffer, "    Mesh %3d: \n",_ulElementIndex );
    Write( buffer );

    m_state = eState_InElement;
}

//------------------------------------------
// XeLightUsageLogger::EndElement
//------------------------------------------
void       
XeLightUsageLogger::EndElement( )
{
    ERR_X_Assert( m_state == eState_InElement );
    Write("\n");
    m_state = eState_InGao;
}

//------------------------------------------
// XeLightUsageLogger::EndGAO
//------------------------------------------
void        
XeLightUsageLogger::EndGAO( )
{
    Write("\n\n");
    m_state = eState_ReadyForGao;
}

//------------------------------------------
// XeLightUsageLogger::AddLight
//------------------------------------------
void    
XeLightUsageLogger::AddLight( const char * _szLightName )
{
    ERR_X_Assert( m_state == eState_InGao || m_state == eState_InElement );

    char buffer[256];
    sprintf( buffer, "    %sLight [%2d] : %s\n",  m_state == eState_InGao ? "" : "    ", m_ulCurrentLightIndex, _szLightName );
    Write( buffer );
    m_ulCurrentLightIndex++;
}

//------------------------------------------
// XeLightUsageLogger::Write
//------------------------------------------
void     
XeLightUsageLogger::Write( const char * _szLine )
{
    ERR_X_Assert( m_handle != 0 && _szLine != NULL );
    DWORD dwBytesWritten = 0;
    BOOL result = WriteFile( m_handle, _szLine, strlen(_szLine), &dwBytesWritten, NULL );
    ERR_X_Assert( result != 0 );
}

//------------------------------------------
// XeLightUsageLogger::Close
//------------------------------------------
void  
XeLightUsageLogger::Close( )
{
    if( m_handle != 0 )
    {
        CloseHandle( m_handle );
        m_handle = 0;
    }
}

#endif // ACTIVE_EDITORS

#if defined(ACTIVE_EDITORS)
// ------------------------------------------------------------------------------------------------
// Name   : XeEdModifyColor
// Params : _pColor		 : Color vector to modify
//			_ulQMFRFlags : Flags to test
// RetVal : 
// Descr. : Test for Highlight mode in EDItor
// ------------------------------------------------------------------------------------------------
void XeEdModifyColor(VECTOR4FLOAT* _pColor, ULONG _ulQMFRFlags)
{
	switch (XeGetQMFREditorInfo(_ulQMFRFlags))
	{
	case QMFR_EDINFO_COLOR_01:
		XeJadeColorToV4F(_pColor, GDI_gpst_CurDD->st_HighlightColors.ul_Colors[0]);
		break;
	case QMFR_EDINFO_COLOR_02:
		XeJadeColorToV4F(_pColor, GDI_gpst_CurDD->st_HighlightColors.ul_Colors[1]);
		break;
	case QMFR_EDINFO_COLOR_03:
		XeJadeColorToV4F(_pColor, GDI_gpst_CurDD->st_HighlightColors.ul_Colors[2]);
		break;
	case QMFR_EDINFO_COLOR_04:
		XeJadeColorToV4F(_pColor, GDI_gpst_CurDD->st_HighlightColors.ul_Colors[3]);
		break;
	case QMFR_EDINFO_COLOR_05:
		XeJadeColorToV4F(_pColor, GDI_gpst_CurDD->st_HighlightColors.ul_Colors[4]);
		break;
	default:
		break;
	}
}
#endif

// ------------------------------------------------------------------------------------------------
// Name   : GetScreenShotDirName
// Params : Pointer to string
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void GetScreenShotDirName(char* _sz_ScreenshotDirName)
{
	if(WOR_gpst_CurrentWorld)
	{
		sprintf(_sz_ScreenshotDirName, "%s\\[%08x]", XE_SCREENSHOT_DIR_NAME, WOR_gpst_CurrentWorld->h_WorldKey);
	}
	else
	{
		strcpy(_sz_ScreenshotDirName, XE_SCREENSHOT_DIR_NAME);
	}
}

// ------------------------------------------------------------------------------------------------
// Name   : GetScreenShotFileName
// Params : Pointer to string
//		  : FileFormat Descriptor
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void GetScreenShotFileName(char* _sz_ScreenshotFileName, D3DXIMAGE_FILEFORMAT _FileFormat)
{
	char sz_FileName[260];

	GetScreenShotDirName(_sz_ScreenshotFileName);
	strcat(_sz_ScreenshotFileName, "\\");
	
	GetGenericFileName(sz_FileName);
	strcat(_sz_ScreenshotFileName, sz_FileName);

	switch(_FileFormat)
	{
	case D3DXIFF_PNG:
		strcat(_sz_ScreenshotFileName, ".png");
		break;
	default:
		break;
	}
}

// ------------------------------------------------------------------------------------------------
// Name   : GetProfilingDirName
// Params : Pointer to string
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void GetProfilingDirName(char* _sz_ProfilingDirName)
{
	if(WOR_gpst_CurrentWorld)
	{
		sprintf(_sz_ProfilingDirName, "%s\\[%08x]", XE_PROFILING_DIR_NAME, WOR_gpst_CurrentWorld->h_WorldKey);
	}
	else
	{
		strcpy(_sz_ProfilingDirName, XE_PROFILING_DIR_NAME);
	}}

// ------------------------------------------------------------------------------------------------
// Name   : GetProfilingFileName
// Params : Pointer to string
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void GetProfilingFileName(char* _sz_ProfilingFileName)
{
	char sz_FileName[260];

	GetProfilingDirName(_sz_ProfilingFileName);
	strcat(_sz_ProfilingFileName, "\\");

	GetGenericFileName(sz_FileName);
	strcat(_sz_ProfilingFileName, sz_FileName);

	strcat(_sz_ProfilingFileName, ".cap");
}
// ------------------------------------------------------------------------------------------------
// Name   : GetGenericFileName
// Params : Pointer to string
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void GetGenericFileName(char* _sz_FileName)
{
	SYSTEMTIME LocalSysTime;
	GetLocalTime(&LocalSysTime);

	if(WOR_gpst_CurrentWorld)
	{
		sprintf(_sz_FileName, "%02d-%02d-%02d[%08x]-F%06d", LocalSysTime.wYear, LocalSysTime.wMonth, LocalSysTime.wDay,
		WOR_gpst_CurrentWorld->h_WorldKey, (int)g_oXeRenderer.GetFrameCounter());
	}
	else
	{
		sprintf(_sz_FileName, "d:\\%02d-%02d-%02d-F%06d", LocalSysTime.wYear, LocalSysTime.wMonth, LocalSysTime.wDay,
				(int)g_oXeRenderer.GetFrameCounter());
	}
}

// ------------------------------------------------------------------------------------------------
// Name   : XeCreateDirectory
// Params : Pointer to string
// RetVal : 
// Descr. : recursively parse path, test if subdir exist and create it if not
// ------------------------------------------------------------------------------------------------
BOOL XeCreateDirectory(char* _szDirName)
{
	int i_Res;
    std::string strSubPath;
	std::string strPath = _szDirName;
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	BOOL b_Res;
	
	//find first ':'
	i_Res = strPath.find(":");
	if(i_Res == -1)
		return FALSE;

	//find first '\'
	i_Res = strPath.find("\\", i_Res+1);
	if(i_Res == -1)
		return FALSE;

	//find next '\'
	i_Res = strPath.find("\\", i_Res+1);

	//test sub dir
	while(i_Res != -1)
	{
		strSubPath = strPath.substr(0, i_Res);
		hFind = FindFirstFile(strSubPath.c_str(), &wfd);
		if(hFind == INVALID_HANDLE_VALUE)
		{
			b_Res = CreateDirectory(strSubPath.c_str(), NULL);
			if(!b_Res)
				return FALSE;
		}

		//find next '\'
		i_Res = strPath.find("\\", i_Res+1);
	}

	//test full path
	hFind = FindFirstFile(strPath.c_str(), &wfd);
	if(hFind == INVALID_HANDLE_VALUE)
	{
		b_Res = CreateDirectory(strPath.c_str(), NULL);
		if(!b_Res)
			return FALSE;
	}

	return TRUE;
}