;   1                    OCP_VERTEX_NUMBER .assign 28
;   2                    OCP_VIF_STEP	.assign 2
;   3                    OCP_GIF_STEP	.assign 3
;   4                    OCP_VIF_BASE0 .assign 1
;   5                    OCP_UV_H_JMP	.assign ((128*4)/(OCP_VERTEX_NUMBER - 1))
;   6                    OCP_VIF_BASE1 .assign (OCP_VIF_BASE0 + OCP_VERTEX_NUMBER * OCP_VIF_STEP + 4) 
;   7                    OCP_GIF_BASE0 .assign (OCP_VIF_BASE1 + OCP_VERTEX_NUMBER * OCP_VIF_STEP + 4)
;   8                    OCP_GIF_BASE1 .assign (OCP_GIF_BASE0 + OCP_VERTEX_NUMBER * 2 * OCP_GIF_STEP + 1)
;   9                    OCP_GIF_BASEe .assign (1024)
;  10                    

;  11                    --enter		
	--enter		
;  12                    .init_vf_all  
	.init_vf_all  
;  13                    .init_vi_all
	.init_vi_all
;  14                    --endenter	
	--endenter	
;  15                    

;  16                    /*************************************************************************************************************************
	/*************************************************************************************************************************
;  17                    *************************************************************************************************************************
	*************************************************************************************************************************
;  18                    

;  19                    													INIT
	INIT
;  20                    

;  21                    *************************************************************************************************************************	
	*************************************************************************************************************************	
;  22                    *************************************************************************************************************************/
	*************************************************************************************************************************/
;  23                    

;  24                    OCEAN_PHIL_INIT:
OCEAN_PHIL_INIT:
;  25                    ; 			CAMERA LOAD    
	; 			CAMERA LOAD    
;  26                    		LQ			GIF_TAGA , 	OCP_VIF_BASE0 + 0(VI00) 
	LQ			GIF_TAGA , 	1 + 0(VI00) 
;  27                    		LQ			GIF_TAGB , 	OCP_VIF_BASE0 + 1(VI00) 
	LQ			GIF_TAGB , 	1 + 1(VI00) 
;  28                    		LQ			I_CAMERA , 	OCP_VIF_BASE0 + 2(VI00) 
	LQ			I_CAMERA , 	1 + 2(VI00) 
;  29                    		LQ			J_CAMERA , 	OCP_VIF_BASE0 + 3(VI00)
	LQ			J_CAMERA , 	1 + 3(VI00)
;  30                    		LQ			K_CAMERA , 	OCP_VIF_BASE0 + 4(VI00)
	LQ			K_CAMERA , 	1 + 4(VI00)
;  31                    ; 			PROJECTION INFO LOAD  
	; 			PROJECTION INFO LOAD  
;  32                    		LQ			MUL_SCREEN,	OCP_VIF_BASE0 + 5(VI00) 
	LQ			MUL_SCREEN,	1 + 5(VI00) 
;  33                    		LQ			ADD_SCREEN,	OCP_VIF_BASE0 + 6(VI00)
	LQ			ADD_SCREEN,	1 + 6(VI00)
;  34                    		LQ			MAX_SCREEN,	OCP_VIF_BASE0 + 7(VI00)
	LQ			MAX_SCREEN,	1 + 7(VI00)
;  35                    		LQ			MIN_SCREEN,	OCP_VIF_BASE0 + 8(VI00)
	LQ			MIN_SCREEN,	1 + 8(VI00)
;  36                    ; 			PERTURB FACTOR  
	; 			PERTURB FACTOR  
;  37                    		LQ			PERTURB_MUL,OCP_VIF_BASE0 + 9(VI00)	 ; ! PERTURB MUL. W = -128 !!
	LQ			PERTURB_MUL,1 + 9(VI00)	 ; ! PERTURB MUL. W = -128 !!
;  38                    ; 			SAMPLE OF XYZF2  
	; 			SAMPLE OF XYZF2  
;  39                    		LQ			SXYZF2 , 	OCP_VIF_BASE0 + 10(VI00)
	LQ			SXYZF2 , 	1 + 10(VI00)
;  40                    ; 			SAMPLE OF XYZF3  
	; 			SAMPLE OF XYZF3  
;  41                    		LQ			SXYZF3 , 	OCP_VIF_BASE0 + 11(VI00)
	LQ			SXYZF3 , 	1 + 11(VI00)
;  42                    	
;  43                    		IADDIU 		GIF_BASE , VI00 ,  OCP_GIF_BASE0 ; GIF BASE  
	IADDIU 		GIF_BASE , VI00 ,  121 ; GIF BASE  
;  44                    		IADDIU 		GIF_LAST , VI00 ,  OCP_GIF_BASEe ; GIF BASE  
	IADDIU 		GIF_LAST , VI00 ,  1024 ; GIF BASE  
;  45                    

;  46                    OCEAN_PHIL_INIT_FILLMEM:
OCEAN_PHIL_INIT_FILLMEM:
;  47                    		SQ 			SXYZF2 , 0 (GIF_BASE)
	SQ 			SXYZF2 , 0 (GIF_BASE)
;  48                    		IADDI		GIF_BASE , GIF_BASE , 1
	IADDI		GIF_BASE , GIF_BASE , 1
;  49                    		IBNE		GIF_BASE , GIF_LAST , OCEAN_PHIL_INIT_FILLMEM
	IBNE		GIF_BASE , GIF_LAST , OCEAN_PHIL_INIT_FILLMEM
;  50                    		
;  51                    		SQ			GIF_TAGA , OCP_GIF_BASE0 (VI00)
	SQ			GIF_TAGA , 121 (VI00)
;  52                    		SQ			GIF_TAGB , OCP_GIF_BASE1 (VI00)
	SQ			GIF_TAGB , 290 (VI00)
;  53                    		SQ 			SXYZF3 , OCP_GIF_BASE0 + 1 + OCP_GIF_STEP - 1 (VI00)
	SQ 			SXYZF3 , 121 + 1 + 3 - 1 (VI00)
;  54                    		SQ 			SXYZF3 , OCP_GIF_BASE0 + 1 + OCP_GIF_STEP * 2 - 1 (VI00)
	SQ 			SXYZF3 , 121 + 1 + 3 * 2 - 1 (VI00)
;  55                    		SQ 			SXYZF3 , OCP_GIF_BASE1 + 1 + OCP_GIF_STEP - 1  (VI00)
	SQ 			SXYZF3 , 290 + 1 + 3 - 1  (VI00)
;  56                    		SQ 			SXYZF3 , OCP_GIF_BASE1 + 1 + OCP_GIF_STEP * 2 - 1 (VI00)
	SQ 			SXYZF3 , 290 + 1 + 3 * 2 - 1 (VI00)
;  57                    		 
;  58                    		
;  59                    		IADDIU		State , VI00 , 0
	IADDIU		State , VI00 , 0
;  60                    		
;  61                    /*************************************************************************************************************************
	/*************************************************************************************************************************
;  62                    *************************************************************************************************************************
	*************************************************************************************************************************
;  63                    

;  64                    													STATE SWITCH
	STATE SWITCH
;  65                    

;  66                    *************************************************************************************************************************	
	*************************************************************************************************************************	
;  67                    *************************************************************************************************************************/
	*************************************************************************************************************************/
;  68                    ; switch on different case
	; switch on different case
;  69                    SWITCHER:
SWITCHER:
;  70                    		IADDIU		TEMP , VI00 , 4
	IADDIU		TEMP , VI00 , 4
;  71                    		IBEQ		State , TEMP , OcenPhil_State4
	IBEQ		State , TEMP , OcenPhil_State4
;  72                    		IADDIU		TEMP , VI00 , 3
	IADDIU		TEMP , VI00 , 3
;  73                    		IBEQ		State , TEMP , OcenPhil_State3
	IBEQ		State , TEMP , OcenPhil_State3
;  74                    		IADDIU		TEMP , VI00 , 2
	IADDIU		TEMP , VI00 , 2
;  75                    		IBEQ		State , TEMP , OcenPhil_State2
	IBEQ		State , TEMP , OcenPhil_State2
;  76                    		IADDIU		TEMP , VI00 , 1
	IADDIU		TEMP , VI00 , 1
;  77                    		IBEQ		State , TEMP , OcenPhil_State1
	IBEQ		State , TEMP , OcenPhil_State1
;  78                    ; state 0 : no kick -> state 1
	; state 0 : no kick -> state 1
;  79                    OcenPhil_State0:
OcenPhil_State0:
;  80                    		IADDI		p_STEP 	, VI00 , OCP_VIF_STEP
	IADDI		p_STEP 	, VI00 , 2
;  81                    		IADDIU		T_U_INC , VI00 , OCP_UV_H_JMP
	IADDIU		T_U_INC , VI00 , 18
;  82                    		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE0
	IADDIU		p_DEST  , VI00 , 121
;  83                    		IADDIU		p_SRC   , VI00 , OCP_VIF_BASE0
	IADDIU		p_SRC   , VI00 , 1
;  84                    

;  85                    		IADDI		State	, VI00 , 1
	IADDI		State	, VI00 , 1
;  86                    		B			OCEAN_PHIL_LOOPSTART;*/
	B			OCEAN_PHIL_LOOPSTART;*/
;  87                    		
;  88                    ; state 1 : no kick -> state 2
	; state 1 : no kick -> state 2
;  89                    OcenPhil_State1:
OcenPhil_State1:
;  90                    		IADDI		p_STEP 	, VI00 , -OCP_VIF_STEP
	IADDI		p_STEP 	, VI00 , -2
;  91                    		IADDIU		T_U_INC , VI00 , OCP_UV_H_JMP
	IADDIU		T_U_INC , VI00 , 18
;  92                    		ISUB		T_U_INC , VI00 , T_U_INC
	ISUB		T_U_INC , VI00 , T_U_INC
;  93                    		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE1
	IADDIU		p_DEST  , VI00 , 290
;  94                    		IADDIU		p_SRC   , VI00 , OCP_VIF_BASE1 + OCP_VERTEX_NUMBER * 2
	IADDIU		p_SRC   , VI00 , 61 + 28 * 2
;  95                    

;  96                    		IADDI		State, VI00 , 2
	IADDI		State, VI00 , 2
;  97                    		B			OCEAN_PHIL_LOOPSTART;*/
	B			OCEAN_PHIL_LOOPSTART;*/
;  98                    		
;  99                    ; state 2 : no kick -> state 3 
	; state 2 : no kick -> state 3 
; 100                    OcenPhil_State2:
OcenPhil_State2:
; 101                    		IADDI		p_STEP 	, VI00 , OCP_VIF_STEP
	IADDI		p_STEP 	, VI00 , 2
; 102                    		IADDIU		T_U_INC , VI00 , OCP_UV_H_JMP
	IADDIU		T_U_INC , VI00 , 18
; 103                    		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE0
	IADDIU		p_DEST  , VI00 , 121
; 104                    		IADDIU		p_SRC   , VI00 , OCP_VIF_BASE0
	IADDIU		p_SRC   , VI00 , 1
; 105                    		
; 106                    

; 107                    		IADDI		State, VI00 , 3
	IADDI		State, VI00 , 3
; 108                    		B			OCEAN_PHIL_LOOPSTART;*/
	B			OCEAN_PHIL_LOOPSTART;*/
; 109                    		
; 110                    ; state 3 : kick -> state 4
	; state 3 : kick -> state 4
; 111                    OcenPhil_State3:
OcenPhil_State3:
; 112                    		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE0
	IADDIU		p_DEST  , VI00 , 121
; 113                    		XGKICK		p_DEST
	XGKICK		p_DEST
; 114                    		IADDI		p_STEP 	, VI00 , -OCP_VIF_STEP
	IADDI		p_STEP 	, VI00 , -2
; 115                    		IADDIU		T_U_INC , VI00 , OCP_UV_H_JMP
	IADDIU		T_U_INC , VI00 , 18
; 116                    		ISUB		T_U_INC , VI00 , T_U_INC
	ISUB		T_U_INC , VI00 , T_U_INC
; 117                    		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE1
	IADDIU		p_DEST  , VI00 , 290
; 118                    		IADDIU		p_SRC   , VI00 , OCP_VIF_BASE1 + OCP_VERTEX_NUMBER * 2
	IADDIU		p_SRC   , VI00 , 61 + 28 * 2
; 119                    

; 120                    		IADDI		State, VI00 , 4
	IADDI		State, VI00 , 4
; 121                    		B			OCEAN_PHIL_LOOPSTART;*/
	B			OCEAN_PHIL_LOOPSTART;*/
; 122                    

; 123                    ; state 2 : kick -> state 3
	; state 2 : kick -> state 3
; 124                    OcenPhil_State4:
OcenPhil_State4:
; 125                    /* Copy the 2 Last point of OCP_GIF_BASE0 at the end of GIFBASE1 */ 
	/* Copy the 2 Last point of 121 at the end of GIFBASE1 */ 
; 126                    

; 127                    		IADDIU		p_SRC   , VI00 , OCP_GIF_BASE0 + 1 + (OCP_VERTEX_NUMBER - 1) * OCP_GIF_STEP * 2
	IADDIU		p_SRC   , VI00 , 121 + 1 + (28 - 1) * 3 * 2
; 128                    		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE1 + 1 + OCP_VERTEX_NUMBER * OCP_GIF_STEP * 2
	IADDIU		p_DEST  , VI00 , 290 + 1 + 28 * 3 * 2
; 129                    		lq			Local0 , OCP_GIF_STEP * 1 - 1(p_SRC)
	lq			Local0 , 3 * 1 - 1(p_SRC)
; 130                    		sq			Local0 , OCP_GIF_STEP * 1 - 1(p_DEST)
	sq			Local0 , 3 * 1 - 1(p_DEST)
; 131                    		lq			Local0 , OCP_GIF_STEP * 1 - 2(p_SRC)
	lq			Local0 , 3 * 1 - 2(p_SRC)
; 132                    		sq			Local0 , OCP_GIF_STEP * 1 - 2(p_DEST)
	sq			Local0 , 3 * 1 - 2(p_DEST)
; 133                    		lq			Local0 , OCP_GIF_STEP * 1 - 3(p_SRC)
	lq			Local0 , 3 * 1 - 3(p_SRC)
; 134                    		sq			Local0 , OCP_GIF_STEP * 1 - 3(p_DEST)
	sq			Local0 , 3 * 1 - 3(p_DEST)
; 135                    		lq			Local0 , OCP_GIF_STEP * 2 - 1(p_SRC)
	lq			Local0 , 3 * 2 - 1(p_SRC)
; 136                    		sq			Local0 , OCP_GIF_STEP * 2 - 1(p_DEST)
	sq			Local0 , 3 * 2 - 1(p_DEST)
; 137                    		lq			Local0 , OCP_GIF_STEP * 2 - 2(p_SRC)
	lq			Local0 , 3 * 2 - 2(p_SRC)
; 138                    		sq			Local0 , OCP_GIF_STEP * 2 - 2(p_DEST)
	sq			Local0 , 3 * 2 - 2(p_DEST)
; 139                    		lq			Local0 , OCP_GIF_STEP * 2 - 3(p_SRC)
	lq			Local0 , 3 * 2 - 3(p_SRC)
; 140                    		sq			Local0 , OCP_GIF_STEP * 2 - 3(p_DEST)
	sq			Local0 , 3 * 2 - 3(p_DEST)
; 141                    		
; 142                    	
; 143                    

; 144                    		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE1
	IADDIU		p_DEST  , VI00 , 290
; 145                    		XGKICK		p_DEST
	XGKICK		p_DEST
; 146                    		
; 147                    		IADDI		p_STEP 	, VI00 , OCP_VIF_STEP
	IADDI		p_STEP 	, VI00 , 2
; 148                    		IADDIU		T_U_INC , VI00 , OCP_UV_H_JMP
	IADDIU		T_U_INC , VI00 , 18
; 149                    		IADDIU		p_DEST  , VI00 , OCP_GIF_BASE0
	IADDIU		p_DEST  , VI00 , 121
; 150                    		IADDIU		p_SRC   , VI00 , OCP_VIF_BASE0
	IADDIU		p_SRC   , VI00 , 1
; 151                    		
; 152                    

; 153                    		
; 154                    

; 155                    		IADDI		State, VI00 , 3
	IADDI		State, VI00 , 3
; 156                    		B			OCEAN_PHIL_LOOPSTART;*/
	B			OCEAN_PHIL_LOOPSTART;*/
; 157                    		
; 158                    		
; 159                    

; 160                    /*************************************************************************************************************************
	/*************************************************************************************************************************
; 161                    *************************************************************************************************************************
	*************************************************************************************************************************
; 162                    

; 163                    													MAIN LOOP 
	MAIN LOOP 
; 164                    

; 165                    *************************************************************************************************************************	
	*************************************************************************************************************************	
; 166                    *************************************************************************************************************************/
	*************************************************************************************************************************/
; 167                    OCEAN_PHIL_LOOPSTART:
OCEAN_PHIL_LOOPSTART:
; 168                    --cont	
	--cont	
; 169                    		LQ.xyzw		CAMERAVEC0 , 0(VI00)
	LQ.xyzw		CAMERAVEC0 , 0(VI00)
; 170                    		LQ.xyzw		CNPARAMS,1(VI00)
	LQ.xyzw		CNPARAMS,1(VI00)
; 171                    		LQ.xyzw		G_COLOR,2(VI00)
	LQ.xyzw		G_COLOR,2(VI00)
; 172                    /* Compute normales END **********************************************************/
	/* Compute normales END **********************************************************/
; 173                    

; 174                    /*SHIFT DEST GIF BEGIN **********************************************************/
	/*SHIFT DEST GIF BEGIN **********************************************************/
; 175                    ; jump over Gif tag
	; jump over Gif tag
; 176                    		IADDIU		p_DEST , p_DEST , 1 
	IADDIU		p_DEST , p_DEST , 1 
; 177                    		IADDIU		p_DESTSave , p_DEST , 0
	IADDIU		p_DESTSave , p_DEST , 0
; 178                    ; compute last pointer		
	; compute last pointer		
; 179                    		IADDIU		p_DESTLAST , p_DEST , OCP_VERTEX_NUMBER * OCP_GIF_STEP * 2
	IADDIU		p_DESTLAST , p_DEST , 28 * 3 * 2
; 180                    SHIFT_LOOP:
SHIFT_LOOP:
; 181                    		lq			Local5 , OCP_GIF_STEP * 4 - 3(p_DEST) ; UV
	lq			Local5 , 3 * 4 - 3(p_DEST) ; UV
; 182                    		lq			Local3 , OCP_GIF_STEP * 4 - 2(p_DEST) ; color
	lq			Local3 , 3 * 4 - 2(p_DEST) ; color
; 183                    		lq			Local4 , OCP_GIF_STEP * 4 - 1(p_DEST) ; XYZ
	lq			Local4 , 3 * 4 - 1(p_DEST) ; XYZ
; 184                    		sq.xyzw		Local5 , OCP_GIF_STEP * 3 - 3(p_DEST) ; UV
	sq.xyzw		Local5 , 3 * 3 - 3(p_DEST) ; UV
; 185                    		sq.xyzw		Local3 , OCP_GIF_STEP * 3 - 2(p_DEST) ; color
	sq.xyzw		Local3 , 3 * 3 - 2(p_DEST) ; color
; 186                    		sq.xyz		Local4 , OCP_GIF_STEP * 3 - 1(p_DEST) ; XYZ
	sq.xyz		Local4 , 3 * 3 - 1(p_DEST) ; XYZ
; 187                    		lq			Local2 , OCP_GIF_STEP * 2 - 3(p_DEST) ; UV
	lq			Local2 , 3 * 2 - 3(p_DEST) ; UV
; 188                    		lq			Local0 , OCP_GIF_STEP * 2 - 2(p_DEST) ; color
	lq			Local0 , 3 * 2 - 2(p_DEST) ; color
; 189                    		lq			Local1 , OCP_GIF_STEP * 2 - 1(p_DEST) ; XYZ
	lq			Local1 , 3 * 2 - 1(p_DEST) ; XYZ
; 190                    		sq.xyzw		Local2 , OCP_GIF_STEP * 1 - 3(p_DEST) ; UV
	sq.xyzw		Local2 , 3 * 1 - 3(p_DEST) ; UV
; 191                    		sq.xyzw		Local0 , OCP_GIF_STEP * 1 - 2(p_DEST) ; color
	sq.xyzw		Local0 , 3 * 1 - 2(p_DEST) ; color
; 192                    		sq.xyz		Local1 , OCP_GIF_STEP * 1 - 1(p_DEST) ; XYZ
	sq.xyz		Local1 , 3 * 1 - 1(p_DEST) ; XYZ
; 193                    		IADDIU		p_DEST , p_DEST , OCP_GIF_STEP * 4
	IADDIU		p_DEST , p_DEST , 3 * 4
; 194                    		IBNE		p_DEST , p_DESTLAST , SHIFT_LOOP;*/
	IBNE		p_DEST , p_DESTLAST , SHIFT_LOOP;*/
; 195                    ; Restore p_DEST
	; Restore p_DEST
; 196                    		IADDIU		p_DEST , p_DESTSave , 0
	IADDIU		p_DEST , p_DESTSave , 0
; 197                    		
; 198                    ; Load Rasters
	; Load Rasters
; 199                    

; 200                    		addw.w		WORLD,VF00,VF00w	; WORLD.w = 1.0f
	addw.w		WORLD,VF00,VF00w	; WORLD.w = 1.0f
; 201                    

; 202                    

; 203                    		LQ.xyz		RASTER_BAS , 0(p_SRC) ;BASE  
	LQ.xyz		RASTER_BAS , 0(p_SRC) ;BASE  
; 204                    		LQ.xyz		RASTER_INC , 1(p_SRC) ; INC  
	LQ.xyz		RASTER_INC , 1(p_SRC) ; INC  
; 205                    		
; 206                    		ILW.w		TEXTURE_U , 0(p_SRC)
	ILW.w		TEXTURE_U , 0(p_SRC)
; 207                    		ILW.w		TEXTURE_V , 1(p_SRC)
	ILW.w		TEXTURE_V , 1(p_SRC)
; 208                    

; 209                    		IADD		T_U_INC , T_U_INC , T_U_INC ;* 2
	IADD		T_U_INC , T_U_INC , T_U_INC ;* 2
; 210                    		IADD		T_U_INC , T_U_INC , T_U_INC ;* 4
	IADD		T_U_INC , T_U_INC , T_U_INC ;* 4
; 211                    

; 212                    /* LOOP ************************************************************************/
	/* LOOP ************************************************************************/
; 213                    OCEAN_PHIL_LOOP:
OCEAN_PHIL_LOOP:
; 214                    

; 215                    		--LoopCS	8,1
	--LoopCS	8,1
; 216                    

; 217                    ; Load Perturb
	; Load Perturb
; 218                    		IADD		p_SRC , p_SRC , p_STEP
	IADD		p_SRC , p_SRC , p_STEP
; 219                    		
; 220                    		LQ.xyz		WORLD , 1(p_SRC) ; Perturb  
	LQ.xyz		WORLD , 1(p_SRC) ; Perturb  
; 221                    		LQ.xyzw		COLOR , 0(p_SRC) ; Color 
	LQ.xyzw		COLOR , 0(p_SRC) ; Color 
; 222                    /*
	/*
; 223                    		itof0		COLOR , COLOR						; -128.0f
	itof0		COLOR , COLOR						; -128.0f
; 224                    		addw		COLOR , COLOR , PERTURB_MUL.w 		; - 128.0f
	addw		COLOR , COLOR , PERTURB_MUL.w 		; - 128.0f
; 225                    		mul			COLOR , COLOR , CAMERAVEC0			; *= CameraVec0
	mul			COLOR , COLOR , CAMERAVEC0			; *= CameraVec0
; 226                    		addy.x 		COLOR , COLOR , COLOR				; X += Y 
	addy.x 		COLOR , COLOR , COLOR				; X += Y 
; 227                    		addz.x 		COLOR , COLOR , COLOR				; X += Z 
	addz.x 		COLOR , COLOR , COLOR				; X += Z 
; 228                    		mulx.x		COLOR , COLOR , COLOR				; Coef *= Coef
	mulx.x		COLOR , COLOR , COLOR				; Coef *= Coef
; 229                    		sub.x		COLOR , CNPARAMS , COLOR			; 1.0f - Coef 
	sub.x		COLOR , CNPARAMS , COLOR			; 1.0f - Coef 
; 230                    		mulx.x		COLOR , COLOR , COLOR				; Coef *= Coef
	mulx.x		COLOR , COLOR , COLOR				; Coef *= Coef
; 231                    		mulx.x		COLOR , COLOR , COLOR				; Coef *= Coef
	mulx.x		COLOR , COLOR , COLOR				; Coef *= Coef
; 232                    		mulx.x		COLOR , COLOR , COLOR				; Coef *= Coef
	mulx.x		COLOR , COLOR , COLOR				; Coef *= Coef
; 233                    		mulx 		COLOR , G_COLOR , COLORx			; Color
	mulx 		COLOR , G_COLOR , COLORx			; Color
; 234                    		ftoi0		COLOR , COLOR						; Pack
	ftoi0		COLOR , COLOR						; Pack
; 235                    ;*/
	;*/
; 236                    

; 237                    /*
	/*
; 238                    		itof0		COLOR , COLOR						; -128.0f
	itof0		COLOR , COLOR						; -128.0f
; 239                    		addw		COLOR , COLOR , PERTURB_MUL.w 		; - 128.0f
	addw		COLOR , COLOR , PERTURB_MUL.w 		; - 128.0f
; 240                    		mul			ALPHAC , ALPHAC , CAMERAVEC0		; *= CameraVec0
	mul			ALPHAC , ALPHAC , CAMERAVEC0		; *= CameraVec0
; 241                    		addy.x 		COLOR , ALPHAC , ALPHAC				; X += Y 
	addy.x 		COLOR , ALPHAC , ALPHAC				; X += Y 
; 242                    		addz.x 		ALPHAC , ALPHAC , ALPHAC			; X += Z 
	addz.x 		ALPHAC , ALPHAC , ALPHAC			; X += Z 
; 243                    		mulx.xyz	ALPHAC , COLOR , ALPHACx			; Coef *= Coef
	mulx.xyz	ALPHAC , COLOR , ALPHACx			; Coef *= Coef
; 244                    		add.xyz		ALPHAC , ALPHAC , CAMERAVEC0		; 1.0f - Coef 
	add.xyz		ALPHAC , ALPHAC , CAMERAVEC0		; 1.0f - Coef 
; 245                    		mul			ALPHAC , ALPHAC , CAMERAVEC0		; *= CameraVec0
	mul			ALPHAC , ALPHAC , CAMERAVEC0		; *= CameraVec0
; 246                    		addy.x 		ALPHAC , ALPHAC , ALPHAC			; X += Y 
	addy.x 		ALPHAC , ALPHAC , ALPHAC			; X += Y 
; 247                    		addz.x 		ALPHAC , ALPHAC , ALPHAC			; X += Z 
	addz.x 		ALPHAC , ALPHAC , ALPHAC			; X += Z 
; 248                    		mulx 		COLOR , G_COLOR , COLORx			; Color
	mulx 		COLOR , G_COLOR , COLORx			; Color
; 249                    		ftoi0		COLOR , COLOR						; Pack
	ftoi0		COLOR , COLOR						; Pack
; 250                    ;*/
	;*/
; 251                    		MTIR		TEXTURE_V_Perturb , COLORw
	MTIR		TEXTURE_V_Perturb , COLORw
; 252                    		isubiu		TEXTURE_V_Perturb , TEXTURE_V_Perturb , 48
	isubiu		TEXTURE_V_Perturb , TEXTURE_V_Perturb , 48
; 253                    		iadd		TEXTURE_V_Perturb , TEXTURE_V_Perturb , TEXTURE_V_Perturb
	iadd		TEXTURE_V_Perturb , TEXTURE_V_Perturb , TEXTURE_V_Perturb
; 254                    		iadd		TEXTURE_V_Perturb , TEXTURE_V_Perturb , TEXTURE_V_Perturb
	iadd		TEXTURE_V_Perturb , TEXTURE_V_Perturb , TEXTURE_V_Perturb
; 255                    		iadd		TEXTURE_V_Perturb , TEXTURE_V_Perturb , TEXTURE_V
	iadd		TEXTURE_V_Perturb , TEXTURE_V_Perturb , TEXTURE_V
; 256                    		
; 257                    		ITOF0		WORLD , WORLD
	ITOF0		WORLD , WORLD
; 258                    		Addw.xy  	WORLD , WORLD , PERTURB_MUL.w		; -128  
	Addw.xy  	WORLD , WORLD , PERTURB_MUL.w		; -128  
; 259                    		MUL.xyz	 	WORLD , WORLD , PERTURB_MUL			; Pertub factor  
	MUL.xyz	 	WORLD , WORLD , PERTURB_MUL			; Pertub factor  
; 260                    ; Add rasterization to point
	; Add rasterization to point
; 261                    		add.xyz	 	WORLD , WORLD , RASTER_BAS			; ADD BASE  	
	add.xyz	 	WORLD , WORLD , RASTER_BAS			; ADD BASE  	
; 262                    ; Transform point 
	; Transform point 
; 263                    		mulax		acc,	I_CAMERA, WORLD.x
	mulax		acc,	I_CAMERA, WORLD.x
; 264                    		madday		acc,	J_CAMERA, WORLD.y
	madday		acc,	J_CAMERA, WORLD.y
; 265                    		maddz		WORLD,	K_CAMERA, WORLD.z
	maddz		WORLD,	K_CAMERA, WORLD.z
; 266                    		maxw.z		WORLD,	WORLD,	VF00w
	maxw.z		WORLD,	WORLD,	VF00w
; 267                    ; Compute 1 / Z
	; Compute 1 / Z
; 268                    ; Project point
	; Project point
; 269                    		DIV 		Q,VF00.w , WORLD.z
	DIV 		Q,VF00.w , WORLD.z
; 270                    		MULq.xyzw	SCREEN,WORLD,q
	MULq.xyzw	SCREEN,WORLD,q
; 271                    		MULq.z		SCREEN,SCREEN,q	
	MULq.z		SCREEN,SCREEN,q	
; 272                    ; mul by SX SY SZ 	
	; mul by SX SY SZ 	
; 273                    		MUL.xyzw	SCREEN,SCREEN,MUL_SCREEN
	MUL.xyzw	SCREEN,SCREEN,MUL_SCREEN
; 274                    		ADD.xyzw	SCREEN,SCREEN,ADD_SCREEN
	ADD.xyzw	SCREEN,SCREEN,ADD_SCREEN
; 275                    ; Fast clipping max-min
	; Fast clipping max-min
; 276                    		MAX.xyzw	SCREEN,SCREEN,MAX_SCREEN
	MAX.xyzw	SCREEN,SCREEN,MAX_SCREEN
; 277                    		MINI.xyzw	SCREEN,SCREEN,MIN_SCREEN
	MINI.xyzw	SCREEN,SCREEN,MIN_SCREEN
; 278                    		FTOI0.xyzw	SCREEN,SCREEN		
	FTOI0.xyzw	SCREEN,SCREEN		
; 279                    		sq.xyzw		COLOR ,OCP_GIF_STEP + 1(p_DEST)
	sq.xyzw		COLOR ,3 + 1(p_DEST)
; 280                    		sq.xyz		SCREEN,OCP_GIF_STEP + 2(p_DEST)
	sq.xyz		SCREEN,3 + 2(p_DEST)
; 281                    		
; 282                    		ISW.x		TEXTURE_U , OCP_GIF_STEP + 0(p_DEST)
	ISW.x		TEXTURE_U , 3 + 0(p_DEST)
; 283                    		ISW.y		TEXTURE_V_Perturb , OCP_GIF_STEP + 0(p_DEST)
	ISW.y		TEXTURE_V_Perturb , 3 + 0(p_DEST)
; 284                    		IADD		TEXTURE_U , TEXTURE_U , T_U_INC
	IADD		TEXTURE_U , TEXTURE_U , T_U_INC
; 285                    		
; 286                    ; Local += inc  
	; Local += inc  
; 287                    		ADD.xyz		RASTER_BAS , RASTER_BAS , RASTER_INC
	ADD.xyz		RASTER_BAS , RASTER_BAS , RASTER_INC
; 288                    ; pSrc += increment 
	; pSrc += increment 
; 289                    		IADDI		p_DEST , p_DEST , OCP_GIF_STEP * 2
	IADDI		p_DEST , p_DEST , 3 * 2
; 290                    		IBNE		p_DEST , p_DESTLAST , OCEAN_PHIL_LOOP;*/
	IBNE		p_DEST , p_DESTLAST , OCEAN_PHIL_LOOP;*/
; 291                    

; 292                    	
; 293                    		B			SWITCHER
	B			SWITCHER
; 294                    

; 295                    --exit 
	--exit 
; 296                    --endexit
	--endexit
; 297                    