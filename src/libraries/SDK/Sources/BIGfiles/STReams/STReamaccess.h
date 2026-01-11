#pragma once

#ifdef ACTIVE_EDITORS

// --- NOTE : Alternate Data Stream ---
// You can read about it here: http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnfiles/html/ntfs5.asp
// (A Programmer's Perspective on NTFS 2000 Part 1: Stream and Hard Link)
// another interesting FAQ: http://www.heysoft.de/Frames/f_faq_ads_en.htm

#define STREAM_FILENAME		"FileName"
#define STREAM_BFVERSION	"BFVersion.stream"
#define STREAM_BFPLATFORM	"BFPlatform.stream"
#define STREAM_BFGUID		"BFGUID.stream"
#define STREAM_P4PORT		"P4Port.stream"
#define STREAM_P4USER		"P4User.stream"
#define STREAM_P4HOST		"P4Host.stream"
#define STREAM_P4PASSWORD	"P4Password.stream"
#define STREAM_P4DEPOT		"P4Depot.stream"

#include <string>

int GetStreamByName(const char* szFileName, const char* szStreamName,char* szStreamValue);
int SetStreamByName(const char* szFileName, const char* szStreamName,const char* szStreamValue);

#endif // ACTIVE_EDITORS
