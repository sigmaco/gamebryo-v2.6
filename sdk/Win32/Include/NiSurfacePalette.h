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

#ifndef NISURFACEPALETTE_H
#define NISURFACEPALETTE_H

#include <NiSmartPointer.h>

#include <NiTStringMap.h>
#include <NiTList.h>
#include <NiTSet.h>

#include "NiTerrainLibType.h"

#include "NiSurface.h"
#include "NiSurfacePackage.h"

class NiMetaDataStore;
NiSmartPointer(NiSurfacePalette);

/**
    The palette contains and manages a collection of surface packages and keeps
    a searchable index of surface tags.
    
    The palette should be used to make any modifications to surfaces or
    packages, so that the caches stay intact.

    The surface palette is not owned by a single terrain, it should be regarded
    as a singleton that is referenced across all terrain instances. The
    surface palette should be owned and managed by the same code base that
    is responsible for managing the terrain entities themselves.
 */
class NITERRAIN_ENTRY NiSurfacePalette : public NiRefObject
{

public:

    /// Retrieve a pointer to the NiSurfacePalette instance.
    static NiSurfacePalette* GetInstance();

    /// Create an NiSurfacePalette instance.
    ///
    /// The constructor is private.
    static void CreateInstance();

    /// Destroy the NiSurfacePalette instance.
    static void DestroyInstance();

    /// Destructor
    ~NiSurfacePalette();

    /** 
        Attempt to retrieve a reference to the requested surface.
        
        If the surface is not already in memory, it will attempt to read the
        surface textures from disk.
        
        @param kPackage The name of the package from which to load the 
            requested surface.
        @param kName The name of the surface within the given package.
        @return Pointer to the surface, or 0 if surface textures could not be 
            found.
     */
    NiSurface* GetSurface(const NiFixedString& kPackage,
        const NiFixedString& kName) const;

    /** 
        Create a blank surface with the given name, and adds it to the given
        package.
        
        If the surface already exists, no new surface is created;
        the existing surface is returned. The package must already exist,
        otherwise the surface will not be created.
        
        @param kPackage Name of the package to get the surface from.
        @param kName Name of the surface to retrieve.
        @param pkMetaDataStore Optional pointer to meta data store to use. If 
            NULL, the static singleton meta data store is used.
        @return Pointer to the surface, or 0 if surface could not be found.
     */
    NiSurface* CreateSurface(const NiFixedString& kPackage,
        const NiFixedString& kName, NiMetaDataStore* pkMetaDataStore = 0);

    /** 
        Creates a new surface, copying details from the given source surface.

        @param kNewPackage Name of the package which the new surface will be
            placed in. The package must exist.
        @param kNewName Name to give to the new surface.
        @param pkSource Template from which the new surface shall be created.
        @return Pointer to the new surface, or 0 if the entered package does
            not exist.
     */
    NiSurface* CreateSurfaceFrom(
        const NiFixedString& kNewPackage, const NiFixedString& kNewName,
        const NiSurface* pkSource);

    /** 
        Unloads the given surface.
        
        The package itself will be unloaded if this
        operation results in the package being empty. The surfaces to be 
        removed must not be referenced by anything other than this palette.
        
        @param kPackage name of the package that contains the surface.
        @param kName The name of the surface within the given package.
     */
    void UnloadSurface(const NiFixedString& kPackage,
        const NiFixedString& kName);    

    /** 
        Returns a previously loaded package.
        
        @param kPackage Name of the loaded package to retrieve.
        @return Pointer to a loaded package, or 0 if no package was found.
     */
    NiSurfacePackage* GetPackage(const NiFixedString& kPackage) const;

    /** 
        Populates the given set with pointers to all packages that are being
        used by this palette. The set will not be emptied before it is
        populated, however it will only add packages that are not already
        within the set.
        
        @param kPackages A reference to the set which will be populated.
     */
    void GetLoadedPackages(NiTPrimitiveSet<NiSurfacePackage*>& kPackages)
        const;

    /** 
        Loads the requested package.
        
        @param pcPackageFile Package file to load .
        @param pkMetaDataStore Optional pointer to meta data store to use. If 
            NULL, the static singleton meta data store is used.
        @return A pointer to the package object that was generated, or
            0 if failed.
     */
    const NiSurfacePackage* LoadPackage(const char* pcPackageFile,
        NiMetaDataStore* pkMetaDataStore = 0);

    /** 
        Pre-compiles the requested package.
        
        This will call the Compile function
        on all surfaces contained within the given package
        
        @param kPackageFile Package file to load
     */
    void CompilePackage(const NiFixedString& kPackageFile);

    /** 
        Saves the given package, and then all the contained surfaces.
        
        @param kPackage Name of the loaded package to save
        @return True if the package, and all surfaces were saved successfully
     */
    bool SavePackage(const NiFixedString& kPackage);

    /** 
        Unloads the given package, and all contained surfaces from memory.
        
        The surfaces to be removed must not be referenced by anything other
        than this palette.
        
        @param kPackage The name of the package to unload.
     */
    void UnloadPackage(const NiFixedString& kPackage);

    /**
        Returns the list of tags that have been indexed by the palette.
        
        @param kTags A return parameter that contains a list of tags
            indexed by the palette.
        @return NiUInt32 representing the number of tags found. 
    */
    NiUInt32 GetIndex(NiTList<NiFixedString>& kTags);

    /**
        Adds a surface to the tag indexed hashing table.

        The keys (tags) are contained in the created surface.
        If the tag is not already present in the table, it will be added.
        If the surface is already in the table at the corresponding index
        the addition to that particular index will not be done to avoid
        duplicates.

        @param kSurface The created surface to add to the table.
        @return False if the surface isn't valid or doesn't contain
            tags (in those cases the addition fails). True otherwise.
    */
    bool AddIndexedSurface(NiSurface* kSurface);

    /**
        Gets the list of surfaces containing the tags given in parameter.
        
        @param kTags A list of tags with which to search.
        @param kSurfaces Filled with a list of surfaces containing tags in the 
            specified list.
        @param bAndOperation If true, only finds surfaces with all tags in the 
            specified tag list.
        @return The number of surfaces found.
    */
    NiUInt32 GetSurfaces(NiTList<NiFixedString>& kTags, 
        NiTPrimitiveSet<NiSurface*>& kSurfaces, bool bAndOperation = false);

    /**
        Returns the list of surfaces containing the tag given in parameter.
        
        @param kTag The name of the tags we are looking for
        @param kSurfaces A return parameter that contains a list of surfaces
            that use the tags listed. If this parameter is not NULL, the 
            function will append the found surfaces to the list otherwise
            it will create a new list.
        @return int representing the number of surfaces found. Returns 0 if 
            the tag couldn't be found. Might differ from the size of the list
            if the list already had elements
    */
    NiUInt32 GetSurfaces(const NiFixedString& kTag, 
        NiTPrimitiveSet<NiSurface*>& kSurfaces);

    /**
        Removes every instance of a surface from the hash table.
        
        If the surface is that last one with its tag, the tag will be removed 
        as well.

        @param pkSurface the surface to remove from tagging
        @return Returns false if the surface couldn't be found, true otherwise
    */
    bool RemoveIndexedSurface(NiSurface* pkSurface);

    /** 
        Parses the hash map and checks the surfaces have the tags the they
        belong to.
        
        If it is not the case the surface is removed from the 
        tag they no longer possess. Moreover, if the surface's tags have 
        been modified (not removed), the function calls RefreshSurface.
     */
    void RefreshIndexedSurfaces();

    /** 
        Updates the links to the tags of the surface in parameter by simply
        adding the surface to the hash table.
        
        See definition of AddIndexedSurface for more details on the behavior
        of the function.
        
        @param pkSurface surface to update; 
     */
    void RefreshSurface(NiSurface* pkSurface);

protected:
    /// Default Constructor
    NiSurfacePalette();

    /// Tag index
    NiTStringMap<NiTPointerList<NiSurface*>*> m_kTagIndex;

    // A map of package names, to the packages themselves.
    NiTStringPointerMap<NiSurfacePackage*> m_kPackageMap;

    // A map of package file names, to the packages themselves.
    NiTStringPointerMap<NiSurfacePackage*> m_kPackageFilenameMap;

private:

    /// Global singleton instance of palette
    static NiSurfacePalettePtr ms_spInstance;

};

#endif // NISURFACEPALETTE_H
