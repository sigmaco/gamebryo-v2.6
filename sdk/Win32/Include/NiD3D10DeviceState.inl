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
             
inline void NiD3D10DeviceState::VSGetShader(
    ID3D10VertexShader** ppkVertexShader)
    const
{
    if (ppkVertexShader)
        *ppkVertexShader = m_pkVertexShader;
}
//---------------------------------------------------------------------------
inline void NiD3D10DeviceState::GSGetShader(
    ID3D10GeometryShader** ppkGeometryShader)
    const
{
    if (ppkGeometryShader)
        *ppkGeometryShader = m_pkGeometryShader;
}
//---------------------------------------------------------------------------
inline void NiD3D10DeviceState::PSGetShader(
     ID3D10PixelShader** ppkPixelShader) const
{
    if (ppkPixelShader)
        *ppkPixelShader = m_pkPixelShader;
}
//---------------------------------------------------------------------------
inline ID3D10VertexShader* NiD3D10DeviceState::VSGetShader() const 
{ 
    return m_pkVertexShader; 
}       
//---------------------------------------------------------------------------
inline ID3D10GeometryShader* NiD3D10DeviceState::GSGetShader() const  
{ 
    return m_pkGeometryShader; 
}    
//---------------------------------------------------------------------------
inline ID3D10PixelShader* NiD3D10DeviceState::PSGetShader() const  
{ 
    return m_pkPixelShader; 
}
//---------------------------------------------------------------------------
