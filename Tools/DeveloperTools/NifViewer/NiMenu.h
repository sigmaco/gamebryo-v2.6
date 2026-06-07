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

#ifndef NIMENU_H
#define NIMENU_H

#include "NiConsole.h"
#include <NiColor.h>

class NiMenu : public NiMemObject
{
public:
    enum
    {
        LABEL_LEN = 32
    };

    class Entry : public NiMemObject
    {
    public:
        Entry();
        ~Entry();
        
        const char* GetLabel() const;
        void SetLabel(const char* pcLabel);
        unsigned int GetCommand() const;
        void SetCommand(unsigned int uiCommand);
        bool GetCheck() const;
        void SetCheck(bool bCheck);
        bool GetEnable() const;
        void SetEnable(bool bEnable);
        void SetDivider(bool bDivide);
        bool IsDivider() const;
        bool HasSubEntries() const;
        unsigned int GetHeight() const;
        
        void AllocateSubEntries(unsigned int uiMaxEntries);
        Entry* AddSubEntry(const char* pcLabel, unsigned int uiCommand);
        Entry* GetCurrentEntry();
        Entry* GetSubEntry(unsigned int uiEntry) const;
        
        void Draw(NiConsole& kConsole, unsigned int uiX, unsigned int uiY,
            unsigned int uiWidth);
        
        bool ButtonPress(unsigned int uiNewButtons);
        
    protected:
        char m_acLabel[LABEL_LEN];
        unsigned int m_uiCommand;
        bool m_bCheck;
        bool m_bEnable;
        bool m_bDivide;
        
        unsigned int m_uiMaxSubEntries;
        unsigned int m_uiSubEntries;
        Entry* m_pkSubEntries;
        unsigned int m_uiCurrentSubEntry;
        unsigned int m_uiSubWidth;
        bool m_bOpen;
    };

    class Page : public NiMemObject
    {
    public:
        Page();
        ~Page();
        
        void AllocateEntries(unsigned int uiMaxEntries);
        const char* GetLabel();
        void SetLabel(const char* pcLabel);
        bool GetActive();
        void SetActive(bool bActive);        
        Entry* AddEntry(const char* pcLabel, unsigned int uiCommand);
        Entry* GetCurrentEntry() const;
        unsigned int GetHeight() const;
        
        bool PageNameSelected() const;
        void Draw(NiConsole& kConsole, bool bOpen, unsigned int uiX);
        
        void HandlePadLUp();
        void HandlePadLDown();
        bool ButtonPress(unsigned int uiNewButtons);
        
    protected:        
        unsigned int m_uiCurrentEntry;
        Entry* m_pkEntries;
        unsigned int m_uiEntries;
        unsigned int m_uiMaxEntries;
        char m_acLabel[LABEL_LEN];
        bool m_bActive;
        unsigned int m_uiWidth;
    };

    typedef void (*ExecuteCallback)(Entry* pkEntry, void* pvData);

    NiMenu(NiTexture* pkTexture, unsigned int uiMaxPages,
        unsigned int uiCharacterWidth, unsigned int uiCharacterHeight,
        unsigned int uiColumns, unsigned int uiPosX, unsigned int uiPosY,
        ExecuteCallback pfnExecute, void* pvCallbackData);
    ~NiMenu();

    Page* AddPage(const char* pcLabel, unsigned int uiMaxEntries);
    void Update();
    bool GetActive();
    void SetActive(bool bActive);
    void ButtonPress(unsigned int uiNewButtons, unsigned int uiButtons);
    unsigned int GetHeight() const;
    void MarkAsChanged();

    NiViewRenderClick* GetRenderClick() const;

protected:
    NiConsole m_kConsole;
    Page* m_pkPages;
    unsigned int m_uiMaxPages;
    unsigned int m_uiPages;
    unsigned int m_uiCurrentPage;
    ExecuteCallback m_pfnExecute;
    void* m_pvCallbackData;
    bool m_bChanged;
};

//---------------------------------------------------------------------------
inline bool NiMenu::GetActive()
{   
    return m_kConsole.GetRenderClick()->GetActive();
}

//---------------------------------------------------------------------------
inline void NiMenu::SetActive(bool bActive)
{   
    m_kConsole.GetRenderClick()->SetActive(bActive);
}

//---------------------------------------------------------------------------
inline void NiMenu::MarkAsChanged()
{
    m_bChanged = true;
}

// NiMenu::Entry

//---------------------------------------------------------------------------
inline const char* NiMenu::Entry::GetLabel() const
{
    return m_acLabel;
}

//---------------------------------------------------------------------------
inline void NiMenu::Entry::SetLabel(const char* pcLabel)
{
    strncpy(m_acLabel, pcLabel, LABEL_LEN - 1);
    m_acLabel[LABEL_LEN - 1] = 0;
}

//---------------------------------------------------------------------------
inline unsigned int NiMenu::Entry::GetCommand() const
{
    return m_uiCommand;
}

//---------------------------------------------------------------------------
inline void NiMenu::Entry::SetCommand(unsigned int uiCommand)
{
    m_uiCommand = uiCommand;
}

//---------------------------------------------------------------------------
inline bool NiMenu::Entry::GetCheck() const
{
    return m_bCheck;
}

//---------------------------------------------------------------------------
inline void NiMenu::Entry::SetCheck(bool bCheck)
{
    m_bCheck = bCheck;
}

//---------------------------------------------------------------------------
inline bool NiMenu::Entry::GetEnable() const
{
    return m_bEnable;
}

//---------------------------------------------------------------------------
inline void NiMenu::Entry::SetEnable(bool bEnable)
{
    m_bEnable = bEnable;
}

//---------------------------------------------------------------------------
inline void NiMenu::Entry::SetDivider(bool bDivide)
{
    m_bDivide = bDivide;
}

//---------------------------------------------------------------------------
inline bool NiMenu::Entry::IsDivider() const
{
    return m_bDivide;
}

//---------------------------------------------------------------------------
inline bool NiMenu::Entry::HasSubEntries() const
{
    return m_uiSubEntries > 0;
}

//---------------------------------------------------------------------------
inline NiMenu::Entry* NiMenu::Entry::GetSubEntry(unsigned int uiEntry) const
{
    NIASSERT(uiEntry < m_uiSubEntries);
    return &m_pkSubEntries[uiEntry];
}

// NiMenu::Page

//---------------------------------------------------------------------------
inline const char* NiMenu::Page::GetLabel()
{
    return m_acLabel;
}

//---------------------------------------------------------------------------
inline void NiMenu::Page::SetLabel(const char* pcLabel)
{
    NIASSERT(pcLabel != NULL);
    strncpy(m_acLabel, pcLabel, LABEL_LEN - 1);
    m_acLabel[LABEL_LEN - 1] = 0;
}

//---------------------------------------------------------------------------
inline bool NiMenu::Page::GetActive()
{
    return m_bActive;
}

//---------------------------------------------------------------------------
inline void NiMenu::Page::SetActive(bool bActive)
{
    m_bActive = bActive;
}

//---------------------------------------------------------------------------
inline bool NiMenu::Page::PageNameSelected() const
{
    return m_uiCurrentEntry >= m_uiMaxEntries;
}

//---------------------------------------------------------------------------
inline NiViewRenderClick* NiMenu::GetRenderClick() const
{
    return m_kConsole.GetRenderClick();
}

#endif  // NIMENU_H
