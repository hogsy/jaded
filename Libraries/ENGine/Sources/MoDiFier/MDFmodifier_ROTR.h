/*$T MDFmodifier_XMEC.h GC! 1.100 08/29/01 14:29:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_ROTR_H__
#define __MDFMODIFIER_ROTR_H__

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
    Modifier desc
 ***********************************************************************************************************************
 */

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
typedef struct	GAO_tdst_ModifierROTR_
{
	OBJ_tdst_GameObject *pst_GO;
	int					i_1;
	float				f_RetardX;
	float				f_RetardY;
	float				f_RetardZ;
	float				f_3;
	MATH_tdst_Vector	v_Init;
} GAO_tdst_ModifierROTR;

/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

extern void		GAO_ModifierROTR_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void		GAO_ModifierROTR_Destroy(MDF_tdst_Modifier *);
extern void		GAO_ModifierROTR_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierROTR_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierROTR_Reinit(MDF_tdst_Modifier *);
extern ULONG	GAO_ModifierROTR_Load(MDF_tdst_Modifier *, char *);
extern void		GAO_ModifierROTR_Save(MDF_tdst_Modifier *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
