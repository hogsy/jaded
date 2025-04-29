/*$T MATmulti.c GC! 1.100 08/24/01 14:49:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"
#ifndef PSX2_TARGET
#include "MAD_mem/Sources/MAD_mem.h"
#endif
#include "MATerial/MATmulti.h"
#include "GRObject/GROstruct.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "GEOmetric/GEOload.h"
#ifdef JADEFUSION
#include "DIAlogs/DIAname_dlg.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_Validate_Multi(MAT_tdst_Multi *pst_MMat)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				Counter;
	ULONG				BigAnd;
	MAT_tdst_Material	**pst_Bgin;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern u32 UpperNumberOFSubMaterial;
	
	Counter = pst_MMat->l_NumberOfSubMaterials;
	pst_Bgin = pst_MMat->dpst_SubMaterial;
	pst_MMat->ul_ValidateMask = 0;
	BigAnd = 0xffffffff;
	while(Counter--)
	{
		/*~~~~~~~~~*/
		ULONG	MASK;
		/*~~~~~~~~~*/
		
		UpperNumberOFSubMaterial = pst_MMat->l_NumberOfSubMaterials;

		if(*pst_Bgin)
		{
			if((*pst_Bgin)->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
				MAT_Validate_Multitexture((MAT_tdst_MultiTexture *) (*pst_Bgin));
			if((*pst_Bgin)->st_Id.i->ul_Type == GRO_MaterialSingle)
				MAT_Validate_Single((MAT_tdst_Single *) (*pst_Bgin));

			MASK = (*pst_Bgin)->st_Id.i->pfn_User_0((GRO_tdst_Struct *) (*pst_Bgin), 0);
			BigAnd &= MASK;
			pst_MMat->ul_ValidateMask |= MASK;
		}

		pst_Bgin++;
	}

	pst_MMat->ul_ValidateMask &= ~MAT_ValidateMask_Transparency_And;
	pst_MMat->ul_ValidateMask |= BigAnd & MAT_ValidateMask_Transparency_And;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_AllocMultiMaterial(MAT_tdst_Material *_pst_Material, LONG _l_NbSubMat)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Multi	*pst_MultiMat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Material->st_Id.i->ul_Type != GRO_MaterialMulti) return;

    if (_l_NbSubMat > 255)
        _l_NbSubMat = 0;

	pst_MultiMat = (MAT_tdst_Multi *) _pst_Material;
	pst_MultiMat->l_NumberOfSubMaterials = _l_NbSubMat;
	if(!_l_NbSubMat) 
		pst_MultiMat->dpst_SubMaterial = NULL;
	else
		pst_MultiMat->dpst_SubMaterial = (MAT_tdst_Material **) MEM_p_Alloc(sizeof(MAT_tdst_Material *) * _l_NbSubMat);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_FreeMultiMaterial(MAT_tdst_Material *_pst_Material)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Multi		*pst_MultiMat;
	MAT_tdst_Material	**ppst_SubMat, **ppst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Material->st_Id.i->ul_Type != GRO_MaterialMulti) return;

	pst_MultiMat = (MAT_tdst_Multi *) _pst_Material;
	if(pst_MultiMat->l_NumberOfSubMaterials)
	{
		ppst_SubMat = pst_MultiMat->dpst_SubMaterial;
		ppst_Last = ppst_SubMat + pst_MultiMat->l_NumberOfSubMaterials;
		for(; ppst_SubMat < ppst_Last; ppst_SubMat++)
		{
			if(!(*ppst_SubMat)) continue;
			(*ppst_SubMat)->st_Id.i->pfn_AddRef((*ppst_SubMat), -1);
            // Destroyed afterwards by table.
			//(*ppst_SubMat)->st_Id.i->pfn_Destroy((*ppst_SubMat));
		}

		MEM_Free(pst_MultiMat->dpst_SubMaterial);
		pst_MultiMat->l_NumberOfSubMaterials = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAT_tdst_Multi *MAT_pst_CreateMultiFromBuffer(GRO_tdst_Struct *_pst_Id, char **ppc_Buffer, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Multi	*pst_Material;
	LONG			j;
	BIG_KEY			ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	WOR_gpst_WorldToLoadIn = _pst_World;

	pst_Material = (MAT_tdst_Multi *) MEM_p_Alloc(sizeof(MAT_tdst_Multi));
	L_memset(pst_Material, 0, sizeof(MAT_tdst_Multi));

	GRO_Struct_Init(&pst_Material->st_Id, _pst_Id->i->ul_Type);

	MAT_AllocMultiMaterial((MAT_tdst_Material *) pst_Material, LOA_ReadULong(ppc_Buffer));

	/* Add reference to single material of multi material */
	for(j = 0; j < pst_Material->l_NumberOfSubMaterials; j++)
	{
		ul_Key = LOA_ReadULong(ppc_Buffer);
		if(ul_Key == BIG_C_InvalidKey)
		{
			pst_Material->dpst_SubMaterial[j] = NULL;	/* pst_Material->dpst_SubMaterial[j - 1]; */
		}
		else
		{
			LOA_MakeFileRef
			(
				ul_Key,
				(ULONG *) &pst_Material->dpst_SubMaterial[j],
				GEO_ul_Load_ObjectCallback,
				LOA_C_MustExists
			);
		}
	}

	return pst_Material;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAT_tdst_Multi *MAT_pst_CreateMulti(char *_sz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Multi	*pst_Material;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Material = (MAT_tdst_Multi *) MEM_p_Alloc(sizeof(MAT_tdst_Multi));
	L_memset(pst_Material, 0, sizeof(MAT_tdst_Multi));

	GRO_Struct_Init(&pst_Material->st_Id, GRO_MaterialMulti);
	GRO_Struct_SetName(&pst_Material->st_Id, _sz_Name);
	return pst_Material;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MAT_p_CreateMultiDefault(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Multi	*pst_Material;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Material = MAT_pst_CreateMulti(NULL);

	/* add an invalid material to avoid further bug in material edition */
	MAT_AllocMultiMaterial((MAT_tdst_Material *) pst_Material, 1);
	pst_Material->dpst_SubMaterial[0] = 0;

	return (void *) pst_Material;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG MAT_l_SaveMultiInBuffer(MAT_tdst_Multi *_pst_Mat, void *p_Unused)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
	MAT_tdst_Material	**ppst_Mat, **ppst_LastMat;
	BIG_KEY				ul_FileKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GRO_Struct_Save(&_pst_Mat->st_Id);

	SAV_Buffer(&_pst_Mat->l_NumberOfSubMaterials, 4);

	ppst_Mat = _pst_Mat->dpst_SubMaterial;
	ppst_LastMat = ppst_Mat + _pst_Mat->l_NumberOfSubMaterials;

	for(; ppst_Mat < ppst_LastMat; ppst_Mat++)
	{
		if(*ppst_Mat)
			ul_FileKey = LOA_ul_SearchKeyWithAddress((ULONG) * ppst_Mat);
		else
			ul_FileKey = 0xFFFFFFFF;
		SAV_Buffer(&ul_FileKey, sizeof(BIG_KEY));
	}

#endif
	return 0;
}

#ifndef PSX2_TARGET
#ifdef ACTIVE_EDITORS
/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAT_tdst_Multi *MAT_p_CreateMultiFromMad(MAD_MultiMaterial *_pst_MadMat)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Multi	*pst_MultiMat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_MultiMat = (MAT_tdst_Multi *) MAT_pst_CreateMaterial(GRO_MaterialMulti, _pst_MadMat->MatRef.Name);
	MAT_AllocMultiMaterial((MAT_tdst_Material *) pst_MultiMat, _pst_MadMat->NumberOfSubMaterials);
	L_memcpy(pst_MultiMat->dpst_SubMaterial, _pst_MadMat->SubMats, sizeof(LONG) * _pst_MadMat->NumberOfSubMaterials);

	return pst_MultiMat;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MAD_MultiMaterial *MAT_p_MultiToMad(MAT_tdst_Multi *_pst_Mat, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
	MAD_MultiMaterial	*pst_MadMat;
	LONG				l_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MAD_MALLOC(MAD_MultiMaterial, pst_MadMat, 1);

	pst_MadMat->MatRef.MaterialType = ID_MAT_MultiMaterial;
	L_strcpy(pst_MadMat->MatRef.Name, GRO_sz_Struct_GetName(&_pst_Mat->st_Id));
	pst_MadMat->NumberOfSubMaterials = _pst_Mat->l_NumberOfSubMaterials;
	if(pst_MadMat->NumberOfSubMaterials)
	{
		MAD_MALLOC(ULONG, pst_MadMat->SubMats, pst_MadMat->NumberOfSubMaterials);
		for(l_Index = 0; l_Index < (LONG) pst_MadMat->NumberOfSubMaterials; l_Index++)
		{
			pst_MadMat->SubMats[l_Index] = TAB_ul_Ptable_GetElemIndexWithPointer
				(
					&_pst_World->st_GraphicMaterialsTable,
					_pst_Mat->dpst_SubMaterial[l_Index]
				);
		}
	}

	return pst_MadMat;
#endif
	return NULL;
}

#endif /* ! PSX2_TARGET */
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MAT_MultiAddRef(MAT_tdst_Multi *_pst_Mat, LONG _l_NbRef)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Material	**ppst_Mat, **ppst_LastMat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ppst_Mat = _pst_Mat->dpst_SubMaterial;
	ppst_LastMat = ppst_Mat + _pst_Mat->l_NumberOfSubMaterials;
	for(; ppst_Mat < ppst_LastMat; ppst_Mat++)
	{
		if((*ppst_Mat) && ((*ppst_Mat)->st_Id.i->ul_Type != GRO_MaterialMulti))
			(*ppst_Mat)->st_Id.i->pfn_AddRef((*ppst_Mat), _l_NbRef);
		else
			ppst_LastMat = ppst_LastMat;
	}

	_pst_Mat->st_Id.l_Ref += _l_NbRef;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MAT_p_MultiDuplicate(MAT_tdst_Multi *_pst_Mat, char *_sz_Path, char *_sz_Name, ULONG _ul_Flags)
{
#ifdef ACTIVE_EDITORS

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAT_tdst_Multi	*pst_MultiMat;
    int				i;
    char			sz_Name[ BIG_C_MaxLenName ];
    ULONG			ul_Dir;
#ifdef JADEFUSION
	char			sz_Temp[ 256 ];
    ULONG			ul_Key;
	EDIA_cl_NameDialog	o_Dialog("Generated name too long!");
	int				i_Ok;
#endif
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_MultiMat = (MAT_tdst_Multi *) MAT_pst_CreateMaterial(GRO_MaterialMulti, _sz_Name );
    MAT_AllocMultiMaterial((MAT_tdst_Material *) pst_MultiMat, _pst_Mat->l_NumberOfSubMaterials );
    
#ifdef JADEFUSION
	//we save here because we need the key generated by this function to rename duplicated sub material
	GRO_ul_Struct_FullSave(&pst_MultiMat->st_Id, _sz_Path, NULL, &TEX_gst_GlobalList);
#endif
    if ( _ul_Flags & 0x100 )
    {
    	/* duplicate sub material */
		for( i = 0; i < _pst_Mat->l_NumberOfSubMaterials; i++)
		{
			if( !_pst_Mat->dpst_SubMaterial[ i ] )
				pst_MultiMat->dpst_SubMaterial[ i ] = _pst_Mat->dpst_SubMaterial[ i ];
			else
			{
#ifdef JADEFUSION
				ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_MultiMat);
				strcpy(sz_Name, _pst_Mat->dpst_SubMaterial[ i ]->st_Id.sz_Name);
				if(strchr(sz_Name, '[') != NULL)
				{
					//we replace the actual key number with the new one
					*(strchr(sz_Name, '[') - 6) = 0;
					sprintf(sz_Temp, "_%d[%x].grm", i, ul_Key);
					strcat(sz_Name, sz_Temp);
				}
				else
				{
                	sprintf( sz_Temp, "_%s_%d[%x].grm", _pst_Mat->dpst_SubMaterial[ i ]->st_Id.sz_Name, i, ul_Key );
					if(strlen(sz_Temp) > BIG_C_MaxLenName)
					{
						o_Dialog.mo_Name = sz_Temp;
						//get new name
						do
						{
							i_Ok = o_Dialog.DoModal();
							if( i_Ok == IDOK)
							{
								strcpy(sz_Temp, (char *) (LPCSTR) o_Dialog.mo_Name);
								if(strlen(sz_Temp) > BIG_C_MaxLenName)
								{
									i_Ok = 0;
								}
							}
						}
						while(i_Ok != IDOK);
					}
					strcpy(sz_Name, sz_Temp);
				}
#else
				L_strcpy( sz_Name, _pst_Mat->dpst_SubMaterial[ i ]->st_Id.sz_Name );
				if (strrchr( sz_Name, '.')) *strrchr( sz_Name, '.') = 0;
				L_strcat( sz_Name, ".grm" );
#endif				
				ul_Dir = BIG_ul_SearchDir( _sz_Path );
				if (ul_Dir != BIG_C_InvalidIndex)
				{
					while( BIG_ul_SearchFile( ul_Dir, sz_Name ) != BIG_C_InvalidIndex )
					{
						memmove( sz_Name + 1, sz_Name, strlen( sz_Name ) + 1 );
						sz_Name[ 0 ] = '_';
					}
				}
					
				pst_MultiMat->dpst_SubMaterial[ i ] = (MAT_tdst_Material*)_pst_Mat->dpst_SubMaterial[ i ]->st_Id.i->pfnp_Duplicate( _pst_Mat->dpst_SubMaterial[ i ], _sz_Path, sz_Name, 0 );
				if( pst_MultiMat->dpst_SubMaterial[ i ] )
					pst_MultiMat->dpst_SubMaterial[ i ]->st_Id.l_Ref++;
			}
		}
    }
    else
    {
		/* Add reference to single material of multi material */
		for( i = 0; i < _pst_Mat->l_NumberOfSubMaterials; i++)
		{
			pst_MultiMat->dpst_SubMaterial[ i ] = _pst_Mat->dpst_SubMaterial[ i ];
			if( pst_MultiMat->dpst_SubMaterial[ i ] )
				pst_MultiMat->dpst_SubMaterial[ i ]->st_Id.l_Ref++;
		}
	}
	
	
	GRO_ul_Struct_FullSave(&pst_MultiMat->st_Id, _sz_Path, NULL, &TEX_gst_GlobalList);
    return pst_MultiMat;
#endif
    return NULL;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
