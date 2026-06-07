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
#include "MBrushOperationFactory.h"

using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

//---------------------------------------------------------------------------
MBrushOperationFactory::MBrushOperationFactory()
{
    m_pmBaseOperations = new Hashtable();
}
//---------------------------------------------------------------------------
void MBrushOperationFactory::Do_Dispose(bool)
{
    if(m_pmBaseOperations)
    {
        IDictionaryEnumerator* pkCol = m_pmBaseOperations->GetEnumerator();

        while(pkCol->MoveNext())
        {
            dynamic_cast<MBrushOperation*>(pkCol->get_Value())->Dispose();
        }

        m_pmBaseOperations->Clear();
        m_pmBaseOperations = 0;
    }
}
//---------------------------------------------------------------------------
void MBrushOperationFactory::AddTemplate(MBrushOperation* pmToAdd)
{
    MVerifyValidInstance;

    String* pmKey = String::Concat(pmToAdd->m_pmElement->m_pmName,".",
        pmToAdd->m_pmName);

    m_pmBaseOperations->Add(pmKey, pmToAdd);
}
//---------------------------------------------------------------------------
void MBrushOperationFactory::RemoveTemplate(String* pmElementName, 
    String* pmOperationName)
{
    MVerifyValidInstance;

    String* pmKey = String::Concat(pmElementName,".",
        pmOperationName);

    m_pmBaseOperations->Remove(pmKey);
}
//---------------------------------------------------------------------------
ICollection* MBrushOperationFactory::GetTemplates()
{
    MVerifyValidInstance;

    return m_pmBaseOperations->Values;
}
//---------------------------------------------------------------------------
MBrushOperation* MBrushOperationFactory::GetNewOperation(String* 
    pmElementName, String* pmOperationName)
{
    MVerifyValidInstance;

    MBrushOperation* pmToReturn = 0;
    String* pmName = pmElementName->Concat(pmElementName, ".", 
        pmOperationName);

    if(m_pmBaseOperations->ContainsKey(pmName))
    {
        pmToReturn = dynamic_cast<MBrushOperation*>
            (m_pmBaseOperations->get_Item(pmName))->CloneOperation();
    }

    return pmToReturn;
}
//---------------------------------------------------------------------------
MBrushOperation* MBrushOperationFactory::GetTemplate(String* pmElementName, 
    String* pmOperationName)
{
    MVerifyValidInstance;

    MBrushOperation* pmToReturn = 0;
    String* pmKey = pmElementName->Concat(pmElementName, ".", pmOperationName);

    if(m_pmBaseOperations->ContainsKey(pmKey))
    {
        pmToReturn = dynamic_cast<MBrushOperation*>
            (m_pmBaseOperations->get_Item(pmKey));
    }

    return pmToReturn;
}
//---------------------------------------------------------------------------
void MBrushOperationFactory::UpdateTemplateFrom(MBrushOperation* pmOperation)
{
    MVerifyValidInstance;

    NIASSERT(pmOperation && pmOperation->m_pmElement && "Valid operation must "
        "given as a source.");
    if (!(pmOperation && pmOperation->m_pmElement))
        return;

    MBrushOperation* pmTemplate = GetTemplate(
        pmOperation->m_pmElement->m_pmName, pmOperation->m_pmName);

    if (!pmTemplate)
        return;

    pmTemplate->ReadValues(pmOperation);
    pmTemplate->m_pmElement->ReadValues(pmOperation->m_pmElement);
}
//---------------------------------------------------------------------------
void MBrushOperationFactory::_SDMInit(void)
{
    ms_pmInstance = new MBrushOperationFactory();
}
//---------------------------------------------------------------------------
void MBrushOperationFactory::_SDMShutdown(void)
{
    ms_pmInstance->Dispose();
    ms_pmInstance = 0;
}
//---------------------------------------------------------------------------
MBrushOperationFactory* MBrushOperationFactory::GetInstance()
{
    return ms_pmInstance;
}
//---------------------------------------------------------------------------
void MBrushOperationFactory::Update()
{
    MVerifyValidInstance;
}
