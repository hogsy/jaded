// ---------------------------------------------------------------------------------------------------------------------------------
//  _____            _ _____       _               _     
// |  __ \          | |  __ \     (_)             | |    
// | |__) | __ _  __| | |__) |_ __ _ _ __ ___     | |__  
// |  _  / / _` |/ _` |  ___/| '__| | '_ ` _ \    | '_ \ 
// | | \ \| (_| | (_| | |    | |  | | | | | | | _ | | | |
// |_|  \_\\__,_|\__,_|_|    |_|  |_|_| |_| |_|(_)|_| |_|
//
// Description:
//
//   Polygon as derived from the primitive class, specialized for radiosity usage
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   08/11/2001 by Paul Nettle: Original creation
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

#ifndef	_H_RADPRIM
#define _H_RADPRIM

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "geom/geom.h"
#include "RadPatch.h"

// ---------------------------------------------------------------------------------------------------------------------------------

class	RadPrim : public geom::Primitive
{
public:
	// Construction/Destruction

inline					RadPrim() :
						_textureID(0), _usageIndex(0), _uXFormVector(0,0,0), _vXFormVector(0,0,0),
						_illuminationColor(0,0,0), _reflectanceColor(0,0,0), _uPatches(0), _vPatches(0),
						_originalPrimitive(static_cast<RadPrim *>(0)) {}
inline					~RadPrim() {}

	// Implementation

virtual		void			calcTransformVectors();
virtual		void			prepare(const unsigned int patchResolutionU, unsigned int patchResolutionV);
virtual		void			prepareNoPatches();
virtual		bool			mergePatches2x2();
virtual		bool			localBisect(const geom::Plane3 & plane, RadPrim & back);
virtual		bool			bisectNoTextures(const geom::Plane3 & plane, RadPrim & back);

inline		bool			bisect(const geom::Plane3 & plane, RadPrim & back)
					{
						// Call the local bisection routine

						if (!localBisect(plane, back)) return false;

						// Make sure the back maintains our extra members

						if (back.xyz().size())
						{
							back.textureID() = textureID();
							back.usageIndex() = usageIndex();
							back.uXFormVector() = uXFormVector();
							back.vXFormVector() = vXFormVector();
							back.illuminationColor() = illuminationColor();
							back.reflectanceColor() = reflectanceColor();
							back.originalPrimitive() = originalPrimitive();
							back.polyID() = polyID();

							// These are ignored for speed, but also because they're no longer valid after a bisection

							//back.elemntAreas() = elementAreas();
							//back.minXYZ() = minXYZ();
						}

						return true;
					}

inline		void			calcUVExtents(geom::Point2 & minUV, geom::Point2 & maxUV)
					{
						minUV = uv()[0];
						maxUV = uv()[0];
						for (unsigned int i = 1; i < uv().size(); ++i)
						{
							const geom::Point2 &	c = uv()[i];
							if (c.u() < minUV.u()) minUV.u() = c.u();
							if (c.v() < minUV.v()) minUV.v() = c.v();
							if (c.u() > maxUV.u()) maxUV.u() = c.u();
							if (c.v() > maxUV.v()) maxUV.v() = c.v();
						}
					}

inline		void			calcIntegerUVExtents(int & minU, int & maxU, int & minV, int & maxV)
					{
						geom::Point2	minUV, maxUV;
						calcUVExtents(minUV, maxUV);
						minU = static_cast<int>(floor(minUV.u()));
						minV = static_cast<int>(floor(minUV.v()));
						maxU = static_cast<int>(floor(maxUV.u()));
						maxV = static_cast<int>(floor(maxUV.v()));
					}

inline		void			classify(const geom::Plane3 & plane, bool & neg, bool & pos, const float epsilon = 1.0e-4) const
					{
						pos = false;
						neg = false;
						for (unsigned int i = 0; i < xyz().size(); ++i)
						{
							double	t = plane.halfplane(xyz()[i]);
							if (t < -epsilon) neg = true;
							if (t > +epsilon) pos = true;
							if (neg && pos) break;
						}
					}

	// Accessors

inline		unsigned int &		textureID()			{return _textureID;}
inline	const	unsigned int		textureID() const		{return _textureID;}
inline		unsigned int &		usageIndex()			{return _usageIndex;}
inline	const	unsigned int		usageIndex() const		{return _usageIndex;}
inline		geom::Vector3 &		uXFormVector()			{return _uXFormVector;}
inline	const	geom::Vector3 &		uXFormVector() const		{return _uXFormVector;}
inline		geom::Vector3 &		vXFormVector()			{return _vXFormVector;}
inline	const	geom::Vector3 &		vXFormVector() const		{return _vXFormVector;}
inline		geom::Color3 &		illuminationColor()		{return _illuminationColor;}
inline	const	geom::Color3 &		illuminationColor() const	{return _illuminationColor;}
inline		geom::Color3 &		reflectanceColor()		{return _reflectanceColor;}
inline	const	geom::Color3 &		reflectanceColor() const	{return _reflectanceColor;}
inline		fstl::floatArray &	elementAreas()			{return _elementAreas;}
inline	const	fstl::floatArray &	elementAreas() const		{return _elementAreas;}
inline		RadPatchArray &		patches()			{return _patches;}
inline	const	RadPatchArray &		patches() const			{return _patches;}
inline		unsigned int &		uPatches()			{return _uPatches;}
inline	const	unsigned int		uPatches() const		{return _uPatches;}
inline		unsigned int &		vPatches()			{return _vPatches;}
inline	const	unsigned int		vPatches() const		{return _vPatches;}
inline		geom::Point3 &		minXYZ()			{return _minXYZ;}
inline	const	geom::Point3 &		minXYZ() const			{return _minXYZ;}
inline		geom::Point2 &		minUV()				{return _minUV;}
inline	const	geom::Point2 &		minUV() const			{return _minUV;}
inline		geom::Point2 &		maxUV()				{return _maxUV;}
inline	const	geom::Point2 &		maxUV() const			{return _maxUV;}
inline		RadPrim *&		originalPrimitive()		{return _originalPrimitive;}
inline	const	RadPrim *		originalPrimitive() const	{return _originalPrimitive;}
inline 		geom::Point2Array &	texuv()				{return _texuv;}
inline	const	geom::Point2Array &	texuv() const			{return _texuv;}
inline		unsigned int &		polyID()			{return _polyID;}
inline	const	unsigned int		polyID() const			{return _polyID;}

private:
	// Data members

		unsigned int		_textureID;
		unsigned int		_usageIndex;
		geom::Point3		_uXFormVector;
		geom::Point3		_vXFormVector;
		geom::Color3		_illuminationColor;
		geom::Color3		_reflectanceColor;
		fstl::floatArray	_elementAreas;
		RadPatchArray		_patches;
		unsigned int		_uPatches;
		unsigned int		_vPatches;
		geom::Point3		_minXYZ;
		geom::Point2		_minUV;
		geom::Point2		_maxUV;
		RadPrim *		_originalPrimitive;
		geom::Point2Array	_texuv;
		unsigned int		_polyID;
};

typedef	RadPrim *			RadPrimPointer;
typedef	fstl::array<RadPrim>		RadPrimArray;
typedef	fstl::array<RadPrimPointer>	RadPrimPointerArray;
typedef	fstl::list<RadPrim>		RadPrimList;
typedef	fstl::list<RadPrimPointer>	RadPrimPointerList;
typedef	fstl::list<RadPrim, 256>	RadPrimListGrainy;
typedef	fstl::list<RadPrimPointer, 256>	RadPrimPointerListGrainy;

#endif // _H_RADPRIM
// ---------------------------------------------------------------------------------------------------------------------------------
// RadPrim.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
