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

#include "MEntity.h"
#include "MRenderingContext.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI
{
    public __gc __interface IRenderingMode
    {
    public:
        __property String* get_Name();
        __property bool get_DisplayToUser();

        void Update(float fTime);
        void Begin(MRenderingContext* pmRenderingContext);
        void Render(MEntity* pmEntity, MRenderingContext* pmRenderingContext);
        void Render(MEntity* amEntities[],
            MRenderingContext* pmRenderingContext);
        void End(MRenderingContext* pmRenderingContext);
    };
}}}}
