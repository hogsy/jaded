/*$T MAX_to_MAD.h GC!1.32 10/20/99 15:08:27 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
void					MAX_Color_To_MAD_Color(Color *MaxColor, MAD_ColorARGB *MadColor);
MAD_NodeID			   *MAX_X_To_MAD_X(INode *MAXNode, MAD_World *MW, TimeValue t);
void					MAX_Matrix_To_MAD_Matrix(Matrix3 *Matrix, MAD_Matrix *MADMat, TimeValue t);
void					MAX_NodeToMAD_Node(INode *MAXNode, MAD_NodeID *MADNode, TimeValue t);
void					MAX_Mat_To_MAD_Mat(MAD_World *MW, Mtl **SourceMaterials, TimeValue t);
MAD_texture			   *MAX_Tex_To_MAD_Tex(Texmap *MaxTexture, TimeValue t);
MAD_GeometricObject	   *Mesh_To_MAD_Mesh(Mesh *mesh,MAD_NodeID *MADNode,MAD_ColorARGB *SecondRliField,TimeValue t);
