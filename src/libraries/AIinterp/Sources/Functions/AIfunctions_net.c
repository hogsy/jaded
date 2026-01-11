/*$T AIfunctions_net.c GC!1.65 12/30/99 12:26:10 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"
#include "NETwork/sources/Net.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

//void    NET_PacketSendToIP(char *_psz_Dest, void *_p_Data, int _i_Num);
//void    NET_PacketGet(void *_p_Data, int _i_Num);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_NETSendFloat(AI_tdst_Node *_pst_Node)
{
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_NETGetFloat(AI_tdst_Node *_pst_Node)
{
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_NetCommand_C( int i_Cmd, int i_Param )
{
    switch ( i_Cmd )
    {
    case 2:
        NETSRV_Destroy();
        break;
    case 3:
        return NETSRVL_Request();
        break;
    case 4:
        //return NET_WaitServerResponse( NETCMD_ServerAnswer );
        break;
    case 6:
        return NETSRVL_SetCurrent( i_Param );
    case 7:
        return NETCLT_JoinCurrentServer();
    case 8:
        NETCLT_Leave();
        break;
    case 9:
        return NETSRV_PlayerNumberGet();
    case 10:
        return (NET_b_Client && NET_b_Init && !NET_b_Server) ? 1 : 0;
    case 11:
        break;
        //return NET_PlayerNumberMaxSet( i_Param );
    case 12:
        NET_GameDataReset();
        break;
    case 13:
        return NETSRV_CanPop( i_Param );
    case 14:
        return NETCLT_CanPopData();
    case 15:
        return NETCLT_SendGameData();
    case 16:
        return NETSRV_SendGameData();
    case 17:
        return NETSRVL_GetNbPlayers( i_Param );
    case 18:
        return NETSRVL_GetMaxPlayers( i_Param );
    }
    return 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_NETCommand(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~*/
    int     i_Cmd;
    int     i_Param;
    /*~~~~~~~~~~~~~~~*/

    i_Param = AI_PopInt();
    i_Cmd = AI_PopInt();
    AI_PushInt( AI_EvalFunc_NetCommand_C( i_Cmd, i_Param ) );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_NetCommandStr_C( int i_Cmd, char *sz_String )
{
    switch ( i_Cmd )
    {
    case 1:
        NETSRV_Create( sz_String, 4 );
        break;
    case 5:
        NETCLT_SetName( sz_String );
        break;
    }
    return 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_NETCommandStr(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~*/
    int     i_Cmd;
    char    *sz_String;
    /*~~~~~~~~~~~~~~~*/

    sz_String = AI_PopStringPtr();
    i_Cmd = AI_PopInt();
    AI_PushInt( AI_EvalFunc_NetCommandStr_C( i_Cmd, sz_String ) );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_NETServerCreate(AI_tdst_Node *_pst_Node)
{
    /*~~~~~~~~~~~~~~~*/
    int i_NbPlayers;
    /*~~~~~~~~~~~~~~~*/

    i_NbPlayers = AI_PopInt();
    NETSRV_Create( AI_PopStringPtr(), i_NbPlayers);
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_NETServerGetName(AI_tdst_Node *_pst_Node)
{
    AI_PushString(NETSRVL_GetName( AI_PopInt() ) );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_NETPlayerGetName(AI_tdst_Node *_pst_Node)
{
    AI_PushString(NETSRV_PlayerGetName( AI_PopInt() ) );
    return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunv_NETPushi(AI_tdst_Node *_pst_Node)
{
    int i_Size;

    i_Size = AI_PopInt();
    AI_PushInt( NET_GameDataPushi( AI_PopInt(), i_Size ) );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunv_NETPushf(AI_tdst_Node *_pst_Node) 
{
    AI_PushInt( NET_GameDataPushf( AI_PopFloat() ) );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunv_NETPushv(AI_tdst_Node *_pst_Node)
{
    AI_PushInt( NET_GameDataPushv( AI_PopVectorPtr() ) );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunv_NETPushstr(AI_tdst_Node *_pst_Node)
{
    AI_PushInt( NET_GameDataPushstr( AI_PopStringPtr() ) );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunv_NETPopi(AI_tdst_Node *_pst_Node)
{
    AI_PushInt( NET_GameDataPopi( AI_PopInt() ) );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunv_NETPopf(AI_tdst_Node *_pst_Node)
{
    AI_PushFloat( NET_GameDataPopf() );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunv_NETPopv(AI_tdst_Node *_pst_Node)
{
    MATH_tdst_Vector v;
    AI_PushVector( NET_GameDataPopv( &v ) );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunv_NETPopstr(AI_tdst_Node *_pst_Node)
{
    AI_PushString( NET_GameDataPopstr() );
    return ++_pst_Node;
}









#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

