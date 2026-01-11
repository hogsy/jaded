//------------------------------------------------------------------------------
//   DATCP4ClientInfo.h
/// \author    NBeaufils
/// \date      10-Feb-05
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCP4CLIENTINFO_H__
#define __DATCP4CLIENTINFO_H__

#if 0

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "perforceui.h"
//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------
#define P4_INVALIDREVISION		0xFFFFFFFF
#define P4_INVALIDCHANGELIST	0xFFFFFFFF
#define P4_INVALIDTIME			0xFFFFFFFF
#define P4_INVALIDSTRING		'\0'

#define P4_ACTION_NEW		"new"
#define P4_ACTION_DELETE	"delete"
#define P4_ACTION_ADD		"add"
#define P4_ACTION_EDIT		"edit"
#define P4_ACTION_REMOVE	"removed"
#ifdef JADEFUSION
#define P4_ACTION_INTEGRATE	"integrate"	//modify
#define P4_ACTION_BRANCH	"branch"	//new
#endif

enum DAT_EFileInfo
{	
	eVALID_INFO			= 0x00000001,
	eVALID_KEY			= 0x00000002,
	eVALID_REVISION		= 0x00000004,
	eVALID_ACTION		= 0x00000008,
	eMSG_NOSUCHFILE		= 0x00000010
};


//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientInfo 
{
public:


	UINT		uiFileInfo;

	BIG_KEY		ulKey;								// Key of file 
	ULONG		ulCmdRevision;						// Revision associated to the P4 command executed
	char		aszAction[MAX_PATH];				// Action associated to the P4 command executed

	DAT_CP4ClientInfo() : 
	ulKey(BIG_C_InvalidIndex),
	ulCmdRevision(P4_INVALIDREVISION),
	uiFileInfo(0)
	{
		memset( aszAction,0,MAX_PATH); 
	}

	BOOL IsValid(){return (uiFileInfo & eVALID_INFO || uiFileInfo & eVALID_INFO) ;}
};

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientInfoFStat : public DAT_CP4ClientInfo
{
public:
	char	 aszOwnerName[BIG_C_MaxLenOwner];	// Owner of the file on P4
	ULONG	 ulClientRevision;					// Client Revision of the file on P4
	ULONG	 ulServerRevision;					// Server revision of the file on P4
	L_time_t ulTime;							// Server time of the file on P4
	ULONG	 ulChangeList;						// Server changelist to which file last belongs 
	char	 aszHeadAction[MAX_PATH];			// Server head action on file

	DAT_CP4ClientInfoFStat() :
	ulClientRevision(P4_INVALIDREVISION),
	ulServerRevision(P4_INVALIDREVISION),
	ulTime(P4_INVALIDTIME),
	ulChangeList(P4_INVALIDCHANGELIST)
	{
		aszOwnerName[0]	= P4_INVALIDSTRING;
		aszHeadAction[0] = P4_INVALIDSTRING;
	}
};

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientInfoHeader : public DAT_CP4ClientInfo
{
public:
	UINT bIsUniverseKey;										// Is file AI universe key
	UINT uiVersion;												// version of file
	char aszBFFilename[BIG_C_MaxLenPath + BIG_C_MaxLenName];	// Name of file

	DAT_CP4ClientInfoHeader() :
	bIsUniverseKey(FALSE),
	uiVersion(0)
	{
		memset(	aszBFFilename,0,BIG_C_MaxLenPath + BIG_C_MaxLenName);
	}
};

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class DAT_CP4ClientInfoDiff : public DAT_CP4ClientInfo
{
public:
	enum EFileDiff
	{	
		eFileModified	= 0x00000001,
		eFileRenamed	= 0x00000002,
		eFileMoved		= 0x00000004,
		eFileNew		= 0x00000008,

		eFileSame 		= 0x00000010
	};
	DWORD		dwFileDiff;												// Is file different	
	
	// Server
	ULONG		ulFileSizeServer;										// Size of file on server
	char		aszFilenameServer[BIG_C_MaxLenPath + BIG_C_MaxLenName];	// Name of file on server

	DAT_CP4ClientInfoDiff() :
	dwFileDiff(eFileSame),
	ulFileSizeServer(-1)
	{
		memset(	aszFilenameServer,0,BIG_C_MaxLenPath + BIG_C_MaxLenName);
	}
};


class DAT_CP4ClientInfoChanges : public PerforceFileInfo
{
public:
	BIG_KEY		ulKey;								// Key of file 
};

//------------------------------------------------------------------------------

#endif

#endif //#ifndef __DATCP4CLIENTINFO_H__

