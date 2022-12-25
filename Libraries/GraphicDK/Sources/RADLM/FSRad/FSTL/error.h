// ---------------------------------------------------------------------------------------------------------------------------------
//   ___ _ __ _ __  ___  _ __
//  / _ \ '__| '__|/ _ \| '__|
// |  __/ |  | |  | (_) | |
//  \___|_|  |_|   \___/|_|
//
// Description:
//
//   Cascading error management
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   04/22/2001 by Paul Nettle: Original creation
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

#ifndef	_FSTL_ERROR
#define _FSTL_ERROR

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "common.h"
#include "string.h"
#include "array.h"

FSTL_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------------------------------------------------------------

template <class T = string>
class	basic_error
{
public:
	// Construction/Destruction

inline				basic_error() {}

inline				basic_error(const T & location, const T & instruction, const T & message)
				{
					locations() += location;
					instructions() += instruction;
					messages() += message;
				}

inline				basic_error(const basic_error & rhs)
				{
					*this = rhs;
				}

inline				~basic_error() {}

	// Operators
	
inline		basic_error &	operator=(const basic_error & rhs)
				{
					if (this == &rhs) return *this;
					locations() = rhs.locations();
					instructions() = rhs.instructions();
					messages() = rhs.messages();
					return *this;
				}

inline		basic_error	operator+(const T & rhs) const
				{
					basic_error	result(*this);
					result.locations() += rhs.findWord(0);
					result.instructions() += rhs.findWord(1);
					result.messages() += rhs.findWord(2);
					return result;
				}

inline		basic_error &	operator+=(const T & rhs)
				{
					locations() += rhs.findWord(0);
					instructions() += rhs.findWord(1);
					messages() += rhs.findWord(2);
					return *this;
				}

inline		basic_error	operator+(const basic_error & rhs) const
				{
					basic_error	result(*this);
					result.locations() += rhs.locations();
					result.instructions() += rhs.locations();
					result.messages() += rhs.messages();
					return result;
				}

inline		basic_error &	operator+=(const basic_error & rhs)
				{
					locations() += rhs.locations();
					instructions() += rhs.locations();
					messages() += rhs.messages();
					return *this;
				}

	// Implementation

	// Accessors
	
inline		array<T> &	locations()          {return _locations;}
inline	const	array<T> &	locations() const    {return _locations;}
inline		array<T> &	instructions()       {return _instructions;}
inline	const	array<T> &	instructions() const {return _instructions;}
inline		array<T> &	messages()           {return _messages;}
inline	const	array<T> &	messages() const     {return _messages;}

private:
	// Data members
	
		array<T>	_locations;
		array<T>	_instructions;
		array<T>	_messages;
};

typedef	basic_error<string>	error;
typedef	basic_error<wstring>	werror;

FSTL_NAMESPACE_END
#endif // _FSTL_ERROR
// ---------------------------------------------------------------------------------------------------------------------------------
// error - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

