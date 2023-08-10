// Created by Mark "hogsy" Sowden, 2023 <hogsy@oldtimes-software.com>
// https://oldtimes-software.com/jaded/
// Purpose: General export interfaces for formats other than MAD

#include "Precomp.h"
#include "BIGfiles/BIGfat.h"
#include "TEXture/TEXfile.h"
#include "WOR.h"

namespace eng
{
	namespace wor
	{
		class Exporter
		{
		public:
			virtual bool ExportFile( WOR_tdst_World *world, const char *filename, const char *exportDir, unsigned char sel, bool textures ) = 0;

			static void ExportTextures( const char *outDir );
		};

		class SMDExporter : public Exporter
		{
			bool ExportFile( WOR_tdst_World *world, const char *filename, const char *exportDir, unsigned char sel, bool textures ) override;
		};

		class GLTFExporter : public Exporter
		{
			bool ExportFile( WOR_tdst_World *world, const char *filename, const char *exportDir, unsigned char sel, bool textures ) override;
		};
	}// namespace wor
}// namespace eng

void eng::wor::Exporter::ExportTextures( const char *outDir )
{
	for ( unsigned int i = 0; i < TEX_gst_GlobalList.l_NumberOfTextures; ++i )
	{
		TEX_tdst_Data *textureData = &TEX_gst_GlobalList.dst_Texture[ i ];
		unsigned int index         = BIG_ul_SearchKeyToFat( textureData->ul_Key );
		if ( index == BIG_C_InvalidKey )
			continue;

		const char *name = BIG_NameFile( index );
		assert( name != nullptr );
		if ( name == nullptr )
			continue;

		const char *extension = strrchr( name, '.' );
		assert( extension != nullptr );
		if ( extension == nullptr )
			continue;

		std::string outPath = outDir;
		if ( stricmp( extension, ".tex" ) == 0 )
		{
			TEX_tdst_File_Desc desc;
			TEX_l_File_GetInfoAndContent( textureData->ul_Key, &desc );
		}
		else if ( stricmp( extension, ".tga" ) == 0 )
		{
			TEX_tdst_File_Desc desc;
			TEX_l_File_GetInfoAndContent( textureData->ul_Key, &desc );
			outPath += "/" + std::string( name );
			TEX_l_File_SaveTga( ( char * ) outPath.c_str(), &desc );
		}
		else
		{
			// some unsupported type ... ??
			assert( 0 );
			continue;
		}
	}
}

bool eng::wor::GLTFExporter::ExportFile( WOR_tdst_World *world, const char *filename, const char *exportDir, unsigned char sel, bool textures )
{
	return false;
}

bool eng::wor::SMDExporter::ExportFile( WOR_tdst_World *world, const char *filename, const char *exportDir, unsigned char sel, bool textures )
{
	TAB_PFtable_RemoveHoles( &world->st_AllWorldObjects );
	TAB_tdst_PFelem *element    = TAB_pst_PFtable_GetFirstElem( &world->st_AllWorldObjects );
	TAB_tdst_PFelem *endElement = TAB_pst_PFtable_GetLastElem( &world->st_AllWorldObjects );
	for ( unsigned int i = 0; element <= endElement; element++, i++ )
	{
		OBJ_tdst_GameObject *gameObject = ( OBJ_tdst_GameObject * ) element->p_Pointer;
		assert( gameObject != nullptr );
		if ( gameObject == NULL )
			continue;
	}

	if ( textures )
	{
		ExportTextures( exportDir );
	}

	return true;
}
