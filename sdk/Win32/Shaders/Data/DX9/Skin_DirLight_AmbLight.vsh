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
dcl_texcoord0 v4

;
; v0 = position
; v1 = weights
; v2 = matrix indices
; v3 = normals
; v4 = texture coords
;
; c0-3 = ViewProj
; c4   = Material Diffuse color
; c5   = constants - 1,1,1,765.01
; c6   = Dir Light 1 vector in world space
; c7   = Dir Light 1 color
; c8   = Amb Light color

;Determine the 4th weight.  We only pack 3, and this insures that the weights
;sum to 1.  We'll keep weights in r0.
mov  r0.xyz, v1.xyz          ; r0.xyz = w0,w1,w2
dp3  r0.w, v1.xyz, c5.xxxx   ; r0.w = w0 + w1 + w2
add  r0.w, -r0.w, c5.x       ; r0.2 = 1 - (w0 + w1 + w2) = w3

;Need to offset the values.  The indices are packed as floats from 0 to 1, but
;they are originally packed as bytes.  Since each bone matrix in this shader 
;takes 3 constant registers, we scale each float by 765.01 to make it equal
;the original byte value times 3.  The .01 insures that any floating point
;rounding does not bias our results incorrectly downward.
mul  r1, v2.zyxw, c5.wwww    ; r1 = indices w/ offset (D3DCOLOR Version)

;We now have the weights in r0 and the indices in r1.  We will move each 
;index into a0.x and blend a matrix together with the weights.  The registers
;r2, r3, and r4 will hold this final 3x4 matrix which we will use to transform
;points into world space as well as normals.  The points will end up in world
;space because each BoneMatrix3 represents a transform from bind pose skin
;space to bone space and the current transform from bone space to world space
;as well as transforms to account for motion of the skinned mesh independent
;of the bones.

;First bone
mov  a0.x, r1.x
mul  r2, r0.x, c[a0.x+0+18]
mul  r3, r0.x, c[a0.x+1+18]
mul  r4, r0.x, c[a0.x+2+18]

;Second bone
mov  a0.x, r1.y
mad  r2, r0.y, c[a0.x+0+18], r2
mad  r3, r0.y, c[a0.x+1+18], r3
mad  r4, r0.y, c[a0.x+2+18], r4

;Third bone
mov  a0.x, r1.z
mad  r2, r0.z, c[a0.x+0+18], r2
mad  r3, r0.z, c[a0.x+1+18], r3
mad  r4, r0.z, c[a0.x+2+18], r4

;Fourth bone
mov  a0.x, r1.w
mad  r2, r0.w, c[a0.x+0+18], r2
mad  r3, r0.w, c[a0.x+1+18], r3
mad  r4, r0.w, c[a0.x+2+18], r4

;The vertex will now be deformed into world space with the blended matrix in
;r2, r3, and r4.
dp4  r5.x, v0, r2
dp4  r5.y, v0, r3
dp4  r5.z, v0, r4
mov  r5.w, c5.x

;This line applies the ViewProj matrix to the deformed position.  As
;stated above, the verts are in world space.  
m4x4	oPos, r5, c0

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

;lighting

;r5 holds the deformed point in world space.  We can use that to calculate 
;specular.  r11 holds the normal.
mov  r6, c16.xxxy       ;get eye pos in view space assumed (0,0,0,1)
m4x4 r2, r6, c12         ;transform back into world space

add  r2, -r5, r2        ;find inv view vector

dp3  r10.x, r2, r2      ;normalize inv view vec
rsq  r10.y, r10.x
mul  r2, r2, r10.y

;r2 now holds the view vector.  We need to get the half vector.
add  r3.xyz, -c6, r2         ;L + V
dp3  r10.x, r3, r3      ;normalize half vector
rsq  r10.y, r10.x
mul  r3.xyz, r3, r10.y

;c6 is L.  r11 is N, r3 is H.
dp3  r4.x, r11, -c6       ;place N dot L into r4.x
dp3  r4.y, r11, r3       ;place N dot H into r4.y
mov  r4.zw, c17.xx       ;get specular exponent
lit  r4, r4             ;get diffuse and specular

mul  r6, r4.yyyy, c4
mul  r6, r6, c7
add  r6, r6, c11
mov  r1, c9
mad  oD0, r1, c8, r6

mul  oD1, r4.zzzw, c10

; Texture coordinates for the base map.
mov  oT0.xy, v4.xy

