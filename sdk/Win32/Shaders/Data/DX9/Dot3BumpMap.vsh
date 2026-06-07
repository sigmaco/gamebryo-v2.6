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

;A diffuse bump map effect with one directional and one ambient light
;
;Constant map from the NSF.  
;
;WorldViewProjTranspose  0  0
;DirLight1Direction      7  1
;DirLight2Direction      8  1
;AmbLightColor           9  1   
;MaterialAmbient         10  0
;MaterialDiffuse         11  0

vs_1_1

dcl_position    v0
dcl_normal      v1
dcl_texcoord0   v2
dcl_tangent     v3
dcl_binormal    v4

; Output transformed vertex into device coordinates.
m4x4 oPos, v0, c0

; Output texture coordinates.
mov  oT0.xyzw, v2.xyyy      
mov  oT1.xyzw, v2.xyyy      

; Find light vector in object space
mov r0, -c7  ;Get the light 1 direction in model space.
mov r1, -c8  ;Get the light 2 direction in model space.

;Transform the light vector into tangent space so we can use in the pixel
;shader.
dp3 r2.x, r0, v3             ;Tangent
dp3 r2.y, r0, v4             ;Binormal
dp3 r2.z, r0, v1             ;Normal
mad oT2.xyz, r2, c12, c12    ;Scale and bias since texcoord in the pixel shader
                             ;expects a range of 0 to 1.

dp3 r2.x, r1, v3             ;Tangent
dp3 r2.y, r1, v4             ;Binormal
dp3 r2.z, r1, v1             ;Normal
mad oT3.xyz, r2, c12, c12    ;Scale and bias since texcoord in the pixel shader
                             ;expects a range of 0 to 1.

;Ambient Lighting - rest will be done in ps
mov r1, c10                  ;Move in the ambient material so we can multiply
mul oD0, r1, c9              ;Modulate the ambient light by material
mov oD1, c11                 ;Place the diffuse material in oD1 for use in 
                             ;the pixel shader.

