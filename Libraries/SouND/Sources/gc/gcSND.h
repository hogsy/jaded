/*$T gcSND.h GC! 1.097 05/28/02 09:10:44 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __gcSND_h__
#define __gcSND_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */
#define gcSND_M_WaitWhile(cond) \
	do \
	{ \
		int iInfinite; \
		for(iInfinite = 0; iInfinite < 486000000; iInfinite++) \
		{ \
			if(!(cond)) break; \
		} \
	} while(0);


#define gcSND_M_CheckSoundBuffPtrOrReturn(_ptr, _ret) \
	do \
	{ \
		if((_ptr == NULL) || (((((gcSND_tdst_SoftBuffer*)_ptr)->ui_Flags & SND_Cul_DSF_Used)) == 0)) return _ret; \
	} while(0);

#define lRoudUp64(x)	(((u32) (x) + 64 - 1) &~(64 - 1))

#define gcSND_Cte_StereoFlag	0x00000001
#define gcSND_M_IsStereo(p)		((int)p & gcSND_Cte_StereoFlag)
#define gcSND_M_GetBuffer(p)	((gcSND_tdst_SoftBuffer*)((int)p & ~gcSND_Cte_StereoFlag))

/*$2- priority of voices ---------------------------------------------------------------------------------------------*/

#define gcSND_Cte_VoicePriority		15
#define gcSND_Cte_StreamPriority	AX_PRIORITY_NODROP

/*$2- freq processing ------------------------------------------------------------------------------------------------*/

#define fGetSRCRatioFromFreq(_freq)		((float) (_freq) * 0.00003125f)

_inline_ u16 sGetHiRatioFromfRatio(f32 _f_Ratio)
{
    u32 srcBits;
    u16 ratioHi;
    srcBits = (u32)(0x00010000 * _f_Ratio);
    ratioHi = (u16)(srcBits >> 16);
    return ratioHi;
} 
_inline_ u16 sGetLoRatioFromfRatio(f32 _f_Ratio)
{
    u32 srcBits;
    u16 ratioLo;
    srcBits = (u32)(0x00010000 * _f_Ratio);
    ratioLo = (u16)(srcBits & 0xFFFF);
    return ratioLo;
} 


/*$2- address processing ---------------------------------------------------------------------------------------------*/

#define sGetStartAddr(_buff)			((u32) (_buff) * 2 + 2)
#define sGetMiddleAddr(_buff, _size)	((u32) (_buff) * 2 + (_size) - 1)
#define sGetEndAddr(_buff, _size)		((u32) (_buff) * 2 + 2 * (_size) - 1)

#define sGetStartAddrHi(_buff)			(u16) (((u32) (_buff) * 2 + 2) >> 16)
#define sGetStartAddrLo(_buff)			(u16) (((u32) (_buff) * 2 + 2) & 0xFFFF)
#define sGetEndAddrHi(_buff, _size)		(u16) (((((u32) (_buff) + (_size)) * 2) - 1) >> 16)
#define sGetEndAddrLo(_buff, _size)		(u16) (((((u32) (_buff) + (_size)) * 2) - 1) & 0xFFFF)

#define uiGetAddrFromHiLo(_Hi, _Lo) (u32)(( ((((_Hi)&0xFFFF)<<16) - 2) + ((_Lo)&0xFFFF) -2) /2)

/*$2- default value --------------------------------------------------------------------------------------------------*/

#define gcSND_Cte_MinVol    (-904)
#define gcSND_Cte_MaxVol    (60)

/*$F
 * - auxA & auxB are prefader mode
 * - we force reverb mix to 50%, so 
 *    dry = 0.5 x fader
 *    wet = 0.5 x (auxA+auxB)
 *
 * as we never change the dry volume :
 * - fader = cte = 6dB
 * - aux = 2 x wet_vol
 *
 * each channel volume is set by the input :
 * - input = vol x master_vol
 *
 */
#define gcSND_Cte_DefaultSPan	(127)
#define gcSND_Cte_DefaultPan	(64)

#define gcSND_Cte_DefaultFader	(60)	// +6dB
#define gcSND_Cte_DefaultInput	(-904)

#define gcSND_Cte_DefaultMode	(MIX_MODE_AUXA_PREFADER|MIX_MODE_AUXB_PREFADER)

/*$4
 ***********************************************************************************************************************
	TYPES
 ***********************************************************************************************************************
 */
 
typedef struct	SND_tdst_GameCubeADPCMInfo_
{
	/* start context */
	short			coef[16];
	unsigned short	gain;
	unsigned short	pred_scale;
	short			yn1;
	short			yn2;

	/* loop context */
	unsigned short	loop_pred_scale;
	short			loop_yn1;
	short			loop_yn2;
} SND_tdst_GameCubeADPCMInfo;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern int						    gcSND_gi_MasterVolume;
extern SND_tdst_GameCubeADPCMInfo	gcSND_gst_GameCubeCoefficientsTable;

/*$4
 ***********************************************************************************************************************
    prototype
 ***********************************************************************************************************************
 */

/*$2- module ---------------------------------------------------------------------------------------------------------*/

LONG							gcSND_l_Init(struct SND_tdst_TargetSpecificData_ *_pst_SP);
void							gcSND_Close(struct SND_tdst_TargetSpecificData_ *_pst_SP);

/*$2- master controls ------------------------------------------------------------------------------------------------*/

void							gcSND_SetGlobalVol(LONG _i_Level);
LONG							gcSND_l_GetGlobalVol(void);

/*$2- SB functions ---------------------------------------------------------------------------------------------------*/

struct SND_tdst_SoundBuffer_	*gcSND_pst_SB_Create
								(
									struct SND_tdst_TargetSpecificData_ *_p,
									struct SND_tdst_WaveData_			*_p_Wave,
									void								*_pv_Position
								);
int								gcSND_SB_Play(struct SND_tdst_SoundBuffer_ *_pst_SB, int _i_Flag, int, int);
void							gcSND_SB_Stop(struct SND_tdst_SoundBuffer_ *_pst_SB);
void							gcSND_SB_Release(struct SND_tdst_SoundBuffer_ *_pst_SB);
void							gcSND_SB_Duplicate
								(
									struct SND_tdst_TargetSpecificData_ *_pst_SpeData,
									struct SND_tdst_SoundBuffer_		*_pst_SrcSB,
									struct SND_tdst_SoundBuffer_		**_ppst_DstSB
								);
void							gcSND_SB_SetVolume(struct SND_tdst_SoundBuffer_ *_pst_SB, int _i_Level);
void							gcSND_SB_SetFrequency(struct SND_tdst_SoundBuffer_ *_pst_SB, int _i_Freq);
void							gcSND_SB_SetPan(struct SND_tdst_SoundBuffer_ *_pst_SB, int _i_Pan, int _i_FrontRear);
int								gcSND_SB_GetStatus(struct SND_tdst_SoundBuffer_ *_pst_SB, int *_pi_Status);
void							gcSND_SB_GetFrequency(struct SND_tdst_SoundBuffer_ *_pst_SB, int *_pi_Freq);
void							gcSND_SB_GetPan
								(
									struct SND_tdst_SoundBuffer_	*_pst_SB,
									int								*_pi_Pan,
									int								*_pi_FrontRear
								);
void							gcSND_SB_GetCurrPos
								(
									struct SND_tdst_SoundBuffer_	*_pst_SB,
									int								*_pi_Pos,
									int								*_pi_Write
								);
void							gcSND_SB_Pause(struct SND_tdst_SoundBuffer_ *_pst_SB);
void							gcSND_SB_SetCurrPos(struct SND_tdst_SoundBuffer_ *_pst_SB, int _i_Pos);
struct SND_tdst_SoundBuffer_	*gcSND_Reload
								(
									struct SND_tdst_TargetSpecificData_ *,
									struct SND_tdst_SoundBuffer_ *,
									struct SND_tdst_WaveData_ *,
									unsigned int
								);
void							gcSND_RefreshStatus(void);
int                             gcSND_i_ChangeRenderMode(int _i_Mode);
void                            gcSND_CommitChange(struct MATH_tdst_Vector_ * _pst_MicorPos);

/*$2------------------------------------------------------------------------------------------------------------------*/

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __gcSND_h__ */
