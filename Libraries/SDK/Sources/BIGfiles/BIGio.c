/*$T BIGio.c GC! 1.081 09/09/02 18:05:20 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BIGfiles/BIGio.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGcomp.h"
#include "BASe/MEMory/MEMpro.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"

#ifdef PSX2_TARGET
#include "CDV_Manager.h"
#include "PSX2debug.h"
extern void GSP_OutputConsole(char *);
#else
#ifndef CDV_Cte_SectorSize
#define CDV_Cte_SectorSize	2048
#endif
#if defined(_GAMECUBE) || defined(_XENON)
#include "BIGfiles/LOAding/LOAread.h"
#endif
#define ReadLong(_a)	*(int *) _a
#endif
#ifdef _GAMECUBE
#define max(a, b)	(((a) > (b)) ? (a) : (b))
#define min(a, b)	(((a) < (b)) ? (a) : (b))
#endif

extern void eeRPC_RefreshAsyncStatus(void);

/* only for displaying the loading progression */
volatile float	BIG_gf_DispBinProgress;
unsigned int	BIG_gui_DispBinProgressSize = 0;

#define M_UpdateProgress()\
	do{\
		if(BIG_gui_DispBinProgressSize) \
			BIG_gf_DispBinProgress = fLongToFloat(BIG_gi_RealSeek) / fLongToFloat(BIG_gui_DispBinProgressSize);\
	}\
	while(0);


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

int			BIG_gi_ReadMode = 0;				/* 0 normal, 1 write, 2 read buffer */
int			BIG_gi_SpecialRead = 0;
char		*BIG_gp_ReadBuffer = NULL;			/* Temps buffer for special read/write mode - ReadBuffer - */
#if defined(ACTIVE_EDITORS) && defined(JADEFUSION)
const int   READ_BUFFER_SIZE = 512*1024*1024;
#endif
int			BIG_gi_ReadSize = 0;				/* Size of ReadBuffer */
ULONG		BIG_gul_ReadPos = 0;
int			BIG_gi_ReadSeek = 0;				/* Current ReadBuffer seek */

int			BIG_gi_RealSeek = 0;				/* Current file seek */
ULONG		BIG_gul_ReadFat = -1;

char		*BIG_gp_CompressedBuffer = NULL;	/* buffer cantents = compressed data */
char		*BIG_gp_CompressedBuffer_r;
static int	BIG_si_CompressedSize = 0;			/* size of the buffer */
static char *BIG_sp_CompressedNextBlock = NULL; /* next decompression addr */
static int	BIG_si_DecompressedSize = 0;

static int	BIG_si_BinFileSize = 0;
static int	BIG_si_BinFileReadSize = 0;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef PSX2_TARGET
int				gi_PreLoadFinished = 1;
#else
volatile int	gi_PreLoadFinished = 1;
#endif
int				*gpi_PreLoadBuf = NULL;
int				*gpi_PreLoadBufPtrFree = NULL;
int				*gap_PreLoadArray[PRELOAD_MAXTAB];
int				*gap_PreLoadArrayPtrFree[PRELOAD_MAXTAB];
int				gai_PreLoadArrayOk[PRELOAD_MAXTAB];
int				gai_PreLoadArrayFree[PRELOAD_MAXTAB];
int				gi_PreLoadNum = 0;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#if defined(PSX2_USE_iopCDV)
#define CDV_i_DirectSeekFile(_h, _o, _p) \
	{ \
		LOA_StartLoadRaster_r(LOA_Cte_LDI_BinSeek); \
		eeCDV_i_DirectSeekFile(_p); \
		LOA_StopLoadRaster_r(LOA_Cte_LDI_BinSeek); \
	}
#define CDV_i_DirectReadFile(_h, _pp, _s) \
	{ \
		LOA_StartLoadRaster_r(LOA_Cte_LDI_BinRead); \
		eeCDV_i_DirectReadFile(_h, _pp, _s); \
		LOA_StopLoadRaster_r(LOA_Cte_LDI_BinRead); \
	}
#else
#define CDV_i_DirectSeekFile(_h, _o, _p) \
	{ \
		LOA_StartLoadRaster_r(LOA_Cte_LDI_BinSeek); \
		BIG_fseek(_h, _p, _o); \
		LOA_StopLoadRaster_r(LOA_Cte_LDI_BinSeek); \
	}
#if defined(_XENON)
#define CDV_i_DirectReadFile(_h, _pp, _s) \
	{ \
		int iBytesRead; \
		LOA_StartLoadRaster_r(LOA_Cte_LDI_BinRead); \
		ReadFile(_h, *_pp, _s, (LPDWORD)&iBytesRead, 0); \
		LOA_StopLoadRaster_r(LOA_Cte_LDI_BinRead); \
	}
#elif defined(_XBOX)
#define CDV_i_DirectReadFile(_h, _pp, _s) \
	{ \
		DWORD iBytesRead; \
		LOA_StartLoadRaster_r(LOA_Cte_LDI_BinRead); \
		XBCompositeFile_Read(_h, *_pp, _s, &iBytesRead, 0); \
		LOA_StopLoadRaster_r(LOA_Cte_LDI_BinRead); \
	}
#else /* _XBOX */
#define CDV_i_DirectReadFile(_h, _pp, _s) \
	{ \
		LOA_StartLoadRaster_r(LOA_Cte_LDI_BinRead); \
		L_freadA(*_pp, _s, 1, _h); \
		LOA_StopLoadRaster_r(LOA_Cte_LDI_BinRead); \
	}
#endif /* _XBOX */
#endif

/*$4
 ***********************************************************************************************************************
    file access statistics
 ***********************************************************************************************************************
 */

#ifdef BIGio_M_ActiveRasters
#define M_RasterNameSize	10

/*
 -----------------------------------------------------------------------------------------------------------------------
    private types
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	BIGio_tdst_Rasters_
{
	unsigned int	ui_Total;
	unsigned int	ui_Min;
	unsigned int	ui_Max;
	unsigned int	ui_Current;
	unsigned int	ui_Nb;
	char			asz_Name[M_RasterNameSize];
} BIGio_tdst_Rasters;

typedef enum				BIGio_tden_RastersId_
{
	e_IdFirst = 0,
	e_IdFunction_fseek,
	e_IdFunction_fread,
	e_IdLast
} BIGio_tden_RastersId;

/* private variables */
static BIGio_tdst_Rasters	BIGio_sx_Rasters[e_IdLast];

/*
 =======================================================================================================================
    private functions
 =======================================================================================================================
 */
static _inline_ void BIGio_s_UpdateStat(BIGio_tden_RastersId _id, unsigned int _value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIGio_tdst_Rasters	*pst_stat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_stat = &BIGio_sx_Rasters[_id];

	pst_stat->ui_Nb++;
	pst_stat->ui_Total += _value;
	pst_stat->ui_Current = _value;
	pst_stat->ui_Max = max(pst_stat->ui_Max, _value);
	pst_stat->ui_Min = min(pst_stat->ui_Min, _value);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static _inline_ void BIGio_s_NameRaster(BIGio_tden_RastersId _id, char *_asz)
{
	L_memcpy(BIGio_sx_Rasters[_id].asz_Name, _asz, M_RasterNameSize);
	BIGio_sx_Rasters[_id].asz_Name[M_RasterNameSize - 1] = '\0';
}

/*
 =======================================================================================================================
    public functions
 =======================================================================================================================
 */
void BIGio_InitStat(void)
{
	/*~~*/
	int i;
	/*~~*/

	L_memset(BIGio_sx_Rasters, 0, sizeof(BIGio_tdst_Rasters) * e_IdLast);
	for(i = e_IdFirst + 1; i < e_IdLast; i++) BIGio_sx_Rasters[i].ui_Min = (unsigned int) - 1;

	/* names rasters */
	BIGio_s_NameRaster(e_IdFunction_fseek, "fseek");
	BIGio_s_NameRaster(e_IdFunction_fread, "fread");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIGio_PrintRasters(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIGio_tdst_Rasters	*pst_Raster, *pst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Raster = &BIGio_sx_Rasters[e_IdFirst + 1];
	pst_Last = &BIGio_sx_Rasters[e_IdLast];
	for(; pst_Raster < pst_Last; pst_Raster++)
	{
		printf
		(
			"-- %s: %d [%d/%d/%d] %d (%d)\n",
			pst_Raster->asz_Name,
			pst_Raster->ui_Current,
			pst_Raster->ui_Min,
			pst_Raster->ui_Total / pst_Raster->ui_Nb,
			pst_Raster->ui_Max,
			pst_Raster->ui_Total,
			pst_Raster->ui_Nb
		);
	}
}

#else /* BIGio_M_ActiveRasters */
#define BIGio_s_UpdateStat(a, b)
#endif /* BIGio_M_ActiveRasters */

/*$4
 ***********************************************************************************************************************
    file access functions
 ***********************************************************************************************************************
 */
#ifndef PSX2_TARGET
unsigned int BIG_ftell(BIGFileHandle _hfile)
{
#if defined( _XBOX ) && !defined(_XENON)
    LARGE_INTEGER li;
    LARGE_INTEGER position;

    li.QuadPart = 0;

    XBCompositeFile_SetFilePointerEx( _hfile, li, &position, FILE_CURRENT );
    ERR_X_Assert( position.HighPart == 0 );

    return position.LowPart;
#else
	fpos_t pos;

	fgetpos((FILE*)_hfile, &pos);
	return (unsigned int) pos;
#endif
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int BIG_fseek(BIGFileHandle _hfile, unsigned int _offset, int _origin)
{
#if defined( PSX2_TARGET )
#  if defined(PSX2_USE_iopCDV)
	return eeCDV_i_SeekFile(_hfile, _origin, _offset);

	/*~~*/
#  else
	int i;
	/*~~*/

	i = sceLseek(_hfile, _offset, _origin);
	if(i < 0)
		return -1;
	else
		return 0;

#  endif
#elif defined(_GAMECUBE)
	return L_fseek(_hfile, _offset, _origin);
#elif defined(_XENON)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		LONG	lDistanceToMoveHigh;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		lDistanceToMoveHigh = 0;

		/*
		 * high order part of the distance to move the file pointer (high order 32 bits of
		 * the 64 bits)
		 */
		lDistanceToMoveHigh = (int)((__int64)_offset >> 32);
		_offset &= 0xFFFFFFFF;

		if(SetFilePointer(_hfile, _offset, &lDistanceToMoveHigh, _origin) != INVALID_SET_FILE_POINTER)
			return 0;
		else
			return(-1);
	}
#elif defined( _XBOX )
    {
        LARGE_INTEGER li;

        li.QuadPart = _offset;

        if( XBCompositeFile_SetFilePointerEx( _hfile, li, NULL, _origin ) )
        {
            return 0;
        }
        else
        {
			return -1;
        }
    }
#else
	{
		fpos_t	pos;
		if(_origin == L_SEEK_SET)
		{
			pos = (fpos_t)_offset;
			return fsetpos(_hfile, &pos);
		}
		else if(_origin == L_SEEK_CUR)
		{
			fgetpos(_hfile, &pos);
			pos += (fpos_t)_offset;
			return fsetpos(_hfile, &pos);
		}
		else
		{
			return fseek(_hfile, _offset, _origin);
		}
	}
#endif
}

#if defined(_GAMECUBE) || defined(_XENON)
int				BIG_SpeedMode_fread(void **, int, L_FILE);
extern BOOL		LOA_gb_CompressBin;
extern int		gi_TestBin;
extern int		gi_CurBin;
extern int		gai_CurSize[];
extern int		PreLoadTex_Finished(int *, int);
extern void		PreloadCancel(void);
extern char		*gap_FirstPointer[];
extern char		*MEM_gp_MaxBlock;
extern ULONG	LOA_ul_BinKey;

/*
 =======================================================================================================================
    Only GC::BIG_fread for bin files, not compressed, and without preload
 =======================================================================================================================
 */
int BIG_fread_UncompressedBin(void **_p_Buffer, int _i_Size, L_FILE _h_Handle)
{
	/*~~~~~~~~~*/
	char	*tmp;
	/*~~~~~~~~~*/

	/*$2- first time -------------------------------------------------------------------------------------------------*/
	if(!BIG_gp_ReadBuffer)
	{

		/*$1- alloc read buffer space ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if defined(ACTIVE_EDITORS) && defined(JADEFUSION)
		tmp = BIG_gp_ReadBuffer = (char* )L_malloc(READ_BUFFER_SIZE); 
#else
		tmp = BIG_gp_ReadBuffer = (char *) MEM_p_AllocTmp(BIG_gi_ReadSize);
#endif

		/*$1- read the bin file size (in BIG_gp_ReadBuffer only for tmp use) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		CDV_i_DirectSeekFile(_h_Handle, L_SEEK_SET, BIG_gul_ReadPos);
		CDV_i_DirectReadFile(_h_Handle, &BIG_gp_ReadBuffer, CDV_Cte_SectorSize);
		BIG_si_BinFileSize = ReadLong(BIG_gp_ReadBuffer);
		SwapDWord((LONG *) &BIG_si_BinFileSize);
		BIG_gui_DispBinProgressSize = BIG_si_BinFileSize;

		/*$1- restore read buffer pointer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		BIG_gp_ReadBuffer = tmp;
		BIG_gi_RealSeek = 0;
		BIG_gf_DispBinProgress = 0.0f;		
	}

	/*$2- big size read (never) --------------------------------------------------------------------------------------*/

	if(_i_Size > BIG_gi_ReadSize)
	{
		BIG_gi_ReadSize = _i_Size;
		MEM_Free(BIG_gp_ReadBuffer);
		BIG_gp_ReadBuffer = (char *) MEM_p_AllocTmp(BIG_gi_ReadSize);
	}

	/*$2- check size -------------------------------------------------------------------------------------------------*/

	if(_i_Size + BIG_gi_RealSeek > BIG_si_BinFileSize)
	{
#if defined(_XENON)
        _breakpoint_;
#else
		OSReport("*** Try to acces out of bin file ***\n");
#endif
	}

	/*$2- read -------------------------------------------------------------------------------------------------------*/

	CDV_i_DirectSeekFile(_h_Handle, L_SEEK_SET, BIG_gul_ReadPos + 4 + BIG_gi_RealSeek);
	CDV_i_DirectReadFile(_h_Handle, &BIG_gp_ReadBuffer, _i_Size);
	BIG_gi_RealSeek += _i_Size;

	M_UpdateProgress();
	
	/*$2- assign pointer ---------------------------------------------------------------------------------------------*/

	*_p_Buffer = BIG_gp_ReadBuffer;

	return 1;
}

#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define PRELOADTEXGRAN (40 * 1024)

char	*BIG_CachePointers[200];
ULONG   BIG_CachePos[200];
int		BIG_i_CacheNumPointers = 0;
ULONG	BIG_ul_CachePos = 0;
ULONG	BIG_ul_CacheNum = 0;
extern int PreLoadTex_Finished(int *, int);
extern void PreLoadTex_Load(ULONG, ULONG, void *);
extern int AllPreloadDone(void);
volatile int BIG_gi_AsyncReadIsFinished=1;
extern BIG_KEY LOA_ul_BinKey;
extern int gai_CurBinFileSize[];
extern unsigned int gaui_CurBinFilePos[];
/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
#ifdef PSX2_TARGET
extern int eeRPC_i_LastExecAsyncReadIsFinished(void);
int eeRPC_i_ExecAsyncRead(int _i_File, unsigned int _ui_Pos, char *_pc_Buff, int _i_Size);
int eeRPC_AsyncReadRq = -1;
int gi_SpecialHandler2=-1;
extern int          PS2_gi_SpecialDebug;

void AsyncRead(ULONG _ul_Pos, ULONG _ul_Size, void *p_Buf)
{
    if(gi_SpecialHandler2 < 0) return;
	eeRPC_AsyncReadRq = eeRPC_i_ExecAsyncRead(gi_SpecialHandler2, _ul_Pos,p_Buf, _ul_Size);

    /*if(eeRPC_AsyncReadRq < 0)
    {
        while(PS2_gi_SpecialDebug)
        {
    	    extern void GSP_DisplayMessageFullScreen(char*);
            GSP_DisplayMessageFullScreen("Disc Error (3), fatal !");
        }
    }*/
}
#endif
 

void BIG_UpdateCache(void)
{
#if 0
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern char *GS_RAMBUFFER1[];
	extern u_long128 	GSP_GEO_Cache_XYZW[];
	extern u_long 	GSP_GEO_Cache_UV[];
	char *pbufram;
	extern int			eeRPC_PreloadTextRq;
	extern int			ps2SND_gi_Loading;
    int id;
    extern int TEXT_gi_ChangingLang;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	FlushCache(0);
	
    if(TEXT_gi_ChangingLang) return;
	if(!BIG_ul_CachePos) return;
	if(AllPreloadDone()) return;
	if(!eeRPC_i_LastExecAsyncReadIsFinished()) return;
    
	if(BIG_i_CacheNumPointers == 200) return;
   
	/* On choisit un nouveau pointeur */
	pbufram = (char *) GS_RAMBUFFER1;
    *((u_int *)&(pbufram)) |= 0x30000000;
    pbufram += 1024 * 1024;

	switch(BIG_ul_CacheNum)
	{
	
	case 0:		BIG_CachePointers[BIG_i_CacheNumPointers] = pbufram - PRELOADTEXGRAN; break;
	case 1:		BIG_CachePointers[BIG_i_CacheNumPointers] = pbufram - 2 * PRELOADTEXGRAN; break;
	
	case 2:		BIG_CachePointers[BIG_i_CacheNumPointers] = (char *) AI_gast_GlobalVarStack; break;
	case 3:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) AI_gast_GlobalVarStack) + PRELOADTEXGRAN; break;
	
	case 4:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_XYZW) + (0*PRELOADTEXGRAN); break;
	case 5:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_XYZW) + (1*PRELOADTEXGRAN); break;
	case 6:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_XYZW) + (2*PRELOADTEXGRAN); break;
	case 7:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_XYZW) + (3*PRELOADTEXGRAN); break;
	case 8:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_XYZW) + (4*PRELOADTEXGRAN); break;
	case 9:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_XYZW) + (5*PRELOADTEXGRAN); break;
	case 10:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_XYZW) + (6*PRELOADTEXGRAN); break;
	case 11:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_XYZW) + (7*PRELOADTEXGRAN); break;
	case 12:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_XYZW) + (8*PRELOADTEXGRAN); break;
	case 13:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_XYZW) + (9*PRELOADTEXGRAN); break;
	case 14:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_XYZW) + (10*PRELOADTEXGRAN); break;
	case 15:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_XYZW) + (11*PRELOADTEXGRAN); break;
	case 16:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_XYZW) + (12*PRELOADTEXGRAN); break;
	case 17:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_XYZW) + (13*PRELOADTEXGRAN); break;
	
	case 18:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_UV) + (0*PRELOADTEXGRAN); break;
	case 19:		BIG_CachePointers[BIG_i_CacheNumPointers] = ((char *) GSP_GEO_Cache_UV) + (1*PRELOADTEXGRAN); break;
	
	default:	return;
	}

	BIG_ul_CacheNum++;
	BIG_CachePos[BIG_i_CacheNumPointers] = BIG_ul_CachePos;
	
    *((u_int *)&BIG_CachePointers[BIG_i_CacheNumPointers]) |= 0x30000000;


    switch(LOA_ul_BinKey & 0xFF000000)
    {
        case 0xFD000000:id=2;break;
        case 0xFE000000:id=3;break;
        case 0xFF000000:
            switch(LOA_ul_BinKey & 0xFFF00000)
            {
                case 0xFF000000:id=0;break;
                case 0xFF400000:id=4;break;
                case 0xFF800000:id=1;break;
                default:return;
            }
            break;
       default:return;
    }
        
    if(gai_CurBinFileSize[id] && gaui_CurBinFilePos[id])
    {
        if((BIG_ul_CachePos) > (gaui_CurBinFilePos[id] + gai_CurBinFileSize[id]))
        {
            return;
        }         
    }   

	/* On lance le chargement */
	AsyncRead(BIG_ul_CachePos, PRELOADTEXGRAN, BIG_CachePointers[BIG_i_CacheNumPointers]);
	
	if(eeRPC_AsyncReadRq != -1)
	{
	    BIG_i_CacheNumPointers++;
	    BIG_ul_CachePos += PRELOADTEXGRAN;
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_ResetCache(void)
{
	BIG_i_CacheNumPointers = 0;
	BIG_ul_CachePos = 0;
	BIG_ul_CacheNum = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void BIG_CacheRead(BIGFileHandle _h_Handle, ULONG pos, void **pdest, int size)
{
#ifdef PSX2_TARGET
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		i;
	int		dec;
	char	*pc_dest;
	char	*pc_destorg;
	int		realsize;
	int ttt=0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	while(!eeRPC_i_LastExecAsyncReadIsFinished()) {};

	FlushCache(0);

	pc_dest = (char *) *pdest;
	for(i = 0; i < BIG_i_CacheNumPointers; i++)
	{
		if((pos >= BIG_CachePos[i]) && (pos < BIG_CachePos[i] + PRELOADTEXGRAN))
		{
			dec = pos - BIG_CachePos[i];

			realsize = PRELOADTEXGRAN - dec > size ? size : PRELOADTEXGRAN - dec;
			L_memcpy(pc_dest, BIG_CachePointers[i] + dec, realsize);
			size -= realsize;
			if(!size) 
			{
                //printf("cache fini %d\n", ttt);
            	BIG_ul_CacheNum=0;
            	BIG_i_CacheNumPointers = 0;
            	BIG_ul_CachePos = pos + realsize - CDV_Cte_SectorSize;
    			return;
    		}

			pc_dest += realsize;
			pos += realsize;
			
			ttt += realsize;
		}
	}

    //printf("cache %d uncache %d\n", ttt, size);

	/* set pos */
	CDV_i_DirectSeekFile(_h_Handle, L_SEEK_SET, pos);

	/* update read size and read */
	pc_destorg = pc_dest;
	CDV_i_DirectReadFile(_h_Handle, &pc_dest, size);

	/* decalage si besoin */
	if(pc_destorg != pc_dest) L_memmove(pc_destorg, pc_dest, size);
	
	BIG_ul_CacheNum=0;
	BIG_i_CacheNumPointers = 0;
	BIG_ul_CachePos = pos + size - CDV_Cte_SectorSize;
#else
	CDV_i_DirectSeekFile(_h_Handle, L_SEEK_SET, pos);
	CDV_i_DirectReadFile(_h_Handle, pdest, size);
#endif
}


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */


/*
 =======================================================================================================================
    Aim:    Low level function to read some bytes from a bigfile.

    In:     _p_Buffer Buffer to read to. _i_Size Number of bytes to read. _h_Handle Handle of bigfile (C standard).

    Out:    Returns the real number of bytes read.
 =======================================================================================================================
 */
int BIG_SpeedMode_fread(void **_p_Buffer, int _i_Size, BIGFileHandle _h_Handle)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				preload;
	int				initsize, totalsize;
	int				i_Finished;
	int				ioffsetbuf;
	extern int		gi_TestBin;
	extern int		gi_CurBin;
	extern int		gai_CurSize[];
	extern int		PreLoadTex_Finished(int *, int);
	extern void		PreloadCancel(void);
	extern char		*gap_FirstPointer[];
	extern char		*MEM_gp_MaxBlock;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define PRELOADTEXGRAN	(40 * 1024)

	PROPS2_StartRaster(&PROPS2_gst_BIG_fread_mode2);

#ifdef _GAMECUBE
	if(!LOA_gb_CompressBin) return BIG_fread_UncompressedBin(_p_Buffer, _i_Size, _h_Handle);
#endif
	preload = 0;
	ioffsetbuf = 0;

	if(!BIG_gp_ReadBuffer)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		BIGcomp_tdst_DecompressParams	st_Decomp;
		char							*tmp;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef _GAMECUBE
		tmp = BIG_gp_ReadBuffer = (char *) MEM_p_AllocTmp(BIG_gi_ReadSize);
#else
#if defined(ACTIVE_EDITORS) && defined(JADEFUSION)
		tmp = BIG_gp_ReadBuffer = (char *) L_malloc(READ_BUFFER_SIZE);
#else
		tmp = BIG_gp_ReadBuffer = (char *) MEM_p_AllocFromEndAlign(BIG_gi_ReadSize, 64);
#endif
#endif

		/* Preload ? */
		i_Finished = 0;
#if !defined(MAX_PLUGIN)
		PreLoadTex_Finished(&i_Finished, 1);
		if(!i_Finished) PreloadCancel();
#endif // !defined(MAX_PLUGIN)

		/* Preload */
#if !defined(MAX_PLUGIN)
		if(gi_TestBin != -1 && gi_TestBin <= gi_CurBin && gai_CurSize[gi_TestBin])
		{
			if(gap_FirstPointer[gi_TestBin] - gai_CurSize[gi_TestBin] > MEM_gp_MaxBlock)
			{
				/*~~~~~~~~*/
				LONG	*pp;
				/*~~~~~~~~*/

				pp = (LONG *) (gap_FirstPointer[gi_TestBin] - PRELOADTEXGRAN);
#ifdef PSX2_TARGET
				if((LOA_ul_BinKey & 0xFF400000) == 0xFF400000)
					initsize = BIG_si_BinFileSize = max(10 * 1024, _i_Size);
				else
#endif
					initsize = BIG_si_BinFileSize = *pp;

				BIG_gui_DispBinProgressSize = BIG_si_BinFileSize;
				ioffsetbuf = gai_CurSize[gi_TestBin];

#if defined(PSX2_TARGET) && !defined(_FINAL_)
				{
					/*~~~~~~~~~~~~*/
					char	tmp[64];
					/*~~~~~~~~~~~~*/

					sprintf(tmp, "-- Preload %d Ko\n", ioffsetbuf / 1024);
					GSP_OutputConsole(tmp);
				}

#endif
				preload = 1;
				goto zappreload;
			}
		}
#endif // !defined(MAX_PLUGIN)

		/* read the bin file size (in BIG_gp_ReadBuffer only for tmp use) */
		CDV_i_DirectSeekFile(_h_Handle, L_SEEK_SET, BIG_gul_ReadPos);
		CDV_i_DirectReadFile(_h_Handle, &BIG_gp_ReadBuffer, CDV_Cte_SectorSize);

#ifdef PSX2_TARGET
		if((LOA_ul_BinKey & 0xFF400000) == 0xFF400000)
			BIG_si_BinFileSize = max(10 * 1024, _i_Size);
		else
#endif
			BIG_si_BinFileSize = ReadLong(BIG_gp_ReadBuffer);
		
#if defined(_GAMECUBE) || defined(_XENON)
		SwapDWord((LONG *) &BIG_si_BinFileSize);
#endif /* #ifdef _GAMECUBE */
		BIG_gui_DispBinProgressSize = BIG_si_BinFileSize;

		BIG_gp_ReadBuffer = tmp;

zappreload:
		BIG_si_BinFileReadSize = min((int) BIGcomp_M_CompressedSize, BIG_si_BinFileSize);
		BIG_si_CompressedSize = BIG_si_BinFileReadSize;

		/*
		 * now we can alloc optimized size £
		 * but cause to MEM module alloc always the same size
		 */
		BIG_gp_CompressedBuffer_r = BIG_gp_CompressedBuffer = (char *) MEM_p_AllocTmp(BIGcomp_M_CompressedSize + (3 * CDV_Cte_SectorSize));

		/* set position at start of bin file */
		if((!preload) || (ioffsetbuf < BIG_si_BinFileReadSize))
		{
			/* read the 1st buff */
			CDV_i_DirectSeekFile(_h_Handle, L_SEEK_SET, BIG_gul_ReadPos + (unsigned int) ioffsetbuf);

			BIG_gp_CompressedBuffer += ioffsetbuf;
			CDV_i_DirectReadFile(_h_Handle, &BIG_gp_CompressedBuffer, BIG_si_CompressedSize - ioffsetbuf);
			BIG_gp_CompressedBuffer -= ioffsetbuf;
		}

		BIG_si_BinFileSize = max(BIG_si_BinFileSize - BIG_si_BinFileReadSize, 0);

		/* Copie all preloaded buffers */
		if(preload)
		{
		    int ij;
			totalsize = 0;
			for(totalsize = 0; totalsize < gai_CurSize[gi_TestBin]; totalsize += PRELOADTEXGRAN)
			{
				L_memcpy
				(
					BIG_gp_CompressedBuffer + totalsize,
					gap_FirstPointer[gi_TestBin] - totalsize - PRELOADTEXGRAN,
					PRELOADTEXGRAN
				);
			}
			
			
			totalsize -= PRELOADTEXGRAN;
			for(ij=0; ij < gi_CurBin; ij++)
			{
			    if(gap_FirstPointer[ij] < gap_FirstPointer[gi_TestBin])
			    {
			        L_memmove(gap_FirstPointer[ij]+totalsize-gai_CurSize[ij], gap_FirstPointer[ij]-gai_CurSize[ij], gai_CurSize[ij]);
			        gap_FirstPointer[ij] += totalsize;
			    }
			}
		}

		BIG_si_BinFileReadSize = min(BIG_si_BinFileSize, (int) BIGcomp_M_CompressedSize);
		BIG_gi_ReadSeek = 0;
		st_Decomp.bWait = 0;

		/* first word is the bin file size, so we jump it */
		st_Decomp.in = (u_char *) (BIG_gp_CompressedBuffer + 4);
		st_Decomp.out = (u_char *) (BIG_gp_ReadBuffer);
		BIG_sp_CompressedNextBlock = (char *) BIGcomp_DecodeBlocks(&st_Decomp, 1);

		BIG_gi_RealSeek = BIG_sp_CompressedNextBlock - BIG_gp_CompressedBuffer;
		M_UpdateProgress();

		BIG_si_DecompressedSize = st_Decomp.uUncompressedLen;

		/* On en est la du chargement */
		BIG_ResetCache();
   		BIG_ul_CachePos = BIG_gul_ReadPos + BIG_si_CompressedSize - CDV_Cte_SectorSize;
	}
	else if(BIG_gi_ReadSeek + _i_Size > BIG_si_DecompressedSize)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		BIGcomp_tdst_DecompressParams	st_Decomp;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* first read the end of decompressed buffer to free it */
		BIG_si_DecompressedSize = BIG_si_DecompressedSize - BIG_gi_ReadSeek;
		if(BIG_si_DecompressedSize)
			L_memmove(BIG_gp_ReadBuffer, BIG_gp_ReadBuffer + BIG_gi_ReadSeek, BIG_si_DecompressedSize);
		BIG_gi_ReadSeek = 0;

		/* check decompressed buffer length */
		if(_i_Size > BIG_gi_ReadSize)
		{
			/*~~~~~~~~~*/
			char	*tmp;
			/*~~~~~~~~~*/
#ifndef _GAMECUBE
#if defined(ACTIVE_EDITORS) && defined(JADEFUSION)
			L_free(BIG_gp_ReadBuffer);
#else
			MEM_FreeFromEndAlign(BIG_gp_ReadBuffer);
#endif
#endif
			BIG_gi_ReadSize = _i_Size;
#ifdef _GAMECUBE
			tmp = (char *) MEM_p_AllocTmp(BIG_gi_ReadSize);
#else			
			tmp = (char *) MEM_p_AllocFromEndAlign(BIG_gi_ReadSize, 64);
#endif			
			L_memmove(tmp, BIG_gp_ReadBuffer, BIG_si_DecompressedSize);
#ifdef _GAMECUBE
			MEM_Free(BIG_gp_ReadBuffer);
#endif
			BIG_gp_ReadBuffer = tmp;
			BIG_gi_ReadSeek = 0;
		}

		/* then decompressed others blocks */
		do
		{
			/* check if compressed buffer contents the whole next block */
			if
			(
				!BIGcomp_CheckBlock
				(
					(u_char *) BIG_sp_CompressedNextBlock,
					BIG_si_CompressedSize - (BIG_sp_CompressedNextBlock - BIG_gp_CompressedBuffer)
				)
			)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
				int saved_size, remain, rest;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

				/*
				 * we have to reload the compressed buffer but without losting the data yet loaded £
				 * rounded down to 1 sect
				 */
				saved_size = (BIG_si_CompressedSize - (BIG_sp_CompressedNextBlock - BIG_gp_CompressedBuffer)) / CDV_Cte_SectorSize;
				saved_size *= CDV_Cte_SectorSize;
				rest = (BIG_si_CompressedSize - (BIG_sp_CompressedNextBlock - BIG_gp_CompressedBuffer)) - saved_size;

				/* remain to load */
				remain = BIGcomp_i_CompressedSize((u_char *) BIG_sp_CompressedNextBlock) - saved_size;

				/* save */
				L_memcpy(BIG_gp_CompressedBuffer_r, BIG_sp_CompressedNextBlock, saved_size);

				/* update the buff ptr */
				BIG_gp_CompressedBuffer = BIG_gp_CompressedBuffer_r + saved_size;

				/* update the remain file size (rounded up to 1 sector) */
				BIG_si_BinFileSize += rest;
				BIG_si_BinFileSize = (BIG_si_BinFileSize + CDV_Cte_SectorSize - 1) / CDV_Cte_SectorSize;
				BIG_si_BinFileSize *= CDV_Cte_SectorSize;

				/* reeval the read file size */
				BIG_si_BinFileReadSize = min(BIG_si_BinFileSize, (int) BIGcomp_M_CompressedSize - saved_size);
				BIG_si_CompressedSize = BIG_si_BinFileReadSize;


				/* Lecture */
				BIG_CacheRead(_h_Handle, BIG_gul_ReadPos + BIG_gi_RealSeek + saved_size, (void**)&BIG_gp_CompressedBuffer, BIG_si_CompressedSize);


				BIG_si_BinFileSize = max(BIG_si_BinFileSize - BIG_si_BinFileReadSize, 0);
				BIG_si_BinFileReadSize = min(BIG_si_BinFileSize, (int) BIGcomp_M_CompressedSize);

				/* case the direct read modified ptr */
				if(BIG_gp_CompressedBuffer != (BIG_gp_CompressedBuffer_r + saved_size))
				{
					if(BIG_si_CompressedSize > saved_size)
					{
						L_memmove(BIG_gp_CompressedBuffer - saved_size, BIG_gp_CompressedBuffer_r, saved_size);
						BIG_gp_CompressedBuffer = BIG_gp_CompressedBuffer - saved_size;
					}
					else
					{
						L_memcpy
						(
							BIG_gp_CompressedBuffer_r + saved_size,
							BIG_gp_CompressedBuffer,
							BIG_si_CompressedSize
						);
						BIG_gp_CompressedBuffer = BIG_gp_CompressedBuffer_r;
					}
				}
				else
				{
					BIG_gp_CompressedBuffer = BIG_gp_CompressedBuffer_r;
				}

				BIG_si_CompressedSize += saved_size;
				BIG_sp_CompressedNextBlock = BIG_gp_CompressedBuffer;
			}

			/* check if decompressed buffer can't contents the next block */
			if(!BIGcomp_CheckBuffer((u_char *) BIG_sp_CompressedNextBlock, BIG_gi_ReadSize - BIG_si_DecompressedSize))
			{
				/*~~~~~~~~~*/
				char	*tmp;
				/*~~~~~~~~~*/

#ifndef _GAMECUBE
				MEM_FreeFromEndAlign(BIG_gp_ReadBuffer);
#endif
				BIG_gi_ReadSize = BIGcomp_i_DecompressedSize((u_char *) BIG_sp_CompressedNextBlock) + BIG_si_DecompressedSize;
				tmp = (char*)MEM_p_AllocFromEndAlign(BIG_gi_ReadSize, 64);
				L_memmove(tmp, BIG_gp_ReadBuffer, BIG_si_DecompressedSize);
#ifdef _GAMECUBE
				MEM_Free(BIG_gp_ReadBuffer);
#endif
				BIG_gp_ReadBuffer = tmp;
			}

			/* decompressed next block */
			st_Decomp.bWait = 0;
			st_Decomp.in = (u_char *) BIG_sp_CompressedNextBlock;
			st_Decomp.out = (u_char *) (BIG_gp_ReadBuffer + BIG_si_DecompressedSize);
			BIG_sp_CompressedNextBlock = (char *) BIGcomp_DecodeBlocks(&st_Decomp, 1);

			BIG_gi_RealSeek += (int) BIG_sp_CompressedNextBlock - (int) st_Decomp.in;
			M_UpdateProgress();
			
			BIG_si_DecompressedSize += st_Decomp.uUncompressedLen;
		} while(BIG_si_DecompressedSize < _i_Size);
	}

	*_p_Buffer = BIG_gp_ReadBuffer + BIG_gi_ReadSeek;
	BIG_gi_ReadSeek += _i_Size;
	PROPS2_StopRaster(&PROPS2_gst_BIG_fread_mode2);
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int BIG_fread(void *_p_Buffer, int _i_Size, BIGFileHandle _h_Handle)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			ret;
	int			preload;
	int			initsize, totalsize;
	int			i_Finished;
	int			ioffsetbuf;
	extern int	gi_TestBin;
	extern int	gi_CurBin;
	extern int	gai_CurSize[];
	extern int	PreLoadTex_Finished(int *, int);
	extern char *gap_FirstPointer[];
	extern char *MEM_gp_MaxBlock;
	extern void PreloadCancel(void);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define PRELOADTEXGRAN	(40 * 1024)

	MEMpro_StartMemRaster();
	PROPS2_StartRaster(&PROPS2_gst_BIG_fread);
	preload = 0;
	ioffsetbuf = 0;

	if(BIG_gi_ReadMode == 2)
	{
		PROPS2_StartRaster(&PROPS2_gst_BIG_fread_mode2);
		if(!BIG_gp_ReadBuffer)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			BIGcomp_tdst_DecompressParams	st_Decomp;
			char							*tmp;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef _GAMECUBE
			tmp = BIG_gp_ReadBuffer = (char *) MEM_p_AllocTmp(BIG_gi_ReadSize);
#else
#if defined(ACTIVE_EDITORS) && defined(JADEFUSION)
			tmp = BIG_gp_ReadBuffer = (char *) L_malloc(READ_BUFFER_SIZE);
#else
			tmp = BIG_gp_ReadBuffer = (char *) MEM_p_AllocFromEndAlign(BIG_gi_ReadSize, 64);
#endif
#endif
			/* Preload ? */
			i_Finished = 0;
#if !defined(MAX_PLUGIN)
			PreLoadTex_Finished(&i_Finished, 1);
			if(!i_Finished) PreloadCancel();
#endif // !defined(MAX_PLUGIN)

			/* Preload */
#if !defined(MAX_PLUGIN)
			if(gi_TestBin != -1 && gi_TestBin <= gi_CurBin && gai_CurSize[gi_TestBin])
			{
				if(gap_FirstPointer[gi_TestBin] - gai_CurSize[gi_TestBin] > MEM_gp_MaxBlock)
				{
					/*~~~~~~~~*/
					LONG	*pp;
					/*~~~~~~~~*/

					pp = (LONG *) (gap_FirstPointer[gi_TestBin] - PRELOADTEXGRAN);
					initsize = BIG_si_BinFileSize = *pp;
					ioffsetbuf = gai_CurSize[gi_TestBin];
					ioffsetbuf = (ioffsetbuf > initsize) ? initsize : ioffsetbuf;

#if defined(PSX2_TARGET) && !defined(_FINAL_)
					{
						/*~~~~~~~~~~~~*/
						char	tmp[64];
						/*~~~~~~~~~~~~*/

						sprintf(tmp, "-- Preload %d Ko\n", ioffsetbuf / 1024);
						GSP_OutputConsole(tmp);
					}

#endif
					preload = 1;
					goto zappreload;
				}
			}
#endif // !defined(MAX_PLUGIN)

			/* read the bin file size (in BIG_gp_ReadBuffer only for tmp use) */
			CDV_i_DirectSeekFile(_h_Handle, L_SEEK_SET, BIG_gul_ReadPos);
			CDV_i_DirectReadFile(_h_Handle, &BIG_gp_ReadBuffer, CDV_Cte_SectorSize);
			BIG_si_BinFileSize = ReadLong(BIG_gp_ReadBuffer);
#if defined(_GAMECUBE) || defined(_XENON)

			SwapDWord((LONG *) &BIG_si_BinFileSize);
#endif /* #ifdef _GAMECUBE */
			
			BIG_gf_DispBinProgress = 0.0f;
			BIG_gui_DispBinProgressSize = BIG_si_BinFileSize;
			
			BIG_gp_ReadBuffer = tmp;

zappreload:
			BIG_si_BinFileReadSize = min((int) BIGcomp_M_CompressedSize, BIG_si_BinFileSize);
			BIG_si_CompressedSize = BIG_si_BinFileReadSize;
			BIG_gp_CompressedBuffer_r = BIG_gp_CompressedBuffer = (char *) MEM_p_AllocTmp(BIGcomp_M_CompressedSize + (3 * CDV_Cte_SectorSize));

			/* set position at start of bin file */
			if((!preload) || (ioffsetbuf < BIG_si_BinFileReadSize))
			{
				/* read the 1st buff */
				CDV_i_DirectSeekFile(_h_Handle, L_SEEK_SET, BIG_gul_ReadPos + ioffsetbuf);

				BIG_gp_CompressedBuffer += ioffsetbuf;
				CDV_i_DirectReadFile(_h_Handle, &BIG_gp_CompressedBuffer, BIG_si_CompressedSize - ioffsetbuf);
				BIG_gp_CompressedBuffer -= ioffsetbuf;
			}

			BIG_si_BinFileSize = max(BIG_si_BinFileSize - BIG_si_BinFileReadSize, 0);

			/* Copie all preloaded buffers */
			if(preload)
			{
			    int ij;
			    
				totalsize = 0;
				for(totalsize = 0; totalsize < gai_CurSize[gi_TestBin]; totalsize += PRELOADTEXGRAN)
				{
					L_memcpy
					(
						BIG_gp_CompressedBuffer + totalsize,
						gap_FirstPointer[gi_TestBin] - totalsize - PRELOADTEXGRAN,
						PRELOADTEXGRAN
					);
				}
    			
    			totalsize -= PRELOADTEXGRAN;
    			for(ij=0; ij < gi_CurBin; ij++)
    			{
    			    if(gap_FirstPointer[ij] < gap_FirstPointer[gi_TestBin])
    			    {
    			        L_memmove(gap_FirstPointer[ij]+totalsize-gai_CurSize[ij], gap_FirstPointer[ij]-gai_CurSize[ij], gai_CurSize[ij]);
    			        gap_FirstPointer[ij] += totalsize;
    			    }
    			}
			}

			BIG_si_BinFileReadSize = min(BIG_si_BinFileSize, (int) BIGcomp_M_CompressedSize);
			BIG_gi_ReadSeek = 0;
			st_Decomp.bWait = 0;

			/* first word is the bin file size, so we jump it */
			st_Decomp.in = (u_char *) (BIG_gp_CompressedBuffer + 4);
			st_Decomp.out = (u_char *) (BIG_gp_ReadBuffer);
			BIG_sp_CompressedNextBlock = (char *) BIGcomp_DecodeBlocks(&st_Decomp, 1);

			BIG_gi_RealSeek = BIG_sp_CompressedNextBlock - BIG_gp_CompressedBuffer;
			M_UpdateProgress();
			
			BIG_si_DecompressedSize = st_Decomp.uUncompressedLen;

    		/* On en est la du chargement */
    		BIG_ResetCache();
    		BIG_ul_CachePos = BIG_gul_ReadPos + BIG_si_CompressedSize - CDV_Cte_SectorSize;
		}
		else if(BIG_gi_ReadSeek + _i_Size > BIG_si_DecompressedSize)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			BIGcomp_tdst_DecompressParams	st_Decomp;
			int								i_FirstPartSize;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			/* first read the end of decompressed buffer to free it */
			i_FirstPartSize = BIG_si_DecompressedSize - BIG_gi_ReadSeek;
			if(_p_Buffer)
			{
				L_memcpy(_p_Buffer, BIG_gp_ReadBuffer + BIG_gi_ReadSeek, i_FirstPartSize);
				_p_Buffer = (void *) ((int) _p_Buffer + i_FirstPartSize);
			}

			BIG_gi_ReadSeek += i_FirstPartSize;
			_i_Size = _i_Size - i_FirstPartSize;

			/* check decompressed buffer length */
			if(_i_Size > BIG_gi_ReadSize)
			{
				/*~~~~~~~~~*/
				char	*tmp;
				/*~~~~~~~~~*/

#ifndef _GAMECUBE
				MEM_FreeFromEndAlign(BIG_gp_ReadBuffer);
#endif
#ifdef _GAMECUBE
				tmp = (char *) MEM_p_AllocTmp(_i_Size);
#else
				tmp = (char *) MEM_p_AllocFromEndAlign(_i_Size, 64);
#endif				

				L_memmove(tmp, BIG_gp_ReadBuffer, BIG_gi_ReadSize);
#ifdef _GAMECUBE
				MEM_Free(BIG_gp_ReadBuffer);
#endif
				BIG_gp_ReadBuffer = tmp;
				BIG_gi_ReadSize = _i_Size;
				BIG_gi_ReadSeek = 0;
				BIG_si_DecompressedSize = 0;
			}
			else if(_i_Size + i_FirstPartSize > BIG_gi_ReadSize)
			{
				BIG_gi_ReadSeek = 0;
				BIG_si_DecompressedSize = 0;
			}

			/* then decompressed others blocks */
			do
			{
				/* check if compressed buffer contents the whole next block */
				if
				(
					!BIGcomp_CheckBlock
					(
						(u_char *) BIG_sp_CompressedNextBlock,
						BIG_si_CompressedSize - (BIG_sp_CompressedNextBlock - BIG_gp_CompressedBuffer)
					)
				)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
					int saved_size, remain, rest;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

					/*
					 * we have to reload the compressed buffer but without losting the data yet loaded £
					 * rounded down to 1 sect
					 */
					saved_size = (BIG_si_CompressedSize - (BIG_sp_CompressedNextBlock - BIG_gp_CompressedBuffer)) / CDV_Cte_SectorSize;
					saved_size *= CDV_Cte_SectorSize;
					rest = (BIG_si_CompressedSize - (BIG_sp_CompressedNextBlock - BIG_gp_CompressedBuffer)) - saved_size;

					/* remain to load */
					remain = BIGcomp_i_CompressedSize((u_char *) BIG_sp_CompressedNextBlock) - saved_size;

					/* save */
					L_memcpy(BIG_gp_CompressedBuffer_r, BIG_sp_CompressedNextBlock, saved_size);

					/* update the buff ptr */
					BIG_gp_CompressedBuffer = BIG_gp_CompressedBuffer_r + saved_size;

					/* update the remain file size (rounded up to 1 sector) */
					BIG_si_BinFileSize += rest;
					BIG_si_BinFileSize = (BIG_si_BinFileSize + CDV_Cte_SectorSize - 1) / CDV_Cte_SectorSize;
					BIG_si_BinFileSize *= CDV_Cte_SectorSize;

					/* reeval the read file size */
					BIG_si_BinFileReadSize = min(BIG_si_BinFileSize, (int) BIGcomp_M_CompressedSize - saved_size);
					BIG_si_CompressedSize = BIG_si_BinFileReadSize;

					/* set pos */
					/*CDV_i_DirectSeekFile(_h_Handle, L_SEEK_SET, BIG_gul_ReadPos + BIG_gi_RealSeek + saved_size);
					CDV_i_DirectReadFile(_h_Handle, &BIG_gp_CompressedBuffer, BIG_si_CompressedSize);*/
    				
    				/* Lecture */
    				BIG_CacheRead(_h_Handle, BIG_gul_ReadPos + BIG_gi_RealSeek + saved_size, (void**)&BIG_gp_CompressedBuffer, BIG_si_CompressedSize);
					
					
					BIG_si_BinFileSize = max(BIG_si_BinFileSize - BIG_si_BinFileReadSize, 0);
					BIG_si_BinFileReadSize = min(BIG_si_BinFileSize, (int) BIGcomp_M_CompressedSize);

					/* case the direct read modified ptr */
					if(BIG_gp_CompressedBuffer != (BIG_gp_CompressedBuffer_r + saved_size))
					{
						if(BIG_si_CompressedSize > saved_size)
						{
							L_memmove(BIG_gp_CompressedBuffer - saved_size, BIG_gp_CompressedBuffer_r, saved_size);
							BIG_gp_CompressedBuffer = BIG_gp_CompressedBuffer - saved_size;
						}
						else
						{
							L_memcpy
							(
								BIG_gp_CompressedBuffer_r + saved_size,
								BIG_gp_CompressedBuffer,
								BIG_si_CompressedSize
							);
							BIG_gp_CompressedBuffer = BIG_gp_CompressedBuffer_r;
						}
					}
					else
					{
						BIG_gp_CompressedBuffer = BIG_gp_CompressedBuffer_r;
					}

					BIG_si_CompressedSize += saved_size;

					BIG_sp_CompressedNextBlock = BIG_gp_CompressedBuffer;
				}

				/* check if decompressed buffer can't contents the next block */
				if
				(
					!BIGcomp_CheckBuffer
					(
						(u_char *) BIG_sp_CompressedNextBlock,
						BIG_gi_ReadSize - BIG_si_DecompressedSize
					)
				)
					{
						/* then read the unread decompressed data */
						i_FirstPartSize = BIG_si_DecompressedSize -
						BIG_gi_ReadSeek;
					if(i_FirstPartSize && _p_Buffer)
					{
						L_memcpy(_p_Buffer, BIG_gp_ReadBuffer + BIG_gi_ReadSeek, i_FirstPartSize);
						_p_Buffer = (void *) ((int) _p_Buffer + i_FirstPartSize);
					}

					_i_Size = max(_i_Size - i_FirstPartSize, 0);

					/* and go to the beginning of uncompressed buffer */
					BIG_gi_ReadSeek = 0;
					BIG_si_DecompressedSize = 0;
				}

				/* decompressed next block */
				st_Decomp.bWait = 0;
				st_Decomp.in = (u_char *) BIG_sp_CompressedNextBlock;
				st_Decomp.out = (u_char *) (BIG_gp_ReadBuffer + BIG_si_DecompressedSize);
				BIG_sp_CompressedNextBlock = (char *) BIGcomp_DecodeBlocks(&st_Decomp, 1);

				BIG_gi_RealSeek += (int) BIG_sp_CompressedNextBlock - (int) st_Decomp.in;
				M_UpdateProgress();
				
				BIG_si_DecompressedSize += st_Decomp.uUncompressedLen;
			} while(BIG_si_DecompressedSize < _i_Size);
		}

		if(_p_Buffer)
		{
			L_memcpy(_p_Buffer, BIG_gp_ReadBuffer + BIG_gi_ReadSeek, _i_Size);
		}

		BIG_gi_ReadSeek += _i_Size;
		ret = 1;
		PROPS2_StopRaster(&PROPS2_gst_BIG_fread_mode2);
	}
	else
	{
		/*~~~~~~~~~~~*/
#ifdef _XBOX
		DWORD iBytesRead;
#elif defined(_XENON)
		int iBytesRead;
#endif
		/*~~~~~~~~~~~*/

	/* _XBOX */
#if defined(PSX2_USE_iopCDV)
		LOA_StartLoadRaster_r(LOA_Cte_LDI_fRead);
		ret = eeCDV_i_ReadFile(_h_Handle, _p_Buffer, _i_Size);
		LOA_StopLoadRaster_r(LOA_Cte_LDI_fRead);
		if(ret)
			ret = 0;
		else
			ret = 1;
#else
		LOA_StartLoadRaster_r(LOA_Cte_LDI_fRead);
#if defined(_XBOX)
		ret = XBCompositeFile_Read(_h_Handle, _p_Buffer, _i_Size, &iBytesRead, 0);
#elif defined(_XENON)
		ret = ReadFile(_h_Handle, _p_Buffer, _i_Size, (LPDWORD)&iBytesRead, 0);

#if !defined(_FINAL_)
		int nRetryNum = 0;
		while( ret == FALSE )
		{
			char szMessage[ 64 ];
			sprintf( szMessage, "=== Read error!  Retrying (%d)...\n", ++nRetryNum );
			OutputDebugString( szMessage );

			ret = ReadFile(_h_Handle, _p_Buffer, _i_Size, (LPDWORD)&iBytesRead, 0);
			if( nRetryNum >= 10 )
				break;
		}
#endif // !defined(_FINAL_)
#else
		ret = L_freadA(_p_Buffer, _i_Size, 1, _h_Handle);
#endif /* _XBOX */
		LOA_StopLoadRaster_r(LOA_Cte_LDI_fRead);
#endif
#ifdef ACTIVE_EDITORS
		if((BIG_gi_ReadMode == 1) && (_i_Size))
		{
			BIG_gi_ReadSize += _i_Size;
			if(!BIG_gp_ReadBuffer)
#ifdef JADEFUSION
			{
#if defined(ACTIVE_EDITORS)
				BIG_gp_ReadBuffer = (char*)L_malloc(READ_BUFFER_SIZE);
#else
				BIG_gp_ReadBuffer = (char*)L_malloc(BIG_gi_ReadSize);
#endif
			}
#if !defined(ACTIVE_EDITORS)
			else
				BIG_gp_ReadBuffer = (char*)L_realloc(BIG_gp_ReadBuffer, BIG_gi_ReadSize);
#endif
			if (!BIG_gp_ReadBuffer)
				_breakpoint_;
#else //JADEFUSION
				BIG_gp_ReadBuffer = L_malloc(BIG_gi_ReadSize);
			else
				BIG_gp_ReadBuffer = L_realloc(BIG_gp_ReadBuffer, BIG_gi_ReadSize);
#endif
			L_memcpy(BIG_gp_ReadBuffer + BIG_gi_ReadSeek, _p_Buffer, _i_Size);
			BIG_gi_ReadSeek += _i_Size;
		}

#endif
	}

	PROPS2_StopRaster(&PROPS2_gst_BIG_fread);
	MEMpro_StopMemRaster(MEMpro_Id_Bigfread);
	return ret;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim:    Low level function to save bytes to a bigfile.

    In:     _p_Buffer Buffer to save. _i_Size Number of bytes to save. _h_Handle Handle of the bigfile (C standard).

    Out:    Returns the real number of bytes saved.
 =======================================================================================================================
 */
int BIG_fwrite(void *_p_Buffer, int _i_Size, L_FILE _h_Handle)
{
    int iResult;
	if(!_i_Size) return 1;
	iResult = L_fwriteA(_p_Buffer, _i_Size, 1, _h_Handle);
    BIG_SetRegistryBfSize(); // So that we always save the size of the bf in the registry
    return iResult;
}

#endif /* ACTIVE_EDITORS */

#if defined(_XBOX) || defined(_XENON)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int XB_fWrite(void *_p_Buffer, int _i_Size, L_FILE _h_Handle)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	DWORD	dwNbBytesWritten;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if(!_i_Size) return 1;

	WriteFile(_h_Handle, _p_Buffer, _i_Size, &dwNbBytesWritten, NULL);
	return dwNbBytesWritten;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int XB_fRead(void *_p_Buffer, int _i_Size, L_FILE _h_Handle)
{
	/*~~~~~~~~~~~~~~~~~~*/
	DWORD	dwNbBytesRead;
	/*~~~~~~~~~~~~~~~~~~*/

	if(!_i_Size) return 1;

	ReadFile(_h_Handle, _p_Buffer, _i_Size, &dwNbBytesRead, NULL);
	return dwNbBytesRead;
}

#endif /* _XBOX */ 
 
 