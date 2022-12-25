
/* mpeg2dec.c, main(), initialization, option processing                    */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>

#define GLOBAL
#include "config.h"
#include "global.h"

/* private prototypes */
static int  video_sequence (int *framenum);
static int Decode_Bitstream (void);
static int  Headers (void);
static void Initialize_Sequence (void);
static void Initialize_Decoder (void);
static void Deinitialize_Sequence (void);
static void Process_Options ();


static void Clear_Options();

#ifdef JADEFUSION
int MPEG_INIT(struct MPG_IOStruct_ *p_IO)
#else
int MPEG_INIT(MPG_IOStruct *p_IO)
#endif
{
	int code;

	memset(&gst_AllGloballs,0,sizeof(gst_AllGloballs));
	gst_AllGloballs.stSystem = p_IO;
	gst_AllGloballs.stSystem->GoStart();
	
	
	Clear_Options();
	
	/* decode command line arguments */
	Process_Options();
	
	gst_AllGloballs.ld = &gst_AllGloballs.base; /* select gst_AllGloballs.base layer context */
	
	/* open MPEG gst_AllGloballs.base layer bitstream file(s) */
	/* NOTE: this is either a gst_AllGloballs.base layer stream or a spatial enhancement stream */
	
	Initialize_Buffer(); 
	
	if(Show_Bits(8)==0x47)
	{
		Error("Decoder currently does not parse transport streams\n");
	}
	
	next_start_code();
	code = Show_Bits(32);
	
	switch(code)
	{
	case SEQUENCE_HEADER_CODE:
		break;
	case PACK_START_CODE:
	case VIDEO_ELEMENTARY_STREAM:
		gst_AllGloballs.System_Stream_Flag = 1;
		break;
	default:
		Error("Decoder currently does not parse transport streams\n");
		break;
	}
	
	gst_AllGloballs.stSystem->GoStart();
	Initialize_Buffer(); 
	
	gst_AllGloballs.stSystem->GoStart();
	
	Initialize_Buffer(); 
	
	Initialize_Decoder();
	Get_Hdr();
	Initialize_Sequence();
	p_IO -> SizeX = gst_AllGloballs.Coded_Picture_Width ;
	p_IO -> SizeY = gst_AllGloballs.Coded_Picture_Height;
	p_IO -> fFrameRate = 30.0f;
	p_IO -> ulCurrentFrame = 0;
	p_IO -> ulNumberOfFrames = -1;
	gst_AllGloballs.I_TypeCounter = 0;
	return 0;
}

int MPEG_FRAME()
{
	int ret;
	static int ChessCounter = 0;
	unsigned long SaveFlags;
	gst_AllGloballs.ulFLAGS = gst_AllGloballs.stSystem->ulFlags;
	gst_AllGloballs.stSystem->ulCurrentFrame ++;
	gst_AllGloballs.MY_framenum++;
	gst_AllGloballs.stSystem->ulBufferResult = NULL;


	if (gst_AllGloballs.MY_framenum == 1)
	{
		Decode_Picture(0, 0);
		return 0;
	}
	SaveFlags = gst_AllGloballs.ulFLAGS;

	/* loop through the rest of the pictures in the sequence */
    ret = Headers();
	
    if(ChessCounter < 2)
	{
		Decode_Picture(gst_AllGloballs.MY_framenum -1, gst_AllGloballs.MY_framenum-1);
		if(ret==1) ChessCounter = 0;
		else ChessCounter++;
	}
	else
	{
		/* restart */
		ChessCounter = 0;
		gst_AllGloballs.ulFLAGS = SaveFlags;
		MPEG_END();
		MPEG_INIT(gst_AllGloballs.stSystem);
		return 1;
	}
	gst_AllGloballs.ulFLAGS = SaveFlags;
	return 0;
}

int MPEG_END()
{
	Deinitialize_Sequence();
	return 0l;
}

/* IMPLEMENTAION specific rouintes */
static void Initialize_Decoder()
{
	int i;
	
	/* gst_AllGloballs.Clip table */
	if (!(gst_AllGloballs.Clip=(unsigned char *)MPG_MALLOC(1024)))
		Error("gst_AllGloballs.Clip[] malloc failed\n");
	
	gst_AllGloballs.Clip += 384;
	
	for (i=-384; i<640; i++)
		gst_AllGloballs.Clip[i] = (i<0) ? 0 : ((i>255) ? 255 : i);
	
	/* IDCT */
	Initialize_Fast_IDCT();
	Initialize_Reference_IDCT();
	
}
#define RESTART_TRESH 10
void MPEG_GOTO(int ulNum)
{
	unsigned long SaveFlags;
	long Delta;
	Delta = ulNum - gst_AllGloballs.MY_framenum;
	SaveFlags = gst_AllGloballs.stSystem->ulFlags;
	if (Delta < 0)
	{
		MPEG_END();
		MPEG_INIT(gst_AllGloballs.stSystem);
		Delta = ulNum;
	}
	if (Delta <= 1) return;

	if (Delta > 0)
	{
		if (Delta > RESTART_TRESH)
		{
			gst_AllGloballs.stSystem->ulFlags |= MPEG_Mode_SuperFast;
			Delta -= RESTART_TRESH;
			while (Delta--) MPEG_FRAME();
			Delta += RESTART_TRESH;
		}
		gst_AllGloballs.stSystem->ulFlags = SaveFlags ;
		while (Delta--) MPEG_FRAME();
	}
	gst_AllGloballs.stSystem->ulFlags = SaveFlags ;
}

/* mostly IMPLEMENTAION specific rouintes */
static void Initialize_Sequence()
{
	int cc, size;
	static int Table_6_20[3] = {6,8,12};
	
	/* see ISO/IEC 13818-2 section D.9.14 */
	if (!gst_AllGloballs.base.MPEG2_Flag)
	{
		gst_AllGloballs.progressive_sequence = 1;
		gst_AllGloballs.progressive_frame = 1;
		gst_AllGloballs.picture_structure = FRAME_PICTURE;
		gst_AllGloballs.frame_pred_frame_dct = 1;
		gst_AllGloballs.chroma_format = CHROMA420;
		gst_AllGloballs.matrix_coefficients = 5;
	}
	
	/* round to nearest multiple of coded macroblocks */
	/* ISO/IEC 13818-2 section 6.3.3 sequence_header() */
	gst_AllGloballs.mb_width = (gst_AllGloballs.horizontal_size+15)/16;
	gst_AllGloballs.mb_height = (gst_AllGloballs.base.MPEG2_Flag && !gst_AllGloballs.progressive_sequence) ? 2*((gst_AllGloballs.vertical_size+31)/32)
		: (gst_AllGloballs.vertical_size+15)/16;
	
	gst_AllGloballs.Coded_Picture_Width = 16*gst_AllGloballs.mb_width;
	gst_AllGloballs.Coded_Picture_Height = 16*gst_AllGloballs.mb_height;
	
	/* ISO/IEC 13818-2 sections 6.1.1.8, 6.1.1.9, and 6.1.1.10 */
	gst_AllGloballs.Chroma_Width = (gst_AllGloballs.chroma_format==CHROMA444) ? gst_AllGloballs.Coded_Picture_Width
		: gst_AllGloballs.Coded_Picture_Width>>1;
	gst_AllGloballs.Chroma_Height = (gst_AllGloballs.chroma_format!=CHROMA420) ? gst_AllGloballs.Coded_Picture_Height
		: gst_AllGloballs.Coded_Picture_Height>>1;
	
	/* derived based on Table 6-20 in ISO/IEC 13818-2 section 6.3.17 */
	gst_AllGloballs.block_count = Table_6_20[gst_AllGloballs.chroma_format-1];
	
	for (cc=0; cc<3; cc++)
	{
		if (cc==0)
			size = gst_AllGloballs.Coded_Picture_Width*gst_AllGloballs.Coded_Picture_Height;
		else
			size = gst_AllGloballs.Chroma_Width*gst_AllGloballs.Chroma_Height;
		
		if (!(gst_AllGloballs.backward_reference_frame[cc] = (unsigned char *)MPG_MALLOC(size)))
			Error("backward_reference_frame[] malloc failed\n");
		
		if (!(gst_AllGloballs.forward_reference_frame[cc] = (unsigned char *)MPG_MALLOC(size)))
			Error("forward_reference_frame[] malloc failed\n");
		
		if (!(gst_AllGloballs.auxframe[cc] = (unsigned char *)MPG_MALLOC(size)))
			Error("auxframe[] malloc failed\n");
		
		
		if (gst_AllGloballs.base.scalable_mode==SC_SPAT)
		{
			/* this assumes lower layer is 4:2:0 */
			if (!(gst_AllGloballs.llframe0[cc] = (unsigned char *)MPG_MALLOC((gst_AllGloballs.lower_layer_prediction_horizontal_size*gst_AllGloballs.lower_layer_prediction_vertical_size)/(cc?4:1))))
				Error("llframe0 malloc failed\n");
			if (!(gst_AllGloballs.llframe1[cc] = (unsigned char *)MPG_MALLOC((gst_AllGloballs.lower_layer_prediction_horizontal_size*gst_AllGloballs.lower_layer_prediction_vertical_size)/(cc?4:1))))
				Error("llframe1 malloc failed\n");
		}
	}
	
	/* SCALABILITY: Spatial */
	if (gst_AllGloballs.base.scalable_mode==SC_SPAT)
	{
		if (!(gst_AllGloballs.lltmp = (short *)MPG_MALLOC(gst_AllGloballs.lower_layer_prediction_horizontal_size*((gst_AllGloballs.lower_layer_prediction_vertical_size*gst_AllGloballs.vertical_subsampling_factor_n)/gst_AllGloballs.vertical_subsampling_factor_m)*sizeof(short))))
			Error("lltmp malloc failed\n");
	}
	
	Initialize_Display_Process();
	Initialize_Dither_Matrix();
	
}

#ifdef JADEFUSION
void Error(char *text)
#else
void Error(text)
char *text;
#endif
{
	exit(1);
}



/* option processing */
static void Process_Options()
{
	gst_AllGloballs.Frame_Store_Flag = 0; /* to avoid calling dither() twice */
	if(gst_AllGloballs.Frame_Store_Flag)
		gst_AllGloballs.Display_Progressive_Flag = 1;
	else
		gst_AllGloballs.Display_Progressive_Flag = 0;
	
}



static int Headers()
{
	int ret;
	gst_AllGloballs.ld = &gst_AllGloballs.base;
	ret = Get_Hdr();
	return ret;
}



static int Decode_Bitstream()
{
}


static void Deinitialize_Sequence()
{
	int i;
	
	/* clear flags */
	gst_AllGloballs.base.MPEG2_Flag=0;
	Terminate_Display_Process();
	
	for(i=0;i<3;i++)
	{
		free(gst_AllGloballs.backward_reference_frame[i]);
		free(gst_AllGloballs.forward_reference_frame[i]);
		free(gst_AllGloballs.auxframe[i]);
		
		if (gst_AllGloballs.base.scalable_mode==SC_SPAT)
		{
			free(gst_AllGloballs.llframe0[i]);
			free(gst_AllGloballs.llframe1[i]);
		}
	}
	
	if (gst_AllGloballs.base.scalable_mode==SC_SPAT)
		free(gst_AllGloballs.lltmp);
	
}


static int video_sequence(int *Bitstream_Framenumber)
{
}



static void Clear_Options()
{
	gst_AllGloballs.Frame_Store_Flag = 0;
	gst_AllGloballs.Reference_IDCT_Flag = 0;
}


