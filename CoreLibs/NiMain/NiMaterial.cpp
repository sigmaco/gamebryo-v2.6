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

#include <NiMaterial.h>
#include <NiMaterialInstance.h>

NiTFixedStringMap<NiMaterial*>* NiMaterial::ms_pkMaterials = NULL;
char NiMaterial::ms_acDefaultWorkingDirectory[NI_MAX_PATH];

NiImplementRootRTTI(NiMaterial);

//---------------------------------------------------------------------------
NiMaterial::NiMaterial(const NiFixedString& kName) :
    m_kMaterialName(kName)
{
    NIASSERT(ms_pkMaterials);

    // Ensure name is nontrivial
    NIASSERT(kName.GetLength() != 0 &&
        "Material instances must have valid names!");

    // Ensure material is unique by name
    NIASSERT((GetMaterial(kName) == 0) && 
        "Derived NiMaterials must ensure in their Create function that their "
        "Material name is unique by calling GetMaterial!");

    ms_pkMaterials->SetAt(m_kMaterialName, this);
}
//---------------------------------------------------------------------------
NiMaterial::~NiMaterial()
{
    NIASSERT(GetMaterial(m_kMaterialName) == this);

    ms_pkMaterials->RemoveAt(m_kMaterialName);
}
//---------------------------------------------------------------------------
void NiMaterial::SetWorkingDirectory(const char*)
{
}
//---------------------------------------------------------------------------
NiMaterialIterator NiMaterial::GetFirstMaterialIter()
{
    return ms_pkMaterials->GetFirstPos();
}
//---------------------------------------------------------------------------
NiMaterial* NiMaterial::GetNextMaterial(NiMaterialIterator& kIter)
{
    if (kIter != NULL)
    {
         NiMaterial* pkMaterial = NULL;
         NiFixedString kName;
         ms_pkMaterials->GetNext(kIter, kName, pkMaterial);
         return pkMaterial;
    }
    return NULL;
}
//---------------------------------------------------------------------------
void NiMaterial::UnloadShadersForAllMaterials()
{
    NiTMapIterator kIter = ms_pkMaterials->GetFirstPos();

    NiMaterial* pkMaterial = NULL;
    NiFixedString kName;
    while (kIter)
    {
        ms_pkMaterials->GetNext(kIter, kName, pkMaterial);

        if (pkMaterial)
            pkMaterial->UnloadShaders();
    }
}
//---------------------------------------------------------------------------
void NiMaterial::SetWorkingDirectoryForAllMaterials(const char* pcWorkingDir)
{
    NiTMapIterator kIter = ms_pkMaterials->GetFirstPos();

    NiMaterial* pkMaterial = NULL;
    NiFixedString kName;
    while (kIter)
    {
        ms_pkMaterials->GetNext(kIter, kName, pkMaterial);

        if (pkMaterial)
            pkMaterial->SetWorkingDirectory(pcWorkingDir);
    }
}
//---------------------------------------------------------------------------
void NiMaterial::_SDMInit()
{
    ms_pkMaterials = NiNew NiTFixedStringMap<NiMaterial*>;
    SetDefaultWorkingDirectory(NULL);
}
//---------------------------------------------------------------------------
void NiMaterial::_SDMShutdown()
{
    NiDelete ms_pkMaterials;
    ms_pkMaterials = NULL;
}
//---------------------------------------------------------------------------
void NiMaterial::SetDefaultWorkingDirectory(const char* pcWorkingDir)
{
    if (pcWorkingDir == NULL)
    {
        ms_acDefaultWorkingDirectory[0] = '\0';
    }
    else
    {
        NiStrcpy(ms_acDefaultWorkingDirectory, NI_MAX_PATH, pcWorkingDir);
        NiPath::Standardize(ms_acDefaultWorkingDirectory);
        if (NiPath::IsRelative(ms_acDefaultWorkingDirectory))
        {
            NIVERIFY(NiPath::ConvertToAbsolute(
                ms_acDefaultWorkingDirectory, NI_MAX_PATH));
        }
    }
}
//---------------------------------------------------------------------------
const char* NiMaterial::GetDefaultWorkingDirectory()
{
    if (ms_acDefaultWorkingDirectory[0] == '\0')
        return NULL;
    else
        return ms_acDefaultWorkingDirectory;
}
//---------------------------------------------------------------------------
