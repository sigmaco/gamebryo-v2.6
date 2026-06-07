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

#ifndef MBRUSHOPERATIONFACTORY_H
#define MBRUSHOPERATIONFACTORY_H

//#include <NiMain.h>
#include <vcclr.h>
#include "MBrushOperation.h"

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{
    using namespace System;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
    using namespace Emergent::Gamebryo::SceneDesigner::Framework;

    public __gc class MBrushOperationFactory : public MDisposable
    {

    public :
        //===================================================================
        // Constructor
        //===================================================================

        MBrushOperationFactory();

        //===================================================================
        // MDisposable implementation
        //===================================================================

        virtual void Do_Dispose(bool bDisposing);

        //===================================================================
        // Data access functions
        //===================================================================

        void AddTemplate(MBrushOperation* pmToAdd);

        void RemoveTemplate(String* pmElementName, 
            String* pmOperationName);
        
        ICollection* GetTemplates();
        
        MBrushOperation* GetNewOperation(String* pmElementName, 
            String* pmOperationName);

        MBrushOperation* GetTemplate(String* pmElementName, 
            String* pmOperationName);

        void UpdateTemplateFrom(MBrushOperation* pmOperation);

        //===================================================================
        // Static functions
        //===================================================================

        static void _SDMInit(void);
        
        static void _SDMShutdown(void);
        
        static MBrushOperationFactory* GetInstance();

        //===================================================================
        // Helper functions
        //===================================================================
        void Update();

    private:
        //===================================================================
        // Member variables
        //===================================================================
        Hashtable* m_pmBaseOperations;

        static MBrushOperationFactory* ms_pmInstance;
    };

}
}
}
}


#endif