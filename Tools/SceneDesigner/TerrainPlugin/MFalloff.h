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

#ifndef MFALLOFF_H
#define MFALLOFF_H

#include <NiTerrainComponent.h>

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
    using namespace Emergent::Gamebryo::SceneDesigner::Framework;

    /** 
        Abstract class for calculating falloff.
        
     */
    public __gc __abstract class MFalloff : public MDisposable
    { 
    
    public:
        /** 
            Calculates the fall off for the applied operation
            
            @param fDist distance of the fall off
            @return returns the result of the calculation
         */
        inline virtual float CalcFalloff(float fDist);
    };

#include "MFalloff.inl"

}
}
}
}

#endif