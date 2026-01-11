/*$T MDFmodifier_GEO.h GC! 1.081 11/14/00 12:12:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_MATRIXBORE_H__
#define __MDFMODIFIER_MATRIXBORE_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Modifier const
 ***********************************************************************************************************************
 */


/*$4
 ***********************************************************************************************************************
    Modifier desc
 ***********************************************************************************************************************
 */
typedef struct	MatrixBore_tdst_Modifier_
{
#ifdef ACTIVE_EDITORS
	ULONG				ulCodeKey;
#endif
	float				Strenght;
} MatrixBore_tdst_Modifier;



/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

extern void							MatrixBore_Modifier_Create(struct OBJ_tdst_GameObject_ *,MDF_tdst_Modifier *,void *);
extern void							MatrixBore_Modifier_Destroy(MDF_tdst_Modifier *);
extern void							MatrixBore_Modifier_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void							MatrixBore_Modifier_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern ULONG						MatrixBore_ul_Modifier_Load(MDF_tdst_Modifier *, char *);
extern void							MatrixBore_Modifier_Save(MDF_tdst_Modifier *);
extern int							MatrixBore_Modifier_Copy(MDF_tdst_Modifier *, MDF_tdst_Modifier *);
void 								MatrixBore_Modifier_Interpolate(struct MDF_tdst_Modifier_ * p_Mod, unsigned int Globalmode , float fInterpolatedValue);
extern void							MatrixBore_Modifier_FlushAll();

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


#endif /* __MDFMODIFIER_MATRIXBORE_H__ */
