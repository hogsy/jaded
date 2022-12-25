/*$T VAVview_grp.cpp GC! 1.081 10/18/01 16:13:40 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "VAVview/VAVview.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BASe/MEMory/MEM.h"
#include "AIinterp/Sources/AIengine.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGread.h"

#ifdef JADEFUSION
extern char		*BIG_FileName(BIG_INDEX);
#else
extern "C" char		*BIG_FileName(BIG_INDEX);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
*/
CString EVAV_Grp_DrawItem(EVAV_cl_ViewItem *_po_Item, void *_p_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index;
	char		asz_World[512];
	char		asz_Sector[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	ul_Index = (BIG_INDEX) (* (ULONG * ) _p_Value);
	
	if(ul_Index && ((ULONG) ul_Index != 0xFFFFFFFF))
	{
		sprintf(asz_World, BIG_NameDir(BIG_ParentDir(BIG_ParentFile(ul_Index))));
		sprintf(asz_Sector, "%s (%s)", BIG_NameFile(ul_Index), asz_World);
		return(asz_Sector);
	}
	else
		return "- None - ";
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EVAV_Grp_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox		*po_Combo;
	WOR_tdst_World	*pst_World;
	BIG_KEY			ul_SelIndex, ul_Key;
	BIG_INDEX		ul_Index, ul_Dir, ul_Fat;
	int				i_Res;
	int				iSel;
	char			asz_Path[BIG_C_MaxLenPath];
	char			asz_Sel[BIG_C_MaxLenName];
	char			asz_Name[BIG_C_MaxLenName];
	char			asz_World[BIG_C_MaxLenName];
	CString			o_Name;
	ULONG			*pul_Buffer;
	ULONG			l_Length;
	ULONG			i;
	char			*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	po_Combo = (CComboBox *) _po_Wnd;
	
	/* Fill the combo box with the list of all groups of current world */
	if(_b_Fill)
	{
		po_Combo->ResetContent();
		
		/* Search the current object */
		if(*(ULONG *) _p_Value)
			ul_SelIndex = * (ULONG *) _p_Value;
		else
			ul_SelIndex = BIG_C_InvalidIndex;
		
		/* None objects */
		ul_Index = po_Combo->AddString("- None - ");
		L_strcpy(asz_Sel, "- None - ");
		
		po_Combo->SetItemData(ul_Index, BIG_C_InvalidIndex);
		
		pst_World = (WOR_tdst_World *) WOR_gst_Universe.st_WorldsTable.p_Table->p_Pointer;
		if(!pst_World) return;
		
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_World);
		if(ul_Index == BIG_C_InvalidIndex) return;		
		
		BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
		L_strcpy(asz_Name, BIG_NameFile(ul_Index));
		psz_Temp = L_strrchr(asz_Name, '.');
		if(psz_Temp) *psz_Temp = 0;
		L_strcat(asz_Name, ".wol");
		
		ul_Fat = BIG_ul_SearchFileExt(asz_Path, asz_Name);

		if(ul_Fat != BIG_C_InvalidIndex)
		{			
			pul_Buffer = (ULONG *) BIG_pc_ReadFileTmp(BIG_PosFile(ul_Fat), &l_Length);
			for(i = 0; i < l_Length >> 3; i++)
			{
				ul_Key = pul_Buffer[0];
				if(!ul_Key)
				{
					pul_Buffer += 2;
					continue;
				}
				
				ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
				
				L_strcpy(asz_World, BIG_NameFile(ul_Fat));
				psz_Temp = L_strrchr(asz_World, '.');
				if(psz_Temp) *psz_Temp = 0;
				
				BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
				L_strcat(asz_Path, "/Groups");
				
				ul_Dir = BIG_ul_SearchDir(asz_Path);
				if(ul_Dir == BIG_C_InvalidIndex)
				{
					pul_Buffer += 2;
					continue;
				}
				
				/* Scan Groups dir */
				ul_Index = BIG_FirstFile(ul_Dir);
				while(ul_Index != BIG_C_InvalidIndex)
				{
					if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtObjGolGroups))
					{
						/* If the current GRP is not a SCT one, we skip it ... */
						sprintf(asz_Name, "%s (%s)", BIG_FileName(ul_Index), asz_World);						
						o_Name = asz_Name;
						i_Res = po_Combo->AddString(o_Name);
						if(ul_SelIndex == ul_Index) L_strcpy(asz_Sel, asz_Name);
						po_Combo->SetItemData(i_Res, ul_Index);					
					}
					
					ul_Index = BIG_NextFile(ul_Index);
				}
				
				pul_Buffer += 2;
			}		
			
			/* Select current function */
			po_Combo->SetTopIndex(0);
			po_Combo->SetCurSel(po_Combo->FindStringExact(-1, asz_Sel));
			return;
		}
		else
		{
			L_strcat(asz_Path, "/");
			L_strcat(asz_Path, EDI_Csz_Path_Groups);
			ul_Dir = BIG_ul_SearchDir(asz_Path);
			if(ul_Dir == BIG_C_InvalidIndex) return;
	
			/* Scan dir */
			ul_Index = BIG_FirstFile(ul_Dir);
			while(ul_Index != BIG_C_InvalidIndex)
			{
				if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtObjGolGroups))					
				{
					o_Name = BIG_NameFile(ul_Index);
					if(ul_SelIndex == ul_Index) L_strcpy(asz_Sel, BIG_NameFile(ul_Index));
					i_Res = po_Combo->AddString(o_Name);
					po_Combo->SetItemData(i_Res, ul_Index);
				}
				
				ul_Index = BIG_NextFile(ul_Index);
			}
			
			/* Select current function */
			po_Combo->SetTopIndex(0);
			po_Combo->SetCurSel(po_Combo->FindStringExact(-1, asz_Sel));
			return;
		}
	}

	/* Search the selected object, and change it */
	iSel = po_Combo->GetCurSel();
	if(iSel == -1) return;
	ul_SelIndex = po_Combo->GetItemData(iSel);
	if(ul_SelIndex == BIG_C_InvalidIndex)
	{
		*(ULONG *) _p_Value = NULL;
	}
	else
	{
		if(ul_SelIndex)
			*(ULONG *) _p_Value = ul_SelIndex;
		else
			*(ULONG *) _p_Value = NULL;
	}
}

#endif /* ACTIVE_EDITORS */
