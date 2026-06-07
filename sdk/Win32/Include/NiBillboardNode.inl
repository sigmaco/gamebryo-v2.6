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
//  NiBillboardNode inline functions

//---------------------------------------------------------------------------
inline NiBillboardNode::NiBillboardNode() :
    m_uFlags(0)
{
    SetMode(ROTATE_ABOUT_UP);
    SetUpdateControllers(true);
}
//---------------------------------------------------------------------------
inline void NiBillboardNode::SetMode(FaceMode eMode)
{
    SetField(NiInt32ToUInt16(eMode), FACE_MODE_MASK, FACE_MODE_POS);
}
//---------------------------------------------------------------------------
inline NiBillboardNode::FaceMode NiBillboardNode::GetMode() const
{
    return (FaceMode) GetField(FACE_MODE_MASK, FACE_MODE_POS);
}
//---------------------------------------------------------------------------
inline void NiBillboardNode::SetUpdateControllers(bool)
{
    SetBit(true, UPDATECONTROLLERS_MASK);
}
//---------------------------------------------------------------------------
inline bool NiBillboardNode::GetUpdateControllers() const
{
    return GetBit(UPDATECONTROLLERS_MASK);
}
//---------------------------------------------------------------------------
