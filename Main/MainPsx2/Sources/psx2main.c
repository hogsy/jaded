/*$T psx2main.c GC 1.138 04/17/03 09:18:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_BENCH
#define __PSX2MAIN_C__

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#include <sys/types.h>
#include <stdio.h>
#include <libgraph.h>
#include <libpc.h>
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/ENGmsg.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "GDInterface.h"
#include "GDIrasters.h"
#include "WORld/WORrender.h"
#include "MEM.h"
#include "PSX2debug.h"
#include "PSX2struct.h"
#include "MEMpro.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "INOut/INOstruct.h"
#include "INOut/INO.h"
#include "Gsp.h"

#include "IOP/RPC_Manager.h"
#include "IOP/CDV_Manager.h"
#include "IOP/ee/eeDebug.h"
#include "INOut/ps2/ps2INOsaving.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"

#ifdef ODE_INSIDE
extern  void LCP_StaticInit(void);
#endif


//#define PSX2_TESTBIN
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
void PSX2_TestBin(void);
extern void AI_EvalFunc_WORLoadList_C(ULONG);


/* static variables */
extern void				GSP_SpecialInit(void);
char					PSX2_gasz_BigFile[L_MAX_PATH] __attribute__((aligned(64)));

/* Purpose: Windows main entry function. */
extern void				GSP_FirstInit_SPR(void);
GDI_tdst_DisplayData	*GDI_pst_GlobalDD;

static char				*p_Filter(char *pcIn, int *piSize);
static char				*p_FindKey(char *pc_In, int size, char *pc_Key);
static void				PSX2_ReadCNF(void);
extern BOOL             ps2MAI_gb_VideoModeNTSC;
extern int              eeRPC_gi_BF;
extern int              eeRPC_i_OpenBigfile(void);
extern void             INO_Joystick_Update(void);    
extern BOOL             ps2INO_b_Port0IsKo(void) ;
extern BOOL             ps2INO_b_Joystick_IsValid(void) ;
extern void ps2INO_NoPad1(void);
extern int 	                        WOR_gi_IsLoading;    
extern BOOL         LOA_gb_Loading ;
/*$4
 ***********************************************************************************************************************
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
	GSP_FirstInit_SPR();
	ph->pst_DisplayData = GDI_gpst_CurDD;

	ph->pst_DisplayData->st_ScreenFormat.ul_Flags |= GDI_Cul_SFF_OccupyAll;

	GDI_fnl_InitInterface(&ph->pst_DisplayData->st_GDI, 0);

	ph->pst_DisplayData->pv_SpecificData = ph->pst_DisplayData->st_GDI.pfnpv_InitDisplay();

	GDI_AttachDisplay(ph->pst_DisplayData); /* , ph->h_DisplayWindow); */
#ifdef RASTERS_ON
	GDI_Rasters_Init(ph->pst_DisplayData->pst_Raster, "Display Data");
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_DestroyDisplay(MAI_tdst_WinHandles *ph)
{
}

/* main function */
#if !defined _FINAL_ && defined GSP_PS2_BENCH
extern u_int	StackInit;
#endif
extern void		PS2_MATH_InitTrigo2(void);
int				PSX2_MainThread(void);
extern void     eeRPC_FlushCommandAfterEngine(void);
extern void 	STR_InitGlobal( void );

extern int 		i_PS2Time;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int PSX2_MainThread(void)
{
    int i;
    
	/* stack (RW) : ORIGIN = 0x01F80000, LENGTH = 0x00080000 */
#if !defined _FINAL_ && defined GSP_PS2_BENCH
	StackInit = 0x00080000 - (GetSP() - 0x01F80000);
//	L_memset((void *) 0x01F80000, 0xC0, GetSP() - 0x01F80020);
#endif

	/*$2- memory rasters ---------------------------------------------------------------------------------------------*/

	MEMpro_Init();

restart:

	/*$2- Common SDK inits -------------------------------------------------------------------------------------------*/

	ENG_InitApplication();
	PSX2_ReadCNF();
	STR_InitGlobal();

    if(ps2MAI_gb_VideoModeNTSC)
		L_strcpy(ps2INO_gsz_GameSaveName, ps2INO_Csz_SceaGameSaveId);
	else
	{
		if ( INO_b_LanguageIsPresent(INO_e_Russian))
			L_strcpy(ps2INO_gsz_GameSaveName, ps2INO_Csz_SceeGameSaveId_rus);
		else if ( INO_b_LanguageIsPresent(INO_e_Polish))
			L_strcpy(ps2INO_gsz_GameSaveName, ps2INO_Csz_SceeGameSaveId_pld);
		else
			L_strcpy(ps2INO_gsz_GameSaveName, ps2INO_Csz_SceeGameSaveId);
	}

	{
		L_time_t    x_Time;
		L_time( &x_Time );
		L_localtime( &x_Time );
		i_PS2Time = x_Time.year;
		i_PS2Time = (i_PS2Time * 12) + x_Time.month;
		i_PS2Time = (i_PS2Time * 31) + x_Time.day;
		i_PS2Time = (i_PS2Time * 24) + x_Time.hour;
		i_PS2Time = (i_PS2Time * 60) + x_Time.minute;
		i_PS2Time = (i_PS2Time * 60) + x_Time.second;
	}


	MEMpro_StartMemRaster();
	Gsp_InitFB(GSP_512x2__, GSP_VRES_x2, 32, 32);
	MEMpro_StopMemRaster(MEMpro_Id_GSP);

	PS2_MATH_InitTrigo2();

#ifdef ODE_INSIDE
	LCP_StaticInit();
#endif	

	/*$2- Open bigfile -----------------------------------------------------------------------------------------------*/


	PROPS2_StartRaster(&PROPS2_gst_Big_Open);
	BIG_Open(PSX2_gasz_BigFile);
	PROPS2_StopRaster(&PROPS2_gst_Big_Open);

	MEMpro_StartMemRaster();
	PROPS2_StartRaster(&PROPS2_gst_s_CreateDisplay);
	s_CreateDisplay(&MAI_gst_MainHandles);
	PROPS2_StopRaster(&PROPS2_gst_s_CreateDisplay);
	GSP_SpecialInit();
	MEMpro_StopMemRaster(MEMpro_Id_GSP);

	/*$2- ENG init ---------------------------------------------------------------------------------------------------*/

    WOR_gi_IsLoading = 1;
    LOA_gb_Loading = TRUE;
	PROPS2_StartRaster(&PROPS2_gst_ENG_InitEngine);
	ENG_InitEngine();
	PROPS2_StopRaster(&PROPS2_gst_ENG_InitEngine);
    
#ifdef PSX2_TESTBIN
    eeRPC_FlushCommandAfterEngine();
#endif	
    

	/*$2- pad check -------------------------------------------------------------------------------------------------*/

    for(i=0; i<80; i++)
    {
        extern u_int volatile VBlankCounter ;
        u_int oldVBlankCounter = 0;
        
        oldVBlankCounter = VBlankCounter;
        while(oldVBlankCounter == VBlankCounter) {};                
        
        INO_Joystick_Update();    
    }
    WOR_gi_IsLoading = 0;
    LOA_gb_Loading = FALSE;
    
    if(! ps2INO_b_Joystick_IsValid() )
        ps2INO_NoPad1();


	/*$2- Call the engine main loop ----------------------------------------------------------------------------------*/
	eeRPC_gi_BF = eeRPC_i_OpenBigfile();

#ifdef PSX2_TESTBIN
    PSX2_TestBin();
#else  
	ENG_Loop();
#endif	

	/*$2- close ------------------------------------------------------------------------------------------------------*/
	{
		extern u32 BinkVideoMode;
		extern int BinkZob( );
	    BinkVideoMode = 2;
	    BinkZob( );
	}

	WOR_Universe_Close(0);
	ENG_CloseEngine();
	BIG_Close();
	ENG_CloseApplication();

	/*$2- delete the wnd handler -------------------------------------------------------------------------------------*/

	s_DestroyDisplay(&MAI_gst_MainHandles);

	/*$2- r ----------------------------------------------------------------------------------------------------------*/

	if(!ENG_gb_ForceEndEngine) goto restart;

	return 0;
}	/* main */

#ifdef PSX2_USE_iopCDV
char JADEPS2NAMES[][100] = 
{
	"",
	"",
	"",
	"",
	"",
	"",
	""
};
#else
char JADEPS2NAMES[][100] = 
{
	"...",
	"...",
	"...",
	"host0:x:/INTRO.BIK",
	"host0:x:/ATTRACT.BIK",
	"host0:x:/KKE.BIK",
	""
};
#endif
char JADEPS2NAMES_SOURCES[][100] = 
{
	"PS2.ICO;1",
	"DATA.BF;1",
	"JADE.SPE;1",
	"INTRO.BIK;1",
	"ATTRACT.BIK;1",
	"KKE.BIK;1",
	""
};
void JADEPS2_ComputePath(char *ElfPath)
{
#ifdef PSX2_USE_iopCDV
	char COM_ELFPATH[100] = "\\DATA\\";
	char *Local, *Dest;
	u32 I;
	if (ElfPath)
	{
		Dest = COM_ELFPATH;
		Local = ElfPath;
		
		if ((ElfPath[0] == 'c') && 
			(ElfPath[1] == 'd') && 
			(ElfPath[2] == 'r') && 
			(ElfPath[3] == 'o') && 
			(ElfPath[4] == 'm') && 
			(ElfPath[5] == '0') && 
			(ElfPath[6] == ':'))
		{
			ElfPath += 7;
		}
		
		while (*Local) 
		{
			Local++;
		};
		while ((*Local != '\\') && (*Local != '/') && (Local > ElfPath))
		{
			Local--;
		}
		while (ElfPath <= Local) 
		{
			*(Dest++) = *(ElfPath++);	
		}
		*(Dest++) = 'D';
		*(Dest++) = 'A';
		*(Dest++) = 'T';
		*(Dest++) = 'A';
		*(Dest++) = '\\';
		*(Dest++) = 0;
	}

	/* Compute definitvie paths */
	for (I = 0 ; I < 7 ; I ++)
	{
		/* */
		Dest = &JADEPS2NAMES[I][0];
		if (I >= 3)
		{
			*(Dest++) = 'c';
			*(Dest++) = 'd';
			*(Dest++) = 'r';
			*(Dest++) = 'o';
			*(Dest++) = 'm';
			*(Dest++) = '0';
			*(Dest++) = ':';
		}
		

		
		Local = COM_ELFPATH;
		while (*Local)
		{
			if (*(Local) == '/')
			{
				*(Dest++) = '\\';
				Local++;
			}
			else
				*(Dest++) = *(Local++);
		}
	
		Local = &JADEPS2NAMES_SOURCES[I][0];
		while (*Local)
		{
			*(Dest++) = *(Local++);
		}
		*(Dest++) = 0;
	}
#endif
}

char *JADEPS2_GETNAME(u32 What)
{
	return JADEPS2NAMES[What];
}

char *GET_BINK_NAME(u32 Context)
{
	// Context: 0 = intro 1 = demo 2 = enddemo
	return JADEPS2_GETNAME(Context+3);
}

extern unsigned int WOR_gul_WorldKey;
extern unsigned int AI_gul_AlternativeVideoKey;

void GSP_ComputeVideoName(char *FileName)
{
	char 	HEXAConv[] = "0123456789ABCDEF";
	ULONG 	ul_Key;
	
	ul_Key = WOR_gul_WorldKey;

	if (AI_gul_AlternativeVideoKey)
	{
		if (AI_gul_AlternativeVideoKey == 0xFFFFFFFF)
			FileName[ 0 ] = 0;
		else
			ul_Key = AI_gul_AlternativeVideoKey;
		AI_gul_AlternativeVideoKey = 0;
	}
	
	sprintf(FileName,"BK_xxxx.BIK");
	FileName[3] = HEXAConv[((WOR_gul_WorldKey >> 12) & 0xf)];
	FileName[4] = HEXAConv[((WOR_gul_WorldKey >> 8) & 0xf)];
	FileName[5] = HEXAConv[((WOR_gul_WorldKey >> 4) & 0xf)];
	FileName[6] = HEXAConv[((WOR_gul_WorldKey >> 0) & 0xf)];
}
void GSP_ComputeVideoPath(char *FileName,char *BNKName)
{
#ifdef PSX2_USE_iopCDV
	sprintf(BNKName,"%s%s;1",JADEPS2_GETNAME(6) , FileName);
#else
	sprintf(BNKName,"host0:X:/%s",FileName);
#endif
}


#ifdef PSX2_USE_iopCDV

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static char *p_Filter(char *pcIn, int *piSize)
{
	/*~~~~~~~~~~~*/
	int		i;
	char	*pcOut;
	int		iOut;
	/*~~~~~~~~~~~*/

	iOut = 0;
	pcOut = MEM_p_Alloc(*piSize);

	for(i = 0; i < *piSize; i++)
	{
		if(pcIn[i] == ' ') continue;
		if(pcIn[i] == '\t') continue;
		if(pcIn[i] == 0x0D) continue;
		if(pcIn[i] == 0x0A) pcIn[i] = 0;
		pcOut[iOut++] = pcIn[i];
	}

	L_memcpy(pcIn, pcOut, iOut);
	MEM_Free(pcOut);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
static char *p_FindKey(char *pc_In, int size, char *pc_Key)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < size; i++)
	{
		if(pc_In[i] != *pc_Key) continue;
		if(L_strncmp(&pc_In[i], pc_Key, strlen(pc_Key)-1)) continue;

		for(; i < size; i++)
		{
			if(pc_In[i] != '=') continue;
			return &pc_In[i + 1];
		}
	}

	return NULL;
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void PSX2_ReadCNF(void)
{
	/*~~~~~~~~~*/
	int		fd;
	int		size;
	char	*pc;
	char	*p1;
	/*~~~~~~~~~*/

#ifdef PSX2_USE_iopCDV
	ps2MAI_gb_VideoModeNTSC = TRUE;
	fd = eeCDV_i_OpenFile("\\SYSTEM.CNF;1");
	if(fd >= 0)
	{
		size = eeCDV_i_GetFileSize(fd);
		pc = MEM_p_Alloc(size);
		eeCDV_i_ReadFile(fd, pc, size);
		p_Filter(pc, &size);
		p1 = p_FindKey(pc, size, "VMODE");
		if(p1)
		{
			if(!L_strcmp(p1, "PAL"))
				ps2MAI_gb_VideoModeNTSC = FALSE;
			else if(!strcmp(p1, "NTSC"))
				ps2MAI_gb_VideoModeNTSC = TRUE;
		}
		eeCDV_i_CloseFile(fd);
		MEM_Free(pc);
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef PSX2_TESTBIN

typedef struct st_TestBin 
{
    int     b_skip;
    char    asz_Name[128];
    ULONG   ul_Key;  
}  tdst_TestBin;

tdst_TestBin gst_WolKey[] = 
{
//*
{0, "_main/_main_logo/_main_logo.wol", 0x3d00c456},
{0, "_main/_main_menu/_main_menu.wol", 0x3d00c45a},
{0, "_main/_main_E3EndScreeen/_main_E3EndScreeen.wol", 0x4902d989},
{0, "PRODUCTION_Levels_Jack/03A_Escape_from_natives/03A_Escape_from_natives.wol", 0xbf00975d},
{0, "PRODUCTION_Levels_Jack/03B_On_Kongs_Tracks/03B_On_Kongs_Tracks.wol", 0x350134e8},
{0, "PRODUCTION_Levels_Jack/03C_Hayes_is_back/03C_Hayes_is_back.wol", 0x02008887},
{0, "PRODUCTION_Levels_Jack/03E_Chased_by_The_Trex/03E_Chased_by_The_Trex.wol", 0x08001b17},
{0, "PRODUCTION_Levels_Jack/03F_Ann_first_escape/03F_Ann_first_escape.wol", 0x09001683},
{0, "PRODUCTION_Levels_Jack/04A_Brontosaurs/04A_Brontosaurs.wol", 0x08002001},
{0, "PRODUCTION_Levels_Jack/04A_Brontosaurs/04A_Brontosaurs_part1/04A_Brontosaurs_part1.wol", 0x08005efa},
{0, "PRODUCTION_Levels_Jack/05A_On_the_raft_Part_1/05A_On_the_raft_Part_1.wol", 0xbf00b3b3},
{0, "PRODUCTION_Levels_Jack/05A_On_the_Raft_Part_2/05A_On_the_Raft_Part_2.wol", 0x0200beca},
{0, "PRODUCTION_Levels_Kong/05C_Kong_vs_first_Trex/05C_Kong_vs_first_Trex.wol", 0xc101eabc},
{0, "PRODUCTION_Levels_Jack/11A_Ann_alone/11A_Ann_alone.wol", 0x080054c2},
//*/
};

void PSX2_TestBin(void)
{
    int i;
    ULONG ul_Fix = 0x3d00c452;
    long128 *p128;
    ULONG uSize;
    extern void MEM_vLogHeapState(void);
    extern void AI_Reset(void);
    extern void WOR_Universe_Open(ULONG);
    extern ULONG MEM_uGetLastBlockSize(MEM_tdst_MainStruct *_pMem);
    extern void MEM_PrintHistory(void);
    extern ULONG    MEM_gul_PrintHistoryMode;


#ifdef _DEBUG    
    printf("*** *** CLEAR 64MB -> 128MB RAM *** ***\n");
    for(p128=(long128*)0x4000000; (long128)p128 < 0x8000000; p128++)
#else    
    printf("*** *** CLEAR 32MB -> 128MB RAM *** ***\n");
    for(p128=(long128*)0x2000000; (long128)p128 < 0x8000000; p128++)
#endif    
    {        *(long*)p128 = (long)0x5aa535FF;
        *((long*)p128 +1) = (long)0xCAFEDECA;
    }




    // load universe instance
    AI_Reset();
	WOR_Universe_Open(BIG_UniverseKey());


#ifdef _DEBUG
    MEM_gul_PrintHistoryMode = 2;  
#endif    


    for(i=0; i<sizeof(gst_WolKey) / sizeof(tdst_TestBin);i++)
    {   
        if(gst_WolKey[i].b_skip) continue;
        
        AI_EvalFunc_WORLoadList_C(gst_WolKey[i].ul_Key);
        
    	uSize = MEM_uGetLastBlockSize(&MEM_gst_MemoryInfo);

#ifdef _DEBUG
        printf("map %s [%x]\n", gst_WolKey[i].asz_Name, gst_WolKey[i].ul_Key);
        MEM_PrintHistory();
        printf("reste dispo %d\n", uSize);
#else
        printf("map %s [%x] reste dispo %d\n", gst_WolKey[i].asz_Name, gst_WolKey[i].ul_Key, uSize);
#endif        

    }
    ENG_gb_ForceEndEngine = TRUE;
    




#ifdef _DEBUG
    printf("*** *** SCAN RAM *** ***\n");
    for(p128=(long128*)0x4000000; (long128)p128 < 0x8000000; p128++)
#else    
    printf("*** *** SCAN RAM *** ***\n");
    for(p128=(long128*)0x2000000; (long128)p128 < 0x8000000; p128++)
#endif    
    {
        if(*(long*)p128 != (long)0x5aa535FF) 
            printf("write detected at 0x%08x : %d %d\n", (int)p128, *(unsigned int*)p128, *((unsigned int*)p128+1) );
            
        if(*((long*)p128 +1) != (long)0xCAFEDECA)
            printf("write detected at 0x%08x : %d %d\n", (int)((long*)p128+1), *((unsigned int*)((long*)p128+1)+1), *((unsigned int*)((long*)p128+1)+1) );
            
    }
    
    
    printf("*** *** BIN LOADING TEST ENDED *** ***\n");
}
#endif
/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
