/*$T DIAbrickmapper_dlg.cpp GC 1.134 06/08/04 13:34:43 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS

#include "EDIpaths.h"
#include "EDIeditors_infos.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "Res/Res.h"
#include "DIAlogs/DIAbrickmapper_dlg.h"
#include "DIAlogs/DIAgraphchoose_dlg.h"
#include "SELection/SELection.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "GraphicDK/Sources/GEOmetric/GEOload.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "EDItors/Sources/PreFaB/PFBdata.h"
#include "LINKs/LINKtoed.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"



/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_BrickMapper, EDIA_cl_BaseDialog)
	ON_BN_CLICKED(ID_BRICK_MAP, OnBnMap)
	ON_BN_CLICKED(ID_CLOSE_BRICK_MAP, OnClose)
	ON_CBN_SELCHANGE(IDC_COMBO_GRAPH_AMBIENCE, OnAmbienceChanged)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
	ON_WM_CLOSE(OnClose)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */

EDIA_cl_BrickMapper::EDIA_cl_BrickMapper(F3D_cl_View *_po_View) :
	EDIA_cl_BaseDialog(IDD_BRICKMAPPER)	//(struct WOR_tdst_World_ *W)
{
	 mpo_3DView = _po_View;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_BrickMapper::~EDIA_cl_BrickMapper(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_BrickMapper::OnInitDialog(void)
{
	/*~~~~~~~~~~~~*/
	CComboBox *o_ComboGraph;
	BIG_INDEX	h_File;
	CString sz_Path;
	CString sz_Graph, sz_Abbrev, sz_PathGraph, sz_PathLD;
	char *pc_Buffer;
	int index = 0;
	int i = 0;
	/*~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnInitDialog();

	h_File = BIG_ul_SearchFileExt(EDI_Csz_Path_ToolSettings, "BrickMapper.var");
	if(h_File == BIG_C_InvalidIndex)
	{
		MessageBox("Couldn't find file BrickMapper.var", "Missing file");
		return false;
	}
	else
	{
		pc_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(h_File), NULL);
	}
	while (*pc_Buffer != NULL)
	{
		// Reading "BrickMapper.var" file
		sz_Graph.Empty();
		sz_Abbrev.Empty();
		sz_PathGraph.Empty();

		if (*pc_Buffer == '"')
		{
			*pc_Buffer++;
		}

		while (*pc_Buffer != '"')
		{
			sz_Graph.AppendChar(*pc_Buffer);
			*pc_Buffer++;
		}
		while (*pc_Buffer == ' ' || *pc_Buffer == '"')
			*pc_Buffer++;

		while (*pc_Buffer != ' ')
		{
			sz_Abbrev.AppendChar(*pc_Buffer);
			*pc_Buffer++;
		}
		while (*pc_Buffer == ' ' || *pc_Buffer == '"')
			*pc_Buffer++;

		while (*pc_Buffer != '"')
		{
			sz_PathLD.AppendChar(*pc_Buffer);
			*pc_Buffer++;
		}
		while (*pc_Buffer == ' ' || *pc_Buffer == '"')
			*pc_Buffer++;

		while (*pc_Buffer != '"' && *pc_Buffer != '\n' && *pc_Buffer != NULL)
		{
			sz_PathGraph.AppendChar(*pc_Buffer);
			*pc_Buffer++;
		}

		while (*pc_Buffer == '\n' || *pc_Buffer == '"')
			*pc_Buffer++;

		
		// Filling the theme combo box
		o_ComboGraph = (CComboBox *) GetDlgItem(IDC_COMBO_GRAPH_THEME);
		o_ComboGraph->AddString(sz_Graph);

		CString **p_PathGraph = (CString **) MEM_p_Alloc (3*sizeof(CString *));
		p_PathGraph[0] = new CString(sz_Abbrev);
		p_PathGraph[1] = new CString(sz_PathGraph);
		p_PathGraph[2] = new CString(sz_PathLD);
				
		o_ComboGraph->SetItemDataPtr(index++, p_PathGraph);
	}

	if (o_ComboGraph->GetCount())
		o_ComboGraph->SetCurSel(0);

	FillAmbience();
	FillAlternative();

	UpdateData(FALSE);

	CenterWindow();
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_BrickMapper::FillAlternative(void)
{
	/*~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*p_Sel, *p_Sel2;
	OBJ_tdst_GameObject		*pst_GO;
	CComboBox				*o_ComboTheme, *o_ComboAlt, *o_ComboAmbience;
	BIG_INDEX				h_Dir, h_File;
	CString					firstName, brickName, firstBrickName, selName, selectionTheme;
	int						index = 0;
	/*~~~~~~~~~~~~*/

	o_ComboAlt = (CComboBox *) GetDlgItem(IDC_COMBO_GRAPH_ALTERNATIVE);
	o_ComboAlt->ResetContent();

	o_ComboTheme = (CComboBox *) GetDlgItem(IDC_COMBO_GRAPH_THEME);
	CString **strTemp = (CString **) o_ComboTheme->GetItemDataPtr(o_ComboTheme->GetCurSel());
	CString *abbrev = strTemp[0];
	CString *pathGraphTemp = strTemp[1];	
	CString *pathLDTemp = strTemp[2];
	CString pathGraph(EDI_Csz_Path_Prefab + CString('/'));

	// Get selected ambience
	o_ComboAmbience = (CComboBox *) GetDlgItem(IDC_COMBO_GRAPH_AMBIENCE);
	CString selectedAmbience;
	if (o_ComboAmbience->GetCount() != 0)
	{
		o_ComboAmbience->GetLBText(o_ComboAmbience->GetCurSel(), selectedAmbience);
	}
	if (!L_strcmp(selectedAmbience, "LD"))
		pathGraph.Append(*pathLDTemp);
	else
		pathGraph.Append(*pathGraphTemp);


	p_Sel = mpo_3DView->GetFirstSel();

	/* Ensure that only bricks are treated */
	while (p_Sel && (CString(((OBJ_tdst_GameObject *) p_Sel->p_Content)->sz_Name)).Find("BRK") == -1)
	{
		p_Sel = mpo_3DView->GetNextSel(p_Sel);
	}
	if (p_Sel)
	{
		p_Sel2 = p_Sel;
		selName = ((OBJ_tdst_GameObject *) p_Sel->p_Content)->sz_Name;
		selectionTheme = GetTheme(selName);
		firstBrickName = GetBrickName(selName);
	}
	else
		return FALSE;

	/* If all selected item instance the same prefab, get alternative names */ 
	while (p_Sel)
	{
		// Get selected objects
		pst_GO = (OBJ_tdst_GameObject *) p_Sel->p_Content;
		
		CString fileName(pst_GO->sz_Name);
		if (fileName.Find("BRK") == -1)
		{
			p_Sel = mpo_3DView->GetFirstSel();
			continue;
		}

		brickName = GetBrickName(fileName);
		if (L_strcmp(brickName, firstBrickName))
		{
			return FALSE;
		}

		p_Sel = mpo_3DView->GetNextSel(p_Sel);
	}

	h_Dir = BIG_ul_SearchDir(pathGraph);
	if (h_Dir == BIG_C_InvalidIndex)
	{
		ERR_X_Warning(0, "Couldn't find graph directory ", pathGraph.GetBuffer(pathGraph.GetLength()));
		return FALSE;
	}

	h_File = BIG_FirstFile(h_Dir);
	while (h_File != BIG_C_InvalidIndex && !BIG_b_IsFileExtension(h_File, EDI_Csz_ExtPrefab))
		h_File = BIG_NextFile(h_File);

	/* If all selected item instance the same prefab, get alternative names */ 
	while (h_File != BIG_C_InvalidIndex)
	{
		// Get selected objects
		CString fileName(BIG_NameFile(h_File));
		if (fileName.Find("BRK") == -1 || fileName.Find(firstBrickName) == -1 || !BIG_b_IsFileExtension(h_File, EDI_Csz_ExtPrefab))
		{
			h_File = BIG_NextFile(h_File);
			continue;
		}

		int indexAlt = fileName.ReverseFind('_');
		o_ComboAlt->AddString(CString(fileName[indexAlt-1]));

		h_File = BIG_NextFile(h_File);				
	}		

	/* Select by default current alternative */
	if (o_ComboAlt->GetCount())
	{
		o_ComboAlt->SetCurSel(0);
	}
		/*pst_GO = (OBJ_tdst_GameObject *) p_Sel2->p_Content;
		CString nameTemp(pst_GO->sz_Name);
		CString currentAlt;
		int indexAltTemp = nameTemp.ReverseFind('_');

		// En cas de non respect de la nomenclature des briques graph...
		if(nameTemp[indexAltTemp-1] == 'D' && nameTemp[indexAltTemp-2] == 'L')
		{
			indexAltTemp -= 3;
		}

		for (int i=0 ; i<o_ComboGraph->GetCount() ; i++)
		{
			o_ComboAlt->GetLBText(i, currentAlt);
			if (currentAlt[0] == nameTemp[indexAltTemp-1])
			{
				o_ComboAlt->SetCurSel(i);
				break;
			}
		}
	}*/
	
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_BrickMapper::FillAmbience(void)
{
	/*~~~~~~~~~~~~*/
	CString					lastFileName;
    CString					selName;
	SEL_tdst_SelectedItem	*p_Sel, *p_Sel2;
	OBJ_tdst_GameObject		*pst_GO;
	CComboBox				*o_ComboTheme, *o_ComboAmbience;
	BIG_INDEX				h_Dir, h_File;
	char					selectionTheme;
	int						index = 0;
	/*~~~~~~~~~~~~*/

	o_ComboAmbience = (CComboBox *) GetDlgItem(IDC_COMBO_GRAPH_AMBIENCE);
	o_ComboAmbience->ResetContent();

	o_ComboTheme = (CComboBox *) GetDlgItem(IDC_COMBO_GRAPH_THEME);
	CString **strTemp = (CString **) o_ComboTheme->GetItemDataPtr(o_ComboTheme->GetCurSel());
	CString *abbrev = strTemp[0];
	CString *pathGraphTemp = strTemp[1];	
	CString pathGraph(EDI_Csz_Path_Prefab + CString('/') + *pathGraphTemp);

	p_Sel = mpo_3DView->GetFirstSel();

	/* Ensure that only bricks are treated */
	while (p_Sel && (CString(((OBJ_tdst_GameObject *) p_Sel->p_Content)->sz_Name)).Find("BRK") == -1)
	{
		p_Sel = mpo_3DView->GetNextSel(p_Sel);
	}
	if (p_Sel)
	{
		p_Sel2 = p_Sel;
		selName = ((OBJ_tdst_GameObject *) p_Sel->p_Content)->sz_Name;
		selectionTheme = GetTheme(selName);
	}
	else
		return FALSE;

	// Fill ambience combo box only if all the selected bricks have the same theme
	while (p_Sel)
	{
		pst_GO = (OBJ_tdst_GameObject *) p_Sel->p_Content;
		if (GetTheme(CString(pst_GO->sz_Name)) != selectionTheme)
			return FALSE;
		p_Sel = mpo_3DView->GetNextSel(p_Sel);
	}

	o_ComboAmbience->AddString("LD");

	h_Dir = BIG_ul_SearchDir(pathGraph);
	if (h_Dir == BIG_C_InvalidIndex)
	{
		ERR_X_Warning(0, "Couldn't find graph directory ", pathGraph.GetBuffer(pathGraph.GetLength()));
		return FALSE;
	}

	h_File = BIG_FirstFile(h_Dir);
	if (h_File != BIG_C_InvalidIndex)
	{
		lastFileName = CString(BIG_NameFile(h_File));
		o_ComboAmbience->AddString(GetAmbienceName(lastFileName));
	}
	
	/* Fill ambience combo box */ 
	while (h_File != BIG_C_InvalidIndex)
	{
		CString fileName(BIG_NameFile(h_File));
		if (!L_strcmp(GetAmbienceName(lastFileName), GetAmbienceName(fileName)))
		{
			h_File = BIG_NextFile(h_File);
			continue;
		}

		o_ComboAmbience->AddString(GetAmbienceName(fileName));

        lastFileName = fileName;
		h_File = BIG_NextFile(h_File);
	}


	/* Select by default current alternative */
	if (o_ComboAmbience->GetCount())
	{
		pst_GO = (OBJ_tdst_GameObject *) p_Sel2->p_Content;
		CString nameTemp(pst_GO->sz_Name);
		CString currentAmbience;
		int indexAmbienceTemp = nameTemp.ReverseFind('.');

		int i=0;
		for (i=0 ; i<o_ComboAmbience->GetCount() ; i++)
		{
			o_ComboAmbience->GetLBText(i, currentAmbience);
			if (!L_strcmp(currentAmbience, GetAmbienceName(nameTemp)))
			{
				o_ComboAmbience->SetCurSel(i);
				break;
			}
        }
		if (i == o_ComboAmbience->GetCount())
			o_ComboAmbience->SetCurSel(0);
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString EDIA_cl_BrickMapper::GetAmbienceName(CString &fileName)
{
	CString ambienceName;
	int indexStop;
	
	int indexAmbience = fileName.ReverseFind('_');
	int indexExtension = fileName.ReverseFind('.');
	int indexArobase = fileName.ReverseFind('@');
	if (indexArobase != -1)
		indexStop = indexArobase;
	else
		indexStop = indexExtension;

	for (int i=indexAmbience+1 ; i<indexStop ; i++)
	{
		ambienceName.AppendChar(fileName[i]);
	}

	return ambienceName;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
char EDIA_cl_BrickMapper::GetTheme(CString &graphName)
{
	// Look for the theme abbreviation
	int indexFirstUnderscore = graphName.Find('_');
	int indexSecondUnderscore = graphName.Find('_', indexFirstUnderscore+1);
	if (indexFirstUnderscore == -1 || indexSecondUnderscore == -1)
	{
		ERR_X_Warning(0, "Bad file name ! ", graphName.GetBuffer(graphName.GetLength()));
		return NULL;
	}

	return graphName[indexSecondUnderscore+1];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString EDIA_cl_BrickMapper::GetBrickName(CString &fileName)
{
	CString brickName;
	
	int indexAlt = fileName.ReverseFind('_');

	// En cas de non respect de la nomenclature des briques graph...
	if(fileName[indexAlt-1] == 'D' && fileName[indexAlt-2] == 'L')
	{
		indexAlt -= 3;
	}

	int indexBracket = fileName.Find(']');
	if (indexBracket == -1)
		indexBracket = -2;
	for (int i=indexBracket+2 ; i<indexAlt -2 ; i++)
	{
		brickName.AppendChar(fileName[i]);
	}

	return brickName;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString EDIA_cl_BrickMapper::GetBrickNameWithAlt(CString &fileName)
{
	CString brickName;
	
	int indexAlt = fileName.ReverseFind('_');

	// En cas de non respect de la nomenclature des briques graph...
	if(fileName[indexAlt-1] == 'D' && fileName[indexAlt-2] == 'L')
	{
		indexAlt -= 3;
	}

	int indexBracket = fileName.Find(']');
	if (indexBracket == -1)
		indexBracket = -2;
	for (int i=indexBracket+2 ; i<indexAlt ; i++)
	{
		brickName.AppendChar(fileName[i]);
	}

	return brickName;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString EDIA_cl_BrickMapper::ChangeBrickName(CString &fileName, CString &brickName)
{
	int indexStart, indexStop;
	CString newFileName;

	int indexBracket = fileName.Find(']');
	if (indexBracket == -1)
		indexStart = 0;
	else
		indexStart = indexBracket + 2;

	int indexExtension = fileName.Find('.');
	int indexArobase = fileName.Find('@');
	if (indexArobase == -1)
		indexStop = indexExtension;
	else
		indexStop = indexArobase;

	int i;
	for (i=0 ; i<indexStart ; i++)
		newFileName.AppendChar(fileName[i]);

	for (i=0 ; i<brickName.GetLength() ; i++)
		newFileName.AppendChar(brickName[i]);

	for (i=indexStop ; i<fileName.GetLength() ; i++)
		newFileName.AppendChar(fileName[i]);
	
	return newFileName;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BrickMapper::OnAmbienceChanged(void)
{
	FillAlternative();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BrickMapper::OnBnMap(void)
{
	/*~~~~~~~~~~~~*/
	int						i,l;
	BIG_INDEX				h_File, h_Dir;
	CComboBox				*o_ComboGraph, *o_ComboAlt, *o_ComboAmbience;
	OBJ_tdst_GameObject		*pst_GO;
	SEL_tdst_SelectedItem	*p_Sel;
	OBJ_tdst_Prefab			*pst_PrefabNew;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// Get selected theme, with corresponding abbrev and path
	o_ComboGraph = (CComboBox *) GetDlgItem(IDC_COMBO_GRAPH_THEME);
	CString **strTemp = (CString **) o_ComboGraph->GetItemDataPtr(o_ComboGraph->GetCurSel());
	CString *abbrev = strTemp[0];
	CString *pathGraphTemp = strTemp[1];	
	CString *pathLDTemp = strTemp[2];
	CString pathGraph(EDI_Csz_Path_Prefab + CString('/') + *pathGraphTemp);
	CString pathLD(EDI_Csz_Path_Prefab + CString('/') + *pathLDTemp);

	// Get selected alternative
	o_ComboAlt = (CComboBox *) GetDlgItem(IDC_COMBO_GRAPH_ALTERNATIVE);
	CString altSel;
	if (o_ComboAlt->GetCount() != 0)
	{
		o_ComboAlt->GetLBText(o_ComboAlt->GetCurSel(), altSel);
	}

	// Get selected ambience
	o_ComboAmbience = (CComboBox *) GetDlgItem(IDC_COMBO_GRAPH_AMBIENCE);
	CString selectedAmbience;
	if (o_ComboAmbience->GetCount() != 0)
	{
		o_ComboAmbience->GetLBText(o_ComboAmbience->GetCurSel(), selectedAmbience);
	}

	p_Sel = mpo_3DView->GetFirstSel();
	while (p_Sel && (CString(((OBJ_tdst_GameObject *) p_Sel->p_Content)->sz_Name)).Find("BRK") == -1)
	{
		p_Sel = mpo_3DView->GetNextSel(p_Sel);
	}
	if (!p_Sel) return;
	pst_GO = (OBJ_tdst_GameObject *) p_Sel->p_Content;

	/* If ambience "LD" is selected, search graph bricks in LD directory */
	if (!L_strcmp(selectedAmbience, "LD") && p_Sel)
	{
		h_Dir = BIG_ul_SearchDir(pathLD);
	}
	else
	{
		h_Dir = BIG_ul_SearchDir(pathGraph);
	}
	if (h_Dir == BIG_C_InvalidIndex)
	{
		ERR_X_Warning(0, "Couldn't find graph directory, check BrickMapper.var file. ", NULL);
		return;
	}
	
	while (p_Sel)
	{
		// Get selected objects
		pst_GO = (OBJ_tdst_GameObject *) p_Sel->p_Content;
		
		CString fileName(pst_GO->sz_Name);
		/* If current selection is a brick, and the base element of a prefab */
		if (fileName.Find("BRK") != -1 && o_ComboAmbience->FindString(0, GetAmbienceName(fileName)) != CB_ERR)
		{
			h_File = BIG_FirstFile(h_Dir);

			while (h_File != BIG_C_InvalidIndex && !BIG_b_IsFileExtension(h_File, EDI_Csz_ExtPrefab))
				h_File = BIG_NextFile(h_File);

			CString brickName(GetBrickNameWithAlt(fileName));
			int count = o_ComboAlt->GetCount();
			// Change the brick's name if an alternative is selected
			if (o_ComboAlt->GetCount() != 0)
			{
				brickName.SetAt(brickName.GetLength()-1, altSel[0]);
			}

			while (h_File != BIG_C_InvalidIndex)
			{
				if (!BIG_b_IsFileExtension(h_File, EDI_Csz_ExtPrefab)) 
				{
					h_File = BIG_NextFile(h_File);
					continue;
				}

				CString graphName(BIG_NameFile(h_File));
				CString ambienceName(GetAmbienceName(graphName));
			
				int indexGraphName = graphName.ReverseFind('_');
				CString graphName2;
				for (l = indexGraphName + 1 ; l < graphName.GetLength() ; l++)
				{
					graphName2.AppendChar(graphName[l]);
				}

				// If current file corresponds to selected brick and ambience, switch prefab link
				if (graphName.Find(brickName) != -1 && !L_strcmp(ambienceName, selectedAmbience))
				{
					/* Load the prefab if it is not present */
					pst_PrefabNew = (OBJ_tdst_Prefab *) LOA_ul_SearchAddress(BIG_PosFile(h_File));
					if((BIG_INDEX) pst_PrefabNew == BIG_C_InvalidIndex)
						//pst_PrefabNew = mpo_3DView->BrickLoad(h_File);
						pst_PrefabNew = Prefab_pst_Load(h_File, NULL);

					for(i = 0 ; i < pst_PrefabNew->l_NbRef ; ++i)
					{
						//pst_GO2 = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(pst_PrefabNew->dst_Ref[0].ul_Index));

						// Get subelement's name in loaded prefab
						CString subObjName(BIG_NameFile(pst_PrefabNew->dst_Ref[i].ul_Index));
						// Avec chargement ---- CString subObjName(pst_GO2->sz_Name);
						CString elementName;
						int j = subObjName.ReverseFind('_')+1;//attAbbrev[o_ComboGraph->GetCurSel()])-2;
						while (subObjName[j] != '.')
						{
							elementName.AppendChar(subObjName[j++]);
						}
						
						// If elementName is not the ambience (i.e. if it's really the element's name, therefore if it's a subelement of prefab)
						/*if (o_ComboAmbience->FindString(0, elementName) == CB_ERR)
						{
							// Find the old prefab's element that corresponds to this name
							pst_PrefabOld = (OBJ_tdst_Prefab *) LOA_ul_SearchAddress(BIG_PosFile(BIG_ul_SearchKeyToFat(pst_GO->ul_PrefabKey)));
							for(k = 0; k < pst_PrefabOld->l_NbRef; k++)
							{
								OBJ_tdst_GameObject *oldElement = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(pst_PrefabOld->dst_Ref[k].ul_Index));
								CString oldElementName(oldElement->sz_Name);
								if (oldElementName.Find(elementName) != -1)
								{
									pst_GO = oldElement;
									break;
								}
							}
						}*/	

						// Switch prefab infos
						// Avec chargement ---- pst_GO->ul_PrefabKey = pst_GO2->ul_PrefabKey;
						// Avec chargement ---- pst_GO->ul_PrefabObjKey = pst_GO2->ul_PrefabObjKey;
						if ((CString(BIG_NameFile(pst_PrefabNew->dst_Ref[i].ul_Index))).Find(elementName) == -1)
							continue;
                        
						pst_GO->ul_PrefabObjKey = pst_PrefabNew->dst_Ref[i].ul_Key;
					}

					CString newFileName(brickName + CString('_') + selectedAmbience);
					mpo_3DView->RenameObj(pst_GO, ChangeBrickName(CString(pst_GO->sz_Name), newFileName).GetBuffer());

					pst_GO->ul_PrefabKey = BIG_FileKey(pst_PrefabNew->ul_Index);

					mpo_3DView->BrickUpdate(pst_GO);

					if(pst_PrefabNew->l_NbRef != 0) L_free(pst_PrefabNew->dst_Ref);
					if(pst_PrefabNew->sz_Comment) L_free(pst_PrefabNew->sz_Comment);
					L_memset(pst_PrefabNew, 0, sizeof(OBJ_tdst_Prefab));

					mpo_3DView->BrickMapperSave(pst_GO);
					
					break;
				}

				h_File = BIG_NextFile(h_File);
			}
		}		

		p_Sel = mpo_3DView->GetNextSel(p_Sel);
	}

	FillAmbience();
	FillAlternative();

	LINK_Refresh();

	//mpo_3DView->BrickMapperSave();
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */

void EDIA_cl_BrickMapper::OnClose()
{
	EDIA_cl_BaseDialog::OnClose();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void EDIA_cl_BrickMapper::OnDestroy()
{
	mpo_3DView->BrickMapperClose();
	EDIA_cl_BaseDialog::OnDestroy();
}


#endif