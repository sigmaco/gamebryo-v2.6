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

#ifndef NISORTSWITCHNODE_H
#define NISORTSWITCHNODE_H

#include "NiAccumulator.h"
#include "NiNode.h"

class NIMAIN_ENTRY NiSortAdjustNode : public NiNode
{
    NiDeclareRTTI;
    NiDeclareClone(NiSortAdjustNode);
    NiDeclareStream;

public:
    NiSortAdjustNode();
    virtual ~NiSortAdjustNode();

    enum SortingMode
    {
        SORTING_INHERIT,
        SORTING_OFF,
        SORTING_MAX
    };

    inline void SetSortingMode(SortingMode eMode);
    SortingMode GetSortingMode() const;

    virtual void OnVisible(NiCullingProcess& kCuller);

    // *** begin Emergent internal use only ***
    virtual void ProcessClone(NiCloningProcess& kCloning);
    // *** end Emergent internal use only ***

protected:
    SortingMode m_eSortingMode;
};


typedef NiPointer<NiSortAdjustNode> NiSortAdjustNodePtr;

//---------------------------------------------------------------------------
//  Inline include
#include "NiSortAdjustNode.inl"


#endif
