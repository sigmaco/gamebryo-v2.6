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
inline void _NiSwap(char* pcA, char* pcB)
{
    NIASSERT(pcA != pcB);
    char acValue = pcA[0];
    pcA[0] = pcB[0];
    pcB[0] = acValue;
}
//---------------------------------------------------------------------------
inline void NiEndian::Swap16(char* pcValue, unsigned int uiCount)
{
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        _NiSwap(&pcValue[0], &pcValue[1]);
        pcValue += 2;
    }
}
//---------------------------------------------------------------------------
inline void NiEndian::Swap32(char* pcValue, unsigned int uiCount)
{
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        _NiSwap(&pcValue[0], &pcValue[3]);
        _NiSwap(&pcValue[1], &pcValue[2]);
        pcValue += 4; 
    }
}
//---------------------------------------------------------------------------
inline void NiEndian::Swap64(char* pcValue, unsigned int uiCount)
{
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        _NiSwap(&pcValue[0], &pcValue[7]);
        _NiSwap(&pcValue[1], &pcValue[6]);
        _NiSwap(&pcValue[2], &pcValue[5]);
        _NiSwap(&pcValue[3], &pcValue[4]);
        pcValue += 8;
    }
}
//---------------------------------------------------------------------------
