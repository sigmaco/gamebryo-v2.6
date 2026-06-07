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
#include "NiColor.h"

const NiColor NiColor::BLACK(0.0f,0.0f,0.0f);
const NiColor NiColor::WHITE(1.0f,1.0f,1.0f);
const NiColorA NiColorA::BLACK(0.0f,0.0f,0.0f,1.0f);
const NiColorA NiColorA::WHITE(1.0f,1.0f,1.0f,1.0f);
const NiRGBA NiRGBA::BLACK(0,0,0,255);
const NiRGBA NiRGBA::WHITE(255,255,255,255);

#ifndef __SPU__

//---------------------------------------------------------------------------
void NiColor::LoadBinary(NiStream& stream)
{
    NiStreamLoadBinary(stream,r);
    NiStreamLoadBinary(stream,g);
    NiStreamLoadBinary(stream,b);
}
//---------------------------------------------------------------------------
void NiColor::SaveBinary(NiStream& stream)
{
    NiStreamSaveBinary(stream,r);
    NiStreamSaveBinary(stream,g);
    NiStreamSaveBinary(stream,b);
}
//---------------------------------------------------------------------------
char* NiColor::GetViewerString(const char* pPrefix) const
{
    size_t stLen = strlen(pPrefix) + 28;
    char* pString = NiAlloc(char, stLen);
    NiSprintf(pString, stLen, "%s = (%5.3f,%5.3f,%5.3f)", pPrefix, r, g, b);
    return pString;
}
//---------------------------------------------------------------------------
void NiColorA::LoadBinary(NiStream& stream)
{
    NiStreamLoadBinary(stream,r);
    NiStreamLoadBinary(stream,g);
    NiStreamLoadBinary(stream,b);
    NiStreamLoadBinary(stream,a);
}
//---------------------------------------------------------------------------
void NiColorA::SaveBinary(NiStream& stream)
{
    NiStreamSaveBinary(stream,r);
    NiStreamSaveBinary(stream,g);
    NiStreamSaveBinary(stream,b);
    NiStreamSaveBinary(stream,a);
}
//---------------------------------------------------------------------------
char* NiColorA::GetViewerString(const char* pPrefix) const
{
    size_t stLen = strlen(pPrefix) + 36;
    char* pString = NiAlloc(char, stLen);
    NiSprintf(pString, stLen, "%s = (%5.3f,%5.3f,%5.3f,%5.3f)",
        pPrefix, r, g, b, a);
    return pString;
}
//---------------------------------------------------------------------------
void NiRGBA::LoadBinary(NiStream& kStream)
{
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 4, 0, 11))
    {
        NiUInt32 uiColor;
        NiStreamLoadBinary(kStream, uiColor);

#if defined(_XENON) || defined(_PS3)
        // Byte swapping was performed upon load. Swizzle the bits so that
        // the colors are properly stored.
        r() = static_cast<NiUInt8>(uiColor);
        g() = static_cast<NiUInt8>(uiColor >> 8);
        b() = static_cast<NiUInt8>(uiColor >> 16);
        a() = static_cast<NiUInt8>(uiColor >> 24);
#else
        m_uiColor = uiColor;
#endif
    }
    else
    {
        // The color values must be streamed separately to avoid endian
        // swapping issues.
        NiStreamLoadBinary(kStream, r());
        NiStreamLoadBinary(kStream, g());
        NiStreamLoadBinary(kStream, b());
        NiStreamLoadBinary(kStream, a());
    }
}
//---------------------------------------------------------------------------
void NiRGBA::SaveBinary(NiStream& kStream)
{
    // The color values must be streamed separately to avoid endian swapping
    // issues.
    NiStreamSaveBinary(kStream, r());
    NiStreamSaveBinary(kStream, g());
    NiStreamSaveBinary(kStream, b());
    NiStreamSaveBinary(kStream, a());
}
//---------------------------------------------------------------------------
char* NiRGBA::GetViewerString(const char* pcPrefix) const
{
    size_t stLen = strlen(pcPrefix) + 24;
    char* pcString = NiAlloc(char, stLen);
    NiSprintf(pcString, stLen, "%s = (%3d, %3d, %3d, %3d)",
        pcPrefix, r(), g(), b(), a());
    return pcString;
}
//---------------------------------------------------------------------------

#endif
