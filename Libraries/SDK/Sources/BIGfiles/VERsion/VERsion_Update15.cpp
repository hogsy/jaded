/*$T VERsion_Update14.cpp GC!1.52 01/07/00 16:01:23 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGmdfy_file.h"


/*
 ===================================================================================================
    Aim:    Update geometric objet for Add MRM inside
 ===================================================================================================
 */
void VERsion_UpdateOneFileVersion15(char *_pc_Buf, ULONG _ul_Len, char *_pz_Path, char *_pz_File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    *pc_dstBuff, *b;
    char    *psz_ext;
    LONG    l_Type, l_Size;
    LONG    l_NbPoints, l_NbUVs, l_NbElements, l_RLI,Local;
	LONG	*AllTriangleNum,*AllMaterialID;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    /* Find extension of file */
    psz_ext = strrchr(_pz_File, '.');

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

		/* Get structure content */
		l_NbPoints = *(LONG *) _pc_Buf;
		L_memcpy( b, _pc_Buf, 4 ); 
		_pc_Buf += 4;
		b += 4;
		l_RLI = *(LONG *) _pc_Buf;
		L_memcpy( b, _pc_Buf, 4 ); 
		_pc_Buf += 4;
		b += 4;
		l_NbUVs = *(LONG *) _pc_Buf;
		L_memcpy( b, _pc_Buf, 4 ); 
		_pc_Buf += 4;
		b += 4;

		l_NbElements = *(LONG *) _pc_Buf;
		L_memcpy( b, _pc_Buf, 4 ); 
		_pc_Buf += 4;
		b += 4;

        /* keep draw mask */
        L_memcpy( b, _pc_Buf, 4 ); 
        _pc_Buf += 4;
        b += 4;

		/* Swap dummy data */
		L_memcpy( b, _pc_Buf, 8 ); 
		_pc_Buf += 8;
		b += 8;

		/* realloc new size */
		l_Size = b - pc_dstBuff;
		b = pc_dstBuff;
		pc_dstBuff = (char *) L_malloc( _ul_Len + l_NbElements * 4L * 8L);
		L_memset(pc_dstBuff , 0 , _ul_Len + l_NbElements * 4L * 8L);
		L_memcpy(pc_dstBuff , b , l_Size);
		L_free(b);
		b = pc_dstBuff + l_Size;

		/* Get list of points and normals */
		l_Size = 12L /*sizeof(MATH_tdst_Vector)*/ * l_NbPoints;
		L_memcpy(b , _pc_Buf, l_Size);
		_pc_Buf += l_Size;
		b += l_Size;
		L_memcpy(b , _pc_Buf, l_Size);
		_pc_Buf += l_Size;
		b += l_Size;

		/* Get list of vertex colors */
		if(l_RLI)
		{
			if((l_RLI) & 0xFFF00000)
				l_RLI = l_NbPoints;
			else
			{
				if
					(
					( l_RLI != l_NbPoints) &&
					( l_RLI != l_NbPoints * 2)
					) l_RLI = 0;
			}
			if (l_RLI)
			{
				l_Size = 4L * l_RLI;
				L_memcpy(b, _pc_Buf, l_Size);
				_pc_Buf += l_Size;
				b += l_Size;
			}
		}

		/* Get list of UVs */
		L_memcpy(b , _pc_Buf, 8L /*sizeof(GEO_tdst_UV)*/ * l_NbUVs);
		_pc_Buf += 8L /*sizeof(GEO_tdst_UV)*/ * l_NbUVs;
		b += 8L /*sizeof(GEO_tdst_UV)*/ * l_NbUVs;

		/* Read all element data */
		AllTriangleNum = (LONG *)malloc(l_NbElements << 2);
		AllMaterialID  = (LONG *)malloc(l_NbElements << 2);
		Local = l_NbElements;
		while (Local--)
		{
			AllTriangleNum[Local] = *(LONG *) _pc_Buf;
			L_memcpy(b , _pc_Buf, 4);
			_pc_Buf += 4;
			b += 4;
			AllMaterialID [Local] = *(LONG *) _pc_Buf;
			L_memcpy(b , _pc_Buf, 4);
			_pc_Buf += 4;
			b += 4;
			b += 4L * 8L;
		}

		/* Read all element content */
		Local = l_NbElements;
		while (Local--)
		{
			/* Read triangles */
			l_Size = 20L /*sizeof(GEO_tdst_IndexedTriangle)*/ * AllTriangleNum[Local];
			L_memcpy(b, _pc_Buf, l_Size);
			_pc_Buf += l_Size;
			b += l_Size;
		}

		/* Finally, we replace the file with the destination buffer */
        _ul_Len = b - pc_dstBuff;
        BIG_UpdateFileFromBuffer(_pz_Path, _pz_File, pc_dstBuff, _ul_Len);
        L_free(pc_dstBuff);
		free(AllTriangleNum);
		free(AllMaterialID);
    }
}

#endif /* ACTIVE_EDITORS */
