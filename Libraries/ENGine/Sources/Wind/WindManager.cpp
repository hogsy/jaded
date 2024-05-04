//------------------------------------------------------------------------
//
// (C) Copyright 2002 Ubisoft
//
// Author		Stephane Girard
// Date			11 Dec 2002
//
// File			WindManager.cpp
// Description
//
//------------------------------------------------------------------------
#include "precomp.h"
#include "WindManager.h"
#include "WindSource.h"
#include "Engine/Sources/OBJects/OBJaccess.h"

#ifdef ACTIVE_EDITORS
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "ENGine/Sources/OBJects/OBJsave.h"
#endif // #ifdef ACTIVE_EDITORS


// Singleton instance
CWindManager * CWindManager::m_Instance = NULL;

#define WIND_C_InitialNbElems   4

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				11 Dec 2002
// 
// Description		Constructor
// 
//------------------------------------------------------------------------
CWindManager::CWindManager()
{
    TAB_Ptable_Init(&m_ListOfSource, WIND_C_InitialNbElems, 0.1f);
	TAB_Ptable_Init(&m_ListOfDynamicSource, WIND_C_InitialNbElems, 0.1f);
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				11 Dec 2002
// 
// Description		Destructor
// 
//------------------------------------------------------------------------
CWindManager::~CWindManager()
{
    TAB_Ptable_Close(&m_ListOfSource);
	TAB_Ptable_Close(&m_ListOfDynamicSource);
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				11 Dec 2002
// 
// Description		Get instance
// 
//------------------------------------------------------------------------
CWindManager * CWindManager::GetInstance()
{
    return m_Instance;
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				11 Dec 2002
// 
// Description		Create instance
// 
//------------------------------------------------------------------------
void CWindManager::CreateInstance()
{
    m_Instance = new CWindManager();
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				11 Dec 2002
// 
// Description		Delete instance
// 
//------------------------------------------------------------------------
void CWindManager::DeleteInstance()
{
    delete m_Instance;
    m_Instance = NULL;
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				11 Dec 2002
// 
// Description		Add wind Source in the list.
// 
//------------------------------------------------------------------------
void CWindManager::AddWindSource(CWindSource * _p_Source)
{
	if(!_p_Source->m_bIsDynamic)
	{
		if (TAB_ppv_Ptable_GetElemWithPointer(&m_ListOfSource, _p_Source) == NULL)
		{		
			TAB_Ptable_AddElemAndResize(&m_ListOfSource, _p_Source);
		}
	}
	else
	{
		if(TAB_ppv_Ptable_GetElemWithPointer(&m_ListOfDynamicSource, _p_Source) == NULL)
		{
			TAB_Ptable_AddElemAndResize(&m_ListOfDynamicSource, _p_Source);
		}
    }
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				11 Dec 2002
// 
// Description		Remove wind source in the list.
// 
//------------------------------------------------------------------------
void CWindManager::RemoveWindSource(CWindSource * _p_Source)
{
	if(!_p_Source->m_bIsDynamic)
		TAB_Ptable_RemoveElemWithPointer(&m_ListOfSource, _p_Source);
	else
		TAB_Ptable_RemoveElemWithPointer(&m_ListOfDynamicSource, _p_Source);
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				11 Dec 2002
// 
// Description		Get current wind.
// 
//------------------------------------------------------------------------
BOOL CWindManager::GetWind(OBJ_tdst_GameObject * _p_Gao, MATH_tdst_Vector * _p_Wind)
{
    FLOAT f_NbWind = 0.0f;

    CWindSource ** ppSource     = (CWindSource **) TAB_ppv_Ptable_GetFirstElem(&m_ListOfSource);
    CWindSource ** ppLastSource = (CWindSource **) TAB_ppv_Ptable_GetLastElem(&m_ListOfSource);

    MATH_SetNulVector(_p_Wind);

    for(; ppSource <= ppLastSource; ppSource++)
    {
        if (TAB_b_IsAHole(*ppSource))
            continue;

        if ((*ppSource)->IsInSource(_p_Gao))
        {
            MATH_AddEqualVector(_p_Wind, (*ppSource)->GetWind());
            f_NbWind++;
        }
    }

    if (f_NbWind > 1.0f)
    {
        MATH_ScaleEqualVector(_p_Wind, fInv(f_NbWind));
    }

    return (f_NbWind > 0.0f);
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		Get current wind.
// 
//------------------------------------------------------------------------
BOOL CWindManager::GetWind(OBJ_tdst_GameObject * _p_Gao, CCurrentWind * _p_CurrentWind, MATH_tdst_Vector * _p_Wind)
{
 	BOOL result=FALSE;

	_p_Wind->x = 0.0f;
	_p_Wind->y = 0.0f;
	_p_Wind->z = 0.0f;

    ERR_X_Assert( _p_Gao );

	if (_p_CurrentWind != NULL)
	{        
		switch(_p_CurrentWind->GetType())
		{
		case CCurrentWind::eType_Static:
			{
                // Find the closest wind
                FLOAT         f_Dist;
                FLOAT         f_MinDist = Cf_Infinit;
                CWindSource * p_BestSource = NULL;

                CWindSource ** ppSource     = (CWindSource **) TAB_ppv_Ptable_GetFirstElem(&m_ListOfSource);
                CWindSource ** ppLastSource = (CWindSource **) TAB_ppv_Ptable_GetLastElem(&m_ListOfSource);

                for(; ppSource <= ppLastSource; ppSource++)
                {
                    if (TAB_b_IsAHole(*ppSource))
                        continue;

                    if ((*ppSource)->IsInSource(_p_Gao, &f_Dist))
                    {
                        if (f_Dist < f_MinDist)
                        {
                            f_MinDist = f_Dist;
                            p_BestSource = *ppSource;
                        }
                    }
                }
                if( p_BestSource )
                {
                    result = TRUE;
                    *_p_Wind = *p_BestSource->GetWind( );
                }
			}
			break;

		case CCurrentWind::eType_Dynamic:
			{
				CCurrentDynamicWind * p_CurrentDynWind = (CCurrentDynamicWind *)_p_CurrentWind;
				ULONG                 ul_Index;

				if (!p_CurrentDynWind->IsUpdated())
				{
					if (TAB_ul_Ptable_GetNbElems(&m_ListOfSource) > 0)
					{
						ul_Index = (p_CurrentDynWind->GetCurrentIndex() + 1) % TAB_ul_Ptable_GetNbElems(&m_ListOfSource);
						CWindSource ** ppSource =  (CWindSource **) TAB_ppv_Ptable_GetFirstElem(&m_ListOfSource) + ul_Index;
						CWindSource ** ppLastSource = (CWindSource **) TAB_ppv_Ptable_GetLastElem(&m_ListOfSource);

						while (TAB_b_IsAHole(*ppSource))
						{
							ppSource++;
							ul_Index++;
							if (ppSource > ppLastSource)
							{
								ppSource = (CWindSource **) TAB_ppv_Ptable_GetFirstElem(&m_ListOfSource);
								ul_Index = 0;
							}
						}

						p_CurrentDynWind->SetCurrentIndex(ul_Index);

						if ((*ppSource)->IsInSource(_p_Gao))
						{
							p_CurrentDynWind->AddWindSource(*ppSource);
						}
						else 
						{
							p_CurrentDynWind->RemoveWindSource(*ppSource);
						}
					}

					p_CurrentDynWind->Update();
				}

				result = _p_CurrentWind->GetWind(_p_Wind);
			}
			break;
		default:
			return FALSE;
		}
	}

	// add dynamic wind source contribution if any
	result |= GetDynamicWind(_p_Gao,_p_Wind);

    return result;
}

//------------------------------------------------------------------------
//
// Author			Charles Jacob
// Date				30 Mar 2004
// 
// Description		Add dynamic wind contribution.
// 
//------------------------------------------------------------------------
BOOL CWindManager::GetDynamicWind(OBJ_tdst_GameObject * _p_Gao, MATH_tdst_Vector * _p_Wind)
{	
	BOOL result = FALSE;
	void** elem =  TAB_ppv_Ptable_GetFirstElem(&m_ListOfDynamicSource);
	void** last_elem =  TAB_ppv_Ptable_GetLastElem(&m_ListOfDynamicSource);
	CWindSource* wind;
	MATH_tdst_Vector dynamicWind;
	MATH_tdst_Vector sourceToObject;
	MATH_tdst_Vector speedModulation;


	// for all the dynamic sources
	for(; elem <= last_elem; elem++)
	{	
		if(TAB_b_IsAHole(*elem)) continue;
			
		wind = *(CWindSource**)elem;

		if(wind->GetGao() == _p_Gao)continue;

		// is the gao affected by the wind source
        MATH_tdst_Vector vDistance;
        MATH_SubVector(&vDistance, &_p_Gao->pst_GlobalMatrix->T, &wind->GetGao()->pst_GlobalMatrix->T );
        float squareDistance = MATH_f_SqrNormVector(&vDistance);
		float squareNear = (wind->m_fNear*wind->m_fNear);		
		float squareFar = (wind->m_fFar*wind->m_fFar);		
		if(squareDistance <= squareFar)
		{
			float attenuation = 1.0f;

			// compute attenuation factor if in between falloff value
			if(squareDistance >= squareNear)
			{
				attenuation = (squareFar - squareDistance)/(squareFar - squareNear);
			}

			// get wind direction
			if( wind->m_bIsRadial )
			{
				// radial				
				MATH_SubVector(&dynamicWind,&_p_Gao->pst_GlobalMatrix->T,&wind->GetGao()->pst_GlobalMatrix->T);
				MATH_NormalizeEqualVector(&dynamicWind);
				MATH_ScaleEqualVector(&dynamicWind,wind->GetForce());
			}
			else
			{
				// plane
				if(wind->m_bWindBehindPlane)
				{
					MATH_CopyVector(&dynamicWind, wind->GetWind());
				}
				else
				{										
					MATH_SubVector(&sourceToObject,&_p_Gao->pst_GlobalMatrix->T,&wind->GetGao()->pst_GlobalMatrix->T);
					MATH_CopyVector(&dynamicWind, wind->GetWind());

					// wind contribution is NULL if object is behind the plane
					if( MATH_f_DotProduct(&sourceToObject,&dynamicWind) < 0.0f )
					{
						attenuation = 0.0f;
					}
				}				
			}

			// modulate by attenuation
			MATH_ScaleEqualVector(&dynamicWind,attenuation);

			// displacement modulation
			if(wind->m_bIsSpeedModulated)
			{
				if(wind->m_bFirstInit)
				{
					MATH_CopyVector(&wind->m_prevPos,&wind->GetGao()->pst_GlobalMatrix->T);
					wind->m_bFirstInit = FALSE;
				}				
				
				MATH_SubVector(&speedModulation,&wind->GetGao()->pst_GlobalMatrix->T,&wind->m_prevPos);
				MATH_ScaleEqualVector(&speedModulation,wind->m_fSpeedModulation);				

				MATH_AddEqualVector(&dynamicWind,&speedModulation);
				MATH_CopyVector(&wind->m_prevPos,&wind->GetGao()->pst_GlobalMatrix->T);
			}

			// add to existing wind
			MATH_AddEqualVector(_p_Wind,&dynamicWind);

			result = TRUE;
		}
	}

	return result;
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		Initialise wind manager.
// 
//------------------------------------------------------------------------
void WIND_InitWindManager()
{
    CWindManager::CreateInstance();
}


//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				16 Dec 2002
// 
// Description		Close wind manager.
// 
//------------------------------------------------------------------------
void WIND_CloseWindManager()
{
    CWindManager::DeleteInstance();
}


#ifdef ACTIVE_EDITORS

CWindSource * CWindManager::AffectedBy(OBJ_tdst_GameObject * _p_Gao, BAS_tdst_barray & _MemGroup)
{
    return NULL;

    //FLOAT         f_Dist;
    //FLOAT         f_MinDist = Cf_Infinit;
    //CWindSource * p_BestSource = NULL;

    //CWindSource ** ppSource     = (CWindSource **) TAB_ppv_Ptable_GetFirstElem(&m_ListOfSource);
    //CWindSource ** ppLastSource = (CWindSource **) TAB_ppv_Ptable_GetLastElem(&m_ListOfSource);

    //for(; ppSource <= ppLastSource; ppSource++)
    //{
    //    if (TAB_b_IsAHole(*ppSource))
    //        continue;

    //    if( BAS_bsearch( (*ppSource)->GetKey(), &_MemGroup ) != -1 )
    //    {
    //        if ((*ppSource)->IsInSource(_p_Gao, &f_Dist))
    //        {
    //            if (f_Dist < f_MinDist)
    //            {
    //                f_MinDist = f_Dist;
    //                p_BestSource = *ppSource;
    //            }
    //        }
    //    }
    //}

    //return p_BestSource;
}

//------------------------------------------------------------------------
//
// Author			Stephane Girard
// Date				2 Jan 2003
// 
// Description		
// 
//------------------------------------------------------------------------

void WIND_PreCompute(WOR_tdst_World * _pst_World, BOOL _b_Save /*= FALSE*/)
{
    //TAB_tdst_PFelem     * pst_PFElem;
    //TAB_tdst_PFelem     * pst_PFLastElem;
    //OBJ_tdst_GameObject * pst_Gao;
    //clGroup             * po_Group;
    //CWindSource         * p_Source;
    //stGroup_Table::iterator	iter;

    //pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
    //pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);

    //for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
    //{
    //    pst_Gao = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
    //    if(TAB_b_IsAHole(pst_Gao))
    //        continue;

    //    if( OBJ_uw_ExtraFlagsTest( pst_Gao, OBJ_C_ExtraFlag_AffectedByStaticWind ) )
    //    {
    //        po_Group = NULL;
    //        iter = EOUT_cl_Frame::m_TableOfGOL.begin();
    //        while(po_Group == NULL && iter != EOUT_cl_Frame::m_TableOfGOL.end())
    //        {
    //            if ((*iter)->m_ulWOWKey == pst_Gao->pst_World->h_WorldKey )
    //            {
    //                po_Group = (*iter);
    //            }
    //            iter++;
    //        }

    //        if (po_Group)
    //        {
    //            p_Source = CWindManager::GetInstance()->AffectedBy(pst_Gao, po_Group->m_groupArray );

    //            CCurrentWind * p_CurrentWind = OBJ_pst_GetCurrentWind(pst_Gao);

    //            if (p_CurrentWind->GetType() == CCurrentWind::eType_Static)
    //            {
    //                if (p_Source)
    //                {
    //                    //WOR_MakeSureGAOIsInAllThisWOW(pst_Gao, p_Source->GetGao(), "Wind");
    //                    ((CCurrentStaticWind *)p_CurrentWind)->Initialise(p_Source);
    //                }
    //                else
    //                {
    //                    ((CCurrentStaticWind *)p_CurrentWind)->Initialise(NULL);
    //                }
    //            }

    //            if (_b_Save)
    //            {
    //                char  asz_Path[BIG_C_MaxLenPath];
    //                ULONG ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Gao);
    //                ULONG ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
    //                BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
    //                //OBJ_ul_GameObject_SaveContent(_pst_World, pst_Gao, asz_Path);
    //            }
    //        }
    //        else
    //        {
    //            ERR_X_ErrorAssert(0, "WIND_PreCompute", NULL);
    //        }
    //    }
    //}
}

#endif // #ifdef ACTIVE_EDITORS