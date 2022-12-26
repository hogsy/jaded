/*$T NET.c GC! 1.081 06/14/02 12:25:32 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef JADEFUSION
#include "math.h"
#endif
#include "NET.h"
#include "NET_struct.h"
#ifndef JADEFUSION
#include "MATHs/MATH.h"
#endif
#include "TIMer/TIMdefs.h"

BOOL				NET_b_Server = FALSE;
BOOL				NET_b_Init = FALSE;
BOOL				NET_b_Open = FALSE;
BOOL				NET_b_Client = FALSE;

#ifdef ACTIVE_EDITORS
#define IPPORT_USERRESERVED 26000
int					NET_i_SendSocket;
int					NET_i_InSocket;
unsigned short		NET_uw_JadePort;

NET_tdst_Player		NET_st_Me;
NET_tdst_Server		NET_st_Server;

NET_tdst_ServerList *NET_pst_ServerList = NULL;
NET_tdst_ServerList *NET_pst_ServerCurrent = NULL;

NET_tdst_Player		*NET_pst_Player4GameDataPop = NULL;

/*$4
 ***********************************************************************************************************************
    prototypes of internal functions
 ***********************************************************************************************************************
 */

/*$1- server list function */
NET_tdst_ServerList *NETSRVL_GetByAddr(struct sockaddr_in *);

/*$4
 ***********************************************************************************************************************
    misc functions
 ***********************************************************************************************************************
 */

#if 0	/* function not used */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NET_i_GetLocalName(char *_sz_Name, int _l_Length)
{
	return gethostname(_sz_Name, _l_Length);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NET_GetLocalAddress(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			hostname[1024];
	struct hostent	*hostentry;
	int				err;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	err = gethostname(hostname, sizeof(hostname));
	if(err == -1) return -1;
	hostentry = gethostbyname(hostname);
	if(!hostentry) return -1;
	return *(int *) hostentry->h_addr_list[0];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NET_i_Address2String(struct sockaddr_in *addr, char *sz_Text)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	*c;
	ULONG			ul_IP;
	unsigned short	uw_Port;
	/*~~~~~~~~~~~~~~~~~~~~*/

	c = (unsigned char *) &ul_IP;
	ul_IP = ntohl(addr->sin_addr.S_un.S_addr);
	uw_Port = ntohs(addr->sin_port);

	return sprintf(sz_Text, "[%d.%d.%d.%d:%d]", c[0], c[1], c[2], c[3], uw_Port);
}

#endif

/*$4
 ***********************************************************************************************************************
    general functions (init / open / close)
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NET_Init(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	WORD	wVersionRequested;
	WSADATA wsaData;
	int		err;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(NET_b_Init) return;

	NET_uw_JadePort = (IPPORT_USERRESERVED + 0x1d);
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if(err != 0) return;

	NET_b_Init = TRUE;

	NET_Open();
	if(!NET_b_Open)
	{
		NET_uw_JadePort++;
		NET_Open();
		if(!NET_b_Open)
		{
			NET_uw_JadePort++;
			NET_Open();
		}
	}

	L_memset(&NET_st_Server, 0, sizeof(NET_st_Server));
	NET_st_Server.pc_GameDataCur = NET_st_Server.ac_GameData;
	NET_st_Server.w_MaxPlayer = 4;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NET_Open(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					err, option;
	ULONG				trueval;
	struct sockaddr_in	address;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!NET_b_Init) return;
	if(NET_b_Open) NET_Close();

	NET_i_InSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(NET_i_InSocket == -1) return;

	option = ~0;
	err = setsockopt(NET_i_InSocket, SOL_SOCKET, SO_BROADCAST, (char *) &option, sizeof(option));
	if(err == -1) return;

	L_memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(NET_uw_JadePort);
#if defined (__cplusplus)
	err = bind(NET_i_InSocket, (const sockaddr *) &address, sizeof(address));
#else
	err = bind(NET_i_InSocket, (void *) &address, sizeof(address));
#endif
	if(err == -1) return;

	trueval = 1;
	ioctlsocket(NET_i_InSocket, FIONBIO, &trueval);

	NET_b_Open = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NET_Close(void)
{
	if(!NET_b_Init) return;
	if(!NET_b_Open) return;

	if(NET_b_Server) NETSRV_Destroy();
	if(NET_b_Client) NETCLT_Leave();

	closesocket(NET_i_InSocket);
	NET_b_Open = FALSE;
	NET_b_Server = FALSE;
	NET_b_Client = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NET_SendMsgTo(NET_tdst_Message *_pst_Msg, struct sockaddr_in *_pst_To)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	char	pc_Buffer[1024];
	/*~~~~~~~~~~~~~~~~~~~~*/

	*(int *) pc_Buffer = _pst_Msg->i_Msg;
	L_memcpy(pc_Buffer + 4, _pst_Msg->pc_Data, _pst_Msg->i_Size);

	NET_PacketSend(_pst_To, pc_Buffer, _pst_Msg->i_Size + 4);
}

/*$4
 ***********************************************************************************************************************
    server functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NETSRV_Create(char *sz_Name, int _i_NbMax)
{
	L_memcpy(NET_st_Server.asz_Name, sz_Name, 32);
	NET_st_Server.asz_Name[31] = 0;
	NET_st_Server.pc_GameDataCur = NET_st_Server.ac_GameData;
	NET_st_Server.w_MaxPlayer = _i_NbMax;
	NET_st_Server.pst_Player = (NET_tdst_Player*)L_malloc(NET_st_Server.w_MaxPlayer * sizeof(NET_tdst_Player));
	NET_b_Server = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NETSRV_Destroy(void)
{
	/*~~~~~~~~~*/
	int i, i_Cmd;
	/*~~~~~~~~~*/

	if(!NET_b_Server) return;
	if(!(NET_st_Server.ul_Flags & NET_C_ServerFlag_Used)) return;

	i_Cmd = NETCMD_ServerShutdown;

	for(i = 0; i < NET_st_Server.w_MaxPlayer; i++)
	{
		if(NET_st_Server.pst_Player[i].ul_Flags & NET_C_PlayerFlag_Used)
			NET_PacketSend(&NET_st_Server.pst_Player[i].st_Addr, &i_Cmd, 4);
	}

	L_free(NET_st_Server.pst_Player);

	L_memset(&NET_st_Server, 0, sizeof(NET_st_Server));
	NET_b_Server = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NETSRV_UpdateNbPlayer(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	NET_tdst_Player *pst_LastPlayer, *pst_Player;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	NET_st_Server.w_NumberOfPlayer = 0;
	if(!NET_b_Server) return;

	pst_Player = NET_st_Server.pst_Player;
	pst_LastPlayer = pst_Player + NET_st_Server.w_MaxPlayer;
	for(; pst_Player < pst_LastPlayer; pst_Player++)
	{
		if(pst_Player->ul_Flags & NET_C_PlayerFlag_Used) NET_st_Server.w_NumberOfPlayer++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
NET_tdst_Player *NETSRV_GetPlayerById(int i)
{
	if(!NET_b_Server || (!NET_st_Server.ul_Flags & NET_C_ServerFlag_Used)) return NULL;
	if((i < 0) || (i >= NET_st_Server.w_MaxPlayer)) return NULL;
	return NET_st_Server.pst_Player + i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
NET_tdst_Player *NETSRV_GetPlayerByAddr(struct sockaddr_in *addr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	NET_tdst_Player *pst_LastPlayer, *pst_Player;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!NET_b_Server || (!NET_st_Server.ul_Flags & NET_C_ServerFlag_Used)) return NULL;

	pst_Player = NET_st_Server.pst_Player;
	pst_LastPlayer = pst_Player + NET_st_Server.w_MaxPlayer;
	for(; pst_Player < pst_LastPlayer; pst_Player++)
	{
		if(!(pst_Player->ul_Flags & NET_C_PlayerFlag_Used)) continue;
		if(!L_memcmp(&pst_Player->st_Addr, addr, 4 + sizeof(struct in_addr))) return pst_Player;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NETSRV_Update(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				pc_Buffer[1024];
	char				pc_Answer[1024];
	int					i_Size, i_Cmd;
	struct sockaddr_in	st_From;
	NET_tdst_Player		*pst_LastPlayer, *pst_Player;
	NET_tdst_Message	st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(NET_b_Init && NET_b_Server)) return;

	while(1)
	{
		i_Size = NET_i_PacketGet(pc_Buffer, 1024, &st_From);
		if(i_Size <= 0) return;

		if(*(int *) pc_Buffer == NETCMD_RequestServer)
		{
			/* client searching for server, send back server name */
			*(int *) pc_Buffer = NETCMD_ServerAnswer;
			*(short *) (pc_Buffer + 4) = NET_st_Server.w_MaxPlayer;
			*(short *) (pc_Buffer + 6) = NET_st_Server.w_NumberOfPlayer;
			strcpy(pc_Buffer + 8, NET_st_Server.asz_Name);

			NET_PacketSend(&st_From, pc_Buffer, strlen(NET_st_Server.asz_Name) + 9);
		}
		else if(*(int *) pc_Buffer == NETCMP_BroadcastMessage)
		{
			pst_Player = NETSRV_GetPlayerByAddr(&st_From);
			sprintf(pc_Answer + 4, "[%s] %s", pst_Player ? pst_Player->asz_Name : "Unknow", pc_Buffer + 8);
			*(int *) pc_Answer = NETCMP_Message;
			i_Size = strlen(pc_Answer + 4) + 5;

			pst_Player = NET_st_Server.pst_Player;
			pst_LastPlayer = pst_Player + NET_st_Server.w_MaxPlayer;
			for(; pst_Player < pst_LastPlayer; pst_Player++)
			{
				if(pst_Player->ul_Flags & NET_C_PlayerFlag_Used)
					NET_PacketSend(&pst_Player->st_Addr, pc_Answer, i_Size);
			}
		}
		else if(*(int *) pc_Buffer == NETCMD_PlayerJoin)
		{
			/* add player to list */
			pst_Player = NET_st_Server.pst_Player;
			pst_LastPlayer = pst_Player + NET_st_Server.w_MaxPlayer;
			for(; pst_Player < pst_LastPlayer; pst_Player++)
				if(!(pst_Player->ul_Flags & NET_C_PlayerFlag_Used)) break;

			if(pst_Player == pst_LastPlayer)
			{
				i_Cmd = NETCMD_PlayerRejected;
				NET_PacketSend(&st_From, &i_Cmd, 4);
			}
			else
			{
				pst_Player->st_Addr = st_From;
				pst_Player->pc_GameDataCur = pst_Player->ac_GameData;
				L_memcpy(pst_Player->asz_Name, pc_Buffer + 4, 32);
				i_Cmd = NETCMD_PlayerAccepted;
				NET_PacketSend(&st_From, &i_Cmd, 4);
			}

			NETSRV_UpdateNbPlayer();
		}
		else if(*(int *) pc_Buffer == NETCMD_PlayerLeave)
		{
			/* remove a player from list */
			pst_Player = NETSRV_GetPlayerByAddr(&st_From);
			if(pst_Player) pst_Player->ul_Flags = 0;
			NETSRV_UpdateNbPlayer();
		}
		else if(*(int *) pc_Buffer == NETCMD_PlayerGameData)
		{
			pst_Player = NETSRV_GetPlayerByAddr(&st_From);
			if(pst_Player)
			{
				st_Msg.i_Size = (i_Size - 4 < NET_C_PlayerGameDataMaxSize) ? i_Size - 4 : NET_C_PlayerGameDataMaxSize;
				L_memcpy(pst_Player->ac_GameData, pc_Buffer + 4, st_Msg.i_Size);
				pst_Player->pc_GameDataCur = pst_Player->ac_GameData + st_Msg.i_Size;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NETSRV_BroadcastMsg(NET_tdst_Message *_pst_Msg)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	int		i;
	char	pc_Buffer[1024];
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(!NET_b_Server || (!NET_st_Server.ul_Flags & NET_C_ServerFlag_Used)) return;
	if(!NET_st_Server.w_NumberOfPlayer) return;

	*(int *) pc_Buffer = _pst_Msg->i_Msg;
	L_memcpy(pc_Buffer + 4, _pst_Msg->pc_Data, _pst_Msg->i_Size);

	for(i = 0; i < NET_st_Server.w_NumberOfPlayer; i++)
		NET_PacketSend(&NET_st_Server.pst_Player[i].st_Addr, pc_Buffer, _pst_Msg->i_Size + 4);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NETSRV_SendGameData(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	NET_tdst_Message	st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	st_Msg.i_Msg = NETCMD_ServerGameData;
	st_Msg.i_Size = NET_st_Me.pc_GameDataCur - NET_st_Me.ac_GameData;
	st_Msg.pc_Data = NET_st_Me.ac_GameData;
	st_Msg.st_From = NET_st_Me.st_Addr;

	NET_st_Me.pc_GameDataCur = NET_st_Me.ac_GameData;

	NETSRV_BroadcastMsg(&st_Msg);
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NETSRV_CanPop(int i)
{
	if(!NET_b_Server || (!NET_st_Server.ul_Flags & NET_C_ServerFlag_Used)) return 0;
	if((i < 0) || (i >= NET_st_Server.w_MaxPlayer)) return 0;
	NET_pst_Player4GameDataPop = &NET_st_Server.pst_Player[i];
	return NET_GameDataCanPop();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NETSRV_PlayerNumberGet(void)
{
	NETSRV_UpdateNbPlayer();
	return NET_st_Server.w_NumberOfPlayer;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NETSRV_PlayerNumberMaxGet(void)
{
	return NET_st_Server.w_MaxPlayer;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *NETSRV_PlayerGetName(int i)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	NET_tdst_Player *pst_Player;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!NET_b_Server || (!NET_st_Server.ul_Flags & NET_C_ServerFlag_Used)) return NULL;
	if((i < 0) || (i >= NET_st_Server.w_MaxPlayer)) return NULL;

	pst_Player = NET_st_Server.pst_Player + i;
	if(pst_Player->ul_Flags & NET_C_PlayerFlag_Connected) return NULL;
	return pst_Player->asz_Name;
}

/*$4
 ***********************************************************************************************************************
    client functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NETCLT_WaitAnswer(int _i_Response)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				pc_Buffer[1024];
	struct sockaddr_in	st_From;
	int					i_Size, i_Ret;
	NET_tdst_ServerList *pst_Server;
	float				f_TimeStart;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_TimeStart = TIM_f_Clock_TrueRead();
	i_Ret = 0;

	while(TIM_f_Clock_TrueRead() - f_TimeStart < 1)
	{
		while(1)
		{
			i_Size = NET_i_PacketGet(pc_Buffer, 1024, &st_From);
			if(i_Size == 0) break;

			if(_i_Response == NETCMD_ServerAnswer)
			{
				if(*(int *) pc_Buffer == NETCMD_ServerAnswer)
				{
					if(!NETSRVL_GetByAddr(&st_From))
					{
						i_Ret++;
						pst_Server = (NET_tdst_ServerList *) L_malloc(sizeof(NET_tdst_Player));
						pst_Server->st_Addr = st_From;
						pst_Server->w_MaxPlayer = *(short *) (pc_Buffer + 4);
						pst_Server->w_NumberOfPlayer = *(short *) (pc_Buffer + 6);
						L_memcpy(pst_Server->asz_Name, pc_Buffer + 8, i_Size - 8);
						pst_Server->pst_Next = NET_pst_ServerList;
						NET_pst_ServerList = pst_Server;
					}
				}
			}
			else if(_i_Response == NETCMD_PlayerAccepted)
			{
				if(*(int *) pc_Buffer == NETCMD_PlayerAccepted)
					return 1;
				else if(*(int *) pc_Buffer == NETCMD_PlayerRejected) return 0;
			}
		}
	}

	return i_Ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NETCLT_SetName(char *_sz_Name)
{
	if(strlen(_sz_Name) >= NET_C_MaxPlayerName) _sz_Name[NET_C_MaxPlayerName - 1] = 0;
	L_strcpy(NET_st_Me.asz_Name, _sz_Name);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NETCLT_Join(NET_tdst_ServerList *_pst_Server)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	pc_Buffer[40];
	/*~~~~~~~~~~~~~~~~~~*/

	if(NET_b_Client) NETCLT_Leave();

	NET_st_Server.st_Addr = _pst_Server->st_Addr;
	NET_st_Server.w_MaxPlayer = _pst_Server->w_MaxPlayer;
	NET_st_Server.w_NumberOfPlayer = _pst_Server->w_NumberOfPlayer;
	L_strcpy(NET_st_Server.asz_Name, _pst_Server->asz_Name);
	NET_st_Server.pc_GameDataCur = NET_st_Server.ac_GameData;
	NET_st_Server.ul_Flags = 0;

	/* send message to server to register me as player */
	*(int *) pc_Buffer = NETCMD_PlayerJoin;
	L_memcpy(pc_Buffer + 4, NET_st_Me.asz_Name, 32);
	NET_PacketSend(&NET_st_Server.st_Addr, pc_Buffer, 36);

	if(NETCLT_WaitAnswer(NETCMD_PlayerAccepted)) NET_b_Client = TRUE;
	return(NET_b_Client ? 1 : 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NETCLT_JoinCurrentServer(void)
{
	return NETCLT_Join(NET_pst_ServerCurrent);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NETCLT_Leave(void)
{
	/*~~~~~~*/
	int i_Cmd;
	/*~~~~~~*/

	NET_b_Client = FALSE;
	i_Cmd = NETCMD_PlayerLeave;
	NET_PacketSend(&NET_st_Server.st_Addr, &i_Cmd, 4);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NETCLT_SendMsgToServer(NET_tdst_Message *_pst_Msg)
{
	NET_SendMsgTo(_pst_Msg, &NET_st_Server.st_Addr);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NETCLT_Update(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				pc_Buffer[1024];
	int					i_Size;
	struct sockaddr_in	st_From;
	NET_tdst_Message	st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(NET_b_Init && NET_b_Client)) return;

	while(1)
	{
		i_Size = NET_i_PacketGet(pc_Buffer, 1024, &st_From);
		if(i_Size <= 0) break;;

		if(*(int *) pc_Buffer == NETCMD_ServerShutdown) NET_b_Client = FALSE;

		if(*(int *) pc_Buffer == NETCMD_ServerGameData)
		{
			st_Msg.i_Size = (i_Size - 4 < NET_C_PlayerGameDataMaxSize) ? i_Size - 4 : NET_C_PlayerGameDataMaxSize;
			L_memcpy(NET_st_Server.ac_GameData, pc_Buffer + 4, st_Msg.i_Size);
			NET_st_Server.pc_GameDataCur = NET_st_Server.ac_GameData + st_Msg.i_Size;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NETCLT_CanPopData(void)
{
	NET_pst_Player4GameDataPop = (NET_tdst_Player *) &NET_st_Server;
	return NET_GameDataCanPop();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NETCLT_SendGameData(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	NET_tdst_Message	st_Msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	st_Msg.i_Msg = NETCMD_PlayerGameData;
	st_Msg.i_Size = NET_st_Me.pc_GameDataCur - NET_st_Me.ac_GameData;
	st_Msg.pc_Data = NET_st_Me.ac_GameData;
	st_Msg.st_From = NET_st_Me.st_Addr;

	NET_st_Me.pc_GameDataCur = NET_st_Me.ac_GameData;

	NETCLT_SendMsgToServer(&st_Msg);
	return 1;
}

/*$4
 ***********************************************************************************************************************
    List of server
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NETSRVL_Free(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	NET_tdst_ServerList *pst_Server;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	while(NET_pst_ServerList)
	{
		pst_Server = NET_pst_ServerList;
		NET_pst_ServerList = pst_Server->pst_Next;
		L_free(pst_Server);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
NET_tdst_ServerList *NETSRVL_GetByIndex(int i)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	NET_tdst_ServerList *pst_Server;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Server = NET_pst_ServerList;
	while(i-- && pst_Server) pst_Server = pst_Server->pst_Next;
	return pst_Server;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
NET_tdst_ServerList *NETSRVL_GetByAddr(struct sockaddr_in *_pst_From)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	NET_tdst_ServerList *pst_Server;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Server = NET_pst_ServerList;
	while(pst_Server)
	{
		if(!L_memcmp(&pst_Server->st_Addr, _pst_From, 4 + sizeof(struct in_addr))) return pst_Server;
		pst_Server = pst_Server->pst_Next;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *NETSRVL_GetName(int i)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	NET_tdst_ServerList *pst_Server;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Server = NETSRVL_GetByIndex(i);
	return(pst_Server ? pst_Server->asz_Name : NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NETSRVL_GetMaxPlayers(int i)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	NET_tdst_ServerList *pst_Server;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Server = NETSRVL_GetByIndex(i);
	return(pst_Server ? pst_Server->w_MaxPlayer : NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NETSRVL_GetNbPlayers(int i)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	NET_tdst_ServerList *pst_Server;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Server = NETSRVL_GetByIndex(i);
	return(pst_Server ? pst_Server->w_NumberOfPlayer : NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NETSRVL_Request(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	int				i_Cmd;
	unsigned short	uw_Port;
	/*~~~~~~~~~~~~~~~~~~~~*/

	/* free previously scanned server */
	NETSRVL_Free();

	/* send broadcast message to port IPPORT_USERRESERVED to IPPORT_USERRESERVED + 0x3F */
	uw_Port = IPPORT_USERRESERVED;
	i_Cmd = NETCMD_RequestServer;

	for(uw_Port = IPPORT_USERRESERVED; uw_Port < IPPORT_USERRESERVED + 0x40; uw_Port++)
		NET_BroadcastPacketSend(uw_Port, &i_Cmd, 4);

	/* wait for response */
	return NETCLT_WaitAnswer(NETCMD_ServerAnswer);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NETSRVL_SetCurrent(int i)
{
	NET_pst_ServerCurrent = NET_pst_ServerList;
	while(i-- && NET_pst_ServerCurrent) NET_pst_ServerCurrent = NET_pst_ServerCurrent->pst_Next;
	return (NET_pst_ServerCurrent) ? 1 : 0;
}

/*$4
 ***********************************************************************************************************************
    Game data
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NET_GameDataReset(void)
{
	NET_st_Me.pc_GameDataCur = NET_st_Me.ac_GameData;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NET_GameDataCanPop(void)
{
	if(NET_pst_Player4GameDataPop == NULL) return 0;
	return(NET_pst_Player4GameDataPop->pc_GameDataCur - NET_pst_Player4GameDataPop->ac_GameData);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NET_GameDataPushi(int i, int i_Size)
{
	if((NET_st_Me.pc_GameDataCur - NET_st_Me.ac_GameData) > (NET_C_PlayerGameDataMaxSize - 4)) return 0;
	if(i_Size == 1)
		*NET_st_Me.pc_GameDataCur++ = (char) i;
	else if(i_Size == 2)
#if defined (__cplusplus)
		*((short *&) NET_st_Me.pc_GameDataCur)++ = (short) i;
#else
		*((short *) NET_st_Me.pc_GameDataCur)++ = (short) i;
#endif
	else if(i_Size == 4)
#if defined (__cplusplus)
		*((int *&) NET_st_Me.pc_GameDataCur)++ = i;
#else
		*((int *) NET_st_Me.pc_GameDataCur)++ = i;
#endif
	else
		return 0;
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NET_GameDataPushf(float f)
{
	if((NET_st_Me.pc_GameDataCur - NET_st_Me.ac_GameData) > (NET_C_PlayerGameDataMaxSize - 4)) return 0;
#if defined (__cplusplus)
	*((float *&) NET_st_Me.pc_GameDataCur)++ = f;
#else
	*((float *) NET_st_Me.pc_GameDataCur)++ = f;
#endif
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NET_GameDataPushstr(char *str)
{
	/*~~~~~~~*/
	int i_Size;
	/*~~~~~~~*/

	i_Size = 5 + strlen(str);

	if((NET_st_Me.pc_GameDataCur - NET_st_Me.ac_GameData) > (NET_C_PlayerGameDataMaxSize - i_Size)) return 0;

	L_strcpy(NET_st_Me.pc_GameDataCur + 4, str);
	NET_st_Me.pc_GameDataCur += i_Size - 4;
#if defined (__cplusplus)
	*((int *&) NET_st_Me.pc_GameDataCur)++ = i_Size;
#else
	*((int *) NET_st_Me.pc_GameDataCur)++ = i_Size;
#endif
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NET_GameDataPushv(MATH_tdst_Vector *v)
{
	if((NET_st_Me.pc_GameDataCur - NET_st_Me.ac_GameData) > (NET_C_PlayerGameDataMaxSize - sizeof(MATH_tdst_Vector)))
		return 0;

	MATH_CopyVector((MATH_tdst_Vector *) NET_st_Me.pc_GameDataCur, v);
	NET_st_Me.pc_GameDataCur += sizeof(MATH_tdst_Vector);
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int NET_GameDataPopi(int i_Size)
{
	if(!NET_pst_Player4GameDataPop) return 0;
	if(NET_pst_Player4GameDataPop->pc_GameDataCur - NET_pst_Player4GameDataPop->ac_GameData < i_Size) return 0;

	if(i_Size == 1)
	{
		NET_pst_Player4GameDataPop->pc_GameDataCur--;
		return *NET_pst_Player4GameDataPop->pc_GameDataCur;
	}
	else if(i_Size == 2)
	{
		NET_pst_Player4GameDataPop->pc_GameDataCur -= 2;
		return *(short *) NET_pst_Player4GameDataPop->pc_GameDataCur;
	}
	else if(i_Size == 4)
	{
		NET_pst_Player4GameDataPop->pc_GameDataCur -= 4;
		return *(int *) NET_pst_Player4GameDataPop->pc_GameDataCur;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float NET_GameDataPopf(void)
{
	if(!NET_pst_Player4GameDataPop) return 0;
	if(NET_pst_Player4GameDataPop->pc_GameDataCur - NET_pst_Player4GameDataPop->ac_GameData < 4) return 0;
	NET_pst_Player4GameDataPop->pc_GameDataCur -= 4;
	return *(float *) NET_pst_Player4GameDataPop->pc_GameDataCur;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *NET_GameDataPopstr(void)
{
	/*~~~~~~~*/
	int i_Size;
	/*~~~~~~~*/

	if(!NET_pst_Player4GameDataPop) return NULL;
	if(NET_pst_Player4GameDataPop->pc_GameDataCur - NET_pst_Player4GameDataPop->ac_GameData < 4) return NULL;
	i_Size = *(int *) (NET_pst_Player4GameDataPop->pc_GameDataCur - 4);
	if(NET_pst_Player4GameDataPop->pc_GameDataCur - NET_pst_Player4GameDataPop->ac_GameData < i_Size) return NULL;
	NET_pst_Player4GameDataPop->pc_GameDataCur -= i_Size;
	return NET_pst_Player4GameDataPop->pc_GameDataCur;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MATH_tdst_Vector *NET_GameDataPopv(MATH_tdst_Vector *v)
{
	MATH_InitVectorToZero(v);
	if(!NET_pst_Player4GameDataPop) return v;
	if(NET_pst_Player4GameDataPop->pc_GameDataCur - NET_pst_Player4GameDataPop->ac_GameData < sizeof(MATH_tdst_Vector))
		return v;
	NET_pst_Player4GameDataPop->pc_GameDataCur -= sizeof(MATH_tdst_Vector);
	MATH_CopyVector(v, (MATH_tdst_Vector *) NET_pst_Player4GameDataPop->pc_GameDataCur);
	return v;
}

/*$4
 ***********************************************************************************************************************
    debug functions
 ***********************************************************************************************************************
 */

#ifdef NET_LOG

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void NET_Error(char *_sz_Title)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int		err;
	char	*ErrString[] =
	{
		"",
		"",
		"",
		"",
		"WSAEINTR",
		"",
		"",
		"",
		"",
		"WSAEBADF",
		"",
		"",
		"",
		"WSAEACCES",
		"WSAEFAULT",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"WSAEINVAL",
		"",
		"WSAEMFILE",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"WSAEWOULDBLOCK",
		"WSAEINPROGRESS",
		"WSAEALREADY",
		"WSAENOTSOCK",
		"WSAEDESTADDRREQ",
		"WSAEMSGSIZE",
		"WSAEPROTOTYPE",
		"WSAENOPROTOOPT",
		"WSAEPROTONOSUPPORT",
		"WSAESOCKTNOSUPPORT",
		"WSAEOPNOTSUPP",
		"WSAEPFNOSUPPORT",
		"WSAEAFNOSUPPORT",
		"WSAEADDRINUSE",
		"WSAEADDRNOTAVAIL",
		"WSAENETDOWN",
		"WSAENETUNREACH",
		"WSAENETRESET",
		"WSAECONNABORTED",
		"WSAECONNRESET",
		"WSAENOBUFS",
		"WSAEISCONN",
		"WSAENOTCONN",
		"WSAESHUTDOWN",
		"WSAETOOMANYREFS",
		"WSAETIMEDOUT",
		"WSAECONNREFUSED",
		"WSAELOOP",
		"WSAENAMETOOLONG",
		"WSAEHOSTDOWN",
		"WSAEHOSTUNREACH",
		"WSAENOTEMPTY",
		"WSAEPROCLIM",
		"WSAEUSERS",
		"WSAEDQUOT",
		"WSAESTALE",
		"WSAEREMOTE",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"WSASYSNOTREADY",
		"WSAVERNOTSUPPORTED",
		"WSANOTINITIALISED",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"WSAEDISCON",
		"WSAENOMORE",
		"WSAECANCELLED",
		"WSAEINVALIDPROCTABLE",
		"WSAEINVALIDPROVIDER",
		"WSAEPROVIDERFAILEDINIT",
		"WSASYSCALLFAILURE",
		"WSASERVICE_NOT_FOUND",
		"WSATYPE_NOT_FOUND",
		"WSA_E_NO_MORE",
		"WSA_E_CANCELLED",
		"WSAEREFUSED"
	};
	/*~~~~~~~~~~~~~~~~~~*/

	err = WSAGetLastError();

	if(err == WSAEWOULDBLOCK) return;

	/*
	 * sprintf(NET_LogText, "%s : %s\r\n", _sz_Title, ErrString[err - WSABASEERR]); if
	 * (NET_LogCallback) NET_LogCallback(NET_LogText);
	 */
}

#endif /* NET_LOG */

#endif /* WIN32 */
