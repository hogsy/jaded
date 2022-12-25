/******************************************************************************************************************
	Ubi Pictures, 13/04/2006, Gilles Jaffier

	Define some constants and structures useful to max importers and exporters.

 ******************************************************************************************************************/


#pragma once


#define MAD_SCALE_IMPORT_FACTOR			100.0f
#define MAD_IMPORT_MAX_BONE_PER_VERT	3

// Flags for supressPrompts
#define MADEXP_DONOTINCLUDETEX		1  // One of these 2 flags must be set so
#define MADEXP_INCLUDETEX			2  // that suppressPrompts is TRUE.
#define MADEXP_SELONLY				4
#define MADEXP_NOSKIN				8


struct MadImportParam
{
	BOOL		mb_Merge;
	BOOL		mb_MergeTexture;
	char		*msz_TexturePath;
	BOOL		mb_SetFullOpacity;
	BOOL		mb_ImportMultiTexMat;
};