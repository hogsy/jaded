/* ============================================================= */
/* ============= Main entry point for Xenon Project ============= */
/* ============================================================= */

#include "Precomp.h"

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "stdio.h"
#include <float.h>
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEMpro.h"
#include "BASe\BENch\BENch.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/ENGmsg.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "SouND/sources/SNDstruct.h"
#include "SouND/sources/SNDconst.h"
#include "INOut/INOjoystick.h"
#include "GDInterface/GDIrasters.h"
#include "Sdk\Sources\TIMer\PROfiler\PROdefs.h"
#include "GEOmetric/GEOstatistics.h"

#include "BIGfiles/LOAding/LOAread.h"
#include "BASe/MEMory/MEM.h"

#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeProfiling.h"

#include "Xenon/VersionInfo/VersionInfo.h"
#include "Xenon/MenuManager/MenuManager.h"

#include "SouND/Sources/Xenon/xeSND_Engine.h"

#include "BinarizedBFManager.h"

#if !defined(_FINAL_) 
//#define FLOATING_POINT_ENABLE
#endif

BOOL ENG_gb_SlashL = FALSE;
ULONG ENG_ulCustomLevel;

#if PLAN_B_MEMORY_LEAK_FIX
bool g_bJustRebooted = false;
#endif

//--------------------------------------------------------------------------------------
// Title ID
//--------------------------------------------------------------------------------------
#include <xtitleidbegin.h>
	XEX_TITLE_ID(0x555307D3)
#include <xtitleidend.h>

// ***********************************************************************************************************************
//    Externs
// ***********************************************************************************************************************

extern void LOA_SetIsSwapperActive(BOOL _bSwapperActive);
extern /*"C"*/ BOOL					LOA_gb_SpeedMode; 
#ifdef JOYRECORDER
extern int                          INO_b_InputWillBePlayed;
extern int                          INO_b_InputWillBeRecorded;
extern int                          INO_n_NumRepeat;
#endif

extern BOOL g_bSoundDatabaseEnabled;

// ***********************************************************************************************************************
//    Memory Functions (Currently Unused)
// ***********************************************************************************************************************

/*
LPVOID WINAPI XMemAlloc(SIZE_T dwSize,DWORD dwAllocAttributes)
{
#pragma unused(dwAllocAttributes)
return (LPVOID)MEM_p_AllocAlign(dwSize, 32);
}

VOID WINAPI XMemFree(PVOID pAddress,DWORD dwAllocAttributes)
{
#pragma unused(dwAllocAttributes)
MEM_FreeAlign(pAddress);
}

SIZE_T WINAPI XMemSize(PVOID pAddress,DWORD dwAllocAttributes)
{
#pragma unused(dwAllocAttributes)
return 0;
}
*/

// ***********************************************************************************************************************
//    Generic Helpers
// ***********************************************************************************************************************

void FlipEndian_32(void* Param_pBuffer, int Param_iLength_Byte)
{
	// Error Checking

	if ((Param_pBuffer == NULL) || (Param_iLength_Byte <= 0))
	{
		return;
	}

	// Quick & Dirty Endian-Flip

	int*	BufferPointer	= (int*)Param_pBuffer;
	int		iLength_Int		= (Param_iLength_Byte + 3) >> 2;

	// Clean-up Last Int (To Avoid Garbage When Original Size Not Modulo 4)

	switch (Param_iLength_Byte & 3)
	{
		case 0: break;
		case 1: BufferPointer[iLength_Int - 1] &= 0xFF000000; break;
		case 2: BufferPointer[iLength_Int - 1] &= 0xFFFF0000; break;
		case 3: BufferPointer[iLength_Int - 1] &= 0xFFFFFF00; break;
	}

	// Swap Bytes

	while (iLength_Int > 0)
	{
		SwapDWord(BufferPointer);
		BufferPointer++;
		iLength_Int--;
	}
}

// ***********************************************************************************************************************

void FlipEndian_16(void* Param_pBuffer, int Param_iLength_Byte)
{
	// Error Checking

	if ((Param_pBuffer == NULL) || (Param_iLength_Byte <= 0))
	{
		return;
	}

	// Quick & Dirty Endian-Flip

	short*	BufferPointer	= (short*)Param_pBuffer;
	int		iLength_Short	= (Param_iLength_Byte + 1) >> 1;

	// Clean-up Last Short (To Avoid Garbage When Original Size Not Modulo 2)

	switch (Param_iLength_Byte & 1)
	{
		case 0: break;
		case 1: BufferPointer[iLength_Short - 1] &= 0xFF00; break;
	}

	// Swap Bytes

	while (iLength_Short > 0)
	{
		SwapWord(BufferPointer);
		BufferPointer++;
		iLength_Short--;
	}
}


// ***********************************************************************************************************************
//    Display Creation / Destruction
// ***********************************************************************************************************************

static void s_CreateDisplay(MAI_tdst_WinHandles *ph)
{
	// Create Display

	ph->h_DisplayWindow = NULL;
	ph->pst_DisplayData = GDI_fnpst_CreateDisplayData();

	GDI_gpst_CurDD = ph->pst_DisplayData;

	ph->pst_DisplayData->st_ScreenFormat.ul_Flags |= GDI_Cul_SFF_OccupyAll;

	GDI_fnl_InitInterface(&ph->pst_DisplayData->st_GDI, 1);

	ph->pst_DisplayData->pv_SpecificData = ph->pst_DisplayData->st_GDI.pfnpv_InitDisplay();

	GDI_AttachDisplay(ph->pst_DisplayData , ph->h_DisplayWindow);

#ifdef RASTERS_ON
	GDI_Rasters_Init(ph->pst_DisplayData->pst_Raster, "Display Data");
#endif
}

// ***********************************************************************************************************************

static void s_DestroyDisplay(MAI_tdst_WinHandles *ph)
{
	if(ph && ph->pst_DisplayData)
	{
		GDI_DetachDisplay(ph->pst_DisplayData);
		ph->pst_DisplayData->st_GDI.pfnv_DesinitDisplay(ph->pst_DisplayData->pv_SpecificData);
		GDI_fnv_DestroyDisplayData(ph->pst_DisplayData);
	}
}

void ReadIniFile(char* fileName)
{
	// Check for .ini file to get input parameters
	FILE* fp;
	fp = fopen(fileName, "r");
	UINT pos;
	if (fp)
	{
		char line[MAX_PATH];
		char block[MAX_PATH];
		char* pParam;
		char* pVal;

		while (!feof(fp))
		{		
			if (fgets(line, MAX_PATH, fp) == 0)
				break;
			strtok(line, "\n");	// remove trailing \n

			// check for block delimiters
			if (line[0]=='[')
			{
				strcpy(block, line);
				continue;
			}

			// Extract parameter and value from line
			for (pos = 0; pos < strlen(line); pos++)
				if (line[pos] == '=')
					break;
			if (pos < strlen(line))
			{
				pParam = line;
				line[pos++] = NULL;
				pVal   = &line[pos];
			}
			else
				continue;

			// handle parameters for 'settings' block
			if (!stricmp(block, "[settings]"))
			{
				if (!stricmp(pParam, "Binarized") && !stricmp(pVal, "1"))
					LOA_gb_SpeedMode = TRUE;

				else if (!stricmp(pParam, "BigFile"))
					strcpy(MAI_gst_InitStruct.asz_ProjectName, pVal);

				else if (!stricmp(pParam, "HideFPS") && !stricmp(pVal, "1"))
					g_MenuManager.SetDemoMode_HiddenFPS(TRUE);

				else if (!stricmp(pParam, "HideVersion") && !stricmp(pVal, "1"))
					g_MenuManager.SetDemoMode_HiddenVersion(TRUE);

                else if (!stricmp(pParam, "TripleBuffer") && !stricmp(pVal, "1"))
                    g_oXeRenderer.UseTripleBuffer(TRUE);

                else if (!stricmp(pParam, "ParallelGDK"))
                {
                    if (!stricmp(pVal, "0"))
                        g_oXeRenderer.UseParallelGDK(FALSE);
                    else
                        g_oXeRenderer.UseParallelGDK(TRUE);
                }
            }
			if (!stricmp(block, "[BinarizedBF]"))
			{
				if (!stricmp(pParam, "BigFileTextures"))
					g_BinarizedBFManager.InsertBinarizedBF(pVal, eBinarizedBFTextures);
				if (!stricmp(pParam, "BigFileMaps"))
					g_BinarizedBFManager.InsertBinarizedBF(pVal, eBinarizedBFMaps);
			}
		}
		fclose(fp);
	}
}

void ReadVersionInformation()
{
#if defined(DESKTOP_ENABLE)
	g_XeVersionInfo.InitVersionInfo();
#endif // DESKTOP_ENABLE
}

void ReadCommandLine()
{
    LPSTR pszCommandLine;
    char pszLevel[64];
    char* pz;

#if PLAN_B_MEMORY_LEAK_FIX
	std::string str = GetCommandLine();

	if( g_bJustRebooted )
		str += " /load 3d00c45a";

    pszCommandLine = (char*)str.c_str( );
#else
    pszCommandLine = GetCommandLine();
#endif
    pz = (char *) pszCommandLine;

    while(*pz)
    {
        while(*pz == ' ') *pz++;
        if(*pz == '/')
        {
            *pz++;
            if(!L_strnicmp(pz, "play", 4))
            {
#ifdef JOYRECORDER
			    char pszNumRepeat[64];

                INO_b_InputWillBePlayed = 1;
                pz += 5;

                if (*pz != '/')
                {
                    L_strcpy(pszNumRepeat, pz);  
                    INO_n_NumRepeat = atoi(pszNumRepeat);
                    if (INO_n_NumRepeat > 1000 || INO_n_NumRepeat < 0)
                    {
                        INO_n_NumRepeat = 0;
                    }
                }      
                else
                {
                    *pz--;
                }
#endif
            }
            if(!L_strnicmp(pz, "record", 6))
            {
#ifdef JOYRECORDER
                INO_b_InputWillBeRecorded = 1;
                pz += 7;
#endif
            }
#ifdef XENONVIDEOSTATISTICS
            else if(!L_strnicmp(pz, "dump", 4))
            {
                XeGOStatistics *XeStats = XeGOStatistics::Instance();

                XeStats->TriggerDumpStat();                
                pz += 4;
            }
#endif
            else if(!L_strnicmp(pz, "load", 4))
            {
                pz += 5;

                if (*pz != '/')
                {
                    ENG_gb_SlashL = TRUE;

                    L_strcpy(pszLevel, pz);  
                    if(L_strchr(pszLevel, ' ')) 
                    {
                        *L_strchr(pszLevel, ' ') = 0;
                    }
                    sscanf(pszLevel,"%x",&ENG_ulCustomLevel);
                }      
                else
                {
                    *pz--;
                }
            }
			else if(!L_strnicmp(pz, "profile", 7))
			{
#if defined (PROFILE_ENABLE)
				pz += 8;
				g_oXeProfileManager.SetContinuousCapture();
#endif
			}
			else if(!L_strnicmp(pz, "sounddb", 7))
			{
#if _XENON_SOUND_ENGINE
				pz += 8;
				g_bSoundDatabaseEnabled = TRUE;
#endif
			}
            else if(!L_strnicmp(pz, "fp_except", 9))
            {
                pz += 10;
#define _EXC_MASK    \
    _EM_UNDERFLOW  + \
    _EM_OVERFLOW   + \
    _EM_ZERODIVIDE + \
    _EM_INEXACT
                _controlfp(_EXC_MASK, _MCW_EM); 
            }
        }

        *pz++;
    }
}

// ***********************************************************************************************************************
//    Main Entry Point
// ***********************************************************************************************************************

VOID __cdecl main()
{
#if defined(FLOATING_POINT_ENABLE)
    _clearfp();
    int i = _controlfp(0, 0);
    i    &= ~(EM_ZERODIVIDE | EM_OVERFLOW | EM_INVALID);
    _controlfp(i, MCW_EM);
#endif

	LOA_SetIsSwapperActive(true);

#if PLAN_B_MEMORY_LEAK_FIX
	DWORD dwSize = 0;
	bool bSuccess = XGetLaunchDataSize( &dwSize ) == ERROR_SUCCESS;

	if( bSuccess && dwSize > 0 )
	{
		int nCode;
		XGetLaunchData( &nCode, sizeof( nCode ) );

		if( nCode == 0x00ABCDEF )
			g_bJustRebooted = true;
	}
#endif

	MEMpro_Init();
	MEMpro_StartMemRaster();
	MEMpro_StopMemRaster(MEMpro_Id_GSP);

	// Common SDK inits

	ENG_InitApplication();

	// Default big file name
	strcpy(MAI_gst_InitStruct.asz_ProjectName, "d:\\rayman4.bf");

	// Read configuration parameters from ini file

	ReadIniFile("d:\\Jade_xe.ini");

	// Read version information

	ReadVersionInformation();

    // Treat Command Line
    ReadCommandLine();

	// Open BF binarized bigfiles
	if ( LOA_gb_SpeedMode )
		g_BinarizedBFManager.LoadBinarizedBF();
	else
		BIG_Open(MAI_gst_InitStruct.asz_ProjectName);

	// Initialize Engine

	s_CreateDisplay(&MAI_gst_MainHandles);

	ENG_InitEngine();

	// Call the engine main loop

	ENG_Loop();

	// Destroy Engine

#if defined(_XENON_RENDERER_USETHREAD)
	g_oXeRenderer.WaitForEventComplete(eXeThreadEvent_Flip);
#endif

	WOR_Universe_Close(0);
	ENG_CloseEngine();

	s_DestroyDisplay(&MAI_gst_MainHandles);

	// Close bigfile

	BIG_Close();

	// Close SDK

	ENG_CloseApplication();
}

// ***********************************************************************************************************************
