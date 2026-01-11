/*$T VERsion_Update26.cpp GC! 1.081 06/30/00 15:25:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLstruct.h"

/*
 =======================================================================================================================
    Aim:    Update Collision Objects. Game Materials new generation of the space.
 =======================================================================================================================
 */
void VERsion_UpdateOneFileVersion26(char *_pc_Buf, ULONG _ul_Len, char *_pz_Path, char *_pz_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	*pc_dstBuff, *a, *b;
	char	*psz_ext;
	UCHAR	uc_Type, uc_Flag;
	ULONG	ul_NewSize, ul_Length;
	ULONG	ul_NbOfPoints, ul_NbOfFaces, ul_NbOfElements;
	USHORT	uw_NbOfTriangles, uw_Flag;
	ULONG	i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Find extension of file */
	psz_ext = strrchr(_pz_File, '.');

	a = _pc_Buf;

	if(psz_ext && !strcmp(psz_ext, EDI_Csz_ExtCOLObject) && (_ul_Len != 4))
	{
		ul_NewSize = _ul_Len;

		/* We have to add a new Invalid BIG_KEY to a GameMaterial File to ALL Cobs */
		ul_NewSize += sizeof(BIG_KEY);

		/* Gets Type of the Cob */
		uc_Type = *(char *) _pc_Buf;
		_pc_Buf++;

		/* Gets Flag of the Cob */
		uc_Flag = *(char *) _pc_Buf;
		_pc_Buf++;

		if
		(
			(uc_Type != COL_C_Zone_Box)
		&&	(uc_Type != COL_C_Zone_Sphere)
		&&	(uc_Type != COL_C_Zone_Cylinder)
		&&	(uc_Type != COL_C_Zone_Triangles)
		)
		{
			return;
		}

		if(uc_Type != COL_C_Zone_Triangles)
		{
			/* We have to add a Material ID to the Mathematical Cob. */
			ul_NewSize += sizeof(ULONG);
		}

		/* Mathematical Cob. */
		if(uc_Type != COL_C_Zone_Triangles)
		{
			/* If the old Cob has a GameMaterial, we dont want to save any longer. */
			if(uc_Flag & COL_C_Cob_GameMat)
			{
				ul_NewSize -= sizeof(COL_tdst_GameMat);

				/* We skip the old GameMaterial info that was stored there. */
				_pc_Buf += sizeof(COL_tdst_GameMat);
			}

			switch(uc_Type)
			{
			case COL_C_Zone_Sphere:
				_pc_Buf += sizeof(COL_tdst_Sphere);
				break;

			case COL_C_Zone_Box:
				_pc_Buf += sizeof(COL_tdst_Box);
				break;

			case COL_C_Zone_Cylinder:
				_pc_Buf += sizeof(COL_tdst_Cylinder);
				break;
			}

			ul_Length = *(ULONG *) _pc_Buf;
			ul_NewSize -= sizeof(ULONG) + ul_Length;
		}
		else
		{
			ul_NbOfPoints = *(ULONG *) _pc_Buf;
			_pc_Buf += 4;

			/* Skip the points array. */
			_pc_Buf += ul_NbOfPoints * sizeof(MATH_tdst_Vector);

			ul_NbOfFaces = *(ULONG *) _pc_Buf;
			_pc_Buf += 4;

			/* Skip the Faces normal. */
			_pc_Buf += ul_NbOfFaces * sizeof(MATH_tdst_Vector);

			ul_NbOfElements = *(ULONG *) _pc_Buf;
			_pc_Buf += 4;

			for(i = 0; i < ul_NbOfElements; i++)
			{
				uw_NbOfTriangles = *(USHORT *) _pc_Buf;
				_pc_Buf += 2;

				uw_Flag = *(USHORT *) _pc_Buf;
				_pc_Buf += 2;

				if(uw_Flag & COL_C_Cob_GameMat)
				{
					ul_NewSize -= sizeof(COL_tdst_GameMat);

					/* We skip the old GameMaterial info that was stored there. */
					_pc_Buf += sizeof(COL_tdst_GameMat);
				}

				/* We have to add Material ID. */
				ul_NewSize += sizeof(ULONG);

				_pc_Buf += uw_NbOfTriangles * sizeof(COL_tdst_IndexedTriangle);
			}

			ul_Length = *(ULONG *) _pc_Buf;
			ul_NewSize -= sizeof(ULONG) + ul_Length;
		}

		_pc_Buf = a;

		/* Allocate a buffer with the new size */
		b = pc_dstBuff = (char *) L_malloc(ul_NewSize);

		/* Add a Key to a GameMaterial File. */
		*(ULONG *) b = BIG_C_InvalidIndex;
		b += sizeof(ULONG);

		/* Gets Type of the Cob */
		uc_Type = *(char *) _pc_Buf;
		*(UCHAR *) b = uc_Type;
		b++;
		_pc_Buf++;

		/* Gets Flag of the Cob */
		uc_Flag = *(char *) _pc_Buf;
		*(UCHAR *) b = 0;	/* WE REMOVE THE GAME MATERIAL FLAG IF THERE IS ONE !!! */
		b++;
		_pc_Buf++;

		if(uc_Type != COL_C_Zone_Triangles)
		{
			/* Add a 0 Material ID; */
			*(ULONG *) b = 0;
			b += sizeof(ULONG);
		}

		/* Mathematical Cob. */
		if(uc_Type != COL_C_Zone_Triangles)
		{
			/* We Skip the old GameMaterial info. */
			if(uc_Flag & COL_C_Cob_GameMat) _pc_Buf += sizeof(COL_tdst_GameMat);

			switch(uc_Type)
			{
			case COL_C_Zone_Sphere:
				L_memcpy(b, _pc_Buf, sizeof(COL_tdst_Sphere));
				b += sizeof(COL_tdst_Sphere);
				_pc_Buf += sizeof(COL_tdst_Sphere);
				break;

			case COL_C_Zone_Box:
				L_memcpy(b, _pc_Buf, sizeof(COL_tdst_Box));
				b += sizeof(COL_tdst_Box);
				_pc_Buf += sizeof(COL_tdst_Box);
				break;

			case COL_C_Zone_Cylinder:
				L_memcpy(b, _pc_Buf, sizeof(COL_tdst_Cylinder));
				b += sizeof(COL_tdst_Cylinder);
				_pc_Buf += sizeof(COL_tdst_Cylinder);
				break;
			}
		}
		else
		{
			ul_NbOfPoints = *(ULONG *) _pc_Buf;
			*(ULONG *) b = ul_NbOfPoints;
			b += 4;
			_pc_Buf += 4;

			L_memcpy(b, _pc_Buf, ul_NbOfPoints * sizeof(MATH_tdst_Vector));
			b += ul_NbOfPoints * sizeof(MATH_tdst_Vector);
			_pc_Buf += ul_NbOfPoints * sizeof(MATH_tdst_Vector);

			ul_NbOfFaces = *(ULONG *) _pc_Buf;
			*(ULONG *) b = ul_NbOfFaces;
			b += 4;
			_pc_Buf += 4;

			L_memcpy(b, _pc_Buf, ul_NbOfFaces * sizeof(MATH_tdst_Vector));
			b += ul_NbOfFaces * sizeof(MATH_tdst_Vector);
			_pc_Buf += ul_NbOfFaces * sizeof(MATH_tdst_Vector);

			ul_NbOfElements = *(ULONG *) _pc_Buf;
			*(ULONG *) b = ul_NbOfElements;
			b += 4;
			_pc_Buf += 4;

			for(i = 0; i < ul_NbOfElements; i++)
			{
				uw_NbOfTriangles = *(USHORT *) _pc_Buf;
				*(USHORT *) b = uw_NbOfTriangles;
				b += 2;
				_pc_Buf += 2;

				uw_Flag = *(USHORT *) _pc_Buf;
				*(USHORT *) b = uw_Flag;
				b += 2;
				_pc_Buf += 2;

				/* Add the Material ID. */
				*(ULONG *) b = i;
				b += 4;

				if(uw_Flag & COL_C_Cob_GameMat) _pc_Buf += sizeof(COL_tdst_GameMat);

				L_memcpy(b, _pc_Buf, uw_NbOfTriangles * sizeof(COL_tdst_IndexedTriangle));
				b += uw_NbOfTriangles * sizeof(COL_tdst_IndexedTriangle);
				_pc_Buf += uw_NbOfTriangles * sizeof(COL_tdst_IndexedTriangle);
			}
		}

		/* Finally, we replace the file with the destination buffer */
		BIG_UpdateFileFromBuffer(_pz_Path, _pz_File, pc_dstBuff, ul_NewSize);

		/* free the buffer */
		L_free(pc_dstBuff);
	}
}

#endif /* ACTIVE_EDITORS */
