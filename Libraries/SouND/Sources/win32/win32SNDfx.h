/*$T win32SNDfx.h GC 1.138 04/07/04 16:07:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __win32SNDfx_h__
#define __win32SNDfx_h__
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
int		win32SND_i_FxInit(void);
void	win32SND_FxClose(void);
int		win32SND_i_FxSetMode(int, int);
int		win32SND_i_FxSetDelay(int, int);
int		win32SND_i_FxSetFeedback(int, int);
int		win32SND_i_FxSetWetVolume(int, int);
int		win32SND_i_FxSetWetPan(int, int);
int		win32SND_i_FxEnable(int);
int		win32SND_i_FxDisable(int);
void    win32SND_FxDel(struct SND_tdst_SoundBuffer_*_pst_SB);
void    win32SND_FxAdd(struct SND_tdst_SoundBuffer_*_pst_SB, int _i_Core, int _i_FxVol);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
