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

using namespace System::Drawing::Design;
using namespace System::ComponentModel;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MDepthBiasEditor : public UITypeEditor
    {
    public:
        virtual UITypeEditorEditStyle GetEditStyle(
            ITypeDescriptorContext* pmContext);
        virtual Object* EditValue(ITypeDescriptorContext* pmContext,
            System::IServiceProvider* pmProvider, Object* pmValue);

    private:
        NiShadowGeneratorComponent* GetShadowGeneratorComponent(
            PropertyDescriptor* pmPropertyDescriptor);
    };
}}}}
