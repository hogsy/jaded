
/*$T WORload.c GC! 1.100 08/31/01 14:50:14 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "TABles/TABles.h"
#include "GRObject/GROsave.h"
#include "MATerial/MATstruct.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJgrp.h"
#include "ENGine/Sources/OBJects/OBJgizmo.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/WORld/WORsecto.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/WAYs/WAYload.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "ENGine/Sources/WAYs/WAYinit.h"
#include "AIinterp/Sources/AIload.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/Events/EVEload.h"
#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKstruct.h"
#include "EDIpaths.h"
#include "ENGine/Sources/GRP/GRPload.h"
#include "ENGine/Sources/COLlision/COLload.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/ANImation/ANIplay.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/ACTions/ACTload.h"
#include "ENGine/Sources/ACTions/ACTinit.h"
#include "ENGine/Sources/ACTions/ACTcompute.h"
#include "ENGine/Sources/GRId/GRI_load.h"
#include "ENGine/Sources/GRId/GRI_compute.h"
#include "GraphicDK/Sources/SELection/SELection.h"
#include "GraphicDK/Sources/GDInterface/GDInterface.h"
#include "SouND/Sources/SND.h"
#include "BASe/BASarray.h"



#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEN.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_XMEN.h"


#ifdef ACTIVE_EDITORS

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern char *BIG_FileName(BIG_INDEX);
/*
BIG_INDEX LOA_ul_SearchIndexWithAddress(ULONG _ul_Adr)
BIG_ParentFile => recup index répertoire
BIG_ComputeFullName(BIG_ParentFile(mul_CurrentModel), asz_Path1);
void SAV_Begin(char *_psz_Path, char *_psz_Name)
void SAV_Buffer(void *_p_Buffer, int _i_Size)
BIG_INDEX SAV_ul_End(void)
char *BIG_pc_ReadFileTmp(ULONG _ul_Pos, ULONG *_pul_Length)
_ul_Pos = BIG_PosFile(BIG_INDEX)
BIG_INDEX = BIG_ul_SeachFileExt(path, name) 			BIG_C_InvalidIndex

*/

void WORGos_DestroyGroup(WOR_tdst_World *_pst_Dest)
{
	ULONG Size;
	if (!_pst_Dest->ulNumberOfGroups) return;
	Size = _pst_Dest->ulNumberOfGroups;
	while (Size--)
	{
		MEM_Free(_pst_Dest->pListOfGroups[Size].BigKeyListe);
	}
	MEM_Free(_pst_Dest->pListOfGroups);
	_pst_Dest->ulNumberOfGroups = 0;
	_pst_Dest->pListOfGroups = NULL;
}
void WORGos_DefineGroup(WOR_tdst_World *_pst_World, char *GroupName)
{
    TAB_tdst_PFelem     *pst_Elem, *pst_LastElem;
	OBJ_tdst_GameObject *pst_GO;
	ULONG ulNumberOfObject,StrLenght;



	for(ulNumberOfObject = 0 , pst_Elem = _pst_World->st_AllWorldObjects.p_Table , pst_LastElem = _pst_World->st_AllWorldObjects.p_NextElem ; pst_Elem < pst_LastElem;pst_Elem++)
	{
		if(TAB_b_IsAHole(pst_Elem->p_Pointer)) continue;
		pst_GO = (OBJ_tdst_GameObject *) (pst_Elem->p_Pointer);
		if (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected) 
		{
			ULONG ul_Key;
			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG)pst_GO);
			if (ul_Key != BIG_C_InvalidIndex)
				ulNumberOfObject++;
		}
	}
	if (!ulNumberOfObject) return;
	if (_pst_World->pListOfGroups)
		_pst_World->pListOfGroups = (WOR_SelectionGroup*)MEM_p_Realloc(_pst_World->pListOfGroups , (_pst_World->ulNumberOfGroups + 1) * sizeof(WOR_SelectionGroup));
	else
		_pst_World->pListOfGroups = (WOR_SelectionGroup*)MEM_p_Alloc((_pst_World->ulNumberOfGroups + 1) * sizeof(WOR_SelectionGroup));


	StrLenght = strlen(GroupName);

	if (StrLenght > 250) StrLenght = 250;
	memcpy(_pst_World->pListOfGroups[_pst_World->ulNumberOfGroups].Name , GroupName , StrLenght );
	while (StrLenght<256) _pst_World->pListOfGroups[_pst_World->ulNumberOfGroups].Name[StrLenght++] = 0;

	_pst_World->pListOfGroups[_pst_World->ulNumberOfGroups].ulNumberOfKey = ulNumberOfObject;
	_pst_World->pListOfGroups[_pst_World->ulNumberOfGroups].BigKeyListe = (unsigned long*)MEM_p_Alloc(ulNumberOfObject * 4);
	
	for(ulNumberOfObject = 0 , pst_Elem = _pst_World->st_AllWorldObjects.p_Table , pst_LastElem = _pst_World->st_AllWorldObjects.p_NextElem ; pst_Elem < pst_LastElem;pst_Elem++)
	{
		if(TAB_b_IsAHole(pst_Elem->p_Pointer)) continue;
		pst_GO = (OBJ_tdst_GameObject *) (pst_Elem->p_Pointer);
		if (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected) 
		{
			ULONG ul_Key;
			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG)pst_GO);
			if (ul_Key != BIG_C_InvalidIndex)
				_pst_World->pListOfGroups[_pst_World->ulNumberOfGroups].BigKeyListe[ulNumberOfObject++] = ul_Key ;
		}
	}

	_pst_World->ulNumberOfGroups++;

}
ULONG WORGos_IsInGroup(WOR_SelectionGroup *p_Group, ULONG ul_Key)
{
	ULONG JeyCountre;
	JeyCountre = p_Group->ulNumberOfKey;
	while (JeyCountre--) if (p_Group->BigKeyListe[JeyCountre] == ul_Key) return 1;
	return 0;
}

void WORGos_RemindGroup(WOR_tdst_World *_pst_World, ULONG ulNumber)
{
    TAB_tdst_PFelem     *pst_Elem, *pst_LastElem;
	OBJ_tdst_GameObject *pst_GO;
	WOR_SelectionGroup *p_Group;
	ULONG ulNumberOfObject,AddMode;

	AddMode = -1;
	if(GetAsyncKeyState(VK_CONTROL) >= 0) AddMode = ~OBJ_C_EditFlags_Selected;

	if (_pst_World->ulNumberOfGroups < ulNumber) return;
	p_Group = &_pst_World->pListOfGroups[ulNumber];

	for(ulNumberOfObject = 0 , pst_Elem = _pst_World->st_AllWorldObjects.p_Table , pst_LastElem = _pst_World->st_AllWorldObjects.p_NextElem ; pst_Elem < pst_LastElem;pst_Elem++)
	{
		ULONG ul_Key;
		if(TAB_b_IsAHole(pst_Elem->p_Pointer)) continue;
		pst_GO = (OBJ_tdst_GameObject *) (pst_Elem->p_Pointer);
		pst_GO->ul_EditorFlags &= AddMode ;
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG)pst_GO);
		if (ul_Key != BIG_C_InvalidIndex)
		{
			if (WORGos_IsInGroup(p_Group, ul_Key))
				pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_Selected;
		}
	}
}

void WORGos_GetFileFromAddresse(WOR_tdst_World *_pst_Dest, char *asz_Path12 , char *FileName12)
{
	char			*DotFinder;
	BIG_INDEX ul_FileIndex , ul_DirIndex;
	*asz_Path12 = *FileName12 = 0;
	ul_FileIndex = LOA_ul_SearchIndexWithAddress((ULONG)_pst_Dest);
	if(ul_FileIndex == BIG_C_InvalidIndex) return;
	ul_DirIndex = BIG_ParentFile(ul_FileIndex);
	if(ul_DirIndex == BIG_C_InvalidIndex) return;


	/* Save world in file associated with world */
	BIG_ComputeFullName(ul_DirIndex, asz_Path12);
	L_strcpy(FileName12, BIG_FileName(ul_FileIndex));

	DotFinder = FileName12;
	while (*DotFinder) DotFinder++;
	DotFinder--;
	while ((*DotFinder != '.') && *DotFinder) DotFinder--;
	if (*DotFinder == '.')
	{
		DotFinder[1] = 'g';
		DotFinder[2] = 'o';
		DotFinder[3] = 's';

	} else *FileName12 = 0;

}

void WORGos_Load(WOR_tdst_World *_pst_Dest)
{
	char			asz_Path12[1024];
	char			FileName12[1024];
	BIG_INDEX		GosPos;
	ULONG			ul_Length,KeyCounter;
	char			*pc_Buffer ;

	WORGos_GetFileFromAddresse(_pst_Dest, asz_Path12, FileName12);
	
	if (*asz_Path12 == 0) return;
	if (*FileName12 == 0) return;

	GosPos = BIG_ul_SearchFileExt(asz_Path12, FileName12);
	if (GosPos == BIG_C_InvalidIndex) return;
	GosPos = BIG_PosFile(GosPos);

	pc_Buffer = BIG_pc_ReadFileTmp(GosPos, &ul_Length);
	/* Read NumberOfGroups */
	ul_Length = *(ULONG *)pc_Buffer;
	pc_Buffer += 4;
	if (!ul_Length) return;
	if (_pst_Dest->pListOfGroups)
		_pst_Dest->pListOfGroups = (WOR_SelectionGroup*)MEM_p_Realloc(_pst_Dest->pListOfGroups , (_pst_Dest->ulNumberOfGroups + ul_Length ) * sizeof(WOR_SelectionGroup));
	else
		_pst_Dest->pListOfGroups = (WOR_SelectionGroup*)MEM_p_Alloc(ul_Length * sizeof(WOR_SelectionGroup));
	while (ul_Length--)
	{
		memcpy((void*)&_pst_Dest->pListOfGroups[_pst_Dest->ulNumberOfGroups].Name[0],(void*)pc_Buffer, 256);
		pc_Buffer += 256;
		KeyCounter = _pst_Dest->pListOfGroups[_pst_Dest->ulNumberOfGroups].ulNumberOfKey  = *(ULONG *)pc_Buffer;
		pc_Buffer += 4;
		_pst_Dest->pListOfGroups[_pst_Dest->ulNumberOfGroups].BigKeyListe = (unsigned long*)MEM_p_Alloc(4 * KeyCounter);
		while (KeyCounter)
		{
			_pst_Dest->pListOfGroups[_pst_Dest->ulNumberOfGroups].BigKeyListe = (unsigned long*)MEM_p_Alloc(4 * KeyCounter--);
			pc_Buffer += 4;
		}
		_pst_Dest->ulNumberOfGroups++;
	}

}
void WORGos_Save(WOR_tdst_World *_pst_Dest)
{
	char			asz_Path12[1024];
	char			FileName12[1024];
	ULONG Size;
	WORGos_GetFileFromAddresse(_pst_Dest, asz_Path12, FileName12);
	if (*asz_Path12 == 0) return;
	if (*FileName12 == 0) return;


	SAV_Begin(asz_Path12, FileName12);
	Size = _pst_Dest->ulNumberOfGroups;
	/* SaveNuberOfGroups */
	SAV_Buffer((void*)&Size, 4);
	while (Size--)
	{
		ULONG GroupCounter;
		/* SaveGroupName */
		SAV_Buffer((void*)&_pst_Dest->pListOfGroups[Size].Name[0], 256);
		/* SaveGroupNumber*/
		SAV_Buffer((void*)&_pst_Dest->pListOfGroups[Size].ulNumberOfKey, 4);
		GroupCounter = _pst_Dest->pListOfGroups[Size].ulNumberOfKey;
		while (GroupCounter --)
			SAV_Buffer((void*)&_pst_Dest->pListOfGroups[Size].BigKeyListe[GroupCounter], 4);
	}
	SAV_ul_End();
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif