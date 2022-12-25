/*$T iopCDV_Manager.c GC 1.138 06/23/03 14:19:50 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP
#ifdef PSX2_USE_iopCDV

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include <stdio.h>
#include <kernel.h>
#include <libcdvd.h>
#include <string.h>
#include <sys/file.h>

#include "CDV_Manager.h"
#include "RPC_Manager.h"

#include "iopSND.h"
#include "iopDebug.h"
#include "iopMEM.h"
#include "iopCLI.h"
#include "iopKernel.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    private macros
 ***********************************************************************************************************************
 */

#define _inline_			inline extern
#define M_DiscErrorTimeOut	20

/*$4
 ***********************************************************************************************************************
    private prototypes
 ***********************************************************************************************************************
 */

static int	iopCDV_si_SearchFileInDir(unsigned int *_pui_StartSect, unsigned int *_pui_SectNb, char *_asz_Name);
static int	iopCDV_si_MountDisc(void);
static int	iopCDV_si_SearchFile(CDV_tdst_FileHandler *_pst_File, char *_asz_CompletePath);

#ifdef _DEBUG
static int	iopCDV_dbg_si_CheckISO(void);
static int	iopCDV_dbg_si_CheckDisk(void);
#endif
static int	iopCDV_si_CheckResult(int _i_result);

/*$4
 ***********************************************************************************************************************
    inline fonctions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    get 2 bytes integer from unaligned address
 =======================================================================================================================
 */
_inline_ unsigned short iopCDV_i_Geti16(char *c)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	register unsigned int	tmp1, tmp2, tmp3;
	asm						__volatile__("lbu  %0, 1(%1)" : "=r"(tmp1) : "r"(c));
	asm						__volatile__("lbu  %0, 0(%1)" : "=r"(tmp2) : "r"(c));
	asm						__volatile__("sll  %0, %1, 8" : "=r"(tmp3) : "r"(tmp1));
	asm						__volatile__("or   %0, %1, %2" : "=r"(tmp1) : "r"(tmp2), "r"(tmp3));
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	return (unsigned short) tmp1;
}

/*
 =======================================================================================================================
    get 4 bytes integer from unaligned address
 =======================================================================================================================
 */
_inline_ unsigned int iopCDV_i_Geti32(char *addr)
{
	register unsigned int	tmp;

	asm	__volatile__ ("
		lwr	tmp, 0(addr)
		lwl tmp, 3(addr)
		");

	return tmp;
}

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

static CDV_tdst_PVD			*iopCDV_pst_PVD = NULL;
static char					*sasz_ReadBuffer = NULL;
static sceCdRMode			sst_iopCDV_Mode;	/* Mode settings for CD reading */
static int					si_DiscAvailable;
static CDV_tdst_FileHandler sax_FileHandler[CDV_Cte_MaxFileNb];
static unsigned int			CDV_sui_DirectSeek;
static unsigned int			CDV_sui_DirectSeekOct;
static int					si_ReadingSema;
int							iopCDV_gi_DiscError = 0;

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim: CD initialisation function
 =======================================================================================================================
 */
int iopCDV_i_InitModule(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	struct SemaParam	sem;
	int					res;
	/*~~~~~~~~~~~~~~~~~~~~*/

	iopDbg_M_Msg(iopDbg_Msg_0022);

	sst_iopCDV_Mode.trycount = CDV_Cte_TryCount;
	sst_iopCDV_Mode.spindlctrl = SCECdSpinStm;
	sst_iopCDV_Mode.datapattern = CDV_Cte_DataPattern;

	iopCDV_pst_PVD = iopMEM_pv_iopAllocAlign(sizeof(CDV_tdst_PVD), 64);
	sasz_ReadBuffer = iopMEM_pv_iopAllocAlign(CDV_Cte_SectorSize * sizeof(char), 64);
	L_memset(iopCDV_pst_PVD, 0, sizeof(CDV_tdst_PVD));
	L_memset(sax_FileHandler, 0, CDV_Cte_MaxFileNb * sizeof(CDV_tdst_FileHandler));

	si_DiscAvailable = 0;
	CDV_sui_DirectSeek = CDV_sui_DirectSeekOct = 0;

	sem.initCount = 0;
	sem.maxCount = 1;
	sem.attr = AT_THPRI;
	si_ReadingSema = L_CreateSema(&sem);
	iopDbg_M_Assert((si_ReadingSema > 0), iopDbg_Err_0052);
	CLI_SignalSema(si_ReadingSema);

	res = iopCDV_si_MountDisc();
	iopDbg_M_Msg(iopDbg_Msg_0023);
	return res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline int L_sceCdDiskReady(void)
{
	iopCDV_gi_DiscError = 0;
	while(sceCdDiskReady(0) != SCECdComplete)
	{
		if(iopCDV_gi_DiscError++ >= 160000000) break;
	}
	iopDbg_M_AssertX((iopCDV_gi_DiscError==0), iopDbg_Err_0098" line %d", __LINE__);
	

	if(iopCDV_gi_DiscError < M_DiscErrorTimeOut)
	{
		iopCDV_gi_DiscError = 0;
	}
	else
	{
		iopDbg_M_Err(iopDbg_Err_004D);
	}

	return iopCDV_gi_DiscError;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline int L_sceCdRead(u_int lsn, u_int sectors, void *buf, sceCdRMode *mode)
{
	iopCDV_gi_DiscError = 0;
	while(sceCdRead(lsn, sectors, buf, mode) != 1)
	{
		if(iopCDV_gi_DiscError++ >= M_DiscErrorTimeOut) break;
	}
	iopDbg_M_AssertX((iopCDV_gi_DiscError==0), iopDbg_Err_0098" line %d", __LINE__);

	if(iopCDV_gi_DiscError < M_DiscErrorTimeOut)
	{
		iopCDV_gi_DiscError = 0;
	}
	else
	{
		iopDbg_M_Err(iopDbg_Err_004D);
	}
	return iopCDV_gi_DiscError;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
inline int L_sceCdSync(void)
{
	iopCDV_gi_DiscError = 0;
	while(sceCdSync(0) != 0)
	{
		if(iopCDV_gi_DiscError++ >= 160000000) break;
	}
	iopDbg_M_AssertX((iopCDV_gi_DiscError==0), iopDbg_Err_0098" line %d", __LINE__);

	if(iopCDV_gi_DiscError < M_DiscErrorTimeOut)
	{
		iopCDV_gi_DiscError = 0;
	}
	else
	{
		iopDbg_M_Err(iopDbg_Err_004D);
	}

	return iopCDV_gi_DiscError;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int iopCDV_si_MountDisc(void)
{
	/*~~~~*/
	int res;
	/*~~~~*/

	si_DiscAvailable = 0;
	if(L_sceCdDiskReady()) return -1;
	res = L_sceCdRead(CDV_Cte_SectPVD, 1, (void *) iopCDV_pst_PVD, &sst_iopCDV_Mode);
	res |= L_sceCdSync();

	if(res)
	{
		iopDbg_M_Err(iopDbg_Err_0036);
		return -1;
	}

	si_DiscAvailable = 1;

#ifdef _DEBUG
	res |= iopCDV_dbg_si_CheckISO();
	res |= iopCDV_dbg_si_CheckDisk();
#endif
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopCDV_i_CloseModule(void)
{
	sceCdBreak();
	sceCdInit(SCECdEXIT);
	si_DiscAvailable = 0;

	iopMEM_iopFreeAlign(iopCDV_pst_PVD);
	iopCDV_pst_PVD = NULL;

	iopMEM_iopFreeAlign(sasz_ReadBuffer);
	sasz_ReadBuffer = NULL;

	L_memset(sax_FileHandler, 0, CDV_Cte_MaxFileNb * sizeof(CDV_tdst_FileHandler));
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopCDV_i_RemoteOpenFile(char *asz_FileName, CDV_tdst_FileHandler *_pst_Handler)
{
	/*~~~*/
	int id;
	/*~~~*/

	if(!si_DiscAvailable) return -1;
	id = iopCDV_i_OpenFile(asz_FileName);
	if(id < 0) return -1;
	L_memcpy(_pst_Handler, &sax_FileHandler[id], sizeof(CDV_tdst_FileHandler));
	return id;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopCDV_i_OpenFile(char *asz_FileName)
{
	/*~~~~~*/
	int id;
	int ko;
	int size;
	/*~~~~~*/

	if(!si_DiscAvailable) return -1;

	for(id = 0; id < CDV_Cte_MaxFileNb; id++)
	{
		if(sax_FileHandler[id].ui_FileSize == 0) break;
	}

	if(id == CDV_Cte_MaxFileNb) return -1;

	ko = iopCDV_si_SearchFile(&sax_FileHandler[id], asz_FileName);
	if(ko)
	{
		return -1;
	}

	sax_FileHandler[id].puc_Buffer = (unsigned char *) iopMEM_pv_iopAllocAlign(CDV_Cte_DefaultBuffSize, 64);

	if(sax_FileHandler[id].puc_Buffer == NULL)
	{
		iopCDV_i_CloseFile(id);
		return -1;
	}

	sax_FileHandler[id].ui_BufferSize = CDV_Cte_DefaultBuffSize;
	size = sax_FileHandler[id].ui_BufferSize / CDV_Cte_SectorSize;

	ko = iopCDV_i_SyncRead
		(
			size,
			sax_FileHandler[id].ui_StartSector + sax_FileHandler[id].ui_FileSeek,
			sax_FileHandler[id].puc_Buffer
		);
	if(ko)
	{
		iopCDV_i_CloseFile(id);
		return -1;
	}

	sax_FileHandler[id].ui_BufferSeek = 0;
	sax_FileHandler[id].ui_FileSeek = size;

#if 0
	printf("##IOP open file:\n");
	printf("##IOP name           : %s\n", sax_FileHandler[id].asz_Name);
	printf("##IOP ui_StartSector : %d\n", sax_FileHandler[id].ui_StartSector);
	printf("##IOP ui_SectorNb    : %d\n", sax_FileHandler[id].ui_SectorNb);
	printf("##IOP ui_FileSize    : %d\n", sax_FileHandler[id].ui_FileSize);
	printf("##IOP ui_FileSeek    : %d\n", sax_FileHandler[id].ui_FileSeek);
	printf("##IOP puc_Buffer     : %x\n", (int) sax_FileHandler[id].puc_Buffer);
	printf("##IOP ui_BufferSize  : %d\n", sax_FileHandler[id].ui_BufferSize);
	printf("##IOP ui_BufferSeek  : %d\n", sax_FileHandler[id].ui_BufferSeek);
	printf("##IOP file opened !!\n");
#endif
	return id;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopCDV_i_ReadFile(int _i_FileHandler, char *_pc_Buff, int _i_Size)
{
	/*~~~~~~~*/
	int remain;
	int save;
	int ko;
	int timeout;
	/*~~~~~~~*/

	save = _i_Size;

	if(!si_DiscAvailable) return -1;

	if(_i_Size > (sax_FileHandler[_i_FileHandler].ui_FileSize - iopCDV_ui_TellFile(_i_FileHandler)))
	{
		_i_Size = (sax_FileHandler[_i_FileHandler].ui_FileSize - iopCDV_ui_TellFile(_i_FileHandler));
	}

	if(!sax_FileHandler[_i_FileHandler].ui_Flags)
	{
		remain = sax_FileHandler[_i_FileHandler].ui_BufferSize - sax_FileHandler[_i_FileHandler].ui_BufferSeek;
		if(remain > _i_Size)
		{
			L_memcpy
			(
				_pc_Buff,
				sax_FileHandler[_i_FileHandler].puc_Buffer + sax_FileHandler[_i_FileHandler].ui_BufferSeek,
				_i_Size
			);
			sax_FileHandler[_i_FileHandler].ui_BufferSeek += _i_Size;
			_i_Size = 0;
		}
		else if(remain > 0)
		{
			L_memcpy
			(
				_pc_Buff,
				sax_FileHandler[_i_FileHandler].puc_Buffer + sax_FileHandler[_i_FileHandler].ui_BufferSeek,
				remain
			);
			sax_FileHandler[_i_FileHandler].ui_BufferSeek = 0;
			_pc_Buff += remain;
			_i_Size -= remain;
		}
		else
			sax_FileHandler[_i_FileHandler].ui_BufferSeek = 0;
	}

	sax_FileHandler[_i_FileHandler].ui_Flags = 0;

	while(_i_Size > (sax_FileHandler[_i_FileHandler].ui_BufferSize - sax_FileHandler[_i_FileHandler].ui_BufferSeek))
	{
		remain = (sax_FileHandler[_i_FileHandler].ui_BufferSize + CDV_Cte_SectorSize - 1) / CDV_Cte_SectorSize;
		timeout = 0;
		do
		{
			ko = iopCDV_i_SyncRead
				(
					remain,
					sax_FileHandler[_i_FileHandler].ui_StartSector + sax_FileHandler[_i_FileHandler].ui_FileSeek,
					sax_FileHandler[_i_FileHandler].puc_Buffer
				);
			
			iopDbg_M_AssertX((timeout==0), iopDbg_Err_0098" line %d", __LINE__);
			if(timeout++>M_DiscErrorTimeOut)
			{
			    iopCDV_gi_DiscError += timeout;
			    break;
			}
		} while(ko);
		if(iopCDV_gi_DiscError>M_DiscErrorTimeOut) return (save - _i_Size);
		iopCDV_gi_DiscError = 0;
		sax_FileHandler[_i_FileHandler].ui_FileSeek += remain;

		L_memcpy
		(
			_pc_Buff,
			sax_FileHandler[_i_FileHandler].puc_Buffer + sax_FileHandler[_i_FileHandler].ui_BufferSeek,
			sax_FileHandler[_i_FileHandler].ui_BufferSize - sax_FileHandler[_i_FileHandler].ui_BufferSeek
		);
		_pc_Buff += (sax_FileHandler[_i_FileHandler].ui_BufferSize - sax_FileHandler[_i_FileHandler].ui_BufferSeek);
		_i_Size -= (sax_FileHandler[_i_FileHandler].ui_BufferSize - sax_FileHandler[_i_FileHandler].ui_BufferSeek);

		sax_FileHandler[_i_FileHandler].ui_BufferSeek = 0;
	}

	if(_i_Size > 0)
	{
		remain = (sax_FileHandler[_i_FileHandler].ui_BufferSize + CDV_Cte_SectorSize - 1) / CDV_Cte_SectorSize;
		timeout = 0;
		do
		{
			ko = iopCDV_i_SyncRead
				(
					remain,
					sax_FileHandler[_i_FileHandler].ui_StartSector + sax_FileHandler[_i_FileHandler].ui_FileSeek,
					sax_FileHandler[_i_FileHandler].puc_Buffer
				);
            iopDbg_M_AssertX((timeout==0), iopDbg_Err_0098" line %d", __LINE__);				
			if(timeout++>M_DiscErrorTimeOut)
			{
			    iopCDV_gi_DiscError += timeout;
			    break;
			}
		} while(ko);
		if(iopCDV_gi_DiscError>M_DiscErrorTimeOut) return (save - _i_Size);
		iopCDV_gi_DiscError = 0;

		sax_FileHandler[_i_FileHandler].ui_FileSeek += remain;
		L_memcpy
		(
			_pc_Buff,
			sax_FileHandler[_i_FileHandler].puc_Buffer + sax_FileHandler[_i_FileHandler].ui_BufferSeek,
			_i_Size
		);
		sax_FileHandler[_i_FileHandler].ui_BufferSeek += _i_Size;
	}

	return save;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopCDV_i_SeekFile(int _i_FileHandler, int origin, unsigned int pos)
{
	if(!si_DiscAvailable) return -1;

	if(origin == CDV_Cte_SeekCur)
		pos += iopCDV_ui_TellFile(_i_FileHandler);
	else if(origin == CDV_Cte_SeekEnd)
		pos = sax_FileHandler[_i_FileHandler].ui_FileSize - pos;
	else if(origin != CDV_Cte_SeekSet)
	{
		return -1;
	}

	if(pos > sax_FileHandler[_i_FileHandler].ui_FileSize)
	{
		return -1;
	}

	if
	(
		(
			pos >=
				(
					(sax_FileHandler[_i_FileHandler].ui_FileSeek * CDV_Cte_SectorSize) -
					sax_FileHandler[_i_FileHandler].ui_BufferSize
				)
		)
	&&	(pos < (sax_FileHandler[_i_FileHandler].ui_FileSeek * CDV_Cte_SectorSize))
	)
	{
		sax_FileHandler[_i_FileHandler].ui_BufferSeek = pos -
			(
				sax_FileHandler[_i_FileHandler].ui_FileSeek *
				CDV_Cte_SectorSize
			) +
					sax_FileHandler[_i_FileHandler].ui_BufferSize;
	}
	else
	{
		sax_FileHandler[_i_FileHandler].ui_FileSeek = pos / CDV_Cte_SectorSize;
		sax_FileHandler[_i_FileHandler].ui_Flags = 1;
		sax_FileHandler[_i_FileHandler].ui_BufferSeek = pos - (sax_FileHandler[_i_FileHandler].ui_FileSeek * CDV_Cte_SectorSize);
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int iopCDV_ui_TellFile(int _i_FileHandler)
{
	/*~~~~~~~~~~~~~~~~*/
	unsigned int	ret;
	/*~~~~~~~~~~~~~~~~*/

	if(!si_DiscAvailable) return -1;
	ret = (sax_FileHandler[_i_FileHandler].ui_FileSeek * CDV_Cte_SectorSize) -
		sax_FileHandler[_i_FileHandler].ui_BufferSize +
		sax_FileHandler[_i_FileHandler].ui_BufferSeek;
	return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopCDV_i_CloseFile(int _i_FileHandler)
{
	if(!si_DiscAvailable) return -1;
	sax_FileHandler[_i_FileHandler].asz_Name[0] = '\0';
	sax_FileHandler[_i_FileHandler].ui_StartSector = 0;
	sax_FileHandler[_i_FileHandler].ui_SectorNb = 0;
	sax_FileHandler[_i_FileHandler].ui_FileSize = 0;
	sax_FileHandler[_i_FileHandler].ui_FileSeek = 0;
	sax_FileHandler[_i_FileHandler].ui_Flags = 0;
	if(sax_FileHandler[_i_FileHandler].puc_Buffer) iopMEM_iopFreeAlign(sax_FileHandler[_i_FileHandler].puc_Buffer);
	sax_FileHandler[_i_FileHandler].puc_Buffer = NULL;
	sax_FileHandler[_i_FileHandler].ui_BufferSize = 0;
	sax_FileHandler[_i_FileHandler].ui_BufferSeek = 0;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopCDV_i_DirectSetSize(int _i_FileHandler, int _i_NewSize)
{
	/*~~~~~~~~~~~~~~~~~*/
	unsigned char	*tmp;
	/*~~~~~~~~~~~~~~~~~*/

	if(!si_DiscAvailable) return -1;

	_i_NewSize = ((_i_NewSize + CDV_Cte_SectorSize - 1) / CDV_Cte_SectorSize) * CDV_Cte_SectorSize;
	if(_i_NewSize == sax_FileHandler[_i_FileHandler].ui_BufferSize) return 0;

	tmp = iopMEM_pv_iopAllocAlign(_i_NewSize, 64);
	if(tmp == NULL) return -1;
	iopMEM_iopFreeAlign(sax_FileHandler[_i_FileHandler].puc_Buffer);

	sax_FileHandler[_i_FileHandler].puc_Buffer = tmp;
	sax_FileHandler[_i_FileHandler].ui_BufferSize = _i_NewSize;
	sax_FileHandler[_i_FileHandler].ui_BufferSeek = 0;
	sax_FileHandler[_i_FileHandler].ui_FileSeek = 0;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopCDV_i_SetBuffer(int _i_FileHandler, int _i_Size, char *_pc_Buff)
{
	iopMEM_iopFreeAlign(sax_FileHandler[_i_FileHandler].puc_Buffer);
	sax_FileHandler[_i_FileHandler].puc_Buffer = _pc_Buff;
	sax_FileHandler[_i_FileHandler].ui_BufferSize = _i_Size;
	sax_FileHandler[_i_FileHandler].ui_BufferSeek = 0;
	sax_FileHandler[_i_FileHandler].ui_FileSeek = 0;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopCDV_i_GetiopBufferSize(int _i_FileHandler)
{
	if(!si_DiscAvailable) return -1;
	return sax_FileHandler[_i_FileHandler].ui_BufferSize;
}

#ifdef __CDV_Trace__
char	CDV_dbg_Proc[4] = "iop";
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopCDV_i_SyncRead(unsigned int _ui_SectorsNb, unsigned int _ui_1stSector, char *_pc_Buffer)
{
	/*~~~~*/
	int res;
	/*~~~~*/

	if(!si_DiscAvailable) return -1;

	CDV_Dbg_Display("--IOP: Read(%s) Seek %d Size %d\n", CDV_dbg_Proc, 2048 * _ui_1stSector, 2048 * _ui_SectorsNb);

	CLI_WaitSema(si_ReadingSema);
	res = L_sceCdDiskReady();
	res |= L_sceCdRead(_ui_1stSector, _ui_SectorsNb, (void *) _pc_Buffer, &sst_iopCDV_Mode);
	res |= L_sceCdSync();
	CLI_SignalSema(si_ReadingSema);

	if(res)
	{
		iopCDV_si_CheckResult(res);
		iopDbg_M_Msg(iopDbg_Msg_0020);
		return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
    Aim: Search a file/dir from the root directory. The name must be complete from the root directory. Out: start
    sector, size in bytes.
 =======================================================================================================================
 */
static int iopCDV_si_SearchFile(CDV_tdst_FileHandler *_pst_File, char *_asz_CompletePath)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			asz_PartialPath[CDV_Cte_MaxNameLength];
	char			*pc_PartialPathParser;
	char			*pc_PathParser;
	int				i_DirIndex;
	unsigned int	ui_StartSect;
	unsigned int	ui_SectNb;
	unsigned int	ui_BytesSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!si_DiscAvailable) return -1;

	/* Check we have a root directory */
	if(*_asz_CompletePath != '\\') return(-1);

	/* get root dir sector + sector size */
	ui_StartSect = iopCDV_i_Geti32(iopCDV_pst_PVD->st_RootDirectoryEntry.i32_StartSector);
	ui_SectNb = iopCDV_i_Geti32(iopCDV_pst_PVD->st_RootDirectoryEntry.i32_DataLength);
	ui_SectNb = (ui_SectNb + CDV_Cte_SectorSize - 1) / CDV_Cte_SectorSize;

	/*
	 * Now create a directory name and check that it is on the CD £
	 * only do this a maximum of 8 times (iso 9660 limitation)
	 */
	pc_PathParser = _asz_CompletePath;

	for(i_DirIndex = 0; i_DirIndex < CDV_Cte_MaxDirLevel; i_DirIndex++)
	{
		asz_PartialPath[0] = '\0';
		pc_PartialPathParser = &asz_PartialPath[0];
		pc_PathParser++;

		while(*pc_PathParser != '\\' && *pc_PathParser != '\0') *pc_PartialPathParser++ = *pc_PathParser++;

		*pc_PartialPathParser = '\0';

		if(*pc_PathParser == '\0')
		{
			/* find file in root directory */
			if((ui_BytesSize = iopCDV_si_SearchFileInDir(&ui_StartSect, &ui_SectNb, asz_PartialPath)))
			{
				_pst_File->ui_StartSector = ui_StartSect;
				_pst_File->ui_SectorNb = ui_SectNb;
				_pst_File->ui_FileSize = ui_BytesSize;
				strcpy(_pst_File->asz_Name, asz_PartialPath);
				return 0;
			}
			else
				return -1;
		}
		else
		{
			if(!iopCDV_si_SearchFileInDir(&ui_StartSect, &ui_SectNb, asz_PartialPath)) return -1;
		}
	}

	return -1;
}

/*$4
 ***********************************************************************************************************************
    private functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim: returns size in bytes, start sector and sectors number. Size is 0 if the file/dir isn't found in the current
    directory.
 =======================================================================================================================
 */
static int iopCDV_si_SearchFileInDir(unsigned int *_pui_StartSect, unsigned int *_pui_SectNb, char *_asz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i_SectIndex;
	int				i_DirIndex;
	unsigned int	ui_Size;
	CDV_tdst_DIR	*pst_DirDesc;
	int				ko;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!si_DiscAvailable) return -1;

	i_DirIndex = 0;

	/* loop through all the sectors, checking for directory or filename */
	for(i_SectIndex = 0; i_SectIndex < *_pui_SectNb; i_SectIndex++)
	{
		i_DirIndex = 0;
		do
		{
			ko = iopCDV_i_SyncRead(1, (*_pui_StartSect + i_SectIndex), sasz_ReadBuffer);
		} while(ko);
		pst_DirDesc = (CDV_tdst_DIR *) sasz_ReadBuffer;

		while(pst_DirDesc->i8_Length)
		{
			if(strcmp(pst_DirDesc->asz_Name, _asz_Name) == 0)
			{
				/* Get value of sector position */
				ui_Size = iopCDV_i_Geti32(pst_DirDesc->i32_DataLength);
				*_pui_StartSect = iopCDV_i_Geti32(pst_DirDesc->i32_StartSector);
				*_pui_SectNb = ((ui_Size + CDV_Cte_SectorSize - 1) / CDV_Cte_SectorSize);
				return ui_Size;
			}

			i_DirIndex += pst_DirDesc->i8_Length;
			pst_DirDesc = (CDV_tdst_DIR *) &sasz_ReadBuffer[i_DirIndex];
		}
	}

	return 0;
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    debug fct
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef _DEBUG

/*
 =======================================================================================================================
    Aim: Checks if CD has an ISO image
 =======================================================================================================================
 */
static int iopCDV_dbg_si_CheckISO(void)
{
	if(iopCDV_pst_PVD->i8_VDType != CDV_Cte_StdVolType) return -1;
	if(strncmp(iopCDV_pst_PVD->asz_VSStandardID, CDV_Cte_StdVolID, 5) != 0) return -1;
	return 0;
}

/*
 =======================================================================================================================
    Aim: Checks that CD disk can be used with CdSearchFile() library function Return Value: 1 - CD OK 0 - error found
    Note: CdSearchFile can only use directory entries that are only one sector in length, this function checks a CD for
    all it's directory entries and makes sure that they are only one sector in length. If they are greater then the
    offending directory name is displayed.
 =======================================================================================================================
 */
static int iopCDV_dbg_si_CheckDisk(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i_SubSectorIndex;
	int				i_BuffIndex;
	unsigned int	ui_DataSize;
	CDV_tdst_Sector ast_DirToCheck[CDV_Cte_MaxDirNb];
	CDV_tdst_Sector st_Current;
	CDV_tdst_DIR	*pst_DirFileDesc;
	int				i_NewDir, i_CurrentDir;
	int				ko;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	iopDbg_M_Msg(iopDbg_Msg_0020);

	i_BuffIndex = 0;
	i_NewDir = 0;
	i_CurrentDir = 0;

	/* 1st dir is the root */
	st_Current.ui_StartSector = iopCDV_i_Geti32(iopCDV_pst_PVD->st_RootDirectoryEntry.i32_StartSector);
	st_Current.ui_SectorNb = iopCDV_i_Geti32(iopCDV_pst_PVD->st_RootDirectoryEntry.i32_DataLength);
	st_Current.ui_SectorNb = st_Current.ui_SectorNb / CDV_Cte_SectorSize;

	ast_DirToCheck[i_NewDir].ui_StartSector = st_Current.ui_StartSector;
	ast_DirToCheck[i_NewDir++].ui_SectorNb = st_Current.ui_SectorNb;

	while(i_CurrentDir != i_NewDir)
	{
		st_Current.ui_StartSector = ast_DirToCheck[i_CurrentDir].ui_StartSector;
		st_Current.ui_SectorNb = ast_DirToCheck[i_CurrentDir++].ui_SectorNb;

		for(i_SubSectorIndex = 0; i_SubSectorIndex < st_Current.ui_SectorNb; i_SubSectorIndex++)
		{
			i_BuffIndex = 0;

			/*
			 * Read Sector, for each entry if file output file details, if dir call this £
			 * function with start sector and number of sectors
			 */
			do
			{
				ko = iopCDV_i_SyncRead(1, (st_Current.ui_StartSector + i_SubSectorIndex), sasz_ReadBuffer);
			} while(ko);
			pst_DirFileDesc = (CDV_tdst_DIR *) &sasz_ReadBuffer[i_BuffIndex];

			while(pst_DirFileDesc->i8_Length)
			{
				/* Get start sector and length for file / dir entry */
				ui_DataSize = iopCDV_i_Geti32(pst_DirFileDesc->i32_DataLength);

				/* Is it current directory */
				if((pst_DirFileDesc->i8_NameLength == 1) && (pst_DirFileDesc->asz_Name[0] == 0))
				{
					/* ./ directory */
					if(st_Current.ui_SectorNb != 1)
					{
						iopDbg_M_Err(iopDbg_Err_0030);
						iopDbg_M_Err(iopDbg_Err_0031);
						return -1;
					}
				}
				else if((pst_DirFileDesc->i8_NameLength == 1) && (pst_DirFileDesc->asz_Name[0] == 1))
				{
					/* ../ directory */
					if((ui_DataSize / CDV_Cte_SectorSize) != 1)
					{
						iopDbg_M_Err(iopDbg_Err_0030);
						iopDbg_M_Err(iopDbg_Err_0032);
						return -1;
					}
				}
				else
				{
					if(pst_DirFileDesc->i8_FileFlags & CDV_Cte_directoryBit)
					{
						if((ui_DataSize / CDV_Cte_SectorSize) != 1)
						{
							/*~~~*/
							int kk;
							/*~~~*/

							/* Print file name */
							iopDbg_M_Err(iopDbg_Err_0030);
							printf(" ** ");
							for(kk = 0; kk < pst_DirFileDesc->i8_NameLength; kk++)
								printf("%c", pst_DirFileDesc->asz_Name[kk]);
							printf(" too big **\n");
							return -1;
						}

						/* new directory, put in array to process later */
						ast_DirToCheck[i_NewDir].ui_StartSector = iopCDV_i_Geti32(pst_DirFileDesc->i32_StartSector);
						ast_DirToCheck[i_NewDir++].ui_SectorNb = ui_DataSize / CDV_Cte_SectorSize;

						/* make sure pointer does go over the array size */
						i_NewDir &= 0xFF;
					}
				}

				/* add the number of bytes in the record onto i_BuffIndex */
				i_BuffIndex += pst_DirFileDesc->i8_Length;
				pst_DirFileDesc = (CDV_tdst_DIR *) &sasz_ReadBuffer[i_BuffIndex];
			}
		}
	}

	iopDbg_M_Msg(iopDbg_Msg_0021);
	return 0;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int iopCDV_si_CheckResult(int _i_result)
{
	/*~~~~*/
	int err;
	/*~~~~*/

	if(_i_result == 1)
		return 0;
	else
	{
		err = sceCdGetError();
		switch(err)
		{
		case SCECdErFAIL:	iopDbg_M_Err(iopDbg_Err_003A); break;
		case SCECdErNO:		iopDbg_M_Err(iopDbg_Err_0040); break;
		case SCECdErEOM:	iopDbg_M_Err(iopDbg_Err_0041); break;
		case SCECdErTRMOPN: iopDbg_M_Err(iopDbg_Err_0042); break;
		case SCECdErREAD:	iopDbg_M_Err(iopDbg_Err_0043); break;
		case SCECdErPRM:	iopDbg_M_Err(iopDbg_Err_0044); break;
		case SCECdErILI:	iopDbg_M_Err(iopDbg_Err_0045); break;
		case SCECdErIPI:	iopDbg_M_Err(iopDbg_Err_0046); break;
		case SCECdErCUD:	iopDbg_M_Err(iopDbg_Err_0047); break;
		case SCECdErNORDY:	iopDbg_M_Err(iopDbg_Err_0048); break;
		case SCECdErNODISC: iopDbg_M_Err(iopDbg_Err_0049); break;
		case SCECdErOPENS:	iopDbg_M_Err(iopDbg_Err_004A); break;
		case SCECdErCMD:	iopDbg_M_Err(iopDbg_Err_004B); break;
		case SCECdErABRT:	iopDbg_M_Err(iopDbg_Err_004C); break;
		default:			iopDbg_M_Err(iopDbg_Err_004D); break;
		}

		return -1;
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* PSX2_USE_iopCDV */
#endif /* PSX2_IOP */
