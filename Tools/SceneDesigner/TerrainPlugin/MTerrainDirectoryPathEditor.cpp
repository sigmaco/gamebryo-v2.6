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
#include "TerrainPluginPCH.h"

#include "MTerrainDirectoryPathEditor.h"

using namespace System::Windows::Forms;
using namespace System::Windows::Forms::Design;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

//---------------------------------------------------------------------------
UITypeEditorEditStyle MTerrainDirectoryPathEditor::GetEditStyle(
    ITypeDescriptorContext* pmContext)
{
    if (pmContext->PropertyDescriptor != NULL &&
        pmContext->PropertyDescriptor->IsReadOnly)
    {
        return UITypeEditorEditStyle::None;
    }
    else
    {
        return UITypeEditorEditStyle::Modal;
    }
}
//---------------------------------------------------------------------------
Object* MTerrainDirectoryPathEditor::EditValue(
    ITypeDescriptorContext*, System::IServiceProvider*, 
    Object* pmValue)
{
    if (pmValue->GetType() == __typeof(String))
    {
        String* pmStringValue = static_cast<String*>(pmValue);

        FolderBrowserDialog* dlg = new FolderBrowserDialog();
        if (pmStringValue->Length == 0)
        {
            ISettingsService* pmSettingsService = MGetService(
                ISettingsService);

            pmStringValue = pmSettingsService->get_ScenePath();
        }
        dlg->SelectedPath = pmStringValue;
        dlg->Description = "Select location for terrain data archive.";

        if (dlg->ShowDialog() == DialogResult::OK)
        {
            // Since a directory is being chose, ensure the ending '\'
            dlg->SelectedPath = String::Concat(dlg->SelectedPath, "\\");
            return dlg->SelectedPath;
        }
    }

    return pmValue;
}
