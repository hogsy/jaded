/*$T WORcheck.h GC!1.5 10/06/99 16:06:39 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifndef __WORCHECK_H__
#define __WORCHECK_H__
#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS

void	WORCheck_AllRef_Close( void );
void	WORCheck_AllRef_Init( void );
void	WORCheck_AllRef_AddRef( ULONG , ULONG );
ULONG	WORCheck_AllRef_FatherOf( ULONG , void *, ULONG );
ULONG	WORCheck_AllRef_ChildOf( ULONG , void *, ULONG );

void    WOR_Check(WOR_tdst_World *_pst_World);
void    WOR_Check_InfoPhoto( WOR_tdst_World *, int );

void    WORCheck_LoadAllGao(void);
void    WORCheck_FreeAllGao(void);
ULONG   WORCheck_ul_SearchMaterial(ULONG *, ULONG );

void    WORCheck_LoadAllGroup( void );
void    WORCheck_GroupStat( void );
void    WORCheck_GroupOwnerOfFile( ULONG );
void    WORCheck_GroupOwnerOfDir( ULONG );
void    WORCheck_GaoNotInGroup( char *, ULONG );

#else

#define WOR_Check(a)
#define WOR_Check_InfoPhoto(a, b)

#define WORCheck_LoadAllGao
#define WORCheck_FreeAllGao
#define WORCheck_ul_SearchMaterial(a, b)    0

#define WORCheck_LoadAllGroup
#define WORCheck_GroupStat
#define WORCheck_GroupOwnerOfFile(a)
#define WORCheck_GroupOwnerOfDir(a)
#define WORCheck_GaoNotInGroup(a, b)

#endif /* ACTIVE_EDITORS */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __WORCHECK_H */ 
 