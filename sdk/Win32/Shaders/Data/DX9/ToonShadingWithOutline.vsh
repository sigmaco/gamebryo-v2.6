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

;Toon shader.  Implement simple toon shading using a luminance transfer.
;
;Packing Definition
;
dcl_position    v0
dcl_normal      v1
dcl_texcoord0   v2

;Constant map from NSF
;
;WorldViewProjTranspose  0       0
;Light1Direction         4   1   
;Light2Direction         5   1   
;Light1Color             6   1   
;Light2Color             7   1   
;Zoze                    8   1   0.0,1.0,0.0,1.0
;YUVConversion           9   1   0.299, 0.587, 0.114, 1.0
;MaterialDiffuse         10      0
;InvWorldTranspose       11      0
;InvWorldViewTranspose   15      0

m4x4 oPos, v0, c0       ;tranform and emit to oPos for optimization

mov oT0.xyzw, v2          ;Get base map coords.

mov oD0, c10            ;Pass material diffuse through since NSF can't set
                        ;material for pixel shader constant map

mov  r6, c8.xxxy        ;get eye pos in view space assumed (0,0,0,1)
m4x4 r2, r6, c15        ;transform back into object space

add  r2, -v0, r2        ;find view vector

dp3  r10.x, r2, r2      ;normalize view vec
rsq  r10.y, r10.x
mul  r2, r2, r10.y

dp3  r2, r2, v1         ;Calculate N dot V.
;Place N dot V biased and scaled into oT0 for use in pixel shader.
mad  oT2.xyzw, r2, c8.w, c8.w

;Light 1
mov  r1, -c4            ;Load model space light vector
dp3  r1, r1, v1         ;Get N dot L
max  r1, r1, c8.x       ;Clamp light to positive range.
mul  r3, r1, c6         ;Modulate by light color

;Light 2
mov  r1, -c5            ;Load model space light vector
dp3  r1, r1, v1         ;Get N dot L
max  r1, r1, c8.x       ;Clamp light to positive range.
mad  r1.xyz, r1, c7, r3 ;Modulate by light color

dp3  oT1.xyz, r1, c9    ;Do the YUV luminance conversion.
