// ---------------------------------------------------------------------------------------------------------------------------------
//  _____            _ _      __  __                 _     
// |  __ \          | | |    |  \/  |               | |    
// | |__) | __ _  __| | |    | \  / | __ _ _ __     | |__  
// |  _  / / _` |/ _` | |    | |\/| |/ _` | '_ \    | '_ \ 
// | | \ \| (_| | (_| | |____| |  | | (_| | |_) | _ | | | |
// |_|  \_\\__,_|\__,_|______|_|  |_|\__,_| .__/ (_)|_| |_|
//                                        | |              
//                                        |_|              
//
// Description:
//
//   Lightmap used for tracking light from the radiosity process
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   08/17/2001 by Paul Nettle: Original creation
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

#ifndef	_H_RADLMAP
#define _H_RADLMAP

#include "geom/geom.h"

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------------------

class	RadLMap
{
public:
	// Construction/Destruction

					RadLMap(unsigned int w, unsigned int h, int id = -1);
virtual					~RadLMap();

	// Implementation

virtual		void			init(unsigned int w, unsigned int h);
virtual		void			applyGamma(const float gamma = 2.2f);
virtual		void			clampSaturate();
virtual		void			clampRetainColorRatio();
virtual		void			addAmbient(const geom::Color3 & ambient);
virtual		void			writeRaw(const fstl::string & folder);

	// Accessors

inline		unsigned int &		width()		{return _width;}
inline	const	unsigned int		width() const	{return _width;}
inline		unsigned int &		height()	{return _height;}
inline	const	unsigned int		height() const	{return _height;}
inline		int &			id()		{return _id;}
inline	const	int			id() const	{return _id;}
inline		bool &			updated()	{return _updated;}
inline	const	bool			updated() const	{return _updated;}
inline		geom::Color3Array &	data()		{return _data;}
inline	const	geom::Color3Array &	data() const	{return _data;}

private:
	// Data members

		unsigned int		_width;
		unsigned int		_height;
		int			_id;
		bool			_updated;
		geom::Color3Array	_data;
};

typedef	fstl::array<RadLMap>	RadLMapArray;
typedef	fstl::array<RadLMap *>	RadLMapPointerArray;

#endif // _H_RADLMAP
// ---------------------------------------------------------------------------------------------------------------------------------
// RadLMap.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
