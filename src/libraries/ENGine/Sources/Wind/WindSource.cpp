//------------------------------------------------------------------------
//
// (C) Copyright 2002 Ubisoft
//
// Author		Stephane Girard
// Date			11 Dec 2002
//
// File			WindSource.cpp
// Description
//
//------------------------------------------------------------------------
#include "precomp.h"
#include "WindSource.h"
#include "ENGine/Sources/INTersection/INTmain.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "noise.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "ENGine/Sources/WORld/WORaccess.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif //#ifdef ACTIVE_EDITORS

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				12 Dec 2002
// 
// Description		Constructor
// 
//------------------------------------------------------------------------
CWindSource::CWindSource()
{
    m_RefCount = 1;
    m_pGao = NULL;
    m_fDx = 10.0f;
    m_fDz = 10.0f;
    m_fMinForce = 0.0f;
    m_fMaxForce = 1.0f;
    m_fAmplitude = 0.0f;
    m_fShape = 0.0f;
    m_fFrequency = 0.0f;
    m_fSpeed = 1.0f;
    m_fErrorDirection = 0.0f;
    m_fVariationDirection = 1.0f;
	m_fForce = 0.0f;

    MATH_SetNulVector(&m_CurrentWind);
    L_memset(&m_InvMatrix, 0, sizeof(MATH_tdst_Matrix));

#ifdef ACTIVE_EDITOR
    m_fErrorAngleDirection = 0.0f
#endif // #ifdef ACTIVE_EDITOR
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				12 Dec 2002
// 
// Description		Destructor
// 
//------------------------------------------------------------------------
CWindSource::~CWindSource()
{
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				12 Dec 2002
// 
// Description		Init
// 
//------------------------------------------------------------------------
void CWindSource::Init(OBJ_tdst_GameObject * _p_Gao)
{
    MATH_tdst_Matrix * p_Matrix = OBJ_pst_GetAbsoluteMatrix(_p_Gao);

    m_pGao = _p_Gao;

    if (MATH_b_TestScaleType(p_Matrix))
    {
        MATH_tdst_Matrix NoScaleMatrix;

        MATH_CopyMatrix(&NoScaleMatrix, p_Matrix);
        MATH_ClearScale(&NoScaleMatrix, 1);

        MATH_InvertMatrix(&m_InvMatrix, &NoScaleMatrix);
    }
    else
    {
        MATH_InvertMatrix(&m_InvMatrix, p_Matrix);
    }

    MATH_InitVector(&m_LocalDirection, 0.0f, 1.0f, 0.0f);

	m_bFirstInit = TRUE;
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				18 Dec 2002
// 
// Description		Load
// 
//------------------------------------------------------------------------
ULONG CWindSource::Load(char *_pc_Buffer)
{
    ULONG  ul_Version;
    char * pc_BufferSave;

    pc_BufferSave = _pc_Buffer;

    ul_Version = LOA_ReadLong(&_pc_Buffer);

    m_fMinForce  = LOA_ReadFloat(&_pc_Buffer);
    m_fMaxForce  = LOA_ReadFloat(&_pc_Buffer);
    m_fAmplitude = LOA_ReadFloat(&_pc_Buffer);
    m_fShape     = LOA_ReadFloat(&_pc_Buffer);
    m_fFrequency = LOA_ReadFloat(&_pc_Buffer);
    m_fSpeed     = LOA_ReadFloat(&_pc_Buffer);
    m_fDx        = LOA_ReadFloat(&_pc_Buffer);
    m_fDz        = LOA_ReadFloat(&_pc_Buffer);
    m_fErrorDirection = LOA_ReadFloat(&_pc_Buffer);
    m_fVariationDirection = LOA_ReadFloat(&_pc_Buffer);


	if(ul_Version >= 0)
	{
		m_bIsDynamic = FALSE;
		m_bIsRadial = FALSE;
		m_fNear = 0.0f;
		m_fFar = 0.0f;
		m_bIsSpeedModulated = FALSE;
		m_fSpeedModulation = 0.0f;
	}

	if(ul_Version >= 1)
	{
		m_bIsDynamic = LOA_ReadInt(&_pc_Buffer);
		m_bIsRadial = LOA_ReadInt(&_pc_Buffer);		
		m_fNear = LOA_ReadFloat(&_pc_Buffer);
		m_fFar = LOA_ReadFloat(&_pc_Buffer);				
	}

	if(ul_Version >= 2)
	{
		m_bWindBehindPlane = LOA_ReadInt(&_pc_Buffer);				
	}

	if(ul_Version >= 3)
	{
		m_bIsSpeedModulated = LOA_ReadInt(&_pc_Buffer);
		m_fSpeedModulation = LOA_ReadFloat(&_pc_Buffer);		
	}

#ifdef ACTIVE_EDITORS
    UpdateErrorAngleDirection();
#endif // #ifdef ACTIVE_EDITORS

    return (_pc_Buffer - pc_BufferSave);
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				12 Dec 2002
// 
// Description		Update current wind.
// 
//------------------------------------------------------------------------
void CWindSource::Update()
{    
    MATH_tdst_Vector   Normal;

    if (!m_pGao)
        return;

    m_fForce = m_fMinForce
            + (m_fMaxForce - m_fMinForce) * Swave(m_fSpeed*TIM_gf_MainClock, m_fShape, m_fFrequency, m_fAmplitude);

	FLOAT degre = m_fErrorDirection * InterpolatedNoise1D(m_fVariationDirection * TIM_gf_MainClock);

	if(!m_bIsRadial)
	{
		m_LocalDirection.z = 0.0f;
		m_LocalDirection.y = fCos(degre);
		m_LocalDirection.x = fSin(degre);

		MATH_TransformVector(&Normal, OBJ_pst_GetAbsoluteMatrix(m_pGao), &m_LocalDirection);
		MATH_NormalizeEqualVector(&Normal);

		MATH_ScaleVector(&m_CurrentWind, &Normal, m_fForce);
	}
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				12 Dec 2002
// 
// Description		Test if the point is in the sector.
// 
//------------------------------------------------------------------------
BOOL CWindSource::IsInSource(OBJ_tdst_GameObject * _p_Gao, FLOAT * _pf_MinDist /* = NULL */)
{
    MATH_tdst_Vector    LocalPos;
    MATH_tdst_Vector    Direction;
    MATH_tdst_Vector    Center;
    MATH_tdst_Vector *  p_Normal;
    BOOL                b_Visible = TRUE;
    
    p_Normal = MATH_pst_GetYAxis(OBJ_pst_GetAbsoluteMatrix(m_pGao));

    if (_pf_MinDist)
        *_pf_MinDist = Cf_Infinit;

    OBJ_BV_ComputeCenter(_p_Gao, &Center);
    MATH_TransformVertex(&LocalPos, &m_InvMatrix, &Center);

    if (LocalPos.y >= 0.0f && fAbs(LocalPos.x) <= m_fDx && fAbs(LocalPos.z) <= m_fDz)
    { 
        FLOAT f_BVRadius;

        if(OBJ_BV_IsAABBox(_p_Gao->pst_BV))
        {
            MATH_tdst_Vector * pst_Max = OBJ_pst_BV_GetGMax(_p_Gao->pst_BV);
            MATH_tdst_Vector * pst_Min = OBJ_pst_BV_GetGMin(_p_Gao->pst_BV);

            if (fAbs(p_Normal->x) > fAbs(p_Normal->y))
            {
                if (fAbs(p_Normal->x) > fAbs(p_Normal->z))
                    f_BVRadius = 0.5f*(pst_Max->x - pst_Min->x);
                else
                    f_BVRadius = 0.5f*(pst_Max->z - pst_Min->z);
            }
            else
            {
                if (fAbs(p_Normal->y) > fAbs(p_Normal->z))
                    f_BVRadius = 0.5f*(pst_Max->y - pst_Min->y);
                else
                    f_BVRadius = 0.5f*(pst_Max->z - pst_Min->z);
            }
        }
        else
        {
            f_BVRadius = OBJ_f_BV_GetRadius(_p_Gao->pst_BV);
        }

        MATH_NegVector(&Direction, p_Normal);
        
        f_BVRadius *= 0.5f;
        
        if (LocalPos.y < f_BVRadius)
        {
            // TODO KK:
            //COL_ColMap_RayCast( WOR_World_GetWorldOfObject(_p_Gao),
            //                    _p_Gao,
            //                    &Center, 
            //                    &Direction,
            //                    LocalPos.y,
            //                    0xFFFFFFFF,
            //                    0,
            //                    0, 
            //                    &b_Visible,
            //                    TRUE,
            //                    TRUE,
            //                    TRUE,
            //                    FALSE );
        }
        else
        {
            MATH_AddScaleVector(&Center, &Center, &Direction, f_BVRadius);
            //COL_ColMap_RayCast(WOR_World_GetWorldOfObject(_p_Gao), _p_Gao, &Center, &Direction, (LocalPos.y - f_BVRadius), 0xFFFFFFFF,
            //                   0, 0, &b_Visible, TRUE, TRUE, TRUE, FALSE);
        }

        if (_pf_MinDist)
        {
            *_pf_MinDist = fMin(*_pf_MinDist, LocalPos.y);
        }

        if (b_Visible)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void CWindSource::DeleteRef()
{
    m_RefCount--;
    if (m_RefCount == 0)
    {
        delete this;
    }
}

#ifdef ACTIVE_EDITORS

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				18 Dec 2002
// 
// Description		Save
// 
//------------------------------------------------------------------------
void CWindSource::Save()
{
    ULONG ul_Version = WindSource_C_VersionNumber;

    SAV_Buffer(&ul_Version, sizeof(ULONG));
    SAV_Buffer(&m_fMinForce, sizeof(FLOAT));
    SAV_Buffer(&m_fMaxForce, sizeof(FLOAT));
    SAV_Buffer(&m_fAmplitude, sizeof(FLOAT));
    SAV_Buffer(&m_fShape, sizeof(FLOAT));
    SAV_Buffer(&m_fFrequency, sizeof(FLOAT));
    SAV_Buffer(&m_fSpeed, sizeof(FLOAT));
    SAV_Buffer(&m_fDx, sizeof(FLOAT));
    SAV_Buffer(&m_fDz, sizeof(FLOAT));
    SAV_Buffer(&m_fErrorDirection, sizeof(FLOAT));
    SAV_Buffer(&m_fVariationDirection, sizeof(FLOAT));

	SAV_Buffer(&m_bIsDynamic, sizeof(BOOL));
	SAV_Buffer(&m_bIsRadial, sizeof(BOOL));
	SAV_Buffer(&m_fNear, sizeof(FLOAT));
	SAV_Buffer(&m_fFar, sizeof(FLOAT));	

	SAV_Buffer(&m_bWindBehindPlane, sizeof(BOOL));	

	SAV_Buffer(&m_bIsSpeedModulated, sizeof(BOOL));
	SAV_Buffer(&m_fSpeedModulation, sizeof(FLOAT));
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				23 Jan 2003
// 
// Description		UpdateErrorAngleDirection
// 
//------------------------------------------------------------------------
void CWindSource::UpdateErrorAngleDirection()
{
    m_fErrorAngleDirection = m_fErrorDirection;//fAsin(m_fErrorDirection) * 180.0f / Cf_Pi;
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				23 Jan 2003
// 
// Description		UpdateErrorDirection
// 
//------------------------------------------------------------------------
void CWindSource::UpdateErrorDirection()
{
    m_fErrorDirection = m_fErrorAngleDirection;//fSin(m_fErrorAngleDirection * Cf_Pi / 180.0f);
}

//------------------------------------------------------------------------
//
// Author			Charles Jacob
// Date				30 Mar 2004
// 
// Description		UpdateDynamic
// 
//------------------------------------------------------------------------
void CWindSource::UpdateDynamic()
{
	if(!m_bIsDynamic)
		m_bIsRadial = FALSE;
}

#endif // #ifdef ACTIVE_EDITORS
