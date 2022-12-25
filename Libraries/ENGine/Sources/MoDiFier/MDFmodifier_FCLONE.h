#ifndef __MDFMODIFIER_FCLONE_H__
#define __MDFMODIFIER_FCLONE_H__

//flag
#define	CLONE_BVReject							0x00000001
#define CLONE_Cull								0x00000002
#define CLONE_ShowGroup							0x00000004

#define MAXCLONE	256
#define MAXLIGHTPERCLONE	8

//#include "engine/MoDiFier/MDFstruct.h"

/*typedef struct CLONE_Data_
{
MATH_tdst_Vector vPos;//12b
MATH_tdst_Quaternion	qQ;//16b
float zoom; //4b
} ONLY_XBOX_ALIGNED(32) CLONE_Data ONLY_PSX2_ALIGNED(32);*/

typedef struct CLONE_Data_
{
	//OBJ_tdst_GameObject *Gao;
	void *Gao;
}CLONE_Data;

/*typedef struct CLONE_ElementDescriptor_
{
//bool *b_CloneCulled;
//MATH_tdst_Matrix *m_CloneMatrix;
//	CLONE_ElementDescriptor* next;
CLONE_Data *cl_eData;

} CLONE_ElementDescriptor;*/

typedef struct	FCLONE_tdst_Modifier_
{

	ULONG				NBR_Instances;
	CLONE_Data *cl_eData;
	ULONG ulFlags;
#ifdef ACTIVE_EDITORS
	ULONG				NBR_SavInstances;
#endif
	//CLONE_ElementDescriptor Cl_ElementDescriptor;

} FCLONE_tdst_Modifier;
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

	extern void FCLONE_Modifier_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
	extern void FCLONE_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod);
	extern void FCLONE_Modifier_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
	extern void FCLONE_Modifier_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
	extern ULONG FCLONE_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer);

	extern void FCLONE_Modifier_Update(MDF_tdst_Modifier *_pst_Mod );

#endif
#ifdef ACTIVE_EDITORS
	extern BOOL FCLONE_Modifier_CanBeApply(OBJ_tdst_GameObject *_pst_GO, char *_psz_Msg);
	extern void FCLONE_Modifier_Save(MDF_tdst_Modifier *_pst_Mod);
	extern int FCLONE_Modifier_CurrentSelection();
#endif


	/*extern int					NbrFurLayers ;
	extern float				OffsetNormal ;
	extern float				OffsetU ;
	extern float				OffsetV ;*/


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif