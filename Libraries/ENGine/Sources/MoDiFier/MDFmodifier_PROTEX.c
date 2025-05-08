/*$T MDFmodifier_GEO.c GC! 1.081 06/09/00 09:00:51 */


/*$6
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/


#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_PROTEX.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "SOFT/SOFTstruct.h"
#include "SOFT/SOFTzlist.h"
#include "MATerial/MATSingle.h"
#include "MATerial/MATmulti.h"
#include "MATerial/MATSprite.h"
#ifdef PSX2_TARGET
#include "GS_PS2/Gsp.h"
#endif

#include "BASe/BENch/BENch.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"

#ifdef PSX2_TARGET
#include "MainPsx2/Sources/PSX2debug.h"
#define _ReadLong(_a)   ReadLong((char*)_a)
#define _ReadFloat(_a)   ReadFloat((char*)_a)
#else
#define _ReadLong(_a)    *(ULONG*)_a
#define _ReadFloat(_a)   *(float*)_a
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifndef PSX2_TARGET
ULONG	Water_Palette[256];
#endif

	
PROTEX_tdst_Modifier *p_FirstActiveProtex = NULL;

void PROTEX_DrawBorders(PROTEX_tdst_Modifier *p_PT)
{
	USHORT *S1;
	ULONG LineCounter;
	UCHAR	*P_AllDotNumbers,*P_AllDotNumbersLast,*pDots;
	if (p_PT->P_AllDotNumbers == NULL) return;
	if (p_PT->p_Surface0 == NULL) return;
	if (p_PT->ulNumberOfHLines != p_PT->H) return;
	S1 = (USHORT*)p_PT->p_Surface0;
   	P_AllDotNumbers = p_PT->P_AllDotNumbers;
	P_AllDotNumbersLast = p_PT->P_AllDotNumbers + p_PT->H;
	pDots = p_PT->p_AllDot;
	while(P_AllDotNumbers < P_AllDotNumbersLast)
	{
		LineCounter = *(P_AllDotNumbers++);
		 while (LineCounter--) S1 [*(pDots++)] = 0;
		S1+= p_PT->W;
	}
}

#ifdef ACTIVE_EDITORS
void PROTEX_CompressBorder(PROTEX_tdst_Modifier *p_PT)
{
	USHORT *S1;
	ULONG LineCounter,XCounter,DotNumbers;
	UCHAR	*P_AllDotNumbers,*pDots;
	/* Count NumberOfDots*/
	LineCounter = p_PT->W*p_PT->H;
	DotNumbers = 0;
	S1 = (USHORT*)p_PT->p_Surface1;
	while(LineCounter--)
	{
		DotNumbers += (*(S1++) ^ 1) & 1;
	}
	if (!DotNumbers)
	{
		if (p_PT->p_AllDot)			MEM_Free(p_PT->p_AllDot);
		if (p_PT->P_AllDotNumbers)	MEM_Free(p_PT->P_AllDotNumbers);
		p_PT->ulNumberOfDots = 0;
		p_PT->ulNumberOfHLines = 0;
		p_PT->p_AllDot = NULL;
		p_PT->P_AllDotNumbers = NULL;
		return;
	}
	if (p_PT->ulNumberOfDots < DotNumbers)
	{
		if (p_PT->p_AllDot)			MEM_Free(p_PT->p_AllDot);
		p_PT->p_AllDot			=	(UCHAR*)MEM_p_Alloc(DotNumbers);
		p_PT->ulNumberOfDots	=	DotNumbers;
	}
	if (p_PT->ulNumberOfHLines != p_PT->H) 
	{
		if (p_PT->P_AllDotNumbers)	MEM_Free(p_PT->P_AllDotNumbers);
		p_PT->P_AllDotNumbers	=	(UCHAR*)MEM_p_Alloc(p_PT->H);
		p_PT->ulNumberOfHLines	=	p_PT->H;
	}
	S1 = (USHORT*)p_PT->p_Surface1;
	LineCounter = p_PT->H;
	P_AllDotNumbers = p_PT->P_AllDotNumbers;
	pDots = p_PT->p_AllDot;
	while(LineCounter--)
	{
		*P_AllDotNumbers = 0;
		for (XCounter = 0 ; XCounter < p_PT->W ; XCounter++)
		{	
			if (((*(S1++)) ^ 1) & 1)
			{
   				(*P_AllDotNumbers)++;
				*(pDots++) = (unsigned char)XCounter;
			}
		}
		P_AllDotNumbers++;
	}
}

void PROTEX_WaterBresenheim(ULONG ulLineX1Y1X2Y2 , USHORT *p_Surface , ULONG PitchPo2 , USHORT Color)
{
	LONG X1,X2,Y1,Y2 , DX,DY , BrezInc , LineCounter , Lineinc2;
	
	X1 = (LONG)(UCHAR)(ulLineX1Y1X2Y2 >> 24);
	X2 = (LONG)(UCHAR)(ulLineX1Y1X2Y2 >> 8);
	Y1 = (LONG)(UCHAR)(ulLineX1Y1X2Y2 >> 16);
	Y2 = (LONG)(UCHAR)(ulLineX1Y1X2Y2 >> 0);
	DX = lMax(X1,X2) - lMin(X1,X2);
	DY = lMax(Y1,Y2) - lMin(Y1,Y2);
	BrezInc = 0;
	if (DX > DY)
	{
		if (X1 > X2)
		{
			LONG SWP;
			SWP = X1;X1 = X2;X2 = SWP;
			SWP = Y1;Y1 = Y2;Y2 = SWP;
		}
		if (Y2 > Y1)
			Lineinc2 = 1 <<PitchPo2;
		else
			Lineinc2 = -1 <<PitchPo2;
		p_Surface += X1;
		p_Surface += Y1 << PitchPo2;
		LineCounter = DX;
		while (LineCounter--)
		{
			*(p_Surface ++) &= ~1;
			BrezInc += DY;
			if (BrezInc > DX)
			{
				p_Surface+=Lineinc2;
				BrezInc -= DX;
			}
		}
	} else
	{
		if (Y1 > Y2)
		{
			LONG SWP;
			SWP = X1;X1 = X2;X2 = SWP;
			SWP = Y1;Y1 = Y2;Y2 = SWP;
		}
		if (X2 > X1)
			Lineinc2 = 1;
		else
			Lineinc2 = -1;
		p_Surface += X1;
		p_Surface += Y1 << PitchPo2;
		LineCounter = DY;
		while (LineCounter--)
		{
			*(p_Surface ) &= ~1;
			p_Surface += 1<<PitchPo2;
			BrezInc += DX;
			if (BrezInc > DY)
			{
				p_Surface+=Lineinc2;
				BrezInc -= DY;
			}
		}
	}
}
#endif
#ifdef PSX2_TARGET
void PROTEX_l_WaterUpdate_ASM(USHORT *p_T ,  USHORT *p_Tp1 , ULONG Pitch , ULONG Counter)
{
/*
;Differential equation is:  u  = a²( u  + u  )
;                             tt       xx   yy
;
;Where a² = tension * gravity / surface_density.
;
;Aproximating second derivatives by central differences:
;
;  [ u(t+1)-2u(t)+u(t-1) ] / Ùt² = a² (u(x+1)+u(x-1)+u(y+1)+u(y-1)-4u) / h²
;
;(where Ùt = time step, h=Ùx=Ùy = mesh resolution
;
;From where u(t+1) may be calculated as:
;
;                   |0 1 0|
; u(t+1) = a²Ùt²/h² |1 0 1|u - u(t-1) + (2-4a²Ùt²/h²)u
;                   |0 1 0|
;
;When a²Ùt²/h² = ½ last term vanishes, giving:
;
;           |0 1 0|
;u(t+1) = ½ |1 0 1| u - u(t-1)
;           |0 1 0|
;
;Computing on PS2 is more easy and faster if made 8 by 8, but it need a special order of points:
;
;					Y = Y;
;					X = (X & ~63) | ((X & 0x07) << 3) | ((X & 0x38) >> 3);
;
;alow us to have this QWORD + 1 = X + 1 && QWORD - 1 = X - 1
;
;					<==== QWORD = 8 shorts =====>
;
;				+--	56  48	40	32	24	16	8	0 <----- MEMORY
;				|
;				+---------------------------------------+
;														|
;				+--	57	49	41	33	25	17	9	1 <-----+
;				|
;				+---------------------------------------+
;														|
;										<...10	2 <-----+
;
;
;					T8:	55	47	39	31	23	15	7	(-1) <--+ (more complicated! avoid this is why I use this de-interlaced format)
;															|
;					T0:	56  48	40	32	24	16	8	0	 ---|---+
;					T1:	57	49	41	33	25	17	9	1		|	|
;					T2:	58	50	42	34	26	18	10	2		|	|
;					T3:	59	51	43	35	27	19	11	3		|	|
;					T4:	60	52	44	36	28	20	12	4		|	|
;					T5:	61	53	45	37	29	21	13	5		|	|
;					T6:	62	54	46	38	30	22	14	6		|	|
;					T7:	63  55	47	39	31	23	15	7	 ---+	|
;																|
;					T9:	(64)56	48	40	32	24	16	8	 <------+ (QFRSV T9 , (short) next , T0)
;
*/
#if 0
	
	while (Counter--)
	{
		LONG Counter64 , Counter64R;
		for (Counter64 = 0 ; Counter64 < 64 ; Counter64++)
		{
			SHORT VI,VIP1 , XN;
			Counter64R = (Counter64 & ~0x3f) | ((Counter64 & 0x07) << 3) | ((Counter64 & 0x38) >> 3);
			VI 	= p_T[Counter64R - (Pitch)];
			VIP1= p_T[Counter64R + (Pitch)];
			VI += VIP1;
			
			XN = Counter64+1;
			XN = (XN & ~0x3f) | ((XN & 0x07) << 3) | ((XN & 0x38) >> 3);
			VIP1= p_T[XN];
			VI += VIP1;
			
			if (Counter64 == 0)
				VIP1= p_T[-1];
			else {
				XN = Counter64-1;
				XN = (XN & ~0x3f) | ((XN & 0x07) << 3) | ((XN & 0x38) >> 3);
				VIP1= p_T[XN];
			}
			VI += VIP1;
			
			VI >>= 1;
			VIP1= p_Tp1[Counter64R];
			VI -= VIP1 ;
			VI -= VI>>8;
			
			VI = lMax(VI,-32<<8);
			VI = lMin(VI,32<<8);
			
			p_Tp1[Counter64R] = VI;
		}
		p_T += Pitch;
		p_Tp1 += Pitch;
	}
#else // do 64 by 64
asm __volatile__ ("
		.set noreorder
		
	mtsah 	zero, 1
	li		v0,0x2000
	ppacw 	v0,v0,v0
	pcpyh 	v0,v0
	psubh 	v1,zero,v0
	add 	a2,a2,a2
LineLoop:
	lhu		t6 , -2(a0)
	lq		t7,0x70(a0)
	lq		t0,0x00(a0)
	lq		t1,0x10(a0)
	lq		t2,0x20(a0)
	psrlw	t8 , t7 , 16
	lq		t3,0x30(a0)
	ppach	t8 , t7 , t8
	lq		t4,0x40(a0)
	dsll	t8 , t8 , 16
	lq		t5,0x50(a0)
	or 		t8 , t8 , t6/*(short)previous*/
	lhu		t6 , 0x80(a0)
	pinth	t8 , t8 , t8
	QFSRV 	t9 , t6/*(short)next*/ , t0
	lq		t6,0x60(a0)
	sub		a0,a0,a2
	
	paddsh	s0,t8,t1
	paddsh	s1,t0,t2
	paddsh	s2,t1,t3
	paddsh	s3,t2,t4
	paddsh	s4,t3,t5
	paddsh	s5,t4,t6
	paddsh	s6,t5,t7
	paddsh	s7,t6,t9
	
/*	psllh	t0,t0,2
	psllh	t1,t1,2
	psllh	t2,t2,2
	psllh	t3,t3,2
	psllh	t4,t4,2
	psllh	t5,t5,2
	psllh	t6,t6,2
	psllh	t7,t7,2
	paddsh	s0,s0,t0
	paddsh	s1,s1,t1
	paddsh	s2,s2,t2
	paddsh	s3,s3,t3
	paddsh	s4,s4,t4
	paddsh	s5,s5,t5
	paddsh	s6,s6,t6
	paddsh	s7,s7,t7
	
	//*/
/*;									---> 20
;2:load 	nex line*/
	lq		t0,0x00(a0)
	lq		t1,0x10(a0)
	lq		t2,0x20(a0)
	lq		t3,0x30(a0)
	lq		t4,0x40(a0)
	lq		t5,0x50(a0)
	lq		t6,0x60(a0)
	lq		t7,0x70(a0)
	add		a0,a0,a2
	add		a0,a0,a2
	paddsh	s0,s0,t0
	lw		zero,0x00(a1)
	paddsh	s1,s1,t1
/*;									--->	16
;3:load 	previous line*/
	lq		t0,0x00(a0)
	paddsh	s2,s2,t2
	lq		t1,0x10(a0)
	paddsh	s3,s3,t3
	lq		t2,0x20(a0)
	paddsh	s4,s4,t4
	lq		t3,0x30(a0)
	paddsh	s5,s5,t5
	lq		t4,0x40(a0)
	paddsh	s6,s6,t6
	lq		t5,0x50(a0)
	paddsh	s7,s7,t7
	lq		t6,0x60(a0)
	lq		t7,0x70(a0)
	paddsh	s0,s0,t0
	paddsh	s1,s1,t1
	paddsh	s2,s2,t2
	lw		zero,0x40(a1)
	paddsh	s3,s3,t3
	paddsh	s4,s4,t4
	paddsh	s5,s5,t5
	paddsh	s6,s6,t6
	paddsh	s7,s7,t7
	add		a0,a0,a2
	psrah	s0,s0,1
	lq		t0,0x00(a1)
	psrah	s1,s1,1
	lq		t1,0x10(a1)
	psrah	s2,s2,1
	lq		t2,0x20(a1)
	psrah	s3,s3,1
	lq		t3,0x30(a1)
	psrah	s4,s4,1
	lq		t4,0x40(a1)
	psrah	s5,s5,1
	lq		t5,0x50(a1)
	psrah	s6,s6,1
	lq		t6,0x60(a1)
	psrah	s7,s7,1
/*;									--->	24
;4:load 	t+1 line*/
	lq		t7,0x70(a1)
	psubsh	s0,s0,t0
	lw		zero,-4(a0)
	psubsh	s1,s1,t1
	psubsh	s2,s2,t2
	psubsh	s3,s3,t3
	psubsh	s4,s4,t4
	psubsh	s5,s5,t5
	psubsh	s6,s6,t6
	psubsh	s7,s7,t7
	lw		zero,0x80(a0)
	psrah	t0,s0,10
	psrah	t1,s1,10
	psrah	t2,s2,10
	psrah	t3,s3,10
	psrah	t4,s4,10
	psrah	t5,s5,10
	psrah	t6,s6,10
	psrah	t7,s7,10
	psubsh	s0,s0,t0
	psubsh	s1,s1,t1
	lw		zero,0x00(a0)
	psubsh	s2,s2,t2
	psubsh	s3,s3,t3
	psubsh	s4,s4,t4
	psubsh	s5,s5,t5
	psubsh	s6,s6,t6
	psubsh	s7,s7,t7
/*;									--->	16
;6:MAX MIN			*/						
	pmaxh	s0,s0,v1
	pmaxh	s1,s1,v1
	pmaxh	s2,s2,v1
	lw		zero,0x40(a0)
	pmaxh	s3,s3,v1
	pmaxh	s4,s4,v1
	pmaxh	s5,s5,v1
	pmaxh	s6,s6,v1
	pmaxh	s7,s7,v1
	pminh	s0,s0,v0
	pminh	s1,s1,v0
	pminh	s2,s2,v0
/*;									--->	16
;7:save 	t+1 line*/
	sq		s0,0x00(a1)
	pminh	s3,s3,v0
	sq		s1,0x10(a1)
	pminh	s4,s4,v0
	sq		s2,0x20(a1)
	pminh	s5,s5,v0
	sq		s3,0x30(a1)
	pminh	s6,s6,v0
	sq		s4,0x40(a1)
	pminh	s7,s7,v0
	sq		s5,0x50(a1)
	addi	a3,a3,-1
	sq		s6,0x60(a1)
	sub		a0,a0,a2
	sq		s7,0x70(a1)
	bne		a3,zero,LineLoop
	add		a1,a1,a2
	.set reorder
	");
#endif	
}
	
/*
;									--->	8
;DE-interlace 						---> 	40 cycles
;===>	140 cycles for 64 points
;256*256/64 = 1024 * 140 = 143360 instruction counter (~3 per point) = ~3% of a frame (let say 5% with cache misses);
;
;DE-interlace is made as : */
void PROTEX_l_WaterUpdate_DeEnterlace(USHORT *p_Source ,  UCHAR *p_Dest , ULONG Pitch , ULONG Counter)
{
#if 0
	while (Counter--)
	{
		ULONG Counter64 , Counter64R;
		Counter64 = 64;
		while (Counter64--)
		{
			SHORT VI,VIP1;
			Counter64R = (Counter64 & ~0x3f) | ((Counter64 & 0x07) << 3) | ((Counter64 & 0x38) >> 3);
			VI 	= p_Source[Counter64R];
			VIP1= p_Source[Counter64R + (Pitch)];
			VI -= VIP1;
			VI >>= 6;
			if (Counter64 == 0) VI = 140;
			p_Dest[Counter64] = (UCHAR)VI;
		}
		p_Source += Pitch;
		p_Dest += Pitch;
	}
#else // do 64 by 64
asm __volatile__ ("
		.set noreorder
		li		v1,0x8080
		ppacw 	v1,v1,v1
		pcpyh 	v1,v1
		add 	v0,a2,a2
DI_LOOP:		
		lq		t0,0x00(a0)
		lq		t1,0x10(a0)
		lq		t2,0x20(a0)
		lq		t3,0x30(a0)
		lq		t4,0x40(a0)
		lq		t5,0x50(a0)
		lq		t6,0x60(a0)
		lq		t7,0x70(a0)
		psrlh	t0 , t0 , 6
		psrlh	t1 , t1 , 6
		psrlh	t2 , t2 , 6
		psrlh	t3 , t3 , 6
		psrlh	t4 , t4 , 6
		psrlh	t5 , t5 , 6
		psrlh	t6 , t6 , 6
		psrlh	t7 , t7 , 6
		add		a0,a0,v0
		lq		zero,0x00(a0)
		ppacb	t0 , t2 , t0
		ppacb	t1 , t3 , t1
		ppacb	t2 , t6 , t4
		ppacb	t3 , t7 , t5;//(0)
		pextlb	t4 , t1 , t0
		pextub	t5 , t1 , t0
		pextlb	t6 , t3 , t2
		pextub	t7 , t3 , t2;//(1)
		pextlh	t0 , t5 , t4
		lq		zero,0x40(a0)
		pextuh	t1 , t5 , t4
		pextlh	t2 , t7 , t6
		pextuh	t3 , t7 , t6;//(2)
		pextlw	t4 , t2 , t0
		pextuw	t5 , t2 , t0
		pextlw	t6 , t3 , t1
		pextuw	t7 , t3 , t1;//(3)
		psubb	s0 , s0 , t4
		psubb	s1 , s1 , t5
		psubb	s2 , s2 , t6
		psubb	s3 , s3 , t7
		addi	a3,a3,-1
		sq		s0,0x00(a1)
		paddb	s0,t4,v1
		sq		s1,0x10(a1)
		paddb	s1,t5,v1
		sq		s2,0x20(a1)
		paddb	s2,t6,v1
		sq		s3,0x30(a1)
		paddb	s3,t7,v1
		bne a3,zero,DI_LOOP
		add		a1,a1,a2
		.set reorder
		");
#endif	
}
/*			  
;	T0:	56  48	40	32	24	16	8	0	
;	T1:	57	49	41	33	25	17	9	1	
;	T2:	58	50	42	34	26	18	10	2	
;	T3:	59	51	43	35	27	19	11	3	
;	T4:	60	52	44	36	28	20	12	4	
;	T5:	61	53	45	37	29	21	13	5	
;	T6:	62	54	46	38	30	22	14	6	
;	T7:	63  55	47	39	31	23	15	7	
				
;	t0 = 	58	50	42	34	26	18	10	2	56  48	40	32	24	16	8	[0]
;	t1 = 	59	51	43	35	27	19	11	3	57	49	41	33	25	17	9	[1]	
;	t2 = 	62	54	46	38	30	22	14	6	60	52	44	36	28	20	12	[4]
;	t3 = 	63  55	47	39	31	23	15	7	61	53	45	37	29	21	13	[5]	(0)
				  
;	t4 = 	57	56  49	48	41	40	33	32	25	24	17	16	9	8	[1	0]
;	t5 = 	59	58	51	50	43	42	35	34	27	26	19	18	11	10	[3	2]
;	t6 =	61	60	53	52	45	44	37	36	29	28	21	20	13	12	[5	4]
;	t7 =	63	62	55	54	47	46	39	38	31	30	23	22	15	14	[7	6]	(1)
					
;	t0 = 	27	26	25	24	19	18	17	16	11	10	9	8	[3	2	1	0]
;	t1 = 	59	58	57	56  51	50	49	48	43	42	41	40	[35	34	33	32]	
;	t2 =	31	30	29	28	23	22	21	20	15	14	13	12	[7	6	5	4]
;	t3 =	63	62	61	60	55	54	53	52	47	46	45	44	[39	38	37	36]	(2)
					  
;	t4 = 	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0	
;	t5 = 	31	30	29	28	27	26	25	24	23	22	21	20	19	18	17	16	
;	t6 =	47	46	45	44	43	42	41	40	39	38	37	36	35	34	33	32	
;	t7 =	63	62	61	60	59	58	57	56  55	54	53	52	51	50	49	48	(3)
						
*/
#endif

void PROTEX_l_WaterUpdate(PROTEX_tdst_Modifier *_pst_ProTex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				X, Y;
	LONG Z;
	SHORT				*S1, *S2;
	LONG                W, H;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	W = (LONG) _pst_ProTex->W;
	H = (LONG) _pst_ProTex->H;
	L_memset(_pst_ProTex->p_Surface0  , 0, W * 2);
	L_memset(_pst_ProTex->p_Surface1  , 0, W * 2);
	L_memset((USHORT *)_pst_ProTex->p_Surface0  + W * H - W , 0, W * 2);
	L_memset((USHORT *)_pst_ProTex->p_Surface1  + W * H - W , 0, W * 2);
		
#ifdef PSX2_TARGET
	{
		UCHAR *IP;
		S1 = (USHORT *)_pst_ProTex->p_Surface0 + W;
		S2 = (USHORT *)_pst_ProTex->p_Surface1 + W;
		IP = _pst_ProTex->p_SurfaceTexture;
		X = W >> 6;
		while (X--)
		{
			PROTEX_l_WaterUpdate_ASM(S1 , S2 , W , H - 2);
			PROTEX_l_WaterUpdate_DeEnterlace(S2,IP , W , H - 2);
			S1 += 64;
			S2 += 64;
			IP += 64;
		}
	}
#else
	S1 = (SHORT *)_pst_ProTex->p_Surface0 + W + 1;
	S2 = (SHORT *)_pst_ProTex->p_Surface1 + W + 1;
	Y = H - 2;
	while(Y--)
	{
		X = W - 2;
		while(X--)
		{
			//			Z = ((S2[1L] + S2[-1L] + S2[ W ] + S2[-W]) >> 6) - *S1 + (*S2 >> 0) + (*S2 >> 1) + (*S2 >> 2) + (*S2 >> 3) + (*S2 >> 4); // 1 / 64
			//			Z = ((S2[1L] + S2[-1L] + S2[ W ] + S2[-W]) >> 5) - *S1 + (*S2 >> 0) + (*S2 >> 1) + (*S2 >> 2) + (*S2 >> 3); // 1 / 32
			//			Z = ((S2[1L] + S2[-1L] + S2[ W ] + S2[-W]) >> 4) - *S1 + (*S2 >> 0) + (*S2 >> 1) + (*S2 >> 2); // 1 / 16
			//			Z = ((S2[1L] + S2[-1L] + S2[ W ] + S2[-W]) >> 3) - *S1 + (*S2 >> 0) + (*S2 >> 1); // 1 / 8
			//			Z = ((S2[1L] + S2[-1L] + S2[ W ] + S2[-W]) >> 2) - *S1 + *S2;Z-= Z>>8; // 1 / 4
			Z = ((S2[1L] + S2[-1L] + S2[ W ] + S2[-W] + S2[1L + W] + S2[-1L + W] + S2[1L - W] + S2[-1L - W]) >> 2) - *S1; Z -= Z>>8;	  // 1 / 2
//			Z = ((S2[1L] + S2[-1L] + S2[ W ] + S2[-W]) >> 1) - *S1; Z -= Z>>8;	  // 1 / 2
			//			Z = ((S2[1L] + S2[-1L] + S2[ W ] + S2[-W]) >> 1) + ((S2[1L] + S2[-1L] + S2[ W ] + S2[-W]) >> 2) - *S1; Z-= Z>>8;	  // 3 / 4
			//							3 / 4
			//			Z = ((S2[1L] + S2[-1L] + S2[ W ] + S2[-W] + S2[W + 1L] + S2[W -1L] + S2[-W - 1L] + S2[-W + 1L]) >> 2) - *S1;Z-= Z>>8;	  // 1 / 2 2
			
			//*/
			Z = lMax(Z,-32<<8);
			Z = lMin(Z,32<<8);
			*S1 = (SHORT) Z;
			S1++;	
			S2++;
		}
		
		*(S1++) = 0;
		*(S1++) = 0;
		*(S2++) = 0;
		*(S2++) = 0;
	}

#endif
	/* FLip buffer */
	S1 = (SHORT *)_pst_ProTex->p_Surface0;
	_pst_ProTex->p_Surface0 = _pst_ProTex->p_Surface1;
	S1 = (SHORT *)_pst_ProTex->p_Surface0;
	/* Update VRAM */
#ifndef PSX2_TARGET
	{
		GDI_tdst_Request_ReloadTextureParams	st_RTP;
		ULONG *p_Convert , *p_ConvertLast;
		UCHAR *p_Convert_I;
		S1 = (SHORT *)_pst_ProTex->p_Surface0;
		{
			UCHAR *pDest;
			pDest = (UCHAR *) _pst_ProTex->p_SurfaceTexture;
			Y = H;
			while(Y--)
			{
				X = W;
				while(X--)
				{
					*(pDest++) = 128 + (CHAR)((*S1 - *(S1+1)) >> 6);
					//*(pDest++) = X & 255;
					S1++;
				}
			}
		}
		p_Convert = _pst_ProTex->p_ColorBuffer;
		p_ConvertLast =  p_Convert + _pst_ProTex->H * _pst_ProTex->W;
		p_Convert_I = (UCHAR*)_pst_ProTex->p_SurfaceTexture;
//		p_Convert_I = _pst_ProTex->p_SurfaceTexture;
		while (p_Convert < p_ConvertLast)
		{
			*(p_Convert ++) = Water_Palette[*(p_Convert_I ++)];
		}
		st_RTP.pc_Data = (char *)_pst_ProTex->p_ColorBuffer;
		st_RTP.w_Texture = (USHORT)_pst_ProTex->TextureRawPalID ;//_pst_ProTex->TextureID;
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_ReloadTexture2, (ULONG) & st_RTP);
	}
#else
	{
		GDI_tdst_Request_ReloadTextureParams	st_RTP;
		st_RTP.pc_Data = (void *)((ULONG)_pst_ProTex->p_SurfaceTexture & 0x0fffffff);
		st_RTP.w_Texture = (USHORT)_pst_ProTex->TextureID;//_pst_ProTex->TextureID;
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_ReloadTexture2, (ULONG) & st_RTP);
	}
#endif

}

void PROTEX_UpdateWaterEffect(PROTEX_tdst_Modifier *p_Protex)
{
#ifdef ACTIVE_EDITORS1
	ULONG LineCounter,ulLineX1Y1X2Y2 , *p_ulLines;
#endif

	if (!(p_Protex->ulFlags & PRTX_TextureIDValid)) return;

	if (!p_Protex->p_Surface0)
	{
		/* First time? */
        p_Protex->H = TEX_gst_GlobalList.dst_Texture[p_Protex->TextureID].w_Height;
		p_Protex->W = TEX_gst_GlobalList.dst_Texture[p_Protex->TextureID].w_Width;
		p_Protex->p_Surface0 = MEM_p_AllocAlign(p_Protex->H * p_Protex->W * 5L , 64);
		if (!p_Protex->p_Surface0) return;
		*(ULONG *)&p_Protex->p_Surface1			= ((ULONG)p_Protex->p_Surface0) + p_Protex->H * p_Protex->W * 2;
		*(ULONG *)&p_Protex->p_SurfaceTexture	= ((ULONG)p_Protex->p_Surface1) + p_Protex->H * p_Protex->W * 2;
		
		L_memset(p_Protex->p_Surface0 , 0 , p_Protex->H * p_Protex->W * 5L );

#ifndef PSX2_TARGET
		p_Protex->p_ColorBuffer = (ULONG*)MEM_p_Alloc(p_Protex->H * p_Protex->W * 4L );
#else
		GSP_UnCacheAccPtr(p_Protex->p_SurfaceTexture);
#endif

	}

	/* Draw contour lines */
#ifdef ACTIVE_EDITORS1
	/* Clear Last bit*/
	{
		USHORT *p_Eraser;
		LineCounter = p_Protex->H * p_Protex->W;
		p_Eraser = p_Protex->p_Surface1;
		while (LineCounter--) *(p_Eraser++) |= 1;
	}
	LineCounter = p_Protex->ulNumberOfLines;
	p_ulLines = p_Protex->p_ulLines;
	while (LineCounter--)
	{
		ulLineX1Y1X2Y2 = *(p_ulLines++);
		if (p_Protex->W != 256)
		{
			ulLineX1Y1X2Y2 &= 0xfefefefe;
			ulLineX1Y1X2Y2 >>= 1;
			if (p_Protex->W == 128)
				PROTEX_WaterBresenheim(ulLineX1Y1X2Y2, p_Protex->p_Surface1 , 7 , 0);
			else
			{
				ulLineX1Y1X2Y2 &= 0xfefefefe;
				ulLineX1Y1X2Y2 >>= 1;
				if (p_Protex->W == 64)
					PROTEX_WaterBresenheim(ulLineX1Y1X2Y2, p_Protex->p_Surface1 , 6 , 0);
				else
				{
					ulLineX1Y1X2Y2 &= 0xfefefefe;
					ulLineX1Y1X2Y2 >>= 1;
					PROTEX_WaterBresenheim(ulLineX1Y1X2Y2, p_Protex->p_Surface1 , 5 , 0);
				}
			}

		} else
		{
			PROTEX_WaterBresenheim(ulLineX1Y1X2Y2, p_Protex->p_Surface1 , 8 , 0);
		} 
	}
	PROTEX_CompressBorder(p_Protex);
#endif
//	PROTEX_DrawBorders(p_Protex);
	/* Update Water effect & Load in VRAM */
	PROTEX_l_WaterUpdate(p_Protex);
}

void PROTEX_BeforeDraw()
{
	PROTEX_tdst_Modifier **pp_ProTxParser = &p_FirstActiveProtex;
	while (*pp_ProTxParser)
	{
		if ((*pp_ProTxParser)->ulFlags & PRTX_HasBeenUsedInFrame)
		{
			/* update water effect */
			PROTEX_UpdateWaterEffect((*pp_ProTxParser));
			(*pp_ProTxParser)->ulFlags &= ~(PRTX_HasBeenUsedInFrame );
			(*pp_ProTxParser)->ulFlags |= PRTX_LineHasBeenUsed;
			
			pp_ProTxParser = &(*pp_ProTxParser)->p_NextAcitveProtex;
		} else  
		{
			/* delete water effect from draw list */
			(*pp_ProTxParser)->ulFlags &= ~PRTX_IsInDrawList;
			(*pp_ProTxParser) = (*pp_ProTxParser)->p_NextAcitveProtex;
		}
	}
}




/*$4
***********************************************************************************************************************
***********************************************************************************************************************
*/
void PROTEX_Modifier_Reinit(MDF_tdst_Modifier *p_Mod)
{
	PROTEX_tdst_Modifier *p_Protex;
	p_Protex = (PROTEX_tdst_Modifier *) p_Mod->p_Data;
	if (!(p_Protex->ulFlags & PRTX_TextureIDValid)) return;

	if (p_Protex->p_Surface0)
	{
		L_memset(p_Protex->p_Surface0 , 0 , p_Protex->H * p_Protex->W * 2L );
	}
	if (p_Protex->p_Surface1)
	{
		L_memset(p_Protex->p_Surface1 , 0 , p_Protex->H * p_Protex->W * 2L );
	}
}

void PROTEX_InitModifier(PROTEX_tdst_Modifier *pst_SPG)
{
#ifndef PSX2_TARGET
	{
		ULONG i;
		FILE *File = NULL;
		i = 256;
		while(i--) 
		{
			ULONG ColorA , Color2;
			ColorA = (ULONG)(255.0f * fAbs(fSin((float)i * 4.0f * 3.1415927f / 256.0f)));
			if (i<128) Color2 = 0;
			else Color2 = (i - 128)<<1;
			if (i>128)
			{
				float C2;
				C2 = fAbs(fSin((float)i * 4.0f * 3.1415927f / 256.0f));
				C2 *= C2;
				C2 *= C2;
				Color2 = (ULONG)(128.0f * C2);
			}
			else Color2 = 0;

			Water_Palette[i] = (Color2) | ((Color2) << 8) | ((Color2) << 16) | ((ColorA) << 24);
		}
	}//*/
#endif
	L_memset(pst_SPG , 0 , sizeof(PROTEX_tdst_Modifier));
#ifdef ACTIVE_EDITORS
	pst_SPG->ulCodeKey = 0xC0DE2002;
#endif
	
}
/*
=======================================================================================================================
=======================================================================================================================
*/
void PROTEX_Modifier_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PROTEX_tdst_Modifier *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(PROTEX_tdst_Modifier));
	pst_Data = (PROTEX_tdst_Modifier *) _pst_Mod->p_Data;
	
	if(!p_Data)
	{
		PROTEX_InitModifier(pst_Data);
	}
	else
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(PROTEX_tdst_Modifier));
		pst_Data -> p_GO = _pst_GO;
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void PROTEX_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PROTEX_tdst_Modifier *pst_Data;
	PROTEX_tdst_Modifier **pp_ProTxParser;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	pst_Data = (PROTEX_tdst_Modifier *) _pst_Mod->p_Data;
	
	pp_ProTxParser = &p_FirstActiveProtex;
	if (pst_Data->ulFlags & PRTX_IsInDrawList)
	{
		while (*pp_ProTxParser)
		{
			if (*pp_ProTxParser == pst_Data)
				(*pp_ProTxParser) = (*pp_ProTxParser)->p_NextAcitveProtex;
			else
				pp_ProTxParser = &(*pp_ProTxParser)->p_NextAcitveProtex;
		}
	}
#ifndef PSX2_TARGET
	if (pst_Data->p_ColorBuffer)
	{
		MEM_Free(pst_Data->p_ColorBuffer );
		pst_Data->p_ColorBuffer = NULL;
	}	
#endif

	if (pst_Data->p_Surface0)
	{
		if (pst_Data->p_Surface0 > pst_Data->p_Surface1)
			MEM_FreeAlign(pst_Data->p_Surface1);
		else
			MEM_FreeAlign(pst_Data->p_Surface0);
		pst_Data->p_Surface0 = NULL;
		pst_Data->p_Surface1 = NULL;
	}	
	if (pst_Data->ulNumberOfHLines)
	{
		MEM_Free(pst_Data->P_AllDotNumbers);
		pst_Data->P_AllDotNumbers = NULL;
		pst_Data->ulNumberOfHLines = 0;
		MEM_Free(pst_Data->p_AllDot);
		pst_Data->p_AllDot = NULL;
		pst_Data->ulNumberOfDots = 0;
	}

#ifdef ACTIVE_EDITORS		
	if (pst_Data->p_ulLines)
	{
		MEM_Free(pst_Data->p_ulLines);
		pst_Data->p_ulLines = NULL;
		pst_Data->ulMaxNumberOfLines = 0;
		pst_Data->ulNumberOfLines = 0;
	}
#endif		
	MEM_Free(pst_Data);
}
#ifdef ACTIVE_EDITORS
void WaterBrez(PROTEX_tdst_Modifier *p_Protex , OBJ_tdst_GameObject *pCurrentGameObject,GEO_tdst_Object *p_CurrentGEOObject , SHORT Color)
{
	/* Transform stu stv & stp in local coordinate */
	MATH_tdst_Matrix stInvertedMatrix ONLY_PSX2_ALIGNED(16);
	MATH_tdst_Vector stLocalU,stLocalV,stLocalU_I,stLocalV_I,stLocalZ,stLocalP ONLY_PSX2_ALIGNED(16);
	GEO_Vertex *p_XYZ;
	float fLenght;
	if (!MATH_b_TestScaleType(pCurrentGameObject -> pst_GlobalMatrix))
	{
		
		MATH_InvertMatrix(&stInvertedMatrix , pCurrentGameObject -> pst_GlobalMatrix );
		MATH_TransformVertex(&stLocalP, &stInvertedMatrix , &p_Protex ->stTextureCenter);
		stInvertedMatrix.T.x = stInvertedMatrix.T.y = stInvertedMatrix.T.z = 0.0f;
		MATH_TransformVertex(&stLocalU_I, &stInvertedMatrix , &p_Protex ->stUVector);
		MATH_TransformVertex(&stLocalV_I, &stInvertedMatrix , &p_Protex ->stVVector);
		MATH_CrossProduct(&stLocalZ , &stLocalV_I , &stLocalU_I);
		MATH_NormalizeVector(&stLocalZ,&stLocalZ);
		
		fLenght = MATH_f_NormVector(&stLocalU_I);
		fLenght = 1.0f / fLenght;
		fLenght *= fLenght;
		MATH_ScaleVector(&stLocalU , &stLocalU_I , fLenght);
		fLenght = MATH_f_NormVector(&stLocalV_I);
		fLenght = 1.0f / fLenght;
		fLenght *= fLenght;
		MATH_ScaleVector(&stLocalV , &stLocalV_I , fLenght);
		{
			GEO_tdst_ElementIndexedTriangles *p_Elem,*p_ElemLast;
			p_Elem = p_CurrentGEOObject->dst_Element;
			p_ElemLast = p_Elem  + p_CurrentGEOObject->l_NbElements;
			p_XYZ = GDI_gpst_CurDD->p_Current_Vertex_List;
			while (p_Elem < p_ElemLast)
			{
				GEO_tdst_IndexedTriangle	*p_Triangle,*p_TriangleLast;
				p_Triangle = p_Elem ->dst_Triangle;
				p_TriangleLast = p_Triangle  + p_Elem -> l_NbTriangles;
				while (p_Triangle < p_TriangleLast )
				{
					float Z[3];
					ULONG D1,D2;
					MATH_tdst_Vector stLocalP2;
					MATH_SubVector(&stLocalP2 , p_XYZ + p_Triangle -> auw_Index[0] , &stLocalP);
					Z[0] = MATH_f_DotProduct(&stLocalP2, &stLocalZ);
					MATH_SubVector(&stLocalP2 , p_XYZ + p_Triangle -> auw_Index[1] , &stLocalP);
					Z[1] = MATH_f_DotProduct(&stLocalP2, &stLocalZ);
					MATH_SubVector(&stLocalP2 , p_XYZ + p_Triangle -> auw_Index[2] , &stLocalP);
					Z[2] = MATH_f_DotProduct(&stLocalP2, &stLocalZ);
					D1 = ((ULONG *)Z)[0] | ((ULONG *)Z)[1] | ((ULONG *)Z)[2];
					D2 = ((ULONG *)Z)[0] & ((ULONG *)Z)[1] & ((ULONG *)Z)[2];
					if ((D1 ^ D2) & 0x80000000)
					{
						MATH_tdst_Vector stClippedPoint[2];
						ULONG ulPointCounter , ulLastPointCounter , ulCPC;
						/* Pass over the plane */
						/* Create A line */
						ulCPC = 0;
						ulLastPointCounter = 2;
						for (ulPointCounter = 0; ulPointCounter < 3; ulPointCounter++)
						{
							if ((((ULONG *)Z)[ulPointCounter] ^ ((ULONG *)Z)[ulLastPointCounter]) & 0x80000000)
							{
								MATH_BlendVector(&stClippedPoint[ulCPC] , 
									p_XYZ + p_Triangle -> auw_Index[ulPointCounter] , 
									p_XYZ + p_Triangle -> auw_Index[ulLastPointCounter] , 
									Z[ulPointCounter] / (Z[ulPointCounter] - Z[ulLastPointCounter]));
								ulCPC++;
							}
							ulLastPointCounter = ulPointCounter;
						}
						/* Then Clip over The texture Quad */
						{
							float fU[2] , fV[2];
							MATH_SubVector(&stLocalP2 , stClippedPoint + 0 , &stLocalP);
							fU[0] = MATH_f_DotProduct(&stLocalP2, &stLocalU);
							fV[0] = MATH_f_DotProduct(&stLocalP2, &stLocalV);
							MATH_SubVector(&stLocalP2 , stClippedPoint + 1 , &stLocalP);
							fU[1] = MATH_f_DotProduct(&stLocalP2, &stLocalU);
							fV[1] = MATH_f_DotProduct(&stLocalP2, &stLocalV);
							/* Clip U -*/ 
							if ((((ULONG * )fU)[0] | ((ULONG * )fU)[1]) & 0x80000000)
							{
								if ((((ULONG * )fU)[0] ^ ((ULONG * )fU)[1]) & 0x80000000) 
								{
									float Coef;
									/* Clip u */
									if (fU[0] < 0.0f)
									{
										Coef = fU[0] / (fU[0] - fU[1]);
										fU[0] = 0.0f;
										fV[0] = Coef * (fV[1] - fV[0]) + fV[0];
									} else
									{
										Coef = fU[1] / (fU[1] - fU[0]);
										fU[1] = 0.0f;
										fV[1] = Coef * (fV[0] - fV[1]) + fV[1];
									}
								} else ulCPC = 0; /* Culled */
							}
							if (ulCPC)
							{
								/* Clip V -*/ 
								if ((((ULONG * )fV)[0] | ((ULONG * )fV)[1]) & 0x80000000)
								{
									if ((((ULONG * )fV)[0] ^ ((ULONG * )fV)[1]) & 0x80000000) 
									{
										float Coef;
										/* Clip u */
										if (fV[0] < 0.0f)
										{
											Coef = fV[0] / (fV[0] - fV[1]);
											fV[0] = 0.0f;
											fU[0] = Coef * (fU[1] - fU[0]) + fU[0];
										} else
										{
											Coef = fV[1] / (fV[1] - fV[0]);
											fV[1] = 0.0f;
											fU[1] = Coef * (fU[0] - fU[1]) + fU[1];
										}
									} else ulCPC = 0; /* Culled */
								}
								
							}
							fU[0] = 1.0f - fU[0];
							fU[1] = 1.0f - fU[1];
							fV[0] = 1.0f - fV[0];
							fV[1] = 1.0f - fV[1];
							if (ulCPC)
							{
								/* Clip U -*/ 
								if ((((ULONG * )fU)[0] | ((ULONG * )fU)[1]) & 0x80000000)
								{
									if ((((ULONG * )fU)[0] ^ ((ULONG * )fU)[1]) & 0x80000000) 
									{
										float Coef;
										/* Clip u */
										if (fU[0] < 0.0f)
										{
											Coef = fU[0] / (fU[0] - fU[1]);
											fU[0] = 0.0f;
											fV[0] = Coef * (fV[1] - fV[0]) + fV[0];
										} else
										{
											Coef = fU[1] / (fU[1] - fU[0]);
											fU[1] = 0.0f;
											fV[1] = Coef * (fV[0] - fV[1]) + fV[1];
										}
									} else ulCPC = 0; /* Culled */
								}
							}
							if (ulCPC == 2)
							{
								/* Clip V +*/ 
								if ((((ULONG * )fV)[0] | ((ULONG * )fV)[1]) & 0x80000000)
								{
									if ((((ULONG * )fV)[0] ^ ((ULONG * )fV)[1]) & 0x80000000) 
									{
										float Coef;
										/* Clip u */
										if (fV[0] < 0.0f)
										{
											Coef = fV[0] / (fV[0] - fV[1]);
											fV[0] = 0.0f;
											fU[0] = Coef * (fU[1] - fU[0]) + fU[0];
										} else
										{
											Coef = fV[1] / (fV[1] - fV[0]);
											fV[1] = 0.0f;
											fU[1] = Coef * (fU[0] - fU[1]) + fU[1];
										}
									} else ulCPC = 0; /* Culled */
								}
							}
							fU[0] = 1.0f - fU[0];
							fU[1] = 1.0f - fU[1];
							fV[0] = 1.0f - fV[0];
							fV[1] = 1.0f - fV[1];
							
							if (ulCPC == 2)
							{
								GDI_tdst_Request_DrawLineEx DLX;
								MATH_AddScaleVector(stClippedPoint + 0 , &stLocalP , &stLocalU_I , fU[0]);
								MATH_AddScaleVector(stClippedPoint + 1 , &stLocalP , &stLocalU_I , fU[1]);
								MATH_AddScaleVector(stClippedPoint + 0 , stClippedPoint + 0 , &stLocalV_I , fV[0]);
								MATH_AddScaleVector(stClippedPoint + 1 , stClippedPoint + 1 , &stLocalV_I , fV[1]);
								DLX.A = stClippedPoint + 0;
								DLX.B = stClippedPoint + 1;
								DLX.ul_Flags = 0;
								DLX.f_Width = 4.0f;
								DLX.ul_Color = 0xffff00;
								{
									float	f[2];
									f[0] = f[1] = 2; /* Marche pas ce truc?*/
									GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Enable, 0x8037 /* GL_POLYGON_OFFSET_FILL */ );
									GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Enable, 0x2A02 /* GL_POLYGON_OFFSET_LINE */ );
									GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_PolygonOffset, (ULONG) f);
								}
								
								GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) &DLX);
								{
									GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Disable, 0x8037 /* GL_POLYGON_OFFSET_FILL */ );
									GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Disable, 0x2A02 /* GL_POLYGON_OFFSET_LINE */ );
								}
								{
									ULONG ulLineX1Y1X2Y2;
									ulLineX1Y1X2Y2 = ((ULONG)(fU[0] * 255.0f) << 24) | ((ULONG)(fV[0] * 255.0f) << 16) | ((ULONG)(fU[1] * 255.0f) << 8) | ((ULONG)(fV[1] * 255.0f) << 0);
									if (p_Protex->ulFlags & PRTX_LineHasBeenUsed) 
									{
										p_Protex->ulNumberOfLines = 0;
										p_Protex->ulFlags &= ~PRTX_LineHasBeenUsed;
									}
									if (p_Protex->ulMaxNumberOfLines <= p_Protex->ulNumberOfLines)
									{
										p_Protex->ulMaxNumberOfLines += 256;
										if (p_Protex->p_ulLines == NULL)
										{
											p_Protex->p_ulLines = (ULONG*)MEM_p_Alloc(4 * p_Protex->ulMaxNumberOfLines);
										} else p_Protex->p_ulLines = (ULONG*)MEM_p_Realloc(p_Protex->p_ulLines,4 * p_Protex->ulMaxNumberOfLines);
									}
									
									p_Protex->p_ulLines[p_Protex->ulNumberOfLines++] = ulLineX1Y1X2Y2;//*/
								}
								
								
							}
						}
						
					}
					p_Triangle++;
				}
				p_Elem++;
			}
		}
	}
}
#endif
/*
=======================================================================================================================
=======================================================================================================================
*/
#ifdef ACTIVE_EDITORS
extern MAT_tdst_MultiTexture    MAT_gst_DefaultSingleMaterial;
#endif

void PROTEX_Disturb(OBJ_tdst_GameObject *pCurrentGameObject,GEO_tdst_Object *p_CurrentGEOObject)
{
	PROTEX_tdst_Modifier *p_Protex;
	p_Protex = p_FirstActiveProtex;
#ifdef ACTIVE_EDITORS
	if (pCurrentGameObject && pCurrentGameObject->pst_Base && pCurrentGameObject->pst_Base->pst_Visu && pCurrentGameObject->pst_Base->pst_Visu->pst_Material)
	if ((MAT_tdst_MultiTexture*)pCurrentGameObject->pst_Base->pst_Visu->pst_Material == &MAT_gst_DefaultSingleMaterial) return;/*maybe Is an helper. logique floue. on n'est pas des machines hein?*/
	if ((GDI_gpst_CurDD->ul_CurrentDrawMask & (GDI_Cul_DM_NotWired | GDI_Cul_DM_UseTexture | GDI_Cul_DM_DontForceColor )) != (GDI_Cul_DM_NotWired | GDI_Cul_DM_UseTexture | GDI_Cul_DM_DontForceColor)) return;
#endif
		
		if (OBJ_ul_FlagsIdentityGet(pCurrentGameObject) & (OBJ_C_IdentityFlag_Dyna|OBJ_C_IdentityFlag_Bone))
			/* disturb only with dynamic objects */
		{
			while (p_Protex)
			{
				if (p_Protex -> p_GO != pCurrentGameObject)
				{
					if (p_Protex->p_Surface0)
					{
						/* Transform stu stv & stp in local coordinate */
						MATH_tdst_Matrix stInvertedMatrix ONLY_PSX2_ALIGNED(16);
						MATH_tdst_Vector stLocalU,stLocalV,stLocalZ,stLocalP , stLocalP2 ONLY_PSX2_ALIGNED(16);
						ULONG PertuBCounter;
						float FinalU,FinalV;
						GEO_Vertex *p_XYZ,*p_XYZLast;
						float fLenght;

						PertuBCounter = 0;
						FinalU = FinalV = 0.0f;

						if (!MATH_b_TestScaleType(pCurrentGameObject -> pst_GlobalMatrix))
						{
							
							MATH_InvertMatrix(&stInvertedMatrix , pCurrentGameObject -> pst_GlobalMatrix );
							MATH_TransformVertex(&stLocalP, &stInvertedMatrix , &p_Protex ->stTextureCenter);
							stInvertedMatrix.T.x = stInvertedMatrix.T.y = stInvertedMatrix.T.z = 0.0f;
							MATH_TransformVertex(&stLocalU, &stInvertedMatrix , &p_Protex ->stUVector);
							MATH_TransformVertex(&stLocalV, &stInvertedMatrix , &p_Protex ->stVVector);
							MATH_CrossProduct(&stLocalZ , &stLocalV , &stLocalU);
							MATH_NormalizeVector(&stLocalZ,&stLocalZ);
							
							fLenght = MATH_f_NormVector(&stLocalU);
							fLenght = 1.0f / fLenght;
							fLenght *= fLenght;
							MATH_ScaleEqualVector(&stLocalU , fLenght);
							fLenght = MATH_f_NormVector(&stLocalV);
							fLenght = 1.0f / fLenght;
							fLenght *= fLenght;
							MATH_ScaleEqualVector(&stLocalV , fLenght);
							
							p_XYZ = GDI_gpst_CurDD->p_Current_Vertex_List;
							p_XYZLast = p_XYZ + p_CurrentGEOObject->l_NbPoints;
							while (p_XYZ < p_XYZLast)
							{
								float Z;
								MATH_SubVector(&stLocalP2 , p_XYZ , &stLocalP);
								Z = MATH_f_DotProduct(&stLocalP2, &stLocalZ);
								Z *= Z;
								if (Z < 0.02f)
								{
									float fU,fV;
									fU = MATH_f_DotProduct(&stLocalP2, &stLocalU);
									fV = MATH_f_DotProduct(&stLocalP2, &stLocalV);
									if ((fU > 0.0f) && (fV > 0.0f) && (fU < 1.0f) && (fV < 1.0f))
									{
										PertuBCounter ++;
										FinalU += fU;
										FinalV += fV;
									}
								}
								p_XYZ ++;
							}
							if (PertuBCounter)
							{
								SHORT /**ptr1,*/*ptr2,Counter;
								LONG X,Y;
								FinalU /= (float)PertuBCounter;
								FinalV /= (float)PertuBCounter;
								X = (ULONG)(1024.0f*1024.0f + (float)p_Protex->W * FinalU);
								Y = (ULONG)(1024.0f*1024.0f + (float)p_Protex->H * FinalV);
								ptr2 = ((SHORT *)p_Protex->p_Surface1) + Y * p_Protex->W + X;
								{
									LONG SurfaceSizeCounter;
#define surfacesize 5
									for (SurfaceSizeCounter = 1; SurfaceSizeCounter < surfacesize; SurfaceSizeCounter++)
									{
										for (Counter = 0; Counter < (SurfaceSizeCounter<<1)+1; Counter ++)
										{
											LONG X2,Y2,Counter2;
											X2 = X + p_Protex->H - SurfaceSizeCounter + Counter;
											X2 &= p_Protex->W  - 1;
#ifdef PSX2_TARGET
											X2 = (X2 & ~0x3f) | ((X2 & 0x07) << 3) | ((X2 & 0x38) >> 3);
#endif													
											for (Counter2 = 0; Counter2 < (SurfaceSizeCounter<<1)+1; Counter2 ++)
											{
												Y2 = Y + p_Protex->H - SurfaceSizeCounter + Counter2;
												Y2 &= p_Protex->W  - 1;
												ptr2 = ((SHORT *)p_Protex->p_Surface0) + Y2 * p_Protex->W + X2;
												//											*(ptr2) += (surfacesize - lMin(lMin(Counter -surfacesize , surfacesize - Counter ) , lMin(Counter2 -surfacesize , surfacesize - Counter2 ) )) << 16;
//													*(ptr2) = 0x1000;
												*(ptr2) += 0x200;
												ptr2 = ((SHORT *)p_Protex->p_Surface1) + Y2 * p_Protex->W + X2;
												*(ptr2) -= 0x200;
											}
										}
									}
								}
							}
						}
				}
			}
			p_Protex = p_Protex->p_NextAcitveProtex;
		}
	}
#ifdef ACTIVE_EDITORS
	else { /* Update border if necessary */
		while (p_Protex)
		{
			if (p_Protex -> p_GO != pCurrentGameObject)
			{
				if (p_Protex->p_Surface0)
				{
					WaterBrez(p_Protex , pCurrentGameObject , p_CurrentGEOObject , 0);
				}
			}
			p_Protex = p_Protex->p_NextAcitveProtex;
		}
	}
#endif
}

extern void MAT_UV_COMPUTE_GetMAtrix(GDI_tdst_DisplayData	*pst_CurDD,OBJ_tdst_GameObject *_pst_GO , ULONG CurrentAddFlag , MATH_tdst_Matrix	*p_MatrixDSt);
void PROTEX_SetStUstVstP(PROTEX_tdst_Modifier *p_PT,MATH_tdst_Vector *pstU,MATH_tdst_Vector *pstV,MATH_tdst_Vector *pstT)
{
	p_PT->stTextureCenter = *pstT;
	p_PT->stUVector = *pstU;
	p_PT->stVVector = *pstV;
}
void PROTEX_Apply2DMatrix(PROTEX_tdst_Modifier *p_PT,MAT_tdst_MTLevel		*pst_MLTTXLVL)
{
	if(!MAT_VUISIdentity(pst_MLTTXLVL))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MAT_tdst_Decompressed_UVMatrix	TDST_ummAT;
		MATH_tdst_Vector stSaveU,stSaveV;
		float fLenght;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		stSaveU = p_PT->stUVector;
		stSaveV = p_PT->stVVector;
		/* Compute new UV */
		MAT_VUDecompress(pst_MLTTXLVL, &TDST_ummAT);
		/* Turn & rotate UV */
		fLenght = TDST_ummAT.UVMatrix[0] * TDST_ummAT.UVMatrix[0] + TDST_ummAT.UVMatrix[2] * TDST_ummAT.UVMatrix[2];
		fLenght = fInvSqrt(fLenght);
		fLenght *= fLenght;
		TDST_ummAT.UVMatrix[0] *= fLenght;
		TDST_ummAT.UVMatrix[2] *= fLenght;
		fLenght = TDST_ummAT.UVMatrix[1] * TDST_ummAT.UVMatrix[1] + TDST_ummAT.UVMatrix[3] * TDST_ummAT.UVMatrix[3];
		fLenght = fInvSqrt(fLenght);
		fLenght *= fLenght;
		TDST_ummAT.UVMatrix[1] *= fLenght;
		TDST_ummAT.UVMatrix[3] *= fLenght;
		MATH_ScaleVector(&p_PT->stUVector , &stSaveU , TDST_ummAT.UVMatrix[0]);
		MATH_AddScaleVector(&p_PT->stUVector , &p_PT->stUVector , &stSaveV , TDST_ummAT.UVMatrix[2]);
		MATH_ScaleVector(&p_PT->stVVector , &stSaveU , TDST_ummAT.UVMatrix[1]);
		MATH_AddScaleVector(&p_PT->stVVector , &p_PT->stVVector , &stSaveV , TDST_ummAT.UVMatrix[3]);
		MATH_AddScaleVector(&p_PT->stTextureCenter ,&p_PT->stTextureCenter ,&p_PT->stUVector , -TDST_ummAT.AddU);
		MATH_AddScaleVector(&p_PT->stTextureCenter ,&p_PT->stTextureCenter ,&p_PT->stVVector , -TDST_ummAT.AddV);
		/*			DstUV->u = TDST_ummAT.UVMatrix[0] * fSaveU + TDST_ummAT.UVMatrix[2] * SrcUV->v + TDST_ummAT.AddU;
		DstUV->v = TDST_ummAT.UVMatrix[1] * fSaveU + TDST_ummAT.UVMatrix[3] * SrcUV->v + TDST_ummAT.AddV;
		SrcUV++;
		DstUV++;*/
	}
}
void PROTEX_ComputeStUstVstP(PROTEX_tdst_Modifier *p_PT,MAT_tdst_MTLevel		*pst_MLTTXLVL,OBJ_tdst_GameObject *_pst_GO)
{
	ULONG UVMode;
	MATH_tdst_Matrix	Matrix ,MatrixW ONLY_PSX2_ALIGNED(16);
	ULONG 				GetXYZ;
	UVMode = MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags);
	if (UVMode != MAT_Cc_UV_Planar_GZMO) return;
	GetXYZ = MAT_GET_XYZ(pst_MLTTXLVL->s_AditionalFlags);
	if(MAT_GET_MatrixFrom(pst_MLTTXLVL->s_AditionalFlags) == MAT_CC_OBJECT)
	{
		switch(GetXYZ)
		{
		case MAT_CC_X:
			MATH_MakeOGLMatrix(&MatrixW, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
			PROTEX_SetStUstVstP(p_PT,MATH_pst_GetYAxis(&MatrixW),MATH_pst_GetZAxis(&MatrixW),&OBJ_pst_GetAbsoluteMatrix(_pst_GO)->T);					
		case MAT_CC_Y:
			MATH_MakeOGLMatrix(&MatrixW, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
			PROTEX_SetStUstVstP(p_PT,MATH_pst_GetXAxis(&MatrixW),MATH_pst_GetZAxis(&MatrixW),&OBJ_pst_GetAbsoluteMatrix(_pst_GO)->T);
			break;
		case MAT_CC_Z:
			MATH_MakeOGLMatrix(&MatrixW, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
			PROTEX_SetStUstVstP(p_PT,MATH_pst_GetXAxis(&MatrixW),MATH_pst_GetYAxis(&MatrixW),&OBJ_pst_GetAbsoluteMatrix(_pst_GO)->T);
			break;
		case MAT_CC_XYZ:
			break;
		}
	}
	else
	{
		MAT_UV_COMPUTE_GetMAtrix(GDI_gpst_CurDD , _pst_GO ,  pst_MLTTXLVL->s_AditionalFlags , &Matrix);
		switch(GetXYZ)
		{
		case MAT_CC_X:
			{
				MATH_tdst_Vector VT;
				MATH_InitVector(&VT , Matrix.T.y , Matrix.T.z , 0.0f);
				MATH_MulMatrixMatrix(&MatrixW, OBJ_pst_GetAbsoluteMatrix(_pst_GO), &Matrix );
				PROTEX_SetStUstVstP(p_PT,MATH_pst_GetYAxis(&MatrixW),MATH_pst_GetZAxis(&MatrixW),&OBJ_pst_GetAbsoluteMatrix(_pst_GO)->T);
			}
		case MAT_CC_Y:
			{
				MATH_tdst_Vector VT;
				MATH_InitVector(&VT , Matrix.T.x , Matrix.T.z , 0.0f);
				MATH_MulMatrixMatrix(&MatrixW, OBJ_pst_GetAbsoluteMatrix(_pst_GO), &Matrix );
				PROTEX_SetStUstVstP(p_PT,MATH_pst_GetXAxis(&MatrixW),MATH_pst_GetZAxis(&MatrixW),&OBJ_pst_GetAbsoluteMatrix(_pst_GO)->T);					
			}
			break;
		case MAT_CC_Z:
			{
				MATH_tdst_Vector VT;
				MATH_InitVector(&VT , Matrix.T.x , Matrix.T.y , 0.0f);
				MATH_MulMatrixMatrix(&MatrixW, OBJ_pst_GetAbsoluteMatrix(_pst_GO), &Matrix );
				PROTEX_SetStUstVstP(p_PT,MATH_pst_GetXAxis(&MatrixW),MATH_pst_GetYAxis(&MatrixW),&OBJ_pst_GetAbsoluteMatrix(_pst_GO)->T);
			}
			break;
		case MAT_CC_XYZ:
			{
				MATH_tdst_Vector stU,stV,stUC,stVC;
				MATH_tdst_Vector A;
				MATH_InitVector(&stUC , Cf_Sqrt2 * 0.5f , -Cf_Sqrt2 * 0.5f , 0.0f);
				MATH_InitVector(&stVC , -Cf_InvSqrt3 , -Cf_InvSqrt3 , Cf_InvSqrt3);
				A.x = MATH_f_DotProduct(&stUC , &Matrix.T);
				A.y = MATH_f_DotProduct(&stVC , &Matrix.T);
				stU.x = Matrix.Ix * stUC.x + Matrix.Jx * stUC.y + Matrix.Kx * stUC.z;
				stU.y = Matrix.Iy * stUC.x + Matrix.Jy * stUC.y + Matrix.Ky * stUC.z;
				stU.z = Matrix.Iz * stUC.x + Matrix.Jz * stUC.y + Matrix.Kz * stUC.z;
				stV.x = Matrix.Ix * stVC.x + Matrix.Jx * stVC.y + Matrix.Kx * stVC.z;
				stV.y = Matrix.Iy * stVC.x + Matrix.Jy * stVC.y + Matrix.Ky * stVC.z;
				stV.z = Matrix.Iz * stVC.x + Matrix.Jz * stVC.y + Matrix.Kz * stVC.z;
				PROTEX_SetStUstVstP(p_PT,&stU,&stV,&OBJ_pst_GetAbsoluteMatrix(_pst_GO)->T);
			}
			break;
		}
	}
	PROTEX_Apply2DMatrix(p_PT,pst_MLTTXLVL);
}

void PROTEX_FindAndComputeStUstVstP(PROTEX_tdst_Modifier *p_PT,OBJ_tdst_GameObject *_pst_GO)
{
	MAT_tdst_Material				*p_stMaterial;
	if (_pst_GO && _pst_GO ->pst_Base && _pst_GO->pst_Base->pst_Visu && _pst_GO ->pst_Base->pst_Visu->pst_Material)
	{
		p_stMaterial = (MAT_tdst_Material	*)_pst_GO ->pst_Base->pst_Visu->pst_Material;
		if(p_stMaterial->st_Id.i->ul_Type == GRO_MaterialMulti)
		{
			p_stMaterial = ((MAT_tdst_Multi *)p_stMaterial)->dpst_SubMaterial[lMin(p_PT->ulSubMatSourceNum, ((MAT_tdst_Multi *)p_stMaterial)->l_NumberOfSubMaterials - 1)];
		}
		if(p_stMaterial->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
		{
			MAT_tdst_MultiTexture	*pst_MLTTX;
			MAT_tdst_MTLevel		*pst_MLTTXLVL;
			ULONG SmtNum;
			pst_MLTTX = (MAT_tdst_MultiTexture	*)p_stMaterial;
			pst_MLTTXLVL = 	pst_MLTTX->pst_FirstLevel;
			SmtNum = p_PT->ulSubMatMuTexSourceNum;
			if (pst_MLTTXLVL)
			{
				while (pst_MLTTXLVL->pst_NextLevel && SmtNum)
				{
					pst_MLTTXLVL = pst_MLTTXLVL->pst_NextLevel;
					SmtNum--;
				}
				PROTEX_ComputeStUstVstP(p_PT,pst_MLTTXLVL,_pst_GO);
				/* Find procedural texture */
				{
					ULONG w_TextureRaw;
					w_TextureRaw = pst_MLTTXLVL->s_TextureId ;
					if (w_TextureRaw != -1)
					{
						if (TEX_gst_GlobalList.dst_Texture[w_TextureRaw].uw_Flags & TEX_uw_RawPal)
						{
							p_PT->TextureRawPalID = w_TextureRaw ;
							w_TextureRaw = TEX_gst_GlobalList.dst_Texture[w_TextureRaw].w_Height; /* Après une longue enquête... Merci Vincent! */
						}
						if (w_TextureRaw != -1)
						{
							if (p_PT->ulFlags & PRTX_TextureIDValid)
							{
								if (w_TextureRaw != (USHORT)p_PT->TextureID)
								{
									/* Texture Has changed ? */
									if (p_PT->p_Surface0)
									{
										MEM_FreeAlign(p_PT->p_Surface0);
										p_PT->p_Surface0 = NULL;
									}
								}
							}
							p_PT->TextureID = w_TextureRaw;
							if (p_PT->TextureID < (ULONG)TEX_gst_GlobalList.l_NumberOfTextures)
								p_PT->ulFlags |= PRTX_TextureIDValid;
						}
					}
/*	                if ( (pst_RawPal->uw_Flags & TEX_uw_RawPal) && (pst_RawPal->w_Height == (SHORT) _ul_Texture) )
                {
                    OGL_Texture_SetPalette( pst_RawPal->w_Width );
*/
				
				}
			}
		}
	}
}


void PROTEX_Modifier_Apply
(
	MDF_tdst_Modifier		*_pst_Mod,
	GEO_tdst_Object			*_pst_Obj
	)
{
	
}
/*
=======================================================================================================================
=======================================================================================================================
*/

void PROTEX_Modifier_Unapply
(
	MDF_tdst_Modifier		*_pst_Mod,
	GEO_tdst_Object			*_pst_Obj
	)
{
	/* Compute stU stV SstCenter */
	PROTEX_tdst_Modifier *p_PT;
	p_PT = (PROTEX_tdst_Modifier *) _pst_Mod->p_Data;
	

	/* Activate Protex */
	if (!(p_PT->ulFlags & PRTX_IsInDrawList))
	{
		p_PT->p_NextAcitveProtex = p_FirstActiveProtex;
		p_PT ->ulFlags |= PRTX_IsInDrawList;
		p_FirstActiveProtex = p_PT;
	}

	p_PT ->ulFlags |= PRTX_HasBeenUsedInFrame;
	PROTEX_FindAndComputeStUstVstP(p_PT,_pst_Mod->pst_GO);
	
	//	UPDATEOK = 1;
	/* Disrupt Procedural texture */
	/*	if ((p_PT->p_Surface0) && (p_PT->p_Surface1) )
	{
	UCHAR *ptr ,*ptr2 , Valu;
	ptr = &((UCHAR *)p_PT->p_Surface0)[(p_PT->H >> 1) * p_PT->W + (p_PT->W >> 1)];
	ptr2= &((UCHAR *)p_PT->p_Surface1)[(p_PT->H >> 1) * p_PT->W + (p_PT->W >> 1)];
	Valu = (rand()%8) - 4;
	*ptr += Valu;
	*(ptr + 1) += Valu;
	*(ptr - 1) += Valu;
	*(ptr + p_PT->W) += Valu;
	*(ptr - p_PT->W) += Valu;
	*ptr2 += Valu;
	*(ptr2 + 1) += Valu;
	*(ptr2 - 1) += Valu;
	*(ptr2 + p_PT->W) += Valu;
	*(ptr2 - p_PT->W) += Valu;
}*/
	
	/* Display Helper */
#ifdef ACTIVE_EDITORS
	{
		
		GDI_tdst_Request_DrawLineEx DLX;
		MATH_tdst_Vector stLineEx[2];
		
		DLX.A = stLineEx + 0;
		DLX.B = stLineEx + 1;
		DLX.f_Width = 4.0f;
		DLX.ul_Color = 0xffff;
		DLX.ul_Flags = 0;
		
		GDI_SetViewMatrix((*GDI_gpst_CurDD) , &GDI_gpst_CurDD->st_Camera.st_InverseMatrix);
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			static float	f[2] = { -2, -2 };
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Enable, 0x2A02 /* GL_POLYGON_OFFSET_LINE */ );
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_PolygonOffset, (ULONG) f);
		}
		stLineEx[0] = p_PT->stTextureCenter;
		MATH_AddScaleVector(&stLineEx[1] , &stLineEx[0] , &p_PT->stUVector , 1.0f);
		
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) &DLX);
		
		MATH_AddScaleVector(&stLineEx[1] , &stLineEx[0] , &p_PT->stVVector , 1.0f);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) &DLX);
		
		MATH_AddScaleVector(&stLineEx[0] , &p_PT->stTextureCenter , &p_PT->stUVector , 1.0f);
		MATH_AddScaleVector(&stLineEx[0] , &stLineEx[0] , &p_PT->stVVector , 1.0f);
		
		MATH_AddScaleVector(&stLineEx[1] , &stLineEx[0] , &p_PT->stUVector , -1.0f);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) &DLX);
		//*/
		MATH_AddScaleVector(&stLineEx[1] , &stLineEx[0] , &p_PT->stVVector , -1.0f);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) &DLX);
		GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix);
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_Disable, 0x2A02 /* GL_POLYGON_OFFSET_LINE */ );
	}
#endif
}





/*
=======================================================================================================================
=======================================================================================================================
*/

ULONG PROTEX_ul_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PROTEX_tdst_Modifier *pst_Data;
	ULONG							ul_Size;
	ULONG							ul_Version;
	unsigned char					*pc_Cur;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	pc_Cur = (unsigned char*)_pc_Buffer;
	pst_Data = (PROTEX_tdst_Modifier *) _pst_Mod->p_Data;
	/* Read Size */
	ul_Size = LOA_ReadLong((CHAR**)&pc_Cur);
	/* Read version */
	ul_Version = LOA_ReadLong((CHAR**)&pc_Cur);
	PROTEX_InitModifier(pst_Data);
	pst_Data ->p_GO = _pst_Mod->pst_GO;
	if (ul_Version > 0)
	{
		pst_Data->ulNumberOfHLines = LOA_ReadLong((CHAR**)&pc_Cur);
		pst_Data->P_AllDotNumbers = (UCHAR*)MEM_p_Alloc(pst_Data->ulNumberOfHLines);
		LOA_ReadCharArray((CHAR**)&pc_Cur, (CHAR*)pst_Data->P_AllDotNumbers, pst_Data->ulNumberOfHLines);
		
		pst_Data->ulNumberOfDots = LOA_ReadLong((CHAR**)&pc_Cur);
		pst_Data->p_AllDot = (UCHAR*)MEM_p_Alloc(pst_Data->ulNumberOfDots );
		LOA_ReadCharArray((CHAR**)&pc_Cur, (CHAR*)pst_Data->p_AllDot, pst_Data->ulNumberOfDots);
#ifdef PSX2_TARGET
		{
			ULONG DotCounbter;
			UCHAR *pDot;
			DotCounbter = pst_Data->ulNumberOfDots;
			pDot = pst_Data->p_AllDot;
			while (DotCounbter--)
			{
				*(pDot++) = (*pDot & ~0x3f) | ((*pDot & 0x07) << 3) | ((*pDot & 0x38) >> 3);
			}
		}
#endif		
		
	}
	return (ULONG)(pc_Cur - (unsigned char*)_pc_Buffer);
//	return pc_Cur - _pc_Buffer;
}

#ifdef ACTIVE_EDITORS
	
/*
=======================================================================================================================
=======================================================================================================================
*/
void PROTEX_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	PROTEX_tdst_Modifier *pst_Data;
	ULONG							ulSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	pst_Data = (PROTEX_tdst_Modifier *) _pst_Mod->p_Data;
	/* Save Size */
	ulSize = 4/* version */ + pst_Data->ulNumberOfHLines + 4 + pst_Data->ulNumberOfDots + 4;
	SAV_Buffer(&ulSize, 4);
	/* Save version */
	ulSize = 1;
	SAV_Buffer(&ulSize, 4);
	/* Save borders */
	SAV_Buffer(&pst_Data->ulNumberOfHLines, 4);
	SAV_Buffer(pst_Data->P_AllDotNumbers , pst_Data->ulNumberOfHLines);
	SAV_Buffer(&pst_Data->ulNumberOfDots, 4);
	SAV_Buffer(pst_Data->p_AllDot , pst_Data->ulNumberOfDots);
}

#endif


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
