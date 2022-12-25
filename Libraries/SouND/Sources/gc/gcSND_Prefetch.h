/*$T gcSND_Prefetch.h GC 1.138 12/08/04 09:25:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef _GAMECUBE
#ifndef __gcSND_Prefetch_h__
#define __gcSND_Prefetch_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    TYPES
 ***********************************************************************************************************************
 */

typedef enum	gcSND_tden_PrefetchState_
{
	e_PrefetchFree			= 0,
	e_PrefetchLoadingRam	= 1,
	e_PrefetchLoadingAram	= 2,
	e_PrefetchWaitingForUse = 3,
	e_PrefetchRestoringRam	= 4
} gcSND_tden_PrefetchState;

typedef struct	gcSND_tdst_Prefetch_
{
	gcSND_tden_PrefetchState volatile	en_State;
	struct GC_tdst_StreamHandler_ * volatile pst_SoundFileHandler;
	unsigned int	ui_PrefetchPosition;
	unsigned int	ui_PrefetchSize;
	char			*pc_AramBuffer;
	char *volatile pc_RamBuffer;
	ARQRequest						st_ARQRequest;

	struct GC_tdst_StreamHandler_	*pst_UserStreamHandler;
	char							*pc_UserBufferPointer;
	unsigned int					ui_UserBufferSize;
	unsigned int					ui_UserFilePointer;
	void							*pfv_UserCallBack;
	unsigned int					ui_UserContext;
} gcSND_tdst_Prefetch;

/*$4
 ***********************************************************************************************************************
    PROTOTYPES
 ***********************************************************************************************************************
 */

void	gcSND_StreamPrefetch(unsigned int, unsigned int);
void	gcSND_StreamFlush(unsigned int);
void	gcSND_StreamPrefetchArray(unsigned int *, unsigned int *);
void	gcSND_StreamGetPrefetchStatus(unsigned int *auiPos);
int		Overloaded_GC_i_StreamRead
		(
			struct GC_tdst_StreamHandler_	*_pst_Stream,
			char							*_pc_Buff,
			unsigned int					_ui_Size,
			unsigned int					_ui_Position,
			void							*_pfv_CallBack,
			unsigned int					_ui_Context
		);

/*$4
 ***********************************************************************************************************************
    VARIABLES
 ***********************************************************************************************************************
 */

extern gcSND_tdst_Prefetch	gcSND_gst_PrefetchList[];
extern volatile int			gcSND_gi_PendingPrefetch;

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __gcSND_Stream_h__ */
#endif /* _GAMECUBE */
