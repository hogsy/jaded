#ifndef __PCRETAILSNDFX_H__
#define __PCRETAILSNDFX_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define M_I3DL2Assign(_a, _b) \
	{ \
		_a.lRoom = _b.lRoom; \
		_a.lRoomHF = _b.lRoomHF; \
		_a.flRoomRolloffFactor = _b.flRoomRolloffFactor; \
		_a.flDecayTime = _b.flDecayTime; \
		_a.flDecayHFRatio = _b.flDecayHFRatio; \
		_a.lReflections = _b.lReflections; \
		_a.flReflectionsDelay = _b.flReflectionsDelay; \
		_a.lReverb = _b.lReverb; \
		_a.flReverbDelay = _b.flReverbDelay; \
		_a.flDiffusion = _b.flDiffusion; \
		_a.flDensity = _b.flDensity; \
		_a.flHFReference = _b.flHFReference; \
	}

/*Struct to hold the interfaces to Fx*/
typedef struct _SND_tdst_DirectSoundFX_
{
	int i;
	LPDIRECTSOUNDFXCHORUS				pst_DSFXChorus;
	LPDIRECTSOUNDFXCOMPRESSOR			pst_DSFXCompressor;
	LPDIRECTSOUNDFXDISTORTION			pst_DSFXDistortion;
	LPDIRECTSOUNDFXECHO					pst_DSFXEcho;
	LPDIRECTSOUNDFXFLANGER				pst_DSFXFlanger;
	LPDIRECTSOUNDFXGARGLE				pst_DSFXGargle;
	LPDIRECTSOUNDFXI3DL2REVERB			pst_DSFXI3DL2Reverb;
	LPDIRECTSOUNDFXWAVESREVERB			pst_DSFXWavesReverb;
	LPDIRECTSOUNDFXPARAMEQ				pst_DSFXParamEq;
	int									i_Id;
} SND_tdst_DirectSoundFX;

/*$2- prototypes for FX ----------------------------------------------------------------------------------------------*/

int								pcretail_i_FxInit(void);
void							pcretail_FxClose(void);
int								pcretail_i_FxSetMode(struct SND_tdst_FxParam_ *, int);
int								pcretail_i_FxGetMode(struct SND_tdst_FxParam_ *);
int								pcretail_i_FxSetDelay(struct SND_tdst_FxParam_ *, int);
int								pcretail_i_FxGetDelay(struct SND_tdst_FxParam_ *);
int								pcretail_i_FxSetFeedback(struct SND_tdst_FxParam_ *, int);
int								pcretail_i_FxGetFeedback(struct SND_tdst_FxParam_ *);
int								pcretail_i_FxSetWetVolume(struct SND_tdst_FxParam_ *, int);
int								pcretail_i_FxGetWetVolume(struct SND_tdst_FxParam_ *);
int								pcretail_i_FxSetWetPan(struct SND_tdst_FxParam_ *, int);
int								pcretail_i_FxGetWetPan(struct SND_tdst_FxParam_ *);
int								pcretail_i_FxEnable(struct SND_tdst_FxParam_ *);
int								pcretail_i_FxDisable(struct SND_tdst_FxParam_ *);
DWORD							SetFxFlag(bool minimumBuffer);
int								GetMinimumBufferSize(DWORD nAverageBytePerSecond);

/* Private specific functions */

int								pcretailSND_i_FxAddSB(struct SND_tdst_FxParam_ *, struct SND_tdst_SoundBuffer_ *,int looped);
int								pcretailSND_i_FxDelSB(struct SND_tdst_FxParam_ *, struct SND_tdst_SoundBuffer_ *);

#ifdef __cplusplus
}
#endif

#endif //__PCRETAILSBDFX_H__