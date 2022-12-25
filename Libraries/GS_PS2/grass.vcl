OCP_VERTEX_NUMBER .assign 28
OCP_VIF_STEP	.assign 2
OCP_GIF_STEP	.assign 3
OCP_VIF_BASE0 .assign 1
OCP_UV_H_JMP	.assign ((128*4)/(OCP_VERTEX_NUMBER - 1))
OCP_VIF_BASE1 .assign (OCP_VIF_BASE0 + OCP_VERTEX_NUMBER * OCP_VIF_STEP + 4) 
OCP_GIF_BASE0 .assign (OCP_VIF_BASE1 + OCP_VERTEX_NUMBER * OCP_VIF_STEP + 4)
OCP_GIF_BASE1 .assign (OCP_GIF_BASE0 + OCP_VERTEX_NUMBER * 2 * OCP_GIF_STEP + 1)
OCP_GIF_BASEe .assign (1024)

--enter		
.init_vf_all  
.init_vi_all
--endenter	

/*************************************************************************************************************************
*************************************************************************************************************************

													INIT

*************************************************************************************************************************	
*************************************************************************************************************************/

OCEAN_PHIL_INIT:
; 			CAMERA LOAD    
		LQ			GIF_TAGA , 	OCP_VIF_BASE0 + 0(VI00) 
		LQ			GIF_TAGB , 	OCP_VIF_BASE0 + 1(VI00) 
		LQ			I_CAMERA , 	OCP_VIF_BASE0 + 2(VI00) 
		LQ			J_CAMERA , 	OCP_VIF_BASE0 + 3(VI00)
		LQ			K_CAMERA , 	OCP_VIF_BASE0 + 4(VI00)
; 			PROJECTION INFO LOAD  
		LQ			MUL_SCREEN,	OCP_VIF_BASE0 + 5(VI00) 
		LQ			ADD_SCREEN,	OCP_VIF_BASE0 + 6(VI00)
		LQ			MAX_SCREEN,	OCP_VIF_BASE0 + 7(VI00)
		LQ			MIN_SCREEN,	OCP_VIF_BASE0 + 8(VI00)
; 			PERTURB FACTOR  
		LQ			PERTURB_MUL,OCP_VIF_BASE0 + 9(VI00)	 ; ! PERTURB MUL. W = -128 !!
; 			SAMPLE OF XYZF2  
		LQ			SXYZF2 , 	OCP_VIF_BASE0 + 10(VI00)
; 			SAMPLE OF XYZF3  
		LQ			SXYZF3 , 	OCP_VIF_BASE0 + 11(VI00)
	
		IADDIU 		GIF_BASE , VI00 ,  OCP_GIF_BASE0 ; GIF BASE  
		IADDIU 		GIF_LAST , VI00 ,  OCP_GIF_BASEe ; GIF BASE  

OCEAN_PHIL_INIT_FILLMEM:
		SQ 			SXYZF2 , 0 (GIF_BASE)
		IADDI		GIF_BASE , GIF_BASE , 1
		IBNE		GIF_BASE , GIF_LAST , OCEAN_PHIL_INIT_FILLMEM
		
		SQ			GIF_TAGA , OCP_GIF_BASE0 (VI00)
		SQ			GIF_TAGB , OCP_GIF_BASE1 (VI00)
		SQ 			SXYZF3 , OCP_GIF_BASE0 + 1 + OCP_GIF_STEP - 1 (VI00)
		SQ 			SXYZF3 , OCP_GIF_BASE0 + 1 + OCP_GIF_STEP * 2 - 1 (VI00)
		SQ 			SXYZF3 , OCP_GIF_BASE1 + 1 + OCP_GIF_STEP - 1  (VI00)
		SQ 			SXYZF3 , OCP_GIF_BASE1 + 1 + OCP_GIF_STEP * 2 - 1 (VI00)
		 
		
		IADDIU		State , VI00 , 0
		
/*************************************************************************************************************************
*************************************************************************************************************************

													STATE SWITCH

*************************************************************************************************************************	
*************************************************************************************************************************/
; switch on different case
SWITCHER:
		IADDIU		TEMP , VI00 , 4
		IBEQ		State , TEMP , OcenPhil_State4
		IADDIU		TEMP , VI00 , 3
		IBEQ		State , TEMP , OcenPhil_State3
		IADDIU		TEMP , VI00 , 2
		IBEQ		State , TEMP , OcenPhil_State2
		IADDIU		TEMP , VI00 , 1
		IBEQ		State , TEMP , OcenPhil_State1
; state 0 : no kick -> state 1
OcenPhil_State0:
		IADDI		p_STEP 	, VI00 , OCP_VIF_STEP
		IADDIU		T_U_INC , VI00 , OCP_UV_H_JMP
		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE0
		IADDIU		p_SRC   , VI00 , OCP_VIF_BASE0

		IADDI		State	, VI00 , 1
		B			OCEAN_PHIL_LOOPSTART;*/
		
; state 1 : no kick -> state 2
OcenPhil_State1:
		IADDI		p_STEP 	, VI00 , -OCP_VIF_STEP
		IADDIU		T_U_INC , VI00 , OCP_UV_H_JMP
		ISUB		T_U_INC , VI00 , T_U_INC
		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE1
		IADDIU		p_SRC   , VI00 , OCP_VIF_BASE1 + OCP_VERTEX_NUMBER * 2

		IADDI		State, VI00 , 2
		B			OCEAN_PHIL_LOOPSTART;*/
		
; state 2 : no kick -> state 3 
OcenPhil_State2:
		IADDI		p_STEP 	, VI00 , OCP_VIF_STEP
		IADDIU		T_U_INC , VI00 , OCP_UV_H_JMP
		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE0
		IADDIU		p_SRC   , VI00 , OCP_VIF_BASE0
		

		IADDI		State, VI00 , 3
		B			OCEAN_PHIL_LOOPSTART;*/
		
; state 3 : kick -> state 4
OcenPhil_State3:
		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE0
		XGKICK		p_DEST
		IADDI		p_STEP 	, VI00 , -OCP_VIF_STEP
		IADDIU		T_U_INC , VI00 , OCP_UV_H_JMP
		ISUB		T_U_INC , VI00 , T_U_INC
		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE1
		IADDIU		p_SRC   , VI00 , OCP_VIF_BASE1 + OCP_VERTEX_NUMBER * 2

		IADDI		State, VI00 , 4
		B			OCEAN_PHIL_LOOPSTART;*/

; state 2 : kick -> state 3
OcenPhil_State4:
/* Copy the 2 Last point of OCP_GIF_BASE0 at the end of GIFBASE1 */ 

		IADDIU		p_SRC   , VI00 , OCP_GIF_BASE0 + 1 + (OCP_VERTEX_NUMBER - 1) * OCP_GIF_STEP * 2
		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE1 + 1 + OCP_VERTEX_NUMBER * OCP_GIF_STEP * 2
		lq			Local0 , OCP_GIF_STEP * 1 - 1(p_SRC)
		sq			Local0 , OCP_GIF_STEP * 1 - 1(p_DEST)
		lq			Local0 , OCP_GIF_STEP * 1 - 2(p_SRC)
		sq			Local0 , OCP_GIF_STEP * 1 - 2(p_DEST)
		lq			Local0 , OCP_GIF_STEP * 1 - 3(p_SRC)
		sq			Local0 , OCP_GIF_STEP * 1 - 3(p_DEST)
		lq			Local0 , OCP_GIF_STEP * 2 - 1(p_SRC)
		sq			Local0 , OCP_GIF_STEP * 2 - 1(p_DEST)
		lq			Local0 , OCP_GIF_STEP * 2 - 2(p_SRC)
		sq			Local0 , OCP_GIF_STEP * 2 - 2(p_DEST)
		lq			Local0 , OCP_GIF_STEP * 2 - 3(p_SRC)
		sq			Local0 , OCP_GIF_STEP * 2 - 3(p_DEST)
		
	

		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE1
		XGKICK		p_DEST
		
		IADDI		p_STEP 	, VI00 , OCP_VIF_STEP
		IADDIU		T_U_INC , VI00 , OCP_UV_H_JMP
		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE0
		IADDIU		p_SRC   , VI00 , OCP_VIF_BASE0
		

		

		IADDI		State, VI00 , 3
		B			OCEAN_PHIL_LOOPSTART;*/
		
		

/*************************************************************************************************************************
*************************************************************************************************************************

													MAIN LOOP 

*************************************************************************************************************************	
*************************************************************************************************************************/
OCEAN_PHIL_LOOPSTART:
--cont	
		LQ.xyzw		CAMERAVEC0 , 0(VI00)
		LQ.xyzw		CNPARAMS,1(VI00)
		LQ.xyzw		G_COLOR,2(VI00)
/* Compute normales END **********************************************************/

/*SHIFT DEST GIF BEGIN **********************************************************/
; jump over Gif tag
		IADDIU		p_DEST , p_DEST , 1 
		IADDIU		p_DESTSave , p_DEST , 0
; compute last pointer		
		IADDIU		p_DESTLAST , p_DEST , OCP_VERTEX_NUMBER * OCP_GIF_STEP * 2
SHIFT_LOOP:
		lq			Local5 , OCP_GIF_STEP * 4 - 3(p_DEST) ; UV
		lq			Local3 , OCP_GIF_STEP * 4 - 2(p_DEST) ; color
		lq			Local4 , OCP_GIF_STEP * 4 - 1(p_DEST) ; XYZ
		sq.xyzw		Local5 , OCP_GIF_STEP * 3 - 3(p_DEST) ; UV
		sq.xyzw		Local3 , OCP_GIF_STEP * 3 - 2(p_DEST) ; color
		sq.xyz		Local4 , OCP_GIF_STEP * 3 - 1(p_DEST) ; XYZ
		lq			Local2 , OCP_GIF_STEP * 2 - 3(p_DEST) ; UV
		lq			Local0 , OCP_GIF_STEP * 2 - 2(p_DEST) ; color
		lq			Local1 , OCP_GIF_STEP * 2 - 1(p_DEST) ; XYZ
		sq.xyzw		Local2 , OCP_GIF_STEP * 1 - 3(p_DEST) ; UV
		sq.xyzw		Local0 , OCP_GIF_STEP * 1 - 2(p_DEST) ; color
		sq.xyz		Local1 , OCP_GIF_STEP * 1 - 1(p_DEST) ; XYZ
		IADDIU		p_DEST , p_DEST , OCP_GIF_STEP * 4
		IBNE		p_DEST , p_DESTLAST , SHIFT_LOOP;*/
; Restore p_DEST
		IADDIU		p_DEST , p_DESTSave , 0
		
; Load Rasters

		addw.w		WORLD,VF00,VF00w	; WORLD.w = 1.0f


		LQ.xyz		RASTER_BAS , 0(p_SRC) ;BASE  
		LQ.xyz		RASTER_INC , 1(p_SRC) ; INC  
		
		ILW.w		TEXTURE_U , 0(p_SRC)
		ILW.w		TEXTURE_V , 1(p_SRC)

		IADD		T_U_INC , T_U_INC , T_U_INC ;* 2
		IADD		T_U_INC , T_U_INC , T_U_INC ;* 4

/* LOOP ************************************************************************/
OCEAN_PHIL_LOOP:

		--LoopCS	8,1

; Load Perturb
		IADD		p_SRC , p_SRC , p_STEP
		
		LQ.xyz		WORLD , 1(p_SRC) ; Perturb  
		LQ.xyzw		COLOR , 0(p_SRC) ; Color 
/*
		itof0		COLOR , COLOR						; -128.0f
		addw		COLOR , COLOR , PERTURB_MUL.w 		; - 128.0f
		mul			COLOR , COLOR , CAMERAVEC0			; *= CameraVec0
		addy.x 		COLOR , COLOR , COLOR				; X += Y 
		addz.x 		COLOR , COLOR , COLOR				; X += Z 
		mulx.x		COLOR , COLOR , COLOR				; Coef *= Coef
		sub.x		COLOR , CNPARAMS , COLOR			; 1.0f - Coef 
		mulx.x		COLOR , COLOR , COLOR				; Coef *= Coef
		mulx.x		COLOR , COLOR , COLOR				; Coef *= Coef
		mulx.x		COLOR , COLOR , COLOR				; Coef *= Coef
		mulx 		COLOR , G_COLOR , COLORx			; Color
		ftoi0		COLOR , COLOR						; Pack
;*/

/*
		itof0		COLOR , COLOR						; -128.0f
		addw		COLOR , COLOR , PERTURB_MUL.w 		; - 128.0f
		mul			ALPHAC , ALPHAC , CAMERAVEC0		; *= CameraVec0
		addy.x 		COLOR , ALPHAC , ALPHAC				; X += Y 
		addz.x 		ALPHAC , ALPHAC , ALPHAC			; X += Z 
		mulx.xyz	ALPHAC , COLOR , ALPHACx			; Coef *= Coef
		add.xyz		ALPHAC , ALPHAC , CAMERAVEC0		; 1.0f - Coef 
		mul			ALPHAC , ALPHAC , CAMERAVEC0		; *= CameraVec0
		addy.x 		ALPHAC , ALPHAC , ALPHAC			; X += Y 
		addz.x 		ALPHAC , ALPHAC , ALPHAC			; X += Z 
		mulx 		COLOR , G_COLOR , COLORx			; Color
		ftoi0		COLOR , COLOR						; Pack
;*/
		MTIR		TEXTURE_V_Perturb , COLORw
		isubiu		TEXTURE_V_Perturb , TEXTURE_V_Perturb , 48
		iadd		TEXTURE_V_Perturb , TEXTURE_V_Perturb , TEXTURE_V_Perturb
		iadd		TEXTURE_V_Perturb , TEXTURE_V_Perturb , TEXTURE_V_Perturb
		iadd		TEXTURE_V_Perturb , TEXTURE_V_Perturb , TEXTURE_V
		
		ITOF0		WORLD , WORLD
		Addw.xy  	WORLD , WORLD , PERTURB_MUL.w		; -128  
		MUL.xyz	 	WORLD , WORLD , PERTURB_MUL			; Pertub factor  
; Add rasterization to point
		add.xyz	 	WORLD , WORLD , RASTER_BAS			; ADD BASE  	
; Transform point 
		mulax		acc,	I_CAMERA, WORLD.x
		madday		acc,	J_CAMERA, WORLD.y
		maddz		WORLD,	K_CAMERA, WORLD.z
		maxw.z		WORLD,	WORLD,	VF00w
; Compute 1 / Z
; Project point
		DIV 		Q,VF00.w , WORLD.z
		MULq.xyzw	SCREEN,WORLD,q
		MULq.z		SCREEN,SCREEN,q	
; mul by SX SY SZ 	
		MUL.xyzw	SCREEN,SCREEN,MUL_SCREEN
		ADD.xyzw	SCREEN,SCREEN,ADD_SCREEN
; Fast clipping max-min
		MAX.xyzw	SCREEN,SCREEN,MAX_SCREEN
		MINI.xyzw	SCREEN,SCREEN,MIN_SCREEN
		FTOI0.xyzw	SCREEN,SCREEN		
		sq.xyzw		COLOR ,OCP_GIF_STEP + 1(p_DEST)
		sq.xyz		SCREEN,OCP_GIF_STEP + 2(p_DEST)
		
		ISW.x		TEXTURE_U , OCP_GIF_STEP + 0(p_DEST)
		ISW.y		TEXTURE_V_Perturb , OCP_GIF_STEP + 0(p_DEST)
		IADD		TEXTURE_U , TEXTURE_U , T_U_INC
		
; Local += inc  
		ADD.xyz		RASTER_BAS , RASTER_BAS , RASTER_INC
; pSrc += increment 
		IADDI		p_DEST , p_DEST , OCP_GIF_STEP * 2
		IBNE		p_DEST , p_DESTLAST , OCEAN_PHIL_LOOP;*/

	
		B			SWITCHER

--exit 
--endexit
