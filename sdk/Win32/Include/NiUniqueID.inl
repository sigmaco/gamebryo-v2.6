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

#include <NiSystem.h>

//---------------------------------------------------------------------------
inline NiUniqueID::NiUniqueID(bool bGenerate)
{
    memset(&m_aucValue, 0, NUM_BYTES_IN_UNIQUE_ID);
    if (bGenerate)
    {
        NIVERIFY(Generate(*this));
    }
}
//---------------------------------------------------------------------------
inline NiUniqueID::NiUniqueID(const unsigned char* pucID)
{
    NiMemcpy(&m_aucValue, NUM_BYTES_IN_UNIQUE_ID, pucID,
        NUM_BYTES_IN_UNIQUE_ID);
}
//---------------------------------------------------------------------------
inline NiUniqueID::NiUniqueID(unsigned char ucA, unsigned char ucB,
    unsigned char ucC, unsigned char ucD, unsigned char ucE,
    unsigned char ucF, unsigned char ucG, unsigned char ucH,
    unsigned char ucI, unsigned char ucJ, unsigned char ucK,
    unsigned char ucL, unsigned char ucM, unsigned char ucN,
    unsigned char ucO, unsigned char ucP)
{
    m_aucValue[0] = ucA;
    m_aucValue[1] = ucB;
    m_aucValue[2] = ucC;
    m_aucValue[3] = ucD;
    m_aucValue[4] = ucE;
    m_aucValue[5] = ucF;
    m_aucValue[6] = ucG;
    m_aucValue[7] = ucH;
    m_aucValue[8] = ucI;
    m_aucValue[9] = ucJ;
    m_aucValue[10] = ucK;
    m_aucValue[11] = ucL;
    m_aucValue[12] = ucM;
    m_aucValue[13] = ucN;
    m_aucValue[14] = ucO;
    m_aucValue[15] = ucP;
}
//---------------------------------------------------------------------------
inline bool NiUniqueID::operator==(const NiUniqueID& kUniqueID) const
{
    return (memcmp(&m_aucValue, &kUniqueID.m_aucValue, NUM_BYTES_IN_UNIQUE_ID)
        == 0);
}
//---------------------------------------------------------------------------
inline bool NiUniqueID::operator!=(const NiUniqueID& kUniqueID) const
{
    return !(*this == kUniqueID);
}
//---------------------------------------------------------------------------
inline NiString NiUniqueID::ToString() const
{
    NiString kString(NUM_BYTES_IN_UNIQUE_ID * 3);
    for (unsigned int ui = 0; ui < NUM_BYTES_IN_UNIQUE_ID; ui++)
    {
        char acBuff[4];
        NiSprintf(acBuff, 4, "%02x", m_aucValue[ui]);
        kString.Concatenate(acBuff);
        if (ui != NUM_BYTES_IN_UNIQUE_ID - 1)
        {
            kString.Concatenate(',');
        }
    }

    return kString;
}
//---------------------------------------------------------------------------
inline bool NiUniqueID::FromString(const char* pcString)
{
    NIASSERT(pcString);

    // This function is written assuming 16 bytes in a unique ID. If the
    // number of bytes changes, this function will need to be rewritten.
    NIASSERT(NUM_BYTES_IN_UNIQUE_ID == 16);

    int iA, iB, iC, iD, iE, iF, iG, iH, iI, iJ, iK, iL, iM, iN, iO, iP;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int iFieldsAssigned = sscanf_s(pcString, 
        "%x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x", 
        &iA,&iB,&iC,&iD,&iE,&iF,&iG,&iH,&iI,&iJ,&iK,&iL,&iM,&iN,&iO,&iP);
#else
    int iFieldsAssigned = sscanf(pcString, 
        "%x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x", 
        &iA,&iB,&iC,&iD,&iE,&iF,&iG,&iH,&iI,&iJ,&iK,&iL,&iM,&iN,&iO,&iP);
#endif
    if (iFieldsAssigned != NUM_BYTES_IN_UNIQUE_ID)
    {
        return false;
    }

    *this = NiUniqueID(
        (unsigned char) iA, (unsigned char) iB, (unsigned char) iC,
        (unsigned char) iD, (unsigned char) iE, (unsigned char) iF,
        (unsigned char) iG, (unsigned char) iH, (unsigned char) iI,
        (unsigned char) iJ, (unsigned char) iK, (unsigned char) iL,
        (unsigned char) iM, (unsigned char) iN, (unsigned char) iO,
        (unsigned char) iP);

    return true;
}
//---------------------------------------------------------------------------
inline NiUniqueID::NiUniqueID(int)
{
    // This constructor should only be used by the Gamebryo container classes.
    // It is not valid to pass anything other then 0 into this function.

    memset(&m_aucValue, 0, NUM_BYTES_IN_UNIQUE_ID);
}
//---------------------------------------------------------------------------
inline unsigned int NiUniqueID::ComputeHashValue() const
{
    return (
        *((unsigned int*) &m_aucValue[0]) ^
        *((unsigned int*) &m_aucValue[4]) ^
        *((unsigned int*) &m_aucValue[8]) ^
        *((unsigned int*) &m_aucValue[12]));
}
//---------------------------------------------------------------------------
inline unsigned int NiUniqueID::HashFunctor::KeyToHashIndex(
    const NiUniqueID* pkKey,
    unsigned int uiTableSize)
{
    return pkKey->ComputeHashValue() % uiTableSize;
}
//---------------------------------------------------------------------------
inline bool NiUniqueID::HashFunctor::IsKeysEqual(
    const NiUniqueID* pkKey1,
    const NiUniqueID* pkKey2)
{
    return *pkKey1 == *pkKey2;
}
//---------------------------------------------------------------------------
