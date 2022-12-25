/*$T MATShadow.h */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __MATShadow_H__
#define __MATShadow_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "GDInterface/GDInterface.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Display Data functions
 ***********************************************************************************************************************
 */
void    SDW_AddAShadow(struct DD_tdst_ShadowStackNode_ * p_NewShadow);
void    SDW_Clear(void);
void    SDW_Destroy(void);
void    SDW_UpdatePositions(void);
void	SDW_DisplayShadowsOnCurrentObject(struct GEO_tdst_Object_   *pst_Obj);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif 
