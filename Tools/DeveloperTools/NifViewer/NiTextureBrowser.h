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

#ifndef NITEXTUREBROWSER_H
#define NITEXTUREBROWSER_H

#include "NiConsole.h"

#include <NiAVObject.h>
#include <NiMaterialProperty.h>
#include <NiTexturingProperty.h>
#include <NiTextureEffect.h>
#include <NiColor.h>
#include <NiViewRenderClick.h>

class NiTextureBrowser : public NiMemObject
{
public:
    class Entry : public NiMemObject
    {
    public:
        Entry() {}
        Entry(const NiMaterialProperty* pkMaterial,
            NiTexturingProperty* pkTexProp);
        
        const inline NiMaterialProperty* GetMaterial() const;
        inline NiTexturingProperty* GetTexturingProperty() const;
        inline NiTexturingProperty::Map* GetCurrentMap();
        
        void SwitchMap(int iAmt);

        void AddMapString(unsigned int x, unsigned int y, NiConsole& kConsole);
        void AddString(NiConsole& kConsole, unsigned int& x,unsigned int y, 
            const char* pcString, bool bSelected);
        
    protected:
        enum
        {
            MAX_DECALS = 3,
            MAX_MAPS = NiTexturingProperty::DECAL_BASE + MAX_DECALS
        };
        
        const NiMaterialProperty* m_pkMaterial;
        NiTexturingProperty* m_pkTexProp;
        unsigned int m_uiMap;
    };

    NiTextureBrowser(NiTexture* pkSourceTexture,
        unsigned int uiCharacterWidth, unsigned int uiCharacterHeight,
        unsigned int uiColumns, unsigned int uiPosX, unsigned int uiPosY,
        NiAVObject* pkScene);
    ~NiTextureBrowser();

    inline bool HasEntries() const;
    void Update();

    bool GetActive();
    void SetActive(bool bActive);

    void ButtonPress(unsigned int uiNewButtons, unsigned int uiButtons,
        bool& bExit);
    void HandlePadLUp();
    void HandlePadLDown();
    void HandlePadLeftRight(unsigned int uiButtons, int iMul);
    inline NiViewRenderClick* GetRenderClick() const;

protected:
    enum
    {
        MATERIAL_NAME,
        MAP,
        TEXTURE_NAME,
        SIZE,
        FILTER,
        EXIT,
        MODIFIER_COUNT
    };

    void CreateTextureArray(NiAVObject* pkScene);
    void CreateTextureArray(NiTObjectSet<Entry>& kEntries,
        NiAVObject* pkObject, const NiMaterialProperty* pkMaterial);
    void ChangeCurrentEntry(int iDelta);

    static int CompareEntryTexProp(const void* pvEntry0,
        const void* pvEntry1);

    NiConsole m_kConsole;
    unsigned int m_uiCurrentModifier;
    Entry* m_pkEntries;
    unsigned int m_uiEntries;
    unsigned int m_uiCurrentEntry;
    float m_afRepeatTime[MODIFIER_COUNT];
    bool m_abSelectable[MODIFIER_COUNT];
};

//---------------------------------------------------------------------------
inline bool NiTextureBrowser::GetActive()
{
    return m_kConsole.GetRenderClick()->GetActive();
}

//---------------------------------------------------------------------------
inline void NiTextureBrowser::SetActive(bool bActive)
{
    // It's only safe to alter the render click if there are entries or if
    // we're setting it to false. Otherwise, the browser will assert
    // internally on having zero textures or on input operations.
    if (!bActive || HasEntries())
        m_kConsole.GetRenderClick()->SetActive(bActive);
    m_uiCurrentModifier = 0;
}

//---------------------------------------------------------------------------
inline bool NiTextureBrowser::HasEntries() const
{
    return m_uiEntries > 0;
}

// NiTextureBrowser::Entry

//---------------------------------------------------------------------------
inline const NiMaterialProperty* NiTextureBrowser::Entry::GetMaterial() const
{
    return m_pkMaterial;
}

//---------------------------------------------------------------------------
inline NiTexturingProperty* NiTextureBrowser::Entry::GetTexturingProperty()
    const
{
    return m_pkTexProp;
}

//---------------------------------------------------------------------------
inline NiTexturingProperty::Map* NiTextureBrowser::Entry::GetCurrentMap()
{
    return m_pkTexProp->GetMaps().GetAt(m_uiMap);
}

//---------------------------------------------------------------------------
inline NiViewRenderClick* NiTextureBrowser::GetRenderClick() const
{
    return m_kConsole.GetRenderClick();
}

#endif  // NITEXTUREBROWSER_H
