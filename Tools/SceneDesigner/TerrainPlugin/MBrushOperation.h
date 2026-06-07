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

#ifndef MBRUSHOPERATION_H
#define MBRUSHOPERATION_H

#include "NiBrushPassPointInfo.h"
#include "MBrushElement.h"

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    /** 
        Class that allows the definition of different operations for the brush.
     */
    public __gc __abstract class MBrushOperation : public MDisposable
    {
    public:

        /// Defines the strength at which the operation should be performed
        float m_fStrength;

        /// Percentage
        float m_fPercent;
        
        /// Defines the Element associated with the operation
        MBrushElement* m_pmElement;

        /// Defines the Operation name
        String* m_pmName;

        /// Defines the Operation icon
        System::Drawing::Image* m_pmOperationIcon;

        /// Tells whether the operation is part of the list or not
        bool m_bInList;

        /// Boolean that determines whether already modified vertices should
        /// be modified again when applying the operation.
        /// IMPORTANT NOTE: when creating a custom operation, this boolean 
        /// should be used and set so to have a default value whether it is
        /// going to be used or not. If in doubt initialize this member to true
        /// in the constructor of your operation and copy it in the copy 
        /// constructor.
        bool m_bUseAllVertices;
        
        MBrushOperation();

        MBrushOperation(MBrushOperation* pmCopy);

        /** 
            Cloning function. Allows copying of the information held by an
            instance of a derived object.
            @return A base NiBrushOperation freshly allocated
         */
        virtual MBrushOperation* CloneOperation() = 0;
        
        /** 
            Initializes the operation to be done
            
            @param kOrigin The point where to perform the operation
            @param pkActivePoints A set of points that will contain the 
                affected points by the operation
            @param eType The detected mouse event to know which operation to 
                perform
         */
        virtual void StartOperation(NiPoint3& kOrigin,
            NiBrushPassPointInfo* pkActivePoints, 
            IInteractionMode::MouseButtonType eType)
        {
            NI_UNUSED_ARG(eType);
            NI_UNUSED_ARG(pkActivePoints);
            NI_UNUSED_ARG(kOrigin);
            NI_UNUSED_ARG(pkActivePoints);
        };
        
        /** 
            Executes the operation.
            
            @param pkActivePoints The points to be modified by the operation
         */
        virtual void ApplyOperation(NiBrushPassPointInfo* pkActivePoints, 
            NiPoint3* pOrigin, float fRadius, float fMinHeight, 
            float fMaxHeight)
        {
            NI_UNUSED_ARG(fMaxHeight);
            NI_UNUSED_ARG(fMinHeight);
            NI_UNUSED_ARG(fRadius);
            NI_UNUSED_ARG(pkActivePoints);
            NI_UNUSED_ARG(pOrigin);
        };


        /** 
            Default implementation that calculates and apply fall off. This can
            be overridden if needed however it will usually be used as such.
            
            @param pkPoints Point information about the currently modified
                set of points for which a fall off should be applied
         */
        virtual void ApplyFalloff(NiBrushPassPointInfo* pkPoints, 
            float fMinHeight, float fMaxHeight);

        /** 
            Stops the operation.
         */
        virtual void StopOperation() {}
        
        /** 
            Gets a control panel for the operation. An operation is responsible
            for creating its interface depending on its options. The panel
            will then be dynamically added to the scene designer user 
            interface hence easying the creation of custom operations.
            
            @return A panel control which should contain all needed controls.
                That control panel should be set as scrollable as well as being
                self managed in the operation class (events etc...)
            @param bInList A boolean that specifies whether the operation is
                being added to the list or not. This alters the panel returned
                as in the list the picture box is not necessary
         */
        virtual void GetOperationFullPanel(
            System::Windows::Forms::Panel* pmPanel,
            bool bInList)
        {
            NI_UNUSED_ARG(bInList);
            NI_UNUSED_ARG(pmPanel);
        }

        /** 
            Fills in the text value for the label in parameter briefly 
                describing properties of the operation
            
            @param m_ElementDesc Label to modify
         */
        virtual void GetOptionInformation(
            System::Windows::Forms::Label* pmElementDesc);
        
        /**
            Copy all GUI relevant values from the given operation, to this one.
            This function assumes that the given operation is of the same type
            as this one.
         */
        virtual void ReadValues(MBrushOperation* pmOperation);

        /** 
            Some values have changed and the forms have to be updated.
            The form specific to an operation will be updated in this function
            that which then call the AeBrushElement::Update() function to have
            the forms belonging to the element updated.

            This function is mainly used to populate the combo boxes for 
            each operation and for each elements.
            
         */
        virtual void Update(bool bUpdateElement);
        
    protected:

        void Do_Dispose(bool bDisposing);

        /// variables used to create the panel and place the widgets
        int m_iNextPropertyY;
        int m_iNextPropertyX;

    };

}
}
}
}

#endif
