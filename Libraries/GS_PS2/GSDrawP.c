 
/*$T GSPinit.c GC! 1.081 05/04/00 15:08:05 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
/*#pragma global_optimizer on
#pragma optimization_level 4*/


#include <eeregs.h>
#include <eestruct.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <libpc.h>


#include "Gsp.h"
#include "GSPinit.h"
#include "GSPtex.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJculling.h"
#include "MATerial/MATstruct.h"

#include "Gsp_Bench.h"

#ifdef PSX2_TARGET
#include "ENGvars.h"

#define GSP_DRAWP_ASSEMBLY
extern ULONG VU0_JumpTatble[5];
extern ULONG VU0_JumpTatble_STRIPS[5];
extern ULONG VU0_JumpTatble_SYM_STRIPS[5];
extern ULONG ShowNormals;
/* mamagouille */
#include "PSX2debug.h"
#endif
#if defined PSX2_TARGET && defined __cplusplus
extern "C"
{
#endif
extern void GSP_FlushABackCache(GSP_GEOBackCache *p_BCPtr,u_int FlushWhat);
extern void Gsp_SetColorMulAdd(Gsp_BigStruct	  *p_BIG);
asm void GSP_FAST_COPY_TRIS_XUVPP(ULONG Number ,GEO_Vertex	*_pst_PointSRC , GEO_Vertex	*_pst_PointDST , GEO_tdst_IndexedTriangle *p_Tris,GEO_tdst_UV	*_pst_UVSRC , GEO_tdst_UV	*_pst_UVDST , ULONG *p_Colors)
{
		.set noreorder
		qmove	t8,t2
		addiu   t6,t0,0
		addiu   t7,t1,0
		ppacw 	a1,a1,a1
		ppacw 	a1,a1,a1
		ppacw 	t6,t6,t6
		ppacw 	t6,t6,t6
TLOOP:
		lq 		t0,0(a3)
		prot3w	t3,t0
		prevh	t3,t3
		pextlh 	t3,zero,t3
		pextlh 	t0,zero,t0
		psllw  	t0,t0,4
		psllw  	t3,t3,3
		paddw  	t0,t0,a1
		dsrl32 	t1,t0,0
		pcpyud  t2,t0,t0
		lq		t0,0(t0) //
		paddw  	t3,t3,t6
		pcpyud  t5,t3,t3
		dsrl32 	t4,t3,0
		sq		t0,0(a2)
		ld		t5,0(t5)
		ld		t4,0(t4)
		ld		t3,0(t3)
		sd		t5,0(t7)
		sd		t4,8(t7)
		sd		t3,16(t7)
		addiu	a3,a3,16
		lq		t1,0(t1) //
		lq		t9,0(t2) //
		sq		t1,16(a2)
		addi	a0,a0,-1
		sq		t9,32(a2)
		pextuw	t1,t1,t0
		addiu	t7,t7,24
		pcpyud	t1,t1
		beq 	a0,zero,TLOOP_END_Store_Last_Color
		sd		t1,0(t8)
		lq 		t0,0(a3)
		prot3w	t3,t0
		prevh	t3,t3
		pextlh 	t3,zero,t3
		pextlh 	t0,zero,t0
		psllw  	t0,t0,4
		psllw  	t3,t3,3
		paddw  	t0,t0,a1
		dsrl32 	t1,t0,0
		pcpyud  t2,t0,t0
		lq		t0,0(t0) //
		paddw  	t3,t3,t6
		pcpyud  t5,t3,t3
		dsrl32 	t4,t3,0
		sq		t0,48+0(a2)
		ld		t5,0(t5)
		pextuw	t0,t0,t9
		ld		t4,0(t4)
		pcpyud	t0,t0
		ld		t3,0(t3)
		sd		t5,0(t7)
		sd		t4,8(t7)
		sd		t0,8(t8)
		lq		t1,0(t1) //
		sd		t3,16(t7)
		addiu	a3,a3,16
		lq		t2,0(t2) //
		sq		t1,48+16(a2)
		addi	a0,a0,-1
		sq		t2,48+32(a2)
		pextuw	t0,t2,t1
		addiu	t7,t7,24
		pcpyud	t0,t0
		addiu	a2,a2,48+48
		sd		t0,16(t8)
		bne 	a0,zero,TLOOP
		addiu	t8 , t8 , 24
TLOOP_END:
		jr		ra
		nop
TLOOP_END_Store_Last_Color:
		pextuw	t9,t9,t9
		pcpyud	t9,t9
		jr		ra
		sw		t9,8(t8)
.set reorder
}

asm void GSP_FAST_COPY_TRIS_1UVPP(ULONG Number ,GEO_Vertex	*_pst_PointSRC , GEO_Vertex	*_pst_PointDST , GEO_tdst_IndexedTriangle *p_Tris,GEO_tdst_UV	*_pst_UVSRC , GEO_tdst_UV	*_pst_UVDST , ULONG *p_Colors)
 {
		.set noreorder
		qmove	t8,t2
		addiu   t6,t0,0
		addiu   t7,t1,0
		ppacw 	a1,a1,a1
		ppacw 	a1,a1,a1
		ppacw 	t6,t6,t6
		ppacw 	t6,t6,t6
TLOOP:
		lq 		t0,0(a3)
		pextlh 	t0,zero,t0
		psllw  	t0,t0,4
		psrlw  	t3,t0,1
		paddw  	t0,t0,a1
		dsrl32 	t1,t0,0
		pcpyud  t2,t0,t0
		lq		t0,0(t0)	//
		paddw  	t3,t3,t6
		pcpyud  t5,t3,t3
		dsrl32 	t4,t3,0
		sq		t0,0(a2)
		ld		t3,0(t3)
		ld		t4,0(t4)
		ld		t5,0(t5)
		sd		t3,0(t7)
		sd		t4,8(t7)
		sd		t5,16(t7)
		addiu	a3,a3,16
		lq		t1,0(t1)	//
		addi	a0,a0,-1
		sq		t1,16(a2)
		lq		t9,0(t2)	//
		pextuw	t1,t1,t0
		sq		t9,32(a2)
		pcpyud	t1,t1
		addiu	t7,t7,24
		beq 	a0,zero,TLOOP_END_Store_Last_Color
		sd		t1,0(t8)
		lq 		t0,0(a3)
		pextlh 	t0,zero,t0
		psllw  	t0,t0,4
		psrlw  	t3,t0,1
		paddw  	t0,t0,a1
		dsrl32 	t1,t0,0
		pcpyud  t2,t0,t0
		lq		t0,0(t0)	//
		paddw  	t3,t3,t6
		sq		t0,48+0(a2)
		pcpyud  t5,t3,t3
		dsrl32 	t4,t3,0
		pextuw	t0,t0,t9
		ld		t3,0(t3)
		pcpyud	t0,t0
		ld		t4,0(t4)
		ld		t5,0(t5)
		sd		t0,8(t8)
		sd		t3,0(t7)
		sd		t4,8(t7)
		sd		t5,16(t7)
		addiu	a3,a3,16
		lq		t1,0(t1)	//
		sq		t1,48+16(a2)
		addi	a0,a0,-1
		lq		t2,0(t2)	//
		pextuw	t0,t2,t1
		sq		t2,48+32(a2)
		addiu	t7,t7,24
		pcpyud	t0,t0
		addiu	a2,a2,48+48
		sd		t0,16(t8)
		bne 	a0,zero,TLOOP
		addiu	t8 , t8 , 24
TLOOP_END:
		jr ra
		nop
TLOOP_END_Store_Last_Color:
		pextuw	t9,t9,t9
		pcpyud	t9,t9
		jr		ra
		sw		t9,8(t8)
		
		.set reorder
}


asm void GSP_FAST_COPY_TRIS(ULONG Number ,GEO_Vertex	*_pst_PointSRC , GEO_Vertex	*_pst_PointDST , GEO_tdst_IndexedTriangle *p_Tris , ULONG *p_Colors)
{
.set noreorder
		qmove	t8,t0
		ppacw 	a1,a1,a1
		ppacw 	a1,a1,a1
TLOOP:
		lq 		t0,0(a3)
		lq 		t3,16(a3)
		pextlh 	t0,zero,t0
		pextlh 	t3,zero,t3
		psllw  	t0,t0,4
		psllw  	t3,t3,4
		paddw  	t0,t0,a1
		paddw  	t3,t3,a1
		dsrl32 	t1,t0,0
		dsrl32 	t4,t3,0
		pcpyud  t2,t0,t0
		pcpyud  t5,t3,t3
		addiu	a3,a3,32
		lq		t0,0(t0)
		lq		t1,0(t1)
		sq		t0,0(a2) //
		sq		t1,16(a2)//
		pextuw	t0,t1,t0
		addi	a0,a0,-1
		lq		t2,0(t2)
		pcpyud	t0,t0
		sq		t2,32(a2)//
		beq		a0,zero,TLOOP_Store_Last_Color
		sd		t0,0(t8)
		lq		t3,0(t3)
		sq		t3,48(a2)//
		pextuw	t0,t3,t2
		lq		t4,0(t4)
		pcpyud	t0,t0
		sq		t4,64(a2)//
		lq		t5,0(t5)
		sd		t0,8(t8)
		pextuw	t1,t5,t4
		addi	a0,a0,-1
		pcpyud	t1,t1
		sq		t5,80(a2)//
		sd		t1,16(t8)
		addiu	a2,a2,96
		bne 	a0,zero,TLOOP
		addiu	t8,t8,24
TLOOP_END:
		jr		ra
		nop
TLOOP_Store_Last_Color:
		pextuw	t2,t2,t2
		pcpyud	t2,t2
		jr		ra
		sw		t2,8(t8)
.set reorder
}

asm void GSP_FAST_COPY_UV_1PP(ULONG Number ,GEO_tdst_UV	*_pst_PointSRC , GEO_tdst_UV	*_pst_PointDST , GEO_tdst_IndexedTriangle *p_Tris)
{
.set noreorder
		ppacw 	a1,a1,a1
		ppacw 	a1,a1,a1
TLOOP:
		lq 		t0,0(a3)
		lq 		t3,16(a3)
		pextlh 	t0,zero,t0
		pextlh 	t3,zero,t3
		psllw  	t0,t0,3
		psllw  	t3,t3,3
		paddw  	t0,t0,a1
		paddw  	t3,t3,a1
		dsrl32 	t1,t0,0
		dsrl32 	t4,t3,0
		pcpyud  t2,t0,t0
		pcpyud  t5,t3,t3
		addi	a0,a0,-1
		ld		t0,0(t0)
		ld		t1,0(t1)
		ld		t2,0(t2)
		pcpyld  t0,t1,t0
		addiu	a3,a3,32
		beq		a0,zero,TLOOP_END
		sq		t0,0x00(a2) //
		ld		t3,0(t3)
		ld		t4,0(t4)
		ld		t5,0(t5)
		addi	a0,a0,-1
		pcpyld  t2,t3,t2
		pcpyld  t4,t5,t4
		sq		t2,0x10(a2) //
		sq		t4,0x20(a2)//
		
		bne 	a0,zero,TLOOP
		addiu	a2,a2,0x30
TLOOP_END:
		jr		ra
		sd		t2,0x10(a2)//
.set reorder
}
asm void GSP_FAST_COPY_UV(ULONG Number ,GEO_tdst_UV	*_pst_PointSRC , GEO_tdst_UV	*_pst_PointDST , GEO_tdst_IndexedTriangle *p_Tris)
{
.set noreorder
		ppacw 	a1,a1,a1
		ppacw 	a1,a1,a1
TLOOP:
		lq 		t0,0(a3)
		lq 		t3,16(a3)
		prot3w	t0,t0
		prot3w	t3,t3
		prevh	t0,t0
		prevh	t3,t3
		pextlh 	t0,zero,t0
		pextlh 	t3,zero,t3
		psllw  	t0,t0,3
		psllw  	t3,t3,3
		paddw  	t0,t0,a1
		paddw  	t3,t3,a1
		dsrl32 	t1,t0,0
		dsrl32 	t4,t3,0
		pcpyud  t2,t0,t0
		pcpyud  t5,t3,t3
		addi	a0,a0,-1
		ld		t1,0(t1)
		ld		t2,0(t2)
		ld		t0,0(t0)
		pcpyld  t2,t1,t2
		addiu	a3,a3,32
		beq		a0,zero,TLOOP_END
		sq		t2,0x00(a2)//
		ld		t3,0(t3)
		ld		t4,0(t4)
		ld		t5,0(t5)
		addi	a0,a0,-1
		pcpyld  t0,t5,t0
		pcpyld  t4,t3,t4
		sq		t0,0x10(a2) //
		sq		t4,0x20(a2)//
		bne 	a0,zero,TLOOP
		addiu	a2,a2,0x30
		jr		ra
		nop
TLOOP_END:
		jr		ra
		sd		t0,0x10(a2) //
.set reorder
}

asm void GSP_FAST_COPY_COLORS(ULONG Number ,ULONG	*_pst_ColorSRC , ULONG	*_pst_ColorDST , GEO_tdst_IndexedTriangle *p_Tris )
{
.set noreorder
		addiu        sp,sp,-16
		sw           a0,0(sp)
		ppacw 	t8,t0,t0
		ppacw 	t9,t1,t1
		pref	0,0x00(a3)
		srl		a0,a0,2
		ppacw 	t8,t8,t8
		ppacw 	t9,t9,t9
		pref	0,0x40(a3)
		ppacw 	a1,a1,a1
		beq 	a0,zero,TLOOP_STOP
		ppacw 	a1,a1,a1
TLOOP:	// Do 4 by 4 = 12 colors
		addi	a0,a0,-1
		lq 		t0,0x00(a3)
		lq 		t3,0x10(a3)
		lq 		t6,0x20(a3)
		lq 		t7,0x30(a3)
		pextlh 	t0,zero,t0
		pextlh 	t3,zero,t3
		pextlh 	t6,zero,t6
		pextlh 	t7,zero,t7
		pref	0,0x80(a3)
		psllw  	t0,t0,2
		psllw  	t3,t3,2
		psllw  	t6,t6,2
		psllw  	t7,t7,2
		paddw  	t0,t0,a1
		paddw  	t3,t3,a1
		paddw  	t6,t6,a1
		paddw  	t7,t7,a1
		addiu	a3,a3,0x40
		dsrl32 	t1,t0,0
		dsrl32 	t4,t3,0
		pcpyud  t2,t0,t0
		pcpyud  t5,t3,t3
		lw		t0,0(t0)
		lw		t1,0(t1)
		lw		t2,0(t2)
		lw		t3,0(t3)
		lw		t4,0(t4)
		lw		t5,0(t5)
		pextlw  t0,t1,t0
		pextlw  t2,t3,t2
		pextlw  t4,t5,t4
		pcpyld  t0,t2,t0
		dsrl32 	t1,t6,0
		dsrl32 	t2,t7,0
		pcpyud  t3,t6,t6
		pcpyud  t5,t7,t7
		lw		t6,0(t6)
		lw		t1,0(t1)
		lw		t3,0(t3)
		lw		t7,0(t7)
		lw		t2,0(t2)
		lw		t5,0(t5)
		pextlw  t6,t1,t6
		pextlw  t3,t7,t3
		pextlw  t2,t5,t2
		pcpyld  t4,t6,t4
		pcpyld  t3,t2,t3
		sq		t0,0x00(a2) //
		sq		t4,0x10(a2) //
		sq		t3,0x20(a2)//
		bne 	a0,zero,TLOOP
		addiu	a2,a2,0x30
TLOOP_STOP:
		lw      a0,0(sp)
		andi	a0,a0,3
		beq		a0,zero,TLOOP_END
		nop
TLOOP_Finish:
		lq 		t0,0x00(a3)
		pextlh 	t0,zero,t0
		psllw  	t0,t0,2
		paddw  	t0,t0,a1
		dsrl32 	t1,t0,0
		pcpyud  t2,t0,t0
		addi	a0,a0,-1
		addiu	a3,a3,0x10
		lw		t0,0(t0)
		lw		t1,0(t1)
		lw		t2,0(t2)
		sw		t0,0(a2)
		sw		t1,4(a2)
		sw		t2,8(a2)
		bne		a0,zero,TLOOP_Finish
		addiu	a2,a2,12
TLOOP_END:
		jr		ra
		addiu        sp,sp,16
.set reorder
}


void GSP_DB_Transfert_NO_UV(ULONG Number ,GEO_tdst_IndexedTriangle	*p_CurentT,GEO_Vertex	*_pst_Point,u_int *p_Colors)
{
	register MyVectorFormat 		*p_CurrentPoint;
	register int					lRealNum;
	register int					lRealNum3;
	register Gsp_BigStruct			*p_BIG;
	p_BIG = &gs_st_Globals;
	while ( Number )
	{
		lRealNum = lMin(Number , DP_MaxNumT);
		lRealNum3 = lRealNum + (lRealNum << 1);
		p_CurrentPoint = Gsp_SetDrawBuffer_XYZ_COLORS(p_BIG , lRealNum3 , (u_int *)1);
		GSP_FAST_COPY_TRIS(lRealNum ,_pst_Point , p_CurrentPoint , p_CurentT , p_BIG->p_Last_COLOR_pointer);
		Gsp_SetDrawBuffer_CMD1(p_BIG , lRealNum);
		p_CurentT += lRealNum;
		Number -= lRealNum;
	}
}

void GSP_DB_Transfert_1_UV_PP(ULONG Number ,GEO_tdst_IndexedTriangle	*p_CurentT,GEO_Vertex	*_pst_Point ,GEO_tdst_UV *_pst_UV,u_int *p_Colors)
{
	register MyVectorFormat 		*p_CurrentPoint;
	register int					lRealNum;
	register int					lRealNum3;
	register Gsp_BigStruct			*p_BIG;
	p_BIG = &gs_st_Globals;
	while ( Number )
	{
		lRealNum = lMin(Number , DP_MaxNumT);
		lRealNum3 = lRealNum + (lRealNum << 1);
		p_CurrentPoint = Gsp_SetDrawBuffer_XYZ_COLORS_UV(p_BIG , lRealNum3 , (u_int *)1);
		GSP_FAST_COPY_TRIS_1UVPP(lRealNum ,_pst_Point , p_CurrentPoint , p_CurentT,_pst_UV , p_BIG->p_Last_UV_pointer , p_BIG->p_Last_COLOR_pointer);
		Gsp_SetDrawBuffer_CMD1(p_BIG , lRealNum );
		p_CurentT += DP_MaxNumT;
		Number -= lRealNum;
	}
}
void GSP_DB_Transfert_X_UV_PP(ULONG Number ,GEO_tdst_IndexedTriangle	*p_CurentT,GEO_Vertex	*_pst_Point ,GEO_tdst_UV *_pst_UV,u_int *p_Colors)
{
	register MyVectorFormat 		*p_CurrentPoint;
	register int					lRealNum;
	register int					lRealNum3;
	register Gsp_BigStruct			*p_BIG;
	p_BIG = &gs_st_Globals;

	while ( Number )
	{
		lRealNum = lMin(Number , DP_MaxNumT);
		lRealNum3 = lRealNum + (lRealNum << 1);
		p_CurrentPoint = Gsp_SetDrawBuffer_XYZ_COLORS_UV(p_BIG , lRealNum3 , (u_int *)1);
		GSP_FAST_COPY_TRIS_XUVPP(lRealNum ,_pst_Point , p_CurrentPoint , p_CurentT,_pst_UV , p_BIG->p_Last_UV_pointer , p_BIG->p_Last_COLOR_pointer);
		Gsp_SetDrawBuffer_CMD1(p_BIG , lRealNum);
		p_CurentT += DP_MaxNumT;
		Number -= lRealNum;
	}
}

/* Strip Copier will also copying Colors */
#define GSP_STRIP_DRAW_FLAGS		0<<15
#define GSP_STRIP_DONT_DRAW_FLAGS	1<<15
asm void GSP_FAST_COPY_STRIPS_NO_UV(GEO_Vertex	*_pst_PointSRC , GEO_Vertex	*_pst_PointDST ,USHORT	*p_Tris,USHORT	Number,ULONG *p_Colors)
{
		.set noreorder
		addiu   sp,sp,-0x60
		sq      s0,0x00(sp)
		sq      s1,0x10(sp)
		sq      s2,0x20(sp)
		sq      s3,0x30(sp)
		sq      s4,0x40(sp)
		sq      s5,0x50(sp)
		qmove	s0,zero
		qmove	s3,t0
		ppacw 	a0,a0,a0
		addiu	s0,s0,-1
		ppacw 	a0,a0,a0
		ppacw 	s0,s0,s0
		addiu	a3,a3,3
		ppacw 	s0,s0,s0
		srl		a3,a3,2			// Align on 8
		psllh  	s0,s0,15
S8LOOP:
		lq 		t0,0(a2)		//Load indexes
		qmove	s1,t0
		psllh  	t0,t0,1			// Compute 8 adresses
		pand	s1,s1,s0
		pextuh 	t4,zero,t0
		psrlw	s2,s1,16
		pextlh 	t0,zero,t0
		psllw  	t0,t0,3			// Compute 8 adresses
		psllw  	t4,t4,3
		paddw  	t0,t0,a0
		paddw  	t4,t4,a0		// Compute 8 adresses
		dsrl32	t1,t0,0
		pcpyud  t2,t0,t0
		dsrl32	t3,t2,0
		dsrl32	t5,t4,0
		pcpyud  t6,t4,t4
		dsrl32	t7,t6,0			// Compute 8 adresses -> t0 t1 t2 t3 t4 t5 t6 t7
		lq		t0,0(t0)
		lq		t1,0(t1)
		lq		t2,0(t2)
		lq		t3,0(t3)
		addiu   a3,a3,-1
		pextuw	s4,t1,t0
		sq		t0,0x00(a1)
		sw		s1,0x0c(a1)
		pextuw	s5,t3,t2
		sq		t1,0x10(a1)
		sw		s2,0x1c(a1)
		pcpyud	s4,s4,s5
		dsrl32	s1,s1,0			
		dsrl32	s2,s2,0			
		sq		t2,0x20(a1)
		sw		s1,0x2c(a1)
		sq		t3,0x30(a1)
		sw		s2,0x3c(a1)
		beQ 	a3,zero,S8LOOP_END
		sq      s4,0x00(s3)
		pcpyud  s1,s1,s1
		pcpyud  s2,s2,s2
		lq		t4,0(t4)
		lq		t5,0(t5)
		lq		t6,0(t6)
		lq		t7,0(t7)
		pextuw	s4,t5,t4
		sq		t4,0x40(a1)
		sw		s1,0x4c(a1)
		pextuw	s5,t7,t6
		sq		t5,0x50(a1)
		sw		s2,0x5c(a1)
		pcpyud	s4,s4,s5
		dsrl32	s1,s1,0			
		dsrl32	s2,s2,0			
		sq		t6,0x60(a1)
		sw		s1,0x6c(a1)
		sq		t7,0x70(a1)
		sw		s2,0x7c(a1)
		sq      s4,0x10(s3)
		addiu   a3,a3,-1
		addiu	a2,a2,16
		addiu	s3,s3,0x20
		bne 	a3,zero,S8LOOP
		addiu	a1,a1,0x80
S8LOOP_END:
		lq      	s5,0x50(sp)
		lq      	s4,0x40(sp)
		lq      	s3,0x30(sp)
		lq          s0,0x00(sp)
		lq          s1,0x10(sp)
		lq          s2,0x20(sp)
		jr ra
		addiu       sp,sp,0x60
		.set reorder
}

asm void GSP_FAST_COPY_STRIPS_XYZW(u_long64 *_pst_PointSRC , u_long64	*_pst_PointDST ,USHORT	*p_Tris,USHORT	Number)
{
	/* PARAMS : 
		a0 = _pst_PointSRC 
		a1 = _pst_PointDST ;
		A2 = p_Tris
		a3 = Number 
		t0 = p_Colors
	*/
		.set noreorder
		addiu   sp,sp,-0x30
		sq      s0,0x00(sp)
		sq      s1,0x10(sp)
		sq      s2,0x20(sp)
		qmove	s0,zero
		ppacw 	a0,a0,a0
		addiu	s0,s0,-1
		ppacw 	a0,a0,a0
		ppacw 	s0,s0,s0
		addiu	a3,a3,3
		ppacw 	s0,s0,s0
		srl		a3,a3,2			// Align on 8
		psllh  	s0,s0,15
S8LOOP:
		lq 		t0,0(a2)		//Load indexes
		qmove	s1,t0
		pand	s1,s1,s0
		psrlw	s2,s1,16
		psllh  	t0,t0,1			// Compute 8 adresses
		pextuh 	t4,zero,t0
		pextlh 	t0,zero,t0
		psllw  	t0,t0,3			// Compute 8 adresses
		psllw  	t4,t4,3
		paddw  	t0,t0,a0
		paddw  	t4,t4,a0		// Compute 8 adresses
		dsrl32	t1,t0,0
		pcpyud  t2,t0,t0
		dsrl32	t3,t2,0
		dsrl32	t5,t4,0
		pcpyud  t6,t4,t4
		dsrl32	t7,t6,0			// Compute 8 adresses -> t0 t1 t2 t3 t4 t5 t6 t7
		lq		t0,0(t0)
		lq		t1,0(t1)
		lq		t2,0(t2)
		lq		t3,0(t3)
		addiu   a3,a3,-1
		sq		t0,0x00(a1)
		sw		s1,0x0c(a1)
		sq		t1,0x10(a1)
		sw		s2,0x1c(a1)
		
		dsrl32	s1,s1,0			// Compute 8 adresses -> t0 t1 t2 t3 t4 t5 t6 t7
		dsrl32	s2,s2,0			// Compute 8 adresses -> t0 t1 t2 t3 t4 t5 t6 t7

		sq		t2,0x20(a1)
		sw		s1,0x2c(a1)
		sq		t3,0x30(a1)
		beQ 	a3,zero,S8LOOP_END
		sw		s2,0x3c(a1)
		
		pcpyud  s1,s1,s1
		pcpyud  s2,s2,s2
		lq		t4,0(t4)
		lq		t5,0(t5)
		lq		t6,0(t6)
		lq		t7,0(t7)
		sq		t4,0x40(a1)
		sw		s1,0x4c(a1)
		sq		t5,0x50(a1)
		sw		s2,0x5c(a1)
		
		dsrl32	s1,s1,0			// Compute 8 adresses -> t0 t1 t2 t3 t4 t5 t6 t7
		dsrl32	s2,s2,0			// Compute 8 adresses -> t0 t1 t2 t3 t4 t5 t6 t7
		
		sq		t6,0x60(a1)
		sw		s1,0x6c(a1)
		sq		t7,0x70(a1)
		sw		s2,0x7c(a1)
		addiu   a3,a3,-1
		addiu	a2,a2,16
		bne 	a3,zero,S8LOOP
		addiu	a1,a1,0x80
S8LOOP_END:
		lq           s0,0x00(sp)
		lq           s1,0x10(sp)
		lq           s2,0x20(sp)
		jr ra
		addiu        sp,sp,0x30
		.set reorder
}
asm void GSP_FAST_COPY_STRIPS_UV(u_long64 *_pst_PointSRC , u_long64	*_pst_PointDST ,USHORT	*p_Tris,USHORT	Number)
{
	/* PARAMS : 
		a0 = _pst_PointSRC 
		a1 = _pst_PointDST ;
		A2 = p_Tris
		a3 = Number 
		t0 = p_Colors
	*/
		.set noreorder
		ppacw 	a0,a0,a0
		ppacw 	a0,a0,a0
		addiu	a3,a3,3
		srl		a3,a3,2			// Align on 8
S8LOOP:
		lq 		t0,0(a2)		//Load indexes
		psllh  	t0,t0,1			// Compute 8 adresses
		pextuh 	t4,zero,t0		
		pextlh 	t0,zero,t0
		psllw  	t0,t0,2			// Compute 8 adresses
		psllw  	t4,t4,2
		paddw  	t0,t0,a0
		paddw  	t4,t4,a0		// Compute 8 adresses
		dsrl32	t1,t0,0
		pcpyud  t2,t0,t0
		dsrl32	t3,t2,0
		dsrl32	t5,t4,0
		pcpyud  t6,t4,t4
		dsrl32	t7,t6,0			// Compute 8 adresses -> t0 t1 t2 t3 t4 t5 t6 t7
		ld		t0,0(t0)
		ld		t1,0(t1)
		ld		t2,0(t2)
		ld		t3,0(t3)
		addiu   a3,a3,-1
		pcpyld  t0,t1,t0
		pcpyld  t2,t3,t2
		sq		t0,0x00(a1)
		beQ 	a3,zero,S8LOOP_END
		sq		t2,0x10(a1)
		ld		t4,0(t4)
		ld		t5,0(t5)
		ld		t6,0(t6)
		ld		t7,0(t7)
		pcpyld  t4,t5,t4
		pcpyld  t6,t7,t6
		sq		t4,0x20(a1)
		sq		t6,0x30(a1)
		addiu   a3,a3,-1
		addiu	a2,a2,16
		bne 	a3,zero,S8LOOP
		addiu	a1,a1,0x40
S8LOOP_END:
		jr ra
		nop
		.set reorder
}
asm void GSP_FAST_COPY_STRIPS_CC(u_int *_pst_PointSRC , u_int	*_pst_PointDST ,USHORT	*p_Tris,USHORT	Number)
{
	/* PARAMS : 
		a0 = _pst_PointSRC 
		a1 = _pst_PointDST ;
		A2 = p_Tris
		a3 = Number 
		t0 = p_Colors
	*/
		.set noreorder
		ppacw 	a0,a0,a0
		ppacw 	a0,a0,a0
		addiu	a3,a3,3
		srl		a3,a3,2			// Align on 8
S8LOOP:
		lq 		t0,0(a2)		//Load indexes
		psllh  	t0,t0,1			// Compute 8 adresses
		pextuh 	t4,zero,t0		
		pextlh 	t0,zero,t0
		psllw  	t0,t0,1			// Compute 8 adresses
		psllw  	t4,t4,1
		paddw  	t0,t0,a0
		paddw  	t4,t4,a0		// Compute 8 adresses
		dsrl32	t1,t0,0
		pcpyud  t2,t0,t0
		dsrl32	t3,t2,0
		dsrl32	t5,t4,0
		pcpyud  t6,t4,t4
		dsrl32	t7,t6,0			// Compute 8 adresses -> t0 t1 t2 t3 t4 t5 t6 t7
		lw		t0,0(t0)
		lw		t1,0(t1)
		lw		t2,0(t2)
		lw		t3,0(t3)
		addiu   a3,a3,-1
		pextlw	t0,t1,t0
		pextlw	t2,t3,t2
		pcpyld  t0,t2,t0
		beQ 	a3,zero,S8LOOP_END
		sq		t0,0x00(a1)
		lw		t4,0(t4)
		lw		t5,0(t5)
		lw		t6,0(t6)
		lw		t7,0(t7)
		pextlw	t4,t5,t4
		pextlw  t6,t7,t6
		pcpyld  t4,t6,t4
		addiu   a3,a3,-1
		addiu	a2,a2,16
		sq		t4,0x10(a1)
		bne 	a3,zero,S8LOOP
		addiu	a1,a1,0x20
S8LOOP_END:
		jr ra
		nop
		.set reorder
}
asm void GSP_FAST_COPY_STRIPS_COLORS(u_int *_pst_PointSRC , u_int	*_pst_PointDST ,USHORT	*p_Tris,USHORT	Number  )
{
	/* PARAMS : 
		a0 = _pst_PointSRC 
		a1 = _pst_PointDST ;
		A2 = p_Tris
		a3 = Number 
		t0 = p_Colors
	*/
		.set noreorder
		ppacw 	t8,t0,t0
		ppacw 	t9,t1,t1
		ppacw 	t8,t8,t8
		ppacw 	t9,t9,t9
		
		ppacw 	a0,a0,a0
		ppacw 	a0,a0,a0
		addiu	a3,a3,3
		srl		a3,a3,2			// Align on 8
S8LOOP:
		lq 		t0,0(a2)		//Load indexes
		psllh  	t0,t0,1			// Compute 8 adresses
		pextuh 	t4,zero,t0		
		pextlh 	t0,zero,t0
		psllw  	t0,t0,1			// Compute 8 adresses
		psllw  	t4,t4,1
		paddw  	t0,t0,a0
		paddw  	t4,t4,a0		// Compute 8 adresses
		dsrl32	t1,t0,0
		pcpyud  t2,t0,t0
		dsrl32	t3,t2,0
		dsrl32	t5,t4,0
		pcpyud  t6,t4,t4
		dsrl32	t7,t6,0			// Compute 8 adresses -> t0 t1 t2 t3 t4 t5 t6 t7
		lw		t0,0(t0)
		lw		t1,0(t1)
		lw		t2,0(t2)
		lw		t3,0(t3)
		addiu   a3,a3,-1
		beQ 	a3,zero,S8LOOP_END
		pextlw  t0,t1,t0
		lw		t4,0(t4)
		lw		t5,0(t5)
		lw		t6,0(t6)
		lW		t7,0(t7)
		pextlw  t4,t5,t4
		pextlw  t2,t3,t2
		pextlw  t6,t7,t6
		pcpyld  t0,t2,t0
		pcpyld  t4,t6,t4
		addiu   a3,a3,-1
		addiu	a2,a2,16
		sq		t0,0x00(a1)
		sq		t4,0x10(a1)
		bne 	a3,zero,S8LOOP
		addiu	a1,a1,0x20
		b 		FINISH
		nop
S8LOOP_END:
		pextlw  t2,t3,t2
		pcpyld  t0,t2,t0
		sq		t0,0x00(a1)
FINISH:		
		jr ra
		nop
		.set reorder
}
void GSP_DB_Transfert_XYZW_In_Cache_STRIP(GEO_tdst_ElementIndexedTriangles *p_Elem,GEO_Vertex	*_pst_Point)
{
	register int						lRealNum;
	register int						Number;
	register GSP_GEOCacheBlock 			*pCurrentBlock;
	register GSP_GEOCacheBlock 			**pFreeList;
	register GSP_GEOCacheBlock 			**pListBlock;
	register USHORT						*p_CurentT;
	
	Number 		= p_Elem->pst_StripDataPS2->ulNumberOfAtoms;
	p_CurentT 	= p_Elem->pst_StripDataPS2->pVertexIndexes;
	pFreeList 	= &GspGlobal_ACCESS(gs_st_First_Free_XYZW);
	pListBlock =  &p_Elem->p_ElementCache->p_STRIPED_XYZW_Block_Entry;
	p_Elem->p_ElementCache->CacheFLags &= ~GCF_AllStripXyzAreIn;
	*pListBlock = NULL;
	while ( Number )
	{		
		lRealNum = lMin(Number , DP_MaxNumSTRP);
		pCurrentBlock = *pFreeList;
		if (pCurrentBlock) 
		{
			*pFreeList = pCurrentBlock->p_stNextBlock;
			*pListBlock = pCurrentBlock;
			pListBlock = &((*pListBlock) -> p_stNextBlock);
			*pListBlock  = NULL;
			GSP_FAST_COPY_STRIPS_XYZW(_pst_Point , (MyVectorFormat *)pCurrentBlock->DataBlock ,p_CurentT,lRealNum);
		} else return;
		p_CurentT += lRealNum;
		Number -= lRealNum;
	}
	p_Elem->p_ElementCache->CacheFLags |= GCF_AllStripXyzAreIn;
}
void GSP_DB_Transfert_UV_In_Cache_STRIP(GEO_tdst_ElementIndexedTriangles *p_Elem,GEO_tdst_UV	*_pst_Point)
{
	register int						lRealNum;
	register int						Number;
	register GSP_GEOCacheBlock 			*pCurrentBlock;
	register GSP_GEOCacheBlock 			**pFreeList;
	register GSP_GEOCacheBlock 			**pListBlock;
	register USHORT						*p_CurentT;
	Number 		= p_Elem->pst_StripDataPS2->ulNumberOfAtoms;
	p_CurentT 	= p_Elem->pst_StripDataPS2->pVertexUVIndexes;
	pFreeList 	= &GspGlobal_ACCESS(gs_st_First_Free___UV);
	pListBlock =  &p_Elem->p_ElementCache->p_STRIPED_UV___Block_Entry;
	*pListBlock = NULL;
	p_Elem->p_ElementCache->CacheFLags &= ~GCF_AllStripUVAreIn;
	while ( Number )
	{		
		lRealNum = lMin(Number , DP_MaxNumSTRP);
		pCurrentBlock = *pFreeList;
		if (pCurrentBlock) 
		{
			*pFreeList = pCurrentBlock->p_stNextBlock;
			*pListBlock = pCurrentBlock;
			pListBlock = &((*pListBlock) -> p_stNextBlock);
			*pListBlock  = NULL;
			GSP_FAST_COPY_STRIPS_UV(_pst_Point , (MyVectorFormat *)pCurrentBlock->DataBlock , p_CurentT , lRealNum);
		} else return;
		p_CurentT += lRealNum;
		Number -= lRealNum;
	}
	p_Elem->p_ElementCache->CacheFLags |= GCF_AllStripUVAreIn;
}
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
void GSP_DB_Transfert_CC_In_Cache_STRIP(GEO_tdst_ElementIndexedTriangles *p_Elem,u_int	*_pst_Colors)
{
	register int						lRealNum;
	register int						Number;
	register GSP_GEOCacheBlock 			*pCurrentBlock;
	register GSP_GEOCacheBlock 			**pFreeList;
	register GSP_GEOCacheBlock 			**pListBlock;
	register USHORT						*p_CurentT;
	Number 		= p_Elem->pst_StripDataPS2->ulNumberOfAtoms;
	p_CurentT 	= p_Elem->pst_StripDataPS2->pVertexIndexes;
	pFreeList 	= &GspGlobal_ACCESS(gs_st_First_Free___CC);
	pListBlock =  &p_Elem->p_ElementCache->p_STRIPED_CC___Block_Entry;
	*pListBlock = NULL;
	p_Elem->p_ElementCache->CacheFLags &= ~(GCF_AllStripCCAreIn);
	while ( Number )
	{		
		lRealNum = lMin(Number , DP_MaxNumSTRP);
		pCurrentBlock = *pFreeList;
		if (pCurrentBlock) 
		{
			*pFreeList = pCurrentBlock->p_stNextBlock;
			*pListBlock = pCurrentBlock;
			pListBlock = &((*pListBlock) -> p_stNextBlock);
			*pListBlock  = NULL;
			GSP_FAST_COPY_STRIPS_CC(_pst_Colors , (MyVectorFormat *)pCurrentBlock->DataBlock , p_CurentT , lRealNum);
		} else return;
		p_CurentT += lRealNum;
		Number -= lRealNum;
	}
	p_Elem->p_ElementCache->CacheFLags |= GCF_AllStripCCAreIn;
}
void GSP_DB_Transfert_CC_FUR_In_Cache_STRIP(GEO_tdst_ElementIndexedTriangles *p_Elem,u_int	*_pst_Colors,u_int	*_pst_Compressednormals)
{
	register int						lRealNum;
	register int						Number;
	register GSP_GEOCacheBlock 			*pCurrentBlock;
	register GSP_GEOCacheBlock 			**pFreeList;
	register GSP_GEOCacheBlock 			**pListBlock;
	register USHORT						*p_CurentT;
	Number 		= p_Elem->pst_StripDataPS2->ulNumberOfAtoms;
	p_CurentT 	= p_Elem->pst_StripDataPS2->pVertexIndexes;
	pFreeList 	= &GspGlobal_ACCESS(gs_st_First_Free___CC);
	pListBlock =  &p_Elem->p_ElementCache->p_STRIPED_CC___Block_Entry;
	*pListBlock = NULL;
	p_Elem->p_ElementCache->CacheFLags &= ~(GCF_AllStripCCAreIn);
	while ( Number )
	{		
		lRealNum = lMin(Number , DP_MaxNumSTRP);
		pCurrentBlock = *pFreeList;
		if (pCurrentBlock) 
		{
			*pFreeList = pCurrentBlock->p_stNextBlock;
			*pListBlock = pCurrentBlock;
			pListBlock = &((*pListBlock) -> p_stNextBlock);
			*pListBlock  = NULL;
			GSP_FAST_COPY_STRIPS_CC(_pst_Compressednormals , (MyVectorFormat *)pCurrentBlock->DataBlock , p_CurentT , lRealNum);
		} else return;
		if (_pst_Colors)
		{
			pCurrentBlock = *pFreeList;
			if (pCurrentBlock) 
			{
				*pFreeList = pCurrentBlock->p_stNextBlock;
				*pListBlock = pCurrentBlock;
				pListBlock = &((*pListBlock) -> p_stNextBlock);
				*pListBlock  = NULL;
				GSP_FAST_COPY_STRIPS_CC(_pst_Colors , (MyVectorFormat *)pCurrentBlock->DataBlock , p_CurentT , lRealNum);
			} else return;
		}
		p_CurentT += lRealNum;
		Number -= lRealNum;
	}
	p_Elem->p_ElementCache->CacheFLags |= GCF_AllStripCCAreIn;
}
#endif
void GSP_DB_Transfert_STRIP_1_UV_PP(GEO_tdst_StripDataPS2 *pStrip,GEO_Vertex	*_pst_Point ,GEO_tdst_UV *_pst_UV,u_int *p_Colors)
{
	register GEO_Vertex 		*p_CurrentPoint;
	register int					lRealNum;
	USHORT							*pIdx;
	register ULONG 					Number;
	register Gsp_BigStruct			*p_BIG;
	p_BIG = &gs_st_Globals;
	
	Number = pStrip->ulNumberOfAtoms;
	pIdx = pStrip->pVertexIndexes;
	while (Number)
	{
		lRealNum = lMin (Number , DP_MaxNumSTRP); 
		p_CurrentPoint = (GEO_Vertex*)Gsp_SetDrawBuffer_XYZ_COLORS_UV(p_BIG , (lRealNum + 3) & ~3 , p_Colors);
		if (p_CurrentPoint)					GSP_FAST_COPY_STRIPS_XYZW(_pst_Point , p_CurrentPoint  ,pIdx,lRealNum );//*/);
		if (p_BIG->p_Last_COLOR_pointer)	GSP_FAST_COPY_STRIPS_COLORS(p_Colors , p_BIG->p_Last_COLOR_pointer  ,pIdx,lRealNum );//*/);
		if (p_BIG->p_Last_UV_pointer) 		GSP_FAST_COPY_STRIPS_UV(_pst_UV , p_BIG->p_Last_UV_pointer , pIdx , lRealNum);
		Gsp_SetDrawBuffer_CMD1(p_BIG , lRealNum );
		Number -= lRealNum;
		pIdx += DP_MaxNumSTRP;
	}
}

void GSP_DB_Transfert_STRIP_X_UV_PP(GEO_tdst_StripDataPS2 *pStrip,GEO_Vertex	*_pst_Point ,GEO_tdst_UV *_pst_UV,u_int *p_Colors)
{
	register GEO_Vertex 		*p_CurrentPoint;
	register int					lRealNum;
	USHORT							*pIdx,*pIdxUV;
	register ULONG 					Number;
	register Gsp_BigStruct			*p_BIG;
	p_BIG = &gs_st_Globals;
	Number = pStrip->ulNumberOfAtoms;
	pIdx = pStrip->pVertexIndexes;
	pIdxUV = pStrip->pVertexUVIndexes;
	
	while (Number)
	{
		lRealNum = lMin (Number , DP_MaxNumSTRP); 
		p_CurrentPoint = (GEO_Vertex*)Gsp_SetDrawBuffer_XYZ_COLORS_UV(p_BIG , (lRealNum + 3) & ~3 , p_Colors);
		if (p_CurrentPoint)				GSP_FAST_COPY_STRIPS_XYZW(_pst_Point , p_CurrentPoint  ,pIdx,lRealNum );//*/);
		if (p_BIG->p_Last_COLOR_pointer)GSP_FAST_COPY_STRIPS_COLORS(p_Colors , p_BIG->p_Last_COLOR_pointer  ,pIdx,lRealNum );//*/);
		if (p_BIG->p_Last_UV_pointer) 	GSP_FAST_COPY_STRIPS_UV(_pst_UV , p_BIG->p_Last_UV_pointer , pIdxUV , lRealNum);
		Gsp_SetDrawBuffer_CMD1(p_BIG , lRealNum );
		Number -= lRealNum;
		pIdx += DP_MaxNumSTRP;
		pIdxUV += DP_MaxNumSTRP;
	}
}


void GSP_DB_Transfert_STRIP_X_UV_PP_FUR(ULONG 	Number ,USHORT *pIdx,USHORT *pIdxUV ,GEO_Vertex	*_pst_Point ,GEO_tdst_UV *_pst_UV,u_int *p_Colors,u_int *p_NormalsPtr)
{
	register GEO_Vertex 		*p_CurrentPoint;
	register int					lRealNum;
	register Gsp_BigStruct			*p_BIG;
	p_BIG = &gs_st_Globals;
	if (!p_NormalsPtr) p_NormalsPtr = p_Colors;
	
	while (Number)
	{
		lRealNum = lMin (Number , DP_MaxNumSTRP); 
		p_CurrentPoint = (GEO_Vertex*)Gsp_SetDrawBuffer_XYZ_FUR_COLORS_UV(p_BIG , (lRealNum + 3) & ~3 , p_Colors , (void *)_pst_UV);
		if (p_CurrentPoint)				GSP_FAST_COPY_STRIPS_XYZW(_pst_Point , p_CurrentPoint  ,pIdx,lRealNum );//*/);
		if (p_BIG->p_Last_FUR_pointer)	GSP_FAST_COPY_STRIPS_COLORS(p_NormalsPtr , p_BIG->p_Last_FUR_pointer  ,pIdx,lRealNum );//*/);
		if (p_BIG->p_Last_COLOR_pointer && p_Colors)GSP_FAST_COPY_STRIPS_COLORS(p_Colors , p_BIG->p_Last_COLOR_pointer  ,pIdx,lRealNum );//*/);
		if (p_BIG->p_Last_UV_pointer && _pst_UV) 	GSP_FAST_COPY_STRIPS_UV(_pst_UV , p_BIG->p_Last_UV_pointer , pIdxUV , lRealNum);
		Gsp_SetDrawBuffer_CMD1(p_BIG , lRealNum );
		Number -= lRealNum;
		pIdx += DP_MaxNumSTRP;
		pIdxUV += DP_MaxNumSTRP;
	}
}


void GSP_DB_Transfert_STRIP_NO_UV(GEO_tdst_StripDataPS2 *pStrip,GEO_Vertex	*_pst_Point ,u_int *p_Colors )
{
	register GEO_Vertex 		*p_CurrentPoint;
	register int				lRealNum;
	USHORT						*pIdx;
	register ULONG 				Number;
	
	register Gsp_BigStruct			*p_BIG;
	p_BIG = &gs_st_Globals;
	
	Number = pStrip->ulNumberOfAtoms;
	pIdx = pStrip->pVertexIndexes;
	while (Number)
	{
		lRealNum = lMin( Number , DP_MaxNumSTRP );
		p_CurrentPoint = (GEO_Vertex*)Gsp_SetDrawBuffer_XYZ_COLORS(p_BIG , (lRealNum + 3) & ~3 , p_Colors);
		if (p_CurrentPoint)					GSP_FAST_COPY_STRIPS_XYZW(_pst_Point , p_CurrentPoint  ,pIdx,lRealNum );//*/);
		if (p_BIG->p_Last_COLOR_pointer)	GSP_FAST_COPY_STRIPS_COLORS(p_Colors , p_BIG->p_Last_COLOR_pointer  ,pIdx,lRealNum );//*/);
		Gsp_SetDrawBuffer_CMD1(p_BIG , lRealNum );
		Number -= lRealNum;
		pIdx += DP_MaxNumSTRP;
	}
}

void GSP_v_CopyUsedColors(int ulNumberOfUsedIndexes , unsigned short *p_UsedIndex , GEO_Vertex *_pst_PointDest , int *pColor)
{
	unsigned short *p_LastUsedIndex;
	if (!p_UsedIndex)
	{
		while (ulNumberOfUsedIndexes--) (_pst_PointDest++)->c = *(pColor++);
	} else
	{
		p_LastUsedIndex = p_UsedIndex + ulNumberOfUsedIndexes;
		while (p_UsedIndex < p_LastUsedIndex)
		{
			_pst_PointDest[*(p_UsedIndex++)].c = pColor[*p_UsedIndex];
		}//*/
	}
}

extern void GSP_DrawSpriteBegin();
extern void GSP_DrawSpriteEnd();
extern void GSP_DrawSpriteX(SOFT_tdst_AVertex	*_pst_Point , u_int Color);

void GSP_SetCullingMask(UCHAR c_CullingMask)
{
	gs_st_Globals.ulCurrentCullingMask = (ULONG)c_CullingMask;
}
	
void Gsp_SetDiffuse2XMode()
{
	GspGlobal_ACCESS(Mode2X) |= 1;
}


extern void Gsp_DrawHook_SetFirstStrip(Gsp_BigStruct	  *p_BIG,  u_int NumberOfPrims);
extern void Gsp_SetPlanarProjectionMode(ULONG ProjMode);


LONG GSP_l_DrawElementIndexedTriangles12
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex				*_pst_Point,
	GEO_tdst_UV				*_pst_UV,
	ULONG 					ulNbPoints
);


void GEO_I_Need_The_Triangles_Begin_Element(GEO_tdst_ElementIndexedTriangles    *pst_Element , ULONG FromEnd);
void GEO_I_Need_The_Triangles_End_Element(GEO_tdst_ElementIndexedTriangles    *pst_Element , ULONG FromEnd);
LONG GSP_l_DrawElementIndexedTriangles
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex				*_pst_Point,
	GEO_tdst_UV				*_pst_UV,
	ULONG 					ulNbPoints
)
/*{
	GEO_tdst_StripDataPS2		*pst_StripDataPS2;
	if (_pst_UV)
	{
		
	_pst_UV[0].fU = 0.0f;
	_pst_UV[0].fV = 0.0f;
	_pst_UV[1].fU = 0.0f;
	_pst_UV[1].fV = 1.0f;
	_pst_UV[2].fU = 1.0f;
	_pst_UV[2].fV = 0.0f;
	}
	
	GEO_I_Need_The_Triangles_Begin_Element(_pst_Element , 1);
	
	pst_StripDataPS2 = _pst_Element->pst_StripDataPS2;
	_pst_Element->pst_StripDataPS2 = NULL;
	GSP_l_DrawElementIndexedTriangles12(_pst_Element,_pst_Point,_pst_UV,ulNbPoints);
	_pst_Element->pst_StripDataPS2 = pst_StripDataPS2 ;
	GEO_I_Need_The_Triangles_End_Element(_pst_Element , 1);
}


LONG GSP_l_DrawElementIndexedTriangles12
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex				*_pst_Point,
	GEO_tdst_UV				*_pst_UV,
	ULONG 					ulNbPoints
)//*/
{	
	u_int						*pst_Color/*,*pst_Alpha*/,DoubleCounter;
	SOFT_tdst_ComputingBuffers	*p_ComputingBuffer;
	register Gsp_BigStruct		*p_Local;
	
	GspGlobal_ACCESS(Mode2X) |= 1;
	if (GspGlobal_ACCESS(Rendering2D))
		GspGlobal_ACCESS(Mode2X) &= ~1;

	p_Local = &gs_st_Globals;
//	if (p_Local->ulCurrentCullingMask & (OBJ_Culling_Z_OverLap|OBJ_Culling_XY_OverLap)) return 0;

#ifdef GSP_PS2_BENCH
	GSP_BeginRaster(4);
#endif
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
	p_ComputingBuffer = GDI_gpst_CurDD_SPR.pst_ComputingBuffers;
	p_Local->ulOGLSetCol_XOr = p_ComputingBuffer->ulColorXor;
	

	
	
//	if (_pst_Element->pst_StripDataPS2 && (!_pst_Element->p_ElementCache))	return 0;
	
	if (p_Local->bSRSMustBeUodate) 
	{
		if(p_Local->ulNewBMD & MAT_Cul_Flag_UseLocalAlpha)
			p_Local->ulColorOr = 0xff000000;	
		else
			p_Local->ulColorOr = 0;//*/
	}

#ifdef GSP_PS2_BENCH	
	if ((ShowNormals) && ((ShowNormals & 3) != 2))
	{
		p_Local->Mode2X = 0;
		p_Local->ulColorOr = 0;
	}
#endif
	
	if (p_Local->bPlanarGizmoEnable)	_pst_UV = NULL;
#ifdef GSP_GEO_ACTIVE_CACHE
		/* Add geometrie in Cache */	
	p_Local -> Current_XYZW = NULL;
	p_Local -> Current___UV = NULL;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	p_Local -> Current___CC = NULL;
#endif		
#endif	
				
#ifdef GSP_PS2_BENCH
		if (p_Local->Status& GSP_Status_Show_Depth)
		{
			pst_Color = NULL;
		} else pst_Color = p_ComputingBuffer->CurrentColorField;
#else		
		pst_Color = p_ComputingBuffer->CurrentColorField;
#endif			
		
		if (pst_Color)
		{		
			u_int ColorMad;

			ColorMad = p_Local->ulColorOr >> 31;
			if (ColorMad)
			{
				p_Local->stCurrentCMA.ColorMul.w = 0.0f;
				p_Local->stCurrentCMA.ColorAdd.w = ((float)((((u_int)p_Local->ulOGLSetCol_XOr) ^ 0xff000000) >> 25));
			} else
			{
				if (p_Local->ulOGLSetCol_XOr & 0xff000000)
				{
					p_Local->stCurrentCMA.ColorMul.w = -0.5f;
					p_Local->stCurrentCMA.ColorAdd.w = 128.0f;
				} else
				{
					p_Local->stCurrentCMA.ColorMul.w = 0.5f;
					p_Local->stCurrentCMA.ColorAdd.w = 0.0f;
				}
			} 
			if (p_Local->Mode2X & 1)
				p_Local->stCurrentCMA.ColorMul.x = 	p_Local->stCurrentCMA.ColorMul.y = 	p_Local->stCurrentCMA.ColorMul.z = 	1.0f;
			else
				p_Local->stCurrentCMA.ColorMul.x = 	p_Local->stCurrentCMA.ColorMul.y = 	p_Local->stCurrentCMA.ColorMul.z = 	0.5f;
			p_Local->stCurrentCMA.ColorAdd.x = 	p_Local->stCurrentCMA.ColorAdd.y = 	p_Local->stCurrentCMA.ColorAdd.z = 	0.0f;
			Gsp_SetColorMulAdd(p_Local);
		} else
		{
			u_int ColorMad;
			ColorMad = (p_ComputingBuffer->ul_Ambient | p_Local->ulColorOr) ^ p_Local->ulOGLSetCol_XOr ; 
			p_Local->stCurrentCMA.ColorMul.x = 	p_Local->stCurrentCMA.ColorMul.y = 	p_Local->stCurrentCMA.ColorMul.z = p_Local->stCurrentCMA.ColorMul.w = 0.0f;
			if (p_Local->Mode2X & 2)
				p_Local->stCurrentCMA.ColorAdd.w = ((float)(ColorMad >> 24));
			else
				p_Local->stCurrentCMA.ColorAdd.w = ((float)(ColorMad >> 25));
			if (p_Local->Mode2X & 1)
			{
				p_Local->stCurrentCMA.ColorAdd.x = ((float)((ColorMad & 0xff) >> 0));
				p_Local->stCurrentCMA.ColorAdd.y = ((float)((ColorMad & 0xff00) >> 8));
				p_Local->stCurrentCMA.ColorAdd.z = ((float)((ColorMad & 0xff0000) >> 16));
			}
			else
			{
				p_Local->stCurrentCMA.ColorAdd.x = ((float)((ColorMad & 0xff) >> 1));
				p_Local->stCurrentCMA.ColorAdd.y = ((float)((ColorMad & 0xff00) >> 9));
				p_Local->stCurrentCMA.ColorAdd.z = ((float)((ColorMad & 0xff0000) >> 17));
			}
			Gsp_SetColorMulAdd(p_Local);
		}
		p_Local->Mode2X = 0;

#ifdef GSP_PS2_BENCH
		p_Local->NCalls++;
#endif
	/* ******************************  DRAW ******************************************/
		if (_pst_Element->pst_StripDataPS2)
		{
			p_Local->Gsp_DrawHook2 = p_Local->Gsp_DrawHook;
			p_Local->Gsp_DrawHook = Gsp_DrawHook_SetFirstStrip;//*/
			
			/* Add geometrie in Cache */	
			{
#ifdef GSP_PS2_BENCH
				if (!(p_Local->Status & GSP_Status_Strips_On))
#endif				
				{
					if (_pst_Element->p_ElementCache)
					{
						/* Retreive or store XYZW */
						_pst_Element->p_ElementCache->ulSTRIP_Ponderation_XYZW += _pst_Element->pst_StripDataPS2->ulNumberOfAtoms;
//						if(!(GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer))
						{
							if (!_pst_Element->p_ElementCache->p_STRIPED_XYZW_Block_Entry)
							{
								GSP_DB_Transfert_XYZW_In_Cache_STRIP(_pst_Element,_pst_Point);
							}
							p_Local -> Current_XYZW = _pst_Element->p_ElementCache->p_STRIPED_XYZW_Block_Entry;
						}//*/
						/* Retreive or store UV'S */
						if (_pst_UV)
						{
							if (!(GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint))
							{
								_pst_Element->p_ElementCache->ulSTRIP_Ponderation_UV += _pst_Element->pst_StripDataPS2->ulNumberOfAtoms;
								if (!_pst_Element->p_ElementCache->p_STRIPED_UV___Block_Entry)
								{
									GSP_DB_Transfert_UV_In_Cache_STRIP(_pst_Element,_pst_UV);
								} 
								p_Local -> Current___UV = _pst_Element->p_ElementCache->p_STRIPED_UV___Block_Entry;
							}
						}//*/
						/* Retreive or store UV'S */
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
						if ((pst_Color) && (pst_Color == p_ComputingBuffer->ComputedColors))
						{
							_pst_Element->p_ElementCache->ulSTRIP_Ponderation_CC += _pst_Element->pst_StripDataPS2->ulNumberOfAtoms;
							if (!_pst_Element->p_ElementCache->p_STRIPED_CC___Block_Entry)
							{
								if (p_Local -> pCompresseddNormals)
									GSP_DB_Transfert_CC_FUR_In_Cache_STRIP(_pst_Element,pst_Color,p_Local -> pCompresseddNormals);
								else
									GSP_DB_Transfert_CC_In_Cache_STRIP(_pst_Element,pst_Color);
							} 
							p_Local -> Current___CC = _pst_Element->p_ElementCache->p_STRIPED_CC___Block_Entry;
						}else 
						if (p_Local -> pCompresseddNormals)
						{
							_pst_Element->p_ElementCache->ulSTRIP_Ponderation_CC += _pst_Element->pst_StripDataPS2->ulNumberOfAtoms;
							if (!_pst_Element->p_ElementCache->p_STRIPED_CC___Block_Entry)
							{
								GSP_DB_Transfert_CC_FUR_In_Cache_STRIP(_pst_Element,NULL,p_Local -> pCompresseddNormals);
							} 
							p_Local -> Current___CC = _pst_Element->p_ElementCache->p_STRIPED_CC___Block_Entry;
						}						
#endif						
					}
				} 
			}
	
			
			{
				register u_int SaveNeVUJump;
				SaveNeVUJump = p_Local->ulNextVU1Jump;
				if (p_Local->ulCurrentCullingMask & (OBJ_Culling_Z_OverLap|OBJ_Culling_XY_OverLap))
				{
					p_Local->Current_UV_Compute_VU0_Jump = VU0_JumpTatble[p_Local->bPlanarGizmoEnable];
					p_Local->ulNextVU1Jump |= 0x14; // SHADOW CLIPPED Strips rendering 
				}
				else//*/
				{
					p_Local->Current_UV_Compute_VU0_Jump = VU0_JumpTatble_STRIPS[p_Local->bPlanarGizmoEnable];
					p_Local->ulNextVU1Jump |= 0x10; // SHADOW Strips rendering 
				}
				if (p_Local -> pCompresseddNormals)
				{
					if (GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
					{
						GSP_DB_Transfert_STRIP_X_UV_PP_FUR(_pst_Element->pst_StripDataPS2->ulNumberOfAtoms ,_pst_Element->pst_StripDataPS2->pVertexIndexes ,  _pst_Element->pst_StripDataPS2->pVertexIndexes, _pst_Point , _pst_UV ,pst_Color,p_Local -> pCompresseddNormals);
					} else
						GSP_DB_Transfert_STRIP_X_UV_PP_FUR(_pst_Element->pst_StripDataPS2->ulNumberOfAtoms ,_pst_Element->pst_StripDataPS2->pVertexIndexes ,  _pst_Element->pst_StripDataPS2->pVertexUVIndexes, _pst_Point , _pst_UV ,pst_Color,p_Local -> pCompresseddNormals);
				} else
				{
					if (_pst_UV)
					{
						if (GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint)
						{
							GSP_DB_Transfert_STRIP_1_UV_PP(_pst_Element->pst_StripDataPS2 , _pst_Point , _pst_UV ,pst_Color);
						} else
							GSP_DB_Transfert_STRIP_X_UV_PP(_pst_Element->pst_StripDataPS2 , _pst_Point , _pst_UV ,pst_Color);
					} else
						GSP_DB_Transfert_STRIP_NO_UV(_pst_Element->pst_StripDataPS2 , _pst_Point ,pst_Color);		//*/
				}
				
				p_Local->ulNextVU1Jump = SaveNeVUJump; // SHADOW Strips rendering 
#ifdef GSP_PS2_BENCH
				if ((GspGlobal_ACCESS(Status) & GSP_Status_TNumCounterOn) || ShowNormals)
				{
					USHORT	*p0,*pL;
					p0 = _pst_Element->pst_StripDataPS2->pVertexIndexes;
					pL = p0 + _pst_Element->pst_StripDataPS2->ulNumberOfAtoms;
					while (p0 < pL) p_Local->Tnum += (*(p0++) >> 15) ^ 1;
				}
#endif

			}
				
		} else
		{
			p_Local->Current_UV_Compute_VU0_Jump = VU0_JumpTatble[p_Local->bPlanarGizmoEnable];
			if (pst_Color)
			{
				if ((_pst_Element -> pus_ListOfUsedIndex) && (_pst_Element -> ul_NumberOfUsedIndex < (ulNbPoints >> 1)))
					GSP_v_CopyUsedColors(_pst_Element -> ul_NumberOfUsedIndex , _pst_Element -> pus_ListOfUsedIndex , _pst_Point , pst_Color);	
				else
					GSP_v_CopyUsedColors(ulNbPoints , NULL , _pst_Point , pst_Color);
			}
			if (_pst_UV)
			{
				if(GDI_gpst_CurDD_SPR.ul_DisplayInfo & GDI_Cul_DI_UseOneUVPerPoint) 
					GSP_DB_Transfert_1_UV_PP(_pst_Element->l_NbTriangles , _pst_Element->dst_Triangle , _pst_Point , _pst_UV , pst_Color );
				else
					GSP_DB_Transfert_X_UV_PP(_pst_Element->l_NbTriangles , _pst_Element->dst_Triangle , _pst_Point , _pst_UV , pst_Color );//*/
			}
			else
			{
				GSP_DB_Transfert_NO_UV(_pst_Element->l_NbTriangles , _pst_Element->dst_Triangle , _pst_Point , pst_Color );
			}
		}

#ifdef GSP_PS2_BENCH
	p_Local->Tnum += _pst_Element->l_NbTriangles;
	GSP_EndRaster(4);
#endif
	p_Local->Mode2X = 0;
	
	if (!p_Local -> pCompresseddNormals)
		Gsp_SetPlanarProjectionMode(0);
	
	p_Local -> pCompresseddNormals = NULL;

	
	return 0;
}

LONG GSP_l_DrawElementIndexedTriangles_FUR_FAKE
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex				*_pst_Point,
	GEO_tdst_UV				*_pst_UV,
	ULONG 					ulNbPoints,
	ULONG 					*pNormals
)
{
	extern int					NbrFurLayers;
	extern float				OffsetNormal;
	extern float				OffsetU;
	extern float				OffsetV;
	extern int					FurInvertAlpha;
	u32 Counter,AlT,AlT_Inc,ulNewBMD,ulNewTXT;
	
	if (!pNormals) 	return;

	GspGlobal_ACCESS(stCurrentFUR_SizeInX_Undef_Undef_Undef) . x = 0;
	GspGlobal_ACCESS(stCurrentFUR_SizeInX_Undef_Undef_Undef) . y = 128.0f;
	GspGlobal_ACCESS(pCompresseddNormals) = pNormals;
	GSP_l_DrawElementIndexedTriangles(_pst_Element,_pst_Point,_pst_UV,ulNbPoints);
	GspGlobal_ACCESS(stCurrentFUR_SizeInX_Undef_Undef_Undef) . x = 0;
	GspGlobal_ACCESS(stCurrentFUR_SizeInX_Undef_Undef_Undef) . y = 0;
	Gsp_SetPlanarProjectionMode(0);
	return 0;
}


LONG GSP_l_DrawElementIndexedTriangles_FUR
(
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex				*_pst_Point,
	GEO_tdst_UV				*_pst_UV,
	ULONG 					ulNbPoints,
	ULONG 					*pNormals
)
{
	extern int					NbrFurLayers;
	extern float				OffsetNormal;
	extern float				OffsetU;
	extern float				OffsetV;
	extern int					FurInvertAlpha;
	extern int					DontDrawFirstLayer;
	u32 Counter,AlT,AlT_Inc,ulNewBMD,ulNewTXT;
	Counter = NbrFurLayers;
	
	if (!Counter) return;
	
	if (!pNormals) 
		return;
	
	AlT_Inc = (u32)(256.0f * 65536.0f / (float)NbrFurLayers);
	AlT = 0;
	
	ulNewTXT = GspGlobal_ACCESS(ulNewTXT);
	ulNewBMD = GspGlobal_ACCESS(ulNewBMD);

	GspGlobal_ACCESS(stCurrentFUR_SizeInX_Undef_Undef_Undef) . x = 0;
	GspGlobal_ACCESS(stCurrentFUR_SizeInX_Undef_Undef_Undef) . y = 0;

	if (DontDrawFirstLayer)
	{
		AlT += AlT_Inc;
		Counter--;
		if (!Counter) return;
		GspGlobal_ACCESS(stCurrentFUR_SizeInX_Undef_Undef_Undef) . x += OffsetNormal / 128.0f;
	}
	
	MAT_SET_FLAG(ulNewBMD,(MAT_GET_FLAG(ulNewBMD) | MAT_Cul_Flag_AlphaTest));
	ulNewBMD &= ~MAT_Cc_AlphaTresh_MASK;
	ulNewBMD &= ~MAT_Cul_Flag_InvertAlpha;
	GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ulColorXor ^= 0xff000000;

//	GSP_SetTextureBlending(ulNewTXT, ulNewBMD , 0);
	
	while (Counter--)
	{
		MAT_SET_AlphaTresh(ulNewBMD, (AlT>>16));
		AlT += AlT_Inc;
		GSP_SetTextureBlending(ulNewTXT, ulNewBMD , 0);
		GspGlobal_ACCESS(bSRSMustBeUodate) = 1;
		GspGlobal_ACCESS(stCurrentFUR_SizeInX_Undef_Undef_Undef) . y = 128.0f;
		GspGlobal_ACCESS(pCompresseddNormals) = pNormals;

		Gsp_SetDiffuse2XMode();
		GSP_l_DrawElementIndexedTriangles(_pst_Element,_pst_Point,_pst_UV,ulNbPoints);
		GspGlobal_ACCESS(stCurrentFUR_SizeInX_Undef_Undef_Undef) . x += OffsetNormal / 128.0f;
	}
	GspGlobal_ACCESS(stCurrentFUR_SizeInX_Undef_Undef_Undef) . x = 0;
	GspGlobal_ACCESS(stCurrentFUR_SizeInX_Undef_Undef_Undef) . y = 0;
	GDI_gpst_CurDD_SPR.pst_ComputingBuffers->ulColorXor ^= 0xff000000;
	Gsp_SetPlanarProjectionMode(0);
	return 0;
}


void GSP_l_DrawVertexTable(u_long128 *pBase , u_long64 *pBaseUV , ULONG *pColors , ULONG SX ,ULONG SY)
{
	register u_long128 		*pBaseNext,*pBaseEnd,*pBaseEndLocal,*p_CurrentPoint,*p_CurrentPointLast;
	USHORT						*pIdx;
	register u_long64 		*pUVDest;
	register ULONG 				*pColorDest;
	
	register Gsp_BigStruct			*p_BIG;
	u_int SaveNeVUJump,DP_WTR_Pitch;
	p_BIG = &gs_st_Globals;
	
	
	
	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);
	
	
	{		
		extern void Gsp_SetColorMulAdd(Gsp_BigStruct	  *p_BIG);

		p_BIG->stCurrentCMA.ColorMul.x = 	p_BIG->stCurrentCMA.ColorMul.y = 	p_BIG->stCurrentCMA.ColorMul.z = 	p_BIG->stCurrentCMA.ColorMul.w = 1.0f;
		p_BIG->stCurrentCMA.ColorAdd.x = 	p_BIG->stCurrentCMA.ColorAdd.y = 	p_BIG->stCurrentCMA.ColorAdd.z = 	p_BIG->stCurrentCMA.ColorAdd.w = 0.0f;
		p_BIG->stCurrentCMA.ColorMul.w = 	0.0f;
		p_BIG->stCurrentCMA.ColorAdd.w = 	127.0f;
		Gsp_SetColorMulAdd(p_BIG);
	}
	
	Gsp_SetPlanarProjectionMode(0);
	Gsp_SetUVMatrix_Identity();
	SaveNeVUJump = p_BIG->ulNextVU1Jump;
	
	
	
	p_BIG->Current_UV_Compute_VU0_Jump = VU0_JumpTatble_STRIPS[0];
	p_BIG->Gsp_DrawHook2 = p_BIG->Gsp_DrawHook;
	p_BIG->Gsp_DrawHook = Gsp_DrawHook_SetFirstStrip;
	
	// !!!!!!!!! SX Must be DP_MaxNumSTRP >> 1 !!!!!!!!!!
	DP_WTR_Pitch = SX;
//	if (SX != ((24 >> 1) - 1)) return;
	p_BIG->Tnum += (SX * SY * 2);	
	
	
#ifdef GSP_GEO_ACTIVE_CACHE
	/* no cache */	
	p_BIG -> Current_XYZW = NULL;
	p_BIG -> Current___UV = NULL;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	p_BIG -> Current___CC = NULL;
#endif		
	
#endif	
	pBaseEnd = pBase + (SX * (SY - 1));
	pBaseNext = pBase ;
	
	if (pBaseUV == NULL) pBaseUV = (u_long64 *)pBase;
	
	p_BIG -> ulNextVU1Jump |= 0x10	; // Go to Strips
	
	while (pBase < pBaseEnd)
	{
		u32 COunterK,ToDo;
		COunterK = SX;
		while (COunterK)
		{
			ToDo = lMin(12,COunterK);
			p_CurrentPoint = (u_long128*)Gsp_SetDrawBuffer_XYZ_COLORS_UV(p_BIG , ToDo<<1 , pColors);
			pColorDest = p_BIG->p_Last_COLOR_pointer;
			pUVDest = p_BIG->p_Last_UV_pointer;
			
			pBaseEndLocal = pBase + (ToDo);
			*(p_CurrentPoint) = *pBase;
			((GEO_Vertex*)(p_CurrentPoint++))->c = 1 << 15;
			*(p_CurrentPoint) = *(pBase + DP_WTR_Pitch);
			((GEO_Vertex*)(p_CurrentPoint++))->c = 1 << 15;
			*(pUVDest++) = *pBaseUV;
			*(pUVDest++) = *(pBaseUV + DP_WTR_Pitch);
			*(pColorDest++) = *pColors;
			*(pColorDest++) = *(pColors + DP_WTR_Pitch);
			pBase++;
			pBaseUV++;
			pColors++;
			while (pBase < pBaseEndLocal)
			{
				*(p_CurrentPoint++) = *pBase;
				*(p_CurrentPoint++) = *(pBase + DP_WTR_Pitch);
				*(pUVDest++) = *pBaseUV;
				*(pUVDest++) = *(pBaseUV + DP_WTR_Pitch);
				*(pColorDest++) = *pColors;
				*(pColorDest++) = *(pColors + DP_WTR_Pitch);
				pBase++;
				pBaseUV++;
				pColors++;
			}
			Gsp_SetDrawBuffer_CMD1(p_BIG , ToDo<<1 );
			pBase--;
			pBaseUV--;
			pColors--;
			
			COunterK -= ToDo;
		}
		pBaseNext += DP_WTR_Pitch;
		pColors += pBaseNext - pBase;
		pBaseUV += pBaseNext - pBase;
		pBase = pBaseNext;
	}
	p_BIG->ulNextVU1Jump = SaveNeVUJump;
}

void GSP_l_DrawQuad(u_long128 *pBase , ULONG *pColors)
{
	register u_long128 		*pBaseEnd,*pBaseEndLocal,*p_CurrentPoint,*p_CurrentPointLast;
	USHORT						*pIdx;
	register u_long64 		*pUVDest;
	register ULONG 				*pColorDest;
	
	register Gsp_BigStruct			*p_BIG;
	u_int SaveNeVUJump;
	p_BIG = &gs_st_Globals;

	GSP_Vu1LoadCode(( u_int )&My_Triangle_code);

	
	Gsp_SetPlanarProjectionMode(0);
	SaveNeVUJump = p_BIG->ulNextVU1Jump;
	
	p_BIG->Current_UV_Compute_VU0_Jump = VU0_JumpTatble_STRIPS[0];
	p_BIG->Gsp_DrawHook2 = p_BIG->Gsp_DrawHook;
	p_BIG->Gsp_DrawHook = Gsp_DrawHook_SetFirstStrip;
	
#ifdef GSP_GEO_ACTIVE_CACHE
	/* no cache */	
	p_BIG -> Current_XYZW = NULL;
	p_BIG -> Current___UV = NULL;
#ifdef GSP_GEO_ACTIVE_COLOR_CACHE
	p_BIG -> Current___CC = NULL;
#endif		
	
#endif	
	
	p_BIG -> ulNextVU1Jump |= 0x10	; // Go to Strips
	
	p_CurrentPoint = (u_long128*)Gsp_SetDrawBuffer_XYZ_COLORS_UV(p_BIG , 4 , pColors);
	pColorDest = p_BIG->p_Last_COLOR_pointer;
	pUVDest = p_BIG->p_Last_UV_pointer;
		
	*(p_CurrentPoint) = *(pBase + 0);
	((GEO_Vertex*)(p_CurrentPoint++))->c = 1 << 15;
	*(p_CurrentPoint) = *(pBase + 1);
	((GEO_Vertex*)(p_CurrentPoint++))->c = 1 << 15;
	*(p_CurrentPoint) = *(pBase + 3);
	((GEO_Vertex*)(p_CurrentPoint++))->c = 0;
	*(p_CurrentPoint) = *(pBase + 2);
	((GEO_Vertex*)(p_CurrentPoint++))->c = 0;
	*(pColorDest++) = *pColors;
	*(pColorDest++) = *(pColors + 1);
	*(pColorDest++) = *(pColors + 3);
	*(pColorDest++) = *(pColors + 2);
	Gsp_SetDrawBuffer_CMD1(p_BIG , 4 );
	p_BIG->ulNextVU1Jump = SaveNeVUJump;
}


#if defined PSX2_TARGET && defined __cplusplus
}
#endif

