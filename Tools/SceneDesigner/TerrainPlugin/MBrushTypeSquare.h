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

#ifndef MBRUSHTYPESQUARE_H
#define MBRUSHTYPESQUARE_H

#include "MBrushType.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    /** 
         Class that implements a AeBrushType to create a square tool
        
     */
    public __gc class MBrushTypeSquare : public MBrushType
    {
    public:
        //===================================================================
        // Constructors
        //===================================================================
        
        /** 
            Default constructor
            
         */
        MBrushTypeSquare();
        
        /** 
            Copy constructor
            
            @param pkCopy AeBrushTypeSqare object to copy
         */
        MBrushTypeSquare(MBrushTypeSquare* pmCopy);
    
        //==================================================================
        // Member functions inherited from AeBrushType
        //==================================================================
        virtual bool SetupBrushOverlay();
        
        virtual void UpdateBrushOverlay(NiPoint3& kCenter, 
            NiTerrainInteractor* pkTerrainComponent, 
            NiBrushPassPointInfo* pkCategorisedPoints);
        
        virtual void CategoriseVerticies(NiPoint3& kIntersection, 
            unsigned int uiNumVerticies, 
            NiDeformablePointSetPtr spActivePoints, 
            NiBrushPassPointInfo* pkCategorisedPoints, 
            MFalloff* pmFalloffFunction, 
            NiTerrainInteractor* pkTerrainComponent,
            float fTerrainScale);
        
        virtual void CalculateBoundaryValues(
            NiPoint3 kIntersection, float fInnerRadius, float fOuterRadius,
            NiBrushPassPointInfo* pkPassPointInfo, 
            NiTerrainInteractor* pkTerrainComponent,
            MBrushElement* pmActiveElement);
        
        virtual bool ResizeBrushOverlay(float fCurrentScale);

        virtual void GetBrushOptionPanel(
            System::Windows::Forms::Panel* pmPanel)
        {
            NI_UNUSED_ARG(pmPanel);
        }

        virtual MBrushType* CloneBrushType();
    };

}
}
}
}

#endif