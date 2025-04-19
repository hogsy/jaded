#pragma once

class WorldExporter
{
public:
	WorldExporter()  = default;
	~WorldExporter() = default;

	virtual bool ExportFile( WOR_tdst_World *world, const char *filename, const char *exportDir, unsigned char sel, bool textures ) = 0;

	static void ExportTextures( const char *outDir );
};

class MADExporter : public WorldExporter
{
public:
	bool ExportFile( WOR_tdst_World *world, const char *filename, const char *exportDir, unsigned char sel, bool textures ) override;
};

class SMDExporter : public WorldExporter
{
public:
	SMDExporter()  = default;
	~SMDExporter() = default;

	bool ExportFile( WOR_tdst_World *world, const char *filename, const char *exportDir, unsigned char sel, bool textures ) override;
};

class GLTFExporter : public WorldExporter
{
public:
	bool ExportFile( WOR_tdst_World *world, const char *filename, const char *exportDir, unsigned char sel, bool textures ) override;
};
