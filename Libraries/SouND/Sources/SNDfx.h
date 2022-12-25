/*$T SNDfx.h GC 1.138 03/09/05 15:59:04 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDfx_h__
#define __SNDfx_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    constant
 ***********************************************************************************************************************
 */

/*$2- request --------------------------------------------------------------------------------------------------------*/

#define SND_Cte_Fx_iEnable			0x00001001
#define SND_Cte_Fx_iMode			0x00001002
#define SND_Cte_Fx_iDelay			0x00001003
#define SND_Cte_Fx_iFeedback		0x00001004
#define SND_Cte_Fx_fWetVol			0x00001005
#define SND_Cte_Fx_iWetPan			0x00001006
#define SND_Cte_Fx_iCoreId			0x00001007
#define SND_Cte_Fx_iContext			0x00001008
#define SND_Cte_Fx_iFlags			0x00001009
#define SND_Cte_Fx_iActiveContext	0x0000100A
#define SND_Cte_Fx_iReinit			0x0000100B

/*$2- activation -----------------------------------------------------------------------------------------------------*/

#define SND_Cte_FxState_Inactive	0
#define SND_Cte_FxState_Active		1
#define SND_Cte_FxState_Lock		2
#define SND_Cte_FxState_Unlock		3

/*$2- Fx Mode --------------------------------------------------------------------------------------------------------*/

#define SND_Cte_FxMode_Off			0x00000000
#define SND_Cte_FxMode_Room			0x00000001
#define SND_Cte_FxMode_StudioA		0x00000002
#define SND_Cte_FxMode_StudioB		0x00000003
#define SND_Cte_FxMode_StudioC		0x00000004
#define SND_Cte_FxMode_Hall			0x00000005
#define SND_Cte_FxMode_Space		0x00000006
#define SND_Cte_FxMode_Echo			0x00000007
#define SND_Cte_FxMode_Delay		0x00000008
#define SND_Cte_FxMode_Pipe			0x00000009
#define SND_Cte_FxMode_Mountains	0x0000000A
#define SND_Cte_FxMode_City			0x0000000B
#define SND_Cte_FxMode_Nb			0x0000000C

/*$2- flags ----------------------------------------------------------------------------------------------------------*/

/* ui_FxFlag */
#define SND_FxFlg_Locked		0x00000001
#define SND_FxFlg_RqValidate	0x00000002

/*$2------------------------------------------------------------------------------------------------------------------*/

#define SND_Cte_FxCoreA				0
#define SND_Cte_FxCoreB				1
#ifdef _RVL
#define SND_Cte_FxCoreC				2
#define SND_Cte_FxCoreNb			3
#else
#define SND_Cte_FxCoreNb			2
#endif

#define SND_Cte_FxContextLocal		0
#define SND_Cte_FxContextGlobal		1
#define SND_Cte_FxContextNb			2

#define SND_Cte_FxMaxVoiceNbByCore	24

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	SND_tdst_FxParam_
{
	unsigned int	ui_FxFlag;
	int				i_Mode;
	int				i_Delay;
	int				i_Feedback;
	float			f_WetVol;
	int				i_WetPan;
} SND_tdst_FxParam;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern SND_tdst_FxParam SND_gst_FxParam[SND_Cte_FxCoreNb][SND_Cte_FxContextNb];

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2- open/close module ----------------------------------------------------------------------------------------------*/

int		SND_i_FxInitModule(void);
void	SND_FxCloseModule(void);

/*$2- interface ------------------------------------------------------------------------------------------------------*/

int		SND_i_FxSeti(int _i_RqId, int _i_Value);
int		SND_i_FxGeti(int _i_RqId);
int		SND_i_FxSetf(int _i_RqId, float _f_Value);
float	SND_f_FxGetf(int _i_RqId);

/*$2- updating -------------------------------------------------------------------------------------------------------*/

void	SND_FxUpdate(void);
void	SND_FxLocalRequest(int _i_CoreId, float _f_WetVol, int _i_Mode, int _i_Delay, int _i_FeedBack);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#undef __EXTERN
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SNDfx_h__ */
