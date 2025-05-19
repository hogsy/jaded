/*$T LOAdefs.c GC 1.138 07/02/03 10:40:07 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "Precomp.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"
#include "LINks/LINKmsg.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAerrid.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGio.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BASarray.h"
#include "MATHs/MATHfloat.h"

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "ENGine/Sources/WORld/WORuniverse.h"


#ifdef _XENON
#include "../../MainXenon/Sources/BinarizedBFManager.h"
#endif // _XENON

#include "TIMer/TIMdefs.h"
#include "BIGfiles/BIGcomp.h"
#include "TIMer/PROfiler/PROPS2.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

// hogsy: 40 ?? what is this magic number crap... TODO: use an enum for this!
ULONG LOA_ul_FileTypeSize[40] =
{
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0
};

ULONG			LOA_gul_CurrentKey = 0;

#ifdef ACTIVE_EDITORS
BOOL			LOA_gb_LogBin = FALSE;
ULONG			LOA_gul_BinSize = 0;

extern CHAR		*LOA_GetBinaryBuffer(void);
extern CHAR		*LOA_GetCurBinaryBuffer(void);
#endif
BIG_KEY			LOA_ul_BinKey = 0;
LOA_tdst_Ref	*LOA_gpst_CurRef = NULL;

extern CHAR		*LOA_FetchBuffer(ULONG _ul_Length);
extern int		WOR_gi_IsLoadingFix;

/*$4
 ***********************************************************************************************************************
    extern prototypes
 ***********************************************************************************************************************
 */

extern void WORCheck_AllRef_AddRef(ULONG, ULONG);
#ifdef PSX2_TARGET
extern void eeSND_StartScheduler(int);
extern void eeSND_StopScheduler(int);
#else
#define eeSND_StartScheduler(_a)
#define eeSND_StopScheduler(_a)
#endif

/*$4
 ***********************************************************************************************************************
    special surrounding for file access
 ***********************************************************************************************************************
 */

#if defined(PSX2_USE_iopCDV) //PS2 + CD
#define M_OpenFile(_file)			eeCDV_i_OpenFile(_file)
#define M_GetFileSize(_size, _id)	(_size) = eeCDV_i_GetFileSize(_id)
#define M_ReadFile(_id, _buf, _len)	eeCDV_i_ReadFile(_id, _buf, _len)
#define M_CloseFile(_id)			eeCDV_i_CloseFile(_id)

#else
#define M_OpenFile(_file)			L_fopen(_file, L_fopen_RB)
#define M_ReadFile(_id, _buf, _len)	L_fread(_buf, _len, 1, _id)
#define M_CloseFile(_id)			L_fclose(_id)


#ifdef PSX2_TARGET
#define M_GetFileSize(_size, _id)   ((_size) = sceLseek(_id, 0, SEEK_END), sceLseek(_id, 0, SEEK_SET))
#elif defined(_XBOX)
#define M_GetFileSize(_size, _id)   _size = GetFileSize(_id, NULL);
#else
#define M_GetFileSize(_size, _id) 	do{L_fseek(_id, 0, SEEK_END); _size = L_ftell(_id); L_fseek(_id, 0, SEEK_SET);}while(0)
#endif

#endif

/*$4
 ***********************************************************************************************************************
    extern variables
 ***********************************************************************************************************************
 */

extern int		BIG_gi_ReadMode;	/* 0 normal, 1 write, 2 read buffer */
extern char		*BIG_gp_ReadBuffer; /* Temps buffer for special read/write mode */
extern int		BIG_gi_ReadSize;	/* Size of buffer above */
extern ULONG	BIG_gul_ReadPos;
extern ULONG	BIG_gul_ReadFat;
extern int		BIG_gi_ReadSeek;	/* Current seek */
extern int		BIG_gi_RealSeek;	/* Current seek */
extern char		*BIG_gp_CompressedBuffer;
extern char		*BIG_gp_CompressedBuffer_r;
extern volatile float BIG_gf_DispBinProgress ;
extern unsigned int	BIG_gui_DispBinProgressSize ;

/*$4
 ***********************************************************************************************************************
    private variables
 ***********************************************************************************************************************
 */

int			EDI_gi_GenSpe = 0;
static int	LOA_si_SaveReadMode = -1;
BOOL		LOA_gb_CompressBin = TRUE;
BOOL		LOA_sb_ActivateSoundLoadAfterBin = FALSE;

#ifdef PSX2_TARGET
#ifdef PSX2_USE_iopCDV
#define LOA_SPE_NAME	CDV_Cte_SpeFile
#else
char		LOA_SPE_NAME[1024];
#endif
#else
#if defined(_XBOX) || defined(_XENON)
#define LOA_SPE_NAME	"D:\\jade.spe"
#else
#define LOA_SPE_NAME	"jade.spe"
#endif /* _XBOX */
#endif

/*$4
 ***********************************************************************************************************************
    Global variables
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
BAS_tdst_barray LOA_gst_RefArray;
int				LOA_gi_InitRef = 0;
#endif
#define LOA_C_MaxStack 10000
LOA_tdst_Ref	*LOA_gpst_LastRef = NULL;

#define WOR_Cte_MaxFixWorldNb	8
BAS_tdst_barray LOA_gst_SpecialArrayKey[WOR_Cte_MaxFixWorldNb];
BAS_tdst_barray LOA_gst_SpecialArrayAddr;
BAS_tdst_barray LOA_gst_SpecialArrayAddrToDestroy;
int				LOA_gi_SpecialArrayNb=0;

LOA_tdst_Ref	LOA_gast_Stack[LOA_C_MaxStack];
int				LOA_gi_CurRef = 0;
int				LOA_gi_LastRef = 0;
BOOL			LOA_gb_SpeedMode = FALSE;
BOOL			LOA_gb_SlashY = FALSE;

/* Array to remember loading address depending on file pos */
#ifndef ACTIVE_EDITORS
BAS_tdst_barray LOA_gst_Array[256];
BAS_tdst_barray LOA_gst_InvArray;
#else
BAS_tdst_barray LOA_gst_Array;
#endif

/* Array to remember key depending of loading address */
#ifdef ACTIVE_EDITORS
BAS_tdst_barray LOA_gst_ArrayAdr;
#endif

/*$4
 ***********************************************************************************************************************
    proto
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
void			LOA_LoadExFileKey(void);
ULONG			LOA_ul_ExFileKey(ULONG _ul_InKey);
void			LOA_UnloadExFileKey(void);

ULONG			LOA_gul_MakeFileRefSpy = -1;
ULONG			LOA_gul_MakeFileRefSpyCaller = -1;
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

BOOL LOA_bIsInTrashDir(const char *_sCompleteName)
{
    char sCompleteNameUpper[256];
    strcpy(sCompleteNameUpper,_sCompleteName);
    _strupr(sCompleteNameUpper);

    return (strstr(sCompleteNameUpper,"TRASH") || strstr(sCompleteNameUpper,"TRACH"));
}

void LOA_CheckIfInTrashDir(BIG_INDEX _ul_Fat)
{
    char sPath[256];
    char sCompleteName[256];

    // Display warning when directory or filename contains "Trash"
    BIG_ComputeFullName(BIG_ParentFile(_ul_Fat), sPath);
    sprintf(sCompleteName,"%s/%s",sPath,BIG_NameFile(_ul_Fat));

    if (LOA_bIsInTrashDir(sCompleteName))
    {
        char sMsg[256];
        sprintf(sMsg,"[WARNING] ######## File loaded in trash dir : %s/%s ######## ",sPath,BIG_NameFile(_ul_Fat));
        LINK_PrintStatusMsg(sMsg);
    }
}

BOOL LOA_bLogLoad = FALSE;

ULONG LOA_g_ul_RefStackSize = 0;
BIG_KEY LOA_g_ul_RefStack[1000];

void LOA_LogLoad(LOA_tdst_Ref *_pstRef)
{
    char sMsg[256];
    char szFilename[256];
	ULONG ul_FATIndex;

    if (!LOA_bLogLoad)
        return;

    ul_FATIndex = BIG_ul_SearchKeyToFat(_pstRef->ul_Key);
    BIG_ComputeFullName(BIG_ParentFile(ul_FATIndex), szFilename);
    strcat( szFilename, "/" );
    strcat( szFilename, BIG_NameFile(ul_FATIndex) );

#ifdef _DEBUG
    sprintf(sMsg,"Loading %s(%08x) with %s\\n",szFilename,_pstRef->ul_Key,_pstRef->s_CallbackName);
#else //_DEBUG
    sprintf(sMsg,"Loading %s(%08x)\\n",szFilename,_pstRef->ul_Key);
#endif // _DEBUG

    LINK_PrintStatusMsgEOLCanal(sMsg,3);
    LOA_g_ul_RefStackSize = 0;
}

void LOA_AddRefToLog(BIG_KEY _ul_Key)
{
    if (!LOA_bLogLoad)
        return;

    LOA_g_ul_RefStack[LOA_g_ul_RefStackSize] = _ul_Key;
    LOA_g_ul_RefStackSize++;
    if (LOA_g_ul_RefStackSize >= 999)
        LOA_g_ul_RefStackSize = 999;

}

void LOA_PrintRefToLog()
{
    int i,iMaxLine;
    char sMsg[256];
    

    if (LOA_g_ul_RefStackSize == 0 || !LOA_bLogLoad)
        return;

    sprintf(sMsg,"%08x references ",LOA_gul_CurrentKey);
    for (i=0,iMaxLine=0; i<(int)LOA_g_ul_RefStackSize; i++,iMaxLine++)
    {
         char sNb[256];
         sprintf(sNb,"%08x ",LOA_g_ul_RefStack[i]);
         strcat(sMsg,sNb);

         if (iMaxLine>=10)
         {
             iMaxLine = 0;
             strcat(sMsg,"\\n");
             LINK_PrintStatusMsgEOLCanal(sMsg,3);
             sMsg[0] = 0;
         }
    }

    if (iMaxLine>0)
    {
        strcat(sMsg,"\\n");
        LINK_PrintStatusMsgEOLCanal(sMsg,3);
    }

}


#endif //ACTIVE_EDITORS


#if !defined(XML_CONV_TOOL)
/*$off*/
#ifdef _DEBUG
	void _LOA_MakeFileRef(BIG_KEY _ul_Key, ULONG * _pul_Res, LOA_tdpfnul_CallBack _pfnul_CallBack, char _c_Type, char *_s_CallbackName, char *_psz_File, int _i_Line)
#else
	void _LOA_MakeFileRef(BIG_KEY _ul_Key, ULONG * _pul_Res, LOA_tdpfnul_CallBack _pfnul_CallBack, char _c_Type)
#endif
/*$on*/
{
	LOA_tdst_Ref	*pst_Ref;

	ERR_X_Assert(LOA_gi_CurRef != ((LOA_gi_LastRef + 1) % LOA_C_MaxStack));
	

#ifdef ACTIVE_EDITORS
	/* WORCheck_AllRef_AddRef( LOA_gul_CurrentKey, _ul_Key ); */
	_ul_Key = LOA_ul_ExFileKey(_ul_Key);
#endif


	if(_ul_Key == 0)
	{
		ERR_X_Warning(0, "A reference key is null", NULL);
		if(_pul_Res) *_pul_Res = (ULONG) NULL;
	}
	else if(_ul_Key == 0xFFFFFFFF)
	{
		ERR_X_Warning(0, "A reference key is invalid", NULL);
		if(_pul_Res) *_pul_Res = (ULONG) NULL;
	}
	else
	{
#ifdef _DEBUG
		static ULONG	ul_Break = (ULONG) 1;
		if(ul_Break == _ul_Key)
		{
			ul_Break = _ul_Key;
		}
#endif


#ifdef ACTIVE_EDITORS
        LOA_AddRefToLog(_ul_Key);

		if(LOA_gul_MakeFileRefSpy != -1)
		{
			if(_ul_Key == LOA_gul_MakeFileRefSpy)
			{
				char log[512];
				ULONG ulFat;

				ulFat = BIG_ul_SearchKeyToFat(LOA_gul_CurrentKey);
				if(ulFat != BIG_C_InvalidIndex)
				{
					sprintf(log, "[LOA]=> %08x is referenced by [%08x] %s",LOA_gul_MakeFileRefSpy, LOA_gul_CurrentKey, BIG_NameFile(ulFat));

					LINK_gul_ColorTxt = 0x00FF0000;
					LINK_PrintStatusMsg(log);
					LINK_gul_ColorTxt = 0;
				}
			}
		}
#ifdef _DEBUG
        AI_vAddKeyDependency(_ul_Key,LOA_gul_CurrentKey,_s_CallbackName);
#else // _DEBUG
        AI_vAddKeyDependency(_ul_Key,LOA_gul_CurrentKey,NULL);
#endif // _DEBUG

#endif // ACTIVE_EDITORS

		pst_Ref = &LOA_gast_Stack[LOA_gi_LastRef];
		LOA_gpst_LastRef = pst_Ref;
		pst_Ref->ul_Key = _ul_Key;
		pst_Ref->pul_Res = _pul_Res;
		pst_Ref->pfnul_CallBack = _pfnul_CallBack;
		pst_Ref->c_Type = _c_Type;
#ifdef _DEBUG
        pst_Ref->s_CallbackName = _s_CallbackName;
		pst_Ref->psz_File = _psz_File;
		pst_Ref->i_NumLine = _i_Line;
#endif
		LOA_gi_LastRef = (LOA_gi_LastRef + 1) % LOA_C_MaxStack;
	}
}
#endif // XML_CONV_TOOL
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_TakeBetterPos(ULONG _ul_ActPos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i, ii;
	int				ul_Dif, ul_BestDif, ul_BestIndex;
	LOA_tdst_Ref	st_Ref;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_BestDif = 0xFFFFFFFF;
	i = LOA_gi_CurRef;
	ul_BestIndex = ii = i = (i + 1) % LOA_C_MaxStack;

	while((i % LOA_C_MaxStack) != LOA_gi_LastRef)
	{
		ul_Dif = LOA_gast_Stack[i].ul_Key;
		ul_Dif = BIG_ul_SearchKeyToPos(ul_Dif);
		if((unsigned int) ul_Dif - _ul_ActPos < (unsigned int) ul_BestDif)
		{
			ul_BestDif = ul_Dif - _ul_ActPos;
			ul_BestIndex = i;
		}

		i = (i + 1) % LOA_C_MaxStack;
	}

	if(ul_BestDif != (int) 0xFFFFFFFF)
	{
		L_memcpy(&st_Ref, &LOA_gast_Stack[ii], sizeof(st_Ref));
		L_memcpy(&LOA_gast_Stack[ii], &LOA_gast_Stack[ul_BestIndex], sizeof(st_Ref));
		L_memcpy(&LOA_gast_Stack[ul_BestIndex], &st_Ref, sizeof(st_Ref));
	}
}

extern int	WOR_gi_CurrentConsole;
BOOL LOA_gb_Loading=FALSE;

#ifdef PSX2_TARGET
extern void ps2SND_SetBinSeek(ULONG);
#else
#define ps2SND_SetBinSeek(a)
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int LOA_gi_LoadNb=0; 
void LOA_BeginSpeedMode(BIG_KEY _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LOA_tdst_Ref	*pst_Ref;
	ULONG			ul_Pos;
	extern int		gi_TestBin;

#ifdef ACTIVE_EDITORS
	char			az[100];
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Redefinition des tailles des blocs de compression/decompression pour GameCube et PS2. */
#if !defined(MAX_PLUGIN)
	if((WOR_gi_CurrentConsole == 2) || (WOR_gi_CurrentConsole == 1))
	{
		BIGcomp_M_BlockSize = 250 * 1024;
		BIGcomp_M_CompressedSize = 512 * 1024;
	}
#endif // !defined(MAX_PLUGIN)

#if !defined(MAX_PLUGIN)
	gi_TestBin = -1;
#endif // !defined(MAX_PLUGIN)

	eeSND_StopScheduler(_ul_Key);

	if
	(
		(_ul_Key == 0xFF800000)
	||	(_ul_Key == 0xFF400000)
	||	((_ul_Key & 0xFF000000) == 0xFD000000)
	||	((_ul_Key & 0xFF000000) == 0xFE000000)
	) LOA_sb_ActivateSoundLoadAfterBin = FALSE;
	else
	{
		LOA_sb_ActivateSoundLoadAfterBin = TRUE;
#if !defined(MAX_PLUGIN)
		SND_gst_Params.pst_RefForVol = NULL;
#endif // !defined(MAX_PLUGIN)
		LOA_gb_Loading = TRUE;
	}

    
	if(!LOA_gb_SpeedMode)
	{
#if defined(ACTIVE_EDITORS) || defined(PCWIN_TOOL)
		if((_ul_Key & 0xFFF00000) != 0xFF800000)
#endif
			
		switch(_ul_Key)
		{
		case 0xFF800000:
		case 0xFF400000:
			break;

		default:
			if((_ul_Key & 0xFF000000) == 0xFE000000)
				break;
			else if((_ul_Key & 0xFF000000) == 0xFD000000)
				break;
#if !defined(MAX_PLUGIN)
			else
				SND_NotifyNewLoading();
#endif // !defined(MAX_PLUGIN)
		}

		return;
	}

	/*
	 * printf("[EE] BeginSpeedMode 0x%x\n", _ul_Key); £
	 * if special map that includes texture or sound without wol
	 */
	if(!LOA_ul_BinKey && (_ul_Key == 0xFF800000)) return;
	if(!LOA_ul_BinKey && (_ul_Key == 0xFF400000)) return;
	if(!LOA_ul_BinKey && ((_ul_Key & 0xFF000000) == 0xFE000000)) return;

	MEMpro_StartMemRaster();
	PROPS2_StartRaster(&PROPS2_gst_SpeedMode);

	switch(_ul_Key)
	{
	case 0xFF800000:
	
#ifdef _XENON	
		// Switch to the binarized BF containing textures to load
		if ( LOA_gb_SpeedMode )
		{
			g_BinarizedBFManager.SwitchBinarizedBF(eBinarizedBFTextures);
		}
#endif // _XENON

		/* texture bin file */
#if !defined(MAX_PLUGIN)
		gi_TestBin = 1;
#endif // !defined(MAX_PLUGIN)
		_ul_Key = 0xFF800000 | (LOA_ul_BinKey & 0x0007FFFF);
		LOA_gb_CompressBin = TRUE;
		LOA_sb_ActivateSoundLoadAfterBin = FALSE;
		LOA_StartLoadRaster(LOA_Cte_LDI_SpeedMode_Display);
		LOA_gi_LoadNb++;
		break;

	case 0xFF400000:
		/* sound bin file */
#if !defined(MAX_PLUGIN)
		gi_TestBin = 4;
#endif // !defined(MAX_PLUGIN)
		_ul_Key = 0xFF400000 | (LOA_ul_BinKey & 0x0007FFFF);
		LOA_gb_CompressBin = FALSE;
		LOA_sb_ActivateSoundLoadAfterBin = FALSE;
		LOA_StartLoadRaster(LOA_Cte_LDI_SpeedMode_Sound);
		break;

	default:
		if((_ul_Key & 0xFF000000) == 0xFE000000)
		{
			/* header sound bin file */
#if !defined(MAX_PLUGIN)
			gi_TestBin = 3;
#endif // !defined(MAX_PLUGIN)
			LOA_gb_CompressBin = TRUE;
			LOA_sb_ActivateSoundLoadAfterBin = FALSE;
			LOA_StartLoadRaster(LOA_Cte_LDI_SpeedMode_Sound2);
		}
		else if((_ul_Key & 0xFF000000) == 0xFD000000)
		{
			/* text bin file */
#if !defined(MAX_PLUGIN)
			gi_TestBin = 2;
#endif // !defined(MAX_PLUGIN)
			LOA_gb_CompressBin = TRUE;
			LOA_sb_ActivateSoundLoadAfterBin = FALSE;
			LOA_StartLoadRaster(LOA_Cte_LDI_SpeedMode_Text);
		}
		else
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			extern int	AllPreloadDone(void);
			extern void SND_NotifyNewLoading(void);
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if !defined(MAX_PLUGIN)
			SND_NotifyNewLoading();
#endif // !defined(MAX_PLUGIN)

#ifdef PSX2_TARGET
			/* this can not be activated during bin process */
			if(AllPreloadDone())
			{
				
				LOA_ul_BinKey = 0xFF000000 | (_ul_Key & 0x0007FFFF);
			
			}
#endif

#ifdef JADEFUSION
			{
#if !defined(MAX_PLUGIN)
				if(!SND_gc_NoSound) SND_StreamStopAll();
#endif // !defined(MAX_PLUGIN)
			}
#endif

#if !defined(MAX_PLUGIN)
			gi_TestBin = 0;
#endif // !defined(MAX_PLUGIN)
			_ul_Key = 0xFF000000 | (_ul_Key & 0x0007FFFF);
			LOA_gb_CompressBin = TRUE;
			LOA_sb_ActivateSoundLoadAfterBin = TRUE;
		}
		break;
	}

	LOA_ul_BinKey = _ul_Key;

	pst_Ref = &LOA_gast_Stack[LOA_gi_CurRef];
	BIG_gi_ReadSeek = 0;
	BIG_gp_CompressedBuffer = BIG_gp_ReadBuffer = NULL;
	BIG_gi_ReadSize = 0;
	BIG_gi_ReadMode = 0;

	/* search the bin file */
	ul_Pos = BIG_ul_SearchKeyToPos(_ul_Key);
	if(ul_Pos != 0xFFFFFFFF)
	{
		if((_ul_Key & 0xFFF00000) == 0xFF400000)
			ps2SND_SetBinSeek(ul_Pos + 4);
		
		BIG_gul_ReadPos = ul_Pos;
		BIG_gi_ReadMode = 2;
#if defined(_GAMECUBE) || defined(PSX2_TARGET)
		BIG_gi_ReadSize = 1024 * 1024;
#else
		BIG_gi_ReadSize = 2 * 1024 * 1024;	/* alloc 2Mo instead of BIGcomp_M_BlockSize = optimization of big clusters
											 * in MEM module; */
#endif
		BIG_gi_RealSeek = BIG_gi_ReadSeek = 0;
		BIG_gf_DispBinProgress = 0.0f;
		BIG_gui_DispBinProgressSize = 0;
	}

#ifdef ACTIVE_EDITORS
	else
	{
		sprintf(az, "%x.bin", _ul_Key);
		BIG_gul_ReadFat = ul_Pos = BIG_ul_CreateFile("Root/Bin", az);

		BIG_DeleteKeyToFat(BIG_FileKey(ul_Pos));

		BIG_FileKey(ul_Pos) = _ul_Key;
		BIG_UpdateOneFileInFat(ul_Pos);

		BIG_InsertKeyToFat(_ul_Key, ul_Pos);

		BIG_gi_ReadMode = 1;

		/*
		 * LOA_InitBinaryBuffer(4096, 4096); test RFe pour accélérer la binarisation avec
		 * checksum
		 */
		LOA_InitBinaryBuffer(102400, 102400);
	}
#endif
	MEMpro_StopMemRaster(MEMpro_Id_BeginSpeedMode);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int LOA_BreakSpeedMode(void)
{
	LOA_si_SaveReadMode = BIG_gi_ReadMode;
	BIG_gi_ReadMode = 0;
	PROPS2_StopRaster(&PROPS2_gst_SpeedMode);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_ResumeSpeedMode(void)
{
	PROPS2_StartRaster(&PROPS2_gst_SpeedMode);
	BIG_gi_ReadMode = LOA_si_SaveReadMode;
	LOA_si_SaveReadMode = -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_EndSpeedMode(void)
{

	eeSND_StartScheduler(LOA_ul_BinKey);

	if(LOA_gb_SpeedMode)
	{
		/* printf("[EE] EndSpeedMode 0x%x\n", LOA_ul_BinKey); */
		MEMpro_StartMemRaster();
		PROPS2_StopRaster(&PROPS2_gst_SpeedMode);

#ifdef ACTIVE_EDITORS
		if(BIG_gp_ReadBuffer && BIG_gi_ReadMode == 1)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			BIGcomp_tdst_CompressParams st_param;
			unsigned int				ui_BuffLen;
#ifdef JADEFUSION
			char* BIG_gp_ReadBuffer_original = BIG_gp_ReadBuffer;
#endif
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			/* TEMP while the sound is not binarized with the new system */
			if(LOA_FetchBuffer(0) != (CHAR *) -1)	/* "Terminate" the previous file */
			{
				/* TEMP BIG_gp_ReadBuffer should be NULL here */
				if(BIG_gp_ReadBuffer) L_free(BIG_gp_ReadBuffer);//¨POPOWARNING
				BIG_gp_ReadBuffer = LOA_GetBinaryBuffer();
				BIG_gi_ReadSize = LOA_GetCurBinaryBuffer() - LOA_GetBinaryBuffer();
				if(BIG_gi_ReadSize < sizeof(LOA_tdstBinFileHeader)) BIG_gi_ReadSize = sizeof(LOA_tdstBinFileHeader);
			}
			if(BIG_gi_ReadSize)
			{
				if(LOA_gb_CompressBin)
				{
#ifdef JADEFUSION
					SAV_Begin("Root/Bin", BIG_NameFile(BIG_gul_ReadFat));
					int pos = 0;	
					ui_BuffLen = BIGcomp_M_BlockSize*2;
					char* workBuf = (char* )L_malloc(ui_BuffLen);
					while(pos < BIG_gi_ReadSize)
					{					
						st_param.in = (u_char*)&BIG_gp_ReadBuffer[pos];
						if (BIGcomp_M_BlockSize > (BIG_gi_ReadSize - pos))
							st_param.uInBufLen = BIG_gi_ReadSize - pos;
						else
							st_param.uInBufLen = BIGcomp_M_BlockSize;						

  						st_param.out = (unsigned char *)workBuf;
#else
					st_param.in = (u_char*)BIG_gp_ReadBuffer;
					st_param.uInBufLen = BIG_gi_ReadSize;

					ui_BuffLen = (BIG_gi_ReadSize / BIGcomp_M_BlockSize);
					ui_BuffLen = BIGcomp_M_CompressBufferSize(BIGcomp_M_BlockSize) *
						ui_BuffLen +
						BIGcomp_M_CompressBufferSize(BIG_gi_ReadSize - (BIGcomp_M_BlockSize * ui_BuffLen));

					st_param.out = (unsigned char *) L_malloc(ui_BuffLen);
#endif
					L_memset(st_param.out, 0, ui_BuffLen);
					st_param.uOutBufLen = ui_BuffLen;

					/* compression */
					ERR_X_Error(BIGcomp_EncodeBlocks(&st_param, BIGcomp_M_BlockSize) != 0, L_ERR_Csz_BIGcomp, NULL);

#ifndef JADEFUSION
					SAV_Begin("Root/Bin", BIG_NameFile(BIG_gul_ReadFat));
#endif
					SAV_Buffer(st_param.out, st_param.uOutBufLen);
#ifdef JADEFUSION
						pos+= BIGcomp_M_BlockSize;
					}
#endif
					SAV_ul_End();
#ifdef JADEFUSION
					L_free(workBuf);
#else
					L_free(st_param.out);
#endif
				}
				else
				{
					SAV_Begin("Root/Bin", BIG_NameFile(BIG_gul_ReadFat));
					SAV_Buffer(BIG_gp_ReadBuffer, BIG_gi_ReadSize);
					SAV_ul_End();
				}
			}


			LOA_ClearBinaryBuffer();
#ifdef JADEFUSION
			BIG_gp_ReadBuffer = BIG_gp_ReadBuffer_original;
#else
			BIG_gp_ReadBuffer = NULL;
#endif
			BIG_gi_ReadMode = 0;
		}
        else
        {
            LOA_ClearBinaryBuffer();
        }
#endif //ACTIVE_EDITORS
		if(BIG_gp_ReadBuffer && BIG_gi_ReadMode == 2)
		{
#if defined(ACTIVE_EDITORS) && defined(JADEFUSION)
			if(BIG_gp_ReadBuffer) L_free(BIG_gp_ReadBuffer);
#else
			if(BIG_gp_ReadBuffer) MEM_FreeFromEndAlign(BIG_gp_ReadBuffer);
#endif
#ifdef ACTIVE_EDITORS
			if(BIG_gp_CompressedBuffer) MEM_FreeFromEndAlign(BIG_gp_CompressedBuffer);
#else
			if(BIG_gp_CompressedBuffer_r) MEM_Free(BIG_gp_CompressedBuffer_r);
#endif
			BIG_gp_CompressedBuffer = BIG_gp_ReadBuffer = NULL;
			BIG_gp_CompressedBuffer_r = NULL;
		}

		BIG_gi_ReadMode = 0;

		MEMpro_StopMemRaster(MEMpro_Id_EndSpeedMode);
	}


	if((LOA_ul_BinKey & 0xFFF00000) == 0xFF800000)
	{
#ifdef _XENON	
		// Switch to the binarized BF containing textures to load
		if ( LOA_gb_SpeedMode )
		{
			g_BinarizedBFManager.SwitchBinarizedBF(eBinarizedBFMaps);
		}
#endif // _XENON
		LOA_StopLoadRaster(LOA_Cte_LDI_SpeedMode_Display);
	}
	else if((LOA_ul_BinKey & 0xFFF00000) == 0xFF400000)
	{
		LOA_ul_BinKey &= ~0x00400000;
		LOA_StopLoadRaster(LOA_Cte_LDI_SpeedMode_Sound);
	}
	else if((LOA_ul_BinKey & 0xFF000000) == 0xFD000000)
    {
        LOA_StopLoadRaster(LOA_Cte_LDI_SpeedMode_Text);
    }
    else if((LOA_ul_BinKey & 0xFF000000) == 0xFE000000)
    {
        LOA_StopLoadRaster(LOA_Cte_LDI_SpeedMode_Sound2);
    }
    
}



/*
 * Aim: Call that function to resolve (load) all the references registered with
 * LOA_MakeFileRef function. This is the function that really makes some load.
 */
#ifdef PSX2_TARGET1
extern void PROPS2_Print(PROPS2_tdst_Raster *);
#endif
#ifdef _DEBUG
ULONG		gul_LastLoaded = 0;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_Resolve(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX		ul_Pos,ulFat;
	LOA_tdst_Ref	*pst_Ref;
	ULONG			ul_Res;
#ifdef ACTIVE_EDITORS
	ULONG			savemem;
	char			asz_Msg[200];
	float			f_StartTimeEditors;
	BAS_tdst_barray *p;
	int				i_MemRef;
#endif // ACTIVE_EDITORS
	extern BOOL		BIG_gb_CanCache;
	extern BAS_tdst_barray WOR_gst_LoadedKeyForCurrentFix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	BIG_gb_CanCache = TRUE;

#ifdef ACTIVE_EDITORS
	f_StartTimeEditors = TIM_f_Clock_TrueRead();
	if(!LOA_gi_InitRef)
	{
		BAS_binit(&LOA_gst_RefArray, 500);
		LOA_gi_InitRef = 1;
	}
#endif// ACTIVE_EDITORS
	_Try_ while((LOA_gi_CurRef % LOA_C_MaxStack) != LOA_gi_LastRef)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		extern void BIG_UpdateCache(void);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		BIG_UpdateCache();

		pst_Ref = &LOA_gast_Stack[LOA_gi_CurRef];
		LOA_gpst_CurRef = pst_Ref;

#ifdef _DEBUG
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			static BIG_KEY	ul_ToDebug = 0x2600c6b3;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(pst_Ref->ul_Key == ul_ToDebug)
			{
				pst_Ref->ul_Key = ul_ToDebug;
			}
		}
#endif //_DEBUG

		/* To mark the file as loaded */
#ifdef ACTIVE_EDITORS
		ulFat = BIG_ul_SearchKeyToFat(pst_Ref->ul_Key);
		if(ulFat == BIG_C_InvalidIndex)
		{
			sprintf(asz_Msg, "A key does not exist in that bigfile !!! (%08x)", pst_Ref->ul_Key);
			ERR_X_Warning(0, asz_Msg, NULL);
#	if !defined(NDEBUG)
			sprintf(asz_Msg, "=> Source file is %s, line %d", pst_Ref->psz_File, pst_Ref->i_NumLine);
			ERR_X_Warning(0, asz_Msg, NULL);
#	endif
			if(pst_Ref->pul_Res) *pst_Ref->pul_Res = NULL;
			LOA_gi_CurRef = (LOA_gi_CurRef + 1) % LOA_C_MaxStack;
			continue;
		}

        LOA_CheckIfInTrashDir(ulFat);

        // ACTIVE_EDITORS
		if(LOA_gb_LogBin)
		{
			/*~~~~~~~~~~~~~~~~~~*/
			char	asz_Name[150];
			char	*psz_Ext;
			ULONG	ul_Size;
			ULONG	ul_Index;
			/*~~~~~~~~~~~~~~~~~~*/

			ul_Size = (LOA_GetCurBinaryBuffer() - LOA_GetBinaryBuffer()) - LOA_gul_BinSize;
			LOA_gul_BinSize += ul_Size;

			if(ul_Size && LOA_gul_CurrentKey)
			{
				ul_Index = BIG_ul_SearchKeyToFat(LOA_gul_CurrentKey);
				L_strcpy(asz_Name, BIG_NameFile(ul_Index));

				psz_Ext = strrchr(asz_Name, '.');

				if(!L_strcmp(psz_Ext, ".gro")) LOA_ul_FileTypeSize[0] += ul_Size;
				if(!L_strcmp(psz_Ext, ".grm")) LOA_ul_FileTypeSize[1] += ul_Size;
				if(!L_strcmp(psz_Ext, ".gao")) LOA_ul_FileTypeSize[2] += ul_Size;
				if(!L_strcmp(psz_Ext, ".trl")) LOA_ul_FileTypeSize[3] += ul_Size;
				if(!L_strcmp(psz_Ext, ".ofc")) LOA_ul_FileTypeSize[4] += ul_Size;
				if(!L_strcmp(psz_Ext, ".omd")) LOA_ul_FileTypeSize[4] += ul_Size;
				if(!L_strcmp(psz_Ext, ".cob")) LOA_ul_FileTypeSize[5] += ul_Size;
				if(!L_strcmp(psz_Ext, ".map")) LOA_ul_FileTypeSize[5] += ul_Size;
				if(!L_strcmp(psz_Ext, ".cin")) LOA_ul_FileTypeSize[5] += ul_Size;
				if(!L_strcmp(psz_Ext, ".act")) LOA_ul_FileTypeSize[6] += ul_Size;
				if(!L_strcmp(psz_Ext, ".ova")) LOA_ul_FileTypeSize[7] += ul_Size;
			}
		}

		LOA_gul_CurrentKey = pst_Ref->ul_Key;

		BIG_FileChanged(ulFat) |= EDI_FHC_Loaded;
		ul_Pos = BIG_PosFile(ulFat);
#else // ACTIVE_EDITORS
		LOA_gul_CurrentKey = pst_Ref->ul_Key;
		ul_Pos = BIG_ul_SearchKeyToPos(pst_Ref->ul_Key);
#endif // ACTIVE_EDITORS
		if((ul_Pos == -1) && (pst_Ref->c_Type & LOA_C_MustExists))
		{
			/*~~~~~~~~~~~~*/
			char	az[100];
			/*~~~~~~~~~~~~*/

			sprintf(az, "Key does not exists (%08x)", pst_Ref->ul_Key);
			ERR_X_Error(0, az, NULL);
		}

		PROPS2_StartRaster(&PROPS2_gst_LOA_Resolve1);
		if((int) ul_Pos != -1)
		{
			/* Reference already loaded, and we want to load it only once */
			ul_Res = LOA_ul_SearchAddress(ul_Pos);
			
			/* we are loading a fix wolrd, and the key is yet loaded */
			if(WOR_gi_IsLoadingFix && (ul_Res != -1))
			{
				/* call the callback in special mode (we have to read the data, but without keeping the result) */
				char log[64];

				if(BAS_bsearch(ul_Pos, &WOR_gst_LoadedKeyForCurrentFix) == -1)
				{
					sprintf(log, "a key is shared by several fix world : %08x", pst_Ref->ul_Key);
					ERR_X_Warning(0, log, NULL);
					WOR_gi_IsLoadingFix++ ;
					pst_Ref->pfnul_CallBack(ul_Pos);
					WOR_gi_IsLoadingFix-- ;

					BAS_binsert(ul_Pos, ul_Pos, &WOR_gst_LoadedKeyForCurrentFix);
				}
			}
			
			/* update the user counter if any */
			if(pst_Ref->c_Type & LOA_C_HasUserCounter)
			{
				if(ul_Res && (ul_Res != -1))
				{
					((int*)ul_Res)[1]++;
				}
			}
			
			/* set a ref to a bad ref if ref for more than one time */
			if(pst_Ref->c_Type & LOA_C_OnlyOneRef)
			{
				if(ul_Res && (ul_Res != -1))
					ul_Res = (ULONG)NULL;
			}

#ifdef ACTIVE_EDITORS
			if(ul_Res == NULL)
			{
				sprintf(asz_Msg, "A key is referencing a null pointer !!! (%08x)", pst_Ref->ul_Key);
				ERR_X_Warning(0, asz_Msg, NULL);
#	if !defined( NDEBUG )
				sprintf(asz_Msg, "=> Source file is %s, line %d", pst_Ref->psz_File, pst_Ref->i_NumLine);
				ERR_X_Warning(0, asz_Msg, NULL);
#	endif
				LOA_gi_CurRef = (LOA_gi_CurRef + 1) % LOA_C_MaxStack;
				continue;
			}
#endif // ACTIVE_EDITORS
			if((int) ul_Res == -1)
			{
#ifdef ACTIVE_EDITORS
				/* Put file at the end of the bigfile ? */
				{
					/*~~~~~~~~~~~~~~~~~~~*/
					static int	endend = 0;
					/*~~~~~~~~~~~~~~~~~~~*/

					if(endend)
					{
						/*~~~~~~~~~~~~~~~~~~~~*/
						char	*p;
						ULONG	ul, pospos, fat;
						/*~~~~~~~~~~~~~~~~~~~~*/

						p = BIG_pc_ReadFileTmp(ul_Pos, &ul);
						L_fseek(BIG_Handle(), 0, L_SEEK_END);
						pospos = L_ftell(BIG_Handle());
						BIG_fwrite(&ul, 4, BIG_Handle());
						BIG_fwrite(p, ul, BIG_Handle());
						fat = BIG_ul_SearchKeyToFat(pst_Ref->ul_Key);
						BIG_PosFile(fat) = pospos;
						BIG_LengthDiskFile(fat) = ul;
						BIG_UpdateOneFileInFat(fat);
						ul_Pos = pospos;
						BAS_binsert(ul_Pos, fat, &BIG_gst.st_PosTableToFat);
					}
				}

				/* Save for references storage */
				p = (BAS_tdst_barray *) BAS_bsearch(pst_Ref->ul_Key, &LOA_gst_RefArray);
				if((int) p == -1)
				{
					p = (BAS_tdst_barray *) L_malloc(sizeof(BAS_tdst_barray));
					BAS_binit(p, 10);
				}

				BAS_binsert(pst_Ref->ul_Key, (ULONG) p, &LOA_gst_RefArray);
				i_MemRef = LOA_gi_LastRef;
				savemem = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
#endif // ACTIVE_EDITORS

				/* Call the callback to load the file */
				PROPS2_StartRaster(&PROPS2_gst_LOA_Resolve2);

                // Log begining of loading
#ifdef ACTIVE_EDITORS
                LOA_LogLoad(pst_Ref);
#endif //ACTIVE_EDITORS

#if defined(PSX2_TARGET1) && !defined(_FINAL_)
				{
					/*~~~~~~~~~~~~~~~~*/
					float	start, stop;
					/*~~~~~~~~~~~~~~~~*/

					start = RealTime();
					ul_Res = pst_Ref->pfnul_CallBack(ul_Pos);
					stop = RealTime();
					LOA_RegisterRaster((ULONG) pst_Ref->pfnul_CallBack, start, stop);
				}

#else // defined(PSX2_TARGET1) && !defined(_FINAL_)
				ul_Res = pst_Ref->pfnul_CallBack(ul_Pos);
#endif // defined(PSX2_TARGET1) && !defined(_FINAL_)

#ifdef ACTIVE_EDITORS
                // End of loading
                LOA_PrintRefToLog();

				if(LOA_gul_CurrentKey)
				{
					/*~~~~~~~~~~~~~~~~~~*/
					char	asz_Name[150];
					char	*psz_Ext;
					ULONG	ul_Index;
					ULONG	ul_Size;
					/*~~~~~~~~~~~~~~~~~~*/

					ul_Index = BIG_ul_SearchKeyToFat(LOA_gul_CurrentKey);
					L_strcpy(asz_Name, BIG_NameFile(ul_Index));

					psz_Ext = strrchr(asz_Name, '.');
					if (psz_Ext)
					{
						ul_Size = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated - savemem;
						if(!L_strcmp(psz_Ext, ".gro")) LOA_ul_FileTypeSize[0] += ul_Size;
						if(!L_strcmp(psz_Ext, ".grm")) LOA_ul_FileTypeSize[1] += ul_Size;
						if(!L_strcmp(psz_Ext, ".gao")) LOA_ul_FileTypeSize[2] += ul_Size;
						if(!L_strcmp(psz_Ext, ".trl")) LOA_ul_FileTypeSize[3] += ul_Size;
						if(!L_strcmp(psz_Ext, ".ofc")) LOA_ul_FileTypeSize[4] += ul_Size;
						if(!L_strcmp(psz_Ext, ".omd")) LOA_ul_FileTypeSize[4] += ul_Size;
						if(!L_strcmp(psz_Ext, ".cob")) LOA_ul_FileTypeSize[5] += ul_Size;
						if(!L_strcmp(psz_Ext, ".map")) LOA_ul_FileTypeSize[5] += ul_Size;
						if(!L_strcmp(psz_Ext, ".cin")) LOA_ul_FileTypeSize[5] += ul_Size;
						if(!L_strcmp(psz_Ext, ".act")) LOA_ul_FileTypeSize[6] += ul_Size;
						if(!L_strcmp(psz_Ext, ".ova")) LOA_ul_FileTypeSize[7] += ul_Size;
					}
				}
#endif // ACTIVE_EDITORS
				PROPS2_StopRaster(&PROPS2_gst_LOA_Resolve2);
#ifdef PSX2_TARGET1
				printf("-- -- 0x%08X", (int) pst_Ref->pfnul_CallBack);
				PROPS2_Print(&PROPS2_gst_LOA_Resolve2);
#endif // PSX2_TARGET1
#ifdef _DEBUG
				gul_LastLoaded = pst_Ref->ul_Key;
#endif // _DEBUG
#ifdef ACTIVE_EDITORS
				while((i_MemRef % LOA_C_MaxStack) != LOA_gi_LastRef)
				{
					BAS_binsert(LOA_gast_Stack[i_MemRef].ul_Key, NULL, p);
					i_MemRef = (i_MemRef + 1) % LOA_C_MaxStack;
				}
#endif // ACTIVE_EDITORS

				/* Add result in array */
				if(!(pst_Ref->c_Type & LOA_C_NotSavePtr))
				{
					if(ul_Pos)
					{
						MEMpro_StartMemRaster();
						LOA_AddPosAddress(ul_Pos, (void *) ul_Res);
						MEMpro_StopMemRaster(MEMpro_Id_LOA_ResolveArray);
					}

#ifdef ACTIVE_EDITORS
					sprintf
					(
						asz_Msg,
						"LOA_Resolve: Trying to add a null address. Object must be invalid (%08x)",
						pst_Ref->ul_Key
					);
					ERR_X_Warning(ul_Res, asz_Msg, NULL);
#endif // ACTIVE_EDITORS
				}
			}

			/* Remember key depending of address */
#ifdef ACTIVE_EDITORS
			if(!(pst_Ref->c_Type & LOA_C_NotSavePtr)) BAS_binsert(ul_Res, pst_Ref->ul_Key, &LOA_gst_ArrayAdr);
#endif // ACTIVE_EDITORS

			/* Save the result in destination address */
			if(pst_Ref->pul_Res) *pst_Ref->pul_Res = ul_Res;						
			
		}

		PROPS2_StopRaster(&PROPS2_gst_LOA_Resolve1);

		/*
		 * Load next reference £
		 * LOA_TakeBetterPos(ul_Pos);
		 */
		LOA_gi_CurRef = (LOA_gi_CurRef + 1) % LOA_C_MaxStack;
	}

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	_Catch_ LOA_gi_CurRef = LOA_gi_LastRef = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	TIM_gf_EditorTime += (TIM_f_Clock_TrueRead() - f_StartTimeEditors);
#endif // ACTIVE_EDITORS
	_EndThrow_
#ifdef ACTIVE_EDITORS
	TIM_gf_EditorTime += (TIM_f_Clock_TrueRead() - f_StartTimeEditors);
#endif // ACTIVE_EDITORS

	/* Free loading buffer */
	BIG_FreeGlobalBuffer();

	BIG_gb_CanCache = FALSE;
}

#ifdef ACTIVE_EDITORS
ULONG gul_addr=0;
/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG LOA_ul_SearchAddress(ULONG _ul_Pos)
{	
	return BAS_bsearch(_ul_Pos, &LOA_gst_Array);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_DeleteAddress(void *_pv_Address)
{
	/*~~*/
	int i;
	/*~~*/

	if(gul_addr == (ULONG)_pv_Address)
	{
		gul_addr = (ULONG)_pv_Address;
	}

	for(i = 0; i < LOA_gst_Array.num; i++)
	{
		if(LOA_gst_Array.base[i].ul_Val == (ULONG) _pv_Address)
		{
			BAS_bdelete((ULONG) LOA_gst_Array.base[i].ul_Key, &LOA_gst_Array);
			break;
		}
	}

	BAS_bdelete((ULONG) _pv_Address, &LOA_gst_ArrayAdr);
}

#else // ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned char LOA_HashPos(ULONG _ul_Pos)
{
	return (unsigned char) ((_ul_Pos & 0xFF) + (_ul_Pos >> 8));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG LOA_ul_SearchAddress(ULONG _ul_Pos)
{
	return BAS_bsearch(_ul_Pos, &LOA_gst_Array[LOA_HashPos(_ul_Pos)]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_DeleteAddress(void *_pv_Address)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		pos;
	extern void BAS_bdeletegetval(ULONG, BAS_tdst_barray *, ULONG *);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	BAS_bdeletegetval((ULONG) _pv_Address, &LOA_gst_InvArray, &pos);
	if(pos) BAS_bdeletegetval(pos, &LOA_gst_Array[LOA_HashPos(pos)], &pos);
}

void LOA_DeleteAddressGetPos(void *_pv_Address, ULONG*pul_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		pos;
	extern void BAS_bdeletegetval(ULONG, BAS_tdst_barray *, ULONG *);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	BAS_bdeletegetval((ULONG) _pv_Address, &LOA_gst_InvArray, &pos);
	*pul_Pos = pos;
	
	if(pos) BAS_bdeletegetval(pos, &LOA_gst_Array[LOA_HashPos(pos)], &pos);
}
#endif // ACTIVE_EDITORS
#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_AddPosAddress(int pos, void *_pv_Address)
{
	if(gul_addr == (ULONG)_pv_Address)
	{
		gul_addr = (ULONG)_pv_Address;
	}

	BAS_binsert(pos, (ULONG) _pv_Address, &LOA_gst_Array);
}

#else// ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_AddPosAddress(int pos, void *_pv_Address)
{
	BAS_binsert(pos, (ULONG) _pv_Address, &LOA_gst_Array[LOA_HashPos(pos)]);
	BAS_binsert((ULONG) _pv_Address, pos, &LOA_gst_InvArray);
}
#endif // ACTIVE_EDITORS
#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_AddAddress(BIG_INDEX _ul_File, void *_pv_Address)
{
#if !defined(XML_CONV_TOOL)
	if(!_pv_Address)
	{
		ERR_X_Warning(0, "LOA_AddAddress: Add a null address", NULL);
		return;
	}
#endif // XML_CONV_TOOL

	BIG_InsertKeyToFat(BIG_FileKey(_ul_File), _ul_File);
	LOA_AddPosAddress(BIG_PosFile(_ul_File), _pv_Address);
	BAS_binsert((ULONG) _pv_Address, BIG_FileKey(_ul_File), &LOA_gst_ArrayAdr);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BIG_KEY LOA_ul_SearchKeyWithAddress(ULONG _ul_Adr)
{
#if !defined(XML_CONV_TOOL)
	if(!_ul_Adr)
	{
		ERR_X_Warning(0, "Search key for a null address", NULL);
		return BIG_C_InvalidKey;
	}

	return(BIG_KEY) BAS_bsearch(_ul_Adr, &LOA_gst_ArrayAdr);
#else
	return _ul_Adr;
#endif // XML_CONV_TOOL
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BIG_KEY LOA_ul_SearchIndexWithAddress(ULONG _ul_Adr)
{
	/*~~~~~~~~~~~~~*/
	BIG_KEY ul_Index;
	/*~~~~~~~~~~~~~*/

	if(!_ul_Adr)
	{
		ERR_X_Warning(0, "Search index for a null address", NULL);
		return BIG_C_InvalidKey;
	}

	ul_Index = LOA_ul_SearchKeyWithAddress(_ul_Adr);
	if(ul_Index == BIG_C_InvalidIndex) return ul_Index;
	return BIG_ul_SearchKeyToFat(ul_Index);
}
#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_BeforeLoading(void)
{
#ifdef ACTIVE_EDITORS
	BAS_binit(&LOA_gst_Array, 10000);
	BAS_binit(&LOA_gst_ArrayAdr, 200);
#else // ACTIVE_EDITORS
	{
		/*~~*/
		int i;
		/*~~*/

		for(i = 0; i < 256; i++) BAS_binit(&LOA_gst_Array[i], 100);
		BAS_binit(&LOA_gst_InvArray, 10000);
	}
#endif // ACTIVE_EDITORS
	LOA_gi_CurRef = LOA_gi_LastRef = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_AfterLoading(void)
{
#ifdef ACTIVE_EDITORS
	BAS_bfree(&LOA_gst_Array);
	BAS_bfree(&LOA_gst_ArrayAdr);
#else // ACTIVE_EDITORS
	{
		/*~~*/
		int i;
		/*~~*/

		for(i = 0; i < 256; i++) BAS_bfree(&LOA_gst_Array[i]);
		BAS_bfree(&LOA_gst_InvArray);
	}
#endif // ACTIVE_EDITORS
	LOA_gi_CurRef = LOA_gi_LastRef = 0;
}

/*
 =======================================================================================================================
    Aim: Allow user to retrieve registered result address with makefileref Note: Have to be called only in a loading
    callback function
 =======================================================================================================================
 */
ULONG *LOA_pul_GetCurrentAddress(void)
{
	return LOA_gast_Stack[LOA_gi_CurRef].pul_Res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BIG_KEY LOA_ul_GetCurrentKey(void)
{
	return LOA_gast_Stack[LOA_gi_CurRef].ul_Key;
}

/*
 =======================================================================================================================
    Aim: Change address associated with a key Note: Have to be called only in a loading callback function
 =======================================================================================================================
 */
void LOA_SetCurrentAddress(ULONG *_pul_Address)
{
	LOA_gast_Stack[LOA_gi_CurRef].pul_Res = _pul_Address;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL LOA_IsSpecial(ULONG _h_Key)
{
	int i;
	

	if(!BIG_gi_ReadMode) return FALSE;
	if(WOR_gi_IsLoadingFix) return TRUE;

	for(i=0;i<LOA_gi_SpecialArrayNb;i++)
	{
		
#ifdef ACTIVE_EDITORS
		// if not binarizing
		if(BIG_gi_ReadMode != 1)
#endif // ACTIVE_EDITORS
		{
			// if this world is not loaded => skip its table
			if(!WOR_gul_FixWorldLoaded[i]) continue;
		}

		// look in all loaded fix map
		if((int) BAS_bsearch(_h_Key, &LOA_gst_SpecialArrayKey[i]) != -1) 
			return TRUE;
	}
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_LoadSpecialArray(void)
{
	/*~~~~~~~~~*/
	int		size;
	ULONG	ulFileKey,ulFixKey, prev;
	int		i;
	L_FILE	file;
	extern void WOR_RegisterFixWorldKey(int _l_Index, ULONG _ul_Key);
	/*~~~~~~~~~*/

	LOA_gi_SpecialArrayNb = 0;
	BAS_binit(&LOA_gst_SpecialArrayAddrToDestroy, 4000);		
	BAS_binit(&LOA_gst_SpecialArrayAddr, 4000);				
	
	for(i=0;i<WOR_Cte_MaxFixWorldNb;i++)
	{
		BAS_binit(&LOA_gst_SpecialArrayKey[i], 256);		
	}
	

	file = M_OpenFile(LOA_SPE_NAME);
	if(!CLI_FileOpen(file)) return;

	ulFileKey = ulFixKey = prev = 0;

	M_GetFileSize(size, file);
	while(size > 0)
	{
		M_ReadFile(file, &ulFileKey, sizeof(ULONG));
		size -= sizeof(ULONG);
		
		prev = ulFixKey;
		M_ReadFile(file, &ulFixKey, sizeof(ULONG));
		size -= sizeof(ULONG);

		// new fix list
		if(!ulFileKey && !ulFixKey) 
		{		
			WOR_RegisterFixWorldKey(LOA_gi_SpecialArrayNb, prev);
			LOA_gi_SpecialArrayNb++;
			continue;
		}

#if defined (_GAMECUBE) || defined (_XENON)
		SwapDWord(&ulFileKey);
		SwapDWord(&ulFixKey);
#endif

		BAS_binsert(ulFileKey, ulFixKey, &LOA_gst_SpecialArrayKey[LOA_gi_SpecialArrayNb]);
	}

	M_CloseFile(file);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_SaveSpecialArray(void)
{
	/*~~~~~~~~~*/
	L_FILE	file;
	int		i,j, zero;
	/*~~~~~~~~~*/

	file = L_fopen(LOA_SPE_NAME, L_fopen_WB);
	if(!CLI_FileOpen(file)) return;

	zero = 0;
	for(j=0; j<LOA_gi_SpecialArrayNb; j++)
	{
		for(i = 0; i < LOA_gst_SpecialArrayKey[j].num; i++)
		{
			L_fwrite(&LOA_gst_SpecialArrayKey[j].base[i].ul_Key, sizeof(ULONG), 1, file);
			L_fwrite(&LOA_gst_SpecialArrayKey[j].base[i].ul_Val, sizeof(ULONG), 1, file);
		}
		// mark the end of the current fix list
		L_fwrite(&zero , sizeof(ULONG), 1, file);
		L_fwrite(&zero , sizeof(ULONG), 1, file);
	}

	L_fclose(file);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_MakeSpecialArrayAddrToDestroy(void)
{
	int i,j;
	BAS_tdst_barray st_Delete;

	BAS_bfree(&LOA_gst_SpecialArrayAddrToDestroy);
	BAS_binit(&LOA_gst_SpecialArrayAddrToDestroy, 1000);

	BAS_binit(&st_Delete, 1000);


	// add all keys from unloaded fix
	for(i=0; i<WOR_Cte_MaxFixWorldNb; i++)
	{
		// skip empty
		if(!WOR_gul_FixWorldToUnload[i]) continue;

		// get all key to delete
		for(j=0; j<LOA_gst_SpecialArrayKey[i].num; j++)
		{
			BAS_binsert(LOA_gst_SpecialArrayKey[i].base[j].ul_Key, LOA_gst_SpecialArrayKey[i].base[j].ul_Key, &st_Delete);
		}
	}

	// now discard all keys we have to keep
	for(i=0; i<WOR_Cte_MaxFixWorldNb; i++)
	{
		// skip not loaded fix
		if(!WOR_gul_FixWorldLoaded[i]) continue;

		for(j=0; j<LOA_gst_SpecialArrayKey[i].num; j++)
		{
			BAS_bdelete(LOA_gst_SpecialArrayKey[i].base[j].ul_Key, &st_Delete);
		}
	}

	// now discard all keys we will load
	for(i=0; i<WOR_Cte_MaxFixWorldNb; i++)
	{
		// skip not loaded fix
		if(!WOR_gul_FixWorldToLoad[i]) continue;

		for(j=0; j<LOA_gst_SpecialArrayKey[i].num; j++)
		{
			BAS_bdelete(LOA_gst_SpecialArrayKey[i].base[j].ul_Key, &st_Delete);
		}
	}

	// now store the address to destroy
	for(i=0; st_Delete.num; i++)
	{
		j = BAS_bsearch(st_Delete.base[i].ul_Key, &LOA_gst_SpecialArrayAddr);
		BAS_binsert(j,j,&LOA_gst_SpecialArrayAddrToDestroy);
	}

	BAS_bfree(&st_Delete);
}

BOOL LOA_b_IsSpecialAddrToDestroy(ULONG ul_Addr)
{
	if(BAS_bsearch(ul_Addr, &LOA_gst_SpecialArrayAddrToDestroy) == -1) return FALSE;
	return TRUE;
}

void LOA_AddAddressToSpecialArray(ULONG _h_Key, ULONG _ul_Adr)
{
	int i;

	for(i=0; i<LOA_gi_SpecialArrayNb; i++)
	{
		// skip this world if it doesn't know the key
		if(BAS_bsearch(_h_Key, &LOA_gst_SpecialArrayKey[i]) == -1) continue;
		
		// add the key <-> address correspondance
		BAS_binsert(_h_Key, _ul_Adr, &LOA_gst_SpecialArrayAddr);
	}
}

void LOA_AddKeyToSpecialArray(ULONG _h_Key)
{
#ifdef JADEFUSION
	extern UINT WOR_gul_WorldKey;
#else
	extern ULONG WOR_gul_WorldKey;
#endif
	
	if(EDI_gi_GenSpe == LOA_gi_SpecialArrayNb + 1)
	{
		LOA_gi_SpecialArrayNb ++;
	}

	BAS_binsert(_h_Key, WOR_gul_WorldKey, &LOA_gst_SpecialArrayKey[EDI_gi_GenSpe-1]);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_KillSpecialArray(void)
{
	int j;
#ifdef JADEFUSION
	for(j=0; j<WOR_Cte_MaxFixWorldNb; j++) 
#else
	for(j=0; j<LOA_gi_SpecialArrayNb; j++) 
#endif
	{
		BAS_bfree(&LOA_gst_SpecialArrayKey[j]);	
	}
	BAS_bfree(&LOA_gst_SpecialArrayAddr);
#ifdef JADEFUSION
	BAS_bfree(&LOA_gst_SpecialArrayAddrToDestroy);
#endif
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    loading duration rasters
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef LOA_DEFINE_LOAD_RASTER
extern float	TIM_f_Clock_TrueRead(void);

#ifdef PSX2_TARGET
extern void		GSP_OutputConsole(char *);
#define LDI_Display GSP_OutputConsole
#else
#define fOoGlobalAcc	1.0f
#define LDI_Display		printf
#endif
float			LOA_gaf_StartTime[LOA_Cte_LDI_Number];
float			LOA_gaf_Duration[LOA_Cte_LDI_Number];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_InitLoadRaster(void)
{
	L_memset(LOA_gaf_StartTime, 0, LOA_Cte_LDI_Number * sizeof(float));
	L_memset(LOA_gaf_Duration, 0, LOA_Cte_LDI_Number * sizeof(float));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
void LOA_DisplayDuration(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				tmp[128];
	extern unsigned int gui_SaveStrip;
	extern unsigned int gui_SaveTriangle;
	extern unsigned int WOR_gul_WorldKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sprintf(tmp, "\n-- Loading Durations %08x ---\n", WOR_gul_WorldKey);
	LDI_Display(tmp);
	
	sprintf(tmp, "WOR-AddWorldList : %3.3f\n", LOA_gaf_Duration[LOA_Cte_LDI_AddWorldList]);
	LDI_Display(tmp);
	
	sprintf(tmp, "(WOR-destruction): %3.3f\n", LOA_gaf_Duration[LOA_Cte_LDI_res3]);
	LDI_Display(tmp);

	LDI_Display("-- bin details ---------\n");
	sprintf(tmp, "Bin Engine       : %3.3f\n", LOA_gaf_Duration[LOA_Cte_LDI_SpeedMode_Engine]);
	LDI_Display(tmp);
	sprintf(tmp, "Bin Text         : %3.3f\n", LOA_gaf_Duration[LOA_Cte_LDI_SpeedMode_Text]);
	LDI_Display(tmp);
	sprintf(tmp, "Bin Display      : %3.3f\n", LOA_gaf_Duration[LOA_Cte_LDI_SpeedMode_Display]);
	LDI_Display(tmp);
	sprintf(tmp, "Bin Sound        : %3.3f\n", LOA_gaf_Duration[LOA_Cte_LDI_SpeedMode_Sound]);
	LDI_Display(tmp);
	sprintf(tmp, "Bin Sound2       : %3.3f\n", LOA_gaf_Duration[LOA_Cte_LDI_SpeedMode_Sound2]);
	LDI_Display(tmp);
	sprintf
	(
		tmp,
		"Bin Total =      : %3.3f\n",
		LOA_gaf_Duration[LOA_Cte_LDI_SpeedMode_Engine] +
			LOA_gaf_Duration[LOA_Cte_LDI_SpeedMode_Text] +
			LOA_gaf_Duration[LOA_Cte_LDI_SpeedMode_Display] +
			LOA_gaf_Duration[LOA_Cte_LDI_SpeedMode_Sound] +
			LOA_gaf_Duration[LOA_Cte_LDI_SpeedMode_Sound2]
	);
	LDI_Display(tmp);

	LDI_Display("-- low level funct -----\n");

	sprintf(tmp, "bin read         : %3.3f\n", LOA_gaf_Duration[LOA_Cte_LDI_BinRead]);
	LDI_Display(tmp);

	sprintf(tmp, "bin seek         : %3.3f\n", LOA_gaf_Duration[LOA_Cte_LDI_BinSeek]);
	LDI_Display(tmp);
	

	sprintf(tmp, "nobin fread      : %3.3f\n", LOA_gaf_Duration[LOA_Cte_LDI_fRead]);
	LDI_Display(tmp);
	

	sprintf(tmp, "res1             : %3.3f\n", LOA_gaf_Duration[LOA_Cte_LDI_res1]);
	LDI_Display(tmp);

	sprintf(tmp, "res4             : %3.3f\n", LOA_gaf_Duration[LOA_Cte_LDI_res4]);
	LDI_Display(tmp);

	sprintf(tmp, "res2             : %3.3f\n", LOA_gaf_Duration[LOA_Cte_LDI_res2]);
	LDI_Display(tmp);

	LDI_Display("------------------------\n");

#if defined(PSX2_TARGET1) && !defined(_FINAL_)
	LOA_PrintCBRaster();
#endif
}
#endif


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
char			LOA_FILELIST[] = "loaded.bin";
BAS_tdst_barray LOA_st_LoadedFileList;
static BOOL		LOA_gb_LoadedFileListExists = FALSE;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_LoadLoadedFileList(void)
{
	/*~~~~~~~~~~~~~*/
	ULONG	ul_Index;
	L_FILE	h_File;
	int		i_size;
	/*~~~~~~~~~~~~~*/

	BAS_binit(&LOA_st_LoadedFileList, 4000);
	h_File = L_fopen(LOA_FILELIST, "rb");
	LOA_gb_LoadedFileListExists = FALSE;

	if(!h_File) return;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    get existing list
	 -------------------------------------------------------------------------------------------------------------------
	 */

	L_fseek(h_File, 0, SEEK_END);
	i_size = L_ftell(h_File);
	L_fseek(h_File, 0, SEEK_SET);

	while(i_size > 0)
	{
		L_fread(&ul_Index, 4, 1, h_File);
		BAS_binsert(ul_Index, 1, &LOA_st_LoadedFileList);
		i_size -= 4;
	}

	L_fclose(h_File);
	LOA_gb_LoadedFileListExists = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_SaveLoadedFileList(void)
{
	/*~~~~~~~~~~~~~*/
	int		i_size;
	ULONG	ul_Index;
	L_FILE	h_File;
	/*~~~~~~~~~~~~~*/

    do { h_File = L_fopen(LOA_FILELIST, "wb"); }
    while(!h_File);

	if(!h_File) return;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    save file
	 -------------------------------------------------------------------------------------------------------------------
	 */

	for(i_size = 0; i_size < LOA_st_LoadedFileList.num; i_size++)
	{
		ul_Index = LOA_st_LoadedFileList.base[i_size].ul_Key;
		L_fwrite(&ul_Index, 4, 1, h_File);
	}

	L_fclose(h_File);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_FreeLoadedFileList(void)
{
	BAS_bfree(&LOA_st_LoadedFileList);
	LOA_gb_LoadedFileListExists = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_UpdateLoadedFileList(void)
{
	/*~~~~~~~~~~~~~*/
	ULONG	ul_Index;
	/*~~~~~~~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    add current list
	 -------------------------------------------------------------------------------------------------------------------
	 */

	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(BIG_FileKey(ul_Index) == BIG_C_InvalidKey) continue;
		if((BIG_FileChanged(ul_Index) & EDI_FHC_Loaded) == 0) continue;
		if(!BIG_FileKey(ul_Index)) continue;

		BAS_binsert(BIG_FileKey(ul_Index), 1, &LOA_st_LoadedFileList);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL LOA_b_FileIsInLoadedList(ULONG _h_Key)
{
	if(!LOA_gb_LoadedFileListExists) return TRUE;
	if((int) BAS_bsearch(_h_Key, &LOA_st_LoadedFileList) != -1) return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_AddRefInLoadedList(void)
{
	LOA_LoadLoadedFileList();
	LOA_UpdateLoadedFileList();
	LOA_SaveLoadedFileList();
	LOA_FreeLoadedFileList();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

char			LOA_gaz_EdiExFileKey[L_MAX_PATH];
BOOL			LOA_gb_EdiExFileKey = FALSE;
BAS_tdst_barray LOA_ga_ExKey;

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void LOA_LoadExFileKey(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	FILE		*f;
	ULONG		i, j;
	static BOOL b_YetLoaded = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(b_YetLoaded) return;
	b_YetLoaded = TRUE;

	if(!LOA_gb_EdiExFileKey) return;

	f = fopen(LOA_gaz_EdiExFileKey, "r+t");
	if(f)
	{
		BAS_binit(&LOA_ga_ExKey, 50);

		/* start at the 1st line */
		fseek(f, 0, SEEK_SET);

		/* read the file line by line */
		while(!feof(f))
		{
			fscanf(f, "%x %x\n", &i, &j); /* oldkey newkey */
			BAS_binsert(i, j, &LOA_ga_ExKey);
		}

		fclose(f);
	}
	else
	{
		LOA_gb_EdiExFileKey = FALSE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG LOA_ul_ExFileKey(ULONG _ul_InKey)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	ul_OutKey;
	/*~~~~~~~~~~~~~~*/

	LOA_LoadExFileKey();

	if(!LOA_gb_EdiExFileKey) return _ul_InKey;

	ul_OutKey = BAS_bsearch(_ul_InKey, &LOA_ga_ExKey);
	if(ul_OutKey == -1) return _ul_InKey;

	return ul_OutKey;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LOA_UnloadExFileKey(void)
{
	if(!LOA_gb_EdiExFileKey) return;
	BAS_bfree(&LOA_ga_ExKey);
	LOA_gb_EdiExFileKey = TRUE;
}
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
