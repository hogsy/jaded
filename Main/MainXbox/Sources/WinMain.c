/* ============================================================= */
/* ============= Main entry point for XBox Project ============= */
/* ============================================================= */

//#define UseIniFile

#include "Precomp.h"

#include <stdio.h>
#include "ExceptionHandler.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEMpro.h"
#include "BASe/BENch/BENch.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/ENGmsg.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/XBox/xboxStreamingEmul.h"
#include "SouND/sources/SNDconst.h"  
//#include "GDInterface/GDInterface.h"
#include "INOut/INO.h"
#include "ENGine/Sources/text/text.h"

#include "GDInterface/GDIrasters.h"
#include "TIMer/Profiler/XBox/PRO_xb.h"
#include "Gx8/Gx8.h"
#include "Gx8/Gx8init.h"
#include "ENGine/Sources/DEModisk/DEModisk.h"
#include "Gx8/Gx8FileError.h"

#define XBOX_Field_Bin				"Bin"
#define XBOX_Field_Sound			"Sound"
#define XBOX_Field_BigFile			"BigFile"
#define XBOX_Field_ToHd				"CopyToHd"

BOOL Normalmap =0;
BOOL Antialias_E3=0; //<------
BOOL Antialias_PE3=0;

extern BOOL		LOA_gb_SpeedMode;
char			XBOX_gasz_BigFile[260];
static bool	_copyBigFileToHd;


/*
 =======================================================================================================================
 =======================================================================================================================
 */

static void s_CreateDisplay(MAI_tdst_WinHandles *ph)
{
	/* create display */
	ph->h_DisplayWindow = NULL;
	ph->pst_DisplayData = GDI_fnpst_CreateDisplayData();

	GDI_gpst_CurDD = ph->pst_DisplayData;

	ph->pst_DisplayData = GDI_gpst_CurDD;

	ph->pst_DisplayData->st_ScreenFormat.ul_Flags |= GDI_Cul_SFF_OccupyAll;

	GDI_fnl_InitInterface(&ph->pst_DisplayData->st_GDI, 1);

	ph->pst_DisplayData->pv_SpecificData = ph->pst_DisplayData->st_GDI.pfnpv_InitDisplay();

	GDI_AttachDisplay(ph->pst_DisplayData , ph->h_DisplayWindow);

#ifdef RASTERS_ON
	GDI_Rasters_Init(ph->pst_DisplayData->pst_Raster, "Display Data");
#endif

#if defined(XBOX_TUNING) || defined (RASTERS_ON) || defined(GX8_BENCH)
#ifndef _FINAL_
	XB_fn_vCreate();
#endif
#endif // XBOX_TUNING || RASTERS_ON
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int si_Filter(const char *_p_in, char *_p_out, int _i_size)
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

static int s_GetNextField(const char *_p, char *_out)
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

static int si_FindField(const char *_p_in, const char *_p_pattern, char *_p_value, int _i_size)
{
	/*~~~~~~~~~~~~*/
	char	buf[256];
	const char	*end;
	/*~~~~~~~~~~~~*/

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

static int si_AnalyseCommandLine()
{
	/*~~~~~~~~~~~~~~~~*/
	FILE	*i_IniFile;
	char	tmp[10];
	char	*p_FileBuff;
	int	    i_size;
	/*~~~~~~~~~~~~~~~~*/
	char TempString[260];
	char *p = DEM_GetLaunchPath();

	SND_gc_NoSound = 1;
	LOA_gb_SpeedMode = FALSE;
	_copyBigFileToHd = FALSE;

#ifdef UseIniFile

	if(p==NULL)
		strcpy(TempString,"D:\\");
	else
	{
		strcpy(TempString,p);
		//XBUtil_SetMediaPath( p );
	}
	strcat(TempString,"JadeXbox.ini");
	/* read the ini file */
	i_IniFile = fopen(TempString, "r");
	

	if(i_IniFile == 0)
	{
		OutputDebugStringA("#######################################\n");
		OutputDebugStringA("##        CAN'T OPEN INI FILE        ##\n");
		OutputDebugStringA("#######################################\n");
		return -1;
	}

	fseek(i_IniFile, 0, SEEK_END);
	i_size = ftell(i_IniFile);
	fseek(i_IniFile, 0, SEEK_SET);

	p_FileBuff = malloc((size_t)i_size);
    if( p_FileBuff == NULL )
    {
        OutputDebugStringA("No dynamic memory!\n");
        fclose( i_IniFile );
        return -1;
    }
	fread(p_FileBuff, 1,(size_t)i_size, i_IniFile);
	fclose(i_IniFile);

	/* filter ini file */
	si_Filter(p_FileBuff, p_FileBuff, i_size);

	/* switches */
	if( si_FindField(p_FileBuff, XBOX_Field_Sound, tmp, i_size) == 0 )
    {
	strlwr(tmp);
	if(!strncmp(tmp, "on", 2)) SND_gc_NoSound = 0;
    }

	/* bin mode */
	if( si_FindField(p_FileBuff, XBOX_Field_Bin, tmp, i_size) == 0 )
    {
	strlwr(tmp);
	if(!strncmp(tmp, "on", 2)) LOA_gb_SpeedMode = TRUE;
    }

	/* copy to hd */
    if( si_FindField(p_FileBuff, XBOX_Field_ToHd, tmp, i_size ) == 0 )
    {
	strlwr( tmp );
	if( !strncmp(tmp, "on", 2)) _copyBigFileToHd = TRUE;
    }

	si_FindField(p_FileBuff, XBOX_Field_BigFile, XBOX_gasz_BigFile, i_size);

/*	if(p==NULL)
		strcpy(TempString,"D:\\");
	else
	{
		strcpy(TempString,p);
//		XBUtil_SetMediaPath( p );
	}*/

	free(p_FileBuff);
#else

	//WARNING if IniFile not Loaded then set these commands
	
	LOA_gb_SpeedMode = TRUE;//TRUE;FALSE;//
	_copyBigFileToHd = FALSE;
	SND_gc_NoSound = 0;//0/1;

#endif
	
	L_strcpy(XBOX_gasz_BigFile, "D:\\KingKongTheGame.001");
	//L_strcpy(XBOX_gasz_BigFile, "D:\\testwater.bf");
	//L_strcpy(XBOX_gasz_BigFile, "D:\\popo\\game.bf");
	//strcat(XBOX_gasz_BigFile,"KingKongTheGame.bf");

	//L_strcpy(XBOX_gasz_BigFile, "D:\\KingKongfur.bf");
	//L_strcpy(XBOX_gasz_BigFile, "D:\\testufur2mat.bf");//for test
	//L_strcpy(XBOX_gasz_BigFile, "D:\\testnormalRf.bf");//for test
	//L_strcpy(XBOX_gasz_BigFile, "D:\\testnormalLight.bf");//for test
	//L_strcpy(XBOX_gasz_BigFile, "D:\\testucenter2.bf");//for test
	//L_strcpy(XBOX_gasz_BigFile, "D:\\testbody.bf");//for test


	if(p)
	{
		strcpy(TempString,p);
		strcat(TempString,&XBOX_gasz_BigFile[3]);
		strcpy(XBOX_gasz_BigFile,TempString);
	}

	return 0;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
	//int STOP = 1;

void __cdecl main(int argc, char *argv[])
{
    Gx8_tdst_SpecificData   *pst_SD;
	(void) argc;
    (void) argv;
	//prepare all the Demodisk stuff
	
	//while (STOP)	
  	//{};
	
	DEM_InitDemoDisk();
    Gx8_FileError_Init();

	si_AnalyseCommandLine();

		// set rounding behavior of the FPU to "Round toward zero"
		// needed by compiling option /QIfist (suppress _ftol)
//	_controlfp( _RC_CHOP, _MCW_RC );

    MEMpro_Init();
    MEMpro_StartMemRaster();
	Gx8_FirstInit();
    MEMpro_StopMemRaster(MEMpro_Id_GSP);

	// Common SDK inits
	ENG_InitApplication();
	
	//>>>> Force language <<<<
	{
	INO_tden_LanguageId language =(INO_tden_LanguageId)TEXT_gst_Global.i_CurrentLanguage=
	AI_EvalFunc_IoConsLangGet_C();
	}
	s_CreateDisplay(&MAI_gst_MainHandles);
	//BinkPlayer(pst_SD->mp_D3DDevice,0);

	strcpy(MAI_gst_InitStruct.asz_ProjectName, XBOX_gasz_BigFile);
	/*{
		char			popo[]="D:\\popo\\Game.bf";
		strcpy(MAI_gst_InitStruct.asz_ProjectName, popo);
	//}*/
	InitExceptionHandler();
	//SND_gc_NoSound = 1;
	
	// Call the engine main loop
	ENG_InitEngine();


	//Start Intro
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>

/*	SND_MuteAll(TRUE);
	SND_TrackPauseAll(TRUE);
	SND_Update(NULL);
	Mem_CreateForBink();

	BinkPlayer(pst_SD->mp_D3DDevice,0);//INTRO
	BinkPlayer(pst_SD->mp_D3DDevice,3);//UNIVERSAL
	BinkPlayer(pst_SD->mp_D3DDevice,4);//WETA

	Mem_RestoreForBink();
	SND_MuteAll(FALSE);
	SND_TrackPauseAll(FALSE);
	SND_Update(NULL);*/

	// Open bigfile
	xbox_StreamingInit();
	BIG_Open(MAI_gst_InitStruct.asz_ProjectName);
//	BIG_Open(popo);
	//}
	ENG_Loop();

	WOR_Universe_Close(0);
	ENG_CloseEngine();

	// Close bigfile   
	BIG_Close();

	// Close SDK
	ENG_CloseApplication();

	if( INO_isRunMode_Demo() ) DEM_ReturnToDash();
	else DEM_GoToDashBoard();
}
