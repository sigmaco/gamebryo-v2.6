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

#include "NiTerrainPCH.h"
#include "NiSurfacePalette.h"

#include "NiMetaDataStore.h"

NiSurfacePalettePtr NiSurfacePalette::ms_spInstance = NULL;

//---------------------------------------------------------------------------
NiSurfacePalette* NiSurfacePalette::GetInstance()
{
    NIASSERT(ms_spInstance && "NiSurfacePalette not initialized!");
    return ms_spInstance;
}
//---------------------------------------------------------------------------
void NiSurfacePalette::CreateInstance()
{
    NIASSERT(ms_spInstance== NULL && "NiSurfacePalette ALREADY! initialized!");
    ms_spInstance = NiNew NiSurfacePalette();
}
//---------------------------------------------------------------------------
void NiSurfacePalette::DestroyInstance()
{
    NIASSERT(ms_spInstance && "NiSurfacePalette not initialized!");
    ms_spInstance = NULL;
}
//---------------------------------------------------------------------------
NiSurfacePalette::NiSurfacePalette() :
    m_kTagIndex(61),
    m_kPackageMap(17),
    m_kPackageFilenameMap(17)
{
}
//---------------------------------------------------------------------------
NiSurfacePalette::~NiSurfacePalette()
{
    //Loop through all the loaded packages and remove them:
    NiTMapIterator kIterator = m_kPackageMap.GetFirstPos();
    NiSurfacePackage* pkCurrentPackage;
    const char* pcKey;

    while (kIterator)
    {
        m_kPackageMap.GetNext(kIterator, pcKey, pkCurrentPackage);
        NiDelete pkCurrentPackage;
    }

    // Loop through all the index lists and remove them. The surfaces 
    // are removed by the packages
    NiTMapIterator kSurfaceIter = m_kTagIndex.GetFirstPos();
    NiTPointerList<NiSurface*>* pkSurfaceList;

    while (kSurfaceIter)
    {
        m_kTagIndex.GetNext(kSurfaceIter, pcKey, pkSurfaceList);
        NiDelete pkSurfaceList;
    }
}
//---------------------------------------------------------------------------
NiSurface* NiSurfacePalette::GetSurface(const NiFixedString& kPackage,
    const NiFixedString& kName) const
{
    NiSurfacePackage* pkPackage = GetPackage(kPackage);
    if (!pkPackage) 
    {
        return 0;
    }
    
    NiSurface* pkSurface;
    if (pkPackage->GetSurface(kName, pkSurface))
    {
        return pkSurface;
    }
    else
    {
        return 0;
    }    
}
//---------------------------------------------------------------------------
NiSurface* NiSurfacePalette::CreateSurface(const NiFixedString& kPackage,
    const NiFixedString& kName, NiMetaDataStore* pkMetaDataStore)
{
    // Ensure the package exists
    NiSurfacePackage* pkPackage = GetPackage(kPackage);
    if (!pkPackage) 
    {
        return 0;
    }
    
    // Surface already exist?
    NiSurface* pkSurface;
    if (pkPackage->GetSurface(kName, pkSurface))
    {
        return pkSurface;
    }

    // Create the surface
    pkPackage->CreateSurface(kName, pkSurface, pkMetaDataStore);

    return pkSurface;    
}
//---------------------------------------------------------------------------
NiSurface* NiSurfacePalette::CreateSurfaceFrom(
    const NiFixedString& kNewPackage, const NiFixedString& kNewName,
    const NiSurface* pkSource)
{
    NiSurface* pkNewSurface = CreateSurface(
        kNewPackage, kNewName
        );

    pkNewSurface->CopySettings(pkSource);

    return pkNewSurface;
}
//---------------------------------------------------------------------------
void NiSurfacePalette::UnloadSurface(const NiFixedString& kPackage,
    const NiFixedString& kName)
{
    NiSurfacePackage* pkPackage = GetPackage(kPackage);
    if (!pkPackage) 
    {
        return;
    }

    pkPackage->UnloadSurface(kName);
}
//---------------------------------------------------------------------------
NiSurfacePackage* NiSurfacePalette::GetPackage(const NiFixedString& kPackage) 
    const
{
    NiSurfacePackage* pkValue;
    if (m_kPackageMap.GetAt(kPackage, pkValue))
    {
        return pkValue;
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
void NiSurfacePalette::GetLoadedPackages(
    NiTPrimitiveSet<NiSurfacePackage*>& kPackages) const
{
    NiTMapIterator kIterator = m_kPackageMap.GetFirstPos();
    NiSurfacePackage* pkCurrentPackage;
    const char* pcKey;

    while (kIterator)
    {
        m_kPackageMap.GetNext(kIterator, pcKey, pkCurrentPackage);
        kPackages.Add(pkCurrentPackage);
    }
}
//---------------------------------------------------------------------------
const NiSurfacePackage* NiSurfacePalette::LoadPackage(
    const char* pcPackageRelFile, NiMetaDataStore* pkMetaDataStore)
{
    char* pcPathBuffer = NiAlloc(char, 512);
    NiString pcPackageFile;

    NiStrncpy(pcPathBuffer, 512, pcPackageRelFile, NI_TRUNCATE);    
    if (NiPath::IsRelative(pcPathBuffer))
    {
        // Make sure the package file is an absolute path, not relative
        // Absolute to current working directory, that is where we
        // are expected to get it from anyways.        
        NiPath::ConvertToAbsolute(pcPathBuffer, 512, pcPackageRelFile, 0);
    }

    NiPath::RemoveDotDots(pcPathBuffer);
    pcPackageFile = pcPathBuffer;
    NiFree(pcPathBuffer);

    NiSurfacePackage* pkSurfacePackage;

    // Have we already loaded a package from this path?
    if (m_kPackageFilenameMap.GetAt(pcPackageFile, pkSurfacePackage)) 
    {
        // Already loaded a file with this path.        
        return pkSurfacePackage;
    }

    if (!pkMetaDataStore)
        pkMetaDataStore = NiMetaDataStore::GetInstance();
    
    pkSurfacePackage = NiNew NiSurfacePackage();
    bool bLoaded = pkSurfacePackage->Load(pcPackageFile, pkMetaDataStore);
    if (!bLoaded)
    {
        NiDelete pkSurfacePackage;
        return NULL;
    }

    // Does a package already exist with this name?
    NiSurfacePackage* pkExistingPackage = 0;
    if (m_kPackageMap.GetAt(pkSurfacePackage->GetName(), pkExistingPackage)) 
    {
        NiDelete pkSurfacePackage;
        return pkExistingPackage;
    }

    // Add it to the maps
    m_kPackageFilenameMap.SetAt(pkSurfacePackage->GetFilename(), 
        pkSurfacePackage);
    m_kPackageMap.SetAt(pkSurfacePackage->GetName(), pkSurfacePackage);

    // Add the surfaces to the tag index table
    NiTMapIterator kIterator = pkSurfacePackage->m_kSurfaces.GetFirstPos();
    
    while (kIterator != NULL)
    {
        NiSurface* pkSurfaceToIndex;
        const char* pcKey;
        pkSurfacePackage->m_kSurfaces.GetNext(kIterator, pcKey, 
            pkSurfaceToIndex);
        AddIndexedSurface(pkSurfaceToIndex);
    }

    return pkSurfacePackage;
}
//---------------------------------------------------------------------------
void NiSurfacePalette::CompilePackage(const NiFixedString&)
{
    NiTMapIterator kIterator = m_kPackageMap.GetFirstPos();
    NiSurfacePackage* pkCurrentPackage;
    const char* pcKey;

    while (kIterator)
    {
        m_kPackageMap.GetNext(kIterator, pcKey, pkCurrentPackage);
        pkCurrentPackage->PrecompileSurfaces();
    }
}
//---------------------------------------------------------------------------
bool NiSurfacePalette::SavePackage(const NiFixedString& kPackage)
{
    NiSurfacePackage* pkPackage = GetPackage(kPackage);
    if (!pkPackage)
    {
        return false;
    }

    pkPackage->Save();

    return true;
}
//---------------------------------------------------------------------------
void NiSurfacePalette::UnloadPackage(const NiFixedString& kPackage)
{
    NiSurfacePackage* pkPackage = GetPackage(kPackage);
    if (!pkPackage)
    {
        return;
    }

    NiFixedString kSurfaceName; 
    NiTPrimitiveSet<NiSurface*> kSurfaces;
    pkPackage->GetLoadedSurfaces(kSurfaces);
    for(NiUInt32 i = 0; i < kSurfaces.GetSize(); ++i)
    {
        RemoveIndexedSurface(kSurfaces.GetAt(i));
    }
    pkPackage->UnloadSurfaces();

    m_kPackageFilenameMap.RemoveAt(pkPackage->GetFilename());
    m_kPackageMap.RemoveAt(kPackage);

    NiDelete pkPackage;
}
//---------------------------------------------------------------------------
NiUInt32 NiSurfacePalette::GetIndex(NiTList<NiFixedString>& kTags)
{
    const char* kCurKey;
    NiTPointerList<NiSurface*> *pkKeyValue;

    // Iterate over the map and add all the key values:
    NiTMapIterator kIter = m_kTagIndex.GetFirstPos();
    while (kIter != NULL)
    {
        m_kTagIndex.GetNext(kIter, kCurKey, pkKeyValue);

        kTags.AddHead(kCurKey);
    }

    return kTags.GetSize();
}
//---------------------------------------------------------------------------
bool NiSurfacePalette::AddIndexedSurface(NiSurface* pkSurface)
{
    NiTObjectSet<NiFixedString> kTags;
    NiUInt32 uiNumTags;

    uiNumTags = pkSurface->GetTags(kTags);

    if (uiNumTags != 0)
    {        
        NiUInt32 uiSize = kTags.GetSize();
        for (NiUInt32 ui = 0; ui < uiSize; ++ui)
        {
            const NiFixedString& kTag = kTags.GetAt(ui);
            
            //Loops through all the tags in the surface
            NiTPointerList<NiSurface*> * pkSurfaceList;

            if (m_kTagIndex.GetAt(kTag, pkSurfaceList))
            {
                // Check the surface isn't already there
                NiTListIterator kSurfaceIter = pkSurfaceList->GetHeadPos();
                bool bAlreadyIndexed = false;

                while (kSurfaceIter)
                {
                    NiSurface* pkTemp = pkSurfaceList->GetNext(kSurfaceIter);
                    if (pkTemp->GetName() == pkSurface->GetName())
                    {
                        bAlreadyIndexed = true;
                        break;
                    }

                }

                // Adding surface to an existing tag
                if (!bAlreadyIndexed)
                {
                    pkSurfaceList->AddTail(pkSurface);
                }
            }
            else
            {
                // Add the key and a new list containing the surface
                pkSurfaceList = NiNew NiTPointerList<NiSurface*>;
                pkSurfaceList->AddHead(pkSurface);
                
                m_kTagIndex.SetAt(kTag, pkSurfaceList);
            }
        }
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
NiUInt32 NiSurfacePalette::GetSurfaces(NiTList<NiFixedString>& kTags, 
        NiTPrimitiveSet<NiSurface*>& kSurfaces, bool bAndOperation)
{
    NiTListIterator kTagIter = kTags.GetHeadPos();
    bool bFirstPass = true;

    while (kTagIter)
    {
        NiTPrimitiveSet<NiSurface*> kTagSurfaceSet;

        // For each tag in the list, we build up the returned list
        const NiFixedString kTag = kTags.GetNext(kTagIter);
        GetSurfaces(kTag, kTagSurfaceSet);

        // Decide whether to AND/OR these tags together:
        if(bAndOperation && !bFirstPass)
        {
            for (NiUInt32 i = 0; i < kSurfaces.GetSize(); ++i)
            {
                NiSurface* pkSurface = kSurfaces.GetAt(i);
                int index = kTagSurfaceSet.Find(pkSurface);
                if(index < 0)
                {
                    kSurfaces.RemoveAt(i);
                }
            }
        }
        else
        {
            for (NiUInt32 i = 0; i < kTagSurfaceSet.GetSize(); ++i)
            {
                NiSurface* pkSurface = kTagSurfaceSet.GetAt(i);
                kSurfaces.Add(pkSurface);    
            }
        }

        bFirstPass = false;
    }

    return kSurfaces.GetSize();
}

//---------------------------------------------------------------------------
NiUInt32 NiSurfacePalette::GetSurfaces(const NiFixedString& kTag, 
        NiTPrimitiveSet<NiSurface*>& kSurfaces)
{
    NiTPointerList<NiSurface*>* pkSurfaceList = 0;
    
    m_kTagIndex.GetAt(kTag,pkSurfaceList);
    if (pkSurfaceList)
    {
        NiTListIterator kSurfaceIter = pkSurfaceList->GetHeadPos();

        while (kSurfaceIter)
        {
            NiSurface* pkToAdd = pkSurfaceList->GetNext(kSurfaceIter);
            if (kSurfaces.Find(pkToAdd) == -1)
            {
                kSurfaces.Add(pkToAdd);
            }
        }
        
        return pkSurfaceList->GetSize();
    }

    return 0;
}

//---------------------------------------------------------------------------
bool NiSurfacePalette::RemoveIndexedSurface(NiSurface* pkSurface)
{
    NiTMapIterator kTagIter = m_kTagIndex.GetFirstPos();
    NiTPointerList<NiSurface*>* pkSurfaceList = NULL;
    const char* pcKey;
    NiTList<NiFixedString> kRemoveTags;

    bool bFoundAtLeastOne = false;

    while (kTagIter)
    {
        m_kTagIndex.GetNext(kTagIter, pcKey, pkSurfaceList);

        pkSurfaceList->Remove(pkSurface);
        if ((pkSurfaceList->GetSize() == 0) && (pcKey != NULL))
        {
            // Log what tags need removing (can't remove whilst iterating!)
            kRemoveTags.AddHead(NiFixedString(pcKey));
        }
    }

    // Loop through all the empty tags now and remove those
    for (NiUInt32 i = 0; i < kRemoveTags.GetSize(); ++i)
    {
        pcKey = kRemoveTags.GetHead();
        NIVERIFY(m_kTagIndex.GetAt(pcKey, pkSurfaceList));

        // Delete this surface list, it is no longer needed
        NiDelete pkSurfaceList;

        // Remove this tag from the map:
        m_kTagIndex.RemoveAt(pcKey);

        kRemoveTags.RemoveHead();
    }
    return bFoundAtLeastOne;
}
//--------------------------------------------------------------------------
void NiSurfacePalette::RefreshIndexedSurfaces()
{
    //Refreshing is done by parsing each package looking for ones that were 
    //modified    
    NiTMapIterator kPackageIterator = m_kPackageMap.GetFirstPos();

    while (kPackageIterator)
    {
        //iterates through the packages
        NiSurfacePackage* pkSurfacePackage;
        const char* pcKey;
        
        m_kPackageMap.GetNext(kPackageIterator, pcKey, pkSurfacePackage);
        
        if (pkSurfacePackage->m_bSurfaceChanged)
        {
            // if the package has changed, iterate through all the surfaces
            // to update the surfaces that have changed
            NiTMapIterator kSurfaceIterator = 
                pkSurfacePackage->m_kSurfaces.GetFirstPos();
        
            while (kSurfaceIterator)
            {
                NiSurface* pkSurface;
                const char* pcNextKey;
                pkSurfacePackage->m_kSurfaces.GetNext(kSurfaceIterator,
                    pcNextKey, pkSurface);

                if (pkSurface->m_bTagsChanged)
                {
                    // A modified surface has been found, we update it
                    RefreshSurface(pkSurface);
                }
            }
            pkSurfacePackage->m_bSurfaceChanged = false;
        }
    }

}
//--------------------------------------------------------------------------
void NiSurfacePalette::RefreshSurface(NiSurface* pkSurface)
{
    NiTObjectSet<NiFixedString> kRemovedTags;

    pkSurface->GetRemovedTags(kRemovedTags);
    
    // A surface was found changed
    if (kRemovedTags.GetSize())
    {
        // If the surface has tags that have been removed, the
        // surface needs to be removed from those tags in the
        // table. The tags must as well be removed from the 
        // removed tag list.

        NiUInt32 uiSize = kRemovedTags.GetSize();
        for (NiUInt32 ui = 0; ui < uiSize; ++ui)
        {
            const NiFixedString& kTag = kRemovedTags.GetAt(ui);

            NiTPointerList<NiSurface*>* pkSurfaceList = NULL;
            m_kTagIndex.GetAt(kTag, pkSurfaceList);

            pkSurfaceList->Remove(pkSurface);
            pkSurface->DeleteTag(kTag);
        }
    }

    // Adds the surface to make sur modified tags and added 
    // tags are properly updated in the table
    AddIndexedSurface(pkSurface);
    pkSurface->m_bTagsChanged = false;
}
