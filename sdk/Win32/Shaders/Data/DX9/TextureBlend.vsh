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

//---------------------------------------------------------------------------
//	Texture Blend Vertex Shader
//---------------------------------------------------------------------------
//	Vertex shader for blending 3 textures together by their vertex colors
//
//  This shader attenuates up to 8 lights and their diffuse colors. It also
//  supports the object's material color and 1 ambient light. The vertex 
//  packing is specified in the inputs section. The constants are also 
//  specified below. The important things to note here is that the attenuated
//  lighting code can be dropped into other shaders easily, although 8 lights
//  will take up a fair bit of your instruction count. The MOST important 
//  aspect of this shader is that it exports out the lit coloring through
//  oD0 and the vertex colors, which are used in the texture blending, are 
//  sent through the oD1 channel. These vertex colors are normalized relative
//  to each other to add up to an overall opacity of 1.0. 
//---------------------------------------------------------------------------

; INPUTS:
; v0  -- position
; v3  -- normal
; v5  -- vertex color
; v7  -- tex coord 0
; v8  -- tex coord 1
; v9  -- tex coord 2
; v10 -- tex coord 3
;
; CONSTANTS:
; c0 - c3   world/view/pos matrix
; c4        (0.0, 1.0, 0.0, 0.0)

; c5        CV_AMBIENT_COLOR
; c6        CV_MATERIAL_COLOR

; c7        CV_LIGHT1_POSITION
; c8        CV_LIGHT1_DIFFUSE
; c9        CV_LIGHT1_ATTENUATION

; c10       CV_LIGHT2_POSITION
; c11       CV_LIGHT2_DIFFUSE
; c12       CV_LIGHT2_ATTENUATION

; c13       CV_LIGHT3_POSITION
; c14       CV_LIGHT3_DIFFUSE
; c15       CV_LIGHT3_ATTENUATION

; c16       CV_LIGHT4_POSITION
; c17       CV_LIGHT4_DIFFUSE
; c18       CV_LIGHT4_ATTENUATION

; c19       CV_LIGHT5_POSITION
; c20       CV_LIGHT5_DIFFUSE
; c21       CV_LIGHT5_ATTENUATION

; c22       CV_LIGHT6_POSITION
; c23       CV_LIGHT6_DIFFUSE
; c24       CV_LIGHT6_ATTENUATION

; c25       CV_LIGHT7_POSITION
; c26       CV_LIGHT7_DIFFUSE
; c27       CV_LIGHT7_ATTENUATION

; c28       CV_LIGHT8_POSITION
; c29       CV_LIGHT8_DIFFUSE
; c30       CV_LIGHT8_ATTENUATION


; c50       scaling factor for vertex colors
;           Set to 0 if the texture does not exist

; OUTPUTS:
; oD0       diffusely lit color
; oD1       vertex colors with opacity information
; oT0 - oT3 textures

vs_1_1

dcl_position    v0
dcl_normal      v3
dcl_color       v5
dcl_texcoord0   v7
dcl_texcoord1   v8
dcl_texcoord2   v9
dcl_texcoord3   v10

; transform the vertex 
m4x4  oPos, v0, c0

; ************ LIGHT #1 *****************
; compute the light vector
sub   r0, c7, v0
dp3   r1, r0, r0
rsq   r1, r1
mul   r0, r0, r1.x
; compute the light distance
rcp   r1, r1
; compute n dot l
dp3   r2, r0, v3
max   r2, r2, c4.x
; compute effect of linear attenutaion
mul   r3, c9.y, r1
; compute effect of quadratic attenuation
mul   r4, r1, r1        ; light distance squared
mad   r3, r4, c9.z, r3
; add in effect of constant attenuation
add   r3, r3, c9.x
; invert to get the attenuation factor
rcp   r3, r3
; n dot l * attenuation
mul   r5, r2, r3
; multiply in the light color
mul   r6, r5, c8

; ************ LIGHT #2 *****************
; compute the light vector
sub   r0, c10, v0
dp3   r1, r0, r0
rsq   r1, r1
mul   r0, r0, r1.x
; compute the light distance
rcp   r1, r1
; compute n dot l
dp3   r2, r0, v3
max   r2, r2, c4.x
; compute effect of linear attenutaion
mul   r3, c12.y, r1
; compute effect of quadratic attenuation
mul   r4, r1, r1        ; light distance squared
mad   r3, r4, c12.z, r3
; add in effect of constant attenuation
add   r3, r3, c12.x
; invert to get the attenuation factor
rcp   r3, r3
; n dot l * attenuation
mul   r5, r2, r3
; multiply & accumulate in the light color
mad   r6, r5, c11, r6

; ************ LIGHT #3 *****************
; compute the light vector
sub   r0, c13, v0
dp3   r1, r0, r0
rsq   r1, r1
mul   r0, r0, r1.x
; compute the light distance
rcp   r1, r1
; compute n dot l
dp3   r2, r0, v3
max   r2, r2, c4.x
; compute effect of linear attenutaion
mul   r3, c15.y, r1
; compute effect of quadratic attenuation
mul   r4, r1, r1        ; light distance squared
mad   r3, r4, c15.z, r3
; add in effect of constant attenuation
add   r3, r3, c15.x
; invert to get the attenuation factor
rcp   r3, r3
; n dot l * attenuation
mul   r5, r2, r3
; multiply & accumulate in the light color
mad   r6, r5, c14, r6

; ************ LIGHT #4 *****************
; compute the light vector
sub   r0, c16, v0
dp3   r1, r0, r0
rsq   r1, r1
mul   r0, r0, r1.x
; compute the light distance
rcp   r1, r1
; compute n dot l
dp3   r2, r0, v3
max   r2, r2, c4.x
; compute effect of linear attenutaion
mul   r3, c18.y, r1
; compute effect of quadratic attenuation
mul   r4, r1, r1        ; light distance squared
mad   r3, r4, c18.z, r3
; add in effect of constant attenuation
add   r3, r3, c18.x
; invert to get the attenuation factor
rcp   r3, r3
; n dot l * attenuation
mul   r5, r2, r3
; multiply & accumulate in the light color
mad   r6, r5, c17, r6

; ************ LIGHT #5 *****************
; compute the light vector
sub   r0, c19, v0
dp3   r1, r0, r0
rsq   r1, r1
mul   r0, r0, r1.x
; compute the light distance
rcp   r1, r1
; compute n dot l
dp3   r2, r0, v3
max   r2, r2, c4.x
; compute effect of linear attenutaion
mul   r3, c21.y, r1
; compute effect of quadratic attenuation
mul   r4, r1, r1        ; light distance squared
mad   r3, r4, c21.z, r3
; add in effect of constant attenuation
add   r3, r3, c21.x
; invert to get the attenuation factor
rcp   r3, r3
; n dot l * attenuation
mul   r5, r2, r3
; multiply & accumulate in the light color
mad   r6, r5, c20, r6

; ************ LIGHT #6 *****************
; compute the light vector
sub   r0, c22, v0
dp3   r1, r0, r0
rsq   r1, r1
mul   r0, r0, r1.x
; compute the light distance
rcp   r1, r1
; compute n dot l
dp3   r2, r0, v3
max   r2, r2, c4.x
; compute effect of linear attenutaion
mul   r3, c24.y, r1
; compute effect of quadratic attenuation
mul   r4, r1, r1        ; light distance squared
mad   r3, r4, c24.z, r3
; add in effect of constant attenuation
add   r3, r3, c24.x
; invert to get the attenuation factor
rcp   r3, r3
; n dot l * attenuation
mul   r5, r2, r3
; multiply & accumulate in the light color
mad   r6, r5, c23, r6

; ************ LIGHT #7 *****************
; compute the light vector
sub   r0, c25, v0
dp3   r1, r0, r0
rsq   r1, r1
mul   r0, r0, r1.x
; compute the light distance
rcp   r1, r1
; compute n dot l
dp3   r2, r0, v3
max   r2, r2, c4.x
; compute effect of linear attenutaion
mul   r3, c27.y, r1
; compute effect of quadratic attenuation
mul   r4, r1, r1        ; light distance squared
mad   r3, r4, c27.z, r3
; add in effect of constant attenuation
add   r3, r3, c27.x
; invert to get the attenuation factor
rcp   r3, r3
; n dot l * attenuation
mul   r5, r2, r3
; multiply & accumulate in the light color
mad   r6, r5, c26, r6

; ************ LIGHT #8 *****************
; compute the light vector
sub   r0, c28, v0
dp3   r1, r0, r0
rsq   r1, r1
mul   r0, r0, r1.x
; compute the light distance
rcp   r1, r1
; compute n dot l
dp3   r2, r0, v3
max   r2, r2, c4.x
; compute effect of linear attenutaion
mul   r3, c30.y, r1
; compute effect of quadratic attenuation
mul   r4, r1, r1        ; light distance squared
mad   r3, r4, c30.z, r3
; add in effect of constant attenuation
add   r3, r3, c30.x
; invert to get the attenuation factor
rcp   r3, r3
; n dot l * attenuation
mul   r5, r2, r3
; multiply & accumulate in the light color
mad   r6, r5, c29, r6

; multiply in the material color
mul   r6, r6, c6
; Produce final vert color = 
; ambient + (diffuse material color)(Sum (attenuation*n dot l* light color))
add   oD0, r6, c5

;scale opacity information
mul r1, v5, c50
dp3 r2.x, r1, c4.yyyy
rcp r2.x, r2.x
mul oD1, r1, r2.xxxx

;copy over texture coords
mov oT0,  v7
mov oT1,  v8
mov oT2,  v9
mov oT3,  v10
