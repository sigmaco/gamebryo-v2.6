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
#include "NiAnimationPCH.h"

#include "NiEvaluator.h"
#include <NiQuaternion.h>
#include <NiPoint3.h>
#include <NiColor.h>
#include "NiQuatTransform.h"

NiImplementRTTI(NiEvaluator, NiObject);

const NiBool NiEvaluator::INVALID_BOOL = NiBool(2);
const float NiEvaluator::INVALID_FLOAT = -FLT_MAX;
const NiQuaternion NiEvaluator::INVALID_QUATERNION = NiQuaternion(
    -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
const NiPoint3 NiEvaluator::INVALID_POINT3 = NiPoint3(
    -FLT_MAX, -FLT_MAX, -FLT_MAX);
const NiColorA NiEvaluator::INVALID_COLORA = NiColorA(
    -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);


//---------------------------------------------------------------------------
NiEvaluator::NiEvaluator(const NiFixedString& kAVObjectName, 
    const NiFixedString& kPropertyType, const NiFixedString& kCtlrType, 
    const NiFixedString& kCtlrID, const NiFixedString& kEvaluatorID) :
    m_kIDTag(kAVObjectName, kPropertyType, kCtlrType, kCtlrID, kEvaluatorID)
{
    NIASSERT(EVALMAXINDICES == 4);
    m_aiEvalChannelTypes[0] = EVALINVALIDCHANNEL;
    m_aiEvalChannelTypes[1] = EVALINVALIDCHANNEL;
    m_aiEvalChannelTypes[2] = EVALINVALIDCHANNEL;
    NIASSERT(EVALFLAGINDEX == 3);
    m_aiEvalChannelTypes[3] = 0;
}
//---------------------------------------------------------------------------
NiEvaluator::NiEvaluator(const IDTag& kIDTag) :
    m_kIDTag(kIDTag)
{
    NIASSERT(EVALMAXINDICES == 4);
    m_aiEvalChannelTypes[0] = EVALINVALIDCHANNEL;
    m_aiEvalChannelTypes[1] = EVALINVALIDCHANNEL;
    m_aiEvalChannelTypes[2] = EVALINVALIDCHANNEL;
    NIASSERT(EVALFLAGINDEX == 3);
    m_aiEvalChannelTypes[3] = 0;
}
//---------------------------------------------------------------------------
NiEvaluator::NiEvaluator() :
    m_kIDTag()
{
    NIASSERT(EVALMAXINDICES == 4);
    m_aiEvalChannelTypes[0] = EVALINVALIDCHANNEL;
    m_aiEvalChannelTypes[1] = EVALINVALIDCHANNEL;
    m_aiEvalChannelTypes[2] = EVALINVALIDCHANNEL;
    NIASSERT(EVALFLAGINDEX == 3);
    m_aiEvalChannelTypes[3] = 0;
}
//---------------------------------------------------------------------------
NiEvaluator::~NiEvaluator()
{
    m_kIDTag.ClearValues();
}
//---------------------------------------------------------------------------
void NiEvaluator::Collapse()
{
}
//---------------------------------------------------------------------------
bool NiEvaluator::GetChannelPosedValue(unsigned int, 
    void*) const
{
    return false;
}
//---------------------------------------------------------------------------
bool NiEvaluator::UpdateChannel(float, unsigned int, 
    NiEvaluatorSPData*, void*) const
{
    NIASSERT(!"UpdateChannel is not supported by this base class!");
    return false;
}
//---------------------------------------------------------------------------
void NiEvaluator::ShutdownChannelScratchPadData(unsigned int, 
    NiEvaluatorSPData*) const
{
    NIASSERT(!"ShutdownChannelScratchPadData does nothing "
        "in this base class!");
}
//---------------------------------------------------------------------------
void NiEvaluator::GetActiveTimeRange(float& fBeginKeyTime,
    float& fEndKeyTime) const
{
    fBeginKeyTime = 0.0f;
    fEndKeyTime = 0.0f;
}
//---------------------------------------------------------------------------
void NiEvaluator::GuaranteeTimeRange(float, 
    float)
{
}
//---------------------------------------------------------------------------
NiEvaluator* NiEvaluator::GetSequenceEvaluator(float,
    float)
{
    NiEvaluator* pkEval = (NiEvaluator*)Clone();
    NIASSERT(pkEval);
    return pkEval;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiEvaluator::CopyMembers(NiEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_kIDTag = m_kIDTag;

    NIASSERT(EVALMAXINDICES == 4);
    pkDest->m_aiEvalChannelTypes[0] = m_aiEvalChannelTypes[0];
    pkDest->m_aiEvalChannelTypes[1] = m_aiEvalChannelTypes[1];
    pkDest->m_aiEvalChannelTypes[2] = m_aiEvalChannelTypes[2];
    pkDest->m_aiEvalChannelTypes[3] = m_aiEvalChannelTypes[3];
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiEvaluator::IDTag::RegisterStreamables(NiStream& kStream)
{
    kStream.RegisterFixedString(m_kAVObjectName);
    kStream.RegisterFixedString(m_kPropertyType);
    kStream.RegisterFixedString(m_kCtlrType);
    kStream.RegisterFixedString(m_kCtlrID);
    kStream.RegisterFixedString(m_kEvaluatorID);
}
//---------------------------------------------------------------------------
void NiEvaluator::IDTag::SaveBinary(NiStream& kStream)
{
    kStream.SaveFixedString(m_kAVObjectName);
    kStream.SaveFixedString(m_kPropertyType);
    kStream.SaveFixedString(m_kCtlrType);
    kStream.SaveFixedString(m_kCtlrID);
    kStream.SaveFixedString(m_kEvaluatorID);
}
//---------------------------------------------------------------------------
void NiEvaluator::IDTag::LoadBinary(NiStream& kStream)
{
    kStream.LoadFixedString(m_kAVObjectName);
    kStream.LoadFixedString(m_kPropertyType);
    kStream.LoadFixedString(m_kCtlrType);
    kStream.LoadFixedString(m_kCtlrID);
    kStream.LoadFixedString(m_kEvaluatorID);

    UpdateHashTableValues();
}
//---------------------------------------------------------------------------
bool NiEvaluator::IDTag::IsEqual(const IDTag& kDest)
{
    if (m_kAVObjectName != kDest.m_kAVObjectName ||
        m_kPropertyType != kDest.m_kPropertyType ||
        m_kCtlrType != kDest.m_kCtlrType ||
        m_kCtlrID != kDest.m_kCtlrID ||
        m_kEvaluatorID != kDest.m_kEvaluatorID)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiEvaluator::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    m_kIDTag.LoadBinary(kStream);

    NIASSERT(EVALMAXINDICES == 4);
    for (unsigned int ui = 0; ui < 4; ui++)
    {
        NiStreamLoadBinary(kStream, m_aiEvalChannelTypes[ui]);
    }
}
//---------------------------------------------------------------------------
void NiEvaluator::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
    {
        return false;
    }

    m_kIDTag.RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiEvaluator::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    m_kIDTag.SaveBinary(kStream);

    NIASSERT(EVALMAXINDICES == 4);
    for (unsigned int ui = 0; ui < EVALMAXINDICES; ui++)
    {
        NiStreamSaveBinary(kStream, m_aiEvalChannelTypes[ui]);
    }
}
//---------------------------------------------------------------------------
bool NiEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    const NiEvaluator* pkOther = (const NiEvaluator*)pkObject;

    if (!m_kIDTag.IsEqual(pkOther->m_kIDTag))
    {
        return false;
    }

    NIASSERT(EVALMAXINDICES == 4);
    if (m_aiEvalChannelTypes[0] != pkOther->m_aiEvalChannelTypes[0] ||
        m_aiEvalChannelTypes[1] != pkOther->m_aiEvalChannelTypes[1] ||
        m_aiEvalChannelTypes[2] != pkOther->m_aiEvalChannelTypes[2] ||
        m_aiEvalChannelTypes[3] != pkOther->m_aiEvalChannelTypes[3])
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiEvaluator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiEvaluator::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_kIDTag.m_kAVObjectName", 
        m_kIDTag.GetAVObjectName()));
    pkStrings->Add(NiGetViewerString("m_kIDTag.m_kPropertyType", 
        m_kIDTag.GetPropertyType()));
    pkStrings->Add(NiGetViewerString("m_kIDTag.m_kCtlrType", 
        m_kIDTag.GetCtlrType()));
    pkStrings->Add(NiGetViewerString("m_kIDTag.m_kCtlrID", 
        m_kIDTag.GetCtlrID()));
    pkStrings->Add(NiGetViewerString("m_kIDTag.m_kEvaluatorID", 
        m_kIDTag.GetEvaluatorID()));

    NIASSERT(EVALMAXINDICES == 4);
    pkStrings->Add(NiGetViewerString("m_aiEvalChannelTypes[0]", 
        m_aiEvalChannelTypes[0]));
    pkStrings->Add(NiGetViewerString("m_aiEvalChannelTypes[1]", 
        m_aiEvalChannelTypes[1]));
    pkStrings->Add(NiGetViewerString("m_aiEvalChannelTypes[2]", 
        m_aiEvalChannelTypes[2]));
    pkStrings->Add(NiGetViewerString("m_aiEvalChannelTypes[3]", 
        m_aiEvalChannelTypes[3]));
}
//---------------------------------------------------------------------------
