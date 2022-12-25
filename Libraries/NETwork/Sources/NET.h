/*$T NET.h GC! 1.081 06/14/02 14:44:11 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __NET_H__
#define __NET_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    constant
 ***********************************************************************************************************************
 */

/* net primary command */
#define NETCMD_RequestServer	1	/* no data */
#define NETCMD_ServerAnswer		2	/* data = (ULONG) name length, (...) server name */
#define NETCMD_PlayerJoin		3	/* data = (32) player name */
#define NETCMD_PlayerLeave		4	/* no data */
#define NETCMD_PlayerAccepted	5	/* no data */
#define NETCMD_PlayerRejected	6	/* no data */
#define NETCMD_ServerShutdown	7	/* no data */
#define NETCMD_PlayerGameData	8	/* no data */
#define NETCMD_ServerGameData	9

#define NETCMP_BroadcastMessage 50	/* (ULONG) message length, (...) message content */
#define NETCMP_Message			51	/* (ULONG) message length, (...) message content */

/*$4
 ***********************************************************************************************************************
    globals
 ***********************************************************************************************************************
 */

extern int						NET_i_InSocket;
extern unsigned short			NET_uw_JadePort;

extern BOOL						NET_b_Server;
extern BOOL						NET_b_Init;
extern BOOL						NET_b_Open;
extern BOOL						NET_b_Client;

extern struct NET_tdst_Server_	NET_st_Server;
extern struct NET_tdst_Player_	NET_st_Me;

/*$4
 ***********************************************************************************************************************
    functions WIN32
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
#define NET_ServerUpdate	NETSRV_Update
#define NET_PlayerUpdate	NETCLT_Update

/*$1- general function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void						NET_Init(void);
extern void						NET_Open(void);
extern void						NET_Close(void);

/*$1- server function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void						NETSRV_Create(char *, int);
extern void						NETSRV_Destroy(void);
extern void						NETSRV_Update(void);
extern int						NETSRV_SendGameData(void);
extern int						NETSRV_CanPop(int);
extern int						NETSRV_PlayerNumberGet(void);
extern int						NETSRV_PlayerMaxGet(void);
extern char						*NETSRV_PlayerGetName(int);

/*$1- client function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void						NETCLT_SetName(char *);
extern int						NETCLT_JoinCurrentServer(void);
extern void						NETCLT_Leave(void);
extern void						NETCLT_Update(void);
extern int						NETCLT_CanPopData(void);
extern int						NETCLT_SendGameData(void);

/*$1- server list function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern char						*NETSRVL_GetName(int);
extern int						NETSRVL_GetMaxPlayers(int);
extern int						NETSRVL_GetNbPlayers(int);
extern int						NETSRVL_Request(void);
extern int						NETSRVL_SetCurrent(int);

/*$1- gamedata function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void						NET_GameDataReset(void);
extern int						NET_GameDataCanPop(void);
extern int						NET_GameDataPushi(int, int);
extern int						NET_GameDataPushf(float);
extern int						NET_GameDataPushstr(char *);
extern int						NET_GameDataPushv(struct MATH_tdst_Vector_ *);
extern int						NET_ServerSetPlayerForPop(int);
extern int						NET_ServerCanPopPlayer(void);
extern int						NET_GameDataPopi(int);
extern float					NET_GameDataPopf(void);
extern char						*NET_GameDataPopstr(void);
extern struct MATH_tdst_Vector_ *NET_GameDataPopv(struct MATH_tdst_Vector_ *);

/*$1- function defined in NET_io.c ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void						NET_PacketSend(struct sockaddr_in *, void *, int);
extern void						NET_BroadcastPacketSend(unsigned short, void *, int);
extern int						NET_i_PacketGet(void *, int, struct sockaddr_in *);

/*$4
 ***********************************************************************************************************************
    NO NETWORK
 ***********************************************************************************************************************
 */

#else
#define NET_Init()
#define NET_Open()
#define NET_Close()

#define NETSRV_Create(a, b)
#define NETSRV_Destroy()
#define NET_ServerUpdate()
#define NETSRV_Update()
#define NETSRV_SendGameData()		0
#define NETSRV_CanPop(a)			0
#define NETSRV_PlayerNumberGet()	0
#define NETSRV_PlayerMaxGet()		0
#define NETSRV_PlayerGetName(a)		NULL

#define NETCLT_SetName(a)
#define NETCLT_JoinCurrentServer()	0
#define NETCLT_Leave()
#define NET_PlayerUpdate()
#define NETCLT_Update()
#define NETCLT_CanPopData()			0
#define NETCLT_SendGameData()		0

#define NETSRVL_GetName(a)			NULL
#define NETSRVL_GetMaxPlayers(a)	0
#define NETSRVL_GetNbPlayers(a)		0
#define NETSRVL_Request()			0
#define NETSRVL_SetCurrent(a)		0

#define NET_GameDataReset()
#define NET_GameDataCanPop()		0
#define NET_GameDataPushi(a, b)		0
#define NET_GameDataPushf(a)		0
#define NET_GameDataPushstr(a)		0
#define NET_GameDataPushv(a)		0
#define NET_GameDataPopi(a)			0
#define NET_GameDataPopf()			0.0f
#define NET_GameDataPopstr()		NULL
#define NET_GameDataPopv(a)			a
#define NET_PlayerSendGameData()	0
#define NET_ServerSendGameData()	0

#define NET_PacketSend(a, b, c)
#define NET_BroadcastPacketSend(a, b, c)
#define NET_i_PacketGet(a, b, c)	0

#endif /* TEST PLATFORM */

#ifdef NET_LOG
extern void NET_Error(char *);
#else
#define NET_Error(a)
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __NET_H__ */
