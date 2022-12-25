/*$T MTX.h GC! 1.081 07/29/02 12:22:11 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MTX_h__
#define __MTX_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$2- IPU file -------------------------------------------------------------------------------------------------------*/

#define MTX_Cte_VideoFileId			'mupi'
#define MTX_Cte_FrameDelimiter		0xb0010000
#define MTX_Cte_EndFrameDelimiter	0xb1010000

/*$2- MTX file -------------------------------------------------------------------------------------------------------*/

#define MTX_Cte_FileId		' xtm'
#define MTX_Cte_FileVersion 0x0001001

/*$2- EE cache config ------------------------------------------------------------------------------------------------*/

#define MTX_Cte_CacheNb		2
#define MTX_Cte_CacheSize	(300 * 1024 + 16)

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    type for MTX file
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef PSX2_TARGET
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif
/**/
typedef struct	MTX_tdst_IPUHeader_
{
	char			asz_ID[4];			/* 0 4 bytes ID (='ipum') */
	unsigned int	ui_DataSize;		/* 4 4 bytes Data size (number of bytes) */
	unsigned short	us_ImageWidth;		/* 8 2 bytes Image width (number of pixels) */
	unsigned short	us_ImageHeight;		/* 10 2 bytes Image height (number of pixels) */
	unsigned int	ui_NumberOfFrames;	/* 12 4 bytes Number of frames */
} MTX_tdst_IPUHeader;

typedef struct	MTX_tdst_VideoHeader_
{
	unsigned int	ui_VideoBufferSize;
	unsigned int	ui_FileSize;
} MTX_tdst_VideoHeader;

typedef struct	MTX_tdst_SoundHeader_
{
	unsigned int	ui_SoundBufferSize;
	unsigned int	ui_Channel;
	unsigned int	ui_DataSize;
	unsigned int	ui_Freq;
	float			f_Vol;
} MTX_tdst_SoundHeader;

typedef struct	MTX_tdst_MtxFileHeader_
{
	unsigned int			ui_FileID;
	unsigned int			ui_FileVersion;
	unsigned int			ui_FileSize;
	MTX_tdst_VideoHeader	st_Video;
	MTX_tdst_SoundHeader	st_Sound;
} MTX_tdst_MtxFileHeader;
/**/
#ifdef PSX2_TARGET
#pragma pack(1)
#else
#pragma pack(pop, 1)
#endif

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	MTX_tdst_Interface_
{
	void (*pfv_StartMtx) (void);
	void (*pfv_RestartMtx) (void);
	void (*pfv_KillMtx) (void);
}
MTX_tdst_Interface;

typedef struct	MTX_tdst_GlobalStruct_
{
	unsigned int			ui_CurrentValidity;
	unsigned int			ui_FileKey;
	unsigned int			ui_FilePos;
	unsigned int			ui_FileSize;
	MTX_tdst_Interface		i;
	unsigned int			ui_DataPosition;
	MTX_tdst_VideoHeader	st_VideoHeader;
	MTX_tdst_SoundHeader	st_SoundHeader;
} MTX_tdst_GlobalStruct;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    type for input file parser
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	MTX_tdst_VideoInfo_
{
	unsigned int	ui_FileKey;
	unsigned int	ui_FilePos;
	unsigned int	ui_FileSize;
	unsigned int	ui_ImageNb;
	unsigned int	ui_ImageSizeMax;
	unsigned int	ui_AvgOctBySeconds;
	unsigned int	*aui_ImageOffset;
	unsigned int	*aui_ImageSize;
} MTX_tdst_VideoInfo;

typedef struct	MTX_tdst_SoundInfo_
{
	unsigned int	ui_FileKey;
	unsigned int	ui_FilePos;
	unsigned int	ui_FileSize;
	unsigned int	ui_DataPos;
	unsigned int	ui_DataSize;
	char			*pc_Buffer;
	unsigned int	ui_OctBySeconds;
	int				i_Freq;
	float			f_Duration;
	float			f_PlayerVol;
	int				i_PlayerFreq;
	unsigned int	ui_Channel;
	char			*pc_Data;
} MTX_tdst_SoundInfo;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

void			MTX_InitModule(void);
void			MTX_CloseModule(void);
ULONG			MTX_ul_CallbackLoadFile(ULONG _ul_Key);
void			MTX_UnloadFile(void);
void			MTX_Start(void);
void			MTX_Restart(void);
void			MTX_Kill(void);
BOOL			MTX_b_CurrentFileIsMTX(void);
unsigned int	MTX_ui_GetVideoBufferSize(void);
unsigned int	MTX_ui_GetVideoBufferNumber(void);
unsigned int	MTX_ui_GetVideoFilePosition(void);
unsigned int	MTX_ui_GetVideoFileSize(void);
unsigned int	MTX_ui_GetSoundBufferNumber(void);
unsigned int	MTX_ui_GetSoundBufferSize(void);
unsigned int	MTX_ui_GetSoundFrequency(void);
float			MTX_f_GetSoundVolume(void);
unsigned int	MTX_ui_GetSoundDataSize(void);
unsigned int	MTX_ui_GetStreamPosition(void);
unsigned int	MTX_ui_GetStreamSize(void);
unsigned int	MTX_ui_GetSoundChannelNb(void);

#ifdef ACTIVE_EDITORS
int				MTX_i_ParseSoundFile(MTX_tdst_SoundInfo *, ULONG _ul_Key);
int				MTX_i_ParseVideoFile(MTX_tdst_VideoInfo *, ULONG _ul_Key);
void			MTX_FreeVideoFileInfo(MTX_tdst_VideoInfo *);
void			MTX_FreeSoundFileInfo(MTX_tdst_SoundInfo *);
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __MTX_h__ */
