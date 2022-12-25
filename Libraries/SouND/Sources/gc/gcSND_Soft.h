/*$T gcSND_Soft.h GC! 1.081 01/29/03 18:20:33 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef _GAMECUBE
#ifndef __gcSND_Soft_h__
#define __gcSND_Soft_h__

#include "SNDfx.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct gcSND_tdst_HardBuffer_
{
	AXPBADDR							st_AxAddrSettings;	
	AXVPB								*pst_AxBuffer;		
	int									i_MIXinput;			
	int									i_MIXpan;			
	unsigned short						pred_scale;
	unsigned short						loop_pred_scale;
} gcSND_tdst_HardBuffer;


typedef struct	gcSND_tdst_SoftBuffer_
{
	unsigned int					ui_Flags;

	/* hight level command */
	int								i_Pan;
	int								i_SPan;
	int								i_Volume;
	int								i_Frequency;
	int								i_StereoPanIdx;
	int								i_FxVol;

	/* low level command */
	int								i_MIXaux[SND_Cte_FxCoreNb];
	int								i_MIXspan;
	float							f_SRCratio;
	gcSND_tdst_HardBuffer			ast_Hard[2];
	
	/* memory config */
	struct GC_tdst_StreamHandler_	*pst_SndFileHandler;
	char *volatile 					pc_RamBuffer;
	ARQRequest						st_ARQRequest; 		
	char							*pc_AramBuffer;		

	/* data config */
	unsigned int volatile			ui_DataPosition;
	unsigned int volatile			ui_DataSize;

	/* duplication data */
	struct gcSND_tdst_SoftBuffer_	*pst_DuplicateSrc;
	unsigned int					ui_UserNb;
} gcSND_tdst_SoftBuffer;


/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern gcSND_tdst_SoftBuffer	gcSND_gax_SoftBuffer[SND_Cte_MaxSoftBufferNb];


/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

struct SND_tdst_SoundBuffer_	*gcSND_pst_CreateSoftBuffer
								(
									int				_i_InitFreq,
									unsigned int	_ui_DataPosition,
									unsigned int	_ui_DataSize
								);
void							gcSND_SoftRelease(gcSND_tdst_SoftBuffer *);
gcSND_tdst_SoftBuffer			*gcSND_pst_SoftDuplicate(gcSND_tdst_SoftBuffer *);

/*$2------------------------------------------------------------------------------------------------------------------*/

int								gcSND_i_SoftPlay(struct gcSND_tdst_SoftBuffer_ *, int _i_Flag, int);
void							gcSND_SoftStop(gcSND_tdst_SoftBuffer *);
void							gcSND_SoftPause(gcSND_tdst_SoftBuffer *);

/*$2------------------------------------------------------------------------------------------------------------------*/

gcSND_tdst_SoftBuffer			*gcSND_pst_AllocSoftBuffer(void);
void							gcSND_FreeSoftBuffer(gcSND_tdst_SoftBuffer *);

/*$2------------------------------------------------------------------------------------------------------------------*/

void							gcSND_SoftStreamRelease(gcSND_tdst_SoftBuffer *);

/*$2------------------------------------------------------------------------------------------------------------------*/

void							gcSND_SoftLoadData(gcSND_tdst_SoftBuffer *);
void							gcSND_SoftWaitEndOfLoadings(void);
void							gcSND_ARQCallBack(u32 task);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __gcSND_Soft_h__ */
#endif /* _GAMECUBE */
