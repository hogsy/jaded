/*$T WAYinit.h GC!1.53 11/19/99 10:41:49 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __WAYINIT_H__
#define __WAYINIT_H__

#include "ENGine/Sources/WAYs/WAYstruct.h"
#include "GraphicDK/Sources/SELection/SELection.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern void                 WAY_RemoveAllNetworks(WAY_tdst_AllNetworks *);
extern void                 WAY_FreeStruct(WAY_tdst_Struct *);
extern WAY_tdst_LinkList    *WAY_pst_SearchLinkList(OBJ_tdst_GameObject *, WAY_tdst_Network *);
extern WAY_tdst_Link        *WAY_pst_SearchOneLinkInList
                            (
                                WAY_tdst_LinkList *,
                                OBJ_tdst_GameObject *
                            );
extern WAY_tdst_LinkList    *WAY_pst_SearchOneLinkList(OBJ_tdst_GameObject *, WAY_tdst_Link *);
extern WAY_tdst_Network     *WAY_pst_SearchNetwork(WOR_tdst_World *, char *, OBJ_tdst_GameObject *);
extern WAY_tdst_Network     *WAY_pst_CreateNetwork(WOR_tdst_World *, char *, OBJ_tdst_GameObject *);
extern WAY_tdst_Struct      *WAY_pst_CreateStruct(WOR_tdst_World *, OBJ_tdst_GameObject *);
extern WAY_tdst_LinkList    *WAY_pst_AddOneLinkList(OBJ_tdst_GameObject *, WAY_tdst_Network *);
extern WAY_tdst_Link        *WAY_pst_AddOneLink
                            (
                                OBJ_tdst_GameObject *,
                                WAY_tdst_LinkList *,
                                OBJ_tdst_GameObject *
                            );
extern void                 WAY_DelOneLink
                            (
                                OBJ_tdst_GameObject *,
                                WAY_tdst_LinkList *,
                                OBJ_tdst_GameObject *
                            );
extern void                 WAY_RegisterLink
                            (
                                WAY_tdst_Network *,
                                OBJ_tdst_GameObject *,
                                WAY_tdst_Link *
                            );
extern void                 WAY_RegisterAllLinks(OBJ_tdst_GameObject *);
extern void                 WAY_UnRegisterAllLinks(OBJ_tdst_GameObject *);
extern void                 WAY_DelOneNetwork(WOR_tdst_World *, WAY_tdst_Network *,BOOL , BOOL);
extern void                 WAY_ReinitLinks(WAY_tdst_Struct *, int);
extern void                 WAY_ComputePoundNetworks(WOR_tdst_World *);
extern void                 WAY_ComputePoundOneNetwork(WAY_tdst_Network *);
extern void                 WAY_ComputePoundOneNetworkForObj
                            (
                                WAY_tdst_Network *,
                                OBJ_tdst_GameObject *
                            );

#ifdef ACTIVE_EDITORS
extern void					WAY_CheckObjectInNetwork(OBJ_tdst_GameObject *);
extern void                 WAY_WhenDestroyAnObject(WOR_tdst_World *, OBJ_tdst_GameObject *);
#endif //ACTIVE_EDITORS

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __WAYINIT_H__ */