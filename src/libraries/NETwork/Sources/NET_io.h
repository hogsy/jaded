/*$T NET_io.h GC!1.52 12/13/99 10:40:20 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifndef __NET_IO_H__
#define __NET_IO_H__

#ifdef __cplusplus
extern "C"
{
#endif
extern void NET_PacketSendToIP(char *, void *, int);
extern BOOL NET_PacketGet(void *, int);

#ifdef __cplusplus
}
#endif
#endif /* __NET_IO_H__ */
