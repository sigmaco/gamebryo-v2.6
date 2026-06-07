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
// NiBinaryShader inline functions
//---------------------------------------------------------------------------
inline NSBD3D10UserDefinedDataSet*
    NiD3D10BinaryShader::GetShaderUserDefinedDataSet()
{
    return m_spUDDSet_Shader;
}
//---------------------------------------------------------------------------
inline NSBD3D10UserDefinedDataSet* 
    NiD3D10BinaryShader::GetImplementationUserDefinedDataSet()
{
    return m_spUDDSet_Implementation;
}
//---------------------------------------------------------------------------
inline NSBD3D10UserDefinedDataSet*
    NiD3D10BinaryShader::GetPassUserDefinedDataSet(unsigned int uiPass)
{
    if (uiPass > m_uiPassCount)
        return 0;

    return m_kUDDSet_PassArray.GetAt(uiPass);
}
//---------------------------------------------------------------------------
inline void NiD3D10BinaryShader::SetUserDefinedDataSet(
    NSBD3D10UserDefinedDataSet* pkUDDSet)
{
    m_spUDDSet_Shader = pkUDDSet;
}
//---------------------------------------------------------------------------
inline void NiD3D10BinaryShader::SetImplementationUserDefinedDataSet(
    NSBD3D10UserDefinedDataSet* pkUDDSet)
{
    m_spUDDSet_Implementation = pkUDDSet;
}
//---------------------------------------------------------------------------
inline void NiD3D10BinaryShader::SetPassUserDefinedDataSet(
    unsigned int uiPass, NSBD3D10UserDefinedDataSet* pkUDDSet)
{
    m_kUDDSet_PassArray.SetAtGrow(uiPass, pkUDDSet);
}
//---------------------------------------------------------------------------
inline void NiD3D10BinaryShader::SetRenderStateGroup(
    NiD3D10RenderStateGroup* pkRenderStateGroup)
{
    m_spRenderStateGroup = pkRenderStateGroup;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10BinaryShader::GetVertexConstantMapCount() const
{
    return m_kVertexShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
inline NiD3D10ShaderConstantMap* NiD3D10BinaryShader::GetVertexConstantMap(
    unsigned int uiIndex) const
{
    if (uiIndex < m_kVertexShaderConstantMaps.GetSize())
        return m_kVertexShaderConstantMaps.GetAt(uiIndex);
    else
        return NULL;
}
//---------------------------------------------------------------------------
inline void NiD3D10BinaryShader::SetVertexConstantMap(unsigned int uiIndex, 
    NiD3D10ShaderConstantMap* pkSCMVertex)
{
    m_kVertexShaderConstantMaps.SetAtGrow(uiIndex, pkSCMVertex);
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10BinaryShader::GetGeometryConstantMapCount() const
{
    return m_kGeometryShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
inline NiD3D10ShaderConstantMap* NiD3D10BinaryShader::GetGeometryConstantMap(
    unsigned int uiIndex) const
{
    if (uiIndex < m_kGeometryShaderConstantMaps.GetSize())
        return m_kGeometryShaderConstantMaps.GetAt(uiIndex);
    else
        return NULL;
}
//---------------------------------------------------------------------------
inline void NiD3D10BinaryShader::SetGeometryConstantMap(unsigned int uiIndex,
    NiD3D10ShaderConstantMap* pkSCMGeometry)
{
    m_kGeometryShaderConstantMaps.SetAtGrow(uiIndex, pkSCMGeometry);
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10BinaryShader::GetPixelConstantMapCount() const
{
    return m_kPixelShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
inline NiD3D10ShaderConstantMap* NiD3D10BinaryShader::GetPixelConstantMap(
    unsigned int uiIndex) const
{
    if (uiIndex < m_kPixelShaderConstantMaps.GetSize())
        return m_kPixelShaderConstantMaps.GetAt(uiIndex);
    else
        return NULL;
}
//---------------------------------------------------------------------------
inline void NiD3D10BinaryShader::SetPixelConstantMap(unsigned int uiIndex, 
    NiD3D10ShaderConstantMap* pkSCMPixel)
{
    m_kPixelShaderConstantMaps.SetAtGrow(uiIndex, pkSCMPixel);
}
//---------------------------------------------------------------------------
inline bool NiD3D10BinaryShader::InsertPass(unsigned int uiPass, 
    NiD3D10Pass* pkPass)
{
    // Increment the pass if there was none there previously
    if (m_kPasses.GetAllocatedSize() <= uiPass ||
        m_kPasses.GetAt(uiPass) == 0)
    {
        m_uiPassCount++;
    }

    m_kPasses.SetAtGrow(uiPass, pkPass);
    
    m_kUDDSet_PassArray.SetSize(m_uiPassCount);

    return true;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10BinaryShader::GetBonesPerPartition() const
{
    return m_uiBonesPerPartition;
}
//---------------------------------------------------------------------------
inline void NiD3D10BinaryShader::SetBonesPerPartition(unsigned int uiBones)
{
    m_uiBonesPerPartition = uiBones;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10BinaryShader::GetBoneMatrixRegisters() const
{
    return m_uiBoneMatrixRegisters;
}
//---------------------------------------------------------------------------
inline void NiD3D10BinaryShader::SetBoneMatrixRegisters(
    unsigned int uiRegisters)
{
    m_uiBoneMatrixRegisters = uiRegisters;
}
//---------------------------------------------------------------------------
inline NiD3D10BinaryShader::BoneMatrixCalcMethod
    NiD3D10BinaryShader::GetBoneCalcMethod() const
{
    return m_eBoneCalcMethod;
}
//---------------------------------------------------------------------------
inline void NiD3D10BinaryShader::SetBoneCalcMethod(
    NiD3D10BinaryShader::BoneMatrixCalcMethod eMethod)
{
    m_eBoneCalcMethod = eMethod;
}
//---------------------------------------------------------------------------
inline NiShaderRequirementDesc::NBTFlags
    NiD3D10BinaryShader::GetBinormalTangentMethod()
    const
{
    return m_eBinormalTangentMethod;
}
//---------------------------------------------------------------------------
inline void NiD3D10BinaryShader::SetBinormalTangentMethod(
    NiShaderRequirementDesc::NBTFlags eNBTMethod)
{
    m_eBinormalTangentMethod = eNBTMethod;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10BinaryShader::GetBinormalTangentUVSource() 
    const
{
    return m_uiBinormalTangentUVSource;
}
//---------------------------------------------------------------------------
inline void NiD3D10BinaryShader::SetBinormalTangentUVSource(
    unsigned int uiSource)
{
    m_uiBinormalTangentUVSource = uiSource;
}
//---------------------------------------------------------------------------
