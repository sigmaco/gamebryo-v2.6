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

#include "MFileLocation.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace System::ComponentModel::Design::Serialization;
using namespace System::Reflection;
using namespace System::Collections;

//---------------------------------------------------------------------------
bool MFileLocationConverter::CanConvertFrom(
    ITypeDescriptorContext* pmContext, Type* pmSourceType)
{
    if (pmSourceType == __typeof(String))
    {
        return true;
    }

    return __super::CanConvertFrom(pmContext, pmSourceType);
}
//---------------------------------------------------------------------------
bool MFileLocationConverter::CanConvertTo(ITypeDescriptorContext* pmContext, 
    Type* pmDestinationType)
{
    if (pmDestinationType == __typeof(InstanceDescriptor))
    {
        return true;
    }

    return __super::CanConvertTo(pmContext, pmDestinationType);
}
//---------------------------------------------------------------------------
Object* MFileLocationConverter::ConvertFrom(
    ITypeDescriptorContext* pmContext, CultureInfo* pmCulture,
    Object* pmValue)
{
    if (pmValue->GetType() == __typeof(String))
    {
        MFileLocation* pmLocation = new MFileLocation(
            static_cast<String*>(pmValue));

        return pmLocation;
    }
        
    return __super::ConvertFrom(pmContext, pmCulture, pmValue);
}
//---------------------------------------------------------------------------
Object* MFileLocationConverter::ConvertTo(ITypeDescriptorContext* pmContext, 
    CultureInfo* pmCulture, Object* pmValue, Type* pmDestinationType)
{
    if (pmDestinationType == __typeof(InstanceDescriptor) && 
        pmValue->GetType() == __typeof(MFileLocation))
    {
        Type* amTypes[] = new Type*[1];
        amTypes[0] = __typeof(String);
        ConstructorInfo* pmInfo = __typeof(MFileLocation)->GetConstructor(
            amTypes);
        ArrayList* pmArguments = new ArrayList(1);
        pmArguments->Add(static_cast<MFileLocation*>(pmValue)->Path);
        InstanceDescriptor* pmDescriptor = new InstanceDescriptor(pmInfo,
            pmArguments);

        return pmDescriptor;
    }

    return __super::ConvertTo(pmContext, pmCulture, pmValue,
        pmDestinationType);
}
//---------------------------------------------------------------------------
Object* MFileLocationNameEditor::EditValue(
    ITypeDescriptorContext* pmContext, System::IServiceProvider* pmProvider,
    Object* pmValue)
{
    Object* pmResult = __super::EditValue(pmContext, pmProvider, pmValue);
    if (pmResult != NULL && pmResult->GetType() == __typeof(String))
    {
        return new MFileLocation(static_cast<String*>(pmResult));
    }

    return pmResult;
}
//---------------------------------------------------------------------------
MFileLocation::MFileLocation(String* strPath) :
    m_strDefaultBrowsePath(String::Empty)
{
    Path = strPath;
}
//---------------------------------------------------------------------------
MFileLocation::MFileLocation(String* strPath,
    String* strDefaultBrowsePath)
{
    Path = strPath;
    DefaultBrowsePath = strDefaultBrowsePath;
}
//---------------------------------------------------------------------------
String* MFileLocation::get_Path()
{
    return ToString();
}
//---------------------------------------------------------------------------
void MFileLocation::set_Path(String* strPath)
{
    m_strPath = strPath;
}
//---------------------------------------------------------------------------
String* MFileLocation::get_DefaultBrowsePath()
{
    return m_strDefaultBrowsePath;
}
//---------------------------------------------------------------------------
void MFileLocation::set_DefaultBrowsePath(String* strDefaultBrowsePath)
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
String* MFileLocation::ToString()
{
    return m_strPath;
}
//---------------------------------------------------------------------------
