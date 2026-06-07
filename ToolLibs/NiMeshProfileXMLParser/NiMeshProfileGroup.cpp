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

#include "NiMeshProfileGroup.h"
#include "NiMeshProfile.h"
#include "NiMeshProfileProcessor.h"

//---------------------------------------------------------------------------
NiMeshProfileGroup::NiMeshProfileGroup()
{
}
//---------------------------------------------------------------------------
NiMeshProfileGroup::~NiMeshProfileGroup()
{  
    for (NiUInt32 ui = 0; ui < MESHPROFILE_PARSER_NUM_RENDERERS; ui++)
    {
        m_akProfiles[ui].RemoveAll();
    }
    
}
//---------------------------------------------------------------------------
const NiMeshProfile* NiMeshProfileGroup::GetProfile(
    const NiFixedString& pProfileName, NiSystemDesc::RendererID eRenderer)
{
    for (NiUInt32 ui = 0; ui < m_akProfiles[eRenderer].GetSize(); ui++)
    {
        if (m_akProfiles[eRenderer][ui]->GetName() == pProfileName)
        {
            return m_akProfiles[eRenderer][ui];
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiMeshProfile* NiMeshProfileGroup::NewProfile(
    NiSystemDesc::RendererID eRenderer)
{
    NiMeshProfile* pkNewProfile = NiNew NiMeshProfile;
    m_akProfiles[eRenderer].Add(pkNewProfile);
    return pkNewProfile;
}
//---------------------------------------------------------------------------
bool NiMeshProfileGroup::Validate()
{
    bool bValid = true;
    for (NiUInt32 uiRenderer = 0; 
        uiRenderer < MESHPROFILE_PARSER_NUM_RENDERERS; uiRenderer++)
    {
        for (NiUInt32 ui = 0; ui < m_akProfiles[uiRenderer].GetSize(); ui++)
        {
            if (!m_akProfiles[uiRenderer][ui]->Validate())
            {
                m_akProfiles[uiRenderer].RemoveAtAndFill(ui);
                ui--;
            }
        }

        if (!m_akProfiles[uiRenderer].GetSize())
        {
            NiMeshProfileProcessor::GetErrorHandler().ReportError(
                "No profiles found in this group for renderer \"%s\".\n",
                NiSystemDesc::GetRendererString(
                    (NiSystemDesc::RendererID)uiRenderer));

            bValid = false;
            continue;
        }
    }

    return bValid;
}
//---------------------------------------------------------------------------
void NiMeshProfileGroup::PrintDescription()
{
    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "\n-------Begin Profile Group Description-------\n");

    for (NiUInt32 uiRenderer = 0; 
        uiRenderer < MESHPROFILE_PARSER_NUM_RENDERERS; uiRenderer++)
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
            "\tThis profile group contains %d profiles of renderer \"%s\".\n",
            m_akProfiles[uiRenderer].GetSize(),
            NiSystemDesc::GetRendererString(
            (NiSystemDesc::RendererID)uiRenderer));

        for (NiUInt32 ui = 0; ui < m_akProfiles[uiRenderer].GetSize(); ui++)
        {
            m_akProfiles[uiRenderer][ui]->PrintDescription();
        }
    }

    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "\n-------End Profile Group Description-------\n\n");
}
//---------------------------------------------------------------------------
