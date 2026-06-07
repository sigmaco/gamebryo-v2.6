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

#ifndef NICLONINGPROCESS_H
#define NICLONINGPROCESS_H

#include "NiMainLibType.h"
#include "NiObjectNET.h"
#include "NiTPointerMap.h"

class NiObject;

class NIMAIN_ENTRY NiCloningProcess : public NiMemObject
{
public:
    enum CloneRelationBehavior
    {
        CLONE_RELATION_NONE,
        CLONE_RELATION_CLONEDONLY,
        CLONE_RELATION_ALL
    };
    inline NiCloningProcess(unsigned int uiHashSize = 37) :
        m_eAffectedNodeRelationBehavior(CLONE_RELATION_CLONEDONLY),
        m_eDynamicEffectRelationBehavior(CLONE_RELATION_CLONEDONLY)
    {
        m_pkCloneMap = NiNew NiTPointerMap<NiObject*, NiObject*>(uiHashSize);
        NIASSERT(m_pkCloneMap != NULL);
        m_pkProcessMap = NiNew NiTPointerMap<NiObject*, bool>(uiHashSize);
        NIASSERT(m_pkProcessMap != NULL);
        m_eCopyType = NiObjectNET::GetDefaultCopyType();
        m_cAppendChar = NiObjectNET::GetDefaultAppendCharacter();
    }

    inline ~NiCloningProcess()
    {
        NiDelete m_pkCloneMap;
        NiDelete m_pkProcessMap;
    }

    NiTPointerMap<NiObject*, NiObject*>* m_pkCloneMap;
    NiTPointerMap<NiObject*, bool>* m_pkProcessMap;
    NiObjectNET::CopyType m_eCopyType;
    CloneRelationBehavior m_eAffectedNodeRelationBehavior;
    CloneRelationBehavior m_eDynamicEffectRelationBehavior;
    char m_cAppendChar;
};

#endif // NICLONINGPROCESS_H
