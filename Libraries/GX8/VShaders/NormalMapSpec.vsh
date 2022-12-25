vs.1.0

;------------------------------------------------------------------------------
//		Vertex Declarations

//	position v0 
//	normal v1
//	couleur v2
//	texcoord0 v3
//	texcoord1 v4
//	tangent2 v5
//	binormal3 v6

;------------------------------------------------------------------------------
def c25,1,0,-1,0 //diffuse
def c26,0,0,0,0 //zero
def c24,2,0,0,0
def c27,0.3f,0.3f,0.3f,0.3f //ambiant
def c28,1,1,1,1 //spec
def c29,1,1,1,0 //bump scal
def c30,0.5f,0.5f,0.5f,0.5f //bias

// mul normal by offest
//mul r2,v1,c15

// add to vertex position
//add r1, r2,v0

//tranform position
m4x4 r0,v0,c0//v0//r1
mov oPos,r0
m4x4 r3,v0,c8

// Send Fog
mov oFog.x, r0.w


/*     
//tnb generation      
m3x3 r7,v1,c8            //gen normal

//m3x3 r4,v6,c8
m3x3 r5,v5,c8            //gen tangent
*/

dp3 r7.x, v1, c8			// N
dp3 r7.y, v1, c9
dp3 r7.z, v1, c10

dp3 r5.x, v5, c8			// T
dp3 r5.y, v5, c9
dp3 r5.z, v5, c10

//mul r7.xyz,r7.xyz,c24.x
//mul r5.xyz,r5.xyz,c24.x

//direction lumiere au vertex
add r9,c4,-r3 //attention vertex non transformé
//mov r9,-c5//inverse le vecteur

; Normalise le beau vecteur lumiere
/*dp3 r9.w, r9, r9
rsq r9.w, r9.w
mul r2, r9, r9.w*/

//gen binormal via Cross product

mul r0,r5.zxy,r7.yzx   // cross prod pour binormal
mad r4,r5.yzx,r7.zxy,-r0
mul r4,r4,v5.w

; Transformation vecteur lumiere en espace tangent
dp3 r0.x, r5, r9 //tangent//v5
dp3 r0.y, r4, r9 //binormale//v6 ou r6//c5
dp3 r0.z, r7, r9 //normal



; Normalise le beau vecteur lumiere
dp3 r0.w, r0, r0
rsq r0.w, r0.w
mul r2, r0, r0.w

;ajoute 
; normalise lumiere de 0 à 1[-1,1] => [0,1]
; place dans diffuse oD0 pour 
; calcul DOTPRODUCT3 avec la lumiere transformé

//mad  oD0, r2, c7.x, c7.x//<-----------------
//add oD0,r9,v2

//mov oT2.xyz,r2

/*mad  r2.xyz, r2, c30, c30
mov oT2.xyz,r2
mov oD0,r2
*/


//specular
m4x4 r6,v0,c8 //? c0 ?

//transform position from world
//get a vector toward the camera
add r6,c6,-r6 // =V

//mul r4,r4,v5.w
// V to Tangent Space
dp3  r1.x, r5.xyz, r6.xyz
dp3  r1.y, r4.xyz, r6.xyz
dp3  r1.z, r7.xyz, r6.xyz

//normalize V
dp3  r1.w, r1.xyz, r1.xyz       // r1.w = |r1|^2
rsq  r1.w, r1.w         // r1.w = 1/sqrt(r0.w) = 1/|r1|
mul  r1.xyz, r1.xyz, r1.w   // r1 - normalized V vector in Tangent Space

add	 r1.xyz, r1.xyz, r2.xyz //V+L

//mul r1,r1,c24.x

//normalize
dp3  r1.w, r1.xyz, r1.xyz       // r1.w = |r1|^2
rsq  r1.w, r1.w         // r1.w = 1/sqrt(r0.w) = 1/|r1|
mul  r1.xyz, r1.xyz, r1.w   // r1 - normalized V vector in Tangent Space


/*
// normalize vector in r1
dp3  r1.w, r1, r1       // r1.w = |r1|^2
rsq  r1.w, r1.w         // r1.w = 1/sqrt(r0.w) = 1/|r1|
mul  r1.xyz, r1, r1.w   // r1 - normalized H vector in Tangent Space
*/

/*mad  r1.xyz, r1, c30, c30 // [-1,1] => [0,1]
//mov  oD1.xyz, r1        // Tangent Space Halfangle vector
*/
//mov  oD1.xyz, r1

//mad oD1.xyz, r1, c7.x, c7.x 

//mov r1.w,c26.x

//bias temp
//mad  r1.xyz, r1, c30, c30
//mad  r2.xyz, r2, c30, c30

mov oT3,r1 //V+L = H//r2
mov oT2,r2 //L//r1



//mov oD1,v2//envoi de la couleur diffuse

mov oD0,v2//envoi de la couleur diffuse



// transformation matrix
dp4 r1.x,v3,c20;//
dp4 r1.y,v3,c21;

; sorti normal map coordonnée (+ offset pour test)

add oT0.x, r1.x, c7.z
add oT0.y, r1.y, c7.y

; sorite texture map coordonnées (+ offset pour test)
add oT1.x, r1.x, c7.y
add oT1.y, r1.y, c7.z

