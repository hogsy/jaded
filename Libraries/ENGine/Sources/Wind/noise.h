//------------------------------------------------------------------------
//
//
// Author       Martin Sevigny
// Date         january 2000
//
// File         Noise.h
// Description  Noise Generators with pseudo-random numbers
//
//------------------------------------------------------------------------

//***********************************************************************
//
// Author           Martin Sevigny
// Function         Noise1D
// Description      Generate a pseudo-random number [-1,1] from an integer seed
//
//***********************************************************************
/*inline*/ FLOAT Noise1D(INT x);

//***********************************************************************
//
// Author           Martin Sevigny
// Function         SmoothedNoise1D
// Description      Generate a smoothed pseudo-random number [-1,1] from an integer seed
//
//***********************************************************************
/*inline*/ FLOAT SmoothedNoise1D(INT x);

//***********************************************************************
//
// Author           Martin Sevigny
// Function         InterpolatedNoise1D
// Description      Generate a pseudo-random number [-1,1] from a FLOAT seed
//
//***********************************************************************
/*inline*/ FLOAT InterpolatedNoise1D(FLOAT x);

//***********************************************************************
//
// Author           Martin Sevigny
// Function         InterpolatedNoise1D
// Description      Generate a pseudo-random number [-1,1] from a float
//                  seed using multiple noise frequencies
//
//***********************************************************************
FLOAT PerlinNoise_1D(FLOAT x, FLOAT persistence, INT Number_Of_Octaves);

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
/*inline*/ FLOAT Noise2D(INT x, INT y);

//***********************************************************************
//
// Author           Martin Sevigny
// Function         Noise2D
// Description      Generate a smoothed pseudo-random number [-1,1] from two integer seeds
//
//***********************************************************************
/*inline*/ FLOAT SmoothedNoise2D(INT x, INT y);

//***********************************************************************
//
// Author           Martin Sevigny
// Function         InterpolatedNoise2D
// Description      Generate a pseudo-random number [-1,1] from two FLOAT seeds
//
//***********************************************************************
/*inline*/ FLOAT InterpolatedNoise2D(FLOAT x, FLOAT y);

//***********************************************************************
//
// Author           Martin Sevigny
// Function         InterpolatedNoise1D
// Description      Generate a pseudo-random number [-1,1] from two float
//                  seeds using multiple noise frequencies
//
//***********************************************************************
FLOAT PerlinNoise_2D(FLOAT x, FLOAT y, FLOAT persistence, INT Number_Of_Octaves);


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
FLOAT PseudoRandom(register INT s);

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
FLOAT Swave(FLOAT t, FLOAT s,FLOAT f, FLOAT amp);
