//------------------------------------------------------------------------
//
// (C) Copyright 2002 Ubisoft
//
// Author		Stephane Girard
// Date			16 Dec 2002
//
// File			CurrentWind.cpp
// Description
//
//------------------------------------------------------------------------
#include "precomp.h"
#include "CurrentWind.h"
#include "ENGine/Sources/Modifier/MDFmodifier_Wind.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/SAVing/SAVdefs.h"

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		Contructor
// 
//------------------------------------------------------------------------
CCurrentWind::CCurrentWind()
{
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		Destructor
// 
//------------------------------------------------------------------------
CCurrentWind::~CCurrentWind()
{
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		Contructor
// 
//------------------------------------------------------------------------
CCurrentStaticWind::CCurrentStaticWind()
{
    m_pGao = NULL;
    m_pCurrentSources = NULL;
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		Destructor
// 
//------------------------------------------------------------------------
CCurrentStaticWind::~CCurrentStaticWind()
{
    if (m_pCurrentSources)
        m_pCurrentSources->DeleteRef();
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		Reinit
// 
//------------------------------------------------------------------------
void CCurrentStaticWind::Reinit()
{
    if (m_pCurrentSources)
        m_pCurrentSources->DeleteRef();

    if (m_pGao)
    {
        m_pCurrentSources = ((GAO_tdst_ModifierWind *)m_pGao->pst_Extended->pst_Modifiers->p_Data)->po_Source;
        m_pCurrentSources->AddRef();
    }
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		Get wind
// 
//------------------------------------------------------------------------
BOOL CCurrentStaticWind::GetWind(MATH_tdst_Vector * _p_Wind)
{
    if (m_pCurrentSources)
    {
        MATH_CopyVector(_p_Wind, m_pCurrentSources->GetWind());

        return TRUE;
    }

    return FALSE;
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		Load
// 
//------------------------------------------------------------------------
ULONG CCurrentStaticWind::Load(char * _pc_Buffer)
{
    ULONG  ul_Version;
    ULONG  ul_Key;
    char * pc_BufferSave;

    pc_BufferSave = _pc_Buffer;

    ul_Version = LOA_ReadULong(&_pc_Buffer);
    ul_Key = LOA_ReadULong(&_pc_Buffer);

    if (ul_Key != BIG_C_InvalidKey)
    {
        LOA_MakeFileRef(ul_Key, (ULONG *) &m_pGao, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
    }

    return (_pc_Buffer - pc_BufferSave);
}


#ifdef ACTIVE_EDITORS

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				2 Jan 2003
// 
// Description		Initialise
// 
//------------------------------------------------------------------------
void CCurrentStaticWind::Initialise(CWindSource * po_Source)
{
    if (po_Source)
    {
        m_pCurrentSources = po_Source;
        m_pGao = po_Source->GetGao();
        po_Source->AddRef();
    }
    else
    {
        if (m_pCurrentSources)
            m_pCurrentSources->DeleteRef();

        m_pCurrentSources = NULL;
        m_pGao = NULL;
    }
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		Save
// 
//------------------------------------------------------------------------
void CCurrentStaticWind::Save()
{
    ULONG ul_Version = CurrentStaticWind_C_VersionNumber;
    ULONG ul_Key = BIG_C_InvalidKey;

    if (m_pGao)
    {
        ul_Key = LOA_ul_SearchKeyWithAddress((ULONG)m_pGao);
    }

    SAV_Buffer(&ul_Version, sizeof(ULONG));
    SAV_Buffer(&ul_Key, sizeof(ULONG));
}

#endif // #ifdef ACTIVE_EDITORS


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		Constructor
// 
//------------------------------------------------------------------------
CCurrentDynamicWind::CCurrentDynamicWind()
{
    L_memset(m_apCurrentSources, 0, WIND_C_MaxCurrentSource * sizeof(CWindSource *));
    m_ulCurrentIndex = 0;
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		Destructor
// 
//------------------------------------------------------------------------
CCurrentDynamicWind::~CCurrentDynamicWind()
{
    for(ULONG i=0; i<WIND_C_MaxCurrentSource; i++)
    {
        if (m_apCurrentSources[i])
            m_apCurrentSources[i]->DeleteRef();
    }
}


void CCurrentDynamicWind::Reinit()
{
    for(ULONG i=0; i<WIND_C_MaxCurrentSource; i++)
    {
        if (m_apCurrentSources[i])
        {
            m_apCurrentSources[i]->DeleteRef();
            m_apCurrentSources[i] = NULL;
        }
    }

    m_ulCurrentIndex = NULL;
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		
// 
//------------------------------------------------------------------------
void CCurrentDynamicWind::AddWindSource(CWindSource * _p_Source)
{
    ULONG ul_Index = 0;

    for(ULONG i=0; i<WIND_C_MaxCurrentSource; i++)
    {
        if (m_apCurrentSources[i] == _p_Source)
            return;

        if (m_apCurrentSources[i] == NULL)
            ul_Index = i;
    }

    m_apCurrentSources[ul_Index] = _p_Source;
    _p_Source->AddRef();
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		
// 
//------------------------------------------------------------------------
void CCurrentDynamicWind::RemoveWindSource(CWindSource * _p_Source)
{
    for(ULONG i=0; i<WIND_C_MaxCurrentSource; i++)
    {
        if (m_apCurrentSources[i] == _p_Source)
        {
            m_apCurrentSources[i]->DeleteRef();
            m_apCurrentSources[i] = NULL;
            return;
        }
    }
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		
// 
//------------------------------------------------------------------------
BOOL CCurrentDynamicWind::GetWind(MATH_tdst_Vector * _p_Wind)
{
    FLOAT f_NbWind = 0.0f;
    BOOL  b_Wind = FALSE;

    MATH_SetNulVector(_p_Wind);

    for(ULONG i=0; i<WIND_C_MaxCurrentSource; i++)
    {
        if (m_apCurrentSources[i])
        {
            MATH_AddEqualVector(_p_Wind, m_apCurrentSources[i]->GetWind());
            f_NbWind++;
            b_Wind = TRUE;
        }
    }

    if (f_NbWind > 1.0f)
    {
        MATH_ScaleEqualVector(_p_Wind, fInv(f_NbWind));
    }

    return b_Wind;
}
