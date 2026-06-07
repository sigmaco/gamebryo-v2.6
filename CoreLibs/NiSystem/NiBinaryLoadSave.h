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

#ifndef NIBINARYLOADSAVE_H
#define NIBINARYLOADSAVE_H

#include "NiRTLib.h"
#include "NiBinaryStream.h"
#include "NiSystemLibType.h"

//---------------------------------------------------------------------------
// Inline load/save routines for endian-aware streams.
//---------------------------------------------------------------------------
template <class T>
void NiBinaryStreamLoad (NiBinaryStream& is, T* pValue, 
    unsigned int uiNumEls = 1)
{
    NIASSERT(uiNumEls > 0);
    unsigned int uiSize = sizeof(T);
    unsigned int uiBytesToRead = sizeof(T) * uiNumEls;
    NIVERIFYEQUALS(
        is.BinaryRead(pValue, uiBytesToRead, &uiSize, 1), 
        uiBytesToRead);
}

//---------------------------------------------------------------------------
inline void NiBinaryStreamLoad (NiBinaryStream& is, void* pvValue, 
    unsigned int uiNumEls, unsigned int* puiComponentSizes,
    unsigned int uiNumComponents = 1)
{
    NIASSERT(uiNumEls > 0);
    NIASSERT(puiComponentSizes != NULL);
    NIASSERT(uiNumComponents > 0);

    unsigned int uiBytesToRead = 0;
    {
        for (unsigned int uiComp = 0; uiComp < uiNumComponents; uiComp++)
        {
            unsigned int uiElSize = puiComponentSizes[uiComp];
            uiBytesToRead += uiElSize;
            NIASSERT(uiElSize == 1 || uiElSize == 2 || 
                uiElSize == 4 || uiElSize == 8);
        }
    }
    uiBytesToRead *= uiNumEls;
    NIASSERT(uiBytesToRead != 0);

    NIVERIFYEQUALS(
        is.BinaryRead(
            pvValue, uiBytesToRead, puiComponentSizes, uiNumComponents), 
        uiBytesToRead);
}

//---------------------------------------------------------------------------
template <class T>
void NiBinaryStreamLoadEnum (NiBinaryStream& is, T* pValue)
{
    int iDummy;
    unsigned int uiBytesToRead = sizeof(int);
    NIVERIFYEQUALS(        
        is.BinaryRead(&iDummy, uiBytesToRead, &uiBytesToRead, 1),
        uiBytesToRead);

    *pValue = (T)iDummy;
}

//---------------------------------------------------------------------------
template <class T>
void NiBinaryStreamSave (NiBinaryStream& os, const T* pValue,
    unsigned int uiNumEls = 1)
{
    NIASSERT(uiNumEls > 0);
    unsigned int uiSize = sizeof(T);
    unsigned int uiBytesToWrite = uiSize * uiNumEls;
    NIVERIFYEQUALS(
        os.BinaryWrite(pValue, uiBytesToWrite, &uiSize, 1),
        uiBytesToWrite);
}

//---------------------------------------------------------------------------
inline void NiBinaryStreamSave (NiBinaryStream& os, const void* pvValue, 
    unsigned int uiNumEls, unsigned int* puiComponentSizes,
    unsigned int uiNumComponents)
{
    NIASSERT(uiNumEls != 0);
    NIASSERT(puiComponentSizes != NULL);
    NIASSERT(uiNumComponents > 0);

    unsigned int uiBytesToWrite = 0;
    for (unsigned int uiComp = 0; uiComp < uiNumComponents; uiComp++)
    {
        unsigned int uiElSize = puiComponentSizes[uiComp];
        uiBytesToWrite += uiElSize;
        NIASSERT(uiElSize == 1 || uiElSize == 2 || 
            uiElSize == 4 || uiElSize == 8);
    }
    uiBytesToWrite *= uiNumEls;

    NIVERIFYEQUALS(
        os.BinaryWrite(
            pvValue, uiBytesToWrite, puiComponentSizes, uiNumComponents),
        uiBytesToWrite);
}

//---------------------------------------------------------------------------
template <class T>
void NiBinaryStreamSaveEnum (NiBinaryStream& os, const T& value)
{
    unsigned int uiBytesToWrite = sizeof(int);
    NIVERIFYEQUALS(
        os.BinaryWrite(&value, uiBytesToWrite, &uiBytesToWrite, 1),
        uiBytesToWrite);
}

#endif // NIBINARYLOADSAVE_H
