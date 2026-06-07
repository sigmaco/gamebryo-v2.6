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

#include "TerrainPluginPCH.h"

#include "MImageFileNameEditor.h"

using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;
using namespace System::Windows::Forms;
using namespace System::Windows::Forms::Design;

//---------------------------------------------------------------------------
UITypeEditorEditStyle MImageFileNameEditor::GetEditStyle(
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
Object* MImageFileNameEditor::EditValue(ITypeDescriptorContext*,
    System::IServiceProvider*, Object* pmValue)
{
    if (pmValue->GetType() == __typeof(String))
    {
        String* strValue = static_cast<String*>(pmValue);

        OpenFileDialog* pmDialog = new OpenFileDialog();
        pmDialog->FileName = strValue;
        pmDialog->Title = GetDialogTitle();
        pmDialog->Filter = GetDialogFilter();
        if (pmDialog->ShowDialog() == DialogResult::OK)
        {
            return pmDialog->FileName;
        }
    }

    return pmValue;
}
//---------------------------------------------------------------------------
String* MImageFileNameEditor::GetDialogTitle()
{
    return "Choose File";
}
//---------------------------------------------------------------------------
String* MImageFileNameEditor::GetDialogFilter()
{
    return "Supported Image files (*.dds;*.tga;*.bmp;*.nif;*.sgi)|"
        "*.dds;*.tga;*.bmp;*.nif;*.sgi|"
        "DirectX DDS files(*.dds)|*.dds|"
        "Targa files (*.tga)|*.tga|"
        "Bitmap files (*.bmp)|*.bmp|"
        "Gamebryo NIF files (*.nif)|*.nif|"        
        "Silicon graphics (*.sgi)|*.sgi|"
        "All files (*.*)|*.*";
}
//---------------------------------------------------------------------------
