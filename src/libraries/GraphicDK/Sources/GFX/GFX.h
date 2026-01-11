/*$T GFX.h GC! 1.081 10/17/00 09:37:01 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFX_H__
#define __GFX_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

//#define _DEBUGGFXGEOM
#ifdef _DEBUGGFXGEOM
void GFX_CheckGeom();
#define M_GFX_CheckGeom() GFX_CheckGeom()
#else
#define M_GFX_CheckGeom()
#endif

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */
struct GFX_tdst_List_;
extern struct GFX_tdst_List_	    *GFX_gpst_Current;
struct OBJ_tdst_GameObject_;
extern struct OBJ_tdst_GameObject_	*GFX_gpst_GO;
extern struct GEO_tdst_Object_		*GFX_gpst_Geo;

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

/* special for int : flag for 1 or 2 Bytes values */
#define GFX_OneByteInteger  0x1000
#define GFX_TwoByteInteger  0x2000

#define GFX_Special         0x4000

/* type */
#define GFX_Unknown			0
#define GFX_Line			1
#define GFX_Lightning		2
#define GFX_Desintegrate	3
#define GFX_Flare			4
#define GFX_ComplexFlare	5
#define GFX_Smoke			6
#define GFX_Table           7
#define GFX_Speed			8
#define GFX_Spark           9
#define GFX_Water2			10
#define GFX_Fade            11
#define GFX_Profiler		12
#define GFX_Explode			13
#define GFX_BorderFade      14
#define GFX_Diaphragm       15
#define GFX_Carte           16
#define GFX_Gao             17
#define GFX_Sun             18
#define GFX_String          19
#define GFX_Ripple          20
#define GFX_AnotherFlare    21
#define GFX_Tetris          22

#define GFX_Number			23

/* flags */
#define GFX_Active			            0x01    /* activation du GFX */
#define GFX_NotSorted                   0x02    /* ne pas trier le GFX */
#define GFX_Transparent		            0x04    /* Le matériau est transparent */
#define GFX_AutoDestruct	            0x08    /* auto destruction */
#define GFX_DoNotDestroyWithOwner       0x10    /* ne pas détruire le GFX si le owner est détruit */
#define GFX_Death						0x20    /* le GFX est en train de mourrir */
#define GFX_Sorted                      0x40    /* indicate a sorted list of GFX, Id is order */
#define GFX_Terminated		            0x80    /* le GFX est terminé il va être détruit bientôt */

/* Id */
#define GFX_FirstNotSorted              1024    /* first index of unsorted effect */

/* params */
#define GFX_LineParamsf_First			1000
#define GFX_LineParamsf_Number			1
#define GFX_LineParamsi_First			1100
#define GFX_LineParamsi_Number			1
#define GFX_LineParamsv_First			1200
#define GFX_LineParamsv_Number			3

#define GFX_LightningParamsf_First		2000
#define GFX_LightningParamsf_Number		6
#define GFX_LightningParamsi_First		2100
#define GFX_LightningParamsi_Number		3
#define GFX_LightningParamsv_First		2200
#define GFX_LightningParamsv_Number		3

#define GFX_DesintegrateParamsf_First	3000
#define GFX_DesintegrateParamsf_Number	7
#define GFX_DesintegrateParamsi_First	3100
#define GFX_DesintegrateParamsi_Number	0
#define GFX_DesintegrateParamsv_First	3200
#define GFX_DesintegrateParamsv_Number	1

#define GFX_FlareParamsf_First			4000
#define GFX_FlareParamsf_Number			3
#define GFX_FlareParamsi_First			4100
#define GFX_FlareParamsi_Number			4
#define GFX_FlareParamsv_First			4200
#define GFX_FlareParamsv_Number			2

#define GFX_ComplexFlareParamsf_First	4000
#define GFX_ComplexFlareParamsf_Number	21
#define GFX_ComplexFlareParamsi_First	4100
#define GFX_ComplexFlareParamsi_Number	20
#define GFX_ComplexFlareParamsv_First	4200
#define GFX_ComplexFlareParamsv_Number	2

#define GFX_SmokeParamsf_First			6000
#define GFX_SmokeParamsf_Number			4
#define GFX_SmokeParamsi_First			6100
#define GFX_SmokeParamsi_Number			4
#define GFX_SmokeParamsv_First			6200
#define GFX_SmokeParamsv_Number			3

#define GFX_TableParamsf_First			7000
#define GFX_TableParamsf_Number			0
#define GFX_TableParamsi_First			7100
#define GFX_TableParamsi_Number			13
#define GFX_TableParamsv_First			7200
#define GFX_TableParamsv_Number			4

#define GFX_SpeedParamsf_First			8000
#define GFX_SpeedParamsf_Number			5
#define GFX_SpeedParamsi_First			8100
#define GFX_SpeedParamsi_Number			1
#define GFX_SpeedParamsv_First			8200
#define GFX_SpeedParamsv_Number			1

#define GFX_SparkParamsf_First			9000
#define GFX_SparkParamsf_Number			12
#define GFX_SparkParamsi_First			9100
#define GFX_SparkParamsi_Number			4
#define GFX_SparkParamsv_First			9200
#define GFX_SparkParamsv_Number			3

#define GFX_Water2Paramsf_First			10000
#define GFX_Water2Paramsf_Number		4
#define GFX_Water2Paramsi_First			10100
#define GFX_Water2Paramsi_Number		4
#define GFX_Water2Paramsv_First			10200
#define GFX_Water2Paramsv_Number		2

#define GFX_FadeParamsf_First			11000
#define GFX_FadeParamsf_Number		    6
#define GFX_FadeParamsi_First			11100
#define GFX_FadeParamsi_Number		    2
#define GFX_FadeParamsv_First			11200
#define GFX_FadeParamsv_Number		    0

#define GFX_ProfilerParamsf_First		12000
#define GFX_ProfilerParamsi_First		12100
#define GFX_ProfilerParamsv_First		12200

#define GFX_ExplodeParamsf_First		13000
#define GFX_ExplodeParamsi_First		13100
#define GFX_ExplodeParamsv_First		13200

#define GFX_BorderFadeParamsf_First		14000
#define GFX_BorderFadeParamsf_Number	   12
#define GFX_BorderFadeParamsi_First		14100
#define GFX_BorderFadeParamsi_Number	    3
#define GFX_BorderFadeParamsv_First		14200
#define GFX_BorderFadeParamsv_Number	    0

#define GFX_DiaphragmParamsf_First		15000
#define GFX_DiaphragmParamsf_Number	    4
#define GFX_DiaphragmParamsi_First		15100
#define GFX_DiaphragmParamsi_Number	    1
#define GFX_DiaphragmParamsv_First		15200
#define GFX_DiaphragmParamsv_Number	    1

#define GFX_CarteParamsf_First		    16000
#define GFX_CarteParamsf_Number	        4
#define GFX_CarteParamsi_First		    16100
#define GFX_CarteParamsi_Number	        7
#define GFX_CarteParamsv_First		    16200
#define GFX_CarteParamsv_Number	        7

#define GFX_GaoParamsf_First		    17000
#define GFX_GaoParamsf_Number	        1
#define GFX_GaoParamsi_First		    17100
#define GFX_GaoParamsi_Number	        0
#define GFX_GaoParamsv_First		    17200
#define GFX_GaoParamsv_Number	        0

#define GFX_SunParamsf_First		    18000
#define GFX_SunParamsf_Number	        14
#define GFX_SunParamsi_First		    18100
#define GFX_SunParamsi_Number	        5
#define GFX_SunParamsv_First		    18200
#define GFX_SunParamsv_Number	        1

#define GFX_StrParamsf_First		    19000
#define GFX_StrParamsf_Number	        0
#define GFX_StrParamsi_First		    19100
#define GFX_StrParamsi_Number	        3
#define GFX_StrParamsv_First		    19200
#define GFX_StrParamsv_Number	        0

#define GFX_RippleParamsf_First		    20000
#define GFX_RippleParamsf_Number	    5
#define GFX_RippleParamsi_First		    20100
#define GFX_RippleParamsi_Number	    3
#define GFX_RippleParamsv_First		    20200
#define GFX_RippleParamsv_Number	    3

#define GFX_AnotherFlareParamsf_First	21000
#define GFX_AnotherFlareParamsf_Number	7
#define GFX_AnotherFlareParamsi_First	21100
#define GFX_AnotherFlareParamsi_Number	5
#define GFX_AnotherFlareParamsv_First	21200
#define GFX_AnotherFlareParamsv_Number	2

#define GFX_TetrisParamsf_First			22000
#define GFX_TetrisParamsf_Number			2
#define GFX_TetrisParamsi_First			22100
#define GFX_TetrisParamsi_Number			5
#define GFX_TetrisParamsv_First			22200
#define GFX_TetrisParamsv_Number			3


#define GFX_Paramsf_MaxNumber			21
#define GFX_Paramsi_MaxNumber			20
#define GFX_Paramsv_MaxNumber			20


/* Line parameters */   
#define GFX_LineStart                   GFX_LineParamsv_First + 0               
#define GFX_LineEnd                     GFX_LineParamsv_First + 1
#define GFX_LineBanking                 GFX_LineParamsv_First + 2

#define GFX_LineColor                   GFX_LineParamsi_First + 0

#define GFX_LineSize                    GFX_LineParamsf_First + 0

/* lighning parameters */
#define GFX_LightningStart              GFX_LightningParamsv_First + 0               
#define GFX_LightningEnd                GFX_LightningParamsv_First + 1
#define GFX_LightningBanking            GFX_LightningParamsv_First + 2

#define GFX_LightningSize               GFX_LightningParamsf_First + 0
#define GFX_LightningRadius             GFX_LightningParamsf_First + 1
#define GFX_LightningTimeCur            GFX_LightningParamsf_First + 2
#define GFX_LightningTimeBirth          GFX_LightningParamsf_First + 3
#define GFX_LightningTimeLife           GFX_LightningParamsf_First + 4
#define GFX_LightningTimeDeath          GFX_LightningParamsf_First + 5

#define GFX_LightningSampleNumber       GFX_LightningParamsi_First + 0
#define GFX_LightningFlags              GFX_LightningParamsi_First + 1
#define GFX_LightningColor              GFX_LightningParamsi_First + 2

/* desintegrate parameters */
#define GFX_DesintegrateGrav			GFX_DesintegrateParamsv_First + 0

#define GFX_DesintegrateTimeMin			GFX_DesintegrateParamsf_First + 0
#define GFX_DesintegrateTimeMax			GFX_DesintegrateParamsf_First + 1
#define GFX_DesintegrateMulMin			GFX_DesintegrateParamsf_First + 2
#define GFX_DesintegrateMulMax			GFX_DesintegrateParamsf_First + 3
#define GFX_DesintegrateFriction		GFX_DesintegrateParamsf_First + 4
#define GFX_DesintegrateZMin			GFX_DesintegrateParamsf_First + 5
#define GFX_DesintegrateZMinStrength	GFX_DesintegrateParamsf_First + 6

/* flare and complex flare params */
#define GFX_FlarePos		            GFX_FlareParamsv_First + 0
#define GFX_FlareOffset		            GFX_FlareParamsv_First + 1

#define GFX_FlareSize		            GFX_FlareParamsf_First + 0
#define GFX_FlareHiddenTime             GFX_FlareParamsf_First + 1
#define GFX_FlareInterval               GFX_FlareParamsf_First + 2
#define GFX_FlarePos0		            GFX_FlareParamsf_First + 3
#define GFX_FlarePos1		            GFX_FlareParamsf_First + 4
#define GFX_FlarePos2		            GFX_FlareParamsf_First + 5
#define GFX_FlarePos3		            GFX_FlareParamsf_First + 6
#define GFX_FlarePos4		            GFX_FlareParamsf_First + 7
#define GFX_FlarePos5	    	        GFX_FlareParamsf_First + 8
#define GFX_FlarePos6		            GFX_FlareParamsf_First + 9
#define GFX_FlarePos7		            GFX_FlareParamsf_First + 10
#define GFX_FlarePos8		            GFX_FlareParamsf_First + 11
#define GFX_FlareScale0		            GFX_FlareParamsf_First + 12
#define GFX_FlareScale1		            GFX_FlareParamsf_First + 13
#define GFX_FlareScale2		            GFX_FlareParamsf_First + 14
#define GFX_FlareScale3		            GFX_FlareParamsf_First + 15
#define GFX_FlareScale4		            GFX_FlareParamsf_First + 16
#define GFX_FlareScale5		            GFX_FlareParamsf_First + 17
#define GFX_FlareScale6		            GFX_FlareParamsf_First + 18
#define GFX_FlareScale7		            GFX_FlareParamsf_First + 19
#define GFX_FlareScale8		            GFX_FlareParamsf_First + 20

#define GFX_FlareFlags		            GFX_FlareParamsi_First + 0
#define GFX_FlareId			            GFX_FlareParamsi_First + 1
#define GFX_FlareColor		            GFX_FlareParamsi_First + 2
#define GFX_FlareDeathColor		        GFX_FlareParamsi_First + 3

#define GFX_FlareNumber		            GFX_FlareParamsi_First + 1
#define GFX_FlareId0		            GFX_FlareParamsi_First + 2
#define GFX_FlareId1		            GFX_FlareParamsi_First + 3
#define GFX_FlareId2		            GFX_FlareParamsi_First + 4
#define GFX_FlareId3		            GFX_FlareParamsi_First + 5
#define GFX_FlareId4		            GFX_FlareParamsi_First + 6
#define GFX_FlareId5		            GFX_FlareParamsi_First + 7
#define GFX_FlareId6		            GFX_FlareParamsi_First + 8
#define GFX_FlareId7		            GFX_FlareParamsi_First + 9
#define GFX_FlareId8		            GFX_FlareParamsi_First + 10
#define GFX_FlareColor0		            GFX_FlareParamsi_First + 11
#define GFX_FlareColor1		            GFX_FlareParamsi_First + 12
#define GFX_FlareColor2		            GFX_FlareParamsi_First + 13
#define GFX_FlareColor3		            GFX_FlareParamsi_First + 14
#define GFX_FlareColor4		            GFX_FlareParamsi_First + 15
#define GFX_FlareColor5		            GFX_FlareParamsi_First + 16
#define GFX_FlareColor6		            GFX_FlareParamsi_First + 17
#define GFX_FlareColor7		            GFX_FlareParamsi_First + 18
#define GFX_FlareColor8		            GFX_FlareParamsi_First + 19

/* smoke params */
#define GFX_SmokePos		            GFX_SmokeParamsv_First + 0
#define GFX_SmokeSpeed		            GFX_SmokeParamsv_First + 1

#define GFX_SmokeTime		            GFX_SmokeParamsf_First + 0
#define GFX_SmokeFriction               GFX_SmokeParamsf_First + 1
#define GFX_SmokeStartSize	            GFX_SmokeParamsf_First + 2
#define GFX_SmokeEndSize	            GFX_SmokeParamsf_First + 3

#define GFX_SmokeSetNumber              GFX_SmokeParamsi_First + 0
#define GFX_SmokeStartColor		        GFX_SmokeParamsi_First + 1
#define GFX_SmokeEndColor               GFX_SmokeParamsi_First + 2
#define GFX_SmokeAddOne                 GFX_SmokeParamsi_First + 3

/* Water params */
#define GFX_TablePos		            GFX_TableParamsv_First + 0
#define GFX_TableRowSize                GFX_TableParamsv_First + 1
#define GFX_TableColSize                GFX_TableParamsv_First + 2
#define GFX_TableCaseContent            GFX_TableParamsv_First + 3

#define GFX_TableFlags                  GFX_TableParamsi_First + 0
#define GFX_TableNbRow                  GFX_TableParamsi_First + 1
#define GFX_TableNbCol                  GFX_TableParamsi_First + 2
#define GFX_TableColor0                 GFX_TableParamsi_First + 3
#define GFX_TableColor1                 GFX_TableParamsi_First + 4
#define GFX_TableColor2                 GFX_TableParamsi_First + 5
#define GFX_TableColor3                 GFX_TableParamsi_First + 6
#define GFX_TableColor4                 GFX_TableParamsi_First + 7
#define GFX_TableColor5                 GFX_TableParamsi_First + 8
#define GFX_TableColor6                 GFX_TableParamsi_First + 9
#define GFX_TableColor7                 GFX_TableParamsi_First + 10
#define GFX_TableColor8                 GFX_TableParamsi_First + 11
#define GFX_TableColor9                 GFX_TableParamsi_First + 12

/* speed params */
#define GFX_SpeedPos	                GFX_SpeedParamsv_First + 0

#define GFX_SpeedPosX	                GFX_SpeedParamsf_First + 0
#define GFX_SpeedPosY	                GFX_SpeedParamsf_First + 1
#define GFX_SpeedPosZ	                GFX_SpeedParamsf_First + 2
#define GFX_SpeedTime	                GFX_SpeedParamsf_First + 3
#define GFX_SpeedSize	                GFX_SpeedParamsf_First + 4

#define GFX_SpeedColor	                GFX_SpeedParamsi_First + 0

/* speed params */
#define GFX_SparkPos	                GFX_SparkParamsv_First + 0
#define GFX_SparkNorm                   GFX_SparkParamsv_First + 1
#define GFX_SparkGrav	                GFX_SparkParamsv_First + 2

#define GFX_SparkAngle                  GFX_SparkParamsf_First + 0
#define GFX_SparkSpeedMin               GFX_SparkParamsf_First + 1
#define GFX_SparkSpeedMax               GFX_SparkParamsf_First + 2
#define GFX_SparkFriction               GFX_SparkParamsf_First + 3
#define GFX_SparkSize                   GFX_SparkParamsf_First + 4
#define GFX_SparkTimeMin                GFX_SparkParamsf_First + 5
#define GFX_SparkTimeMax                GFX_SparkParamsf_First + 6
#define GFX_SparkScale                  GFX_SparkParamsf_First + 7
#define GFX_SparkPeriod                 GFX_SparkParamsf_First + 8
#define GFX_SparkDeathMin               GFX_SparkParamsf_First + 9
#define GFX_SparkDeathMax               GFX_SparkParamsf_First + 10


#define GFX_SparkNumber                 GFX_SparkParamsi_First + 0
#define GFX_SparkColor	                GFX_SparkParamsi_First + 1
#define GFX_SparkFlags                  GFX_SparkParamsi_First + 2

/* Water2 params */
#define GFX_Water2Pos		            GFX_Water2Paramsv_First + 0
#define GFX_Water2Normal	            GFX_Water2Paramsv_First + 1

#define GFX_Water2Time		            GFX_Water2Paramsf_First + 0
#define GFX_Water2Size		            GFX_Water2Paramsf_First + 1
#define GFX_Water2EndSize	            GFX_Water2Paramsf_First + 2
#define GFX_Water2YoX		            GFX_Water2Paramsf_First + 3

#define GFX_Water2NbMax		            GFX_Water2Paramsi_First + 0
#define GFX_Water2Color		            GFX_Water2Paramsi_First + 1
#define GFX_Water2Add                   GFX_Water2Paramsi_First + 2
#define GFX_Water2LocalColor            GFX_Water2Paramsi_First + 3

/* Fade params */
#define GFX_FadeTimeC0Start             GFX_FadeParamsf_First + 0
#define GFX_FadeTimeC0C1                GFX_FadeParamsf_First + 1
#define GFX_FadeTimeC1                  GFX_FadeParamsf_First + 2
#define GFX_FadeTimeC1C0                GFX_FadeParamsf_First + 3
#define GFX_FadeTimeC0End               GFX_FadeParamsf_First + 4
#define GFX_FadeDepth                   GFX_FadeParamsf_First + 5

#define GFX_FadeColorStart              GFX_FadeParamsi_First + 0
#define GFX_FadeColorEnd	            GFX_FadeParamsi_First + 1

/* Border Fade params */
#define GFX_BorderFadeX1                GFX_BorderFadeParamsf_First + 0
#define GFX_BorderFadeY1                GFX_BorderFadeParamsf_First + 1
#define GFX_BorderFadeX2                GFX_BorderFadeParamsf_First + 2
#define GFX_BorderFadeY2                GFX_BorderFadeParamsf_First + 3
#define GFX_BorderFadeTimeWait          GFX_BorderFadeParamsf_First + 4
#define GFX_BorderFadeTimeIn            GFX_BorderFadeParamsf_First + 5
#define GFX_BorderFadeTimeOut           GFX_BorderFadeParamsf_First + 6
#define GFX_BorderFadeLineDx            GFX_BorderFadeParamsf_First + 7
#define GFX_BorderFadeLineDy            GFX_BorderFadeParamsf_First + 8
#define GFX_BorderFadeLineSx            GFX_BorderFadeParamsf_First + 9
#define GFX_BorderFadeLineSy            GFX_BorderFadeParamsf_First + 10
#define GFX_BorderFadeLineW             GFX_BorderFadeParamsf_First + 11

#define GFX_BorderFadeFlags             GFX_BorderFadeParamsi_First + 0
#define GFX_BorderFadeColor	            GFX_BorderFadeParamsi_First + 1
#define GFX_BorderFadeColorLine         GFX_BorderFadeParamsi_First + 2

/* Diaphragm params */
#define GFX_DiaphragmTimeStart          GFX_DiaphragmParamsf_First + 0
#define GFX_DiaphragmTimeClose          GFX_DiaphragmParamsf_First + 1
#define GFX_DiaphragmTimeClosed         GFX_DiaphragmParamsf_First + 2
#define GFX_DiaphragmTimeOpen           GFX_DiaphragmParamsf_First + 3

#define GFX_DiaphragmColor              GFX_DiaphragmParamsi_First + 0

#define GFX_DiaphragmCenter             GFX_DiaphragmParamsv_First + 0

/* Carte params */
#define GFX_Carte_2DPos0                GFX_CarteParamsv_First + 0
#define GFX_Carte_2DPos1                GFX_CarteParamsv_First + 1
#define GFX_Carte_Offset                GFX_CarteParamsv_First + 2
#define GFX_Carte_Size                  GFX_CarteParamsv_First + 3
#define GFX_Carte_CursorSize            GFX_CarteParamsv_First + 4

#define GFX_Carte_Ratio                 GFX_CarteParamsf_First + 0
#define GFX_Carte_BlinkTime             GFX_CarteParamsf_First + 1
#define GFX_Carte_EffectTime            GFX_CarteParamsf_First + 2
#define GFX_Carte_CurEffectTime         GFX_CarteParamsf_First + 3

#define GFX_Carte_TurnOnOff             GFX_CarteParamsi_First + 0
#define GFX_Carte_ShowMapOnOff          GFX_CarteParamsi_First + 1
#define GFX_Carte_FirstPosOnOff         GFX_CarteParamsi_First + 2
#define GFX_Carte_PieceStatus           GFX_CarteParamsi_First + 3
#define GFX_Carte_Pos2Rotation          GFX_CarteParamsi_First + 4
#define GFX_Carte_UncoverPiece          GFX_CarteParamsi_First + 5
#define GFX_Carte_UpdatablePalette      GFX_CarteParamsi_First + 6

/* Sun parameters */
#define GFX_Sun_Pos                     GFX_SunParamsv_First + 0

#define GFX_Sun_BRayAngleTolerance      GFX_SunParamsf_First + 0
#define GFX_Sun_BRayLengthMin           GFX_SunParamsf_First + 1
#define GFX_Sun_BRayLengthMax           GFX_SunParamsf_First + 2
#define GFX_Sun_BRayWidthMin            GFX_SunParamsf_First + 3
#define GFX_Sun_BRayWidhMax             GFX_SunParamsf_First + 4
#define GFX_Sun_SRayAngleTolerance      GFX_SunParamsf_First + 5
#define GFX_Sun_SRayLengthMin           GFX_SunParamsf_First + 6
#define GFX_Sun_SRayLengthMax           GFX_SunParamsf_First + 7
#define GFX_Sun_SRayWidthMin            GFX_SunParamsf_First + 8
#define GFX_Sun_SRayWidthMax            GFX_SunParamsf_First + 9
#define GFX_Sun_LengthRatio             GFX_SunParamsf_First + 10
#define GFX_Sun_TestPointInterval       GFX_SunParamsf_First + 11
#define GFX_Sun_HideTime                GFX_SunParamsf_First + 12
#define GFX_Sun_Distance                GFX_SunParamsf_First + 13

#define GFX_Sun_Compute                 GFX_SunParamsi_First + 0
#define GFX_Sun_Number                  GFX_SunParamsi_First + 1
#define GFX_Sun_Color                   GFX_SunParamsi_First + 2
#define GFX_Sun_Flags                   GFX_SunParamsi_First + 3
#define GFX_Sun_Death                   GFX_SunParamsi_First + 4

/* String parameters */
#define GFX_StrIndex                    GFX_StrParamsi_First + 0
#define GFX_StrColor                    GFX_StrParamsi_First + 1

/* Ripple parameters */
#define GFX_RipplePos                   GFX_RippleParamsv_First + 0
#define GFX_RippleDir                   GFX_RippleParamsv_First + 1
#define GFX_RippleBanking               GFX_RippleParamsv_First + 2

#define GFX_RippleLifeTime              GFX_RippleParamsf_First + 0
#define GFX_RippleIncreaseSpeed         GFX_RippleParamsf_First + 1
#define GFX_RippleGenerationRate        GFX_RippleParamsf_First + 2
#define GFX_RippleSpeed                 GFX_RippleParamsf_First + 3
#define GFX_RippleStartSize             GFX_RippleParamsf_First + 4

#define GFX_RippleSetNumber             GFX_RippleParamsi_First + 0
#define GFX_RippleStartColor		    GFX_RippleParamsi_First + 1
#define GFX_RippleEndColor              GFX_RippleParamsi_First + 2

/* Another flare */
#define GFX_AnotherFlarePos		        GFX_AnotherFlareParamsv_First + 0
#define GFX_AnotherFlareSight           GFX_AnotherFlareParamsv_First + 1

#define GFX_AnotherFlareSize		    GFX_AnotherFlareParamsf_First + 0
#define GFX_AnotherFlareLimit           GFX_AnotherFlareParamsf_First + 1
#define GFX_AnotherFlareOffset          GFX_AnotherFlareParamsf_First + 2
#define GFX_AnotherFlareInWallFactor    GFX_AnotherFlareParamsf_First + 3
#define GFX_AnotherFlareOutWallFactor   GFX_AnotherFlareParamsf_First + 4
#define GFX_AnotherFlareDeathTime       GFX_AnotherFlareParamsf_First + 5
#define GFX_AnotherFlareAngle           GFX_AnotherFlareParamsf_First + 6

#define GFX_AnotherFlareFlags		    GFX_AnotherFlareParamsi_First + 0
#define GFX_AnotherFlareColor		    GFX_AnotherFlareParamsi_First + 1
#define GFX_AnotherFlareDeathColor		GFX_AnotherFlareParamsi_First + 2
#define GFX_AnotherFlareType            GFX_AnotherFlareParamsi_First + 3
#define GFX_AnotherFlareNumber          GFX_AnotherFlareParamsi_First + 4


/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	GFX_tdst_List_
{
	char					c_Type;
	char					c_Flags;
	unsigned short			uw_Id;
	void					*p_Data;
	void					*p_Material;
	void					*p_Owner;
	struct GFX_tdst_List_	*pst_Next;
	float					f_LifeTime;
} GFX_tdst_List;

/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */
#define GFX_M_GetList( _world_, _val_) (((_val_) < GFX_FirstNotSorted) ? _world_->pst_GFXInterface : _world_->pst_GFX)
#define GFX_M_GetListPtr( _world_, _val_) (((_val_) < GFX_FirstNotSorted) ? &_world_->pst_GFXInterface : &_world_->pst_GFX)
    

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void                GFX_Attached( GFX_tdst_List * );
void				GFX_End(void);

void				GFX_SetCurrent(GFX_tdst_List *, USHORT);


void				GFX_Free(GFX_tdst_List **, int);
GFX_tdst_List		*GFX_IsExist(GFX_tdst_List *_pst_GFX, USHORT _uw_Id);

int					GFX_i_SetDeath(GFX_tdst_List **, GFX_tdst_List *);
GFX_tdst_List		*GFX_pst_FindById(GFX_tdst_List **, USHORT);
void				GFX_Render(GFX_tdst_List **, char );
#ifdef USE_DOUBLE_RENDERING	
void 				GFX_Interpolate_All(GFX_tdst_List **_ppst_GFX, u_int Mode , float fInterpoler );
#endif
void				GFX_Test(GFX_tdst_List * );

int                 GFX_i_Add(GFX_tdst_List **, char, void *);
int                 GFX_i_AddSorted(GFX_tdst_List **, char, USHORT, void *);
void				GFX_Del(GFX_tdst_List **, USHORT);
void				GFX_DelOwnedBy(GFX_tdst_List **, void *);

void				GFX_Stop(GFX_tdst_List *, USHORT);
void				GFX_Start(GFX_tdst_List *, USHORT);

void				GFX_FlagSet(GFX_tdst_List *, USHORT, int, int);
void				GFX_SetMaterial(GFX_tdst_List *, USHORT, void *);

void				GFX_NeedGeom(ULONG, ULONG, ULONG, BOOL);
void				GFX_NeedGeomEx(ULONG, ULONG, ULONG, ULONG, BOOL);
void                GFX_NeedGeomExElem(ULONG, ULONG );
void                GFX_NeedGeomNbElemSprites( ULONG );
void                GFX_NeedGeomElemNbSprites( ULONG , ULONG );

int					GFX_i_Request(int, int, int, int);

int					GFX_i_Geti(GFX_tdst_List *, USHORT, int);
void				GFX_Seti(GFX_tdst_List *, USHORT, int, int);
float				GFX_f_Getf(GFX_tdst_List *, USHORT, int);
void				GFX_Setf(GFX_tdst_List *, USHORT, int, float);
MATH_tdst_Vector	*GFX_pst_Getv(GFX_tdst_List *, USHORT, int);
void				GFX_Setv(GFX_tdst_List *, USHORT, int, MATH_tdst_Vector *);


/*$4
 ***********************************************************************************************************************
    specific Functions
 ***********************************************************************************************************************
 */
void GFX_Carte_Object( GFX_tdst_List *, USHORT , struct OBJ_tdst_GameObject_ *, int, int );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFX_H__ */
