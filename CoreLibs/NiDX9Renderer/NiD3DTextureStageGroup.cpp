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

// Precompiled Header
#include "NiD3DRendererPCH.h"

#include "NiD3DTextureStageGroup.h"
#include "NiD3DDefines.h"
#include "NiDX9RenderState.h"
#include "NiD3DRendererHeaders.h"

NiD3DRenderState* NiD3DTextureStageGroup::ms_pkD3DRenderState = 0;
NiTObjectPool<NiD3DTextureStageGroup>* 
    NiD3DTextureStageGroup::ms_pkTextureStageGroupPool = 0;

//---------------------------------------------------------------------------
NiD3DTextureStageGroup::NiD3DTextureStageGroup() :
    m_bRendererOwned(false),
    m_uiNumSavedStages(0),
    m_uiSavedStageArrayIter(0),
    m_uiNumStages(0),
    m_uiStageArrayIter(0),
    m_uiNumSavedSamplers(0),
    m_uiSavedSamplerArrayIter(0),
    m_uiNumSamplers(0),
    m_uiSamplerArrayIter(0),
    m_uiNumUseMapValue(0)
{
    memset(m_auiSavedStageArray, 0, sizeof(m_auiSavedStageArray));
    memset(m_abSavedStageValid, 0, sizeof(m_abSavedStageValid));
    memset(m_auiStageArray, 0, sizeof(m_auiStageArray));
    memset(m_abStageValid, 0, sizeof(m_abStageValid));
    memset(m_auiSavedSamplerArray, 0, sizeof(m_auiSavedSamplerArray));
    memset(m_abSavedSamplerValid, 0, sizeof(m_abSavedSamplerValid));
    memset(m_auiSamplerArray, 0, sizeof(m_auiSamplerArray));
    memset(m_abSamplerValid, 0, sizeof(m_abSamplerValid));
    memset(m_abSamplerUseMapValue, 0, sizeof(m_abSamplerUseMapValue));
}
//---------------------------------------------------------------------------
NiD3DTextureStageGroup::~NiD3DTextureStageGroup()
{
}
//---------------------------------------------------------------------------
void NiD3DTextureStageGroup::InitializePools()
{
    ms_pkTextureStageGroupPool = NiNew 
        NiTObjectPool<NiD3DTextureStageGroup>;
}
//---------------------------------------------------------------------------
void NiD3DTextureStageGroup::ShutdownPools()
{
    NiDelete ms_pkTextureStageGroupPool;
}
//---------------------------------------------------------------------------
void NiD3DTextureStageGroup::SetStageState(unsigned int uiState, 
    unsigned int uiValue, bool bSave)
{
    if (uiState >= STAGEARRAYSIZE)
        return;

    // Determine the mapping used
    if (bSave)
    {
        // See if it's in the intended map
        if (m_abSavedStageValid[uiState])
        {
            m_auiSavedStageArray[uiState] = uiValue;
            return;
        }

        // Wasn't in the intended pool. Check the other one
        if (m_abStageValid[uiState])
        {
            m_abStageValid[uiState] = false;
            m_abSavedStageValid[uiState] = true;
            m_auiSavedStageArray[uiState] = uiValue;
            m_uiNumStages--;
            m_uiNumSavedStages++;
        }

        // Wasn't in either... get an entry and add it
        m_abSavedStageValid[uiState] = true;
        m_auiSavedStageArray[uiState] = uiValue;
        m_uiNumSavedStages++;
    }
    else
    {
        // See if it's in the intended map
        if (m_abStageValid[uiState])
        {
            m_auiStageArray[uiState] = uiValue;
            return;
        }

        // Wasn't in the intended pool. Check the other one
        if (m_abSavedStageValid[uiState])
        {
            m_abSavedStageValid[uiState] = false;
            m_abStageValid[uiState] = true;
            m_auiStageArray[uiState] = uiValue;
            m_uiNumStages++;
            m_uiNumSavedStages--;
        }

        // Wasn't in either... get an entry and add it
        m_abStageValid[uiState] = true;
        m_auiStageArray[uiState] = uiValue;
        m_uiNumStages++;
    }
}
//---------------------------------------------------------------------------
void NiD3DTextureStageGroup::RemoveStageState(unsigned int uiState)
{
    if (uiState < STAGEARRAYSIZE)
    {
        m_abSavedStageValid[uiState] = false;
        m_abStageValid[uiState] = false;
    }
}
//---------------------------------------------------------------------------
bool NiD3DTextureStageGroup::GetStageState(unsigned int uiState, 
    unsigned int& uiValue, bool& bSave)
{
    if (uiState < STAGEARRAYSIZE)
    {
        if (m_abStageValid[uiState])
        {
            uiValue =  m_auiStageArray[uiState];
            bSave = false;
            return true;
        }
        else if (m_abSavedStageValid[uiState])
        {
            uiValue =  m_auiSavedStageArray[uiState];
            bSave = true;
            return true;
        }
    }

    // Wasn't found - return failure.
    return false;
}
//---------------------------------------------------------------------------
void NiD3DTextureStageGroup::SetSamplerState(unsigned int uiState, 
    unsigned int uiValue, bool bSave, bool bUseMapValue)
{
    if (uiState >= SAMPLERARRAYSIZE)
        return;

    // Determine the mapping used
    if (bSave)
    {
        // See if it's in the intended map
        if (m_abSavedSamplerValid[uiState])
        {
            m_auiSavedSamplerArray[uiState] = uiValue;
            if (bUseMapValue && !m_abSamplerUseMapValue[uiState])
            {
                m_uiNumUseMapValue++;
            }
            else if (!bUseMapValue && m_abSamplerUseMapValue[uiState])
            {
                m_uiNumUseMapValue--;
            }
            m_abSamplerUseMapValue[uiState] = bUseMapValue;
            return;
        }

        // Wasn't in the intended pool. Check the other one
        if (m_abSamplerValid[uiState])
        {
            // Was in the other pool - clear it out
            m_abSamplerValid[uiState] = false;
            m_uiNumSamplers--;
            if (m_abSamplerUseMapValue[uiState])
            {
                m_uiNumUseMapValue--;
            }
            m_abSamplerUseMapValue[uiState] = false;
        }

        // Set it in the saved pool
        m_abSavedSamplerValid[uiState] = true;
        m_auiSavedSamplerArray[uiState] = uiValue;
        m_uiNumSavedSamplers++;
        if (bUseMapValue)
        {
            m_uiNumUseMapValue++;
        }
        m_abSamplerUseMapValue[uiState] = bUseMapValue;
    }
    else
    {
        // See if it's in the intended map
        if (m_abSamplerValid[uiState])
        {
            m_auiSamplerArray[uiState] = uiValue;
            if (bUseMapValue && !m_abSamplerUseMapValue[uiState])
            {
                m_uiNumUseMapValue++;
            }
            else if (!bUseMapValue && m_abSamplerUseMapValue[uiState])
            {
                m_uiNumUseMapValue--;
            }
            m_abSamplerUseMapValue[uiState] = bUseMapValue;
            return;
        }

        // Wasn't in the intended pool. Check the other one
        if (m_abSavedSamplerValid[uiState])
        {
            // Was in the other pool - clear it out
            m_abSavedSamplerValid[uiState] = false;
            m_uiNumSavedSamplers--;
            if (m_abSamplerUseMapValue[uiState])
            {
                m_uiNumUseMapValue--;
            }
            m_abSamplerUseMapValue[uiState] = false;
        }

        // Wasn't in either... get an entry and add it
        m_abSamplerValid[uiState] = true;
        m_auiSamplerArray[uiState] = uiValue;
        m_uiNumSamplers++;
        if (bUseMapValue)
        {
            m_uiNumUseMapValue++;
        }
        m_abSamplerUseMapValue[uiState] = bUseMapValue;
    }
}
//---------------------------------------------------------------------------
void NiD3DTextureStageGroup::RemoveSamplerState(unsigned int uiState)
{
    if (uiState < SAMPLERARRAYSIZE)
    {
        m_abSavedSamplerValid[uiState] = false;
        m_abSamplerValid[uiState] = false;
        if (m_abSamplerUseMapValue[uiState])
        {
            m_uiNumUseMapValue--;
        }
        m_abSamplerUseMapValue[uiState] = false;
    }
}
//---------------------------------------------------------------------------
bool NiD3DTextureStageGroup::GetSamplerState(unsigned int uiState, 
    unsigned int& uiValue, bool& bSave, bool& bUseMapValue)
{
    if (uiState < SAMPLERARRAYSIZE)
    {
        if (m_abSamplerValid[uiState])
        {
            uiValue =  m_auiSamplerArray[uiState];
            bSave = false;
            bUseMapValue = m_abSamplerUseMapValue[uiState];
            return true;
        }
        else if (m_abSavedSamplerValid[uiState])
        {
            uiValue =  m_auiSavedSamplerArray[uiState];
            bSave = true;
            bUseMapValue = m_abSamplerUseMapValue[uiState];
            return true;
        }
    }

    // Wasn't found - return failure.
    return false;
}
//---------------------------------------------------------------------------
void NiD3DTextureStageGroup::RemoveAllStageStates()
{
    for (unsigned int i = 0; i < STAGEARRAYSIZE; i++)
    {
        m_abSavedStageValid[i] = false;
        m_abStageValid[i] = false;
    }
    m_uiNumStages = 0;
    m_uiNumSavedStages = 0;
}
//---------------------------------------------------------------------------
void NiD3DTextureStageGroup::RemoveAllSamplerStates()
{
    for (unsigned int i = 0; i < SAMPLERARRAYSIZE; i++)
    {
        m_abSavedSamplerValid[i] = false;
        m_abSamplerValid[i] = false;
        m_abSamplerUseMapValue[i] = false;
    }
    m_uiNumSamplers = 0;
    m_uiNumSavedSamplers = 0;
    m_uiNumUseMapValue = 0;
}
//---------------------------------------------------------------------------
NiD3DError NiD3DTextureStageGroup::SetTextureStage(unsigned int uiStage)
{
    NiD3DError eStage = NISHADERERR_OK;
    if (uiStage < NiD3DPass::ms_uiMaxTextureBlendStages)
        eStage = SetAllStageStates(uiStage);
    NiD3DError eSampler = SetAllStageSamplers(uiStage);

    if (eStage != NISHADERERR_OK)
        return eStage;
    if (eSampler != NISHADERERR_OK)
        return eSampler;

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
NiD3DError NiD3DTextureStageGroup::RestoreTextureStage(unsigned int uiStage)
{
    NiD3DError eStage = RestoreAllStageStates(uiStage);
    NiD3DError eSampler = RestoreAllStageSamplers(uiStage);

    if (eStage != NISHADERERR_OK)
        return eStage;
    if (eSampler != NISHADERERR_OK)
        return eSampler;

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
NiD3DError NiD3DTextureStageGroup::SetAllStageStates(unsigned int uiStage)
{
    for (unsigned int i = 0; i < STAGEARRAYSIZE; i++)
    {
        if (m_abStageValid[i])
        {
            ms_pkD3DRenderState->SetTextureStageState(uiStage, 
                (D3DTEXTURESTAGESTATETYPE)i, 
                m_auiStageArray[i]);
        }
        else if (m_abSavedStageValid[i])
        {
            ms_pkD3DRenderState->SetTextureStageState(uiStage, 
                (D3DTEXTURESTAGESTATETYPE)i, 
                m_auiSavedStageArray[i]);
        }
    }

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
NiD3DError NiD3DTextureStageGroup::RestoreAllStageStates(
    unsigned int uiStage)
{
    for (unsigned int i = 0; i < STAGEARRAYSIZE; i++)
    {
        if (m_abSavedStageValid[i])
        {
            ms_pkD3DRenderState->RestoreTextureStageState(uiStage, 
                (D3DTEXTURESTAGESTATETYPE)i);
        }
    }

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
NiD3DError NiD3DTextureStageGroup::SetAllStageSamplers(
    unsigned int uiStage)
{
    for (unsigned int i = 0; i < SAMPLERARRAYSIZE; i++)
    {
        if (m_abSamplerValid[i])
        {
            ms_pkD3DRenderState->SetSamplerState(uiStage, 
                (NiD3DRenderState::NiD3DSamplerState)i, 
                m_auiSamplerArray[i]);
        }
        else if (m_abSavedSamplerValid[i])
        {
            ms_pkD3DRenderState->SetSamplerState(uiStage, 
                (NiD3DRenderState::NiD3DSamplerState)i, 
                m_auiSavedSamplerArray[i]);
        }
    }

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
NiD3DError NiD3DTextureStageGroup::RestoreAllStageSamplers(
    unsigned int uiStage)
{
    for (unsigned int i = 0; i < SAMPLERARRAYSIZE; i++)
    {
        if (m_abSavedSamplerValid[i])
        {
            ms_pkD3DRenderState->RestoreSamplerState(uiStage, 
                (NiD3DRenderState::NiD3DSamplerState)i);
        }
    }

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
NiD3DTextureStageGroup* NiD3DTextureStageGroup::GetFreeTextureStageGroup()
{
    NiD3DTextureStageGroup* pkGroup = 
        ms_pkTextureStageGroupPool->GetFreeObject();

    if (!pkGroup->m_bRendererOwned)
    {
        // If the renderer owns this pass, it is part of the NiD3DPass pool
        pkGroup->m_bRendererOwned = true;
    }

    return pkGroup;
}
//---------------------------------------------------------------------------
void NiD3DTextureStageGroup::ReleaseTextureStageGroup(
    NiD3DTextureStageGroup* pkGroup)
{
    if (!pkGroup->m_bRendererOwned)
        return;

    // Release any entries
    memset(pkGroup->m_auiSavedStageArray, 0, 
        sizeof(pkGroup->m_auiSavedStageArray));
    memset(pkGroup->m_abSavedStageValid, 0, 
        sizeof(pkGroup->m_abSavedStageValid));
    memset(pkGroup->m_auiStageArray, 0, 
        sizeof(pkGroup->m_auiStageArray));
    memset(pkGroup->m_abStageValid, 0, 
        sizeof(pkGroup->m_abStageValid));
    memset(pkGroup->m_auiSavedSamplerArray, 0, 
        sizeof(pkGroup->m_auiSavedSamplerArray));
    memset(pkGroup->m_abSavedSamplerValid, 0, 
        sizeof(pkGroup->m_abSavedSamplerValid));
    memset(pkGroup->m_auiSamplerArray, 0, 
        sizeof(pkGroup->m_auiSamplerArray));
    memset(pkGroup->m_abSamplerValid, 0, 
        sizeof(pkGroup->m_abSamplerValid));
    memset(pkGroup->m_abSamplerUseMapValue, 0,
        sizeof(pkGroup->m_abSamplerValid));

    pkGroup->m_uiNumStages = 0;
    pkGroup->m_uiNumSavedStages = 0;
    pkGroup->m_uiNumSamplers = 0;
    pkGroup->m_uiNumSavedSamplers = 0;
    pkGroup->m_uiNumUseMapValue = 0;

    ms_pkTextureStageGroupPool->ReleaseObject(pkGroup);
}
//---------------------------------------------------------------------------
