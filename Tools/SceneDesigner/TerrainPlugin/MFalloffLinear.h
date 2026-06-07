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

#ifndef MFALLOFFLINEAR_H
#define MFALLOFFLINEAR_H

#include "MFalloff.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    public __gc class MFalloffLinear : public MFalloff
    { 
    
    public:

        virtual float CalcFalloff(float fDist);

    
    protected:

        void Do_Dispose(bool bDisposing)
        {
            NI_UNUSED_ARG(bDisposing);
        }

    };
}
}
}
}

#endif
