// ---------------------------------------------------------------------------------------------------------------------------------
//   _____  ____       _                      _     
//  / ____|/ __ \     | |                    | |    
// | (___ | |  | | ___| |_ _ __  ___  ___    | |__  
//  \___ \| |  | |/ __| __| '__|/ _ \/ _ \   | '_ \ 
//  ____) | |__| | (__| |_| |  |  __/  __/ _ | | | |
// |_____/ \____/ \___|\__|_|   \___|\___|(_)|_| |_|
//                                                  
//                                                  
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

#ifndef	_H_SOCTREE
#define _H_SOCTREE

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "GeomDB.h"
#include "RadPrim.h"
#include "RadPatch.h"
#include "BSP.h"

// ---------------------------------------------------------------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------------------------------------------------------------

class	ProgressDlg;

// ---------------------------------------------------------------------------------------------------------------------------------

class	SOctree
{
public:
	typedef	struct
	{
		// Octree stuff

		unsigned int			thresholdLimiter;
		float				minRadiusLimiter;
		unsigned int			maxDepthLimiter;
		unsigned int			maxDepth;
		unsigned int			originalPolys;
		unsigned int			totalPolys;
		unsigned int			totalLights;
		unsigned int			totalLightmaps;
		unsigned int			totalDepth;
		unsigned int			totalNodes;
		unsigned int			patchSubdivisionV;
		unsigned int			patchSubdivisionU;
		float				totalSceneSurfaceArea;
		float				processedSurfaceArea;
		fstl::reservoir<SOctree, 256>	octreeNodeReservoir;

		// BSP stuff

		unsigned int			bspQuantizeResolution;
		float				bspLeastDepthErrorBoundsPercent;
	} sBuildInfo;
	
	typedef	fstl::array<SOctree*, 8>	SOctreePointerArray;

	typedef	bool (*SOctreeTraversalHandler)(SOctree & node, void * userData);

	// Construction/Destruction

inline						SOctree() {}
virtual						~SOctree();

	// Implementation

virtual		bool				build(GeomDB & geometry, sBuildInfo & bi);
virtual		bool				traverse(SOctreeTraversalHandler th, void *userData);

	// Accessors

inline		geom::Point3 &			center()		{return _center;}
inline	const	geom::Point3 &			center() const		{return _center;}
inline		float &				radius()		{return _radius;}
inline	const	float				radius() const		{return _radius;}
inline		SOctreePointerArray &		children()		{return _children;}
inline	const	SOctreePointerArray &		children() const	{return _children;}
inline		BSP &				bsp()			{return _bsp;}
inline	const	BSP &				bsp() const		{return _bsp;}
inline		RadPrimList &			polys()			{return _polys;}
inline	const	RadPrimList &			polys() const		{return _polys;}

private:
	// Utilitarian

virtual		bool				recursiveBuild(RadPrimList & inPolys, const RadPatchList & inLights, sBuildInfo & bi, const unsigned int depth = 0);

	// Data members

		geom::Point3			_center;
		float				_radius;
		SOctreePointerArray		_children;
		BSP				_bsp;
		RadPrimList			_polys;

	// We store this here, because it contains the reservoir for all BSP nodes, and must remain valid for the duration of
	// the octree's lifetime

		BSP::sBuildInfo			buildInfo;
};

#endif // _H_SOCTREE
// ---------------------------------------------------------------------------------------------------------------------------------
// SOctree.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
