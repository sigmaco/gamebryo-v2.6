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
dcl_normal   v3

;WorldViewProjTranspose      0       0
;MaterialDiffuse             4       0
;Zoze                        5   1   0.0,1.0,0.0,0.5
;Light1Direction             6   1   
;Light1Color                 7   1   
;AmbientLightColor           8   1
;MaterialAmbient             9       0
;InvWorldViewTranspose       10      0

m4x4    oPos, v0, c0

;lighting

;r5 holds the deformed point in world space.  We can use that to calculate 
;specular.  r11 holds the normal.
mov  r6, c5.xxxy        ;get eye pos in view space assumed (0,0,0,1)
m4x4 r2, r6, c10        ;transform back into object space

add  r2, -v0, r2        ;find view vector

dp3  r10.x, r2, r2      ;normalize view vec
rsq  r10.y, r10.x
mul  r2, r2, r10.y

dp3  r2, r2, v3         ;Calculate N dot V.
;Place N dot V biased and scaled into oT0 for use ;in pixel shader.
mad  oT0.xyz, r2, c5.w, c5.w

mov  r1, -c6

dp3  r1, r1, v3         ;Get N dot L
max  r1, r1, c5.x       ;Clamp light to positive range.
mul  r1, r1, c7         ;Modulate by light color
mul  r1, r1, c4         ;Modulate by material color
mov  r2, c8
mad  oD0, r2, c9, r1    ;Add in ambient component.

