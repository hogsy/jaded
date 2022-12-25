;------------------------------------------------------------------------------
; Vertex shader Choucroutte Petarelle , test atomique
;
;	Normal map
;
;------------------------------------------------------------------------------
xvs.1.1

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
;Reset registre r5,r7
def c25,0,0,0,0
//mov r5,c25
//mov r7,c25

; transformation du vertex matrice world 
m4x4 r1, v0, c0
mov oPos,r1

//
//
//
/*
mov r7,v1
dp3 r7.w, r7, r7
rsq r7.w, r7.w
mul r7, r7, r7.w
*/

m3x3 r5,v5,c8      // transforme tan vers world space
m3x3 r7,v1,c8     // transforme norm vers world space
//m3x3 r8,v6,c8     // transforme binorm vers world space

//mov r5,v5
//mov r7,v1


; calcul BiNormale
mul r0,r5.zxy,r7.yzx   // cross prod pour binormal
mad r4,r5.yzx,r7.zxy,-r0
mul r4,r4,v5.w

/*mul r0,-r7.zxy,r5.yzx
mad r4,-r7.yzx,r5.zxy,-r0*/


//mov r4.w, c25.w
/*
mul r0,-r7.yxz,r5.zyx
mad r4,-r7.zyx,r5.yxz,-r0
mov r4.w, c25.w
*/
; Transformation vecteur lumiere en espace tangent
dp3 r0.x, r5.xyz, -c5 //tangent//v5
dp3 r0.y, r4.xyz, c5 //binormale//r4
dp3 r0.z, r7.xyz, -c5 //normal

; Normalise le beau vecteur lumiere
dp3 r0.w, r0, r0
rsq r0.w, r0.w
mul r0, r0, r0.w


; normalise lumiere de 0 à 1
; place dans diffuse oD0 pour 
; calcul DOTPRODUCT3 avec la lumiere transformé
mad oD0, r0, c7.x, c7.x
//mov oD0,v5

dp4 r1.x,v3,c20;//
dp4 r1.y,v3,c21;

; sorti normal map coordonnée (+ offset pour test)
add oT0.x, r1.x, c7.z//z
add oT0.y, r1.y, c7.y//y

; sorite texture map coordonnées (+ offset pour test)
add oT1.x, r1.x, c7.y//y
add oT1.y, r1.y, c7.z//z

// Send Fog

//mov oFog.x, r1.w

