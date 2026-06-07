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
dcl_normal v1

;The offset is determined by taking the Y value of the object coordinates
;after it is rotated by some angle determined by c11 and c12 in the XY plane.
;This offset is then applied to object Z of the vertex.  Since the bars are 
;arranged in a circle, this results in a circular rise and fall of the bars
;along a sine wave.  The Z offset therefore is:
; Y = Vx * sin + Vy * cos
;This is just an expansion of a 2D matrix rotation around Z in the XY plane.

;Get vertical offset.  YRotated = Vx * Sine + Vy * Cosine
mul r1.x, v0.x, c12.x        ;c12 is Sine by the NSF definition
mad r1.x, v0.y, c11.x, r1.x  ;c11 is Cosine by the NSF definition
add r1.x, r1.x, c13.x

;Select if this should be used as an upward bias using CenterlineConstants
sge r2, c10.yyyy, v0.zzzz    ;if(v0.z < center.y) then r2 = 1
mul r3.x, r2.x, r1.x         ;Scale bias by 0 or 1 to effectively select.

mov r0, v0                   ;r0 is pos from here out
add r0.z, r0.z, r3.x         ;Add bias.  The sge above makes it 0 if not applied

;Now see if this should be used as a downward bias using CenterlineConstants
sge r2, v0.zzzz, c10.yyyy    ;if(v0.z > center.y) then r2 = 1
mul r3.x, r2.x, r1.x         ;Scale bias by 0 or 1
mul r3.x, r3.x, c8.z         ;Negate since this is a downward bias.  
add r0.z, r0.z, r3.x         ;Add into pos

;Output to oPos as early as possible so HW can reject triangle if needed.
;r0 still holds position in object space after the fact.
m4x4 oPos, r0, c0

add oT0.xyz, r0.zzz, -c14.xxx;If the bar is below a certain z threshold in
                             ;object space.  I want to texkill in the pixel
                             ;shader.

;r0 now has the biased bar position.  If the vert falls between 0 and 200, we
;add in up to 1 to each color component.  This will be clamped on export, so it
;won't matter if we add in on top of blue.  We use some selections to either
;multiply the bias by 1 or 0 twice.  In the end we have either 0 or the bias
;which is normalize by multiplying through by CenterlineConstants.w and used to
;scale the vertex color.  The result is that bars near each other "glow" white
;while bars further away have a dull blue look.

;Is vert above CenterlineConstants.x?
sge r2, r0.zzzz, c10.xxxx    ;if(r0.z > center.x) then r2 = 1
mul r1.xyzw, r2, r1.xxxx     ;r1 is bias or 0

;Is vert below CenterlineConstants.z?
sge r2, c10.zzzz, r0.zzzz    ;if(r0.z < center.z) then r2 = 1
mul r1, r1, r2               ;r1 is bias or 0

;Scale by CenterlineConstants.w
mul r1, r1, c10.wwww         ;Normalize r1
mov r3, c9                   ;Move mat color into r3
mad r3, r1, c8.yyyy, r3      ;Add in to each color component based off how 
                             ;close the bars are(nomalized bias)

;We now know the diffuse color of the bar in r3.  Using the view vector and the
;normal, we will simulate a Fresnel effect with 1 - (N dot V).  That factor
;will be used to modulate the already calculated vertex color in r3.  This
;gives an effect where bars viewed edge on appear solid while looking straight
;at the bars gives you translucency.

mov  r6, c8.xxxy             ;Get eye pos in view space. Assumed to be (0,0,0,1)
m4x4 r2, r6, c4              ;Transform back into obj space

add  r2, -r0, r2             ;Find view vector(V)

dp3  r10.x, r2, r2           ;Normalize V
rsq  r10.y, r10.x
mul  r2, r2, r10.y

dp3  r2, r2, v1              ;Take N dot V
max  r2, r2, c8.xxxx         ;Clamp from 0 to 1
add  r2, c8.yyyy, -r2        ;Take 1 - N dot V
mul  oD0, r2, r3             ;Modulate as diffuse color
