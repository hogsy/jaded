// Parameters :
// INPUT_COLOR : input or temporary register containing the initial color (parallel light effect is added to this register)
// OUTPUT_COLOR : output or temporary register for the resulting color (note that OUTPUT_COLOR.w is left untouched)
// INPUT_NORMAL : input register for the normal (usually v2 on Xbox or v3 on PC)
// PLIGHT_DIR : offset of the parallel light direction vector constant
// PLIGHT_COL : offset of the parallel light color constant
// TEMP_0 : a temporary register that can be safely used

// Check that the requested defines exist
///////////////////////////////////////////
#ifndef INPUT_COLOR
#error You need to specify INPUT_COLOR
#endif
#ifndef OUTPUT_COLOR
#error You need to specify OUTPUT_COLOR
#endif
#ifndef INPUT_NORMAL
#error You need to specify INPUT_NORMAL
#endif
#ifndef PLIGHT_DIR
#error You need to specify PLIGHT_DIR
#endif
#ifndef PLIGHT_COL
#error You need to specify PLIGHT_COL
#endif
#ifndef TEMP_0
#error You need to specify TEMP_0
#endif

// add the parallel light (keeping alpha untouched)
/////////////////////////////////////////////////////
dp3 TEMP_0.w, c[PLIGHT_DIR], INPUT_NORMAL				// compute light angle for vertex
//min TEMP_0.w, TEMP_0.w, C_ONE_AND_ZERO.y					// clamp angle (max angle = 0)
mad OUTPUT_COLOR.xyz, c[PLIGHT_COL], -TEMP_0.w, INPUT_COLOR	// compute color for angle (keeping alpha/w untouched)

// Undefine the used defines so that they do not have side-effects on another module
//////////////////////////////////////////////////////////////////////////////////////
#undef INPUT_COLOR
#undef OUTPUT_COLOR
#undef INPUT_NORMAL
#undef PLIGHT_DIR
#undef PLIGHT_COL
// no point in undefining TEMP_0
