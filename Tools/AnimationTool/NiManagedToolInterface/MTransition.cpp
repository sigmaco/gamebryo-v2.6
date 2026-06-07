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
#include "MTransition.h"
#include "MSharedData.h"
#include "MAnimation.h"

using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MTransition::MBlendPair::MBlendPair(
    NiKFMTool::Transition::BlendPair* pkBlendPair) :
    m_pkBlendPair(pkBlendPair)
{
    NIASSERT(m_pkBlendPair);
}
//---------------------------------------------------------------------------
void MTransition::MBlendPair::DeleteContents()
{
}
//---------------------------------------------------------------------------
String* MTransition::MBlendPair::get_StartKey()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    String* strStartKey = m_pkBlendPair->GetStartKey();
    pkData->Unlock();

    return strStartKey;
}
//---------------------------------------------------------------------------
void MTransition::MBlendPair::set_StartKey(String* strStartKey)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    const char* pcStartKey = MStringToCharPointer(strStartKey);
    m_pkBlendPair->SetStartKey(pcStartKey);
    MFreeCharPointer(pcStartKey);
    pkData->Unlock();

    OnBlendPairChanged(Prop_StartKey, this);
}
//---------------------------------------------------------------------------
String* MTransition::MBlendPair::get_TargetKey()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    String* strTargetKey = m_pkBlendPair->GetTargetKey();
    if (strTargetKey == String::Empty)
    {
        strTargetKey = NiAnimationConstants::GetStartTextKey();
    }
    pkData->Unlock();

    return strTargetKey;
}
//---------------------------------------------------------------------------
void MTransition::MBlendPair::set_TargetKey(String* strTargetKey)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    const char* pcTargetKey = MStringToCharPointer(strTargetKey);
    m_pkBlendPair->SetTargetKey(pcTargetKey);
    MFreeCharPointer(pcTargetKey);
    pkData->Unlock();

    OnBlendPairChanged(Prop_TargetKey, this);
}
//---------------------------------------------------------------------------
MTransition::MChainInfo::MChainInfo(
    NiKFMTool::Transition::ChainInfo* pkChainInfo) :
    m_pkChainInfo(pkChainInfo)
{
    NIASSERT(m_pkChainInfo);
}
//---------------------------------------------------------------------------
void MTransition::MChainInfo::DeleteContents()
{
}
//---------------------------------------------------------------------------
unsigned int MTransition::MChainInfo::get_SequenceID()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    unsigned int uiSequenceID = m_pkChainInfo->GetSequenceID();
    pkData->Unlock();

    return uiSequenceID;
}
//---------------------------------------------------------------------------
float MTransition::MChainInfo::get_Duration()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fDuration = m_pkChainInfo->GetDuration();
    pkData->Unlock();

    return fDuration;
}
//---------------------------------------------------------------------------
void MTransition::MChainInfo::set_Duration(float fDuration)
{
    if (fDuration < 0.0f && fDuration != MAX_DURATION)
    {
        return;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkChainInfo->SetDuration(fDuration);
    pkData->Unlock();

    OnChainInfoChanged(Prop_Duration, this);
}
//---------------------------------------------------------------------------
MTransition::MTransition(MAnimation* pkAnimation, unsigned int uiSrcID,
    unsigned int uiDesID, NiKFMTool::Transition* pkTransition) :
    m_pkAnimation(pkAnimation), m_uiSrcID(uiSrcID), m_uiDesID(uiDesID),
    m_pkTransition(pkTransition), m_aBlendPairs(NULL), m_aChainInfo(NULL)
{
    NIASSERT(m_pkTransition);
    NIASSERT(m_pkAnimation);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    BuildBlendPairs();
    BuildChainInfo();
    pkData->Unlock();

    DefaultPositionTime = 0;
    DefaultTransitionTime = 0;
}
//---------------------------------------------------------------------------
MTransition::MBlendPair* MTransition::GetNextBlendPair(float fTime)
{
    if (Type != MTransition::TransitionType::Trans_DelayedBlend)
        return NULL;

    // Must find the Lowest bp after fTime
    MSequence* pkSrc = Source;
    
    // Only makes sense for us to deal with time with respect
    // to the sequence data, not system animation time.
    NIASSERT(fTime <= pkSrc->DurationDivFreq);

    MTransition::MBlendPair* paBP[] = BlendPairs;
    MTransition::MBlendPair* pkNextBP = NULL;
    MTransition::MBlendPair* pkSmallestBP = NULL;

    for(int i=0; i<paBP->Length; i++)
    {
        MTransition::MBlendPair* pkBP = paBP[i];
        float fThisBPTime = pkSrc->GetKeyTimeAt(pkBP->StartKey);

        // Record the smallest blend pair time
        if (pkSmallestBP == NULL)
            pkSmallestBP = pkBP;
        else if (fThisBPTime < pkSrc->GetKeyTimeAt(pkSmallestBP->StartKey))
            pkSmallestBP = pkBP;

        if (fThisBPTime > fTime)
        {
            // also must be lower than our current next
            if (pkNextBP == NULL)
                pkNextBP = pkBP;
            else if (fThisBPTime < pkSrc->GetKeyTimeAt(pkNextBP->StartKey))
                pkNextBP = pkBP;
        }
    }

    if (pkNextBP == NULL)
        pkNextBP = pkSmallestBP;

    return pkNextBP;
}
//---------------------------------------------------------------------------
void MTransition::BuildBlendPairs()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    unsigned int uiCount = m_pkTransition->GetBlendPairs().GetSize();
    if (uiCount > 0)
    {
        m_aBlendPairs = NiExternalNew MBlendPair*[uiCount];
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            m_aBlendPairs[ui] = NiExternalNew MBlendPair(
                m_pkTransition->GetBlendPairs().GetAt(ui));
            __hook(&MBlendPair::OnBlendPairChanged, (m_aBlendPairs[ui]),
                &NiManagedToolInterface::MTransition::OnBlendPairChanged);
        }
    }
    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MTransition::BuildChainInfo()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    unsigned int uiCount = m_pkTransition->GetChainInfo().GetSize();
    if (uiCount > 0)
    {
        m_aChainInfo = NiExternalNew MChainInfo*[uiCount];
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            m_aChainInfo[ui] = NiExternalNew MChainInfo(
                &m_pkTransition->GetChainInfo().GetAt(ui));
            __hook(&MChainInfo::OnChainInfoChanged, (m_aChainInfo[ui]),
                &NiManagedToolInterface::MTransition::OnChainInfoChanged);
        }
    }
    else
    {
        m_aChainInfo = NULL;
    }

    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MTransition::DeleteContents()
{
    if (m_aBlendPairs != NULL)
    {
        for (int i = 0; i < m_aBlendPairs->Length; i++)
        {
            m_aBlendPairs[i]->DeleteContents();
        }
    }
    if (m_aChainInfo != NULL)
    {
        for (int i = 0; i < m_aChainInfo->Length; i++)
        {
            m_aChainInfo[i]->DeleteContents();
        }
    }
}
//---------------------------------------------------------------------------
void MTransition::ReplaceChain_NoEvent(MSequence* kSequences[], 
    float fDurations __gc [])
{
    System::Diagnostics::Debug::Assert(kSequences != NULL);
    System::Diagnostics::Debug::Assert(fDurations != NULL);
    System::Diagnostics::Debug::Assert(kSequences->Length == 
        fDurations->Length);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    
    int i;
    while (ChainInfo != NULL)
        PopChainInfo();

    for (i = 0; i < kSequences->Length; i++)
    {
        MSequence* pkSequence = 
            dynamic_cast<MSequence*>(kSequences->GetValue(i));
        float fDuration = fDurations[i];
        PushChainInfo(pkSequence->SequenceID, fDuration);
    }
    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MTransition::ReplaceChain(MSequence* kSequences[], 
    float fDurations __gc [])
{
    ReplaceChain_NoEvent(kSequences, fDurations);
    m_pkAnimation->ThrowTransitionModifiedEvent(PropertyType::Prop_ChainInfo,
        this);
}
//---------------------------------------------------------------------------
void MTransition::ConvertToChain(MSequence* kSequences[], 
    float fDurations __gc [])
{
    if (TransitionType::Trans_Chain == this->StoredType)
        return;

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool::TransitionType eKFMType = TranslateTransitionType(
        TransitionType::Trans_Chain);

    m_pkAnimation->RemoveChainsContainingTransitions(m_uiSrcID, m_uiDesID);

    m_pkTransition = m_pkAnimation->ChangeTransitionType(m_uiSrcID, m_uiDesID,
        m_pkTransition, eKFMType);
    NIASSERT(m_pkTransition);
    BuildBlendPairs();

    ReplaceChain_NoEvent(kSequences, fDurations);

    pkData->Unlock();
    m_pkAnimation->ThrowTransitionModifiedEvent(Prop_Type, this);
}
//---------------------------------------------------------------------------
NiKFMTool::TransitionType MTransition::TranslateTransitionType(
    TransitionType eType)
{
    NiKFMTool::TransitionType eKFMType;
    switch (eType)
    {
    case Trans_ImmediateBlend:
    case Trans_DelayedBlend:
        eKFMType = NiKFMTool::TYPE_BLEND;
        break;
    case Trans_Morph:
        eKFMType = NiKFMTool::TYPE_MORPH;
        break;
    case Trans_CrossFade:
        eKFMType = NiKFMTool::TYPE_CROSSFADE;
        break;
    case Trans_Chain:
        eKFMType = NiKFMTool::TYPE_CHAIN;
        break;
    case Trans_DefaultSync:
        eKFMType = NiKFMTool::TYPE_DEFAULT_SYNC;
        break;
    case Trans_DefaultNonSync:
        eKFMType = NiKFMTool::TYPE_DEFAULT_NONSYNC;
        break;
    default:
        eKFMType = NiKFMTool::TYPE_DEFAULT_INVALID;
        NIASSERT(false);
        break;
    }

    return eKFMType;
}
//---------------------------------------------------------------------------
MTransition::TransitionType MTransition::TranslateTransitionType(
    NiKFMTool::TransitionType eKFMType)
{
    return TranslateTransitionType(eKFMType, this);
}
//---------------------------------------------------------------------------
MTransition::TransitionType MTransition::TranslateTransitionType(
    NiKFMTool::TransitionType eKFMType, MTransition* pkTransition)
{
    TransitionType eType;
    switch (eKFMType)
    {
    case NiKFMTool::TYPE_BLEND:
        if (pkTransition)
        {
            NiKFMTool::Transition* pkKFMTrans = pkTransition->m_pkTransition;
            if (pkKFMTrans->GetBlendPairs().GetSize() == 0)
            {
                eType = Trans_ImmediateBlend;
            }
            else if (pkKFMTrans->GetBlendPairs().GetSize() == 1 &&
                !pkKFMTrans->GetBlendPairs().GetAt(0)->GetStartKey().Exists())
            {
                eType = Trans_ImmediateBlend;
            }
            else
            {
                eType = Trans_DelayedBlend;
            }
        }
        else
        {
            eType = Trans_ImmediateBlend;
        }
        break;
    case NiKFMTool::TYPE_MORPH:
        eType = Trans_Morph;
        break;
    case NiKFMTool::TYPE_CROSSFADE:
        eType = Trans_CrossFade;
        break;
    case NiKFMTool::TYPE_CHAIN:
        eType = Trans_Chain;
        break;
    case NiKFMTool::TYPE_DEFAULT_SYNC:
        eType = Trans_DefaultSync;
        break;
    case NiKFMTool::TYPE_DEFAULT_NONSYNC:
        eType = Trans_DefaultNonSync;
        break;
    default:
        eType = Trans_None;
        NIASSERT(false);
        break;
    }

    return eType;
}
//---------------------------------------------------------------------------
System::Collections::ArrayList* MTransition::GetValidTypesForTransition(
    MSequence* pkSrc, MSequence* pkDest)
{
    System::Collections::ArrayList* pkList = NiExternalNew 
        System::Collections::ArrayList();
    
    if (pkSrc == pkDest || pkSrc->SequenceID == pkDest->SequenceID)
    {
        pkList->Add(__box(Trans_None));
        return pkList;
    }

    pkList->Add(__box(Trans_ImmediateBlend));
    pkList->Add(__box(Trans_DelayedBlend));

    bool bCanSyncTo = pkSrc->CanSyncTo(pkDest);
    if (bCanSyncTo)
        pkList->Add(__box(Trans_Morph));
    
    pkList->Add(__box(Trans_CrossFade));
    pkList->Add(__box(Trans_Chain));
    pkList->Add(__box(Trans_DefaultNonSync));
    
    if (bCanSyncTo)
        pkList->Add(__box(Trans_DefaultSync));

    pkList->Add(__box(Trans_None));
    return pkList;
}
//---------------------------------------------------------------------------
bool MTransition::DoesBlendPairExistForSrcKey(String* strSrcKey)
{
    if (m_aBlendPairs == NULL)
        return false;

    for (int i = 0; i < m_aBlendPairs->Length; i++)
    {
        if (m_aBlendPairs[i]->StartKey->CompareTo(strSrcKey) == 0)
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
int MTransition::GetBlendPairIndex(String* strSrcKey, String* strDestKey)
{
    if (m_aBlendPairs != NULL)
    {
        for (int i = 0; i < m_aBlendPairs->Length; i++)
        {
            if (m_aBlendPairs[i]->StartKey->CompareTo(strSrcKey) == 0 &&
                m_aBlendPairs[i]->TargetKey->CompareTo(strDestKey) == 0)
            {
                return i;
            }
        }
    }

    return -1;
}
//---------------------------------------------------------------------------
void MTransition::AddBlendPair(String* strSrcKey, String* strDestKey)
{
    if (GetBlendPairIndex(strSrcKey, strDestKey) != -1)
        return;
    
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NIASSERT(pkData->GetKFMTool() != NULL);
    const char* pcSrcKey = MStringToCharPointer(strSrcKey);
    const char* pcDestKey = MStringToCharPointer(strDestKey);
    pkData->GetKFMTool()->AddBlendPair(SrcID, DesID, pcSrcKey, pcDestKey);
    MFreeCharPointer(pcSrcKey);
    MFreeCharPointer(pcDestKey);
    BuildBlendPairs();
    pkData->Unlock();

    m_pkAnimation->ThrowTransitionModifiedEvent(PropertyType::Prop_BlendPairs,
        this);
}
//---------------------------------------------------------------------------
void MTransition::RemoveBlendPair(String* strSrcKey, String* strDestKey)
{
    if (GetBlendPairIndex(strSrcKey, strDestKey) == -1)
        return;
    
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NIASSERT(pkData->GetKFMTool() != NULL);
    const char* pcSrcKey = MStringToCharPointer(strSrcKey);
    const char* pcDestKey = MStringToCharPointer(strDestKey);
    pkData->GetKFMTool()->RemoveBlendPair(SrcID, DesID, pcSrcKey, pcDestKey);
    MFreeCharPointer(pcSrcKey);
    MFreeCharPointer(pcDestKey);
    BuildBlendPairs();
    pkData->Unlock();

    m_pkAnimation->ThrowTransitionModifiedEvent(PropertyType::Prop_BlendPairs,
        this);
}
//---------------------------------------------------------------------------
int MTransition::GetChainInfoIndex(unsigned int uiSequenceID, int iOccurance)
{
    if (m_aChainInfo != NULL)
    {
        for (int i = 0; i < m_aChainInfo->Length; i++)
        {
            if (m_aChainInfo[i]->SequenceID == uiSequenceID)
            {
                if (iOccurance == 0)
                    return i;
                else
                    iOccurance--;
            }
        }
    }

    return -1;
}
//---------------------------------------------------------------------------
void MTransition::PushChainInfo(unsigned int uiSequenceID,
    float fDuration)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NIASSERT(pkData->GetKFMTool() != NULL);
    NiKFMTool::KFM_RC rc = pkData->GetKFMTool()->AddSequenceToChain(SrcID,
        DesID, uiSequenceID, fDuration);
    System::Diagnostics::Debug::Assert(rc == NiKFMTool::KFM_SUCCESS);
    BuildChainInfo();
    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MTransition::PopChainInfo()
{
    if (m_aChainInfo->Length == 0)
        return;

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NIASSERT(pkData->GetKFMTool() != NULL);
    NiKFMTool::KFM_RC rc = pkData->GetKFMTool()->RemoveSequenceFromChain(
        SrcID, DesID, m_aChainInfo[m_aChainInfo->Length-1]->SequenceID);
    System::Diagnostics::Debug::Assert(rc == NiKFMTool::KFM_SUCCESS);
    BuildChainInfo();
    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MTransition::RebuildChainInfoArray()
{
    BuildChainInfo();
}
//---------------------------------------------------------------------------
NiKFMTool::Transition* MTransition::GetTransition()
{
    return m_pkTransition;
}
//---------------------------------------------------------------------------
void MTransition::OnBlendPairChanged(MBlendPair::PropertyType,
    MBlendPair*)
{
    m_pkAnimation->ThrowTransitionModifiedEvent(PropertyType::Prop_BlendPairs,
        this);
}
//---------------------------------------------------------------------------
void MTransition::OnChainInfoChanged(MChainInfo::PropertyType,
    MChainInfo*)
{
    m_pkAnimation->ThrowTransitionModifiedEvent(PropertyType::Prop_ChainInfo,
        this);
}
//---------------------------------------------------------------------------
void MTransition::ChangeSequenceID(unsigned int uiOldID, unsigned int uiNewID)
{
    if (m_uiSrcID == uiOldID)
    {
        m_uiSrcID = uiNewID;
    }
    if (m_uiDesID == uiOldID)
    {
        m_uiDesID = uiNewID;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Properties
//---------------------------------------------------------------------------
unsigned int MTransition::get_SrcID()
{
    return m_uiSrcID;
}
//---------------------------------------------------------------------------
unsigned int MTransition::get_DesID()
{
    return m_uiDesID;
}
//---------------------------------------------------------------------------
MTransition::TransitionType MTransition::get_Type()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool::TransitionType eKFMType = m_pkTransition->GetType();
    pkData->Unlock();

    return TranslateTransitionType(eKFMType);
}
//---------------------------------------------------------------------------
MTransition::TransitionType MTransition::get_StoredType()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool::TransitionType eKFMType = m_pkTransition->GetStoredType();
    pkData->Unlock();

    return TranslateTransitionType(eKFMType);
}
//---------------------------------------------------------------------------
void MTransition::set_StoredType(TransitionType eType)
{
    if (eType == this->StoredType)
        return;

    System::Diagnostics::Debug::Assert(eType != TransitionType::Trans_Chain, 
        "Please use the \"ConvertToChain\" method instead");

    if (eType == TransitionType::Trans_Chain)
        return;

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool::TransitionType eKFMType = TranslateTransitionType(eType);
    m_pkTransition = m_pkAnimation->ChangeTransitionType(m_uiSrcID, m_uiDesID,
        m_pkTransition, eKFMType);
    NIASSERT(m_pkTransition);
    
    if (eType == TransitionType::Trans_DelayedBlend)
    {
        m_pkTransition->ClearBlendPairs();
        NIASSERT(pkData->GetKFMTool() != NULL);
        pkData->GetKFMTool()->AddBlendPair(SrcID, DesID, 
            NiAnimationConstants::GetEndTextKey(), 
            NiAnimationConstants::GetStartTextKey());
    }
    else if (eType == TransitionType::Trans_ImmediateBlend)
    {
        NIASSERT(pkData->GetKFMTool() != NULL);
        m_pkTransition->ClearBlendPairs();
    }

    BuildBlendPairs();
    BuildChainInfo();
    pkData->Unlock();

    m_pkAnimation->ThrowTransitionModifiedEvent(Prop_Type, this);
}
//---------------------------------------------------------------------------
float MTransition::get_Duration()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fDuration = m_pkTransition->GetDuration();
    pkData->Unlock();

    return fDuration;
}
//---------------------------------------------------------------------------
void MTransition::set_Duration(float fDuration)
{
    if (fDuration <= 0.0f)
    {
        return;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkTransition->SetDuration(fDuration);
    pkData->Unlock();

    m_pkAnimation->ThrowTransitionModifiedEvent(Prop_Duration, this);
}
//---------------------------------------------------------------------------
MTransition::MBlendPair* MTransition::get_BlendPairs()[]
{
    return m_aBlendPairs;
}
//---------------------------------------------------------------------------
MTransition::MChainInfo* MTransition::get_ChainInfo()[]
{
    return m_aChainInfo;
}
//---------------------------------------------------------------------------
MSequence* MTransition::get_Source()
{
    return m_pkAnimation->GetSequence(SrcID);
}
//---------------------------------------------------------------------------
MSequence* MTransition::get_Destination()
{
    return m_pkAnimation->GetSequence(DesID);
}
//---------------------------------------------------------------------------
String* MTransition::get_ImmediateOffsetTextKey()
{
    if (m_pkTransition->GetBlendPairs().GetSize() == 1 &&
        m_pkTransition->GetBlendPairs().GetAt(0) != NULL &&
        !m_pkTransition->GetBlendPairs().GetAt(0)->GetStartKey().Exists())
    {
        return NiExternalNew String(m_pkTransition->GetBlendPairs().GetAt(0)
            ->GetTargetKey());
    }
    return NiAnimationConstants::GetStartTextKey();
}
//---------------------------------------------------------------------------
void MTransition::set_ImmediateOffsetTextKey(String* strTargetKey)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    if (strTargetKey == NULL || 
        strTargetKey->CompareTo(NiAnimationConstants::GetStartTextKey()) == 0)
    {
        m_pkTransition->ClearBlendPairs();
        BuildBlendPairs();
        pkData->Unlock();
        m_pkAnimation->ThrowTransitionModifiedEvent(
            Prop_ImmediateOffsetTextKey, this);
        return;
    }


    NiKFMTool::Transition::BlendPair* pkBlendPair = NULL;
    if (m_pkTransition->GetBlendPairs().GetSize() == 0)
    {
        pkBlendPair = NiNew NiKFMTool::Transition::BlendPair();
        pkBlendPair->SetStartKey(NULL);
        pkBlendPair->SetTargetKey(NULL);
        m_pkTransition->GetBlendPairs().Add(pkBlendPair);

    }
    else if (m_pkTransition->GetBlendPairs().GetSize() == 1 &&
        m_pkTransition->GetBlendPairs().GetAt(0) != NULL &&
        !m_pkTransition->GetBlendPairs().GetAt(0)->GetStartKey().Exists())
    {
        pkBlendPair = m_pkTransition->GetBlendPairs().GetAt(0);
    }
    else
    {
        NIASSERT(false);
        pkData->Unlock();
        return;
    }

    const char* pcDestKey = MStringToCharPointer(strTargetKey);
    pkBlendPair->SetTargetKey(pcDestKey);
    MFreeCharPointer(pcDestKey);

    BuildBlendPairs();
    pkData->Unlock();
    m_pkAnimation->ThrowTransitionModifiedEvent(Prop_ImmediateOffsetTextKey,
        this);
}
//---------------------------------------------------------------------------
float MTransition::get_DefaultTransitionTime()
{
    return m_fDefaultTransitionTime;
}
//---------------------------------------------------------------------------
void MTransition::set_DefaultTransitionTime(float fTime)
{
    m_fDefaultTransitionTime = fTime;
}
//---------------------------------------------------------------------------
float MTransition::get_DefaultPositionTime()
{
    return m_fDefaultPositionTime;
}
//---------------------------------------------------------------------------
void MTransition::set_DefaultPositionTime(float fTime)
{
    m_fDefaultPositionTime = fTime;
}
//---------------------------------------------------------------------------
MTransitionData::MTransitionData(MTransition* pkTransition,
    MSequence* pkSource, MSequence* pkDestination)
{
    m_pkTransition = pkTransition;
    m_pkSource = pkSource;
    m_pkDestination = pkDestination;
}
//---------------------------------------------------------------------------
MSequence* MTransitionData::get_Source()
{
    return m_pkSource;
}
//---------------------------------------------------------------------------
MSequence* MTransitionData::get_Destination()
{
    return m_pkDestination;
}
//---------------------------------------------------------------------------
MTransition* MTransitionData::get_Transition()
{
    return m_pkTransition;
}
//---------------------------------------------------------------------------
void MTransitionData::set_Transition(MTransition* pkTran)
{
    m_pkTransition = pkTran;
}
//---------------------------------------------------------------------------
