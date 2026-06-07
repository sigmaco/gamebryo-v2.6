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
//	Alpha Texture Blender
//---------------------------------------------------------------------------
//	Vertex shader for blending 2 textures together by a third.
//
//  This shader computes position, and passes through texture coordinates.
//  The vertex packing is specified in the inputs section. 
//---------------------------------------------------------------------------

; INPUTS:
; v0  -- position
; v1  -- tex coord 0
; v2  -- tex coord 1
; v3  -- tex coord 2
;
; CONSTANTS:
; c0 - c3   world/view/pos matrix
;
; OUTPUTS:
; oD0       diffusely lit color
; oD1       vertex colors with opacity information
; oT0 - oT3 textures

vs_1_1

dcl_position    v0
dcl_texcoord0   v1
dcl_texcoord1   v2
dcl_texcoord2   v3

; transform the vertex 
m4x4  oPos, v0, c0

;copy over texture coords
mov oT0,  v1
mov oT1,  v2
mov oT2,  v3
