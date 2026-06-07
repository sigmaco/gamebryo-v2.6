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

;Demonstrating parallax mapping.  This shader is basically the same as the
;base-bump-glow effect from Eturnum.  The math related to the glow effect
;is removed, and the eye vector is now passed into the pixel shader via
;oT2.  Normal mapping is performed in the pixel shader as well.  The glow
;map is removed in the pixel shader as well.

;Constant map from the NSF.  The first two matrices are pretty straightforward.
;The light directions are global constants that are 
;wired to lights with application controllers.  
;
;WorldViewProjTranspose  0       0
;InvWorldTranspose       4       0
;Zoze                    9   1   0.0, 1.0, 0.0, 0.5
;Light1Direction         11  1   
;Light2Direction         12  1   
;InvWorldViewTranspose   13      0

vs_1_1

dcl_position    v0
dcl_normal      v1
dcl_texcoord0   v2 
dcl_tangent     v3
dcl_binormal    v4
dcl_color       v5

;Output transformed vertex
m4x4 oPos, v0, c0

;Output packed texture coordinates
mov  oT0, v2      

;Output vertex color
mov  oD0, v5

;Transform eye position into object space.
mov r0, c9.xxxy
m4x4 r1, r0, c13

;r1 has eye pos in object space.  We get the eye vector.
add r0, r1, -v0
;add r0, -r1, v0

;Normalize the eye vector
dp3 r1.x, r0, r0
rsq r1.x, r1.x
mul r0, r0, r1.x

;Transform eye vector into tangent space.
dp3 oT2.x, r0, v3
dp3 oT2.y, r0, v4
dp3 oT2.z, r0, v1
;mov oT2.w, c9.y

;We assume here that light vectors are normalized.  We'll transform them into
;tangent space to use for dot3 bump mapping in the pixel shader.

;Light1Direction
mov r3, -c11
dp3 oT3.x, r3, v3            ;Tangent
dp3 oT3.y, r3, v4            ;Binormal
dp3 oT3.z, r3, v1            ;Normal

;Light2Direction
mov r3, -c12
dp3 oT4.x, r3, v3            ;Tangent
dp3 oT4.y, r3, v4            ;Binormal
dp3 oT4.z, r3, v1            ;Normal
