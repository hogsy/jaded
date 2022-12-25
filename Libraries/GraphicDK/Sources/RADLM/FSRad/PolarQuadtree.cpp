// ---------------------------------------------------------------------------------------------------------------------------------
//  _____        _             ____                  _ _                                       
// |  __ \      | |           / __ \                | | |                                      
// | |__) | ___ | | __ _ _ __| |  | |_   _  __ _  __| | |_ _ __  ___  ___      ___ _ __  _ __  
// |  ___/ / _ \| |/ _` | '__| |  | | | | |/ _` |/ _` | __| '__|/ _ \/ _ \    / __| '_ \| '_ \ 
// | |    | (_) | | (_| | |  | |__| | |_| | (_| | (_| | |_| |  |  __/  __/ _ | (__| |_) | |_) |
// |_|     \___/|_|\__,_|_|   \___\_\\__,_|\__,_|\__,_|\__|_|   \___|\___|(_) \___| .__/| .__/ 
//                                                                                | |   | |    
//                                                                                |_|   |_|    
//
// Description:
//
//   Polar Quadtree - a quadtree that is wrapped onto the surface of a sphere
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   10/25/2001 by Paul Nettle: Original creation
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

#include "PolarQuadtree.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ---------------------------------------------------------------------------------------------------------------------------------

	PolarQuadtree::PolarQuadtree(const geom::Ray3 & polarAxis)
{
	axis() = polarAxis;
	axisXForm() = geom::Matrix3::genLookat(axis().vector(), 0);

	// Init the root node

	root().min().phi() = -1;
	root().max().phi() = +1;

	root().min().theta() = -fstl::pi<float>();
	root().max().theta() = +fstl::pi<float>();

	root().clearChildren();

}

// ---------------------------------------------------------------------------------------------------------------------------------

	PolarQuadtree::~PolarQuadtree()
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

geom::Point2	PolarQuadtree::toPolar(const geom::Point3 & point) const
{
	// Our new point

	geom::Point2	polar;

	// Calculate phi (angular distance from the axis)

	geom::Vector3	temp = point - axis().origin();
	temp.normalize();

	polar.phi() = temp ^ axis().normal();

	geom::Point3	onPlane = axis().closest(point);
	onPlane.normalize();

	// Calculate theta (rotation around the axis)

	if ((onPlane ^ axisXForm().extractXVector()) >= 0)
	{
		polar.theta() = static_cast<float>(asin(onPlane ^ axisXForm().extractYVector()));
	}
	else
	{
		polar.theta() = fstl::pi<float>() - static_cast<float>(asin(onPlane ^ axisXForm().extractYVector()));
	}

	// Make sure the values are in range

	//assert(polar.phi() >= -1 && polar.phi() <= 1);
	//assert(polar.theta() >= -fstl::pi<float>() && polar.theta() <= fstl::pi<float>());

	return polar;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	PolarQuadtree::insert(const RadPrim & prim)
{
	// Convert the polygon into polar coordinates and retain the bounding rect of those coordinates

	sPQTVert	verts[64];
	geom::Point3	min = verts[0].polar;
	geom::Point3	max = verts[0].polar;
	{
		for (unsigned int i = 0; i < prim.xyz().size(); ++i)
		{
			verts[i].polar = toPolar(prim.xyz()[i]);

			if (verts[i].polar.phi()   < min.phi()  ) min.phi()   = verts[i].polar.phi();
			if (verts[i].polar.theta() < min.theta()) min.theta() = verts[i].polar.theta();
			if (verts[i].polar.phi()   > max.phi()  ) max.phi()   = verts[i].polar.phi();
			if (verts[i].polar.theta() > max.theta()) max.theta() = verts[i].polar.theta();
		}
	}

	// PDNDEBUG -- need to correct for wrap-around in theta...

	// Gather a list of potentially overlapping polygons

	RadPrimPointerListGrainy	overlappingPolygons;
	gatherOverlappingPolygons(root(), min, max, overlappingPolygons);

	// Clip the input primitive to all the overlapping polygons

	RadPrimListGrainy	clipPrimitives;
	clipPrimitives += prim;

	for (RadPrimPointerListGrainy::node * i = overlappingPolygons.head(); i; i = i->next())
	{
		RadPrim &	maskPrim = *i->data();

		RadPrimListGrainy	newList;

		for (RadPrimListGrainy::node * j = clipPrimitives.head(); j; j = j->next())
		{
			RadPrim &	clipPrim = j->data();

			// Clip the primitive against this "mask" primitive

			unsigned int v0 = maskPrim.xyz().size() - 1;
			for (unsigned int v1 = 0; v1 < maskPrim.xyz().size(); v0 = v1, ++v1)
			{
				// Build an edge plane

				geom::Point3	&p0 = maskPrim.xyz()[v0];
				geom::Point3	&p1 = maskPrim.xyz()[v1];
				geom::Point3	&p2 = axis().origin();

				// Generate an edgePlane that faces the interior of the primitive

				geom::Plane3	edgePlane(p2, (p0 - p1) % (p2 - p1));

				// Bisect it

				RadPrim	back;
				if (!clipPrim.bisect(edgePlane, back)) break;

				// If there's anything on the back-side, add it to the list to be clipped

				if (back.xyz().size()) newList += back;

				// Keep going?

				if (!clipPrim.xyz().size()) break;
			}
		}

		clipPrimitives = newList;
	}

	// Is there anything visible?

	if (clipPrimitives.size())
	{
		// Keep track of the visible pieces

		visiblePieces() += clipPrimitives;

		// Add this input primitive to the tree

		insertPrim(prim);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	PolarQuadtree::insertPrim(const RadPrim & prim)
{
	root().polys() += const_cast<RadPrim *>(&prim);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	PolarQuadtree::gatherOverlappingPolygons(const PQTNode & node, const geom::Point2 & min, const geom::Point2 & max, RadPrimPointerListGrainy & polys)
{
	// Skip nodes that don't overlap the input rect

	if (max.phi()   < node.min().phi()  ) return;
	if (max.theta() < node.min().theta()) return;
	if (min.phi()   > node.max().phi()  ) return;
	if (min.theta() > node.max().theta()) return;

	// Okay, we have an overlap... associate my polygons and then visit my children

	polys += node.polys();

	for (unsigned int i = 0; i < 4; ++i)
	{
		if (node.children()[i]) gatherOverlappingPolygons(*node.children()[i], min, max, polys);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// PolarQuadtree.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

#endif // #ifdef ACTIVE_EDITORS