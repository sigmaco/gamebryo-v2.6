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
inline NiTexture* NiFlipController::GetTexture() const
{ 
    return (NiTexture*) m_kTextures.GetAt(m_uiIndex); 
}
//---------------------------------------------------------------------------
inline unsigned int NiFlipController::GetIndex() const 
{ 
    return m_uiIndex; 
}
//---------------------------------------------------------------------------
inline unsigned int NiFlipController::GetTextureArraySize() const 
{ 
    return m_kTextures.GetSize(); 
}
//---------------------------------------------------------------------------
inline bool NiFlipController::InterpTargetIsCorrectType(NiObjectNET* pkTarget)
    const
{
    return NiIsKindOf(NiTexturingProperty, pkTarget);
}
//---------------------------------------------------------------------------
