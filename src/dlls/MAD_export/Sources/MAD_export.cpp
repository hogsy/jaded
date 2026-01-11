/*$T MAD_export.cpp GC!1.32 10/20/99 15:08:15 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    MAD_export.cpp - Ouf File Exporter By Philippe Vimont UBI Pictures mai 99
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "MAD_mem/Sources/MAD_mem.h"
#include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#include "Max.h"
#include "MAX_to_MAD.h"
#include "MAD_export.h"
#include "MAD_RADMOD\MAD_RADMOD.h"
#include "resource.h"
#include "modstack.h"

#include "SkinUtils.h"
#include "iparamm2.h"
#include "iskin.h"
#include "ImportExportDef.h"

extern unsigned long M_IsStringEqual(char *srcA, char *srcB);

/*$off*/
/*
 ===================================================================================================
	Max compatibility stuff 
 ===================================================================================================
*/
static HINSTANCE   hInstance;
static BOOL bInsertTextures = TRUE;
static BOOL bExportSkin = TRUE;
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{   hInstance = hinstDLL;
    return(TRUE);
}

#define MADOUT_CLASS_ID Class_ID(0xcae75cd0, 0xcaa9130f)
class MadClassDesc : public ClassDesc
{
public:
    int         IsPublic(void)                  { return 1; }
    void        *Create(BOOL loading = FALSE)   { return new MadOut; }
    const TCHAR *ClassName(void)                { return _T("MadOut"); }
    SClass_ID   SuperClassID(void)              { return SCENE_EXPORT_CLASS_ID; }
    Class_ID    ClassID(void)                   { return MADOUT_CLASS_ID; }
    const TCHAR *Category(void)                 { return _T("MAD Utility"); }
};

static MadClassDesc MadDesc;
ClassDesc *GetMadOutDesc(void)              {return &MadDesc;}

__declspec(dllexport)   const TCHAR *LibDescription(void)           {return "Qu'est ce que je vais bouffer ce midi ? ";}
__declspec(dllexport)   int LibNumberClasses(void)                  {return 1;}
__declspec(dllexport)   ClassDesc *LibClassDesc(int i)              {return GetMadOutDesc();}
__declspec(dllexport)   ULONG LibVersion(void)                      {return VERSION_3DSMAX;}
             MadOut::~MadOut(void)          {}
int          MadOut::ExtCount(void)         {return 1;}
const TCHAR *MadOut::Ext(int n)             {return _T("MAD");}
const TCHAR *MadOut::LongDesc(void)         {return _T("MAD Export");}
const TCHAR *MadOut::ShortDesc(void)        {return _T("MAD");}
const TCHAR *MadOut::AuthorName(void)       {return _T("Philippe Vimont");}
const TCHAR *MadOut::CopyrightMessage(void) {return _T("UBI PICTURE 1999");}
const TCHAR *MadOut::OtherMessage1(void)    {return _T("Aglaaaaa glaaaaaaaaaaaaaa!! ");}
const TCHAR *MadOut::OtherMessage2(void)    {return _T("shbrumf");}
unsigned int MadOut::Version(void)          {return 100;}
void            MadOut::ShowAbout(HWND hWnd)   {}
DWORD WINAPI fn (LPVOID arg)                {return(0);}
/*
 ===================================================================================================
	End of Max compatibility stuff 
 ===================================================================================================
*/
/*$on*/

/*
 ===================================================================================================
 ===================================================================================================
 */
MadOut::MadOut(void)
{
    ExportSelected = FALSE;
}

/*
 ===================================================================================================
    Return the maximum number of material included in a material
 ===================================================================================================
 */
unsigned long GetNumberOfMaterial(Mtl *Material)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    long    ReturnValue, MaterialCounter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!Material) return(0);
    if(Material->ClassID() == Class_ID(CMTL_CLASS_ID, 0)) return(1);  /* Case of simple material */
    ReturnValue = 1;
    for(MaterialCounter = 0; MaterialCounter < Material->NumSubMtls(); MaterialCounter++)
        ReturnValue += GetNumberOfMaterial(Material->GetSubMtl(MaterialCounter));
    return ReturnValue;
}

/*
 ===================================================================================================
    AllMaterials Builder
 ===================================================================================================
 */
unsigned long AllMaterialsBuilder(MadOut *MO, Mtl *Material)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   MaterialCnt;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!Material) return(0);

    if(Material->ClassID() != Class_ID(CMTL_CLASS_ID, 0))
    {
        for(MaterialCnt = 0; MaterialCnt < (ULONG) Material->NumSubMtls(); MaterialCnt++)
            AllMaterialsBuilder(MO, Material->GetSubMtl(MaterialCnt));
    }

    /* Insert new material in list */
    for(MaterialCnt = 0; MaterialCnt < MO->MaterialsNumber; MaterialCnt++)
    {
        if(MO->AllMaterials[MaterialCnt] == Material) return(0);
    }

    MO->AllMaterials[MaterialCnt] = Material;
    MO->MaterialsNumber++;
    return(0);
}

/*
 ===================================================================================================
    Materials Counter & builder
 ===================================================================================================
 */
DWORD WINAPI MAD_WorldMaterialBuilder(MadOut *MO, INode *node)
{
    AllMaterialsBuilder(MO, node->GetMtl());
    return(0);
}

/*
 ===================================================================================================
    This function compute the maximum number of material for allocate mem
 ===================================================================================================
 */
DWORD WINAPI MAD_WorldMaxMaterialCount(MadOut *MO, INode *node)
{
    MO->MaximumMaterials += GetNumberOfMaterial(node->GetMtl());
    return(0);
}

/*
 ===================================================================================================
    Counter Callback Funciton
 ===================================================================================================
 */
DWORD WINAPI MAD_WorldNodesStock(MadOut *MO, INode *MAXnode)
{
    MO->AllINode[MO->NodeNumber] = MAXnode;
    return(0);
}

/*
 ===================================================================================================
    Counter Callback Funciton
 ===================================================================================================
 */
DWORD WINAPI MAD_WorldObjRefCounAndStock(MadOut *MO, INode *MAXnode)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   ObjRefCounter;
    Object          *ObjREf;
	IDerivedObject *DrvdObj;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ObjREf = MAXnode->GetObjectRef();
    MO->All_INODE_To_ObjRef[MO->NodeNumber] = MAD_NULL_INDEX;
	MO->All_INODE_To_ObjRADIOSITY[MO->NodeNumber] = MAD_NULL_INDEX;
	MO->All_INODE_RADIOSITY_RLI[MO->NodeNumber] = NULL;
    if(ObjREf == NULL) return(0);
	if (ObjREf -> ClassID() == derivObjClassID)
	{
		DrvdObj = (IDerivedObject *)ObjREf;
		if (DrvdObj -> NumModifiers())
		{
			if (DrvdObj -> GetModifier(0) -> ClassID() == MAD_RadMod_CLASS_ID)
			{
				ObjREf = DrvdObj->GetObjRef();
				MO->AdditionnalObjects++;
			}
		}
	}
    for(ObjRefCounter = 0; ObjRefCounter < MO->AllObjRefNumber; ObjRefCounter++)
    {
        if(ObjREf == MO->AllObjRef[ObjRefCounter])
        {
            MO->All_INODE_To_ObjRef[MO->NodeNumber] = ObjRefCounter;
            return(0);
        }
    }

    MO->AllObjRef[MO->AllObjRefNumber] = ObjREf;
    MO->AllObjRefINode[MO->AllObjRefNumber] = MAXnode;
    MO->All_INODE_To_ObjRef[MO->NodeNumber] = MO->AllObjRefNumber;
    MO->AllObjRefNumber++;
    return(0);
}

/*
 ===================================================================================================
    Counter Callback Funciton
 ===================================================================================================
 */
DWORD WINAPI MAD_WorldAdditionalObjRefBuilder(MadOut *MO, INode *MAXnode)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Object          *ObjREf;
	IDerivedObject *DrvdObj;
	char TransformaX[MAD_NAME_LENGHT];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ObjREf = MAXnode->GetObjectRef();
    if(ObjREf == NULL) return(0);
	if (ObjREf -> ClassID() == derivObjClassID)
	{
		DrvdObj = (IDerivedObject *)ObjREf;
		if (DrvdObj -> NumModifiers())
		{
			if (DrvdObj -> GetModifier(0) -> ClassID() == MAD_RadMod_CLASS_ID)
			{
				if (((MAD_RadMod *)DrvdObj -> GetModifier(0))->RadiosityObject)
				{
					sprintf (TransformaX,"%s_%s",(char *)MAXnode->GetName(),"radied");
					MO->AllObjects[MO->AllObjRefNumber + MO->AdditionnalObjects] = (MAD_NodeID *)
						Mesh_To_MAD_Mesh(((MAD_RadMod *)DrvdObj -> GetModifier(0))->RadiosityObject,MO->AllObjects[MO->All_INODE_To_ObjRef[MO->NodeNumber]],((MAD_RadMod *)DrvdObj -> GetModifier(0))->UL_RadiosityObjectSecondRliField,MO->ExportTime);
					strcpy(MO->AllObjects[MO->AllObjRefNumber + MO->AdditionnalObjects] ->Name , TransformaX);
					MO->All_INODE_To_ObjRADIOSITY[MO->NodeNumber] = MO->AllObjRefNumber + MO->AdditionnalObjects;
					MO->AdditionnalObjects++;
				}
				MO->All_INODE_RADIOSITY_RLI[MO->NodeNumber] = (unsigned long)((MAD_RadMod *)DrvdObj -> GetModifier(0))->OriginalsRLI;
			}
		}
	}
    return(0);
}
/*
 ===================================================================================================
    Retreive node index
 ===================================================================================================
 */
unsigned long MadOut::MAX_Node_To_Index(INode *MAXnode)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   INodeCounter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(INodeCounter = 0; INodeCounter < AllNodeNumber; INodeCounter++)
    {
        if(AllINode[INodeCounter] == MAXnode) return INodeCounter;
    }

    /* Come here is a bug */
    return 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
unsigned long MadOut::MAX_Material_To_Index(Mtl *MaxMat)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   MatCounter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(MatCounter = 0; MatCounter < MaterialsNumber; MatCounter++)
    {
        if(AllMaterials[MatCounter] == MaxMat) return MatCounter;
    }

    /* Come here is a bug */
    return 0;
}

/*
 ===================================================================================================
    Counter Callback Funciton
 ===================================================================================================
 */
DWORD WINAPI MAD_HierarchieBuilder(MadOut *MO, INode *MAXnode)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_WorldNode   *MWN;
    Matrix3         ParentMatrixInvert, RelativMatrix;
	Modifier		*pst_Mod;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MWN = &MO->Hierarchie[MO->NodeNumber];
    MWN->Parent = MAD_NULL_INDEX;
    MWN->Target = MAD_NULL_INDEX;
    if(MAXnode->GetParentNode())
        MWN->Parent = MO->MAX_Node_To_Index(MAXnode->GetParentNode());
    if(MAXnode->GetTarget())
        MWN->Target = MO->MAX_Node_To_Index(MAXnode->GetTarget());
    MWN->Object = MO->All_INODE_To_ObjRef[MO->NodeNumber];
	MWN->ObjectWithRadiosity = MAD_NULL_INDEX;
	MWN->RLIOfObject = NULL;
    if(!MO->AllObjects[MWN->Object])
        MWN->Object = MAD_NULL_INDEX;
    MAX_Matrix_To_MAD_Matrix(&MAXnode->GetObjectTM(MO->ExportTime), &MWN->Matrix, MO->ExportTime);
    if(MWN->Parent != MAD_NULL_INDEX)
    {
        ParentMatrixInvert = Inverse(MAXnode->GetParentNode()->GetObjectTM(MO->ExportTime));
        RelativMatrix = ParentMatrixInvert * MAXnode->GetObjectTM(MO->ExportTime);
        MAX_Matrix_To_MAD_Matrix(&RelativMatrix, &MWN->RelativeMatrix, MO->ExportTime);
    }
    else
        MWN->RelativeMatrix = MWN->Matrix;

    MAX_NodeToMAD_Node(MAXnode, &MWN->ID, MO->ExportTime);

	if (MWN->Object != MAD_NULL_INDEX)
		MWN->ObjectWithRadiosity = MO->All_INODE_To_ObjRADIOSITY[MO->NodeNumber];
	if (MWN->Object != MAD_NULL_INDEX)
		MWN->RLIOfObject = (unsigned long *)MO->All_INODE_RADIOSITY_RLI[MO->NodeNumber];

	if (MWN->ObjectWithRadiosity != MAD_NULL_INDEX)
		MWN->NODE_Flags |= MNF_MustBeMadRaded;
	
    MWN->Material = MAD_NULL_INDEX; /* No material */
    if(MAXnode->GetMtl())
        MWN->Material = MO->MAX_Material_To_Index(MAXnode->GetMtl());
    MWN->WiredColor = MAXnode->GetWireColor();

	/******************************************
	 *         Export skin info               *
	 ******************************************/
	pst_Mod = SkinUtils::GetSkin(MAXnode);
	if (pst_Mod && bExportSkin)
	{
		// Export skin info : First construct the MAD_SkinnedGeometricObject object.
		//                    Then change the MO->AllObjects[MWN->Object] with it.

		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MAD_SkinnedGeometricObject	*pst_SknObj;
		ISkin						*pst_SkinI;
		ISkinContextData			*pst_SkinCntxt;
		int							i_NumBones, i_NumVtx, i_NumPond, i_BoneIdx, i_PondListIdx;
		MAD_PonderationList			*pst_PondList;
		float						f_Temp;
		INode						*pst_BoneNode;
		Matrix3						st_TempMat1, st_TempMat2, st_TempMat3;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MAD_MALLOC(MAD_SkinnedGeometricObject, pst_SknObj, 1);
		pst_SknObj->pst_GeoObj = (MAD_GeometricObject *) MO->AllObjects[MWN->Object];
		pst_SknObj->ID.IDType = ID_MAD_SkinnedGeometricObject;
		pst_SknObj->ID.SizeOfThisOne = sizeof(MAD_SkinnedGeometricObject);
		strcpy(pst_SknObj->ID.Name, MO->AllObjects[MWN->Object]->Name);

		// Get interface of modifier
		pst_SkinI = (ISkin *) pst_Mod->GetInterface(I_SKIN);
		if (!pst_SkinI) return(0);
		pst_SkinCntxt = (ISkinContextData *) pst_SkinI->GetContextInterface(MAXnode);
		if (!pst_SkinCntxt) return(0);

		// Create PondLists
		i_NumBones = pst_SkinI->GetNumBones();
		if (!i_NumBones) return(0);
		pst_SknObj->us_NumberOfPonderationLists = i_NumBones;
		MAD_MALLOC(MAD_PonderationList, pst_SknObj->pst_PonderationList, i_NumBones);
		memset( pst_SknObj->pst_PonderationList, 0, i_NumBones * sizeof(MAD_PonderationList) );

		// Fill the pond vertices lists in each of them
		i_NumVtx = pst_SknObj->pst_GeoObj->NumberOfPoints; //pst_SkinCntxt->GetNumPoints();
		for (int i_VtxIdx = 0; i_VtxIdx < i_NumVtx; i_VtxIdx++)
		{
			i_NumPond = pst_SkinCntxt->GetNumAssignedBones(i_VtxIdx);
			if (i_NumPond > 3)
			{
				// TODO : Warning
			}
			for (int i = 0; i < i_NumPond; i++)
			{
				i_BoneIdx = pst_SkinCntxt->GetAssignedBone(i_VtxIdx, i);
				pst_PondList = &pst_SknObj->pst_PonderationList[i_BoneIdx];

				// Update pond list
				pst_PondList->us_NumberOfPonderatedVertices++;
				if (pst_PondList->us_NumberOfPonderatedVertices == 1)
					pst_PondList->p_CmpPdrtVrt =
						(MAD_CompressedPonderatedVertex *) malloc(sizeof(MAD_CompressedPonderatedVertex));
				else
					pst_PondList->p_CmpPdrtVrt =
						(MAD_CompressedPonderatedVertex *) realloc(
						     pst_PondList->p_CmpPdrtVrt,
						     pst_PondList->us_NumberOfPonderatedVertices * sizeof(MAD_CompressedPonderatedVertex));
				pst_PondList->p_CmpPdrtVrt[pst_PondList->us_NumberOfPonderatedVertices - 1].Index = i_VtxIdx;
				f_Temp = pst_SkinCntxt->GetBoneWeight( i_VtxIdx, i );
				pst_PondList->p_CmpPdrtVrt[pst_PondList->us_NumberOfPonderatedVertices - 1].Ponderation =
					*((unsigned short *)&f_Temp + 1);
			}
		}

		// Complete the creation filling us_MatrixIdx and st_FlashedMatrix.
		// And check that there is no empty pond list.
		// WARNING : as pst_SknObj->us_NumberOfPonderationLists can become different from i_NumBones
		i_PondListIdx = 0;
		for (
			i_BoneIdx = 0;
			i_BoneIdx < i_NumBones && i_PondListIdx < pst_SknObj->us_NumberOfPonderationLists;
			i_BoneIdx++)
		{
			pst_PondList = &pst_SknObj->pst_PonderationList[i_PondListIdx];

			// If no pond vert, remove the list
			if (!pst_PondList->us_NumberOfPonderatedVertices)
			{
				// If this is the last pond list, give up skin info export
				if (pst_SknObj->us_NumberOfPonderationLists <= 1) return(0);

				if (i_PondListIdx < pst_SknObj->us_NumberOfPonderationLists - 1)
				{
					memmove(
						pst_SknObj->pst_PonderationList + i_PondListIdx,
						pst_SknObj->pst_PonderationList + i_PondListIdx + 1,
						(pst_SknObj->us_NumberOfPonderationLists - i_PondListIdx -1) * sizeof(MAD_PonderationList));
				}
				pst_SknObj->us_NumberOfPonderationLists--;
				(MAD_PonderationList *) realloc(
						pst_SknObj->pst_PonderationList,
						pst_SknObj->us_NumberOfPonderationLists * sizeof(MAD_PonderationList));
				continue;
			}

			// pst_PondList->us_MatrixIdx = index in MO->hierachy of the corresponding bone.
			pst_BoneNode = pst_SkinI->GetBone(i_BoneIdx);
			pst_PondList->us_MatrixIdx = MO->MAX_Node_To_Index(pst_BoneNode);

			// st_FlashedMatrix
			// With explicit names :
			//    pst_SkinI->GetSkinInitTM(MAXnode, BoneInitMat, true);
			//    InvertSkinInitMat = Inverse( SkinInitMat );
			//    pst_SkinI->GetBoneInitTM(pst_BoneNode, BoneInitMat);
			//    LocalBoneMat = BoneInitMat * InvertSkinInitMat;
			//    st_FlashedMatrix = InvertLocalBoneMat = Inverse( LocalBoneMat );
			pst_SkinI->GetSkinInitTM(MAXnode, st_TempMat1, true);
			st_TempMat2 = Inverse( st_TempMat1 );
			pst_SkinI->GetBoneInitTM(pst_BoneNode, st_TempMat1);
			st_TempMat3 = st_TempMat1 * st_TempMat2;
			st_TempMat1 = Inverse( st_TempMat3 );
			MAX_Matrix_To_MAD_Matrix (
				&st_TempMat1,
				&pst_PondList->st_FlashedMatrix,
				MO->ExportTime);

			i_PondListIdx++;
		}

		MO->AllObjects[MWN->Object] = (MAD_NodeID *)pst_SknObj;
	}

    return(0);
}

/*
 ===================================================================================================
    Counter Callback Function
 ===================================================================================================
 */
DWORD WINAPI MAD_DEBUG_WRITE(MadOut *MO, INode *MAXnode)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_WorldNode   HierarchieNode;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    HierarchieNode = MO->Hierarchie[MO->NodeNumber];

    fprintf(MO->pStream, "Node Number:%d\n", MO->NodeNumber);
    fprintf(MO->pStream, "Node Name:%s\n", HierarchieNode.ID.Name);
    if(HierarchieNode.Object == MAD_NULL_INDEX)
        fprintf(MO->pStream, "No Object \n");
    else
        fprintf(MO->pStream, "Object Name: %s\n", MO->AllObjects[HierarchieNode.Object]->Name);
    if(HierarchieNode.Parent == MAD_NULL_INDEX)
        fprintf(MO->pStream, "No Parent \n");
    else
        fprintf(MO->pStream, "Parent number: %d\n", HierarchieNode.Parent);
    if(HierarchieNode.Target == MAD_NULL_INDEX)
        fprintf(MO->pStream, "No Target\n");
    else
        fprintf(MO->pStream, "Target number: %d\n", HierarchieNode.Target);
    if(HierarchieNode.Material == MAD_NULL_INDEX)
        fprintf(MO->pStream, "No Material \n");
    else
        fprintf(MO->pStream, "Material number: %d\n", HierarchieNode.Material);
    /*$off*/
    fprintf(MO->pStream,"T: %.2f %.2f %.2f\n",HierarchieNode.Matrix.Translation.x,HierarchieNode.Matrix.Translation.y,HierarchieNode.Matrix.Translation.z);
    fprintf(MO->pStream,"I: %.2f %.2f %.2f\n",HierarchieNode.Matrix.I.x,HierarchieNode.Matrix.I.y,HierarchieNode.Matrix.I.z);
    fprintf(MO->pStream,"J: %.2f %.2f %.2f\n",HierarchieNode.Matrix.J.x,HierarchieNode.Matrix.J.y,HierarchieNode.Matrix.J.z);
    fprintf(MO->pStream,"K: %.2f %.2f %.2f\n",HierarchieNode.Matrix.K.x,HierarchieNode.Matrix.K.y,HierarchieNode.Matrix.K.z);
    /*$on*/
    fprintf(MO->pStream, "\n");
    fprintf(MO->pStream, "\n");

    return(0);
}

/*
 ===================================================================================================
    This function will call MAD_NodeEnumCallBack for each node.
 ===================================================================================================
 */
BOOL MadOut::HaveOneChildrenSelected(INode * node)
{
	if (node->Selected()) return TRUE;
    for(int c = 0; c < node->NumberOfChildren(); c++)
	{
        if (HaveOneChildrenSelected(node->GetChildNode(c))) return TRUE;
	}
	return FALSE;
}
BOOL MadOut::nodeEnum(INode *node)
{
    if(ip->GetCancel()) return FALSE;
	if (ExportSelected)
	{
		/* Continue if at least one child is selected */
		if (!HaveOneChildrenSelected(node)) return TRUE;
	}

    if(MAD_NodeEnumCallBack)
        MAD_NodeEnumCallBack(this, node);

    NodeNumber++;
    for(int c = 0; c < node->NumberOfChildren(); c++)
        nodeEnum(node->GetChildNode(c));
    return TRUE;
}

/*
 ===================================================================================================
    This function will enum each node
 ===================================================================================================
 */
BOOL MadOut::DOnodeEnum(INode * node, DWORD(WINAPI * CLBKFunc) (MadOut * MO, INode * node))
{
    NodeNumber = 0;
    MAD_NodeEnumCallBack = CLBKFunc;
    return nodeEnum(ip->GetRootNode());
}

/*
 ===================================================================================================
    DlgCallBack
 ===================================================================================================
 */
static BOOL CALLBACK ExportDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MadOut  *MO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MO = (MadOut *) GetWindowLong(hWnd, GWL_USERDATA);
    switch(msg)
    {
    case WM_INITDIALOG:
        MO = (MadOut *) lParam;
        SetWindowLong(hWnd, GWL_USERDATA, lParam);
        CenterWindow(hWnd, GetParent(hWnd));
        CheckRadioButton
        (
            hWnd,
            IDC_RADIO1,
            IDC_RADIO2,
            MO->ExportSelected ? IDC_RADIO2 : IDC_RADIO1
        );
        if(bInsertTextures)
            CheckDlgButton(hWnd, IDC_CHECKPutTextures, BST_CHECKED);
        else
            CheckDlgButton(hWnd, IDC_CHECKPutTextures, BST_UNCHECKED);
        return TRUE;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            MO->ExportSelected = IsDlgButtonChecked(hWnd, IDC_RADIO2);
            bInsertTextures = IsDlgButtonChecked(hWnd, IDC_CHECKPutTextures);
            EndDialog(hWnd, 1);
            break;
        case IDCANCEL:
            EndDialog(hWnd, 0);
            break;
        }

        break;
    default:
        return FALSE;
    }

    return TRUE;
}

/* Resolve problem of duplicated names */
static unsigned char    MAD_RN_Text[256];
static unsigned char    *NewName;
static unsigned char    *OldName;

/*
 ===================================================================================================
 ===================================================================================================
 */
static BOOL CALLBACK ResolveDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MadOut  *MO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MO = (MadOut *) GetWindowLong(hWnd, GWL_USERDATA);
    switch(msg)
    {
    case WM_INITDIALOG:
        MO = (MadOut *) lParam;
        SetWindowLong(hWnd, GWL_USERDATA, lParam);
        CenterWindow(hWnd, GetParent(hWnd));
        SetDlgItemText(hWnd, IDC_THETEXT, (char *) MAD_RN_Text);
        SetDlgItemText(hWnd, IDC_EDIT1, (char *) NewName);
        return TRUE;
    case WM_COMMAND:
        if(HIWORD(wParam) == EN_CHANGE)
        {
            switch(LOWORD(wParam))
            {
            case IDC_EDIT1:
                GetDlgItemText(hWnd, IDC_EDIT1, (char *) NewName, 256);
                break;
            }
        }

        switch(LOWORD(wParam))
        {
        case IDOK:
            if(!M_IsStringEqual((char *) OldName, (char *) NewName))
            {
                EndDialog(hWnd, 1);
            }

            break;
        }

        break;
    default:
        return FALSE;
    }

    return FALSE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void MadOut::ResolveNameConflict
(
    unsigned char   *Type,
    unsigned char   *SrcToModify,
    unsigned char   *TOldName
)
{
    sprintf
    (
        (char *) MAD_RN_Text,
        "The %s %s have a name which already exist in the scene.\nPlease type a new name for this %s.",
        Type,
        SrcToModify,
        Type
    );
    NewName = SrcToModify;
    OldName = TOldName;
    DialogBoxParam
    (
        hInstance,
        MAKEINTRESOURCE(IDD_DIALOGSAMENAMECONFLICT),
        ip->GetMAXHWnd(),
        ResolveDlgProc,
        (LPARAM) this
    );
}

/*
 * Dialog proc Start the exporter! This is the real entrypoint to the exporter. After the user
 * has selected the filename (and he's prompted for overwrite etc.) this method is called. &
 */
#if defined(MAX3) || defined(MAX5) || defined(MAX7) || defined(MAX8)
int MadOut::
DoExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD Option)
#else
int MadOut::
DoExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts)
#endif
{
    unsigned long   Counter, Counter2;
    Color           Col;

    ip = i;

    if(!suppressPrompts)
    {
        if
        (
            !DialogBoxParam
            (
                hInstance,
                MAKEINTRESOURCE(IDD_MADDialog),
                ip->GetMAXHWnd(),
                ExportDlgProc,
                (LPARAM) this
            )
        ) return 1;
		bExportSkin = DialogBoxParam (
			hInstance,
			MAKEINTRESOURCE(IDD_SKNDLG),
			ip->GetMAXHWnd(),
			SkinDlgProc,
			(LPARAM) this);
    }
    else
    {
        bInsertTextures = suppressPrompts & MADEXP_INCLUDETEX;
		ExportSelected = suppressPrompts & MADEXP_SELONLY;
		bExportSkin = !(suppressPrompts & MADEXP_NOSKIN);
    }
    if(ExportSelected)
	{
		if (!HaveOneChildrenSelected(ip->GetRootNode()))
			ExportSelected = 0;
	}

    ExportTime = ip->GetTime();
    Mad_meminit();

    /* Open the stream */
    pStream = _tfopen(name, _T("wb"));
	if (!pStream)
	{
	    char    FName[300]; /* */
		sprintf(FName , "Could not open %s for writing. \n Abort export." , name);
        MessageBox( ip->GetMAXHWnd() , FName  , TEXT("Write error") , MB_OK | MB_ICONSTOP | MB_TASKMODAL ); 
        return 0;
	}

    /* Startup the progress bar. */
    ip->ProgressStart(_T("Sauvegarde..."), TRUE, fn, NULL);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Compute AllNodeNumber
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    DOnodeEnum(ip->GetRootNode(), NULL);
    AllNodeNumber = NodeNumber;
    ip->ProgressUpdate(10);

    /* Compute AllNodeNumber OK */

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Compute **AllINode
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    MAD_MALLOC(INode *, AllINode, AllNodeNumber);
    DOnodeEnum(ip->GetRootNode(), MAD_WorldNodesStock);
    ip->ProgressUpdate(20);

    /* Compute **AllINode OK */

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Compute AllObjRefNumber + **AllObjRef + All_INODE_To_ObjRef
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    AllObjRefNumber = 0;
    MAD_MALLOC(Object *, AllObjRef, AllNodeNumber);
    MAD_MALLOC(INode *, AllObjRefINode, AllNodeNumber);
    MAD_MALLOC(ULONG, All_INODE_To_ObjRef, AllNodeNumber);
    MAD_MALLOC(ULONG, All_INODE_To_ObjRADIOSITY, AllNodeNumber);
    MAD_MALLOC(ULONG, All_INODE_RADIOSITY_RLI, AllNodeNumber);

	AdditionnalObjects = 0;
    DOnodeEnum(ip->GetRootNode(), MAD_WorldObjRefCounAndStock);
	MAD_MALLOC(MAD_NodeID *, AllObjects, AllObjRefNumber + AdditionnalObjects);
	MW.NumberOfObjects = AllObjRefNumber + AdditionnalObjects;
    MW.AllObjects = AllObjects;
	/* Load (MAX to MAD struct) **AllObjects */

	// DEBUG
	FILE *flog = _tfopen("log.txt", _T("wt"));
#ifdef MAX7
	fprintf(flog, "MAX 7\n\n");
#endif
#ifdef MAX8
	fprintf(flog, "MAX 8\n\n");
#endif
	fclose(flog);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Compute AllObjects
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    for(Counter = 0; Counter < AllObjRefNumber; Counter++)
    {
		flog = _tfopen("log.txt", _T("at"));
		fprintf(flog, "***********Next Obj\n");
		fclose(flog);
        AllObjects[Counter] = MAX_X_To_MAD_X(AllObjRefINode[Counter], NULL, ExportTime);
		flog = _tfopen("log.txt", _T("at"));
		fprintf(flog, "***********End Obj\n");
		fclose(flog);
    }
	AdditionnalObjects = 0;
	DOnodeEnum(ip->GetRootNode(), MAD_WorldAdditionalObjRefBuilder);

    ip->ProgressUpdate(30);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Compute MaximumMaterials , MaterialsNumber & AllMaterials
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    MaximumMaterials = 0;
    DOnodeEnum(ip->GetRootNode(), MAD_WorldMaxMaterialCount);
    MAD_MALLOC(Mtl *, AllMaterials, MaximumMaterials);
    MaterialsNumber = 0;
    DOnodeEnum(ip->GetRootNode(), MAD_WorldMaterialBuilder);

    /* Compute MaximumMaterials OK */

    ip->ProgressUpdate(40);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Compute Hierarchie
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    MAD_MALLOC(MAD_WorldNode, Hierarchie, AllNodeNumber);
    MW.NumberOfHierarchieNodes = AllNodeNumber;
    MW.Hierarchie = Hierarchie;
    DOnodeEnum(ip->GetRootNode(), MAD_HierarchieBuilder);
    ip->ProgressUpdate(50);

    /* Compute Hierarchie OK */

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Comput mad world
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    MW.NumberOfMaterials = MaterialsNumber;
    MAD_MALLOC(MAD_MAT_MatID *, MW.AllMaterial, MaterialsNumber);
    MW.NumberOftexture = NULL;
    MW.AllTextures = NULL;
    MW.NumberOfObjects = AllObjRefNumber+ AdditionnalObjects;
    MW.AllObjects = AllObjects;
    MW.NumberOfHierarchieNodes = AllNodeNumber;
    MW.Hierarchie = Hierarchie;
    MW.ID.IDType = ID_MAD_World;
    MW.ID.SizeOfThisOne = sizeof(MW);
    MW.ID.Name[0] = 0;
	MW.MAD_Version = MAD_WORLD_VERION_0;
    ip->ProgressUpdate(60);

    /* Comput mad world OK */

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Convert all Max matrerial to mad materials (also compute (number of texture)
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    MAX_Mat_To_MAD_Mat(&MW, AllMaterials, ExportTime);
    ip->ProgressUpdate(70);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Convert all Max Texture to mad Texture
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    for(Counter = 0; Counter < MW.NumberOftexture; Counter++)
        MW.AllTextures[Counter] = MAX_Tex_To_MAD_Tex
            (
                (Texmap *) (MW.AllTextures[Counter]),
                ExportTime
            );
    ip->ProgressUpdate(80);

    /* Convert all Max Texture to mad Texture OK */

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Globals
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    Col = ip->GetAmbient(ExportTime, FOREVER);
    MAX_Color_To_MAD_Color(&Col, &MW.AmbientColor);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Check for existant names resolve names conflicts;
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    {
        long    AsLong;
        AsLong = 1;

        /*
         * Sorry for the following square computation, but I think that it will not be used on
         * important numbers
         */

        /* For hierarchie */
        if(AllNodeNumber)
        {
            while(AsLong)
            {
                AsLong = 0;
                for(Counter = 0; Counter < AllNodeNumber - 1; Counter++)
                {
                    for(Counter2 = Counter + 1; Counter2 < AllNodeNumber; Counter2++)
                    {
                        if
                        (
                            M_IsStringEqual
                            (
                                Hierarchie[Counter].ID.Name,
                                Hierarchie[Counter2].ID.Name
                            )
                        )
                        {
                            ResolveNameConflict
                            (
                                (unsigned char *) "node",
                                (unsigned char *) Hierarchie[Counter2].ID.Name,
                                (unsigned char *) Hierarchie[Counter].ID.Name
                            );
                            AsLong = 1;
                        }
                    }
                    }
            }
        }

        /* For materials */
        AsLong = 1;
        if(MW.NumberOfMaterials)
        {
            while(AsLong)
            {
                AsLong = 0;
                for(Counter = 0; Counter < MW.NumberOfMaterials - 1; Counter++)
                {
                    for(Counter2 = Counter + 1; Counter2 < MW.NumberOfMaterials; Counter2++)
                    {
                        if
                        (
                            M_IsStringEqual
                            (
                                MW.AllMaterial[Counter]->Name,
                                MW.AllMaterial[Counter2]->Name
                            )
                        )
                        {
                            ResolveNameConflict
                            (
                                (unsigned char *) "material",
                                (unsigned char *) MW.AllMaterial[Counter2]->Name,
                                (unsigned char *) MW.AllMaterial[Counter]->Name
                            );
                            AsLong = 1;
                        }
                    }
                    }
            }
        }
    }

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        SaveAll
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    ip->ProgressUpdate(80);
    if(bInsertTextures)
        MAD_Save(&MW, pStream, MAD_SaveTextureFiles | MAD_RelativeTexFileName);
    else
        MAD_Save(&MW, pStream, MAD_RelativeTexFileName);
    ip->ProgressUpdate(90);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Free all
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    MAD_FREE();

    /* We're done. Finish the progress bar. */
    ip->ProgressEnd();

    /* Close the stream */
    fclose(pStream);

    return 1;
}

BOOL CALLBACK MadOut::SkinDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch (msg)
    {
    case WM_INITDIALOG:
        CenterWindow(hWnd, GetParent(hWnd));
        break;

    case WM_DESTROY:
		EndDialog(hWnd, 1);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_SKNYES:
			EndDialog(hWnd, 1);
            break;
        case IDC_SKNNO:
            EndDialog(hWnd, 0);
            break;
        }
        break;

	default:
        return TRUE;
    }
    return TRUE;
}
