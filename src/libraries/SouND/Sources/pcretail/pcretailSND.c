#include "Precomp.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/win32/win32SND.h"
#include "SouND/Sources/win32/win32SNDdebug.h"
#include "Sdk/Sources/BASe/MEMory/MEM.h"
#include "SouND/Sources/SNDload.h"
#include "SDK/Sources/base/bastypes.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDconst.h"
#include "LINKs/LINKmsg.h"
#include "SouND/Sources/win32/win32SND_Stream.h"

#define C_SoundSettings_Entry "Software\\Ubi Soft\\BGE PC\\SettingsApplication.INI\\Sound"

#define C_ReadRegistry_Ok			0
#define C_ReadRegistry_Error		1

extern HWND					MAI_gh_MainWindow;
extern HANDLE				win32SND_gh_ThreadHandle;
extern BAS_tdst_barray		win32SND_gst_PlayingSB;

static int	pcretailSND_l_InitPrimaryBuffer(SND_tdst_TargetSpecificData *);
static int ReadRegistryFXEnable(int *iFXEnable);
static int ReadRegistrySoundCardGUID(GUID* pguid);

void pcretailSND_SB_Duplicate
(
	SND_tdst_TargetSpecificData *_pst_SpeData,
	SND_tdst_SoundBuffer		*_pst_SrcSB,
	SND_tdst_SoundBuffer		**_ppst_DstSB
)
{
	//DSBCAPS dsbcaps;
	//WAVEFORMATEX wavefmt;
	//DWORD size1,size2,writtenbytes,status;
	//void *ptr1,*ptr2;
	//char* pTemporaryBuffer;
	//LPDIRECTSOUNDBUFFER pSndBuffer;
	//HRESULT hr;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_WaveData		st_WaveFile;
	SND_tdst_SoundBuffer	*pst_SoundBuffer;//,*pNextSB;
	LONG					l_Index;
	SND_tdst_OneSound		*pst_Sound;
	//DWORD					status;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_ppst_DstSB = NULL;
	//HACK
	return;
	//HACK
	//Does not duplicate the stream buffers
	if(_pst_SrcSB->pst_SS) 
		return;
	//#######SOUND BUFFER LIST#######
	//Check if a SoundBuffer already allocated for duplication
//	pNextSB = (SND_tdst_SoundBuffer *) _pst_SrcSB->pNextDuplicatedBuffer;
//	if( pNextSB != NULL )
//		IDirectSoundBuffer8_GetStatus(pNextSB->pst_DSB,&status);
//	while( !((pNextSB == NULL) || (status & DSBSTATUS_PLAYING)) )
//	{
//		pNextSB = (SND_tdst_SoundBuffer *) pNextSB->pNextDuplicatedBuffer;
//	if( pNextSB != NULL )
//		IDirectSoundBuffer8_GetStatus(pNextSB->pst_DSB,&status);
//
//	}
	//#######SOUND BUFFER LIST#######
	*_ppst_DstSB = (SND_tdst_SoundBuffer *) MEM_p_Alloc(sizeof(SND_tdst_SoundBuffer));
	L_memset(*_ppst_DstSB,0,sizeof(SND_tdst_SoundBuffer));
	(*_ppst_DstSB)->i_Channel = _pst_SrcSB->i_Channel;
	(*_ppst_DstSB)->i_PanIdx = 64;
	/*$2- activation -------------------------------------------------------------------------------------------------*/
	//ORIGif(!SND_gst_Params.l_Available) return -1;
	//ORIG_SND_DebugSpy(_ul_WavKey, SND_l_LoadData);
	/*$2- Test extension and existence -------------------------------------------------------------------------------*/
	l_Index = SND_l_GetSoundIndex(_pst_SpeData->pOneSound->ul_FileKey);
	if(l_Index == -1) return;
    //ESON_M_ReturnIfFileIsTruncated(_pst_SpeData->pOneSound->ul_FileKey, -1);

	pst_Sound = SND_gst_Params.dst_Sound + l_Index;
//	if(pst_Sound->ul_Flags & SND_Cul_OSF_Loaded) return l_Index;

	/*$2- create the sound buffer ------------------------------------------------------------------------------------*/

	st_WaveFile.st_WaveFmtx.wFormatTag = pst_Sound->us_FormatTag;
	st_WaveFile.st_WaveFmtx.nChannels = pst_Sound->w_Channel;
	st_WaveFile.st_WaveFmtx.nSamplesPerSec = pst_Sound->i_BaseFrequency;
	st_WaveFile.st_WaveFmtx.nAvgBytesPerSec = pst_Sound->ui_AvgBytesPerSec;
	st_WaveFile.st_WaveFmtx.nBlockAlign = pst_Sound->us_BlockAlign;
	st_WaveFile.st_WaveFmtx.wBitsPerSample = pst_Sound->w_BPS;
	st_WaveFile.st_WaveFmtx.cbSize = pst_Sound->us_cbSize;
	st_WaveFile.i_Size = pst_Sound->ui_DataSize;
	st_WaveFile.pbData = (void *) pst_Sound->ui_DataPos;

	//SND_M_RasterRegisterFile(_pst_SpeData->pOneSound->ul_FileKey);
#ifdef _PC_RETAIL
	//Check if Sound needs Fx
//	if(pst_Sound->ul_Flags & SND_Cul_SF_Fx)
//		SND_gst_Params.pst_SpecificD->hasFx = true;
//	else
//		SND_gst_Params.pst_SpecificD->hasFx = false;
#endif //_PC_RETAIL
	pst_SoundBuffer = win32SND_pst_SB_Create
	//pst_SoundBuffer = SND_gpst_Interface[ISound].pfp_SndBuffCreate
		(
			SND_gst_Params.pst_SpecificD,
			&st_WaveFile,
			pst_Sound->ui_DataPos
		);
	if(!pst_SoundBuffer) return;

	/*$2- register it ------------------------------------------------------------------------------------------------*/

//	pst_Sound->ul_Flags = SND_Cul_DSF_Used | SND_Cul_OSF_Loaded | SND_Cul_SF_HeaderLoaded;
//	pst_Sound->pst_DSB = pst_SoundBuffer;

	//ORIGreturn(l_Index);

	*_ppst_DstSB = pst_SoundBuffer;

	//#######SOUND BUFFER LIST#######
	//A SoundBuffer has been allocated, remember it
	//_pst_SrcSB->pNextDuplicatedBuffer = pst_SoundBuffer;
	//#######SOUND BUFFER LIST#######
}

int pcretailSND_l_Init(SND_tdst_TargetSpecificData *_pst_SpecificD)
{
	/*~~~~~~~*/
	HRESULT hr;
	GUID audioboardguid;
	int iFXEnable;
	/*~~~~~~~*/
	//Global var reset
	BAS_binit(&win32SND_gst_PlayingSB, 48);
	L_memset(&win32SND_gst_SoundManager, 0, sizeof(win32SND_tdst_Manager));

	win32SND_gh_ThreadHandle = NULL;
	win32SND_gst_SoundManager.l_GlobalVol = 0;

	_pst_SpecificD->h_Wnd = MAI_gh_MainWindow;

	//DirectSound initializazion
	hr = CoInitialize(NULL);
	ERR_X_Warning((hr == S_OK), "bad return value (win32SND_l_Init)", NULL);
	if(hr != S_OK)
		return 0;
	if( ReadRegistryFXEnable(&iFXEnable) != C_ReadRegistry_Ok )
		return 0;
	//Retrieve Audio board GUID
	if( ReadRegistrySoundCardGUID(&audioboardguid) != C_ReadRegistry_Ok )
        return 0;
	if(DirectSoundCreate8(&audioboardguid, &_pst_SpecificD->pst_DS, NULL) == DS_OK)
	{
		win32SND_gst_SoundManager.st_DriverCapacity.dwSize = sizeof(DSCAPS);
		hr = IDirectSound8_GetCaps(_pst_SpecificD->pst_DS, &win32SND_gst_SoundManager.st_DriverCapacity);
		ERR_X_Warning((hr == S_OK), "bad return value (win32SND_l_Init)", NULL);

		hr = IDirectSound8_SetCooperativeLevel(_pst_SpecificD->pst_DS, _pst_SpecificD->h_Wnd, DSSCL_PRIORITY);
		ERR_X_Warning((hr == S_OK), "bad return value (win32SND_l_Init)", NULL);

		if(!pcretailSND_l_InitPrimaryBuffer(_pst_SpecificD)) return 0;
		win32SND_StreamInitModule();

		return 1;
	}

	return 0;
}

static int pcretailSND_l_InitPrimaryBuffer(SND_tdst_TargetSpecificData *_pst_SpecificD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DSBUFFERDESC	dsbd;
	WAVEFORMATEX	st_Format;
	HRESULT			hr;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- set capacities ---------------------------------------------------------------------------------------------*/

	L_memset(&dsbd, 0, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;		/* only for primary buffer settings */
	dsbd.lpwfxFormat = NULL;	/* only for primary buffer settings */
	dsbd.guid3DAlgorithm = GUID_NULL;

	hr = IDirectSound8_CreateSoundBuffer
		(
			_pst_SpecificD->pst_DS,
			&dsbd,
			(IDirectSoundBuffer **) &_pst_SpecificD->pst_PrimaryDSB,
			NULL
		);
	win32SND_M_Assert(hr == DS_OK);
	if(FAILED(hr)) return 0;

	/*$2- set format -------------------------------------------------------------------------------------------------*/

	L_memset(&st_Format, 0, sizeof(WAVEFORMATEX));

	st_Format.nSamplesPerSec = 48000;
	st_Format.wBitsPerSample = 16;
	st_Format.nChannels = 2;
	st_Format.nBlockAlign = st_Format.nChannels * sizeof(short);
	st_Format.nAvgBytesPerSec = st_Format.nBlockAlign * st_Format.nSamplesPerSec;
	st_Format.cbSize = 0;
	st_Format.wFormatTag = WAVE_FORMAT_PCM;

	hr = IDirectSoundBuffer8_SetFormat(_pst_SpecificD->pst_PrimaryDSB, &st_Format);
	win32SND_M_Assert(hr == DS_OK);
	if(hr != DS_OK) return 0;

	/*$2- start playing ----------------------------------------------------------------------------------------------*/

	hr = IDirectSoundBuffer8_Play(_pst_SpecificD->pst_PrimaryDSB, 0, 0, DSBPLAY_LOOPING);
	if(hr != DS_OK) return 0;

	return 1;
}

int ReadRegistryFXEnable(int *iFXEnable)
{
	LONG result;
	HKEY hSoundKey;
	DWORD typebuffer,size;

	result = RegOpenKeyEx(HKEY_CURRENT_USER,C_SoundSettings_Entry,0,KEY_EXECUTE,&hSoundKey);
	if( result == ERROR_SUCCESS )
	{
		size = sizeof(DWORD);
		result = RegQueryValueEx(hSoundKey,"Sound FX",NULL,&typebuffer,(LPBYTE) iFXEnable,&size);
		if( result == ERROR_SUCCESS )
		{
			if( (size==sizeof(int)) && (typebuffer==REG_DWORD) )
			{
				RegCloseKey(hSoundKey);
				return C_ReadRegistry_Ok;
			}
			else
				return C_ReadRegistry_Error;
		}
		else
			return C_ReadRegistry_Error;
	}
	else
		return C_ReadRegistry_Error;
}

int ReadRegistrySoundCardGUID(GUID* pguid)
{
	LONG result;
	HKEY hSoundKey;
	DWORD typebuffer,size;

	result = RegOpenKeyEx(HKEY_CURRENT_USER,C_SoundSettings_Entry,0,KEY_QUERY_VALUE,&hSoundKey);
	if( result == ERROR_SUCCESS )
	{
		size = sizeof(GUID);
		result = RegQueryValueEx(hSoundKey,"Audio board GUID",NULL,&typebuffer,(LPBYTE) pguid,&size);
		if( result == ERROR_SUCCESS )
		{
			if( (size==sizeof(GUID)) && (typebuffer==REG_BINARY) )
			{
				RegCloseKey(hSoundKey);
				return C_ReadRegistry_Ok;
			}
			else
				return C_ReadRegistry_Error;
		}
		else
		{
			DWORD dw = GetLastError();
			char pc[1000];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,result,0,pc,1000,NULL);
			return C_ReadRegistry_Error;
		}
	}
	else
		return C_ReadRegistry_Error;
}

























/*
	L_memset(&dsbcaps,0,sizeof(DSBCAPS));
	L_memset(&wavefmt,0,sizeof(WAVEFORMATEX));
	L_memset(&dsbdesc,0,sizeof(DSBUFFERDESC));
	dsbcaps.dwSize = sizeof(DSBCAPS);
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	//Get info from the SoundBuffer
	_pst_SrcSB->pst_DSB->lpVtbl->GetCaps(_pst_SrcSB->pst_DSB,&dsbcaps);
	_pst_SrcSB->pst_DSB->lpVtbl->GetFormat(_pst_SrcSB->pst_DSB,&wavefmt,sizeof(WAVEFORMATEX),&writtenbytes);
	//If the buffer has not FX, process as for win32
	//if( !_pst_SrcSB->hasFx )
	//if(! (dsbcaps.dwFlags & DSBCAPS_CTRLFX) )
	//{
	//	win32SND_SB_Duplicate(_pst_SpeData,_pst_SrcSB,_ppst_DstSB);
	//	return;
	//}
	*_ppst_DstSB = (SND_tdst_SoundBuffer *) MEM_p_Alloc(sizeof(SND_tdst_SoundBuffer));
	L_memset(*_ppst_DstSB,0,sizeof(SND_tdst_SoundBuffer));
	(*_ppst_DstSB)->i_Channel = _pst_SrcSB->i_Channel;
	(*_ppst_DstSB)->i_PanIdx = 64;
	//Stop the SoundBuffer and Lock it
	hr = _pst_SrcSB->pst_DSB->lpVtbl->GetStatus(_pst_SrcSB->pst_DSB,&status);
	if( status & DSBSTATUS_PLAYING )
		_pst_SrcSB->pst_DSB->lpVtbl->Stop(_pst_SrcSB->pst_DSB);
	hr = _pst_SrcSB->pst_DSB->lpVtbl->Lock(_pst_SrcSB->pst_DSB,0,dsbcaps.dwBufferBytes,&ptr1,&size1,&ptr2,&size2,DSBLOCK_ENTIREBUFFER);
	pTemporaryBuffer = MEM_p_Alloc(size1);
	//pTemporaryBuffer = L_malloc(size1);
	//Read the Source buffer
	L_memcpy(pTemporaryBuffer,ptr1,size1);
	//Unlock the buffer
	hr = _pst_SrcSB->pst_DSB->lpVtbl->Unlock(_pst_SrcSB->pst_DSB,ptr1,size1,ptr2,size2);
	//Create the copy Buffer
	dsbdesc.dwFlags = dsbcaps.dwFlags;
	dsbdesc.dwBufferBytes = dsbcaps.dwBufferBytes;
	dsbdesc.lpwfxFormat = &wavefmt;
	dsbdesc.guid3DAlgorithm = GUID_NULL;
	hr = _pst_SpeData->pst_DS->lpVtbl->CreateSoundBuffer(_pst_SpeData->pst_DS,&dsbdesc,&pSndBuffer,NULL);
	//Assign the SoundBuffer	
	(*_ppst_DstSB)->pst_DSB = pSndBuffer;
	win32SND_M_Assert(hr == DS_OK);
	(*_ppst_DstSB)->pst_SS = _pst_SrcSB->pst_SS;

	//L_free(pTemporaryBuffer);
	MEM_Free(pTemporaryBuffer);

*/