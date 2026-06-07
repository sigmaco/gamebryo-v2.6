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

#ifndef NIBRUSHCOMMANDINTERFACE_H
#define NIBRUSHCOMMANDINTERFACE_H

#include <vcclr.h> //to use managed coded variables

#include "MBrushType.h"
#include "MBrushElement.h"
#include "MBrushOperation.h"
#include "MFalloff.h"

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{
    using namespace System;
    using namespace System::Windows::Forms;
    using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
    using namespace Emergent::Gamebryo::SceneDesigner::Framework;

    NiSmartPointer(NiBrushCommandInterface);

    class NiBrushCommandInterface : public NiEntityCommandInterface,
        public NiRefObject
    {

    private:
        /// Name of the command
        NiFixedString m_kName;

        /// Smart pointer to the active pointset
        NiDeformablePointSetPtr m_spActivePointSet;

        /// Component we are acting upon;
        NiTerrainInteractor* m_pkTerrainInteractor;

        /// The following members are there to remember the state of the 
        /// environement when the do command is called
        
        /// Array that stores the original values of points that have changed
        NiDeformablePointSetPtr m_spOriginalPointSet;

        /// The brush to use
        gcroot<MBrushType*> m_pmActiveBrush;

        /// The operation that should be done on the elements
        gcroot<MBrushOperation*> m_pmActiveOperation;
        
        /// Intersection where to execute the changes
        NiPoint3 m_kIntersection;

        /// The brush size used
        float m_fBrushSize;

        /// Scale of the terrain
        float m_fTerrainScale;

        /// Following members are used to create the modification on the active
        /// point set

        /// Created every time the do function is called
        NiBrushPassPointInfo* m_pkPassPointInfo;

        /// Created every time the do function is called
        gcroot<MFalloff*> m_pmActiveFalloff;

        /// List of the origins that have been used in previous operation 
        /// during a drag
        NiPoint3* m_pkListOfOrigins;
        
        /// Number of origins held
        int m_iNumberOfOrigins;

        /// Index of the origin previously used
        int m_iCurrentIndex;

        NiBrushCommandInterface* m_pkPreviousCommand;
        NiBrushCommandInterface* m_pkNextCommand;

        bool m_bSmartPainting;

    public:

        
        /** 
            Default constructor.
         */
        NiBrushCommandInterface();

        /** 
            Detailed constructor. 
                Sets needed values to remember the environment.
            
            @param spTerrain The terrain component associated with the command
            @param spPointSet The current active point set
            @param pkBrush The brush type to use
            @param pkBrushOperation The operation to perform
            @param pkActiveFalloff The falloff to use
            @param pkPointInfo The point info to build
            @param kIntersection The position on the terrain where to perform 
                the operation
            @param fBrushSize The size of the brush to use
            @param pListOfIntersections the list of origins previously used
            @param numberOfIntersections the number of origins in the list
            @param currentIndex Index of the previously used origin
            @param bUsePoint debug parameter set on false by default
         */
        NiBrushCommandInterface(NiTerrainInteractor* pkTerrain, 
            NiDeformablePointSetPtr spPointSet,
            MBrushType* pmBrush,
            MBrushOperation* pmBrushOperation,
            MFalloff* pmActiveFalloff,
            NiBrushPassPointInfo* pkPointInfo,
            NiPoint3 kIntersection,
            float fBrushSize,
            float fTerrainScale,
            NiPoint3* pkListOfIntersections,
            int iNumberOfIntersections,
            int iCurrentIndex);

        /** 
            Destructor.
            
         */
        ~NiBrushCommandInterface();

        virtual void AddReference();

        virtual void RemoveReference();

        virtual NiFixedString GetName();

        virtual void DoCommand(NiEntityErrorInterface* pkErrors, 
            bool bUndoable);

        virtual void UndoCommand(NiEntityErrorInterface* pkErrors);


        /** 
            Sets the type of brush to use.            
            @param pkBrushElement The type of brush to use
         */
        void SetBrushElement(MBrushElement* pmBrushElement);

        /** 
            Sets the operation to perform.
            
            @param pkBrushOperation The operation to perform
         */
        void SetBrushOperation(MBrushOperation* pmBrushOperation);

        /** 
            Sets the point of intersection where the action should be 
            performed.
            
            @param kIntersection The point of intersection
         */
        void SetIntersection(NiPoint3 kIntersection);

        /** 
            Sets the brush size.
            
            @param fBrushSize The size of the brush
         */
        void SetBrushSize(float fBrushSize);

        /** 
            Sets the brush strength.
            
            @param fBrushStrength The strength of the brush 
         */
        void SetBrushStrength(float fBrushStrength);

        /** 
            Checks if the given point has already been changed in that drag 
                session.
            
            @param pPoints Point to check. Sets the HasChanged flag of the 
                point if it has been changed
            @return true if the point was changed false otherwise
         */
        bool HasChanged(NiDeformablePoint* pkPoints);

        /** 
            Sets the previous command that was executed
            
            @param pToSet value to set to
         */
        void SetPreviousCommand(NiBrushCommandInterface* pkToSet);

        /** 
            Sets the next command that was executed
            
            @param pToSet value to set to
         */
        void SetNextCommand(NiBrushCommandInterface* pkToSet);

        /** 
            Returns the previous command
            
            @return the previous command
         */
        NiBrushCommandInterface* GetPreviousCommand();

        /** 
            Returns the next command
            
            @return the next command
         */
        NiBrushCommandInterface* GetNextCommand();

        NiUInt32 GetSize();
    };

}
}
}
}



#endif
