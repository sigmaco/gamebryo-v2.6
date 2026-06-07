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
#include "NiTerrainComponent.h"
#include "SurfaceCommandPanel.h"
#include "DlgSurfaceEditor.h"
#include "DlgNewPackage.h"
#include "MTerrainPlugin.h"

using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

System::Void SurfaceCommandPanel::EntitySelected(MEntity* pmEntity)
{
    if (pmEntity)
    {
        NiTerrainComponent *pkTerrainComponent = 0;
        NiEntityInterface *pkEntityInterface = 
            pmEntity->GetNiEntityInterface();
        for (unsigned int i=0;i<pkEntityInterface->GetComponentCount();i++)
        {
            NiEntityComponentInterface *pkComponent = 
                pkEntityInterface->GetComponentAt(i);
            if (pkComponent->GetClassName() == 
                NiFixedString("NiTerrainComponent"))
            {
                pkTerrainComponent = (NiTerrainComponent*)pkComponent;
                break;
            } 
        }
        
        if (pkTerrainComponent)
        {
            UseSurfacePalette(pkTerrainComponent->GetActiveSurfacePalette());
            return;
        }
    }

    UseSurfacePalette(NiSurfacePalette::GetInstance());
}
//---------------------------------------------------------------------------
void SurfaceCommandPanel::RefreshPackages()
{
    PopulatePackageList(0);
    PopulateSurfaceList();
    NotifyPaletteUpdate();
}
//---------------------------------------------------------------------------
void SurfaceCommandPanel::UseSurfacePalette(NiSurfacePalette* pkPalette)
{
    MDisposeRefObject(m_pkPalette);

    if (pkPalette)
    {
        m_pkPalette = pkPalette;
        MInitRefObject(m_pkPalette);

        PopulatePackageList(0);
        PopulateSurfaceList();
    }
}
//---------------------------------------------------------------------------
void SurfaceCommandPanel::PopulatePackageList(
    const NiSurfacePackage* pSelectedPackage)
{
    if (!m_pkPalette)
        return;

    NiTPrimitiveSet<NiSurfacePackage*> kPackages;
    m_pkPalette->GetLoadedPackages(kPackages);

    cbSelectedPackage->BeginUpdate();
    cbSelectedPackage->Items->Clear();
    cbSelectedPackage->Items->Add(new String("All Packages"));
    cbSelectedPackage->SelectedIndex = 0;

    for (unsigned int i = 0; i < kPackages.GetSize(); ++i)
    {
        NiSurfacePackage *pkPackage = kPackages.GetAt(i);
        cbSelectedPackage->Items->Add(new String(pkPackage->GetName()));

        if (pSelectedPackage && pSelectedPackage == pkPackage)
            cbSelectedPackage->SelectedIndex = 
            cbSelectedPackage->Items->Count - 1;
    }

    // Check that the current selection is still valid:
    if (cbSelectedPackage->SelectedIndex == 0)
    {
        if (!cbSelectedPackage->Items->Contains(
            cbSelectedPackage->SelectedText))
        {
            cbSelectedPackage->SelectedIndex = 0;
        }
    }

    cbSelectedPackage->EndUpdate();
}
//---------------------------------------------------------------------------
void SurfaceCommandPanel::PopulateSurfaceList(void)
{
    if (!m_pkPalette)
        return;

    NiTPrimitiveSet<NiSurface*>* kSurfaces = NULL;
    
    // Clear any surfaces currently present
    listSurface->BeginUpdate();
    listTags->BeginUpdate();
    listSurface->Items->Clear();
    listTags->Items->Clear();
    imgSurfaceList->Images->Clear();
    imgSurfaceListLarge->Images->Clear();
    m_cbMoreTags->Items->Clear();
    m_cbMoreTags->AutoCompleteCustomSource->Clear();
 
    // Get a list of all the surfaces with these tags
    NiTPrimitiveSet<NiSurface*> kSurfacesWithTags;
    NiTList<NiFixedString> kTags;
    for (int i=0; i<flowTags->Controls->Count; ++i)
    {
        System::Windows::Forms::Control *pkItem = 
            flowTags->Controls->get_Item(i);
        if (!pkItem->Name->StartsWith("__"))
        {
            kTags.AddHead(MStringToCharPointer(pkItem->Name));
        }
    }
    
    m_pkPalette->GetSurfaces(kTags,kSurfacesWithTags,true);

    // Now get a list of all the surfaces in the selected package
    NiTPrimitiveSet<NiSurface*> kSurfacesFromPackage;
    if (cbSelectedPackage->SelectedIndex != 0)
    {
        NiFixedString kPackageName = 
            MStringToCharPointer(cbSelectedPackage->Text);
        NiSurfacePackage* pkPackage = m_pkPalette->GetPackage(kPackageName);

        if (pkPackage)
        {
            pkPackage->GetLoadedSurfaces(kSurfacesFromPackage);
        }
    }

    // There are four cases:
    // Case 0: Package = Any, Tags = Any
    //   .: Get ALL surfaces
    // Case 1: Package = Any, Tags = Some
    //   .: Show all the tag ones  
    // Case 2: Package = One, Tags = Any
    //   .: Show all the package ones
    // Case 3: Package = One, Tags = Some
    //   .: Show intersection of groups
    char cCase = 0;
    if (cbSelectedPackage->SelectedIndex != 0)
        cCase += 2;
    if (kTags.GetSize() != 0)
        cCase += 1;
    
    switch (cCase)
    {
    case 0: // Package = Any, Tags = Any .: Get ALL surfaces
        {
            NiTPrimitiveSet<NiSurfacePackage*> packageSet;
            m_pkPalette->GetLoadedPackages(packageSet);

            for (unsigned int i = 0; i < packageSet.GetSize(); ++i)
            {
                NiSurfacePackage* pkPackage = packageSet.GetAt(i);
                if (pkPackage == 0)
                {
                    continue;
                }

                pkPackage->GetLoadedSurfaces(kSurfacesFromPackage);
            }
            kSurfaces = &kSurfacesFromPackage;
            break;
        }
    case 1: // Package = Any, Tags = Some .: Show all the tag ones  
        {
            kSurfaces = &kSurfacesWithTags;
            break;
        }
    case 2: // Package = One, Tags = Any .: Show all the package ones
        {
            kSurfaces = &kSurfacesFromPackage;
            break;
        }
    case 3: // Package = One, Tags = Some .: Show intersection of groups
        {
            // Figure out the smallest set to iterate over:
            NiTPrimitiveSet<NiSurface*>* kSmallestSet;
            NiTPrimitiveSet<NiSurface*>* kLargestSet;
            if(kSurfacesWithTags.GetSize() < kSurfacesFromPackage.GetSize())
            {
                kSmallestSet = &kSurfacesWithTags;
                kLargestSet = &kSurfacesFromPackage;
            }
            else
            {
                kSmallestSet = &kSurfacesFromPackage;
                kLargestSet = &kSurfacesWithTags;
            }
            
            // Iterate over the lists and look for items that are in both
            for (unsigned int i = 0; i < kSmallestSet->GetSize(); ++i)
            {
                if (kLargestSet->Find(kSmallestSet->GetAt(i)) < 0)
                {
                    kSmallestSet->ReplaceAt(i,0); // Replace this entry with 0
                }
            }
            kSurfaces = kSmallestSet;
            break;
        }
    default:
        NIASSERT(!"Populate Surface List - Unknown case encountered");
        return;
    }

    // Display the list of surfaces:
    for (unsigned int i = 0; i < kSurfaces->GetSize(); ++i)
    {
        NiSurface *pkSurface = kSurfaces->GetAt(i);
        if(pkSurface == 0)continue;

        const NiSurfacePackage *pkPackage = pkSurface->GetPackage();
        
        // Figure out the package path
        NiString kPackageDir = pkSurface->GetPackage()->GetFilename();
        NiFilename kPackagePath = kPackageDir;
        kPackageDir = kPackageDir.Left(kPackageDir.Length() - 
            (strlen(kPackagePath.GetFilename())) - 
            (strlen(kPackagePath.GetExt())));
        
        // Setup the icon:
        NiFixedString kMapFile;
        NiDevImageConverter kImageConverter;
        NiPixelDataPtr spSrcPixData;
        NiPixelDataPtr spPixData;
        Image *pmSurfImage = 0;

        pkSurface->GetMapFile(NiSurface::SURFACE_MAP_DIFFUSE,kMapFile);
        if (kMapFile)
        {
            kMapFile = kPackageDir + kMapFile;
            spSrcPixData = kImageConverter.ReadImageFile(kMapFile,0);
        }
        if (spSrcPixData)
        {
            spPixData = kImageConverter.ConvertPixelData(
                *spSrcPixData,NiPixelFormat::BGR888,0,false);
            System::Drawing::Imaging::PixelFormat mPixFormat;
            mPixFormat = System::Drawing::Imaging::PixelFormat::Format24bppRgb;
            pmSurfImage = new Bitmap(
                spPixData->GetWidth(),
                spPixData->GetHeight(),
                spPixData->GetPixelStride() * spPixData->GetWidth(),
                mPixFormat,
                spPixData->GetPixels()
                ); 
        }
        if (pmSurfImage == 0)
        {
            // Load a default "NON" texture?
            System::ComponentModel::ComponentResourceManager* resourceManager 
                =  MTerrainPlugin::GetResourceManager();

            pmSurfImage = dynamic_cast<Image*>(
                resourceManager->GetObject("SurfaceMapUnavailable"));
        }

        pmSurfImage = new Bitmap(pmSurfImage);
        imgSurfaceList->Images->Add(pmSurfImage);
        imgSurfaceListLarge->Images->Add(pmSurfImage);
           
        // Setup the actual item in the list:
        ListViewItem *pkItem = new ListViewItem(pkSurface->GetName());
        pkItem->set_ImageIndex(i);
        pkItem->SubItems->Add(pkPackage->GetName());
        pkItem->SubItems->Add(pkSurface->GetDescription());
        
        pkItem->ToolTipText = NiString(pkSurface->GetName()) + 
            " (" + NiString(pkPackage->GetName()) + ")\n" + 
            NiString(pkSurface->GetDescription());;

        listSurface->Items->Add(pkItem);

        // Add any appropriate tags:
        NiTObjectSet<NiFixedString> kAvailableTags;
        pkSurface->GetTags(kAvailableTags);

        for (unsigned int iTag = 0; iTag < kAvailableTags.GetSize(); ++iTag)
        {
            NiFixedString kTag = kAvailableTags.GetAt(iTag);

            if (!listTags->Items->ContainsKey(kTag) && 
                !flowTags->Controls->ContainsKey(kTag))
            {
                ListViewItem *pkTagItem = 
                    listTags->Items->Add(kTag,kTag,"Tag");
                pkTagItem->ToolTipText = 
                    "Double click to filter results by this tag";

                m_cbMoreTags->AutoCompleteCustomSource->Add(new String(kTag));
                m_cbMoreTags->Items->Add(new String(kTag));
            }
        }
    }

    // Add in a summary of the tags found:
    if (listTags->Items->Count == 0)
    {
        // Say "No tags found"
        lbTagSummary->Text = "No subtags found";
    }
    else
    {
        // Say "found X subtags
        lbTagSummary->Text = String::Concat(
            S"Found ",
            Convert::ToString(listTags->Items->Count),
            S" subtags");
    }
    
    // Resize the columns for the surfaces
    if (listSurface->View == View::List)
    {
        cmSurfaceSmallIcon_Click(0,0);
    }
    
    // Resize the columns for the tags
    if (listTags->View == View::List)
    {
        // Resize the columns
        listTags->BeginUpdate();
        listTags->View = View::Details;
        listTags->AutoResizeColumn(
            0,
            ColumnHeaderAutoResizeStyle::ColumnContent);
        int autoWidth = listTags->Columns->get_Item(0)->Width;

        // Use this size in the list mode:
        listTags->View = View::List;
        listTags->Columns->get_Item(0)->Width = autoWidth;
        listTags->EndUpdate();
    }

    listSurface->EndUpdate();
    listTags->EndUpdate();
}   
//---------------------------------------------------------------------------
void SurfaceCommandPanel::NotifyPaletteUpdate()
{
    TerrainCommandPanel *pmTerrainPanel;
    pmTerrainPanel = MTerrainPlugin::GetInstance()->GetTerrainCommandPanel();
    pmTerrainPanel->Palette_Update();
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::SurfaceCommandPanel_Load(
    System::Object*  sender, System::EventArgs*  e)
{
    SetClean();

    // Initialise the surface palette
    listSurface_SelectedIndexChanged(sender,e);
    UseSurfacePalette(NiSurfacePalette::GetInstance());

    // Initialise the tags and surfaces
    listSelectedTags_Click(sender,e);
    cbSelectedPackage_SelectedIndexChanged(sender,e);
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::tsSurfaceNew_Click(
    System::Object*, System::EventArgs*) 
{
    if (!m_pkPalette)
        return;

    NiSurface* pkSurface = NiNew NiSurface(NiMetaDataStore::GetInstance());
    
    // Open the surface editor:
    if (pkSurface)
    {
        DlgSurfaceEditor *pmSurfaceEditor = new DlgSurfaceEditor();
        pmSurfaceEditor->Text = "New Material";
        pmSurfaceEditor->InitialiseSurface(m_pkPalette, pkSurface,
            cbSelectedPackage->Text);

        if (pmSurfaceEditor->ShowDialog(this) == DialogResult::OK)
        {
            // Repopulate everything:
            PopulateSurfaceList();
            SetDirty();
        }
    }

    NotifyPaletteUpdate();
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::tsSurfaceEdit_Click(
    System::Object*, System::EventArgs*) 
{
    //Check for a selected surface:
    NiSurface* pkSurface = 0;
    if (listSurface->SelectedItems->Count > 0)
    {
        ListViewItem *pkItem = listSurface->SelectedItems->get_Item(0);
        ListViewItem::ListViewSubItem *pkSubItem = 
            pkItem->SubItems->get_Item(1);
        NiSurfacePackage* pkPackage = m_pkPalette->GetPackage(
            MStringToCharPointer(pkSubItem->Text));
        
        if (pkPackage)
        {
            pkPackage->GetSurface(
                MStringToCharPointer(pkItem->Text),
                pkSurface);
        }
    }

    // Open the surface editor:
    if (pkSurface)
    {
        DlgSurfaceEditor *pmSurfaceEditor = new DlgSurfaceEditor();
        pmSurfaceEditor->InitialiseSurface(m_pkPalette, pkSurface, "");
        
        if (pmSurfaceEditor->ShowDialog(this) == DialogResult::OK)
        {
            // Tell any terrain using the given surface to update with 
            // the modified surface
            MEntity* apmEntities[] = 
                MFramework::Instance->Scene->GetEntities();
            NiEntityInterface* pkEntity;
            NiTerrainInteractor* pkInteractor;
            size_t stSize;
            for (int i = 0; i < apmEntities->Count; ++i)
            {
                pkEntity = apmEntities[i]->GetNiEntityInterface();

                if (pkEntity->GetPropertyData("Interactor", 
                    (void*&)pkInteractor, stSize, 0))
                {
                    pkInteractor->MarkSurfaceChanged(pkSurface);
                }
            }

            // Repopulate everything:
            PopulateSurfaceList();        
            SetDirty();
        }
    }
    else
    {
        NiMessageBox("Select a Material please", "No material selected");
    }

    NotifyPaletteUpdate();
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::tsSurfaceDelete_Click(
    System::Object*, System::EventArgs*) 
{
    //Check to see if a surface has been selected:
    NiSurface* pkSurface = 0;
    if (listSurface->SelectedItems->Count > 0)
    {
        ListViewItem *pkItem = listSurface->SelectedItems->get_Item(0);
        ListViewItem::ListViewSubItem *pkSubItem = 
            pkItem->SubItems->get_Item(1);
        NiSurfacePackage *pkPackage = 
            m_pkPalette->GetPackage(MStringToCharPointer(pkSubItem->Text));

        if (pkPackage)
        {
            pkPackage->GetSurface(
                MStringToCharPointer(pkItem->Text),pkSurface);
        }
    }

    if (pkSurface==0)
    {
        return;
    }

    // Notify user that deleting a surface from a package will remove it from 
    // all terrains currently loaded. And possibly cause other terrains that 
    // use this package to be displayed incorrectly...

    if (MessageBox::Show("Warning: Deleting a material "\
        "that is used by a terrain will cause it to load incorrectly. \n"\
        "Please make sure that no terrains use this material before "\
        "deleting it.", "Warning: Material Deletion",
        MessageBoxButtons::OKCancel, 
        MessageBoxIcon::Warning) == DialogResult::Cancel)
    {
        return;
    }

    //Find terrain entities
    MEntity* pmTerrainEntity;
    MTerrainPlugin::GetInstance()->GetTerrainEntity(pmTerrainEntity);
    if (!pmTerrainEntity)
    {
        MTerrainPlugin::GetInstance()->SelectClosestTerrain();
        MTerrainPlugin::GetInstance()->GetTerrainEntity(pmTerrainEntity);

        if (!pmTerrainEntity)
        {            
            String* pmCaption = new String("No terrain found in current scene "
                "from which to remove selected material.");
            
            IMessageService* pmMessageService = MGetService(IMessageService);
            pmMessageService->AddMessage(MessageChannelType::Errors, 
                pmCaption);
        }
    }

    // if terrain was found, remove surface
    if (pmTerrainEntity)
    {
        NiTerrainInteractor* pkInteractor;
        size_t stSize;
        pmTerrainEntity->GetNiEntityInterface()->GetPropertyData(
            "Interactor", (void*&)pkInteractor, stSize, 0);

        if (pkInteractor)
        {
            pkInteractor->RemoveSurface(pkSurface);
        }
    }


    // Delete the surface:
    m_pkPalette->UnloadSurface(
        pkSurface->GetPackage()->GetName(),
        pkSurface->GetName());

    TerrainCommandPanel* pmTerrainPanel = 
        MTerrainPlugin::GetInstance()->GetTerrainCommandPanel();
    pmTerrainPanel->Surface_Remove(pkSurface);

    //Repopulate everything:
    PopulateSurfaceList();
    NotifyPaletteUpdate();
    SetDirty();
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::tsPackageNew_Click(
    System::Object*, System::EventArgs*) 
{
    // Figure out what to call the package
    DlgNewPackage* pmPackage = new DlgNewPackage();
    bool invalidInput = true;

    while (invalidInput)
    {
        invalidInput = false;
        if (pmPackage->ShowDialog()==DialogResult::Cancel)
        {
            return;
        }
        
        // Check that the values are valid:
        if (pmPackage->tbInput->Text->Trim()->CompareTo("")==0)
        {
            // Invalid package name
            NiMessageBox("A package name must be specified",
                "New Material Package");
            invalidInput = true;
        }else if (pmPackage->tbLocation->Text->Trim()->CompareTo("")==0)
        {
            // Invalid package location
            NiMessageBox("A package location must be specified",
                "New Material Package");
            invalidInput = true;
        }
    }

    // Create the package appropriately
    NiSurfacePackage* pkPackage = NiNew NiSurfacePackage(
        MStringToCharPointer(pmPackage->tbInput->Text));
    pkPackage->SetFilename(MStringToCharPointer(pmPackage->tbLocation->Text));
    pkPackage->Save();
    delete pkPackage;

    // Now load that package
    const NiSurfacePackage* pNewPackage = m_pkPalette->LoadPackage(
        MStringToCharPointer(pmPackage->tbLocation->Text));
    
    // Update everything
    PopulatePackageList(pNewPackage);
    PopulateSurfaceList();
    NotifyPaletteUpdate();
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::tsPackageLoad_Click(
    System::Object*, System::EventArgs*) 
{
    if (!m_pkPalette)
        return;

    OpenFileDialog* pmOpenFileDialog = new OpenFileDialog();

    pmOpenFileDialog->Filter = 
        "Package Files (*.xml)|*.xml|All Files (*.*)|*.*";
    pmOpenFileDialog->CheckFileExists = true;
    pmOpenFileDialog->Title = "Load a Material Package";

    if (pmOpenFileDialog->ShowDialog() == ::DialogResult::OK)
    {
        const NiSurfacePackage* pkNewPackage = m_pkPalette->LoadPackage(
            MStringToCharPointer(pmOpenFileDialog->FileName));

        if (pkNewPackage != NULL)
        {
            PopulatePackageList(pkNewPackage);
            PopulateSurfaceList();

            NotifyPaletteUpdate();
        }
        else
        {
            MessageBox::Show("The selected package file failed to load. Make "\
                "sure that it is a valid material package file.", "Error: "\
                "Package failed to load", MessageBoxButtons::OK, 
                MessageBoxIcon::Error);
        }
    }
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::tsPackageClose_Click(
    System::Object*, System::EventArgs*) 
{
    if (cbSelectedPackage->SelectedIndex==0)
    {
        return;
    }

    NiSurfacePackage *pkPackage = 
        m_pkPalette->GetPackage(MStringToCharPointer(cbSelectedPackage->Text));

    if (pkPackage==0)
    {
        return;
    }

    // Display warning to user:
    if (MessageBox::Show("Warning: Closing a package "\
        "that is used by a terrain will cause it to display incorrectly. \n"\
        "Please make sure that no terrains use this package before "\
        "closing it.\n This package will be reloaded if a terrain is loaded "\
        "that requires it.", "Warning: Closing Package",
        MessageBoxButtons::OKCancel, 
        MessageBoxIcon::Warning) == DialogResult::Cancel)
    {
        return;
    }

    m_pkPalette->UnloadPackage(pkPackage->GetName());

    // Update the lists:
    RefreshPackages();
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::tsPackageSave_Click(
    System::Object*, System::EventArgs*) 
{
    // Check for a selected package:
    if (cbSelectedPackage->SelectedIndex==0)
    {
        return;
    }

    NiSurfacePackage *pkPackage = 
        m_pkPalette->GetPackage(MStringToCharPointer(cbSelectedPackage->Text));

    if (pkPackage==0)
    {
        return;
    }

    pkPackage->Save();
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::tsPackageSaveAll_Click(
    System::Object*, System::EventArgs*) 
{
    SaveAll();
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::SaveAll()
{
    bool bAllSucceeded = true;
    NiTPrimitiveSet<NiSurfacePackage*> kPackages;
    if (!m_pkPalette)
        return;

    m_pkPalette->GetLoadedPackages(kPackages);

    for (unsigned int i = 0; i < kPackages.GetSize(); ++i)
    {
        NiSurfacePackage *pkPackage = kPackages.GetAt(i);
        if (pkPackage)
        {
            while(pkPackage->Save() == false)
            {
                char pcErrorMessage[512];
                NiSprintf(pcErrorMessage, 512, "Unable to write to file:" \
                    "\n\n    %s\n\nPermissions may be read-only. Try again?",
                    pkPackage->GetFilename());

                if (MessageBox::Show(pcErrorMessage, "File I/O Error",
                    MessageBoxButtons::YesNo, MessageBoxIcon::Error) ==
                    DialogResult::No)
                {
                    bAllSucceeded = false;
                    break;
                }
            }
        }
    }

    // only set clean on success
    if (bAllSucceeded)
        SetClean();
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::cmSurfaceEdit_Click(
    System::Object*  sender, System::EventArgs*  e)
{
    tsSurfaceEdit_Click(sender,e);
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::cmSurfaceDelete_Click(
    System::Object*  sender, System::EventArgs*  e)
{
    tsSurfaceDelete_Click(sender,e);
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::cmSurfaceRemoveCurrent_Click(
    System::Object*, System::EventArgs*)
{ 
    // Find the currently selected surface
    NiSurface* pkSurface = 0;
    if (listSurface->SelectedItems->Count > 0)
    {
        ListViewItem *pkItem = listSurface->SelectedItems->get_Item(0);
        ListViewItem::ListViewSubItem *pkSubItem = 
            pkItem->SubItems->get_Item(1);
        NiSurfacePackage* pkPackage = m_pkPalette->GetPackage(
            MStringToCharPointer(pkSubItem->Text));
        
        if (pkPackage)
        {
            pkPackage->GetSurface(
                MStringToCharPointer(pkItem->Text),
                pkSurface);
        }
    }

    if (!pkSurface)
        return;

    // Remove current surface from selected terrain
    MEntity* pmTerrainEntity;
    MTerrainPlugin::GetInstance()->GetTerrainEntity(pmTerrainEntity);
    if (!pmTerrainEntity)
    {
        MTerrainPlugin::GetInstance()->SelectClosestTerrain();
        MTerrainPlugin::GetInstance()->GetTerrainEntity(pmTerrainEntity);
        if (!pmTerrainEntity)
        {            
            String* pmCaption = new String("No terrain found in current scene "
                "from which to remove selected material.");
            
            IMessageService* pmMessageService = MGetService(IMessageService);
            pmMessageService->AddMessage(MessageChannelType::Errors, 
                pmCaption);

            return;
        }
    }

    NiTerrainInteractor* pkInteractor;
    size_t stSize;
    pmTerrainEntity->GetNiEntityInterface()->GetPropertyData("Interactor",
        (void*&)pkInteractor, stSize, 0);

    if (pkInteractor)
    {
        pkInteractor->RemoveSurface(pkSurface);
    }
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::cmSurfaceRemoveAll_Click(
    System::Object*, System::EventArgs*)
{
    // Find the currently selected surface
    NiSurface* pkSurface = 0;
    if (listSurface->SelectedItems->Count > 0)
    {
        ListViewItem *pkItem = listSurface->SelectedItems->get_Item(0);
        ListViewItem::ListViewSubItem *pkSubItem = 
            pkItem->SubItems->get_Item(1);
        NiSurfacePackage* pkPackage = m_pkPalette->GetPackage(
            MStringToCharPointer(pkSubItem->Text));
        
        if (pkPackage)
        {
            pkPackage->GetSurface(
                MStringToCharPointer(pkItem->Text),
                pkSurface);
        }
    }

    if (!pkSurface)
        return;

    // Remove current surface from all loaded terrains
    MScene* pmScene = MFramework::Instance->Scene;
    if (!pmScene)
        return;

    // Cycle through each entity in the scene, looking for one which contains
    // a terrain
    MEntity* apmEntities[] = pmScene->GetEntities();
    NiEntityInterface* pkTerrainEntity;
    NiTerrainInteractor* pkInteractor;
    size_t stSize;
    bool bFound = false;
    for (int i = 0; i < apmEntities->Count; ++i)
    {
        pkTerrainEntity = apmEntities[i]->GetNiEntityInterface();
        pkTerrainEntity->GetPropertyData("Interactor", (void*&)pkInteractor, 
            stSize, 0);

        if (pkInteractor)
        {
            pkInteractor->RemoveSurface(pkSurface);
            bFound = true;
        }
    }
    
    if (!bFound)
    {            
        String* pmCaption = new String("No terrains found in current scene "
            "from which to remove selected material.");
        
        IMessageService* pmMessageService = MGetService(IMessageService);
        pmMessageService->AddMessage(MessageChannelType::General, 
            pmCaption);
    }
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::cmSurfaceDetailView_Click(
    System::Object*, System::EventArgs*) 
{
    listSurface->View = View::Details;

    // Loop through and size each column header to fit the column header text.
    IEnumerator* iEnum = listSurface->Columns->GetEnumerator();
    while (iEnum->MoveNext())                
    {
        ColumnHeader *columnHeader;
        columnHeader = __try_cast<ColumnHeader*>(iEnum->Current);
        columnHeader->AutoResize(ColumnHeaderAutoResizeStyle::ColumnContent);
        if (columnHeader->Index == 2)
        {
            columnHeader->Width = -2;
        }
        else
        {
            columnHeader->Width = -1;
        }
    }
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::cmSurfaceLargeIcon_Click(
    System::Object*, System::EventArgs*) 
{
    listSurface->View = View::LargeIcon;
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::cmSurfaceSmallIcon_Click(
    System::Object*, System::EventArgs*) 
{
    // Figure out the width of the first column:
    listSurface->BeginUpdate();
    listSurface->View = View::Details;
    listSurface->AutoResizeColumn(
        0,
        ColumnHeaderAutoResizeStyle::ColumnContent);
    int autoWidth = listSurface->Columns->get_Item(0)->Width;

    // Use this size in the list mode:
    listSurface->View = View::List;
    listSurface->Columns->get_Item(0)->Width = autoWidth;
    listSurface->EndUpdate();
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::listSurface_SelectedIndexChanged(
    System::Object*, System::EventArgs*) 
{
    if (listSurface->SelectedItems->Count > 0)
    {
        //Tell the context menu what it should show:
        cmSurfaceEdit->Visible = true;
        cmSurfaceDelete->Visible = true;
        cmSurfaceEditSeparator->Visible = true;

        // Get a description of this surface
        ListViewItem *pkItem = listSurface->SelectedItems->get_Item(0);
        ListViewItem::ListViewSubItem *pkSubItem = 
            pkItem->SubItems->get_Item(1);
        
        NiSurface* pkSurface = 0;
        pkSurface = m_pkPalette->GetSurface(
            MStringToCharPointer(pkSubItem->Text), 
            MStringToCharPointer(pkItem->Text));
        
        if (pkSurface)
        {
            lbDescription->Text = pkSurface->GetDescription();
        }    
    }
    else
    {
        //Tell the context menu what it should show:
        cmSurfaceEdit->Visible = false;
        cmSurfaceDelete->Visible = false;
        cmSurfaceEditSeparator->Visible = false;

        lbDescription->Text = "";
    }
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::listSurface_Click(
    System::Object*, System::EventArgs*)
{
    if (!m_pkPalette)
        return;

    if (listSurface->SelectedItems->Count > 0)
    {
        ListViewItem *pkItem = listSurface->SelectedItems->get_Item(0);
        
        if (pkItem == 0)
            return;
        
        ListViewItem::ListViewSubItem *pkSubItem = 
            pkItem->SubItems->get_Item(1);
        
        if (pkSubItem == 0)
            return;
        
        NiSurfacePackage *pkPackage = m_pkPalette->GetPackage(
            MStringToCharPointer(pkSubItem->Text)
            );

        if (pkPackage==0)
            return;

        NiSurface* pkSurface = 0;
        pkPackage->GetSurface(MStringToCharPointer(pkItem->Text), pkSurface);

        if (pkSurface)
        {
            // Send a single click surface message to the terrain command panel
            TerrainCommandPanel *pmTerrainPanel;
            pmTerrainPanel = 
                MTerrainPlugin::GetInstance()->GetTerrainCommandPanel();
            pmTerrainPanel->Surface_Click(pkSurface);       
        }
    }
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::listSurface_DoubleClick(
    System::Object*, System::EventArgs*)
{
    if (!m_pkPalette)
        return;

    if (listSurface->SelectedItems->Count > 0)
    {
        ListViewItem *pkItem = listSurface->SelectedItems->get_Item(0);
        ListViewItem::ListViewSubItem *pkSubItem = 
            pkItem->SubItems->get_Item(1);
        NiSurfacePackage *pkPackage = m_pkPalette->GetPackage(
            MStringToCharPointer(pkSubItem->Text));

        if (pkPackage==0)
            return;

        NiSurface *pkSurface = 0;
        pkPackage->GetSurface(MStringToCharPointer(pkItem->Text),pkSurface);

        if (pkSurface)
        {
            // Send a double click surface message to the Terrain Command Panel
            TerrainCommandPanel *pmTerrainPanel;
            pmTerrainPanel = 
                MTerrainPlugin::GetInstance()->GetTerrainCommandPanel();
            pmTerrainPanel->Surface_DoubleClick(pkSurface);
        }
    }
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::listSurface_ColumnClick(
    System::Object*, System::Windows::Forms::ColumnClickEventArgs*  e) 
{
    // Determine if the clicked column is already the column being sorted.
    if ( e->Column == m_lvwColumnSorter->SortColumn )
    {
        // Reverse the current sort direction for this column.
        if (m_lvwColumnSorter->Order == SortOrder::Ascending)
        {
            m_lvwColumnSorter->Order = SortOrder::Descending;
        }
        else
        {
            m_lvwColumnSorter->Order = SortOrder::Ascending;
        }
    }
    else
    {
        // Set the column number to be sorted. 
        // By default, this is in ascending order.
        m_lvwColumnSorter->SortColumn = e->Column;
        m_lvwColumnSorter->Order = SortOrder::Ascending;
    }

    // Perform the sort with these new sort options.
    listSurface->Sort();
}
//---------------------------------------------------------------------------
SurfaceCommandPanel::SurfaceListViewColumnSorter::SurfaceListViewColumnSorter()
{
    // Initialize the column to '0'.
    SortColumn = 0;

    // Initialize the sort order to 'none'.
    Order = SortOrder::None;

    // Initialize the CaseInsensitiveComparer object.
    ObjectCompare = new CaseInsensitiveComparer();

}
//---------------------------------------------------------------------------
SurfaceCommandPanel::SurfaceListViewColumnSorter::
    ~SurfaceListViewColumnSorter()
{
}
//---------------------------------------------------------------------------
int SurfaceCommandPanel::SurfaceListViewColumnSorter::Compare(
    Object *x, Object *y)
{
    int compareResult;
    ListViewItem *listviewX, *listviewY;

    // Cast the objects to be compared to ListViewItem objects.
    listviewX = static_cast <ListViewItem *> (x);
    listviewY = static_cast <ListViewItem *> (y);

    // Compare the two items.
    compareResult = ObjectCompare->Compare(
        listviewX->SubItems->Item[SortColumn]->Text,
        listviewY->SubItems->Item[SortColumn]->Text);
        
    // Calculate correct return value based on object comparison.
    if (Order == SortOrder::Ascending)
    {
        return compareResult;
    }
    else if (Order == SortOrder::Descending)
    {
        return (-compareResult);
    }
    else
    {
        // Return '0' to indicate that they are equal.
        return 0;
    }
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::listTags_DoubleClick(
    System::Object*, System::EventArgs*) 
{
    if (listTags->SelectedItems->Count == 0)
    {
        return;
    }

    ListViewItem *pkItem = listTags->SelectedItems->get_Item(0);
    if (pkItem->Name->StartsWith("__"))
    {
        return;
    }

    if (flowTags->Controls->Find("__anyTag",true)){
        flowTags->Controls->RemoveByKey("__anyTag");
    }

    System::Windows::Forms::LinkLabel* btTag = 
        new System::Windows::Forms::LinkLabel();
    btTag->Name = pkItem->Text;
    btTag->Text = String::Concat(pkItem->Text, S",");
    btTag->AutoSize = true;
    btTag->Margin = System::Windows::Forms::Padding(0,2,0,0);
    btTag->Click += new System::EventHandler(this, 
        &SurfaceCommandPanel::listSelectedTags_Click); 

    flowTags->Controls->Add(btTag);
    flowTags->Controls->SetChildIndex(m_btMoreTags,flowTags->Controls->Count);

    PopulateSurfaceList();
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::listSelectedTags_Click(
    System::Object*  sender, System::EventArgs*  e) 
{
    System::Windows::Forms::MouseEventArgs *mouseEvent = 
        dynamic_cast<System::Windows::Forms::MouseEventArgs*>(e);

    // Check if Left of Right Click:
    if (mouseEvent && 
        mouseEvent->Button == System::Windows::Forms::MouseButtons::Left)
    {
        // Left Click - Set as only tag
        flowTags->Controls->Clear();
        flowTags->Controls->Add(sender);
        flowTags->Controls->Add(m_btMoreTags);
        flowTags->Controls->SetChildIndex(
            m_btMoreTags,flowTags->Controls->Count);
    }
    else
    {
        // Right click - Remove tag
        flowTags->Controls->Remove(sender);
    }

    PopulateSurfaceList();
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::btMoreTags_Click(
    System::Object*, System::EventArgs*) 
{
    flowTags->Controls->Remove(m_btMoreTags);
    flowTags->Controls->Add(m_cbMoreTags);
    this->ActiveControl = m_cbMoreTags;
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::cbMoreTags_Blur(
    System::Object*  sender, System::EventArgs*  e) 
{
    flowTags->Controls->Remove(m_cbMoreTags);
    flowTags->Controls->Add(m_btMoreTags);    

    cbMoreTags_Enter(sender,e);
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::cbMoreTags_Enter(
    System::Object*  sender, System::EventArgs*  e) 
{
    listTags->SelectedItems->Clear();
    int iIndex = listTags->Items->IndexOfKey(m_cbMoreTags->Text);
    if(iIndex >= 0)
    {
        listTags->SelectedIndices->Add(iIndex);
    }
    m_cbMoreTags->Text = "";
    listTags_DoubleClick(sender,e);     
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::cbSelectedPackage_SelectedIndexChanged(
    System::Object*, System::EventArgs*)
{
    // Check if the 0th index is selected   
    if (cbSelectedPackage->SelectedIndex == 0)
    {
        // Disable the buttons that require a package to be selected
        tsPackageClose->Enabled = false;
        tsPackageSave->Enabled = false;
    }
    else
    {
        // Enable the buttons that modify packages
        tsPackageClose->Enabled = true;
        tsPackageSave->Enabled = true;
    }

    PopulateSurfaceList();
}
//---------------------------------------------------------------------------
System::Void SurfaceCommandPanel::panTags_Paint(
    System::Object*, System::Windows::Forms::PaintEventArgs*  e) 
{
    e->Graphics->DrawRectangle(
        m_panelBorderPen,
        0,
        0,
        panTags->Width-1,
        panTags->Height-1);
}
//---------------------------------------------------------------------------
void SurfaceCommandPanel::SetDirty()
{
    m_bDirty = true;
}
//---------------------------------------------------------------------------
void SurfaceCommandPanel::SetClean()
{
    m_bDirty = false;
}
//---------------------------------------------------------------------------
bool SurfaceCommandPanel::IsDirty()
{
    return m_bDirty;
}
//---------------------------------------------------------------------------
// Begin auto generated code
System::Void SurfaceCommandPanel::panTags_Resize(System::Object*  sender, 
    System::EventArgs*  e) 
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);
    lbTagSummary->MaximumSize = System::Drawing::Size(
        panTags->Size.Width - lbTagSummary->Location.X - 1,
        lbTagSummary->Size.Height);

    panTags->Invalidate();
}
// END auto generated code
