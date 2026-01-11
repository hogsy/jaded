/*$T MAD_import.cpp GC!1.32 05/27/99 15:20:39 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    MAD_Import.cpp - Ouf File Importer By Philippe Vimont UBI Pictures mai 99
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "MAD_mem/Sources/MAD_mem.h"
#include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#include "Max.h"
#include "stdmat.h"
#include "MAD_Import.h"
#include "MAD_Export/Sources/resource.h"
#include "MADCDLG.h"
#include "Dummy.h"
#include "modstack.h"

#include "iparamm2.h"
#include "iskin.h"

#include "SkinUtils.h"
#include "ImportExportDef.h"
//#include "MixMat.h"

#include "MAD_RADMOD/MAD_RADMOD.h"

/*$off*/
/*
 ===================================================================================================
	Max compatibility stuff 
 ===================================================================================================
*/
static HINSTANCE   hInstance;
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{   hInstance = hinstDLL;
    return(TRUE);
}

#define MadIn_CLASS_ID Class_ID(0xeba6838, 0x23ff56d3)
class MadImpClassDesc : public ClassDesc
{
public:
    int         IsPublic(void)                  { return 1; }
    void        *Create(BOOL loading = FALSE)   { return new MadIn; }
    const TCHAR *ClassName(void)                { return _T("MadIn"); }
    SClass_ID   SuperClassID(void)              { return SCENE_IMPORT_CLASS_ID; }
    Class_ID    ClassID(void)                   { return MadIn_CLASS_ID; }
    const TCHAR *Category(void)                 { return _T("MAD Utility"); }
};

static MadImpClassDesc MadImpDesc;
ClassDesc *GetMadInDesc(void)              {return &MadImpDesc;}

__declspec(dllexport)   const TCHAR *LibDescription(void)           {return "Qu'est ce que je vais bouffer ce midi ? ";}
__declspec(dllexport)   int LibNumberClasses(void)                  {return 1;}
__declspec(dllexport)   ClassDesc *LibClassDesc(int i)              {return GetMadInDesc();}
__declspec(dllexport)   ULONG LibVersion(void)                      {return VERSION_3DSMAX;}
             MadIn::~MadIn(void)          {}
int          MadIn::ExtCount(void)         {return 1;}
const TCHAR *MadIn::Ext(int n)             {return _T("MAD");}
const TCHAR *MadIn::LongDesc(void)         {return _T("MAD Import");}
const TCHAR *MadIn::ShortDesc(void)        {return _T("MAD");}
const TCHAR *MadIn::AuthorName(void)       {return _T("Philippe Vimont");}
const TCHAR *MadIn::CopyrightMessage(void) {return _T("UBI PICTURE 1999");}
const TCHAR *MadIn::OtherMessage1(void)    {return _T("Aglaaaaa glaaaaaaaaaaaaaa!! ");}
const TCHAR *MadIn::OtherMessage2(void)    {return _T("shbrumf");}
unsigned int MadIn::Version(void)          {return 100;}
void            MadIn::ShowAbout(HWND hWnd)   {}
DWORD WINAPI fn (LPVOID arg)                {return(0);}
/*
 ===================================================================================================
	End of Max compatibility stuff 
 ===================================================================================================
*/
/*$on*/
/*
 ===================================================================================================
 function for enble - disable windows buttons
 ===================================================================================================
 */
void MAD_EnableWindow(HWND hwndDlg,unsigned long ED )
{
    DWORD dwStyle ;
    dwStyle = GetWindowLong(hwndDlg , GWL_STYLE);  
    if (!ED)
        dwStyle |= WS_DISABLED;
    else
        dwStyle &= ~WS_DISABLED;
    SetWindowLong(hwndDlg , GWL_STYLE, dwStyle );
    ShowWindow(hwndDlg,SW_HIDE );
    ShowWindow(hwndDlg,SW_SHOW );
}
/*
 ===================================================================================================
 ===================================================================================================
 */
MadIn::MadIn(void)
{
    MERGE = FALSE;
    TexturePATH[0] = 0;

	mb_SetFullOpacity = FALSE;
	mb_ImportMultiTexMat = FALSE;
}



/*
 ===================================================================================================
    DlgCallBack
 ===================================================================================================
 */
static BOOL CALLBACK ImportDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MadIn   *MO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MO = (MadIn *) GetWindowLong(hWnd, GWL_USERDATA);
    switch(msg)
    {
    case WM_INITDIALOG:
        MO = (MadIn *) lParam;
        SetWindowLong(hWnd, GWL_USERDATA, lParam);
        CenterWindow(hWnd, GetParent(hWnd));
        CheckRadioButton
        (
            hWnd,
            IDC_RADIOREPLACE,
            IDC_RADIOMERGE,
            MO->MERGE ? IDC_RADIOMERGE : IDC_RADIOREPLACE
        );
        MO->MERGETEXTURES = TRUE;
        CheckDlgButton( hWnd, IDC_CHECKInporteTextures , MO->MERGETEXTURES ? BST_CHECKED : BST_UNCHECKED);
        return TRUE;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            MO->MERGETEXTURES = IsDlgButtonChecked(hWnd, IDC_CHECKInporteTextures );
            MO->MERGE = IsDlgButtonChecked(hWnd, IDC_RADIOMERGE);
            EndDialog(hWnd, 1);
            break;
        case IDCANCEL:
            EndDialog(hWnd, 0);
            break;
        case IDC_CHECKInporteTextures:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                if (MO->MERGETEXTURES)
                    MO->MERGETEXTURES = FALSE;
                else
                    MO->MERGETEXTURES = TRUE;
                MAD_EnableWindow(GetDlgItem(hWnd,IDC_BUTTONSETITDIR),MO->MERGETEXTURES);
            }
            break;
        case IDC_BUTTONSETITDIR:
            fn_SH_bBrowseForFolder( /*hWnd, */"Choose an extract texture directory ", MO->TexturePATH );
            break;

        }

        break;
    default:
        return FALSE;
    }

    return TRUE;
}

/*$5
 ###################################################################################################
    Colors
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
void MadIn::MAD_Color_To_MAX_Color(MAD_ColorARGB *MadColor, Color *MaxColor)
{
#if  defined(MAX7) || defined(MAX8)
	MaxColor->r = (((*MadColor) & 0x00FF0000) >> 16) / 255.0f;
	MaxColor->g = (((*MadColor) & 0x0000FF00) >>  8) / 255.0f;
	MaxColor->b = (((*MadColor) & 0x000000FF) >>  0) / 255.0f;
#else
    *MaxColor = *MadColor;
#endif
};

/*$5
 ###################################################################################################
    Matrix
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
void MadIn::MAD_Matrix_To_MAX_Matrix(MAD_Matrix *MadMatrix, Matrix3 *MaxMAt)
{
    MaxMAt->IdentityMatrix();
    MaxMAt->SetTrans( MAD_SCALE_IMPORT_FACTOR * *(Point3 *) &MadMatrix->Translation);
    MaxMAt->SetRow(0, *(Point3 *) &MadMatrix->I);
    MaxMAt->SetRow(1, *(Point3 *) &MadMatrix->J);
    MaxMAt->SetRow(2, *(Point3 *) &MadMatrix->K);
};

/*$5
 ###################################################################################################
    Textures
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
Texmap *MadIn::MAD_Tex_To_MAX_Tex(MAD_texture *MadTexture)
{
	BitmapTex *bmt = NewDefaultBitmapTex();
    bmt->SetMapName(MadTexture -> Texturefile);
    return (Texmap *)bmt;
}

/*$5
 ###################################################################################################
    Geometric Object
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
Object *MadIn::MAD_Object_To_MAX_Object(MAD_GeometricObject *MADMesh)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Face        *Face;
    unsigned long   MaterialID, Counter , Nothing;
    TriObject       *object;
    Mesh            *MAXMesh;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    object = CreateNewTriObject();
	if(!object)
		return NULL;
#if defined(MAX3) || defined(MAX5) || defined(MAX7) || defined(MAX8)
    MAXMesh = &object->GetMesh();
#else
	MAXMesh = &object->Mesh();
#endif

    MAXMesh->setNumVerts(MADMesh->NumberOfPoints);
    for(Counter = 0; Counter < MADMesh->NumberOfPoints; Counter++)
    {
        MAXMesh->setVert(Counter, MAD_SCALE_IMPORT_FACTOR * *(Point3 *) &MADMesh->OBJ_PointList[Counter].Point);
    }

    MAXMesh->setNumTVerts(MADMesh->NumberOfUV);
    for(Counter = 0; Counter < MADMesh->NumberOfUV; Counter++)
    {
        MAXMesh->setTVert(Counter, *(Point3 *) &MADMesh->OBJ_UVtextureList[Counter]);
    }

    MAXMesh->setNumFaces(MAD_GetNumberOfFace(MADMesh));
    if (MADMesh->NumberOfUV)
        MAXMesh->setNumTVFaces(MAD_GetNumberOfFace(MADMesh));
    if (MADMesh->Colors)
	{
		MAXMesh->setNumVertCol(MADMesh->NumberOfPoints);
		MAXMesh->setNumVCFaces(MAD_GetNumberOfFace(MADMesh));
		for(Counter = 0; Counter < MADMesh->NumberOfPoints; Counter++)
		{
			float Swap;
			MAD_Vertex Convert;
			MAD_ColorARGB_to_MAD_Vertex(Convert,MADMesh->Colors[Counter]);
			Convert . x *= 1.0f / 256.0f;
			Convert . y *= 1.0f / 256.0f;
			Convert . z *= 1.0f / 256.0f;
			MAXMesh->vertCol[Counter] = *(Point3 *) &Convert;
			Swap = MAXMesh->vertCol[Counter].x;
			MAXMesh->vertCol[Counter].x = MAXMesh->vertCol[Counter].z;
			MAXMesh->vertCol[Counter].z = Swap;
		}
	}
    for(Counter = 0; Counter < (unsigned long) MAXMesh->numFaces; Counter++)
    {
        MAD_GetFace(MADMesh, Counter, &Face,&Nothing, &MaterialID);
        MAXMesh->faces[Counter].setVerts(Face->Index);
        MAXMesh->faces[Counter].setSmGroup(Face->SmoothingGroup);
        MAXMesh->faces[Counter].flags = Face->MAXflags;
	    if (MADMesh->Colors)
		{
	        MAXMesh->vcFace[Counter].t[0] = Face->Index[0];
	        MAXMesh->vcFace[Counter].t[1] = Face->Index[1];
	        MAXMesh->vcFace[Counter].t[2] = Face->Index[2];
		}

        if (MADMesh->NumberOfUV)
        {
            MAXMesh->tvFace[Counter].t[0] = Face->UVIndex[0];
            MAXMesh->tvFace[Counter].t[1] = Face->UVIndex[1];
            MAXMesh->tvFace[Counter].t[2] = Face->UVIndex[2];
        }
    }



    MAXMesh->buildNormals();
    MAXMesh->EnableEdgeList(1);
    return (Object *) object;
}

/*$5
 ###################################################################################################
    Lights
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
Object *MadIn::MAD_Light_To_MAX_Light(MAD_Light *MADLight)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GenLight    *light;
    Color       LightCol;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MAD_Color_To_MAX_Color(&MADLight->LightColor, &LightCol);
    switch(MADLight->LightType)
    {
    case MAD_LightOmni:
        light = ii->CreateLightObject(OMNI_LIGHT);
        light->SetUseLight(MADLight->IsOn);
		light->SetAttenNearDisplay(TRUE);
        light->SetSpotShape(CIRCLE_LIGHT);

        light->SetAtten(ImportTime, ATTEN_START, MADLight->Omni.Near * MAD_SCALE_IMPORT_FACTOR);
        light->SetAtten(ImportTime, ATTEN_END  , MADLight->Omni.Far  * MAD_SCALE_IMPORT_FACTOR);
        light->SetAtten(ImportTime, ATTEN1_START, MADLight->Omni.RadiusOfLight * MAD_SCALE_IMPORT_FACTOR);
        light->SetAtten(ImportTime, ATTEN1_END  , MADLight->Omni.RadiusOfLight * MAD_SCALE_IMPORT_FACTOR);
		light->SetAttenNearDisplay(TRUE);
		light->SetUseAtten(TRUE);

        light->SetAttenDisplay(1);
        break;
    case MAD_LightSpot:
        light = ii->CreateLightObject(FSPOT_LIGHT);
        light->SetUseLight(MADLight->IsOn);
        light->SetAtten(ImportTime, ATTEN_START, MADLight->Spot.Near * MAD_SCALE_IMPORT_FACTOR);
        light->SetAtten(ImportTime, ATTEN_END  , MADLight->Spot.Far  * MAD_SCALE_IMPORT_FACTOR);
        light->SetHotspot(ImportTime, MADLight->Spot.LittleAlpha * 180.0f / 3.1415927f);
        light->SetFallsize(ImportTime, MADLight->Spot.BigAlpha   * 180.0f / 3.1415927f);

        light->SetAtten(ImportTime, ATTEN1_START, MAD_SCALE_IMPORT_FACTOR * MADLight->Spot.RadiusOfLight);
        light->SetAtten(ImportTime, ATTEN1_END  , MAD_SCALE_IMPORT_FACTOR * MADLight->Spot.RadiusOfLight);
		light->SetAttenNearDisplay(TRUE);
		light->SetUseAtten(TRUE);

        light->SetSpotShape(CIRCLE_LIGHT);
        light->SetAttenDisplay(1);
        break;
    case MAD_LightDirect:
        light = ii->CreateLightObject(DIR_LIGHT);
        light->SetUseLight(MADLight->IsOn);
        light->SetSpotShape(CIRCLE_LIGHT);
		light->SetAttenNearDisplay(TRUE);
        break;
    }
	if (MADLight -> LightFlags & MAD_LightFlag_CastShadow)
	{
		light->SetShadow(TRUE);
	}
	else
	{
		light->SetShadow(FALSE);
	}

	if (MADLight -> LightFlags & MAD_LightFlag_OverWalls)
	{
		light->SetUseGlobal(TRUE);
	}
	else
	{
		light->SetUseGlobal(FALSE);
	}

	if (MADLight -> LightFlags & MAD_LightFlag_PaintLight)
	{
		light->SetConeDisplay(1, FALSE);
	}
	else
	{
		light->SetConeDisplay(0, FALSE);
	}

	
	light->SetRGBColor(ImportTime, *(Point3 *)&LightCol );
	light->SetIntensity(ImportTime, MADLight -> Multiplier);
    light->Enable(TRUE);
    return (Object *) light;
}

/*$5
 ###################################################################################################
    Camera
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
Object *MadIn::MAD_Cam_To_MAX_Cam(MAD_Camera *MADCam)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GenCamera   *cam;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    switch(MADCam->CameraType)
    {
    case MAD_e_OrthoCamera:
        cam = ii->CreateCameraObject(FREE_CAMERA);
        cam->SetOrtho(TRUE);
        cam->SetFOV(ImportTime, MAD_SCALE_IMPORT_FACTOR * MADCam->OrthoInf.ChannelSize * 3.141592765f / 180.0f);
        cam->SetEnvRange(ImportTime, ENV_NEAR_RANGE, MAD_SCALE_IMPORT_FACTOR * MADCam->OrthoInf.Znear);
        cam->SetEnvRange(ImportTime, ENV_FAR_RANGE, MAD_SCALE_IMPORT_FACTOR * MADCam->OrthoInf.Zfar);
        break;
    case MAD_e_PerspCamera:
        cam = ii->CreateCameraObject(FREE_CAMERA);
        cam->SetOrtho(FALSE);
        cam->SetFOV(ImportTime, MADCam->PerspInf.Fov);
        cam->SetEnvRange(ImportTime, ENV_NEAR_RANGE, MAD_SCALE_IMPORT_FACTOR * MADCam->PerspInf.Znear);
        cam->SetEnvRange(ImportTime, ENV_FAR_RANGE, MAD_SCALE_IMPORT_FACTOR * MADCam->PerspInf.Zfar);
        break;
    }

    cam->Enable(TRUE);
    return (Object *) cam;
}

/*$5
 ###################################################################################################
    Dummy
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
Object *MadIn::MAD_Dum_To_MAX_Dum(MAD_NodeID *MADNode)
{
	Object *Helper;
	Box3	Box;
	switch (((MAD_Dummy *)MADNode) -> Type )
	{
	default:
	case DUMMY_POINT:
		Helper = (Object *)ii->Create(HELPER_CLASS_ID,Class_ID(POINTHELP_CLASS_ID,0));
		break;
	case DUMMY_BOX:
		Helper = (Object *)ii->Create(HELPER_CLASS_ID,Class_ID(DUMMY_CLASS_ID,0));
		Box = Box3( MAD_SCALE_IMPORT_FACTOR * *(Point3 *)&((MAD_Dummy *)MADNode) -> BOXMin , MAD_SCALE_IMPORT_FACTOR * *(Point3 *)&((MAD_Dummy *)MADNode) -> BOXMax);
		((DummyObject *)Helper) -> SetBox(Box);
		break;
	case DUMMY_BONE:
		Helper = (Object *)ii->Create(HELPER_CLASS_ID,Class_ID(BONE_CLASS_ID,0));
		
		break;
	}
    return Helper;
}

/*$5
 ###################################################################################################
    Shapes
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
Object *MadIn::MAD_Shape_To_MAX_Shape(MAD_NodeID *MADNode)
{
    return ii->CreateTargetObject();
}

/*$5
 ###################################################################################################
    Targets
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
Object *MadIn::MAD_Target_To_MAX_Target(MAD_NodeID *MADNode)
{
    return ii->CreateTargetObject();
}

/*$5
 ###################################################################################################
    Materials
 ###################################################################################################
 */

/*
 ===================================================================================================
    Complex materials
 ===================================================================================================
 */
Mtl *MadIn::MAD_Cplx_MAT_To_MAX_Cplx_Mat(MAD_MultiMaterial *MadMat)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MultiMtl        *ret;
    unsigned long   Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(MadMat->MatRef.MaterialType != ID_MAT_MultiMaterial) return NULL;
    ret = NewDefaultMultiMtl();
    ret->SetName(MadMat->MatRef.Name);
    ret->SetNumSubMtls(MadMat->NumberOfSubMaterials);
    for(Counter = 0; Counter < MadMat->NumberOfSubMaterials; Counter++)
    {
		if(MadMat->SubMats[Counter] != 0xffffffff)
			if(AllMaterials[MadMat->SubMats[Counter]])
				ret->SetSubMtl(Counter, AllMaterials[MadMat->SubMats[Counter]]);
    }

    return ret;
}

/*
 ===================================================================================================
    Multi textures materials
 ===================================================================================================
 */
//Mtl *MadIn::MAD_MultiTex_MAT_To_MAX_MultiTex_Mat(MAD_MultiTexMaterial *MadMat)
//{
//    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//	MixMatClassDesc		cd;
//	MixMat				*MTMat;
//	Mtl					*SubMat;
//	MAD_StandarMaterial	MadStdMat;
//    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
//	MTMat = (MixMat *)cd.Create(FALSE);
//	MTMat->SetName(MadMat->MatRef.Name);
//
//	/*******************************************************************************/
//	/* First version of the code. We only take the first two textures into account */
//	/*******************************************************************************/
//
//	if (!MadMat->NumberOfLevels) return (Mtl *)MTMat;
//
//	// This should work with actual version (MAD_WORLD_VERION_2)
//	memcpy (&MadStdMat, MadMat, sizeof(MAD_StandarMaterial));
//	MadStdMat.MatRef.MaterialType = ID_MAT_Standard;
//	MadStdMat.Undefined0 = 0;
//	MadStdMat.Undefined1 = 0;
//	MadStdMat.Undefined2 = 0;
//	MadStdMat.Undefined3 = 0;
//	MadStdMat.Undefined4 = 0;
//	MadStdMat.Undefined5 = 0;
//	MadStdMat.Undefined6 = 0;
//
//	if (MadMat->AllLevels[0].s_TextureId != MAD_NULL_INDEX
//		&& MadMat->AllLevels[0].s_TextureId < MW->NumberOftexture)
//	{
//		memcpy (MadStdMat.MatRef.Name, MW->AllTextures[MadMat->AllLevels[0].s_TextureId]->Texturefile, 256);
//		MadStdMat.MatRef.Name[255] = 0;
//		MadStdMat.MadTexture = MadMat->AllLevels[0].s_TextureId;
//		SubMat = MAD_Simple_MAT_To_MAX_Simple_Mat(&MadStdMat);
//
//		MTMat->SetSubMtl(0, SubMat);
//	}
//
//	if (MadMat->NumberOfLevels == 1) return MTMat;
//
//	if (MadMat->AllLevels[1].s_TextureId != MAD_NULL_INDEX
//		&& MadMat->AllLevels[1].s_TextureId < MW->NumberOftexture)
//	{
//		memcpy (MadStdMat.MatRef.Name, MW->AllTextures[MadMat->AllLevels[1].s_TextureId]->Texturefile, 256);
//		MadStdMat.MatRef.Name[255] = 0;
//		MadStdMat.MadTexture = MadMat->AllLevels[1].s_TextureId;
//		SubMat = MAD_Simple_MAT_To_MAX_Simple_Mat(&MadStdMat);
//
//		MTMat->SetSubMtl(1, SubMat);
//	}
//
//	// Test
//	NameData nd;
//	nd.channel = 5;
//	nd.subid = 2;
//	VCol *mask = new VCol;
//	mask->mapID = 3;
//	mask->subID = 4;
//	//mask->pblock->SetValue(0, 0, 0);
//	//mask->pblock->SetValue(0, 0, 1);
//	mask->channelData.Append(1, &nd, 1);
//	MTMat->SetSubTexmap(0, mask);
//
//	return MTMat;
//}

/*
 ===================================================================================================
    Simple materials
 ===================================================================================================
 */
Mtl *MadIn::MAD_Simple_MAT_To_MAX_Simple_Mat(MAD_StandarMaterial *MadMat)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    StdMat  *ret;
    Color   CvrtCol;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(MadMat->MatRef.MaterialType != ID_MAT_Standard) return NULL;
    ret = NewDefaultStdMat();
    ret->SetName(MadMat->MatRef.Name);
    MAD_Color_To_MAX_Color(&MadMat->Diffuse, &CvrtCol);
    ret->SetDiffuse(CvrtCol, ImportTime);
    MAD_Color_To_MAX_Color(&MadMat->Ambient, &CvrtCol);
    ret->SetAmbient(CvrtCol, ImportTime);
    MAD_Color_To_MAX_Color(&MadMat->Specular, &CvrtCol);
    ret->SetSpecular(CvrtCol, ImportTime);
    if(mb_SetFullOpacity)
		ret->SetOpacity(1.0f, ImportTime);
	else
		ret->SetOpacity(MadMat->Opacity, ImportTime);
    ret->GetSelfIllum((int) MadMat->SelfIllum);
    if (MadMat->MadTexture != MAD_NULL_INDEX)
    {
        if(AllTexture[MadMat->MadTexture])
        {
            ret->EnableMap(ID_DI, TRUE);

            ret->SetSubTexmap(ID_DI, AllTexture[MadMat->MadTexture]);
            ret->SetActiveTexmap( AllTexture[MadMat->MadTexture]);
            ret->SetMtlFlag(MTL_TEX_DISPLAY_ENABLED);
        }
    }
    switch(MadMat->MaterialFlag & MAD_MTF_TransparencyModeMask)
    {
    case MAD_MTF_Trans_Copy:
        break;
    case MAD_MTF_Trans_AddSmooth:
    case MAD_MTF_Trans_Add:
        ret->SetTransparencyType(TRANSP_ADDITIVE);
        break;
    case MAD_MTF_Trans_Alpha:
    case MAD_MTF_Trans_AlphaPremult:
        break;
    case MAD_MTF_Trans_Invert:
    case MAD_MTF_Trans_Mul:
    case MAD_MTF_Trans_Mul2X:
        ret->SetTransparencyType(TRANSP_SUBTRACTIVE);
        break;
    }

    return (Mtl *) ret;
}

/*$5
 ###################################################################################################
    Main converter
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
Object *MadIn::MAD_X_To_MAX_X(MAD_NodeID *MADNode)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Object      *Ret;
    ObjectState os;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Ret = NULL;
    switch(MADNode->IDType)
    {
    case ID_MAD_GeometricObject_V0:
        Ret = (Object *) MAD_Object_To_MAX_Object((MAD_GeometricObject *) MADNode);
        break;
    case ID_MAD_SkinnedGeometricObject:
		Ret = (Object *) MAD_Object_To_MAX_Object(((MAD_SkinnedGeometricObject *) MADNode)->pst_GeoObj);
        break;
    case ID_MAD_Light:
        Ret = (Object *) MAD_Light_To_MAX_Light((MAD_Light *) MADNode);
        break;
    case ID_MAD_Camera:
        Ret = (Object *) MAD_Cam_To_MAX_Cam((MAD_Camera *) MADNode);
        break;
    case ID_MAD_Dummy:
        Ret = (Object *) MAD_Dum_To_MAX_Dum(MADNode);
        break;
    case ID_MAD_Shape:
        Ret = (Object *) MAD_Shape_To_MAX_Shape(MADNode);
        break;
    case ID_MAD_TargetObject:
    default:
        Ret = (Object *) MAD_Target_To_MAX_Target(MADNode);
        break;
    }

    return Ret;
}
/*$5
 ###################################################################################################
    Import textures files management
 ###################################################################################################
 */

/*
 ===================================================================================================
    ASk user replace, rename or skip
 ===================================================================================================
 */
static BOOL CALLBACK ReplaceDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MadIn   *MO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MO = (MadIn *) GetWindowLong(hWnd, GWL_USERDATA);
    switch(msg)
    {
    case WM_INITDIALOG:
        MO = (MadIn *) lParam;
        SetWindowLong(hWnd, GWL_USERDATA, lParam);
        CenterWindow(hWnd, GetParent(hWnd));
        SetDlgItemText(hWnd , IDC_STATICTEXTUREFILENAME , MO -> CurrentTextureName);
        CheckDlgButton( hWnd, IDC_CHECKApplyToAll , BST_UNCHECKED  );
        MAD_EnableWindow(GetDlgItem(hWnd,IDC_BUTTONSETITDIR),0);
        return TRUE;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDC_BUTTONreplaceyes:
            MO -> ASKUSERFORTEXTURECONFLICT = !IsDlgButtonChecked(hWnd, IDC_CHECKApplyToAll);
            MO -> ReplaceOption = 0;
            EndDialog(hWnd, 1);
            break;
        case IDC_BUTTONRename:
            MO -> ASKUSERFORTEXTURECONFLICT = !IsDlgButtonChecked(hWnd, IDC_CHECKApplyToAll);
            MO -> ReplaceOption = 1;
            EndDialog(hWnd, 1);
            break;
        case IDC_BUTTONreplaceno:
            MO -> ASKUSERFORTEXTURECONFLICT = !IsDlgButtonChecked(hWnd, IDC_CHECKApplyToAll);
            MO -> ReplaceOption = 2;
            EndDialog(hWnd, 1);
            break;
        }

        break;
    default:
        return FALSE;
    }
    return TRUE;
}
/*
 ===================================================================================================
Read a file choose by user
use like this
    char name [256];
    name = Default nema;
    Retrun 0 if failed
 ===================================================================================================
 */
unsigned long CDLG_USR_GetFileName_Save(char *Name, HWND MUM)
{
    OPENFILENAME ofn;
    char Convert[32];
    char DirectoryName[260];
    char FileName[260];
    char *FilterSRC;
    memset(Convert,0,32);
    FilterSRC = Name;
    /* find the end of Name */
    while (*(FilterSRC++)) ; 
    /* find Name extension */
    while ((*(FilterSRC) != '.') && (FilterSRC > Name ))
        FilterSRC--;
    /* write specific filter info */
    if (*(FilterSRC) == '.')
        sprintf(Convert , "%s Image File|*.%s" , FilterSRC+1 , FilterSRC+1);
    FilterSRC = Convert;
    while (*FilterSRC)
    {
        if (*FilterSRC == '|')
        *FilterSRC = 0;
        FilterSRC++;
    } 
    *(FilterSRC+1) = 0;
    /* compute Name & dir */
    MAD_SeparateDirNameFromName(Name,DirectoryName,FileName);
    /* call windows fonction */
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = MUM;
    ofn.hInstance = NULL;
    ofn.lpstrFilter = Convert;
    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = FileName;
    ofn.nMaxFile = 256;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrInitialDir = DirectoryName;
    ofn.lpstrTitle = "Rename texture file";
    ofn.Flags = OFN_NOCHANGEDIR ;
    ofn.lpstrDefExt = NULL;
    if (!GetSaveFileName(&ofn))
        return 0;
    strcpy(Name,FileName);
    return 1;
}

/*
 ===================================================================================================
    LoadTextureFilesCallback
    The importer have to handle this callback. Name should be modified if file name change.
 ===================================================================================================
 */
void LoadTextureFilesCallback(FILE *f,int size,char *Name,int P)
{
    MadIn   *MO;
    FILE *dstFILE;
    BOOL    CouldBeOpenForRead;
    BOOL    CouldBeOpenForWrite;
    char    SaveName[1024]; /* */
    char    DirectoryName[260];
    char    FileName[260];

    MO = (MadIn *)P;
    MO -> CurrentTextureName = Name;
    if ( MO -> TexturePATH[0] != 0) 
        /* User had selected an import directory */
    {
        MAD_SeparateDirNameFromName(Name,DirectoryName,FileName);
        sprintf(Name , "%s%s" , MO -> TexturePATH , FileName);
    }

    dstFILE = fopen(Name,"rb");
    if (dstFILE)
    {
        CouldBeOpenForRead = TRUE;
        /* the file is existing */ 
        if (FileEqual(f,dstFILE,size))
        {
            /* the files are the sames */
            fclose(dstFILE);
            fseek(f, size, SEEK_CUR);
            return; /* OK */
        } 
        fclose(dstFILE);
        /* the file is existing but is different -> conflict */ 
        CouldBeOpenForWrite = FALSE;
        dstFILE = fopen(Name,"ab");
        if (dstFILE)
        {
            CouldBeOpenForWrite = TRUE;
            fclose(dstFILE);
        }
    } else
    {
        CouldBeOpenForRead = FALSE;
        /* couldn't open file for read */
        /* the file is not existing */ 
        dstFILE = fopen(Name,"wb");
        if (dstFILE)
        {
            /* so create it */
            FileCopy(f,dstFILE,size);
            fclose(dstFILE);
            return;
        } 
        /* the file is not existing and could not be create (directory is not exist) */ 
        CouldBeOpenForWrite = FALSE;
    }
    /* problem */
    
    if (CouldBeOpenForRead && CouldBeOpenForWrite) 
    {
        /* file is exsit and is different */
        /* -> there is a file conflict */
        if (MO->ASKUSERFORTEXTURECONFLICT)
        {
            DialogBoxParam
                (
                hInstance,
                MAKEINTRESOURCE(IDD_DIALOGREPLACE),
                MO->ip->GetMAXHWnd(),
                ReplaceDlgProc,
                (LPARAM) MO
                );
        }
        if (MO -> ReplaceOption == 1) 
        {
            /* file is exist, and have to be Rename */
            strcpy(SaveName,Name);
            /* ASk user rename his texture */
            if (!CDLG_USR_GetFileName_Save(Name, MO->ip->GetMAXHWnd()))
            {
                strcpy(Name,SaveName);
                fseek(f, size, SEEK_CUR); /* Save cancel -> Skip */
                return; /* OK */
            }
            /* Test the new name file */
            CouldBeOpenForWrite = FALSE; 
            CouldBeOpenForRead = FALSE;
            dstFILE = fopen(Name,"ab");
            if (dstFILE)
            {
                CouldBeOpenForWrite = TRUE;
                CouldBeOpenForRead = TRUE;
                fclose(dstFILE);
            } else
            {
                dstFILE = fopen(Name,"rb");
                if (dstFILE)
                {
                    CouldBeOpenForWrite = FALSE; 
                    CouldBeOpenForRead = TRUE;
                    fclose(dstFILE);
                }
            }
        }
    }

    if (CouldBeOpenForRead && !CouldBeOpenForWrite)
    {
        /* the file is write protected */
        if (!MO -> USER_AS_BEEN_PREVENT_ON_WO)
        {
            sprintf(SaveName , "The file ""%s"" \n is READ-ONLY and is DIFFERENT \n than those included in .MAD. \n \n Read only files will not be replaced. \n Please modify access and re-import \n if you want to update that textures " , Name);
            MessageBox( MO->ip->GetMAXHWnd() , SaveName , TEXT("Write protected") , MB_OK | MB_ICONINFORMATION | MB_TASKMODAL ); 
            MO -> USER_AS_BEEN_PREVENT_ON_WO = TRUE;
        }
        fseek(f, size, SEEK_CUR); /* Skip */
        return; /* OK */
    }

    if (MO -> ReplaceOption == 2) 
    {
        /* User want to skip */
        fseek(f, size, SEEK_CUR);
        return; /* OK */
    }

    if (CouldBeOpenForWrite)
    {
        /* the file is replaced (or created if new name) */
        dstFILE = fopen(Name,"wb");
        FileCopy(f,dstFILE,size);
        fclose(dstFILE);
        return;
    }
    
    /* the file could not be open for read and could not be open for write: */
    /* The directory is not exist */
	if (!MO -> USER_AS_BEEN_PREVENT_ON_DNE)
	{
		sprintf(SaveName , "The file ""%s"" \n could not be saved \n because the directory is not exist. \n \n Please select ""Set extraction directory"". \n when re-import or create this directory \n if you want to update that textures " , Name);
		MessageBox( MO->ip->GetMAXHWnd() , SaveName , TEXT("Directory not exist") , MB_OK | MB_ICONINFORMATION | MB_TASKMODAL ); 
		MO -> USER_AS_BEEN_PREVENT_ON_DNE = TRUE;
	}
    fseek(f, size, SEEK_CUR);
    return; /* OK */

    
}


/*
 ===================================================================================================
    Dialog proc Start the Importer! This is the real entrypoint to the Importer. After the user has
    selected the filename (and he's prompted for overwrite etc.) this method is called. &
 ===================================================================================================
 */
int MadIn::DoImport(const TCHAR *name, ImpInterface *ei, Interface *i, BOOL suppressPrompts)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter, Counter2;
    ImpNode         *IMPN;
    Matrix3         NodeMAt;
    Color           Col;
	MadImportParam	*pst_TheParams;
    char			DirectoryName[260];
    char			FileName[260];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ip = i;
    ii = ei;
    ImportTime = 0/*ip->GetTime()*/;

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        User Dialog
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

	pst_TheParams = (MadImportParam *)suppressPrompts;
    if (!pst_TheParams)
    {
        if (!DialogBoxParam (
				hInstance,
				MAKEINTRESOURCE(IDD_MADDialogImport),
				ip->GetMAXHWnd(),
				ImportDlgProc,
				(LPARAM) this)) 
			return 1;
		mb_SetFullOpacity = FALSE;
		mb_ImportMultiTexMat = FALSE;
    }
	else
	{
		// Get params
		MERGE = pst_TheParams->mb_Merge;
		MERGETEXTURES = pst_TheParams->mb_MergeTexture;
		if (MERGETEXTURES)
			strcpy(TexturePATH, pst_TheParams->msz_TexturePath);
		mb_SetFullOpacity = pst_TheParams->mb_SetFullOpacity;
		mb_ImportMultiTexMat = pst_TheParams->mb_ImportMultiTexMat;
	}
    if(!MERGE) ei->NewScene();

    ip->ProgressStart(_T("Load..."), TRUE, fn, NULL);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Load MADWorld
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    ASKUSERFORTEXTURECONFLICT = TRUE;
    USER_AS_BEEN_PREVENT_ON_WO = FALSE;
	USER_AS_BEEN_PREVENT_ON_DNE= FALSE;

	sprintf(FileName, "%s", name);
    if (MERGETEXTURES)
         MW = MAD_Load(FileName,LoadTextureFilesCallback,(int)this);
    else
         MW = MAD_Load(FileName,NULL,0);

    if(MW == NULL)
    {
        MessageBox
        (
            ip->GetMAXHWnd(),
            _T("This is not a valid .MAD format. "),
            _T(""),
            MB_OK | MB_ICONERROR | MB_TASKMODAL
        );
	    MAD_FREE();
	    ip->ProgressEnd();
        return 1;
    }
    ip->ProgressUpdate(10);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Import Object, light, camera , shapes ....
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    MAD_MALLOC(Object *, AllObjRef, MW->NumberOfObjects);
    for(Counter = 0; Counter < MW->NumberOfObjects; Counter++)
    {
        AllObjRef[Counter] = MAD_X_To_MAX_X(MW->AllObjects[Counter]);
    }

    ip->ProgressUpdate(20);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        import TEXTURES
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    MAD_MALLOC(Texmap*, AllTexture , MW->NumberOftexture);
    for(Counter = 0; Counter < MW->NumberOftexture; Counter++)
    {
		
		// First, if user had selected an import directory for textures, use it.
		if ( TexturePATH[0] != 0)
		{
			MAD_SeparateDirNameFromName(MW->AllTextures[Counter]->Texturefile, DirectoryName, FileName);
			sprintf(MW->AllTextures[Counter]->Texturefile , "%s%s" , TexturePATH , FileName);
		}
		// Then we can import texture
        AllTexture[Counter] = MAD_Tex_To_MAX_Tex(MW -> AllTextures[Counter]);
    }

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        First import MATERIALS (simples)
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    MAD_MALLOC(Mtl *, AllMaterials, MW->NumberOfMaterials);
    for(Counter = 0; Counter < MW->NumberOfMaterials; Counter++)
    {
        AllMaterials[Counter] =
            MAD_Simple_MAT_To_MAX_Simple_Mat((MAD_StandarMaterial *) MW->AllMaterial[Counter]);
    }

    ip->ProgressUpdate(30);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Second import MATERIALS (complex)
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    for(Counter2 = 0; Counter2 < 4; Counter2++)
    {
        for(Counter = 0; Counter < MW->NumberOfMaterials; Counter++)
        {
            if(!AllMaterials[Counter])
			{
				switch (MW->AllMaterial[Counter]->MaterialType)
				{
				case ID_MAT_MultiMaterial:
					AllMaterials[Counter] =
						MAD_Cplx_MAT_To_MAX_Cplx_Mat((MAD_MultiMaterial *) MW->AllMaterial[Counter]);
					break;
				case ID_MAT_MultiTexMaterial:
					//if (mb_ImportMultiTexMat) {
					//	AllMaterials[Counter] =
					//		MAD_MultiTex_MAT_To_MAX_MultiTex_Mat((MAD_MultiTexMaterial *) MW->AllMaterial[Counter]);
					//}
					//else
					{
						// Transform the multi texture material into a standard mat
						MAD_StandarMaterial MadStdMat;
						memcpy (&MadStdMat, MW->AllMaterial[Counter], sizeof(MAD_StandarMaterial));
						MadStdMat.MatRef.MaterialType = ID_MAT_Standard;
						if (((MAD_MultiTexMaterial *) MW->AllMaterial[Counter])->NumberOfLevels)
						{
							MadStdMat.MadTexture = 
								((MAD_MultiTexMaterial *) MW->AllMaterial[Counter])->AllLevels->s_TextureId;
						}
						else
						{
							MadStdMat.MadTexture = MAD_NULL_INDEX;
						}
						MadStdMat.Undefined0 = 0;
						MadStdMat.Undefined1 = 0;
						MadStdMat.Undefined2 = 0;
						MadStdMat.Undefined3 = 0;
						MadStdMat.Undefined4 = 0;
						MadStdMat.Undefined5 = 0;
						MadStdMat.Undefined6 = 0;
						AllMaterials[Counter] = MAD_Simple_MAT_To_MAX_Simple_Mat (&MadStdMat);
					}
					break;
				}
			}
        }
    }

    ip->ProgressUpdate(40);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        First import NODES
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    /* Convert MADNODE to MAXNODE */
    MAD_MALLOC(INode *, AllINodes, MW->NumberOfHierarchieNodes);
    MAD_MALLOC(ImpNode *, AllImpNodes, MW->NumberOfHierarchieNodes);
	// and remember gizmo of bone nodes

    for(Counter = 0; Counter < MW->NumberOfHierarchieNodes; Counter++)
    {
		Object *Reference;
        IMPN = AllImpNodes[Counter] = ii->CreateNode();
        if(IMPN)
        {
            IMPN->SetName(_T(MW->Hierarchie[Counter].ID.Name));
            MAD_Matrix_To_MAX_Matrix(&MW->Hierarchie[Counter].Matrix, &NodeMAt);
            IMPN->SetTransform(0, NodeMAt);
            if(MW->Hierarchie[Counter].Object != MAD_NULL_INDEX)
            {
				Reference = AllObjRef[MW->Hierarchie[Counter].Object];
				/* BEGIN CREATE SEMI HINSTANCE*/
				///*
				if (MW->AllObjects [ MW->Hierarchie[Counter].Object] ->IDType == ID_MAD_GeometricObject_V0)
					if ((MW->Hierarchie[Counter].RLIOfObject != NULL) || 
						((MW->Hierarchie[Counter].ObjectWithRadiosity != MAD_NULL_INDEX) &&
						(MW->AllObjects [ MW->Hierarchie[Counter].ObjectWithRadiosity ] ->IDType == ID_MAD_GeometricObject_V0)))
				{
					IDerivedObject *Dobj;
					MAD_RadMod *RAMO = (MAD_RadMod *)ip->CreateInstance(OSM_CLASS_ID,MAD_RadMod_CLASS_ID);
					 if (RAMO)
					{
						Dobj = CreateDerivedObject(AllObjRef[MW->Hierarchie[Counter].Object]);
						RAMO -> RLI = NULL;
						RAMO -> RadiosityObject = NULL;
						RAMO -> OriginalsRLI = NULL;
						RAMO -> P3_RadiosityObjectSecondRliField = NULL;
						RAMO -> UL_RadiosityObjectSecondRliField = NULL;
						if (MW->Hierarchie[Counter].RLIOfObject != NULL)
						{
							unsigned long NumOfPoints,CounterX;
							MAD_GeometricObject *RadiosityObj;
							NumOfPoints = ((MAD_GeometricObject *)MW->AllObjects[MW->Hierarchie[Counter].Object])->NumberOfPoints;
							RAMO -> OriginalsRLI = (unsigned long *)malloc (sizeof(MAD_ColorARGB) * NumOfPoints );
							memcpy(RAMO -> OriginalsRLI , MW->Hierarchie[Counter].RLIOfObject , NumOfPoints  * sizeof(unsigned long ));
							RAMO -> RLI = (Point3 *)malloc (sizeof(Point3) * NumOfPoints);
							for (CounterX = 0 ; CounterX < NumOfPoints; CounterX++)
							{
								float Swap;
								MAD_Vertex Convert;
								MAD_ColorARGB_to_MAD_Vertex(Convert,MW->Hierarchie[Counter].RLIOfObject[CounterX]);
								Convert . x *= 1.0f / 256.0f;
								Convert . y *= 1.0f / 256.0f;
								Convert . z *= 1.0f / 256.0f;
								RAMO -> RLI [CounterX] = *(Point3 *) &Convert;
								Swap = RAMO -> RLI [CounterX].x;
								RAMO -> RLI [CounterX].x = RAMO -> RLI [CounterX].z;
								RAMO -> RLI [CounterX].z = Swap;
							}
							RAMO -> RadiosityObject = NULL; 
							RAMO -> P3_RadiosityObjectSecondRliField = NULL;
							RAMO -> UL_RadiosityObjectSecondRliField = NULL;
							if	((MW->Hierarchie[Counter].ObjectWithRadiosity != MAD_NULL_INDEX) &&
								(MW->AllObjects [ MW->Hierarchie[Counter].ObjectWithRadiosity ] ->IDType == ID_MAD_GeometricObject_V0))
							{
								RadiosityObj = ((MAD_GeometricObject *)MW->AllObjects[MW->Hierarchie[Counter].ObjectWithRadiosity]);
								NumOfPoints = RadiosityObj->NumberOfPoints;
								RAMO -> UL_RadiosityObjectSecondRliField = (unsigned long *)malloc (sizeof(MAD_ColorARGB) * NumOfPoints );
								memcpy(RAMO -> UL_RadiosityObjectSecondRliField , RadiosityObj->SecondRLIField , NumOfPoints  * sizeof(unsigned long ));
								RAMO -> P3_RadiosityObjectSecondRliField = (Point3 *)malloc (sizeof(Point3) * NumOfPoints);
								for (CounterX = 0 ; CounterX < NumOfPoints; CounterX++)
								{
									float Swap;
									MAD_Vertex Convert;
									MAD_ColorARGB_to_MAD_Vertex(Convert,RadiosityObj->SecondRLIField[CounterX]);
									Convert . x *= 1.0f / 256.0f;
									Convert . y *= 1.0f / 256.0f;
									Convert . z *= 1.0f / 256.0f;
									RAMO -> P3_RadiosityObjectSecondRliField [CounterX] = *(Point3 *) &Convert;
									Swap = RAMO -> P3_RadiosityObjectSecondRliField [CounterX].x;
									RAMO -> P3_RadiosityObjectSecondRliField [CounterX].x = RAMO -> P3_RadiosityObjectSecondRliField [CounterX].z;
									RAMO -> P3_RadiosityObjectSecondRliField [CounterX].z = Swap;
								}
#if defined(MAX3) || defined(MAX5) || defined(MAX7) || defined(MAX8)
								RAMO -> RadiosityObject = &((TriObject *)AllObjRef[MW->Hierarchie[Counter].ObjectWithRadiosity])->GetMesh();
#else
								RAMO -> RadiosityObject = &((TriObject *)AllObjRef[MW->Hierarchie[Counter].ObjectWithRadiosity])->Mesh();
#endif
							}
						}
						Dobj->ReferenceObject(AllObjRef[MW->Hierarchie[Counter].Object]);
						Dobj->AddModifier(RAMO);
						Dobj->ReferenceObject(AllObjRef[MW->Hierarchie[Counter].Object]);
						Reference = Dobj;
					}
				}
				//*/
				/* END CREATE SEMI HINSTANCE*/
                IMPN->Reference(Reference);
                ii->AddNodeToScene(IMPN);
                AllINodes[Counter] = IMPN->GetINode();
                AllINodes[Counter]->SetWireColor(MW->Hierarchie[Counter].WiredColor);
            }
        }
    }

    ip->ProgressUpdate(50);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Second import NODES
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    for(Counter = 0; Counter < MW->NumberOfHierarchieNodes; Counter++)
    {
        if(AllImpNodes[Counter])
        {
			
            if(MW->Hierarchie[Counter].Target != MAD_NULL_INDEX)
            {
                if(AllImpNodes[MW->Hierarchie[Counter].Target])
				{
                    ii->BindToTarget
                        (
                            AllImpNodes[Counter],
                            AllImpNodes[MW->Hierarchie[Counter].Target]
                        );
					MAD_Matrix_To_MAX_Matrix(&MW->Hierarchie[Counter].RelativeMatrix, &NodeMAt);
					AllImpNodes[Counter]->SetTransform(0, NodeMAt);
				}
            }

            if(MW->Hierarchie[Counter].Parent != MAD_NULL_INDEX)
            {
                if(AllINodes[MW->Hierarchie[Counter].Parent])
				{
					/* sTRANGE max !!*/
                    AllINodes[MW->Hierarchie[Counter].Parent]->AttachChild(AllINodes[Counter]);
					MAD_Matrix_To_MAX_Matrix(&MW->Hierarchie[Counter].Matrix, &NodeMAt);
					AllImpNodes[Counter]->SetTransform(0, NodeMAt);
				}
            }

            if(MW->Hierarchie[Counter].Material != MAD_NULL_INDEX)
            {
                if(AllMaterials[MW->Hierarchie[Counter].Material])
                    AllINodes[Counter]->SetMtl(AllMaterials[MW->Hierarchie[Counter].Material]);
            }

			if (MW->Hierarchie[Counter].NODE_Flags & MNF_IsGroupHead)
				AllINodes[Counter]->SetGroupHead(TRUE);
			if (MW->Hierarchie[Counter].NODE_Flags & MNF_IsGroupMember)
				AllINodes[Counter]->SetGroupMember(TRUE);


			/**************************************
			 *           Import skin info         *
			 **************************************/

			/*****************************************************************************
			 *  WARNING :
			 *		This doesn't work well : SetSkinTm ans AddWeights don't work.
			 *		Skin is created, bones are added but weights are nuts.
			 *		That's why DoImportSkin is called by the Mad_Util object
			 *		after import.
			 *		
			 *		The part of this code which compute weights is, therefore, useless but
			 *		I didn't delete it because I hope I will fix the problem some day.
			 *****************************************************************************/

			if (AllINodes[Counter] &&
				MW->AllObjects[MW->Hierarchie[Counter].Object]->IDType == ID_MAD_SkinnedGeometricObject)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				IDerivedObject				*Dobj;
				Modifier					*mod;
				ISkinImportData				*skin;
				MAD_SkinnedGeometricObject	*pst_Mad_SknObj;
				INode						*pst_BoneNode;
				MAD_PonderationList			*pst_PondList;
				INode						**apst_Bone;
				Tab<INode*>					tab_Bone;
				float						*af_Pond;
				Tab<float>					tab_Pond;
				float						f_Temp;
				int							i_NumVert, i_VertIdx;;
				unsigned char				*auc_NumPond;
				Matrix3						st_IdentityMat;

				//// DEBUG
				//INode	*pst_TempBone;

				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				st_IdentityMat.IdentityMatrix();

				// Check if there is a skin modifier
				mod = SkinUtils::GetSkin(AllINodes[Counter]);
				if (!mod)
				{
					// Create skin modifier
					mod = (Modifier *)ip->CreateInstance(OSM_CLASS_ID, SKIN_CLASSID);
					// Apply modifier
					Dobj = CreateDerivedObject(AllObjRef[MW->Hierarchie[Counter].Object]);
					Dobj->ReferenceObject(AllObjRef[MW->Hierarchie[Counter].Object]);
					Dobj->AddModifier(mod);
					Dobj->ReferenceObject(AllObjRef[MW->Hierarchie[Counter].Object]);
					AllINodes[Counter]->SetObjectRef(Dobj);
					
					//// DEBUG
					//mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
					//mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_NUM_SUBOBJECTTYPES_CHANGED);
					//AllINodes[Counter]->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
				}
				if (!mod) continue;  // useless

				// Set modifiers data
				skin = (ISkinImportData *) mod->GetInterface(I_SKINIMPORTDATA);
				if (!skin) continue;

				//// DEBUG
				//Matrix3 mat;
				//ISkin *SkinI = (ISkin *) mod->GetInterface(I_SKIN);
				//if (SkinI->GetSkinInitTM(AllINodes[Counter], mat ) != SKIN_OK)
				//	int i = 0;
				//MAD_Matrix_To_MAX_Matrix(&MW->Hierarchie[Counter].Matrix, &NodeMAt);

				if (!skin->SetSkinTm(AllINodes[Counter], st_IdentityMat, NodeMAt))
					int i = 0;   // For debug
					// TODO : Warning

				pst_Mad_SknObj = (MAD_SkinnedGeometricObject *)MW->AllObjects[MW->Hierarchie[Counter].Object];
				if (!pst_Mad_SknObj->us_NumberOfPonderationLists) continue;

				// Init arrays
				i_NumVert = pst_Mad_SknObj->pst_GeoObj->NumberOfPoints;
				apst_Bone   = (INode **)        malloc(MAD_IMPORT_MAX_BONE_PER_VERT * i_NumVert * sizeof(INode *));
				af_Pond     = (float *)         malloc(MAD_IMPORT_MAX_BONE_PER_VERT * i_NumVert * sizeof(float));
				auc_NumPond = (unsigned char *) malloc(    i_NumVert * sizeof(unsigned char));
				if (!apst_Bone || !af_Pond || !auc_NumPond) continue;
				memset(apst_Bone,   0, MAD_IMPORT_MAX_BONE_PER_VERT * i_NumVert * sizeof(INode *));
				memset(af_Pond,     0, MAD_IMPORT_MAX_BONE_PER_VERT * i_NumVert * sizeof(float));
				memset(auc_NumPond, 0,                                i_NumVert * sizeof(unsigned char));

				// Add bones and stores ponderations in arrays

				//// DEBUG
				//int i_NumBones = 0;
				//tab_Bone.ZeroCount();
				//tab_Pond.ZeroCount();
				//f_Temp = 0.0f;

				for (WORD us=0; us<pst_Mad_SknObj->us_NumberOfPonderationLists; us++)
				{
					pst_PondList = pst_Mad_SknObj->pst_PonderationList + us;

					// Find node associated to this bone
					if (pst_PondList->us_MatrixIdx < 0 || pst_PondList->us_MatrixIdx >= MW->NumberOfHierarchieNodes)
						continue;

					//// DEBUG
					//pst_TempBone = pst_BoneNode;

					pst_BoneNode = AllINodes[pst_PondList->us_MatrixIdx];
					if (!pst_BoneNode)
						continue;

					// Add bone to skin modif
					if (!skin->AddBoneEx(pst_BoneNode, (us == pst_Mad_SknObj->us_NumberOfPonderationLists - 1)))
						continue;

					if (!skin->SetBoneTm(pst_BoneNode, st_IdentityMat, pst_BoneNode->GetNodeTM(0)))
						int i = 0;  // For debug
						// TODO : Warning

					// Update pond and bone arrays
					for (WORD us2 = 0; us2 < pst_PondList->us_NumberOfPonderatedVertices; us2++)
					{
						i_VertIdx = (int)pst_PondList->p_CmpPdrtVrt[us2].Index;
						if (auc_NumPond[i_VertIdx] >= MAD_IMPORT_MAX_BONE_PER_VERT)
							// Too much weights. Impossible.
							// TODO : Warning
							continue;
						apst_Bone[MAD_IMPORT_MAX_BONE_PER_VERT * i_VertIdx + auc_NumPond[i_VertIdx]] = pst_BoneNode;
						f_Temp = *((float *) &pst_PondList->p_CmpPdrtVrt[us2]);
						*(ULONG *)&f_Temp &= 0xffff0000;
						af_Pond[MAD_IMPORT_MAX_BONE_PER_VERT * i_VertIdx + auc_NumPond[i_VertIdx]] = f_Temp;
						auc_NumPond[i_VertIdx]++;
					}

					////DEBUG
					//i_NumBones++;
					//tab_Bone.Append(1, &pst_BoneNode);
					//f_Temp = 0.0f;
					//tab_Pond.Append(1, &f_Temp);

				}

				//// DEBUG
				//MAD_Matrix_To_MAX_Matrix(&MW->Hierarchie[Counter].Matrix, &NodeMAt);
				//if (!skin->SetSkinTm(AllINodes[Counter], mat, NodeMAt))
				//	int i = 0;

				////DEBUG
				//tab_Pond[i_NumBones - 2] = 0.5f;
				//tab_Pond[i_NumBones - 1] = 0.5f;

				for (i_VertIdx = 0; i_VertIdx < i_NumVert; i_VertIdx++)
				{

					//// DEBUG
					//if (!skin->AddWeights(AllINodes[Counter], i_VertIdx, tab_Bone, tab_Pond))
					//	int i = 0;

					if (!auc_NumPond[i_VertIdx])
						continue;

					tab_Bone.ZeroCount();
					tab_Pond.ZeroCount();
					// Add ponderations
					for (int i = 0; i < MAD_IMPORT_MAX_BONE_PER_VERT && i < auc_NumPond[i_VertIdx]; i++)
					{
						tab_Bone.Append( 1, apst_Bone + (MAD_IMPORT_MAX_BONE_PER_VERT * i_VertIdx + i) );
						tab_Pond.Append( 1, af_Pond   + (MAD_IMPORT_MAX_BONE_PER_VERT * i_VertIdx + i) );
					}
					if (!skin->AddWeights(AllINodes[Counter], i_VertIdx, tab_Bone, tab_Pond))
						int i = 0;   // For debug
						// TODO : Warning
					else
						int i = 0;  // For DEBUG
				}

				mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
				mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_NUM_SUBOBJECTTYPES_CHANGED);
				AllINodes[Counter]->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
				ip->ForceCompleteRedraw();
			}
        }
    }

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Set Vertex color on-off
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
	for(Counter = 0; Counter < MW->NumberOfHierarchieNodes; Counter++)
    {
		MAD_GeometricObject *Mad_Obj;

        if(AllINodes[Counter])
        {
			if(MW->AllObjects[MW->Hierarchie[Counter].Object] -> IDType == ID_MAD_GeometricObject_V0)
				Mad_Obj = (MAD_GeometricObject *)MW->AllObjects[MW->Hierarchie[Counter].Object];
			else if (MW->AllObjects[MW->Hierarchie[Counter].Object] -> IDType == ID_MAD_SkinnedGeometricObject)
				Mad_Obj = ((MAD_SkinnedGeometricObject *)MW->AllObjects[MW->Hierarchie[Counter].Object])->pst_GeoObj;
			else
				Mad_Obj = NULL;

			if(Mad_Obj)
			{
				if ((MAD_VerifyVertexColorAreNotTo0(Mad_Obj)) ||
					(MW->Hierarchie[Counter].RLIOfObject != NULL))
				{
					AllINodes[Counter] -> SetCVertMode(TRUE);
				}
				else
					AllINodes[Counter] -> SetCVertMode(FALSE);
				if(Mad_Obj->ObjectFlags & GO_ShadowCut)
					AllINodes[Counter] -> SetRcvShadows(TRUE);
				else
					AllINodes[Counter] -> SetRcvShadows(FALSE);
			}
        }
    }

    ip->ProgressUpdate(60);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Import Globals
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    MAD_Color_To_MAX_Color(&MW->AmbientColor, &Col);
    ip->SetAmbient(ImportTime, Col);

    ip->ProgressUpdate(70);

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        End
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    MAD_FREE();
    ip->ProgressEnd();

	MERGE = FALSE;
    return 1;
}