/*$T SOFTcolor.c GC! 1.081 05/23/01 18:50:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "SOFT/SOFTlinear.h"
#include "SOFT/SOFTcolor.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COLOR_ul_Blend(ULONG C1, ULONG C2, float f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	RetValue, Interpoler;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(f >= 0.98f) return C2;
	if(f <= 0.02f) return C1;
	*((float *) &Interpoler) = f + 32768.0f + 16384.0f;

	RetValue = (Interpoler & 128) ? (C2 & 0xfefefefe) >> 1 : (C1 & 0xfefefefe) >> 1;
	RetValue += (Interpoler & 64) ? (C2 & 0xfcfcfcfc) >> 2 : (C1 & 0xfcfcfcfc) >> 2;
	RetValue += (Interpoler & 32) ? (C2 & 0xf8f8f8f8) >> 3 : (C1 & 0xf8f8f8f8) >> 3;
	RetValue += (Interpoler & 16) ? (C2 & 0xf0f0f0f0) >> 4 : (C1 & 0xf0f0f0f0) >> 4;
	RetValue += (Interpoler & 8) ? (C2 & 0xe0e0e0e0) >> 5 : (C1 & 0xe0e0e0e0) >> 5;
	RetValue += (Interpoler & 4) ? (C2 & 0xc0c0c0c0) >> 6 : (C1 & 0xc0c0c0c0) >> 6;
	RetValue += (Interpoler & 2) ? (C2 & 0x80808080) >> 7 : (C1 & 0x80808080) >> 7;

	return RetValue;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COLOR_ul_Mul(ULONG C, float f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	RetValue, Interpoler;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(f >= 0.98f) return C;
	if(f <= 0.02f) return 0;

	*((float *) &Interpoler) = f + 32768.0f + 16384.0f;

	/* Interpoler = COLOR_pul_Table[Interpoler & 0xff]; */
	RetValue = 0;
	if(Interpoler & 128) RetValue += (C & 0xfefefefe) >> 1;
	if(Interpoler & 64) RetValue += (C & 0xfcfcfcfc) >> 2;
	if(Interpoler & 32) RetValue += (C & 0xf8f8f8f8) >> 3;
	if(Interpoler & 16) RetValue += (C & 0xf0f0f0f0) >> 4;
	if(Interpoler & 8) RetValue += (C & 0xe0e0e0e0) >> 5;
	return RetValue;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COLOR_ul_Blend3(ULONG ul_C0, ULONG ul_C1, ULONG ul_C2, float f0, float f1, float f2)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int		i, decal;
	float	c, c0, c1, c2;
	ULONG	Color;
	/*~~~~~~~~~~~~~~~~~~*/

	decal = 0;
	Color = 0;
	for(i = 0; i < 4; i++, decal += 8)
	{
		c0 = (float) (ul_C0 >> decal & 0xFF) / 255.0f;
		c1 = (float) (ul_C1 >> decal & 0xFF) / 255.0f;
		c2 = (float) (ul_C2 >> decal & 0xFF) / 255.0f;

		c = c0 * f0 + c1 * f1 + c2 * f2;
		if(c >= 1.0f)
			Color |= 0xFF << decal;
		else
			Color |= ((int) (c * 255.0f)) << decal;
	}

	return Color;
}

/*
 =======================================================================================================================
 =======================================================================================================================
	EDITOR FUNCTIONS
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS

typedef struct COLOR_RLIList_Node_
{
	ULONG *pul_RLI;
	ULONG *dul_Save;
} COLOR_RLIList_Node;

int					COLOR_RLIList_Nb = 0;
int					COLOR_RLIList_Max = 0;
COLOR_RLIList_Node	*COLOR_RLIList = NULL;
COLOR_RLIList_Node	*COLOR_RLINext = NULL;


double	COLOR_gd_H;
double	COLOR_gd_S;
double	COLOR_gd_L;
double	COLOR_gd_a;
double	COLOR_gd_r;
double	COLOR_gd_g;
double	COLOR_gd_b;
double	COLOR_gad_RLut[ 256 ];
double	COLOR_gad_GLut[ 256 ];
double	COLOR_gad_BLut[ 256 ];
double	COLOR_gad_RLum[ 256 ];
double	COLOR_gad_GLum[ 256 ];
double	COLOR_gad_BLum[ 256 ];
UCHAR	COLOR_BC_LUT[ 256 ];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COLOR_RLIAdjust_ListDestroy()
{
	int i;
	
	if( COLOR_RLIList )
	{
		for (i = 0; i < COLOR_RLIList_Nb ; i++)
		{
			if ( COLOR_RLIList[ i ].dul_Save )
				MEM_Free( COLOR_RLIList[ i ].dul_Save );
		}
		MEM_Free( COLOR_RLIList );
	}
	COLOR_RLIList = NULL;
	COLOR_RLIList_Nb = 0;
	COLOR_RLIList_Max = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
COLOR_RLIList_Node *COLOR_RLIAdjust_ListGet( ULONG *_pul_RLI )
{
	int i;
	
	if ( COLOR_RLINext && (COLOR_RLINext->pul_RLI == _pul_RLI ) )
		return COLOR_RLINext++;
	
	for (i = 0; i < COLOR_RLIList_Nb ; i++)
	{
		if ( COLOR_RLIList[ i ].pul_RLI == _pul_RLI )
		{
			COLOR_RLINext = (i+1 == COLOR_RLIList_Nb) ? NULL : &COLOR_RLIList[ i + 1];
			return &COLOR_RLIList[ i ];
		}
	}
	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
COLOR_RLIList_Node *COLOR_RLIAdjust_ListAdd( ULONG *_pul_RLI )
{
	if ( !_pul_RLI ) return NULL;
	
	COLOR_RLINext = NULL;
	
	if (COLOR_RLIList_Nb == COLOR_RLIList_Max)
	{
		COLOR_RLIList_Max += 1000;
		if ( !COLOR_RLIList_Nb )
			COLOR_RLIList = (COLOR_RLIList_Node* )MEM_p_Alloc( COLOR_RLIList_Max * sizeof( COLOR_RLIList_Node ) );
		else
			COLOR_RLIList = (COLOR_RLIList_Node* )MEM_p_Realloc( COLOR_RLIList, COLOR_RLIList_Max * sizeof( COLOR_RLIList_Node ) );
	}
	
	COLOR_RLIList[ COLOR_RLIList_Nb ].pul_RLI = _pul_RLI;
	COLOR_RLIList[ COLOR_RLIList_Nb ].dul_Save = (ULONG *)MEM_p_Alloc( (_pul_RLI[0] + 1) * 4 );
	L_memcpy( COLOR_RLIList[ COLOR_RLIList_Nb ].dul_Save, _pul_RLI, (_pul_RLI[0] + 1) * 4 );
	
	return &COLOR_RLIList[ COLOR_RLIList_Nb++ ];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COLOR_RLIAdjust_Free( void )
{
	COLOR_RLIAdjust_ListDestroy();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COLOR_RLIAdjust_Restore( void )
{
	int i;

	for (i = 0; i < COLOR_RLIList_Nb ; i++)
	{
		L_memcpy
		( 
			COLOR_RLIList[ i ].pul_RLI, 
			COLOR_RLIList[ i ].dul_Save, 
			(COLOR_RLIList[ i ].pul_RLI[0] + 1) * 4 
		);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COLOR_RLIAdjust_SetAlphaParam( double a )
{
	COLOR_gd_a = a;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void COLOR_RLIAdjust_AlphaAddSerial( ULONG *_pul_Tgt, ULONG *_pul_Src, int n )
{
	ULONG	RGB;
	double	a;
		
	while ( n-- )
	{
		RGB = *_pul_Src++;
		
		// Alpha in float
		a = ((RGB & 0xff000000) >> 24) * 1.0 / 255.0;

		a += COLOR_gd_a;
		if (a < 0) a = 0; 
		else if (a > 1) a = 1;
	
		RGB &= 0x00FFFFFF;
		RGB |= ((ULONG) (a * 255.0f)) << 24;
		
		*_pul_Tgt++ = RGB;
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COLOR_RLIAdjust_SetHLSParams( double h, double l, double s )
{
	COLOR_gd_H = h;
	COLOR_gd_L = l;
	COLOR_gd_S = s;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COLOR_RLIAdjust_HLSSerial( ULONG *_pul_Tgt, ULONG *_pul_Src, int n )
{
	ULONG	RGB;
	double	r, g, b, a, h, l, s;
		
	while ( n-- )
	{
		RGB = *_pul_Src++;
		
		// RGB in float
		r = (RGB & 0xff) * 1.0 / 255.0;
		g = ((RGB & 0xff00) >> 8) * 1.0 / 255.0;
		b = ((RGB & 0xff0000) >> 16) * 1.0 / 255.0;
	
		// convert r,g,b to h,l,s
		RGB_to_HLS( r, g, b, &h, &l, &s );
		h *= 360.0;
	
		// transform h,l,s
		l += COLOR_gd_L;
		if (l < 0) l = 0;
		if (l > 1) l = 1;
	
		s += COLOR_gd_S;
		if (s < 0) s = 0;
		if (s > 1) s = 1;
		
		h += COLOR_gd_H;
		if ( h < 0 ) h += 360;
		
		// convert h, l, s to r, g, b
		h /= 360.0;
		HLS_to_RGB( h, l, s, &r, &g, &b );
	
		a = ((RGB & 0xff000000) >> 24) * 1.0 / 255.0;
		a += COLOR_gd_a;
		if (a < 0) a = 0; 
		else if (a > 1) a = 1;
		
		// r,g,b, 2 RGB
		RGB &= 0x00000000;
		RGB |= ((ULONG) (a * 255.0f)) << 24;
		RGB |= ((ULONG) (b * 255.0f)) << 16;
		RGB |= ((ULONG) (g * 255.0f)) << 8;
		RGB |= ((ULONG) (r * 255.0f));
		
		*_pul_Tgt++ = RGB;
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void COLOR_RLIAdjust_HLS( ULONG *_pul_RLI )
{
	COLOR_RLIList_Node *pst_RLI;
	
	if ( !_pul_RLI ) return;
	pst_RLI = COLOR_RLIAdjust_ListGet( _pul_RLI );
	if (!pst_RLI) pst_RLI = COLOR_RLIAdjust_ListAdd( _pul_RLI );
	
	COLOR_RLIAdjust_HLSSerial( pst_RLI->pul_RLI + 1, pst_RLI->dul_Save + 1, pst_RLI->pul_RLI[ 0 ] );
	//COLOR_RLIAdjust_AlphaAddSerial( pst_RLI->pul_RLI + 1, pst_RLI->dul_Save + 1, pst_RLI->pul_RLI[ 0 ] );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COLOR_RLIAdjust_HLSColorizeSerial( ULONG *_pul_Tgt, ULONG *_pul_Src, int n )
{
	ULONG	RGB;
	int		R, G, B, i;
	double	r, g, b, a, l, lum;

		
	while ( n-- )
	{
		RGB = *_pul_Src++;
		
		R = RGB & 0xFF;
		G = (RGB & 0xFF00) >> 8;
		B = (RGB & 0xFF0000) >> 16;
		
		lum = COLOR_gad_RLum[ R ] + COLOR_gad_GLum[ G ] + COLOR_gad_BLum[ B ];
		l = COLOR_gd_L * 100;
		
		if ( l > 0)
		{
			lum = (lum * 255.0) * (100.0 - l) / 100.0;
			lum += 255 - (100.0 - l) * 255.0 / 100.0;
		}
		else if (l < 0)
		{
			lum = (lum *255.0) * (l + 100.0) / 100.0;
		}
		
		i = ( int ) lum;
		r = COLOR_gad_RLut[ i ];
		g = COLOR_gad_GLut[ i ];
		b = COLOR_gad_BLut[ i ];
		
		a = ((RGB & 0xff000000) >> 24) * 1.0 / 255.0;
		a += COLOR_gd_a;
		if (a < 0) a = 0; 
		else if (a > 1) a = 1;
		
		RGB &= 0x00000000;
		RGB |= ((ULONG) (a * 255.0f)) << 24;
		RGB |= ((ULONG) b) << 16;
		RGB |= ((ULONG) g) << 8;
		RGB |= ((ULONG) r);
    	
		*_pul_Tgt++ = RGB;
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void COLOR_RLIAdjust_HLSColorize( ULONG *_pul_RLI )
{
	COLOR_RLIList_Node *pst_RLI;
	
	if ( !_pul_RLI ) return;
	pst_RLI = COLOR_RLIAdjust_ListGet( _pul_RLI );
	if (!pst_RLI) pst_RLI = COLOR_RLIAdjust_ListAdd( _pul_RLI );
	
	COLOR_RLIAdjust_HLSColorizeSerial( pst_RLI->pul_RLI + 1, pst_RLI->dul_Save + 1, pst_RLI->pul_RLI[ 0 ] );
	//COLOR_RLIAdjust_AlphaAddSerial( pst_RLI->pul_RLI + 1, pst_RLI->dul_Save + 1, pst_RLI->pul_RLI[ 0 ] );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COLOR_RLIAdjust_SetHLSColorizeParams( double h, double l, double s )
{
	int		i;
	double	r, g, b;
	
	COLOR_gd_H = h;
	COLOR_gd_L = l;
	COLOR_gd_S = s;
	
	for (i = 0; i < 256; i++ )
	{
		COLOR_gad_RLum[ i ] = ((float) i) * RGB_INTENSITY_RED / 255.0;
		COLOR_gad_GLum[ i ] = ((float) i) * RGB_INTENSITY_GREEN / 255.0;
		COLOR_gad_BLum[ i ] = ((float) i) * RGB_INTENSITY_BLUE / 255.0;
	}
	
	// compute final lookup table
	h = h / 360.0;
	s = s / 100.0;
	for (i = 0; i < 256; i ++)
	{
		l = i / 255.0;
		HLS_to_RGB( h, l, s, &r, &g, &b );
		COLOR_gad_RLut[i] = i * r;
		COLOR_gad_GLut[i] = i * g;
		COLOR_gad_BLut[i] = i * b;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COLOR_RLIAdjust_SetBCParams( double contrast, double brightness )
{
	int		i, nb;
	double	value, nvalue, power;
	
	for (i = 0; i < 256; i++)
	{
		value = (double) i / 255.0;
		
		/* apply brightness */
		if ( brightness < 0.0 )
			value = value * (1.0 + brightness);
		else
			value = value + ((1.0 - value) * brightness);

		/* apply contrast */
		if ( contrast < 0.0 )
		{
			if (value > 0.5)
				nvalue = 1.0 - value;
			else
				nvalue = value;

			if (nvalue < 0.0)
				nvalue = 0.0;

			nvalue = 0.5 * pow (nvalue * 2.0 , (double) (1.0 + contrast));

			if (value > 0.5)
				value = 1.0 - nvalue;
			else
				value = nvalue;
		}
		else
		{
			if (value > 0.5)
				nvalue = 1.0 - value;
			else
				nvalue = value;

			if (nvalue < 0.0)
				nvalue = 0.0;

			power = (contrast == 1.0) ? 127 : 1.0 / (1.0 - contrast);
			nvalue = 0.5 * pow (2.0 * nvalue, power);

			if (value > 0.5)
				value = 1.0 - nvalue;
			else
				value = nvalue;
		}
		
		nb = (int) (value * 255.0);
		if (nb < 0) nb = 0;
		if (nb > 255) nb = 255;
		COLOR_BC_LUT[ i ] = (UCHAR) (nb & 0xFF);
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void COLOR_RLIAdjust_BCSerial( ULONG *_pul_Tgt, ULONG *_pul_Src, int n )
{
	ULONG	ul_RLI;
	double a;

	
	while (n--)
	{
		a = ((*_pul_Src & 0xff000000) >> 24) * 1.0 / 255.0;
		a += COLOR_gd_a;
		if (a < 0) a = 0; 
		else if (a > 1) a = 1;

		ul_RLI = 0x00000000;
		ul_RLI |= COLOR_BC_LUT[ *_pul_Src & 0xFF ];
		ul_RLI |= COLOR_BC_LUT[ (*_pul_Src >> 8) & 0xFF ] << 8;
		ul_RLI |= COLOR_BC_LUT[ (*_pul_Src >> 16) & 0xFF ] << 16;
		ul_RLI |= ((ULONG) (a * 255.0f)) << 24;
		_pul_Src++;
		*_pul_Tgt++ = ul_RLI;
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void COLOR_RLIAdjust_BC( ULONG *_pul_RLI )
{
	COLOR_RLIList_Node *pst_RLI;
	
	if ( !_pul_RLI ) return;
	pst_RLI = COLOR_RLIAdjust_ListGet( _pul_RLI );
	if (!pst_RLI) pst_RLI = COLOR_RLIAdjust_ListAdd( _pul_RLI );
	
	COLOR_RLIAdjust_BCSerial( pst_RLI->pul_RLI + 1, pst_RLI->dul_Save + 1, pst_RLI->pul_RLI[ 0 ] );
	//COLOR_RLIAdjust_AlphaAddSerial( pst_RLI->pul_RLI + 1, pst_RLI->dul_Save + 1, pst_RLI->pul_RLI[ 0 ] );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COLOR_RLIAdjust_SetRGBParams( double r, double g, double b )
{
	COLOR_gd_r = r;
	COLOR_gd_g = g;
	COLOR_gd_b = b;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void COLOR_RLIAdjust_RGBAddSerial( ULONG *_pul_Tgt, ULONG *_pul_Src, int n )
{
	ULONG	RGB;
	double	r, g, b, a;
		
	while ( n-- )
	{
		RGB = *_pul_Src++;
		
		// RGB in float
		r = (RGB & 0xff) * 1.0 / 255.0;
		g = ((RGB & 0xff00) >> 8) * 1.0 / 255.0;
		b = ((RGB & 0xff0000) >> 16) * 1.0 / 255.0;
		a = ((RGB & 0xff000000) >> 24) * 1.0 / 255.0;

		r += COLOR_gd_r;
		if (r < 0) r = 0; 
		else if (r > 1) r = 1;

		g += COLOR_gd_g;
		if (g < 0) g = 0; 
		else if (g > 1) g = 1;

		b += COLOR_gd_b;
		if (b < 0) b = 0; 
		else if (b > 1) b = 1;

		a += COLOR_gd_a;
		if (a < 0) a = 0; 
		else if (a > 1) a = 1;
	
		// r,g,b,a 2 RGB
		RGB &= 0x00000000;
		RGB |= ((ULONG) (a * 255.0f)) << 24;
		RGB |= ((ULONG) (b * 255.0f)) << 16;
		RGB |= ((ULONG) (g * 255.0f)) << 8;
		RGB |= ((ULONG) (r * 255.0f));
		
		*_pul_Tgt++ = RGB;
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void COLOR_RLIAdjust_RGBAdd( ULONG *_pul_RLI )
{
	COLOR_RLIList_Node *pst_RLI;
	
	if ( !_pul_RLI ) return;
	pst_RLI = COLOR_RLIAdjust_ListGet( _pul_RLI );
	if (!pst_RLI) pst_RLI = COLOR_RLIAdjust_ListAdd( _pul_RLI );
	
	COLOR_RLIAdjust_RGBAddSerial( pst_RLI->pul_RLI + 1, pst_RLI->dul_Save + 1, pst_RLI->pul_RLI[ 0 ] );

	//pst_RLI = COLOR_RLIAdjust_ListGet( _pul_RLI );
	//if (!pst_RLI) pst_RLI = COLOR_RLIAdjust_ListAdd( _pul_RLI );
	//COLOR_RLIAdjust_AlphaAddSerial( pst_RLI->pul_RLI + 1, pst_RLI->dul_Save + 1, pst_RLI->pul_RLI[ 0 ] );
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void COLOR_RLIAdjust_RGBMulSerial( ULONG *_pul_Tgt, ULONG *_pul_Src, int n )
{
	ULONG	RGB;
	double	r, g, b, a;
		
	while ( n-- )
	{
		RGB = *_pul_Src++;
		
		// RGB in float
		r = (RGB & 0xff) * 1.0 / 255.0;
		g = ((RGB & 0xff00) >> 8) * 1.0 / 255.0;
		b = ((RGB & 0xff0000) >> 16) * 1.0 / 255.0;
		a = ((RGB & 0xff000000) >> 24) * 1.0 / 255.0;

		r *= COLOR_gd_r;
		if (r < 0) r = 0; 
		else if (r > 1) r = 1;

		g *= COLOR_gd_g;
		if (g < 0) g = 0; 
		else if (g > 1) g = 1;

		b *= COLOR_gd_b;
		if (b < 0) b = 0; 
		else if (b > 1) b = 1;

		a += COLOR_gd_a;
		if (a < 0) a = 0; 
		else if (a > 1) a = 1;
	
		// r,g,b, 2 RGB
		RGB &= 0x00000000;
		RGB |= ((ULONG) (a * 255.0f)) << 24;
		RGB |= ((ULONG) (b * 255.0f)) << 16;
		RGB |= ((ULONG) (g * 255.0f)) << 8;
		RGB |= ((ULONG) (r * 255.0f));
		
		*_pul_Tgt++ = RGB;
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void COLOR_RLIAdjust_RGBMul( ULONG *_pul_RLI )
{
	COLOR_RLIList_Node *pst_RLI;
	
	if ( !_pul_RLI ) return;
	pst_RLI = COLOR_RLIAdjust_ListGet( _pul_RLI );
	if (!pst_RLI) pst_RLI = COLOR_RLIAdjust_ListAdd( _pul_RLI );
	
	COLOR_RLIAdjust_RGBMulSerial( pst_RLI->pul_RLI + 1, pst_RLI->dul_Save + 1, pst_RLI->pul_RLI[ 0 ] );
	//COLOR_RLIAdjust_AlphaAddSerial( pst_RLI->pul_RLI + 1, pst_RLI->dul_Save + 1, pst_RLI->pul_RLI[ 0 ] );
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void RGB_to_HLS( double r, double g, double b, double *h, double *l , double *s )
{
    double max, min, delta, sum;

    max = (r > g) ? ( (r > b) ? r : b) : ( (g > b) ? g : b);
    min = (r < g) ? ( (r < b) ? r : b) : ( (g < b) ? g : b);
    delta = max - min;
    sum = max + min;

    /* luminosity */
    *l = (max + min) / 2.0;

    /* saturation */
    if (max == min)
    {
        *s = 0;
        *h = 0;
    }
    else
    {
        *s = (*l <= 0.5) ? (delta / sum) : (delta / (2 - sum) );

        /* hue */
        if (r == max)
            *h  = (g-b) / delta;
        else if (g == max)
            *h = 2.0 + (b - r) / delta;
        else
            *h = 4.0 + (r - g) / delta;

        *h *= 60.0;
        if ( *h < 0) *h += 360.0;
        if ( *h > 360.0) *h -= 360.0;
        *h /= 360;
    }

    *s = 1.0 - *s;
}


/*
 ===================================================================================================
 ===================================================================================================
 */
double HLS_Value( double n1, double n2, double h)
{
    if ( h > 360.0)
        h -= 360.0;
    else if (h < 0)
        h += 360.0;

    if ( h < 60.0)
        return n1 + ( (n2 - n1) * h) / 60.0;
    if (h < 180.0)
        return n2;
    if (h < 240.0)
        return n1 + (n2 - n1) * (240.0 - h ) / 60.0;
    
    return n1;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void HLS_to_RGB( double h, double l, double s, double *r, double *g, double *b )
{
    double m1, m2;

    s = 1.0f - s;

    if (s == 0.0)
    {
        *r = *g = *b = l;
        return;
    }

    m2 = ( l <= 0.5f) ? ( l + (l * s) ) : (l + s - l * s);
    m1 = 2.0f * l - m2;
    h *= 360.0f;

    *r = HLS_Value(m1, m2, h + 120.0f );
    *g = HLS_Value(m1, m2, h );
    *b = HLS_Value( m1, m2, h - 120.0f );
}

#endif

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
