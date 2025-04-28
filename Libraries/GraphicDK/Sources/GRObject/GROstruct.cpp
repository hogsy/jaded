/*$T GROstruct.c GC! 1.081 05/17/01 08:05:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGfat.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/BAStypes.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "GDInterface/GDInterface.h"
#include "GEOmetric/GEODebugObject.h"
#include "GRObject/GROstruct.h"
#include "SOFT/SOFTuvgizmo.h"
#include "LINks/LINKstruct.h"

#ifndef PSX2_TARGET
#ifdef ACTIVE_EDITORS
#include "EDIpaths.h"
#include "MAD_loadSave/Sources/MAD_Struct_V0.h"
#include "MAD_mem/Sources/MAD_mem.h"
#define _ReadLong(_a)	*(LONG *) _a
#endif
#else
#include "MainPsx2/Sources/PSX2debug.h"
#define _ReadLong(_a)	ReadLong((char *) _a)
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
char				*GRO_gasz_InterfaceName[GRO_Cl_NumberOfInterfaces] =
{
	"Unknown",
	"Geometric",
	"Light",
	"Single Material",
	"Multi Material",
	"Multi-texture Material",
	"Camera",
	"Waypoint",
	"Static LOD",
	"2D-Sprite List",
	"2D-Text",
	"Particle generator"
};
#endif /* ACTIVE_EDITORS */

GRO_tdst_Interface	GRO_gast_Interface[GRO_Cl_NumberOfInterfaces];
GRO_tdst_Struct		GRO_gst_Unknown;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GRO_tdst_Struct *GRO_pst_Struct_Create(LONG _l_Type)
{
	return (GRO_tdst_Struct *) GRO_gast_Interface[_l_Type].pfnp_CreateDefault();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_Struct_Init(GRO_tdst_Struct *_pst_Struct, LONG _l_Type)
{
	_pst_Struct->i = &GRO_gast_Interface[_l_Type];
	_pst_Struct->l_Ref = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_Struct_Free(GRO_tdst_Struct *_pst_Struct)
{
#ifdef ACTIVE_EDITORS
	if(_pst_Struct->sz_Name)
	{
		MEM_Free(_pst_Struct->sz_Name);
		_pst_Struct->sz_Name = NULL;
	}

	LINK_SetDelPointer(_pst_Struct);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GRO_l_Struct_Load(GRO_tdst_Struct *_pst_Struct, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~*/
	char	*pc_Current;
	ULONG	ul_Type;
    LONG    l_Size;
	/*~~~~~~~~~~~~~~~~*/

	pc_Current = _pc_Buffer;

	ul_Type = LOA_ReadULong(&pc_Current);

	/* check interface */
	if(ul_Type < 0) ul_Type = GRO_Unknown;
	if(ul_Type >= GRO_Cl_NumberOfInterfaces) ul_Type = GRO_Unknown;

	_pst_Struct->i = &GRO_gast_Interface[ul_Type];
    _pst_Struct->l_Ref = 0;

    /* on récupére la taille des données supplémentaires :
        avant c'était le nom du fichier et l_Size correspondait à la taille de ce nom
        maintenant c'est 0
    */
	l_Size = LOA_ReadLong_Ed(&pc_Current, NULL); 

#ifdef ACTIVE_EDITORS
	_pst_Struct->sz_Name = 0;
#endif

#if defined(XML_CONV_TOOL)
	if (l_Size > 0)
		GRO_Struct_SetName(_pst_Struct, pc_Current);
#endif

	return (pc_Current - _pc_Buffer) + l_Size;
}

/*$4
 ***********************************************************************************************************************
    Interface
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GRO_p_UnknowCreateDefault(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct *pst_Struct;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Struct = (GRO_tdst_Struct *) MEM_p_Alloc(sizeof(GRO_tdst_Struct));
	pst_Struct->i = &GRO_gast_Interface[GRO_Unknown];
	pst_Struct->l_Ref = 0;
	GRO_Struct_SetName(pst_Struct, "");
	return pst_Struct;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GRO_p_UnknowCreateFromBuffer(GRO_tdst_Struct *_pst_Struct, char **ppc, void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct *pst_Struct;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Struct = (GRO_tdst_Struct *) MEM_p_Alloc(sizeof(GRO_tdst_Struct));
	L_memset(pst_Struct, 0, sizeof(GRO_tdst_Struct));
	pst_Struct->i = &GRO_gast_Interface[GRO_Unknown];
	pst_Struct->l_Ref = 0;

	/* GRO_Struct_SetName(pst_Struct, _pst_Struct->sz_Name); */
	return pst_Struct;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GRO_p_UnknowDuplicate(GRO_tdst_Struct *_pst_Struct, char *_sz_Path, char *_sz_Name, ULONG _ul_Flag)
{
	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_UnknownDestroy(void *p)
{
	if(((GRO_tdst_Struct *) p)->l_Ref > 0) return;
	GRO_Struct_Free((GRO_tdst_Struct *) p);
	LOA_DeleteAddress(p);
	MEM_Free(p);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GRO_UnknowSaveInBuffer(void *p, void *p2)
{
	GRO_Struct_Save((GRO_tdst_Struct *) p);
	return 0;
}

#ifdef ACTIVE_EDITORS /* no MAD with PS2 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GRO_tdst_Struct *GRO_p_UnknowCreateFromMad(void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct *pst_Struct;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Struct = (GRO_tdst_Struct *) MEM_p_Alloc(sizeof(GRO_tdst_Struct));
	L_memset(pst_Struct, 0, sizeof(GRO_tdst_Struct));
	pst_Struct->i = &GRO_gast_Interface[GRO_Unknown];
	pst_Struct->l_Ref = 0;
	GRO_Struct_SetName(pst_Struct, ((MAD_NodeID *) p)->Name);
	return pst_Struct;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GRO_p_UnknowToMad(void *p, void *p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	MAD_NodeID	*pst_MadNode;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	MAD_MALLOC(MAD_NodeID, pst_MadNode, 1);
	pst_MadNode->IDType = ID_MAD_UNDEFINED;
	pst_MadNode->SizeOfThisOne = sizeof(MAD_NodeID);
	*pst_MadNode->Name = 0;
	return pst_MadNode;
}

#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GRO_l_UnknowHasSomethingToRender(void *p1, GEO_tdst_Object **ppst_PickableObject)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	i_Index;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(!(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_DisplayInvisible)) return FALSE;

#ifdef ACTIVE_EDITORS
	i_Index = GDI_gpst_CurDD->pst_CurrentGameObject->ul_InvisibleObjectIndex;
	if(i_Index > GEO_DebugObject_LastInvisible - GEO_DebugObject_Invisible) i_Index = 0;

	if( !GDI_WPShowStatusOn( GDI_gpst_CurDD, i_Index ) ) 
		return FALSE;

#else
	i_Index = 0;
#endif
	*ppst_PickableObject = GEO_pst_DebugObject_Get(GEO_DebugObject_Invisible + i_Index);
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_UnknowRender(OBJ_tdst_GameObject *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
	ULONG				C, DM;
	MATH_tdst_Matrix	*M;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	i = _pst_Node->ul_InvisibleObjectIndex;
	if((i < 0) || (i > GEO_DebugObject_LastInvisible - GEO_DebugObject_Invisible)) i = 0;
	C = (_pst_Node->ul_EditorFlags & OBJ_C_EditFlags_Selected) ? 1 : 0;
	C = GEO_ul_DebugObject_GetColor(GEO_DebugObject_Invisible + i, C);
	GEO_DebugObject_SetLight(C);

	M = OBJ_pst_GetAbsoluteMatrix(_pst_Node);
	DM = GDI_Cul_DM_All - GDI_Cul_DM_UseAmbient - GDI_Cul_DM_Fogged - GDI_Cul_DM_MaterialColor;
    GEO_DebugObject_Draw(GDI_gpst_CurDD, GEO_DebugObject_Invisible + i, DM, _pst_Node->ul_EditorFlags, M);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GRO_UnknowPushSpecialMatrix(void *p1)
{
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GRO_PushSpecialMatrixForProportionnalBone(OBJ_tdst_GameObject *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_Scale;
	MATH_tdst_Vector	st_Pos;
	MATH_tdst_Matrix	Scale;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_Proportionnal)) return 0;

	f_Scale = 0.8f * fNormalTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2);

	MATH_TransformVertex(&st_Pos, GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix, &MATH_gst_NulVector);
	MATH_SetIdentityMatrix(&Scale);
	if(st_Pos.z > 0)
	{
		f_Scale *= st_Pos.z;
		if(f_Scale < 1) f_Scale = 1;
		MATH_SetZoom(&Scale, f_Scale);
	}

	SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, &Scale);
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GRO_PushSpecialMatrixForProportionnal(OBJ_tdst_GameObject *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_Scale;
	MATH_tdst_Vector	st_Pos;
	MATH_tdst_Matrix	Scale;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_Proportionnal)) return 0;

#if defined(_XENON_RENDER)
    if (OBJ_b_TestIdentityFlag(_pst_Node, OBJ_C_IdentityFlag_Lights) &&
        _pst_Node->pst_Extended && _pst_Node->pst_Extended->pst_Light &&
        (_pst_Node->pst_Extended->pst_Light->i->ul_Type == GRO_Light))
    {
        // Do not scale the light shafts
        if ((((LIGHT_tdst_Light*)_pst_Node->pst_Extended->pst_Light)->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_LightShaft)
            return 0;
    }
#endif
	f_Scale = 0.1f * fNormalTan(GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2);

	MATH_TransformVertex(&st_Pos, GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix, &MATH_gst_NulVector);
	MATH_SetIdentityMatrix(&Scale);
	if(st_Pos.z > 0)
	{
		f_Scale *= st_Pos.z;
		if(f_Scale < 1) f_Scale = 1;
		MATH_SetZoom(&Scale, f_Scale);
	}

	SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, &Scale);
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_UnknowAddRef(void *p, LONG l)
{
	((GRO_tdst_Struct *) p)->l_Ref += l;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if defined(ACTIVE_EDITORS)
char *GRO_sz_UnknowFileExtension(void)
{
	return EDI_Csz_ExtGraphicObject;
}
#endif // defined(ACTIVE_EDITORS)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_UnknowReinit(GRO_tdst_Struct *p)
{
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GRO_Unknow_User_0 (struct GRO_tdst_Struct_ *p , ULONG P0)
{
	return 0;
}
/*
 =======================================================================================================================
    Init Interface
 =======================================================================================================================
 */
void GRO_Struct_InitInterfaces(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_Type;
	GRO_tdst_Interface	*i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	i = GRO_gast_Interface;

	for(ul_Type = 0; ul_Type < GRO_Cl_NumberOfInterfaces; ul_Type++, i++)
	{
		i->ul_Type = ul_Type;

		i->pfnp_CreateDefault = GRO_p_UnknowCreateDefault;
		i->pfnp_CreateFromBuffer = GRO_p_UnknowCreateFromBuffer;
#if defined (__cplusplus)
		i->pfnp_Duplicate = (void *(__cdecl *)(void *,char *, char*, ULONG))GRO_p_UnknowDuplicate;
#else
		i->pfnp_Duplicate = GRO_p_UnknowDuplicate;
#endif
		i->pfn_Destroy = GRO_UnknownDestroy;
#if defined (__cplusplus)
		i->pfnl_HasSomethingToRender = (LONG (__cdecl *)(void *,void *))GRO_l_UnknowHasSomethingToRender;
#else
		i->pfnl_HasSomethingToRender = GRO_l_UnknowHasSomethingToRender;
#endif
		i->pfn_Render = NULL;
		i->pfn_AddRef = GRO_UnknowAddRef;
		i->pfn_Reinit = GRO_UnknowReinit;
		i->pfn_User_0 = GRO_Unknow_User_0;

#ifdef ACTIVE_EDITORS
		i->pfnl_SaveInBuffer = GRO_UnknowSaveInBuffer;
#if defined (__cplusplus)
		i->pfnp_CreateFromMad = (void*(__cdecl*)(void*))GRO_p_UnknowCreateFromMad;
#else
		i->pfnp_CreateFromMad = GRO_p_UnknowCreateFromMad;
#endif
		i->pfnp_ToMad = GRO_p_UnknowToMad;
		i->pfnl_PushSpecialMatrix = GRO_UnknowPushSpecialMatrix;
		i->pfnsz_FileExtension = GRO_sz_UnknowFileExtension;
#if defined (__cplusplus)
		i->pfn_Render = (void (__cdecl *)(void *))GRO_UnknowRender;
#else
		i->pfn_Render = GRO_UnknowRender;
#endif
#endif
	}

#ifdef ACTIVE_EDITORS
	i = &GRO_gast_Interface[GRO_Unknown];
#if defined (__cplusplus)
	i->pfnl_PushSpecialMatrix = (LONG (__cdecl *)(void *))GRO_PushSpecialMatrixForProportionnal;
#else
	i->pfnl_PushSpecialMatrix = GRO_PushSpecialMatrixForProportionnal;
#endif
#endif
	GRO_gst_Unknown.i = i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_Struct_DestroyTable(TAB_tdst_Ptable *_pst_Table, int _i_Pass)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct **ppst_First, **ppst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ppst_Last = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(_pst_Table);

    /* first pass : decrement and eventually destroy */
    ppst_First = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(_pst_Table);
    for(; ppst_First <= ppst_Last; ppst_First++)
	{
        (*ppst_First)->i->pfn_AddRef((*ppst_First), -1);

		/* Fix GRO */
		if(ENG_gb_ExitApplication && (((*ppst_First)->l_Ref >= 1000000)))
		{
			(*ppst_First)->l_Ref -= 1000000;
		}
#ifdef _GAMECUBE	/* Only Fix Display-Lists Desallocation */
		if((*ppst_First)->l_Ref  >= 1000000)
		{
		    (*ppst_First)->i->pfn_Destroy(*ppst_First);	
		}
#endif		

		if((*ppst_First)->l_Ref == 0)
		{
		    (*ppst_First)->i->pfn_Destroy(*ppst_First);
            TAB_Ptable_RemoveElem(_pst_Table,(void **)ppst_First);
        }
    }


    /* est ce que le multipasse est vraiment necessaire, hein je vous le demande ? */
    // Oui il est nécessaire dans le cas de GRO_Struct_DestroyTable(&_pst_World->st_GraphicMaterialsTable, 2),
    // où on détruit une arborescence.  
    _i_Pass--;
    /* another pass : destroy all with no reference */
	while(_i_Pass--)
	{
		ppst_First = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(_pst_Table);
		for(; ppst_First <= ppst_Last; ppst_First++)
		{
			if(!(*ppst_First)) continue;
            if(TAB_b_IsAHole(*ppst_First)) continue;
			if((*ppst_First)->l_Ref == 0)
			{
				(*ppst_First)->i->pfn_Destroy(*ppst_First);
				*ppst_First = NULL;
			}
		}
	}

#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~*/
		char	asz_Log[100];
		ULONG	ul_Ref;
		/*~~~~~~~~~~~~~~~~~*/

		ul_Ref = 0;
		ppst_First = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(_pst_Table);
		for(; ppst_First <= ppst_Last; ppst_First++)
		{
			if(!(*ppst_First)) continue;
            if(TAB_b_IsAHole(*ppst_First)) continue;
            if ((*ppst_First)->l_Ref == 0) continue;

			if(!ul_Ref)
			{
				LINK_PrintStatusMsg("----------------------------------");
				LINK_PrintStatusMsg("      Desallocation warning:      ");
				LINK_PrintStatusMsg("----------------------------------");
			}

            sprintf
			(
				asz_Log,
				"[%s] %s -> Ref = %i",
				GRO_gasz_InterfaceName[(*ppst_First)->i->ul_Type],
				GRO_sz_Struct_GetName(*ppst_First),
				(*ppst_First)->l_Ref
			);
			LINK_PrintStatusMsg(asz_Log);
			ul_Ref++;
		}
	}

#endif
	TAB_Ptable_Close(_pst_Table);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_Struct_SetName(GRO_tdst_Struct *_pst_Struct, char *_sz_Name)
{
	/* Free old name */
	if(_pst_Struct->sz_Name) MEM_Free(_pst_Struct->sz_Name);

	/* Set new name */
	if(_sz_Name)
		_pst_Struct->sz_Name = L_strcpy((char *) MEM_p_Alloc(strlen(_sz_Name) + 1), _sz_Name);
	else
		_pst_Struct->sz_Name = NULL;
}

static char GRO_ssz_Name[64];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *GRO_sz_Struct_GetName(GRO_tdst_Struct *_pst_Struct)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	*sz_FileName, *sz_Ext;
	ULONG	ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Struct);
	if(ul_Index == BIG_C_InvalidIndex)
	{
		strcpy(GRO_ssz_Name, "Unnamed");
		return GRO_ssz_Name;
	}

	sz_FileName = BIG_NameFile(ul_Index);
	sz_Ext = strrchr(sz_FileName, '.');
	if(sz_Ext) *sz_Ext = 0;

	L_strcpy(GRO_ssz_Name, sz_FileName);

	if(sz_Ext) *sz_Ext = '.';

	return GRO_ssz_Name;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRO_Struct_Save(GRO_tdst_Struct *_pst_Struct)
{
	/*~~~~~~~~~~~~~*/
	ULONG	ul_Value;
	/*~~~~~~~~~~~~~*/
#if !defined(XML_CONV_TOOL)
	ul_Value = _pst_Struct->i->ul_Type;
	SAV_Buffer(&ul_Value, 4);
	ul_Value = 0;
	SAV_Buffer(&ul_Value, 4);
#else
	int ix;
	int len = 0;

	ul_Value = _pst_Struct->i->ul_Type;
	SAV_Buffer(&ul_Value, 4);
	if (_pst_Struct->sz_Name)
		len = strlen(_pst_Struct->sz_Name);
	if (len > 0)
		len++;	// add null termination
	SAV_Buffer(&len, 4);
	for (ix = 0; ix < len; ix++)
	{
		ul_Value = _pst_Struct->sz_Name[ix];
		SAV_Buffer(&ul_Value, 1);
	}
#endif	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BIG_KEY GRO_ul_Struct_FullSave(GRO_tdst_Struct *_pst_Struct, char *_sz_Path, char *_sz_Name, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		ul_Value;
	char		sz_Path[BIG_C_MaxLenPath];
	char		sz_Name[BIG_C_MaxLenPath];
	int			i_File;
	BIG_INDEX	ul_Dir;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_sz_Name == NULL)
	{
		ul_Value = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Struct);
		if(ul_Value != BIG_C_InvalidKey)
		{
			ul_Value = BIG_ul_SearchKeyToFat(ul_Value);
			if(ul_Value != BIG_C_InvalidIndex)
			{
				if(_sz_Path == NULL)
				{
					BIG_ComputeFullName(BIG_ParentFile(ul_Value), sz_Path);
					_sz_Path = sz_Path;
				}

				_sz_Name = BIG_NameFile(ul_Value);
			}
		}
	}

	if(_sz_Name == NULL)
	{
		if(_sz_Path == NULL)
		{
			L_strcpy(sz_Path, EDI_Csz_Path_TrashCan_Objects);
			_sz_Path = sz_Path;
		}

		ul_Dir = BIG_ul_CreateDir(_sz_Path);
		i_File = 0;
		while(1)
		{
			sprintf(sz_Name, "Unnamed%i", i_File);
			L_strcat(sz_Name, _pst_Struct->i->pfnsz_FileExtension());
			if(BIG_ul_SearchFile(ul_Dir, sz_Name) == BIG_C_InvalidIndex) break;
			i_File++;
		}

		GRO_Struct_SetName(_pst_Struct, sz_Name);
		_sz_Name = sz_Name;
	}

	SAV_Begin(_sz_Path, _sz_Name);
	_pst_Struct->i->pfnl_SaveInBuffer(_pst_Struct, p_Data);
	ul_Value = SAV_ul_End();

	LOA_AddAddress(ul_Value, _pst_Struct);

	if(ul_Value == BIG_C_InvalidIndex) return BIG_C_InvalidKey;
	return BIG_FileKey(ul_Value);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL GRO_b_IsAMaterial(GRO_tdst_Struct *_pst_Gro)
{
	if(_pst_Gro->i->ul_Type == GRO_MaterialSingle) return TRUE;
	if(_pst_Gro->i->ul_Type == GRO_MaterialMulti) return TRUE;
	if(_pst_Gro->i->ul_Type == GRO_MaterialMultiTexture) return TRUE;
	return FALSE;
}

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
