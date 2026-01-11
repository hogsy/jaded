/*$T ANIsave.h GC! 1.077 03/13/00 15:59:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __ANISAVE_H__
#define __ANISAVE_H__

#ifndef PSX2_TARGET
#pragma once
#endif
#include "BASe/BAStypes.h"
#include "TABles/TABles.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern ULONG	ANI_ul_SaveShape(ANI_tdst_Shape *, BIG_KEY);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __ANISAVE_H__ */
