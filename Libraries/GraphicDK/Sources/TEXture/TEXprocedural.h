/*$T TEXprocedural.h GC! 1.081 03/06/02 09:46:10 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Name : TEXstruct.h Description : */
#ifndef __TEXPROCEDURAL_H__
#define __TEXPROCEDURAL_H__

#ifndef PSX2_TARGET
#pragma once
#endif
#include "BIGfiles/BIGkey.h"
#include "BASe/BAStypes.h"
#include "MPEGLIB/MPG_EXPORT.H"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

/*$1- mpeg procedural texture constants ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define TEXPRO_C_Mpeg_Pause             0x01
#define TEXPRO_C_Mpeg_StopAtEnd         0x02

#define TEXPRO_C_Mpeg_ReachEnd          0x01

#define TEXPRO_C_Mpeg_RequestStop       1
#define TEXPRO_C_Mpeg_RequestRestart    2

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

/*$1- Fire procedural texture data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	TEXPRO_tdst_Fire_
{
	UCHAR	*puc_Surface[2];
	ULONG	aul_Palette[1024];
	int		i_Type;
} TEXPRO_tdst_Fire;

/*$1- water texture data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	 TEXPRO_tdst_Water_
{
	USHORT	*puc_Surface[2];
	ULONG	aul_Palette[1024];
} TEXPRO_tdst_Water;

/*$1- mpeg procedural texture data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	TEXPRO_tdst_Mpeg_
{
	ULONG					ul_Key;
	ULONG					ul_Pos;
	ULONG					ul_Offset;
	ULONG					ul_EndOffset;
	char					c_Flags;
	char					c_SystemFlags;
	char					c_Request;
	char					c_Dummy;

	/* PS2 ipu specific stream */
	ULONG					ul_PSX2_IPUKey;
	ULONG					ul_MaxBufSize;
	ULONG					ul_SizeX;
	ULONG					ul_SizeY;
	ULONG					ul_NbImages;
	ULONG					ul_Size;
#ifdef PSX2_TARGET
	struct GSP_VideoStream_ *pst_Video;
#endif
	struct MPG_IOStruct_	st_IO;

	// XBOX_E3 WITHOUT VIDEO
#if defined(_XBOX) || defined(_XENON)
	int								reloaded;
#endif

	/* GAMECUBE ipu specific stream */
	ULONG					ul_BinkSizeX;
	ULONG					ul_BinkSizeY;
	ULONG					ul_BinkBpp;
	ULONG					ul_BINK_Key;
#ifdef BINK_VIDEO
	TEX_tdst_Bink			*Bink;
#endif

	/* XBOX xmv specific stream */
	ULONG					ul_XMV_Key;
} TEXPRO_tdst_Mpeg;

/*$1- photo procedural texture data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	TEXPRO_tdst_Photo_
{
	char	c_Photo;
	char	c_AskedPhoto;
	char	c_Update;
	char	c_UpdateNow;

    ULONG   ul_CarteObj;
    float   f_Time;
    float   f_TimeLeft;
    SHORT   X[4], Y[4];

} TEXPRO_tdst_Photo;

/*$1- plasma procedural texture data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	 TEXPRO_tdst_Plasma_
{
	UCHAR	*puc_Surface[2];
	ULONG	aul_Palette[1024];
	int		i_Type;
	float	d1, d2;
} TEXPRO_tdst_Plasma;

/*
 -----------------------------------------------------------------------------------------------------------------------
    procedural texture
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	TEX_tdst_ProceduralInterface_
{
	ULONG	ul_Type;

	void * (*pfnpv_Load) (char *, LONG, ULONG, ULONG);
	void (*pfnv_Free) (struct TEX_tdst_Procedural_ *);
	void (*pfnv_Disrupt) (struct TEX_tdst_Procedural_ *);
	LONG (*pfnl_Update) (struct TEX_tdst_Procedural_ *);

#ifdef ACTIVE_EDITORS
	void (*pfnv_Save) (struct TEX_tdst_Procedural_ *);
#endif
}
TEX_tdst_ProceduralInterface;

typedef struct	TEX_tdst_Procedural_
{
	SHORT							w_Index;
	USHORT							uw_Flags;
	char							*pc_Buffer;
	USHORT							uw_Width;
	USHORT							uw_Height;

	TEX_tdst_ProceduralInterface	*i;
	void							*p_Data;
#ifdef ACTIVE_EDITORS
	ULONG							ul_Type;
	USHORT							uw_WantedWidth;
	USHORT							uw_WantedHeight;
#endif
} TEX_tdst_Procedural;

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#define TEXPRO_Unknow		0
#define TEXPRO_Water		1
#define TEXPRO_Fire			2
#define TEXPRO_Mpeg			3
#define TEXPRO_Photo		4
#define TEXPRO_Plasma		5

#define TEXPRO_Number		6

#define TEXPRO_Interface	0x0001
#define TEXPRO_UpdateOnce   0x0100
#define TEXPRO_UpdatedFromACopy   0x0200


/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

extern int							TEX_gi_NbProcedural;
extern TEX_tdst_Procedural			*TEX_gdst_Procedural;
extern TEX_tdst_ProceduralInterface TEX_gast_ProceduralInterface[TEXPRO_Number];

#ifdef ACTIVE_EDITORS
extern char							*TEX_gasz_ProceduralInterfaceName[TEXPRO_Number];
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void								TEX_Procedural_Init(void);
void								TEX_Procedural_Reinit(struct GDI_tdst_DisplayData_ *);
void								TEX_Procedural_Add(SHORT, int, USHORT, void *);
void								TEX_Procedural_AfterLoad(struct GDI_tdst_DisplayData_ *);
void								TEX_Procedural_Update(struct GDI_tdst_DisplayData_ *);
TEX_tdst_Procedural					*TEX_pst_Procedural_Get(SHORT);

void                                TEXPRO_Photo_SetMapGao( TEX_tdst_Procedural *, ULONG);
void								TEX_Procedural_UpdateWithAnother( TEX_tdst_Procedural *, TEX_tdst_Procedural * );

#ifdef ACTIVE_EDITORS
void								TEX_Procedural_Save(void);

void								TEXPRO_Mpeg_SetKey(TEX_tdst_Procedural *, ULONG);
void								TEXPRO_Mpeg_SetIPUKey(TEX_tdst_Procedural *, ULONG);
void								TEXPRO_Mpeg_SetBinkKey(TEX_tdst_Procedural *_pst_Pro, ULONG _ul_Key);
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __TEXPROCEDURAL_H__ */
