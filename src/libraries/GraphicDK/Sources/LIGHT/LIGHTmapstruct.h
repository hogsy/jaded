#ifndef __LIGHTMAP_STRUCT_H__
#define __LIGHTMAP_STRUCT_H__


typedef struct _LIGHT_st_LightmapPageInfo
{
	ULONG					texBFKey;
	USHORT					texIndex;
	USHORT					nbRef;
#ifdef ACTIVE_EDITORS
	void*					pData;
	int						pageWidth;
	int						pageHeight;
#endif
	struct _LIGHT_st_LightmapPageInfo* pNextLightmap;
} LIGHT_tdst_LightmapPageInfo;
 
#ifdef ACTIVE_EDITORS

// lightmap settings 1st version
typedef struct 
{
	unsigned int bUseLightmaps:1;			// do not move this bit field unless you also change the links in the editor!!!!
	unsigned int bCastShadows:1;
	unsigned int bReceiveShadows:1;
	unsigned int bCustomTexelRatio:1;
	unsigned int bTemporaryStopUsingLightmaps:1;
	float		 fTexelPerMeter;
} LIGHT_tdst_LightmapSettings_V1;		
 
typedef LIGHT_tdst_LightmapSettings_V1 LIGHT_tdst_LightmapSettings;

#endif

typedef enum {LMSettingsV1 = 1} LIGHT_eLMSettingsVersion;

// change this define when switching the version
#define LMSettingsVersion LMSettingsV1

typedef enum {LMDataV1 = 1} LIGHT_eLMDataVersion;

#define LMDataVersion	LMDataV1

#endif

