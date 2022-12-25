// This shader is not supposed to be compiled as is.
// It should be included in a shader that defines the several tokens :
// - one or both of the tokens USE_PLIGHT and USE_SLIGHT, which repsectively specify that a parallel light and a spherical light is used.
// - One of the 4 tokens : UV_COUNT_1, UV_COUNT_2, UV_COUNT_3, UV_COUNT_4. It specifies the number of UV layers generated.
// - Optionnally, on or all of the following :
//   UV_0_SOURCE_POSITION : if defined, means that the UVs for the first layer should be derived from the eye-space vertex coordinates.
//   UV_1_SOURCE_POSITION : same as UV_0_SOURCE_POSITION for the second UV layer
//   UV_2_SOURCE_POSITION : same as UV_0_SOURCE_POSITION for the third UV layer
//   UV_3_SOURCE_POSITION : same as UV_0_SOURCE_POSITION for the fourth UV layer
//   UV_0_SOURCE_NORMAL : if defined, means that the UVs for the first layer should be derived from the eye-space vertex normals.
//   UV_1_SOURCE_NORMAL : same as UV_0_SOURCE_NORMAL for the second UV layer
//   UV_2_SOURCE_NORMAL : same as UV_0_SOURCE_NORMAL for the third UV layer
//   UV_3_SOURCE_NORMAL : same as UV_0_SOURCE_NORMAL for the fourth UV layer
//   For a given layer n, UV_n_SOURCE_POSITION and UV_n_SOURCE_NORMAL can't be used simultaneously.

// Note that even if one of the 2 lights is unused, the space used by its parameters is still held in the constant array.

#ifdef _XBOX
xvs.1.0

#define VERTEX_POSITION v0
#define VERTEX_NORMAL v2
#define VERTEX_COLOR v3
#define VERTEX_UV v9

#else
vs.1.0

#define VERTEX_POSITION v0
#define VERTEX_NORMAL v3
#define VERTEX_COLOR v5
#define VERTEX_UV v7

#endif

// define temporary register
#define TEMP_0 r9
#define TEMP_1 r10
#define TEMP_2 r11

#define TEMP_TPOS r0
#define TEMP_COLOR r1

////////////////////
// Position & Fog //
////////////////////

#define INPUT_POSITION VERTEX_POSITION
#define TRANSFORM_MATRIX 0
#define RESULT_TPOS TEMP_TPOS
#define FOG_PARAMS 4
#include "Modules\TransformAndFog_module.vsh"

//////////////
// Lighting //
//////////////

// start with the RLIs (ambient is already in the RLIs)
// lighting leaves the alpha (w) component untouched, so we have to do a mov at some point... here it is :
mov TEMP_COLOR, VERTEX_COLOR

#ifdef USE_PLIGHT
#define INPUT_COLOR TEMP_COLOR
#define OUTPUT_COLOR TEMP_COLOR
#define INPUT_NORMAL VERTEX_NORMAL
#define PLIGHT_DIR 4
#define PLIGHT_COL 5
#include "Modules\PLight_module.vsh"
#endif USE_PLIGHT

#ifdef USE_SLIGHT
#define INPUT_COLOR TEMP_COLOR
#define OUTPUT_COLOR TEMP_COLOR
#define INPUT_POSITION VERTEX_POSITION
#define INPUT_NORMAL VERTEX_NORMAL
#define SLIGHT_POS 6
#define SLIGHT_ATTENUATION 7
#define SLIGHT_COL 8
#include "Modules\SLight_module.vsh"
#endif USE_SLIGHT

mov oD0, TEMP_COLOR // have to do this do get the alpha correctly...

//////////////////
// UV Computing //
//////////////////

#ifndef UV_0_SOURCE_POSITION
	#ifndef UV_0_SOURCE_NORMAL
		#define INPUT_UV_SOURCE VERTEX_UV
	#else //UV_0_SOURCE_NORMAL
		#define INPUT_UV_SOURCE VERTEX_NORMAL
	#endif //UV_0_SOURCE_NORMAL
#else UV_0_SOURCE_POSITION
	#define INPUT_UV_SOURCE VERTEX_POSITION
#endif UV_0_SOURCE_POSITION
#define TEXTURE_MATRIX 9
#define OUTPUT_UV oT0
#include "Modules\UV_module.vsh"

#ifndef UV_COUNT_1

	#ifndef UV_1_SOURCE_POSITION
		#ifndef UV_1_SOURCE_NORMAL
			#define INPUT_UV_SOURCE VERTEX_UV
		#else //UV_1_SOURCE_NORMAL
			#define INPUT_UV_SOURCE VERTEX_NORMAL
		#endif //UV_1_SOURCE_NORMAL
	#else // UV_1_SOURCE_POSITION
		#define INPUT_UV_SOURCE VERTEX_POSITION
	#endif // UV_1_SOURCE_POSITION
	#define TEXTURE_MATRIX 11
	#define OUTPUT_UV oT1
	#include "Modules\UV_module.vsh"

	#ifndef UV_COUNT_2

		#ifndef UV_2_SOURCE_POSITION
			#ifndef UV_2_SOURCE_NORMAL
				#define INPUT_UV_SOURCE VERTEX_UV
			#else //UV_2_SOURCE_NORMAL
				#define INPUT_UV_SOURCE VERTEX_NORMAL
			#endif //UV_2_SOURCE_NORMAL
		#else // UV_2_SOURCE_POSITION
			#define INPUT_UV_SOURCE VERTEX_POSITION
		#endif // UV_2_SOURCE_POSITION
		#define TEXTURE_MATRIX 13
		#define OUTPUT_UV oT2
		#include "Modules\UV_module.vsh"

		#ifndef UV_COUNT_3

			#ifndef UV_3_SOURCE_POSITION
				#ifndef UV_3_SOURCE_NORMAL
					#define INPUT_UV_SOURCE VERTEX_UV
				#else //UV_3_SOURCE_NORMAL
					#define INPUT_UV_SOURCE VERTEX_NORMAL
				#endif //UV_3_SOURCE_NORMAL
			#else // UV_3_SOURCE_POSITION
				#define INPUT_UV_SOURCE VERTEX_POSITION
			#endif // UV_3_SOURCE_POSITION
			#define TEXTURE_MATRIX 15
			#define OUTPUT_UV oT3
			#include "Modules\UV_module.vsh"

		#endif // UV_COUNT_3
	#endif // UV_COUNT_2
#endif // UV_COUNT_1
