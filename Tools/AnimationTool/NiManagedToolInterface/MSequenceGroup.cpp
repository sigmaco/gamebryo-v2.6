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
#include "MSequenceGroup.h"
#include "MSharedData.h"
#include "MAnimation.h"

using namespace NiManagedToolInterface;
//---------------------------------------------------------------------------
MSequenceGroup::MSequenceInfo::MSequenceInfo(MAnimation* pkAnimation,
    MSequenceGroup* pkOwner, NiKFMTool::SequenceGroup::SequenceInfo* 
    pkInfo) : m_pkInfo(pkInfo), m_pkAnimation(pkAnimation), m_pkOwner(pkOwner)
{
    NIASSERT(m_pkInfo);
    NIASSERT(m_pkOwner);
    NIASSERT(m_pkAnimation);
    m_bDisposed = false;
    DefaultActiveValue = true;
}
//---------------------------------------------------------------------------
void MSequenceGroup::MSequenceInfo::DeleteContents()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    m_pkAnimation = NULL;
    m_pkOwner = NULL;
    m_bDisposed = true;
}
//---------------------------------------------------------------------------
bool MSequenceGroup::MSequenceInfo::CanSyncTo(
    unsigned int uiSynchronizeToSequenceID)
{
    MSequence* pkTargetMSequence = m_pkAnimation->GetSequence(
        uiSynchronizeToSequenceID);
    if (pkTargetMSequence == NULL)
        return false;
    return Sequence->CanSyncTo(pkTargetMSequence);
}
//---------------------------------------------------------------------------
unsigned int MSequenceGroup::MSequenceInfo::get_SequenceID()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    unsigned int uiSequenceID = m_pkInfo->GetSequenceID();
    pkData->Unlock();

    return uiSequenceID;
}
//---------------------------------------------------------------------------
MSequence* MSequenceGroup::MSequenceInfo::get_Sequence()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    MSequence* pkMSequence = m_pkAnimation->GetSequence(
        m_pkInfo->GetSequenceID());
    pkData->Unlock();
    return pkMSequence;
}
//---------------------------------------------------------------------------
String* MSequenceGroup::MSequenceInfo::get_Name()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    MSequence* pkMSequence = m_pkAnimation->GetSequence(
        m_pkInfo->GetSequenceID());
    pkData->Unlock();

    if (pkMSequence == NULL)
        return NULL;
    else
        return pkMSequence->Name;
}
//---------------------------------------------------------------------------
int MSequenceGroup::MSequenceInfo::get_Priority()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    int iPriority = m_pkInfo->GetPriority();
    pkData->Unlock();

    return iPriority;
}
//---------------------------------------------------------------------------
void MSequenceGroup::MSequenceInfo::set_Priority(int iPriority)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkInfo->SetPriority(iPriority);
    pkData->Unlock();

    NIASSERT(m_pkAnimation);
    m_pkAnimation->ThrowSequenceGroupSequenceInfoModifiedEvent(
        Prop_Priority, this, m_pkOwner);
}
//---------------------------------------------------------------------------
float MSequenceGroup::MSequenceInfo::get_Weight()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fWeight = m_pkInfo->GetWeight();
    pkData->Unlock();

    return fWeight;
}
//---------------------------------------------------------------------------
void MSequenceGroup::MSequenceInfo::set_Weight(float fWeight)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    if (fWeight < 0.0f || fWeight > 1.0f)
    {
        return;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkInfo->SetWeight(fWeight);
    pkData->Unlock();

    NIASSERT(m_pkAnimation);
    m_pkAnimation->ThrowSequenceGroupSequenceInfoModifiedEvent(
        Prop_Weight, this, m_pkOwner);
}
//---------------------------------------------------------------------------
float MSequenceGroup::MSequenceInfo::get_EaseInTime()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fEaseInTime = m_pkInfo->GetEaseInTime();
    pkData->Unlock();

    return fEaseInTime;
}
//---------------------------------------------------------------------------
void MSequenceGroup::MSequenceInfo::set_EaseInTime(float fEaseInTime)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkInfo->SetEaseInTime(fEaseInTime);
    pkData->Unlock();
    NIASSERT(m_pkAnimation);
    m_pkAnimation->ThrowSequenceGroupSequenceInfoModifiedEvent(
        Prop_EaseInTime, this, m_pkOwner);
}
//---------------------------------------------------------------------------
float MSequenceGroup::MSequenceInfo::get_EaseOutTime()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fEaseOutTime = m_pkInfo->GetEaseOutTime();
    pkData->Unlock();

    return fEaseOutTime;
}
//---------------------------------------------------------------------------
void MSequenceGroup::MSequenceInfo::set_EaseOutTime(float fEaseOutTime)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkInfo->SetEaseOutTime(fEaseOutTime);
    pkData->Unlock();

    NIASSERT(m_pkAnimation);
    m_pkAnimation->ThrowSequenceGroupSequenceInfoModifiedEvent(
        Prop_EaseOutTime, this, m_pkOwner);
}
//---------------------------------------------------------------------------
bool MSequenceGroup::MSequenceInfo::get_Synchronized()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    bool bSynched = m_pkInfo->GetSynchronizeSequenceID() != 
        NiKFMTool::SYNC_SEQUENCE_ID_NONE;
    pkData->Unlock();

    return bSynched;
}
//---------------------------------------------------------------------------
unsigned int MSequenceGroup::MSequenceInfo::get_SynchronizeToSequenceID()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    unsigned int uiSeq = m_pkInfo->GetSynchronizeSequenceID();
    pkData->Unlock();

    return uiSeq;
}
//---------------------------------------------------------------------------
void MSequenceGroup::MSequenceInfo::ResetSyncSequence()
{
    SynchronizeToSequenceID =  NiKFMTool::SYNC_SEQUENCE_ID_NONE;
}
//---------------------------------------------------------------------------
void MSequenceGroup::MSequenceInfo::set_SynchronizeToSequenceID(unsigned int 
    uiSynchronizeToSequenceID)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkInfo->SetSynchronizeSequenceID(uiSynchronizeToSequenceID);
    pkData->Unlock();

    NIASSERT(m_pkAnimation);
    m_pkAnimation->ThrowSequenceGroupSequenceInfoModifiedEvent(
        Prop_SynchronizeToSequence, this, m_pkOwner);
}
//---------------------------------------------------------------------------
bool  MSequenceGroup::MSequenceInfo::get_Activated()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    return Sequence->GetAnimState() != MSequence::INACTIVE;
}
//---------------------------------------------------------------------------
bool  MSequenceGroup::MSequenceInfo::get_DefaultActiveValue()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    return m_bDefaultActiveValue;
}
//---------------------------------------------------------------------------
void MSequenceGroup::MSequenceInfo::set_DefaultActiveValue(
    bool bDefaultActiveValue)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    m_bDefaultActiveValue = bDefaultActiveValue;
}
//---------------------------------------------------------------------------
String* MSequenceGroup::MSequenceInfo::ToString()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    return Name;
}
//---------------------------------------------------------------------------
bool MSequenceGroup::MSequenceInfo::ActivateImmediate()
{
    bool bSuccess = false;

    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    if (!Activated)
    {
        MSharedData* pkData = MSharedData::Instance;
        pkData->Lock();
        if (Synchronized)
        {
            bSuccess = m_pkAnimation->ActivateSequence(SequenceID, Priority, 
                Weight, 0.0f, SynchronizeToSequenceID);
        }
        else
        {
            bSuccess = m_pkAnimation->ActivateSequence(SequenceID, Priority, 
                Weight, 0.0f);
        }
        pkData->Unlock();
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
bool MSequenceGroup::MSequenceInfo::Activate()
{
    bool bSuccess = false;

    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    if (!Activated)
    {
        MSharedData* pkData = MSharedData::Instance;
        pkData->Lock();
        if (Synchronized)
        {
            bSuccess = m_pkAnimation->ActivateSequence(SequenceID, Priority, 
                Weight, EaseInTime, SynchronizeToSequenceID);
        }
        else
        {
            bSuccess = m_pkAnimation->ActivateSequence(SequenceID, Priority, 
                Weight, EaseInTime);
        }
        pkData->Unlock();
    }

    DefaultActiveValue = bSuccess;
    return bSuccess;
}
//---------------------------------------------------------------------------
void  MSequenceGroup::MSequenceInfo::DeactivateImmediate()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    if (Activated)
    {
        MSharedData* pkData = MSharedData::Instance;
        pkData->Lock();
        m_pkAnimation->DeactivateSequence(SequenceID);
        pkData->Unlock();
    }
}
//---------------------------------------------------------------------------
bool MSequenceGroup::MSequenceInfo::Deactivate()
{
    bool bSuccess = false;

    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    if (Activated)
    {
        MSharedData* pkData = MSharedData::Instance;
        pkData->Lock();
        bSuccess = m_pkAnimation->DeactivateSequence(SequenceID, EaseOutTime);
        pkData->Unlock();
    }

    if (bSuccess)
        DefaultActiveValue = false;
    else
        DefaultActiveValue = true;

    return bSuccess;
}
//---------------------------------------------------------------------------
MSequenceGroup::MSequenceGroup(MAnimation* pkAnimation, 
    NiKFMTool::SequenceGroup* pkGroup) : m_pkAnimation(pkAnimation),
    m_pkGroup(pkGroup), m_aSequenceInfo(NULL)
{
    m_bDisposed = false;
    NIASSERT(pkAnimation);
    FillSequenceInfoArray();
}
//---------------------------------------------------------------------------
NiTMap<unsigned int, bool>* MSequenceGroup::CreateDefaultActiveValuesArray()
{
    // caller is required to delete the array when finished.

    unsigned int uiCount = m_pkGroup->GetSequenceInfo().GetSize();
    NiTMap<unsigned int, bool>* pkMapActiveValues = NULL;

    // Make an attempt to save existing Defaults
    if (uiCount > 0)
    {
        pkMapActiveValues = NiNew NiTMap<unsigned int, bool>(
            NiTMap<unsigned int, bool>::NextPrime(uiCount));

        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            NiKFMTool::SequenceGroup::SequenceInfo* pkSI =
                &m_pkGroup->GetSequenceInfo().GetAt(ui);
            unsigned int uiSeqID = pkSI->GetSequenceID();
            pkMapActiveValues->SetAt(uiSeqID, true);

            if (m_aSequenceInfo != NULL)
            {
                for (int i = 0; i < m_aSequenceInfo->Count; i++)
                {
                    if (uiSeqID == m_aSequenceInfo[i]->SequenceID)
                    {

                        pkMapActiveValues->SetAt(uiSeqID, 
                            m_aSequenceInfo[i]->DefaultActiveValue);
                        break;
                    }
                }
            }
        }
    }

    return pkMapActiveValues;
}
//---------------------------------------------------------------------------
void MSequenceGroup::FillSequenceInfoArray()
{
    FillSequenceInfoArray(NULL);
}
//---------------------------------------------------------------------------
void MSequenceGroup::FillSequenceInfoArray(
    NiTMap<unsigned int, bool>* pkActiveMap)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    NIASSERT(m_pkAnimation);
    NIASSERT(m_pkGroup);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    unsigned int uiCount = m_pkGroup->GetSequenceInfo().GetSize();

    ClearInfo();

    if (uiCount > 0)
    {
        m_aSequenceInfo = NiExternalNew MSequenceInfo*[uiCount];
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            NiKFMTool::SequenceGroup::SequenceInfo* pkSI =
                &m_pkGroup->GetSequenceInfo().GetAt(ui);

            m_aSequenceInfo[ui] = NiExternalNew MSequenceInfo(m_pkAnimation,
                this, pkSI);

            if (pkActiveMap)
            {
                bool bValue = true;
                pkActiveMap->GetAt( m_aSequenceInfo[ui]->SequenceID,
                    bValue);

                m_aSequenceInfo[ui]->DefaultActiveValue = bValue;
            }
        }
    }

    pkData->Unlock();
}
//---------------------------------------------------------------------------
bool MSequenceGroup::AddSequence(unsigned int uiSequenceID)
{
    return AddSequence(uiSequenceID, 
        m_pkAnimation->DefaultSeqGroupPriority,
        m_pkAnimation->DefaultSeqGroupWeight, 
        m_pkAnimation->DefaultSeqGroupEaseIn,
        m_pkAnimation->DefaultSeqGroupEaseOut);
}
//---------------------------------------------------------------------------
bool MSequenceGroup::AddSequence(unsigned int uiSequenceID, int iPriority, 
    float fWeight, float fEaseIn, float fEaseOut)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    NIASSERT(m_pkGroup);
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    int iExistingIndex = -1;
    for (unsigned int ui = 0; ui < m_pkGroup->GetSequenceInfo().GetSize();
        ui++)
    {
        NiKFMTool::SequenceGroup::SequenceInfo& kInfo = 
            m_pkGroup->GetSequenceInfo().GetAt(ui);
        if (kInfo.GetSequenceID() == uiSequenceID)
        {
            iExistingIndex = ui;
            break;
        }
    }

    if (iExistingIndex == -1)
    {
        NiTMap<unsigned int, bool>* pkMapDefaultActiveValues = 
            CreateDefaultActiveValuesArray();

        NiKFMTool::SequenceGroup::SequenceInfo kNewInfo(uiSequenceID, 
            iPriority, fWeight, fEaseIn, fEaseOut);
        int iNewIndex = m_pkGroup->GetSequenceInfo().GetSize();
        m_pkGroup->GetSequenceInfo().Add(kNewInfo);
        FillSequenceInfoArray(pkMapDefaultActiveValues);     
        NIASSERT(m_pkAnimation);
        NiDelete pkMapDefaultActiveValues;

        iNewIndex = GetSequenceInfoIndex(uiSequenceID);
        GetAt(iNewIndex);
        pkData->Unlock();
        m_pkAnimation->ThrowSequenceGroupModifiedEvent(
            NiExternalNew MSequenceGroupChangedSequenceInfoEventArgs(
            true, uiSequenceID,fEaseIn, fEaseOut), this);

       return true;
    }
    else
    {
        pkData->Unlock();
        return true;
    }

    
}
//---------------------------------------------------------------------------
bool MSequenceGroup::RemoveSequence(unsigned int uiSequenceID)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    NIASSERT(m_pkGroup);
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    int iExistingIndex = -1;
    MSequenceGroup::MSequenceInfo* pkInfo;
    bool bReturn = false;
    for (unsigned int ui = 0; ui < m_pkGroup->GetSequenceInfo().GetSize();
        ui++)
    {
        NiKFMTool::SequenceGroup::SequenceInfo& kInfo = 
            m_pkGroup->GetSequenceInfo().GetAt(ui);
        if (kInfo.GetSequenceID() == uiSequenceID)
        {
            iExistingIndex = ui;
            pkInfo = GetAt(ui);
            break;
        }
    }

    if (iExistingIndex != -1)
    {
        int iManagedIndex = GetSequenceInfoIndex(uiSequenceID);
        MSequenceInfo* pkInfo = GetAt(iManagedIndex);
        float fEaseIn = pkInfo->EaseInTime;
        float fEaseOut = pkInfo->EaseOutTime;
        
        NiTMap<unsigned int, bool>* pkMapDefaultActiveValues = 
            CreateDefaultActiveValuesArray();

        // Removing the NiKFMTool::SequenceGroup::SequenceInfo from the 
        // SequenceGroup will invalidate the pointer to the SequenceInfo 
        m_pkGroup->GetSequenceInfo().RemoveAt(iExistingIndex);
        pkInfo = NULL;
        FillSequenceInfoArray(pkMapDefaultActiveValues);
        NIASSERT(m_pkAnimation);

        NiDelete pkMapDefaultActiveValues;

        m_pkAnimation->ThrowSequenceGroupModifiedEvent(
            NiExternalNew MSequenceGroupChangedSequenceInfoEventArgs(false, 
            uiSequenceID, fEaseIn, fEaseOut),
            this);

        bReturn = true;
    }

    pkData->Unlock();
    return bReturn;
}

//---------------------------------------------------------------------------
int MSequenceGroup::GetSequenceInfoIndex(unsigned int uiSequenceID)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    if (m_aSequenceInfo == NULL)
        return -1;

    for (int i = 0; i < m_aSequenceInfo->Count; i++)
    {
        if (GetAt(i)->SequenceID == uiSequenceID)
            return i;
    }

    return -1;
}
//---------------------------------------------------------------------------
void MSequenceGroup::ClearInfo()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    if (m_aSequenceInfo != NULL)
    {
        for (int i = 0; i < m_aSequenceInfo->Count; i++)
        {
            GetAt(i)->DeleteContents();
        }

        m_aSequenceInfo = NULL;
    }
}
//---------------------------------------------------------------------------
void MSequenceGroup::DeleteContents()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    ClearInfo();
    m_bDisposed = true;
}
//---------------------------------------------------------------------------
unsigned int MSequenceGroup::get_GroupID()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    unsigned int uiGroupID = m_pkGroup->GetGroupID();
    pkData->Unlock();

    return uiGroupID;
}
//---------------------------------------------------------------------------
void MSequenceGroup::set_GroupID(unsigned int uiGroupID)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    unsigned int uiOldID = m_pkGroup->GetGroupID();
    pkData->Unlock();

    NIASSERT(m_pkAnimation);
    if (m_pkAnimation->ChangeGroupID(uiOldID, uiGroupID))
    {
        m_pkAnimation->ThrowSequenceGroupModifiedEvent(
            NiExternalNew MSequenceGroupChangedIDEventArgs(uiOldID, uiGroupID),
            this);
    }
}
//---------------------------------------------------------------------------
String* MSequenceGroup::get_Name()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    String* strName = m_pkGroup->GetName();
    pkData->Unlock();

    return strName;
}
//---------------------------------------------------------------------------
void MSequenceGroup::set_Name(String* strName)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    String* strOldName = Name;
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    const char* pcName = MStringToCharPointer(strName);
    m_pkGroup->SetName(pcName);
    MFreeCharPointer(pcName);
    pkData->Unlock();

    NIASSERT(m_pkAnimation);
    m_pkAnimation->ThrowSequenceGroupModifiedEvent(
        NiExternalNew MSequenceGroupChangedNameEventArgs(strOldName, Name), 
        this);
}
//---------------------------------------------------------------------------
MSequenceGroup::MSequenceInfo* MSequenceGroup::get_SequenceInfo()[]
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    return m_aSequenceInfo;
}
//---------------------------------------------------------------------------
String* MSequenceGroup::ToString()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    return Name;
}
//---------------------------------------------------------------------------
MSequenceGroup::MSequenceInfo* MSequenceGroup::GetAt(int i)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    if (m_aSequenceInfo != NULL)
        return dynamic_cast<MSequenceInfo*>(m_aSequenceInfo->get_Item(i));

    return NULL;
}
//---------------------------------------------------------------------------
void MSequenceGroup::ActivateAll()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    if (m_aSequenceInfo != NULL)
    {
        for (int i = 0; i < m_aSequenceInfo->Count; i++)
        {
            if (!GetAt(i)->Activated)
                GetAt(i)->Activate();
        }
    }
    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MSequenceGroup::DeactivateAll()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    if (m_aSequenceInfo != NULL)
    {
        for (int i = 0; i < m_aSequenceInfo->Count; i++)
        {
            if (GetAt(i)->Activated)
                GetAt(i)->Deactivate();
        }
    }
    pkData->Unlock();
}
//---------------------------------------------------------------------------
MSequenceGroupChangedSequenceInfoEventArgs::
MSequenceGroupChangedSequenceInfoEventArgs(bool bAdded, 
    unsigned int uiSequenceID, float fEaseIn, float fEaseOut) : 
    MSequenceGroupEventArgs(MSequenceGroup::Prop_SequenceInfo)
{
    Added = bAdded;
    EaseIn = fEaseIn;
    EaseOut = fEaseOut;
    SequenceID = uiSequenceID;
}
//---------------------------------------------------------------------------
MSequenceGroupChangedIDEventArgs::
MSequenceGroupChangedIDEventArgs(unsigned int uiOldID,
    unsigned int uiNewID) : 
    MSequenceGroupEventArgs(MSequenceGroup::Prop_GroupID)
{
    OldID = uiOldID;
    NewID = uiNewID;
}
//---------------------------------------------------------------------------
MSequenceGroupChangedNameEventArgs::
MSequenceGroupChangedNameEventArgs(String* strOldName,
    String* strNewName) : 
    MSequenceGroupEventArgs(MSequenceGroup::Prop_Name)
{
    OldName = strOldName;
    NewName = strNewName;
}
//---------------------------------------------------------------------------
MSequenceGroupEventArgs::MSequenceGroupEventArgs(
    MSequenceGroup::PropertyType eType) 
{
    Type = eType;
}
//---------------------------------------------------------------------------
