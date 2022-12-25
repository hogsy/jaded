
#ifndef __GX8FLARE_H__
#define __GX8FLARE_H__


#include "BASe/BAStypes.h"

struct GFX_tdst_ComplexFlare_;
typedef struct GFX_tdst_ComplexFlare_ GFX_tdst_ComplexFlare;

struct GFX_tdst_Sun_;
typedef struct GFX_tdst_Sun_ GFX_tdst_Sun;

// =======================================================================================================================
// =======================================================================================================================

int Gx8_TestFlareVisibility(GDI_tdst_DisplayData *_pst_DD, GFX_tdst_ComplexFlare *_pst_Data);

void Gx8_TestSunVisibility(GDI_tdst_DisplayData *_pst_DD, GFX_tdst_Sun *_pst_Data);

#endif	//__GX8FLARE_H__


