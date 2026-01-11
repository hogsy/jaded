/*$T SOFTbackgroundimage.h GC! 1.081 05/09/00 10:30:28 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __SOFTBACKGROUNDIMAGE_H__
#define __SOFTBACKGROUNDIMAGE_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    constants
 ***********************************************************************************************************************
 */

#define SOFT_C_BI_Init		            0x00000001
#define SOFT_C_BI_Visible	            0x00000002
#define SOFT_C_BI_Freeze	            0x00000004
#define SOFT_C_BI_Mpeg		            0x00000008
#define SOFT_C_BI_Reload	            0x00000010
#define SOFT_C_BI_NotInBF               0x00000020
#define SOFT_C_BI_MPEG_BlackAndWhite    0x00010000

/*$4
 ***********************************************************************************************************************
    Structure
 ***********************************************************************************************************************
 */

typedef struct	SOFT_tdst_BackgroundImage_
{
	ULONG					    ul_Flags;

    char                        sz_Name[64];
    ULONG                       ul_Key;

	/* params for picture list */
	ULONG					    ul_PLKey;
	ULONG					    ul_PLLastKey;
	char					    sz_PLFile[260];
	int						    i_FirstIndex;
	int						    i_LastIndex;

	/* params for mpeg */
	ULONG					    ul_MpegKey;
	ULONG					    ul_MpegIndex;
    char					    sz_MpegFile[260];
    ULONG                       ul_MpegOffset;
    ULONG                       ul_MpegEndOffset;
	FILE					    *hp_File;
	struct MPG_IOStruct_	    *pst_MpegIO;

	/* data for refresh */
	int						    i_Counter;

	/* data for window position */
	float					    x, y, depth, w, h;
    float                       ImageRatio;

	/* buffers */
	int						    i_Number;
    int                         i_NumberOfImages;
	char					    **dpc_Image;
	short			            w_Texture;

    /* data for linking to anim */
    struct OBJ_tdst_GameObject_ *pst_Gao;
    short                       w_AnimFreq;
    short                       w_FilmFreq;
    float                       f_FreqRatio;

} SOFT_tdst_BackgroundImage;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void	SOFT_BackgroundImage_MainInit(void);
void	SOFT_BackgroundImage_MainClose(void);

void	SOFT_BackgroundImage_AddTexture();
void	SOFT_BackgroundImage_Init(SOFT_tdst_BackgroundImage *);
void	SOFT_BackgroundImage_Close(SOFT_tdst_BackgroundImage *);
void	SOFT_BackgroundImage_Update(SOFT_tdst_BackgroundImage *, struct GDI_tdst_DisplayData_ *);
void	SOFT_BackGroundImage_MoveWindow(SOFT_tdst_BackgroundImage *, float, float, float, float, float);

void    SOFT_BackGroundImage_SetTextureInfo( SOFT_tdst_BackgroundImage *, struct TEX_tdst_File_Desc_ * );
void    SOFT_BackGroundImage_SetAnimGao( SOFT_tdst_BackgroundImage *, struct OBJ_tdst_GameObject_ * );

void    SOFT_BackgroundImage_Save( SOFT_tdst_BackgroundImage * );
void    SOFT_BackgroundImage_Load( SOFT_tdst_BackgroundImage *, ULONG  );

int     SOFT_i_BackgroundImage_Pick( SOFT_tdst_BackgroundImage *, int, int,  struct GDI_tdst_DisplayData_ *);
void    SOFT_BackgroundImage_MoveSize( SOFT_tdst_BackgroundImage *, int, float, float, int );

#else
#define SOFT_BackgroundImage_MainInit()
#define SOFT_BackgroundImage_MainClose()
#define SOFT_BackgroundImage_AddTexture()
#define SOFT_BackgroundImage_Init(a)
#define SOFT_BackgroundImage_Close(a)
#define SOFT_BackgroundImage_Update(a, b)
#define SOFT_BackGroundImage_MoveWindow(a, b, c, d, e, f)
#define SOFT_BackGroundImage_SetTextureInfo(a,b)
#define SOFT_BackgroundImage_SetAnimGao(a)

#define SOFT_BackgroundImage_Save(a)
#define SOFT_BackgroundImage_Load(a,b)

#define SOFT_i_BackgroundImage_Pick(a,b,c,d) 0
#define SOFT_BackgroundImage_MoveSize(a,b,c,d,e) 0


#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SOFTBACKGROUNDIMAGE_H__ */
