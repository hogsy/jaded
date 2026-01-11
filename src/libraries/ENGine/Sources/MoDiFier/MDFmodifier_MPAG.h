/*$T MDFmodifier_XMEC.h GC! 1.100 08/29/01 14:29:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_MPAG_H__
#define __MDFMODIFIER_MPAG_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"
#include "PArticleGenerator/PAGstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
typedef struct	MPAG_tdst_Modifier_
{
	PAG_tdst_Struct *pst_P;
} MPAG_tdst_Modifier;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern void		MPAG_Modifier_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void		MPAG_Modifier_Destroy(MDF_tdst_Modifier *);
extern void		MPAG_Modifier_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		MPAG_Modifier_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		MPAG_ul_Modifier_Reinit(MDF_tdst_Modifier *);
extern ULONG	MPAG_ul_Modifier_Load(MDF_tdst_Modifier *, char *);
extern void		MPAG_ul_Modifier_Save(MDF_tdst_Modifier *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
