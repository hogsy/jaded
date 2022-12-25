#ifndef _HEADER_DEMODISK
#define	_HEADER_DEMODISK

#include "Base\BAStypes.h"

BOOL DEM_InitDemoDisk(void);
char *DEM_GetLaunchPath(void);
void DEM_StartTimer(void);
void DEM_ResetTimer(void);
BOOL DEM_ManageDemoDisk(void);
HRESULT DEM_PlayVideo( const CHAR* strFilename );
void DEM_AttractMode(int OnOff);
void DEM_StopAttractMode(void);
DWORD DEM_GetTimeOut(void);
DWORD DEM_GetAttrackMode(void);
DWORD DEM_IsDemoDisk(void);
void DEM_DontIncrementTimer(int newstate);

BOOL DEM_ReturnToDash(void);

//New for TRC..go to dashboard
void DEM_GoToDashBoard(void);

bool DEM_IsJoyValid(void);
bool DEM_IsStartPressed(void);

void DEM_ManageNO_PAD(void);
void DEM_NoPadConnected(int);



#endif