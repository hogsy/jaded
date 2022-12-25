#include "Precomp.h"
#include "XeRenderObject.h"
#include "XeVertexShaderManager.h"
#include "XeShader.h"

// ------------------------------------------------------------------------------------------------
// XeRenderObject
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderObject::XeRenderObject
// Params : None
// RetVal : None
// Descr. : Constructor
// ------------------------------------------------------------------------------------------------
XeRenderObject::XeRenderObject()
{
    ReInit();
}

void XeRenderObject::ReInit()
{
    m_poMaterial = NULL;
    m_poMesh     = NULL;
    m_ulDrawMask = 0;
    m_ePrimitiveType = D3DPT_TRIANGLELIST;
    m_pUserData = NULL;
    SetCustomVS(CUSTOM_VS_INVALID);
    m_ucLOD = 255;
    m_ulWorldMatrixIndex = 0;
    m_ulWorldViewMatrixIndex = 0;
    m_ulProjMatrixIndex = 0;
    m_ulShadowWorldViewMatrixIndex = 0;
    m_ulShadowProjMatrixIndex = 0;
    L_memset(&m_oSkinningSet, 0, sizeof(XeSkinningSet));
    m_ulLightSetIndex = 0;
    m_ulWaterPatchSetIndex = 0;
    m_ulExtraFlags = 0;
    m_ulExtraDataIndex = 0;
    m_iVisQueryIndex = -1;
    m_iWYBParamsIndex1 = -1;
    m_iWYBParamsIndex2 = -1;
    m_ulSymmetryIndex = 0;
    m_ulPlanarGizmoMatrixIndex = 0;
    m_ulMaterialLODDetailState = MatLODFull;
    m_fMaterialLODDetailBlend = 0.0f;
    m_ulMaterialLODState = MatLODFull;
    m_fMaterialLODBlend = 0.0f;


    SetObjectType(Common);
}

void XeRenderObject::SetPrimType(XEPRIMITIVETYPE _ePrimType)
{
    switch(_ePrimType)
    {
        case TriangleList:
            m_ePrimitiveType = D3DPT_TRIANGLELIST;
    	    break;

        case PointList:
            m_ePrimitiveType = D3DPT_POINTLIST;
    	    break;

        case QuadList:
            m_ePrimitiveType = D3DPT_QUADLIST;
            break;

		case TriangleStrip:
			m_ePrimitiveType = D3DPT_TRIANGLESTRIP;
			break;

#if defined(ACTIVE_EDITORS)
        case LineList:
            m_ePrimitiveType = D3DPT_LINELIST;
            break;
#endif
        case LineStrip:
            m_ePrimitiveType = D3DPT_LINESTRIP;
            break;

        default:
            // Invalid primitive type
            ERR_X_Assert(0);
            break;
    }
}

UINT XeRenderObject::GetPrimitiveCount()
{
    XeMesh*        poMesh           = GetMesh();
    XeIndexBuffer* poIndices        = poMesh->GetIndices();
    XeBuffer*      poVertexBuffer   = poMesh->GetStream(0)->pBuffer;

    if (poIndices != NULL)
    {
        // Indexed case
        return poIndices->GetFaceCount();
    }
    else
    {
        // Non-indexed case
        switch(GetPrimType())
        {
            case D3DPT_TRIANGLELIST:
                return poVertexBuffer->GetVertexCount() / 3;
                break;

            case D3DPT_POINTLIST:
                return poVertexBuffer->GetVertexCount();
                break;

            case D3DPT_QUADLIST:
#if defined(_XENON_RENDER_PC)
                return poVertexBuffer->GetVertexCount() / 2;
#else
                return poVertexBuffer->GetVertexCount() / 4;
#endif
                break;

			case D3DPT_TRIANGLESTRIP:
				return poVertexBuffer->GetVertexCount() - 2;
				break;

#if defined(ACTIVE_EDITORS)
            case D3DPT_LINELIST:
                return poVertexBuffer->GetVertexCount() / 2;
                break;
#endif
            case D3DPT_LINESTRIP:
                return poVertexBuffer->GetVertexCount() - 1;
                break;

            default:
                ERR_X_Error(0, "Unknown primitive type", NULL);
                break;
        }
    }

    // Should never get here.
    ERR_X_Error(0, "Invalid case", NULL);

    return 0;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBuffer::XeRenderObject
// Params : None
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeRenderObject::SetFaceCount(unsigned long ulNewFaceCount)
{
    XeMesh*        poMesh           = GetMesh();
    XeIndexBuffer* poIndices        = poMesh->GetIndices();
    XeBuffer*      poVertexBuffer   = poMesh->GetStream(0)->pBuffer;

    if (poIndices != NULL)
    {
        // Indexed case
        return poIndices->SetFaceCount(ulNewFaceCount);
    }
    else
    {
        switch(GetPrimType())
        {
        case D3DPT_TRIANGLELIST:
            poVertexBuffer->SetVertexCount(ulNewFaceCount * 3);
            break;

        case D3DPT_POINTLIST:
            poVertexBuffer->SetVertexCount(ulNewFaceCount);
            break;

        case D3DPT_QUADLIST:
#if defined(_XENON_RENDER_PC)
            poVertexBuffer->SetVertexCount(ulNewFaceCount * 2);
#else
            poVertexBuffer->SetVertexCount(ulNewFaceCount * 4);
#endif
            break;

        case D3DPT_TRIANGLESTRIP:
            poVertexBuffer->SetVertexCount(ulNewFaceCount + 2);
            break;

#if defined(ACTIVE_EDITORS)
        case D3DPT_LINELIST:
            poVertexBuffer->SetVertexCount(ulNewFaceCount * 2);
            break;
#endif
        case D3DPT_LINESTRIP:
            poVertexBuffer->SetVertexCount(ulNewFaceCount + 1);
            break;

        default:
            // Invalid primitive type
            ERR_X_Assert(0);
            break;
        }    
    }
}