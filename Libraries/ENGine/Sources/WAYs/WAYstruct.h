/*$T WAYstruct.h GC!1.41 08/24/99 14:20:46 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __WAYSTRUCT_H__
#define __WAYSTRUCT_H__

#include "ENGine/Sources/OBJects/OBJstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/* Flags for a network */
#define WAY_C_DisplayNet    0x00000001      /* To display all the links of the network */
#define WAY_C_DisplayBV     0x00000002      /* Display all WP with their BV, else display WP alone */
#define WAY_C_HasBeenMerge	0x00000004		/* Network is in a sub .wow */

/*
 ---------------------------------------------------------------------------------------------------
    Definition of all networks
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  WAY_tdst_Network_
{
    OBJ_tdst_GameObject *pst_Root;          /* Root gameobject */
    ULONG               ul_Flags;
} WAY_tdst_Network;

typedef struct  WAY_tdst_AllNetworks_
{
    WAY_tdst_Network    **ppst_AllNetworks; /* All the networks (root gameobject) */
    ULONG               ul_Num;             /* Number of networks */
} WAY_tdst_AllNetworks;

/*
 ---------------------------------------------------------------------------------------------------
    Definition of a link
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  WAY_tdst_Link_
{
	ULONG				uw_Capacities;
	ULONG               uw_CapacitiesInit;
	float				f_Pound;
    OBJ_tdst_GameObject *pst_Next;          /* Linked object */
	char				c_Design;
	char				c_DesignInit;
	USHORT				uw_Design2;
} WAY_tdst_Link;

/*
 ---------------------------------------------------------------------------------------------------
    Definition of a list of links
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  WAY_tdst_LinkList_
{
    WAY_tdst_Network    *pst_Network;       /* Network attached to the link list */
    WAY_tdst_Link       *pst_Links;         /* All the links of that network */
    ULONG               ul_Num;             /* Number of links */
} WAY_tdst_LinkList;

/*
 ---------------------------------------------------------------------------------------------------
    MS to describe a network in an object
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  WAY_tdst_Struct_
{
    WAY_tdst_LinkList   *pst_AllLinks;      /* All the link lists for that object */
    ULONG               ul_Num;             /* Number of list of links */
} WAY_tdst_Struct;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __WAYSTRUCT_H__ */
