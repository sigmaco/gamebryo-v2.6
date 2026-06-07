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

#ifndef NIUNIQUEID_H
#define NIUNIQUEID_H

#include "NiEntityLibType.h"
#include <NiMemObject.h>
#include <NiString.h>

#define NUM_BYTES_IN_UNIQUE_ID 16

class NIENTITY_ENTRY NiUniqueID 
{
public:
    NiUniqueID(bool bGenerate = false);
    NiUniqueID(const unsigned char* pucID);
    NiUniqueID(unsigned char ucA, unsigned char ucB,
        unsigned char ucC, unsigned char ucD, unsigned char ucE,
        unsigned char ucF, unsigned char ucG, unsigned char ucH,
        unsigned char ucI, unsigned char ucJ, unsigned char ucK,
        unsigned char ucL, unsigned char ucM, unsigned char ucN,
        unsigned char ucO, unsigned char ucP);
    inline bool operator==(const NiUniqueID& kUniqueID) const;
    inline bool operator!=(const NiUniqueID& kUniqueID) const;

    NiString ToString() const;
    bool FromString(const char* pcString);

    static bool Generate(NiUniqueID& kUniqueID);

    unsigned char m_aucValue[NUM_BYTES_IN_UNIQUE_ID];

    // *** begin Emergent internal use only ***
    // This constructor is only used to support assignment to 0, which is
    // required by some container classes.
    NiUniqueID(int iInt);

    // This function is used by NiUniqueIDHashFunctor to generate a 32-bit
    // hash value from the 128-bit unique ID.
    unsigned int ComputeHashValue() const;

    // A hash function class to allow NiUniqueID* objects to be a key in an
    // NiTMap.
    class HashFunctor : public NiMemObject
    {
    public:
        static unsigned int KeyToHashIndex(
            const NiUniqueID* pkKey,
            unsigned int uiTableSize);
        static bool IsKeysEqual(
            const NiUniqueID* pkKey1,
            const NiUniqueID* pkKey2);
    };
    // *** end Emergent internal use only ***
};

#include "NiUniqueID.inl"

#endif // NIUNIQUEID_H
