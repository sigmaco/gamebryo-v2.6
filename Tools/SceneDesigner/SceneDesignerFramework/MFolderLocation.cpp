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

#include "MFolderLocation.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace System::ComponentModel::Design::Serialization;
using namespace System::Reflection;
using namespace System::Collections;
using namespace System::Windows::Forms;

//---------------------------------------------------------------------------
bool MFolderLocationConverter::CanConvertFrom(
    ITypeDescriptorContext* pmContext, Type* pmSourceType)
{
    if (pmSourceType == __typeof(String))
    {
        return true;
    }

    return __super::CanConvertFrom(pmContext, pmSourceType);
}
//---------------------------------------------------------------------------
bool MFolderLocationConverter::CanConvertTo(ITypeDescriptorContext* pmContext, 
    Type* pmDestinationType)
{
    if (pmDestinationType == __typeof(InstanceDescriptor))
    {
        return true;
    }

    return __super::CanConvertTo(pmContext, pmDestinationType);
}
//---------------------------------------------------------------------------
Object* MFolderLocationConverter::ConvertFrom(
    ITypeDescriptorContext* pmContext, CultureInfo* pmCulture,
    Object* pmValue)
{
    if (pmValue->GetType() == __typeof(String))
    {
        MFolderLocation* pmLocation = new MFolderLocation(
            static_cast<String*>(pmValue));

        return pmLocation;
    }
        
    return __super::ConvertFrom(pmContext, pmCulture, pmValue);
}
//---------------------------------------------------------------------------
Object* MFolderLocationConverter::ConvertTo(ITypeDescriptorContext* pmContext, 
    CultureInfo* pmCulture, Object* pmValue, Type* pmDestinationType)
{
    if (pmDestinationType == __typeof(InstanceDescriptor) && 
        pmValue->GetType() == __typeof(MFolderLocation))
    {
        Type* amTypes[] = new Type*[1];
        amTypes[0] = __typeof(String);
        ConstructorInfo* pmInfo = __typeof(MFolderLocation)->GetConstructor(
            amTypes);
        ArrayList* pmArguments = new ArrayList(1);
        pmArguments->Add(static_cast<MFolderLocation*>(pmValue)->Path);
        InstanceDescriptor* pmDescriptor = new InstanceDescriptor(pmInfo,
            pmArguments);

        return pmDescriptor;
    }

    return __super::ConvertTo(pmContext, pmCulture, pmValue,
        pmDestinationType);
}
//---------------------------------------------------------------------------
Object* MFolderLocationNameEditor::EditValue(
    ITypeDescriptorContext*, System::IServiceProvider*,
    Object* pmValue)
{
    FolderBrowserDialog* pmDlg = new FolderBrowserDialog();
    pmDlg->SelectedPath = static_cast<MFolderLocation*>(pmValue)->Path;
    if (pmDlg->SelectedPath->Equals(String::Empty))
    {
        pmDlg->SelectedPath = static_cast<MFolderLocation*>(pmValue)
            ->DefaultBrowsePath;
    }
    
    if (pmDlg->ShowDialog() != DialogResult::OK)
    {
        return pmValue;
    }

    return new MFolderLocation(pmDlg->SelectedPath);
}
//---------------------------------------------------------------------------
MFolderLocation::MFolderLocation(String* strPath) :
    m_strDefaultBrowsePath(String::Empty)
{
    Path = strPath;
}
//---------------------------------------------------------------------------
MFolderLocation::MFolderLocation(String* strPath,
    String* strDefaultBrowsePath)
{
    Path = strPath;
    DefaultBrowsePath = strDefaultBrowsePath;
}
//---------------------------------------------------------------------------
String* MFolderLocation::get_Path()
{
    return ToString();
}
//---------------------------------------------------------------------------
void MFolderLocation::set_Path(String* strPath)
{
    MAssert(strPath != NULL, "Null path provided to function!");

    if (strPath->EndsWith(ms_strSeparatorString) ||
        strPath->EndsWith(ms_strAltSeparatorString))
    {
        m_strPath = strPath;
    }
    else if (!strPath->Equals(String::Empty))
    {
        m_strPath = String::Format("{0}{1}", strPath,
            ms_strSeparatorString);
    }
    else
    {
        m_strPath = strPath;
    }
}
//---------------------------------------------------------------------------
String* MFolderLocation::get_DefaultBrowsePath()
{
    return m_strDefaultBrowsePath;
}
//---------------------------------------------------------------------------
void MFolderLocation::set_DefaultBrowsePath(String* strDefaultBrowsePath)
{
    MAssert(strDefaultBrowsePath != NULL, "Null path provided to function!");

    if (strDefaultBrowsePath->EndsWith(ms_strSeparatorString) ||
        strDefaultBrowsePath->EndsWith(ms_strAltSeparatorString))
    {
        m_strDefaultBrowsePath = strDefaultBrowsePath;
    }
    else if (!strDefaultBrowsePath->Equals(String::Empty))
    {
        m_strDefaultBrowsePath = String::Format("{0}{1}",
            strDefaultBrowsePath, ms_strSeparatorString);
    }
    else
    {
        m_strDefaultBrowsePath = strDefaultBrowsePath;
    }
}
//---------------------------------------------------------------------------
String* MFolderLocation::ToString()
{
    return m_strPath;
}
//---------------------------------------------------------------------------
