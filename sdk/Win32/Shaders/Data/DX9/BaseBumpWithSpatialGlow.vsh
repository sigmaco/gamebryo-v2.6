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

;The base-glow-bump effect from the alien temple.  Most of the lighting is
;done in the associated pixel shader since the surface is normal mapped.  

;Constant map from the NSF.  The first two matrices are pretty straightforward.
;The GlowLightPosition and light directions are global constants that are 
;wired to lights with application controllers.  GlowAttenuation hangs off the
;object and is artist controlled.  The fourth component of Zoze controls the
;fraction of the GlowAttenuation distance inside which there is no attenuation.
;This gives us a really hot glow in the middle that tapers off quickly.
;
;WorldViewProjTranspose  0       0
;InvWorldTranspose       4       0
;GlowLightPosition       8   1   
;Zoze                    9   1   0.0, 1.0, 0.0, 0.3
;GlowAttenuation         10  1   0
;Light1Direction         11  1   
;Light2Direction         12  1   

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
mov  oT1, v2      
mov  oT2, v2      

;Output vertex color
mov  oD0, v5

;Find light position and vector in object space for the glow calculations.
mov r5, c8                   ;Get the light position in world space.
m4x4 r4, r5, c4              ;Transform to object space with inverse world mat
add r0, r4, -v0              ;Get the light vector in object space.

;Get the distance squared in r3.x and 1/d in r2.x.  We'll need those values
;for the glow clamping, attenuation, etc...
dp3 r3.x, r0, r0
rsq r2.x, r3.x

;This is a fairly confusing block of code.  The glow value is attenuated using
;GlowAttenuation, referred to as G.  The distance from the GlowLightPosition
;is referred to as s.  We use s because it is not simply the distance but is
;Max(0, (d - (G.w * Zoze.w)).  The final value placed in oD1 for
;multiplication in the pixel shader is 1 / (G.x(s^2) + G.y(s) + 1).  This
;is standard attenuation except that by shifting the value towards zero, no
;attenuation is applied if the point is close to the GlowLightPosition giving
;a very hot glow near that position.  Note also that G.w could be precomputed
;to contain G.w * Zoze.w.  Originally, G.w was used to clamp the glow, but 
;that functionality was removed in favor of the current approach.  It would
;require changes to all the art, and since we use mad to do the work, it would
;save us no shader instructions.

rcp r2.x, r2.x                ;d in r2.x
mov r7, c10                   ;glowAttenuation in r7
mad r2.y, r7.w, -c9.w, r2.x   ;s = (glowClamp * -hotSpotFraction) + d
max r2.y, r2.y, c9.x          ;Clamp s to positive range.
mul r3.x, r2.y, r2.y          ;Get s^2 in r3.x
mul r3.x, r3.x, c10.x         ;Apply the quadratic attenuation constant
mad r3.x, r2.y, c10.y, r3.x   ;Apply the linear attenuation constant
add r3.x, r3.x, c9.y          ;Add one for constant attenuation and no clamping
rcp oD1.xyzw, r3.xxxx         ;Take 1 / attenuation factor and place in oD0
                              ;for multiplying in the pixel shader.

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
