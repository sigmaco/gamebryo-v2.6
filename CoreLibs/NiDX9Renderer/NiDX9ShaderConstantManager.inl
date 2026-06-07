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
inline bool NiDX9ShaderConstantManager::SetVertexShaderConstantB(
    unsigned int uiStartRegister, const BOOL* pbConstantData, 
    unsigned int uiBoolCount, bool bSave)
{
    if (bSave)
    {
        if ((uiStartRegister + uiBoolCount) > m_uiNumBoolVSConstants)
            return false;

        unsigned int uiByteSize = uiBoolCount * sizeof(*pbConstantData);

        NiMemcpy(m_pbSavedBoolVSConstantData + uiStartRegister, 
            m_pbBoolVSConstantData + uiStartRegister, uiByteSize);
    }
    return SetVertexShaderConstantB(uiStartRegister, pbConstantData, 
        uiBoolCount);
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::SetVertexShaderConstantB(
    unsigned int uiStartRegister, const BOOL* pbConstantData, 
    unsigned int uiBoolCount)
{
    if ((uiStartRegister + uiBoolCount) > m_uiNumBoolVSConstants)
        return false;

    unsigned int uiByteSize = uiBoolCount * sizeof(*pbConstantData);

    NiMemcpy(m_pbBoolVSConstantData + uiStartRegister,
        pbConstantData, uiByteSize);

    if (m_uiFirstDirtyBoolVSReg > uiStartRegister)
        m_uiFirstDirtyBoolVSReg = uiStartRegister;
    if (m_uiFirstCleanBoolVSReg < uiStartRegister + uiBoolCount)
        m_uiFirstCleanBoolVSReg = uiStartRegister + uiBoolCount;

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::GetVertexShaderConstantB(
    unsigned int uiStartRegister, BOOL* pbConstantData, 
    unsigned int uiBoolCount)
{
    if ((uiStartRegister + uiBoolCount) > m_uiNumBoolVSConstants)
        return false;

    unsigned int uiByteSize = uiBoolCount * sizeof(*pbConstantData);
    NiMemcpy(pbConstantData, m_pbBoolVSConstantData + uiStartRegister,
        uiByteSize);

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::RestoreVertexShaderConstantB(
    unsigned int uiStartRegister, unsigned int uiBoolCount)
{
    if ((uiStartRegister + uiBoolCount) > m_uiNumBoolVSConstants)
        return false;

    unsigned int uiByteSize = uiBoolCount * sizeof(*m_pbBoolVSConstantData);
    NiMemcpy(m_pbBoolVSConstantData + uiStartRegister,
        m_pbSavedBoolVSConstantData + uiStartRegister, uiByteSize);

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::SetVertexShaderConstantF(
    unsigned int uiStartRegister, const float* pfConstantData, 
    unsigned int uiFloat4Count, bool bSave)
{
    if (bSave)
    {
        if ((uiStartRegister + uiFloat4Count) > m_uiNumFloatVSConstants)
            return false;

        unsigned int uiBase = uiStartRegister * 4;
        unsigned int uiByteSize = uiFloat4Count * 4 * sizeof(*pfConstantData);

        NiMemcpy(m_pfSavedFloatVSConstantData + uiBase, 
            m_pfFloatVSConstantData + uiBase, uiByteSize);
    }
    return SetVertexShaderConstantF(uiStartRegister, pfConstantData,
        uiFloat4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::SetVertexShaderConstantF(
    unsigned int uiStartRegister, const float* pfConstantData, 
    unsigned int uiFloat4Count)
{
    if ((uiStartRegister + uiFloat4Count) > m_uiNumFloatVSConstants)
        return false;

    unsigned int uiBase = uiStartRegister * 4;
    unsigned int uiByteSize = uiFloat4Count * 4 * sizeof(*pfConstantData);

    NiMemcpy(m_pfFloatVSConstantData + uiBase, pfConstantData, uiByteSize);

    if (m_uiFirstDirtyFloatVSReg > uiStartRegister)
        m_uiFirstDirtyFloatVSReg = uiStartRegister;
    if (m_uiFirstCleanFloatVSReg < uiStartRegister + uiFloat4Count)
        m_uiFirstCleanFloatVSReg = uiStartRegister + uiFloat4Count;

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::GetVertexShaderConstantF(
    unsigned int uiStartRegister, float* pfConstantData, 
    unsigned int uiFloat4Count)
{
    if ((uiStartRegister + uiFloat4Count) > m_uiNumFloatVSConstants)
        return false;

    unsigned int uiByteSize = uiFloat4Count * 4 *
        sizeof(*m_pfFloatVSConstantData);
    NiMemcpy(pfConstantData, m_pfFloatVSConstantData + uiStartRegister * 4,
        uiByteSize);

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::RestoreVertexShaderConstantF(
    unsigned int uiStartRegister, unsigned int uiFloat4Count)
{
    if ((uiStartRegister + uiFloat4Count) > m_uiNumFloatVSConstants)
        return false;

    unsigned int uiByteSize = uiFloat4Count * 4 * 
        sizeof(*m_pfFloatVSConstantData);
    NiMemcpy(m_pfFloatVSConstantData + uiStartRegister * 4, 
        m_pfSavedFloatVSConstantData + uiStartRegister * 4, uiByteSize);

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::SetVertexShaderConstantI(
    unsigned int uiStartRegister, const int* piConstantData, 
    unsigned int uiInt4Count, bool bSave)
{
    if (bSave)
    {
        if ((uiStartRegister + uiInt4Count) > m_uiNumIntVSConstants)
            return false;

        unsigned int uiBase = uiStartRegister * 4;
        unsigned int uiByteSize = uiInt4Count * 4 * sizeof(*piConstantData);

        NiMemcpy(m_piSavedIntVSConstantData + uiBase,
            m_piIntVSConstantData + uiBase, uiByteSize);
    }
    return SetVertexShaderConstantI(uiStartRegister, piConstantData, 
        uiInt4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::SetVertexShaderConstantI(
    unsigned int uiStartRegister, const int* piConstantData, 
    unsigned int uiInt4Count)
{
    if ((uiStartRegister + uiInt4Count) > m_uiNumIntVSConstants)
        return false;

    unsigned int uiBase = uiStartRegister * 4;
    unsigned int uiByteSize = uiInt4Count * 4 * sizeof(*piConstantData);

    NiMemcpy(m_piIntVSConstantData + uiBase, piConstantData, uiByteSize);

    if (m_uiFirstDirtyIntVSReg > uiStartRegister)
        m_uiFirstDirtyIntVSReg = uiStartRegister;
    if (m_uiFirstCleanIntVSReg < uiStartRegister + uiInt4Count)
        m_uiFirstCleanIntVSReg = uiStartRegister + uiInt4Count;

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::GetVertexShaderConstantI(
    unsigned int uiStartRegister, int* piConstantData, 
    unsigned int uiInt4Count)
{
    if ((uiStartRegister + uiInt4Count) > m_uiNumIntVSConstants)
        return false;

    unsigned int uiByteSize = uiInt4Count * 4 * 
        sizeof(*m_piIntVSConstantData);
    NiMemcpy(piConstantData, m_piIntVSConstantData + uiStartRegister * 4, 
        uiByteSize);

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::RestoreVertexShaderConstantI(
    unsigned int uiStartRegister, unsigned int uiInt4Count)
{
    if ((uiStartRegister + uiInt4Count) > m_uiNumIntVSConstants)
        return false;

    unsigned int uiByteSize = uiInt4Count * 4 * 
        sizeof(*m_piIntVSConstantData);
    NiMemcpy(m_piIntVSConstantData + uiStartRegister * 4,
        m_piSavedIntVSConstantData + uiStartRegister * 4, uiByteSize);

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::SetPixelShaderConstantB(
    unsigned int uiStartRegister, const BOOL* pbConstantData, 
    unsigned int uiBoolCount, bool bSave)
{
    if (bSave)
    {
        if ((uiStartRegister + uiBoolCount) > m_uiNumBoolPSConstants)
            return false;

        unsigned int uiByteSize = uiBoolCount * sizeof(*pbConstantData);

        NiMemcpy(m_pbSavedBoolPSConstantData + uiStartRegister,  
            m_pbBoolPSConstantData + uiStartRegister, uiByteSize);
    }
    return SetPixelShaderConstantB(uiStartRegister, pbConstantData, 
        uiBoolCount);
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::SetPixelShaderConstantB(
    unsigned int uiStartRegister, const BOOL* pbConstantData, 
    unsigned int uiBoolCount)
{
    if ((uiStartRegister + uiBoolCount) > m_uiNumBoolPSConstants)
        return false;

    unsigned int uiByteSize = uiBoolCount * sizeof(*pbConstantData);

    NiMemcpy(m_pbBoolPSConstantData + uiStartRegister,
        pbConstantData, uiByteSize);

    if (m_uiFirstDirtyBoolPSReg > uiStartRegister)
        m_uiFirstDirtyBoolPSReg = uiStartRegister;
    if (m_uiFirstCleanBoolPSReg < uiStartRegister + uiBoolCount)
        m_uiFirstCleanBoolPSReg = uiStartRegister + uiBoolCount;

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::GetPixelShaderConstantB(
    unsigned int uiStartRegister, BOOL* pbConstantData, 
    unsigned int uiBoolCount)
{
    if ((uiStartRegister + uiBoolCount) > m_uiNumBoolPSConstants)
        return false;

    unsigned int uiByteSize = uiBoolCount * sizeof(*pbConstantData);
    NiMemcpy(pbConstantData, m_pbBoolPSConstantData + uiStartRegister, 
        uiByteSize);

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::RestorePixelShaderConstantB(
    unsigned int uiStartRegister, unsigned int uiBoolCount)
{
    if ((uiStartRegister + uiBoolCount) > m_uiNumBoolPSConstants)
        return false;

    unsigned int uiByteSize = uiBoolCount * sizeof(*m_pbBoolPSConstantData);
    NiMemcpy(m_pbBoolPSConstantData + uiStartRegister,
        m_pbSavedBoolPSConstantData + uiStartRegister, uiByteSize);

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::SetPixelShaderConstantF(
    unsigned int uiStartRegister, const float* pfConstantData, 
    unsigned int uiFloat4Count, bool bSave)
{
    if (bSave)
    {
        if ((uiStartRegister + uiFloat4Count) > m_uiNumFloatPSConstants)
            return false;

        unsigned int uiBase = uiStartRegister * 4;
        unsigned int uiByteSize = uiFloat4Count * 4 * sizeof(*pfConstantData);

        NiMemcpy(m_pfSavedFloatPSConstantData + uiBase, 
            m_pfFloatPSConstantData + uiBase, uiByteSize);
    }
    return SetPixelShaderConstantF(uiStartRegister, pfConstantData, 
        uiFloat4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::SetPixelShaderConstantF(
    unsigned int uiStartRegister, const float* pfConstantData, 
    unsigned int uiFloat4Count)
{
    if ((uiStartRegister + uiFloat4Count) > m_uiNumFloatPSConstants)
        return false;

    unsigned int uiBase = uiStartRegister * 4;
    unsigned int uiByteSize = uiFloat4Count * 4 * sizeof(*pfConstantData);

    NiMemcpy(m_pfFloatPSConstantData + uiBase, pfConstantData, uiByteSize);

    if (m_uiFirstDirtyFloatPSReg > uiStartRegister)
        m_uiFirstDirtyFloatPSReg = uiStartRegister;
    if (m_uiFirstCleanFloatPSReg < uiStartRegister + uiFloat4Count)
        m_uiFirstCleanFloatPSReg = uiStartRegister + uiFloat4Count;

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::GetPixelShaderConstantF(
    unsigned int uiStartRegister, float* pfConstantData, 
    unsigned int uiFloat4Count)
{
    if ((uiStartRegister + uiFloat4Count) > m_uiNumFloatPSConstants)
        return false;

    unsigned int uiByteSize = uiFloat4Count * 4 * 
        sizeof(*m_pfFloatPSConstantData);
    NiMemcpy(pfConstantData, m_pfFloatPSConstantData + uiStartRegister * 4, 
        uiByteSize);

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::RestorePixelShaderConstantF(
    unsigned int uiStartRegister, unsigned int uiFloat4Count)
{
    if ((uiStartRegister + uiFloat4Count) > m_uiNumFloatPSConstants)
        return false;

    unsigned int uiByteSize = uiFloat4Count * 4 * 
        sizeof(*m_pfFloatPSConstantData);
    NiMemcpy(m_pfFloatPSConstantData + uiStartRegister * 4, 
        m_pfSavedFloatPSConstantData + uiStartRegister * 4, uiByteSize);

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::SetPixelShaderConstantI(
    unsigned int uiStartRegister, const int* piConstantData, 
    unsigned int uiInt4Count, bool bSave)
{
    if (bSave)
    {
        if ((uiStartRegister + uiInt4Count) > m_uiNumIntPSConstants)
            return false;

        unsigned int uiBase = uiStartRegister * 4;
        unsigned int uiByteSize = uiInt4Count * 4 * sizeof(*piConstantData);

        NiMemcpy(m_piSavedIntPSConstantData + uiBase, 
            m_piIntPSConstantData + uiBase, uiByteSize);
    }
    return SetPixelShaderConstantI(uiStartRegister, piConstantData, 
        uiInt4Count);
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::SetPixelShaderConstantI(
    unsigned int uiStartRegister, const int* piConstantData, 
    unsigned int uiInt4Count)
{
    if ((uiStartRegister + uiInt4Count) > m_uiNumIntPSConstants)
        return false;

    unsigned int uiBase = uiStartRegister * 4;
    unsigned int uiByteSize = uiInt4Count * 4 * sizeof(*piConstantData);

    NiMemcpy(m_piIntPSConstantData + uiBase, piConstantData, uiByteSize);

    if (m_uiFirstDirtyIntPSReg > uiStartRegister)
        m_uiFirstDirtyIntPSReg = uiStartRegister;
    if (m_uiFirstCleanIntPSReg < uiStartRegister + uiInt4Count)
        m_uiFirstCleanIntPSReg = uiStartRegister + uiInt4Count;

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::GetPixelShaderConstantI(
    unsigned int uiStartRegister, int* piConstantData, 
    unsigned int uiInt4Count)
{
    if ((uiStartRegister + uiInt4Count) > m_uiNumIntPSConstants)
        return false;

    unsigned int uiByteSize = uiInt4Count * 4 * 
        sizeof(*m_piIntPSConstantData);
    NiMemcpy(piConstantData, m_piIntPSConstantData + 
        uiStartRegister * 4, uiByteSize);

    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9ShaderConstantManager::RestorePixelShaderConstantI(
    unsigned int uiStartRegister, unsigned int uiInt4Count)
{
    if ((uiStartRegister + uiInt4Count) > m_uiNumIntPSConstants)
        return false;

    unsigned int uiByteSize = uiInt4Count * 4 * 
        sizeof(*m_piIntPSConstantData);
    NiMemcpy(m_piIntPSConstantData + uiStartRegister * 4, 
        m_piSavedIntPSConstantData + uiStartRegister * 4, uiByteSize);

    return true;
}
//---------------------------------------------------------------------------
inline void NiDX9ShaderConstantManager::FastOverwriteVertexShaderConstantB(
    unsigned int uiStartRegister, const BOOL* pbConstantData, 
    unsigned uiBoolCount)
{
    BOOL* pbDest = m_pbBoolVSConstantData + 4 * uiStartRegister;
    while (uiBoolCount--)
        *pbDest++ = *pbConstantData++;
}
//---------------------------------------------------------------------------
inline void NiDX9ShaderConstantManager::MarkVertexShaderConstantsDirtyB(
    unsigned int uiFirstDirtyRegister, unsigned int uiFirstCleanRegister)
{
    if (m_uiFirstDirtyBoolVSReg >= uiFirstDirtyRegister)
        m_uiFirstDirtyBoolVSReg = uiFirstDirtyRegister;
    if (m_uiFirstCleanBoolVSReg < uiFirstCleanRegister)
        m_uiFirstCleanBoolVSReg = uiFirstCleanRegister;
}
//---------------------------------------------------------------------------
inline void NiDX9ShaderConstantManager::FastOverwriteVertexShaderConstantF(
    unsigned int uiStartRegister, const float* pfConstantData, 
    unsigned uiFloat4Count)
{
    float* pfDest = m_pfFloatVSConstantData + 4 * uiStartRegister;
    unsigned int uiTotalFloatCount = 4 * uiFloat4Count;
    while (uiTotalFloatCount--)
        *pfDest++ = *pfConstantData++;
}
//---------------------------------------------------------------------------
inline void NiDX9ShaderConstantManager::MarkVertexShaderConstantsDirtyF(
    unsigned int uiFirstDirtyRegister, unsigned int uiFirstCleanRegister)
{
    if (m_uiFirstDirtyFloatVSReg >= uiFirstDirtyRegister)
        m_uiFirstDirtyFloatVSReg = uiFirstDirtyRegister;
    if (m_uiFirstCleanFloatVSReg < uiFirstCleanRegister)
        m_uiFirstCleanFloatVSReg = uiFirstCleanRegister;
}
//---------------------------------------------------------------------------
inline void NiDX9ShaderConstantManager::FastOverwriteVertexShaderConstantI(
    unsigned int uiStartRegister, const int* piConstantData, 
    unsigned uiInt4Count)
{
    int* piDest = m_piIntVSConstantData + 4 * uiStartRegister;
    unsigned int uiTotalIntCount = 4 * uiInt4Count;
    while (uiTotalIntCount--)
        *piDest++ = *piConstantData++;
}
//---------------------------------------------------------------------------
inline void NiDX9ShaderConstantManager::MarkVertexShaderConstantsDirtyI(
    unsigned int uiFirstDirtyRegister, unsigned int uiFirstCleanRegister)
{
    if (m_uiFirstDirtyIntVSReg >= uiFirstDirtyRegister)
        m_uiFirstDirtyIntVSReg = uiFirstDirtyRegister;
    if (m_uiFirstCleanIntVSReg < uiFirstCleanRegister)
        m_uiFirstCleanIntVSReg = uiFirstCleanRegister;
}
//---------------------------------------------------------------------------
inline void NiDX9ShaderConstantManager::FastOverwritePixelShaderConstantB(
    unsigned int uiStartRegister, const BOOL* pbConstantData, 
    unsigned uiBoolCount)
{
    BOOL* pbDest = m_pbBoolPSConstantData + 4 * uiStartRegister;
    while (uiBoolCount--)
        *pbDest++ = *pbConstantData++;
}
//---------------------------------------------------------------------------
inline void NiDX9ShaderConstantManager::MarkPixelShaderConstantsDirtyB(
    unsigned int uiFirstDirtyRegister, unsigned int uiFirstCleanRegister)
{
    if (m_uiFirstDirtyBoolPSReg >= uiFirstDirtyRegister)
        m_uiFirstDirtyBoolPSReg = uiFirstDirtyRegister;
    if (m_uiFirstCleanBoolPSReg < uiFirstCleanRegister)
        m_uiFirstCleanBoolPSReg = uiFirstCleanRegister;
}
//---------------------------------------------------------------------------
inline void NiDX9ShaderConstantManager::FastOverwritePixelShaderConstantF(
    unsigned int uiStartRegister, const float* pfConstantData, 
    unsigned uiFloat4Count)
{
    float* pfDest = m_pfFloatPSConstantData + 4 * uiStartRegister;
    unsigned int uiTotalFloatCount = 4 * uiFloat4Count;
    while (uiTotalFloatCount--)
        *pfDest++ = *pfConstantData++;
}
//---------------------------------------------------------------------------
inline void NiDX9ShaderConstantManager::MarkPixelShaderConstantsDirtyF(
    unsigned int uiFirstDirtyRegister, unsigned int uiFirstCleanRegister)
{
    if (m_uiFirstDirtyFloatPSReg >= uiFirstDirtyRegister)
        m_uiFirstDirtyFloatPSReg = uiFirstDirtyRegister;
    if (m_uiFirstCleanFloatPSReg < uiFirstCleanRegister)
        m_uiFirstCleanFloatPSReg = uiFirstCleanRegister;
}
//---------------------------------------------------------------------------
inline void NiDX9ShaderConstantManager::FastOverwritePixelShaderConstantI(
    unsigned int uiStartRegister, const int* piConstantData, 
    unsigned uiInt4Count)
{
    int* piDest = m_piIntPSConstantData + 4 * uiStartRegister;
    unsigned int uiTotalIntCount = 4 * uiInt4Count;
    while (uiTotalIntCount--)
        *piDest++ = *piConstantData++;
}
//---------------------------------------------------------------------------
inline void NiDX9ShaderConstantManager::MarkPixelShaderConstantsDirtyI(
    unsigned int uiFirstDirtyRegister, unsigned int uiFirstCleanRegister)
{
    if (m_uiFirstDirtyIntPSReg >= uiFirstDirtyRegister)
        m_uiFirstDirtyIntPSReg = uiFirstDirtyRegister;
    if (m_uiFirstCleanIntPSReg < uiFirstCleanRegister)
        m_uiFirstCleanIntPSReg = uiFirstCleanRegister;
}
//---------------------------------------------------------------------------
