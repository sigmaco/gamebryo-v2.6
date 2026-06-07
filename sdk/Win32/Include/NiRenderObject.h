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

#ifndef NIRENDEROBJECT_H
#define NIRENDEROBJECT_H

#include "NiAVObject.h"
#include "NiCullingProcess.h"
#include "NiDynamicEffectState.h"
#include "NiFixedString.h"
#include "NiPropertyState.h"
#include "NiMaterialInstance.h"
#include "NiTSet.h"

class NiSkinInstance;
class NiMeshHWInstance;

class NIMAIN_ENTRY NiRenderObject : public NiAVObject
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiRenderObject);
    NiDeclareStream;
    NiDeclareViewerStrings;
public:    
    
    /// Default constructor
    NiRenderObject();

    /// Virtual destructor
    virtual ~NiRenderObject();

    /// @name State Access
    //@{
    /// Get the property state
    inline NiPropertyStatePtr GetPropertyState() const;
    /// Set the property state
    inline void SetPropertyState(NiPropertyState* pkState);
    /// Get the effect state
    inline NiDynamicEffectStatePtr GetEffectState() const;
    /// Set the effect state
    inline void SetEffectState(NiDynamicEffectState* pkState);
    //@}

    /// Get the sort object flag
    inline void SetSortObject(bool bSort);
    /// Set the sort object flag
    inline bool GetSortObject() const;

    /// Render the mesh immediately
    virtual void RenderImmediate(NiRenderer* pkRenderer) = 0;

    /** 
    @name Public NiShader Helper Methods
    Helper method for working with NiShaders attached to this mesh. 
    */
    //@{
    /// Returns the cached NiShader for the active material
    NiShader* GetShader() const;
    /// Returns the NiShader from the active material, 
    /// generating a new one if necessary.
    NiShader* GetShaderFromMaterial();
    /// Legacy method for attaching an NiSingleShaderMaterial to the mesh.
    void SetShader(NiShader* pkShader);
    //@}

    /**
    RequiresMaterialOption may be used by materials to determine 
    whether or not they are expected to do some work. 
    
    @param kMaterialOption The material option string.
    @return True means that:
        - the material should implement kMaterialOption, and
        - the mesh has the required data for implementing kMaterialOption. 
    False means that:
        - the material should not implement kMaterialOption.
    */
    virtual bool RequiresMaterialOption(const NiFixedString& kMaterialOption)
        const = 0;

    /** 
    @name Public NiMaterial Helper Methods
    NiMaterial system helper methods
    */
    //@{
    /**
    Apply a material to this mesh.

    @param kName The name of the NiMaterial to apply
    @param uiMaterialExtraData Any extra data for the material (Optional).
    @return Whether or not the operation succeeded.
    */
    bool ApplyMaterial(const NiFixedString& kName,
        NiUInt32 uiMaterialExtraData = 
        NiMaterialInstance::DEFAULT_EXTRA_DATA);

    /**
    Apply a material to this mesh.

    @param pkMaterial The NiMaterial to apply
    @param uiMaterialExtraData Any extra data for the material (Optional).
    @return Whether or not the operation succeeded.
    */
    bool ApplyMaterial(NiMaterial* pkMaterial,
        NiUInt32 uiMaterialExtraData = 
        NiMaterialInstance::DEFAULT_EXTRA_DATA);

    /**
    Apply a material to this mesh and make it the active material.

    @param kName The name of the NiMaterial to apply
    @param uiMaterialExtraData Any extra data for the material (Optional).
    @return Whether or not the operation succeeded.
    */
    bool ApplyAndSetActiveMaterial(const NiFixedString& kName,
        NiUInt32 uiMaterialExtraData = 
        NiMaterialInstance::DEFAULT_EXTRA_DATA);

    /**
    Apply a material to this mesh and make it the active material.

    @param pkMaterial The NiMaterial to apply
    @param uiMaterialExtraData Any extra data for the material (Optional).
    @return Whether or not the operation succeeded.
    */
    bool ApplyAndSetActiveMaterial(NiMaterial* pkMaterial,
        NiUInt32 uiMaterialExtraData = 
        NiMaterialInstance::DEFAULT_EXTRA_DATA);

    /// Remove the named material from the mesh.
    inline bool RemoveMaterial(const NiFixedString& kName);

    /// Remove the material from the mesh.
    bool RemoveMaterial(NiMaterial* pkMaterial);

    /// Is the named material applied?
    inline bool IsMaterialApplied(const NiFixedString& kName) const;

    /// Is the material applied?
    bool IsMaterialApplied(const NiMaterial* pkMaterial) const;

    /// Set the active material to the named material (It must already be 
    /// attached for this to succeed).
    inline bool SetActiveMaterial(const NiFixedString& kName);

    /// Set the active material to the input material (It must already be 
    /// attached for this to succeed).
    bool SetActiveMaterial(const NiMaterial* pkMaterial);

    /// Get the active material
    inline const NiMaterial* GetActiveMaterial() const;

    /// Get the active NiMaterialInstance
    inline const NiMaterialInstance* GetActiveMaterialInstance() const;

    /// Sets the flag indicating that the materials for this NiMesh need 
    /// to be updated before rendering with them.
    void SetMaterialNeedsUpdate(bool bFlag);

    /// Sets the flag indicating that the materials for this NiMesh always
    /// need to be updated before rendering with them.
    inline void SetMaterialNeedsUpdateDefault(bool bFlag);

    /// Get whether or not NiMaterials are always considered dirty.
    inline bool GetMaterialNeedsUpdateDefault() const;
    //@}

    /** 
    @name Public Static NiMaterial Helper Methods
    NiMaterial system helper methods
    */
    //@{
    /// Static method for determining whether or not to apply the default 
    /// material
    static bool GetStreamingAppliesDefaultMaterial();

    /// Static method for determining whether or not to apply the renderer's 
    /// default material when streaming in.
    static void SetStreamingAppliesDefaultMaterial(bool bApplyDefault = true);

    /// Recursive function that will set the 
    /// NiRenderObject::SetMaterialNeedsUpdate flag on all NiRenderObject 
    /// objects in the input subtree.
    static void RecursiveSetMaterialNeedsUpdate(
        NiAVObject* pkObject, 
        bool bDirty);

    /// Recursive function that will set the 
    /// NiRenderObject::SetMaterialNeedsUpdateDefault flag on all 
    /// NiRenderObject objects in the input subtree.
    static void RecursiveSetMaterialNeedsUpdateDefault(
        NiAVObject* pkObject, 
        bool bDirty);
    //@}

    /// Returns true if the specified semantic exists
    virtual bool ContainsData(const NiFixedString& kSemantic, 
        NiUInt32 uiSemanticIndex = 0) const = 0;

    /// Returns the number of elements with the specified semantic across all
    /// semantic indices.
    virtual NiUInt32 GetSemanticCount(
        const NiFixedString& kSemantic) const = 0;
    
    // *** begin Emergent internal use only ***
    inline NiUInt32 GetMaterialCount();
    inline const NiMaterialInstance* GetMaterialInstance(NiUInt32 uiInstance);
    bool ComputeCachedShader(NiUInt32 uiMaterial);
    void ClearCachedShaderAndSetNeedsUpdate(NiUInt32 uiMaterial);

    /// culling (separate from display)
    virtual void OnVisible(NiCullingProcess& kCuller) = 0;
    /// cloning
    virtual void ProcessClone(NiCloningProcess& kCloning);    

    /// renderer data
    virtual void PurgeRendererData(NiRenderer* pkRenderer);
    // *** end Emergent internal use only ***

protected:
    inline void AppendToCullingProcess(NiCullingProcess& kCuller);

    /// Property state cache
    NiPropertyStatePtr m_spPropertyState;
    /// Effect state cache
    NiDynamicEffectStatePtr m_spEffectState;

    enum { NO_MATERIAL = UINT_MAX };

    /// Static flag indicating whether or not streaming applies the renderer's
    /// default material.
    static bool ms_bStreamingAppliesDefaultMaterial;
    /// Materials
    NiTObjectSet<NiMaterialInstance> m_kMaterials;

    /// Active material
    unsigned int m_uiActiveMaterial;

    /// Bit to determine whether the material always be considered dirty
    bool m_bMaterialNeedsUpdateDefault;
};

// note these are stars, not smart pointers
typedef NiTPointerList<NiRenderObject*> NiRenderObjectList;

NiSmartPointer(NiRenderObject);

#include "NiRenderObject.inl"

#endif // #ifndef NIRENDEROBJECT_H
