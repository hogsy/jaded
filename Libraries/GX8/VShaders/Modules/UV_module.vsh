// Parameters :
// INPUT_UV_SOURCE : input or temporary register from wich UVs are taken
// TEXTURE_MATRIX : offset specifying where to find the texture matrix in the constant array
// OUTPUT_UV : output register for the computed UVs

// Check that the requested defines exist
///////////////////////////////////////////
#ifndef INPUT_UV_SOURCE
#error You need to specify INPUT_UV_SOURCE
#endif
#ifndef TEXTURE_MATRIX
#error You need to specify TEXTURE_MATRIX
#endif
#ifndef OUTPUT_UV
#error You need to specify OUTPUT_UV
#endif

// UV Transformation
//////////////////////
dp4 OUTPUT_UV.x,INPUT_UV_SOURCE,c[TEXTURE_MATRIX+0];
dp4 OUTPUT_UV.y,INPUT_UV_SOURCE,c[TEXTURE_MATRIX+1];
//dp3 OUTPUT_UV.x,INPUT_UV_SOURCE,c[TEXTURE_MATRIX+0];
//dp3 OUTPUT_UV.y,INPUT_UV_SOURCE,c[TEXTURE_MATRIX+1];

// Undefine the used defines so that they do not have side-effects on another module
//////////////////////////////////////////////////////////////////////////////////////
#undef INPUT_UV_SOURCE
#undef OUTPUT_UV
#undef TEXTURE_MATRIX
