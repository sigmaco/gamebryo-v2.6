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
inline NiRenderedCubeMap::FaceID 
    NiRenderedCubeMap::GetCurrentCubeFace() const
{
    return m_eFace;
}
//---------------------------------------------------------------------------
inline void NiRenderedCubeMap::SetCurrentCubeFace(FaceID eFace)
{
    NIASSERT(eFace < FACE_NUM);
    m_eFace = eFace;
    m_spBuffer = m_aspFaceBuffers[eFace];
}
//---------------------------------------------------------------------------
inline Ni2DBuffer* NiRenderedCubeMap::GetFaceBuffer(FaceID eFace)
{
    NIASSERT(eFace < FACE_NUM);
    return m_aspFaceBuffers[eFace];
}
//---------------------------------------------------------------------------
