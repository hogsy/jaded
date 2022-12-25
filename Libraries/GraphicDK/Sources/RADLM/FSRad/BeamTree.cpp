// ---------------------------------------------------------------------------------------------------------------------------------
//  ____                       _______                                     
// |  _ \                     |__   __|                                    
// | |_) | ___  __ _ _ __ ___    | |   _ __  ___  ___      ___ _ __  _ __  
// |  _ < / _ \/ _` | '_ ` _ \   | |  | '__|/ _ \/ _ \    / __| '_ \| '_ \ 
// | |_) |  __/ (_| | | | | | |  | |  | |  |  __/  __/ _ | (__| |_) | |_) |
// |____/ \___|\__,_|_| |_| |_|  |_|  |_|   \___|\___|(_) \___| .__/| .__/ 
//                                                            | |   | |    
//                                                            |_|   |_|    
//
// Description:
//
//   Beam tree (a 2D bsp tree originated at a viewpoint)
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   09/22/2001 by Paul Nettle: Original creation
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

#include "BeamTree.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ---------------------------------------------------------------------------------------------------------------------------------

BeamTree * const BeamTree::_terminated = reinterpret_cast<BeamTree * const>(-1);

// ---------------------------------------------------------------------------------------------------------------------------------

void	BeamTree::uninit()
{
	front() = static_cast<BeamTree *>(0);
	back() = static_cast<BeamTree *>(0);

	if (reservoir())
	{
		delete reservoir();
		reservoir() = static_cast<BeamTreeReservoir *>(0);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	BeamTree::init(geom::Plane3Array & planes)
{
	// Release the thing if it exists

	uninit();

	if (!planes.size()) return;

	// Build up a set of default planes with terminated backs

	BeamTree *	cur = this;

	for (unsigned int i = 0; i < planes.size(); ++i)
	{
        	cur->plane() = planes[i];
		cur->back() = terminated();

		if (i < planes.size() - 1)
		{
			cur->front() = new BeamTree;
			cur = cur->front();
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	BeamTree::init(geom::Plane3 & plane)
{
	geom::Plane3Array	pa;
	pa += plane;
	init(pa);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	BeamTree::insert(const geom::Point3 & viewpoint, RadPrim & primitive, RadPrimListGrainy & visiblePieces)
{
	// Crate a series of edges from the primitive...

	BTEdge	edges[32];
	for (unsigned int i = 0; i < primitive.xyz().size(); ++i)
	{
		edges[i].xyz = primitive.xyz()[i];
		edges[i].uv  = primitive.uv()[i];
		edges[i].closed = false;
	}

	if (!reservoir()) reservoir() = new BeamTreeReservoir;
	recursiveInsert(viewpoint, edges, primitive.xyz().size(), *reservoir(), primitive, visiblePieces);
}

// ---------------------------------------------------------------------------------------------------------------------------------

// PDNDEBUG -- note.. can clip to the front only if back is deemed terminated.

// ---------------------------------------------------------------------------------------------------------------------------------

void	BeamTree::recursiveInsert(const geom::Point3 & viewpoint, BTEdge * edges, const unsigned int edgeCount, BeamTreeReservoir & btPool, RadPrim & primitive, RadPrimListGrainy & visiblePieces)
{
static	const	float	epsilon = 1.0e-4f;

	// Calculate deltas. Note that edge points close to a plane are snapped to the plane

	unsigned int	posCount = 0;
	unsigned int	negCount = 0;
	unsigned int	firstNonZero = -1;
	{
		for (unsigned int i = 0; i < edgeCount; ++i)
		{
			BTEdge &	e = edges[i];

			// PDNDEBUG -- gotta be a way to use the origin of the plane (0,0,0) to speed this up...

			e.delta = plane().distance(e.xyz);

			if (e.delta < -epsilon) negCount++;
			else if (e.delta > epsilon) posCount++;
			else e.delta = 0;

			// Keep track of the first non-zero delta

			if (firstNonZero == -1 && e.delta) firstNonZero = i;
		}
	}

	// The edges on the front-side of the plane and back-side of the plane

	BTEdge		fEdges[32];
	unsigned int	fEdgeCount = 0;

	BTEdge		bEdges[32];
	unsigned int	bEdgeCount = 0;

	bool		totallyClosed = false;

	// Build a set of edges that represent just those edges on the front & back side of the plane. No edge may span a plane.

	if (!posCount && !negCount)
	{
		// Polygon rests entirely on the plane, cannot be clipped

		return;
	}
	else if (!negCount)
	{
		fstl::memcpy(fEdges, edges, edgeCount);
		fEdgeCount = edgeCount;
	}
	else if (!posCount)
	{
		fstl::memcpy(bEdges, edges, edgeCount);
		bEdgeCount = edgeCount;
	}
	else // must clip
	{
		unsigned int	v0 = firstNonZero;
		bool		posSide = edges[firstNonZero].delta > 0;

		for (;;)
		{
			unsigned int	v1 = v0 + 1;
			if (v1 >= edgeCount) v1 = 0;

			BTEdge &	e0 = edges[v0];
			BTEdge &	e1 = edges[v1];

			// Which side are we currently on?

			if (posSide)
			{
				fEdges[fEdgeCount] = e0;
				fEdgeCount++;

				// Is the next vertex a zero or crossover?

				if (e0.delta > 0 && e1.delta < 0)
				{
					float	dlta = -e1.delta / (e0.delta - e1.delta);
					geom::Point3	clipped3 = e1.xyz + (e0.xyz - e1.xyz) * dlta;
					geom::Point2	clipped2 = e1.uv  + (e0.uv  - e1.uv ) * dlta;

					fEdges[fEdgeCount].closed = false;
					fEdges[fEdgeCount].delta = 0;
					fEdges[fEdgeCount].xyz   = clipped3;
					fEdges[fEdgeCount].uv    = clipped2;
					fEdgeCount++;

					bEdges[bEdgeCount].closed = false;
					bEdges[bEdgeCount].delta = 0;
					bEdges[bEdgeCount].xyz   = clipped3;
					bEdges[bEdgeCount].uv    = clipped2;
					bEdgeCount++;

					// Crossover

					posSide = !posSide;
				}
				else if (e1.delta == 0)
				{
					// Hit a zero edge, add the zero point, too

					fEdges[fEdgeCount] = e1;
					fEdgeCount++;

					// Crossover

					posSide = !posSide;
				}
			}

			// Negative side

			else
			{
				bEdges[bEdgeCount] = e0;
				bEdgeCount++;

				// Is the next vertex a zero or crossover?

				if (e0.delta < 0 && e1.delta > 0)
				{
					float	dlta = -e0.delta / (e1.delta - e0.delta);
					geom::Point3	clipped3 = e0.xyz + (e1.xyz - e0.xyz) * dlta;
					geom::Point2	clipped2 = e0.uv  + (e1.uv  - e0.uv ) * dlta;

					fEdges[fEdgeCount].closed = false;
					fEdges[fEdgeCount].delta = 0;
					fEdges[fEdgeCount].xyz   = clipped3;
					fEdges[fEdgeCount].uv    = clipped2;
					fEdgeCount++;

					bEdges[bEdgeCount].closed = false;
					bEdges[bEdgeCount].delta = 0;
					bEdges[bEdgeCount].xyz   = clipped3;
					bEdges[bEdgeCount].uv    = clipped2;
					bEdgeCount++;

					// Crossover

					posSide = !posSide;
				}
				else if (e1.delta == 0)
				{
					// Hit a zero edge, add the zero point, too

					bEdges[bEdgeCount] = e1;
					bEdgeCount++;

					// Crossover

					posSide = !posSide;
				}
			}

			++v0;
			if (v0 >= edgeCount) v0 = 0;
			if (v0 == firstNonZero) break;
		}
	}


	{
		// Locate closed edges
		//
		// PDNDEBUG -- this is pretty ugly... can this be integrated somehow?

		if (fEdgeCount)
		{
			totallyClosed = true;
			unsigned int	v0 = fEdgeCount - 1;
			for (unsigned int v1 = 0; v1 < fEdgeCount; v0 = v1, ++v1)
			{
				if (fEdges[v0].closed) continue;

				if (!fEdges[v0].delta && !fEdges[v1].delta)
				{
					fEdges[v0].closed = true;
				}
				else
				{
					totallyClosed = false;
				}
			}
		}
		if (bEdgeCount)
		{
			unsigned int	v0 = bEdgeCount - 1;
			for (unsigned int v1 = 0; v1 < bEdgeCount; v0 = v1, ++v1)
			{
				if (bEdges[v0].closed) continue;

				if (!bEdges[v0].delta && !bEdges[v1].delta)
				{
					bEdges[v0].closed = true;
				}
			}
		}
	}

	// Insert into front-side

	if (fEdgeCount && front() != terminated())
	{
		if (!front())
		{
			// Build a set of edge planes that face away from the polygon

			RadPrim	newPrim;
			newPrim.originalPrimitive() = primitive.originalPrimitive();
			newPrim.uXFormVector() = primitive.uXFormVector();
			newPrim.vXFormVector() = primitive.vXFormVector();
			newPrim.plane() = primitive.plane();

			newPrim.xyz().reserve(fEdgeCount);
			newPrim.uv().reserve(fEdgeCount);

			// Closed?

			if (totallyClosed)
			{
				front() = terminated();

				for (unsigned int i = 0; i < fEdgeCount; ++i)
				{
					newPrim.xyz() += fEdges[i].xyz;
					newPrim.uv()  += fEdges[i].uv;
				}

				newPrim.texuv() = newPrim.uv();
				visiblePieces += newPrim;
			}

			// Nope, keep going...

			else
			{
				// Grab a new beam tree object from the reservoir

				front() = &btPool.get();
				BeamTree *	cur = front();

				unsigned int	v0 = fEdgeCount - 1;
				for (unsigned int v1 = 0; v1 < fEdgeCount; v0 = v1, ++v1)
				{
					// PDNDEBUG -- sometimes, this calculation is wasted.. any way to avoid that?

					cur->plane() = geom::Plane3(viewpoint, (fEdges[v0].xyz - viewpoint) % (fEdges[v1].xyz - fEdges[v0].xyz));
					if (!fEdges[v0].closed)
					{
						if (v1 < fEdgeCount - 1)
						{
							cur->back() = &btPool.get();
							cur = cur->back();
						}
					}

					newPrim.xyz() += fEdges[v0].xyz;
					newPrim.uv()  += fEdges[v0].uv;
				}

				cur->back() = terminated();

				newPrim.texuv() = newPrim.uv();
				visiblePieces += newPrim;
			}
		}
		else
		{
			front()->recursiveInsert(viewpoint, fEdges, fEdgeCount, btPool, primitive, visiblePieces);
			if (front()->front() == terminated() && front()->back() == terminated())
			{
				front() = terminated();
			}

			if (totallyClosed)
			{
				front() = terminated();
			}

		}
	}

	// Insert into back-side

	if (bEdgeCount && back() != terminated())
	{
		// back() should always have a value.. but just in case....

		if (back())
		{
			back()->recursiveInsert(viewpoint, bEdges, bEdgeCount, btPool, primitive, visiblePieces);
			if (back()->front() == terminated() && back()->back() == terminated())
			{
				back() = terminated();
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// BeamTree.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

#endif // #ifdef ACTIVE_EDITORS