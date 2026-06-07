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
#include "StdPluginsCppPCH.h"

#include "NiGridComponent.h"
#include "NiEntityErrorInterface.h"

NiFixedString NiGridComponent::ms_kMajorLinesString;
NiFixedString NiGridComponent::ms_kGridExtentString;
NiFixedString NiGridComponent::ms_kGridColorString;
NiFixedString NiGridComponent::ms_kMajorColorString;
NiFixedString NiGridComponent::ms_kStaticString;
NiFixedString NiGridComponent::ms_kSceneRootString;

NiFixedString NiGridComponent::ms_kClassName;
NiFixedString NiGridComponent::ms_kComponentName;

//---------------------------------------------------------------------------
void NiGridComponent::RebuildInvalidGrid()
{
    if ((!m_bValidGrid) || (!m_spGrid) || (!m_spMajorLines))
    {
        // if the grid geometry is not the correct type, recreate it
        if (m_bStaticGrid)
        {
            ConstructStaticGrid(m_fGridSpacing);
        }
        else
        {
            ConstructDynamicGrid(m_fGridSpacing);
        }
    }
}
//---------------------------------------------------------------------------
void NiGridComponent::_SDMInit()
{
    ms_kMajorLinesString = "MajorLines";
    ms_kGridExtentString = "GridExtent";
    ms_kGridColorString = "GridColor";
    ms_kMajorColorString = "MajorColor";
    ms_kStaticString = "Static";
    ms_kSceneRootString = "Scene Root Pointer";

    ms_kClassName = "NiGridComponent";
    ms_kComponentName = "Grid";
}
//---------------------------------------------------------------------------
void NiGridComponent::_SDMShutdown()
{
    ms_kMajorLinesString = NULL;
    ms_kGridExtentString = NULL;
    ms_kGridColorString = NULL;
    ms_kMajorColorString = NULL;
    ms_kStaticString = NULL;
    ms_kSceneRootString = NULL;
        
    ms_kClassName = NULL;
    ms_kComponentName = NULL;
}
//---------------------------------------------------------------------------
float NiGridComponent::GetTrueSpacing(NiCamera* pkCamera, float fSpacing)
{
    // this function figures what the actual spacing between lines should be
    // this is different form the Grid Spacing setting because we want to set
    // the true spacing on how close the camera is to the grid
    NiFrustum kFrustum;

    kFrustum = pkCamera->GetViewFrustum();
    if (kFrustum.m_bOrtho)
    {
        // first, figure out how many lines fit within the frustum
        // then start reducing or increasing that amount by multiples of
        // m_iMajorLines until we have a number between the minimum and 
        // maximum line density
        float fCurrentLineCount;
        float fCurrentSpacing;

        fCurrentLineCount = NiAbs(kFrustum.m_fRight - kFrustum.m_fLeft) / 
            fSpacing;
        fCurrentSpacing = fSpacing;

        while (fCurrentLineCount < m_fMinDensity)
        {
            if (m_iMajorLines > 1)
            {
                fCurrentSpacing /= m_iMajorLines;
            }
            else
            {
                fCurrentSpacing -= 1;
            }
            fCurrentLineCount = NiAbs(kFrustum.m_fRight - kFrustum.m_fLeft) / 
                fCurrentSpacing;
        }

        while (fCurrentLineCount > m_fMaxDensity)
        {
            if (m_iMajorLines > 1)
            {
                fCurrentSpacing *= m_iMajorLines;
            }
            else
            {
                fCurrentSpacing += 1;
            }
            fCurrentLineCount = NiAbs(kFrustum.m_fRight - kFrustum.m_fLeft) / 
                fCurrentSpacing;
        }

        return fCurrentSpacing;
    }
    else
    {
        return fSpacing;
    }
}
//---------------------------------------------------------------------------
void NiGridComponent::ConstructStaticGrid(float fSpacing)
{
    // return if already created.
    if (m_bValidGrid && (m_spGrid || m_spMajorLines))
        return;

    // Determine if render has been created yet.
    MRenderingContext* pmRenderingContext;
    pmRenderingContext = MFramework::Instance->Renderer->RenderingContext;

    if (!pmRenderingContext)
        return;

    NiEntityRenderingContext* pkRenderingContext = 
        pmRenderingContext->GetRenderingContext();

    if (!pkRenderingContext)
        return;

    if (!pkRenderingContext->m_pkRenderer)
        return;

    int iMinorVertCount;
    int iMajorVertCount;
    int iMajorLineCount;

    // figure out how many total Major lines we'll need
    if (m_iMajorLines > 0)
    {
        iMajorLineCount = (m_iGridExtent / m_iMajorLines);
    }
    else
    {
        iMajorLineCount = m_iGridExtent;
    }
    // take in to account symmetry, the origin line, and both axes
    iMajorLineCount = ((iMajorLineCount * 2) + 1) * 2;

    // calculate the number of major verts
    iMajorVertCount = iMajorLineCount * 2;
    // the number of minor verts = total verts - major verts
    iMinorVertCount = ((m_iGridExtent * 2) + 1) * 4 - 
        iMajorVertCount;


    m_spGrid = NiNew NiMesh();
    m_spGrid->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);
    NiDataStreamRef* pkMinorStreamRef = 
    m_spGrid->AddStream( NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, iMinorVertCount, 
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_VERTEX, NULL, false);
    NIASSERT(pkMinorStreamRef);
    NiDataStream* pkMinorStream = pkMinorStreamRef->GetDataStream();
    NiPoint3* pkMinorVerts = (NiPoint3*)pkMinorStream->Lock(
        NiDataStream::LOCK_WRITE);

    m_spMajorLines = NiNew NiMesh();
    m_spMajorLines->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);
    NiDataStreamRef* pkMajorStreamRef = 
    m_spMajorLines->AddStream( NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, iMajorVertCount, 
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_VERTEX, NULL, false);
    NIASSERT(pkMajorStreamRef);
    NiDataStream* pkMajorStream = pkMajorStreamRef->GetDataStream();
    NiPoint3* pkMajorVerts = (NiPoint3*)pkMajorStream->Lock(
        NiDataStream::LOCK_WRITE);

    // these count how far into the respective vertex buffers we've filled
    int iMinorCount = 0;
    int iMajorCount = 0;
    for (int i = 0; i <= 2 * m_iGridExtent; i++)
    {
        // figure out if this is a major or minor line
        int iLineNum = i - m_iGridExtent;
        if (m_iMajorLines < 2 || iLineNum % m_iMajorLines == 0)
        {
            // if the line number we are on is divisible by Major lines, 
            // we are on a major line
            // first build x axis line
            pkMajorVerts[iMajorCount] = NiPoint3(-m_iGridExtent * 
                fSpacing, iLineNum * fSpacing, 0.0f);
            pkMajorVerts[iMajorCount + 1] = NiPoint3(m_iGridExtent *
                fSpacing, iLineNum * fSpacing, 0.0f);

            // now build y axis line
            pkMajorVerts[iMajorCount + 2] = NiPoint3(iLineNum * fSpacing, 
                -m_iGridExtent * fSpacing, 0.0f);
            pkMajorVerts[iMajorCount + 3] = NiPoint3(iLineNum * fSpacing, 
                m_iGridExtent * fSpacing, 0.0f);

            iMajorCount += 4;
        }
        else
        {
            // first build x axis line
            pkMinorVerts[iMinorCount] = NiPoint3(-m_iGridExtent * 
                fSpacing, iLineNum * fSpacing, 0.0f);
            pkMinorVerts[iMinorCount + 1] = NiPoint3(m_iGridExtent *
                fSpacing, iLineNum * fSpacing, 0.0f);

            // now build y axis line
            pkMinorVerts[iMinorCount + 2] = NiPoint3(iLineNum * fSpacing, 
                -m_iGridExtent * fSpacing, 0.0f);
            pkMinorVerts[iMinorCount + 3] = NiPoint3(iLineNum * fSpacing, 
                m_iGridExtent * fSpacing, 0.0f);

            iMinorCount += 4;
        }
    }
    pkMinorStream->Unlock(NiDataStream::LOCK_WRITE);
    pkMajorStream->Unlock(NiDataStream::LOCK_WRITE);

    NiMaterialProperty* pkGridMaterial = NiNew NiMaterialProperty();
    pkGridMaterial->SetAmbientColor(NiColor::BLACK);
    pkGridMaterial->SetDiffuseColor(NiColor::BLACK);
    pkGridMaterial->SetSpecularColor(NiColor::BLACK);
    pkGridMaterial->SetEmittance(m_kGridColor);
    pkGridMaterial->SetShineness(0.0f);
    pkGridMaterial->SetAlpha(1.0f);
    m_spGrid->AttachProperty(pkGridMaterial);

    NiVertexColorProperty* pkGridVCProp = NiNew NiVertexColorProperty();
    pkGridVCProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    m_spGrid->AttachProperty(pkGridVCProp);

    m_spGrid->RecomputeBounds();
    m_spGrid->UpdateProperties();
    m_spGrid->UpdateEffects();
    m_spGrid->Update(0.0f);

    NiMaterialProperty* pkMajorMaterial = NiNew NiMaterialProperty();
    pkMajorMaterial->SetAmbientColor(NiColor::BLACK);
    pkMajorMaterial->SetDiffuseColor(NiColor::BLACK);
    pkMajorMaterial->SetSpecularColor(NiColor::BLACK);
    pkMajorMaterial->SetEmittance(m_kMajorColor);
    pkMajorMaterial->SetShineness(0.0f);
    pkMajorMaterial->SetAlpha(1.0f);
    m_spMajorLines->AttachProperty(pkMajorMaterial);

    NiVertexColorProperty* pkMajorVCProp = NiNew NiVertexColorProperty();
    pkMajorVCProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    m_spMajorLines->AttachProperty(pkMajorVCProp);

    m_spMajorLines->RecomputeBounds();
    m_spMajorLines->UpdateProperties();
    m_spMajorLines->UpdateEffects();
    m_spMajorLines->Update(0.0f);

    m_fGridSpacing = fSpacing;
    m_bValidGrid = true;
}
//---------------------------------------------------------------------------
void NiGridComponent::ConstructDynamicGrid(float fSpacing)
{
    // return if already created.
    if (m_bValidGrid && (m_spGrid || m_spMajorLines))
        return;

    // Determine if render has been created yet.
    MRenderingContext* pmRenderingContext;
    pmRenderingContext = MFramework::Instance->Renderer->RenderingContext;

    if (!pmRenderingContext)
        return;

    NiEntityRenderingContext* pkRenderingContext = 
        pmRenderingContext->GetRenderingContext();

    if (!pkRenderingContext)
        return;

    if (!pkRenderingContext->m_pkRenderer)
        return;

    // the grid will inherit the camera's rotation matrix, so we know that
    // the +z and +y axes correspond to the +x and +y grid lines
    int iMinorVertCount;
    int iMajorVertCount;
    int iMajorLineCount;
    int iExtent;
    float fLeftRange;
    float fRightRange;

    // how many total lines will we need?
    // iExtent is the max number of lines allowed in the frustum rounded
    // up to the nearest interval of m_iMajorLines
    if (m_iMajorLines > 0)
    {
        iExtent = ((int)m_fMaxDensity / m_iMajorLines) + 1;
        iExtent = iExtent * m_iMajorLines;
        // figure out how many total Major lines we'll need
        iMajorLineCount = ((int)m_fMaxDensity / m_iMajorLines) + 1;
        // round up to the next major line and account for two axes
        iMajorLineCount = (iMajorLineCount + 1) * 2;
    }
    else
    {
        iExtent = (int)m_fMaxDensity + 1;
        // figure out how many total Major lines we'll need
        iMajorLineCount = (int)m_fMaxDensity + 1;
        // round up to the next major line and account for two axes
        iMajorLineCount = (iMajorLineCount + 1) * 2;
    }

    // calculate the number of major verts
    iMajorVertCount = iMajorLineCount * 2;
    // the number of minor verts = total verts - major verts
    iMinorVertCount = (iExtent + 1) * 4 - 
        iMajorVertCount;

    m_spGrid = NiNew NiMesh();
    m_spGrid->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);
    NiDataStreamRef* pkMinorStreamRef = 
    m_spGrid->AddStream( NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, iMinorVertCount, 
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_VERTEX, NULL, false);
    NIASSERT(pkMinorStreamRef);
    NiDataStream* pkMinorStream = pkMinorStreamRef->GetDataStream();
    NiPoint3* pkMinorVerts = (NiPoint3*)pkMinorStream->Lock(
        NiDataStream::LOCK_WRITE);

    m_spMajorLines = NiNew NiMesh();
    m_spMajorLines->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);
    NiDataStreamRef* pkMajorStreamRef = 
    m_spMajorLines->AddStream( NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, iMajorVertCount, 
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_VERTEX, NULL, false);
    NIASSERT(pkMajorStreamRef);
    NiDataStream* pkMajorStream = pkMajorStreamRef->GetDataStream();
    NiPoint3* pkMajorVerts = (NiPoint3*)pkMajorStream->Lock(
        NiDataStream::LOCK_WRITE);

    // these ranges are how many units from the edge the center of the grid
    // mesh is
    float fAdjustedHalfExtent;
    if (m_iMajorLines > 0)
    {
        fAdjustedHalfExtent = -NiFloor((float)(iExtent / 2.0f) / m_iMajorLines)
            * m_iMajorLines;
    }
    else
    {
        fAdjustedHalfExtent = -NiFloor((float)(iExtent / 2.0f));
    }
    fRightRange = (iExtent + fAdjustedHalfExtent) * fSpacing;
    fLeftRange = fAdjustedHalfExtent * fSpacing;

    // these count how far into the respective vertex buffers we've filled
    int iMinorCount = 0;
    int iMajorCount = 0;
    for (int i = 0; i <= iExtent; i++)
    {
        // line num starts negative such that a major grid line crosses the
        // origin but the the grid is still somewhat centered about origin
        int iLineNum;
        iLineNum = i + (int)fAdjustedHalfExtent;

        // figure out if this is a major or minor line
        if (m_iMajorLines < 2 || i % m_iMajorLines == 0)
        {
            // if the line number we are on is divisible by Major lines, 
            // we are on a major line
            // first build z axis line (vertical)
            pkMajorVerts[iMajorCount] = NiPoint3(0.0f, 
                iLineNum * fSpacing, fLeftRange);
            pkMajorVerts[iMajorCount + 1] = NiPoint3(0.0f, 
                iLineNum * fSpacing, fRightRange);

            // now build y axis line (horizontal)
            pkMajorVerts[iMajorCount + 2] = NiPoint3(0.0f, 
                fLeftRange, iLineNum * fSpacing);
            pkMajorVerts[iMajorCount + 3] = NiPoint3(0.0f, 
                fRightRange, iLineNum * fSpacing);

            iMajorCount += 4;
        }
        else
        {
            // first build z axis line (vertical)
            pkMinorVerts[iMinorCount] = NiPoint3(0.0f, 
                iLineNum * fSpacing, fLeftRange);
            pkMinorVerts[iMinorCount + 1] = NiPoint3(0.0f, 
                iLineNum * fSpacing, fRightRange);

            // now build y axis line (horizontal)
            pkMinorVerts[iMinorCount + 2] = NiPoint3(0.0f, 
                fLeftRange, iLineNum * fSpacing);
            pkMinorVerts[iMinorCount + 3] = NiPoint3(0.0f, 
                fRightRange, iLineNum * fSpacing);

            iMinorCount += 4;
        }
    }
    pkMinorStream->Unlock(NiDataStream::LOCK_WRITE);
    pkMajorStream->Unlock(NiDataStream::LOCK_WRITE);

    NiMaterialProperty* pkGridMaterial = NiNew NiMaterialProperty();
    pkGridMaterial->SetAmbientColor(NiColor::BLACK);
    pkGridMaterial->SetDiffuseColor(NiColor::BLACK);
    pkGridMaterial->SetSpecularColor(NiColor::BLACK);
    pkGridMaterial->SetEmittance(m_kGridColor);
    pkGridMaterial->SetShineness(0.0f);
    pkGridMaterial->SetAlpha(1.0f);
    m_spGrid->AttachProperty(pkGridMaterial);

    NiVertexColorProperty* pkGridVCProp = NiNew NiVertexColorProperty();
    pkGridVCProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    m_spGrid->AttachProperty(pkGridVCProp);

    m_spGrid->RecomputeBounds();
    m_spGrid->UpdateProperties();
    m_spGrid->UpdateEffects();
    m_spGrid->Update(0.0f);

    NiMaterialProperty* pkMajorMaterial = NiNew NiMaterialProperty();
    pkMajorMaterial->SetAmbientColor(NiColor::BLACK);
    pkMajorMaterial->SetDiffuseColor(NiColor::BLACK);
    pkMajorMaterial->SetSpecularColor(NiColor::BLACK);
    pkMajorMaterial->SetEmittance(m_kMajorColor);
    pkMajorMaterial->SetShineness(0.0f);
    pkMajorMaterial->SetAlpha(1.0f);
    m_spMajorLines->AttachProperty(pkMajorMaterial);

    NiVertexColorProperty* pkMajorVCProp = NiNew NiVertexColorProperty();
    pkMajorVCProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    m_spMajorLines->AttachProperty(pkMajorVCProp);

    m_spMajorLines->RecomputeBounds();
    m_spMajorLines->UpdateProperties();
    m_spMajorLines->UpdateEffects();
    m_spMajorLines->Update(0.0f);

    m_bValidGrid = true;
}
//---------------------------------------------------------------------------
void NiGridComponent::AlignGrid(NiCamera* pkCamera)
{
    // this function must handle the grid geometry's translation
    // if in perspective, stick to the origin
    // if in ortho, we must move it to be in the viewport and push it back
    // to near the far clip plane so it doesn't draw over anything.
    if ((pkCamera->GetViewFrustum().m_bOrtho) && (!m_bStaticGrid))
    {
        // first, we set the values for the axes perpendicular to the view
        // we must make the origin of the grid round off to the nearest
        // major line location to the camera's focal point
        // also, orient the lines relative to the camera
        // and scale the lines based off of their spacing
        int iOffsetX;
        int iOffsetY;
        float fSpacingSetting;
        NiPoint3 kNewPos;
        NiPoint3 kLook;
        NiMatrix3 kCamRot;

        fSpacingSetting = GetTrueSpacing(pkCamera, m_fGridSpacing);
        kNewPos = pkCamera->GetTranslate();
        kCamRot = pkCamera->GetRotate();
        iOffsetY = ((NiPoint3::UNIT_X * kCamRot).x > 0.1f) ? 1 : 0;
        iOffsetX = (((NiPoint3::UNIT_X * kCamRot).y > 0.1f) ||
            !((NiPoint3::UNIT_X * kCamRot).z > 0.1f)) ? 1 : 0;
        kCamRot.GetCol(0, kLook);
        float fSpaceFactor;
        if (m_iMajorLines > 1)
        {
            fSpaceFactor = m_iMajorLines * fSpacingSetting;
        }
        else
        {
            fSpaceFactor = fSpacingSetting;
        }
        // round newpos off to the nearest m_iLines * m_fGridSpacing
        kNewPos.x = (NiFloor(kNewPos.x / (fSpaceFactor)) +
            iOffsetX) * (fSpaceFactor);
        kNewPos.y = (NiFloor(kNewPos.y / (fSpaceFactor)) + 
            iOffsetY) * (fSpaceFactor);
        kNewPos.z = (NiFloor(kNewPos.z / (fSpaceFactor))) * 
            (fSpaceFactor);
        kNewPos -= kLook * kNewPos.Dot(kLook);

        m_spGrid->SetTranslate(kNewPos);
        m_spGrid->SetRotate(pkCamera->GetRotate());
        m_spGrid->SetScale((fSpacingSetting / m_fGridSpacing));

        m_spMajorLines->SetTranslate(kNewPos);
        m_spMajorLines->SetRotate(pkCamera->GetRotate());
        m_spMajorLines->SetScale((fSpacingSetting / m_fGridSpacing));

        m_spGrid->Update(0.0f);
        m_spMajorLines->Update(0.0f);
    }
    else
    {
        m_spGrid->SetTranslate(NiPoint3::ZERO);
        m_spMajorLines->SetTranslate(NiPoint3::ZERO);
    }
}
//---------------------------------------------------------------------------
// NiEntityComponentInterface overrides.
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiGridComponent::Clone(bool)
{
    return NiNew NiGridComponent(m_fGridSpacing, m_iMajorLines, m_iGridExtent,
        m_kGridColor, m_kMajorColor, m_bStaticGrid);
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiGridComponent::GetMasterComponent() const
{
    // This component does not have a master component.
    return NULL;
}
//---------------------------------------------------------------------------
void NiGridComponent::SetMasterComponent(
    NiEntityComponentInterface*)
{
    // This component does not have a master component.
}
//---------------------------------------------------------------------------
void NiGridComponent::GetDependentPropertyNames(
    NiTObjectSet<NiFixedString>&)
{
}
//---------------------------------------------------------------------------
// NiEntityPropertyInterface overrides.
//---------------------------------------------------------------------------
NiBool NiGridComponent::SetTemplateID(const NiUniqueID&)
{
    //Not supported for custom components
    return false;
}
//---------------------------------------------------------------------------
NiUniqueID  NiGridComponent::GetTemplateID()
{
    static const NiUniqueID kUniqueID = 
        NiUniqueID(0x35,0x83,0x33,0x47,0xFE,0x1E,0x1F,0x45,0xB5,0x83,0xAB,0xD6,
        0xB2,0x83,0x5B,0xC2);
    return kUniqueID;
}
//---------------------------------------------------------------------------
void NiGridComponent::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiGridComponent::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiGridComponent::GetClassName() const
{
    return ms_kClassName;
}
//---------------------------------------------------------------------------
NiFixedString NiGridComponent::GetName() const
{
    return ms_kComponentName;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::SetName(const NiFixedString&)
{
    // This component does not allow its name to be set.
    return false;
}
//---------------------------------------------------------------------------
void NiGridComponent::Update(NiEntityPropertyInterface*, 
    float, NiEntityErrorInterface*,
    NiExternalAssetManager*)
{
    // this component has no need to update
}
//---------------------------------------------------------------------------
void NiGridComponent::BuildVisibleSet(NiEntityRenderingContext* 
    pkRenderingContext, NiEntityErrorInterface*)
{
    if (!m_bDisplayGrid)
    {
        return;
    }

    RebuildInvalidGrid();

    // we are ready to render
    // first, determine where to place the grid
    AlignGrid(pkRenderingContext->m_pkCamera);

    NiVisibleArray* pkVisibleSet = pkRenderingContext->m_pkCullingProcess
        ->GetVisibleSet();
    NIASSERT(pkVisibleSet);
    pkVisibleSet->Add(*m_spMajorLines);
    pkVisibleSet->Add(*m_spGrid);
}
//---------------------------------------------------------------------------
void NiGridComponent::GetPropertyNames(
    NiTObjectSet<NiFixedString>& kPropertyNames) const
{
    kPropertyNames.Add(ms_kMajorLinesString);
    kPropertyNames.Add(ms_kGridExtentString);
    kPropertyNames.Add(ms_kGridColorString);
    kPropertyNames.Add(ms_kMajorColorString);
    kPropertyNames.Add(ms_kStaticString);
    kPropertyNames.Add(ms_kSceneRootString);
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::CanResetProperty(const NiFixedString& kPropertyName, 
    bool& bCanReset) const
{
    if (kPropertyName == ms_kMajorLinesString ||
        kPropertyName == ms_kGridExtentString ||
        kPropertyName == ms_kGridColorString ||
        kPropertyName == ms_kMajorColorString ||
        kPropertyName == ms_kStaticString ||
        kPropertyName == ms_kSceneRootString)
    {
        bCanReset = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::ResetProperty(const NiFixedString&)
{
    // No properties can be reset.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::MakePropertyUnique(const NiFixedString& kPropertyName,
    bool& bMadeUnique)
{
    if (kPropertyName == ms_kMajorLinesString ||
        kPropertyName == ms_kGridExtentString ||
        kPropertyName == ms_kGridColorString ||
        kPropertyName == ms_kMajorColorString ||
        kPropertyName == ms_kStaticString ||
        kPropertyName == ms_kSceneRootString)
    {
        // No properties are inherited, so they are all already unique.
        bMadeUnique = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::GetDisplayName(const NiFixedString& kPropertyName, 
    NiFixedString& kDisplayName) const
{
    if (kPropertyName == ms_kMajorLinesString ||
        kPropertyName == ms_kGridExtentString ||
        kPropertyName == ms_kGridColorString ||
        kPropertyName == ms_kMajorColorString)
    {
        kDisplayName = kPropertyName;
        return true;
    }
    else if ((kPropertyName == ms_kSceneRootString) ||
        (kPropertyName == ms_kStaticString))
    {
        kDisplayName = NULL;
        return true;
    }
    
    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::SetDisplayName(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the display name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::GetPrimitiveType(const NiFixedString& kPropertyName, 
    NiFixedString& kPrimitiveType) const
{
    if (kPropertyName == ms_kMajorLinesString)
    {
        kPrimitiveType = PT_INT;
    }
    else if (kPropertyName == ms_kGridExtentString)
    {
        kPrimitiveType = PT_INT;
    }
    else if (kPropertyName == ms_kGridColorString)
    {
        kPrimitiveType = PT_COLOR;
    }
    else if (kPropertyName == ms_kMajorColorString)
    {
        kPrimitiveType = PT_COLOR;
    }
    else if (kPropertyName == ms_kStaticString)
    {
        kPrimitiveType = PT_BOOL;
    }
    else if (kPropertyName == ms_kSceneRootString)
    {
        kPrimitiveType = PT_NIOBJECTPOINTER;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::SetPrimitiveType(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the primitive type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::GetSemanticType(const NiFixedString& kPropertyName, 
    NiFixedString& kSemanticType) const
{
    if (kPropertyName == ms_kMajorLinesString)
    {
        kSemanticType = PT_INT;
    }
    else if (kPropertyName == ms_kGridExtentString)
    {
        kSemanticType = PT_INT;
    }
    else if (kPropertyName == ms_kGridColorString)
    {
        kSemanticType = PT_COLOR;
    }
    else if (kPropertyName == ms_kMajorColorString)
    {
        kSemanticType = PT_COLOR;
    }
    else if (kPropertyName == ms_kStaticString)
    {
        kSemanticType = PT_BOOL;
    }
    else if (kPropertyName == ms_kSceneRootString)
    {
        kSemanticType = PT_NIOBJECTPOINTER;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::SetSemanticType(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the semantic type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::GetDescription(const NiFixedString& kPropertyName, 
    NiFixedString& kDescription) const
{
    if (kPropertyName == ms_kMajorLinesString ||
        kPropertyName == ms_kGridExtentString ||
        kPropertyName == ms_kGridColorString ||
        kPropertyName == ms_kMajorColorString ||
        kPropertyName == ms_kStaticString ||
        kPropertyName == ms_kSceneRootString)
    {
        kDescription = kPropertyName;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::SetDescription(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the description to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::GetCategory(const NiFixedString& kPropertyName, 
    NiFixedString& kCategory) const
{
    if (kPropertyName == ms_kMajorLinesString ||
        kPropertyName == ms_kGridExtentString ||
        kPropertyName == ms_kGridColorString ||
        kPropertyName == ms_kMajorColorString ||
        kPropertyName == ms_kStaticString ||
        kPropertyName == ms_kSceneRootString)
    {
        kCategory = ms_kComponentName;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::IsPropertyReadOnly(const NiFixedString& kPropertyName, 
    bool& bIsReadOnly)
{
    if (kPropertyName == ms_kMajorLinesString ||
        kPropertyName == ms_kGridExtentString ||
        kPropertyName == ms_kGridColorString ||
        kPropertyName == ms_kMajorColorString)
    {
        bIsReadOnly = false;
        return true;
    }
    else if ((kPropertyName == ms_kSceneRootString) ||
        (kPropertyName == ms_kStaticString))
    {
        bIsReadOnly = true;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::IsPropertyUnique(
    const NiFixedString& kPropertyName, bool& bIsUnique)
{
    if (kPropertyName == ms_kMajorLinesString ||
        kPropertyName == ms_kGridExtentString ||
        kPropertyName == ms_kGridColorString ||
        kPropertyName == ms_kMajorColorString ||
        kPropertyName == ms_kStaticString ||
        kPropertyName == ms_kSceneRootString)
    {
        bIsUnique = true;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::IsPropertySerializable(
    const NiFixedString& kPropertyName, bool& bIsSerializable)
{
    bool bIsUnique;
    NiBool bSuccess = IsPropertyUnique(kPropertyName, bIsUnique);
    if (bSuccess)
    {
        bool bIsReadOnly;
        bSuccess = IsPropertyReadOnly(kPropertyName, bIsReadOnly);
        NIASSERT(bSuccess);

        bIsSerializable = bIsUnique && !bIsReadOnly;
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::IsPropertyInheritable(
    const NiFixedString& kPropertyName, bool& bIsInheritable)
{
    if (kPropertyName == ms_kMajorLinesString ||
        kPropertyName == ms_kGridExtentString ||
        kPropertyName == ms_kGridColorString ||
        kPropertyName == ms_kMajorColorString ||
        kPropertyName == ms_kStaticString ||
        kPropertyName == ms_kSceneRootString)
    {
        bIsInheritable = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::IsExternalAssetPath(
    const NiFixedString& kPropertyName, unsigned int,
    bool& bIsExternalAssetPath) const
{
    if (kPropertyName == ms_kMajorLinesString ||
        kPropertyName == ms_kGridExtentString ||
        kPropertyName == ms_kGridColorString ||
        kPropertyName == ms_kMajorColorString ||
        kPropertyName == ms_kStaticString ||
        kPropertyName == ms_kSceneRootString)
    {
        bIsExternalAssetPath = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::GetElementCount(const NiFixedString& kPropertyName,
    unsigned int& uiCount) const
{
    if (kPropertyName == ms_kMajorLinesString ||
        kPropertyName == ms_kGridExtentString ||
        kPropertyName == ms_kGridColorString ||
        kPropertyName == ms_kMajorColorString ||
        kPropertyName == ms_kStaticString ||
        kPropertyName == ms_kSceneRootString)
    {
        uiCount = 1;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::SetElementCount(const NiFixedString& kPropertyName,
    unsigned int, bool& bCountSet)
{
    if (kPropertyName == ms_kMajorLinesString ||
        kPropertyName == ms_kGridExtentString ||
        kPropertyName == ms_kGridColorString ||
        kPropertyName == ms_kMajorColorString ||
        kPropertyName == ms_kStaticString ||
        kPropertyName == ms_kSceneRootString)
    {
        bCountSet = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::IsCollection(const NiFixedString& kPropertyName,
    bool& bIsCollection) const
{
    if (kPropertyName == ms_kMajorLinesString ||
        kPropertyName == ms_kGridExtentString ||
        kPropertyName == ms_kGridColorString ||
        kPropertyName == ms_kMajorColorString || 
        kPropertyName == ms_kStaticString ||
        kPropertyName == ms_kSceneRootString)
    {
        bIsCollection = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::GetPropertyData(const NiFixedString& kPropertyName,
    bool& bData, unsigned int uiIndex) const
{
    if ((kPropertyName == ms_kStaticString) && (uiIndex == 0))
    {
        bData = m_bStaticGrid;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::GetPropertyData(const NiFixedString& kPropertyName, 
    int& iData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kMajorLinesString)
    {
        iData = m_iMajorLines;
    }
    else if (kPropertyName == ms_kGridExtentString)
    {
        iData = m_iGridExtent;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::SetPropertyData(const NiFixedString& kPropertyName, 
    int iData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kMajorLinesString)
    {
        SetMajorLineSpacing(iData);
    }
    else if (kPropertyName == ms_kGridExtentString)
    {
        SetGridExtent(iData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::GetPropertyData(const NiFixedString& kPropertyName, 
    NiColor& kData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kGridColorString)
    {
        kData = m_kGridColor;
    }
    else if (kPropertyName == ms_kMajorColorString)
    {
        kData = m_kMajorColor;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::SetPropertyData(const NiFixedString& kPropertyName, 
    const NiColor& kData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kGridColorString)
    {
        SetGridColor(kData);
    }
    else if (kPropertyName == ms_kMajorColorString)
    {
        SetMajorColor(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiGridComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiObject*& pkData, unsigned int) const
{
    if (kPropertyName == ms_kSceneRootString)
    {
        pkData = m_spGrid;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
