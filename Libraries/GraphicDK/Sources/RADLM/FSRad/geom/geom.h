// ---------------------------------------------------------------------------------------------------------------------------------
//   __ _  ___  ___  _ __ ___
//  / _` |/ _ \/ _ \| '_ ` _ \
// | (_| |  __/ (_) | | | | | |
//  \__, |\___|\___/|_| |_| |_|
//   __/ |
//  |___/
//
// Description:
//
//   Primary include file for GEOM
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   05/05/2001 by Paul Nettle: Original creation
//
// Restrictions & freedoms pertaining to usage and redistribution of this software:
//
//   This software is 100% free. If you use this software (in part or in whole) you must credit the author. This software may not be
//   re-distributed (in part or in whole) in a modified form without clear documentation on how to obtain a copy of the original
//   work. You may not use this software to directly or indirectly cause harm to others. This software is provided as-is and without
//   warrantee -- Use at your own risk. For more information, visit HTTP://www.FluidStudios.com/
//
// Copyright 2001, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_GEOM_GEOM
#define _GEOM_GEOM

// ---------------------------------------------------------------------------------------------------------------------------------
// Precision - do we use floats or doubles?
// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef	GEOM_PRECISION_DOUBLE
	#define	GEOM_TYPE	double
#else
	#define	GEOM_TYPE	float
#endif

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "common.h"
#include "vmath.h"
#include "3dutils.h"
#include "rayplaneline.h"
#include "primitive.h"

#endif // _GEOM_GEOM
// ---------------------------------------------------------------------------------------------------------------------------------
// geom - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
