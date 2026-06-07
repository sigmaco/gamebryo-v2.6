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
;MaterialDiffuse         8       0
;Zoze                    9   1   0

vs_1_1

dcl_position v0
dcl_normal v1

m4x4 oPos, v0, c0            ;Tranform and emit to oPos for optimization

mov  r6, c9.xxxy             ;Get eye pos in view space assumed (0,0,0,1)
m4x4 r2, r6, c4              ;Transform back into obj space

add  r2, -v0, r2             ;Find view vector

;Normalize view vector
dp3  r10.x, r2, r2      
rsq  r10.y, r10.x
mul  r2, r2, r10.y

mov  oD0.xyz, c8.xyz         ;Output material color.
dp3  r1.x, r2, v1            ;(1 - N dot V) ^ 2 to alpha
add  r1.x, -r1.x, c9.y
mul  oD0.w, r1.x, r1.x


