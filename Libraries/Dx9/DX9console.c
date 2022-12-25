// Dx9console.c


#include "Dx9console.h"
#include <stdio.h>
#include <stdlib.h>

/************************************************************************************************************************
    Public Function
 ************************************************************************************************************************/

#pragma warning(disable: 4324)
static struct
{
	__declspec(align(4))
		DLGTEMPLATE mTemplate;
	WORD mData0[3];
	__declspec(align(4))
		DLGITEMTEMPLATE mItem1;
	WORD mData1[4];
	__declspec(align(4))
		DLGITEMTEMPLATE mItem2;
	WORD mData2[6];
} gDlgTemplate = 
{
	{
		DS_SETFOREGROUND | DS_MODALFRAME | WS_POPUP | WS_VISIBLE,
		WS_EX_DLGMODALFRAME, 2, 0, 0, 200, 13
	},
	{ 0, 0, 0 },
	{ WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 0, 0, 0, 186, 12, 1 }, { 0xffff, 0x0081, 0, 0 },
	{ WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 0, 188, 0, 12, 12, IDOK }, { 0xffff, 0x0080, L'O', L'k', 0, 0 }
};

static char gs_ConsoleString[128];

int CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	(void)lParam;

	if(uMsg == WM_INITDIALOG)
	{
		SetFocus((HWND)wParam);
		SetWindowText((HWND)wParam, gs_ConsoleString);
		SendMessage((HWND)wParam, EM_SETSEL, 0, -1);
		return FALSE;
	}
	else if(uMsg == WM_COMMAND && (wParam == IDOK || wParam == IDCANCEL))
	{
		if(wParam == IDOK)
			GetDlgItemText(hwndDlg, 1, gs_ConsoleString, sizeof(gs_ConsoleString));
		EndDialog(hwndDlg, wParam);
		return TRUE;
	}
	else
		return FALSE;
}

enum CmdType
{
	CT_INTEGER_VAR, CT_FLOAT_VAR, CT_CALLBACK
};

struct Command
{
	char			mCommand[32];
	enum CmdType	mCommandType;
	void*			mCommandParam;
};

#define countof(array) (sizeof(array) / sizeof(array[0]))

#pragma warning(disable : 4055) // 'type cast' : from data pointer 'void *' to function pointer 'int (__cdecl *)(float)'

extern int	 DX9_GAU_g_iDrawGauges;				// from DX9gauges.c
extern float DX9_GAU_g_fFrameRateAlarmLimit;	// from DX9gauges.c
extern float DX9_GAU_g_fFrameRateTarget;		// from DX9gauges.c
extern int	 DX9_GAU_g_iDrawFrameRate;			// from DX9gauges.c
extern int	gDumpReflectionTexture;
extern int	TEX_MESH_g_BlurWaterNormal;		// from TEX_mesh_x86.c

static struct Command gs_Commands[] =
{
	// keep strings in alphabetical order!!
	{ "dump",				CT_INTEGER_VAR, &gDumpReflectionTexture },
	{ "perf",				CT_INTEGER_VAR,	&DX9_GAU_g_iDrawGauges },
	{ "perf_fps",			CT_INTEGER_VAR,	&DX9_GAU_g_iDrawFrameRate },
	{ "perf_fps_min",		CT_FLOAT_VAR,	&DX9_GAU_g_fFrameRateAlarmLimit },
	{ "perf_fps_target",	CT_FLOAT_VAR,	&DX9_GAU_g_fFrameRateTarget },
	{ "wtr_blur_normal",	CT_INTEGER_VAR, &TEX_MESH_g_BlurWaterNormal }
};

void DX9_ShowConsole( HWND wnd )
{
	if(DialogBoxIndirect(0, &gDlgTemplate.mTemplate, wnd, DialogProc) == IDOK)
	{
		char cmdName[32];
		float cmdValue;
		struct Command* cmd;

		sscanf(gs_ConsoleString, "%s %f", cmdName, &cmdValue);
		cmd = bsearch(cmdName, gs_Commands, countof(gs_Commands), sizeof(gs_Commands[0]), strcmp);
		if(cmd)
		{
			switch(cmd->mCommandType)
			{
			case CT_INTEGER_VAR:
				*(int*)cmd->mCommandParam = (int)cmdValue;
				break;

			case CT_FLOAT_VAR:
				*(float*)cmd->mCommandParam = cmdValue;
				break;

			case CT_CALLBACK:
				((int(*)(float))cmd->mCommandParam)(cmdValue);
				break;
			}
		}
	}
}
