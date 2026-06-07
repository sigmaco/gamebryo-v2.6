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

// NifRenderViewWireframeCommand.cpp

#include "stdafx.h"
#include "NifRenderViewWireframeCommand.h"
#include "NifRenderView.h"
#include "NifDoc.h"

//---------------------------------------------------------------------------
bool CNifRenderViewWireframeCommand::Execute(CNifRenderView* pkView)
{
    if(pkView)
    {
        m_pkView = pkView;
        CNifDoc* pkDoc = pkView->GetDocument();
        if(pkDoc)
        {
            NiNode* pkRoot = pkDoc->GetSceneGraph();

            NiWireframeProperty* pkWireframe = GetWireframeProperty(pkRoot);
            if(pkWireframe == NULL)
            {
                pkWireframe = NiNew NiWireframeProperty();
                pkRoot->AttachProperty(pkWireframe);
            }

            pkWireframe->SetWireframe(!pkWireframe->GetWireframe());
            pkRoot->UpdateProperties();

        }
        // No document == error
        else 
            return false;
    }
    // No view == error
    else 
        return false;

    return true;
}
//---------------------------------------------------------------------------
CNifRenderViewWireframeCommand::CNifRenderViewWireframeCommand()
{
    m_spCamera = NULL;
    m_spScene = NULL;
    m_pkView = NULL;
}
//---------------------------------------------------------------------------
CNifRenderViewWireframeCommand::~CNifRenderViewWireframeCommand()
{
    m_spCamera = NULL;
    m_spScene = NULL;
    m_pkView = NULL;
}
//---------------------------------------------------------------------------
NiWireframeProperty* 
CNifRenderViewWireframeCommand::GetWireframeProperty(NiNode* scene)
{
    // properties
    NiProperty* pkProp;
    NiPropertyList* pPropList = &scene->GetPropertyList();

    NiTListIterator kIter = pPropList->GetHeadPos();
    while(kIter)
    {
        pkProp = pPropList->GetNext(kIter);
       if(NiIsKindOf(NiWireframeProperty, pkProp))
            return (NiWireframeProperty*)pkProp;
    }
    return NULL;
}
//---------------------------------------------------------------------------
