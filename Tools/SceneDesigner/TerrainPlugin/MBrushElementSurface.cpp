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
#include <NiFileName.h>

#include "MTerrainPlugin.h"
#include "MBrushElementSurface.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

MBrushElementSurface::MBrushElementSurface() :
    m_pkCurSurface(NULL),
    m_bPreventUpdate(false)
{
    m_pmPackageBox = 
        new System::Windows::Forms::ComboBox();
    m_pmPackageBox->SelectedIndexChanged += new System::EventHandler
        (this, &MBrushElementSurface::ComboBox_SelectedIndexChange);

    m_pmSurfaceBox = 
        new System::Windows::Forms::ComboBox();
    m_pmSurfaceBox->SelectedIndexChanged += new System::EventHandler
        (this, &MBrushElementSurface::ComboBox_SelectedIndexChange);
        
    m_pmPicture = 
        new System::Windows::Forms::PictureBox();

    m_fMinimumValue = 0;
    m_fMaximumValue = 100;

    m_fMinHeight = -100.0f;
    m_fMaxHeight = 100.0f;
    m_fMinSlope = 0.0f;
    m_fMaxSlope = 90.0f;
    m_bUseHeight = false;
    m_bUseSlope = false;
}
//---------------------------------------------------------------------------
MBrushElementSurface::MBrushElementSurface(MBrushElementSurface* pkToCopy) :
    m_bPreventUpdate(false)
{
    m_pmPackageBox = 
        new System::Windows::Forms::ComboBox();
    m_pmPackageBox->SelectedIndexChanged += new System::EventHandler
        (this, &MBrushElementSurface::ComboBox_SelectedIndexChange);

    m_pmSurfaceBox = 
        new System::Windows::Forms::ComboBox();
    m_pmSurfaceBox->SelectedIndexChanged += new System::EventHandler
        (this, &MBrushElementSurface::ComboBox_SelectedIndexChange);
        
    m_pmPicture = 
        new System::Windows::Forms::PictureBox();

    this->m_pkCurSurface = pkToCopy->GetSurface();
    this->m_pmName = pkToCopy->m_pmName;
    this->Update(NULL);    

    m_fMinimumValue = 0;
    m_fMaximumValue = 100;
    m_fMinHeight = pkToCopy->m_fMinHeight;
    m_fMaxHeight = pkToCopy->m_fMaxHeight;
    m_fMinSlope = pkToCopy->m_fMinSlope;
    m_fMaxSlope = pkToCopy->m_fMaxSlope;
    m_bUseHeight = pkToCopy->m_bUseHeight;
    m_bUseSlope = pkToCopy->m_bUseSlope;
}
//---------------------------------------------------------------------------
unsigned int MBrushElementSurface::PreparePoints(const NiBound& kBound, 
    NiTerrainInteractor* pkTerrain, NiDeformablePointSet* pkPoints)
{
    if(m_pkCurSurface)
    {
        // Prepare the point map for the operation
        if (m_pkPointMap)
        {
            m_pkPointMap->RemoveAll();
            NiDelete m_pkPointMap;
            m_pkPointMap = 0;
        }

        pkPoints->SetPointType(NiDeformablePoint::MASK_PIXEL);
        NiUInt32 uiNumPoints = pkTerrain->GetPixelsInBound2D(kBound, pkPoints, 
            m_pkCurSurface, true, 0, true);

        // Threshold is 75% - so multiply it by 0.75
        NiUInt32 uiNumBuckets;
        m_pkPointMap = NiNew NiTMap<const NiPoint3*, float, 
            PointLocationKeyMapFunctors, PointLocationKeyMapFunctors>(1);
        uiNumBuckets = NiMax(
            NiMin(uiNumPoints - (uiNumPoints >> 2), 524287), 7);
        m_pkPointMap->Resize(uiNumBuckets);

        return uiNumPoints;
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
void MBrushElementSurface::Update(MBrushOperation* pmOperation)
{    
    NiSurfacePalette* pkPalette = NiSurfacePalette::GetInstance();
    
    bool bPackages = false;
    NiTPrimitiveSet<NiSurfacePackage*> kPackages;
    pkPalette->GetLoadedPackages(kPackages);

    // Stop the drop downs updating while we repopulate
    m_bPreventUpdate = true;

    // Make sure current surface is correct
    const NiSurface* pkSelectedSurface = 0;
    if (m_pmPackageBox->SelectedItem && m_pmSurfaceBox->SelectedItem)
    {
        pkSelectedSurface = pkPalette->GetSurface
            (MStringToCharPointer(dynamic_cast<String*>(
                m_pmPackageBox->SelectedItem)), 
            (MStringToCharPointer(dynamic_cast<String*>(
                m_pmSurfaceBox->SelectedItem))));
    }
    else if (m_pkCurSurface)
    {
        pkSelectedSurface = m_pkCurSurface;

        if (!m_pmPicture->Image)
        {
            // Force loading of image
            m_pkCurSurface = 0;
        }
    }

    m_pmPackageBox->SelectedItem = 0;
    m_pmSurfaceBox->SelectedItem = 0;    
    m_pmPackageBox->Items->Clear();
    m_pmSurfaceBox->Items->Clear();    

    for(unsigned int i = 0; i < kPackages.GetSize(); ++i)
    {
        NiSurfacePackage *pkPackage = kPackages.GetAt(i);
        m_pmPackageBox->Items->Add(new String(pkPackage->GetName()));
        bPackages = true;
    }

    if(bPackages && pkSelectedSurface)
    {
        m_pmPackageBox->SelectedItem = 
            new String(pkSelectedSurface->GetPackage()->GetName());

        m_pmSurfaceBox->SelectedItem = 0;

        // Re-enable the delegate
        m_bPreventUpdate = false;

        m_pmSurfaceBox->SelectedItem = 
            new String(pkSelectedSurface->GetName());
    }
    else
    {
        // Re-enable the delegate
        m_bPreventUpdate = false;

        if (m_pmSurfaceBox->Parent)
        {
            SetIcon(NULL, dynamic_cast<MUCBrushElement*>
                (m_pmSurfaceBox->Parent->Parent));
        }
    }

    if(pmOperation && m_pmPicture)
    {
        pmOperation->m_pmOperationIcon = m_pmPicture->Image;
    }
}
//---------------------------------------------------------------------------
void MBrushElementSurface::GetOperationFullPanel(
    System::Windows::Forms::Panel* pmPanel,
    bool bInList,
    MBrushOperation* pmOperation)
{
    NI_UNUSED_ARG(pmOperation);
    System::Windows::Forms::Label* pmLabel2 = 
        new System::Windows::Forms::Label();
    
    System::Windows::Forms::Label* pmLabel3 = 
        new System::Windows::Forms::Label();
    
    int iNextPropertyY = 0;
    if (pmPanel->Controls->Count)
    {
           iNextPropertyY = pmPanel->Controls->get_Item(
                pmPanel->Controls->Count - 1)->Location.Y + 
            pmPanel->Controls->get_Item(
                pmPanel->Controls->Count - 1)->Height + 5;
    }
    int iNextPropertyX = 0;

    m_bInList = bInList;
    
    if(!bInList)
    {
        m_pmPicture->Location = System::Drawing::Point(iNextPropertyX, 
            iNextPropertyY);
        m_pmPicture->Name = S"Picture";
        m_pmPicture->BorderStyle = BorderStyle::FixedSingle;
        m_pmPicture->Size = System::Drawing::Size(50, 39);    
    
        iNextPropertyX += m_pmPicture->Width + 10;
    }
    
    pmLabel2->Text = S"Packages";
    pmLabel2->Size = Size(80,18);
    pmLabel2->AutoSize = true;        
    pmLabel2->Location = Point(iNextPropertyX, iNextPropertyY);

    if(!bInList)
    {
        iNextPropertyY += pmLabel2->Height;
    }
    else
    {
        iNextPropertyX += pmLabel2->Width;
    }

    m_pmPackageBox->Name = S"Packages";
    m_pmPackageBox->DropDownStyle = 
        System::Windows::Forms::ComboBoxStyle::DropDownList;
    m_pmPackageBox->FormattingEnabled = true;
    m_pmPackageBox->Location = System::Drawing::Point(iNextPropertyX, 
        iNextPropertyY);
    m_pmPackageBox->Size = System::Drawing::Size(85, 20);

    iNextPropertyY += m_pmPackageBox->Height + 5;

    if(!bInList)
    {
        iNextPropertyX = m_pmPicture->Width + 10;
    }
    else
    {
        iNextPropertyX = 0;
    }
    

    pmLabel3->Text = S"Materials";
    pmLabel3->Size = Size(80,18);
    pmLabel3->AutoSize = true;        
    pmLabel3->Location = Point(iNextPropertyX, iNextPropertyY);

    if(!bInList)
    {
        iNextPropertyY += pmLabel3->Height;
    }
    else
    {
        iNextPropertyX += pmLabel3->Width;
    }

    m_pmSurfaceBox->Name = S"Materials";
    m_pmSurfaceBox->DropDownStyle = 
        System::Windows::Forms::ComboBoxStyle::DropDownList;
    m_pmSurfaceBox->FormattingEnabled = true;
    m_pmSurfaceBox->Location = System::Drawing::Point(iNextPropertyX, 
        iNextPropertyY);
    m_pmSurfaceBox->Size = System::Drawing::Size(85, 20);

    if(!bInList)
    {
        pmPanel->Controls->Add(m_pmPicture);
    }   

    pmPanel->Controls->Add(pmLabel2);
    pmPanel->Controls->Add(m_pmPackageBox);
    pmPanel->Controls->Add(pmLabel3);
    pmPanel->Controls->Add(m_pmSurfaceBox);
    
    __super::GetOperationFullPanel(pmPanel, bInList, pmOperation);    

}
//---------------------------------------------------------------------------
void MBrushElementSurface::ReadValues(MBrushElement* pmElement)
{
    MBrushElement::ReadValues(pmElement);
    
    MBrushElementSurface* pmSurfaceElement = 
        dynamic_cast<MBrushElementSurface*>(pmElement);

    if (!pmSurfaceElement)
        return;

    if (m_pmPackageBox && m_pmPackageBox->Items->Count)
    {
        m_bPreventUpdate = true;
        m_pmPackageBox->SelectedIndex = 
            pmSurfaceElement->m_pmPackageBox->SelectedIndex;
        m_bPreventUpdate = false;
        m_pmSurfaceBox->SelectedIndex =
            pmSurfaceElement->m_pmSurfaceBox->SelectedIndex;
    }
    else
    {
        m_pkCurSurface = pmSurfaceElement->m_pkCurSurface;
    }
}
//---------------------------------------------------------------------------
void MBrushElementSurface::GetPointSpacing(float& fSpacing)
{
    MEntity* pmEntity;
    NiTerrainInteractor* pkInteractor;
    size_t fSize = 0;
    MTerrainPlugin::GetInstance()->GetTerrainEntity(pmEntity);

    pmEntity->GetNiEntityInterface()->GetPropertyData("Interactor",
        (void*&)pkInteractor, fSize, 0);

    NiUInt32 uiNumLOD = pkInteractor->GetTerrain()->GetNumLOD();

    pkInteractor->GetPointSpacing(NiDeformablePoint::MASK_PIXEL, uiNumLOD,
        fSpacing);
}
//---------------------------------------------------------------------------
void MBrushElementSurface::GetOptionInformation(
    System::Windows::Forms::Label* pmElementDesc)
{
    if(m_pkCurSurface)
    {
        String* pmText = String::Concat(pmElementDesc->Text, 
            S"Material: ", 
            new String(m_pkCurSurface->GetName()));

        pmElementDesc->Text = pmText;
    }
}
//---------------------------------------------------------------------------
float MBrushElementSurface::GetValue(NiTerrainInteractor* pkTerrain, 
    const NiPoint3& kWorldLocation)
{
    unsigned char ucOpacity;
    NiRay kRay(kWorldLocation,NiPoint3(0,0,-1));

    pkTerrain->GetSurfaceOpacity(kRay, m_pkCurSurface, ucOpacity, 0);

    return (float)(ucOpacity);
}
//---------------------------------------------------------------------------
void MBrushElementSurface::Do_Dispose(bool bDisposing)
{
    NI_UNUSED_ARG(bDisposing);
    m_pmPackageBox = 0;
    m_pmSurfaceBox = 0;
    m_pmPicture = 0;
}
//---------------------------------------------------------------------------
System::Void MBrushElementSurface::ComboBox_SelectedIndexChange
    (System::Object* sender, System::EventArgs* e)
{
    NI_UNUSED_ARG(e);
    ComboBox* pmBox = dynamic_cast<ComboBox*>(sender);

    // Get the palette
    NiSurfacePalette* pkPalette = NiSurfacePalette::GetInstance();
    if(!pkPalette)
        return;

    Panel* pmParent = dynamic_cast<Panel*>(pmBox->Parent);

    if(pmBox == m_pmPackageBox && pmBox->SelectedItem)
    {
        // If the event is called by the package combo box
        NiTPrimitiveSet<NiSurface*> kSurfaces;

        String *pmItem = dynamic_cast<String*> (pmBox->SelectedItem);
        NiSurfacePackage *pkPackage = pkPalette->GetPackage
            (MStringToCharPointer(pmItem));

        if(pkPackage==0)
        {
            return;
        }

        // Load the surfaces of the now selected package in the surface box
        pkPackage->GetLoadedSurfaces(kSurfaces);

        //Add the list of items
        m_pmSurfaceBox->SelectedItem = 0;
        m_pmSurfaceBox->Items->Clear();
        for(unsigned int i = 0; i < kSurfaces.GetSize(); ++i)
        {
            NiSurface *pkSurface = kSurfaces.GetAt(i);
            m_pmSurfaceBox->Items->Add(new String(pkSurface->GetName()));
        }

        if (m_pmSurfaceBox->Items->Count)
            m_pmSurfaceBox->SelectedIndex = 0;
    }
    else if(pmBox == m_pmSurfaceBox && pmBox->SelectedItem)
    {

        // Event was thrown by the surface combo box
        NiSurface* pkSurface = pkPalette->GetSurface
            (MStringToCharPointer(dynamic_cast<String*>
            (m_pmPackageBox->SelectedItem)), (MStringToCharPointer
            (dynamic_cast<String*>(pmBox->SelectedItem))));

        if(pkSurface)
        {
            m_pkCurSurface = pkSurface;

            // Dont load an image if we aren't added to the operation panel
            if (!m_bInList)
                return;

            if (m_bPreventUpdate)
                return;

            // Is the surface compiled
            if (!pkSurface->IsCompiled())
            {
                pkSurface->CompileSurface();
            }

            SetIcon(pkSurface, 
                dynamic_cast<MUCBrushElement*>(pmParent->Parent));
        }
    }
    else
    {
        return;
    }
}
//---------------------------------------------------------------------------
void MBrushElementSurface::SetIcon(NiSurface* pkSurface, 
    MUCBrushElement* pmPanel)
{
    // Display the texture in the preview window:
    Image *pmSurfImage = 0;
    NiDevImageConverter kImageConverter;
    NiPixelDataPtr spSrcPixData;
    NiPixelDataPtr spPixData;

    if (pkSurface != NULL)
    {
        NiFixedString kMapFile;
        pkSurface->GetMapFile(NiSurface::SURFACE_MAP_DIFFUSE,kMapFile);

        //Figure out the package path
        NiString kPackageDir = pkSurface->GetPackage()->GetFilename();
        NiFilename kPackagePath = kPackageDir;
        kPackageDir = kPackageDir.Left(kPackageDir.Length() - 
            (strlen(kPackagePath.GetFilename())) - 
            (strlen(kPackagePath.GetExt())));

        if(kMapFile)
        {
            kMapFile = kPackageDir + kMapFile;
            spSrcPixData = kImageConverter.ReadImageFile(kMapFile,0);
        }
        if(spSrcPixData)
        {
            spPixData = kImageConverter.ConvertPixelData
                (*spSrcPixData,NiPixelFormat::BGR888,0,false);
            System::Drawing::Imaging::PixelFormat mPixFormat;
            mPixFormat = 
                System::Drawing::Imaging::PixelFormat::Format24bppRgb;
            pmSurfImage = new Bitmap(
                spPixData->GetWidth(),
                spPixData->GetHeight(),
                spPixData->GetPixelStride() * spPixData->GetWidth(),
                mPixFormat,
                spPixData->GetPixels()
                ); 
        }
    }

    if(pmSurfImage == 0)
    {
        //Load a default "NON" texture?
        System::ComponentModel::ComponentResourceManager* 
            pmResourceManager = MTerrainPlugin::GetResourceManager();

        pmSurfImage = dynamic_cast<Image*>
            (pmResourceManager->GetObject("SurfaceMapUnavailable"));
    }
    pmSurfImage = new Bitmap(pmSurfImage);

    if(m_pmPicture != 0)
    {
        m_pmPicture->Image = pmSurfImage;
    }

    if (pmPanel)
        pmPanel->SetPicture(pmSurfImage);
}
//----------------------------------------------------------------------------
bool PointLocationKeyMapFunctors::IsKeysEqual(
    const NiPoint3* pkKey1, const NiPoint3* pkKey2)
{   
    return (*pkKey1 == *pkKey2);
}
//---------------------------------------------------------------------------
unsigned int PointLocationKeyMapFunctors::KeyToHashIndex(
    const NiPoint3* pkKey1, unsigned int uiTableSize)
{
    NIASSERT(sizeof(*pkKey1) % sizeof(NiUInt32) == 0);
    
    // Split the structure into 32bit ints, and hash them all together
    NiUInt32 uiResult = 0;
    NiUInt32* pkCurInt = (NiUInt32*)pkKey1;
    for (NiUInt32 ui = 0; ui < sizeof(*pkKey1) / sizeof(NiUInt32); ++ui)
    {
        uiResult *= 4111; // Reasonably large prime number for a basic hash
        uiResult += pkCurInt[ui];
    }

    return uiResult % uiTableSize;
}
