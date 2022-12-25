// ---------------------------------------------------------------------------------------------------------------------------------
//   _____  ____       _                                       
//  / ____|/ __ \     | |                                      
// | (___ | |  | | ___| |_ _ __  ___  ___      ___ _ __  _ __  
//  \___ \| |  | |/ __| __| '__|/ _ \/ _ \    / __| '_ \| '_ \ 
//  ____) | |__| | (__| |_| |  |  __/  __/ _ | (__| |_) | |_) |
// |_____/ \____/ \___|\__|_|   \___|\___|(_) \___| .__/| .__/ 
//                                                | |   | |    
//                                                |_|   |_|    
//
// Description:
//
//   Splitting Octree
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   08/15/2001 by Paul Nettle: Original creation
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

#include "SOctree.h"
#include "BSP.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// --------------------------------------------------------------------------------------------------------------------------------

static	geom::Point3	childCenterOffsets[] =
{
	geom::Point3(-0.5f, -0.5f, -0.5f),	// - - -
	geom::Point3( 0.5f, -0.5f, -0.5f),	// + - -
	geom::Point3(-0.5f,  0.5f, -0.5f),	// - + -
	geom::Point3( 0.5f,  0.5f, -0.5f),	// + + -
	geom::Point3(-0.5f, -0.5f,  0.5f),	// - - +
	geom::Point3( 0.5f, -0.5f,  0.5f),	// + - +
	geom::Point3(-0.5f,  0.5f,  0.5f),	// - + +
	geom::Point3( 0.5f,  0.5f,  0.5f)	// + + +
};

// --------------------------------------------------------------------------------------------------------------------------------

static	geom::Point3	planeCenterOffsets[] =
{
	geom::Point3( -1,  0,  0),		// -X
	geom::Point3( +1,  0,  0),		// +X
	geom::Point3(  0, -1,  0),		// -Y
	geom::Point3(  0, +1,  0),		// +Y
	geom::Point3(  0,  0, -1),		// -Z
	geom::Point3(  0,  0, +1),		// +Z
};

// ---------------------------------------------------------------------------------------------------------------------------------

static	bool	updateDisplay(SOctree::sBuildInfo &buildInfo, const unsigned int depth, const bool forceUpdate = false)
{
	// Update the display

	static	periodicUpdates;

    // DJ_TEMP : 
    /*
	if (!(periodicUpdates & 0xF) || forceUpdate)
	{
		if (!forceUpdate)	buildInfo.progressDialog->setCurrentPercent(buildInfo.processedSurfaceArea / buildInfo.totalSceneSurfaceArea * 100.0f);
		else			buildInfo.progressDialog->setCurrentPercent(100.0f);

		char	dsp[90];
		sprintf(dsp, "%d", buildInfo.totalNodes);
		buildInfo.progressDialog->setLabel2("Total Nodes:");
		buildInfo.progressDialog->setText2(dsp);

		sprintf(dsp, "%d", buildInfo.totalPolys);
		buildInfo.progressDialog->setLabel3("Total polygons:");
		buildInfo.progressDialog->setText3(dsp);

		sprintf(dsp, "%d", static_cast<int>((static_cast<float>(buildInfo.totalDepth) / static_cast<float>(buildInfo.totalNodes))));
		buildInfo.progressDialog->setLabel4("Average depth:");
		buildInfo.progressDialog->setText4(dsp);

		sprintf(dsp, "%d", static_cast<int>(buildInfo.totalSceneSurfaceArea));
		buildInfo.progressDialog->setLabel6("Total area:");
		buildInfo.progressDialog->setText6(dsp);

		sprintf(dsp, "%d", static_cast<int>(buildInfo.processedSurfaceArea));
		buildInfo.progressDialog->setLabel7("Current area:");
		buildInfo.progressDialog->setText7(dsp);

		sprintf(dsp, "%d", static_cast<int>(buildInfo.totalLights));
		buildInfo.progressDialog->setLabel8("Total lights:");
		buildInfo.progressDialog->setText8(dsp);

		sprintf(dsp, "%d", static_cast<int>(buildInfo.totalLightmaps));
		buildInfo.progressDialog->setLabel9("Total lightmaps:");
		buildInfo.progressDialog->setText9(dsp);
	}
    */

	periodicUpdates++;

	// Always check for this...

	// DJ_TEMP : if (buildInfo.progressDialog->stopRequested()) return false;

	// Track our max depth

    // DJ_TEMP : 
    /*
	if (depth > buildInfo.maxDepth)
	{
		buildInfo.maxDepth = depth;
		char	dsp[90];
		sprintf(dsp, "%d", depth);
		buildInfo.progressDialog->setLabel5("Max depth:");
		buildInfo.progressDialog->setText5(dsp);
	}
    */

	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------

	SOctree::~SOctree()
{
}

// --------------------------------------------------------------------------------------------------------------------------------

bool	SOctree::build(GeomDB & geometry, sBuildInfo & bi)
{
	// Find the bounding box of the entire scene

	RadPrimList &	inPolys = geometry.polys();
	geom::Point3	min = inPolys.head()->data().xyz()[0];
	geom::Point3	max = inPolys.head()->data().xyz()[0];
	float		totalArea = 0;

	// DJ_TEMP : if (bi.progressDialog) bi.progressDialog->setCurrentStatus("Preparing scene...");

	// Get the bounding box of the scene from polygons
	{
		RadPrimList::node *	n = inPolys.head();
		unsigned int	index = 0;
		while(n)
		{
			// DJ_TEMP : if (!(index&0xf) && bi.progressDialog) bi.progressDialog->setCurrentPercent(static_cast<float>(index) / static_cast<float>(inPolys.size()) * 100.0f);
			index++;

			const RadPrim &	p = n->data();
			for (unsigned int j = 0; j < p.xyz().size(); ++j)
			{
				min.x() = fstl::min(min.x(), p.xyz()[j].x());
				min.y() = fstl::min(min.y(), p.xyz()[j].y());
				min.z() = fstl::min(min.z(), p.xyz()[j].z());

				max.x() = fstl::max(max.x(), p.xyz()[j].x());
				max.y() = fstl::max(max.y(), p.xyz()[j].y());
				max.z() = fstl::max(max.z(), p.xyz()[j].z());
			}

			// Calculate surface area for tracking percent complete

			totalArea += p.calcArea();

			n = n->next();
		}
	}

	// Account for lights in the bounding box

	const	RadPatchList &	inLights = geometry.lights();
	{
		for (RadPatchList::node * i = inLights.head(); i; i = i->next())
		{
			const RadPatch &	l = i->data();
			min.x() = fstl::min(min.x(), l.origin().x());
			min.y() = fstl::min(min.y(), l.origin().y());
			min.z() = fstl::min(min.z(), l.origin().z());
			max.x() = fstl::max(max.x(), l.origin().x());
			max.y() = fstl::max(max.y(), l.origin().y());
			max.z() = fstl::max(max.z(), l.origin().z());
		}
	}

	// Generate a new buildinfo struct with the statistics initialized

	bi.maxDepth = 0;
	bi.totalDepth = 0;
	bi.originalPolys = inPolys.size();
	bi.totalPolys = 0;
	bi.totalLights = 0;
	bi.totalLightmaps = geometry.lightmaps().size();
	bi.totalNodes = 0;
	bi.totalSceneSurfaceArea = totalArea;
	bi.processedSurfaceArea = 0;

	// Initial polygon count

    // DJ_TEMP : 
    /*
	char	dsp[90];
	sprintf(dsp, "%d", bi.originalPolys);
	if (bi.progressDialog)
	{
		bi.progressDialog->setLabel1("Polygons:");
		bi.progressDialog->setText1(dsp);
		bi.progressDialog->setCurrentPercent(0.0f);
		bi.progressDialog->setCurrentStatus("Building splitting octree...");
	}
    */

	// Root's center & radius...

	geom::Point3	boxDim = max - min;
	center() = min + boxDim / 2;
	radius() = fstl::max(boxDim.x(), fstl::max(boxDim.y(), boxDim.z())) / 2;

	// Start the recursive build process

	if (!recursiveBuild(inPolys, inLights, bi)) return false;

	// Force an update for that "100%" status

	updateDisplay(bi, 0, true);

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	SOctree::recursiveBuild(RadPrimList & inPolys, const RadPatchList & inLights, sBuildInfo & bi, const unsigned int depth)
{
	// Per-node stats

	bi.totalNodes++;
	bi.totalDepth += depth;

	// User display

	if (!updateDisplay(bi, depth)) return false;

	// Conditions for being a leaf...

	bool	leaf = false;
	assert(inPolys.size() || inLights.size());
	if (depth >= bi.maxDepthLimiter) leaf = true;
	if (radius() <= bi.minRadiusLimiter) leaf = true;
	if (inPolys.size() <= bi.thresholdLimiter) leaf = true;

	// Am I a leaf?

	if (leaf)
	{
		// This _is_ a clipping octree, is it not? :)

		polys() = inPolys;
		{
			for (RadPrimList::node *i = polys().head(); i; i = i->next())
			{
				RadPrim &	p = i->data();

				// Six sides to clip to

				for (unsigned int j = 0; j < 6; ++j)
				{
					// Origin & Center of this node-bounding plane

					geom::Point3	org = center() + planeCenterOffsets[j] * radius();
					geom::Vector3	dir = -planeCenterOffsets[j];

					// Build a plane that faces the interior of the node

					geom::Plane3	plane(org, dir);

					// Clip this polygon to the plane, keeping everything in the interior of the node

					RadPrim		backSide;
					p.bisect(plane, backSide);
				}

				bi.processedSurfaceArea += p.calcArea();
			}
		}

		// Build a BSP tree from this sucker
		{
			buildInfo.scenePolys = &polys();
			buildInfo.quantizeResolution = bi.bspQuantizeResolution;
			buildInfo.leastDepthErrorBoundsPercent = bi.bspLeastDepthErrorBoundsPercent;
			buildInfo.progressDialog = NULL;

			// Build the BSP

			if (!bsp().build(buildInfo)) return false;
			bi.totalPolys += buildInfo.totalPolys;
		}

		// Track lights

		bi.totalLights += inLights.size();

		return true;
	}

	// Determine which node(s) each polygon intersects

	for (unsigned int i = 0; i < 8; ++i)
	{
		// Child dimensions

		float		childRadius = radius() / 2.0f;
		geom::Point3	childCenter = center() + childCenterOffsets[i] * radius();
		geom::Point3	cr(childRadius, childRadius, childRadius);

		// The list of polygons that intersect this child's AABB

		RadPrimList	childPolys;
		childPolys.reserve(inPolys.size());
		{
			RadPrimList::node *	n = inPolys.head();
			while(n)
			{
				if (n->data().intersectAABB(childCenter, cr))
				{
					childPolys += n->data();
				}

				n = n->next();
			}

			// Remove the waste left over from what we originally reserved

			childPolys.compact();
		}

		// The list of lights that intersect this child's AABB

		RadPatchList	childLights;
		{
			geom::Point3	cMin = childCenter - childRadius;
			geom::Point3	cMax = childCenter + childRadius;

			for (RadPatchList::node * j = inLights.head(); j; j = j->next())
			{
				const geom::Point3 &	pos = j->data().origin();

				if (pos.x() >= cMin.x() && pos.x() <= cMax.x() &&
				    pos.y() >= cMin.y() && pos.y() <= cMax.y() &&
				    pos.z() >= cMin.z() && pos.z() <= cMax.z())
				{
					childLights += j->data();
				}
			}
		}

		// If there weren't any polys or lights, don't recurse

		if (!childPolys.size() && !childLights.size())
		{
			// No child goes here, stick in a placeholder

			children() += static_cast<SOctree *>(0);
			continue;
		}

		// Create a new child

		SOctree *	child = &bi.octreeNodeReservoir.get();
		child->center() = childCenter;
		child->radius() = childRadius;

		// Add this child to my array of children

		children() += child;

		// Recursively build this branch of the tree

		if (!child->recursiveBuild(childPolys, childLights, bi, depth + 1)) return false;
	}

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	SOctree::traverse(SOctreeTraversalHandler th, void *userData)
{
	// Call the traversal function for this node

	if (!th(*this, userData)) return false;

	// Visit my children

	for (unsigned int i = 0; i < children().size(); ++i)
	{
		if (!children()[i]) continue;
		SOctree	*child = children()[i];
		if (!child->traverse(th, userData)) return false;
	}

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// SOctree.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

#endif // #ifdef ACTIVE_EDITORS