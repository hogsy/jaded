/*=============================================================================
	Perlin.h: Support for Perlin noise (copyright Ken Perlin)
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Warren Marshall
=============================================================================*/

#ifndef __PERLINNOISE_H__INCLUDED
#define __PERLINNOISE_H__INCLUDED

#ifdef JADEFUSION
extern class CPerlinNoise g_PerlinNoiseMaker;

class CPerlinNoise
{
	#define p_B 0x100
	#define p_BM 0xff

	#define p_N 0x1000
	#define p_NP 12   // 2^N
	#define p_NM 0xfff

	INT p[p_B + p_B + 2];
	FLOAT g3[p_B + p_B + 2][3];
	FLOAT g2[p_B + p_B + 2][2];
	FLOAT g1[p_B + p_B + 2];

	#define s_curve(t) ( t * t * (3.f - 2.f * t) )

	#define lerp(t, a, b) ( a + t * (b - a) )

	#define setup(i,b0,b1,r0,r1)\
		t = vec[i] + p_N;\
		b0 = ((INT)t) & p_BM;\
		b1 = (b0+1) & p_BM;\
		r0 = t - (INT)t;\
		r1 = r0 - 1.f;

	void normalize2(FLOAT v[2])
	{
		FLOAT s;

		s = (FLOAT)L_sqrt(v[0] * v[0] + v[1] * v[1]);
		v[0] = v[0] / s;
		v[1] = v[1] / s;
	}

	void normalize3(FLOAT v[3])
	{
		FLOAT s;

		s = (FLOAT)L_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		v[0] = v[0] / s;
		v[1] = v[1] / s;
		v[2] = v[2] / s;
	}

public:
	FLOAT noise1(FLOAT arg)
	{
		INT bx0, bx1;
		FLOAT rx0, rx1, sx, t, u, v, vec[1];

		vec[0] = arg;

		setup(0, bx0,bx1, rx0,rx1);

		sx = s_curve(rx0);

		u = rx0 * g1[ p[ bx0 ] ];
		v = rx1 * g1[ p[ bx1 ] ];

		return lerp(sx, u, v);
	}

	FLOAT noise2(FLOAT vec[2])
	{
		INT bx0, bx1, by0, by1, b00, b10, b01, b11;
		FLOAT rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
		register int i, j;

		setup(0, bx0,bx1, rx0,rx1);
		setup(1, by0,by1, ry0,ry1);

		i = p[ bx0 ];
		j = p[ bx1 ];

		b00 = p[ i + by0 ];
		b10 = p[ j + by0 ];
		b01 = p[ i + by1 ];
		b11 = p[ j + by1 ];

		sx = s_curve(rx0);
		sy = s_curve(ry0);

	#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

		q = g2[ b00 ] ; u = at2(rx0,ry0);
		q = g2[ b10 ] ; v = at2(rx1,ry0);
		a = lerp(sx, u, v);

		q = g2[ b01 ] ; u = at2(rx0,ry1);
		q = g2[ b11 ] ; v = at2(rx1,ry1);
		b = lerp(sx, u, v);

		return lerp(sy, a, b);
	}

	FLOAT noise3(FLOAT vec[3])
	{
		INT bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
		FLOAT rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
		register int i, j;

		setup(0, bx0,bx1, rx0,rx1);
		setup(1, by0,by1, ry0,ry1);
		setup(2, bz0,bz1, rz0,rz1);

		i = p[ bx0 ];
		j = p[ bx1 ];

		b00 = p[ i + by0 ];
		b10 = p[ j + by0 ];
		b01 = p[ i + by1 ];
		b11 = p[ j + by1 ];

		t  = s_curve(rx0);
		sy = s_curve(ry0);
		sz = s_curve(rz0);

	#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

		q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
		q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
		a = lerp(t, u, v);

		q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
		q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
		b = lerp(t, u, v);

		c = lerp(sy, a, b);

		q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
		q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
		a = lerp(t, u, v);

		q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
		q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
		b = lerp(t, u, v);

		d = lerp(sy, a, b);

		return lerp(sz, c, d);
	}

	void init(void)
	{
		INT i, j, k;

		for (i = 0 ; i < p_B ; i++) {
			p[i] = i;

			g1[i] = (FLOAT)((rand() % (p_B + p_B)) - p_B) / p_B;

			for (j = 0 ; j < 2 ; j++)
				g2[i][j] = (FLOAT)((rand() % (p_B + p_B)) - p_B) / p_B;
			normalize2(g2[i]);

			for (j = 0 ; j < 3 ; j++)
				g3[i][j] = (FLOAT)((rand() % (p_B + p_B)) - p_B) / p_B;
			normalize3(g3[i]);
		}

		while (--i) {
			k = p[i];
			p[i] = p[j = rand() % p_B];
			p[j] = k;
		}


		for (i = 0 ; i < p_B + 2 ; i++) {
			p[p_B + i] = p[i];
			g1[p_B + i] = g1[i];
			for (j = 0 ; j < 2 ; j++)
				g2[p_B + i][j] = g2[i][j];
			for (j = 0 ; j < 3 ; j++)
				g3[p_B + i][j] = g3[i][j];
		}
	}
};
#else //JADEFUSION
#define p_B 0x100
#define p_BM 0xff
#define p_N 0x1000
#define p_NP 12   // 2^N
#define p_NM 0xfff
#define s_curve(t) ( t * t * (3.f - 2.f * t) ) 
#define lerp(t, a, b) ( a + t * (b - a) )
#define setup(i,b0,b1,r0,r1)\
    t = vec[i] + p_N;\
    b0 = ((INT)t) & p_BM;\
    b1 = (b0+1) & p_BM;\
    r0 = t - (INT)t;\
    r1 = r0 - 1.f;
#define at2(rx,ry) ( rx * q[0] + ry * q[1] )
#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

typedef struct PerlinNoise_
{
	INT p[p_B + p_B + 2];
	FLOAT g3[p_B + p_B + 2][3];
	FLOAT g2[p_B + p_B + 2][2];
	FLOAT g1[p_B + p_B + 2];
} PelinNoise;

extern PelinNoise g_PerlinNoiseMaker;

void PerlinNormalize2(FLOAT v[2]);
void PerlinNormalize3(FLOAT v[3]);
FLOAT PerlinNoise1(FLOAT arg);
FLOAT PerlinNoise2(FLOAT vec[2]);
FLOAT PerlinNoise3(FLOAT vec[3]);
void PerlinInit(void);

#endif //JADEFUSION
#endif // #ifndef __PERLINNOISE_H__INCLUDED
/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
