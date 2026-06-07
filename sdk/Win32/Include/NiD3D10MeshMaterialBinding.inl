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
inline D3D10_INPUT_ELEMENT_DESC* 
NiD3D10MeshMaterialBinding::GetInputElementDescArray() const
{
    return m_pkElementDescArray;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiD3D10MeshMaterialBinding::GetInputElementDescCount() const
{
    return m_uiElementCount;
}
//---------------------------------------------------------------------------
inline ID3D10InputLayout* NiD3D10MeshMaterialBinding::GetInputLayout() const
{
    return m_pkCurrentInputLayout;
}
//---------------------------------------------------------------------------
inline void NiD3D10MeshMaterialBinding::ReleaseCachedInputLayouts()
{
    NiDelete m_pkInputLayouts;
    m_pkInputLayouts = NULL;
}
//---------------------------------------------------------------------------
inline void NiD3D10MeshMaterialBinding::ReleaseElementArray()
{
    NiFree(m_pkElementDescArray);
    m_pkElementDescArray = NULL;
    m_uiElementCount = 0;
}
//---------------------------------------------------------------------------
inline const NiDataStreamRef* NiD3D10MeshMaterialBinding::GetIndexStreamRef() const
{
    return m_pkIndexStreamRef;
}
//---------------------------------------------------------------------------
inline const NiUInt16* NiD3D10MeshMaterialBinding::GetStreamsToSetArray() const
{
    return m_auiStreamsToSet;
}
//---------------------------------------------------------------------------
inline NiUInt16 NiD3D10MeshMaterialBinding::GetLastValidStream() const
{
    return m_uiLastValidStream;
}
