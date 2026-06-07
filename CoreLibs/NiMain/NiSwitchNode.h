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

#ifndef NISWITCHNODE_H
#define NISWITCHNODE_H

#include "NiNode.h"
#include "NiStream.h"

class NIMAIN_ENTRY NiSwitchNode : public NiNode
{
    NiDeclareRTTI;
    NiDeclareClone(NiSwitchNode);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

public:
    // construction and destruction
    NiSwitchNode();

    // child attaching/detaching
    virtual NiAVObjectPtr DetachChild(NiAVObject* pkChild);
    virtual NiAVObjectPtr DetachChildAt(unsigned int i);

    // switch operations
    inline NiAVObject* GetActiveChild() const;
    inline void SetIndex(int iIndex);
    inline int GetIndex() const;

    // *** begin Emergent internal use only ***

    // culling (separate from drawing)
    virtual void OnVisible(NiCullingProcess& kCuller);

    // *** end Emergent internal use only ***

protected:
    // --- Begin NIF conversion code
    // Deprecated flags - use new flag code
    enum
    {
        MAX_POS = 9
    };
    // --- End NIF conversion code


    int m_iIndex;
};

typedef NiPointer<NiSwitchNode> NiSwitchNodePtr;

#include "NiSwitchNode.inl"

#endif
