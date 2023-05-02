/*$T MAD_mem.h GC!1.32 05/18/99 10:51:41 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C" {
#endif

void    Mad_meminit(void);
void    *Mad_malloc(unsigned long size);
void    *Mad_Realloc(void *address , unsigned long size);
void    Mad_free(void);

#if 1
#define MAD_MALLOC(type, address, size) \
    address = (type *) Mad_malloc(sizeof(type) * (size)); 

#define MAD_REALLOC(type, address, size) \
    address = (type *) Mad_Realloc(address , sizeof(type) * (size)); \

#define MAD_FREE()  Mad_free();
#else // test for tracking down possible memory issue ~hogsy
#	define MAD_MALLOC( type, address, size ) address = ( type * ) malloc( sizeof( type ) * (size) )
#	define MAD_REALLOC( type, address, size ) address = ( type * ) realloc( address, sizeof( type ) * (size ) )
#	define MAD_FREE()                         
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
