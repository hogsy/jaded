#ifndef __GXI_INIT_H__
#define __GXI_INIT_H__

#include "GXI_GC/GXI_Def.h"

#include "MATHs/MATH.h"
#include "BASe/BAStypes.h"
#include "SOFT/SOFTstruct.h"

#include "BIGfiles/LOAding/LOAread.h" // DJ_TEMP : sortir le code de swap dans un fichier swapper.h

//#define FALSE_640




#undef USE_SELF_SHADOWS


/*$4
 ***************************************************************************************************
    Function
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Close / init / reinit
 ===================================================================================================
 */
void					GXI_Init(void);
void				 	GXI_Setup();

/*
 ===================================================================================================
    Flip / clear / surface status
 ===================================================================================================
 */
void                    GXI_Flip(void);
void                    GXI_Clear(LONG, ULONG);

inline u32 GXI_ColorABGR2RGBA(u32 _abgr)
{
	u32 swap = _abgr;
	SwapDWord((LONG*)&swap);
	return swap;
}	

/*
 ===================================================================================================
    Render function
 ===================================================================================================
 */
void                    GXI_SetViewMatrix(MATH_tdst_Matrix *);
void                    GXI_SetProjectionMatrix(struct CAM_tdst_Camera_ *);
void                    GXI_SetTextureBlending( ULONG, ULONG , ULONG );

void					GXI_SetViewMatrix_SDW(struct MATH_tdst_Matrix_ *_pst_Matrix , float *Limits);
void 					GXI_SetTextureTarget(u32 Num , u32 Clear);

extern BOOL GXI_b3DRendering;

/*
 ===================================================================================================
    Draw primitives
 ===================================================================================================
 */
LONG                    GXI_l_DrawElementIndexedTriangles( struct GEO_tdst_ElementIndexedTriangles_ *, GEO_Vertex *, MATH_tdst_Vector *,struct GEO_tdst_UV_ * , ULONG);
LONG                    GXI_l_DrawElementIndexedSprite(struct GEO_tdst_ElementIndexedSprite_		*_pst_Element,GEO_Vertex	*,	ULONG);

/*
 ===================================================================================================
    Debug
 ===================================================================================================
 */
void GXI_DrawText(int X,int Y,char *P_String,long Color,long ColorBack,long TFront,long TBack);

/*
 ===================================================================================================
    globals extern declarations
 ===================================================================================================
 */

#undef USE_PERPECTIVE_CORRECT_COLORS

extern GXColor GX_WHITE;
extern GXColor GX_BLACK;

#ifdef USE_PERPECTIVE_CORRECT_COLORS
extern GXColor GX_RG;
extern GXColor GX_BA;
#endif

extern GXI_BigStruct gs_st_Globals;
#define GXI_Global_ACCESS(a) gs_st_Globals.a

extern ULONG gul_FRAME_BUFFER_WIDTH;
extern ULONG gul_FRAME_BUFFER_HEIGHT;

#define FRAME_BUFFER_WIDTH gul_FRAME_BUFFER_WIDTH
#define FRAME_BUFFER_HEIGTH gul_FRAME_BUFFER_HEIGHT

extern float WIDE_SCREEN_ADJUST;

#define WatertexHeight 100
#define WatertexWidth 512
#define FrameBufferHeight 528

//#define USE_MY_TEX_REGION

#define USE_TRIPPLE_BUFFERING
#define DONT_USE_ALPHA_DEST
//#define GIGSCALE0 20.0f
#undef GIGSCALE0

#endif /* __GXI_INIT_H__ */