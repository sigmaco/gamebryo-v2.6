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

// Precmpiled Header
#include "NiVisualTrackerPCH.h"

#include "NiVisualTracker.h"
#include <NiMaterialProperty.h>
#include <NiZBufferProperty.h>
#include <NiWireframeProperty.h>
#include <NiVertexColorProperty.h>
#include <NiCamera.h>
#include <NiMesh.h>
#include <NiDataStream.h>
#include <NiDataStreamElement.h>
#include <NiSystem.h>
#include <NiRect.h>
#include <NiRenderer.h>
#include <NiMeshScreenElements.h>
#include <NiTexturingProperty.h>
#include <NiSourceTexture.h>
#include <NiDataStreamLock.h>
#include <NiCommonSemantics.h>
#include "Courier16.h"

typedef NiTStridedRandomAccessIterator<NiPoint3>    Point3Iter;

//---------------------------------------------------------------------------
//  NiVisualTrackerBase implementation
//---------------------------------------------------------------------------
NiVisualTrackerBase::NiVisualTrackerBase(NiRect<float> kWindowRect,
    bool bShow,
    const char* pcName)
    : m_kDimensions(kWindowRect),
    m_bShow(bShow), 
    m_kScreenTextures(6, 1)
{
    NiSprintf(m_acName, 255, "%s", pcName);
    m_acName[255] = '\0';
}
//---------------------------------------------------------------------------
NiVisualTrackerBase::~NiVisualTrackerBase()
{
}
//---------------------------------------------------------------------------
void NiVisualTrackerBase::SetShow(bool bShow)
{
    m_bShow = bShow;
}
//---------------------------------------------------------------------------
bool NiVisualTrackerBase::GetShow()
{
    return m_bShow;
}
//---------------------------------------------------------------------------
void NiVisualTrackerBase::Update()
{
}
//---------------------------------------------------------------------------
void NiVisualTrackerBase::Draw()
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  NiVisualTracker implementation
//---------------------------------------------------------------------------
NiVisualTracker::NiVisualTracker(float fMaxValue, 
    unsigned int uiNumDecimalPlaces,
    NiRect<float> kWindowRect,
    const char* pcName,
    bool bShow,
    unsigned int uiNumTrackers) 
    : NiVisualTrackerBase(kWindowRect, bShow, pcName),
    m_kCallbackData(uiNumTrackers, 1), 
    m_fMaxValue(fMaxValue)
{
    Ni2DBuffer* pkBuffer = NiRenderer::GetRenderer()->GetDefaultBackBuffer();
    NIASSERT(pkBuffer);

    unsigned int uiBBWidth = pkBuffer->GetWidth();
    unsigned int uiBBHeight = pkBuffer->GetHeight();
    
    m_pkTextTitle = NiNew ScreenText(32, &m_kScreenTextures, NiColorA::WHITE);

    m_spWindowRoot = NiNew NiNode(uiNumTrackers);
    m_spWindowRoot->SetTranslate(NiPoint3(m_kDimensions.m_left,
        m_kDimensions.m_top, 0.1f));

    // Graph visualization dimensions
    float fOuterWidth = NiAbs(m_kDimensions.m_left - m_kDimensions.m_right);
    float fOuterHeight = NiAbs(m_kDimensions.m_top - m_kDimensions.m_bottom);
   
    unsigned int uiSpacing = m_pkTextTitle->GetCharacterSpacing();
    float fNormTextWidth = ((float)uiSpacing)/((float)uiBBWidth);
    float fNormTextHeight = 
        ((float)m_pkTextTitle->GetHeight())/((float)uiBBHeight);

    m_kGraphLineDimensions.m_left = 0.0f;
    m_kGraphLineDimensions.m_right = fOuterWidth - 0.10f*fOuterWidth;
    m_kGraphLineDimensions.m_top = fNormTextHeight;
    m_kGraphLineDimensions.m_bottom = fOuterHeight - fNormTextHeight;

    float fWidth = NiAbs(m_kGraphLineDimensions.m_left - 
        m_kGraphLineDimensions.m_right);
    float fHeight = NiAbs(m_kGraphLineDimensions.m_top - 
        m_kGraphLineDimensions.m_bottom);
    
    // Create the border line segments
    unsigned int uiNumVerts = 11;
    m_spBorders = GraphCallbackObjectData::CreateLines(
        uiNumVerts, NiColor::WHITE, m_kGraphLineDimensions);

    // Obtain positions lock (unlocks when goes out of scope)
    NiDataStreamRef* pkStreamRef = m_spBorders->FindStreamRef("POSITION", 0, 
        NiDataStreamElement::F_FLOAT32_3);
    NIASSERT(pkStreamRef);

    // Scope the lock appropriately
    {
        NiDataStreamLock kPosLock(pkStreamRef->GetDataStream(), 0);

        Point3Iter kPosIt = kPosLock.begin<NiPoint3>();
        
        // Build vertex data and region data
        NiDataStream* pkDataStream = kPosLock.GetDataStream();

        // Clear region information
        pkDataStream->RemoveAllRegions();

        // Remember y runs from top (0.0) to bottom (1.0)
        // in Gamebryo screen space.
        
        // The frame vertices
        *kPosIt++ = NiPoint3(0.0f,   fHeight, 0.0f);
        *kPosIt++ = NiPoint3(fWidth, fHeight, 0.0f);
        *kPosIt++ = NiPoint3(fWidth, 0.0f, 0.0f);
        *kPosIt++ = NiPoint3(0.0f, 0.0f, 0.0f);
        *kPosIt++ = NiPoint3(0.0f, fHeight, 0.0f);
        pkDataStream->AddRegion(NiDataStream::Region(0, 5));
        pkStreamRef->BindRegionToSubmesh(0,0);

        // The 0.25% line
        *kPosIt++ = NiPoint3(0.0f, 0.75f*fHeight, 0.0f);
        *kPosIt++ = NiPoint3(fWidth, 0.75f*fHeight, 0.0f);
        pkDataStream->AddRegion(NiDataStream::Region(5, 2));
        pkStreamRef->BindRegionToSubmesh(1,1);

        // The 0.50% line
        *kPosIt++ = NiPoint3(0.0f, 0.5f*fHeight, 0.0f);
        *kPosIt++ = NiPoint3(fWidth, 0.5f*fHeight, 0.0f);
        pkDataStream->AddRegion(NiDataStream::Region(7, 2));
        pkStreamRef->BindRegionToSubmesh(2,2);
        
        // The 0.75% line
        *kPosIt++ = NiPoint3(0.0f, 0.25f*fHeight, 0.0f);
        *kPosIt++ = NiPoint3(fWidth, 0.25f*fHeight, 0.0f);
        pkDataStream->AddRegion(NiDataStream::Region(9, 2));
        pkStreamRef->BindRegionToSubmesh(3,3);

    }

    // Obtain positions lock (unlocks when goes out of scope)   
    NiMesh* pkMesh = NiSmartPointerCast(NiMesh,m_spBorders);
    pkMesh->RecomputeBounds();
    pkMesh->SetSubmeshCount(4);
   
    // Attach object to scene
    m_spWindowRoot->AttachChild(m_spBorders);

    // Build labels
    size_t stLen = strlen(m_acName);

    float fOffset = fWidth - fNormTextWidth*(float)stLen;
    float fNormX;
    
    if (fOffset > 1.0f)
        fOffset = 0.0f;

    fNormX = fOffset / 2.0f;

    unsigned int uiXPos, uiYPos;
    uiXPos = (unsigned int)((fNormX + m_kDimensions.m_left) *
        (float)uiBBWidth);
    uiYPos = (unsigned int)((m_kDimensions.m_top) * (float)uiBBHeight);
    m_pkTextTitle->SetTextOrigin(uiXPos, uiYPos);
    m_pkTextTitle->SetVisible(true);
    m_pkTextTitle->SetString(m_acName);

    char acNumberString[16];
    uiXPos =  (NiUInt32)((m_kDimensions.m_left + 
        m_kGraphLineDimensions.m_right + 0.01f) * (float)uiBBWidth);
    uiYPos = (NiUInt32)(
        (m_kDimensions.m_top + m_kGraphLineDimensions.m_top - 
        0.5f * fNormTextHeight) * (float)uiBBHeight);

    NiUInt32 uiHeightIncrement = 
        (NiUInt32)((0.25f*fHeight)*(float)uiBBHeight);

    char acFormatString[128];
    NiSprintf(acFormatString, 127, "%%.%df", uiNumDecimalPlaces);

    // 100% text
    NiSprintf(acNumberString, 14, acFormatString, m_fMaxValue);
    acNumberString[15] = '\0';
    m_pkTextOneHundred = NiNew ScreenText(15, &m_kScreenTextures,
        NiColorA::WHITE);
    m_pkTextOneHundred->SetTextOrigin(uiXPos, uiYPos);
    m_pkTextOneHundred->SetVisible(true);
    m_pkTextOneHundred->SetString(acNumberString);

    // 75% text
    uiYPos += uiHeightIncrement;
    NiSprintf(acNumberString, 14, acFormatString, 0.75f*m_fMaxValue);
    acNumberString[15] = '\0';
    m_pkTextSeventyFive = NiNew ScreenText(15, &m_kScreenTextures,
        NiColorA::WHITE);
    m_pkTextSeventyFive->SetTextOrigin(uiXPos, uiYPos);
    m_pkTextSeventyFive->SetVisible(true);
    m_pkTextSeventyFive->SetString(acNumberString);

    // 50% text
    uiYPos += uiHeightIncrement;
    NiSprintf(acNumberString, 14, acFormatString, 0.5f*m_fMaxValue);
    acNumberString[15] = '\0';
    m_pkTextFifty = NiNew ScreenText(15, &m_kScreenTextures, 
        NiColorA::WHITE);
    m_pkTextFifty->SetTextOrigin(uiXPos, uiYPos);
    m_pkTextFifty->SetVisible(true);
    m_pkTextFifty->SetString(acNumberString);

    // 25% text
    uiYPos += uiHeightIncrement;
    NiSprintf(acNumberString, 14, acFormatString, 0.25f*m_fMaxValue);
    acNumberString[15] = '\0';
    m_pkTextTwentyFive = NiNew ScreenText(15, &m_kScreenTextures, 
        NiColorA::WHITE);
    m_pkTextTwentyFive->SetTextOrigin(uiXPos, uiYPos);
    m_pkTextTwentyFive->SetVisible(true);
    m_pkTextTwentyFive->SetString(acNumberString);

    // 0.0% text
    uiYPos += uiHeightIncrement;
    NiSprintf(acNumberString, 14, acFormatString, 0.0f);
    acNumberString[15] = '\0';
    m_pkTextZero = NiNew ScreenText(32, &m_kScreenTextures, 
        NiColorA::WHITE);
    m_pkTextZero->SetTextOrigin(uiXPos, uiYPos);
    m_pkTextZero->SetVisible(true);
    m_pkTextZero->SetString(acNumberString);

    // Define legend location
    m_uiLegendX = (unsigned int)((m_kDimensions.m_left) * (float)uiBBWidth);
    m_uiLegendY = (unsigned int)((m_kDimensions.m_bottom - 
        fNormTextHeight) * (float)uiBBHeight);
 }
//---------------------------------------------------------------------------
NiVisualTracker::~NiVisualTracker()
{
    m_spBorders = 0;

    for (unsigned int ui = 0; ui < m_kCallbackData.GetSize(); ui++)
    {
        NiDelete m_kCallbackData.GetAt(ui);
    }

    NiDelete m_pkTextTitle;
    NiDelete m_pkTextZero;
    NiDelete m_pkTextTwentyFive;
    NiDelete m_pkTextFifty;
    NiDelete m_pkTextSeventyFive;
    NiDelete m_pkTextOneHundred;

}
//---------------------------------------------------------------------------
unsigned int NiVisualTracker::AddGraph(GraphCallbackObject* pkObject, 
    const char* pcName, const NiColor& kColor, 
    unsigned int uiNumSamplesToKeep, float fSamplingTime, bool bShow)
{
    
    NiVisualTracker::GraphCallbackObjectData* pkCBData = NiNew
        NiVisualTracker::GraphCallbackObjectData(pkObject, 
        pcName, kColor, uiNumSamplesToKeep, m_fMaxValue, 
        fSamplingTime, bShow, m_kGraphLineDimensions, m_spWindowRoot,
        m_uiLegendX, m_uiLegendY, m_kScreenTextures);

    return m_kCallbackData.AddFirstEmpty(pkCBData);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void NiVisualTracker::RemoveGraph(const char* pcName)
{
    for (unsigned int ui = 0; ui < m_kCallbackData.GetSize(); ui++)
    {
        NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(ui);
        NIASSERT(pkData);
        if (strcmp(pkData->GetName(), pcName) == 0)
        {
            int iXAdjust = -(int)pkData->GetLegendWidth();

            for (unsigned int uj = ui+1; uj < m_kCallbackData.GetSize(); uj++)
            {
                NiVisualTracker::GraphCallbackObjectData* pkNewData = 
                    m_kCallbackData.GetAt(uj);
                NIASSERT(pkNewData);
                pkNewData->AdjustLegend(iXAdjust, 0);
            }

            m_kCallbackData.RemoveAt(ui);
            NiDelete pkData;

            m_kCallbackData.Compact();

            m_uiLegendX += iXAdjust;
            break;
        }
    }   
}
//---------------------------------------------------------------------------
void NiVisualTracker::RemoveAll()
{
    for (unsigned int ui = 0; ui < m_kCallbackData.GetSize(); ui++)
    {
        NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(ui);

        NiDelete pkData;
    }

    m_kCallbackData.RemoveAll();

    // reset legend
    Ni2DBuffer* pkBuffer = NiRenderer::GetRenderer()->GetDefaultBackBuffer();
    NIASSERT(pkBuffer);
    unsigned int uiBBWidth = pkBuffer->GetWidth();

    m_uiLegendX = (unsigned int)((m_kDimensions.m_left) * (float)uiBBWidth);
}
//---------------------------------------------------------------------------
const char* NiVisualTracker::GetName()
{
    return m_acName;
}
//---------------------------------------------------------------------------
void NiVisualTracker::Update()
{
    float fTime = NiGetCurrentTimeInSec();
    for (unsigned int ui = 0; ui < m_kCallbackData.GetSize(); ui++)
    {
        NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(ui);
        NIASSERT(pkData);
        pkData->Update(fTime);
    }   
    m_spWindowRoot->Update(fTime);
}
//---------------------------------------------------------------------------
void NiVisualTracker::Draw()
{
    if (!m_bShow)
        return;

    NiRenderer::GetRenderer()->SetScreenSpaceCameraData();

    m_spBorders->RenderImmediate(NiRenderer::GetRenderer());

    for (unsigned int ui = 0; ui < m_kCallbackData.GetSize(); ui++)
    {
        NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(ui);
        NIASSERT(pkData);
        pkData->Draw();
    }

    for (unsigned int uj = 0; uj < m_kScreenTextures.GetSize(); uj++)
    {
        NiMeshScreenElements* pkTexture = m_kScreenTextures.GetAt(uj);
        NIASSERT(pkTexture);
        pkTexture->RenderImmediate(NiRenderer::GetRenderer());
    }
}
//---------------------------------------------------------------------------
unsigned int NiVisualTracker::GetGraphCount()
{
    return m_kCallbackData.GetSize();
}
//---------------------------------------------------------------------------
void NiVisualTracker::SetName(const char* pcName, unsigned int uiWhichGraph)
{
    NIASSERT(uiWhichGraph < m_kCallbackData.GetSize());

    NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(uiWhichGraph);
    NIASSERT(pkData);
    int iXAdjust = -(int)pkData->GetLegendWidth();

    pkData->SetName(pcName);
    iXAdjust += pkData->GetLegendWidth();

    for (unsigned int uj = uiWhichGraph + 1; uj < m_kCallbackData.GetSize();
        uj++)
    {
        NiVisualTracker::GraphCallbackObjectData* pkNewData = 
            m_kCallbackData.GetAt(uj);
        NIASSERT(pkNewData);
        pkNewData->AdjustLegend(iXAdjust, 0);
    }

}
//---------------------------------------------------------------------------
const char* NiVisualTracker::GetName(unsigned int uiWhichGraph)
{
    NIASSERT(uiWhichGraph < m_kCallbackData.GetSize());

    NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(uiWhichGraph);
    NIASSERT(pkData);
    return pkData->GetName();
}
//---------------------------------------------------------------------------
unsigned int NiVisualTracker::GetGraphIndexByName(const char* pcName)
{
    for (unsigned int ui = 0; ui < m_kCallbackData.GetSize(); ui++)
    {
        NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(ui);
        NIASSERT(pkData);

        if (0 == strcmp(pcName, pkData->GetName()))
            return ui;
    }  

    return (unsigned int) -1;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  NiVisualTracker::GraphCallbackObjectData implementation
//---------------------------------------------------------------------------
void NiVisualTracker::GraphCallbackObjectData::SetShow(bool bShow)
{
    m_bShow = bShow;
}
//---------------------------------------------------------------------------
bool NiVisualTracker::GraphCallbackObjectData::GetShow()
{
    return m_bShow;
}
//---------------------------------------------------------------------------
void NiVisualTracker::GraphCallbackObjectData::SetName(const char* pcName)
{
    NiSprintf(m_acName, 255, "%s", pcName);
    m_acName[255] = '\0';
    m_pkText->SetString(m_acName);
}
//---------------------------------------------------------------------------
const char* NiVisualTracker::GraphCallbackObjectData::GetName()
{
    return m_acName;
}
//---------------------------------------------------------------------------
void NiVisualTracker::GraphCallbackObjectData::Draw()
{
    if (m_bShow)
    {
        m_aspLines[m_uiBufferID]->RenderImmediate(NiRenderer::GetRenderer());
    }
}
//---------------------------------------------------------------------------
void NiVisualTracker::GraphCallbackObjectData::Update(float fTime)
{
    if (m_fLastTime != -NI_INFINITY && 
        fTime - m_fLastTime < m_fSamplingTime)
    {
        return;
    }

    m_uiBufferID++;
    if (m_uiBufferID >= BUFFER_COUNT)
        m_uiBufferID = 0;

    unsigned int uiUpdateBufferID = m_uiBufferID+1;
    if (uiUpdateBufferID >= BUFFER_COUNT)
        uiUpdateBufferID = 0;

    float fDataValue = m_pkCallbackObj->TakeSample(fTime);

    if (fDataValue > m_fMaxValue)
        fDataValue = m_fMaxValue;

    // Push the sampled data at the end of the prev frames array.
    for (NiUInt32 ui = 0; ui < BUFFER_COUNT - 1; ui++)
        m_afPrevFramesValues[ui] = m_afPrevFramesValues[ui+1];
    m_afPrevFramesValues[BUFFER_COUNT - 1] = fDataValue;
   
    float fWidth = NiAbs(m_kDimensions.m_left - m_kDimensions.m_right);
    float fHeight = NiAbs(m_kDimensions.m_top - m_kDimensions.m_bottom);
    
    // Scope the lock appropriately
    {
        // Build vertex data
        NiDataStreamElementLock kPosLock(
            m_aspLines[uiUpdateBufferID], 
            NiCommonSemantics::POSITION(), 
            0, 
            NiDataStreamElement::F_FLOAT32_3, 
            NiDataStream::LOCK_WRITE);
        Point3Iter kPosIt = kPosLock.begin<NiPoint3>();

        for (NiUInt32 ui = 0; 
            ui < m_uiNumSamplesToKeep - BUFFER_COUNT;
            ui++)
        {
            kPosIt[ui] = kPosIt[ui+BUFFER_COUNT];
            kPosIt[ui].x = (((float)ui) / (float)m_uiNumSamplesToKeep) *
                fWidth;
        }

        for (NiUInt32 ui = 1; ui <= BUFFER_COUNT; ui++)
        {
            kPosIt[m_uiNextVertex - BUFFER_COUNT + ui].x = 
                (float)1.0f * fWidth; 
            kPosIt[m_uiNextVertex - BUFFER_COUNT + ui].y = fHeight - 
                ((float)m_afPrevFramesValues[ui-1] / m_fMaxValue) * fHeight;
            kPosIt[m_uiNextVertex - BUFFER_COUNT + ui].z = 0.0f;
        }
            
        // Update region to reflect new sample count
        NiDataStream* pkDataStream = kPosLock.GetDataStream();
        NiDataStream::Region& kRegion = pkDataStream->GetRegion(0);
        kRegion.SetRange(m_uiNumSamplesToKeep);
        
    }
    NiMesh* pkMesh = NiSmartPointerCast(NiMesh, m_aspLines[uiUpdateBufferID]);
    pkMesh->RecomputeBounds();   

    m_fLastTime = fTime;
}
//---------------------------------------------------------------------------
NiMeshPtr NiVisualTracker::GraphCallbackObjectData::CreateLines(
    unsigned int uiNumSamplesToKeep, const NiColor& kColor,
    const NiRect<float>& kDims)
{
    // float fWidth = NiAbs(kDimensions.m_left - kDimensions.m_right);
    float fHeight = NiAbs(kDims.m_top - kDims.m_bottom);
    
    // Build lines mesh and mesh data
    NiMeshPtr spLinesMesh = NiNew NiMesh;
    spLinesMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINESTRIPS);
    spLinesMesh->AddStream(
            NiCommonSemantics::POSITION(), 
            0, 
            NiDataStreamElement::F_FLOAT32_3, 
            uiNumSamplesToKeep, 
            NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_GPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStream::USAGE_VERTEX);
    
    // Initialize the position data
    NiDataStreamElementLock kPosLock(
        spLinesMesh, 
        NiCommonSemantics::POSITION(), 
        0, 
        NiDataStreamElement::F_FLOAT32_3, 
        NiDataStream::LOCK_WRITE);

    NiPoint3 kDefaultValue = NiPoint3(0.0f, fHeight, 0.0f);
    Point3Iter kOutIt = kPosLock.begin<NiPoint3>();
    Point3Iter kEndIt = kPosLock.end<NiPoint3>();
    for (; kOutIt != kEndIt; kOutIt++)
    {    
        *kOutIt = kDefaultValue;
    }

    // Set transform
    spLinesMesh->SetTranslate(NiPoint3(kDims.m_left, kDims.m_top, 0.0f));
    NiMatrix3 kRot;
    kRot.MakeIdentity();
    spLinesMesh->SetRotate(kRot);
    spLinesMesh->SetScale(1.0f);

    // Set color property
    NiMaterialProperty* pkMatProp = NiNew NiMaterialProperty();
    pkMatProp->SetEmittance(kColor);
    spLinesMesh->AttachProperty(pkMatProp);

    // Set depth buffer property
    NiZBufferProperty* pkZProp = NiNew NiZBufferProperty();
    pkZProp->SetZBufferTest(false);
    pkZProp->SetZBufferWrite(false);
    spLinesMesh->AttachProperty(pkZProp);

    spLinesMesh->Update(0.0f);
    spLinesMesh->UpdateProperties();
    spLinesMesh->UpdateEffects();
    spLinesMesh->UpdateNodeBound();

    return spLinesMesh;
}
//---------------------------------------------------------------------------
NiVisualTracker::GraphCallbackObjectData::GraphCallbackObjectData(
    GraphCallbackObject* pkObject, const char* pcName,
    const NiColor& kColor, unsigned int uiNumSamplesToKeep, float fMaxValue,
    float fSamplingTime, bool bShow, const NiRect<float>& kDimensions,
    NiNode* pkParentNode, unsigned int& uiLegendX, unsigned int& uiLegendY,
    NiMeshScreenElementsArray& kTextures)
{
    m_pkCallbackObj = pkObject;
    m_bShow = bShow;
    m_uiNumSamplesToKeep = uiNumSamplesToKeep;
    m_fSamplingTime = fSamplingTime;
    m_kDimensions = kDimensions;

    // Create the buffered line meshes that will be rendered and updated in 
    // a round robin pattern.
    for (unsigned int ui = 0; ui < BUFFER_COUNT; ui++ )
    {
        m_aspLines[ui] = 
            CreateLines(m_uiNumSamplesToKeep, kColor, kDimensions);
        pkParentNode->AttachChild(m_aspLines[ui]);
        m_afPrevFramesValues[ui] = 0;
    }

    m_kColor = kColor;
    m_fLastTime = -NI_INFINITY;
    NiSprintf(m_acName, 255, "%s", pcName);
    m_acName[255] = '\0';
    m_fMaxValue = fMaxValue;
    m_uiNextVertex = m_uiNumSamplesToKeep - 1;
    m_uiBufferID = 0;

    NiColorA kTextColor(m_kColor.r, m_kColor.g, m_kColor.b, 1.0f);
    m_pkText = NiNew ScreenText(15, &kTextures, kTextColor);
    m_pkText->SetTextOrigin(uiLegendX, uiLegendY);
    m_pkText->SetVisible(true);
    m_pkText->SetString(m_acName);

    uiLegendX += m_pkText->GetWidth();
}
//---------------------------------------------------------------------------
NiVisualTracker::GraphCallbackObjectData::~GraphCallbackObjectData()
{
    NiDelete m_pkCallbackObj;
    NiDelete m_pkText;

    // Destroy the duplicated line meshes.
    for (unsigned int ui = 0; ui < BUFFER_COUNT; ui++)
        m_aspLines[ui] = 0;
}
//---------------------------------------------------------------------------
void NiVisualTracker::GraphCallbackObjectData::AdjustLegend(
    int iX, int iY)
{
    unsigned int uiXCurrent, uiYCurrent;
    m_pkText->GetTextOrigin(uiXCurrent, uiYCurrent);
    uiXCurrent += iX;
    uiYCurrent += iY;
    m_pkText->SetTextOrigin(uiXCurrent, uiYCurrent);
}
//---------------------------------------------------------------------------
unsigned int NiVisualTracker::GraphCallbackObjectData::GetLegendWidth()
{
    return m_pkText->GetWidth();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  NiVisualTracker::ScreenText implementation
//---------------------------------------------------------------------------
NiPixelDataPtr NiVisualTrackerBase::ScreenText::ms_spFontPixelData = 0;
NiTexturePtr NiVisualTrackerBase::ScreenText::ms_spTextTexture = 0;
unsigned int NiVisualTrackerBase::ScreenText::ms_uiCharWidth = 11;
unsigned int NiVisualTrackerBase::ScreenText::ms_uiCharHeight = 21;
unsigned int NiVisualTrackerBase::ScreenText::ms_uiCharSpacingX = 11;
unsigned int NiVisualTrackerBase::ScreenText::ms_uiCharSpacingY = 21;
const unsigned int NiVisualTrackerBase::ScreenText::ms_uiCharBaseU = 0;
const unsigned int NiVisualTrackerBase::ScreenText::ms_uiCharBaseV = 0;
const unsigned int NiVisualTrackerBase::ScreenText::ms_uiASCIIMin = 33;
const unsigned int NiVisualTrackerBase::ScreenText::ms_uiASCIIMax = 122;
unsigned int NiVisualTrackerBase::ScreenText::ms_uiASCIICols = 23;
int NiVisualTrackerBase::ScreenText::ms_iCount = 0;

//---------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::SetString(const char* pcString)
{
    size_t stNewLength = strlen(pcString);

    if (stNewLength >= m_uiMaxChars)
        stNewLength = m_uiMaxChars - 1;
    m_uiNumChars = (unsigned int)stNewLength + 1;

    NiStrncpy(m_pcString, m_uiMaxChars, pcString, stNewLength);
    RecreateText();
}
//---------------------------------------------------------------------------
const char* NiVisualTrackerBase::ScreenText::GetString() const
{
    return m_pcString;
}
//---------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::SetTextOrigin(unsigned int uiX, 
    unsigned int uiY)
{
    if (m_uiTextOriginX != uiX || m_uiTextOriginY != uiY)
    {
        m_uiTextOriginX = uiX;
        m_uiTextOriginY = uiY;
        RecreateText();
    }
}
//---------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::GetTextOrigin(unsigned int& uiX, 
    unsigned int& uiY) const
{
    uiX = m_uiTextOriginX;
    uiY = m_uiTextOriginY;
}
//---------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::SetScrollDown(bool bDown)
{
    if (m_bScrollDown != bDown)
    {
        m_bScrollDown = bDown;
        RecreateText();
    }
}
//---------------------------------------------------------------------------
bool NiVisualTrackerBase::ScreenText::GetScrollDown() const
{
    return m_bScrollDown;
}
//---------------------------------------------------------------------------
const NiColorA& NiVisualTrackerBase::ScreenText::GetColor() const
{
    return m_kColor;
}
//---------------------------------------------------------------------------
short NiVisualTrackerBase::ScreenText::GetHeight() const
{
    return (short)ms_uiCharSpacingY;
}
//---------------------------------------------------------------------------
short NiVisualTrackerBase::ScreenText::GetWidth() const
{
    return (short)(ms_uiCharSpacingX * m_uiNumChars);
}
//---------------------------------------------------------------------------
short NiVisualTrackerBase::ScreenText::GetCharacterSpacing() const
{
    return (short)ms_uiCharSpacingX;
}
//---------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::SetMaxLineLength(unsigned int uiColumns)
{
    if (m_uiMaxCols != uiColumns)
    {
        m_uiMaxCols = uiColumns;
        RecreateText();
    }
}
//---------------------------------------------------------------------------
unsigned int NiVisualTrackerBase::ScreenText::GetMaxLineLength() const
{
    return m_uiMaxCols;
}
//---------------------------------------------------------------------------
NiTexture* NiVisualTrackerBase::ScreenText::GetTexture()
{
    return ms_spTextTexture;
}
//---------------------------------------------------------------------------
NiVisualTrackerBase::ScreenText::ScreenText(unsigned int uiMaxChars,
    NiMeshScreenElementsArray* pkScreenTextures, const NiColorA& kColor)
{
    Init(uiMaxChars, pkScreenTextures, kColor);
}
//---------------------------------------------------------------------------
NiVisualTrackerBase::ScreenText::~ScreenText()
{
    m_pkScreenElements->Remove(m_spScreenElement);
    m_pkScreenElements->Compact();

    if (--ms_iCount < 1)
    {
        ms_spTextTexture = 0;
        ms_spFontPixelData = 0;
    }

    NiFree(m_pcString);
}
//---------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::Init(unsigned int uiMaxChars,
    NiMeshScreenElementsArray* pkScreenTextures, const NiColorA& kColor)
{
    ms_iCount++;

    if (!ms_spTextTexture)
    {
        ms_spTextTexture = NiSourceTexture::Create(
            CreateCourier16PixelData());
    }

    m_uiMaxChars = uiMaxChars;
    m_kColor = kColor;
    
    m_spScreenElement = NiNew NiMeshScreenElements(false, true, 1);

    NiTexturingProperty* pkTexProp = NiNew NiTexturingProperty;
    pkTexProp->SetBaseTexture(ms_spTextTexture);
    pkTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
    pkTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
    pkTexProp->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);

    NiAlphaProperty* pkAlphaProp = NiNew NiAlphaProperty;
    pkAlphaProp->SetAlphaBlending(true);

    NiZBufferProperty* pkZBufProp = NiNew NiZBufferProperty;
    pkZBufProp->SetZBufferTest(false);
    pkZBufProp->SetZBufferWrite(true);

    NiMaterialProperty* pkMatProp = NiNew NiMaterialProperty;
    pkMatProp->SetEmittance(NiColor(m_kColor.r, m_kColor.g, m_kColor.b));

    m_spScreenElement->AttachProperty(pkTexProp);
    m_spScreenElement->AttachProperty(pkAlphaProp);
    m_spScreenElement->AttachProperty(pkZBufProp);
    m_spScreenElement->AttachProperty(pkMatProp);
    m_spScreenElement->UpdateProperties();

    m_pkScreenElements = pkScreenTextures;

    m_uiTextOriginX = 0;
    m_uiTextOriginY = 0;
    m_bScrollDown = true;

    // String is _not_ NULL terminated
    m_pcString = NiAlloc(char, m_uiMaxChars);
    m_uiNumChars = 0;

    m_uiMaxCols = 40;

    m_uiNumRects = 0;
    m_uiNumCurrentRows = 0;
    m_uiCurrentColumn = 0;
}
//---------------------------------------------------------------------------
NiPixelData* NiVisualTrackerBase::ScreenText::CreateCourier16PixelData()
{
   if (ms_spFontPixelData != NULL)
       return ms_spFontPixelData;
   
   ms_spFontPixelData = NiNew NiPixelData(256, 128, NiPixelFormat::RGBA32);
   NIASSERT(ms_spFontPixelData);
   unsigned char* pucPixels = ms_spFontPixelData->GetPixels();
   for (unsigned int ui = 0; ui < ms_spFontPixelData->GetSizeInBytes(); ui++)
   {
        pucPixels[ui] = g_aucPixels[ui];
   }

   return ms_spFontPixelData;
}
//---------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::RecreateText()
{
    m_spScreenElement->RemoveAll();

    m_uiNumCurrentRows = 0;
    m_uiCurrentColumn = 0;

    NiRendererPtr spRenderer = NiRenderer::GetRenderer();

    for (unsigned int i = 0; i < m_uiNumChars; i++)
    {
        char cChar = m_pcString[i];

        // if we are at the end of a line or if the char is '\n' then move
        // to the start of the next line
        if ((m_uiCurrentColumn >= m_uiMaxCols) || (cChar == '\n'))
        {
            m_uiNumCurrentRows++;
            m_uiCurrentColumn = 0;

            if (!m_bScrollDown)
            {
                // Move all characters up one row
                unsigned int uiNumPolys = 
                    m_spScreenElement->GetNumPolygons();
                for (unsigned int j = 0; j < uiNumPolys; j++)
                {
                    float fLeft, fTop, fWidth, fHeight;
                    m_spScreenElement->GetRectangle(j,
                        fLeft, fTop, fWidth, fHeight);
                    fTop -= ms_uiCharSpacingY;
                    m_spScreenElement->SetRectangle(j,
                        fLeft, fTop, fWidth, fHeight);
                }
            }

            // if the extra char is a '\n', skip it
            if (cChar == '\n')
                continue;
        }

        unsigned int uiChar = (unsigned int)cChar;

        float fCharWidth, fCharHeight;
        spRenderer->ConvertFromPixelsToNDC(
            ms_uiCharSpacingX, ms_uiCharSpacingY,
            fCharWidth, fCharHeight);

        // skip whitespace or unprintable character
        if ((uiChar >= ms_uiASCIIMin) && (uiChar <= ms_uiASCIIMax))
        {
            uiChar -= ms_uiASCIIMin;
    
            unsigned int usPixTop = m_uiTextOriginY;
            if (m_bScrollDown)
                usPixTop += (m_uiNumCurrentRows) * ms_uiCharSpacingY;
            unsigned short usPixLeft = (unsigned short)(m_uiTextOriginX + 
                m_uiCurrentColumn * ms_uiCharSpacingX);

            unsigned short usTexTop = (unsigned short)(ms_uiCharBaseV + 
                (uiChar / ms_uiASCIICols) * ms_uiCharSpacingY);
            unsigned short usTexLeft = (unsigned short)(ms_uiCharBaseU + 
                (uiChar % ms_uiASCIICols) * ms_uiCharSpacingX);

            float fLeft, fTop;
            spRenderer->ConvertFromPixelsToNDC(usPixLeft, usPixTop,
                fLeft, fTop);

            float fTexLeft = (float) usTexLeft / ms_spTextTexture->GetWidth();
            float fTexTop = (float) usTexTop / ms_spTextTexture->GetHeight();
            float fTexRight = fTexLeft + 
                ((float) ms_uiCharWidth / ms_spTextTexture->GetWidth());
            float fTexBottom = fTexTop + 
                ((float) ms_uiCharHeight / ms_spTextTexture->GetHeight());

            NiInt32 iPolyIndex = m_spScreenElement->Insert(4);
            m_spScreenElement->SetRectangle(iPolyIndex, 
                fLeft, fTop, fCharWidth, fCharHeight);

            m_spScreenElement->SetTextures(iPolyIndex, 0, 
                fTexLeft, fTexTop, fTexRight, fTexBottom);

            m_spScreenElement->UpdateBound();
            m_spScreenElement->SetColors(iPolyIndex, m_kColor);
        }

        m_uiCurrentColumn++;
    }
}
//---------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::AppendCharacter(char cChar)
{
    if (m_uiNumChars >= (m_uiMaxChars - 1))
        return;
    
    m_pcString[m_uiNumChars] = cChar;

    m_uiNumChars++;
    m_pcString[m_uiNumChars] = '\0';

    // if we are at the end of a line or if the char is '\n' then move
    // to the start of the next line
    if ((m_uiCurrentColumn >= m_uiMaxCols) || (cChar == '\n'))
    {
        m_uiNumCurrentRows++;
        m_uiCurrentColumn = 0;

        if (!m_bScrollDown)
        {
            // Move all characters up one row
            unsigned int uiNumPolys = 
                m_spScreenElement->GetNumPolygons();
            for (unsigned int j = 0; j < uiNumPolys; j++)
            {
                float fLeft, fTop, fWidth, fHeight;
                m_spScreenElement->GetRectangle(j,
                    fLeft, fTop, fWidth, fHeight);
                fTop -= ms_uiCharSpacingY;
                m_spScreenElement->SetRectangle(j,
                    fLeft, fTop, fWidth, fHeight);
            }
        }

        // if the extra char is a '\n', skip it
        if (cChar == '\n')
            return;
    }

    unsigned int uiChar = (unsigned int)cChar;

    // skip whitespace or unprintable character
    if ((uiChar >= ms_uiASCIIMin) && (uiChar <= ms_uiASCIIMax))
    {
        uiChar -= ms_uiASCIIMin;
    
        unsigned int usPixTop = m_uiTextOriginY;
        if (m_bScrollDown)
            usPixTop += (m_uiNumCurrentRows + 1) * ms_uiCharSpacingY;
        unsigned short usPixLeft = (unsigned short)(m_uiTextOriginX + 
            m_uiCurrentColumn * ms_uiCharSpacingX);

        unsigned short usTexTop = (unsigned short)(ms_uiCharBaseV + 
            (uiChar / ms_uiASCIICols) * ms_uiCharSpacingY);
        unsigned short usTexLeft = (unsigned short)(ms_uiCharBaseU + 
            (uiChar % ms_uiASCIICols) * ms_uiCharSpacingX);

        NiRendererPtr spRenderer = NiRenderer::GetRenderer();
        float fCharWidth, fCharHeight;
        spRenderer->ConvertFromPixelsToNDC(
            ms_uiCharSpacingX, ms_uiCharSpacingY,
            fCharWidth, fCharHeight);

        float fLeft, fTop;
        spRenderer->ConvertFromPixelsToNDC(usPixLeft, usPixTop,
            fLeft, fTop);

        float fTexLeft = (float) usTexLeft / ms_spTextTexture->GetWidth();
        float fTexTop = (float) usTexTop / ms_spTextTexture->GetHeight();
        float fTexRight = fTexLeft + 
            ((float) ms_uiCharWidth / ms_spTextTexture->GetWidth());
        float fTexBottom = fTexTop + 
            ((float) ms_uiCharHeight / ms_spTextTexture->GetHeight());

        NiInt32 iPolyIndex = m_spScreenElement->Insert(4);
        m_spScreenElement->SetRectangle(iPolyIndex, 
            fLeft, fTop, fCharWidth, fCharHeight);

        m_spScreenElement->SetTextures(iPolyIndex, 0, 
            fTexLeft, fTexTop, fTexRight, fTexBottom);

        m_spScreenElement->UpdateBound();
        m_spScreenElement->SetColors(iPolyIndex, m_kColor);
    }

    m_uiCurrentColumn++;
}
//---------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::DeleteLastCharacter()
{
    if (!m_uiNumChars)
        return;

    m_uiNumChars--;
    char cChar = m_pcString[m_uiNumChars];
    m_pcString[m_uiNumChars] = '\0';

    // if we are at the beginning of a line then return to end of the 
    // previous line - easiest way to do this is to recreate the text
    if (m_uiCurrentColumn == 0)
    {
        RecreateText();
    }
    else
    {
        unsigned int uiChar = (unsigned int)cChar;

        // skip whitespace or unprintable character
        if ((uiChar >= ms_uiASCIIMin) && (uiChar <= ms_uiASCIIMax))
        {
            // Remove character
            m_spScreenElement->Remove(m_spScreenElement->GetNumPolygons() - 1);
        }
        m_uiCurrentColumn--;
    }
}
//---------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::SetColor(NiColorA& kNewColor)
{
    if (kNewColor == m_kColor)
        return;

    m_kColor = kNewColor;

    NiMaterialProperty* pkMatProp = NiDynamicCast(NiMaterialProperty,
        m_spScreenElement->GetProperty(NiProperty::MATERIAL));
    pkMatProp->SetEmittance(NiColor(m_kColor.r, m_kColor.g, m_kColor.b));
    m_spScreenElement->UpdateProperties();
}
//---------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::SetVisible(bool bVisible)
{
    if (bVisible)
        m_pkScreenElements->AddFirstEmpty(m_spScreenElement);
    else
        m_pkScreenElements->Remove(m_spScreenElement);
}
//---------------------------------------------------------------------------

