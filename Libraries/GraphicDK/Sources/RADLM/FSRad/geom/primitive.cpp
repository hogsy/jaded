// ---------------------------------------------------------------------------------------------------------------------------------
//             _           _ _   _                                 
//            (_)         (_) | (_)                                
//  _ __  _ __ _ _ __ ___  _| |_ ___   __ ___      ___ _ __  _ __  
// | '_ \| '__| | '_ ` _ \| | __| \ \ / // _ \    / __| '_ \| '_ \ 
// | |_) | |  | | | | | | | | |_| |\ V /|  __/ _ | (__| |_) | |_) |
// | .__/|_|  |_|_| |_| |_|_|\__|_| \_/  \___|(_) \___| .__/| .__/ 
// | |                                                | |   | |    
// |_|                                                |_|   |_|    
//
// Description:
//
//   Octree class -- need I say more?
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   06/25/2001 by Paul Nettle: Original creation
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

//#include <Windows.h>
#include "Precomp.h"

#ifdef ACTIVE_EDITORS

#include "geom.h"
using namespace geom;

// ---------------------------------------------------------------------------------------------------------------------------------

static	const	float	EPSILON = 1.0e-5f;

// ---------------------------------------------------------------------------------------------------------------------------------

int	Primitive::calcPrimaryAxisIndex() const
{
	// Calculate |normal|

	Vector3	absNormal = plane().normal();
	absNormal.abs();

	// Primary axis == X

	if (absNormal.x() >= absNormal.y() && absNormal.x() >= absNormal.z())
	{
		return 0;
	}

	// Primary axis == Y

	else if (absNormal.y() >= absNormal.x() && absNormal.y() >= absNormal.z())
	{
		return 1;
	}

	// Primary axis == Z

	else
	{
		return 2;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	Primitive::setWorldTexture(const float uScale, const float vScale)
{
	// Primary axis index

	int	paIndex = calcPrimaryAxisIndex();

	// Primary axis == X

	if (paIndex == 0)
	{
		for (unsigned int i = 0; i < uv().size(); ++i)
		{
	        	uv()[i].x() =  xyz()[i].z() * uScale;
			uv()[i].y() = -xyz()[i].y() * vScale;
		}
	}

	// Primary axis == Y

	else if (paIndex == 1)
	{
		for (unsigned int i = 0; i < uv().size(); ++i)
		{
	        	uv()[i].x() =  xyz()[i].x() * uScale;
			uv()[i].y() = -xyz()[i].z() * vScale;
		}
	}

	// Primary axis == Z

	else if (paIndex == 2)
	{
		for (unsigned int i = 0; i < uv().size(); ++i)
		{
	        	uv()[i].x() =  xyz()[i].x() * uScale;
			uv()[i].y() = -xyz()[i].y() * vScale;
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

const	Point3	Primitive::calcCenterOfMass() const
{
	Point3	center(0.0f, 0.0f, 0.0f);
	if (xyz().size() < 1) return center;

	for (unsigned int i = 0; i < xyz().size(); ++i)
	{
		center += xyz()[i];
	}

	return center / static_cast<float>(xyz().size());
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	Primitive::calcPlane(const bool counterClock)
{
	plane().origin() = xyz()[0];
	Vector3	v0 = xyz()[1] - xyz()[0];
	Vector3	v1 = xyz()[2] - xyz()[1];
	plane().vector() = v1 % v0;
	if (!counterClock) plane().vector() = -plane().vector();
}

// ---------------------------------------------------------------------------------------------------------------------------------
// This calcArea() routine works for convex & concave polygons. It was adapted from a 2D algorithm presented in Computer Graphics
// Principles & Practice 2ed (Foley/vanDam/Feiner/Hughes) p. 477
// ---------------------------------------------------------------------------------------------------------------------------------

float	Primitive::calcArea() const
{
	float	xyArea = 0.0f;
	float	yzArea = 0.0f;
	float	zxArea = 0.0f;

	unsigned int	v0 = xyz().size() - 1;
	for (unsigned int v1 = 0; v1 < xyz().size(); v0 = v1, ++v1)
	{
		const Point3	&p0 = xyz()[v0];
		const Point3	&p1 = xyz()[v1];

		xyArea += (p0.y() + p1.y()) * (p1.x() - p0.x()) / 2.0f;
		yzArea += (p0.z() + p1.z()) * (p1.y() - p0.y()) / 2.0f;
		zxArea += (p0.x() + p1.x()) * (p1.z() - p0.z()) / 2.0f;
	}

	return static_cast<float>(sqrt(xyArea * xyArea + yzArea * yzArea + zxArea * zxArea));
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	Primitive::inside(const Point3 &p, const float epsilon) const
{
	int	pos = 0;
	int	neg = 0;
	Point3	center = calcCenterOfMass();
	Point3	normal = plane().normal();

	unsigned int	v0 = xyz().size() - 1;
	for (unsigned int v1 = 0; v1 < xyz().size(); v0 = v1, ++v1)
	{
		const Point3	&p0 = xyz()[v0];
		const Point3	&p1 = xyz()[v1];

		// Generate a normal for this edge

		Vector3	n = (p1 - p0) % normal;

		// Which side of this edge-plane is the point?

		float	halfPlane = (p ^ n) - (p0 ^ n);

		// Keep track of positives & negatives (but not zeros -- which means it's on the edge)

		if (halfPlane > epsilon) pos++;
		else if (halfPlane < -epsilon) neg++;

		// Early-out

		if (pos && neg) return false;
	}

	// If they're ALL positive, or ALL negative, then it's inside

	if (!pos || !neg) return true;

	// Must not be inside, because some were pos and some were neg

	return false;

}

// ---------------------------------------------------------------------------------------------------------------------------------
// This one is less accurate and slower, but considered "standard"
// ---------------------------------------------------------------------------------------------------------------------------------

bool	Primitive::inside2(const Point3 &p, const float epsilon) const
{
	float	total = -2.0f * fstl::pi<float>();

	Point3	p0 = p - xyz()[xyz().size() - 1];
	p0.normalize();

	for (unsigned int i = 0; i < xyz().size(); ++i)
	{
		Point3	p1 = p - xyz()[i];
		p1.normalize();
		float	t = p0 ^ p1;
		// Protect acos() input
		if (t < -1) t = -1;
		if (t > 1) t = 1;
		total += static_cast<float>(acos(t));
		p0 = p1;
	}

	if (fabs(total) > epsilon) return false;
	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	Primitive::isConvex(const float epsilon) const
{
	// Center point

	Point3	center = calcCenterOfMass();

	// Visit each edge, make a plane, and verify that all vertices are on the same side of the plane

	int	v0 = xyz().size() - 1;
	for (unsigned int v1 = 0; v1 < xyz().size(); v0 = v1, ++v1)
	{
		const Point3 &	p0 = xyz()[v0];
		const Point3 &	p1 = xyz()[v1];

		// Generate an edge normal

		Plane3	edgePlane(p0, (p1 - p0) % plane().normal());

		// Make sure it points inward

		if (edgePlane.distance(center) < 0) edgePlane.vector() = -edgePlane.vector();

		// Test all vertices against this plane

		for (unsigned int i = 0; i < xyz().size(); ++i)
		{
			// Don't bother testing against the same points tha formed the plane

			if (i == v0 || i == v1) continue;

			// Is this point on the wrong side of the plane?

			float	dist  = edgePlane.distance(xyz()[i]);
			if (dist < -epsilon) return false;
		}
	}

	// If we made it here, we're convex

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	Primitive::isPlanar(const float epsilon) const
{
	// Visit each vertex and verify that it is on the plane

	for (unsigned int i = 0; i < xyz().size(); ++i)
	{
		const Point3 &	p = xyz()[i];
		if (plane().distance(p) > epsilon) return false;
	}

	// If we made it here, we're planar

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	Primitive::removeLinearVertices(const float epsilon)
{
	if (xyz().size() < 3) return;

	// We wind around the polygon, looking for linear edges. Note that the edge (n-1, 0, 1) to be a linear edge, since most
	// ngons are fanned from the base (0) vertex, and removing that vertex would not be good. :)

	geom::Point3Array	newXYZ;
	geom::Point2Array	newUV;

	newXYZ += xyz()[0];
	newUV += uv()[0];

	for (unsigned int v0 = 0; v0 < xyz().size() - 1; ++v0)
	{
		unsigned int	v1 = v0 + 1;
		unsigned int	v2 = v1 + 1; if (v2 >= xyz().size()) v2 = 0;

		geom::Vector3	longEdge = xyz()[v1] - xyz()[v0];
		longEdge.normalize();
		geom::Vector3	shrtEdge = xyz()[v2] - xyz()[v1];
		shrtEdge.normalize();

		if ((longEdge ^ shrtEdge) <= (1-epsilon))
		{
			newXYZ += xyz()[v1];
			newUV += uv()[v1];
		}
	}

	xyz() = newXYZ;
	uv() = newUV;
}

// ---------------------------------------------------------------------------------------------------------------------------------

Point3	Primitive::closestPointOnPerimeter(const Point3 &p, Point3 &e0, Point3 &e1, bool &edgeFlag) const
{
	bool	found = false;
	float	closestDistance = 0.0f;
	Point3	closestPoint = Point3::zero();
	Point3	closestP0, closestP1;
	int	closestIndex;

	unsigned int	v0 = xyz().size() - 1;
	int	index = 0;
	for (unsigned int v1 = 0; v1 < xyz().size(); ++v1, index++)
	{
		const Point3	&p0 = xyz()[v0];
		const Point3	&p1 = xyz()[v1];

		bool	edge;
		Point3	cp = closestPointOnLineSegment(p0, p1, p, edge);
		float	d = cp.distance(p);

		if (!found || d < closestDistance)
		{
			closestDistance = d;
			closestPoint = cp;
			closestP0 = p0;
			closestP1 = p1;
			edgeFlag = edge;
			closestIndex = index;
			found = true;
		}

		v0 = v1;
	}

	if (!edgeFlag)
	{
		int	a = closestIndex - 1; if (a < 0) a = xyz().size() - 1;
		int	b = closestIndex + 1; if (b >= static_cast<int>(xyz().size())) b = 0;
		e0 = xyz()[a];
		e1 = xyz()[b];
	}
	else
	{
		e0 = closestP0;
		e1 = closestP1;
	}

	return closestPoint;
}

// --------------------------------------------------------------------------------------------------------------------------------
// This code comes with compliments of Tomas Moller's AABB-Triangle intersection test
// --------------------------------------------------------------------------------------------------------------------------------

bool	Primitive::intersectAABB(const Point3 &center, const Point3 &radius) const
{
	// This only works for triangles!

	assert(xyz().size() == 3);

	/*======================== X-tests ========================*/
	#define AXISTEST_X01(a, b, fa, fb)			   \
		p0 = a*v0.y() - b*v0.z();			   \
		p2 = a*v2.y() - b*v2.z();			   \
		if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
		rad = fa * radius.y() + fb * radius.z();           \
		if(min>rad || max<-rad) return false;

	#define AXISTEST_X2(a, b, fa, fb)			   \
		p0 = a*v0.y() - b*v0.z();			   \
		p1 = a*v1.y() - b*v1.z();			   \
		if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
		rad = fa * radius.y() + fb * radius.z();           \
		if(min>rad || max<-rad) return false;

	/*======================== Y-tests ========================*/
	#define AXISTEST_Y02(a, b, fa, fb)			   \
		p0 = -a*v0.x() + b*v0.z();		      	   \
		p2 = -a*v2.x() + b*v2.z();	       	       	   \
		if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
		rad = fa * radius.x() + fb * radius.z();           \
		if(min>rad || max<-rad) return false;

	#define AXISTEST_Y1(a, b, fa, fb)			   \
		p0 = -a*v0.x() + b*v0.z();		      	   \
		p1 = -a*v1.x() + b*v1.z();	     	       	   \
		if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
		rad = fa * radius.x() + fb * radius.z();           \
		if(min>rad || max<-rad) return false;

	/*======================== Z-tests ========================*/

	#define AXISTEST_Z12(a, b, fa, fb)			   \
		p1 = a*v1.x() - b*v1.y();			   \
		p2 = a*v2.x() - b*v2.y();			   \
		if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
		rad = fa * radius.x() + fb * radius.y();           \
		if(min>rad || max<-rad) return false;

	#define AXISTEST_Z0(a, b, fa, fb)			   \
		p0 = a*v0.x() - b*v0.y();			   \
		p1 = a*v1.x() - b*v1.y();			   \
		if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
		rad = fa * radius.x() + fb * radius.y();           \
		if(min>rad || max<-rad) return false;

	// 1) first test overlap in the {x,y,z}-directions
	//    find min, max of the triangle each direction, and test for overlap in
	//    that direction -- this is equivalent to testing a minimal AABB around
	//    the triangle against the AABB - note that we also calculate v0/v1/v2,
	//    our vertices (relative the center of the box)

	Point3	v0;
	Point3	v1;
	Point3	v2;
	{
		v0.x() = xyz()[0].x() - center.x();
		v1.x() = xyz()[1].x() - center.x();
		v2.x() = xyz()[2].x() - center.x();
		if (fstl::min(v0.x(), fstl::min(v1.x(), v2.x())) >  radius.x()) return false;
		if (fstl::max(v0.x(), fstl::max(v1.x(), v2.x())) < -radius.x()) return false;

		v0.z() = xyz()[0].z() - center.z();
		v1.z() = xyz()[1].z() - center.z();
		v2.z() = xyz()[2].z() - center.z();
		if (fstl::min(v0.z(), fstl::min(v1.z(), v2.z())) >  radius.z()) return false;
		if (fstl::max(v0.z(), fstl::max(v1.z(), v2.z())) < -radius.z()) return false;

		v0.y() = xyz()[0].y() - center.y();
		v1.y() = xyz()[1].y() - center.y();
		v2.y() = xyz()[2].y() - center.y();
		if (fstl::min(v0.y(), fstl::min(v1.y(), v2.y())) >  radius.y()) return false;
		if (fstl::max(v0.y(), fstl::max(v1.y(), v2.y())) < -radius.y()) return false;
	}

	// Recalc the plane equation since we moved the polygon (relative center)

	float	d = -plane().normal().dot(v0);

	// 2) test if the box intersects the plane of the triangle
	
	if(!planeBoxOverlap(plane().normal(), d, radius)) return false;
	
	// 3) crossproduct(edge from tri, {x,y,z}-directin) this gives 3x3=9 more tests 
	
	{
		Vector3	e0 = v1 - v0;
		float	fex = static_cast<float>(fabs(e0.x()));
		float	fey = static_cast<float>(fabs(e0.y()));
		float	fez = static_cast<float>(fabs(e0.z()));
		float	p0,p1,p2,rad,min,max;
		AXISTEST_X01(e0.z(), e0.y(), fez, fey);
		AXISTEST_Y02(e0.z(), e0.x(), fez, fex);
		AXISTEST_Z12(e0.y(), e0.x(), fey, fex);
	}
	
	{
		Vector3	e1 = v2 - v1;
		float	fex = static_cast<float>(fabs(e1.x()));
		float	fey = static_cast<float>(fabs(e1.y()));
		float	fez = static_cast<float>(fabs(e1.z()));
		float	p0,p1,p2,rad,min,max;
		AXISTEST_X01(e1.z(), e1.y(), fez, fey);
		AXISTEST_Y02(e1.z(), e1.x(), fez, fex);
		AXISTEST_Z0 (e1.y(), e1.x(), fey, fex);
	}
	
	{
		Vector3	e2 = v0 - v2;
		float	fex = static_cast<float>(fabs(e2.x()));
		float	fey = static_cast<float>(fabs(e2.y()));
		float	fez = static_cast<float>(fabs(e2.z()));
		float	p0,p1,p2,rad,min,max;
		AXISTEST_X2 (e2.z(), e2.y(), fez, fey);
		AXISTEST_Y1 (e2.z(), e2.x(), fez, fex);
		AXISTEST_Z12(e2.y(), e2.x(), fey, fex);
	}
	
	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Routine bisects primitive to the positive side of the plane. The portion of the primitive on the negative side is returned.
//
// Only the XYZ portion of the primitive is considered. All other data is left in an unknown state, except the plane of the returned
// primitive, which is calculated.
// ---------------------------------------------------------------------------------------------------------------------------------

bool	Primitive::bisect(const Plane3 & clipPlane, Primitive & back)
{
	// First, classify all points. This allows us to avoid any bisection if possible

	fstl::floatArray	classifications;
	classifications.reserve(xyz().size());
	{
		bool	pos = false;
		bool	neg = false;

		for (unsigned int i = 0; i < xyz().size(); ++i)
		{
			// Get the distance from the plane to the vertex

			float	dist = clipPlane.distance(xyz()[i]);

			// Classify the point

			if (dist > -EPSILON && dist < EPSILON)	dist = 0.0f;
			else if (dist > 0)			pos = true;
			else					neg = true;

			// Keep track

			classifications += dist;
		}

		// Special (and necessary) case:
		//
		// If the polygon rests on the plane, it could be associated with either side (or neither side)... so we pick one.
		// The only criteria we have for this choice is to use the normals....
		//
		// The specific case ("dot product > 0 == associate with front") was chosen for cases in the octree -- a polygon facing
		// the interior of a node, but resting on an outer-most-node boundary. We want it associated with the node, not thrown
		// out completely.

		if (!pos && !neg)
		{
			// Associate with front

			if ((plane().vector() ^ clipPlane.vector()) > 0)
			{
				pos = true;
			}

			// Associate with back

			else
			{
				neg = true;
			}
		}

		// If no negative points, return an empty negative polygon

		if (!neg)
		{
			back.xyz().erase();
			back.uv().erase();
			return true;
		}

		// If no positive points, return self as negative polygon

		else if (!pos)
		{
			back = *this;
			xyz().erase();
			uv().erase();
			return true;
		}

	}

	// Our result is stored here

	back = *this;
	Point3Array &	negXYZ = back.xyz();
	Point2Array &	negUV  = back.uv();
	negXYZ.erase();
	negXYZ.reserve(xyz().size());
	negUV.erase();
	negUV.reserve(xyz().size());

	Point3Array	posXYZ;
	Point2Array	posUV;
	posXYZ.reserve(xyz().size());
	posUV.reserve(xyz().size());

	// Visit all points in the list, adding points that are on the front-side of the current plane. If an edge bisects the plane,
	// add the bisection point.

	unsigned int	v0 = xyz().size() - 1;

	for (unsigned int v1 = 0; v1 < xyz().size(); ++v1)
	{
		const Point3 &	p0 = xyz()[v0];
		const Point3 &	p1 = xyz()[v1];
		const Point2 &	t0 = uv()[v0];
		const Point2 &	t1 = uv()[v1];
		float		d0 = classifications[v0];
		float		d1 = classifications[v1];

		// neg->neg non-crossover?

		if (d0 < 0 && d1 < 0)
		{
			negXYZ += p1;
			negUV += t1;
		}

		// neg->zed crossover?

		else if (d0 < 0 && d1 == 0)
		{
			negXYZ += p1;
			negUV += t1;
			posXYZ += p1;
			posUV += t1;
		}

		// neg->pos crossover?

		else if (d0 < 0 && d1 > 0)
		{
			float	delta = d0 / (d1-d0);

			Point3	bisectionXYZ = p0 - (p1 - p0) * delta;
			negXYZ += bisectionXYZ;
			posXYZ += bisectionXYZ;
			posXYZ += p1;

			Point2	bisectionUV = t0 - (t1 - t0) * delta;
			negUV += bisectionUV;
			posUV += bisectionUV;
			posUV += t1;
		}

		// zed->pos crossover?

		else if (d0 == 0 && d1 > 0)
		{
			posXYZ += p1;
			posUV += t1;
		}


		// zed->neg crossover?

		else if (d0 == 0 && d1 < 0)
		{
			negXYZ += p1;
			negUV += t1;
		}


		// pos->pos non-crossover?

		else if (d0 > 0 && d1 > 0)
		{
			posXYZ += p1;
			posUV += t1;
		}

		// pos->neg crossover?

		else if (d0 > 0 && d1 < 0)
		{
			float	delta = d1 / (d0-d1);

			Point3	bisectionXYZ = p1 - (p0 - p1) * delta;
			posXYZ += bisectionXYZ;
			negXYZ += bisectionXYZ;
			negXYZ += p1;

			Point2	bisectionUV = t1 - (t0 - t1) * delta;
			posUV  += bisectionUV;
			negUV  += bisectionUV;
			negUV  += t1;
		}

		// pos->zed crossover?

		else if (d0 > 0 && d1 == 0)
		{
			posXYZ += p1;
			negXYZ += p1;

			posUV += t1;
			negUV += t1;
		}

		// If the edge rests completely on the plane, then we've got a concave polygon. How's that, you say? Well...
		// This really shouldn't ever happen since an edge that rests on the plane means it can never cross the plane
		// without going concave, but inaccuracy issues do happen...
		//
		// Chances are (and we're guessing here!) that p0 and p1 are nearly (or completely) identical, in which case
		// we'll just ignore this, and it'll skip the extra vertex.

		else if (d0 == 0 && d1 == 0)
		{
			// Do nothing!
		}

		// Next vertex, please!

		v0 = v1;
	}

	// Replace the vertices with just those found on the postiive side

	xyz() = posXYZ;
	uv() = posUV;

	// Done

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// primitive.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

#endif // #ifdef ACTIVE_EDITORS