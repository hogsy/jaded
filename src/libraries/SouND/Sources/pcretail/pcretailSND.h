

#ifndef __pcretailSND_h__
#define __pcretailSND_h__

#ifdef __cplusplus
extern "C"
{
#endif//__cplusplus

void pcretailSND_SB_Duplicate(SND_tdst_TargetSpecificData *_pst_SpeData,SND_tdst_SoundBuffer *_pst_SrcSB,SND_tdst_SoundBuffer **_ppst_DstSB);
int pcretailSND_l_Init(SND_tdst_TargetSpecificData *_pst_SpecificD);

#endif //__pcretailSND_h__

#ifdef __cplusplus
}
#endif //__cplusplus