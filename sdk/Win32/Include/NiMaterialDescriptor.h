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

#ifndef NIMATERIALDESCRIPTOR_H
#define NIMATERIALDESCRIPTOR_H

#include <NiMemObject.h>
#include "NiMainLibType.h"
#include "NiFixedString.h"

class NIMAIN_ENTRY NiMaterialDescriptor : public NiMemObject
{
public:
    NiMaterialDescriptor();

    bool GenerateKey(char* pcValue, unsigned int uiMaxSize);
    bool IsEqual(const NiMaterialDescriptor* pkOther) const;

    NiMaterialDescriptor& operator=(const NiMaterialDescriptor& kDescriptor);

    enum 
    {
        BIT_ARRAY_SIZE = 4
    };

    NiFixedString m_kIdentifier;
    unsigned int m_auiBitArray[BIT_ARRAY_SIZE];       // 128 total bits
};
#endif  //#ifndef NIMATERIALDESCRIPTOR_H
