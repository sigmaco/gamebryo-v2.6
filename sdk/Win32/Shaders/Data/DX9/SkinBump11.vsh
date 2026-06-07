// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

vs_1_1

dcl_position v0
dcl_blendweight v1
dcl_blendindices v2
dcl_normal v3
dcl_binormal v4
dcl_tangent v5
dcl_texcoord0 v7

;Constant map from the NSF.  
;
;CM_Defined      ViewProjTranspose           0    0
;CM_Constant     const_1_1_1_255             4    1    1.0,1.0,1.0,765.01
;CM_Global       Light1Direction             5    1   
;CM_Global       Light2Direction             6    1   
;CM_Constant     Half                        7    1    0.5,0.5,0.5,0.5   
;CM_Defined      SkinBoneMatrix3             8    0    28

;Determine the 4th weight.  We only pack 3, and this insures that the weights
;sum to 1.  We'll keep weights in r0.
mov  r0.xyz, v1.xyz          ; r0.xyz = w0,w1,w2
dp3  r0.w, v1.xyz, c4.xxxx   ; r0.w = w0 + w1 + w2
add  r0.w, -r0.w, c4.x       ; r0.2 = 1 - (w0 + w1 + w2) = w3

;Need to offset the values.  The indices are packed as floats from 0 to 1, but
;they are originally packed as bytes.  Since each bone matrix in this shader 
;takes 3 constant registers, we scale each float by 765.01 to make it equal
;the original byte value times 3.  The .01 insures that any floating point
;rounding does not bias our results incorrectly downward.
mul  r1, v2.zyxw, c4.wwww    ; r1 = indices w/ offset (D3DCOLOR Version)

;We now have the weights in r0 and the indices in r1.  We will move each 
;index into a0.x and blend a matrix together with the weights.  The registers
;r2, r3, and r4 will hold this final 3x4 matrix which we will use to transform
;points into world space as well as normals.  The points will end up in world
;space because each SkinBoneMatrix3 represents a transform from bind pose skin
;space to bone space and the current transform from bone space to world space
;as well as transforms to account for motion of the skinned mesh independent
;of the bones.

;First bone
mov  a0.x, r1.x
mul  r2, r0.x, c[a0.x+0+8]
mul  r3, r0.x, c[a0.x+1+8]
mul  r4, r0.x, c[a0.x+2+8]

;Second bone
mov  a0.x, r1.y
mad  r2, r0.y, c[a0.x+0+8], r2
mad  r3, r0.y, c[a0.x+1+8], r3
mad  r4, r0.y, c[a0.x+2+8], r4

;Third bone
mov  a0.x, r1.z
mad  r2, r0.z, c[a0.x+0+8], r2
mad  r3, r0.z, c[a0.x+1+8], r3
mad  r4, r0.z, c[a0.x+2+8], r4

;Fourth bone
mov  a0.x, r1.w
mad  r2, r0.w, c[a0.x+0+8], r2
mad  r3, r0.w, c[a0.x+1+8], r3
mad  r4, r0.w, c[a0.x+2+8], r4

;The vertex will now be deformed into world space with the blended matrix in
;r2, r3, and r4.
dp4  r5.x, v0, r2
dp4  r5.y, v0, r3
dp4  r5.z, v0, r4
mov  r5.w, c4.x

;This line applies the ViewProj matrix to the deformed position.  As
;stated above, the verts are in world space.  
m4x4    oPos, r5, c0

;We deform N here with the blended matrix in r2, r3, and r4.  This is not
;wholly correct, but it eliminates the need to sort the interactions on the
;host and since we have already calculated it, we should reuse it for 
;efficiency.
dp3  r11.x, v3, r2
dp3  r11.y, v3, r3
dp3  r11.z, v3, r4 

;Now we normalize N.  r11 holds N from here out.
dp3  r10, r11, r11
rsq  r10, r10
mul  r11.xyz, r11, r10

;We need to deform B and T now.  We're taking B and T to world space.  Then
;we can use them to deform back into tangent space bypassing the object space
;of the skin which can be problematic.

dp3  r10.x, v4, r2
dp3  r10.y, v4, r3
dp3  r10.z, v4, r4

;Now we normalize B.  r10 holds B from here on.
dp3  r9, r10, r10
rsq  r9, r9
mul  r10.xyz, r9, r10

;Deform T
dp3  r9.x, v5, r2
dp3  r9.y, v5, r3
dp3  r9.z, v5, r4

;normalize T.  r9 holds T from here on.
dp3  r8, r9, r9
rsq  r8, r8
mul  r9.xyz, r8, r9

;The vectors for the 2 directional lights come to us in world space so we can
;use N, B, and T to transform into tangent space.  We'll use oT3, and oT4
dp3 r7.x, -c5, r9             ;Tangent
dp3 r7.y, -c5, r10            ;Binormal
dp3 r7.z, -c5, r11            ;Normal
mad oT2.xyz, r7.xyz, c7, c7  ;Scale and Bias for v1.1. pixel shader.

dp3 r7.x, -c6, r9             ;Tangent
dp3 r7.y, -c6, r10            ;Binormal
dp3 r7.z, -c6, r11            ;Normal
mad oT3.xyz, r7.xyz, c7, c7  ;Scale and Bias for v1.1. pixel shader.

; Texture coordinates for the base map and normal map.
mov  oT0.xy, v7.xy
mov  oT1.xy, v7.xy

