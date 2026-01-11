#ifndef __MDFMODIFIER_Vine_H__
#define __MDFMODIFIER_Vine_H__

#define Vine_C_MaxIteration  3
#define Vine_C_MaxColliders  128
#define Vine_C_PausedTreshold 0.001f
typedef struct	GAO_tdst_VineVertex_
{    
    OBJ_tdst_GameObject         *m_pBone;
    MATH_tdst_Vector            m_CurrPos;            
    MATH_tdst_Vector            m_PrevPos;
    MATH_tdst_Vector            m_Accel;
    struct GAO_tdst_VineVertex_ *m_pFather;
    struct GAO_tdst_VineVertex_ *m_pChildren;
    float                       fDistanceWithFather;
} GAO_tdst_VineVertex;


typedef struct	VINE_tdst_Modifier_
{
    UINT                  bFirstFrame;
    GAO_tdst_VineVertex*  a_VineVetices;
    ULONG                 ul_NbVertices;
	OBJ_tdst_GameObject*  pst_GO;
    GAO_tdst_VineVertex*  p_HookVertices;
    GAO_tdst_VineVertex*  p_FirstVertices;
	OBJ_tdst_GameObject** pst_Colliders;    
    float                 fPrevTime;
    float                 fCurrentTime;
    ULONG                 ul_Paused;
    ULONG                 ul_BrokenVines;
#ifdef JADEFUSION
    ULONG                 ul_NbCollided;
#endif

    // Configuration
    float                 fDampeningFactor;
    float                 fGravity;
    float                 fForceApplied;
#ifdef JADEFUSION
    float                 fMaxForce;
#endif
	ULONG                 ul_ChildIsAlsoHook;
    ULONG                 ul_ChildHookBrokeOnContact;
} VINE_tdst_Modifier;


extern void VINE_Modifier_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void VINE_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod);
extern void VINE_Modifier_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
extern void VINE_Modifier_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
extern ULONG VINE_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer);
extern void VINE_Modifier_Reinit(MDF_tdst_Modifier *_pst_Mod);

#ifdef ACTIVE_EDITORS
extern BOOL VINE_Modifier_CanBeApply(OBJ_tdst_GameObject *_pst_GO, char *_psz_Msg);
extern void VINE_Modifier_Save(MDF_tdst_Modifier *_pst_Mod);
#endif

void MDFModifier_Vine_Solver(VINE_tdst_Modifier * _p_Vine, FLOAT _f_DT);
void VINE_Modifier_Normalize(VINE_tdst_Modifier * _p_Vine);
void MDFModifier_Vine_ApplyForces(VINE_tdst_Modifier * _p_Vine);

#endif //__MDFMODIFIER_Vine_H__
