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

; Shader version 1.1
vs_1_1

dcl_position        v0
dcl_normal          v3
dcl_texcoord        v7

; Transform position to clip space and output it
m4x4 oPos, v0, c0

; Just copy the color (decal)
; Use the passed in constant for the color (red)
mov oD0, c4

; Just copy the texture coords
mov oT0, v7
