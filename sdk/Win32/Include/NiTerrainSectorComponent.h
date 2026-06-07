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

#ifndef NITERRAINSECTORCOMPONENT_H
#define NITERRAINSECTORCOMPONENT_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include <NiEntityComponentInterface.h>
#include <NiFlags.h>

#include "NiTerrainLibType.h"
#include "NiSurfacePalette.h"

#include "NiTerrainSector.h"
#include "NiTerrainComponent.h"

NiSmartPointer(NiTerrainSectorComponent);

/**
    This class serves as an NiEntity wrapper around the NiTerrainSector class.

    The Terrain Sector is a manager class for a collection of terrain blocks,
    contained within a quadtree. It keeps track of data streams, decals, 
    surface masks and the quad-trees of blocks themselves.

    This class should not be interacted with directly by the end application
    for anything other than attaching dynamic effects, lights, shadowing etc.
*/
class NITERRAIN_ENTRY NiTerrainSectorComponent : public NiRefObject,
    public NiEntityComponentInterface
{

public:
    /// @name Properties
    //@{ 
    static NiFixedString ms_kPropDeformable;
    static NiFixedString ms_kPropTerrainID;
    static NiFixedString ms_kPropMaskSize;
    static NiFixedString ms_kPropSectorIndex;
    static NiFixedString ms_kPropSceneRoot;
    static NiFixedString ms_kPropHasChanged;
    //@}

    /// @name Sibling properties (Transformation)
    //@{ 
    static NiFixedString ms_kPropStorageFileName;
    static NiFixedString ms_kPropSectors;
    static NiFixedString ms_kPropBlockSize;
    static NiFixedString ms_kPropNumLOD;
    static NiFixedString ms_kPropCalcSectorSize;
    static NiFixedString ms_kPropTerrainMinHeight;
    static NiFixedString ms_kPropTerrainMaxHeight;
    //@}

public:

    /// Default Constructor
    NiTerrainSectorComponent();

    /// Destructor
    ~NiTerrainSectorComponent();

    /**
    Tells the contained sector to save to the currently defined location.
    If this is a backup save, data is saved to a backup directory within
    the current terrain directory instead.
    */
    bool Save(const char* pcArchive, NiEntityErrorInterface* pkError = 0);

    /**
    Tells the contained sector to load from the currently defined location.
    If this is a backup, data is loaded from the backup directory within
    the current terrain directory instead.
    */
    bool Load(const char* pcArchive, NiEntityErrorInterface* pkErrors = 0);

    /**
    Works out whether the component is a template or not.
    */
    bool IsTemplate();

    /// @cond EMERGENT_INTERNAL

    /// Returns a pointer to the internal NiSector
    NiTerrainSector* GetSector() const;

    /// Returns a pointer to the internal NiSector, cast as a node
    NiNode* GetSectorAsNode() const;

    /// Unloads all height data, so that it may be re-loaded according to 
    /// current settings during the next call to update.
    void Unload();

    static void _SDMInit();
    static void _SDMShutdown();

    /// @endcond

    /**
        Returns whether or not the settings have changed since the last update
    */
    inline bool HasSettingChanged() const;

    /**
        Lets the sector know that it needs to process settings in the next
        update
    */
    inline void MarkSettingChanged();

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
    NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiFixedString& kData, NiUInt32 uiIndex = 0) const;
    NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiFixedString& kData, NiUInt32 uiIndex = 0);

    /// Getter and Setter for Boolean properties
    NiBool GetPropertyData(const NiFixedString& kPropertyName,
        bool& bData, NiUInt32 uiIndex = 0) const;
    NiBool SetPropertyData(const NiFixedString& kPropertyName,
        bool bData, NiUInt32 uiIndex = 0);

    /// Getter and Setter for NiObject* properties
    NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiObject*& pkData, NiUInt32 uiIndex = 0) const;
    NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiObject* pkData, NiUInt32 uiIndex = 0);

    /// Getter and Setter for NiUInt32 int properties
    NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiUInt32& uiData, NiUInt32 uiIndex = 0) const;
    NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiUInt32 uiData, NiUInt32 uiIndex = 0);
    //@}
    
protected:

    static NiFixedString ms_kClassName;
    static NiFixedString ms_kComponentName;

    static NiFixedString ms_kDescDeformable;
    static NiFixedString ms_kDescTerrainID;
    static NiFixedString ms_kDescMaskSize;
    static NiFixedString ms_kDescSectorIndex;
    static NiFixedString ms_kDescHasChanged;

    static NiFixedString ms_kErrorNoTerrainComponent;

    // Bit Flags
    NiDeclareFlags(NiUInt8);

    enum FLAGS
    {
        PROP_DEFORMABLE_CHANGED             = 0x01,
        PROP_TERRAIN_ID_CHANGED             = 0x02,
        PROP_MASK_SIZE_CHANGED              = 0x04,
        PROP_SETTING_CHANGED                = 0x08
    };

    // Property Data
	bool m_bPropDeformable;
    NiFixedString m_kPropTerrainID;
    NiInt16 m_iPropSectorIndex[2];
    NiUInt32 m_uiPropMaskSize;
    bool m_bHasChanged;

    // Parent Component
    NiTerrainSectorComponentPtr m_spMasterComponent;

    // A pointer to the terrain component that we link to
    NiTerrainComponentPtr m_spTerrainComponent;

    // The terrain sector that we are responsible for
    NiTerrainSectorPtr m_spSector;

    /// Property data access functions
    //@{
    const NiFixedString& GetPropTerrainID() const;
    void SetPropTerrainID(const NiFixedString& kTerrainID);
    NiUInt32 GetPropMaskSize() const;
    bool SetPropMaskSize(NiUInt32 uiSize);
    bool GetPropDeformable() const;
    void SetPropDeformable(bool bDeformable);
    NiInt16 GetPropSectorIndexX() const;
    void SetPropSectorIndexX(NiInt16 uiIndexX);
    NiInt16 GetPropSectorIndexY() const;
    void SetPropSectorIndexY(NiInt16 uiIndexY);
    //@}
   
    /// Parameterized constructor, copies inheritable values from the parent 
    /// component 
    NiTerrainSectorComponent(NiTerrainSectorComponent* pkTemplateComponent);
};

#include "NiTerrainSectorComponent.inl"

#endif // NITERRAINSECTORCOMPONENT_H
