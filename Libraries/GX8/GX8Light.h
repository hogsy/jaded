/* Gx8Light.h */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GX8LIGHT_H__
#define __GX8LIGHT_H__

#include "Gx8init.h"
#include "BASe/BAStypes.h"
#include "OBJects/OBJstruct.h"
#include "Light/LIGHTstruct.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 ===================================================================================================
    Functions
 ===================================================================================================
 */

int Gx8_SendObjectToHWLight( LIGHT_tdst_List *_pst_LightList, OBJ_tdst_GameObject *_pst_Node);
void LIGHT_SetLighting(BOOL bSet);
void Gx8_SetAmbient(DWORD color);


#ifdef __cplusplus
}
#endif

#endif // __GX8LIGHT_H__
