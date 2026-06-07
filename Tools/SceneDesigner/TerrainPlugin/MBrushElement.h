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

#ifndef MBRUSHELEMENT_H
#define MBRUSHELEMENT_H

#include <NiTerrainLib.h>

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{
    public __gc __abstract class MBrushOperation;

    /** 
        Brush elements define what layer the brush operates upon.
        
     */
    public __gc __abstract class MBrushElement : public MDisposable
    {
    public:

        /// Element name
        String* m_pmName;

        /// Whether the element is in the list or not
        bool m_bInList;

        /// Minimum value
        float m_fMinimumValue;

        /// maximum value
        float m_fMaximumValue;

        // Procedural filter variables
        float m_fMinHeight;
        float m_fMaxHeight;
        float m_fMinSlope;
        float m_fMaxSlope;
        bool m_bUseHeight;
        bool m_bUseSlope;

        MBrushOperation* m_pmAssociatedOperation;

        /** 
            Virtual destructor to allow propper deconstruction of subclasses
         */
        virtual ~MBrushElement(){}

        virtual MBrushElement* CloneOperation() = 0;

        /** 
            Retreive the current value of the vertex. The value retrieved
                is determined by subclass implementing this method.
                SUBCLASSES MUST IMPLEMENT THIS METHOD
            
            @param pkPoint a pointer to the terrain vertex object to retrive
                the value from. 
            @return The value of the point
         */
        virtual float GetValue(const NiDeformablePoint* pkPoint) = 0;

        /**
            Returns the average of this deformable point, as well as all 
            adjacent points.

            @param pkTerrain The interactor that the given deformable point was
                allocated through
            @param pkPoint The deformable point to base the smoothed value 
                around
         */
        virtual float GetSmoothedValue(NiTerrainInteractor* pkInteractor,
            NiDeformablePointSet* pkPointSet, 
            const NiDeformablePoint* pkPoint);
        
        /** 
            Set the value of the vertex. The value set is determined by 
                subclass implementation of this method. 
                SUBCLASSES MUST IMPLEMENT THIS METHOD
            
            @param pkPoint a pointer to the terrain vertex object to set
                the value on.
            @param fValue the value to set
         */
        virtual void SetValue(NiDeformablePoint* pkPoint, float fValue)= 0;

        /** 
            Finds what the value of the terrain is, at the given world location
                on a 2D xy plane. The Z value of the given world location is 
                ignored
                SUBCLASSES MUST IMPLEMENT THIS METHOD
            
            @param pkTerrain The terrain component to detect the value from
            @param kWorldLocation The world location
            @return The value of the terrain
         */
        virtual float GetValue(NiTerrainInteractor* pkTerrain, 
            const NiPoint3& kWorldLocation) = 0;

        /** 
            Commits the modified vertex set back to the terrain. 
            SUBCLASSES MUST IMPLEMENT THIS METHOD

            @param pkPoints the set of vertices that are about to be modified
            @param pkTerrain the terrain to commit to.
            @param origin The center of the points to be modified
            @params radius The distance from the center where points are 
                modified
         */
        virtual void CommitPoints(NiTerrainInteractor* pkTerrain,
            NiDeformablePointSet* pkPoints, 
            const NiPoint3& kOrigin, const float& fRadius) = 0;

        /** 
            Prepare the vertex set to be modified by this class. In the case
            of painting surfaces, this function is used to retrieve all the
            surface mask values for a particular surface.
            
            @note the default implementation does not do anything
            @param pkPoints the set of vertices that are about to be modified
            @param pkTerrain the terrain the verts belong to.
         */
        virtual unsigned int PreparePoints(const NiBound& kBound, 
            NiTerrainInteractor* pkTerrain, 
            NiDeformablePointSet* pkPoints) 
        {
            NI_UNUSED_ARG(kBound);
            NI_UNUSED_ARG(pkPoints);
            NI_UNUSED_ARG(pkTerrain);
            return 0; 
        }

        virtual void GetPointSpacing(float&){}

        /** 
            Some values have changed and the forms have to be updated.
            The form specific to an element will be updated in this function.

            This function is mainly used to populate the combo boxes for 
            each element.            
         */
        virtual void Update(MBrushOperation* pmOperation)
        {
            NI_UNUSED_ARG(pmOperation);
        }

        virtual void GetOperationFullPanel(
            System::Windows::Forms::Panel* pmPanel,
            bool bInList,
            MBrushOperation* pmOperation);

        virtual void GetOptionInformation(
            System::Windows::Forms::Label* pmElementDesc)
        {
            NI_UNUSED_ARG(pmElementDesc);
        }

        /**
            Copy all GUI relevant values from the given element, to this one.
            This function assumes that the given element is of the same type
            as this one.
         */
        virtual void ReadValues(MBrushElement* pmElement);

        /** 
            Increase the current value in the vertex by a particular amount. 
                Subclasses may re-implement this method to take advantage of 
                any possible optimizations.
            
            @param pkPoint a pointer to the terrain vertex object to set
                the value on.
            @param fValue the value to be increased by.
         */
        virtual void IncreaseBy(NiDeformablePoint* pkPoint, float fValue, 
            float fMinHeight, float fMaxHeight)
        {
            SetValue(pkPoint,NiClamp(GetValue(pkPoint)+fValue, fMinHeight,
                fMaxHeight));
        }

        /**
            Checks whether the procedural filtering values are verified for
            the given point.
            @param pkPoint The point to check
            @return True if the procedural filters are validated, 
                false otherwise
        */
        virtual bool CheckProceduralFilters(const NiDeformablePoint* pkPoint);

           /** 
            Scale the current value of the vertex towards a new value. Scale 
                the current value of the vertex towards a new value possible 
                optimizations.
            
            @param pkPoint a pointer to the terrain vertex object to set the 
                value on.
            @param fValue the value to be scaled towards.
            @param fBoundary the minimum value of the scaling.
            @param fScale The amount of scaling to perform on the overall 
                operation.
         */
        inline virtual void ScaleTowards(NiDeformablePoint *pkPoint, 
            float fValue, float fBoundary, float fScale);
        
        /** 
            Scale the current value of the vertex towards a new value ignoring
                any existing value the vertex has. Subclasses may re-implement 
                this method to take advantage of any possible optimizations.
            
            @param pkPoint a pointer to the terrain vertex object to set the 
                value on.
            @param fValue the value to be scaled towards.
            @param fBoundary the minimum value of the scaling.
            @param fScale The amount of scaling to perform on the overall 
                operation.
         */
        inline virtual void ScaleTowardsIgnorePoint(NiDeformablePoint *pkPoint,
            float fValue, float fBoundary, float fScale);  


        inline virtual void GetMinAndMax(float& fMin, float& fMax);

        inline void SetAssociatedOperation(MBrushOperation* pmOperation);

        System::Void NumericUpDown_PropertyValueChanged(
            System::Object*  sender, System::EventArgs*  e);
        System::Void CheckBox_PropertyValueChanged(
            System::Object*  sender, System::EventArgs*  e);
    };

#include "MBrushElement.inl"
}
}
}
}



#endif
