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

#include "NiAnimationConstants.h"
#include "NiKFMTool.h"
#include <NiStream.h>
#include <NiMath.h>
#include <NiBool.h>
#include <NiPath.h>
#include <NiVersion.h>

#define MAX_NUM_CHAIN_IDS 1000

const char* g_pcCurrentKFMVersion = GAMEBRYO_NIF_VERSION_STRING;
const float NiKFMTool::MAX_DURATION = -1.0f;
const unsigned int NiKFMTool::SYNC_SEQUENCE_ID_NONE = (unsigned int ) -2;

void CorrectPathSlash(NiFixedString& kFixedString)
{   
    const char* pcString = kFixedString;

    const char* pcLast = pcString;       
    if (pcLast[0] == NI_PATH_DELIMITER_CHAR)
    {
        // Find the last consecutive slash
        for ( ; ; )
        {
            if (pcLast[1] == NI_PATH_DELIMITER_CHAR)
                pcLast++;
            break;
        }
        
        bool bNetworkPath = false;
        if (pcLast - pcString == 1)
        {
            // There are two slashes, so this is possibly a network path.
            // If another slash occurs, it'll be assumed that this is a 
            // network path. If no other slash occurs, it'll be assumed that
            // this was a malformed path name.
            if (pcLast != strrchr(pcLast, NI_PATH_DELIMITER_CHAR))
            {
                bNetworkPath = true;
            }
        }

        if (bNetworkPath)
            pcLast = &pcString[0];
        else 
            pcLast++;
    }

    NiFixedString kReleaseAtEndOfScope = kFixedString;
    kFixedString = pcLast;
}

//---------------------------------------------------------------------------
// Functions for adding components.
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::AddSequence(unsigned int uiSequenceID,
    const NiFixedString& kFilename, const NiFixedString& kSequenceName)
{
    // Create sequence.
    Sequence* pkSequence = NiNew Sequence();
    pkSequence->SetSequenceID(uiSequenceID);
    pkSequence->SetFilename(kFilename);
    pkSequence->SetSequenceName(kSequenceName);

    // Add sequence to map.
    Sequence* pkOldSequence;
    if (m_mapSequences.GetAt(uiSequenceID, pkOldSequence))
    {
        NiDelete pkOldSequence;
    }
    m_mapSequences.SetAt(uiSequenceID, pkSequence);

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::AddSequence(unsigned int uiSequenceID,
    const NiFixedString& kFilename, int iAnimIndex)
{
    // This function is deprecated.

    // Create sequence.
    Sequence* pkSequence = NiNew Sequence();
    pkSequence->SetSequenceID(uiSequenceID);
    pkSequence->SetFilename(kFilename);
    pkSequence->SetAnimIndex(iAnimIndex);

    // Add sequence to map.
    Sequence* pkOldSequence;
    if (m_mapSequences.GetAt(uiSequenceID, pkOldSequence))
    {
        NiDelete pkOldSequence;
    }
    m_mapSequences.SetAt(uiSequenceID, pkSequence);

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::AddTransition(unsigned int uiSrcID,
    unsigned int uiDesID, TransitionType eType, float fDuration)
{
    // Get specified sequence.
    Sequence* pkSequence = GetSequenceFromID(uiSrcID);
    if (!pkSequence)
    {
        return KFM_ERR_SEQUENCE;
    }

    // Create transition.
    Transition* pkTransition;
    if (eType == TYPE_DEFAULT_SYNC)
    {
        pkTransition = m_pkDefaultSyncTrans;
    }
    else if (eType == TYPE_DEFAULT_NONSYNC)
    {
        pkTransition = m_pkDefaultNonSyncTrans;
    }
    else
    {
        pkTransition = NiNew Transition(eType, fDuration);
    }
    

    // Add transition to sequence.
    Transition* pkOldTransition;
    if (pkSequence->GetTransitions().GetAt(uiDesID, pkOldTransition))
    {
        if (pkOldTransition != m_pkDefaultSyncTrans &&
            pkOldTransition != m_pkDefaultNonSyncTrans)
        {
            NiDelete pkOldTransition;
        }
    }
    pkSequence->GetTransitions().SetAt(uiDesID, pkTransition);

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::AddBlendPair(unsigned int uiSrcID,
    unsigned int uiDesID, const NiFixedString& kStartKey, 
    const NiFixedString& kTargetKey)
{
    // Get specified sequence.
    Sequence* pkSequence = GetSequenceFromID(uiSrcID);
    if (!pkSequence)
    {
        return KFM_ERR_SEQUENCE;
    }

    // Get specified transition.
    Transition* pkTransition = GetTransitionFromID(uiDesID, pkSequence);
    if (!pkTransition)
    {
        return KFM_ERR_TRANSITION;
    }
    else if (pkTransition->GetType() != TYPE_BLEND)
    {
        return KFM_ERR_TRANSITION_TYPE;
    }

    // Check for null pointers.
    if (!kStartKey.Exists() && !kTargetKey.Exists())
    {
        return KFM_ERR_NULL_TEXT_KEYS;
    }

    // Add blend pair.
    Transition::BlendPair* pkPair = NiNew Transition::BlendPair(kStartKey,
        kTargetKey);
    pkTransition->GetBlendPairs().Add(pkPair);

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::AddSequenceToChain(unsigned int uiSrcID,
    unsigned int uiDesID, unsigned int uiSequenceID, float fDuration)
{
    // Get specified sequence.
    Sequence* pkSequence = GetSequenceFromID(uiSrcID);
    if (!pkSequence)
    {
        return KFM_ERR_SEQUENCE;
    }

    // Get specified transition.
    Transition* pkTransition = GetTransitionFromID(uiDesID, pkSequence);
    if (!pkTransition)
    {
        return KFM_ERR_TRANSITION;
    }
    else if (pkTransition->GetType() != TYPE_CHAIN)
    {
        return KFM_ERR_TRANSITION_TYPE;
    }

    // Add chain info.
    Transition::ChainInfo kInfo(uiSequenceID, fDuration);
    pkTransition->GetChainInfo().Add(kInfo);

    if (!IsValidChainTransition(uiSrcID, uiDesID, pkTransition))
    {
        pkTransition->GetChainInfo().RemoveAt(
            pkTransition->GetChainInfo().GetSize());
        return KFM_ERR_INFINITE_CHAIN;
    }

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::AddSequenceGroup(unsigned int uiGroupID,
    const NiFixedString& kName)
{
    // Create Sequence group.
    SequenceGroup* pkGroup = NiNew SequenceGroup(uiGroupID, kName);

    // Add Sequence group to map.
    SequenceGroup* pkOldGroup;
    if (m_mapSequenceGroups.GetAt(uiGroupID, pkOldGroup))
    {
        NiDelete pkOldGroup;
    }
    m_mapSequenceGroups.SetAt(uiGroupID, pkGroup);

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::AddSequenceToGroup(unsigned int uiGroupID,
    unsigned int uiSequenceID, int iPriority, float fWeight,
    float fEaseInTime, float fEaseOutTime, 
    unsigned int uiSynchronizeToSequence)
{
    // Get specified group.
    SequenceGroup* pkGroup = GetSequenceGroupFromID(uiGroupID);
    if (!pkGroup)
    {
        return KFM_ERR_SEQUENCE_GROUP;
    }

    SequenceGroup::SequenceInfo kInfo(uiSequenceID, iPriority, fWeight,
        fEaseInTime, fEaseOutTime, uiSynchronizeToSequence);
    pkGroup->GetSequenceInfo().Add(kInfo);

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
// Functions for updating components.
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::UpdateSequence(unsigned int uiSequenceID,
    const NiFixedString& kFilename, const NiFixedString& kSequenceName)
{
    // Get specified sequence.
    Sequence* pkSequence = GetSequenceFromID(uiSequenceID);
    if (!pkSequence)
    {
        return KFM_ERR_SEQUENCE;
    }

    // Update data.
    pkSequence->SetFilename(kFilename);
    pkSequence->SetSequenceName(kSequenceName);

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::UpdateTransition(unsigned int uiSrcID,
    unsigned int uiDesID, TransitionType eType, float fDuration)
{
    // Get specified sequence.
    Sequence* pkSequence = GetSequenceFromID(uiSrcID);
    if (!pkSequence)
    {
        return KFM_ERR_SEQUENCE;
    }

    // Get specified transition.
    Transition* pkTransition = GetTransitionFromID(uiDesID, pkSequence);
    if (!pkTransition)
    {
        return KFM_ERR_TRANSITION;
    }

    // Update data.
    if (pkTransition->GetStoredType() != eType)
    {
        if (eType == TYPE_DEFAULT_SYNC)
        {
            if (pkTransition != m_pkDefaultSyncTrans &&
                pkTransition != m_pkDefaultNonSyncTrans)
            {
                NiDelete pkTransition;
            }
            pkTransition = m_pkDefaultSyncTrans;
            pkSequence->GetTransitions().SetAt(uiDesID, pkTransition);
        }
        else if (eType == TYPE_DEFAULT_NONSYNC)
        {
            if (pkTransition != m_pkDefaultSyncTrans &&
                pkTransition != m_pkDefaultNonSyncTrans)
            {
                NiDelete pkTransition;
            }
            pkTransition = m_pkDefaultNonSyncTrans;
            pkSequence->GetTransitions().SetAt(uiDesID, pkTransition);
        }
        else
        {
            if (pkTransition == m_pkDefaultSyncTrans ||
                pkTransition == m_pkDefaultNonSyncTrans)
            {
                pkTransition = 
                    NiNew Transition(eType, pkTransition->GetDuration());

                pkSequence->GetTransitions().SetAt(uiDesID, pkTransition);
            }
            else
            {
                pkTransition->m_eType = eType;
            }
        }
    }

    // Duration set only if not going to a sync or nonsync
    if (eType != TYPE_DEFAULT_SYNC && eType != TYPE_DEFAULT_NONSYNC)
    {
        pkTransition->SetDuration(fDuration);
    }

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::UpdateSequenceID(unsigned int uiOldID,
    unsigned int uiNewID)
{
    // Get specified sequence.
    Sequence* pkSequence = GetSequenceFromID(uiOldID);
    if (!pkSequence)
    {
        return KFM_ERR_SEQUENCE;
    }

    // Remove sequence and add it again with new event code.
    m_mapSequences.RemoveAt(uiOldID);
    pkSequence->SetSequenceID(uiNewID);
    m_mapSequences.SetAt(uiNewID, pkSequence);

     // Update all transitions that contain the sequence.
    UpdateTransitionsContainingSequence(uiOldID, uiNewID);

    // Update all sequence groups that contain the sequence.
    UpdateSequenceGroupsContainingSequence(uiOldID, uiNewID);

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
void NiKFMTool::UpdateTransitionsContainingSequence(unsigned int uiOldID,
    unsigned int uiNewID)
{
    unsigned int uiSrcID;
    Sequence* pkSequence;
    NiTMapIterator spos = m_mapSequences.GetFirstPos();
    while (spos)
    {
        m_mapSequences.GetNext(spos, uiSrcID, pkSequence);

        unsigned int uiDesID;
        Transition* pkTransition;
        NiTMapIterator tpos = pkSequence->GetTransitions().GetFirstPos();
        while (tpos)
        {
            pkSequence->GetTransitions().GetNext(tpos, uiDesID,
                pkTransition);

            if (uiDesID == uiOldID)
            {
                pkSequence->GetTransitions().RemoveAt(uiOldID);
                pkSequence->GetTransitions().SetAt(uiNewID, pkTransition);
            }

            for (unsigned int ui=0;
                ui < pkTransition->GetChainInfo().GetSize(); ui++)
            {
                Transition::ChainInfo& kChainInfo =
                    pkTransition->GetChainInfo().GetAt(ui);
                
                if (kChainInfo.GetSequenceID() == uiOldID)
                {
                    kChainInfo.SetSequenceID(uiNewID);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiKFMTool::UpdateSequenceGroupsContainingSequence(unsigned int uiOldID,
    unsigned int uiNewID)
{
    unsigned int uiGroupID;
    SequenceGroup* pkGroup;
    NiTMapIterator pos = m_mapSequenceGroups.GetFirstPos();
    while (pos)
    {
        m_mapSequenceGroups.GetNext(pos, uiGroupID, pkGroup);

        for (unsigned int ui = 0; ui < pkGroup->GetSequenceInfo().GetSize();
            ui++)
        {
            if (pkGroup->GetSequenceInfo().GetAt(ui).GetSequenceID() ==
                uiOldID)
            {
                pkGroup->GetSequenceInfo().GetAt(ui).SetSequenceID(uiNewID);
            }

            if (pkGroup->GetSequenceInfo().GetAt(ui)
                .GetSynchronizeSequenceID() == uiOldID)
            {
                pkGroup->GetSequenceInfo().GetAt(ui).SetSynchronizeSequenceID(
                    uiNewID);
            }
        }
    }
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::UpdateGroupID(unsigned int uiOldID,
    unsigned int uiNewID)
{
    // Get specified layer group.
    SequenceGroup* pkGroup = GetSequenceGroupFromID(uiOldID);
    if (!pkGroup)
    {
        return KFM_ERR_SEQUENCE_GROUP;
    }

    // Remove sequence group and add it again with the new ID.
    m_mapSequenceGroups.RemoveAt(uiOldID);
    pkGroup->SetGroupID(uiNewID);
    m_mapSequenceGroups.SetAt(uiNewID, pkGroup);

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
// Functions for removing components.
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::RemoveSequence(unsigned int uiSequenceID)
{
    // Get specified sequence.
    Sequence* pkSequence = GetSequenceFromID(uiSequenceID);
    if (!pkSequence)
    {
        return KFM_ERR_SEQUENCE;
    }

    // Remove sequence and delete object.
    m_mapSequences.RemoveAt(uiSequenceID);
    NiDelete pkSequence;

    // Remove all transitions that contain the deleted sequence.
    RemoveTransitionsContainingSequence(uiSequenceID);

    // Remove the deleted sequence from all sequence groups.
    RemoveSequenceFromSequenceGroups(uiSequenceID);

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
void NiKFMTool::RemoveTransitionsContainingSequence(
    unsigned int uiSequenceID)
{
    unsigned int uiSrcID;
    Sequence* pkSequence;
    NiTMapIterator spos = m_mapSequences.GetFirstPos();
    while (spos)
    {
        m_mapSequences.GetNext(spos, uiSrcID, pkSequence);

        unsigned int uiDesID;
        Transition* pkTransition;
        NiTMapIterator tpos = pkSequence->GetTransitions().GetFirstPos();
        while (tpos)
        {
            pkSequence->GetTransitions().GetNext(tpos, uiDesID,
                pkTransition);

            if (uiSrcID == uiSequenceID || uiDesID == uiSequenceID)
            {
                pkSequence->GetTransitions().RemoveAt(uiDesID);
                if (pkTransition != m_pkDefaultSyncTrans &&
                    pkTransition != m_pkDefaultNonSyncTrans)
                {
                    NiDelete pkTransition;
                }
            }
            else if (pkTransition->GetType() == TYPE_CHAIN)
            {
                for (unsigned int ui = 0;
                    ui < pkTransition->GetChainInfo().GetSize(); ui++)
                {
                    if (pkTransition->GetChainInfo().GetAt(ui).GetSequenceID()
                        == uiSequenceID)
                    {
                        pkSequence->GetTransitions().RemoveAt(uiDesID);
                        if (pkTransition != m_pkDefaultSyncTrans &&
                            pkTransition != m_pkDefaultNonSyncTrans)
                        {
                            NiDelete pkTransition;
                        }
                        break;
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiKFMTool::RemoveSequenceFromSequenceGroups(unsigned int uiSequenceID)
{
    unsigned int uiGroupID;
    SequenceGroup* pkGroup;
    NiTMapIterator pos = m_mapSequenceGroups.GetFirstPos();
    while (pos)
    {
        m_mapSequenceGroups.GetNext(pos, uiGroupID, pkGroup);

        for (unsigned int ui = 0; ui < pkGroup->GetSequenceInfo().GetSize();
            ui++)
        {
            if (pkGroup->GetSequenceInfo().GetAt(ui).GetSequenceID() ==
                uiSequenceID)
            {
                pkGroup->GetSequenceInfo().RemoveAt(ui);
                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::RemoveTransition(unsigned int uiSrcID,
    unsigned int uiDesID)
{
    // Get specified sequence.
    Sequence* pkSequence = GetSequenceFromID(uiSrcID);
    if (!pkSequence)
    {
        return KFM_ERR_SEQUENCE;
    }

    // Get specified transition.
    Transition* pkTransition = GetTransitionFromID(uiDesID, pkSequence);
    if (!pkTransition)
    {
        return KFM_ERR_TRANSITION;
    }

    // Remove transition and delete object.
    pkSequence->GetTransitions().RemoveAt(uiDesID);
    if (pkTransition != m_pkDefaultSyncTrans &&
        pkTransition != m_pkDefaultNonSyncTrans)
    {
        NiDelete pkTransition;
    }

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::RemoveBlendPair(unsigned int uiSrcID,
    unsigned int uiDesID,const NiFixedString& kStartKey, 
    const NiFixedString& kTargetKey)
{
    // Get specified sequence.
    Sequence* pkSequence = GetSequenceFromID(uiSrcID);
    if (!pkSequence)
    {
        return KFM_ERR_SEQUENCE;
    }

    // Get specified transition.
    Transition* pkTransition = GetTransitionFromID(uiDesID, pkSequence);
    if (!pkTransition)
    {
        return KFM_ERR_TRANSITION;
    }

    // Check for null pointers.
    if (!kStartKey.Exists() && !kTargetKey.Exists())
    {
        return KFM_ERR_NULL_TEXT_KEYS;
    }

    bool bFound = false;
    for (unsigned int ui = 0; ui < pkTransition->GetBlendPairs().GetSize();
        ui++)
    {
        Transition::BlendPair* pkPair = pkTransition->GetBlendPairs().GetAt(
            ui);
        NIASSERT(pkPair);

        bool bStartSame = false;
        if (kStartKey == pkPair->GetStartKey())
        {
            bStartSame = true;
        }

        bool bTargetSame = false;
        if (kTargetKey == pkPair->GetTargetKey())
        {
            bTargetSame = true;
        }

        if (bStartSame && bTargetSame)
        {
            pkTransition->GetBlendPairs().RemoveAt(ui);
            NiDelete pkPair;
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        return KFM_ERR_BLEND_PAIR;
    }

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::RemoveAllBlendPairs(unsigned int uiSrcID,
    unsigned int uiDesID)
{
    // Get specified sequence.
    Sequence* pkSequence = GetSequenceFromID(uiSrcID);
    if (!pkSequence)
    {
        return KFM_ERR_SEQUENCE;
    }

    // Get specified transition.
    Transition* pkTransition = GetTransitionFromID(uiDesID, pkSequence);
    if (!pkTransition)
    {
        return KFM_ERR_TRANSITION;
    }

    pkTransition->ClearBlendPairs();

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::RemoveSequenceFromChain(unsigned int uiSrcID,
    unsigned int uiDesID, unsigned int uiSequenceID)
{
    // Get specified sequence.
    Sequence* pkSequence = GetSequenceFromID(uiSrcID);
    if (!pkSequence)
    {
        return KFM_ERR_SEQUENCE;
    }

    // Get specified transition.
    Transition* pkTransition = GetTransitionFromID(uiDesID, pkSequence);
    if (!pkTransition)
    {
        return KFM_ERR_TRANSITION;
    }

    bool bFound = false;
    for (unsigned int ui = 0; ui < pkTransition->GetChainInfo().GetSize();
        ui++)
    {
        unsigned int uiID = pkTransition->GetChainInfo().GetAt(ui)
            .GetSequenceID();

        if (uiID == uiSequenceID)
        {
            pkTransition->GetChainInfo().OrderedRemoveAt(ui);
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        return KFM_ERR_CHAIN_SEQUENCE;
    }

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::RemoveAllSequencesFromChain(unsigned int uiSrcID,
    unsigned int uiDesID)
{
    // Get specified sequence.
    Sequence* pkSequence = GetSequenceFromID(uiSrcID);
    if (!pkSequence)
    {
        return KFM_ERR_SEQUENCE;
    }

    // Get specified transition.
    Transition* pkTransition = GetTransitionFromID(uiDesID, pkSequence);
    if (!pkTransition)
    {
        return KFM_ERR_TRANSITION;
    }

    pkTransition->ClearChainInfo();

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::RemoveSequenceGroup(unsigned int uiGroupID)
{
    // Get specified sequence group.
    SequenceGroup* pkGroup = GetSequenceGroupFromID(uiGroupID);
    if (!pkGroup)
    {
        return KFM_ERR_SEQUENCE_GROUP;
    }

    // Remove sequence group and delete object.
    m_mapSequenceGroups.RemoveAt(uiGroupID);
    NiDelete pkGroup;

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::RemoveSequenceFromGroup(unsigned int uiGroupID,
    unsigned int uiSequenceID)
{
    // Get specified sequence group.
    SequenceGroup* pkGroup = GetSequenceGroupFromID(uiGroupID);
    if (!pkGroup)
    {
        return KFM_ERR_SEQUENCE_GROUP;
    }

    // Remove sequence from group.
    for (unsigned int ui = 0; ui < pkGroup->GetSequenceInfo().GetSize(); ui++)
    {
        if (pkGroup->GetSequenceInfo().GetAt(ui).GetSequenceID() ==
            uiSequenceID)
        {
            pkGroup->GetSequenceInfo().RemoveAt(ui);
            break;
        }
    }

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::RemoveAllSequencesFromGroup(
    unsigned int uiGroupID)
{
    // Get specified sequence group.
    SequenceGroup* pkGroup = GetSequenceGroupFromID(uiGroupID);
    if (!pkGroup)
    {
        return KFM_ERR_SEQUENCE_GROUP;
    }

    // Remove all sequences from group.
    pkGroup->GetSequenceInfo().RemoveAll();

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
// Functions for retrieving identifier codes.
//---------------------------------------------------------------------------
void NiKFMTool::GetSequenceIDs(unsigned int*& puiSequenceIDs,
    unsigned int& uiNumIDs) const
{
    // Get number of IDs.
    uiNumIDs = m_mapSequences.GetCount();

    if (uiNumIDs == 0)
    {
        puiSequenceIDs = NULL;
        return;
    }

    // Retrieve sequence IDs.
    puiSequenceIDs = NiAlloc(unsigned int, uiNumIDs);
    unsigned int ui = 0;
    NiTMapIterator pos = m_mapSequences.GetFirstPos();
    while (pos)
    {
        unsigned int uiID;
        Sequence* pkSequence;
        m_mapSequences.GetNext(pos, uiID, pkSequence);
        puiSequenceIDs[ui++] = uiID;
    }
}
//---------------------------------------------------------------------------
void NiKFMTool::GetGroupIDs(unsigned int*& puiGroupIDs,
    unsigned int& uiNumIDs) const
{
    // Get number of codes.
    uiNumIDs = m_mapSequenceGroups.GetCount();

    if (uiNumIDs == 0)
    {
        puiGroupIDs = NULL;
        return;
    }

    // Retrieve group IDs.
    puiGroupIDs = NiAlloc(unsigned int, uiNumIDs);
    unsigned int ui = 0;
    NiTMapIterator pos = m_mapSequenceGroups.GetFirstPos();
    while (pos)
    {
        unsigned int uiGroupID;
        SequenceGroup* pkGroup;
        m_mapSequenceGroups.GetNext(pos, uiGroupID, pkGroup);
        puiGroupIDs[ui++] = uiGroupID;
    }
}
//---------------------------------------------------------------------------
unsigned int NiKFMTool::FindUnusedSequenceID() const
{
    if (m_mapSequences.GetCount() == 0)
    {
        return 0;
    }

    unsigned int uiTryNumber = 0;
    bool bKeepGoing = true;
    while (bKeepGoing && uiTryNumber < 60000)
    {
        bool bFoundMatch = false;
        NiTMapIterator pos = m_mapSequences.GetFirstPos();
        while (pos != NULL && !bFoundMatch)
        {
            unsigned int uiID;
            Sequence* pkSeq;
            m_mapSequences.GetNext(pos, uiID, pkSeq);
            if (uiID == uiTryNumber)
                bFoundMatch = true;
        }

        if (!bFoundMatch)
            return uiTryNumber;
        else
            uiTryNumber++;
    }

    return (unsigned int) -1;
}
//---------------------------------------------------------------------------
unsigned int NiKFMTool::FindUnusedGroupID() const
{
    if (m_mapSequenceGroups.GetCount() == 0)
    {
        return 0;
    }

    unsigned int uiTryNumber = 0;
    bool bKeepGoing = true;
    while (bKeepGoing && uiTryNumber < 60000)
    {
        bool bFoundMatch = false;
        NiTMapIterator pos = m_mapSequenceGroups.GetFirstPos();
        while (pos != NULL && !bFoundMatch)
        {
            unsigned int uiGroupID;
            SequenceGroup* pkGroup;
            m_mapSequenceGroups.GetNext(pos, uiGroupID, pkGroup);
            if (uiGroupID == uiTryNumber)
                bFoundMatch = true;
        }

        if (!bFoundMatch)
            return uiTryNumber;
        else
            uiTryNumber++;
    }

    return (unsigned int) -1;
}
//---------------------------------------------------------------------------
bool NiKFMTool::IsValidChainTransition(unsigned int uiSrcID,
    unsigned int uiDesID, Transition* pkTransition)
{
    NiUnsignedIntSet kChainIDs;
    GatherChainIDs(uiSrcID, uiDesID, pkTransition, kChainIDs);
    if (kChainIDs.GetSize() > MAX_NUM_CHAIN_IDS)
    {
        // No chain should have MAX_NUM_CHAIN_IDS entries.
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Functions for getting fully qualified paths.
//---------------------------------------------------------------------------
const NiFixedString& NiKFMTool::GetFullModelPath()
{
    if (m_kModelPath.Exists() && NiPath::IsRelative(m_kModelPath))
    {
        // Construct and return an absolute path.
        char acFullPathBuffer[NI_MAX_PATH];
        NIVERIFY(NiPath::ConvertToAbsolute(acFullPathBuffer, 
            NI_MAX_PATH, m_kModelPath, m_kBaseKFMPath) > 0);

        m_kFullPathBuffer = NiFixedString(acFullPathBuffer);
        return m_kFullPathBuffer;
    }
    else
    {
        // m_acModelPath is null or already an absolute path; just return it.
        return m_kModelPath;
    }
}
//---------------------------------------------------------------------------
NiFixedString NiKFMTool::GetFullKFFilename(unsigned int uiSequenceID)
{
    Sequence* pkSequence = GetSequenceFromID(uiSequenceID);
    if (!pkSequence)
    {
        return NULL;
    }

    if (pkSequence->GetFilename())
    {
        if (!NiPath::IsRelative(pkSequence->GetFilename()))
        {
            // The filename is already an absolute path; just return it.
            return pkSequence->GetFilename();
        }
        else 
        {
            // Construct and return an absolute path.
            char acFullPathBuffer[NI_MAX_PATH];
            NIVERIFY(NiPath::ConvertToAbsolute(acFullPathBuffer, 
                NI_MAX_PATH, pkSequence->GetFilename(), m_kBaseKFMPath) > 0);

            // Construct and return an absolute path.
            return NiFixedString(acFullPathBuffer);
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
// Internal protected functions.
//---------------------------------------------------------------------------
void NiKFMTool::GatherChainIDs(unsigned int uiSrcID, unsigned int uiDesID,
    Transition* pkTransition, NiUnsignedIntSet& kChainIDs)
{
    if (pkTransition->GetType() != TYPE_CHAIN)
    {
        return;
    }

    if (kChainIDs.GetSize() > MAX_NUM_CHAIN_IDS)
    {
        return;
    }

    unsigned int uiChainCount = pkTransition->GetChainInfo().GetSize();
    if (uiChainCount == 0)
    {
        return;
    }

    kChainIDs.Add(uiSrcID);
    for (unsigned int ui = 0; ui < uiChainCount; ui++)
    {
        unsigned int uiDestID = pkTransition->GetChainInfo().GetAt(ui)
            .GetSequenceID();
        Transition* pkTempTrans = GetTransition(uiSrcID, uiDestID);
        if (pkTempTrans && pkTempTrans->GetType() == TYPE_CHAIN)
        {
            GatherChainIDs(uiSrcID, uiDestID, pkTempTrans, kChainIDs);
        }
        else
        {
            kChainIDs.Add(uiDestID);
        }
        uiSrcID = uiDestID;
    }
    kChainIDs.Add(uiDesID);
}
//---------------------------------------------------------------------------
void NiKFMTool::ConvertRelativePaths(const char* pcNewBaseKFMPath)
{
    // Reset all relative path for the KFM and all the KFs to be relative to
    // pcNewBaseKFMPath

    char acModelPath[NI_MAX_PATH];
    if (NiPath::ConvertToRelative(acModelPath, NI_MAX_PATH, GetFullModelPath(),
        pcNewBaseKFMPath))
    {
        SetModelPath(acModelPath);
    }
    else
    {
        SetModelPath(GetFullModelPath());
    }

    NiTMapIterator pos = m_mapSequences.GetFirstPos();
    while (pos)
    {
        unsigned int uiSequenceID;
        Sequence* pkSequence;
        m_mapSequences.GetNext(pos, uiSequenceID, pkSequence);

        if (NiPath::ConvertToRelative(acModelPath, NI_MAX_PATH, 
            GetFullKFFilename(uiSequenceID), pcNewBaseKFMPath))
        {
            pkSequence->SetFilename(acModelPath);
        }
        else
        {
            pkSequence->SetFilename(GetFullKFFilename(uiSequenceID));
        }
    }
}
//---------------------------------------------------------------------------
// Functions for streaming data to a file.
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::LoadFile(const char* pcFilename)
{
    char acStdFilename[NI_MAX_PATH];
    NiStrcpy(acStdFilename, NI_MAX_PATH, pcFilename);
    NiPath::Standardize(acStdFilename);

    // Load file.
    NiFile* pkFile = NiFile::GetFile(acStdFilename,  NiFile::READ_ONLY);
    if (!pkFile || !(*pkFile))
    {
        NiDelete pkFile;
        return KFM_ERR_FILE_IO;
    }

    // Set file to read as little endian
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    pkFile->SetEndianSwap(!bPlatformLittle);

    // Read file header in little endian.
    char acBuf[256];
    pkFile->GetLine(acBuf, 256);
    const unsigned int uiVerLength = 16;
    char acVersion[uiVerLength];
    unsigned int uiVersionEnd = 3;
    bool bBinary = false;

    if (strncmp(acBuf, ";Gamebryo KFM File Version ", 27) == 0)
    {
        size_t uiLen = strlen(acBuf);
        bBinary = (acBuf[uiLen-1] == 'b');
        // binary files have a 'b' on the end, so one fewer character
        uiVersionEnd = (unsigned int) (uiLen - (bBinary ? 28 : 27));
        NIASSERT(uiVersionEnd < uiVerLength);
        NiStrncpy(acVersion, uiVerLength, &acBuf[27], uiVersionEnd);
    }
    else
    {
        NiDelete pkFile;
        return KFM_ERR_FILE_VERSION;
    }
    acVersion[uiVersionEnd] = '\0';

    // Check version.
    bool bOldVersion = false;
    unsigned int uiVersion = NiStream::GetVersionFromString(acVersion);
    if (uiVersion < NiStream::GetVersion(1, 2, 0, 0))
    {
        bOldVersion = true;
    }
    else if (uiVersion > NiStream::GetVersionFromString(
        g_pcCurrentKFMVersion))
    {
        NiDelete pkFile;
        return KFM_ERR_FILE_VERSION;
    }

    // Store base KFM path.
    char* pcPtr = strrchr(acStdFilename, NI_PATH_DELIMITER_CHAR);
    if (pcPtr)
    {
        pcPtr++;
        *pcPtr = '\0';
        SetBaseKFMPath(acStdFilename);
    }
    else
    {
        SetBaseKFMPath(NULL);
    }

    KFM_RC eResult;
    if (bOldVersion)
    {
        eResult = ReadOldVersionAscii(*pkFile, uiVersion);
    }
    else if (bBinary)
    {
        // By default, binary files are little endian
        bool bFileLittle = true;
        // Read endianness byte, if streamed
        if (uiVersion >= NiStream::GetVersion(1,2,6,0))
        {
            NiStreamLoadBinary(*pkFile, bFileLittle);

            if (bFileLittle != bPlatformLittle)
            {
                if (NiBinaryStream::GetEndianMatchHint())
                {
                    NiDelete pkFile;
                    return KFM_ERR_ENDIAN_MISMATCH;
                }
                else
                {
                    // Give warning to the user
                    NiOutputDebugString("Warning: ");
                    NiOutputDebugString(pcFilename);
                    NiOutputDebugString(" has to be endian swapped.\n");
                }
            }
        }
        pkFile->SetEndianSwap(bFileLittle != bPlatformLittle);

        eResult = ReadBinary(*pkFile, uiVersion);
    }
    else
    {
        eResult = ReadAscii(*pkFile, uiVersion);
    }

    NiDelete pkFile;
    return eResult;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::SaveFile(const char* pcFilename, bool bUseBinary,
    bool bLittleEndian)
{
    char acStdFilename[NI_MAX_PATH];
    NiStrcpy(acStdFilename, NI_MAX_PATH, pcFilename);
    NiPath::Standardize(acStdFilename);

    // Create file.
    NiFile* pkFile = NiFile::GetFile(acStdFilename,  NiFile::WRITE_ONLY);
    if (!pkFile || !(*pkFile))
    {
        NiDelete pkFile;
        return KFM_ERR_FILE_IO;
    }

    // Set endianness
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    pkFile->SetEndianSwap(bPlatformLittle != bLittleEndian);

    // Write file header.
    char acBuf[255];
    NiSprintf(acBuf, 255, ";Gamebryo KFM File Version %s%s\n",
        g_pcCurrentKFMVersion, bUseBinary ? "b" : "a");
    pkFile->Write(acBuf, (unsigned int)(strlen(acBuf)));

    // Convert relative paths.
    char* pcPtr = strrchr(acStdFilename, NI_PATH_DELIMITER_CHAR);
    if (pcPtr)
    {
        pcPtr++;
        *pcPtr = '\0';

        ConvertRelativePaths(acStdFilename);
        SetBaseKFMPath(acStdFilename);
    }

    KFM_RC eResult;
    if (bUseBinary)
    {
        // Save endianness
        NiBinaryStreamSave(*pkFile, &bLittleEndian);
        eResult = WriteBinary(*pkFile);
    }
    else
    {
        eResult = WriteAscii(*pkFile);
    }

    NiDelete pkFile;
    return eResult;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::WriteBinary(NiFile& kFile)
{
    SaveCString(kFile, m_kModelPath);
    SaveFixedString(kFile, m_kModelRoot);

    // Save default transition settings.
    NiBinaryStreamSaveEnum(kFile, m_pkDefaultSyncTrans->m_eType);
    NiBinaryStreamSaveEnum(kFile, m_pkDefaultNonSyncTrans->m_eType);
    NiStreamSaveBinary(kFile, m_pkDefaultSyncTrans->m_fDuration);
    NiStreamSaveBinary(kFile, m_pkDefaultNonSyncTrans->m_fDuration);

    // Save sequences.
    unsigned int uiNumSequences = m_mapSequences.GetCount();
    NiStreamSaveBinary(kFile, uiNumSequences);
    NiTMapIterator spos = m_mapSequences.GetFirstPos();
    while (spos)
    {
        unsigned int uiSrcID;
        Sequence* pkSequence;
        m_mapSequences.GetNext(spos, uiSrcID, pkSequence);

        NiStreamSaveBinary(kFile, pkSequence->m_uiSequenceID);
        SaveCString(kFile, pkSequence->m_kFilename);
        SaveCString(kFile, pkSequence->m_kSequenceName);

        // Save transitions.
        unsigned int uiNumTransitions =
            pkSequence->m_mapTransitions.GetCount();
        NiStreamSaveBinary(kFile, uiNumTransitions);
        unsigned int uiDesID;
        Transition* pkTransition;
        NiTMapIterator tpos = pkSequence->m_mapTransitions.GetFirstPos();
        while (tpos)
        {
            pkSequence->m_mapTransitions.GetNext(tpos, uiDesID,
                pkTransition);

            NiStreamSaveBinary(kFile, uiDesID);

            TransitionType eStoredType = pkTransition->GetStoredType();
            NiBinaryStreamSaveEnum(kFile, eStoredType);

            if (eStoredType != TYPE_DEFAULT_SYNC &&
                eStoredType != TYPE_DEFAULT_NONSYNC)
            {
                NiStreamSaveBinary(kFile, pkTransition->m_fDuration);

                unsigned int uiNumBlendPairs =
                    pkTransition->m_aBlendPairs.GetSize();
                NiStreamSaveBinary(kFile, uiNumBlendPairs);
                unsigned int ui;
                for (ui = 0; ui < uiNumBlendPairs; ui++)
                {
                    Transition::BlendPair* pkPair =
                        pkTransition->m_aBlendPairs.GetAt(ui);
                    SaveFixedString(kFile, pkPair->m_kStartKey);
                    SaveFixedString(kFile, pkPair->m_kTargetKey);
                }

                unsigned int uiNumChainSequences =
                    pkTransition->m_aChainInfo.GetSize();
                NiStreamSaveBinary(kFile, uiNumChainSequences);
                for (ui = 0; ui < uiNumChainSequences; ui++)
                {
                    Transition::ChainInfo& kInfo =
                        pkTransition->m_aChainInfo.GetAt(ui);
                    NiStreamSaveBinary(kFile, kInfo.m_uiSequenceID);
                    NiStreamSaveBinary(kFile, kInfo.m_fDuration);
                }
            }
        }
    }

    // Save sequence groups.
    unsigned int uiNumSequenceGroups = m_mapSequenceGroups.GetCount();
    NiStreamSaveBinary(kFile, uiNumSequenceGroups);
    NiTMapIterator gpos = m_mapSequenceGroups.GetFirstPos();
    while (gpos)
    {
        unsigned int uiGroupID;
        SequenceGroup* pkGroup;
        m_mapSequenceGroups.GetNext(gpos, uiGroupID, pkGroup);

        NiStreamSaveBinary(kFile, pkGroup->m_uiGroupID);
        SaveFixedString(kFile, pkGroup->m_kName);

        unsigned int uiSequencesCount = pkGroup->m_aSequenceInfo.GetSize();
        NiStreamSaveBinary(kFile, uiSequencesCount);
        for (unsigned int ui = 0; ui < uiSequencesCount; ui++)
        {
            SequenceGroup::SequenceInfo& kInfo =
                pkGroup->m_aSequenceInfo.GetAt(ui);
            NiStreamSaveBinary(kFile, kInfo.m_uiSequenceID);
            NiStreamSaveBinary(kFile, kInfo.m_iPriority);
            NiStreamSaveBinary(kFile, kInfo.m_fWeight);
            NiStreamSaveBinary(kFile, kInfo.m_fEaseInTime);
            NiStreamSaveBinary(kFile, kInfo.m_fEaseOutTime);
            NiStreamSaveBinary(kFile, kInfo.m_uiSynchronizeSequenceID);
        }
    }

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::WriteAscii(NiFile&)
{
    // ASCII KFM files are not supported.
    NIASSERT(false);
    return KFM_ERR_FILE_FORMAT;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::ReadBinary(NiFile& kFile,
    unsigned int uiVersion)
{
    // Note: this function assumes endianness of kFile has already been set.
    char* pcDefaultNIFPath = NULL;
    char* pcDefaultKFPath = NULL;
    if (uiVersion < NiStream::GetVersion(1, 2, 3, 0))
    {
        NiBool bDefaultPaths;
        NiStreamLoadBinary(kFile, bDefaultPaths);
        if (bDefaultPaths != 0)
        {
            LoadCString(kFile, pcDefaultNIFPath);
            LoadCString(kFile, pcDefaultKFPath);
        }
    }

    LoadCStringAsFixedString(kFile, m_kModelPath);   
    NiStandardizeFilePath(m_kModelPath);
    if (uiVersion < NiStream::GetVersion(2, 2, 0, 0))
    {
        CorrectPathSlash(m_kModelPath);
    }

    if (pcDefaultNIFPath)
    {
        // Convert old default paths.
        char acModelPath[NI_MAX_PATH];
        NiStrcpy(acModelPath, NI_MAX_PATH, pcDefaultNIFPath);
        NiStrcat(acModelPath, NI_MAX_PATH, m_kModelPath);
        SetModelPath(acModelPath);
    }

    if (uiVersion < NiStream::GetVersion(2, 1, 0, 0))
    {
        LoadCStringAsFixedString(kFile, m_kModelRoot);
    }
    else
    {
        LoadFixedString(kFile, m_kModelRoot);
    }

    if (uiVersion >= NiStream::GetVersion(1, 2, 2, 0))
    {
        // Load default transition settings.
        NiBinaryStreamLoadEnum(kFile, &m_pkDefaultSyncTrans->m_eType);
        NiBinaryStreamLoadEnum(kFile, &m_pkDefaultNonSyncTrans->m_eType);
        NiStreamLoadBinary(kFile, m_pkDefaultSyncTrans->m_fDuration);
        NiStreamLoadBinary(kFile, m_pkDefaultNonSyncTrans->m_fDuration);
    }

    // Load sequences.
    unsigned int uiNumSequences;
    NiStreamLoadBinary(kFile, uiNumSequences);
    unsigned int ui;
    for (ui = 0; ui < uiNumSequences; ui++)
    {
        Sequence* pkSequence = NiNew Sequence;
        NiStreamLoadBinary(kFile, pkSequence->m_uiSequenceID);

        if (uiVersion < NiStream::GetVersion(1, 2, 5, 0))
        {
            char* pcName = NULL;
            LoadCString(kFile, pcName);
            NiFree(pcName);
        }
        
        LoadCStringAsFixedString(kFile, pkSequence->m_kFilename);
        NiStandardizeFilePath(pkSequence->m_kFilename);
        if (uiVersion < NiStream::GetVersion(2, 2, 0, 0))
        {
            CorrectPathSlash(pkSequence->m_kFilename);
        }

        if (pcDefaultKFPath)
        {
            // Convert old default paths.
            char acFilename[NI_MAX_PATH];
            NiStrcpy(acFilename, NI_MAX_PATH, pcDefaultKFPath);
            NiStrcat(acFilename, NI_MAX_PATH, pkSequence->GetFilename());
            pkSequence->SetFilename(acFilename);
        }

        if (uiVersion >= NiStream::GetVersion(2, 5, 0, 0))
        {
            LoadCStringAsFixedString(kFile, pkSequence->m_kSequenceName);
        }
        else
        {
            NiStreamLoadBinary(kFile, pkSequence->m_iAnimIndex);
        }

        // Load transitions.
        unsigned int uiNumTransitions;
        NiStreamLoadBinary(kFile, uiNumTransitions);
        for (unsigned int uj = 0; uj < uiNumTransitions; uj++)
        {
            unsigned int uiDesID;
            NiStreamLoadBinary(kFile, uiDesID);
            TransitionType eType;
            NiBinaryStreamLoadEnum(kFile, &eType);

            Transition* pkTransition;
            if (eType == TYPE_DEFAULT_SYNC)
            {
                pkTransition = m_pkDefaultSyncTrans;
            }
            else if (eType == TYPE_DEFAULT_NONSYNC)
            {
                pkTransition = m_pkDefaultNonSyncTrans;
            }
            else
            {
                pkTransition = NiNew Transition;
                pkTransition->m_eType = eType;

                NiStreamLoadBinary(kFile, pkTransition->m_fDuration);

                unsigned int uiNumBlendPairs;
                NiStreamLoadBinary(kFile, uiNumBlendPairs);
                unsigned int uk;
                for (uk = 0; uk < uiNumBlendPairs; uk++)
                {
                    Transition::BlendPair* pkPair =
                        NiNew Transition::BlendPair;
                    if (uiVersion < NiStream::GetVersion(2, 1, 0, 0))
                    {
                        LoadCStringAsFixedString(kFile, pkPair->m_kStartKey);
                        LoadCStringAsFixedString(kFile, pkPair->m_kTargetKey);
                    }
                    else
                    {
                        LoadFixedString(kFile, pkPair->m_kStartKey);
                        LoadFixedString(kFile, pkPair->m_kTargetKey);
                    }

                    pkTransition->m_aBlendPairs.Add(pkPair);
                }

                unsigned int uiNumChainSequences;
                NiStreamLoadBinary(kFile, uiNumChainSequences);
                if (uiVersion < NiStream::GetVersion(1, 2, 4, 0) &&
                    uiNumChainSequences > 0)
                {
                    // The source sequence used to be stored in chains.
                    // This is no longer the case. Thus, we ignore the first
                    // item in the array and decrement the array count here.
                    NIASSERT(uiNumChainSequences > 1);
                    Transition::ChainInfo kTemp;
                    NiStreamLoadBinary(kFile, kTemp.m_uiSequenceID);
                    NiStreamLoadBinary(kFile, kTemp.m_fDuration);
                    uiNumChainSequences--;
                }
                for (uk = 0; uk < uiNumChainSequences; uk++)
                {
                    Transition::ChainInfo kInfo;
                    NiStreamLoadBinary(kFile, kInfo.m_uiSequenceID);
                    NiStreamLoadBinary(kFile, kInfo.m_fDuration);
                    pkTransition->m_aChainInfo.Add(kInfo);
                }
            }

            pkSequence->m_mapTransitions.SetAt(uiDesID, pkTransition);
        }

        m_mapSequences.SetAt(pkSequence->m_uiSequenceID, pkSequence);
    }

    // Load sequence groups.
    unsigned int uiNumSequenceGroups;
    NiStreamLoadBinary(kFile, uiNumSequenceGroups);
    for (ui = 0; ui < uiNumSequenceGroups; ui++)
    {
        SequenceGroup* pkGroup = NiNew SequenceGroup;
        NiStreamLoadBinary(kFile, pkGroup->m_uiGroupID);

        if (uiVersion < NiStream::GetVersion(2, 1, 0, 0))
        {
            LoadCStringAsFixedString(kFile, pkGroup->m_kName);
        }
        else
        {
            LoadFixedString(kFile, pkGroup->m_kName);
        }

        unsigned int uiSequencesCount;
        NiStreamLoadBinary(kFile, uiSequencesCount);
        for (unsigned int uj = 0; uj < uiSequencesCount; uj++)
        {
            SequenceGroup::SequenceInfo kInfo;
            NiStreamLoadBinary(kFile, kInfo.m_uiSequenceID);
            NiStreamLoadBinary(kFile, kInfo.m_iPriority);
            NiStreamLoadBinary(kFile, kInfo.m_fWeight);
            NiStreamLoadBinary(kFile, kInfo.m_fEaseInTime);
            NiStreamLoadBinary(kFile, kInfo.m_fEaseOutTime);
            if (uiVersion >= NiStream::GetVersion(1, 2, 1, 0))
            {
                NiStreamLoadBinary(kFile, kInfo.m_uiSynchronizeSequenceID);
            }
            else
            {
                kInfo.m_uiSynchronizeSequenceID = SYNC_SEQUENCE_ID_NONE;
            }
            pkGroup->m_aSequenceInfo.Add(kInfo);
        }

        m_mapSequenceGroups.SetAt(pkGroup->m_uiGroupID, pkGroup);
    }

    NiFree(pcDefaultNIFPath);
    NiFree(pcDefaultKFPath);

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::ReadAscii(NiFile&, unsigned int)
{
    // ASCII KFM files are not supported.
    NIASSERT(false);
    return KFM_ERR_FILE_FORMAT;
}
//---------------------------------------------------------------------------
NiKFMTool::KFM_RC NiKFMTool::ReadOldVersionAscii(NiFile& kFile,
    unsigned int)
{

    char acBuf[256];
    char acSeps[] = "#\n";
    char* pcToken;

    bool bFoundDefaultPaths = false;
    char* pcDefaultNIFPath = NULL;
    char* pcDefaultKFPath = NULL;
    unsigned int uiCurID = 0;
    char* pcContext;
    while (kFile.GetLine(acBuf, 256) > 0)
    {
        pcToken = NiStrtok(acBuf, acSeps, &pcContext);

        if (pcToken && pcToken[0] != ';')
        {              
            if (strcmp(pcToken, "END_KFM_FILE") == 0)
            {
                break;
            }

            if (strcmp(pcToken, "DEFAULTPATHS") == 0)
            {
                // Get default paths.
                char* pcDefaultPaths = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcDefaultPaths)
                {
                    return KFM_ERR_FILE_FORMAT;
                }

                bool bDefaultPaths = (atoi(pcDefaultPaths) == 1) ?
                    true : false;
                bFoundDefaultPaths = true;

                if (bDefaultPaths)
                {
                    if (kFile.GetLine(acBuf, 256) == 0)
                    {
                        return KFM_ERR_FILE_FORMAT;
                    }
                    char* pcTempDefaultNIFPath = 
                        NiStrtok(acBuf, acSeps, &pcContext);
                    if (!pcTempDefaultNIFPath ||
                        strcmp(pcTempDefaultNIFPath, "DEFAULTNIFPATH") != 0)
                    {
                        return KFM_ERR_FILE_FORMAT;
                    }
                    pcTempDefaultNIFPath = NiStrtok(NULL, acSeps, &pcContext);
                    if (!pcTempDefaultNIFPath)
                    {
                        return KFM_ERR_FILE_FORMAT;
                    }
                    else if (strcmp(pcTempDefaultNIFPath, "NULL") == 0)
                    {
                        pcTempDefaultNIFPath = NULL;
                    }
                    if (pcTempDefaultNIFPath)
                    {
                        size_t uiLen = strlen(pcTempDefaultNIFPath) + 1;
                        pcDefaultNIFPath = NiAlloc(char, uiLen);
                        NiStrcpy(pcDefaultNIFPath, uiLen, 
                            pcTempDefaultNIFPath);
                    }

                    if (kFile.GetLine(acBuf, 256) == 0)
                    {
                        return KFM_ERR_FILE_FORMAT;
                    }
                    char* pcTempDefaultKFPath = 
                        NiStrtok(acBuf, "#", &pcContext);
                    if (!pcTempDefaultKFPath ||
                        strcmp(pcTempDefaultKFPath, "DEFAULTKFPATH") != 0)
                    {
                        return KFM_ERR_FILE_FORMAT;
                    }
                    pcTempDefaultKFPath = NiStrtok(NULL, acSeps, &pcContext);
                    if (!pcTempDefaultKFPath)
                    {
                        return KFM_ERR_FILE_FORMAT;
                    }
                    else if (strcmp(pcTempDefaultKFPath, "NULL") == 0)
                    {
                        pcTempDefaultKFPath = NULL;
                    }
                    if (pcTempDefaultKFPath)
                    {
                        size_t uiLen = strlen(pcTempDefaultKFPath) + 1;
                        pcDefaultKFPath = NiAlloc(char, uiLen);
                        NiStrcpy(pcDefaultKFPath, uiLen, pcTempDefaultKFPath);
                    }
                }
            }
            else if (strcmp(pcToken, "MODEL") == 0)
            {
                // Get model path.
                char* pcModelPath = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcModelPath)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                else if (strcmp(pcModelPath, "NULL") == 0)
                {
                    pcModelPath = NULL;
                }

                // Set model path.
                if (pcDefaultNIFPath)
                {
                    // Convert old default paths.
                    char acModelPath[NI_MAX_PATH];
                    NiStrcpy(acModelPath, NI_MAX_PATH, pcDefaultNIFPath);
                    NiStrcat(acModelPath, NI_MAX_PATH, pcModelPath);
                    SetModelPath(acModelPath);
                }
                else
                {
                    SetModelPath(pcModelPath);
                }

                // Get model root.
                char* pcModelRoot = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcModelRoot)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                else if (strcmp(pcModelRoot, "NULL") == 0)
                {
                    pcModelRoot = NULL;
                }

                // Set model root.
                SetModelRoot(pcModelRoot);
            }
            else if (strcmp(pcToken, "ANIMATION") == 0)
            {
                // Get event code.
                char* pcSequenceID = NiStrtok(NULL, " \n", &pcContext);
                if (!pcSequenceID || strcmp(pcSequenceID,
                    "EVENTCODE") != 0)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                pcSequenceID = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcSequenceID)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                unsigned int uiSequenceID = atoi(pcSequenceID);

                // Get animation name.
                char* pcName = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcName)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                else if (strcmp(pcName, "NULL") == 0)
                {
                    pcName = NULL;
                }

                // Get animation filename.
                char* pcFilename = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcFilename)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                else if (strcmp(pcFilename, "NULL") == 0)
                {
                    pcFilename = NULL;
                }
                char acFilename[NI_MAX_PATH];
                if (pcDefaultKFPath)
                {
                    // Convert old default paths.
                    NiStrcpy(acFilename, NI_MAX_PATH, pcDefaultKFPath);
                    NiStrcat(acFilename, NI_MAX_PATH, pcFilename);
                }
                else
                {
                    NiStrcpy(acFilename, NI_MAX_PATH, pcFilename);
                }

                // Get animation animation index.
                char* pcAnimIndex = NiStrtok(NULL, " \n", &pcContext);
                if (!pcAnimIndex || strcmp(pcAnimIndex, "INDEX") != 0)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                pcAnimIndex = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcAnimIndex)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                int iAnimIndex = atoi(pcAnimIndex);

                // Add the animation.
                AddSequence(uiSequenceID, acFilename, iAnimIndex);

                uiCurID = uiSequenceID;
            }
            else if (strcmp(pcToken, "\tTRANSITION") == 0)
            {
                // Get event code.
                char* pcSequenceID = NiStrtok(NULL, " \n", &pcContext);
                if (!pcSequenceID || strcmp(pcSequenceID, "EVENTCODE") != 0)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                pcSequenceID = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcSequenceID)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                unsigned int uiSequenceID = atoi(pcSequenceID);

                // Get type.
                char* pcType = NiStrtok(NULL, " \n", &pcContext);
                if (!pcType || strcmp(pcType, "TYPE") != 0)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                pcType = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcType)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                bool bBlend;
                if (strcmp(pcType, "BLEND") == 0)
                {
                    bBlend = true;
                }
                else if (strcmp(pcType, "MORPH") == 0)
                {
                    bBlend = false;
                }
                else
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                // Get duration.
                char* pcDuration = NiStrtok(NULL, " \n", &pcContext);
                if (!pcDuration || strcmp(pcDuration, "DURATION") != 0)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                pcDuration = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcDuration)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                float fDuration = (float) atof(pcDuration);

                // Add the transition.
                TransitionType eType;
                if (bBlend)
                {
                    eType = TYPE_BLEND;
                }
                else
                {
                    eType = TYPE_MORPH;
                }
                AddTransition(uiCurID, uiSequenceID, eType, fDuration);

                // Get intermediate animations.
                unsigned int uiNumAnims = 0;
                char* pcNumAnims = NiStrtok(NULL, " \n", &pcContext);
                if (!pcNumAnims || strcmp(pcNumAnims,
                    "INTERMEDIATEANIMS") != 0)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                pcNumAnims = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcNumAnims)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                uiNumAnims = atoi(pcNumAnims);

                // If intermediate animations exist, this transition is
                // really a chain. Change its type here.
                if (uiNumAnims > 0)
                {
                    Transition* pkTransition = GetTransition(uiCurID,
                        uiSequenceID);
                    NIASSERT(pkTransition);
                    pkTransition->m_eType = TYPE_CHAIN;
                }

                // Add intermediate animations.
                unsigned int ui;
                for (ui = 0; ui < uiNumAnims; ui++)
                {
                    char* pcChainID = NiStrtok(NULL, acSeps, &pcContext);
                    if (!pcChainID)
                    {
                        return KFM_ERR_FILE_FORMAT;
                    }

                    unsigned int uiChainID = atoi(pcChainID);
                    AddSequenceToChain(uiCurID, uiSequenceID, uiChainID,
                        MAX_DURATION);
                }

                // Get text keys.
                unsigned int uiNumPairs = 1;
                char* pcNumPairs = NiStrtok(NULL, " \n", &pcContext);
                if (!pcNumPairs ||
                    strcmp(pcNumPairs, "TEXTKEYPAIRS") != 0)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                pcNumPairs = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcNumPairs)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                uiNumPairs = atoi(pcNumPairs);

                for (ui = 0; ui < uiNumPairs; ui++)
                {
                    // Get start key.
                    char* pcStartKey = NiStrtok(NULL, acSeps, &pcContext);
                    if (!pcStartKey)
                    {
                        return KFM_ERR_FILE_FORMAT;
                    }
                    else if (strcmp(pcStartKey, "NULL") == 0)
                        pcStartKey = NULL;

                    // Get target key.
                    char* pcTargetKey = NiStrtok(NULL, acSeps, &pcContext);
                    if (!pcTargetKey)
                    {
                        return KFM_ERR_FILE_FORMAT;
                    }
                    else if (strcmp(pcTargetKey, "NULL") == 0)
                    {
                        pcTargetKey = NULL;
                    }

                    if (pcStartKey || pcTargetKey)
                    {
                        AddBlendPair(uiCurID, uiSequenceID,
                            pcStartKey, pcTargetKey);
                    }
                }
            }
            else if (strcmp(pcToken, "LAYERGROUP") == 0)
            {
                // Get group ID.
                char* pcGroupID = NiStrtok(NULL, " \n", &pcContext);
                if (!pcGroupID || strcmp(pcGroupID, "GROUPID") != 0)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                pcGroupID = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcGroupID)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                unsigned int uiGroupID = atoi(pcGroupID);

                // Get group name.
                char* pcName = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcName)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                else if (strcmp(pcName, "NULL") == 0)
                {
                    pcName = NULL;
                }

                // Add the layer group.
                AddSequenceGroup(uiGroupID, pcName);

                // Add layers.
                char* pcNumLayers = NiStrtok(NULL, " \n", &pcContext);
                if (!pcNumLayers || strcmp(pcNumLayers, "LAYERS") != 0)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                pcNumLayers = NiStrtok(NULL, acSeps, &pcContext);
                if (!pcNumLayers)
                {
                    return KFM_ERR_FILE_FORMAT;
                }
                unsigned int uiNumLayers = atoi(pcNumLayers);

                for (unsigned int ui = 0; ui < uiNumLayers; ui++)
                {
                    // Get event code.
                    char* pcSequenceID = NiStrtok(NULL, acSeps, &pcContext);
                    if (!pcSequenceID)
                    {
                        return KFM_ERR_FILE_FORMAT;
                    }
                    unsigned int uiSequenceID = atoi(pcSequenceID);

                    // Get priority.
                    char* pcPriority = NiStrtok(NULL, acSeps, &pcContext);
                    if (!pcPriority)
                    {
                        return KFM_ERR_FILE_FORMAT;
                    }
                    int iPriority = atoi(pcPriority);

                    // Add layer to group.
                    KFM_RC eRC = 
                        AddSequenceToGroup(uiGroupID, uiSequenceID,
                        iPriority, 1.0f, 0.0f, 0.0f);
                    NI_UNUSED_ARG(eRC);
                    NIASSERT(eRC == KFM_SUCCESS);
                }
            }
            else
            {
                return KFM_ERR_FILE_FORMAT;
            }
        }
    }

    NiFree(pcDefaultNIFPath);
    NiFree(pcDefaultKFPath);

    HandleDelayedBlendsInChains();

    return KFM_SUCCESS;
}
//---------------------------------------------------------------------------
void NiKFMTool::HandleDelayedBlendsInChains()
{
    NiTMapIterator spos = m_mapSequences.GetFirstPos();
    while (spos)
    {
        unsigned int uiSrcID;
        Sequence* pkSequence;
        m_mapSequences.GetNext(spos, uiSrcID, pkSequence);

        NiTMapIterator tpos = pkSequence->GetTransitions().GetFirstPos();
        while (tpos)
        {
            unsigned int uiDesID;
            Transition* pkTransition;
            pkSequence->GetTransitions().GetNext(tpos, uiDesID, pkTransition);

            if (pkTransition->GetType() == TYPE_CHAIN)
            {
                for (unsigned int ui = 0;
                    ui < pkTransition->GetChainInfo().GetSize(); ui++)
                {
                    Transition::ChainInfo& kInfo1 =
                        pkTransition->GetChainInfo().GetAt(ui);
                    if (kInfo1.GetDuration() != MAX_DURATION)
                    {
                        continue;
                    }

                    unsigned int uiChainID2;
                    if (ui + 1 < pkTransition->GetChainInfo().GetSize())
                    {
                        uiChainID2 = pkTransition->GetChainInfo()
                            .GetAt(ui + 1).GetSequenceID();
                    }
                    else
                    {
                        uiChainID2 = uiDesID;
                    }

                    Transition* pkChainTrans = GetTransition(
                        kInfo1.GetSequenceID(), uiChainID2);
                    if (pkChainTrans && pkChainTrans->GetType() == TYPE_BLEND
                        && pkChainTrans->GetBlendPairs().GetSize() == 1)
                    {
                        Transition::BlendPair* pkPair =
                            pkChainTrans->GetBlendPairs().GetAt(0);
                        if (pkPair->GetStartKey() &&
                            strcmp(pkPair->GetStartKey(), 
                            NiAnimationConstants::GetEndTextKey()) == 0)
                        {
                            kInfo1.SetDuration(0.0f);
                        }
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiKFMTool::SaveCString(NiFile& kFile, const char* pcString)
{
    size_t uiLength = (pcString ? strlen(pcString) : 0);
    NiStreamSaveBinary(kFile, uiLength);
    if (uiLength > 0)
    {
        kFile.Write(pcString, (unsigned int)uiLength);
    }
}
//---------------------------------------------------------------------------
void NiKFMTool::SaveFixedString(NiFile& kFile, const NiFixedString& kString)
{
    size_t uiLength = kString.GetLength();
    NIASSERT(uiLength < 1024);
    NiStreamSaveBinary(kFile, uiLength);
    if (uiLength > 0)
    {
        kFile.Write((const char*)kString, (unsigned int)uiLength);
    }
}
//---------------------------------------------------------------------------
void NiKFMTool::LoadCString(NiFile& kFile, char*& pcString)
{
    int iLength;
    NiStreamLoadBinary(kFile, iLength);
    if (iLength > 0)
    {
        pcString = NiAlloc(char, iLength + 1);
        NIASSERT(pcString);
        kFile.Read(pcString, iLength);
        pcString[iLength] = 0;
    }
    else
    {
        pcString = NULL;
    }
}
//---------------------------------------------------------------------------
void NiKFMTool::LoadFixedString(NiFile& kFile, NiFixedString& kString)
{
    int iLength;
    NiStreamLoadBinary(kFile, iLength);
    NIASSERT(iLength < 1024);
    if (iLength > 0)
    {
        char acString[1024];
        kFile.Read(acString, iLength);
        acString[iLength] = 0;
        kString = acString;
    }
    else
    {
        kString = NULL;
    }
}
//---------------------------------------------------------------------------
void NiKFMTool::LoadCStringAsFixedString(NiFile& kFile, NiFixedString& kString)
{
    int iLength;
    NiStreamLoadBinary(kFile, iLength);
    NIASSERT(iLength < 1024);
    if (iLength > 0)
    {
        char acString[1024];
        kFile.Read(acString, iLength);
        acString[iLength] = 0;
        kString = acString;
    }
    else
    {
        kString = NULL;
    }
}
//---------------------------------------------------------------------------
