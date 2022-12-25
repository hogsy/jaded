/*$T NET_struct.h GC!1.52 12/13/99 10:48:01 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifndef __NET_STRUCT_H__
#define __NET_STRUCT_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/* Max len of a player name */
#define NET_C_MaxPlayerName             32
#define NET_C_PlayerGameDataMaxSize     512

#define NET_C_PlayerFlag_Used           1
#define NET_C_PlayerFlag_Connected      2

#define NET_C_ServerFlag_Used           1
#define NET_C_ServerFlag_Connected      2

/*
 ---------------------------------------------------------------------------------------------------
    Description of a message
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  NET_tdst_Message_
{
#ifdef WIN32
    struct sockaddr_in  st_From;
#endif    
    int                 i_Msg;
    int                 i_Size;
    char                *pc_Data;
} NET_tdst_Message;

/*
 ---------------------------------------------------------------------------------------------------
    Player description
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  NET_tdst_Player_
{
#ifdef WIN32
    struct sockaddr_in          st_Addr;
#endif    

    ULONG                       ul_Flags;
    char                        asz_Name[NET_C_MaxPlayerName];
    char                        ac_GameData[ NET_C_PlayerGameDataMaxSize ];
    char                        *pc_GameDataCur;
} NET_tdst_Player;

typedef struct NET_tdst_Server_
{
#ifdef WIN32
    struct sockaddr_in          st_Addr;
#endif    

    ULONG                       ul_Flags;
    char                        asz_Name[NET_C_MaxPlayerName];
    char                        ac_GameData[ NET_C_PlayerGameDataMaxSize ];
    char                        *pc_GameDataCur;

    short                       w_NumberOfPlayer;
    short                       w_MaxPlayer;
    NET_tdst_Player             *pst_Player;
} NET_tdst_Server;

typedef struct NET_tdst_ServerList_
{
#ifdef WIN32
    struct sockaddr_in          st_Addr;
#endif    
    char                            asz_Name[ NET_C_MaxPlayerName ];
    short                           w_NumberOfPlayer;
    short                           w_MaxPlayer;
    struct NET_tdst_ServerList_     *pst_Next;
} NET_tdst_ServerList;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __NET_STRUCT_H__ */
