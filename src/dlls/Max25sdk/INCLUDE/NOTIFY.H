/**********************************************************************
 *<
	FILE: notify.h

	DESCRIPTION: Include file for event notification support

	CREATED BY: Tom Hudson

	HISTORY: Created 8 April 1995

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#ifndef _NOTIFY_H_

#define _NOTIFY_H_

// Pre-defined Jaguar system notification codes

#define NOTIFY_UNITS_CHANGE			0x00000001
#define NOTIFY_TIMEUNITS_CHANGE	 	0x00000002
#define NOTIFY_VIEWPORT_CHANGE	 	0x00000003
#define NOTIFY_SPACEMODE_CHANGE	 	0x00000004
#define NOTIFY_SYSTEM_PRE_RESET	 	0x00000005	// Sent before system is reset
#define NOTIFY_SYSTEM_POST_RESET 	0x00000006	// Sent after system is reset
#define NOTIFY_SYSTEM_PRE_NEW		0x00000007	// Sent before system is NEW'd-out
#define NOTIFY_SYSTEM_POST_NEW		0x00000008	// Sent after system is NEW'd-out
#define NOTIFY_FILE_PRE_OPEN		0x00000009	// Sent before a new file is opened
#define NOTIFY_FILE_POST_OPEN		0x0000000A	// Sent after a new file is opened successfully
#define NOTIFY_FILE_PRE_MERGE		0x0000000B	// Sent before a file is merged
#define NOTIFY_FILE_POST_MERGE		0x0000000C	// Sent after a file is merged successfully
#define NOTIFY_FILE_PRE_SAVE		0x0000000D	// Sent before a file is saved
#define NOTIFY_FILE_POST_SAVE		0x0000000E	// Sent after a file is saved

// Notification information structure -- Passed to NOTIFYPROC to inform it what
// it's being notified about...
typedef struct {
	int intcode;
	} NotifyInfo;

// The notification callback function
typedef void (* NOTIFYPROC)(void *param, NotifyInfo *info);

// Integer versions -- For pre-defined MAX codes
int CoreExport RegisterNotification(NOTIFYPROC proc, void *param, int code);
int CoreExport UnRegisterNotification(NOTIFYPROC proc, void *param, int code);
void CoreExport BroadcastNotification(int code);

// Unregister a callback from all codes
int CoreExport UnRegisterNotification(NOTIFYPROC proc, void *param);

#endif // _NOTIFY_H_
