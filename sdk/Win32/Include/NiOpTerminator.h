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

#ifndef NIOPTERMINATOR_H
#define NIOPTERMINATOR_H

#include "NiOpNode.h"

// Invalid share group to signify that it should not be interleaved
#define INVALID_SHARE_GROUP 0xffffffff

/// @cond EMERGENT_INTERNAL
/**
    Special Operation node that specifies the end of all mesh operation
    processing for a particular stream. The class also stores information
    for specifying final access flags, interleaving rquirements, etc.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiOpTerminator : public NiOpNode
{
    NiDeclareClone(NiOpTerminator);

public:
    NiOpTerminator();
    virtual ~NiOpTerminator();

    // A recursive function that resolves the output for the node.
    virtual bool Resolve(NiMeshErrorInterface* pkReportedErrors);
    virtual NiFixedString GetOperationName();
    virtual bool RetrieveExpectedFinalAccessAndUsage(
        NiUInt8& uiAccessFlags, NiDataStream::Usage& eUsage) const;

    bool SetParameters(NiUInt32 uiStreamID, const NiUInt8 uiAccessFlags, 
        const NiDataStream::Usage eUsage, const NiUInt8 uiStrictFlags, 
        bool bRequired);

    // Specialized
    NiUInt32 GetStreamID() const;
    NiUInt8 GetAccessFlags() const;
    NiDataStream::Usage GetUsage() const;
    NiUInt8 GetStrictFlags() const;
    bool IsStrictInterleave() const;
    bool IsStrictFormat() const;

    NiDataStreamRef RemoveOutput();

    bool IsRequired();
    virtual bool IsEqual(NiObject* pkOther);

protected:
    NiUInt32 m_uiStreamID;
    NiUInt8 m_uiAccessFlags;
    NiUInt8 m_uiStrictFlags;
    NiDataStream::Usage m_eUsage;
    bool m_bRequired;
};

NiSmartPointer(NiOpTerminator);

#endif // NIOPTERMINATOR_H