/*$T iopSND.h GC! 1.097 05/18/02 13:32:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __iopSND_h__
#define __iopSND_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$2- configuration --------------------------------------------------------------------------------------------------*/

#define iopSND_Cte_CoreVoiceNb		24
#define iopSND_Cte_CoreNb			2

/*$2- flags ----------------------------------------------------------------------------------------------------------*/

#define iopSND_Cte_Stream					0x00000002
#define iopSND_Cte_Stereo					0x00000004
#define iopSND_Cte_Loaded					0x00000010
#define iopSND_Cte_EndRead					0x00000020
#define iopSND_Cte_EndPlayed0				0x00000040
#define iopSND_Cte_StartedOnce				0x00000080
#define iopSND_Cte_AutoStopped				0x00000100
#define iopSND_Cte_EndPlayed1				0x00000200
#define iopSND_Cte_StreamIsPlayingNow	    0x00000400
#define iopSND_Cte_UseFxA					0x00001000
#define iopSND_Cte_UseFxB					0x00002000
#define iopSND_Cte_MixL						0x00004000
#define iopSND_Cte_MixR						0x00008000
#define iopSND_Cte_FxMixL					0x00010000
#define iopSND_Cte_FxMixR					0x00020000

/*$2- masks ----------------------------------------------------------------------------------------------------------*/

#define iopSND_Cte_SndMsk_Flag	0x00FFFFFF
#define iopSND_Cte_SndMsk_State 0xFF000000
#define iopSND_Cte_SndMsk_Mix   (iopSND_Cte_MixL|iopSND_Cte_MixR|iopSND_Cte_FxMixL|iopSND_Cte_FxMixR)

/*$2- state ----------------------------------------------------------------------------------------------------------*/

#define iopSND_Cte_SndFlg_Free	0x01000000
#define iopSND_Cte_SndFlg_Stop	0x02000000
#define iopSND_Cte_SndFlg_Play	0x04000000

/*$2- macros ---------------------------------------------------------------------------------------------------------*/

#define iopSND_M_SetState(__flag, __status) \
	do \
	{ \
		__flag = (~iopSND_Cte_SndMsk_State & __flag) | __status; \
	} while(0);

#define iopSND_M_SetFlag(__flag, __flagid) \
	do \
	{ \
		__flag |= __flagid; \
	} while(0);

#define iopSND_M_ResetFlag(__flag, __flagid) \
	do \
	{ \
		__flag &= ~__flagid; \
	} while(0);

#define iopSND_M_CheckSoftBuffIdxOrReturn(_id, _ret) \
	do \
	{ \
		if \
		( \
			(_id < 0) \
		||	((_id & 0x0000FFFF) == 0x0000FFFF) \
		||	((_id & 0xFFFF0000) == 0xFFFF0000) \
		||	((_id & 0x0000FFFF) >= SND_Cte_MaxSoftBufferNb) \
		||	(((_id & 0xFFFF0000) >> 16) >= SND_Cte_MaxSoftBufferNb) \
		) \
		{ \
			iopDbg_M_ErrX(iopDbg_Err_0054 " "__FILE__ "(%d) : %d", __LINE__, _id); \
			return _ret; \
		} \
	} while(0);

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern volatile int				iopSND_vi_BigFd;
extern volatile int				iopSND_vi_BigFd_Snd;
extern char						asz_BigFile[1024];
extern volatile int				iopSND_gi_SndThreadVoice;
extern volatile int				iopSND_gi_SndThreadVoicePrio;
extern volatile int				iopSND_gi_SeekLock;
extern volatile int				iopSND_gb_UseBinSeek;
extern volatile unsigned int	iopSND_gui_BinSeek;  
extern int						iopSND_gb_EnableSound;
extern int                      iopSND_gi_RenderMode;
/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2- module ---------------------------------------------------------------------------------------------------------*/

int								iopSND_i_InitModule(void);
int								iopSND_i_CloseModule(void);

/*$2- Snd Files ------------------------------------------------------------------------------------------------------*/

int								iopSND_i_BigOpen(char *_str_name);

/*$2- master volume --------------------------------------------------------------------------------------------------*/

int								iopSND_i_SetMasterVol(int _i_Left, int _i_Right);
int								iopSND_i_LoadData
								(
									unsigned int	*_pui_Pos,
									unsigned int	_ui_Size,
									char			**_ppc_Buff,
									int				iiii
								);
int								iopSND_i_GetMasterVol(int *_pi_Left, int *_pi_Right);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __iopSND_h__ */
