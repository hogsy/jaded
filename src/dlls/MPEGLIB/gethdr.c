
/* gethdr.c, header decoding                                                */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */

#include <stdio.h>

#include "config.h"
#include "global.h"


/* private prototypes */
static void sequence_header (void);
static void group_of_pictures_header (void);
static void picture_header (void);
static void extension_and_user_data (void);
static void sequence_extension (void);
static void sequence_display_extension (void);
static void quant_matrix_extension (void);
static void sequence_scalable_extension (void);
static void picture_display_extension (void);
static void picture_coding_extension (void);
static void picture_spatial_scalable_extension (void);
static void picture_temporal_scalable_extension (void);
static int  extra_bit_information (void);
static void copyright_extension (void);
static void user_data (void);
static void user_data (void);


/* introduced in September 1995 to assist spatial scalable decoding */
static void Update_Temporal_Reference_Tacking_Data (void);
/* private variables */
static int Temporal_Reference_Base = 0;
static int True_Framenum_max  = -1;
static int Temporal_Reference_GOP_Reset = 0;

#define RESERVED    -1 
static double frame_rate_Table[16] =
{
  0.0,
  ((23.0*1000.0)/1001.0),
  24.0,
  25.0,
  ((30.0*1000.0)/1001.0),
  30.0,
  50.0,
  ((60.0*1000.0)/1001.0),
  60.0,
 
  RESERVED,
  RESERVED,
  RESERVED,
  RESERVED,
  RESERVED,
  RESERVED,
  RESERVED
};

/*
 * decode headers from one input stream
 * until an End of Sequence or picture start code
 * is found
 */
int Get_Hdr()
{
  unsigned int code;

  for (;;)
  {
    /* look for next_start_code */
    next_start_code();
    code = Get_Bits32();
  
    switch (code)
    {
    case SEQUENCE_HEADER_CODE:
      sequence_header();
      break;
    case GROUP_START_CODE:
      group_of_pictures_header();
      break;
    case PICTURE_START_CODE:
      picture_header();
      return 1;
      break;
    case SEQUENCE_END_CODE:
      return 0;
      break;
    default:
      break;
    }
  }
}



/* decode sequence header */

static void sequence_header()
{
  int i;
  int pos;

  pos = gst_AllGloballs.ld->Bitcnt;
  gst_AllGloballs.horizontal_size             = Get_Bits(12);
  gst_AllGloballs.vertical_size               = Get_Bits(12);
  gst_AllGloballs.aspect_ratio_information    = Get_Bits(4);
  gst_AllGloballs.frame_rate_code             = Get_Bits(4);
  gst_AllGloballs.bit_rate_value              = Get_Bits(18);
  marker_bit("sequence_header()");
  gst_AllGloballs.vbv_buffer_size             = Get_Bits(10);
  gst_AllGloballs.constrained_parameters_flag = Get_Bits(1);

  if((gst_AllGloballs.ld->load_intra_quantizer_matrix = Get_Bits(1)))
  {
    for (i=0; i<64; i++)
      gst_AllGloballs.ld->intra_quantizer_matrix[scan[ZIG_ZAG][i]] = Get_Bits(8);
  }
  else
  {
    for (i=0; i<64; i++)
      gst_AllGloballs.ld->intra_quantizer_matrix[i] = default_intra_quantizer_matrix[i];
  }

  if((gst_AllGloballs.ld->load_non_intra_quantizer_matrix = Get_Bits(1)))
  {
    for (i=0; i<64; i++)
      gst_AllGloballs.ld->non_intra_quantizer_matrix[scan[ZIG_ZAG][i]] = Get_Bits(8);
  }
  else
  {
    for (i=0; i<64; i++)
      gst_AllGloballs.ld->non_intra_quantizer_matrix[i] = 16;
  }

  /* copy luminance to chrominance matrices */
  for (i=0; i<64; i++)
  {
    gst_AllGloballs.ld->chroma_intra_quantizer_matrix[i] =
      gst_AllGloballs.ld->intra_quantizer_matrix[i];

    gst_AllGloballs.ld->chroma_non_intra_quantizer_matrix[i] =
      gst_AllGloballs.ld->non_intra_quantizer_matrix[i];
  }


  extension_and_user_data();
}



/* decode group of pictures header */
/* ISO/IEC 13818-2 section 6.2.2.6 */
static void group_of_pictures_header()
{
  int pos;

  if (gst_AllGloballs.ld == &gst_AllGloballs.base)
  {
    Temporal_Reference_Base = True_Framenum_max + 1; 	/* *CH* */
    Temporal_Reference_GOP_Reset = 1;
  }
  pos = gst_AllGloballs.ld->Bitcnt;
  gst_AllGloballs.drop_flag   = Get_Bits(1);
  gst_AllGloballs.hour        = Get_Bits(5);
  gst_AllGloballs.minute      = Get_Bits(6);
  marker_bit("group_of_pictures_header()");
  gst_AllGloballs.sec         = Get_Bits(6);
  gst_AllGloballs.frame       = Get_Bits(6);
  gst_AllGloballs.closed_gop  = Get_Bits(1);
  gst_AllGloballs.broken_link = Get_Bits(1);


  extension_and_user_data();

}


/* decode picture header */

/* ISO/IEC 13818-2 section 6.2.3 */
static void picture_header()
{
  int pos;
  int Extra_Information_Byte_Count;

  /* unless later overwritten by picture_spatial_scalable_extension() */
  gst_AllGloballs.ld->pict_scal = 0; 
  
  pos = gst_AllGloballs.ld->Bitcnt;
  gst_AllGloballs.temporal_reference  = Get_Bits(10);
  gst_AllGloballs.picture_coding_type = Get_Bits(3);
  gst_AllGloballs.vbv_delay           = Get_Bits(16);

  if (gst_AllGloballs.picture_coding_type==P_TYPE || gst_AllGloballs.picture_coding_type==B_TYPE)
  {
    gst_AllGloballs.full_pel_forward_vector = Get_Bits(1);
    gst_AllGloballs.forward_f_code = Get_Bits(3);
  }
  if (gst_AllGloballs.picture_coding_type==B_TYPE)
  {
    gst_AllGloballs.full_pel_backward_vector = Get_Bits(1);
    gst_AllGloballs.backward_f_code = Get_Bits(3);
  }

  Extra_Information_Byte_Count = 
    extra_bit_information();
  
  extension_and_user_data();

  /* update tracking information used to assist spatial scalability */
  Update_Temporal_Reference_Tacking_Data();
}

/* decode slice header */

/* ISO/IEC 13818-2 section 6.2.4 */
int slice_header()
{
  int slice_vertical_position_extension;
  int quantizer_scale_code;
  int pos;
  int slice_picture_id_enable = 0;
  int slice_picture_id = 0;
  int extra_information_slice = 0;

  pos = gst_AllGloballs.ld->Bitcnt;

  slice_vertical_position_extension =
    (gst_AllGloballs.ld->MPEG2_Flag && gst_AllGloballs.vertical_size>2800) ? Get_Bits(3) : 0;

  if (gst_AllGloballs.ld->scalable_mode==SC_DP)
    gst_AllGloballs.ld->priority_breakpoint = Get_Bits(7);

  quantizer_scale_code = Get_Bits(5);
  gst_AllGloballs.ld->quantizer_scale =
    gst_AllGloballs.ld->MPEG2_Flag ? (gst_AllGloballs.ld->q_scale_type ? Non_Linear_quantizer_scale[quantizer_scale_code] : quantizer_scale_code<<1) : quantizer_scale_code;

  /* slice_id introduced in March 1995 as part of the video corridendum
     (after the IS was drafted in November 1994) */
  if (Get_Bits(1))
  {
    gst_AllGloballs.ld->intra_slice = Get_Bits(1);

    slice_picture_id_enable = Get_Bits(1);
	slice_picture_id = Get_Bits(6);

    extra_information_slice = extra_bit_information();
  }
  else
    gst_AllGloballs.ld->intra_slice = 0;

  return slice_vertical_position_extension;
}


/* decode extension and user data */
/* ISO/IEC 13818-2 section 6.2.2.2 */
static void extension_and_user_data()
{
  int code,ext_ID;

  next_start_code();

  while ((code = Show_Bits(32))==EXTENSION_START_CODE || code==USER_DATA_START_CODE)
  {
    if (code==EXTENSION_START_CODE)
    {
      Flush_Buffer32();
      ext_ID = Get_Bits(4);
      switch (ext_ID)
      {
      case SEQUENCE_EXTENSION_ID:
        sequence_extension();
        break;
      case SEQUENCE_DISPLAY_EXTENSION_ID:
        sequence_display_extension();
        break;
      case QUANT_MATRIX_EXTENSION_ID:
        quant_matrix_extension();
        break;
      case SEQUENCE_SCALABLE_EXTENSION_ID:
        sequence_scalable_extension();
        break;
      case PICTURE_DISPLAY_EXTENSION_ID:
        picture_display_extension();
        break;
      case PICTURE_CODING_EXTENSION_ID:
        picture_coding_extension();
        break;
      case PICTURE_SPATIAL_SCALABLE_EXTENSION_ID:
        picture_spatial_scalable_extension();
        break;
      case PICTURE_TEMPORAL_SCALABLE_EXTENSION_ID:
        picture_temporal_scalable_extension();
        break;
      case COPYRIGHT_EXTENSION_ID:
        copyright_extension();
        break;
     default:
        Error("reserved extension start code ID \n");
        break;
      }
      next_start_code();
    }
    else
    {
      Flush_Buffer32();
      user_data();
    }
  }
}


/* decode sequence extension */

/* ISO/IEC 13818-2 section 6.2.2.3 */
static void sequence_extension()
{
  int horizontal_size_extension;
  int vertical_size_extension;
  int bit_rate_extension;
  int vbv_buffer_size_extension;

  /* derive bit position for trace */
  gst_AllGloballs.ld->MPEG2_Flag = 1;

  gst_AllGloballs.ld->scalable_mode = SC_NONE; /* unless overwritten by sequence_scalable_extension() */
  gst_AllGloballs.layer_id = 0;                /* unless overwritten by sequence_scalable_extension() */
  
  gst_AllGloballs.profile_and_level_indication	= Get_Bits(8);
  gst_AllGloballs.progressive_sequence			= Get_Bits(1);
  gst_AllGloballs.chroma_format					= Get_Bits(2);
  horizontal_size_extension						= Get_Bits(2);
  vertical_size_extension						= Get_Bits(2);
  bit_rate_extension							= Get_Bits(12);
  marker_bit("sequence_extension");
  vbv_buffer_size_extension						= Get_Bits(8);
  gst_AllGloballs.low_delay						= Get_Bits(1);
  gst_AllGloballs.frame_rate_extension_n		= Get_Bits(2);
  gst_AllGloballs.frame_rate_extension_d		= Get_Bits(5);

  gst_AllGloballs.frame_rate = frame_rate_Table[gst_AllGloballs.frame_rate_code] *
    ((gst_AllGloballs.frame_rate_extension_n+1)/(gst_AllGloballs.frame_rate_extension_d+1));

  /* special case for 422 profile & level must be made */
  if((gst_AllGloballs.profile_and_level_indication>>7) & 1)
  {  /* escape bit of profile_and_level_indication set */
  
    /* 4:2:2 Profile @ Main Level */
    if((gst_AllGloballs.profile_and_level_indication&15)==5)
    {
      gst_AllGloballs.profile = PROFILE_422;
      gst_AllGloballs.level   = MAIN_LEVEL;  
    }
  }
  else
  {
    gst_AllGloballs.profile = gst_AllGloballs.profile_and_level_indication >> 4;  /* Profile is upper nibble */
    gst_AllGloballs.level   = gst_AllGloballs.profile_and_level_indication & 0xF;  /* Level is lower nibble */
  }
  
 
  gst_AllGloballs.horizontal_size = (horizontal_size_extension<<12) | (gst_AllGloballs.horizontal_size&0x0fff);
  gst_AllGloballs.vertical_size = (vertical_size_extension<<12) | (gst_AllGloballs.vertical_size&0x0fff);


  /* ISO/IEC 13818-2 does not define bit_rate_value to be composed of
   * both the original bit_rate_value parsed in sequence_header() and
   * the optional bit_rate_extension in sequence_extension_header(). 
   * However, we use it for bitstream verification purposes. 
   */

  gst_AllGloballs.bit_rate_value += (bit_rate_extension << 18);
  gst_AllGloballs.bit_rate = ((double) gst_AllGloballs.bit_rate_value) * 400.0;
  gst_AllGloballs.vbv_buffer_size += (vbv_buffer_size_extension << 10);

}


/* decode sequence display extension */

static void sequence_display_extension()
{
  int pos;

  pos = gst_AllGloballs.ld->Bitcnt;
  gst_AllGloballs.video_format      = Get_Bits(3);
  gst_AllGloballs.color_description = Get_Bits(1);

  if (gst_AllGloballs.color_description)
  {
    gst_AllGloballs.color_primaries          = Get_Bits(8);
    gst_AllGloballs.transfer_characteristics = Get_Bits(8);
    gst_AllGloballs.matrix_coefficients      = Get_Bits(8);
  }

  gst_AllGloballs.display_horizontal_size = Get_Bits(14);
  marker_bit("sequence_display_extension");
  gst_AllGloballs.display_vertical_size   = Get_Bits(14);


}


/* decode quant matrix entension */
/* ISO/IEC 13818-2 section 6.2.3.2 */
static void quant_matrix_extension()
{
  int i;
  int pos;

  pos = gst_AllGloballs.ld->Bitcnt;

  if((gst_AllGloballs.ld->load_intra_quantizer_matrix = Get_Bits(1)))
  {
    for (i=0; i<64; i++)
    {
      gst_AllGloballs.ld->chroma_intra_quantizer_matrix[scan[ZIG_ZAG][i]]
      = gst_AllGloballs.ld->intra_quantizer_matrix[scan[ZIG_ZAG][i]]
      = Get_Bits(8);
    }
  }

  if((gst_AllGloballs.ld->load_non_intra_quantizer_matrix = Get_Bits(1)))
  {
    for (i=0; i<64; i++)
    {
      gst_AllGloballs.ld->chroma_non_intra_quantizer_matrix[scan[ZIG_ZAG][i]]
      = gst_AllGloballs.ld->non_intra_quantizer_matrix[scan[ZIG_ZAG][i]]
      = Get_Bits(8);
    }
  }

  if((gst_AllGloballs.ld->load_chroma_intra_quantizer_matrix = Get_Bits(1)))
  {
    for (i=0; i<64; i++)
      gst_AllGloballs.ld->chroma_intra_quantizer_matrix[scan[ZIG_ZAG][i]] = Get_Bits(8);
  }

  if((gst_AllGloballs.ld->load_chroma_non_intra_quantizer_matrix = Get_Bits(1)))
  {
    for (i=0; i<64; i++)
      gst_AllGloballs.ld->chroma_non_intra_quantizer_matrix[scan[ZIG_ZAG][i]] = Get_Bits(8);
  }


}


/* decode sequence scalable extension */
/* ISO/IEC 13818-2   section 6.2.2.5 */
static void sequence_scalable_extension()
{
  int pos;

  pos = gst_AllGloballs.ld->Bitcnt;

  /* values (without the +1 offset) of scalable_mode are defined in 
     Table 6-10 of ISO/IEC 13818-2 */
  gst_AllGloballs.ld->scalable_mode = Get_Bits(2) + 1; /* add 1 to make SC_DP != SC_NONE */

  gst_AllGloballs.layer_id = Get_Bits(4);

  if (gst_AllGloballs.ld->scalable_mode==SC_SPAT)
  {
    gst_AllGloballs.lower_layer_prediction_horizontal_size = Get_Bits(14);
    marker_bit("sequence_scalable_extension()");
    gst_AllGloballs.lower_layer_prediction_vertical_size   = Get_Bits(14); 
    gst_AllGloballs.horizontal_subsampling_factor_m        = Get_Bits(5);
    gst_AllGloballs.horizontal_subsampling_factor_n        = Get_Bits(5);
    gst_AllGloballs.vertical_subsampling_factor_m          = Get_Bits(5);
    gst_AllGloballs.vertical_subsampling_factor_n          = Get_Bits(5);
  }

  if (gst_AllGloballs.ld->scalable_mode==SC_TEMP)
    Error("temporal scalability not implemented\n");


}


/* decode picture display extension */
/* ISO/IEC 13818-2 section 6.2.3.3. */
static void picture_display_extension()
{
  int i;
  int number_of_frame_center_offsets;
  int pos;

  pos = gst_AllGloballs.ld->Bitcnt;
  /* based on ISO/IEC 13818-2 section 6.3.12 
    (November 1994) Picture display extensions */

  /* derive number_of_frame_center_offsets */
  if(gst_AllGloballs.progressive_sequence)
  {
    if(gst_AllGloballs.repeat_first_field)
    {
      if(gst_AllGloballs.top_field_first)
        number_of_frame_center_offsets = 3;
      else
        number_of_frame_center_offsets = 2;
    }
    else
    {
      number_of_frame_center_offsets = 1;
    }
  }
  else
  {
    if(gst_AllGloballs.picture_structure!=FRAME_PICTURE)
    {
      number_of_frame_center_offsets = 1;
    }
    else
    {
      if(gst_AllGloballs.repeat_first_field)
        number_of_frame_center_offsets = 3;
      else
        number_of_frame_center_offsets = 2;
    }
  }


  /* now parse */
  for (i=0; i<number_of_frame_center_offsets; i++)
  {
    gst_AllGloballs.frame_center_horizontal_offset[i] = Get_Bits(16);
    marker_bit("picture_display_extension, first marker bit");
    
    gst_AllGloballs.frame_center_vertical_offset[i]   = Get_Bits(16);
    marker_bit("picture_display_extension, second marker bit");
  }

}


/* decode picture coding extension */
static void picture_coding_extension()
{
  int pos;

  pos = gst_AllGloballs.ld->Bitcnt;

  gst_AllGloballs.f_code[0][0] = Get_Bits(4);
  gst_AllGloballs.f_code[0][1] = Get_Bits(4);
  gst_AllGloballs.f_code[1][0] = Get_Bits(4);
  gst_AllGloballs.f_code[1][1] = Get_Bits(4);

  gst_AllGloballs.intra_dc_precision         = Get_Bits(2);
  gst_AllGloballs.picture_structure          = Get_Bits(2);
  gst_AllGloballs.top_field_first            = Get_Bits(1);
  gst_AllGloballs.frame_pred_frame_dct       = Get_Bits(1);
  gst_AllGloballs.concealment_motion_vectors = Get_Bits(1);
  gst_AllGloballs.ld->q_scale_type           = Get_Bits(1);
  gst_AllGloballs.intra_vlc_format           = Get_Bits(1);
  gst_AllGloballs.ld->alternate_scan         = Get_Bits(1);
  gst_AllGloballs.repeat_first_field         = Get_Bits(1);
  gst_AllGloballs.chroma_420_type            = Get_Bits(1);
  gst_AllGloballs.progressive_frame          = Get_Bits(1);
  gst_AllGloballs.composite_display_flag     = Get_Bits(1);

  if (gst_AllGloballs.composite_display_flag)
  {
    gst_AllGloballs.v_axis            = Get_Bits(1);
    gst_AllGloballs.field_sequence    = Get_Bits(3);
    gst_AllGloballs.sub_carrier       = Get_Bits(1);
    gst_AllGloballs.burst_amplitude   = Get_Bits(7);
    gst_AllGloballs.sub_carrier_phase = Get_Bits(8);
  }

}


/* decode picture spatial scalable extension */
/* ISO/IEC 13818-2 section 6.2.3.5. */
static void picture_spatial_scalable_extension()
{
  int pos;

  pos = gst_AllGloballs.ld->Bitcnt;

  gst_AllGloballs.ld->pict_scal = 1; /* use spatial scalability in this picture */

  gst_AllGloballs.lower_layer_temporal_reference = Get_Bits(10);
  marker_bit("picture_spatial_scalable_extension(), first marker bit");
  gst_AllGloballs.lower_layer_horizontal_offset = Get_Bits(15);
  if (gst_AllGloballs.lower_layer_horizontal_offset>=16384)
    gst_AllGloballs.lower_layer_horizontal_offset-= 32768;
  marker_bit("picture_spatial_scalable_extension(), second marker bit");
  gst_AllGloballs.lower_layer_vertical_offset = Get_Bits(15);
  if (gst_AllGloballs.lower_layer_vertical_offset>=16384)
    gst_AllGloballs.lower_layer_vertical_offset-= 32768;
  gst_AllGloballs.spatial_temporal_weight_code_table_index = Get_Bits(2);
  gst_AllGloballs.lower_layer_progressive_frame = Get_Bits(1);
  gst_AllGloballs.lower_layer_deinterlaced_field_select = Get_Bits(1);

}


/* decode picture temporal scalable extension
 *
 * not implemented
 */
/* ISO/IEC 13818-2 section 6.2.3.4. */
static void picture_temporal_scalable_extension()
{
  Error("temporal scalability not supported\n");

}


/* decode extra bit information */
/* ISO/IEC 13818-2 section 6.2.3.4. */
static int extra_bit_information()
{
  int Byte_Count = 0;

  while (Get_Bits1())
  {
    Flush_Buffer8();
    Byte_Count++;
  }

  return(Byte_Count);
}



/* ISO/IEC 13818-2 section 5.3 */
/* Purpose: this function is mainly designed to aid in bitstream conformance
   testing.  A simple Flush_Buffer(1) would do */
#ifdef JADEFUSION
void marker_bit(char *text)
#else
void marker_bit(text)
char *text;
#endif
{
  int marker;

  marker = Get_Bits(1);

}


/* ISO/IEC 13818-2  sections 6.3.4.1 and 6.2.2.2.2 */
static void user_data()
{
  /* skip ahead to the next start code */
  next_start_code();
}



/* Copyright extension */
/* ISO/IEC 13818-2 section 6.2.3.6. */
/* (header added in November, 1994 to the IS document) */


static void copyright_extension()
{
  int pos;
  int reserved_data;

  pos = gst_AllGloballs.ld->Bitcnt;
  

  gst_AllGloballs.copyright_flag =       Get_Bits(1); 
  gst_AllGloballs.copyright_identifier = Get_Bits(8);
  gst_AllGloballs.original_or_copy =     Get_Bits(1);
  
  /* reserved */
  reserved_data = Get_Bits(7);

  marker_bit("copyright_extension(), first marker bit");
  gst_AllGloballs.copyright_number_1 =   Get_Bits(20);
  marker_bit("copyright_extension(), second marker bit");
  gst_AllGloballs.copyright_number_2 =   Get_Bits(22);
  marker_bit("copyright_extension(), third marker bit");
  gst_AllGloballs.copyright_number_3 =   Get_Bits(22);


}



/* introduced in September 1995 to assist Spatial Scalability */
static void Update_Temporal_Reference_Tacking_Data()
{
  static int temporal_reference_wrap  = 0;
  static int temporal_reference_old   = 0;

  if (gst_AllGloballs.ld == &gst_AllGloballs.base)			/* *CH* */
  {
    if (gst_AllGloballs.picture_coding_type!=B_TYPE && gst_AllGloballs.temporal_reference!=temporal_reference_old) 	
    /* check first field of */
    {							
       /* non-B-frame */
      if (temporal_reference_wrap) 		
      {/* wrap occured at previous I- or P-frame */	
       /* now all intervening B-frames which could 
          still have high temporal_reference values are done  */
        Temporal_Reference_Base += 1024;
	    temporal_reference_wrap = 0;
      }
      
      /* distinguish from a reset */
      if (gst_AllGloballs.temporal_reference<temporal_reference_old && !Temporal_Reference_GOP_Reset)	
	    temporal_reference_wrap = 1;  /* we must have just passed a GOP-Header! */
      
      temporal_reference_old = gst_AllGloballs.temporal_reference;
      Temporal_Reference_GOP_Reset = 0;
    }

    gst_AllGloballs.True_Framenum = Temporal_Reference_Base + gst_AllGloballs.temporal_reference;
    
    /* temporary wrap of TR at 1024 for M frames */
    if (temporal_reference_wrap && gst_AllGloballs.temporal_reference <= temporal_reference_old)	
      gst_AllGloballs.True_Framenum += 1024;				

    True_Framenum_max = (gst_AllGloballs.True_Framenum > True_Framenum_max) ?
                        gst_AllGloballs.True_Framenum : True_Framenum_max;
  }
}
