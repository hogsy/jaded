/*$T MAD_Util.c GC!1.32 10/19/99 17:48:26 */

/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#include <memory.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include "MAD_mem/Sources/MAD_mem.h"
#include "MAD_Struct_V0.h"

unsigned long  MAD_VerifyVertexColorAreNotTo0(MAD_GeometricObject *MO)
{
	unsigned long Counter;
	MAD_ColorARGB SumCol;
	SumCol =  0;
	if (!MO -> Colors) return 0;
    for(Counter = 0; Counter < MO->NumberOfPoints; Counter++)
    {
        SumCol |= MO->Colors [ Counter ];
    }
	if (SumCol & 0xfcfcfc) return 1;

	return 0;
}


MAD_GeometricObject *MAD_CopyGo(MAD_GeometricObject *GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter;
	MAD_GeometricObject *ReturnGO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAD_MALLOC(MAD_GeometricObject, ReturnGO, 1);
	memcpy(ReturnGO , GO , sizeof (*GO));

	MAD_MALLOC(MAD_Simple3DVertex, ReturnGO->OBJ_PointList , ReturnGO->NumberOfPoints);
	memcpy(ReturnGO ->OBJ_PointList , GO ->OBJ_PointList , sizeof (MAD_Simple3DVertex) * ReturnGO->NumberOfPoints);

	MAD_MALLOC(MAD_Vertex, ReturnGO->OBJ_UVtextureList , ReturnGO->NumberOfUV); 
	memcpy(ReturnGO ->OBJ_UVtextureList , GO ->OBJ_UVtextureList , sizeof (MAD_Vertex) * ReturnGO->NumberOfUV);

	MAD_MALLOC(MAD_ColorARGB, ReturnGO->Colors , ReturnGO->NumberOfPoints);
	memcpy(ReturnGO ->Colors , GO ->Colors , sizeof (MAD_ColorARGB) * ReturnGO->NumberOfPoints);

	MAD_MALLOC(MAD_GeometricObjectElement *, ReturnGO->Elements , ReturnGO->NumberOfElements);
	memcpy(ReturnGO ->Elements , GO ->Elements , sizeof (MAD_GeometricObjectElement *) * ReturnGO->NumberOfElements);

	for(Counter = 0; Counter < ReturnGO->NumberOfElements; Counter++)
	{
		MAD_MALLOC(MAD_GeometricObjectElement, ReturnGO->Elements[Counter] , 1);
		memcpy(ReturnGO ->Elements[Counter] , GO ->Elements[Counter] , sizeof (MAD_GeometricObjectElement) * 1);

		MAD_MALLOC(MAD_Face, ReturnGO->Elements[Counter]->Faces , GO->Elements[Counter]->NumberOfTriangles);
		memcpy(ReturnGO ->Elements[Counter]->Faces , GO->Elements[Counter]->Faces , sizeof (MAD_Face) * GO->Elements[Counter]->NumberOfTriangles);
	}

	return ReturnGO;
}
/*
===================================================================================================
===================================================================================================
*/
void MAD_CleanUnusedObject(MAD_World *MW)
{
	unsigned long *Reorder;
	unsigned long Counter,Counter2;
#ifdef JADEFUSION
	Reorder = (unsigned long*)malloc(MW->NumberOfObjects * 4L);
#else
	Reorder = malloc(MW->NumberOfObjects * 4L);
#endif
	memset((void*)Reorder,0,MW->NumberOfObjects * 4L);
	/* tagtging */ 
	for (Counter=0;Counter<MW->NumberOfHierarchieNodes;Counter++)
	{
		if(MW->Hierarchie[Counter].Object != MAD_NULL_INDEX)
			Reorder[MW->Hierarchie[Counter].Object] = 1;
		if(MW->Hierarchie[Counter].ObjectWithRadiosity != MAD_NULL_INDEX)
			Reorder[MW->Hierarchie[Counter].ObjectWithRadiosity] = 1;
	}
	Counter2 = 0;
	for (Counter=0;Counter<MW->NumberOfObjects;Counter++)
	{
		if (Reorder[Counter] != 0)
			Reorder[Counter] = Counter2++;
		else
			Reorder[Counter] = Counter2;
		MW->AllObjects[Reorder[Counter]] = MW->AllObjects[Counter];
	}
	MW->NumberOfObjects = Counter2;
	MAD_REALLOC(MAD_NodeID *, MW->AllObjects , Counter2 );
	for (Counter=0;Counter<MW->NumberOfHierarchieNodes;Counter++)
	{
		if(MW->Hierarchie[Counter].Object != MAD_NULL_INDEX)
			MW->Hierarchie[Counter].Object = Reorder[MW->Hierarchie[Counter].Object];
		if(MW->Hierarchie[Counter].ObjectWithRadiosity != MAD_NULL_INDEX)
			MW->Hierarchie[Counter].ObjectWithRadiosity = Reorder[MW->Hierarchie[Counter].ObjectWithRadiosity];
	}
    free( Reorder );
	
}
/*
===================================================================================================
===================================================================================================
*/
void MAD_MakeNoHinstancesGO(MAD_World *MW)
{

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter;
	unsigned long NumBerOfObjects;
	char TransformaX[MAD_NAME_LENGHT];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAD_REALLOC(MAD_NodeID *, MW->AllObjects , MW->NumberOfHierarchieNodes + MW->NumberOfObjects );
	NumBerOfObjects = MW->NumberOfObjects;
    for(Counter = 0; Counter < MW->NumberOfHierarchieNodes; Counter++)
    {
		MW->Hierarchie[Counter].NODE_Flags &= ~MNF_MustBeMadRaded;
        if(MW->Hierarchie[Counter].Object != MAD_NULL_INDEX)
        {
            if(MW->AllObjects[MW->Hierarchie[Counter].Object]->IDType == ID_MAD_GeometricObject_V0)
            {
				if (MW->Hierarchie[Counter].ObjectWithRadiosity == MAD_NULL_INDEX)
				{
					MW->AllObjects[NumBerOfObjects] = (MAD_NodeID *)MAD_CopyGo((MAD_GeometricObject *)MW->AllObjects[MW->Hierarchie[Counter].Object]);
					((MAD_GeometricObject *) MW->AllObjects[NumBerOfObjects])->ObjectFlags |= GO_ShadowCut;
					MAD_MALLOC(MAD_ColorARGB, ((MAD_GeometricObject *)MW->AllObjects[NumBerOfObjects])->SecondRLIField , ((MAD_GeometricObject *)MW->AllObjects[NumBerOfObjects])->NumberOfPoints * sizeof(MAD_ColorARGB));
					memcpy(((MAD_GeometricObject *)MW->AllObjects[NumBerOfObjects])->SecondRLIField , ((MAD_GeometricObject *)MW->AllObjects[NumBerOfObjects])->Colors , ((MAD_GeometricObject *)MW->AllObjects[NumBerOfObjects])->NumberOfPoints * sizeof(MAD_ColorARGB));
					MW->Hierarchie[Counter].ObjectWithRadiosity = NumBerOfObjects;
					snprintf( TransformaX, sizeof(TransformaX), "%s_%s", ( char * ) MW->Hierarchie[ Counter ].ID.Name, "radied" );
					strcpy(MW->AllObjects[NumBerOfObjects] ->Name , TransformaX);
					MW->Hierarchie[Counter].NODE_Flags |= MNF_MustBeMadRaded;
					NumBerOfObjects++;
				}else
					((MAD_GeometricObject *) MW->AllObjects[MW->Hierarchie[Counter].ObjectWithRadiosity])->ObjectFlags &= ~GO_ShadowCut;
            }
        }
    }
	MAD_REALLOC(MAD_NodeID *, MW->AllObjects , NumBerOfObjects );
	MW->NumberOfObjects = NumBerOfObjects;
}
/*
===================================================================================================
===================================================================================================
*/
unsigned long MAD_GetNumberOfFace(MAD_GeometricObject *MO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter, Ret;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    Ret = 0;
    for(Counter = 0; Counter < MO->NumberOfElements; Counter++)
    {
        Ret += MO->Elements[Counter]->NumberOfTriangles;
    }

    return Ret;
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long MAD_GetFace
(
 MAD_GeometricObject *MO,
 unsigned long       FaceNum,
 MAD_Face            **Face,
 unsigned long       *ElementNumber,
 unsigned long       *MaterialID
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter, Ret;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Ret = 0;
    for(Counter = 0; Counter < MO->NumberOfElements; Counter++)
    {
        if((Ret + MO->Elements[Counter]->NumberOfTriangles) <= FaceNum)
            Ret += MO->Elements[Counter]->NumberOfTriangles;
        else
        {
            *MaterialID = MO->Elements[Counter]->MaterialID;
            *ElementNumber = Counter;
            *Face = &MO->Elements[Counter]->Faces[FaceNum - Ret];
            return 0;
        }
    }
	
    return 1;
}

/* FileCompare returrn TRE if Files are the sames F1 is the file included in the .MAD */
#define MAD_LOCAL_BUFFER_SIZE   256

/*
===================================================================================================
===================================================================================================
*/
int FileEqual(FILE *F1, FILE *F2, int size)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned char   Buffer1[MAD_LOCAL_BUFFER_SIZE];
    unsigned char   Buffer2[MAD_LOCAL_BUFFER_SIZE];
    long            Read2, Read1, Local;
    int             RET;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Read2 = Read1 = 0;
    RET = 0;
    while(size > MAD_LOCAL_BUFFER_SIZE)
    {
        Read1 += fread(Buffer1, 1, MAD_LOCAL_BUFFER_SIZE, F1);
        Read2 += fread(Buffer2, 1, MAD_LOCAL_BUFFER_SIZE, F2);
        if(Read2 != Read1) goto IKnow;
        if(memcmp(Buffer1, Buffer2, MAD_LOCAL_BUFFER_SIZE)) goto IKnow;
        size -= MAD_LOCAL_BUFFER_SIZE;
    }
	
    Read1 += fread(Buffer1, 1, size, F1);
    Local = fread(Buffer2, 1, size, F2);
    Read2 += Local;
    if(Local != size) goto IKnow;
    if(memcmp(Buffer1, Buffer2, size)) goto IKnow;
    RET = 1;
IKnow:
    fseek(F2, -Read2, SEEK_CUR);
    fseek(F1, -Read1, SEEK_CUR);
    return RET;
}

/*
===================================================================================================
FileCopy copy F1 -> F2
===================================================================================================
*/
void FileCopy(FILE *F1, FILE *F2, int size)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned char   Buffer[MAD_LOCAL_BUFFER_SIZE];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    while(size > MAD_LOCAL_BUFFER_SIZE)
    {
        fread(Buffer, 1, MAD_LOCAL_BUFFER_SIZE, F1);
        fwrite(Buffer, 1, MAD_LOCAL_BUFFER_SIZE, F2);
        size -= MAD_LOCAL_BUFFER_SIZE;
    }
	
    fread(Buffer, 1, size, F1);
    fwrite(Buffer, 1, size, F2);
}

/*
===================================================================================================
SeparateDirNameFromName extract the file name & directory name.
===================================================================================================
*/
void MAD_SeparateDirNameFromName(char *Filename, char *DirectoryName, char *TextureName)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   i, j;
    char            *src, SAVE;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    i = 0;
	
    /* Save the first character */
    SAVE = *Filename;
	
    /* Set the first character to \ */
    *Filename = 0x5c;   /* == '\' */
	
    /* Set pointer to start of the string */
    src = Filename;
	
    /* Till the end transform / in \ */
    while(*src)
    {
        i++;
        if(*src == '/') *src = 0x5c;
        src++;
    }
	
    /* Stop here if string null */
    if(!i) return;
	
    /* Set j to length of string */
    j = i;
	
    /* Go to the first letter of the name */
    while((j > 0) && (*src != 0x5c))
    {
        j--;
        src--;
    }
	
    if(j > 0)
    {
        j++;
        src++;
    }
	
    /* Here j is the length of the directory */
	
    /* Restore the first letter of the texture */
    *Filename = SAVE;
    i = i - j;
	
    /* Here i is the length of the name */
	
    /* Save the name */
    while(i--)
        *(TextureName++) = *(src++);
    *TextureName = 0;
	
    /* Save the directory */
    while(j--)
        *(DirectoryName++) = *(Filename++);
    *DirectoryName = 0;
    return;
}

/*
===================================================================================================
===================================================================================================
*/
void MAD_NodeEnum
(
 MAD_World * MW,
 unsigned long FILTER,
 void(*NodeCallback) (MAD_World * MW, MAD_WorldNode * MWN, unsigned long NodeNumber)
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    for(Counter = 0; Counter < MW->NumberOfHierarchieNodes; Counter++)
    {
        if(MW->Hierarchie[Counter].Object != MAD_NULL_INDEX)
        {
            if(MW->AllObjects[MW->Hierarchie[Counter].Object]->IDType != ID_MAD_UNDEFINED)
            {
                if(MW->AllObjects[MW->Hierarchie[Counter].Object]->IDType == FILTER)
                    NodeCallback(MW, &MW->Hierarchie[Counter], Counter);
            }
        }
    }
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long MAD_Unify(MAD_Face *F1, MAD_Face *F2, unsigned long N1, unsigned long N2)
{
    /* Test for Neightbour 1 */
#ifdef JADEFUSION
    if((F1->Index[0] == F2->Index[1]) && (F1->Index[1] == F2->Index[0]) && (F1->UVIndex[0] == F2->UVIndex[1]) && (F1->UVIndex[1] == F2->UVIndex[0]))
#else
	if((F1->Index[0] == F2->Index[1]) && (F1->Index[1] == F2->Index[0]))
#endif
	{
        F1->Undefined1 = N2;
        F2->Undefined1 = N1;
        return 1;
    }
#ifdef JADEFUSION	
    if((F1->Index[1] == F2->Index[1]) && (F1->Index[2] == F2->Index[0]) && (F1->UVIndex[1] == F2->UVIndex[1]) && (F1->UVIndex[2] == F2->UVIndex[0]))
#else
	if((F1->Index[1] == F2->Index[1]) && (F1->Index[2] == F2->Index[0]))
#endif
	{
        F1->Undefined2 = N2;
        F2->Undefined1 = N1;
        return 1;
    }
#ifdef JADEFUSION	
    if((F1->Index[2] == F2->Index[1]) && (F1->Index[0] == F2->Index[0]) && (F1->UVIndex[2] == F2->UVIndex[1]) && (F1->UVIndex[0] == F2->UVIndex[0]))
#else
	if((F1->Index[2] == F2->Index[1]) && (F1->Index[0] == F2->Index[0]))
#endif
	{
        F1->Undefined3 = N2;
        F2->Undefined1 = N1;
        return 1;
    }
#ifdef JADEFUSION	
    if((F1->Index[0] == F2->Index[2]) && (F1->Index[1] == F2->Index[1]) && (F1->UVIndex[0] == F2->UVIndex[2]) && (F1->UVIndex[1] == F2->UVIndex[1]))
#else
	if((F1->Index[0] == F2->Index[2]) && (F1->Index[1] == F2->Index[1]))
#endif
	{
        F1->Undefined1 = N2;
        F2->Undefined2 = N1;
        return 1;
    }
#ifdef JADEFUSION	
    if((F1->Index[1] == F2->Index[2]) && (F1->Index[2] == F2->Index[1]) && (F1->UVIndex[1] == F2->UVIndex[2]) && (F1->UVIndex[2] == F2->UVIndex[1]))
#else
	if((F1->Index[1] == F2->Index[2]) && (F1->Index[2] == F2->Index[1]))
#endif
	{
        F1->Undefined2 = N2;
        F2->Undefined2 = N1;
        return 1;
    }
#ifdef JADEFUSION	
    if((F1->Index[2] == F2->Index[2]) && (F1->Index[0] == F2->Index[1]) && (F1->UVIndex[2] == F2->UVIndex[2]) && (F1->UVIndex[0] == F2->UVIndex[1]))
#else
	if((F1->Index[2] == F2->Index[2]) && (F1->Index[0] == F2->Index[1]))
#endif
	{
        F1->Undefined3 = N2;
        F2->Undefined2 = N1;
        return 1;
    }
#ifdef JADEFUSION	
    if((F1->Index[0] == F2->Index[0]) && (F1->Index[1] == F2->Index[2]) && (F1->UVIndex[0] == F2->UVIndex[0]) && (F1->UVIndex[1] == F2->UVIndex[2]))
#else
	if((F1->Index[0] == F2->Index[0]) && (F1->Index[1] == F2->Index[2]))
#endif
	{
        F1->Undefined1 = N2;
        F2->Undefined3 = N1;
        return 1;
    }
#ifdef JADEFUSION	
    if((F1->Index[1] == F2->Index[0]) && (F1->Index[2] == F2->Index[2]) && (F1->UVIndex[1] == F2->UVIndex[0]) && (F1->UVIndex[2] == F2->UVIndex[2]))
#else
	if((F1->Index[1] == F2->Index[0]) && (F1->Index[2] == F2->Index[2]))
#endif
	{
        F1->Undefined2 = N2;
        F2->Undefined3 = N1;
        return 1;
    }
#ifdef JADEFUSION	
    if((F1->Index[2] == F2->Index[0]) && (F1->Index[0] == F2->Index[2]) && (F1->UVIndex[2] == F2->UVIndex[0]) && (F1->UVIndex[0] == F2->UVIndex[2]))
#else
	if((F1->Index[2] == F2->Index[0]) && (F1->Index[0] == F2->Index[2]))
#endif
	{
        F1->Undefined3 = N2;
        F2->Undefined3 = N1;
        return 1;
    }
	
    return 0;   /* Bug! */
}

#define CROSS_PRODUCT(a, b, c, _X, _Y, _Z) \
{ \
	c->_X = a->_Y * b->_Z - a->_Z * b->_Y; \
	c->_Y = a->_Z * b->_X - a->_X * b->_Z; \
	c->_Z = a->_X * b->_Y - a->_Y * b->_X; \
}

#define SUB_VECTOR(a, b, c, _X, _Y, _Z) \
{ \
	c->_X = (a->_X - b->_X); \
	c->_Y = (a->_Y - b->_Y); \
	c->_Z = (a->_Z - b->_Z); \
}
#define ADD_VECTOR(a, b, c, _X, _Y, _Z) \
{ \
	c->_X = (a->_X + b->_X); \
	c->_Y = (a->_Y + b->_Y); \
	c->_Z = (a->_Z + b->_Z); \
}

#define NORMALE_TRIANGLE(PA, PB, PC, NT, _X, _Y, _Z) \
{ \
	MAD_Simple3DVertex  V1, V2; \
	SUB_VECTOR(PA, PB, (&V1), _X, _Y, _Z); \
	SUB_VECTOR(PA, PC, (&V2), _X, _Y, _Z); \
	CROSS_PRODUCT((&V2), (&V1), NT, _X, _Y, _Z); \
}
#define NORMALIZE(a, _X, _Y, _Z) \
{ \
	float   lenght; \
	lenght = a->_X * a->_X + a->_Y * a->_Y + a->_Z * a->_Z; \
	if(lenght != 0.0f) \
	lenght = 1.0f / (float) sqrt(lenght); \
	a->_X = a->_X * lenght; \
	a->_Y = a->_Y * lenght; \
	a->_Z = a->_Z * lenght; \
}
#define NORME(a, N, _X, _Y, _Z) \
    { \
	N = a->_X * a->_X + a->_Y * a->_Y + a->_Z * a->_Z; \
	if(N != 0.0f) \
	N = 1.0f / (float) sqrt(N); \
}

#define DOT_PRODUCT(a, b, _X, _Y, _Z)   (a->_Y * b->_Y) + (a->_Z * b->_Z) + (a->_X * b->_X)

/*
===================================================================================================
===================================================================================================
*/
void MAD_ComputeNormales(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long       Counter, NumberOfFace, MID, Nothing,i;
    MAD_Face            *CurrentFace;
    MAD_Simple3DVertex  Normale;
    MAD_GeometricObject *MO;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	for (i=0;i<2;i++)
	{
		MO = (MAD_GeometricObject *) MW->AllObjects[MWN->Object];
		if ( (i==1) && (MWN->ObjectWithRadiosity != MAD_NULL_INDEX))
			MO = (MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity];
		for ( Counter = 0 ; Counter < MO->NumberOfPoints ; Counter ++ )
		{
			(MO->OBJ_PointList + Counter)->Normale.x = 0.0f;
			(MO->OBJ_PointList + Counter)->Normale.y = 0.0f;
			(MO->OBJ_PointList + Counter)->Normale.z = 0.0f;
		}

		NumberOfFace = MAD_GetNumberOfFace(MO);
		for(Counter = 0; Counter < NumberOfFace; Counter++)
		{
			MAD_GetFace(MO, Counter, &CurrentFace, &MID, &Nothing);
			NORMALE_TRIANGLE
				(
				(MO->OBJ_PointList + CurrentFace->Index[0]),
				(MO->OBJ_PointList + CurrentFace->Index[1]),
				(MO->OBJ_PointList + CurrentFace->Index[2]),
				(&Normale),
				Point.x,
				Point.y,
				Point.z
				);
			Normale.Normale = Normale.Point;
			NORMALIZE((&Normale), Normale.x, Normale.y, Normale.z);
			ADD_VECTOR
				(
				(MO->OBJ_PointList + CurrentFace->Index[0]),
				(&Normale),
				(MO->OBJ_PointList + CurrentFace->Index[0]),
				Normale.x,
				Normale.y,
				Normale.z
				);
			ADD_VECTOR
				(
				(MO->OBJ_PointList + CurrentFace->Index[1]),
				(&Normale),
				(MO->OBJ_PointList + CurrentFace->Index[1]),
				Normale.x,
				Normale.y,
				Normale.z
				);
			ADD_VECTOR
				(
				(MO->OBJ_PointList + CurrentFace->Index[2]),
				(&Normale),
				(MO->OBJ_PointList + CurrentFace->Index[2]),
				Normale.x,
				Normale.y,
				Normale.z
				);
		}
		
		for(Counter = 0; Counter < MO->NumberOfPoints; Counter++)
		{
			NORMALIZE((MO->OBJ_PointList + Counter), Normale.x, Normale.y, Normale.z);
		}
	}
}

/*
===================================================================================================
===================================================================================================
*/
void MAD_ComputeNeightbouringOnObject(MAD_GeometricObject *MO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   FaceCounter, FaceCounter2, FaceCounter3, NumberOfFace, MID, Local, Nothing;
    MAD_Face        *CurrentFace, *CurrentFace2;
    unsigned long   *Vertex2Triangles;
    unsigned long   *Vertex2TrianglesIndex;
    unsigned long   *TrianglesIndex;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    MAD_RemoveIllgalFaces(MO);
    Vertex2Triangles = (unsigned long *) malloc(4L * MO->NumberOfPoints);
    Vertex2TrianglesIndex = (unsigned long *) malloc(4L * MO->NumberOfPoints);
    memset(Vertex2Triangles, 0, 4L * MO->NumberOfPoints);
    memset(Vertex2Triangles, 0, 4L * MO->NumberOfPoints);
    NumberOfFace = MAD_GetNumberOfFace(MO);
	
    /* Count number of triangles per vertex */
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        CurrentFace->Undefined1 = MAD_NULL_INDEX;
        CurrentFace->Undefined2 = MAD_NULL_INDEX;
        CurrentFace->Undefined3 = MAD_NULL_INDEX;
        Vertex2Triangles[CurrentFace->Index[0]]++;
        Vertex2Triangles[CurrentFace->Index[1]]++;
        Vertex2Triangles[CurrentFace->Index[2]]++;
    }
	
    /* Allocate liste of pervertex -> triangles */
    Local = 0;
    for(FaceCounter = 0; FaceCounter < MO->NumberOfPoints; FaceCounter++)
    {
        Vertex2TrianglesIndex[FaceCounter] = Local;
        Local += Vertex2Triangles[FaceCounter];
        Vertex2Triangles[FaceCounter] = 0;
    }
	
    TrianglesIndex = (unsigned long *) malloc(4L * Local);
    memset(TrianglesIndex, 0, 4L * Local);
	
    /* Initialize liste of pervertex -> triangles */
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        Local = Vertex2TrianglesIndex[CurrentFace->Index[0]];
        Local += Vertex2Triangles[CurrentFace->Index[0]];
        Vertex2Triangles[CurrentFace->Index[0]]++;
        TrianglesIndex[Local] = FaceCounter;
        Local = Vertex2TrianglesIndex[CurrentFace->Index[1]];
        Local += Vertex2Triangles[CurrentFace->Index[1]];
        Vertex2Triangles[CurrentFace->Index[1]]++;
        TrianglesIndex[Local] = FaceCounter;
        Local = Vertex2TrianglesIndex[CurrentFace->Index[2]];
        Local += Vertex2Triangles[CurrentFace->Index[2]];
        Vertex2Triangles[CurrentFace->Index[2]]++;
        TrianglesIndex[Local] = FaceCounter;
    }
	
    for(FaceCounter = 0; FaceCounter < MO->NumberOfPoints; FaceCounter++)
    {
        if(Vertex2Triangles[FaceCounter])
        {
            for(FaceCounter2 = 0; FaceCounter2 < Vertex2Triangles[FaceCounter] - 1; FaceCounter2++)
            {
                MAD_GetFace
					(
                    MO,
                    TrianglesIndex[Vertex2TrianglesIndex[FaceCounter] + FaceCounter2],
                    &CurrentFace,
                    &MID,
                    &Nothing
					);
                for
					(
                    FaceCounter3 = FaceCounter2 + 1;
				FaceCounter3 < Vertex2Triangles[FaceCounter]; FaceCounter3++
					)
                {
                    MAD_GetFace
						(
                        MO,
                        TrianglesIndex[Vertex2TrianglesIndex[FaceCounter] + FaceCounter3],
                        &CurrentFace2,
                        &MID,
                        &Nothing
						);
                    MAD_Unify
						(
                        CurrentFace,
                        CurrentFace2,
                        TrianglesIndex[Vertex2TrianglesIndex[FaceCounter] + FaceCounter2],
                        TrianglesIndex[Vertex2TrianglesIndex[FaceCounter] + FaceCounter3]
						);
                }
            }
        }
    }
	
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        if
			(
            (CurrentFace->Undefined1 == MAD_NULL_INDEX) ||
            (CurrentFace->Undefined2 == MAD_NULL_INDEX) ||
            (CurrentFace->Undefined3 == MAD_NULL_INDEX)
			)
            CurrentFace = CurrentFace;
    }
	
    free(Vertex2TrianglesIndex);
    free(Vertex2Triangles);
    free(TrianglesIndex);
}

/*
===================================================================================================
===================================================================================================
*/
void MAD_ComputeNeightbouringCLBCK(MAD_World *MW, MAD_WorldNode *MWN, unsigned long NodeNumber)
{
    MAD_ComputeNeightbouringOnObject((MAD_GeometricObject *) MW->AllObjects[MWN->Object]);
    MAD_ComputeNeightbouringOnObject((MAD_GeometricObject *) MW->AllObjects[MWN->ObjectWithRadiosity]);
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long MAD_ISCmptbl_Num
(
 MAD_GeometricObject *MO,
 MAD_Face            *CurrentFace,
 MAD_Face            *CurrentFace2
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Simple3DVertex  Normale1, Normale2;
    float               Resultx;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    NORMALE_TRIANGLE
		(
        (MO->OBJ_PointList + CurrentFace->Index[0]),
        (MO->OBJ_PointList + CurrentFace->Index[1]),
        (MO->OBJ_PointList + CurrentFace->Index[2]),
        (&Normale1),
        Point.x,
        Point.y,
        Point.z
		);
    NORMALE_TRIANGLE
		(
        (MO->OBJ_PointList + CurrentFace2->Index[0]),
        (MO->OBJ_PointList + CurrentFace2->Index[1]),
        (MO->OBJ_PointList + CurrentFace2->Index[2]),
        (&Normale2),
        Point.x,
        Point.y,
        Point.z
		);
    NORMALIZE((&Normale1), Point.x, Point.y, Point.z);
    NORMALIZE((&Normale2), Point.x, Point.y, Point.z);
    Resultx = DOT_PRODUCT((&Normale1), (&Normale2), Point.x, Point.y, Point.z);
    if(Resultx > 0.99f) return 1;
	
    return 0;
}

unsigned long   Base = 0;

/*
===================================================================================================
===================================================================================================
*/
void MAD_Numerotize(MAD_World *MW, MAD_GeometricObject *MO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   NumberOfFace, MID, FaceCounter, Nothing, Counter;
    MAD_Face        *CurrentFace, *CurrentFace2;
    unsigned long   FirstBase, Local;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    MAD_ComputeNeightbouringOnObject(MO);
    FirstBase = Base;
	
    /* Be sure to have only the first bit of undefined0 set */
    NumberOfFace = MAD_GetNumberOfFace(MO);
	
    /* First find new number of points */
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        CurrentFace->Undefined0 = Base;
        Base++;
    }
	
    /* First find new number of points */
    for(Counter = 0; Counter < 50; Counter++)
    {
        for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
        {
            MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
            Nothing = 0;
            if((CurrentFace->Undefined1 != MAD_NULL_INDEX))
            {
                MAD_GetFace(MO, CurrentFace->Undefined1, &CurrentFace2, &MID, &Nothing);
                if(CurrentFace->Undefined0 > CurrentFace2->Undefined0)
                {
                    Local = (CurrentFace2->Undefined0) - (FirstBase);
                    MAD_GetFace(MO, Local, &CurrentFace2, &MID, &Nothing);
                    if(MAD_ISCmptbl_Num(MO, CurrentFace, CurrentFace2))
                        CurrentFace->Undefined0 = CurrentFace2->Undefined0;
                }
            }
			
            if((CurrentFace->Undefined2 != MAD_NULL_INDEX))
            {
                MAD_GetFace(MO, CurrentFace->Undefined2, &CurrentFace2, &MID, &Nothing);
                if(CurrentFace->Undefined0 > CurrentFace2->Undefined0)
                {
                    Local = (CurrentFace2->Undefined0) - (FirstBase);
                    MAD_GetFace(MO, Local, &CurrentFace2, &MID, &Nothing);
                    if(MAD_ISCmptbl_Num(MO, CurrentFace, CurrentFace2))
                        CurrentFace->Undefined0 = CurrentFace2->Undefined0;
                }
            }
			
            if((CurrentFace->Undefined3 != MAD_NULL_INDEX))
            {
                MAD_GetFace(MO, CurrentFace->Undefined3, &CurrentFace2, &MID, &Nothing);
                if(CurrentFace->Undefined0 > CurrentFace2->Undefined0)
                {
                    Local = (CurrentFace2->Undefined0) - (FirstBase);
                    MAD_GetFace(MO, Local, &CurrentFace2, &MID, &Nothing);
                    if(MAD_ISCmptbl_Num(MO, CurrentFace, CurrentFace2))
                        CurrentFace->Undefined0 = CurrentFace2->Undefined0;
                }
            }
        }
    }
	
    /* First find new number of points */
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        CurrentFace->Undefined0 <<= 8;
    }
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long MAD_CreateMiddlePoint
(
 MAD_World           *MW,
 MAD_WorldNode		 *MWN , 
 MAD_GeometricObject *MO,
 unsigned long       I1,
 unsigned long       I2
 )
{
    (MO->OBJ_PointList + MO->NumberOfPoints)->Point.x =
        ((MO->OBJ_PointList + I1)->Point.x + (MO->OBJ_PointList + I2)->Point.x) * 0.5f;
    (MO->OBJ_PointList + MO->NumberOfPoints)->Point.y =
        ((MO->OBJ_PointList + I1)->Point.y + (MO->OBJ_PointList + I2)->Point.y) * 0.5f;
    (MO->OBJ_PointList + MO->NumberOfPoints)->Point.z =
        ((MO->OBJ_PointList + I1)->Point.z + (MO->OBJ_PointList + I2)->Point.z) * 0.5f;
	
    *(MO->Colors + MO->NumberOfPoints) = ((*(MO->Colors + I1) & 0xfefefefe) >> 1) + ((*(MO->Colors + I2) & 0xfefefefe) >> 1);
    *(MO->SecondRLIField + MO->NumberOfPoints) = ((*(MO->SecondRLIField + I1) & 0xfefefefe) >> 1) + ((*(MO->SecondRLIField + I2) & 0xfefefefe) >> 1);
	if (MWN ->UsersRLIOfObject)	
		*(MWN ->UsersRLIOfObject + MO->NumberOfPoints) = ((*(MWN ->UsersRLIOfObject + I1) & 0xfefefefe) >> 1) + ((*(MWN ->UsersRLIOfObject + I2) & 0xfefefefe) >> 1);
    /* Interpole normale */
    (MO->OBJ_PointList + MO->NumberOfPoints)->Normale.x =
        ((MO->OBJ_PointList + I1)->Normale.x + (MO->OBJ_PointList + I2)->Normale.x) * 0.5f;
    (MO->OBJ_PointList + MO->NumberOfPoints)->Normale.y =
        ((MO->OBJ_PointList + I1)->Normale.y + (MO->OBJ_PointList + I2)->Normale.y) * 0.5f;
    (MO->OBJ_PointList + MO->NumberOfPoints)->Normale.z =
        ((MO->OBJ_PointList + I1)->Normale.z + (MO->OBJ_PointList + I2)->Normale.z) * 0.5f;
    NORMALIZE((MO->OBJ_PointList + MO->NumberOfPoints), Normale.x, Normale.y, Normale.z);
	
    return MO->NumberOfPoints++;
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long MAD_CreateMiddlePointUV
(
 MAD_World           *MW,
 MAD_GeometricObject *MO,
 unsigned long       I1,
 unsigned long       I2,
 MAD_Face            *AdjascentFace,
 MAD_Face            *NewAdjacentFace,
 unsigned long       ThisFace
 )
{
    /* First detect if UV are compatibles */
    if(AdjascentFace)
    {
        if(AdjascentFace->Undefined1 == ThisFace)
        {
            if((AdjascentFace->UVIndex[0] == I2) && (AdjascentFace->UVIndex[1] == I1))
                return NewAdjacentFace->UVIndex[0];
        }
		
        if(AdjascentFace->Undefined2 == ThisFace)
        {
            if((AdjascentFace->UVIndex[1] == I2) && (AdjascentFace->UVIndex[2] == I1))
                return NewAdjacentFace->UVIndex[1];
        }
		
        if(AdjascentFace->Undefined3 == ThisFace)
        {
            if((AdjascentFace->UVIndex[2] == I2) && (AdjascentFace->UVIndex[0] == I1))
                return NewAdjacentFace->UVIndex[2];
        }
    }
    (MO->OBJ_UVtextureList + MO->NumberOfUV)->x =
        ((MO->OBJ_UVtextureList + I1)->x + (MO->OBJ_UVtextureList + I2)->x) * 0.5f;
    (MO->OBJ_UVtextureList + MO->NumberOfUV)->y =
        ((MO->OBJ_UVtextureList + I1)->y + (MO->OBJ_UVtextureList + I2)->y) * 0.5f;
    (MO->OBJ_UVtextureList + MO->NumberOfUV)->z =
        ((MO->OBJ_UVtextureList + I1)->z + (MO->OBJ_UVtextureList + I2)->z) * 0.5f;
    return MO->NumberOfUV++;
}

/*
===================================================================================================
===================================================================================================
*/
void TagSubdivisionSymetrie(unsigned long Face1, MAD_Face *CurrentFace2)
{
    if(CurrentFace2->Undefined1 == Face1)
        CurrentFace2->Undefined0 |= 8;
    if(CurrentFace2->Undefined2 == Face1)
        CurrentFace2->Undefined0 |= 4;
    if(CurrentFace2->Undefined3 == Face1)
        CurrentFace2->Undefined0 |= 2;
}

/* This will turn each edge Fully linear but optimisable. */
#define TooSmallAngle   0.03f
#define Equality        0.98f

/*
===================================================================================================
===================================================================================================
*/
unsigned long NoCollinear
(
 MAD_Simple3DVertex  *E0,
 MAD_Simple3DVertex  *E1,
 MAD_Simple3DVertex  *V0,
 MAD_Simple3DVertex  *V1
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Simple3DVertex  N1, N2;
    MAD_Simple3DVertex  C1, C2;
    float               NA, NE;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    SUB_VECTOR(V0, E0, (&C1), Point.x, Point.y, Point.z);
    SUB_VECTOR(E1, E0, (&C2), Point.x, Point.y, Point.z);
    CROSS_PRODUCT((&C1), (&C2), (&N1), Point.x, Point.y, Point.z);
    SUB_VECTOR(V0, E0, (&C1), Point.x, Point.y, Point.z);
    SUB_VECTOR(V1, E0, (&C2), Point.x, Point.y, Point.z);
    CROSS_PRODUCT((&C1), (&C2), (&N2), Point.x, Point.y, Point.z);
    if(DOT_PRODUCT((&N1), (&N2), Point.x, Point.y, Point.z) <= 0.f) return 0;
    SUB_VECTOR(E0, E1, (&C1), Point.x, Point.y, Point.z);
    SUB_VECTOR(V1, E1, (&C2), Point.x, Point.y, Point.z);
    CROSS_PRODUCT((&C1), (&C2), (&N1), Point.x, Point.y, Point.z);
    SUB_VECTOR(V0, E1, (&C1), Point.x, Point.y, Point.z);
    SUB_VECTOR(V1, E1, (&C2), Point.x, Point.y, Point.z);
    CROSS_PRODUCT((&C1), (&C2), (&N2), Point.x, Point.y, Point.z);
    if(DOT_PRODUCT((&N1), (&N2), Point.x, Point.y, Point.z) <= 0.f) return 0;
	
    SUB_VECTOR(E0, E1, (&C1), Point.x, Point.y, Point.z);
    NORME((&C1), NE, Point.x, Point.y, Point.z);
    SUB_VECTOR(V0, V1, (&C1), Point.x, Point.y, Point.z);
    NORME((&C1), NA, Point.x, Point.y, Point.z);
    if((NA / NE) > 1.1f) return 0;
    if((NE / NA) > 1.1f) return 0;
	
    /*
	* SUB_VECTOR(E0, E1, (&C1), Point.x, Point.y, Point.z); NORME((&C1), NE, Point.x, Point.y,
	* Point.z); SUB_VECTOR(V0, E0, (&C1), Point.x, Point.y, Point.z); NORME((&C1), NA, Point.x,
	* Point.y, Point.z); if (NA <= NE) return 0; SUB_VECTOR(V1, E0, (&C1), Point.x, Point.y,
	* Point.z); NORME((&C1), NA, Point.x, Point.y, Point.z); if (NA <= NE) return 0;
	* SUB_VECTOR(V1, E1, (&C1), Point.x, Point.y, Point.z); NORME((&C1), NA, Point.x, Point.y,
	* Point.z); if (NA <= NE) return 0; SUB_VECTOR(V0, E1, (&C1), Point.x, Point.y, Point.z);
	* NORME((&C1), NA, Point.x, Point.y, Point.z); if (NA <= NE) return 0;
	*/
	
    /* /* */
    SUB_VECTOR(V0, E0, (&C1), Point.x, Point.y, Point.z);
    SUB_VECTOR(E0, V1, (&C2), Point.x, Point.y, Point.z);
    NORMALIZE((&C1), Point.x, Point.y, Point.z);
    NORMALIZE((&C2), Point.x, Point.y, Point.z);
    if(DOT_PRODUCT((&C1), (&C2), Point.x, Point.y, Point.z) > 0.9f) return 0;
    SUB_VECTOR(V0, E1, (&C1), Point.x, Point.y, Point.z);
    SUB_VECTOR(E1, V1, (&C2), Point.x, Point.y, Point.z);
    NORMALIZE((&C1), Point.x, Point.y, Point.z);
    NORMALIZE((&C2), Point.x, Point.y, Point.z);
    if(DOT_PRODUCT((&C1), (&C2), Point.x, Point.y, Point.z) > 0.9f) return 0;
    return 1;
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long NoCollinear2
(
 MAD_Simple3DVertex  *E0,
 MAD_Simple3DVertex  *E1,
 MAD_Simple3DVertex  *V0,
 MAD_Simple3DVertex  *V1
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Simple3DVertex  Vedge;
    MAD_Simple3DVertex  Edge2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    SUB_VECTOR(E1, E0, (&Vedge), Point.x, Point.y, Point.z);
    NORMALIZE((&Vedge), Point.x, Point.y, Point.z);
    SUB_VECTOR(V0, E0, (&Edge2), Point.x, Point.y, Point.z);
    NORMALIZE((&Edge2), Point.x, Point.y, Point.z);
    if(DOT_PRODUCT((&Vedge), (&Edge2), Point.x, Point.y, Point.z) < TooSmallAngle) return 0;
    SUB_VECTOR(V0, E1, (&Edge2), Point.x, Point.y, Point.z);
    NORMALIZE((&Edge2), Point.x, Point.y, Point.z);
    if(DOT_PRODUCT((&Vedge), (&Edge2), Point.x, Point.y, Point.z) > -TooSmallAngle) return 0;
    SUB_VECTOR(V1, E0, (&Edge2), Point.x, Point.y, Point.z);
    NORMALIZE((&Edge2), Point.x, Point.y, Point.z);
    if(DOT_PRODUCT((&Vedge), (&Edge2), Point.x, Point.y, Point.z) < TooSmallAngle) return 0;
    SUB_VECTOR(V1, E1, (&Edge2), Point.x, Point.y, Point.z);
    NORMALIZE((&Edge2), Point.x, Point.y, Point.z);
    if(DOT_PRODUCT((&Vedge), (&Edge2), Point.x, Point.y, Point.z) > -TooSmallAngle) return 0;
    return 1;
}

/*
===================================================================================================
PARRALLELOGFREAM TESt
===================================================================================================
*/
unsigned long NoCollinear3
(
 MAD_Simple3DVertex  *E0,
 MAD_Simple3DVertex  *E1,
 MAD_Simple3DVertex  *V0,
 MAD_Simple3DVertex  *V1
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    /* MAD_Simple3DVertex N1,N2; */
    MAD_Simple3DVertex  C1, C2;
    float/* NB, */      NA, NE;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    SUB_VECTOR(E0, V1, (&C1), Point.x, Point.y, Point.z);
    SUB_VECTOR(E1, V0, (&C2), Point.x, Point.y, Point.z);
    NORME((&C1), NE, Point.x, Point.y, Point.z);
    NORME((&C2), NA, Point.x, Point.y, Point.z);
    NORMALIZE((&C1), Point.x, Point.y, Point.z);
    NORMALIZE((&C2), Point.x, Point.y, Point.z);
    if(DOT_PRODUCT((&C1), (&C2), Point.x, Point.y, Point.z) < 0.95f) return 0;
    if((NA / NE) > 1.2f) return 0;
    if((NE / NA) > 1.2f) return 0;
    return 1;
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long IsEqualColor(MAD_Vertex *C1, MAD_Vertex *C2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   Dist;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Dist = (C1->x - C2->x) * (C1->x - C2->x) + (C1->y - C2->y) * (C1->y - C2->y) + (C1->z - C2->z) *
        (C1->z - C2->z);
    if(Dist < 1000.0f) return 1;
    return 0;
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long IsDifferentColor(MAD_Vertex *C1, MAD_Vertex *C2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   Dist;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Dist = (C1->x - C2->x) * (C1->x - C2->x) + (C1->y - C2->y) * (C1->y - C2->y) + (C1->z - C2->z) *
        (C1->z - C2->z);
    if(Dist > 4000.0f) return 1;
    return 0;
}

/*
===================================================================================================
===================================================================================================
*/
float GetDistance(MAD_Vertex *C1, MAD_Vertex *C2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   Dist;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    Dist = (C1->x - C2->x) * (C1->x - C2->x) + (C1->y - C2->y) * (C1->y - C2->y) + (C1->z - C2->z) *
        (C1->z - C2->z);
    return Dist;
}
/*
===================================================================================================
===================================================================================================
*/
float GetDistanceARGB(MAD_ColorARGB C1, MAD_ColorARGB C2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Vertex V1,V2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAD_ColorARGB_to_MAD_Vertex(V1,C1);
	MAD_ColorARGB_to_MAD_Vertex(V2,C2);
    return GetDistance(&V1,&V2);
}

/*
===================================================================================================
===================================================================================================
*/
void TurnEdgeCompatibles
(
 MAD_GeometricObject *MO,
 MAD_Face            *CF1,
 MAD_Face            *CF2,
 unsigned long       Face1,
 unsigned long       Face2
 )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   IndexA[3];
    unsigned long   IndexB[3];
    unsigned long   UVIndexA[3];
    unsigned long   UVIndexB[3];
    unsigned long   VA[3];
    unsigned long   VB[3];
    unsigned long   Nothing, ElementNumber;
    unsigned long   FlagA, FlagB;
    MAD_Face        *Restore;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	FlagA = 0;
	FlagB = 0;
	
    if((CF1->Undefined0 | CF2->Undefined0) & 0x00000010) return;
	
    /* Compute A0 A1 A2 */
    if(Face2 == CF1->Undefined1)
    {
        IndexA[0] = CF1->Index[0];
        IndexA[1] = CF1->Index[1];
        IndexA[2] = CF1->Index[2];
		if (CF1->MAXflags & 1) FlagA |= 1;
		if (CF1->MAXflags & 2) FlagA |= 2;
		if (CF1->MAXflags & 4) FlagA |= 4;
        VA[0] = CF1->Undefined1;
        VA[1] = CF1->Undefined2;
        VA[2] = CF1->Undefined3;
        UVIndexA[0] = CF1->UVIndex[0];
        UVIndexA[1] = CF1->UVIndex[1];
        UVIndexA[2] = CF1->UVIndex[2];
    }
    else if(Face2 == CF1->Undefined2)
    {
        IndexA[0] = CF1->Index[1];
        IndexA[1] = CF1->Index[2];
        IndexA[2] = CF1->Index[0];
		if (CF1->MAXflags & 1) FlagA |= 4; /* 0 1 */
		if (CF1->MAXflags & 2) FlagA |= 1; /* 1 2 */
		if (CF1->MAXflags & 4) FlagA |= 2; /* 2 0 */
        VA[0] = CF1->Undefined2;
        VA[1] = CF1->Undefined3;
        VA[2] = CF1->Undefined1;
        UVIndexA[0] = CF1->UVIndex[1];
        UVIndexA[1] = CF1->UVIndex[2];
        UVIndexA[2] = CF1->UVIndex[0];
    }
    else if(Face2 == CF1->Undefined3)
    {
        IndexA[0] = CF1->Index[2];
        IndexA[1] = CF1->Index[0];
        IndexA[2] = CF1->Index[1];
		if (CF1->MAXflags & 1) FlagA |= 2; /* 0 1 */
		if (CF1->MAXflags & 2) FlagA |= 4; /* 1 2 */
		if (CF1->MAXflags & 4) FlagA |= 1; /* 2 0 */
        VA[0] = CF1->Undefined3;
        VA[1] = CF1->Undefined1;
        VA[2] = CF1->Undefined2;
        UVIndexA[0] = CF1->UVIndex[2];
        UVIndexA[1] = CF1->UVIndex[0];
        UVIndexA[2] = CF1->UVIndex[1];
    }
    else
        return; /* BUG!! */
	
    /* Compute B0 B1 B2 */
    if(Face1 == (CF2->Undefined1 & 0x7fffffff))
    {
        IndexB[0] = CF2->Index[1];
        IndexB[1] = CF2->Index[2];
        IndexB[2] = CF2->Index[0];
		if (CF2->MAXflags & 1) FlagB |= 4; /* 0 1 */
		if (CF2->MAXflags & 2) FlagB |= 1; /* 1 2 */
		if (CF2->MAXflags & 4) FlagB |= 2; /* 2 0 */
        VB[0] = CF2->Undefined2;
        VB[1] = CF2->Undefined3;
        VB[2] = CF2->Undefined1;
        UVIndexB[0] = CF2->UVIndex[1];
        UVIndexB[1] = CF2->UVIndex[2];
        UVIndexB[2] = CF2->UVIndex[0];
    }
    else if(Face1 == (CF2->Undefined2 & 0x7fffffff))
    {
        IndexB[0] = CF2->Index[2];
        IndexB[1] = CF2->Index[0];
        IndexB[2] = CF2->Index[1];
		if (CF2->MAXflags & 1) FlagB |= 2; /* 0 1 */
		if (CF2->MAXflags & 2) FlagB |= 4; /* 1 2 */
		if (CF2->MAXflags & 4) FlagB |= 1; /* 2 0 */
        VB[0] = CF2->Undefined3;
        VB[1] = CF2->Undefined1;
        VB[2] = CF2->Undefined2;
        UVIndexB[0] = CF2->UVIndex[2];
        UVIndexB[1] = CF2->UVIndex[0];
        UVIndexB[2] = CF2->UVIndex[1];
    }
    else if(Face1 == (CF2->Undefined3 & 0x7fffffff))
    {
        IndexB[0] = CF2->Index[0];
        IndexB[1] = CF2->Index[1];
        IndexB[2] = CF2->Index[2];
		if (CF2->MAXflags & 1) FlagB |= 1; /* 0 1 */
		if (CF2->MAXflags & 2) FlagB |= 2; /* 1 2 */
		if (CF2->MAXflags & 4) FlagB |= 4; /* 2 0 */
        VB[0] = CF2->Undefined1;
        VB[1] = CF2->Undefined2;
        VB[2] = CF2->Undefined3;
        UVIndexB[0] = CF2->UVIndex[0];
        UVIndexB[1] = CF2->UVIndex[1];
        UVIndexB[2] = CF2->UVIndex[2];
    }
    else
        return; /* BUG!! */
	
    /* First detect If we must turn this edge */
    if
        (
		(GetDistanceARGB(MO->Colors [ IndexA[0]], MO->Colors [ IndexA[1]])) >
		(GetDistanceARGB(MO->Colors [ IndexA[2]], MO->Colors [ IndexB[1]]))
        )
    {
        /* No topologic Errors? */
        if
			(
            NoCollinear
            (
			MO->OBJ_PointList + (IndexA[0] & 0x7fffffff),
			MO->OBJ_PointList + (IndexA[1] & 0x7fffffff),
			MO->OBJ_PointList + (IndexA[2] & 0x7fffffff),
			MO->OBJ_PointList + (IndexB[1] & 0x7fffffff)
            )
			)
        {
            CF1->Undefined0 |= 0x00000010;
            CF2->Undefined0 |= 0x00000010;
			
            CF1->Index[0] = IndexA[0];
            CF1->Index[1] = IndexB[1];
            CF1->Index[2] = IndexA[2];
            CF1->Undefined1 = VB[0];
            CF1->Undefined2 = Face2;
            CF1->Undefined3 = VA[2];
            CF1->UVIndex[0] = UVIndexA[0];
            CF1->UVIndex[1] = UVIndexB[1];
            CF1->UVIndex[2] = UVIndexA[2];
			CF1->MAXflags &= ~7L;
			if (FlagB & 1) CF1->MAXflags |= 1; /* 0 1 */
			if (FlagA & 4) CF1->MAXflags |= 4; /* 0 1 */

            CF2->Index[0] = IndexA[2];
            CF2->Index[1] = IndexB[1];
            CF2->Index[2] = IndexA[1];
            CF2->Undefined1 = Face1;
            CF2->Undefined2 = VB[1];
            CF2->Undefined3 = VA[1];
            CF2->UVIndex[0] = UVIndexA[2];
            CF2->UVIndex[1] = UVIndexB[1];
            CF2->UVIndex[2] = UVIndexA[1];
			CF2->MAXflags &= ~7L;
			if (FlagB & 2) CF2->MAXflags |= 2; /* 0 1 */
			if (FlagA & 2) CF2->MAXflags |= 4; /* 0 1 */
			
            /* Restore VB0 & VA1 */
            if(VB[0] != MAD_NULL_INDEX)
            {
                MAD_GetFace(MO, VB[0] & 0x7fffffff, &Restore, &ElementNumber, &Nothing);
                if((Restore->Undefined1 & 0x7fffffff) == Face2)
                {
                    Restore->Undefined1 = Face1 | (Restore->Undefined1 & 0x80000000);
                }
                if((Restore->Undefined2 & 0x7fffffff) == Face2)
                {
                    Restore->Undefined2 = Face1 | (Restore->Undefined2 & 0x80000000);
                }
                if((Restore->Undefined3 & 0x7fffffff) == Face2)
                {
                    Restore->Undefined3 = Face1 | (Restore->Undefined3 & 0x80000000);
                }
            }
			
            if(VA[1] != MAD_NULL_INDEX)
            {
                MAD_GetFace(MO, VA[1] & 0x7fffffff, &Restore, &ElementNumber, &Nothing);
                if((Restore->Undefined1 & 0x7fffffff) == Face1)
                {
                    Restore->Undefined1 = Face2 | (Restore->Undefined1 & 0x80000000);
                }
				
                if((Restore->Undefined2 & 0x7fffffff) == Face1)
                {
                    Restore->Undefined2 = Face2 | (Restore->Undefined2 & 0x80000000);
                }
				
                if((Restore->Undefined3 & 0x7fffffff) == Face1)
                {
                    Restore->Undefined3 = Face2 | (Restore->Undefined3 & 0x80000000);
                }
            }
        }
    }
}

/*
===================================================================================================
===================================================================================================
*/
void MAD_TurnEdges(MAD_World *MW, MAD_GeometricObject *MO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   NumberOfFace, MID, FaceCounter, Nothing;
    unsigned long   ElementNumber, XS;
    MAD_Face        *CurrentFace, *CurrentFace2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//	return;
	
    NumberOfFace = MAD_GetNumberOfFace(MO);
	
    for(XS = 0; XS < 2; XS++)
    {
        /* First find new number of points */
        for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
        {
            MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
            CurrentFace->Undefined0 &= ~0x00000010;
			
            /* CurrentFace->Undefined0 |= 0x00000010; */
            if(CurrentFace->Undefined1 != MAD_NULL_INDEX)
            {
                MAD_GetFace(MO, CurrentFace->Undefined1, &CurrentFace2, &ElementNumber, &Nothing);
                if
					(
                    ((CurrentFace2->Undefined0 >> 8) != (CurrentFace->Undefined0 >> 8)) ||
                    (CurrentFace->Undefined1 > FaceCounter) ||
                    (ElementNumber != MID) ||
					(CurrentFace->MAXflags & 1L)
					)
                    CurrentFace->Undefined1 |= 0x80000000;
            }
			
            if(CurrentFace->Undefined2 != MAD_NULL_INDEX)
            {
                MAD_GetFace(MO, CurrentFace->Undefined2, &CurrentFace2, &ElementNumber, &Nothing);
                if
					(
                    ((CurrentFace2->Undefined0 >> 8) != (CurrentFace->Undefined0 >> 8)) ||
                    (CurrentFace->Undefined2 > FaceCounter) ||
                    (ElementNumber != MID) ||
					(CurrentFace->MAXflags & 2L)
					)
                    CurrentFace->Undefined2 |= 0x80000000;
            }
			
            if(CurrentFace->Undefined3 != MAD_NULL_INDEX)
            {
                MAD_GetFace(MO, CurrentFace->Undefined3, &CurrentFace2, &ElementNumber, &Nothing);
                if
					(
                    ((CurrentFace2->Undefined0 >> 8) != (CurrentFace->Undefined0 >> 8)) ||
                    (CurrentFace->Undefined3 > FaceCounter) ||
                    (ElementNumber != MID) || 
					(CurrentFace->MAXflags & 4L)
					)
                    CurrentFace->Undefined3 |= 0x80000000;
            }
        }
		
        for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
        {
            MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
            if(!(CurrentFace->Undefined1 & 0x80000000))
            {
                MAD_GetFace(MO, CurrentFace->Undefined1, &CurrentFace2, &ElementNumber, &Nothing);
                TurnEdgeCompatibles
					(
                    MO,
                    CurrentFace,
                    CurrentFace2,
                    FaceCounter,
                    CurrentFace->Undefined1
					);
            }
			
            if(!(CurrentFace->Undefined2 & 0x80000000))
            {
                MAD_GetFace(MO, CurrentFace->Undefined2, &CurrentFace2, &ElementNumber, &Nothing);
                TurnEdgeCompatibles
					(
                    MO,
                    CurrentFace,
                    CurrentFace2,
                    FaceCounter,
                    CurrentFace->Undefined2
					);
            }
			
            if(!(CurrentFace->Undefined3 & 0x80000000))
            {
                MAD_GetFace(MO, CurrentFace->Undefined3, &CurrentFace2, &ElementNumber, &Nothing);
                TurnEdgeCompatibles
					(
                    MO,
                    CurrentFace,
                    CurrentFace2,
                    FaceCounter,
                    CurrentFace->Undefined3
					);
            }
        }
		
        for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
        {
            MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
            CurrentFace->Undefined0 &= ~0x00000010;
            if(CurrentFace->Undefined1 != MAD_NULL_INDEX)
            {
                CurrentFace->Undefined1 &= 0x7fffffff;
            }
			
            if(CurrentFace->Undefined2 != MAD_NULL_INDEX)
            {
                CurrentFace->Undefined2 &= 0x7fffffff;
            }
			
            if(CurrentFace->Undefined3 != MAD_NULL_INDEX)
            {
                CurrentFace->Undefined3 &= 0x7fffffff;
            }
        }
    }
	
    /* MAD_ComputeNeightbouringOnObject(MO); */
}
unsigned long MAD_ulIsEdge(long i0, long i1)
{
	unsigned long ReturnVal;
	ReturnVal = 1L << (i0 & 0xf);
	if (!((i0 | i1) & 0x10)) return ReturnVal;
	if ((i0 & i1) & 0x10) return 0;
	if (i0  == (i1 & 0xf)) return ReturnVal;
	if ((((i0 & 0xf) + 1) % 3)  == i1) return ReturnVal;
	return 0;
}

void MAD_ComputeEdgeVisibility(MAD_Face        *CurrentFace,MAD_Face        *CurrentFace2,unsigned long i0,unsigned long i1,unsigned long i2)
{
	unsigned long Flags;
	Flags = CurrentFace ->MAXflags & (~7L);
	if (CurrentFace -> MAXflags & MAD_ulIsEdge(i0, i1)) Flags |= 1L;
	if (CurrentFace -> MAXflags & MAD_ulIsEdge(i1, i2)) Flags |= 2L;
	if (CurrentFace -> MAXflags & MAD_ulIsEdge(i2, i0)) Flags |= 4L;
	CurrentFace2 ->MAXflags = Flags;

}
/*
===================================================================================================
This will subdivide by 4 each triangle marked in bit 0 of face->undefined0. Fully linear but
optimisable.
===================================================================================================
*/
void MAD_SubdiviseGO(MAD_World *MW, MAD_WorldNode *MWN , MAD_GeometricObject *MO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   NumberOfFace, NumberOfNewPoint, MID, FaceCounter, Local1, NewNumberOfFaces,
        Nothing;
    unsigned long   ComputeUV;
    MAD_Face        *CurrentFace, *CurrentFace2;
    MAD_Face        *NewPointsIndex;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    /* Be sure to have only the first bit of undefined0 set */
    MAD_RemoveIllgalFaces(MO);
	
    NewNumberOfFaces = NumberOfFace = MAD_GetNumberOfFace(MO);
	
	
    /* Reset undefined0 for each elements */
    for(FaceCounter = 0; FaceCounter < MO->NumberOfElements; FaceCounter++)
        MO->Elements[FaceCounter]->Undefined0 = 0;
	
    /* Propagation of subdiv flag */
	
    /*
	* For(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++) { MAD_GetFace(MO,
	* FaceCounter, &CurrentFace, &MID, &Nothing); if(CurrentFace->Undefined0 & 1) { if
	* (CurrentFace -> Undefined1 != MAD_NULL_INDEX) { MAD_GetFace(MO, CurrentFace->Undefined1,
	* &CurrentFace2, &Nothing, &Nothing); CurrentFace2 -> Undefined0 |= CurrentFace ->
	* Undefined0 & 1; } if (CurrentFace -> Undefined2 != MAD_NULL_INDEX) { MAD_GetFace(MO,
	* CurrentFace->Undefined2, &CurrentFace2, &Nothing, &Nothing); CurrentFace2 -> Undefined0 |=
	* CurrentFace -> Undefined0 & 1; } if (CurrentFace -> Undefined3 != MAD_NULL_INDEX) {
	* MAD_GetFace(MO, CurrentFace->Undefined3, &CurrentFace2, &Nothing, &Nothing); CurrentFace2
	* -> Undefined0 |= CurrentFace -> Undefined0 & 1; } } }
	*/
	
    /* Mark the 3 flags if total subdivision is wanted */
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        if(CurrentFace->Undefined0 & 1)
            CurrentFace->Undefined0 |= (8 + 4 + 2);
		else
			CurrentFace ->MAXflags |= (1<<8);
    }
	
    /* MAKE IT SYMETRIC */
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        if((CurrentFace->Undefined1 != MAD_NULL_INDEX) && (CurrentFace->Undefined0 & 8))
        {
            MAD_GetFace(MO, CurrentFace->Undefined1, &CurrentFace2, &Nothing, &Nothing);
            TagSubdivisionSymetrie(FaceCounter, CurrentFace2);
        }
		
        if((CurrentFace->Undefined2 != MAD_NULL_INDEX) && (CurrentFace->Undefined0 & 4))
        {
            MAD_GetFace(MO, CurrentFace->Undefined2, &CurrentFace2, &Nothing, &Nothing);
            TagSubdivisionSymetrie(FaceCounter, CurrentFace2);
        }
		
        if((CurrentFace->Undefined3 != MAD_NULL_INDEX) && (CurrentFace->Undefined0 & 2))
        {
            MAD_GetFace(MO, CurrentFace->Undefined3, &CurrentFace2, &Nothing, &Nothing);
            TagSubdivisionSymetrie(FaceCounter, CurrentFace2);
        }
    }
	
    /* First find new number of points */
    NumberOfNewPoint = 0;
	
    /*
	* For(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++) { MAD_GetFace(MO,
	* FaceCounter, &CurrentFace, &MID, &Nothing); if(CurrentFace->Undefined0 & 1) {
	* NumberOfNewPoint += 3; if(CurrentFace->Undefined1 == MAD_NULL_INDEX) NumberOfNewPoint++;
	* if(CurrentFace->Undefined2 == MAD_NULL_INDEX) NumberOfNewPoint++;
	* if(CurrentFace->Undefined3 == MAD_NULL_INDEX) NumberOfNewPoint++;
	* MO->Elements[MID]->Undefined0 += 3; NewNumberOfFaces += 3; } else { Local1 =
	* NumberOfNewPoint; if(CurrentFace->Undefined1 != MAD_NULL_INDEX) { MAD_GetFace(MO,
	* CurrentFace->Undefined1, &CurrentFace2, &Nothing, &Nothing); if(CurrentFace2->Undefined0 &
	* 1) NumberOfNewPoint++; } if(CurrentFace->Undefined2 != MAD_NULL_INDEX) { MAD_GetFace(MO,
	* CurrentFace->Undefined2, &CurrentFace2, &Nothing, &Nothing); if(CurrentFace2->Undefined0 &
	* 1) NumberOfNewPoint++; } if(CurrentFace->Undefined3 != MAD_NULL_INDEX) { MAD_GetFace(MO,
	* CurrentFace->Undefined3, &CurrentFace2, &Nothing, &Nothing); if(CurrentFace2->Undefined0 &
	* 1) NumberOfNewPoint++; } Local1 = NumberOfNewPoint - Local1; MO->Elements[MID]->Undefined0
	* += Local1; NewNumberOfFaces += Local1; } }
	*/
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        Local1 = NumberOfNewPoint;
        if(CurrentFace->Undefined0 & 8)
        {
            NumberOfNewPoint++;
            if(CurrentFace->Undefined1 == MAD_NULL_INDEX) NumberOfNewPoint++;
        }
		
        if(CurrentFace->Undefined0 & 4)
        {
            NumberOfNewPoint++;
            if(CurrentFace->Undefined2 == MAD_NULL_INDEX) NumberOfNewPoint++;
        }
		
        if(CurrentFace->Undefined0 & 2)
        {
            NumberOfNewPoint++;
            if(CurrentFace->Undefined3 == MAD_NULL_INDEX) NumberOfNewPoint++;
        }
		
        Local1 = NumberOfNewPoint - Local1;
        MO->Elements[MID]->Undefined0 += Local1;
        NewNumberOfFaces += Local1;
    }
	
    NumberOfNewPoint >>= 1;
	
    /* Realloc new number of points */
    MAD_REALLOC(MAD_Simple3DVertex	, MO->OBJ_PointList		, MO->NumberOfPoints + NumberOfNewPoint);
    MAD_REALLOC(MAD_ColorARGB		, MO->Colors			, MO->NumberOfPoints + NumberOfNewPoint);
    MAD_REALLOC(MAD_ColorARGB		, MO->SecondRLIField	, MO->NumberOfPoints + NumberOfNewPoint);
	if (MWN ->UsersRLIOfObject)
		MAD_REALLOC(MAD_ColorARGB	, MWN ->UsersRLIOfObject, MO->NumberOfPoints + NumberOfNewPoint);
		
    ComputeUV = 0;
    if(MO->OBJ_UVtextureList) ComputeUV = 1;
    if(ComputeUV)
        MAD_REALLOC(MAD_Vertex, MO->OBJ_UVtextureList, MO->NumberOfUV + (NumberOfNewPoint << 1));
    ComputeUV = MO->NumberOfUV + (NumberOfNewPoint << 1);
	
    /* ComputeUV = 0; */
	
    /* Realloc new number of faces */
    for(FaceCounter = 0; FaceCounter < MO->NumberOfElements; FaceCounter++)
    {
        MAD_REALLOC
			(
            MAD_Face,
            MO->Elements[FaceCounter]->Faces,
            MO->Elements[FaceCounter]->NumberOfTriangles + MO->Elements[FaceCounter]->Undefined0
			);
        MO->Elements[FaceCounter]->Undefined1 = MO->Elements[FaceCounter]->NumberOfTriangles;
    }
	
    /* Compute New Neightbour points index */
    NumberOfFace = MAD_GetNumberOfFace(MO);
    NewPointsIndex = (MAD_Face *) malloc(NewNumberOfFaces * sizeof(MAD_Face));
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        NewPointsIndex[FaceCounter].Index[0] = MAD_NULL_INDEX;
        NewPointsIndex[FaceCounter].Index[1] = MAD_NULL_INDEX;
        NewPointsIndex[FaceCounter].Index[2] = MAD_NULL_INDEX;
		
        if(CurrentFace->Undefined1 != MAD_NULL_INDEX)
        {
            MAD_GetFace(MO, CurrentFace->Undefined1, &CurrentFace2, &MID, &Nothing);
            if(CurrentFace->Undefined0 & 8)
            {
                if(CurrentFace->Undefined1 < FaceCounter)
                {
                    if(ComputeUV)
                    {
                        NewPointsIndex[FaceCounter].UVIndex[0] = MAD_CreateMiddlePointUV
                            (
							MW,
							MO,
							CurrentFace->UVIndex[0],
							CurrentFace->UVIndex[1],
							CurrentFace2,
							NewPointsIndex + CurrentFace->Undefined1,
							FaceCounter
                            );
                    }
					
                    /* Retrieve it */
                    if(CurrentFace2->Undefined1 == FaceCounter)
                    {
                        NewPointsIndex[FaceCounter].Index[0] = NewPointsIndex[CurrentFace->
                            Undefined1].Index[0];
                    }
					
                    if(CurrentFace2->Undefined2 == FaceCounter)
                    {
                        NewPointsIndex[FaceCounter].Index[0] = NewPointsIndex[CurrentFace->
                            Undefined1].Index[1];
                    }
					
                    if(CurrentFace2->Undefined3 == FaceCounter)
                    {
                        NewPointsIndex[FaceCounter].Index[0] = NewPointsIndex[CurrentFace->
                            Undefined1].Index[2];
                    }
                }
                else
                {
                    /* Create it */
                    NewPointsIndex[FaceCounter].Index[0] = MAD_CreateMiddlePoint
                        (
						MW,
						MWN,
						MO,
						CurrentFace->Index[0],
						CurrentFace->Index[1]
                        );
                    if(ComputeUV)
                    {
                        NewPointsIndex[FaceCounter].UVIndex[0] = MAD_CreateMiddlePointUV
                            (
							MW,
							MO,
							CurrentFace->UVIndex[0],
							CurrentFace->UVIndex[1],
							NULL,
							NULL,
							FaceCounter
                            );
                    }
                }
            }
        }
        else if(CurrentFace->Undefined0 & 8)    /* Create it */
        {
            if(ComputeUV)
            {
                NewPointsIndex[FaceCounter].UVIndex[0] = MAD_CreateMiddlePointUV
                    (
					MW,
					MO,
					CurrentFace->UVIndex[0],
					CurrentFace->UVIndex[1],
					NULL,
					NULL,
					FaceCounter
                    );
            }
			
            NewPointsIndex[FaceCounter].Index[0] = MAD_CreateMiddlePoint
                (
				MW,
				MWN,
				MO,
				CurrentFace->Index[0],
				CurrentFace->Index[1]
                );
        }
		
        if(CurrentFace->Undefined2 != MAD_NULL_INDEX)
        {
            MAD_GetFace(MO, CurrentFace->Undefined2, &CurrentFace2, &MID, &Nothing);
            if(CurrentFace->Undefined0 & 4)
            {
                if(CurrentFace->Undefined2 < FaceCounter)
                {
                    if(ComputeUV)
                    {
                        NewPointsIndex[FaceCounter].UVIndex[1] = MAD_CreateMiddlePointUV
                            (
							MW,
							MO,
							CurrentFace->UVIndex[1],
							CurrentFace->UVIndex[2],
							CurrentFace2,
							NewPointsIndex + CurrentFace->Undefined2,
							FaceCounter
                            );
                    }
					
                    /* Retrieve it */
                    if(CurrentFace2->Undefined1 == FaceCounter)
                        NewPointsIndex[FaceCounter].Index[1] = NewPointsIndex[CurrentFace->
						Undefined2].Index[0];
                    if(CurrentFace2->Undefined2 == FaceCounter)
                        NewPointsIndex[FaceCounter].Index[1] = NewPointsIndex[CurrentFace->
						Undefined2].Index[1];
                    if(CurrentFace2->Undefined3 == FaceCounter)
                        NewPointsIndex[FaceCounter].Index[1] = NewPointsIndex[CurrentFace->
						Undefined2].Index[2];
                }
                else
                {
                    if(ComputeUV)
                    {
                        NewPointsIndex[FaceCounter].UVIndex[1] = MAD_CreateMiddlePointUV
                            (
							MW,
							MO,
							CurrentFace->UVIndex[1],
							CurrentFace->UVIndex[2],
							NULL,
							NULL,
							FaceCounter
                            );
                    }
					
                    /* Create it */
                    NewPointsIndex[FaceCounter].Index[1] = MAD_CreateMiddlePoint
                        (
						MW,
						MWN,
						MO,
						CurrentFace->Index[1],
						CurrentFace->Index[2]
                        );
                }
            }
        }
        else if(CurrentFace->Undefined0 & 4)    /* Create it */
        {
            if(ComputeUV)
            {
                NewPointsIndex[FaceCounter].UVIndex[1] = MAD_CreateMiddlePointUV
                    (
					MW,
					MO,
					CurrentFace->UVIndex[1],
					CurrentFace->UVIndex[2],
					NULL,
					NULL,
					FaceCounter
                    );
            }
			
            NewPointsIndex[FaceCounter].Index[1] = MAD_CreateMiddlePoint
                (
				MW,
				MWN,
				MO,
				CurrentFace->Index[1],
				CurrentFace->Index[2]
                );
        }
		
        if(CurrentFace->Undefined3 != MAD_NULL_INDEX)
        {
            MAD_GetFace(MO, CurrentFace->Undefined3, &CurrentFace2, &MID, &Nothing);
            if(CurrentFace->Undefined0 & 2)
            {
                if(CurrentFace->Undefined3 < FaceCounter)
                {
                    if(ComputeUV)
                    {
                        NewPointsIndex[FaceCounter].UVIndex[2] = MAD_CreateMiddlePointUV
                            (
							MW,
							MO,
							CurrentFace->UVIndex[2],
							CurrentFace->UVIndex[0],
							CurrentFace2,
							NewPointsIndex + CurrentFace->Undefined3,
							FaceCounter
                            );
                    }
					
                    /* Retrieve it */
                    if(CurrentFace2->Undefined1 == FaceCounter)
                        NewPointsIndex[FaceCounter].Index[2] = NewPointsIndex[CurrentFace->
						Undefined3].Index[0];
                    if(CurrentFace2->Undefined2 == FaceCounter)
                        NewPointsIndex[FaceCounter].Index[2] = NewPointsIndex[CurrentFace->
						Undefined3].Index[1];
                    if(CurrentFace2->Undefined3 == FaceCounter)
                        NewPointsIndex[FaceCounter].Index[2] = NewPointsIndex[CurrentFace->
						Undefined3].Index[2];
                }
                else
                {
                    if(ComputeUV)
                    {
                        NewPointsIndex[FaceCounter].UVIndex[2] = MAD_CreateMiddlePointUV
                            (
							MW,
							MO,
							CurrentFace->UVIndex[2],
							CurrentFace->UVIndex[0],
							NULL,
							NULL,
							FaceCounter
                            );
                    }
					
                    /* Create it */
                    NewPointsIndex[FaceCounter].Index[2] = MAD_CreateMiddlePoint
                        (
						MW,
						MWN,
						MO,
						CurrentFace->Index[2],
						CurrentFace->Index[0]
                        );
                }
            }
        }
        else if(CurrentFace->Undefined0 & 2)    /* Create it */
        {
            if(ComputeUV)
            {
                NewPointsIndex[FaceCounter].UVIndex[2] = MAD_CreateMiddlePointUV
                    (
					MW,
					MO,
					CurrentFace->UVIndex[2],
					CurrentFace->UVIndex[0],
					NULL,
					NULL,
					FaceCounter
                    );
            }
			
            NewPointsIndex[FaceCounter].Index[2] = MAD_CreateMiddlePoint
                (
				MW,
				MWN,
				MO,
				CurrentFace->Index[2],
				CurrentFace->Index[0]
                );
        }
    }
	
    /* Subdivide */
    NumberOfFace = MAD_GetNumberOfFace(MO);
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        Local1 = 0;
        if(NewPointsIndex[FaceCounter].Index[0] != MAD_NULL_INDEX) Local1++;
        if(NewPointsIndex[FaceCounter].Index[1] != MAD_NULL_INDEX) Local1++;
        if(NewPointsIndex[FaceCounter].Index[2] != MAD_NULL_INDEX) Local1++;
        switch(Local1)
        {
        case 0: /* No subdivision */
            break;
        case 1: /* One triangle */
            CurrentFace2 = MO->Elements[MID]->Faces + MO->Elements[MID]->Undefined1++;
            *CurrentFace2 = *CurrentFace;
            if(NewPointsIndex[FaceCounter].Index[0] != MAD_NULL_INDEX)
            {
                CurrentFace2->Index[0] = NewPointsIndex[FaceCounter].Index[0];
                CurrentFace2->Index[1] = CurrentFace->Index[1];
                CurrentFace2->Index[2] = CurrentFace->Index[2];
                CurrentFace->Index[1] = NewPointsIndex[FaceCounter].Index[0];
                CurrentFace2->UVIndex[0] = NewPointsIndex[FaceCounter].UVIndex[0];
                CurrentFace2->UVIndex[1] = CurrentFace->UVIndex[1];
                CurrentFace2->UVIndex[2] = CurrentFace->UVIndex[2];
                CurrentFace->UVIndex[1] = NewPointsIndex[FaceCounter].UVIndex[0];
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace2,0x10,1,2);
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace,0,0x10,2);
            } else
            if(NewPointsIndex[FaceCounter].Index[1] != MAD_NULL_INDEX)
            {
                CurrentFace2->Index[0] = CurrentFace->Index[0];
                CurrentFace2->Index[1] = NewPointsIndex[FaceCounter].Index[1];
                CurrentFace2->Index[2] = CurrentFace->Index[2];
                CurrentFace->Index[2] = NewPointsIndex[FaceCounter].Index[1];
                CurrentFace2->UVIndex[0] = CurrentFace->UVIndex[0];
                CurrentFace2->UVIndex[1] = NewPointsIndex[FaceCounter].UVIndex[1];
                CurrentFace2->UVIndex[2] = CurrentFace->UVIndex[2];
                CurrentFace->UVIndex[2] = NewPointsIndex[FaceCounter].UVIndex[1];
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace2,0,0x11,2);
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace,0,1,0x11);
            } else
            if(NewPointsIndex[FaceCounter].Index[2] != MAD_NULL_INDEX)
            {
                CurrentFace2->Index[0] = NewPointsIndex[FaceCounter].Index[2];
                CurrentFace2->Index[1] = CurrentFace->Index[1];
                CurrentFace2->Index[2] = CurrentFace->Index[2];
                CurrentFace->Index[2] = NewPointsIndex[FaceCounter].Index[2];
                CurrentFace2->UVIndex[0] = NewPointsIndex[FaceCounter].UVIndex[2];
                CurrentFace2->UVIndex[1] = CurrentFace->UVIndex[1];
                CurrentFace2->UVIndex[2] = CurrentFace->UVIndex[2];
                CurrentFace->UVIndex[2] = NewPointsIndex[FaceCounter].UVIndex[2];
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace2,0x12,1,2);
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace,0,1,0x12);
            }
			
            break;
        case 2: /* 2 triangles */
            if(NewPointsIndex[FaceCounter].Index[0] == MAD_NULL_INDEX)
            {
                CurrentFace2 = MO->Elements[MID]->Faces + MO->Elements[MID]->Undefined1++;
                *CurrentFace2 = *CurrentFace;
                CurrentFace2->Index[0] = CurrentFace->Index[0];
                CurrentFace2->Index[1] = NewPointsIndex[FaceCounter].Index[1];
                CurrentFace2->Index[2] = NewPointsIndex[FaceCounter].Index[2];
                CurrentFace2->UVIndex[0] = CurrentFace->UVIndex[0];
                CurrentFace2->UVIndex[1] = NewPointsIndex[FaceCounter].UVIndex[1];
                CurrentFace2->UVIndex[2] = NewPointsIndex[FaceCounter].UVIndex[2];
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace2,0,0x11,0x12);
																																					
                CurrentFace2 = MO->Elements[MID]->Faces + MO->Elements[MID]->Undefined1++;
                *CurrentFace2 = *CurrentFace;
                CurrentFace2->Index[0] = NewPointsIndex[FaceCounter].Index[2];
                CurrentFace2->Index[1] = NewPointsIndex[FaceCounter].Index[1];
                CurrentFace2->Index[2] = CurrentFace->Index[2];
                CurrentFace2->UVIndex[0] = NewPointsIndex[FaceCounter].UVIndex[2];
                CurrentFace2->UVIndex[1] = NewPointsIndex[FaceCounter].UVIndex[1];
                CurrentFace2->UVIndex[2] = CurrentFace->UVIndex[2];
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace2,0x12,0x11,2);
				
                CurrentFace->Index[2] = NewPointsIndex[FaceCounter].Index[1];
                CurrentFace->UVIndex[2] = NewPointsIndex[FaceCounter].UVIndex[1];
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace,0,1,0x11);
            } else
            if(NewPointsIndex[FaceCounter].Index[1] == MAD_NULL_INDEX)
            {
                CurrentFace2 = MO->Elements[MID]->Faces + MO->Elements[MID]->Undefined1++;
                *CurrentFace2 = *CurrentFace;
                CurrentFace2->Index[0] = NewPointsIndex[FaceCounter].Index[2];
                CurrentFace2->Index[1] = NewPointsIndex[FaceCounter].Index[0];
                CurrentFace2->Index[2] = CurrentFace->Index[2];
                CurrentFace2->UVIndex[0] = NewPointsIndex[FaceCounter].UVIndex[2];
                CurrentFace2->UVIndex[1] = NewPointsIndex[FaceCounter].UVIndex[0];
                CurrentFace2->UVIndex[2] = CurrentFace->UVIndex[2];
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace2,0x12,0x10,2);
				
                CurrentFace2 = MO->Elements[MID]->Faces + MO->Elements[MID]->Undefined1++;
                *CurrentFace2 = *CurrentFace;
                CurrentFace2->Index[0] = NewPointsIndex[FaceCounter].Index[0];
                CurrentFace2->Index[1] = CurrentFace->Index[1];
                CurrentFace2->Index[2] = CurrentFace->Index[2];
                CurrentFace2->UVIndex[0] = NewPointsIndex[FaceCounter].UVIndex[0];
                CurrentFace2->UVIndex[1] = CurrentFace->UVIndex[1];
                CurrentFace2->UVIndex[2] = CurrentFace->UVIndex[2];
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace2,0x10,1,2);
				
                CurrentFace->Index[1] = NewPointsIndex[FaceCounter].Index[0];
                CurrentFace->Index[2] = NewPointsIndex[FaceCounter].Index[2];
                CurrentFace->UVIndex[1] = NewPointsIndex[FaceCounter].UVIndex[0];
                CurrentFace->UVIndex[2] = NewPointsIndex[FaceCounter].UVIndex[2];
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace,0,0x10,0x12);
            } else
            if(NewPointsIndex[FaceCounter].Index[2] == MAD_NULL_INDEX)
            {
                CurrentFace2 = MO->Elements[MID]->Faces + MO->Elements[MID]->Undefined1++;
                *CurrentFace2 = *CurrentFace;
                CurrentFace2->Index[0] = NewPointsIndex[FaceCounter].Index[1];
                CurrentFace2->Index[1] = CurrentFace->Index[0];
                CurrentFace2->Index[2] = NewPointsIndex[FaceCounter].Index[0];
                CurrentFace2->UVIndex[0] = NewPointsIndex[FaceCounter].UVIndex[1];
                CurrentFace2->UVIndex[1] = CurrentFace->UVIndex[0];
                CurrentFace2->UVIndex[2] = NewPointsIndex[FaceCounter].UVIndex[0];
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace2,0x11,0,0x10);
				
                CurrentFace2 = MO->Elements[MID]->Faces + MO->Elements[MID]->Undefined1++;
                *CurrentFace2 = *CurrentFace;
                CurrentFace2->Index[0] = NewPointsIndex[FaceCounter].Index[1];
                CurrentFace2->Index[1] = CurrentFace->Index[2];
                CurrentFace2->Index[2] = CurrentFace->Index[0];
                CurrentFace2->UVIndex[0] = NewPointsIndex[FaceCounter].UVIndex[1];
                CurrentFace2->UVIndex[1] = CurrentFace->UVIndex[2];
                CurrentFace2->UVIndex[2] = CurrentFace->UVIndex[0];
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace2,0x11,2,0);
				
                CurrentFace->Index[0] = NewPointsIndex[FaceCounter].Index[0];
                CurrentFace->Index[2] = NewPointsIndex[FaceCounter].Index[1];
                CurrentFace->UVIndex[0] = NewPointsIndex[FaceCounter].UVIndex[0];
                CurrentFace->UVIndex[2] = NewPointsIndex[FaceCounter].UVIndex[1];
				MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace,0x10,1,0x11);
            }
            break;
        case 3: /* 3 triangles */
            CurrentFace2 = MO->Elements[MID]->Faces + MO->Elements[MID]->Undefined1++;
            *CurrentFace2 = *CurrentFace;
            CurrentFace2->Index[0] = NewPointsIndex[FaceCounter].Index[0];
            CurrentFace2->Index[1] = NewPointsIndex[FaceCounter].Index[1];
            CurrentFace2->Index[2] = NewPointsIndex[FaceCounter].Index[2];
            CurrentFace2->UVIndex[0] = NewPointsIndex[FaceCounter].UVIndex[0];
            CurrentFace2->UVIndex[1] = NewPointsIndex[FaceCounter].UVIndex[1];
            CurrentFace2->UVIndex[2] = NewPointsIndex[FaceCounter].UVIndex[2];
			MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace2,0x10,0x11,0x12);
			
            CurrentFace2 = MO->Elements[MID]->Faces + MO->Elements[MID]->Undefined1++;
            *CurrentFace2 = *CurrentFace;
            CurrentFace2->Index[0] = NewPointsIndex[FaceCounter].Index[0];
            CurrentFace2->Index[1] = CurrentFace->Index[1];
            CurrentFace2->Index[2] = NewPointsIndex[FaceCounter].Index[1];
            CurrentFace2->UVIndex[0] = NewPointsIndex[FaceCounter].UVIndex[0];
            CurrentFace2->UVIndex[1] = CurrentFace->UVIndex[1];
            CurrentFace2->UVIndex[2] = NewPointsIndex[FaceCounter].UVIndex[1];
			MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace2,0x10,1,0x11);
			
            CurrentFace2 = MO->Elements[MID]->Faces + MO->Elements[MID]->Undefined1++;
            *CurrentFace2 = *CurrentFace;
            CurrentFace2->Index[0] = NewPointsIndex[FaceCounter].Index[2];
            CurrentFace2->Index[1] = NewPointsIndex[FaceCounter].Index[1];
            CurrentFace2->Index[2] = CurrentFace->Index[2];
            CurrentFace2->UVIndex[0] = NewPointsIndex[FaceCounter].UVIndex[2];
            CurrentFace2->UVIndex[1] = NewPointsIndex[FaceCounter].UVIndex[1];
            CurrentFace2->UVIndex[2] = CurrentFace->UVIndex[2];
			MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace2,0x12,0x11,2);
			
            CurrentFace->Index[1] = NewPointsIndex[FaceCounter].Index[0];
            CurrentFace->Index[2] = NewPointsIndex[FaceCounter].Index[2];
            CurrentFace->UVIndex[1] = NewPointsIndex[FaceCounter].UVIndex[0];
            CurrentFace->UVIndex[2] = NewPointsIndex[FaceCounter].UVIndex[2];
			MAD_ComputeEdgeVisibility(CurrentFace,CurrentFace,0,0x10,0x12);
			
            break;
        }
    }
	
    free(NewPointsIndex);
    if(ComputeUV)
        MAD_REALLOC(MAD_Vertex, MO->OBJ_UVtextureList, MO->NumberOfUV);
	
    /* Reset flags */
    for(FaceCounter = 0; FaceCounter < MO->NumberOfElements; FaceCounter++)
    {
        MO->Elements[FaceCounter]->NumberOfTriangles = MO->Elements[FaceCounter]->Undefined1;
    }
	
    NewNumberOfFaces = NumberOfFace = MAD_GetNumberOfFace(MO);
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        CurrentFace->Undefined0 &= 0xffffff00;
    }
	
}

/*
===================================================================================================
This will write neightbouring in undefiened fiels 1 , 2 & 3 of each faces
===================================================================================================
*/
void MAD_ComputeNeightbouring(MAD_World *MW)
{
    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, MAD_ComputeNeightbouringCLBCK);
}

/*
===================================================================================================
This will write neightbouring in undefiened fiels 1 , 2 & 3 of each faces
===================================================================================================
*/
void MAD_ComputeAllNormales(MAD_World *MW)
{
    MAD_NodeEnum(MW, ID_MAD_GeometricObject_V0, MAD_ComputeNormales);
}

/*
===================================================================================================
This will write neightbouring in undefiened fiels 1 , 2 & 3 of each faces
===================================================================================================
*/
void MAD_RemoveIllgalFaces(MAD_GeometricObject *MO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Face        *CurrentFace, *CurrentFace2;
    unsigned long   Counter, Counter2;
    unsigned long   NumberOfFace;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    for(Counter = 0; Counter < MO->NumberOfElements; Counter++)
    {
        CurrentFace = CurrentFace2 = MO->Elements[Counter]->Faces;
        NumberOfFace = 0;
        for(Counter2 = 0; Counter2 < MO->Elements[Counter]->NumberOfTriangles; Counter2++)
        {
            if
				(
                (CurrentFace->Index[0] != CurrentFace->Index[1]) &&
                (CurrentFace->Index[1] != CurrentFace->Index[2]) &&
                (CurrentFace->Index[0] != CurrentFace->Index[2]) &&
                (CurrentFace->Index[0] <= MO->NumberOfPoints) &&
                (CurrentFace->Index[1] <= MO->NumberOfPoints) &&
                (CurrentFace->Index[2] <= MO->NumberOfPoints)
				)
            {
                *CurrentFace2 = *CurrentFace;
                NumberOfFace++;
                CurrentFace2++;
            }
			
            CurrentFace++;
        }
		
        MO->Elements[Counter]->NumberOfTriangles = NumberOfFace;
    }
}

/*
===================================================================================================
This will compute the list of index used by each element
===================================================================================================
*/
void MAD_ComputeUsedIndexPerElements(MAD_GeometricObject *MO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    /* Unsigned long *NewPlaces; */
    unsigned long   Counter, ECounter, Local;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    /* NewPlaces = malloc(4L * MO -> NumberOfPoints); */
    for(ECounter = 0; ECounter < MO->NumberOfElements; ECounter++)
    {
        /* Reset flags */
        for(Counter = 0; Counter < MO->NumberOfPoints; Counter++)
        {
            *(unsigned long *) &(MO->OBJ_PointList[Counter].Normale.x) &= 0xfffffffe;
        }
		
        /* Tagging flags */
        for(Counter = 0; Counter < MO->Elements[ECounter]->NumberOfTriangles; Counter++)
        {
            *(unsigned long *) &
                (MO->OBJ_PointList[MO->Elements[ECounter]->Faces[Counter].Index[0]].Normale.x) |= 1;
            *(unsigned long *) &
                (MO->OBJ_PointList[MO->Elements[ECounter]->Faces[Counter].Index[1]].Normale.x) |= 1;
            *(unsigned long *) &
                (MO->OBJ_PointList[MO->Elements[ECounter]->Faces[Counter].Index[2]].Normale.x) |= 1;
        }
		
        /* Counting Flags */
        Local = 0;
        for(Counter = 0; Counter < MO->NumberOfPoints; Counter++)
        {
            if(*(unsigned long *) &(MO->OBJ_PointList[Counter].Normale.x) & 1) Local++;
        }
		
        MO->Elements[ECounter]->NumberOfUsedIndex = Local;
        MAD_MALLOC(unsigned long, MO->Elements[ECounter]->UsedIndex, Local);
        Local = 0;
        for(Counter = 0; Counter < MO->NumberOfPoints; Counter++)
        {
            if(*(unsigned long *) &(MO->OBJ_PointList[Counter].Normale.x) & 1)
            {
                MO->Elements[ECounter]->UsedIndex[Local] = Counter;
                Local++;
            }
        }
    }
}

/*
---------------------------------------------------------------------------------------------------
This will create new elements for have one UV per vertex inside one Element Feild modified will
be: Geometric Object Element: unsigned long NumberOfUsedIndex; unsigned long *UsedIndex;
unsigned long OneUVPerVertexPerElementBase; Geometric Object : unsigned long NumberOfUV;
MAD_Vertex *OBJ_UVtextureList; + number of elements.
---------------------------------------------------------------------------------------------------
*/
typedef struct  MAD_FaceAdditionnalInfo_
{
	unsigned long	ulUVBaseIsComputed;
    float           UBase;
    float           VBase;
	MAD_Vertex      UV[3];
    unsigned long   Compatibilitys[3];
    unsigned long   Channel;
    MAD_Face        Original;
    unsigned long   OriginalMaterialID;
} MAD_FaceAdditionnalInfo;                                                                                                                                                    

/*
===================================================================================================
===================================================================================================
*/
unsigned long GetNeightbourIndex(MAD_Face *Current2, unsigned long FaceCounter)
{
    if(Current2->Undefined1 == FaceCounter) return 0;
    if(Current2->Undefined2 == FaceCounter) return 1;
    if(Current2->Undefined3 == FaceCounter) return 2;
    return MAD_NULL_INDEX;
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long MAD_bIsDiff(float a, float b)
{
	float z;
	z = a-b;
	if (z < 0.0f) z = -z;
#ifdef JADEFUSION
	if (z < 0.0001f) return 0;
#else
	if (z == 0.0f) return 0;
#endif
	//	if (z < 0.000001f) return 0;
	return 1;
}
/* mais non,, je veut pas du floor et pis c'est très beau ça...*/
unsigned long MAD_bIsModulo1(float a)
{
	if(a < 0.0f) a = -a;
	while (a >= 10000000000000000000000.0f) a -= 10000000000000000000000.0f;
	while (a >= 100000000000000000000.0f) a -= 100000000000000000000.0f;
	while (a >= 1000000000000000000.0f) a -= 1000000000000000000.0f;
	while (a >= 10000000000000000.0f) a -= 10000000000000000.0f;
	while (a >= 100000000000000.0f) a -= 100000000000000.0f;
	while (a >= 1000000000000.0f) a -= 1000000000000.0f;
	while (a >= 10000000000.0f) a -= 10000000000.0f;
	while (a >= 100000000.0f) a -= 100000000.0f;
	while (a >= 1000000.0f) a -= 1000000.0f;
	while (a >= 10000.0f) a -= 10000.0f;
	while (a >= 1000.0f) a -= 1000.0f;
	while (a >= 100.0f) a -= 100.0f;
	while (a >= 10.0f) a -= 10.0f;
	while (a >= 1.0f) a -= 1.0f;
	if (a < 0.0000000001f) return 1 ;
	if (a > 0.9999999999f) return 1 ;
	return 0;
}


static MAD_FaceAdditionnalInfo  *Addinf;
static MAD_Vertex      *WorkingUV;
static unsigned long MAD_COUVPVS_TAG = 0;
/*
===================================================================================================
This function will compute the second compatibility and the base...
===================================================================================================
*/
unsigned long MAD_ulCouldBeInsert(unsigned long FaceCounter)
{
#ifdef JADEFUSION
	float fBaseU=0.0f, fBaseV=0.0f;
#endif
	if (Addinf[FaceCounter].ulUVBaseIsComputed) return 0;
	Addinf[FaceCounter].UBase = 0.0f;
	Addinf[FaceCounter].VBase = 0.0f;
	/* first compute a base */
	if (*(unsigned long *)&WorkingUV[Addinf[FaceCounter].Original.Index[0]].z == MAD_COUVPVS_TAG)
	{
#ifdef JADEFUSION
		fBaseU = WorkingUV[Addinf[FaceCounter].Original.Index[0]].x - Addinf[FaceCounter].UV[0].x;
		fBaseV = WorkingUV[Addinf[FaceCounter].Original.Index[0]].y - Addinf[FaceCounter].UV[0].y;
#else
		Addinf[FaceCounter].UBase = WorkingUV[Addinf[FaceCounter].Original.Index[0]].x - Addinf[FaceCounter].UV[0].x;
		Addinf[FaceCounter].VBase = WorkingUV[Addinf[FaceCounter].Original.Index[0]].y - Addinf[FaceCounter].UV[0].y;
#endif
	} else
	if (*(unsigned long *)&WorkingUV[Addinf[FaceCounter].Original.Index[1]].z == MAD_COUVPVS_TAG)
	{
#ifdef JADEFUSION
		fBaseU = WorkingUV[Addinf[FaceCounter].Original.Index[1]].x - Addinf[FaceCounter].UV[1].x;
		fBaseV = WorkingUV[Addinf[FaceCounter].Original.Index[1]].y - Addinf[FaceCounter].UV[1].y;
#else
		Addinf[FaceCounter].UBase = WorkingUV[Addinf[FaceCounter].Original.Index[1]].x - Addinf[FaceCounter].UV[1].x;
		Addinf[FaceCounter].VBase = WorkingUV[Addinf[FaceCounter].Original.Index[1]].y - Addinf[FaceCounter].UV[1].y;
#endif
	} else
	if (*(unsigned long *)&WorkingUV[Addinf[FaceCounter].Original.Index[2]].z == MAD_COUVPVS_TAG)
	{
#ifdef JADEFUSION
		fBaseU = WorkingUV[Addinf[FaceCounter].Original.Index[2]].x - Addinf[FaceCounter].UV[2].x;
		fBaseV = WorkingUV[Addinf[FaceCounter].Original.Index[2]].y - Addinf[FaceCounter].UV[2].y;
#else
		Addinf[FaceCounter].UBase = WorkingUV[Addinf[FaceCounter].Original.Index[2]].x - Addinf[FaceCounter].UV[2].x;
		Addinf[FaceCounter].VBase = WorkingUV[Addinf[FaceCounter].Original.Index[2]].y - Addinf[FaceCounter].UV[2].y;
#endif
	}

	if (!MAD_bIsModulo1(Addinf[FaceCounter].UBase)) return 0;
	if (!MAD_bIsModulo1(Addinf[FaceCounter].VBase)) return 0;

	/* then verify that the base is correct  */
	if (*(unsigned long *)&WorkingUV[Addinf[FaceCounter].Original.Index[0]].z == MAD_COUVPVS_TAG)
	{
#ifdef JADEFUSION
		if (MAD_bIsDiff(WorkingUV[Addinf[FaceCounter].Original.Index[0]].x,Addinf[FaceCounter].UV[0].x + fBaseU)) 
			return 0;
		if (MAD_bIsDiff(WorkingUV[Addinf[FaceCounter].Original.Index[0]].y,Addinf[FaceCounter].UV[0].y + fBaseV)) 
			return 0;
#else
		if (MAD_bIsDiff(WorkingUV[Addinf[FaceCounter].Original.Index[0]].x,Addinf[FaceCounter].UV[0].x + Addinf[FaceCounter].UBase)) return 0;
		if (MAD_bIsDiff(WorkingUV[Addinf[FaceCounter].Original.Index[0]].y,Addinf[FaceCounter].UV[0].y + Addinf[FaceCounter].VBase)) return 0;
#endif
	}
	if (*(unsigned long *)&WorkingUV[Addinf[FaceCounter].Original.Index[1]].z == MAD_COUVPVS_TAG)
	{
#ifdef JADEFUSION
		if (MAD_bIsDiff(WorkingUV[Addinf[FaceCounter].Original.Index[1]].x,Addinf[FaceCounter].UV[1].x + fBaseU)) 
			return 0;
		if (MAD_bIsDiff(WorkingUV[Addinf[FaceCounter].Original.Index[1]].y,Addinf[FaceCounter].UV[1].y + fBaseV)) 
			return 0;
#else
		if (MAD_bIsDiff(WorkingUV[Addinf[FaceCounter].Original.Index[1]].x,Addinf[FaceCounter].UV[1].x + Addinf[FaceCounter].UBase)) return 0;
		if (MAD_bIsDiff(WorkingUV[Addinf[FaceCounter].Original.Index[1]].y,Addinf[FaceCounter].UV[1].y + Addinf[FaceCounter].VBase)) return 0;
#endif
	}
	if (*(unsigned long *)&WorkingUV[Addinf[FaceCounter].Original.Index[2]].z == MAD_COUVPVS_TAG)
	{
#ifdef JADEFUSION
		if (MAD_bIsDiff(WorkingUV[Addinf[FaceCounter].Original.Index[2]].x,Addinf[FaceCounter].UV[2].x + fBaseU)) 
			return 0;
		if (MAD_bIsDiff(WorkingUV[Addinf[FaceCounter].Original.Index[2]].y,Addinf[FaceCounter].UV[2].y + fBaseV)) 
			return 0;
#else
		if (MAD_bIsDiff(WorkingUV[Addinf[FaceCounter].Original.Index[2]].x,Addinf[FaceCounter].UV[2].x + Addinf[FaceCounter].UBase)) return 0;
		if (MAD_bIsDiff(WorkingUV[Addinf[FaceCounter].Original.Index[2]].y,Addinf[FaceCounter].UV[2].y + Addinf[FaceCounter].VBase)) return 0;
#endif
	}

#ifdef JADEFUSION
	Addinf[FaceCounter].UBase = fBaseU;
	Addinf[FaceCounter].VBase = fBaseV;
#endif

	/* The tagging all the vertices */
	WorkingUV[Addinf[FaceCounter].Original.Index[0]].x = Addinf[FaceCounter].UV[0].x + Addinf[FaceCounter].UBase;
	WorkingUV[Addinf[FaceCounter].Original.Index[0]].y = Addinf[FaceCounter].UV[0].y + Addinf[FaceCounter].VBase;
	WorkingUV[Addinf[FaceCounter].Original.Index[1]].x = Addinf[FaceCounter].UV[1].x + Addinf[FaceCounter].UBase;
	WorkingUV[Addinf[FaceCounter].Original.Index[1]].y = Addinf[FaceCounter].UV[1].y + Addinf[FaceCounter].VBase;
	WorkingUV[Addinf[FaceCounter].Original.Index[2]].x = Addinf[FaceCounter].UV[2].x + Addinf[FaceCounter].UBase;
	WorkingUV[Addinf[FaceCounter].Original.Index[2]].y = Addinf[FaceCounter].UV[2].y + Addinf[FaceCounter].VBase;

	*(unsigned long *)&WorkingUV[Addinf[FaceCounter].Original.Index[0]].z = MAD_COUVPVS_TAG;
	*(unsigned long *)&WorkingUV[Addinf[FaceCounter].Original.Index[1]].z = MAD_COUVPVS_TAG;
	*(unsigned long *)&WorkingUV[Addinf[FaceCounter].Original.Index[2]].z = MAD_COUVPVS_TAG;
	Addinf[FaceCounter].ulUVBaseIsComputed = 1;
	return 1;
}

/*
===================================================================================================
===================================================================================================
*/
void MAD_Propagation(unsigned long FaceCounter)
{
    /* Recursive funciotn -> no stack */

    if(Addinf[FaceCounter].Compatibilitys[0] != MAD_NULL_INDEX)
    {
        if(Addinf[Addinf[FaceCounter].Compatibilitys[0]].Channel == MAD_NULL_INDEX)
        {
			if (MAD_ulCouldBeInsert(Addinf[FaceCounter].Compatibilitys[0]))
			{
				Addinf[Addinf[FaceCounter].Compatibilitys[0]].Channel = Addinf[FaceCounter].Channel;
				MAD_Propagation(Addinf[FaceCounter].Compatibilitys[0]);
			}
        }
    }
	
    if(Addinf[FaceCounter].Compatibilitys[1] != MAD_NULL_INDEX)
    {
        if(Addinf[Addinf[FaceCounter].Compatibilitys[1]].Channel == MAD_NULL_INDEX)
        {
			if (MAD_ulCouldBeInsert(Addinf[FaceCounter].Compatibilitys[1]))
			{
				Addinf[Addinf[FaceCounter].Compatibilitys[1]].Channel = Addinf[FaceCounter].Channel;
				MAD_Propagation(Addinf[FaceCounter].Compatibilitys[1]);
			}
        }
    }
	
    if(Addinf[FaceCounter].Compatibilitys[2] != MAD_NULL_INDEX)
    {
        if(Addinf[Addinf[FaceCounter].Compatibilitys[2]].Channel == MAD_NULL_INDEX)
        {
			if (MAD_ulCouldBeInsert(Addinf[FaceCounter].Compatibilitys[2]))
			{
				Addinf[Addinf[FaceCounter].Compatibilitys[2]].Channel = Addinf[FaceCounter].Channel;
				MAD_Propagation(Addinf[FaceCounter].Compatibilitys[2]);
			}
		}
    }
}

/*
===================================================================================================
This will make a mesh with one uv per point per element. Fully linear. 
MAD_RestoreOneIndexPerUVPerElementSolution
===================================================================================================
*/
void MAD_ComputeOneIndexPerUVPerElementSolution(MAD_GeometricObject *MO,unsigned long FromJade)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   FaceCounter, NumberOfFace, Nothing, ECounter, ACounter;
    MAD_Face        *Current, *Current2;
    MAD_Vertex      *SavedUV ;
    MAD_Simple3DVertex stCrossproduct1 , stCrossproduct2 ;
	
    unsigned long   *Convert;
    unsigned long   Material1, Material2, NewNumberOfElement;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    if(!MO->OBJ_UVtextureList) return;
#ifdef JADEFUSION
    WorkingUV = (MAD_Vertex*)malloc(sizeof(MAD_Vertex) * MO->NumberOfPoints);
#else
	WorkingUV = malloc(sizeof(MAD_Vertex) * MO->NumberOfPoints);
#endif	
    /* 1 reset links with other */
    MAD_ComputeNeightbouringOnObject(MO);
    NumberOfFace = MAD_GetNumberOfFace(MO);
#ifdef JADEFUSION
	Addinf = (MAD_FaceAdditionnalInfo*)malloc(sizeof(MAD_FaceAdditionnalInfo) * NumberOfFace);
#else
	Addinf = malloc(sizeof(MAD_FaceAdditionnalInfo) * NumberOfFace);
#endif
	/* Destroy Bad links */
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
#define NO_TOPOLOGIC_PROPAG_THRESH 0.8f
        MAD_GetFace(MO, FaceCounter, &Current, &Nothing, &Material1);
		NORMALE_TRIANGLE((MO->OBJ_PointList + Current->Index[0]),(MO->OBJ_PointList + Current->Index[1]),(MO->OBJ_PointList + Current->Index[2]),(&stCrossproduct1),Point.x,Point.y,Point.z);
		NORMALIZE((&stCrossproduct1),Point.x,Point.y,Point.z);
        if(Current->Undefined1 != MAD_NULL_INDEX)
        {
            MAD_GetFace(MO, Current->Undefined1, &Current2, &Nothing, &Material2);
            if(Material2 != Material1)
                Current->Undefined1 = MAD_NULL_INDEX;
			else 
			if (!FromJade) {
				NORMALE_TRIANGLE((MO->OBJ_PointList + Current2->Index[0]),(MO->OBJ_PointList + Current2->Index[1]),(MO->OBJ_PointList + Current2->Index[2]),(&stCrossproduct2),Point.x,Point.y,Point.z);
				NORMALIZE((&stCrossproduct2),Point.x,Point.y,Point.z);
				if (DOT_PRODUCT((&stCrossproduct1),(&stCrossproduct2),Point.x,Point.y,Point.z) < NO_TOPOLOGIC_PROPAG_THRESH)
					Current->Undefined1 = MAD_NULL_INDEX;
			}
        }
		
        if(Current->Undefined2 != MAD_NULL_INDEX)
        {
            MAD_GetFace(MO, Current->Undefined2, &Current2, &Nothing, &Material2);
            if(Material2 != Material1)
                Current->Undefined2 = MAD_NULL_INDEX;
			else 
			if (!FromJade) {
				NORMALE_TRIANGLE((MO->OBJ_PointList + Current2->Index[0]),(MO->OBJ_PointList + Current2->Index[1]),(MO->OBJ_PointList + Current2->Index[2]),(&stCrossproduct2),Point.x,Point.y,Point.z);
				NORMALIZE((&stCrossproduct2),Point.x,Point.y,Point.z);
				if (DOT_PRODUCT((&stCrossproduct1),(&stCrossproduct2),Point.x,Point.y,Point.z) < NO_TOPOLOGIC_PROPAG_THRESH)
					Current->Undefined2 = MAD_NULL_INDEX;
			}
        }
		
        if(Current->Undefined3 != MAD_NULL_INDEX)
        {
            MAD_GetFace(MO, Current->Undefined3, &Current2, &Nothing, &Material2);
            if(Material2 != Material1)
                Current->Undefined3 = MAD_NULL_INDEX;
			else 
			if (!FromJade) {
				NORMALE_TRIANGLE((MO->OBJ_PointList + Current2->Index[0]),(MO->OBJ_PointList + Current2->Index[1]),(MO->OBJ_PointList + Current2->Index[2]),(&stCrossproduct2),Point.x,Point.y,Point.z);
				NORMALIZE((&stCrossproduct2),Point.x,Point.y,Point.z);
				if (DOT_PRODUCT((&stCrossproduct1),(&stCrossproduct2),Point.x,Point.y,Point.z) < NO_TOPOLOGIC_PROPAG_THRESH)
					Current->Undefined3 = MAD_NULL_INDEX;
			}
        }
    }
	
    /* ini ADD INF */
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &Current, &Nothing, &Material1);
        Addinf[FaceCounter].Original = *Current;
        Addinf[FaceCounter].OriginalMaterialID = Material1;
        Addinf[FaceCounter].Compatibilitys[0] = Current->Undefined1;
        Addinf[FaceCounter].Compatibilitys[1] = Current->Undefined2;
        Addinf[FaceCounter].Compatibilitys[2] = Current->Undefined3;
		Addinf[FaceCounter].UV[0] = MO->OBJ_UVtextureList[Current->UVIndex[0]];
		Addinf[FaceCounter].UV[1] = MO->OBJ_UVtextureList[Current->UVIndex[1]];
		Addinf[FaceCounter].UV[2] = MO->OBJ_UVtextureList[Current->UVIndex[2]];
        Addinf[FaceCounter].Channel = MAD_NULL_INDEX;
        Addinf[FaceCounter].UBase = 0.0f;
        Addinf[FaceCounter].VBase = 0.0f;
		Addinf[FaceCounter].ulUVBaseIsComputed = 0;
    }
    /* Compute new number of element */
	/* propage les compatibilitees */
	/* BEGIN TOUT EST LÀ */
    NewNumberOfElement = 0;
	MAD_COUVPVS_TAG = 0;
	memset(WorkingUV, 0 , sizeof(MAD_Vertex) * MO->NumberOfPoints);
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        if(Addinf[FaceCounter].Channel == MAD_NULL_INDEX)
        {
			MAD_COUVPVS_TAG ++;
            Addinf[FaceCounter].Channel = NewNumberOfElement++;
			MAD_ulCouldBeInsert(FaceCounter);
            MAD_Propagation(FaceCounter);
        }
    }

    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        if(Addinf[FaceCounter].Channel == MAD_NULL_INDEX)
        {
			Addinf[FaceCounter].Channel = Addinf[FaceCounter].Channel;
        } 
		if(!Addinf[FaceCounter].ulUVBaseIsComputed)
		{
			Addinf[FaceCounter].Channel = Addinf[FaceCounter].Channel;
		}
    }
	/* END TOUT EST LÀ */

	/* -------------------------------------*/
	/* Begin realloque les nouveau elements */
	/*	calcule d'apres les compatibilitees */
	/* -------------------------------------*/
    MAD_REALLOC(MAD_GeometricObjectElement *, MO->Elements, NewNumberOfElement);
    while(MO->NumberOfElements < NewNumberOfElement)
    {
        MAD_MALLOC(MAD_GeometricObjectElement, MO->Elements[MO->NumberOfElements], 1);
        MO->Elements[MO->NumberOfElements]->NumberOfTriangles = 1;  /* Abitrary number of faces */
        MAD_MALLOC(MAD_Face,MO->Elements[MO->NumberOfElements]->Faces,MO->Elements[MO->NumberOfElements]->NumberOfTriangles);
        MO->NumberOfElements++;
	}
	
    /* Gounize */
    for(FaceCounter = 0; FaceCounter < MO->NumberOfElements; FaceCounter++)
    {
        MO->Elements[FaceCounter]->NumberOfTriangles = 0;
    }
	
    /* Regounize */
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MO->Elements[Addinf[FaceCounter].Channel]->NumberOfTriangles++;
    }
	
    /* Re-regounize */
    for(FaceCounter = 0; FaceCounter < MO->NumberOfElements; FaceCounter++)
    {
        MAD_REALLOC(MAD_Face,MO->Elements[FaceCounter]->Faces,MO->Elements[FaceCounter]->NumberOfTriangles);
        MO->Elements[FaceCounter]->NumberOfTriangles = 0;
    }
	
    /* Re-re-regounize */
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MO->Elements[Addinf[FaceCounter].Channel]->Faces[MO->Elements[Addinf[FaceCounter].Channel]-> NumberOfTriangles] = Addinf[FaceCounter].Original;
        MO->Elements[Addinf[FaceCounter].Channel]->MaterialID = Addinf[FaceCounter].OriginalMaterialID;
        MO->Elements[Addinf[FaceCounter].Channel]->NumberOfTriangles++;
    }
	/* -------------------------------------*/
	/* end   realloque les nouveau elements */
	/* -------------------------------------*/
	
	/* -------------------------------------*/
	/* begin realloque les nouveau UV		*/
	/* -------------------------------------*/

    MAD_ComputeUsedIndexPerElements(MO);

    /* Save Current UVList */
    SavedUV = (MAD_Vertex*)malloc(sizeof(MAD_Vertex) * MO->NumberOfUV);
    memcpy(SavedUV, MO->OBJ_UVtextureList, MO->NumberOfUV * sizeof(MAD_Vertex));
    Convert = (unsigned long*)malloc(4L * MO->NumberOfPoints);
	
    /* Recompute new number of uv */
    Nothing = 0;
    for(FaceCounter = 0; FaceCounter < MO->NumberOfElements; FaceCounter++)
    {
        Nothing += MO->Elements[FaceCounter]->NumberOfUsedIndex;
    }
	
    MO->NumberOfUV = Nothing;
    MAD_REALLOC(MAD_Vertex, MO->OBJ_UVtextureList, MO->NumberOfUV);

    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        Addinf[FaceCounter].UV[0].x += Addinf[FaceCounter].UBase;
        Addinf[FaceCounter].UV[0].y += Addinf[FaceCounter].VBase;
        Addinf[FaceCounter].UV[1].x += Addinf[FaceCounter].UBase;
        Addinf[FaceCounter].UV[1].y += Addinf[FaceCounter].VBase;
        Addinf[FaceCounter].UV[2].x += Addinf[FaceCounter].UBase;
        Addinf[FaceCounter].UV[2].y += Addinf[FaceCounter].VBase;
    }
	
    ACounter = 0;
    for(ECounter = 0; ECounter < MO->NumberOfElements; ECounter++)
    {
        MO->Elements[ECounter]->OneUVPerVertexPerElementBase = ACounter;
		/* Decompress in Working UV */
        for(FaceCounter = 0; FaceCounter < NumberOfFace ; FaceCounter++)
        {
			if (Addinf[FaceCounter].Channel == ECounter)
			{
				WorkingUV[Addinf[FaceCounter].Original.Index[0]] = Addinf[FaceCounter].UV[0];
				WorkingUV[Addinf[FaceCounter].Original.Index[1]] = Addinf[FaceCounter].UV[1];
				WorkingUV[Addinf[FaceCounter].Original.Index[2]] = Addinf[FaceCounter].UV[2];
			}
        }
		/* Compress */
        for(FaceCounter = 0; FaceCounter < MO->Elements[ECounter]->NumberOfUsedIndex; FaceCounter++)
        {
            MO->OBJ_UVtextureList[ACounter] = WorkingUV[MO->Elements[ECounter]->UsedIndex[FaceCounter]];
            Convert[MO->Elements[ECounter]->UsedIndex[FaceCounter]] = ACounter;
            ACounter++;
        }
		/* recompute new uv index */
        for(FaceCounter = 0; FaceCounter < MO->Elements[ECounter]->NumberOfTriangles; FaceCounter++)
        {
            MO->Elements[ECounter]->Faces[FaceCounter].UVIndex[0] = Convert[MO->Elements[ECounter]->Faces[FaceCounter].Index[0]];
            MO->Elements[ECounter]->Faces[FaceCounter].UVIndex[1] = Convert[MO->Elements[ECounter]->Faces[FaceCounter].Index[1]];
            MO->Elements[ECounter]->Faces[FaceCounter].UVIndex[2] = Convert[MO->Elements[ECounter]->Faces[FaceCounter].Index[2]];
        }
        /* If (ECounter == 0) */
    }
	/* -------------------------------------*/
	/* end	realloque les nouveau UV		*/
	/* -------------------------------------*/

	
    free(Convert);
    free(WorkingUV);
    free(SavedUV);
    free(Addinf);
}

/* -----------------------------------------------------------------------------------------------
this function will restore One UV PerVertex solotuion after the subdivision
!! This will only work if you hte objet is compatible with MAD_ComputeOneIndexPerUVPerElementSolution
  -----------------------------------------------------------------------------------------------*/
void MAD_RestoreOneIndexPerUVPerElementSolution(MAD_GeometricObject *MO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   FaceCounter, ECounter, ACounter;
    MAD_Vertex      *SavedUV ;
    unsigned long   *Convert;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    if(!MO->OBJ_UVtextureList) return;

    WorkingUV = (MAD_Vertex*)malloc(sizeof(MAD_Vertex) * MO->NumberOfPoints);
    SavedUV = (MAD_Vertex*)malloc(sizeof(MAD_Vertex) * MO->NumberOfUV);
    memcpy(SavedUV, MO->OBJ_UVtextureList, MO->NumberOfUV * sizeof(MAD_Vertex));
	
    MAD_ComputeUsedIndexPerElements(MO);

    /* Recompute new number of uv */
    ACounter = 0;
    for(ECounter = 0; ECounter < MO->NumberOfElements; ECounter++)
    {
		ACounter += MO->Elements[ECounter]->NumberOfUsedIndex;
    }
	MO->NumberOfUV = ACounter;
	MAD_REALLOC(MAD_Vertex, MO->OBJ_UVtextureList, MO->NumberOfUV);
	Convert = (unsigned long*)malloc(4L * MO->NumberOfPoints);
	ACounter = 0;
    for(ECounter = 0; ECounter < MO->NumberOfElements; ECounter++)
    {
		MO->Elements[ECounter]->OneUVPerVertexPerElementBase = ACounter;
		for (FaceCounter = 0 ; FaceCounter < MO->Elements[ECounter]->NumberOfTriangles ; FaceCounter++)
		{
			WorkingUV[MO->Elements[ECounter]->Faces[FaceCounter].Index[0]] = SavedUV[MO->Elements[ECounter]->Faces[FaceCounter].UVIndex[0]];
			WorkingUV[MO->Elements[ECounter]->Faces[FaceCounter].Index[1]] = SavedUV[MO->Elements[ECounter]->Faces[FaceCounter].UVIndex[1]];
			WorkingUV[MO->Elements[ECounter]->Faces[FaceCounter].Index[2]] = SavedUV[MO->Elements[ECounter]->Faces[FaceCounter].UVIndex[2]];
		}
        for (FaceCounter = 0 ; FaceCounter < MO->Elements[ECounter]->NumberOfUsedIndex ; FaceCounter++)
        {
            MO->OBJ_UVtextureList[ACounter] = WorkingUV[MO->Elements[ECounter]->UsedIndex[FaceCounter]];
            Convert[MO->Elements[ECounter]->UsedIndex[FaceCounter]] = ACounter;
            ACounter++;
        }
        for(FaceCounter = 0; FaceCounter < MO->Elements[ECounter]->NumberOfTriangles; FaceCounter++)
        {
            MO->Elements[ECounter]->Faces[FaceCounter].UVIndex[0] = Convert[MO->Elements[ECounter]->Faces[FaceCounter].Index[0]];
            MO->Elements[ECounter]->Faces[FaceCounter].UVIndex[1] = Convert[MO->Elements[ECounter]->Faces[FaceCounter].Index[1]];
            MO->Elements[ECounter]->Faces[FaceCounter].UVIndex[2] = Convert[MO->Elements[ECounter]->Faces[FaceCounter].Index[2]];
        }
    }

	/* -------------------------------------*/
	/* end	realloque les nouveau UV		*/
	/* -------------------------------------*/

	
    free(Convert);
    free(WorkingUV);
    free(SavedUV);
}

/* -----------------------------------------------------------------------------------------------
  -----------------------------------------------------------------------------------------------*/
void MAD_MergeElements(MAD_GeometricObject *MO,unsigned long E1,unsigned long E2)
{
	unsigned long OlnNumTri;
	if (E1 == E2) return; 
	if (MO ->Elements[E1]->NumberOfTriangles == 0) return;
	if (MO ->Elements[E2]->NumberOfTriangles == 0) return;
	OlnNumTri = MO ->Elements[E1]->NumberOfTriangles ;
	MO ->Elements[E1]->NumberOfTriangles += MO ->Elements[E2]->NumberOfTriangles;
	MAD_REALLOC(MAD_Face, MO ->Elements[E1]-> Faces , MO ->Elements[E1]->NumberOfTriangles );
	memcpy(MO ->Elements[E1]-> Faces + OlnNumTri , MO ->Elements[E2]->Faces , sizeof(MAD_Face) * MO ->Elements[E2]->NumberOfTriangles);
	MO ->Elements[E2]-> NumberOfTriangles = 0;
	MO ->Elements[E2]-> Faces = NULL;
	/* merge used indexes */
	if ((MO ->Elements[E1]-> UsedIndex != NULL) && (MO ->Elements[E2]-> UsedIndex != NULL))
	{
		unsigned long *Ptr1,*Ptr2,*ResultPtr,*LastResultPtr;
		unsigned long *LastPtr1,*LastPtr2;
		Ptr1 = MO ->Elements[E1]-> UsedIndex ;
		Ptr2 = MO ->Elements[E2]-> UsedIndex ;
		LastPtr1 = Ptr1 + MO ->Elements[E1]->NumberOfUsedIndex;
		LastPtr2 = Ptr2 + MO ->Elements[E2]->NumberOfUsedIndex;
		MO ->Elements[E1]->NumberOfUsedIndex += MO ->Elements[E2]->NumberOfUsedIndex;
		MAD_MALLOC(unsigned long, MO->Elements[E1]->UsedIndex, MO ->Elements[E1]->NumberOfUsedIndex);
		ResultPtr		= MO ->Elements[E1]-> UsedIndex ;
		LastResultPtr	= ResultPtr + MO ->Elements[E1]->NumberOfUsedIndex;
		while (ResultPtr < LastResultPtr)
		{
#ifdef JADEFUSION
			if (Ptr1 != LastPtr1)
				while (((*Ptr1 <= *Ptr2) || (Ptr2 == LastPtr2)) && (Ptr1 != LastPtr1)) *(ResultPtr++) = *(Ptr1++);
			if (Ptr2 != LastPtr2)
				while (((*Ptr2 <= *Ptr1) || (Ptr1 == LastPtr1)) && (Ptr2 != LastPtr2)) *(ResultPtr++) = *(Ptr2++);
#else
			if (Ptr1 != LastPtr1)
				while ((*Ptr1 <= *Ptr2) && (Ptr1 != LastPtr1)) *(ResultPtr++) = *(Ptr1++);
			if (Ptr2 != LastPtr2)
				while ((*Ptr2 <= *Ptr1) && (Ptr2 != LastPtr2)) *(ResultPtr++) = *(Ptr2++);
#endif
		}
	}
}

void MAD_ReComputeElements(MAD_GeometricObject *MO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter1,Counter2;
	MAD_GeometricObjectElement **pst_ElemntCrt,**pst_Elemnt,**pst_ElemntLST;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* Recompute new number of uv */
	for (Counter1 = 0 ; Counter1 < MO->NumberOfElements - 1; Counter1 ++)
	{
		for (Counter2 = Counter1 + 1; Counter2 < MO->NumberOfElements ; Counter2 ++)
		{
			if (MO ->Elements[Counter1]->MaterialID == MO ->Elements[Counter2]->MaterialID)
			{
				MAD_MergeElements(MO,Counter1,Counter2);
			}
		}
	}
	pst_ElemntCrt = pst_Elemnt = MO ->Elements;
	pst_ElemntLST = &MO ->Elements[MO->NumberOfElements];
	MO->NumberOfElements = 0;
	while (pst_Elemnt < pst_ElemntLST)
	{
		if ((*pst_Elemnt) ->NumberOfTriangles)
		{
			MO->NumberOfElements ++;
			*pst_ElemntCrt++ = *pst_Elemnt;
		}
		pst_Elemnt++;
	}
}


/* this function will be used after compu1UVperpoint solution for make as lowest elements as possible */
/* this must be call btween COmpute & REstore one uv per vertex solution */
void MAD_Optimize2Elements(MAD_GeometricObject *MO,unsigned long E1,unsigned long E2)
{
	unsigned long *Ptr1,*Ptr2;
	unsigned long *LastPtr1,*LastPtr2;
	if (E1 == E2) return; 
	if (MO ->Elements[E1]->NumberOfTriangles == 0) return;
	if (MO ->Elements[E2]->NumberOfTriangles == 0) return;
	Ptr1 = MO ->Elements[E1]-> UsedIndex ;
	Ptr2 = MO ->Elements[E2]-> UsedIndex ;
	LastPtr1 = Ptr1 + MO ->Elements[E1]->NumberOfUsedIndex;
	LastPtr2 = Ptr2 + MO ->Elements[E2]->NumberOfUsedIndex;
	/* Could they be merged? That mean there is no common used index */
	while (1)
	{
		while ((*Ptr1 < *Ptr2) && (Ptr1 != LastPtr1)) Ptr1++;
		if (Ptr1 == LastPtr1) break;
		if (*Ptr1 == *Ptr2) return;
		while ((*Ptr2 < *Ptr1) && (Ptr2 != LastPtr2)) Ptr2++;
		if (Ptr2 == LastPtr2) break;
		if (*Ptr1 == *Ptr2) return;
	}
	MAD_MergeElements(MO,E1,E2);
}
void MAD_OptimizeElements(MAD_GeometricObject *MO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long   Counter1,Counter2;
	MAD_GeometricObjectElement **pst_ElemntCrt,**pst_Elemnt,**pst_ElemntLST;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAD_ComputeUsedIndexPerElements(MO);
    /* Recompute new number of uv */
	for (Counter1 = 0 ; Counter1 < MO->NumberOfElements - 1; Counter1 ++)
	{
		for (Counter2 = Counter1 + 1; Counter2 < MO->NumberOfElements ; Counter2 ++)
		{
			if (MO ->Elements[Counter1]->MaterialID == MO ->Elements[Counter2]->MaterialID)
			{
				MAD_Optimize2Elements(MO,Counter1,Counter2);
			}
		}
	}
	pst_ElemntCrt = pst_Elemnt = MO ->Elements;
	pst_ElemntLST = &MO ->Elements[MO->NumberOfElements];
	MO->NumberOfElements = 0;
	while (pst_Elemnt < pst_ElemntLST)
	{
		if ((*pst_Elemnt) ->NumberOfTriangles)
		{
			MO->NumberOfElements ++;
			*pst_ElemntCrt++ = *pst_Elemnt;
		}
		pst_Elemnt++;
	}
}
/*
===================================================================================================
===================================================================================================
*/
void MAD_RemoveIsoPoints(MAD_GeometricObject *MO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    /* Unsigned long *NewPlaces; */
    unsigned long   Counter, ECounter, MID, Nothing, NumberOfFace;
    unsigned long   *TAGODROME;
    MAD_Face        *CurrentFace;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    TAGODROME = (unsigned long*)malloc(4L * MO->NumberOfPoints);
    memset(TAGODROME, 0, 4L * MO->NumberOfPoints);
	
    /* NewPlaces = malloc(4L * MO -> NumberOfPoints); */
    NumberOfFace = MAD_GetNumberOfFace(MO);
    for(Counter = 0; Counter < NumberOfFace; Counter++)
    {
        MAD_GetFace(MO, Counter, &CurrentFace, &MID, &Nothing);
		
        /* Reset flags */
        TAGODROME[CurrentFace->Index[0]] = 1;
        TAGODROME[CurrentFace->Index[1]] = 1;
        TAGODROME[CurrentFace->Index[2]] = 1;
    }
	
    ECounter = 0;
    for(Counter = 0; Counter < MO->NumberOfPoints; Counter++)
    {
        if(TAGODROME[Counter])
        {
            MO->OBJ_PointList[ECounter] = MO->OBJ_PointList[Counter];
            MO->Colors[ECounter] = MO->Colors[Counter];
            TAGODROME[Counter] = ECounter++;
        }
    }
	
    MO->NumberOfPoints = ECounter;
    for(Counter = 0; Counter < NumberOfFace; Counter++)
    {
        MAD_GetFace(MO, Counter, &CurrentFace, &MID, &Nothing);
		
        /* Reset flags */
        CurrentFace->Index[0] = TAGODROME[CurrentFace->Index[0]];
        CurrentFace->Index[1] = TAGODROME[CurrentFace->Index[1]];
        CurrentFace->Index[2] = TAGODROME[CurrentFace->Index[2]];
    }
	
    for(ECounter = 0; ECounter < MO->NumberOfElements; ECounter++)
    {
        if(MO->Elements[ECounter]->UsedIndex)
        {
            for(Counter = 0; Counter < MO->Elements[ECounter]->NumberOfUsedIndex; Counter++)
            {
                MO->Elements[ECounter]->UsedIndex[Counter] = TAGODROME[MO->Elements[ECounter]->
                    UsedIndex[Counter]];
            }
        }
    }
	
    free(TAGODROME);
}

typedef struct  MAD_Vertex2Vertex_
{
    unsigned long   Index;
    unsigned long   Counter;
} MAD_Vertex2Vertex;

/*
===================================================================================================
===================================================================================================
*/
unsigned long MAD_IncludeV2V(MAD_Vertex2Vertex *First, unsigned long Index1, unsigned long Index2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_Vertex2Vertex   *Local;
#ifndef JADEFUSION
	unsigned long	ReturnVal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ReturnVal = 0;
#endif
	Local = First;
    while((Local->Counter != 0) && (Local->Index != Index1))
    {
        Local++;
    }
	
    Local->Index = Index1;
    Local->Counter++;
#ifdef JADEFUSION
	if(Local->Counter > 2) return 1;
#else
	if(Local->Counter > 2) ReturnVal = 1;
#endif
	Local = First;
    while((Local->Counter != 0) && (Local->Index != Index2))
    {
        Local++;
    }
	 
    Local->Index = Index2;
    Local->Counter++;
 
#ifdef JADEFUSION
    if(Local->Counter > 2) return 1;
    return 0;
#else
	if(Local->Counter > 2) ReturnVal = 1;
    return ReturnVal;
#endif
}

/*
===================================================================================================
===================================================================================================
*/
unsigned long MAD_IsMagnifold(MAD_GeometricObject *MO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long       FaceCounter, NumberOfFace, MID, Local, Nothing;
    MAD_Face            *CurrentFace;
    unsigned long       *Vertext2Vertex;
    MAD_Vertex2Vertex   *V2VCounter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    MAD_RemoveIllgalFaces(MO);
    NumberOfFace = MAD_GetNumberOfFace(MO);
    Vertext2Vertex = (unsigned long *) malloc(MO->NumberOfPoints * 4L);
    memset(Vertext2Vertex, 0, MO->NumberOfPoints * 4L);
	
    /* Count number of triangles per vertex */
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        Vertext2Vertex[CurrentFace->Index[0]] += 2;
        Vertext2Vertex[CurrentFace->Index[1]] += 2;
        Vertext2Vertex[CurrentFace->Index[2]] += 2;
    }
	
    /* Count number of triangles per vertex */
    Local = 0;
    for(FaceCounter = 0; FaceCounter < MO->NumberOfPoints; FaceCounter++)
    {
        Local += Vertext2Vertex[FaceCounter];
        Vertext2Vertex[FaceCounter] = Local - Vertext2Vertex[FaceCounter];
    }
	
    V2VCounter = (MAD_Vertex2Vertex *) malloc(Local * sizeof(MAD_Vertex2Vertex));
    memset(V2VCounter, 0, (Local * sizeof(MAD_Vertex2Vertex)));
	
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        if
			(
            MAD_IncludeV2V
            (
			V2VCounter + Vertext2Vertex[CurrentFace->Index[0]],
			CurrentFace->Index[1],
			CurrentFace->Index[2]
            )
			) goto END2;
        if
			(
            MAD_IncludeV2V
            (
			V2VCounter + Vertext2Vertex[CurrentFace->Index[1]],
			CurrentFace->Index[2],
			CurrentFace->Index[0]
            )
			) goto END2;
        if
			(
            MAD_IncludeV2V
            (
			V2VCounter + Vertext2Vertex[CurrentFace->Index[2]],
			CurrentFace->Index[0],
			CurrentFace->Index[1]
            )
			) goto END2;
    }
	
    free(Vertext2Vertex);
    free(V2VCounter);
	
    /* Compute Number of Vertex2Vertex */
    return 1;
END2:
    free(Vertext2Vertex);
    free(V2VCounter);
	
    /* Compute Number of Vertex2Vertex */
    return 0;
}


/*
===================================================================================================
MAD_MakeItMagnifold : this fonction is used to solv the problem of computing strips.
After strip computing, you may have to clen the object (re-merge equals points) with 
following restore magnifold func.
===================================================================================================
*/
unsigned long *MAD_MakeItMagnifold(MAD_GeometricObject *MO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long       FaceCounter, NumberOfFace, MID, Local, Nothing;
    MAD_Face            *CurrentFace;
    unsigned long       *Vertext2VertexNumbers,*Vertext2VertexBases;
	unsigned long		*p_GetFromList;
    MAD_Vertex2Vertex   *V2VCounter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    NumberOfFace = MAD_GetNumberOfFace(MO);
    Vertext2VertexNumbers = (unsigned long *) malloc(MO->NumberOfPoints * 4L);
    Vertext2VertexBases = (unsigned long *) malloc(MO->NumberOfPoints * 4L);
    memset(Vertext2VertexNumbers, 0, MO->NumberOfPoints * 4L);
    memset(Vertext2VertexBases, 0, MO->NumberOfPoints * 4L);
	
    /* Count number of triangles per vertex */
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        Vertext2VertexNumbers[CurrentFace->Index[0]] += 2; /* add two new links */
        Vertext2VertexNumbers[CurrentFace->Index[1]] += 2;
        Vertext2VertexNumbers[CurrentFace->Index[2]] += 2;
    }
	/* OK */
    Local = 0;
    for(FaceCounter = 0; FaceCounter < MO->NumberOfPoints; FaceCounter++)
    {
        Vertext2VertexBases[FaceCounter] = Local;
        Local += Vertext2VertexNumbers[FaceCounter];
    }
	/* OK */
	
    V2VCounter = (MAD_Vertex2Vertex *) malloc(Local * sizeof(MAD_Vertex2Vertex));
    memset(V2VCounter, 0, (Local * sizeof(MAD_Vertex2Vertex)));
	/* Count Number of Edge per points */
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        MAD_IncludeV2V(V2VCounter + Vertext2VertexBases[CurrentFace->Index[0]],CurrentFace->Index[1],CurrentFace->Index[2]);
        MAD_IncludeV2V(V2VCounter + Vertext2VertexBases[CurrentFace->Index[1]],CurrentFace->Index[2],CurrentFace->Index[0]);
        MAD_IncludeV2V(V2VCounter + Vertext2VertexBases[CurrentFace->Index[2]],CurrentFace->Index[0],CurrentFace->Index[1]);
    }
	/* Mark bad points  */

    {
		unsigned long EdgeCounter;
		Local = 0;
		for(FaceCounter = 0; FaceCounter < MO->NumberOfPoints; FaceCounter++)
		{
			EdgeCounter = Vertext2VertexNumbers[FaceCounter];
			Vertext2VertexNumbers[FaceCounter] = 0;
			while (EdgeCounter--)
			{
				if (V2VCounter[Vertext2VertexBases[FaceCounter] + EdgeCounter].Counter > 2)
				{
					Vertext2VertexNumbers[FaceCounter] = 1;
				}
			}
		}
	}
	/* Count number of new points */
 	Local = MO->NumberOfPoints;
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
		if (Vertext2VertexNumbers[CurrentFace->Index[0]])	Local ++;
		if (Vertext2VertexNumbers[CurrentFace->Index[1]])	Local ++;
		if (Vertext2VertexNumbers[CurrentFace->Index[2]])	Local ++;
    }
    MAD_REALLOC(MAD_Simple3DVertex	, MO->OBJ_PointList		, Local);
	//vincent modif
	if (MO->Colors)
		MAD_REALLOC(MAD_ColorARGB, MO->Colors, Local );
	if (MO->SecondRLIField)
		MAD_REALLOC(MAD_ColorARGB, MO->SecondRLIField, Local );
	//vincent fin modif
	
	p_GetFromList = (unsigned long*)malloc(4L * Local);
	memset(p_GetFromList , 0xff , 4L * Local);

 	Local = MO->NumberOfPoints;
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
		if (Vertext2VertexNumbers[CurrentFace->Index[0]])
		{
			MO->OBJ_PointList[Local] = MO->OBJ_PointList[CurrentFace->Index[0]];
			
			//vincent modif
			if (MO->Colors)
				MO->Colors[Local] = MO->Colors[CurrentFace->Index[0]];
			if (MO->SecondRLIField)
				MO->SecondRLIField[Local] = MO->SecondRLIField[CurrentFace->Index[0]];
			//vincent fin modif

			p_GetFromList[Local] = CurrentFace->Index[0];
			CurrentFace->Index[0] = Local++;
		}
		if (Vertext2VertexNumbers[CurrentFace->Index[1]])
		{
			MO->OBJ_PointList[Local] = MO->OBJ_PointList[CurrentFace->Index[1]];
			//vincent modif
			if (MO->Colors)
				MO->Colors[Local] = MO->Colors[CurrentFace->Index[1]];
			if (MO->SecondRLIField)
				MO->SecondRLIField[Local] = MO->SecondRLIField[CurrentFace->Index[1]];
			//vincent fin modif
			p_GetFromList[Local] = CurrentFace->Index[1];
			CurrentFace->Index[1] = Local++;
		}
		if (Vertext2VertexNumbers[CurrentFace->Index[2]])
		{
			MO->OBJ_PointList[Local] = MO->OBJ_PointList[CurrentFace->Index[2]];
			//vincent modif
			if (MO->Colors)
				MO->Colors[Local] = MO->Colors[CurrentFace->Index[2]];
			if (MO->SecondRLIField)
				MO->SecondRLIField[Local] = MO->SecondRLIField[CurrentFace->Index[2]];
			//vincent fin modif
			p_GetFromList[Local] = CurrentFace->Index[2];
			CurrentFace->Index[2] = Local++;
		}
    }
	MO->NumberOfPoints = Local;

    free(Vertext2VertexNumbers);
    free(Vertext2VertexBases);
    free(V2VCounter);
	return p_GetFromList;
}

void MAD_RestoreNonMagnifold(MAD_GeometricObject *MO , unsigned long *pGetFroms)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned long       FaceCounter, NumberOfFace, MID, MaxiV , Nothing;
    MAD_Face            *CurrentFace;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (!pGetFroms) return;
	
    NumberOfFace = MAD_GetNumberOfFace(MO);
	
    /* Retreive real number of points  */
	MaxiV = 0;
	for(FaceCounter = 0; FaceCounter < MO->NumberOfPoints; FaceCounter++)
	{
		if (pGetFroms[FaceCounter] == 0xffffffff) pGetFroms[FaceCounter] = FaceCounter;
		if (MaxiV < pGetFroms[FaceCounter]) MaxiV = pGetFroms[FaceCounter];
	}
	MO->NumberOfPoints = MaxiV +1;
    for(FaceCounter = 0; FaceCounter < NumberOfFace; FaceCounter++)
    {
        MAD_GetFace(MO, FaceCounter, &CurrentFace, &MID, &Nothing);
        CurrentFace->Index[0] = pGetFroms[CurrentFace->Index[0]]; 
        CurrentFace->Index[1] = pGetFroms[CurrentFace->Index[1]]; 
        CurrentFace->Index[2] = pGetFroms[CurrentFace->Index[2]]; 
    }
	free(pGetFroms);
}
