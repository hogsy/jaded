/*$T SONvumeter.h GC! 1.081 09/25/02 10:57:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SONVUMETER_H__
#define __SONVUMETER_H__

#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "DIAlogs/DIAsndvumeter.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */


#define ESON_Cte_CaptureNotificationNb	16
#define ESON_Cte_CaptureDriverMaxNb		20


#define ESON_Cte_CaptureFormat_2x8bits22kHz		10
#define ESON_Cte_CaptureFormat_2x16bits44kHz	15
#define ESON_Cte_CaptureFormatMaxNb				16

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

typedef struct	ESON_tdst_VUmeter_
{
	int i_MaxLevel;
	int i_Scale;
	int i_CurrentLevel;
	int i_PeakLevel;
	int i_PeakAge;
	int i_PeakTTL;
	int i_Falloff;
} ESON_tdst_VUmeter;

typedef struct	ESON_tdst_StereoVUmeter_
{
	ESON_tdst_VUmeter	st_Left;
	ESON_tdst_VUmeter	st_Right;
} ESON_tdst_StereoVUmeter;

typedef struct	ESON_tdst_AudioDrivers_
{
	unsigned int	ui_MaxNb;
	unsigned int	ui_SelIndex;
	GUID			dst_GUIDs[ESON_Cte_CaptureDriverMaxNb];
	GUID			*dpst_pGUIDs[ESON_Cte_CaptureDriverMaxNb];
	char			*dasz_Name[ESON_Cte_CaptureDriverMaxNb];
} ESON_tdst_AudioDrivers;

typedef struct	ESON_tdst_CaptureNotification_
{
	LPDIRECTSOUNDNOTIFY volatile	po_Interface;
	DSBPOSITIONNOTIFY				dst_PosNotify[ESON_Cte_CaptureNotificationNb + 1];
	volatile unsigned int			ui_NextCaptureOffset;
	unsigned int					ui_NotifySize;
	HANDLE							h_NotificationEvent;
} ESON_tdst_CaptureNotification;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class	ESON_cl_VUmeter
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	ESON_cl_VUmeter(void);
	~ESON_cl_VUmeter();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	/* audio drivers */
    ESON_tdst_AudioDrivers			mst_AudioDrivers;
	
    /* capture devices */
    BOOL							mab_CaptureFormatSupported[ESON_Cte_CaptureFormatMaxNb];
	LPDIRECTSOUNDCAPTURE			mpo_DirectSoundCaptureDevice;
	LPDIRECTSOUNDCAPTUREBUFFER		mpo_DirectSoundCaptureBuffer;
	WAVEFORMATEX					mst_CaptureFormatDesc;
	unsigned int					mui_CaptureBufferSize;
    BOOL							mb_IsRecording;
	
    /* notification device */
    ESON_tdst_CaptureNotification	mst_CaptureNotificationManager;

    /* capture thread */
	HANDLE							mh_ThreadHandle;
	unsigned long					mul_ThreadId;
	HANDLE							mh_KillThreadEvent;
	HANDLE							mh_KillThreadOrderDone;

    /* captured data */
	char							*mapv_CapturedDataBuffer[ESON_Cte_CaptureNotificationNb];
	unsigned int					mui_CapturedDataBufferIndex;
	unsigned int					mui_CapturedDataBufferSize;

    /* draw window */
    EDIA_cl_SndVumeterDialog*       mpo_SndVumeterDialog;	    
    ESON_tdst_StereoVUmeter         mst_VumeterValues;

    int						        mi_PeakTTL ;	
    int						        mi_Falloff ;	
    int						        mi_MaxScale ;	
    int						        mi_Scale ;		

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MEMBERS
 -----------------------------------------------------------------------------------------------------------------------
 */

public:

	void	OnGetCaptureDriverList(void);
	int		OnSelCaptureDriver(unsigned int _ui_DriverIndex = 0);

	void	OnGetCaptureFormatList(void);
	int		OnSelCaptureFormat(unsigned int _ui_FormatIndex = ESON_Cte_CaptureFormat_2x16bits44kHz);

	int		OnCreateCaptureBuffer(WAVEFORMATEX *);
    int		OnCreateCaptureNotifications(void);
	void	OnResizeTransfertBuffer(int _i_NewSize);
	int		OnStartStopCapture(BOOL bStartRecording);
    int		OnCaptureNotication(void);
    void	SendDataToWindow(unsigned int , char *);
    void    TreatNewData(char*lpData, int dwBytesRecorded);

	void	OnCreateWindow(void);
	void	OnDestroyWindow(void);
	int		OnShowWindow(void);
	int		OnHideWindow(void);
    void    OnDrawWindow(HWND);

    void	GetWaveFormatFromIndex(int nIndex, WAVEFORMATEX *); 

    void	ValueUpdate(ESON_tdst_VUmeter *, int newlevel);
    void	ValueReset(ESON_tdst_VUmeter *);
    void	ValueResetAll(void);
};

#endif /* ACTIVE_EDITORS */

#endif /* __SONVIEW_H */
