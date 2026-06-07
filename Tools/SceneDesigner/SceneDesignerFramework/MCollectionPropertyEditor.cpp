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
#include "SceneDesignerFrameworkPCH.h"

#include "MCollectionPropertyEditor.h"
#include "MCollectionPropertyEditorForm.h"
#include "MEntityPropertyDescriptor.h"
#include "ServiceProvider.h"
#include "MComponent.h"

using namespace System::Collections;
using namespace System::ComponentModel;
using namespace System::ComponentModel::Design;
using namespace System::Diagnostics;
using namespace System::Reflection;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;


//---------------------------------------------------------------------------
MCollectionPropertyEditor::MCollectionPropertyEditor(Type* pmCollectionType,
    Type* pmItemType) : CollectionEditor(pmCollectionType)
{
    m_pmItemType = pmItemType;
}
//---------------------------------------------------------------------------
Type* MCollectionPropertyEditor::CreateCollectionItemType()
{
    return m_pmItemType;
}
//---------------------------------------------------------------------------
Object* MCollectionPropertyEditor::CreateInstance(System::Type* type)
{
    return __super::CreateInstance(type);
}
//---------------------------------------------------------------------------
Object* MCollectionPropertyEditor::EditValue(ITypeDescriptorContext* pmContext,
    System::IServiceProvider*, Object* pmValue)
{
    if (pmContext != NULL && pmContext->PropertyDescriptor != NULL &&
        __typeof(MEntityPropertyDescriptor)->IsAssignableFrom(
        pmContext->PropertyDescriptor->GetType()))
    {
        // Get propety descriptor.
        MEntityPropertyDescriptor* pmDescriptor =
            static_cast<MEntityPropertyDescriptor*> 
            (pmContext->PropertyDescriptor);
        Debug::Assert(pmDescriptor->PropertyContainer->HasProperty(
            pmDescriptor->PropertyName), "Entity does not contain " 
            "property!");

        // Build array of old data values.
        const unsigned int uiOldCount = 
            pmDescriptor->PropertyContainer->GetElementCount(
            pmDescriptor->PropertyName);
        Object* amOldData[] = new Object*[uiOldCount];
        for (unsigned int ui = 0; ui < uiOldCount; ui++)
        {
            amOldData[ui] = pmDescriptor->PropertyContainer
                ->GetPropertyData(pmDescriptor->PropertyName, ui);
        }

        // Create dialog.
        MCollectionPropertyEditorForm* pmDialog = 
            new MCollectionPropertyEditorForm();
        pmDialog->EditValue = amOldData;

        // Show dialog.
        if (pmDialog->ShowDialog() == DialogResult::OK)
        {
            // Get array of new data values.
            Object* amNewData[] = GetItems(pmDialog->EditValue);
            const unsigned int uiNewCount = amNewData->Count;

            // Get component containing property.
            MEntity* pmEntity = dynamic_cast<MEntity*>(
                pmDescriptor->PropertyContainer);
            MComponent* pmComponent = NULL;
            if (pmEntity == NULL)
            {
                pmComponent = dynamic_cast<MComponent*>(
                    pmDescriptor->PropertyContainer);
            }
            if (pmEntity != NULL)
            {
                pmComponent = GetComponentContainingProperty(
                    pmEntity, pmDescriptor->Name);
            }

            // Begin undo frame.
            ICommandService* pmCommandService = MGetService(ICommandService);
            pmCommandService->BeginUndoFrame(String::Format(
                "Modify number of elements in the \"{0}\" property collection "
                "for the \"{1}\" {2}.", pmDescriptor->PropertyName,
                pmDescriptor->PropertyContainer->Name, new String(
                (pmEntity != NULL) ? "entity" : "component")));

            // Clear out old property values.
            for (unsigned int ui = 0; ui < uiOldCount; ui++)
            {
                pmDescriptor->PropertyContainer->SetPropertyData(
                    pmDescriptor->PropertyName, NULL, ui, true);
            }

            // Resize property collection.
            pmDescriptor->PropertyContainer->SetElementCount(
                pmDescriptor->PropertyName, uiNewCount);

            // Add new data values to the property.
            for (unsigned int ui = 0; ui < uiNewCount; ui++)
            {
                pmDescriptor->PropertyContainer->SetPropertyData(
                    pmDescriptor->PropertyName, amNewData[ui], ui, true);
            }

            // End undo frame.
            pmCommandService->EndUndoFrame(true);
        }
    }

    return pmValue;
}
//---------------------------------------------------------------------------
MComponent* MCollectionPropertyEditor::GetComponentContainingProperty(
    MEntity* pmEntity, String* strPropertyName)
{
    MComponent* amComponents[] = pmEntity->GetComponents();
    unsigned int uiSize = amComponents->Count;
    for ( unsigned int uiIndex = 0; uiIndex < uiSize; uiIndex++)
    {
        MComponent* pmComponent = amComponents[uiIndex];
        if (pmComponent->HasProperty(strPropertyName))
        {
            return pmComponent;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
