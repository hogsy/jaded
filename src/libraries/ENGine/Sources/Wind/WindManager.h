//------------------------------------------------------------------------
//
// (C) Copyright 2002 Ubisoft
//
// Author		Stephane Girard
// Date			11 Dec 2002
//
// File			WindManager.h
// Description
//
//------------------------------------------------------------------------
#ifndef __WINDMANAGER_H__INCLUDED
#define __WINDMANAGER_H__INCLUDED

#include "ENGine/Sources/Wind/WindSource.h"
#include "ENGine/Sources/Wind/CurrentWind.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/BIGgroup.h"
#endif // #ifdef ACTIVE_EDITORS

struct WOR_tdst_World_;

class CWindManager
{
 private:

     static CWindManager * m_Instance;       // Singleton instance

     CWindManager();
     ~CWindManager();

     TAB_tdst_Ptable m_ListOfSource;	// contains the classic static wind sources
	 TAB_tdst_Ptable m_ListOfDynamicSource;	// contains the moving wind sources

	 BOOL GetDynamicWind(OBJ_tdst_GameObject * _p_Gao, MATH_tdst_Vector * _p_Wind);
 public:

     M_DeclareOperatorNewAndDelete();

     // Singleton

     static void CreateInstance();
     static void DeleteInstance();
     static CWindManager * GetInstance();

     // Interface
     void AddWindSource(CWindSource * _p_Source);
     void RemoveWindSource(CWindSource * _p_Source);

     BOOL GetWind(OBJ_tdst_GameObject * _p_Gao, MATH_tdst_Vector * _p_Wind);
     BOOL GetWind(OBJ_tdst_GameObject * _p_Gao, CCurrentWind * _p_CurrentWind, MATH_tdst_Vector * _p_Wind);

     #ifdef ACTIVE_EDITORS
     CWindSource * AffectedBy(OBJ_tdst_GameObject * _p_Gao, BAS_tdst_barray & _Group);
     #endif // #ifdef ACTIVE_EDITORS
};

void WIND_InitWindManager();
void WIND_CloseWindManager();

#ifdef ACTIVE_EDITORS

void WIND_PreCompute(WOR_tdst_World_ * _pst_World, BOOL _b_Save = FALSE);

#endif // #ifdef ACTIVE_EDITORS

#endif // #ifndef __WINDMANAGER_H__INCLUDED