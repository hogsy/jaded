/*$T iopSND_Fx.c GC 1.138 04/09/04 15:45:57 */


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

/*$2- base -----------------------------------------------------------------------------------------------------------*/

#include "iop/iopDebug.h"
#include "iop/iopMEM.h"
#include "iop/iopCLI.h"
#include "iop/iopBAStypes.h"
#include "CDV_Manager.h"
#include "RPC_Manager.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/ps2/ps2SND.h"
#include "iop/iopSND.h"
#include "iop/iopSND_Voice.h"
#include "iop/iopSND_Hard.h"
#include "iop/iopSND_Fx.h"
#include "iop/iopSND_libsd.h"

/*$2------------------------------------------------------------------------------------------------------------------*/

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

typedef struct	iopSND_tdst_FxSettings_
{
	sceSdEffectAttr *pst_FxAttr;
	int				i_VMIXR;
	int				i_VMIXL;
	int				i_VMIXER;
	int				i_VMIXEL;
} iopSND_tdst_FxSettings;

/*$4
 ***********************************************************************************************************************
    private variables
 ***********************************************************************************************************************
 */

iopSND_tdst_FxSettings	iopSND_dst_FxSettings[SD_CORE_MAX];

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_FxInitModule(void)
{
	/*~~~~~*/
	int core;
	/*~~~~~*/

	/* get spu address */
	L_sceSdSetAddr(SD_CORE_0 | SD_A_EEA, (int) iopMEM_pv_spuFxAlloc(0));
	L_sceSdSetAddr(SD_CORE_1 | SD_A_EEA, (int) iopMEM_pv_spuFxAlloc(1));

	/* FX parameters = enable but no voice, no effect */
	L_memset(iopSND_dst_FxSettings, 0, SD_CORE_MAX * sizeof(iopSND_tdst_FxSettings));

	for(core = SD_CORE_0; core < SD_CORE_MAX; core++)
	{
		iopSND_dst_FxSettings[core].i_VMIXER = 0;
		iopSND_dst_FxSettings[core].i_VMIXEL = 0;
		iopSND_dst_FxSettings[core].i_VMIXR = 0;
		iopSND_dst_FxSettings[core].i_VMIXL = 0;
		iopSND_dst_FxSettings[core].pst_FxAttr = iopMEM_pv_iopAllocAlign(sizeof(sceSdEffectAttr), 64);
		iopSND_dst_FxSettings[core].pst_FxAttr->core = core;
		iopSND_dst_FxSettings[core].pst_FxAttr->mode = SD_REV_MODE_OFF;
		iopSND_dst_FxSettings[core].pst_FxAttr->depth_L = 0;
		iopSND_dst_FxSettings[core].pst_FxAttr->depth_R = 0;
		iopSND_dst_FxSettings[core].pst_FxAttr->delay = 0;
		iopSND_dst_FxSettings[core].pst_FxAttr->feedback = 0;

		L_sceSdSetSwitch(core | SD_S_VMIXEL, iopSND_dst_FxSettings[core].i_VMIXER);
		L_sceSdSetSwitch(core | SD_S_VMIXER, iopSND_dst_FxSettings[core].i_VMIXER);
		L_sceSdSetSwitch(core | SD_S_VMIXL, iopSND_dst_FxSettings[core].i_VMIXR);
		L_sceSdSetSwitch(core | SD_S_VMIXR, iopSND_dst_FxSettings[core].i_VMIXR);

		/* set default attribute */
		L_sceSdSetEffectAttr(core, iopSND_dst_FxSettings[core].pst_FxAttr);
		L_sceSdSetCoreAttr(core | SD_C_EFFECT_ENABLE, 1);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_FxCloseModule(void)
{
	/*~~~~~*/
	int core;
	/*~~~~~*/

	for(core = SD_CORE_0; core < SD_CORE_MAX; core++)
	{
		L_sceSdSetSwitch(core | SD_S_VMIXEL, 0);
		L_sceSdSetSwitch(core | SD_S_VMIXER, 0);

		L_sceSdSetCoreAttr(core | SD_C_EFFECT_ENABLE, 0);
		L_sceSdSetCoreAttr(core | SD_C_EFFECT_ENABLE, 0);
		iopMEM_iopFreeAlign(iopSND_dst_FxSettings[core].pst_FxAttr);
	}

	L_memset(&iopSND_dst_FxSettings, 0, sizeof(iopSND_tdst_FxSettings));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_FxSet(RPC_tdst_FxSettings *_pFx)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						core;
	iopSND_tdst_FxSettings	*pMyFx;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pMyFx = iopSND_dst_FxSettings;
	for(core = SD_CORE_0; core < SD_CORE_MAX; core++, _pFx++, pMyFx++)
	{
		if(!_pFx->ul_Modif) continue;

		/*$2- mode ---------------------------------------------------------------------------------------------------*/
		if(_pFx->ul_Modif & ps2SND_Cte_FxModifyMode)
		{
			switch(_pFx->mode)
			{
			case SD_REV_MODE_OFF:
			case SD_REV_MODE_ROOM:
			case SD_REV_MODE_STUDIO_A:
			case SD_REV_MODE_STUDIO_B:
			case SD_REV_MODE_STUDIO_C:
			case SD_REV_MODE_HALL:
			case SD_REV_MODE_SPACE:
			case SD_REV_MODE_ECHO:
			case SD_REV_MODE_DELAY:
			case SD_REV_MODE_PIPE:
				break;

			default:
				iopDbg_M_ErrX(iopDbg_Err_0074 ":%d", _pFx->mode);
				_pFx->mode =SD_REV_MODE_OFF;
				break;
			}
			pMyFx->pst_FxAttr->mode = _pFx->mode;
			
			/* Apply new SFX mode */
			sceSdSetEffectMode(core, iopSND_dst_FxSettings[core].pst_FxAttr);
			sceSdSetEffectModeParams(core, iopSND_dst_FxSettings[core].pst_FxAttr);
		}
		
		/*$2- delay --------------------------------------------------------------------------------------------------*/

		if(_pFx->ul_Modif & ps2SND_Cte_FxModifyDelay)
		{
			pMyFx->pst_FxAttr->delay = _pFx->delay;
			iopDbg_M_Assert((0 <= _pFx->delay) && (_pFx->delay <= 127), iopDbg_Err_0091)
			
			/* Apply new SFX param */
			sceSdSetEffectModeParams(core, iopSND_dst_FxSettings[core].pst_FxAttr);
		}

		/*$2- feedback -----------------------------------------------------------------------------------------------*/

		if(_pFx->ul_Modif & ps2SND_Cte_FxModifyFeedback)
		{
			pMyFx->pst_FxAttr->feedback = _pFx->feedback;
			iopDbg_M_Assert((0 <= _pFx->feedback) && (_pFx->feedback <= 127), iopDbg_Err_0091)
			
			/* Apply new SFX param */
			sceSdSetEffectModeParams(core, iopSND_dst_FxSettings[core].pst_FxAttr);
		}
		
		/*$2- wet vol ------------------------------------------------------------------------------------------------*/

		if(_pFx->ul_Modif & ps2SND_Cte_FxModifyWetVol)
		{
			pMyFx->pst_FxAttr->depth_L = _pFx->depth_L;
			pMyFx->pst_FxAttr->depth_R = _pFx->depth_R;
			/*Apply new SFX volume */
			sceSdSetParam(core|SD_P_EVOLL,pMyFx->pst_FxAttr->depth_L);
			sceSdSetParam(core|SD_P_EVOLR,pMyFx->pst_FxAttr->depth_R);
		}

	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_FxAddVoice(int _i_voice, int core)
{

	/*~~~~~*/
	int R;
	iopSND_tdst_HardBuffer*pst_HardBuffer, *pst_HardBufferLeft;
	/*~~~~~*/

	R = ps2SND_M_GetIdxFromSB(_i_voice);
	iopSND_M_CheckSoftBuffIdxOrReturn(_i_voice, ;);
	
	core = (core == SD_CORE_0) ? iopSND_Cte_UseFxA : iopSND_Cte_UseFxB;
	
	__SoftLock__;
	pst_HardBuffer = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[R].pst_HardBuffer;
	pst_HardBufferLeft = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[R].pst_HardBufferLeft;
	iopSND_sa_SoftBuffer[R].pst_HardBuffer = NULL;
	iopSND_sa_SoftBuffer[R].pst_HardBufferLeft = NULL;
	iopSND_sa_SoftBuffer[R].ui_Flag &= ~(iopSND_Cte_UseFxA | iopSND_Cte_UseFxB);
	iopSND_sa_SoftBuffer[R].ui_Flag |= core;
	__SoftUnlock__;

	iopSND_FxDelHardBuffer( pst_HardBufferLeft, pst_HardBuffer);
	
	if(pst_HardBuffer) iopSND_HardFreeBuffer(pst_HardBuffer);	
	if(pst_HardBufferLeft) iopSND_HardFreeBuffer(pst_HardBufferLeft);

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_FxDelVoice(int _i_voice)
{

	/*~~~~~*/
	int R;
	iopSND_tdst_HardBuffer*pst_HardBuffer, *pst_HardBufferLeft;
	/*~~~~~*/

	R = ps2SND_M_GetIdxFromSB(_i_voice);
	iopSND_M_CheckSoftBuffIdxOrReturn(R, ;);
	
	__SoftLock__;
	pst_HardBuffer = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[R].pst_HardBuffer;
	pst_HardBufferLeft = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[R].pst_HardBufferLeft;
	iopSND_sa_SoftBuffer[R].pst_HardBuffer = NULL;
	iopSND_sa_SoftBuffer[R].pst_HardBufferLeft = NULL;
	iopSND_sa_SoftBuffer[R].ui_Flag &= ~(iopSND_Cte_UseFxA | iopSND_Cte_UseFxB);
	__SoftUnlock__;

	iopSND_FxDelHardBuffer(pst_HardBufferLeft, pst_HardBuffer);

	if(pst_HardBuffer) iopSND_HardFreeBuffer(pst_HardBuffer);
	if(pst_HardBufferLeft) iopSND_HardFreeBuffer(pst_HardBufferLeft);

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_FxAddHardBuffer(unsigned int ui_Flag, iopSND_tdst_HardBuffer *pst_hbL, iopSND_tdst_HardBuffer *pst_hbR)
{
	/*~~~~~~~~~~~~~~~*/
	int iMaskL, iMaskR;
	/*~~~~~~~~~~~~~~~*/

	if(ui_Flag & (iopSND_Cte_UseFxA | iopSND_Cte_UseFxB) == 0) return;
	if(!pst_hbR) return;
	if(!pst_hbL) pst_hbL = pst_hbR;


	iMaskR = (1 << pst_hbR->i_Voice);
	iMaskL = (1 << pst_hbL->i_Voice);

	__SoftLock__;
	iopSND_dst_FxSettings[pst_hbR->i_Core].i_VMIXER &= ~iMaskR;
	iopSND_dst_FxSettings[pst_hbL->i_Core].i_VMIXEL &= ~iMaskL;
	iopSND_dst_FxSettings[pst_hbR->i_Core].i_VMIXR &= ~iMaskR;
	iopSND_dst_FxSettings[pst_hbL->i_Core].i_VMIXL &= ~iMaskL;

	if(ui_Flag & iopSND_Cte_FxMixR) iopSND_dst_FxSettings[pst_hbR->i_Core].i_VMIXER |= iMaskR;
	if(ui_Flag & iopSND_Cte_FxMixL) iopSND_dst_FxSettings[pst_hbL->i_Core].i_VMIXEL |= iMaskL;
	if(ui_Flag & iopSND_Cte_MixR) iopSND_dst_FxSettings[pst_hbR->i_Core].i_VMIXR |= iMaskR;
	if(ui_Flag & iopSND_Cte_MixL) iopSND_dst_FxSettings[pst_hbL->i_Core].i_VMIXL |= iMaskL;
	__SoftUnlock__;

	L_sceSdSetSwitch(pst_hbR->i_Core | SD_S_VMIXEL, iopSND_dst_FxSettings[pst_hbR->i_Core].i_VMIXEL);
	L_sceSdSetSwitch(pst_hbR->i_Core | SD_S_VMIXER, iopSND_dst_FxSettings[pst_hbR->i_Core].i_VMIXER);
	L_sceSdSetSwitch(pst_hbR->i_Core | SD_S_VMIXL, iopSND_dst_FxSettings[pst_hbR->i_Core].i_VMIXL);
	L_sceSdSetSwitch(pst_hbR->i_Core | SD_S_VMIXR, iopSND_dst_FxSettings[pst_hbR->i_Core].i_VMIXR);
	
	
	if(pst_hbR->i_Core != pst_hbL->i_Core)
	{
		L_sceSdSetSwitch(pst_hbL->i_Core | SD_S_VMIXEL, iopSND_dst_FxSettings[pst_hbL->i_Core].i_VMIXEL);
		L_sceSdSetSwitch(pst_hbL->i_Core | SD_S_VMIXER, iopSND_dst_FxSettings[pst_hbL->i_Core].i_VMIXER);
		L_sceSdSetSwitch(pst_hbL->i_Core | SD_S_VMIXL, iopSND_dst_FxSettings[pst_hbL->i_Core].i_VMIXL);
		L_sceSdSetSwitch(pst_hbL->i_Core | SD_S_VMIXR, iopSND_dst_FxSettings[pst_hbL->i_Core].i_VMIXR);
	}

	pst_hbR->ui_Flag |= iopSND_Cte_HardUseFx;
	pst_hbL->ui_Flag |= iopSND_Cte_HardUseFx;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_FxDelHardBuffer(iopSND_tdst_HardBuffer *pst_hbL, iopSND_tdst_HardBuffer *pst_hbR)
{
	/*~~~~~~~~~~~~~~~*/
	int iMaskL, iMaskR;
	/*~~~~~~~~~~~~~~~*/

	if(!pst_hbR) return;
	if(!pst_hbL) pst_hbL = pst_hbR;

	iMaskR = (1 << pst_hbR->i_Voice);
	iMaskL = (1 << pst_hbL->i_Voice);

	__SoftLock__;
	iopSND_dst_FxSettings[pst_hbR->i_Core].i_VMIXER &= ~iMaskR;
	iopSND_dst_FxSettings[pst_hbL->i_Core].i_VMIXEL &= ~iMaskL;
	iopSND_dst_FxSettings[pst_hbR->i_Core].i_VMIXR &= ~iMaskR;
	iopSND_dst_FxSettings[pst_hbL->i_Core].i_VMIXL &= ~iMaskL;
	__SoftUnlock__;

	L_sceSdSetSwitch(pst_hbR->i_Core | SD_S_VMIXEL, iopSND_dst_FxSettings[pst_hbR->i_Core].i_VMIXEL);
	L_sceSdSetSwitch(pst_hbR->i_Core | SD_S_VMIXER, iopSND_dst_FxSettings[pst_hbR->i_Core].i_VMIXER);
	L_sceSdSetSwitch(pst_hbR->i_Core | SD_S_VMIXL, iopSND_dst_FxSettings[pst_hbR->i_Core].i_VMIXL);
	L_sceSdSetSwitch(pst_hbR->i_Core | SD_S_VMIXR, iopSND_dst_FxSettings[pst_hbR->i_Core].i_VMIXR);
	if(pst_hbR->i_Core != pst_hbL->i_Core)
	{
		L_sceSdSetSwitch(pst_hbL->i_Core | SD_S_VMIXEL, iopSND_dst_FxSettings[pst_hbL->i_Core].i_VMIXEL);
		L_sceSdSetSwitch(pst_hbL->i_Core | SD_S_VMIXER, iopSND_dst_FxSettings[pst_hbL->i_Core].i_VMIXER);
		L_sceSdSetSwitch(pst_hbL->i_Core | SD_S_VMIXL, iopSND_dst_FxSettings[pst_hbL->i_Core].i_VMIXL);
		L_sceSdSetSwitch(pst_hbL->i_Core | SD_S_VMIXR, iopSND_dst_FxSettings[pst_hbL->i_Core].i_VMIXR);
	}

	pst_hbR->ui_Flag &= ~iopSND_Cte_HardUseFx;
	pst_hbL->ui_Flag &= ~iopSND_Cte_HardUseFx;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
