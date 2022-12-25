/*$T MAD_Save.cpp GC!1.32 05/21/99 11:24:13 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include <string.h>
#include "MAD_mem/Sources/MAD_mem.h"
#include "MAD_Struct_V0.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SaveUndefined(MAD_World *MW, MAD_NodeID *MN, FILE *File)
{
    fwrite(MN, sizeof(MAD_NodeID), 1, File);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SaveDummy(MAD_World *MW, MAD_Dummy *MD, FILE *File)
{
    fwrite(MD, sizeof(MAD_Dummy), 1, File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SaveTarget(MAD_World *MW, MAD_Target *MT, FILE *File)
{
    fwrite(MT, sizeof(MAD_Target), 1, File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SaveGeometric_V0(MAD_World *MW, MAD_GeometricObject *MO, FILE *File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    switch(MO->TypeOfThis)
    {
    case MAD_MAXMESH_V0:
        fwrite(MO, sizeof(MAD_GeometricObject), 1, File);
        fwrite(MO->OBJ_PointList, sizeof(MAD_Simple3DVertex), MO->NumberOfPoints, File);
        fwrite(MO->OBJ_UVtextureList, sizeof(MAD_Vertex ), MO->NumberOfUV, File);
		if (MO->Colors) 
		{
			fwrite(MO->Colors, sizeof(MAD_ColorARGB), MO->NumberOfPoints, File);
		} 
		if (MO->SecondRLIField)
		{
			fwrite(MO->SecondRLIField, sizeof(MAD_ColorARGB), MO->NumberOfPoints, File);
		}
		
        fwrite(MO->Elements, sizeof(MAD_GeometricObjectElement *), MO->NumberOfElements, File);
        for(Counter = 0 ; Counter < MO->NumberOfElements; Counter++)
        {
            fwrite(MO->Elements[Counter], sizeof(MAD_GeometricObjectElement), 1, File);
            fwrite
            (
                MO->Elements[Counter]->Faces,
                sizeof(MAD_Face),
                MO->Elements[Counter]->NumberOfTriangles,
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
void MAD_SaveSkinnedGeometric(MAD_World *MW, MAD_SkinnedGeometricObject *MO, FILE *File)
{
	MAD_PonderationList	*pst_PdrtElt;
	MAD_PonderationList	*pst_PdrtEnd;

	fwrite(MO, sizeof(MAD_SkinnedGeometricObject), 1, File);
	MAD_SaveGeometric_V0(MW, MO->pst_GeoObj, File);
	fwrite(
		MO->pst_PonderationList,
		sizeof (MAD_PonderationList),
		MO->us_NumberOfPonderationLists,
		File);
	pst_PdrtElt = MO->pst_PonderationList;
	pst_PdrtEnd = MO->pst_PonderationList + MO->us_NumberOfPonderationLists;
	while (pst_PdrtElt < pst_PdrtEnd)
	{
		fwrite(
			pst_PdrtElt->p_CmpPdrtVrt,
			sizeof (MAD_CompressedPonderatedVertex),
			pst_PdrtElt->us_NumberOfPonderatedVertices,
			File);
		++pst_PdrtElt;
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SaveLight(MAD_World *MW, MAD_Light *ML, FILE *File)
{
    switch(ML->LightType)
    {
    case MAD_LightOmni:
    case MAD_LightSpot:
    case MAD_LightDirect:
        fwrite(ML, sizeof(*ML), 1, File);
        break;
    }
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SaveShape(MAD_World *MW, MAD_Shape *ML, FILE *File)
{
    switch(ML->TypeOfShape)
    {
    case ID_MAT_NURB:
        fwrite(ML, sizeof(MAD_Shape), 1, File);
		fwrite(ML->Nurb.Knots , sizeof(NurbKnot) , ML->Nurb.NumberOfKnots, File);
        break;
    case ID_MAT_SPLINE3D:
        fwrite(ML, sizeof(MAD_Shape), 1, File);
		fwrite(ML->Spline.Knots , sizeof(MAD_SplineKnot) , ML->Spline.NumberOfKnots , File);
        break;
    }
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SaveCamera(MAD_World *MW, MAD_Camera *MC, FILE *File)
{
    switch(MC->CameraType)
    {
    case MAD_e_OrthoCamera:
    case MAD_e_PerspCamera:
        fwrite(MC, sizeof(MAD_Camera), 1, File);
        break;
    }
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SaveMaterial(MAD_World *MW, MAD_MAT_MatID *MT, FILE *File)
{
    switch(MT->MaterialType)
    {
    case ID_MAT_NullMat:
        fwrite(MT, sizeof(MAD_MAT_MatID), 1, File);
        break;
    case ID_MAT_Standard:
        fwrite(MT, sizeof(MAD_StandarMaterial), 1, File);
        break;
    case ID_MAT_MultiMaterial:
        fwrite(MT, sizeof(MAD_MultiMaterial), 1, File);
        fwrite
        (
            ((MAD_MultiMaterial *) MT)->SubMats,
            sizeof(unsigned long),
            ((MAD_MultiMaterial *) MT)->NumberOfSubMaterials,
            File
        );
        break;
	case ID_MAT_MultiTexMaterial:
		fwrite(MT, sizeof(MAD_MultiTexMaterial), 1, File);
        fwrite
        (
            ((MAD_MultiTexMaterial *) MT)->AllLevels,
            sizeof(MAD_MTLevel),
            ((MAD_MultiTexMaterial *) MT)->NumberOfLevels,
            File
        );
		break;
    }
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SaveTexture(MAD_World *MW, MAD_texture *MT, FILE *File, MAD_FileFlag SF)
{
	char    DirName[260];
    char    FileName[260];
    char    CompletePathName[260];

	CompletePathName[0] = 0;
	if (SF & MAD_RelativeTexFileName)
	{
		MAD_SeparateDirNameFromName(MT->Texturefile, DirName, FileName);
		if (FileName[0])
		{
			memcpy( CompletePathName, MT->Texturefile, 260 );
			memcpy( MT->Texturefile, FileName, 260 );
		}
	}
    fwrite(MT, sizeof(*MT), 1, File);
	if ((SF & MAD_RelativeTexFileName) && CompletePathName[0])
	{
		memcpy( MT->Texturefile, CompletePathName, 260 );
	}
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SaveAllNodes(MAD_World *MW, FILE *File)
{
	unsigned long NodeCounter,Local;
    fwrite(MW->Hierarchie, sizeof(MAD_WorldNode), MW->NumberOfHierarchieNodes, File);
	for (NodeCounter= 0;NodeCounter < MW->NumberOfHierarchieNodes ; NodeCounter++)
	{
		if (MW->Hierarchie[NodeCounter] . RLIOfObject)
		{
			if (MW->AllObjects [ MW -> Hierarchie [ NodeCounter ] . Object ] -> IDType == ID_MAD_GeometricObject_V0)
			{
				Local = ((MAD_GeometricObject *)MW->AllObjects [ MW -> Hierarchie [ NodeCounter ] . Object ]) -> NumberOfPoints;
				fwrite(MW->Hierarchie[NodeCounter] . RLIOfObject , sizeof(MAD_ColorARGB) , Local , File);
			}else
			{
				MW->Hierarchie[NodeCounter] . RLIOfObject = NULL;
				MW->Hierarchie[NodeCounter] . ObjectWithRadiosity = MAD_NULL_INDEX;
			}
		}
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SaveAllObjects(MAD_World *MW, FILE *File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned    Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(Counter = 0; Counter < MW->NumberOfObjects; Counter++)
    {
        switch(MW->AllObjects[Counter]->IDType)
        {
		case ID_MAD_SkinnedGeometricObject:
			MAD_SaveSkinnedGeometric(MW, (MAD_SkinnedGeometricObject *) MW->AllObjects[Counter], File);
			break;
        case ID_MAD_GeometricObject_V0:
            MAD_SaveGeometric_V0(MW, (MAD_GeometricObject *) MW->AllObjects[Counter], File);
            break;
        case ID_MAD_Light:
            MAD_SaveLight(MW, (MAD_Light *) MW->AllObjects[Counter], File);
            break;
        case ID_MAD_Camera:
            MAD_SaveCamera(MW, (MAD_Camera *) MW->AllObjects[Counter], File);
            break;
        case ID_MAD_Dummy:
            MAD_SaveDummy(MW, (MAD_Dummy *) MW->AllObjects[Counter], File);
            break;
        case ID_MAD_Shape:
            MAD_SaveShape(MW, (MAD_Shape *) MW->AllObjects[Counter], File);
            break;
        case ID_MAD_TargetObject:
            MAD_SaveTarget(MW, (MAD_Target *) MW->AllObjects[Counter], File);
            break;
        case ID_MAD_UNDEFINED:
            MAD_SaveUndefined(MW, (MAD_NodeID *) MW->AllObjects[Counter], File);
            break;
        }
    }
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SaveAllMaterials(MAD_World *MW, FILE *File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned    Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(Counter = 0; Counter < MW->NumberOfMaterials; Counter++)
        MAD_SaveMaterial(MW, MW->AllMaterial[Counter], File);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SaveAllTextures(MAD_World *MW, FILE *File, MAD_FileFlag SF)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned    Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(Counter = 0; Counter < MW->NumberOftexture; Counter++)
        MAD_SaveTexture(MW, MW->AllTextures[Counter], File, SF);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
#define MAD_LOCAL_BUFFER_SIZE 256

void MAD_SaveAllTextureFiles(MAD_World *MW, FILE *File)
{
  FILE          *src;
  MAD_SubFile   MSF;
  unsigned char Buffer[MAD_LOCAL_BUFFER_SIZE];
  unsigned long Result, Counter ;

  for (Counter = 0; Counter < MW->NumberOftexture; Counter++)
  {
    MSF.ID.IDType = ID_MAD_SubFile;
    MSF.TextureIndex = Counter;

    Result = 0;
    if (*MW->AllTextures[Counter]->Texturefile)
    {
      src = fopen(MW->AllTextures[Counter]->Texturefile, "rb");
      /* compute size */
      if (src)
      {
        fseek(src, 0, SEEK_END);
        Result = ftell(src);
        fclose(src);
      }
    }
    MSF.FileSize = Result;
    fwrite(&MSF, sizeof(MSF), 1, File);
    /* insert file */
    if (Result)
    {
      src = fopen(MW->AllTextures[Counter]->Texturefile, "rb");
      if (src)
      {
        while (!feof(src))
        {
          Result = fread(Buffer, 1, MAD_LOCAL_BUFFER_SIZE, src);
          fwrite(Buffer, 1, Result, File);
        }
        fclose(src);
      }
    }
  }
}


/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_Save(MAD_World *MW, FILE *File,MAD_FileFlag SF)
{
  unsigned long FileVersion;

  FileVersion = MAD_WORLD_FILE_LAST_VERION; /* alway the latest version */ 
  fwrite(&FileVersion, sizeof(unsigned long), 1, File);
  fwrite(&SF         , sizeof(unsigned long), 1, File);
  fwrite(MW, sizeof(*MW), 1, File);
  MAD_SaveAllTextures(MW, File, SF);
  MAD_SaveAllMaterials(MW, File);
  MAD_SaveAllObjects(MW, File);
  MAD_SaveAllNodes(MW, File);
  if (SF & MAD_SaveTextureFiles) MAD_SaveAllTextureFiles(MW, File);
};
