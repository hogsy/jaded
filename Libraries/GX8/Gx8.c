/**
 * Gx8.c
 */

#include "Gx8.h"
#include "BASe/MEMory/MEMpro.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "STRing/STRstruct.h"

// -- for normalmap --
#include <xtl.h>
#include "GDInterface/GDInterface.h"
#include <D3D8.h>
#include "Gx8buffer.h"
#include <Gx8/Gx8Init.h>
#include "Gx8VertexBuffer.h"
#include "OBJects/OBJaccess.h"
#include "GEOmetric/GEOobject.h"
extern Gx8_tdst_SpecificData	* p_gGx8SpecificData;
extern BOOL Normalmap;

#define NORMAL
// ---
//#define USE_TANGENTE
Gx8_BigStruct gs_st_Globals;

void
Gx8_FirstInit( void )
{
    MEMpro_StartMemRaster();
    L_memset( &gs_st_Globals, 0, sizeof( gs_st_Globals ));
    
	// PIXEL ASPECT RATIO = 11:10
    Gx8Global_set( f2DFFX_A2D, 1.0f );
    Gx8Global_set( f2DFFY_A2D, 1.1f );
    
    STR_f_YFactor = 1.0f/(1.0f + (Gx8Global_get(f2DFFY_A2D) - 1.0f) / 1.0f);
    
    Gx8Global_set( f2DFFX_B2D, 1.000f );
    Gx8Global_set( f2DFFY_B2D, 1.000f );
    Gx8Global_set( Xsize, 640 );
    Gx8Global_set( Ysize, 480 );

    
    MEMpro_StopMemRaster(MEMpro_Id_GSP);
}

#ifdef NORMAL

void Gx8_GetScalarPlane ( GEO_tdst_Object *pst_Object, GEO_tdst_IndexedTriangle   *pst_Triangle, float *p_3SCL , MATH_tdst_Vector *p_P)
{
	float K0 , K1;// ,Local;
	MATH_tdst_Vector A , B , C ;
	MATH_tdst_Vector L1 , L2 ;
	K0 = p_3SCL[1] - p_3SCL[0];
	K1 = p_3SCL[2] - p_3SCL[0];
	MATH_SubVector(&A , &pst_Object->dst_Point[pst_Triangle->auw_Index[1]], &pst_Object->dst_Point[pst_Triangle->auw_Index[0]]);
	MATH_SubVector(&B , &pst_Object->dst_Point[pst_Triangle->auw_Index[2]], &pst_Object->dst_Point[pst_Triangle->auw_Index[0]]);
	MATH_CrossProduct(&C , &A, &B);
	MATH_CrossProduct(&L1 , &B, &C);
	MATH_CrossProduct(&L2 , &A, &C);
	MATH_ScaleVector(&L1 , &L1 , 1.0f / MATH_f_DotProduct(&L1,&A));
	MATH_ScaleVector(&L2 , &L2 , 1.0f / MATH_f_DotProduct(&L2,&B));
	p_P-> x = L1.x * K0 + L2.x * K1;
	p_P-> y = L1.y * K0 + L2.y * K1;
	p_P-> z = L1.z * K0 + L2.z * K1;
}

#endif
void GX8_ComputeTangentes(GEO_tdst_Object	*pst_Object)
{
#ifdef NORMAL
	Gx8_tdst_SpecificData	* pSD;
	DWORD	FVF;
	UINT	stride;
//	void	* pData;

//	GEO_tdst_OneStrip			*pStrip, *pStripEnd;
//	USHORT						auw_Index, auw_UV;

//	MATH_tdst_Vector vDirVec_v2_to_v1;
//	MATH_tdst_Vector vDirVec_v3_to_v1;
//	float vDirVec_v2u_to_v1u;
//	float vDirVec_v2v_to_v1v;
//	float vDirVec_v3u_to_v1u;
//	float vDirVec_v3v_to_v1v;
	//MATH_tdst_Vector v1,v2,v3;
	//float v1u,v2u,v3u;
	//float v1v,v2v,v3v;
//	float fDenominator;
	int i=0;

	SOFT_tdst_AVertex *vTangent;
	SOFT_tdst_AVertex *vTangentp;

/*	MATH_tdst_Vector *vBiNormal;
	MATH_tdst_Vector *vBiNormalp;*/
//	MATH_tdst_Vector vNormal;

	GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
	GEO_tdst_IndexedTriangle            *pst_Triangle, *pst_LastTriangle;
    GEO_Vertex                          *pst_Point;
	GEO_Vertex                          *pst_Normal;
//    GEO_Vertex                          *pst_Pt[3];
//    GEO_Vertex                          *pst_Norm[3];
	D3DXVECTOR2							*pst_UV;
//	float								*pst_u[3];
//	float								*pst_v[3];
	//GEO_Vertex					        *_pst_Point;

if (!Normalmap)	return;

	GEO_UseNormals(pst_Object); // OK
	//	_pst_Point = &pst_Object->dst_Point;
	pSD = p_gGx8SpecificData;

	// alou de la memoire pour les tangentes
	pst_Object->CotangantesU = (SOFT_tdst_AVertex *) MEM_GEO_p_AllocAlign(sizeof(SOFT_tdst_AVertex) * pst_Object->l_NbPoints,16);
	// alou de la memoire pour les binormales
//	pst_Object->Binormales = (MATH_tdst_Vector *) MEM_GEO_p_AllocAlign(sizeof(MATH_tdst_Vector) * pst_Object->l_NbPoints,16);


    pst_Element = pst_Object->dst_Element;
    pst_LastElement = pst_Element + pst_Object->l_NbElements;
    
	pst_Point = pst_Object->dst_Point;
	pst_Normal = pst_Object->dst_PointNormal;
	pst_UV = (D3DXVECTOR2*)pst_Object->dst_UV;
	vTangent = pst_Object->CotangantesU;
	vTangentp = pst_Object->CotangantesU;
/*	vBiNormal = pst_Object->Binormales;
	vBiNormalp = pst_Object->Binormales;*/

//	pStrip = pst_Element->pst_StripData->pStripList;
//	pStripEnd = pStrip + pst_Element->pst_StripData->ulStripNumber;
/*
	for(; pStrip < pStripEnd; pStrip++)
	{

		for(i = 0; i < pStrip->ulVertexNumber; i++)
		{
			auw_Index = pStrip->pMinVertexDataList[i].auw_Index;
			auw_UV = pStrip->pMinVertexDataList[i].auw_UV;

			pst_Pt[0] = pst_Point[auw_Index];
			pst_Pt[1] = pst_Point[auw_Index+3];
			pst_Pt[2] = pst_Point[auw_Index+1];

			pst_u[0] = pst_UV[auw_Index].x;
			pst_v[0] = pst_UV[auw_Index].y;
			pst_u[1] = pst_UV[auw_Index+3].x;
			pst_v[1] = pst_UV[auw_Index+3].y;
			pst_u[2] = pst_UV[auw_Index+1].x;
			pst_v[2] = pst_UV[auw_Index+1].y;


	
*/

    for(; pst_Element < pst_LastElement; pst_Element++)
    {
        pst_Triangle = pst_Element->dst_Triangle;
        pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;

        for(; pst_Triangle < pst_LastTriangle; pst_Triangle++)
        {
			MATH_tdst_Vector RealBinormale,Binormale,vA,vB,Normale;
			float t3s[3];
			t3s[0] = *(float *)&(pst_UV + pst_Triangle->auw_UV[0])->x;
			t3s[1] = *(float *)&(pst_UV + pst_Triangle->auw_UV[1])->x;
			t3s[2] = *(float *)&(pst_UV + pst_Triangle->auw_UV[2])->x;
			vTangent=vTangentp + pst_Triangle->auw_Index[0];
			Gx8_GetScalarPlane ( pst_Object, pst_Triangle, t3s , (MATH_tdst_Vector*)vTangent);
			MATH_NormalizeVector((MATH_tdst_Vector*)vTangent,(MATH_tdst_Vector*)vTangent);

			t3s[0] = *(float *)&(pst_UV + pst_Triangle->auw_UV[0])->y;
			t3s[1] = *(float *)&(pst_UV + pst_Triangle->auw_UV[1])->y;
			t3s[2] = *(float *)&(pst_UV + pst_Triangle->auw_UV[2])->y;
			Gx8_GetScalarPlane ( pst_Object, pst_Triangle, t3s , &RealBinormale);
			MATH_NormalizeVector(&RealBinormale,&RealBinormale);

			MATH_SubVector(&vA,pst_Object->dst_Point + pst_Triangle->auw_Index[1] , pst_Object->dst_Point + pst_Triangle->auw_Index[0]);
			MATH_SubVector(&vB,pst_Object->dst_Point + pst_Triangle->auw_Index[2] , pst_Object->dst_Point + pst_Triangle->auw_Index[0]);
			MATH_CrossProduct(&Normale, &vA,&vB );
			MATH_CrossProduct(&Binormale , vTangent , &Normale );

			vTangent->w = -1.0f;
			if (MATH_f_DotProduct(&Binormale , &RealBinormale) < 0.0f)
				vTangent->w = 1.0f;

			vTangentp[pst_Triangle->auw_Index[1]] = *vTangent;
			vTangentp[pst_Triangle->auw_Index[2]] = *vTangent;

			//DWORD i;	
			//for (i=0;i<3;i++)
			//{
#if 0
				pst_Pt[0] = pst_Point + pst_Triangle->auw_Index[0];
				pst_Pt[1] = pst_Point + pst_Triangle->auw_Index[1];
				pst_Pt[2] = pst_Point + pst_Triangle->auw_Index[2];

				pst_Norm[0] = pst_Normal + pst_Triangle->auw_Index[0];
				pst_Norm[1] = pst_Normal + pst_Triangle->auw_Index[1];
				pst_Norm[2] = pst_Normal + pst_Triangle->auw_Index[2];

				pst_u[0] = &(pst_UV + pst_Triangle->auw_UV[0])->x;
				pst_v[0] = &(pst_UV + pst_Triangle->auw_UV[0])->y;
				pst_u[1] = &(pst_UV + pst_Triangle->auw_UV[1])->x;
				pst_v[1] = &(pst_UV + pst_Triangle->auw_UV[1])->y;
				pst_u[2] = &(pst_UV + pst_Triangle->auw_UV[2])->x;
				pst_v[2] = &(pst_UV + pst_Triangle->auw_UV[2])->y;


				// Create edge vectors from vertex 1 to vectors 2 and 3.
				D3DXVec3Subtract(&vDirVec_v2_to_v1, pst_Pt[1], pst_Pt[0]);
				D3DXVec3Subtract(&vDirVec_v3_to_v1, pst_Pt[2], pst_Pt[0]);

				// Create edge vectors from the texture coordinates of vertex 1 to vector 2.
				vDirVec_v2u_to_v1u = *pst_u[1] - *pst_u[0];//v2u - v1u;
				vDirVec_v2v_to_v1v = *pst_v[1] - *pst_v[0];//v2v - v1v;

				// Create edge vectors from the texture coordinates of vertex 1 to vector 3.
				vDirVec_v3u_to_v1u = *pst_u[2] - *pst_u[0];//v3u - v1u;
				vDirVec_v3v_to_v1v = *pst_v[2] - *pst_v[0];//v3v - v1v;

				fDenominator = vDirVec_v2u_to_v1u * vDirVec_v3v_to_v1v - 
							vDirVec_v3u_to_v1u * vDirVec_v2v_to_v1v;


				if( fDenominator < 0.0001f && fDenominator > -0.0001f )
				{
					// We're too close to zero and we're at risk of a divide-by-zero! 
					// Set the tangent matrix to the identity matrix and do nothing.

					vTangent = vTangent;

/*			
					x = 1.0f;
					vTangent->y = 0.0f;
					vTangent->z = 0.0f;
					vBiNormal.x = 0.0f;
					vBiNormal.y = 1.0f;
					vBiNormal.z = 0.0f;
					vNormal.x = 0.0f;
					vNormal.y = 0.0f;
					vNormal.z = 1.0f;*/
				}
				else
				{
					// Calculate and cache the reciprocal value
					float fScale2;
					float fScale1 = 1.0f / fDenominator;

					MATH_tdst_Vector T;
					MATH_tdst_Vector B;
					MATH_tdst_Vector N;
					MATH_tdst_Vector vTemp;

					T.x= (vDirVec_v3v_to_v1v * vDirVec_v2_to_v1.x - vDirVec_v2v_to_v1v * vDirVec_v3_to_v1.x) * fScale1;
					T.y= (vDirVec_v3v_to_v1v * vDirVec_v2_to_v1.y - vDirVec_v2v_to_v1v * vDirVec_v3_to_v1.y) * fScale1;
					T.z= (vDirVec_v3v_to_v1v * vDirVec_v2_to_v1.z - vDirVec_v2v_to_v1v * vDirVec_v3_to_v1.z) * fScale1;

					B.x= (-vDirVec_v3u_to_v1u * vDirVec_v2_to_v1.x + vDirVec_v2u_to_v1u * vDirVec_v3_to_v1.x) * fScale1;
					B.y= (-vDirVec_v3u_to_v1u * vDirVec_v2_to_v1.y + vDirVec_v2u_to_v1u * vDirVec_v3_to_v1.y) * fScale1;
					B.z= (-vDirVec_v3u_to_v1u * vDirVec_v2_to_v1.z + vDirVec_v2u_to_v1u * vDirVec_v3_to_v1.z) * fScale1;

					// The normal N is calculated as the cross product between T and B
					D3DXVec3Cross (&N, &T, &B);

					// Calculate and cache the reciprocal value
					fScale2 = 1.0f / ((T.x * B.y * N.z - T.z * B.y * N.x) + 
											(B.x * N.y * T.z - B.z * N.y * T.x) + 
											(N.x * T.y * B.z - N.z * T.y * B.x));

	/*				// Calculate and cache the reciprocal value
					fScale2 = 1.0f / ((T.x * B.z * N.y - T.y * B.z * N.x) + 
											(B.x * N.z * T.y - B.y * N.z * T.x) + 
											(N.x * T.z * B.y - N.y * T.z * B.x));*/


					//
					// Use the temporary T (Tangent), (B) Binormal, and N (Normal) vectors 
					// to calculate the inverse of the tangent matrix that they represent.
					// The inverse of the tangent matrix is what we want since we need that
					// to transform the light's vector into tangent-space.
					//

				
					vTangent=vTangentp + pst_Triangle->auw_Index[0];
					vBiNormal=vBiNormalp + pst_Triangle->auw_Index[0];

					D3DXVec3Cross( &vTemp, &B, &N );
					vTangent->x = vTemp.x * fScale2;
					D3DXVec3Cross( &vTemp, &N, &T );
					vTangent->y = -(vTemp.x * fScale2);
					D3DXVec3Cross( &vTemp, &T, &B );
					vTangent->z = vTemp.x * fScale2;
					D3DXVec3Normalize( (vTangent), (vTangent) );
		
					D3DXVec3Cross( vBiNormal , pst_Norm[0], vTangent );
			        D3DXVec3Normalize( vBiNormal, vBiNormal );//*/


					vTangent=vTangentp + pst_Triangle->auw_Index[1];
					vBiNormal=vBiNormalp + pst_Triangle->auw_Index[1];

					D3DXVec3Cross( &vTemp, &B, &N );
					vTangent->x = vTemp.x * fScale2;
					D3DXVec3Cross( &vTemp, &N, &T );
					vTangent->y = -(vTemp.x * fScale2);
					D3DXVec3Cross( &vTemp, &T, &B );
					vTangent->z = vTemp.x * fScale2;
					D3DXVec3Normalize( (vTangent), (vTangent) );

					D3DXVec3Cross( vBiNormal , pst_Norm[1], vTangent );
			        D3DXVec3Normalize( vBiNormal, vBiNormal );//*/


					vTangent=vTangentp + pst_Triangle->auw_Index[2];
					vBiNormal=vBiNormalp + pst_Triangle->auw_Index[2];

					D3DXVec3Cross( &vTemp, &B, &N );
					vTangent->x = vTemp.x * fScale2;
					D3DXVec3Cross( &vTemp, &N, &T );
					vTangent->y = -(vTemp.x * fScale2);
					D3DXVec3Cross( &vTemp, &T, &B );
					vTangent->z = vTemp.x * fScale2;
					D3DXVec3Normalize( (vTangent), (vTangent) );

					D3DXVec3Cross( vBiNormal , pst_Norm[2], vTangent );
			        D3DXVec3Normalize( vBiNormal, vBiNormal );//*/

/*

{

//static INT ff=0;
float r=0.0f;
float v=0.0f;
float b=0.0f;

if (i==0)		{r=1;v=0;b=0;}
else if (i==1)	{r=1;v=0;b=0;}
else if (i==2)	{r=-1;v=0;b=0;}
else if (i==3)	{r=-1;v=0;b=0;}
else if (i==4)	{r=1;v=0;b=0;}
else if (i==5)	{r=1;v=0;b=0;}
else if (i==6)	{r=1;v=0;b=0;}
else if (i==7)	{r=1;v=0;b=0;}
else if (i==8)	{r=0;v=0;b=1;}
else if (i==9)	{r=0;v=0;b=1;}
else if (i==10) {r=0;v=0;b=-1;}
else if (i==11) {r=0;v=0;b=-1;}
else
{r=1;v=1;b=1;}
//Chouette Youpi c'est localisé ca marche c'est de la poustafouelle !!
vTangent=vTangentp + pst_Triangle->auw_Index[0];
vTangent->x = r;
vTangent->y = v;
vTangent->z = b;
		
vTangent=vTangentp + pst_Triangle->auw_Index[1];
vTangent->x = r;
vTangent->y = v;
vTangent->z = b;

vTangent=vTangentp + pst_Triangle->auw_Index[2];
vTangent->x = r;
vTangent->y = v;
vTangent->z = b;

//if (!ff) ff=1;
//else ff=0;
i++;
}
*/					//(*vBiNormal).x = -(D3DXVec3Cross( &vTemp, &B, &N )->y * fScale2);
					//(*vBiNormal).y =   D3DXVec3Cross( &vTemp, &N, &T )->y * fScale2;
					//(*vBiNormal).z = -(D3DXVec3Cross( &vTemp, &T, &B )->y * fScale2);
					//D3DXVec3Normalize( &(*vBiNormal), &(*vBiNormal) );

					//(*vNormal).x =   D3DXVec3Cross( &vTemp, &B, &N )->z * fScale2;
					//(*vNormal).y = -(D3DXVec3Cross( &vTemp, &N, &T )->z * fScale2);
					//(*vNormal).z =   D3DXVec3Cross( &vTemp, &T, &B )->z * fScale2;
					//D3DXVec3Normalize( &(*vNormal), &(*vNormal) );
				
			}


//===============================================================
/*
{

	D3DXVECTOR3 edge0,edge1;
	D3DXVECTOR2 cross;
	MATH_tdst_Vector vtange;

	edge0.x = pst_Pt[1]->x - pst_Pt[0]->x;
	edge0.y = *pst_u[1] - *pst_u[0];
	edge0.z = *pst_v[1] - *pst_v[0];

	edge1.x = pst_Pt[2]->x - pst_Pt[0]->x;
	edge1.y = *pst_u[2] - *pst_u[0];
	edge1.z = *pst_v[2] - *pst_v[0];

	vTangent=vTangentp + pst_Triangle->auw_Index[0];
//	cross = CROSS( edge0, edge1);
	cross.x = edge0.y*edge1.z - edge0.z*edge1.y;
	cross.y = edge0.z*edge1.x - edge0.x*edge1.z;
	vTangent->x = - cross.y / cross.x;

	edge0.x = pst_Pt[1]->y - pst_Pt[0]->y;
	edge1.x = pst_Pt[2]->y - pst_Pt[0]->y;
	cross.x = edge0.y*edge1.z - edge0.z*edge1.y;
	cross.y = edge0.z*edge1.x - edge0.x*edge1.z;
	vTangent->y = - cross.y / cross.x;

	edge0.x = pst_Pt[1]->z - pst_Pt[0]->z;
	edge1.x = pst_Pt[2]->z - pst_Pt[0]->z;
	cross.x = edge0.y*edge1.z - edge0.z*edge1.y;
	cross.y = edge0.z*edge1.x - edge0.x*edge1.z;
	vTangent->z = - cross.y / cross.x;
	
	D3DXVec3Normalize( vTangent, vTangent );


	edge0.x = pst_Pt[1]->x - pst_Pt[0]->x;
	edge0.y = *pst_u[1] - *pst_u[0];
	edge0.z = *pst_v[1] - *pst_v[0];

	edge1.x = pst_Pt[2]->x - pst_Pt[0]->x;
	edge1.y = *pst_u[2] - *pst_u[0];
	edge1.z = *pst_v[2] - *pst_v[0];

	vTangent=vTangentp + pst_Triangle->auw_Index[1];
//	cross = CROSS( edge0, edge1);
	cross.x = edge0.y*edge1.z - edge0.z*edge1.y;
	cross.y = edge0.z*edge1.x - edge0.x*edge1.z;
	vTangent->x = - cross.y / cross.x;

	edge0.x = pst_Pt[1]->y - pst_Pt[0]->y;
	edge1.x = pst_Pt[2]->y - pst_Pt[0]->y;
	cross.x = edge0.y*edge1.z - edge0.z*edge1.y;
	cross.y = edge0.z*edge1.x - edge0.x*edge1.z;
	vTangent->y = - cross.y / cross.x;

	edge0.x = pst_Pt[1]->z - pst_Pt[0]->z;
	edge1.x = pst_Pt[2]->z - pst_Pt[0]->z;
	cross.x = edge0.y*edge1.z - edge0.z*edge1.y;
	cross.y = edge0.z*edge1.x - edge0.x*edge1.z;
	vTangent->z = - cross.y / cross.x;
	
	D3DXVec3Normalize( vTangent, vTangent );	

	edge0.x = pst_Pt[1]->x - pst_Pt[0]->x;
	edge0.y = *pst_u[1] - *pst_u[0];
	edge0.z = *pst_v[1] - *pst_v[0];

	edge1.x = pst_Pt[2]->x - pst_Pt[0]->x;
	edge1.y = *pst_u[2] - *pst_u[0];
	edge1.z = *pst_v[2] - *pst_v[0];

	vTangent=vTangentp + pst_Triangle->auw_Index[2];
//	cross = CROSS( edge0, edge1);
	cross.x = edge0.y*edge1.z - edge0.z*edge1.y;
	cross.y = edge0.z*edge1.x - edge0.x*edge1.z;
	vTangent->x = - cross.y / cross.x;

	edge0.x = pst_Pt[1]->y - pst_Pt[0]->y;
	edge1.x = pst_Pt[2]->y - pst_Pt[0]->y;
	cross.x = edge0.y*edge1.z - edge0.z*edge1.y;
	cross.y = edge0.z*edge1.x - edge0.x*edge1.z;
	vTangent->y = - cross.y / cross.x;

	edge0.x = pst_Pt[1]->z - pst_Pt[0]->z;
	edge1.x = pst_Pt[2]->z - pst_Pt[0]->z;
	cross.x = edge0.y*edge1.z - edge0.z*edge1.y;
	cross.y = edge0.z*edge1.x - edge0.x*edge1.z;
	vTangent->z = - cross.y / cross.x;
	
	D3DXVec3Normalize( vTangent, vTangent );
}
*/
#endif 0
		}
    }


	//pst_Object->CotangantesU = pst_Object->dst_Point;




		// position VB
//		if ( !pObject->pVB_Position )
//		if ( !pst_Object->pVB_Position )
		{
			FVF = D3DFVF_XYZ;
			stride = D3DXGetFVFVertexSize( FVF );

			// create position VB
			//pst_Object->CotangantesU = Gx8_VertexBuffer_Allocate( pst_Object->l_NbPoints, stride ); 
			//regarder pour le : assert( pst_Object->pVB_Position != NULL );
			/* MAX
			if ( FAILED( IDirect3DDevice8_CreateVertexBuffer( pSD->mp_D3DDevice,
																pObject->l_NbPoints * stride,
																D3DUSAGE_WRITEONLY,
																FVF,
																D3DPOOL_DEFAULT,
																&pObject->pVB_Position ) ) )
			{
				assert(FALSE);
			}
			*/
	        
			// fill position VB
			/*IDirect3DVertexBuffer8_Lock( pst_Object->CotangantesU, 0, 0, (void *) &pData, 0 );
			memcpy( pData, pst_Object->CotangantesU, pst_Object->l_NbPoints * stride );
			IDirect3DVertexBuffer8_Unlock( pst_Object->CotangantesU );*/
		}
/*
		// Normals VB
		if ( !pObject->pVB_Normal )
		{
			FVF = D3DFVF_XYZ;
			stride = D3DXGetFVFVertexSize( FVF );

			// create position VB
			pObject->pVB_Normal= Gx8_VertexBuffer_Allocate( pObject->l_NbPoints, stride ); 
			assert( pObject->pVB_Position != NULL );
			/* MAX
			if ( FAILED( IDirect3DDevice8_CreateVertexBuffer( pSD->mp_D3DDevice,
																pObject->l_NbPoints * stride,
																D3DUSAGE_WRITEONLY,
																FVF,
																D3DPOOL_DEFAULT,
																&pObject->pVB_Normal ) ) )
			{
				assert(FALSE);
			}
	*/
			// fill position VB
	/*		IDirect3DVertexBuffer8_Lock( pObject->pVB_Normal, 0, 0, (void *) &pData, 0 );
			memcpy( pData, pObject->dst_PointNormal, pObject->l_NbPoints * stride );
			IDirect3DVertexBuffer8_Unlock( pObject->pVB_Normal );
		}
*/
#endif

}


