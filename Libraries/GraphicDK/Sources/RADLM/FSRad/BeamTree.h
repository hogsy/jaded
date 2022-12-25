// ---------------------------------------------------------------------------------------------------------------------------------
//  ____                       _______                    _     
// |  _ \                     |__   __|                  | |    
// | |_) | ___  __ _ _ __ ___    | |   _ __  ___  ___    | |__  
// |  _ < / _ \/ _` | '_ ` _ \   | |  | '__|/ _ \/ _ \   | '_ \ 
// | |_) |  __/ (_| | | | | | |  | |  | |  |  __/  __/ _ | | | |
// |____/ \___|\__,_|_| |_| |_|  |_|  |_|   \___|\___|(_)|_| |_|
//                                                              
//                                                              
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

#ifndef	_H_BEAMTREE
#define _H_BEAMTREE

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "geom/geom.h"
#include "RadPrim.h"

// ---------------------------------------------------------------------------------------------------------------------------------

class	BeamTree
{
private:
		typedef	struct tag_beamtree_edge
		{
			geom::Point3	xyz;
			geom::Point2	uv;
			float		delta;
			bool		closed;
		} BTEdge;

		typedef	fstl::reservoir<BeamTree, 256>	BeamTreeReservoir;

public:
	// Construction/Destruction

inline					BeamTree()
					: _front(static_cast<BeamTree *>(0)), _back(static_cast<BeamTree *>(0)), _reservoir(static_cast<BeamTreeReservoir *>(0))
					{
					}

inline					BeamTree(geom::Plane3Array & planes)
					: _front(static_cast<BeamTree *>(0)), _back(static_cast<BeamTree *>(0)), _reservoir(static_cast<BeamTreeReservoir *>(0))
					{
						init(planes);
					}

virtual					~BeamTree()
					{
						uninit();
					}

	// Implementation

virtual		void			uninit();
virtual		void			init(geom::Plane3Array & planes);
virtual		void			init(geom::Plane3 & plane);
virtual		void			insert(const geom::Point3 & viewpoint, RadPrim & primitive, RadPrimListGrainy & visiblePieces);
virtual		void			recursiveInsert(const geom::Point3 & viewpoint, BTEdge * edges, const unsigned int edgeCount, BeamTreeReservoir & btPool, RadPrim & primitive, RadPrimListGrainy & visiblePieces);

	// Accessors

inline		geom::Plane3 &		plane()			{return _plane;}
inline	const	geom::Plane3 &		plane() const		{return _plane;}
inline		BeamTree *&		front()			{return _front;}
inline	const	BeamTree *		front() const		{return _front;}
inline		BeamTree *&		back()			{return _back;}
inline	const	BeamTree *		back() const		{return _back;}
inline		BeamTreeReservoir *&	reservoir()		{return _reservoir;}
inline	const	BeamTreeReservoir *	reservoir() const	{return _reservoir;}
inline		BeamTree * const	terminated() const	{return _terminated;}

private:
	// Data members

		geom::Plane3		_plane;
		BeamTree *		_front;
		BeamTree *		_back;
		BeamTreeReservoir *	_reservoir;
static		BeamTree * const	_terminated;
};

#endif // _H_BEAMTREE
// ---------------------------------------------------------------------------------------------------------------------------------
// BeamTree.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
