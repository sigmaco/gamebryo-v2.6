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

#include "MayaPluginPCH.h"
#include "GetPackingProfiles.h"
#include "maya/MArgList.h"
#include <NiMeshProfileProcessor.h>
#include <NiMeshProfileXMLParser.h>

static const char* s_pcRenderSpecificPrefix = " - (";
static const NiUInt32 s_uiRenderSpecificPrefixCount = 4;

MString GetPackingProfiles::GetProfileName(const MString& sProfileName)
{
    MString sReturnProfileName = sProfileName;
    NiInt32 iLength = sProfileName.length() - s_uiRenderSpecificPrefixCount;
    const char* pcProfileName = sProfileName.asChar();
    for (NiInt32 i = 0; i < iLength; i++)
    {
        bool bFoundSubString = true;
        for (NiUInt32 uiSubString = 0; 
            uiSubString < s_uiRenderSpecificPrefixCount; uiSubString++)
        {
            if (pcProfileName[i] != s_pcRenderSpecificPrefix[uiSubString])
            {
                bFoundSubString = false;
                break;
            }

            i++;
        }

        if (bFoundSubString)
        {
            sReturnProfileName = sProfileName.substring(0, 
                i - s_uiRenderSpecificPrefixCount - 1);
            break;
        }
    }

    return sReturnProfileName;
}

MStatus GetPackingProfiles::doIt( const MArgList& args )
{
    NI_UNUSED_ARG(args);
    MStatus kStat = MStatus::kSuccess;

    NiTMap<const char*, NiUInt8> kProfilesRendererMap;
    NiUInt8 uiRendererValue = 1;
    for (NiUInt32 ui = 0; ui <= NiSystemDesc::RENDERER_NUM; ui++)
    {  
        NiTPrimitiveSet<const char*> kProfiles;
        NiMeshProfileProcessor::GetAvailableProfiles(kProfiles, 
            (NiSystemDesc::RendererID)ui);

        for(unsigned int uiProfileIndex = 0; 
            uiProfileIndex < kProfiles.GetSize(); uiProfileIndex++)
        {
            NiUInt8 uiValue;
            const char* pcFixedProfileName = 
                kProfiles.GetAt(uiProfileIndex);

            if (kProfilesRendererMap.GetAt(pcFixedProfileName,
                uiValue))
            {
                uiValue |= uiRendererValue;
            }
            else
            {
                uiValue = uiRendererValue;
            }

            kProfilesRendererMap.SetAt(pcFixedProfileName, 
                uiValue);
        }

        // Shift over for next renderer
        uiRendererValue <<= 1;
    }

    uiRendererValue = 1;
    NiUInt8 uiAllRenderers = 0;
    for (NiUInt32 ui = 0; ui <= NiSystemDesc::RENDERER_NUM; ui++)
    {
        uiAllRenderers |= uiRendererValue;
        uiRendererValue <<= 1;
    }

    //Convert the NiTPrimitiveSet<const char*> to a MString
    MStringArray kReturnArray;
    MString kCurrentProfile;

    NiTMapIterator kIter = kProfilesRendererMap.GetFirstPos();
    while (kIter)
    {
        const char* pcProfileName = NULL;
        NiUInt8 uiRenderers = 0;

        kProfilesRendererMap.GetNext(kIter, pcProfileName, uiRenderers);
        
        kCurrentProfile = pcProfileName;
        if (uiRenderers != uiAllRenderers)
        {
            kCurrentProfile += s_pcRenderSpecificPrefix;

            uiRendererValue = 1;
            bool bIsFirst = true;
            for (NiUInt32 ui = 0; ui <= NiSystemDesc::RENDERER_NUM; ui++)
            {
                if (uiRenderers & uiRendererValue)
                {  
                    if (!bIsFirst)
                        kCurrentProfile += " | ";
                    kCurrentProfile += NiSystemDesc::GetRendererString(
                        (NiSystemDesc::RendererID)ui);
                    bIsFirst = false;
                }

                uiRendererValue <<= 1;
            }
            kCurrentProfile += ")";
        }
        
        kReturnArray.append(kCurrentProfile);
    }

    clearResult(); 
    setResult(kReturnArray);

    return MStatus::kSuccess;
}

void* GetPackingProfiles::creator()
{
    return new GetPackingProfiles();
}