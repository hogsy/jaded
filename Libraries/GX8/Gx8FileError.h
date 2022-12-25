#if !defined( GX8_FILEERROR_H )
#define GX8_FILEERROR_H 

#if !defined( __cplusplus )
#  include <stdbool.h>
#endif

#if defined( __cplusplus )
extern "C"
{
#endif

#include "xtl.h"

void Gx8_FileError( void );
void Gx8_FileErrorSound(void);
void Gx8_FileError_Init( void );
void  Gx8_FileError_CheckSound( void );


#if defined( __cplusplus )
}
#endif

#endif