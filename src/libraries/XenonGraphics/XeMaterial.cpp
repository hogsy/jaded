// ------------------------------------------------------------------------------------------------
// File   : XeMaterial.cpp
// Date   : 2004-12-21
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "Precomp.h"

#include "XeMaterial.h"
#include "XeRenderer.h"
#include "XeRenderObject.h"
#include "XeShader.h"

// ------------------------------------------------------------------------------------------------
// DEFINITIONS
// ------------------------------------------------------------------------------------------------
// Define to enable material tracing
#if defined(_DEBUG)
#define XE_TRACE_MATERIALS
#endif

// SC: Material debugging information enable
//#define XE_DEBUG_MATERIALS

#define XEMATERIAL_ANGLE_EPSILON 0.005f

// ------------------------------------------------------------------------------------------------
// EXTERNAL VARIABLES
// ------------------------------------------------------------------------------------------------
// Current time
extern float TIM_gf_MainClock;

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterial_CreateCustom
// Params : None
// RetVal : Xenon material
// Descr. : Create a custom Xenon material
// ------------------------------------------------------------------------------------------------
XeMaterial* XeMaterial_Create(void)
{
    XeMaterial* pst_Mat = new XeMaterial();

    return pst_Mat;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterial_CreateFromJade
// Params : _pst_JadeMaterial : Jade multitexture material
//          _ul_LayerIndex    : Layer index within the multitexture
// RetVal : Xenon material
// Descr. : Create a Xenon material from a Jade multitexture material
// ------------------------------------------------------------------------------------------------
XeMaterial* XeMaterial_CreateFromJade(MAT_tdst_MultiTexture_* _pst_JadeMaterial, 
                                      MAT_tdst_MTLevel_*      _pst_JadeMTLevel)
{
    ERR_X_Assert(_pst_JadeMaterial != NULL);
    ERR_X_Assert(_pst_JadeMTLevel != NULL);

    XeMaterial* pst_Mat = new XeMaterial(_pst_JadeMaterial, _pst_JadeMTLevel);

    return pst_Mat;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterial_Destroy
// Params : _pst_Mat : Material to destroy
// RetVal : None
// Descr. : Destroy a Xenon material
// ------------------------------------------------------------------------------------------------
void XeMaterial_Destroy(XeMaterial* _pst_Mat)
{
    SAFE_DELETE(_pst_Mat);
}


// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - XeMaterial
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterial::XeMaterial
// Params : None
// RetVal : None
// Descr. : Destructor
// ------------------------------------------------------------------------------------------------
XeMaterial::XeMaterial(MAT_tdst_MultiTexture_* _pst_JadeMaterial,
                       MAT_tdst_MTLevel_*      _pst_JadeMTLevel)
{
    ReInit(_pst_JadeMaterial, _pst_JadeMTLevel);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterial::ReInit
// Params : _pst_JadeMaterial : Jade material, NULL for custom Xenon material
//          _ul_LayerIndex    : MT level index
// RetVal : None
// Descr. : Constructor
// ------------------------------------------------------------------------------------------------
void XeMaterial::ReInit(MAT_tdst_MultiTexture_* _pst_JadeMaterial,
                        MAT_tdst_MTLevel_*      _pst_JadeMTLevel)
{
    // Clear the custom shaders & features
    SetCustomVS(CUSTOM_VS_INVALID);
	SetCustomPS(CUSTOM_PS_INVALID);

    if (_pst_JadeMaterial != NULL)
    {
        m_stInfo.ul_RawFlags      = 0;
        m_stInfo.b_IsJadeMaterial = TRUE;
        m_pstJadeMaterial         = _pst_JadeMaterial;
        m_pstJadeMTLevel          = _pst_JadeMTLevel;

#if defined(XE_TRACE_MATERIALS)
        if (m_pstJadeMTLevel == NULL)
        {
            ERR_OutputDebugString("XeMaterial::XeMaterial() - Invalid layer for a material, using default material\n");
        }
#endif
    }

    if ((_pst_JadeMaterial == NULL) || (m_pstJadeMTLevel == NULL))
    {
        m_stInfo.ul_RawFlags      = 0;
        m_stInfo.b_IsJadeMaterial = FALSE;
        m_pstProperties   = new XeMaterialProperties();
    }

#if defined(XE_DEBUG_MATERIALS)
    ERR_OutputDebugString("[XeMaterial] Creation of 0x%08x (%s material)\n", (ULONG)this, (m_stInfo.b_IsJadeMaterial ? "Jade" : "Custom"));
#endif
}

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterial::~XeMaterial
// Params : None
// RetVal : None
// Descr. : Destructor
// ------------------------------------------------------------------------------------------------
XeMaterial::~XeMaterial(void)
{
    if (!m_stInfo.b_IsJadeMaterial && !m_stInfo.b_IsACopy)
    {
        SAFE_DELETE(m_pstProperties);
    }

#if defined(XE_DEBUG_MATERIALS)
    ERR_OutputDebugString("[XeMaterial] Destruction of 0x%08x\n", (ULONG)this);
#endif
}

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterial::AddTextureStage
// Params : None
// RetVal : Stage index
// Descr. : Add a texture stage
// ------------------------------------------------------------------------------------------------
ULONG XeMaterial::AddTextureStage(void)
{
    ERR_X_Assert(!m_stInfo.b_IsJadeMaterial);

	XeMaterialStageProperties* pstStage = &m_pstProperties->ast_Stages[m_pstProperties->ul_StagesStackDepth];

    // Sampler states
    pstStage->e_States.ClearAll();
    pstStage->e_States.ul_AddressU         = D3DTADDRESS_CLAMP;
    pstStage->e_States.ul_AddressV         = D3DTADDRESS_CLAMP;
    pstStage->e_States.ul_AddressW         = D3DTADDRESS_CLAMP;
    pstStage->e_States.ul_MagFilter        = D3DTEXF_LINEAR;
    pstStage->e_States.ul_MinFilter        = D3DTEXF_LINEAR;
    pstStage->e_States.ul_MipFilter        = D3DTEXF_LINEAR;
    pstStage->e_States.b_TexCoordTransform = FALSE;
    pstStage->ul_BorderColor               = 0;
    pstStage->f_MipmapLODBias              = 0.0f;

    D3DXMatrixIdentity(&(pstStage->st_Transform));

    return m_pstProperties->ul_StagesStackDepth++;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterial::ComputeJadeTexCoordMatrix
// Params : _pst_Matrix : Resulting matrix [out]
// RetVal : None
// Descr. : Compute the texture coordinate transformation matrix
// ------------------------------------------------------------------------------------------------
void XeMaterial::ComputeJadeTexCoordMatrix(D3DXMATRIX* _pst_Matrix) const
{
    ERR_X_Assert(_pst_Matrix != NULL);
    ERR_X_Assert(m_stInfo.b_IsJadeMaterial);

    MAT_tdst_Decompressed_UVMatrix stUVMatrix;

    if (((m_pstJadeMTLevel->ScaleSPeedPosU != 0) && (m_pstJadeMTLevel->ScaleSPeedPosU != MAT_Cc_Identity)) ||
        ((m_pstJadeMTLevel->ScaleSPeedPosV != 0) && (m_pstJadeMTLevel->ScaleSPeedPosV != MAT_Cc_Identity)))
    {
        MAT_VUDecompress(m_pstJadeMTLevel, &stUVMatrix);

        XeSetD3DXMatrix(_pst_Matrix,
                        stUVMatrix.UVMatrix[0], stUVMatrix.UVMatrix[1], 0.0f, 0.0f,
                        stUVMatrix.UVMatrix[2], stUVMatrix.UVMatrix[3], 0.0f, 0.0f,
                        0.0f,                   0.0f,                   1.0f, 0.0f,
                        stUVMatrix.AddU,        stUVMatrix.AddV,        0.0f, 1.0f);
    }
    else
    {
        XeSetD3DXMatrix(_pst_Matrix,
                        1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterial::ComputeJadeChromeMatrix
// Params : _pst_Matrix : Resulting matrix [out]
// RetVal : None
// Descr. : Compute the chrome texture coordinate transformation matrix
// ------------------------------------------------------------------------------------------------
void XeMaterial::ComputeJadeChromeMatrix(D3DXMATRIX* _pst_Matrix) const
{
    ERR_X_Assert(_pst_Matrix != NULL);
    ERR_X_Assert(m_stInfo.b_IsJadeMaterial);

    XeSetD3DXMatrix(_pst_Matrix,
                    0.5f,  0.0f, 0.0f, 0.0f,
                    0.0f, -0.5f, 0.0f, 0.0f,
                    0.0f,  0.0f, 1.0f, 0.0f,
                    0.5f,  0.5f, 0.0f, 1.0f);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterial::ComputeJadePlanarGizmoMatrix
// Params : _pst_Matrix : Resulting matrix [out]
// RetVal : None
// Descr. : Compute the planar gizmo texture coordinate transformation matrix
// ------------------------------------------------------------------------------------------------
void XeMaterial::ComputeJadePlanarGizmoMatrix(D3DXMATRIX* _pst_Matrix, ULONG _ulWorldIndex ) const
{
    ERR_X_Assert(_pst_Matrix != NULL);
    ERR_X_Assert(m_stInfo.b_IsJadeMaterial);

    D3DXMATRIX* pstObjMatrix;

    if (_ulWorldIndex == 0xffffffff)
    {
        XeRenderObject* pRenderObject = g_pXeContextManagerRender->GetCurrentRenderObject();
        ERR_X_Assert(pRenderObject != NULL);

        pstObjMatrix = pRenderObject->GetWorldMatrix();
    }
    else
    {
        pstObjMatrix = g_pXeContextManagerRender->GetWorldMatrixByIndex(_ulWorldIndex);
    }

    D3DXMATRIX stGizmoMatrix;

    if(MAT_GET_MatrixFrom(m_pstJadeMTLevel->s_AditionalFlags) == MAT_CC_OBJECT)
    {
        switch (MAT_GET_XYZ(m_pstJadeMTLevel->s_AditionalFlags))
        {
        case MAT_CC_X:
            XeSetD3DXMatrix(&stGizmoMatrix, 
                0.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
            break;

        case MAT_CC_Y:
            XeSetD3DXMatrix(&stGizmoMatrix, 
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
            break;

        case MAT_CC_Z:
            XeSetD3DXMatrix(&stGizmoMatrix, 
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
            break;

        case MAT_CC_XYZ:
            XeSetD3DXMatrix(&stGizmoMatrix, 
                Cf_Sqrt2 * 0.5f,    -Cf_InvSqrt3,   0.0f,           0.0f,
                -Cf_Sqrt2 * 0.5f,   -Cf_InvSqrt3,   0.0f,           0.0f,
                0.0f,               Cf_InvSqrt3,    0.0f,           0.0f,
                0.0f,               0.0f,           0.0f,           1.0f);
            break;
        }
    }
    else if(MAT_GET_MatrixFrom(m_pstJadeMTLevel->s_AditionalFlags) == MAT_CC_GIZMO)
    {
        XeRenderObject* pRenderObject = g_pXeContextManagerRender->GetCurrentRenderObject();
        ERR_X_Assert(pRenderObject != NULL);

        stGizmoMatrix = *g_pXeContextManagerRender->GetPlanarGizmoMatrixByIndex( pRenderObject->GetPlanarGizmoMatrixIndex() );
    }
    else
    {
        switch (MAT_GET_XYZ(m_pstJadeMTLevel->s_AditionalFlags))
        {
            case MAT_CC_X:
                XeSetD3DXMatrix(&stGizmoMatrix, 
                                pstObjMatrix->_12, pstObjMatrix->_13, 0.0f, 0.0f,
                                pstObjMatrix->_22, pstObjMatrix->_23, 0.0f, 0.0f,
                                pstObjMatrix->_32, pstObjMatrix->_33, 0.0f, 0.0f,
                                pstObjMatrix->_42, pstObjMatrix->_43, 0.0f, 1.0f);
                break;

            case MAT_CC_Y:
                XeSetD3DXMatrix(&stGizmoMatrix, 
                                pstObjMatrix->_11, pstObjMatrix->_13, 0.0f, 0.0f,
                                pstObjMatrix->_21, pstObjMatrix->_23, 0.0f, 0.0f,
                                pstObjMatrix->_31, pstObjMatrix->_33, 0.0f, 0.0f,
                                pstObjMatrix->_41, pstObjMatrix->_43, 0.0f, 1.0f);
                break;

            case MAT_CC_Z:
                XeSetD3DXMatrix(&stGizmoMatrix, 
                                pstObjMatrix->_11, pstObjMatrix->_12, 0.0f, 0.0f,
                                pstObjMatrix->_21, pstObjMatrix->_22, 0.0f, 0.0f,
                                pstObjMatrix->_31, pstObjMatrix->_32, 0.0f, 0.0f,
                                pstObjMatrix->_41, pstObjMatrix->_42, 0.0f, 1.0f);
                break;

            case MAT_CC_XYZ:
                MATH_tdst_Vector stU,stV,stUC,stVC ;
                FLOAT            fTransX;
                FLOAT            fTransY;

                MATH_InitVector(&stUC , Cf_Sqrt2 * 0.5f , -Cf_Sqrt2 * 0.5f , 0.0f);
                MATH_InitVector(&stVC , -Cf_InvSqrt3 , -Cf_InvSqrt3 , Cf_InvSqrt3);

                fTransX = stUC.x * pstObjMatrix->_41 + stUC.y * pstObjMatrix->_42 + stUC.z * pstObjMatrix->_43;
                fTransY = stVC.x * pstObjMatrix->_41 + stVC.y * pstObjMatrix->_42 + stVC.z * pstObjMatrix->_43;

                stU.x = pstObjMatrix->_11 * stUC.x + pstObjMatrix->_21 * stUC.y + pstObjMatrix->_31 * stUC.z ;
                stU.y = pstObjMatrix->_12 * stUC.x + pstObjMatrix->_22 * stUC.y + pstObjMatrix->_32 * stUC.z ;
                stU.z = pstObjMatrix->_13 * stUC.x + pstObjMatrix->_23 * stUC.y + pstObjMatrix->_33 * stUC.z ;
                stV.x = pstObjMatrix->_11 * stVC.x + pstObjMatrix->_21 * stVC.y + pstObjMatrix->_31 * stVC.z ;
                stV.y = pstObjMatrix->_12 * stVC.x + pstObjMatrix->_22 * stVC.y + pstObjMatrix->_32 * stVC.z ;
                stV.z = pstObjMatrix->_13 * stVC.x + pstObjMatrix->_23 * stVC.y + pstObjMatrix->_33 * stVC.z ;

                XeSetD3DXMatrix(&stGizmoMatrix,
                                stU.x,   stV.x,   0.0f, 0.0f,
                                stU.y,   stV.y,   0.0f, 0.0f,
                                stU.z,   stV.z,   0.0f, 0.0f,
                                fTransX, fTransY, 0.0f, 1.0f);
                break;
        }
    }

    if ((m_pstJadeMTLevel->ScaleSPeedPosU != 0) || (m_pstJadeMTLevel->ScaleSPeedPosV != 0))
    {
        D3DXMATRIX stTexTransform;

        ComputeJadeTexCoordMatrix(&stTexTransform);

        D3DXMatrixMultiply(_pst_Matrix, &stGizmoMatrix, &stTexTransform);
    }
    else
    {
        *_pst_Matrix = stGizmoMatrix;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterial::ComputeTransform
// Params : _pst_Matrix  : Matrix
//          _stTransform : Transform
//          _bAbsolute   : Absolute transform
//          _bScrollU    : Enable U scrolling
//          _bScrollV    : Enable V scrolling
//          _bEnabled    : Transform is enabled for this stage
// RetVal : None
// Descr. : Compute the transform
// ------------------------------------------------------------------------------------------------
void XeMaterial::ComputeTransform(D3DXMATRIX*                 _pst_Matrix, 
                                  const MAT_tdst_XeTransform& _stTransform,
                                  BOOL                        _bAbsolute, 
                                  BOOL                        _bScrollU, 
                                  BOOL                        _bScrollV,
                                  ULONG                       _ulWorldMatrixIndex,
                                  BOOL                        _bEnabled) const
{
    ERR_X_Assert(_pst_Matrix != NULL);

    FLOAT fAngle  = _stTransform.f_Angle + (TIM_gf_MainClock * _stTransform.f_RollSpeed);
    FLOAT fScaleU = _stTransform.f_ScaleU;
    FLOAT fScaleV = _stTransform.f_ScaleV;
    FLOAT fTransU = _stTransform.f_StartU;
    FLOAT fTransV = _stTransform.f_StartV;

    if (_bScrollU)
    {
        fTransU *= TIM_gf_MainClock;
    }

    if (_bScrollV)
    {
        fTransV *= TIM_gf_MainClock;
    }

    if (fScaleU == 0.0f)
    {
        fScaleU = 1.0f;
    }

    if (fScaleV == 0.0f)
    {
        fScaleV = 1.0f;
    }

    fTransU = fmodf(fTransU, 1.0f);
    fTransV = fmodf(fTransV, 1.0f);

    // Scale + Translation
    XeSetD3DXMatrix(_pst_Matrix,
                    fScaleU, 0.0f,    0.0f, 0.0f,
                    0.0f,    fScaleV, 0.0f, 0.0f,
                    0.0f,    0.0f,    1.0f, 0.0f,
                    fTransU, fTransV, 0.0f, 1.0f);

    if (!fNulWithEpsilon(fAngle, XEMATERIAL_ANGLE_EPSILON))
    {
        D3DXMATRIX mRot;

        D3DXMatrixRotationZ(&mRot, -fAngle * 3.14159265358979f / 180.0f);
        D3DXMatrixMultiply(_pst_Matrix, &mRot, _pst_Matrix);
    }

#if defined(ACTIVE_EDITORS)
    // Reset the transform if it is not enabled
    if (!_bEnabled)
    {
        D3DXMatrixIdentity(_pst_Matrix);
    }
#endif

    if (!_bAbsolute)
    {
        D3DXMATRIX mBase;
        GetTransform(TEXTURE_BASE, &mBase, _ulWorldMatrixIndex);
        D3DXMatrixMultiply(_pst_Matrix, &mBase, _pst_Matrix);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterial::ComputeTangentTransform
// Params : _pst_Matrix : Tangent transform matrix [out]
// RetVal : None
// Descr. : Compute the tangent space transformation matrix
// ------------------------------------------------------------------------------------------------
void XeMaterial::ComputeTangentTransform(D3DXMATRIX* _pst_Matrix) const
{
    ERR_X_Assert(_pst_Matrix != NULL);

    FLOAT fAngle = 0.0f;

    fAngle += 360.0f * MAT_GetRotation(m_pstJadeMTLevel->ScaleSPeedPosU, m_pstJadeMTLevel->ScaleSPeedPosV);

    // Add the angle from the normal map transform
    if ((m_pstJadeMTLevel->pst_XeLevel != NULL) &&
        (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapTransform))
    {
        fAngle += m_pstJadeMTLevel->pst_XeLevel->st_NormalMapTransform.f_Angle;
        fAngle += TIM_gf_MainClock * m_pstJadeMTLevel->pst_XeLevel->st_NormalMapTransform.f_RollSpeed;
    }

    D3DXMatrixRotationZ(_pst_Matrix, fAngle * 3.14159265358979f / 180.0f);
}

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - XeMaterialProperties
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterialProperties::XeMaterialProperties
// Params : None
// RetVal : None
// Descr. : Constructor
// ------------------------------------------------------------------------------------------------
XeMaterialProperties::XeMaterialProperties(void)
{
    // Render states
    e_RenderStates.ClearAll();
    e_RenderStates.b_AlphaWriteEnable = 1;
    e_RenderStates.b_ColorWriteEnable = 1;
    e_RenderStates.b_ZEnable          = 1;
    e_RenderStates.b_ZWriteEnable     = 1;
    e_RenderStates.b_TwoSided         = 1;
    ul_AlphaRef    = 0;
    ul_LocalAlpha  = 0;
    ul_SrcBlend    = D3DBLEND_ONE;
    ul_DestBlend   = D3DBLEND_ZERO;
    ul_ZFunc       = D3DCMP_LESSEQUAL;
    ul_ColorSource = MAT_Cc_ColorOp_RLI;
    ul_UVSource    = MAT_Cc_UV_Object1;
    ul_BlendOp     = D3DBLENDOP_ADD;

    // Material properties
    ul_Constant = 0;
    ul_Ambient  = 0;
    ul_Diffuse  = 0;
    ul_Specular = 0;
    f_Shininess = 0.0f;

	// Initialize Stage Stack

	for (int iLoop = 0; iLoop < 8; iLoop++)
	{
		ast_Stages[iLoop].l_TextureId = MAT_Xe_InvalidTextureId;
	}

	ul_StagesStackDepth = 0;
}

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - XeMaterialConstants
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterialConstants::XeMaterialConstants
// Params : None
// RetVal : None
// Descr. : Constructor
// ------------------------------------------------------------------------------------------------
XeMaterialConstants::XeMaterialConstants(void)
{
    f_SpecularBias   = 0.0f;
    ul_EnvMapColor   = 0;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeMaterialConstants::~XeMaterialConstants
// Params : None
// RetVal : None
// Descr. : Destructor
// ------------------------------------------------------------------------------------------------
XeMaterialConstants::~XeMaterialConstants(void)
{
}
