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

#ifndef NIENTITYSTREAMINGASCII_H
#define NIENTITYSTREAMINGASCII_H

#include "NiEntityLibType.h"
#include "NiEntityStreaming.h"
#include "NiDOMTool.h"
#include "NiEntityPropertyInterface.h"
#include <NiFixedString.h>
#include <NiTSet.h>
#include <NiTMap.h>

#define REF_LINK_NULL 4294967295U // Max value for unsigned int

class NiEntityInterface;
class NiEntityComponentInterface;

// Abstract class for dealing with various streaming methods
class NIENTITY_ENTRY NiEntityStreamingAscii : public NiEntityStreaming
{
public:
    static NiFixedString STREAMING_EXTENSION;
    static NiFixedString STREAMING_DESCRIPTION;

    NiEntityStreamingAscii();
    virtual ~NiEntityStreamingAscii();

    // I/O Functions.
    virtual NiBool Load(const char* pcFileName, bool bUseSingleScene = true, 
        bool bFullyLoadDependencies = false);
    virtual NiBool Save(const char* pcFileName, unsigned int uiSceneIdx = 0);

    // File Extension of format that can be load/saved.
    // Also used for registering factory so that different files
    // can be read and saved appropriately.
    virtual NiFixedString GetFileExtension() const;
    virtual NiFixedString GetFileDescription() const;

    // This function returns whether or not an old version of the file was
    // converted upon load. This can be used to mark a file as needing to be
    // saved.
    virtual NiBool HasBeenConverted(NiScene* pkScene) const;

    // This function returns whether or not the scene has been modified during
    // the loading process.
    virtual NiBool HasBeenModified(NiScene* pkScene) const;

    // Resets all cached data from the streaming object.
    virtual void Reset();

    // Removes all entities for the specified scenes from the IDLink map.
    virtual void RemoveLoadedScene(NiFixedString kSourceFilename);
    void ClearLoadedScenes();

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    void RemoveFromLinkMap(NiScene* pkSceneToRemove);
    void Flush();
    NiBool StoreWorkingPath(const char* pcFileName);

    NiBool PopulateElementSet(NiDOMTool& kDOM);
    void BuildIDToElementMap();
    NiEntityPropertyInterface* CreateAndMapObject(
        const char* pcClass,
        const char* pcName,
        const NiUniqueID& kID,
        const NiFixedString& kSourceFilename);

    // DOM writing
    void WriteToDOM(
        NiDOMTool& kDOM,
        unsigned int uiSceneIdx,
        IDLinkMap& kIDMap);
    void WriteVersionToDOM(NiDOMTool& kDOM);
    void WriteSceneToDOM(NiDOMTool& kDOM, NiScene* pkScene);
    void WriteTemplateIDToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf);
    void WriteUniqueIDToDOM(
        NiDOMTool& kDOM,
        const char* pcAttributeName,
        const NiUniqueID& kUniqueID);
    void WriteEntityToDOM(NiDOMTool& kDOM, NiEntityInterface* pkEntity);
    void WriteEntityRefToDOM(NiDOMTool& kDOM, NiEntityInterface* pkEntity);
    void WriteComponentToDOM(
        NiDOMTool& kDOM,
        NiEntityComponentInterface* pkComp);
    void WriteSelectionSetsToDOM(NiDOMTool& kDOM, NiScene* pkScene);
    void WriteXRefsToDOM(NiDOMTool& kDOM, bool bLayer);
    void WriteMapObjectsToDOM(
        NiDOMTool& kDOM,
        IDLinkMap& kIDMap);
    void WritePropertyToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf,
        const NiFixedString& kPropertyName);
    void WritePropertyArrayToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf,
        const NiFixedString& kPropertyName,
        NiFixedString& kPrimitiveType,
        unsigned int uiCount);
    void WritePropertyToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf,
        const NiFixedString& kPropertyName,
        NiFixedString& kPrimitiveType,
        unsigned int uiIndex);

    void WritePropertyInfoToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const NiFixedString& kPropertyName);
    void WriteEntityPointerToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf,
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WriteObjectPointerToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf,
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WriteFloatToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf,
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WriteBoolToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WriteIntToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf,
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WriteUIntToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WriteShortToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WriteUShortToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WriteStringToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WritePoint2ToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WritePoint3ToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WriteQuaternionToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WriteMatrix3ToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WriteColorToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WriteColorAToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf,
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WriteUnknownTypeToDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf,
        const NiFixedString& kPropertyName,
        unsigned int uiIndex = 0);
    void WriteFilenameToDOM(NiDOMTool& kDOM, const NiFixedString& kFilename);

    // DOM Reading
    NiBool ReadClassValueNameID(
        NiDOMTool& kDOM,
        const char*& pcClass,
        const char*& pcValue,
        const char*& pcName,
        NiUniqueID& kID);
    NiBool ReadUniqueID(
        NiDOMTool& kDOM,
        const char* pcAttributeName,
        NiUniqueID& kUniqueID);
    NiBool ParseUniqueID(
        const char* pcUniqueID,
        NiUniqueID& kUniqueID,
        unsigned int uiFileVersion);
    NiBool ReadTemplateID(NiDOMTool& kDOM, NiUniqueID& kTemplateID);

    NiBool PreReadFromDOM(NiDOMTool& kDOM);
    NiBool ReadFromDOM(NiDOMTool& kDOM, bool bIsLayer);
    NiBool ReadAndCheckVersionFromDOM(NiDOMTool& kDOM);
    NiBool ReadSceneFromDOM(
        NiDOMTool& kDOM,
        bool bIsLayer);
    NiBool ReadSelectionSetsFromDOM(NiDOMTool& kDOM, NiScene* pkScene);
    NiBool ReadMasterFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf);
    NiBool ReadPrefabRootFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf);
    NiBool ReadEntityOrComponentFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf);
    NiBool ReadEntityRefFromDOM(NiDOMTool& kDOM, NiEntityInterface*& pkEntity);
    NiBool ReadPropertyFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf,
        NiEntityPropertyInterface* pkMasterPropIntf);
    NiBool ReadPropertyFromDOM(
        NiDOMTool& kDOM,
        const char* pcPrimitive,
        const char* pcName, 
        NiEntityPropertyInterface* pkPropIntf);
    NiBool ReadPropertyFromDOM(
        NiDOMTool& kDOM,
        const char* pcPrimitive,
        const char* pcName,
        const char* pcSemanticType, 
        const char* pcDisplayName,
        const char* pcDescription,
        NiEntityPropertyInterface* pkPropIntf);
    NiBool ReadPropertyArrayFromDOM(
        NiDOMTool& kDOM,
        const char* pcPrimitive,
        const char* pcName,
        NiEntityPropertyInterface* pkPropIntf);
    NiBool ReadXRefsFromDOM(NiDOMTool& kDOM, bool bLayer);
    NiBool ReadObjectsFromDOM(NiDOMTool& kDOM);

    NiBool ReadEntityPointerFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadObjectPointerFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadFloatFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadBoolFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf,
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadIntFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf,
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadUIntFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf,
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadShortFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadUShortFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf,
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadStringFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadPoint2FromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadPoint3FromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadQuaternionFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadMatrix3FromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadColorFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadColorAFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadUnknownTypeFromDOM(
        NiDOMTool& kDOM,
        NiEntityPropertyInterface* pkPropIntf, 
        const char* pcName,
        unsigned int uiIndex = 0);
    NiBool ReadFilenameFromDOM(NiDOMTool& kDOM, NiFixedString& kFilename);

    NiEntityPropertyInterface* GetFromID(const NiUniqueID& kID);

    IDLinkMap m_kLinkMap;
    FilenameSet m_kFilenameSet;

    struct IDElementItem : public NiMemObject
    {
        IDElementItem();
        IDElementItem(
            const NiUniqueID& kID,
            NiDOMTool* pkDOM,
            TiXmlElement* pkElement);

        NiUniqueID m_kID;
        NiDOMTool* m_pkDOM;
        TiXmlElement* m_pkElement;
    };
    NiTObjectSet<IDElementItem> m_kElementSet;
    NiTPointerMap<const NiUniqueID*, unsigned int, NiUniqueID::HashFunctor,
        NiUniqueID::HashFunctor> m_kElementIdxMap;

    NiTObjectArray<NiDOMToolPtr> m_kDOMs;

    struct SceneInfo : public NiMemObject
    {
        SceneInfo(){}
        SceneInfo(int){}

        unsigned int m_uiVersion;
        bool m_bModified;
    };
    NiTMap<NiScene*, SceneInfo> m_kSceneToInfoMap;

    NiTMap<unsigned int, NiUniqueID> m_kOldToNewIDMap;

    char m_acFilenameErrorMsg[512];
};

#include "NiEntityStreamingAscii.inl"

#endif // NIENTITYSTREAMINGASCII_H
