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
// NiD3DTextureStageGroup inline functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
inline void NiD3DTextureStageGroup::SetD3DRenderState(NiD3DRenderState* pkRS)
{
    ms_pkD3DRenderState = pkRS;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DTextureStageGroup::GetTotalStageStateCount() const
{
    return m_uiNumStages + m_uiNumSavedStages;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DTextureStageGroup::GetNoSaveStageStateCount() const
{
    return m_uiNumStages;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DTextureStageGroup::GetSavedStageStateCount() const
{
    return m_uiNumSavedStages;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DTextureStageGroup::GetTotalSamplerStateCount() const
{
    return m_uiNumSamplers + m_uiNumSavedSamplers;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DTextureStageGroup::GetNoSaveSamplerStateCount() const
{
    return m_uiNumSamplers;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DTextureStageGroup::GetSavedSamplerStateCount() const
{
    return m_uiNumSavedSamplers;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DTextureStageGroup::GetUseMapValueCount() const
{
    return m_uiNumUseMapValue;
}
//---------------------------------------------------------------------------
inline bool NiD3DTextureStageGroup::GetFirstNoSaveStageState(
    unsigned int& uiState, unsigned int& uiValue)
{
    m_uiStageArrayIter = 0;
    return GetNextNoSaveStageState(uiState, uiValue);
}
//---------------------------------------------------------------------------
inline bool NiD3DTextureStageGroup::GetNextNoSaveStageState(
    unsigned int& uiState, unsigned int& uiValue)
{
    while (m_uiStageArrayIter < STAGEARRAYSIZE)
    {
        uiState = m_uiStageArrayIter++;
        if (m_abStageValid[uiState])
        {
            uiValue = m_auiStageArray[uiState];
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
inline bool NiD3DTextureStageGroup::GetFirstSavedStageState(
    unsigned int& uiState, unsigned int& uiValue)
{
    m_uiSavedStageArrayIter = 0;
    return GetNextSavedStageState(uiState, uiValue);
}
//---------------------------------------------------------------------------
inline bool NiD3DTextureStageGroup::GetNextSavedStageState(
    unsigned int& uiState, unsigned int& uiValue)
{
    while (m_uiSavedStageArrayIter < STAGEARRAYSIZE)
    {
        uiState = m_uiSavedStageArrayIter++;
        if (m_abSavedStageValid[uiState])
        {
            uiValue = m_auiSavedStageArray[uiState];
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
inline bool NiD3DTextureStageGroup::GetFirstNoSaveSamplerState(
    unsigned int& uiState, unsigned int& uiValue, bool& bUseMapValue)
{
    m_uiSamplerArrayIter = 0;
    return GetNextNoSaveSamplerState(uiState, uiValue, bUseMapValue);
}
//---------------------------------------------------------------------------
inline bool NiD3DTextureStageGroup::GetNextNoSaveSamplerState(
    unsigned int& uiState, unsigned int& uiValue, bool& bUseMapValue)
{
    while (m_uiSamplerArrayIter < SAMPLERARRAYSIZE)
    {
        uiState = m_uiSamplerArrayIter++;
        if (m_abSamplerValid[uiState])
        {
            uiValue = m_auiSamplerArray[uiState];
            bUseMapValue = m_abSamplerUseMapValue[uiState];
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
inline bool NiD3DTextureStageGroup::GetFirstSavedSamplerState(
    unsigned int& uiState, unsigned int& uiValue, bool& bUseMapValue)
{
    m_uiSavedSamplerArrayIter = 0;
    return GetNextSavedSamplerState(uiState, uiValue, bUseMapValue);
}
//---------------------------------------------------------------------------
inline bool NiD3DTextureStageGroup::GetNextSavedSamplerState(
    unsigned int& uiState, unsigned int& uiValue, bool& bUseMapValue)
{
    while (m_uiSavedSamplerArrayIter < SAMPLERARRAYSIZE)
    {
        uiState = m_uiSavedSamplerArrayIter++;
        if (m_abSavedSamplerValid[uiState])
        {
            uiValue = m_auiSavedSamplerArray[uiState];
            bUseMapValue = m_abSamplerUseMapValue[uiState];
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
