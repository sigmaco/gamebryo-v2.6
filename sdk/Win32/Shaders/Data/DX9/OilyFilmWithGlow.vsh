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

;Thin Film shader.  We're going to calculate the specular intensity and 
;pass that into the pixel shader to use as a scalar of a lookup into the 
;thin film ramp.  Diffuse lighting is done as well.  These values will 
;go through oD0 and oD1.  N dot V is passed through as a texture coordinate
;to be used in the pixel shader.

;Constant map from the NSF.  The last four constants for the lights are
;globals that should be updated by the application.  Exponent 
;is an attribute that hang off the object.
;
;WorldViewProjTranspose  0       0
;InvWorldViewTranspose   4       0
;Exponent                8   1   0
;Zoze                    10  1   0.0, 1.0, 0.0, 1.0
;InvWorldTranspose       11      0
;MaterialSpecular        15      0
;Light1Direction         16  1   
;Light2Direction         17  1   
;Light1Color             18  1   
;Light2Color             19  1   
;MaterialDiffuse         20      0


vs_1_1


dcl_position    v0
dcl_normal      v1
dcl_texcoord0   v2

m4x4 oPos, v0, c0            ;Tranform and emit to oPos for optimization

mov  r6, c10.xxxy            ;Get eye pos in view space assumed (0,0,0,1)
m4x4 r2, r6, c4              ;Transform back into obj space

add  r2, -v0, r2             ;Find view vector

;Normalize view vector
dp3  r10.x, r2, r2      
rsq  r10.y, r10.x
mul  r2, r2, r10.y

;Light 1
;We have N and V.  We need L to use the lit instruction.
mov  r1, -c16                 ;Get light direction in local space

;Get the half vector.  
add  r3.xyz, r1, r2          ;H = L + V
dp3  r10.x, r3, r3           ;Normalize half vector
rsq  r10.y, r10.x
mul  r3.xyz, r3, r10.y

;r1 is L; r2 is V; r3 is half vector

dp3  r4.x, r1, v1            ;Place N dot L into r4.x
dp3  r4.y, r3, v1            ;Place N dot H into r4.y
mov  r4.zw, c8.xx            ;Move in specular exponent
lit  r4, r4                  ;Calc diffuse and specular

mul  r5, r4.yyyy, c18        ;Multiply out diffuse color.
mul  r7, r4.zzzw, c18        ;Copy out specular.  

;Light 2
;We have N and V.  We need L to use the lit instruction.
mov  r1, -c17                 ;Get light direction in local space

;Get the half vector
add  r3.xyz, r1, r2          ;H = L + V
dp3  r10.x, r3, r3           ;Normalize half vector
rsq  r10.y, r10.x
mul  r3.xyz, r3, r10.y

;r1 is L; r2 is V; r3 is half vector

dp3  r4.x, r1, v1            ;Place N dot L into r4.x
dp3  r4.y, r3, v1            ;Place N dot H into r4.y
mov  r4.zw, c8.xx            ;Move in specular exponent
lit  r4, r4                  ;Calc diffuse and specular

mad  r5, r4.yyyy, c19, r5   ;Add in 2nd light times color
mul  oD0, r5, c20           ;Modulate by diffuse material.
mad  r7, c19, r4.zzzw, r7  ;mad in second specular.  
mul  oD1, r7, c15           ;Modulate by specular material.

mov  oT0, v2                 ;Output packed texture coordinates
dp3  oT1.xyz, v1, r2         ;Output N dot V
