/*$T GFXstring.h GC! 1.081 04/10/01 10:26:23 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFXSTRING_H__
#define __GFXSTRING_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	GFX_tdst_Gao_
{
    struct OBJ_tdst_GameObject_  *pst_GO;
    float                        f_Focale;
} GFX_tdst_Gao;

typedef struct	GFX_tdst_String_
{
    int                             i_NbLetter;
    ULONG                           ul_Color1;
    ULONG                           ul_Color2;

    float                           f_Time;

    struct OBJ_tdst_GameObject_     *pst_GO;
    GEO_Vertex                      *pst_Vertex;
    GEO_tdst_UV                     *pst_UV;
} GFX_tdst_String;


/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void	*GFX_Gao_Create( struct OBJ_tdst_GameObject_ *);
void	GFX_Gao_Render(void *);
void    GFX_Gao_Destroy( void * );

void	*GFX_Str_Create( struct OBJ_tdst_GameObject_ *);
int     GFX_i_Str_Render(void *);
void    GFX_Str_Destroy( void * );
void    GFX_Str_Seti( void *, int , int );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFXSTRING_H__ */
