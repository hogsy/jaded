// ------------------------------------------------------------------------------------------------
// File   : XeMaterial.inl
// Date   : 2004-12-21
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - XeMaterial
// ------------------------------------------------------------------------------------------------

inline bool XeMaterial::IsJadeMaterial(void)
{
    return (m_stInfo.b_IsJadeMaterial != 0);
}

inline bool XeMaterial::IsJadeExtendedMaterial(void)
{
    return ((m_stInfo.b_IsJadeMaterial != 0) && (m_pstJadeMTLevel->pst_XeLevel != NULL));
}

inline ULONG XeMaterial::GetJadeFlags(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return m_pstJadeMTLevel->ul_Flags;
    }

    return 0xffffffff;
}

inline void XeMaterial::SetJadeFlags(ULONG _ulFlags)
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        m_pstJadeMTLevel->ul_Flags = _ulFlags;
    }
}

inline BOOL XeMaterial::IsTwoSided(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel != NULL)
        {
            return ((m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_TwoSided) != 0);
        }
    }
    else
    {
        return m_pstProperties->e_RenderStates.b_TwoSided;
    }

    return FALSE;
}

inline BOOL XeMaterial::IsReflected(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel != NULL)
        {
            return ((m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_ReflectOnWater) != 0);
        }
    }
    else
    {
        return m_pstProperties->e_RenderStates.b_ReflectOnWater;
    }

    return FALSE;
}

inline BOOL XeMaterial::IsAlphaTestEnabled(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return ((m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_AlphaTest) != 0);
    }
    else
    {
        return m_pstProperties->e_RenderStates.b_AlphaTestEnable;
    }
}

inline BOOL XeMaterial::IsAlphaInverted(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return ((m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_InvertAlpha) != 0);
    }
    else
    {
        return m_pstProperties->e_RenderStates.b_InvertAlpha;
    }
}

inline BOOL XeMaterial::IsUsingLocalAlpha(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return ((m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_UseLocalAlpha) != 0);
    }
    else
    {
        return m_pstProperties->e_RenderStates.b_UsingLocalAlpha;
    }
}

inline BOOL XeMaterial::IsAlphaBlendEnabled(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if ((MAT_GET_Blending(m_pstJadeMTLevel->ul_Flags) != MAT_Cc_Op_Copy) && 
            (MAT_GET_Blending(m_pstJadeMTLevel->ul_Flags) != MAT_Cc_Op_Glow))
            return TRUE;

        return FALSE;
    }
    else
    {
        return m_pstProperties->e_RenderStates.b_AlphaBlendEnable;
    }
}

inline BOOL XeMaterial::IsAlphaWriteEnabled(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return ((m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_HideAlpha) == 0);
    }
    else
    {
        return m_pstProperties->e_RenderStates.b_AlphaWriteEnable;
    }
}

inline BOOL XeMaterial::IsColorWriteEnabled(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return ((m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_HideColor) == 0);
    }
    else
    {
        return m_pstProperties->e_RenderStates.b_ColorWriteEnable;
    }
}

inline BOOL XeMaterial::IsZEnabled(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return TRUE;
    }
    else
    {
        return m_pstProperties->e_RenderStates.b_ZEnable;
    }
}

inline BOOL XeMaterial::IsZWriteEnabled(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return ((m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_NoZWrite) == 0);
    }
    else
    {
        return m_pstProperties->e_RenderStates.b_ZWriteEnable;
    }
}

inline ULONG XeMaterial::GetAlphaRef(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        ULONG alpharef = MAT_GET_AlphaTresh(m_pstJadeMTLevel->ul_Flags);
        return (ULONG)(alpharef * (255.0f/252.0f));
    }
    else
    {
        return m_pstProperties->ul_AlphaRef;
    }
}

inline ULONG XeMaterial::GetLocalAlpha(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return (MAT_GET_LocalAlpha(m_pstJadeMTLevel->s_AditionalFlags) << 3);
    }
    else
    {
        return m_pstProperties->ul_LocalAlpha;
    }
}

inline FLOAT XeMaterial::GetAlphaScale(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel)
        {
            return m_pstJadeMTLevel->pst_XeLevel->f_AlphaScale;
        }
    }

    return 1.0f;
}

inline FLOAT XeMaterial::GetAlphaOffset(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel)
        {
            return m_pstJadeMTLevel->pst_XeLevel->f_AlphaOffset;
        }
    }

    return 0.0f;
}

inline ULONG XeMaterial::GetSrcBlend(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        switch (MAT_GET_Blending(m_pstJadeMTLevel->ul_Flags))
        {
            case MAT_Cc_Op_Copy:
            case MAT_Cc_Op_Glow:
            case MAT_Cc_Op_AlphaPremult:
            case MAT_Cc_Op_Add:
                return D3DBLEND_ONE;
                break;

            case MAT_Cc_Op_Alpha:
                if (m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_InvertAlpha)
                    return D3DBLEND_INVSRCALPHA;
                else
                    return D3DBLEND_SRCALPHA;
                break;

            case MAT_Cc_Op_AlphaDest:
            case MAT_Cc_Op_AlphaDestPremult:
                if (m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_InvertAlpha)
                    return D3DBLEND_INVDESTALPHA;
                else
                    return D3DBLEND_DESTALPHA;
                break;

            case MAT_Cc_Op_Sub:
            case MAT_Cc_Op_PSX2ShadowSpecific:
                return D3DBLEND_ZERO;
                break;

            case MAT_Cc_Op_XeAlphaAdd:
                return D3DBLEND_SRCALPHA;
                break;
        }
    }
    else
    {
        return m_pstProperties->ul_SrcBlend;
    }

    return D3DBLEND_ONE;
}

inline ULONG XeMaterial::GetDestBlend(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        switch (MAT_GET_Blending(m_pstJadeMTLevel->ul_Flags))
        {
            case MAT_Cc_Op_Copy:
            case MAT_Cc_Op_Glow:
                return D3DBLEND_ZERO;
                break;

            case MAT_Cc_Op_Alpha:
            case MAT_Cc_Op_AlphaPremult:
                if (m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_InvertAlpha)
                    return D3DBLEND_SRCALPHA;
                else
                    return D3DBLEND_INVSRCALPHA;
                break;

            case MAT_Cc_Op_AlphaDest:
                if (m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_InvertAlpha)
                    return D3DBLEND_DESTALPHA;
                else
                    return D3DBLEND_INVDESTALPHA;
                break;

            case MAT_Cc_Op_AlphaDestPremult:
            case MAT_Cc_Op_Add:
            case MAT_Cc_Op_XeAlphaAdd:
                return D3DBLEND_ONE;
                break;

            case MAT_Cc_Op_Sub:
            case MAT_Cc_Op_PSX2ShadowSpecific:
                return D3DBLEND_INVSRCCOLOR;
                break;
        }
    }
    else
    {
        return m_pstProperties->ul_DestBlend;
    }

    return D3DBLEND_ZERO;
}

inline ULONG XeMaterial::GetZFunc(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if ((m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_ZEqual) != 0)
            return D3DCMP_EQUAL;
        else
            return D3DCMP_LESSEQUAL;
    }
    else
    {
        return m_pstProperties->ul_ZFunc;
    }
}

inline ULONG XeMaterial::GetColorSource(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return MAT_GET_ColorOp(m_pstJadeMTLevel->ul_Flags);
    }
    else
    {
        return m_pstProperties->ul_ColorSource;
    }
}

inline ULONG XeMaterial::GetUVSource(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return MAT_GET_UVSource(m_pstJadeMTLevel->ul_Flags);
    }
    else
    {
        return m_pstProperties->ul_UVSource;
    }
}

inline BOOL XeMaterial::IsMossInverted(void) const
{
    if(GetTextureId(XeMaterial::TEXTURE_MOSS)!=MAT_Xe_InvalidTextureId)
        return ((m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_InvertMoss) != 0);
    else
        return FALSE;
}

inline BOOL XeMaterial::IsOffsetMapping()
{
    //if (!m_stInfo.b_IsJadeMaterial)
    if ( m_pstJadeMTLevel->pst_XeLevel)
	{
        
		if (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_OffsetMap) 
		   return TRUE;
	   else 
		   return FALSE;
    }
return FALSE;
}

inline void XeMaterial::SetTwoSided(BOOL _b_Enable)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->e_RenderStates.b_TwoSided = _b_Enable;
    }
}

inline void XeMaterial::SetReflected(BOOL _b_Enable)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->e_RenderStates.b_ReflectOnWater = _b_Enable;
    }
}

inline void XeMaterial::SetAlphaTest(BOOL _b_Enable, ULONG _ul_AlphaRef)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->e_RenderStates.b_AlphaTestEnable = _b_Enable;
        m_pstProperties->ul_AlphaRef = _ul_AlphaRef;
    }
}

inline void XeMaterial::SetInvertAlpha(BOOL _b_Invert)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->e_RenderStates.b_InvertAlpha = _b_Invert;
    }
}

inline void XeMaterial::SetLocalAlpha(BOOL _b_Enable, ULONG _ul_LocalAlpha)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->e_RenderStates.b_UsingLocalAlpha = _b_Enable;
        m_pstProperties->ul_LocalAlpha = _ul_LocalAlpha;
    }
    else
    {
        INT iLocalAlpha = (INT) (_ul_LocalAlpha >> 3);
        MAT_SET_LocalAlpha(m_pstJadeMTLevel->s_AditionalFlags, iLocalAlpha);

        if (_b_Enable)
            m_pstJadeMTLevel->ul_Flags |= MAT_Cul_Flag_UseLocalAlpha;
        else
            m_pstJadeMTLevel->ul_Flags &= ~MAT_Cul_Flag_UseLocalAlpha;
    }
}

inline void XeMaterial::SetAlphaBlend(BOOL  _b_Enable, 
                                      ULONG _ul_SrcBlend, 
                                      ULONG _ul_DestBlend)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->e_RenderStates.b_AlphaBlendEnable = _b_Enable;
        m_pstProperties->ul_SrcBlend  = _ul_SrcBlend;
        m_pstProperties->ul_DestBlend = _ul_DestBlend;
    }
}

inline void XeMaterial::SetColorWrite(BOOL _b_Enable)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->e_RenderStates.b_ColorWriteEnable = _b_Enable;
    }
}

inline void XeMaterial::SetAlphaWrite(BOOL _b_Enable)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->e_RenderStates.b_AlphaWriteEnable = _b_Enable;
    }
}

inline void XeMaterial::SetZState(BOOL  _b_Enable, 
                                  BOOL  _b_WriteEnable, 
                                  ULONG _ul_ZFunc)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->e_RenderStates.b_ZEnable      = _b_Enable;
        m_pstProperties->e_RenderStates.b_ZWriteEnable = _b_WriteEnable;
        m_pstProperties->ul_ZFunc = _ul_ZFunc;
    }
}

inline void XeMaterial::SetColorSource(ULONG _ul_ColorSource)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->ul_ColorSource = _ul_ColorSource;
    }
}

inline void XeMaterial::SetUVSource(ULONG _ul_UVSource)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->ul_UVSource = _ul_UVSource;
    }
}

inline void XeMaterial::GetTexCoordTransformStates(ULONG* _pul_BaseMapXForm, 
                                                   ULONG* _pul_NMapXForm, 
                                                   ULONG* _pul_SMapXForm,
                                                   ULONG* _pul_DNMapXForm)
{
    ERR_X_Assert(_pul_BaseMapXForm != NULL);
    ERR_X_Assert(_pul_NMapXForm != NULL);
    ERR_X_Assert(_pul_SMapXForm != NULL);
    ERR_X_Assert(_pul_DNMapXForm != NULL);

    BOOL bBaseMapXForm = FALSE;

    *_pul_BaseMapXForm = TEXTRANSFORM_NONE;
    *_pul_NMapXForm    = TEXTRANSFORM_NO_TEX_COORD;
    *_pul_SMapXForm    = TEXTRANSFORM_NO_TEX_COORD;
    *_pul_DNMapXForm   = TEXTRANSFORM_NO_TEX_COORD;

    if (!m_stInfo.b_IsJadeMaterial)
        return;

    if (m_pstJadeMTLevel->s_TextureId == -1)
    {
        *_pul_BaseMapXForm = TEXTRANSFORM_NO_TEX_COORD;
        return;
    }

    if (((m_pstJadeMTLevel->ScaleSPeedPosU != 0) && (m_pstJadeMTLevel->ScaleSPeedPosU != MAT_Cc_Identity)) ||
        ((m_pstJadeMTLevel->ScaleSPeedPosV != 0) && (m_pstJadeMTLevel->ScaleSPeedPosV != MAT_Cc_Identity)))
    {
        *_pul_BaseMapXForm = TEXTRANSFORM_NORMAL;
        bBaseMapXForm      = TRUE;
    }
    else
    {
        ULONG ulUVSource = MAT_GET_UVSource(m_pstJadeMTLevel->ul_Flags);

        if (ulUVSource == MAT_Cc_UV_Chrome)
        {
            *_pul_BaseMapXForm = TEXTRANSFORM_NORMAL;
            bBaseMapXForm      = TRUE;
        }
        else if (ulUVSource == MAT_Cc_UV_Planar_GZMO)
        {
            *_pul_BaseMapXForm = TEXTRANSFORM_NORMAL;
            bBaseMapXForm      = TRUE;
        }
    }

    if (m_pstJadeMTLevel->pst_XeLevel)
    {
        if (m_pstJadeMTLevel->pst_XeLevel->l_NormalMapId != MAT_Xe_InvalidTextureId)
        {
            if ((m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapTransform) != 0)
            {
                *_pul_NMapXForm = TEXTRANSFORM_NORMAL;
            }
            else if (bBaseMapXForm && ((m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapAbsolute) != 0))
            {
                *_pul_NMapXForm = TEXTRANSFORM_NORMAL;
            }
            else
            {
                *_pul_NMapXForm = TEXTRANSFORM_NONE;
            }

            if (m_pstJadeMTLevel->pst_XeLevel->l_DetailNMapId != MAT_Xe_InvalidTextureId)
            {
                if ((m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_DNMapTransform) != 0)
                {
                    *_pul_DNMapXForm = TEXTRANSFORM_NORMAL;
                }
                else if (bBaseMapXForm && ((m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_DNMapTransform) != 0))
                {
                    *_pul_DNMapXForm = TEXTRANSFORM_NORMAL;
                }
                else
                {
                    *_pul_DNMapXForm = TEXTRANSFORM_NONE;
                }
            }
        }

        if (m_pstJadeMTLevel->pst_XeLevel->l_SpecularMapId != MAT_Xe_InvalidTextureId)
        {
            if ((m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapTransform) != 0)
            {
                *_pul_SMapXForm = TEXTRANSFORM_NORMAL;
            }
            else if (bBaseMapXForm && ((m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapAbsolute) != 0))
            {
                *_pul_SMapXForm = TEXTRANSFORM_NORMAL;
            }
            else
            {
                *_pul_SMapXForm = TEXTRANSFORM_NONE;
            }

        }
    }
}

inline ULONG XeMaterial::GetConstantColor(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        // Jade stores the constant color in the specular color...
        return m_pstJadeMaterial->ul_Specular;
    }
    else
    {
        return m_pstProperties->ul_Constant;
    }
}

inline ULONG XeMaterial::GetAmbientColor(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return m_pstJadeMaterial->ul_Ambiant;
    }
    else
    {
        return m_pstProperties->ul_Ambient;
    }
}

inline ULONG XeMaterial::GetDiffuseColor(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel)
        {
            return m_pstJadeMTLevel->pst_XeLevel->ul_DiffuseColor;
        }
        else
        {
            return m_pstJadeMaterial->ul_Diffuse;
        }
    }
    else
    {
        return m_pstProperties->ul_Diffuse;
    }
}

inline ULONG XeMaterial::GetSpecularColor(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel)
        {
            return m_pstJadeMTLevel->pst_XeLevel->ul_SpecularColor;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return m_pstProperties->ul_Specular;
    }
}

inline FLOAT XeMaterial::GetSpecularExponent(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel)
        {
            return m_pstJadeMTLevel->pst_XeLevel->f_SpecularExp;
        }
        else
        {
            return 1.0f;
        }
    }
    else
    {
        return m_pstProperties->f_Shininess;
    }
}

inline void XeMaterial::SetConstantColor(ULONG _ul_Color)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->ul_Constant = _ul_Color;
    }
}

inline void XeMaterial::SetAmbientColor(ULONG _ul_Color)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->ul_Constant = _ul_Color;
    }
}

inline void XeMaterial::SetDiffuseColor(ULONG _ul_Color)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->ul_Diffuse = _ul_Color;
    }
}

inline void XeMaterial::SetSpecularColor(ULONG _ul_Color)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->ul_Specular = _ul_Color;
    }
}

inline void XeMaterial::SetSpecularExponent(FLOAT _f_Shininess)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->f_Shininess = _f_Shininess;
    }
}

inline ULONG XeMaterial::GetStageCount(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return TEXTURE_COUNT;
    }
    else
    {
        return m_pstProperties->ul_StagesStackDepth;
    }
}

inline LONG XeMaterial::GetTextureId(ULONG _ul_Stage) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
#if defined(_XENON)
		// Using Quick Look-Up Cache

		return m_pstJadeMTLevel->al_TextureIDCache[_ul_Stage];
#else
        switch (_ul_Stage)
        {
            case TEXTURE_BASE:
                return m_pstJadeMTLevel->s_TextureId;
                break;

            case TEXTURE_NORMAL:
                if (
                    m_pstJadeMTLevel->pst_XeLevel
#if defined(ACTIVE_EDITORS)
                    && !(m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapDisable)
#endif
                   )
                {
                    return m_pstJadeMTLevel->pst_XeLevel->l_NormalMapId;
                }
                break;

            case TEXTURE_SPECULAR:
                if (
                    m_pstJadeMTLevel->pst_XeLevel
#if defined(ACTIVE_EDITORS)
                    && !(m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapDisable)
#endif
                   )
                {
                    return m_pstJadeMTLevel->pst_XeLevel->l_SpecularMapId;
                }
                break;

            case TEXTURE_ENVIRONMENT:
                if (
                    m_pstJadeMTLevel->pst_XeLevel
#if defined(ACTIVE_EDITORS)
                    && !(m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_EMapDisable)
#endif
                   )
                {
                    return XE_CUBE_MAP_MASK | m_pstJadeMTLevel->pst_XeLevel->l_EnvMapId;
                }
                break;

            case TEXTURE_MOSS:
                if (
                    m_pstJadeMTLevel->pst_XeLevel
#if defined(ACTIVE_EDITORS)
                    && !(m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_MossMapDisable)
#endif
                    )
                {
                    return m_pstJadeMTLevel->pst_XeLevel->l_MossMapId;
                }
                break;

            case TEXTURE_DETAILNMAP:
                if (
                    m_pstJadeMTLevel->pst_XeLevel
#if defined(ACTIVE_EDITORS)
                    && !(m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_DNMapDisable)
#endif
                   )
                {
                    return m_pstJadeMTLevel->pst_XeLevel->l_DetailNMapId;
                }
                break;
        }

	    return MAT_Xe_InvalidTextureId;
#endif
    }
    else
    {
		return m_pstProperties->ast_Stages[_ul_Stage].l_TextureId;
    }
}

inline void XeMaterial::GetAddressMode(ULONG  _ul_Stage, 
                                       ULONG* _pul_AddressU, 
                                       ULONG* _pul_AddressV, 
                                       ULONG* _pul_AddressW) const
{
    ERR_X_Assert((_pul_AddressU != NULL) && (_pul_AddressV != NULL) && (_pul_AddressW != NULL));

    if (m_stInfo.b_IsJadeMaterial)
    {
        if (_ul_Stage == 0)
        {
            if (m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_TileU)
                *_pul_AddressU = D3DTADDRESS_WRAP;
            else
                *_pul_AddressU = D3DTADDRESS_CLAMP;

            if (m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_TileV)
                *_pul_AddressV = D3DTADDRESS_WRAP;
            else
                *_pul_AddressV = D3DTADDRESS_CLAMP;
        }
        else
        {
            *_pul_AddressU = D3DTADDRESS_WRAP;
            *_pul_AddressV = D3DTADDRESS_WRAP;
        }

        *_pul_AddressW = D3DTADDRESS_WRAP;
    }
    else
    {
        ERR_X_Assert(_ul_Stage < m_pstProperties->ul_StagesStackDepth);

        *_pul_AddressU = m_pstProperties->ast_Stages[_ul_Stage].e_States.ul_AddressU;
        *_pul_AddressV = m_pstProperties->ast_Stages[_ul_Stage].e_States.ul_AddressV;
        *_pul_AddressW = m_pstProperties->ast_Stages[_ul_Stage].e_States.ul_AddressW;
    }
}

inline void XeMaterial::GetFilterMode(ULONG  _ul_Stage, 
                                      ULONG* _pul_MagFilter, 
                                      ULONG* _pul_MinFilter, 
                                      ULONG* _pul_MipFilter) const
{
    ERR_X_Assert((_pul_MagFilter != NULL) && (_pul_MinFilter != NULL) && (_pul_MipFilter != NULL));

    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_Bilinear)
        {
            *_pul_MagFilter = D3DTEXF_LINEAR;
            *_pul_MinFilter = D3DTEXF_LINEAR;
        }
        else
        {
            *_pul_MagFilter = D3DTEXF_POINT;
            *_pul_MinFilter = D3DTEXF_POINT;
        }

        if (m_pstJadeMTLevel->ul_Flags & MAT_Cul_Flag_Trilinear)
        {
            *_pul_MipFilter = D3DTEXF_LINEAR;
        }
        else
        {
            *_pul_MipFilter = D3DTEXF_POINT;
        }
    }
    else
    {
        ERR_X_Assert(_ul_Stage < m_pstProperties->ul_StagesStackDepth);

        *_pul_MagFilter = m_pstProperties->ast_Stages[_ul_Stage].e_States.ul_MagFilter;
        *_pul_MinFilter = m_pstProperties->ast_Stages[_ul_Stage].e_States.ul_MinFilter;
        *_pul_MipFilter = m_pstProperties->ast_Stages[_ul_Stage].e_States.ul_MipFilter;
    }
}

inline ULONG XeMaterial::GetBorderColor(ULONG _ul_Stage) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return 0;
    }
    else
    {
        ERR_X_Assert(_ul_Stage < m_pstProperties->ul_StagesStackDepth);

        return m_pstProperties->ast_Stages[_ul_Stage].ul_BorderColor;
    }
}

inline void XeMaterial::GetTransform(ULONG _ul_Stage, D3DXMATRIX* _pst_Matrix, ULONG _ulWorldIndex) const
{
#if defined(_XENON)
	if (m_stInfo.b_IsJadeMaterial)
	{
		switch (_ul_Stage)
		{
			case TEXTURE_BASE:
			{
				ULONG ulUVSource = MAT_GET_UVSource(m_pstJadeMTLevel->ul_Flags);

				if (ulUVSource == MAT_Cc_UV_Chrome)
				{
					ComputeJadeChromeMatrix(_pst_Matrix);
				}
				else if (ulUVSource == MAT_Cc_UV_Planar_GZMO)
				{
					ComputeJadePlanarGizmoMatrix(_pst_Matrix, _ulWorldIndex);
				}
				else
				{
					ComputeJadeTexCoordMatrix(_pst_Matrix);
				}
				break;
			}

			case TEXTURE_NORMAL:
			{
				ComputeTransform(_pst_Matrix, m_pstJadeMTLevel->pst_XeLevel->st_NormalMapTransform,
								(m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapAbsolute),
								(m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapScrollU),
								(m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapScrollV),
								_ulWorldIndex);
				break;
			}

			case TEXTURE_SPECULAR:
			{
				ComputeTransform(_pst_Matrix, m_pstJadeMTLevel->pst_XeLevel->st_SpecularMapTransform,
								(m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapAbsolute),
								(m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapScrollU),
								(m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapScrollV),
								_ulWorldIndex);
				break;
			}

			case TEXTURE_DETAILNMAP:
			{
				D3DXMATRIX mDNMapTransform;
				D3DXMATRIX mBaseTransform;

				// Compute the detail normal map transform as absolute
				ComputeTransform(&mDNMapTransform, m_pstJadeMTLevel->pst_XeLevel->st_DetailNMapTransform, TRUE,
								(m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_DNMapScrollU),
								(m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_DNMapScrollV),
								_ulWorldIndex);

				// Compute the normal map transform
				if (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapTransform)
				{
					GetTransform(TEXTURE_NORMAL, &mBaseTransform, _ulWorldIndex);
				}
				else
				{
					GetTransform(TEXTURE_BASE, &mBaseTransform, _ulWorldIndex);
				}

				// Compute the detail normal map transform as relative to the normal map transform
				D3DXMatrixMultiply(_pst_Matrix, &mDNMapTransform, &mBaseTransform);
				break;
			}
		}
	}
	else
	{
		*_pst_Matrix = m_pstProperties->ast_Stages[_ul_Stage].st_Transform;
	}
#else
    ERR_X_Assert(_pst_Matrix != NULL);

    if (m_stInfo.b_IsJadeMaterial)
    {
        if (_ul_Stage == TEXTURE_BASE)
        {
            ULONG ulUVSource = MAT_GET_UVSource(m_pstJadeMTLevel->ul_Flags);

            if (ulUVSource == MAT_Cc_UV_Chrome)
            {
                ComputeJadeChromeMatrix(_pst_Matrix);
            }
            else if (ulUVSource == MAT_Cc_UV_Planar_GZMO)
            {
                ComputeJadePlanarGizmoMatrix(_pst_Matrix, _ulWorldIndex);
            }
            else
            {
                ComputeJadeTexCoordMatrix(_pst_Matrix);
            }
        }
        else if ((_ul_Stage == TEXTURE_NORMAL) && (m_pstJadeMTLevel->pst_XeLevel != NULL))
        {
            ComputeTransform(_pst_Matrix, m_pstJadeMTLevel->pst_XeLevel->st_NormalMapTransform,
                             (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapAbsolute) != 0,
                             (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapScrollU) != 0,
                             (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapScrollV) != 0,
                             _ulWorldIndex
#if defined(ACTIVE_EDITORS)
                             , (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapTransform) != 0
#endif
                             );
        }
        else if ((_ul_Stage == TEXTURE_SPECULAR) && (m_pstJadeMTLevel->pst_XeLevel != NULL))
        {
            ComputeTransform(_pst_Matrix, m_pstJadeMTLevel->pst_XeLevel->st_SpecularMapTransform,
                             (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapAbsolute) != 0,
                             (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapScrollU) != 0,
                             (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapScrollV) != 0,
                             _ulWorldIndex
#if defined(ACTIVE_EDITORS)
                             , (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_SMapTransform) != 0
#endif
                             );
        }
        else if ((_ul_Stage == TEXTURE_DETAILNMAP) && (m_pstJadeMTLevel->pst_XeLevel != NULL))
        {
            D3DXMATRIX mDNMapTransform;
            D3DXMATRIX mBaseTransform;

            // Compute the detail normal map transform as absolute
            ComputeTransform(&mDNMapTransform, m_pstJadeMTLevel->pst_XeLevel->st_DetailNMapTransform, true,
                             (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_DNMapScrollU) != 0,
                             (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_DNMapScrollV) != 0,
                             _ulWorldIndex
#if defined(ACTIVE_EDITORS)
                             , (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_DNMapTransform) != 0
#endif
                             );

            // Compute the normal map transform
            if (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_NMapTransform)
            {
                GetTransform(TEXTURE_NORMAL, &mBaseTransform, _ulWorldIndex);
            }
            else
            {
                GetTransform(TEXTURE_BASE, &mBaseTransform, _ulWorldIndex);
            }

            // Compute the detail normal map transform as relative to the normal map transform
            D3DXMatrixMultiply(_pst_Matrix, &mDNMapTransform, &mBaseTransform);
        }
        else
        {
            D3DXMatrixIdentity(_pst_Matrix);
        }
    }
    else
    {
        if ((_ul_Stage < m_pstProperties->ul_StagesStackDepth) && (m_pstProperties->ast_Stages[_ul_Stage].e_States.b_TexCoordTransform))
        {
            *_pst_Matrix = m_pstProperties->ast_Stages[_ul_Stage].st_Transform;
        }
        else
        {
            D3DXMatrixIdentity(_pst_Matrix);
        }
    }
#endif
}

inline void XeMaterial::GetTangentTransform(D3DXMATRIX* _pst_Matrix) const
{
    ERR_X_Assert(_pst_Matrix != NULL);

    if (m_stInfo.b_IsJadeMaterial)
    {
        ComputeTangentTransform(_pst_Matrix);
    }
    else
    {
        D3DXMatrixIdentity(_pst_Matrix);
    }
}

inline void XeMaterial::SetTextureId(ULONG _ul_Stage, LONG _l_TextureId)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        ERR_X_Assert(_ul_Stage < m_pstProperties->ul_StagesStackDepth);

        m_pstProperties->ast_Stages[_ul_Stage].l_TextureId = _l_TextureId;
    }
}

inline void XeMaterial::SetAddressMode(ULONG _ul_Stage, 
                                       ULONG _ul_AddressU, 
                                       ULONG _ul_AddressV, 
                                       ULONG _ul_AddressW)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        ERR_X_Assert(_ul_Stage < m_pstProperties->ul_StagesStackDepth);

        m_pstProperties->ast_Stages[_ul_Stage].e_States.ul_AddressU = _ul_AddressU;
        m_pstProperties->ast_Stages[_ul_Stage].e_States.ul_AddressV = _ul_AddressV;
        m_pstProperties->ast_Stages[_ul_Stage].e_States.ul_AddressW = _ul_AddressW;
    }
}

inline void XeMaterial::SetFilterMode(ULONG _ul_Stage, 
                                      ULONG _ul_MagFilter, 
                                      ULONG _ul_MinFilter, 
                                      ULONG _ul_MipFilter)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        ERR_X_Assert(_ul_Stage < m_pstProperties->ul_StagesStackDepth);

        m_pstProperties->ast_Stages[_ul_Stage].e_States.ul_MagFilter = _ul_MagFilter;
        m_pstProperties->ast_Stages[_ul_Stage].e_States.ul_MinFilter = _ul_MinFilter;
        m_pstProperties->ast_Stages[_ul_Stage].e_States.ul_MipFilter = _ul_MipFilter;
    }
}

inline void XeMaterial::SetBorderColor(ULONG _ul_Stage, ULONG _ul_Color)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        ERR_X_Assert(_ul_Stage < m_pstProperties->ul_StagesStackDepth);

        m_pstProperties->ast_Stages[_ul_Stage].ul_BorderColor = _ul_Color;
    }
}

inline void XeMaterial::SetTransform(ULONG _ul_Stage, BOOL _b_Enable, D3DXMATRIX* _pst_Matrix)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        ERR_X_Assert(_ul_Stage < m_pstProperties->ul_StagesStackDepth);

        if (_b_Enable)
        {
            ERR_X_Assert(_pst_Matrix != NULL);

            m_pstProperties->ast_Stages[_ul_Stage].e_States.b_TexCoordTransform = TRUE;
            m_pstProperties->ast_Stages[_ul_Stage].st_Transform                 = *_pst_Matrix;
        }
        else
        {
            m_pstProperties->ast_Stages[_ul_Stage].e_States.b_TexCoordTransform = FALSE;
        }
    }
}

inline FLOAT XeMaterial::GetSpecularBias(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel != NULL)
        {
            return m_pstJadeMTLevel->pst_XeLevel->f_SpecularBias;
        }
    }
    else
    {
        return m_pstProperties->st_Constants.f_SpecularBias;
    }

    return 0.0f;
}

inline ULONG XeMaterial::GetEnvMapColor(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel != NULL)
        {
            return m_pstJadeMTLevel->pst_XeLevel->ul_EnvMapColor;
        }
    }
    else
    {
        return m_pstProperties->st_Constants.ul_EnvMapColor;
    }

    return 0;
}

inline ULONG XeMaterial::GetMossMapColor(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel != NULL)
        {
            return m_pstJadeMTLevel->pst_XeLevel->ul_MossMapColor;
        }
    }

    return 0;
}

inline FLOAT XeMaterial::GetMossSpecularFactor(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel != NULL)
        {
            return m_pstJadeMTLevel->pst_XeLevel->f_MossSpecularFactor;
        }
    }

    return 0;
}

inline FLOAT XeMaterial::GetDetailNormalMapStrength(UCHAR _uc_LOD) const
{
    if (m_stInfo.b_IsJadeMaterial && (m_pstJadeMTLevel->pst_XeLevel != NULL))
    {
        return m_pstJadeMTLevel->pst_XeLevel->f_DetailNMapStrength;
    }

    return 0.0f;
}

inline void XeMaterial::SetSpecularBias(FLOAT _f_Bias)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->st_Constants.f_SpecularBias = _f_Bias;
    }
}

inline void XeMaterial::SetEnvMapColor(ULONG _ul_Color)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->st_Constants.ul_EnvMapColor = _ul_Color;
    }
}

inline ULONG XeMaterial::GetSpecularMapChannel(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel)
        {
            return MAT_Xe_GetSpecularMapChannel(m_pstJadeMTLevel->pst_XeLevel->ul_Flags);
        }
    }

    return 0;
}

inline ULONG XeMaterial::GetAmbientSelection(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel)
        {
            return MAT_Xe_GetAmbientSel(m_pstJadeMTLevel->pst_XeLevel->ul_Flags);
        }
    }

    return 0;
}

inline ULONG XeMaterial::GetDiffuseSelection(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel)
        {
            return MAT_Xe_GetDiffuseSel(m_pstJadeMTLevel->pst_XeLevel->ul_Flags);
        }
    }

    return 0;
}

inline ULONG XeMaterial::GetSpecularSelection(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel)
        {
            return MAT_Xe_GetSpecularSel(m_pstJadeMTLevel->pst_XeLevel->ul_Flags);
        }
    }

    return 0;
}

inline ULONG XeMaterial::GetBlendOp(void) const
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        return D3DBLENDOP_ADD;
    }
    else
    {
        return m_pstProperties->ul_BlendOp;
    }
}

inline void XeMaterial::SetBlendOp(ULONG _ul_BlendOp)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        m_pstProperties->ul_BlendOp = _ul_BlendOp;
    }
}

inline FLOAT XeMaterial::GetMipmapLODBias(ULONG _ul_Stage)
{
    if (m_stInfo.b_IsJadeMaterial)
    {
        if (m_pstJadeMTLevel->pst_XeLevel != NULL)
        {
            if ((_ul_Stage == TEXTURE_BASE) || (_ul_Stage == TEXTURE_SPECULAR))
            {
                return m_pstJadeMTLevel->pst_XeLevel->f_BaseMipMapLODBias;
            }
            else if ((_ul_Stage == TEXTURE_NORMAL) || (_ul_Stage == TEXTURE_DETAILNMAP))
            {
                return m_pstJadeMTLevel->pst_XeLevel->f_NormalMipMapLODBias;
            }
        }
    }
    else
    {
        if (_ul_Stage < m_pstProperties->ul_StagesStackDepth)
        {
            return m_pstProperties->ast_Stages[_ul_Stage].f_MipmapLODBias;
        }
    }

    return 0.0f;
}

inline void XeMaterial::SetMipmapLODBias(ULONG _ul_Stage, FLOAT _f_Bias)
{
    if (!m_stInfo.b_IsJadeMaterial)
    {
        if (_ul_Stage < m_pstProperties->ul_StagesStackDepth)
        {
            m_pstProperties->ast_Stages[_ul_Stage].f_MipmapLODBias = _f_Bias;
        }
    }
}



inline BOOL XeMaterial::IsRimLightEnabled() const
{
    if( m_stInfo.b_IsJadeMaterial && m_pstJadeMTLevel->pst_XeLevel != NULL )
    {
        return (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_RimLightEnable) != 0;
    }
    return FALSE;
}

inline BOOL XeMaterial::IsRimLightSpecularMapAttenuation() const
{
    if( m_stInfo.b_IsJadeMaterial && m_pstJadeMTLevel->pst_XeLevel != NULL )
    {
        return (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_RimLightSMapAttenuation) != 0;
    }
    return FALSE;
}

inline FLOAT XeMaterial::GetRimLightWidthMin() const
{
    if( m_stInfo.b_IsJadeMaterial && m_pstJadeMTLevel->pst_XeLevel != NULL )
    {
        return m_pstJadeMTLevel->pst_XeLevel->f_RimLightWidthMin;
    }
    return 0.0f;
}

inline FLOAT XeMaterial::GetRimLightWidthMax() const
{
    if( m_stInfo.b_IsJadeMaterial && m_pstJadeMTLevel->pst_XeLevel != NULL )
    {
        return m_pstJadeMTLevel->pst_XeLevel->f_RimLightWidthMax;
    }
    return 0.0f;
}

inline FLOAT XeMaterial::GetRimLightIntensity() const
{
    if( m_stInfo.b_IsJadeMaterial && m_pstJadeMTLevel->pst_XeLevel != NULL )
    {
        return m_pstJadeMTLevel->pst_XeLevel->f_RimLightIntensity;
    }
    return 0.0f;
}

inline FLOAT XeMaterial::GetRimLightNormalMapRatio() const
{
    if( m_stInfo.b_IsJadeMaterial && m_pstJadeMTLevel->pst_XeLevel != NULL )
    {
        return m_pstJadeMTLevel->pst_XeLevel->f_RimLightNormalMapRatio;
    }
    return 0.0f;
}

inline BOOL  XeMaterial::IsGlowEnabled() const
{
    if( m_stInfo.b_IsJadeMaterial && m_pstJadeMTLevel->pst_XeLevel != NULL )
    {
        return (m_pstJadeMTLevel->pst_XeLevel->ul_Flags & MAT_Xe_Flag_GlowEnable) != 0;
    }
    return FALSE;
}

inline void XeMaterial::SetStencil( BOOL _b_Enable )
{
     if( !m_stInfo.b_IsJadeMaterial )
     {
         m_pstProperties->e_RenderStates.b_StencilEnabled = _b_Enable;
     }
}

inline BOOL XeMaterial::IsStencilEnabled( ) const
{
    if( !m_stInfo.b_IsJadeMaterial )
    {
       return  m_pstProperties->e_RenderStates.b_StencilEnabled != 0;
    }
    return FALSE;
}
