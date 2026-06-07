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

#pragma once

#include "MDisposable.h"
#include "MEntity.h"

using namespace System::Collections::Generic;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MEntityCloneHelper : public MDisposable
    {
    public:
        MEntityCloneHelper();

        void Clone(MEntity* pmEntity, String* strNewName, 
            bool bInheritProperties);

        void Fixup(bool bClearExternal);

        MEntity* Lookup(MEntity* pmOriginalEntity);

        MEntity* GetEntities()[];

        void Reset();

    protected:
        virtual void Do_Dispose(bool bDisposing);

    private:
        NiEntityCloneHelper* m_pkCloneHelper;
    };
}}}}
