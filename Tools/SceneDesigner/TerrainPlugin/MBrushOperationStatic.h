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

#ifndef MBRUSHOPERATIONSTATIC_H
#define MBRUSHOPERATIONSTATIC_H

#include "MBrushOperation.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    /** 
        Derived class that implements MBrushOperation specific to an 
        alignment operation.
        
     */
    public __gc class MBrushOperationStatic : public MBrushOperation
    {    
    
    public:
        
        /** 
            Default constructor
            
         */
        MBrushOperationStatic();

        /** 
            Copy constructor
            
            @param pkCopy object to copy
         */
        MBrushOperationStatic(MBrushOperationStatic* pmCopy);        
        
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

        virtual void GetOptionInformation(
            System::Windows::Forms::Label* pmElementDesc);

        System::Void NumericUpDown_PropertyValueChanged(
            System::Object*  sender, System::EventArgs*  e);

        System::Void TrackBar_ValueChanged(System::Object*  sender,
            System::EventArgs*  e);
    };
}
}
}
}

#endif
