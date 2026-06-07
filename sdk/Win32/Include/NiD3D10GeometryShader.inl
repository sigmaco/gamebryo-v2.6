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
inline ID3D10GeometryShader* NiD3D10GeometryShader::GetGeometryShader() const
{
    return m_pkGeometryShader;
}
//---------------------------------------------------------------------------
inline const D3D10_SO_DECLARATION_ENTRY* 
    NiD3D10GeometryShader::GetStreamOutputDeclaration() const
{
    return m_pkSODeclaration;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10GeometryShader::GetNumStreamOutputEntries() const
{
    return m_uiNumSOEntries;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10GeometryShader::GetOutputStreamStride() const
{
    return m_uiOutputStreamStride;
}