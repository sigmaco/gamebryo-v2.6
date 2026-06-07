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

#ifndef MBRUSHOPERATIONFLATTEN_H
#define MBRUSHOPERATIONFLATTEN_H

#include "MBrushOperation.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    public __gc class MBrushOperationFlatten : public MBrushOperation
    {
    public:
     
        /** 
            Default constructor
         */
        MBrushOperationFlatten();

        /** 
            Copy constructor
            
            @param pkCopy Object to copy (AeBrushOperationFlatten)
         */
        MBrushOperationFlatten(MBrushOperationFlatten* pkCopy);

        virtual MBrushOperation* CloneOperation();
        
        virtual void StartOperation(NiPoint3& kOrigin,
            NiBrushPassPointInfo* pkActivePoints, 
            IInteractionMode::MouseButtonType eType
            );
        
        virtual void ApplyOperation(NiBrushPassPointInfo* pkActivePoints, 
            NiPoint3* pkOrigin, float fRadius, float fMinHeight, 
            float fMaxHeight);

        virtual void ApplyFalloff(NiBrushPassPointInfo* pkPoints,
            float fMinHeight, float fMaxHeight);
        
        virtual void GetOperationFullPanel(
            System::Windows::Forms::Panel* pmPanel,
            bool bInList);
    
    private:
        /// Defines how the flattening should be performed
        float m_fFlattenValue;

    };
}
}
}
}

#endif