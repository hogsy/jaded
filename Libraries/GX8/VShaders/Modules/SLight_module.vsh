// Parameters :
// INPUT_COLOR : input or temporary register containing the initial color (spherical light effect is added to this register)
// OUTPUT_COLOR : output or temporary register for the resulting color (note that OUTPUT_COLOR.w is left untouched)
// INPUT_POSITION : input register from which local position is taken (usually v0)
// INPUT_NORMAL : input register for the normal (usually v2 on Xbox or v3 on PC)
// SLIGHT_POS : offset of the spherical light position vector constant
// SLIGHT_ATTENUATION : offset of the spherical light attenuation constant
// SLIGHT_COL : offset of the parallel light color constant
// TEMP_0 : a temporary register that can be safely used
// TEMP_1 : another temporary register
// TEMP_2 : yet another temporary register

// Check that the requested defines exist
///////////////////////////////////////////
#ifndef INPUT_COLOR
#error You need to specify INPUT_COLOR
#endif
#ifndef OUTPUT_COLOR
#error You need to specify OUTPUT_COLOR
#endif
#ifndef INPUT_POSITION
#error You need to specify INPUT_POSITION
#endif
#ifndef INPUT_NORMAL
#error You need to specify INPUT_NORMAL
#endif
#ifndef SLIGHT_POS
#error You need to specify SLIGHT_POS
#endif
#ifndef SLIGHT_ATTENUATION
#error You need to specify SLIGHT_ATTENUATION
#endif
#ifndef SLIGHT_COL
#error You need to specify SLIGHT_COL
#endif

// add the spherical light (keeping alpha untouched)
/////////////////////////////////////////////////////
sub TEMP_0.xyz, c[SLIGHT_POS], INPUT_POSITION		// light direction for vertex
dp3 TEMP_1.y, TEMP_0, TEMP_0							// TEMP_1.y = D²
rsq TEMP_1.x, TEMP_1.y									// TEMP_1.x = 1/D
mul TEMP_0.xyz, TEMP_0, TEMP_1.x							// normalize direction vector

dst TEMP_2, TEMP_1.y, TEMP_1.x							// TEMP_2 = (1, D, D², 1/D)
dp3 TEMP_2.w, TEMP_2, c[SLIGHT_ATTENUATION]			// TEMP_2.w = A0 + A1.D + A2.D²
rcp TEMP_2.w, TEMP_2.w									// Now we have the distance attenuation factor
dp3 TEMP_2.z, TEMP_0, INPUT_NORMAL						// compute light angle for vertex - r2.z = cos(angle)
//min TEMP_2.z, TEMP_2.z, C_ONE_AND_ZERO.y				// clamp angle (max angle = 0)

mul TEMP_1.w, TEMP_2.z, TEMP_2.w							// r1.w = max(0, angle)*max(0, 1-D/Far) = total attenuation
mad OUTPUT_COLOR.xyz, c[SLIGHT_COL], TEMP_1.w, INPUT_COLOR	// Add color to intermediate result, taking into account the attenuation

// Undefine the used defines so that they do not have side-effects on another module
//////////////////////////////////////////////////////////////////////////////////////
#undef INPUT_COLOR
#undef OUTPUT_COLOR
#undef INPUT_POSITION
#undef INPUT_NORMAL
#undef SLIGHT_POS
#undef SLIGHT_ATTENUATION
#undef SLIGHT_COL
// no point in undefining TEMP_0, TEMP_1, and TEMP_2
