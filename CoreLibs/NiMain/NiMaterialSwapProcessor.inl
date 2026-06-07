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
inline NiMaterialSwapProcessor::NiMaterialSwapProcessor(NiMaterial* pkMaterial,
    unsigned int uiMaterialExtraData) : m_spMaterial(pkMaterial),
    m_uiMaterialExtraData(uiMaterialExtraData)
{
}
//---------------------------------------------------------------------------
inline void NiMaterialSwapProcessor::SetMaterial(NiMaterial* pkMaterial,
    unsigned int uiMaterialExtraData)
{
    m_spMaterial = pkMaterial;
    m_uiMaterialExtraData = uiMaterialExtraData;
}
//---------------------------------------------------------------------------
inline NiMaterial* NiMaterialSwapProcessor::GetMaterial() const
{
    return m_spMaterial;
}
//---------------------------------------------------------------------------
inline unsigned int NiMaterialSwapProcessor::GetMaterialExtraData() const
{
    return m_uiMaterialExtraData;
}
//---------------------------------------------------------------------------
