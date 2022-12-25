//////////////////////////////////////////////////////////////////////////
// Version Info
// ----------------
// Access to parameters describing version information
//////////////////////////////////////////////////////////////////////////

#ifndef __VERSIONINFO_H__
#define __VERSIONINFO_H__

#include "Xenon\Desktop\Desktop_ConstantTypes.h"

//////////////////////////////////////////////////////////////////////////
// CXeVersionInfo

class CXeVersionInfo
{
protected:
	Desktop::TWChar strEngineVersion[1024];
	Desktop::TWChar strBigFileVersion[1024];

public:
	CXeVersionInfo();
	~CXeVersionInfo();

	void InitVersionInfo();		
	
	inline Desktop::TWChar* GetEngineVersionInfo() { return strEngineVersion ; } ;
	inline Desktop::TWChar* GetBigFileVersionInfo() { return strBigFileVersion ; } ;
};

extern __declspec(align(32)) CXeVersionInfo g_XeVersionInfo;

#endif // __VERSIONINFO_H__
