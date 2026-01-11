#ifndef __ODE_JADE__
#define __ODE_JADE__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

//#define dSINGLE

#include "BASe/BAStypes.h"
#include "MATHs/MATHstruct.h"
#include "ode/ode.h"


#define	 ODE_FLAGS_MATHCOLMAP			0x01		/* Objects that have a mathematical ColMap */
#define	 ODE_FLAGS_RIGIDBODY			0x02		/* Objects that can collide in ODE */
#define	 ODE_FLAGS_ENABLE				0x04		/* ODE must be computed (except ode internal auto disable objects) */
#define	 ODE_FLAGS_AUTODISABLE			0x08		/* Auto Disable Flag set */
#define	 ODE_FLAGS_PAUSEINIT			0x10		/* This GO is already disabled at the init */
#define	 ODE_FLAGS_FORCEIMMOVABLE		0x20		/* This GO may have an active Rigid Body. We set it as immovable for the time being */
#define	 ODE_FLAGS_NO_GRAVITY			0x40		/* This GO is not influenced by ODE gravity */


#define ODE_TYPE_SPHERE					1
#define ODE_TYPE_BOX					2
#define ODE_TYPE_CYLINDER				3

#define	 ODE_JointTypeBall				1
#define	 ODE_JointTypeHinge				2
#define	 ODE_JointTypeSlider				3
#define	 ODE_JointTypeContact			4
#define	 ODE_JointTypeUniversal			5
#define	 ODE_JointTypeHinge2				6
#define	 ODE_JointTypeFixed				7
#define	 ODE_JointTypeNull				8
#define	 ODE_JointTypeAMotor				9


#define ODE_TYPE_PLAN			4

typedef struct DYN_tdst_ODE_
{
#ifdef ACTIVE_EDITORS
	struct OBJ_tdst_GameObject_		*pst_GO;
#endif

	float					f_X;
	float					f_Y;
	float					f_Z;

	UCHAR					uc_Version;					/* Version to update the load/save ode process */
	UCHAR					uc_Type;					/* Type of ODE Primitive */ 
	UCHAR					uc_Flags;					/* Flags ODE */ 
	UCHAR					uc_Sound;					

	MATH_tdst_Vector		st_Offset;					/* Rigid Body Sphere/Box are not centred on (0,0,0) of the object */

	MATH_tdst_Matrix		st_RotMatrix;

	dGeomID					ode_id_geom;				/* ID of Geometric Body for ODE */
	dBodyID					ode_id_body;				/* ID of Rigid Body for ODE */

	void					*p_User;


	float					f_LinearThres;
	float					f_AngularThres;

	/* Surface/ ColMap Parameters*/
	int						SurfaceMode;
	float					mu;
	float					mu2;
	float					bounce;
	float					bounce_vel;
	float					soft_erp;
	float					soft_cfm;
	float					motion1;
	float					motion2;
	float					slip1;
	float					slip2;

	/* valeurs init */
	float					mass_init;
	float					mu_init;
	float					bounce_init;

} DYN_tdst_ODE;

extern void ODE_ComputeODEMatrixFromJade(MATH_tdst_Matrix *, MATH_tdst_Matrix *, float *);
extern void ODE_ComputeJadeMatrixFromODE(MATH_tdst_Matrix *, MATH_tdst_Matrix *, float *);
extern void ODE_MainCall(struct WOR_tdst_World_ *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


#endif