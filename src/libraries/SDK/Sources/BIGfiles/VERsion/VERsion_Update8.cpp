/*$T VERsion_Update8.cpp GC!1.5 12/01/99 16:36:17 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

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

#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLconst.h"

/*
 ===================================================================================================
    Aim:    Update bigfile from version 1 to Version 2
 ===================================================================================================
 */
void VERsion_UpdateOneFileVersion8(char *_pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Destination buffer */
    char    *pc_dstBuff, *b;

    /* Extension of file */
    char    *psz_ext;
    UCHAR   uc_NbOfZDx, uc_NbOfSpecific, uc_Index, uc_Id, uc_Type;
    ULONG   ul_Length;
	char	Name[7];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_strcpy(Name, "NoName");
    /* Find extension of file */
    psz_ext = strrchr(_psz_File, '.');

    /* Update COL Models */
    if(psz_ext && !strcmp(psz_ext, EDI_Csz_ExtCOLSetModel))
    {
        /* The first thing saved in the ColSet in the number of ZDx. */
        uc_NbOfZDx = *_pc_Buf;

        /* For the ColSet, we add 16 extra bytes for the IA-ENG translation array */

        /*$F  For each ZDx, we alloc 12 extra bytes 
			    - 4 for length of the name
				- 7 for the name "NoName" + \0
				- 1 for the ID of the Zone
		*/
        b = pc_dstBuff = (char *) L_malloc(_ul_Len + 16 + 12 * uc_NbOfZDx);

        /* We copy uc_NbOfZDx, uc_Flag, uw_NbOfInstances; */
        L_memcpy(b, _pc_Buf, 4);
        b += 4;
        _pc_Buf += 4;

        /* There, we add the IA-ENG translation array. */
        L_memset(b, 0, 16);
        b += 16;

        for(uc_Index = 0; uc_Index < uc_NbOfZDx; uc_Index++)
        {
			uc_Type = *(UCHAR *)((ULONG)_pc_Buf + 1);
            /* We copy uc_Flag, uc_Type, uw_BoneIndex. */
            L_memcpy(b, _pc_Buf, 4);
            b += 4;
            _pc_Buf += 4;

            /* There, we add the Length, the name and the ID of the Zone. */
            ul_Length = 7;
            L_memcpy(b, &ul_Length, 4);
            b += 4;
            L_memcpy(b, Name, 7);
            b += 7;
            uc_Id = 0;
            L_memcpy(b, &uc_Id, 1);
            b += 1;

			// Force the type to be a sphere.
			uc_Type = COL_C_Zone_Sphere;
            switch(uc_Type)
            {
            case COL_C_Zone_Sphere:

                /* We copy the sphere. */
                L_memcpy(b, _pc_Buf, sizeof(MATH_tdst_Vector) + sizeof(float));
                b += sizeof(MATH_tdst_Vector) + sizeof(float);
                _pc_Buf += sizeof(MATH_tdst_Vector) + sizeof(float);
                break;

            case COL_C_Zone_Box:

                /* We copy the Box. */
                L_memcpy(b, _pc_Buf, 2 * sizeof(MATH_tdst_Vector));
                b += 2 * sizeof(MATH_tdst_Vector);
                _pc_Buf += 2 * sizeof(MATH_tdst_Vector);
                break;

            case COL_C_Zone_Cylinder:

                /* We copy the sphere. */
                L_memcpy(b, _pc_Buf, sizeof(MATH_tdst_Vector) + 2 * sizeof(float));
                b += sizeof(MATH_tdst_Vector) + 2 * sizeof(float);
                _pc_Buf += sizeof(MATH_tdst_Vector) + 2 * sizeof(float);
                break;
            }
        }

        /* Finally, we replace the file with the destination buffer */
        BIG_UpdateFileFromBuffer
        (
            _psz_Path,
            _psz_File,
            pc_dstBuff,
            _ul_Len + 16 + 12 * uc_NbOfZDx
        );
        L_free(pc_dstBuff);
    }

    /* Update COL Instance */
    if(psz_ext && !strcmp(psz_ext, EDI_Csz_ExtCOLInstance) && _ul_Len != 4)
    {
        /*
         * We get the Number of specific zones of this instance. To do that, we jump over the
         * BIG_Key of the ColSet (4), the total number of ZDX (1) and the number of shared ZDx(1)
         */
        uc_NbOfZDx = *(UCHAR *)((ULONG)_pc_Buf + 4);
        uc_NbOfSpecific = *(UCHAR *)((ULONG)_pc_Buf + 6);

        /* For the Instance, we add uc_NbOZDx extra bytes for the Instance's array of pointers */

        /*$F  For each Specific ZDx, we alloc 13 extra bytes 
			    - 4 for length of the name
				- 7 for the name "NoName" + \0
				- 1 for the ID of the Zone
				- 1 for the ColSetZDx
		*/
        b = pc_dstBuff = (char *) L_malloc(_ul_Len + uc_NbOfZDx + 13 * uc_NbOfSpecific);

        /* We copy the ColSet BIG_KEY, uc_NbOfZDx, uc_NbOfShared, uc_NbOfSpecific, uc_Dummy; */
        L_memcpy(b, _pc_Buf, 8);
        b += 8;
        _pc_Buf += 8;

        /* Create a instance array of pointers */
        for(uc_Index = 0; uc_Index < uc_NbOfZDx; uc_Index++)
        {
            L_memcpy(b, &uc_Index, 1);
            b++;
        }

        for(uc_Index = 0; uc_Index < uc_NbOfSpecific; uc_Index++)
        {
			uc_Type = *(UCHAR *)((ULONG)_pc_Buf + 1);
            /* We copy uc_Flag, uc_Type, uw_BoneIndex. */
            L_memcpy(b, _pc_Buf, 4);
            b += 4;
            _pc_Buf += 4;
			// We add the index to remember the ColSetZDx
			L_memcpy(b, &uc_Index, 1);
			b +=1;

            /* There, we add the Length, the name and the ID of the Zone. */
            ul_Length = 7;
            L_memcpy(b, &ul_Length, 4);
            b += 4;
            L_memcpy(b, Name, 7);
            b += 7;
            uc_Id = 0;
            L_memcpy(b, &uc_Id, 1);
            b += 1;

            switch(uc_Type)
            {
            case COL_C_Zone_Sphere:

                /* We copy the sphere. */
                L_memcpy(b, _pc_Buf, sizeof(MATH_tdst_Vector) + sizeof(float));
                b += sizeof(MATH_tdst_Vector) + sizeof(float);
                _pc_Buf += sizeof(MATH_tdst_Vector) + sizeof(float);
                break;

            case COL_C_Zone_Box:

                /* We copy the Box. */
                L_memcpy(b, _pc_Buf, 2 * sizeof(MATH_tdst_Vector));
                b += 2 * sizeof(MATH_tdst_Vector);
                _pc_Buf += 2 * sizeof(MATH_tdst_Vector);
                break;

            case COL_C_Zone_Cylinder:

                /* We copy the sphere. */
                L_memcpy(b, _pc_Buf, sizeof(MATH_tdst_Vector) + 2 * sizeof(float));
                b += sizeof(MATH_tdst_Vector) + 2 * sizeof(float);
                _pc_Buf += sizeof(MATH_tdst_Vector) + 2 * sizeof(float);
                break;
            }
        }

        /*
         * We copy uw_Activation, uw_Specific, uw_AllowChange, uw_CustomBits1, uw_CustomBits2,
         * uw_CustomBits3
         */
        L_memcpy(b, _pc_Buf, 12);

        /* Finally, we replace the file with the destination buffer */
        BIG_UpdateFileFromBuffer
        (
            _psz_Path,
            _psz_File,
            pc_dstBuff,
            _ul_Len + uc_NbOfZDx + 12 * uc_NbOfSpecific
        );
        L_free(pc_dstBuff);
    }
}

#endif
