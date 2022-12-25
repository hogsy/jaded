/*$T COLinst.h GC!1.5 11/22/99 15:14:23 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __COL_INST__
#define __COL_INST__

#pragma once
#ifdef __cplusplus
extern "C"
{
#endif /* _cplusplcus */

/*$F GC Dependencies 11/22/99 */
#include "COLstruct.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "BASe/BAStypes.h"

void    COL_ComputeArrayOfPointers(COL_tdst_Instance *);

#ifdef __cplusplus
}
#endif /* _cplusplcus */

#endif /* __COL_INST_ */
