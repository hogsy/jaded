#ifndef _LCP_MEMORY_H_
#define _LCP_MEMORY_H_

#ifndef GCN
#include <malloc.h> // alloca
#endif
#include <string.h> // memcpy

/* the efficient alignment. most platforms align data structures to some
 * number of bytes, but this is not always the most efficient alignment.
 * for example, many x86 compilers align to 4 bytes, but on a pentium it
 * is important to align doubles to 8 byte boundaries (for speed), and
 * the 4 floats in a SIMD register to 16 byte boundaries. many other
 * platforms have similar behavior. setting a larger alignment can waste
 * a (very) small amount of memory. NOTE: this number must be a power of
 * two. this is set to 16 by default.
 */
#define EFFICIENT_ALIGNMENT 16

// round something up to be a multiple of the EFFICIENT_ALIGNMENT
#define dEFFICIENT_SIZE(x) ((((x)-1)|(EFFICIENT_ALIGNMENT-1))+1)

// alloca aligned to the EFFICIENT_ALIGNMENT. note that this can waste
// up to 15 bytes per allocation, depending on what alloca() returns.
//#define dALLOCA16(n) \
//  ((char*)dEFFICIENT_SIZE(((int)(alloca((n)+(EFFICIENT_ALIGNMENT-1))))))

#if defined( WIN32 ) || defined( _XBOX ) || defined( _XENON )
#	define __dALLOCA16( n ) \
		( ( char * ) dEFFICIENT_SIZE( ( ( int ) ( malloc( ( n ) + ( EFFICIENT_ALIGNMENT - 1 ) ) ) ) ) )
#	define FREE( n ) free( n )
#else
#	define __dALLOCA16( n ) aligned_alloc( EFFICIENT_ALIGNMENT, n )
#	define FREE( n )        free( n )
#endif

#include "BASe/MEMory/MEM.h"

#define __ALLOCA __dALLOCA16


// STATIC MEMORY ALLOCATIONS
#define LCP_MAX_N 	256

#endif