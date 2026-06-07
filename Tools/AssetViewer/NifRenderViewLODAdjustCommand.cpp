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

// NifRenderViewLODAdjustCommand.cpp

#include "stdafx.h"
#include "NifRenderViewLODAdjustCommand.h"
#include "NifRenderView.h"
#include "NifDoc.h"
#include "NifRenderViewUIManager.h"
#include "NifRenderViewUIState.h"
#include "NiSkinningLODController.h"
#include "NiTNodeTraversal.h"

bool CNifRenderViewLODAdjustCommand::m_bAllowAdjust = false;

//---------------------------------------------------------------------------
class AdjustSkinningLODFunctor
{
public:
    AdjustSkinningLODFunctor(unsigned int uiLOD) : m_uiLOD(uiLOD) {}
    bool operator () (NiAVObject* pkAVObject)
    {
        NiSkinningLODController* pkCont = 
            (NiSkinningLODController*)pkAVObject->GetController(
            &NiSkinningLODController::ms_RTTI);
        if (!pkCont)
            return false;

        if (m_uiLOD < pkCont->GetNumberOfBoneLODs())
            pkCont->SetBoneLOD(m_uiLOD);

        return true;
    }

protected:
    unsigned int m_uiLOD;
};
//---------------------------------------------------------------------------
bool CNifRenderViewLODAdjustCommand::Execute(CNifRenderView* pkView)
{
    if(pkView)
    {
        CNifDoc* pkDoc = pkView->GetDocument();
        if(pkDoc)
        {
            int iLOD = NiLODNode::GetGlobalLOD();
            if (iLOD < 0)
                m_iAdjust += -iLOD;
            iLOD += m_iAdjust;
            if (iLOD < 0)
                iLOD = 0;
            NiLODNode::SetGlobalLOD(iLOD);

            // Adjust all skinning LOD controllers
            if (iLOD >= 0)
            {
                AdjustSkinningLODFunctor kFunctor((unsigned int)iLOD);
                NiTNodeTraversal::DepthFirst_AllObjects(pkDoc->GetSceneGraph(),
                    kFunctor);
            }

            pkDoc->UpdateScene(true);
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
CNifRenderViewLODAdjustCommand::CNifRenderViewLODAdjustCommand(
    int iAdjust)
{
    m_iAdjust = iAdjust;
}
//---------------------------------------------------------------------------
CNifRenderViewLODAdjustCommand::~CNifRenderViewLODAdjustCommand()
{
}
//---------------------------------------------------------------------------
bool CNifRenderViewLODAdjustCommand::IsAdjustEnabled()
{
    return m_bAllowAdjust;
}
//---------------------------------------------------------------------------
void CNifRenderViewLODAdjustCommand::ToggleAdjust(bool bAdjustOn)
{
    m_bAllowAdjust = bAdjustOn;

    if (bAdjustOn)
    {
        NiLODNode::SetGlobalLOD(0);
    }
    else
    {
        NiLODNode::SetGlobalLOD(-1);
    }
}
//---------------------------------------------------------------------------
bool CNifRenderViewLODAdjustCommand::IsDecrementEnabled()
{
    if (!IsAdjustEnabled())
        return false;

    int iLOD = NiLODNode::GetGlobalLOD();
    if (iLOD < 1)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool CNifRenderViewLODAdjustCommand::IsIncrementEnabled()
{
    if (!IsAdjustEnabled())
        return false;
    return true;
}
//---------------------------------------------------------------------------
