/*$T GC_Stream.h GC! 1.097 01/15/02 10:10:50 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef _GAMECUBE
#ifndef __GC_Stream_h__
#define __GC_Stream_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define GC_Cte_StreamMaxNumber		20

#define GC_Cte_StreamStateFree		0
#define GC_Cte_StreamStateIdle		1
#define GC_Cte_StreamStateWorking	2

#define GC_Cte_StreamErrorNone		0
#define GC_Cte_StreamErrorFatal		1

#define lRoundDown4B(x)				(((u32) (x)) &~(4 - 1))

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef void (*GC_tdpfv_StreamCallBack) (s32 result, DVDFileInfo * fileInfo, unsigned int context);

typedef struct	GC_tdst_StreamHandler_
{
	/* file system info */
	DVDFileInfo				st_FileInfo;

	/* file current info */
	char					*pc_FileBuffer;
	unsigned int			ui_FileBufferSize;
	int	volatile					i_CurrentState;
	int						i_ErrorCode;

	unsigned int			ui_CurrentPosition;
	unsigned int			ui_CurrentDeltaAlign;
	unsigned int			ui_CurrentReadSize;
	unsigned int			ui_CurrentWriteSize;
	char					*pc_CurrentWritePtr;

	/* user info */
	int						i_UserPriority;
	char					*pc_UserBuffer;
	unsigned int			ui_UserBufferSize;
	unsigned int			ui_UserPosition;
	GC_tdpfv_StreamCallBack pfv_UserCallBack;
	unsigned int			ui_UserContext;
} GC_tdst_StreamHandler;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

GC_tdst_StreamHandler	*GC_pst_StreamOpen(tdstGC_File *_pst_SrcFileHandler, int _i_Priority);
void					GC_StreamClose(GC_tdst_StreamHandler *_pst_Stream);
int						GC_i_StreamRead
						(
							GC_tdst_StreamHandler	*_pst_Stream,
							char					*_pc_Buff,
							unsigned int			_ui_Size,
							unsigned int			_ui_Position,
							GC_tdpfv_StreamCallBack _pfv_CallBack,
							unsigned int			_ui_Context
						);
void					GC_StreamInitModule(void);
void					GC_StreamCloseModule(void);
void					GC_StreamCancel(GC_tdst_StreamHandler *_pst_Stream);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __GC_Stream_h */
#endif /* _GAMECUBE */
