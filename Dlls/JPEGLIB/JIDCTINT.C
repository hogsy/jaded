/*$T Jidctint.c GC! 1.097 02/16/01 13:42:10 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 * jidctint.c Copyright (C) 1991-1994, Thomas G. Lane. This file is part of the
 * Independent JPEG Group's software. For conditions of distribution and use, see
 * the accompanying README file. This file contains a slow-but-accurate integer
 * implementation of the inverse DCT (Discrete Cosine Transform). In the IJG code,
 * this routine must also perform dequantization of the input coefficients. A 2-D
 * IDCT can be done by 1-D IDCT on each column followed by 1-D IDCT on each row
 * (or vice versa, but it's more convenient to emit a row at a time). Direct
 * algorithms are also available, but they are much more complex and seem not to
 * be any faster when reduced to code. This implementation is based on an
 * algorithm described in C. Loeffler, A. Ligtenberg and G. Moschytz, "Practical
 * Fast 1-D DCT Algorithms with 11 Multiplications", Proc. Int'l. Conf. on
 * Acoustics, Speech, and Signal Processing 1989 (ICASSP '89), pp. 988-991. The
 * primary algorithm described there uses 11 multiplies and 29 adds. We use their
 * alternate method with 12 multiplies and 32 adds. The advantage of this method
 * is that no data path contains more than one multiplication; this allows a very
 * simple and accurate implementation in scaled fixed-point arithmetic, with a
 * minimal number of shifts.
 */
#define JPEG_INTERNALS
#include "JINCLUDE.H"
#include "JPEGLIB.H"
#include "JDCT.H"							/* Private declarations for DCT subsystem */

#ifdef DCT_ISLOW_SUPPORTED

/* This module is specialized to the case DCTSIZE = 8. */
#if DCTSIZE != 8
Sorry, this code only copes with 8 x8 DCTs. /* deliberate syntax err */
#endif

/*
 * The poop on this scaling stuff is as follows: Each 1-D IDCT step produces
 * outputs which are a factor of sqrt(N) larger than the true IDCT outputs. The
 * final outputs are therefore a factor of N larger than desired; since N=8 this
 * can be cured by a simple right shift at the end of the algorithm. The advantage
 * of this arrangement is that we save two multiplications per 1-D IDCT, because
 * the y0 and y4 inputs need not be divided by sqrt(N). We have to do addition and
 * subtraction of the integer inputs, which is no problem, and multiplication by
 * fractional constants, which is a problem to do in integer arithmetic. We
 * multiply all the constants by CONST_SCALE and convert them to integer constants
 * (thus retaining CONST_BITS bits of precision in the constants). After doing a
 * multiplication we have to divide the product by CONST_SCALE, with proper
 * rounding, to produce the correct output. This division can be done cheaply as a
 * right shift of CONST_BITS bits. We postpone shifting as long as possible so
 * that partial sums can be added together with full fractional precision. The
 * outputs of the first pass are scaled up by PASS1_BITS bits so that they are
 * represented to better-than-integral precision. These outputs require
 * BITS_IN_JSAMPLE + PASS1_BITS + 3 bits; this fits in a 16-bit word with the
 * recommended scaling. (To scale up 12-bit sample data further, an intermediate
 * INT32 array would be needed.) To avoid overflow of the 32-bit intermediate
 * results in pass 2, we must have BITS_IN_JSAMPLE + CONST_BITS + PASS1_BITS <=
 * 26. Error analysis shows that the values given below are the most effective.
 */
#if BITS_IN_JSAMPLE == 8
#define CONST_BITS	13
#define PASS1_BITS	2
#else
#define CONST_BITS	13
#define PASS1_BITS	1						/* lose a little precision to avoid overflow */
#endif

/*
 * Some C compilers fail to reduce "FIX(constant)" at compile time, thus causing a
 * lot of useless floating-point operations at run time. To get around this we use
 * the following pre-calculated constants. If you change CONST_BITS you may want
 * to add appropriate values. (With a reasonable C compiler, you can just rely on
 * the FIX() macro...)
 */
#if CONST_BITS == 13
#define FIX_0_298631336 ((INT32) 2446)		/* FIX(0.298631336) */
#define FIX_0_390180644 ((INT32) 3196)		/* FIX(0.390180644) */
#define FIX_0_541196100 ((INT32) 4433)		/* FIX(0.541196100) */
#define FIX_0_765366865 ((INT32) 6270)		/* FIX(0.765366865) */
#define FIX_0_899976223 ((INT32) 7373)		/* FIX(0.899976223) */
#define FIX_1_175875602 ((INT32) 9633)		/* FIX(1.175875602) */
#define FIX_1_501321110 ((INT32) 12299)		/* FIX(1.501321110) */
#define FIX_1_847759065 ((INT32) 15137)		/* FIX(1.847759065) */
#define FIX_1_961570560 ((INT32) 16069)		/* FIX(1.961570560) */
#define FIX_2_053119869 ((INT32) 16819)		/* FIX(2.053119869) */
#define FIX_2_562915447 ((INT32) 20995)		/* FIX(2.562915447) */
#define FIX_3_072711026 ((INT32) 25172)		/* FIX(3.072711026) */
#else
#define FIX_0_298631336 FIX(0.298631336)
#define FIX_0_390180644 FIX(0.390180644)
#define FIX_0_541196100 FIX(0.541196100)
#define FIX_0_765366865 FIX(0.765366865)
#define FIX_0_899976223 FIX(0.899976223)
#define FIX_1_175875602 FIX(1.175875602)
#define FIX_1_501321110 FIX(1.501321110)
#define FIX_1_847759065 FIX(1.847759065)
#define FIX_1_961570560 FIX(1.961570560)
#define FIX_2_053119869 FIX(2.053119869)
#define FIX_2_562915447 FIX(2.562915447)
#define FIX_3_072711026 FIX(3.072711026)
#endif

/*
 * Multiply an INT32 variable by an INT32 constant to yield an INT32 result. For
 * 8-bit samples with the recommended scaling, all the variable and constant
 * values involved are no more than 16 bits wide, so a 16x16->32 bit multiply can
 * be used instead of a full 32x32 multiply. For 12-bit samples, a full 32-bit
 * multiplication will be needed.
 */
#if BITS_IN_JSAMPLE == 8
#define MULTIPLY(var, const)	MULTIPLY16C16(var, const)
#else
#define MULTIPLY(var, const)	((var) * (const))
#endif

/*
 =======================================================================================================================
    Dequantize a coefficient by multiplying it by the multiplier-table entry; produce an int result. In this module,
    both inputs and result are 16 bits or less, so either int or short multiply will work.
 =======================================================================================================================
 */
#define DEQUANTIZE(coef, quantval)	(((ISLOW_MULT_TYPE) (coef)) * (quantval))

/* Perform dequantization and inverse DCT on one block of coefficients. */
#if defined(PSX2_TARGET) && !defined(_DEBUG)
void fct11(int *pi_tmp0, int *pi_tmp1);
/* special for !!**?�!! de code warrior */
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Jade_jpeg_idct_islow
(
	j_decompress_ptr	_pst_cinfo,
	Jade_jpeg_component_info *_pst_compptr,
	short				*_ps_coef_block,
	unsigned char		**_ppuc_output_buf,
	unsigned int		_ui_output_col
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i_tmp0, i_tmp1, i_tmp2, i_tmp3;
	int				i_tmp10, i_tmp11, i_tmp12, i_tmp13;
	int				i_z1, i_z2, i_z3, i_z4, i_z5;
	short			*ps_inptr;
	int				*pi_quantptr;
	int				*pi_wsptr;
	unsigned char	*puc_outptr;
	unsigned char	*puc_range_limit;
	int				i_ctr;
	int				ai_workspace[64];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	puc_range_limit = ((_pst_cinfo)->sample_range_limit + 128);

	ps_inptr = _ps_coef_block;
	pi_quantptr = (int *) _pst_compptr->dct_table;
	pi_wsptr = ai_workspace;
	for(i_ctr = 8; i_ctr > 0; i_ctr--)
	{
		if
		(
			(
				ps_inptr[8 * 1] |
				ps_inptr[8 * 2] |
				ps_inptr[8 * 3] |
				ps_inptr[8 * 4] |
				ps_inptr[8 * 5] |
				ps_inptr[8 * 6] |
				ps_inptr[8 * 7]
			) == 0
		)
		{
			/*~~~~~~~~*/
			int i_dcval;
			/*~~~~~~~~*/

			i_dcval = (((int) (ps_inptr[8 * 0])) * (pi_quantptr[8 * 0])) << 2;

			pi_wsptr[8 * 0] = i_dcval;
			pi_wsptr[8 * 1] = i_dcval;
			pi_wsptr[8 * 2] = i_dcval;
			pi_wsptr[8 * 3] = i_dcval;
			pi_wsptr[8 * 4] = i_dcval;
			pi_wsptr[8 * 5] = i_dcval;
			pi_wsptr[8 * 6] = i_dcval;
			pi_wsptr[8 * 7] = i_dcval;
			ps_inptr++;
			pi_quantptr++;
			pi_wsptr++;
		}
		else
		{
			i_z2 = (((int) (ps_inptr[8 * 2])) * (pi_quantptr[8 * 2]));
			i_z3 = (((int) (ps_inptr[8 * 6])) * (pi_quantptr[8 * 6]));
			i_z1 = ((i_z2 + i_z3) * (((int) 4433)));
			i_tmp2 = i_z1 + ((i_z3) * (-((int) 15137)));
			i_tmp3 = i_z1 + ((i_z2) * (((int) 6270)));

			fct11(&i_tmp0, &i_tmp1);

			i_z2 = (((int) (ps_inptr[8 * 0])) * (pi_quantptr[8 * 0]));
			i_z3 = (((int) (ps_inptr[8 * 4])) * (pi_quantptr[8 * 4]));
			i_tmp0 = (i_z2 + i_z3) << 13;
			i_tmp1 = (i_z2 - i_z3) << 13;

			i_tmp10 = i_tmp0 + i_tmp3;
			i_tmp13 = i_tmp0 - i_tmp3;
			i_tmp11 = i_tmp1 + i_tmp2;
			i_tmp12 = i_tmp1 - i_tmp2;
			i_tmp0 = (((int) (ps_inptr[8 * 7])) * (pi_quantptr[8 * 7]));
			i_tmp1 = (((int) (ps_inptr[8 * 5])) * (pi_quantptr[8 * 5]));
			i_tmp2 = (((int) (ps_inptr[8 * 3])) * (pi_quantptr[8 * 3]));
			i_tmp3 = (((int) (ps_inptr[8 * 1])) * (pi_quantptr[8 * 1]));
			i_z1 = i_tmp0 + i_tmp3;
			i_z2 = i_tmp1 + i_tmp2;
			i_z3 = i_tmp0 + i_tmp2;
			i_z4 = i_tmp1 + i_tmp3;
			i_z5 = ((i_z3 + i_z4) * (((int) 9633)));
			i_tmp0 = ((i_tmp0) * (((int) 2446)));
			i_tmp1 = ((i_tmp1) * (((int) 16819)));
			i_tmp2 = ((i_tmp2) * (((int) 25172)));
			i_tmp3 = ((i_tmp3) * (((int) 12299)));
			i_z1 = ((i_z1) * (-((int) 7373)));
			i_z2 = ((i_z2) * (-((int) 20995)));
			i_z3 = ((i_z3) * (-((int) 16069)));
			i_z4 = ((i_z4) * (-((int) 3196)));
			i_z3 += i_z5;
			i_z4 += i_z5;
			i_tmp0 += i_z1 + i_z3;
			i_tmp1 += i_z2 + i_z4;
			i_tmp2 += i_z2 + i_z3;
			i_tmp3 += i_z1 + i_z4;

			pi_wsptr[8 * 0] = (int) (((i_tmp10 + i_tmp3) + (((int) 1) << 10)) >> 11);
			pi_wsptr[8 * 7] = (int) (((i_tmp10 - i_tmp3) + (((int) 1) << 10)) >> 11);
			pi_wsptr[8 * 1] = (int) (((i_tmp11 + i_tmp2) + (((int) 1) << 10)) >> 11);
			pi_wsptr[8 * 6] = (int) (((i_tmp11 - i_tmp2) + (((int) 1) << 10)) >> 11);
			pi_wsptr[8 * 2] = (int) (((i_tmp12 + i_tmp1) + (((int) 1) << 10)) >> 11);
			pi_wsptr[8 * 5] = (int) (((i_tmp12 - i_tmp1) + (((int) 1) << 10)) >> 11);
			pi_wsptr[8 * 3] = (int) (((i_tmp13 + i_tmp0) + (((int) 1) << 10)) >> 11);
			pi_wsptr[8 * 4] = (int) (((i_tmp13 - i_tmp0) + (((int) 1) << 10)) >> 11);

			ps_inptr++;
			pi_quantptr++;
			pi_wsptr++;
		}
	}

	pi_wsptr = ai_workspace;
	for(i_ctr = 0; i_ctr < 8; i_ctr++)
	{
		puc_outptr = _ppuc_output_buf[i_ctr] + _ui_output_col;
		if((pi_wsptr[1] | pi_wsptr[2] | pi_wsptr[3] | pi_wsptr[4] | pi_wsptr[5] | pi_wsptr[6] | pi_wsptr[7]) == 0)
		{
			/*~~~~~~~~~~~~~~~~~~~~~*/
			unsigned char	uc_dcval;
			/*~~~~~~~~~~~~~~~~~~~~~*/

			uc_dcval = puc_range_limit[(int) ((((int) pi_wsptr[0]) + (((int) 1) << ((2 + 3) - 1))) >> (2 + 3)) & 1023];

			puc_outptr[0] = uc_dcval;
			puc_outptr[1] = uc_dcval;
			puc_outptr[2] = uc_dcval;
			puc_outptr[3] = uc_dcval;
			puc_outptr[4] = uc_dcval;
			puc_outptr[5] = uc_dcval;
			puc_outptr[6] = uc_dcval;
			puc_outptr[7] = uc_dcval;
			pi_wsptr += 8;
		}
		else
		{
			i_z2 = (int) pi_wsptr[2];
			i_z3 = (int) pi_wsptr[6];
			i_z1 = ((i_z2 + i_z3) * (((int) 4433)));
			i_tmp2 = i_z1 + ((i_z3) * (-((int) 15137)));
			i_tmp3 = i_z1 + ((i_z2) * (((int) 6270)));
			fct11(&i_tmp0, &i_tmp1);
			i_tmp0 = ((int) pi_wsptr[0] + (int) pi_wsptr[4]) << 13;
			i_tmp1 = ((int) pi_wsptr[0] - (int) pi_wsptr[4]) << 13;
			i_tmp10 = i_tmp0 + i_tmp3;
			i_tmp13 = i_tmp0 - i_tmp3;
			i_tmp11 = i_tmp1 + i_tmp2;
			i_tmp12 = i_tmp1 - i_tmp2;
			i_tmp0 = (int) pi_wsptr[7];
			i_tmp1 = (int) pi_wsptr[5];
			i_tmp2 = (int) pi_wsptr[3];
			i_tmp3 = (int) pi_wsptr[1];
			i_z1 = i_tmp0 + i_tmp3;
			i_z2 = i_tmp1 + i_tmp2;
			i_z3 = i_tmp0 + i_tmp2;
			i_z4 = i_tmp1 + i_tmp3;
			i_z5 = ((i_z3 + i_z4) * (((int) 9633)));
			i_tmp0 = ((i_tmp0) * (((int) 2446)));
			i_tmp1 = ((i_tmp1) * (((int) 16819)));
			i_tmp2 = ((i_tmp2) * (((int) 25172)));
			i_tmp3 = ((i_tmp3) * (((int) 12299)));
			i_z1 = ((i_z1) * (-((int) 7373)));
			i_z2 = ((i_z2) * (-((int) 20995)));
			i_z3 = ((i_z3) * (-((int) 16069)));
			i_z4 = ((i_z4) * (-((int) 3196)));
			i_z3 += i_z5;
			i_z4 += i_z5;
			i_tmp0 += i_z1 + i_z3;
			i_tmp1 += i_z2 + i_z4;
			i_tmp2 += i_z2 + i_z3;
			i_tmp3 += i_z1 + i_z4;
			puc_outptr[0] = puc_range_limit[(int) (((i_tmp10 + i_tmp3) + (((int) 1) << 17)) >> 18) & 1023];
			puc_outptr[7] = puc_range_limit[(int) (((i_tmp10 - i_tmp3) + (((int) 1) << 17)) >> 18) & 1023];
			puc_outptr[1] = puc_range_limit[(int) (((i_tmp11 + i_tmp2) + (((int) 1) << 17)) >> 18) & 1023];
			puc_outptr[6] = puc_range_limit[(int) (((i_tmp11 - i_tmp2) + (((int) 1) << 17)) >> 18) & 1023];
			puc_outptr[2] = puc_range_limit[(int) (((i_tmp12 + i_tmp1) + (((int) 1) << 17)) >> 18) & 1023];
			puc_outptr[5] = puc_range_limit[(int) (((i_tmp12 - i_tmp1) + (((int) 1) << 17)) >> 18) & 1023];
			puc_outptr[3] = puc_range_limit[(int) (((i_tmp13 + i_tmp0) + (((int) 1) << 17)) >> 18) & 1023];
			puc_outptr[4] = puc_range_limit[(int) (((i_tmp13 - i_tmp0) + (((int) 1) << 17)) >> 18) & 1023];

			pi_wsptr += 8;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void fct11(int *pi_tmp0, int *pi_tmp1)
{
}

#else

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GLOBAL void Jade_jpeg_idct_islow
(
	j_decompress_ptr	cinfo,
	Jade_jpeg_component_info *compptr,
	JCOEFPTR			coef_block,
	JSAMPARRAY			output_buf,
	JDIMENSION			output_col
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	INT32			tmp0, tmp1, tmp2, tmp3;
	INT32			tmp10, tmp11, tmp12, tmp13;
	INT32			z1, z2, z3, z4, z5;
	JCOEFPTR		inptr;
	ISLOW_MULT_TYPE *quantptr;
	int				*wsptr;
	JSAMPROW		outptr;
	JSAMPLE			*range_limit;
	int				ctr;
	int				workspace[DCTSIZE2];			/* buffers data between passes */
	SHIFT_TEMPS

	/*
	 * Pass 1: process columns from input, store into work array. �
	 * Note results are scaled up by sqrt(8) compared to a true IDCT; �
	 * furthermore, we scale the results by 2**PASS1_BITS.
	 */
	inptr;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	range_limit = IDCT_range_limit(cinfo);
	inptr = coef_block;

	quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
	wsptr = workspace;
	for(ctr = DCTSIZE; ctr > 0; ctr--)
	{
		/*
		 * Due to quantization, we will usually find that many of the input coefficients
		 * are zero, especially the AC terms. We can exploit this by short-circuiting the
		 * IDCT calculation for any column in which all the AC terms are zero. In that
		 * case each output is equal to the DC coefficient (with scale factor as needed).
		 * With typical images and quantization tables, half or more of the column DCT
		 * calculations can be simplified this way.
		 */
		if
		(
			(
				inptr[DCTSIZE * 1] |
				inptr[DCTSIZE * 2] |
				inptr[DCTSIZE * 3] |
				inptr[DCTSIZE * 4] |
				inptr[DCTSIZE * 5] |
				inptr[DCTSIZE * 6] |
				inptr[DCTSIZE * 7]
			) == 0
		)
		{
			/*~~~~~~*/

			/* AC terms all zero */
			int dcval;
			/*~~~~~~*/

			dcval = DEQUANTIZE(inptr[DCTSIZE * 0], quantptr[DCTSIZE * 0]) << PASS1_BITS;

			wsptr[DCTSIZE * 0] = dcval;
			wsptr[DCTSIZE * 1] = dcval;
			wsptr[DCTSIZE * 2] = dcval;
			wsptr[DCTSIZE * 3] = dcval;
			wsptr[DCTSIZE * 4] = dcval;
			wsptr[DCTSIZE * 5] = dcval;
			wsptr[DCTSIZE * 6] = dcval;
			wsptr[DCTSIZE * 7] = dcval;

			inptr++;								/* advance pointers to next column */
			quantptr++;
			wsptr++;
			continue;
		}

		/*
		 * Even part: reverse the even part of the forward DCT. �
		 * The rotator is sqrt(2)*c(-6).
		 */
		z2 = DEQUANTIZE(inptr[DCTSIZE * 2], quantptr[DCTSIZE * 2]);
		z3 = DEQUANTIZE(inptr[DCTSIZE * 6], quantptr[DCTSIZE * 6]);

		z1 = MULTIPLY(z2 + z3, FIX_0_541196100);
		tmp2 = z1 + MULTIPLY(z3, -FIX_1_847759065);
		tmp3 = z1 + MULTIPLY(z2, FIX_0_765366865);

		z2 = DEQUANTIZE(inptr[DCTSIZE * 0], quantptr[DCTSIZE * 0]);
		z3 = DEQUANTIZE(inptr[DCTSIZE * 4], quantptr[DCTSIZE * 4]);

		tmp0 = (z2 + z3) << CONST_BITS;
		tmp1 = (z2 - z3) << CONST_BITS;

		tmp10 = tmp0 + tmp3;
		tmp13 = tmp0 - tmp3;
		tmp11 = tmp1 + tmp2;
		tmp12 = tmp1 - tmp2;

		/*
		 * Odd part per figure 8; the matrix is unitary and hence its transpose is its
		 * inverse. i0..i3 are y7,y5,y3,y1 respectively.
		 */
		tmp0 = DEQUANTIZE(inptr[DCTSIZE * 7], quantptr[DCTSIZE * 7]);
		tmp1 = DEQUANTIZE(inptr[DCTSIZE * 5], quantptr[DCTSIZE * 5]);
		tmp2 = DEQUANTIZE(inptr[DCTSIZE * 3], quantptr[DCTSIZE * 3]);
		tmp3 = DEQUANTIZE(inptr[DCTSIZE * 1], quantptr[DCTSIZE * 1]);

		z1 = tmp0 + tmp3;
		z2 = tmp1 + tmp2;
		z3 = tmp0 + tmp2;
		z4 = tmp1 + tmp3;
		z5 = MULTIPLY(z3 + z4, FIX_1_175875602);	/* sqrt(2) * c3 */

		tmp0 = MULTIPLY(tmp0, FIX_0_298631336);		/* sqrt(2) * (-c1+c3+c5-c7) */
		tmp1 = MULTIPLY(tmp1, FIX_2_053119869);		/* sqrt(2) * ( c1+c3-c5+c7) */
		tmp2 = MULTIPLY(tmp2, FIX_3_072711026);		/* sqrt(2) * ( c1+c3+c5-c7) */
		tmp3 = MULTIPLY(tmp3, FIX_1_501321110);		/* sqrt(2) * ( c1+c3-c5-c7) */
		z1 = MULTIPLY(z1, -FIX_0_899976223);		/* sqrt(2) * (c7-c3) */
		z2 = MULTIPLY(z2, -FIX_2_562915447);		/* sqrt(2) * (-c1-c3) */
		z3 = MULTIPLY(z3, -FIX_1_961570560);		/* sqrt(2) * (-c3-c5) */
		z4 = MULTIPLY(z4, -FIX_0_390180644);		/* sqrt(2) * (c5-c3) */

		z3 += z5;
		z4 += z5;

		tmp0 += z1 + z3;
		tmp1 += z2 + z4;
		tmp2 += z2 + z3;
		tmp3 += z1 + z4;

		/* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */
		wsptr[DCTSIZE * 0] = (int) DESCALE(tmp10 + tmp3, CONST_BITS - PASS1_BITS);
		wsptr[DCTSIZE * 7] = (int) DESCALE(tmp10 - tmp3, CONST_BITS - PASS1_BITS);
		wsptr[DCTSIZE * 1] = (int) DESCALE(tmp11 + tmp2, CONST_BITS - PASS1_BITS);
		wsptr[DCTSIZE * 6] = (int) DESCALE(tmp11 - tmp2, CONST_BITS - PASS1_BITS);
		wsptr[DCTSIZE * 2] = (int) DESCALE(tmp12 + tmp1, CONST_BITS - PASS1_BITS);
		wsptr[DCTSIZE * 5] = (int) DESCALE(tmp12 - tmp1, CONST_BITS - PASS1_BITS);
		wsptr[DCTSIZE * 3] = (int) DESCALE(tmp13 + tmp0, CONST_BITS - PASS1_BITS);
		wsptr[DCTSIZE * 4] = (int) DESCALE(tmp13 - tmp0, CONST_BITS - PASS1_BITS);

		inptr++;									/* advance pointers to next column */
		quantptr++;
		wsptr++;
	}

	/*
	 * Pass 2: process rows from work array, store into output array. �
	 * Note that we must descale the results by a factor of 8 == 2**3, �
	 * and also undo the PASS1_BITS scaling.
	 */
	wsptr = workspace;
	for(ctr = 0; ctr < DCTSIZE; ctr++)
	{
		outptr = output_buf[ctr] + output_col;

		/*
		 * Rows of zeroes can be exploited in the same way as we did with columns.
		 * However, the column calculation has created many nonzero AC terms, so the
		 * simplification applies less often (typically 5% to 10% of the time). On
		 * machines with very fast multiplication, it's possible that the test takes more
		 * time than it's worth. In that case this section may be commented out.
		 */
#ifndef NO_ZERO_ROW_TEST
		if((wsptr[1] | wsptr[2] | wsptr[3] | wsptr[4] | wsptr[5] | wsptr[6] | wsptr[7]) == 0)
		{
			/*~~~~~~~~~~*/

			/* AC terms all zero */
			JSAMPLE dcval;
			/*~~~~~~~~~~*/

			dcval = range_limit[(int) DESCALE((INT32) wsptr[0], PASS1_BITS + 3) & RANGE_MASK];

			outptr[0] = dcval;
			outptr[1] = dcval;
			outptr[2] = dcval;
			outptr[3] = dcval;
			outptr[4] = dcval;
			outptr[5] = dcval;
			outptr[6] = dcval;
			outptr[7] = dcval;

			wsptr += DCTSIZE;						/* advance pointer to next row */
			continue;
		}

#endif
		/*
		 * Even part: reverse the even part of the forward DCT. �
		 * The rotator is sqrt(2)*c(-6).
		 */
		z2 = (INT32) wsptr[2];
		z3 = (INT32) wsptr[6];

		z1 = MULTIPLY(z2 + z3, FIX_0_541196100);
		tmp2 = z1 + MULTIPLY(z3, -FIX_1_847759065);
		tmp3 = z1 + MULTIPLY(z2, FIX_0_765366865);

		tmp0 = ((INT32) wsptr[0] + (INT32) wsptr[4]) << CONST_BITS;
		tmp1 = ((INT32) wsptr[0] - (INT32) wsptr[4]) << CONST_BITS;

		tmp10 = tmp0 + tmp3;
		tmp13 = tmp0 - tmp3;
		tmp11 = tmp1 + tmp2;
		tmp12 = tmp1 - tmp2;

		/*
		 * Odd part per figure 8; the matrix is unitary and hence its transpose is its
		 * inverse. i0..i3 are y7,y5,y3,y1 respectively.
		 */
		tmp0 = (INT32) wsptr[7];
		tmp1 = (INT32) wsptr[5];
		tmp2 = (INT32) wsptr[3];
		tmp3 = (INT32) wsptr[1];

		z1 = tmp0 + tmp3;
		z2 = tmp1 + tmp2;
		z3 = tmp0 + tmp2;
		z4 = tmp1 + tmp3;
		z5 = MULTIPLY(z3 + z4, FIX_1_175875602);	/* sqrt(2) * c3 */

		tmp0 = MULTIPLY(tmp0, FIX_0_298631336);		/* sqrt(2) * (-c1+c3+c5-c7) */
		tmp1 = MULTIPLY(tmp1, FIX_2_053119869);		/* sqrt(2) * ( c1+c3-c5+c7) */
		tmp2 = MULTIPLY(tmp2, FIX_3_072711026);		/* sqrt(2) * ( c1+c3+c5-c7) */
		tmp3 = MULTIPLY(tmp3, FIX_1_501321110);		/* sqrt(2) * ( c1+c3-c5-c7) */
		z1 = MULTIPLY(z1, -FIX_0_899976223);		/* sqrt(2) * (c7-c3) */
		z2 = MULTIPLY(z2, -FIX_2_562915447);		/* sqrt(2) * (-c1-c3) */
		z3 = MULTIPLY(z3, -FIX_1_961570560);		/* sqrt(2) * (-c3-c5) */
		z4 = MULTIPLY(z4, -FIX_0_390180644);		/* sqrt(2) * (c5-c3) */

		z3 += z5;
		z4 += z5;

		tmp0 += z1 + z3;
		tmp1 += z2 + z4;
		tmp2 += z2 + z3;
		tmp3 += z1 + z4;

		/* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */
		outptr[0] = range_limit[(int) DESCALE(tmp10 + tmp3, CONST_BITS + PASS1_BITS + 3) & RANGE_MASK];
		outptr[7] = range_limit[(int) DESCALE(tmp10 - tmp3, CONST_BITS + PASS1_BITS + 3) & RANGE_MASK];
		outptr[1] = range_limit[(int) DESCALE(tmp11 + tmp2, CONST_BITS + PASS1_BITS + 3) & RANGE_MASK];
		outptr[6] = range_limit[(int) DESCALE(tmp11 - tmp2, CONST_BITS + PASS1_BITS + 3) & RANGE_MASK];
		outptr[2] = range_limit[(int) DESCALE(tmp12 + tmp1, CONST_BITS + PASS1_BITS + 3) & RANGE_MASK];
		outptr[5] = range_limit[(int) DESCALE(tmp12 - tmp1, CONST_BITS + PASS1_BITS + 3) & RANGE_MASK];
		outptr[3] = range_limit[(int) DESCALE(tmp13 + tmp0, CONST_BITS + PASS1_BITS + 3) & RANGE_MASK];
		outptr[4] = range_limit[(int) DESCALE(tmp13 - tmp0, CONST_BITS + PASS1_BITS + 3) & RANGE_MASK];

		wsptr += DCTSIZE;							/* advance pointer to next row */
	}
}

#endif
#endif /* DCT_ISLOW_SUPPORTED */
