/*$T TEXhardwareload.h GC! 1.081 09/04/00 15:08:12 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __TEXTHARDWARELOAD_H__
#define __TEXTHARDWARELOAD_H__

#include "BIGfiles/BIGkey.h"
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Function header
 ***********************************************************************************************************************
 */

void TEX_Hardware_Load(struct GDI_tdst_DisplayData_ *, TEX_tdst_Data *, TEX_tdst_File_Desc *, int );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __TEXTHARDWARELOAD_H__ */
