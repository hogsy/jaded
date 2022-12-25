/*$T VAVview_modifier.cpp GC! 1.097 07/23/01 09:54:52 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "EDImainframe.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIAgromodifier_dlg.h"
#include "DIAlogs/DIAgromodifiersnap_dlg.h"
#include "DIAlogs/DIAmorphing_dlg.h"
#include "DIAlogs/DIArlicarte_dlg.h"
#include "DIAlogs/DIAsaveaddmatrix_dlg.h"
#include "DIAlogs/DIAorder_dlg.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKstruct.h"
#include "VAVview/VAVview.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GAO.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEN.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GRID.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/GRId/GRI_compute.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "BIGfiles/BIGfat.h"
#include "BASe/MEMory/MEM.h"

#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dview_undo.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "EDIeditors_infos.h"


#include "EDItors/Sources/SOuNd/SONutil.h"

#ifdef JADEFUSION
#include "Engine/Sources/MoDiFier/MDFmodifier_Wind.h"
#include "Engine/Sources/Wind/WindSource.h"
#include "DIAlogs/diawindcurve_dlg.h"
#endif

extern EVAV_cl_ListBox	*gpo_CurVavListBox;
#ifdef JADEFUSION
extern void Grid_Modifier_CopyPaste(MDF_tdst_Modifier *, BOOL);
#else
extern "C" void Grid_Modifier_CopyPaste(MDF_tdst_Modifier *, BOOL);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_Modifier_Rotate(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World					*pst_World;
	OBJ_tdst_GameObject				*pst_GO;
	Grid_tdst_Modifier				*pst_GridMdF;
	MATH_tdst_Vector				st_Min, st_Max;
	int								i, k, A, B;
	int								XMin, XMax;
	int								YMin, YMax;
	int								dX, dY;
	char							*pc_Buf;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Mod || !_pst_Mod->pst_GO) return;

	pst_GO = _pst_Mod->pst_GO;
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_GridMdF = (Grid_tdst_Modifier *) _pst_Mod->p_Data;

	if(!pst_GridMdF || (!pst_GridMdF->pc_Capa && !pst_GridMdF->pc_Capa1)) return;

	pst_GridMdF->ul_Rotate = (pst_GridMdF->ul_Rotate == 270) ? 0 : pst_GridMdF->ul_Rotate += 90;

	/* Rotate BV */
	MATH_CopyVector(&st_Min, OBJ_pst_BV_GetGMin(pst_GO->pst_BV));
	MATH_CopyVector(&st_Max, OBJ_pst_BV_GetGMax(pst_GO->pst_BV));

	MATH_InitVector(OBJ_pst_BV_GetGMinInit(pst_GO->pst_BV), st_Min.y, st_Min.x, st_Min.z);
	MATH_InitVector(OBJ_pst_BV_GetGMaxInit(pst_GO->pst_BV), st_Max.y, st_Max.x, st_Max.z);

	MATH_CopyVector(OBJ_pst_BV_GetGMin(pst_GO->pst_BV), OBJ_pst_BV_GetGMinInit(pst_GO->pst_BV));
	MATH_CopyVector(OBJ_pst_BV_GetGMax(pst_GO->pst_BV), OBJ_pst_BV_GetGMaxInit(pst_GO->pst_BV));

	MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(pst_GO->pst_BV), &pst_GO->pst_GlobalMatrix->T);
	MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(pst_GO->pst_BV), &pst_GO->pst_GlobalMatrix->T);

	if(pst_GridMdF->pc_Capa)
	{
		pc_Buf = (char *) MEM_p_Alloc(pst_GridMdF->ul_Num);
		L_memcpy(pc_Buf, pst_GridMdF->pc_Capa, pst_GridMdF->ul_Num);

		GRI_3Dto2D(pst_World->pst_Grid, &st_Min, &XMin, &YMin);
		GRI_3Dto2D(pst_World->pst_Grid, &st_Max, &XMax, &YMax);

		if(((XMax - XMin + 1) * (YMax - YMin + 1)) != pst_GridMdF->ul_Num)
		{
			if((XMax + 1 - XMin + 1) * (YMax - YMin + 1) == pst_GridMdF->ul_Num)
				XMax ++;
			else if((XMax - XMin + 1) * (YMax + 1- YMin + 1) == pst_GridMdF->ul_Num)
				YMax ++;
			else if((XMax + 1 - XMin + 1) * (YMax + 1 - YMin + 1) == pst_GridMdF->ul_Num)
			{
				XMax ++;
				YMax ++;
			}
			else if((XMax - 1 - XMin + 1) * (YMax - YMin + 1) == pst_GridMdF->ul_Num)
				XMax --;
			else if((XMax - XMin + 1) * (YMax - 1 - YMin + 1) == pst_GridMdF->ul_Num)
				YMax --;
			else if((XMax - 1 - XMin + 1) * (YMax - 1 - YMin + 1) == pst_GridMdF->ul_Num)
			{
				XMax --;
				YMax --;
			}
			else if((XMax - 1 - XMin + 1) * (YMax + 1 - YMin + 1) == pst_GridMdF->ul_Num)
			{
				XMax --;
				YMax ++;
			}
			else if((XMax + 1 - XMin + 1) * (YMax - 1 - YMin + 1) == pst_GridMdF->ul_Num)
			{
				XMax ++;
				YMax --;
			}
		}


		dX = XMax - XMin + 1;
		dY = YMax - YMin + 1;

		for(i = 0; i < (int) pst_GridMdF->ul_Num; i++)
		{
			A = (i % dY) + 1;
			B = (int) ((float)i / ((float)dY));

			k = (dX * (dY - A)) + B;

			pst_GridMdF->pc_Capa[k] = pc_Buf[i];
			
		}

		MEM_Free(pc_Buf);
	}


	if(pst_GridMdF->pc_Capa1)
	{
		pc_Buf = (char *) MEM_p_Alloc(pst_GridMdF->ul_Num1);
		L_memcpy(pc_Buf, pst_GridMdF->pc_Capa1, pst_GridMdF->ul_Num1);

		GRI_3Dto2D(pst_World->pst_Grid1, &st_Min, &XMin, &YMin);
		GRI_3Dto2D(pst_World->pst_Grid1, &st_Max, &XMax, &YMax);

		if(((XMax - XMin + 1) * (YMax - YMin + 1)) != pst_GridMdF->ul_Num)
		{
			if((XMax + 1 - XMin + 1) * (YMax - YMin + 1) == pst_GridMdF->ul_Num)
				XMax ++;
			else if((XMax - XMin + 1) * (YMax + 1- YMin + 1) == pst_GridMdF->ul_Num)
				YMax ++;
			else if((XMax + 1 - XMin + 1) * (YMax + 1 - YMin + 1) == pst_GridMdF->ul_Num)
			{
				XMax ++;
				YMax ++;
			}
			else if((XMax - 1 - XMin + 1) * (YMax - YMin + 1) == pst_GridMdF->ul_Num)
				XMax --;
			else if((XMax - XMin + 1) * (YMax - 1 - YMin + 1) == pst_GridMdF->ul_Num)
				YMax --;
			else if((XMax - 1 - XMin + 1) * (YMax - 1 - YMin + 1) == pst_GridMdF->ul_Num)
			{
				XMax --;
				YMax --;
			}
			else if((XMax - 1 - XMin + 1) * (YMax + 1 - YMin + 1) == pst_GridMdF->ul_Num)
			{
				XMax --;
				YMax ++;
			}
			else if((XMax + 1 - XMin + 1) * (YMax - 1 - YMin + 1) == pst_GridMdF->ul_Num)
			{
				XMax ++;
				YMax --;
			}
		}

		dX = XMax - XMin + 1;
		dY = YMax - YMin + 1;

		for(i = 0; i < (int) pst_GridMdF->ul_Num1; i++)
		{
			A = (i % dY) + 1;
			B = (int) ((float)i / ((float)dY));

			k = (dX * (dY - A)) + B;

			pst_GridMdF->pc_Capa1[k] = pc_Buf[i];
			
		}

		MEM_Free(pc_Buf);
	}

}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_Modifier_CopyPasteGrid(MDF_tdst_Modifier *_pst_Mod, BOOL _bCopy)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject				*pst_GO;
	Grid_tdst_Modifier				*pst_GridMdF;
	F3D_cl_Undo_EditGrid			*po_Modif;
	F3D_cl_View						*po_3DView;
	EOUT_cl_Frame					*po_Out;
	WOR_tdst_World					*pst_World;
	MATH_tdst_Vector				st_Min, st_Max;
	int								XMin, XMax;
	int								YMin, YMax;
	int								i_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Mod || !_pst_Mod->pst_GO) return;

	pst_GO = _pst_Mod->pst_GO;
	po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);

	if(!po_Out) return;

	po_3DView = po_Out->mpo_EngineFrame->mpo_DisplayView;

	pst_World = po_3DView->mst_WinHandles.pst_World;

	if(!pst_World || !pst_World->pst_Grid) return;

	pst_GridMdF = (Grid_tdst_Modifier *) _pst_Mod->p_Data;	

	if(!_bCopy)
	{
		MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(pst_GO->pst_BV), &pst_GO->pst_GlobalMatrix->T);
		MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(pst_GO->pst_BV), &pst_GO->pst_GlobalMatrix->T);

		GRI_3Dto2D(pst_World->pst_Grid, &st_Min, &XMin, &YMin);
		GRI_3Dto2D(pst_World->pst_Grid, &st_Max, &XMax, &YMax);

		po_Modif = new F3D_cl_Undo_EditGrid(po_3DView, pst_World->pst_Grid, XMin, YMin, XMax, YMax, (char *) pst_GridMdF->pc_Capa);
		po_3DView->mo_UndoManager.b_AskFor(po_Modif, FALSE);
	}

	if(_bCopy && (pst_GridMdF->pc_Capa || pst_GridMdF->pc_Capa1))
	{
		i_Res = M_MF()->MessageBox
			(
				"Vous avez deja une copie de grille mémorisée dans ce modifier. Voulez vous l'écraser ?",
				"Confirm",
				MB_ICONQUESTION | MB_YESNO
			);
		if(i_Res == IDNO) return;
	}

	Grid_Modifier_CopyPaste(_pst_Mod, _bCopy);		
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString EVAV_Modifier_DrawItem(EVAV_cl_ViewItem *_po_Item, void *_p_Value)
{
	char							asz_Temp[32];

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier				*pst_Modifier;
	Grid_tdst_Modifier				*pst_GridMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Modifier = NULL;
	if(_p_Value)
		pst_Modifier = *(MDF_tdst_Modifier **) _p_Value;

	if(pst_Modifier && L_stricmp(_po_Item->masz_Name, "Order Sound MdF") )
	{
		switch(pst_Modifier->i->ul_Type)
		{
			case MDF_C_Modifier_GRID:
				if(L_stricmp(_po_Item->masz_Name, "Paste Grid") == 0)
				{
					pst_GridMdF = (Grid_tdst_Modifier *) pst_Modifier->p_Data;
					if(pst_GridMdF->pc_Capa && pst_GridMdF->pc_Capa1)
					{
						sprintf(asz_Temp, "[Grid0+Grid1] %u cases", pst_GridMdF->ul_Num + pst_GridMdF->ul_Num1);
						return asz_Temp;
					}
					else if(pst_GridMdF->pc_Capa)
					{
						sprintf(asz_Temp, "[Grid0] %u cases", pst_GridMdF->ul_Num);
						return asz_Temp;
					}
					else if(pst_GridMdF->pc_Capa1)
					{
						sprintf(asz_Temp, "[Grid1] %u cases", pst_GridMdF->ul_Num1);
						return asz_Temp;
					}
					else
					{
						sprintf(asz_Temp, "No Paste Available");
						return asz_Temp;
					}
				}
				if(L_stricmp(_po_Item->masz_Name, "Rotate Pattern") == 0)
				{
					pst_GridMdF = (Grid_tdst_Modifier *) pst_Modifier->p_Data;
					if(!pst_GridMdF->pc_Capa && !pst_GridMdF->pc_Capa1)
					{
						sprintf(asz_Temp, "--¨No Pattern", pst_GridMdF->ul_Rotate);
						return asz_Temp;
					}

					sprintf(asz_Temp, "--Rotate %i°", pst_GridMdF->ul_Rotate);
					return asz_Temp;
				}

				break;

		}
	}

	sprintf(asz_Temp, "(Click on button)");
	return asz_Temp;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_Modifier_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString							o_String;
	EDIA_cl_GroModifierDialog		o_Dlg;
	EDIA_cl_GroModifierSnapDialog	o_DlgSnap;
	MDF_tdst_Modifier				*pst_Modifier;
	void							*p_Data;
	GEO_tdst_ModifierSnap			*pst_DataSnap;
	EVAV_cl_ViewItem				*po_Parent;
	OBJ_tdst_GameObject				*pst_GO;
	char							asz_Msg[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Retreive GAO
	 -------------------------------------------------------------------------------------------------------------------
	 */

	po_Parent = gpo_CurVavListBox->po_GetParentGAO(_po_Data);
	if(po_Parent == NULL)
	{
		M_MF()->MessageBox
			(
				"Modifier have to be included in Game object properties to be created or destroyed",
				"Error",
				MB_OK | MB_ICONSTOP
			);
		return;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pst_GO = (OBJ_tdst_GameObject *) po_Parent->mp_Data;

	/* Special field */
	if(*(long *) _p_Value != 0)
	{

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(L_stricmp(_po_Data->masz_Name, "Destroy") == 0)
		{
			MDF_Modifier_Destroy(*((MDF_tdst_Modifier **) _p_Value));
			MDF_Modifier_DelInGameObject(pst_GO, *((MDF_tdst_Modifier **) _p_Value));
			LINK_UpdatePointer(pst_GO);
			LINK_UpdatePointers();
		}

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		else if(L_stricmp(_po_Data->masz_Name, "Apply in geometry") == 0)
		{
			(*(MDF_tdst_Modifier **) _p_Value)->ul_Flags |= GEO_C_ModifierFlags_ApplyInGeom;
			LINK_Refresh();
		}

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		else if(L_stricmp(_po_Data->masz_Name, "Re-Snap") == 0)
		{
			pst_DataSnap = (GEO_tdst_ModifierSnap *) (*(MDF_tdst_Modifier **) _p_Value)->p_Data;
			o_DlgSnap.mpst_SrcGao = pst_GO;
			o_DlgSnap.mpst_TgtGao = (OBJ_tdst_GameObject *) pst_DataSnap->p_GameObject;
			o_DlgSnap.mf_Treshold = 1.0f;
			if(o_DlgSnap.DoModal() == IDOK)
			{
				GEO_ModifierSnap_ComputeWithTresh
				(
					pst_GO,
					o_DlgSnap.mpst_TgtGao,
					o_DlgSnap.mf_Treshold,
					*(MDF_tdst_Modifier **) _p_Value
				);
				LINK_UpdatePointer(pst_GO);
				LINK_UpdatePointers();
			}
		}

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		else if(L_stricmp(_po_Data->masz_Name, "edit morph") == 0)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_MorphingDialog	*pst_Dialog;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Dialog = new EDIA_cl_MorphingDialog(*(MDF_tdst_Modifier **) _p_Value, pst_GO, (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO) );
			pst_Dialog->DoModeless();
		}

        
		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		else if(L_stricmp(_po_Data->masz_Name, "edit data") == 0)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_SaveAddMatrixDialog	*pst_Dialog;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Dialog = new EDIA_cl_SaveAddMatrixDialog(*(MDF_tdst_Modifier **) _p_Value);
			pst_Dialog->DoModeless();
		}


		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		else if(L_stricmp(_po_Data->masz_Name, "Compute normals") == 0)
		{
			GEO_ComputeNormalsOfPlanetAura((GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO));
		}

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		else if(L_stricmp(_po_Data->masz_Name, "Compute radius") == 0)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			GAO_tdst_ModifierSpecialLookAt	*pst_LookAt;
			GEO_tdst_Object					*pst_Obj;
			GEO_Vertex						*P;
			int								index;
			float							f, fMax;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Modifier = *(MDF_tdst_Modifier **) _p_Value;
			pst_LookAt = (GAO_tdst_ModifierSpecialLookAt *) pst_Modifier->p_Data;
			if(pst_LookAt->p_GAO != NULL)
			{
				pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo((OBJ_tdst_GameObject *) pst_LookAt->p_GAO);
				fMax = 0;
				if(pst_Obj)
				{
					index = pst_Obj->l_NbPoints;
					P = pst_Obj->dst_Point;
					while(index--)
					{
						f = MATH_f_NormVector((MATH_tdst_Vector *) P);
						if(f > fMax) fMax = f;
						P++;
					}
				}

				pst_LookAt->f_Z = fMax;
			}
		}

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		else if(L_stricmp(_po_Data->masz_Name, "Order Sound MdF") == 0)
		{
			ESON_OrderMdF((MDF_tdst_Modifier *) _p_Value);
			LINK_UpdatePointer(pst_GO);
			LINK_UpdatePointers();
		}

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		else if(L_stricmp(_po_Data->masz_Name, "Edit RLI group and color") == 0)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_RLICarteDialog	*pst_Dialog;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Dialog = new EDIA_cl_RLICarteDialog(*(MDF_tdst_Modifier **) _p_Value, pst_GO, (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO) );
			pst_Dialog->DoModeless();
		}
		else if(L_stricmp(_po_Data->masz_Name, "Copy Grid") == 0)
		{
			EVAV_Modifier_CopyPasteGrid(*(MDF_tdst_Modifier **) _p_Value, 1);
		}
		else if(L_stricmp(_po_Data->masz_Name, "Paste Grid") == 0)
		{
			EVAV_Modifier_CopyPasteGrid(*(MDF_tdst_Modifier **) _p_Value, 0);
		}
		else if(L_stricmp(_po_Data->masz_Name, "Rotate Pattern") == 0)
		{
			EVAV_Modifier_Rotate(*(MDF_tdst_Modifier **) _p_Value);
		}


		return;
	}

	/* Choose of a modifier */
	if(o_Dlg.DoModal() != IDOK) return;
	if(o_Dlg.mi_Modifier == -1) return;

	/* Check apply */
	if(!MDF_gast_ModifierInterface[o_Dlg.mi_Modifier].pfnb_CanBeApply(pst_GO, asz_Msg))
	{
		ERR_X_ForceError(asz_Msg, NULL);
		return;
	}

	/* Special init */
	p_Data = NULL;
	pst_Modifier = NULL;
	switch(o_Dlg.mi_Modifier)
	{
	case MDF_C_Modifier_Snap:
		o_DlgSnap.mpst_SrcGao = pst_GO;
		if(o_DlgSnap.DoModal() != IDOK) return;
		GEO_ModifierSnap_ComputeWithTresh(pst_GO, o_DlgSnap.mpst_TgtGao, o_DlgSnap.mf_Treshold, NULL);
		pst_Modifier = pst_GO->pst_Extended->pst_Modifiers;
		break;
	}

	/* Create new modifier */
	if(!pst_Modifier)
	{
		pst_Modifier = MDF_pst_Modifier_Create(pst_GO, o_Dlg.mi_Modifier, NULL);
		MDF_Modifier_AddToGameObject(pst_GO, pst_Modifier);
	}

	LINK_UpdatePointer(pst_GO);
	LINK_UpdatePointers();
}
#ifdef JADEFUSION
CString EVAV_WindCurve_DrawItem(EVAV_cl_ViewItem *_po_Item, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~*/
    char	asz_Temp[32];
    /*~~~~~~~~~~~~~~~~~*/

    sprintf(asz_Temp, "(Click on button)");
    return asz_Temp;
}

void EVAV_WindCurve_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
    EDIA_cl_WindCurve     * po_Dial;
    GAO_tdst_ModifierWind * p_Wind;
    CWindSource           * p_Source;

    p_Wind = (GAO_tdst_ModifierWind *) _p_Value;

    if (p_Wind)
    {
        p_Source = p_Wind->po_Source;

        po_Dial = new EDIA_cl_WindCurve(p_Source->GetAmplitude(), p_Source->GetShape(), p_Source->GetFrequency(),
            p_Source->GetSpeed(), p_Source->GetMinForce(), p_Source->GetMaxForce());

        if(po_Dial->DoModal() == IDOK)
        {
            p_Source->SetAmplitude(po_Dial->GetAmplitude());
            p_Source->SetShape(po_Dial->GetShape());
            p_Source->SetFrequency(po_Dial->GetFrequency());
            p_Source->SetSpeed(po_Dial->GetSpeed());
            p_Source->SetMinForce(po_Dial->GetMinForce());
            p_Source->SetMaxForce(po_Dial->GetMaxForce());
        }
    }
}
#endif

#endif /* ACTIVE_EDITORS */
