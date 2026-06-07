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

#ifndef NIGPUPROGRAMDESCRIPTOR_H
#define NIGPUPROGRAMDESCRIPTOR_H

#include <NiMemObject.h>
#include "NiFixedString.h"
#include "NiMainLibType.h"

class NIMAIN_ENTRY NiGPUProgramDescriptor : public NiMemObject
{
public:
    NiGPUProgramDescriptor();
    NiGPUProgramDescriptor(const NiGPUProgramDescriptor&);

    bool GenerateKey(char* pcValue, unsigned int uiMaxSize) const;
    bool IsEqual(const NiGPUProgramDescriptor* pkOther) const;

    NiGPUProgramDescriptor& operator=(
        const NiGPUProgramDescriptor& kDescriptor);

    enum 
    {
        BIT_ARRAY_SIZE = 4
    };

    NiFixedString m_kIdentifier;
    unsigned int m_auiBitArray[BIT_ARRAY_SIZE];       // 128 total bits
};
#endif  //#ifndef NIGPUPROGRAMDESCRIPTOR_H
