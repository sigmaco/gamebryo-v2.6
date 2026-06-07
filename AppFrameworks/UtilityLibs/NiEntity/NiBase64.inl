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

//---------------------------------------------------------------------------
inline NiBool NiBase64::LoadFromBase64(
    NiUInt32& ui32Bits, 
    const char* pcSrcBase64, 
    int iBytes)
{
    // Typically, this function will be used to read in 4 bytes of
    // Base64 ascii characters. Since Base64 represents 6 bits, we'll
    // be reading in 24 bits of data at max.

    if (iBytes < 2 || iBytes > 4)
    {
        NIASSERT(!"The last Base64 Character can not be converted!");
        return false;
    }

    iBytes--;

    int j=0;
    do
    {
        unsigned char ucBase64Char = *pcSrcBase64++;

        // Check to see if character value is above 128.
        if (ucBase64Char & 0x80)
            return false;

        // Check to see if the character is a Base64 character
        // is in the range of 0 - 63 or [0x00 - 0x3F].
        // (table should have -1 or 0xFF for invalids)
        if (ms_aucBase64ToBit6[ucBase64Char] & 0xC0)
            return false;

        ui32Bits = (ui32Bits << 6) | ms_aucBase64ToBit6[ucBase64Char];
    } while (j++ < iBytes);

    return true;
}
//---------------------------------------------------------------------------


