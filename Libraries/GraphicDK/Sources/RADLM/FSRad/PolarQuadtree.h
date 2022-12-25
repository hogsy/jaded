// ---------------------------------------------------------------------------------------------------------------------------------
//  _____        _             ____                  _ _                      _     
// |  __ \      | |           / __ \                | | |                    | |    
// | |__) | ___ | | __ _ _ __| |  | |_   _  __ _  __| | |_ _ __  ___  ___    | |__  
// |  ___/ / _ \| |/ _` | '__| |  | | | | |/ _` |/ _` | __| '__|/ _ \/ _ \   | '_ \ 
// | |    | (_) | | (_| | |  | |__| | |_| | (_| | (_| | |_| |  |  __/  __/ _ | | | |
// |_|     \___/|_|\__,_|_|   \___\_\\__,_|\__,_|\__,_|\__|_|   \___|\___|(_)|_| |_|
//                                                                                  
//                                                                                  
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

#ifndef	_H_POLARQUADTREE
#define _H_POLARQUADTREE

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "geom/geom.h"
#include "RadPrim.h"

// ---------------------------------------------------------------------------------------------------------------------------------

class	PolarQuadtree
{
private:
	typedef	struct	_tag_pqt_vert
	{
		geom::Point2	polar;
	} sPQTVert;

	class	PQTNode
	{
	public:

		// Implementation

	inline		void				clearChildren()
							{
								children()[0] = static_cast<PQTNode *>(0);
								children()[1] = static_cast<PQTNode *>(0);
								children()[2] = static_cast<PQTNode *>(0);
								children()[3] = static_cast<PQTNode *>(0);
							}
		// Accessors

	inline		geom::Point2 &			min()			{return _min;}
	inline	const	geom::Point2 &			min() const		{return _min;}
	inline		geom::Point2 &			max()			{return _max;}
	inline	const	geom::Point2 &			max() const		{return _max;}
	inline		PQTNode **			children()		{return _children;}
	inline	const	PQTNode * const *		children() const	{return _children;}
	inline		RadPrimPointerListGrainy &	polys()			{return _polys;}
	inline	const	RadPrimPointerListGrainy &	polys() const		{return _polys;}

	private:
		// Data members

			geom::Point2			_min;
			geom::Point2			_max;
			PQTNode *			_children[4];
			RadPrimPointerListGrainy	_polys;
	};

	typedef	fstl::reservoir<PQTNode, 256>	PQTNodeReservoir;

public:

	// Construction/Destruction

					PolarQuadtree(const geom::Ray3 & polarAxis);
virtual					~PolarQuadtree();

	// Operators

	// Implementation

virtual		geom::Point2		toPolar(const geom::Point3 & point) const;
virtual		void			insert(const RadPrim & prim);
virtual		void			insertPrim(const RadPrim & prim);
virtual		void			gatherOverlappingPolygons(const PQTNode & node, const geom::Point2 & min, const geom::Point2 & max, RadPrimPointerListGrainy & polys);

	// Accessors

inline		geom::Ray3 &		axis()			{return _axis;}	
inline	const	geom::Ray3 &		axis() const		{return _axis;}
inline		geom::Matrix3 &		axisXForm()		{return _axisXForm;}	
inline	const	geom::Matrix3 &		axisXForm() const	{return _axisXForm;}
inline		geom::Plane3Array &	clippingPlanes()	{return _clippingPlanes;}	
inline	const	geom::Plane3Array &	clippingPlanes() const	{return _clippingPlanes;}
inline		PQTNode &		root()			{return _root;}
inline	const	PQTNode &		root() const		{return _root;}
inline		PQTNodeReservoir &	reservoir()		{return _reservoir;}
inline	const	PQTNodeReservoir &	reservoir() const	{return _reservoir;}
inline		RadPrimListGrainy &	visiblePieces()		{return _visiblePieces;}
inline	const	RadPrimListGrainy &	visiblePieces() const	{return _visiblePieces;}


private:
	// Data members

		geom::Ray3		_axis;
		geom::Matrix3		_axisXForm;
		geom::Plane3Array	_clippingPlanes;
		PQTNode			_root;
		PQTNodeReservoir	_reservoir;
		RadPrimListGrainy	_visiblePieces;
};

#endif // _H_POLARQUADTREE
// ---------------------------------------------------------------------------------------------------------------------------------
// PolarQuadtree.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
