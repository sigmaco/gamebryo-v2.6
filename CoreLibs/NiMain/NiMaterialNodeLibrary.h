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

#ifndef NIMATERIALNODELIBRARY_H
#define NIMATERIALNODELIBRARY_H

#include "NiMaterialNode.h"

class NIMAIN_ENTRY NiMaterialNodeLibrary : public NiRefObject
{
public:
    NiMaterialNodeLibrary(unsigned short usVersion);

    unsigned short GetVersion() const;
    unsigned int GetNodeCount() const;
    NiMaterialNode* GetAttachableNodeByName(const NiFixedString& kName,
        bool bClone = true);
    NiMaterialNode* GetNode(unsigned int uiNode);
    void AddNode(NiMaterialNode* pkFrag);

protected:
    NiTObjectArray<NiMaterialNodePtr> m_kNodes;
    unsigned short m_usVersion;
};

typedef NiPointer<NiMaterialNodeLibrary> NiMaterialNodeLibraryPtr;

#endif  //#ifndef NIMATERIALNODELIBRARY_H
