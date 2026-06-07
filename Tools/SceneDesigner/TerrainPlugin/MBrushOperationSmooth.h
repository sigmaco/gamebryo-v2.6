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

#ifndef MBRUSHOPERATIONSMOOTH_H
#define MBRUSHOPERATIONSMOOTH_H

#include <NiTArray2D.h>

#include "NiTQuickReleasePool.h"
#include "MBrushOperation.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{
    /** 
         
        Derived class that implements the smoothing operation.
     */
    public __gc class MBrushOperationSmooth : public MBrushOperation
    {
    public:
        
        MBrushOperationSmooth();

        MBrushOperationSmooth(MBrushOperationSmooth* pmCopy);
        
        ~MBrushOperationSmooth();

        virtual MBrushOperation* CloneOperation();
        
        virtual void ApplyOperation(NiBrushPassPointInfo* pkActivePoints, 
            NiPoint3* pkOrigin, float fRadius, float fMinHeight, 
            float fMaxHeight);

        virtual void ApplyFalloff(NiBrushPassPointInfo* pkPoints,
            float fMinHeight, float fMaxHeight) 
        {
            NI_UNUSED_ARG(fMaxHeight);
            NI_UNUSED_ARG(fMinHeight);
            NI_UNUSED_ARG(pkPoints);
        }

        virtual void GetOperationFullPanel(
            System::Windows::Forms::Panel* pmPanel,
            bool bInList);

    protected:
        
        virtual void Do_Dispose(bool bDisposing);

    };
}
}
}
}

#endif
