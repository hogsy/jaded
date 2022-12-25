// ---------------------------------------------------------------------------------------------------------------------------------
//  ____   _____ _____                       
// |  _ \ / ____|  __ \                      
// | |_) | (___ | |__) |     ___ _ __  _ __  
// |  _ < \___ \|  ___/     / __| '_ \| '_ \ 
// | |_) |____) | |      _ | (__| |_) | |_) |
// |____/|_____/|_|     (_) \___| .__/| .__/ 
//                              | |   | |    
//                              |_|   |_|    
//
// Description:
//
//   BSP (binary space partitioning) tree
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   08/03/2001 by Paul Nettle: Original creation
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
#include "BSP.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ---------------------------------------------------------------------------------------------------------------------------------

static	const	float	EPSILON = 1.0e-5f;

// ---------------------------------------------------------------------------------------------------------------------------------

static	int ppaCompare(const void *elem1, const void *elem2)
{
	const RadPrim &	a = **reinterpret_cast<const RadPrim * const *>(elem1);
	const RadPrim &	b = **reinterpret_cast<const RadPrim * const *>(elem2);

	if (a.plane().D() == b.plane().D()) return 0;
	if (a.plane().D() >  b.plane().D()) return 1;
	return -1;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	int RadPrimCompare(const void *elem1, const void *elem2)
{
	const RadPrim &	a = *reinterpret_cast<const RadPrim *>(elem1);
	const RadPrim &	b = *reinterpret_cast<const RadPrim *>(elem2);

	if (a.plane().D() == b.plane().D()) return 0;
	if (a.plane().D() >  b.plane().D()) return 1;
	return -1;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	int groupCompare(const void *elem1, const void *elem2)
{
	const BSP::sGroup &	a = *reinterpret_cast<const BSP::sGroup *>(elem1);
	const BSP::sGroup &	b = *reinterpret_cast<const BSP::sGroup *>(elem2);

	if (a.leastDepthRatio == b.leastDepthRatio) return 0;
	if (a.leastDepthRatio >  b.leastDepthRatio) return 1;
	return -1;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	bool	classifyForQuantization(BSP::sBuildInfo & buildInfo)
{
	float	anglesPerLatStep = 90.0f / static_cast<float>(buildInfo.quantizeResolution) * 4;
	float	anglesPerLonStep = 360.0f / static_cast<float>(buildInfo.quantizeResolution);

	for (unsigned int i = 0; i < buildInfo.polys.size(); ++i)
	{
        // DJ_TEMP
        // enable compute cancelation here
        //...
		//if (!(i&0xff) && cancelation_asked) return false;

		RadPrim &	poly = *buildInfo.polys[i];

		// The current poly's plane's normal

		geom::Vector3	normal = poly.plane().normal();

		// We're only interested in the top of the hemisphere...

		if (normal.y() < 0) normal.y() = -normal.y();

		// Calculate the angle between the apex of the classification hemisphere and the normal (latitude)

		double	tmp = acos(normal ^ geom::Vector3(0, 1, 0));
		float	latAngle = static_cast<float>(fstl::toDegrees(tmp));

		// Find the quantized step from the latitude

		unsigned int	latStep = static_cast<unsigned int>(latAngle / anglesPerLatStep);
		if (latStep >= buildInfo.quantizeResolution/4) latStep = buildInfo.quantizeResolution/4 - 1;

		// If the latStep is > 0, then we also need the longitude step...

		unsigned int	lonStep = 0;
		if (latStep)
		{
			// Generate a 2D unit vector from the normal

			geom::Vector2	v(normal.x(), normal.z());
			v.normalize();

			// Calculate the lonAngle

			double	tmp = acos(v ^ geom::Vector2(0, 1));
			float	lonAngle = static_cast<float>(fstl::toDegrees(tmp));
			if (v.x() < 0) lonAngle = 360 - lonAngle;

			// Find the quantized step from the longitude angle

			lonStep = static_cast<unsigned int>(lonAngle / anglesPerLonStep);
			if (lonStep >= buildInfo.quantizeResolution) lonStep = 0;
		}

		// Index into the quantized polys array...

		unsigned int	index = 0;
		if (latStep)
		{
			index = (latStep-1) * buildInfo.quantizeResolution + lonStep + 1;
		}

		poly.usageIndex() = index;
	}

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	bool	classifyAndQuantize(BSP::sBuildInfo & buildInfo, BSP::GroupArray & groups)
{
	for (unsigned int i = 0; i < buildInfo.polys.size(); ++i)
	{
        // DJ_TEMP
        // enable compute cancelation here
        //...
        //if (!(i&0xff) && cancelation_asked) return false;

		RadPrim &	poly = *buildInfo.polys[i];
		groups[poly.usageIndex()].polys += &poly;
	}

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	bool	unifyGroups(BSP::sBuildInfo & buildInfo, BSP::GroupArray & groups)
{
	// Remove empty groups
	{
		// Create a new list and reserve for speed

		BSP::GroupArray	newList;
		newList.reserve(groups.size());

		for (unsigned int i = 0; i < groups.size(); ++i)
		{
			if (groups[i].polys.size()) newList += groups[i];
		}

		// Move the unique list over

		groups = newList;
	}

	// Sort the polygons in each group based on D from the plane equation
	{
		for (unsigned int i = 0; i < groups.size(); ++i)
		{
            // DJ_TEMP
            // enable compute cancelation here
            //...
            //if (cancelation_asked) return false;

			RadPrimPointerArray &	ppa = groups[i].polys;

			// Must be something worth working on...

			if (ppa.size() <= 1) continue;

			// Remove duplicate planes
			{
				// Create a new list and reserve for speed

				RadPrimPointerArray	newList;
				newList.reserve(ppa.size());

				unsigned int	lastIndex = 0;
				newList += ppa[0];

				for (unsigned int i = 1; i < ppa.size(); ++i)
				{
					if (ppa[i]->plane().D()      != newList[lastIndex]->plane().D() ||
					    ppa[i]->plane().normal() != newList[lastIndex]->plane().normal())
					{
						newList += ppa[i];
						lastIndex++;
					}
				}

				// Move the unique list over (if it changed)

				if (ppa.size() != newList.size()) ppa = newList;
			}
		}	
	}	

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Tests all polys against a bisection plane. Returns the ratio of polygons that land on the front side as well as the number
// of polygons that are bisected by the plane.
// ---------------------------------------------------------------------------------------------------------------------------------

static	bool	tryBisectionPlane(BSP::sBuildInfo & buildInfo, const geom::Plane3 & plane, float & ratio, unsigned int & splitCount)
{
	splitCount = 0;
	unsigned int	fCount = 0;
	unsigned int	bCount = 0;

	for (unsigned int i = 0; i < buildInfo.polys.size(); ++i)
	{
        // DJ_TEMP
        // enable compute cancelation here
        //...
        //if (!(i&0xf) && cancelation_asked) return false;


		const RadPrim &	poly = *buildInfo.polys[i];

		// Skip all polygons whose plane match the splitter's

		if (poly.plane().D() == plane.D() && poly.plane().normal() == plane.normal()) continue;

		bool	front = false;
		bool	back = false;
		for (unsigned int j = 0; j < poly.xyz().size(); ++j)
		{
			// Calculate the distance of this point to the plane

			float	dist = plane.distance(poly.xyz()[j]);
			if (dist > -EPSILON && dist < EPSILON) dist = 0.0f;

			if (dist > 0)	front = true;
			else		back = true;

			// If it's a split, we can early-out

			if (front && back)
			{
				splitCount++;
				break;
			}
		}

		if (front) fCount++;
		if (back) bCount++;
	}

	// Calculate the ratio of front-sided polytons

	ratio = static_cast<float>(fCount) / static_cast<float>(buildInfo.polys.size());

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	bool	findOptimalSplitters(BSP::sBuildInfo & buildInfo, BSP::GroupArray & groups)
{
	// Binary search dataset for optimal splitters

	for (unsigned int i = 0; i < groups.size(); ++i)
	{
        // DJ_TEMP
        // enable compute cancelation here
        //...
        //if (!(i&0x1) && cancelation_asked) return false;

		BSP::sGroup &	g = groups[i];

		// Binary search this group...

		int	sIndex = 0;
		int	eIndex = g.polys.size();

		do
		{
			// The midpoint of the range

			g.leastDepthIndex = (sIndex + eIndex) / 2;

			// We're about to try a bisection by this plane. In order to follow the binary search
			// we'll need this plane to have a reference, so they always point outward from the center
			// of the hemisphere.

			geom::Plane3	plane = g.polys[g.leastDepthIndex]->plane();
			if (plane.vector().y() < 0) plane.vector().y() = -plane.vector().y();

			// Try this bisection

			if (!tryBisectionPlane(buildInfo, plane, g.leastDepthRatio, g.leastDepthSplits)) return false;

			// Convert the least depth ratio to a percent

			g.leastDepthRatio *= 100.0f;

			// At this point, it should be noted that a series of polygons in the least-depth range
			// will all have very close lesat-depth values, but a wide range of split counts...
			//
			// It may be possible to account for this...

			// Perfect?

			if (g.leastDepthRatio == 50.0f) break;

			// Binary search...

			if (g.leastDepthRatio < 50.0f)	eIndex = g.leastDepthIndex - 1;
			else				sIndex = g.leastDepthIndex + 1;
		} while (sIndex < eIndex);
	}

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	bool	findBestSplitter(BSP::sBuildInfo & buildInfo, BSP::GroupArray & groups, RadPrim ** bestSplitter)
{
	// Sort the groups based on leastDepthRatio

	qsort(static_cast<void *>(&groups[0]), groups.size(), sizeof(BSP::sGroup), groupCompare);

	// Track the group with the fewest splits that falls within the bounds specified by the formal parameter
	// 'leastDepthErrorBoundsPercent'.

	float	minErrorBound = 50.0f - buildInfo.leastDepthErrorBoundsPercent;
	float	maxErrorBound = 50.0f + buildInfo.leastDepthErrorBoundsPercent;
	int	minSplits = -1;
	int	minRatio = -1;

	for (unsigned int i = 0; i < groups.size(); ++i)
	{
        // DJ_TEMP
        // enable compute cancelation here
        //...
        //if (cancelation_asked) return false;

		BSP::sGroup &	g = groups[i];

		// Within the error bound?

		if (g.leastDepthRatio >= minErrorBound && g.leastDepthRatio <= maxErrorBound)
		{
			// Fewer splits?

			if (minSplits == -1 || g.leastDepthSplits < groups[minSplits].leastDepthSplits)
			{
				minSplits = i;
			}
		}

		// If we're outside of the error bounds, we'll get as close to 50% as we can, just in case
		// we never find one in range...

		else
		{
			if (minRatio == -1 || fstl::abs(50.0 - g.leastDepthRatio) < fstl::abs(50.0f - groups[minRatio].leastDepthRatio))
			{
				minRatio = i;
			}
		}
	}

	// If we found a min splits node, use it, otherwise use the minDepth node

	int	bestGroupIndex = (minSplits >= 0) ? minSplits : minRatio;
	BSP::sGroup &	bestGroup = groups[bestGroupIndex];
	*bestSplitter = bestGroup.polys[bestGroup.leastDepthIndex];

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	bool	bisectScene(BSP::sBuildInfo & buildInfo, const geom::Plane3 & plane, RadPrimPointerArray & frontList, RadPrimPointerArray & backList, RadPrimPointerArray & thisList)
{
	// Clear the lists

	frontList.erase();
	backList.erase();

	// Reserve for speed...

	frontList.reserve(buildInfo.polys.size());
	backList.reserve(buildInfo.polys.size());

	// Split the polys

	for (unsigned int i = 0; i < buildInfo.polys.size(); ++i)
	{
        // DJ_TEMP
        // enable compute cancelation here
        //...
        //if (!(i&0xff) && cancelation_asked) return false;


		RadPrim &	poly = *buildInfo.polys[i];

		// Skip all polygons whose plane match the splitter's

		if (poly.plane().D() == plane.D() && poly.plane().normal() == plane.normal())
		{
			thisList += &poly;
			continue;
		}

		// Bisect

		RadPrim	back;
		if (!poly.bisect(plane, back)) return false;
		if (poly.xyz().size())	frontList += &poly;

		// If we have a back-sided polygon, we need to do a bit more work...

		if (back.xyz().size())
		{
			// If we split the polygon, then we need to add the new piece

			if (poly.xyz().size())
			{
				// Add the new piece

				(*buildInfo.scenePolys) += back;

				// Finally, add it to the back list, making sure to reference the polygon from the scene polys

				backList += &buildInfo.scenePolys->tail()->data();

				// For stats

				buildInfo.totalPolys++;
			}

			// We didn't split the polygon, the whole thing is on the backside, so revert back to the original polygon
			// and just use the address of the original polygon

			else
			{
				poly = back;
				backList += &poly;
			}
		}
	}

	// Remove waste

	frontList.compact();
	backList.compact();

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

static	bool	updateDisplay(BSP::sBuildInfo &buildInfo, const unsigned int depth)
{
	if (!buildInfo.progressDialog) return true;

	// Update the display

	static	int	periodicUpdate;
	if (!(periodicUpdate & 0xf))
	{
		float	percent = static_cast<float>(buildInfo.polysUsed) / static_cast<float>(buildInfo.totalPolys) * 100.0f;

		// Sometimes the percent can move backwards (because of splits) -- let's not show that

		if (percent > buildInfo.lastPercent)
		{
			// DJ_TEMP : buildInfo.progressDialog->setCurrentPercent(percent);
			buildInfo.lastPercent = percent;
		}

		char	dsp[90];
		sprintf(dsp, "%d", buildInfo.totalNodes);
		// DJ_TEMP : buildInfo.progressDialog->setLabel2("Nodes:");
		// DJ_TEMP : buildInfo.progressDialog->setText2(dsp);

		sprintf(dsp, "%d", buildInfo.totalPolys - buildInfo.originalPolys);
		// DJ_TEMP : buildInfo.progressDialog->setLabel3("Splits:");
		// DJ_TEMP : buildInfo.progressDialog->setText3(dsp);

		sprintf(dsp, "%d", static_cast<int>(static_cast<float>(buildInfo.totalDepth) / static_cast<float>(buildInfo.totalNodes)));
		// DJ_TEMP : buildInfo.progressDialog->setLabel4("Average depth:");
		// DJ_TEMP : buildInfo.progressDialog->setText4(dsp);
	}
	periodicUpdate++;

	// Always check for this...

    // DJ_TEMP
    // enable compute cancelation here
    //...
    //if (cancelation_asked) return false;

	// Track our max depth

	if (depth > buildInfo.maxDepth)
	{
		buildInfo.maxDepth = depth;
		char	dsp[90];
		sprintf(dsp, "%d", depth);
		// DJ_TEMP : buildInfo.progressDialog->setLabel5("Max depth:");
		// DJ_TEMP : buildInfo.progressDialog->setText5(dsp);
	}

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	BSP::build(sBuildInfo & buildInfo)
{
	// Initialization

	buildInfo.maxDepth = 0;
	buildInfo.totalDepth = 0;
	buildInfo.polysUsed = 0;
	buildInfo.totalPolys = buildInfo.scenePolys->size();
	buildInfo.originalPolys = buildInfo.totalPolys;
	buildInfo.totalNodes = 0;
	buildInfo.lastPercent = 0.0f;

    // DJ_TEMP
	//if (buildInfo.progressDialog)
	//{
	//	char	dsp[90];
	//	buildInfo.progressDialog->setCurrentPercent(0.0f);
	//	sprintf(dsp, "%d", buildInfo.originalPolys);
	//	buildInfo.progressDialog->setLabel1("Polygons:");
	//	buildInfo.progressDialog->setText1(dsp);
    //
	//	buildInfo.progressDialog->setCurrentStatus("Sorting...");
	//}

	// Convert the polys to pointers
	{
		RadPrimPointerArray &	ppa = buildInfo.polys;
		ppa.reserve(buildInfo.scenePolys->size());

		// Convert polys to poly pointers for faster sorting

		RadPrimList::node *	n = buildInfo.scenePolys->head();

		while(n)
		{
			ppa += &n->data();
			n = n->next();
		}
	}

	// Sort based on pointers (for speedier sorts)

	qsort(static_cast<void *>(&buildInfo.polys[0]), buildInfo.polys.size(), sizeof(RadPrim *), ppaCompare);

	// Pre-classify the polygons

	// DJ_TEMP : if (buildInfo.progressDialog) buildInfo.progressDialog->setCurrentStatus("Quantizing...");
	if (!classifyForQuantization(buildInfo)) return false;

	// Recursively build the tree

	// DJ_TEMP : if (buildInfo.progressDialog) buildInfo.progressDialog->setCurrentStatus("Building the BSP tree...");
	return recursiveBuild(buildInfo, 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	BSP::recursiveBuild(sBuildInfo & buildInfo, const unsigned int depth)
{
	// Statistics

	buildInfo.totalNodes++;
	buildInfo.totalDepth += depth;

	// Update the display (statistics)

	if (!updateDisplay(buildInfo, depth)) return false;

	// Should never happen...

	if (!buildInfo.polys.size()) return false;

	// Only one poly?

	if (buildInfo.polys.size() == 1)
	{
		polys() = buildInfo.polys;
		buildInfo.polysUsed += 1;
		return true;
	}

	// Pre-populate the quantized polygon array
	//
	// Array size is half of a sphere (half the quantized resolution) latitude segments by
	// a full set of longitude segments. However, since everything in the top segment quantizes
	// to the apex of the hemisphere, then we have a few extra 1s in the code below...

	GroupArray	groups;
	groups.populate(sGroup(), buildInfo.quantizeResolution * (buildInfo.quantizeResolution / 4 - 1) + 1);

	// Classify all the polygons in the quantized hemisphere

	if (!classifyAndQuantize(buildInfo, groups)) return false;

	// Unify the quantized geometry (sort the polygons within them on D, and remove empty groups)

	if (!unifyGroups(buildInfo, groups)) return false;

	// Find the optimal splitters (one splitter per group)

	if (!findOptimalSplitters(buildInfo, groups)) return false;

	// Find the best splitter

	RadPrim *	bestSplitter;
	if (!findBestSplitter(buildInfo, groups, &bestSplitter)) return false;

	// Bisect the scene

	RadPrimPointerArray	frontList, backList;
	frontList.reserve(buildInfo.polys.size());
	backList.reserve(buildInfo.polys.size());
	if (!bisectScene(buildInfo, bestSplitter->plane(), frontList, backList, polys())) return false;

	// Keep track of how many polygons we've added to the scene (split) and removed (proccesed)

	buildInfo.polysUsed += polys().size();

	// Front

	if (frontList.size())
	{
		buildInfo.polys = frontList;
		front() = &buildInfo.bspNodeReservoir.get();
		if (!front()->recursiveBuild(buildInfo, depth+1)) return false;
	}

	// Back

	if (backList.size())
	{
		buildInfo.polys = backList;
		back() = &buildInfo.bspNodeReservoir.get();
		if (!back()->recursiveBuild(buildInfo, depth+1)) return false;
	}

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	BSP::getOrderedList(const geom::Point3 & viewpoint, RadPrimPointerListGrainy & orderedList)
{
	if (!polys().size()) return;

	// Which side is the viewpoint on?

	float	halfplane = polys()[0]->plane().halfplane(viewpoint);

	// Am I on the front side?

	if (halfplane > 0)
	{
		if (back()) back()->getOrderedList(viewpoint, orderedList);

		// Add the primitives...

		for (unsigned int i = 0; i < polys().size(); ++i)
		{
			// PDNDEBUG -- any way to avoid this situation?

			if (polys()[i]->plane().halfplane(viewpoint) > 0)
			{
				orderedList += polys()[i];
			}
		}

		if (front()) front()->getOrderedList(viewpoint, orderedList);
	}
	else
	{
		if (front()) front()->getOrderedList(viewpoint, orderedList);
		if (back()) back()->getOrderedList(viewpoint, orderedList);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// BSP.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

#endif // #ifdef ACTIVE_EDITORS