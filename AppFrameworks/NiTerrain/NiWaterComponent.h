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

#ifndef NIWATERCOMPONENT_H
#define NIWATERCOMPONENT_H

#include <NiSmartPointer.h>
#include <NiRefObject.h>
#include <NiMesh.h>
#include <NiEntityInterface.h>
#include <NiSourceTexture.h>
#include <NiDynamicTexture.h>
#include <NiNode.h>
#include <NiSourceCubeMap.h>
#include <NiRenderer.h>
#include <NiFloatExtraData.h>
#include <NiRenderedTexture.h>
#include <NiRenderedCubeMap.h>
#include <NiIntegerExtraData.h>

#include "NiTerrainLibType.h"
#include "NiWater.h"

NiSmartPointer(NiWaterComponent);

/**
    The water component is attached to an NiEntity. It manages a creating a
    water mesh, loading the required textures, and animating of water. While
    the water component is not responsible for 'rendering' the water, it is
    responsible for applying the NiWaterMaterial to the water mesh and pushing
    the water mesh into a provided visibility set.
*/
class NITERRAIN_ENTRY NiWaterComponent : public NiRefObject,
    public NiEntityComponentInterface
{
    NiDeclareFlags(NiUInt32);

public:

    /// @name Properties
    //@{ 
    static NiFixedString ms_kPropNameLength;
    static NiFixedString ms_kPropNameWidth;
    static NiFixedString ms_kPropNameMaxVerticesPerSide;
    static NiFixedString ms_kPropNameAssociatedTerrain;
    static NiFixedString ms_kPropNameShallowColor;
    static NiFixedString ms_kPropNameDeepColor;
    static NiFixedString ms_kPropNameDepthThreshold;
    static NiFixedString ms_kPropNameNormalMapDir;
    static NiFixedString ms_kPropNameNormalMapFile;
    static NiFixedString ms_kPropNameTextureSizeRatio;
    static NiFixedString ms_kPropNameSceneRoot;
    static NiFixedString ms_kPropNameTexTransformVelocity;
    static NiFixedString ms_kPropNameNormalWidth;
    static NiFixedString ms_kPropNameNormalLength;
    static NiFixedString ms_kPropNameEnvMap;
    static NiFixedString ms_kPropNameReflectionFactor;
    static NiFixedString ms_kPropNameDynamicEnvMap;
    static NiFixedString ms_kPropNameRefractionMap;
    static NiFixedString ms_kPropNameDynamicRefraction;
    static NiFixedString ms_kPropNameSpecularity;
    static NiFixedString ms_kPropNameDisturbance;
    static NiFixedString ms_kPropNameFogDensity;
    static NiFixedString ms_kPropNameFogDistance;
    static NiFixedString ms_kPropNameFogFallOff;    
    static NiFixedString ms_kPropNameNormalAnimSpeed;
    //@}

    /// @name Sibling properties (Transformation)
    //@{ 
    static NiFixedString ms_kPropNameTranslation;
    static NiFixedString ms_kPropNameRotation;
    static NiFixedString ms_kPropNameScale;
    //@}

    /// @name Descriptions
    //@{ 
    static NiFixedString ms_kPropDescLength;
    static NiFixedString ms_kPropDescWidth;
    static NiFixedString ms_kPropDescMaxVerticesPerSide;
    static NiFixedString ms_kPropDescAssociatedTerrain;
    static NiFixedString ms_kPropDescShallowColor;
    static NiFixedString ms_kPropDescDeepColor;
    static NiFixedString ms_kPropDescDepthThreshold;
    static NiFixedString ms_kPropDescNormalMapDir;
    static NiFixedString ms_kPropDescNormalMapFile;
    static NiFixedString ms_kPropDescTextureSizeRatio;
    static NiFixedString ms_kPropDescSceneRoot;
    static NiFixedString ms_kPropDescTexTransformVelocity;
    static NiFixedString ms_kPropDescNormalWidth;
    static NiFixedString ms_kPropDescNormalLength;
    static NiFixedString ms_kPropDescEnvMap;
    static NiFixedString ms_kPropDescReflectionFactor;
    static NiFixedString ms_kPropDescDynamicEnvMap;
    static NiFixedString ms_kPropDescRefractionMap;
    static NiFixedString ms_kPropDescDynamicRefraction;
    static NiFixedString ms_kPropDescSpecularity;
    static NiFixedString ms_kPropDescDisturbance;
    static NiFixedString ms_kPropDescFogDensity;
    static NiFixedString ms_kPropDescFogDistance;
    static NiFixedString ms_kPropDescFogFallOff;
    static NiFixedString ms_kPropDescNormalAnimSpeed;
    //@}

    /// @name Class name
    //@{ 
    static NiFixedString ms_kClassName;
    static NiFixedString ms_kComponentName;
    //@}
   
    /// @name Constructors
    //@{

    /// Default constructor. 
    NiWaterComponent();

    /** 
        Copy constructor.

        @param pkTemplate Water component that will serve as a template when
        creating another water component.
    */
    NiWaterComponent(NiWaterComponent* pkTemplate);

    /// Destructor
    ~NiWaterComponent();
    //@}

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

    /// @name Overridden member functions.
    //@{
    
    /// Returns an instance of this component that can contain customized data.
    NiEntityComponentInterface* Clone(bool bInheritProperties);

    /// Returns the master component from which this component inherits its
    /// properties, if any. Returns NULL if this component does not inherit
    /// its properties.
    NiEntityComponentInterface* GetMasterComponent() const;

    /// Sets the master component from which this component should inherit its
    /// properties. This function just replaces the existing master component
    /// without changing which properties are set to inherit from its values.
    void SetMasterComponent(
        NiEntityComponentInterface* pkMasterComponent);

    /// Populates an array with all the properties that this component depends
    /// on. If any of these properties are not present for an entity, the
    /// component cannot be added to that entity. Property names are added to
    /// the array. No names are removed from the array.
    void GetDependentPropertyNames(
        NiTObjectSet<NiFixedString>& kDependentPropertyNames);

    /// Sets the template ID for this interface. Implementing classes are not
    /// required to allow setting of their template ID and should return false
    /// if the template ID was not set.    
    NiBool SetTemplateID(const NiUniqueID& kID);

    /// Gets the template ID for this interface.
    NiUniqueID GetTemplateID();

    /// This function should be called whenever a reference is obtained to this
    /// interface.
    void AddReference();

    /// This function should be called whenever a reference to this interface
    /// is no longer needed.
    void RemoveReference();

    /// Returns the class name for the implementing class for this interface.
    NiFixedString GetClassName() const;

    /// Returns the name of this object.
    NiFixedString GetName() const;

    /// Sets the name of this object, if allowed. The return value indicates
    /// whether or not the name was set.
    NiBool SetName(const NiFixedString& kName);

    /// Returns whether or not this object is animated and should be updated
    /// each frame.
    NiBool IsAnimated() const;

    /// Updates the object. Errors are reported to the provided error
    /// interface. External assets are loaded using the provided external
    /// asset interface.
    void Update(NiEntityPropertyInterface* pkParentEntity,
        float fTime, NiEntityErrorInterface* pkErrors,
        NiExternalAssetManager* pkAssetManager);

    /// Builds the Visible Set on the object using the provided rendering
    /// context. Errors are reported to the provided error interface.
    void BuildVisibleSet(NiEntityRenderingContext* pkRenderingContext,
        NiEntityErrorInterface* pkErrors);

    /// Gets all the property names for this object. Property names are
    /// added to the array. No names are removed from the array.
    void GetPropertyNames(
        NiTObjectSet<NiFixedString>& kPropertyNames) const;

    /// Returns whether or not the named property can be reset to a default
    /// value. If the property is not found, the function returns false.
    NiBool CanResetProperty(const NiFixedString& kPropertyName,
        bool& bCanReset) const;

    /// Resets the property to its default value if it can be reset. The return
    /// value indicates whether or not the property was reset.
    NiBool ResetProperty(const NiFixedString& kPropertyName);

    /// Makes the specified property unique, if it is not already. This will
    /// not change the property value but will instead prevent the property
    /// from inheriting its value from a default value. The returned
    /// bMadeUnique value indicates whether or not the property was made
    /// unique. If the property was already unique, this value will be false.
    NiBool MakePropertyUnique(const NiFixedString& kPropertyName,
        bool& bMadeUnique);

    /// Gets and sets the display name for the named property. If the property
    /// is not found, these functions return false. If the display name is set
    /// to NULL, the property should not be displayed.
    NiBool GetDisplayName(const NiFixedString& kPropertyName,
        NiFixedString& kDisplayName) const;
    NiBool SetDisplayName(const NiFixedString& kPropertyName,
        const NiFixedString& kDisplayName);

    /// Gets and sets the primitive type for the named property. If the
    /// property is not found, these functions return false.
    NiBool GetPrimitiveType(const NiFixedString& kPropertyName,
        NiFixedString& kPrimitiveType) const;
    NiBool SetPrimitiveType(const NiFixedString& kPropertyName,
        const NiFixedString& kPrimitiveType);

    /// Gets and sets the semantic type for the named property. If the property
    /// is not found, these functions return false.
    NiBool GetSemanticType(const NiFixedString& kPropertyName,
        NiFixedString& kSemanticType) const;
    NiBool SetSemanticType(const NiFixedString& kPropertyName,
        const NiFixedString& kSemanticType);

    /// Gets and sets the description for the named property. If the property
    /// is not found, these functions return false.
    NiBool GetDescription(const NiFixedString& kPropertyName,
        NiFixedString& kDescription) const;
    NiBool SetDescription(const NiFixedString& kPropertyName,
        const NiFixedString& kDescription);

    /// Returns the category for the named property. Properties are grouped by
    /// category when displaying in a user interface.
    NiBool GetCategory(const NiFixedString& kPropertyName,
        NiFixedString& kCategory) const;

    /// Retrieves whether or not the named property is read-only. If the
    /// property is not found, this function returns false.
    NiBool IsPropertyReadOnly(const NiFixedString& kPropertyName,
        bool& bIsReadOnly);

    /// Retrieves whether or not the named property is unique. If the
    /// property is not found, this function returns false.
    NiBool IsPropertyUnique(const NiFixedString& kPropertyName,
        bool& bIsUnique);

    /// Retrieves whether or not the named property should be serialized to
    /// disk. If the property is not found, this function returns false.
    NiBool IsPropertySerializable(const NiFixedString& kPropertyName,
        bool& bIsSerializable);

    /// Retrieves whether or not the named property has the ability to be 
    /// inherited. If the property is not found, this function returns false
    NiBool IsPropertyInheritable(const NiFixedString& kPropertyName,
        bool& bIsInheritable);

    /// Determines whether or not the property represents the path to an
    /// external asset file. If the property is not found, this function
    /// returns false.
    NiBool IsExternalAssetPath(const NiFixedString& kPropertyName,
        NiUInt32 uiIndex, bool& bIsExternalAssetPath) const;

    /// Sets an upper bound on the number of elements for this property. If
    /// the count is increased, appropriate default values are provided for the
    /// new elements. If the count is decreased, elements with the highest
    /// indices will be lost. This function should not be called for properties
    /// that are not collections. bCountSet returns whether or not the count
    /// was set (will be false for non-collection properties). If the
    /// property is not found, this function returns false.
    NiBool SetElementCount(const NiFixedString& kPropertyName,
        NiUInt32 uiCount,bool &bCountSet);

    /// Retrieves the number of data elements for this property. This number
    /// should be used to limit the index value that is passed into the
    /// GetPropertyData and SetPropertyData functions. If the property is not
    /// found, this function returns false.
    NiBool GetElementCount(const NiFixedString& kPropertyName,
        NiUInt32& uiCount) const;

    /// Returns whether or not the specified property is a collection of
    /// elements or has just a single element. This has a direct bearing on
    /// how such a property is edited in a user interface. This function
    /// returns false if the property is not found. bIsCollection will be true
    /// if the property is a collection and false if the property will only
    /// ever have a single element.
    NiBool IsCollection(const NiFixedString& kPropertyName,
        bool& bIsCollection) const;
    //@}

    /// @name Accessors
    //@{
    /// Getter and Setter for NiFixedString properties
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiFixedString& kData, NiUInt32 uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiFixedString& kData, NiUInt32 uiIndex = 0);

    /// Getter and Setter for Float properties
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        float& kData, NiUInt32 uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        float kData, NiUInt32 uiIndex = 0);

    /// Getter and Setter for NiUInt32 int properties
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiUInt32& kData, NiUInt32 uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiUInt32 kData, NiUInt32 uiIndex = 0);

    /// Getter and Setter for Color properties
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiColorA& kData, NiUInt32 uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiColorA& kData, NiUInt32 uiIndex = 0);

    /// Getter and Setter for Entity interface properties
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiEntityInterface*& pData, NiUInt32 uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiEntityInterface* pData, NiUInt32 uiIndex = 0);

    /// Getter and Setter for NiObject* properties
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiObject*& pkData, NiUInt32 uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiObject* pkData, NiUInt32 uiIndex = 0);

    /// Getter and Setter for NiPoint2 properties
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName, 
        NiPoint2& kData, NiUInt32 uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName, 
        const NiPoint2& kData, NiUInt32 uiIndex = 0);

    /// Getter and Setter for Boolean properties
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName, 
        bool& bData, NiUInt32 uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName, 
        bool bData, NiUInt32 uiIndex = 0);

    /// Getter and Setter for void* properties
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName, 
        void*& pvData, size_t& stDataSizeInBytes, 
        NiUInt32 uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName, 
        const void* pvData, size_t stDataSizeInBytes, 
        NiUInt32 uiIndex = 0);
    //@}

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
    NiFixedString m_kNormalMapDir;
    NiFixedString m_kNormalMapFile;    
    NiFixedString m_kEnvMap;    
    NiFixedString m_kRefractionMap; 
        
    /// Associated terrain entity
    NiEntityInterface* m_spAssociatedTerrain;
    
    /// Parent Component
    NiWaterComponentPtr m_spMasterComponent;

    /// The mesh root node
    NiWaterPtr m_spRootNode;

    /// Flag controling whether a property has changed or not
    bool m_bPropertyChanged;
    
private:
    
    enum FLAGS
    {
        PROP_LENGTH_CHANGED                     = 0x00000001,
        PROP_WIDTH_CHANGED                      = 0x00000002,
        PROP_SHALLOW_COLOR_CHANGED              = 0x00000004,
        PROP_DEEP_COLOR_CHANGED                 = 0x00000008,
        PROP_ENVMAP_CHANGED                     = 0x00000010,
        PROP_DEPTH_CHANGED                      = 0x00000020,
        PROP_NORMAL_DIR_CHANGED                 = 0x00000040,
        PROP_NORMAL_NAME_CHANGED                = 0x00000080, 
        PROP_TEXTURE_SIZE_CHANGED               = 0x00000100,
        PROP_NORMAL_VELOCITY_CHANGED            = 0x00000200,
        PROP_NORMAL_WIDTH_CHANGED               = 0x00000400,
        PROP_NORMAL_LENGTH_CHANGED              = 0x00000800,
        PROP_REFLECTION_FACTOR_CHANGED          = 0x00001000,
        PROP_TERRAIN_CHANGED                    = 0x00002000,
        PROP_DYNAMIC_ENVMAP                     = 0x00004000,
        PROP_REFRACTION_MAP                     = 0x00008000,
        PROP_DYNAMIC_REFRACTION_MAP             = 0x00010000,
        PROP_SPECULAR_VALUE_CHANGED             = 0x00020000,
        PROP_DISTURBANCE_VALUE_CHANGED          = 0x00040000,
        PROP_FOGDENSITY_VALUE_CHANGED           = 0x00080000,
        PROP_FOGDISTANCE_VALUE_CHANGED          = 0x00100000,
        PROP_FOGFALLOFF_VALUE_CHANGED           = 0x00200000,
        PROP_MAX_VERTICES_PER_SIDE_CHANGED      = 0x00400000,
        PROP_NORMAL_ANIMATION_SPEED_CHANGED     = 0x00800000,
    };

    NiUInt32 GetLength() const;
    void SetLength(NiUInt32 uiLength);

    NiUInt32 GetWidth() const;
    void SetWidth(NiUInt32 uiWidth);

    NiUInt32 GetMaxVerticesPerSide() const;
    void SetMaxVerticesPerSide(NiUInt32 uiMaxVerticesPerSide);

    NiUInt32 GetNumLengthVertices() const;
    NiUInt32 GetNumWidthVertices() const;

    NiColorA GetShallowColor() const;
    void SetShallowColor(NiColorA kColor);

    NiColorA GetDeepColor() const;
    void SetDeepColor(NiColorA kColor);

    NiFixedString GetEnvMap() const;
    void SetEnvMap(NiFixedString kEnvMap);

    float GetDepthThreshold() const;
    void SetDepthThreshold(float fDepth);

    NiFixedString GetNormalMapDir() const;
    void SetNormalMapDir(NiFixedString kDir);

    NiFixedString GetNormalFile() const;
    void SetNormalFile(NiFixedString kFile);

    NiUInt32 GetTextureSizeRatio() const;
    void SetTextureSizeRatio(NiUInt32 uiSize);

    NiPoint2 GetNormalVelocity() const;
    void SetNormalVelocity(NiPoint2 kVelocity);

    float GetNormalLength() const;
    void SetNormalLength(float fLength);

    float GetNormalWidth() const;
    void SetNormalWidth(float fWidth);

    float GetReflectionFactor() const;
    void SetReflectionFactor(float fReflection);

    NiEntityInterface* GetAssociatedTerrain() const;
    void SetAssociatedTerrain(NiEntityInterface* pkTerrain);

    NiTexture* GetDynamicEnvMap() const;
    void SetDynamicEnvMap(NiTexture* pkTexture);

    NiFixedString GetRefractionMap() const;
    void SetRefractionMap(NiFixedString kRefractionPath);

    NiTexture* GetDynamicRefractionMap() const;
    void SetDynamicRefractionMap(NiTexture* pkTexture);

    float GetSpecularity() const;
    void SetSpecularity(float fSpecularity);

    float GetDisturbance() const;
    void SetDisturbance(float fSpecularity);

    float GetFogDensity() const;
    void SetFogDensity(float fFogDensity);

    float GetFogDistance() const;
    void SetFogDistance(float fFogDistance);

    float GetFogFallOff() const;
    void SetFogFallOff(float fFogFallOff);

    float GetNormalAnimationSpeed() const;
    void SetNormalAnimationSpeed(float fSpeed);

    /** 
        Check changes to inherited properties in the master component, if
        one exists.
      */
    void CheckForChangesInMasterComponent();
};

#include "NiWaterComponent.inl"

#endif
