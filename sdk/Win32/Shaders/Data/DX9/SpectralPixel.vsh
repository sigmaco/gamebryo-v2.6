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

;Constant map from the NSF.  
;
;WorldViewProjTranspose  0       0
;InvWorldViewTranspose   4       0
;Zoze                    8   1   0

vs_1_1

dcl_position v0
dcl_normal v1
dcl_texcoord0 v2
dcl_tangent v3
dcl_binormal v4

m4x4 oPos, v0, c0            ;Tranform and emit to oPos for optimization

mov  oT0.xy, v2              ;Output tex coordinates

mov  r6, c8.xxxy             ;Get eye pos in view space assumed (0,0,0,1)
m4x4 r2, r6, c4              ;Transform back into obj space

add  r2, -v0, r2             ;Find view vector

;Normalize view vector
dp3  r10.x, r2, r2      
rsq  r10.y, r10.x
mul  r2.xyz, r2, r10.y

;Convert to Tangent Space
dp3  r3.x, r2, v3
dp3  r3.y, r2, v4
dp3  r3.z, r2, v1

;Bias for 1.1 pixel shader
mad  oT1.xyz, r3.xyz, c8.w, c8.w

