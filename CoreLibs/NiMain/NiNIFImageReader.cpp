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

#include <NiFile.h>
#include <NiSystem.h>
#include "NiNIFImageReader.h"
#include "NiPixelData.h"
#include "NiPixelFormat.h"

//---------------------------------------------------------------------------
NiNIFImageReader::NiNIFImageReader()
{
    /* */
}
//---------------------------------------------------------------------------
NiNIFImageReader::~NiNIFImageReader()
{
    /* */
}
//---------------------------------------------------------------------------
bool NiNIFImageReader::CanReadFile(const char* pcFileExtension) const
{
    if(!NiStricmp(pcFileExtension, ".nif"))
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
NiPixelData* NiNIFImageReader::ReadFile(NiFile& kIst, NiPixelData*)
{
    // cannot use the read header function - we can only read the entire
    // data file in this case

    // Since the reader is usually owned by a static image converter class,
    // we lock for safety.
    m_kReadCriticalSection.Lock();

    // special load - NIF file must contain a single NiPixelData object
    m_kStream.RemoveAllObjects();
    m_kStream.Load(&kIst);

    if (m_kStream.GetObjectCount() != 1)
    {
        m_kReadCriticalSection.Unlock();
        return NULL;
    }

    NiObject* pkPixelData = m_kStream.GetObjectAt(0);
    if (!NiIsKindOf(NiPixelData, pkPixelData))
    {
        m_kReadCriticalSection.Unlock();
        return NULL;
    }

    m_kReadCriticalSection.Unlock();
    return (NiPixelData*)pkPixelData;
}
//---------------------------------------------------------------------------
bool NiNIFImageReader::ReadHeader(NiFile& kIst, 
    unsigned int& uiWidth, unsigned int& uiHeight, 
    NiPixelFormat& kFormat, bool& bMipmap, 
    unsigned int& uiFaces)
{
    // Read header is not cheaper than ReadFile in this case
    NiPixelDataPtr spData = ReadFile(kIst, NULL);

    if (spData)
    {
        uiWidth = spData->GetWidth();
        uiHeight = spData->GetHeight();
        kFormat = spData->GetPixelFormat();
        bMipmap = (spData->GetNumMipmapLevels() > 1) ? true : false;
        uiFaces = spData->GetNumFaces();

        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
