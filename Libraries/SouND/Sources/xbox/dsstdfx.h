
#pragma once

typedef enum _DSP_IMAGE_dsstdfx_FX_INDICES {
    GraphZeroTempMixBins_ZeroTempMixbins0 = 0,
    UserStereoChorus_StereoChorus = 1,
    UserStereoEcho_StereoEcho = 2,
    Useriir2_1_iir2_1 = 3,
    Useriir2_2_iir2_2 = 4,
    User2x1Mixer_2x1Mixer = 5,
    GraphI3DL2_I3DL2Reverb = 6,
    GraphXTalk_XTalk = 7
} DSP_IMAGE_dsstdfx_FX_INDICES;

#define DSI3DL2_ENVIRONMENT_GraphI3DL2_I3DL2Reverb -1000, -270, 0.000000, 1.490000, 0.860000, -1204, 0.007000, -4, 0.011000, 100.000000, 100.000000, 5000.000000

typedef struct _GraphZeroTempMixBins_FX0_ZeroTempMixbins0_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[1];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[8];     // XRAM offsets in DSP WORDS, of output mixbins
} GraphZeroTempMixBins_FX0_ZeroTempMixbins0_STATE, *LPGraphZeroTempMixBins_FX0_ZeroTempMixbins0_STATE;

typedef const GraphZeroTempMixBins_FX0_ZeroTempMixbins0_STATE *LPCGraphZeroTempMixBins_FX0_ZeroTempMixbins0_STATE;

typedef struct _UserStereoChorus_FX0_StereoChorus_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[3];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[2];     // XRAM offsets in DSP WORDS, of output mixbins
} UserStereoChorus_FX0_StereoChorus_STATE, *LPUserStereoChorus_FX0_StereoChorus_STATE;

typedef const UserStereoChorus_FX0_StereoChorus_STATE *LPCUserStereoChorus_FX0_StereoChorus_STATE;

typedef struct _UserStereoEcho_FX0_StereoEcho_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[2];     // XRAM offsets in DSP WORDS, of output mixbins
} UserStereoEcho_FX0_StereoEcho_STATE, *LPUserStereoEcho_FX0_StereoEcho_STATE;

typedef const UserStereoEcho_FX0_StereoEcho_STATE *LPCUserStereoEcho_FX0_StereoEcho_STATE;

typedef struct _Useriir2_1_FX0_iir2_1_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[1];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} Useriir2_1_FX0_iir2_1_STATE, *LPUseriir2_1_FX0_iir2_1_STATE;

typedef const Useriir2_1_FX0_iir2_1_STATE *LPCUseriir2_1_FX0_iir2_1_STATE;

typedef struct _Useriir2_2_FX0_iir2_2_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[1];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} Useriir2_2_FX0_iir2_2_STATE, *LPUseriir2_2_FX0_iir2_2_STATE;

typedef const Useriir2_2_FX0_iir2_2_STATE *LPCUseriir2_2_FX0_iir2_2_STATE;

typedef struct _User2x1Mixer_FX0_2x1Mixer_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[1];     // XRAM offsets in DSP WORDS, of output mixbins
} User2x1Mixer_FX0_2x1Mixer_STATE, *LPUser2x1Mixer_FX0_2x1Mixer_STATE;

typedef const User2x1Mixer_FX0_2x1Mixer_STATE *LPCUser2x1Mixer_FX0_2x1Mixer_STATE;

typedef struct _GraphI3DL2_FX0_I3DL2Reverb_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[35];     // XRAM offsets in DSP WORDS, of output mixbins
} GraphI3DL2_FX0_I3DL2Reverb_STATE, *LPGraphI3DL2_FX0_I3DL2Reverb_STATE;

typedef const GraphI3DL2_FX0_I3DL2Reverb_STATE *LPCGraphI3DL2_FX0_I3DL2Reverb_STATE;

typedef struct _GraphXTalk_FX0_XTalk_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[4];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[4];     // XRAM offsets in DSP WORDS, of output mixbins
} GraphXTalk_FX0_XTalk_STATE, *LPGraphXTalk_FX0_XTalk_STATE;

typedef const GraphXTalk_FX0_XTalk_STATE *LPCGraphXTalk_FX0_XTalk_STATE;
