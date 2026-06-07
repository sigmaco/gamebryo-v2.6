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

#include <stdlib.h>

#include <NiSystem.h>

#include "NiMeshProfileProcessor.h"
#include "NiMeshProfileStream.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#endif
#include <tinyxml.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED =
    "Copyright (c) 1996-2008 Emergent Game Technologies.";
//---------------------------------------------------------------------------

#include <NiVersion.h>
static char acGamebryoVersion[] NI_UNUSED =
    GAMEBRYO_MODULE_VERSION_STRING(NiMeshProfile);

#include "NiMeshProfile.h"

//---------------------------------------------------------------------------
NiMeshProfile::NiMeshProfile()
{
    Reset();
}
//---------------------------------------------------------------------------
NiMeshProfile::~NiMeshProfile()
{
    m_kFinalizeStreams.RemoveAll();
}
//---------------------------------------------------------------------------
void NiMeshProfile::Reset()
{
    m_strName = "Error";
    m_strDescription = "None";
    m_bRecomputeNormals = false;

    // Weed out the finalize streams.
    m_bKeepUnspecifiedFinalizeStreams = false;

    m_kFinalizeStreams.RemoveAll();
}
//---------------------------------------------------------------------------
void NiMeshProfile::GetData(const NiMeshProfileElement* pkParentElement,
    const NiTObjectArray <NiMeshProfileStreamPtr>& kStreams,
    const char* pcInput, const NiMeshProfileStream** ppkStream,
    const NiMeshProfileElement** ppkElement,
    const NiMeshProfileComponent** ppkComponent) const
{
    if (ppkStream)
        *ppkStream = NULL;
    if (ppkElement)
        *ppkElement = NULL;
    if (ppkComponent)
        *ppkComponent = NULL;

    NiFixedString kComponent;
    NiFixedString kElement;
    NiFixedString kStream;
    int iIndex;
    if (!NiMeshProfileComponent::ParseComponentLink(pcInput, &kStream, 
        &kElement, &iIndex, &kComponent))
        return;
    NIASSERT(kElement);
    NIASSERT(kComponent);

    if (iIndex < 0)
        iIndex = pkParentElement->GetIndex();

    NiMeshProfileComponent::Component eComponent = 
        NiMeshProfileComponent::PD_DISABLED;
    eComponent = NiMeshProfileComponent::GetComponentType(
        ((const char*)kComponent)[0]);


    for (NiUInt32 uiStream = 0; uiStream < kStreams.GetSize();
        uiStream++)
    {
        if (!kStream || kStreams[uiStream]->GetName()== kStream)
        {
            if (ppkStream)
                *ppkStream = kStreams[uiStream];

            for (NiUInt32 uiElement = 0; 
                uiElement < kStreams[uiStream]->GetElementCount();
                uiElement++)
            {
                const NiMeshProfileElement* pkElement = 
                    kStreams[uiStream]->GetElement(uiElement);

                if (pkElement->GetName() == kElement &&
                    pkElement->GetIndex() == iIndex)
                {
                    if (ppkElement)
                        *ppkElement = pkElement;

                    for (NiUInt32 uiComponent = 0; 
                        uiComponent < pkElement->GetComponentCount();
                        uiComponent++)
                    {
                        const NiMeshProfileComponent* pkComponent = 
                            pkElement->GetComponent(uiComponent);

                        if (pkComponent->GetType() == eComponent)
                        {
                            if (ppkComponent)
                                *ppkComponent = pkComponent;

                            return;
                        }
                    }

                    return;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiMeshProfile::GetFinalizeData(
    const NiMeshProfileElement* pkParentElement, 
    const char* pcInput, const NiMeshProfileStream** ppkStream,
    const NiMeshProfileElement** ppkElement,
    const NiMeshProfileComponent** ppkComponent) const
{
    GetData(pkParentElement, m_kFinalizeStreams, pcInput, ppkStream, 
        ppkElement, ppkComponent);
}
//---------------------------------------------------------------------------
bool NiMeshProfile::Load(const TiXmlElement* pkElement)
{
    const TiXmlAttribute * pkAttribute = pkElement->FirstAttribute();
    while (pkAttribute)
    {
        // Fill in the name
        if (NiStricmp(pkAttribute->Name(), "name") == 0)
        {
            m_strName = pkAttribute->Value();
        }
        // Fill in the description
        else if (NiStricmp(pkAttribute->Name(), "description") == 0)
        {
            m_strDescription = pkAttribute->Value();
        }
        // Fill in the KeepUnspecifiedFinalizeStreams
        else if (NiStricmp(pkAttribute->Name(),
            "keepUnspecifiedFinalizeStreams") == 0)
        {
            if (NiStricmp(pkAttribute->Value(), "true")==0)
                m_bKeepUnspecifiedFinalizeStreams = true;
            else 
                m_bKeepUnspecifiedFinalizeStreams = false;
        }
        // Check to see if we want to recompute normals.
        else if (NiStricmp(pkAttribute->Name(), "recomputenormals") == 0)
        {
            if (NiStricmp(pkAttribute->Value(), "true") == 0)
            {
                m_bRecomputeNormals = true;
            }
            else if (NiStricmp(pkAttribute->Value(), "false") == 0)
            {
                m_bRecomputeNormals = false;
            }
            else
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Expected \"true\" or \"false\". \"%s\" Row: %d. "
                    "Column: %d\n",
                        pkAttribute->Name(), pkAttribute->Row(), 
                        pkAttribute->Column());
            }
        }
        else
        {
            NiMeshProfileProcessor::GetErrorHandler().ReportError(
                "Unexpected attribute \"%s\" Row: %d. Column: %d\n",
                    pkAttribute->Name(), pkAttribute->Row(), 
                    pkAttribute->Column());
        }

        // Goto the next one
        pkAttribute = pkAttribute->Next();
    }

    const TiXmlElement * pkChildElement = pkElement->FirstChildElement();
    while (pkChildElement)
    {
        if (NiStricmp(pkChildElement->Value(), "FinalizeStream")==0)
        {
            NiMeshProfileStream* pkFinalizeStream = 
                NiNew NiMeshProfileStream;
            if (pkFinalizeStream->Load(pkChildElement))
            {
                m_kFinalizeStreams.Add(pkFinalizeStream);
            }
            else
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Failed to load finalize stream Row: %d. Column: %d\n",
                    pkChildElement->Row(), pkChildElement->Column());

                delete pkFinalizeStream;
            }
        }
        else
        {
            NiMeshProfileProcessor::GetErrorHandler().ReportError(
                "Invalid child for profile \"%s\" Row: %d. Column: %d\n",
                pkChildElement->Value(), pkChildElement->Row(), 
                pkChildElement->Column());
        }

        // Goto the next one
        pkChildElement = pkChildElement->NextSiblingElement();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMeshProfile::Validate()
{
    // We need to make sure all the finalize streams are valid.
    for (NiUInt32 ui = 0; ui < m_kFinalizeStreams.GetSize(); ui++)
    {
        if (!m_kFinalizeStreams[ui]->Validate())
        {
            m_kFinalizeStreams.RemoveAtAndFill(ui);
            ui--;
        }
    }

    // We need to make sure all the finalize streams are unique with in this 
    // scope.
    for (NiUInt32 ui = 0; ui < m_kFinalizeStreams.GetSize(); ui++)
    {
        for (NiUInt32 uj = ui+1; uj < m_kFinalizeStreams.GetSize(); uj++)
        {
            if (m_kFinalizeStreams[ui]->GetRenderer() ==
                m_kFinalizeStreams[uj]->GetRenderer() &&
                m_kFinalizeStreams[ui]->GetName() == 
                m_kFinalizeStreams[uj]->GetName())
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Duplicate finalize stream defined \"%s\".\n",
                    m_kFinalizeStreams[ui]->GetName());

                m_kFinalizeStreams.RemoveAtAndFill(uj);

                break;
            }
        }
    }

    // Make sure we have some streams
    if (m_kFinalizeStreams.GetSize() == 0)
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportWarning(
            "Inavlid profile found \"%s\". Contains "
            "%d finalize streams. Could be renderer specific profile"
            "\n", (const char*)m_strName, m_kFinalizeStreams.GetSize());

        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiMeshProfile::PrintDescription()
{
    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "\n-------Begin \"%s\" Profile Description-------\n", 
        (const char*) m_strName);

    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "\tThis profile contains %d finalize " 
        "streams.\n", m_kFinalizeStreams.GetSize());

    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "\n\t-------Begin Finalize Stream Description-------\n");

    for (NiUInt32 ui = 0; ui < m_kFinalizeStreams.GetSize(); ui++)
    {
        m_kFinalizeStreams[ui]->PrintDescription();
    }

    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "\n-------End \"%s\" Profile Description-------\n", 
        (const char*) m_strName);
}
//---------------------------------------------------------------------------
bool NiMeshProfile::Fixup(
    const NiTObjectArray <NiMeshProfileElementPtr>& kElements,
    const NiTObjectArray <NiMeshProfileStreamPtr>& kFinalizeStreams)
{
    // Finalizes turn
    for (unsigned int i = 0; i < m_kFinalizeStreams.GetSize(); i++)
    {
        // Fixup this finalize stream
        if (!m_kFinalizeStreams[i]->Fixup(kElements))
        {
            m_kFinalizeStreams.RemoveAtAndFill(i);
            i--;
            continue;
        }

        NiFixedString kRef = m_kFinalizeStreams[i]->GetRefStream();
        if (kRef)
        {
            bool bFound = false;

            // Find the external reference
            for (unsigned int iRef = 0; iRef < kFinalizeStreams.GetSize(); 
                iRef++)
            {
                if (kFinalizeStreams[iRef]->GetName() == kRef)
                {
                    MergeFinalizeStream(i, kFinalizeStreams[iRef]);
                    bFound = true;
                    break;
                }
            }

            if (!bFound)
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Referenced Finalize \"%s\" could not be found.\n", 
                    kRef);

                m_kFinalizeStreams.RemoveAtAndFill(i);
                i--;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMeshProfile::Link(
    const NiTObjectArray <NiMeshProfileElementPtr>& kElements)
{
    // All the links
    NiTObjectArray<NiString> kLinkNames;

    for (unsigned int i=0; i < m_kFinalizeStreams.GetSize(); i++)
    {
        NiTObjectArray<NiString> kLocalLinkNames;

        m_kFinalizeStreams[i]->GetLinks(kLocalLinkNames);

        // Add all the string links make sure they are not in the list
        for (unsigned int j=0; j < kLocalLinkNames.GetSize(); j++)
        {
            if (kLinkNames.Find(kLocalLinkNames[j]) == (unsigned int)~0)
            {
                kLinkNames.Add(kLocalLinkNames[j]);
            }
        }
    }

    // Find the preparation data by traversing the 
    // string from the component back.
    for (unsigned int i=0; i < kLinkNames.GetSize(); i++)
    {
        NiFixedString strElement;
        NiFixedString strStream;
        int iIndex;

        NiMeshProfileComponent::ParseComponentLink(kLinkNames[i], &strStream,
            &strElement, &iIndex, NULL);

        bool bFound = false;
        if (strElement)
        {
            int iCount = (iIndex < 0) ? 0 : iIndex;
            for (unsigned int j=0; j < kElements.GetSize(); j++)
            {
                if (kElements[j]->GetName() == strElement &&
                    (iIndex < 0 || kElements[j]->GetIndex() == iIndex))
                {
                    NiMeshProfileStream kStream;

                    // Check to see if we are not the default
                    if (iCount == 0)
                    {
                        kStream.SetName(kElements[j]->GetName());
                    }
                    else
                    {
                        NiString strName;
                        strName.Format("%s_%d", kElements[j]->GetName(), 
                            iCount);
                        kStream.SetName((const char*)strName);
                    }
                    iCount++;
                    
                    kStream.InsertElement(kElements[j]);

                    bFound = true;
                }
            }
        }
        else
        {
            // This means we have a literal value so make sure we set the found
            // flag.
            bFound = true;
        }

        if (!bFound)
        {
            NiMeshProfileProcessor::GetErrorHandler().ReportError(
                "Could not find link data \"%s\"\n", kLinkNames[i]);

            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------
void NiMeshProfile::InsertFinalizeStream(
    const NiMeshProfileStream* pkFinalizeStream, 
    NiSystemDesc::RendererID eRenderer)
{
    bool bFound = false;

    for (unsigned int i = 0; i < m_kFinalizeStreams.GetSize(); i++)
    {
        if (!m_kFinalizeStreams[i]->IsUnique(pkFinalizeStream))
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        NiMeshProfileStream* pNewFinalizeStream = NiNew NiMeshProfileStream;
        pNewFinalizeStream->Copy(pkFinalizeStream, eRenderer);
        m_kFinalizeStreams.Add(pNewFinalizeStream);
    }
}
//---------------------------------------------------------------------------
void NiMeshProfile::MergeFinalizeStream(unsigned int uiIndex, 
        const NiMeshProfileStream* pkFinalizeStream)
{
    m_kFinalizeStreams[uiIndex]->Merge(pkFinalizeStream);
}
//---------------------------------------------------------------------------
void NiMeshProfile::Copy(const NiMeshProfile* pkFrom, 
    const bool bCopyFinalizeStreams)
{
    m_strName = pkFrom->GetName();
    m_strDescription = pkFrom->GetDescription();
    m_bKeepUnspecifiedFinalizeStreams =
        pkFrom->GetKeepUnspecifiedFinalizeStreams();
    m_bRecomputeNormals = pkFrom->GetRecomputeNormals();

    if (bCopyFinalizeStreams)
    {
        for (unsigned int i=0; i < pkFrom->GetFinalizeStreamCount(); i++)
        {
            InsertFinalizeStream(pkFrom->GetFinalizeStream(i), 
                pkFrom->GetFinalizeStream(i)->GetRenderer());
        }
    }
}
//---------------------------------------------------------------------------
void NiMeshProfile::CopyToRenderer(NiSystemDesc::RendererID eRenderer, 
        const NiMeshProfile* pkInputMeshProfile)
{
    Copy(pkInputMeshProfile, false);

    if (eRenderer != NiSystemDesc::RENDERER_GENERIC)
    {
        for (unsigned int i=0; 
            i < pkInputMeshProfile->GetFinalizeStreamCount(); i++)
        {
            if (eRenderer == 
                pkInputMeshProfile->GetFinalizeStream(i)->GetRenderer())
            {
                InsertFinalizeStream(
                    pkInputMeshProfile->GetFinalizeStream(i), eRenderer);
            }
        }
    }

    for (unsigned int i=0; 
        i < pkInputMeshProfile->GetFinalizeStreamCount(); i++)
    {
        if (NiSystemDesc::RENDERER_GENERIC == 
            pkInputMeshProfile->GetFinalizeStream(i)->GetRenderer())
        {
            InsertFinalizeStream(
                pkInputMeshProfile->GetFinalizeStream(i), eRenderer);
        }
    }
}
//---------------------------------------------------------------------------



