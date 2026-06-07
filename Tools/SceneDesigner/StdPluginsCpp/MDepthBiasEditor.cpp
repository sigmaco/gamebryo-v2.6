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

// Precompiled Header
#include "StdPluginsCppPCH.h"

#include "MDepthBiasEditor.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace System::Windows::Forms;

//---------------------------------------------------------------------------
UITypeEditorEditStyle MDepthBiasEditor::GetEditStyle(
    ITypeDescriptorContext* pmContext)
{
    if (pmContext != NULL && pmContext->PropertyDescriptor != NULL &&
        !pmContext->PropertyDescriptor->IsReadOnly)
    {
        NiShadowGeneratorComponent* pkComponent =
            GetShadowGeneratorComponent(pmContext->PropertyDescriptor);
        if (pkComponent && pkComponent->GetShadowGenerator())
        {
            return UITypeEditorEditStyle::Modal;
        }
    }

    return UITypeEditorEditStyle::None;
}
//---------------------------------------------------------------------------
Object* MDepthBiasEditor::EditValue(ITypeDescriptorContext* pmContext,
    System::IServiceProvider*, Object* pmValue)
{
    if (pmContext != NULL && pmValue->GetType() == __typeof(float))
    {
        if (MessageBox::Show("Set the depth bias to an appropriate default "
            "value?", "Use Default Depth Bias?", MessageBoxButtons::YesNo,
            MessageBoxIcon::Question) == DialogResult::Yes)
        {
            NiShadowGeneratorComponent* pkComponent =
                GetShadowGeneratorComponent(pmContext->PropertyDescriptor);
            if (pkComponent && pkComponent->GetShadowGenerator())
            {
                pkComponent->GetShadowGenerator()->SetDepthBiasToDefault();
                return __box(pkComponent->GetShadowGenerator()
                    ->GetDepthBias());
            }
        }
    }

    return pmValue;
}
//---------------------------------------------------------------------------
NiShadowGeneratorComponent* MDepthBiasEditor::GetShadowGeneratorComponent(
    PropertyDescriptor* pmPropertyDescriptor)
{
    MEntityPropertyDescriptor* pmEntityDescriptor = dynamic_cast<
        MEntityPropertyDescriptor*>(pmPropertyDescriptor);
    if (pmEntityDescriptor != NULL)
    {
        MPropertyContainer* pmPropertyContainer =
            pmEntityDescriptor->PropertyContainer;
        MAssert(pmPropertyContainer != NULL, "No property container for "
            "descriptor!");

        MComponent* pmComponent = NULL;
        if (pmPropertyContainer->GetType() == __typeof(MEntity))
        {
            MEntity* pmEntity = static_cast<MEntity*>(
                pmPropertyContainer);
            MComponent* pmComponents[] = pmEntity->GetComponents();
            NiFixedString kShadowGeneratorComponentClassName(
                "NiShadowGeneratorComponent");
            for (int i = 0; i < pmComponents->Length; i++)
            {
                if (pmComponents[i]->GetNiEntityComponentInterface()
                    ->GetClassName() == kShadowGeneratorComponentClassName)
                {
                    pmComponent = pmComponents[i];
                    break;
                }
            }
        }
        else if (pmPropertyContainer->GetType() == __typeof(MComponent))
        {
            pmComponent = static_cast<MComponent*>(pmPropertyContainer);
        }

        if (pmComponent != NULL)
        {
            return (NiShadowGeneratorComponent*)
                pmComponent->GetNiEntityComponentInterface();
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
