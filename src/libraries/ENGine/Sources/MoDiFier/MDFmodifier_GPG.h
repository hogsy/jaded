/*$T MDFmodifier_GEO.h GC! 1.081 11/14/00 12:12:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_GPG_H__
#define __MDFMODIFIER_GPG_H__

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

#define GPG_MaxGeom			5
#define GPG_MaxLOD			3
#define GPG_MaxRenderInst	50
#define	GPG_MaxPerturbators	10

/* Flags globaux */
#define MOD_GPG_Validate	0x00000001

/* Flags geom */
#define MOD_GPG_DontPerturb 0x00000001

/*$4
 ***********************************************************************************************************************
    Modifier desc
 ***********************************************************************************************************************
 */

typedef struct  GPG_tdst_RendererInst_
{
	MATH_tdst_Matrix	M;
} GPG_tdst_RendererInst;

typedef struct  GPG_tdst_Renderer_
{
	GPG_tdst_RendererInst	t_Inst[GPG_MaxRenderInst];
	int						i_Num;
	GEO_tdst_Object			*pt_Geom;
} GPG_tdst_Renderer;

typedef struct	GPG_tdst_GeomLOD_
{
	GEO_tdst_Object			*pt_Geom;
	float					f_LOD;
} GPG_tdst_GeomLOD;

typedef struct	GPG_tdst_Geom_
{
	GPG_tdst_GeomLOD		t_LOD[GPG_MaxLOD];
	float					f_RandV;
	float					f_RandP;
	float					f_PertFactor;
	float					f_ZoomMin;
	float					f_ZoomMax;
	ULONG					ul_Flags;
} GPG_tdst_Geom;

typedef struct	GPG_tdst_Modifier_
{
	ULONG					ul_Flags;
	GPG_tdst_Geom			apt_Geoms[GPG_MaxGeom];
} GPG_tdst_Modifier;

typedef struct	GPG_tdst_Perturbator_
{
	OBJ_tdst_GameObject *pt_GAO;
	float				f_Size;
} GPG_tdst_Perturbator;


/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

extern void							GPG_Modifier_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void							GPG_Modifier_Destroy(MDF_tdst_Modifier *);
extern void							GPG_Modifier_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void							GPG_Modifier_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void							GPG_Modifier_Reinit(MDF_tdst_Modifier *);
extern ULONG						GPG_ul_Modifier_Load(MDF_tdst_Modifier *, char *);
extern void							GPG_Modifier_Save(MDF_tdst_Modifier *);
extern void							GPG_Reset(void);


#ifdef JADEFUSION
extern __declspec(align(32)) GPG_tdst_Perturbator GPG_gpt_Perturbators[GPG_MaxPerturbators];
#else
extern GPG_tdst_Perturbator			GPG_gpt_Perturbators[GPG_MaxPerturbators];
#endif

extern int							GPG_gi_NumPerturbators;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GEOMODIFIER_H */
