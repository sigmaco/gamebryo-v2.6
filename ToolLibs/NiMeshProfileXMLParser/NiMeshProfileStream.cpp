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

#include "NiMeshProfileElement.h"
#include "NiMeshProfileProcessor.h"
#include "NiMeshProfileStream.h"
#include <NiSystem.h>
#include <stdlib.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#endif
#include <tinyxml.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

//---------------------------------------------------------------------------
NiMeshProfileStream::NiMeshProfileStream()
{
    Reset();
}
//---------------------------------------------------------------------------
NiMeshProfileStream::~NiMeshProfileStream()
{
    m_kElements.RemoveAll();
}
//---------------------------------------------------------------------------
void NiMeshProfileStream::GetLinks(NiTObjectArray<NiString>& kLinkNames)
{
    for (unsigned int i = 0; i < m_kElements.GetSize(); i++)
    {
        for (unsigned int j = 0; j < m_kElements[i]->GetComponentCount(); j++)
        {
            const char* pLinkName = 
                m_kElements[i]->GetComponent(j)->GetLinkedComponent();
            if (NiStricmp(pLinkName, "None") != 0)
            {
                kLinkNames.Add(pLinkName);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiMeshProfileStream::Reset()
{
    m_strName = "Error";
    m_eRenderer = NiSystemDesc::RENDERER_GENERIC;
    m_uiAccessFlags = 0;
    m_eUsage = NiDataStream::USAGE_MAX_TYPES;
    m_uiModifiedFlags = 0;

    m_kElements.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiMeshProfileStream::Load(const TiXmlElement *pkElement)
{
    const TiXmlAttribute * pkAttribute = pkElement->FirstAttribute();
    while (pkAttribute)
    {
        // Fill in the name
        if (NiStricmp(pkAttribute->Name(), "name")==0)
        {
            m_strName = pkAttribute->Value();
            m_uiModifiedFlags |= MODIFIED_NAME;
        }
        // Fill in the access
        else if (NiStricmp(pkAttribute->Name(), "access")==0)
        {
            NiString strAccess = pkAttribute->Value();
            
            m_uiAccessFlags = 0;
            
            if (strAccess.Find("CPU_READ")!=0xffffffff)
            {
                m_uiAccessFlags |= NiDataStream::ACCESS_CPU_READ;
            }
            if (strAccess.Find("GPU_READ")!=0xffffffff)
            {
                m_uiAccessFlags |= NiDataStream::ACCESS_GPU_READ;
            }
            if (strAccess.Find("CPU_WRITE_ANY")!=0xffffffff)
            {
                m_uiAccessFlags |= NiDataStream::ACCESS_CPU_WRITE_ANY;
            }
            if (strAccess.Find("CPU_WRITE_MUTABLE")!=0xffffffff)
            {
                m_uiAccessFlags |= NiDataStream::ACCESS_CPU_WRITE_MUTABLE;
            }
            if (strAccess.Find("CPU_WRITE_STATIC")!=0xffffffff)
            {
                m_uiAccessFlags |= NiDataStream::ACCESS_CPU_WRITE_STATIC;
            }
            if (strAccess.Find("CPU_WRITE_VOLATILE")!=0xffffffff)
            {
                m_uiAccessFlags |= NiDataStream::ACCESS_CPU_WRITE_VOLATILE;
            }
            if (strAccess.Find("GPU_WRITE")!=0xffffffff)
            {
                m_uiAccessFlags |= NiDataStream::ACCESS_GPU_WRITE;
            }

            // Make sure we specify a write access.
            if (!(m_uiAccessFlags & NiDataStream::ACCESS_CPU_WRITE_ANY))
            {
                m_uiAccessFlags |= NiDataStream::ACCESS_CPU_WRITE_STATIC;
            }

            // It is ok if m_eUsage is currently the MAX as
            // IsAccessRequestValid only checks against USAGE_USER
            if (m_uiAccessFlags && NiDataStream::IsAccessRequestValid(
                m_uiAccessFlags, m_eUsage))
            {
                m_uiModifiedFlags |= MODIFIED_ACCESSFLAGS;
            }
            else
            {
                m_uiAccessFlags = 0;

                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid access flags given \"%s\" Row: %d. Column: %d\n",
                    pkAttribute->Value(), pkAttribute->Row(), 
                    pkAttribute->Column());
            }
        } 
        // Fill in the usage
        else if (NiStricmp(pkAttribute->Name(), "usage")==0)
        {
            bool bError = true;

            if (NiStricmp("USAGE_VERTEX", pkAttribute->Value()) == 0)
            {  
                if (m_uiAccessFlags && !NiDataStream::IsAccessRequestValid(
                    m_uiAccessFlags, NiDataStream::USAGE_VERTEX))
                {
                    // Do nothing, let error be reported.
                }
                else
                {
                    m_eUsage = NiDataStream::USAGE_VERTEX;
                    m_uiModifiedFlags |= MODIFIED_USAGE;
                    bError = false;
                }
            }
            else if (
                NiStricmp("USAGE_VERTEX_INDEX", pkAttribute->Value()) == 0)
            {   
                if (m_uiAccessFlags && !NiDataStream::IsAccessRequestValid(
                    m_uiAccessFlags, NiDataStream::USAGE_VERTEX_INDEX))
                {
                    // Do nothing, let error be reported.
                }
                else
                {
                    m_eUsage = NiDataStream::USAGE_VERTEX_INDEX;
                    m_uiModifiedFlags |= MODIFIED_USAGE;
                    bError = false;
                }
            }
            else if (NiStricmp("USAGE_USER", pkAttribute->Value()) == 0)
            {
                if (m_uiAccessFlags && !NiDataStream::IsAccessRequestValid(
                    m_uiAccessFlags, NiDataStream::USAGE_USER))
                {
                    // Do nothing, let error be reported.
                }
                else
                {
                    m_eUsage = NiDataStream::USAGE_USER;
                    m_uiModifiedFlags |= MODIFIED_USAGE;
                    bError = false;
                }
            }

            if (bError)
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid usage type \"%s\" Row: %d. Column: %d\n",
                    pkAttribute->Value(), pkAttribute->Row(), 
                    pkAttribute->Column());
            }
        } 
        // Fill in the renderer
        else if (NiStricmp(pkAttribute->Name(), "renderer")==0)
        {
            m_eRenderer = NiSystemDesc::GetRendererID(pkAttribute->Value());
            if (m_eRenderer != NiSystemDesc::RENDERER_GENERIC)
            {
                m_uiModifiedFlags |= MODIFIED_RENDERER;
            }
            else
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid renderer name given \"%s\" Row: %d. Column: %d\n",
                    pkAttribute->Value(), pkAttribute->Row(), 
                    pkAttribute->Column());
            }
        }
        else
        {
            NiMeshProfileProcessor::GetErrorHandler().ReportError(
                "Invalid attribute \"%s\" Row: %d. Column: %d\n",
                    pkAttribute->Name(), pkAttribute->Row(), 
                    pkAttribute->Column());
        }

        // Goto the next one
        pkAttribute = pkAttribute->Next();
    }

    const TiXmlNode* pkChild = pkElement->FirstChild();
    while (pkChild)
    {
        if (pkChild->ToElement())
        {
            const TiXmlElement* pkChildElement = (const TiXmlElement*)pkChild;
            if (NiStricmp(pkChildElement->Value(), "Element")==0)
            {
                NiMeshProfileElement* pkElement = 
                    NiNew NiMeshProfileElement;
                int iRangeMin = -1;
                int iRangeMax = -1;
                if (pkElement->Load(pkChildElement, iRangeMin, iRangeMax))
                {
                    // Handle the ranges
                    if (iRangeMin >=0 && iRangeMax >=0 && 
                        iRangeMin < iRangeMax)
                    {
                        for (int i = iRangeMin; i <= iRangeMax; i++)
                        {
                            NiMeshProfileElement* pkNewElement = 
                                NiNew NiMeshProfileElement;
                            pkNewElement->Copy(pkElement);

                            pkNewElement->FixupForRange(i);
                            m_kElements.Add(pkNewElement);
                        }

                        // Destroy the base element
                        NiDelete pkElement;
                    }
                    else
                    {
                        m_kElements.Add(pkElement);
                    }
                }
                else
                {
                    NiMeshProfileProcessor::GetErrorHandler().ReportError(
                        "Failed to load element Row: %d. Column: %d\n",
                        pkChildElement->Row(), pkChildElement->Column());

                    NiDelete pkElement;
                }
            }
            else
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid child for stream \"%s\" Row: %d. Column: %d\n",
                    pkChildElement->Value(), pkChildElement->Row(), 
                    pkChildElement->Column());
            }
        }
        // Check to see if we are a reference
        else if (pkChild->ToText())
        {
            m_strRefStream = pkChild->Value();
            m_uiModifiedFlags |= MODIFIED_REFSTREAM;
        }
        else
        {
            NiMeshProfileProcessor::GetErrorHandler().ReportError(
                "Invalid child for stream \"%s\" Row: %d. Column: %d\n",
                pkChild->Value(), pkChild->Row(), pkChild->Column());
        }

        // Goto the next one
        pkChild = pkChild->NextSibling();
    }

    return Validate();
}
//---------------------------------------------------------------------------
bool NiMeshProfileStream::Validate()
{
    // We need to make sure all the elements are valid.
    for (NiUInt32 ui = 0; ui < m_kElements.GetSize(); ui++)
    {
        if (!m_kElements[ui]->Validate())
        {
            m_kElements.RemoveAtAndFill(ui);
            ui--;
        }
    }

    // We need to make sure all the elements are unique with in this scope.
    for (NiUInt32 ui = 0; ui < m_kElements.GetSize(); ui++)
    {
        for (NiUInt32 uj = ui+1; uj < m_kElements.GetSize(); uj++)
        {
            if (m_kElements[ui]->GetRenderer() ==
                m_kElements[uj]->GetRenderer() &&
                m_kElements[ui]->GetName() == 
                m_kElements[uj]->GetName() &&
                m_kElements[ui]->GetIndex() ==
                m_kElements[uj]->GetIndex() &&
                m_kElements[ui]->GetRefElement() ==
                m_kElements[uj]->GetRefElement())
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Duplicate \"name\" defined \"%s\" index \"%d\".\n",
                    m_kElements[ui]->GetName(), 
                    m_kElements[ui]->GetIndex());

                m_kElements.RemoveAtAndFill(uj);
                break;
            }
        }
    }

    // We need to make sure all the elements are unique with in this scope.
    for (NiUInt32 ui = 0; ui < m_kElements.GetSize(); ui++)
    {
        for (NiUInt32 uj = ui+1; uj < m_kElements.GetSize(); uj++)
        {
            if (m_kElements[ui]->GetRenderer() ==
                m_kElements[uj]->GetRenderer() &&
                m_kElements[ui]->GetSemanticName() == 
                m_kElements[uj]->GetSemanticName() &&
                m_kElements[ui]->GetIndex() ==
                m_kElements[uj]->GetIndex() &&
                m_kElements[ui]->GetRefElement() ==
                m_kElements[uj]->GetRefElement())
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Duplicate \"semantic\" defined \"%s\" index \"%d\".\n",
                    m_kElements[ui]->GetSemanticName(), 
                    m_kElements[ui]->GetIndex());

                m_kElements.RemoveAtAndFill(uj);
                break;
            }
        }
    }

    // Make sure we have some elements
    if (!m_kElements.GetSize())
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportWarning(
            "No elements found in stream \"%s\". Could be renderer specific\n",
            GetName());

        return false;
    }

    // Make sure the access mask and usage are valid
    if (m_uiAccessFlags)
    {
        // It is ok if m_eUsage == NiDataStream::USAGE_MAX_TYPES as
        // IsAccessRequestValid only checks for USAGE_USER.
        if (!NiDataStream::IsAccessRequestValid(m_uiAccessFlags, m_eUsage))
        {
            NiMeshProfileProcessor::GetErrorHandler().ReportError(
                "Invalid access flag \"%x\" with usage \"%d\".\n", 
                m_uiAccessFlags, (int)m_eUsage);

            return false;

        }
    }

    // Make sure we have defined a name for this stream
    if (!(m_uiModifiedFlags & MODIFIED_NAME) && 
        !(m_uiModifiedFlags & MODIFIED_REFSTREAM))
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportError(
            "A stream \"name\" attribute must be defined.\n");

        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMeshProfileStream::IsUnique(const NiMeshProfileStream* pkStream) const
{
    // First check the name make sure we are unique.
    if (m_strName == pkStream->GetName())
        return false;

    // Next make sure none of the elements have the same semantic name.
    for (NiUInt32 ui0 = 0; ui0 < m_kElements.GetSize(); ui0++)
    {
        for (NiUInt32 ui1 = 0; ui1 < pkStream->GetElementCount(); ui1++)
        {
            if (m_kElements[ui0]->GetSemanticName() == 
                pkStream->GetElement(ui1)->GetSemanticName() &&
                m_kElements[ui0]->GetIndex() ==
                pkStream->GetElement(ui1)->GetIndex())
            {
                // This is technically a problem and should be flagged as
                // such.
                NiMeshProfileProcessor::GetErrorHandler().ReportWarning(
                    "Conflicting streams %s and %s.  They both contain "
                    "semantic %s and index %d\n", 
                    m_strName, pkStream->GetName(), 
                    m_kElements[ui0]->GetSemanticName(), 
                    m_kElements[ui0]->GetIndex());
                return false;
            }   
        }
    }

    // Return unique.
    return true;
}
//---------------------------------------------------------------------------
void NiMeshProfileStream::PrintDescription()
{
    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "\n\t-------Begin \"%s\" Stream Description-------\n", 
        (const char*) m_strName);

    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "\t\tThis stream contains %d elements.\n"
        "\t\tIt uses the %s renderer.\n", 
        m_kElements.GetSize(), 
        NiSystemDesc::GetRendererString(m_eRenderer));

    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "\t\tThis stream's access flags are set to %x. And its usage is %d\n", 
        m_uiAccessFlags, (int)m_eUsage);

    for (NiUInt32 ui = 0; ui < m_kElements.GetSize(); ui++)
    {
        m_kElements[ui]->PrintDescription();
    }
}
//---------------------------------------------------------------------------
bool NiMeshProfileStream::Fixup(
    const NiTObjectArray <NiMeshProfileElementPtr>& kElements)
{
    for (unsigned int i = 0; i < m_kElements.GetSize(); i++)
    {
        const char* pcRef = m_kElements[i]->GetRefElement();
        if (pcRef)
        {
            bool bFound = false;
            NiFixedString kElement;
            int iIndex = -1;

            // Parse the element link
            NiString kLink = m_kElements[i]->GetRefElement();
            NiMeshProfileComponent::ParseElementLink(
                kLink, NULL, &kElement, &iIndex);

            // Find the externally referenced element.
            NiUInt32 uiCount = kElements.GetSize();
            for (NiUInt32 uiRef = 0; uiRef < uiCount; uiRef++)
            {
                if (kElements[uiRef]->GetName() == kElement)
                {
                    if (kElements[uiRef]->GetIndex() == iIndex ||
                        (iIndex < 0 && kElements[uiRef]->GetIndex() == 
                        m_kElements[i]->GetIndex()))
                    {
                        MergeElement(i, kElements[uiRef]);
                        bFound = true;
                    }
                }
            }

            if (!bFound)
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Referenced element \"%s\" could not be found.\n", 
                    m_kElements[i]->GetRefElement());

                m_kElements.RemoveAtAndFill(i);
                i--;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiMeshProfileStream::InsertElement(
    const NiMeshProfileElement* pkElement,
    NiSystemDesc::RendererID eRenderer)
{
    bool bFound = false;

    for (unsigned int i = 0; i < m_kElements.GetSize(); i++)
    {
        if (pkElement->GetSemanticName() == 
            m_kElements[i]->GetSemanticName() &&
            pkElement->GetIndex() == 
            m_kElements[i]->GetIndex())
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        NiMeshProfileElement* pNewElement = 
            NiNew NiMeshProfileElement;
        pNewElement->Copy(pkElement, eRenderer);
        m_kElements.Add(pNewElement);
    }
}
//---------------------------------------------------------------------------
void NiMeshProfileStream::MergeElement(unsigned int uiIndex, 
        const NiMeshProfileElement* pkElement)
{
    m_kElements[uiIndex]->Merge(pkElement);
}
//---------------------------------------------------------------------------
void NiMeshProfileStream::Merge(const NiMeshProfileStream* pkFrom)
{
    if (!(m_uiModifiedFlags & MODIFIED_NAME))
        m_strName = pkFrom->GetName();

    if (!(m_uiModifiedFlags & MODIFIED_RENDERER))
        m_eRenderer = pkFrom->GetRenderer();

    if (!(m_uiModifiedFlags & MODIFIED_ACCESSFLAGS))
        m_uiAccessFlags = pkFrom->GetAccessFlags();

    if (!(m_uiModifiedFlags & MODIFIED_USAGE))
        m_eUsage = pkFrom->GetUsage();

    // Do not call the accessor in this case
    if (!(m_uiModifiedFlags & MODIFIED_REFSTREAM))
        m_strRefStream = pkFrom->m_strRefStream;

    for (unsigned int i=0; i < pkFrom->GetElementCount(); i++)
    {
        InsertElement(pkFrom->GetElement(i), m_eRenderer);
    }
}
//---------------------------------------------------------------------------
void NiMeshProfileStream::Copy(const NiMeshProfileStream* pkFrom,
    NiSystemDesc::RendererID eRenderer)
{
    m_strName = pkFrom->GetName();
    m_eRenderer = eRenderer;
    m_uiAccessFlags = pkFrom->GetAccessFlags();
    m_eUsage = pkFrom->GetUsage();
    m_strRefStream = pkFrom->m_strRefStream;
    m_uiModifiedFlags = pkFrom->m_uiModifiedFlags;

    if (eRenderer != NiSystemDesc::RENDERER_GENERIC)
    {
        for (unsigned int i=0; i < pkFrom->GetElementCount(); i++)
        {
            if (eRenderer == pkFrom->GetElement(i)->GetRenderer())
            {
                InsertElement(pkFrom->GetElement(i), eRenderer);
            }
        }
    }

    for (unsigned int i=0; i < pkFrom->GetElementCount(); i++)
    {
        if (NiSystemDesc::RENDERER_GENERIC == 
            pkFrom->GetElement(i)->GetRenderer())
        {
            InsertElement(pkFrom->GetElement(i), eRenderer);
        }
    }
}
//---------------------------------------------------------------------------
void NiMeshProfileStream::Copy(const NiMeshProfileStream* pkFrom)
{
    Copy(pkFrom, pkFrom->GetRenderer());
}
//---------------------------------------------------------------------------
