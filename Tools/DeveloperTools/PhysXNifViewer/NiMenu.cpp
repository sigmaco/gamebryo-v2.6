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

#include "NiMenu.h"

#include <NiInputGamePad.h>
#include <NiMath.h>
#include <NiRTLib.h>
#include <NiSystem.h>

NiColorA gkBlue(0.0f, 0.0f, 1.0f, 0.5f);
NiColorA gkDark(0.0f, 0.0f, 0.0f, 0.25f);

// NiMenu::Entry

//---------------------------------------------------------------------------
NiMenu::Entry::Entry()
{
    m_bCheck = false;
    m_bEnable = true;
    m_uiMaxSubEntries = m_uiSubEntries = 0;
    m_pkSubEntries = NULL;
    m_bOpen = false;
    m_bDivide = false;
    m_uiCurrentSubEntry = 0;
}

//---------------------------------------------------------------------------
NiMenu::Entry::~Entry()
{
    NiDelete [] m_pkSubEntries;
}

//---------------------------------------------------------------------------
void NiMenu::Entry::AllocateSubEntries(unsigned int uiMaxEntries)
{
    NIASSERT(m_pkSubEntries == NULL);
    m_pkSubEntries = NiNew Entry[m_uiMaxSubEntries = uiMaxEntries];
    m_uiSubWidth = 0;
}

//---------------------------------------------------------------------------
NiMenu::Entry* NiMenu::Entry::AddSubEntry(const char* pcLabel,
    unsigned int uiCommand)
{
    NIASSERT(m_uiSubEntries < m_uiMaxSubEntries);
    Entry* pkEntry = &m_pkSubEntries[m_uiSubEntries++];
    pkEntry->SetLabel(pcLabel);
    m_uiSubWidth = NiMax((int) m_uiSubWidth, (int)strlen(pkEntry->GetLabel()) + 3);
    pkEntry->SetCommand(uiCommand);
    return pkEntry;
}

//---------------------------------------------------------------------------
NiMenu::Entry* NiMenu::Entry::GetCurrentEntry()
{
    if (m_bOpen)
        return m_pkSubEntries[m_uiCurrentSubEntry].GetCurrentEntry();
    return this;
}

//---------------------------------------------------------------------------
void NiMenu::Entry::Draw(NiConsole& kConsole, unsigned int uiX,
    unsigned int uiY, unsigned int uiWidth)
{
    NiColorA kGray(0.5f, 0.5f, 0.5f, 1.0f);

    // CodeWarrior won't compile this commented statement, so do it the hard
    // way.
    // pkConsole->SetColor(m_bEnable ? NiColorA::WHITE : kGray);

    if (m_bEnable)
        kConsole.SetColor(NiColorA::WHITE);
    else
        kConsole.SetColor(kGray);
    
    if (m_bCheck)
    {
        kConsole.AddString(uiX, uiY, "X");
    }
    
    kConsole.AddString(uiX + 2, uiY, m_acLabel);

    if (m_uiSubEntries > 0)
    {
        kConsole.AddString(uiX + uiWidth - 1, uiY, ">");
    }

    if (m_bDivide)
    {
        kConsole.AddLine(uiX + 2, uiY, uiWidth - 3, NiColorA::WHITE);
    }

    if (m_bOpen)
    {
        uiX += uiWidth;

        kConsole.AddRectangle(uiX, uiY, m_uiSubWidth, m_uiSubEntries,
            gkDark);

        for (unsigned int i = 0; i < m_uiSubEntries; i++)
        {
            if (i == m_uiCurrentSubEntry)
            {
                kConsole.AddRectangle(uiX, uiY + i, m_uiSubWidth, 1, gkBlue);
            }

            m_pkSubEntries[i].Draw(kConsole, uiX, uiY + i, m_uiSubWidth);
        }
    }
}

//---------------------------------------------------------------------------
bool NiMenu::Entry::ButtonPress(unsigned int uiNewButtons)
{
    if (m_bOpen)
    {
        m_pkSubEntries[m_uiCurrentSubEntry].ButtonPress(uiNewButtons);
    }

    if (uiNewButtons & NiInputGamePad::NIGP_MASK_LUP && m_bOpen) 
    {
        if (m_uiCurrentSubEntry > 0)
        {
            m_uiCurrentSubEntry--;
        }
        return true;
    }

    if (uiNewButtons & NiInputGamePad::NIGP_MASK_LDOWN && m_bOpen)
    {
        if (m_uiCurrentSubEntry + 1 < m_uiSubEntries)
        {
            m_uiCurrentSubEntry++;
        }
        return true;
    }

    if (uiNewButtons & NiInputGamePad::NIGP_MASK_LLEFT && m_bOpen)
    {
        m_bOpen = false;
        return true;
    }

    if (uiNewButtons & NiInputGamePad::NIGP_MASK_LRIGHT &&
        (!m_bOpen && m_uiSubEntries > 0))
    {
        m_bOpen = true;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiMenu::Entry::KeyPress(NiInputKeyboard* pkKeyboard)
{
    if (m_bOpen)
    {
        m_pkSubEntries[m_uiCurrentSubEntry].KeyPress(pkKeyboard);
    }

    if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_UP) && m_bOpen) 
    {
        if (m_uiCurrentSubEntry > 0)
        {
            m_uiCurrentSubEntry--;
        }
        return true;
    }

    if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_DOWN) && m_bOpen)
    {
        if (m_uiCurrentSubEntry + 1 < m_uiSubEntries)
        {
            m_uiCurrentSubEntry++;
        }
        return true;
    }

    if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_LEFT) && m_bOpen)
    {
        m_bOpen = false;
        return true;
    }

    if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_RIGHT) &&
        (!m_bOpen && m_uiSubEntries > 0))
    {
        m_bOpen = true;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
unsigned int NiMenu::Entry::GetHeight() const
{
    int iHeight = 1;

    for (unsigned int i = 0; i < m_uiSubEntries; i++)
    {
        iHeight = NiMax(iHeight, i + m_pkSubEntries[i].GetHeight());
    }

    return iHeight;
}

// NiMenu::Page

//---------------------------------------------------------------------------
NiMenu::Page::Page()
{
    m_pkEntries = NULL;
    m_uiMaxEntries = 0;
    m_uiEntries = 0;
    m_uiCurrentEntry = UINT_MAX;
    m_bActive = false;
    m_uiWidth = 0;
}

//---------------------------------------------------------------------------
NiMenu::Page::~Page()
{
    NiDelete [] m_pkEntries;
}

//---------------------------------------------------------------------------
void NiMenu::Page::AllocateEntries(unsigned int uiMaxEntries)
{
    m_pkEntries = NiNew Entry[uiMaxEntries];
    m_uiMaxEntries = uiMaxEntries;
}

//---------------------------------------------------------------------------
NiMenu::Entry* NiMenu::Page::AddEntry(const char* pcLabel,
    unsigned int uiCommand)
{
    NIASSERT(m_uiEntries < m_uiMaxEntries);
    Entry* pkEntry = &m_pkEntries[m_uiEntries++];
    pkEntry->SetLabel(pcLabel);
    pkEntry->SetCommand(uiCommand);

    // Use pkEntry->GetLabel() instead of pcLabel in case the label got
    // truncated.
    m_uiWidth = NiMax((int) m_uiWidth, (int)strlen(pkEntry->GetLabel()) + 3);
    return pkEntry;
}

//---------------------------------------------------------------------------
void NiMenu::Page::Draw(NiConsole& kConsole, bool bOpen, unsigned int uiX)
{
    unsigned int uiY = 0;
    unsigned int uiWidth = (unsigned int)strlen(GetLabel());
    kConsole.AddRectangle(uiX, uiY, uiWidth, 1, gkDark);

    if (bOpen && PageNameSelected())
    {
        kConsole.AddRectangle(uiX, uiY, uiWidth, 1, gkBlue);
    }

    kConsole.SetColor(NiColorA::WHITE);
    kConsole.AddString(uiX, uiY, m_acLabel);

    if (!bOpen)
        return;

    uiY++;
    kConsole.AddRectangle(uiX, uiY, m_uiWidth, m_uiEntries, gkDark);

    for (unsigned int i = 0; i < m_uiEntries; i++, uiY++)
    {
        if (i == m_uiCurrentEntry)
        {
            kConsole.AddRectangle(uiX, uiY, m_uiWidth, 1, gkBlue);
        }
    
        m_pkEntries[i].Draw(kConsole, uiX, uiY, m_uiWidth);
    }
}

//---------------------------------------------------------------------------
void NiMenu::Page::HandlePadLUp()
{
    if (m_uiCurrentEntry == 0)
        m_uiCurrentEntry = UINT_MAX;
    else if (m_uiCurrentEntry != UINT_MAX)
        m_uiCurrentEntry--;
}

//---------------------------------------------------------------------------
void NiMenu::Page::HandlePadLDown()
{
    if (m_uiCurrentEntry < m_uiEntries - 1)
        m_uiCurrentEntry++;
    else if (m_uiCurrentEntry == UINT_MAX)
        m_uiCurrentEntry = 0;
}

//---------------------------------------------------------------------------
NiMenu::Entry* NiMenu::Page::GetCurrentEntry() const
{
    if (m_uiCurrentEntry >= m_uiEntries)
        return NULL;

    return m_pkEntries[m_uiCurrentEntry].GetCurrentEntry();
}

//---------------------------------------------------------------------------
bool NiMenu::Page::ButtonPress(unsigned int uiNewButtons)
{
    bool bChanged = false;
    if (m_uiCurrentEntry < m_uiEntries)
    {
        Entry* pkEntry = &m_pkEntries[m_uiCurrentEntry];
        
        if (pkEntry->ButtonPress(uiNewButtons))
            return true;
    }
        
    if (uiNewButtons & NiInputGamePad::NIGP_MASK_LUP)
    {
        HandlePadLUp();
        bChanged = true;
    }
    if (uiNewButtons & NiInputGamePad::NIGP_MASK_LDOWN)
    {
        HandlePadLDown();
        bChanged = true;
    }
    return bChanged;
}
//---------------------------------------------------------------------------
bool NiMenu::Page::KeyPress(NiInputKeyboard* pkKeyboard)
{
    bool bChanged = false;
    if (m_uiCurrentEntry < m_uiEntries)
    {
        Entry* pkEntry = &m_pkEntries[m_uiCurrentEntry];
        
        if (pkEntry->KeyPress(pkKeyboard))
            return true;
    }

    if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_UP))
    {
        HandlePadLUp();
        bChanged = true;
    }
    if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_DOWN))
    {
        HandlePadLDown();
        bChanged = true;
    }
    return bChanged;
}
//---------------------------------------------------------------------------
unsigned int NiMenu::Page::GetHeight() const
{
    int iHeight = m_uiEntries;

    for (unsigned int i = 0; i < m_uiEntries; i++)
    {
        iHeight = NiMax(iHeight, i + m_pkEntries[i].GetHeight());
    }

    // + 1 accounts for Page label.
    return 1 + iHeight;
}

// NiMenu

//---------------------------------------------------------------------------
NiMenu::NiMenu(NiTexture* pkTexture, unsigned int uiMaxPages,
    unsigned int uiCharacterWidth, unsigned int uiCharacterHeight,
    unsigned int uiColumns, unsigned int uiPosX, unsigned int uiPosY,
    ExecuteCallback pfnExecute, void* pvCallbackData)
    : m_kConsole(pkTexture, uiCharacterWidth, uiCharacterHeight, uiColumns,
        uiPosX, uiPosY, '!', 'z')
{
    m_uiMaxPages = uiMaxPages;
    m_uiPages = m_uiCurrentPage = 0;
    m_pfnExecute = pfnExecute;
    m_pvCallbackData = pvCallbackData;
    NIASSERT(m_uiMaxPages > 0);
    m_pkPages = NiNew Page[m_uiMaxPages];
    m_bChanged = true;

    SetActive(false);
}

//---------------------------------------------------------------------------
NiMenu::~NiMenu()
{
    NiDelete [] m_pkPages;
}

//---------------------------------------------------------------------------
NiMenu::Page* NiMenu::AddPage(const char* pcLabel, unsigned int uiMaxEntries)
{
    NIASSERT(m_uiPages < m_uiMaxPages);
    Page* pkPage = &m_pkPages[m_uiPages++];
    pkPage->SetLabel(pcLabel);
    pkPage->AllocateEntries(uiMaxEntries);
    return pkPage;
}

//---------------------------------------------------------------------------
void NiMenu::Update()
{
    if (m_bChanged)
    {
        m_kConsole.Clear();
        unsigned int uiX = 0;
        
        for (unsigned int i = 0; i < m_uiPages; i++)
        {
            Page* pkPage = &m_pkPages[i];
            pkPage->Draw(m_kConsole, i == m_uiCurrentPage, uiX);
#if defined(_WII)
            uiX += (unsigned int)strlen(pkPage->GetLabel()) + 1;
#else
            uiX += (unsigned int)strlen(pkPage->GetLabel()) + 2;
#endif
        }

        m_bChanged = false;
    }
}

//---------------------------------------------------------------------------
void NiMenu::ButtonPress(unsigned int uiNewButtons, unsigned int)
{
    Page* pkPage = &m_pkPages[m_uiCurrentPage];

    m_bChanged = true;

    if (uiNewButtons & NiInputGamePad::NIGP_MASK_RDOWN)
    {
        Entry* pkEntry = pkPage->GetCurrentEntry();
        if (pkEntry != NULL && !pkEntry->HasSubEntries() &&
            pkEntry->GetEnable())
        {
            m_pfnExecute(pkEntry, m_pvCallbackData);
        }
        return;
    }

    if (pkPage->ButtonPress(uiNewButtons))
        return;

    if (uiNewButtons & NiInputGamePad::NIGP_MASK_LLEFT && 
        (m_uiCurrentPage > 0))
    {
        pkPage->SetActive(false);
        m_pkPages[--m_uiCurrentPage].SetActive(true);                
        return;
    }

    if (uiNewButtons & NiInputGamePad::NIGP_MASK_LRIGHT &&
        (m_uiCurrentPage + 1 < m_uiPages))
    {
        pkPage->SetActive(false);
        m_pkPages[++m_uiCurrentPage].SetActive(true);                
        return;
    }

    m_bChanged = false;

    return;
}
//---------------------------------------------------------------------------
void NiMenu::KeyPress(NiInputKeyboard* pkKeyboard)
{
    Page* pkPage = &m_pkPages[m_uiCurrentPage];

    m_bChanged = true;

    if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_RETURN))
    {
        Entry* pkEntry = pkPage->GetCurrentEntry();
        if (pkEntry != NULL && !pkEntry->HasSubEntries() &&
            pkEntry->GetEnable())
        {
            m_pfnExecute(pkEntry, m_pvCallbackData);
        }
        return;
    }

    if (pkPage->KeyPress(pkKeyboard))
        return;

    if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_LEFT) && 
        (m_uiCurrentPage > 0))
    {
        pkPage->SetActive(false);
        m_pkPages[--m_uiCurrentPage].SetActive(true);                
        return;
    }

    if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_RIGHT) &&
        (m_uiCurrentPage + 1 < m_uiPages))
    {
        pkPage->SetActive(false);
        m_pkPages[++m_uiCurrentPage].SetActive(true);                
        return;
    }

    m_bChanged = false;

    return;
}
//---------------------------------------------------------------------------
unsigned int NiMenu::GetHeight() const
{
    int iHeight = 0;

    for (unsigned int i = 0; i < m_uiPages; i++)
    {
        iHeight = NiMax(iHeight, m_pkPages[i].GetHeight());
    }

    return iHeight;
}

