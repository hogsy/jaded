/*=============================================================================
	Perlin.h: Support for Perlin noise (copyright Ken Perlin)
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Warren Marshall
=============================================================================*/
#include "precomp.h"
#include "perlinnoise.h"

PelinNoise g_PerlinNoiseMaker;

void PerlinNormalize2(FLOAT v[2])
{
    FLOAT s;

    s = (FLOAT)L_sqrt(v[0] * v[0] + v[1] * v[1]);
    v[0] = v[0] / s;
    v[1] = v[1] / s;
}

void PerlinNormalize3(FLOAT v[3])
{
    FLOAT s;

    s = (FLOAT)L_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] = v[0] / s;
    v[1] = v[1] / s;
    v[2] = v[2] / s;
}


FLOAT PerlinNoise1(FLOAT arg)
{
    INT bx0, bx1;
    FLOAT rx0, rx1, sx, t, u, v, vec[1];

    vec[0] = arg;

    setup(0, bx0,bx1, rx0,rx1);

    sx = s_curve(rx0);

    u = rx0 * g_PerlinNoiseMaker.g1[ g_PerlinNoiseMaker.p[ bx0 ] ];
    v = rx1 * g_PerlinNoiseMaker.g1[ g_PerlinNoiseMaker.p[ bx1 ] ];

    return lerp(sx, u, v);
}

FLOAT PerlinNoise2(FLOAT vec[2])
{
    INT bx0, bx1, by0, by1, b00, b10, b01, b11;
    FLOAT rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
    register int i, j;

    setup(0, bx0,bx1, rx0,rx1);
    setup(1, by0,by1, ry0,ry1);

    i = g_PerlinNoiseMaker.p[ bx0 ];
    j = g_PerlinNoiseMaker.p[ bx1 ];

    b00 = g_PerlinNoiseMaker.p[ i + by0 ];
    b10 = g_PerlinNoiseMaker.p[ j + by0 ];
    b01 = g_PerlinNoiseMaker.p[ i + by1 ];
    b11 = g_PerlinNoiseMaker.p[ j + by1 ];

    sx = s_curve(rx0);
    sy = s_curve(ry0);

    q = g_PerlinNoiseMaker.g2[ b00 ] ; u = at2(rx0,ry0);
    q = g_PerlinNoiseMaker.g2[ b10 ] ; v = at2(rx1,ry0);
    a = lerp(sx, u, v);

    q = g_PerlinNoiseMaker.g2[ b01 ] ; u = at2(rx0,ry1);
    q = g_PerlinNoiseMaker.g2[ b11 ] ; v = at2(rx1,ry1);
    b = lerp(sx, u, v);

    return lerp(sy, a, b);
}

FLOAT PerlinNoise3(FLOAT vec[3])
{
    INT bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
    FLOAT rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
    register int i, j;

    setup(0, bx0,bx1, rx0,rx1);
    setup(1, by0,by1, ry0,ry1);
    setup(2, bz0,bz1, rz0,rz1);

    i = g_PerlinNoiseMaker.p[ bx0 ];
    j = g_PerlinNoiseMaker.p[ bx1 ];

    b00 = g_PerlinNoiseMaker.p[ i + by0 ];
    b10 = g_PerlinNoiseMaker.p[ j + by0 ];
    b01 = g_PerlinNoiseMaker.p[ i + by1 ];
    b11 = g_PerlinNoiseMaker.p[ j + by1 ];

    t  = s_curve(rx0);
    sy = s_curve(ry0);
    sz = s_curve(rz0);

    q = g_PerlinNoiseMaker.g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
    q = g_PerlinNoiseMaker.g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
    a = lerp(t, u, v);

    q = g_PerlinNoiseMaker.g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
    q = g_PerlinNoiseMaker.g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
    b = lerp(t, u, v);

    c = lerp(sy, a, b);

    q = g_PerlinNoiseMaker.g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
    q = g_PerlinNoiseMaker.g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
    a = lerp(t, u, v);

    q = g_PerlinNoiseMaker.g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
    q = g_PerlinNoiseMaker.g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
    b = lerp(t, u, v);

    d = lerp(sy, a, b);

    return lerp(sz, c, d);
}

void PerlinInit(void)
{
    INT i, j, k;

    for (i = 0 ; i < p_B ; i++) {
        g_PerlinNoiseMaker.p[i] = i;

        g_PerlinNoiseMaker.g1[i] = (FLOAT)((rand() % (p_B + p_B)) - p_B) / p_B;

        for (j = 0 ; j < 2 ; j++)
            g_PerlinNoiseMaker.g2[i][j] = (FLOAT)((rand() % (p_B + p_B)) - p_B) / p_B;
        PerlinNormalize2(g_PerlinNoiseMaker.g2[i]);

        for (j = 0 ; j < 3 ; j++)
            g_PerlinNoiseMaker.g3[i][j] = (FLOAT)((rand() % (p_B + p_B)) - p_B) / p_B;
        PerlinNormalize3(g_PerlinNoiseMaker.g3[i]);
    }

    while (--i) {
        k = g_PerlinNoiseMaker.p[i];
        g_PerlinNoiseMaker.p[i] = g_PerlinNoiseMaker.p[j = rand() % p_B];
        g_PerlinNoiseMaker.p[j] = k;
    }

    for (i = 0 ; i < p_B + 2 ; i++) {
        g_PerlinNoiseMaker.p[p_B + i] = g_PerlinNoiseMaker.p[i];
        g_PerlinNoiseMaker.g1[p_B + i] = g_PerlinNoiseMaker.g1[i];
        for (j = 0 ; j < 2 ; j++)
            g_PerlinNoiseMaker.g2[p_B + i][j] = g_PerlinNoiseMaker.g2[i][j];
        for (j = 0 ; j < 3 ; j++)
            g_PerlinNoiseMaker.g3[p_B + i][j] = g_PerlinNoiseMaker.g3[i][j];
    }
}
