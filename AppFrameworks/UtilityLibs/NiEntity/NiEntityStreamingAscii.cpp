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

// Precompiled Header
#include "NiEntityPCH.h"

#include "NiEntityStreamingAscii.h"
#include "NiScene.h"
#include "NiFactories.h"
#include "NiExternalAssetParams.h"
#include "NiPrefabComponent.h"
#include <tinyxml.h>

// Internal Aliases
#define PT_ENTITYPOINTER    NiEntityPropertyInterface::PT_ENTITYPOINTER
#define PT_NIOBJECTPOINTER  NiEntityPropertyInterface::PT_NIOBJECTPOINTER
#define PT_FLOAT            NiEntityPropertyInterface::PT_FLOAT
#define PT_BOOL             NiEntityPropertyInterface::PT_BOOL
#define PT_INT              NiEntityPropertyInterface::PT_INT
#define PT_UINT             NiEntityPropertyInterface::PT_UINT
#define PT_SHORT            NiEntityPropertyInterface::PT_SHORT
#define PT_USHORT           NiEntityPropertyInterface::PT_USHORT
#define PT_STRING           NiEntityPropertyInterface::PT_STRING
#define PT_POINT2           NiEntityPropertyInterface::PT_POINT2
#define PT_POINT3           NiEntityPropertyInterface::PT_POINT3
#define PT_QUATERNION       NiEntityPropertyInterface::PT_QUATERNION
#define PT_MATRIX3          NiEntityPropertyInterface::PT_MATRIX3
#define PT_COLOR            NiEntityPropertyInterface::PT_COLOR
#define PT_COLORA           NiEntityPropertyInterface::PT_COLORA

#define VERSION_MAJOR 2
#define VERSION_MINOR 0
#define VERSION_PATCH 0

#define GSA_VERSION(major, minor, patch) \
    ((major << 16) | (minor << 8) | (patch << 0))
#define CURRENT_GSA_VERSION \
    GSA_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH)

NiFixedString NiEntityStreamingAscii::STREAMING_EXTENSION;
NiFixedString NiEntityStreamingAscii::STREAMING_DESCRIPTION;

//---------------------------------------------------------------------------
void NiEntityStreamingAscii::_SDMInit()
{
    STREAMING_EXTENSION = "GSA";
    STREAMING_DESCRIPTION = "Gamebryo ASCII Scene files (*.gsa)|*.gsa";
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::_SDMShutdown()
{
    STREAMING_EXTENSION = NULL;
    STREAMING_DESCRIPTION = NULL;
}
//---------------------------------------------------------------------------
NiEntityStreamingAscii::NiEntityStreamingAscii() : m_kDOMs(1)
{
}
//---------------------------------------------------------------------------
NiEntityStreamingAscii::~NiEntityStreamingAscii() 
{
    Flush();
}
//---------------------------------------------------------------------------
NiFixedString NiEntityStreamingAscii::GetFileExtension() const
{
    return STREAMING_EXTENSION;
}
//---------------------------------------------------------------------------
NiFixedString NiEntityStreamingAscii::GetFileDescription() const
{
    return STREAMING_DESCRIPTION;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::HasBeenConverted(NiScene* pkScene) const
{
    NIASSERT(pkScene);

    SceneInfo kInfo;
    NIVERIFY(m_kSceneToInfoMap.GetAt(pkScene, kInfo));

    return (kInfo.m_uiVersion != CURRENT_GSA_VERSION);
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::HasBeenModified(NiScene* pkScene) const
{
    NIASSERT(pkScene);

    SceneInfo kInfo;
    NIVERIFY(m_kSceneToInfoMap.GetAt(pkScene, kInfo));

    return kInfo.m_bModified;
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::Reset()
{
    NiEntityStreaming::Reset();

    m_kLinkMap.RemoveLinks();
    m_kSceneToInfoMap.RemoveAll();
    Flush();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::RemoveLoadedScene(NiFixedString kSourceFilename)
{
    NiScene* pkScene = GetLoadedScene(kSourceFilename);
    if (pkScene)
    {
        RemoveFromLinkMap(pkScene);
        m_kSceneToInfoMap.RemoveAt(pkScene);
    }

    NiEntityStreaming::RemoveLoadedScene(kSourceFilename);
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::ClearLoadedScenes()
{
    const unsigned int uiSceneCount = m_kLoadedScenes.GetSize();
    for (unsigned int ui = 0; ui < uiSceneCount; ++ui)
    {
        NiScene* pkLoadedScene = m_kLoadedScenes.GetAt(ui);
        RemoveFromLinkMap(pkLoadedScene);
        m_kSceneToInfoMap.RemoveAt(pkLoadedScene);
    }

    NiEntityStreaming::ClearLoadedScenes();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::RemoveFromLinkMap(NiScene* pkSceneToRemove)
{
    NIASSERT(pkSceneToRemove);

    NiTMapIterator kIter = m_kLinkMap.GetFirstPos();
    while (kIter)
    {
        NiUniqueID kID;
        NiEntityPropertyInterface* pkPropIntf = NULL;
        NiScene* pkScene = NULL;
        m_kLinkMap.GetNext(kIter, kID, pkPropIntf, pkScene);

        if (pkScene == pkSceneToRemove)
        {
            m_kLinkMap.Remove(pkPropIntf->GetID());
        }
    }
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::Flush()
{
    m_kFilenameSet.Clear();
    m_kElementSet.RemoveAll();
    m_kElementIdxMap.RemoveAll();
    m_kDOMs.RemoveAll();
    m_kOldToNewIDMap.RemoveAll();
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::StoreWorkingPath(const char* pcFileName)
{
    // Assets store file names as unique absolute paths, however, 
    // when a GSA file is saved out, relative paths are used.
    // In order to restore a  unique absolute path, we need to know what 
    // directory the relative paths are "relative to".
    // In most cases, this will be the current working directory, but that
    // can't be guaranteed. For now, we assume pcFileName has a full 
    // standarized path so that we can grab.

    char acFullPath[NI_MAX_PATH];
    NiStrcpy(acFullPath, NI_MAX_PATH, pcFileName);

    if (!NiPath::IsUniqueAbsolute(acFullPath))
    {
          if (NiPath::IsRelative(acFullPath))
          {
              // Use the current working directory to build unique abs path.
              // On XENON, this should be something like "D:\\Data\\filename"
              // while on PS3, it might be "/app_home/../Data/Filename"
              NIVERIFY(
                  NiPath::ConvertToAbsolute(acFullPath, NI_MAX_PATH) > 0);
          }
          else
          {
              // This is an absolute path, though not unique, ie. it has
              // relative component parts.
              //Make unique and store in acFullPath
              NiPath::RemoveDotDots(acFullPath);
          }
    }

    // Create filename error message.
    NiSprintf(m_acFilenameErrorMsg, 512, "GSA File Error: %s", acFullPath);

    // Store the current GSA file path in the filename set.
    m_kFilenameSet.Initialize(acFullPath);

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadUniqueID(
    NiDOMTool& kDOM,
    const char* pcAttributeName,
    NiUniqueID& kUniqueID)
{
    const char* pcUniqueID = kDOM.GetAttributeFromCurrent(pcAttributeName);
    if (!pcUniqueID)
    {
        return false;
    }
    if (NiStricmp(pcUniqueID, "NULL") == 0)
    {
        kUniqueID = NiUniqueID();
        return true;
    }

    return ParseUniqueID(pcUniqueID, kUniqueID, kDOM.GetVersion());
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ParseUniqueID(
    const char* pcUniqueID,
    NiUniqueID& kUniqueID,
    unsigned int uiFileVersion)
{
    NIASSERT(pcUniqueID);

    if (uiFileVersion < GSA_VERSION(2, 0, 0))
    {
        unsigned int uiUniqueID;
#if defined(_MSC_VER) && _MSC_VER >= 1400
        int iFieldsAssigned = sscanf_s(pcUniqueID, "%u", &uiUniqueID);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
        int iFieldsAssigned = sscanf(pcUniqueID, "%u", &uiUniqueID);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
        if (iFieldsAssigned != 1)
        {
            char acErrorMsg[1024];
            NiSprintf(acErrorMsg, 1024, "Unable to parse LinkID: \"%s\".",
                pcUniqueID);
            ReportError(acErrorMsg, m_acFilenameErrorMsg, NULL, NULL);
            return false;
        }

        if (!m_kOldToNewIDMap.GetAt(uiUniqueID, kUniqueID))
        {
            NiUniqueID::Generate(kUniqueID);
            m_kOldToNewIDMap.SetAt(uiUniqueID, kUniqueID);
        }
    }
    else
    {
        if (!kUniqueID.FromString(pcUniqueID))
        {
            char acErrorMsg[1024];
            NiSprintf(acErrorMsg, 1024, "Unable to parse UniqueID: \"%s\".",
                pcUniqueID);
            ReportError(acErrorMsg, m_acFilenameErrorMsg, NULL, NULL);
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadTemplateID(
    NiDOMTool& kDOM,
    NiUniqueID& kTemplateID)
{
    if (kDOM.GetVersion() < GSA_VERSION(2, 0, 0))
    {
        const char* pcTemplateID = kDOM.GetAttributeFromCurrent(
            "TemplateID");
        if (!pcTemplateID)
        {
            return false;
        }
        if (NiStricmp(pcTemplateID, "NULL") == 0)
        {
            kTemplateID = NiUniqueID();
            return true;
        }

        if (!kTemplateID.FromString(pcTemplateID))
        {
            char acErrorMsg[1024];
            NiSprintf(acErrorMsg, 1024, "Unable to parse TemplateID: \"%s\".",
                pcTemplateID);
            ReportError(acErrorMsg, m_acFilenameErrorMsg, NULL, NULL);
            return false;
        }

        return true;
    }

    return ReadUniqueID(kDOM, "TemplateID", kTemplateID);
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadClassValueNameID(
    NiDOMTool& kDOM,
    const char*& pcClass,
    const char*& pcValue,
    const char*& pcName,
    NiUniqueID& kID)
{
    NIASSERT(kDOM.IsCurrentSectionValid());

    pcClass = kDOM.GetAttributeFromCurrent("Class");
    pcValue = kDOM.GetValueFromCurrent();

    // Some entities and components can have their names set.
    pcName = kDOM.GetAttributeFromCurrent("Name");

    if (!pcValue || !pcName || !pcClass)
    {
        return false;
    }

    if (!ReadUniqueID(kDOM, "LinkID", kID))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::PopulateElementSet(NiDOMTool& kDOM)
{
    // Child objects can be "ENTITY" or "COMPONENT"
    TiXmlElement* pkElement = kDOM.SetSectionToFirstChild();

    while (kDOM.IsCurrentSectionValid())
    {
        const char *pcValue, *pcClass, *pcName;
        NiUniqueID kID;

        if (!ReadClassValueNameID(kDOM, pcClass, pcValue, pcName, kID))
        {
            ReportError("Could not parse ENTITY/COMPONENT section.",
                m_acFilenameErrorMsg, NULL, NULL);
            return false;
        }

#ifdef _DEBUG
        // Verify that we are not adding duplicate entries to the array.
        const unsigned int uiArrayCount = m_kElementSet.GetSize();
        for (unsigned int ui = 0; ui < uiArrayCount; ++ui)
        {
            if (m_kElementSet.GetAt(ui).m_kID == kID)
            {
                ReportError("Loading multiple entities with the same ID.  "
                    "The second entity will be ignored",
                    m_acFilenameErrorMsg, (const char*) kID.ToString(), 
                    NULL);
            }
        }
#endif

        m_kElementSet.Add(IDElementItem(kID, &kDOM, pkElement));

        pkElement = kDOM.SetSectionToNextSibling();
    }

    kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::BuildIDToElementMap()
{
    // This function must be called after all calls to PopulateElementSet have
    // been completed. If the element set array is re-allocated after the
    // hash table has been generated, the NiUniqueID pointer values will be
    // bogus and cause a crash. Do not modify the m_kElementSet array after
    // calling this function.

    const unsigned int uiArrayCount = m_kElementSet.GetSize();
    for (unsigned int uiIndex = 0; uiIndex < uiArrayCount; ++uiIndex)
    {
        m_kElementIdxMap.SetAt(&m_kElementSet.GetAt(uiIndex).m_kID, uiIndex);
    }
}
//---------------------------------------------------------------------------
NiEntityPropertyInterface* NiEntityStreamingAscii::CreateAndMapObject(
    const char* pcClass,
    const char* pcName,
    const NiUniqueID& kID,
    const NiFixedString& kSourceFilename)
{
    NiTFactory<NiEntityPropertyInterface*>* pkCompFactory = 
        NiFactories::GetEntityCompFactory();
    if (!pkCompFactory)
    {
        char acErrorMsg[1024];
        NiSprintf(acErrorMsg, 1024, "Could not create object of "
            "type \"%s\".\nThe EntityCompFactory could not be found. "
            "Ensure that NiEntity.h is included.", pcClass);
        ReportError(acErrorMsg, m_acFilenameErrorMsg, NULL, NULL);
        return NULL;
    }
    
    NiEntityPropertyInterface* pkPropIntf = pkCompFactory->Create(pcClass);
    if (!pkPropIntf)
    {
        // REPORT_ERROR: Don't know how to create this component.
        // Application or plug-in needs to register it.
        char acErrorMsg[1024];
        NiSprintf(acErrorMsg, 1024, "Could not create object of type "
            "\"%s\".\nNo creation function has been registered for "
            "this type.", pcClass);
        ReportError(acErrorMsg, m_acFilenameErrorMsg, NULL, NULL);
        return NULL;
    }

    // Set the name if allowed.
    pkPropIntf->SetName(pcName);

    // Set the ID.
    pkPropIntf->SetID(kID);
    m_kLinkMap.Add(pkPropIntf, NULL);

    // Set the file path.
    pkPropIntf->SetSourceFilename(kSourceFilename);

    return pkPropIntf;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::Load(
    const char* pcFileName,
    bool bUseSingleScene,
    bool bFullyLoadDependencies)
{
    Flush();

    if (!StoreWorkingPath(pcFileName))
        return false;

    RemoveAllScenes();

    // Create and initialize DOM.
    NiDOMTool* pkMainDOM = NiNew NiDOMTool(
        m_kFilenameSet.GetCurrentFilename());
    NIASSERT(pkMainDOM);
    m_kDOMs.Add(pkMainDOM);

    // Read from main GSA file.
    if (!pkMainDOM->LoadFile())
    {
        Flush();
        ReportError("The main GSA file either does not exist or could not be "
            "parsed as valid XML.", m_acFilenameErrorMsg, NULL, NULL);
        return false;
    }

    // Read from DOM to populate filename set.
    if (!PreReadFromDOM(*pkMainDOM))
    {   
        Flush();
        return false;
    }

    // Set DOM array allocation to match number of filenames.
    unsigned int uiDOMCount = m_kFilenameSet.GetCount() + 1;
    m_kDOMs.SetSize(uiDOMCount);
    if (uiDOMCount > 1)
    {
        m_kDOMs.SetGrowBy(uiDOMCount / 2);
    }

    // Iterate over filenames, loading each into a DOM. This may add additional
    // filenames to the filename set, but they will be added to the end of
    // the set and thus this loop will end up iterating over them until no more
    // have been added. Do not change this loop to loop to a static unsigned
    // int variable instead of calling GetCount on the filename set.
    for (unsigned int ui = 0; ui < m_kFilenameSet.GetCount(); ++ui)
    {
        const NiFixedString& kFilename = m_kFilenameSet.GetAt(ui);
        if (kFilename != m_kFilenameSet.GetCurrentFilename() &&
            !GetLoadedScene(kFilename))
        {
            // Create and initialize DOM.
            NiDOMTool* pkDOM = NiNew NiDOMTool(kFilename);
            NIASSERT(pkDOM);
            
            // Read from GSA file.
            if (!pkDOM->LoadFile())
            {
                ReportError("The dependent GSA file either does not exist "
                    "or could not be parsed as valid XML.",
                    (const char*) kFilename, NULL, NULL);
                m_kFilenameSet.RemoveAt(ui--);
                NiDelete pkDOM;
                continue;
            }

            m_kDOMs.Add(pkDOM);

            // Read from DOM to populate filename set.
            if (!PreReadFromDOM(*pkDOM))
            {
                Flush();
                return false;
            }
        }
    }

    // Build hash table to map ID to element item.
    BuildIDToElementMap();

    // Process main scene DOM.
    NiScene* pkMainScene = GetLoadedScene(pkMainDOM->GetFilename());
    if (pkMainScene)
    {
        InsertScene(pkMainScene);
    }
    else
    {
        if (!ReadFromDOM(*pkMainDOM, true))
        {
            Flush();
            RemoveAllScenes();
            return false;
        }
        NIASSERT(GetSceneCount() > 0);
        pkMainScene = GetSceneAt(0);
    }

    // Now that all XREF files have been loaded and had their entities and
    // components mapped to IDs, continue loading all the objects from those
    // DOMs.
    const unsigned int uiFilenameSetCount = m_kFilenameSet.GetCount();
    for (unsigned int uiFilenameSet = 0; uiFilenameSet < uiFilenameSetCount;
        ++uiFilenameSet)
    {
        const NiFixedString& kFilename = m_kFilenameSet.GetAt(uiFilenameSet);
        bool bIsLayer = m_kFilenameSet.IsLayer(uiFilenameSet);
        NiScene* pkScene = GetLoadedScene(kFilename);
        if (pkScene)
        {
            if (bIsLayer)
            {
                InsertScene(pkScene);
            }
        }
        else if (bFullyLoadDependencies || bIsLayer)
        {
            uiDOMCount = m_kDOMs.GetSize();
            for (unsigned int uiDOM = 0; uiDOM < uiDOMCount; ++uiDOM)
            {
                NiDOMTool* pkDOM = m_kDOMs.GetAt(uiDOM);
                if (!pkDOM || pkDOM->GetFilename() != kFilename)
                {
                    continue;
                }

                if (!ReadFromDOM(*pkDOM, bIsLayer))
                {
                    Flush();
                    RemoveAllScenes();
                    return false;
                }

                break;
            }
        }
    }

    // bUseSingleScene means that we copy all the entities from all of the 
    // scenes into the main scene.  Get each scene, except for the main scene
    // at 0, and put the entities into the main scene.  Note that adding an 
    // entity to a scene changes the filename.  We do not want that, so we have
    // to preserve the original filename and set it again once the entity has
    // been added.  Once all the entities of a scene have been added to the 
    // main scene, remove them from the scenes.
    // NOTE #1: Once all the entities are copied into the main scene, any 
    // request for the main scene will get this combined scene instead of the scene
    // as it was originally loaded.
    // NOTE #2: If the combined scene is saved, only those entities that were
    // originally in the main scene will be saved.  The others will not be 
    // saved because the scene filename does not match the entity filenames.
    if (bUseSingleScene)
    {
        unsigned int uiNumScenes = GetSceneCount();
        for (unsigned int uiScene = uiNumScenes-1; uiScene > 0; uiScene--)
        {
            NiScene* pkScene = GetSceneAt(uiScene);
            unsigned int uiEntityCount = pkScene->GetEntityCount();
            for (unsigned int ui = 0; ui < uiEntityCount; ui++)
            {
                NiEntityInterface* pkEntity = pkScene->GetEntityAt(ui);
                NiFixedString kSourceFilename = pkEntity->GetSourceFilename();
                pkMainScene->AddEntity(pkEntity);
                pkEntity->SetSourceFilename(kSourceFilename);
            }
            RemoveScene(pkScene);
        }
    }

    // Set the source filename for the main scene to be the main GSA filename.
    pkMainScene->SetSourceFilename(m_kFilenameSet.GetCurrentFilename());

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadAndCheckVersionFromDOM(NiDOMTool& kDOM)
{
    const char* pcMajor = kDOM.GetAttributeFromCurrent("Major");
    const char* pcMinor = kDOM.GetAttributeFromCurrent("Minor");
    const char* pcPatch = kDOM.GetAttributeFromCurrent("Patch");

    if (!pcMajor || !pcMinor || !pcPatch)
        return false;

    unsigned int uiMajor, uiMinor, uiPatch;
    int iFieldsAssigned;
    
#if defined(_MSC_VER) && _MSC_VER >= 1400
    iFieldsAssigned = sscanf_s(pcMajor, "%u", &uiMajor);  
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    iFieldsAssigned = sscanf(pcMajor, "%u", &uiMajor);  
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 1)
    {
        return false;
    }

#if defined(_MSC_VER) && _MSC_VER >= 1400
    iFieldsAssigned = sscanf_s(pcMinor, "%u", &uiMinor);  
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    iFieldsAssigned = sscanf(pcMinor, "%u", &uiMinor);  
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 1)
    {
        return false;
    }

#if defined(_MSC_VER) && _MSC_VER >= 1400
    iFieldsAssigned = sscanf_s(pcPatch, "%u", &uiPatch);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    iFieldsAssigned = sscanf(pcPatch, "%u", &uiPatch);  
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 1)
    {
        return false;
    }

    kDOM.SetVersion(GSA_VERSION(uiMajor, uiMinor, uiPatch));

    if (kDOM.GetVersion() > CURRENT_GSA_VERSION)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::PreReadFromDOM(NiDOMTool& kDOM)
{
    if (!kDOM.ResetSectionTo("GSA"))
    {
        ReportError("No GSA section found in file.", m_acFilenameErrorMsg,
            NULL, NULL);
        return false;
    }

    if (!ReadAndCheckVersionFromDOM(kDOM))
    {
        ReportError("GSA file has incompatible version number.",
            m_acFilenameErrorMsg, NULL, NULL);
        return false;
    }

    kDOM.SetSectionToFirstChild();
    if (!kDOM.IsCurrentSectionValid())
    {
        ReportError("Empty GSA file.", m_acFilenameErrorMsg, NULL, NULL);
        return false;
    }

    while (kDOM.IsCurrentSectionValid())
    {
        const char* pcValue = kDOM.GetValueFromCurrent();
        if (NiStricmp(pcValue, "LAYERS") == 0)
        {
            if (!ReadXRefsFromDOM(kDOM, true))
            {
                return false;
            }
        }
        else if (NiStricmp(pcValue, "DEPENDENCIES") == 0)
        {
            if (!ReadXRefsFromDOM(kDOM, false))
            {
                return false;
            }
        }
        else if (NiStricmp(pcValue, "OBJECTS") == 0)
        {
            // Populate element set for all referenced objects. The objects
            // will be created and loaded later when requested via GetFromID.
            if (!PopulateElementSet(kDOM))
            {
                return false;
            }
        }

        kDOM.SetSectionToNextSibling();
    }

    kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadFromDOM(
    NiDOMTool& kDOM,
    bool bIsLayer)
{
    // This function assumes that PreReadFromDOM has already been called for
    // this DOM. That function verifies proper GSA sections and versioning.

    NIVERIFY(kDOM.ResetSectionTo("GSA"));
    kDOM.SetSectionToFirstChild();
    NIASSERT(kDOM.IsCurrentSectionValid());

    while (kDOM.IsCurrentSectionValid())
    {
        const char* pcValue = kDOM.GetValueFromCurrent();

        if (kDOM.GetVersion() < GSA_VERSION(2, 0, 0))
        {
            if (NiStricmp(pcValue, "SCENES") == 0)
            {
                kDOM.SetSectionToFirstChild();

                if (!kDOM.IsCurrentSectionValid())
                {
                    // Should have at least one scene.
                    ReportError("No SCENE sections defined in GSA file.",
                        m_acFilenameErrorMsg, NULL, NULL);
                    return false;
                }

                while (kDOM.IsCurrentSectionValid())
                {
                    pcValue = kDOM.GetValueFromCurrent();
                    if (NiStricmp(pcValue, "SCENE") != 0)
                    {
                        kDOM.SetSectionToNextSibling();
                        continue;
                    }

                    if (!ReadSceneFromDOM(kDOM, bIsLayer))
                    {
                        return false;
                    }

                    kDOM.SetSectionToNextSibling();
                }

                kDOM.EndSection();
            }
        }
        else
        {
            if (NiStricmp(pcValue, "SCENE") == 0)
            {
                if (!ReadSceneFromDOM(kDOM, bIsLayer))
                {
                    return false;
                }
            }
        }

        kDOM.SetSectionToNextSibling();
    }

    kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadXRefsFromDOM(NiDOMTool& kDOM, bool bLayer)
{
    kDOM.SetSectionToFirstChild();

    while (kDOM.IsCurrentSectionValid())
    {
        const char* pcValue = kDOM.GetValueFromCurrent();
        if (NiStricmp(pcValue, "FILENAME") == 0)
        {
            NiFixedString kFilename;
            if (!ReadFilenameFromDOM(kDOM, kFilename))
            {
                ReportError("Unable to parse path for FILENAME tag.",
                    m_acFilenameErrorMsg, NULL, NULL);
                return false;
            }

            bool bExplicit = false;
            const char* pcExplicit = kDOM.GetAttributeFromCurrent("Explicit");
            if (pcExplicit && NiStricmp(pcExplicit, "TRUE") == 0)
            {
                bExplicit = true;
            }

            m_kFilenameSet.Add(kFilename, bLayer, bExplicit,
                kDOM.GetFilename());
        }

        kDOM.SetSectionToNextSibling();
    }

    kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadFilenameFromDOM(
    NiDOMTool& kDOM,
    NiFixedString& kFilename)
{
    NiFixedString kValue;
    if (!kDOM.ReadPrimitive(kValue))
    {
        return false;
    }

    // The path needs to be converted to an absolute path.
    char acAbsolutePath[NI_MAX_PATH];
    NiStrcpy(acAbsolutePath, NI_MAX_PATH, kValue);
    NiPath::Standardize(acAbsolutePath);
    if (NiPath::IsRelative(acAbsolutePath))
    {
        NIVERIFY(NiPath::ConvertToAbsolute(acAbsolutePath,
            NI_MAX_PATH, kDOM.GetFilePath()) > 0);
    }

    kFilename = acAbsolutePath;
    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadEntityRefFromDOM(
    NiDOMTool& kDOM,
    NiEntityInterface*& pkEntity)
{
    NiUniqueID kEntityID;
    if (!ReadUniqueID(kDOM, "RefLinkID", kEntityID))
    {
        return false;
    }

    if (kEntityID == NiUniqueID())
    {
        pkEntity = NULL;
        return true;
    }

    pkEntity = (NiEntityInterface*) GetFromID(kEntityID);
    if (!pkEntity)
    {
        char acMsg[512];
        NiSprintf(acMsg, 512, "CONFLICT: Cannot resolve entity with ID: "
            "\"%s\".  The reference to that entity has been removed.",
            (const char*) kEntityID.ToString());
        ReportError(acMsg, m_acFilenameErrorMsg, NULL, NULL);
        kDOM.SetModified(true);
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadSceneFromDOM(
    NiDOMTool& kDOM,
    bool bIsLayer)
{
    // Traverse DOM for scene, and create NiScene object as needed. Links to
    // entities are resolved as needed in a recursive fashion, resulting in an
    // entity being loaded from DOM the first time it is requested.

    NIASSERT(NiStricmp(kDOM.GetValueFromCurrent(), "SCENE") == 0);

    // Read in class (ensure it is NiScene)
    // Read in name...
    const char* pcClass = kDOM.GetAttributeFromCurrent("Class");
    const char* pcName = kDOM.GetAttributeFromCurrent("Name");
    if (!pcClass || !pcName)
    {
        ReportError("Error parsing SCENE tag.", m_acFilenameErrorMsg,
            NULL, NULL);
        return false;
    }

    // Only NiScene classes are currently supported.
    if (NiStricmp(pcClass, "NiScene") != 0)
    {
        ReportError("Non NiScene specified in SCENE tag. Only NiScene is "
            "supported by GSA files.", m_acFilenameErrorMsg, NULL, NULL);
        return false;
    }

    NiScene* pkScene = NiNew NiScene(pcName);
    pkScene->SetSourceFilename(kDOM.GetFilename());
    if (bIsLayer)
    {
        InsertScene(pkScene);
    }

    AddLoadedScene(pkScene);

    kDOM.SetSectionToFirstChild();
    while (kDOM.IsCurrentSectionValid())
    {
        const char* pcValue = kDOM.GetValueFromCurrent();

        if (NiStricmp(pcValue, "ENTITY") == 0)
        {
            NiEntityInterface* pkEntity; 
            if (!ReadEntityRefFromDOM(kDOM, pkEntity))
            {
                kDOM.SetSectionToNextSibling();
                continue;
            }

            // Check to see if this entity is a prefab root.  If it is a 
            // prefab root and it is not in another prefab, then we need to 
            // make sure that scene in the prefab path has been loaded.  We
            // need this scene so that we can verify the prefab entities 
            // against the master prefab.  We are going to recursively examine
            // each prefab, so we only need to look at prefabs at the top level
            // here.
            NiFixedString kPrefabPath;
            if (pkEntity->GetPrefabRoot() == NULL && 
                pkEntity->GetPropertyData(
                    NiPrefabComponent::PROP_PREFAB_PATH(), 
                    kPrefabPath))
            {
                // Try to get the prefab scene from our loaded scenes
                NiScene* pkPrefabScene = GetLoadedScene(kPrefabPath);
                if (!pkPrefabScene)
                {
                    // Find the scene from our DOM list and load it
                    unsigned int uiDOMCount = m_kDOMs.GetSize();
                    for (unsigned int uiDOM = 0; uiDOM < uiDOMCount; ++uiDOM)
                    {
                        NiDOMTool* pkDOM = m_kDOMs.GetAt(uiDOM);
                        if (!pkDOM || pkDOM->GetFilename() != kPrefabPath)
                        {
                            continue;
                        }

                        if (!ReadFromDOM(*pkDOM, false))
                        {
                            ReportError("Could not read prefab from DOM", 
                                m_acFilenameErrorMsg, kPrefabPath, NULL);
                            return false;
                        }
                        break;
                    }
                }
                pkPrefabScene = GetLoadedScene(kPrefabPath);
                bool bIsError = false;
                if (pkPrefabScene)
                {
                    NIVERIFY(pkScene->AddEntity(pkEntity));
                    // Check the entity against the prefab from which it was 
                    // created
                    if (VerifyPrefabEntities(pkScene, pkEntity, pkPrefabScene, 
                        bIsError))
                    {
                        kDOM.SetModified(true);
                    }                    
                }
                else
                {
                    ReportError("CONFLICT: Could not find prefab source file."
                        "This prefab entity will be removed from the scene.",
                        m_acFilenameErrorMsg, pkEntity->GetName(), 
                        kPrefabPath);
                    kDOM.SetModified(true);
                }

                if (bIsError)
                {
                    ReportError("Could not resolve prefab.  This prefab "
                        "entity will be missing entities.", 
                        m_acFilenameErrorMsg, kPrefabPath, NULL);
                    return false;
                }
            }
            else
            {
                NIVERIFY(pkScene->AddEntity(pkEntity));
            }
        }
        else if (NiStricmp(pcValue, "SELECTIONS") == 0)
        {
            if (!ReadSelectionSetsFromDOM(kDOM, pkScene))
            {
                return false;
            }
        }

        kDOM.SetSectionToNextSibling();
    }
    kDOM.EndSection();

    // Add layers to the NiScene object.
    const NiTObjectSet<NiFixedString>* pkExplicitLayerSet =
        m_kFilenameSet.GetExplicitLayersForFilename(
        pkScene->GetSourceFilename());
    if (pkExplicitLayerSet)
    {
        const unsigned int uiFilenameCount = pkExplicitLayerSet->GetSize();
        for (unsigned int ui = 0; ui < uiFilenameCount; ++ui)
        {
            pkScene->AddLayerFilename(pkExplicitLayerSet->GetAt(ui));
        }
    }

    SceneInfo kInfo;
    NIASSERT(!m_kSceneToInfoMap.GetAt(pkScene, kInfo));
    kInfo.m_uiVersion = kDOM.GetVersion();
    kInfo.m_bModified = NIBOOL_IS_TRUE(kDOM.GetModified());
    m_kSceneToInfoMap.SetAt(pkScene, kInfo);

    // Add scene entities and components to ID map.
    const unsigned int uiEntityCount = pkScene->GetEntityCount();
    for (unsigned int uiEntity = 0; uiEntity < uiEntityCount; ++uiEntity)
    {
        NiEntityInterface* pkEntity = pkScene->GetEntityAt(uiEntity);
        m_kLinkMap.Add(pkEntity, pkScene);

        const unsigned int uiCompCount = pkEntity->GetComponentCount();
        for (unsigned int uiComp = 0; uiComp < uiCompCount; ++uiComp)
        {
            NiEntityComponentInterface* pkComp = pkEntity->GetComponentAt(
                uiComp);
            m_kLinkMap.Add(pkComp, pkScene);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadSelectionSetsFromDOM(
    NiDOMTool& kDOM,
    NiScene* pkScene)
{
    // <SELECTIONS>
    //     <SET Name="The Set Name">
    //          <ENTITY RefLinkID="#">
    //     </SET>
    // </SELECTIONS>

    // Set to first "SET"
    kDOM.SetSectionToFirstChild();
    while (kDOM.IsCurrentSectionValid())
    {
        const char* pcValue = kDOM.GetValueFromCurrent();

        if (NiStricmp(pcValue, "SET") != 0)
        {
            kDOM.SetSectionToNextSibling();
            continue;
        }

        const char* pcName = kDOM.GetAttributeFromCurrent("Name");
        if (!pcName)
        {
            ReportError("Unnamed selection set found in scene. Selection set "
                "will not be created.", m_acFilenameErrorMsg, NULL, NULL);
            continue;
        }

        NiEntitySelectionSet* pkSelectionSet = 
            NiNew NiEntitySelectionSet(pcName);

        // Set to first "ENTITY" entry
        kDOM.SetSectionToFirstChild();
        NIASSERT(kDOM.IsCurrentSectionValid());
        while (kDOM.IsCurrentSectionValid())
        {
            pcValue = kDOM.GetValueFromCurrent();

            if (NiStricmp(pcValue, "ENTITY") != 0)
            {
                kDOM.SetSectionToNextSibling();
                continue;
            }

            NiEntityInterface* pkEntity; 
            if (!ReadEntityRefFromDOM(kDOM, pkEntity))
            {
                kDOM.SetSectionToNextSibling();
                continue;
            }

            pkSelectionSet->AddEntity(pkEntity);

            kDOM.SetSectionToNextSibling();
        }
        kDOM.EndSection();

        pkScene->AddSelectionSet(pkSelectionSet);

        kDOM.SetSectionToNextSibling();
    }

    kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadMasterFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf)
{
    // It is valid for no master to exist (i.e., for templates).
    if (!kDOM.GetAttributeFromCurrent("MasterLinkID"))
    {
        return true;
    }

    NiUniqueID kMasterID;
    if (!ReadUniqueID(kDOM, "MasterLinkID", kMasterID))
    {
        return false;
    }
    bool bNullMasterID = (kMasterID == NiUniqueID());

    if (pkPropIntf->GetInterfaceType() ==
        NiEntityInterface::IT_ENTITYINTERFACE)
    {
        NiEntityInterface* pkMasterEntity = NULL;
        if (!bNullMasterID)
        {
            pkMasterEntity = (NiEntityInterface*) GetFromID(kMasterID);
            if (!pkMasterEntity)
                return false;
        }
        ((NiEntityInterface*) pkPropIntf)->SetMasterEntity(pkMasterEntity);
    }
    else if (pkPropIntf->GetInterfaceType() ==
        NiEntityComponentInterface::IT_COMPONENTINTERFACE)
    {
        NiEntityComponentInterface* pkMasterComp = NULL;
        if (!bNullMasterID)
        {
            pkMasterComp = (NiEntityComponentInterface*) GetFromID(kMasterID);
            if (!pkMasterComp)
                return false;
        }
        ((NiEntityComponentInterface*) pkPropIntf)->SetMasterComponent(
            pkMasterComp);
    }
    else
    {
        // Unknown interface type.
        ReportError("Unknown object interface type.", m_acFilenameErrorMsg,
            pkPropIntf->GetName(), NULL);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadPrefabRootFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf)
{
    // It is valid for no prefab root to exist
    if (!kDOM.GetAttributeFromCurrent("PrefabID"))
    {
        return true;
    }

    NiUniqueID kPrefabID;
    if (!ReadUniqueID(kDOM, "PrefabID", kPrefabID))
    {
        return false;
    }

    if (pkPropIntf->GetInterfaceType() == 
        NiEntityInterface::IT_ENTITYINTERFACE)
    {
        NiEntityInterface* pkPrefabRoot = NULL;
        bool bNullPrefabID = (kPrefabID == NiUniqueID());
        if (!bNullPrefabID)
        {
            pkPrefabRoot = (NiEntityInterface*)GetFromID(kPrefabID);
            if (!pkPrefabRoot)
            {
                char acMsg[512];
                NiSprintf(acMsg, 512, "Unable to resolve prefab root with "
                    "ID: \"%s\". The entity will be removed from the scene.",
                    (const char*) kPrefabID.ToString());
                ReportError(acMsg, m_acFilenameErrorMsg, pkPropIntf->GetName(),
                    NULL);
                return false;
            }
            // Make sure that prefab root matches up with the prefab root of 
            // the enity's master.  It is possible that this entity has been 
            // repurposed within the prefab.  If so, we want to remove this 
            // entity from the scene.  Do it now before all the dependencies 
            // have been created.
            NiEntityInterface* pkMaster = 
                ((NiEntityInterface*) pkPropIntf)->GetMasterEntity();
            if (!pkMaster)
            {
                ReportError("Prefab entity is missing its master entity.  "
                    "The entity will be removed from the scene", 
                    m_acFilenameErrorMsg, pkPropIntf->GetName(), NULL);
                return false;
            }

            // The master of our entity's prefab root should be the same
            // the prefab root of our entity's master.
            if (pkPrefabRoot->GetMasterEntity() != pkMaster->GetPrefabRoot())
            {
                ReportError("CONFLICT: Prefab entity's prefab root does not "
                    "corresponsd with the prefab root of the entity's "
                    "master.  The entity will be removed from the scene", 
                    m_acFilenameErrorMsg, pkPropIntf->GetName(), NULL);
                return false;
            }
        }
        ((NiEntityInterface*) pkPropIntf)->SetPrefabRoot(pkPrefabRoot, false);
    }
    else
    {
        // Unknown interface type.
        ReportError("Unknown object interface type.", m_acFilenameErrorMsg,
            pkPropIntf->GetName(), NULL);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadEntityOrComponentFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf)
{
    if (!ReadMasterFromDOM(kDOM, pkPropIntf))
    {
        return false;
    }
    if (!ReadPrefabRootFromDOM(kDOM, pkPropIntf))
    {
        return false;
    }

    const char* pcTags = kDOM.GetAttributeFromCurrent("Tags");
    if (pcTags)
    {
        if (pkPropIntf->GetInterfaceType() ==
            NiEntityInterface::IT_ENTITYINTERFACE)
        {
            NiEntityInterface* pkEntity = (NiEntityInterface*) pkPropIntf;
            pkEntity->SetTags(pcTags);
        }
        else
        {
            ReportError("Tags string found on non-entity object.",
                m_acFilenameErrorMsg, pkPropIntf->GetName(), NULL);
        }
    }

    if (kDOM.GetAttributeFromCurrent("TemplateID"))
    {
        // It is valid for no TemplateID to exist (e.g., for components).
        NiUniqueID kTemplateID;
        if (!ReadTemplateID(kDOM, kTemplateID))
        {
            return false;
        }
        pkPropIntf->SetTemplateID(kTemplateID);
    }

    const char* pcHidden = kDOM.GetAttributeFromCurrent("Hidden");
    if (pcHidden)
    {
        if (pkPropIntf->GetInterfaceType() == 
            NiEntityInterface::IT_ENTITYINTERFACE)
        {
            bool bHidden = false;
            if (NiStricmp(pcHidden, "TRUE") == 0)
                bHidden = true;

            NiEntityInterface* pkEntity = (NiEntityInterface*)pkPropIntf;
            pkEntity->SetHidden(bHidden);
        }
        else
        {
            ReportError("Hidden tag found on non-entity object.",
                m_acFilenameErrorMsg, pkPropIntf->GetName(), NULL);
        }
    }

    kDOM.SetSectionToFirstChild();

    // In case an entity does not have any properties or components...
    if (!kDOM.IsCurrentSectionValid())
    {
        if (VerifyEntityComponents((NiEntityInterface*)pkPropIntf))
            kDOM.SetModified(true);
        kDOM.EndSection();
        return true;
    }

    // Get the master if there is one
    NiEntityPropertyInterface* pkMasterPropIntf = NULL;
    if (pkPropIntf->GetInterfaceType() ==
        NiEntityComponentInterface::IT_COMPONENTINTERFACE)
    {    
        pkMasterPropIntf = 
            ((NiEntityComponentInterface*)pkPropIntf)->GetMasterComponent();
    }
    else if (pkPropIntf->GetInterfaceType() == 
        NiEntityInterface::IT_ENTITYINTERFACE)
    {
        pkMasterPropIntf = 
            ((NiEntityInterface*)pkPropIntf)->GetMasterEntity();
    }
    // [1] LINK COMPONENTS
    while (kDOM.IsCurrentSectionValid())
    {
        const char* pcValue = kDOM.GetValueFromCurrent();

        if (NiStricmp(pcValue, "COMPONENT") == 0)
        {
            // We must be dealing with an Entity that has components
            // if we enter this section.

            NiUniqueID kRefLinkID;
            if (!ReadUniqueID(kDOM, "RefLinkID", kRefLinkID))
            {
                ReportError("Error parsing component RefLinkID.",
                    m_acFilenameErrorMsg, pkPropIntf->GetName(), NULL);
                return false;
            }

            NiEntityComponentInterface* pkComp = NULL;
            if (kRefLinkID != NiUniqueID())
            {
                pkComp = (NiEntityComponentInterface*) GetFromID(kRefLinkID);
                if (!pkComp)
                {
                    const char* pcLostComponent = NULL;
                    unsigned int uiIndex;
                    if (m_kElementIdxMap.GetAt(&kRefLinkID, uiIndex))
                    {
                        NIASSERT(uiIndex < m_kElementSet.GetSize());
                        const IDElementItem& kItem = m_kElementSet.GetAt(uiIndex);
                        NIASSERT(kItem.m_kID == kRefLinkID);
                        pcLostComponent = kItem.m_pkElement->Attribute("Name");
                    }
                    char acMsg[512];
                    NiSprintf(acMsg, 512, "CONFLICT: Unable to resolve "
                        "component with ID: \"%s\". This component will be "
                        "removed from the entity.",
                        (const char*) kRefLinkID.ToString());
                    ReportError(acMsg, m_acFilenameErrorMsg,
                        pkPropIntf->GetName(), pcLostComponent);
                    kDOM.SetModified(true);
                }
            }

            if (pkComp)
            {
                // Component properties assumed
                if (!((NiEntityInterface*)pkPropIntf)->AddComponent(pkComp,
                    false))
                {
                    char acErrorMsg[1024];
                    NiSprintf(acErrorMsg, 1024, "Unable to add component "
                        "\"%s\" to entity.", (const char*) pkComp->GetName());
                    ReportError(acErrorMsg, m_acFilenameErrorMsg,
                        pkPropIntf->GetName(), NULL);
                    return false;
                }
            }
        }
        else if (NiStricmp(pcValue, "PROPERTY") == 0)
        {
            if (!ReadPropertyFromDOM(kDOM, pkPropIntf, pkMasterPropIntf))
            {
                return false;
            }
        }
        else
        {
            // Unknown tag in ENTITY section. Ignore and continue.
        }

        kDOM.SetSectionToNextSibling();
    }
    if (VerifyEntityComponents((NiEntityInterface*)pkPropIntf))
        kDOM.SetModified(true);

    kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadPropertyFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    NiEntityPropertyInterface* pkMasterPropIntf)
{
    // If pkPropIntf is a Entity interface, then it is assumed that
    // non-component properties are being read in.

    const char* pcClass = kDOM.GetAttributeFromCurrent("Class");
    const char* pcName = kDOM.GetAttributeFromCurrent("Name");

    if (!pcClass || !pcName)
    {
        ReportError("Error parsing PROPERTY tag.", m_acFilenameErrorMsg,
            pkPropIntf->GetName(), NULL);
        return false;
    }

    if (pkMasterPropIntf != NULL)
    {
        NiFixedString kPrimitive;
        if (pkMasterPropIntf->GetPrimitiveType(pcName, kPrimitive))
        {
            if (!kPrimitive.Equals(pcClass))
            {
                ReportError("CONFLICT: Property type mismatch with master "
                    "component.  Taking type and default value of master.",
                    m_acFilenameErrorMsg, pkPropIntf->GetName(), pcName);
                kDOM.SetModified(true);
                // Return true because we handle the read properly
                return true;
            }
        }
        else
        {
                ReportError("CONFLICT: Property no longer exists on master "
                    "component.  Removing property.",
                    m_acFilenameErrorMsg, pkPropIntf->GetName(), pcName);
                kDOM.SetModified(true);
                // Return true because we handle the read properly
                return true;
        }
    }

    if (pkPropIntf->IsAddPropertySupported())
    {
        const char* pcSemanticType = 
            kDOM.GetAttributeFromCurrent("SemanticType");
        const char* pcDisplayName = 
            kDOM.GetAttributeFromCurrent("DisplayName");
        const char* pcDescription = 
            kDOM.GetAttributeFromCurrent("Description");

        if (!pcSemanticType || !pcDisplayName || !pcDescription)
        {
            ReportError("Error parsing PROPERTY tag.",
                m_acFilenameErrorMsg, pkPropIntf->GetName(), pcName);
            return false;
        }

        // Property must be constructed.
        if (!ReadPropertyFromDOM(kDOM, pcClass, pcName, pcSemanticType, 
            pcDisplayName, pcDescription, pkPropIntf))
        {
            ReportError("Error reading property value.",
                m_acFilenameErrorMsg, pkPropIntf->GetName(), pcName);
            return false;
        }
    }
    else
    {
        // Handle each property appropriately
        if (!ReadPropertyFromDOM(kDOM, pcClass, pcName, pkPropIntf))
        {
            ReportError("Error reading property value.",
                m_acFilenameErrorMsg, pkPropIntf->GetName(), pcName);
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadPropertyFromDOM(
    NiDOMTool& kDOM,
    const char* pcPrimitive,
    const char* pcName,
    const char* pcSemanticType, 
    const char* pcDisplayName,
    const char* pcDescription,
    NiEntityPropertyInterface* pkPropIntf)
{
    pkPropIntf->AddProperty(pcName, pcDisplayName, pcPrimitive,
        pcSemanticType, pcDescription);

    return ReadPropertyFromDOM(kDOM, pcPrimitive, pcName, pkPropIntf);
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadPropertyArrayFromDOM(
    NiDOMTool& kDOM,
    const char* pcPrimitive,
    const char* pcName, 
    NiEntityPropertyInterface* pkPropIntf)
{
    unsigned int ui=0;
    // Section is set at ITEM...

    NIASSERT(kDOM.IsCurrentSectionValid());
    if( pkPropIntf->IsAddPropertySupported())
    {
        pkPropIntf->MakeCollection(pcName, true);
    }
    while (kDOM.IsCurrentSectionValid())
    {
        const char* pcValue = kDOM.GetValueFromCurrent();

        if (NiStricmp(pcValue, "ITEM") != 0)
        {
            kDOM.SetSectionToNextSibling();
            continue;
        }

        NiBool bSuccess;

        if (kDOM.IsCurrentLeaf() && 
            kDOM.GetAttributeCountFromCurrent() == 0)
        {
            // Special case where <ITEM/> represents an empty array
            bSuccess = true;
        }
        else if (NiStricmp(pcPrimitive, PT_ENTITYPOINTER) == 0)
            bSuccess = ReadEntityPointerFromDOM(kDOM, pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_NIOBJECTPOINTER) == 0)
            bSuccess = ReadObjectPointerFromDOM(kDOM, pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_FLOAT) == 0)
            bSuccess = ReadFloatFromDOM(kDOM, pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_BOOL) == 0)
            bSuccess = ReadBoolFromDOM(kDOM, pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_INT) == 0)
            bSuccess = ReadIntFromDOM(kDOM, pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_UINT) == 0)
            bSuccess = ReadUIntFromDOM(kDOM, pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_SHORT) == 0)
            bSuccess = ReadShortFromDOM(kDOM, pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_USHORT) == 0)
            bSuccess = ReadUShortFromDOM(kDOM, pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_STRING) == 0)
            bSuccess = ReadStringFromDOM(kDOM, pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_POINT2) == 0)
            bSuccess = ReadPoint2FromDOM(kDOM, pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_POINT3) == 0)
            bSuccess = ReadPoint3FromDOM(kDOM, pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_QUATERNION) == 0)
            bSuccess = ReadQuaternionFromDOM(kDOM, pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_MATRIX3) == 0)
            bSuccess = ReadMatrix3FromDOM(kDOM, pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_COLOR) == 0)
            bSuccess = ReadColorFromDOM(kDOM, pkPropIntf, pcName, ui);
        else if (NiStricmp(pcPrimitive, PT_COLORA) == 0)
            bSuccess = ReadColorAFromDOM(kDOM, pkPropIntf, pcName, ui);
        else
            bSuccess = ReadUnknownTypeFromDOM(kDOM, pkPropIntf, pcName, ui);

        if (!bSuccess)
            return false;

        kDOM.SetSectionToNextSibling();
        ui++;
    }

    kDOM.EndSection();

    return true;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadPropertyFromDOM(
    NiDOMTool& kDOM,
    const char* pcPrimitive,
    const char* pcName,
    NiEntityPropertyInterface* pkPropIntf)
{
    if (kDOM.SetSectionTo("ITEM"))
        return ReadPropertyArrayFromDOM(kDOM, pcPrimitive, pcName, pkPropIntf);
    
    kDOM.EndSection();

    NiBool bSuccess;

    if (NiStricmp(pcPrimitive, PT_ENTITYPOINTER) == 0)
        bSuccess = ReadEntityPointerFromDOM(kDOM, pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_NIOBJECTPOINTER) == 0)
        bSuccess = ReadObjectPointerFromDOM(kDOM, pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_FLOAT) == 0)
        bSuccess = ReadFloatFromDOM(kDOM, pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_BOOL) == 0)
        bSuccess = ReadBoolFromDOM(kDOM, pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_INT) == 0)
        bSuccess = ReadIntFromDOM(kDOM, pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_UINT) == 0)
        bSuccess = ReadUIntFromDOM(kDOM, pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_SHORT) == 0)
        bSuccess = ReadShortFromDOM(kDOM, pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_USHORT) == 0)
        bSuccess = ReadUShortFromDOM(kDOM, pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_STRING) == 0)
        bSuccess = ReadStringFromDOM(kDOM, pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_POINT2) == 0)
        bSuccess = ReadPoint2FromDOM(kDOM, pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_POINT3) == 0)
        bSuccess = ReadPoint3FromDOM(kDOM, pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_QUATERNION) == 0)
        bSuccess = ReadQuaternionFromDOM(kDOM, pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_MATRIX3) == 0)
        bSuccess = ReadMatrix3FromDOM(kDOM, pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_COLOR) == 0)
        bSuccess = ReadColorFromDOM(kDOM, pkPropIntf, pcName);
    else if (NiStricmp(pcPrimitive, PT_COLORA) == 0)
        bSuccess = ReadColorAFromDOM(kDOM, pkPropIntf, pcName);
    else
        bSuccess = ReadUnknownTypeFromDOM(kDOM, pkPropIntf, pcName);

    return bSuccess;
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::Save(
    const char* pcFileName,
    unsigned int uiSceneIdx)
{
#if defined(_XENON) || defined(_PS3) || defined(_WII)
    ReportError("The current platform does not support saving GSA files.",
        pcFileName, NULL, NULL);
    return false;
#else
    Flush();

    if (!StoreWorkingPath(pcFileName))
    {
        return false;
    }

    if (GetSceneCount() <= uiSceneIdx)
    {
        char acMsg[256];
        NiSprintf(acMsg, 256, "The requested scene index %d does not exist; "
            "the GSA file cannot be saved.", uiSceneIdx);
        ReportError(acMsg, m_acFilenameErrorMsg, NULL, NULL);
        return false;
    }

    // Create a link map just for saving. This is to sort
    IDLinkMap kIDMap;
    GatherInformation(kIDMap, m_kFilenameSet, uiSceneIdx);

    //-- Initialize
    NiDOMToolPtr spDOM = NiNew NiDOMTool(m_kFilenameSet.GetCurrentFilename());

    //-- Write to DOM
    WriteToDOM(*spDOM, uiSceneIdx, kIDMap);

    //-- Write to disk
    NiBool bSuccess = spDOM->SaveFile();
    if (!bSuccess)
    {
        ReportError("An error occurred when saving the GSA file.",
            m_acFilenameErrorMsg, NULL, NULL);
    }

    return bSuccess;
#endif
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteToDOM(
    NiDOMTool& kDOM,
    unsigned int uiSceneIdx,
    IDLinkMap& kIDMap)
{
    NIASSERT(uiSceneIdx < GetSceneCount());

    kDOM.WriteHeader();

    kDOM.BeginSection("GSA");

    WriteVersionToDOM(kDOM);

    WriteXRefsToDOM(kDOM, true);

    WriteXRefsToDOM(kDOM, false);

    WriteSceneToDOM(kDOM, GetSceneAt(uiSceneIdx));

    WriteMapObjectsToDOM(kDOM, kIDMap);

    kDOM.EndSection(); // End "GSA"
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteXRefsToDOM(NiDOMTool& kDOM, bool bLayer)
{
    if (bLayer)
    {
        kDOM.BeginSection("LAYERS");
    }
    else
    {
        kDOM.BeginSection("DEPENDENCIES");
    }

    const unsigned int uiFilenameCount = m_kFilenameSet.GetCount();
    for (unsigned int ui = 0; ui < uiFilenameCount; ++ui)
    {
        bool bIsLayer = m_kFilenameSet.IsLayer(ui);
        if (bIsLayer != bLayer)
        {
            continue;
        }

        kDOM.BeginSection("FILENAME");

        WriteFilenameToDOM(kDOM, m_kFilenameSet.GetAt(ui));

        if (bLayer)
        {
            // Only write the explicit attribute to filenames in the LAYERS
            // section.
            bool bExplicit = m_kFilenameSet.IsExplicit(ui);
            kDOM.AssignAttribute("Explicit", bExplicit ? "TRUE" : "FALSE");
        }

        kDOM.EndSection();
    }

    kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteFilenameToDOM(
    NiDOMTool& kDOM,
    const NiFixedString& kFilename)
{
    char acRelativePath[NI_MAX_PATH];
    size_t stBytes = NiPath::ConvertToRelative(acRelativePath,
        NI_MAX_PATH, kFilename, kDOM.GetFilePath());
    if (stBytes == 0)
    {
        // NiPath::ConvertToRelative could have failed if pcKey was
        // a network path or if the path was another drive letter.
        // In either case, we can only store the absolute path.
        char acStdAbsPath[NI_MAX_PATH];
        NiStrcpy(acStdAbsPath, NI_MAX_PATH, kFilename);
        NiPath::Standardize(acStdAbsPath);
        NiStrcpy(acRelativePath, NI_MAX_PATH, acStdAbsPath);
    }

    NiFixedString kRelativePath(acRelativePath);
    kDOM.WritePrimitive(kRelativePath);
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteMapObjectsToDOM(
    NiDOMTool& kDOM,
    IDLinkMap& kIDMap)
{
    kDOM.BeginSection("OBJECTS");

    NiTMapIterator kIter = kIDMap.GetFirstPos();
    while (kIter)
    {
        NiUniqueID kID;
        NiEntityPropertyInterface* pkPropIntf = NULL;
        NiScene* pkScene = NULL;
        kIDMap.GetNext(kIter, kID, pkPropIntf, pkScene);
        NIASSERT(pkPropIntf && pkPropIntf->GetID() == kID);

        // Do not write out entities or components whose source filenames are
        // not the current file.
        const NiFixedString& kSourceFilename = pkPropIntf->GetSourceFilename();
        if (kSourceFilename.Exists() &&
            kSourceFilename != m_kFilenameSet.GetCurrentFilename())
        {
            continue;
        }

        if (pkPropIntf->GetInterfaceType() ==
            NiEntityComponentInterface::IT_COMPONENTINTERFACE)
        {    
            WriteComponentToDOM(kDOM,
                (NiEntityComponentInterface*) pkPropIntf);
        }
        else if (pkPropIntf->GetInterfaceType() ==
            NiEntityInterface::IT_ENTITYINTERFACE)
        {
            WriteEntityToDOM(kDOM, (NiEntityInterface*) pkPropIntf);
        }
        else
        {
            NIASSERT(!"Unknown Interface Type!");
        }
    }

    kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteSceneToDOM(NiDOMTool& kDOM, NiScene* pkScene)
{
    kDOM.BeginSection("SCENE");
    kDOM.AssignAttribute("Class", "NiScene");
    kDOM.AssignAttribute("Name", pkScene->GetName());

    WriteSelectionSetsToDOM(kDOM, pkScene);

    // Process each entity
    unsigned int uiEntityCount = pkScene->GetEntityCount();
    for (unsigned int uiEntity = 0; uiEntity < uiEntityCount; uiEntity++)
    {
        NiEntityInterface* pkEntity = pkScene->GetEntityAt(uiEntity);
        if (pkEntity->GetSourceFilename() == kDOM.GetFilename())
        {
            WriteEntityRefToDOM(kDOM, pkEntity);
        }
    }

    kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteEntityRefToDOM(
    NiDOMTool& kDOM,
    NiEntityInterface* pkEntity)
{
    if (pkEntity == NULL)
    {
        kDOM.BeginSection("ENTITY");
        kDOM.AssignAttribute("RefLinkID", "NULL");
        kDOM.EndSection();
        return;
    }

    kDOM.BeginSection("ENTITY");
    WriteUniqueIDToDOM(kDOM, "RefLinkID", pkEntity->GetID());
    kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteTemplateIDToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf)
{
    NiUniqueID kUniqueID = pkPropIntf->GetTemplateID();
    if (!pkPropIntf->SetTemplateID(kUniqueID))
        return;

    WriteUniqueIDToDOM(kDOM, "TemplateID", kUniqueID);
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteUniqueIDToDOM(
    NiDOMTool& kDOM,
    const char* pcAttributeName,
    const NiUniqueID& kUniqueID)
{
    NIASSERT(pcAttributeName);
    kDOM.AssignAttribute(pcAttributeName, kUniqueID.ToString());
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteEntityToDOM(
    NiDOMTool& kDOM,
    NiEntityInterface* pkEntity)
{
    kDOM.BeginSection("ENTITY");
    WriteUniqueIDToDOM(kDOM, "LinkID", pkEntity->GetID());
    kDOM.AssignAttribute("Class", pkEntity->GetClassName());
    kDOM.AssignAttribute("Name", pkEntity->GetName());

    WriteTemplateIDToDOM(kDOM, pkEntity);

    if (pkEntity->GetHidden())
    {
        kDOM.AssignAttribute("Hidden", "TRUE");
    }

    NiEntityInterface* pkMasterEntity = pkEntity->GetMasterEntity();
    if (pkMasterEntity)
    {
        WriteUniqueIDToDOM(kDOM, "MasterLinkID", pkMasterEntity->GetID());
    }

    NiEntityInterface* pkPrefabRoot = pkEntity->GetPrefabRoot();
    if (pkPrefabRoot)
    {
        WriteUniqueIDToDOM(kDOM, "PrefabID", pkPrefabRoot->GetID());
    }

    const NiFixedString& kTags = pkEntity->GetTags();
    if (kTags.Exists())
    {
        kDOM.AssignAttribute("Tags", kTags);
    }

    unsigned int uiComponentCount = pkEntity->GetComponentCount();

    for (unsigned int uiComp = 0; uiComp < uiComponentCount; uiComp++)
    {
        NiEntityComponentInterface* pkComp = pkEntity->GetComponentAt(uiComp);

        if (pkComp == NULL)
        {
            kDOM.BeginSection("COMPONENT");
            kDOM.AssignAttribute("RefLinkID", "NULL");
            kDOM.EndSection();
            continue;
        }

        kDOM.BeginSection("COMPONENT");
        WriteUniqueIDToDOM(kDOM, "RefLinkID", pkComp->GetID());
        kDOM.EndSection();
    }

    // Write properties
    NiTObjectSet<NiFixedString> kPropertyNames;
    pkEntity->GetPropertyNames(kPropertyNames);

    unsigned int uiNameCount = kPropertyNames.GetSize();
    for (unsigned int uiProp = 0; uiProp < uiNameCount; uiProp++)
    {
        const NiFixedString& kPropertyName = kPropertyNames.GetAt(uiProp);

        bool bIsCompProp = true;
        if (!pkEntity->IsComponentProperty(kPropertyName, bIsCompProp) ||
            bIsCompProp)
        {
            continue;
        }

        bool bSerializable = false;
        if (!pkEntity->IsPropertySerializable(kPropertyName, bSerializable) ||
            !bSerializable)
        {
            continue;
        }

        WritePropertyToDOM(kDOM, pkEntity, kPropertyName);
    }

    kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteComponentToDOM(
    NiDOMTool& kDOM,
    NiEntityComponentInterface* pkComp)
{
    kDOM.BeginSection("COMPONENT");
    WriteUniqueIDToDOM(kDOM, "LinkID", pkComp->GetID());
    kDOM.AssignAttribute("Class", pkComp->GetClassName());
    kDOM.AssignAttribute("Name", pkComp->GetName());

    WriteTemplateIDToDOM(kDOM, pkComp);

    NiEntityComponentInterface* pkMasterComp = pkComp->GetMasterComponent();
    if (pkMasterComp)
    {
        WriteUniqueIDToDOM(kDOM, "MasterLinkID", pkMasterComp->GetID());
    }

    // Write properties
    NiTObjectSet<NiFixedString> kPropertyNames;
    pkComp->GetPropertyNames(kPropertyNames);

    unsigned int uiNameCount = kPropertyNames.GetSize();
    for (unsigned int uiProp = 0; uiProp < uiNameCount; uiProp++)
    {
        const NiFixedString kPropertyName = kPropertyNames.GetAt(uiProp);

        bool bSerializable = false;
        if (!pkComp->IsPropertySerializable(kPropertyName, bSerializable) ||
            !bSerializable)
        {
            continue;
        }

        WritePropertyToDOM(kDOM, pkComp, kPropertyName);
    }

    kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteVersionToDOM(NiDOMTool& kDOM)
{
    kDOM.AssignAttribute("Major", VERSION_MAJOR);
    kDOM.AssignAttribute("Minor", VERSION_MINOR);
    kDOM.AssignAttribute("Patch", VERSION_PATCH);
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WriteSelectionSetsToDOM(
    NiDOMTool& kDOM,
    NiScene* pkScene)
{
    kDOM.BeginSection("SELECTIONS");

    const unsigned uiSelectionSetCount = pkScene->GetSelectionSetCount();
    for (unsigned int uiSet=0; uiSet < uiSelectionSetCount; uiSet++)
    {
        NiEntitySelectionSet* pkSet = pkScene->GetSelectionSetAt(uiSet);
        
        unsigned int uiEntityCount = pkSet->GetEntityCount();
        NIASSERT(uiEntityCount);

        kDOM.BeginSection("SET");

        const char* pcName = pkSet->GetName();
        kDOM.AssignAttribute("Name", pcName);

        for(unsigned int uiEntity=0; uiEntity < uiEntityCount; uiEntity++)
        {
            NiEntityInterface* pkEntity = pkSet->GetEntityAt(uiEntity);
            NIASSERT(pkEntity);
            WriteEntityRefToDOM(kDOM, pkEntity);
        }

        kDOM.EndSection();
    }

    kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WritePropertyArrayToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName,
    NiFixedString& kPrimitiveType,
    unsigned int uiCount)
{
    if (uiCount == 0)
    {
        kDOM.BeginSection("ITEM");
        kDOM.EndSection();
        //WritePropertyToDOM(pkPropIntf, kPropertyName, kPrimitiveType, 0);
        return;
    }
    for(unsigned int ui=0; ui<uiCount; ui++)
    {
        kDOM.BeginSection("ITEM");

        WritePropertyToDOM(kDOM, pkPropIntf, kPropertyName, kPrimitiveType,
            ui);
        kDOM.EndSection();
    }
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WritePropertyToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName, 
    NiFixedString& kPrimitiveType,
    unsigned int uiIndex)
{
    if (kPrimitiveType == PT_ENTITYPOINTER)
        WriteEntityPointerToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_NIOBJECTPOINTER)
        WriteObjectPointerToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_FLOAT)
        WriteFloatToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_BOOL)
        WriteBoolToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_INT)
        WriteIntToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_UINT)
        WriteUIntToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_SHORT)
        WriteShortToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_USHORT)
        WriteUShortToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_STRING)
        WriteStringToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_POINT2)
        WritePoint2ToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_POINT3)
        WritePoint3ToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_QUATERNION)
        WriteQuaternionToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_MATRIX3)
        WriteMatrix3ToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_COLOR)
        WriteColorToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else if (kPrimitiveType == PT_COLORA)
        WriteColorAToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
    else
        WriteUnknownTypeToDOM(kDOM, pkPropIntf, kPropertyName, uiIndex);
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WritePropertyToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName)
{
    kDOM.BeginSection("PROPERTY");

    WritePropertyInfoToDOM(kDOM, pkPropIntf, kPropertyName);

    NiFixedString kPrimitiveType;
    NIVERIFY(NIBOOL_IS_TRUE(pkPropIntf->GetPrimitiveType(kPropertyName, 
        kPrimitiveType)));

    bool bIsCollection;

    if (pkPropIntf->IsCollection(kPropertyName, bIsCollection) 
        && bIsCollection)
    {
        unsigned int uiCount;
        pkPropIntf->GetElementCount(kPropertyName, uiCount);

         WritePropertyArrayToDOM(kDOM, pkPropIntf, kPropertyName,
             kPrimitiveType, uiCount);
    }
    else
    {
        WritePropertyToDOM(kDOM, pkPropIntf, kPropertyName, kPrimitiveType, 0);
    }

    kDOM.EndSection();
}
//---------------------------------------------------------------------------
void NiEntityStreamingAscii::WritePropertyInfoToDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const NiFixedString& kPropertyName)
{
    NiFixedString kDisplayName;
    NiFixedString kPrimitiveType;
    NiFixedString kSemanticType;
    NiFixedString kDescription;

    if (pkPropIntf->GetPrimitiveType(kPropertyName, kPrimitiveType) == false)
    {
        NIASSERT(!"Warning: No Primitive Type!");
        return;
    }


    kDOM.AssignAttribute("Class",kPrimitiveType);
    kDOM.AssignAttribute("Name",kPropertyName);

    if (pkPropIntf->IsAddPropertySupported())
    {
        if (pkPropIntf->GetSemanticType(kPropertyName, kSemanticType) == false)
        {
            NIASSERT(!"Warning: No Semantic Type!");
            return;
        }

        if (pkPropIntf->GetDisplayName(kPropertyName, kDisplayName) == false)
        {
            NIASSERT(!"Warning: No Display Name!");
            return;
        }

        if (pkPropIntf->GetDescription(kPropertyName, kDescription) == false)
        {
            NIASSERT(!"Warning: No Description!");
            return;
        }
        kDOM.AssignAttribute("SemanticType",kSemanticType);
        kDOM.AssignAttribute("DisplayName",kDisplayName);
        kDOM.AssignAttribute("Description",kDescription);
    }
}
//---------------------------------------------------------------------------
NiBool NiEntityStreamingAscii::ReadUnknownTypeFromDOM(
    NiDOMTool& kDOM,
    NiEntityPropertyInterface* pkPropIntf,
    const char* pcName, 
    unsigned int uiIndex)
{
    size_t stDataSizeInBytes = 0;
    unsigned char* pucData = NULL;
    NiBool bSuccess = kDOM.ReadPrimitive(pucData, stDataSizeInBytes);

    if (!bSuccess || pucData == NULL || stDataSizeInBytes == 0)
        return false;

    bSuccess = pkPropIntf->SetPropertyData(pcName, (void*)pucData, 
        stDataSizeInBytes, uiIndex);
    if (bSuccess)
    {
        bool bMadeUnique = false;
        pkPropIntf->MakePropertyUnique(pcName, bMadeUnique);
    }
    NiFree(pucData);
    return bSuccess;
}
//---------------------------------------------------------------------------
NiEntityPropertyInterface* NiEntityStreamingAscii::GetFromID(
    const NiUniqueID& kID)
{
    NiEntityPropertyInterface* pkPropIntf = m_kLinkMap.Get(kID);
    if (!pkPropIntf)
    {
        unsigned int uiIndex;
        if (m_kElementIdxMap.GetAt(&kID, uiIndex))
        {
            NIASSERT(uiIndex < m_kElementSet.GetSize());
            const IDElementItem& kItem = m_kElementSet.GetAt(uiIndex);
            NIASSERT(kItem.m_kID == kID);

            pkPropIntf = CreateAndMapObject(
                kItem.m_pkElement->Attribute("Class"),
                kItem.m_pkElement->Attribute("Name"),
                kID,
                kItem.m_pkDOM->GetFilename());
            if (pkPropIntf)
            {
                NIASSERT(m_kLinkMap.Get(kID) == pkPropIntf);

                kItem.m_pkDOM->PushElement(kItem.m_pkElement);
                if (!ReadEntityOrComponentFromDOM(*kItem.m_pkDOM, pkPropIntf))
                {
                    kItem.m_pkDOM->EndSection();
                    m_kLinkMap.Remove(kID);
                    return NULL;
                }
                kItem.m_pkDOM->EndSection();
            }
        }
    }

    return pkPropIntf;
}
//---------------------------------------------------------------------------
NiEntityStreamingAscii::IDElementItem::IDElementItem() :
    m_pkDOM(NULL),
    m_pkElement(NULL)
{
}
//---------------------------------------------------------------------------
NiEntityStreamingAscii::IDElementItem::IDElementItem(
    const NiUniqueID& kID,
    NiDOMTool* pkDOM,
    TiXmlElement* pkElement) :
    m_kID(kID),
    m_pkDOM(pkDOM),
    m_pkElement(pkElement)
{
}
//---------------------------------------------------------------------------
