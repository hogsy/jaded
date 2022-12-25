#ifndef __GXI_AFTERFX_H__
#define __GXI_AFTERFX_H__

#include "GXI_GC/GXI_Def.h"

#include "MATHs/MATH.h"
#include "BASe/BAStypes.h"

/*$4
 ***************************************************************************************************
    Function
 ***************************************************************************************************
 */

void GXI_AfterFX_Init();
void GXI_AE_MotionBlur(GXTexObj *g_Texture,UINT _factor);
void GXI_AE_ShowTexture();

void GXI_AfterFX();

#endif /* __GXI_AFTERFX_H__ */