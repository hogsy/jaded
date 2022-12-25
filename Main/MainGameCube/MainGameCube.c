/*$T MainGameCube.c GC! 1.081 02/04/03 16:15:40 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Hist: 18/10/01 : Dany Joannette - creation
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include <types.h>
#include <stdio.h>

#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/ENGmsg.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "GDInterface.h"
#include "GDIrasters.h"
#include "WORld/WORrender.h"
#include "MEM.h"
#include "INOut/INO.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#ifdef _DEBUG
#define _OSReport(_a_)	OSReport(_a_)
#else
#define _OSReport(_a_)
#endif

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

GDI_tdst_DisplayData	*GDI_pst_GlobalDD;
char					GC_gasz_BigFile[1024];
char					SND_gc_NoSound;
extern BOOL 			AI_gb_Optim2 ;


/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

static int				si_AnalyseCommandLine(INT __argc, char *[]);
int						gamecubeMainThread(void);
#ifdef _FINAL_
#define GC_InstallExceptionHandler()
#else
static void				GC_InstallExceptionHandler(void);
#endif

void 		GC_TestBin(void);
extern void AI_EvalFunc_WORLoadList_C(ULONG);

//#define 	_GAMECUBE_TESTBIN

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_CreateDisplay(MAI_tdst_WinHandles *ph)
{
	/* create display */
	ph->pst_DisplayData = GDI_pst_GlobalDD = GDI_fnpst_CreateDisplayData();

	GDI_gpst_CurDD = ph->pst_DisplayData;

	/* DJ_TEMP - GSP_FirstInit_SPR(); */
	ph->pst_DisplayData = GDI_gpst_CurDD;

	ph->pst_DisplayData->st_ScreenFormat.ul_Flags |= GDI_Cul_SFF_OccupyAll;

	GDI_fnl_InitInterface(&ph->pst_DisplayData->st_GDI, 0);

	/*
	 * ph->pst_DisplayData->pv_SpecificData =
	 * ph->pst_DisplayData->st_GDI.pfnpv_InitDisplay();
	 */
	GDI_AttachDisplay(ph->pst_DisplayData); /* , ph->h_DisplayWindow); */

#ifdef RASTERS_ON
	GDI_Rasters_Init(ph->pst_DisplayData->pst_Raster, "Display Data");
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int si_AnalyseCommandLine(INT __argc, char *__argv[])
{
	/*~~*/
	int i;
	/*~~*/

	/* default settings */
	L_strcpy(GC_gasz_BigFile, "Rayman4.bf");
	SND_gc_NoSound = 0;
	LOA_gb_SpeedMode = TRUE;

	for(i = 0; i < __argc; i++)
	{
		if(__argv[i][0] != '-') continue;

		if(L_strncmp(__argv[i], "--bigfile-", 10) == 0)
		{
			sprintf(GC_gasz_BigFile, "[main] New Bigfile name : %s\n", &__argv[i][10]);
			_OSReport(GC_gasz_BigFile);
			L_strcpy(GC_gasz_BigFile, &__argv[i][10]);
		}

		if(L_strncmp(__argv[i], "--sound-on", 10) == 0)
		{
			SND_gc_NoSound = 0;
			_OSReport("[main] Sound is enabled\n");
		}

		if(L_strncmp(__argv[i], "--sound-off", 11) == 0)
		{
			SND_gc_NoSound = 1;
		}

		if(L_strncmp(__argv[i], "--bin-on", 8) == 0)
		{
			LOA_gb_SpeedMode = TRUE;
			_OSReport("[main] Bin mode is enabled\n");
		}

		if(L_strncmp(__argv[i], "--bin-off", 9) == 0)
		{
			LOA_gb_SpeedMode = FALSE;
		}
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gamecubeMainThread(void)
{

	/*$2- start ------------------------------------------------------------------------------------------------------*/

	OSReport("InitApplication\n");

	ENG_InitApplication();

	OSReport("BIG_Open\n");
	BIG_Open(GC_gasz_BigFile);

	OSReport("s_CreateDisplay\n");
	s_CreateDisplay(&MAI_gst_MainHandles);

	OSReport("ENG_InitEngine\n");
	ENG_InitEngine();

	OSReport("Init done, start loop\n");
	
#ifdef _GAMECUBE_TESTBIN
    GC_TestBin();
#else    
	ENG_Loop();
#endif	

	/*$2- close ------------------------------------------------------------------------------------------------------*/

	WOR_Universe_Close(0);
	ENG_CloseEngine();

	/* BIG_Close(); */
	ENG_CloseApplication();

	/*$2- restart ----------------------------------------------------------------------------------------------------*/

	if(ENG_gb_ForceEndEngine) return 1;

	return 0;
}	/* main */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 #include "DolphinTrkInit.h"
int main(int argc, char *argv[])
{

	/*InitMetroTRK();
	EnableMetroTRKInterrupts();
	InitMetroTRK_BBA();*/

	OSInit();
	VIInit();
	DVDInit();
	PADInit();

#ifndef _DEBUG
	GC_InstallExceptionHandler();
#endif

	
	LOA_SetIsSwapperActive(TRUE);
	si_AnalyseCommandLine(argc, argv);
	if(LOA_gb_SpeedMode) AI_gb_Optim2 = TRUE;

	while(gamecubeMainThread())
	{

	};
	return 0;
}

/*$4
 ***********************************************************************************************************************
    Exception/interrupt
 ***********************************************************************************************************************
 */

#ifdef _FINAL_
#else

/*$2-macros ----------------------------------------------------------------------------------------------------------*/

#define M_SetErrorHandler(_id)	GC_gapfv_OldHandler[_id] = OSSetErrorHandler(_id, (OSErrorHandler) GC_ErrHandler)
#ifdef USE_OSREPORT
#define M_DisplayBegin()
#define M_DisplayEnd()
#define M_DisplayStr(_str)							OSReport("%s\n", _str)
#define M_DisplayR32(_str, _val)					OSReport("%s %08X\n", _str, _val)
#define M_DisplayR16(_str, _val)					OSReport("%s %04X\n", _str, _val)
#define M_Display2xR32(_str1, _val1, _str2, _val2)	OSReport("%s %08X %s %08X\n", _str1, _val1, _str2, _val2)
#define M_Display4xR32(_str1, _val1, _str2, _val2, _str3, _val3, _str4, _val4) \
	OSReport \
	( \
		"%s %08X - %08X - %08X - %08X\n", \
		_str1, \
		_val1, \
		_val2, \
		_val3, \
		_val4 \
	)
#define M_DisplayF64(_str, _val)					OSReport("%s %5.2f\n", _str, _val)
#define M_Display2xF64(_str1, _val1, _str2, _val2)	OSReport("%s %5.2f %s %5.2f\n", _str1, _val1, _str2, _val2)
#define M_Display4xF64(_str1, _val1, _str2, _val2, _str3, _val3, _str4, _val4) \
	OSReport \
	( \
		"%s %5.2f - %5.2f - %5.2f - %5.2f\n", \
		_str1, \
		_val1, \
		_val2, \
		_val3, \
		_val4 \
	)
#else
void 	GXI_ErrBegin(void);
void	GXI_ErrEnd(void);
void	GXI_ErrPrint(char *);
char	gaz[256];

#define M_DisplayBegin()	GXI_ErrBegin()
#define M_DisplayEnd()		GXI_ErrEnd()

#define M_DisplayStr(_str) \
	do \
	{ \
		sprintf(gaz, "%s", _str); \
		GXI_ErrPrint(gaz); \
	} while(0);

#define M_DisplayR32(_str, _val) \
	do \
	{ \
		sprintf(gaz, "%s %08X", _str, _val); \
		GXI_ErrPrint(gaz); \
	} while(0);

#define M_DisplayR16(_str, _val) \
	do \
	{ \
		sprintf(gaz, "%s %04X", _str, _val); \
		GXI_ErrPrint(gaz); \
	} while(0);

#define M_Display2xR32(_str1, _val1, _str2, _val2) \
	do \
	{ \
		sprintf(gaz, "%s %08X %s %08X", _str1, _val1, _str2, _val2); \
		GXI_ErrPrint(gaz); \
	} while(0);

#define M_Display4xR32(_str1, _val1, _str2, _val2, _str3, _val3, _str4, _val4) \
	do \
	{ \
		sprintf(gaz, "%s %08X - %08X - %08X - %08X", _str1, _val1, _val2, _val3, _val4); \
		GXI_ErrPrint(gaz); \
	} while(0);

#define M_DisplayF64(_str, _val) \
	do \
	{ \
		sprintf(gaz, "%s %5.2f", _str, _val); \
		GXI_ErrPrint(gaz); \
	} while(0);

#define M_Display2xF64(_str1, _val1, _str2, _val2) \
	do \
	{ \
		sprintf(gaz, "%s %5.2f %s %5.2f", _str1, _val1, _str2, _val2); \
		GXI_ErrPrint(gaz); \
	} while(0);

#define M_Display4xF64(_str1, _val1, _str2, _val2, _str3, _val3, _str4, _val4) \
	do \
	{ \
		sprintf(gaz, "%s %5.2f - %5.2f - %5.2f - %5.2f", _str1, _val1, _val2, _val3, _val4); \
		GXI_ErrPrint(gaz); \
	} while(0);

#endif

/*$2- variables ------------------------------------------------------------------------------------------------------*/

OSErrorHandler	GC_gapfv_OldHandler[OS_ERROR_MAX];

/*$2- prototypes -----------------------------------------------------------------------------------------------------*/

void			GC_ErrHandler(OSError error, OSContext *, u32 dsisr, u32 dar);

/*$2- handler --------------------------------------------------------------------------------------------------------*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GC_ErrHandler(OSError error, OSContext *context, u32 dsisr, u32 dar)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OSContext	exceptionContext;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1- save ctxt ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	OSClearContext(&exceptionContext);
	OSSetCurrentContext(&exceptionContext);

	/*$1- cause ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_DisplayBegin();

		M_DisplayStr("*** CRASH ***");
	
		switch(error)
		{
		case OS_ERROR_SYSTEM_RESET:
			M_DisplayStr("Cause = SysReset");
			break;
		case OS_ERROR_MACHINE_CHECK:
			M_DisplayStr("Cause = MachineCheck");
			break;
		case OS_ERROR_DSI:
			M_DisplayStr("Cause = DSI");
			break;
		case OS_ERROR_ISI:
			M_DisplayStr("Cause = ISI");
			break;
		case OS_ERROR_EXTERNAL_INTERRUPT:
			M_DisplayStr("Cause = ExtIT");
			break;
		case OS_ERROR_ALIGNMENT:
			M_DisplayStr("Cause = Disalignment");
			break;
		case OS_ERROR_PROGRAM:
			M_DisplayStr("Cause = Program");
			break;
		case OS_ERROR_FLOATING_POINT:
			M_DisplayStr("Cause = FP");
			break;
		case OS_ERROR_DECREMENTER:
			M_DisplayStr("Cause = Decrementer");
			break;
		case OS_ERROR_SYSTEM_CALL:
			M_DisplayStr("Cause = SysCall");
			break;
		case OS_ERROR_TRACE:
			M_DisplayStr("Cause = Trace");
			break;
		case OS_ERROR_PERFORMACE_MONITOR:
			M_DisplayStr("Cause = PerfMonitor");
			break;
		case OS_ERROR_BREAKPOINT:
			M_DisplayStr("Cause = Break");
			break;
		case OS_ERROR_SYSTEM_INTERRUPT:
			M_DisplayStr("Cause = SysIT");
			break;
		case OS_ERROR_THERMAL_INTERRUPT:
			M_DisplayStr("Cause = ThIT");
			break;
		case OS_ERROR_PROTECTION:
			M_DisplayStr("Cause = Protection");
			break;
		default:
			M_DisplayStr("Cause = Unknown");
			break;
		}

	/*$1- location ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Exception handling registers */

	/*$F
	 *
	 * SRR0 = Exc PC,£
	 * SRR1 = Exc MSR £
	 * DSISR = cause in case of ISI/DSI £
	 * DAR = Data Addr in case of ISI/DSI £
	 * CR = condition £
	 * LR = caller £
	 * CTR = count register £
	 * XER = test/condition
	 *
	 */
	M_Display2xR32("SRR0 ", context->srr0, "SRR1", context->srr1);
	M_Display2xR32("DSISR", dsisr, "DAR ", dar);
	M_Display2xR32("CR   ", context->cr, "LR  ", context->lr);
	M_DisplayStr(" ");

#ifdef DISPLAY_TEST_REGISTER
	M_Display2xR32("CTR  ", context->ctr, "XER  ", context->xer);
#endif

	/*$1- General-purpose registers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_Display4xR32
	(
		"GPR00",
		context->gpr[0],
		"GPR01",
		context->gpr[1],
		"GPR02",
		context->gpr[2],
		"GPR03",
		context->gpr[3]
	);
	M_Display4xR32
	(
		"GPR04",
		context->gpr[4],
		"GPR05",
		context->gpr[5],
		"GPR06",
		context->gpr[6],
		"GPR07",
		context->gpr[7]
	);
	M_Display4xR32
	(
		"GPR08",
		context->gpr[8],
		"GPR09",
		context->gpr[9],
		"GPR10",
		context->gpr[10],
		"GPR11",
		context->gpr[11]
	);
	M_Display4xR32
	(
		"GPR12",
		context->gpr[12],
		"GPR13",
		context->gpr[13],
		"GPR14",
		context->gpr[14],
		"GPR15",
		context->gpr[15]
	);
	M_Display4xR32
	(
		"GPR16",
		context->gpr[16],
		"GPR17",
		context->gpr[17],
		"GPR18",
		context->gpr[18],
		"GPR19",
		context->gpr[19]
	);
	M_Display4xR32
	(
		"GPR20",
		context->gpr[20],
		"GPR21",
		context->gpr[21],
		"GPR22",
		context->gpr[22],
		"GPR23",
		context->gpr[23]
	);
	M_Display4xR32
	(
		"GPR24",
		context->gpr[24],
		"GPR25",
		context->gpr[25],
		"GPR26",
		context->gpr[26],
		"GPR27",
		context->gpr[27]
	);
	M_Display4xR32
	(
		"GPR28",
		context->gpr[28],
		"GPR29",
		context->gpr[29],
		"GPR30",
		context->gpr[30],
		"GPR31",
		context->gpr[31]
	);

	/*$1- Floating-point registers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_DisplayR16("MODE", context->mode);
	M_DisplayR16("STATE", context->state);
	M_Display2xR32("FPSCR-PAD", context->fpscr_pad, "FPSCR    ", context->fpscr);

#ifdef DISPLAY_FPR
	M_Display4xF64
	(
		"FPR00",
		context->fpr[0],
		"FPR01",
		context->fpr[1],
		"FPR02",
		context->fpr[2],
		"FPR03",
		context->fpr[3]
	);
	M_Display4xF64
	(
		"FPR04",
		context->fpr[4],
		"FPR05",
		context->fpr[5],
		"FPR06",
		context->fpr[6],
		"FPR07",
		context->fpr[7]
	);
	M_Display4xF64
	(
		"FPR08",
		context->fpr[8],
		"FPR09",
		context->fpr[9],
		"FPR10",
		context->fpr[10],
		"FPR11",
		context->fpr[11]
	);
	M_Display4xF64
	(
		"FPR12",
		context->fpr[12],
		"FPR13",
		context->fpr[13],
		"FPR14",
		context->fpr[14],
		"FPR15",
		context->fpr[15]
	);
	M_Display4xF64
	(
		"FPR16",
		context->fpr[16],
		"FPR17",
		context->fpr[17],
		"FPR18",
		context->fpr[18],
		"FPR19",
		context->fpr[19]
	);
	M_Display4xF64
	(
		"FPR20",
		context->fpr[20],
		"FPR21",
		context->fpr[21],
		"FPR22",
		context->fpr[22],
		"FPR23",
		context->fpr[23]
	);
	M_Display4xF64
	(
		"FPR24",
		context->fpr[24],
		"FPR25",
		context->fpr[25],
		"FPR26",
		context->fpr[26],
		"FPR27",
		context->fpr[27]
	);
	M_Display4xF64
	(
		"FPR28",
		context->fpr[28],
		"FPR29",
		context->fpr[29],
		"FPR30",
		context->fpr[30],
		"FPR31",
		context->fpr[31]
	);
#endif

/*$1- Place Gekko regs at the end so we have minimal changes to existing code ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef DISPLAY_GQR
	M_Display4xR32("GQR0", context->gqr[0], "GQR1", context->gqr[1], "GQR2", context->gqr[2], "GQR3", context->gqr[3]);
	M_Display4xR32("GQR4", context->gqr[4], "GQR5", context->gqr[5], "GQR6", context->gqr[6], "GQR7", context->gqr[7]);
#endif
#ifdef DISPLAY_PSF
	M_Display4xF64
	(
		"PSF00",
		context->psf[0],
		"PSF01",
		context->psf[1],
		"PSF02",
		context->psf[2],
		"PSF03",
		context->psf[3]
	);
	M_Display4xF64
	(
		"PSF04",
		context->psf[4],
		"PSF05",
		context->psf[5],
		"PSF06",
		context->psf[6],
		"PSF07",
		context->psf[7]
	);
	M_Display4xF64
	(
		"PSF08",
		context->psf[8],
		"PSF09",
		context->psf[9],
		"PSF10",
		context->psf[10],
		"PSF11",
		context->psf[11]
	);
	M_Display4xF64
	(
		"PSF12",
		context->psf[12],
		"PSF13",
		context->psf[13],
		"PSF14",
		context->psf[14],
		"PSF15",
		context->psf[15]
	);
	M_Display4xF64
	(
		"PSF16",
		context->psf[16],
		"PSF17",
		context->psf[17],
		"PSF18",
		context->psf[18],
		"PSF19",
		context->psf[19]
	);
	M_Display4xF64
	(
		"PSF20",
		context->psf[20],
		"PSF21",
		context->psf[21],
		"PSF22",
		context->psf[22],
		"PSF23",
		context->psf[23]
	);
	M_Display4xF64
	(
		"PSF24",
		context->psf[24],
		"PSF25",
		context->psf[25],
		"PSF26",
		context->psf[26],
		"PSF27",
		context->psf[27]
	);
	M_Display4xF64
	(
		"PSF28",
		context->psf[28],
		"PSF29",
		context->psf[29],
		"PSF30",
		context->psf[30],
		"PSF31",
		context->psf[31]
	);
#endif

	/*$1- restore context ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	OSEnableInterrupts();

	M_DisplayEnd();

	while(1)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		extern void GXI_Crash(void);
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* GXI_Crash(); */
	}

	OSSetCurrentContext(context);
	OSClearContext(&exceptionContext);
}

/*$2- install --------------------------------------------------------------------------------------------------------*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void GC_InstallExceptionHandler(void)
{
	L_memset(GC_gapfv_OldHandler, 0, OS_ERROR_MAX * sizeof(OSErrorHandler));

	M_SetErrorHandler(OS_ERROR_SYSTEM_RESET);
	M_SetErrorHandler(OS_ERROR_MACHINE_CHECK);
	M_SetErrorHandler(OS_ERROR_DSI);
	M_SetErrorHandler(OS_ERROR_ISI);

	/* M_SetErrorHandler(OS_ERROR_EXTERNAL_INTERRUPT); */
	M_SetErrorHandler(OS_ERROR_ALIGNMENT);

	/*
	 * M_SetErrorHandler(OS_ERROR_PROGRAM);£
	 * M_SetErrorHandler(OS_ERROR_FLOATING_POINT);£
	 * M_SetErrorHandler(OS_ERROR_DECREMENTER);£
	 * M_SetErrorHandler(OS_ERROR_SYSTEM_CALL);£
	 * M_SetErrorHandler(OS_ERROR_TRACE);£
	 * M_SetErrorHandler(OS_ERROR_PERFORMACE_MONITOR);£
	 * M_SetErrorHandler(OS_ERROR_BREAKPOINT);£
	 */
	M_SetErrorHandler(OS_ERROR_SYSTEM_INTERRUPT);
	M_SetErrorHandler(OS_ERROR_THERMAL_INTERRUPT);
	M_SetErrorHandler(OS_ERROR_PROTECTION);
}

#endif



#ifdef _GAMECUBE_TESTBIN

unsigned int gui_WolKey[] = {
/*_main/_main_credits/_main_credits*/ 0x5e006334,
/*_main/_main_logo/_main_logo*/ 0x5e00631b,
/*_main/_main_menu/_main_menu*/ 0x5e00631a,
/*_main/_main_newgame/_main_newgame*/ 0x5e00631f,
/*_main/_Main_VideoE3/_Main_VideoE3*/ 0x26001d26,
/*_mdisk/Mdisk_00_01_notes_vaisseau/Mdisk_00_01_notes_vaisseau*/ 0xd0003103,
/*_mdisk/MDisk_00_02_Mr_de_Castellac/MDisk_00_02_Mr_de_Castellac*/ 0xc10093de,
/*_mdisk/MDisk_01_05_Pour_Sally/MDisk_01_05_Pour_Sally*/ 0xd9006111,
/*_mdisk/MDisk_01_08_Cine/MDisk_01_08_Cine*/ 0x3500a1a3,
/*_mdisk/MDisk_01_10_databank_army/MDisk_01_10_databank_army*/ 0xd0002b5b,
/*_mdisk/MDisk_02_03_brief_entrepot/MDisk_02_03_brief_entrepot*/ 0x10001848,
/*_mdisk/Mdisk_06_10_animaux_rares/Mdisk_06_10_animaux_rares*/ 0xd00030a8,
/*_mdisk/MDisk_06_12_catalog_mamago/MDisk_06_12_catalog_mamago*/ 0x02004764,
/*_mdisk/Mdisk_SPOON_511/Mdisk_SPOON_511*/ 0xd00030da,
/*_mdisk/Mdisk_SPOON_512/Mdisk_SPOON_512*/ 0xd00030e8,
/*_mdisk/Mdisk_SPOON_513_entrepot/Mdisk_SPOON_513_entrepot*/ 0xd00030f1,
/*_mdisk/Mdisk_SPOON_514_egouts/Mdisk_SPOON_514_egouts*/ 0xd00030fa,
/*_mdisk/MDisk_JeuDuPalet/Mdisk_JeuDuPalet*/ 0x49022627,
/*00_home/00_01_home_sas_hangar/00_01_home_sas_hangar*/ 0xd90057fd,
/*00_home/00_02_home_decryptage/00_02_home_decryptage*/ 0xd9005833,
/*00_home/00_03_dehors_maison_Intro/00_03_dehors_maison_Intro*/ 0x100045a2,
/*00_home/00_03_dehors_maison_Phare_detruit/00_03_dehors_maison_Phare_detruit*/ 0x100046a6,
/*00_home/00_03_dehors_maison_Waf/00_03_dehors_maison_Waf*/ 0x100045a9,
/*01_entrepot/01_00_entrepot_sas_entree/01_00_entrepot_sas_entree*/ 0x2000791b,
/*01_entrepot/01_01_entrepot_ascenseur_central/01_01_entrepot_ascenseur_central*/ 0x20006b96,
/*01_entrepot/01_02_entrepot_salle_fusibles/01_02_entrepot_salle_fusibles*/ 0x20006bfc,
/*01_entrepot/01_03_entrepot_2caisses_crans_vie/01_03_entrepot_2caisses_crans_vie*/ 0xd90054d1,
/*01_entrepot/01_04_entrepot_liberation_mili/01_04_entrepot_liberation_mili*/ 0xe6001fbe,
/*01_entrepot/01_05_entrepot_enlevement_peyj/01_05_entrepot_enlevement_peyj*/ 0xd9005520,
/*01_entrepot/01_06_entrepot_defile_cercueils/01_06_entrepot_defile_cercueils*/ 0xc1001814,
/*01_entrepot/01_07_entrepot_debarras_caisses/01_07_entrepot_debarras_caisses*/ 0x20007685,
/*01_entrepot/01_08_entrepot_cachot_peyj/01_08_entrepot_cachot_peyj*/ 0x100004f4,
/*01_entrepot/01_09_entrepot_boss/01_09_entrepot_boss*/ 0x5e000352,
/*01_entrepot/01_10_entrepot_ramener_cyclope/01_10_entrepot_ramener_cyclope*/ 0x20007821,
/*01_entrepot/01_11_entrepot_tuyau/01_11_entrepot_tuyau*/ 0xc1009e0a,
/*01_entrepot/01_12_Entrepot_tutorial_stealth_01/01_12_Entrepot_tutorial_stealth_01*/ 0x10005b59,
/*01_entrepot/01_13_entrepot_tutorial_stealth_02/01_13_entrepot_tutorial_stealth_02*/ 0x020054f1,
/*01_entrepot/01_14_entrepot_tutorial_stealth_00/01_14_entrepot_tutorial_stealth_00*/ 0x020056db,
/*03_egouts/03_00_egouts_circuit_start/03_00_egouts_circuit_start*/ 0x02000bdd,
/*03_egouts/03_01_Ville_circuit_start/03_01_Ville_circuit_start*/ 0x0200443c,
/*03_egouts/03_04_egouts_ouverture_grille/03_04_egouts_ouverture_grille*/ 0xc100201a,
/*03_egouts/03_05_egouts_cyclope_derriere/03_05_egouts_cyclope_derriere*/ 0x02000569,
/*03_egouts/03_06_egouts_abattoir_exterieur/03_06_egouts_abattoir_exterieur*/ 0x10001006,
/*03_egouts/03_07_egouts_abattoir_interieur/03_07_egouts_abattoir_interieur*/ 0x0200056c,
/*03_egouts/03_08_egouts_bonus_grille/03_08_egouts_bonus_grille*/ 0xc1009415,
/*03_egouts/03_09_egouts_rencontre_passeur/03_09_egouts_rencontre_passeur*/ 0xd9006147,
/*03_egouts/03_10_egouts_acces_usine/03_10_egouts_acces_usine*/ 0xc10097d5,
/*03_egouts/03_11_Egouts_Porte_Usine/03_11_Egouts_Porte_Usine*/ 0x10002301,
/*03_egouts/03_12_egouts_caisses_mines/03_12_egouts_caisses_mines*/ 0x0200141d,
/*03_egouts/03_14_egouts_stealth_01/03_14_egouts_stealth_01*/ 0x1000686c,
/*03_egouts/03_15_egouts_stealth_02/03_15_egouts_stealth_02*/ 0x1000686d,
/*04_vaisseau/04_00_vaisseau_hyllis_planete/04_00_vaisseau_hyllis_planete*/ 0x62000988,
/*04_vaisseau/04_01_vaisseau_hyllis_espace/04_01_vaisseau_hyllis_espace*/ 0x62000b30,
/*04_vaisseau/04_02_vaisseau_lune/04_02_vaisseau_lune*/ 0xd9007061,
/*05_ilot/05_00_ilot_sas_accueil/05_00_ilot_sas_accueil*/ 0xc100948d,
/*05_ilot/05_01_ilot_plaques/05_01_ilot_plaques*/ 0xd9005efb,
/*05_ilot/05_02_ilot_boss/05_02_ilot_boss*/ 0x0200112d,
/*05_ilot/05_03_Ilot_Medusa/05_03_Ilot_Medusa*/ 0x10002009,
/*06_Animaux/06_00_Animaux_Canaux/06_00_Animaux_Canaux*/ 0x1000257a,
/*06_Animaux/06_01_animaux_quartier_pietons/06_01_animaux_quartier_pietons*/ 0xd90066fd,
/*06_Animaux/06_02_animaux_bar_paradise/06_02_animaux_bar_paradise*/ 0xc1009be5,
/*06_Animaux/06_03_animaux_boutique_electro/06_03_animaux_boutique_electro*/ 0xc1009c0b,
/*06_Animaux/06_04_Animaux_quartier_pietons_revolution/06_00_Animaux_quartier_pietons_revolution*/ 0x3d0046b0,
/*06_Animaux/06_08_animaux_Repaire_Spoon/06_08_animaux_Repaire_Spoon*/ 0x3500e127,
/*06_Animaux/06_12_animaux_garage/06_12_animaux_garage*/ 0x10002c8d,
/*06_Animaux/06_13_animaux_secret_02/06_13_animaux_secret_02*/ 0xc1013e65,
/*06_Animaux/06_18_animaux_minimap_matrix/06_18_animaux_minimap_matrix*/ 0xc100e439,
/*07_courses/07_00_course_00/07_00_course_00*/ 0xc100fb16,
/*07_courses/07_02_course_02/07_02_course_02*/ 0xc1008cd8,
/*07_courses/07_03_course_03/07_03_course_03*/ 0x020004c1,
/*07_courses/07_04_course_04/07_04_course_04*/ 0x750024ea,
/*08_Satellite/08_00_satellite_entree/08_00_satellite_entree*/ 0xc100abb4,
/*08_Satellite/08_03_satellite_emetteur/08_03_satellite_emetteur*/ 0xd9006f33,
/*09_Nazh/09_00_nazh_liberation_peyj/09_00_nazh_liberation_peyj*/ 0xc100ae8a,
/*09_Nazh/09_01_nazh_boss/09_01_nazh_boss*/ 0xc100b7ad,
/*09_Nazh/09_01_nazh_boss_cine_finale/09_01_nazh_boss_cine_finale*/ 0x3500ea33,
/*09_Nazh/09_02_nazh_boss_ascenseur/09_02_nazh_boss_ascenseur*/ 0xc1015021,
/*10_Lune/10_02_lune_faisceaux/10_02_lune_faisceaux*/ 0xd900738e,
/*11_minimaps/11_00_vieux_fou_01/11_00_vieux_fou_01*/ 0x750033cb,
/*11_minimaps/11_01_vieux_fou_02/11_01_vieux_fou_02*/ 0x59000c5b,
/*11_minimaps/11_02_vieux_fou_03/11_02_vieux_fou_03*/ 0x63003501,
/*11_minimaps/11_03_vieux_fou_04/11_03_vieux_fou_04*/ 0x59001d41,
/*11_minimaps/11_04_tresor_alpha_01/11_04_tresor_alpha_01*/ 0x83000fc8,
/*11_minimaps/11_05_tresor_alpha_02/11_05_tresor_alpha_02*/ 0x4a0011b4,
/*11_minimaps/11_06_tresor_alpha_03/11_06_tresor_alpha_03*/ 0xc100dd6b,
/*11_minimaps/11_07_tapis_roulant/11_07_tapis_roulant*/ 0xc100f3f8,
/*11_minimaps/11_08_Combat_01/11_08_Combat_01*/ 0xe8001fba,
/*11_minimaps/11_10_Combat_03/11_10_Combat_03*/ 0x830013a3,
/*11_minimaps/11_13_Combat_06/11_13_Combat_06*/ 0x75003e1b,
/*50_Gplay/YO_PEARL/YO_pearl*/ 0x49029588
};

void GC_TestBin(void)
{
    int i, lang;
    unsigned int ui_Fix = 0x15e0084df;
    void *gp_NextFreeAfterDefrag = NULL;
    extern void *MEM_gp_NextFreeAfterDefrag;
    extern void TEXT_ChangeLang(int);
    
   

    OSReport("Loading map fix [%x]\n", ui_Fix);
    MEM_gp_NextFreeAfterDefrag = NULL;
    AI_EvalFunc_WORLoadList_C(ui_Fix);
    
    for(lang = 0; lang<INO_e_MaxLangNb; lang++)
    {
    	if(!INO_b_LanguageIsPresent(lang)) continue;
		
		OSReport("--- --- --- --- << language idx %d >> --- --- --- ---\n", lang);
    	TEXT_ChangeLang(lang);
       	
       	
    	
	    for(i=0; i<sizeof(gui_WolKey) / sizeof(unsigned int);i++)
	    {   
	        OSReport("--- --- Loading map  [%x] --- ---\n", gui_WolKey[i]);
	        MEM_gp_NextFreeAfterDefrag = NULL;

	        AI_EvalFunc_WORLoadList_C(gui_WolKey[i]);
	        
	        if(gp_NextFreeAfterDefrag && (gp_NextFreeAfterDefrag != MEM_gp_NextFreeAfterDefrag))
	        {
	        	OSReport("*** *** memory leaks in [%x] : %d oct *** ***\n", gui_WolKey[i-1], (int)MEM_gp_NextFreeAfterDefrag-(int)gp_NextFreeAfterDefrag);
	        }
	       	gp_NextFreeAfterDefrag = MEM_gp_NextFreeAfterDefrag;
	        	
	    }
    }
    ENG_gb_ForceEndEngine = TRUE;
    ENG_gb_ExitApplication = TRUE;
    

     
    OSReport("*** *** BIN LOADING TEST ENDED *** ***\n");
}
#endif
/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

