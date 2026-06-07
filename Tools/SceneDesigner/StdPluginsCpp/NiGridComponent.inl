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
inline NiGridComponent::NiGridComponent() :  m_bDisplayGrid(true), 
    m_bValidGrid(false), m_bStaticGrid(true), m_fMinDensity(5.0f), 
    m_fMaxDensity(60.0f), m_fGridSpacing(10.0f), m_iMajorLines(10), 
    m_iGridExtent(20), m_kGridColor(0.3f, 0.3f, 0.3f), 
    m_kMajorColor(0.0f, 0.0f, 0.0f)
{
}
//---------------------------------------------------------------------------
inline NiGridComponent::NiGridComponent(float fGridSpacing,
    int iMajorLineSpacing, int iGridExtent, const NiColor& kGridColor,
    const NiColor& kMajorColor, bool bStatic) : m_bDisplayGrid(true), 
    m_bValidGrid(false), m_bStaticGrid(bStatic), m_fMinDensity(5.0f), 
    m_fMaxDensity(60.0f), m_fGridSpacing(fGridSpacing), 
    m_iMajorLines(iMajorLineSpacing), m_iGridExtent(iGridExtent), 
    m_kGridColor(kGridColor), m_kMajorColor(kMajorColor)
{
}
//---------------------------------------------------------------------------
inline NiGridComponent::~NiGridComponent()
{
}
//---------------------------------------------------------------------------
inline void NiGridComponent::SetDisplayGrid(bool bDisplayGrid)
{
    m_bDisplayGrid = bDisplayGrid;
}
//---------------------------------------------------------------------------
inline void NiGridComponent::SetGridSpacing(float fSpacing)
{
    if (fSpacing != m_fGridSpacing)
    {
        //grid spacing cannot be 0 (or negative)
        m_fGridSpacing = fSpacing > 0 ? fSpacing : 1;
        m_bValidGrid = false;
    }
}
//---------------------------------------------------------------------------
inline void NiGridComponent::SetMajorLineSpacing(int iSpacing)
{
    if (iSpacing != m_iMajorLines)
    {
        //line spacing cannot be 0
        m_iMajorLines = iSpacing > 0 ? iSpacing : 1;
        m_bValidGrid = false;
    }
}
//---------------------------------------------------------------------------
inline void NiGridComponent::SetGridExtent(int iExtent)
{
    if (iExtent != m_iGridExtent)
    {
        m_iGridExtent = iExtent;
        m_bValidGrid = false;
    }
}
//---------------------------------------------------------------------------
inline void NiGridComponent::SetGridColor(const NiColor& kColor)
{
    if (kColor != m_kGridColor)
    {
        m_kGridColor = kColor;
        m_bValidGrid = false;
    }
}
//---------------------------------------------------------------------------
inline void NiGridComponent::SetMajorColor(const NiColor& kColor)
{
    if (kColor != m_kMajorColor)
    {
        m_kMajorColor = kColor;
        m_bValidGrid = false;
    }
}
//---------------------------------------------------------------------------
inline void NiGridComponent::SetStatic(bool bStatic)
{
    if (bStatic != m_bStaticGrid)
    {
        m_bStaticGrid = bStatic;
        m_bValidGrid = false;
    }
}
//---------------------------------------------------------------------------
