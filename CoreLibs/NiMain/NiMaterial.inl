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

//---------------------------------------------------------------------------
inline const NiFixedString& NiMaterial::GetName() const
{
    return m_kMaterialName;
}
//---------------------------------------------------------------------------
inline NiMaterial* NiMaterial::GetMaterial(const NiFixedString& kName)
{
    NIASSERT(ms_pkMaterials);
    if (kName.GetLength() == 0)
        return NULL;

    NiMaterial* pkMaterial = 0;
    ms_pkMaterials->GetAt(kName, pkMaterial);
    return pkMaterial;
}
//---------------------------------------------------------------------------
inline unsigned int NiMaterial::GetMaterialCount()
{
    NIASSERT(ms_pkMaterials);
    return ms_pkMaterials->GetCount();
}
//---------------------------------------------------------------------------
