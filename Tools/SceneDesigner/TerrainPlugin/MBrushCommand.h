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

#ifndef MBRUSHCOMMAND_H
#define MBRUSHCOMMAND_H

#include "NiBrushCommandInterface.h"

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{
    using namespace System;
    using namespace System::Windows::Forms;
    using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
    using namespace Emergent::Gamebryo::SceneDesigner::Framework;

    /** 
        AeBrushCommand allows the creation and execution of actions using the
            GameBryo command system.
            It wraps a AeBrushCommandInterface which remembers the state of the
            application before the action is done. It allows the use of 
            functions such as Undo and Redo.        
     */
    public __gc class MBrushCommand : public ICommand , public MDisposable
    {
    private:
        /// The associated command interface
        NiBrushCommandInterface* m_pkCommandInterface;

        /// The entity which this command affects
        MEntity* m_pmEntity;

        /// The previous command
        MBrushCommand* m_pmPreviousCommand;

        /// The Next command
        MBrushCommand* m_pmNextCommand;

    public:

        /** 
            Default Constructor.            
         */
        MBrushCommand(MEntity* pmEntity);

        virtual System::String __gc * get_Name();

        virtual NiEntityCommandInterface * GetNiEntityCommandInterface();

        virtual void DoCommand(bool bInBatch, bool bUndoable);

        virtual void UndoCommand(bool bInBatch);

        virtual void Do_Dispose(bool bDisposing);

        /** 
            Initializes the Command interface by calling the detailed 
            constructor and passing its parameters to the constructor.
            @param spTerrain The associated terrain component
            @param spPointSet The point set in use
            @param pkBrush the brush in use
            @param pkBrushOperation the operation to use
            @param pkActiveFalloff The falloff to use
            @param pkPointInfo The point info to build
            @param kIntersection the intersection where to apply the action
            @param fBrushSize the outer radius of the brush
            @param fTerrainScale scale of the currently selected terrain
         */
        void InitializeCommandInterface(NiTerrainInteractor* pkTerrain, 
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

        void SetNextCommand(MBrushCommand* pmCommand);

        void SetPreviousCommand(MBrushCommand* pmCommand);

        MBrushCommand* GetNextCommand();

        MBrushCommand* GetPreviousCommand();

        NiUInt32 GetSize();

        void ReinitialiseCommand();
    
    };
}
}
}
}

#endif
