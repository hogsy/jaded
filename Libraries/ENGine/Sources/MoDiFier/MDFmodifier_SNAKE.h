/*$T MDFmodifier_XMEC.h GC! 1.100 08/29/01 14:29:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_SNAKE_H__
#define __MDFMODIFIER_SNAKE_H__

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

#define SNAKE_MXBONES	20
#define SNAKE_TAKEY		0x00000001

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
typedef struct	GAO_tdst_ModifierSNAKE_
{
	OBJ_tdst_GameObject *pst_GO[SNAKE_MXBONES];
	OBJ_tdst_GameObject	*pst_Followed;
	float				af_GODist[SNAKE_MXBONES];
	int					i_NumBones;
	int					ai_Bones[SNAKE_MXBONES];
	float				f_Inertie;
	float				f_BlendDist;
	float				f_Attenuation;
	float				f_TimeLeft;

	/* Snake 2 Philippe */
	int					ul_Flags;
	float DeltaT;
	MATH_tdst_Vector	QuaternionSpeed[SNAKE_MXBONES];
} GAO_tdst_ModifierSNAKE;

/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

extern void		GAO_ModifierSNAKE_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void		GAO_ModifierSNAKE_Destroy(MDF_tdst_Modifier *);
extern void		GAO_ModifierSNAKE_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierSNAKE_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierSNAKE_Reinit(MDF_tdst_Modifier *);
extern ULONG	GAO_ModifierSNAKE_Load(MDF_tdst_Modifier *, char *);
extern void		GAO_ModifierSNAKE_Save(MDF_tdst_Modifier *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
