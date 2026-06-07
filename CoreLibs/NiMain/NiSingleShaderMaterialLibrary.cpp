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
#include "NiMainPCH.h"

#include "NiSingleShaderMaterialLibrary.h"
#include "NiShaderLibrary.h"
#include "NiSingleShaderMaterial.h"
#include "NiShaderFactory.h"
#include "NiShaderLibraryDesc.h"

NiSingleShaderMaterialLibrary* NiSingleShaderMaterialLibrary::ms_pkThis = NULL;
//---------------------------------------------------------------------------
NiSingleShaderMaterialLibrary::NiSingleShaderMaterialLibrary() : 
    NiMaterialLibrary("NiSingleShaderMaterialLibrary")
{
    m_pkLastLibrary = 0;
    m_uiLastDesc = 0;
}
//---------------------------------------------------------------------------
NiSingleShaderMaterialLibrary::~NiSingleShaderMaterialLibrary()
{
    m_pkLastLibrary = 0;
    m_uiLastDesc = 0;
}
//---------------------------------------------------------------------------
NiMaterial* NiSingleShaderMaterialLibrary::GetMaterial(
    const NiFixedString& kName)
{
    return NiSingleShaderMaterial::Create(kName);
}
//---------------------------------------------------------------------------
NiShaderDesc* NiSingleShaderMaterialLibrary::GetFirstMaterialDesc() 
{
    NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();
    m_pkLastLibrary = pkFactory->GetFirstLibrary();
    m_uiLastDesc = 0;
    if (m_pkLastLibrary)
    {
        NiShaderLibraryDesc* pkDesc = m_pkLastLibrary->GetShaderLibraryDesc();
        while (pkDesc->GetShaderCount() == 0)
        {
            m_pkLastLibrary = pkFactory->GetNextLibrary();
            m_uiLastDesc = 0;
            pkDesc = m_pkLastLibrary->GetShaderLibraryDesc();
        }
        
        if (pkDesc && pkDesc->GetShaderCount() != 0)
        {
            return pkDesc->GetShaderDesc(m_uiLastDesc);
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiShaderDesc* NiSingleShaderMaterialLibrary::GetNextMaterialDesc() 
{
    NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();
    NiShaderLibrary* pkLib = pkFactory->GetFirstLibrary();

    while (pkLib != m_pkLastLibrary)
    {
        pkLib = pkFactory->GetNextLibrary();
    }

    m_uiLastDesc++;

    if ( pkLib)
    {
        NiShaderLibraryDesc* pkDesc = pkLib->GetShaderLibraryDesc();

        while (!pkDesc || pkDesc->GetShaderCount() <= m_uiLastDesc)
        {
            m_pkLastLibrary = pkFactory->GetNextLibrary();
            m_uiLastDesc = 0;
            if (!m_pkLastLibrary)
                break;
            pkDesc = m_pkLastLibrary->GetShaderLibraryDesc();
        }

        if (m_pkLastLibrary && pkDesc && 
            pkDesc->GetShaderCount() > m_uiLastDesc)
        {
            return pkDesc->GetShaderDesc(m_uiLastDesc);
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
const char* NiSingleShaderMaterialLibrary::GetMaterialName(
    unsigned int uiMaterial)
{
    unsigned int uiCurrentIdx = 0; 

    NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();
    NiShaderLibrary* pkLib = pkFactory->GetFirstLibrary();

    while (pkLib)
    {
        NiShaderLibraryDesc* pkDesc = pkLib->GetShaderLibraryDesc();
        if (pkDesc && pkDesc->GetShaderCount() + uiCurrentIdx > uiMaterial)
        {
            NiShaderDesc* pkShaderDesc =
                pkDesc->GetShaderDesc(uiMaterial - uiCurrentIdx);
            return pkShaderDesc->GetName();
        }
        else if (pkDesc)
        {
            uiCurrentIdx += pkDesc->GetShaderCount();
        }
        pkLib = pkFactory->GetNextLibrary();
    }

    return NULL;
}
//---------------------------------------------------------------------------
unsigned int NiSingleShaderMaterialLibrary::GetMaterialCount() const
{
    unsigned int uiMaterialCount = 0;
    NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();
    if (pkFactory == NULL)
        return 0;

    NiShaderLibrary* pkLib = pkFactory->GetFirstLibrary();
    while (pkLib)
    {
        NiShaderLibraryDesc* pkDesc = pkLib->GetShaderLibraryDesc();
        if (pkDesc)
            uiMaterialCount += pkDesc->GetShaderCount();

        pkLib = pkFactory->GetNextLibrary();
    }

    return uiMaterialCount;
}
//---------------------------------------------------------------------------
void NiSingleShaderMaterialLibrary::_SDMInit()
{
    ms_pkThis = NiNew NiSingleShaderMaterialLibrary();
}
//---------------------------------------------------------------------------
void NiSingleShaderMaterialLibrary::_SDMShutdown()
{
    NiDelete ms_pkThis;
    ms_pkThis = NULL;
}
//---------------------------------------------------------------------------
