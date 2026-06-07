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

#ifndef NIENTITYCOMPONENTINTERFACE_H
#define NIENTITYCOMPONENTINTERFACE_H

#include "NiEntityPropertyInterface.h"
#include "NiUniqueID.h"

class NiEntityInterface;

// A pure virtual class that provides an entity component interface.
class NIENTITY_ENTRY NiEntityComponentInterface :
    public NiEntityPropertyInterface
{
public:
    static NiFixedString IT_COMPONENTINTERFACE;

    // Returns the type of this interface.
    virtual NiFixedString GetInterfaceType();

    // Returns an instance of this component that can contain customized data.
    virtual NiEntityComponentInterface* Clone(bool bInheritProperties) = 0;

    // Returns the master component from which this component inherits its
    // properties, if any. Returns NULL if this component does not inherit
    // its properties.
    virtual NiEntityComponentInterface* GetMasterComponent() const = 0;

    // Sets the master component from which this component should inherit its
    // properties. This function just replaces the existing master component
    // without changing which properties are set to inherit from its values.
    virtual void SetMasterComponent(
        NiEntityComponentInterface* pkMasterComponent) = 0;

    // Populates an array with all the properties that this component depends
    // on. If any of these properties are not present for an entity, the
    // component cannot be added to that entity. Property names are added to
    // the array. No names are removed from the array.
    virtual void GetDependentPropertyNames(
        NiTObjectSet<NiFixedString>& kDependentPropertyNames) = 0;

    // Returns true if this component can be attached to the specified entity.
    // The default implementation calls the static CanAttachComponentToEntity
    // function.
    virtual NiBool CanAttachToEntity(NiEntityInterface* pkEntity);

    // Returns true if this component can be removed to the specified entity.
    // The default implementation calls the static
    // CanDetachComponentFromEntity function.
    virtual NiBool CanDetachFromEntity(NiEntityInterface* pkEntity);

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    ~NiEntityComponentInterface() {}
    // *** end Emergent internal use only ***

protected:
    // Static function that returns whether or not the specified component
    // can be attached to the specified entity. It ensures that none of the
    // component's properties already exist on the entity and that the entity
    // contains all of the component's dependent properties.
    static NiBool CanAttachComponentToEntity(
        NiEntityComponentInterface* pkComponent,
        NiEntityInterface* pkEntity);

    // Static function that returns whether or not the specified component
    // can be removed from the specified entity. It ensures that the entity
    // contains the component and that none of the entity's properties depend
    // on properties in the component.
    static NiBool CanDetachComponentFromEntity(
        NiEntityComponentInterface* pkComponent,
        NiEntityInterface* pkEntity);
};

NiInterfaceSmartPointer(NiEntityComponentInterface);

#endif // NIENTITYCOMPONENTINTERFACE_H
