// Updated by Mark "hogsy" Sowden, 2023 <hogsy@oldtimes-software.com>
// https://oldtimes-software.com/jaded/
// Purpose: Handles bit flag vector for collisions

#include "Precomp.h"
#include "INTConst.h"
#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "INTstruct.h"

#include <assert.h>

SnPBitVec::SnPBitVec( size_t maxobjs ) : maxobjs( maxobjs )
{
	vec.resize( maxobjs * maxobjs );
}

bool SnPBitVec::Get( size_t obj_a, size_t obj_b ) const
{
	if ( obj_a == INT_Cul_InvalidObject || obj_b == INT_Cul_InvalidObject )
	{
		return false;
	}

	assert( obj_a < maxobjs );
	assert( obj_b < maxobjs );

	int l_XMaxRef = lMax( obj_a, obj_b );
	int l_XMinRef = lMin( obj_a, obj_b );

	obj_a = l_XMaxRef;
	obj_b = l_XMinRef;

	return vec[ ( obj_a * maxobjs ) + obj_b ];
}

bool SnPBitVec::Set( size_t obj_a, size_t obj_b, bool value )
{
	assert( obj_a < maxobjs );
	assert( obj_b < maxobjs );

	int l_XMaxRef = lMax( obj_a, obj_b );
	int l_XMinRef = lMin( obj_a, obj_b );

	obj_a = l_XMaxRef;
	obj_b = l_XMinRef;

	bool was_set                       = vec[ ( obj_a * maxobjs ) + obj_b ];
	vec[ ( obj_a * maxobjs ) + obj_b ] = value;
	return was_set;
}

void SnPBitVec::Resize( size_t maxobjs )
{
	assert( maxobjs > this->maxobjs );

	vec.resize( maxobjs * maxobjs );

	/* Starting from the end of vec, we walk backwards, moving the values from their old positions
	 * to their new ones. We have to go in this order to ensure the old (lower) values aren't
	 * overwritten until they have been copied to their new (higher) positions.
	 */
	for ( size_t yi = 0; yi < this->maxobjs; ++yi )
	{
		size_t y = this->maxobjs - yi - 1;

		for ( size_t xi = 0; xi < this->maxobjs; ++xi )
		{
			size_t x = this->maxobjs - xi - 1;

			vec[ ( y * maxobjs ) + x ]       = vec[ ( y * this->maxobjs ) + x ];
			vec[ ( y * this->maxobjs ) + x ] = false;
		}
	}

	this->maxobjs = maxobjs;
}

void SnPBitVec::Clear()
{
	vec.clear();
	vec.resize( maxobjs * maxobjs );
}
