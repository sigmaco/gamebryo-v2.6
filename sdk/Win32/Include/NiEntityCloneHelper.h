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

#ifndef NIENTITYCLONEHELPER_H
#define NIENTITYCLONEHELPER_H

#include "NiEntityLibType.h"
#include <NiMemObject.h>
#include "NiEntityInterface.h"
#include "NiTPtrSet.h"

// This class is designed to help when cloning sets of entities. It has two
// primary functions that are not handled by NiEntityInterface::Clone.
// 1) It clones prefabs atomically. The standard Clone function, when given
//    a prefab root, will clone the root but not the prefab entities. This
//    class will clone the entire prefab and will contain pointers to all
//    of the entities that were cloned as a result.
// 2) Reference fixup. The standard Clone function does not change entity
//    reference pointers. If two entities are cloned, and one refers to the
//    other (i.e. lights), then it will still point to the original entity,
//    not the new clone. This is often wrong, especially in the case of
//    prefabs. This class will make a followup pass and fixup all references
//    to cloned objects.
//
// It should be used by instantiating the clone helper, then calling the
// Clone function for each entity in the set. When it is done, you should
// call the Fixup function to fixup any entity pointers.
class NIENTITY_ENTRY NiEntityCloneHelper : public NiMemObject
{
public:
    NiEntityCloneHelper();

    // Clones the given entity. If the entity is a prefab, this function
    // returns recursively clones all of its prefab entities. In either case,
    // the cloned entity/prefab root and any prefab entities are  added to an
    // internal array and may be accessed via the GetEntityAt member function.
    // The Fixup function must be called before any cloned entities can be
    // accessed. The specified new name is used for the first entity only. If
    // the entity is a prefab, the prefab entity clones will have the same
    // name as their source entities. This function can only be called
    // before calling Fixup.
    void Clone(
        NiEntityInterface* pkEntity,
        const NiFixedString& kNewName,
        bool bInheritProperties);

    // Fixes up all internal pointer references to entities that were cloned.
    // This function must be called after all desired entities have been 
    // cloned before any cloned entities can be accessed. It cannot be called 
    // more than once without calling Reset().
    void Fixup(bool bClearExternalReferences = false);

    // Finds the clone of the given entity. Pass in the pointer to the 
    // original entity, and this function will return the pointer to its new
    // clone. If the entity was not cloned, it returns NULL. The Fixup
    // function must be called prior to calling this function.
    NiEntityInterfaceIPtr Lookup(NiEntityInterface* pkOriginalEntity) const;

    // Returns the number of entities that have resulted from the cloning 
    // operation. This is not necessarily one-to-one with the number that
    // were cloned due to prefabs. The Fixup function must be called prior
    // to calling this function.
    unsigned int GetEntityCount() const;

    // Return the cloned entity internally stored at the specified index. The
    // index provided should never be greater than the value returned by
    // GetEntityCount. The Fixup function must be called prior to calling this
    // function.
    NiEntityInterfaceIPtr GetEntityAt(unsigned int uiIndex) const;

    // Resets the cloning object, removing all internally stored cloned
    // entities.
    void Reset();

private:
    NiEntityInterface* RecursiveClone(
        NiEntityInterface* pkEntity,
        const NiFixedString& kNewName,
        bool bInheritProperties);
    void FixupProperty(
        NiEntityInterface* pkEntity,
        const NiFixedString& kPropName,
        bool bClearExternalReferences);

    NiTObjectPtrSet<NiEntityInterfaceIPtr> m_kEntities;
    NiTPointerMap<NiEntityInterface*, NiEntityInterface*> m_kLookupMap;
    bool m_bFixupCalled;
};

#endif  // #ifndef NIENTITYCLONEHELPER_H
