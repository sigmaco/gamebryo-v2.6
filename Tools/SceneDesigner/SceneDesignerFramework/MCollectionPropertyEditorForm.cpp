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
#include "MCollectionPropertyEditorForm.h"
#include <windows.h>

using namespace Emergent::Gamebryo::SceneDesigner::Framework;


MCollectionPropertyEditorForm::MCollectionPropertyEditorForm()
{
    InitializeComponent();
}
//---------------------------------------------------------------------------
Object* MCollectionPropertyEditorForm::get_EditValue()[]
{
    return m_amEditValue;
}
//---------------------------------------------------------------------------
void MCollectionPropertyEditorForm::set_EditValue(Object* amValue[])
{
    m_amEditValue = amValue;
    m_pmctlNumberOfItems->Value = amValue->Count;
}
//---------------------------------------------------------------------------
void MCollectionPropertyEditorForm::OnOk_Click(System::Object*,
    System::EventArgs *)
{
    int iNewCount = (int) m_pmctlNumberOfItems->Value;
    int iOldCount = m_amEditValue->Count;

    Object* amNewList[] = new Object*[iNewCount];
    for (int iIndex = 0; iIndex < iNewCount; iIndex++)
    {
        if (iIndex < iOldCount)
        {
            amNewList[iIndex] = m_amEditValue[iIndex];
        }
    }
    m_amEditValue = amNewList;
}
//---------------------------------------------------------------------------
