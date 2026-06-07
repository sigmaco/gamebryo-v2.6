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
inline NSBUserDefinedDataSet* NiBinaryShader::GetShaderUserDefinedDataSet()
{
    return m_spUDDSet_Shader;
}
//---------------------------------------------------------------------------
inline NSBUserDefinedDataSet* 
NiBinaryShader::GetImplementationUserDefinedDataSet()
{
    return m_spUDDSet_Implementation;
}
//---------------------------------------------------------------------------
inline NSBUserDefinedDataSet* NiBinaryShader::GetPassUserDefinedDataSet(
    unsigned int uiPass)
{
    if (uiPass > m_uiPassCount)
        return 0;

    return m_kUDDSet_PassArray.GetAt(uiPass);
}
//---------------------------------------------------------------------------
inline void NiBinaryShader::SetUserDefinedDataSet(
    NSBUserDefinedDataSet* pkUDDSet)
{
    m_spUDDSet_Shader = pkUDDSet;
}
//---------------------------------------------------------------------------
inline void NiBinaryShader::SetImplementationUserDefinedDataSet(
    NSBUserDefinedDataSet* pkUDDSet)
{
    m_spUDDSet_Implementation = pkUDDSet;
}
//---------------------------------------------------------------------------
inline void NiBinaryShader::SetPassUserDefinedDataSet(unsigned int uiPass, 
    NSBUserDefinedDataSet* pkUDDSet)
{
    m_kUDDSet_PassArray.SetAtGrow(uiPass, pkUDDSet);
}
//---------------------------------------------------------------------------
inline NiD3DRenderStateGroup* NiBinaryShader::GetRenderStateGroup()
{
    return m_pkRenderStateGroup;
}
//---------------------------------------------------------------------------
inline void NiBinaryShader::SetRenderStateGroup(
    NiD3DRenderStateGroup* pkRenderStateGroup)
{
    m_pkRenderStateGroup = pkRenderStateGroup;
}
//---------------------------------------------------------------------------
inline NiPlatformShaderConstantMap* NiBinaryShader::GetVertexConstantMap()
    const
{
    return m_spVertexConstantMap;
}
//---------------------------------------------------------------------------
inline void NiBinaryShader::SetVertexConstantMap(
    NiPlatformShaderConstantMap* pkSCMVertex)
{
#if defined(WIN32) || defined(_XENON)
    m_spVertexConstantMap = pkSCMVertex;
#elif defined(_PS3)
    m_aspGlobalConstantMap[NiGPUProgram::PROGRAM_VERTEX] = pkSCMVertex;
#else
#error "Undefined platform"
#endif
}
//---------------------------------------------------------------------------
inline NiPlatformShaderConstantMap* NiBinaryShader::GetPixelConstantMap() const
{
    return m_spPixelConstantMap;
}
//---------------------------------------------------------------------------
inline void NiBinaryShader::SetPixelConstantMap(
    NiPlatformShaderConstantMap* pkSCMPixel)
{
#if defined(WIN32) || defined(_XENON)
    m_spPixelConstantMap = pkSCMPixel;
#elif defined(_PS3)
    m_aspGlobalConstantMap[NiGPUProgram::PROGRAM_PIXEL] = pkSCMPixel;
#else
#error "Undefined platform"
#endif
}
//---------------------------------------------------------------------------
inline bool NiBinaryShader::InsertPass(unsigned int uiPass, 
    NiPlatformShaderPass* pkPass)
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
inline D3DDevicePtr NiBinaryShader::GetD3DDevice()
{
    return m_pkD3DDevice;
}
//---------------------------------------------------------------------------
inline NiD3DRenderer* NiBinaryShader::GetD3DRenderer()
{
    return m_pkD3DRenderer;
}
//---------------------------------------------------------------------------
inline NiD3DRenderState* NiBinaryShader::GetD3DRenderState()
{
    return m_pkD3DRenderState;
}
//---------------------------------------------------------------------------
inline unsigned int NiBinaryShader::GetBonesPerPartition() const
{
    return m_uiBonesPerPartition;
}
//---------------------------------------------------------------------------
inline void NiBinaryShader::SetBonesPerPartition(unsigned int uiBones)
{
    m_uiBonesPerPartition = uiBones;
}
//---------------------------------------------------------------------------
inline unsigned int NiBinaryShader::GetBoneMatrixRegisters() const
{
    return m_uiBoneMatrixRegisters;
}
//---------------------------------------------------------------------------
inline void NiBinaryShader::SetBoneMatrixRegisters(unsigned int uiRegisters)
{
    m_uiBoneMatrixRegisters = uiRegisters;
}
//---------------------------------------------------------------------------
inline NiBinaryShader::BoneMatrixCalcMethod 
NiBinaryShader::GetBoneCalcMethod() const
{
    return m_eBoneCalcMethod;
}
//---------------------------------------------------------------------------
inline void NiBinaryShader::SetBoneCalcMethod(
    NiBinaryShader::BoneMatrixCalcMethod eMethod)
{
    m_eBoneCalcMethod = eMethod;
}
//---------------------------------------------------------------------------
inline NiShaderRequirementDesc::NBTFlags
    NiBinaryShader::GetBinormalTangentMethod() const
{
    return m_eBinormalTangentMethod;
}
//---------------------------------------------------------------------------
inline void NiBinaryShader::SetBinormalTangentMethod(
    NiShaderRequirementDesc::NBTFlags eNBTMethod)
{
    m_eBinormalTangentMethod = eNBTMethod;
}
//---------------------------------------------------------------------------
inline unsigned int NiBinaryShader::GetBinormalTangentUVSource() 
    const
{
    return m_uiBinormalTangentUVSource;
}
//---------------------------------------------------------------------------
inline void NiBinaryShader::SetBinormalTangentUVSource(unsigned int uiSource)
{
    m_uiBinormalTangentUVSource = uiSource;
}
//---------------------------------------------------------------------------
