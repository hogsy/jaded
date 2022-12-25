/*$T MAIwind.c GC! 1.098 10/13/00 15:19:04 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef ACTIVE_EDITORS
#include "stdio.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/CLIbrary/CLIstr.h"


#include "SouND/sources/SNDstruct.h"
#include "SouND/sources/SNDconst.h"
#include "SouND/sources/SND.h"

#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/ENGmsg.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "INOut/INOjoystick.h"


extern BOOL LOA_gb_SpeedMode;
extern int  TEX_gi_ForceText;
extern int  GDI_gi_GDIType;

BOOL ENG_gb_SlashL = FALSE;
char ENG_gaz_SlashL[1024];
int	 ENG_gi_Map1=-1;
int	 ENG_gi_Map2=-1;
extern BOOL	AI_gb_Optim2;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static LRESULT CALLBACK sfnl_WindowProcMainOwner(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~~~*/
	LRESULT l_Result;
	/*~~~~~~~~~~~~~*/

	if(MAI_b_TreatMainWndMessages(hwnd, message, wParam, lParam, &l_Result)) return l_Result;
	if(MAI_b_TreatOwnerWndMessages(hwnd, &MAI_gst_MainHandles, message, wParam, lParam, &l_Result)) return l_Result;

	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static LRESULT CALLBACK sfnl_WindowProcDisplay(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~~~*/
	LRESULT l_Result;
	/*~~~~~~~~~~~~~*/

	if(MAI_b_TreatDisplayWndMessages(hwnd, &MAI_gst_MainHandles, message, wParam, lParam, &l_Result)) return l_Result;

	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*
 =======================================================================================================================
    Purpose: Create application window.
 =======================================================================================================================
 */
static void s_CreateWindow(HINSTANCE hInstance)
{
	/*~~~~~~~~~~~~~~~~*/
	WNDCLASS	x_Class;
	/*~~~~~~~~~~~~~~~~*/

	/* Create main window */
	x_Class.style = 0;
	x_Class.lpfnWndProc = sfnl_WindowProcMainOwner;
	x_Class.cbClsExtra = 0;
	x_Class.cbWndExtra = 0;
	x_Class.hInstance = hInstance;
	x_Class.hIcon = NULL;
	x_Class.hCursor = NULL;
	x_Class.hbrBackground = NULL;
	x_Class.lpszMenuName = NULL;
	x_Class.lpszClassName = "SDKMain";
	RegisterClass(&x_Class);

	MAI_gh_MainWindow = MAI_gst_MainHandles.h_OwnerWindow = CreateWindow
		(
			"SDKMain",
			"JADE (c) Ubi Pictures",
			WS_OVERLAPPEDWINDOW,
			0,
			0,
			640,
			480,
			NULL,
			NULL,
			hInstance,
			NULL
		);

	/* Create display wnd inside owner one */
	x_Class.lpfnWndProc = sfnl_WindowProcDisplay;
	x_Class.lpszClassName = "SDKDisplay";
	RegisterClass(&x_Class);

	MAI_gst_MainHandles.h_DisplayWindow = CreateWindow
		(
			"SDKDisplay",
			"Jade",
			WS_VISIBLE | WS_CHILD,
			0,
			0,
			640,
			480,
			MAI_gh_MainWindow,
			NULL,
			hInstance,
			NULL
		);

	ShowWindow(MAI_gh_MainWindow, SW_SHOW);
}

/*
 =======================================================================================================================
    Purpose: Analyse command line.
 =======================================================================================================================
 */
static void s_AnalyseCommandLine(char *_psz_Line)
{
	while(1)
	{
		while(*_psz_Line == ' ') *_psz_Line++;
		if(*_psz_Line != '/') break;

		_psz_Line++;
		if(!L_strnicmp(_psz_Line, "optim2", 6))
		{
			AI_gb_Optim2 = TRUE;
			_psz_Line += 6;
		}
		else if(!L_strnicmp(_psz_Line, "nosound", 7))
		{
			SND_gc_NoSound = 1;
			_psz_Line += 7;
		}
		else if(!L_strnicmp(_psz_Line, "joy", 3))
		{
			/*~~~~~~~~~~~~~*/
			FILE	*hp_File;
			/*~~~~~~~~~~~~~*/

			hp_File = fopen("joy.ini", "rt");
			if(hp_File)
			{
				fscanf(hp_File, "%d\n", &win32INO_l_Joystick_YDownStart);
				fscanf(hp_File, "%d\n", &win32INO_l_Joystick_XRightStart);
				fscanf(hp_File, "%d\n", &win32INO_l_Joystick_YUpStart);
				fscanf(hp_File, "%d\n", &win32INO_l_Joystick_XLeftStart);
				fscanf(hp_File, "%d\n", &win32INO_l_Joystick_YDown);
				fscanf(hp_File, "%d\n", &win32INO_l_Joystick_XRight);
				fscanf(hp_File, "%d\n", &win32INO_l_Joystick_YUp);
				fscanf(hp_File, "%d\n", &win32INO_l_Joystick_XLeft);
			}

			_psz_Line += 3;
		}
		else if(!L_strnicmp(_psz_Line, "ps2joy", 6))
		{
			win32INO_l_Joystick_Mode = INO_Joy_Ps2Mode;
			_psz_Line += 6;
		}
		else if(!L_strnicmp(_psz_Line, "pcjoy", 5))
		{
			win32INO_l_Joystick_Mode = INO_Joy_PCMode;
			_psz_Line += 5;
		}
		else if(!L_strnicmp(_psz_Line, "B", 1))
		{
			LOA_gb_SpeedMode = TRUE;
			_psz_Line += 1;
		}
        else if(!L_strnicmp(_psz_Line, "T4", 2))
        {
            TEX_gi_ForceText |= TEX_Manager_Accept4bpp;
            _psz_Line += 2;
        }
        else if(!L_strnicmp(_psz_Line, "T8", 2))
        {
            TEX_gi_ForceText |= TEX_Manager_Accept8bpp;
            _psz_Line += 2;
        }
        else if(!L_strnicmp(_psz_Line, "TA", 2))
        {
            TEX_gi_ForceText |= TEX_Manager_AcceptAlphaPalette;
            _psz_Line += 2;
        }
        else if(!L_strnicmp(_psz_Line, "TM", 2))
        {
            TEX_gi_ForceText |= TEX_Manager_OneTexForRawPal;
            _psz_Line += 2;
        }
        else if(!L_strnicmp(_psz_Line, "TX", 2))
        {
            TEX_gi_ForceText |= TEX_Manager_AcceptAllPalette | TEX_Manager_OneTexForRawPal;
            _psz_Line += 2;
        }
        else if(!L_strnicmp(_psz_Line, "TF", 2))
        {
            TEX_gi_ForceText |= TEX_Manager_FixVRam;
            _psz_Line += 2;
        }
        else if(!L_strnicmp(_psz_Line, "T32", 3))
        {	// accept ONLY 32 bpp testures 
            TEX_gi_ForceText |= TEX_Manager_Accept32bpp;
            TEX_gi_ForceText &= ~TEX_Manager_Accept4bpp;
            TEX_gi_ForceText &= ~TEX_Manager_Accept8bpp;
            TEX_gi_ForceText &= ~TEX_Manager_Accept16bpp_4444;
            TEX_gi_ForceText &= ~TEX_Manager_Accept16bpp_1555;
            TEX_gi_ForceText &= ~TEX_Manager_Accept24bpp;
			_psz_Line += 3;
        }
        else if(!L_strnicmp(_psz_Line, "DX8", 3))
        {
            GDI_gi_GDIType = 1;
            _psz_Line += 3;
        }
		else if(!L_strnicmp(_psz_Line, "L", 1))
		{
				/* /L<wol file> : load one map */
				ENG_gb_SlashL = TRUE;
				L_strcpy(ENG_gaz_SlashL, _psz_Line + 1);
				if(L_strchr(ENG_gaz_SlashL, ' ')) *L_strchr(ENG_gaz_SlashL, ' ') = 0;
				{
					FILE *f;
					f = fopen("binerr.log", "at");
					if(!f) f = fopen("binerr.log", "wt");
					fprintf(f, "\n\n*********** MAP %s ************\n\n", ENG_gaz_SlashL);
					fclose(f);
				}
				
				while(*_psz_Line != ' ') *_psz_Line++;
				break;
		}
		else if(!L_strnicmp(_psz_Line, "map1", 4))
		{
			sscanf(_psz_Line+4, "%x", &ENG_gi_Map1);
			
			while(*_psz_Line != ' ') *_psz_Line++;
			
		}
		else if(!L_strnicmp(_psz_Line, "map2", 4))
		{
			sscanf(_psz_Line+4, "%x", &ENG_gi_Map2);
			
			while(*_psz_Line != ' ') *_psz_Line++;
			
		}
		else
		{
			while(*_psz_Line && (*_psz_Line != ' ')) _psz_Line++;
		}
	}

	/* Get size of blocks */
	sscanf(_psz_Line, "%s", MAI_gst_InitStruct.asz_ProjectName);
}

/*
 =======================================================================================================================
    Purpose: Windows main entry function.
 =======================================================================================================================
 */
extern  BOOL LOA_gb_SpeedMode;
extern int AI_EvalFunc_WORLoadList_C(int);
extern void LOG_Use(char *);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	/* To detect FPU exeptions as soon as possible */
	//_controlfp(~(_EM_ZERODIVIDE|_EM_INVALID),_MCW_EM );

	__try {
	/* Common SDK inits */
	ENG_InitApplication();

	/* Analyse the command line to get bigfile name and eventually other things... */
	s_AnalyseCommandLine((char *) lpCmdLine);

	/* Test argument */
	if(!(*MAI_gst_InitStruct.asz_ProjectName))
	{
		ERR_X_ForceError("No BigFile Specified as First .exe Argument", NULL);
		exit(-1);
	}
//	BIG_TestCryptage("X:\Cryptage.txt");

	/* Open bigfile */
	BIG_Open(MAI_gst_InitStruct.asz_ProjectName);

	/* Log */
	LOG_Use(MAI_gst_InitStruct.asz_ProjectName);

	/* Create main window */
	MAI_gh_MainInstance = hInstance;
	s_CreateWindow(hInstance);

	/* Call the engine main loop */
	ENG_InitEngine();
	
	if(ENG_gb_SlashL && LOA_gb_SpeedMode)
	{
		AI_EvalFunc_WORLoadList_C(ENG_gi_Map1);
		AI_EvalFunc_WORLoadList_C(ENG_gi_Map2);
	}
	else
	{
		ENG_Loop();
	}

	WOR_Universe_Close(0);
	ENG_CloseEngine();

	/* Destroy main window */
	DestroyWindow(MAI_gh_MainWindow);

	/* Close bigfile */
	BIG_Close();

	/* Close SDK */
	ENG_CloseApplication();

	}

	__except((LOA_gb_SpeedMode&&ENG_gb_SlashL?1:0)) 
	{
		FILE *f;
		f = fopen("binerr.log", "at");
		if(!f) f = fopen("binerr.log", "wt");
		fprintf(f, "\n\n## LOAD CRASH FOR MAP %s [%x]\n\n", ENG_gaz_SlashL, ENG_gi_Map2);
		fclose(f);
		ExitProcess(-1);
	}

	return 0;
}

#endif /* !ACTIVE_EDITORS */
