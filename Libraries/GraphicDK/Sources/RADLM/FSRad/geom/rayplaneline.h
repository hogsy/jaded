// ---------------------------------------------------------------------------------------------------------------------------------
//                         _                  _ _
//                        | |                | (_)
//  _ __  __ _ _   _ _ __ | | __ _ _ __   ___| |_ _ __   ___
// | '__|/ _` | | | | '_ \| |/ _` | '_ \ / _ \ | | '_ \ / _ \
// | |  | (_| | |_| | |_) | | (_| | | | |  __/ | | | | |  __/
// |_|   \__,_|\__, | .__/|_|\__,_|_| |_|\___|_|_|_| |_|\___|
//              __/ | |
//             |___/|_|
//
// Description:
//
//   Generic class to represent rays, planes and lines of N dimensions
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   06/12/2001 by Paul Nettle: Original creation
//
// Restrictions & freedoms pertaining to usage and redistribution of this software:
//
//   This software is 100% free. If you use this software (in part or in whole) you must credit the author. This software may not be
//   re-distributed (in part or in whole) in a modified form without clear documentation on how to obtain a copy of the original
//   work. You may not use this software to directly or indirectly cause harm to others. This software is provided as-is and without
//   warrantee -- Use at your own risk. For more information, visit HTTP://www.FluidStudios.com/
//
// Copyright 2001, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
//
// Note that necessary values (the 'D' value from the plane equation) and a normalized version of the direction vector are
// calculated on-demand and cached to improve performance.
//
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_GEOM_RAYPLANELINE
#define _GEOM_RAYPLANELINE

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "common.h"
#include "../fstl/fstl.h"

GEOM_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------------------------------------------------------------

template <unsigned int N = 3>
class	Ray
{
public:
	// Construction/Destruction

				Ray() {}
				Ray(const Matrix<N, 1> &origin, const Matrix<N, 1> &vector)
					:_origin(origin), _vector(vector), _nCalculated(false), _lCalculated(false), _dCalculated(false) {}
virtual				~Ray() {}

	// Operators

inline		Ray &		operator=(const Ray &r)
				{
					_origin = r._origin;
					_vector = r._vector;
					_normal = r._normal;
					_length = r._length;
					_D = r._D;
					_nCalculated = r._nCalculated;
					_lCalculated = r._lCalculated;
					_dCalculated = r._dCalculated;
					return *this;
				}

	// Implementation

inline	const	GEOM_TYPE	distance(const Matrix<N, 1> &point) const
				{
					GEOM_TYPE	t;
					intersect(Ray(point, -normal()), t);
					return t;
				}

inline	const	GEOM_TYPE	halfplane(const Matrix<N, 1> &point) const
				{
					return vector() ^ (point - origin());
				}

inline	const	bool		intersect(const Ray &r, GEOM_TYPE &time) const
				{
					time = 0;
					GEOM_TYPE	denom = normal() ^ r.normal();

					if (denom == 0) return false;

					GEOM_TYPE	numer = normal() ^ r.origin();

					time = -((numer + D()) / denom);

					return true;
				}

inline	const	Matrix<N, 1>	closest(const Matrix<N, 1> &point) const
				{
					GEOM_TYPE	t = distance(point);
					return point - normal() * t;
				}

inline	const	GEOM_TYPE	length() const
				{
					if (!_lCalculated)
					{
						_length = vector().length();
						_lCalculated = true;
					}

					return _length;
				}

inline		void		setLength(const GEOM_TYPE len)
				{
					// If it's already normalized, just multiply

					if (_nCalculated)	_vector = _normal * len;
					else			_vector.setLength(len);

					_length = len;
					_lCalculated = true;
				}

inline	const	GEOM_TYPE	D() const
				{
					if (!_dCalculated)
					{
						_D = -(origin() ^ normal());
						_dCalculated = true;
					}

					return _D;
				}

inline	const	Matrix<N, 1> &	normal() const
				{
					if (!_nCalculated)
					{
						_normal = _vector;
						_normal.normalize();
						_nCalculated = true;
					}

					return _normal;
				}

inline	const	Matrix<N, 1>	end() const
				{
					return _origin + _vector;
				}

inline	const	Matrix<N, 1>	end(const GEOM_TYPE len) const
				{
					return _origin + normal() * len;
				}

inline		void		flush() const
				{
					_nCalculated = false;
					_lCalculated = false;
					_dCalculated = false;
				}

	// Accessors

inline	const	Matrix<N, 1> &	origin() const		{                                                                  return _origin;}
inline		Matrix<N, 1> &	origin()		{                                            _dCalculated = false; return _origin;}
inline	const	Matrix<N, 1> &	vector() const		{                                                                  return _vector;}
inline		Matrix<N, 1> &	vector()		{_nCalculated = false; _lCalculated = false; _dCalculated = false; return _vector;}

private:
	// Data members

		Matrix<N, 1>	_origin;		// Origin of Ray/Plane/Line
		Matrix<N, 1>	_vector;		// Direction of Ray/Plane
	mutable	Matrix<N, 1>	_normal;		// Direction of Ray/Plane (normalized)
	mutable	GEOM_TYPE	_length;		// Length of the ray
	mutable	GEOM_TYPE	_D;			// 'D' of the plane equation

	mutable	bool		_nCalculated;
	mutable	bool		_lCalculated;
	mutable	bool		_dCalculated;
};

// ---------------------------------------------------------------------------------------------------------------------------------
// For convenience
// ---------------------------------------------------------------------------------------------------------------------------------

typedef	Ray<2>			Ray2;
typedef	Ray<3>			Ray3;
typedef	Ray<4>			Ray4;
typedef	Ray<2>			Plane2;
typedef	Ray<3>			Plane3;
typedef	Ray<4>			Plane4;
typedef	Ray<2>			Line2;
typedef	Ray<3>			Line3;
typedef	Ray<4>			Line4;

typedef	fstl::array<Ray2>	Ray2Array;
typedef	fstl::array<Ray3>	Ray3Array;
typedef	fstl::array<Ray4>	Ray4Array;
typedef	fstl::array<Plane2>	Plane2Array;
typedef	fstl::array<Plane3>	Plane3Array;
typedef	fstl::array<Plane4>	Plane4Array;
typedef	fstl::array<Line2>	Line2Array;
typedef	fstl::array<Line3>	Line3Array;
typedef	fstl::array<Line4>	Line4Array;


GEOM_NAMESPACE_END
#endif // _GEOM_RAYPLANELINE
// ---------------------------------------------------------------------------------------------------------------------------------
// RayPlaneLine - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

