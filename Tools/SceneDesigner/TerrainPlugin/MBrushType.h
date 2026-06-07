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

#ifndef MBRUSHTYPE_H
#define MBRUSHTYPE_H

#include "MFalloff.h"
#include "NiBrushPassPointInfo.h"
#include "MBrushElement.h"

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
    using namespace Emergent::Gamebryo::SceneDesigner::Framework;

    public __gc __abstract class MBrushType : public MDisposable
    {
    public:

        /** 
            Sets the size of the inner tool.
            @param fSize The size to use
         */
        void SetSizeInner(float fSize);
        
        /** 
            Sets the size of the outer tool.
            @param fSize the size to use
         */
        void SetSizeOuter(float fSize);
        
        /** 
            Returns the size of the inner tool
            @return the size of the inner tool
         */
        float GetSizeInner();
        
        /** 
            Returns the size of the outer tool
            @return the size of the outer tool
         */
        float GetSizeOuter();

        virtual void RegisterBrushTypeOptions();
        
        void RegisterCustomSettings(String* pmOptionCategoryName,
            String* pmOptionName, 
            String* pmOptionDescription,
            NiColorA& kValue,
            SettingChangedHandler* pmHandler);       
        
        /** 
            This function will organize the given points into categories to
                facilitate their handling
            
            @param kIntersection The intersection point with the terrain 
                (center of the brush)
            @param uiNumVerticies Number of vertices contained in the point 
                set to modify
            @param spActivePoints Point set to be ordered and modified
            @param pkCategorisedPoints OUTPUT: Re-organised points
            @param pkFalloffFunction OUTPUT: Fall off operation to apply
            @param pkTerrainComponent The terrain component to interact with
         */
        virtual void CategoriseVerticies(NiPoint3& kIntersection, 
            unsigned int uiNumVerticies, 
            NiDeformablePointSetPtr spActivePoints, 
            NiBrushPassPointInfo* pkCategorisedPoints, 
            MFalloff* pmFalloffFunction, 
            NiTerrainInteractor* pkTerrainComponent,
            float fTerrainScale) = 0;
        
        virtual void CalculateBoundaryValues(
            NiPoint3 kIntersection, float fInnerRadius, float fOuterRadius,
            NiBrushPassPointInfo* pkPassPointInfo, 
            NiTerrainInteractor* pkTerrainInteractor,
            MBrushElement* pmActiveElement) = 0;

        /** 
            Pure virtual function that clones a created object of type 
                AeBrushType. MUST BE IMPLEMENTED BY DERIVED CLASSES.
            
            @return Retuns a pointer to a new AeBrushType object
         */
        virtual MBrushType* CloneBrushType() = 0;

        
        /** 
           Returns the brushoverlqy for this brush type
           @return The brush overlay
         */
        NiMesh* GetBrushOverlay();


        /** 
            Pure virtual function that sets the appropriate overlay for the 
                tool in use.
                MUST BE IMPLEMENTED BY DERIVED CLASSES
            
         */
        virtual bool SetupBrushOverlay() = 0;


        /** 
            Pure virtual function that resizes the overlay for the tool in use.
                MUST BE IMPLEMENTED BY DERIVED CLASSES

            @return True if the resize was successful, false otherwise
            
         */
        virtual bool ResizeBrushOverlay(float fCurrentScale) = 0;


        /** 
            Pure virtual function that updates the overlay of the current tool.
            MUST BE IMPLEMENTED BY DERIVED CLASSES
            
            @param kCenter Center point of the overlay
            @param pkTerrainComponent Associated terrain component
            @param pkCategorisedPoints Organized points where to draw the 
                overlay
         */
        virtual void UpdateBrushOverlay(NiPoint3& kCenter, 
            NiTerrainInteractor* pkTerrainComponent, 
            NiBrushPassPointInfo* pkCategorisedPoints) = 0;

        /**
            Pure virtual function that computes the command panel's options
            for a specific brush type. This function is virtual pure as it
            must be created for every derived class of MBrushType

            @param pmPanel The panel we are going to fill with brush type 
                specific options
        */
        virtual void GetBrushOptionPanel(
            System::Windows::Forms::Panel* pmPanel) = 0;

        virtual void SetColor(NiColor kColor);

        virtual void SetColor(NiColorA kColor);

        virtual void SetColor(String* pkColorName);

        /**
            Function that checks whether the brsuh type can be used to paint
            or not. A painting command should not be done if this function 
            returns false.
        */
        virtual bool ValidateBrushType();

        /**
            This optional function is used to randomize some brush options
            after every painting operation
        */
        virtual void Randomize() {}

    protected:
        /** 
            checks the given point if its in the given radius (active or outer)
                of the brush shape.
                fOuterRadius should be greater than or equal to fActiveRadius.

            @param fActiveRadiusSqr the square of the radius of the shape of 
                where the full effect of the brush is applied
            @param fOuterRadiusSqr the square of the outer radius of the shape 
                where there is an interpolation of the effect for smoothing 
                effects
            @param kOrigin the center point of the brush effect shape
            @param kPoint the point to check
            @param pbInOuterRadius OUTPUT: true if point is within the outer 
                radius. This will be true as well if point is with active 
                radius as well.
            @param pDistanceSqr OUTPUT: the distance squared between kOrigin 
                and kPoint.
            @return if point is within active radius.
         */
        inline virtual bool isInRadii(float fActiveRadiusSqr, 
            float fOuterRadiusSqr, 
            NiPoint3& kOrigin, 
            NiPoint3& kPoint, 
            bool* pbInOuterRadius, 
            float* pfDistanceSqr
            );

        void OnColorSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        virtual void Do_Dispose(bool bDisposing);

        /// Base color
        NiColorA* m_pkBaseColor;

        /// Set colors
        NiColorA* m_pkAllowedColor;
        NiColorA* m_pkWarningColor;
        NiColorA* m_pkStopColor;

        /// Size of the inner tool of the brush
        float m_fSizeInner;

        /// Size of the outer tool of the brush
        float m_fSizeOuter;

        /// the square of m_fSizeActive
        float m_fSizeInnerSqr;    

        /// the square of m_fSizeOuter
        float m_fSizeOuterSqr;    

        /// brush shape drawing hint
        NiMesh* m_pkBrushOverlay; 

        /// boolean to remember the fall off type
        bool m_bUseLinearFallOff;

    public:

        static unsigned short ms_usInnerSegments = 32;
        static unsigned short ms_usOuterSegments = 64;

        System::String* m_pmBrushTypeName;
    };

#include "MBrushType.inl"

}
}
}
}

#endif
