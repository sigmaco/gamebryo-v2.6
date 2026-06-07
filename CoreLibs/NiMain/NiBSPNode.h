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

#ifndef NIBSPNODE_H
#define NIBSPNODE_H

#include "NiNode.h"
#include "NiPlane.h"
#include "NiStream.h"

class NIMAIN_ENTRY NiBSPNode : public NiNode
{
    NiDeclareRTTI;
    NiDeclareClone(NiBSPNode);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
        // construction
    NiBSPNode();

    // tree operations
    inline void AttachLeft(NiAVObject* pkChild);
    inline void AttachRight(NiAVObject* pkChild);
    inline NiAVObjectPtr DetachLeft();
    inline NiAVObjectPtr DetachRight();
    inline NiAVObjectPtr GetLeft();
    inline NiAVObjectPtr GetRight();

    // plane methods
    inline void SetModelPlane(const NiPlane& kPlane);
    inline const NiPlane& GetModelPlane() const;
    inline const NiPlane& GetWorldPlane() const;
    inline int WhichSide(const NiPoint3& kP) const;

    // *** begin Emergent internal use only ***

    // culling (separate from display)
    virtual void OnVisible(NiCullingProcess& kCuller);

    // *** end Emergent internal use only ***

protected:
    // update during scene tree traversal
    virtual void UpdateWorldData();
    void UpdateWorldPlane();

    NiPlane m_kModelPlane;
    NiPlane m_kWorldPlane;
};


typedef NiPointer<NiBSPNode> NiBSPNodePtr;

//---------------------------------------------------------------------------
//  Inline include
#include "NiBSPNode.inl"

//---------------------------------------------------------------------------


#endif

