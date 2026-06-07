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

#include "NiMeshProfileElement.h"
#include "NiMeshProfileComponent.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#endif
#include <tinyxml.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

//---------------------------------------------------------------------------
NiMeshProfileElement::NiMeshProfileElement()
{
    Reset();
}
//---------------------------------------------------------------------------
NiMeshProfileElement::~NiMeshProfileElement()
{
    m_kComponents.RemoveAll(); 
}
//---------------------------------------------------------------------------
void NiMeshProfileElement::Reset()
{
    m_strName = "Error";
    m_eType = NiCommonSemantics::E__Invalid;
    m_uiIndex = 0;
    m_uiStrictFlags = 0;
    m_eConvertType = NiMeshProfileOperations::CT_TYPECLAMP;
    m_eConvertFormat = NiDataStreamElement::T_FLOAT32;
    m_eSpecialFormat = NiDataStreamElement::F_UNKNOWN; // not used.
    m_eRenderer = NiSystemDesc::RENDERER_GENERIC;
    m_eRequired = NOT_REQUIRED;
    m_kComponents.RemoveAll();  
    m_uiModifiedFlags = 0;
}
//---------------------------------------------------------------------------
void NiMeshProfileElement::FixupForRange(const int iIndex)
{
    // Set the index
    m_uiIndex = (NiUInt8)iIndex;

    // Fixup the component links
    for (NiUInt32 ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        NiString kLink = m_kComponents[ui]->GetLinkedComponent();
        int iInsertIndex = kLink.Find("[]");
        if (iInsertIndex >= 0)
        {
            kLink.Insert(NiString::FromInt(iIndex), iInsertIndex+1);
            m_kComponents[ui]->SetLinkedComponent(kLink);
        }
        else if (NiMeshProfileComponent::IsLiteral(kLink))
        {
            m_kComponents[ui]->SetLinkedComponent(kLink);
        }
        else
        {
            NiMeshProfileProcessor::GetErrorHandler().ReportError(
                "Could not find the \"[]\" to fixup \"%s\" element for the "
                "given range.\n", (const char*)m_strName);
        }
    }
}
//---------------------------------------------------------------------------
bool NiMeshProfileElement::Load(
    const TiXmlElement* pkElement, 
    int& iRangeMin, 
    int& iRangeMax)
{
    // Initialize return values
    iRangeMin = -1;
    iRangeMax = -1;

    const TiXmlAttribute * pkAttribute = pkElement->FirstAttribute();
    while (pkAttribute)
    {
        // Fill in the name
        if (NiStricmp(pkAttribute->Name(), "name") == 0)
        {
            // First check to see if we are using a common semantic.
            NiCommonSemantics::Enumeration eType =
                NiCommonSemantics::GetEnumeration(pkAttribute->Value());
            if (eType == NiCommonSemantics::E__Invalid)
            {
                m_strName = pkAttribute->Value();
                m_uiModifiedFlags |= MODIFIED_NAME;
            }
            else
            {
                m_eType = eType;
                m_uiModifiedFlags |= MODIFIED_TYPE;
            }
        }
        // Fill in the type
        else if (NiStricmp(pkAttribute->Name(), "type") == 0)
        {
            m_eType = NiCommonSemantics::GetEnumeration(pkAttribute->Value());
            if (m_eType != NiCommonSemantics::E__Invalid)
                m_uiModifiedFlags |= MODIFIED_TYPE;
            else
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid semantic name given \"%s\" Row: %d. Column: %d\n",
                    pkAttribute->Value(), pkAttribute->Row(), 
                    pkAttribute->Column());
            }
        }
        // Fill in the index
        else if (NiStricmp(pkAttribute->Name(), "index") == 0)
        {
            m_uiIndex = (NiUInt8)pkAttribute->IntValue();
            m_uiModifiedFlags |= MODIFIED_INDEX;
        }
        // Fill in the strict flag
        else if (NiStricmp(pkAttribute->Name(), "strict") == 0)
        {
            NiString strAccess = pkAttribute->Value();

            m_uiStrictFlags = 0;

            if (strAccess.Find("STRICT_FORMAT")!=0xffffffff)
            {
                m_uiStrictFlags |= NiMeshProfileProcessor::STRICT_FORMAT;
            }
            if (strAccess.Find("STRICT_INTERLEAVE")!=0xffffffff)
            {
                m_uiStrictFlags |= NiMeshProfileProcessor::STRICT_INTERLEAVE;
            }

            if (m_uiStrictFlags)
            {
                m_uiModifiedFlags |= MODIFIED_STRICTFLAGS;
            }
            else
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid strict flag given \"%s\" Row: %d. Column: %d\n",
                    pkAttribute->Value(), pkAttribute->Row(), 
                    pkAttribute->Column());
            }
        }
        // Fill in the convert format type
        else if (NiStricmp(pkAttribute->Name(), "converttype") == 0)
        {
            m_eConvertType = FromString(pkAttribute->Value());
            if (m_eConvertType != NiMeshProfileOperations::CT_COUNT)
                m_uiModifiedFlags |= MODIFIED_CONVERTTYPE;
            else
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid cast convert type given \"%s\" "
                    "Row: %d. Column: %d\n",
                    pkAttribute->Value(), 
                    pkAttribute->Row(), 
                    pkAttribute->Column());
            }
        }
        // Fill in the format
        else if (NiStricmp(pkAttribute->Name(), "convertformat") == 0)
        {
            m_eConvertFormat = 
                NiDataStreamElement::TypeFromString(pkAttribute->Value());
            if (m_eConvertFormat != NiDataStreamElement::T_UNKNOWN)
                m_uiModifiedFlags |= MODIFIED_CONVERTFORMAT;
            else
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid convert format given \"%s\" "
                    "Row: %d. Column: %d\n",
                    pkAttribute->Value(), 
                    pkAttribute->Row(), 
                    pkAttribute->Column());
            }
        }
        else if (NiStricmp(pkAttribute->Name(), "specialformat") == 0)
        {
            const char* pcFormat = pkAttribute->Value();
            if (strcmp(pcFormat,"UINT_10_10_10_2") == 0)
                m_eSpecialFormat = NiDataStreamElement::F_UINT_10_10_10_2;
            else if (strcmp(pcFormat,"UINT_10_10_10_L1") == 0)
                m_eSpecialFormat = NiDataStreamElement::F_UINT_10_10_10_L1;
            else if (strcmp(pcFormat, "NORMINT_10_10_10_L1") == 0)
                m_eSpecialFormat = NiDataStreamElement::F_NORMINT_10_10_10_L1;
            else if (strcmp(pcFormat,"NORMINT_10_10_10_2") == 0)
                m_eSpecialFormat = NiDataStreamElement::F_NORMINT_10_10_10_2;
            else if (strcmp(pcFormat,"NORMINT_11_11_10") == 0)
                m_eSpecialFormat = NiDataStreamElement::F_NORMINT_11_11_10;
            else if (strcmp(pcFormat,"NORMUINT8_4_BGRA") == 0)
                m_eSpecialFormat = NiDataStreamElement::F_NORMUINT8_4_BGRA;
            else
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid special format given \"%s\" "
                    "Row: %d. Column: %d\n",
                    pkAttribute->Value(), 
                    pkAttribute->Row(), 
                    pkAttribute->Column());
            }
            if (m_eSpecialFormat != NiDataStreamElement::F_UNKNOWN)
            {
                m_uiModifiedFlags |= MODIFIED_SPECIALFORMAT;
            }
        }

        // Fill in the renderer
        else if (NiStricmp(pkAttribute->Name(), "renderer") == 0)
        {
            m_eRenderer = NiSystemDesc::GetRendererID(pkAttribute->Value());
            if (m_eRenderer != NiSystemDesc::RENDERER_GENERIC)
            {
                m_uiModifiedFlags |= MODIFIED_RENDERER;
            }
            else
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid renderer name given \"%s\" "
                    "Row: %d. Column: %d\n",
                    pkAttribute->Value(), 
                    pkAttribute->Row(), 
                    pkAttribute->Column());
            }
        }
        // Fill in the required enumeration
        else if (NiStricmp(pkAttribute->Name(), "required") == 0)
        {
            if (NiStricmp("NOT_REQUIRED", pkAttribute->Value()) == 0)
            {
                m_eRequired = NOT_REQUIRED;
                m_uiModifiedFlags |= MODIFIED_REQUIRED;
            }
            else if (NiStricmp("REQUIRED_FAIL", pkAttribute->Value()) == 0)
            {
                m_eRequired = REQUIRED_FAIL;
                m_uiModifiedFlags |= MODIFIED_REQUIRED;
            }
            else
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid required type \"%s\" "
                    "Row: %d. Column: %d\n",
                    pkAttribute->Value(), 
                    pkAttribute->Row(), 
                    pkAttribute->Column());
            }
        }
        // Check to see if we have a range
        else if (NiStricmp(pkAttribute->Name(), "range") == 0)
        {
            NiString kRange = pkAttribute->Value();
            NiUInt32 uiLength = kRange.Length();
            NiUInt32 uiIndex = kRange.Find('-');
            if (uiIndex > 0 && uiIndex < uiLength)
            {
                kRange.GetSubstring(0, uiIndex).ToInt(iRangeMin);
                kRange.GetSubstring(uiIndex+1, uiLength).ToInt(iRangeMax);
            }
            else
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid range the correct format is \"??-??\" not \"%s\" "
                    "Row: %d. Column: %d\n",
                    pkAttribute->Value(), pkAttribute->Row(), 
                    pkAttribute->Column());
            }
        }
        else
        {
            NiMeshProfileProcessor::GetErrorHandler().ReportError(
                "Invalid attribute \"%s\" "
                "Row: %d. Column: %d\n",
                pkAttribute->Name(), 
                pkAttribute->Row(), 
                pkAttribute->Column());
        }

        // Goto the next one
        pkAttribute = pkAttribute->Next();
    }

    // If the name was not set but the type was set lets set the name to be
    // the type.
    if (!(m_uiModifiedFlags & MODIFIED_NAME) &&
        (m_uiModifiedFlags & MODIFIED_TYPE))
    {
        m_strName = GetSemanticName();
        m_uiModifiedFlags |= MODIFIED_NAME;
    }

    const TiXmlNode* pkChild = pkElement->FirstChild();
    while (pkChild)
    {
        if (pkChild->ToElement())
        {
            const TiXmlElement* pkChildElement = (const TiXmlElement*)pkChild;
            if (NiStricmp(pkChildElement->Value(), "Component") == 0)
            {
                NiMeshProfileComponent* pkComponent = 
                    NiNew NiMeshProfileComponent;
                if (pkComponent->Load(pkChildElement, 
                    (NiMeshProfileComponent::Component)
                    m_kComponents.GetSize()))
                {
                    m_kComponents.Add(pkComponent);
                }
            }
            else
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid child for element \"%s\" "
                    "Row: %d. Column: %d\n",
                    pkAttribute->Value(), 
                    pkAttribute->Row(), 
                    pkAttribute->Column());
            }
        }
        // Check to see if we are a reference
        else if (pkChild->ToText())
        {
            m_strRefElement = pkChild->Value();
            m_uiModifiedFlags |= MODIFIED_REFELEMENT;
        }
        else
        {
            NiMeshProfileProcessor::GetErrorHandler().ReportError(
                "Invalid child for element \"%s\" "
                "Row: %d. Column: %d\n",
                pkAttribute->Value(), 
                pkAttribute->Row(), 
                pkAttribute->Column());
        }

        // Goto the next one
        pkChild = pkChild->NextSibling();
    }

    return Validate();
}
//---------------------------------------------------------------------------
bool NiMeshProfileElement::Validate()
{
    // Check the component count to see if it is too big
    if (m_kComponents.GetSize() > 4)
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportError(
            "Two many components \"%d\" found in element \"%s\" index"
            "\"%d\"\n", m_kComponents.GetSize(), GetSemanticName(), 
            GetIndex());

        return false;
    }

    // Make sure we have some components
    if (!m_kComponents.GetSize() && !m_strRefElement.Length())
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportError(
            "No components found in element \"%s\" index"
            "\"%d\"\n", GetSemanticName(), GetIndex());

        return false;
    }

    // Make sure we have defined a name this is important
    if (!(m_uiModifiedFlags & MODIFIED_NAME) && 
        !(m_uiModifiedFlags & MODIFIED_REFELEMENT))
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportError(
            "An element \"name\" attribute must be defined.\n");

        return false;
    }

    if ((m_uiModifiedFlags & MODIFIED_SPECIALFORMAT) &&
        ((m_uiModifiedFlags & MODIFIED_CONVERTFORMAT) ||
         (m_uiModifiedFlags & MODIFIED_CONVERTTYPE)))
    {
        // If special format is specified, warn that convertFromat and type
        // should not be used.
        NiMeshProfileProcessor::GetErrorHandler().ReportWarning(
            "\"specialformat\" should not be used with \"convertformat\" or " \
            "\"converttype\"\n");
    }

    // Make sure the convert format is specified when the type is
    if (!(m_uiModifiedFlags & MODIFIED_CONVERTFORMAT) && 
        (m_uiModifiedFlags & MODIFIED_CONVERTTYPE))
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportWarning(
            "\"converttype\" must be used with \"convertformat\"\n");
    }

    // Make sure we have at least one non-literal component
    bool bNonLiteral = false;
    // Referencing another element prevents us from having to make this check
    if (m_strRefElement.Length() > 0)
        bNonLiteral = true;
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if ((m_kComponents.GetAt(ui) != NULL) && 
            (!m_kComponents.GetAt(ui)->IsLiteral()))
        {
            bNonLiteral = true;
            break;
        }
    }
    if (!bNonLiteral)
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportError(
            "All components in element \"%s\" index \"%d\" have literal "
            "values. At least one component must reference a data stream. \n",
            GetSemanticName(), GetIndex());
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiMeshProfileElement::PrintDescription()
{
    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "\n\t\t-------Begin \"%s\" Element Description-------\n", 
        (const char*) m_strName);

    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "\t\t\tThis element contains %d components.\n"
        "\t\t\tIt uses the %s renderer.\n", 
        m_kComponents.GetSize(), 
        NiSystemDesc::GetRendererString(m_eRenderer));

    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "\t\t\tThis element's semantic name is %s and semantic index is %d\n", 
        GetSemanticName(), m_uiIndex);

    if (m_uiModifiedFlags & MODIFIED_STRICTFLAGS)
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
            "\t\t\tThis element's strict flags are set to %x.\n", 
            m_uiStrictFlags);
    }

    if (m_uiModifiedFlags & MODIFIED_REQUIRED)
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
            "\t\t\tThis element's required code is %d.\n", m_eRequired);
    }

    if (m_uiModifiedFlags & MODIFIED_SPECIALFORMAT)
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
            "\t\t\tThis element will be converted to a special format.\n");
    }
    else if (m_uiModifiedFlags & MODIFIED_CONVERTFORMAT)
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
            "\t\t\tThis element will be converted to the %s format type.\n"
            "\t\t\tThis conversion will been done with the %s type.\n",
            NiDataStreamElement::GetTypeString(m_eConvertFormat), 
            ToString(m_eConvertType));
    }

    for (NiUInt32 ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        m_kComponents[ui]->PrintDescription();
    }
}
//---------------------------------------------------------------------------
void NiMeshProfileElement::InsertComponent(
    const NiMeshProfileComponent* pkComponent)
{
    bool bFound = false;

    for (unsigned int i = 0; i < m_kComponents.GetSize(); i++)
    {
        if (pkComponent->GetType() == m_kComponents[i]->GetType())
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        NiMeshProfileComponent* pNewComponent = 
            NiNew NiMeshProfileComponent;
        pNewComponent->Copy(pkComponent);
        m_kComponents.Add(pNewComponent);
    }
}
//---------------------------------------------------------------------------
void NiMeshProfileElement::Merge(const NiMeshProfileElement* pkFrom)
{
    if (!(m_uiModifiedFlags & MODIFIED_NAME))
        m_strName = pkFrom->GetName();

    if (!(m_uiModifiedFlags & MODIFIED_TYPE))
        m_eType = pkFrom->GetType();

    if (!(m_uiModifiedFlags & MODIFIED_INDEX))
        m_uiIndex = pkFrom->GetIndex();

    if (!(m_uiModifiedFlags & MODIFIED_STRICTFLAGS))
        m_uiStrictFlags = pkFrom->GetStrictFlags();

    if (!(m_uiModifiedFlags & MODIFIED_CONVERTTYPE))
        m_eConvertType = pkFrom->GetConvertType();

    if (!(m_uiModifiedFlags & MODIFIED_SPECIALFORMAT))
        m_eSpecialFormat = pkFrom->GetSpecialFormat();

    if (!(m_uiModifiedFlags & MODIFIED_CONVERTFORMAT))
        m_eConvertFormat = pkFrom->GetConvertFormat();

    if (!(m_uiModifiedFlags & MODIFIED_RENDERER))
        m_eRenderer = pkFrom->GetRenderer();

    if (!(m_uiModifiedFlags & MODIFIED_REQUIRED))
        m_eRequired = pkFrom->GetRequired();

    // Do not call the accessor in this case.
    if (!(m_uiModifiedFlags & MODIFIED_REFELEMENT))
        m_strRefElement = pkFrom->m_strRefElement;

    for (unsigned int i = 0; i < pkFrom->GetComponentCount(); i++)
    {
        InsertComponent(pkFrom->GetComponent(i));
    }
}
//---------------------------------------------------------------------------
NiFixedString NiMeshProfileElement::ToString(
    NiMeshProfileOperations::CastConvertType eType)
{
    switch(eType)
    {
        case NiMeshProfileOperations::CT_SMARTCLAMP:
            return NiFixedString("SMARTCLAMP");
        case NiMeshProfileOperations::CT_NORMALIZE:
            return NiFixedString("NORMALIZE");
        case NiMeshProfileOperations::CT_TYPECLAMP:
            return NiFixedString("TYPECLAMP");
        case NiMeshProfileOperations::CT_BLINDCAST:
            return NiFixedString("BLINDCAST");
        default:
            return NiFixedString("Invalid");
    }
}
//---------------------------------------------------------------------------
NiMeshProfileOperations::CastConvertType NiMeshProfileElement::FromString(
    const NiFixedString& kName)
{
    for (int i = 0; i < NiMeshProfileOperations::CT_COUNT; i++)
    {
        if (kName == ToString((NiMeshProfileOperations::CastConvertType)i))
            return (NiMeshProfileOperations::CastConvertType)i;
    }

    return NiMeshProfileOperations::CT_COUNT;
}
//---------------------------------------------------------------------------
void NiMeshProfileElement::Copy(const NiMeshProfileElement* pkFrom, 
    NiSystemDesc::RendererID eRenderer)
{
    m_strName = pkFrom->GetName();
    m_eType = pkFrom->GetType();
    m_uiIndex = pkFrom->GetIndex();
    m_uiStrictFlags = pkFrom->GetStrictFlags();
    m_eConvertType = pkFrom->GetConvertType();
    m_eConvertFormat = pkFrom->GetConvertFormat();
    m_eSpecialFormat = pkFrom->GetSpecialFormat();
    m_eRenderer = eRenderer;
    m_eRequired = pkFrom->GetRequired();
    m_strRefElement = pkFrom->m_strRefElement;
    m_uiModifiedFlags = pkFrom->m_uiModifiedFlags;

    for (unsigned int i = 0; i < pkFrom->GetComponentCount(); i++)
    {
        InsertComponent(pkFrom->GetComponent(i));
    }
}
//---------------------------------------------------------------------------
void NiMeshProfileElement::Copy(const NiMeshProfileElement* pkFrom)
{
    Copy(pkFrom, pkFrom->GetRenderer());
}
//---------------------------------------------------------------------------
