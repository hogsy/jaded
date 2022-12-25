//------------------------------------------------------------------------
//
// (C) Copyright 2002 Ubisoft
//
// Author		Stephane Girard
// Date			16 Dec 2002
//
// File			CurrentWind.h
// Description
//
//------------------------------------------------------------------------
#ifndef __CURRENTWIND_H__INCLUDED
#define __CURRENTWIND_H__INCLUDED

#include "ENGine/Sources/Wind/WindSource.h"

#define CurrentStaticWind_C_VersionNumber 0

#define WIND_C_MaxCurrentSource  2

extern float    TIM_gf_MainClock;

class CCurrentWind
{
 private:

 public:

     enum eType
     {
         eType_None    = 0,
         eType_Static  = 1,
         eType_Dynamic = 2,
         eType_Dummy   = 0xFFFFFFFF
     };

     CCurrentWind();
     virtual ~CCurrentWind();

     M_DeclareOperatorNewAndDelete();

     virtual eType GetType() { return eType_None; }
     virtual void  Reinit() {};
     virtual BOOL  GetWind(MATH_tdst_Vector * _p_Wind) { return FALSE; }
};

class CCurrentStaticWind : public CCurrentWind
{
 private:

     OBJ_tdst_GameObject * m_pGao;
     CWindSource         * m_pCurrentSources;

 public:

     CCurrentStaticWind();
     virtual ~CCurrentStaticWind();

     M_DeclareOperatorNewAndDelete();

     virtual eType GetType() { return eType_Static; }
     virtual void Reinit();
     virtual BOOL GetWind(MATH_tdst_Vector * _p_Wind);
    
     ULONG Load(char * _pc_Buffer);

#ifdef ACTIVE_EDITORS
     void Save();
     void Initialise(CWindSource * po_Source);

     OBJ_tdst_GameObject * GetGao() { return m_pGao; }

#endif // #ifdef ACTIVE_EDITORS
};


class CCurrentDynamicWind : public CCurrentWind
{
 private:

     FLOAT          m_fUpdate;
     CWindSource *  m_apCurrentSources[WIND_C_MaxCurrentSource];
     ULONG          m_ulCurrentIndex;

 public:

     CCurrentDynamicWind();
     virtual ~CCurrentDynamicWind();

     M_DeclareOperatorNewAndDelete();

     virtual eType GetType() { return eType_Dynamic; }
     virtual void Reinit();
     virtual BOOL GetWind(MATH_tdst_Vector * _p_Wind);

     void AddWindSource(CWindSource * _p_Source);
     void RemoveWindSource(CWindSource * _p_Source);

     _inline_ ULONG GetCurrentIndex();
     _inline_ void SetCurrentIndex(ULONG _ul_CurrentIndex);
     _inline_ BOOL IsUpdated();
     _inline_ void Update();
};


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				12 Dec 2002
// 
// Description		Get current index in the wind table.
// 
//------------------------------------------------------------------------
_inline_ ULONG CCurrentDynamicWind::GetCurrentIndex()
{
    return m_ulCurrentIndex;
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				12 Dec 2002
// 
// Description		Set current index in the wind table.
// 
//------------------------------------------------------------------------
_inline_ void CCurrentDynamicWind::SetCurrentIndex(ULONG _ul_CurrentIndex)
{
    m_ulCurrentIndex = _ul_CurrentIndex;
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				3 Jan 2003
// 
// Description		Verifie si on a calcule le vent courrant pendant cette trame.
// 
//------------------------------------------------------------------------
_inline_ BOOL CCurrentDynamicWind::IsUpdated()
{
    if (TIM_gf_MainClock == m_fUpdate)
        return TRUE;

    return FALSE;
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				3 Jan 2003
// 
// Description	    
// 
//------------------------------------------------------------------------
_inline_ void CCurrentDynamicWind::Update()
{
    m_fUpdate = TIM_gf_MainClock;
}

#endif // #ifndef __CURRENTWIND_H__INCLUDED