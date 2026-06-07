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
#include <NiMeshProfileProcessor.h>

#include "NiMeshProfileComponent.h"
#include "NiMeshProfileElement.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#endif
#include <tinyxml.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

//---------------------------------------------------------------------------
NiMeshProfileComponent::NiRemap::NiRemap()
{
    // By default the source and dest are the same so we do nothing
    m_dSrcStart = 0.0; 
    m_dSrcEnd = 1.0; 
    m_dDestStart = 0.0; 
    m_dDestEnd = 1.0;
    m_bIsValueSet = false;
}
//---------------------------------------------------------------------------
bool NiMeshProfileComponent::NiRemap::SetValue(const char* pValue)
{
    // By default the source and dest are the same so we do nothing
    m_dSrcStart=0.0; 
    m_dSrcEnd=1.0; 
    m_dDestStart=0.0; 
    m_dDestEnd=1.0;

    const char* pData = NULL;

    pData = strstr(pValue, "src-a");
    if (pData)
        m_dSrcStart = atof(&pData[6]);
    else
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportError(
            "Remap parsing failed to find \"src-a\"\n");

        return false;
    }

    pData = strstr(pValue, "src-b");
    if (pData)
        m_dSrcEnd = atof(&pData[6]);
    else
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportError(
            "Remap parsing failed to find \"src-b\"\n");

        return false;
    }

    pData = strstr(pValue, "dst-a");
    if (pData)
        m_dDestStart = atof(&pData[6]);
    else
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportError(
            "Remap parsing failed to find \"dest-a\"\n");

        return false;
    }

    pData = strstr(pValue, "dst-b");
    if (pData)
        m_dDestEnd = atof(&pData[6]);
    else
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportError(
            "Remap parsing failed to find \"dest-b\"\n");

        return false;
    }

    m_bIsValueSet = true;

    return true;

}
//---------------------------------------------------------------------------
NiMeshProfileComponent::NiClamp::NiClamp()
{
    // By default no clamping
    m_dMin = -FLT_MAX;
    m_dMax = FLT_MAX;
    m_bIsValueSet = false;
}
//---------------------------------------------------------------------------
bool NiMeshProfileComponent::NiClamp::SetValue(const char* pValue)
{
    // By default no clamping
    m_dMin=-FLT_MAX;
    m_dMax=FLT_MAX;

    const char* pData = NULL;

    pData = strstr(pValue, "min");
    if (pData)
        m_dMin = atof(&pData[4]);
    else
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportError(
            "Clamp parsing failed to find \"min\"\n");

        return false;
    }

    pData = strstr(pValue, "max");
    if (pData)
        m_dMax = atof(&pData[4]);
    else
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportError(
            "Clamp parsing failed to find \"max\"\n");

        return false;
    }

    m_bIsValueSet = true;

    return true;
}
//---------------------------------------------------------------------------
NiMeshProfileComponent::NiMeshProfileComponent()
{
    Reset();
}
//---------------------------------------------------------------------------
NiMeshProfileComponent::~NiMeshProfileComponent()
{
}
//---------------------------------------------------------------------------
void NiMeshProfileComponent::Reset()
{
    m_eType = NiMeshProfileComponent::PD_DISABLED;
    m_strLinkedComponent = "None";
}
//---------------------------------------------------------------------------
bool NiMeshProfileComponent::Load(const TiXmlElement* pkElement,
    const Component eComponent)
{
    // Set the type
    m_eType = eComponent;

    const TiXmlAttribute * pkAttribute = pkElement->FirstAttribute();
    while (pkAttribute)
    {
        // Fill in the value
        if (NiStricmp(pkAttribute->Name(), "value") ==0 )
        {
            m_strLinkedComponent = pkAttribute->Value();
        }
        // Fill in the remap
        else if (NiStricmp(pkAttribute->Name(), "remap") == 0)
        {
            if (!m_kRemap.SetValue(pkAttribute->Value()))
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid remap \"%s\" Row: %d. Column: %d\n",
                    pkAttribute->Name(), pkAttribute->Row(), 
                    pkAttribute->Column());
            }
        }
        // Fill in the clamp
        else if (NiStricmp(pkAttribute->Name(), "clamp") == 0)
        {
            if (!m_kClamp.SetValue(pkAttribute->Value()))
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Invalid clamp \"%s\" Row: %d. Column: %d\n",
                    pkAttribute->Name(), pkAttribute->Row(), 
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

    return true;
}
//---------------------------------------------------------------------------
void NiMeshProfileComponent::PrintDescription()
{
    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "\n\t\t\t-------Begin \"%d\" Component Description-------\n", 
        (int)m_eType);

    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "\t\t\t\tThis component is linked to %s component\n", 
        (const char*)m_strLinkedComponent);

    if (m_kClamp.m_bIsValueSet)
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
            "\t\t\t\tThis component is clamp between %f and %f.\n", 
            m_kClamp.m_dMin, m_kClamp.m_dMax);
    }

    if (m_kRemap.m_bIsValueSet)
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
            "\t\t\t\tThis component will be remapped from %f and %f to %f and "
            "%f.\n", m_kRemap.m_dSrcStart, m_kRemap.m_dSrcEnd, 
            m_kRemap.m_dDestStart, m_kRemap.m_dDestEnd);
    }
}
//---------------------------------------------------------------------------
bool NiMeshProfileComponent::IsLiteral(const NiString& strLink)
{
    NiUInt32 uiLength = strLink.Length();
    for (NiUInt32 ui = 0; ui < uiLength; ui++)
    {
        char cTest = strLink.GetAt(ui);
        if (cTest != '.' && (cTest < '0' || cTest > '9'))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMeshProfileComponent::ParseComponentLink(const NiString& strLink,
    NiFixedString* pkStream, NiFixedString* pkElement, int* piIndex,
    NiFixedString* pkComponent)
{
    // Reset the return string and index
    if (pkComponent)
        *pkComponent = NULL;
    if (piIndex)
        *piIndex = 0;

    // Make sure this link is not a literal value
    if (IsLiteral(strLink))
        return false;

    // Parse the segments
    int iCount = strLink.Length();
    int iIndex = strLink.FindReverse('.', iCount-1);
    if (iIndex > 0)
    {
        if (pkComponent)
            *pkComponent = strLink.GetSubstring(iIndex + 1, iCount);

        // Now we have to find the element link
        return NiMeshProfileComponent::ParseElementLink(
            strLink.GetSubstring(0, iIndex), pkStream, pkElement, piIndex);
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiMeshProfileComponent::ParseElementLink(const NiString& strLink,
    NiFixedString* pkStream, NiFixedString* pkElement, int* piIndex)
{
    // Reset the return strings do not reset the index it should be set by
    // the calling function.
    if (pkStream)
        *pkStream = NULL;
    if (pkElement)
        *pkElement = NULL;

    // Make sure this link is not a literal value
    if (IsLiteral(strLink))
        return false;

    // Parse the segments
    int iCount = strLink.Length();
    int iIndex = strLink.FindReverse(']', iCount-1);
    if (iIndex > 0)
    {
        iCount = iIndex;
        iIndex = strLink.FindReverse('[', iCount-1);

        if (piIndex)
        {  
            // Check to see if we should inherit this index from the 
            // element
            if (iCount == (iIndex + 1))
            {
                *piIndex = -1;
            }
            else
            {
                strLink.GetSubstring(iIndex + 1, iCount).ToInt(*piIndex);
            }
        }

        iCount = iIndex;

        iIndex = strLink.FindReverse('.', iCount-1);
        if (iIndex > 0)
        {
            if (pkElement)
                *pkElement = strLink.GetSubstring(iIndex + 1, iCount);
            if (pkStream)
                *pkStream = strLink.GetSubstring(0, iIndex);
        }
        else
        {
            if (pkElement)
                *pkElement = strLink.GetSubstring(0, iCount);
        }
    }
    else
    {
        iIndex = strLink.FindReverse('.', iCount-1);
        if (iIndex > 0)
        {
            if (pkElement)
                *pkElement = strLink.GetSubstring(iIndex + 1, iCount);
            if (pkStream)
                *pkStream = strLink.GetSubstring(0, iIndex);
        }
        else
        {
            if (pkElement)
                *pkElement = strLink.GetSubstring(0, iCount);
        }
    }
    return true;
}
//---------------------------------------------------------------------------
void NiMeshProfileComponent::GetPrepData(
    const NiMeshProfileElement* pkParentElement,
    NiFixedString& kSemantic, int& iIndex, 
    Component& eComponent, double & dLiteral) const
{
    NiFixedString strComponent;
    NiFixedString strElement;
    bool bFound = 
        NiMeshProfileComponent::ParseComponentLink(m_strLinkedComponent, NULL, 
        &strElement, &iIndex, &strComponent);

    if (bFound)
    {
        NIASSERT(strComponent);
        eComponent = GetComponentType(((const char*)strComponent)[0]);
        if (iIndex < 0)
            iIndex = pkParentElement->GetIndex();
        kSemantic = (const char*)strElement;
    }
    else
    {
        kSemantic = NULL;
        float fLiteral = 0.0f;
        m_strLinkedComponent.ToFloat(fLiteral);
        dLiteral = (double)fLiteral;
    } 
}
//---------------------------------------------------------------------------
void NiMeshProfileComponent::Copy(const NiMeshProfileComponent* pkFrom)
{
    m_eType = pkFrom->GetType();
    m_strLinkedComponent = pkFrom->GetLinkedComponent();
    m_kRemap = pkFrom->GetRemap();
    m_kClamp = pkFrom->GetClamp();
}
//---------------------------------------------------------------------------
