

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


#define MATH_Ci_Scale   8
#define MATH_Ci_Rotation        4
#define MATH_Ci_Translation     2

#define USE_NEW_ASM

#ifdef __mips16
	.set nomips16	/* This file contains 32 bit assembly code. */
#endif
    .set noat
   	.set noreorder
	.text			


/* 
 ============================================================================================
    void MATH_TransformVector(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
 ============================================================================================
*/
    .global MATH_TransformVector
    .ent MATH_TransformVector
MATH_TransformVector:
#ifdef USE_NEW_ASM
	mtsab 		a1,0
	lw  		t8,64(a1) /* Load Flags */
	lq			t0,0(a1)
	lq			t1,16(a1)
	lq			t2,32(a1)
	lq			t3,48(a1)
	qfsrv 		t0 , t1 , t0
	qfsrv 		t1 , t2 , t1
	qfsrv 		t2 , t3 , t2
	lq			t5,0(a2)
	lq			t6,16(a2)
	andi 		t8,t8,MATH_Ci_Scale
	mtsab 		a2,0
	qmtc2		t0 , $vf01
	qmtc2		t1 , $vf02
	qmtc2		t2 , $vf03
	beq			t8,zero,no_Scale_TVEC
	qfsrv 		t0 , t6 , t5
    vmulw.xyz   $vf01, $vf01, $vf01w
    vmulw.xyz   $vf02, $vf02, $vf02w
    vmulw.xyz   $vf03, $vf03, $vf03w
no_Scale_TVEC:	
	qmtc2   	t0, $vf04
    vmulax.xyz   ACC, $vf01, $vf04x
    vmadday.xyz  ACC, $vf02, $vf04y
    vmaddz.xyz  $vf10, $vf03, $vf04z
    qmfc2.i t0 , $vf10
    sdl		t0,7(a0)
    sdr		t0,0(a0)
    pcpyud 	t0, t0, t0
    jr      ra
    sw		t0,8(a0)	
	
#else
    
    lw      t2, 64(a1)
    mtsah 	zero, 4
    lui     t1,16256
    andi    t2, t2, MATH_Ci_Scale
    dsll32  t1, t1, 0

    
    lw      t3, 8(a1);  
    lw      t7, 24(a1)
    dsll32  t3, t3, 0;  
    dsll32  t7, t7, 0
    dsrl32  t3, t3, 0;  
    dsrl32  t7, t7, 0
    or      t3, t1, t3; 
    or      t7, t1, t7
    ldl     t0, 15(a1); 
    ldl     t4, 31(a1)    
    ldr     t0, 8(a1);  
    ldr     t4, 24(a1)
    movz    t0, t3, t2;
     movz    t4, t7, t2
    qfsrv 	t0, t0, t0; 
    qfsrv 	t4, t4, t4;
    ldl		t0, 7(a1); 
     ldl		t4, 23(a1);
    ldr		t0, 0(a1); 
     ldr		t4, 16(a1);
    qmtc2   t0, $vf01; 
     qmtc2   t4, $vf02
    
    lw      t3, 40(a1); 
    ldl     t7, 15(a2)
    dsll32  t3, t3, 0;  
    ldr     t7, 8(a2)
    dsrl32  t3, t3, 0;  
    qfsrv 	t7, t7, t7;
    or      t3, t1, t3; 
    ldl		t7, 7(a2);
    ldl     t0, 47(a1); 
    ldr		t7, 0(a2);     
    ldr     t0, 40(a1); 
    qmtc2   t7, $vf04
    movz    t0, t3, t2;
    qfsrv 	t0, t0, t0;
    ldl		t0, 39(a1);
    ldr		t0, 32(a1);
    qmtc2   t0, $vf03;
    vnop
    vmulw.xyz   $vf01, $vf01, $vf01w
    vmulw.xyz   $vf02, $vf02, $vf02w
    vmulw.xyz   $vf03, $vf03, $vf03w
    vnop
    vmulax.xyz   ACC, $vf01, $vf04x
    vmadday.xyz  ACC, $vf02, $vf04y
    vmaddaz.xyz  ACC, $vf03, $vf04z
    vmaddw      $vf10, $vf00, vf00w
    qmfc2.i t0 , $vf10
    sdl		t0,7(a0)
    sdr		t0,0(a0)
    qfsrv 	t0, t0, t0
    jr      ra
    sw		t0,8(a0)	
#endif    
    .end MATH_TransformVector

    

/* 
 ============================================================================================
 void MATH_TransformVectorNoScale(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
 ============================================================================================
*/
    .global MATH_TransformVectorNoScale
    .ent MATH_TransformVectorNoScale

MATH_TransformVectorNoScale:
#ifdef USE_NEW_ASM
	mtsab 		a1,0
	lq			t0,0(a1)
	lq			t1,16(a1)
	lq			t2,32(a1)
	lq			t3,48(a1)
	qfsrv 		t0 , t1 , t0
	qfsrv 		t1 , t2 , t1
	qfsrv 		t2 , t3 , t2
	lq			t5,0(a2)
	lq			t6,16(a2)
	qmtc2		t0 , $vf01
	mtsab 		a2,0
	qmtc2		t1 , $vf02
	qmtc2		t2 , $vf03
	qfsrv 		t0 , t6 , t5
	qmtc2   	t0, $vf04
    vmulax.xyz   ACC, $vf01, $vf04x
    vmadday.xyz  ACC, $vf02, $vf04y
    vmaddz.xyz  $vf10, $vf03, $vf04z
    qmfc2.i t0 , $vf10
    sdl		t0,7(a0)
    sdr		t0,0(a0)
    pcpyud 	t0, t0, t0
    jr      ra
    sw		t0,8(a0)	
#else
    mtsah 	zero, 4
        
    ldl     t0, 15(a1);
     ldl     t4, 31(a1)    
    ldr     t0, 8(a1);  
    ldr     t4, 24(a1)
    qfsrv 	t0, t0, t0; 
    qfsrv 	t4, t4, t4;
    ldl		t0, 7(a1);  
    ldl		t4, 23(a1);
    ldr		t0, 0(a1);  
    ldr		t4, 16(a1);
    qmtc2   t0, $vf01;  
    qmtc2   t4, $vf02
    
    ldl     t0, 47(a1); 
    ldl     t7, 15(a2)
    ldr     t0, 40(a1);
     ldr     t7, 8(a2)
    qfsrv 	t0, t0, t0; 
    qfsrv 	t7, t7, t7
    ldl		t0, 39(a1); 
    ldl		t7, 7(a2)
    ldr		t0, 32(a1); 
    ldr		t7, 0(a2)
    qmtc2   t0, $vf03;  
    qmtc2   t7, $vf04
    
    vmulax.xyz   ACC, $vf01, $vf04x
    vmadday.xyz  ACC, $vf02, $vf04y
    vmaddaz.xyz  ACC, $vf03, $vf04z
    vmaddw      $vf10, $vf00, vf00w

    qmfc2.i t0 , $vf10
    sdl		t0,7(a0)
    sdr		t0,0(a0)
    qfsrv 	t0, t0, t0
    jr      ra
    sw		t0,8(a0)	
#endif    
    .end MATH_TransformVectorNoScale


/* 
 ============================================================================================
 void MATH_TransformVertex(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
 ============================================================================================
*/
    .global MATH_TransformVertex
    .ent MATH_TransformVertex
MATH_TransformVertex:
#ifdef USE_NEW_ASM
	mtsab 		a1,0
	lw  		t8,64(a1) /* Load Flags */
	lq			t0,0(a1)
	lq			t1,16(a1)
	lq			t2,32(a1)
	lq			t3,48(a1)
	lq			t4,64(a1)
	qfsrv 		t0 , t1 , t0
	qfsrv 		t1 , t2 , t1
	qfsrv 		t2 , t3 , t2
	qfsrv 		t3 , t4 , t3
	lq			t5,0(a2)
	lq			t6,16(a2)
	andi 		t8,t8,MATH_Ci_Scale
	mtsab 		a2,0
	qmtc2		t0 , $vf01
	qmtc2		t1 , $vf02
	qmtc2		t2 , $vf03
	qmtc2		t3 , $vf04
	beq			t8,zero,no_Scale_TVER
	qfsrv 		t0 , t6 , t5
    vmulw.xyz   $vf01, $vf01, $vf01w
    vmulw.xyz   $vf02, $vf02, $vf02w
    vmulw.xyz   $vf03, $vf03, $vf03w
no_Scale_TVER:	
	qmtc2   	t0, $vf05
    vmulax.xyz   ACC, $vf01, $vf05x
    vmadday.xyz  ACC, $vf02, $vf05y
    vmaddaz.xyz  ACC, $vf03, $vf05z
    vmaddw      $vf10, $vf04, vf00w
    qmfc2.i t0 , $vf10
    sdl		t0,7(a0)
    sdr		t0,0(a0)
    pcpyud 	t0, t0, t0
    jr      ra
    sw		t0,8(a0)	
#else
    mtsah 	zero, 4
    
    lw      t2, 64(a1)
    andi    t2, t2, MATH_Ci_Scale
    lui     t1,16256
    dsll32  t1, t1, 0
    
    lw      t3, 8(a1); 
     lw      t7, 24(a1)
    dsll32  t3, t3, 0;  
    dsll32  t7, t7, 0
    dsrl32  t3, t3, 0; 
     dsrl32  t7, t7, 0
    or      t3, t1, t3;
     or      t7, t1, t7
    ldl     t0, 15(a1);
     ldl     t4, 31(a1)    
    ldr     t0, 8(a1);
      ldr     t4, 24(a1)
    movz    t0, t3, t2;
     movz    t4, t7, t2
    qfsrv 	t0, t0, t0;
     qfsrv 	t4, t4, t4
    ldl		t0, 7(a1); 
     ldl		t4, 23(a1)
    ldr		t0, 0(a1); 
     ldr		t4, 16(a1)
    qmtc2   t0, $vf01; 
     qmtc2   t4, $vf02
    
    lw      t3, 40(a1);
     ldl     t7, 15(a2)
    dsll32  t3, t3, 0;  
    ldr     t7, 8(a2)
    dsrl32  t3, t3, 0;  
    qfsrv 	t7, t7, t7
    or      t3, t1, t3;
     ldl		t7, 7(a2)
    ldl     t0, 47(a1);
     ldr		t7, 0(a2)     
    ldr     t0, 40(a1);
     qmtc2   t7, $vf04
    ldl     t1, 63(a1); 
    movz    t0, t3, t2; 
    ldr     t1, 56(a1);
     qfsrv 	t0, t0, t0; 
    qfsrv 	t1, t1, t1;
     ldl		t0, 39(a1); 
    ldl		t1, 55(a1); 
    ldr		t0, 32(a1); 
    ldr		t1, 48(a1);
     qmtc2   t0, $vf03;  
    qmtc2   t1, $vf05; 
     vnop
    
    vmulw.xyz   $vf01, $vf01, $vf01w
    vmulw.xyz   $vf02, $vf02, $vf02w
    vmulw.xyz   $vf03, $vf03, $vf03w
    vnop
    vmulax.xyz   ACC, $vf01, $vf04x
    vmadday.xyz  ACC, $vf02, $vf04y
    vmaddaz.xyz  ACC, $vf03, $vf04z
    vmaddw      $vf10, $vf05, vf00w
    qmfc2.i t0 , $vf10
    sdl		t0,7(a0)
    sdr		t0,0(a0)
    qfsrv 	t0, t0, t0
    jr      ra
    sw		t0,8(a0)	
#endif
    .end MATH_TransformVertex


/* 
 ============================================================================================
 void MATH_TransformVertexNoScale(MATH_tdst_Vector *VDst, MATH_tdst_Matrix *M, MATH_tdst_Vector *VSrc)
 ============================================================================================
*/
    .global MATH_TransformVertexNoScale
    .ent MATH_TransformVertexNoScale
MATH_TransformVertexNoScale:
#ifdef USE_NEW_ASM
	mtsab 		a1,0
	lq			t0,0(a1)
	lq			t1,16(a1)
	lq			t2,32(a1)
	lq			t3,48(a1)
	lq			t4,64(a1)
	qfsrv 		t0 , t1 , t0
	qfsrv 		t1 , t2 , t1
	qfsrv 		t2 , t3 , t2
	qfsrv 		t3 , t4 , t3
	lq			t5,0(a2)
	lq			t6,16(a2)
	qmtc2		t0 , $vf01
	mtsab 		a2,0
	qmtc2		t1 , $vf02
	qmtc2		t2 , $vf03
	qmtc2		t3 , $vf04
	qfsrv 		t0 , t6 , t5
	qmtc2   	t0, $vf05
    vmulax.xyz   ACC, $vf01, $vf05x
    vmadday.xyz  ACC, $vf02, $vf05y
    vmaddaz.xyz  ACC, $vf03, $vf05z
    vmaddw      $vf10, $vf04, vf00w
    qmfc2.i t0 , $vf10
    sdl		t0,7(a0)
    sdr		t0,0(a0)
    pcpyud 	t0, t0, t0
    jr      ra
    sw		t0,8(a0)	
 #else
    mtsah 	zero, 4;    
    nop
    ldl     t0, 15(a1); 
    ldl     t4, 31(a1)    
    ldr     t0, 8(a1); 
     ldr     t4, 24(a1)
    qfsrv 	t0, t0, t0; 
    qfsrv 	t4, t4, t4;
    ldl		t0, 7(a1);  
    ldl		t4, 23(a1);
    ldr		t0, 0(a1);  
    ldr		t4, 16(a1);
    qmtc2   t0, $vf01;  
    qmtc2   t4, $vf02
    
    ldl     t0, 47(a1); 
    ldl     t7, 15(a2)
    ldr     t0, 40(a1); 
    ldr     t7, 8(a2)
    qfsrv 	t0, t0, t0; 
    qfsrv 	t7, t7, t7
    ldl		t0, 39(a1); 
    ldl		t7, 7(a2)
    ldr		t0, 32(a1); 
    ldr		t7, 0(a2)
    qmtc2   t0, $vf03;  
    qmtc2   t7, $vf04

    ldl     t0, 63(a1)
    ldr     t0, 56(a1)
    qfsrv 	t0, t0, t0
    ldl		t0, 55(a1)
    ldr		t0, 48(a1)
    qmtc2   t0, $vf05;
    
    vmulax.xyz   ACC, $vf01, $vf04x
    vmadday.xyz  ACC, $vf02, $vf04y
    vmaddaz.xyz  ACC, $vf03, $vf04z
    vmaddw      $vf10, $vf05, vf00w

    qmfc2.i t0 , $vf10
    sdl		t0,7(a0)
    sdr		t0,0(a0)
    qfsrv 	t0, t0, t0
    jr      ra
    sw		t0,8(a0)	
#endif
    .end MATH_TransformVertexNoScale


/* 
 ============================================================================================
 void MATH_TransformHomVector(MATH_tdst_HomVector *VDst, MATH_tdst_Matrix *M, MATH_tdst_HomVector *VSrc)
 ============================================================================================
*/
    .global MATH_TransformHomVector
    .ent MATH_TransformHomVector
MATH_TransformHomVector:
    mtsah 	zero, 4;                lw      t2, 64(a1)
    andi    t2, t2, MATH_Ci_Scale;  lui     t1,16256
    nop;                            dsll32  t1, t1, 0;             
    
    lw      t3, 8(a1);  lw      t7, 24(a1)
    dsll32  t3, t3, 0;  dsll32  t7, t7, 0
    dsrl32  t3, t3, 0;  dsrl32  t7, t7, 0
    or      t3, t1, t3; or      t7, t1, t7
    ldl     t0, 15(a1); ldl     t4, 31(a1)    
    ldr     t0, 8(a1);  ldr     t4, 24(a1)
    movz    t0, t3, t2; movz    t4, t7, t2
    qfsrv 	t0, t0, t0; qfsrv 	t4, t4, t4
    ldl		t0, 7(a1);  ldl		t4, 23(a1)
    ldr		t0, 0(a1);  ldr		t4, 16(a1)
    qmtc2   t0, $vf01;  qmtc2   t4, $vf02
    
    lw      t3, 40(a1); ldl     t7, 15(a2)
    dsll32  t3, t3, 0;  ldr     t7, 8(a2)
    dsrl32  t3, t3, 0;  qfsrv 	t7, t7, t7
    or      t3, t1, t3; ldl		t7, 7(a2)
    ldl     t0, 47(a1); ldr		t7, 0(a2)     
    ldr     t0, 40(a1); qmtc2   t7, $vf04
    ldl     t1, 63(a1); movz    t0, t3, t2; 
    ldr     t1, 56(a1); qfsrv 	t0, t0, t0; 
    qfsrv 	t1, t1, t1; ldl		t0, 39(a1); 
    ldl		t1, 55(a1); ldr		t0, 32(a1); 
    ldr		t1, 48(a1); qmtc2   t0, $vf03;  
    
    lw      t2, 12(a2); movz    t1, $0, t2
    qmtc2   t1, $vf05;  vnop
    
    vmulw.xyz   $vf01, $vf01, $vf01w
    vmulw.xyz   $vf02, $vf02, $vf02w
    vmulw.xyz   $vf03, $vf03, $vf03w
    vnop
    vmulax.xyz   ACC, $vf01, $vf04x
    vmadday.xyz  ACC, $vf02, $vf04y
    vmaddaz.xyz  ACC, $vf03, $vf04z
    vmaddw      $vf10, $vf05, vf00w
    qmfc2.i t0 , $vf10; lui     t1,16256
    sdl		t0,7(a0);   movz    t1, $0, t2
    sdr		t0,0(a0);   qfsrv 	t0, t0, t0; 
    sw		t0,8(a0);   jr      ra
    sw      t1, 12(a0)
    .end MATH_TransformHomVector


/* 
 ============================================================================================
 void MATH_MulMatrixMatrix(MATH_tdst_Matrix *MDst, MATH_tdst_Matrix *M1, MATH_tdst_Matrix *M2)
 ============================================================================================
*/
    .extern  vsm_Special_MulMatMat
    .extern  vsm_Special_Scale
    .global MATH_MulMatrixMatrix
    .ent MATH_MulMatrixMatrix
MATH_MulMatrixMatrix:
#ifdef USE_NEW_ASM
	mtsab 		a1,0
	andi		a3,a0,0xf
	lw  		t8,64(a1) /* Load Flags */
	lq			t0,0(a1)
	lq			t1,16(a1)
	lq			t2,32(a1)
	lq			t3,48(a1)
	lq			t4,64(a1)
	andi		t6,t8,MATH_Ci_Scale
	qfsrv 		t0 , t1 , t0
	qfsrv 		t1 , t2 , t1
	qfsrv 		t2 , t3 , t2
	qfsrv 		t3 , t4 , t3
	qmtc2		t0 , $vf01
	qmtc2		t1 , $vf02
	qmtc2		t2 , $vf03
    vmove.w   	$vf10, $vf00w
    vmove.w   	$vf11, $vf00w
    vmove.w   	$vf12, $vf00w
	beq			t6,zero,no_Scale_M1
	qmtc2		t3 , $vf04
    vmove.w   	$vf10, $vf01
    vmove.w   	$vf11, $vf02
    vmove.w   	$vf12, $vf03
no_Scale_M1:
	mtsab 		a2,0
	lw  		t9,64(a2) /* Load Flags */
	lq			t0,0(a2)
	lq			t1,16(a2)
	lq			t2,32(a2)
	lq			t3,48(a2)
	lq			t4,64(a2)
	andi		t7,t9,MATH_Ci_Scale
	qfsrv 		t0 , t1 , t0
	qfsrv 		t1 , t2 , t1
	qfsrv 		t2 , t3 , t2
	qfsrv 		t3 , t4 , t3
	qmtc2		t0 , $vf05
	qmtc2		t1 , $vf06
	qmtc2		t2 , $vf07
	qmtc2		t3 , $vf08
	bne			t7,zero,Scale_M2
	vmove.w		$vf21,$vf00
no_Scale_M2:
    vmulax.xyz   ACC, 	$vf05 , $vf01x
    vmadday.xyz  ACC, 	$vf06 , $vf01y
    vmaddz.xyz   $vf10, $vf07 , $vf01z
    vmulax.xyz   ACC, 	$vf05 , $vf02x
    vmadday.xyz  ACC, 	$vf06 , $vf02y
    vmaddz.xyz   $vf11, $vf07 , $vf02z
    vmulax.xyz   ACC, 	$vf05 , $vf03x
    vmadday.xyz  ACC, 	$vf06 , $vf03y
    vmaddz.xyz   $vf12, $vf07 , $vf03z
    vmulax.xyz   ACC, 	$vf05 , $vf04x
    vmadday.xyz  ACC, 	$vf06 , $vf04y
    vmaddz.xyz   $vf13, $vf07 , $vf04z
    vmulw.xyz	 $vf13, $vf13 , $vf21w
    vadd.xyz	 $vf13, $vf13, $vf08
    andi         t8, t8, MATH_Ci_Rotation|MATH_Ci_Translation|MATH_Ci_Scale
    andi    	 t9, t9, MATH_Ci_Rotation|MATH_Ci_Scale
Store:
    or      	t6, t8, t9
	beq			a3,zero,Store_Aligned
    sw      	t6, 0x40(a0)
Store_non_Aligned:
	mtsab 		a0,0
	lq			t0,0x00(a0)
	lq			t4,0x40(a0)
   	qfsrv 		t5 , t0 , t4				// Save alignement
	addi		a1,a0,-4					// Prepare save unaligned
	qmfc2.i		t0,$vf10
	qmfc2		t1,$vf11
	mtsab 		a1,0xC						// Prepare save unaligned
	qmfc2		t2,$vf12
	qmfc2		t3,$vf13
	qfsrv 		t4 , t0 , t5	/* Save Alignement */
	qfsrv 		t0 , t1 , t0
	qfsrv 		t1 , t2 , t1
	qfsrv 		t2 , t3 , t2
	qfsrv 		t3 , t5 , t3
	sq			t4,0x00(a0)
	sq			t0,0x10(a0)
	sq			t1,0x20(a0)
	sq			t2,0x30(a0)
    jr      	ra
	sq			t3,0x40(a0)
Store_Aligned:
	sqc2		$vf10,0x00(a0)
	sqc2		$vf11,0x10(a0)
	sqc2		$vf12,0x20(a0)
    jr      	ra
	sqc2		$vf13,0x30(a0)
	
Scale_M2:
/* On devrait tomber là assez rarement */
/* Mais si on tombe là , alors on le traite comme un Zoom  */
/* tant qu'à faire un truc foireux, autant le faire rapidement! */
	lui          t0 ,   16042
    vaddw.w      $vf05, $vf05, $vf06w // $vf05w = sx+sy
	ori          t0 ,   t0 , 0xaaab
	qmtc2		 t0 ,   $vf20		//vf20.x = 0.33333f
    vaddw.w      $vf05, $vf05 , $vf07w // $vf05w = sx+sy+sz
    vmulx.w		 $vf05, $vf05 , $vf20x // $vf05w = (sx+sy+sz) * 0.333333f
    vmulw.w		 $vf10, $vf10 , $vf05w
    vmulw.w		 $vf11, $vf11 , $vf05w
    vmulw.w		 $vf12, $vf12 , $vf05w
    beq			 $0, $0, no_Scale_M2 //b <=> beq 0, 0 sinon CW merde avec les symboles
	vmove.w		 $vf21, $vf05

	

#else    
    sw      $0, 64(a0); 
    mtsah 	zero, 4
    
    lw      t2, 64(a1); 
    andi    t2, t2, MATH_Ci_Scale
    lui     t1,16256;   
    dsll32  t1, t1, 0
    
    lw      t3, 8(a1);  
    lw      t7, 24(a1)
    dsll32  t3, t3, 0;  
    dsll32  t7, t7, 0
    dsrl32  t3, t3, 0;  
    dsrl32  t7, t7, 0
    or      t3, t1, t3; 
    or      t7, t1, t7
    ldl     t0, 15(a1); 
    ldl     t4, 31(a1)    
    ldr     t0, 8(a1);  
    ldr     t4, 24(a1)
    movz    t0, t3, t2; 
    movz    t4, t7, t2
    qfsrv 	t0, t0, t0; 
    qfsrv 	t4, t4, t4;
    ldl		t0, 7(a1);  
    ldl		t4, 23(a1);
    ldr		t0, 0(a1);  
    ldr		t4, 16(a1);
    qmtc2   t0, $vf01;  
    qmtc2   t4, $vf02
    
    lw      t3, 40(a1); 
    ldl     t7, 63(a1)
    dsll32  t3, t3, 0;  
    ldr     t7, 56(a1)
    dsrl32  t3, t3, 0;  
    qfsrv 	t7, t7, t7;
    or      t3, t1, t3; 
    ldl		t7, 55(a1);
    ldl     t0, 47(a1); 
    ldr		t7, 48(a1);     
    ldr     t0, 40(a1); 
    qmtc2   t7, $vf04
    movz    t0, t3, t2; 
    lw      t2, 64(a2)
    qfsrv 	t0, t0, t0; 
    andi    t2, t2, MATH_Ci_Scale
    ldl		t0, 39(a1); 
    lui     t1,16256
    ldr		t0, 32(a1); 
    dsll32  t1, t1, 0
    qmtc2   t0, $vf03;  
    nop
    
    lw      t3, 8(a2); 
     lw      t7, 24(a2)
    dsll32  t3, t3, 0; 
     dsll32  t7, t7, 0
    dsrl32  t3, t3, 0; 
     dsrl32  t7, t7, 0
    or      t3, t1, t3; 
    or      t7, t1, t7
    ldl     t0, 15(a2); 
    ldl     t4, 31(a2)    
    ldr     t0, 8(a2); 
     ldr     t4, 24(a2)
    movz    t0, t3, t2;
     movz    t4, t7, t2
    qfsrv 	t0, t0, t0;
     qfsrv 	t4, t4, t4;
    ldl		t0, 7(a2);  
    ldl		t4, 23(a2);
    ldr		t0, 0(a2);  
    ldr		t4, 16(a2);
    qmtc2   t0, $vf11;  
    qmtc2   t4, $vf12
    
    lw      t3, 40(a2); 
    ldl     t7, 63(a2)
    dsll32  t3, t3, 0;  
    ldr     t7, 56(a2)
    dsrl32  t3, t3, 0;  
    qfsrv 	t7, t7, t7;
    or      t3, t1, t3; 
    ldl		t7, 55(a2);
    ldl     t0, 47(a2);
     ldr		t7, 48(a2);     
    ldr     t0, 40(a2); 
    qmtc2   t7, $vf14
    movz    t0, t3, t2;
    qfsrv 	t0, t0, t0;
    ldl		t0, 39(a2);
    ldr		t0, 32(a2);
    qmtc2   t0, $vf13;

    vcallms vsm_Special_MulMatMat
    
    lw      t1, 64(a1)
    andi    t1, t1, MATH_Ci_Scale
    lw      t2, 64(a2)
    andi    t2, t2, MATH_Ci_Scale
    or      t0, t1, t2
    beq     t0, $0, lWithoutScale
    nop    
    vcallms vsm_Special_Scale
    nop
    

lWithoutScale:        
    
    qmfc2.i t0, $vf21;  
    qmfc2.i t1, $vf22
    sdl		t0, 7(a0);  
    sdl		t1, 23(a0)
    sdr		t0, 0(a0); 
     sdr		t1, 16(a0)
    qfsrv 	t0, t0, t0; 
    qfsrv 	t1, t1, t1
    sdl		t0, 15(a0); 
    sdl		t1, 31(a0)
    sdr		t0, 8(a0);  
    sdr		t1, 24(a0)
    
    qmfc2.i t2, $vf23;  
    qmfc2.i t3, $vf24
    sdl		t2, 39(a0); 
    sdl		t3, 55(a0)
    sdr		t2, 32(a0); 
    sdr		t3, 48(a0)
    qfsrv 	t2, t2,t2;  
    qfsrv 	t3, t3, t3
    sdl		t2, 47(a0); 
    sw		t3, 56(a0)
    sdr		t2, 40(a0); 
    lui     t3, 16256
    sw      t3, 60(a0)

    lw      t0, 64(a1)
    lw      t3, 64(a2)
    andi    t0, t0, MATH_Ci_Rotation|MATH_Ci_Translation|MATH_Ci_Scale
    andi    t3, t3, MATH_Ci_Rotation|MATH_Ci_Scale
    or      t1, t0, t3
    jr      ra
    sw      t1, 64(a0)
#endif
    .end MATH_MulMatrixMatrix




