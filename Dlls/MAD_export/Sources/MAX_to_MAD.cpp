/*$T MAX_to_MAD.cpp GC!1.32 10/20/99 15:08:25 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    MAX_to_MAD.cpp - Ouf File Exporter By Philippe Vimont UBI Pictures mai 99
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include <string.h>
#include "Max.h"
#include "stdmat.h"
#include "dummy.h"
#include "modstack.h"

#include "MAD_mem/Sources/MAD_mem.h"
#include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#include "MAD_RADMOD\MAD_RADMOD.h"

#include "MAX_to_MAD.h"
#include "MAD_export.h"

#include "MixMat.h"

#define MAD_SCALE_EXPORT_FACTOR		0.01f
#define MAD_VERTEX_DEFAULT_COLOR	0xFF000000

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAX_Color_To_MAD_Color(Color *MaxColor, MAD_ColorARGB *MadColor)
{
	float Comp;
	*MadColor = 0;
	Comp = MaxColor->r * 255.0f;
	if (Comp > 255.0f) Comp = 255.0f;
	if (Comp <   0.0f) Comp =   0.0f;
	MAD_SetBlue(*MadColor,(unsigned long )Comp);
	Comp = MaxColor->g * 255.0f;
	if (Comp > 255.0f) Comp = 255.0f;
	if (Comp <   0.0f) Comp =   0.0f;
	MAD_SetGreen(*MadColor,(unsigned long )Comp);
	Comp = MaxColor->b * 255.0f;
	if (Comp > 255.0f) Comp = 255.0f;
	if (Comp <   0.0f) Comp =   0.0f;
	MAD_SetRed(*MadColor,(unsigned long )Comp);
	MAD_SetAlpha(*MadColor,(unsigned long)((unsigned char *)(&MaxColor->b))[0]);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SetIDName(MAD_NodeID *Node, char *Name)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   CharCounter;
    char            *Cpy;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    CharCounter = MAD_NAME_LENGHT;

    Cpy = Node->Name;

    while(*Name)
    {
        CharCounter--;
        *(Cpy++) = *(Name++);
    }

    while(CharCounter--) *(Cpy++) = 0;
}

/*
 ===================================================================================================
    Convert MAX SuperclassID to MAD ID
 ===================================================================================================
 */
ID_MAD_Type MAX_ID_To_MAD_ID(INode *MaxNode, TimeValue t)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    SClass_ID   MAXID;
    ObjectState os;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    os = MaxNode->EvalWorldState(t);

    if(!os.obj)
        return(ID_MAD_UNDEFINED);
    MAXID = os.obj->SuperClassID();
    switch(MAXID)
    {
    case GEOMOBJECT_CLASS_ID:
        if(os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
            return(ID_MAD_TargetObject);
        else
            return(ID_MAD_GeometricObject_V0);
    case CAMERA_CLASS_ID:
        return(ID_MAD_Camera);
    case MATERIAL_CLASS_ID:
        return(ID_MAD_Material);
    case LIGHT_CLASS_ID:
        return(ID_MAD_Light);
    case HELPER_CLASS_ID:
        return(ID_MAD_Dummy);
    case SHAPE_CLASS_ID:
        return(ID_MAD_Shape);
    }

    return ID_MAD_UNDEFINED;
}

/*$5
 ###################################################################################################
    Textures
 ###################################################################################################
 */

/*
 ===================================================================================================
    This will convert only BITMAP TEXTURE TYPE
 ===================================================================================================
 */
MAD_texture *MAX_Tex_To_MAD_Tex(Texmap *MaxTexture, TimeValue t)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_texture *returnStruct;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    returnStruct = NULL;
    if(MaxTexture)
    {
        if(MaxTexture->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00))
        {
            MAD_MALLOC(MAD_texture, returnStruct, 1);
            returnStruct->ID.IDType = ID_MAD_Texture;
            returnStruct->ID.SizeOfThisOne = sizeof(*returnStruct);
            returnStruct->ID.Name[0] = 0;
            strcpy((char *) returnStruct->Texturefile, ((BitmapTex *) MaxTexture)->GetMapName());
        }
    }

    return returnStruct;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
unsigned long M_IsStringEqual(char *srcA, char *srcB)
{
    while((toupper(*srcA) == toupper(*srcB)) && *srcA && *srcB)
    {
        srcA++;
        srcB++;
    }

    if((*srcA | *srcB) == 0) return 1;
    return 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
unsigned long GetIndexFromTexture(MAD_World *MW, Texmap *MaxTexture, TimeValue t)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   textureCounter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(textureCounter = 0; textureCounter < MW->NumberOftexture; textureCounter++)
    {
        if((void *) MaxTexture == (void *) (MW->AllTextures[textureCounter]))
            return(textureCounter);
        else
        {
            if((MaxTexture != NULL) && (MW->AllTextures[textureCounter] != NULL))
            {
                if
                (
                    (((BitmapTex *) MaxTexture)->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00)) &&
                    (
                        ((BitmapTex *) MW->AllTextures[textureCounter])->ClassID() == Class_ID
                            (
                                BMTEX_CLASS_ID,
                                0x00
                            )
                    )
                )
                {
                    if
                    (
                        M_IsStringEqual
                        (
                            ((BitmapTex *) MaxTexture)->GetMapName(),
                            ((BitmapTex *) MW->AllTextures[textureCounter])->GetMapName()
                        )
                    ) return(textureCounter);
                }
            }
        }
    }

    /* Avoid to realloc for each new tecture */

    /* Realloc each 256 tecture */
    if((MW->NumberOftexture & 0xff) == 0)
    {
        MAD_REALLOC(MAD_texture *, MW->AllTextures, (MW->NumberOftexture & 0xffffff00) + 0x100);
    }

    MW->AllTextures[MW->NumberOftexture] = (MAD_texture *) MaxTexture;

    return MW->NumberOftexture++;
}

/*$5
 ###################################################################################################
    Materials
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAD_SetMaterialName(Mtl *MaxMat, MAD_MAT_MatID *MadMaterial)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   CharCounter;
    char            *Cpy, *Name;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Name = MaxMat->GetName();

    CharCounter = MAD_NAME_LENGHT;

    Cpy = MadMaterial->Name;

    while(*Name)
    {
        CharCounter--;
        *(Cpy++) = *(Name++);
    }
    while (CharCounter--) *(Cpy++) = 0;
}

/*
 ===================================================================================================
    A simple material is a material which not contain materials
 ===================================================================================================
 */
MAD_MAT_MatID *MAX_Simple_Mat_To_MAD_Simple_Mat(MAD_World *MW, Mtl *MaxMaterials, TimeValue t)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_StandarMaterial *MMTL;
    Color               ConvertCol;
    int                 TransType;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MAD_MALLOC(MAD_StandarMaterial, MMTL, 1);
    MMTL->MatRef.MaterialType = ID_MAT_Standard;
    MAD_SetMaterialName(MaxMaterials, (MAD_MAT_MatID *) MMTL);
    if(MaxMaterials->ClassID() == Class_ID(DMTL_CLASS_ID, 0))

    /* Here we know that is a standard material */
    {
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        StdMat  *std;
        Texmap  *subTex;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        std = (StdMat *) MaxMaterials;

        ConvertCol = std->GetDiffuse(t);
        MAX_Color_To_MAD_Color(&ConvertCol, &MMTL->Diffuse);
        ConvertCol = std->GetAmbient(t);
        MAX_Color_To_MAD_Color(&ConvertCol, &MMTL->Ambient);
        ConvertCol = std->GetSpecular(t);
        MAX_Color_To_MAD_Color(&ConvertCol, &MMTL->Specular);
        MMTL->Opacity = std->GetOpacity(t);
        MMTL->SelfIllum = std->GetSelfIllum(t);
        TransType = std->GetTransparencyType();
        MMTL->MaterialFlag = 0;
        switch(TransType)
        {
        case TRANSP_SUBTRACTIVE:
            MMTL->MaterialFlag =
                (MMTL->MaterialFlag &~MAD_MTF_TransparencyModeMask) | MAD_MTF_Trans_Mul;
            break;
        case TRANSP_ADDITIVE:
            MMTL->MaterialFlag =
                (MMTL->MaterialFlag &~MAD_MTF_TransparencyModeMask) | MAD_MTF_Trans_Add;
            break;
        case TRANSP_FILTER:
        default:
            MMTL->MaterialFlag =
                (MMTL->MaterialFlag &~MAD_MTF_TransparencyModeMask) | MAD_MTF_Trans_Copy;
            break;
        }

        MMTL->MadTexture = MAD_NULL_INDEX;

        /* Load diffuse text map; */
        subTex = MaxMaterials->GetSubTexmap(ID_DI);
        if(subTex && (std->MapEnabled(ID_DI)))
        {
            if(subTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00))
            {
                /* Here we know that is a texture file .TGA , .BMP , or ?? */
                MMTL->MadTexture = GetIndexFromTexture(MW, subTex, t);
                TransType = ((BitmapTex *) subTex)->GetTextureTiling();
                if(TransType & U_WRAP)
                    MMTL->MaterialFlag |= MAD_MTF_UTiling;
                if(TransType & V_WRAP)
                    MMTL->MaterialFlag |= MAD_MTF_VTiling;
                if(TransType & U_MIRROR)
                    MMTL->MaterialFlag |= MAD_MTF_UMirror;
                if(TransType & U_MIRROR)
                    MMTL->MaterialFlag |= MAD_MTF_VMirror;
                if(((BitmapTex *) subTex)->GetTexout()->GetInvert())
                    MMTL->MaterialFlag |= MAD_MTF_InvertColor;
                if(((BitmapTex *) subTex)->GetPremultAlpha(TRUE))
                    MMTL->MaterialFlag =
                        (MMTL->MaterialFlag &~MAD_MTF_TransparencyModeMask)
                            | MAD_MTF_Trans_AlphaPremult;
            }
        }
    }

    return (MAD_MAT_MatID *) MMTL;
};

/*
 ===================================================================================================
    A complex material is a material which contain materials
 ===================================================================================================
 */
MAD_MAT_MatID *MAX_Cplx_Mat_To_MAD_Cplx_Mat(MAD_World *MW, Mtl **SrcMtls, Mtl *Current, TimeValue t)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_MultiMaterial   *MMTL;
    Mtl                 *LocalMtl;
    unsigned long       i, j;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MAD_MALLOC(MAD_MultiMaterial, MMTL, 1);
    MMTL->MatRef.MaterialType = ID_MAT_MultiMaterial;
    MAD_SetMaterialName(Current, (MAD_MAT_MatID *) MMTL);
    MMTL->NumberOfSubMaterials = Current->NumSubMtls();
    MAD_MALLOC(unsigned long, MMTL->SubMats, MMTL->NumberOfSubMaterials);
    for(i = 0; i < MMTL->NumberOfSubMaterials; i++)
    {
        LocalMtl = Current->GetSubMtl(i);
        MMTL->SubMats[i] = MAD_NULL_INDEX;
        for(j = 0; j < MW->NumberOfMaterials; j++)
        {
            if(SrcMtls[j] == LocalMtl)
            {
                MMTL->SubMats[i] = j;
                break;
            }
        }
    }

    return (MAD_MAT_MatID *) MMTL;
};

/*
 ===================================================================================================
    Main converter;
 ===================================================================================================
 */
void MAX_Mat_To_MAD_Mat(MAD_World *MW, Mtl **SrcMtls, TimeValue t)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    for(i = 0; i < MW->NumberOfMaterials; i++)
    {
        if(SrcMtls[i]->NumSubMtls() == 0)
            /* Simle material */
            MW->AllMaterial[i] = MAX_Simple_Mat_To_MAD_Simple_Mat(MW, SrcMtls[i], t);
        else
            /* Complex material */
            MW->AllMaterial[i] = MAX_Cplx_Mat_To_MAD_Cplx_Mat(MW, SrcMtls, SrcMtls[i], t);
    }
};

/*$5
 ###################################################################################################
    Geometric Objects
 ###################################################################################################
 */

MAD_GeometricObject *Mesh_To_MAD_Mesh
(
    Mesh		*mesh,
    MAD_NodeID  *MADNode,
	MAD_ColorARGB *SecondRliField,
    TimeValue   t
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_GeometricObject         MGO, *returnStruct;
    MAD_GeometricObjectElement  **CurrentElement;
    unsigned long               Counter, Counter2, Counter3;
    unsigned long               Element_To_MaterialID[MAX_MAX_Mtl_Per_OBJECT];
    unsigned long               NumberOffacesPerMaterrial[MAX_MAX_Mtl_Per_OBJECT];
    unsigned long               RealSize;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    memset(&MGO, 0, sizeof(MAD_GeometricObject));

    MGO.TypeOfThis = MAD_MAXMESH_V0;    /* First version Of geometric Objects */

    MGO.NumberOfUV = 0;

    MGO.NumberOfPoints = mesh->getNumVerts();
    MAD_MALLOC(MAD_Simple3DVertex, MGO.OBJ_PointList, MGO.NumberOfPoints);

    RealSize = sizeof(MAD_Simple3DVertex) * MGO.NumberOfPoints;

    MGO.NumberOfUV = mesh->getNumTVerts();
    if(MGO.NumberOfUV)
    {
        MAD_MALLOC(MAD_Vertex, MGO.OBJ_UVtextureList, MGO.NumberOfUV);
        RealSize += sizeof(MAD_Vertex) * MGO.NumberOfUV;
    }

	FILE *flog = _tfopen("log.txt", _T("at"));
	fprintf(flog, "\tNumberOfPoints = %d\n", MGO.NumberOfPoints);
	fprintf(flog, "\tNumberOfUV = %d\n", MGO.NumberOfUV);
	fprintf(flog, "\tNumber of faces = %d\n", mesh->getNumFaces());
	fprintf(flog, "\t%d maps supported\n", mesh->getNumMaps());
	if(mesh->mapSupport(0))
		fprintf(flog, "\t\tMapSupport 0 with %d verts\n", mesh->getNumMapVerts(0));
	if(mesh->mapSupport(1))
		fprintf(flog, "\t\tMapSupport 1 with %d verts\n", mesh->getNumMapVerts(1));
	if(mesh->mapSupport(MAP_ALPHA))
		fprintf(flog, "\t\tMapSupport alpha with %d verts\n", mesh->getNumMapVerts(MAP_ALPHA));
	fclose(flog);

	MAD_MALLOC(MAD_ColorARGB, MGO.Colors, MGO.NumberOfPoints);
	RealSize += sizeof(MAD_ColorARGB) * MGO.NumberOfPoints;

	// Init vert col
	for(Counter = 0; Counter < MGO.NumberOfPoints; Counter++)
		MGO.Colors[Counter] = MAD_VERTEX_DEFAULT_COLOR;
	// Fill with true color if possible
	if (mesh->mapSupport(0))
	{
		//VertColor *theVertCol = mesh->vertCol;
		//if (!theVertCol)
		//	// I don't know if this works
		//	theVertCol = mesh->mapVerts(0);
		//if (theVertCol)
		//{
		//	for(Counter = 0; Counter < MGO.NumberOfPoints; Counter++)
		//	{
		//		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		//		float   Swap;
		//		MAD_Vertex Convert;
		//		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		//		Convert = *(MAD_Vertex *)&theVertCol[Counter];
		//		Convert . x *= 255.0f;
		//		Convert . y *= 255.0f;
		//		Convert . z *= 255.0f;

		//		Swap = Convert.x;
		//		Convert.x = Convert.z;
		//		Convert.z = Swap;
		//		MAD_Vertex_to_MAD_ColorARGB(Convert,MGO.Colors[Counter]);
		//	}
		//}

		UVVert *theVertCol = mesh->mapVerts(0);
		TVFace *mapFaces = mesh->mapFaces(0);
		TVFace *aMapFace;
		Face *aFace;
		for (int i=0; i<mesh->getNumFaces(); i++)
		{
			//flog = _tfopen("log.txt", _T("at"));
			//fprintf(flog, "Face %d :\n", i);
			aMapFace = mapFaces + i;
			aFace = mesh->faces + i;
			for (int j=0; j<3; j++)
			{
				//fprintf(flog, "\tVertex %d (%d) : %f\n", aFace->getVert(j), aMapFace->getTVert(j), alpha[aMapFace->getTVert(j)].x);
				if (aFace->getVert(j) < MGO.NumberOfPoints)
				{
					/*~~~~~~~~~~~~~~~~~~~~~*/
					float   Swap;
					MAD_Vertex Convert;
					/*~~~~~~~~~~~~~~~~~~~~~*/

					// DEBUG
					int idx;
					idx = aFace->getVert(j);

					Convert = *(MAD_Vertex *)&theVertCol[aMapFace->getTVert(j)];
					Convert . x *= 255.0f;
					Convert . y *= 255.0f;
					Convert . z *= 255.0f;

					Swap = Convert.x;
					Convert.x = Convert.z;
					Convert.z = Swap;
					MAD_Vertex_to_MAD_ColorARGB(Convert, MGO.Colors[aFace->getVert(j)]);
				}
				//fprintf(flog, "\t\t0x%x\n", MGO.Colors[aFace->getVert(j)]);
			}
			// fclose(flog);
		}
	}

	// ADD : Gilles, 14/03/06

	//flog = _tfopen("log.txt", _T("at"));
	//fprintf(flog, "\nLet's go alpha !\n");
	//fclose(flog);
	if(mesh->mapSupport(MAP_ALPHA))
	{
		//flog = _tfopen("log.txt", _T("at"));
		//fprintf(flog, "there's %d faces, %d alpha verts and %d verts\n", mesh->getNumFaces(), mesh->getNumMapVerts(MAP_ALPHA), MGO.NumberOfPoints);
		//fclose(flog);

		UVVert *alpha = mesh->mapVerts(MAP_ALPHA);
		TVFace *mapFaces = mesh->mapFaces(MAP_ALPHA);
		TVFace *aMapFace;
		Face *aFace;
		for (int i=0; i<mesh->getNumFaces(); i++)
		{
			//flog = _tfopen("log.txt", _T("at"));
			//fprintf(flog, "Face %d :\n", i);
			aMapFace = mapFaces + i;
			aFace = mesh->faces + i;
			for (int j=0; j<3; j++)
			{
				//fprintf(flog, "\tVertex %d (%d) : %f\n", aFace->getVert(j), aMapFace->getTVert(j), alpha[aMapFace->getTVert(j)].x);
				if (aFace->getVert(j) < MGO.NumberOfPoints)
					MAD_SetAlpha(MGO.Colors[aFace->getVert(j)], (unsigned char)(255.0f * alpha[aMapFace->getTVert(j)].x));
				//fprintf(flog, "\t\t0x%x\n", MGO.Colors[aFace->getVert(j)]);
			}
			// fclose(flog);
		}

		//int numIter;
		//if (mesh->getNumMapVerts(MAP_ALPHA) >= MGO.NumberOfPoints)
		//	numIter = MGO.NumberOfPoints;
		//else
		//	numIter = mesh->getNumMapVerts(MAP_ALPHA);
		//UVVert *alpha = mesh->mapVerts(MAP_ALPHA);
		//for(int i=0; i < numIter;i++)
		//{
		//	flog = _tfopen("log.txt", _T("at"));
		//	fprintf(flog, "%d : %f - 0x%x - 0x%x", i, alpha[i].x, (unsigned short)(255.0f * alpha[i].x), MGO.Colors[i]);
		//	fclose(flog);
		//	MAD_SetAlpha(MGO.Colors[i], (unsigned char)(255.0f * alpha[i].x));
		//	flog = _tfopen("log.txt", _T("at"));
		//	fprintf(flog, " - 0x%x\n", MGO.Colors[i]);
		//	fclose(flog);
		//}
	}
	//flog = _tfopen("log.txt", _T("at"));
	//fprintf(flog, "Fini\n");
	//fclose(flog);

	// End ADD

	if (SecondRliField)
	{
		MAD_MALLOC(MAD_ColorARGB, MGO.SecondRLIField, MGO.NumberOfPoints);
		RealSize += sizeof(MAD_ColorARGB) * MGO.NumberOfPoints;
		memcpy(MGO.SecondRLIField , SecondRliField , sizeof(MAD_ColorARGB) * MGO.NumberOfPoints);
	}

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Convert vertices & UV
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    for(Counter = 0; Counter < MGO.NumberOfPoints; Counter++)
    {
        *(Point3 *) &MGO.OBJ_PointList[Counter].Point = mesh->getVert(Counter);
        MGO.OBJ_PointList[Counter].Point.x *= MAD_SCALE_EXPORT_FACTOR;
        MGO.OBJ_PointList[Counter].Point.y *= MAD_SCALE_EXPORT_FACTOR;
        MGO.OBJ_PointList[Counter].Point.z *= MAD_SCALE_EXPORT_FACTOR;
    }

    if(MGO.NumberOfUV)
    {
        for(Counter = 0; Counter < MGO.NumberOfUV; Counter++)
        {
            *(Point3 *) &MGO.OBJ_UVtextureList[Counter] = mesh->getTVert(Counter);
        }
    }

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Compute number of materials & NumberOffacesPerMaterrial
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    for(Counter = 0; Counter < MAX_MAX_Mtl_Per_OBJECT; Counter++)
    {
        NumberOffacesPerMaterrial[Counter] = 0;
    }

    Counter = mesh->getNumFaces();
    while(Counter--)
    {
        Counter2 = mesh->getFaceMtlIndex(Counter) % MAX_MAX_Mtl_Per_OBJECT;
        NumberOffacesPerMaterrial[Counter2]++;
    }

    Counter2 = 0;
    for(Counter = 0; Counter < MAX_MAX_Mtl_Per_OBJECT; Counter++)
    {
        if(NumberOffacesPerMaterrial[Counter] != 0)
        {
            Element_To_MaterialID[Counter2] = Counter;
            NumberOffacesPerMaterrial[Counter2] = NumberOffacesPerMaterrial[Counter];
            Counter2++;
        }
    }

    MGO.NumberOfElements = Counter2;

    /*$1
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Compute each element
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */

    MAD_MALLOC(MAD_GeometricObjectElement *, MGO.Elements, MGO.NumberOfElements);
    RealSize += sizeof(MAD_GeometricObjectElement *) * MGO.NumberOfElements;
    CurrentElement = MGO.Elements;
    for(Counter = 0; Counter < MGO.NumberOfElements; Counter++)
    {
        MAD_MALLOC(MAD_GeometricObjectElement, *CurrentElement, 1);
        RealSize += sizeof(MAD_GeometricObjectElement);
        (*CurrentElement)->NumberOfTriangles = NumberOffacesPerMaterrial[Counter];
        (*CurrentElement)->MaterialID = Element_To_MaterialID[Counter];
        MAD_MALLOC(MAD_Face, (*CurrentElement)->Faces, (*CurrentElement)->NumberOfTriangles);
        RealSize += sizeof(MAD_Face) * (*CurrentElement)->NumberOfTriangles;
        Counter3 = 0;
        for(Counter2 = 0; Counter2 < (ULONG) mesh->getNumFaces(); Counter2++)
        {
            if
            (
                (*CurrentElement)->MaterialID ==
                    (((unsigned long) mesh->getFaceMtlIndex(Counter2)) % MAX_MAX_Mtl_Per_OBJECT)
            )
            {
                /* Convert each face */
                (*CurrentElement)->Faces[Counter3].Index[0] = mesh->faces[Counter2].v[0];
                (*CurrentElement)->Faces[Counter3].Index[1] = mesh->faces[Counter2].v[1];
                (*CurrentElement)->Faces[Counter3].Index[2] = mesh->faces[Counter2].v[2];
                if(MGO.NumberOfUV)
                {
                    (*CurrentElement)->Faces[Counter3].UVIndex[0] = mesh->tvFace[Counter2].t[0];
                    (*CurrentElement)->Faces[Counter3].UVIndex[1] = mesh->tvFace[Counter2].t[1];
                    (*CurrentElement)->Faces[Counter3].UVIndex[2] = mesh->tvFace[Counter2].t[2];
                }
                (*CurrentElement)->Faces[Counter3].SmoothingGroup = mesh->faces[Counter2].smGroup;
                (*CurrentElement)->Faces[Counter3].MAXflags = mesh->faces[Counter2].flags;
                Counter3++;
            }
        }

        CurrentElement++;
    }

    MAD_MALLOC(MAD_GeometricObject, returnStruct, 1);
    *returnStruct = MGO;
    *(MAD_NodeID *) returnStruct = *MADNode;
    returnStruct->ID.SizeOfThisOne = sizeof(*returnStruct) + RealSize;
    return returnStruct;
};
/*
 ===================================================================================================
    Convert max geometric object to MAD geometric Object
 ===================================================================================================
 */
MAD_GeometricObject *MAX_Object_To_MAD_Object
(
    INode       *MAXNode,
    MAD_NodeID  *MADNode,
    MAD_World   *MW,
    TimeValue   t
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TriObject                   *tri;
    Object                      *obj,*ObjREf;
	IDerivedObject				*DrvdObj;
    MAD_GeometricObject         MGO, *returnStruct;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    obj = MAXNode->EvalWorldState(t).obj;

    if(obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
    {
		FILE *flog = _tfopen("log.txt", _T("at"));
		fprintf(flog, "Triobj\n");
		fclose(flog);
		ObjREf = MAXNode->GetObjectRef();
		if(ObjREf != NULL) 
			if (ObjREf -> ClassID() == derivObjClassID)
			{
				FILE *flog = _tfopen("log.txt", _T("at"));
				fprintf(flog, "Deriv obj\n");
				fclose(flog);
				DrvdObj = (IDerivedObject *)ObjREf;
				if (DrvdObj -> NumModifiers())
				{
					if (DrvdObj -> GetModifier(0) -> ClassID() == MAD_RadMod_CLASS_ID)
					{
						FILE *flog = _tfopen("log.txt", _T("at"));
						fprintf(flog, "obj = GetObjRef()\n");
						fclose(flog);
						obj = DrvdObj->GetObjRef();
					}
				}
			}
        tri = (TriObject *) obj->ConvertToType(t, Class_ID(TRIOBJ_CLASS_ID, 0));
		if (!tri)
		{
			FILE *flog = _tfopen("log.txt", _T("at"));
			fprintf(flog, "Not tri\n");
			fclose(flog);
			tri = (TriObject *) MAXNode->EvalWorldState(t).obj->ConvertToType(t, Class_ID(TRIOBJ_CLASS_ID, 0));
		}
    }
    else
    {
		FILE *flog = _tfopen("log.txt", _T("at"));
		fprintf(flog, "Not Triobj\n");
		fclose(flog);

		MAD_MALLOC(MAD_GeometricObject, returnStruct, 1);
	    memset(&MGO, 0, sizeof(MAD_GeometricObject));
	    MGO.TypeOfThis = MAD_MAXMESH_V0;    /* First version Of geometric Objects */
		*returnStruct = MGO;
		*(MAD_NodeID *) returnStruct = *MADNode;
		returnStruct->ID.SizeOfThisOne = sizeof(*returnStruct);
		return returnStruct;
    }
#if defined(MAX3) || defined(MAX5) || defined(MAX7) || defined(MAX8)
	return (Mesh_To_MAD_Mesh(&tri->GetMesh(), MADNode , NULL, t));
#else
	return (Mesh_To_MAD_Mesh(&tri->Mesh(), MADNode , NULL, t));
#endif
};

/*$5
 ###################################################################################################
    Light
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
MAD_Light *MAX_Light_To_MAD_Light(INode *MAXNode, MAD_NodeID *MADNode, MAD_World *MW, TimeValue t)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GenLight            *light;
    struct LightState   ls;
    Interval            valid;
    MAD_Light           ML, *returnStruct;
    ObjectState         os;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    os = MAXNode->EvalWorldState(t);
    if(!os.obj)
    {
        return NULL;
    }

    light = (GenLight *) os.obj;
    valid = FOREVER;
    memset(&ML, 0, sizeof(MAD_Light));

    light->EvalLightState(t, valid, &ls);
    ML.Multiplier = ls.intens;
    MAX_Color_To_MAD_Color(&ls.color, &ML.LightColor);
    ML.IsOn = ls.on;
    if(ls.shadow)
        ML.LightFlags |= MAD_LightFlag_CastShadow;
    if(light->GetUseGlobal())
        ML.LightFlags |= MAD_LightFlag_OverWalls;
    if(light->GetConeDisplay())
        ML.LightFlags |= MAD_LightFlag_PaintLight;
    switch(ls.type)
    {
    case OMNI_LIGHT:
        ML.LightType = MAD_LightOmni;
        ML.Omni.Near = ls.attenStart * MAD_SCALE_EXPORT_FACTOR;
        ML.Omni.Far = ls.attenEnd * MAD_SCALE_EXPORT_FACTOR;
        ML.Omni.RadiusOfLight = ls.nearAttenStart * MAD_SCALE_EXPORT_FACTOR;
        break;
    case FSPOT_LIGHT:
    case TSPOT_LIGHT:
        ML.LightType = MAD_LightSpot;
        ML.Spot.Near = ls.attenStart * MAD_SCALE_EXPORT_FACTOR;
        ML.Spot.Far = ls.attenEnd * MAD_SCALE_EXPORT_FACTOR;
        ML.Spot.LittleAlpha = ls.hotsize * 3.1415927f / 180.0f;
        ML.Spot.BigAlpha = ls.fallsize * 3.1415927f / 180.0f;
        ML.Spot.RadiusOfLight = ls.nearAttenStart * MAD_SCALE_EXPORT_FACTOR;
        break;
    case DIR_LIGHT:
        ML.LightType = MAD_LightDirect;
        break;
    }

    MAD_MALLOC(MAD_Light, returnStruct, 1);
    *returnStruct = ML;
    *(MAD_NodeID *) returnStruct = *MADNode;
    returnStruct->ID.SizeOfThisOne = sizeof(*returnStruct);
    return returnStruct;
};

/*$5
 ###################################################################################################
    Camera
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
MAD_Camera *MAX_Cam_To_MAD_Cam(INode *MAXNode, MAD_NodeID *MADNode, MAD_World *MW, TimeValue t)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CameraState     cs;
    MAD_Camera      MC, *returnStruct;
    Interval        valid;
    CameraObject    *cam;
    Object          *ObjREf;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ObjREf = MAXNode->GetObjectRef();
    valid = FOREVER;
    cam = (CameraObject *) ObjREf;

    cam->EvalCameraState(t, valid, &cs);

    if(cs.isOrtho)
    {
        MC.CameraType = MAD_e_OrthoCamera;
        MC.OrthoInf.ChannelSize = MAD_SCALE_EXPORT_FACTOR * cs.fov * 180.0f / 3.141592765f;
        MC.OrthoInf.Znear = cs.nearRange * MAD_SCALE_EXPORT_FACTOR;
        MC.OrthoInf.Zfar = cs.farRange * MAD_SCALE_EXPORT_FACTOR;
    }
    else    /* Perspectiv camera */
    {
        MC.CameraType = MAD_e_PerspCamera;
        MC.PerspInf.Fov = cs.fov;
        MC.PerspInf.Znear = cs.nearRange * MAD_SCALE_EXPORT_FACTOR;
        MC.PerspInf.Zfar = cs.farRange * MAD_SCALE_EXPORT_FACTOR;
    }

    MAD_MALLOC(MAD_Camera, returnStruct, 1);
    *returnStruct = MC;
    *(MAD_NodeID *) returnStruct = *MADNode;
    returnStruct->ID.SizeOfThisOne = sizeof(*returnStruct);
    return returnStruct;
};

/*$5
 ###################################################################################################
    Dummy
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
MAD_Dummy *MAX_Dum_To_MAD_Dum(INode *MAXNode, MAD_NodeID *MADNode, MAD_World *MW, TimeValue t)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Class_ID        MAXID;
    MAD_Dummy       *returnStruct;
    ObjectState     os;
    Box3            Box;
    unsigned long   C;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    os = MAXNode->EvalWorldState(t);

    if(!os.obj) return NULL;
    MAXID = os.obj->ClassID();
    MAD_MALLOC(MAD_Dummy, returnStruct, 1);
    returnStruct->Type = 0;
    C = MAXID.PartA();

    switch(C)
    {
    case DUMMY_CLASS_ID:
        returnStruct->Type = DUMMY_BOX;
        Box = ((DummyObject *) os.obj)->GetBox();
        *(Point3 *) &returnStruct->BOXMin = Box.Min();
        returnStruct->BOXMin.x *= MAD_SCALE_EXPORT_FACTOR;
        returnStruct->BOXMin.y *= MAD_SCALE_EXPORT_FACTOR;
        returnStruct->BOXMin.z *= MAD_SCALE_EXPORT_FACTOR;
        *(Point3 *) &returnStruct->BOXMax = Box.Max();
        returnStruct->BOXMax.x *= MAD_SCALE_EXPORT_FACTOR;
        returnStruct->BOXMax.y *= MAD_SCALE_EXPORT_FACTOR;
        returnStruct->BOXMax.z *= MAD_SCALE_EXPORT_FACTOR;
        break;
    case BONE_CLASS_ID:
        returnStruct->Type = DUMMY_BONE;
        break;
    case TAPEHELP_CLASS_ID:
        break;
    case GRIDHELP_CLASS_ID:
        break;
    case POINTHELP_CLASS_ID:
        returnStruct->Type = DUMMY_POINT;
        break;
    case PROTHELP_CLASS_ID:
        break;
    default:
        Object * helperObj = MAXNode->EvalWorldState(0).obj;
        if(helperObj)
        {
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            TSTR    className;
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

            helperObj->GetClassName(className);
        }

        break;
    }

    *(MAD_NodeID *) returnStruct = *MADNode;
    returnStruct->ID.SizeOfThisOne = sizeof(*returnStruct);
    return returnStruct;
};

/*$5
 ###################################################################################################
    Shape
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
MAD_Shape *MAX_Shape_To_MAD_Shape(INode *MAXNode, MAD_NodeID *MADNode, MAD_World *MW, TimeValue t)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Shape   *returnStruct;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MAD_MALLOC(MAD_Shape, returnStruct, 1);
    *(MAD_NodeID *) returnStruct = *MADNode;
    returnStruct->ID.SizeOfThisOne = sizeof(*returnStruct);
    returnStruct->TypeOfShape = ID_MAT_SPLINE3D;
    returnStruct->Spline.NumberOfKnots = 0;
    return returnStruct;
};

/*$5
 ###################################################################################################
    Target
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
MAD_Target *MAX_Target_To_MAD_Target
(
    INode       *MAXNode,
    MAD_NodeID  *MADNode,
    MAD_World   *MW,
    TimeValue   t
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Target  *returnStruct;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MAD_MALLOC(MAD_Target, returnStruct, 1);
    *(MAD_NodeID *) returnStruct = *MADNode;
    returnStruct->ID.SizeOfThisOne = sizeof(*returnStruct);
    return returnStruct;
};

/*$5
 ###################################################################################################
    Main converter
 ###################################################################################################
 */

/*
 ===================================================================================================
 ===================================================================================================
 */
MAD_NodeID *MAX_Unknown(INode *MAXNode, MAD_NodeID *MADNode, MAD_World *MW, TimeValue t)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_NodeID  *returnStruct;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MAD_MALLOC(MAD_NodeID, returnStruct, 1);
    *(MAD_NodeID *) returnStruct = *MADNode;
    returnStruct->SizeOfThisOne = sizeof(*returnStruct);
    return returnStruct;
};

/*
 ===================================================================================================
 ===================================================================================================
 */
MAD_NodeID *MAX_X_To_MAD_X(INode *MAXNode, MAD_World *MW, TimeValue t)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_NodeID  MADNode, *Ret;
    ObjectState os;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    os = MAXNode->EvalWorldState(0);

    MAX_NodeToMAD_Node(MAXNode, &MADNode, t);
    Ret = NULL;
	FILE *flog;
    switch(MADNode.IDType)
    {
    case ID_MAD_GeometricObject_V0:
		flog = _tfopen("log.txt", _T("at"));
		fprintf(flog, "Geom obj\n");
		fclose(flog);
        Ret = (MAD_NodeID *) MAX_Object_To_MAD_Object(MAXNode, &MADNode, MW, t);
        break;
    case ID_MAD_Light:
        Ret = (MAD_NodeID *) MAX_Light_To_MAD_Light(MAXNode, &MADNode, MW, t);
        break;
    case ID_MAD_Camera:
        Ret = (MAD_NodeID *) MAX_Cam_To_MAD_Cam(MAXNode, &MADNode, MW, t);
        break;
    case ID_MAD_Dummy:
        Ret = (MAD_NodeID *) MAX_Dum_To_MAD_Dum(MAXNode, &MADNode, MW, t);
        break;
    case ID_MAD_Shape:
        Ret = (MAD_NodeID *) MAX_Shape_To_MAD_Shape(MAXNode, &MADNode, MW, t);
        break;
    case ID_MAD_TargetObject:
        Ret = (MAD_NodeID *) MAX_Target_To_MAD_Target(MAXNode, &MADNode, MW, t);
        break;
    }

    if(Ret == NULL)
        Ret = MAX_Unknown(MAXNode, &MADNode, MW, t);

    return Ret;
};

/*
 ===================================================================================================
 ===================================================================================================
 */
void MAX_Matrix_To_MAD_Matrix(Matrix3 *Matrix, MAD_Matrix *MADMatrix, TimeValue t)
{
    MADMatrix->ID.IDType = ID_MAD_Matrix;
    MADMatrix->ID.SizeOfThisOne = sizeof(*MADMatrix);
    MADMatrix->Translation = *((MAD_Vertex *) &Matrix->GetTrans());
    MADMatrix->Translation.x *= MAD_SCALE_EXPORT_FACTOR;
    MADMatrix->Translation.y *= MAD_SCALE_EXPORT_FACTOR;
    MADMatrix->Translation.z *= MAD_SCALE_EXPORT_FACTOR;
    MADMatrix->I = *((MAD_Vertex *) &Matrix->GetRow(0));
    MADMatrix->J = *((MAD_Vertex *) &Matrix->GetRow(1));
    MADMatrix->K = *((MAD_Vertex *) &Matrix->GetRow(2));
}

/*
 ===================================================================================================
    Convertion
 ===================================================================================================
 */
void MAX_NodeToMAD_Node(INode *MAXNode, MAD_NodeID *MADNode, TimeValue t)
{
    Control *Controlos;
    MADNode->IDType = MAX_ID_To_MAD_ID(MAXNode, t);
    MAD_SetIDName(MADNode, (char *)MAXNode->GetName());
    ((MAD_WorldNode *) MADNode)->NODE_Flags = 0;
    if(MAXNode->IsGroupHead())
        ((MAD_WorldNode *) MADNode)->NODE_Flags |= MNF_IsGroupHead;
    if(MAXNode->IsGroupMember())
        ((MAD_WorldNode *) MADNode)->NODE_Flags |= MNF_IsGroupMember;
	((MAD_WorldNode *) MADNode)->ObjectWithRadiosity = MAD_NULL_INDEX;
	((MAD_WorldNode *) MADNode)->RLIOfObject		 = NULL;
    Controlos = MAXNode->GetTMController();
    if(Controlos)
    {
        if(ID_MAD_GeometricObject_V0 == MADNode->IDType)
        {
            if(Controlos->ClassID() == Class_ID(LOOKAT_CONTROL_CLASS_ID, 0))
                ((MAD_WorldNode *) MADNode)->NODE_Flags |= MNF_IsLookAt;
            }
    }
};
