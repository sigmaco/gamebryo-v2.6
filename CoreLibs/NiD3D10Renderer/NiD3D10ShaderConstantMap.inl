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


//---------------------------------------------------------------------------
inline void NiD3D10ShaderConstantMap::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiD3D10ShaderConstantMap::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline NiD3D10DataStream* NiD3D10ShaderConstantMap::
    GetShaderConstantDataStream()  const
{
    return m_spShaderConstantDataStream;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiD3D10ShaderConstantMap::
    GetShaderConstantBufferIndex() const
{
    return m_uiShaderConstantBufferIndex;
}
//---------------------------------------------------------------------------
inline void NiD3D10ShaderConstantMap::SetConstantBufferObsolete()
{
    m_bConstantBufferCurrent = false;
}

//---------------------------------------------------------------------------
inline const void* NiD3D10ShaderConstantMap::ObtainGlobalConstantValue(
    NiShaderConstantMapEntry* pkEntry, const NiRenderCallContext&)
{
    NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();
    NiGlobalConstantEntry* pkGlobalEntry = 
        pkFactory->GetGlobalShaderConstantEntry(pkEntry->GetKey());
    if (pkGlobalEntry)
        return pkGlobalEntry->GetDataSource();
    else
        return pkEntry->GetDataSource();
}
//---------------------------------------------------------------------------
inline bool NiD3D10ShaderConstantMap::IsConstantBufferCurrent() const
{
    return m_bConstantBufferCurrent;
}
//---------------------------------------------------------------------------
