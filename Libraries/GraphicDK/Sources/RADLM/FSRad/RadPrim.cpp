// ---------------------------------------------------------------------------------------------------------------------------------
//  _____            _ _____       _                                
// |  __ \          | |  __ \     (_)                               
// | |__) | __ _  __| | |__) |_ __ _ _ __ ___       ___ _ __  _ __  
// |  _  / / _` |/ _` |  ___/| '__| | '_ ` _ \     / __| '_ \| '_ \ 
// | | \ \| (_| | (_| | |    | |  | | | | | | | _ | (__| |_) | |_) |
// |_|  \_\\__,_|\__,_|_|    |_|  |_|_| |_| |_|(_) \___| .__/| .__/ 
//                                                     | |   | |    
//                                                     |_|   |_|    
//
// Description:
//
//   Polygon as derived from the primitive class, specialized for radiosity usage
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   08/11/2001 by Paul Nettle: Original creation
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

#include "RadPrim.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ---------------------------------------------------------------------------------------------------------------------------------

static	const	float	EPSILON = 1.0e-5f;

// ---------------------------------------------------------------------------------------------------------------------------------
// Warning: The following code is bizarre, to say the least. I'll try to explain it here:
//
// A polygon with multiple 'n' coordinates per vertex will exist in 'n' spaces simultaneously. In other words, a polygon that has
// a (1) 3D coordinate per vertex, (2) a 2D texture coordinate per vertex and (3) a 2D lightmap coordinate per vertex, will exist
// in each of these three space simultaneously.
//
// Because these three spaces are seemingly arbitrary (chosen by an artist, for example) there is no immediate way to map from one
// to the next. For the following explanation, we are only concerned with two of all possible spaces: 3-space (the description of
// the polygon as it exists in 3D) and 2-space (the description of the polygon as it exists in 2D lightmap space.)
//
// For the sake of simplicity, I will refer to these two spaces as "3-space" and "2-space".
//
// Because this code was created for the purpose of lightmapping (at least, originally) our main focus is the ability to transform
// from 3-space into 2-space. In the end, we want this to be as quick as possible, so our final product will be two 3D vectors.
//
// These two vectors represent the direction that you must travel (in 3-space) in order to follow along each of the two axes in
// 2-space. These two vectors can (and should) be given a length that will represent the distance in 3-space needed to travel
// one unit in 2-space, along the associated vector.
//
// I hope I've made the academics happy, but for those of you that want it in plain ol' fashioned English, here's a clearer
// description: Given a point in 3-space that maps to the point [15.84, 12.43] in 2-space, if we add the U-vector (the first of
// our two 2 vectors) to that 3-space point, we will end up with a 3-space point that maps to the 2-space coordinate [16.84, 12.43]
// (one unit along the U-axis in the positive direction in textures-space.)
//
// Confused? If not, you soon will be, because the method by which we need to calculate this is rather bizarre. I'll try to be
// clear about it...
//
// Going back to an earlier portion of the text, remember that the polygon exists in multiple spaces simultaneously. Also note that
// these spaces are chosen arbitrarily. Because of this, we actually need to search for the solution, rather than simply calculate
// it.
//
// It's best to think of the solution (or the search for the solution) in terms of axes. There are three axes in 3-space and two
// axes in 2-space. Our result will be the two (3-space) vectors that point along the direction of the 2 (2-space) axes. Here's what
// it looks like:
//
//             /\
//            / |\
//           /  | \
//          /   |  \
//         /    |   \
//        /     |    \
//       /____________\_____ V-Vector
//      /----___|      \
//              |----___\
//              |
//              |
//              U-Vector
// 
// What you're looking at is a 3D polygon as seen from a perpendicular view to the camera, with the two Vectors (U and V) shown as
// vertical and horizontal lines. This is the simplest case, these lines will probably often be at odd orientations, which is why
// we must search for them. Here's how we'll perform that search:
//
// 	point3	u1_3, u2_3, v1_3, v2_3;
// 	point2	u1_2, u2_2, v1_2, v2_2;
// 	double	maxUDist, maxVDist;
//
// 	for (each vertex)
// 	{
// 		for (each edge not sharing that vertex)
// 		{
// 			// Intercept in U?
//
// 			if (edge UVs form a line that intercepts the U value from the current vertex)
// 			{
// 				if (interceptDist > maxUDist)
// 				{
// 					maxUDist = interceptDist
// 					u1_3 = current vertex
// 					u2_3 = interceptPoint
// 					u1_2 = current vertex (UV)
// 					u2_2 = interceptPoint (UV)
// 				}
// 			}
//
// 			// Intercept in V (this code is the same as above, but for the V component)?
//
// 			if (edge UVs form a line that intercepts the V value from the current vertex)
// 			{
// 				if (interceptDist > maxVDist)
// 				{
// 					maxVDist = interceptDist
// 					v1_3 = current vertex
// 					v2_3 = interceptPoint
// 					v1_2 = current vertex (UV)
// 					v2_2 = interceptPoint (UV)
// 				}
// 			}
// 		}
// 	}
//
// At this point, we have 8 points: four of which are points in 2-space (one point for each endpoint of a line in 3-space that
// follows the axis of the associated 2-space) and the four corresponding 2-space coordinates. From this, we are able to easily
// calculate the resulting vectors (see code below.)
//
// ---------------------------------------------------------------------------------------------------------------------------------

void	RadPrim::calcTransformVectors()
{
	// Init these...

	vXFormVector() = geom::Vector3(0, 0, 0);
	uXFormVector() = geom::Vector3(0, 0, 0);

	// Visit each vertex

	geom::Point3	u0_3d(0,0,0), u1_3d(0,0,0), v0_3d(0,0,0), v1_3d(0,0,0);
	geom::Point2	u0_2d(0,0),   u1_2d(0,0),   v0_2d(0,0),   v1_2d(0,0);
	double		maxUDist = -1, maxVDist = -1;
	const double	epsilon = 1.0e-5;

	for (unsigned int i = 0; i < xyz().size(); ++i)
	{
		geom::Point3 &	c3D = xyz()[i];
		geom::Point2 &	c2D = uv()[i];
		unsigned int	v0 = xyz().size() - 1;

		// Visit each opposing edge

		for (unsigned int v1 = 0; v1 < xyz().size(); ++v1)
		{
			// Skip edges that include the current vertex

			if (i != v0 && i != v1)
			{
				geom::Point3 &	v03D = xyz()[v0];
				geom::Point2 &	v02D = uv()[v0];
				geom::Point3 &	v13D = xyz()[v1];
				geom::Point2 &	v12D = uv()[v1];

				// Intercept in U?

				if ((v02D.u() - epsilon <= c2D.u() && v12D.u() + epsilon >= c2D.u()) ||
				    (v02D.u() + epsilon >= c2D.u() && v12D.u() - epsilon <= c2D.u()))
				{
					float		delta = (c2D.u() - v02D.u()) / (v12D.u()  - v02D.u());
					geom::Point3	interceptPoint3D = v03D + (v13D - v03D) * delta;
					geom::Point2	interceptPoint2D = v02D + (v12D - v02D) * delta;
					float		interceptDist3D = c3D.distance(interceptPoint3D);

					if (interceptDist3D > maxUDist)
					{
						maxUDist = interceptDist3D;
						u0_3d = c3D;
        					u1_3d = interceptPoint3D;
						u0_2d = c2D;
						u1_2d = interceptPoint2D;
					}
				}

 				// Intercept in V (this code is the same as above, but for the V component)?

				if ((v02D.v() - epsilon <= c2D.v() && v12D.v() + epsilon >= c2D.v()) ||
				    (v02D.v() + epsilon >= c2D.v() && v12D.v() - epsilon <= c2D.v()))
				{
					float		delta = (c2D.v() - v02D.v()) / (v12D.v()  - v02D.v());
					geom::Point3	interceptPoint3D = v03D + (v13D - v03D) * delta;
					geom::Point2	interceptPoint2D = v02D + (v12D - v02D) * delta;
					float		interceptDist3D = c3D.distance(interceptPoint3D);

					if (interceptDist3D > maxVDist)
					{
						maxVDist = interceptDist3D;
						v0_3d = c3D;
        					v1_3d = interceptPoint3D;
						v0_2d = c2D;
						v1_2d = interceptPoint2D;
					}
				}
			}

			// Next edge...

			v0 = v1;
		}
	}

	// Calc the two vectors (setting the length of each to the distance in 3-space that covers a single unit in 2-space.)

	if (maxUDist > 0)	vXFormVector() = (u1_3d - u0_3d) / (u1_2d.v() - u0_2d.v());
	if (maxVDist > 0)	uXFormVector() = (v1_3d - v0_3d) / (v1_2d.u() - v0_2d.u());
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadPrim::prepare(const unsigned int patchResolutionU, unsigned int patchResolutionV)
{
	// Wipe out any current list

	elementAreas().erase();

	// Find the UV extents and minimum XYZ

	unsigned int	uCount, vCount;
	{
		// UV extents

		int	minU, minV, maxU, maxV;
		calcIntegerUVExtents(minU, maxU, minV, maxV);

		// UV dimensions

		uCount = maxU - minU + 1;
		vCount = maxV - minV + 1;

		// The actual upper-left-most part of the of the upper-left-most lightmap texel

		minUV() = geom::Point2(static_cast<float>(minU), static_cast<float>(minV));
		maxUV() = geom::Point2(static_cast<float>(maxU), static_cast<float>(maxV));

		// The delta that goes from a vertex (any vertex) to the point in 3-space where the upper-left texel begins

		geom::Vector2	delta2 = minUV() - uv()[0];

		// Find the upper-left 3D coordinate

		minXYZ() = xyz()[0] + uXFormVector() * delta2.u() + vXFormVector() * delta2.v();
	}

	// Two planes: one horizontal and one vertical for slicing up a grid of patches and elements

	geom::Plane3	uPlane(minXYZ(), plane().normal() % vXFormVector());
	geom::Plane3	vPlane(minXYZ(), plane().normal() % uXFormVector());
	{
		// Make sure these planes face the interior of the primitive

		geom::Point3	primCenter = calcCenterOfMass();
		if (uPlane.halfplane(primCenter) < 0) uPlane.vector() = - uPlane.vector();
		if (vPlane.halfplane(primCenter) < 0) vPlane.vector() = - vPlane.vector();
	}

	// How many potential patches?

	uPatches() = uCount / patchResolutionU;
	if (uCount % patchResolutionU) uPatches()++;
	vPatches() = vCount / patchResolutionV;
	if (vCount % patchResolutionV) vPatches()++;

	// Our patches

	fstl::intArray	patchElementCounts;

	// Populate our patches with default information
	{

		RadPatch	defaultPatch;
		defaultPatch.area()   = 0;
		defaultPatch.energy() = illuminationColor();
		defaultPatch.plane()  = plane();
		defaultPatch.origin() = geom::Point3(0,0,0);

		patches().erase();
		patches().populate(defaultPatch, uPatches() * vPatches());

		// Initialize this temp array...

		patchElementCounts.populate(0, uPatches() * vPatches());
	}

	// Slice up the primitive into elements & patches
	{
		// Get a working copy of self so we can slice it up

		RadPrim	prim = *this;

		// Reserve for speed

		elementAreas().reserve(uCount * vCount);

		// Make vertical slices through the polygon, chopping it up into rows of elements

		geom::Plane3	evPlane = vPlane;
		for (unsigned int v = 0; v < vCount; ++v)
		{
			// Move the V plane

			evPlane.origin() += vXFormVector();

			// Slice off a piece of the primitive -- this will represent a row of elements
			// (no need to slice the last one -- it will be the leftover)

			RadPrim	elementRow;
			if (v < vCount-1)	prim.bisect(evPlane, elementRow);
			else			elementRow = prim;

			// Slice the row of elements into individual elements

			geom::Plane3	euPlane = uPlane;
			for (unsigned int u = 0; u < uCount; ++u)
			{
				euPlane.origin() += uXFormVector();

				// Slice off an element
				// (no need to slice the last one -- it will be the leftover)

				RadPrim	element;
				if (u < uCount-1)	elementRow.bisect(euPlane, element);
				else			element = elementRow;

				// Add this element area to the patch

				float	elementArea = element.calcArea();
				elementAreas() += elementArea;

				if (elementArea && patches().size())
				{
					// Index into this patch

					unsigned int	patchIndex = v/patchResolutionV * uPatches() + u/patchResolutionU;

					// This element belongs to the following patch:

					RadPatch &	patch = patches()[patchIndex];

					// Add the area of the elment to the patch

					patch.area() += elementArea;
					patch.origin() += element.calcCenterOfMass();

					// We'll be doing an average of the element centers to find the center of the patch,
					// so keep track of how many elements are part of the average

					patchElementCounts[patchIndex] += 1;
				}
			}
		}
	}

	// Cleanup and finalize the patch array

	if (patches().size())
	{
		// Our total surface area

		float	totalArea = calcArea();

		// Finalize...

		for (unsigned int i = 0; i < patches().size(); ++i)
		{
			RadPatch &	patch = patches()[i];

			// Find the center of the patch

			patch.origin() /= static_cast<float>(patchElementCounts[i]);

			// Account for patch's area

			patch.energy() *= patch.area() / totalArea;
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadPrim::prepareNoPatches()
{
	// Wipe out any current list

	elementAreas().erase();

	// Find the UV extents and minimum XYZ

	unsigned int	uCount, vCount;
	{
		// UV extents

		int	minU, minV, maxU, maxV;
		calcIntegerUVExtents(minU, maxU, minV, maxV);

		// UV dimensions

		uCount = maxU - minU + 1;
		vCount = maxV - minV + 1;

		// The actual upper-left-most part of the of the upper-left-most lightmap texel

		minUV() = geom::Point2(static_cast<float>(minU), static_cast<float>(minV));
		maxUV() = geom::Point2(static_cast<float>(maxU), static_cast<float>(maxV));

		// The delta that goes from a vertex (any vertex) to the point in 3-space where the upper-left texel begins

		geom::Vector2	delta2 = minUV() - uv()[0];

		// Find the upper-left 3D coordinate

		minXYZ() = xyz()[0] + uXFormVector() * delta2.u() + vXFormVector() * delta2.v();
	}

	// Two planes: one horizontal and one vertical for slicing up a grid of patches and elements

	geom::Plane3	uPlane(minXYZ(), plane().normal() % vXFormVector());
	geom::Plane3	vPlane(minXYZ(), plane().normal() % uXFormVector());
	{
		// Make sure these planes face the interior of the primitive

		geom::Point3	primCenter = calcCenterOfMass();
		if (uPlane.halfplane(primCenter) < 0) uPlane.vector() = - uPlane.vector();
		if (vPlane.halfplane(primCenter) < 0) vPlane.vector() = - vPlane.vector();
	}

	// Slice up the primitive into elements & patches
	{
		// Get a working copy of self so we can slice it up

		RadPrim	prim = *this;

		// Reserve for speed

		elementAreas().reserve(uCount * vCount);

		// Make vertical slices through the polygon, chopping it up into rows of elements

		geom::Plane3	evPlane = vPlane;
		for (unsigned int v = 0; v < vCount; ++v)
		{
			// Move the V plane

			evPlane.origin() += vXFormVector();

			// Slice off a piece of the primitive -- this will represent a row of elements
			// (no need to slice the last one -- it will be the leftover)

			RadPrim	elementRow;
			if (v < vCount-1)	prim.bisect(evPlane, elementRow);
			else			elementRow = prim;

			// Slice the row of elements into individual elements

			geom::Plane3	euPlane = uPlane;
			for (unsigned int u = 0; u < uCount; ++u)
			{
				euPlane.origin() += uXFormVector();

				// Slice off an element
				// (no need to slice the last one -- it will be the leftover)

				RadPrim	element;
				if (u < uCount-1)	elementRow.bisect(euPlane, element);
				else			element = elementRow;

				// Add this element area to the patch

				float	elementArea = element.calcArea();
				elementAreas() += elementArea;
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	RadPrim::mergePatches2x2()
{
	unsigned int	 newUPatches = uPatches() / 2;
	if (uPatches() % 2) newUPatches++;
	unsigned int	 newVPatches = vPatches() / 2;
	if (vPatches() % 2) newVPatches++;

	// If we can't combine, bail

	if (newUPatches == uPatches() && newVPatches == vPatches()) return false;

	RadPatchArray	newPatches;
	fstl::intArray	newPatchesCount;
	RadPatch	defaultPatch;
	defaultPatch.area()   = 0;
	defaultPatch.energy() = geom::Color3(0,0,0);
	defaultPatch.origin() = geom::Point3(0,0,0);
	defaultPatch.plane()  = plane();
	newPatches.populate(defaultPatch, newUPatches * newVPatches);
	newPatchesCount.populate(0, newUPatches * newVPatches);

	for(unsigned int v = 0; v < vPatches(); ++v)
	{
		for(unsigned int u = 0; u < uPatches(); ++u)
		{
			unsigned int	idx = v * uPatches() + u;
			unsigned int	newIdx = v/2 * newUPatches + u/2;

			// Skip unused patches

			if (!patches()[idx].area()) continue;

			newPatches[newIdx].area() += patches()[idx].area();
			newPatches[newIdx].energy() += patches()[idx].energy();
			newPatches[newIdx].origin() += patches()[idx].origin();
			newPatchesCount[newIdx]++;
		}
	}

	// Get the origin as the average of the other centers

	for (unsigned int j = 0; j < newPatches.size(); ++j)
	{
		if (newPatchesCount[j]) newPatches[j].origin() /= static_cast<float>(newPatchesCount[j]);
	}

	patches() = newPatches;
	uPatches() = newUPatches;
	vPatches() = newVPatches;

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Hacked from the one in fsgl::Primitive...
// ---------------------------------------------------------------------------------------------------------------------------------

bool	RadPrim::localBisect(const geom::Plane3 & clipPlane, RadPrim & back)
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
			back.texuv().erase();

			return true;
		}

		// If no positive points, return self as negative polygon

		else if (!pos)
		{
			back.xyz() = xyz();
			back.uv() = uv();
			back.texuv() = texuv();
			back.plane() = plane();

			xyz().erase();
			uv().erase();
			texuv().erase();
			return true;
		}

	}

	// Our result is stored here

	back.xyz() = xyz();
	back.uv() = uv();
	back.texuv() = texuv();
	back.plane() = plane();

	geom::Point3Array &	negXYZ = back.xyz();
	geom::Point2Array &	negUV  = back.uv();
	geom::Point2Array &	negTUV = back.texuv();
	negXYZ.erase();
	negXYZ.reserve(xyz().size());
	negUV.erase();
	negUV.reserve(xyz().size());
	negTUV.erase();
	negTUV.reserve(xyz().size());

	geom::Point3Array	posXYZ;
	geom::Point2Array	posUV;
	geom::Point2Array	posTUV;
	posXYZ.reserve(xyz().size());
	posUV.reserve(xyz().size());
	posTUV.reserve(xyz().size());

	// Visit all points in the list, adding points that are on the front-side of the current plane. If an edge bisects the plane,
	// add the bisection point.

	unsigned int	v0 = xyz().size() - 1;

	for (unsigned int v1 = 0; v1 < xyz().size(); ++v1)
	{
		const geom::Point3 &	p0 = xyz()[v0];
		const geom::Point3 &	p1 = xyz()[v1];
		const geom::Point2 &	l0 = uv()[v0];
		const geom::Point2 &	l1 = uv()[v1];
		const geom::Point2 &	t0 = texuv()[v0];
		const geom::Point2 &	t1 = texuv()[v1];
		float			d0 = classifications[v0];
		float			d1 = classifications[v1];

		// neg->neg non-crossover?

		if (d0 < 0 && d1 < 0)
		{
			negXYZ += p1;
			negUV += l1;
			negTUV += t1;
		}

		// neg->zed crossover?

		else if (d0 < 0 && d1 == 0)
		{
			negXYZ += p1;
			posXYZ += p1;

			negUV += l1;
			posUV += l1;

			negTUV += t1;
			posTUV += t1;
		}

		// neg->pos crossover?

		else if (d0 < 0 && d1 > 0)
		{
			float	delta = d0 / (d1-d0);

			geom::Point3	bisectionXYZ = p0 - (p1 - p0) * delta;
			negXYZ += bisectionXYZ;
			posXYZ += bisectionXYZ;
			posXYZ += p1;

			geom::Point2	bisectionUV = l0 - (l1 - l0) * delta;
			negUV += bisectionUV;
			posUV += bisectionUV;
			posUV += l1;

			geom::Point2	bisectionTUV = t0 - (t1 - t0) * delta;
			negTUV += bisectionTUV;
			posTUV += bisectionTUV;
			posTUV += t1;
		}

		// zed->pos crossover?

		else if (d0 == 0 && d1 > 0)
		{
			posXYZ += p1;
			posUV += l1;
			posTUV += t1;
		}


		// zed->neg crossover?

		else if (d0 == 0 && d1 < 0)
		{
			negXYZ += p1;
			negUV += l1;
			negTUV += t1;
		}


		// pos->pos non-crossover?

		else if (d0 > 0 && d1 > 0)
		{
			posXYZ += p1;
			posUV += l1;
			posTUV += t1;
		}

		// pos->neg crossover?

		else if (d0 > 0 && d1 < 0)
		{
			float	delta = d1 / (d0-d1);

			geom::Point3	bisectionXYZ = p1 - (p0 - p1) * delta;
			posXYZ += bisectionXYZ;
			negXYZ += bisectionXYZ;
			negXYZ += p1;

			geom::Point2	bisectionUV = l1 - (l0 - l1) * delta;
			posUV  += bisectionUV;
			negUV  += bisectionUV;
			negUV  += l1;

			geom::Point2	bisectionTUV = t1 - (t0 - t1) * delta;
			posTUV  += bisectionTUV;
			negTUV  += bisectionTUV;
			negTUV  += t1;
		}

		// pos->zed crossover?

		else if (d0 > 0 && d1 == 0)
		{
			posXYZ += p1;
			negXYZ += p1;

			posUV += l1;
			negUV += l1;

			posTUV += t1;
			negTUV += t1;
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
	texuv() = posTUV;

	// Done

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	RadPrim::bisectNoTextures(const geom::Plane3 & clipPlane, RadPrim & back)
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
			return true;
		}

		// If no positive points, return self as negative polygon

		else if (!pos)
		{
			back.xyz() = xyz();
			back.plane() = plane();

			xyz().erase();
			return true;
		}

	}

	// Our result is stored here

	back.xyz() = xyz();
	back.plane() = plane();

	geom::Point3Array &	negXYZ = back.xyz();
	negXYZ.erase();
	negXYZ.reserve(xyz().size());

	geom::Point3Array	posXYZ;
	posXYZ.reserve(xyz().size());

	// Visit all points in the list, adding points that are on the front-side of the current plane. If an edge bisects the plane,
	// add the bisection point.

	unsigned int	v0 = xyz().size() - 1;

	for (unsigned int v1 = 0; v1 < xyz().size(); ++v1)
	{
		const geom::Point3 &	p0 = xyz()[v0];
		const geom::Point3 &	p1 = xyz()[v1];
		float			d0 = classifications[v0];
		float			d1 = classifications[v1];

		// neg->neg non-crossover?

		if (d0 < 0 && d1 < 0)
		{
			negXYZ += p1;
		}

		// neg->zed crossover?

		else if (d0 < 0 && d1 == 0)
		{
			negXYZ += p1;
			posXYZ += p1;
		}

		// neg->pos crossover?

		else if (d0 < 0 && d1 > 0)
		{
			float	delta = d0 / (d1-d0);

			geom::Point3	bisectionXYZ = p0 - (p1 - p0) * delta;
			negXYZ += bisectionXYZ;
			posXYZ += bisectionXYZ;
			posXYZ += p1;
		}

		// zed->pos crossover?

		else if (d0 == 0 && d1 > 0)
		{
			posXYZ += p1;
		}


		// zed->neg crossover?

		else if (d0 == 0 && d1 < 0)
		{
			negXYZ += p1;
		}


		// pos->pos non-crossover?

		else if (d0 > 0 && d1 > 0)
		{
			posXYZ += p1;
		}

		// pos->neg crossover?

		else if (d0 > 0 && d1 < 0)
		{
			float	delta = d1 / (d0-d1);

			geom::Point3	bisectionXYZ = p1 - (p0 - p1) * delta;
			posXYZ += bisectionXYZ;
			negXYZ += bisectionXYZ;
			negXYZ += p1;
		}

		// pos->zed crossover?

		else if (d0 > 0 && d1 == 0)
		{
			posXYZ += p1;
			negXYZ += p1;
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

	// Done

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// RadPrim.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

#endif // #ifdef ACTIVE_EDITORS