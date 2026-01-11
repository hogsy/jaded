/*$T SONvumeter.cpp GC! 1.081 10/21/02 16:23:49 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "Editors/Sources/SOuNd/SONvumeter.h"
#include "DIAlogs/DIAsndvumeter.h"
#include "Res/Res.h"
#include "VAVview/VAVlist.h"
#include "LINks/LINKtoed.h"
#include "EDImainframe.h"
#include "ENGine/Sources/ENGvars.h"

#include "SouND/sources/SND.h"
#include "SouND/sources/SNDtrack.h"


/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define ESON_Cte_CaturedDataBufferMinSize	(4 * 1024)
#define PiBy2								((double) (1.5707963267948966192313216916398))

/*$4 */

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

#ifdef JADEFUSION
extern LONG ediSND_l_GetGlobalVol(void);
extern LONG ediSND_l_GetGlobalPan(void);
#else
extern "C" LONG ediSND_l_GetGlobalVol(void);
extern "C" LONG ediSND_l_GetGlobalPan(void);
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */


INT_PTR CALLBACK	ESON_i_DSoundEnumCallback(GUID *, const char *, const char *, void *);
unsigned int		ESON_CaptureNotificationThread(void *);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ESON_cl_VUmeter::ESON_cl_VUmeter(void)
{
	/*~~*/
	int i;
	/*~~*/

    mi_PeakTTL = 50;	
    mi_Falloff = 50;	
    mi_MaxScale = 120;	
    mi_Scale = 100;		

    mpo_SndVumeterDialog = NULL;
	/* reset all */
	
	memset(&mst_AudioDrivers, 0, sizeof(ESON_tdst_AudioDrivers));
	memset(&mst_CaptureNotificationManager, 0, sizeof(ESON_tdst_CaptureNotification));
	memset(&mst_CaptureFormatDesc, 0, sizeof(mst_CaptureFormatDesc));
	memset(mab_CaptureFormatSupported, 0, sizeof(BOOL) * ESON_Cte_CaptureFormatMaxNb);

	mh_ThreadHandle = NULL;
	mul_ThreadId = 0;
	mpo_DirectSoundCaptureDevice = NULL;
	mpo_DirectSoundCaptureBuffer = NULL;

	mb_IsRecording = FALSE;

	mui_CapturedDataBufferIndex = 0;
	mui_CapturedDataBufferSize = ESON_Cte_CaturedDataBufferMinSize;

	if(!SND_gst_Params.l_Available) return;

	mst_CaptureNotificationManager.h_NotificationEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	mh_KillThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	mh_KillThreadOrderDone = CreateEvent(NULL, FALSE, FALSE, NULL);

	for(i = 0; i < ESON_Cte_CaptureNotificationNb; i++)
	{
		mapv_CapturedDataBuffer[i] = (char *) L_malloc(ESON_Cte_CaturedDataBufferMinSize);
		memset(mapv_CapturedDataBuffer[i], 0, ESON_Cte_CaturedDataBufferMinSize);
	}

	/* Audio drivers -> create capture obj */
	OnGetCaptureDriverList();
	OnSelCaptureDriver(1);

	/* input format -> create capture buffer + notification interface */
	OnGetCaptureFormatList();
	OnSelCaptureFormat(ESON_Cte_CaptureFormat_2x16bits44kHz);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ESON_cl_VUmeter::~ESON_cl_VUmeter(void)
{
	/*~~*/
	int i;
	/*~~*/

	/* stop */
	OnHideWindow();

	if(mh_ThreadHandle) TerminateThread(mh_ThreadHandle, 0);
	mh_ThreadHandle = NULL;

	if(mst_CaptureNotificationManager.h_NotificationEvent)
		CloseHandle(mst_CaptureNotificationManager.h_NotificationEvent);
	mst_CaptureNotificationManager.h_NotificationEvent = NULL;

	if(mh_KillThreadEvent) CloseHandle(mh_KillThreadEvent);
	mh_KillThreadEvent = NULL;

	if(mh_KillThreadOrderDone) CloseHandle(mh_KillThreadOrderDone);
	mh_KillThreadOrderDone = NULL;

		

	/* Clean up everything */
	if(mst_CaptureNotificationManager.po_Interface) mst_CaptureNotificationManager.po_Interface->Release();
	mst_CaptureNotificationManager.po_Interface = NULL;

	if(mpo_DirectSoundCaptureBuffer) mpo_DirectSoundCaptureBuffer->Release();
	mpo_DirectSoundCaptureBuffer = NULL;

	if(mpo_DirectSoundCaptureDevice) mpo_DirectSoundCaptureDevice->Release();
	mpo_DirectSoundCaptureDevice = NULL;

	for(i = 0; i < ESON_Cte_CaptureNotificationNb; i++)
	{
		if(mapv_CapturedDataBuffer[i]) L_free(mapv_CapturedDataBuffer[i]);
		mapv_CapturedDataBuffer[i] = NULL;
	}

	for(i = 0; i < ESON_Cte_CaptureDriverMaxNb; i++)
	{
		if(mst_AudioDrivers.dasz_Name[i]) L_free(mst_AudioDrivers.dasz_Name[i]);
		mst_AudioDrivers.dasz_Name[i] = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_VUmeter::OnGetCaptureDriverList(void)
{
	memset(&mst_AudioDrivers, 0, sizeof(ESON_tdst_AudioDrivers));
	mst_AudioDrivers.ui_SelIndex = -1;

	DirectSoundCaptureEnumerate((LPDSENUMCALLBACK) ESON_i_DSoundEnumCallback, (void *) this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
INT_PTR CALLBACK ESON_i_DSoundEnumCallback(GUID *pGUID, const char *strDesc, const char *strDrvName, void *pContext)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	GUID			*pTemp;
	ESON_cl_VUmeter *p;
	/*~~~~~~~~~~~~~~~~~~~*/

	p = (ESON_cl_VUmeter *) pContext;
	if(p->mst_AudioDrivers.ui_MaxNb >= ESON_Cte_CaptureDriverMaxNb) return TRUE;

	/* record GUID */
	if(pGUID)
	{
		pTemp = &p->mst_AudioDrivers.dst_GUIDs[p->mst_AudioDrivers.ui_MaxNb];
		memcpy(pTemp, pGUID, sizeof(GUID));
	}
	else
	{
		pTemp = NULL;
		memset(&p->mst_AudioDrivers.dst_GUIDs[p->mst_AudioDrivers.ui_MaxNb], 0, sizeof(GUID));
	}

	p->mst_AudioDrivers.dpst_pGUIDs[p->mst_AudioDrivers.ui_MaxNb] = pTemp;

	/* record its name */
	p->mst_AudioDrivers.dasz_Name[p->mst_AudioDrivers.ui_MaxNb] = (char *) L_malloc(strlen(strDesc) + 1);
	strcpy(p->mst_AudioDrivers.dasz_Name[p->mst_AudioDrivers.ui_MaxNb], strDesc);

	/* update nb */
	p->mst_AudioDrivers.ui_MaxNb++;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ESON_cl_VUmeter::OnSelCaptureDriver(unsigned int _ui_DriverIndex)
{
	/*~~~~~~~*/
	HRESULT hr;
	/*~~~~~~~*/

	mst_AudioDrivers.ui_SelIndex = -1;
	if(_ui_DriverIndex >= mst_AudioDrivers.ui_MaxNb) return -1;
	mst_AudioDrivers.ui_SelIndex = _ui_DriverIndex;

	/* Create IDirectSoundCapture using the preferred capture device */
	hr = DirectSoundCaptureCreate
		(
			mst_AudioDrivers.dpst_pGUIDs[mst_AudioDrivers.ui_SelIndex],
			&mpo_DirectSoundCaptureDevice,
			NULL
		);
	if(hr != DS_OK)
	{
		ERR_X_Error(0, "Error while creating sound capture object", NULL);
		return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_VUmeter::OnGetCaptureFormatList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HRESULT						hr;
	WAVEFORMATEX				wfx;
	HCURSOR						hCursor;
	DSCBUFFERDESC				dscbd;
	LPDIRECTSOUNDCAPTUREBUFFER	pDSCaptureBuffer;
	int							iIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDSCaptureBuffer = NULL;

	/* This might take a second or two, so throw up the hourglass */
	hCursor = GetCursor();
	SetCursor(LoadCursor(NULL, IDC_WAIT));

	memset(&wfx, 0, sizeof(wfx));
	wfx.wFormatTag = WAVE_FORMAT_PCM;

	memset(&dscbd, 0, sizeof(dscbd));
	dscbd.dwSize = sizeof(dscbd);

	/*
	 * Try ESON_Cte_CaptureFormatMaxNb different standard formats to see if they are
	 * supported
	 */
	for(iIndex = 0; iIndex < ESON_Cte_CaptureFormatMaxNb; iIndex++)
	{
		GetWaveFormatFromIndex(iIndex, &wfx);

		/*
		 * To test if a capture format is supported, try to create a new capture buffer
		 * using a specific format. If it works then the format is supported, otherwise
		 * not.
		 */
		dscbd.dwBufferBytes = wfx.nAvgBytesPerSec;
		dscbd.lpwfxFormat = &wfx;

		if(FAILED(hr = mpo_DirectSoundCaptureDevice->CreateCaptureBuffer(&dscbd, &pDSCaptureBuffer, NULL)))
            mab_CaptureFormatSupported[iIndex] = FALSE;            
        else
			mab_CaptureFormatSupported[iIndex] = TRUE;

		if(pDSCaptureBuffer) pDSCaptureBuffer->Release();
		pDSCaptureBuffer = NULL;
	}

	SetCursor(hCursor);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ESON_cl_VUmeter::OnSelCaptureFormat(unsigned int _ui_FormatIndex)
{
	if(_ui_FormatIndex >= ESON_Cte_CaptureFormatMaxNb) return -1;
	if(mab_CaptureFormatSupported[_ui_FormatIndex] == FALSE) return -1;

	memset(&mst_CaptureFormatDesc, 0, sizeof(mst_CaptureFormatDesc));
	mst_CaptureFormatDesc.wFormatTag = WAVE_FORMAT_PCM;
	GetWaveFormatFromIndex(_ui_FormatIndex, &mst_CaptureFormatDesc);

	if(this->OnCreateCaptureBuffer(&mst_CaptureFormatDesc)) return -1;

	mb_IsRecording = FALSE;

	if
	(
		(
			mh_ThreadHandle = CreateThread
				(
					NULL,
					0,
					(LPTHREAD_START_ROUTINE) ESON_CaptureNotificationThread,
					this,
					0,
					&mul_ThreadId
				)
		) == NULL
	)
	{
		ERR_X_Error(0, "can't create thread for capture sounds", NULL);
		return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int ESON_CaptureNotificationThread(void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ESON_cl_VUmeter *po_VUmeter;
	BOOL			bDone;
	DWORD			dwResult;
	HANDLE			dh_Event[2];
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_VUmeter = (ESON_cl_VUmeter *) p;
	bDone = FALSE;

	dh_Event[0] = po_VUmeter->mst_CaptureNotificationManager.h_NotificationEvent;
	dh_Event[1] = po_VUmeter->mh_KillThreadEvent;

	while(!bDone)
	{
		dwResult = WaitForMultipleObjects(2, dh_Event, FALSE, INFINITE);
		switch(dwResult)
		{
		case WAIT_OBJECT_0 + 0:
			po_VUmeter->OnCaptureNotication();
			break;

		case WAIT_OBJECT_0 + 1:
			SetEvent(po_VUmeter->mh_KillThreadOrderDone);
			SuspendThread(GetCurrentThread());
			break;

		default:
			bDone = TRUE;
			break;
		}
	}

	ExitThread(0);
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ESON_cl_VUmeter::OnCreateCaptureBuffer(WAVEFORMATEX *pwfxInput)
{
	/*~~~~~~~~~~~~~~~~~~*/
	HRESULT			hr;
	DSCBUFFERDESC	dscbd;
	/*~~~~~~~~~~~~~~~~~~*/

    if(pwfxInput->wFormatTag != WAVE_FORMAT_PCM) return -1;
    if(!pwfxInput->nAvgBytesPerSec) return -1;
    if(!pwfxInput->nChannels) return -1;
    if(!pwfxInput->wBitsPerSample) return -1;
    if(!pwfxInput->nBlockAlign) return -1;

	if(mst_CaptureNotificationManager.po_Interface) mst_CaptureNotificationManager.po_Interface->Release();
	mst_CaptureNotificationManager.po_Interface = NULL;

	if(mpo_DirectSoundCaptureBuffer) mpo_DirectSoundCaptureBuffer->Release();
	mpo_DirectSoundCaptureBuffer = NULL;

	/* Set the notification size */
	mst_CaptureNotificationManager.ui_NotifySize = ESON_Cte_CaturedDataBufferMinSize;	/* max(ESON_Cte_CaturedDataBufferMinSize,
																						 * pwfxInput->nAvgBytesPerSec
																						 * / 8); */
	mst_CaptureNotificationManager.ui_NotifySize -= mst_CaptureNotificationManager.ui_NotifySize % pwfxInput->nBlockAlign;

	/* Set the buffer sizes */
	mui_CaptureBufferSize = mst_CaptureNotificationManager.ui_NotifySize * ESON_Cte_CaptureNotificationNb;

	if(mst_CaptureNotificationManager.po_Interface) mst_CaptureNotificationManager.po_Interface->Release();
	mst_CaptureNotificationManager.po_Interface = NULL;

	if(mpo_DirectSoundCaptureBuffer) mpo_DirectSoundCaptureBuffer->Release();
	mpo_DirectSoundCaptureBuffer = NULL;

	/* Create the capture buffer */
	memset(&dscbd, 0, sizeof(dscbd));
	dscbd.dwSize = sizeof(dscbd);
	dscbd.dwBufferBytes = mui_CaptureBufferSize;
	dscbd.lpwfxFormat = pwfxInput;	/* Set the format during creatation */

	if(FAILED(hr = mpo_DirectSoundCaptureDevice->CreateCaptureBuffer(&dscbd, &mpo_DirectSoundCaptureBuffer, NULL)))
	{
		return -1;
	}

	mst_CaptureNotificationManager.ui_NextCaptureOffset = 0;

	if(OnCreateCaptureNotifications()) return -1;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ESON_cl_VUmeter::OnCreateCaptureNotifications(void)
{
	/*~~~~~~~*/
	HRESULT hr;
	/*~~~~~~~*/

	if(NULL == mpo_DirectSoundCaptureBuffer) return -1;

	/* Create a notification event, for when the sound stops playing */
	if
	(
		FAILED
		(
			hr = mpo_DirectSoundCaptureBuffer->QueryInterface
				(
					IID_IDirectSoundNotify,
					(VOID **) &mst_CaptureNotificationManager.po_Interface
				)
		)
	) return -1;

	/* Setup the notification positions */
	for(INT i = 0; i < ESON_Cte_CaptureNotificationNb; i++)
	{
		mst_CaptureNotificationManager.dst_PosNotify[i].dwOffset = (mst_CaptureNotificationManager.ui_NotifySize * i) +
			mst_CaptureNotificationManager.ui_NotifySize -
			1;
		mst_CaptureNotificationManager.dst_PosNotify[i].hEventNotify = mst_CaptureNotificationManager.h_NotificationEvent;
	}

	if
	(
		FAILED
		(
			hr = mst_CaptureNotificationManager.po_Interface->SetNotificationPositions
				(
					ESON_Cte_CaptureNotificationNb,
					mst_CaptureNotificationManager.dst_PosNotify
				)
		)
	) return hr;

	return S_OK;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ESON_cl_VUmeter::OnStartStopCapture(BOOL bStartRecording)
{
	/*~~~~~~~*/
	HRESULT hr;
	/*~~~~~~~*/

	if(bStartRecording)
	{
		if(this->OnCreateCaptureBuffer(&mst_CaptureFormatDesc))
		{
			ERR_X_Error(0, "[SND] Error while creating capture", NULL);
			return -1;
		}

		mb_IsRecording = TRUE;
		ResumeThread(mh_ThreadHandle);

		if(FAILED(hr = mpo_DirectSoundCaptureBuffer->Start(DSCBSTART_LOOPING)))
		{
			ERR_X_Error(0, "[SND] Error while starting capture", NULL);
			return -1;
		}
	}
	else
	{
		if(NULL == mpo_DirectSoundCaptureBuffer) return -1;

		/* suspend the thread */
		if(mb_IsRecording)
		{
			SetEvent(mh_KillThreadEvent);
			WaitForSingleObject(mh_KillThreadOrderDone, INFINITE);
		}

		mb_IsRecording = FALSE;

		/* Stop the buffer, and read any data that was not caught by a notification */
		if(FAILED(hr = mpo_DirectSoundCaptureBuffer->Stop()))
		{
			ERR_X_Error(0, "[SND] Error while stoping capture", NULL);
			return -1;
		}
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_VUmeter::OnResizeTransfertBuffer(int _i_NewSize)
{
	/*~~*/
	int i;
	/*~~*/

	if(_i_NewSize <= (int) mui_CapturedDataBufferSize) return;

	for(i = 0; i < ESON_Cte_CaptureNotificationNb; i++)
	{
		mapv_CapturedDataBuffer[i] = (char *) L_realloc(mapv_CapturedDataBuffer[i], _i_NewSize);
	}

	mui_CapturedDataBufferSize = _i_NewSize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ESON_cl_VUmeter::OnShowWindow(void)
{
    if(!mpo_SndVumeterDialog)
    {
        OnCreateWindow();
		OnStartStopCapture(TRUE);
    }

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ESON_cl_VUmeter::OnHideWindow(void)
{
    if(mpo_SndVumeterDialog) delete mpo_SndVumeterDialog;
    mpo_SndVumeterDialog = NULL;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ESON_cl_VUmeter::OnCaptureNotication(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	HRESULT hr;
	VOID	*pbCaptureData;
	DWORD	dwCaptureLength;
	VOID	*pbCaptureData2;
	DWORD	dwCaptureLength2;
	VOID	*pbPlayData;
	DWORD	dwReadPos;
	DWORD	dwCapturePos;
	LONG	lLockSize;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pbCaptureData = NULL;
	pbCaptureData2 = NULL;
	pbPlayData = NULL;

	if(!mpo_DirectSoundCaptureBuffer) return 0;

	if(FAILED(hr = mpo_DirectSoundCaptureBuffer->GetCurrentPosition(&dwCapturePos, &dwReadPos)))
	{
		return -1;
	}

	lLockSize = dwReadPos - mst_CaptureNotificationManager.ui_NextCaptureOffset;
	if(lLockSize < 0) lLockSize += mui_CaptureBufferSize;

	/* Block align lock size so that we are always write on a boundary */
	lLockSize -= (lLockSize % mst_CaptureNotificationManager.ui_NotifySize);

	if(lLockSize == 0)
	{
		return -1;
	}

	/* Lock the capture buffer down */
	if
	(
		FAILED
		(
			hr = mpo_DirectSoundCaptureBuffer->Lock
				(
					mst_CaptureNotificationManager.ui_NextCaptureOffset,
					lLockSize,
					&pbCaptureData,
					&dwCaptureLength,
					&pbCaptureData2,
					&dwCaptureLength2,
					0L
				)
		)
	)
	{
		return -1;
	}

	/* Write the data into the wav file */
	SendDataToWindow(dwCaptureLength, (char *) pbCaptureData);

	/* Move the capture offset along */
	mst_CaptureNotificationManager.ui_NextCaptureOffset += dwCaptureLength;
	mst_CaptureNotificationManager.ui_NextCaptureOffset %= mui_CaptureBufferSize;	/* Circular buffer */

	if(pbCaptureData2 != NULL)
	{
		/* Write the data into the wav file */
		SendDataToWindow(dwCaptureLength2, (char *) pbCaptureData2);

		/* Move the capture offset along */
		mst_CaptureNotificationManager.ui_NextCaptureOffset += dwCaptureLength2;
		mst_CaptureNotificationManager.ui_NextCaptureOffset %= mui_CaptureBufferSize;	/* Circular buffer */
	}

	/* Unlock the capture buffer */
	mpo_DirectSoundCaptureBuffer->Unlock(pbCaptureData, dwCaptureLength, pbCaptureData2, dwCaptureLength2);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_VUmeter::GetWaveFormatFromIndex(int nIndex, WAVEFORMATEX *pwfx)
{
	/*~~~~~~~~~~~~*/
	INT iSampleRate;
	INT iType;
	/*~~~~~~~~~~~~*/

	iSampleRate = nIndex / 4;
	iType = nIndex % 4;

	switch(iSampleRate)
	{
	case 0:
		pwfx->nSamplesPerSec = 8000;
		break;
	case 1:
		pwfx->nSamplesPerSec = 11025;
		break;
	case 2:
		pwfx->nSamplesPerSec = 22050;
		break;
	case 3:
		pwfx->nSamplesPerSec = 44100;
		break;
	}

	switch(iType)
	{
	case 0:
		pwfx->wBitsPerSample = 8;
		pwfx->nChannels = 1;
		break;
	case 1:
		pwfx->wBitsPerSample = 16;
		pwfx->nChannels = 1;
		break;
	case 2:
		pwfx->wBitsPerSample = 8;
		pwfx->nChannels = 2;
		break;
	case 3:
		pwfx->wBitsPerSample = 16;
		pwfx->nChannels = 2;
		break;
	}

	pwfx->nBlockAlign = pwfx->nChannels * (pwfx->wBitsPerSample / 8);
	pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_VUmeter::SendDataToWindow(unsigned int nSizeToWrite, char *pbSrcData)
{
	if(nSizeToWrite > (unsigned int) mui_CapturedDataBufferSize) OnResizeTransfertBuffer(nSizeToWrite);

	if(++mui_CapturedDataBufferIndex == ESON_Cte_CaptureNotificationNb) mui_CapturedDataBufferIndex = 0;

	L_memset(mapv_CapturedDataBuffer[mui_CapturedDataBufferIndex], 0, nSizeToWrite);
	L_memcpy(mapv_CapturedDataBuffer[mui_CapturedDataBufferIndex], pbSrcData, nSizeToWrite);

	TreatNewData(pbSrcData, nSizeToWrite);
    
    if(mpo_SndVumeterDialog)
    {
        mpo_SndVumeterDialog->mb_New = TRUE;
        mpo_SndVumeterDialog->Invalidate(TRUE);
    }    
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */




/*
 =======================================================================================================================
 =======================================================================================================================
 */
void	ESON_cl_VUmeter::ValueResetAll(void)
{
	ValueReset(&mst_VumeterValues.st_Left);
	ValueReset(&mst_VumeterValues.st_Right);
}

void ESON_cl_VUmeter::OnCreateWindow(void)
{

    if(mpo_SndVumeterDialog) return;
 
	ValueReset(&mst_VumeterValues.st_Left);
	ValueReset(&mst_VumeterValues.st_Right);

    mpo_SndVumeterDialog = new EDIA_cl_SndVumeterDialog(this);
    mpo_SndVumeterDialog->DoModeless();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_VUmeter::ValueReset(ESON_tdst_VUmeter *lpVU)
{
	lpVU->i_MaxLevel = mi_MaxScale;
	lpVU->i_Scale = mi_Scale;
	lpVU->i_CurrentLevel = 0;
	lpVU->i_PeakLevel = 0;
	lpVU->i_PeakAge = 0;
	lpVU->i_PeakTTL = mi_PeakTTL;
	lpVU->i_Falloff = mi_Falloff;
}

/*
 =======================================================================================================================
    sets a new i_CurrentLevel for the vu meter
 =======================================================================================================================
 */
void ESON_cl_VUmeter::ValueUpdate(ESON_tdst_VUmeter *lpVU, int newlevel)
{
	if(newlevel > lpVU->i_PeakLevel)
	{
		lpVU->i_PeakLevel = newlevel > lpVU->i_MaxLevel ? lpVU->i_MaxLevel : newlevel;
		lpVU->i_PeakAge = 0;
	}

	lpVU->i_PeakAge++;
	if(lpVU->i_PeakAge >= lpVU->i_PeakTTL)
	{
		lpVU->i_PeakLevel = lpVU->i_CurrentLevel;
		lpVU->i_PeakAge = 0;
	}

	if(newlevel > lpVU->i_CurrentLevel)
	{
		lpVU->i_CurrentLevel = newlevel > lpVU->i_MaxLevel ? lpVU->i_MaxLevel : newlevel;
		return;
	}

    if((lpVU->i_CurrentLevel - newlevel) > lpVU->i_Falloff)
        lpVU->i_CurrentLevel -= lpVU->i_Falloff;
    else
        lpVU->i_CurrentLevel = newlevel;

	if(lpVU->i_CurrentLevel < SND_Cte_MinAtt) lpVU->i_CurrentLevel = SND_Cte_MinAtt;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_VUmeter::TreatNewData(char *lpData, int dwBytesRecorded)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DWORD			i;
	short			*ps;
	unsigned int	sample;

	double			avg_amp_left, avg_amp_right;
	static double	sum_Left = 0.0;
	static double	sum_Right = 0.0;
	static double	sum_Size = 0.0;
	double			noise_left, noise_right;
	static int		call = 0;
	static double	factor = 1.0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!lpData) return;
	if(!dwBytesRecorded) return;

	sample = dwBytesRecorded / (2 * sizeof(short));
	avg_amp_left = 0.0;
	avg_amp_right = 0.0;
	ps = (short *) lpData;

	if(call++ >= 100)
	{
		call = 0;
		sum_Left = 0.0;
		sum_Right = 0.0;
		sum_Size = 0.0;
	}

	for(i = 0; i < sample; i++)
	{
		sum_Size += 1.0;

		if(*ps < 0)
		{
			sum_Left += (double) (- *(ps));
			avg_amp_left += (double) (- *(ps++));
		}
		else
		{
			sum_Left += (double) (*(ps));
			avg_amp_left += (double) (*(ps++));
		}

		if(*ps < 0)
		{
			sum_Right += (double) (- *(ps));
			avg_amp_right += (double) (- *(ps++));
		}
		else
		{
			sum_Right += (double) (*(ps));
			avg_amp_right += (double) (*(ps++));
		}
	}

	avg_amp_left = PiBy2 * (avg_amp_left / (double) sample);
	avg_amp_right = PiBy2 * (avg_amp_right / (double) sample);

	noise_left = (sum_Left / sum_Size);
	noise_right = (sum_Right / sum_Size);

	avg_amp_left -= factor * noise_left;
	avg_amp_right -= factor * noise_right;

	if(avg_amp_left > 0.0)
		avg_amp_left = 1000.0 * log10(avg_amp_left / 32767.0);
	else
		avg_amp_left = -10000.0;

	if(avg_amp_right > 0.0)
		avg_amp_right = 1000.0 * log10(avg_amp_right / 32767.0);
	else
		avg_amp_right = -10000.0;

	/**/
	avg_amp_right = avg_amp_right + ((0.9873417721518987342) * avg_amp_right + 848.987);
	avg_amp_left = avg_amp_left + ((0.9873417721518987342) * avg_amp_left + 848.987);

    /**/
	ValueUpdate(&mst_VumeterValues.st_Left, (int) avg_amp_left);
	ValueUpdate(&mst_VumeterValues.st_Right, (int) avg_amp_right);
}


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */

