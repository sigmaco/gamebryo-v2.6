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

#ifndef NIPALETTE_H
#define NIPALETTE_H

#include "NiObject.h"
#include "NiStream.h"

class NIMAIN_ENTRY NiPalette : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    class NIMAIN_ENTRY PaletteEntry : public NiMemObject
    {
    public:
        inline PaletteEntry() { /* */ }

        unsigned char m_ucRed;
        unsigned char m_ucGreen;
        unsigned char m_ucBlue;
        unsigned char m_ucAlpha;
    };

    NiPalette();
    NiPalette(bool bHasAlpha, unsigned int uiNumEntries);
    NiPalette(bool bHasAlpha, unsigned int uiNumEntries,
        const PaletteEntry* pkEntries);
    NiPalette(const NiPalette& kPal);
    virtual ~NiPalette();

    inline bool GetHasAlpha() const;
    inline unsigned int GetNumEntries() const;

    PaletteEntry* GetEntries();
    inline const PaletteEntry* GetEntries() const;

    NiPalette& operator=(const NiPalette& kPal);

    inline void MarkAsChanged();
    inline unsigned int GetRevisionID() const;

    /*** begin Emergent internal use only ***/
    
    class NIMAIN_ENTRY RendererData : public NiMemObject
    {
    public:
        RendererData(NiPalette* pkPalette);
        virtual ~RendererData();

    protected:
        NiPalette* m_pkPalette;
    };

    inline void SetRendererData(RendererData* pkRendererData);
    RendererData* GetRendererData() const;

    inline NiPalette* GetListNext();
    inline NiPalette* GetListPrev();

    static NiPalette* GetListHead();
    static NiPalette* GetListTail();

    /*** end Emergent internal use only ***/

protected:
    inline void AddPaletteToList();
    inline void RemovePaletteFromList();

    void CreateRendererData();

    bool m_bAlpha;
    unsigned int m_uiEntries;
    unsigned int m_uiRevID;
    PaletteEntry* m_pkPalette;
    RendererData* m_pkRendererData;

    // Doubly linked list traversal members for list of all textures
    NiPalette* m_pkPrev;
    NiPalette* m_pkNext;
    static NiPalette* ms_pkHead;
    static NiPalette* ms_pkTail;
    static NiCriticalSection ms_kPaletteListCriticalSection;
};

typedef NiPointer<NiPalette> NiPalettePtr;

#include "NiPalette.inl"

#endif
