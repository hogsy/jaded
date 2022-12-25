/*$T PFBdata.h GC! 1.081 02/02/04 10:49:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    contants
 ***********************************************************************************************************************
 */

/* constant for type of a ref */
#define OBJPREFAB_C_RefIsGao	((unsigned char) 0)
#define OBJPREFAB_C_RefIsPrefab ((unsigned char) 1)
#define OBJPREFAB_C_RefIsUnknow ((unsigned char) 255)

/* constant for type of prefab generation */
#define OBJPREFAB_C_AllRef		0
#define OBJPREFAB_C_Cycle		1
#define OBJPREFAB_C_Random		2
#define OBJPREFAB_C_RandomUser	3

/* saving/loading constants */
#define PREFABFILE_Mark		0x5046422E
#define PREFABFILE_Mark1	0x5046422F

#define PREFABFILE_Comment		1
#define PREFABFILE_Refs			2
#define PREFABFILE_Type			3
#define PREFABFILE_Offset		4
#define PREFABFILE_RefNumber	5
#define PREFABFILE_Refi			6
#define PREFABFILE_RefiMatrix	7
#define PREFABFILE_RefiBV		8

/* error constants */

#define PREFABERROR_BadFile		1
#define PREFABERROR_BadDir		2
#define PREFABERROR_BadFileType	3
#define PREFABERROR_NoFileInDir	4

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */
extern int Prefab_i_LastError;

/*$4
 ***********************************************************************************************************************
    structure
 ***********************************************************************************************************************
 */

/*
 -----------------------------------------------------------------------------------------------------------------------
    prefab data : one ref
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	OBJ_tdst_PrefabRef_
{
	ULONG				ul_Key;
	ULONG				ul_Index;

	unsigned char		uc_Type; /* gao or prefab */
	char				c_RandomPercent;
	char				c_IsActive;
	char				c_Dummy;

	MATH_tdst_Matrix	st_Matrix;
	MATH_tdst_Vector	st_BVMin;
	MATH_tdst_Vector	st_BVMax;
} OBJ_tdst_PrefabRef;

/*
 -----------------------------------------------------------------------------------------------------------------------
    prefab data : file index, comment, list of obj, type, ...
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	OBJ_tdst_Prefab_
{
	ULONG				ul_Index;
	char				*sz_Comment;	/* comment about this prefab */
	long				l_Type;			/* how to duplicate this prefab : all ref, cycle ref, random ref */
	
	MATH_tdst_Vector	st_Offset;		/* offset of object when duplicating */
	long				l_NbRef;		/* number of game object in this prefab */
	OBJ_tdst_PrefabRef	*dst_Ref;		/* list of ref in this prefab */
	
	char				c_CurrentRef;
	char				c_Show;
	char				c_Dummy2;
	char				c_Dummy3;
	
	MATH_tdst_Vector	st_BVMin;
	MATH_tdst_Vector	st_BVMax;
	
	float				f_Rotate;
	
} OBJ_tdst_Prefab;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

void			Prefab_Free(OBJ_tdst_Prefab *);
void			Prefab_Destroy(OBJ_tdst_Prefab **);

OBJ_tdst_Prefab *Prefab_pst_Load(ULONG, OBJ_tdst_Prefab *);
ULONG			Prefab_ul_SaveExt(ULONG, char *, OBJ_tdst_Prefab *);
ULONG			Prefab_ul_Save(OBJ_tdst_Prefab *);

void			Prefab_Merge(OBJ_tdst_Prefab *, OBJ_tdst_Prefab *);
void			Prefab_CreateNew(ULONG, char *, OBJ_tdst_Prefab *);
void			Prefab_Update(OBJ_tdst_Prefab *);

OBJ_tdst_Prefab *Prefab_pst_BuildFromSel(struct SEL_tdst_Selection_ *);
BOOL			Prefab_b_AddRefFromFile( OBJ_tdst_Prefab *, ULONG );
BOOL			Prefab_b_AddRefFromDir( OBJ_tdst_Prefab *, ULONG );
void			Prefab_RemoveRef( OBJ_tdst_Prefab *, int );

BOOL			PrefabRef_b_UpdateBV( OBJ_tdst_PrefabRef * );
BOOL			Prefab_b_ComputeBV( OBJ_tdst_Prefab *, BOOL );
void			Prefab_CheckBV( MATH_tdst_Vector *, MATH_tdst_Vector * );

#endif /* ACTIVE_EDITORS */
