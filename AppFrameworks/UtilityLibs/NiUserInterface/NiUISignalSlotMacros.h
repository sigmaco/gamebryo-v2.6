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

#ifndef NISIGNALSLOTMACROS_H
#define NISIGNALSLOTMACROS_H

// All of the macros in this header are used in NiUISignal* and/or 
// NiUIBaseSlot*.  Since they have the same functionality, the macros remove 
// the necessity of excessive copy and paste.

//---------------------------------------------------------------------------
// This is called in destructors for Slots and Signals.
// NOTE: the unsubscribe function returns and calls (*this).Unsubscribe()
// which removes that person from my NiTSet.  That decreases the size of the
// set by one each iteration of the while loop.  Hence always referring to 
// GetAt(0).
#define NiUnsubscribeFromAll(NiTSetName)                    \
    {                                                       \
        unsigned int uiSize;                                \
        uiSize = NiTSetName.GetSize();                      \
        while (uiSize != 0)                                 \
        {                                                   \
            NiTSetName.GetAt(0)->Unsubscribe(this);         \
            uiSize = NiTSetName.GetSize();                  \
        }                                                   \
    }
//---------------------------------------------------------------------------
#define NiSubscribeToMe(NiTSetName, Element)                \
    {                                                       \
        int iIsThere = NiTSetName.Find(Element);            \
        if (iIsThere == -1)                                 \
        {                                                   \
            NiTSetName.Add(Element);                        \
            Element->Subscribe(this);                        \
        }                                                   \
    }
//---------------------------------------------------------------------------
#define NiUnsubscribeToMe(NiTSetName, Element)              \
    {                                                       \
        int iIsThere = NiTSetName.Find(Element);            \
        if (iIsThere != -1)                                 \
        {                                                   \
            NiTSetName.RemoveAt(iIsThere);                  \
            Element->Unsubscribe(this);                      \
        }                                                   \
    }
//---------------------------------------------------------------------------
// ArgList is actually a parenthesized list of arguments.
#define NiEmitSignalToAll(NiTSetName, ArgList)              \
    {                                                       \
        unsigned int uiIdx, uiSize;                         \
        uiSize = NiTSetName.GetSize();                      \
        for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)            \
        {                                                   \
            NiTSetName.GetAt(uiIdx)->ReceiveSignal ArgList; \
        }                                                   \
    }
//---------------------------------------------------------------------------
// Although these macros are very short, they were created so that if the
// system were to change in the future to use NiTArray or some other storage
// type, the only changes necessary would be in the declaration in the .h
// files and in this macro file.
#define NiNumElements(NiTSetName)                           \
    NiTSetName.GetSize();
//---------------------------------------------------------------------------
#define NiIsElementInGroup(NiTSetName, Element)             \
    (NiTSetName.Find(Element) != -1);
//---------------------------------------------------------------------------

#endif // NISIGNALSLOTMACROS_H
