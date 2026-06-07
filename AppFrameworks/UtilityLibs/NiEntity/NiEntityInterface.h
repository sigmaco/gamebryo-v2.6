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

#ifndef NIENTITYINTERFACE_H
#define NIENTITYINTERFACE_H

#include "NiEntityPropertyInterface.h"
#include "NiEntityComponentInterface.h"

// A pure virtual class that provides an entity interface.
class NIENTITY_ENTRY NiEntityInterface :
    public NiEntityPropertyInterface
{
public:
    static NiFixedString IT_ENTITYINTERFACE;

    // Returns the type of this interface.
    virtual NiFixedString GetInterfaceType();

    // Returns an instance of this entity that can contain customized data.
    virtual NiEntityInterface* Clone(const NiFixedString& kNewName,
        bool bInheritProperties) = 0;

    // Returns the master entity from which this entity inherits its
    // properties. If this entity does not have a master entity, this function
    // returns NULL.
    virtual NiEntityInterface* GetMasterEntity() const = 0;

    // Replaces the master entity from which this entity should inherit its
    // properties. This function just replaces the existing master entity
    // without changing which properties are set to inherit from its values.
    virtual void ReplaceMasterEntity(NiEntityInterface* pkMasterEntity) = 0;

    // Gets and sets whether or not the entity should be rendered.
    // Implementing classes should disable rendering if this value is true.
    virtual NiBool GetHidden() const = 0;
    virtual void SetHidden(bool bHidden) = 0;

    // Indicates whether or not this entity supports components. This value
    // may be true even though GetComponentCount returns 0 if the entity
    // supports components but does not yet contain any.
    virtual NiBool SupportsComponents() = 0;

    // Functions for managing components.
    virtual NiBool AddComponent(NiEntityComponentInterface* pkComponent,
        bool bPerformErrorChecking = true) = 0;
    virtual unsigned int GetComponentCount() const = 0;
    virtual NiEntityComponentInterface* GetComponentAt(unsigned int uiIndex)
        const = 0;
    virtual NiEntityComponentInterface* GetComponentByTemplateID(
        const NiUniqueID& kTemplateID) const = 0;
    virtual NiBool RemoveComponent(NiEntityComponentInterface* pkComponent,
        bool bPerformErrorChecking = true) = 0;
    virtual NiBool RemoveComponentAt(unsigned int uiIndex,
        bool bPerformErrorChecking = true) = 0;
    virtual void RemoveAllComponents() = 0;

    // Indicates whether or not the named property belongs to a component or
    // directly to the entity itself. If the property is not found, this
    // function returns false.
    virtual NiBool IsComponentProperty(const NiFixedString& kPropertyName, 
        bool& bIsComponentProperty) = 0;

    // Returns the entity that is the root of the prefab. If this entity 
    // is not part of a prefab this function returns NULL.
    virtual NiEntityInterface* GetPrefabRoot() const = 0;

    // Sets the prefab root entity. If this is set to NULL then this
    // entity is not part of a prefab. If this entity previously had a prefab
    // root, it will remove itself from that old root's "Prefab Entities" 
    // property.  If bAddToPrefabEntitiesProperty = true, this function will 
    // add the current entity to the new prefab root's "Prefab Entities" 
    // property. If bAddToPrefabEntitiesProperty = false, then the current 
    // entity must be added to the new prefab root's "Prefab Entities" 
    // property some other means.  
    virtual void SetPrefabRoot(NiEntityInterface* pkPrefabRoot, 
        bool bAddToPrefabEntitiesProperty = true) = 0;

    // Functions for accessing and manipulating tags.
    const NiFixedString& GetTags() const;
    void SetTags(const NiFixedString& kTags);
    void AddTag(const char* pcTag);
    void RemoveTag(const char* pcTag);
    NiBool ContainsTag(const char* pcTag);
    static char GetTagDelimiter();

    // *** begin Emergent internal use only ***
    virtual void SetMasterEntity(NiEntityInterface* pkMasterEntity) = 0;
    static void _SDMInit();
    static void _SDMShutdown();
    inline virtual ~NiEntityInterface() {}
    // *** end Emergent internal use only ***

private:
    // A string containing a set of delimited tags.
    NiFixedString m_kTags;
};

NiInterfaceSmartPointer(NiEntityInterface);

#endif // NIENTITYINTERFACE_H
