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

;Luminance transfer shader.  We're going to calculate the
;intensity and pass that into the pixel shader.
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
;MaterialDiffuse         8       0
;Zoze                    9   1   0.0,1.0,0.0,1.0
;YUVConversion           10  1   0.299, 0.587, 0.114, 1.0

m4x4 oPos, v0, c0       ;tranform and emit to oPos for optimization

mov oT1.xy, v2          ;Get base map coords.

;Light 1
dp3  r1, c4, c4         ;normalize light vector
rsq  r2, r1
mul  r1, r2, c4

dp3  r1, r1, v1         ;Get N dot L
max  r1, r1, c9.x       ;Clamp light to positive range.
mul  r1, r1, c6         ;Modulate by light color
mul  r3, r1, c8         ;Modulate by material color

;Light 2
dp3  r1, c5, c5         ;normalize light vector
rsq  r2, r1
mul  r1, r2, c5

dp3  r1, r1, v1         ;Get N dot L
max  r1, r1, c9.x       ;Clamp light to positive range.
mul  r1, r1, c7         ;Modulate by light color
mad  r1.xyz, r1, c8, r3 ;Modulate by material color

dp3  oT0.xyz, r1, c10   ;Do the YUV luminance conversion.
