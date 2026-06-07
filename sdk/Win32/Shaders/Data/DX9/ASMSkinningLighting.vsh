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

;----------------------------------------------------------------------------
; This sample assumes that the partitioning will be done with 20 bones per
; partition, and 4 bones per vertex. This is a condition of the vertex shader
; which will need to be enforced in the ShaderDeclaration associated with the
; partitioning.
; The options for dealing with this are writing a single shader which handles
; the maximum number of bones you will partition your objects with.
;
; One of the issues with indexed palette skinning via the vertex shader 
; pipeline is lighting. For this sample, we are using the first light found
; on the object.
;
; Constants specified by the app
;       c0-3 = ViewProj
;       c4   = World-space light direction
;       c5   = constants - 1.0f, 0.0f, 1.0f, 1020.01f
;       c10+ = bone matrices (with bone-to-skin-to-world transform)
;
; Vertex components (as specified in the vertex DECL)
;       v0  = pVertex[i].position
;       v1  = pVertex[i].blendweights
;       v2  = pVertex[i].blendindices
;       v3  = pVertex[i].normal
;       v7  = pVertex[i].texturecoords
;
;----------------------------------------------------------------------------

dcl_position        v0
dcl_blendweight     v1
dcl_blendindices    v2
dcl_normal          v3
dcl_texcoord        v7

;----------------------------------------------------------------------------
; Determine the last blending weight
;----------------------------------------------------------------------------
mov     r0.xyz, v1.xyz              ; r0.xyz = w0, w1, w2
dp3     r0.w, v1.xyz, c5.xxxx       ; r0.w = w0 + w1 + w2
add     r0.w, -r0.w, c5.x           ; r0.w = 1 - (w0 + w1 + w2) = w3

;----------------------------------------------------------------------------
; Decode the index - it is swizzled due to the usage of D3DCOLOR for 
;       packing the indices...
;----------------------------------------------------------------------------
mul     r1, v2.zyxw, c5.wwww        ; r1 = indices w/ offset

;----------------------------------------------------------------------------
; Transform the position and normal for each bone
; v0 = position
; v3 = normal
;----------------------------------------------------------------------------
; Get the index of the bone matrix [0]
mov     a0.x, r1.x
; Transform position and normal
m4x3    r5, v0, c[a0.x + 10]
m3x3    r6, v3, c[a0.x + 10]
; Blend them
mul     r5.xyz, r5.xyz, r0.xxx
mul     r6.xyz, r6.xyz, r0.xxx

; Get the index of the bone matrix [1]
mov     a0.x, r1.y
; Transform position and normal
m4x3    r2, v0, c[a0.x + 10]
m3x3    r3, v3, c[a0.x + 10]
; Blend them
mad     r5.xyz, r2.xyz, r0.yyy, r5.xyz
mad     r6.xyz, r3.xyz, r0.yyy, r6.xyz

; Get the index of the bone matrix [2]
mov     a0.x, r1.z
; Transform position and normal
m4x3    r2, v0, c[a0.x + 10]
m3x3    r3, v3, c[a0.x + 10]
; Blend them
mad     r5.xyz, r2.xyz, r0.zzz, r5.xyz
mad     r6.xyz, r3.xyz, r0.zzz, r6.xyz

; Get the index of the bone matrix [3]
mov     a0.x, r1.w
; Transform position and normal
m4x3    r2, v0, c[a0.x + 10]
m3x3    r3, v3, c[a0.x + 10]
; Blend them
mad     r5.xyz, r2, r0.wwww, r5
mad     r6.xyz, r3, r0.wwww, r6

;----------------------------------------------------------------------------
; Transform the resulting position into world space
;----------------------------------------------------------------------------
mov     r5.w, c5.x
m4x4    oPos, r5, c0

;----------------------------------------------------------------------------
; Normalize the normal
;----------------------------------------------------------------------------
dp3     r11.x, r6.xyz, r6.xyz       ; r11.x = length of normal
rsq     r11.xyz, r11.x              ; r11.xyz = 1/sqrt(length of normal)
mul     r6.xyz, r6.xyz, r11.xyz     ; r6 = normalized normal

;----------------------------------------------------------------------------
; Simple directional lighting
;----------------------------------------------------------------------------
dp3     r8.x, r6.xyz, -c4
max     oD0.xyzw, c5.y, r8.x        ; Output color

;----------------------------------------------------------------------------
; Just pass texture coordinates through
;----------------------------------------------------------------------------
mov     oT0.xy, v7.xy
