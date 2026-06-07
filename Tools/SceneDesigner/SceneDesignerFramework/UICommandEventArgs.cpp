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

#include "UICommandEventArgs.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;

//---------------------------------------------------------------------------
UICommandEventArgs::UICommandEventArgs()
{
    m_pmParameters = new Hashtable();
}
//---------------------------------------------------------------------------
String* UICommandEventArgs::GetParameters()[]
{
    int index = 0;
    String* pmRetVal[] = new String*[m_pmParameters->Count];
    IEnumerator* keyEnumerator = m_pmParameters->Keys->GetEnumerator(); 
    while (keyEnumerator->MoveNext())
    {
        String* strKeyName = dynamic_cast<String*>(keyEnumerator->Current);
        pmRetVal[index++] = strKeyName;
    }
    return pmRetVal;
}
//---------------------------------------------------------------------------
String* UICommandEventArgs::GetValue(String* strParameterName)
{
    return dynamic_cast<String*>(m_pmParameters->get_Item(strParameterName));
}
//---------------------------------------------------------------------------
void UICommandEventArgs::SetValue(String* strParameterName, String* strValue)
{
    m_pmParameters->set_Item(strParameterName, strValue);
}
//---------------------------------------------------------------------------
