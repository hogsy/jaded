#ifndef __MDFMODIFIER_ODE_H__
#define __MDFMODIFIER_ODE_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"
#ifdef ODE_INSIDE
#include "ode_jade.h"
#endif // ODE_INSIDE

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

typedef struct GAO_tdst_ModifierODE_
{

	UCHAR					uc_Version;
	UCHAR					uc_Type;		/* Type of Joint ODE */

	USHORT					uw_Dummy;

	OBJ_tdst_GameObject		*pst_GO1;
	OBJ_tdst_GameObject		*pst_GO2;

	MATH_tdst_Vector		st_Axis;
	MATH_tdst_Vector		st_Anchor;

	float					f_LoLimit;
	float					f_HiLimit;
	float					f_BounceStop;
	float					f_Friction;

#ifdef ODE_INSIDE
	dJointID				i_ode_joint_id;
#endif // ODE_INSIDE

#ifdef ACTIVE_EDITORS
	OBJ_tdst_GameObject		*pst_GO;
#endif

} GAO_tdst_ModifierODE;


/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern void GAO_ModifierODE_Create(OBJ_tdst_GameObject *, MDF_tdst_Modifier *, void *);
extern void GAO_ModifierODE_Destroy(MDF_tdst_Modifier *);
extern void GAO_ModifierODE_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void GAO_ModifierODE_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void GAO_ModifierODE_Reinit(MDF_tdst_Modifier *);
extern ULONG GAO_ModifierODE_Load(MDF_tdst_Modifier *, char *);
#ifdef ACTIVE_EDITORS
extern void GAO_ModifierODE_Save(MDF_tdst_Modifier *);
#endif



#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif

