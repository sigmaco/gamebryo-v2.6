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

#ifndef NIEXTRAOBJECTSSHAREDDATA_H
#define NIEXTRAOBJECTSSHAREDDATA_H

#include "NiStandardSharedDataLibType.h"
#include "NiSharedData.h"
#include "NiPluginToolkitDefinitions.h"
#include "NiTPtrSet.h"
#include <NiNode.h>
#include "NiString.h"



/// This class stores additional nodes for streaming. It is motivated by a
/// need to stream nodes that are not part of the scene graph but still
/// must be saved in a NIF file.
class NISTANDARDSHAREDDATA_ENTRY NiExtraObjectsSharedData : public NiSharedData
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiExtraObjectsSharedData();

    /// Virtual destructor.
    virtual ~NiExtraObjectsSharedData();

    /// Adds an object into the object array.
    void AddObject(NiObject* pkObj, const char* pcName = "",
        bool bStreamable = true);

    /// Removes root from the root list.
    NiObjectPtr RemoveObject(NiObject* pkObj);

    /// Removes all the objects.
    void RemoveAllObjects();

    /// Get the total number of objects in the system
    unsigned int GetObjectCount();

    /// Get a root out of the shared data by index.
    NiObject* GetObjectAt(unsigned int uiIndex);

    /// Returns the stored name of the specified root node.
    NiString GetObjectName(NiObject* pkObj);

    /// Returns whether or not the specified root node is streamable.
    bool IsObjectStreamable(NiObject* pkObj);

protected:

    /// The set of scene root nodes. This set is provided for convenience
    /// only. The root nodes are "owned" by m_spMainRoot.
    NiTObjectArray<NiObjectPtr> m_kObjects;

    class ObjectInfo : public NiRefObject
    {
    public:
        ObjectInfo(NiString strName, bool bStreamable)
        {
            m_strName = strName;
            m_bStreamable = bStreamable;
        }

        NiString m_strName;
        bool m_bStreamable;
    };
    typedef NiPointer<ObjectInfo> ObjectInfoPtr;

    /// The names and streamability corresponding to the root nodes.
    NiTMap<NiObject*, ObjectInfoPtr> m_kObjectInfoMap;

};

NiSmartPointer(NiExtraObjectsSharedData);

#endif  // #ifndef NIEXTRAOBJECTSSHAREDDATA_H
