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
 Aim: Update bigfile from version 2 to Version 3
 ===================================================================================================
 */
void VERsion_UpdateOneFileVersion3(char *_pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    *pc_dstBuff, *b;
    char    *psz_ext;
    float   f;

    static LONG l_SizeOfPFTable = 28;
    static LONG l_SizeOfPTable = 28;
    static LONG l_SizeOfSetOfEOT = 280;
    static LONG l_ActivatorSize = 89;
    static LONG l_ViewSize = 170;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    /* Find extension of file */
    psz_ext = strrchr(_psz_File, '.');

    /* Update lights */
    if(psz_ext && !strcmp(psz_ext, EDI_Csz_ExtWorld))
    {
        /* We found a world to update */
        b = pc_dstBuff = (char *) L_malloc( _ul_Len );

        /*$1 keep 8 first octet : signature and old version number = 4 */
        L_memcpy( b, _pc_Buf, 8 );
        b += 8;
        _pc_Buf += 8;

        /*$1 old version : all world structure was written , now keep only pertinent information */
        /* keep number of game objects */
        L_memcpy( b, _pc_Buf, 4 );
        b += 4;
        _pc_Buf += 4;

        /* swap rebuild information */
        _pc_Buf += 8 + l_SizeOfPFTable + 2 * l_SizeOfPTable;
        _pc_Buf += 12 + l_SizeOfPFTable;
        _pc_Buf += 6 * 4 + l_SizeOfSetOfEOT;

        /* keep ambiant color */
        L_memcpy( b, _pc_Buf, 4 );
        b += 4;
        _pc_Buf += 4;

        /* swap rebuild information */
        _pc_Buf += 5;

        /* keep name, but reduce it size from 256 to 64 */
        L_memcpy( b, _pc_Buf, 64 );
        b[63] = 0;
        b += 64; 
        _pc_Buf += 256;
        _pc_Buf += 3; /* for 4 bytes alignment */

        /* swap rebuild information */
        _pc_Buf += 4 + 4;
        
        /* get last camera position : in old version w of matrix was camera focale, deplace it after matrix */
        L_memcpy( b, _pc_Buf, 4 * 15 );
        b += 4 * 15;
        _pc_Buf += 4 * 15;
        f = 1.0f;
        *(float *) b = f;
        b += 4;
        f = *(float *) _pc_Buf;
        _pc_Buf += 4;
        L_memcpy( b, _pc_Buf, 4 );
        b += 4;
        _pc_Buf += 4;
        L_memcpy( b, (char *) &f, 4 );
        b += 4;

        /*$1 add 8 LONG for eventual additionnal data */
        L_memset( b, 0xFF, 64 );
        b+= 64;

        /*$1 swap view and activator */
        _pc_Buf += l_ActivatorSize + l_ViewSize;

        /*$1 we ended with world structure add additional info */
        /* keep reference on game object group and on game object group group */
        L_memcpy( b, _pc_Buf, 12 );
        b += 8;
        _pc_Buf += 12;
        L_memset( b, 0xFF, 4 );
        b+= 4;

		/* Finally, we replace the file with the destination buffer */
        _ul_Len = b - pc_dstBuff;
        BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, pc_dstBuff, _ul_Len);
        L_free(pc_dstBuff);
    }
}

#endif
