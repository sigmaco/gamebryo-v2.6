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

#ifndef MBRUSHTYPECIRCLE_H
#define MBRUSHTYPECIRCLE_H

#include "MBrushType.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    public __gc class MBrushTypeCircle : public MBrushType
    {
    // Member functions inherited from AeBrushType
    public:

        /** 
            Default constructor
         */
        MBrushTypeCircle();

        /** 
            Copy constructor
            @param pkCopy AeBrushTypeCircle to copy
         */
        MBrushTypeCircle(MBrushTypeCircle* pmCopy);

        inline virtual bool isInRadii(float fActiveRadiusSqr, 
            float fOuterRadiusSqr, 
            NiPoint3& kOrigin, 
            NiPoint3& kPoint, 
            bool* pbInOuterRadius, 
            float* pfDistanceSqr);

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

        virtual MBrushType* CloneBrushType();

        virtual bool SetupBrushOverlay();

        virtual void UpdateBrushOverlay(NiPoint3& kCenter, 
            NiTerrainInteractor* pkTerrainComponent, 
            NiBrushPassPointInfo* pkCategorisedPoints);

        virtual void GetBrushOptionPanel(
            System::Windows::Forms::Panel* pmPanel);

        virtual bool ResizeBrushOverlay(float fCurrentScale);

        System::Void NumericUpDown_ValueChanged(
            System::Object*  sender, System::EventArgs*  e);

        System::Void RadioButton_CheckedChanged(System::Object*  sender,
            System::EventArgs*  e);

        System::Void Slider_ValueChanged(System::Object*  sender,
            System::EventArgs*  e);
            
    private:

        /** 
            Brief. Detailed after fullstop.
            
            @param kIntersection
            @param pkTerrainComponent
            @param fIntersectionHeight
            @return
         */
        NiBool GetBoundaryIntersection( NiPoint3& kIntersection, 
            NiTerrainInteractor* pkTerrainComponent, 
            float& fIntersectionHeight);
        
    private:
        ///
        static unsigned short ms_usVertices = ms_usInnerSegments + 
            ms_usOuterSegments + 2;

    };

    #include "MBrushTypeCircle.inl"

}
}
}
}

#endif
