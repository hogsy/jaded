#if !defined( GX8_HANDLE_NO_PAD_H )
#define GX8_HANDLE_NO_PAD_H 

#if !defined( __cplusplus )
#  include <stdbool.h>
#endif

#if defined( __cplusplus )
extern "C"
{
#endif
    
typedef enum
{
    GX8_NO_PAD_NULL,
    GX8_NO_PAD_RECONNECT,
    GX8_NO_PAD_PRESS_START
}
Gx8NoPadWriting;

void
Gx8WriteNoPadString( Gx8NoPadWriting sentence );

void
Gx8_HandleNoPad_setMessage( Gx8NoPadWriting sentence );

bool
Gx8_HandleNoPad_isMessageToDisplay(void);

void
Gx8_HandleNoPad( void );

void
Gx8_HandleNoPadPresent( void );

#if defined( __cplusplus )
}
#endif

#endif
