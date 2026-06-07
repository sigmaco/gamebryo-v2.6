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

#ifndef MBRUSHTYPEMASK_H
#define MBRUSHTYPEMASK_H

#include "MBrushType.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    public __gc class MBrushTypeMask : public MBrushType
    {
    // Member functions inherited from AeBrushType
    public:

        /** 
            Default constructor
         */
        MBrushTypeMask();

        /** 
            Copy constructor
            @param pkCopy MBrushTypeMask to copy
         */
        MBrushTypeMask(MBrushTypeMask* pmCopy);

        virtual void Do_Dispose(bool bDisposing);

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

        virtual bool ValidateBrushType();

        virtual void Randomize();

        /**
            Event function that catches all the NumericUpDown value changed 
            event. There is only one for all the controls so it uses the 
            sender argument to resolve which control triggered the event
        */
        System::Void NumericUpDown_ValueChanged(
            System::Object*  sender, System::EventArgs*  e);

        /**
            Event function that catches all the TrackBar value changed 
            event. There is only one for all the controls so it uses the 
            sender argument to resolve which control triggered the event
        */
        System::Void Slider_ValueChanged(System::Object*  sender,
            System::EventArgs*  e);

        /**
            Event function that catches all the Button clicked 
            event. There is only one for all the controls so it uses the 
            sender argument to resolve which control triggered the event
        */
        System::Void Button_Click(System::Object*  sender,
            System::EventArgs*  e);

        /**
            Event function that catches all the ComboBox return on change 
            event. There is only one for all the controls so it uses the 
            sender argument to resolve which control triggered the event
        */
        System::Void ComboBox_ChangeReturn(System::Object*  sender,
            System::Windows::Forms::KeyPressEventArgs*  e);

        /**
            Event function that catches all the ComboBox selection changed 
            event. There is only one for all the controls so it uses the 
            sender argument to resolve which control triggered the event
        */
        System::Void ComboBox_SelectionChanged(System::Object*  sender,
            System::EventArgs*  e);

        /**
            Event function that catches all the ComboBox draw items 
            event. It defines how to draw each items in the list. This is 
            mainly used to draw an reduced version of the images in the list
        */
        System::Void ComboBox_DrawItems(System::Object*  sender, 
            DrawItemEventArgs* e);

        /**
            Event function that catches all the CheckBox checked changed 
            event. There is only one for all the controls so it uses the 
            sender argument to resolve which control triggered the event
        */
        System::Void CheckBox_CheckedChanged(System::Object*  sender,
            System::EventArgs*  e);

        /**
            Once an the mask is changed, we call on this function to apply the 
            changes to the overlay and the image control. This is always called
            from a control event (after a button click, after combobox 
            selection changed...)

            @param pmFileName the filename of the image to use
            @param pmControl Control that called the event. Will allow to get
                The image control and modify it
        */
        bool SetAndConvertMaskFile(Control *pmControl, 
            System::String* pmFileName);
            
    protected:

        /// @cond EMERGENT_INTERNAL
        /**
            Structure that defines the variables to keep for the combo box
        */
        __gc struct ItemRecord
        {
            Bitmap* pmImage;
            String* pmFilePath;
            String* pmName;
        };
        /// @endcond

        /** 
            Function that gets the height value of a point at a given location.
            It casts a ray downwards using the location information given by 
            the first parameter. It return the height of the intersected point.

            @param kIntersection Point that has a valid x and y to test against 
                the terrain
            @param pkTerrainComponent Terrain to collide with
            @param fIntersectionHeight return value giving the height of the 
                point
            @return return true if succeeded, false otherwise
         */
        NiBool GetBoundaryIntersection( NiPoint3& kIntersection, 
            NiTerrainInteractor* pkTerrainComponent, 
            float& fIntersectionHeight);

        /// @cond EMERGENT_INTERNAL
        /**
            Function that adds an ItemRecord to m_pmAddedItems.
            This function also checks whether the item has already be added.
            If it hasn't it will add it to the combo box.
        */
        void AddItem(System::String* pmFileName, Control* pmControl);
        /// @endcond

        //{@
        /**
            Helper functions that create part of the GUI panel. Each part
            is returned in the TableLayoutPanel given in parameter
        */
        void BuildStandardOptions(
            System::Windows::Forms::TableLayoutPanel* pmTablePanel);
        void BuildRandomizationOptions(
            System::Windows::Forms::TableLayoutPanel* pmTablePanel);
        void BuildAdvancedOptions(
            System::Windows::Forms::TableLayoutPanel* pmTablePanel);
        //@}
        
    private:
        ///
        static unsigned short ms_usVertices = 64;

        /// List that stores all previously added height masks
        System::Collections::Generic::List<ItemRecord*>* m_pmAddedItems;
    protected:

        /// Stores the neutral color (grey scale)
        NiUInt32 m_uiNeutralColor;

        /// Stores the range of the random size
        float m_fRandomSizeRange;

        /// Stores the range of the random rotation
        float m_fRandomRotationRange;
        
        /// Stores the demi side length of the square tool
        float m_fSideLength;

        /// Stores the side length value when randomized. This is done so we
        /// can keep the original side length
        float m_fRandomizedSideLength;

        /// Orientation angle in radians (only affects rotation arround z)
        /// This is set by the GUI
        float m_fRotationAngle;

        /// Stores the rotation value when randomized. This is done so we
        /// can keep the original rotation
        float m_fRandomizedRotationAngle;
        
        /// Pointer to the height mask pixel data associated with this intance
        /// of the brush type
        NiPixelData* m_pkMaskPixData;

        /// The rotation applied to the overlay
        NiMatrix3* m_pkRotation;

        /// Full file path used to load the height mask
        System::String* m_pmMaskPath;        

        /// Flag defining whether to use size randomization or not
        bool m_bUseRandomSize;

        /// Flag defining whether to use rotation randomization of not
        bool m_bUseRandomRotation;

        /// Flag defining whether to use bilinear filtering or not
        bool m_bUseFiltering;

        /// Flag defining whether to invert the colors of the mask or not
        bool m_bUseInvertMask;

        /// Flag defining whether to align to the camera or not
        bool m_bUseAlignToCamera;
    };

}
}
}
}

#endif
