// Parameters :
// INPUT_POSITION : input register from which local position is taken (usually v0)
// TRANSFORM_MATRIX : offset specifying where to find the transform matrix in the constant array
// RESULT_TPOS : temporary register where the transformed position will be held (you can then use it later in the shader - e.g. for UV generation)
// FOG_PARAMS : offset for the fog parameters so that c[FOG_PARAMS] is in the form (fog_speed, z_start, ignored, ignored)
// TEMP_0 : a temporary register that can be safely used

// Check that the requested defines exist
///////////////////////////////////////////
#ifndef INPUT_POSITION
#error You need to specify INPUT_POSITION
#endif
#ifndef TRANSFORM_MATRIX
#error You need to specify TRANSFORM_MATRIX
#endif
#ifndef RESULT_TPOS
#error You need to specify RESULT_TPOS
#endif
#ifndef FOG_PARAMS
#error You need to specify FOG_PARAMS
#endif
#ifndef TEMP_0
#error You need to specify TEMP_0
#endif

/////////////////////
// POINT TRANSFORM //
/////////////////////

// Transform vertex position from world to projection space.
dp4 RESULT_TPOS.x, INPUT_POSITION, c[TRANSFORM_MATRIX+0]
dp4 RESULT_TPOS.y, INPUT_POSITION, c[TRANSFORM_MATRIX+1]
dp4 RESULT_TPOS.z, INPUT_POSITION, c[TRANSFORM_MATRIX+2]
dp4 RESULT_TPOS.w, INPUT_POSITION, c[TRANSFORM_MATRIX+3]
mov oPos, RESULT_TPOS

/////////////////////
// FOG COMPUTATION //
/////////////////////
// we use depth based fog (as does the rest of the engine) as this is faster to compute
mov oFog.x, RESULT_TPOS.w

// Undefine the used defines so that they do not have side-effects on another module
//////////////////////////////////////////////////////////////////////////////////////
#undef INPUT_POSITION
#undef TRANSFORM_MATRIX
#undef RESULT_TPOS
#undef FOG_PARAMS
// no point in undefining TEMP_0
