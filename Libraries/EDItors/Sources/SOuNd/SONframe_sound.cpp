/*$T SONframe_sound.cpp GC 1.138 11/05/04 10:09:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS

/*$2- editor ---------------------------------------------------------------------------------------------------------*/

#include "Res/Res.h"
#include "LINKs/LINKmsg.h"
#include "EDIpaths.h"
#include "EDImainframe.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAmessage_dlg.h"
#include "DIAlogs/DIAsndtrans_dlg.h"
#include "BROwser/BROframe.h"
#include "EDImsg.h"

/*$2- engine ---------------------------------------------------------------------------------------------------------*/

#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGread.h"
#include "WORld/WORvars.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_GEN.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "SouND/sources/SND.h"
#include "SouND/sources/SNDwave.h"
#include "SouND/sources/SNDconst.h"
#include "SouND/sources/SNDmodifier.h"
#include "SouND/sources/SNDmusic.h"
#include "SouND/sources/SNDambience.h"
#include "SouND/sources/SNDdialog.h"
#include "SouND/sources/SNDloadingsound.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/sources/SNDvolume.h"
#include "SouND/sources/SNDconv_xboxadpcm.h"
#include "SouND/Sources/SNDbank.h"
#include "SouND/sources/SNDrasters.h"
#include "SouND/sources/SNDfx.h"

#include "SONframe.h"
#include "SONview.h"
#include "SONutil.h"
#include "SONpane.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

#ifdef JADEFUSION
extern LONG							SND_l_GetLoadingSound(void);
extern struct TEXT_tdst_AllText_	TEXT_gst_Global;
extern int							ediSND_i_FxGetMode(int core);
extern int							ediSND_i_FxGetDelay(int core);
extern int							ediSND_i_FxGetFeedback(int core);
extern float						ediSND_f_FxGetWetVol(int core);
#else //JADEFUSION
extern "C" struct TEXT_tdst_AllText_	TEXT_gst_Global;
extern "C" int							ediSND_i_FxGetMode(int core);
extern "C" int							ediSND_i_FxGetDelay(int core);
extern "C" int							ediSND_i_FxGetFeedback(int core);
extern "C" float						ediSND_f_FxGetWetVol(int core);
#endif

char									ESON_asz_Temp[32];
int										gaiFxDelay[2], gaiFxFeedback[2];
float									gafWetVol[2];
unsigned int							gauiFxFlags[2];

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */
#ifdef JADEFUSION
extern void BIG_ReadNoSeek(ULONG _ul_Pos, void *_p_Buffer, ULONG _ul_Length);
#else
extern "C" void BIG_ReadNoSeek(ULONG, void *, ULONG);
#endif
WAVEFORMATEX	stWAVEFORMATEX;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Sound_Set(BIG_INDEX _ul_Fat, BOOL _b_Update)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					*psz_Name, *pz_Temp;
	char					az_Path[BIG_C_MaxLenPath];
	HRESULT					hr;
	DSBUFFERDESC			stDSBUFFERDESC;
	char					*pc_Buffer;
	char					*pc_Data1;
	char					*pc_Data2;
	ULONG					ui_Data1;
	ULONG					ui_Data2;
	EDIA_cl_MessageDialog	*po_Dialog;
	ULONG					ul_DataSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1- is sound engine enabled ? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
    if(_ul_Fat == -1) return;

	/*$1- is .smd file ? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	psz_Name = BIG_NameFile(_ul_Fat);
	pz_Temp = L_strrchr(psz_Name, '.');
	if(!L_strnicmp(pz_Temp, EDI_Csz_ExtSModifier, 4))
	{
		SModifier_Set(_ul_Fat, _b_Update);
		return;
	}

	/*$1- is file valid ? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ESON_b_IsFileTruncated(BIG_FileKey(_ul_Fat)))
	{
		ERR_X_Warning(0, "[SND] the sound file is truncated", NULL);
		Sound_Close();
		return;
	}

	if(_ul_Fat == BIG_C_InvalidIndex)
	{
		ERR_X_Warning(0, "[SND] the sound file has an invalid index", NULL);
		Sound_Close();
		return;
	}

	if(BIG_FileKey(_ul_Fat) == BIG_C_InvalidKey)
	{
		ERR_X_Warning(0, "[SND] the sound file has an invalid key", NULL);
		Sound_Close();
		return;
	}

	if(BIG_PosFile(_ul_Fat) == -1)
	{
		ERR_X_Warning(0, "[SND] the sound file has an invalid position", NULL);
		Sound_Close();
		return;
	}

	/*$1- is the file yet loaded ? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((mst_SDesc.ul_Key == BIG_FileKey(_ul_Fat)) && (BIG_FileChanged(_ul_Fat) == 0)) return;
	M_MF()->AddHistoryFile(this, BIG_FileKey(_ul_Fat));

	/*$1- close the previous ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_SDesc.ul_Key != BIG_C_InvalidKey)
	{
		Sound_Close();
	}

	/*$1- parse the file header ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Dialog = new EDIA_cl_MessageDialog("Sound Editor", "Parsing the file...", MB_ICONEXCLAMATION);
	po_Dialog->DoModeless();

	LOA_MakeFileRef
	(
		BIG_FileKey(_ul_Fat),
		(ULONG *) &mst_SDesc.pWave,
		SND_ul_WaveCallback,
		LOA_C_MustExists | LOA_C_HasUserCounter
	);
	LOA_Resolve();
	ESON_LoadAllSounds();

	if(mst_SDesc.pWave == NULL)
	{
		ERR_X_Warning(0, "[SND] the sound file has an invalid header", NULL);
		Sound_Close();
		return;
	}

	if(mst_SDesc.pWave->wFormatTag != SND_gst_Params.i_EdiWaveFormat)
	{
		ERR_X_Warning(0, "[SND] the sound file has an invalid format", NULL);
		Sound_Close();
		return;
	}

	/*$1- create a direct sound buffer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	stWAVEFORMATEX.cbSize = mst_SDesc.pWave->cbSize;
	stWAVEFORMATEX.nChannels = mst_SDesc.pWave->wChannels;
	stWAVEFORMATEX.nSamplesPerSec = mst_SDesc.pWave->dwSamplesPerSec;
	stWAVEFORMATEX.wFormatTag = WAVE_FORMAT_PCM;
	stWAVEFORMATEX.wBitsPerSample = 16;
	stWAVEFORMATEX.nAvgBytesPerSec = stWAVEFORMATEX.nChannels * sizeof(short) * stWAVEFORMATEX.nSamplesPerSec;
	stWAVEFORMATEX.nBlockAlign = stWAVEFORMATEX.nChannels * sizeof(short);

	L_memset(&stDSBUFFERDESC, 0, sizeof(DSBUFFERDESC));
	stDSBUFFERDESC.dwSize = sizeof(DSBUFFERDESC);
	stDSBUFFERDESC.dwFlags = (DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY);
	ul_DataSize = stDSBUFFERDESC.dwBufferBytes = SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM ? SND_ui_GetDecompressedSize(mst_SDesc.pWave->ul_DataSize) : mst_SDesc.pWave->ul_DataSize;
	stDSBUFFERDESC.lpwfxFormat = &stWAVEFORMATEX;
	stDSBUFFERDESC.guid3DAlgorithm = GUID_NULL;

	hr = IDirectSound8_CreateSoundBuffer
		(
			SND_gst_Params.pst_SpecificD->pst_DS,
			&stDSBUFFERDESC,
			(IDirectSoundBuffer **) &mst_SDesc.po_SoundBuffer,
			NULL
		);
	if(hr != DS_OK)
	{
		ERR_X_Warning(0, "[SND] can not create a DirectSoundBuffer for this sound file", NULL);
		Sound_Close();
		return;
	}

	/*$1- load data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	delete po_Dialog;
	po_Dialog = new EDIA_cl_MessageDialog("Sound Editor", "Loading the file...", MB_ICONEXCLAMATION);
	po_Dialog->DoModeless();

	pc_Buffer = (char *) L_malloc(ul_DataSize);
	if(!pc_Buffer)
	{
		ERR_X_Warning(0, "[SND] no more memory for loading the sound file", NULL);
		Sound_Close();
		return;
	}

	if(SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM)
	{
		/*~~~~~~~~~~~*/
		char	*pTemp;
		BOOL	ok;
		/*~~~~~~~~~~~*/

		pTemp = (char *) L_malloc(mst_SDesc.pWave->ul_DataSize);
		BIG_ReadNoSeek(mst_SDesc.pWave->ul_DataPosition, pTemp, mst_SDesc.pWave->ul_DataSize);
		ok = SND_b_Decode
			(
				pTemp,
				pc_Buffer,
				mst_SDesc.pWave->ul_DataSize / mst_SDesc.pWave->wBlockAlign,
				mst_SDesc.pWave->wChannels
			);
		L_free(pTemp);

		if(!ok)
		{
			ERR_X_Warning(0, "[SND] no more memory for loading the sound file (2)", NULL);
			Sound_Close();
			return;
		}
	}
	else
	{
		BIG_ReadNoSeek(mst_SDesc.pWave->ul_DataPosition, pc_Buffer, mst_SDesc.pWave->ul_DataSize);
	}

	hr = IDirectSoundBuffer8_Lock
		(
			mst_SDesc.po_SoundBuffer,
			0,
			ul_DataSize,
			(void **) &pc_Data1,
			&ui_Data1,
			(void **) &pc_Data2,
			&ui_Data2,
			DSBLOCK_ENTIREBUFFER
		);

	if(hr == DS_OK)
	{
		L_memcpy(pc_Data1, pc_Buffer, ui_Data1);
		if(ui_Data2 && pc_Data2) L_memcpy(pc_Data2, pc_Buffer + ui_Data1, ui_Data2);
		IDirectSoundBuffer8_Unlock(mst_SDesc.po_SoundBuffer, pc_Data1, ui_Data1, pc_Data2, ui_Data2);
		L_free(pc_Buffer);
	}
	else
	{
		L_free(pc_Buffer);
		ERR_X_Warning(0, "[SND] can not write to the DirectSoundBuffer", NULL);
		Sound_Close();
		return;
	}

	/*$1- update the sound descriptor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mst_SDesc.ul_Fat = _ul_Fat;
	mst_SDesc.ul_Key = BIG_FileKey(_ul_Fat);

	mst_SDesc.ul_NumberOfSamples = (ULONG) SND_ui_SizeToSample(mst_SDesc.pWave->wFormatTag, mst_SDesc.pWave->wChannels, mst_SDesc.pWave->ul_DataSize) ;
    mst_SDesc.f_Time = (float)mst_SDesc.ul_NumberOfSamples / (float)mst_SDesc.pWave->dwSamplesPerSec;

	BIG_ComputeFullName(BIG_ParentFile(mst_SDesc.ul_Fat), mst_SDesc.sz_Path);

	/*$1- update the display ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	UpdateVarView_Sound();
	sprintf(az_Path, "%s", BIG_NameFile(mst_SDesc.ul_Fat));

	delete po_Dialog;
	mpo_PaneSound->Invalidate();

	if(mst_Ini.i_Option & ESON_Cte_AutoPlay) Sound_Play();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Sound_Stop(void)
{
	mst_SDesc.b_Paused = FALSE;
	mst_SDesc.b_PlayLoop = FALSE;

	if(!mst_SDesc.po_SoundBuffer) return;
	IDirectSoundBuffer8_Stop(mst_SDesc.po_SoundBuffer);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Sound_Close(void)
{
	if(mst_SDesc.po_SoundBuffer)
	{
		IDirectSoundBuffer8_Stop(mst_SDesc.po_SoundBuffer);
		IDirectSoundBuffer8_Release(mst_SDesc.po_SoundBuffer);
		mst_SDesc.po_SoundBuffer = NULL;
	}

	if(mst_SDesc.pWave) SND_WaveUnload(mst_SDesc.pWave);

	mst_SDesc.f_Time = 0.0f;
	mst_SDesc.ul_NumberOfSamples = 0;

	L_strcpy(mst_SDesc.sz_Path, "");
	mst_SDesc.ul_Fat = BIG_C_InvalidIndex;
	mst_SDesc.ul_Key = BIG_C_InvalidKey;

	mst_SDesc.b_Paused = FALSE;
	mst_SDesc.b_PlayLoop = FALSE;

	UpdateVarView_Sound();
	mpo_PaneSound->Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Sound_Pause(void)
{
	/*~~~~~~~~~*/
	ULONG	play;
	/*~~~~~~~~~*/

	if(!mst_SDesc.po_SoundBuffer) return;

	if(mst_SDesc.b_Paused)
	{
		Sound_Play(mst_SDesc.b_PlayLoop);
	}
	else
	{
		IDirectSoundBuffer8_GetStatus(mst_SDesc.po_SoundBuffer, &play);
		if(play & DSBSTATUS_PLAYING)
		{
			IDirectSoundBuffer8_Stop(mst_SDesc.po_SoundBuffer);
			mst_SDesc.b_Paused = TRUE;
		}
		else
			Sound_Play();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Sound_Play(BOOL _b_Looped)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	unsigned int	ui_Pos;
	HRESULT			hr;
	/*~~~~~~~~~~~~~~~~~~~*/

	/*$1- check player settings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_SDesc.f_PlayerVol < 0.0f) mst_SDesc.f_PlayerVol = 1.0f;
	if(mst_SDesc.f_PlayerVol > 1.0f) mst_SDesc.f_PlayerVol = 1.0f;
	if(mst_SDesc.f_PlayerStartTime < 0.0f) mst_SDesc.f_PlayerStartTime = 0.0f;
	if(mst_SDesc.f_PlayerStartTime > 100.0f) mst_SDesc.f_PlayerStartTime = 0.0f;
	if(mst_SDesc.i_PlayerPan < DSBPAN_LEFT) mst_SDesc.i_PlayerPan = DSBPAN_LEFT;
	if(mst_SDesc.i_PlayerPan > DSBPAN_RIGHT) mst_SDesc.i_PlayerPan = DSBPAN_RIGHT;

	if(mst_SDesc.ui_PlayerFreq)
	{
		if(mst_SDesc.ui_PlayerFreq < DSBFREQUENCY_MIN) mst_SDesc.ui_PlayerFreq = DSBFREQUENCY_MIN;
		if(mst_SDesc.ui_PlayerFreq > DSBFREQUENCY_MAX) mst_SDesc.ui_PlayerFreq = DSBFREQUENCY_MAX;
	}

	if(!mst_SDesc.po_SoundBuffer) return;

	/*$1- stop previous sound ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Sound_Stop();

	/*$1- set param ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	IDirectSoundBuffer8_SetVolume(mst_SDesc.po_SoundBuffer, SND_l_GetAttFromVol(mst_SDesc.f_PlayerVol));
	IDirectSoundBuffer8_SetPan(mst_SDesc.po_SoundBuffer, mst_SDesc.i_PlayerPan);

	if(mst_SDesc.ui_PlayerFreq)
	{
		IDirectSoundBuffer8_SetFrequency(mst_SDesc.po_SoundBuffer, mst_SDesc.ui_PlayerFreq);
	}

	ui_Pos = (unsigned int) (mst_SDesc.f_PlayerStartTime * (float) (mst_SDesc.pWave->dwSamplesPerSec * mst_SDesc.pWave->wBlockAlign));
	ui_Pos = ui_Pos / mst_SDesc.pWave->wBlockAlign;
	ui_Pos = ui_Pos * mst_SDesc.pWave->wBlockAlign;
	IDirectSoundBuffer8_SetCurrentPosition(mst_SDesc.po_SoundBuffer, ui_Pos);

	/*$1- play ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mst_SDesc.b_PlayLoop = _b_Looped;
	mst_SDesc.b_Paused = FALSE;
	hr = IDirectSoundBuffer8_Play(mst_SDesc.po_SoundBuffer, 0, 0, (_b_Looped ? DSBPLAY_LOOPING : 0));
	ERR_X_Warning((hr == DS_OK), "[SND] can not play this DirectSoundBuffer", NULL);

	if(hr == DS_OK)
	{
		mui_TimerId = SetTimer(2, 50, NULL);
	}
}

int nSamplesPerSec, nChannels, wBitsPerSample;

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void ESON_CB_ModifyMicroPos2(void *p1, void *p2, void *p_NewValue, LONG l_OldValue)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(*(LONG *) p_NewValue != l_OldValue)
	{
		pst_GO = *(OBJ_tdst_GameObject **) p_NewValue;
		SND_gst_Params.pst_RefForVol = pst_GO->pst_GlobalMatrix;
		SND_gst_Params.ul_Flags &= ~SND_Cte_Freeze3DVol;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::UpdateVarView_Sound(BOOL bForce)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem		*po_Item;
	LONG					key, idx;
	SND_tdst_SoundInstance	*pst_SI;
	SND_tdst_OneSound		*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_MF()->LockDisplay(mpo_VarsView1);
	mpo_VarsView1->ResetList();

	if(SND_gst_Params.l_Available)
	{
		/* FX */
		mpo_VarsView1->AddItem("Fx A", EVAV_EVVIT_Separator, NULL);
        SND_i_FxSeti(SND_Cte_Fx_iCoreId, SND_Cte_FxCoreA);
        key = SND_i_FxGeti(SND_Cte_Fx_iActiveContext);
        if(key == -1)
        {
            mpo_VarsView1->AddItem("No fx", EVAV_EVVIT_String, "No fx", EVAV_ReadOnly);
        }
        else
        {
            if(key == SND_Cte_FxContextLocal)
                mpo_VarsView1->AddItem("Context", EVAV_EVVIT_String, "local", EVAV_ReadOnly);
            else
                mpo_VarsView1->AddItem("Context", EVAV_EVVIT_String, "global", EVAV_ReadOnly);

            SND_i_FxSeti(SND_Cte_Fx_iCoreId, SND_Cte_FxCoreA);
            SND_i_FxSeti(SND_Cte_Fx_iContext, key);
            gauiFxFlags[0] = SND_i_FxGeti(SND_Cte_Fx_iFlags);
		    if(gauiFxFlags[0] & SND_FxFlg_Locked)
			    mpo_VarsView1->AddItem("Flags", EVAV_EVVIT_String, "locked", EVAV_ReadOnly);
		    else
			    mpo_VarsView1->AddItem("Flags", EVAV_EVVIT_String, "unlocked", EVAV_ReadOnly);

		    gafWetVol[0] = ediSND_f_FxGetWetVol(SND_Cte_FxCoreA);
		    mpo_VarsView1->AddItem("WetVol", EVAV_EVVIT_Float, &gafWetVol[0], EVAV_ReadOnly);

		    switch(ediSND_i_FxGetMode(SND_Cte_FxCoreA))
		    {
		    case SND_Cte_FxMode_Off:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "Off", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_Room:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "room", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_StudioA:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "studioA", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_StudioB:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "studioB", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_StudioC:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "studioC", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_Hall:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "hall", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_Delay:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "dealy", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_Echo:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "echo", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_Space:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "space", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_Pipe:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "pipe", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_City:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "city", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_Mountains:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "mountain", EVAV_ReadOnly);break;
		    default:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "invalid", EVAV_ReadOnly);break;
		    }

		    switch(ediSND_i_FxGetMode(SND_Cte_FxCoreA))
		    {
            case SND_Cte_FxMode_Echo:
            case SND_Cte_FxMode_Delay:
    		    gaiFxDelay[0] = ediSND_i_FxGetDelay(SND_Cte_FxCoreA);
	    	    mpo_VarsView1->AddItem("Delay", EVAV_EVVIT_Int, &gaiFxDelay[0], EVAV_ReadOnly);

		        gaiFxFeedback[0] = ediSND_i_FxGetFeedback(SND_Cte_FxCoreA);
		        mpo_VarsView1->AddItem("Feedback", EVAV_EVVIT_Int, &gaiFxFeedback[0], EVAV_ReadOnly);
                break;
            default:break;
            }
        }
        
		

		/* FX B */
		mpo_VarsView1->AddItem("Fx B", EVAV_EVVIT_Separator, NULL);
        SND_i_FxSeti(SND_Cte_Fx_iCoreId, SND_Cte_FxCoreB);
        key = SND_i_FxGeti(SND_Cte_Fx_iActiveContext);
        if(key == -1)
        {
            mpo_VarsView1->AddItem("No fx", EVAV_EVVIT_String, "No fx", EVAV_ReadOnly);
        }
        else
        {
            if(key == SND_Cte_FxContextLocal)
                mpo_VarsView1->AddItem("Context", EVAV_EVVIT_String, "local", EVAV_ReadOnly);
            else
                mpo_VarsView1->AddItem("Context", EVAV_EVVIT_String, "global", EVAV_ReadOnly);

            SND_i_FxSeti(SND_Cte_Fx_iCoreId, SND_Cte_FxCoreB);
            SND_i_FxSeti(SND_Cte_Fx_iContext, key);
            gauiFxFlags[0] = SND_i_FxGeti(SND_Cte_Fx_iFlags);
		    if(gauiFxFlags[0] & SND_FxFlg_Locked)
			    mpo_VarsView1->AddItem("Flags", EVAV_EVVIT_String, "locked", EVAV_ReadOnly);
		    else
			    mpo_VarsView1->AddItem("Flags", EVAV_EVVIT_String, "unlocked", EVAV_ReadOnly);

            gafWetVol[1] = ediSND_f_FxGetWetVol(SND_Cte_FxCoreB);
		    mpo_VarsView1->AddItem("WetVol", EVAV_EVVIT_Float, &gafWetVol[1], EVAV_ReadOnly);

            switch(ediSND_i_FxGetMode(SND_Cte_FxCoreB))
		    {
		    case SND_Cte_FxMode_Off:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "Off", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_Room:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "room", EVAV_ReadOnly);break;
            case SND_Cte_FxMode_StudioA:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "studioA", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_StudioB:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "studioB", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_StudioC:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "studioC", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_Hall:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "hall", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_Delay:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "dealy", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_Echo:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "echo", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_Space:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "space", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_Pipe:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "pipe", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_City:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "city", EVAV_ReadOnly);break;
		    case SND_Cte_FxMode_Mountains:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "mountain", EVAV_ReadOnly);break;
		    default:mpo_VarsView1->AddItem("Mode", EVAV_EVVIT_String, "invalid", EVAV_ReadOnly);break;
		    }

		    switch(ediSND_i_FxGetMode(SND_Cte_FxCoreB))
		    {
            case SND_Cte_FxMode_Echo:
            case SND_Cte_FxMode_Delay:
		        gaiFxDelay[1] = ediSND_i_FxGetDelay(SND_Cte_FxCoreB);
		        mpo_VarsView1->AddItem("Delay", EVAV_EVVIT_Int, &gaiFxDelay[1], EVAV_ReadOnly);
    		    
                gaiFxFeedback[1] = ediSND_i_FxGetFeedback(SND_Cte_FxCoreB);
		        mpo_VarsView1->AddItem("Feedback", EVAV_EVVIT_Int, &gaiFxFeedback[1], EVAV_ReadOnly);
                break;

            default:break;
            }
        }

		/* MICRO */
		mpo_VarsView1->AddItem("Micro", EVAV_EVVIT_Separator, NULL);
		po_Item = mpo_VarsView1->AddItem("MicroPos", EVAV_EVVIT_GO, &SND_gst_Params.pst_EdiMicroGao, EVAV_None);
		po_Item->mpfn_CB = ESON_CB_ModifyMicroPos2;

		/* GLOBAL FACTOR */
		mpo_VarsView1->AddItem("3D Settings", EVAV_EVVIT_Separator, NULL);
		mpo_VarsView1->AddItem("Doppler factor", EVAV_EVVIT_Float, &SND_gst_Params.f_DopplerFactor, EVAV_None);
		mpo_VarsView1->AddItem("Doppler Sound Speed", EVAV_EVVIT_Float, &SND_gst_Params.f_DopplerSoundSpeed, EVAV_None);
		mpo_VarsView1->AddItem("Rolloff factor", EVAV_EVVIT_Float, &SND_gst_Params.f_RolloffFactor, EVAV_None);
		mpo_VarsView1->AddItem("Surround factor", EVAV_EVVIT_Float, &SND_gst_Params.f_SurroundFactor, EVAV_None);

		/* WAC */
		mpo_VarsView1->AddItem("WAC Settings", EVAV_EVVIT_Separator, NULL);

		key = SND_i_GetWacSound();
		if(key >= 0)
		{
			pst_Sound = SND_gst_Params.dst_Sound + key;
			if(pst_Sound->ul_Flags & SND_Cul_DSF_Used)
			{
				if(pst_Sound->ul_Flags & SND_Cul_SF_LoadingSound)
					mpo_VarsView1->AddItem("Current file", EVAV_EVVIT_SndKey, &pst_Sound->ul_FileKey, EVAV_ReadOnly);
				else
				{
					key = -1;
					mpo_VarsView1->AddItem("Current file", EVAV_EVVIT_SndKey, &key, EVAV_ReadOnly);
				}
			}
			else
			{
				key = -1;
				mpo_VarsView1->AddItem("Current file", EVAV_EVVIT_SndKey, &key, EVAV_ReadOnly);
			}
		}

		idx = SND_i_GetWacInstance();
		if(idx >= 0)
		{
			pst_SI = SND_gst_Params.dst_Instance + idx;
			if(pst_SI->ul_Flags & SND_Cul_DSF_Used)
			{
				if(pst_SI->p_GameObject)
					mpo_VarsView1->AddItem("Current Gao", EVAV_EVVIT_GO, &pst_SI->p_GameObject, EVAV_ReadOnly);
			}
		}

		if(mst_SDesc.ul_Fat != BIG_C_InvalidIndex)
		{
			/* player */
			mpo_VarsView1->AddItem("Player Settings", EVAV_EVVIT_Separator, NULL);
			mpo_VarsView1->AddItem("Frequency (Hz)", EVAV_EVVIT_Int, &mst_SDesc.ui_PlayerFreq, 0);
			mpo_VarsView1->AddItem("Pan", EVAV_EVVIT_Int, &mst_SDesc.i_PlayerPan, 0);
			mpo_VarsView1->AddItem("Volume", EVAV_EVVIT_Float, &mst_SDesc.f_PlayerVol, 0);
			mpo_VarsView1->AddItem("StartTime", EVAV_EVVIT_Float, &mst_SDesc.f_PlayerStartTime, 0);

			/* static data */
			mpo_VarsView1->AddItem("Static Data", EVAV_EVVIT_Separator, NULL);
			mpo_VarsView1->AddItem("Key", EVAV_EVVIT_Hexa, &mst_SDesc.ul_Key, EVAV_ReadOnly);
			mpo_VarsView1->AddItem("Name", EVAV_EVVIT_Key, &mst_SDesc.ul_Key, EVAV_ReadOnly);
			mpo_VarsView1->AddItem("Path", EVAV_EVVIT_String, &mst_SDesc.sz_Path, EVAV_ReadOnly);
			nSamplesPerSec = mst_SDesc.pWave->dwSamplesPerSec;
			mpo_VarsView1->AddItem("Frequency (Hz)", EVAV_EVVIT_Int, &nSamplesPerSec, EVAV_ReadOnly);
			nChannels = mst_SDesc.pWave->wChannels;
			mpo_VarsView1->AddItem("Channels", EVAV_EVVIT_Int, &nChannels, EVAV_ReadOnly);
			wBitsPerSample = mst_SDesc.pWave->wBitsPerSample;
			mpo_VarsView1->AddItem("Bit per sample", EVAV_EVVIT_Int, &wBitsPerSample, EVAV_ReadOnly);
			mpo_VarsView1->AddItem("Size (oct)", EVAV_EVVIT_Int, &mst_SDesc.pWave->ul_DataSize, EVAV_ReadOnly);
			mpo_VarsView1->AddItem("Time (s)", EVAV_EVVIT_Float, &mst_SDesc.f_Time, EVAV_ReadOnly);

			if(BIG_b_IsFileExtension(mst_SDesc.ul_Fat, EDI_Csz_ExtSoundMusic))
				sprintf(ESON_asz_Temp, "music");
			else if(BIG_b_IsFileExtension(mst_SDesc.ul_Fat, EDI_Csz_ExtSoundAmbience))
				sprintf(ESON_asz_Temp, "ambience");
			else if(BIG_b_IsFileExtension(mst_SDesc.ul_Fat, EDI_Csz_ExtSoundDialog))
				sprintf(ESON_asz_Temp, "dialog");
			else if(BIG_b_IsFileExtension(mst_SDesc.ul_Fat, EDI_Csz_ExtSoundFile))
				sprintf(ESON_asz_Temp, "sound");
			else if(BIG_b_IsFileExtension(mst_SDesc.ul_Fat, EDI_Csz_ExtLoadingSound))
				sprintf(ESON_asz_Temp, "loading sound");
			else if(BIG_b_IsFileExtension(mst_SDesc.ul_Fat, EDI_Csz_ExtSModifier))
				sprintf(ESON_asz_Temp, "smodifier");
			else
				sprintf(ESON_asz_Temp, "unknown");
			mpo_VarsView1->AddItem("Type", EVAV_EVVIT_String, ESON_asz_Temp, EVAV_ReadOnly);
		}
	}

	mpo_VarsView1->SetItemList(&mo_ListItems1);
	M_MF()->UnlockDisplay(mpo_VarsView1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Sound_ResetSpy(void)
{
	ESON_ResetSpy();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Sound_SetSpy(BOOL _b_UseSelected)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog("Enter a key");
	EDIA_cl_FileDialog	o_File
						(
							"Choose File",
							0,
							0,
							1,
							EDI_Csz_Path_Audio,
							"*" EDI_Csz_ExtSoundFile ",*" EDI_Csz_ExtLoadingSound ",*" EDI_Csz_ExtSModifier ",*" EDI_Csz_ExtSoundMusic ",*"
								EDI_Csz_ExtSoundAmbience ",*" EDI_Csz_ExtSoundDialog
						);
	CString				o_Temp;
	BIG_INDEX			ul_Index;
	int					ret;
	char				*psz_Name;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_b_UseSelected)
	{
		if(mst_SDesc.ul_Key == BIG_C_InvalidKey) return;
		mst_Ini.i_Option |= ESON_Cte_EnableDebugLog;
		ESON_SetSpy(mst_SDesc.ul_Key);
	}
	else
	{
		ret = M_MF()->MessageBox
			(
				"Do you know the key (cancel=desactivate)?",
				"SND-SPY",
				MB_YESNOCANCEL | MB_ICONQUESTION
			);
		switch(ret)
		{
		case IDYES:
			if(o_Dialog.DoModal() == IDOK)
			{
				psz_Name = (char *) (LPCSTR) o_Dialog.mo_Name;
				_strlwr(psz_Name);
				if(L_strchr(psz_Name, 'x'))
					sscanf(psz_Name, "0x%x", &ul_Index);
				else
					sscanf(psz_Name, "%x", &ul_Index);
				mst_Ini.i_Option |= ESON_Cte_EnableDebugLog;
				ESON_SetSpy(ul_Index);
			}
			else
			{
				mst_Ini.i_Option |= ESON_Cte_EnableDebugLog;
				ESON_ResetSpy();
			}
			break;

		case IDNO:
			if(o_File.DoModal() == IDOK)
			{
				o_File.GetItem(o_File.mo_File, 0, o_Temp);
				ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
				ESON_SetSpy(BIG_FileKey(ul_Index));
			}
			else
			{
				ESON_ResetSpy();
			}
			break;

		case IDCANCEL:
		default:
			ESON_ResetSpy();
			break;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Sound_UserReport(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem				*pst_CurrentElemW;
	TAB_tdst_PFelem				*pst_EndElemW;
	WOR_tdst_World				*pst_World;
	TAB_tdst_PFtable			*pst_AWO;
	TAB_tdst_PFelem				*pst_CurrentElem;
	TAB_tdst_PFelem				*pst_EndElem;
	OBJ_tdst_GameObject			*pst_GAO;
	MDF_tdst_Modifier			*pst_MdF;
	GEN_tdst_ModifierSound		*pst_SndMdF;
	char						az_msg[1024];
	int							i, j;
	SND_tdst_Bank				*pst_Bank;
	SND_tdst_OneSound			*pst_Sound;
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	ULONG						ul_Fat, ul_Key;
	int							nb;
	unsigned short				us;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_SDesc.ul_Fat == BIG_C_InvalidIndex) return;

	ul_Fat = mst_SDesc.ul_Fat;
	ul_Key = mst_SDesc.ul_Key;
	if(mst_SDesc.pWave == NULL)
	{
		LINK_PrintStatusMsg("[SND] This sound is not loaded !");
		return;
	}

	LINK_PrintStatusMsg("---------------------------------------------");
	LINK_PrintStatusMsg("Sound user report :");
	LINK_PrintStatusMsg("-------------------");

	sprintf(az_msg, "Users of sound %s [%08x] are :", BIG_NameFile(ul_Fat), ul_Key);
	LINK_PrintStatusMsg(az_msg);

	/*$2- search in bank ---------------------------------------------------------------------------------------------*/

	for(i = 0; i < (int) SND_gul_MainRefListSize; i++)
	{
		pst_Bank = SND_p_MainGetBank(SND_gap_MainRefList[i]);
		nb = 0;

		if(pst_Bank->pi_SaveBank)
		{
			for(j = 0; j < pst_Bank->i_SoundNb; j++)
			{
				if(pst_Bank->pi_SaveBank[j] == -1) continue;
				if(SND_gst_Params.dst_Sound[pst_Bank->pi_SaveBank[j]].ul_FileKey == ul_Key)
				{
					nb++;
				}
			}
		}
		else
		{
			for(j = 0; j < pst_Bank->i_SoundNb; j++)
			{
				if(SND_gst_Params.dst_Sound[pst_Bank->pi_Bank[j]].ul_FileKey == ul_Key)
				{
					nb++;
				}
			}
		}

		if(nb)
		{
			sprintf
			(
				az_msg,
				"[%08x] x%d %s",
				pst_Bank->ul_FileKey,
				nb,
				BIG_NameFile(BIG_ul_SearchKeyToFat(pst_Bank->ul_FileKey))
			);
			LINK_PrintStatusMsg(az_msg);
		}
	}

	/*$2- search in MdF ----------------------------------------------------------------------------------------------*/

	pst_CurrentElemW = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
	pst_EndElemW = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
	for(pst_CurrentElemW; pst_CurrentElemW <= pst_EndElemW; pst_CurrentElemW++)
	{
		pst_World = (WOR_tdst_World *) pst_CurrentElemW->p_Pointer;

		/*$1- in each world ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(!TAB_b_IsAHole(pst_World))
		{
			pst_AWO = &(pst_World->st_AllWorldObjects);

			pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AWO);
			pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AWO);

			/*$1- register all gao that has one Sound Bank ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
			{
				pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
				if(TAB_b_IsAHole(pst_GAO)) continue;

				if((pst_GAO->pst_Extended) && (pst_GAO->pst_Extended->pst_Modifiers))
				{
					pst_MdF = pst_GAO->pst_Extended->pst_Modifiers;
					while(pst_MdF)
					{
						if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
						{
							pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
							if(pst_SndMdF->ui_FileKey == ul_Key)
							{
								sprintf
								(
									az_msg,
									"[%08x] %s (MdF #%d)",
									LOA_ul_SearchKeyWithAddress((ULONG) pst_GAO),
									pst_GAO->sz_Name,
									pst_SndMdF->ui_Id
								);
								LINK_PrintStatusMsg(az_msg);
							}
						}

						pst_MdF = pst_MdF->pst_Next;
					}
				}
			}
		}
	}

	/*$2- search in SModifier ----------------------------------------------------------------------------------------*/

	for(i = 0; i < SND_gst_Params.l_SoundNumber; i++)
	{
		pst_Sound = SND_gst_Params.dst_Sound + i;

		if((pst_Sound->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
		if((pst_Sound->ul_Flags & SND_Cul_SF_SModifier) == 0) continue;

		pst_ExtPlayer = (SND_tdst_SModifierExtPlayer *) (SND_pst_SModifierGet
			(
				pst_Sound->pst_SModifier,
				SND_Cte_SModifierExtPlayer,
				0
				))->pv_Data;
		if(pst_ExtPlayer)
		{
			nb=0;
			for(us=0; us<pst_ExtPlayer->st_PlayList.us_Size; us++)
			{
				if(pst_ExtPlayer->st_PlayList.aul_KeyList[us] == ul_Key)
				{
					nb++;
				}
			}

			if(nb)
			{
				sprintf
				(
					az_msg,
					"[%08x] x%d %s",
					pst_Sound->ul_FileKey,
					nb,
					BIG_NameFile(BIG_ul_SearchKeyToFat(pst_Sound->ul_FileKey))
				);
				LINK_PrintStatusMsg(az_msg);
			}
		}
	}

	LINK_PrintStatusMsg("---------------------------------------------");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Sound_OnFindFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	EBRO_cl_Frame	*po_Browser;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_SDesc.ul_Fat == BIG_C_InvalidIndex) return;

	po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
	po_Browser->mpo_MyView->IWantToBeActive(po_Browser);
	po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(mst_SDesc.ul_Fat), mst_SDesc.ul_Fat);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Sound_FindReference(ULONG ulKey)
{
	ULONG ul_Index;
	SND_tdst_OneSound*pst_Sound ;
	SND_tdst_SModifierExtPlayer*pst_ExtPlayer ;
	char	log[256];
	LONG ll;
	EDIA_cl_NameDialog	o_Dialog("Enter key to search");

	
	

	if(ulKey == BIG_C_InvalidKey)
	{
		if(o_Dialog.DoModal() == IDOK)
			sscanf((char *) (LPCSTR) o_Dialog.mo_Name, "%x", &ulKey);
		else
			return;
	}
	
	AfxGetApp()->DoWaitCursor(1);

	SModifier_CloseCurrent();
	SoundBank_Close();


	sprintf(log, "Looking for which smd/snk is referencing [%08x] key...", ulKey);
	LINK_PrintStatusMsg(log);
	LINK_PrintStatusMsg("-------------------------------------------------");


	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(BIG_FileKey(ul_Index) == BIG_C_InvalidKey) continue;

		if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtSModifier)) 
		{
			SModifier_Set(ul_Index, FALSE);
			if(mst_SModifierDesc.i_Index < 0) continue;
			
			pst_Sound = &SND_gst_Params.dst_Sound[mst_SModifierDesc.i_Index];
			if(!pst_Sound->pst_SModifier) continue;

			pst_ExtPlayer = (SND_tdst_SModifierExtPlayer*)pst_Sound->pst_SModifier->pv_Data;
			for(unsigned short us=0; us<pst_ExtPlayer->st_PlayList.us_Size; us++)
			{
				if(pst_ExtPlayer->st_PlayList.aul_KeyList[us] == ulKey)
				{
					sprintf(log, "[%08x] %s",BIG_FileKey(ul_Index), BIG_NameFile(ul_Index));
					LINK_PrintStatusMsg(log);
					break;
				}
			}
			
			SModifier_CloseCurrent(0);
		}
		else if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtSoundBank)) 
		{
			SoundBank_Set(ul_Index, FALSE);
			for(int i = 0; i<mst_SndBankDesc.pst_CurrentBank->st_Bank.i_SoundNb; i++)
			{
				ll = mst_SndBankDesc.pst_CurrentBank->st_Bank.pi_Bank[i];
				pst_Sound = &SND_gst_Params.dst_Sound[ll];

				if(pst_Sound->ul_FileKey == ulKey)
				{
					sprintf(log, "[%08x] %s",BIG_FileKey(ul_Index), BIG_NameFile(ul_Index));
					LINK_PrintStatusMsg(log);
					break;
				}
			}
			SoundBank_Close();
		}
	}

	LINK_PrintStatusMsg("--.");
	AfxGetApp()->DoWaitCursor(-1);
}
/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
