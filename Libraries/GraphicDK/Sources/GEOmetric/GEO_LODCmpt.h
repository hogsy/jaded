#include "BASe/BAStypes.h"

#ifdef ACTIVE_EDITORS
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
ULONG LOD_C_ComputeObjectLOD(MAD_GeometricObject * ObjectToDraw, ULONG(*SeprogressPos) (float ,char *),unsigned short **pp_usReorder, float **pdf_Errors);
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* ACTIVE_EDITOR */
