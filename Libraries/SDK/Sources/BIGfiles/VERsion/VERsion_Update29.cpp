/*$T VERsion_Update29.cpp GC! 1.081 10/05/00 11:29:56 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/AIstruct.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VERsion_UpdateOneFileVersion29(char *pc_Buf, ULONG _ul_Len, char *_psz_Path, char *_psz_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	*psz_ext, *pc_dstBuff, *a, *b;
	USHORT	uw_NumTracks, uw_Flags, uw_NumEvents, uw_Type, uw_NumFrames;
	ULONG	ul_NonAnimEvents, ul_Size;
	BOOL	b_AnimTrack;
	int		i, j;
	float	f_Delay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_NonAnimEvents = 0;

	/* Find extension of file */
	psz_ext = strrchr(_psz_File, '.');

	/* Empty File ... We do nothing ... */
	if(!_ul_Len) return;

	if(!(psz_ext && !strcmp(psz_ext, EDI_Csz_ExtEventAllsTracks))) return;

	a = pc_Buf;

	/* Number of tracks */
	uw_NumTracks = *(USHORT *) pc_Buf;
	pc_Buf += 2;

	/* Flags */
	pc_Buf += 2;

	/* Parse all tracks */
	for(i = 0; i < uw_NumTracks; i++)
	{
		/* Flags */
		uw_Flags = *(USHORT *) pc_Buf;
		pc_Buf += 2;

		/* Is it an Anim Track ? */
		b_AnimTrack = uw_Flags & EVE_C_Track_Anims;

		/* Gizmo */
		pc_Buf += 2;

		/* Specific Data Length */
		pc_Buf += 4;

		if(b_AnimTrack)
		{
			/* Track Type */
			uw_Type = (short) (*(ULONG *) pc_Buf);
			pc_Buf += 4;
		}
		else
		{
			/* Key GO Track */
			pc_Buf += 4;

			/* Track Name */
			pc_Buf += 16;
		}

		/* Num Events */
		uw_NumEvents = (short) (*(ULONG *) pc_Buf);
		pc_Buf += 4;	/* +4 and not +2 NORMAL !! */

		// Anti-bug...
		if(uw_NumEvents > 10000) return;

		for(j = 0; j < uw_NumEvents; j++)
		{
			if(b_AnimTrack)
			{
				/* Number of Frames */
				pc_Buf += 2;
			}
			else
			{
				ul_NonAnimEvents++;

				/* Delay */
				pc_Buf += 4;

				/* Type */
				uw_Type = *(USHORT *) pc_Buf;
				pc_Buf += 2;
			}

			/* Flags */
			uw_Flags = *(USHORT *) pc_Buf;
			pc_Buf += 2;

			uw_Flags += (uw_Type << 6);

			if((uw_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)
			{
				ul_Size = 0;

				/* Specific Data Size. */
				ul_Size = (ULONG) (*(short *) pc_Buf);
				pc_Buf += ul_Size;
			}

			if((uw_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_AIFunction)
			{
				ul_Size = 0;

				/* Specific Data Size. */
				ul_Size = *(ULONG *) pc_Buf;
				pc_Buf += 4 + (ul_Size * sizeof(AI_tdst_Node));
			}
		}
	}

	pc_Buf = a;

	/* allocate a buffer with the new size */
	b = pc_dstBuff = (char *) L_malloc(_ul_Len - (ul_NonAnimEvents * sizeof(float)));	/* We skip the Delay */

	/* Number of tracks */
	uw_NumTracks = *(USHORT *) pc_Buf;
	L_memcpy(b, pc_Buf, 2);
	pc_Buf += 2;
	b += 2;

	/* Flags */
	L_memcpy(b, pc_Buf, 2);
	pc_Buf += 2;
	b += 2;

	/* Parse all tracks */
	for(i = 0; i < uw_NumTracks; i++)
	{
		/* Flags */
		uw_Flags = *(USHORT *) pc_Buf;
		L_memcpy(b, pc_Buf, 2);
		pc_Buf += 2;
		b += 2;

		/* Is it an Anim Track ? */
		b_AnimTrack = uw_Flags & EVE_C_Track_Anims;

		/* Gizmo */
		L_memcpy(b, pc_Buf, 2);
		pc_Buf += 2;
		b += 2;

		/* Specific Data Length */
		L_memcpy(b, pc_Buf, 4);
		pc_Buf += 4;
		b += 4;

		if(b_AnimTrack)
		{
			/* Track Type */
			uw_Type = (short) (*(ULONG *) pc_Buf);
			L_memcpy(b, pc_Buf, 4);
			pc_Buf += 4;
			b += 4;
		}
		else
		{
			/* Key GO Track */
			L_memcpy(b, pc_Buf, 4);
			pc_Buf += 4;
			b += 4;

			/* Track Name */
			L_memcpy(b, pc_Buf, 16);
			pc_Buf += 16;
			b += 16;
		}

		/* Num Events */
		uw_NumEvents = (short) (*(ULONG *) pc_Buf);
		L_memcpy(b, pc_Buf, 4);
		pc_Buf += 4;
		b += 4;

		for(j = 0; j < uw_NumEvents; j++)
		{
			if(b_AnimTrack)
			{
				/* Number of Frames */
				L_memcpy(b, pc_Buf, 2);
				pc_Buf += 2;
				b += 2;
			}
			else
			{
				/* Delay */
				f_Delay = *(float *) pc_Buf;
				uw_NumFrames = (USHORT) ((f_Delay * 60.0f) + 0.5f);
				L_memcpy(b, &uw_NumFrames, 2);
				pc_Buf += 4;
				b += 2;

				/* Type */
				uw_Type = *(USHORT *) pc_Buf;
				pc_Buf += 2;
			}

			/* Flags */
			uw_Flags = *(USHORT *) pc_Buf;
			uw_Flags += (uw_Type << 6);
			L_memcpy(b, &uw_Flags, 2);
			pc_Buf += 2;
			b += 2;

			if((uw_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)
			{
				ul_Size = 0;

				/* Specific Data Size. */
				ul_Size = (ULONG) (*(short *) pc_Buf);
				L_memcpy(b, pc_Buf, ul_Size);
				pc_Buf += ul_Size;
				b += ul_Size;
			}

			if((uw_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_AIFunction)
			{
				ul_Size = 0;

				/* Specific Data Size. */
				ul_Size = *(ULONG *) pc_Buf;
				L_memcpy(b, pc_Buf, 4 + (ul_Size * sizeof(AI_tdst_Node)));
				pc_Buf += 4 + (ul_Size * sizeof(AI_tdst_Node));
				b += 4 + (ul_Size * sizeof(AI_tdst_Node));;
			}
		}
	}

	/* update the file size */
	_ul_Len -= ul_NonAnimEvents * sizeof(float);

	/* Finally, we replace the file with the destination buffer */
	BIG_UpdateFileFromBuffer(_psz_Path, _psz_File, pc_dstBuff, _ul_Len);

	/* free the buffer */
	L_free(pc_dstBuff);
}

#endif
