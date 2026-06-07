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
inline NiUInt16 NiPhysXMaterialDesc::GetIndex() const
{
    return m_usIndex;
}
//---------------------------------------------------------------------------
inline void NiPhysXMaterialDesc::SetIndex(const NiUInt16 usIndex)
{
    m_usIndex = usIndex;
}
//---------------------------------------------------------------------------
inline void NiPhysXMaterialDesc::ToMaterialDesc(NxMaterialDesc& kMaterialDesc,
    const NiUInt32 uiIndex) const
{
    kMaterialDesc = m_pkMaterialDescs[uiIndex];
}
//---------------------------------------------------------------------------
inline void NiPhysXMaterialDesc::ToMaterial(NxMaterial* pkMaterial,
    const NiUInt32 uiIndex) const
{
    NxMaterialDesc kMatDesc = m_pkMaterialDescs[uiIndex];
    pkMaterial->loadFromDesc(kMatDesc);
}
//---------------------------------------------------------------------------
inline void NiPhysXMaterialDesc::FromMaterial(NxMaterial* pkMaterial,
    const NiUInt32 uiIndex)
{
    NxMaterialDesc kMatDesc;
    pkMaterial->saveToDesc(kMatDesc);
    FromMaterialDesc(kMatDesc, uiIndex);
}
//---------------------------------------------------------------------------
inline bool NiPhysXMaterialDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
