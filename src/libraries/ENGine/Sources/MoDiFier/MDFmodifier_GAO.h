/*$T MDFmodifier_GAO.h GC! 1.100 06/12/01 16:18:12 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_GAO_H__
#define __MDFMODIFIER_GAO_H__

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

/* for leg link modifier */
#define MFG_C_LegLink_OrientX			0x00000001
#define MFG_C_LegLink_OrientY			0x00000002
#define MFG_C_LegLink_OrientZ			0x00000004
#define MFG_C_LegLink_OrientNeg			0x00000008
#define MFG_C_LegLink_InGroup			0x00000010
#define MFG_C_LegLink_LengthInit		0x00000020
#define MFG_C_LegLink_LengthComputed	0x00000040

/*$4
 ***********************************************************************************************************************
    Modifier desc
 ***********************************************************************************************************************
 */

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    A sort of IK...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct	GAO_tdst_ModifierLegLink_
{
	void	*p_GAOA;
	void	*p_GAOC;
	float	f_AB;
	float	f_BC;
	void	*p_GaoOrient;
	ULONG	ul_Flags;
} GAO_tdst_ModifierLegLink;

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    move objet to face the camera and to be full screen
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct	GAO_tdst_ModifierSpecialLookAt_
{
	char	c_Type;
    char	c_Dummy[3];
    int     i_GaoRank;
	void	*p_GAO;
    float	f_Z;
   
    union
    {
	    float	f_Value1;
        ULONG   ul_Flags;
    };
	float	f_Value2;
} GAO_tdst_ModifierSpecialLookAt;

#define MDF_SpecialLookAt_FullScreen	0
#define MDF_SpecialLookAt_PlanetAura	1
#define MDF_SpecialLookAt_Object        2

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    explosion of triangles
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct	GAO_tdst_ModifierExplode_
{
	GEO_tdst_Object *pst_Obj;
	GEO_tdst_Object *pst_ObjSave;
	LONG			l_NbTriangles;
	LONG			l_NbPoints;
	LONG			l_NbElements;
	LONG			*dl_NbTriangles;
	float			f_Time;
	float			f_Mul;
	float			f_Grav;
} GAO_tdst_ModifierExplode;

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Infos Photos ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#define	MDF_InfoPhoto_ForceBadFrame	1
#define	MDF_InfoPhoto_ForceBadLOD	2

typedef struct	GAO_tdst_ModifierPhoto_
{
	int						i_Mission;
	int						i_Info;

	int						i_BoneForSpherePivot;		/* Sur quel GO est centre la sphere ? */
	int						i_BoneForInfoPivot;		/* Sur quel GO le "carre vert" de l'info photo est-il centre */

	MATH_tdst_Vector		st_SphereOffset;
	MATH_tdst_Vector		st_InfoOffset;

	/* LOD */
	float					f_LODMin;
	float					f_LODMax;
	float					f_CurrentLOD;

	/* Cadrage */
	float					f_FrameMin;
	float					f_FrameMax;
	float					f_CurrentFrame;

	/* Sphere Radius */
	float					f_Radius;

	ULONG					ul_Flags;

} GAO_tdst_ModifierPhoto;


/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    sauvegarde de position de matrice additionnelle
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct GAO_tdst_ModifierSaveAddMatrix_Gizmo_
{
    ULONG                       ul_Key;
    struct OBJ_tdst_GameObject_ *pst_GO;
    MATH_tdst_Matrix            *pst_Matrix;
} GAO_tdst_ModifierSaveAddMatrix_Gizmo;

typedef struct GAO_tdst_ModifierSaveAddMatrix_Slot_
{
    char             sz_Name[ 64 ];
    MATH_tdst_Matrix *dst_Mat;
} GAO_tdst_ModifierSaveAddMatrix_Slot;

typedef struct GAO_tdst_ModifierSaveAddMatrix_
{   
    char                                    c_GizmoPtr;
    char                                    c_Init;
    short                                   w_Dummy;

    LONG                                    l_NbGizmo;
    GAO_tdst_ModifierSaveAddMatrix_Gizmo    *dst_Gizmo;

    LONG                                    l_NbSlot;
    GAO_tdst_ModifierSaveAddMatrix_Slot     *dst_Slot;
} GAO_tdst_ModifierSaveAddMatrix;


/*$1
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Animated GAO
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

typedef struct tdstAnimatedGAOFlags_
{
#if defined(_GAMECUBE) || defined(_XENON)
    ULONG   Unused : 17;
    ULONG	bUsePositionOffset : 1;
    ULONG	bUseReferenceGAO : 1;
    ULONG   bSyncWithGameTime : 1;
    ULONG   bApplyToScaleZ : 1;
    ULONG	bApplyToScaleY : 1;
    ULONG	bApplyToScaleX : 1;
    ULONG	bApplyToScale : 1;
    ULONG   bApplyToTranslationZ : 1;
    ULONG	bApplyToTranslationY : 1;
    ULONG	bApplyToTranslationX : 1;
    ULONG	bApplyToTranslation : 1;
    ULONG   bApplyToRotationZ : 1;
    ULONG	bApplyToRotationY : 1;
    ULONG	bApplyToRotationX : 1;
    ULONG	bApplyToRotation : 1;
#else
    ULONG	bApplyToRotation : 1;
    ULONG	bApplyToRotationX : 1;
    ULONG	bApplyToRotationY : 1;
    ULONG   bApplyToRotationZ : 1;
    ULONG	bApplyToTranslation : 1;
    ULONG	bApplyToTranslationX : 1;
    ULONG	bApplyToTranslationY : 1;
    ULONG   bApplyToTranslationZ : 1;
    ULONG	bApplyToScale : 1;
    ULONG	bApplyToScaleX : 1;
    ULONG	bApplyToScaleY : 1;
    ULONG   bApplyToScaleZ : 1;
    ULONG   bSyncWithGameTime : 1;
    ULONG	bUseReferenceGAO : 1;
    ULONG	bUsePositionOffset : 1;
    ULONG   Unused : 17;
#endif
} tdstAnimatedGAOFlags;

typedef enum 
{
    MDF_AnimatedGAO_Linear, 
    MDF_AnimatedGAO_Noise, 
    MDF_AnimatedGAO_Sinus, 
    MDF_AnimatedGAO_NumberOfTypes,
    MDF_AnimatedGAO_Align = 0xffffffff

} MDF_AnimatedGAO_Interpolation_Types;

typedef struct	GAO_tdst_ModifierAnimatedGAOLinearParams_
{
    FLOAT				f_Min;
    FLOAT				f_Max;
    FLOAT				f_StartTime;
    FLOAT				f_StopTime;
    FLOAT				f_TotalTime;
    BOOL				b_BackAndForth;

    FLOAT				f_CurTime;

} GAO_tdst_ModifierAnimatedGAOLinearParams;

typedef struct	GAO_tdst_ModifierAnimatedGAONoiseParams_
{
    FLOAT				f_Min;
    FLOAT				f_Max;
    FLOAT				f_Var;
    FLOAT				f_VarSpeed;

} GAO_tdst_ModifierAnimatedGAONoiseParams;

typedef struct	GAO_tdst_ModifierAnimatedGAOSinusParams_
{
    FLOAT				f_TotalTime;
    FLOAT               f_Angle;
    FLOAT				f_TimeBias;
    BOOL				b_BackAndForth;

    FLOAT				f_CurTime;

} GAO_tdst_ModifierAnimatedGAOSinusParams;

typedef struct	GAO_tdst_ModifierAnimatedGAO_
{
    MATH_tdst_Matrix    st_Original;

    union 
    {
        ULONG					ulRawFlags;
        tdstAnimatedGAOFlags	ulFlags;
    };

    void				*p_GAOReference;
    MATH_tdst_Vector	vOffset;

    MDF_AnimatedGAO_Interpolation_Types eAnimationTypes[9]; // 3 for rotation, 3 for translation and 3 for scale

#ifdef ACTIVE_EDITORS
    MDF_AnimatedGAO_Interpolation_Types eLastAnimationTypes[9]; // to detect changes
#endif

    void						*pParams[9]; // x,y,z / rot,trans,scale

} GAO_tdst_ModifierAnimatedGAO;


/*$1
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Rotation paste
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

typedef struct	GAO_tdst_ModifierRotationPaste_
{
    OBJ_tdst_GameObject	*pObjectToPasteFrom;

} GAO_tdst_ModifierRotationPaste;


/*$1
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Translation paste
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

typedef struct	GAO_tdst_ModifierTranslationPaste_
{
    OBJ_tdst_GameObject	*pObjectToPasteFrom;

} GAO_tdst_ModifierTranslationPaste;


/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern BOOL		GAO_Modifier_ApplyAlways(struct OBJ_tdst_GameObject_ *, char *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void		GAO_ModifierLegLink_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void		GAO_ModifierLegLink_Destroy(MDF_tdst_Modifier *);
extern void		GAO_ModifierLegLink_Compute(MATH_tdst_Matrix *, MATH_tdst_Matrix *, MATH_tdst_Matrix *, float, float, MATH_tdst_Vector * );
extern void		GAO_ModifierLegLink_DoIt(GAO_tdst_ModifierLegLink *, OBJ_tdst_GameObject *);
extern void		GAO_ModifierLegLink_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierLegLink_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierLegLink_Reinit(MDF_tdst_Modifier *);
extern ULONG	GAO_ModifierLegLink_Load(MDF_tdst_Modifier *, char *);
extern void		GAO_ModifierLegLink_Save(MDF_tdst_Modifier *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void		GAO_ModifierSemiLookAt_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void		GAO_ModifierSemiLookAt_Destroy(MDF_tdst_Modifier *);
extern void		GAO_ModifierSemiLookAt_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierSemiLookAt_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierSemiLookAt_Reinit(MDF_tdst_Modifier *);
extern ULONG	GAO_ModifierSemiLookAt_Load(MDF_tdst_Modifier *, char *);
extern void		GAO_ModifierSemiLookAt_Save(MDF_tdst_Modifier *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void		GAO_ModifierSpecialLookAt_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void		GAO_ModifierSpecialLookAt_Destroy(MDF_tdst_Modifier *);
extern void		GAO_ModifierSpecialLookAt_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierSpecialLookAt_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierSpecialLookAt_Reinit(MDF_tdst_Modifier *);
extern ULONG	GAO_ModifierSpecialLookAt_Load(MDF_tdst_Modifier *, char *);
extern void		GAO_ModifierSpecialLookAt_Save(MDF_tdst_Modifier *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void		GAO_ModifierExplode_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void		GAO_ModifierExplode_Destroy(MDF_tdst_Modifier *);
extern void		GAO_ModifierExplode_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierExplode_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierExplode_Reinit(MDF_tdst_Modifier *);
extern ULONG	GAO_ModifierExplode_Load(MDF_tdst_Modifier *, char *);
extern void		GAO_ModifierExplode_Save(MDF_tdst_Modifier *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void		GAO_ModifierPhoto_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void		GAO_ModifierPhoto_Destroy(MDF_tdst_Modifier *);
extern void		GAO_ModifierPhoto_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierPhoto_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierPhoto_Reinit(MDF_tdst_Modifier *);
extern ULONG	GAO_ModifierPhoto_Load(MDF_tdst_Modifier *, char *);
extern void		GAO_ModifierPhoto_Save(MDF_tdst_Modifier *);


/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void		GAO_ModifierSaveAddMatrix_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void		GAO_ModifierSaveAddMatrix_Destroy(MDF_tdst_Modifier *);
extern ULONG	GAO_ul_ModifierSaveAddMatrix_Load(MDF_tdst_Modifier *, char *);
extern void		GAO_ModifierSaveAddMatrix_Save(MDF_tdst_Modifier *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void		GAO_ModifierRotationPaste_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void		GAO_ModifierRotationPaste_Destroy(MDF_tdst_Modifier *);
extern void		GAO_ModifierRotationPaste_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierRotationPaste_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierRotationPaste_Reinit(MDF_tdst_Modifier *);
extern ULONG	GAO_ModifierRotationPaste_Load(MDF_tdst_Modifier *, char *);
extern void		GAO_ModifierRotationPaste_Save(MDF_tdst_Modifier *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void		GAO_ModifierTranslationPaste_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void		GAO_ModifierTranslationPaste_Destroy(MDF_tdst_Modifier *);
extern void		GAO_ModifierTranslationPaste_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierTranslationPaste_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierTranslationPaste_Reinit(MDF_tdst_Modifier *);
extern ULONG	GAO_ModifierTranslationPaste_Load(MDF_tdst_Modifier *, char *);
extern void		GAO_ModifierTranslationPaste_Save(MDF_tdst_Modifier *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void		GAO_ModifierAnimatedGAO_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void		GAO_ModifierAnimatedGAO_Destroy(MDF_tdst_Modifier *);
extern void		GAO_ModifierAnimatedGAO_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierAnimatedGAO_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierAnimatedGAO_Reinit(MDF_tdst_Modifier *);
extern ULONG	GAO_ModifierAnimatedGAO_Load(MDF_tdst_Modifier *, char *);
extern void		GAO_ModifierAnimatedGAO_Save(MDF_tdst_Modifier *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GEOMODIFIER_H */
