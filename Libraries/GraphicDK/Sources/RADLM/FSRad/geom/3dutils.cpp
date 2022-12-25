// ---------------------------------------------------------------------------------------------------------------------------------
//  ____      _       _   _ _                          
// |___ \    | |     | | (_) |                         
//   __) | __| |_   _| |_ _| |___      ___ _ __  _ __  
//  |__ < / _` | | | | __| | / __|    / __| '_ \| '_ \ 
//  ___) | (_| | |_| | |_| | \__ \ _ | (__| |_) | |_) |
// |____/ \__,_|\__,_|\__|_|_|___/(_) \___| .__/| .__/ 
//                                        | |   | |    
//                                        |_|   |_|    
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

//#include <Windows.h>
#include "Precomp.h"

#ifdef ACTIVE_EDITORS

#include "geom.h"
using namespace geom;

// ---------------------------------------------------------------------------------------------------------------------------------

static	const	float	EPSILON = 1.0e-5f;

// ---------------------------------------------------------------------------------------------------------------------------------
// Returns the closest point to 'p' on the line-segment defined by a & b. Sets 'edge' to true if result is one of the endpoints
// ---------------------------------------------------------------------------------------------------------------------------------

Point3	geom::closestPointOnLineSegment(const Point3 &a, const Point3 &b, const Point3 &p, bool &edge)
{
	Vector3 c = p - a;
	Vector3 v = b - a;
	v.normalize();

	edge = false;
	float	t = v ^ c;
	if (t < 0) return a;

	float	d = b.distance(a);
	if (t > d) return b;

	edge = true;
	return a + v * t;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Returns the closest point to 'p' on the line defined by 'a' and 'b'
// ---------------------------------------------------------------------------------------------------------------------------------

Point3	geom::closestPointOnLine(const Point3 &a, const Point3 &b, const Point3 &p)
{
	Vector3 c = p - a;
	Vector3 v = b - a;
	v.normalize();

	float	t = v ^ c;
	float	d = b.distance(a);
	return a + v * t;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Returns true/false if point 'p' is between 'a' and 'b'. This routine assumes that all three points are linear.
// ---------------------------------------------------------------------------------------------------------------------------------

bool	geom::isPointInsideEdge(const Point3 &a, const Point3 &b, const Point3 &p)
{
	return (Vector3(a-p) ^ Vector3(b-p)) <= 0;
}

// --------------------------------------------------------------------------------------------------------------------------------
// This code compliments of Tomas Moller, as part of his AABB-Triangle overlap test
// --------------------------------------------------------------------------------------------------------------------------------

bool	geom::planeBoxOverlap(const Vector3 &normal, const float d, const Point3 &radius)
{
	Point3	vmin, vmax;

	if(normal.x() > 0.0f)
	{
		vmin.x() = -radius.x();
		vmax.x() =  radius.x();
	}
	else
	{
		vmin.x() =  radius.x();
		vmax.x() = -radius.x();
	}

	if(normal.y() > 0.0f)
	{
		vmin.y() = -radius.y();
		vmax.y() =  radius.y();
	}
	else
	{
		vmin.y() =  radius.y();
		vmax.y() = -radius.y();
	}

	if(normal.z() > 0.0f)
	{
		vmin.z() = -radius.z();
		vmax.z() =  radius.z();
	}
	else
	{
		vmin.z() =  radius.z();
		vmax.z() = -radius.z();
	}

	if (normal.dot(vmin) + d >  0) return false;
	if (normal.dot(vmax) + d >= 0) return true;
	return false;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Simple AABB overlap test
// ---------------------------------------------------------------------------------------------------------------------------------

bool	geom::aabbOverlapTest(const Point3 & aMin, const Point3 & aMax, const Point3 & bMin, const Point3 & bMax)
{
	return	aMax.x() >= bMin.x() && aMin.x() <= bMax.x() &&           
		aMax.y() >= bMin.y() && aMin.y() <= bMax.y() &&           
		aMax.z() >= bMin.z() && aMin.z() <= bMax.z();
}

// ---------------------------------------------------------------------------------------------------------------------------------
// 3dutils.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

#endif // #ifdef ACTIVE_EDITORS