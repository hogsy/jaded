/*$T iopMTX.c GC! 1.081 07/29/02 17:29:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP

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
#include "iop/iopDebug.h"
#include "iop/iopCLI.h"
#include "iop/iopMEM.h"
#include "iop/iopMain.h"

#include "CDV_Manager.h"
#include "RPC_Manager.h"

#include "Sound/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "iop/iopSND.h"
#include "iop/iopSND_Voice.h"
#include "iop/iopSND_Hard.h"
#include "iop/iopSND_DmaScheduler.h"
#include "iop/iopSND_Stream.h"

#include "SouND/Sources/MTX.h"

#include "iop/iopMTX.h"

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */



typedef struct	iopMTX_tdst_MuxDesc_
{
	unsigned int	ui_DataPos;
	unsigned int	ui_DataSize;
	unsigned int	ui_BlockSize;
	unsigned int	ui_VirtualPos;
} iopMTX_tdst_MuxDesc;

typedef struct	iopMTX_tdst_MuxStream_
{
	int					i_MtxFileHandler;
	int					i_MtxPlayCount;
	unsigned int		ui_MtxDataSize;
	unsigned int		ui_MtxDataPos;
	unsigned int		ui_MtxFrameSize;

	iopMTX_tdst_MuxDesc *pst_Video;
	iopMTX_tdst_MuxDesc *pst_Sound1;
	iopMTX_tdst_MuxDesc *pst_Sound2;

	unsigned int		ui_SoundFrequency;
	int					i_SoundVolume;
	unsigned int		ui_SoundChannel;
	unsigned int		ui_SoundBuffer;
	int					i_SoundIsPlaying;
} iopMTX_tdst_MuxStream;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

#define M_SoundCacheLineNb    7
char * iopMTX_gap_SoundCache1[M_SoundCacheLineNb];
char * iopMTX_gap_SoundCache2[M_SoundCacheLineNb];
int RdIdx1,RdIdx2;
int WrIdx1,WrIdx2;

volatile iopMTX_tdst_MuxStream	*iopMTX_gpst_MuxStream;
unsigned int iopMTX_gui_VideoSeek=0;
unsigned int iopMTX_gui_VideoFrame=0;
//int dbg_repeat = 1;
unsigned short iopMTX_gus_Volume;


/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

static void						iopMTX_FreeMuxDesc(iopMTX_tdst_MuxDesc *);
static iopMTX_tdst_MuxDesc		*iopMTX_pst_AllocMuxDesc(void);
void							iopMTX_StartSound(void);
static void iopMTX_ReadSound(int _i_FileHandler);

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopMTX_InitModule(void)
{
	iopMTX_gpst_MuxStream = NULL;
	L_memset(iopMTX_gap_SoundCache1, 0, sizeof(char *)*M_SoundCacheLineNb);
	L_memset(iopMTX_gap_SoundCache2, 0, sizeof(char *)*M_SoundCacheLineNb);
	iopMTX_gus_Volume = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopMTX_CloseModule(void)
{
    int i;
	if(iopMTX_gpst_MuxStream) iopMTX_Close();
	iopMTX_gpst_MuxStream = NULL;
	
	for(i=0; i<M_SoundCacheLineNb; i++)
	{
    	if(iopMTX_gap_SoundCache1[i]) iopMEM_iopFree(iopMTX_gap_SoundCache1[i]);
    	iopMTX_gap_SoundCache1[i] = NULL;
    	
    	if(iopMTX_gap_SoundCache2[i]) iopMEM_iopFree(iopMTX_gap_SoundCache2[i]);
    	iopMTX_gap_SoundCache2[i] = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopMTX_i_Open(RPC_tdst_VArg_FileStreamCreate *pVArgs)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopMTX_tdst_MuxDesc		*pst_Video;
	iopMTX_tdst_MuxDesc		*pst_Sound;
	iopMTX_tdst_MuxStream	*pNew;
	int i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- check consistency ------------------------------------------------------------------------------------------*/

	if(pVArgs->i_FileHandler < 0) return -1;
	if(!pVArgs->ui_FilePos) return -1;
	if(!pVArgs->ui_FileSize) return -1;
	if(!pVArgs->i_FileIsMTX) return 0;

	/*$2- kill previous ----------------------------------------------------------------------------------------------*/

	if(iopMTX_gpst_MuxStream) iopMTX_Close();
	iopMTX_gui_VideoFrame=0;
	iopMTX_gui_VideoSeek = 0;
	RdIdx1=RdIdx2 = -3;
    WrIdx1=WrIdx2 = -1;
    iopMTX_gus_Volume = 0;


	/*$2- alloc pointer ----------------------------------------------------------------------------------------------*/

	pNew = iopMEM_pv_iopAlloc(sizeof(iopMTX_tdst_MuxStream));
	L_memset(pNew, 0, sizeof(iopMTX_tdst_MuxStream));

	/*$2- get settings -----------------------------------------------------------------------------------------------*/

	/* MTX file */
	pNew->i_MtxFileHandler = pVArgs->i_FileHandler;
	pNew->ui_MtxDataPos = pVArgs->ui_FilePos;
	pNew->ui_MtxDataSize = pVArgs->ui_FileSize;
	pNew->ui_MtxFrameSize = 0;
	pNew->i_SoundIsPlaying = 0;

	/*$2- video part -------------------------------------------------------------------------------------------------*/

	pst_Video = iopMTX_pst_AllocMuxDesc();

	if(!pst_Video)
	{
		iopDbg_M_Err(iopDbg_Err_0090);
		iopMEM_iopFree((void *) pNew);
		iopMTX_gpst_MuxStream = NULL;
		return -1;
	}

	pst_Video->ui_VirtualPos = pVArgs->ui_VideoPosition;
	pst_Video->ui_DataPos = pNew->ui_MtxDataPos+pVArgs->ui_VideoBufferSize;
	pst_Video->ui_DataSize = pVArgs->ui_VideoSize;
	pst_Video->ui_BlockSize = pVArgs->ui_VideoBufferSize;

	pNew->pst_Video = pst_Video;
	pNew->ui_MtxFrameSize = pst_Video->ui_BlockSize;

	/*$2- sound ------------------------------------------------------------------------------------------------------*/

	if(pVArgs->ui_SoundBufferSize)
	{
		pst_Sound = iopMTX_pst_AllocMuxDesc();

		if(!pst_Sound)
		{
			iopDbg_M_Err(iopDbg_Err_0090);
			iopMTX_FreeMuxDesc(pNew->pst_Video);
			iopMEM_iopFree((void *) pNew);
			iopMTX_gpst_MuxStream = NULL;
			return -1;
		}

		pst_Sound->ui_VirtualPos = pNew->ui_MtxDataPos;
		pst_Sound->ui_DataPos = pNew->ui_MtxDataPos;
		pst_Sound->ui_DataSize = pVArgs->ui_SoundSize;
		pst_Sound->ui_BlockSize = pVArgs->ui_SoundBufferSize;
		
    	for(i=0; i<M_SoundCacheLineNb; i++)
    	{
        	iopMTX_gap_SoundCache1[i] = iopMEM_pv_iopAlloc(SND_Cte_MaxBufferSize / 2);
        	if(!iopMTX_gap_SoundCache1[i])
        	{
    			while(i) 
    			{
    			    iopMEM_iopFree(iopMTX_gap_SoundCache1[--i]);
       			    iopMTX_gap_SoundCache1[i] = NULL;
    			} 
    			iopDbg_M_Err(iopDbg_Err_0090);
    			iopMTX_FreeMuxDesc(pNew->pst_Video);
    			iopMEM_iopFree((void *) pNew);
    			iopMTX_gpst_MuxStream = NULL;
    			return -1;
        	}
    	}
        
        WrIdx1=0;
		

		/* update video data pos */
		pst_Video->ui_DataPos += pst_Sound->ui_BlockSize;
		pst_Video->ui_DataPos += pst_Sound->ui_BlockSize;

		/* sound player settings */
		pNew->ui_SoundFrequency = pVArgs->ui_Frequency;
		pNew->i_SoundVolume = 0;//pVArgs->i_Volume;
		pNew->ui_SoundChannel = pVArgs->ui_SoundChannel;
		pNew->ui_SoundBuffer = 0x00000001;
		pNew->pst_Sound1 = pst_Sound;
		pNew->ui_MtxFrameSize += pst_Sound->ui_BlockSize;

		/*$2- stereo -------------------------------------------------------------------------------------------------*/

		if(pNew->ui_SoundChannel == 2)
		{
			pst_Sound = iopMTX_pst_AllocMuxDesc();
			if(!pst_Sound)
			{
				iopDbg_M_Err(iopDbg_Err_0090);
            	for(i=0; i<M_SoundCacheLineNb; i++)
            	{
                	iopMEM_iopFree(iopMTX_gap_SoundCache1[i]);
                	iopMTX_gap_SoundCache1[i] = NULL;
            	}

				iopMTX_FreeMuxDesc(pNew->pst_Video);
				iopMTX_FreeMuxDesc(pNew->pst_Sound2);
				iopMEM_iopFree((void *) pNew);
				iopMTX_gpst_MuxStream = NULL;
				return -1;
			}

			pst_Sound->ui_VirtualPos = pNew->pst_Sound1->ui_VirtualPos + pVArgs->ui_SoundBufferSize;
			pst_Sound->ui_DataPos = pNew->pst_Sound1->ui_DataPos + pVArgs->ui_SoundBufferSize;
			pst_Sound->ui_DataSize = pVArgs->ui_SoundSize;
			pst_Sound->ui_BlockSize = pVArgs->ui_SoundBufferSize;

        	for(i=0; i<M_SoundCacheLineNb; i++)
        	{
            	iopMTX_gap_SoundCache2[i] = iopMEM_pv_iopAlloc(SND_Cte_MaxBufferSize / 2);
            	if(!iopMTX_gap_SoundCache2[i])
            	{
    				iopDbg_M_Err(iopDbg_Err_0090);
        			while(i) 
        			{
        			    iopMEM_iopFree(iopMTX_gap_SoundCache1[--i]);
        			    iopMTX_gap_SoundCache1[i]=NULL;
        			} 

                	for(i=0; i<M_SoundCacheLineNb; i++)
                	{
                    	iopMEM_iopFree(iopMTX_gap_SoundCache1[i]);
                    	iopMTX_gap_SoundCache1[i] = NULL;
                	}

    				iopMTX_FreeMuxDesc(pNew->pst_Video);
    				iopMTX_FreeMuxDesc(pNew->pst_Sound2);
    				iopMEM_iopFree((void *) pNew);
    				iopMTX_gpst_MuxStream = NULL;
    				return -1;
			    }
        	}
        	WrIdx2 = 0;

			/* update video data pos */
			pst_Video->ui_DataPos += pst_Sound->ui_BlockSize;
			pst_Video->ui_DataPos += pst_Sound->ui_BlockSize;

			/* sound player settings */
			pNew->ui_SoundBuffer = 0x00020001;
			pNew->pst_Sound2 = pst_Sound;
			pNew->ui_MtxFrameSize += pst_Sound->ui_BlockSize;
		}
	}
	
	iopMTX_gui_VideoSeek = pst_Video->ui_DataPos;
	iopMTX_gui_VideoFrame = 0;
	iopMTX_gpst_MuxStream = pNew;


    if(iopMTX_gpst_MuxStream->pst_Sound1) 
    {
        

        iopMTX_gui_VideoSeek = iopMTX_gui_VideoSeek - iopMTX_gpst_MuxStream->ui_MtxFrameSize;
        if(iopMTX_gpst_MuxStream->pst_Sound1)
        {
            iopMTX_gui_VideoSeek = iopMTX_gui_VideoSeek - iopMTX_gpst_MuxStream->pst_Sound1->ui_BlockSize;
    	    RdIdx1 = -2;
        }
        if(iopMTX_gpst_MuxStream->pst_Sound2)
        {
            iopMTX_gui_VideoSeek = iopMTX_gui_VideoSeek - iopMTX_gpst_MuxStream->pst_Sound2->ui_BlockSize;
        	RdIdx2 = -2;
        }
        
        iopMTX_ReadSound(iopMTX_gpst_MuxStream->i_MtxFileHandler);

    	iopMTX_gui_VideoSeek = pst_Video->ui_DataPos;
        if(iopMTX_gpst_MuxStream->pst_Sound1) iopMTX_gui_VideoSeek = iopMTX_gui_VideoSeek - iopMTX_gpst_MuxStream->pst_Sound1->ui_BlockSize;
        if(iopMTX_gpst_MuxStream->pst_Sound2) iopMTX_gui_VideoSeek = iopMTX_gui_VideoSeek - iopMTX_gpst_MuxStream->pst_Sound2->ui_BlockSize;
        iopMTX_ReadSound(iopMTX_gpst_MuxStream->i_MtxFileHandler);

    	iopMTX_gui_VideoSeek = pst_Video->ui_DataPos;
	}

    //iopMTX_StartSound();
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopMTX_Close(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopMTX_tdst_MuxStream	*pNew;
	int i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!iopMTX_gpst_MuxStream) return;

	CLI_WaitSema(iopCLI_FileStreamingSema);
	pNew = (iopMTX_tdst_MuxStream *) iopMTX_gpst_MuxStream;
	iopMTX_gpst_MuxStream = NULL;
	CLI_SignalSema(iopCLI_FileStreamingSema);

	if(pNew->pst_Sound1)
	{
		iopSND_i_StopVoice(pNew->ui_SoundBuffer);
		iopMTX_FreeMuxDesc(pNew->pst_Sound1);		
	}

	if(pNew->pst_Sound2)
	{
		iopMTX_FreeMuxDesc(pNew->pst_Sound2);
	}

	if(pNew->pst_Video)
	{
		iopMTX_FreeMuxDesc(pNew->pst_Video);
	}

	for(i=0; i<M_SoundCacheLineNb; i++)
	{
    	if(iopMTX_gap_SoundCache1[i]) iopMEM_iopFree(iopMTX_gap_SoundCache1[i]);
    	iopMTX_gap_SoundCache1[i] = NULL;
    	
    	if(iopMTX_gap_SoundCache2[i]) iopMEM_iopFree(iopMTX_gap_SoundCache2[i]);
    	iopMTX_gap_SoundCache2[i] = NULL;
	}

	iopMEM_iopFree((void *) pNew);

	iopSND_StreamIopFlushAll();
	iopMTX_gui_VideoSeek=0;
	iopMTX_gui_VideoFrame = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopMTX_Reinit(RPC_tdst_VArg_FileStreamReinit *pVArgs)
{
#pragma unused(pVArgs)
    int i;
    
	if(!iopMTX_gpst_MuxStream) return;

	WrIdx1 = -1;
	RdIdx1 = -3;
	WrIdx2 = -1;
	RdIdx2 = -3;

	if(iopMTX_gpst_MuxStream->pst_Sound2)
	{
    	WrIdx2 = 0;
    	RdIdx2 = -2;
	}
	if(iopMTX_gpst_MuxStream->pst_Sound1)
	{
		iopMTX_gpst_MuxStream->i_SoundIsPlaying = 0;
    	WrIdx1 = 0;
    	RdIdx1 = -2;
		iopMTX_StartSound();
	}
	iopMTX_gui_VideoSeek = iopMTX_gpst_MuxStream->pst_Video->ui_DataPos;
	iopMTX_gui_VideoFrame = 0;


	for(i=0; i<M_SoundCacheLineNb; i++)
	{
    	if(iopMTX_gap_SoundCache1[i]) L_memset(iopMTX_gap_SoundCache1[i], 0,SND_Cte_MaxBufferSize/2 );
    	if(iopMTX_gap_SoundCache2[i]) L_memset(iopMTX_gap_SoundCache2[i], 0,SND_Cte_MaxBufferSize/2 );
	}
	
	iopMTX_gus_Volume=0;
}

BOOL iopMTX_b_EnableReading(int i_voice)
{
    if(!iopMTX_gpst_MuxStream) return TRUE;
    
    switch(i_voice)
    {
    case 1:
        if(RdIdx1 == -3) return FALSE;
        if(RdIdx1 == WrIdx1)
        {
            return FALSE;
        }
        break;
        
    case 2:
        if(RdIdx2 == -3) return FALSE;
        if(RdIdx2 == WrIdx2) 
        {
            return FALSE;
        }
        break;
    
    default:break;
    }    
    
//    if(dbg_repeat<50) return FALSE;
    return TRUE;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void iopMTX_ReadSound(int _i_FileHandler)
{
    if(iopMTX_gpst_MuxStream->pst_Sound1)
    {
		CDV_Dbg_Trace("pee");
		L_seekandread(_i_FileHandler, iopMTX_gui_VideoSeek, iopMTX_gap_SoundCache1[WrIdx1++], SND_Cte_MaxBufferSize/2);
		CDV_Dbg_Trace("iop");
		iopMTX_gui_VideoSeek += SND_Cte_MaxBufferSize/2;
		
		if(WrIdx1 >= M_SoundCacheLineNb) WrIdx1 = 0;
    }
    
    if(iopMTX_gpst_MuxStream->pst_Sound2)
    {
		CDV_Dbg_Trace("pee");
		L_seekandread(_i_FileHandler, iopMTX_gui_VideoSeek, iopMTX_gap_SoundCache2[WrIdx2++], SND_Cte_MaxBufferSize/2);
		CDV_Dbg_Trace("iop");
		iopMTX_gui_VideoSeek += SND_Cte_MaxBufferSize/2;
		
		if(WrIdx2 >= M_SoundCacheLineNb) WrIdx2 = 0;
    }
}

int iopMTX_i_Read
(
	int				_i_FileHandler,
	unsigned int	_ui_Seek,
	char			*_pc_Buff,
	unsigned int	_ui_BuffSize,
	int				_i_StreamType
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopMTX_tdst_MuxDesc *pst_MuxDesc;
	unsigned int		remain;
	unsigned int		frame, ui_CurrentBlockPos, ui_CurrentBlockSeek;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pc_Buff) return -1;
	if(_i_FileHandler < 0) return -1;
	if(!iopMTX_gpst_MuxStream) return -1;


	switch(_i_StreamType)
	{
	case iopMTX_Cte_VideoStream:
		pst_MuxDesc = iopMTX_gpst_MuxStream->pst_Video;
//		dbg_repeat++;
		if(iopMTX_gpst_MuxStream->pst_Sound1 && (iopMTX_gus_Volume < 0x3FFF))
		{
		    remain = iopMTX_gui_VideoSeek-pst_MuxDesc->ui_DataPos;
		    remain = (80 * remain) / pst_MuxDesc->ui_BlockSize;//iopCLI_Cte_PreLoadSize;
		    remain = remain * 0x3FFF;
		    remain = remain / 100;
		    iopMTX_gus_Volume = (unsigned short)remain;
		    if(iopMTX_gus_Volume > 0x3FFF) iopMTX_gus_Volume = 0x3FFF;
		    iopSND_i_SetVoiceVol(iopMTX_gpst_MuxStream->ui_SoundBuffer, iopMTX_gus_Volume, iopMTX_gus_Volume);
		}
		break;


	case iopMTX_Cte_SoundStream:
	    pst_MuxDesc = iopMTX_gpst_MuxStream->pst_Sound1;
        switch(RdIdx1)
        {
        case -3: 
            L_memset(_pc_Buff, 0, _ui_BuffSize);
            return 0;
        case -2: 
        case -1: 
            L_memset(_pc_Buff, 0, _ui_BuffSize);
            if(++RdIdx1 >= M_SoundCacheLineNb) RdIdx1 = 0;
            return 0;
        default:
            L_memcpy(_pc_Buff, iopMTX_gap_SoundCache1[RdIdx1], _ui_BuffSize);
            L_memset(iopMTX_gap_SoundCache1[RdIdx1], 0, _ui_BuffSize);
            if(++RdIdx1 >= M_SoundCacheLineNb) RdIdx1 = 0;
    	    return 0;
        }
	    

	case iopMTX_Cte_Sound2Stream:
		pst_MuxDesc = iopMTX_gpst_MuxStream->pst_Sound2;
        switch(RdIdx2)
        {
        case -3: 
            L_memset(_pc_Buff, 0, _ui_BuffSize);
            return 0;
        case -2: 
        case -1: 
            L_memset(_pc_Buff, 0, _ui_BuffSize);
            if(++RdIdx2 >= M_SoundCacheLineNb) RdIdx2 = 0;
            return 0;
        default:
            L_memcpy(_pc_Buff, iopMTX_gap_SoundCache2[RdIdx2], _ui_BuffSize);
            L_memset(iopMTX_gap_SoundCache2[RdIdx2], 0, _ui_BuffSize);
            if(++RdIdx2 >= M_SoundCacheLineNb) RdIdx2 = 0;
    	    return 0;
        }
		
	default: return -1;
	}

	_ui_Seek = _ui_Seek - pst_MuxDesc->ui_VirtualPos;
	frame = _ui_Seek / (pst_MuxDesc->ui_BlockSize);
	

	if(iopMTX_gui_VideoFrame != frame)
	{
        iopMTX_gui_VideoFrame++;
        iopMTX_ReadSound(_i_FileHandler);
	}
	
	if(iopMTX_gui_VideoFrame != frame)
	{
	    //
        iopMTX_gui_VideoFrame = frame;
		iopMTX_gui_VideoSeek = pst_MuxDesc->ui_DataPos + (iopMTX_gui_VideoFrame * iopMTX_gpst_MuxStream->ui_MtxFrameSize);

	    RdIdx1=RdIdx2 = -1;
	    WrIdx1=WrIdx2 = -1;
	    if(iopMTX_gpst_MuxStream->pst_Sound1) WrIdx1 = 0;
	    if(iopMTX_gpst_MuxStream->pst_Sound2) WrIdx2 = 0;
	}

	ui_CurrentBlockPos = pst_MuxDesc->ui_DataPos + (frame * iopMTX_gpst_MuxStream->ui_MtxFrameSize);
	ui_CurrentBlockSeek = _ui_Seek - frame * pst_MuxDesc->ui_BlockSize;
	
	remain = pst_MuxDesc->ui_BlockSize - ui_CurrentBlockSeek;
	if(_ui_BuffSize <= remain)
	{

		/*$2- read just what I want ----------------------------------------------------------------------------------*/

		CDV_Dbg_Trace("pee");
		L_seekandread(_i_FileHandler, iopMTX_gui_VideoSeek, (_pc_Buff), _ui_BuffSize);
		CDV_Dbg_Trace("iop");
		iopMTX_gui_VideoSeek += _ui_BuffSize;
	}
	else
	{

		/*$2- read the remainder -------------------------------------------------------------------------------------*/

		CDV_Dbg_Trace("pee");
		L_seekandread(_i_FileHandler, iopMTX_gui_VideoSeek, (_pc_Buff), remain);
		CDV_Dbg_Trace("iop");
		iopMTX_gui_VideoSeek += remain;
		_ui_BuffSize -= remain;

        iopMTX_gui_VideoFrame++;
        iopMTX_ReadSound(_i_FileHandler);		

		/*$2- is threre any big read size ? --------------------------------------------------------------------------*/

		while(_ui_BuffSize > pst_MuxDesc->ui_BlockSize)
		{
    		
			CDV_Dbg_Trace("pee");
			L_seekandread(_i_FileHandler, iopMTX_gui_VideoSeek, (_pc_Buff) + remain, pst_MuxDesc->ui_BlockSize);
			CDV_Dbg_Trace("iop");
			iopMTX_gui_VideoSeek += pst_MuxDesc->ui_BlockSize;
			_ui_BuffSize -= pst_MuxDesc->ui_BlockSize;
			remain += pst_MuxDesc->ui_BlockSize;
			
            iopMTX_gui_VideoFrame++;
            iopMTX_ReadSound(_i_FileHandler);
		}

		/*$2- any else ? ---------------------------------------------------------------------------------------------*/
 
		if(_ui_BuffSize)
		{
			CDV_Dbg_Trace("pee");
			L_seekandread(_i_FileHandler, iopMTX_gui_VideoSeek, (_pc_Buff) + remain, _ui_BuffSize);
			CDV_Dbg_Trace("iop");
			iopMTX_gui_VideoSeek += _ui_BuffSize;
		}
	}

	if((_i_StreamType == iopMTX_Cte_VideoStream) && !iopMTX_gpst_MuxStream->i_SoundIsPlaying) 
	{
	    iopMTX_StartSound();
	}
	
	return 0;
}

/*$4
 ***********************************************************************************************************************
    static members
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static iopMTX_tdst_MuxDesc *iopMTX_pst_AllocMuxDesc(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopMTX_tdst_MuxDesc *pst_MuxDesc;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_MuxDesc = iopMEM_pv_iopAlloc(sizeof(iopMTX_tdst_MuxDesc));
	if(pst_MuxDesc) L_memset(pst_MuxDesc, 0, sizeof(iopMTX_tdst_MuxDesc));

	return pst_MuxDesc;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void iopMTX_FreeMuxDesc(iopMTX_tdst_MuxDesc *pst_MuxDesc)
{
	if(!pst_MuxDesc) return;
	iopMEM_iopFree(pst_MuxDesc);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopMTX_StartSound(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_ReinitAndPlayStreamLong_VArgs	stArg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!iopMTX_gpst_MuxStream) return;
	if(iopMTX_gpst_MuxStream->i_SoundIsPlaying) return;

	stArg.ui_RPosition = iopMTX_gpst_MuxStream->pst_Sound1->ui_VirtualPos;
	stArg.ui_RSize = iopMTX_gpst_MuxStream->pst_Sound1->ui_DataSize;
	stArg.ui_RStartPos = 0;
	stArg.ui_RStopPos = 0;
	
	stArg.ui_LPosition = 0;
	stArg.ui_LSize = 0;
	stArg.ui_LStartPos = 0;
	stArg.ui_LStopPos = 0;

	if(iopMTX_gpst_MuxStream->pst_Sound2)
	{
		stArg.ui_RPosition = iopMTX_gpst_MuxStream->pst_Sound1->ui_VirtualPos;
		stArg.ui_RSize = iopMTX_gpst_MuxStream->pst_Sound1->ui_DataSize;
		stArg.ui_LPosition = iopMTX_gpst_MuxStream->pst_Sound2->ui_VirtualPos;
		stArg.ui_LSize = iopMTX_gpst_MuxStream->pst_Sound2->ui_DataSize;
	}

	stArg.i_ID = iopMTX_gpst_MuxStream->ui_SoundBuffer;
	stArg.i_LoopNb = -1;
	stArg.i_Flag = 0;
	stArg.ui_Frequency = iopMTX_gpst_MuxStream->ui_SoundFrequency;
	stArg.ui_LVol = 0xFFFF & iopMTX_gpst_MuxStream->i_SoundVolume;
	stArg.ui_RVol = 0xFFFF & iopMTX_gpst_MuxStream->i_SoundVolume;
	stArg.c_DryMixL = 1;
	stArg.c_DryMixR = 1;
	stArg.c_WetMixL = 0;
	stArg.c_WetMixR = 0;
	iopSND_ReinitAndPlayStreamLong(&stArg);
	iopMTX_gpst_MuxStream->i_SoundIsPlaying = 1;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* PSX2_IOP */
