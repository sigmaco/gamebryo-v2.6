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

#ifndef NIWATER_H
#define NIWATER_H

#include "NiTerrain.h"

class NiNode;
class NiMesh;
class NiTexture;
class NiDynamicTexture;
class NiMaterial;
class NiSourceTexture;

/**
    The water scene graph node contains the root scene graph data and meshes needed for rendering a
    simple water plane. This class stores all the necessary properties for creating and managing 
    water geometry in the scene graph.
*/
class NITERRAIN_ENTRY NiWater : public NiNode
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;

    /// @endcond

public:

    /// Enumerates the different reflection modes supported by the water node. The reflection modes
    /// are used for reflecting the environment off of the water's surface.
    enum eReflectionMode
    {
        /// Indicates no reflection.
        REFLECTION_MODE_NONE                = 0,

        /// Indicates to use cube map for the reflection mode. Note, this is strictly static 
        /// reflection.
        REFLECTION_MODE_CUBE_MAP            = 1,

        /// Indicates to use planar projection for reflection mode. Not yet supported.
        REFLECTION_MODE_PLANAR_PROJECTION   = 2
    };
    
    /// Default constructor.
    NiWater();

    /**
        Copies the data from the specified water object.
        @param pkTemplate Pointer to a water object to use as a template.
        @note This is similar to a copy constructor but necessary for interaction with the existing
            NiEntity system.
    */
    NiWater(NiWater* pkTemplate);

    /// Destructor.
    ~NiWater();
    
    /**
        Creates the underlying water geometry based on the current property values.
    */
    void Initialize();   

    /// @cond EMERGENT_INTERNAL

    /**
        Triggers an update of all internal data structures for the water node.
        @param kUpdate Update process.
    */     
    void DoUpdate(NiUpdateProcess& kUpdate);

    
    /** 
        Override the update function of nodes so that those are called
        appropriately when updating water.
    */
    virtual void UpdateDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateRigidDownwardPass(NiUpdateProcess& kUpdate);

    /// @endcond

    /// Gets the length of the water plane in world units.
    inline NiUInt32 GetLength() const;

    /// Sets the length of the water plane in world units.
    inline void SetLength(NiUInt32 uiLength);
    
    /// Gets the width of the water plane in world units.
    inline NiUInt32 GetWidth() const;

    /// Sets the width of the water plane in world units.
    inline void SetWidth(NiUInt32 uiWidth);
    
    /// Gets the maximum vertices per side of the water plane.
    inline NiUInt32 GetMaxVerticesPerSide() const;

    /**
        Defines the number of vertices on each side of the water plane. This is useful for fog 
        calculations on SM2 cards. On >= SM3 platforms, this value can be set to 2.
    */
    void SetMaxVerticesPerSide(NiUInt32 uiMaxVerticesPerSide);

    /// Gets the number of vertices along the y-axis of the water plane. 
    /// @note The length and width vertex count may be different depending on the ratio of width
    ///     and length of the water plane. 
    inline NiUInt32 GetNumLengthVertices() const;

    /// Gets the number of vertices along the x-axis of the water plane. 
    /// @note The length and width vertex count may be different depending on the ratio of width
    ///     and length of the water plane. 
    inline NiUInt32 GetNumWidthVertices() const;

    /// Gets the current color used for rendering shallow water.
    inline NiColorA GetShallowColor() const;

    /// Sets the color used to render the water if depth is less than depth threshold.
    inline void SetShallowColor(NiColorA kColor);

    /// Gets the current color used for rendering deep water.
    inline NiColorA GetDeepColor() const;

    /// Sets the color used to render the water if depth is greater than depth threshold.
    inline void SetDeepColor(NiColorA kColor);

    /// Gets the name of the cube map used for rendering static reflections on the water.
    inline NiFixedString GetEnvMap() const;

    /// Sets the name of the cube map used for rendering static reflections on the water.
    inline void SetEnvMap(NiFixedString kEnvMap);

    /// Gets the current depth threshold.
    inline float GetDepthThreshold() const;

    /// Sets the current depth threshold used for determining how to blend the shallow and deep 
    /// water colors.
    inline void SetDepthThreshold(float fDepth);

    /// Gets the directory used for fetching a flip-book of normal map textures.
    inline NiFixedString GetNormalMapDir() const;

    /// Sets the directory used to search for normal maps.
    inline bool SetNormalMapDir(NiFixedString kDir);

    /// Gets the mask used to find normal map textures in the specified normal map directory.
    inline NiFixedString GetNormalFile() const;

    /// Sets the mask used to find normal map textures in the specified normal map directory.
    inline void SetNormalFile(NiFixedString kFile);

    /// Gets the size of the texture map used as a blend map for determining height of the water 
    /// plane above the terrain.
    inline NiUInt32 GetTextureSizeRatio() const;

    /// Sets the size of the texture map used as a blend map for determining height of the water 
    /// plane above the terrain.
    inline void SetTextureSizeRatio(NiUInt32 uiSize);

    /// Gets the velocity at which the normal map scrolls in the xy plane.
    inline NiPoint2 GetNormalVelocity() const;

    /// Sets the velocity at which the normal map scrolls in the xy plane.
    inline void SetNormalVelocity(NiPoint2 kVelocity);

    /// Gets the strength of the reflection map.
    inline float GetReflectionFactor() const;

    /// Sets the strength of the reflection map which is used to determine how much of the 
    /// reflection map is visible on the water's surface.
    inline void SetReflectionFactor(float fReflection);

    /// Gets the terrain object associated with the water plane. The associated terrain object is 
    /// used to build a height map encoding how far the water is from the terrain.
    inline NiTerrain* GetAssociatedTerrain() const;

    /// Sets the associated terrain object.
    inline void SetAssociatedTerrain(NiTerrain* pkTerrain);

    /// Gets the current reflection mode.
    inline NiUInt8 GetReflectionMode() const;

    /// Sets the current reflection mode.
    inline void SetReflectionMode(NiUInt8 uiRefMode);

    /// Gets the specular strength.
    inline float GetSpecularity() const;

    /// Sets the specular strength on the water. A higher value will create smaller more focused 
    /// highlights on the water's surface.
    inline void SetSpecularity(float fSpecularity);

    /// Gets the animation speed of the flip-book style animated normal maps.
    inline float GetNormalAnimationSpeed() const;

    /// Sets the animation speed of the flip-book style animated normal maps. The speed indicates
    /// how many frames per second in which the normal maps should be played back.
    inline void SetNormalAnimationSpeed(float fSpeed);

    /// @cond EMERGENT_INTERNAL
    inline NiTexture* GetDynamicEnvMap() const;
    inline void SetDynamicEnvMap(NiTexture* pkTexture);

    inline NiFixedString GetRefractionMap() const;
    inline void SetRefractionMap(NiFixedString kRefractionPath);

    inline NiTexture* GetDynamicRefractionMap() const;
    inline void SetDynamicRefractionMap(NiTexture* pkTexture);

    inline float GetDisturbance() const;
    inline void SetDisturbance(float fSpecularity);

    inline float GetFogDensity() const;
    inline void SetFogDensity(float fFogDensity);

    inline float GetFogDistance() const;
    inline void SetFogDistance(float fFogDistance);

    inline float GetFogFallOff() const;
    inline void SetFogFallOff(float fFogFallOff);

    inline float GetNormalLength() const;
    inline void SetNormalLength(float fLength);

    inline float GetNormalWidth() const;
    inline void SetNormalWidth(float fWidth);

    /// @endcond

protected:
    /// Property values
    NiColorA m_kShallowColor;
    NiColorA m_kDeepColor;
    NiPoint2 m_kTexTransformVelocity;    
    NiUInt32 m_uiLength;
    NiUInt32 m_uiWidth;
    NiUInt32 m_uiMaxVerticesPerSide;
    NiUInt32 m_uiNumVerticesLength;
    NiUInt32 m_uiNumVerticesWidth;
    float m_fDepthThreshold;
    float m_fNormalWidth;
    float m_fNormalLength;
    float m_fReflectionFactor;
    float m_fSpecularity;
    float m_fDisturbance;
    float m_fFogDensity;
    float m_fFogDistance;
    float m_fFogFallOff;
    float m_fNormalAnimationSpeed;
    NiUInt32 m_uiTextureSizeRatio;
    NiUInt8 m_uiRefMode;    
    NiFixedString m_kNormalMapDir;
    NiFixedString m_kNormalMapFile;    
    NiFixedString m_kEnvMap;    
    NiFixedString m_kRefractionMap; 

    /// Associated terrain node
    NiTerrainPtr m_spAssociatedTerrain;
              
    /// The list of normal maps
    NiTPointerList<NiSourceTexturePtr> m_kNormalMapList;
    
    /// List iterator
    NiTListIterator m_kCurrentNormalMap;

    /// Translation
    NiPoint3 m_kTranslation;
       
    /// Texture offset
    float m_fTextureOffsetX;
    float m_fTextureOffsetY;

    /// Time of last update
    float m_fPreviousTime;

    /// The DX9 Renderer device reset callback index
    bool m_bRegisteredDXDeviceResetCallback;
    NiUInt32 m_uiDXDeviceResetCallbackIndex;

    /// Length of the texture
    NiUInt32 m_uiTextureLength;

    /// Width of the texture
    NiUInt32 m_uiTextureWidth;       
    
    /// water mesh
    NiMeshPtr m_spWaterMesh;

    /// The water material
    NiMaterialPtr m_spWaterMaterial;    
    
    /// Texture Mask
    NiDynamicTexturePtr m_spTextureMask;

    /// Environment map
    NiTexturePtr m_spEnvironmentMap;

    /// Refraction map
    NiTexturePtr m_spRefractionMap;
    
    /// A blank texture, in case no normals are provided
    static NiSourceTexturePtr ms_spBlankNormals;

    /// Flag to know when to redraw the texture
    bool m_bRedrawTexture;

    /// Flag to indicate the normal maps needs to be reloaded
    bool m_bReloadNormalMaps;

    /// Flag to indicate that extra data on mesh needs to be updated
    bool m_bUpdateExtraData;

    /// Flag to indicate that the environment map needs to be loaded
    bool m_bReloadEnvMap;

    /// Flag to indicate that the refraction map needs to be loaded
    bool m_bReloadRefractionMap;

    /// Defining which way the parsing of the list is done
    bool m_bParsingLeftToRight;


private:

    /**
        Interpolate values between the vertices along the width of the
        water plane.
      */
    float WidthLerp(float fMinValue, float fMaxValue, NiUInt32 uiVertex);

    /**
        Interpolate values between the vertices along the height of the
        water plane.
      */    
    float LengthLerp(float fMinValue, float fMaxValue, NiUInt32 uiVertex);

    /**
        Returns the index for the vertex at the user-specified row and column 
        in the tessellated grid that the water mesh is comprised of.
      */    
    NiUInt32 GetVertexIndex(NiUInt32 uiRow, NiUInt32 uiCol);
    
    /** 
        Creates the Mesh and allocates data streams
     */
    void CreateWaterMesh(NiFixedString kEntityName);

    /** 
        Creates and adds mesh properties to the water mesh
     */
    void CreateMeshProperties();

    /** 
        Fills in values for the streams of the existing mesh.
     */
    void ComputeMesh();

    /** 
        Populates the diffuse texture by calculating the depth below the 
            surface. A diffuse texture is created if none is present
     */
    void ComputeTexture();

    /** 
        This function loads the normal maps using the directory and file 
            format component properties.
     */
    void LoadNormalMaps();

    /** 
        Update all the extra attributes on the water mesh.
      */
    void UpdateExtraData();

    /** 
        Load the environment map reflected from the water.
      */
    void LoadEnvMap();

    /** 
        Load the refraction map used to calulate lighting on water.
      */
    void LoadRefractionMap();    

    /**
        This function recalculates the UVs of the mesh when the water is
        transformed.
    */
    void ComputeUVStream();

    /**
        Calculate the texcoords based on the size of associated terrain,
        if there is one. Otherwise, texcoord are 0.0 to 1.0.
     */
    void CalculateUVTexCoords(float& fMinXTexCoord, float& fMaxXTexCoord, 
        float& fMinYTexCoord, float& fMaxYTexCoord);

    /// @cond EMERGENT_INTERNAL
    
    /// Register for notification of a DX9 device reset.
    void SubscribeToDXDeviceResetNotification();

    /// Unregister from DX9 device reset notifications.
    void UnsubscribeToDXDeviceResetNotification();

    /// Handle a DX9 device reset by regenerating the blend mask textures.
    static bool HandleDXDeviceReset(bool bBeforeReset, void* pkVoid);
    
    /// @endcond

};

NiSmartPointer(NiWater);

#include "NiWater.inl"

#endif
