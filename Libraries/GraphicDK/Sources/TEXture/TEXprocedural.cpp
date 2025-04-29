/*$T TEXprocedural.c GC! 1.081 03/06/02 09:55:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include <fcntl.h>

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BAStypes.h"

#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKtoed.h"

#include "TEXture/TEXfile.h"
#include "TEXture/TEXstruct.h"
#include "TEXture/TEX_Bink.h"
#include "TEXture/TEXprocedural.h"
#include "TEXture/TEXconvert.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"

#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "SOFT/SOFTlinear.h"
#include "SOFT/SOFTcolor.h"
#include "AIinterp/Sources/Functions/AIfunctions_savephotos.h"

#include "INOut/INOfile.h"

#if defined(_XBOX)
#include "Gx8/Gx8init.h"
#include "Gx8/Gx8video.h"
#endif

#ifdef PSX2_TARGET
#include <eeregs.h>
#include <eestruct.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <libpc.h>
#include "GS_PS2/Gsp.h"
#include "GS_PS2/GSP_Video.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

int								TEX_gi_NbProcedural = 0;
TEX_tdst_Procedural				*TEX_gdst_Procedural = NULL;
TEX_tdst_ProceduralInterface	TEX_gast_ProceduralInterface[TEXPRO_Number];
TEXPRO_tdst_Mpeg				*TEXPRO_pst_CurrentMpeg;

int								TEXPRO_Cos[256];

#ifdef ACTIVE_EDITORS
char							*TEX_gasz_ProceduralInterfaceName[TEXPRO_Number] = { "Unknown", "Water", "Fire", "MPeg",
		"Photo", "Plasma" };
#endif /* ACTIVE_EDITORS */

/*$4
 ***********************************************************************************************************************
    Unknow procedural texture functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEXPRO_l_UnknowUpdate(TEX_tdst_Procedural *_pst_Pro)
{
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_UnknowDisrupt(TEX_tdst_Procedural *_pst_Pro)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *TEXPRO_pv_UnknowLoad(char *_pc_Buffer, LONG _l_Size, ULONG _ul_Width, ULONG _ul_Height)
{
	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_UnknowFree(TEX_tdst_Procedural *_pst_Pro)
{
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_UnknowSave(TEX_tdst_Procedural *_pst_Pro)
{
	/*~~~~~~~~~~~*/
	LONG	l_Size;
	/*~~~~~~~~~~~*/

	l_Size = 4;
	SAV_Buffer(&l_Size, 4);
}

#endif

/*$4
 ***********************************************************************************************************************
    Fire procedural texture functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEXPRO_l_FireUpdate(TEX_tdst_Procedural *_pst_Pro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Fire	*pst_Fire;
	ULONG				X, Y;
	LONG				W, H;
	UCHAR				*S1, *S2;
	ULONG				*S;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Fire = (TEXPRO_tdst_Fire *) _pst_Pro->p_Data;
	W = (LONG) _pst_Pro->uw_Width;
	H = (LONG) _pst_Pro->uw_Height;

	if(pst_Fire->i_Type == 0)
	{
		S1 = pst_Fire->puc_Surface[0] + W * 3 + 1;
		S2 = pst_Fire->puc_Surface[1] + W + 1;

		Y = H - 4;
		while(Y--)
		{
			X = W - 2;
			while(X--)
			{
				*S1 = (S2[1L] + S2[-1L] + S2[W] + S2[-W]) >> 2;
				*S1 -= (*S1 >> 4);
				S1++;
				S2++;
			}

			S1 += 2;
			S2 += 2;
		}

		S1 = pst_Fire->puc_Surface[0];
		pst_Fire->puc_Surface[0] = pst_Fire->puc_Surface[1];
		pst_Fire->puc_Surface[1] = S1;
	}
	else if(pst_Fire->i_Type == 1)
	{
		S1 = pst_Fire->puc_Surface[0] + W * H;

		Y = (H - 2) * W;
		while(Y--) *S1-- = (S1[-(W - 1)] + S1[-W] + S1[-(W + 1)] + S1[-(W << 1)]) >> 2;
	}

	S1 = pst_Fire->puc_Surface[0];
	S = (ULONG *) _pst_Pro->pc_Buffer;

	Y = W;
	while(Y--)
	{
		X = H;
		while(X--)
		{
			*S++ = pst_Fire->aul_Palette[*S1++];
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_FireDisrupt(TEX_tdst_Procedural *_pst_Pro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Fire	*pst_Fire;
	UCHAR				*S1, *S2;
	ULONG				X, Y, N, L, U, V, W, H;
	ULONG				MaxL;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	return;

	pst_Fire = (TEXPRO_tdst_Fire *) _pst_Pro->p_Data;
	W = (ULONG) _pst_Pro->uw_Width;
	H = (ULONG) _pst_Pro->uw_Height;

	if(pst_Fire->i_Type == 0)
	{
		S1 = pst_Fire->puc_Surface[0];
		S2 = pst_Fire->puc_Surface[1];

		MaxL = (W >> 3);

		N = (W >> 2);
		while(N--)
		{
			X = rand() % (W >> 1) + (W >> 2) - MaxL;
			Y = (ULONG) (0.0001f * (H - MaxL) * (float) ((rand() % 10) * (rand() % 10) * (rand() % 10) * (rand() % 10)));

			L = rand() % MaxL;
			U = rand() % MaxL;

			while(L--)
			{
				V = U;
				while(V--)
				{
					if(S1[X + V + (Y + L) * W] < 0xF3)
						S1[X + V + (Y + L) * W] += 12;
					else
						S1[X + V + (Y + L) * W] = 0xFF;

					if(S2[X + V + (Y + L) * W] < 0xF3)
						S2[X + V + (Y + L) * W] += 12;
					else
						S2[X + V + (Y + L) * W] = 0xFF;
				}
			}
		}
	}
	else if(pst_Fire->i_Type == 1)
	{
		S1 = pst_Fire->puc_Surface[0] + W;
		X = W;
		while(X--)
		{
			*S1++ = (rand() & 1) ? 255 : 0;
		}
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		static unsigned char	p1, p2, p3, p4;
		unsigned char			t1, t2, t3, t4;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		S1 = pst_Fire->puc_Surface[0];

		t1 = p1;
		t2 = p2;
		for(Y = 0; Y < H; Y++)
		{
			t3 = p3;
			t4 = p4;
			for(X = 0; X < W; X++)
			{
				*S1++ = TEXPRO_Cos[t1] + TEXPRO_Cos[t2] + TEXPRO_Cos[t3] + TEXPRO_Cos[t4];
				t3 += 1;
				t4 += 3;
			}

			t1 += 2;
			t2 += 1;
		}

		p1 += 1;
		p2 -= 2;
		p3 += 3;
		p4 -= 4;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *TEXPRO_pv_FireLoad(char *_pc_Buffer, LONG _l_Size, ULONG _ul_Width, ULONG _ul_Height)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Fire	*pst_Fire;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Fire = (TEXPRO_tdst_Fire *) MEM_p_Alloc(sizeof(TEXPRO_tdst_Fire));
	pst_Fire->puc_Surface[0] = (UCHAR *) MEM_p_Alloc(_ul_Width * _ul_Height);
	L_memset(pst_Fire->puc_Surface[0], 0, _ul_Width * _ul_Height);
	pst_Fire->puc_Surface[1] = (UCHAR *) MEM_p_Alloc(_ul_Width * _ul_Height);
	L_memset(pst_Fire->puc_Surface[1], 0, _ul_Width * _ul_Height);

	i = 64;
	while(i--)
	{
		pst_Fire->aul_Palette[0 + i] = ((i) << 10) | ((i) << 18) | ((i) << 26);
		pst_Fire->aul_Palette[64 + i] = ((i) << 2) | 0xFFFFFF00;
		pst_Fire->aul_Palette[128 + i] = 0xffffffff;
		pst_Fire->aul_Palette[192 + i] = ((63 - i) << 2) | ((63 - i) << 18) | ((63 - i) << 10) | ((63 - i) << 26);
	}

	return pst_Fire;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_FireFree(TEX_tdst_Procedural *_pst_Pro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Fire	*pst_Fire;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Fire = (TEXPRO_tdst_Fire *) _pst_Pro->p_Data;

	MEM_Free(pst_Fire->puc_Surface[0]);
	MEM_Free(pst_Fire->puc_Surface[1]);
	MEM_Free(pst_Fire);
}

/*$4
 ***********************************************************************************************************************
    Water procedural texture functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEXPRO_l_WaterUpdate(TEX_tdst_Procedural *_pst_Pro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Water	*pst_Water;
	ULONG				X, Y;
	LONG				Z;
	SHORT				*S1, *S2;
	ULONG				*S;
	LONG				W, H;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Water = (TEXPRO_tdst_Water *) _pst_Pro->p_Data;
	W = (LONG) _pst_Pro->uw_Width;
	H = (LONG) _pst_Pro->uw_Height;

	S1 = (SHORT*)pst_Water->puc_Surface[0] + W + 1;
	S2 = (SHORT*)pst_Water->puc_Surface[1] + W + 1;
	L_memset(pst_Water->puc_Surface[0], 0, W * 2);
	L_memset(pst_Water->puc_Surface[1], 0, W * 2);
	L_memset(pst_Water->puc_Surface[0] + W * H - W, 0, W * 2);
	L_memset(pst_Water->puc_Surface[1] + W * H - W, 0, W * 2);
	Y = H - 2;

	while(Y--)
	{
		X = W - 2;
		while(X--)
		{
			Z = ((S2[1L] + S2[-1L] + S2[W] + S2[-W]) >> 1) - *S1;
			Z -= Z >> 8;	/* 1 / 2 */
			Z = lMax(Z, -32 << 8);
			Z = lMin(Z, 32 << 8);
			*S1 = (SHORT) Z;
			S1++;
			S2++;
		}

		*(S1++) = 0;
		*(S1++) = 0;
		*(S2++) = 0;
		*(S2++) = 0;
	}

	Y = W * H;
	S1 = (SHORT*)pst_Water->puc_Surface[0];
	S2 = (SHORT*)pst_Water->puc_Surface[1];
	while(Y--)
	{
		*S1 -= *S1 >> 6;
		*S2 -= *S2 >> 6;
		S1++;
		S2++;
	}

	/*
	 * S1 = pst_Water->puc_Surface[0] + W; S2 = pst_Water->puc_Surface[1] + W; Y = H -
	 * 2; while(Y--) { Z = ((S2[1L] + S2[W - 1] + S2[W] + S2[-W]) >> 1) - *S1; if (Z &
	 * 0x8000000) S1 = 0; else { Z -= Z >> 8; S1 = (UCHAR) Z; } S1 += W - 1; S2 += W -
	 * 1; Z = ((S2[-W + 1] + S2[-1L] + S2[W] + S2[-W]) >> 1) - *S1; if (Z & 0x8000000)
	 * S1 = 0; else { Z -= Z >> 8; S1 = (UCHAR) Z; } S1++; S2++; } S1 =
	 * pst_Water->puc_Surface[0] + 1; S2 = pst_Water->puc_Surface[1] + 1; X = W - 2;
	 * l_Size = W * (H - 1); while(X--) { Z = ((S2[1L] + S2[-1L] + S2[W] + S2[l_Size])
	 * >> 1) - *S1; if (Z & 0x8000000) S1 = 0; else { Z -= Z >> 8; S1 = (UCHAR) Z; }S1
	 * += l_Size; S2 += l_Size; Z = ((S2[1L] + S2[-1L] + S2[-l_Size] + S2[-W]) >> 1) -
	 * S1; if (Z & 0x8000000) S1 = 0; else { Z -= Z >> 8; S1 = (UCHAR) Z; } S1 += 1 -
	 * l_Size; S2 += 1 - l_Size; }
	 */
	S1 = (SHORT*)pst_Water->puc_Surface[0];
	pst_Water->puc_Surface[0] = pst_Water->puc_Surface[1];
	pst_Water->puc_Surface[1] = (USHORT*)S1;

	S1 = (SHORT*)pst_Water->puc_Surface[0];
	S = (ULONG *) _pst_Pro->pc_Buffer;

	Y = H;
	while(Y--)
	{
		X = W;
		while(X--)
		{
			*S++ = pst_Water->aul_Palette[128 + (CHAR) ((*S1 - *(S1 + 1)) >> 6)];
			S1++;
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_WaterDisrupt(TEX_tdst_Procedural *_pst_Pro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Water	*pst_Water;
	USHORT				*S1, *S2;
	ULONG				X, Y, N, M;
	static float		ALPHA[2], KX[2], KY[2];
	static LONG			K;
	SHORT				w_Value;
	LONG				W, H;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	K++;

	pst_Water = (TEXPRO_tdst_Water *) _pst_Pro->p_Data;
	W = (LONG) _pst_Pro->uw_Width;
	H = (LONG) _pst_Pro->uw_Height;

	S1 = pst_Water->puc_Surface[0] + W;
	S2 = pst_Water->puc_Surface[1] + W;

	N = 4;
	M = rand() % 100;
	while(N--)
	{
		if(N < 2)
		{
			ALPHA[N] += (50 - rand() % 100) / 200.0f;
			KX[N] += (float) sin(ALPHA[N]) * M * 0.02f;
			KY[N] += (float) cos(ALPHA[N]) * M * 0.02f;
			X = ((ULONG) KX[N]) % W;
			Y = ((ULONG) KY[N]) % H;
			if((LONG) X > W - 8) X = W - 8;
			if((LONG) Y > H - 8) Y = H - 8;
		}
		else
		{
			X = rand() % (W - 8) + 4;
			Y = rand() % (H - 8) + 4;
		}

		w_Value = (SHORT) ((sin(K * 0.2f)) * 200.0f);

		S1[X + (Y * W)] += w_Value;
		S1[X + ((Y + 1) * W)] += w_Value;
		S1[X + 1 + (Y * W)] += w_Value;
		S1[X + 1 + ((Y + 1) * W)] += w_Value;

		S2[X + (Y * W)] += w_Value;
		S2[X + ((Y + 1) * W)] += w_Value;
		S2[X + 1 + (Y * W)] += w_Value;
		S2[X + 1 + ((Y + 1) * W)] += w_Value;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *TEXPRO_pv_WaterLoad(char *_pc_Buffer, LONG _l_Size, ULONG _ul_Width, ULONG _ul_Height)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Water	*pst_Water;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Water = (TEXPRO_tdst_Water *) MEM_p_Alloc(sizeof(TEXPRO_tdst_Water));
	pst_Water->puc_Surface[0] = (USHORT *) MEM_p_Alloc(_ul_Width * _ul_Height * 2);
	L_memset(pst_Water->puc_Surface[0], 0, _ul_Width * _ul_Height * 2);
	pst_Water->puc_Surface[1] = (USHORT *) MEM_p_Alloc(_ul_Width * _ul_Height * 2);
	L_memset(pst_Water->puc_Surface[1], 0, _ul_Width * _ul_Height * 2);

	i = 256;
	while(i--)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		unsigned long	ColorA, Color2;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ColorA = (unsigned long) (255.0f * fAbs(fSin((float) i * 4.0f * 3.1415927f / 256.0f)));
		if(i < 128)
			Color2 = 0;
		else
			Color2 = (i - 128) << 1;
		if(i > 128)
		{
			/*~~~~~~~*/
			float	C2;
			/*~~~~~~~*/

			C2 = fAbs(fSin((float) i * 4.0f * 3.1415927f / 256.0f));
			C2 *= C2;
			C2 *= C2;
			Color2 = (unsigned long) (128.0f * C2);
		}
		else
			Color2 = 0;

		pst_Water->aul_Palette[i] = (Color2) | ((Color2) << 8) | ((Color2) << 16) | ((ColorA) << 24);
	}

	/*
	 * pst_Water->aul_Palette[128] = 0xffff; i = 10; while (i--) {
	 * pst_Water->aul_Palette[i] = 0xff; pst_Water->aul_Palette[255-i] = 0xff; } //
	 * while(i--) pst_Water->aul_Palette[i + 128] |= 0xff000000;
	 */
	return pst_Water;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_WaterFree(TEX_tdst_Procedural *_pst_Pro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Water	*pst_Water;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Water = (TEXPRO_tdst_Water *) _pst_Pro->p_Data;

	MEM_Free(pst_Water->puc_Surface[0]);
	MEM_Free(pst_Water->puc_Surface[1]);
	MEM_Free(pst_Water);
}

/*$4
 ***********************************************************************************************************************
    Mpeg procedural texture functions
 ***********************************************************************************************************************
 */

#ifndef PSX2_TARGET

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_Mpeg_GoStart(void)
{
	if(TEXPRO_pst_CurrentMpeg->ul_Pos == (ULONG) - 1) return;
	TEXPRO_pst_CurrentMpeg->ul_Offset = TEXPRO_pst_CurrentMpeg->ul_Pos + 4;
	TEXPRO_pst_CurrentMpeg->c_SystemFlags &= ~TEXPRO_C_Mpeg_ReachEnd;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int TEXPRO_i_Mpeg_Read2048(int Ptr)
{
	/*~~~~~~~*/
	int i_Size;
	/*~~~~~~~*/

#ifdef ACTIVE_EDITORS
	if(LOA_IsBinarizing()) return 0;
#endif
	if(TEXPRO_pst_CurrentMpeg->ul_Pos == (ULONG) - 1) return 0;
	i_Size = TEXPRO_pst_CurrentMpeg->ul_EndOffset - TEXPRO_pst_CurrentMpeg->ul_Offset;
	if(i_Size > 2048)
		i_Size = 2048;
	else
		TEXPRO_pst_CurrentMpeg->c_SystemFlags |= TEXPRO_C_Mpeg_ReachEnd;

	BIG_Read(TEXPRO_pst_CurrentMpeg->ul_Offset, (void *) Ptr, i_Size);
	TEXPRO_pst_CurrentMpeg->ul_Offset += i_Size;
	return i_Size;
}

#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEXPRO_l_MpegUpdate(TEX_tdst_Procedural *_pst_Pro)
{
#if defined(_XBOX)
	extern bool Gx8_Update_Video;

    Gx8_Update_Video=true;

	return TRUE;

#elif defined( BINK_VIDEO )
	TEXPRO_tdst_Mpeg		*pst_Mpeg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mpeg = (TEXPRO_tdst_Mpeg *) _pst_Pro->p_Data;
	TEXPRO_pst_CurrentMpeg = (TEXPRO_tdst_Mpeg *) _pst_Pro->p_Data;
	if(pst_Mpeg->ul_BINK_Key != BIG_C_InvalidKey)
	{
		if(!pst_Mpeg->Bink)
		{
			pst_Mpeg->Bink = MEM_p_Alloc(sizeof(TEX_tdst_Bink));
			L_memset(pst_Mpeg->Bink , 0 , sizeof(TEX_tdst_Bink));
			pst_Mpeg->Bink->_pst_Pro =  _pst_Pro;
			BINK_JADE_Create(pst_Mpeg->Bink,pst_Mpeg->ul_BINK_Key);
		}

		/* gestion des flags et des requêtes */
		if
		(
			(TEXPRO_pst_CurrentMpeg->c_SystemFlags & TEXPRO_C_Mpeg_ReachEnd)
		&&	(TEXPRO_pst_CurrentMpeg->c_Flags & TEXPRO_C_Mpeg_StopAtEnd)
		)
		{
			if (TEXPRO_pst_CurrentMpeg->c_Request != TEXPRO_C_Mpeg_RequestRestart)
				TEXPRO_pst_CurrentMpeg->c_Request = TEXPRO_C_Mpeg_RequestStop;
		}

		if(TEXPRO_pst_CurrentMpeg->c_Request)
		{
			if(TEXPRO_pst_CurrentMpeg->c_Request == TEXPRO_C_Mpeg_RequestStop)
			{
				TEXPRO_pst_CurrentMpeg->c_Flags |= TEXPRO_C_Mpeg_Pause;
				L_memset(_pst_Pro->pc_Buffer, 0, _pst_Pro->uw_Width * _pst_Pro->uw_Height * 4);
				TEXPRO_pst_CurrentMpeg->c_Request = 0;
				return 1;
			}

			if(TEXPRO_pst_CurrentMpeg->c_Request == TEXPRO_C_Mpeg_RequestRestart)
			{
				TEXPRO_pst_CurrentMpeg->c_Flags &= ~TEXPRO_C_Mpeg_Pause;
				BINK_JADE_Restart(pst_Mpeg->Bink);
			}

			TEXPRO_pst_CurrentMpeg->c_Request = 0;
		}

		/* mpeg en pause, pas de refresh */
		if(TEXPRO_pst_CurrentMpeg->c_Flags & TEXPRO_C_Mpeg_Pause) return FALSE;

		BINK_JADE_Update(pst_Mpeg->Bink);
		_pst_Pro->uw_Flags &= ~TEXPRO_UpdatedFromACopy;
	}
#elif defined(NO_BINK)
	_pst_Pro->uw_Flags &= ~TEXPRO_UpdatedFromACopy;
#elif defined( ACTIVE_EDITORS ) || defined( _PC_RETAIL ) || defined( PCWIN_TOOL )
	TEX_tdst_File_Desc		st_Src, st_Tgt;


#if defined(ACTIVE_EDITORS)
	if(LOA_gb_SpeedMode) return 0;
#endif	// defined(ACTIVE_EDITORS)


	TEXPRO_pst_CurrentMpeg = (TEXPRO_tdst_Mpeg *) _pst_Pro->p_Data;
	if(TEXPRO_pst_CurrentMpeg->ul_Pos == (ULONG) - 1) return FALSE;

	/* gestion des flags et des requêtes */
	if
	(
		(TEXPRO_pst_CurrentMpeg->c_SystemFlags & TEXPRO_C_Mpeg_ReachEnd)
	&&	(TEXPRO_pst_CurrentMpeg->c_Flags & TEXPRO_C_Mpeg_StopAtEnd)
	)
	{
        if (TEXPRO_pst_CurrentMpeg->c_Request != TEXPRO_C_Mpeg_RequestRestart)
		    TEXPRO_pst_CurrentMpeg->c_Request = TEXPRO_C_Mpeg_RequestStop;
	}

	if(TEXPRO_pst_CurrentMpeg->c_Request)
	{
		if(TEXPRO_pst_CurrentMpeg->c_Request == TEXPRO_C_Mpeg_RequestStop)
		{
			TEXPRO_pst_CurrentMpeg->c_Flags |= TEXPRO_C_Mpeg_Pause;
			L_memset(_pst_Pro->pc_Buffer, 0, _pst_Pro->uw_Width * _pst_Pro->uw_Height * 4);
			TEXPRO_pst_CurrentMpeg->c_Request = 0;
			return 1;
		}

		if(TEXPRO_pst_CurrentMpeg->c_Request == TEXPRO_C_Mpeg_RequestRestart)
		{
			TEXPRO_pst_CurrentMpeg->c_Flags &= ~TEXPRO_C_Mpeg_Pause;
			MPEG_GOTO(0);
		}

		TEXPRO_pst_CurrentMpeg->c_Request = 0;
	}

    /* mpeg en pause, pas de refresh */
	if(TEXPRO_pst_CurrentMpeg->c_Flags & TEXPRO_C_Mpeg_Pause) return FALSE;
	
	if(!LOA_gb_SpeedMode) MPEG_FRAME();

	if(TEXPRO_pst_CurrentMpeg->st_IO.ulBufferResult)
	{
		st_Src.uw_Width = (unsigned short) TEXPRO_pst_CurrentMpeg->st_IO.SizeX;
		st_Src.uw_Height = (unsigned short) TEXPRO_pst_CurrentMpeg->st_IO.SizeY;
		st_Src.p_Bitmap = (void *) TEXPRO_pst_CurrentMpeg->st_IO.ulBufferResult;

		st_Tgt.uw_Width = _pst_Pro->uw_Width;
		st_Tgt.uw_Height = _pst_Pro->uw_Height;
		st_Tgt.p_Bitmap = _pst_Pro->pc_Buffer;

		TEX_l_ConvertSize_32(&st_Src, &st_Tgt);
	}

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#elif defined(PSX2_TARGET)
	TEXPRO_tdst_Mpeg		*pst_Mpeg;
	TEX_tdst_File_Desc		st_Src, st_Tgt;
	static GSP_VideoStream	stV2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mpeg = (TEXPRO_tdst_Mpeg *) _pst_Pro->p_Data;
	TEXPRO_pst_CurrentMpeg = (TEXPRO_tdst_Mpeg *) _pst_Pro->p_Data;
	if(pst_Mpeg->ul_PSX2_IPUKey != BIG_C_InvalidKey)
	{
		if(!pst_Mpeg->pst_Video)
		{
			pst_Mpeg->ul_Pos = BIG_ul_SearchKeyToPos(pst_Mpeg->ul_PSX2_IPUKey);
			pst_Mpeg->pst_Video = MEM_p_Alloc(sizeof(GSP_VideoStream));

			GSP_VideoStreamCreate
			(
				pst_Mpeg->pst_Video,
				pst_Mpeg->ul_Pos + 4,
				pst_Mpeg->ul_MaxBufSize,
				_pst_Pro->pc_Buffer
			);
		}

		/* gestion des flags et des requêtes */
		if
		(
			(TEXPRO_pst_CurrentMpeg->c_SystemFlags & TEXPRO_C_Mpeg_ReachEnd)
		&&	(TEXPRO_pst_CurrentMpeg->c_Flags & TEXPRO_C_Mpeg_StopAtEnd)
		)
		{
			if (TEXPRO_pst_CurrentMpeg->c_Request != TEXPRO_C_Mpeg_RequestRestart)
				TEXPRO_pst_CurrentMpeg->c_Request = TEXPRO_C_Mpeg_RequestStop;
		}

		if(TEXPRO_pst_CurrentMpeg->c_Request)
		{
			if(TEXPRO_pst_CurrentMpeg->c_Request == TEXPRO_C_Mpeg_RequestStop)
			{
				TEXPRO_pst_CurrentMpeg->c_Flags |= TEXPRO_C_Mpeg_Pause;
				L_memset(_pst_Pro->pc_Buffer, 0, _pst_Pro->uw_Width * _pst_Pro->uw_Height * 4);
				TEXPRO_pst_CurrentMpeg->c_Request = 0;
				return 1;
			}

			if(TEXPRO_pst_CurrentMpeg->c_Request == TEXPRO_C_Mpeg_RequestRestart)
			{
				TEXPRO_pst_CurrentMpeg->c_Flags &= ~TEXPRO_C_Mpeg_Pause;
				GSP_VideoStreamRestart(pst_Mpeg->pst_Video);
			}

			TEXPRO_pst_CurrentMpeg->c_Request = 0;
		}

		/* mpeg en pause, pas de refresh */
		if(TEXPRO_pst_CurrentMpeg->c_Flags & TEXPRO_C_Mpeg_Pause) return FALSE;

		GSP_VideoStreamSync(pst_Mpeg->pst_Video);
	}
#elif defined(_XENON)
#pragma message ("video not supported on XENON yet\n")
#else
#  error Unsupported Platform.
#endif
	return TRUE;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */

#if defined(_XBOX)

bool TEXPRO_CreateTmpVideoFile(unsigned long ul_Key)
{
#define BUFFER_SIZE 512*1024

	FILE *f;
	unsigned long ul_Pos;
	void *p_TempBuffer;
	int i_Size,i_BufferSize;

	ul_Pos = BIG_ul_SearchKeyToPos(ul_Key);
	
	if(ul_Pos != (ULONG) - 1)
	{
		i_Size = BIG_ul_GetLengthFile(ul_Pos);

		f=fopen("z:\\temp.xmv","wb+");

		ul_Pos += 4;

		p_TempBuffer = (void *)MEM_p_Alloc(BUFFER_SIZE);

		while (i_Size>0)
		{
			if (i_Size<BUFFER_SIZE)
			{
				i_BufferSize=i_Size;
			}
			else
			{
				i_BufferSize=BUFFER_SIZE;
			}
		
			BIG_Read(ul_Pos, p_TempBuffer, i_BufferSize);

			fwrite(p_TempBuffer, i_BufferSize, 1, f);

			i_Size -= i_BufferSize;
			ul_Pos += i_BufferSize;
		}

		MEM_Free( p_TempBuffer );

		fclose(f);

	}
	else
	{
		return false;
	}

	return true;
}

#endif

void *TEXPRO_pv_MpegLoad(char *_pc_Buffer, LONG _l_Size, ULONG _ul_Width, ULONG _ul_Height)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Mpeg	*pst_Mpeg;
	ULONG				ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mpeg = (TEXPRO_tdst_Mpeg *) MEM_p_Alloc(sizeof(TEXPRO_tdst_Mpeg));
	L_memset(pst_Mpeg, 0, sizeof(TEXPRO_tdst_Mpeg));
#ifndef PSX2_TARGET
	//pst_Mpeg->st_IO.ulFlags = MPEG_Mode_VerticalFlip | MPEG_Mode_SwapRedBlue;
    pst_Mpeg->st_IO.ulFlags = MPEG_Mode_SwapRedBlue;
	pst_Mpeg->st_IO.GoStart = TEXPRO_Mpeg_GoStart;
	pst_Mpeg->st_IO.Read2048 = TEXPRO_i_Mpeg_Read2048;
#endif
	if(_l_Size != 0) ul_Size = LOA_ReadULong(&_pc_Buffer);

	if((_l_Size == 0) || (_l_Size != (LONG) ul_Size))
	{
		pst_Mpeg->ul_Key = (ULONG) - 1;
		pst_Mpeg->ul_PSX2_IPUKey = (ULONG) - 1;
		pst_Mpeg->ul_BINK_Key = (ULONG) - 1;
		pst_Mpeg->ul_Pos = (ULONG) - 1;
	}
	else
	{
		pst_Mpeg->ul_Key = LOA_ReadULong(&_pc_Buffer);
		if(_l_Size == 8)
		{
			pst_Mpeg->ul_PSX2_IPUKey = BIG_C_InvalidKey;
			pst_Mpeg->ul_BINK_Key = BIG_C_InvalidKey;
			pst_Mpeg->ul_XMV_Key =  BIG_C_InvalidKey;
			pst_Mpeg->ul_MaxBufSize = pst_Mpeg->ul_NbImages = pst_Mpeg->ul_Size = pst_Mpeg->ul_SizeY = pst_Mpeg->ul_SizeX = pst_Mpeg->ul_Size = 0;
		}
		else
		if(_l_Size == 32)
		{
			pst_Mpeg->ul_PSX2_IPUKey = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_MaxBufSize = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_NbImages = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_Size = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_SizeY = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_SizeX = LOA_ReadULong(&_pc_Buffer);

			pst_Mpeg->ul_BINK_Key = BIG_C_InvalidKey;
			pst_Mpeg->ul_BinkSizeY = pst_Mpeg->ul_BinkSizeX = pst_Mpeg->ul_BinkBpp = 0;
			pst_Mpeg->ul_XMV_Key =  BIG_C_InvalidKey;

		}
		else
		if(_l_Size == 32 + 16)
		{
			pst_Mpeg->ul_PSX2_IPUKey = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_MaxBufSize = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_NbImages = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_Size = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_SizeY = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_SizeX = LOA_ReadULong(&_pc_Buffer);

			pst_Mpeg->ul_BINK_Key = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_BinkSizeX = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_BinkSizeY = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_BinkBpp = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_XMV_Key =  BIG_C_InvalidKey;
		}
		else
		{
			pst_Mpeg->ul_PSX2_IPUKey = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_MaxBufSize = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_NbImages = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_Size = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_SizeY = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_SizeX = LOA_ReadULong(&_pc_Buffer);

			pst_Mpeg->ul_BINK_Key = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_BinkSizeX = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_BinkSizeY = LOA_ReadULong(&_pc_Buffer);
			pst_Mpeg->ul_BinkBpp = LOA_ReadULong(&_pc_Buffer);

			pst_Mpeg->ul_XMV_Key =  LOA_ReadULong(&_pc_Buffer);
		}

		pst_Mpeg->ul_Pos = BIG_ul_SearchKeyToPos(pst_Mpeg->ul_Key);
		if(pst_Mpeg->ul_Pos != (ULONG) - 1)
		{
			pst_Mpeg->ul_Offset = pst_Mpeg->ul_Pos + 4;
			pst_Mpeg->ul_EndOffset = pst_Mpeg->ul_Offset + BIG_ul_GetLengthFile(pst_Mpeg->ul_Pos);
#ifndef PSX2_TARGET
			TEXPRO_pst_CurrentMpeg = pst_Mpeg;
#ifndef _XBOX
#ifndef _GAMECUBE
#ifdef ACTIVE_EDITORS
			if(!LOA_IsBinarizing())
#else
				if(!LOA_IsBinaryData())
#endif
					MPEG_INIT(&pst_Mpeg->st_IO);
#endif					
#endif
#endif
		}
	}

#if defined(_XBOX)
	/*
	if (TEXPRO_CreateTmpVideoFile( pst_Mpeg->ul_XMV_Key ))
	{
		Gx8_InitVideo(GX8_M_SD(GDI_gpst_CurDD), "z:\\temp.xmv");
	}
	else
	{
		Gx8_SetNoVideo();		
	}
*/
	
	// This maps videos to external files. Used for E3, maybe it can be used
    // for final production if the number of videos is small enough.
	switch(pst_Mpeg->ul_PSX2_IPUKey)
	{

	case 1644177131:
		Gx8_InitVideo(GX8_M_SD(GDI_gpst_CurDD), "d:\\boxing.xmv");
		break;

	case 3204452092:
		Gx8_InitVideo(GX8_M_SD(GDI_gpst_CurDD), "d:\\cap_course_01.xmv");
		break;

	case 3204452101:
		Gx8_InitVideo(GX8_M_SD(GDI_gpst_CurDD), "d:\\propaganda_01.xmv");
		break;

	case 3204463234:
		Gx8_InitVideo(GX8_M_SD(GDI_gpst_CurDD), "d:\\video_kehck_1.xmv");
	break;

    case 3204463236:
		Gx8_InitVideo(GX8_M_SD(GDI_gpst_CurDD), "d:\\video_kehck_2.xmv");
	break;

	case 3204463238:
		Gx8_InitVideo(GX8_M_SD(GDI_gpst_CurDD), "d:\\video_kehck_3.xmv");
	break;

	case 889250496:
		Gx8_InitVideo(GX8_M_SD(GDI_gpst_CurDD), "d:\\Correspondants_Spoon01.xmv");
	break;

	case 3489678005:
		Gx8_InitVideo(GX8_M_SD(GDI_gpst_CurDD), "d:\\videomap_matrix.xmv");
	break;

	default:
		Gx8_InitVideo(GX8_M_SD(GDI_gpst_CurDD), "d:\\videomap_matrix.xmv");
		break;
	}

#endif


	return pst_Mpeg;
}

//#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_MpegFree(TEX_tdst_Procedural *_pst_Pro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Mpeg	*pst_Mpeg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mpeg = (TEXPRO_tdst_Mpeg *) _pst_Pro->p_Data;
#ifdef BINK_VIDEO
	if(pst_Mpeg->ul_BINK_Key != BIG_C_InvalidKey)
	{
		if(pst_Mpeg->Bink)
		{
			BINK_JADE_Destroy(pst_Mpeg->Bink);
			MEM_Free(pst_Mpeg->Bink);
			pst_Mpeg->Bink = NULL;
		}
	}
#elif defined(PSX2_TARGET)
	if(pst_Mpeg->ul_PSX2_IPUKey != BIG_C_InvalidKey)
	{
		if(pst_Mpeg->pst_Video)
		{
			GSP_VideoStreamClose(pst_Mpeg->pst_Video);
			MEM_Free(pst_Mpeg->pst_Video);
		}
	}

#endif
	MEM_Free(pst_Mpeg);

#if defined(_XBOX)
	Gx8_CloseVideo();
#endif
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_Mpeg_SetKey(TEX_tdst_Procedural *_pst_Pro, ULONG _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Mpeg	*pst_Mpeg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mpeg = (TEXPRO_tdst_Mpeg *) _pst_Pro->p_Data;

	pst_Mpeg->ul_Offset = 0;
	pst_Mpeg->ul_Key = _ul_Key;
	pst_Mpeg->ul_Pos = BIG_ul_SearchKeyToPos(pst_Mpeg->ul_Key);
	if(pst_Mpeg->ul_Pos != (ULONG) - 1)
	{
		pst_Mpeg->ul_Offset = pst_Mpeg->ul_Pos + 4;
		pst_Mpeg->ul_EndOffset = pst_Mpeg->ul_Offset + BIG_ul_GetLengthFile(pst_Mpeg->ul_Pos) + 4;
		TEXPRO_pst_CurrentMpeg = pst_Mpeg;
#ifndef _XBOX
#ifdef ACTIVE_EDITORS
		if(!LOA_IsBinarizing())
#else
			if(!LOA_IsBinaryData())
#endif
				MPEG_INIT(&pst_Mpeg->st_IO);
#endif
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_Mpeg_SetIPUKey(TEX_tdst_Procedural *_pst_Pro, ULONG _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Mpeg	*pst_Mpeg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define TEMPIPUBUFSIZE	8192
	pst_Mpeg = (TEXPRO_tdst_Mpeg *) _pst_Pro->p_Data;

	pst_Mpeg->ul_PSX2_IPUKey = _ul_Key;
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG	ulPos, ulSize, ulSizeOfFrameCounter;
		ULONG	*p_TempBuffer;
		UCHAR	*p_FinalFrameFinder;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ulPos = BIG_ul_SearchKeyToPos(pst_Mpeg->ul_PSX2_IPUKey);
		if(ulPos != (ULONG) - 1)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			ULONG	BaseRead, LastRead, ulLastImgaKeyFinder;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			p_TempBuffer = (ULONG*)MEM_p_Alloc(TEMPIPUBUFSIZE * 4);
			ulSize = BIG_ul_GetLengthFile(ulPos);
			ulPos += 4;
			BIG_Read(ulPos, (void *) p_TempBuffer, 16);

			/* Check IPU validity 0x6d757069 */
			if(p_TempBuffer[0] != 0x6d757069)
			{
				pst_Mpeg->ul_PSX2_IPUKey = (ULONG) - 1;
				return;
			}

			pst_Mpeg->ul_Size = p_TempBuffer[1];
			pst_Mpeg->ul_SizeY = p_TempBuffer[2] >> 16;
			pst_Mpeg->ul_SizeX = p_TempBuffer[2] & 0xffff;
			pst_Mpeg->ul_NbImages = p_TempBuffer[3];
			ulSize -= 16;
			ulPos += 16;
			ulSizeOfFrameCounter = 0;
			pst_Mpeg->ul_MaxBufSize = BaseRead = LastRead = 0;
			ulLastImgaKeyFinder = 0xffffffff;
			while(ulSize)
			{
				BIG_Read(ulPos, (void *) p_TempBuffer, TEMPIPUBUFSIZE * 4);
				ulSizeOfFrameCounter = lMin(ulSize, TEMPIPUBUFSIZE * 4);
				p_FinalFrameFinder = (UCHAR *) p_TempBuffer;
				while(ulSizeOfFrameCounter--)
				{
					ulLastImgaKeyFinder <<= 8;
					ulLastImgaKeyFinder |= (ULONG) (*p_FinalFrameFinder);
					if(ulLastImgaKeyFinder == 0x000001b0)
					{
						pst_Mpeg->ul_MaxBufSize = lMax(pst_Mpeg->ul_MaxBufSize, LastRead - BaseRead);
						BaseRead = LastRead = 0;
					}
					else
						LastRead++;

					p_FinalFrameFinder++;
				}

				ulSize -= lMin(ulSize, TEMPIPUBUFSIZE * 4);
				ulPos += lMin(ulSize, TEMPIPUBUFSIZE * 4);
			}

			MEM_Free(p_TempBuffer);
		}
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_Mpeg_SetBinkKey(TEX_tdst_Procedural *_pst_Pro, ULONG _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Mpeg	*pst_Mpeg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mpeg = (TEXPRO_tdst_Mpeg *) _pst_Pro->p_Data;

	pst_Mpeg->ul_BINK_Key = _ul_Key;
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG	ulPos, ulSize;
		ULONG	*p_TempBuffer;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ulPos = BIG_ul_SearchKeyToPos(pst_Mpeg->ul_BINK_Key);
		if(ulPos != (ULONG) - 1)
		{
			p_TempBuffer = (ULONG*)MEM_p_Alloc(TEMPIPUBUFSIZE * 4);
			ulSize = BIG_ul_GetLengthFile(ulPos);
			ulPos += 4;
			BIG_Read(ulPos, (void *) p_TempBuffer, 44);

			/* Check Bink validity 'iKIB'*/
			if(p_TempBuffer[0] != 'iKIB')
			{
				pst_Mpeg->ul_BINK_Key = (ULONG) - 1;
				return;
			}

			pst_Mpeg->ul_BinkSizeX = p_TempBuffer[5];
			pst_Mpeg->ul_BinkSizeY = p_TempBuffer[6];
			pst_Mpeg->ul_BinkBpp = 4;
			MEM_Free(p_TempBuffer);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_MpegSave(TEX_tdst_Procedural *_pst_Pro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Mpeg	*pst_Mpeg;
	LONG				l_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mpeg = (TEXPRO_tdst_Mpeg *) _pst_Pro->p_Data;

	l_Size = 32 + 16 + 4;
	SAV_Buffer(&l_Size, 4);
	SAV_Buffer(&pst_Mpeg->ul_Key, 4);
	SAV_Buffer(&pst_Mpeg->ul_PSX2_IPUKey, 4);
	SAV_Buffer(&pst_Mpeg->ul_MaxBufSize, 4);

	SAV_Buffer(&pst_Mpeg->ul_NbImages, 4);
	SAV_Buffer(&pst_Mpeg->ul_Size, 4);
	SAV_Buffer(&pst_Mpeg->ul_SizeY, 4);
	SAV_Buffer(&pst_Mpeg->ul_SizeX, 4);

	SAV_Buffer(&pst_Mpeg->ul_BINK_Key, 4);
	SAV_Buffer(&pst_Mpeg->ul_BinkSizeX, 4);
	SAV_Buffer(&pst_Mpeg->ul_BinkSizeY, 4);
	SAV_Buffer(&pst_Mpeg->ul_BinkBpp, 4);

	SAV_Buffer(&pst_Mpeg->ul_XMV_Key , 4);
}

#endif

/*$4
 ***********************************************************************************************************************
    Photo procedural texture functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern "C" int win32INO_i_FileGetSize( char * );
LONG TEXPRO_l_PhotoUpdate(TEX_tdst_Procedural *_pst_Pro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Photo	*pst_Photo;
	char				*pc_Buffer;
	LONG				i, l_Size;
#ifdef ACTIVE_EDITORS
	L_FILE				l_File;
	char				sz_Name[128];
#endif
	TEX_tdst_File_Desc	st_TexDesc, st_TexDesc2;
	ULONG				ul_Color, *pul_Pixel, *pul_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Photo = (TEXPRO_tdst_Photo *) _pst_Pro->p_Data;

    if ( pst_Photo->ul_CarteObj && (pst_Photo->ul_CarteObj <= 128 ) )
    {
        int i, j, k, y, x[2];

        y = pst_Photo->ul_CarteObj;
        pst_Photo->f_TimeLeft -= TIM_gf_dt;
        if (pst_Photo->f_TimeLeft <= 0)
            pst_Photo->ul_CarteObj = 0;
        else
            pst_Photo->ul_CarteObj = (ULONG) ((pst_Photo->f_TimeLeft / pst_Photo->f_Time) * 128.0f);
        if (pst_Photo->ul_CarteObj >= (ULONG) y) 
            return FALSE;

        while ( --y >= (int) pst_Photo->ul_CarteObj)
        {
            if ( (y < 2) || (y > 125) )
                continue;
        
            pul_Pixel = (ULONG *) _pst_Pro->pc_Buffer;
            pul_Pixel += y * 256 + 2;
            for (k = 0, i = 0; i < 4; i++)
            {
                j = (i+1) % 4;
                
                if (pst_Photo->Y[i] == y)
                {
                    if (pst_Photo->Y[j] == y)
                    {
                        x[0] = pst_Photo->X[i];
                        x[1] = pst_Photo->Y[i];
                        break;
                    }
                    else
                    {
                        x[k++] = pst_Photo->X[i]; 
                        continue;
                    }
                }
                else if (pst_Photo->Y[i] < y)
                {
                    if (pst_Photo->Y[j] <= y) 
                        continue;
                    x[k++] = pst_Photo->X[i] + ((pst_Photo->X[j] - pst_Photo->X[i]) * (y - pst_Photo->Y[i]))/(pst_Photo->Y[j] - pst_Photo->Y[i]);
                }
                else if (pst_Photo->Y[i] > y)
                {
                    if (pst_Photo->Y[j] >= y) 
                        continue;
                    x[k++] = pst_Photo->X[i] + ((pst_Photo->X[j] - pst_Photo->X[i]) * (y - pst_Photo->Y[i]))/(pst_Photo->Y[j] - pst_Photo->Y[i]);
                }
                if (k == 2) 
                    break;
            }
            if (k != 2)
            {
                for (i = 2; i < 254; i++)
                    *pul_Pixel++ = COLOR_ul_Blend( *pul_Pixel, 0, 0.5 );
            }
            else
            {
                if (x[0] > x[1]) 
                {
                    k = x[1];
                    x[1] = x[0];
                    x[0] = k;
                }
                if (x[0] > 254) x[0] = x[1] = 254;
                if (x[0] < 2) x[0] = 2;
                if (x[1] > 254) x[1] = 254;
                for ( i =2; i < x[0]; i++)
                {
                    *pul_Pixel = COLOR_ul_Blend( *pul_Pixel, 0, 0.5 );
                    pul_Pixel++;
                }
                if ( (y & 0x7) == 0 )
                    for ( ; i < x[1]; i++)
                    {
                        *pul_Pixel = 0xFF00FF00 | ( ~*pul_Pixel );
                        pul_Pixel++;
                    }
                else
                    for ( ; i < x[1]; i++)
                    {
                        *pul_Pixel = 0xFF000000 | ( ~*pul_Pixel );
                        if ( (i & 0x7) == 0)
                            *pul_Pixel |= 0xFF00FF00;
                        pul_Pixel++;
                }
                for ( ; i < 254; i++)
                {
                    *pul_Pixel = COLOR_ul_Blend( *pul_Pixel, 0, 0.5 );
                    pul_Pixel++;
                }

            }
        }
        return TRUE;
    }

	if(pst_Photo->c_UpdateNow == 0)
	{
		if(pst_Photo->c_Update || (pst_Photo->c_AskedPhoto != pst_Photo->c_Photo)) pst_Photo->c_UpdateNow = 1;
		return FALSE;
	}

	pst_Photo->c_UpdateNow = 0;
	pst_Photo->c_Update = 0;
	pst_Photo->c_Photo = pst_Photo->c_AskedPhoto;

	if(pst_Photo->c_Photo == -1) return FALSE;

	L_memset(&st_TexDesc, 0, sizeof(TEX_tdst_File_Desc));
	st_TexDesc.uw_DescFlags = TEX_Cuw_DF_Info | TEX_Cuw_DF_Content;

	if(pst_Photo->c_Photo > 100)
	{
#ifdef ACTIVE_EDITORS
		sprintf(sz_Name, "photo%03d.jpg", pst_Photo->c_Photo);

		l_Size = win32INO_i_FileGetSize(sz_Name);
		if(l_Size == -1) return FALSE;
		pc_Buffer = (char *) MEM_p_Alloc(l_Size);
		l_File = L_fopen(sz_Name, "r");
		L_fread(pc_Buffer, 1, l_Size, l_File);
		L_fclose(l_File);
		TEX_l_File_LoadJpeg(pc_Buffer, &st_TexDesc, l_Size);
		MEM_Free(pc_Buffer);
#endif
	}
	else
	{
		l_Size = 0;
		pc_Buffer = SAV_PhotoGet(pst_Photo->c_Photo, (int*)&l_Size);
		if(!pc_Buffer || !l_Size) return FALSE;
		TEX_l_File_LoadJpeg(pc_Buffer, &st_TexDesc, l_Size);
	}

	L_memcpy(&st_TexDesc2, &st_TexDesc, sizeof(TEX_tdst_File_Desc));
	st_TexDesc.p_Bitmap = _pst_Pro->pc_Buffer;

	st_TexDesc.uc_BPP = 32;
	if((st_TexDesc.uw_Width != _pst_Pro->uw_Width) || (st_TexDesc.uw_Height != _pst_Pro->uw_Height))
	{
		st_TexDesc.uw_Width = _pst_Pro->uw_Width;
		st_TexDesc.uw_Height = _pst_Pro->uw_Height;
		TEX_l_ConvertSize_24To32(&st_TexDesc2, &st_TexDesc);
	}
	else
	{
		TEX_l_ConvertPF(&st_TexDesc2, &st_TexDesc);
	}

	/* RGB ->
	BGR */
	pul_Pixel = (ULONG *) st_TexDesc.p_Bitmap;
	l_Size = 4L * _pst_Pro->uw_Width * _pst_Pro->uw_Height;
	pul_Last = (ULONG *) (((char *) pul_Pixel) + l_Size);
	while(pul_Pixel < pul_Last)
	{
		ul_Color = (*pul_Pixel & 0xFF) << 16;
		ul_Color |= (*pul_Pixel & 0xFF00FF00);
		ul_Color |= (*pul_Pixel & 0xFF0000) >> 16;
		*pul_Pixel++ = ul_Color;
	}

    // white border 
    {
        ULONG *P0, *P1, *P2, *P3;
        
        P0 = (ULONG *) _pst_Pro->pc_Buffer;
        P1 = P0 + _pst_Pro->uw_Width;
        P2 = P0 + ((_pst_Pro->uw_Height - 2) * _pst_Pro->uw_Width); 
        P3 = P0 + ((_pst_Pro->uw_Height - 1) * _pst_Pro->uw_Width); 
        for (i = 0; i < _pst_Pro->uw_Width; i++)
        {
            *P0++ = 0xFFFFFFFF;
            *P1++ = 0xFFFFFFFF;
            *P2++ = 0xFFFFFFFF;
            *P3++ = 0xFFFFFFFF;
        }

        P0 = (ULONG *) _pst_Pro->pc_Buffer;
        for (i = 0; i < _pst_Pro->uw_Height; i++)
        {
            *P0++ = 0xFFFFFFFF;
            *P0++ = 0xFFFFFFFF;
            P0 += (_pst_Pro->uw_Width-4);
            *P0++ = 0xFFFFFFFF;
            *P0++ = 0xFFFFFFFF;
        }
    }

	// grosse merde
#if 0
	{
		ULONG	*P0;
		int		i, j, k;
		char	FUCK[2][ 11 ][ 79 ] = 
		{
			{
				{ 1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
				{ 0,1,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
				{ 0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
				{ 0,1,0,0,1,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
				{ 0,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
				{ 0,1,0,0,1,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
				{ 0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
				{ 0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
				{ 1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
				{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
				{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}
			},
			{
				{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
				{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
				{ 1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,1,1,},
				{ 0,1,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,},
				{ 0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,},
				{ 0,1,0,0,1,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,1,0,0,1,0,0,},
				{ 0,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,1,0,1,0,0,0,},
				{ 0,1,0,0,1,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,1,1,1,0,0,0,},
				{ 0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,1,1,1,1,1,0,0,0,1,0,0,1,0,0,},
				{ 0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,},
				{ 1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,1,0,1,1,1,0,1,1,1,0,0,1,1,}
			}
		};

		
		P0 = (ULONG *) _pst_Pro->pc_Buffer;
		k = rand() & 127;
        P0 += 4 + k;
		k = rand() & 63;
		P0 += ((_pst_Pro->uw_Height - 8 - k) * _pst_Pro->uw_Width);
		k = rand() & 1;

		for (i = 0; i < 11; i++)
		{
			for (j = 0; j < 79; j++ )
			{
				if (FUCK[k][i][j])
					P0[ j ] = 0xFFFFFFFF;
			}
			P0 -= _pst_Pro->uw_Width;
		}
	}
#endif


        


    // photo
    if (pst_Photo->ul_CarteObj == 129) pst_Photo->ul_CarteObj--;

	TEX_File_FreeDescription(&st_TexDesc2);
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *TEXPRO_pv_PhotoLoad(char *_pc_Buffer, LONG _l_Size, ULONG _ul_Width, ULONG _ul_Height)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Photo	*pst_Photo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Photo = (TEXPRO_tdst_Photo *) MEM_p_Alloc(sizeof(TEXPRO_tdst_Photo));
	pst_Photo->c_AskedPhoto = (char) 0xFF;
	pst_Photo->c_Photo = (char) 0xFF;
	pst_Photo->c_Update = 0;
	pst_Photo->c_UpdateNow = 0;

    pst_Photo->ul_CarteObj = 0;

	if(_l_Size)
	{
		LOA_ReadLong_Ed(&_pc_Buffer, NULL); /* skip first 4 bytes, size */
		pst_Photo->c_Photo = LOA_ReadChar(&_pc_Buffer);
	}

	return pst_Photo;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_PhotoFree(TEX_tdst_Procedural *_pst_Pro)
{
	MEM_Free(_pst_Pro->p_Data);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_PhotoSave(TEX_tdst_Procedural *_pst_Pro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Photo	*pst_Photo;
	LONG				l_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Photo = (TEXPRO_tdst_Photo *) _pst_Pro->p_Data;

	l_Size = 8;
	SAV_Buffer(&l_Size, 4);
	SAV_Buffer(&pst_Photo->c_Photo, 1);
	SAV_Buffer(&pst_Photo->c_UpdateNow, 1);
	SAV_Buffer(&pst_Photo->c_UpdateNow, 1);
	SAV_Buffer(&pst_Photo->c_UpdateNow, 1);
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_Photo_SetMapGao( TEX_tdst_Procedural *_pst_Pro, ULONG _ul_Gao )
{
    TEXPRO_tdst_Photo   *pst_Photo; 
    CAM_tdst_Camera	    *pst_Cam;
    GEO_tdst_Object     *pst_Obj;
    MATH_tdst_Matrix    M;
    MATH_tdst_Vector    V[4];
    int                 i, j, X, Y, index[4];
    float               f_Cos, f_MaxCos;


    if ( (!GDI_gpst_CurDD) || (!_pst_Pro) || (!_ul_Gao)) return;

    if(GDI_gpst_CurDD->st_Device.l_Width < 512) return;
    if(GDI_gpst_CurDD->st_Device.l_Height < 256) return;
    
    pst_Photo = (TEXPRO_tdst_Photo *) _pst_Pro->p_Data;
    
    pst_Obj = (GEO_tdst_Object *) OBJ_p_GetGro( (OBJ_tdst_GameObject *) _ul_Gao );
	if ( !pst_Obj ) return;
    if (pst_Obj->st_Id.i->ul_Type != GRO_Geometric ) 
        return;
    if (pst_Obj->l_NbPoints != 4)
        return;

    /*
    for (i = 0; i < pst_Obj->l_NbElements; i++)
    {
        if (pst_Obj->dst_Element[i].l_NbTriangles == 2)
        {
            index[0] = 6 - pst_Obj->dst_Element[i].dst_Triangle[0].auw_Index[0] - pst_Obj->dst_Element[i].dst_Triangle[0].auw_Index[1] - pst_Obj->dst_Element[i].dst_Triangle[0].auw_Index[2];
            index[2] = 6 - pst_Obj->dst_Element[i].dst_Triangle[1].auw_Index[0] - pst_Obj->dst_Element[i].dst_Triangle[1].auw_Index[1] - pst_Obj->dst_Element[i].dst_Triangle[1].auw_Index[2];
            index[1] = (pst_Obj->dst_Element[i].dst_Triangle[0].auw_Index[0] != index[2]) ? pst_Obj->dst_Element[i].dst_Triangle[0].auw_Index[0] : pst_Obj->dst_Element[i].dst_Triangle[0].auw_Index[1];
            index[3] = 6 - index[0] - index[1] - index[2];
            break;
        }
    }
    */
    
    f_MaxCos = 0;
    index[0] = 0;
    for (i = 1; i <=3; i++)
    {
        MATH_SubVector( V, pst_Obj->dst_Point, pst_Obj->dst_Point + i );
        j = i + 1;
        if (j > 3) j -= 3;
        MATH_SubVector( V+1, pst_Obj->dst_Point + j, pst_Obj->dst_Point + (6-i-j) );
        f_Cos = MATH_f_DotProduct( V, V+1 );
        if (f_Cos < 0) 
        {
            f_Cos = -f_Cos;
            j = (6-i-j);
        }
        if (f_Cos > f_MaxCos)
        {
            f_MaxCos = f_Cos;
            index[1] = i;
            index[2] = 6-i-j;
            index[3] = j;
        }
    }

    X = (GDI_gpst_CurDD->st_Device.l_Width - 512) / 2;
    Y = (GDI_gpst_CurDD->st_Device.l_Height - 256) / 2;

    pst_Cam = &GDI_gpst_CurDD->st_Camera;
    MATH_MulMatrixMatrix( &M, OBJ_pst_GetAbsoluteMatrix( (OBJ_tdst_GameObject *) _ul_Gao), &pst_Cam->st_InverseMatrix );
    pst_Cam->pst_ObjectToCameraMatrix = &M;
    for (i = 0; i < 4; i++)
    {
		SOFT_TransformAndProject(&V[i], &pst_Obj->dst_Point[index[i]], 1, pst_Cam);
        pst_Photo->X[i] = (SHORT) ((V[i].x - X) / 2);
        pst_Photo->Y[i] = (SHORT) ((V[i].y - Y) / 2);
    }
    pst_Photo->ul_CarteObj = 0x10000000;
    pst_Photo->f_Time = pst_Photo->f_TimeLeft = 1;
}


/*$4
 ***********************************************************************************************************************
    plasma procedural texture functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG TEXPRO_l_PlasmaUpdate(TEX_tdst_Procedural *_pst_Pro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Plasma	*pst_Plasma;
	LONG				x1, y1, x2, y2, x3, y3, Z;
	LONG				W, H;
	ULONG				*S;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Plasma = (TEXPRO_tdst_Plasma *) _pst_Pro->p_Data;

	W = (LONG) pst_Plasma->aul_Palette[0];
	for(Z = 0; Z < 255; Z++) pst_Plasma->aul_Palette[Z] = pst_Plasma->aul_Palette[Z + 1];
	pst_Plasma->aul_Palette[255] = (W & 0xFF000000) + ((W & 0xFFFF) << 8) + ((W & 0xFF0000) >> 16);

	W = (LONG) _pst_Pro->uw_Width;
	H = (LONG) _pst_Pro->uw_Height;
	S = (ULONG *) _pst_Pro->pc_Buffer;

	x1 = (LONG) ((W >> 1) * (1.0f + sin(pst_Plasma->d1 * Cf_PiBy180)));
	pst_Plasma->d1 += 1.666f;
	if(pst_Plasma->d1 >= 360) pst_Plasma->d1 -= 360;

	y1 = (LONG) ((H >> 1) * (1.0f + sin(pst_Plasma->d2 * Cf_PiBy180)));
	pst_Plasma->d2 += 1.88f;
	if(pst_Plasma->d2 >= 360) pst_Plasma->d2 -= 360;

	for(x2 = 0; x2 < W; x2++)
	{
		x3 = x1 + x2;
		if(x3 >= W) x3 -= W;
		for(y2 = 0; y2 < H; y2++)
		{
			y3 = y1 + y2;
			if(y3 >= H) y3 -= H;
			Z = (pst_Plasma->puc_Surface[0][W * y2 + x2] + pst_Plasma->puc_Surface[1][W * y3 + x3]) >> 1;
			*S++ = pst_Plasma->aul_Palette[Z];
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_PlasmaDisrupt(TEX_tdst_Procedural *_pst_Pro)
{
	return;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *TEXPRO_pv_PlasmaLoad(char *_pc_Buffer, LONG _l_Size, ULONG _ul_Width, ULONG _ul_Height)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Plasma	*pst_Plasma;
	UCHAR				*b1, *b2;
	int					c1, c2, c3, c4;
	int					i, s, x1, x2, x3, y1, y2, y3, W, H;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Plasma = (TEXPRO_tdst_Plasma *) MEM_p_Alloc(sizeof(TEXPRO_tdst_Plasma));
	pst_Plasma->puc_Surface[0] = (UCHAR *) MEM_p_Alloc(_ul_Width * _ul_Height);
	L_memset(pst_Plasma->puc_Surface[0], 0, _ul_Width * _ul_Height);
	pst_Plasma->puc_Surface[1] = (UCHAR *) MEM_p_Alloc(_ul_Width * _ul_Height);
	L_memset(pst_Plasma->puc_Surface[1], 0, _ul_Width * _ul_Height);

	pst_Plasma->d1 = fRand(0, 360);
	pst_Plasma->d2 = fRand(0, 360);

	W = (int) _ul_Width;
	H = (int) _ul_Height;

	b1 = pst_Plasma->puc_Surface[0];
	b2 = pst_Plasma->puc_Surface[1];
	for(i = 0; i < 2; i++)
	{
		for(x1 = 0; x1 < W; x1++)
			for(y1 = 0; y1 < H; y1++) b1[W * y1 + x1] = rand() & 0xFF;
		s = 32;
		while(s)
		{
			for(x1 = 0; x1 < W; x1 += s)
			{
				x2 = s + x1;
				if(x2 == W) x2 = 0;
				x3 = s / 2 + x1;
				for(y1 = 0; y1 < H; y1 += s)
				{
					y2 = s + y1;
					c1 = b1[W * y1 + x1];
					c2 = b1[W * y1 + x2];
					if(y2 == H) y2 = 0;
					c3 = b1[W * y2 + x1];
					c4 = b1[W * y2 + x2];
					y3 = s / 2 + y1;
					b1[W * y3 + x2] = (c2 + c4) / 2;
					b1[W * y3 + x3] = (c1 + c2 + c3 + c4) / 4;
					b1[W * y2 + x3] = (c3 + c4) / 2;
				}
			}

			s >>= 1;
		}

		if(i) memcpy(b2, b1, H * W);
	}

	for(i = 0; i < 64; i++)
	{
		pst_Plasma->aul_Palette[i] = 0xFF000000;
		pst_Plasma->aul_Palette[64 + i] = 0xFF000000 | (i << 2);
		pst_Plasma->aul_Palette[128 + i] = 0xFF000000 | ((64 - i) << 2);
		pst_Plasma->aul_Palette[192 + i] = 0xFF000000;
	}

	return pst_Plasma;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXPRO_PlasmaFree(TEX_tdst_Procedural *_pst_Pro)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXPRO_tdst_Plasma	*pst_Plasma;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Plasma = (TEXPRO_tdst_Plasma *) _pst_Pro->p_Data;

	MEM_Free(pst_Plasma->puc_Surface[0]);
	MEM_Free(pst_Plasma->puc_Surface[1]);
	MEM_Free(pst_Plasma);
}

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Procedural_Init(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_ProceduralInterface	*i;
	int								index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i = &TEX_gast_ProceduralInterface[TEXPRO_Unknow];
	i->ul_Type = TEXPRO_Unknow;
	i->pfnl_Update = TEXPRO_l_UnknowUpdate;
	i->pfnpv_Load = TEXPRO_pv_UnknowLoad;
	i->pfnv_Disrupt = TEXPRO_UnknowDisrupt;
	i->pfnv_Free = TEXPRO_UnknowFree;
#ifdef ACTIVE_EDITORS
	i->pfnv_Save = TEXPRO_UnknowSave;
#endif
	i = &TEX_gast_ProceduralInterface[TEXPRO_Water];
	i->ul_Type = TEXPRO_Water;
	i->pfnl_Update = TEXPRO_l_WaterUpdate;
	i->pfnpv_Load = TEXPRO_pv_WaterLoad;
	i->pfnv_Disrupt = TEXPRO_WaterDisrupt;
	i->pfnv_Free = TEXPRO_WaterFree;
#ifdef ACTIVE_EDITORS
	i->pfnv_Save = TEXPRO_UnknowSave;
#endif
	i = &TEX_gast_ProceduralInterface[TEXPRO_Fire];
	i->ul_Type = TEXPRO_Fire;
	i->pfnl_Update = TEXPRO_l_FireUpdate;
	i->pfnpv_Load = TEXPRO_pv_FireLoad;
	i->pfnv_Disrupt = TEXPRO_FireDisrupt;
	i->pfnv_Free = TEXPRO_FireFree;
#ifdef ACTIVE_EDITORS
	i->pfnv_Save = TEXPRO_UnknowSave;
#endif

    i = &TEX_gast_ProceduralInterface[TEXPRO_Mpeg];
#if defined(_GAMECUBE)
	i->ul_Type = TEXPRO_Mpeg;
	i->pfnl_Update = TEXPRO_l_MpegUpdate;
	i->pfnpv_Load = TEXPRO_pv_MpegLoad;
	i->pfnv_Disrupt = TEXPRO_UnknowDisrupt;
	i->pfnv_Free = TEXPRO_MpegFree;
#else
	i = &TEX_gast_ProceduralInterface[TEXPRO_Mpeg];
	i->ul_Type = TEXPRO_Mpeg;
	i->pfnl_Update = TEXPRO_l_MpegUpdate;
	i->pfnpv_Load = TEXPRO_pv_MpegLoad;
	i->pfnv_Disrupt = TEXPRO_UnknowDisrupt;
	i->pfnv_Free = TEXPRO_MpegFree;
#endif
#ifdef ACTIVE_EDITORS
	i->pfnv_Save = TEXPRO_MpegSave;
#endif

	i = &TEX_gast_ProceduralInterface[TEXPRO_Photo];
	i->ul_Type = TEXPRO_Photo;
	i->pfnl_Update = TEXPRO_l_PhotoUpdate;
	i->pfnpv_Load = TEXPRO_pv_PhotoLoad;
	i->pfnv_Disrupt = TEXPRO_UnknowDisrupt;
	i->pfnv_Free = TEXPRO_PhotoFree;
#ifdef ACTIVE_EDITORS
	i->pfnv_Save = TEXPRO_PhotoSave;
#endif
	i = &TEX_gast_ProceduralInterface[TEXPRO_Plasma];
	i->ul_Type = TEXPRO_Plasma;
	i->pfnl_Update = TEXPRO_l_PlasmaUpdate;
	i->pfnpv_Load = TEXPRO_pv_PlasmaLoad;
	i->pfnv_Disrupt = TEXPRO_PlasmaDisrupt;
	i->pfnv_Free = TEXPRO_PlasmaFree;
#ifdef ACTIVE_EDITORS
	i->pfnv_Save = TEXPRO_UnknowSave;
#endif
	/* cosinus table */
	for(index = 0; index < 256; index++) TEXPRO_Cos[index] = (int) (30 * (fCos(index * Cf_Pi / 64)));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Procedural_Reinit(GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	TEX_tdst_Procedural *pst_Pro;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(TEX_gi_NbProcedural)
	{
		pst_Pro = TEX_gdst_Procedural;

		for(i = 0; i < TEX_gi_NbProcedural; i++, pst_Pro++)
		{
			pst_Pro->i->pfnv_Free(pst_Pro);
			if
			(
				(_pst_DD->st_TexManager.ul_Flags & TEX_Manager_StoreInterfaceTex)
			&&	(pst_Pro->uw_Flags & TEXPRO_Interface)
			) continue;

            if (pst_Pro->pc_Buffer)
			    MEM_FreeAlign(pst_Pro->pc_Buffer);
#ifdef ACTIVE_EDITORS
			LINK_DelRegisterPointer(pst_Pro);
			LOA_DeleteAddress(pst_Pro);
#endif
		}

		TEX_gi_NbProcedural = 0;
		MEM_Free(TEX_gdst_Procedural);
		TEX_gdst_Procedural = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Procedural_Add(SHORT _w_Index, int _i_Type, USHORT w_Flags, void *_p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Procedural *pst_Pro;
	TEX_tdst_Data		*pst_Tex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!TEX_gi_NbProcedural) 
	{
		TEX_gdst_Procedural = (TEX_tdst_Procedural *) MEM_p_Alloc(16 * sizeof(TEX_tdst_Procedural));
	}

	if(TEX_gi_NbProcedural >= 16)
	{
		ERR_X_Warning(0, "Too many procedural textures", NULL);
		return;
	}

	pst_Pro = &TEX_gdst_Procedural[TEX_gi_NbProcedural++];
	pst_Tex = TEX_gst_GlobalList.dst_Texture + _w_Index;
	pst_Pro->w_Index = _w_Index;
	pst_Pro->uw_Flags = w_Flags;
	pst_Pro->uw_Height = (USHORT) pst_Tex->w_Height;
	pst_Pro->uw_Width = (USHORT) pst_Tex->w_Width;
	pst_Pro->pc_Buffer = NULL;
#ifdef ACTIVE_EDITORS
	pst_Pro->uw_WantedHeight = (USHORT) pst_Tex->w_Height;
	pst_Pro->uw_WantedWidth = (USHORT) pst_Tex->w_Width;
	pst_Pro->ul_Type = _i_Type;
#endif
	pst_Pro->i = &TEX_gast_ProceduralInterface[_i_Type];
	pst_Pro->p_Data = _p_Data;

#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		BIG_INDEX	ul_Fat;
		char		asz_Path[BIG_C_MaxLenPath];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_Fat = BIG_ul_SearchKeyToFat(pst_Tex->ul_Key);
		BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
		LINK_RegisterPointer(pst_Pro, LINK_C_TEX_Procedural, BIG_NameFile(ul_Fat), asz_Path);
		LINK_UpdatePointers();
		LOA_AddAddress(ul_Fat, pst_Pro);
	}

#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Procedural_AfterLoad(GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Procedural *pst_Pro;
	TEX_tdst_Data		*pst_Tex;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Pro = TEX_gdst_Procedural;
	for(i = 0; i < TEX_gi_NbProcedural; i++, pst_Pro++)
	{
		pst_Tex = TEX_gst_GlobalList.dst_Texture + pst_Pro->w_Index;

		pst_Pro->uw_Height = (USHORT) pst_Tex->w_Height;
		pst_Pro->uw_Width = (USHORT) pst_Tex->w_Width;

		if((_pst_DD->st_TexManager.ul_Flags & TEX_Manager_StoreInterfaceTex) && (pst_Pro->uw_Flags & TEXPRO_Interface))
		{
		
			pst_Pro->pc_Buffer = (char *) GDI_gpst_CurDD->st_GDI.pfnl_Request
				(
					GDI_Cul_Request_GetInterfaceTexBuffer,
					pst_Tex->ul_Key
				);
		}

		if(pst_Pro->pc_Buffer == NULL)
        {
			pst_Pro->pc_Buffer = (char*)MEM_p_AllocAlign(4L * pst_Tex->w_Height * pst_Tex->w_Width, 16);
            L_memset( pst_Pro->pc_Buffer, 0, 4L * pst_Tex->w_Height * pst_Tex->w_Width );
        }
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
TEX_tdst_Procedural *TEX_pst_Procedural_Get(SHORT _w_Index)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < TEX_gi_NbProcedural; i++)
	{
		if(TEX_gdst_Procedural[i].w_Index == _w_Index) return &TEX_gdst_Procedural[i];
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Procedural_Update(GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_Request_ReloadTextureParams	st_RTP;
	TEX_tdst_Procedural						*pst_Pro;
	int										i;
    static ULONG                            TEXPRO_sul_UpdateCounter = 0xFFFFFFFF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GDI_gpst_CurDD = _pst_DD;
    if (TEXPRO_sul_UpdateCounter == _pst_DD->ul_RenderingCounter)
        return;
    TEXPRO_sul_UpdateCounter = _pst_DD->ul_RenderingCounter;

	pst_Pro = TEX_gdst_Procedural;
	for(i = 0; i < TEX_gi_NbProcedural; i++, pst_Pro++)
	{

		pst_Pro->i->pfnv_Disrupt(pst_Pro);

		if(pst_Pro->i->pfnl_Update(pst_Pro) || ( !(pst_Pro->uw_Flags & TEXPRO_UpdateOnce) ) )
		{

            pst_Pro->uw_Flags |= TEXPRO_UpdateOnce;
			if
			(
				(_pst_DD->st_TexManager.ul_Flags & TEX_Manager_StoreInterfaceTex)
			&&	(pst_Pro->uw_Flags & TEXPRO_Interface)
			) continue;

			st_RTP.w_Texture = pst_Pro->w_Index;
			st_RTP.pc_Data = pst_Pro->pc_Buffer;
			st_RTP.IsAlreadySwizzled = 0;
#ifdef _GAMECUBE
			if (!(pst_Pro->uw_Flags & TEXPRO_UpdatedFromACopy))
			if (pst_Pro->i->ul_Type == TEXPRO_Mpeg) st_RTP.IsAlreadySwizzled = 24;
#endif			

			_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_ReloadTexture, (ULONG) & st_RTP);

		}

	}

}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Procedural_Save(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Procedural		*pst_Pro;
	int						i;
	char					sz_Path[BIG_C_MaxLenPath];
	ULONG					ul_Value;
	LONG					l_Length;
	TEX_tdst_File_Params	st_Params;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Pro = TEX_gdst_Procedural;
	for(i = 0; i < TEX_gi_NbProcedural; i++, pst_Pro++)
	{
		ul_Value = LOA_ul_SearchKeyWithAddress((ULONG) pst_Pro);
		if(ul_Value == BIG_C_InvalidKey) continue;
		ul_Value = BIG_ul_SearchKeyToFat(ul_Value);
		if(ul_Value == BIG_C_InvalidIndex) continue;

		BIG_ComputeFullName(BIG_ParentFile(ul_Value), sz_Path);
		SAV_Begin(sz_Path, BIG_NameFile(ul_Value));

		l_Length = 12;
		SAV_Buffer(&l_Length, 4);
		SAV_Buffer(&pst_Pro->uw_Flags, 2);
		SAV_Buffer(&pst_Pro->uw_Width, 2);
		SAV_Buffer(&pst_Pro->uw_Height, 2);
		SAV_Buffer(&pst_Pro->i->ul_Type, 2);

		pst_Pro->i->pfnv_Save(pst_Pro);

		TEX_File_InitParams(&st_Params);
		st_Params.uw_Flags = TEX_FP_QualityVeryHigh;
		st_Params.uc_Type = TEX_FP_ProFile;
		st_Params.uw_Height = pst_Pro->uw_Height;
		st_Params.uw_Width = pst_Pro->uw_Width;

		SAV_Buffer(&st_Params, sizeof(TEX_tdst_File_Params));

		SAV_ul_End();
	}
}

#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEX_Procedural_UpdateWithAnother( TEX_tdst_Procedural *_pst_Tgt, TEX_tdst_Procedural *_pst_Src )
{
	unsigned short W, H;
	W = _pst_Src->uw_Width;
	H = _pst_Src->uw_Height;

	while ( W > _pst_Tgt->uw_Width )
	{
		TEX_Compress_Xo2( (ULONG *) _pst_Src->pc_Buffer, (ULONG *) _pst_Src->pc_Buffer, W, H );
		W >>= 1;
	}
	while ( H > _pst_Tgt->uw_Height )
	{
		TEX_Compress_Yo2( (ULONG *) _pst_Src->pc_Buffer, (ULONG *) _pst_Src->pc_Buffer, W, H );
		H >>= 1;
	}

	L_memcpy( _pst_Tgt->pc_Buffer, _pst_Src->pc_Buffer, (4L * W * H) );
	_pst_Tgt->uw_Flags &= ~TEXPRO_UpdateOnce;
	_pst_Tgt->uw_Flags |= TEXPRO_UpdatedFromACopy;
}


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
