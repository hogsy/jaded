/*$T WORexporttomad.c GC!1.71 01/31/00 14:17:37 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#	include "BASe/BAStypes.h"
#	include "BASe/CLIbrary/CLImem.h"
#	include "BASe/CLIbrary/CLIstr.h"
#	include "BIGfiles/BIGfat.h"
#	include "BIGfiles/BIGexport.h"
#	include "EDIPaths.h"
#	include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#	include "MAD_mem/Sources/MAD_mem.h"
#	include "LIGHT/LIGHTstruct.h"
#	include "CAMera/CAMera.h"
#	include "CAMera/CAMstruct.h"
#	include "MATerial/MATstruct.h"
#	include "ENGine/Sources/WORld/WORexporttomad.h"
#	include "ENGine/Sources/OBJects/OBJgrp.h"
#	include "ENGine/Sources/OBJects/OBJconst.h"
#	include "ENGine/Sources/OBJects/OBJorient.h"
#	include "ENGine/Sources/OBJects/OBJslowaccess.h"
#	include "GraphicDK/Sources/GEOmetric/GEO_SKIN.h"
#	include "GraphicDK/Sources/TEXture/TEXfile.h"
#	include "GraphicDK/Sources/TEXture/TEXconvert.h"
#	include "SDK/Sources/BIGfiles/BIGdefs.h"

#	define WOR_M_SetMadId( st_Id, id, type, name ) \
		st_Id.IDType = id;                          \
		st_Id.SizeOfThisOne = sizeof( type );       \
		strcpy( st_Id.Name, name );

static void SavePonderations( WOR_tdst_World *_pst_World, MAD_World *st_MadWorld, unsigned long ul_Waypoint )
{
	for ( unsigned long ul = 0; ul < st_MadWorld->NumberOfObjects - ul_Waypoint; ul++ )
	{
		if ( st_MadWorld->AllObjects[ ul ]->IDType != ID_MAD_SkinnedGeometricObject )
			continue;

		GRO_tdst_Struct *pst_Gro = ( GRO_tdst_Struct * ) _pst_World->st_GraphicObjectsTable.p_Table[ ul ];

		// Search for his GaO.
		GRO_tdst_Struct *pst_GaO_Gro;
		OBJ_tdst_GameObject *pst_GaO;
		TAB_tdst_PFelem *pst_Elem = TAB_pst_PFtable_GetFirstElem( &_pst_World->st_AllWorldObjects );
		TAB_tdst_PFelem *pst_LastElem = TAB_pst_PFtable_GetLastElem( &_pst_World->st_AllWorldObjects );
		do
		{
			pst_GaO = ( OBJ_tdst_GameObject * ) pst_Elem->p_Pointer;
			if ( pst_GaO->pst_Base && pst_GaO->pst_Base->pst_Visu )
				pst_GaO_Gro = pst_GaO->pst_Base->pst_Visu->pst_Object;
			else
				pst_GaO_Gro = NULL;
			++pst_Elem;
		} while ( pst_Elem <= pst_LastElem && pst_GaO_Gro != pst_Gro );

		if ( pst_Gro == NULL || pst_GaO == NULL )
			continue;

		if ( pst_GaO_Gro != pst_Gro )
		{
			st_MadWorld->AllObjects[ ul ]->IDType = ID_MAD_GeometricObject_V0;
			continue;
		}
		else if ( pst_GaO_Gro->i->ul_Type == GRO_GeoStaticLOD )
		{
			st_MadWorld->AllObjects[ ul ]->IDType = ID_MAD_GeometricObjectLOD;
			continue;
		}

		// When GaO found.

		GEO_tdst_Object *pst_Object = ( GEO_tdst_Object * ) pst_Gro;
		GEO_SKN_Compress( pst_Object );
		for ( unsigned short j = 0; j < pst_Object->p_SKN_Objectponderation->NumberPdrtLists; ++j )
		{
			pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ j ];
		}

		MAD_SkinnedGeometricObject *pst_SkndObj;
		MAD_MALLOC( MAD_SkinnedGeometricObject, pst_SkndObj, 1 );
		WOR_M_SetMadId(
		        pst_SkndObj->ID,
		        ID_MAD_SkinnedGeometricObject,
		        MAD_SkinnedGeometricObject,
		        pst_Gro->sz_Name );
		pst_SkndObj->pst_GeoObj = ( MAD_GeometricObject * ) st_MadWorld->AllObjects[ ul ];
		pst_SkndObj->us_NumberOfPonderationLists = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;

		MAD_MALLOC( MAD_PonderationList, pst_SkndObj->pst_PonderationList, pst_SkndObj->us_NumberOfPonderationLists );
		for ( unsigned short j = 0; j < pst_SkndObj->us_NumberOfPonderationLists; ++j )
		{
			MAD_PonderationList *pst_Mad_PdrtList = &pst_SkndObj->pst_PonderationList[ j ];
			GEO_tdst_VertexPonderationList *pst_Geo_PdrtList = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ j ];
			OBJ_tdst_GizmoPtr *pst_GaO_GizmoPtr = pst_GaO->pst_Base->pst_AddMatrix->dst_GizmoPtr;

			// Fill the pst_SkndObj.
			if ( pst_GaO_GizmoPtr[ pst_Geo_PdrtList->us_IndexOfMatrix ].pst_GO )
			{
				pst_Mad_PdrtList->us_MatrixIdx = 0;
				char *psz_Ext = strrchr( pst_GaO_GizmoPtr[ pst_Geo_PdrtList->us_IndexOfMatrix ].pst_GO->sz_Name, '.' );
				if ( psz_Ext )
					*psz_Ext = 0;
				while ( pst_Mad_PdrtList->us_MatrixIdx < st_MadWorld->NumberOfHierarchieNodes && strcmp( st_MadWorld->Hierarchie[ pst_Mad_PdrtList->us_MatrixIdx ].ID.Name, pst_GaO_GizmoPtr[ pst_Geo_PdrtList->us_IndexOfMatrix ].pst_GO->sz_Name ) )
					++pst_Mad_PdrtList->us_MatrixIdx;
				if ( psz_Ext )
					*psz_Ext = '.';
			}
			else
			{
				pst_Mad_PdrtList->us_MatrixIdx = 0xFFFF;
			}
			WOR_M_SetMadId(
			        pst_Mad_PdrtList->st_FlashedMatrix.ID,
			        ID_MAD_Matrix,
			        MAD_Matrix,
			        "" );
			pst_Mad_PdrtList->st_FlashedMatrix.I.x = pst_Geo_PdrtList->st_FlashedMatrix.Ix;
			pst_Mad_PdrtList->st_FlashedMatrix.I.y = pst_Geo_PdrtList->st_FlashedMatrix.Iy;
			pst_Mad_PdrtList->st_FlashedMatrix.I.z = pst_Geo_PdrtList->st_FlashedMatrix.Iz;
			pst_Mad_PdrtList->st_FlashedMatrix.J.x = pst_Geo_PdrtList->st_FlashedMatrix.Jx;
			pst_Mad_PdrtList->st_FlashedMatrix.J.y = pst_Geo_PdrtList->st_FlashedMatrix.Jy;
			pst_Mad_PdrtList->st_FlashedMatrix.J.z = pst_Geo_PdrtList->st_FlashedMatrix.Jz;
			pst_Mad_PdrtList->st_FlashedMatrix.K.x = pst_Geo_PdrtList->st_FlashedMatrix.Kx;
			pst_Mad_PdrtList->st_FlashedMatrix.K.y = pst_Geo_PdrtList->st_FlashedMatrix.Ky;
			pst_Mad_PdrtList->st_FlashedMatrix.K.z = pst_Geo_PdrtList->st_FlashedMatrix.Kz;
			pst_Mad_PdrtList->st_FlashedMatrix.Translation.x = pst_Geo_PdrtList->st_FlashedMatrix.T.x;
			pst_Mad_PdrtList->st_FlashedMatrix.Translation.y = pst_Geo_PdrtList->st_FlashedMatrix.T.y;
			pst_Mad_PdrtList->st_FlashedMatrix.Translation.z = pst_Geo_PdrtList->st_FlashedMatrix.T.z;
			pst_Mad_PdrtList->us_NumberOfPonderatedVertices = pst_Geo_PdrtList->us_NumberOfPonderatedVertices;
			MAD_MALLOC( MAD_CompressedPonderatedVertex, pst_Mad_PdrtList->p_CmpPdrtVrt, pst_Mad_PdrtList->us_NumberOfPonderatedVertices );
			for ( unsigned short k = 0; k < pst_Mad_PdrtList->us_NumberOfPonderatedVertices; ++k )
			{
				pst_Mad_PdrtList->p_CmpPdrtVrt[ k ].Ponderation = pst_Geo_PdrtList->p_PdrtVrc_C[ k ].Ponderation;
				pst_Mad_PdrtList->p_CmpPdrtVrt[ k ].Index = pst_Geo_PdrtList->p_PdrtVrc_C[ k ].Index;
			}
		}
		st_MadWorld->AllObjects[ ul ] = ( MAD_NodeID * ) pst_SkndObj;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL WOR_b_World_ExportMadFile( WOR_tdst_World *_pst_World,
                                char *_sz_FileName,
                                char *_sz_ExportDir,
                                unsigned char _uc_Sel,
                                unsigned char _uc_Texture,
                                BOOL _b_ExportSkin )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAD_World st_MadWorld;
	ULONG ul, ul_HierarchyIdx, ul_Size;
	MAD_MAT_MatID *pst_MadMat;
	GRO_tdst_Struct *pst_Gro, *pst_Mat;
	GRO_tdst_Struct *pst_GaO_Gro;
	OBJ_tdst_GameObject *pst_GaO;
	//GEO_tdst_Object     *pst_Object;
	MAD_NodeID *pst_MadObj;
	MAD_texture *pst_MadTex;
	TEX_tdst_Data *pst_TexData;
	TEX_tdst_File_Desc st_Tex_FileDesc;
	TEX_tdst_File_Desc st_TgaDesc;
	TEX_tdst_File_Tex st_Tex;
	TEX_tdst_Palette st_Palette;
	MAD_WorldNode *pst_MadNode;
	MAD_WorldNode st_TmpMadNode;
	MAD_Matrix *pst_MadMatrix;
	MATH_tdst_Matrix TempMat, M;
	FILE *hpFile;
	TAB_tdst_PFelem *pst_Elem, *pst_LastElem;
	OBJ_tdst_GameObject *pst_GO, *pst_Father;
	ULONG ul_Index, ul_Waypoint, ul_WPIndex, ul_NbOfGao;
	ULONG *pul_RLI;
	char sz_Name[ BIG_C_MaxLenPath ];
	char sz_Path[ BIG_C_MaxLenPath ];
	char sz_TgaName[ BIG_C_MaxLenPath ];
	char *psz_Ext;
	MATH_tdst_Vector st_Axis;
	LONG *dl_PFTableToMad;
	LONG *dl_MatIndex, MatMad;
	ULONG Mat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Mad_meminit();

	/* Put the mad path in ``sz_Path''. */
	L_strcpy( sz_Path, _sz_FileName );
	psz_Ext = sz_Path;
	while ( *psz_Ext )
	{
		if ( *psz_Ext == '\\' )
			*psz_Ext = '/';
		++psz_Ext;
	}
	psz_Ext = L_strrchr( sz_Path, '/' );
	*psz_Ext = 0;

	L_memset( &st_MadWorld, 0, sizeof( MAD_World ) );
	st_MadWorld.MAD_Version = MAD_WORLD_VERION_2;
	WOR_M_SetMadId( st_MadWorld.ID, ID_MAD_World, MAD_World, _pst_World->sz_Name );
	st_MadWorld.AmbientColor = _pst_World->ul_AmbientColor;

	/* Pre computation for group data and waypoint */
	TAB_PFtable_RemoveHoles( &_pst_World->st_AllWorldObjects );
	pst_Elem = TAB_pst_PFtable_GetFirstElem( &_pst_World->st_AllWorldObjects );
	pst_LastElem = TAB_pst_PFtable_GetLastElem( &_pst_World->st_AllWorldObjects );
	ul_Waypoint = 0;
	ul_NbOfGao = pst_LastElem - pst_Elem + 1;
	dl_PFTableToMad = ( LONG * ) L_malloc( ul_NbOfGao * 4 );
	ul_NbOfGao = 0;

	for ( ul = 0; pst_Elem <= pst_LastElem; pst_Elem++, ul++ )
	{
		pst_GO = ( OBJ_tdst_GameObject * ) pst_Elem->p_Pointer;

		if ( ( !_uc_Sel ) || ( pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected ) )
			dl_PFTableToMad[ ul ] = ++ul_NbOfGao;
		else
			dl_PFTableToMad[ ul ] = -1;

		if ( !OBJ_b_TestIdentityFlag( pst_GO, OBJ_C_IdentityFlag_Visu ) )
		{
			if ( OBJ_b_TestIdentityFlag( pst_GO, OBJ_C_IdentityFlag_Waypoints ) )
				ul_Waypoint++;
		}
	}

	/* Convert texture list */
	st_MadWorld.NumberOftexture = TEX_gst_GlobalList.l_NumberOfTextures;
	if ( st_MadWorld.NumberOftexture )
	{
		MAD_MALLOC( MAD_texture *, st_MadWorld.AllTextures, st_MadWorld.NumberOftexture );
		for ( ul = 0; ul < st_MadWorld.NumberOftexture; ul++ )
		{
			pst_TexData = &TEX_gst_GlobalList.dst_Texture[ ul ];
			MAD_MALLOC( MAD_texture, pst_MadTex, 1 );
			st_MadWorld.AllTextures[ ul ] = pst_MadTex;
			WOR_M_SetMadId( pst_MadTex->ID, ID_MAD_Texture, MAD_texture, "" );

			pst_MadTex->Texturefile[ 0 ] = 0;
			if ( ul < 2 )
				continue;

			ul_Index = BIG_ul_SearchKeyToFat( pst_TexData->ul_Key );
			if ( ul_Index != BIG_C_InvalidKey )
			{
				if ( _uc_Texture )
				{
					//BIG_ComputeFullName( BIG_ParentFile( ul_Index ), sz_Name );
					//if (L_strnicmp( sz_Name, EDI_Csz_Path_Textures, L_strlen( EDI_Csz_Path_Textures ) ) == 0)
					//    psz_Name = sz_Name + strlen( EDI_Csz_Path_Textures ) + 1;
					//else if (L_strnicmp( sz_Name, EDI_Csz_Path_TrashCan_Textures, L_strlen( EDI_Csz_Path_TrashCan_Textures ) ) == 0)
					//    psz_Name = sz_Name + strlen( EDI_Csz_Path_TrashCan_Textures ) + 1;
					//else
					//    psz_Name = NULL;

					//if (psz_Name)
					//    sprintf( sz_OutName, "%s/Textures/%s/%s", _sz_ExportDir, psz_Name, BIG_NameFile( ul_Index ) );
					//else
					//    sprintf( sz_OutName, "%s/Textures/%s", _sz_ExportDir, BIG_NameFile( ul_Index ) );

					///* create directory */
					//psz_Name = sz_OutName;
					//while( *psz_Name )
					//{
					//    if (*psz_Name == '\\') *psz_Name = '/';
					//    psz_Name++;
					//}

					//psz_Name = L_strchr( sz_OutName, '/' );
					//while( psz_Name )
					//{
					//    *psz_Name = 0;
					//    CreateDirectory( sz_OutName, NULL );
					//    *psz_Name = '/';
					//    psz_Name = L_strchr( psz_Name + 1, '/' );
					//}
					//BIG_ExportFileToDisk( sz_OutName, sz_Name, BIG_NameFile( ul_Index ) );
					//L_strcpy(pst_MadTex->Texturefile, sz_OutName );

					L_strcpy( sz_Name, BIG_NameFile( ul_Index ) );
					psz_Ext = L_strrchr( sz_Name, '.' );
					if ( psz_Ext )
					{
						++psz_Ext;
						if ( !L_stricmp( psz_Ext, "tex" ) )
						{
							TEX_l_File_GetInfoAndContent( pst_TexData->ul_Key, &st_Tex_FileDesc );
							L_memcpy( &st_Tex, &st_Tex_FileDesc.st_Tex, sizeof( TEX_tdst_File_Tex ) );
#	if defined( _XENON_RENDER )
							if ( st_Tex.st_XeProperties.ul_OriginalTexture != BIG_C_InvalidKey )
							{
								// Change the extension to .tga and export the original texture
								strcpy( psz_Ext, "tga" );
								TEX_l_File_GetInfoAndContent( st_Tex.st_XeProperties.ul_OriginalTexture, &st_TgaDesc );
								sprintf( sz_TgaName, "%s/%s", sz_Path, sz_Name );
								TEX_l_File_SaveTga( sz_TgaName, &st_TgaDesc );
								L_strcpy( pst_MadTex->Texturefile, sz_TgaName );
							}
							else
#	endif
							        if ( st_Tex.ast_Slot[ 0 ].ul_Raw != BIG_C_InvalidKey )
							{
								TEX_File_FreeDescription( &st_Tex_FileDesc );
								TEX_l_File_GetInfoAndContent( st_Tex.ast_Slot[ 0 ].ul_Raw, &st_Tex_FileDesc );
								L_memset( &st_TgaDesc, 0, sizeof( TEX_tdst_File_Desc ) );
								TEX_File_InitParams( &st_TgaDesc.st_Params );
								st_TgaDesc.uw_Width = st_Tex_FileDesc.st_Params.uw_Width;
								st_TgaDesc.uw_Height = st_Tex_FileDesc.st_Params.uw_Height;
								ul_Size = st_TgaDesc.uw_Width * st_TgaDesc.uw_Height;
								st_TgaDesc.p_Bitmap = MEM_p_Alloc( ul_Size * 4 );
								if ( st_Tex.ast_Slot[ 0 ].ul_Pal != BIG_C_InvalidKey )
								{
									L_memset( &st_Palette, 0, sizeof( TEX_tdst_Palette ) );
									st_Palette.ul_Key = st_Tex.ast_Slot[ 0 ].ul_Pal;
									TEX_File_LoadPalette( &st_Palette, FALSE );
								}
								if ( st_Palette.uc_Flags & TEX_uc_AlphaPalette )
								{
									st_TgaDesc.uc_BPP = 32;
									if ( st_Tex_FileDesc.uc_BPP == 4 )
										TEX_Convert_4To32( ( ULONG * ) st_TgaDesc.p_Bitmap,
										                   ( UCHAR * ) st_Tex_FileDesc.p_Bitmap,
										                   st_Palette.pul_Color,
										                   ul_Size );
									else
										TEX_Convert_8To32( ( ULONG * ) st_TgaDesc.p_Bitmap,
										                   ( UCHAR * ) st_Tex_FileDesc.p_Bitmap,
										                   st_Palette.pul_Color,
										                   ul_Size );
								}
								else
								{
									st_TgaDesc.uc_BPP = 24;
									if ( st_Tex_FileDesc.uc_BPP == 4 )
										TEX_Convert_4To24( ( UCHAR * ) st_TgaDesc.p_Bitmap,
										                   ( UCHAR * ) st_Tex_FileDesc.p_Bitmap,
										                   st_Palette.pul_Color,
										                   st_Tex_FileDesc.st_Params.uw_Width,
										                   st_Tex_FileDesc.st_Params.uw_Height,
										                   0 );
									else
										TEX_Convert_8To24( ( UCHAR * ) st_TgaDesc.p_Bitmap,
										                   ( UCHAR * ) st_Tex_FileDesc.p_Bitmap,
										                   st_Palette.pul_Color,
										                   st_Tex_FileDesc.st_Params.uw_Width,
										                   st_Tex_FileDesc.st_Params.uw_Height,
										                   0 );
								}
								L_strcpy( psz_Ext, "tga" );
								sprintf( sz_TgaName, "%s/%s", sz_Path, sz_Name );
								TEX_l_File_SaveTga( sz_TgaName, &st_TgaDesc );
								L_strcpy( pst_MadTex->Texturefile, sz_TgaName );
								TEX_File_FreeDescription( &st_TgaDesc );
								TEX_File_FreeDescription( &st_Tex_FileDesc );
							}
							else if ( st_Tex_FileDesc.st_Tex.ast_Slot[ 0 ].ul_TC != BIG_C_InvalidKey )
							{
								/** Got to find a TrueColor texture in order to test it deeply...
                    */
								int i = 0;
								//TEX_File_FreeDescription(&st_Tex_FileDesc);
								//TEX_l_File_GetInfoAndContent(st_Tex.ast_Slot[0].ul_TC, &st_Tex_FileDesc);
								//L_memset(&st_TgaDesc, 0, sizeof (TEX_tdst_File_Desc));
								//TEX_File_InitParams(&st_TgaDesc.st_Params);
								//st_TgaDesc.uw_Width = st_Tex_FileDesc.st_Params.uw_Width;
								//st_TgaDesc.uw_Height = st_Tex_FileDesc.st_Params.uw_Height;
								//ul_Size = st_TgaDesc.uw_Width * st_TgaDesc.uw_Height;
								//st_TgaDesc.p_Bitmap = MEM_p_Alloc(ul_Size * 4);
								//*psz_Ext = 0;
								//sprintf(sz_TgaName, "%s/%stga", _sz_ExportDir, psz_Name);
								//TEX_l_File_SaveTga(sz_TgaName, &st_TgaDesc);
								//L_strcpy(pst_MadTex->Texturefile, sz_TgaName);
								//TEX_File_FreeDescription(&st_TgaDesc);
								//TEX_File_FreeDescription(&st_Tex_FileDesc);
							}
						}
						else if ( !L_stricmp( psz_Ext, "tga" ) )
						{
							TEX_l_File_GetInfoAndContent( pst_TexData->ul_Key, &st_TgaDesc );
							sprintf( sz_TgaName, "%s/%s", sz_Path, sz_Name );
							TEX_l_File_SaveTga( sz_TgaName, &st_TgaDesc );
							L_strcpy( pst_MadTex->Texturefile, sz_TgaName );
						}
					}
				}
				else
					L_strcpy( pst_MadTex->Texturefile, BIG_NameFile( ul_Index ) );
			}
		}
	}

	/* Convert material list */
	dl_MatIndex = NULL;
	st_MadWorld.NumberOfMaterials = Mat = TAB_ul_Ptable_GetNbElems( &_pst_World->st_GraphicMaterialsTable );
	if ( Mat )
	{
		MAD_MALLOC( MAD_MAT_MatID *, st_MadWorld.AllMaterial, Mat );
		dl_MatIndex = ( LONG * ) L_malloc( 4 * Mat );

		MatMad = 0;
		for ( ul = 0; ul < Mat; ul++ )
		{
			pst_Gro = ( GRO_tdst_Struct * ) _pst_World->st_GraphicMaterialsTable.p_Table[ ul ];
			if ( !GRO_b_IsAMaterial( pst_Gro ) )
			{
				dl_MatIndex[ ul ] = -1;
				continue;
			}

			pst_MadMat = ( MAD_MAT_MatID * ) pst_Gro->i->pfnp_ToMad( pst_Gro, _pst_World );
			st_MadWorld.AllMaterial[ MatMad ] = pst_MadMat;
			dl_MatIndex[ ul ] = MatMad++;
		}

		st_MadWorld.NumberOfMaterials = MatMad;
	}

	/* Convert object list */
	st_MadWorld.NumberOfObjects = TAB_ul_Ptable_GetNbElems( &_pst_World->st_GraphicObjectsTable ) + ul_Waypoint;
	if ( st_MadWorld.NumberOfObjects )
	{
		MAD_MALLOC( MAD_NodeID *, st_MadWorld.AllObjects, st_MadWorld.NumberOfObjects );

		for ( ul = 0; ul < st_MadWorld.NumberOfObjects - ul_Waypoint; ul++ )
		{
			pst_Gro = ( GRO_tdst_Struct * ) _pst_World->st_GraphicObjectsTable.p_Table[ ul ];
			pst_MadObj = ( MAD_NodeID * ) pst_Gro->i->pfnp_ToMad( pst_Gro, _pst_World );
			if ( _b_ExportSkin && pst_MadObj->IDType == ID_MAD_GeometricObject_V0 && GEO_SKN_IsSkinned( ( GEO_tdst_Object * ) pst_Gro ) )
			{
				pst_Elem = TAB_pst_PFtable_GetFirstElem( &_pst_World->st_AllWorldObjects );
				pst_LastElem = TAB_pst_PFtable_GetLastElem( &_pst_World->st_AllWorldObjects );
				do
				{
					pst_GaO = ( OBJ_tdst_GameObject * ) pst_Elem->p_Pointer;
					if ( pst_GaO->pst_Base && pst_GaO->pst_Base->pst_Visu )
						pst_GaO_Gro = pst_GaO->pst_Base->pst_Visu->pst_Object;
					else
						pst_GaO_Gro = NULL;
					++pst_Elem;
				} while ( pst_Elem <= pst_LastElem && pst_GaO_Gro != pst_Gro );

				// If GaO found and no skinning errors (e.g. skinned GrO's bone).
				if ( pst_GaO_Gro == pst_Gro && 
					OBJ_b_TestIdentityFlag( pst_GaO, OBJ_C_IdentityFlag_AdditionalMatrix ) && 
					OBJ_b_TestIdentityFlag( pst_GaO, OBJ_C_IdentityFlag_AddMatArePointer ) && 
					pst_GaO->pst_Base->pst_AddMatrix && 
					pst_GaO->pst_Base->pst_AddMatrix->l_Number >= 2 )
				{
					pst_MadObj->IDType = ID_MAD_SkinnedGeometricObject;
				}
			}
			st_MadWorld.AllObjects[ ul ] = pst_MadObj;
		}

		/* Create object for waypoint node */
		for ( ; ul < st_MadWorld.NumberOfObjects; ul++, ul_Index++ )
		{
			MAD_Dummy *pstTempDummy = NULL;
			MAD_MALLOC( MAD_Dummy, pstTempDummy, 1 );
			pst_MadObj = ( MAD_NodeID * ) pstTempDummy;
			WOR_M_SetMadId( ( ( MAD_Dummy * ) pst_MadObj )->ID, ID_MAD_Dummy, MAD_Dummy, "Dummy" );
			( ( MAD_Dummy * ) pst_MadObj )->Type = DUMMY_POINT;
			st_MadWorld.AllObjects[ ul ] = pst_MadObj;
		}
	}

	/* Save hierarchy nodes, add one to number to put root node in mad scene */
	st_MadWorld.NumberOfHierarchieNodes = ul_NbOfGao + 1;
	if ( st_MadWorld.NumberOfHierarchieNodes )
	{
		MAD_MALLOC( MAD_WorldNode, st_MadWorld.Hierarchie, st_MadWorld.NumberOfHierarchieNodes );
		pst_MadNode = st_MadWorld.Hierarchie;

		/* Root node */
		WOR_M_SetMadId( pst_MadNode->ID, ID_MAD_WorldNode, MAD_WorldNode, "root" );
		pst_MadMatrix = &pst_MadNode->Matrix;
		L_memset( pst_MadMatrix, 0, sizeof( MAD_Matrix ) );
		WOR_M_SetMadId( pst_MadMatrix->ID, ID_MAD_Matrix, MAD_Matrix, "" );
		pst_MadMatrix->I.x = pst_MadMatrix->J.y = pst_MadMatrix->K.z = 1.0f;
		L_memcpy( &pst_MadNode->RelativeMatrix, pst_MadMatrix, sizeof( MAD_Matrix ) );
		pst_MadNode->Object = MAD_NULL_INDEX;
		pst_MadNode->Parent = MAD_NULL_INDEX;
		pst_MadNode->Target = MAD_NULL_INDEX;
		pst_MadNode->Material = MAD_NULL_INDEX;
		pst_MadNode++;

		/* Other nodes */
		ul_WPIndex = 0;
		pst_Elem = TAB_pst_PFtable_GetFirstElem( &_pst_World->st_AllWorldObjects );
		pst_LastElem = TAB_pst_PFtable_GetLastElem( &_pst_World->st_AllWorldObjects );
		for ( ; pst_Elem <= pst_LastElem; pst_Elem++ )
		{
			pst_GO = ( OBJ_tdst_GameObject * ) pst_Elem->p_Pointer;

			if ( _uc_Sel && !( pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected ) )
				continue;

			// If elem is a bone, copy the gizmo number to order the nodes in Sioux
			if ( pst_GO->sz_Name && ( pst_GO->sz_Name[ 0 ] == 'b' || pst_GO->sz_Name[ 0 ] == 'B' ) && pst_GO->sz_Name[ 1 ] == '_' )
			{
				if ( pst_GO->pst_Base && pst_GO->pst_Base->pst_AddMatrix && pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr )
				{
					pst_MadNode->Undefined4 = ( unsigned long ) pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr->l_MatrixId;
				}
			}

			psz_Ext = strrchr( pst_GO->sz_Name, '.' );
			if ( psz_Ext ) *psz_Ext = 0;
			WOR_M_SetMadId( pst_MadNode->ID, ID_MAD_WorldNode, MAD_WorldNode, pst_GO->sz_Name );
			if ( psz_Ext ) *psz_Ext = '.';

			pst_MadMatrix = &pst_MadNode->Matrix;
			OBJ_MakeAbsoluteMatrix( pst_GO, &TempMat );
			MATH_MakeOGLMatrix( &M, &TempMat );

			WOR_M_SetMadId( pst_MadMatrix->ID, ID_MAD_Matrix, MAD_Matrix, "" );
			MATH_CopyVector( ( MATH_tdst_Vector * ) &pst_MadMatrix->Translation, &M.T );
			MATH_CopyVector( ( MATH_tdst_Vector * ) &pst_MadMatrix->I, ( MATH_tdst_Vector * ) &M.Ix );
			MATH_CopyVector( ( MATH_tdst_Vector * ) &pst_MadMatrix->J, ( MATH_tdst_Vector * ) &M.Jx );
			MATH_CopyVector( ( MATH_tdst_Vector * ) &pst_MadMatrix->K, ( MATH_tdst_Vector * ) &M.Kx );

			if ( OBJ_b_TestIdentityFlag( pst_GO, OBJ_C_IdentityFlag_Visu ) )
			{
				pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Object;
				pst_Mat = pst_GO->pst_Base->pst_Visu->pst_Material;
				pul_RLI = pst_GO->pst_Base->pst_Visu->dul_VertexColors;
				if ( pul_RLI )
				{
					MAD_MALLOC( ULONG, pst_MadNode->RLIOfObject, pul_RLI[ 0 ] );
					L_memcpy( pst_MadNode->RLIOfObject, pul_RLI + 1, pul_RLI[ 0 ] * 4 );
				}
			}
			else if ( OBJ_b_TestIdentityFlag( pst_GO, OBJ_C_IdentityFlag_Lights ) )
			{
				pst_Gro = pst_GO->pst_Extended->pst_Light;
				pst_Mat = NULL;
			}
			else
			{
				pst_Gro = NULL;
				pst_Mat = NULL;
			}

			if ( pst_Gro && ( ( pst_Gro->i->ul_Type == GRO_Camera ) || ( pst_Gro->i->ul_Type == GRO_Light ) ) )
			{
				MATH_CopyVector( &st_Axis, ( MATH_tdst_Vector * ) &pst_MadMatrix->J );
				MATH_CopyVector( ( MATH_tdst_Vector * ) &pst_MadMatrix->J, ( MATH_tdst_Vector * ) &pst_MadMatrix->K );
				MATH_CopyVector( ( MATH_tdst_Vector * ) &pst_MadMatrix->K, &st_Axis );
				MATH_NegEqualVector( ( MATH_tdst_Vector * ) &pst_MadMatrix->I );
			}

			L_memcpy( &pst_MadNode->RelativeMatrix, pst_MadMatrix, sizeof( MAD_Matrix ) );
			pst_MadNode->Parent = 0;
			if ( OBJ_b_IsChild( pst_GO ) )
			{
				pst_Father = pst_GO->pst_Base->pst_Hierarchy->pst_Father;
				ul_Index = TAB_ul_PFtable_GetElemIndexWithPointer( &_pst_World->st_AllWorldObjects, pst_Father );
				if ( dl_PFTableToMad[ ul_Index ] != -1 )
				{
					pst_MadNode->Parent = dl_PFTableToMad[ ul_Index ];
					MATH_MakeOGLMatrix( &M, &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix );
					pst_MadMatrix = &pst_MadNode->RelativeMatrix;
					WOR_M_SetMadId( pst_MadMatrix->ID, ID_MAD_Matrix, MAD_Matrix, "" );
					MATH_CopyVector( ( MATH_tdst_Vector * ) &pst_MadMatrix->Translation, &M.T );
					MATH_CopyVector( ( MATH_tdst_Vector * ) &pst_MadMatrix->I, ( MATH_tdst_Vector * ) &M.Ix );
					MATH_CopyVector( ( MATH_tdst_Vector * ) &pst_MadMatrix->J, ( MATH_tdst_Vector * ) &M.Jx );
					MATH_CopyVector( ( MATH_tdst_Vector * ) &pst_MadMatrix->K, ( MATH_tdst_Vector * ) &M.Kx );
				}
			}

			pst_MadNode->Target = MAD_NULL_INDEX;
			pst_MadNode->ObjectWithRadiosity = MAD_NULL_INDEX;
			pst_MadNode->NODE_Flags = 0;
			pst_MadNode->Object = MAD_NULL_INDEX;
			pst_MadNode->Material = MAD_NULL_INDEX;

			if ( pst_Gro )
			{
				pst_MadNode->Object = TAB_ul_Ptable_GetElemIndexWithPointer( &_pst_World->st_GraphicObjectsTable, pst_Gro );
				if ( pst_Mat )
				{
					pst_MadNode->Material = TAB_ul_Ptable_GetElemIndexWithPointer( &_pst_World->st_GraphicMaterialsTable, pst_Mat );
					pst_MadNode->Material = dl_MatIndex[ pst_MadNode->Material ];
				}
			}
			else
			{
				if ( OBJ_b_TestIdentityFlag( pst_GO, OBJ_C_IdentityFlag_Waypoints ) )
				{
					pst_MadNode->Object = ( ul_WPIndex++ ) + ( st_MadWorld.NumberOfObjects - ul_Waypoint );
					if ( ( psz_Ext = strrchr( pst_GO->sz_Name, '.' ) ) )
						*psz_Ext = 0;
					strcpy( pst_MadNode->ID.Name, pst_GO->sz_Name );
					if ( psz_Ext ) *psz_Ext = '.';
				}
			}

			// changed to check for null index instead ~hogsy
			if ( ( pst_MadNode->Object != MAD_NULL_INDEX ) && ( pst_MadNode->Object < st_MadWorld.NumberOfObjects ) )
			{
				if ( st_MadWorld.AllObjects[ pst_MadNode->Object ]->IDType != ID_MAD_UNDEFINED )
					pst_MadNode->ID.IDType = st_MadWorld.AllObjects[ pst_MadNode->Object ]->IDType;
			}
			pst_MadNode++;
		}

		/* Set skins in last ranges */
		if ( _b_ExportSkin && st_MadWorld.NumberOfHierarchieNodes >= 3 )
		{
			ULONG *pul_UpdateIdxTable;
			ULONG ul2;

			pul_UpdateIdxTable = ( ULONG * ) MEM_p_Alloc( st_MadWorld.NumberOfHierarchieNodes * sizeof( ULONG ) );
			for ( ul = 0; ul < st_MadWorld.NumberOfHierarchieNodes; ++ul )
				pul_UpdateIdxTable[ ul ] = ul;
			ul_HierarchyIdx = st_MadWorld.NumberOfHierarchieNodes - 2;
			ul = 1;
			while ( ul_HierarchyIdx > 0 )
			{
				pst_MadNode = &st_MadWorld.Hierarchie[ ul_HierarchyIdx ];
				if ( pst_MadNode->Object != MAD_NULL_INDEX && st_MadWorld.AllObjects[ pst_MadNode->Object ]->IDType == ID_MAD_SkinnedGeometricObject )
				{
					L_memmove( &st_TmpMadNode, pst_MadNode, sizeof( MAD_WorldNode ) );
					L_memmove( pst_MadNode, pst_MadNode + 1, ul * sizeof( MAD_WorldNode ) );
					L_memmove( pst_MadNode + ul, &st_TmpMadNode, sizeof( MAD_WorldNode ) );
					pul_UpdateIdxTable[ ul_HierarchyIdx ] = st_MadWorld.NumberOfHierarchieNodes - 1;
					ul2 = ul_HierarchyIdx + 1;
					while ( ul2 < st_MadWorld.NumberOfHierarchieNodes )
					{
						pul_UpdateIdxTable[ ul2 ] -= 1;
						++ul2;
					}
				}
				--ul_HierarchyIdx;
				++ul;
			}
			ul_HierarchyIdx = 1;
			while ( ul_HierarchyIdx < st_MadWorld.NumberOfHierarchieNodes )
			{
				if ( st_MadWorld.Hierarchie[ ul_HierarchyIdx ].Parent != MAD_NULL_INDEX )
					st_MadWorld.Hierarchie[ ul_HierarchyIdx ].Parent = pul_UpdateIdxTable[ st_MadWorld.Hierarchie[ ul_HierarchyIdx ].Parent ];
				++ul_HierarchyIdx;
			}
			MEM_Free( pul_UpdateIdxTable );
		}
	}

	/* Save ponderations */
	if ( _b_ExportSkin )
		SavePonderations( _pst_World, &st_MadWorld, ul_Waypoint );

	hpFile = fopen( _sz_FileName, "wb" );
	if ( hpFile == NULL )
	{
		sprintf( sz_Name, "Can't access file %s", _sz_FileName );
		ERR_X_Warning( 0, "Error", sz_Name );
	}
	else
	{
		MAD_Save( &st_MadWorld, hpFile, MAD_SaveTextureFiles | MAD_RelativeTexFileName );
		fclose( hpFile );
	}

	Mad_free();
	L_free( dl_PFTableToMad );
	L_free( dl_MatIndex );

	return TRUE;
}

#endif
