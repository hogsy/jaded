/*$T MAD_Load.c GC!1.32 05/27/99 15:23:29 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "MAD_mem/Sources/MAD_mem.h"
#include "MAD_Struct_V0.h"
#include <WINDOWS.H>
#include <COMMCTRL.H>

#ifdef JADEFUSION
#define MAD_Load_MainStruct(type, address, file) \
    MAD_MALLOC(type, address, 1); \
    if(1 != fread(&(address)->ID.SizeOfThisOne, sizeof(type) - sizeof(ID_MAD_Type), 1, file)) \
	{MessageBox(NULL, MAD_ERROR_READ, TEXT("MAD Read"), MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);}\
    (address)->ID.IDType = IDType;

#define MAD_Load_NormalStruct(type, address, size, file) \
    MAD_MALLOC(type, address, size); \
    {if(size != fread(address, sizeof(type), size, file)) \
	{MessageBox(NULL, MAD_ERROR_READ, TEXT("MAD Read"), MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);}\
	}

#define MAD_ERROR_READ "MAD file read error\nCheck the file size of your MAD file"
#else
#define MAD_Load_MainStruct(type, address, file) \
    MAD_MALLOC(type, address, 1); \
    fread(&(address)->ID.SizeOfThisOne, sizeof(type) - sizeof(ID_MAD_Type), 1, file); \
    (address)->ID.IDType = IDType;

#define MAD_Load_NormalStruct(type, address, size, file) \
    MAD_MALLOC(type, address, size); \
    fread(address, sizeof(type), size, file);
#endif


static char gaz_MAD_CurrentPath[_MAX_PATH] = "";

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadUndefined(MAD_World *MW, MAD_Undefined **MN, FILE *File, ID_MAD_Type IDType)
{
    MAD_Load_MainStruct(MAD_Undefined, *MN, File);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadDummy(MAD_World *MW, MAD_Dummy **MD, FILE *File, ID_MAD_Type IDType)
{
    MAD_Load_MainStruct(MAD_Dummy, *MD, File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadTarget(MAD_World *MW, MAD_Target **MT, FILE *File, ID_MAD_Type IDType)
{
    MAD_Load_MainStruct(MAD_Target, *MT, File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadGeometric_V0(MAD_World *MW, MAD_GeometricObject **MO, FILE *File, ID_MAD_Type IDType)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MAD_Load_MainStruct(MAD_GeometricObject, *MO, File);
    switch((*MO)->TypeOfThis)
    {
    case MAD_MAXMESH_V0:
        MAD_Load_NormalStruct
        (
            MAD_Simple3DVertex,
            (*MO)->OBJ_PointList,
            (*MO)->NumberOfPoints,
            File
        );
        MAD_Load_NormalStruct(MAD_Vertex, (*MO)->OBJ_UVtextureList, (*MO)->NumberOfUV, File);
		if ((*MO)->Colors) 
		{
			MAD_Load_NormalStruct(MAD_ColorARGB, (*MO)->Colors, (*MO)->NumberOfPoints, File);
		} else
			MAD_MALLOC(MAD_ColorARGB, (*MO)->Colors, (*MO)->NumberOfPoints);
		if ((*MO)->SecondRLIField) 
		{
			MAD_Load_NormalStruct(MAD_ColorARGB, (*MO)->SecondRLIField, (*MO)->NumberOfPoints, File);
		}		

        MAD_Load_NormalStruct
        (
            MAD_GeometricObjectElement *,
            (*MO)->Elements,
            (*MO)->NumberOfElements,
            File
        );
        for(Counter = 0; Counter < (*MO)->NumberOfElements; Counter++)
        {
            MAD_Load_NormalStruct(MAD_GeometricObjectElement, (*MO)->Elements[Counter], 1, File);
            MAD_Load_NormalStruct
            (
                MAD_Face,
                (*MO)->Elements[Counter]->Faces,
                (*MO)->Elements[Counter]->NumberOfTriangles,
                File
            );
        }

        break;
    }
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadSkinnedGeometric(MAD_World *MW, MAD_SkinnedGeometricObject **MO, FILE *File, ID_MAD_Type IDType)
{
    unsigned long	Counter;
	unsigned short	i;

	MAD_Load_MainStruct(MAD_SkinnedGeometricObject, *MO, File);
	MAD_Load_NormalStruct(MAD_GeometricObject, (*MO)->pst_GeoObj, 1, File);
	switch((*MO)->pst_GeoObj->TypeOfThis)
    {
    case MAD_MAXMESH_V0:
        MAD_Load_NormalStruct(
            MAD_Simple3DVertex,
			(*MO)->pst_GeoObj->OBJ_PointList,
            (*MO)->pst_GeoObj->NumberOfPoints,
            File);
        MAD_Load_NormalStruct(
			MAD_Vertex,
			(*MO)->pst_GeoObj->OBJ_UVtextureList,
			(*MO)->pst_GeoObj->NumberOfUV,
			File);
		if ((*MO)->pst_GeoObj->Colors)
		{
			MAD_Load_NormalStruct(
				MAD_ColorARGB,
				(*MO)->pst_GeoObj->Colors,
				(*MO)->pst_GeoObj->NumberOfPoints,
				File);
		}
		else
		{
			MAD_MALLOC(
				MAD_ColorARGB,
				(*MO)->pst_GeoObj->Colors,
				(*MO)->pst_GeoObj->NumberOfPoints);
		}
		if ((*MO)->pst_GeoObj->SecondRLIField)
		{
			MAD_Load_NormalStruct(
				MAD_ColorARGB,
				(*MO)->pst_GeoObj->SecondRLIField,
				(*MO)->pst_GeoObj->NumberOfPoints,
				File);
		}

        MAD_Load_NormalStruct(
            MAD_GeometricObjectElement *,
            (*MO)->pst_GeoObj->Elements,
            (*MO)->pst_GeoObj->NumberOfElements,
            File);
		for(Counter = 0; Counter < (*MO)->pst_GeoObj->NumberOfElements; Counter++)
        {
            MAD_Load_NormalStruct(
				MAD_GeometricObjectElement,
				(*MO)->pst_GeoObj->Elements[Counter],
				1,
				File);
            MAD_Load_NormalStruct(
                MAD_Face,
                (*MO)->pst_GeoObj->Elements[Counter]->Faces,
                (*MO)->pst_GeoObj->Elements[Counter]->NumberOfTriangles,
                File);
        }
        break;
    }
	MAD_Load_NormalStruct(
		MAD_PonderationList,
		(*MO)->pst_PonderationList,
		(*MO)->us_NumberOfPonderationLists,
		File);
	for (i = 0; i < (*MO)->us_NumberOfPonderationLists; ++i)
	{
		MAD_Load_NormalStruct(
			MAD_CompressedPonderatedVertex,
			(*MO)->pst_PonderationList[i].p_CmpPdrtVrt,
			(*MO)->pst_PonderationList[i].us_NumberOfPonderatedVertices,
			File);
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadLight(MAD_World *MW, MAD_Light **ML, FILE *File, ID_MAD_Type IDType)
{
    MAD_Load_MainStruct(MAD_Light, *ML, File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadShape(MAD_World *MW, MAD_Shape **ML, FILE *File, ID_MAD_Type IDType)
{
    MAD_Load_MainStruct(MAD_Shape, *ML, File);
    switch((*ML)->TypeOfShape)
    {
    case ID_MAT_NURB:
        MAD_Load_NormalStruct(NurbKnot, (*ML)->Nurb.Knots, (*ML)->Nurb.NumberOfKnots, File);
        break;
    case ID_MAT_SPLINE3D:
        MAD_Load_NormalStruct(MAD_SplineKnot, (*ML)->Spline.Knots, (*ML)->Spline.NumberOfKnots, File);
        break;
    }
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadCamera(MAD_World *MW, MAD_Camera **MC, FILE *File, ID_MAD_Type IDType)
{
    MAD_Load_MainStruct(MAD_Camera, *MC, File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadMaterial(MAD_World *MW, MAD_MAT_MatID **MT, FILE *File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_MAT_MatID   NM;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION    
    if(1 != fread(&NM, sizeof(MAD_MAT_MatID), 1, File))
	{
		MessageBox(NULL, MAD_ERROR_READ, TEXT("MAD Read"), MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
	}
#else	
	fread(&NM, sizeof(MAD_MAT_MatID), 1, File);
#endif
	
	fseek(File, -(long) sizeof(MAD_MAT_MatID), SEEK_CUR);
    switch(NM.MaterialType)
    {
    case ID_MAT_NullMat:
        MAD_Load_NormalStruct(MAD_MAT_MatID, *MT, 1, File);
        **MT = NM;
        break;
    case ID_MAT_Standard:
        MAD_Load_NormalStruct(MAD_StandarMaterial, *(MAD_StandarMaterial **) MT, 1, File);
        **MT = NM;
        break;
    case ID_MAT_MultiMaterial:
        MAD_Load_NormalStruct(MAD_MultiMaterial, *(MAD_MultiMaterial **) MT, 1, File);
        MAD_Load_NormalStruct
        (
            unsigned long,
            (*(MAD_MultiMaterial **) MT)->SubMats,
            (*(MAD_MultiMaterial **) MT)->NumberOfSubMaterials,
            File
        );
        **MT = NM;
        break;
    case ID_MAT_MultiTexMaterial:
		MAD_Load_NormalStruct(MAD_MultiTexMaterial, *(MAD_MultiTexMaterial **) MT, 1, File);
		MAD_Load_NormalStruct
        (
            MAD_MTLevel,
            (*(MAD_MultiTexMaterial **) MT)->AllLevels,
            (*(MAD_MultiTexMaterial **) MT)->NumberOfLevels,
            File
        );
        **MT = NM;
        break;
    }
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadTexture(MAD_World *MW, MAD_texture **MT, FILE *File, MAD_FileFlag SF)
{
	char	temp[_MAX_PATH];

    MAD_Load_NormalStruct(MAD_texture, *MT, 1, File);
	
	if (SF & MAD_RelativeTexFileName)
	{
		// Compute absolute path
		strcpy(temp, (*MT)->Texturefile);
		strcpy((*MT)->Texturefile, gaz_MAD_CurrentPath);
		strcat((*MT)->Texturefile, temp);
	}
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadAllNodes(MAD_World *MW, FILE *File)
{
	unsigned long NodeCounter,Local;
	MAD_Load_NormalStruct(MAD_WorldNode, MW->Hierarchie, MW->NumberOfHierarchieNodes, File);
	for (NodeCounter= 0;NodeCounter < MW->NumberOfHierarchieNodes ; NodeCounter++)
	{
		MW->Hierarchie[NodeCounter] . UsersRLIOfObject = NULL;
		if (MW->Hierarchie[NodeCounter] . RLIOfObject)
		{
			if (MW->AllObjects [ MW -> Hierarchie [ NodeCounter ] . Object ] -> IDType == ID_MAD_GeometricObject_V0)
			{
				Local = ((MAD_GeometricObject *)MW->AllObjects [ MW -> Hierarchie [ NodeCounter ] . Object ]) -> NumberOfPoints;
				MAD_Load_NormalStruct(MAD_ColorARGB, MW->Hierarchie[NodeCounter] . RLIOfObject , Local , File);
				MAD_MALLOC(MAD_ColorARGB ,  MW->Hierarchie[NodeCounter] . UsersRLIOfObject , Local);
				memcpy(MW->Hierarchie[NodeCounter] . UsersRLIOfObject , MW->Hierarchie[NodeCounter] . RLIOfObject , sizeof(MAD_ColorARGB) * Local);//*/
			} else
			{
				MW->Hierarchie[NodeCounter] . RLIOfObject = NULL;
				MW->Hierarchie[NodeCounter] . ObjectWithRadiosity = MAD_NULL_INDEX;
			}
		} else
		if (MW -> Hierarchie [ NodeCounter ] . Object != MAD_NULL_INDEX)
		{
			if (MW->AllObjects [ MW -> Hierarchie [ NodeCounter ] . Object ] -> IDType == ID_MAD_GeometricObject_V0)
			{
				if (((MAD_GeometricObject *)MW->AllObjects [ MW -> Hierarchie [ NodeCounter ] . Object ]) ->Colors )
				{
					Local = ((MAD_GeometricObject *)MW->AllObjects [ MW -> Hierarchie [ NodeCounter ] . Object ]) -> NumberOfPoints;
//					MAD_Load_NormalStruct(MAD_ColorARGB, MW->Hierarchie[NodeCounter] . RLIOfObject , Local , File);
					MAD_MALLOC(MAD_ColorARGB ,  MW->Hierarchie[NodeCounter] . UsersRLIOfObject , Local);
					memcpy(MW->Hierarchie[NodeCounter] . UsersRLIOfObject , ((MAD_GeometricObject *)MW->AllObjects [ MW -> Hierarchie [ NodeCounter ] . Object ]) ->Colors , sizeof(MAD_ColorARGB) * Local);//*/
				}
			} 
		}
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadAllObjects(MAD_World *MW, FILE *File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned    Counter;
    ID_MAD_Type IDType;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(Counter = 0; Counter < MW->NumberOfObjects; Counter++)
    {
#ifdef JADEFUSION
		if(1 != fread(&IDType, sizeof(ID_MAD_Type), 1, File))
		{
			MessageBox(NULL, MAD_ERROR_READ, TEXT("MAD Read"), MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
		}
#else
		fread(&IDType, sizeof(ID_MAD_Type), 1, File);
#endif
		switch(IDType)
        {
        case ID_MAD_SkinnedGeometricObject:
            MAD_LoadSkinnedGeometric(
              MW,
              (MAD_SkinnedGeometricObject **) &MW->AllObjects[Counter],
              File,
              IDType);
            break;
        case ID_MAD_GeometricObject_V0:
            MAD_LoadGeometric_V0(
              MW,
              (MAD_GeometricObject **) &MW->AllObjects[Counter],
              File,
              IDType);
            break;
        case ID_MAD_Light:
            MAD_LoadLight(MW, (MAD_Light **) &MW->AllObjects[Counter], File, IDType);
            break;
        case ID_MAD_Camera:
            MAD_LoadCamera(MW, (MAD_Camera **) &MW->AllObjects[Counter], File, IDType);
            break;
        case ID_MAD_Dummy:
            MAD_LoadDummy(MW, (MAD_Dummy **) &MW->AllObjects[Counter], File, IDType);
            break;
        case ID_MAD_Shape:
            MAD_LoadShape(MW, (MAD_Shape **) &MW->AllObjects[Counter], File, IDType);
            break;
        case ID_MAD_TargetObject:
            MAD_LoadTarget(MW, (MAD_Target **) &MW->AllObjects[Counter], File, IDType);
            break;
        case ID_MAD_UNDEFINED:
            MAD_LoadUndefined(MW, (MAD_Undefined **) &MW->AllObjects[Counter], File, IDType);
            break;
        }
    }
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadAllMaterials(MAD_World *MW, FILE *File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned    Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(Counter = 0; Counter < MW->NumberOfMaterials; Counter++)
        MAD_LoadMaterial(MW, &MW->AllMaterial[Counter], File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadAllTextures(MAD_World *MW, FILE *File, MAD_FileFlag SF)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned    Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(Counter = 0; Counter < MW->NumberOftexture; Counter++)
        MAD_LoadTexture(MW, &MW->AllTextures[Counter], File, SF);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
MAD_World *MAD_Load_Version_0(FILE *File, MAD_FileFlag SF)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_World   *MW;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MAD_MALLOC(MAD_World, MW, 1);
#ifdef JADEFUSION
    if(1 != fread(MW, sizeof(*MW), 1, File))
	{
		MessageBox(NULL, MAD_ERROR_READ, TEXT("MAD Read"), MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
	}
#else
	fread(MW, sizeof(*MW), 1, File);
#endif
	MAD_MALLOC(MAD_texture *, MW->AllTextures, MW->NumberOftexture);
    MAD_MALLOC(MAD_MAT_MatID *, MW->AllMaterial, MW->NumberOfMaterials);
    MAD_MALLOC(MAD_NodeID *, MW->AllObjects, MW->NumberOfObjects);
    MAD_MALLOC(MAD_WorldNode, MW->Hierarchie, MW->NumberOfHierarchieNodes);
    MAD_LoadAllTextures(MW, File, SF);
    MAD_LoadAllMaterials(MW, File);
    MAD_LoadAllObjects(MW, File);
    MAD_LoadAllNodes(MW, File);
    return MW;
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadTexturesFilesDefaultCallback(FILE *File,int size,char *Name,int USERPARRAM)
{
    fseek(File, size , SEEK_CUR);
}
/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_LoadTexturesFiles(FILE *File,MAD_World *MW,void (*LoadCallback)(FILE *f,int size,char *Name,int USERPARRAM),int USERPARRAM)
{
    MAD_SubFile     MSF;
    unsigned long   Result;
    char            *sz_Name;

    while (!feof(File))
    {
        Result = fread(&MSF,1,sizeof(MSF), File);
        if (Result != sizeof(MSF)) return;
        if (MSF . ID . IDType == ID_MAD_SubFile)
        {
            sz_Name = MW->AllTextures[MSF. TextureIndex]->Texturefile;
            if (*sz_Name)
                LoadCallback(File , MSF . FileSize , sz_Name, USERPARRAM);
        }
		else return;
    }
};
/*
 ===================================================================================================
 ===================================================================================================
 */
MAD_World *MAD_Load(char *FileName,void (*LoadCallback)(FILE *f,int size,char *Name , int USERPARRAM),int USERPARRAM)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Version;
    unsigned long   FileFlag;
    MAD_World       *MaW;
	char			Texte[256];
	FILE			*File;
	char			temp[_MAX_PATH];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	File = fopen(FileName, "rb");
	if (!File)
		return NULL;

#ifdef JADEFUSION
	size_t ReadLength = 0;

    if(1 != fread(&Version, sizeof(unsigned long), 1, File))
	{
		MessageBox(NULL, MAD_ERROR_READ, TEXT("MAD Read"), MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
		return NULL;
	}
    if(1 != fread(&FileFlag, sizeof(unsigned long), 1, File))
	{
		MessageBox(NULL, MAD_ERROR_READ, TEXT("MAD Read"), MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
		return NULL;
	}
#else
    fread(&Version, sizeof(unsigned long), 1, File);
    fread(&FileFlag, sizeof(unsigned long), 1, File);
#endif

	if (FileFlag & MAD_RelativeTexFileName)
	{
		int iLen;
		char *ptr;
		// Texture paths are not stored. Store path to MAD because we will look for
		// them there.
		MAD_SeparateDirNameFromName(FileName, gaz_MAD_CurrentPath, temp);
		// Replace '\' by '/'
		ptr = strrchr(gaz_MAD_CurrentPath, '\\');
		while (ptr)
		{
			ptr[0] = '/';
			ptr = strrchr(gaz_MAD_CurrentPath, '\\');
		}
		// Be sure last char is '/'
		iLen = strlen(gaz_MAD_CurrentPath);
		if (iLen && gaz_MAD_CurrentPath[iLen -1] != '/')
			strcat(gaz_MAD_CurrentPath, "/");
	}

    MaW = NULL;
	if (Version == MAD_WORLD_FILE_LAST_VERION)
        MaW = MAD_Load_Version_0(File, FileFlag);
    else 
	{
		switch (Version)
		{
		case MAD_WORLD_FILE_VERION_2:
		case MAD_WORLD_FILE_VERION_1:
			MaW = MAD_Load_Version_0(File, FileFlag);
			break;
		case MAD_WORLD_FILE_VERION_0:
			MaW = (MAD_World*)MAD_Load_V0(File);
#ifdef JADEFUSION
			MaW = (MAD_World*)MAD_Update_0_to_1((struct MAD_0_World_*)MaW);
#else
			MaW = (MAD_World*)MAD_Update_0_to_1((void *)MaW);
#endif
			break;
		default:
			sprintf(Texte,"The .MAD that you try to read is more recent \nthan your program(or plugin).\nProgram version is %d \n.MAD file version is %d \nPlease update your .exe(or plugin)!!.\n\n",MAD_WORLD_FILE_LAST_VERION & 0xffff,  Version & 0xffff);
			MessageBox
				(
				NULL,
				Texte,
				TEXT("File format"),
				MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL
				);
			fclose(File);
			return NULL;
			break;
		}
	}

	/* Texture file must stay compatible for each update */
    if (FileFlag & MAD_SaveTextureFiles)
    {
        if (LoadCallback)
            MAD_LoadTexturesFiles(File,MaW,LoadCallback,USERPARRAM);
        else
            MAD_LoadTexturesFiles(File,MaW,MAD_LoadTexturesFilesDefaultCallback,USERPARRAM);
    }

	MAD_CleanUnusedObject(MaW);
	fclose(File);
    return MaW;
};

