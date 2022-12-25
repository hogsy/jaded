#ifndef __CW__
#define zero	$0	/* wired zero */
#define AT	$at	/* assembler temp */
#define v0	$2	/* return value */
#define v1	$3
#define a0	$4	/* argument registers */
#define a1	$5
#define a2	$6
#define a3	$7
#define t0	$8	/* caller saved */
#define t1	$9
#define t2	$10
#define t3	$11
#define t4	$12
#define t5	$13
#define t6	$14
#define t7	$15
#define s0	$16	/* callee saved */
#define s1	$17
#define s2	$18
#define s3	$19
#define s4	$20
#define s5	$21
#define s6	$22
#define s7	$23
#define t8	$24	/* code generator */
#define t9	$25
#define k0	$26	/* kernel temporary */
#define k1	$27
#define gp	$28	/* global pointer */
#define sp	$29	/* stack pointer */
#define fp	$30	/* frame pointer */
#define ra	$31	/* return address */
#endif

#ifdef __mips16
	.set nomips16	/* This file contains 32 bit assembly code. */
#endif
    .set noat
   	.set noreorder
	.text			



/*
 =======================================================================================================================
 =======================================================================================================================
 */
    .extern ps2MATH_vmask1
    .extern ps2MATH_vmask2
    .global asm_MATH_GetRotationMatrix
    .ent asm_MATH_GetRotationMatrix
asm_MATH_GetRotationMatrix:
    lui     t0, %hi(ps2MATH_vmask1)
    lq      t1, 0(a1)
    addiu   t6, t0, %lo(ps2MATH_vmask1)
    lq      t2, 16(a1)
    lq      t0, 0(t6)
    lq      t3, 32(a1)
    pand    t4, t0, t1
    pand    t5, t0, t2
    pand    t6, t0, t3
    sq      t4, 0(a0)
    sq      t5, 16(a0)
    sq      t6, 32(a0)
    lui     t1, %hi(ps2MATH_vmask2)
    lui     t2, %hi(0xfffffff5)
    lw      t6, 64(a0)
    addiu   t3, t2, %lo(0xfffffff5)
    addiu   t0, t1, %lo(ps2MATH_vmask2)
    and     t4, t3, t6
    lq      t1, 0(t0)
    ori     t3, t4, 0x4
    sq      t1, 48(a0)
    jr      ra
    sw      t3, 64(a0)
    .end asm_MATH_GetRotationMatrix

/*
 =======================================================================================================================
 Serial mul matrix vertex, used in GEO_SKN.C
 a0 = DST
 a1 = SRC
 a2 = Matrix
 a3	= PonDeRATIONs number
 t0	= PonDeRATIONs
 =======================================================================================================================
 */
	.extern Fast_Skin
	.global asm_MATH_SerialSkin
	.ent asm_MATH_SerialSkin
asm_MATH_SerialSkin:
	addi	sp, sp, -(16*2)
	sq	v0,16*0(sp)
	sq	v1,16*1(sp)
	/* Load MAtrix */
	lqc2    $vf05, 0(a2)
	lqc2    $vf06, 16(a2)
	lqc2    $vf07, 32(a2)
	lqc2    $vf08, 48(a2)
	addiu 	a3,a3,3
	ppacw 	a0,a0,a0
	ppacw 	a0,a0,a0
	ppacw 	a1,a1,a1
	ppacw 	a1,a1,a1
	addiu 	a2,t0,0
	srl 	a3,a3,2

	lq 		v0,0(a2)
	qmtc2 	v0,$vf02
	pinteh 	v0,zero,v0
	psllw 	v0,v0,4 
	paddw 	t0,v0,a1
	dsrl32 	t1,t0,0
	pcpyud  t2,t0,t0
	dsrl32	t3,t2,0
	paddw   t4,v0,a0
	dsrl32	t5,t4,0
	pcpyud  t6,t4,t4
	dsrl32	t7,t6,0
	/* Load Point */
	lqc2 	$vf15,0(t0)
	lqc2 	$vf16,0(t1)
	lqc2 	$vf17,0(t2)
	lqc2 	$vf18,0(t3)
	/* Load Destination Point */
	lqc2 	$vf20,0(t4)
	lqc2 	$vf21,0(t5)
	lqc2 	$vf22,0(t6)
	lqc2 	$vf23,0(t7)
	VCALLMS	Fast_Skin
	nop
	nop
	nop
ASM_SKN_LOOP: // Treat 4 by 4 ! 
	addi a3,a3,-1
	addiu a2,a2,16
	lq 		v0,0(a2)
	qmtc2 	v0,$vf02
	pinteh 	v0,zero,v0
	psllw 	v0,v0,4 
	paddw 	t0,v0,a1
	dsrl32 	t1,t0,0
	pcpyud  t2,t0,t0	
	dsrl32	t3,t2,0
	lqc2 	$vf15,0(t0)
	lqc2 	$vf16,0(t1)
	lqc2 	$vf17,0(t2)
	lqc2 	$vf18,0(t3)
	qmfc2.i	v1,$vf20
	sq 		v1,0(t4)
	sqc2 	$vf21,0(t5)
	sqc2 	$vf22,0(t6)
	sqc2 	$vf23,0(t7)
	paddw   t4,v0,a0
	dsrl32	t5,t4,0
	pcpyud  t6,t4,t4
	dsrl32	t7,t6,0
	lqc2 	$vf20,0(t4)
	lqc2 	$vf21,0(t5)
	lqc2 	$vf22,0(t6)
	lqc2 	$vf23,0(t7)
	VCALLMS	Fast_Skin
	bne a3,zero,ASM_SKN_LOOP
	nop
ASM_SKN_END:		
	qmfc2.i	v1,$vf20
	lq	v0,16*0(sp)
	lq	v1,16*1(sp)
	jr 		ra
	addi	sp, sp, (16*2)
	.end asm_MATH_SerialSkin

/*
/*
 =======================================================================================================================
 Serial mul matrix vertex, used in GEO_SKN.C
 a0 = DST
 a1 = SRC
 a2 = Matrix
 a3	= PonDeRATIONs number
 t0	= PonDeRATIONs
 =======================================================================================================================
 */
	.extern Fast_Skin
	.global asm_MATH_SerialSkin_NRM
	.ent asm_MATH_SerialSkin_NRM
asm_MATH_SerialSkin_NRM:
	addi	sp, sp, -(16*6)
	sq	v0,16*0(sp)
	sq	v1,16*1(sp)
	sq	s0,16*2(sp)
	sq	s1,16*3(sp)
	sq	s2,16*4(sp)
	sq	s3,16*5(sp)
	/* Load MAtrix */
	lqc2    $vf05, 0(a2)
	lqc2    $vf06, 16(a2)
	lqc2    $vf07, 32(a2)
	lqc2    $vf08, 48(a2)
	vmulx.xyzw $vf08, $vf08, $vf00x
	addiu 	a3,a3,3
	ppacw 	a0,a0,a0
	ppacw 	a0,a0,a0
	ppacw 	a1,a1,a1
	ppacw 	a1,a1,a1
	addiu 	a2,t0,0
	srl 	a3,a3,2

	lq 		v0,0(a2)
	qmtc2 	v0,$vf02
	pinteh 	v0,zero,v0
	psllw 	v0,v0,3 
	paddw 	t0,v0,a1
	dsrl32 	t1,t0,0
	pcpyud  t2,t0,t0
	dsrl32	t3,t2,0
	paddw   t4,v0,a0
	dsrl32	t5,t4,0
	pcpyud  t6,t4,t4
	dsrl32	t7,t6,0
	/* Load Point */
	ld 		s0,0(t0)
	ld 		s1,0(t1)
	ld 		s2,0(t2)
	ld 		s3,0(t3)
	pextlh s0 , s0 , zero
	pextlh s1 , s1 , zero
	pextlh s2 , s2 , zero
	pextlh s3 , s3 , zero
	qmtc2 	s0,$vf15
	qmtc2 	s1,$vf16
	qmtc2 	s2,$vf17
	qmtc2 	s3,$vf18
	/* Load Destination Point */
	ld 		s0,0(t4)
	ld 		s1,0(t5)
	ld 		s2,0(t6)
	ld 		s3,0(t7)
	pextlh s0 , s0 , zero
	pextlh s1 , s1 , zero
	pextlh s2 , s2 , zero
	pextlh s3 , s3 , zero
	qmtc2 	s0,$vf20
	qmtc2 	s1,$vf21
	qmtc2 	s2,$vf22
	qmtc2 	s3,$vf23
	VCALLMS	Fast_Skin
	nop
	nop
	nop
	nop
	nop
	nop
ASM_SKN_NRM_LOOP: // Treat 4 by 4 ! 
	addi a3,a3,-1
	addiu a2,a2,16
	lq 		v0,0(a2)
	qmtc2 	v0,$vf02
	pinteh 	v0,zero,v0
	psllw 	v0,v0,3
	paddw 	t0,v0,a1
	dsrl32 	t1,t0,0
	pcpyud  t2,t0,t0	
	dsrl32	t3,t2,0
	ld 		s0,0(t0)
	ld 		s1,0(t1)
	ld 		s2,0(t2)
	ld 		s3,0(t3)
	pextlh s0 , s0 , zero
	pextlh s1 , s1 , zero
	pextlh s2 , s2 , zero
	pextlh s3 , s3 , zero
	qmtc2 	s0,$vf15
	qmtc2 	s1,$vf16
	qmtc2 	s2,$vf17
	qmtc2 	s3,$vf18
	qmfc2.i	s0,$vf20
	qmfc2	s1,$vf21
	qmfc2	s2,$vf22
	qmfc2	s3,$vf23
	psrlw 	s0 , s0 , 16
	psrlw 	s1 , s1 , 16
	psrlw 	s2 , s2 , 16
	psrlw 	s3 , s3 , 16
	ppach  	s0 , zero , s0
	ppach  	s1 , zero , s1
	ppach  	s2 , zero , s2
	ppach  	s3 , zero , s3
	sd 		s0,0(t4)
	sd 		s1,0(t5)
	sd 		s2,0(t6)
	sd 		s3,0(t7)
	paddw   t4,v0,a0
	dsrl32	t5,t4,0
	pcpyud  t6,t4,t4
	dsrl32	t7,t6,0
	ld 		s0,0(t4)
	ld 		s1,0(t5)
	ld 		s2,0(t6)
	ld 		s3,0(t7)
	pextlh s0 , s0 , zero
	pextlh s1 , s1 , zero
	pextlh s2 , s2 , zero
	pextlh s3 , s3 , zero
	qmtc2 	s0,$vf20
	qmtc2 	s1,$vf21
	qmtc2 	s2,$vf22
	qmtc2 	s3,$vf23
	VCALLMS	Fast_Skin
	bne a3,zero,ASM_SKN_NRM_LOOP
	nop
ASM_SKN_NRM_END:	
	qmfc2.i	v1,$vf20	
	lq	s3,16*5(sp)
	lq	s2,16*4(sp)
	lq	s1,16*3(sp)
	lq	s0,16*2(sp)
	lq	v1,16*1(sp)
	lq	v0,16*0(sp)
	jr 		ra
	addi	sp, sp, (16*6)
	.end asm_MATH_SerialSkin

/*
 =======================================================================================================================
 GSP_ASM_ComputeDirect, used in Lightstruct.C
PARAMS = 
		a0  = pst_N1
		a1	= Number
		a2	= pst_Dir 
		a3	= pu_ulColors 
		t0	= COLOR
 =======================================================================================================================
 */
	.global GSP_ASM_ComputeDirect
	.extern VU0_CompteDirLight
	.ent GSP_ASM_ComputeDirect
GSP_ASM_ComputeDirect:
	mtsah 		zero, 4
	PEXTLB		t0, zero, t0	/* Extend light Color */
	PEXTLB		t0, zero, t0	/* Extend light Color */
	qmtc2    	t0,	$vf02	
	VITOF0		$vf02,$vf02		/* VF02 = Light Color */
	lw		t0,	8(a2)			/* Load pst_Dir  (non aligned)	*/
	qfsrv 	t0, t0, t0			/* Load pst_Dir  (non aligned)	*/
	ldl		t0,	7(a2)			/* Load pst_Dir  (non aligned)	*/
	ldr		t0,	0(a2)			/* Load pst_Dir  (non aligned)	*/
	qmtc2	t0, $vf01			/* VF01 = pst_Dir */
	lq	t0,0(a0)		/* t0 =  x1  z0   y0  x0 */
	lq	t1,16(a0)		/* t1 =  y2  x2   z1  y1 */
	lq	t2,32(a0)		/* t2 =  z3  y3   x3  z2 */
	pref    0,64(a0)
	prot3w	t3,t0		/* t3 = [x1  x0]  z0  y0 */
	prot3w	t4,t2		/* t4 =  z3  z2   y3  x3 */
	pextlw	t5,t1,t3	/* t5 = [z1  z0] [y1  y0] */
	pcpyud	t6,t1,t1	/* t6 =  y2  x2   y2  x2 */
	pextlw	t7,t4,t6	/* t7 = [y3  y2] [x3  x2] */
	pcpyud  t2,t5,t4    /* t2 = [z3  z2   z1  z0] */
	pcpyud  t3,t3,t3	/* t3 = [x1  x0] [x1  x0] */
	pcpyld  t5,t5,t5	/* t5 = [y1  y0] [y1  y0] */
	pcpyud  t1,t5,t7	/* t1 = [y3  y2] [y1  y0] */
	pcpyld  t0,t7,t3	/* t0 = [x3  x2] [x1  x0] */
	pref    0,64(a3)
	qmtc2	t0,$vf10
	qmtc2	t1,$vf11
	qmtc2	t2,$vf12
	vcallms VU0_CompteDirLight
	addiu  	a0,a0,3*16
	srl 	a1,a1,2
	beq		zero, zero, DIRECT_LIGHT_LOOP_ENTRY
	addiu	a1,a1,1	
DIRECT_LIGHT_LOOP:	 // treat 4 by 4 ! 
	lq	t0,0(a0)		/* t0 =  x1  z0   y0  x0 */
	lq	t1,16(a0)		/* t1 =  y2  x2   z1  y1 */
	lq	t2,32(a0)		/* t2 =  z3  y3   x3  z2 */
	pref    0,64(a0)
	prot3w	t3,t0		/* t3 = [x1  x0]  z0  y0 */
	prot3w	t4,t2		/* t4 =  z3  z2   y3  x3 */
	pextlw	t5,t1,t3	/* t5 = [z1  z0] [y1  y0] */
	pcpyud	t6,t1,t1	/* t6 =  y2  x2   y2  x2 */
	pextlw	t7,t4,t6	/* t7 = [y3  y2] [x3  x2] */
	pcpyud  t2,t5,t4    /* t2 = [z3  z2   z1  z0] */
	pcpyud  t3,t3,t3	/* t3 = [x1  x0] [x1  x0] */
	pcpyld  t5,t5,t5	/* t5 = [y1  y0] [y1  y0] */
	pcpyud  t1,t5,t7	/* t1 = [y3  y2] [y1  y0] */
	pcpyld  t0,t7,t3	/* t0 = [x3  x2] [x1  x0] */
	pref    0,64(a3)
	qmfc2.i	t4,$vf20
	qmfc2	t5,$vf21
	qmfc2	t6,$vf22
	qmfc2	t7,$vf23
	qmtc2	t0,$vf10
	qmtc2	t1,$vf11
	qmtc2	t2,$vf12
	vcallms VU0_CompteDirLight
	ppach	t4, t5,t4			/* COMPACT COLORS  */
	lq		t2,0(a3)			/* Add to dest  */
	ppach	t6, t7,t6
	ppacb	t4, t6,t4
	addiu  	a3,a3,16
	paddub	t0, t4,t2
	addiu  	a0,a0,3*16
	sq	t0,-16(a3)
DIRECT_LIGHT_LOOP_ENTRY:
	bne a1, $0, DIRECT_LIGHT_LOOP
	addi	a1,a1,-1
	jr ra
	nop
	.end GSP_ASM_ComputeDirect

/*
 =======================================================================================================================
 GSP_ASM_ComputeDirect, used in Lightstruct.C
PARAMS = 
		a0  = pst_N1
		a1	= Number
		a2	= pst_Dir 
		a3	= pu_ulColors 
		t0	= COLOR
 =======================================================================================================================
 */
	.global GSP_ASM_ComputeDirect_C
	.extern VU0_CompteDirLight_C
	.ent GSP_ASM_ComputeDirect_C
GSP_ASM_ComputeDirect_C:
	mtsah 		zero, 4
	PEXTLB		t0, zero, t0	/* Extend light Color */
	addi		t8 , zero , -1
	PEXTLB		t0, zero, t0	/* Extend light Color */
	PEXTLB		t8, t8 ,zero 	/* Extendmask */		
	qmtc2    	t0,	$vf02	
	PEXTLH		t8, zero, t8	/* Extendmask */
	VITOF0		$vf02,$vf02		/* VF02 = Light Color */
	lw			t0,	8(a2)			/* Load pst_Dir  (non aligned)	*/
	qfsrv 		t0, t0, t0			/* Load pst_Dir  (non aligned)	*/
	ldl			t0,	7(a2)			/* Load pst_Dir  (non aligned)	*/
	ldr			t0,	0(a2)			/* Load pst_Dir  (non aligned)	*/
	qmtc2		t0, $vf01			/* VF01 = pst_Dir */
	lq			t0,0(a0)		/* t0 =  x1  z0   y0  x0 */
	pref    	0,64(a0)
	psrlw		t2,t0,8
	psrlw		t1,t0,0
	psllw		t0,t0,8
	pand		t2,t2,t8
	pand		t1,t1,t8
	pand		t0,t0,t8
	pref    	0,64(a3)
	qmtc2		t0,$vf10
	qmtc2		t1,$vf11
	qmtc2		t2,$vf12
	vcallms 	VU0_CompteDirLight_C
	addiu  		a0,a0,16
	srl 		a1,a1,2
	beq			zero, zero, DIRECT_LIGHT_LOOP_ENTRY_C
	addiu		a1,a1,1	
DIRECT_LIGHT_LOOP_C:	 // treat 4 by 4 ! 
	lq			t0,0(a0)		/* t0 =  x1  z0   y0  x0 */
	pref    	0,64(a0)
	psrlw		t2,t0,8
	psrlw		t1,t0,0
	psllw		t0,t0,8
	pand		t2,t2,t8
	pand		t1,t1,t8
	pand		t0,t0,t8
	pref    	0,64(a3)
	qmtc2.i		t0,$vf10
	qmtc2		t1,$vf11
	qmtc2		t2,$vf12
	qmfc2		t4,$vf20
	qmfc2		t5,$vf21
	qmfc2		t6,$vf22
	qmfc2		t7,$vf23
	vcallms 	VU0_CompteDirLight_C
	ppach		t4, t5,t4			/* COMPACT COLORS  */
	lq			t2,0(a3)			/* Add to dest  */
	ppach		t6, t7,t6
	ppacb		t4, t6,t4
	addiu  		a3,a3,16
	paddub		t0, t4,t2
	addiu  		a0,a0,16
	sq			t0,-16(a3)
DIRECT_LIGHT_LOOP_ENTRY_C:
	bne 		a1, $0, DIRECT_LIGHT_LOOP_C
	addi		a1,a1,-1
	jr 			ra
	nop
	.end GSP_ASM_ComputeDirect_C

/*
 =======================================================================================================================
 asm_MATH_SerialSphericalLight, used in Lightstruct.C
 p0 = Vertex Source base 	(Aligned)
 p1 = Normales Base		(non Aligned)
 p2 = Number Of Points		
 p3 = Point Light coordinate	(Aligned)
 p4 = 1.0f / (1/near - 1/far)	(float)
 p5 = 1/near			(float)
 p6 = Color			(Aligned)
 p7 = Color dest
 =======================================================================================================================
 */
	.global asm_MATH_SerialSphericalLight_C
	.extern PS2_ComputeLightSperical
	.ent asm_MATH_SerialSphericalLight_C
asm_MATH_SerialSphericalLight_C:
	addiu	sp, sp, -(16 * 8)
	sq	a0,00(sp)
	sq	a1,96(sp)
	sq	t0,16(sp)
	sq	t1,32(sp)
	sq	t2,48(sp)
	sq	t3,64(sp)
	sq	t4,80(sp)
	sq	t5,112(sp)
	
	mtsah 		zero, 4

	lw		t0,	12(a0)					/* Point Light Coordinate */
	lqc2    	$vf01, 	0(t0)
	lw		t0,	24(a0)					/* Point Light Color */

	PEXTLB		t0, zero, t0			/* Extend light Color */
	PEXTLB		t0, zero, t0			/* Extend light Color */
	qmtc2    	t0,	$vf02
	VITOF0		$vf02,$vf02
	
	lw		t0,	16(a0)					/* Factor 1 */
	qmtc2 		t0, 	$vf03
	lw		t0,	20(a0)					/* Factor 2 */
	qmtc2 		t0, 	$vf04

	lw		t0,	0(a0)					/* Vertex source base */
	lw		t1,	4(a0)					/* Normales source base */
	lw		t2,	8(a0)					/* Number of points */
	lw		t3,	28(a0)					/* Color dest Base */
	lqc2    	$vf10, 	0(t0)  			/* Load Point */
	
	beq     	t2 , 	zero , CLS_Finish_C
	nop

	lqc2    	$vf10, 	0(t0)  			/* Load Point n°1 */
	vcallms 	PS2_ComputeLightSperical_C
	qmfc2.i 	t4, 	$vf12
	
	lqc2    	$vf10, 	16(t0)  		/* Load Point n°2 */
	
	lw			t5,	0(t1)					/* Load first Normale (non aligned)	*/
	pextlb		t5, zero , t5
	pextlh		t5, zero , t5
	psllw		t5, t5 , 8
	
	
	qmtc2 		t5, 	$vf11   		/* Load first Normale in VU0 		*/
	vitof15		$vf11 , $vf11
	vcallms 	PS2_ComputeLightSperical_C
	qmfc2.i 	t4, 	$vf12
	
	addi 		t1,	t1,4
	
	lqc2    	$vf10, 	32(t0)  		/* Load Point n°3 */
	
	lw			t5,	0(t1)					/* Load first Normale (non aligned)	*/
	pextlb		t5, zero , t5
	pextlh		t5, zero , t5
	psllw		t5, t5 , 8
	
	qmtc2 		t5, 	$vf11   		/* Load first Normale in VU0 		*/
	vitof15		$vf11 , $vf11
	vcallms 	PS2_ComputeLightSperical_C
	qmfc2.i 	t4, 	$vf12
	
	
	addi 		t1,	t1,4
	addi 		t0,	t0,32

CLS_Loop_C:

	lw			t5,	0(t1)					/* Load next+2 Normale (non aligned)	*/
	addi 		t2,	t2,-1
	pextlb		t5, zero , t5
	pref		0,64(t0)
	pextlh		t5, zero , t5
	psllw		t5, t5 , 8
	qmfc2.i 	t4, 	$vf12			/* Get color 				*/
	qmtc2 		t5, 	$vf11   		/* Load next+2 Normale in VU0*/ 		
	lqc2    	$vf10, 	16(t0)  		/* Load Next+3 Point in VU0 */		
	vitof15		$vf11 , $vf11
	vcallms 	PS2_ComputeLightSperical_C/* call VU0 				*/
	PPACB		t4, 	zero, t4		/* Pack Color Result 		*/	
	lw			t5,	0(t3)					/* Load color 				*/
	lw			zero,128(t0)
	PPACB		t4, 	zero, t4		/* Pack Color Result 		*/	
	addi 		t1,	t1,4
	addi 		t0,	t0,16
	PADDUB		t4,	t4,t5				/* Add Color to colors dest 		*/
	sw		t4,	0(t3)					/* Save Color 				*/
	bne     	t2 , 	zero , CLS_Loop_C
	addi 		t3,	t3,4

CLS_Finish_C:
	lq		a0,00(sp)
	lq		a1,96(sp)
	lq		t0,16(sp)
	lq		t1,32(sp)
	lq		t2,48(sp)
	lq		t3,64(sp)
	lq		t4,80(sp)
	lq		t5,112(sp)
	jr		ra
	addiu	sp, sp, (16 * 8)	
	.end asm_MATH_SerialSphericalLight

/*
 =======================================================================================================================
 asm_MATH_SerialSphericalLight, used in Lightstruct.C
 p0 = Vertex Source base 	(Aligned)
 p1 = Normales Base		(non Aligned)
 p2 = Number Of Points		
 p3 = Point Light coordinate	(Aligned)
 p4 = 1.0f / (1/near - 1/far)	(float)
 p5 = 1/near			(float)
 p6 = Color			(Aligned)
 p7 = Color dest
 =======================================================================================================================
 */
	.global asm_MATH_SerialSphericalLight
	.extern PS2_ComputeLightSperical
	.ent asm_MATH_SerialSphericalLight
asm_MATH_SerialSphericalLight:
	addiu	sp, sp, -(16 * 8)
	sq	a0,00(sp)
	sq	a1,96(sp)
	sq	t0,16(sp)
	sq	t1,32(sp)
	sq	t2,48(sp)
	sq	t3,64(sp)
	sq	t4,80(sp)
	sq	t5,112(sp)
	
	mtsah 		zero, 4

	lw		t0,	12(a0)					/* Point Light Coordinate */
	lqc2    	$vf01, 	0(t0)
	lw		t0,	24(a0)					/* Point Light Color */

	PEXTLB		t0, zero, t0			/* Extend light Color */
	PEXTLB		t0, zero, t0			/* Extend light Color */
	qmtc2    	t0,	$vf02
	VITOF0		$vf02,$vf02
	
	lw		t0,	16(a0)					/* Factor 1 */
	qmtc2 		t0, 	$vf03
	lw		t0,	20(a0)					/* Factor 2 */
	qmtc2 		t0, 	$vf04

	lw		t0,	0(a0)					/* Vertex source base */
	lw		t1,	4(a0)					/* Normales source base */
	lw		t2,	8(a0)					/* Number of points */
	lw		t3,	28(a0)					/* Color dest Base */
	lqc2    	$vf10, 	0(t0)  			/* Load Point */
	
	beq     	t2 , 	zero , CLS_Finish
	nop

	lqc2    	$vf10, 	0(t0)  			/* Load Point n°1 */
	vcallms 	PS2_ComputeLightSperical
	qmfc2.i 	t4, 	$vf12
	
	lqc2    	$vf10, 	16(t0)  		/* Load Point n°2 */
	lw		t5,	8(t1)					/* Load first Normale (non aligned)	*/
	qfsrv 		t5, 	t5, t5			/* Load first Normale (non aligned)	*/
	ldl		t5,	7(t1)					/* Load first Normale (non aligned)	*/
	ldr		t5,	0(t1)					/* Load first Normale (non aligned)	*/
	qmtc2 		t5, 	$vf11   		/* Load first Normale in VU0 		*/
	vcallms 	PS2_ComputeLightSperical
	qmfc2.i 	t4, 	$vf12
	
	addi 		t1,	t1,12
	
	lqc2    	$vf10, 	32(t0)  		/* Load Point n°3 */
	lw		t5,	8(t1)					/* Load first Normale (non aligned)	*/
	qfsrv 		t5, 	t5, t5			/* Load first Normale (non aligned)	*/
	ldl		t5,	7(t1)					/* Load first Normale (non aligned)	*/
	ldr		t5,	0(t1)					/* Load first Normale (non aligned)	*/
	qmtc2 		t5, 	$vf11   		/* Load first Normale in VU0 		*/
	vcallms 	PS2_ComputeLightSperical
	qmfc2.i 	t4, 	$vf12
	
	
	addi 		t1,	t1,12
	addi 		t0,	t0,32

CLS_Loop:
	lw			t5,	8(t1)					/* Load next+2 Normale (non aligned)	*/
	qfsrv 		t5, 	t5, t5			/* Load next+2 Normale (non aligned)	*/
	addi 		t2,	t2,-1
	ldl			t5,	7(t1)					/* Load next+2 Normale (non aligned)	*/
	ldr			t5,	0(t1)					/* Load next+2 Normale (non aligned)	*/
	lw			zero,128(t1)
	qmfc2.i 	t4, 	$vf12			/* Get color 				*/
	qmtc2 		t5, 	$vf11   		/* Load next+2 Normale in VU0*/ 		
	lqc2    	$vf10, 	16(t0)  		/* Load Next+3 Point in VU0 */		
	vcallms 	PS2_ComputeLightSperical/* call VU0 				*/
	PPACB		t4, 	zero, t4		/* Pack Color Result 		*/	
	lw			t5,	0(t3)					/* Load color 				*/
	lw			zero,128(t0)
	PPACB		t4, 	zero, t4		/* Pack Color Result 		*/	
	addi 		t1,	t1,12
	addi 		t0,	t0,16
	PADDUB		t4,	t4,t5				/* Add Color to colors dest 		*/
	sw		t4,	0(t3)					/* Save Color 				*/
	bne     	t2 , 	zero , CLS_Loop
	addi 		t3,	t3,4

CLS_Finish:
	lq		a0,00(sp)
	lq		a1,96(sp)
	lq		t0,16(sp)
	lq		t1,32(sp)
	lq		t2,48(sp)
	lq		t3,64(sp)
	lq		t4,80(sp)
	lq		t5,112(sp)
	jr		ra
	addiu	sp, sp, (16 * 8)	
	.end asm_MATH_SerialSphericalLight

/*
 =======================================================================================================================
 asm_MATH_SerialSphericalLight_PAINT, used in Lightstruct.C
 p0 = Vertex Source base 	(Aligned)
 p1 = Normales Base		(non Aligned)
 p2 = Number Of Points		
 p3 = Point Light coordinate	(Aligned)
 p4 = 1.0f / (1/near - 1/far)	(float)
 p5 = 1/near			(float)
 p6 = Color			(Aligned)
 p7 = Color dest
 =======================================================================================================================
 */
	.global asm_MATH_SerialSphericalLight_PAINT
	.extern PS2_ComputeLightSperical_PAINT
	.ent asm_MATH_SerialSphericalLight_PAINT
asm_MATH_SerialSphericalLight_PAINT:
	addiu	sp, sp, -(16 * 8)
	sq	a0,00(sp)
	sq	a1,96(sp)
	sq	t0,16(sp)
	sq	t1,32(sp)
	sq	t2,48(sp)
	sq	t3,64(sp)
	sq	t4,80(sp)
	sq	t5,112(sp)
	
	mtsah 		zero, 4

	lw		t0,	12(a0)					/* Point Light Coordinate */
	lqc2    	$vf01, 	0(t0)
	lw		t0,	24(a0)					/* Point Light Color */

	PEXTLB		t0, zero, t0			/* Extend light Color */
	PEXTLB		t0, zero, t0			/* Extend light Color */
	qmtc2    	t0,	$vf02
	VITOF0		$vf02,$vf02
	
	lw		t0,	16(a0)					/* Factor 1 */
	qmtc2 		t0, 	$vf03
	lw		t0,	20(a0)					/* Factor 2 */
	qmtc2 		t0, 	$vf04

	lw		t0,	0(a0)					/* Vertex source base */
	lw		t1,	4(a0)					/* Normales source base */
	lw		t2,	8(a0)					/* Number of points */
	lw		t3,	28(a0)					/* Color dest Base */
	lqc2    	$vf10, 	0(t0)  			/* Load Point */
	
	beq     	t2 , 	zero , CLS_Finish_PAINT
	nop

	lqc2    	$vf10, 	0(t0)  			/* Load Point n°1 */
	vcallms 	PS2_ComputeLightSperical_PAINT
	qmfc2.i 	t4, 	$vf12
	
	lqc2    	$vf10, 	16(t0)  		/* Load Point n°2 */
	lw		t5,	8(t1)					/* Load first Normale (non aligned)	*/
	qfsrv 		t5, 	t5, t5			/* Load first Normale (non aligned)	*/
	ldl		t5,	7(t1)					/* Load first Normale (non aligned)	*/
	ldr		t5,	0(t1)					/* Load first Normale (non aligned)	*/
	qmtc2 		t5, 	$vf11   		/* Load first Normale in VU0 		*/
	vcallms 	PS2_ComputeLightSperical_PAINT
	qmfc2.i 	t4, 	$vf12
	
	addi 		t1,	t1,12
	
	lqc2    	$vf10, 	32(t0)  		/* Load Point n°3 */
	lw		t5,	8(t1)					/* Load first Normale (non aligned)	*/
	qfsrv 		t5, 	t5, t5			/* Load first Normale (non aligned)	*/
	ldl		t5,	7(t1)					/* Load first Normale (non aligned)	*/
	ldr		t5,	0(t1)					/* Load first Normale (non aligned)	*/
	qmtc2 		t5, 	$vf11   		/* Load first Normale in VU0 		*/
	vcallms 	PS2_ComputeLightSperical_PAINT
	qmfc2.i 	t4, 	$vf12
	
	
	addi 		t1,	t1,12
	addi 		t0,	t0,32

CLS_Loop_PAINT:
	lw			t5,	8(t1)					/* Load next+2 Normale (non aligned)	*/
	qfsrv 		t5, 	t5, t5			/* Load next+2 Normale (non aligned)	*/
	addi 		t2,	t2,-1
	ldl			t5,	7(t1)					/* Load next+2 Normale (non aligned)	*/
	ldr			t5,	0(t1)					/* Load next+2 Normale (non aligned)	*/
	lw			zero,128(t1)
	qmfc2.i 	t4, 	$vf12			/* Get color 				*/
	qmtc2 		t5, 	$vf11   		/* Load next+2 Normale in VU0*/ 		
	lqc2    	$vf10, 	16(t0)  		/* Load Next+3 Point in VU0 */		
	vcallms 	PS2_ComputeLightSperical_PAINT/* call VU0 				*/
	PPACB		t4, 	zero, t4		/* Pack Color Result 		*/	
	lw			t5,	0(t3)					/* Load color 				*/
	lw			zero,128(t0)
	PPACB		t4, 	zero, t4		/* Pack Color Result 		*/	
	addi 		t1,	t1,12
	addi 		t0,	t0,16
	PADDUB		t4,	t4,t5				/* Add Color to colors dest 		*/
	sw		t4,	0(t3)					/* Save Color 				*/
	bne     	t2 , 	zero , CLS_Loop_PAINT
	addi 		t3,	t3,4

CLS_Finish_PAINT:
	lq		a0,00(sp)
	lq		a1,96(sp)
	lq		t0,16(sp)
	lq		t1,32(sp)
	lq		t2,48(sp)
	lq		t3,64(sp)
	lq		t4,80(sp)
	lq		t5,112(sp)
	jr		ra
	addiu	sp, sp, (16 * 8)	
	.end asm_MATH_SerialSphericalLight_PAINT

/*
 =======================================================================================================================
 asm_MATH_SerialSpotLight, used in Lightstruct.C
 p0 = Vertex Source base 	(Aligned)
 p1 = Normales Base		(non Aligned)
 p2 = Number Of Points		
 p3 = Point Light coordinate	(Aligned)
 p4 = 1.0f / (1/near - 1/far)	(float)
 p5 = 1/near			(float)
 p6 = Color			(Aligned)
 p7 = Color dest
 =======================================================================================================================
 */
	.global asm_MATH_SerialSpotLight
	.extern PS2_ComputeLightSpot
	.ent asm_MATH_SerialSpotLight
asm_MATH_SerialSpotLight:
	addiu	sp, sp, -(16 * 8)
	sq	a0,00(sp)
	sq	a1,96(sp)
	sq	t0,16(sp)
	sq	t1,32(sp)
	sq	t2,48(sp)
	sq	t3,64(sp)
	sq	t4,80(sp)
	sq	t5,112(sp)
	
	mtsah 		zero, 4

	lw		t0,	12(a0)					/* Point Light Coordinate */
	lqc2    	$vf01, 	0(t0)
	lw		t0,	24(a0)					/* Point Light Color */

	PEXTLB		t0, zero, t0			/* Extend light Color */
	PEXTLB		t0, zero, t0			/* Extend light Color */
	qmtc2    	t0,	$vf02
	VITOF0		$vf02,$vf02
	
	lw		t0,	16(a0)					/* Factor 1 */
	qmtc2 		t0, 	$vf03
	lw		t0,	20(a0)					/* Factor 2 */
	qmtc2 		t0, 	$vf04
	
/* Load Light Dir */	
/* Load Factors */
	lw		t0,	32(a0)					/* Light Dir */
	lqc2	$vf14,0(t0)
	
	lw		t0,	36(a0)					/* Light Dir factors */
	lqc2	$vf25,0(t0)

	lw		t0,	0(a0)					/* Vertex source base */
	lw		t1,	4(a0)					/* Normales source base */
	lw		t2,	8(a0)					/* Number of points */
	lw		t3,	28(a0)					/* Color dest Base */
	
	beq     	t2 , 	zero , CLSpot_Finish
	nop
	

	lqc2    	$vf10, 	0(t0)  			/* Load Point n°1 */
	vcallms 	PS2_ComputeLightSpot
	qmfc2.i 	t4, 	$vf12
	
	lqc2    	$vf10, 	16(t0)  		/* Load Point n°2 */
	lw		t5,	8(t1)					/* Load first Normale (non aligned)	*/
	qfsrv 		t5, 	t5, t5			/* Load first Normale (non aligned)	*/
	ldl		t5,	7(t1)					/* Load first Normale (non aligned)	*/
	ldr		t5,	0(t1)					/* Load first Normale (non aligned)	*/
	qmtc2 		t5, 	$vf11   		/* Load first Normale in VU0 		*/
	vcallms 	PS2_ComputeLightSpot
	qmfc2.i 	t4, 	$vf12
	
	addi 		t1,	t1,12
	
	lqc2    	$vf10, 	32(t0)  		/* Load Point n°3 */
	lw		t5,	8(t1)					/* Load first Normale (non aligned)	*/
	qfsrv 		t5, 	t5, t5			/* Load first Normale (non aligned)	*/
	ldl		t5,	7(t1)					/* Load first Normale (non aligned)	*/
	ldr		t5,	0(t1)					/* Load first Normale (non aligned)	*/
	qmtc2 		t5, 	$vf11   		/* Load first Normale in VU0 		*/
	vcallms 	PS2_ComputeLightSpot
	qmfc2.i 	t4, 	$vf12
	
	
	addi 		t1,	t1,12
	addi 		t0,	t0,32

CLSpot_Loop:
	lw		t5,	8(t1)					/* Load next+2 Normale (non aligned)	*/
	qfsrv 		t5, 	t5, t5			/* Load next+2 Normale (non aligned)	*/
	addi 		t2,	t2,-1
	ldl		t5,	7(t1)					/* Load next+2 Normale (non aligned)	*/
	ldr		t5,	0(t1)					/* Load next+2 Normale (non aligned)	*/
	qmfc2.i 	t4, 	$vf12			/* Get color 				*/
	qmtc2 		t5, 	$vf11   		/* Load next+2 Normale in VU0*/ 	
	lqc2    	$vf10, 	16(t0)  		/* Load Next+3 Point in VU0 */		
	vcallms 	PS2_ComputeLightSpot		/* call VU0 			*/	
	PPACB		t4, 	zero, t4		/* Pack Color Result 		*/	
	lw		t5,	0(t3)					/* Load color 				*/
	PPACB		t4, 	zero, t4		/* Pack Color Result 		*/	
	addi 		t1,	t1,12
	addi 		t0,	t0,16
	PADDUB		t4,	t4,t5				/* Add Color to colors dest 		*/
	sw		t4,	0(t3)					/* Save Color 				*/
	bne     	t2 , 	zero , CLSpot_Loop
	addi 		t3,	t3,4

CLSpot_Finish:
	lq		a0,00(sp)
	lq		a1,96(sp)
	lq		t0,16(sp)
	lq		t1,32(sp)
	lq		t2,48(sp)
	lq		t3,64(sp)
	lq		t4,80(sp)
	lq		t5,112(sp)
	jr		ra
	addiu	sp, sp, (16 * 8)
	.end asm_MATH_SerialSpotLight

/*
 =======================================================================================================================
 asm_SOFT_ComputeSpecularVectors, used in SOFT_Linear.C
 p0 = Vertex Source base 	(Aligned)
 p1 = Normale Source base 	(Non-Aligned)
 p2 = Destination Normale	(Non-Aligned)
 p3 = Camera Coordinate
 p4 = Number Of Points		
 =======================================================================================================================
 */
	.global asm_SOFT_ComputeSpecularVectors
	.extern PS2_ComputeSpecular
	.ent asm_SOFT_ComputeSpecularVectors
asm_SOFT_ComputeSpecularVectors:
	addiu	sp, sp, -(16 * 8)
	sq	a0,00(sp)
	sq	a1,96(sp)
	sq	t0,16(sp)
	sq	t1,32(sp)
	sq	t2,48(sp)
	sq	t3,64(sp)
	sq	t4,80(sp)
	sq	t5,112(sp)

	mtsah 		zero, 4
	lw		t3,	12(a0)	/* Camera coordinate */
	
	lw		t5,	8(t3)		/* Load Camera point  (non aligned)	*/
	qfsrv 		t5, 	t5, t5		/* Load Camera point  (non aligned)	*/
	ldl		t5,	7(t3)		/* Load Camera point  (non aligned)	*/
	ldr		t5,	0(t3)		/* Load Camera point  (non aligned)	*/
	qmtc2 		t5, 	$vf01   	/* Load Camera point in VU0 */		

	
	lw		t0,	0(a0)	/* Vertex source base */
	lw		t1,	4(a0)	/* Normales source base*/ 
	lw		t2,	8(a0)	/* Normales dest base */
	lw		t4,	16(a0)	/* Number Of points */
	
	beq     		t4 , 	zero , CSpc_Finish
	nop
	
	lqc2    	$vf02, 	0(t0)  		/* Load Point n°0 in VU0 	*/
	vcallms 	PS2_ComputeSpecular	/* call VU0 			*/
	qmfc2.i 	t3, 	$vf04		/* Get Normale 			*/
	
	lqc2    	$vf02, 	16(t0)  	/* Load Point n°1 in VU0 	*/
	lw		t5,	8(t1)		/* Load Normale (non aligned)	*/
	qfsrv 		t5, 	t5, t5		/* Load Normale (non aligned)	*/
	ldl		t5,	7(t1)		/* Load Normale (non aligned)	*/
	ldr		t5,	0(t1)		/* Load Normale (non aligned)	*/
	qmtc2 		t5, 	$vf03   	/* Load Normale  in VU0 */	
	vcallms 	PS2_ComputeSpecular	/* call VU0 			*/
	qmfc2.i 	t3, 	$vf04		/* Get Normale 			*/
	addi 		t1,	t1,12		/* Normales source base + 1 */	
	
	lqc2    	$vf02, 	32(t0)  	/* Load Point n°2 in VU0 	*/
	lw		t5,	8(t1)		/* Load Normale (non aligned)	*/
	qfsrv 		t5, 	t5, t5		/* Load Normale (non aligned)	*/
	ldl		t5,	7(t1)		/* Load Normale (non aligned)	*/
	ldr		t5,	0(t1)		/* Load Normale (non aligned)	*/
	qmtc2 		t5, 	$vf03   	/* Load Normale  in VU0 */
	vcallms 	PS2_ComputeSpecular	/* call VU0 			*/
	qmfc2.i 	t3, 	$vf04		/* Get Normale 			*/
	addi 		t1,	t1,12		/* Normales source base + 1 	*/
	
	lqc2    	$vf02, 	48(t0)  	/* Load Point n°3 in VU0 	*/
	lw		t5,	8(t1)		/* Load Normale (non aligned)	*/
	qfsrv 		t5, 	t5, t5		/* Load Normale (non aligned)	*/
	ldl		t5,	7(t1)		/* Load Normale (non aligned)	*/
	ldr		t5,	0(t1)		/* Load Normale (non aligned)	*/
	qmtc2 		t5, 	$vf03   	/* Load Normale  in VU0 	*/
	vcallms 	PS2_ComputeSpecular	/* call VU0 			*/
	qmfc2.i 	t3, 	$vf04		/* Get Normale 			*/

	addi 		t1,	t1,12		/* Normales source base 	*/
	addi 		t0,	t0,32		/* Vertex source base 		*/
CSpc_Loop:
	lw		t5,	8(t1)		/* Load next + 1 Normale (non aligned)	*/
	addi 		t0,	t0,16		/* Vertex source base */
	qfsrv 		t5, 	t5, t5		/* Load next + 1 Normale (non aligned)	*/
	ldl		t5,	7(t1)		/* Load next + 1 Normale (non aligned)	*/
	ldr		t5,	0(t1)		/* Load next + 1 Normale (non aligned)	*/
	qmfc2.i 	t3, 	$vf04		/* Get Normale 			*/
	qmtc2 		t5, 	$vf03   	/* Load next + 1 Normale  in VU0 */
	lqc2    	$vf02, 	0(t0)  		/* Load Next + 1 Point in VU0 	*/
	vcallms 	PS2_ComputeSpecular	/* call VU0 			*/
	sdl		t3,	7(t2)		/* store Normale (non aligned)	*/
	addi 		t1,	t1,12		/* Normales source base */
	sdr		t3,	0(t2)		/* store Normale (non aligned)	*/
	qfsrv 		t3, 	t3, t3		/* store Normale (non aligned)	*/
	addi 		t4,	t4,-1
	sw		t3,	8(t2)		/* store Normale (non aligned)	*/
	bne     	t4 , 	zero , CSpc_Loop
	addi 		t2,	t2,12	/* Normales dest base */
	
CSpc_Finish:
	
	lq		a0,00(sp)
	lq		a1,96(sp)
	lq		t0,16(sp)
	lq		t1,32(sp)
	lq		t2,48(sp)
	lq		t3,64(sp)
	lq		t4,80(sp)
	lq		t5,112(sp)
	jr		ra
	addiu	sp, sp, (16 * 8)
	.end asm_SOFT_ComputeSpecularVectors
    
/*
 =======================================================================================================================
 =======================================================================================================================
 */
    .extern vsm_MATH_GetRotationMatrix
    .global asm_MATH_GetRotationMatrix2
    .ent asm_MATH_GetRotationMatrix2
asm_MATH_GetRotationMatrix2:
    lqc2    $vf01, 0(a1)    
    lqc2    $vf02, 16(a1)    
    lqc2    $vf03, 32(a1)    
    lqc2    $vf04, 48(a1)
    vcallms vsm_MATH_GetRotationMatrix
    qmfc2.i t0, $vf01
    sqc2    $vf01, 0(a0)
    sqc2    $vf02, 16(a0)
    sqc2    $vf03, 32(a0)
    jr      ra
    sqc2    $vf04, 48(a0)
    .end asm_MATH_GetRotationMatrix2


    .global MATH_CrossProduct
    .ent MATH_CrossProduct
MATH_CrossProduct:    
    mtsah $0, 4
    lw t1,8(a2)
    lw t0,8(a1)
    qfsrv t0, t0, t0
    qfsrv t1, t1, t1 
    ldl t0,7(a1) 
    ldl t1,7(a2) 
    ldr t1,0(a2) 
    ldr t0,0(a1)
    qmtc2 t0 , $vf02
    qmtc2 t1 , $vf03
    vopmula.xyz ACCxyz, $vf02, $vf03
    vopmsub.xyz $vf01, $vf03, $vf02
    qmfc2.i t0 , $vf01
    sdl t0,7(a0)
    sdr t0,0(a0) 
    qfsrv t0, t0, t0
    jr ra
    sw t0,8(a0) 
    .end MATH_CrossProduct


    .global MATH_NormalizeVector
    .ent MATH_NormalizeVector
MATH_NormalizeVector:
         mtsah        $0,4
         lw           t0,8(a1)
         qfsrv        t0,t0,t0
         ldl          t0,7(a1)
         ldr          t0,0(a1)
         qmtc2.ni     t0,vf1
         vmul.xyz     vf2,vf1,vf1
         vaddy.x      vf2,vf2,vf2
         vaddz.x      vf2,vf2,vf2
         vrsqrt       Q,vf0w,vf2x
         vwaitq       
         vmulq.xyz    vf1,vf1,Q
         qmfc2.i      t0,vf1
         sdl          t0,7(a0)
         sdr          t0,0(a0)
         qfsrv        t0,t0,t0
         jr           ra
         sw           t0,8(a0)
         .end MATH_NormalizeVector



        .global MATH_NormalizeEqualVector
        .ent MATH_NormalizeEqualVector
MATH_NormalizeEqualVector:
         mtsah        $0,4
         lw           t0,8(a0)
         qfsrv        t0,t0,t0
         ldl          t0,7(a0)
         ldr          t0,0(a0)
         qmtc2.ni     t0,vf1
         vmul.xyz     vf2,vf1,vf1
         vaddy.x      vf2,vf2,vf2
         vaddz.x      vf2,vf2,vf2
         vrsqrt       Q,vf0w,vf2x
         vwaitq       
         vmulq.xyz    vf1,vf1,Q
         qmfc2.i      t0,vf1
         sdl          t0,7(a0)
         sdr          t0,0(a0)
         qfsrv        t0,t0,t0
         jr           ra
         sw           t0,8(a0)
         .end MATH_NormalizeEqualVector


        .global MATH_SetNormVector
        .ent MATH_SetNormVector
MATH_SetNormVector:
         mtsah        $0,4
         lw           t0,8(a1)
         qfsrv        t0,t0,t0
         ldl          t0,7(a1)
         ldr          t0,0(a1)
         mfc1         t3,$f12
         qmtc2.ni     t0,vf1
         qmtc2.ni     t3,vf3
         vmul.xyz     vf2,vf1,vf1
         vaddy.x      vf2,vf2,vf2
         vaddz.x      vf2,vf2,vf2
         vrsqrt       Q,vf3x,vf2x
         vwaitq       
         vmulq.xyz    vf1,vf1,Q
         qmfc2.i      t0,vf1
         sdl          t0,7(a0)
         sdr          t0,0(a0)
         qfsrv        t0,t0,t0
         jr           ra
         sw           t0,8(a0)
         .end MATH_SetNormVector



        .global MATH_AddScaleVector
        .ent MATH_AddScaleVector
MATH_AddScaleVector:
         mtsah        $0,4
         lw           t1,8(a2)
         lw           t0,8(a1)
         qfsrv        t0,t0,t0
         qfsrv        t1,t1,t1
         ldl          t0,7(a1)
         ldl          t1,7(a2)
         ldr          t1,0(a2)
         ldr          t0,0(a1)
         mfc1         t3,$f12
         qmtc2.ni     t0,vf1
         qmtc2.ni     t1,vf2
         qmtc2.ni     t3,vf3
         vmulx.xyzw   vf2,vf2,vf3
         vadd.xyzw    vf1,vf2,vf1
         qmfc2.i      t0,vf1
         sdl          t0,7(a0)
         sdr          t0,0(a0)
         qfsrv        t0,t0,t0
         jr           ra
         sw           t0,8(a0)
         .end MATH_AddScaleVector
         
         
         
        .global MATH_BlendVector_asm
        .ent MATH_BlendVector_asm
MATH_BlendVector_asm:
         mtsah        $0,4
         lw           t1,8(a2)
         lw           t0,8(a1)
         qfsrv        t0,t0,t0
         qfsrv        t1,t1,t1
         ldl          t0,7(a1)
         ldl          t1,7(a2)
         ldr          t1,0(a2)
         ldr          t0,0(a1)
         mfc1         t3,$f12
         qmtc2.ni     t0,vf1
         qmtc2.ni     t1,vf2
         qmtc2.ni     t3,vf3
         vsub.xyzw    vf2,vf2,vf1
         vmulx.xyzw   vf2,vf2,vf3
         vadd.xyzw    vf1,vf2,vf1
         qmfc2.i      t0,vf1
         sdl          t0,7(a0)
         sdr          t0,0(a0)
         qfsrv        t0,t0,t0
         
		psllw 		t2, t3 , 2
         
         jr           ra
         sw           t0,8(a0)
         .end MATH_BlendVector_asm
         
         
         
/*
 =======================================================================================================================
 bon ce n'est pas sa place, mais je ne voulais pas faire un fichier juste pour ca !
 =======================================================================================================================
 */
    .global asm_break
    .ent asm_break
asm_break:
    break
    jr  ra
    nop
    .end asm_break

