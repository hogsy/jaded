
#pragma once

typedef enum _DSP_IMAGE_jadedsp_FX_INDICES {
    UserStereoEchoA_StereoEchoA = 0,
    UserStereoEchoB_StereoEchoB = 1,
    UserI3DL224KReverbB_I3DL224KReverbB = 2,
    User2x1Mixer_2x1Mixer = 3,
    GraphI3DL2_I3DL2Reverb = 4,
    GraphXTalk_XTalk = 5
} DSP_IMAGE_jadedsp_FX_INDICES;

#define DSI3DL2_ENVIRONMENT_UserI3DL224KReverbB_I3DL224KReverbB -1000, -454, 0.000000, 0.400000, 0.830000, -1646, 0.002000, 53, 0.003000, 100.000000, 100.000000, 5000.000000

#define DSI3DL2_ENVIRONMENT_GraphI3DL2_I3DL2Reverb -1000, -454, 0.000000, 0.400000, 0.830000, -1646, 0.002000, 53, 0.003000, 100.000000, 100.000000, 5000.000000

typedef struct _UserStereoEchoA_FX0_StereoEchoA_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[2];     // XRAM offsets in DSP WORDS, of output mixbins
} UserStereoEchoA_FX0_StereoEchoA_STATE, *LPUserStereoEchoA_FX0_StereoEchoA_STATE;

typedef const UserStereoEchoA_FX0_StereoEchoA_STATE *LPCUserStereoEchoA_FX0_StereoEchoA_STATE;

typedef struct _UserStereoEchoB_FX0_StereoEchoB_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[2];     // XRAM offsets in DSP WORDS, of output mixbins
} UserStereoEchoB_FX0_StereoEchoB_STATE, *LPUserStereoEchoB_FX0_StereoEchoB_STATE;

typedef const UserStereoEchoB_FX0_StereoEchoB_STATE *LPCUserStereoEchoB_FX0_StereoEchoB_STATE;

typedef struct _UserI3DL224KReverbB_FX0_I3DL224KReverbB_STATE {
    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX
    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX
    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX
    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX
    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation
    DWORD dwInMixbinPtrs[2];      // XRAM offsets in DSP WORDS, of input mixbins
    DWORD dwOutMixbinPtrs[35];     // XRAM offsets in DSP WORDS, of output mixbins
} UserI3DL224KReverbB_FX0_I3DL224KReverbB_STATE, *LPUserI3DL224KReverbB_FX0_I3DL224KReverbB_STATE;

typedef const UserI3DL224KReverbB_FX0_I3DL224KReverbB_STATE *LPCUserI3DL224KReverbB_FX0_I3DL224KReverbB_STATE;

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
