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

#include "TerrainPluginPCH.h"

#include <NiRay.h>

#include "MBrushTypeCircle.h"
#include "MBrushElement.h"
#include "MTerrainPlugin.h"
#include <NiMesh.h>
#include <NiMeshLib.h>
#include <NiToolDataStream.h>

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

#define ENSURE_GIZMO_LOADED() if (!m_pkBrushOverlay) \
    { \
        if (SetupBrushOverlay() == false) \
        { \
        } \
    }

MBrushTypeCircle::MBrushTypeCircle()
{
    m_pkBrushOverlay = 0;
    
    m_fSizeInner = 10.0f;
    m_fSizeInnerSqr = 100.0f;
    m_fSizeOuter = 12.0f;
    m_fSizeOuterSqr = 144.0f;
    m_bUseLinearFallOff = true;
    m_pmBrushTypeName = S"Circle";
}
//---------------------------------------------------------------------------
MBrushTypeCircle::MBrushTypeCircle(MBrushTypeCircle* pmCopy)
{
    this->m_fSizeInner = pmCopy->m_fSizeInner;
    this->m_fSizeInnerSqr = pmCopy->m_fSizeInnerSqr;
    this->m_fSizeOuter = pmCopy->m_fSizeOuter;
    this->m_fSizeOuterSqr = pmCopy->m_fSizeOuterSqr;
    this->m_bUseLinearFallOff = pmCopy->m_bUseLinearFallOff;
    m_pmBrushTypeName = S"Circle";
}
//---------------------------------------------------------------------------
MBrushType* MBrushTypeCircle::CloneBrushType()
{
    MBrushType* pmToReturn = new MBrushTypeCircle(this);
    return pmToReturn;
}
//---------------------------------------------------------------------------
void MBrushTypeCircle::CategoriseVerticies(
    NiPoint3& kIntersection, 
    unsigned int uiNumVerticies, 
    NiDeformablePointSetPtr spActivePoints, 
    NiBrushPassPointInfo* pkCategorisedPoints, 
    MFalloff* pmFalloffFunction, 
    NiTerrainInteractor* pkTerrainComponent,
    float fTerrainScale)
{
    NI_UNUSED_ARG(pkTerrainComponent);
    // Empty out the contents of previously categorized vertices:
    pkCategorisedPoints->m_kInnerPoints.RemoveAll();
    pkCategorisedPoints->m_kOuterPoints.RemoveAll();

    float fSizeInner = m_fSizeInner * fTerrainScale;
    float fSizeOuter = m_fSizeOuter * fTerrainScale;

    // Do some pre-calculation
    float fInnerRadiusSqr = NiSqr(fSizeInner);
    float fOuterRadiusSqr = NiSqr(fSizeOuter);
    float fInvRadDiff = (fSizeOuter != fSizeInner) ? 
        1.0f / (fSizeOuter - fSizeInner) : 1000.0f;

    NiDeformablePoint* pkPickWTV;
    NiBrushPassPointInfo::FalloffData kFalloffData;

    // Count how many fall off points
    NiUInt32 uiOuterPointCount = 0;
    NiUInt32 uiInnerPointCount = 0;
    for (unsigned int ui = 0; ui < uiNumVerticies; ui++)
    {
        pkPickWTV = spActivePoints->GetAt(ui);

        float fPointRadius = pkPickWTV->GetRadius() * fTerrainScale;
        float fPointRadiusSqr = fPointRadius * fPointRadius;

        float fDistSqr = 
            NiSqr(pkPickWTV->GetWorldLocation().x - kIntersection.x) + 
            NiSqr(pkPickWTV->GetWorldLocation().y - kIntersection.y);

        if (fDistSqr > (fInnerRadiusSqr + fPointRadiusSqr))
        {
            if (fDistSqr <= (fOuterRadiusSqr + fPointRadiusSqr))
            {
                uiOuterPointCount++;
            }
        }
        else
        {
            uiInnerPointCount++;
        }
    }

    // Preallocate the data.
    pkCategorisedPoints->m_kOuterPoints.SetSize(uiOuterPointCount);
    pkCategorisedPoints->m_kInnerPoints.SetSize(uiInnerPointCount);

    // Categories all the vertices that have been picked
    for (unsigned int ui = 0; ui < uiNumVerticies; ui++)
    {
        pkPickWTV = spActivePoints->GetAt(ui);

        float fPointRadius = pkPickWTV->GetRadius() * fTerrainScale;
        float fPointRadiusSqr = fPointRadius * fPointRadius;

        float fDistSqr = 
            NiSqr(pkPickWTV->GetWorldLocation().x - kIntersection.x) + 
            NiSqr(pkPickWTV->GetWorldLocation().y - kIntersection.y);

        if (fDistSqr > (fInnerRadiusSqr + fPointRadiusSqr))
        {
            if (fDistSqr <= (fOuterRadiusSqr + fPointRadiusSqr))
            {
                float fDist = NiSqrt(fDistSqr);

                // Add an outer radius falloff point
                kFalloffData.m_pkActual = pkPickWTV;              

                kFalloffData.m_fInterpolatedModifier = 
                    pmFalloffFunction->CalcFalloff((fSizeOuter - fDist)
                    * fInvRadDiff);

                kFalloffData.m_fInterpolatedModifier = 
                    NiMax(kFalloffData.m_fInterpolatedModifier, 0.0f);
                kFalloffData.m_fInterpolatedModifier = 
                    NiMin(kFalloffData.m_fInterpolatedModifier, 1.0f);

                pkCategorisedPoints->m_kOuterPoints.Add(kFalloffData);
                NIASSERT(kFalloffData.m_pkActual);
            }
        }
        else
        {
            // Add a regular inner radius point
            pkCategorisedPoints->m_kInnerPoints.Add(pkPickWTV);
        }
    }
}
//---------------------------------------------------------------------------
void MBrushTypeCircle::CalculateBoundaryValues(
    NiPoint3 kIntersection, float, float fOuterRadius,
    NiBrushPassPointInfo* pkPassPointInfo, 
    NiTerrainInteractor* pkTerrainComponent,
    MBrushElement* pmActiveElement)
{
    unsigned int uiNumOuterPoints = pkPassPointInfo->m_kOuterPoints.GetSize();
    NiPoint3 kDirection, kInnerIntersection, kOuterIntersection;
    NiDeformablePoint* pkPoint;
    NiBrushPassPointInfo::FalloffData kFalloffData;

    for (unsigned int ui = 0; ui < uiNumOuterPoints; ++ui)
    {
        kFalloffData = pkPassPointInfo->m_kOuterPoints.GetAt(ui);
        pkPoint = kFalloffData.m_pkActual;

        // Calculate the normalised direction of the outer point, from the 
        // center intersection. Only working on 2D plane - ignoring Z
        kDirection = pkPoint->GetWorldLocation() - kIntersection;
        kDirection.z = 0.0f;
        kDirection.Unitize();

        // From the direction, work out where the intersection on the outer 
        // radii lies
        kOuterIntersection = kIntersection + kDirection * fOuterRadius;

        // Now get the value at that point
        kFalloffData.m_fInnerBoundaryValue = pmActiveElement->GetValue(
            kFalloffData.m_pkInnerBoundaryPoint
            );

        kFalloffData.m_fOuterBoundaryValue = pmActiveElement->GetValue(
            pkTerrainComponent, kOuterIntersection
            );

        pkPassPointInfo->m_kOuterPoints.SetAt(ui, kFalloffData);
    }
}
//---------------------------------------------------------------------------
NiBool MBrushTypeCircle::GetBoundaryIntersection( 
    NiPoint3& kBoundaryIntersection,
    NiTerrainInteractor* pkTerrainComponent, 
    float& fIntersectionHeight
    )
{
    NiRay kRay;
    NiBool bTerrain;
    NiPoint3 kHelperInter, kIntersectionNormal;
    NiPoint3 kDir(0.0, 0.0, -1);
    
    kBoundaryIntersection.z += 10000.0f;
    kRay.SetRay(kBoundaryIntersection, kDir, 0.0f);

    bTerrain = pkTerrainComponent->Collide(kRay);

    if (bTerrain)
    {
        kRay.GetIntersection(kHelperInter, kIntersectionNormal);
        fIntersectionHeight = kHelperInter.z;
    }

    return bTerrain;
}
//---------------------------------------------------------------------------
bool MBrushTypeCircle::SetupBrushOverlay()
{   
    m_pkBrushOverlay = NiNew NiMesh();
    m_pkBrushOverlay->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);
    m_pkBrushOverlay->SetName("Brush");
    m_pkBrushOverlay->SetTranslate(0,0,0);
       
    // Create the element reference.
    NiDataStreamElementLock kPosLock = m_pkBrushOverlay->AddStreamGetLock(
        NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, 
        ms_usVertices,
        NiDataStream::ACCESS_CPU_READ | 
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_VERTEX, false, true
        );

    NIASSERT(kPosLock.count() == ms_usVertices);

    NiTStridedRandomAccessIterator<NiPoint3> kPointsIter = 
        kPosLock.begin<NiPoint3>();

    NiDataStreamElementLock kIndLock = m_pkBrushOverlay->AddStreamGetLock(
        NiCommonSemantics::INDEX(), 0,
        NiDataStreamElement::F_UINT16_1, 
        2 * (ms_usInnerSegments + ms_usOuterSegments),
        NiDataStream::ACCESS_CPU_READ | 
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX_INDEX, false, true
        );

    NIASSERT((int)(kIndLock.count()) == 
        2 * (NiUInt32)(ms_usInnerSegments + ms_usOuterSegments));

    NiTStridedRandomAccessIterator<NiUInt16> kIndIter = 
        kIndLock.begin<NiUInt16>();

    // Create the color stream
    NiDataStreamElementLock kColorLock = m_pkBrushOverlay->AddStreamGetLock(
        NiCommonSemantics::COLOR(), 0, 
        NiDataStreamElement::F_NORMUINT8_4, 
        ms_usVertices, 
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, false, true);
    
    NIASSERT(kColorLock.count() == ms_usVertices);

    NiTStridedRandomAccessIterator<NiRGBA> kColorsIter = 
        kColorLock.begin<NiRGBA>();


    float fOuterAngle;
    int i = -1;
    int iIndex = 0;
    for (NiUInt32 ui = 0; ui < ms_usVertices; ++ui)
    {
        i++;

        if(iIndex < 2 * (ms_usInnerSegments + ms_usOuterSegments))
        {// creates the needed indexes
            if(i != ms_usInnerSegments && i != (ms_usVertices - 1))
            { //the lines from the inner to outer circles and end to beginning 
                // should not be indexed
                kIndIter[iIndex] = (NiUInt16)ui;
                iIndex++;
                kIndIter[iIndex] = (NiUInt16)(ui+1);
                iIndex++;
            }        
        }

        if (i <= ms_usInnerSegments)
        { // inner circle
            m_pkBaseColor->GetAs(kColorsIter[ui]);
            kPointsIter[ui] = NiPoint3(m_fSizeInner*NiCos((float)i/float
                (ms_usInnerSegments)*NI_TWO_PI), 
                m_fSizeInner*NiSin((float)i /
                float(ms_usInnerSegments)*NI_TWO_PI),
                0);
                        
        }
        else
        { // outer circle
            NiRGBA kTemp;
            m_pkBaseColor->GetAs(kTemp);
            kColorsIter[ui] = kTemp / 2.0f;  // Brown    

            fOuterAngle = ((float)i - (ms_usInnerSegments + 1.0f))/(float)
                ms_usOuterSegments*NI_TWO_PI;
                        
            kPointsIter[ui] = NiPoint3(m_fSizeOuter*NiCos(fOuterAngle), 
                m_fSizeOuter*NiSin(fOuterAngle), 0);
        }
    }

    kPosLock.Unlock();
    kIndLock.Unlock();
    kColorLock.Unlock();
   
    // Set the bound
    NiBound kBound;
    kBound.SetCenterAndRadius(NiPoint3(0,0,0), m_fSizeOuter);
    m_pkBrushOverlay->SetModelBound(kBound); 

    m_pkBrushOverlay->SetSubmeshCount(1);

    
    NiVertexColorProperty *pkVertexColorProperty = 
        NiNew NiVertexColorProperty();
    pkVertexColorProperty->SetSourceMode
        (NiVertexColorProperty::SOURCE_EMISSIVE);
    pkVertexColorProperty->SetLightingMode
        (NiVertexColorProperty::LIGHTING_E);
    m_pkBrushOverlay->AttachProperty(pkVertexColorProperty);

    m_pkBrushOverlay->UpdateProperties();
    m_pkBrushOverlay->UpdateEffects();
    m_pkBrushOverlay->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_pkBrushOverlay);
    return true;
}
//---------------------------------------------------------------------------
void MBrushTypeCircle::UpdateBrushOverlay(
    NiPoint3& kCenter, 
    NiTerrainInteractor* pkTerrainComponent, 
    NiBrushPassPointInfo*)
{
    ENSURE_GIZMO_LOADED();
    if (!m_pkBrushOverlay)
        return;

    // Make sure the overlay has the right scale
    float fScale = MTerrainPlugin::GetInstance()->GetBrush()->GetScale();
    ResizeBrushOverlay(fScale);

    // We first translate the brush so the bound gets updated
    m_pkBrushOverlay->SetTranslate(kCenter);
    
    NiDataStreamElementLock kPosLock(m_pkBrushOverlay, 
        NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_WRITE | 
        NiDataStream::LOCK_READ);

    NIASSERT(kPosLock.IsLocked()); 
    NiTStridedRandomAccessIterator<NiPoint3> kPointsIter = 
        kPosLock.begin<NiPoint3>();

    
    // We now need to retrieve the vertices and transform them in order to get 
    // the boundaries' height so the overlay follows the terrain
    float fTerrainIntersection = 0;
    NiPoint3 kOverlayPoint;
    for (NiUInt32 ui = 0; ui < ms_usVertices; ++ui)
    {
        kOverlayPoint = kPointsIter[ui];
        kOverlayPoint += kCenter;

        // Working the height out
        if (GetBoundaryIntersection(
                kOverlayPoint, pkTerrainComponent, fTerrainIntersection
                )
            ) 
        {
            // Modify the height in accordance to the translation made
            // so we don't translate the mesh on z twice.
            kPointsIter[ui].z = fTerrainIntersection - kCenter.z;
        }
       
    }
    kPosLock.Unlock();    
    
    m_pkBrushOverlay->Update(0.0f);
   
}
//---------------------------------------------------------------------------
void MBrushTypeCircle::GetBrushOptionPanel(
    System::Windows::Forms::Panel* pmPanel)
{
    System::Windows::Forms::Label*  pmLabel1 = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::RadioButton*  pmFalloffTypeSin = 
        new System::Windows::Forms::RadioButton();
    System::Windows::Forms::RadioButton*  pmFalloffTypeLinear = 
        new System::Windows::Forms::RadioButton();    
    System::Windows::Forms::Label*  pmLabel2 = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::NumericUpDown*  pmBrushSizeOuter = 
        new System::Windows::Forms::NumericUpDown();
    System::Windows::Forms::TrackBar*  pmSliderOuterRadius = 
        new System::Windows::Forms::TrackBar();    
    System::Windows::Forms::Label*  pmLabel3 = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::NumericUpDown*  pmBrushSizeInner = 
        new System::Windows::Forms::NumericUpDown();
    System::Windows::Forms::TrackBar*  pmSliderInnerRadius = 
        new System::Windows::Forms::TrackBar();    

    int iNextPositionX = 0;
    int iNextPositionY = 2;

    pmLabel1->AutoSize = false;
    pmLabel1->Location = System::Drawing::Point(iNextPositionX,
        iNextPositionY);
    pmLabel1->Size = System::Drawing::Size(80, 13);
    pmLabel1->Text = S"Falloff";

    iNextPositionX += pmLabel1->Width + 5;
    
    pmFalloffTypeSin->AutoSize = false;
    pmFalloffTypeSin->Location = System::Drawing::Point(iNextPositionX, 
        iNextPositionY - 2);
    pmFalloffTypeSin->Name = S"FalloffTypeSin";
    pmFalloffTypeSin->Size = System::Drawing::Size(46, 17);
    pmFalloffTypeSin->Text = S"Sine";
    pmFalloffTypeSin->Checked = !m_bUseLinearFallOff;
    pmFalloffTypeSin->UseVisualStyleBackColor = true;
    pmFalloffTypeSin->CheckedChanged += new System::EventHandler(
        this, &MBrushTypeCircle::RadioButton_CheckedChanged);

    iNextPositionX += pmFalloffTypeSin->Width + 5;
     
    pmFalloffTypeLinear->AutoSize = false;
    pmFalloffTypeLinear->Location = System::Drawing::Point(iNextPositionX, 
        iNextPositionY - 2);
    pmFalloffTypeLinear->Name = S"FalloffTypeLinear";
    pmFalloffTypeLinear->Size = System::Drawing::Size(54, 17);
    pmFalloffTypeLinear->Text = S"Linear";
    pmFalloffTypeLinear->Checked = m_bUseLinearFallOff;
    pmFalloffTypeLinear->UseVisualStyleBackColor = true;
    pmFalloffTypeLinear->CheckedChanged += new System::EventHandler(
        this, &MBrushTypeCircle::RadioButton_CheckedChanged);

    iNextPositionX = 0;
    iNextPositionY += pmFalloffTypeLinear->Height + 5;

    pmLabel3->AutoSize = false;
    pmLabel3->Location = System::Drawing::Point(iNextPositionX, 
        iNextPositionY);
    pmLabel3->Size = System::Drawing::Size(70, 25);
    pmLabel3->Text = S"Inner Radius";

    iNextPositionX += pmLabel3->Width + 5;
    
    pmSliderInnerRadius->AutoSize = false;
    pmSliderInnerRadius->Location = System::Drawing::Point(iNextPositionX, 
        iNextPositionY - 5);
    pmSliderInnerRadius->Maximum = 200;
    pmSliderInnerRadius->Minimum = 1;
    pmSliderInnerRadius->Name = S"SliderInnerRadius";
    pmSliderInnerRadius->Size = System::Drawing::Size(112, 25);
    pmSliderInnerRadius->Value = (int)(m_fSizeInner * 2);
    pmSliderInnerRadius->SmallChange = 2;
    pmSliderInnerRadius->LargeChange = 10;
    pmSliderInnerRadius->TickStyle = 
        System::Windows::Forms::TickStyle::None;
    pmSliderInnerRadius->ValueChanged += new System::EventHandler(
        this, &MBrushTypeCircle::Slider_ValueChanged);
    
    iNextPositionX += pmSliderInnerRadius->Width + 5;    
     
    pmBrushSizeInner->AutoSize = false;
    pmBrushSizeInner->DecimalPlaces = 1;
    pmBrushSizeInner->Location = System::Drawing::Point(iNextPositionX, 
        iNextPositionY - 5);
    pmBrushSizeInner->Name = S"BrushSizeInner";
    pmBrushSizeInner->Size = System::Drawing::Size(48, 20);
    pmBrushSizeInner->Minimum = 0.5;
    pmBrushSizeInner->Value = m_fSizeInner;
    pmBrushSizeInner->ValueChanged += new System::EventHandler(
        this, &MBrushTypeCircle::NumericUpDown_ValueChanged);

    iNextPositionX = 0;
    iNextPositionY += pmSliderInnerRadius->Height + 5;

    pmLabel2->AutoSize = false;
    pmLabel2->Location = System::Drawing::Point(iNextPositionX, 
        iNextPositionY);
    pmLabel2->Name = S"label6";
    pmLabel2->Size = System::Drawing::Size(70, 25);
    pmLabel2->Text = S"Outer Radius";
     
    iNextPositionX += pmLabel2->Width + 5;
     
    pmSliderOuterRadius->AutoSize = false;
    pmSliderOuterRadius->Location = System::Drawing::Point(iNextPositionX, 
        iNextPositionY - 5);
    pmSliderOuterRadius->Minimum = 1;
    pmSliderOuterRadius->Maximum = 200;
    pmSliderOuterRadius->Value = (int)(m_fSizeOuter * 2);
    pmSliderOuterRadius->Name = S"SliderOuterRadius";
    pmSliderOuterRadius->Size = System::Drawing::Size(112, 25);
    pmSliderOuterRadius->SmallChange = 10;
    pmSliderOuterRadius->LargeChange = 10;
    pmSliderOuterRadius->TickFrequency = 0;
    pmSliderOuterRadius->TickStyle = 
        System::Windows::Forms::TickStyle::None;
    pmSliderOuterRadius->ValueChanged += new System::EventHandler(
        this, &MBrushTypeCircle::Slider_ValueChanged);

    iNextPositionX += pmSliderOuterRadius->Width + 5;

    pmBrushSizeOuter->AutoSize = false;
    pmBrushSizeOuter->DecimalPlaces = 1;
    pmBrushSizeOuter->Location = System::Drawing::Point(iNextPositionX, 
        iNextPositionY - 5);
    pmBrushSizeOuter->Minimum = 0.5;
    pmBrushSizeOuter->Value = m_fSizeOuter;
    pmBrushSizeOuter->Name = S"BrushSizeOuter";
    pmBrushSizeOuter->Size = System::Drawing::Size(48, 20);
    pmBrushSizeOuter->ValueChanged += new System::EventHandler(
        this, &MBrushTypeCircle::NumericUpDown_ValueChanged);   
    
    pmPanel->Controls->Add(pmLabel1);
    pmPanel->Controls->Add(pmFalloffTypeSin);
    pmPanel->Controls->Add(pmFalloffTypeLinear);    
    pmPanel->Controls->Add(pmLabel3);
    pmPanel->Controls->Add(pmSliderInnerRadius);
    pmPanel->Controls->Add(pmBrushSizeInner);
    pmPanel->Controls->Add(pmLabel2);
    pmPanel->Controls->Add(pmSliderOuterRadius);
    pmPanel->Controls->Add(pmBrushSizeOuter);

    if (!m_bUseLinearFallOff)
    {
        MTerrainPlugin::GetInstance()->GetBrush()->
            SetSculptFalloff(MStringToCharPointer(
            MBrush::ms_pmBrushFalloffSine));
    }
    else
    {
        MTerrainPlugin::GetInstance()->GetBrush()->
            SetSculptFalloff(MStringToCharPointer(
            MBrush::ms_pmBrushFalloffLinear));
    }

}
//---------------------------------------------------------------------------
bool MBrushTypeCircle::ResizeBrushOverlay(float fCurrentScale)
{
    if (!m_pkBrushOverlay)
        return false;

    NiDataStreamElementLock kPosLock(m_pkBrushOverlay, 
        NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_WRITE);

    NIASSERT(kPosLock.IsLocked()); 
    NiTStridedRandomAccessIterator<NiPoint3> kPointsIter = 
        kPosLock.begin<NiPoint3>();

    int i = -1;
    float fOuterAngle;
    for (NiUInt32 ui = 0; ui < ms_usVertices; ++ui)
    {
        i++;
        if (i <= ms_usInnerSegments)
        { // inner circle
            kPointsIter[ui] = NiPoint3(
                fCurrentScale * m_fSizeInner * NiCos((float)i / 
                float(ms_usInnerSegments) * NI_TWO_PI), 
                fCurrentScale * m_fSizeInner * NiSin((float)i / 
                float(ms_usInnerSegments) * NI_TWO_PI),
                0.0f);
                        
        }
        else
        { // outer circle
            fOuterAngle = ((float)i - (ms_usInnerSegments + 1.0f))/(float)
                ms_usOuterSegments*NI_TWO_PI;
                        
            kPointsIter[ui] = NiPoint3(
                fCurrentScale * m_fSizeOuter * NiCos(fOuterAngle), 
                fCurrentScale * m_fSizeOuter * NiSin(fOuterAngle), 
                0.0f);
        }
    }
    
    kPosLock.Unlock();

    m_pkBrushOverlay->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_pkBrushOverlay);

    return true;
}
//---------------------------------------------------------------------------
System::Void MBrushTypeCircle::NumericUpDown_ValueChanged(
    System::Object*  sender, System::EventArgs*  e)
{
    NI_UNUSED_ARG(e);

    NumericUpDown* pmControl = dynamic_cast<NumericUpDown*>(sender);

    if (String::Compare(pmControl->Name, S"BrushSizeInner") == 0)
    {
        m_fSizeInner = (float)pmControl->Value;

        TrackBar* pmTrack = dynamic_cast<TrackBar*>
            (pmControl->Parent->Controls->get_Item(S"SliderInnerRadius"));
                  
        if (pmTrack->Value / 2.0f != m_fSizeInner && 
            pmTrack->Maximum / 2.0f >= m_fSizeInner &&
            pmTrack->Minimum / 2.0f <= m_fSizeInner)
        {
            pmTrack->Value = (int)(m_fSizeInner * 2.0f);
            if (m_fSizeInner > m_fSizeOuter)
            {
                TrackBar* pmOutTrack = dynamic_cast<TrackBar*>(pmControl->
                    Parent->Controls->get_Item(S"SliderOuterRadius"));
                pmOutTrack->Value = (int)(m_fSizeInner * 2.0f);
            }
        }        
    }
    else if (String::Compare(pmControl->Name, S"BrushSizeOuter") == 0)
    {
        m_fSizeOuter = (float)pmControl->Value;
        
        TrackBar* pmTrack = dynamic_cast<TrackBar*>
            (pmControl->Parent->Controls->get_Item(S"SliderOuterRadius"));
                  
        if (pmTrack->Value / 2.0f != m_fSizeOuter && 
            pmTrack->Maximum / 2.0f >= m_fSizeOuter &&
            pmTrack->Minimum / 2.0f <= m_fSizeOuter)
        {           
            pmTrack->Value = (int)(m_fSizeOuter * 2.0f);
            if (m_fSizeInner > m_fSizeOuter)
            {
                TrackBar* pmInTrack = dynamic_cast<TrackBar*>(pmControl->
                    Parent->Controls->get_Item(S"SliderInnerRadius"));
                pmInTrack->Value = (int)(m_fSizeOuter * 2.0f);
            }
        }
    }
    else
    {
        return;
    }
}
//---------------------------------------------------------------------------
System::Void MBrushTypeCircle::RadioButton_CheckedChanged(
    System::Object*  sender,
    System::EventArgs*  e)
{
    NI_UNUSED_ARG(e);

    RadioButton* pmRadioButton = dynamic_cast<RadioButton*>(sender);

    if (String::Compare(pmRadioButton->Name, S"FalloffTypeSin") == 0)
    {
        if (pmRadioButton->Checked)
        {
            MTerrainPlugin::GetInstance()->GetBrush()->
                SetSculptFalloff(MStringToCharPointer(
                MBrush::ms_pmBrushFalloffSine));
            m_bUseLinearFallOff = false;
        }
    }
    else if (String::Compare(pmRadioButton->Name, S"FalloffTypeLinear") == 0)
    {
        if (pmRadioButton->Checked)
        {
            MTerrainPlugin::GetInstance()->GetBrush()->
                SetSculptFalloff(MStringToCharPointer(
                MBrush::ms_pmBrushFalloffLinear));
            m_bUseLinearFallOff = true;
        }
    }
    else
    {
        return;
    }
}
//---------------------------------------------------------------------------
System::Void MBrushTypeCircle::Slider_ValueChanged(System::Object*  sender,
    System::EventArgs*  e)
{
    NI_UNUSED_ARG(e);

    TrackBar* pmTrackBar = dynamic_cast<TrackBar*>(sender);

    if (String::Compare(pmTrackBar->Name, S"SliderInnerRadius") == 0)
    {
        m_fSizeInner = (float)(pmTrackBar->Value / 2.0f);
        
        NumericUpDown* pmUpDown = dynamic_cast<NumericUpDown*>
            (pmTrackBar->Parent->Controls->get_Item(S"BrushSizeInner"));
       
        if (pmUpDown->Value != m_fSizeInner)
        {
            pmUpDown->Value = m_fSizeInner;

            if (m_fSizeInner > m_fSizeOuter)
            {
                TrackBar* pmOutTrack = dynamic_cast<TrackBar*>(pmTrackBar->
                    Parent->Controls->get_Item(S"SliderOuterRadius"));
                pmOutTrack->Value = (int)(m_fSizeInner * 2.0f);
            }
        }
    }
    else if (String::Compare(pmTrackBar->Name, S"SliderOuterRadius") == 0)
    {
        m_fSizeOuter = (float)(pmTrackBar->Value / 2.0f);
        
        NumericUpDown* pmUpDown = dynamic_cast<NumericUpDown*>
            (pmTrackBar->Parent->Controls->get_Item(S"BrushSizeOuter"));
       
        if (pmUpDown->Value != m_fSizeOuter)
        {
            pmUpDown->Value = m_fSizeOuter;
            if (m_fSizeInner > m_fSizeOuter)
            {
                TrackBar* pmInTrack = dynamic_cast<TrackBar*>(pmTrackBar->
                    Parent->Controls->get_Item(S"SliderInnerRadius"));
                pmInTrack->Value = (int)(m_fSizeOuter * 2.0f);
            }
        }
    }
    else
    {
        return;
    }

}
//---------------------------------------------------------------------------
