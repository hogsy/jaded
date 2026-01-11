//////////////////////////////////////////////////////////////////////////
// Version Info
// ----------------
// Access to parameters describing version information
//////////////////////////////////////////////////////////////////////////

#include "Precomp.h"
#include "VersionInfo.h"

#include "BIGfiles\VERsion\VERsion_Number.h"
#include "ENGine\Sources\ENGvars.h"

#define FILENAME_BIGFILE_VERSION	"d:\\Version_Number_Data.txt"
#define FILENAME_ENGINE				"d:\\Version_Number_Data.txt"

//////////////////////////////////////////////////////////////////////////
// CXeVersionInfo

CXeVersionInfo g_XeVersionInfo;

CXeVersionInfo::CXeVersionInfo()
{
	strEngineVersion[0] = '\0';
	strBigFileVersion[0] = '\0';
}

CXeVersionInfo::~CXeVersionInfo()
{
}

void CXeVersionInfo::InitVersionInfo()
{
	Desktop::TWChar szBigFileVersion[60]	= L"n/a";
	Desktop::TWChar szBigFileTime[60]		= L"n/a";
	Desktop::TWChar szEngineBuild[60]		= L"n/a";
	Desktop::TWChar szEngineTarget[60]		= L"";
	
	// Get BF version (stored in Version_Number_Data.txt if present)
	if ( access(FILENAME_BIGFILE_VERSION, 4) == 0 )	
	{
		FILE* pFile = fopen(FILENAME_BIGFILE_VERSION, "r");
		
		if ( pFile )
		{
			// Read content of file
			fgetws(szBigFileVersion, 60, pFile);
	
			// Close file
			fclose(pFile);
		}
	}

	// Get BF time 
	if ( access(MAI_gst_InitStruct.asz_ProjectName, 4) == 0 )
	{
		HANDLE hFile = CreateFile(MAI_gst_InitStruct.asz_ProjectName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if  ( hFile != INVALID_HANDLE_VALUE )
		{
			// Read file time
			FILETIME BigFileTime;
			GetFileTime(hFile, &BigFileTime, NULL, NULL);
			
			FILETIME LocalFileTime;
			FileTimeToLocalFileTime( &BigFileTime, &LocalFileTime );

			SYSTEMTIME SystemFileTime;
			FileTimeToSystemTime(&LocalFileTime, &SystemFileTime);

			swprintf( szBigFileTime, L"%02u/%02u/%u %u:%u:%u", SystemFileTime.wDay, SystemFileTime.wMonth, SystemFileTime.wYear, SystemFileTime.wHour, SystemFileTime.wMinute, SystemFileTime.wSecond );

			// Close file
			CloseHandle(hFile);
		}
	}

	// Get engine build
#if defined(_DEBUG)
	wcscpy(szEngineBuild, L"DEBUG");
#elif defined(RELEASE) 
	wcscpy(szEngineBuild, L"RELEASE");
#elif defined(_FINAL_)
	wcscpy(szEngineBuild, L"FINAL");
#endif

	// Get engine target
#if _XENON_DEMO
	wcscpy(szEngineTarget, L"DEMO");
#endif
#define BIG_Cu4_MontrealAppVersion 52
#define BIG_Cu4_MontrealXeAppVersion 238
	swprintf(strEngineVersion, L"Engine Version: %d %s %s", BIG_Cu4_MontrealXeAppVersion, szEngineBuild, szEngineTarget);
	swprintf(strBigFileVersion, L"BigFile Version: %s (%s)", szBigFileVersion, szBigFileTime);
}