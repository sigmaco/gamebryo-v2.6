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

#ifndef NISURFACE_H
#define NISURFACE_H

#include <NiSourceTexture.h>
#include <NiString.h>
#include <NiPoint2.h>

#include "NiTerrainLibType.h"
#include "NiMetaData.h"

class NiSurfacePackage;
class NiEntityErrorInterface;

/** 
    NiSurface objects are used to describe different patches of terrain.
    
    An NiSurface object can be painted onto the terrain to create patches
    of grass, sand, snow, gravel, etc. This class is mainly used to define
    how these patches will be rendered by correlating all the relevant data
    ready to be sent to the NiTerrainMaterial.
 */
class NITERRAIN_ENTRY NiSurface : public NiMemObject 
{

public:
        
    /// The enumeration used to select different map channels:
    enum SurfaceMapID
    {
        /// Diffuse map which may or may not contain an alpha 
        /// channel. If the alpha channel is present then alpha is interpreted 
        /// as a detail map.
        SURFACE_MAP_DIFFUSE,

        /// Normal map which may or may not contain an alpha 
        /// channel. If the alpha channel is present then alpha is interpreted
        /// as a parallax map.
        SURFACE_MAP_NORMAL,

        /// This map is tiled the same number of times as the diffuse and normal map
        /// but is used to provide additional blending information per layer. This allows
        /// a non-linear blend around the borders of layers.
        SURFACE_MAP_DISTRIBUTION,

        /// Maximum number of maps that may be assigned to a surface/material.
        NUM_SURFACE_MAPS
    };
  
    /// @cond EMERGENT_INTERNAL
    /** True if the tags have changed since the last indexing */
    bool m_bTagsChanged;
    /// @endcond EMERGENT_INTERNAL

   
    /// @name Constructors
    //@{

    /**
        Constructor with parameters.

        @param pkMetaDataStore Meta data store to use for the internal meta 
        data.
     */
    NiSurface(NiMetaDataStore* pkMetaDataStore);
    //@}

    /// Destructor
    virtual ~NiSurface();   

    /// @cond EMERGENT_INTERNAL
    static void _SDMInit();
    static void _SDMShutdown();
    /// @endcond
   
    /// @name Serialization
    //@{
    /** 
        Load the data for this surface from a DOM interface.        

        @param kDom The DOM to load the data from.
     */
    void Load(NiDOMTool& kDom);
    
    /// Save the data for this surface to a DOM interface.        
    ///
    /// @param kDom DOM object with which to save the surface.     
    void Save(NiDOMTool& kDom) const;
    //@}
        
    /// @name Package Management
    //@{
    
    /** 
        Sets a reference to the package that contains this surface instance.
        
        This function will update both the new and the old package to reflect
        the package change.
        
        @note If the new package is set to NULL, the user claims responsibility
            for deleting this surface when it is no longer used.
        @param pkPackage The package that owns this surface instance.
     */
    void SetPackage(NiSurfacePackage* pkPackage);

    /// Gets a reference to the package that contains this surface instance.
    ///
    /// @return The package that owns this surface instance.
    NiSurfacePackage* GetPackage() const;

    //@}
    
    /// @name Properties
    //@{
    /** 
        Set the name of this surface.
        
        This function will also update the containing package to reflect
        the name change.
        
        @param kName Name of this surface.
     */
    void SetName(const NiFixedString& kName);

    /// Get the name of this surface.     
    const NiFixedString& GetName() const;

    /** 
        Set the description of this surface.        

        @param kDescription Description of this surface.
    */
    void SetDescription(const NiFixedString& kDescription);

        
    /// Get the description of this surface
    const NiFixedString& GetDescription() const;
    //@}

    /// @name Tagging
    //@{
    
    /// Add a tag to the surface.
    bool AddTag(const NiFixedString& kTag);

    /**
        Populate the given list with all the tags that are contained within
        this surface.
        
        Tags that have been marked for removal will not be included in this
        list. This function will only add a tag to the set if it is not
        already contained in the list

        @param pkTags List to be populated with all tags
            contained in this surface.
        @return Number of tags, NOT the number added to list.
    */
    NiUInt32 GetTags(NiTObjectSet<NiFixedString>& pkTags);

    /// Replace the given old tag with a new tag
    void SetTag(const NiFixedString& kOldTag, const NiFixedString& kNewTag);

    /// Test for existence of the given tag within this surface.
    bool HasTag(const NiFixedString& kTag);

    /// Test for existence of the given tag within this surface.
    bool HasTag(const char* pcTag);

    /// Get the number of tags associated with the surface
    NiUInt32 GetTagCount();

    /**
        Mark a tag for removal from this surface.
        
        This function will not delete the tag, but instead moves it to a
        'removed tags' array accessible via the GetRemovedTags function.
    */
    bool RemoveTag(const NiFixedString& kTag);

    /**
        Mark a tag for removal from this surface.
        
        This function will not delete the tag, but instead moves it to a
        'removed tags' array accessible via the GetRemovedTags function.
    */
    bool RemoveTag(const char* pcTag);

    /**
        Find all the tags that have been marked for removal.
        
        Will only add a tag to the set if it is not already contained in the
        list.

        @param pkTags List to be populated with all tags marked for removal in 
            this surface.
        @return The number of tags marked for removal, NOT the
            number added to list
    */
    NiUInt32 GetRemovedTags(NiTObjectSet<NiFixedString>& pkTags);

    /**
        Mark all tags for removal from this surface.
        
        This function will not delete the tags, but instead move them to a
        'removed tags' array accessible via the GetRemovedTags function.
     */
    void RemoveAllTags();

    /**
        Completely removes a tag from this surface, and the 'removed tags' 
        array.
    */
    bool DeleteTag(const NiFixedString& kTag);

    /**
        Completely removes a tag from this surface, and the 'removed tags' 
        array.
    */
    bool DeleteTag(const char* pcTag);

    //@}
    
    /// @name Surface Configuration
    //@{
    /**
        Set the texture file to use for the selected map of this surface.

        @param eMapID The id of the map to set.
        @param kMap The filename of a texture to use.
        @return False if the map ID was invalid. True otherwise.
    **/
    bool SetMapFile(SurfaceMapID eMapID, const NiFixedString& kMap);

    /**
        Get the texture file to use for the selected map of this surface.

        @param eMapID The id of the map to set.
        @param kMap The filename of the texture in use.
        @return False if the map ID was invalid. True otherwise.
    **/
    bool GetMapFile(SurfaceMapID eMapID, NiFixedString& kMap) const;

    /**
        Set a string-valued surface map parameter.
        
        Parameters can be used to store additional info about a surface map
        and can represent plain text, integer, floating point or an 
        NiColorA.

        @param eMapID The id of the map that the parameter belongs to.
        @param kParamName Name (or key) of the parameter.
        @param kValue Value of the parameter.
        @return False if the map ID was invalid. True otherwise.
    */
    bool SetMapParam(SurfaceMapID eMapID, const NiFixedString& kParamName,
        NiFixedString& kValue);

    /**
        Set a color-valued surface map parameter.
        
        Parameters can be used to store additional info about a surface map
        and can represent plain text, integer, floating point or an 
        NiColorA.

        @param eMapID The id of the map that the parameter belongs to.
        @param kParamName Name (or key) of the parameter.
        @param kValue Value of the parameter.
        @return False if the map ID was invalid. True otherwise.
    */
    bool SetMapParam(SurfaceMapID eMapID, const NiFixedString& kParamName,
        NiColorA& kValue);
    
    /**
        Get a string-valued surface map parameter.
        
        @param eMapID The id of the map that the parameter belongs to.
        @param kParamName Name (or key) of the parameter.
        @param kValue Value of the parameter.
        @return False if the map ID was invalid, or if the type of the stored
            parameter was not a string. True otherwise.
    */
    bool GetMapParam(SurfaceMapID eMapID, const NiFixedString& kParamName,
        NiFixedString& kValue) const;    

    /**
        Get a color-valued surface map parameter.
        
        @param eMapID The id of the map that the parameter belongs to.
        @param kParamName Name (or key) of the parameter.
        @param kValue Value of the parameter.
        @return False if the map ID was invalid, or if the type of the stored
            parameter was not a color. True otherwise.
    */
    bool GetMapParam(SurfaceMapID eMapID, const NiFixedString& kParamName,
        NiColorA& kValue) const;

    /**
        Remove the given parameter from the given map ID, if it exists.

        @param eMapID The id of the map that the parameter belongs to.
        @param kParamName Name (or key) of the parameter.
        @return False if the map ID was invalid, or if the given param name
            does not exist for the given map. True otherwise.
    */
    bool RemoveMapParam(SurfaceMapID eMapID, const NiFixedString& kParamName);

    /** 
        Get the name of a map based on it's map Enum.
        
        @param eMapID the id of the map to get.
        @param kMapName the place to put the name of the map.
        @return False if the map ID was invalid. True otherwise.
     */
    static bool GetMapName(SurfaceMapID eMapID, NiFixedString& kMapName);

    /// Set the U and V scaling factors for this surface's texture coordinates.
    void SetUVScale(NiPoint2 kScale);
    
    /// Set the U and V offsets for this surface's texture coordinates.
    void SetUVOffset(NiPoint2 kOffset);

    /// Get the U and V scaling factors for this surface's texture coordinates.
    NiPoint2 GetUVScale() const;
    
    /// Get the U and V offsets for this surface's texture coordinates.
    NiPoint2 GetUVOffset() const;

    /// Gets the scaler value applied to the distribution mask.
    float GetDistributionMaskStrength() const;

    /// Sets the distribution mask strength.
    void SetDistributionMaskStrength(float fValue);

    /** 
        Set all of our internal variables to that of the given template, with
            the exception of the name and the package.
        
        @param pkTemplate The surface from which to copy settings
     */
    void CopySettings(const NiSurface* pkTemplate);

    /// Retrieve a reference to the NiMetaData object that this surface uses.
    NiMetaData& GetMetaData();

    /// Retrieve a const reference to the NiMetaData object that this surface
    /// uses.
    const NiMetaData& GetMetaData() const;
    //@}
    
    /// @name Shader Interface
    //@{
    /// This function loads all the texture files that have been specified.
    void CompileSurface(NiEntityErrorInterface* pkError = NULL);

    /// Return true if this surface has been compiled through the 
    /// CompileSurface function
    bool IsCompiled() const;

    /** 
        Get the texture of the specified type.        
        @param eMapID Type of texture map to retrieve from this surface.
        @return a pointer to the texture object to use or NULL if the texture
            has not been specified for the surface.
     */
    NiTexture* GetTexture(SurfaceMapID eMapID) const;   
    //@}

protected:

    /// The scaling factors for this surface's UV mapping:
    NiPoint2 m_kUVScale;
    NiPoint2 m_kUVOffset;

    float m_fDistributionMaskStrength;

    /// The array of map ID names:
    static NiFixedString* ms_pkSurfaceMapNames;

    /// The package that we belong to
    NiSurfacePackage* m_pkPackage;

    /// The name of this surface
    NiFixedString m_kName;

    /// A short description of this surface
    NiFixedString m_kDescription;

    /// Base diffuse texture 
    NiFixedString m_kDiffuseMapFile; 

    /// Normal map texture. May contain a parallax map in the alpha channel.
    NiFixedString m_kNormalMapFile; 

    /// Distribution map texture.
    NiFixedString m_kDistributionMapFile;

    /// MetaData storage. (E.g. footfall information)
    NiMetaData m_kMetaData; 

    /// Textures compiled for shader use
    NiSourceTexturePtr m_aspShaderTextures[NUM_SURFACE_MAPS]; 

    /// The tags linked to this surface
    NiTStringMap<bool> m_kTags;

    /// Tags recently removed
    NiTStringMap<bool> m_kRemovedTags;

    /// Surface map parameters
    NiTStringMap<NiString*> m_kMapParameters[NUM_SURFACE_MAPS];

private:

    void SaveSurfaceMap(SurfaceMapID eMapID, NiDOMTool& kDom) const;

    void LoadSurfaceMap(SurfaceMapID eMapID, NiDOMTool& kDom);

    /// True if Compile() has been called successfully
    bool m_bCompiled;

};

#endif // NISURFACE_H
