// ---------------------------------------------------------------------------------------------------------------------------------
//  _      __  __              _____                                 
// | |    |  \/  |            / ____|                                
// | |    | \  / | __ _ _ __ | |  __  ___ _ __       ___ _ __  _ __  
// | |    | |\/| |/ _` | '_ \| | |_ |/ _ \ '_ \     / __| '_ \| '_ \ 
// | |____| |  | | (_| | |_) | |__| |  __/ | | | _ | (__| |_) | |_) |
// |______|_|  |_|\__,_| .__/ \_____|\___|_| |_|(_) \___| .__/| .__/ 
//                     | |                              | |   | |    
//                     |_|                              |_|   |_|    
//
// Description:
//
//   Lightmap generation
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   10/10/2001 by Paul Nettle: Original creation
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

#include "RadLMap.h"
#include "LMapGen.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ---------------------------------------------------------------------------------------------------------------------------------

static	const	float	epsilon = 0.00001f;
const	unsigned int	LMapGen::_borderPixels = 1;

// ---------------------------------------------------------------------------------------------------------------------------------

static	int planeCompare(const void *elem1, const void *elem2)
{
	const RadPrimPointer &	a = *reinterpret_cast<const RadPrimPointer *>(elem1);
	const RadPrimPointer &	b = *reinterpret_cast<const RadPrimPointer *>(elem2);

	float	dist = a->plane().D() - b->plane().D();

	if (fstl::abs(dist) < epsilon) return 0;
	if (dist > 0) return 1;
	return -1;
}

// ---------------------------------------------------------------------------------------------------------------------------------

	LMapGen::LMapGen()
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

	LMapGen::~LMapGen()
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	LMapGen::generate(RadPrimList & polygons, RadLMapArray & lightmaps)
{
	// Combine primitives into completely connected entities, even if concave

	CombinedPolyList	cpl;
	if (!buildCombinedPolygons(polygons, cpl)) return false;

	// Clip any Combined polygons that extend beyond the lightmap dimensions

	if (!clipCombinedPolygons(cpl, polygons, lightmapWidth(), lightmapHeight())) return false;

	// Start adding Combined polygons to the lightmaps, until none are left

	if (!populateLightmaps(cpl, lightmaps, lightmapWidth(), lightmapHeight())) return false;

	// Debug code -- generate lightmaps so we can see where stuff is
	#if 0
	{
		RadLMapArray	lmaps;
		RadLMap		blank(lightmapWidth(),lightmapHeight());
		lmaps.populate(blank, lightmaps.size());

		for (CombinedPolyList::node *i = cpl.head(); i; i = i->next())
		{
			CombinedPoly &	cp = i->data();
			if (!cp.complete()) continue;
			RadLMap &	lm = lmaps[cp.lightmapID()];

			unsigned int	ptr = static_cast<unsigned int>(&cp);
			unsigned int	r = ((ptr >> 0)&0x7f) + 0x70;
			unsigned int	g = ((ptr >> 1)&0x7f) + 0x70;
			unsigned int	b = ((ptr >> 2)&0x7f) + 0x70;
			geom::Color3	clr(r, g, b);

			for (unsigned int j = 0; j < cp.primitives().size(); ++j)
			{
				RadPrim &	prim = *cp.primitives()[j];
				int	minX, maxX, minY, maxY;
				prim.calcIntegerUVExtents(minX, maxX, minY, maxY);

				for (unsigned int y = minY; y <= maxY; ++y)
				{
					for (unsigned int x = minX; x <= maxX; ++x)
					{
						lm.data()[y * lm.width() + x] = clr;
					}
				}
			}
		}

		{
			for (unsigned int i = 0; i < lightmaps.size(); ++i)
			{
				RadLMap &	lm = lmaps[i];
				lm.id() = i;
				lm.writeRaw("j:\\t");

			}
		}
	}
	#endif

	// Done

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	LMapGen::buildCombinedPolygons(const RadPrimList & polygons, CombinedPolyList & cpl) const
{
	// DJ_TEMP : progress.setCurrentStatus("Preparing to generate lightmaps");
	// DJ_TEMP : progress.setCurrentPercent(0);

	// Start fresh

	cpl.erase();

	// We'll be sorting, so copy our polygon list into a list of polygon pointers for faster sorting

	RadPrimPointerArray	ptrArray;
	{
		// Reserve for speed

		ptrArray.reserve(polygons.size());

		for (RadPrimList::node *i = polygons.head(); i; i = i->next())
		{
			ptrArray += &i->data();
		}
	}

	// Sort the list

	qsort(static_cast<void *>(&ptrArray[0]), ptrArray.size(), sizeof(RadPrimPointer), planeCompare);

	// DJ_TEMP : progress.setCurrentPercent(20);

	// Build a primary list of Combined polygons... these are all polygons that share the same plane, but are not necessarily
	// connected
	{
		float	lastD = ptrArray[0]->plane().D() + 1000; // make sure the 'last' value is _not_ the same as the first

		for (unsigned int i = 0; i < ptrArray.size(); ++i)
		{
			RadPrim &	prim = *ptrArray[i];

			float	dist = fstl::abs(prim.plane().D() - lastD);
			if (dist > epsilon)
			{
				CombinedPoly	cp;
				cpl += cp;
				lastD = prim.plane().D();
			}

			// Add this poly to the last Combined primitive in the list

			cpl.tail()->data().primitives() += &prim;
		}
	}

	// DJ_TEMP : progress.setCurrentPercent(40);

	// The list of Combined polygons contains pieces with the same D, but not necessarily the same normal. We'll also separate
	// polygons that don't have the same material properties (like illumination color)...
	{
		for (CombinedPolyList::node *i = cpl.head(); i; i = i->next())
		{
			CombinedPoly &	cp = i->data();

			// We'll be setting some polygons aside in a new Combined polygon...

			CombinedPoly	keepCP;
			CombinedPoly	tossCP;

			// Visit each polygon in the list

			geom::Vector3	lastNormal = cp.primitives()[0]->plane().normal();
			geom::Color3	lastIllumination = cp.primitives()[0]->illuminationColor();
			geom::Color3	lastReflectance = cp.primitives()[0]->reflectanceColor();
			keepCP.primitives() += cp.primitives()[0];

			for (unsigned int j = 1; j < cp.primitives().size(); ++j)
			{
				RadPrim &	prim = *cp.primitives()[j];
				unsigned int	paIndex = prim.calcPrimaryAxisIndex();

				// If this primitive is very different from the last normal, shove it in a new Combined poly

				if ((prim.plane().normal() ^ lastNormal) > 1-epsilon && prim.illuminationColor() == lastIllumination && prim.reflectanceColor() == lastReflectance)
				{
					keepCP.primitives() += &prim;
				}
				else
				{
					tossCP.primitives() += &prim;
				}
			}

			// What's the jury say? Do we keep them all?

			if (!tossCP.primitives().size()) continue;

			// We're tossing some and keeping some...

			cp.primitives() = keepCP.primitives();
			cpl += tossCP;
		}
	}

	// DJ_TEMP : progress.setCurrentPercent(60);

	// Our list still isn't done yet... We need to go through and separate Combined polygons by those polygons that contain
	// connected points (yes, points, not edges -- fortunately, this is easier :)
	{
		for (CombinedPolyList::node *i = cpl.head(); i; i = i->next())
		{
			CombinedPoly &	cp = i->data();

			// We'll be setting some polygons aside in a new Combined polygon...

			CombinedPoly	keepCP;
			CombinedPoly	tossCP = cp;

			// First polygon into the keep list, the rest in the toss list

			keepCP.primitives() += tossCP.primitives()[0];
			tossCP.primitives().erase(0, 1);

			// List of points from the keep list

			geom::Vector3Array	points = keepCP.primitives()[0]->xyz();

			// Go through the toss list, over and over, adding any primitives to the keep list that match the points list.
			// Also, each time a toss polygon is added, we add its points to the points list, so that we compare future
			// primitives to those points. After all is said and done, we'll have a set of primitives that contain
			// connected points, even if the poitns are connected over a series of polygons (a connects to be, which
			// connects to c which connects do d, but a and d do not directly connect.) Hope that makes sense! :)

			while (tossCP.primitives().size())
			{
				// Visit each polygon in the toss list and see if it can be moved to the keep list

				bool	found = false;

				for (unsigned int j = 0; j < tossCP.primitives().size() && !found; ++j)
				{
					RadPrim &	prim = *tossCP.primitives()[j];

					for (unsigned int k = 0; k < points.size() && !found; ++k)
					{
						for (unsigned int l = 0; l < prim.xyz().size() && !found; ++l)
						{
							// If we found one, move this point over and start the process over again

							if (prim.xyz()[l] == points[k])
							{
								points += prim.xyz();
								keepCP.primitives() += &prim;
								tossCP.primitives().erase(j, 1);
								found = true;
							}
						}
					}
				}

				// If we never found one, we're done

				if (!found) break;
			}

			// What's the jury say? Do we keep them all?

			if (!tossCP.primitives().size()) continue;

			// We're tossing some and keeping some...

			cp.primitives() = keepCP.primitives();
			cpl += tossCP;
		}
	}

	// DJ_TEMP : progress.setCurrentPercent(80);

	// Generate mapping coordinates...
	{
		for (CombinedPolyList::node * i = cpl.head(); i; i = i->next())
		{
			CombinedPoly &	cp = i->data();
			cp.mapWorldTexture(uTexelsPerUnit(), vTexelsPerUnit());
		}
	}

	// DJ_TEMP : progress.setCurrentPercent(100);

	// Done

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	LMapGen::clipCombinedPolygons(CombinedPolyList & cpl, RadPrimList & polygons, const unsigned int limitU, const unsigned int limitV) const
{
	// Scan the list of Combined polygons, looking for those that need to be clipped. Note that if we do clip, we'll be
	// adding them to the end of the list that we're currently scanning. But that's okay, because that works fine. :)

	// DJ_TEMP : progress.setCurrentStatus("Clipping polygons to lightmaps");

	unsigned int	idx = 0;
	for (CombinedPolyList::node * i = cpl.head(); i; i = i->next(), ++idx)
	{
		if (!(idx & 0xf))
		{
			// DJ_TEMP : progress.setCurrentPercent(static_cast<float>(idx) / static_cast<float>(cpl.size()) * 100.0f);
			// DJ_TEMP : if (progress.stopRequested()) throw "";
		}

		CombinedPoly &	cp = i->data();

		// Skip those that don't need clipping

		if (cp.widthIncludingBorder() <= limitU && cp.heightIncludingBorder() <= limitV) continue;

		// We'll need a primitive, any primitive, to work with

		RadPrim &	firstPrim = *cp.primitives()[0];

		// The actual upper-left-most part of the of the upper-left-most lightmap texel

		geom::Point2	minUV = geom::Point2(static_cast<float>(cp.minU()), static_cast<float>(cp.minV()));

		// The delta that goes from a vertex (any vertex) to the point in 3-space where the upper-left texel begins

		geom::Vector2	delta2 = minUV - firstPrim.uv()[0];

		// Find the upper-left 3D coordinate

		geom::Point3	minXYZ = firstPrim.xyz()[0] + firstPrim.uXFormVector() * delta2.u() + firstPrim.vXFormVector() * delta2.v();

		// Two planes: one horizontal and one vertical for slicing up a grid of patches and elements

		geom::Plane3	uPlane(minXYZ, firstPrim.plane().normal() % firstPrim.vXFormVector());
		geom::Plane3	vPlane(minXYZ, firstPrim.plane().normal() % firstPrim.uXFormVector());
		{
			// Make sure these planes face the interior of the primitive

			geom::Point3	primCenter = firstPrim.calcCenterOfMass();
			if (uPlane.halfplane(primCenter) < 0) uPlane.vector() = - uPlane.vector();
			if (vPlane.halfplane(primCenter) < 0) vPlane.vector() = - vPlane.vector();
		}

		// We'll shove our newly clipped polygons over here...

		CombinedPoly	newCPu;
		CombinedPoly	newCPv;

		// Slice and dice: first pass, uPlane...
		{
			// We move our vPlane far enough to cover the width of an entire lightmap. Note that we need to subtract
			// 2 for the pixel buffer, but we also need to subtract one more, because of the way the math works out.

			uPlane.origin() += firstPrim.uXFormVector() * static_cast<float>(lightmapWidth() - borderPixels()*2 - 1);

			for (unsigned int j = 0; j < cp.primitives().size();)
			{
				RadPrim &	prim = *cp.primitives()[j];

				// Slice it

				RadPrim	back;
				prim.bisect(uPlane, back);

				// If it's entirely on the back-side, put it back and skip it (it never got clipped)

				if (!prim.xyz().size())
				{
					prim = back;
					++j;
					continue;
				}

				// If it's entirely on the front side, it gets moved to the new CP

				if (!back.xyz().size())
				{
					cp.primitives().erase(j, 1);
					newCPu.primitives() += &prim;
					continue;
				}

				// We got a clip. We add the front side to the new CP and keep the back-side

				polygons += prim;
				newCPu.primitives() += &polygons.tail()->data();
				prim = back;
				++j;
			}
		}

		// ...second pass, vPlane
		{
			// We move our vPlane far enough to cover the width of an entire lightmap. Note that we need to subtract
			// 2 for the pixel buffer, but we also need to subtract one more, because of the way the math works out.

			vPlane.origin() += firstPrim.vXFormVector() * static_cast<float>(lightmapHeight() - borderPixels()*2 - 1);

			for (unsigned int j = 0; j < cp.primitives().size();)
			{
				RadPrim &	prim = *cp.primitives()[j];

				// Slice it

				RadPrim	back;
				prim.bisect(vPlane, back);

				// If it's entirely on the back-side, put it back and skip it (it never got clipped)

				if (!prim.xyz().size())
				{
					prim = back;
					++j;
					continue;
				}

				// If it's entirely on the front side, it gets moved to the new CP

				if (!back.xyz().size())
				{
					cp.primitives().erase(j, 1);
					newCPv.primitives() += &prim;
					continue;
				}

				// We got a clip. We add the front side to the new CP and keep the back-side

				polygons += prim;
				newCPv.primitives() += &polygons.tail()->data();
				prim = back;
				++j;
			}
		}

		// Our new Combined primitives

		if (newCPu.primitives().size())
		{
			newCPu.calcExtents();
			cpl += newCPu;
		}

		if (newCPv.primitives().size())
		{
			newCPv.calcExtents();
			cpl += newCPv;
		}

		// In case we updated the CP, recalc its extents

		cp.calcExtents();
	}

	// Remove empty Combined polygons
	{
		for (CombinedPolyList::node * i = cpl.head(); i;)
		{
			CombinedPoly &	cp = i->data();
			if (!cp.primitives().size())
			{
				CombinedPolyList::node * next = i->next();
				cpl.erase(i, 1);
				i = next;
			}
			else
			{
				i = i->next();
			}
		}
	}

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	LMapGen::populateLightmaps(CombinedPolyList & cpl, RadLMapArray & lightmaps, const unsigned int limitU, const unsigned int limitV) const
{
	// DJ_TEMP : progress.setCurrentStatus("Populating lightmaps");

	unsigned int	total = 0;
	unsigned int	lightmapCount = 0;

	for(;;)
	{
		// DJ_TEMP : progress.setCurrentPercent(static_cast<float>(total) / static_cast<float>(cpl.size()) * 100.0f);
		// DJ_TEMP : if (progress.stopRequested()) throw "";

		unsigned int	count = populateLightmap(cpl, lightmapCount, Rect(0, 0, limitU, limitV));
		if (!count) break;
		total += count;
		++lightmapCount;
	}

	// Make sure we actually mapped all polygons

	if (total != cpl.size()) return false;

	// Generate lightmaps

	lightmaps.erase();
	RadLMap		blank(limitU, limitV);
	lightmaps.populate(blank, lightmapCount);

	for (unsigned int i = 0; i < lightmapCount; ++i)
	{
		lightmaps[i].id() = i;
	}

	// Done

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

unsigned int	LMapGen::populateLightmap(CombinedPolyList & cpl, const unsigned int id, Rect rect) const
{
	// Get the width/height of this rectangle

	if (!rect.width() || !rect.height()) return 0;

	// We'll be building a list of available rects, the input rect is the primer

	Rect::RectList	availableRects;
	availableRects += rect;

	// Used to keep track of the number of Combined polygons assigned to this lightmap

	unsigned int	assignedCount = 0;

	// Loop until we run out of useful lightmap area

	while(availableRects.size())
	{
		// Find the largest rect

		Rect	workingRect;
		{
			Rect::RectList::node *	largestRect = static_cast<Rect::RectList::node *>(0);
			unsigned int		largestArea = 0;
			for (Rect::RectList::node * i = availableRects.head(); i; i = i->next())
			{
				Rect &		r = i->data();
				unsigned int	area = r.area();

				if (area > largestArea)
				{
					largestArea = area;
					largestRect = i;
				}
			}

			// If we didn't find one, bail (should never happen, but what the heck)

			if (!largestRect) break;

			// Get a working copy, because we're about to remove it from the list

			workingRect = largestRect->data();

			// Remove this rect from the list of available rects

			availableRects.erase(largestRect, 1);
		}

		// Is this rect primarily horizontal or vertical?

		bool	primarilyHorizontal = workingRect.width() >= workingRect.height();

		// Using the current rect, find the largest Combined polygon that fits

		Rect::RectList	occupiedRects;
		{
			unsigned int	maxArea = 0;
			CombinedPoly *	maxCP = static_cast<CombinedPoly *>(0);

			for (CombinedPolyList::node * i = cpl.head(); i; i = i->next())
			{
				CombinedPoly &	cp = i->data();
				if (cp.complete()) continue;

				// Make sure the orientation matches

				cp.setOrientation(primarilyHorizontal);

				// If it won't fit, skip it
				
				if (workingRect.width() < cp.widthIncludingBorder() || workingRect.height() < cp.heightIncludingBorder()) continue;

				// Track the piece with the most area

				if (cp.areaIncludingBorder() > maxArea)
				{
					maxArea = cp.areaIncludingBorder();
					maxCP = & cp;
				}
			}

			// If we didn't find a match, then this rect is too small, so skip it

			if (!maxCP) continue;

			// We found a match, set it up

			maxCP->offsetU() = workingRect.minX();
			maxCP->offsetV() = workingRect.minY();
			maxCP->lightmapID() = id;
			maxCP->complete() = true;

			// Remap it

			maxCP->remap();

			// Keep track of how many Combined polygons were assigned

			assignedCount++;

			// Add the rects from the primitives to the occupiedRects list

			RadPrimPointerArray &	primArray = maxCP->primitives();
			for (unsigned int j = 0; j < primArray.size(); ++j)
			{
				RadPrim &	prim = *primArray[j];

				// Get the rect for this primitive

				Rect		occupiedRect;
				prim.calcIntegerUVExtents(occupiedRect.minX(), occupiedRect.maxX(), occupiedRect.minY(), occupiedRect.maxY());

				// Account for the pixel border

				occupiedRect.minX() -= borderPixels();
				occupiedRect.minY() -= borderPixels();
				occupiedRect.maxX() += borderPixels() + 1;
				occupiedRect.maxY() += borderPixels() + 1;

				// Add it to the occupied list

				occupiedRects += occupiedRect;
			}
		}

		// -----------------------------------------------------------------------------------------------------------------
		// The following code is a bit cumbersome because we're working with multiple lists, so let's clarify what's going
		// on.
		//
		// We've got these lists:
		//
		//  * availableRects - this is a list of rectangular regions within the lightmap that are completely available.
		//
		//  * workingList - this list is the list of rectangles that have been removed from the available list because
		//    they overlap the Combined polygon that we're currently working with. However, the Combined polygon may not be
		//    completely covering this rectangle. So we want to take the pieces that are still available (within this
		//    workingList) and return them back to the availableRects list.
		//
		//  * occupiedRects - this is the list of rects from the Combined polygon. When all is said and done, these rects
		//    should not overlap any rects in the availableRects list. This, after all, is our goal.
		//
		// ** Note about the block of pseudo-code below: I wrote this to help keep it straight in my mind. Chances are, the
		//    actual implementation won't resemble this, or it may lose any resemblance over time and maintainence. Don't
		//    believe everything you read in this pseudo-code. I'm leaving it in here only because (1) it seems like a waste
		//    to remove it, and (2) it might give you some insight as to the original intent of this code. If you really
		//    want to know what's going on, read the actual code (below this comment block) and follow the comments.
		//
		//	// Keep slicing until we can't slice anything else
		//
		//	for (;;)
		//	{
		//		unsigned int		largestArea = 0;
		//		RectList::node *	largestOccupiedRect;
		//		RectList::node *	largestWorkingRect;
		//
		//		for (each rect in workingList)
		//		{
		//			Rect	workingRect = current rect from the workingList;
		//
		//			for (each rect in occupiedRects)
		//			{
		//				Rect	occupiedRect = current rect from the occupiedRects;
		//
		//				clip the occupiedRect to the workingRect, skip this occupied rect if no overlap;
		//
		//				Find the four pieces of working rect that extened past the four extents of occupiedRect;
		//
		//				Find the largest area of those four pieces;
		//
		//				if (any of those four pieces is larger than the current largestArea)
		//				{
		//					largestArea = largestPiece.area();
		//					largestOccupiedRect = occupiedRect's node from the list;
		//					largestWorkingRect = workingRect's node from the list;
		//				}
		//			}
		//		}
		//
		//		// If there were no overlaps, we're done
		//
		//		if (!largestArea) break;
		//
		//		// We now have the best cut from all of the working rects and occupied rects. We'll slice up the working rect
		//		// into four pieces (ignoring those pieces that have no area) and replace it in the list with the fragments.
		//
		//		[do what the comment above says :]
		//	}
		//
		//	// Okay.. at this point, we've sliced up the working list and removed those portions of it that contain the occupied
		//	// list. These pieces are now available... add them to the availableList.
		//
		//	availableList += workingList;
		//
		// -----------------------------------------------------------------------------------------------------------------
		
		// Our working list gets primed with the one rect that covers the entire region of the CombinedPoly's area within
		// the lightmap.

		Rect::RectList	workingRects;
		workingRects += workingRect;

		// Keep slicing until we can't slice anything else

		for (;;)
		{
			Rect::RectArray		largestPieces;
			unsigned int		largestArea = 0;
			Rect::RectList::node *	largestWorkingRect = static_cast<Rect::RectList::node *>(0);

			for (Rect::RectList::node * i = workingRects.head(); i; i = i->next())
			{
				Rect &	workingRect = i->data();

				for (Rect::RectList::node * j = occupiedRects.head(); j; j = j->next())
				{
					Rect	occupiedRect = j->data();

					// Find the (up to) four pieces of workingRect that extened past the four extents of
					// occupiedRect. This is effectively a boolean subtraction. Fortunately, these are
					// rects we're dealing with. :)

					bool	emptyResult;
					Rect::RectArray	nonOverlappingRects = workingRect.booleanSubtract(occupiedRect, emptyResult);

					// If we get an empty result, we need to remove it from the list without adding any
					// pieces.

					if (emptyResult)
					{
						largestWorkingRect = i;
						largestPieces.erase();
						break;
					}

					// If nothing exists, skip it

					if (!nonOverlappingRects.size()) continue;

					// Find the largest area the boolean result pieces

					unsigned int	maxArea = nonOverlappingRects[0].area();
					for (unsigned int k = 1; k < nonOverlappingRects.size(); ++k)
					{
						unsigned int	a = nonOverlappingRects[k].area();
						if (a > maxArea) maxArea = a;
					}

					// Is the largest of the four pieces larger than the largestArea thus far?

					if (maxArea > largestArea)
					{
						largestArea = maxArea;
        					largestWorkingRect = i;
						largestPieces = nonOverlappingRects;
					}
				}
			}

			// If there were no overlaps, we're done

			if (!largestWorkingRect) break;

			// We now have the best cut from all of the working rects and occupied rects. We also have the sliced-up
			// fragments of the working rect. So replace the working rect with those fragments that have any area.

			workingRects.erase(largestWorkingRect, 1);

			for (unsigned int j = 0; j < largestPieces.size(); j++)
			{
				Rect &	piece = largestPieces[j];
				if (piece.area()) workingRects += piece;
			}
		}

		// Okay.. at this point, we've sliced up the working list and removed those portions of it that contain the occupied
		// list. These pieces are now available... add them to the availableList.

		availableRects += workingRects;

		// Go through the available Rects and merge rects wherever possible. We merge based on largest area.

		for (;;)
		{
			// Find the largest result of two combined rects

			Rect::RectList::node *	aRect = static_cast<Rect::RectList::node *>(0);
			Rect::RectList::node *	bRect = static_cast<Rect::RectList::node *>(0);
			unsigned int		largestArea = 0;

			for (Rect::RectList::node * i = availableRects.head(); i; i = i->next())
			{
				Rect &	ar = i->data();

				for (Rect::RectList::node * j = i->next(); j; j = j->next())
				{
					Rect &	br = j->data();

					// Share vertical edge?

					if (!ar.canMergeWith(br)) continue;

					// What's the conbined area?

					unsigned int	combinedArea = ar.area() + br.area();

					// Largest area thus far?

					if (combinedArea > largestArea)
					{
						largestArea = combinedArea;
						aRect = i;
						bRect = j;
					}
				}
			}

			// Nothing to combine?

			if (!aRect || !bRect) break;

			// Get the combined rect

			Rect	combinedRect;
			combinedRect.boundingRect(aRect->data(), bRect->data());

			// Okay, we'll remove the two rects from the list and add their combined rect

			availableRects.erase(aRect, 1);
			bRect->data() = combinedRect;
		}
	}

	// Done

	return assignedCount;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// LMapGen.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

#endif // #ifdef ACTIVE_EDITORS