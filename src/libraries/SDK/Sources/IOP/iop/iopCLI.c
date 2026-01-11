/*$T iopCLI.c GC 1.138 06/07/04 11:16:28 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP
#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include <stdio.h>
#include <sys/fcntl.h>
#include <kernel.h>
#include <memory.h>

#include "iop/iopBAStypes.h"

#include "iopDebug.h"
#include "iopCLI.h"
#include "CDV_Manager.h"
#include "iopMEM.h"
#include "iopMain.h"
#include "RPC_Manager.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "iopSND.h"
#include "iopSND_Voice.h"
#include "iopSND_Hard.h"
#include "iopSND_DmaScheduler.h"
#include "iopSND_Stream.h"

#include "SouND/Sources/MTX.h"
#include "iop/iopMTX.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define IOP_M_GetStrOrReturn(Idx, Ptr, RetCode) \
	do \
	{ \
		if(Idx < 0) return RetCode; \
		if(IOP_Cte_StrFileNb <= Idx) return RetCode; \
		Ptr = &IOP_gast_StrFile[Idx]; \
	} while(0);

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	iopCLI_tdst_CacheLine_
{
	unsigned int volatile	ui_TrueSize;
	unsigned int volatile	ui_TruePosition;
	unsigned int volatile	ui_Offset;
	char					*pc_Buffer;
} iopCLI_tdst_CacheLine;

typedef struct	iopCLI_tdst_Stream_
{
	unsigned int			ui_VirtualPosition;
	unsigned int			ui_SizeToRead;
	unsigned int volatile	ui_ReadSize;

	unsigned int volatile	ui_WriteIndex;
	unsigned int volatile	ui_ReadIndex;
	unsigned int			ui_CacheLineNumber;
	unsigned int			ui_CacheLineSize;
	iopCLI_tdst_CacheLine	*ast_Cache;
} iopCLI_tdst_Stream;

typedef struct	iopCLI_tdst_MuxStream_
{
	int						i_FileHandler;
	unsigned int			ui_FilePosition;
	unsigned int			ui_FileSize;

	unsigned int volatile	ui_CurrentPosition;

	unsigned int			ui_StreamIsMux;
	iopCLI_tdst_Stream		*pst_Video;
	iopCLI_tdst_Stream		*pst_Sound;
	iopCLI_tdst_Stream		*pst_Data;
	iopCLI_tdst_Stream		*pst_Sound2;

	int						i_SoundSoftBufferId;
	int						i_Volume;
	unsigned int			ui_Frequency;
	int						i_SoundIsPlaying;
} iopCLI_tdst_MuxStream;

/*$4
 ***********************************************************************************************************************
    extern variables
 ***********************************************************************************************************************
 */

extern int	iopCLI_Sema;
extern int	iopCLI_FileStreamingSema;

/*$4
 ***********************************************************************************************************************
    global variables
 ***********************************************************************************************************************
 */

volatile iopCLI_tdst_ReadRequest	iopCLI_gast_Buff[RPC_Cte_CLIMaxRequestNumber];
int									iopCLI_gast_RessourceBuff[RPC_Cte_CLIMaxRunningRequestNumber];
volatile int						iopCLI_gi_ResetAll;
IOP_tdst_StrFile					IOP_gast_StrFile[IOP_Cte_StrFileNb];


/*$4
 ***********************************************************************************************************************
    proto
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    asm function for optimizing speed
 ***********************************************************************************************************************
 */


/*$off*/

asm void *iopMemSet_asm(char *dst, unsigned char c, int size)
{
	add		v0, a0, $0;		sll		t1, a1, 8;
	beq		a2, $0, l_send; sltiu	t3, a2, 4;
	bne		t3, $0, l_set1; or		t2, t1, a1;
	sll		t3, t2, 16;		or		t0, t3, t2

l_set4:
	swr		t0, 0(a0);		addiu	a2, a2, -4
	sltiu	t1, a2, 4;		swl		t0, 3(a0);
	beq		t1, $0, l_set4; addiu	a0, a0, 4
	beq		a2, $0, l_send; nop

l_set1:
	addiu	a2, a2, -1;		sb		a1, 0(a0)
	bne		a2, $0, l_set1; addiu	a0, a0, 1

l_send:
	jr		ra;				nop
}
/*$on*/



#ifdef TRACE_OVERWRITE
void *iopMemSet(char * p, unsigned char c, int size, char*file, int line)
{

    if((int)p<(int)start)
    {
        printf("[IOP][iopMemSet] %x tentative ecrasement ptr %x size %d %s(%d)\n",start, (int)p, size, file, line);
    }


    iopMemSet_asm(p, c, size);
    return p;
}


void *iopMemCpy(char *p1, char *p2, int size)
{
    if((int)p1<(int)start)
    {
        printf("[IOP][iopMemCpy] %x tentative ecrasement ptr %x size %d %s(%d)\n",start, (int)p1, size, file, line);
    }

    memcpy(p1, p2, size);
    return p1;
}
#endif



/*$4
 ***********************************************************************************************************************
    function
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopCLI_InitModule(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < RPC_Cte_CLIMaxRunningRequestNumber; i++)
	{
		iopCLI_gast_RessourceBuff[i] = (int) iopMEM_pv_iopAllocAlign(iopCLI_Cte_PreLoadSize, 64);
		if(!iopCLI_gast_RessourceBuff[i])
		{
			iopDbg_M_Err(iopDbg_Err_0082);
		}
		else
		{
			iopCLI_gast_RessourceBuff[i] |= 1;
		}
	}

	for(i = 0; i < RPC_Cte_CLIMaxRequestNumber; i++)
	{
		iopCLI_gast_Buff[i].i_FileHandler = -1;
		iopCLI_gast_Buff[i].ui_FileSeek = 0;
		iopCLI_gast_Buff[i].i_BufferSize = 0;
		iopCLI_gast_Buff[i].i_Status = iopCLI_Cte_RqFree;
		iopCLI_gast_Buff[i].i_RdId = -1;

		iopCLI_gast_Buff[i].pc_FileBuffer = NULL;
		iopCLI_gast_Buff[i].pc_WriteBuffer = NULL;
	}

	iopCLI_gi_ResetAll = 0;

	L_memset(IOP_gast_StrFile, 0, IOP_Cte_StrFileNb * sizeof(IOP_tdst_StrFile));
	
    IOP_gast_StrFile[0].pc_StrBuffer = iopMEM_pv_iopAllocAlign(SND_Cte_MaxBufferSize>>1, 64);
    IOP_gast_StrFile[1].pc_StrBuffer = iopMEM_pv_iopAllocAlign(SND_Cte_MaxBufferSize>>1, 64);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *iopCLI_pc_AllocBuffer(void)
{
	/*~~~~~~~~*/
	int		i;
	char	*pc;
	/*~~~~~~~~*/

	pc = NULL;

	for(i = 0; i < RPC_Cte_CLIMaxRunningRequestNumber; i++)
	{
		if(iopCLI_gast_RessourceBuff[i] & 1)
		{
			iopCLI_gast_RessourceBuff[i] &= 0xFFFFFFFE;
			pc = (char *) iopCLI_gast_RessourceBuff[i];
			break;
		}
	}

	return pc;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopCLI_FreeBuffer(char *pc)
{
	/*~~*/
	int i;
	/*~~*/

	if(!pc) return;

	for(i = 0; i < RPC_Cte_CLIMaxRunningRequestNumber; i++)
	{
		if(iopCLI_gast_RessourceBuff[i] == (int) pc)
		{
			iopCLI_gast_RessourceBuff[i] |= 1;
			break;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopCLI_OpenFile(char *_pc_name)
{
	return L_fopen(_pc_name);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopCLI_CloseFile(int _i_Id)
{
	L_fclose(_i_Id);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopCLI_i_AddRequestReadFile(int _i_Rq, int _i_fd, unsigned int _ui_Pos, int _i_size)
{
	if(_i_fd < 0) return -1;
	if(_i_Rq < 0) return -1;
	if(_i_Rq >= RPC_Cte_CLIMaxRequestNumber) return -1;
	iopCLI_gi_ResetAll = 0;

	iopCLI_gast_Buff[_i_Rq].i_FileHandler = _i_fd;
	iopCLI_gast_Buff[_i_Rq].ui_FileSeek = _ui_Pos;
	iopCLI_gast_Buff[_i_Rq].i_BufferSize = _i_size;
	iopCLI_gast_Buff[_i_Rq].i_RdId = _i_Rq;
	iopCLI_gast_Buff[_i_Rq].i_Status = iopCLI_Cte_RqReading;
	iopCLI_gast_Buff[_i_Rq].pc_FileBuffer = iopCLI_pc_AllocBuffer();
	iopCLI_gast_Buff[_i_Rq].pc_WriteBuffer = iopCLI_gast_Buff[_i_Rq].pc_FileBuffer + 4;

	iopSND_RequestWrite(0, iopSND_Cte_DmaCLIRead0 + _i_Rq);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopCLI_ResetAllRequestReadFile(void)
{
	/*~~*/
	int i;
	/*~~*/

	iopCLI_gi_ResetAll = 1;
	iopSND_RequestDelete(0);
	for(i = 0; i < RPC_Cte_CLIMaxRequestNumber; i++)
	{
		if(iopCLI_gast_Buff[i].pc_FileBuffer)
		    iopCLI_FreeBuffer(iopCLI_gast_Buff[i].pc_FileBuffer);
		
		iopCLI_gast_Buff[i].i_FileHandler = -1;
		iopCLI_gast_Buff[i].ui_FileSeek = 0;
		iopCLI_gast_Buff[i].i_BufferSize = 0;
		iopCLI_gast_Buff[i].i_Status = iopCLI_Cte_RqFree;
		iopCLI_gast_Buff[i].i_RdId = -1;
		iopCLI_gast_Buff[i].pc_FileBuffer = NULL;
		iopCLI_gast_Buff[i].pc_WriteBuffer = NULL;
	}

	iopCLI_gi_ResetAll = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopCLI_FileSystemExec(int _id)
{
	/*~~~~*/
	int res;
	/*~~~~*/

	if(_id < 0) return;
	if(_id >= RPC_Cte_CLIMaxRequestNumber) return;
	if(iopCLI_gi_ResetAll) return;

	if(iopCLI_gast_Buff[_id].i_Status != iopCLI_Cte_RqReading)
	{
		iopCLI_gast_Buff[_id].i_Status = iopCLI_Cte_RqFailed;
		iopDbg_M_Err(iopDbg_Err_0082 "-2");
	}
	else
	{
		res = iopMTX_i_Read
			(
				iopCLI_gast_Buff[_id].i_FileHandler,
				iopCLI_gast_Buff[_id].ui_FileSeek,
				(char **) &iopCLI_gast_Buff[_id].pc_WriteBuffer,
				iopCLI_gast_Buff[_id].i_BufferSize,
				iopMTX_Cte_VideoStream
			);

		if(res < 0)
		{
			CDV_Dbg_Trace("pee");
			res = L_seekandread
				(
					iopCLI_gast_Buff[_id].i_FileHandler,
					iopCLI_gast_Buff[_id].ui_FileSeek,
					iopCLI_gast_Buff[_id].pc_WriteBuffer,
					iopCLI_gast_Buff[_id].i_BufferSize
				);
			CDV_Dbg_Trace("iop");
		}

		if(res)
			iopCLI_gast_Buff[_id].i_Status = iopCLI_Cte_RqFailed;
		else
			iopCLI_gast_Buff[_id].i_Status = iopCLI_Cte_RqSucceed;
	}

	if(iopCLI_gi_ResetAll)
	{
		iopCLI_ResetAllRequestReadFile();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopCLI_i_GetFileRequest(int _i_RqId, char **_pc_buff)
{
	if(_i_RqId < 0) return -1;
	if(_i_RqId >= RPC_Cte_CLIMaxRequestNumber) return -1;
	if(!_pc_buff) return -1;

	switch(iopCLI_gast_Buff[_i_RqId].i_Status)
	{
	case iopCLI_Cte_RqFree:
		return 0;

	case iopCLI_Cte_RqReading:
		return 1;

	case iopCLI_Cte_RqSucceed:
		*_pc_buff = (char *) iopCLI_gast_Buff[_i_RqId].pc_FileBuffer;
		*(int *) (*_pc_buff) = 0;
		iopCLI_gast_Buff[_i_RqId].i_FileHandler = -1;
		iopCLI_gast_Buff[_i_RqId].ui_FileSeek = 0;
		iopCLI_gast_Buff[_i_RqId].i_BufferSize = 0;
		iopCLI_gast_Buff[_i_RqId].i_RdId = -1;
		iopCLI_gast_Buff[_i_RqId].i_Status = iopCLI_Cte_RqFree;
		iopCLI_FreeBuffer(iopCLI_gast_Buff[_i_RqId].pc_FileBuffer);
		iopCLI_gast_Buff[_i_RqId].pc_WriteBuffer = NULL;
		iopCLI_gast_Buff[_i_RqId].pc_FileBuffer = NULL;;
		return 0;

	default:

	case iopCLI_Cte_RqFailed:
		iopCLI_gast_Buff[_i_RqId].i_FileHandler = -1;
		iopCLI_gast_Buff[_i_RqId].ui_FileSeek = 0;
		iopCLI_gast_Buff[_i_RqId].i_BufferSize = 0;
		iopCLI_gast_Buff[_i_RqId].i_RdId = -1;
		iopCLI_gast_Buff[_i_RqId].i_Status = iopCLI_Cte_RqFree;
		iopCLI_FreeBuffer(iopCLI_gast_Buff[_i_RqId].pc_FileBuffer);
		iopCLI_gast_Buff[_i_RqId].pc_WriteBuffer = NULL;
		iopCLI_gast_Buff[_i_RqId].pc_FileBuffer = NULL;;
		return -1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopCLI_i_FileExecRead(void *pVa, char *pc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	RPC_tdst_FileExecAsyncRead	*pVargs;
	int							res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pVargs = (RPC_tdst_FileExecAsyncRead *) pVa;

	CDV_Dbg_Trace("p1e");
	res = L_seekandread(pVargs->i_FileId, pVargs->ui_Position, pc, pVargs->ui_Size);
	CDV_Dbg_Trace("iop");
	if(res) return -1;
	return 0;
}

/*$4
 ***********************************************************************************************************************
    stream files
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void IOP_StrFileFree(int i)
{
	if(i < 0) return;
	if(i >= IOP_Cte_StrFileNb) return;

	IOP_gast_StrFile[i].us_Allocated = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int IOP_i_StrFileInit(IOP_tdst_StrFile *_pStr)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < IOP_Cte_StrFileNb; i++)
	{
		if(IOP_gast_StrFile[i].us_Allocated) continue;

        IOP_gast_StrFile[i].us_Allocated = 1;
		IOP_gast_StrFile[i].i_FileHandler = _pStr->i_FileHandler;
		IOP_gast_StrFile[i].ui_FilePosition = _pStr->ui_FilePosition;
		IOP_gast_StrFile[i].ui_FileSize = _pStr->ui_FileSize * _pStr->us_Channel;

		
			IOP_gast_StrFile[i].ui_BufferSize = _pStr->ui_BufferSize;
			IOP_gast_StrFile[i].ui_MuxFrameSize = _pStr->ui_MuxFrameSize;
			IOP_gast_StrFile[i].us_Channel = _pStr->us_Channel;

			if(IOP_gast_StrFile[i].ui_MuxFrameSize)
			{
				iopDbg_M_Assert(IOP_gast_StrFile[i].us_Channel, iopDbg_Err_0093);
				IOP_gast_StrFile[i].ui_BufferSize = IOP_gast_StrFile[i].ui_MuxFrameSize * IOP_gast_StrFile[i].us_Channel;
			}
			else if(IOP_gast_StrFile[i].ui_BufferSize == 0)
			{
				IOP_gast_StrFile[i].ui_BufferSize = 50 * 1024;
			}
            
			if(!IOP_gast_StrFile[i].pc_StrBuffer)
			{
				iopDbg_M_Err(iopDbg_Err_0092);
				IOP_gast_StrFile[i].us_Allocated = 0;
				return -1;
			}
		

		iopDbg_M_Assert(IOP_gast_StrFile[i].pc_StrBuffer && IOP_gast_StrFile[i].ui_BufferSize, iopDbg_Err_0093);

		/* we can use the buffer contents */
		IOP_gast_StrFile[i].ui_FileCurrSeek = IOP_gast_StrFile[i].ui_FilePosition + IOP_gast_StrFile[i].ui_FileSize;

		if(IOP_gast_StrFile[i].ui_MuxFrameSize)
		{
			iopDbg_M_Assert(IOP_gast_StrFile[i].us_Channel, iopDbg_Err_0093);
			iopDbg_M_Assert
			(
				IOP_gast_StrFile[i].ui_BufferSize == (IOP_gast_StrFile[i].us_Channel * IOP_gast_StrFile[i].ui_MuxFrameSize),
				iopDbg_Err_0093
			);
		}

		return i;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int IOP_i_StrFileReinit(int i, unsigned int ui_FilePosition, unsigned int ui_FileSize)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	IOP_tdst_StrFile	*pStr;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	IOP_M_GetStrOrReturn(i, pStr, 0);

	if(!pStr->us_Allocated) 
	{
	return -1;
	}

	pStr->ui_FilePosition = ui_FilePosition;
	pStr->ui_FileSize = ui_FileSize * pStr->us_Channel;

	iopDbg_M_Assert(pStr->pc_StrBuffer && pStr->ui_BufferSize, iopDbg_Err_0093);

	/* we can use the buffer contents */
	pStr->ui_FileCurrSeek = pStr->ui_FilePosition + pStr->ui_FileSize;

	if(pStr->ui_MuxFrameSize)
	{
		iopDbg_M_Assert(pStr->us_Channel, iopDbg_Err_0093);
		iopDbg_M_Assert(pStr->ui_BufferSize == (pStr->us_Channel * pStr->ui_MuxFrameSize), iopDbg_Err_0093);
	}

	return 0;
}

/*
 =======================================================================================================================
    <file_seek> is for one channel <pc_buffer> will contain all channels <ui_size> is readen by channels
 =======================================================================================================================
 */
int IOP_StrFileRead(int id, unsigned int _ui_Seek, char *_pc_Buffer, unsigned int _ui_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	IOP_tdst_StrFile	*pStr;
	unsigned int		uiRead, uiFrame, uiBlock;
	unsigned int		uiSizeByChannel;
	unsigned int		i;
	unsigned int		ui_FileCurrSeek, ui_BufferCurrSeek;
    unsigned int        uiPrefechedSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(_pc_Buffer, 0, _ui_Size);
	IOP_M_GetStrOrReturn(id, pStr, 0);

	if(pStr->ui_MuxFrameSize)
	{
		uiSizeByChannel = _ui_Size;

		/* check seek */
		if(_ui_Seek < pStr->ui_FilePosition)
		{
			return -1;
		}

		/* relative seeking */
		_ui_Seek -= pStr->ui_FilePosition;

		/* eval the frame idx */
		uiFrame = _ui_Seek / pStr->ui_MuxFrameSize;
		while(_ui_Seek >= pStr->ui_MuxFrameSize) _ui_Seek -= pStr->ui_MuxFrameSize;

		/* eval the frame seek */
		uiFrame *= pStr->ui_BufferSize; /* buffer size = channel x frame size */

		/* get seek */
		ui_FileCurrSeek = pStr->ui_FilePosition + uiFrame;
		ui_BufferCurrSeek = _ui_Seek;

		uiBlock = L_min
			(
				(pStr->ui_FileSize - (ui_FileCurrSeek - pStr->ui_FilePosition)) / pStr->us_Channel,
				pStr->ui_MuxFrameSize
			);

		/* read data (?) */
		if(ui_FileCurrSeek != pStr->ui_FileCurrSeek)
		{
		    
			pStr->ui_FileCurrSeek = ui_FileCurrSeek;
			
			uiPrefechedSize = iopSND_ui_StreamPrefetchGet(pStr->pc_StrBuffer, pStr->ui_FileCurrSeek, pStr->us_Channel * uiBlock);
			if(uiPrefechedSize < (pStr->us_Channel * uiBlock))
			{
			    L_seekandread(pStr->i_FileHandler, pStr->ui_FileCurrSeek+uiPrefechedSize, pStr->pc_StrBuffer+uiPrefechedSize, (pStr->us_Channel * uiBlock)-uiPrefechedSize);    
			}
		}

		/* copy to user buffer */
		uiRead = L_min(uiSizeByChannel, uiBlock - ui_BufferCurrSeek);
		for(i = 0; i < pStr->us_Channel; i++)
		{
			L_memcpy
			(
				_pc_Buffer + (i * pStr->ui_MuxFrameSize),
				pStr->pc_StrBuffer + ui_BufferCurrSeek + (i * uiBlock),
				uiRead
			);
		}

		_pc_Buffer += uiRead;
		uiSizeByChannel -= uiRead;

		/* any more ? */
		while(uiSizeByChannel)
		{
			/* goto next frame */
			pStr->ui_FileCurrSeek += pStr->ui_BufferSize;
			if(pStr->ui_FileCurrSeek >= (pStr->ui_FilePosition + pStr->ui_FileSize))
			{
				return 1;
			}

			/* read data */
			uiBlock = L_min
				(
					(pStr->ui_FileSize - (pStr->ui_FileCurrSeek - pStr->ui_FilePosition)) / pStr->us_Channel,
					pStr->ui_MuxFrameSize
				);
			
			uiPrefechedSize = iopSND_ui_StreamPrefetchGet(pStr->pc_StrBuffer, pStr->ui_FileCurrSeek, pStr->us_Channel * uiBlock);
			if(uiPrefechedSize < (pStr->us_Channel * uiBlock))
			{
			    L_seekandread(pStr->i_FileHandler, pStr->ui_FileCurrSeek+uiPrefechedSize, pStr->pc_StrBuffer+uiPrefechedSize, (pStr->us_Channel * uiBlock)-uiPrefechedSize);    
			}

			/* write data */
			uiRead = L_min(uiSizeByChannel, uiBlock);
			for(i = 0; i < pStr->us_Channel; i++)
			{
				L_memcpy(_pc_Buffer + (i * pStr->ui_MuxFrameSize), pStr->pc_StrBuffer + (i * uiBlock), uiRead);
			}

			uiSizeByChannel -= uiRead;
			_pc_Buffer += uiRead;
		}
	}
	else
	{
		if(_ui_Seek < pStr->ui_FilePosition)
		{
			return -1;
		}

		if(_ui_Seek > (pStr->ui_FilePosition + pStr->ui_FileSize))
		{
			return -1;
		}

		if((_ui_Seek + _ui_Size) > (pStr->ui_FilePosition + pStr->ui_FileSize))
		{
			uiRead = pStr->ui_FilePosition + pStr->ui_FileSize - _ui_Seek;
			
			uiPrefechedSize = iopSND_ui_StreamPrefetchGet(_pc_Buffer, _ui_Seek, uiRead);
			if(uiPrefechedSize < uiRead)
			{
			    L_seekandread(pStr->i_FileHandler, _ui_Seek+uiPrefechedSize, _pc_Buffer+uiPrefechedSize, uiRead-uiPrefechedSize);
			}
		}
		else
		{
		    uiPrefechedSize = iopSND_ui_StreamPrefetchGet(_pc_Buffer, _ui_Seek, _ui_Size);
			if(uiPrefechedSize < _ui_Size)
			{
			    L_seekandread(pStr->i_FileHandler, _ui_Seek+uiPrefechedSize, _pc_Buffer+uiPrefechedSize, _ui_Size-uiPrefechedSize);
			}
		}
	}

	return 0;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* PSX2_IOP */
