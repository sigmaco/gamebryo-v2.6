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

#ifndef MTERRAINPICKPOLICY_H
#define MTERRAINPICKPOLICY_H

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    /** 
        Overrides the default picking behavior for terrain entities
     */
    public __gc class MTerrainPickPolicy : public MPickUtility::MPickPolicy
    {
    public:
        MTerrainPickPolicy();

        virtual bool PickOnEntity(MEntity* pmEntity, 
            const NiPoint3& kOrigin, const NiPoint3& kDir, 
            NiPick* pkPick, Hashtable* pmPickedObjectToEntity);
    };
}
}
}
}


#endif
