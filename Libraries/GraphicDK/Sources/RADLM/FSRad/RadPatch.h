// ---------------------------------------------------------------------------------------------------------------------------------
//  _____            _ _____        _        _         _     
// |  __ \          | |  __ \      | |      | |       | |    
// | |__) | __ _  __| | |__) | __ _| |_  ___| |__     | |__  
// |  _  / / _` |/ _` |  ___/ / _` | __|/ __| '_ \    | '_ \ 
// | | \ \| (_| | (_| | |    | (_| | |_| (__| | | | _ | | | |
// |_|  \_\\__,_|\__,_|_|     \__,_|\__|\___|_| |_|(_)|_| |_|
//                                                           
//                                                           
//
// Description:
//
//   Radiosity patch class
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   09/22/2001 by Paul Nettle: Original creation
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

#ifndef	_H_RADPATCH
#define _H_RADPATCH

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "geom/geom.h"

// ---------------------------------------------------------------------------------------------------------------------------------

class	RadPatch
{
public:
	// Construction/Destruction

	// Operators

	// Implementation

	// Accessors

inline		float &			area()			{return _area;}
inline	const	float			area() const		{return _area;}
inline		geom::Color3 &		energy()		{return _energy;}
inline	const	geom::Color3 &		energy() const		{return _energy;}
inline		geom::Point3 &		origin()		{return _origin;}
inline	const	geom::Point3 &		origin() const		{return _origin;}
inline		geom::Plane3 &		plane()			{return _plane;}
inline	const	geom::Plane3 &		plane() const		{return _plane;}
inline		bool			isPointLight()		{return area() == 0;}

private:
	// Data members

		float			_area;
		geom::Color3		_energy;
		geom::Point3		_origin;
		geom::Plane3		_plane;
};
typedef	fstl::array<RadPatch>		RadPatchArray;
typedef	fstl::list<RadPatch>		RadPatchList;

#endif // _H_RADPATCH
// ---------------------------------------------------------------------------------------------------------------------------------
// RadPatch.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
