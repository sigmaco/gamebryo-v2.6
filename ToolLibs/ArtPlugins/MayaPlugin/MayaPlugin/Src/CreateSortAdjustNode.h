// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
// 
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
// 
// Copyright (c) 1996-2008 Emergent Game Technologies.
// All Rights Reserved.
// 
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef CREATESORTADJUSTNODE_H
#define CREATESORTADJUSTNODE_H

#include "MayaPluginPCH.h"


class CreateSortAdjustNode
{
public:
    void ProcessScene(NiNodePtr spScene);
    
protected:
    void SortAdjustReplaceNode(NiNodePtr spNode, 
        NiSortAdjustNode::SortingMode eType);
    
    void RecursiveReplaceNodes(NiAVObject* pkRoot);

    bool ScanExtraData(NiNode* pkNode, 
        NiSortAdjustNode::SortingMode& eType, NiUInt32& uiDataIndex);
};

#endif