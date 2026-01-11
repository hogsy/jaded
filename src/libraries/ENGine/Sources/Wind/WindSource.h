//------------------------------------------------------------------------
//
// (C) Copyright 2002 Ubisoft
//
// Author		Stephane Girard
// Date			16 Dec 2002
//
// File			WindSource.h
// Description
//
//------------------------------------------------------------------------
#ifndef __WINDSOURCE_H__INCLUDED
#define __WINDSOURCE_H__INCLUDED

#define WindSource_C_VersionNumber 3

#include "BIGfiles/LOAding/LOAdefs.h"

class CWindSource
{
 public:

     CWindSource();
     ~CWindSource();

     M_DeclareOperatorNewAndDelete();

     _inline_ MATH_tdst_Vector * GetWind();

     void  Init(OBJ_tdst_GameObject * _p_Gao);
     void  Update();
     ULONG Load(char *_pc_Buffer);
     BOOL  IsInSource(OBJ_tdst_GameObject * _p_Gao, FLOAT * _pf_Distance = NULL);

     void     AddRef() { m_RefCount++; };
     void     DeleteRef();

#ifdef ACTIVE_EDITORS
     void Save();
     _inline_ void  GetSizePlane(FLOAT &_fDx, FLOAT &_fDz);
     _inline_ void  SetSizePlane(FLOAT _fDx, FLOAT _fDz);
     _inline_ void  ScalePlane(FLOAT _fScaleDx, FLOAT _fScaleDz);
    
     _inline_ void  SetShape(FLOAT _fValue);
     _inline_ void  SetFrequency(FLOAT _fValue);
     _inline_ void  SetSpeed(FLOAT _fValue);
	 _inline_ void  SetDynamic(BOOL _bValue);
	 _inline_ void  SetRadial(BOOL _bValue);	 

     _inline_ FLOAT GetAmplitude();
     _inline_ FLOAT GetShape();
     _inline_ FLOAT GetFrequency();
     _inline_ FLOAT GetSpeed();
     _inline_ FLOAT GetMinForce();
     _inline_ FLOAT GetMaxForce();
	 

     _inline_ ULONG GetKey();
     

     void     UpdateErrorAngleDirection();
     void     UpdateErrorDirection();
	 void     UpdateDynamic();

#endif // #ifdef ACTIVE_EDITORS

	 _inline_ void  SetAmplitude(FLOAT _fValue);
	 _inline_ void  SetMinForce(FLOAT _fValue);
	 _inline_ void  SetMaxForce(FLOAT _fValue);
	 _inline_ void  SetNear(FLOAT _fValue);	 
	 _inline_ void  SetFar(FLOAT _fValue);
	 _inline_ void  SetWindBehindPlane(BOOL _bValue);

	 _inline_ FLOAT GetForce();
	 _inline_ OBJ_tdst_GameObject * GetGao();

 private:

     ULONG                 m_RefCount;
     OBJ_tdst_GameObject * m_pGao;
     MATH_tdst_Matrix      m_InvMatrix;
     MATH_tdst_Vector      m_CurrentWind;
     MATH_tdst_Vector      m_LocalDirection;

 public:

     // Source zone
     FLOAT            m_fDx;
     FLOAT            m_fDz;
     FLOAT            m_fMinForce;
     FLOAT            m_fMaxForce;

     // Swave param
     FLOAT            m_fAmplitude;     // Amplitude variance [0,1]
     FLOAT            m_fShape;         // Shape factor [-1,1]: Stretches toward low values (s < 0) or toward high values (s > 0)
     FLOAT            m_fFrequency;     // Frequency variance [0,1]  
     FLOAT            m_fSpeed;

     // Direction
     FLOAT            m_fErrorDirection;
     FLOAT            m_fVariationDirection;

#ifdef ACTIVE_EDITORS
    // Direction
     FLOAT            m_fErrorAngleDirection; // degree
#endif // #ifdef ACTIVE_EDITORS

	 // Dynamic wind sources
	 BOOL			 m_bIsDynamic;
	 BOOL			 m_bIsRadial;
	 FLOAT			 m_fNear;
	 FLOAT			 m_fFar;
	 FLOAT			 m_fForce;
	 BOOL			 m_bWindBehindPlane;
	 BOOL			 m_bIsSpeedModulated;
	 FLOAT			 m_fSpeedModulation;
	 BOOL			 m_bFirstInit;
	 MATH_tdst_Vector m_prevPos;
};


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				12 Dec 2002
// 
// Description		Get current wind.
// 
//------------------------------------------------------------------------
_inline_ MATH_tdst_Vector * CWindSource::GetWind()
{
    return &m_CurrentWind;
}

_inline_ FLOAT CWindSource::GetForce()
{
	return m_fForce;
}

_inline_ OBJ_tdst_GameObject * CWindSource::GetGao()
{
	return m_pGao;
}

#ifdef ACTIVE_EDITORS

_inline_ void CWindSource::GetSizePlane(FLOAT &_fDx, FLOAT &_fDz)
{
    _fDx = m_fDx;
    _fDz = m_fDz;
}

_inline_ void CWindSource::SetSizePlane(FLOAT _fDx, FLOAT _fDz)
{
    m_fDx = _fDx;
    m_fDz = _fDz;
}

_inline_ void CWindSource::ScalePlane(FLOAT _fScaleDx, FLOAT _fScaleDz)
{
    m_fDx *= _fScaleDx;
    m_fDz *= _fScaleDz;
}

_inline_ void  CWindSource::SetShape(FLOAT _fValue)
{
    m_fShape = _fValue;
}

_inline_ void  CWindSource::SetFrequency(FLOAT _fValue)
{
    m_fFrequency = _fValue;
}

_inline_ void  CWindSource::SetSpeed(FLOAT _fValue)
{
    m_fSpeed = _fValue;
}

_inline_ void  CWindSource::SetDynamic(BOOL _bValue)
{
	m_bIsDynamic = _bValue;
}

_inline_ void  CWindSource::SetRadial(BOOL _bValue)
{
	m_bIsRadial = _bValue;
}

_inline_ FLOAT CWindSource::GetAmplitude()
{
    return m_fAmplitude;
}

_inline_ FLOAT CWindSource::GetShape()
{
    return m_fShape;
}

_inline_ FLOAT CWindSource::GetFrequency()
{
    return m_fFrequency;
}

_inline_ FLOAT CWindSource::GetSpeed()
{
    return m_fSpeed;
}

_inline_ FLOAT CWindSource::GetMinForce()
{
    return m_fMinForce;
}

_inline_ FLOAT CWindSource::GetMaxForce()
{
    return m_fMaxForce;
}

_inline_ ULONG CWindSource::GetKey()
{
    return LOA_ul_SearchKeyWithAddress((ULONG)m_pGao);
}

#endif // #ifdef ACTIVE_EDITORS

_inline_ void  CWindSource::SetAmplitude(FLOAT _fValue)
{
	m_fAmplitude = _fValue;
}

_inline_ void  CWindSource::SetMinForce(FLOAT _fValue)
{
	m_fMinForce = _fValue;
}

_inline_ void  CWindSource::SetMaxForce(FLOAT _fValue)
{
	m_fMaxForce = _fValue;
}

_inline_ void  CWindSource::SetNear(FLOAT _fValue)
{
	m_fNear = _fValue;
}

_inline_ void  CWindSource::SetFar(FLOAT _fValue)
{
	m_fFar = _fValue;
}

_inline_ void  CWindSource::SetWindBehindPlane(BOOL _bValue)
{
	m_bWindBehindPlane = _bValue;
}

#endif // #ifndef __WINDSOURCE_H__INCLUDED