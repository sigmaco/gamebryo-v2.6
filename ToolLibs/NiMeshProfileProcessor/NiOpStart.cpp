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

#include "NiOpStart.h"

#include <NiDataStreamLock.h>
#include "NiMPPMessages.h"

//---------------------------------------------------------------------------
const NiFixedString& NiOpStart::GetSemantic() const
{
    return m_kSemantic;
}
//---------------------------------------------------------------------------
NiUInt8 NiOpStart::GetSemanticIndex() const
{
    return m_uiIndex;
}
//---------------------------------------------------------------------------
bool NiOpStart::IsEqual(const NiFixedString& kSemantic, 
    const NiUInt8 uiIndex) const
{
    return (m_kSemantic == kSemantic && m_uiIndex == uiIndex);
}
//---------------------------------------------------------------------------
bool NiOpStart::SetParameters(const NiFixedString& kSemantic, 
    const NiUInt8 uiIndex)
{
    m_kSemantic = kSemantic;
    m_uiIndex = uiIndex;
    return true;
}
//---------------------------------------------------------------------------
bool NiOpStart::SetOutput(NiDataStreamRef& kStreamRef)
{
    m_kOutStreamRef = kStreamRef;
    return true;
}
//---------------------------------------------------------------------------
bool NiOpStart::Resolve(NiMeshErrorInterface*)
{
    if (m_kOutStreamRef.IsValid())
        return true;

    return m_kOutStreamRef.IsValid();
}
//---------------------------------------------------------------------------
bool NiOpStart::RetrieveExpectedFinalAccessAndUsage(
    NiUInt8& uiAccessFlags, NiDataStream::Usage& eUsage) const
{
    if (m_kOutStreamRef.IsValid())
    {
        const NiDataStream* pkStream = m_kOutStreamRef.GetDataStream();
        if (pkStream == NULL)
            return false;

        uiAccessFlags = pkStream->GetAccessMask();
        eUsage = pkStream->GetUsage();
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiOpStart::RetrieveExpectedFinalFormat(
    NiDataStreamElement::Format& eFormat) const
{
    // This function assumes that the start ops have been initialized.

    // From the perspective of the NiOpStart node, this is entirely 
    // dependent on the current format of the stream.
    if (m_kOutStreamRef.IsValid())
    {
        const NiDataStreamElement& kElem = m_kOutStreamRef.GetElementDescAt(0);
        eFormat = kElem.GetFormat();
        return true;
    }

    return false;
    
}
//---------------------------------------------------------------------------
bool NiOpStart::CanResolve() const
{
    if (m_kOutStreamRef.IsValid())
        return true;

    return false;
}
//---------------------------------------------------------------------------
NiFixedString NiOpStart::GetOperationName()
{
    return "Start";
}
//---------------------------------------------------------------------------
bool NiOpStart::GetSemantic(NiFixedString& kSemantic, 
    NiUInt8& uiSemanticIndex) const
{
    kSemantic = m_kSemantic;
    uiSemanticIndex = m_uiIndex;
    return true;
}
//---------------------------------------------------------------------------
void NiOpStart::GetOpStartNodes(NiTPrimitiveArray<NiOpNode*>& kNodes)
{  
    kNodes.Add(this);
}
//---------------------------------------------------------------------------
NiOpNode* NiOpStart::RetrieveFinalFormatAlteringNode()
{
    return NULL;
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiObject* NiOpStart::CreateClone(NiCloningProcess& kCloning)
{
    // Do not create the start nodes that are owned by the NiOpProfile class
    // simply return what was created if it was found other wise create one.
    NiObject* pkObject = NULL;
    kCloning.m_pkCloneMap->GetAt(this, pkObject);
    if (!pkObject)
    {
        pkObject = NiNew NiOpStart;
        NIASSERT(pkObject != NULL);
        CopyMembers((NiOpStart*)pkObject, kCloning);
    }
    return pkObject;
}
//---------------------------------------------------------------------------
void NiOpStart::CopyMembers(NiOpStart* pkDest, NiCloningProcess& kCloning)
{
    NiOpNode::CopyMembers(pkDest, kCloning);

    pkDest->m_kSemantic = m_kSemantic;
    pkDest->m_uiIndex = m_uiIndex;

    // Make sure to add ourselves to the map so we do not duplicate ourselves.
    kCloning.m_pkCloneMap->SetAt(this, pkDest);
}
//---------------------------------------------------------------------------
bool NiOpStart::IsEqual(NiObject* pkOther)
{
    if (!NiOpNode::IsEqual(pkOther))
        return false;

    NiOpStart* pkOtherStart = NiDynamicCast(NiOpStart, pkOther);

    if (!pkOtherStart)
        return false;

    if (pkOtherStart->m_kSemantic != m_kSemantic) 
        return false;
    if (pkOtherStart->m_uiIndex != m_uiIndex) 
        return false;

    return true;
}
//---------------------------------------------------------------------------
