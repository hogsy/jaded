/*$T MDFmodifier_SPG2_Holder.h GC! 1.081 11/14/00 12:12:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_SPG2_Holder_H__
#define __MDFMODIFIER_SPG2_Holder_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"

#include <list>

#ifdef __cplusplus
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

typedef struct _XeSPG2Instances
{
    MATH_tdst_Vector    vPos;
    MATH_tdst_Vector    vX;
    MATH_tdst_Vector    vY;
    MATH_tdst_Vector    vZ;
    ULONG               ulColor;

    M_DeclareOperatorNewAndDelete();

} XeSPG2Instances;

typedef struct	SPG2Holder_tdst_Modifier_
{
    M_DeclareOperatorNewAndDelete();

    std::list<SPG2_tdst_Modifier*> st_SPG2List;

    struct	SPG2Holder_tdst_Modifier_* p_NextSPG2Holder;
    MDF_tdst_Modifier       *pParentModifier;

    int NumberOfSPG2Entries;
    XeSPG2Instances * pSPG2Instances;

} SPG2Holder_tdst_Modifier;

/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

extern void		SPG2Holder_Modifier_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *,	void *);
extern void		SPG2Holder_Modifier_Destroy(MDF_tdst_Modifier *);
extern void		SPG2Holder_Modifier_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void		SPG2Holder_Modifier_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern ULONG	SPG2Holder_Modifier_Load(MDF_tdst_Modifier *, char *);
extern void		SPG2Holder_Modifier_Save(MDF_tdst_Modifier *);
extern void		SPG2Holder_Modifier_Reinit(MDF_tdst_Modifier *);

#ifdef __cplusplus
}
#endif
#endif 
