#ifndef __GXI_FONT_H__
#define __GXI_FONT_H__

/*
 ===================================================================================================
    Functions
 ===================================================================================================
 */

void GXI_init_font(void);
void GXI_DrawText(int X,int Y,char *P_String,long Color,long ColorBack,long TFront,long TBack);

#endif 