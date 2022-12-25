#ifndef XESHAREDDEFINES_HEADER
#define XESHAREDDEFINES_HEADER

/////////////////////////////////////////////////////////////////////////////////////////
// common shader feature defines.
/////////////////////////////////////////////////////////////////////////////////////////

// Custom Shader feature
#define CUSTOMSHADER_DISABLE            0

// Environment mapping feature
#define ENVMAP_OFF                      0
#define ENVMAP_ON                       1

// Fog feature
#define FOG_ON                          1
#define FOG_OFF                         0

// RLI features
#define ADD_RLI_OFF                     0
#define ADD_RLI_ON                      1

// shadows
#define SHADOW_OFF                      0
#define SHADOW_ON                       1

// Vertex Specular feature
#define VERTEX_SPECULAR_OFF             0
#define VERTEX_SPECULAR_ON              1

/////////////////////////////////////////////////////////////////////////////////////////
// vertex shader feature defines.
/////////////////////////////////////////////////////////////////////////////////////////

#ifdef ACTIVE_EDITORS
#define VS_MAX_LIGHTS                   55
#define VS_MAX_LIGHTS_IN_ENGINE         4
#else
#define VS_MAX_LIGHTS                   9
#define VS_MAX_LIGHTS_IN_ENGINE         VS_MAX_LIGHTS
#endif

#define VS_MAX_LIGHTS_PER_PASS          4
#define VS_MAX_BONES                    50

// Vertex Transform feature
#define TRANSFORM_NONE                  0
#define TRANSFORM_NORMAL                1
#define TRANSFORM_SKINNING              2
#define TRANSFORM_MORPHING              3

// Texture Transform feature
#define TEXTRANSFORM_NO_TEX_COORD       0
#define TEXTRANSFORM_NONE               1
#define TEXTRANSFORM_NORMAL             2
#define TEXTRANSFORM_NONE_FUR_OFFSETS   3
#define TEXTRANSFORM_NORMAL_FUR_OFFSETS 4

// Color Source feature
#define COLORSOURCE_DIFFUSE             0
#define COLORSOURCE_DIFFUSE2X           1
#define COLORSOURCE_CONSTANT            2
#define COLORSOURCE_VERTEX_COLOR        3
#define COLORSOURCE_FULL_LIGHT          4
#define COLORSOURCE_INVERSE_DIFFUSE     5

// Lighting type feature
#define LIGHTING_NO_COLOR               0
#define LIGHTING_USE_COLOR              1
#define LIGHTING_VERTEX                 2
#define LIGHTING_PIXEL                  3

// UV Source feature
#define UVSOURCE_OBJECT                 0
#define UVSOURCE_CHROME                 1
#define UVSOURCE_PLANAR_GIZMO           2
#define UVSOURCE_DIFFUSEPHALOFF         3

// light type feature
#define LIGHT_TYPE_NONE                 0
#define LIGHT_TYPE_DIRECT               1
#define LIGHT_TYPE_OMNI                 2
#define LIGHT_TYPE_SPOT                 3
#define LIGHT_TYPE_CYLINDRICAL_SPOT     4
#define LIGHT_TYPE_OMNI_INVERTED        5

// ambient feature
#define ADD_AMBIENT_OFF                 0
#define ADD_AMBIENT_ON                  1

// RLI feature
#define RLI_SCALE_OFFSET_OFF            0
#define RLI_SCALE_OFFSET_ON             1

// Reflection feature
#define REFLECTION_TYPE_NONE            0
#define REFLECTION_TYPE_PLANAR          1
#define REFLECTION_TYPE_CUBEMAP         2

// Rim Light
#define RIMLIGHT_OFF                    0
#define RIMLIGHT_ON                     1

// Depth to color
#define DEPTH_TO_COLOR_OFF              0
#define DEPTH_TO_COLOR_ON               1

// Lightmap
#define LIGHTMAP_OFF                    0
#define LIGHTMAP_ON                     1

// Lightmap
#define INVERTMOSS_OFF                  0
#define INVERTMOSS_ON                   1

/////////////////////////////////////////////////////////////////////////////////////////
// pixel shader feature defines.
/////////////////////////////////////////////////////////////////////////////////////////

// Local Alpha feature
#define LOCAL_ALPHA_ON                  1
#define LOCAL_ALPHA_OFF                 0

// Base map
#define BASEMAP_OFF                     0
#define BASEMAP_ON                      1

// Diffuse PPL
#define DIFFUSE_PPL_OFF                 0
#define DIFFUSE_PPL_ON                  1

// Specular PPL
#define SPECULAR_PPL_OFF                0
#define SPECULAR_PPL_ON                 1

// Specular map
#define SPECULARMAP_OFF                 0
#define SPECULARMAP_ON                  1

// Detail normal map
#define DETAILNMAP_OFF                  0
#define DETAILNMAP_ON                   1

// Moss map
#define MOSS_MAP_OFF                    0
#define MOSS_MAP_ON                     1

// offset map
#define USE_OFFSET_OFF					0
#define USE_OFFSET_ON					1

// Specular map channel
#define SPECULARMAPCHANNEL_A            0
#define SPECULARMAPCHANNEL_R            1
#define SPECULARMAPCHANNEL_G            2
#define SPECULARMAPCHANNEL_B            3

// Apply 2X on final color
#define COLOR2X_OFF                     0
#define COLOR2X_ON                      1

// Alpha range remaping
#define DONT_REMAP_ALPHA_RANGE          0
#define REMAP_ALPHA_RANGE               1

// ambient lighting
#define DONT_ADD_AMBIENT                0
#define ADD_AMBIENT                     1

// normal offset feature
#define NORMAL_OFFSET_OFF               0
#define NORMAL_OFFSET_ON                1

/////////////////////////////////////////////////////////////////////////////////////////
// custom shader feature defines.
/////////////////////////////////////////////////////////////////////////////////////////

#define ZOOM_BLUR_MAX_STEPS_PER_PASS    3

// GodRay features 
#define GODRAY_SHADER_1                 1
#define GODRAY_SHADER_2                 2
#define GODRAY_SHADER_3                 3
#define GODRAY_SHADER_4                 4
#define GODRAY_SHADER_5                 5

// Blit features 
#define BLIT_SHADER_1                   1

// Zoom smooth features 
#define ZOOM_SMOOTH_SHADER_1            1
#define ZOOM_SMOOTH_SHADER_2            2

// Remanance features
#define REMANANCE_SHADER_1              1

// Black and white features
#define BLACK_WHITE_SHADER              1

// Motion Blur features
#define MOTION_BLUR_SHADER_1            1

// Big Blur features
#define BIG_BLUR_SHADER_1               1

// Water features
#define WATER_SHADER_1                  1
#define WATER_SHADER_2                  2

// Heat shimmering features
#define HEAT_SHIMMER_MASK_PASS          0 
#define HEAT_SHIMMER_OFFSET_PASS        1 
#define HEAT_SHIMMER_ALPHA_FROM_TEXTURE 1

// Fog Effect
#define FOG_SHADER_1                    1


#define COLOR_DIFFUSION_SHADER_1        1
#define COLOR_DIFFUSION_SHADER_2        2
#define COLOR_DIFFUSION_SHADER_3        3
#define COLOR_DIFFUSION_SHADER_4        4
#define COLOR_DIFFUSION_SHADER_5        5

// SPG2 features
#define SPG2_MODE_NONE                  0
#define SPG2_MODE_Y                     1
#define SPG2_MODE_X                     2
#define SPG2_MODE_HAT                   3
#define SPG2_MODE_SPRITES               4
#define SPG2_MODE_SPECIALX              5

// Symmetry modifier modes
#define SYMMETRY_MODE_NONE              0
#define SYMMETRY_MODE_X                 1
#define SYMMETRY_MODE_Y                 2
#define SYMMETRY_MODE_Z                 3

// Material LOD states
#define MAT_LOD_FULL                    0
#define MAT_LOD_BLEND                   1
#define MAT_LOD_OPTIMIZED               2

#endif // XESHAREDDEFINES_HEADER
