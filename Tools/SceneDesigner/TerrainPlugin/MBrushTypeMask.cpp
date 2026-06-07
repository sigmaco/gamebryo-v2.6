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

#include "MBrushTypeMask.h"
#include "MBrushElement.h"
#include "MTerrainPlugin.h"
#include "MImageFileNameEditor.h"
#include <NiMesh.h>
#include <NiMeshLib.h>
#include <NiToolDataStream.h>
#include <NiTexturingProperty.h>
#include <NiPalette.h>
#include <NiMath.h>

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

#define ENSURE_GIZMO_LOADED() if (!m_pkBrushOverlay) \
    { \
        if (SetupBrushOverlay() == false) \
        { \
        } \
    }

MBrushTypeMask::MBrushTypeMask() :
    m_uiNeutralColor(0),
    m_fRandomSizeRange(1.0f),
    m_fRandomRotationRange(NI_PI / 180.0f),
    m_fSideLength(15.0f),
    m_fRandomizedSideLength(15.0f),
    m_fRotationAngle(0.0f),
    m_fRandomizedRotationAngle(0.0f),
    m_bUseRandomSize(false),
    m_bUseRandomRotation(false),
    m_bUseFiltering(true),
    m_bUseInvertMask(false),
    m_bUseAlignToCamera(false)
{
    m_pkBrushOverlay = 0;
    m_pmBrushTypeName = S"Mask";       
    m_fSizeInnerSqr = 100.0f;
    m_fSizeOuterSqr = 144.0f;
    m_fSizeInner = (m_fSideLength);
    m_fSizeOuter = ((float) sqrt(2.0f) * (m_fSideLength));
    m_bUseLinearFallOff = true;    
    
    m_pmAddedItems = 0; 
    m_pkMaskPixData = 0;  
    m_pkRotation = NiNew NiMatrix3();
    m_pmMaskPath = 0;
}
//---------------------------------------------------------------------------
MBrushTypeMask::MBrushTypeMask(MBrushTypeMask* pmCopy):
    m_uiNeutralColor(pmCopy->m_uiNeutralColor),
    m_fRandomSizeRange(pmCopy->m_fRandomSizeRange),
    m_fRandomRotationRange(pmCopy->m_fRandomRotationRange),
    m_fSideLength(pmCopy->m_fSideLength),
    m_fRandomizedSideLength(pmCopy->m_fRandomizedSideLength),
    m_fRotationAngle(pmCopy->m_fRotationAngle),
    m_fRandomizedRotationAngle(pmCopy->m_fRandomizedRotationAngle),
    m_bUseRandomSize(pmCopy->m_bUseRandomSize),
    m_bUseRandomRotation(pmCopy->m_bUseRandomRotation),
    m_bUseFiltering(pmCopy->m_bUseFiltering),
    m_bUseInvertMask(pmCopy->m_bUseInvertMask),
    m_bUseAlignToCamera(pmCopy->m_bUseAlignToCamera)
{   
    this->m_fSizeInner = pmCopy->m_fSizeInner;
    this->m_fSizeInnerSqr = pmCopy->m_fSizeInnerSqr;
    this->m_fSizeOuter = pmCopy->m_fSizeOuter;
    this->m_fSizeOuterSqr = pmCopy->m_fSizeOuterSqr;
    this->m_bUseLinearFallOff = pmCopy->m_bUseLinearFallOff;
            
    m_pmBrushTypeName = S"Mask";    
    m_pmAddedItems = 0;
    
    // We need to copy a refference of the pixel data
    m_pkMaskPixData = pmCopy->m_pkMaskPixData;
    m_pkMaskPixData->IncRefCount();

    // We need to keep a copy of the rotation matrix
    NiPoint3 kCol0;
    NiPoint3 kCol1;
    NiPoint3 kCol2;
    pmCopy->m_pkRotation->GetCol(0, kCol0);
    pmCopy->m_pkRotation->GetCol(1, kCol1);
    pmCopy->m_pkRotation->GetCol(2, kCol2);
    this->m_pkRotation = NiNew NiMatrix3(kCol0, kCol1, kCol2); 

    m_pmMaskPath = pmCopy->m_pmMaskPath;    
}
//---------------------------------------------------------------------------
void MBrushTypeMask::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        if (m_pkMaskPixData)
        {
            m_pkMaskPixData->DecRefCount();
            m_pkMaskPixData = 0;
        }
        
        MBrushType::Do_Dispose(bDisposing);
    }

    NiDelete m_pkRotation;
}
//---------------------------------------------------------------------------
MBrushType* MBrushTypeMask::CloneBrushType()
{
    MBrushType* pmToReturn = new MBrushTypeMask(this);
    return pmToReturn;
}
//---------------------------------------------------------------------------
void MBrushTypeMask::CategoriseVerticies(
    NiPoint3& kIntersection, 
    unsigned int uiNumVerticies, 
    NiDeformablePointSetPtr spActivePoints, 
    NiBrushPassPointInfo* pkCategorisedPoints, 
    MFalloff* pmFalloffFunction, 
    NiTerrainInteractor* pkTerrainComponent,
    float fTerrainScale)
{
    NI_UNUSED_ARG(pkTerrainComponent);
    NI_UNUSED_ARG(pmFalloffFunction);

    if (!m_pkMaskPixData)
        return;

    // Empty out the contents of previously categorized vertices.
    pkCategorisedPoints->m_kInnerPoints.RemoveAll();
    pkCategorisedPoints->m_kOuterPoints.RemoveAll();

    float fSizeInner = m_fSizeInner * fTerrainScale;
   
    NiDeformablePoint* pkPickWTV;
    NiBrushPassPointInfo::FalloffData kFalloffData;

    // Count how many fall off points.
    NiUInt32 uiOuterPointCount = uiNumVerticies;
    NiUInt32 uiInnerPointCount = uiNumVerticies;

    // Preallocate the data.
    pkCategorisedPoints->m_kOuterPoints.SetSize(uiOuterPointCount);
    pkCategorisedPoints->m_kInnerPoints.SetSize(uiInnerPointCount);

    NiUInt32 uiHeight = m_pkMaskPixData->GetHeight();
    NiUInt32 uiStride = m_pkMaskPixData->GetPixelStride();
    
    // Categories all the vertices that have been picked.
    for (unsigned int ui = 0; ui < uiNumVerticies; ui++)
    {
        pkPickWTV = spActivePoints->GetAt(ui);
        float fValX = pkPickWTV->GetWorldLocation().x - kIntersection.x;
        float fValY = pkPickWTV->GetWorldLocation().y - kIntersection.y;

        // We now transform the vertices into the brush space 
        NiPoint3 kPoint = NiPoint3(fValX, fValY, 0.0f);
        NiMatrix3 kRotMat = m_pkRotation->Inverse();        
        kPoint = kRotMat * kPoint;

        fValX = kPoint.x;
        fValY = kPoint.y;

        unsigned char* pucPix = m_pkMaskPixData->GetPixels();
        float fFalloff = 0;                
        if (NiAbs(fValX) < fSizeInner)
        {
            if (NiAbs(fValY) < fSizeInner)
            {
                // Convert Points to choose the correct Height Map Values. 
                // using a bilinear filtering technique
                float fXPercent = (fValX + fSizeInner) / (fSizeInner * 2.0f);
                float fYPercent = (fValY + fSizeInner) / (fSizeInner * 2.0f);
                fYPercent = 1.0f - fYPercent;
                float fReadHeightX = fXPercent * (uiHeight - 1);
                float fReadHeightY = fYPercent * (uiHeight - 1);       
                
                NiUInt32 uiX = (NiUInt32)NiFloor(fReadHeightX);
                NiUInt32 uiY = (NiUInt32)NiFloor(fReadHeightY);
                float fRatioX = fReadHeightX - uiX;
                float fRatioY = fReadHeightY - uiY;
                float fOppositeX = 1 - fRatioX;
                float fOppositeY = 1 - fRatioY;
                
                NiUInt32 uiPixelOffset = (NiUInt32)(uiX + 
                    uiY * (uiHeight) ) * uiStride;

                unsigned char ucFiltered;
                                               
                if (m_bUseFiltering)
                {
                    // Apply the filter
                    ucFiltered = (unsigned char)(fOppositeY * (
                    pucPix[uiPixelOffset] * fOppositeX + 
                    pucPix[uiPixelOffset + uiStride] * fRatioX) + 
                    fRatioY * (pucPix[uiPixelOffset + uiHeight * uiStride] *
                    fOppositeX + fRatioX *
                    pucPix[uiPixelOffset + (uiHeight + 1) * uiStride]));
                }
                else
                {
                    ucFiltered = pucPix[uiPixelOffset];
                }
                                                                               
                // If Height Map Value is white, add point to list.
                if (ucFiltered == 255 + m_uiNeutralColor)
                {
                    // We only do inner points when the neutral color is 0
                    // in all other cases we points are fall off points as they
                    // will be modified by a multiplier
                    pkCategorisedPoints->m_kInnerPoints.Add(pkPickWTV);
                }
                else
                {                   
                    // Add a falloff point. Value depends on grayscale value 
                    // and the set neutral color
                    fFalloff = ucFiltered / 255.0f - m_uiNeutralColor / 255.0f;
                    kFalloffData.m_pkActual = pkPickWTV; 
                    kFalloffData.m_fInterpolatedModifier = fFalloff;
                    pkCategorisedPoints->m_kOuterPoints.Add(kFalloffData);
                    NIASSERT(kFalloffData.m_pkActual);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void MBrushTypeMask::CalculateBoundaryValues(
    NiPoint3 kIntersection, float, float fOuterRadius,
    NiBrushPassPointInfo* pkPassPointInfo, 
    NiTerrainInteractor* pkTerrainComponent,
    MBrushElement* pmActiveElement)
{
    NiUInt32 uiNumOuterPoints = pkPassPointInfo->m_kOuterPoints.GetSize();
    NiPoint3 kDirection, kInnerIntersection, kOuterIntersection;
    NiDeformablePoint* pkPoint;
    NiBrushPassPointInfo::FalloffData kFalloffData;

    for (NiUInt32 ui = 0; ui < uiNumOuterPoints; ++ui)
    {
        kFalloffData = pkPassPointInfo->m_kOuterPoints.GetAt(ui);
        pkPoint = kFalloffData.m_pkActual;

        // Calculate the normalised direction of the outer point, from the 
        // center intersection. Only working on 2D plane - ignoring Z.
        kDirection = pkPoint->GetWorldLocation() - kIntersection;
        kDirection.z = 0.0f;
        kDirection.Unitize();

        // From the direction, work out where the intersection on the outer 
        // radii lies.
        kOuterIntersection = kIntersection + kDirection * fOuterRadius;

        // Now get the value at that point.
        kFalloffData.m_fInnerBoundaryValue = pmActiveElement->GetValue(
            kFalloffData.m_pkInnerBoundaryPoint);

        kFalloffData.m_fOuterBoundaryValue = pmActiveElement->GetValue(
            pkTerrainComponent, kOuterIntersection);

        pkPassPointInfo->m_kOuterPoints.SetAt(ui, kFalloffData);
    }
}
//---------------------------------------------------------------------------
NiBool MBrushTypeMask::GetBoundaryIntersection( 
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
void MBrushTypeMask::AddItem(System::String* pmFileName, Control* pmControl)
{
    ComboBox* pmHeightMapPath = dynamic_cast<ComboBox*>(pmControl->
        Parent->Controls->get_Item(S"MaskPath"));

    // Make sure the filename is an absolute path
    if (NiPath::IsRelative(MStringToCharPointer(pmFileName)))
    {
        char pcPath[512];
        NiPath::ConvertToAbsolute(pcPath, 512, 
            MStringToCharPointer(pmFileName), NULL);
        m_pmMaskPath = pcPath;
    }
    else
    {
        m_pmMaskPath = pmFileName;
    }

    PictureBox* pmHeightMapPicture = dynamic_cast<PictureBox*>(pmControl->
        Parent->Controls->get_Item(S"MaskPicture"));

    if (!pmHeightMapPath || !pmHeightMapPicture)
        return;

    // check we haven't already added this file
    for (NiInt32 i = 0; i < m_pmAddedItems->Count; ++i)
    {
        ItemRecord* pmRecord = m_pmAddedItems->get_Item(i);
        if (String::Compare(m_pmMaskPath, pmRecord->pmFilePath) == 0)
        {
            if (pmHeightMapPath->SelectedIndex != i)
                pmHeightMapPath->SelectedIndex = i;
            return;
        }
    }

    // Create the new ItemRecord
    ItemRecord* pkRecord = new ItemRecord;
    pkRecord->pmFilePath = pmFileName;

    int iIndex = pmFileName->LastIndexOf("\\");
    pkRecord->pmName = pmFileName->Substring(iIndex + 1);
    
    System::Drawing::Imaging::PixelFormat kPixFormat;
    kPixFormat = System::Drawing::Imaging::PixelFormat::Format32bppArgb;

    Bitmap* pmSurfImage = new Bitmap(pmHeightMapPicture->BackgroundImage);    
    pkRecord->pmImage = new Bitmap(pmSurfImage);
        
    m_pmAddedItems->Add(pkRecord);

    pmHeightMapPath->Text = pkRecord->pmName;
    pmHeightMapPath->Items->Add(pkRecord->pmName);

}
//---------------------------------------------------------------------------
void MBrushTypeMask::BuildStandardOptions(
    System::Windows::Forms::TableLayoutPanel*  pmTablePanel)
{
    System::Windows::Forms::NumericUpDown*  pmBrushSize = 
        new System::Windows::Forms::NumericUpDown();
    System::Windows::Forms::TrackBar*  pmSliderSize = 
        new System::Windows::Forms::TrackBar();    
    System::Windows::Forms::Label*  pmLabel1 = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::Label*  pmLabel2 = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::Label*  pmLabel3 = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::NumericUpDown*  pmBrushSize1 = 
        new System::Windows::Forms::NumericUpDown();
    System::Windows::Forms::TrackBar*  pmSliderSize1 = 
        new System::Windows::Forms::TrackBar(); 
    System::Windows::Forms::ComboBox*  pmMaskPathComboBox = 
        new System::Windows::Forms::ComboBox();
    System::Windows::Forms::Button*  pmMaskPathTextButton = 
        new System::Windows::Forms::Button();
    System::Windows::Forms::PictureBox*  pmMaskPicture = 
        new System::Windows::Forms::PictureBox();
    System::Windows::Forms::TableLayoutPanel* pmOptionTable = 
        new System::Windows::Forms::TableLayoutPanel();

    // First set the table panel options
    pmTablePanel->ColumnCount = 3;
    pmTablePanel->RowCount = 2;
    pmTablePanel->AutoSize = true;
    pmTablePanel->AutoSizeMode = 
        System::Windows::Forms::AutoSizeMode::GrowAndShrink;
    pmTablePanel->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmTablePanel->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmTablePanel->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmTablePanel->RowStyles->Add(new System::Windows::Forms::RowStyle());
    pmTablePanel->RowStyles->Add(new System::Windows::Forms::RowStyle());
    pmTablePanel->Name = L"MainOptionPanel";
    pmTablePanel->Location = Point(0, -8);
    pmTablePanel->Anchor = AnchorStyles::Left;

    //Create the controls to be added to the panel
    // Add the combo box label
    pmLabel3->AutoSize = true;
    pmLabel3->Anchor = AnchorStyles::Left;
    pmLabel3->Text = S"Mask:";
    
    // we add the combo box
    pmMaskPathComboBox->AutoSize = false;
    pmMaskPathComboBox->Name = S"MaskPath";    
    pmMaskPathComboBox->Size = System::Drawing::Size(120, 22);
    pmMaskPathComboBox->DropDownWidth = 150;
    pmMaskPathComboBox->DrawMode = DrawMode::OwnerDrawFixed;
        
    if(m_pmAddedItems)
    {
        for (NiInt32 i = 0; i < m_pmAddedItems->Count; ++i)
        {
            pmMaskPathComboBox->Items->Add(
                m_pmAddedItems->get_Item(i)->pmName);
            if (String::Compare(m_pmAddedItems->get_Item(i)->pmFilePath,
                m_pmMaskPath) == 0)
            {
                pmMaskPathComboBox->Text = 
                    m_pmAddedItems->get_Item(i)->pmName;
            }
        }
    }
    else
    {
        m_pmAddedItems = new System::Collections::Generic::List<ItemRecord*>;
    }

    pmMaskPathComboBox->SelectedIndexChanged += new System::EventHandler(
        this, &MBrushTypeMask::ComboBox_SelectionChanged);
    pmMaskPathComboBox->DrawItem += 
        new System::Windows::Forms::DrawItemEventHandler(
        this, &MBrushTypeMask::ComboBox_DrawItems);
    pmMaskPathComboBox->KeyPress += 
        new System::Windows::Forms::KeyPressEventHandler(
        this, &MBrushTypeMask::ComboBox_ChangeReturn);

    // We now add the browse button
    pmMaskPathTextButton->AutoSize = false;
    pmMaskPathTextButton->Size = System::Drawing::Size(30, 22);
    pmMaskPathTextButton->Text = S"...";
    pmMaskPathTextButton->Name = S"MaskButton";     
    pmMaskPathTextButton->Click += new System::EventHandler(
        this, &MBrushTypeMask::Button_Click);

    // Add the picture box
    pmMaskPicture->BackgroundImageLayout = 
        System::Windows::Forms::ImageLayout::Stretch;
    pmMaskPicture->Name = S"MaskPicture";
    pmMaskPicture->BorderStyle = BorderStyle::FixedSingle;
    pmMaskPicture->Size = System::Drawing::Size(50, 50);  
    pmMaskPicture->BackColor = System::Drawing::Color::Black;
    
    if (m_pkMaskPixData)
    {
        System::Drawing::Imaging::PixelFormat kPixFormat;
        kPixFormat = System::Drawing::Imaging::PixelFormat::Format32bppArgb;

        Image* pmSurfImage = new Bitmap(
            m_pkMaskPixData->GetWidth(),
            m_pkMaskPixData->GetHeight(),
            m_pkMaskPixData->GetPixelStride() * m_pkMaskPixData->
            GetWidth(),
            kPixFormat,
            m_pkMaskPixData->GetPixels()
            );

        pmMaskPicture->BackgroundImage = new Bitmap(pmSurfImage);
    }
    
    pmOptionTable->ColumnCount = 3;
    pmOptionTable->RowCount = 2;
    pmOptionTable->AutoSize = true;
    pmOptionTable->AutoSizeMode = 
        System::Windows::Forms::AutoSizeMode::GrowAndShrink;
    pmOptionTable->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmOptionTable->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmOptionTable->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmOptionTable->RowStyles->Add(new System::Windows::Forms::RowStyle());
    pmOptionTable->RowStyles->Add(new System::Windows::Forms::RowStyle());
    pmTablePanel->SetColumnSpan(pmOptionTable, 2);
    pmOptionTable->Name = L"OptionPanel";

    // Add the size label
    pmLabel1->AutoSize = true;
    pmLabel1->Anchor = AnchorStyles::Left;
    pmLabel1->Text = S"Size";
     
    // add the size slider
    pmSliderSize->AutoSize = false;
    pmSliderSize->Minimum = 1;
    pmSliderSize->Maximum = 100;
    pmSliderSize->Value = (int)(m_fSideLength);
    pmSliderSize->Name = S"SliderSize";
    pmSliderSize->Size = System::Drawing::Size(90, 25);
    pmSliderSize->TickFrequency = 0;
    pmSliderSize->BackColor = SystemColors::ControlLightLight;
    pmSliderSize->TickStyle = 
        System::Windows::Forms::TickStyle::None;
    pmSliderSize->ValueChanged += new System::EventHandler(
        this, &MBrushTypeMask::Slider_ValueChanged);
    
    // add the size up down
    pmBrushSize->AutoSize = false;
    pmBrushSize->DecimalPlaces = 1;
    pmBrushSize->Minimum = 1;
    pmBrushSize->Maximum = 100;
    pmBrushSize->Value = m_fSideLength;
    pmBrushSize->Name = S"BrushSize";
    pmBrushSize->Size = System::Drawing::Size(48, 20);
    pmBrushSize->ValueChanged += new System::EventHandler(
        this, &MBrushTypeMask::NumericUpDown_ValueChanged);  

    // Add the rotation label
    pmLabel2->AutoSize = true;
    pmLabel2->Anchor = AnchorStyles::Left;
    pmLabel2->Text = S"Rotation";
     
    // Add the rotation slider
    pmSliderSize1->AutoSize = false;
    pmSliderSize1->Minimum = 0;
    pmSliderSize1->Maximum = 360;
    pmSliderSize1->Value = (int)(m_fRotationAngle);
    pmSliderSize1->Name = S"SliderRotation";
    pmSliderSize1->Size = System::Drawing::Size(90, 25);
    pmSliderSize1->BackColor = SystemColors::ControlLightLight;
    pmSliderSize1->TickFrequency = 0;
    pmSliderSize1->TickStyle = 
        System::Windows::Forms::TickStyle::None;
    pmSliderSize1->ValueChanged += new System::EventHandler(
        this, &MBrushTypeMask::Slider_ValueChanged);
    
    // Add the rotation up down
    pmBrushSize1->AutoSize = false;
    pmBrushSize1->DecimalPlaces = 1;
    pmBrushSize1->Minimum = 0;
    pmBrushSize1->Maximum = 360;
    pmBrushSize1->Value = m_fRotationAngle;
    pmBrushSize1->Name = S"BrushRotation";
    pmBrushSize1->Size = System::Drawing::Size(48, 20);
    pmBrushSize1->ValueChanged += new System::EventHandler(
        this, &MBrushTypeMask::NumericUpDown_ValueChanged);

    pmOptionTable->Controls->Add(pmLabel1,0,0);
    pmOptionTable->Controls->Add(pmSliderSize,1,0);
    pmOptionTable->Controls->Add(pmBrushSize,2,0);
    pmOptionTable->Controls->Add(pmLabel2,0,1);
    pmOptionTable->Controls->Add(pmSliderSize1,1,1);
    pmOptionTable->Controls->Add(pmBrushSize1,2,1);
    
    //We now add all those to the given panel
    pmTablePanel->Controls->Add(pmLabel3,0,0);
    pmTablePanel->Controls->Add(pmMaskPathComboBox,1,0);
    pmTablePanel->Controls->Add(pmMaskPathTextButton,2,0);
    pmTablePanel->Controls->Add(pmMaskPicture,0,1);
    pmTablePanel->Controls->Add(pmOptionTable,1,1);

}
//---------------------------------------------------------------------------
void MBrushTypeMask::BuildRandomizationOptions(
    System::Windows::Forms::TableLayoutPanel* pmTablePanel)
{
    // First set the table panel options
    pmTablePanel->ColumnCount = 3;
    pmTablePanel->RowCount = 3;
    pmTablePanel->AutoSize = true;
    pmTablePanel->AutoSizeMode = 
        System::Windows::Forms::AutoSizeMode::GrowAndShrink;
    pmTablePanel->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmTablePanel->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmTablePanel->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmTablePanel->RowStyles->Add(new System::Windows::Forms::RowStyle());
    pmTablePanel->RowStyles->Add(new System::Windows::Forms::RowStyle());
    pmTablePanel->RowStyles->Add(new System::Windows::Forms::RowStyle());
    pmTablePanel->Location = Point(0, -8);
    pmTablePanel->Name = L"RandomOptionPanel";
    pmTablePanel->Anchor = AnchorStyles::Left;

    System::Windows::Forms::Label*  pmRandomizeLbl = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::Button*  pmRandomizeButton = 
        new System::Windows::Forms::Button();
    System::Windows::Forms::CheckBox* pmRandomSizeCBox =
        new System::Windows::Forms::CheckBox();
    System::Windows::Forms::Label*  pmSizeVariationLbl = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::NumericUpDown* pmRandomSizeRange = 
        new System::Windows::Forms::NumericUpDown();
    System::Windows::Forms::CheckBox* pmRandomRotationCBox =
        new System::Windows::Forms::CheckBox();
    System::Windows::Forms::Label*  pmRotationVariationLbl = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::NumericUpDown* pmRandomRotationRange = 
        new System::Windows::Forms::NumericUpDown();

    // add the randomization label
    pmRandomizeLbl->AutoSize = true;
    pmRandomizeLbl->Text = S"Randomization options";
    pmRandomizeLbl->Anchor = AnchorStyles::Left;
       
    // Add the randomzie button
    pmRandomizeButton->AutoSize = false;    
    pmRandomizeButton->Size = System::Drawing::Size(100, 22);
    pmRandomizeButton->Text = S"Randomize now";
    pmRandomizeButton->Name = S"RandButton"; 
    pmTablePanel->SetColumnSpan(pmRandomizeButton,2);
    pmRandomizeButton->Enabled = m_bUseRandomSize || m_bUseRandomRotation;
    pmRandomizeButton->Click += new System::EventHandler(
        this, &MBrushTypeMask::Button_Click);

    // Add the randomize size check box
    pmRandomSizeCBox->Text = S"Randomize size";
    pmRandomSizeCBox->Name = S"RandSizeCheckBox"; 
    pmRandomSizeCBox->AutoSize = true;
    pmRandomSizeCBox->Checked = m_bUseRandomSize;
    pmRandomSizeCBox->CheckedChanged += new System::EventHandler(
        this, &MBrushTypeMask::CheckBox_CheckedChanged);

    // Add the variation label
    pmSizeVariationLbl->Text = S"Delta:"; 
    pmSizeVariationLbl->AutoSize = true;
    pmSizeVariationLbl->Anchor = AnchorStyles::Left;
    
    // Add the range up down
    pmRandomSizeRange->Name = S"RandSizeUpDown";
    pmRandomSizeRange->Size = System::Drawing::Size(48, 20);
    pmRandomSizeRange->Minimum = 1.0f;
    pmRandomSizeRange->Maximum = 100.0f;
    pmRandomSizeRange->Enabled = m_bUseRandomSize;
    pmRandomSizeRange->Value = m_fRandomSizeRange;
    pmRandomSizeRange->ValueChanged += new System::EventHandler(
        this, &MBrushTypeMask::NumericUpDown_ValueChanged); 
    
    // Add the rotation check box
    pmRandomRotationCBox->Text = S"Randomize Rotation";
    pmRandomRotationCBox->Name = S"RandRotCheckBox";
    pmRandomRotationCBox->AutoSize = true;
    pmRandomRotationCBox->Checked = m_bUseRandomRotation;
    pmRandomRotationCBox->CheckedChanged += new System::EventHandler(
        this, &MBrushTypeMask::CheckBox_CheckedChanged);

    // add the variation label
    pmRotationVariationLbl->Text = S"Delta:";
    pmRotationVariationLbl->AutoSize = true;
    pmRotationVariationLbl->Anchor = AnchorStyles::Left;
    
    // add the range up down for the rotation
    pmRandomRotationRange->Name = S"RandRotUpDown";
    pmRandomRotationRange->Size = System::Drawing::Size(48, 20);
    pmRandomRotationRange->Minimum = 1.0f;
    pmRandomRotationRange->Maximum = 360.0f;
    pmRandomRotationRange->Enabled = m_bUseRandomRotation;
    pmRandomRotationRange->Value = 
        NiClamp(m_fRandomRotationRange * 180.0f / NI_PI, 1.0f, 360.0f);
    pmRandomRotationRange->ValueChanged += new System::EventHandler(
        this, &MBrushTypeMask::NumericUpDown_ValueChanged); 

    // Add all the created controlas to the panel
    pmTablePanel->Controls->Add(pmRandomizeLbl, 0,0);
    pmTablePanel->Controls->Add(pmRandomizeButton, 1,0);
    pmTablePanel->Controls->Add(pmRandomSizeCBox, 0,1);
    pmTablePanel->Controls->Add(pmSizeVariationLbl, 1,1);
    pmTablePanel->Controls->Add(pmRandomSizeRange, 2,1);
    pmTablePanel->Controls->Add(pmRandomRotationCBox, 0,2);
    pmTablePanel->Controls->Add(pmRotationVariationLbl, 1,2);
    pmTablePanel->Controls->Add(pmRandomRotationRange, 2,2);    

}
//---------------------------------------------------------------------------
void MBrushTypeMask::BuildAdvancedOptions(
    System::Windows::Forms::TableLayoutPanel* pmTablePanel)
{
    // First set the table panel options
    pmTablePanel->ColumnCount = 2;
    pmTablePanel->RowCount = 3;
    pmTablePanel->AutoSize = true;
    pmTablePanel->AutoSizeMode = 
        System::Windows::Forms::AutoSizeMode::GrowAndShrink;
    pmTablePanel->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmTablePanel->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmTablePanel->RowStyles->Add(new System::Windows::Forms::RowStyle());
    pmTablePanel->RowStyles->Add(new System::Windows::Forms::RowStyle());
    pmTablePanel->RowStyles->Add(new System::Windows::Forms::RowStyle());
    pmTablePanel->Location = Point(0, -8);
    pmTablePanel->Name = L"AdvancedOptionPanel";
    pmTablePanel->Anchor = AnchorStyles::Left;

    System::Windows::Forms::TableLayoutPanel* pmNeutralColorTable = 
        new System::Windows::Forms::TableLayoutPanel;
    System::Windows::Forms::Label*  pmNeutralColorLbl = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::Panel*  pmNeutralColorPanel = 
        new System::Windows::Forms::Panel();
    System::Windows::Forms::TrackBar*  pmNeutralColorSlider = 
        new System::Windows::Forms::TrackBar();
    System::Windows::Forms::NumericUpDown*  pmNeutralColorUpDown = 
        new System::Windows::Forms::NumericUpDown();
    System::Windows::Forms::CheckBox* pmInvertMask =
        new System::Windows::Forms::CheckBox();
    System::Windows::Forms::CheckBox* pmRotateWithCam =
        new System::Windows::Forms::CheckBox();
    System::Windows::Forms::CheckBox* pmUseFilter =
        new System::Windows::Forms::CheckBox();

    pmNeutralColorTable->ColumnCount = 4;
    pmNeutralColorTable->RowCount = 1;
    pmNeutralColorTable->AutoSize = true;
    pmNeutralColorTable->AutoSizeMode = 
        System::Windows::Forms::AutoSizeMode::GrowAndShrink;
    pmNeutralColorTable->ColumnStyles->Add(
        new System::Windows::Forms::ColumnStyle());
    pmNeutralColorTable->ColumnStyles->Add(
        new System::Windows::Forms::ColumnStyle());
    pmNeutralColorTable->RowStyles->Add(
        new System::Windows::Forms::RowStyle());
    pmNeutralColorTable->Name = L"NeutralColorOptionPanel";
    pmNeutralColorTable->Dock = DockStyle::Fill;
    pmTablePanel->SetColumnSpan(pmNeutralColorTable, 2); 

    pmNeutralColorLbl->Text = S"Neutral color:";
    pmNeutralColorLbl->AutoSize = true;
    pmNeutralColorLbl->Anchor = AnchorStyles::Left;
    
    // Add the panel that will serve as a color display
    pmNeutralColorPanel->Name = S"ColorDisplay";
    pmNeutralColorPanel->Size = System::Drawing::Size(40, 25);
    pmNeutralColorPanel->BackColor = System::Drawing::Color::FromArgb(
        m_uiNeutralColor, m_uiNeutralColor, m_uiNeutralColor);
    pmNeutralColorPanel->BorderStyle = BorderStyle::FixedSingle;
    pmNeutralColorPanel->Anchor = AnchorStyles::Left;

    // Add the color selecting slider
    pmNeutralColorSlider->Name = S"ColorSlider";
    pmNeutralColorSlider->AutoSize = false;
    pmNeutralColorSlider->Size = System::Drawing::Size(90, 25);
    pmNeutralColorSlider->Minimum = 0;
    pmNeutralColorSlider->Maximum = 255;
    pmNeutralColorSlider->Value = m_uiNeutralColor;
    pmNeutralColorSlider->TickFrequency = 0;
    pmNeutralColorSlider->BackColor = SystemColors::ControlLightLight;
    pmNeutralColorSlider->Anchor = AnchorStyles::Left;
    pmNeutralColorSlider->TickStyle = 
        System::Windows::Forms::TickStyle::None;
    pmNeutralColorSlider->ValueChanged += new System::EventHandler(
        this, &MBrushTypeMask::Slider_ValueChanged);

    // Add the color selectin numeric up down
    pmNeutralColorUpDown->AutoSize = false;
    pmNeutralColorUpDown->Name = S"ColorUpDown";
    pmNeutralColorUpDown->Size = System::Drawing::Size(48, 20);
    pmNeutralColorUpDown->Minimum = 0;
    pmNeutralColorUpDown->Maximum = 255;
    pmNeutralColorUpDown->Value = m_uiNeutralColor;
    pmNeutralColorUpDown->Anchor = AnchorStyles::Left;
    pmNeutralColorUpDown->ValueChanged += new System::EventHandler(
        this, &MBrushTypeMask::NumericUpDown_ValueChanged);

    pmNeutralColorTable->Controls->Add(pmNeutralColorLbl, 0, 0);
    pmNeutralColorTable->Controls->Add(pmNeutralColorPanel, 1, 0);
    pmNeutralColorTable->Controls->Add(pmNeutralColorSlider, 2, 0);
    pmNeutralColorTable->Controls->Add(pmNeutralColorUpDown, 3, 0);

    // Add the invert color check box
    pmInvertMask->Text = S"Invert colors";
    pmInvertMask->Name = S"InvertMask"; 
    pmInvertMask->AutoSize = true;
    pmInvertMask->Checked = m_bUseInvertMask;
    pmInvertMask->CheckedChanged += new System::EventHandler(
        this, &MBrushTypeMask::CheckBox_CheckedChanged);
    
    // Add the bilinear filter check box
    pmUseFilter->Text = S"Use bilinear filtering";
    pmUseFilter->Name = S"BiFilterCheckBox"; 
    pmUseFilter->AutoSize = true;
    pmUseFilter->Checked = m_bUseFiltering;
    pmUseFilter->CheckedChanged += new System::EventHandler(
        this, &MBrushTypeMask::CheckBox_CheckedChanged);
    
    // add the align to camera check box
    pmRotateWithCam->Text = S"Rotate with camera";
    pmRotateWithCam->Name = S"CamFollowCheckBox"; 
    pmRotateWithCam->AutoSize = true;
    pmRotateWithCam->Checked = m_bUseAlignToCamera;
    pmTablePanel->SetColumnSpan(pmRotateWithCam, 2);   
    pmRotateWithCam->CheckedChanged += new System::EventHandler(
        this, &MBrushTypeMask::CheckBox_CheckedChanged);

    // Add all the created controls to the panel
    pmTablePanel->Controls->Add(pmNeutralColorTable, 0, 0);
    pmTablePanel->Controls->Add(pmInvertMask, 0, 1);
    pmTablePanel->Controls->Add(pmUseFilter, 1, 1);
    pmTablePanel->Controls->Add(pmRotateWithCam, 0, 2);
    
}
//---------------------------------------------------------------------------
bool MBrushTypeMask::SetupBrushOverlay()
{   
    // Create the brush overlay
    m_pkBrushOverlay = NiNew NiMesh();
    m_pkBrushOverlay->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);
    m_pkBrushOverlay->SetName("Brush");
    m_pkBrushOverlay->SetTranslate(0,0,0);
    m_pkBrushOverlay->SetRotate(m_fRotationAngle, 0.0f, 0.0f, 1.0f);
      
    // calculate the number of indices and vertices required
    NiUInt32 uiNumIndex = 
        (NiUInt32)(NiPow((NiSqrt(ms_usVertices) - 1) * 6, 2));
    NiUInt32 uiNumVertSide = (NiUInt32)(NiSqrt(ms_usVertices));
    
    // Create the streams
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

    NiDataStreamElementLock kTexLock = m_pkBrushOverlay->AddStreamGetLock(
        NiCommonSemantics::TEXCOORD(), 0,
        NiDataStreamElement::F_FLOAT32_2, 
        ms_usVertices,
        NiDataStream::ACCESS_CPU_READ | 
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_VERTEX, false, true
        );

    NIASSERT(kTexLock.count() == ms_usVertices);

    NiTStridedRandomAccessIterator<NiPoint2> kTexIter = 
        kTexLock.begin<NiPoint2>();

    NiDataStreamElementLock kIndLock = m_pkBrushOverlay->AddStreamGetLock(
        NiCommonSemantics::INDEX(), 0,
        NiDataStreamElement::F_UINT16_1, 
        uiNumIndex,
        NiDataStream::ACCESS_CPU_READ | 
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX_INDEX, false, true
        );

    NIASSERT((int)(kIndLock.count()) == (uiNumIndex));

    NiTStridedRandomAccessIterator<NiUInt16> kIndIter = 
        kIndLock.begin<NiUInt16>();

    // Create the color stream.
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

    // now fill in the information
    float fXPosVal = - m_fSizeInner;
    float fYPosVal = - m_fSizeInner;
    float fPosStep = (m_fSizeInner * 2.0f) / (uiNumVertSide - 1.0f);
    float fXTexVal = 0.0f;
    float fYTexVal = 1.0f;
    float fTexStep = 1.0f / (float)(uiNumVertSide - 1.0f);
    NiRGBA kColor;
    m_pkBaseColor->GetAs(kColor);
    
    for (NiUInt32 i = 0; i < uiNumVertSide; i++)
    {
        // first add the vertices info
        fXPosVal = - m_fSizeInner;
        fXTexVal = 0;
        for (NiUInt32 j = 0; j < uiNumVertSide; j++)
        {
            NiUInt32 uiIndex = j + (uiNumVertSide * i);
            kPointsIter[uiIndex] = NiPoint3(fXPosVal, fYPosVal, 0.0f);
            kColorsIter[uiIndex] = kColor;
            kTexIter[uiIndex] = NiPoint2(fXTexVal, fYTexVal);
    
            fXPosVal += fPosStep;
            fXTexVal += fTexStep;
            
        }
        fYPosVal += fPosStep;
        fYTexVal -= fTexStep;
    }

    NiUInt32 uiIndex = 0;

    // now we add the index data to the index stream
    for(NiUInt32 uiCol = 0; uiCol < uiNumVertSide - 1; ++uiCol)
    {
        for(NiUInt32 uiRow = 0; uiRow < uiNumVertSide - 1; ++uiRow)
        {
            NiUInt32 uiVertA = uiRow + (uiCol * uiNumVertSide);
            NiUInt32 uiVertB = uiRow + 1 + (uiCol * uiNumVertSide);
            NiUInt32 uiVertC = uiRow + ((uiCol + 1) * uiNumVertSide);
            NiUInt32 uiVertD = uiRow + 1 + ((uiCol + 1) * uiNumVertSide);

            kIndIter[uiIndex] = (NiUInt16)uiVertA;
            kIndIter[uiIndex + 1] = (NiUInt16)uiVertB;
            kIndIter[uiIndex + 2] = (NiUInt16)uiVertC;

            kIndIter[uiIndex + 3] = (NiUInt16)uiVertB;
            kIndIter[uiIndex + 4] = (NiUInt16)uiVertD;
            kIndIter[uiIndex + 5] = (NiUInt16)uiVertC;
            uiIndex += 6;
        }
    }
    
    kPosLock.Unlock();
    kIndLock.Unlock();
    kColorLock.Unlock();
    kTexLock.Unlock();
   
    // Set the bound
    NiBound kBound;
    kBound.SetCenterAndRadius(NiPoint3(0,0,0), m_fSizeOuter);
    m_pkBrushOverlay->SetModelBound(kBound); 

    m_pkBrushOverlay->SetSubmeshCount(1);
    
    // Create the texturing property
    NiTexturingProperty* pkMaskBrushTexture = NiNew NiTexturingProperty();
        NiTexture* pkTexture = NULL;
    if (m_pkMaskPixData)
        pkTexture = 
        NiSourceTexture::Create((NiPixelData*)m_pkMaskPixData->Clone());
    pkMaskBrushTexture->SetBaseTexture(pkTexture);
    m_pkBrushOverlay->AttachProperty(pkMaskBrushTexture);

    NiAlphaProperty* pkAlphaMask = NiNew NiAlphaProperty();
    pkAlphaMask->SetAlphaBlending(true); 
    m_pkBrushOverlay->AttachProperty(pkAlphaMask); 
    
    NiVertexColorProperty* pkVertexColorProperty = 
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
void MBrushTypeMask::UpdateBrushOverlay(
    NiPoint3& kCenter, 
    NiTerrainInteractor* pkTerrainComponent, 
    NiBrushPassPointInfo*)
{
    ENSURE_GIZMO_LOADED();
    if (!m_pkBrushOverlay)
        return;

    // Make sure the overlay has the right scale.
    float fScale = MTerrainPlugin::GetInstance()->GetBrush()->GetScale();
    ResizeBrushOverlay(fScale);

    // We first translate the brush so the bound gets updated.
    m_pkBrushOverlay->SetTranslate(kCenter);
    NiMatrix3 kMatRotate = NiMatrix3::IDENTITY;

    if (m_bUseAlignToCamera)
    {
        // Get the camera's rotation arround Z
        NiCamera* pkActiveCamera = MFramework::Instance->
            ViewportManager->ActiveViewport->GetNiCamera();

        NiPoint3 kViewDirection = pkActiveCamera->GetWorldDirection();
        kViewDirection.z = 0;
        kViewDirection.Unitize();
        float fZRotationAngle = NiATan2(kViewDirection.y, kViewDirection.x);
        fZRotationAngle *= -1.0f;         

        kMatRotate.MakeZRotation(fZRotationAngle);
    }
    // Apply the given rotation arround Z
    NiMatrix3 temp;

    if (!m_bUseRandomRotation)
        temp.MakeZRotation(m_fRotationAngle);
    else
        temp.MakeZRotation(m_fRandomizedRotationAngle);

    *m_pkRotation = kMatRotate * temp;
    m_pkBrushOverlay->SetRotate(*m_pkRotation);
        
    NiDataStreamElementLock kPosLock(m_pkBrushOverlay, 
        NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_WRITE | 
        NiDataStream::LOCK_READ);

    NIASSERT(kPosLock.IsLocked()); 
    NiTStridedRandomAccessIterator<NiPoint3> kPointsIter = 
        kPosLock.begin<NiPoint3>();
    
    // We now need to retrieve the vertices and transform them in order to get 
    // the boundary heights so the overlay follows the terrain.
    float fTerrainIntersection = 0;
    NiPoint3 kOverlayPoint;
    for (NiUInt32 ui = 0; ui < ms_usVertices; ++ui)
    {
        kOverlayPoint = 
            m_pkBrushOverlay->GetWorldTransform() * kPointsIter[ui];

        // Working the height out.
        if (GetBoundaryIntersection(
            kOverlayPoint, pkTerrainComponent, fTerrainIntersection)) 
        {
            // Modify the height in accordance to the translation made
            // so we don't translate the mesh on z twice.
            kPointsIter[ui].z = fTerrainIntersection - kCenter.z;
        }
       
    }
    
    kPosLock.Unlock();

    if (m_pkBrushOverlay && m_pkMaskPixData)
    {
        // set the filtering mode on the texturing property
        NiTexturingProperty* pkTexProp = (NiTexturingProperty*)
            m_pkBrushOverlay->GetProperty(NiTexturingProperty::GetType());
        
        if (m_bUseFiltering)
            pkTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_BILERP);
        else
            pkTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);        

        m_pkBrushOverlay->UpdateProperties();
    }

         
    m_pkBrushOverlay->Update(0.0f);   
}
//---------------------------------------------------------------------------
void MBrushTypeMask::GetBrushOptionPanel(
    System::Windows::Forms::Panel* pmPanel)
{
    System::Windows::Forms::TabControl* pmTabPanel = 
        new System::Windows::Forms::TabControl();
    System::Windows::Forms::TabPage* pmStandardPage = 
        new System::Windows::Forms::TabPage();
    System::Windows::Forms::TabPage* pmRandomPage = 
        new System::Windows::Forms::TabPage();
    System::Windows::Forms::TabPage* pmAdvancePage = 
        new System::Windows::Forms::TabPage();
    
    System::Windows::Forms::TableLayoutPanel* pmOptionPanel = 
        new System::Windows::Forms::TableLayoutPanel();
    System::Windows::Forms::TableLayoutPanel* pmRandomPanel = 
        new System::Windows::Forms::TableLayoutPanel();
    System::Windows::Forms::TableLayoutPanel* pmAdvancePanel = 
        new System::Windows::Forms::TableLayoutPanel();
    
    // we first create the tab control and the tab pages that will hold all the
    // required controls
    pmTabPanel->Controls->Add(pmStandardPage);
    pmTabPanel->Controls->Add(pmRandomPage);
    pmTabPanel->Controls->Add(pmAdvancePage);
    pmTabPanel->Location = System::Drawing::Point(0, 0);
    pmTabPanel->Name = S"OptionTab";
    pmTabPanel->SelectedIndex = 0;
    pmTabPanel->Dock = DockStyle::Fill;
    
    // The standard option page
    pmStandardPage->Location = System::Drawing::Point(0, 0);
    pmStandardPage->Name = L"StandOpt";
    pmStandardPage->Padding = System::Windows::Forms::Padding(3);
    pmStandardPage->Size = System::Drawing::Size(253, 77);
    pmStandardPage->TabIndex = 0;
    pmStandardPage->Text = L"Standard";
    pmStandardPage->UseVisualStyleBackColor = true;
    pmStandardPage->Controls->Add(pmOptionPanel);
     
    // the randomize options page
    pmRandomPage->Location = System::Drawing::Point(0, 0);
    pmRandomPage->Name = L"RandOpt";
    pmRandomPage->Padding = System::Windows::Forms::Padding(3);
    pmRandomPage->Size = System::Drawing::Size(253, 77);
    pmRandomPage->TabIndex = 1;
    pmRandomPage->Text = L"Randomization";
    pmRandomPage->UseVisualStyleBackColor = true;
    pmRandomPage->Controls->Add(pmRandomPanel);
    
    // the advanced option page
    pmAdvancePage->Location = System::Drawing::Point(0, 0);
    pmAdvancePage->Name = L"AdvOpt";
    pmAdvancePage->Size = System::Drawing::Size(253, 77);
    pmAdvancePage->TabIndex = 2;
    pmAdvancePage->Text = L"Advanced";
    pmAdvancePage->UseVisualStyleBackColor = true;
    pmAdvancePage->Controls->Add(pmAdvancePanel);

    // Build the different panels
    BuildStandardOptions(pmOptionPanel);
    BuildRandomizationOptions(pmRandomPanel);
    BuildAdvancedOptions(pmAdvancePanel);

    pmPanel->Controls->Add(pmTabPanel);

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
bool MBrushTypeMask::ResizeBrushOverlay(float fCurrentScale)
{
    if (!m_pkBrushOverlay)
        return false;

    // Lock the position stream
    NiDataStreamElementLock kPosLock(m_pkBrushOverlay, 
        NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_WRITE);

    NIASSERT(kPosLock.IsLocked()); 
    NiTStridedRandomAccessIterator<NiPoint3> kPointsIter = 
        kPosLock.begin<NiPoint3>();

    NiUInt32 uiNumVertSide = (NiUInt32)(NiSqrt(ms_usVertices));
    float fXPosVal = - fCurrentScale * m_fSizeInner;
    float fYPosVal = - fCurrentScale * m_fSizeInner;
    float fPosStep = (fCurrentScale * (m_fSizeInner * 2.0f)) 
        / (uiNumVertSide - 1.0f);
     
    // recompute each vertices according to the new size
    for (NiUInt32 i = 0; i < uiNumVertSide; i++)
    {
        fXPosVal = - fCurrentScale * m_fSizeInner;
        for (NiUInt32 j = 0; j < uiNumVertSide; j++)
        {
            NiUInt32 uiIndex = j + (uiNumVertSide * i);
            kPointsIter[uiIndex] = NiPoint3(fXPosVal, fYPosVal, 0.0f);
    
            fXPosVal += fPosStep;
        }
        fYPosVal += fPosStep;
    }    

    kPosLock.Unlock();

    m_pkBrushOverlay->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_pkBrushOverlay);

    return true;
}
//---------------------------------------------------------------------------
bool MBrushTypeMask::ValidateBrushType()
{
    if (m_fSizeInner <= 0 || m_pkMaskPixData == 0)
        return false;

    return true;
}
//---------------------------------------------------------------------------
void MBrushTypeMask::Randomize()
{ 
    // Randomize rotation if necessary
    if (m_bUseRandomRotation)
    {
        float fMin = NiMax(m_fRotationAngle - m_fRandomRotationRange, 0.0f);
        float fMax = 
            NiMin(m_fRotationAngle + m_fRandomRotationRange, NI_TWO_PI);

        m_fRandomizedRotationAngle =  
            (float)(fMin + (NiUnitRandom() * (fMax - fMin)));
    }

    // randomize size if necessary
    if (m_bUseRandomSize)
    {
        NiUInt32 uiMin = (NiUInt32)
            (NiMax(m_fSideLength - m_fRandomSizeRange, 1.0f));
        NiUInt32 uiMax = (NiUInt32)
            (NiMin(m_fSideLength + m_fRandomSizeRange, 100.0f));
        
        m_fRandomizedSideLength = 
            (float)(uiMin + (NiRand() % (uiMax - uiMin)));

        // Set the size appropriately
        m_fSizeInner = m_fRandomizedSideLength;
        m_fSizeOuter = (float) NiSqrt(2.0f) * (m_fRandomizedSideLength);
    }
}
//---------------------------------------------------------------------------
System::Void MBrushTypeMask::NumericUpDown_ValueChanged(
    System::Object*  sender, System::EventArgs*  e)
{
    NI_UNUSED_ARG(e);

    NumericUpDown* pmControl = dynamic_cast<NumericUpDown*>(sender);

    if (String::Compare(pmControl->Name, S"BrushSize") == 0)
    {
        // the brush size has changed
        m_fSideLength = (float)pmControl->Value;
                    
        TrackBar* pmOutTrack = dynamic_cast<TrackBar*>(pmControl->
            Parent->Controls->get_Item(S"SliderSize"));
        pmOutTrack->Value = (int)(m_fSideLength);

        m_fSizeInner = (m_fSideLength);
        m_fSizeOuter = ((float) sqrt(2.0f) * (m_fSideLength));
        Randomize();
    }
    else if (String::Compare(pmControl->Name, S"BrushRotation") == 0)
    {
        // the rotation angle has changed
        m_fRotationAngle = ((float)(pmControl->Value) * NI_PI) / 180.0f;
                    
        TrackBar* pmOutTrack = dynamic_cast<TrackBar*>(pmControl->
            Parent->Controls->get_Item(S"SliderRotation"));
        pmOutTrack->Value = (int)pmControl->Value;
        Randomize();
    }
    else if (String::Compare(pmControl->Name, S"RandSizeUpDown") == 0)
    {
        // the range for the size randomization has changed
        m_fRandomSizeRange = (float)pmControl->Value;
        Randomize();
    }   
    else if (String::Compare(pmControl->Name, S"RandRotUpDown") == 0)
    {
        // the range for the rotation randomization has changed
        m_fRandomRotationRange = ((float)(pmControl->Value) * NI_PI) / 180.0f;
        Randomize();
    }
    else if (String::Compare(pmControl->Name, S"ColorUpDown") == 0) 
    {
        // the neutral color has changed
        m_uiNeutralColor = (NiUInt32)pmControl->Value;
        TrackBar* pmTrackBar = dynamic_cast<TrackBar*>(pmControl->
            Parent->Controls->get_Item(S"ColorSlider"));
        Panel* pmColor = dynamic_cast<Panel*>(pmControl->
            Parent->Controls->get_Item(S"ColorDisplay"));        
        pmColor->BackColor = System::Drawing::Color::FromArgb(
            m_uiNeutralColor, m_uiNeutralColor, m_uiNeutralColor);
        pmTrackBar->Value = (int)(pmControl->Value);
    }
}

//---------------------------------------------------------------------------
bool MBrushTypeMask::SetAndConvertMaskFile(Control *pmControl, 
    System::String* pmFileName)
{
    Image* pmSurfImage = 0;
    NiPixelDataPtr spSrcPixData;
    PictureBox* pmHeightMapPicture = dynamic_cast<PictureBox*>(pmControl->
        Parent->Controls->get_Item(S"MaskPicture"));

    // Check that the file exists and is accessible.
    if (!NiFile::Access(MStringToCharPointer(pmFileName), NiFile::READ_ONLY))
    {
        MessageBox::Show("Unable To Access File.", "Error Opening Brush Mask",
            MessageBoxButtons::OK, MessageBoxIcon::Error);
        return false;
    }

    // Load the file using the image converter if possible.
    NiDevImageConverter kImageConverter;
    if (kImageConverter.CanReadImageFile(MStringToCharPointer(pmFileName)))
    {
        spSrcPixData = 
            kImageConverter.ReadImageFile(MStringToCharPointer(pmFileName),0);
        
        if (spSrcPixData)
        {
            if (m_pkMaskPixData)
                m_pkMaskPixData->DecRefCount();
            m_pkMaskPixData = kImageConverter.ConvertPixelData
                (*spSrcPixData, NiPixelFormat::RGBA32, 0, false);
            m_pkMaskPixData->IncRefCount();

            System::Drawing::Imaging::PixelFormat kPixFormat;
            kPixFormat =
                System::Drawing::Imaging::PixelFormat::Format32bppArgb;

            NiUInt32 uiMaskSize = m_pkMaskPixData->GetWidth();
            NiUInt32 uiMaskStride = m_pkMaskPixData->GetPixelStride();
            unsigned char* pucPixels = m_pkMaskPixData->GetPixels();
            unsigned char ucGrayscale = 0;

            // Check the selected image is of appropriate size
            if (m_pkMaskPixData->GetWidth() != m_pkMaskPixData->GetHeight())
            {
                MessageBox::Show("Mask Image Must Be Square.",
                    "Error Opening Brush Mask",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
                return false;
            }
            else
            {     
                // read the values of the image and set them in the pixel data
                for (unsigned int uiReadX=0; uiReadX < uiMaskSize;
                    ++uiReadX)
                {
                    for (unsigned int uiReadY=0; uiReadY < uiMaskSize;
                        ++uiReadY)
                    {
                        NiUInt32 uiPixOffset = (uiReadX + uiReadY *
                            uiMaskSize) * uiMaskStride;
                        unsigned char R = pucPixels[uiPixOffset + 0];
                        unsigned char G = pucPixels[uiPixOffset + 1];
                        unsigned char B = pucPixels[uiPixOffset + 2];
                        
                        // Do the luminence and greyscale conversion
                        if (R!=255 || G!=255 || B!=255)
                        {
                            ucGrayscale = (unsigned char)(0.299*R + 0.587*G 
                                + 0.114*B);
                            pucPixels[uiPixOffset + 0] = ucGrayscale;
                            pucPixels[uiPixOffset + 1] = ucGrayscale;
                            pucPixels[uiPixOffset + 2] = ucGrayscale;
                            pucPixels[uiPixOffset + 3] = ucGrayscale;
                        }
                    }
                }
                // create the bitmap to be displayed in the image control
                pmSurfImage = new Bitmap(
                    m_pkMaskPixData->GetWidth(),
                    m_pkMaskPixData->GetHeight(),
                    m_pkMaskPixData->GetPixelStride() * m_pkMaskPixData->
                    GetWidth(),
                    kPixFormat,
                    m_pkMaskPixData->GetPixels()
                    );

                pmHeightMapPicture->BackgroundImage = new Bitmap(pmSurfImage);
                                
                if (m_bUseInvertMask)
                {
                    // If we want the mask to me inverted we need to recompute 
                    // it now
                    for (unsigned int uiReadX=0; uiReadX < uiMaskSize;
                    ++uiReadX)
                    {
                        for (unsigned int uiReadY=0; uiReadY < uiMaskSize;
                            ++uiReadY)
                        {
                            NiUInt32 uiPixOffset = (uiReadX + uiReadY *
                                uiMaskSize) * uiMaskStride;
                            ucGrayscale = (unsigned char)
                                (255 - pucPixels[uiPixOffset]);
                            
                            pucPixels[uiPixOffset + 0] = ucGrayscale;
                            pucPixels[uiPixOffset + 1] = ucGrayscale;
                            pucPixels[uiPixOffset + 2] = ucGrayscale;
                            pucPixels[uiPixOffset + 3] = ucGrayscale;
                        }
                    }
                }

                if (m_pkBrushOverlay)
                {
                    // Update the texturing property with the new texture
                    NiTexturingProperty* pkTexProp = (NiTexturingProperty*)
                        m_pkBrushOverlay->GetProperty(NiTexturingProperty::
                        GetType());
                    
                    pkTexProp->SetBaseTexture(
                        NiSourceTexture::Create((NiPixelData*)m_pkMaskPixData->
                        Clone()));
                    m_pkBrushOverlay->UpdateProperties();
                }
            }
        }
        else
        {
            MessageBox::Show("Pixel Data Invalid.", "Error Opening Brush Mask",
                MessageBoxButtons::OK, MessageBoxIcon::Error);

            if (pmSurfImage == 0)
            {
                // Load a default "NON" texture?
                System::ComponentModel::ComponentResourceManager*
                    resourceManager = MTerrainPlugin::GetResourceManager();
                pmSurfImage = dynamic_cast<Image*>
                    (resourceManager->GetObject("HeightMapUnavailable"));
            }
            return false;
        }
    }
    else
    {
        MessageBox::Show("File Type Invalid.", "Error Opening Brush Mask",
            MessageBoxButtons::OK, MessageBoxIcon::Error);

        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
System::Void MBrushTypeMask::ComboBox_ChangeReturn(
    System::Object*  sender, System::Windows::Forms::KeyPressEventArgs*  e)
{
    // Do nothing unless return is pressed.
    if (e->KeyChar != 13) 
      return;
    
    Control* pmControl = dynamic_cast<Control*>(sender);
    ComboBox* pmHeightMapPath = dynamic_cast<ComboBox*>(pmControl->
        Parent->Controls->get_Item(S"MaskPath"));
    
    NiFixedString kFileName = MStringToCharPointer(pmHeightMapPath->Text);
        
    if(!SetAndConvertMaskFile(pmControl, kFileName))
    {  
        // if we couldn't load the image check the already loaded items
        bool bFound = false;
        for (NiInt32 i = 0; i < m_pmAddedItems->Count; ++i)
        {
            ItemRecord* pmRecord = m_pmAddedItems->get_Item(i);
            if (String::Compare(m_pmMaskPath, pmRecord->pmFilePath) == 0)
            {
                pmHeightMapPath->Text = pmRecord->pmName;
                bFound = true;
                break;
            }
        }
        // nothing was found set the text back to what it was
        if (!bFound)
            pmHeightMapPath->Text = m_pmMaskPath;        
    }
    else
    {
        // This is a new item that needs to be added to the combo box
        AddItem(m_pmMaskPath, pmHeightMapPath);
    }
}
//---------------------------------------------------------------------------
System::Void MBrushTypeMask::ComboBox_SelectionChanged(System::Object*  sender,
    System::EventArgs*  e)
{
    NI_UNUSED_ARG(e);

    Control* pmControl = dynamic_cast<Control*>(sender);
    ComboBox* pmHeightMapPath = dynamic_cast<ComboBox*>(pmControl->
        Parent->Controls->get_Item(S"MaskPath"));

    if (pmHeightMapPath->SelectedIndex >= m_pmAddedItems->Count)
        return;

    // Get the full path of the selected item
    System::String* pmFileName =  
        m_pmAddedItems->get_Item(pmHeightMapPath->SelectedIndex)->pmFilePath;
    
    // load the item using the full path
    if (SetAndConvertMaskFile(pmControl, pmFileName))
    {
        m_pmMaskPath = pmFileName; 
    }
}
//---------------------------------------------------------------------------
System::Void MBrushTypeMask::ComboBox_DrawItems(System::Object*  sender, 
    DrawItemEventArgs* e)
{
    NI_UNUSED_ARG(sender);

    if (e->Index >= m_pmAddedItems->Count)
        return;

    // first we should draw the background
    e->DrawBackground();

    Graphics* pmGraphics = e->Graphics;
    System::Drawing::Rectangle kRect = 
        System::Drawing::Rectangle(e->Bounds.Location.X + 1, 
        e->Bounds.Location.Y + 1, e->Bounds.Height - 2, e->Bounds.Height - 2);

    System::String* pmName = m_pmAddedItems->get_Item(e->Index)->pmName; 
       
    // Set the back and fore colors
    Color kBackColor = e->BackColor;
    Color kForeColor = e->ForeColor;
           
    // then draw the image
    pmGraphics->FillRectangle(new SolidBrush(kForeColor), kRect);
    pmGraphics->DrawImage(m_pmAddedItems->get_Item(e->Index)->pmImage, kRect);

    // finally draw the text
    System::Drawing::RectangleF kStringBounds(
        (float)(kRect.X + kRect.Width + 1.0f), 
        (float)(kRect.Y), 
        (float)(e->Bounds.Width - kRect.Width), 
        (float)(kRect.Height));
    pmGraphics->DrawString(pmName, e->Font, new SolidBrush(kForeColor), 
        kStringBounds);
    
}
//---------------------------------------------------------------------------
System::Void MBrushTypeMask::CheckBox_CheckedChanged(System::Object*  sender,
    System::EventArgs*  e)
{
    NI_UNUSED_ARG(e);

    CheckBox* pmCheckBox = dynamic_cast<CheckBox*>(sender);

    if (String::Compare(pmCheckBox->Name, S"RandSizeCheckBox") == 0)
    {
        // The random size check box has changed
        m_bUseRandomSize = pmCheckBox->Checked;
        pmCheckBox->Parent->Controls->
            get_Item(S"RandSizeUpDown")->Enabled = m_bUseRandomSize;
        pmCheckBox->Parent->Controls->
            get_Item(S"RandButton")->Enabled = 
            m_bUseRandomSize || m_bUseRandomRotation;

        Randomize();

        // We need to update back to the original version
        if (!m_bUseRandomSize)
        {
            m_fSizeInner = m_fSideLength;
            m_fSizeOuter = (float) NiSqrt(2.0f) * (m_fSideLength);
        }
    }
    else if (String::Compare(pmCheckBox->Name, S"RandRotCheckBox") == 0)
    {
        // the random rotation check box has changed
        m_bUseRandomRotation = pmCheckBox->Checked;
        pmCheckBox->Parent->Controls->
            get_Item(S"RandRotUpDown")->Enabled = m_bUseRandomRotation;
        pmCheckBox->Parent->Controls->
            get_Item(S"RandButton")->Enabled = 
            m_bUseRandomSize || m_bUseRandomRotation;
        Randomize();
    }
    else if (String::Compare(pmCheckBox->Name, S"InvertMask") == 0)
    {
        // the invert mask check box has changed
        m_bUseInvertMask = pmCheckBox->Checked;
        if (m_pmMaskPath)
        {
            // If we have a selected mask, we need to compute its inverse
            TabControl* pmTab = 
                dynamic_cast<TabControl*>(pmCheckBox->Parent->Parent->Parent);
            ComboBox* pmCombo = 
                dynamic_cast<ComboBox*>
                (pmTab->TabPages->get_Item(0)->Controls->
                get_Item(S"MainOptionPanel")->Controls->get_Item(S"MaskPath"));
            
            SetAndConvertMaskFile(pmCombo, m_pmMaskPath);
        }
    }
    else if (String::Compare(pmCheckBox->Name, S"BiFilterCheckBox") == 0)
    {
        // the bilinear check box has changed
        m_bUseFiltering = pmCheckBox->Checked;
    }
    else if (String::Compare(pmCheckBox->Name, S"CamFollowCheckBox") == 0)
    {
        // the camera alignement check box has changed
        m_bUseAlignToCamera = pmCheckBox->Checked;
    }
}
//---------------------------------------------------------------------------
System::Void MBrushTypeMask::Slider_ValueChanged(System::Object*  sender,
    System::EventArgs*  e)
{
    NI_UNUSED_ARG(e);

    TrackBar* pmTrackBar = dynamic_cast<TrackBar*>(sender);

    if (String::Compare(pmTrackBar->Name, S"SliderSize") == 0)
    {
        // the side length slider has changed
        m_fSideLength = (float)pmTrackBar->Value;
        NumericUpDown* pmControl = dynamic_cast<NumericUpDown*>(pmTrackBar->
            Parent->Controls->get_Item(S"BrushSize"));
        pmControl->Value = m_fSideLength;

        m_fSizeInner = (m_fSideLength);
        m_fSizeOuter = ((float) sqrt(2.0f) * (m_fSideLength));
        Randomize();
    }
    else if (String::Compare(pmTrackBar->Name, S"SliderRotation") == 0)
    {
        // the rotation slider has changed
        m_fRotationAngle = ((float)(pmTrackBar->Value) * NI_PI) / 180.0f;
        NumericUpDown* pmControl = dynamic_cast<NumericUpDown*>(pmTrackBar->
            Parent->Controls->get_Item(S"BrushRotation"));
        pmControl->Value = (float)(pmTrackBar->Value);
        Randomize();
    }
    else if (String::Compare(pmTrackBar->Name, S"ColorSlider") == 0) 
    {
        // the neutral color slider has changed
        m_uiNeutralColor = pmTrackBar->Value;
        NumericUpDown* pmControl = dynamic_cast<NumericUpDown*>(pmTrackBar->
            Parent->Controls->get_Item(S"ColorUpDown"));
        Panel* pmColor = dynamic_cast<Panel*>(pmTrackBar->
            Parent->Controls->get_Item(S"ColorDisplay"));
        pmColor->BackColor = System::Drawing::Color::FromArgb(
            m_uiNeutralColor, m_uiNeutralColor, m_uiNeutralColor);
        pmControl->Value = (float)(pmTrackBar->Value);
    }

}
//---------------------------------------------------------------------------
System::Void MBrushTypeMask::Button_Click(System::Object*  sender,
    System::EventArgs*  e)
{
    NI_UNUSED_ARG(e);

    Control* pmControl = dynamic_cast<Control*>(sender); 
    
    if (String::Compare(pmControl->Name, S"MaskButton") == 0)
    {
        // the import button was clicked
        // Import an image as a height-map into the scene.
        OpenFileDialog* pmOpenFileDialog = new OpenFileDialog();
        pmOpenFileDialog->Filter = "Supported Brush Mask Files "
            "(*.bmp;*.tga;*.dds)|*.bmp;*.tga;*.dds|"
            "Bitmap files (*.bmp)|*.bmp|"
            "Targa files (*.tga)|*.tga|"
            "DirectX files (*.dds)|*.dds|"
            "All files (*.*)|*.*";
        pmOpenFileDialog->FilterIndex = 1;
        pmOpenFileDialog->CheckFileExists = true;
        pmOpenFileDialog->Title = "Import Brush Mask";

        if (pmOpenFileDialog->ShowDialog() != DialogResult::OK)
            return;

        NiFixedString kFileName = MStringToCharPointer(pmOpenFileDialog->FileName);    
        
        if (SetAndConvertMaskFile(pmControl, kFileName))
        {
            ComboBox* pmHeightMapPath = dynamic_cast<ComboBox*>(pmControl->
                Parent->Controls->get_Item(S"MaskPath"));
            m_pmMaskPath = kFileName;
            AddItem(kFileName, pmHeightMapPath);
        }
    }
    else if(String::Compare(pmControl->Name, S"RandButton") == 0)
    {
        // the randomize button was pressed
        Randomize();
    }
}
//---------------------------------------------------------------------------
