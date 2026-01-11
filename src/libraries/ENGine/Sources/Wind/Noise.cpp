//------------------------------------------------------------------------
//
//
// Author       Martin Sevigny
// Date         january 2000
//
// File         Noise.cpp
// Description  Noise Generators with pseudo-random numbers
//
//------------------------------------------------------------------------
#include "precomp.h"

#define Floor(x) ((INT)(x) - ((x) < 0.0f))

#define Interpolate(a, b, t) (a*(1.0f - t) + b*t)

//***********************************************************************
//
//  1D Perlin Noise
//
//***********************************************************************

//***********************************************************************
//
// Author           Martin Sevigny
// Function         Noise1D
// Description      Generate a pseudo-random number [-1,1] from an integer seed
//
//***********************************************************************
/*inline*/ FLOAT Noise1D(INT x)
{
    x = (x<<13) ^ x;
    return ( 1.0f - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);    
}

//***********************************************************************
//
// Author           Martin Sevigny
// Function         SmoothedNoise1D
// Description      Generate a smoothed pseudo-random number [-1,1] from an integer seed
//
//***********************************************************************
/*inline*/ FLOAT SmoothedNoise1D(INT x)
{
    return Noise1D(x)/2.0f  +  Noise1D(x-1)/4.0f  +  Noise1D(x+1)/4.0f;
}

//***********************************************************************
//
// Author           Martin Sevigny
// Function         InterpolatedNoise1D
// Description      Generate a pseudo-random number [-1,1] from a FLOAT seed
//
//***********************************************************************
/*inline*/ FLOAT InterpolatedNoise1D(FLOAT x)
{
    INT integer_X    = Floor(x);
    FLOAT fractional_X = x - integer_X;
    
    FLOAT v1 = SmoothedNoise1D(integer_X);
    FLOAT v2 = SmoothedNoise1D(integer_X + 1);
    
    return Interpolate(v1 , v2 , fractional_X);
}

//***********************************************************************
//
// Author           Martin Sevigny
// Function         InterpolatedNoise1D
// Description      Generate a pseudo-random number [-1,1] from a float
//                  seed using multiple noise frequencies
//
//***********************************************************************
FLOAT PerlinNoise_1D(FLOAT x, FLOAT persistence, INT Number_Of_Octaves)
{
    FLOAT total = 0.0f;
    INT n = Number_Of_Octaves - 1;
    FLOAT frequency;
    FLOAT amplitude;

    if(n==1)
    {
        return InterpolatedNoise1D(x);
    }
    
    for (INT i = 0; i < n; i++)
    {
        frequency = (FLOAT) L_pow(2,i);
        amplitude = (FLOAT) L_pow(persistence,i);

        total = total + InterpolatedNoise1D(x * frequency) * amplitude;
    }
    
    return total;
}

//***********************************************************************
//
//  2D Perlin Noise
//
//***********************************************************************

//***********************************************************************
//
// Author           Martin Sevigny
// Function         Noise2D
// Description      Generate a pseudo-random number [-1,1] from two integer seeds
//
//***********************************************************************
/*inline*/ FLOAT Noise2D(INT x, INT y)
{
    INT n = x + y * 57;
    n = (n<<13) ^ n;
    return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);    
}

//***********************************************************************
//
// Author           Martin Sevigny
// Function         Noise2D
// Description      Generate a smoothed pseudo-random number [-1,1] from two integer seeds
//
//***********************************************************************
/*inline*/ FLOAT SmoothedNoise2D(INT x, INT y)
{
//    FLOAT corners;
    FLOAT sides;
    FLOAT center;

        sides   = ( Noise2D(x-1, y)  +Noise2D(x+1, y)  +Noise2D(x, y-1)  +Noise2D(x, y+1) ) /  6.0f;
        center  =  Noise2D(x, y) / 3.0f;
        return sides + center;

// If we want a smoother result, we can use a point 8 neighbors but it's more costly
  //  {
        //corners = ( Noise2D(x-1, y-1)+Noise2D(x+1, y-1)+Noise2D(x-1, y+1)+Noise2D(x+1, y+1) ) / 16.0f;
        //sides   = ( Noise2D(x-1, y)  +Noise2D(x+1, y)  +Noise2D(x, y-1)  +Noise2D(x, y+1) ) /  8.0f;
        //center  =  Noise2D(x, y) / 4.0f;
        //return corners + sides + center;
    //}

}

//***********************************************************************
//
// Author           Martin Sevigny
// Function         InterpolatedNoise2D
// Description      Generate a pseudo-random number [-1,1] from two FLOAT seeds
//
//***********************************************************************
/*inline*/ FLOAT InterpolatedNoise2D(FLOAT x, FLOAT y)
{
    INT integer_X    = Floor(x);
    FLOAT fractional_X = x - integer_X;

    INT integer_Y    = Floor(y);
    FLOAT fractional_Y = y - integer_Y;

    
    FLOAT v1 = SmoothedNoise2D(integer_X,     integer_Y);
    FLOAT v2 = SmoothedNoise2D(integer_X + 1, integer_Y);
    FLOAT v3 = SmoothedNoise2D(integer_X,     integer_Y + 1);
    FLOAT v4 = SmoothedNoise2D(integer_X + 1, integer_Y + 1);
    
    FLOAT i1 = Interpolate(v1 , v2 , fractional_X);
    FLOAT i2 = Interpolate(v3 , v4 , fractional_X);
    
    return Interpolate(i1 , i2 , fractional_Y);
        
}        

//***********************************************************************
//
// Author           Martin Sevigny
// Function         InterpolatedNoise1D
// Description      Generate a pseudo-random number [-1,1] from two float
//                  seeds using multiple noise frequencies
//
//***********************************************************************
FLOAT PerlinNoise_2D(FLOAT x, FLOAT y, FLOAT persistence, INT Number_Of_Octaves)
{
    FLOAT total = 0.0f;
    FLOAT p = persistence;
    INT n = Number_Of_Octaves - 1;
    
    if(n==1)
    {
        return InterpolatedNoise2D(x, y);
    }
    
    for (INT i = 0; i < n; i++)
    {
        FLOAT frequency = (FLOAT) L_pow(2.0f,i);
        FLOAT amplitude = (FLOAT) L_pow(p,i);
        
        total = total + InterpolatedNoise2D(x * frequency, y * frequency) * amplitude;
        
    }
    return total;

}


//***********************************************************************
//
//  This method generates sinusoidal-like waves 
//
//  source: Graphics Gem V p.367
//
//***********************************************************************

//***********************************************************************
//
// Author           Martin Sevigny
// Function         PseudoRandom
// Description      Generate a pseudo-random number [0,1] from an integer seeds
//
//***********************************************************************
FLOAT PseudoRandom(register INT s)
{
    s = s<<13 ^ s;
    return ( (s * (s * s * 15731 + 789221) + 1376312589) & 0x7fffffff) / 2147483648.0f;    

}

#define FRND(a) PseudoRandom(17*a)
#define ARND(a) PseudoRandom(97*a)


//***********************************************************************
//
// Author           Martin Sevigny
// Function         Swave
// Description      Generate a sinusoidal-like  number [0,1] from a FLOAT seed
//
//  t = Wave parameter
//  s = Shape factor [-1,1]: Stretches toward low values (s < 0) or toward high values (s > 0)
//  f = Frequency variance [0,1]
//  a = Amplitude variance [0,1]
//
//***********************************************************************
FLOAT Swave(FLOAT t, FLOAT s,FLOAT f, FLOAT amp)
{
    INT i,j;
    FLOAT a,b;

    i = j = (INT)floor(t);
    t-=i;
    j++;
    
    if(f)
    {
        a = (FRND(i) - 0.5f) * f;
        b = (FRND(j) - 0.5f) * f + 1.0f;

        if (t<a)
        {
            i--;
            j--;
            t++;
            a++;
            b = a;
            a = (FRND(i) - 0.5f) * f;
        }
        else if (t > b)
        {
            i++;
            j++;
            t--;
            b--;
            a = b;
            b = (FRND(j) - 0.5f) * f + 1.0f;
        }

        t = (t-a) / (b-a);

    }


    if(i & 1)
    {
        i++;
        j--; 
        t = 1.0f-t;
    }

    t = ( s< 0.0f) ? (t+s*t) / (1.0f+s*t) : (s > 0.0f) ? t / (1.0f-s+s*t) : t;
    t *= t * ( 3.0f-t-t);


    if(amp)
    {
        a = ARND(i) * amp * 0.5f;
        b = ARND(j) * amp * 0.5f;
        t = a + t * ( 1.0f -a -b);
    }

    return t;
}
