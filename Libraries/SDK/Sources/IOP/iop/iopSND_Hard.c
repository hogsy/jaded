/*$T iopSND_Hard.c GC! 1.097 11/06/01 09:46:35 */


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

/*$2- sce ------------------------------------------------------------------------------------------------------------*/

#include <libsd.h>
#include <stdio.h>
#include <sys/file.h>
#include <kernel.h>
#include <string.h>

/*$2- iop ------------------------------------------------------------------------------------------------------------*/

#include "iop/iopDebug.h"
#include "iop/iopMEM.h"
#include "iop/iopCLI.h"
#include "CDV_Manager.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "Sound/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "iop/iopSND.h"
#include "iop/iopSND_Voice.h"
#include "iop/iopSND_Hard.h"
#include "iop/iopSND_libsd.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define M_CheckPointer(_ptr, _ret)	{ if(!_ptr) return _ret; }

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

volatile iopSND_tdst_HardBuffer iopSND_sa_HardBuffer[SND_Cte_MaxHardBufferNb];
volatile int					iopSND_si_ActiveHardBuffer[iopSND_Cte_CoreNb];
int								iopSND_gi_HardLock;
//volatile int iopSND_gi_SpecialLock;
void iopSND_WaitT(int T);


/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_HardInitModule(void)
{
	/*~~~~~~~~~~~~*/
	int core, voice;
	/*~~~~~~~~~~~~*/

	for(core = 0; core < iopSND_Cte_CoreNb; core++)
	{
		/* set the master volume */
		L_sceSdSetParam(core | SD_P_MVOLL, 0x3fff);
		L_sceSdSetParam(core | SD_P_MVOLR, 0x3fff);

		for(voice = 0; voice < iopSND_Cte_CoreVoiceNb; voice++)
		{
			/* set voice volume */
			L_sceSdSetParam(core | (voice << 1) | SD_VP_VOLL, 0x1eff);
			L_sceSdSetParam(core | (voice << 1) | SD_VP_VOLR, 0x1eff);

			/* set the enveloppe */
			L_sceSdSetParam(core | (voice << 1) | SD_VP_ADSR1, 0/*0x000f*/);
			L_sceSdSetParam(core | (voice << 1) | SD_VP_ADSR2, 0/*0x1fc0*/);

			iopSND_sa_HardBuffer[iopSND_Cte_CoreVoiceNb * core + voice].i_Core = core;
			iopSND_sa_HardBuffer[iopSND_Cte_CoreVoiceNb * core + voice].i_Voice = voice;

			/* init sound buffers */
			iopSND_HardFreeBuffer((iopSND_tdst_HardBuffer *) &iopSND_sa_HardBuffer[iopSND_Cte_CoreVoiceNb * core + voice]);
		}
	}

	L_memset(iopSND_si_ActiveHardBuffer, 0, sizeof(int) * iopSND_Cte_CoreNb);
//	iopSND_gi_SpecialLock = 0;
}

/*
 =======================================================================================================================
    due to buggy read when crossing 128k borduaries, we have to read more than once time NAX register fixed in 2.20 lib
 =======================================================================================================================
 */
int iopSND_i_HardGetNAX(iopSND_tdst_HardBuffer *_pst_hb)
{
	/*~~~~*/
	int nax;
	/*~~~~*/

	M_CheckPointer(_pst_hb, -1);
	nax = L_sceSdGetAddr(_pst_hb->i_Core | (_pst_hb->i_Voice << 1) | SD_VA_NAX);

	return nax;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_HardFreeBuffer(iopSND_tdst_HardBuffer *_pst_hard_buffer)
{
	/*~~~~~~~~~~~~*/
	int voice, core;
	/*~~~~~~~~~~~~*/

	M_CheckPointer(_pst_hard_buffer, ;);
	
	CLI_WaitSema(iopSND_gi_HardLock);
	voice = _pst_hard_buffer->i_Voice;
	core = _pst_hard_buffer->i_Core;
	L_sceSdSetParam(core | (voice << 1) | SD_VP_VOLL, 0);
	L_sceSdSetParam(core | (voice << 1) | SD_VP_VOLR, 0);
	L_sceSdSetParam(core | (voice << 1) | SD_VP_PITCH, 0);
	_pst_hard_buffer->ui_Flag = iopSND_Cte_HardFree;
	iopSND_si_ActiveHardBuffer[core]--;
	CLI_SignalSema(iopSND_gi_HardLock);
	
	iopSND_WaitT(4);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
iopSND_tdst_HardBuffer *iopSND_pst_HardAllocBuffer(int core)
{
	/*~~~~~~~~~~~~*/
	int voice;
	/*~~~~~~~~~~~~*/

	CLI_WaitSema(iopSND_gi_HardLock);
	
	if(core == -1) 
		core = (iopSND_si_ActiveHardBuffer[1] > iopSND_si_ActiveHardBuffer[0]) ? 0 : 1;

	for(voice = 0; voice < iopSND_Cte_CoreVoiceNb; voice++)
	{
		if(iopSND_sa_HardBuffer[iopSND_Cte_CoreVoiceNb * core + voice].ui_Flag == iopSND_Cte_HardFree)
		{
			iopSND_sa_HardBuffer[iopSND_Cte_CoreVoiceNb * core + voice].ui_Flag = 0;
			iopSND_si_ActiveHardBuffer[core]++;
			CLI_SignalSema(iopSND_gi_HardLock);

			return (iopSND_tdst_HardBuffer *) &iopSND_sa_HardBuffer[iopSND_Cte_CoreVoiceNb * core + voice];
		}
	}

	core = 1 - core;
	for(voice = 0; voice < iopSND_Cte_CoreVoiceNb; voice++)
	{
		if(iopSND_sa_HardBuffer[iopSND_Cte_CoreVoiceNb * core + voice].ui_Flag == iopSND_Cte_HardFree)
		{
			iopSND_sa_HardBuffer[iopSND_Cte_CoreVoiceNb * core + voice].ui_Flag = 0;
			iopSND_si_ActiveHardBuffer[core]++;
			CLI_SignalSema(iopSND_gi_HardLock);
			
			return (iopSND_tdst_HardBuffer *) &iopSND_sa_HardBuffer[iopSND_Cte_CoreVoiceNb * core + voice];
		}
	}

	CLI_SignalSema(iopSND_gi_HardLock);
	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_HardPlay(iopSND_tdst_HardBuffer *_pst_hbl, iopSND_tdst_HardBuffer *_pst_hbr)
{
	M_CheckPointer(_pst_hbr, ;);

    	
	CLI_WaitSema(iopSND_gi_HardLock);
	if(_pst_hbl)
	{	    
		if(_pst_hbl->ui_Flag & iopSND_Cte_HardPause)
		{
			if(_pst_hbl->i_Core == _pst_hbr->i_Core)
			{
				L_sceSdSetSwitch(_pst_hbl->i_Core | SD_S_KON, (1 << _pst_hbl->i_Voice) | (1 << _pst_hbr->i_Voice));
			}
			else
			{
				L_sceSdSetSwitch(_pst_hbl->i_Core | SD_S_KON, 1 << _pst_hbl->i_Voice);
				L_sceSdSetSwitch(_pst_hbr->i_Core | SD_S_KON, 1 << _pst_hbr->i_Voice);
			}

			L_sceSdSetParam(_pst_hbl->i_Core | (_pst_hbl->i_Voice << 1) | SD_VP_VOLL, 0xC01f);
			L_sceSdSetParam(_pst_hbr->i_Core | (_pst_hbr->i_Voice << 1) | SD_VP_VOLR, 0xC01f);
		}
		else
		{
			if(_pst_hbl->i_Core == _pst_hbr->i_Core)
			{
				L_sceSdSetSwitch(_pst_hbr->i_Core | SD_S_KON, (1 << _pst_hbr->i_Voice) | (1 << _pst_hbl->i_Voice));
			}
			else
			{
				L_sceSdSetSwitch(_pst_hbr->i_Core | SD_S_KON, 1 << _pst_hbr->i_Voice);
				L_sceSdSetSwitch(_pst_hbl->i_Core | SD_S_KON, 1 << _pst_hbl->i_Voice);
			}
		}

		_pst_hbl->ui_Flag &= iopSND_Cte_HardFlagMask;
		_pst_hbl->ui_Flag |= iopSND_Cte_HardStartedOnce | iopSND_Cte_HardPlay;

		_pst_hbr->ui_Flag &= iopSND_Cte_HardFlagMask;
		_pst_hbr->ui_Flag |= iopSND_Cte_HardStartedOnce | iopSND_Cte_HardPlay;
		

	}
	else
	{
		if(_pst_hbr->ui_Flag & iopSND_Cte_HardPause)
		{
			/* pop up the volume and start music */
			L_sceSdSetSwitch(_pst_hbr->i_Core | SD_S_KON, (1 << _pst_hbr->i_Voice));
			L_sceSdSetParam(_pst_hbr->i_Core | (_pst_hbr->i_Voice << 1) | SD_VP_VOLL, 0xC01f);
			L_sceSdSetParam(_pst_hbr->i_Core | (_pst_hbr->i_Voice << 1) | SD_VP_VOLR, 0xC01f);
		}
		else
		{
			/* start sound generation */
			L_sceSdSetSwitch(_pst_hbr->i_Core | SD_S_KON, 1 << _pst_hbr->i_Voice);
		}

		_pst_hbr->ui_Flag &= iopSND_Cte_HardFlagMask;
		_pst_hbr->ui_Flag |= iopSND_Cte_HardStartedOnce | iopSND_Cte_HardPlay;
	}

    iopSND_WaitT(4);
	//if(_pst_hbl) L_sceSdSetAddr(_pst_hbl->i_Core | (_pst_hbl->i_Voice << 1) | SD_VA_LSAX, _pst_hbl->ui_LoopPos);
	//L_sceSdSetAddr(_pst_hbr->i_Core | (_pst_hbr->i_Voice << 1) | SD_VA_LSAX, _pst_hbr->ui_LoopPos);
	CLI_SignalSema(iopSND_gi_HardLock);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_HardSetLoopPos(iopSND_tdst_HardBuffer *_pst_hb, int _i_loop_pos)
{
	M_CheckPointer(_pst_hb, ;);
	L_sceSdSetAddr(_pst_hb->i_Core | (_pst_hb->i_Voice << 1) | SD_VA_LSAX, _i_loop_pos);
	_pst_hb->ui_LoopPos = _i_loop_pos;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_HardSetStartPos(iopSND_tdst_HardBuffer *_pst_hb, int _i_loop_pos)
{
	M_CheckPointer(_pst_hb, ;);
	L_sceSdSetAddr(_pst_hb->i_Core | (_pst_hb->i_Voice << 1) | SD_VA_SSA, _i_loop_pos);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_HardSetPitch(iopSND_tdst_HardBuffer *_pst_hb, unsigned short us_pitch)
{
	M_CheckPointer(_pst_hb, ;);
	_pst_hb->us_Pitch = us_pitch;
	 
	L_sceSdSetParam(_pst_hb->i_Core | (_pst_hb->i_Voice << 1) | SD_VP_PITCH, us_pitch);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_HardSetVol(iopSND_tdst_HardBuffer *_pst_hb, unsigned short us_left, unsigned short us_right)
{
	M_CheckPointer(_pst_hb, ;);
    
    us_right &= 0x7FFF;
    us_left  &= 0x7FFF;
  
    _pst_hb->us_VolRight = us_right;
	L_sceSdSetParam(_pst_hb->i_Core | (_pst_hb->i_Voice << 1) | SD_VP_VOLR,us_right);

    _pst_hb->us_VolLeft = us_left;
	L_sceSdSetParam(_pst_hb->i_Core | (_pst_hb->i_Voice << 1) | SD_VP_VOLL,us_left);
	
	iopSND_WaitT(4);
}

void iopSND_HardGetVol(iopSND_tdst_HardBuffer *_pst_hb, unsigned short* pus_left, unsigned short* pus_right)
{
	M_CheckPointer(_pst_hb, ;);

	*pus_right= L_sceSdGetParam(_pst_hb->i_Core | (_pst_hb->i_Voice << 1) | SD_VP_VOLR);
	*pus_left = L_sceSdGetParam(_pst_hb->i_Core | (_pst_hb->i_Voice << 1) | SD_VP_VOLL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_HardStop(iopSND_tdst_HardBuffer *_pst_hbl, iopSND_tdst_HardBuffer *_pst_hbr)
{
	M_CheckPointer(_pst_hbr, ;);


	if(_pst_hbl)
	{
		if(_pst_hbr->i_Core == _pst_hbl->i_Core)
		{
			L_sceSdSetSwitch(_pst_hbr->i_Core | SD_S_KOFF, (1 << _pst_hbr->i_Voice) | (1 << _pst_hbl->i_Voice));
		}
		else
		{
			L_sceSdSetSwitch(_pst_hbr->i_Core | SD_S_KOFF, (1 << _pst_hbr->i_Voice));
			L_sceSdSetSwitch(_pst_hbl->i_Core | SD_S_KOFF, (1 << _pst_hbl->i_Voice));
		}

		_pst_hbl->ui_Flag &= ~iopSND_Cte_HardDynMask;
		_pst_hbr->ui_Flag &= ~iopSND_Cte_HardDynMask;
	}
	else
	{
		L_sceSdSetSwitch(_pst_hbr->i_Core | SD_S_KOFF, (1 << _pst_hbr->i_Voice));
		_pst_hbr->ui_Flag &= ~iopSND_Cte_HardDynMask;
	}
}

void iopSND_HardSetPos(iopSND_tdst_HardBuffer *_pst_hbl, iopSND_tdst_HardBuffer *_pst_hbr, int i_lPos, int i_rPos)
{
	M_CheckPointer(_pst_hbr, ;);

	if(_pst_hbl)
	{
		if(_pst_hbr->i_Core == _pst_hbl->i_Core)
		{
			L_sceSdSetSwitch(_pst_hbr->i_Core | SD_S_KOFF, (1 << _pst_hbr->i_Voice) | (1 << _pst_hbl->i_Voice));
		}
		else
		{
			L_sceSdSetSwitch(_pst_hbr->i_Core | SD_S_KOFF, (1 << _pst_hbr->i_Voice));
			L_sceSdSetSwitch(_pst_hbl->i_Core | SD_S_KOFF, (1 << _pst_hbl->i_Voice));
		}

        iopSND_HardSetStartPos(_pst_hbr, i_rPos);
        iopSND_HardSetStartPos(_pst_hbl, i_lPos);
        
        iopSND_HardPlay(_pst_hbl, _pst_hbr);
	}
	else
	{
		L_sceSdSetSwitch(_pst_hbr->i_Core | SD_S_KOFF, (1 << _pst_hbr->i_Voice));
        iopSND_HardSetStartPos(_pst_hbr, i_rPos);
        iopSND_HardPlay(NULL, _pst_hbr);
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_HardGetPlayingStatus(iopSND_tdst_HardBuffer *_pst_hb)
{
	/*~~~~~~~~~~~~~~~~*/
	int endx, ret, nax;
	/*~~~~~~~~~~~~~~~~*/

	ret = 0;

	M_CheckPointer(_pst_hb, 0);

	//CLI_WaitSema(iopSND_gi_HardLock);
	if(_pst_hb->ui_Flag & iopSND_Cte_HardPlay)
	{
		endx = L_sceSdGetSwitch(_pst_hb->i_Core | SD_S_ENDX);
		endx = (endx >> _pst_hb->i_Voice) & 1;
		//if(endx) return 0;
		
		nax = L_sceSdGetAddr(_pst_hb->i_Core | (_pst_hb->i_Voice << 1) | SD_VA_NAX);
		if(nax >= 0x5010)  ret = 1;
	}
	//CLI_SignalSema(iopSND_gi_HardLock);
	return ret;
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
#define ONE_T ((1000*1000)/48000)        /* One TICK        (1/48000th of a second) */ 

void iopSND_WaitT(int T)
{
//*
    DelayThread(T*21); //ONE_T # 21
    //*/
   
/*
    int i, j;
    
    T = 10;
    for(i=0; i<(T); i++)
    {
       for(j=0; j<130; j++)
       {
           asm("nop");
       }
    }
    //*/
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif
