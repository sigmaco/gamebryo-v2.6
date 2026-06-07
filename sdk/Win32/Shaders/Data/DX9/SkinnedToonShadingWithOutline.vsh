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

;Skinned Toon shader. Implement simple toon shading using a luminance transfer.
;
;Packing Definition

dcl_position        v0
dcl_blendweight     v1
dcl_blendindices    v2
dcl_normal          v3
dcl_texcoord        v7

;Constant map from NSF
;
;ViewProjTranspose       0       0
;Light1Direction         4   1   
;Light2Direction         5   1   
;Light1Color             6   1   
;Light2Color             7   1   
;Constants               8   1   1.0, 0.0, 0.5, 765.01
;YUVConversion           9   1   0.299, 0.587, 0.114, 1.0
;MaterialDiffuse         10      0
;InvWorldTranspose       11      0
;InvWorldViewTranspose   15      0
;SkinBoneMatrix3         19  0   20

; Skinning section

; Determine the 4th weight
mov     r0.xyz, v1.xyz			; r0.xyz = w0,w1,w2
dp3     r0.w, v1.xyz, c8.xxxx	; r0.w = w0 + w1 + w2
add     r0.w, -r0.w, c8.x       ; r0.2 = 1 - (w0 + w1 + w2) = w3

; Need to offset the values
mul     r1, v2.zyxw, c8.wwww     ; r1 = indices w/ offset (D3DCOLOR Version)

; Transform the position and normal for each bone

; First bone
mov		a0.x, r1.x
; Transform position and normal
m4x3	r5, v0, c[a0.x + 19]
m3x3	r6, v3, c[a0.x + 19]
; Blend them
mul		r5.xyz, r5.xyz, r0.xxx
mul		r6.xyz, r6.xyz, r0.xxx

; Second bone
mov		a0.x, r1.y
; Transform position and normal
m4x3	r2, v0, c[a0.x + 19]
m3x3	r3, v3, c[a0.x + 19]
; Blend them
mad		r5.xyz, r2.xyz, r0.yyy, r5.xyz
mad		r6.xyz, r3.xyz, r0.yyy, r6.xyz

; Third bone
mov		a0.x, r1.z
; Transform position and normal
m4x3	r2, v0, c[a0.x + 19]
m3x3	r3, v3, c[a0.x + 19]
; Blend them
mad		r5.xyz, r2.xyz, r0.zzz, r5.xyz
mad		r6.xyz, r3.xyz, r0.zzz, r6.xyz

; Fourth bone
mov		a0.x, r1.w
; Transform position and normal
m4x3	r2, v0, c[a0.x + 19]
m3x3	r3, v3, c[a0.x + 19]
; Blend them
mad		r5.xyz, r2, r0.wwww, r5
mad		r6.xyz, r3, r0.wwww, r6

; View, projection
mov		r5.w, c8.x
m4x4	oPos, r5, c0

;Convert normal to object space
m3x3    r0, r6, c11

; Normalize the normal
dp3		r11.x, r0.xyz, r0.xyz       ; r11.x = length of normal
rsq		r11.xyz, r11.x              ; r11.xyz = 1/sqrt(length of normal)
mul		r0.xyz, r0.xyz, r11.xyz     ; r0 = normalized normal

mov oT0.xy, v7          ;Get base map coords.

; Toon Shading section

mov oD0, c10            ;Pass material diffuse through since NSF can't set
                        ;material for pixel shader constant map

mov  r6, c8.yyyx        ;get eye pos in view space assumed (0,0,0,1)
m4x4 r2, r6, c15        ;transform back into object space

add  r2, -r5, r2        ;find view vector

dp3  r10.x, r2, r2      ;normalize view vec
rsq  r10.y, r10.x
mul  r2, r2, r10.y

dp3  r2, r2, r0         ;Calculate N dot V.
;Place N dot V biased and scaled into oT0 for use in pixel shader.
mad  oT2.xyz, r2, c8.z, c8.z

;Light 1
mov  r4, -c4            ;Load world space light vector
m3x3 r1, r4, c11        ;Convert light vector to object space


dp3  r1, r1, r0         ;Get N dot L
max  r1, r1, c8.y       ;Clamp light to positive range.
mul  r3, r1, c6         ;Modulate by light color

;Light 2
mov  r4, -c5            ;Load world space light vector
m3x3 r1, r4, c11        ;Convert light vector to object space

dp3  r1, r1, r0         ;Get N dot L
max  r1, r1, c8.y       ;Clamp light to positive range.
mad  r1.xyz, r1, c7, r3 ;Modulate by light color

dp3  oT1.xyz, r1, c9    ;Do the YUV luminance conversion.
