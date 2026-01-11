/*$T TEXfile.h GC! 1.081 04/06/00 09:28:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __TEXANI_H__
#define __TEXANI_H__


#ifdef __cplusplus
extern "C"
{
#endif

ULONG	TEX_ANI_LoadIfAnim(BIG_KEY _ul_FileKey);
void	TEX_ANI_CleanAll();


#ifdef __cplusplus
}
#endif
#endif /* __TEXANI_H__ */
