/*$T NET_io.c GC!1.52 12/13/99 10:52:13 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "NET.h"

#ifdef ACTIVE_EDITORS

/*
 ===================================================================================================
 ===================================================================================================
 */
void NET_PacketSend(struct sockaddr_in *_pst_Adr, void *_p_Data, int _i_Num)
{
    int err;

    err = sendto
    (
        NET_i_InSocket, 
        (const char far *) _p_Data,
        _i_Num,
        0,
#if defined (__cplusplus)
		(const sockaddr *) _pst_Adr,
#else
		(void *) _pst_Adr,
#endif
		sizeof(struct sockaddr_in)
    );

    if (err < 0)
    {
        NET_Error("NET_PackedSend");
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void NET_BroadcastPacketSend(unsigned short uw_Port, void *_p_Data, int _i_Num)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    struct sockaddr_in  st_Adr;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    st_Adr.sin_family = AF_INET;
    st_Adr.sin_port = htons( uw_Port );
    st_Adr.sin_addr.s_addr = INADDR_BROADCAST;
    NET_PacketSend(&st_Adr, _p_Data, _i_Num);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void NET_PacketSendToIP(char *_psz_Dest, void *_p_Data, int _i_Num)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    struct sockaddr_in  st_Adr;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    st_Adr.sin_family = AF_INET;
    st_Adr.sin_port = htons(NET_uw_JadePort);
    st_Adr.sin_addr.s_addr = inet_addr(_psz_Dest);
    NET_PacketSend(&st_Adr, _p_Data, _i_Num);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL NET_PacketGet(void *_p_Data, int _i_Num)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 c;
    struct sockaddr_in  fromaddress;
    int                 fromlen;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    fromlen = sizeof(fromaddress);
    c = recvfrom
        (
            NET_i_InSocket,
            (char far *) _p_Data,
            _i_Num,
            0,
            (struct sockaddr *) &fromaddress,
            &fromlen
        );

    if(c == SOCKET_ERROR)
    {
        NET_Error("NET_PackedGet");
        return FALSE;
    }

    return TRUE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int NET_i_PacketGet(void *_p_Data, int _i_Num, struct sockaddr_in *_pst_From )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int                 c;
    struct sockaddr_in  fromaddress;
    struct sockaddr *   p_From;
    int                 fromlen;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    p_From = (struct sockaddr *) (_pst_From ? _pst_From : &fromaddress);
    fromlen = sizeof(struct sockaddr_in);
    
    c = recvfrom( NET_i_InSocket, (char far *) _p_Data, _i_Num, 0, p_From, &fromlen );

    if (( c > 0) && ( *(int *) _p_Data == 1001) )
    {
        int a = 0;
    }
    

    
    if(c == SOCKET_ERROR)
    {
        NET_Error("NET_i_PackedGet");
        return -1;
    }

    return c;
}

#endif /* WIN32 */