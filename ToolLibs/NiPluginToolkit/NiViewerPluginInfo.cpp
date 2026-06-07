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

#include "NiViewerPluginInfo.h"

NiImplementRTTI(NiViewerPluginInfo, NiPluginInfo);

//---------------------------------------------------------------------------
NiViewerPluginInfo::NiViewerPluginInfo()
{
    SetType("VIEWER");
    SetRenderer(NiSystemDesc::RENDERER_DX9);
}
//---------------------------------------------------------------------------
NiViewerPluginInfo::NiViewerPluginInfo(NiSystemDesc::RendererID eRenderer)
{
    SetType("VIEWER");
    SetRenderer(eRenderer);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiViewerPluginInfo::Clone(NiPluginInfo* pkClone)
{
    if (!pkClone)
    {
        pkClone = NiNew NiViewerPluginInfo;
    }

    NIASSERT(NiIsKindOf(NiViewerPluginInfo, pkClone));

    NiPluginInfo::Clone(pkClone);

    return pkClone;
}
//---------------------------------------------------------------------------
bool NiViewerPluginInfo::Equals(NiPluginInfo* pkInfo)
{
    if (!NiIsKindOf(NiViewerPluginInfo, pkInfo))
    {
        return false;
    }

    if (!NiPluginInfo::Equals(pkInfo))
    {
        return false;
    }

    NiViewerPluginInfo* pkViewerInfo = (NiViewerPluginInfo*) pkInfo;

    if (pkViewerInfo->GetRenderer() != GetRenderer())
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiSystemDesc::RendererID NiViewerPluginInfo::GetRenderer() const
{ 
    return (NiSystemDesc::RendererID) GetInt(VIEWER_PLUGIN_TYPE);
}
//---------------------------------------------------------------------------
void NiViewerPluginInfo::SetRenderer(NiSystemDesc::RendererID eRenderer)
{ 
    SetInt(VIEWER_PLUGIN_TYPE, (int)eRenderer);
}
//---------------------------------------------------------------------------
