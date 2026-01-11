/**********************************************************************
 *<
	FILE: DllEntry.cpp

	DESCRIPTION:Contains the Dll Entry stuff

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/
#include "Max.h"
#include "resource.h"
#if defined(MAX3) || defined(MAX8)
#include "iparamm2.h"
#include "istdplug.h"
#include "meshadj.h"
#endif


#include "MAD_RADMOD.h"
extern HINSTANCE hInstance;

#if defined(MAX3) || defined(MAX8)
extern ClassDesc2* GetMAD_RadModDesc();
#else
extern ClassDesc* GetMAD_RadModDesc();
#endif



HINSTANCE hInstance;

// ----------------------------------------------------------------------------
// SC: Externs used by the library we are linking with
//     Compilation hacks!
extern "C"
{

};
// ----------------------------------------------------------------------------

int controlsInit = FALSE;

// This function is called by Windows when the DLL is loaded.  This 
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
	hInstance = hinstDLL;				// Hang on to this DLL's instance handle.

	if (!controlsInit) {
		controlsInit = TRUE;
		InitCustomControls(hInstance);	// Initialize MAX's custom controls
		InitCommonControls();			// Initialize Win95 controls
	}
			
	return (TRUE);
}

__declspec( dllexport ) const TCHAR* LibDescription()
{
	return GetString(IDS_LIBDESCRIPTION);
}

//TODO: Must change this number when adding a new class
__declspec( dllexport ) int LibNumberClasses()
{
	return 1;
}

__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
	switch(i) {
		case 0: 
			return GetMAD_RadModDesc();
		default: 
			return 0;
	}
}

__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
}

