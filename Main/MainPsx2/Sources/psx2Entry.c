/*$T psx2Entry.c GC 1.138 08/19/03 10:21:02 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include <stdio.h>

#define __PSX2ENTRY_C__
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEMpro.h"
#include "MainPsx2/Sources/PSX2struct.h"
#include "MainPsx2/Sources/psx2Entry.h"
#include "MainPsx2/Sources/libscedemo.h"
#include "WORld/WORrender.h"
#include "IOP/RPC_Manager.h"
#include "IOP/CDV_Manager.h"
#include "IOP/ee/eeDebug.h"
#include "INOut/INOstruct.h"
#include "INOut/INO.h"
#include "INOut/ps2/ps2INOsaving.h"
#include "BIGfiles/VERsion/VERsion_Number.h"


/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */
 
#define Cte_SupervisorPriority	10
#define Cte_ThreadPriority		30
#define Cte_SchedulerFrequency	10
#define Cte_SupervisorStackSize 0x800
#define Cte_MainStackSize		0x80000

/*$4
 ***********************************************************************************************************************
    private types
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    private prototypes
 ***********************************************************************************************************************
 */

static int	s_GetNextField(char *_p, char *_out);
static int	si_AnalyseCommandLine(INT __argc, char *__argv[]);
static void s_PrintSyntax(char *_prog_name);
static int	si_Filter(char *_p_in, char *_p_out, int _i_size);
static int	si_FindField(char *_p_in, char *_p_pattern, char *_p_value, int _i_size);
static int	si_Supervisor(void);
void		sv_Synchronisation(int id, unsigned short time, void *arg);
void		s_PrintConfig(void);
void		DbgExc_ExcHandler(u_int SR, u_int cause, u_int epc, u_int bva, u_int bpa, u_long128 *gpr);
void		DbgExc_InitExceptionHandler(void);
void		DbgExc_DumpMemory(u_int *p_Mem);
static void PSX2_ExitGame(int argc, char *argv[]);
static void PSX2_EntryGame(int argc, char *argv[]);
extern void	ps2SND_ShutDownStreams(void);

/*$4
 ***********************************************************************************************************************
    private variables
 ***********************************************************************************************************************
 */


extern char PSX2_gasz_SysModulePath[L_MAX_PATH];
extern char PSX2_gasz_ModulePath[L_MAX_PATH];


extern int	PSX2_MainThread(void);
char PSX2_gasz_DemoPath[L_MAX_PATH];
extern char PSX2_gasz_BigFile[L_MAX_PATH];
extern BOOL LOA_gb_SpeedMode;
extern char SND_gc_NoSound;
BOOL		ps2MAI_gb_VideoModeNTSC = TRUE;
extern int  INO_gi_LaunchDemo;
extern BOOL AI_gb_Optim2 ;
/*$4
 ***********************************************************************************************************************
    DEMO DEMO
 ***********************************************************************************************************************
 */


/*$2------------------------------------------------------------------------------------------------------------------*/

/*$2------------------------------------------------------------------------------------------------------------------*/

unsigned short		Demo_gus_language = SCE_DEMO_LANGUAGE_ENGLISH;
unsigned short		Demo_gus_aspect = SCE_DEMO_ASPECT_4_3;
unsigned short		Demo_gus_playmode = SCE_DEMO_PLAYMODE_PLAYABLE;
unsigned short		Demo_gus_to_inactive = 2 * 60;
unsigned short		Demo_gus_to_gameplay = 2 * 4 * 60;
sceDemoEndReason	Demo_ge_EndReason = SCE_DEMO_ENDREASON_PLAYABLE_QUIT;

unsigned short		Demo_gus_mediatype = SCE_DEMO_MEDIATYPE_CD;
unsigned int		Demo_gui_dataDirectorySectorNumber = 0;

extern ULONG		ENG_gp_DoubleRendering;

extern BOOL			Demo_gb_IsActive;
unsigned short		Demo_gus_masterVolumeScale = 10;
static int                 PS2_gi_SpecialDebug=0;


/*$4
 ***********************************************************************************************************************
    main
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int main(int argc, char *argv[])
{
	/*~~~~~~~*/
	int syntax;
	void JADEPS2_ComputePath(char *ElfPath);
	/*~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    init
	 -------------------------------------------------------------------------------------------------------------------
	 */
	 
//	JADEPS2_ComputePath("cdrom0:\\hhh.elf;1");
	JADEPS2_ComputePath(argv[0]);

	PSX2_EntryGame(argc, argv);
	eeRPC_i_InitModule();

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    read params
	 -------------------------------------------------------------------------------------------------------------------
	 */

	syntax = si_AnalyseCommandLine(argc, argv);
	if(LOA_gb_SpeedMode) AI_gb_Optim2 = TRUE;
	if(syntax) return syntax;

#if defined(PSX2_DEMO_SCEE)
	ps2MAI_gb_VideoModeNTSC = FALSE;
#elif defined(PSX2_DEMO_SCEA)
	ps2MAI_gb_VideoModeNTSC = TRUE;
#endif

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    load modules
	 -------------------------------------------------------------------------------------------------------------------
	 */

	eeRPC_i_LoadModule((!SND_gc_NoSound) || LOA_gb_SpeedMode);
#if defined(PSX2_USE_iopCDV)
	eeCDV_i_InitModule();
#endif

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    main
	 -------------------------------------------------------------------------------------------------------------------
	 */

	PSX2_MainThread();

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    exit
	 -------------------------------------------------------------------------------------------------------------------
	 */

	PSX2_ExitGame(argc, argv);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void PSX2_EntryGame(int argc, char *argv[])
{
	Demo_gb_IsActive = FALSE;
	if (argc >= 9) Demo_gb_IsActive = TRUE;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    demo
 -----------------------------------------------------------------------------------------------------------------------
 */

	if (Demo_gb_IsActive)
	{
		extern u32 ps2_ScreenMode;
		
		sceDemoStart
		(
			argc,
			argv,
			&Demo_gus_language,
			&Demo_gus_aspect,
			&Demo_gus_playmode,
			&Demo_gus_to_inactive,
			&Demo_gus_to_gameplay,
			&Demo_gus_mediatype,
			&Demo_gus_masterVolumeScale,
			&Demo_gui_dataDirectorySectorNumber
		);
		if (Demo_gus_aspect == SCE_DEMO_ASPECT_16_9)
			ps2_ScreenMode = 2;

	}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    debug
 -----------------------------------------------------------------------------------------------------------------------
 */
#ifndef _FINAL_
#if ( defined(PSX2_TESTBIN) || (!defined(PSX2_MASTER_SCEE) && !defined(PSX2_MASTER_SCEA) && !defined(_DEBUG) && !defined(PSX2_DEMO) && defined(PSX2_USE_iopCDV) ) )
	// only on CD + RELEASE
#ifndef _DEBUG	
	DbgExc_InitExceptionHandler();
#endif	
#endif
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    master
 -----------------------------------------------------------------------------------------------------------------------
 */
#ifdef PSX2_MASTER_SCEE
	INO_ge_MasterType=e_MasterPS2_SCEE;
#elif defined(PSX2_MASTER_SCEA)
	INO_ge_MasterType=e_MasterPS2_SCEA;
#else
    INO_ge_MasterType=e_Master_None;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

char PSX2_gasz_VideoArgv[3][1024];
static void PSX2_ExitGame(int argc, char *argv[])
{
    
	ps2SND_ShutDownStreams();

	sceDemoEnd(Demo_ge_EndReason);

}

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

#ifdef PSX2_USE_iopCDV

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int si_AnalyseCommandLine(INT __argc, char *__argv[])
{
	/*~~*/
	int i;
	/*~~*/

	L_strcpy(PSX2_gasz_BigFile, CDV_Cte_BigFile);
	SND_gc_NoSound = (CDV_Cte_DefaultSnd == TRUE ? 0 : 1);
	LOA_gb_SpeedMode = CDV_Cte_DefaultBin;
	ps2MAI_gb_VideoModeNTSC = TRUE;

	for(i = 0; i < __argc; i++)
	{
		if(__argv[i][0] != '-') continue;
		if(L_strncmp(__argv[i], "--bigfile-", 10) == 0)
		{
			L_strcpy(PSX2_gasz_BigFile, &__argv[i][10]);
			eeDbg_M_MsgX(eeDbg_Msg_000C "%s", PSX2_gasz_BigFile);
		}

		if(L_strncmp(__argv[i], "--sound-on", 10) == 0)
		{
			SND_gc_NoSound = 0;
			eeDbg_M_Msg(eeDbg_Msg_000F "on");
		}

		if(L_strncmp(__argv[i], "--sound-off", 11) == 0)
		{
			SND_gc_NoSound = 1;
			eeDbg_M_Msg(eeDbg_Msg_000F "off");
		}

		if(L_strncmp(__argv[i], "--bin-on", 8) == 0)
		{
			LOA_gb_SpeedMode = TRUE;
			eeDbg_M_Msg(eeDbg_Msg_000D "on");
		}

		if(L_strncmp(__argv[i], "--bin-off", 9) == 0)
		{
			LOA_gb_SpeedMode = FALSE;
			eeDbg_M_Msg(eeDbg_Msg_000D "off");
		}

		if(L_strncmp(__argv[i], "--pal", 9) == 0)
		{
			ps2MAI_gb_VideoModeNTSC = FALSE;
			eeDbg_M_Msg(eeDbg_Msg_000D "off");
		}
	}


	return 0;
}

#else
extern char LOA_SPE_NAME[1024];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int si_AnalyseCommandLine(INT __argc, char *__argv[])
{
	/*~~~~~~~~~~~~~~~~*/
	int		i_IniFile;
	char	tmp[10];
	char	*p_FileBuff;
	int		i_size;
	/*~~~~~~~~~~~~~~~~*/

	/* read the ini file */
	i_IniFile = sceOpen(PSX2_Cte_DefaultIniFile, SCE_RDONLY);

	if(i_IniFile < 0)
	{
		printf("#######################################\n");
		printf("##        CAN'T OPEN INI FILE        ##\n");
		printf("#######################################\n");
		s_PrintSyntax(__argv[0]);
		return -1;
	}

	sceLseek(i_IniFile, 0, SCE_SEEK_SET);
	i_size = sceLseek(i_IniFile, 0, SCE_SEEK_END);
	sceLseek(i_IniFile, 0, SCE_SEEK_SET);
	p_FileBuff = (char *) malloc(i_size);
	sceRead(i_IniFile, p_FileBuff, i_size);
	sceClose(i_IniFile);

	/* filter ini file */
	si_Filter(p_FileBuff, p_FileBuff, i_size);

	/* ini file version check */
	si_FindField(p_FileBuff, PSX2_Field_IniVersion, tmp, i_size);
	if(strncmp(tmp, PSX2_Cte_IniFileVersion, strlen(PSX2_Cte_IniFileVersion)))
	{
		printf("#######################################\n");
		printf("##          BAD INI FILE             ##\n");
		printf("#######################################\n");
		return -1;
	}

	/* bigfile */
	si_FindField(p_FileBuff, PSX2_Field_BigFile, PSX2_gasz_BigFile, i_size);
	si_FindField(p_FileBuff, PSX2_Field_SpeFile, LOA_SPE_NAME, i_size);

	/* IRX modules */
	si_FindField(p_FileBuff, PSX2_Field_SysModulePath, PSX2_gasz_SysModulePath, i_size);
	L_strcat(PSX2_gasz_SysModulePath, "/");
	si_FindField(p_FileBuff, PSX2_Field_HomeModulePath, PSX2_gasz_ModulePath, i_size);
	L_strcat(PSX2_gasz_ModulePath, "/");

	/* switches */
	si_FindField(p_FileBuff, PSX2_Field_Sound, tmp, i_size);
	strlwr(tmp);
	SND_gc_NoSound = 1;
	if(!strncmp(tmp, "on", 2)) SND_gc_NoSound = 0;

	/* bin mode */
	si_FindField(p_FileBuff, PSX2_Field_Bin, tmp, i_size);
	strlwr(tmp);
	LOA_gb_SpeedMode = FALSE;
	if(!strncmp(tmp, "on", 2)) LOA_gb_SpeedMode = TRUE;

	/* pal/ntsc */
	si_FindField(p_FileBuff, PSX2_Field_VideoMode, tmp, i_size);
	strlwr(tmp);
	ps2MAI_gb_VideoModeNTSC = TRUE;
	if(!strncmp(tmp, "pal", 3)) ps2MAI_gb_VideoModeNTSC = FALSE;

	free(p_FileBuff);

	return 0;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_PrintSyntax(char *_prog_name)
{
#ifdef _DEBUG
	printf("** %s: Syntax: jade.elf\n", _prog_name);
	printf("  The program config is in the file "PSX2_Cte_DefaultIniFile ".\n");
#endif
}

/*
 =======================================================================================================================
    suppress space, tab, and DOS carriage return and comment lines £
    add a mark (0xff) at the end of the buffer
 =======================================================================================================================
 */
static int si_Filter(char *_p_in, char *_p_out, int _i_size)
{
	/*~~~~~~*/
	int		i;
	char	c;
	/*~~~~~~*/

	for(i = 0; i < _i_size; i++)
	{
		c = _p_in[i];

		if((c == '/') && (_p_in[i + 1] == '/'))
		{
			do
			{
				c = _p_in[i++];
			} while((c != '\n') && (i < _i_size));
			c = _p_in[i];
		}

		if((c == ' ') || (c == '\r') || (c == '\t')) continue;

		*_p_out++ = c;
	}

	*_p_out = 0xff; /* end mark */
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int s_GetNextField(char *_p, char *_out)
{
	/*~~~~~~*/
	char	c;
	int		n;
	/*~~~~~~*/

	n = 0;

	do
	{
		c = *_p++;
		if((c == '\n') || (c == '=') || ((unsigned char) c == 0xff)) c = '\0';
		_out[n++] = c;
	} while(c);

	return n;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int si_FindField(char *_p_in, char *_p_pattern, char *_p_value, int _i_size)
{
	/*~~~~~~~~~~~~~*/
	char	buf[256];
	char	*end;
	/*~~~~~~~~~~~~~*/

	end = _p_in + _i_size;
	do
	{
		_p_in += s_GetNextField(_p_in, buf);
	} while(L_strcmp(_p_pattern, buf) && (end > _p_in));

	if(end <= _p_in) return -1;
	if(s_GetNextField(_p_in, _p_value) > 0)
		return 0;
	else
		return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void s_PrintConfig(void)
{
	printf("--- --- config --- ---\n");
	printf("Jade version %d-%d\n", BIG_Cu4_AppVersion, BIG_Cu4_Version);

#ifdef PSX2_USE_iopCDV
	printf("CD/DVD config\n");
	printf("big file               : %s\n", CDV_Cte_BigFile);
	printf("spe file               : %s\n", CDV_Cte_SpeFile);
	printf("icon file              : %s\n", CDV_Cte_IcoFile);
	printf("sound                  : %s\n", (SND_gc_NoSound ? "off" : "on"));
	printf("bin                    : %s\n", (LOA_gb_SpeedMode ? "on" : "off"));
#else
	printf(".ini file version      : "PSX2_Cte_IniFileVersion "\n");
	printf("big file               : %s\n", PSX2_gasz_BigFile);
	printf("spe file               : %s\n", LOA_SPE_NAME);
	printf("icon file              : %s\n", "x:/PS2.ICO");
	printf("path for system module : %s\n", PSX2_gasz_SysModulePath);
	printf("path for module        : %s\n", PSX2_gasz_ModulePath);
	printf("sound                  : %s\n", (SND_gc_NoSound ? "off" : "on"));
	printf("bin                    : %s\n", (LOA_gb_SpeedMode ? "on" : "off"));
#endif
	printf("--- --- ------ --- ---\n");
}

/*$4
 ***********************************************************************************************************************
    special section pour debug
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    macros
 -----------------------------------------------------------------------------------------------------------------------
 */

#define DbgExc_Id_ExcInterruption				0
#define DbgExc_Id_ExcTLBchange					1
#define DbgExc_Id_ExcTLBmismatchLoadOrInstFetch 2
#define DbgExc_Id_ExcTLBmismatchStore			3
#define DbgExc_Id_ExcAddrErrorLoadOrInstFetch	4
#define DbgExc_Id_ExcAddrErrorStore				5
#define DbgExc_Id_ExcBusErrorInstFetch			6
#define DbgExc_Id_ExcBusErrorLoadOrStore		7
#define DbgExc_Id_ExcSystemCall					8
#define DbgExc_Id_ExcBreakpoint					9
#define DbgExc_Id_ExcReservedInst				10
#define DbgExc_Id_ExcCoproUnused				11
#define DbgExc_Id_ExcOverflow					12
#define DbgExc_Id_ExcTrap						13
#define DbgExc_Id_ExcVCEI						14
#define DbgExc_Id_ExcFPE						15
#define DbgExc_Id_ExcC2E						16
#define DbgExc_Id_ExcReserved17					17
#define DbgExc_Id_ExcReserved18					18
#define DbgExc_Id_ExcReserved19					19
#define DbgExc_Id_ExcReserved20					20
#define DbgExc_Id_ExcReserved21					21
#define DbgExc_Id_ExcReserved22					22
#define DbgExc_Id_ExcWatch						23
#define DbgExc_Id_ExcReserved24					24
#define DbgExc_Id_ExcReserved25					25
#define DbgExc_Id_ExcReserved26					26
#define DbgExc_Id_ExcReserved27					27
#define DbgExc_Id_ExcReserved28					28
#define DbgExc_Id_ExcReserved29					29
#define DbgExc_Id_ExcReserved30					30
#define DbgExc_Id_ExcVCED						31
#define DbgExc_Cte_ExHandlerNb					32

#define DbgExc_M_SetHandler(_id) \
	{ \
		DbgExc_apfv_ExHandler[_id] = (DbgExc_pfv_ExHandler *) SetDebugHandler(_id, DbgExc_ExcHandler); \
		if(DbgExc_apfv_ExHandler[_id] == DbgExc_ExcHandler) \
			DbgExc_apfv_ExHandler[_id] = (DbgExc_pfv_ExHandler *) NULL; \
	}

char		az[256];
extern void Gsp_CrashPrintf(char *);
#ifndef PSX2_USE_iopCDV
#define DbgExc_M_DisplayReg128(_name, _ptr) \
	{ \
		sprintf \
		( \
			az, \
			"%s 0x%08x.%08x.%08x.%08x\n", \
			(_name), \
			((u_int *) (_ptr))[3], \
			((u_int *) (_ptr))[2], \
			((u_int *) (_ptr))[1], \
			((u_int *) (_ptr))[0] \
		); \
		printf(az); \
		Gsp_CrashPrintf(az); \
	}

#define DbgExc_M_DisplayMem(_marker, _ptr) \
	{ \
		sprintf(az, "%s 0x%08x : 0x%08x\n", _marker, (u_int) (_ptr), *(u_int *) (_ptr)); \
		printf(az); \
		Gsp_CrashPrintf(az); \
	}

#define DbgExc_M_DisplayReg32(_name, _reg) \
	{ \
		sprintf(az, "%s 0x%08x\n", (_name), (_reg)); \
		printf(az); \
		Gsp_CrashPrintf(az); \
	}

#define DbgExc_M_DisplayStr(_ptr)	{ sprintf(az, "%s\n", (_ptr)); iprintf(az); Gsp_CrashPrintf(az); }
#else
#define DbgExc_M_DisplayReg128(_name, _ptr) \
	{ \
		sprintf \
		( \
			az, \
			"%s 0x%08x.%08x.%08x.%08x\n", \
			(_name), \
			((u_int *) (_ptr))[3], \
			((u_int *) (_ptr))[2], \
			((u_int *) (_ptr))[1], \
			((u_int *) (_ptr))[0] \
		); \
		Gsp_CrashPrintf(az); \
	}

#define DbgExc_M_DisplayMem(_marker, _ptr) \
	{ \
		sprintf(az, "%s 0x%08x : 0x%08x\n", _marker, (u_int) (_ptr), *(u_int *) (_ptr)); \
		Gsp_CrashPrintf(az); \
	}

#define DbgExc_M_DisplayReg32(_name, _reg)	{ sprintf(az, "%s 0x%08x\n", (_name), (_reg)); Gsp_CrashPrintf(az); }

#define DbgExc_M_DisplayStr(_ptr)			{ sprintf(az, "%s\n", (_ptr)); Gsp_CrashPrintf(az); }
#endif /* PSX2_USE_iopCDV */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    types
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef void (*DbgExc_pfv_ExHandler) (u_int SR, u_int cause, u_int epc, u_int bva, u_int bpa, u_long128 * gpr);
typedef struct	DbgExc_tdst_Cause_
{
	u_int	pad1 : 2;
	u_int	ExcCode : 5;
	u_int	pad2 : 1;
	u_int	IP : 8;
	u_int	pad3 : 12;
	u_int	CE : 2;
	u_int	pad4 : 1;
	u_int	BD : 1;
} DbgExc_tdst_Cause;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    variables
 -----------------------------------------------------------------------------------------------------------------------
 */

DbgExc_pfv_ExHandler	DbgExc_apfv_ExHandler[DbgExc_Cte_ExHandlerNb];
char					*DbgExc_asz_CauseStrg[DbgExc_Cte_ExHandlerNb] =
{
	"CAUSE : External interuption",
	"CAUSE : TLB change",
	"CAUSE : TLB mismatch on load or I-fetch",
	"CAUSE : TLB mismatch on store",
	"CAUSE : Address error on load or I-fetch",
	"CAUSE : Address error on store",
	"CAUSE : Bus error on I-fetch",
	"CAUSE : Bus error on load or store",
	"CAUSE : System call",
	"CAUSE : Breakpoint",
	"CAUSE : Reserved instruction",
	"CAUSE : Coprocessor unused",
	"CAUSE : Calculation overflow",
	"CAUSE : Trap",
	"CAUSE : Virtual coherency error in I-cache",
	"CAUSE : Floating point error",
	"CAUSE : COP2 exception",
	"CAUSE : Reserved 17",
	"CAUSE : Reserved 18",
	"CAUSE : Reserved 19",
	"CAUSE : Reserved 20",
	"CAUSE : Reserved 21",
	"CAUSE : Reserved 22",
	"CAUSE : Watchpoint",
	"CAUSE : Reserved 24",
	"CAUSE : Reserved 25",
	"CAUSE : Reserved 26",
	"CAUSE : Reserved 27",
	"CAUSE : Reserved 28",
	"CAUSE : Reserved 29",
	"CAUSE : Reserved 30",
	"CAUSE : Virtual coherency error in D-cache"
};

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    functions
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DbgExc_DumpMemory(u_int *p_Mem)
{
	DbgExc_M_DisplayMem(" ", p_Mem - 3);
	DbgExc_M_DisplayMem(" ", p_Mem - 2);
	DbgExc_M_DisplayMem(" ", p_Mem - 1);
	DbgExc_M_DisplayMem(">", p_Mem);
	DbgExc_M_DisplayMem(" ", p_Mem + 1);
	DbgExc_M_DisplayMem(" ", p_Mem + 2);
}

extern u_int	Gsp_Crash(u_int Address, unsigned char *TEXT);
extern void		Gsp_FlipFB(void);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DbgExc_ExcHandler(u_int SR, u_int cause, u_int epc, u_int bva, u_int bpa, u_long128 *gpr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DbgExc_tdst_Cause	st_cause;
	u_int				tepc;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    PS2_gi_SpecialDebug = 1;

	*(u_int *) &st_cause = cause;

	if(st_cause.BD)
	{
		DbgExc_M_DisplayStr("Excp in delay slot.");
		tepc = epc + 4;
	}
	else
		tepc = epc;

	if(st_cause.ExcCode < DbgExc_Cte_ExHandlerNb)
		Gsp_Crash(tepc, DbgExc_asz_CauseStrg[st_cause.ExcCode]);
	else
		Gsp_Crash(tepc, "CAUSE : Unknown");

	/* DbgExc_M_DisplayStr(" EXCEPTION "); */
	if(st_cause.ExcCode < DbgExc_Cte_ExHandlerNb)
	{
		DbgExc_M_DisplayStr(DbgExc_asz_CauseStrg[st_cause.ExcCode]);
	}
	else
	{
		DbgExc_M_DisplayStr("CAUSE : Unknown");
	}

	/*
	 * DbgExc_M_DisplayReg32("Exception PC :", tepc); £
	 * DbgExc_M_DisplayStr(" COP0 SR "); DbgExc_M_DisplayReg32("cause", cause);
	 * DbgExc_M_DisplayReg32("epc ", epc); DbgExc_M_DisplayReg32("stat ", SR);
	 * DbgExc_M_DisplayReg32("bva ", bva); DbgExc_M_DisplayReg32("bpa ", bpa); £
	 * DbgExc_M_DisplayStr(" COP0 GPR ");
	 */
	DbgExc_M_DisplayReg128("at", gpr + 1);
	DbgExc_M_DisplayReg128("v0", gpr + 2);
	DbgExc_M_DisplayReg128("v1", gpr + 3);
	DbgExc_M_DisplayReg128("a0", gpr + 4);
	DbgExc_M_DisplayReg128("a1", gpr + 5);
	DbgExc_M_DisplayReg128("a2", gpr + 6);
	DbgExc_M_DisplayReg128("a3", gpr + 7);
	DbgExc_M_DisplayReg128("t0", gpr + 8);
	DbgExc_M_DisplayReg128("t1", gpr + 9);
	DbgExc_M_DisplayReg128("t2", gpr + 10);
	DbgExc_M_DisplayReg128("t3", gpr + 11);
	DbgExc_M_DisplayReg128("t4", gpr + 12);
	DbgExc_M_DisplayReg128("t5", gpr + 13);
	DbgExc_M_DisplayReg128("t6", gpr + 14);
	DbgExc_M_DisplayReg128("t7", gpr + 15);
	DbgExc_M_DisplayReg128("s0", gpr + 16);
	DbgExc_M_DisplayReg128("s1", gpr + 17);
	DbgExc_M_DisplayReg128("s2", gpr + 18);
	DbgExc_M_DisplayReg128("s3", gpr + 19);
	DbgExc_M_DisplayReg128("s4", gpr + 20);
	DbgExc_M_DisplayReg128("s5", gpr + 21);
	DbgExc_M_DisplayReg128("s6", gpr + 22);
	DbgExc_M_DisplayReg128("s7", gpr + 23);
	DbgExc_M_DisplayReg128("t8", gpr + 24);
	DbgExc_M_DisplayReg128("t9", gpr + 25);
	DbgExc_M_DisplayReg128("k0", gpr + 26);
	DbgExc_M_DisplayReg128("k1", gpr + 27);
	DbgExc_M_DisplayReg128("gp", gpr + 28);
	DbgExc_M_DisplayReg128("sp", gpr + 29);
	DbgExc_M_DisplayReg128("fp", gpr + 30);
	DbgExc_M_DisplayReg128("ra", gpr + 31);

	Gsp_FlipFB();

#if 0
	if(DbgExc_apfv_ExHandler[cause])
	{
		DbgExc_M_DisplayStr("call next exception handler...");
		DbgExc_apfv_ExHandler[cause](SR, cause, epc, bva, bpa, gpr);
	}
#endif
	DbgExc_M_DisplayStr("lock the excution in infinite loop...");

l_infinite:
	goto l_infinite;
	ExitHandler();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DbgExc_InitExceptionHandler(void)
{
    PS2_gi_SpecialDebug = 1;
    
	DbgExc_M_SetHandler(DbgExc_Id_ExcTLBchange);
	DbgExc_M_SetHandler(DbgExc_Id_ExcTLBmismatchLoadOrInstFetch);
	DbgExc_M_SetHandler(DbgExc_Id_ExcTLBmismatchStore);
	DbgExc_M_SetHandler(DbgExc_Id_ExcAddrErrorLoadOrInstFetch);
	DbgExc_M_SetHandler(DbgExc_Id_ExcAddrErrorStore);
	DbgExc_M_SetHandler(DbgExc_Id_ExcBusErrorInstFetch);
	DbgExc_M_SetHandler(DbgExc_Id_ExcBusErrorLoadOrStore);

	/*
	 * DbgExc_M_SetHandler(DbgExc_Id_ExcSystemCall); £
	 * DbgExc_M_SetHandler(DbgExc_Id_ExcBreakpoint);
	 */
	DbgExc_M_SetHandler(DbgExc_Id_ExcReservedInst);
	DbgExc_M_SetHandler(DbgExc_Id_ExcCoproUnused);
	DbgExc_M_SetHandler(DbgExc_Id_ExcOverflow);

	/* DbgExc_M_SetHandler(DbgExc_Id_ExcTrap); */
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
