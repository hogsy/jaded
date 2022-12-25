// ---------------------------------------------------------------------------------------------------------------------------------
//             _           _ _   _
//            (_)         (_) | (_)
//  _ __  _ __ _ _ __ ___  _| |_ ___   __ ___
// | '_ \| '__| | '_ ` _ \| | __| \ \ / // _ \
// | |_) | |  | | | | | | | | |_| |\ V /|  __/
// | .__/|_|  |_|_| |_| |_|_|\__|_| \_/  \___|
// | |
// |_|
//
// Generic N-dimensional primitive class for points, lines, polygons
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
// Originally created on 12/06/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_GEOM_PRIMITIVE
#define _GEOM_PRIMITIVE

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "common.h"
#include "../fstl/fstl.h"
#include "geom.h"

GEOM_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------------------------------------------------------------

class	Primitive
{
public:
	// Implementation

virtual		int			calcPrimaryAxisIndex() const;
virtual		void			setWorldTexture(const GEOM_TYPE uScale = static_cast<GEOM_TYPE>(1), const GEOM_TYPE vScale = static_cast<GEOM_TYPE>(1));
virtual	const	Point3			calcCenterOfMass() const;
virtual		void			calcPlane(const bool counterClock = true);
virtual		GEOM_TYPE		calcArea() const;
virtual		bool			inside(const Point3 &p, const GEOM_TYPE epsilon = static_cast<GEOM_TYPE>(0)) const;
virtual		bool			inside2(const Point3 &p, const GEOM_TYPE epsilon = static_cast<GEOM_TYPE>(0)) const;
virtual		bool			isConvex(const GEOM_TYPE epsilon = 1.0e-4) const;
virtual		bool			isPlanar(const GEOM_TYPE epsilon = 1.0e-4) const;
virtual		void			removeLinearVertices(const GEOM_TYPE epsilon = 1.0e-5);
virtual		Point3			closestPointOnPerimeter(const Point3 &p, Point3 &e0, Point3 &e1, bool &edgeFlag) const;
virtual		bool			intersectAABB(const Point3 &center, const Point3 &radius) const;
virtual		bool			bisect(const Plane3 & plane, Primitive & back);

	// Accessors

inline 		Point3Array &		xyz()			{return _xyz;}
inline const	Point3Array &		xyz()	const		{return _xyz;}
inline 		Point2Array &		uv()			{return _uv;}
inline const	Point2Array &		uv() const		{return _uv;}
inline 		Plane3 &		plane()			{return _plane;}
inline const	Plane3 &		plane()	const		{return _plane;}

protected:

	// Data

		Point3Array		_xyz;
		Point2Array		_uv;
		Plane3			_plane;
};

typedef	fstl::array<Primitive>		PrimitiveArray;
typedef	fstl::array<Primitive *>	PrimitivePointerArray;

GEOM_NAMESPACE_END

#endif // _GEOM_PRIMITIVE
// ---------------------------------------------------------------------------------------------------------------------------------
// primitive - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

