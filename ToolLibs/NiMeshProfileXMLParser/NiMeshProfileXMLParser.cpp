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

#include "NiMeshProfileProcessor.h"
#include "NiMeshProfileXMLParser.h"
#include "NiMeshProfileElement.h"
#include "NiMeshProfileStream.h"
#include "NiMeshProfile.h"
#include "NiMeshProfileGroup.h"
#include "NiOpProfile.h"

#include "NiOpMergeAndSwizzle.h"
#include "NiOpClamp.h"
#include "NiOpRename.h"
#include "NiOpRemap.h"
#include "NiOpReinterpretFormat.h"
#include "NiOpCastConvert.h"
#include "NiOpComponentPack.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#endif
#include <tinyxml.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

static const int s_nMeshProfileVersionNumber = 1;

//---------------------------------------------------------------------------
bool NiMeshProfileXMLParser::Validate(
    NiTObjectArray <NiMeshProfilePtr>& kMeshProfiles,
    NiTObjectArray <NiMeshProfileStreamPtr>& kFinalizeStreams,
    NiTObjectArray <NiMeshProfileElementPtr>& kElements)
{
    // We need to make sure all the elements are unique with in this scope.
    for (NiUInt32 ui = 0; ui < kElements.GetSize(); ui++)
    {
        for (NiUInt32 uj = ui+1; uj < kElements.GetSize(); uj++)
        {
            if (kElements[ui]->GetRenderer() ==
                kElements[uj]->GetRenderer() &&
                kElements[ui]->GetName() == 
                kElements[uj]->GetName() &&
                kElements[ui]->GetIndex() ==
                kElements[uj]->GetIndex() &&
                kElements[ui]->GetRefElement() ==
                kElements[uj]->GetRefElement())
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Duplicate \"name\" defined \"%s\" index \"%d\".\n",
                    kElements[ui]->GetName(), 
                    kElements[ui]->GetIndex());

                kElements.RemoveAtAndFill(uj);
                break;
            }
        }
    }

    // We need to make sure all the elements are unique with in this scope.
    for (NiUInt32 ui = 0; ui < kElements.GetSize(); ui++)
    {
        for (NiUInt32 uj = ui+1; uj < kElements.GetSize(); uj++)
        {
            if (kElements[ui]->GetRenderer() ==
                kElements[uj]->GetRenderer() &&
                kElements[ui]->GetSemanticName() == 
                kElements[uj]->GetSemanticName() &&
                kElements[ui]->GetIndex() ==
                kElements[uj]->GetIndex() &&
                kElements[ui]->GetRefElement() ==
                kElements[uj]->GetRefElement())
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Duplicate \"semantic\" defined \"%s\" index \"%d\".\n",
                    kElements[ui]->GetSemanticName(), 
                    kElements[ui]->GetIndex());

                kElements.RemoveAtAndFill(uj);
                break;
            }
        }
    }

    // We need to make sure all the finalize streams are unique with in this 
    // scope.
    for (NiUInt32 ui = 0; ui < kFinalizeStreams.GetSize(); ui++)
    {
        for (NiUInt32 uj = ui+1; uj < kFinalizeStreams.GetSize(); uj++)
        {
            if (kFinalizeStreams[ui]->GetRenderer() ==
                kFinalizeStreams[uj]->GetRenderer() &&
                kFinalizeStreams[ui]->GetName() == 
                kFinalizeStreams[uj]->GetName() &&
                kFinalizeStreams[ui]->GetRefStream() ==
                kFinalizeStreams[uj]->GetRefStream())
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Duplicate finalize stream defined \"%s\".\n",
                    kFinalizeStreams[ui]->GetName());

                kFinalizeStreams.RemoveAtAndFill(uj);
                break;
            }
        }
    }

    // We need to make sure all the finalize streams are unique with in this 
    // scope.
    for (NiUInt32 ui = 0; ui < kMeshProfiles.GetSize(); ui++)
    {
        for (NiUInt32 uj = ui+1; uj < kMeshProfiles.GetSize(); uj++)
        {
            if (kMeshProfiles[ui]->GetName() == 
                kMeshProfiles[uj]->GetName())
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "Duplicate mesh profile defined \"%s\".\n",
                    kMeshProfiles[ui]->GetName());

                kMeshProfiles.RemoveAtAndFill(uj);
                break;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMeshProfileXMLParser::Process(
    NiTObjectArray <NiMeshProfilePtr>& kMeshProfiles,
    NiTObjectArray <NiMeshProfileStreamPtr>& kFinalizeStreams,
    NiTObjectArray <NiMeshProfileElementPtr>& kElements,
    NiMeshProfileGroup* pkMeshProfileGroup)
{
    for (NiUInt32 ui = 0; ui < kFinalizeStreams.GetSize(); ui++)
    {
        if (!kFinalizeStreams[ui]->Fixup(kElements))
        {
            kFinalizeStreams.RemoveAtAndFill(ui);
            ui--;
        }
    }

    for (NiUInt32 ui = 0; ui < kMeshProfiles.GetSize(); ui++)
    {
        if (!kMeshProfiles[ui]->Fixup(kElements, kFinalizeStreams))
        {
            kMeshProfiles.RemoveAtAndFill(ui);
            ui--;
        }
    }

    // Link in all the preparation data into the profiles
    for (NiUInt32 ui = 0; ui < kMeshProfiles.GetSize(); ui++)
    {
        if (!kMeshProfiles[ui]->Link(kElements))
        {
            kMeshProfiles.RemoveAtAndFill(ui);
            ui--;
        }
    }

    // Covert all the profiles to the renderer specific profile.
    for (NiUInt32 ui=0; ui < kMeshProfiles.GetSize(); ui++)
    {  
        for (NiUInt32 uiRenderer = 0; 
            uiRenderer < MESHPROFILE_PARSER_NUM_RENDERERS; uiRenderer++)
        {   
            NiMeshProfile* pkNewProfile = pkMeshProfileGroup->NewProfile(
                (NiSystemDesc::RendererID)uiRenderer);
            pkNewProfile->CopyToRenderer((NiSystemDesc::RendererID)uiRenderer, 
                kMeshProfiles[ui]);
        }
    }

    // Validate all the profiles
    return pkMeshProfileGroup->Validate();
}
//---------------------------------------------------------------------------
bool NiMeshProfileXMLParser::Load(const TiXmlElement* pkRoot,
    NiTObjectArray <NiMeshProfilePtr>& kMeshProfiles,
    NiTObjectArray <NiMeshProfileStreamPtr>& kFinalizeStreams,
    NiTObjectArray <NiMeshProfileElementPtr>& kElements,
    NiMeshProfileGroup*)
{
    // Make sure we have a valid root
    if (pkRoot == NULL)
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportError(
            "Could not find the root node.\n");
        return false;
    }

    for (const TiXmlElement* pkElement = pkRoot->FirstChildElement(); 
        pkElement != NULL; pkElement = pkElement->NextSiblingElement())
    {
        if (NiStricmp(pkElement->Value(), 
            NiMeshProfile::GetTypeName()) == 0)
        {   
            NiMeshProfile* pkMeshProfile = NiNew NiMeshProfile;
            if (pkMeshProfile->Load(pkElement))
            {
                kMeshProfiles.Add(pkMeshProfile);
            }
            else
            {
                delete pkMeshProfile; 
            }
        }
        else if (NiStricmp(pkElement->Value(), 
            NiMeshProfileStream::GetFinalizeTypeName()) == 0)
        {
            NiMeshProfileStream* pkFinalizeStream = NiNew NiMeshProfileStream;
            if (pkFinalizeStream->Load(pkElement))
            {
                kFinalizeStreams.Add(pkFinalizeStream);
            }
            else
            {
                delete pkFinalizeStream; 
            }
        }
        else if (NiStricmp(pkElement->Value(), 
            NiMeshProfileElement::GetTypeName()) == 0)
        {
            NiMeshProfileElement* pkProfileElement = 
                NiNew NiMeshProfileElement;
            int iRangeMin = -1;
            int iRangeMax = -1;
            if (pkProfileElement->Load(pkElement, iRangeMin, iRangeMax))
            {
                // Handle the ranges
                if (iRangeMin >=0 && iRangeMax >=0 && 
                    iRangeMin < iRangeMax)
                {
                    for (int i = iRangeMin; i <= iRangeMax; i++)
                    {
                        NiMeshProfileElement* pkNewProfileElement = 
                            NiNew NiMeshProfileElement;
                        pkNewProfileElement->Copy(pkProfileElement);

                        pkNewProfileElement->FixupForRange(i);
                        kElements.Add(pkNewProfileElement);
                    }

                    // Destroy the base element
                    NiDelete pkProfileElement;
                }
                else
                {
                    kElements.Add(pkProfileElement);
                }
            }
            else
            {
                delete pkProfileElement; 
            }
        }
        else
        {
            NiMeshProfileProcessor::GetErrorHandler().ReportError(
                "Unsupported type found %s\n", pkElement->Value());
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMeshProfileXMLParser::Load(const char *pcXMLFilename,
    NiTObjectArray <NiMeshProfilePtr>& kMeshProfiles,
    NiTObjectArray <NiMeshProfileStreamPtr>& kFinalizeStreams,
    NiTObjectArray <NiMeshProfileElementPtr>& kElements,
    NiMeshProfileGroup* pkMeshProfileGroup)
{
    TiXmlBase::SetCondenseWhiteSpace(false);
    TiXmlDocument kDoc(pcXMLFilename);

    if (kDoc.LoadFile() == true)
    {
        TiXmlElement* pkRoot = kDoc.RootElement();
        TiXmlAttribute* pkAttrib = pkRoot->FirstAttribute();

        if (pkAttrib && NiStricmp(pkAttrib->Name(), "version") == 0)
        {
            if (s_nMeshProfileVersionNumber != pkAttrib->IntValue())
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportError(
                    "File: %s. Invalid version number: %d\n", pcXMLFilename,
                    pkAttrib->IntValue());
                return false;
            }
        }

        return Load(pkRoot, kMeshProfiles, kFinalizeStreams, kElements,
            pkMeshProfileGroup);
    }
    else if (kDoc.Error())
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportError(
            "File: %s. Parsing error found %d.\nDesc:%s\nRow:%d\tColumn:%d\n",
            pcXMLFilename, kDoc.ErrorId(), kDoc.ErrorDesc(), 
            kDoc.ErrorRow(), kDoc.ErrorCol());
        return false;
    }
    
     NiMeshProfileProcessor::GetErrorHandler().ReportError(
         "File: %s. Could not find file.\n", pcXMLFilename);
    return false;
}
//---------------------------------------------------------------------------
bool NiMeshProfileXMLParser::RegisterAllProfilesWithProcessor(
    const char *pcDirectoryName, const bool bPrintDescription)
{
    // Clear the error messages
    NiMeshProfileProcessor::GetErrorHandler().ClearMessages();

    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "Begin registering all profiles with the processor.\n");

    if (NiMeshProfileProcessor::GetMeshProfileProcessor())
    {
        char acMeshProfilePath[MAX_PATH];
        // Create the mesh profile data
#if defined(_MSC_VER) && _MSC_VER >= 1400
        NiUInt32 uiLen = 0;
        getenv_s(&uiLen, acMeshProfilePath, MAX_PATH, 
            "EGB_TOOL_PLUGIN_PATH");
        NIASSERT(uiLen <= MAX_PATH);
#else
        NiStrcpy(acMeshProfilePath, MAX_PATH, getenv(
            "EGB_TOOL_PLUGIN_PATH"));
#endif

        NiStrcat(acMeshProfilePath, MAX_PATH, "\\Data\\MeshProfiles");

        NiString kDirectories;
        if (pcDirectoryName)
        {
            kDirectories = pcDirectoryName;
            kDirectories += ";";
        }
        kDirectories += acMeshProfilePath;

        // Load all the profiles from the given directory
        NiMeshProfileGroup kProfileGroup;
        LoadAll((const char*)kDirectories, &kProfileGroup);

        // Check to see if we want to print out a description of all the 
        // profiles.
        if (bPrintDescription)
        {
            kProfileGroup.PrintDescription();
        }

        NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
            "Converting %d profiles to operational profile.\n", 
            kProfileGroup.GetProfileCount());

        for (NiUInt32 uiRenderer = 0; 
            uiRenderer < MESHPROFILE_PARSER_NUM_RENDERERS; uiRenderer++)
        {
            NiSystemDesc::RendererID eRenderer = 
                (NiSystemDesc::RendererID)uiRenderer;

            for (NiUInt32 ui = 0; 
                ui < kProfileGroup.GetProfileCount(eRenderer); ui++)
            {
                NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
                    "Converting %s profile for %s renderer.\n", 
                    kProfileGroup.GetProfile(eRenderer, ui)->GetName(),
                    NiSystemDesc::GetRendererString(eRenderer));

                NiOpProfile* pkProfile = NiNew NiOpProfile;
                ConstructProfile(pkProfile, kProfileGroup.GetProfile(
                    eRenderer, ui), eRenderer);

                bool bValid = pkProfile->ValidateProfile();
                if (!bValid)
                {
                    NiMeshProfileProcessor::GetErrorHandler().ReportError(
                        "Could not convert %s profile for %s renderer because "
                        "profile was not valid.\n",
                        kProfileGroup.GetProfile(eRenderer, ui)->GetName(),
                        NiSystemDesc::GetRendererString(eRenderer));

                    NiDelete pkProfile;
                    continue;
                }

                NiMeshProfileProcessor::AddProfile(pkProfile);
            }
        }

        NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
            "Finished registering all profiles with the processor.\n");
    }
    else
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportError(
            "The NiMeshProfileProcessor does not exist.\n");
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMeshProfileXMLParser::LoadAll(const char *pcDirectoryNames,
    NiMeshProfileGroup* pkMeshProfileGroup)
{
    NiTObjectArray <NiMeshProfilePtr> kMeshProfiles;
    NiTObjectArray <NiMeshProfileStreamPtr> kFinalizeStreams;
    NiTObjectArray <NiMeshProfileElementPtr> kElements;

    // Load all the files from all the given directories
    NiString kDirectories = pcDirectoryNames;
    int iCount = kDirectories.Length();
    int iIndex = kDirectories.FindReverse(';', iCount-1);
    while (iIndex > 0)
    {
        LoadAll((const char*)kDirectories.GetSubstring(iIndex + 1, iCount),
            kMeshProfiles, kFinalizeStreams, kElements, pkMeshProfileGroup);

        iCount = iIndex;
        iIndex = kDirectories.FindReverse(';', iCount-1);
    }

    LoadAll((const char*)kDirectories.GetSubstring(0, iCount),
            kMeshProfiles, kFinalizeStreams, kElements, pkMeshProfileGroup);

    // Validate the data
    if (!Validate(kMeshProfiles, kFinalizeStreams, kElements))
    {
        return false;
    }

    // Process all the data
    if (!Process(kMeshProfiles, kFinalizeStreams, kElements,
        pkMeshProfileGroup))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMeshProfileXMLParser::LoadAll(const char* pcFolderName,
    NiTObjectArray <NiMeshProfilePtr>& kMeshProfiles,
    NiTObjectArray <NiMeshProfileStreamPtr>& kFinalizeStreams,
    NiTObjectArray <NiMeshProfileElementPtr>& kElements,
    NiMeshProfileGroup* pkMeshProfileGroup)
{
    NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
        "Loading all profiles from %s.\n", pcFolderName);

    // Get the current directory
    char acBuffer[256];
    GetCurrentDirectory(sizeof(acBuffer), acBuffer);

    // Set the current directory
    if (!SetCurrentDirectory(pcFolderName))
        return false;

    // Run through all the xml files in the given folder
    WIN32_FIND_DATA kWFD;
    HANDLE hFind = FindFirstFile("*.xml", &kWFD);
    while(hFind != INVALID_HANDLE_VALUE)
    {
        NiMeshProfileProcessor::GetErrorHandler().ReportAnnotation(
            "Found %s profile file.\n", kWFD.cFileName);

        Load(kWFD.cFileName, kMeshProfiles, kFinalizeStreams, kElements,
            pkMeshProfileGroup);

        if (!FindNextFile(hFind, &kWFD))
            break;
    }

    // Close the handle.
    FindClose(hFind);

    // Set the current directory back
    SetCurrentDirectory(acBuffer);

    return true;
}
//---------------------------------------------------------------------------
NiOpMergeAndSwizzle* NiMeshProfileXMLParser::SetMergeAndSwizzle(
    NiOpMergeAndSwizzle* pkOpMergeAndSwizzle, NiOpNode* pkPrevOpNode, 
    const char* pcSemantic, const NiUInt32 uiIndex, 
    const NiUInt32 uiComponentCount, 
    const NiUInt32 uiComponentIndex, 
    NiMeshProfileComponent::Component eComponent, const bool bIsLiteral, 
    const double dLiteralValue)
{
    if (!pkOpMergeAndSwizzle)
    {
        pkOpMergeAndSwizzle = NiNew NiOpMergeAndSwizzle();
        pkOpMergeAndSwizzle->SetExpectedInputs((NiUInt8)uiComponentCount);
        pkOpMergeAndSwizzle->SetParameters(pcSemantic, (NiUInt8)uiIndex);
    }

    // Order is important please do not change
    pkOpMergeAndSwizzle->SetInput((NiUInt8)uiComponentIndex, 
        pkPrevOpNode, (NiUInt8)eComponent);

    // Check to see if we are a literal value and set it.
    if (bIsLiteral)
        pkOpMergeAndSwizzle->SetInput((NiUInt8)uiComponentIndex,
        dLiteralValue);
    
    return pkOpMergeAndSwizzle;
}
//---------------------------------------------------------------------------
NiOpClamp* NiMeshProfileXMLParser::SetClamp(NiOpNode* pkPrevOpNode, 
    NiMeshProfileComponent::Component eComponent, 
    const NiMeshProfileComponent::NiClamp* pkClamp)
{
    NiOpClamp* pkOpClamp =  NiNew NiOpClamp();
    pkOpClamp->SetParameters(eComponent, pkClamp->m_dMin, pkClamp->m_dMax);
    pkOpClamp->SetInput(pkPrevOpNode);
    
    return pkOpClamp;
}
//---------------------------------------------------------------------------
NiOpRemap* NiMeshProfileXMLParser::SetRemap(NiOpNode* pkPrevOpNode, 
    NiMeshProfileComponent::Component eComponent, 
    const NiMeshProfileComponent::NiRemap* pkRemap)
{
    NiOpRemap* pkOpRemap =  NiNew NiOpRemap();
    pkOpRemap->SetParameters(eComponent, pkRemap->m_dSrcStart, 
        pkRemap->m_dSrcEnd, pkRemap->m_dDestStart, pkRemap->m_dDestEnd);
    pkOpRemap->SetInput(pkPrevOpNode);
    
    return pkOpRemap;
}
//---------------------------------------------------------------------------
NiOpNode* NiMeshProfileXMLParser::SetSpecialConvert(
    const char* pcSemanticName, NiUInt32 uiIndex, NiOpNode* pkPrevOpNode,
    NiDataStreamElement::Format eDstFmt)
{
    if (eDstFmt == NiDataStreamElement::F_NORMUINT8_4_BGRA)
    {
        NiOpCastConvert* pkOpCastConvert =  NiNew NiOpCastConvert();
        NiMeshProfileOperations::CastConvertType eCastConvertType = 
            (NiCommonSemantics::BLENDINDICES() == pcSemanticName) ?
            NiMeshProfileOperations::CT_TYPECLAMP :
            NiMeshProfileOperations::CT_NORMALIZE;
        pkOpCastConvert->SetParameters(NiDataStreamElement::T_UINT8,
            eCastConvertType);
        pkOpCastConvert->SetInput(pkPrevOpNode);

        // Explicit Swizzle required
        NiOpMergeAndSwizzle* pkMaS = NiNew NiOpMergeAndSwizzle();
        pkMaS->SetExpectedInputs(4);
        pkMaS->SetInput(0, pkOpCastConvert, 2, true);
        pkMaS->SetInput(1, pkOpCastConvert, 1, true);
        pkMaS->SetInput(2, pkOpCastConvert, 0, true);
        pkMaS->SetInput(3, pkOpCastConvert, 3, true);
        pkMaS->SetParameters(NiFixedString(pcSemanticName), (NiUInt8)uiIndex);

        // Reinterpret as BGRA
        NiOpReinterpretFormat* pkReinterpret = NiNew NiOpReinterpretFormat();
        pkReinterpret->SetParameters(NiDataStreamElement::F_NORMUINT8_4_BGRA);
        pkReinterpret->SetInput(pkMaS);
        return pkReinterpret;
    }
    
    NIASSERT(eDstFmt == NiDataStreamElement::F_UINT_10_10_10_2 ||
        eDstFmt == NiDataStreamElement::F_UINT_10_10_10_L1 ||
        eDstFmt == NiDataStreamElement::F_NORMINT_10_10_10_2 ||
        eDstFmt == NiDataStreamElement::F_NORMINT_10_10_10_L1 ||
        eDstFmt == NiDataStreamElement::F_NORMINT_11_11_10);

    NiOpComponentPack* pkPack = NiNew NiOpComponentPack();
    pkPack->SetParameters(eDstFmt);
    pkPack->SetInput(pkPrevOpNode);
    return pkPack;
}
//---------------------------------------------------------------------------

NiOpCastConvert* NiMeshProfileXMLParser::SetCastConvert(NiOpNode* pkPrevOpNode,
    NiDataStreamElement::Type eDstTyp,
    NiMeshProfileOperations::CastConvertType eConvertType)
{
    NiOpCastConvert* pkOpCastConvert =  NiNew NiOpCastConvert();
    pkOpCastConvert->SetParameters(eDstTyp, eConvertType);
    pkOpCastConvert->SetInput(pkPrevOpNode);
    
    return pkOpCastConvert;
}
//---------------------------------------------------------------------------
void NiMeshProfileXMLParser::SetFinalizeOpNodes(NiOpProfile* pkOpProfile,
    const NiMeshProfile* pkProfile, const NiMeshProfileStream* pkStream, 
    const NiMeshProfileElement* pkElement, const NiUInt32 uiStreamID)
{
    if (!pkOpProfile)
        return;

    // Set the prevoius node to NULL
    NiOpNode* pkPrevOpNode = NULL;

    NiOpMergeAndSwizzle* pkMergeAndSwizzleOpNode = NULL;
    NiOpClamp* pkClampOpNode = NULL;
    NiOpRemap* pkRemapOpNode = NULL;

    // User can not currently explicitly specify the final stream as
    // being required, however, if any of the start streams are
    // required or generated, then it is assumed that the final stream
    // is required.
    bool bFinalStreamRequired = false;

    // Run through all the components
    for (NiUInt32 uiComponent = 0; 
        uiComponent < pkElement->GetComponentCount(); uiComponent++)
    {
        // Get the component
        const NiMeshProfileComponent* pkComponent = 
            pkElement->GetComponent(uiComponent);

        // Get the linked stream, element and component
        const NiMeshProfileComponent* pkLinkedComponent = NULL;
        const NiMeshProfileElement* pkLinkedElement = NULL;
        const NiMeshProfileStream* pkLinkedStream = NULL;

        pkProfile->GetFinalizeData(pkElement, 
            pkComponent->GetLinkedComponent(),
            &pkLinkedStream, &pkLinkedElement, &pkLinkedComponent);

        const char* pcSemanticName = 
            NiCommonSemantics::GetString(NiCommonSemantics::E__Invalid);
        NiUInt32 uiIndex = 0;
        NiMeshProfileComponent::Component eComponent = 
            NiMeshProfileComponent::PD_DISABLED;
        double dLiteral = 0.0;

        // These are used to parse the link if needed.
        NiFixedString kComponent;
        NiFixedString kElement;
        NiFixedString kStream;
        bool bIsLiteral = false;

        if (pkLinkedElement && pkLinkedComponent)
        {
            pcSemanticName = pkLinkedElement->GetSemanticName();
            uiIndex = pkLinkedElement->GetIndex();
            eComponent = pkLinkedComponent->GetType();
        }
        else
        {
            // Check to see if we are an external link or a literal value
            int iIndex;
            if (NiMeshProfileComponent::ParseComponentLink(
                pkComponent->GetLinkedComponent(), &kStream, 
                &kElement, &iIndex, &kComponent))
            {
                pcSemanticName = kElement;
                eComponent = NiMeshProfileComponent::GetComponentType(
                    ((const char*)kComponent)[0]);
                if (iIndex >= 0)
                    uiIndex = iIndex;
                else
                    uiIndex = pkElement->GetIndex();
            }
            else
            {
                dLiteral = atof(pkComponent->GetLinkedComponent());
                bIsLiteral = true;
            }
        }

        NiTObjectArray <NiOpStartPtr>& kStartFinalizeNodes =
            pkOpProfile->GetFinalizeStartNodes();

        // Start the nodes if not all ready started
        NiOpStart* pkStartOpNode = NULL;
        for (NiUInt32 ui = 0; ui < kStartFinalizeNodes.GetSize(); ui++)
        {
            if (kStartFinalizeNodes[ui]->IsEqual(pcSemanticName, (NiUInt8)uiIndex))
            {
                pkStartOpNode = kStartFinalizeNodes[ui];
                break;
            }
        }

        if (pkElement->GetRequired() == NiMeshProfileElement::REQUIRED_FAIL)
            bFinalStreamRequired = true;

        if (!pkStartOpNode)
        {
            pkStartOpNode = NiNew NiOpStart();
            pkStartOpNode->SetParameters(pcSemanticName, (NiUInt8)uiIndex);
            
            // Add it to the buffer
            kStartFinalizeNodes.Add(pkStartOpNode);
        }

        // Set the previous node
        pkPrevOpNode = pkStartOpNode;

        // Set the clamp operation if we need to
        if (pkComponent->NeedsClamp())
        {
            pkClampOpNode = SetClamp(pkPrevOpNode, 
                eComponent, &pkComponent->GetClamp());

            // If we are clamping the positional data we need to make sure
            // we recalculate the bound.
            if (pkElement->GetSemanticName() == NiCommonSemantics::POSITION())
            {
                pkOpProfile->SetPostResolveOp(NiOpProfile::PRO_COMPUTE_BOUNDS,
                    true);
            }

            // Set the previous node
            pkPrevOpNode = pkClampOpNode;
        }

        // Set the remap operation
        if (pkComponent->NeedsRemap())
        {
            pkRemapOpNode = SetRemap(pkPrevOpNode, 
                eComponent, &pkComponent->GetRemap());

            // If we are remapping the positional data we need to make sure
            // we recalculate the bound.
            if (pkElement->GetSemanticName() == NiCommonSemantics::POSITION())
            {
                pkOpProfile->SetPostResolveOp(NiOpProfile::PRO_COMPUTE_BOUNDS,
                    true);
            }

            // Set the previous node
            pkPrevOpNode = pkRemapOpNode;
        }

        // We always swizzle and merge the components
        pkMergeAndSwizzleOpNode = SetMergeAndSwizzle(pkMergeAndSwizzleOpNode,
            pkPrevOpNode, pkElement->GetSemanticName(), pkElement->GetIndex(),
            pkElement->GetComponentCount(), uiComponent, eComponent,
            bIsLiteral, dLiteral);

        // Set the previous node
        pkPrevOpNode = pkMergeAndSwizzleOpNode;
    }
    
    // Set the output format
    if (pkElement->NeedsSpecialConverting())
    {
        pkPrevOpNode = SetSpecialConvert(pkElement->GetSemanticName(),
            pkElement->GetIndex(), pkPrevOpNode, 
            pkElement->GetSpecialFormat());
    }
    else if (pkElement->NeedsConverting())
    {
        pkPrevOpNode = SetCastConvert(pkPrevOpNode, 
            pkElement->GetConvertFormat(), pkElement->GetConvertType());
    }

    NiOpTerminator* pkTerminatorOpNode =  NiNew NiOpTerminator();
    pkTerminatorOpNode->SetParameters(uiStreamID, 
        pkStream->GetAccessFlags(), pkStream->GetUsage(), 
        pkElement->GetStrictFlags(), bFinalStreamRequired);
    pkTerminatorOpNode->SetInput(pkPrevOpNode);

    NiTObjectArray <NiOpTerminatorPtr>& kTerminatorFinalizeNodes = 
        pkOpProfile->GetFinalizeTerminatorNodes();

    // Add it to the array
    kTerminatorFinalizeNodes.Add(pkTerminatorOpNode);
}
//---------------------------------------------------------------------------
bool NiMeshProfileXMLParser::ConstructProfile(NiOpProfile* pkOpProfile,
    const NiMeshProfile* pkProfile, const NiSystemDesc::RendererID eRenderer)
{
    // Init the member data
    pkOpProfile->SetName(pkProfile->GetName());
    pkOpProfile->SetDescription(pkProfile->GetDescription());
    pkOpProfile->SetRenderer(eRenderer);
    pkOpProfile->SetKeepUnspecifiedFinalizeStreams(
        pkProfile->GetKeepUnspecifiedFinalizeStreams());

    // If we need to recompute the normals set the post resolve operation.
    if (pkProfile->GetRecomputeNormals())
    {
        pkOpProfile->SetPostResolveOp(
            NiOpProfile::PRO_RECOMPUTE_NORMALS, true);
    }

    // Convert from the XML mesh profile to the operational mesh profile

    // Now we want to do the finalize phase
    for (NiUInt32 uiStream = 0; 
        uiStream < pkProfile->GetFinalizeStreamCount(); uiStream++)
    {
        const NiMeshProfileStream* pkStream = 
            pkProfile->GetFinalizeStream(uiStream);

        for (NiUInt32 uiElement = 0; 
            uiElement < pkStream->GetElementCount(); uiElement++)
        {
            // Create the terminator node from this element
            SetFinalizeOpNodes(pkOpProfile, pkProfile, pkStream,
                pkStream->GetElement(uiElement), uiStream);

        }
    }

    // Return
    return true;
}
//---------------------------------------------------------------------------
