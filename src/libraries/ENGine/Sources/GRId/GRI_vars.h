/*$T GRI_vars.h GC!1.68 01/06/00 12:04:13 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GRI_VARS_H__
#define __GRI_VARS_H__

#include "ENGine/Sources/GRId/GRI_struct.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define GRID_CASE   1.0f

#define GRID_MAX_BEST		1000
#define GRID_MAX_EXPLORED	2000
extern GRID_tdst_Best       GRID_gast_Best[GRID_MAX_BEST];
extern GRID_tdst_Explored   GRID_gast_Explored[GRID_MAX_EXPLORED];

#endif /* !__GRI_VARS_H__ */
