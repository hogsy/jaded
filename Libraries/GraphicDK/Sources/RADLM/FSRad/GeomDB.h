// ---------------------------------------------------------------------------------------------------------------------------------
//   _____                      _____  ____      _     
//  / ____|                    |  __ \|  _ \    | |    
// | |  __  ___  ___  _ __ ___ | |  | | |_) |   | |__  
// | | |_ |/ _ \/ _ \| '_ ` _ \| |  | |  _ <    | '_ \ 
// | |__| |  __/ (_) | | | | | | |__| | |_) | _ | | | |
//  \_____|\___|\___/|_| |_| |_|_____/|____/ (_)|_| |_|
//                                                     
//                                                     
//
// Description:
//
//   Geometry database manager
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   08/03/2001 by Paul Nettle: Original creation
//
// Restrictions & freedoms pertaining to usage and redistribution of this software:
//
//   This software is 100% free. If you use this software (in part or in whole) you must credit the author. This software may not be
//   re-distributed (in part or in whole) in a modified form without clear documentation on how to obtain a copy of the original
//   work. You may not use this software to directly or indirectly cause harm to others. This software is provided as-is and without
//   warrantee -- Use at your own risk. For more information, visit HTTP://www.FluidStudios.com/
//
// Copyright 2002, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_H_GEOMDB
#define _H_GEOMDB

// ---------------------------------------------------------------------------------------------------------------------------------
// Forward declarations & required modules
// ---------------------------------------------------------------------------------------------------------------------------------

#include "RadPrim.h"
#include "RadPatch.h"
#include "RadLMap.h"

// ---------------------------------------------------------------------------------------------------------------------------------

typedef	struct	oct_file_texture
{
	unsigned int	id;				// texture id
	char		name[64];			// texture name
} sOctTexture;

typedef	fstl::array<sOctTexture>	sOctTextureArray;

// ---------------------------------------------------------------------------------------------------------------------------------

class	GeomDB
{
public:
	// Construction/Destruction

    					GeomDB();
virtual					~GeomDB();

	// Implementation		

    bool ReadGeomety(const geom::Color3 & defaultReflectivity);
    
	// Accessors			

inline		RadPrimList &		polys()			{return _polys;}
inline	const	RadPrimList &		polys() const		{return _polys;}
inline		RadPatchList &		lights()		{return _lights;}
inline	const	RadPatchList &		lights() const		{return _lights;}
inline		RadLMapArray &		lightmaps()		{return _lightmaps;}
inline	const	RadLMapArray &		lightmaps() const	{return _lightmaps;}
inline 		sOctTextureArray &	octTextures()		{return _octTextures;}
inline const	sOctTextureArray &	octTextures() const	{return _octTextures;}

private:
	// Data members

		RadPrimList		_polys;
		RadPatchList		_lights;
		RadLMapArray		_lightmaps;
		sOctTextureArray	_octTextures;
};

#endif // _H_GEOMDB
// ---------------------------------------------------------------------------------------------------------------------------------
// GeomDB.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
