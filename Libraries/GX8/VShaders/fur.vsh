
xvs.1.1

// ------------------- FUR SHADER -----------------------

// constant c0 a c3 matrix position
// constant offset c15
// r0 - r2 : for compute
// r2 normals
// c0 - c21 : UV matrix

// mul normal by offest
mul r2,v1,c15

// add to vertex position
add r1, r2,v0

// transformation : world matrix to projection matrix

dp4 r0.x, r1, c0
dp4 r0.y, r1, c1
dp4 r0.z, r1, c2
dp4 r0.w, r1, c3

/*
; r0 = r1 x r2 (3-vector cross-product)
mul  r0, r1.yzxw, r2.zxyw
mad  r0, -r2.yzxw, r1.zxyw, r0
*/




// send vertex
mov oPos, r0

// send Color
mov oD0, v2

// Compute and send UV matrix
dp4 oT0.x,v3,c20;
dp4 oT0.y,v3,c21;

// Send Fog
mov oFog.x, r0.w