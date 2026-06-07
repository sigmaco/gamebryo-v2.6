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
// NSBD3D10Implementation inline functions
//---------------------------------------------------------------------------
inline const char* NSBD3D10Implementation::GetName()
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline const char* NSBD3D10Implementation::GetDesc()
{
    return m_pcDesc;
}
//---------------------------------------------------------------------------
inline const char* NSBD3D10Implementation::GetClassName()
{
    return m_pcClassName;
}
//---------------------------------------------------------------------------
inline const char* NSBD3D10Implementation::GetPackingDef()
{
    return m_pcPackingDef;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Implementation::GetIndex()
{
    return m_uiIndex;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Implementation::SetIndex(unsigned int uiIndex)
{
    m_uiIndex = uiIndex;
}
//---------------------------------------------------------------------------
inline NSBD3D10UserDefinedDataSet*
    NSBD3D10Implementation::GetUserDefinedDataSet()
{
    return m_spUserDefinedDataSet;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Implementation::SetUserDefinedDataSet(
    NSBD3D10UserDefinedDataSet* pkUDDSet)
{
    m_spUserDefinedDataSet = pkUDDSet;
}
//---------------------------------------------------------------------------
inline bool NSBD3D10Implementation::ReleaseShader()
{
    bool bRet = (m_spShader != NULL);
    m_spShader = NULL;
    return bRet;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Implementation::GetVertexConstantMapCount()
{
    return m_kVertexShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Implementation::GetGeometryConstantMapCount()
{
    return m_kGeometryShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Implementation::GetPixelConstantMapCount()
{
    return m_kPixelShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
inline const NiSemanticAdapterTable&
    NSBD3D10Implementation::GetSemanticAdapterTable() const
{
    return m_kAdapterTable;
}
//---------------------------------------------------------------------------
inline NiSemanticAdapterTable&
    NSBD3D10Implementation::GetSemanticAdapterTable()
{
    return m_kAdapterTable;
}
//---------------------------------------------------------------------------
