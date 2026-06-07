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

#ifndef MBrush_H
#define MBrush_H

#include <NiTerrainLib.h>

#include "ManagedMacros.h"
#include "MBrushType.h"
#include "MBrushElement.h"
#include "MBrushOperation.h"
#include "MFalloff.h"
#include "MBrushCommand.h"

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{
    using namespace System;
    using namespace System::Windows::Forms;
    using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
    using namespace Emergent::Gamebryo::SceneDesigner::Framework;
    using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

    /** 
        AeBrush class is a singleton. It holds all the information regarding 
            the currently selected brush size, shape, operation and what 
            elements to apply the operation to.
            
        @see IInteractionMode documentation in GameBryo
     */
    public __gc class MBrush : public MViewInteractionMode
    {
    public:

        MBrush();

        /** 
            Get inner radius of current brush.
            
            @return Returns the size of the inner radius as a float
         */
        inline float GetSizeInner(); 

        /** 
            Get outer radius of current brush.
            
            @return Returns the size of the outer radius as a float
         */
        inline float GetSizeOuter(); 

        /// Scale of the currently selected terrain
        //@{
        inline void SetScale(float fScale);
        inline float GetScale();
        //@}

        /// Control updating of lighting whilst painting on the terrain. 
        inline void SetUpdateLightingRealtime(bool bValue);

        /** 
            Set the Brush type from a user selection.
            
            @param uiBrushTypeIndex An unsigned int representing the index
                of the registered brush type to use. The index correspondance
                should be defined when initialising the list of shapes
                
            @return true if successful
         */
        bool SetBrushType(NiUInt32 uiBrushTypeIndex, 
            const NiFixedString& kBrushType);

        /** 
            Set the falloff function to apply to the brush.
            
            @param kFalloffType A string representing the desired falloff 
                function. Can take one of the following values:
                #SCULPT_FALLOFF_LINEAR
                #SCULPT_FALLOFF_SINE
            @return true if successful
         */
        bool SetSculptFalloff(const NiFixedString& kFalloffType);

        /** 
            Get the type of brush currently used.
            
            @return a pointer to the brush being used.
         */
        MBrushType* GetBrushType();

        System::Collections::Generic::List<MBrushOperation*>* GetOperations();

        bool MoveOperationTo(MBrushOperation* pmOperation, int iIndex);

        void CommandsFinished(ICommand* amCommands[]);

        void UpdateOverlay();

        __property String* get_Name();
        virtual bool Initialize(); 
        virtual void Update(float fTime);
        virtual void RenderGizmo(MRenderingContext* pmRenderingContext);
        virtual void MouseDown(MouseButtonType eType, int iX, int iY);
        virtual void MouseUp(MouseButtonType eType, int iX, int iY);
        virtual void MouseMove(int iX, int iY);

    public:

        /// Sculpt Falloff Elements
        static System::String* ms_pmBrushFalloffLinear; ///< "Linear"
        static System::String* ms_pmBrushFalloffSine; ///< "Sine"

        /// Static Initialization:
        static void _SDMInit(void);
        static void _SDMShutdown(void);

        /// brush type registrations
        static void _RegisterBrushType(MBrushType* pmBrushType);

        /// Collection of registered brush types
        static System::Collections::Generic::List<MBrushType*>* 
            ms_pmRegisteredBrushTypes;

        /// Registering custom options
        void RegisterCustomSettings(String* pmOptionCategoryName,
            String* pmOptionName, 
            String* pmOptionDescription,
            System::UInt32& uiValue,
            SettingChangedHandler* pmHandler);

    protected:

        /** 
            Begin the painting operation. Calculates the values for the
                rest of the operation
            
            @param kIntersection The point of intersection of the brush with 
                the terrain
            @param eType Type of mouse action that triggered the event.
         */
        void StartPaint(NiPoint3& kIntersection,
            IInteractionMode::MouseButtonType eType);

        /** 
            Continue painting the operation. Uses pre-calculated values
                from the StartPaint function call.
            
            @param kIntersection The point of intersection of the brush with 
                the terrain
            @param bRecategorise Set to true if the vertex set has been 
                updated since another paint call.
         */
        void Paint(NiPoint3& kIntersection,bool bRecategorise);

        /** 
            Stop and reset the painting operation.
            
         */
        void StopPaint();

        virtual void Do_Dispose(bool bDisposing);

    private:

        /** 
            Terrain Helper Functions. 
            
            @param fPixelX
            @param fPixelY
            @param pkIntersection
            @return
         */
        NiBool GetTerrainIntersection(float fPixelX, float fPixelY, 
            NiPoint3* pkIntersection);

        /** 
            Performs appropriate actions when the left mouse button is pressed
            
         */
        void OnLeftClick(int iX, int iY);

        void OnSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

    protected:

        /// Current brush index
        System::UInt32 m_uiCurrentBrushType;
        
        /// Pointer to the selected brush type.
        //MBrushType* m_pmActiveBrush; 

        /// List of selected operation
        System::Collections::Generic::List<MBrushOperation*>* 
            m_pmListOfOperations;

        /// Pointer to the selected falloff function
        MFalloff* m_pmActiveFalloff;

    private:

        /// Variable that remembers if an undo frame is open
        bool m_bCommandFrame;        

        /// Update lighting after every operation?
        bool m_bUpdateLightingRealtime;

        /// terrain has been selected
        NiBool m_bTerrainSelected;    

        /// internal variable used during painting
        NiBrushPassPointInfo* m_pkPassPointInfo; 

        /// keep the view-port mouse was clicked in.
        MViewport* m_pmActiveViewport;        

        /// mouse button pressed
        MouseButtonType m_eMouseBtn;  

        /// number of vertex in above array.
        unsigned int m_uiNumDeformablePoints;    

        // Size check of a stroke
        unsigned int m_uiStrokeSize;

        // Maximum size of a stroke
        System::UInt32 m_uiMaxStrokeSize;

        // Maximum number of strokes held in the undo list
        System::UInt32 m_uiMaxNumberOfStrokes;

        // number of points before changing brush color
        System::UInt32 m_uiNumWarningPoints;

        /// The scale of the currently selected terrain
        float m_fCurrentScale;

        /// Variables for managing the list of origins used by previous
        /// operation during the drag
        NiPoint3* m_pkListOfOrigins;
        int m_iNumberOfOrigins;
        int m_iMaxOrigins;
        int m_iCurrentPosition;

        MBrushCommand* m_pmPreviousCommand;

        /// Flag to resize the overlay when this failed on loading
        bool m_bResizeOverlaySuccess;

        /// Details on any intersection with the terrain
        float m_fIntersectionHeight;
        float m_fIntersectionSlope;
    };

    #include "MBrush.inl"
}
}
}
}

#endif
