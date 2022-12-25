/*$T VERsion_Update2.cpp GC!1.52 11/06/99 11:22:15 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/VERsion/VERsion_Update.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDIpaths.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGkey.h"




/*
 ===================================================================================================
 Aim: Update bigfile from version 3 to Version 4
 ===================================================================================================
 */
void VERsion_UpdateOneFileVersion4(char *_pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    *pc_dstBuff, *b;
    char    *psz_ext;
    LONG    l_Type, l_Size, i;
    LONG    l_NbPoints, l_NbUVs, l_NbElements, l_RLI;
    LONG    *pl_Element;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    /* Find extension of file */
    psz_ext = strrchr(_psz_File, '.');

    /* Update lights */
    if(psz_ext && (!strcmp(psz_ext, EDI_Csz_ExtGraphicObject) || !strcmp(psz_ext, EDI_Csz_ExtGraphicLight)) )
    {
        l_Type = *(LONG *) _pc_Buf;
        if ( l_Type != 1 )
            return;

        /* We found a graphic object to update */
        b = pc_dstBuff = (char *) L_malloc( _ul_Len );

        /* $1 keep gro structure */
        L_memcpy( b, _pc_Buf, 8 ); /* type and name length */
        _pc_Buf += 8;
        b += 4;
        l_Size = *(LONG *) b;
        b += 4;
        L_memcpy( b, _pc_Buf, l_Size );
        b += l_Size;
        _pc_Buf += l_Size;

        /*$1 geometric object */
        /* keep number of points */
        l_NbPoints = *(LONG *) _pc_Buf;
        L_memcpy( b, _pc_Buf, 4 ); 
        b += 4;
        _pc_Buf += 4;

        /* swap pointers : points, normals  */
        _pc_Buf += 8;

        /* keep pointer on RLI (indicate if object has RLI or not */
        l_RLI = *(LONG *) _pc_Buf;
        L_memcpy( b, _pc_Buf, 4 ); 
        b += 4;
        _pc_Buf += 4;

        /* keep number of UV */
        l_NbUVs = *(LONG *) _pc_Buf;
        L_memcpy( b, _pc_Buf, 4 ); 
        b += 4;
        _pc_Buf += 4;
        
        /* swap pointers : UV */
        _pc_Buf += 4;

        /* keep number of elements */
        l_NbElements = *(LONG *) _pc_Buf;
        L_memcpy( b, _pc_Buf, 4 ); 
        b += 4;
        _pc_Buf += 4;

        /* swap pointers : elements */
        _pc_Buf += 4;

        /* keep draw mask */
        L_memcpy( b, _pc_Buf, 4 ); 
        b += 4;
        _pc_Buf += 4;

        /* swap last 8 bytes : editor mask and bv pointer */
        _pc_Buf += 8;

        /* Add eight dummy bytes */
        L_memset( b, 0, 8 );
        b += 8;

        /* keep list of points, normals, RLI and UV */
        l_Size = l_NbPoints * 12;
        L_memcpy( b, _pc_Buf, l_Size );
        b += l_Size;
        _pc_Buf += l_Size;
        L_memcpy( b, _pc_Buf, l_Size );
        b += l_Size;
        _pc_Buf += l_Size;
        if ( l_RLI )
        {
            l_Size = l_NbPoints * sizeof( LONG );
            L_memcpy( b, _pc_Buf, l_Size );
            b += l_Size;
            _pc_Buf += l_Size;
        }

        l_Size = 8 * l_NbUVs;
        L_memcpy( b, _pc_Buf, l_Size );
        b += l_Size;
        _pc_Buf += l_Size;

        /* treat element */
        pl_Element = (LONG *) b;
        for (i = 0; i < l_NbElements; i++)
        {
            L_memcpy( b, _pc_Buf, 8);
            b += 8;
            _pc_Buf += 12;
        }

        /* keep element data */
        for (i = 0; i < l_NbElements; i++, pl_Element+=2 )
        {
            l_Size = 20 * (*pl_Element);
            L_memcpy( b, _pc_Buf, l_Size );
            b+= l_Size;
            _pc_Buf += l_Size;
        }

		/* Finally, we replace the file with the destination buffer */
        _ul_Len = b - pc_dstBuff;
        BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, pc_dstBuff, _ul_Len);
        L_free(pc_dstBuff);
    }
}

#endif
