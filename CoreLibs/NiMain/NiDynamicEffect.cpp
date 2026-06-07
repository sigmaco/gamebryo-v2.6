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
#include "NiMainPCH.h"

#include "NiBool.h"
#include "NiCloningProcess.h"
#include "NiDynamicEffect.h"
#include "NiNode.h"
#include "NiShadowManager.h"
#include "NiRenderer.h"
#include "NiShadowGenerator.h"

NiImplementRTTI(NiDynamicEffect,NiAVObject);

int NiDynamicEffect::ms_iNextIndex = 1;

//---------------------------------------------------------------------------
#if !defined(WIN32) && !defined(_XENON)
NiCriticalSection NiDynamicEffect::ms_kCritSec;
#endif
//---------------------------------------------------------------------------
NiDynamicEffect::NiDynamicEffect() : m_bOn(true), m_ucEffectType(0xFF),
    m_uiPushCount(0),  m_uiRevID(1), m_pkShadowGenerator(NULL)
{
    // bookkeeping
#if !defined(WIN32) && !defined(_XENON)
    ms_kCritSec.Lock();
    m_iIndex = ms_iNextIndex++;
    ms_kCritSec.Unlock();
#else
    m_iIndex = InterlockedIncrement((LONG*)&ms_iNextIndex);
#endif
}
//---------------------------------------------------------------------------
NiDynamicEffect::~NiDynamicEffect()
{
    DetachAllAffectedNodes();
    DetachAllUnaffectedNodes();
    if (m_pkShadowGenerator)
    {
        m_pkShadowGenerator->SetDynamicEffect(NULL);
        if (NiShadowManager::GetShadowManager())
        {
            NiShadowManager::DeleteShadowGenerator(m_pkShadowGenerator);
        }
    }
}
//---------------------------------------------------------------------------
void NiDynamicEffect::SetSwitch(bool bOn, bool bSetShadowGenerator)
{
    m_bOn = bOn;

    // Update the NiShadowGenerator object to reflect the dynamic effect's 
    // switch state.
    if (bSetShadowGenerator && m_pkShadowGenerator)
      m_pkShadowGenerator->SetActive(m_bOn);
}
//---------------------------------------------------------------------------
void NiDynamicEffect::AttachAffectedNode(NiNode* pkAffectedNode)
{
    // ensure that we don't recurse forever
    if(m_kAffectedNodeList.FindPos(pkAffectedNode))
        return;

    m_kAffectedNodeList.AddHead(pkAffectedNode);
    pkAffectedNode->AttachEffect(this);
}

//---------------------------------------------------------------------------
void NiDynamicEffect::DetachAffectedNode(NiNode* pkAffectedNode)
{
    // ensure that we don't recurse forever
    if(!m_kAffectedNodeList.FindPos(pkAffectedNode))
        return;

    m_kAffectedNodeList.Remove(pkAffectedNode);
    pkAffectedNode->DetachEffect(this);
}
//---------------------------------------------------------------------------
void NiDynamicEffect::DetachAllAffectedNodes()
{
    while(!m_kAffectedNodeList.IsEmpty())
    {
        NiNode *pkNode = m_kAffectedNodeList.GetHead();
        m_kAffectedNodeList.RemoveHead();
        pkNode->DetachEffect(this);
    }
}
//---------------------------------------------------------------------------
void NiDynamicEffect::AttachUnaffectedNode(NiNode* pkUnaffectedNode)
{
    // ensure that we don't recurse forever
    if(m_kUnaffectedNodeList.FindPos(pkUnaffectedNode))
        return;

    m_kUnaffectedNodeList.AddHead(pkUnaffectedNode);
}
//---------------------------------------------------------------------------
void NiDynamicEffect::DetachUnaffectedNode(NiNode* pkUnaffectedNode)
{
    // ensure that we don't recurse forever
    if(!m_kUnaffectedNodeList.FindPos(pkUnaffectedNode))
        return;

    m_kUnaffectedNodeList.Remove(pkUnaffectedNode);
}
//---------------------------------------------------------------------------
void NiDynamicEffect::DetachAllUnaffectedNodes()
{
    while(!m_kUnaffectedNodeList.IsEmpty())
    {
        m_kUnaffectedNodeList.RemoveHead();
    }
}
//---------------------------------------------------------------------------
void NiDynamicEffect::AssignShadowGenerator(
    NiShadowGenerator* pkShadowGenerator)
{
#ifdef NIDEBUG
    if (NiShadowManager::GetShadowManager() == NULL)
    {
        NiOutputDebugString("Warning: An NiShadowGenerator object is being "
            "applied to a NiDynamiceEffect object while there is no active "
            "shadow manager. The NiShadowGenerator object will go unused and "
            "will need to be manually deleted by the application.\n");
    }
#endif //#ifdef NIDEBUG

    m_pkShadowGenerator = pkShadowGenerator;

    // The ensures that the light type for the NiDynamicEffect is up-to-date.
    // This code requires the EffectType enum in NiDynamicEffect to be proved
    // the indexing diff between shadow casting and non-shadow casting lights.
    if (m_pkShadowGenerator && m_ucEffectType < SHADOWLIGHT_INDEX)
        m_ucEffectType +=SHADOW_INDEX_DIFF;
    else if (!m_pkShadowGenerator && m_ucEffectType >= SHADOWLIGHT_INDEX)
        m_ucEffectType -=SHADOW_INDEX_DIFF;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiDynamicEffect::CopyMembers(NiDynamicEffect* pkDest,
    NiCloningProcess& kCloning)
{
    NiAVObject::CopyMembers(pkDest, kCloning);

    // DO NOT copy the index - this must be unique across all lights, or the
    // renderers will get confused...
    // pDest->m_iIndex = m_iIndex;

    // This should be 0, as we should not be cloning during a Click pass...
    pkDest->m_uiPushCount = m_uiPushCount;

    // This is dubious - it should be safe, but the Rev ID could simply be
    // incremented, since these are two different lights.
    pkDest->m_uiRevID = m_uiRevID;

    // Copy m_bOn so that effects will retain the state.
    pkDest->m_bOn = m_bOn;

    // Copy the shadow generator if one is assigned.
    // Note that casters list and unaffected node list of the shadow generator
    // will not be copied. It is up to the application to clone this data.
    pkDest->m_pkShadowGenerator = NULL;
    if (m_pkShadowGenerator)
    {
        NiShadowGenerator* pkShadowGenerator = (NiShadowGenerator*)
            m_pkShadowGenerator->CreateClone(kCloning);
        pkShadowGenerator->SetDynamicEffect(pkDest);

        if (NiShadowManager::GetShadowManager())
        {
            NiShadowManager::AddShadowGenerator(pkShadowGenerator);
        }
        else
        {
            pkDest->m_pkShadowGenerator = pkShadowGenerator;
        }
    }

    // m_kAffectedNodeList and m_kUnaffectedNodeList are processed in
    // ProcessClone.
}
//---------------------------------------------------------------------------
void NiDynamicEffect::ProcessClone(NiCloningProcess& kCloning)
{
    // Get clone from the clone map.
    NiObject* pkClone = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkClone));
    NiDynamicEffect* pkDest = (NiDynamicEffect*) pkClone;

    if (m_pkShadowGenerator)
    {
        m_pkShadowGenerator->ProcessClone(kCloning);
    }

    // Only process affected and unaffected nodes if the cloning behavior for
    // dynamic effects is not set to CLONE_DYNEFF_NONE.
    if (kCloning.m_eDynamicEffectRelationBehavior !=
        NiCloningProcess::CLONE_RELATION_NONE)
    {
        // Iterate in reverse order so cloned lists will be in the same order.

        // Process affected nodes.
        NiTListIterator kIter = m_kAffectedNodeList.GetTailPos();
        while (kIter)
        {
            NiNode* pkAffectedNode = m_kAffectedNodeList.GetPrev(kIter);
            if (kCloning.m_pkCloneMap->GetAt(pkAffectedNode, pkClone))
            {
                pkAffectedNode = (NiNode*) pkClone;
            }
            else if (kCloning.m_eDynamicEffectRelationBehavior ==
                NiCloningProcess::CLONE_RELATION_CLONEDONLY)
            {
                pkAffectedNode = NULL;
            }

            if (pkAffectedNode)
            {
                pkDest->AttachAffectedNode(pkAffectedNode);
            }
        }
        
        // Process unaffected nodes.
        kIter = m_kUnaffectedNodeList.GetTailPos();
        while (kIter)
        {
            NiNode* pkUnaffectedNode = m_kUnaffectedNodeList.GetPrev(kIter);
            if (kCloning.m_pkCloneMap->GetAt(pkUnaffectedNode, pkClone))
            {
                pkUnaffectedNode = (NiNode*) pkClone;
            }
            else if (kCloning.m_eDynamicEffectRelationBehavior ==
                NiCloningProcess::CLONE_RELATION_CLONEDONLY)
            {
                pkUnaffectedNode = NULL;
            }

            if (pkUnaffectedNode)
            {
                pkDest->AttachUnaffectedNode(pkUnaffectedNode);
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiDynamicEffect::LoadBinary(NiStream& kStream)
{
    NiAVObject::LoadBinary(kStream);

    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 102))
    {
        NiBool bValue;
        NiStreamLoadBinary(kStream, bValue);
        m_bOn = (bValue != 0);
    }

    kStream.ReadMultipleLinkIDs();  // m_kUnaffectedNodeList
}
//---------------------------------------------------------------------------
void NiDynamicEffect::LinkObject(NiStream& kStream)
{
    NiAVObject::LinkObject(kStream);

    // link lights in reverse order (for IsEqual to work properly)
    unsigned int uiSize = kStream.GetNumberOfLinkIDs();

    while (uiSize--)
    {
        NiNode* pkNode = 
            (NiNode*) kStream.GetObjectFromLinkID();

        if (pkNode)
        {
            m_kUnaffectedNodeList.AddHead(pkNode);
        }
    }

    // Affected nodes will be linked by the nodes themselves.  They
    // should not be linked here (else you get a recursive loop).
}
//---------------------------------------------------------------------------
bool NiDynamicEffect::RegisterStreamables(NiStream& kStream)
{
    if (!NiAVObject::RegisterStreamables(kStream))
        return false;

    // Affected nodes are already registered by the nodes themselves.
    // They should not be registered here (else you get a recursive loop).

    if (m_pkShadowGenerator)
        return m_pkShadowGenerator->RegisterStreamables(kStream);    
   
    return true;
}
//---------------------------------------------------------------------------
void NiDynamicEffect::SaveBinary(NiStream& kStream)
{
    NiAVObject::SaveBinary(kStream);

    // Save switch starting with version 10.1.0.102
    NiBool bValue = m_bOn;
    NiStreamSaveBinary(kStream, bValue);

    // save unaffected nodes
    int iListSize = m_kUnaffectedNodeList.GetSize();
    NiStreamSaveBinary(kStream, iListSize);
    if (iListSize > 0)
    {
        // save in reverse order because lights will link faster that way
        NiTListIterator kPos = m_kUnaffectedNodeList.GetTailPos();
        while (kPos)
        {
            kStream.SaveLinkID(m_kUnaffectedNodeList.GetPrev(kPos));
        }
    }
}
//---------------------------------------------------------------------------
bool NiDynamicEffect::IsEqual(NiObject* pkObject)
{
    if ( !NiAVObject::IsEqual(pkObject) )
        return false;

    NiDynamicEffect* pkEffect = (NiDynamicEffect*) pkObject;

    // node pointers
    unsigned int uiCount0 = m_kAffectedNodeList.GetSize();
    unsigned int uiCount1 = pkEffect->m_kAffectedNodeList.GetSize();
    if ( uiCount0 != uiCount1 )
        return false;

    if ( uiCount0 > 0 )
    {
        NiTListIterator kPos0 = m_kAffectedNodeList.GetHeadPos();
        NiTListIterator kPos1 = pkEffect->m_kAffectedNodeList.GetHeadPos();
        while ( kPos0 )
        {
            NiNode* pkNode0 = m_kAffectedNodeList.GetNext(kPos0);
            NiNode* pkNode1 = pkEffect->m_kAffectedNodeList.GetNext(kPos1);

            if ( (pkNode0 && !pkNode1) || (!pkNode0 && pkNode1) )
                return false;

            // Do not compare pkNode0 and pkNode1 for equality here.  Same
            // recursive loop problem as in RegisterStreamables.  The
            // comparisons of lights in NiNode::IsEqual will catch any
            // nonequalities.

        }
    }

        // node pointers
    uiCount0 = m_kUnaffectedNodeList.GetSize();
    uiCount1 = pkEffect->m_kUnaffectedNodeList.GetSize();
    if ( uiCount0 != uiCount1 )
        return false;

    if ( uiCount0 > 0 )
    {
        NiTListIterator kPos0 = m_kUnaffectedNodeList.GetHeadPos();
        NiTListIterator kPos1 = pkEffect->m_kUnaffectedNodeList.GetHeadPos();
        while ( kPos0 )
        {
            NiNode* pkNode0 = m_kUnaffectedNodeList.GetNext(kPos0);
            NiNode* pkNode1 = pkEffect->m_kUnaffectedNodeList.GetNext(kPos1);

            if ( (pkNode0 && !pkNode1) || (!pkNode0 && pkNode1) )
                return false;

            // Do not compare pkNode0 and pkNode1 for equality here.  Same
            // recursive loop problem as in RegisterStreamables.  The
            // comparisons of lights in NiNode::IsEqual will catch any
            // nonequalities.

        }
    }
    return true;
}
//---------------------------------------------------------------------------
void NiDynamicEffect::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiAVObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiDynamicEffect::ms_RTTI.GetName()));

    if ( !m_kAffectedNodeList.IsEmpty())
    {
        int i = 0;
        char acPrefix[64];

        NiTListIterator kPos = m_kAffectedNodeList.GetHeadPos();
        while ( kPos )
        {
            NiNode* pkNode = m_kAffectedNodeList.GetNext(kPos);
            NiSprintf(acPrefix, 64, "affected node[%d]", i);
            pkStrings->Add(NiGetViewerString(acPrefix,pkNode));
            i++;
        }

        kPos = m_kUnaffectedNodeList.GetHeadPos();
        while ( kPos )
        {
            NiNode* pkNode = m_kUnaffectedNodeList.GetNext(kPos);
            NiSprintf(acPrefix, 64, "unaffected node[%d]", i);
            pkStrings->Add(NiGetViewerString(acPrefix,pkNode));
            i++;
        }
    }
}
//---------------------------------------------------------------------------
bool NiDynamicEffect::IsUnaffectedNode(NiNode* pkPossibleNode)
{
    NiTListIterator kPos = m_kUnaffectedNodeList.GetHeadPos();
    while ( kPos )
    {
        NiNode* pkNode = m_kUnaffectedNodeList.GetNext(kPos);
        if (pkNode == pkPossibleNode)
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
