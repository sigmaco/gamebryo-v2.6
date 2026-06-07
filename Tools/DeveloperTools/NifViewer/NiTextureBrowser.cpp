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

#include "NiTextureBrowser.h"
#include "NiConsole.h"

#include <NiInputGamePad.h>
#include <NiNode.h>
#include <NiPixelFormat.h>
#include <NiSourceTexture.h>

//---------------------------------------------------------------------------
//  NiTextureBrowser::Entry

NiTextureBrowser::Entry::Entry(const NiMaterialProperty* pkMaterial,
    NiTexturingProperty* pkTexProp)
{
    m_pkMaterial = pkMaterial;
    m_pkTexProp = pkTexProp;

    const NiTPrimitiveArray<NiTexturingProperty::Map*>& kMaps = 
        m_pkTexProp->GetMaps();

    for (m_uiMap = 0; m_uiMap < kMaps.GetSize(); m_uiMap++)
    {
        if (kMaps.GetAt(m_uiMap) != NULL)
        {
            break;
        }
    }

    // Make sure a map was found:
    NIASSERT(m_uiMap != kMaps.GetSize());
}

//---------------------------------------------------------------------------
void NiTextureBrowser::Entry::AddString(NiConsole& kConsole, unsigned int& x,
    unsigned int y, const char* pcString, bool bSelected)
{
    unsigned int uiLen = strlen(pcString);

    if (bSelected)
    {
        NiColorA kBlue(0.0f, 0.0f, 1.0f, 0.5f);
        kConsole.AddRectangle(x, y, uiLen, 1, kBlue);
    }

    kConsole.AddString(x, y, pcString);
    x += uiLen + 1;
}

//---------------------------------------------------------------------------
void NiTextureBrowser::Entry::AddMapString(unsigned int x, unsigned int y,
    NiConsole& kConsole)
{
    if (m_pkTexProp->GetBaseMap() != NULL)
    {
        AddString(kConsole, x, y, "BASE",
            m_uiMap == NiTexturingProperty::BASE_INDEX);
    }

    if (m_pkTexProp->GetDarkMap() != NULL)
    {
        AddString(kConsole, x, y, "DARK",
            m_uiMap == NiTexturingProperty::DARK_INDEX);
    }

    if (m_pkTexProp->GetDetailMap() != NULL)
    {
        AddString(kConsole, x, y, "DETAIL",
            m_uiMap == NiTexturingProperty::DETAIL_INDEX);
    }

    if (m_pkTexProp->GetGlossMap() != NULL)
    {
        AddString(kConsole, x, y, "GLOSS",
            m_uiMap == NiTexturingProperty::GLOSS_INDEX);
    }

    if (m_pkTexProp->GetGlowMap() != NULL)
    {
        AddString(kConsole, x, y, "GLOW",
            m_uiMap == NiTexturingProperty::GLOW_INDEX);
    }

    if (m_pkTexProp->GetNormalMap() != NULL)
    {
        AddString(kConsole, x, y, "NORMAL",
            m_uiMap == NiTexturingProperty::NORMAL_INDEX);
    }

    unsigned int uiMaps = m_pkTexProp->GetMaps().GetSize();
    for (unsigned int i = NiTexturingProperty::DECAL_BASE; i < uiMaps; i++)
    {
        if (m_pkTexProp->GetMaps().GetAt(i) != NULL)
        {
            char acDecalString[40];            
            NiSprintf(acDecalString, 40, "DECAL%d", i -
                NiTexturingProperty::DECAL_BASE);
            AddString(kConsole, x, y, acDecalString, m_uiMap == i);
        }
    }
}

//---------------------------------------------------------------------------
void NiTextureBrowser::Entry::SwitchMap(int iAmt)
{
    int iMap = m_uiMap;
    int iMaps = m_pkTexProp->GetMaps().GetSize();

    do
    {
        iMap += iAmt;

        if (iMap < 0)
            iMap = iMaps - 1;
        else if (iMap >= iMaps)
            iMap = 0;

        m_uiMap = iMap;

    } while (GetCurrentMap() == NULL);
}

//---------------------------------------------------------------------------
//  NiTextureBrowser

NiTextureBrowser::NiTextureBrowser(NiTexture* pkSourceTexture,
    unsigned int uiCharacterWidth, unsigned int uiCharacterHeight,
    unsigned int uiColumns, unsigned int uiPosX, unsigned int uiPosY,
    NiAVObject* pkScene)
    : m_kConsole(pkSourceTexture, uiCharacterWidth,
        uiCharacterHeight, uiColumns, uiPosX, uiPosY, '!', 'z',
        "NifViewer NiTextureBrowser Render Click")
{
    m_pkEntries = NULL;
    m_uiCurrentEntry = 0;
    m_uiCurrentModifier = 0;
    CreateTextureArray(pkScene);

    m_afRepeatTime[MATERIAL_NAME] = 0.15f;
    m_afRepeatTime[MAP] = 0.25f;

    m_abSelectable[MATERIAL_NAME] = true;
    m_abSelectable[TEXTURE_NAME] = false;
    m_abSelectable[MAP] = true;
    m_abSelectable[SIZE] = false;
    m_abSelectable[FILTER] = false;
    m_abSelectable[EXIT] = true;

    SetActive(false);
}

//---------------------------------------------------------------------------
NiTextureBrowser::~NiTextureBrowser()
{
    NiDelete [] m_pkEntries;
}

//---------------------------------------------------------------------------
int NiTextureBrowser::CompareEntryTexProp(const void* pvEntry0,
    const void* pvEntry1)
{
    const Entry* pkEntry0 = (const Entry*) pvEntry0;
    const Entry* pkEntry1 = (const Entry*) pvEntry1;

    if (pkEntry0->GetTexturingProperty() < pkEntry1->GetTexturingProperty())
        return -1;
    if (pkEntry0->GetTexturingProperty() == pkEntry1->GetTexturingProperty())
        return 0;
    return 1;
}

//---------------------------------------------------------------------------
void NiTextureBrowser::CreateTextureArray(NiAVObject* pkScene)
{
    NiTObjectSet<Entry> kEntries;

    // Gather all texture properties.

    CreateTextureArray(kEntries, pkScene, NiMaterialProperty::GetDefault());

    if (kEntries.GetSize() == 0)
    {
        m_pkEntries = NULL;
        m_uiEntries = 0;
        return;
    }

    // Sort.

    qsort(kEntries.GetBase(), kEntries.GetSize(), sizeof(kEntries.GetAt(0)),
        CompareEntryTexProp);

    // Count unique.

    NiTexturingProperty* pkPrev = NULL;
    unsigned int uiProps = kEntries.GetSize();
    m_uiEntries = 0;

    for (unsigned int i = 0; i < uiProps; i++)
    {
        NiTexturingProperty* pkTexProp = kEntries.GetAt(i).
            GetTexturingProperty();
        if (pkTexProp != pkPrev)
        {
            m_uiEntries++;
            pkPrev = pkTexProp;
        }
    }

    // Extract unique.

    m_pkEntries = NiNew Entry[m_uiEntries];
    NIASSERT(m_pkEntries != NULL);
    m_uiEntries = 0;
    pkPrev = NULL;

    for (unsigned int i = 0; i < uiProps; i++)
    {
        NiTexturingProperty* pkTexProp =
            kEntries.GetAt(i).GetTexturingProperty();
        if (pkTexProp != pkPrev)
        {
            m_pkEntries[m_uiEntries++] = kEntries.GetAt(i);
            pkPrev = pkTexProp;
        }
    }
}

//---------------------------------------------------------------------------
void NiTextureBrowser::CreateTextureArray(NiTObjectSet<Entry>& kEntries,
    NiAVObject* pkObject, const NiMaterialProperty* pkMaterial)
{
    NiMaterialProperty* pkNewMaterial = (NiMaterialProperty*) 
        pkObject->GetProperty(NiProperty::MATERIAL);

    if (pkNewMaterial != NULL)
    {
        pkMaterial = pkNewMaterial;
    }

    NiTexturingProperty* pkTexProp = (NiTexturingProperty*)
        pkObject->GetProperty(NiProperty::TEXTURING);

    if (pkTexProp)
    {
        // Only add the texture property if it has at least one non-NULL map.

        const NiTPrimitiveArray<NiTexturingProperty::Map*>& kMaps =
            pkTexProp->GetMaps();

        unsigned int uiMap;
        for (uiMap = 0; uiMap < kMaps.GetSize(); uiMap++)
        {
            if (kMaps.GetAt(uiMap) != NULL)
            {
                break;
            }
        }

        if (uiMap < kMaps.GetSize())
        {
            Entry kEntry(pkMaterial, pkTexProp);
            kEntries.Add(kEntry);
        }
    }                        
    
    if (NiIsKindOf(NiNode, pkObject))
    {
        const NiNode* pkNode = (const NiNode*) pkObject;
        for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild != NULL)
            {
                CreateTextureArray(kEntries, pkChild, pkMaterial);
            }
        }
    }
}

//---------------------------------------------------------------------------
void NiTextureBrowser::ChangeCurrentEntry(int iDelta)
{
    int iEntry = m_uiCurrentEntry + iDelta;
    int iObjects = m_uiEntries;

    if (iEntry < 0)
    {
        iEntry += iObjects;
    }
    else if (iEntry >= iObjects)
    {
        iEntry -= iObjects;
    }

    m_uiCurrentEntry = iEntry;

    NIASSERT(m_uiCurrentEntry < (unsigned int) iObjects);
}

//---------------------------------------------------------------------------
void NiTextureBrowser::Update()
{
    m_kConsole.Clear();
    
    NiColorA kDark(0.0f, 0.0f, 0.0f, 0.25f);
    enum
    {
        WIDTH = 36
    };

    m_kConsole.AddRectangle(0, 0, WIDTH, MODIFIER_COUNT + 1, kDark);

    NIASSERT(m_uiEntries > 0);

    char acString[256];
    unsigned int uiRow = 0;
    
    NiSprintf(acString, 256, "%d/%d", m_uiCurrentEntry + 1, m_uiEntries);
    m_kConsole.AddString(1, uiRow++, acString);
    
    Entry* pkEntry = &m_pkEntries[m_uiCurrentEntry];
    
    strcpy(acString, "Material: ");
    const NiMaterialProperty* pkMat = pkEntry->GetMaterial();
    const char* pcName = pkMat->GetName();
    strcat(acString, (pcName == NULL) ? "<noname>" : pcName);
    acString[WIDTH - 2] = 0; // truncate string
    m_kConsole.AddString(1, uiRow++, acString);

    pkEntry->AddMapString(1, uiRow++, m_kConsole);    

    const NiTexturingProperty::Map* pkMap = pkEntry->GetCurrentMap();
    const NiTexture* pkTexture = pkMap->GetTexture();

    const char* pcImageName;
    strcpy(acString, "  Image: ");
    if (NiIsKindOf(NiSourceTexture, pkTexture))
    {
        NiSourceTexture* pkSource = (NiSourceTexture*) pkTexture;
        pcImageName = pkSource->GetFilename();
    }
    else
    {
        pcImageName = NULL;
    }

    strcat(acString, (pcImageName == NULL) ? "<noname>" : pcImageName);
    acString[WIDTH - 2] = 0; // truncate    
    m_kConsole.AddString(1, uiRow++, acString);

    const NiPixelFormat* pkFormat = pkTexture->GetRendererData()->
        GetPixelFormat();
    NiSprintf(acString, 256, "  size: %d x %d x %d-bit", pkTexture->GetWidth(),
        pkTexture->GetHeight(), pkFormat->GetBitsPerPixel());

    m_kConsole.AddString(1, uiRow++, acString);

    const char* pcMip;
    switch (pkMap->GetFilterMode())
    {
    case NiTexturingProperty::FILTER_NEAREST:
        pcMip = "no (NEAREST)";
        break;
    case NiTexturingProperty::FILTER_BILERP:
        pcMip = "no (BILERP)";
        break;
    case NiTexturingProperty::FILTER_TRILERP:
        pcMip = "yes (TRILERP)";
        break;
    case NiTexturingProperty::FILTER_NEAREST_MIPNEAREST:
        pcMip = "yes (NEAREST_MIPNEAREST)";
        break;
    case NiTexturingProperty::FILTER_NEAREST_MIPLERP:
        pcMip = "yes (NEAREST_MIPLERP)";
        break;
    case NiTexturingProperty::FILTER_BILERP_MIPNEAREST:
        pcMip = "yes (BILERP_MIPNEAREST)";
        break;
    default:
        NIASSERT(0);
        pcMip = "error";
        break;
    }
    NiSprintf(acString, 256, "  mipmap: %s", pcMip);
    m_kConsole.AddString(1, uiRow++, acString);
    
    m_kConsole.AddString(1, uiRow++, "Exit Texture Browser");
    
    m_kConsole.AddString(0, 1 + m_uiCurrentModifier, ">");
}

//---------------------------------------------------------------------------
void NiTextureBrowser::ButtonPress(unsigned int uiNewButtons,
    unsigned int uiButtons, bool& bExit)
{
    bExit = false;
    
    if (uiNewButtons & NiInputGamePad::NIGP_MASK_LUP)
    {
        HandlePadLUp();
    }

    if (uiNewButtons & NiInputGamePad::NIGP_MASK_LDOWN)
    {
        HandlePadLDown();
    }

    if (uiButtons & NiInputGamePad::NIGP_MASK_LLEFT)
    {
        HandlePadLeftRight(uiButtons, -1);
    }

    if (uiButtons & NiInputGamePad::NIGP_MASK_LRIGHT)
    {
        HandlePadLeftRight(uiButtons, 1);
    }

    if (uiNewButtons & NiInputGamePad::NIGP_MASK_RDOWN)
    {
        if (m_uiCurrentModifier == EXIT)
        {
            bExit = true;
        }
    }
}

//---------------------------------------------------------------------------
void NiTextureBrowser::HandlePadLUp()
{
    // This loop assumes first entry is selectable. The assert checks that.

    if (m_uiCurrentModifier > 0)
    {
        do
        {
            NIASSERT(m_uiCurrentModifier > 0);
            m_uiCurrentModifier--;
        } while (!m_abSelectable[m_uiCurrentModifier]);
    }

    NIASSERT(m_uiCurrentModifier < MODIFIER_COUNT);
}

//---------------------------------------------------------------------------
void NiTextureBrowser::HandlePadLDown()
{
    // This loop assumes last entry is selectable. The assert checks that.

    if (m_uiCurrentModifier + 1 < MODIFIER_COUNT)
    {
        do
        {
            m_uiCurrentModifier++;
            NIASSERT(m_uiCurrentModifier < MODIFIER_COUNT);
        } while (!m_abSelectable[m_uiCurrentModifier]);
    }

    NIASSERT(m_uiCurrentModifier < MODIFIER_COUNT);
}

//---------------------------------------------------------------------------
void NiTextureBrowser::HandlePadLeftRight(unsigned int uiButtons, int iMul)
{
    static float sfLastTime = 0.0f;
    float fTime = NiGetCurrentTimeInSec();

    NIASSERT(m_uiCurrentModifier < MODIFIER_COUNT);

    if (m_uiCurrentModifier == EXIT)
        return;

    if (fTime - sfLastTime < m_afRepeatTime[m_uiCurrentModifier])
    {
        return;
    }

    sfLastTime = fTime;

    switch (m_uiCurrentModifier)
    {
    case MATERIAL_NAME:
        ChangeCurrentEntry(iMul);
        return;
    case MAP:
        m_pkEntries[m_uiCurrentEntry].SwitchMap(iMul);
        return;
    }
}

