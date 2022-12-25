// ---------------------------------------------------------------------------------------------------------------------------------
//  _      __  __              _____                _     
// | |    |  \/  |            / ____|              | |    
// | |    | \  / | __ _ _ __ | |  __  ___ _ __     | |__  
// | |    | |\/| |/ _` | '_ \| | |_ |/ _ \ '_ \    | '_ \ 
// | |____| |  | | (_| | |_) | |__| |  __/ | | | _ | | | |
// |______|_|  |_|\__,_| .__/ \_____|\___|_| |_|(_)|_| |_|
//                     | |                                
//                     |_|                                
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

#ifndef	_H_LMAPGEN
#define _H_LMAPGEN

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "geom/geom.h"
#include "RadPrim.h"

// ---------------------------------------------------------------------------------------------------------------------------------

class	LMapGen
{
private:
	// Local-scope class -------------------------------------------------------------------------------------------------------

	class	Rect
	{
	public:
			// Types (for convenience)

		typedef	fstl::array<Rect>	RectArray;
		typedef	fstl::array<Rect *>	RectPointerArray;
		typedef	fstl::list<Rect>	RectList;
		typedef	fstl::list<Rect *>	RectPointerList;

			// Construction/Destruction

	inline				Rect() : _minX(0), _maxX(0), _minY(0), _maxY(0) {}
	inline				Rect(const int x0, const int y0, const int x1, const int y1) : _minX(x0), _maxX(x1), _minY(y0), _maxY(y1) {}
	virtual				~Rect() {}

			// Operators

	inline		bool		operator <  (const Rect & rhs) const	{return area() <  rhs.area();}
	inline		bool		operator <= (const Rect & rhs) const	{return area() <= rhs.area();}
	inline		bool		operator >  (const Rect & rhs) const	{return area() >  rhs.area();}
	inline		bool		operator >= (const Rect & rhs) const	{return area() >= rhs.area();}
	inline		bool		operator == (const Rect & rhs) const	{return area() == rhs.area();}
	inline		bool		operator != (const Rect & rhs) const	{return area() != rhs.area();}

			// Implementation

	inline		bool		canMergeWith(const Rect & r) const
					{
						// Share a vertical edge?

						if ((minX() == r.maxX() || maxX() == r.minX()) && minY() == r.minY() && maxY() == r.maxY()) return true;

						// Share a horizontal edge?

						if ((minY() == r.maxY() || maxY() == r.minY()) && minX() == r.minX() && maxX() == r.maxX()) return true;

						// Can't be merged

						return false;
					}

	inline		bool		overlapTest(const Rect & test) const
					{
						if (minX() >= test.maxX()) return false;
						if (maxX() <= test.minX()) return false;
						if (minY() >= test.maxY()) return false;
						if (maxY() <= test.minY()) return false;
						return true;
					}

	inline		void		boundingRect(const Rect & a, const Rect & b)
					{
						*this = a;

						if (b.minX() < minX()) minX() = b.minX();
						if (b.maxX() > maxX()) maxX() = b.maxX();
						if (b.minY() < minY()) minY() = b.minY();
						if (b.maxY() > maxY()) maxY() = b.maxY();
					}

	inline		bool		clipTo(const Rect & dst)
					{
						if (!overlapTest(dst)) return false;

						if (minX() < dst.minX()) minX() = dst.minX();
						if (maxX() > dst.maxX()) maxX() = dst.maxX();
						if (minY() < dst.minY()) minY() = dst.minY();
						if (maxY() > dst.maxY()) maxY() = dst.maxY();

						return true;
					}

	inline		RectArray	booleanSubtract(const Rect & operandB, bool & emptyResult)
					{
						// Our result

						RectArray	result;
						emptyResult = false;

						// The two operands

						Rect		opA = *this;
						Rect		opB = operandB;

						// For convenience, clip B to A (return blank result if they don't overlap)

						if (!opB.clipTo(opA)) return result;

						// If operand B completely covers A, then the result will be empty

						if (opB == opA)
						{
							emptyResult = true;
							return result;
						}

						// Do the boolean

						for (;;)
						{
							// Four pieces, depending on how it might get split up

							Rect	lRect = opA;	lRect.maxX() = opB.minX();
							Rect	tRect = opA;	tRect.maxY() = opB.minY();
							Rect	rRect = opA;	rRect.minX() = opB.maxX();
							Rect	bRect = opA;	bRect.minY() = opB.maxY();

							// Areas of the four rects

							unsigned int	lArea = lRect.area();
							unsigned int	tArea = tRect.area();
							unsigned int	rArea = rRect.area();
							unsigned int	bArea = bRect.area();

							// If nothing left, we're done

							if (!lArea && !tArea && !rArea && !bArea) return result;

							// Slice off the largest area

							else if (lArea >= tArea && lArea >= rArea && lArea >= bArea)
							{
								result += lRect;
								opA.minX() = lRect.maxX();
							}
							else if (tArea >= lArea && tArea >= rArea && tArea >= bArea)
							{
								result += tRect;
								opA.minY() = tRect.maxY();
							}
							else if (rArea >= lArea && rArea >= tArea && rArea >= bArea)
							{
								result += rRect;
								opA.maxX() = rRect.minX();
							}
							else if (bArea >= lArea && bArea >= tArea && bArea >= rArea)
							{
								result += bRect;
								opA.maxY() = bRect.minY();
							}
						}
					}

			// Accessors

	inline		int &		minX()		{return _minX;}
	inline	const	int		minX() const	{return _minX;}
	inline		int &		maxX()		{return _maxX;}
	inline	const	int		maxX() const	{return _maxX;}
	inline		int &		minY()		{return _minY;}
	inline	const	int		minY() const	{return _minY;}
	inline		int &		maxY()		{return _maxY;}
	inline	const	int		maxY() const	{return _maxY;}

	inline	const	unsigned int	width() const	{return maxX() - minX();}
	inline	const	unsigned int	height() const	{return maxY() - minY();}
	inline	const	unsigned int	area() const	{return width() * height();}


	private:
			// Data members

			int		_minX;
			int		_maxX;
			int		_minY;
			int		_maxY;
	};

	// Local-scope class -------------------------------------------------------------------------------------------------------

	class	CombinedPoly
	{
	public:
			// Construction/Destruction

	inline					CombinedPoly() : _offsetU(0), _offsetV(0), _lightmapID(0), _rotated(false), _complete(false) {}
	virtual					~CombinedPoly() {}

			// Implementation

	inline		void			rotate()
						{
							fstl::swap(minU(), minV());
							fstl::swap(maxU(), maxV());
							rotated() = !rotated();
						}

	inline		void			setOrientation(const bool primarilyHorizontal)
						{
							if (primarilyHorizontal)
							{
								if (width() >= height()) return;
							}
							else
							{
								if (height() >= width()) return;
							}

							rotate();
						}

	inline		void			mapWorldTexture(const float uScale, const float vScale)
						{
							for (unsigned int i = 0; i < primitives().size(); ++i)
							{
								RadPrim &	prim = *primitives()[i];
								prim.setWorldTexture(uScale, vScale);
								prim.calcTransformVectors();
							}

							calcExtents();
						}

	inline		void			calcExtents()
						{
							for (unsigned int i = 0; i < primitives().size(); ++i)
							{
								RadPrim &	prim = *primitives()[i];

								int	mnU, mxU, mnV, mxV;
								prim.calcIntegerUVExtents(mnU, mxU, mnV, mxV);

								if (!i || mnU < minU()) minU() = mnU;
								if (!i || mxU > maxU()) maxU() = mxU;
								if (!i || mnV < minV()) minV() = mnV;
								if (!i || mxV > maxV()) maxV() = mxV;
							}

							rotated() = false;
						}

	inline		void			remap()
						{
							for (unsigned int i = 0; i < primitives().size(); ++i)
							{
								RadPrim &	prim = *primitives()[i];

								prim.textureID() = lightmapID();

								for (unsigned int j = 0; j < prim.uv().size(); ++j)
								{
									// Rotate?

									if (rotated()) fstl::swap(prim.uv()[j].u(), prim.uv()[j].v());

									// Offset

									prim.uv()[j].u() -= minU();
									prim.uv()[j].v() -= minV();
									prim.uv()[j].u() += offsetU() + LMapGen::borderPixels();
									prim.uv()[j].v() += offsetV() + LMapGen::borderPixels();
								}

								prim.calcTransformVectors();
							}
						}

			// Accessors

	inline		    RadPrimPointerArray &	primitives()			{return _primitives;}
	inline	const	RadPrimPointerArray &	primitives() const		{return _primitives;}

	inline		int &			    minU()				{return _minU;}
	inline	const	int			    minU() const		{return _minU;}
	inline		int &			    maxU()				{return _maxU;}
	inline	const	int			    maxU() const		{return _maxU;}
	inline		int &			    minV()				{return _minV;}
	inline	const	int			    minV() const		{return _minV;}
	inline		int &			    maxV()				{return _maxV;}
	inline	const	int			    maxV() const		{return _maxV;}
	inline		int &			    offsetU()			{return _offsetU;}
	inline	const	int			    offsetU() const		{return _offsetU;}
	inline		int &			    offsetV()			{return _offsetV;}
	inline	const	int			    offsetV() const		{return _offsetV;}
	inline		unsigned int &		lightmapID()		{return _lightmapID;}

	inline	const	unsigned int	lightmapID()    const	{return _lightmapID;}
	inline		bool &			    rotated()			    {return _rotated;}
	inline	const	bool			rotated()       const	{return _rotated;}
	inline		bool &			    complete()			    {return _complete;}
	inline	const	bool			complete()      const	{return _complete;}

	inline	const	unsigned int	width() const			{return maxU() - minU() + 1;}
	inline	const	unsigned int	height() const			{return maxV() - minV() + 1;}
	inline	const	unsigned int	area() const			{return width() * height();}

	inline	const	unsigned int	widthIncludingBorder() const	{return width() + LMapGen::borderPixels()*2;}
	inline	const	unsigned int	heightIncludingBorder() const	{return height() + LMapGen::borderPixels()*2;}
	inline	const	unsigned int	areaIncludingBorder() const	{return widthIncludingBorder() * heightIncludingBorder();}

	private:
			// Data members

			RadPrimPointerArray		_primitives;
			int				        _minU;
			int				        _minV;
			int				        _maxU;
			int				        _maxV;
			int				        _offsetU;
			int				        _offsetV;
			unsigned int			_lightmapID;
			bool				    _rotated;
			bool				    _complete;
	};

	typedef	fstl::array<CombinedPoly>	CombinedPolyArray;
	typedef	fstl::array<CombinedPoly *>	CombinedPolyPointerArray;
	typedef	fstl::list<CombinedPoly>	CombinedPolyList;
	typedef	fstl::list<CombinedPoly *>	CombinedPolyPointerList;

public:

	// Construction/Destruction

				LMapGen();
virtual			~LMapGen();

	// Operators

	// Implementation

virtual		bool		    generate(RadPrimList & polygons, RadLMapArray & lightmaps);
virtual		bool		    buildCombinedPolygons(const RadPrimList & polygons, CombinedPolyList & cpl) const;
virtual		bool		    clipCombinedPolygons(CombinedPolyList & cpl, RadPrimList & polygons, const unsigned int limitU, const unsigned int limitV) const;
virtual		bool		    populateLightmaps(CombinedPolyList & cpl, RadLMapArray & lightmaps, const unsigned int limitU, const unsigned int limitV) const;
virtual		unsigned int	populateLightmap(CombinedPolyList & cpl, const unsigned int id, Rect rect) const;

	// Accessors

static		    unsigned int	borderPixels()		    {return _borderPixels;}
inline		    unsigned int &	lightmapWidth()		    {return _lightmapWidth;}
inline	const	unsigned int	lightmapWidth()  const	{return _lightmapWidth;}
inline		    unsigned int &	lightmapHeight()	    {return _lightmapHeight;}
inline	const	unsigned int	lightmapHeight() const	{return _lightmapHeight;}
inline		    float &		    uTexelsPerUnit()	    {return _uTexelsPerUnit;}
inline	const	float		    uTexelsPerUnit() const	{return _uTexelsPerUnit;}
inline		    float &		    vTexelsPerUnit()	    {return _vTexelsPerUnit;}
inline	const	float		    vTexelsPerUnit() const	{return _vTexelsPerUnit;}

private:

		// Data members

        static const unsigned int	_borderPixels;
		unsigned int	            _lightmapWidth;
		unsigned int	            _lightmapHeight;
		float		                _uTexelsPerUnit;
		float		                _vTexelsPerUnit;

};

#endif // _H_LMAPGEN
// ---------------------------------------------------------------------------------------------------------------------------------
// LMapGen.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
