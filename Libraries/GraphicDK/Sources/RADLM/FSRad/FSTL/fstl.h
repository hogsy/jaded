// ---------------------------------------------------------------------------------------------------------------------------------
//   __     _   _
//  / _|   | | | |
// | |_ ___| |_| |
// |  _/ __| __| |
// | | \__ \ |_| |
// |_| |___/\__|_|
//
// Description:
//
//   Use this file to manage proper inclusion order for all fstl files
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   06/04/2001 by Paul Nettle: Original creation
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
//
// MSVC Handy tip! Edit your autoexp.dat file (for MSVC) and add this to the end, then restart MSVC
//
//  ; From FSTL
//  fstl::string=<_buffer,s>
//  fstl::error=<_messages._buf[0]._buffer,s>
//  fstl::array<*>=size=<_size,u>
//  fstl::list<*>=size=<_size,u>
//  fstl::hash<*>=size=<_size,u>
//
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_FSTL_FSTL
#define _FSTL_FSTL

// ---------------------------------------------------------------------------------------------------------------------------------

#include "common.h"
#include "string.h"
#include "util.h"
#include "error.h"
#include "array.h"
#include "list.h"
#include "reservoir.h"
#include "hash.h"

#endif // _FSTL_FSTL
// ---------------------------------------------------------------------------------------------------------------------------------
// fstl - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

