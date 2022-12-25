#ifdef _XBOX
xvs.1.0

#define INPUT_POSITION v0
#define INPUT_CORNER_OFFSET v2
#define INPUT_COLOR v3
#define INPUT_UV v9

#else //_XBOX
vs.1.0

#define INPUT_POSITION v0
#define INPUT_CORNER_OFFSET v3
#define INPUT_COLOR v5
#define INPUT_UV v7

#endif //_XBOX

// INPUT_POSITION			: 3D position of the sprite center
// INPUT_CORNER_OFFSET.xy	: 2 offsets to get 2D corner from 2D center position
// INPUT_COLOR				: vertex color
// INPUT_UV					: corner UVs (2 floats)
// c0-c3 : world x view x projection 
// c4.x  : global size
// c4.y  : global ratio

def c7,0.5f,0.5f,0.5f,1
//def c5,1,0,0,1

// Transform center vertex position from world to projection space.
dp4 r0.x, INPUT_POSITION, c0
dp4 r0.y, INPUT_POSITION, c1
dp4 r0.z, INPUT_POSITION, c2
dp4 r0.w, INPUT_POSITION, c3

// Handle offset and point size
mov r1.xy, c4.x
mul r1.x, r1.x, c4.y
mad r0.xy, r1.xyyy, INPUT_CORNER_OFFSET, r0
mov oPos, r0               // Not exactly optimal, eh ?

// Write the diffuse color to the vertex
mov r1, INPUT_COLOR
mul oD0,r1,c7
//mov oD0,c5

// Write the UVs
mov oT0, INPUT_UV

// Compute fog
// we use depth based fog (as does the rest of the engine) as this is faster to compute
mov oFog.x, r0.w
