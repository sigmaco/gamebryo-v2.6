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

#include "stdafx.h"
#include "MSequence.h"
#include "MSharedData.h"
#include "MAnimation.h"

using namespace NiManagedToolInterface;
//---------------------------------------------------------------------------
// MSequence::MTextKey
//---------------------------------------------------------------------------
MSequence::MTextKey::MTextKey(NiTextKey* pkKey)
{
    m_fTime = pkKey->GetTime();
    m_strText = NiExternalNew String(pkKey->GetText());
}
//---------------------------------------------------------------------------
float MSequence::MTextKey::get_Time()
{
    return m_fTime;
}
//---------------------------------------------------------------------------
String* MSequence::MTextKey::get_Text()
{
    return m_strText;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// MSequence::MEvaluatorInfo
//---------------------------------------------------------------------------
MSequence::MEvaluatorInfo::MEvaluatorInfo(NiEvaluator* pkEvaluator)
{
    m_pkEvaluator = pkEvaluator;
}
//---------------------------------------------------------------------------
String* MSequence::MEvaluatorInfo::get_RTTI()
{
    return NiExternalNew String(m_pkEvaluator->GetRTTI()->GetName());
}
//---------------------------------------------------------------------------
String* MSequence::MEvaluatorInfo::get_AVObjectName()
{
    return NiExternalNew String(m_pkEvaluator->GetIDTag().GetAVObjectName());
}
//---------------------------------------------------------------------------
String* MSequence::MEvaluatorInfo::get_PropertyType()
{
    return NiExternalNew String(m_pkEvaluator->GetIDTag().GetPropertyType());
}
//---------------------------------------------------------------------------
String* MSequence::MEvaluatorInfo::get_ControllerType()
{
    return NiExternalNew String(m_pkEvaluator->GetIDTag().GetCtlrType());
}
//---------------------------------------------------------------------------
String* MSequence::MEvaluatorInfo::get_ControllerID()
{
    return NiExternalNew String(m_pkEvaluator->GetIDTag().GetCtlrID());
}
//---------------------------------------------------------------------------
String* MSequence::MEvaluatorInfo::get_EvaluatorID()
{
    return NiExternalNew String(m_pkEvaluator->GetIDTag().GetEvaluatorID());
}
//---------------------------------------------------------------------------
unsigned short MSequence::MEvaluatorInfo::GetKeyChannelCount()
{
    if (NiIsKindOf(NiBSplineEvaluator, m_pkEvaluator))
    {
        return  ((NiBSplineEvaluator*)m_pkEvaluator)->
            GetChannelCount();
    }

    if (!NiIsKindOf(NiKeyBasedEvaluator, m_pkEvaluator))
    {
        return 0;
    }
    else
    {
        return ((NiKeyBasedEvaluator*)m_pkEvaluator)->
            GetKeyChannelCount();
    }
}
//---------------------------------------------------------------------------
unsigned int MSequence::MEvaluatorInfo::GetKeyCount(
    unsigned short usChannel)
{
    if (NiIsKindOf(NiBSplineEvaluator, m_pkEvaluator))
    {
        return  ((NiBSplineEvaluator*)m_pkEvaluator)->
            GetControlPointCount(usChannel);
    }

    if (!NiIsKindOf(NiKeyBasedEvaluator, m_pkEvaluator))
    {
        return 0;
    }
    else
    {
        return ((NiKeyBasedEvaluator*)m_pkEvaluator)->
            GetKeyCount(usChannel);      
    }
}
//---------------------------------------------------------------------------
MSequence::MEvaluatorInfo::KeyType 
    MSequence::MEvaluatorInfo::GetKeyType(unsigned short usChannel)
{
    if (!NiIsKindOf(NiKeyBasedEvaluator, m_pkEvaluator))
    {
        return NUMKEYTYPES;
    }
    else
    {
        return (MSequence::MEvaluatorInfo::KeyType)
            ((NiKeyBasedEvaluator*)m_pkEvaluator)->GetKeyType(
            usChannel);      
    }
}
//---------------------------------------------------------------------------
MSequence::MEvaluatorInfo::KeyContent 
    MSequence::MEvaluatorInfo::GetKeyContent(unsigned short usChannel)
{
    if (!NiIsKindOf(NiKeyBasedEvaluator, m_pkEvaluator))
    {
        return NUMKEYCONTENTS;
    }
    else
    {
        return (MSequence::MEvaluatorInfo::KeyContent)
            ((NiKeyBasedEvaluator*)m_pkEvaluator)->
            GetKeyContent(usChannel);  
    }
}
//---------------------------------------------------------------------------
bool MSequence::MEvaluatorInfo::GetChannelPosed(unsigned short usChannel)
{
    if (NiIsKindOf(NiBSplineEvaluator, m_pkEvaluator))
    {
        return  NiVirtualBoolBugWrapper::NiBSplineEvaluator_GetChannelPosed(
            (NiBSplineEvaluator*)m_pkEvaluator, usChannel) ?
            true : false;       
    }

    if (!NiIsKindOf(NiKeyBasedEvaluator, m_pkEvaluator))
    {
        return 0;
    }
    else
    {
        return NiVirtualBoolBugWrapper::NiKeyBasedEvaluator_GetChannelPosed(
            (NiKeyBasedEvaluator*)m_pkEvaluator, usChannel) ?
            true : false;      
    }
}
//---------------------------------------------------------------------------
unsigned int MSequence::MEvaluatorInfo::GetDimension(
    unsigned short usChannel)
{
    if (!NiIsKindOf(NiBSplineEvaluator, m_pkEvaluator))
    {
        return 0;
    }
    else
    {
        return ((NiBSplineEvaluator*)m_pkEvaluator)->
            GetDimension(usChannel);      
    }
}
//---------------------------------------------------------------------------
unsigned int MSequence::MEvaluatorInfo::GetDegree(
    unsigned short usChannel)
{
    if (!NiIsKindOf(NiBSplineEvaluator, m_pkEvaluator))
    {
        return 0;
    }
    else
    {
        return ((NiBSplineEvaluator*)m_pkEvaluator)->
            GetDegree(usChannel);      
    }
}
//---------------------------------------------------------------------------
unsigned int MSequence::MEvaluatorInfo::GetAllocatedSize(
    unsigned short usChannel)
{
    if (NiIsKindOf(NiBSplineEvaluator, m_pkEvaluator))
    {
        return ((NiBSplineEvaluator*)m_pkEvaluator)->
            GetAllocatedSize(usChannel);      
    }

    if (!NiIsKindOf(NiKeyBasedEvaluator, m_pkEvaluator))
    {
        return 0;
    }
    else
    {
        return ((NiKeyBasedEvaluator*)m_pkEvaluator)->
            GetAllocatedSize(usChannel);      
    }
}
//---------------------------------------------------------------------------
bool MSequence::MEvaluatorInfo::IsBSplineEvaluator()
{
    if (NiIsKindOf(NiBSplineEvaluator, m_pkEvaluator))
    {
        return true;
    }
    
    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// MSequence
//---------------------------------------------------------------------------
MSequence::MSequence(MAnimation* pkAnimation, NiKFMTool::Sequence*
    pkKFMSequence, NiSequenceData* pkSeqData) : m_pkAnimation(pkAnimation), 
    m_pkKFMSequence(pkKFMSequence), m_pkSeqData(pkSeqData), 
    m_pkControllerSequence(NULL), m_aTransitions(NULL), m_pkStoredAccum(NULL)
{
    NIASSERT(m_pkKFMSequence && m_pkSeqData && m_pkAnimation);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkSeqData->IncRefCount(); // hold a ref
    pkData->Unlock();

    RebuildTransitionsArray();
    m_pkStoredAccum = NiNew NiQuatTransform;
    m_bDisposed = false;
    FillArrays();

    DefaultPositionTime = 0;
}
//---------------------------------------------------------------------------
void MSequence::DeleteContents()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    m_pkAnimation = NULL;
    SetControllerSequence(NULL);
    NiDelete m_pkStoredAccum;
    m_pkStoredAccum = NULL;

    if (m_aTransitions != NULL)
    {
        for (int i = 0; i < m_aTransitions->Length; i++)
        {
            m_aTransitions[i]->DeleteContents();
        }
        m_aTransitions = NULL;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkSeqData->DecRefCount(); // release ref
    pkData->Unlock();
    m_pkSeqData = NULL;

    m_bDisposed = true;
}
//---------------------------------------------------------------------------
int MSequence::CompareTo(Object* pkObject)
{
    MSequence* pkMSequence = dynamic_cast<MSequence*>(pkObject);
    NIASSERT(pkMSequence);
    return this->Name->CompareTo(pkMSequence->Name);
}
//---------------------------------------------------------------------------
void MSequence::RebuildTransitionsArray()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    unsigned int uiCount = m_pkKFMSequence->GetTransitions().GetCount();
    MTransition* aNewTransitions[] = NULL;
    if (uiCount > 0)
    {
        aNewTransitions = NiExternalNew MTransition*[uiCount];
        int uiIndex = 0;
        NiTMapIterator pos = m_pkKFMSequence->GetTransitions().GetFirstPos();
        while (pos)
        {
            unsigned int uiID;
            NiKFMTool::Transition* pkTransition;
            m_pkKFMSequence->GetTransitions().GetNext(pos, uiID, pkTransition);

            MTransition* pkMTransition = NULL;
            if (m_aTransitions != NULL)
            {
                for (int i = 0; i < m_aTransitions->Length; i++)
                {
                    if (m_aTransitions[i]->DesID != uiID)
                        continue;

                    if (m_aTransitions[i]->GetTransition() == pkTransition)
                    {
                        pkMTransition = m_aTransitions[i];
                        break;
                    }
                }
            }

            if (pkMTransition == NULL)
            {
                pkMTransition = NiExternalNew MTransition(m_pkAnimation,
                    this->SequenceID, uiID, pkTransition);
            }

            aNewTransitions[uiIndex++] = pkMTransition;
        }
    }
    m_aTransitions = aNewTransitions;

    pkData->Unlock();
}
//---------------------------------------------------------------------------
MTransition* MSequence::GetTransition(unsigned int uiDesID)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    
    if (m_aTransitions == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < m_aTransitions->Length; i++)
    {
        MTransition* pkTransition = m_aTransitions[i];
        if (pkTransition->DesID == uiDesID)
        {
            return pkTransition;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
float MSequence::GetKeyTimeAt(String* strKeyText)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    const char* pcKeyText = MStringToCharPointer(strKeyText);
    float fKeyTime = m_pkSeqData->GetKeyTimeAt(pcKeyText);
    MFreeCharPointer(pcKeyText);

    pkData->Unlock();

    if (fKeyTime == NiSequenceData::INVALID_TIME)
    {
        return INVALID_TIME;
    }

    return fKeyTime;
}
//---------------------------------------------------------------------------
float MSequence::GetKeyTimeAtDivFreq(String* strKeyText)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    const char* pcKeyText = MStringToCharPointer(strKeyText);
    float fKeyTime = m_pkSeqData->GetKeyTimeAt(pcKeyText);
    MFreeCharPointer(pcKeyText);

    if (fKeyTime != NiSequenceData::INVALID_TIME)
    {
        fKeyTime = m_pkSeqData->TimeDivFreq(fKeyTime);
    }

    pkData->Unlock();

    if (fKeyTime == NiSequenceData::INVALID_TIME)
    {
        return INVALID_TIME;
    }

    return fKeyTime;
}
//---------------------------------------------------------------------------
float MSequence::TimeDivFreq(float fTime)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    if (fTime != NiKFMTool::MAX_DURATION)
    {
        fTime = m_pkSeqData->TimeDivFreq(fTime);
    }
    pkData->Unlock();

    return fTime;
}
//---------------------------------------------------------------------------
float MSequence::TimeMultFreq(float fTime)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    if (fTime != NiKFMTool::MAX_DURATION)
    {
        fTime = m_pkSeqData->TimeMultFreq(fTime);
    }
    pkData->Unlock();

    return fTime;
}
//---------------------------------------------------------------------------
void MSequence::StoreAccum()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (pkActorManager)
    {
        NIASSERT(pkActorManager->GetControllerManager());
        pkActorManager->GetControllerManager()->GetAccumulatedTransform(
            *m_pkStoredAccum);
    }
    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MSequence::RestoreAccum()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (pkActorManager)
    {
        NIASSERT(pkActorManager->GetControllerManager());
        pkActorManager->GetControllerManager()->SetAccumulatedTransform(
            *m_pkStoredAccum);
    }
    pkData->Unlock();
}
//---------------------------------------------------------------------------
bool MSequence::IsDeleted()
{
    return m_bDisposed;
}
//---------------------------------------------------------------------------
String* MSequence::ToString()
{
    return Name;
}
//---------------------------------------------------------------------------
bool MSequence::CanSyncTo(MSequence* pkMSequence)
{
    bool bReturn = false;
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    if (m_pkSeqData)
    {
        bReturn = m_pkSeqData->CanSyncTo(
           pkMSequence->m_pkSeqData);
    }
    pkData->Unlock();
    return bReturn;
}
//---------------------------------------------------------------------------
float MSequence::FindCorrespondingMorphFrame(
    MSequence* pkPartnerMSequence, float fFrameTime)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fMorphFrame = m_pkSeqData->FindCorrespondingMorphFrame(
        pkPartnerMSequence->GetSequenceData(), fFrameTime);
    pkData->Unlock();

    return fMorphFrame;
}
//---------------------------------------------------------------------------
NiSequenceData* MSequence::GetSequenceData()
{
    return m_pkSeqData;
}
//---------------------------------------------------------------------------
NiControllerSequence* MSequence::GetControllerSequence()
{
    return m_pkControllerSequence;
}
//---------------------------------------------------------------------------
void MSequence::SetControllerSequence(
    NiControllerSequence* pkControllerSequence)
{
    // Maintain ref counting.
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    if (pkControllerSequence)
    {
        pkControllerSequence->IncRefCount();
    }
    if (m_pkControllerSequence)
    {
        m_pkControllerSequence->DecRefCount();
    }
    m_pkControllerSequence = pkControllerSequence;
    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MSequence::FillArrays()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    NIASSERT(m_pkSeqData);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    ClearArrays();

    unsigned int uiCount = m_pkSeqData->GetNumEvaluators();
    if (uiCount > 0)
    {
        m_aEvaluatorInfo = NiExternalNew MEvaluatorInfo*[uiCount];
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            if (m_pkSeqData->GetEvaluatorAt(ui) != NULL)
            {
                m_aEvaluatorInfo[ui] = NiExternalNew MEvaluatorInfo(
                    m_pkSeqData->GetEvaluatorAt(ui));
            }
            else
            {
                m_aEvaluatorInfo[ui] = NULL;
            }
        }
    }

    NiTextKeyExtraData* pkTextKeyExtraData = 
        m_pkSeqData->GetTextKeys();
    unsigned int uiNumKeys = 0;
    if (pkTextKeyExtraData)
    {
        NiTextKey* pkTextKeys = pkTextKeyExtraData->GetKeys(uiNumKeys);
        if (uiNumKeys > 0)
        {
            m_aTextKeys = NiExternalNew MTextKey*[uiNumKeys];
            for (unsigned int ui = 0; ui < uiNumKeys; ui++)
            {
                 m_aTextKeys[ui] = NiExternalNew MTextKey(&pkTextKeys[ui]);
            }
        }
    }
    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MSequence::ClearArrays()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    m_aEvaluatorInfo = NULL;
    m_aTextKeys = NULL;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Properties
//---------------------------------------------------------------------------
unsigned int MSequence::get_SequenceID()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    unsigned int uiSequenceID = m_pkKFMSequence->GetSequenceID();
    pkData->Unlock();

    return uiSequenceID;
}
//---------------------------------------------------------------------------
void MSequence::set_SequenceID(unsigned int uiSequenceID)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    unsigned int uiOldID = m_pkKFMSequence->GetSequenceID();
    pkData->Unlock();

    NIASSERT(m_pkAnimation);
    if (m_pkAnimation->ChangeSequenceID(uiOldID, uiSequenceID))
    {
        m_pkAnimation->ThrowSequenceModifiedEvent(Prop_SequenceID, this);
    }
}
//---------------------------------------------------------------------------
String* MSequence::get_Name()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    String* strName = m_pkSeqData->GetName();
    pkData->Unlock();

    return strName;
}
//---------------------------------------------------------------------------
String* MSequence::get_Filename()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    String* strFilename = m_pkKFMSequence->GetFilename();
    pkData->Unlock();

    return strFilename;
}
//---------------------------------------------------------------------------
void MSequence::set_Filename(String* strFilename)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    const char* pcFilename = MStringToCharPointer(strFilename);
    m_pkKFMSequence->SetFilename(pcFilename);
    MFreeCharPointer(pcFilename);
    pkData->Unlock();

    NIASSERT(m_pkAnimation);
    m_pkAnimation->ThrowSequenceModifiedEvent(Prop_Filename, this);
}
//---------------------------------------------------------------------------
int MSequence::get_AnimIndex()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    int iAnimIndex = m_pkKFMSequence->GetAnimIndex();
    pkData->Unlock();

    return iAnimIndex;
}
//---------------------------------------------------------------------------
void MSequence::set_AnimIndex(int iAnimIndex)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkKFMSequence->SetAnimIndex(iAnimIndex);
    pkData->Unlock();

    NIASSERT(m_pkAnimation);
    m_pkAnimation->ThrowSequenceModifiedEvent(Prop_AnimIndex, this);
}
//---------------------------------------------------------------------------
float MSequence::get_Duration()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fDuration = m_pkSeqData->GetDuration();
    pkData->Unlock();

    return fDuration;
}
//---------------------------------------------------------------------------
bool MSequence::get_Loop()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    bool bLoop = (m_pkSeqData->GetCycleType() ==
        NiTimeController::LOOP);
    pkData->Unlock();

    return bLoop;
}
//---------------------------------------------------------------------------
float MSequence::get_Frequency()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fFrequency = m_pkSeqData->GetFrequency();
    pkData->Unlock();

    return fFrequency;
}
//---------------------------------------------------------------------------
#ifdef NDL_DEBUG
void MSequence::set_Frequency(float fFrequency)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkSeqData->SetFrequency(fFrequency);
    pkData->Unlock();
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
float MSequence::get_DurationDivFreq()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fDurationDivFreq = m_pkSeqData->GetDurationDivFreq();
    pkData->Unlock();

    return fDurationDivFreq;
}
//---------------------------------------------------------------------------
float MSequence::get_DefaultPositionTime()
{
    return m_fDefaultPositionTime;
}
//---------------------------------------------------------------------------
void MSequence::set_DefaultPositionTime(float fTime)
{
    m_fDefaultPositionTime = fTime;
}
//---------------------------------------------------------------------------
float MSequence::get_LastScaledTime()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fLastScaledTime = -NI_INFINITY;
    NIASSERT(m_pkControllerSequence);
    if (m_pkControllerSequence)
    {
        fLastScaledTime = m_pkControllerSequence->GetLastScaledTime();
    }
    pkData->Unlock();

    return fLastScaledTime;
}
//---------------------------------------------------------------------------
MSequence::AnimState MSequence::GetAnimState()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiAnimState eState = (NiAnimState)INACTIVE;
    if (m_pkControllerSequence)
    {
        eState = m_pkControllerSequence->GetState();
    }
    pkData->Unlock();
    return (MSequence::AnimState)eState;
}
//---------------------------------------------------------------------------
MTransition* MSequence::get_Transitions()[]
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    return m_aTransitions;
}
//---------------------------------------------------------------------------
MSequence::MEvaluatorInfo* MSequence::get_EvaluatorInfo()[]
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    return m_aEvaluatorInfo;
}
//---------------------------------------------------------------------------
MSequence::MTextKey* MSequence::get_TextKeys()[]
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    return m_aTextKeys;
}
//---------------------------------------------------------------------------
ArrayList* MSequence::GetSequenceNamesFromKF(String* pkFileName)
{
    const char* pcName = MStringToCharPointer(pkFileName);
    NiStream kStream;
    if (!kStream.Load(pcName))
        return NULL;

    unsigned int uiObjects = kStream.GetObjectCount();
    ArrayList* pkList = NiExternalNew ArrayList(uiObjects);

    for (unsigned int i = 0; i < uiObjects; i++)
    {
        NiControllerSequence* pkCont = NiDynamicCast(NiControllerSequence,
            kStream.GetObjectAt(i));
        if (!pkCont)
            continue;

        String* strName = NiExternalNew String(pkCont->GetName());
        pkList->Add(strName);
    }

    return pkList;
}
//---------------------------------------------------------------------------
