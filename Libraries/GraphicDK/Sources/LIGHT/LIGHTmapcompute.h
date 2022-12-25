#ifndef PSX2_TARGET
#endif

#ifndef __LIGHTMAPCOMPUTE_H__
#define __LIGHTMAPCOMPUTE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef ACTIVE_EDITORS

typedef struct 
{
	float texelPerMeter;
	bool  doSuperSampling;
	float superSamplingFactor;
	bool  computeShadows;
	bool  shadowMapMode;
	bool  computePS2Lightmaps;
	float	shadowOpacity;
	bool  fixBackFaceBug;
} tdst_LightmapsComputationOptions;

typedef void (*LMProgressCallback)(float, char*, void*);

void LIGHT_ComputeAllLightmaps(WOR_tdst_World* _pst_World, BIG_KEY _worldKey, tdst_LightmapsComputationOptions* _pOptions, LMProgressCallback _fn_callback, void* _CallbackUserData, SEL_tdst_Selection * _pSelection);

#endif

#ifdef __cplusplus
}
#endif


#endif /* __LIGHTMAP_H__ */

