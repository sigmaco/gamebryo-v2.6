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

#ifndef NIMESHPROFILEXMLPARSER_H
#define NIMESHPROFILEXMLPARSER_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include <NiTArray.h>
#include <NiOpProfile.h>

#include "NiMeshProfileXMLParserLibType.h"
#include "NiMeshProfileElement.h"
#include "NiMeshProfileStream.h"
#include "NiMeshProfile.h"
#include "NiMeshProfileGroup.h"
#include "NiMeshProfileOperations.h"
#include "NiMeshProfileComponent.h"

class NiOpMergeAndSwizzle;
class NiOpClamp;
class NiOpReinterpretFormat;
class NiOpRemap;
class NiOpRename;
class NiOpStart;
class NiOpTerminator;
class NiOpNode;
class NiOpCastConvert;

/// @cond EMERGENT_INTERNAL
/**
    This class is a singleton that provides the access to all the profiles.
    The only public function is the one to register the profiles which, calls
    all the function to parse, compile, and convert to the NiOpProfile data
    structure.  This class loads all profiles from a given directory.
*/
/// @endcond
class NIMESHPROFILEXMLPARSER_ENTRY NiMeshProfileXMLParser :
    public NiRefObject
{
public:

    // Register all the profiles
    static bool RegisterAllProfilesWithProcessor(
        const char* pcDirectoryName = NULL, 
        const bool bPrintDescription = false);

protected:

    // Modifiers
    static bool ConstructProfile(NiOpProfile* pkOpProfile,
        const NiMeshProfile* pkProfile, 
        const NiSystemDesc::RendererID eRenderer);

    // Validate the profiles
    static bool Validate(
        NiTObjectArray <NiMeshProfilePtr>& kMeshProfiles,
        NiTObjectArray <NiMeshProfileStreamPtr>& kFinalizeStreams,
        NiTObjectArray <NiMeshProfileElementPtr>& kElements);

    // Process the profiles
    static bool Process(
        NiTObjectArray <NiMeshProfilePtr>& kMeshProfiles,
        NiTObjectArray <NiMeshProfileStreamPtr>& kFinalizeStreams,
        NiTObjectArray <NiMeshProfileElementPtr>& kElements,
        NiMeshProfileGroup* pkMeshProfileGroup);

    // Main load all function for all the directories
    static bool LoadAll(const char* pcDirectory, 
        NiMeshProfileGroup* pkMeshProfileGroup);

    // Main load function for one directory
    static bool LoadAll(const char* pcFolderName,
        NiTObjectArray <NiMeshProfilePtr>& kMeshProfiles,
        NiTObjectArray <NiMeshProfileStreamPtr>& kFinalizeStreams,
        NiTObjectArray <NiMeshProfileElementPtr>& kElements,
        NiMeshProfileGroup* pkMeshProfileGroup);

    static bool Load(const TiXmlElement* pkRoot,
        NiTObjectArray <NiMeshProfilePtr>& kMeshProfiles,
        NiTObjectArray <NiMeshProfileStreamPtr>& kFinalizeStreams,
        NiTObjectArray <NiMeshProfileElementPtr>& kElements,
        NiMeshProfileGroup* pkMeshProfileGroup);

    static bool Load(const char *pcXMLFilename,
        NiTObjectArray <NiMeshProfilePtr>& kMeshProfiles,
        NiTObjectArray <NiMeshProfileStreamPtr>& kFinalizeStreams,
        NiTObjectArray <NiMeshProfileElementPtr>& kElements,
        NiMeshProfileGroup* pkMeshProfileGroup);

    // Set functions
    static NiOpMergeAndSwizzle* SetMergeAndSwizzle(
        NiOpMergeAndSwizzle* pkOpMergeAndSwizzle, NiOpNode* pkPrevOpNode,
        const char* pcSemantic, const NiUInt32 uiIndex,
        const NiUInt32 uiComponentCount, const NiUInt32 uiComponentIndex,
        NiMeshProfileComponent::Component eComponent, const bool bIsLiteral,
        const double dLiteralValue);

    static NiOpClamp* SetClamp(NiOpNode* pkPrevOpNode,
        NiMeshProfileComponent::Component eComponent, 
        const NiMeshProfileComponent::NiClamp* pkClamp);

    static NiOpRemap* SetRemap(NiOpNode* pkPrevOpNode,
        NiMeshProfileComponent::Component eComponent, 
        const NiMeshProfileComponent::NiRemap* pkRemap);

    static NiOpCastConvert* SetCastConvert(NiOpNode* pkPrevOpNode, 
        NiDataStreamElement::Type eDstTyp,
        NiMeshProfileOperations::CastConvertType eConvertType);

    static NiOpNode* SetSpecialConvert(const char* pcSemanticName, NiUInt32
        uiIndex, NiOpNode* pkPrevOpNode, NiDataStreamElement::Format eDstFmt);

    // Function to convert an element to a op node chain
    static void SetFinalizeOpNodes(NiOpProfile* pkOpProfile, 
        const NiMeshProfile* pkProfile, const NiMeshProfileStream* pkStream, 
        const NiMeshProfileElement* pkElement, const NiUInt32 uiStreamID);
};

NiSmartPointer(NiMeshProfileXMLParser);

#endif  //#ifndef NIMESHPROFILEXMLPARSER_H
