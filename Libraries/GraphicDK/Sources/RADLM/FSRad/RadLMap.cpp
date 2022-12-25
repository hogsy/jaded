// ---------------------------------------------------------------------------------------------------------------------------------
//  _____            _ _      __  __                                  
// |  __ \          | | |    |  \/  |                                 
// | |__) | __ _  __| | |    | \  / | __ _ _ __       ___ _ __  _ __  
// |  _  / / _` |/ _` | |    | |\/| |/ _` | '_ \     / __| '_ \| '_ \ 
// | | \ \| (_| | (_| | |____| |  | | (_| | |_) | _ | (__| |_) | |_) |
// |_|  \_\\__,_|\__,_|______|_|  |_|\__,_| .__/ (_) \___| .__/| .__/ 
//                                        | |            | |   | |    
//                                        |_|            |_|   |_|    
//
// Description:
//
//   Lightmap used for tracking light from the radiosity process
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   08/17/2001 by Paul Nettle: Original creation
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

#include "Precomp.h"

#ifdef ACTIVE_EDITORS

#include "RadLMap.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ---------------------------------------------------------------------------------------------------------------------------------

	RadLMap::RadLMap(const unsigned int w, const unsigned int h, const int id)
	: _width(w), _height(h), _id(id), _updated(false)
{
	init(w, h);
}

// ---------------------------------------------------------------------------------------------------------------------------------

	RadLMap::~RadLMap()
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadLMap::init(unsigned int w, unsigned int h)
{
	// Wipe the current data

	data().erase();

	// Fill 'er up with blanks

	geom::Color3	blank(0, 0, 0);
	data().populate(blank, w*h);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadLMap::applyGamma(const float gamma)
{
	unsigned int	sz = width() * height();
	geom::Color3 *	ptr = &data()[0];

	for (unsigned int j = 0; j < sz; ++j, ++ptr)
	{
		ptr->r() = 256 * static_cast<float>(pow(ptr->r() / 256, 1 / gamma));
		ptr->g() = 256 * static_cast<float>(pow(ptr->g() / 256, 1 / gamma));
		ptr->b() = 256 * static_cast<float>(pow(ptr->b() / 256, 1 / gamma));
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadLMap::clampSaturate()
{
	unsigned int	sz = width() * height();
	geom::Color3 *	ptr = &data()[0];

	for (unsigned int j = 0; j < sz; ++j, ++ptr)
	{
		if (ptr->r() > 255) ptr->r() = 255;
		else if (ptr->r() < 0) ptr->r() = 0;

		if (ptr->g() > 255) ptr->g() = 255;
		else if (ptr->g() < 0) ptr->g() = 0;

		if (ptr->b() > 255) ptr->b() = 255;
		else if (ptr->b() < 0) ptr->b() = 0;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadLMap::clampRetainColorRatio()
{
	unsigned int	sz = width() * height();
	geom::Color3 *	ptr = &data()[0];

	for (unsigned int i = 0; i < sz; ++i, ++ptr)
	{
		float	maximum = fstl::max(ptr->r(), fstl::max(ptr->g(), ptr->b()));

		if (maximum > 255.0f)
		{
			float	scalar = 255.0f / maximum;
			ptr->r() *= scalar;
			ptr->g() *= scalar;
			ptr->b() *= scalar;
		}

		if (ptr->r() < 0) ptr->r() = 0;
		if (ptr->g() < 0) ptr->g() = 0;
		if (ptr->b() < 0) ptr->b() = 0;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadLMap::addAmbient(const geom::Color3 & ambient)
{
	unsigned int	sz = width() * height();
	geom::Color3 *	ptr = &data()[0];

	for (unsigned int i = 0; i < sz; ++i, ++ptr)
	{
		*ptr += ambient;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadLMap::writeRaw(const fstl::string & folder)
{
	fstl::string	fname = folder;
	if (fname[fname.length() - 1] != '\\') fname += "\\";
	fname += fstl::string(id()) + " - " + fstl::string(width()) + "x" + fstl::string(height()) + ".raw";
	FILE *fp = fopen(fname.asArray(), "wb");

	if (fp)
	{
		for (unsigned int j = 0; j < width() * height(); ++j)
		{
			unsigned int	r = static_cast<unsigned int>(data()[j].r());
			unsigned int	g = static_cast<unsigned int>(data()[j].g());
			unsigned int	b = static_cast<unsigned int>(data()[j].b());
			fwrite(&r, 1, 1, fp);
			fwrite(&g, 1, 1, fp);
			fwrite(&b, 1, 1, fp);
		}
		fclose(fp);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// RadLMap.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

#endif // #ifdef ACTIVE_EDITORS