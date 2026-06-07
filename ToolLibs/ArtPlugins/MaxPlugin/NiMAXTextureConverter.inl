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
//  NiMAXTextureConverter inline functions
//---------------------------------------------------------------------------
inline void NiMAXTextureConverter::SetSaveTexturesSeparately(
    bool bSaveTexuresSeparately)
{ 
    ms_bSaveTexturesSeparately = bSaveTexuresSeparately; 
}
//---------------------------------------------------------------------------
inline bool NiMAXTextureConverter::GetSaveTexturesSeparately() 
{ 
    return ms_bSaveTexturesSeparately; 
}
//---------------------------------------------------------------------------
inline void NiMAXTextureConverter::SetUseOrigExt(bool bUseOrigExt)
{ 
    ms_bUseOrigExt = bUseOrigExt; 
}
//---------------------------------------------------------------------------
inline void NiMAXTextureConverter::SetUseOrigExtVRAM(bool bUseOrigExtVRAM)
{ 
    ms_bUseOrigExtVRAM = bUseOrigExtVRAM; 
}
//---------------------------------------------------------------------------
inline bool NiMAXTextureConverter::GetUseOrigExt() 
{ 
    return ms_bUseOrigExt; 
}
//---------------------------------------------------------------------------
inline bool NiMAXTextureConverter::GetUseOrigExtVRAM() 
{ 
    return ms_bUseOrigExtVRAM; 
}
//---------------------------------------------------------------------------
inline void NiMAXTextureConverter::SetPlatform(
    NiDevImageConverter::Platform ePlatform)
{
    ms_ePlatform = ePlatform;
}
//---------------------------------------------------------------------------
inline NiDevImageConverter::Platform NiMAXTextureConverter::GetPlatform()
{
    return ms_ePlatform;
}
//---------------------------------------------------------------------------
inline void NiMAXTextureConverter::SetPixelLayout(
    NiTexture::FormatPrefs::PixelLayout ePixelLayout)
{
    ms_ePixelLayout = ePixelLayout;
}
//---------------------------------------------------------------------------
inline NiTexture::FormatPrefs::PixelLayout 
    NiMAXTextureConverter::GetPixelLayout()
{
    return ms_ePixelLayout;
}
//---------------------------------------------------------------------------
inline bool NiMAXTextureConverter::GetDevImageQuantizerFound()
{
    return ms_bDevImageQuantizerFound;
}
//---------------------------------------------------------------------------
