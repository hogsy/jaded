/*$T SND.h GC 1.138 06/22/04 15:49:58 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SND_H__
#define __SND_H__

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"

#include "BASe/BAStypes.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

extern SND_tdst_Parameters		SND_gst_Params;
extern SND_tdst_LowInterface	SND_gpst_Interface[];

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    special
 -----------------------------------------------------------------------------------------------------------------------
 */

void	SND_UpdateLang(void);
void	SND_RegisterInactiveGAO(OBJ_tdst_GameObject *);
void	SND_DestroyInstanceOfGao(OBJ_tdst_GameObject *);
void	SND_AutoPlay(SND_tdst_SoundInstance *);
void	SND_AutoStop(SND_tdst_SoundInstance *);
void	SND_ReinitOneWorld(int _i_Type);
void    SND_Reinit(void);


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    sound module function
 -----------------------------------------------------------------------------------------------------------------------
 */

void	SND_InitModule(void);
void	SND_CloseModule(void);

int		SND_i_AllocSound(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    sound interface function
 -----------------------------------------------------------------------------------------------------------------------
 */

void	SND_Update(MATH_tdst_Matrix *);
LONG	SND_l_GetSoundIndex(BIG_KEY);
void	SND_Delete(BIG_KEY);
int		SND_i_ChangeRenderMode(int _i_Mode);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    sound instance function
 -----------------------------------------------------------------------------------------------------------------------
 */

LONG	SND_l_Request(void *, LONG);
void	SND_Release(LONG);
void	SND_SetFlags(OBJ_tdst_GameObject *, LONG, ULONG);
void	SND_ResetFlags(OBJ_tdst_GameObject *, LONG _l_Index, ULONG _ul_Flags);
void	SND_SetObject(OBJ_tdst_GameObject *pGO, int index);
void	SND_SetPos(void *, LONG, MATH_tdst_Matrix *);
void	SND_SetNearFar(LONG, int, float, float);
void	SND_StopAll(int);
void	SND_SetVol(LONG, float);
float	SND_f_GetVol(LONG);
void	SND_SetWetVol(LONG, float);
float	SND_f_GetWetVol(LONG);
void	SND_SetFreq(LONG, LONG);
LONG	SND_l_GetFreq(LONG);
void	SND_SetPan(LONG, LONG, LONG);
LONG	SND_l_GetPan(LONG);
LONG	SND_l_GetFront(LONG);
#ifdef ACTIVE_EDITORS
void	SND_PlayRq(SND_tdst_SoundInstance *,int, int, int, int);
#endif
void	SND_PlaySound(LONG);
void	SND_PlaySoundInVolume(LONG);
void	SND_PlaySoundLooping(LONG, LONG);
void	SND_PlaySoundLoopingFrom(LONG _l_Index, LONG _l_LoopCount, unsigned int uiPos);

void	SND_Stop(LONG);
void	SND_StopRq(LONG);
void	SND_Pause(LONG);
int		SND_i_IsPlaying(LONG);
LONG	SND_l_GetSoundFromInstance(LONG _l_Instance);
void	SND_FreeInstance(LONG _l_Index);

int		SND_l_ActiveInstanceInsert(LONG _l_Index, LONG _idx, BOOL _b_Active, BOOL b_reinit);

void	SND_ReinitMdFPlayParam(LONG);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    General
 -----------------------------------------------------------------------------------------------------------------------
 */

void	SND_Duplicate(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *, BOOL);
int		SND_i_CheckOrInitGAO(OBJ_tdst_GameObject *);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus )&& !defined(JADEFUSION)
}
#endif
#endif /* __SND_H__ */
