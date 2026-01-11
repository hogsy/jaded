/*$T eeCDV_Manager.c GC! 1.097 01/11/01 14:44:52 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
/*$off*/
/*
 * CdSearchFile Replacement plus other low level CD stuff £
 *
 * NOTES The new CdSearchFile can find files on an ISO image without the limitations 
 * of the original CdSearchFile. Note that however the maximum number of directories 
 * that it can search is 256, however you can have as many files as you want in each 
 * directory. A limitation of ISO 9660 means that the maximum depth of directories is 8.
 * 
 * The new CdSearchFile also returns just the position of the file and the
 * length in bytes. This is to conserve space as to convert to a value that can be
 * used just involves a call to CdIntToPos().
 * Normal format is: 
 *  Sectors 0 - 15  Used by device defined in System ID 
 *          16      PVD - primary volume descriptor 
 *          17      PVDT - primary volume decriptor terminator 
 *          18      le path table 
 *          19      optional le path table
 *          20      be path table 
 *          21      be optional path table 
 *          22      Root Directory location 
 */
 /*$on*/

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#include "CDV_Manager.h"

#if defined(PSX2_USE_iopCDV) 


/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include <stdio.h>
#include <eekernel.h>
#include <string.h>
#include <malloc.h>
#include <libcdvd.h>
#include "IOP/ee/eeDebug.h"
#include "IOP/RPC_Manager.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    private macros
 ***********************************************************************************************************************
 */

#ifdef _DEBUG
#define M_CheckSceResult	eeCDV_si_dbg_CheckResult
#else
#define M_CheckSceResult(_res)	((_res == 1) ? 0 : -1)
#endif

/*$4
 ***********************************************************************************************************************
    private prototypes
 ***********************************************************************************************************************
 */

static int	eeCDV_si_SyncRead(unsigned int _ui_SectorsNb, unsigned int _ui_1stSector, char *_pc_Buffer);
int eeCDV_i_TellFile(int _i_FileHandler);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    debug fonctions
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef _DEBUG
static int	eeCDV_si_dbg_CheckResult(int _i_result);
int eeDbg_i_CheckISO(void);
int eeDbg_i_CheckDisk(void);
#endif


/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

static int						si_DiscAvailable;
static CDV_tdst_FileHandler		sax_FileHandler[CDV_Cte_MaxFileNb];
static unsigned int				sui_DirectSeek;
static unsigned int				sui_DirectSeekOct;

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    CD initialisation function
 =======================================================================================================================
 */
int eeCDV_i_InitModule(void)
{
	/* init of global variables */
	L_memset(sax_FileHandler, 0, CDV_Cte_MaxFileNb * sizeof(CDV_tdst_FileHandler));
	sui_DirectSeekOct = sui_DirectSeek = 0;

	/* if iop code, then init IOP */
	eeDbg_M_Msg(eeDbg_Msg_0003);
	if(eeRPC_i_PushCommand(RPC_Cmd_CDVInitModule, NULL, 0, NULL, 0))
		si_DiscAvailable = 0;
	else
		si_DiscAvailable = 1;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeCDV_i_CloseModule(void)
{
	L_memset(sax_FileHandler, 0, CDV_Cte_MaxFileNb * sizeof(CDV_tdst_FileHandler));
	eeRPC_i_PushCommand(RPC_Cmd_CDVCloseModule,NULL, 0, NULL, 0);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeCDV_i_OpenFile(char *asz_FileName)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						id;
	int						ko;
	int						size;
	CDV_tdst_FileHandler	st_TmpHandler;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!si_DiscAvailable) return -1;

	id = eeRPC_i_PushCommand
		(
			RPC_Cmd_CDVOpenFile,
			asz_FileName,
			strlen(asz_FileName) + 1,
			&st_TmpHandler,
			sizeof(CDV_tdst_FileHandler)
		);
	if(id < 0)
	{
		return -1;
	}

	sax_FileHandler[id] = st_TmpHandler;

	sax_FileHandler[id].puc_Buffer = (unsigned char *) MEM_p_AllocAlign(CDV_Cte_DefaultBuffSize, 64);
	if(sax_FileHandler[id].puc_Buffer == NULL)
	{
		eeCDV_i_CloseFile(id);
		return -1;
	}

	sax_FileHandler[id].ui_BufferSize = CDV_Cte_DefaultBuffSize;
	size = sax_FileHandler[id].ui_BufferSize / CDV_Cte_SectorSize;
	sax_FileHandler[id].ui_FileSeek = 0;
	sax_FileHandler[id].ui_BufferSeek = 0;
	sax_FileHandler[id].ui_Flags = 0;

	ko = eeCDV_si_SyncRead
		(
			size,
			sax_FileHandler[id].ui_StartSector + sax_FileHandler[id].ui_FileSeek,
			sax_FileHandler[id].puc_Buffer
		);
	if(ko)
	{
		eeCDV_i_CloseFile(id);
		return -1;
	}

	sax_FileHandler[id].ui_FileSeek = size;
#if 0
	scePrintf("##EE open file:\n");
	scePrintf("##EE name           : %s\n", sax_FileHandler[id].asz_Name);
	scePrintf("##EE ui_StartSector : %d\n", sax_FileHandler[id].ui_StartSector);
	scePrintf("##EE ui_SectorNb    : %d\n", sax_FileHandler[id].ui_SectorNb);
	scePrintf("##EE ui_FileSize    : %d\n", sax_FileHandler[id].ui_FileSize);
	scePrintf("##EE ui_FileSeek    : %d\n", sax_FileHandler[id].ui_FileSeek);
	scePrintf("##EE puc_Buffer     : %x\n", (int) sax_FileHandler[id].puc_Buffer);
	scePrintf("##EE ui_BufferSize  : %d\n", sax_FileHandler[id].ui_BufferSize);
	scePrintf("##EE ui_BufferSeek  : %d\n", sax_FileHandler[id].ui_BufferSeek);
	scePrintf("##EE file opened !!\n");
#endif
	return id;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeCDV_i_ReadFile(int _i_FileHandler, char *_pc_Buff, int _i_Size)
{
	/*~~~~~~~*/
	int remain;
	/*~~~~~~~*/

	if(!si_DiscAvailable) return -1;

	if(_i_Size > (sax_FileHandler[_i_FileHandler].ui_FileSize - eeCDV_i_TellFile(_i_FileHandler)))
	{
	    /* if we want to read out of the file : saturation */ 
		_i_Size = (sax_FileHandler[_i_FileHandler].ui_FileSize - eeCDV_i_TellFile(_i_FileHandler));
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

	while(_i_Size > (sax_FileHandler[_i_FileHandler].ui_BufferSize-sax_FileHandler[_i_FileHandler].ui_BufferSeek))
	{
		remain = sax_FileHandler[_i_FileHandler].ui_BufferSize / CDV_Cte_SectorSize;
		eeCDV_si_SyncRead
		(
			remain,
			sax_FileHandler[_i_FileHandler].ui_StartSector + sax_FileHandler[_i_FileHandler].ui_FileSeek,
			sax_FileHandler[_i_FileHandler].puc_Buffer
		);
		sax_FileHandler[_i_FileHandler].ui_FileSeek += remain;
		
		L_memcpy
		(
			_pc_Buff,
			sax_FileHandler[_i_FileHandler].puc_Buffer + sax_FileHandler[_i_FileHandler].ui_BufferSeek,
			(sax_FileHandler[_i_FileHandler].ui_BufferSize - sax_FileHandler[_i_FileHandler].ui_BufferSeek)
		);
		_i_Size -= (sax_FileHandler[_i_FileHandler].ui_BufferSize - sax_FileHandler[_i_FileHandler].ui_BufferSeek);
		_pc_Buff += (sax_FileHandler[_i_FileHandler].ui_BufferSize - sax_FileHandler[_i_FileHandler].ui_BufferSeek);
		
		sax_FileHandler[_i_FileHandler].ui_BufferSeek = 0;				
	}

	if(_i_Size > 0)
	{
		remain = sax_FileHandler[_i_FileHandler].ui_BufferSize / CDV_Cte_SectorSize;

		eeCDV_si_SyncRead
		(
			remain,
			sax_FileHandler[_i_FileHandler].ui_StartSector + sax_FileHandler[_i_FileHandler].ui_FileSeek,
			sax_FileHandler[_i_FileHandler].puc_Buffer
		);

		sax_FileHandler[_i_FileHandler].ui_FileSeek += remain;
		L_memcpy
		(
			_pc_Buff,
			sax_FileHandler[_i_FileHandler].puc_Buffer + sax_FileHandler[_i_FileHandler].ui_BufferSeek,
			_i_Size
		);
		sax_FileHandler[_i_FileHandler].ui_BufferSeek += _i_Size;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeCDV_i_DirectReadFile(int _i_FileHandler, char **_pc_Buff, int _i_Size)
{
	/*~~~~~*/
	int size;
	/*~~~~~*/

	if(!si_DiscAvailable) return -1;

	size = (_i_Size + CDV_Cte_SectorSize - 1) / CDV_Cte_SectorSize;
	if(sui_DirectSeekOct) size++;
	eeCDV_si_SyncRead(size, sax_FileHandler[_i_FileHandler].ui_StartSector + sui_DirectSeek, *_pc_Buff);
	*_pc_Buff += sui_DirectSeekOct;
	sui_DirectSeek += size;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeCDV_i_DirectSeekFile(unsigned int pos)
{
	if(!si_DiscAvailable) return -1;

	sui_DirectSeek = pos / CDV_Cte_SectorSize;
	sui_DirectSeekOct = (pos - (sui_DirectSeek * CDV_Cte_SectorSize));
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeCDV_i_SeekFile(int _i_FileHandler, int origin,unsigned int pos)
{
	if(!si_DiscAvailable) return -1;

	if(origin == CDV_Cte_SeekCur)
		pos += eeCDV_i_TellFile(_i_FileHandler);
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
			(sax_FileHandler[_i_FileHandler].ui_FileSeek *
				CDV_Cte_SectorSize) +
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
int eeCDV_i_GetFileSize(int _i_FileHandler)
{
	if(!si_DiscAvailable) return -1;
	return sax_FileHandler[_i_FileHandler].ui_FileSize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeCDV_i_TellFile(int _i_FileHandler)
{
	/*~~~~*/
	int ret;
	/*~~~~*/

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
int eeCDV_i_CloseFile(int _i_FileHandler)
{
    int data;
	if(!si_DiscAvailable) return -1;

    data = _i_FileHandler;
	eeRPC_i_PushCommand(RPC_Cmd_CDVCloseFile, &data, sizeof(int), NULL, 0);

	sax_FileHandler[_i_FileHandler].asz_Name[0] = '\0';
	sax_FileHandler[_i_FileHandler].ui_StartSector = 0;
	sax_FileHandler[_i_FileHandler].ui_SectorNb = 0;
	sax_FileHandler[_i_FileHandler].ui_FileSize = 0;
	sax_FileHandler[_i_FileHandler].ui_FileSeek = 0;
	sax_FileHandler[_i_FileHandler].ui_Flags = 0;
	if(sax_FileHandler[_i_FileHandler].puc_Buffer) MEM_FreeAlign(sax_FileHandler[_i_FileHandler].puc_Buffer);
	sax_FileHandler[_i_FileHandler].puc_Buffer = NULL;
	sax_FileHandler[_i_FileHandler].ui_BufferSize = 0;
	sax_FileHandler[_i_FileHandler].ui_BufferSeek = 0;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeCDV_i_GeteeBufferSize(int _i_FileHandler)
{
	if(!si_DiscAvailable) return -1;
	return sax_FileHandler[_i_FileHandler].ui_BufferSize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeCDV_i_SeteeBufferSize(int _i_FileHandler, int _i_NewSize)
{
	/*~~~~~~~~~~~~~~~~~*/
	unsigned char	*tmp;
	/*~~~~~~~~~~~~~~~~~*/

	if(!si_DiscAvailable) return -1;

	_i_NewSize = ((_i_NewSize + CDV_Cte_SectorSize - 1) / CDV_Cte_SectorSize) * CDV_Cte_SectorSize;
	tmp = MEM_p_AllocAlign(_i_NewSize, 64);
	if(tmp == NULL) return -1;
	L_memcpy(tmp, sax_FileHandler[_i_FileHandler].puc_Buffer, sax_FileHandler[_i_FileHandler].ui_BufferSize);
	MEM_FreeAlign(sax_FileHandler[_i_FileHandler].puc_Buffer);
	sax_FileHandler[_i_FileHandler].puc_Buffer = tmp;
	sax_FileHandler[_i_FileHandler].ui_BufferSize = _i_NewSize;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int eeCDV_si_SyncRead(unsigned int _ui_SectorsNb, unsigned int _ui_1stSector, char *_pc_Buffer)
{
	/*~~~~~~~*/
	int res, i;
	int tmp[2];
	int nb;
	unsigned int ui_MaxSectNb;
	/*~~~~~~~*/

	if(!si_DiscAvailable) return -1;

	res = 0;
	do {
    	ui_MaxSectNb = RPC_Cte_UserSize_Srv / CDV_Cte_SectorSize;
    	nb = (_ui_SectorsNb ) / ui_MaxSectNb;
    	for(i=0; i<nb; i++)
    	{
    		tmp[0] = ui_MaxSectNb;
    		tmp[1] = _ui_1stSector+ui_MaxSectNb*i;
    		res |= eeRPC_i_PushCommand(RPC_Cmd_CDVReadFile, tmp, 2 * sizeof(int), _pc_Buffer + (ui_MaxSectNb*CDV_Cte_SectorSize*i), ui_MaxSectNb * CDV_Cte_SectorSize);
    		_ui_SectorsNb -= ui_MaxSectNb;
    	}

    	
    	if(_ui_SectorsNb)
    	{
    		tmp[0] = _ui_SectorsNb;
    		tmp[1] = _ui_1stSector+ui_MaxSectNb*i;
    		res |= eeRPC_i_PushCommand(RPC_Cmd_CDVReadFile, tmp, 2 * sizeof(int), _pc_Buffer + (ui_MaxSectNb*CDV_Cte_SectorSize*i), _ui_SectorsNb * CDV_Cte_SectorSize);
    	}
    	
    	/*if(res)
    	{
    	    extern void GSP_DisplayMessageFullScreen(char*);
            GSP_DisplayMessageFullScreen("Disc Error, retrying...");
    	}*/
    } while(res);	
    
    return 0;
}

/*$4
 ***********************************************************************************************************************
    private functions
 ***********************************************************************************************************************
 */



/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeCDV_i_GetiopBufferSize(int _i_FileHandler)
{
    int data;
	if(!si_DiscAvailable) return -1;
	data = _i_FileHandler;
	return eeRPC_i_PushCommand(RPC_Cmd_CDVGetBufferSize, &data, sizeof(int), NULL, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeCDV_i_SetiopBufferSize(int _i_FileHandler, int _i_NewSize)
{
	/*~~~~~~~*/
	int tmp[2];
	/*~~~~~~~*/

	if(!si_DiscAvailable) return -1;
	tmp[0] = _i_FileHandler;
	tmp[1] = _i_NewSize;
	return eeRPC_i_PushCommand(RPC_Cmd_CDVSetBufferSize, tmp, 2 * sizeof(int), NULL, 0);
}


#ifdef __cplusplus
}
#endif
#endif /* PSX2_USE_iopCDV */
