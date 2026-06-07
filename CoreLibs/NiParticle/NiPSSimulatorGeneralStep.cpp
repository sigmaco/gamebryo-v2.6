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
#include "NiParticlePCH.h"

#include "NiPSSimulatorGeneralStep.h"
#include "NiPSCommonSemantics.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSSimulatorGeneralStep, NiPSSimulatorStep);

//---------------------------------------------------------------------------
NiPSSimulatorGeneralStep::NiPSSimulatorGeneralStep() :
    m_kInputStructIS(&m_kInputStruct, 1),
    m_pkLastUpdateIS(NULL),
    m_pkFlagsIS(NULL),
    m_pkAgeIS(NULL),
    m_pkLifeSpanIS(NULL),
    m_pkRotAngleIS(NULL),
    m_pkRotSpeedIS(NULL),
    m_pkSizeOS(NULL),
    m_pkColorOS(NULL),
    m_pkRotAngleOS(NULL),
    m_pkColorKeys(NULL),
    m_ucNumColorKeys(0)
{
    // Initialize grow/shrink data.
    SetGrowTime(0.0f);
    SetShrinkTime(0.0f);
    SetGrowGeneration(0);
    SetShrinkGeneration(0);

    m_kInputStruct.m_ucNumColorKeys = 0;
}
//---------------------------------------------------------------------------
NiPSSimulatorGeneralStep::~NiPSSimulatorGeneralStep()
{
    NiAlignedFree(m_pkColorKeys);
}
//---------------------------------------------------------------------------
NiSPKernel* NiPSSimulatorGeneralStep::GetKernel()
{
    return &m_kKernel;
}
//---------------------------------------------------------------------------
NiUInt16 NiPSSimulatorGeneralStep::GetLargestInputStride()
{
    return m_pkLastUpdateIS->GetStride();
}
//---------------------------------------------------------------------------
void NiPSSimulatorGeneralStep::PrepareInputStream(
    NiPSParticleSystem* pkParticleSystem,
    const NiFixedString& kSemantic,
    NiSPStream* pkStream)
{
    // Update the stored pointer to the stream.
    // Associate particle data with Floodgate streams.
    if (kSemantic == NiPSCommonSemantics::PARTICLELASTUPDATE())
    {
        if (m_pkLastUpdateIS != pkStream)
        {
            NiDelete m_pkLastUpdateIS;
            m_pkLastUpdateIS = pkStream;
        }
        m_pkLastUpdateIS->SetData(pkParticleSystem->GetLastUpdateTimes());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEFLAGS())
    {
        if (m_pkFlagsIS != pkStream)
        {
            NiDelete m_pkFlagsIS;
            m_pkFlagsIS = pkStream;
        }
        m_pkFlagsIS->SetData(pkParticleSystem->GetFlags());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEAGE())
    {
        if (m_pkAgeIS != pkStream)
        {
            NiDelete m_pkAgeIS;
            m_pkAgeIS = pkStream;
        }
        m_pkAgeIS->SetData(pkParticleSystem->GetAges());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLELIFESPAN())
    {
        if (m_pkLifeSpanIS != pkStream)
        {
            NiDelete m_pkLifeSpanIS;
            m_pkLifeSpanIS = pkStream;
        }
        m_pkLifeSpanIS->SetData(pkParticleSystem->GetLifeSpans());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEROTANGLE())
    {
        if (m_pkRotAngleIS != pkStream)
        {
            NiDelete m_pkRotAngleIS;
            m_pkRotAngleIS = pkStream;
        }
        m_pkRotAngleIS->SetData(pkParticleSystem->GetRotationAngles());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEROTSPEED())
    {
        if (m_pkRotSpeedIS != pkStream)
        {
            NiDelete m_pkRotSpeedIS;
            m_pkRotSpeedIS = pkStream;
        }
        m_pkRotSpeedIS->SetData(pkParticleSystem->GetRotationSpeeds());
    }
    else
    {
        NIASSERT(!"Unknown semantic type!");
    }
}
//---------------------------------------------------------------------------
void NiPSSimulatorGeneralStep::PrepareOutputStream(
    NiPSParticleSystem* pkParticleSystem,
    const NiFixedString& kSemantic,
    NiSPStream* pkStream)
{
    NI_UNUSED_ARG(pkStream);
    // Update the stored pointer to the stream.
    if (kSemantic == NiPSCommonSemantics::PARTICLESIZE())
    {
        NIASSERT(m_pkSizeOS == pkStream);
        m_pkSizeOS->SetData(pkParticleSystem->GetSizes());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLECOLOR())
    {
        NIASSERT(m_pkColorOS == pkStream);
        m_pkColorOS->SetData(pkParticleSystem->GetColors());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEROTANGLE())
    {
        NIASSERT(m_pkRotAngleOS == pkStream);
        m_pkRotAngleOS->SetData(pkParticleSystem->GetRotationAngles());
    }
    else
    {
        NIASSERT(!"Unknown semantic type!");
    }
}
//---------------------------------------------------------------------------
NiSPTaskPtr NiPSSimulatorGeneralStep::Attach(
    NiPSParticleSystem*)
{
    NiSPTaskPtr spTask = NiSPTask::GetNewTask(8, 4);

    // Create input streams.
    SetInputCount(6);
    m_pkLastUpdateIS = NiNew NiTSPStream<float>();
    AddInput(NiPSCommonSemantics::PARTICLELASTUPDATE(), m_pkLastUpdateIS);
    m_pkFlagsIS = NiNew NiTSPStream<NiUInt32>();
    AddInput(NiPSCommonSemantics::PARTICLEFLAGS(), m_pkFlagsIS);
    m_pkAgeIS = NiNew NiTSPStream<float>();
    AddInput(NiPSCommonSemantics::PARTICLEAGE(), m_pkAgeIS);
    m_pkLifeSpanIS = NiNew NiTSPStream<float>();
    AddInput(NiPSCommonSemantics::PARTICLELIFESPAN(), m_pkLifeSpanIS);
    m_pkRotAngleIS = NiNew NiTSPStream<float>();
    AddInput(NiPSCommonSemantics::PARTICLEROTANGLE(), m_pkRotAngleIS);
    m_pkRotSpeedIS = NiNew NiTSPStream<float>();
    AddInput(NiPSCommonSemantics::PARTICLEROTSPEED(), m_pkRotSpeedIS);

    // Create output streams.
    SetOutputCount(3);
    m_pkSizeOS = NiNew NiTSPStream<float>();
    AddOutput(NiPSCommonSemantics::PARTICLESIZE(), m_pkSizeOS);
    m_pkColorOS = NiNew NiTSPStream<NiRGBA>();
    AddOutput(NiPSCommonSemantics::PARTICLECOLOR(), m_pkColorOS);
    m_pkRotAngleOS = NiNew NiTSPStream<float>();
    AddOutput(NiPSCommonSemantics::PARTICLEROTANGLE(), m_pkRotAngleOS);

    // Add local streams to the task.
    spTask->AddInput(&m_kInputStructIS);
    spTask->AddInput(&m_kColorKeyIS);
    
    return spTask;
}
//---------------------------------------------------------------------------
void NiPSSimulatorGeneralStep::Detach(NiPSParticleSystem* pkParticleSystem)
{
    // Call base class version to ensure that streams and semantics arrays
    // are properly cleared.
    NiPSSimulatorStep::Detach(pkParticleSystem);

    // Clear out input stream pointers.
    m_pkLastUpdateIS = NULL;
    m_pkFlagsIS = NULL;
    m_pkAgeIS = NULL;
    m_pkLifeSpanIS = NULL;
    m_pkRotAngleIS = NULL;
    m_pkRotSpeedIS = NULL;

    // Clear out output stream pointers.
    m_pkSizeOS = NULL;
    m_pkColorOS = NULL;
    m_pkRotAngleOS = NULL;
}
//---------------------------------------------------------------------------
bool NiPSSimulatorGeneralStep::Update(
    NiPSParticleSystem* pkParticleSystem,
    float fTime)
{
    // Update block count for Floodgate streams to the number of active
    // particles.
    const NiUInt32 uiNumParticles = pkParticleSystem->GetNumParticles();
    m_pkLastUpdateIS->SetBlockCount(uiNumParticles);
    m_pkFlagsIS->SetBlockCount(uiNumParticles);
    m_pkAgeIS->SetBlockCount(uiNumParticles);
    m_pkLifeSpanIS->SetBlockCount(uiNumParticles);
    if (pkParticleSystem->HasRotations())
    {
        m_pkRotAngleIS->SetBlockCount(uiNumParticles);
        m_pkRotAngleOS->SetBlockCount(uiNumParticles);
        m_pkRotSpeedIS->SetBlockCount(uiNumParticles);
    }
    m_pkSizeOS->SetBlockCount(uiNumParticles);
    if (pkParticleSystem->HasColors())
    {
        m_pkColorOS->SetBlockCount(uiNumParticles);
    }

    if (pkParticleSystem->HasColors())
    {
        // Set up color key stream.
        m_kColorKeyIS.SetData(m_pkColorKeys);
        m_kColorKeyIS.SetBlockCount(m_ucNumColorKeys);

        // Set number of color keys.
        m_kInputStruct.m_ucNumColorKeys = m_ucNumColorKeys;
    }

    // Set up input struct.
    m_kInputStruct.m_fCurrentTime = fTime;

    return (
        GetGrowTime() > 0.0f ||
        GetShrinkTime() > 0.0f ||
        (pkParticleSystem->HasColors() && m_ucNumColorKeys > 0) ||
        pkParticleSystem->HasRotations());
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSSimulatorGeneralStep);
//---------------------------------------------------------------------------
void NiPSSimulatorGeneralStep::CopyMembers(
    NiPSSimulatorGeneralStep* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSSimulatorStep::CopyMembers(pkDest, kCloning);

    if (m_pkColorKeys)
    {
        pkDest->CopyColorKeys(m_pkColorKeys, m_ucNumColorKeys);
    }

    pkDest->SetGrowTime(GetGrowTime());
    pkDest->SetShrinkTime(GetShrinkTime());
    pkDest->SetGrowGeneration(GetGrowGeneration());
    pkDest->SetShrinkGeneration(GetShrinkGeneration());
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSSimulatorGeneralStep);
//---------------------------------------------------------------------------
void NiPSSimulatorGeneralStep::LoadBinary(NiStream& kStream)
{
    NiPSSimulatorStep::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_ucNumColorKeys);
    if (m_ucNumColorKeys > 0)
    {
        m_pkColorKeys = NiAlignedAlloc(NiPSKernelColorKey, m_ucNumColorKeys,
            NIPSKERNEL_ALIGNMENT);
        for (NiUInt8 uc = 0; uc < m_ucNumColorKeys; ++uc)
        {
            NiStreamLoadBinary(kStream, m_pkColorKeys[uc].m_fTime);
            m_pkColorKeys[uc].m_kColor.LoadBinary(kStream);
        }
    }

    float fValue;
    NiStreamLoadBinary(kStream, fValue);
    SetGrowTime(fValue);
    NiStreamLoadBinary(kStream, fValue);
    SetShrinkTime(fValue);
    NiUInt16 usValue;
    NiStreamLoadBinary(kStream, usValue);
    SetGrowGeneration(usValue);
    NiStreamLoadBinary(kStream, usValue);
    SetShrinkGeneration(usValue);
}
//---------------------------------------------------------------------------
void NiPSSimulatorGeneralStep::LinkObject(NiStream& kStream)
{
    NiPSSimulatorStep::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSSimulatorGeneralStep::RegisterStreamables(NiStream& kStream)
{
    return NiPSSimulatorStep::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSSimulatorGeneralStep::SaveBinary(NiStream& kStream)
{
    NiPSSimulatorStep::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_ucNumColorKeys);
    for (NiUInt8 uc = 0; uc < m_ucNumColorKeys; ++uc)
    {
        NiStreamSaveBinary(kStream, m_pkColorKeys[uc].m_fTime);
        m_pkColorKeys[uc].m_kColor.SaveBinary(kStream);
    }

    NiStreamSaveBinary(kStream, GetGrowTime());
    NiStreamSaveBinary(kStream, GetShrinkTime());
    NiStreamSaveBinary(kStream, GetGrowGeneration());
    NiStreamSaveBinary(kStream, GetShrinkGeneration());
}
//---------------------------------------------------------------------------
bool NiPSSimulatorGeneralStep::IsEqual(NiObject* pkObject)
{
    if (!NiPSSimulatorStep::IsEqual(pkObject))
    {
        return false;
    }

    NiPSSimulatorGeneralStep* pkDest = (NiPSSimulatorGeneralStep*) pkObject;

    if (pkDest->m_ucNumColorKeys != m_ucNumColorKeys)
    {
        return false;
    }

    for (NiUInt8 uc = 0; uc < pkDest->m_ucNumColorKeys; ++uc)
    {
        if (pkDest->m_pkColorKeys[uc].m_kColor != m_pkColorKeys[uc].m_kColor ||
            pkDest->m_pkColorKeys[uc].m_fTime != m_pkColorKeys[uc].m_fTime)
        {
            return false;
        }
    }

    if (pkDest->GetGrowTime() != GetGrowTime() ||
        pkDest->GetShrinkTime() != GetShrinkTime() ||
        pkDest->GetGrowGeneration() != GetGrowGeneration() ||
        pkDest->GetShrinkGeneration() != GetShrinkGeneration())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSSimulatorGeneralStep::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSSimulatorStep::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSSimulatorGeneralStep::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("NumColorKeys", m_ucNumColorKeys));

    pkStrings->Add(NiGetViewerString("GrowTime", GetGrowTime()));
    pkStrings->Add(NiGetViewerString("ShrinkTime", GetShrinkTime()));
    pkStrings->Add(NiGetViewerString("GrowGeneration", GetGrowGeneration()));
    pkStrings->Add(NiGetViewerString("ShrinkGeneration",
        GetShrinkGeneration()));
}
//---------------------------------------------------------------------------
