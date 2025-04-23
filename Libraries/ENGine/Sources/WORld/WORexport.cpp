// Created by Mark "hogsy" Sowden, 2023 <hogsy@oldtimes-software.com>
// https://oldtimes-software.com/jaded/
// Purpose: General export interfaces for formats other than MAD

#include "Precomp.h"
#include "BIGfiles/BIGfat.h"
#include "TEXture/TEXfile.h"
#include "WOR.h"
#include "WORexport.h"
#include "MAD_loadsave/Sources/MAD_Struct_V0.h"
#include "MAD_mem/Sources/MAD_mem.h"

void WorldExporter::ExportTextures( const char *outDir )
{
	for ( unsigned int i = 0; i < TEX_gst_GlobalList.l_NumberOfTextures; ++i )
	{
		TEX_tdst_Data *textureData = &TEX_gst_GlobalList.dst_Texture[ i ];
		unsigned int index         = BIG_ul_SearchKeyToFat( textureData->ul_Key );
		if ( index == BIG_C_InvalidKey )
		{
			continue;
		}

		const char *name = BIG_NameFile( index );
		assert( name != nullptr );
		if ( name == nullptr )
		{
			continue;
		}

		const char *extension = strrchr( name, '.' );
		assert( extension != nullptr );
		if ( extension == nullptr )
		{
			continue;
		}

		TEX_tdst_File_Desc desc;
		BAS_ZERO( &desc, sizeof( desc ) );

		std::string outPath = outDir;
		if ( stricmp( extension, ".tex" ) == 0 )
		{
			TEX_l_File_GetInfoAndContent( textureData->ul_Key, &desc );
			//TODO
		}
		else if ( stricmp( extension, ".tga" ) == 0 )
		{
			TEX_tdst_File_Desc desc;
			TEX_l_File_GetInfoAndContent( textureData->ul_Key, &desc );
			outPath += "/" + std::string( name );
			TEX_l_File_SaveTga( ( char * ) outPath.c_str(), &desc );
		}
		else if ( stricmp( extension, ".spr" ) == 0 )
		{
			//TODO
		}
		else if ( stricmp( extension, ".raw" ) == 0 )
		{
			//TODO
		}
		else
		{
			// some unsupported type ... ??
			assert( 0 );
			continue;
		}

		TEX_File_FreeDescription( &desc );
	}
}

bool MADExporter::ExportFile( WOR_tdst_World *world, const char *filename, const char *exportDir, bool selected, bool textures )
{
	Mad_meminit();

	//TODO: eventually migrate the existing WORexporttomad implementation here...

	return false;
}

bool GLTFExporter::ExportFile( WOR_tdst_World *world, const char *filename, const char *exportDir, bool selected, bool textures )
{
	return false;
}

bool SMDExporter::ExportFile( WOR_tdst_World *world, const char *filename, const char *exportDir, bool selected, bool textures )
{
	//TODO: bones + animations aren't currently dealt with here, I'd decided it wasn't yet worth the effort so I'll look at it later

	assert( exportDir != nullptr );

	TAB_PFtable_RemoveHoles( &world->st_AllWorldObjects );
	TAB_tdst_PFelem *element    = TAB_pst_PFtable_GetFirstElem( &world->st_AllWorldObjects );
	TAB_tdst_PFelem *endElement = TAB_pst_PFtable_GetLastElem( &world->st_AllWorldObjects );
	for ( unsigned int i = 0; element <= endElement; element++, i++ )
	{
		OBJ_tdst_GameObject *gameObject = ( OBJ_tdst_GameObject * ) element->p_Pointer;
		if ( gameObject == nullptr || ( selected && !( gameObject->ul_EditorFlags & OBJ_C_EditFlags_Selected ) ) )
		{
			continue;
		}

		OBJ_tdst_Base *base = gameObject->pst_Base;
		if ( base == nullptr )
		{
			continue;
		}

		GEO_tdst_Object *graphics;
		MAT_tdst_Multi *material;
		if ( OBJ_b_TestIdentityFlag( gameObject, OBJ_C_IdentityFlag_Visu ) )
		{
			graphics = ( GEO_tdst_Object * ) base->pst_Visu->pst_Object;
			if ( graphics == nullptr || graphics->st_Id.i->ul_Type != GRO_Geometric )
			{
				continue;
			}

			material = ( MAT_tdst_Multi * ) base->pst_Visu->pst_Material;
			if ( material == nullptr || material->st_Id.i->ul_Type != GRO_MaterialMulti )
			{
				continue;
			}
		}
		else
		{
			// nothing we can convert to SMD (yet)
			continue;
		}

		if ( graphics->l_NbPoints <= 0 || graphics->l_NbElements <= 0 )
		{
			continue;
		}

		// assumed to be a dir here, due to the fact that an SMD can't hold multiple meshes!
		std::string outname = std::string( exportDir ) + "/" + std::string( gameObject->sz_Name ) + ".smd";

		FILE *out = fopen( outname.c_str(), "w" );
		if ( out == nullptr )
		{
			std::string msg = "Failed to create SMD file (" + outname + ")!";
			LINK_PrintStatusMsg( ( char * ) msg.c_str() );
			continue;
		}

		fprintf( out, "version 1\n" );

		//TEMP: we'll worry about bones after geom
		fprintf( out, "nodes\n" );
		fprintf( out, "0 \"root\" -1\n" );
		fprintf( out, "end\n" );

		fprintf( out, "triangles\n" );
		for ( unsigned int j = 0; j < graphics->l_NbElements; ++j )
		{
			const GEO_tdst_ElementIndexedTriangles *element = &graphics->dst_Element[ j ];

			for ( unsigned int k = 0; k < element->l_NbTriangles; ++k )
			{
				const char *materialName = GRO_sz_Struct_GetName( &material->st_Id );
				assert( materialName != nullptr );
				fprintf( out, "%s.bmp\n", materialName );

				const GEO_tdst_IndexedTriangle *tri = &element->dst_Triangle[ k ];
				for ( unsigned int l = 0; l < 3; ++l )
				{
					// bone index
					fprintf( out, "0 " );
					// position
					fprintf( out, "%f %f %f ",
					         graphics->dst_Point[ tri->auw_Index[ l ] ].x,
					         graphics->dst_Point[ tri->auw_Index[ l ] ].y,
					         graphics->dst_Point[ tri->auw_Index[ l ] ].z );
					// normal
					MATH_tdst_Vector normal;
					BAS_ZERO( &normal, sizeof( normal ) );
					if ( graphics->dst_PointNormal != nullptr )
					{
						normal = graphics->dst_PointNormal[ tri->auw_Index[ l ] ];
					}
					fprintf( out, "%f %f %f ", normal.x, normal.y, normal.z );
					// uv
					GEO_tdst_UV uv;
					BAS_ZERO( &uv, sizeof( uv ) );
					if ( graphics->dst_UV != nullptr )
					{
						uv = graphics->dst_UV[ tri->auw_UV[ l ] ];
					}
					fprintf( out, "%f %f ", uv.fU, uv.fV );
					// extras
					fprintf( out, "0 0 0\n" );
				}
			}
		}
		fprintf( out, "end\n" );

		fclose( out );
	}

	if ( textures )
	{
		ExportTextures( exportDir );
	}

	return true;
}
