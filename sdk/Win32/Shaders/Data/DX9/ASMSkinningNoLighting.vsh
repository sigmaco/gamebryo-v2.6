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
; This method for performing vertex shader skinning does not transform the
; normal, because there's no transform available that can be used to do
; lighting correctly. For this reason, only the material color will be used.
;
; Constants specified by the app
;       c0-3 = SkinWorldViewProj
;       c4   = Material color
;       c5   = constants - 1.0f, 0.0f, 1.0f, 1020.01f
;       c10+ = bone matrices (without bone-to-skin-to-world transform)
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
; Assemble the transform from the bones' contributions
;----------------------------------------------------------------------------
; Get the index of the bone matrix [0]
mov     a0.x, r1.x
; Calculate the contribution of the second bone
mul     r2, r0.x, c[a0.x + 0 + 10]
mul     r3, r0.x, c[a0.x + 1 + 10]
mul     r4, r0.x, c[a0.x + 2 + 10]

; Get the index of the bone matrix [1]
mov     a0.x, r1.y
; Add in the contribution of the second bone
mad     r2, r0.y, c[a0.x + 0 + 10], r2
mad     r3, r0.y, c[a0.x + 1 + 10], r3
mad     r4, r0.y, c[a0.x + 2 + 10], r4

; Get the index of the bone matrix [2]
mov     a0.x, r1.z
; Add in the contribution of the third bone
mad     r2, r0.z, c[a0.x + 0 + 10], r2
mad     r3, r0.z, c[a0.x + 1 + 10], r3
mad     r4, r0.z, c[a0.x + 2 + 10], r4

; Get the index of the bone matrix [3]
mov     a0.x, r1.w
; Add in the contribution of the fourth bone
mad     r2, r0.w, c[a0.x+0+10], r2
mad     r3, r0.w, c[a0.x+1+10], r3
mad     r4, r0.w, c[a0.x+2+10], r4

;----------------------------------------------------------------------------
; Transform the position using the resulting matrix
;----------------------------------------------------------------------------
dp4     r5.x, v0, r2
dp4     r5.y, v0, r3
dp4     r5.z, v0, r4
mov     r5.w, c5.x
m4x4    oPos, r5, c0

;----------------------------------------------------------------------------
; No lighting, just the material color
;----------------------------------------------------------------------------
mov     oD0, c4

;----------------------------------------------------------------------------
; Just pass texture coordinates through
;----------------------------------------------------------------------------
mov     oT0.xy, v7.xy
