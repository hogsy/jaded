/*******************************************************************************************
	Jungle code for Jade
	Philippe Vimont 2004
	Generate big leaves(ex coconuts) or branches & bend then 
	1 color per leave,
	1 fog per leave,
	1,2,4 or 8 segments per leave
	fast culling - clipping method
	each leave is defined with 4 vertice (Start 1 , Start 2 , NormaleBegin , NormaleEnd)
*******************************************************************************************/

InputPackSize	.assign 64
OutputPackSize	.assign	20*3
VIF_XYZW_STEP	.assign	4
VIF_COLOR_STEP	.assign	1
GIF_STEP		.assign	3
VIFBASE0_XYZW	.assign	4
VIFBASE0_COLOR	.assign (VIFBASE0_XYZW + VIF_XYZW_STEP * InputPackSize )
GIF_BASE0		.assign 512 - OutputPackSize - 8 - 8

VIFBASE1_XYZW	.assign VIFBASE0_XYZW	+	512
VIFBASE1_COLOR	.assign VIFBASE0_COLOR 	+ 	512
GIF_BASE1		.assign GIF_BASE0		+	512

CLIPPING_RESULT	.assign vi01
CLIP_XP			.assign	1
CLIP_XM			.assign	2
CLIP_YP			.assign	4
CLIP_YM			.assign	8
CLIP_ZP			.assign	16
CLIP_ZM			.assign	32

FOG_KIGK_BASE	.assign 1024 - 2


.macro	SWITCH_BASE	Reg
	iaddiu		Register1023 , VI00 , 1023
	iaddiu		\Reg , \Reg , 512
.endm	


.macro	KICK_PRIM	
		iaddiu				TmpKick , VI00 , FOG_KIGK_BASE
		XGKICK				TmpKick
		XGKICK							GIF_BASE ; Draw 1 sprite*/
		SWITCH_BASE						GIF_BASE
.endm	

/*===========================================================================================
VIF header input structure ==================================================================
============================================================================================*/

.macro	LOAD_PARAMS		SOURCE_PTR SOURCE_COLOR_PTR
		Iadd		\SOURCE_PTR 	, VI00 		, VIF_BASE
		LQ			NUMP_TILE_NumSegs_Trapeze, 	0(\SOURCE_PTR) 
		ITOF0.yzw	NUMP_TILE_NumSegs_Trapeze, 	NUMP_TILE_NumSegs_Trapeze
		ILW.z		SegmentNumber , 0(\SOURCE_PTR)
		ILW.x		CurrentMode , 1(\SOURCE_PTR)
		ILW.y		SpriteNumber , 1(\SOURCE_PTR)
		LQ.zw		SU_SV_void_void , 	1 (\SOURCE_PTR) 
		addz.x		SU_SV_void_void , VF00 , SU_SV_void_void
		addw.y		SU_SV_void_void , VF00 , SU_SV_void_void
		sub.zw		SU_SV_void_void , VF00 , VF00
		
		lq.xyzw		GlobalAdd , 2(\SOURCE_PTR)
		lq.xyzw		GlobalZadd , 3(\SOURCE_PTR)
		lq.w		PointUV 	, 3(\SOURCE_PTR)
		mulw.x		PointUV 	, SU_SV_void_void 	, PointUV 	
		sub.yzw		PointUV 	, VF00	, VF00
		
		
		
		Iaddiu		\SOURCE_COLOR_PTR 	, \SOURCE_PTR	, VIFBASE0_COLOR
		iaddiu		\SOURCE_PTR 		, \SOURCE_PTR , VIFBASE0_XYZW
		DIV 		Q,VF00.w , NUMP_TILE_NumSegs_Trapeze.z			; 1.0f / number of segments
		SUB			UVIncr , VF00 ,	VF00			; DeltaU = (float)TileNumber / (float)ulNumberOfSegments;
		mulq.y		UVIncr , NUMP_TILE_NumSegs_Trapeze , Q			; DeltaU = (float)TileNumber / (float)ulNumberOfSegments;
		mul.y		UVIncr , UVIncr , SU_SV_void_void
		subw.z		ParameterInc , VF00 , NUMP_TILE_NumSegs_Trapeze	; fTrapezeDelta(ParameterInc.z) = -fTrapeze(NUMP_TILE_NumSegs_Trapeze.w) / (float)(ulNumberOfSegments(Q));
		mulq.z		ParameterInc , ParameterInc , Q
		addq.x		Oonseg_fRatio_fEOHP_VOID , VF00 , Q
		addw.z		PointUV 	, VF00	, VF00w
		sub.y		SU_SV_void_void , VF00 , SU_SV_void_void
.endm

.macro	LOAD_HEADER		SOURCE_PTR 
		Iadd		\SOURCE_PTR 	, VI00 		, VIF_BASE
		/* Copy Wind */
		LQ			WIND , 	0 (\SOURCE_PTR) 
		SQ			WIND , 	512 - 8 (VI00) 
		LQ			WIND , 	1 (\SOURCE_PTR) 
		SQ			WIND , 	512 - 7 (VI00) 
		LQ			WIND , 	2 (\SOURCE_PTR) 
		SQ			WIND , 	512 - 6 (VI00) 
		LQ			WIND , 	3 (\SOURCE_PTR) 
		SQ			WIND , 	512 - 5 (VI00) 
		LQ			WIND , 	4 (\SOURCE_PTR) 
		SQ			WIND , 	512 - 4 (VI00) 
		LQ			WIND , 	5 (\SOURCE_PTR) 
		SQ			WIND , 	512 - 3 (VI00) 
		LQ			WIND , 	6 (\SOURCE_PTR) 
		SQ			WIND , 	512 - 2 (VI00) 
		LQ			WIND , 	7 (\SOURCE_PTR) 
		SQ			WIND , 	512 - 1 (VI00) 
		
		Iaddiu		\SOURCE_PTR 	, \SOURCE_PTR	, 8

		LQ			I_CAMERA , 	0 (\SOURCE_PTR) 
		LQ			J_CAMERA , 	1 (\SOURCE_PTR) 
		LQ			K_CAMERA , 	2 (\SOURCE_PTR) 
		LQ			S_CAMERA , 	3 (\SOURCE_PTR) 

		LQ			SprteGeneratorRadius_OoSpriteNumber_SpriteSize_SqrEoHpO2 , 	4 (\SOURCE_PTR) 
		
		LQ			ADD_SCREEN,	5 (\SOURCE_PTR) 
		LQ			MAX_SCREEN,	6 (\SOURCE_PTR) 
		LQ			MIN_SCREEN,	7 (\SOURCE_PTR) 
		LQ			GIF_TAG , 	8(\SOURCE_PTR) 
		SQ			GIF_TAG , 	FOG_KIGK_BASE(VI00) 
		
		LQ			Oonseg_fRatio_fEOHP_VOID,	9 (\SOURCE_PTR) 
		LQ			GIF_TAG , 	10(\SOURCE_PTR) 
		LQ			COSAS_SINAS_One_FOGMul , 	11(\SOURCE_PTR) 
		addw.z		COSAS_SINAS_One_FOGMul , VF00 , VF00w
		Isubiu		\SOURCE_PTR 	, \SOURCE_PTR	, 8
		

.endm

.macro	LOAD_NUMBER_Of_Branches		REG , SOURCE_PTR 
		ILW.x			\REG , 	0(\SOURCE_PTR)
.endm		
/*
VIFBASE0_HEADER:
	CAMERA_I 		0
	CAMERA_J 		1
	CAMERA_K 		2
	CAMERA_S 		3
	--------		4
	ADD_SCREEN		5
	MAX_SCREEN		6
	MIN_SCREEN		7
	------			8
	------			9
	GIF_TAG			10
	NUMP_TILE_NumSegs_Trapeze
	NumberOfPoints	11.x
	TileNumber		11.y
	NumberOfSegment	11.z
	trapeze			11.w
*/
.macro LOAD_A_LEAF_X
		muly.w				Oonseg_fRatio_fEOHP_VOID , YAw , Oonseg_fRatio_fEOHP_VOID
		mulaw				acc , Point0 , VF00w
		msubw				Point0 , YA ,  Oonseg_fRatio_fEOHP_VOIDw
		maddw				Point1 , YA ,  Oonseg_fRatio_fEOHP_VOIDw
		mulw.xyz			ZA , ZA , YAw
		mulx.xyz			ZA , ZA , Oonseg_fRatio_fEOHP_VOID
		mulw.xyz			Gravity_ , Gravity_ , YAw
		mulx.xyz			Gravity_ , Gravity_ , Oonseg_fRatio_fEOHP_VOID
		mulx.w				ParameterInc , Gravity_ , Oonseg_fRatio_fEOHP_VOID
		TRANSFORM_POINT		Point0 , Point0
		TRANSFORM_POINT		Point1 , Point1
		TURN_POINT			ZA,ZA
		TURN_POINT			Gravity_,Gravity_
		
		
.endm
.macro LOAD_A_LEAF_Y
		muly.w				Oonseg_fRatio_fEOHP_VOID , YAw , Oonseg_fRatio_fEOHP_VOID
		mulaw				acc , Point0 , VF00w
		msubw				Point0 , XA ,  Oonseg_fRatio_fEOHP_VOIDw
		maddw				Point1 , XA ,  Oonseg_fRatio_fEOHP_VOIDw
		mulw.xyz			ZA , ZA , YAw
		mulx.xyz			ZA , ZA , Oonseg_fRatio_fEOHP_VOID
		mulw.xyz			Gravity_ , Gravity_ , YAw
		mulx.xyz			Gravity_ , Gravity_ , Oonseg_fRatio_fEOHP_VOID
		mulx.w				ParameterInc , Gravity_ , Oonseg_fRatio_fEOHP_VOID
		TRANSFORM_POINT		Point0 , Point0
		TRANSFORM_POINT		Point1 , Point1
		TURN_POINT			ZA,ZA
		TURN_POINT			Gravity_,Gravity_
		
		
.endm
.macro EXTRACT_WITH_fEOHP

		sub					Local0 , Gravity_ , ZA
		mulw				Local0 , Local0 , SprteGeneratorRadius_OoSpriteNumber_SpriteSize_SqrEoHpO2
		mulw				Local0 , Local0 , Gravity_ 

		mulz				Local1 , ZA , Oonseg_fRatio_fEOHP_VOID
		add					Local0 , Local0 , Local1

		add.xyz				Point0 , Point0 , Local0
.endm

.macro LOAD_A_LEAF_H
		mulw.xyz			ZA , ZA , YAw
		mulw.xyz			Gravity_ , Gravity_ , YAw

		EXTRACT_WITH_fEOHP
/*
HAT  !!!!!!!!
*/
		muly.w				Oonseg_fRatio_fEOHP_VOID , YAw , Oonseg_fRatio_fEOHP_VOID
		mulw				XA , XA , Oonseg_fRatio_fEOHP_VOID
		mulw				YA , YA , Oonseg_fRatio_fEOHP_VOID
		sub					Point0 , Point0 , XA
		sub					Point0 , Point0 , YA
		add					Point1 , Point0 , YA
		add					Point1 , Point1 , YA
		add					ZA , XA , XA
		add					Gravity_ , XA , XA
		sub.w				ParameterInc , VF00 , VF00

		TRANSFORM_POINT		Point0 , Point0
		TRANSFORM_POINT		Point1 , Point1
		TURN_POINT			ZA,ZA
		TURN_POINT			Gravity_,Gravity_
		
		
.endm


.macro LOAD_A_LEAF_S
		mulw.xyz			ZA , ZA , YAw
		mulw.xyz			Gravity_ , Gravity_ , YAw

		EXTRACT_WITH_fEOHP

		muly.w				Oonseg_fRatio_fEOHP_VOID , YAw , Oonseg_fRatio_fEOHP_VOID
		mulx.w				Oonseg_fRatio_fEOHP_VOID , Oonseg_fRatio_fEOHP_VOID , SprteGeneratorRadius_OoSpriteNumber_SpriteSize_SqrEoHpO2 ; P3 = SpriteGeneratorRadius

		TURN_POINT			XA,XA
		TURN_POINT			YA,YA

		mulw				XA , XA , Oonseg_fRatio_fEOHP_VOIDw
		mulw				YA , YA , Oonseg_fRatio_fEOHP_VOIDw
		
		subz.w				Oonseg_fRatio_fEOHP_VOID , VF00 , Oonseg_fRatio_fEOHP_VOID
		muly.w				Oonseg_fRatio_fEOHP_VOID , Oonseg_fRatio_fEOHP_VOID , SprteGeneratorRadius_OoSpriteNumber_SpriteSize_SqrEoHpO2 ; P2 = OoSpriteNumber
		
		
		mulw.xyz			ZA , ZA , Oonseg_fRatio_fEOHP_VOID ; P2 = OoSpriteNumber
		mulw.xyz			Gravity_ , Gravity_ , Oonseg_fRatio_fEOHP_VOID ; P2 = OoSpriteNumber
		mulW.w				ParameterInc , Gravity_ , Oonseg_fRatio_fEOHP_VOID
		mulz.w				Parameter , Gravity_ , Oonseg_fRatio_fEOHP_VOID
		
		TRANSFORM_POINT		Point0 , Point0
		TURN_POINT			ZA,ZA
		TURN_POINT			Gravity_,Gravity_
.endm



/*==========================================================================================
VIF Input structure  =======================================================================
============================================================================================*/
.macro LOAD_A_LEAF  SOURCE_PTR	SOURCE_COLOR_PTR SUB_MACRO
		
		FCSET				0x000000
		IADDIU				DST_Ptr 	, GIF_BASE 		, 1
		sub.w				Parameter , VF00 , VF00			; Interpolator = 0.0f;
		addw.z				Parameter , VF00 , NUMP_TILE_NumSegs_Trapeze	; fTrapezeInc = 	fTrapeze ; 
		lq					Point0 		, 0(\SOURCE_PTR) 
		add.xyz				Point0 , Point0 , GlobalAdd
		lq					XA	, 1(\SOURCE_PTR) 
		lq					YA 	, 2(\SOURCE_PTR) 
		lq					ZA 	, 3(\SOURCE_PTR) 
		mulw.xyz			ZA 	, ZA 	, GlobalAdd
		add.xyz				ZA 	, ZA 	, GlobalZadd
		
/*
		MATH_AddVector((MATH_tdst_Vector *)&stNormale ,(MATH_tdst_Vector *)&stNormale ,&p_stII->GlobalZADD);
		MATH_ScaleEqualVector((MATH_tdst_Vector *)&stNormale , p_stII->GlobalSCale);
*/		
		ILW.w				WindSelector, 3(\SOURCE_PTR) 
		lq					Gravity_ 	, 512 - 8 (WindSelector) 
		lqi.xyz				PColor 		, (\SOURCE_COLOR_PTR++) 
		Iaddiu				\SOURCE_PTR 	, \SOURCE_PTR , VIF_XYZW_STEP
		
		\SUB_MACRO
		
		/* Fog is compute one time with point0 */
		
		
		/* Compute UV Tile Animation */
		
		add.x				PointUV , PointUV , SU_SV_void_void
		
		
		/* Transform PointUV.x in POINTUV0.xy & POINTUV1.xy */
		ftoi0.x 			TempUV0 , PointUV 
		itof0.x				TempUV0 , TempUV0
		sub.x				PointUV0 , PointUV , TempUV0
		mulx.y				PointUV0 , SU_SV_void_void , TempUV0

		ftoi0.y 			TempUV0 , PointUV0 
		itof0.y				TempUV0 , TempUV0
		sub.y				PointUV0 , PointUV0 , TempUV0

		addw.z				PointUV0 	, VF00	, VF00
		add.xyz				PointUV1 	, PointUV0	, SU_SV_void_void
		sub.y				PointUV1 , PointUV1 , SU_SV_void_void
.endm
/*
VIFBASE0_XYZW VIFBASE1_XYZW 			(VIF_XYZW_STEP = 4):
	PA 0
	PB 0
	Z  0
	G  0
	PA 1
	PB 1
	Z  1
	G  1
	PA 2
	PB 2
	....
	G  InputPackSize
VIFBASE0_COLOR VIFBASE1_COLOR			(VIF_COLOR_STEP = 1):
	Color 0
	Color 1
	Color 2
	Color 3
	....
	Color  InputPackSize - 1
============================================================================================
GIF Output structure (strip mode) ==========================================================
============================================================================================*/
.macro	SAVE_A_LEAF		DEST_PTR
		sq.xyz				TempUV0 , 0(\DEST_PTR)
		sq.xyz				TempUV1 , GIF_STEP(\DEST_PTR)

		sq					PColor , 1(\DEST_PTR)
		sq					PColor , GIF_STEP + 1(\DEST_PTR)
		
		sq					SCREEN0 , 2(\DEST_PTR)
		sq					SCREEN1 , GIF_STEP + 2(\DEST_PTR)
		
		FCOR				CLIPPING_RESULT , ~((CLIP_ZP << (6*3)) | (CLIP_ZP << (6*2))  | (CLIP_ZP << (6*1))  | (CLIP_ZP << (6*0)))
		iaddi				DrawKickADC_FOG , CLIPPING_RESULT , -1
		
		isw.w				DrawKickADC_FOG , 2(\DEST_PTR)
		isw.w				DrawKickADC_FOG , GIF_STEP + 2(\DEST_PTR)

.endm
/*
GIF_BASE0-1				(GIF_STEP = 3)
	GIF_TAG
	STQ			0	=>	S	T	Q
	RGBAQ		0	=> 	R	G	B	A
	XYZF(2-3)	0	=>	X	y	z	F(2-3)
	STQ			1
	RGBAQ		1
	XYZF(2-3)	1
	STQ			2
	RGBAQ		2
	XYZF(2-3)	2
	...
	STQ			OutputPackSize - 1
	RGBAQ		OutputPackSize - 1
	XYZF(2-3)	OutputPackSize - 1
	
============================================================================================
============================================================================================
============================================================================================*/


.macro	TRANSFORM_POINT 	PSRC , PDST
		mulax				acc,		I_CAMERA, \PSRC.x
		madday				acc,		J_CAMERA, \PSRC.y
		maddaz				acc,		K_CAMERA, \PSRC.z
		maddw				\PDST,		S_CAMERA, VF00w
.endm

.macro	TURN_POINT 			PSRC , PDST
		mulax				acc,		I_CAMERA, \PSRC.x
		madday				acc,		J_CAMERA, \PSRC.y
		maddz				\PDST,		K_CAMERA, \PSRC.z
.endm

.macro	PROJECT_POINT 		PSRC , PDST , UVQ , UVQTEMP
		maxw.z				\PSRC,	\PSRC,	K_CAMERAw
		DIV 				Q,VF00w , \PSRC.z
		MULq.xyw			\PDST,\PSRC,Q
		mulq.zw				\PDST,COSAS_SINAS_One_FOGMul,Q
		
		mulq.xyz			\UVQTEMP , \UVQ , Q
		ADD.xyzw			\PDST,\PDST,ADD_SCREEN
		MAX.xyw				\PDST,\PDST,MAX_SCREEN
		MINI.xyw			\PDST,\PDST,MIN_SCREEN
		FTOI4.xyzw			\PDST,\PDST

.endm


.macro	PROJECT_POINT_SPRITE	PSRC , PDST 
		maxw.z				\PSRC,	\PSRC,	K_CAMERAw
		DIV 				Q,VF00w , \PSRC.z

		MULq.xy				\PDST,\PSRC,Q
		mulq.zw				\PDST,COSAS_SINAS_One_FOGMul,Q	
		ADD.xyzw			\PDST,\PDST,ADD_SCREEN
		MAX.xyw				\PDST,\PDST,MAX_SCREEN
		MINI.xyw			\PDST,\PDST,MIN_SCREEN
		FTOI4.xyzw			\PDST,\PDST
.endm
.macro	PROJECT_POINT_SPRITE_REUSE_Q	PSRC , PDST 
		MULq.xy				\PDST,\PSRC,Q
		addq.zw				\PDST,VF00,Q	
		ADD.xyzw			\PDST,\PDST,ADD_SCREEN
		MAX.xyw				\PDST,\PDST,MAX_SCREEN
		MINI.xyw			\PDST,\PDST,MIN_SCREEN
		FTOI4.xyzw			\PDST,\PDST
.endm

.macro	COMPUTE_AND_STORE_2_POINTS
/* Compute trapèze ******/
		sub								Trapeze , Point0 , Point1 		
		mulz							Trapeze , Trapeze , Parameter	
		add								PointLocal0 , Point0 , Trapeze	
		sub								PointLocal1 , Point1 , Trapeze

		CLIPw							PointLocal0 , CLIPPING_TEST
		CLIPw							PointLocal1 , CLIPPING_TEST
		
/* Project & Store XYZW */
		PROJECT_POINT					PointLocal0 , SCREEN0 , PointUV0 ,TempUV0
		PROJECT_POINT					PointLocal1 , SCREEN1 , PointUV1 ,TempUV1
		
		sq.w							SCREEN0 , FOG_KIGK_BASE + 1(VI00)

/* Store all ************/
		SAVE_A_LEAF						DST_Ptr
/* Compute next point ***/
		sub.w							OneMinusP , VF00 , Parameter
		mulaw							acc, 	Gravity_ , Parameter
		maddw							Derivative , ZA , OneMinusP
		add								Point0 , Point0 , Derivative;Derivative
		add								Point1 , Point1 , Derivative;Derivative
		add.zw							Parameter , Parameter , ParameterInc
		mini.w							Parameter , Parameter , VF00
		add.y							PointUV0 , PointUV0 , UVIncr
		add.y							PointUV1 , PointUV1 , UVIncr
		iaddiu							DST_Ptr , DST_Ptr , GIF_STEP * 2
		
.endm

.macro	COMPUTE_AND_STORE_A_SPRITE
/* Compute trapèze ******/
		IADDIU							DST_Ptr 	, GIF_BASE 		, 1
		
		
		add								Point1 , Point0 , XA
		add								Point1 , Point1 , YA
		sub.xyz							PointA , Point1 , XCAM
		sub.xyz							PointA , PointA , YCAM
		add.xyz							PointB , Point1 , XCAM
		sub.xyz							PointB , PointB , YCAM
		sub.xyz							PointC , Point1 , XCAM
		add.xyz							PointC , PointC , YCAM
		add.xyz							PointD , Point1 , XCAM
		add.xyz							PointD , PointD , YCAM
		
		CLIPw							Point1 , CLIPPING_TEST
		
; Helicoïdal effect around branch 
		mulax.xyzw						acc , XA , CosAlpha_SinAlpha
		maddy.xyzw						XATemp , YA , CosAlpha_SinAlpha
		mulax.xyzw						acc , YA , CosAlpha_SinAlpha
		msuby.xyzw						YA , XA , CosAlpha_SinAlpha
		addx.xyz						XA , XATemp , VF00
		
; Turn XCam
		mulax.xyzw						acc , XCAM , COSAS_SINAS_One_FOGMul
		maddy.xyzw						XATemp , YCAM , COSAS_SINAS_One_FOGMul
		mulax.xyzw						acc , YCAM , COSAS_SINAS_One_FOGMul
		msuby.xyzw						YCAM , XCAM , COSAS_SINAS_One_FOGMul
		addx.xyz						XCAM , XATemp , VF00

/* Project & Store XYZW */

		PROJECT_POINT_SPRITE			PointA , SCREEN0 
		PROJECT_POINT_SPRITE_REUSE_Q	PointB , SCREEN1
		PROJECT_POINT_SPRITE_REUSE_Q	PointC , SCREEN2
		PROJECT_POINT_SPRITE_REUSE_Q	PointD , SCREEN3
		
/* Store all ************/
		sq								PColor , GIF_STEP*0 + 1(DST_Ptr)
		sq								PColor , GIF_STEP*1 + 1(DST_Ptr)
		sq								PColor , GIF_STEP*2 + 1(DST_Ptr)
		sq								PColor , GIF_STEP*3 + 1(DST_Ptr)

		sq.xyz							SCREEN0 , GIF_STEP*0 + 2(DST_Ptr)
		sq.xyz							SCREEN1 , GIF_STEP*1 + 2(DST_Ptr)
		sq.xyz							SCREEN2 , GIF_STEP*2 + 2(DST_Ptr)
		sq.xyz							SCREEN3 , GIF_STEP*3 + 2(DST_Ptr)
		
		sq.w							SCREEN0 , FOG_KIGK_BASE + 1(VI00)
		
		FCAND							CLIPPING_RESULT , (CLIP_ZP << (6*0))
		iaddi							DrawKickADC_FOG , CLIPPING_RESULT , -1
		
		isw.w							VI00			, GIF_STEP*0 + 2(DST_Ptr)
		isw.w							VI00 			, GIF_STEP*1 + 2(DST_Ptr)
		isw.w							DrawKickADC_FOG , GIF_STEP*2 + 2(DST_Ptr)
		isw.w							DrawKickADC_FOG , GIF_STEP*3 + 2(DST_Ptr)
		
		KICK_PRIM
		
		
		
/* Compute next point ***/
		sub.w							OneMinusP , VF00 , Parameter
		mulaw							acc, 	Gravity_ , Parameter
		maddw							Derivative , ZA , OneMinusP
		add								Point0 , Point0 , Derivative
		add.zw							Parameter , Parameter , ParameterInc
		mini.w							Parameter , Parameter , VF00
		iaddiu							DST_Ptr , DST_Ptr , GIF_STEP * 2
		
.endm

--enter		
.init_vf_all  
.init_vi_all
--endenter	

/*****************************************************************************************************
	CODE START
******************************************************************************************************/

Jungle_First_init:
		iaddiu							VIF_BASE 	 , VI00 , 0
		iaddiu							GIF_BASE 	 , VI00 , GIF_BASE0
		
; force Alpha = 255		
		iaddiu							TEMP4 	 , VI00 , 128
		mfir.w							PColor , TEMP4						
		
		SUB								CLIPPING_TEST , VF00 , VF00
		
		
--cont
		LOAD_HEADER						SRC_p_Ptr
		
/* For the pack */
Jungle_init:
--cont
		iaddiu 							VIF_BASE , VIF_BASE , 0
		LOAD_PARAMS						SRC_p_Ptr , SRC_c_Ptr

		IADDIU							PrimitiveCounter , VI00 , 16384
		iadd							PrimitiveCounter , PrimitiveCounter , PrimitiveCounter ; 32048
		iadd							PrimitiveCounter , PrimitiveCounter , SegmentNumber
		iadd							PrimitiveCounter , PrimitiveCounter , SegmentNumber
		iaddiu							PrimitiveCounter , PrimitiveCounter , 2

		mfir.x							GIF_TAG , PrimitiveCounter
		sq								GIF_TAG , GIF_BASE0(VI00)	; Yes, I Write in a place witch is xkicked
		sq								GIF_TAG , GIF_BASE1(VI00)	; But this is the first place to be read, is readed very fast.
																	; so no risks of asyncronous bug.
		
		LOAD_NUMBER_Of_Branches			NumberOfBranches	, VIF_BASE
		
		ibeq							NumberOfBranches , VI00 , Jungle_End

		ibeq							CurrentMode , VI00 , DrawHat
		isubiu							CurrentMode , CurrentMode , 1
		ibeq							CurrentMode , VI00 , DrawX
		isubiu							CurrentMode , CurrentMode , 1
		ibeq							CurrentMode , VI00 , DrawY
		isubiu							CurrentMode , CurrentMode , 1
		ibeq							CurrentMode , VI00 , DrawSrpites_Start
		B								Jungle_End

/*****************************************************************************************************************/
/* DRAW Srpites **************************************************************************************************/
/*****************************************************************************************************************/
DrawSrpites_Start:

		

		IADDIU							PrimitiveCounter , VI00 , 16384
		iadd							PrimitiveCounter , PrimitiveCounter , PrimitiveCounter ; 32048
		iaddiu							PrimitiveCounter , PrimitiveCounter , 4
		mfir.x							GIF_TAG , PrimitiveCounter
		sq								GIF_TAG , GIF_BASE0(VI00)	; Yes, I Write in a place witch is xkicked
		sq								GIF_TAG , GIF_BASE1(VI00)	; But this is the first place to be read, is readed very fast.

;		set ADC begin

		sub.xyzw						CosAlpha_SinAlpha , VF00 , VF00
		addw.xyzw						CosAlpha_SinAlpha , CosAlpha_SinAlpha , VF00

		iaddi							GIF_BASE , GIF_BASE , 1
		
		sq.xyzw							VF00			 , 0(GIF_BASE)
		sq.xyzw							VF00			 , 3(GIF_BASE)
		sq.xyzw							VF00			 , 6(GIF_BASE)
		sq.xyzw							VF00			 , 9(GIF_BASE)
		sq.z							CosAlpha_SinAlpha, 0(GIF_BASE)
		sq.xz							CosAlpha_SinAlpha, 3(GIF_BASE)
		sq.yz							CosAlpha_SinAlpha, 6(GIF_BASE)
		sq.xyz							CosAlpha_SinAlpha, 9(GIF_BASE)
		
		SWITCH_BASE						GIF_BASE

		sq.xyzw							VF00			 , 0(GIF_BASE)
		sq.xyzw							VF00			 , 3(GIF_BASE)
		sq.xyzw							VF00			 , 6(GIF_BASE)
		sq.xyzw							VF00			 , 9(GIF_BASE)
		sq.z							CosAlpha_SinAlpha, 0(GIF_BASE)
		sq.xz							CosAlpha_SinAlpha, 3(GIF_BASE)
		sq.yz							CosAlpha_SinAlpha, 6(GIF_BASE)
		sq.xyz							CosAlpha_SinAlpha, 9(GIF_BASE)
		
		SWITCH_BASE						GIF_BASE

		iaddi							GIF_BASE , GIF_BASE , -1
		
		loi								-0.34202014332566873304409961468226
		addi.x							CosAlpha_SinAlpha , VF00 , I
		loi								0.9396926207859083840541092773247
		addi.y							CosAlpha_SinAlpha , VF00 , I
		
		
;		set ADC end

		sub.xyzw						XCAM , VF00 , VF00
		sub.xyzw						YCAM , VF00 , VF00
		
		addw.x							XCAM , VF00 , VF00
		addw.y							YCAM , VF00 , VF00
		
		mulz							XCAM , XCAM , SprteGeneratorRadius_OoSpriteNumber_SpriteSize_SqrEoHpO2
		mulz							YCAM , YCAM , SprteGeneratorRadius_OoSpriteNumber_SpriteSize_SqrEoHpO2
		


DrawSrpites:
--LoopCS	1,1
		iaddiu							SpriteCounter , VI00 , 0
		LOAD_A_LEAF						SRC_p_Ptr, SRC_c_Ptr , LOAD_A_LEAF_S
		

DrawSrpites_Loop:
--LoopCS	1,1
		COMPUTE_AND_STORE_A_SPRITE
		iaddiu							SpriteCounter , SpriteCounter , 1
		IBNE							SpriteCounter , SpriteNumber , DrawSrpites_Loop
		iaddi							NumberOfBranches , NumberOfBranches , -1
		ibne							NumberOfBranches , VI00 , DrawSrpites
		B								Jungle_End


/*****************************************************************************************************************/
/* DRAW HAT ******************************************************************************************************/
/*****************************************************************************************************************/
DrawHat:
		IADDIU							PrimitiveCounter , VI00 , 16384
		iadd							PrimitiveCounter , PrimitiveCounter , PrimitiveCounter ; 32048
		iaddiu							PrimitiveCounter , PrimitiveCounter , 4
		
;		add.y							UVIncr , VF00 , SU_SV_void_void
		
		mfir.x							GIF_TAG , PrimitiveCounter
		sq								GIF_TAG , GIF_BASE0(VI00)	; Yes, I Write in a place witch is xkicked
		sq								GIF_TAG , GIF_BASE1(VI00)	; But this is the first place to be read, is readed very fast.
DrawHatLoop:
;--LoopCS	1,1
		LOAD_A_LEAF						SRC_p_Ptr, SRC_c_Ptr , LOAD_A_LEAF_H
		COMPUTE_AND_STORE_2_POINTS
		COMPUTE_AND_STORE_2_POINTS
		KICK_PRIM
		iaddi							NumberOfBranches , NumberOfBranches , -1
		IBNE							NumberOfBranches , VI00 , DrawHatLoop
		B								Jungle_End

/*****************************************************************************************************************/
/* DRAW X ******************************************************************************************************/
/*****************************************************************************************************************/
DrawX:
		isubiu							TEMPVar , SegmentNumber , 1
		ibeq							TEMPVar , VI00 , JungleX_KickPack_1_Segment
JungleX_KickPack:
		iaddiu							SegmentCounter , VI00 , 0
		LOAD_A_LEAF						SRC_p_Ptr, SRC_c_Ptr , LOAD_A_LEAF_X
		COMPUTE_AND_STORE_2_POINTS
JungleX_Loop_Segments:													; for each segment of the branch 
--LoopCS	1,1
		COMPUTE_AND_STORE_2_POINTS
		iaddiu							SegmentCounter , SegmentCounter , 1
		IBNE							SegmentCounter , SegmentNumber , JungleX_Loop_Segments
		KICK_PRIM
		iaddi							NumberOfBranches , NumberOfBranches , -1
		ibne							NumberOfBranches , VI00 , JungleX_KickPack
		B								Jungle_End
JungleX_KickPack_1_Segment:
;--LoopCS	1,1
		LOAD_A_LEAF						SRC_p_Ptr, SRC_c_Ptr , LOAD_A_LEAF_X
		COMPUTE_AND_STORE_2_POINTS
		COMPUTE_AND_STORE_2_POINTS
		KICK_PRIM
		iaddi							NumberOfBranches , NumberOfBranches , -1
		IBNE							NumberOfBranches , VI00 , JungleX_KickPack_1_Segment
		B								Jungle_End

/*****************************************************************************************************************/
/* DRAW Y ******************************************************************************************************/
/*****************************************************************************************************************/
DrawY:
		isubiu							TEMPVar , SegmentNumber , 1
		ibeq							TEMPVar , VI00 , JungleY_KickPack_1_Segment
JungleY_KickPack:
		iaddiu							SegmentCounter , VI00 , 0
		LOAD_A_LEAF						SRC_p_Ptr, SRC_c_Ptr , LOAD_A_LEAF_Y
		COMPUTE_AND_STORE_2_POINTS
JungleY_Loop_Segments:													; for each segment of the branch 
--LoopCS	1,1
		COMPUTE_AND_STORE_2_POINTS
		iaddiu							SegmentCounter , SegmentCounter , 1
		IBNE							SegmentCounter , SegmentNumber , JungleY_Loop_Segments
		KICK_PRIM
		iaddi							NumberOfBranches , NumberOfBranches , -1
		ibne							NumberOfBranches , VI00 , JungleY_KickPack
		B								Jungle_End
JungleY_KickPack_1_Segment:
;--LoopCS	1,1
		LOAD_A_LEAF						SRC_p_Ptr, SRC_c_Ptr , LOAD_A_LEAF_Y
		COMPUTE_AND_STORE_2_POINTS
		COMPUTE_AND_STORE_2_POINTS
		KICK_PRIM
		iaddi							NumberOfBranches , NumberOfBranches , -1
		IBNE							NumberOfBranches , VI00 , JungleY_KickPack_1_Segment
		B								Jungle_End

/*****************************************************************************************************************/
/* DRAW END ******************************************************************************************************/
/*****************************************************************************************************************/
Jungle_End:
		SWITCH_BASE						VIF_BASE
		B								Jungle_init
		

--exit 
--endexit
