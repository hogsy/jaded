// ---------------------------------------------------------------------------------------------------------------------------------
//  ____   _____ _____      _     
// |  _ \ / ____|  __ \    | |    
// | |_) | (___ | |__) |   | |__  
// |  _ < \___ \|  ___/    | '_ \ 
// | |_) |____) | |      _ | | | |
// |____/|_____/|_|     (_)|_| |_|
//                                
//                                
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

#ifndef	_H_BSP
#define _H_BSP

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "fstl/fstl.h"
#include "geom/geom.h"

// ---------------------------------------------------------------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------------------------------------------------------------

class	ProgressDlg;

// ---------------------------------------------------------------------------------------------------------------------------------

class	BSP
{
public:
	// Types

	typedef	struct	tag_build_info
	{
		RadPrimList *			scenePolys;
		RadPrimPointerArray		polys;
		unsigned int			quantizeResolution;
		float				leastDepthErrorBoundsPercent;
		ProgressDlg *			progressDialog;
		unsigned int			totalPolys;
		unsigned int			originalPolys;
		unsigned int			polysUsed;
		unsigned int			maxDepth;
		unsigned int			totalDepth;
		unsigned int			totalNodes;
		float				lastPercent;
		fstl::reservoir<BSP, 256>	bspNodeReservoir;
	} sBuildInfo;

	typedef	struct	tag_poly_group
	{
		unsigned int			leastDepthIndex;
		unsigned int			leastDepthSplits;
		float				leastDepthRatio;
		RadPrimPointerArray		polys;
	} sGroup;

	typedef	fstl::array<sGroup>		GroupArray;

	// Construction/Destruction

inline						BSP(): _front(static_cast<BSP *>(0)), _back(static_cast<BSP *>(0)) {}

	// Implementation

virtual		bool				build(sBuildInfo & buildInfo);
virtual		void				getOrderedList(const geom::Point3 & viewpoint, RadPrimPointerListGrainy & orderedList);

	// Accessors

inline		RadPrimPointerArray &		polys()		{return _polys;};
inline	const	RadPrimPointerArray &		polys() const	{return _polys;};
inline		BSP *&				front()		{return _front;};
inline	const	BSP *				front() const	{return _front;};
inline		BSP *&				back()		{return _back;};
inline	const	BSP *				back() const	{return _back;};

private:
	// Data members

		RadPrimPointerArray		_polys;
		BSP *				_front;
		BSP *				_back;

virtual		bool				recursiveBuild(sBuildInfo & buildInfo, const unsigned int depth = 0);
};

#endif // _H_BSP
// ---------------------------------------------------------------------------------------------------------------------------------
// BSP.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
