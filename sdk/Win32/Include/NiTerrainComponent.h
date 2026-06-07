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

#ifndef NITERRAINCOMPONENT_H
#define NITERRAINCOMPONENT_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include <NiEntityComponentInterface.h>
#include <NiFlags.h>

#include "NiTerrainLibType.h"
#include "NiMetaDataStore.h"
#include "NiTerrainInteractor.h"
#include "NiTerrain.h"
#include "NiRay.h"
#include "NiScene.h"

NiSmartPointer(NiTerrainComponent);

/**
    The terrain component is attached to an NiEntity. It manages a loaded 
    terrain, but does not actually have any geometry associated with it. 
    It holds links to other NiTerrainSectorComponent objects, that hold 
    actual sector geometry
 */
class NITERRAIN_ENTRY NiTerrainComponent : public NiRefObject,
    public NiEntityComponentInterface
{
    NiDeclareFlags(NiUInt32);
    friend class NiTerrainInteractor;
    
public:

    enum
    {
        /// Indicates that the filename has changed.
        PROP_STORAGE_FILENAME_CHANGED   = 0x001,

        /// Indicates that the sectors has changed.
        PROP_SECTORS_CHANGED            = 0x002,

        /// Indicates that the LOD count has changed.
        PROP_NUMLOD_CHANGED             = 0x004,

        /// Indicates that the block-size has changed.
        PROP_BLOCKSIZE_CHANGED          = 0x008,

        /// Indicates that the environment map has changed.
        PROP_ENVMAP_CHANGED             = 0x010,

        /// Indicates that setting changes.
        PROP_SETTING_CHANGED            = 0x020,

        /// Indicates that the minimum elevation has changed.
        PROP_MIN_ELEVATION_CHANGED      = 0x040,

        /// Indicates that the maximum elevation has changed.
        PROP_MAX_ELEVATION_CHANGED      = 0x080,

        /// Indicates that the mask size has changed.
        PROP_MASK_SIZE_CHANGED          = 0x100,
    
        /// Indicates the the cook physX setting has changed.
        PROP_COOKPHYSX_CHANGED          = 0x200
    };
    
    /// @name Properties
    //@{
    static NiFixedString ms_kPropStorageFileName;
    static NiFixedString ms_kPropSectorList;
    static NiFixedString ms_kPropBlockSize;
    static NiFixedString ms_kPropNumLOD;
    static NiFixedString ms_kPropCalcSectorSize;
    static NiFixedString ms_kPropInteractor;   
    static NiFixedString ms_kPropSceneRoot;
    static NiFixedString ms_kPropTerrainMinHeight;
    static NiFixedString ms_kPropTerrainMaxHeight;
    static NiFixedString ms_kPropMaskSize;
    static NiFixedString ms_kPropCookPhysXData;
    //@}

    /// @name Depricated Properties
    //@{
    static NiFixedString ms_kPropSectors;
    //@}

    /// @name Sibling properties (Transformation)
    //@{
    static NiFixedString ms_kPropTranslation;
    static NiFixedString ms_kPropRotation;
    static NiFixedString ms_kPropScale;
    //@}

    /// @name Descriptions
    //@{
    static NiFixedString ms_kDescStorageFileName;
    static NiFixedString ms_kDescSectorList;
    static NiFixedString ms_kDescBlockSize;
    static NiFixedString ms_kDescNumLOD;
    static NiFixedString ms_kDescCalcSectorSize;
    static NiFixedString ms_kDescTerrainMinHeight;
    static NiFixedString ms_kDescTerrainMaxHeight;
    static NiFixedString ms_kDescMaskSize;
    static NiFixedString ms_kDescCookPhysXData;
    //@}

    /// @name Class name
    //@{
    static NiFixedString ms_kClassName;
    static NiFixedString ms_kComponentName;
    //@}

    /// @name Constructors
    //@{

    /** 
        Default constructor. If a null meta data store is given, then the 
        default static meta data store singleton is used.

        @param pkMetaDataStore Optional meta data store that will be used by
        surfaces in this component
    */
    NiTerrainComponent(NiMetaDataStore* pkMetaDataStore = 0);
    //@}

    /// Destructor
    virtual ~NiTerrainComponent(void);

    /// @name Static Terrain Management 
    //@{
    /**
        Static function that handles loading terrain components from the given
        scene. This assumes that the scene has already been streamed in and
        NiTerrainComponent::Update has been called at least once.

        @return true if at least one terrain loads returns true.
    */
    static bool LoadTerrain(NiScene* pkScene, 
        NiExternalAssetManager* pkAssetManager,NiDefaultErrorHandler* pkError);

    /// Remove a terrain component from the terrain component list.
    static void Remove(NiTerrainComponent* pkTerrainComponent);

    /// Retrieve a list of all known terrain components in memory.
    static const NiTPointerList<NiTerrainComponent*>* GetTerrainComponents();

    /// Retrieves the terrain component on the entity if there is one.
    static NiTerrainComponent* FindTerrainComponent(const NiEntityInterface*
        pkEntity);

    /**
        Function that works out the appropriate error messages depending on 
        the given error code.
        @param uiErrorCode the error code to analyse
        @param pkError The error interface
        @param bSaving flag defining where the error occured
    */
    static void GenerateErrorMessages(NiUInt32 uiErrorCode,
        NiEntityErrorInterface* pkError,
        bool bSaving = false);

    //@}

    /// @name Terrain Interface
    //{@
    /// Get the terrain object that this component is an interface to. 
    NiTerrain* GetTerrain();

    /// Get the terrain object that this component is an interface to.
    const NiTerrain* GetTerrain() const;
    //@}   

    /// @cond EMERGENT_INTERNAL
    /** @name Sector management */
    //@{
    /**
        @return the number of attached sector components that currently have 
        data loaded in memory.
    */
    inline NiUInt32 GetNumLoadedSectors() const;
    //@}
    /// @endcond

    /** @name Surfaces */
    //@{

    /**
        The active surface palette is defined to be the default static surface 
        palette, unless a specific palette has been assigned to this component 
        directly via SetSurfacePalette.

        @returns The active surface palette for this component.
    */
    inline NiSurfacePalette* GetActiveSurfacePalette() const;

    /**
        Return the custom surface palette that has been optionally assigned to 
        this component. If no palette has been assigned, and the component is 
        using the default static palette, this function returns NULL.

        @return The custom surface palette currently in use by this component
        (if any)
    */
    inline NiSurfacePalette* GetSurfacePalette() const;

    /**
        @return the default static surface palette singleton
    */
    inline static NiSurfacePalette* GetStaticSurfacePalette();

    /**
        Optionally set the surface palette that any new surfaces created by 
        this component will be added to. If no custom surface palette is 
        defined, the component will add all surfaces to the static palette 
        singleton.

        @param spSurfacePalette Custom surface palette to add new surfaces too
    */
    inline void SetSurfacePalette(NiSurfacePalette* spSurfacePalette);
    //@}

    /**
        Causes all attached sectors to unload all mesh and surface data. The
        next time a sector is updated, it will then reload its data from disk.
    */
    void UnloadData();

    /**
        Trigger a save of all attached sectors data to disk, in the directory 
        and format currently specified by this component.
    */
    bool Save(const char* pcArchive, NiEntityErrorInterface* pkErrors = 0);

    /**
        Trigger a load of all attached sectors data from disk, in the 
        directory and format currently specified by this component.
    */
    bool Load(const char* pcArchive, NiEntityErrorInterface* pkErrors = 0);

    /**
        Works out whether the component is a template or not. Returns true if
        it is the case.
    */
    bool IsTemplate();

    /**
        Retrieves the component's unique ID. Returns m_kUniqueTerrainID.
    */
    NiFixedString GetUniqueComponentName();

    /// @cond EMERGENT_INTERNAL
    static void _SDMInit();
    static void _SDMShutdown();
    /// @endcond

    /** @name NiEntityComponentInterface overrides */
    //@{
    virtual NiEntityComponentInterface* Clone(bool bInheritProperties);
    virtual NiEntityComponentInterface* GetMasterComponent() const;
    virtual void SetMasterComponent(
        NiEntityComponentInterface* pkMasterComponent);
    virtual void GetDependentPropertyNames(
        NiTObjectSet<NiFixedString>& kDependentPropertyNames);
    //@}

    /** @name NiEntityPropertyInterface overrides */
    //@{
    virtual NiBool SetTemplateID(const NiUniqueID& kID);
    virtual NiUniqueID GetTemplateID();
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetClassName() const;
    virtual NiFixedString GetName() const;
    virtual NiBool SetName(const NiFixedString& kName);
    virtual NiBool IsAnimated() const;
    virtual void Update(NiEntityPropertyInterface* pkParentEntity,
        float fTime, NiEntityErrorInterface* pkErrors,
        NiExternalAssetManager* pkAssetManager);
    virtual void BuildVisibleSet(NiEntityRenderingContext* pkRenderingContext,
        NiEntityErrorInterface* pkErrors);
    virtual void GetPropertyNames(
        NiTObjectSet<NiFixedString>& kPropertyNames) const;
    virtual NiBool CanResetProperty(const NiFixedString& kPropertyName,
        bool& bCanReset) const;
    virtual NiBool ResetProperty(const NiFixedString& kPropertyName);
    virtual NiBool MakePropertyUnique(const NiFixedString& kPropertyName,
        bool& bMadeUnique);
    virtual NiBool GetDisplayName(const NiFixedString& kPropertyName,
        NiFixedString& kDisplayName) const;
    virtual NiBool SetDisplayName(const NiFixedString& kPropertyName,
        const NiFixedString& kDisplayName);
    virtual NiBool GetPrimitiveType(const NiFixedString& kPropertyName,
        NiFixedString& kPrimitiveType) const;
    virtual NiBool SetPrimitiveType(const NiFixedString& kPropertyName,
        const NiFixedString& kPrimitiveType);
    virtual NiBool GetSemanticType(const NiFixedString& kPropertyName,
        NiFixedString& kSemanticType) const;
    virtual NiBool SetSemanticType(const NiFixedString& kPropertyName,
        const NiFixedString& kSemanticType);
    virtual NiBool GetDescription(const NiFixedString& kPropertyName,
        NiFixedString& kDescription) const;
    virtual NiBool SetDescription(const NiFixedString& kPropertyName,
        const NiFixedString& kDescription);
    virtual NiBool GetCategory(const NiFixedString& kPropertyName,
        NiFixedString& kCategory) const;
    virtual NiBool IsPropertyReadOnly(const NiFixedString& kPropertyName,
        bool& bIsReadOnly);
    virtual NiBool IsPropertyUnique(const NiFixedString& kPropertyName,
        bool& bIsUnique);
    virtual NiBool IsPropertySerializable(const NiFixedString& kPropertyName,
        bool& bIsSerializable);
    virtual NiBool IsPropertyInheritable(const NiFixedString& kPropertyName,
        bool& bIsInheritable);
    virtual NiBool IsExternalAssetPath(const NiFixedString& kPropertyName,
        NiUInt32 uiIndex, bool& bIsExternalAssetPath) const;
    virtual NiBool SetElementCount(const NiFixedString& kPropertyName,
        NiUInt32 uiCount,bool &bCountSet);
    virtual NiBool GetElementCount(const NiFixedString& kPropertyName,
        NiUInt32& uiCount) const;
    virtual NiBool IsCollection(const NiFixedString& kPropertyName,
        bool& bIsCollection) const;

    /**
        Get NiFixedString property data.
    */
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiFixedString& kData, NiUInt32 uiIndex = 0) const;
    /**
        Set NiFixedString property data.
    */
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiFixedString& kData, NiUInt32 uiIndex = 0);

    /**
        Get NiUInt32 property data.
    */
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiUInt32& uiData, NiUInt32 uiIndex = 0) const;
    /**
        Set NiUInt32 property data.
    */
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiUInt32 uiData, NiUInt32 uiIndex = 0);

    /**
        Get Terrain interactorproperty data.
    */
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        void*& pvData, size_t& stDataSizeInBytes, NiUInt32 uiIndex = 0)
        const;
    /**
        Set Terrain interactorproperty data.
    */
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const void* pvData, size_t stDataSizeInBytes,
        NiUInt32 uiIndex = 0);

    /**
        Get NiInt32 property data.
    */
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiInt32& iData, NiUInt32 uiIndex) const;
    
    /**
        Set NiInt32 property data.
    */
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiInt32 iData, NiUInt32 uiIndex);

    /**
        Get NiObject* property data.
    */
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiObject*& pkData, NiUInt32 uiIndex) const;
    
    /**
        Set NiObject* property data.
    */
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiObject* pkData, NiUInt32 uiIndex);
        
    /**
        Get NiPoint2 property data.
    */
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiPoint2& kData, NiUInt32 uiIndex) const;
    
    /**
        Set NiPoint2 property data.
    */
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiPoint2& kData, NiUInt32 uiIndex);

    /**
        Get NiBool property data.
    */
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        bool& bData, NiUInt32 uiIndex) const;
    
    /**
        Set NiBool property data.
    */
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        bool bData, NiUInt32 uiIndex);
    //@}
    
protected:

    // Terrain Object to manipulate:
    NiTerrainPtr m_spTerrain;

    // Component Management
    static NiTPointerList<NiTerrainComponent*>* ms_pkTerrainComponents;

    // Parent Component
    NiTerrainComponentPtr m_spMasterComponent;

    // Local 'working copy' of terrain details. Changes to these need to be
    // 'committed' to the terrain when changed.
    NiFixedString m_kPropStorageFileName;
    NiUInt32 m_auiPropSectors[2];
    NiUInt32 m_uiPropBlockSize;
    NiUInt32 m_uiPropNumLOD;  
    NiUInt32 m_uiPropMaskSize;
    NiTPrimitiveSet<NiUInt32> m_kSectorList;
    NiInt32 m_iTerrainMinHeight;
    NiInt32 m_iTerrainMaxHeight;

    // Member sector information
    NiFixedString m_kUniqueTerrainID;

    // Enable physx cooking when saving in scene designer
    bool m_bCookPhysXData;

    // Get and Set of settings
    const NiFixedString& GetPropStorageFilename() const;
    void SetPropStorageFilename(const NiFixedString& kFilename);
    const NiTPrimitiveSet<NiUInt32>& GetPropSectorList() const;
    void SetPropSectorList(const NiTPrimitiveSet<NiUInt32>& kSectorList);
    bool SetPropSectorList(NiUInt32 uiIndex, const NiPoint2& kValue);
    NiUInt32 GetPropSectorsX() const;
    void SetPropSectorsX(NiUInt32 uiSectorsX);
    NiUInt32 GetPropSectorsY() const;
    void SetPropSectorsY(NiUInt32 uiSectorsY);
    NiUInt32 GetPropBlockSize() const;
    bool SetPropBlockSize(NiUInt32 uiBlockSize);
    NiUInt32 GetPropNumLOD() const;
    inline bool SetPropNumLOD(NiUInt32 uiNumLOD);
    NiUInt32 GetPropMaskSize() const;
    bool SetPropMaskSize(NiUInt32 uiSize);
    NiUInt32 GetPropCalcSectorSize() const;    
    NiInt32 GetPropTerrainMinHeight() const;
    bool SetPropTerrainMinHeight(NiInt32 iNewValue);
    NiInt32 GetPropTerrainMaxHeight() const;
    bool SetPropTerrainMaxHeight(NiInt32 iNewValue);
    bool GetPropCookPhysXData() const;
    bool SetPropCookPhysXData(bool bNewValue);

    /**
        Returns whether or not the settings have changed since the last update
    */
    bool HasSettingChanged() const;

    // Cloning constructor
    NiTerrainComponent(NiTerrainComponent* pkTemplateComponent,
        NiMetaDataStore* pkMetaDataStore = 0);

    void Initialize();
};

#include "NiTerrainComponent.inl"

#endif // NITERRAINCOMPONENT_H
