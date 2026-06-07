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

#include "NiOpTerminator.h"

#include <NiToolDataStream.h>
#include "NiMeshProfileProcessor.h"

NiImplementCreateClone(NiOpTerminator);

//---------------------------------------------------------------------------
NiOpTerminator::NiOpTerminator() : m_uiStreamID(0), m_uiAccessFlags(0), 
m_eUsage(NiDataStream::USAGE_MAX_TYPES), m_uiStrictFlags(0), m_bRequired(
    false) 
{
}
//---------------------------------------------------------------------------
NiOpTerminator::~NiOpTerminator()
{
}
//---------------------------------------------------------------------------
bool NiOpTerminator::Resolve(NiMeshErrorInterface* pkReportedErrors)
{
    // Terminator does not directly hold output, so m_pkOutStreamRef unused
    
    // Can not resolve until an input operation is given.
    if (!m_spInputOp)
        return false;

    if (m_spInputOp->GetOutput().IsValid())
        return true;

    if (m_spInputOp->Resolve(pkReportedErrors) == false)
        return false;

    return true;
}
//---------------------------------------------------------------------------
NiDataStreamRef NiOpTerminator::RemoveOutput()
{
    if (!m_spInputOp)
        return NiDataStreamRef();

    NiDataStreamRef kOut = m_spInputOp->GetOutput();

    // Set the access flags
    if (m_uiAccessFlags != 0)
    {
        NiToolDataStream* pkToolDS = NiDynamicCast(NiToolDataStream, 
            kOut.GetDataStream());
        if (pkToolDS)
        {
            pkToolDS->SetAccessMask(m_uiAccessFlags);
            pkToolDS->SetDefaultCloningBehavior();
        }
        else
        {
            NIASSERT(0 && "Could not cast to tool data stream");
        }
    }

    // Set the usage type
    if (m_eUsage != NiDataStream::USAGE_MAX_TYPES)
    {
        NiToolDataStream* pkToolDS = NiDynamicCast(NiToolDataStream, 
            kOut.GetDataStream());
        if (pkToolDS)
        {
            pkToolDS->SetUsage(m_eUsage);
        }
        else
        {
            NIASSERT(0 && "Could not cast to tool data stream");
        }
    }

    m_spInputOp->ClearOutput();
    return kOut;
}
//---------------------------------------------------------------------------
bool NiOpTerminator::SetParameters(NiUInt32 uiStreamID, 
    const NiUInt8 uiAccessFlags, const NiDataStream::Usage eUsage, 
    const NiUInt8 uiStrictFlags, bool bRequired)
{
    m_uiStreamID = uiStreamID;
    m_uiAccessFlags = uiAccessFlags;
    m_eUsage = eUsage;
    m_uiStrictFlags = uiStrictFlags;
    m_bRequired = bRequired;
    return true;
}
//---------------------------------------------------------------------------
bool NiOpTerminator::IsStrictInterleave() const
{
    if (m_uiStrictFlags &  NiMeshProfileProcessor::STRICT_INTERLEAVE)
        return true;

    return false;
}
//---------------------------------------------------------------------------
bool NiOpTerminator::IsStrictFormat() const
{
    if (m_uiStrictFlags & NiMeshProfileProcessor::STRICT_FORMAT)
        return true;

    return false;
}
//---------------------------------------------------------------------------
bool NiOpTerminator::IsRequired()
{
    return m_bRequired;
}
//---------------------------------------------------------------------------
NiUInt32 NiOpTerminator::GetStreamID() const
{
    return m_uiStreamID;
}
//---------------------------------------------------------------------------
NiUInt8 NiOpTerminator::GetAccessFlags() const
{
    return m_uiAccessFlags;
}
//---------------------------------------------------------------------------
NiDataStream::Usage NiOpTerminator::GetUsage() const
{
    return m_eUsage;
}
//---------------------------------------------------------------------------
NiUInt8 NiOpTerminator::GetStrictFlags() const
{
    return m_uiStrictFlags;
}
//---------------------------------------------------------------------------
bool NiOpTerminator::RetrieveExpectedFinalAccessAndUsage(
    NiUInt8& uiAccessFlags, NiDataStream::Usage& eUsage) const
{
    uiAccessFlags = GetAccessFlags();
    eUsage = GetUsage();

    // Grab DCCT access and usage
    NiUInt8 uiStreamAccess = 0;
    NiDataStream::Usage eStreamUsage = NiDataStream::USAGE_MAX_TYPES;

    if (uiAccessFlags == 0 || eUsage == NiDataStream::USAGE_MAX_TYPES)
    {
        if (m_spInputOp)
        {
            if (m_spInputOp->RetrieveExpectedFinalAccessAndUsage(
                uiStreamAccess, eStreamUsage) == false)
                return false;

            NIASSERT(uiStreamAccess != 0);
            NIASSERT(eStreamUsage != NiDataStream::USAGE_MAX_TYPES);
        }

        if (uiAccessFlags == 0)
            uiAccessFlags = uiStreamAccess;
        
        if (eUsage == NiDataStream::USAGE_MAX_TYPES)
            eUsage = eStreamUsage;
    }
    return true;
}
//---------------------------------------------------------------------------
NiFixedString NiOpTerminator::GetOperationName()
{
    return "Terminator";
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiOpTerminator::CopyMembers(NiOpTerminator* pkDest, 
    NiCloningProcess& kCloning)
{
    NiOpNode::CopyMembers(pkDest, kCloning);

    pkDest->m_uiStreamID = m_uiStreamID;
    pkDest->m_uiAccessFlags = m_uiAccessFlags;
    pkDest->m_eUsage = m_eUsage;
    pkDest->m_uiStrictFlags = m_uiStrictFlags;
    pkDest->m_bRequired = m_bRequired;
}
//---------------------------------------------------------------------------
bool NiOpTerminator::IsEqual(NiObject* pkOther)
{
    if (!NiOpNode::IsEqual(pkOther))
        return false;

    NiOpTerminator* pkOtherNode = 
        NiDynamicCast(NiOpTerminator, pkOther);

    if (!pkOtherNode)
        return false;

    if (pkOtherNode->m_uiStreamID != m_uiStreamID) 
        return false;
    if (pkOtherNode->m_uiAccessFlags != m_uiAccessFlags) 
        return false;
    if (pkOtherNode->m_eUsage != m_eUsage) 
        return false;
    if (pkOtherNode->m_uiStrictFlags != m_uiStrictFlags) 
        return false;
    if (pkOtherNode->m_bRequired != m_bRequired) 
        return false;

    return true;
}
//---------------------------------------------------------------------------
