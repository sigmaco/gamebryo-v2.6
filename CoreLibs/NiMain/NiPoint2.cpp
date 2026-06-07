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

#include "NiPoint2.h"

const NiPoint2 NiPoint2::ZERO(0.0f,0.0f);
const NiPoint2 NiPoint2::UNIT_X(1.0f,0.0f);
const NiPoint2 NiPoint2::UNIT_Y(0.0f,1.0f);

//---------------------------------------------------------------------------
void NiPoint2::LoadBinary(NiStream& stream)
{
    NiStreamLoadBinary(stream,x);
    NiStreamLoadBinary(stream,y);
}
//---------------------------------------------------------------------------
void NiPoint2::SaveBinary(NiStream& stream)
{
    NiStreamSaveBinary(stream,x);
    NiStreamSaveBinary(stream,y);
}
//---------------------------------------------------------------------------
char* NiPoint2::GetViewerString(const char* pPrefix) const
{
    size_t stLen = strlen(pPrefix) + 36;
    char* pString = NiAlloc(char, stLen);
    NiSprintf(pString, stLen, "%s = (%g,%g)", pPrefix, x, y);
    return pString;
}
//---------------------------------------------------------------------------
