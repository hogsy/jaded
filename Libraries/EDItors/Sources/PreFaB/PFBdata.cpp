/*$T PFBdata.cpp GC! 1.081 02/18/04 11:55:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "SELection/SELection.h"
#include "EDIpaths.h"

#include "PFBdata.h"


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Globals
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


int Prefab_i_LastError;


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    STRUCT functions
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 =======================================================================================================================
    free content of prefab structure
 =======================================================================================================================
 */
void Prefab_Free(OBJ_tdst_Prefab *_pst_Prefab)
{
	if(_pst_Prefab->l_NbRef != 0) L_free(_pst_Prefab->dst_Ref);
	if(_pst_Prefab->sz_Comment) L_free(_pst_Prefab->sz_Comment);
	L_memset(_pst_Prefab, 0, sizeof(OBJ_tdst_Prefab));
}

/*
 =======================================================================================================================
    free content of prefab structure and free structure too
 =======================================================================================================================
 */
void Prefab_Destroy(OBJ_tdst_Prefab **_pst_Prefab)
{
	Prefab_Free(*_pst_Prefab);
	*_pst_Prefab = NULL;
}


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    LOAD / SAVE functions
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 =======================================================================================================================
    load a prefab into given struct
 =======================================================================================================================
 */
OBJ_tdst_Prefab *Prefab_pst_Load(ULONG ul_Index, OBJ_tdst_Prefab *_pst_Prefab)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG			l_Length;
	ULONG			ul_Chunk;
	LONG			i, l_Size, l_CurRef;
	char			*pc_Buffer;
	OBJ_tdst_Prefab *P;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* bad index */
	if(ul_Index == BIG_C_InvalidIndex)
	{
		Prefab_i_LastError = -1;
		return NULL;
	}

	/* get content of file */
	pc_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(ul_Index), (ULONG *) &l_Length);

	/* check file length */
	if(l_Length == 0)
	{
		Prefab_i_LastError = -1;
		return NULL;
	}

	/* check file content */
	if(*(ULONG *) pc_Buffer != PREFABFILE_Mark)
	{
		Prefab_i_LastError = -1;
		return NULL;
	}

	pc_Buffer += 4;
	l_Length -= 4;
	l_CurRef = -1;

	/* alloc / init data */
	if(_pst_Prefab)
	{
		Prefab_Free(_pst_Prefab);
		P = _pst_Prefab;
	}
	else
	{
		P = (OBJ_tdst_Prefab *) L_malloc(sizeof(OBJ_tdst_Prefab));
		L_memset(P, 0, sizeof(OBJ_tdst_Prefab));
	}

	P->ul_Index = ul_Index;
	P->c_CurrentRef = -1;

	/* get data */
	while(l_Length > 0)
	{
		ul_Chunk = *(ULONG *) pc_Buffer;
		pc_Buffer += 4;
		l_Length -= 4;
		l_Size = ul_Chunk & 0xFFFFFF;
		ul_Chunk >>= 24;

		switch(ul_Chunk)
		{
		case PREFABFILE_Comment:
			P->sz_Comment = (char *) L_malloc(l_Size);
			L_memcpy(P->sz_Comment, pc_Buffer, l_Size);
			break;
		case PREFABFILE_Refs:
			P->l_NbRef = *(ULONG *) pc_Buffer;
			P->dst_Ref = (OBJ_tdst_PrefabRef *) L_malloc(P->l_NbRef * sizeof(OBJ_tdst_PrefabRef));
			L_memcpy(P->dst_Ref, pc_Buffer + 4, l_Size - 4);

			/* check ref */
			for(i = 0; i < P->l_NbRef; i++)
			{
				P->dst_Ref[i].ul_Index = BIG_ul_SearchKeyToFat(P->dst_Ref[i].ul_Key);
				MATH_InitVectorToZero(&P->dst_Ref[i].st_BVMin);
				MATH_InitVectorToZero(&P->dst_Ref[i].st_BVMax);
			}
			break;
		case PREFABFILE_Type:
			P->l_Type = *(ULONG *) pc_Buffer;
			break;

		case PREFABFILE_Offset:
			MATH_CopyVector(&P->st_Offset, (MATH_tdst_Vector *) pc_Buffer);
			break;

		case PREFABFILE_RefNumber:
			P->l_NbRef = *(ULONG *) pc_Buffer;
			P->dst_Ref = (OBJ_tdst_PrefabRef *) L_malloc(P->l_NbRef * sizeof(OBJ_tdst_PrefabRef));
			L_memset(P->dst_Ref, 0, P->l_NbRef * sizeof(OBJ_tdst_PrefabRef));
			break;

		case PREFABFILE_Refi:
			l_CurRef = *(ULONG *) pc_Buffer;
			if((l_CurRef < 0) || (l_CurRef >= P->l_NbRef)) break;
			P->dst_Ref[l_CurRef].ul_Key = *(ULONG *) (pc_Buffer + 4);
			P->dst_Ref[l_CurRef].ul_Index = BIG_ul_SearchKeyToFat(P->dst_Ref[l_CurRef].ul_Key);
			i = *(ULONG *) (pc_Buffer + 8);
			P->dst_Ref[l_CurRef].uc_Type = (char) (i >> 24);
			P->dst_Ref[l_CurRef].c_RandomPercent = (char) ((i >> 16) & 0xFF);
			/* check type */
			P->dst_Ref[l_CurRef].uc_Type = OBJPREFAB_C_RefIsUnknow;
			if ( P->dst_Ref[l_CurRef].ul_Index != BIG_C_InvalidIndex )
			{
				if (BIG_b_IsFileExtension(P->dst_Ref[l_CurRef].ul_Index, EDI_Csz_ExtPrefab )) 
					P->dst_Ref[l_CurRef].uc_Type = OBJPREFAB_C_RefIsPrefab;
				else if (BIG_b_IsFileExtension(P->dst_Ref[l_CurRef].ul_Index, EDI_Csz_ExtGameObject)) 
					P->dst_Ref[l_CurRef].uc_Type = OBJPREFAB_C_RefIsGao;
			}
			break;

		case PREFABFILE_RefiMatrix:
			if((l_CurRef < 0) || (l_CurRef >= P->l_NbRef)) break;
			MATH_CopyMatrix(&P->dst_Ref[l_CurRef].st_Matrix, (MATH_tdst_Matrix *) pc_Buffer);
			break;

		case PREFABFILE_RefiBV:
			if((l_CurRef < 0) || (l_CurRef >= P->l_NbRef)) break;
			MATH_CopyVector(&P->dst_Ref[l_CurRef].st_BVMin, (MATH_tdst_Vector *) pc_Buffer);
			MATH_CopyVector
			(
				&P->dst_Ref[l_CurRef].st_BVMax,
				(MATH_tdst_Vector *) (pc_Buffer + sizeof(MATH_tdst_Vector))
			);

			Prefab_CheckBV( &P->dst_Ref[l_CurRef].st_BVMin, &P->dst_Ref[l_CurRef].st_BVMax );
			break;
		}

		pc_Buffer += l_Size;
		l_Length -= l_Size;
	}

	Prefab_b_ComputeBV( P, TRUE );
	return P;
}

/*
 =======================================================================================================================
    save a prefab
 =======================================================================================================================
 */
ULONG Prefab_ul_SaveExt(ULONG _ul_DirIndex, char *_sz_Name, OBJ_tdst_Prefab *_pst_Prefab)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	char	sz_Path[260];
	ULONG	ul_Size, ul_Data;
	LONG	i;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	BIG_ComputeFullName(_ul_DirIndex, sz_Path);

	SAV_Begin(sz_Path, _sz_Name);

	/* saving mark */
	ul_Data = PREFABFILE_Mark;
	SAV_Buffer(&ul_Data, 4);

	/* saving commnent */
	if(_pst_Prefab->sz_Comment)
	{
		ul_Size = strlen(_pst_Prefab->sz_Comment) + 1;
		ul_Data = (PREFABFILE_Comment << 24) | ul_Size;
		SAV_Buffer(&ul_Data, 4);
		SAV_Buffer(_pst_Prefab->sz_Comment, ul_Size);
	}

	/* saving refs */
	if(_pst_Prefab->l_NbRef)
	{
		ul_Size = 4;
		ul_Data = (PREFABFILE_RefNumber << 24) | ul_Size;
		SAV_Buffer(&ul_Data, 4);
		SAV_Buffer(&_pst_Prefab->l_NbRef, 4);

		for(i = 0; i < _pst_Prefab->l_NbRef; i++)
		{
			/* update key */
			_pst_Prefab->dst_Ref[i].ul_Key = BIG_FileKey(_pst_Prefab->dst_Ref[i].ul_Index);

			/* save index, key and params */
			ul_Size = 12;
			ul_Data = (PREFABFILE_Refi << 24) | ul_Size;
			SAV_Buffer(&ul_Data, 4);
			SAV_Buffer(&i, 4);
			SAV_Buffer(&_pst_Prefab->dst_Ref[i].ul_Key, 4);
			ul_Data = _pst_Prefab->dst_Ref[i].uc_Type << 24;
			ul_Data |= _pst_Prefab->dst_Ref[i].c_RandomPercent << 16;
			SAV_Buffer(&ul_Data, 4);

			/* save matrix */
			ul_Size = sizeof(MATH_tdst_Matrix);
			ul_Data = (PREFABFILE_RefiMatrix << 24) | ul_Size;
			SAV_Buffer(&ul_Data, 4);
			SAV_Buffer(&_pst_Prefab->dst_Ref[i].st_Matrix, sizeof(MATH_tdst_Matrix));

			/* save bv */
			if(!MATH_b_EqVector(&_pst_Prefab->dst_Ref[i].st_BVMin, &_pst_Prefab->dst_Ref[i].st_BVMax))
			{
				Prefab_CheckBV( &_pst_Prefab->dst_Ref[i].st_BVMin, &_pst_Prefab->dst_Ref[i].st_BVMax );

				ul_Size = 2 * sizeof(MATH_tdst_Vector);
				ul_Data = (PREFABFILE_RefiBV << 24) | ul_Size;
				SAV_Buffer(&ul_Data, 4);
				SAV_Buffer(&_pst_Prefab->dst_Ref[i].st_BVMin, sizeof(MATH_tdst_Vector));
				SAV_Buffer(&_pst_Prefab->dst_Ref[i].st_BVMax, sizeof(MATH_tdst_Vector));
			}
		}
	}


	/* saving type */
	ul_Size = 4;
	ul_Data = (PREFABFILE_Type << 24) | ul_Size;
	SAV_Buffer(&ul_Data, 4);
	SAV_Buffer(&_pst_Prefab->l_Type, ul_Size);

	/* saving offset */
	ul_Size = sizeof(MATH_tdst_Vector);
	ul_Data = (PREFABFILE_Offset << 24) | ul_Size;
	SAV_Buffer(&ul_Data, 4);
	SAV_Buffer(&_pst_Prefab->st_Offset, ul_Size);
	
	/* ending saving process */
	_pst_Prefab->ul_Index = SAV_ul_End();
	return(_pst_Prefab->ul_Index);
}
/**/
ULONG Prefab_ul_Save(OBJ_tdst_Prefab *_pst_Prefab)
{
	if(!_pst_Prefab) return BIG_C_InvalidIndex;
	if(_pst_Prefab->ul_Index == BIG_C_InvalidIndex) return BIG_C_InvalidIndex;
	return Prefab_ul_SaveExt(BIG_ParentFile(_pst_Prefab->ul_Index), BIG_NameFile(_pst_Prefab->ul_Index), _pst_Prefab);
}

/*
 =======================================================================================================================
    merge new obj to existent prefab
 =======================================================================================================================
 */
void Prefab_Merge(OBJ_tdst_Prefab *_pst_Main, OBJ_tdst_Prefab *_pst_Add)
{
	/*~~~~~~~~~~~~*/
	LONG	l_Count;
	/*~~~~~~~~~~~~*/

	if(_pst_Add->l_NbRef == 0) return;

	l_Count = _pst_Main->l_NbRef + _pst_Add->l_NbRef;
	_pst_Main->dst_Ref = (OBJ_tdst_PrefabRef *) L_realloc(_pst_Main->dst_Ref, sizeof(OBJ_tdst_PrefabRef) * l_Count);

	L_memcpy
	(
		_pst_Main->dst_Ref + _pst_Main->l_NbRef,
		_pst_Add->dst_Ref,
		_pst_Add->l_NbRef * sizeof(OBJ_tdst_PrefabRef)
	);
	_pst_Main->l_NbRef = l_Count;

	Prefab_ul_Save(_pst_Main);
}

/*
 =======================================================================================================================
    create new prefab
 =======================================================================================================================
 */
void Prefab_CreateNew(ULONG _ul_DirIndex, char *_sz_Name, OBJ_tdst_Prefab *_pst_Prefab)
{
	Prefab_ul_SaveExt(_ul_DirIndex, _sz_Name, _pst_Prefab);
}

/*
 =======================================================================================================================
    Update existent prefab
 =======================================================================================================================
 */
void Prefab_Update(OBJ_tdst_Prefab *_pst_Prefab)
{
	Prefab_ul_Save(_pst_Prefab);
}


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    MISC functions
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 =======================================================================================================================
    create a prefab with all found in selection
 =======================================================================================================================
 */
BOOL Prefab_BuilFromSel_CB(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Prefab, ULONG _ul_Matrix)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_Prefab		*pst_Prefab;
	ULONG				ul_Index;
	MATH_tdst_Matrix	*pst_Matrix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;

	pst_Prefab = (OBJ_tdst_Prefab *) _ul_Prefab;
	pst_Matrix = (MATH_tdst_Matrix *) _ul_Matrix;
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO);

	if(ul_Index != BIG_C_InvalidIndex)
	{
		pst_Prefab->dst_Ref[pst_Prefab->l_NbRef].uc_Type = OBJPREFAB_C_RefIsGao;
		pst_Prefab->dst_Ref[pst_Prefab->l_NbRef].ul_Index = ul_Index;
		pst_Prefab->dst_Ref[pst_Prefab->l_NbRef].ul_Key = BIG_FileKey(ul_Index);

		/* relative matrix */
		if(pst_Prefab->l_NbRef)
		{
			MATH_MulMatrixMatrix
			(
				&pst_Prefab->dst_Ref[pst_Prefab->l_NbRef].st_Matrix,
				OBJ_pst_GetAbsoluteMatrix(pst_GO),
				pst_Matrix
			);
		}
		else
		{
			MATH_SetIdentityMatrix(&pst_Prefab->dst_Ref[pst_Prefab->l_NbRef].st_Matrix);
		}
		
		/* update BV */
		PrefabRef_b_UpdateBV( &pst_Prefab->dst_Ref[pst_Prefab->l_NbRef] );

		pst_Prefab->l_NbRef++;
	}

	return TRUE;
}
/**/
OBJ_tdst_Prefab *Prefab_pst_BuildFromSel(SEL_tdst_Selection *_pst_Sel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					l_Count;
	OBJ_tdst_Prefab		*P;
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Matrix	*M, I;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_Count = SEL_l_CountItem(_pst_Sel, SEL_C_SIF_Object);
	if(!l_Count) return NULL;

	P = (OBJ_tdst_Prefab *) L_malloc(sizeof(OBJ_tdst_Prefab));
	L_memset(P, 0, sizeof(OBJ_tdst_Prefab));

	P->l_NbRef = 0;
	P->dst_Ref = (OBJ_tdst_PrefabRef *) L_malloc(l_Count * sizeof(OBJ_tdst_PrefabRef));

	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(_pst_Sel, SEL_C_SIF_Object);
	M = OBJ_pst_GetAbsoluteMatrix(pst_GO);
	MATH_SetIdentityMatrix(&I);
	MATH_InvertMatrix(&I, M);

	SEL_EnumItem(_pst_Sel, SEL_C_SIF_Object, Prefab_BuilFromSel_CB, (ULONG) P, (ULONG) & I);
	return P;
}

/*
 =======================================================================================================================
    add a file given by it's index into a prefab
 =======================================================================================================================
 */
BOOL Prefab_b_AddRefFromFile(OBJ_tdst_Prefab *_pst_Prefab, ULONG _ul_File)
{
	/*~~~~~~~~~~~~*/
	int		iAdd;
	ULONG	ul_Size;
	/*~~~~~~~~~~~~*/

	if(_ul_File == BIG_C_InvalidIndex)
	{
		Prefab_i_LastError = PREFABERROR_BadFile;
		return FALSE;
	}

	iAdd = OBJPREFAB_C_RefIsUnknow;
	if(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtGameObject)) iAdd = OBJPREFAB_C_RefIsGao;
	if(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtPrefab)) iAdd = OBJPREFAB_C_RefIsPrefab;

	if(iAdd == OBJPREFAB_C_RefIsUnknow)
	{
		Prefab_i_LastError = PREFABERROR_BadFileType;
		return FALSE;
	}

	/* add new prefab */
	ul_Size = (_pst_Prefab->l_NbRef + 1) * sizeof(OBJ_tdst_PrefabRef);
	if(_pst_Prefab->l_NbRef)
		_pst_Prefab->dst_Ref = (OBJ_tdst_PrefabRef *) L_realloc(_pst_Prefab->dst_Ref, ul_Size);
	else
		_pst_Prefab->dst_Ref = (OBJ_tdst_PrefabRef *) L_malloc(ul_Size);

	_pst_Prefab->dst_Ref[_pst_Prefab->l_NbRef].uc_Type = (unsigned char) iAdd;
	_pst_Prefab->dst_Ref[_pst_Prefab->l_NbRef].ul_Index = _ul_File;
	_pst_Prefab->dst_Ref[_pst_Prefab->l_NbRef].ul_Key = BIG_FileKey(_ul_File);
	MATH_SetIdentityMatrix(&_pst_Prefab->dst_Ref[_pst_Prefab->l_NbRef].st_Matrix);
	
	/* update BV */
	PrefabRef_b_UpdateBV( &_pst_Prefab->dst_Ref[_pst_Prefab->l_NbRef] );
	
	_pst_Prefab->l_NbRef++;
	return TRUE;
}

/*
 =======================================================================================================================
    add content of a dir into a prefab
 =======================================================================================================================
 */
BOOL Prefab_b_AddRefFromDir(OBJ_tdst_Prefab *_pst_Prefab, ULONG _ul_Dir)
{
	/*~~~~~~~~~~~~*/
	ULONG	ul_File;
	LONG	l_Count;
	/*~~~~~~~~~~~~*/

	if(_ul_Dir == BIG_C_InvalidIndex)
	{
		Prefab_i_LastError = PREFABERROR_BadDir;
		return FALSE;
	}

	l_Count = _pst_Prefab->l_NbRef;
	ul_File = BIG_FirstFile(_ul_Dir);
	while(ul_File != BIG_C_InvalidIndex)
	{
		Prefab_b_AddRefFromFile(_pst_Prefab, ul_File);
		ul_File = BIG_NextFile(ul_File);
	}

	if(l_Count == _pst_Prefab->l_NbRef)
	{
		Prefab_i_LastError = PREFABERROR_NoFileInDir;
		return FALSE;
	}

	return TRUE;
}

/*
 =======================================================================================================================
    remove a ref in the given prefab
 =======================================================================================================================
 */
void Prefab_RemoveRef(OBJ_tdst_Prefab *_pst_Prefab, int _i_RefIndex)
{
	/*~~*/
	int i;
	/*~~*/

	if(!_pst_Prefab) return;
	if((_i_RefIndex < 0) || (_i_RefIndex >= _pst_Prefab->l_NbRef)) return;

	for(i = _i_RefIndex; i < _pst_Prefab->l_NbRef - 1; i++)
		L_memcpy(_pst_Prefab->dst_Ref + i, _pst_Prefab->dst_Ref + i + 1, sizeof(OBJ_tdst_PrefabRef));

	_pst_Prefab->l_NbRef--;
}


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    BV functions
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Prefab_CheckBV( MATH_tdst_Vector *Min, MATH_tdst_Vector *Max )
{
	/*~~~~~~~~~~~*/
	float	f_Swap;
	/*~~~~~~~~~~~*/

	if(Min->x > Max->x)
	{
		f_Swap = Min->x;
		Min->x = Max->x;
		Max->x = f_Swap;
	}

	if(Min->y > Max->y)
	{
		f_Swap = Min->y;
		Min->y = Max->y;
		Max->y = f_Swap;
	}

	if(Min->z > Max->z)
	{
		f_Swap = Min->z;
		Min->z = Max->z;
		Max->z = f_Swap;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL PrefabRef_b_UpdateBV(OBJ_tdst_PrefabRef *PR)
{
	MATH_tdst_Vector Min, Max;
	OBJ_tdst_Prefab		*pst_Prefab;
	
	if ( PR->uc_Type == OBJPREFAB_C_RefIsGao )
	{
		if ( !OBJ_b_AllGao_GetBV( PR->ul_Index, &Min, &Max ) )
			return FALSE;
	}
	else if (PR->uc_Type == OBJPREFAB_C_RefIsPrefab )
	{
		pst_Prefab = Prefab_pst_Load( PR->ul_Index, NULL );
		if (!pst_Prefab) return FALSE;
		
		if ( !Prefab_b_ComputeBV( pst_Prefab, FALSE ) )
			return FALSE;
			
		MATH_CopyVector( &Min, &pst_Prefab->st_BVMin );
		MATH_CopyVector( &Max, &pst_Prefab->st_BVMax );
	}
	else
		return FALSE;
		
	if ( MATH_b_EqVector( &PR->st_BVMin, &Min) && MATH_b_EqVector( &PR->st_BVMax, &Max ) )
		return FALSE;

	MATH_CopyVector( &PR->st_BVMin, &Min );
	MATH_CopyVector( &PR->st_BVMax, &Max );
	Prefab_CheckBV( &PR->st_BVMin, &PR->st_BVMax );
	
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL Prefab_b_ComputeBV( OBJ_tdst_Prefab *P, BOOL _b_Next )
{
	int					ref, point;
	MATH_tdst_Vector	V;
	
	if (!P || !P->dst_Ref )
		return FALSE;
		
	switch ( P->l_Type )
	{
	case OBJPREFAB_C_AllRef:
		{
			MATH_CopyVector( &P->st_BVMin, &P->dst_Ref[ 0 ].st_BVMin );
			MATH_CopyVector( &P->st_BVMax, &P->dst_Ref[ 0 ].st_BVMax );
			for ( ref = 1; ref < P->l_NbRef; ref++ )
			{
				for (point = 0; point < 8; point ++ )
				{
					V.x = (( point & 1 ) ? P->dst_Ref[ ref ].st_BVMin : P->dst_Ref[ ref ].st_BVMax).x;
					V.y = (( point & 2 ) ? P->dst_Ref[ ref ].st_BVMin : P->dst_Ref[ ref ].st_BVMax).y;
					V.z = (( point & 4 ) ? P->dst_Ref[ ref ].st_BVMin : P->dst_Ref[ ref ].st_BVMax).z;
					MATH_TransformVertex( &V, &P->dst_Ref[ ref ].st_Matrix, &V);
					
					if(V.x < P->st_BVMin.x) P->st_BVMin.x = V.x;
					if(V.y < P->st_BVMin.y) P->st_BVMin.y = V.y;
					if(V.z < P->st_BVMin.z) P->st_BVMin.z = V.z;

					if(V.x > P->st_BVMax.x) P->st_BVMax.x = V.x;
					if(V.y > P->st_BVMax.y) P->st_BVMax.y = V.y;
					if(V.z > P->st_BVMax.z) P->st_BVMax.z = V.z;
				}
			}
		}	
		break;
	
	case OBJPREFAB_C_Cycle:
		if ( _b_Next )
			P->c_CurrentRef++;
		if ( (P->c_CurrentRef < 0) || (P->c_CurrentRef >= P->l_NbRef ) )
			P->c_CurrentRef = 0;
		MATH_CopyVector( &P->st_BVMin, &P->dst_Ref[ P->c_CurrentRef ].st_BVMin );
		MATH_CopyVector( &P->st_BVMax, &P->dst_Ref[ P->c_CurrentRef ].st_BVMax );
		break;
		
	case OBJPREFAB_C_Random:
		if ( ( _b_Next ) || (P->c_CurrentRef < 0) || (P->c_CurrentRef >= P->l_NbRef ) )
			P->c_CurrentRef = (char) (rand() % P->l_NbRef);
		MATH_CopyVector( &P->st_BVMin, &P->dst_Ref[ P->c_CurrentRef ].st_BVMin );
		MATH_CopyVector( &P->st_BVMax, &P->dst_Ref[ P->c_CurrentRef ].st_BVMax );
		break;
		
	case OBJPREFAB_C_RandomUser:
		break;
	}
	
	MATH_AddEqualVector( &P->st_BVMin, &P->st_Offset );
	MATH_AddEqualVector( &P->st_BVMax, &P->st_Offset );
	
	Prefab_CheckBV( &P->st_BVMin, &P->st_BVMax );
	return TRUE;
}

#endif /* ACTIVE_EDITORS */
