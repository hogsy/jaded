#ifndef __GXI_SPECIALFX_H__
#define __GXI_SPECIALFX_H__

#include "GXI_GC/GXI_Def.h"

#include "MATHs/MATH.h"
#include "BASe/BAStypes.h"

#define SPECIALFX_SAND_TEXTURE_SIZE 256

typedef struct _tdstSpecialFXGlobals
{
	// sand storm variables
	FLOAT    fSandStormIntensity;
	GXColor  lSandStormColor;
	BOOL     bSandStormActive;
	GXTexObj oSandStormTexture;
	FLOAT    fSandStormTimeCurrent;
	FLOAT    fSandStormTimeStop;
	FLOAT    fSandStormTimeStart;
	FLOAT    fSandStormIntensityStart;	
	FLOAT    fSandStormIntensityStop;
	UINT     iSandStormCurrentStep;
	FLOAT    fSandStormSpeed;
	FLOAT    fSandStormAmplitude;
	FLOAT    fSandStormGrainSizeFront;
	FLOAT    fSandStormGrainSizeBack;
	Vec		 vWindDirection;
	
} tdstSpecialFXGlobal;

extern tdstSpecialFXGlobal g_SpecialFXGlobals;

/*$4
 ***************************************************************************************************
    Function
 ***************************************************************************************************
 */

void GXI_SFX_Init(){};

void  GXI_SFX_SandStormInit(){};
void  GXI_SFX_SandStormUpdate(){};
void  GXI_SFX_SandStormRender(){};

void  GXI_SFX_SandStormSetActivation(BOOL _bOn) { g_SpecialFXGlobals.bSandStormActive = _bOn; GXI_Global_ACCESS(bUseMegaFogOveride) = _bOn;};
BOOL  GXI_SFX_SandStormGetActivation() { return g_SpecialFXGlobals.bSandStormActive;};

void  GXI_SFX_SandStormSetIntensity(FLOAT _fStart, FLOAT _fEnd, FLOAT _fTime) {};
FLOAT GXI_SFX_SandStormGetIntensity() { return g_SpecialFXGlobals.fSandStormIntensity; };

void  GXI_SFX_SandStormSetSpeed(FLOAT _speed) { g_SpecialFXGlobals.fSandStormSpeed=_speed; };
FLOAT GXI_SFX_SandStormGetSpeed() { return g_SpecialFXGlobals.fSandStormSpeed; };

FLOAT GXI_SFX_SandStormSetAmplitude(FLOAT _amp) { return (g_SpecialFXGlobals.fSandStormAmplitude=_amp); };
FLOAT GXI_SFX_SandStormGetAmplitude() { return g_SpecialFXGlobals.fSandStormAmplitude; };

void  GXI_SFX_SandStormSetGrainSizeFront(FLOAT _size) { g_SpecialFXGlobals.fSandStormGrainSizeFront=_size; };
FLOAT GXI_SFX_SandStormGetGrainSizeFront() { return g_SpecialFXGlobals.fSandStormGrainSizeFront; };

FLOAT GXI_SFX_SandStormSetGrainSizeBack(FLOAT _size) { return (g_SpecialFXGlobals.fSandStormGrainSizeBack=_size); };
FLOAT GXI_SFX_SandStormGetGrainSizeBack() { return g_SpecialFXGlobals.fSandStormGrainSizeBack; };


#endif /* __GXI_AFTERFX_H__ */