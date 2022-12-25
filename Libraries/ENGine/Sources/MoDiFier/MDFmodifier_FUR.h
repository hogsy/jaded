
typedef struct	FUR_tdst_Modifier_
{
	float				f_NormalOffset;
	float				f_UOffset;
	float				f_VOffset;
	ULONG				NBR_Layers;
	ULONG				ul_Flags;
	ULONG				LineColor;
	int					uc_NearLod;
	float				f_Near,f_Far;
} FUR_tdst_Modifier;

#define FUR_C_OUTLINE				0x00000001
#define FUR_C_CONST					0x00000002
#define FUR_C_DONTDRAWLAYER0		0x00000004

extern void FUR_Modifier_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void FUR_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod);
extern void FUR_Modifier_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
extern void FUR_Modifier_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
extern ULONG FUR_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer);

#ifdef ACTIVE_EDITORS
extern BOOL FUR_Modifier_CanBeApply(OBJ_tdst_GameObject *_pst_GO, char *_psz_Msg);
extern void FUR_Modifier_Save(MDF_tdst_Modifier *_pst_Mod);
#endif


extern int					NbrFurLayers ;
extern float				OffsetNormal ;
extern float				OffsetU ;
extern float				OffsetV ;

#ifdef _XENON_RENDER

typedef struct	DYNFUR_tdst_Modifier_
{
    ULONG               ul_Version;
    float				f_FurLength;
    float				f_UOffset;
    float				f_VOffset;
    ULONG				NBR_Layers;
    float               f_Gravity;
    float               f_GlobalLinVelBoost;
    float               f_GlobalAngVelBoost;
    float               f_LocalAngVelBoost;
    float               f_Damping;
	float				f_Near,f_Far;

    // Xenon
    D3DXQUATERNION      *m_pavLastBonesRotation;
    D3DXVECTOR4         *m_pavBonesVelocity;
    D3DXVECTOR4         *m_pavBonesAccel;
    D3DXVECTOR4         m_vAccel;
    D3DXVECTOR4         m_vPrevVel;
    D3DXVECTOR4         m_vOmegaAccel;
    D3DXVECTOR4         m_vPrevOmega;
    D3DXVECTOR4         m_vTransDelta;
    D3DXVECTOR4         m_vLastPosition;
    D3DXQUATERNION      m_qLastRot;


	

} DYNFUR_tdst_Modifier;

extern void DYNFUR_Modifier_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void DYNFUR_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod);
extern void DYNFUR_Modifier_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
extern void DYNFUR_Modifier_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
extern ULONG DYNFUR_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer);
extern void	DYNFUR_Modifier_Reinit(MDF_tdst_Modifier *);

#ifdef ACTIVE_EDITORS
extern BOOL DYNFUR_Modifier_CanBeApply(OBJ_tdst_GameObject *_pst_GO, char *_psz_Msg);
extern void DYNFUR_Modifier_Save(MDF_tdst_Modifier *_pst_Mod);
#endif

extern float  				FurLength ;

#endif

