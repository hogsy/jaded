// ---------------------------------------------------------------------------------------------------------------------------------
//  ____      _       _   _ _
// |___ \    | |     | | (_) |
//   __) | __| |_   _| |_ _| |___
//  |__ < / _` | | | | __| | / __|
//  ___) | (_| | |_| | |_| | \__ \
// |____/ \__,_|\__,_|\__|_|_|___/
//
// Utilitarian funcitons useful for 3D graphics
//
// Best viewed with 8-character tabs and (at least) 132 columns
//
// ---------------------------------------------------------------------------------------------------------------------------------
//
// Restrictions & freedoms pertaining to usage and redistribution of this software:
//
//  * This software is 100% free
//  * If you use this software (in part or in whole) you must credit the author.
//  * This software may not be re-distributed (in part or in whole) in a modified
//    form without clear documentation on how to obtain a copy of the original work.
//  * You may not use this software to directly or indirectly cause harm to others.
//  * This software is provided as-is and without warrantee. Use at your own risk.
//
// For more information, visit HTTP://www.FluidStudios.com
//
// ---------------------------------------------------------------------------------------------------------------------------------
// Originally created on 12/20/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_GEOM_3DUTILS
#define _GEOM_3DUTILS

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "common.h"
#include "../fstl/fstl.h"
#include "geom.h"

GEOM_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------------------------------------------------------------

Point3		closestPointOnLineSegment(const Point3 &a, const Point3 &b, const Point3 &p, bool &edge);
Point3		closestPointOnLine(const Point3 &a, const Point3 &b, const Point3 &p);
bool		isPointInsideEdge(const Point3 &a, const Point3 &b, const Point3 &p);
bool		planeBoxOverlap(const Vector3 &normal, const GEOM_TYPE d, const Point3 &radius);
bool		aabbOverlapTest(const Point3 & aMin, const Point3 & aMax, const Point3 & bMin, const Point3 & bMax);

// ---------------------------------------------------------------------------------------------------------------------------------

GEOM_NAMESPACE_END

#endif // _GEOM_3DUTILS
// ---------------------------------------------------------------------------------------------------------------------------------
// 3dutils - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

