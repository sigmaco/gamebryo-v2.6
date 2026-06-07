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

#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoNodeMap.h"

#include "Model.h"
#include "Transform.h"

#include "NiNode.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Conversion of unknown node use a default converter, as spec'ed.
    //---------------------------------------------------------------------------

    bool ConvertDefault(Context& io_Context, CSLModel& in_Model)
    {
        NiNode* object = NiNew NiNode;

        return ConvertCommonProperties(io_Context, in_Model, *object);
    }
    //---------------------------------------------------------------------------
}
