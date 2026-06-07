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
#include "DlgSurfaceEditor.h"
#include "MTerrainPlugin.h"


using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

//---------------------------------------------------------------------------
void DlgSurfaceEditor::InitialiseSurface(NiSurfacePalette *pkPalette, 
    NiSurface *pkSurface, System::String* pmCurrentPackage) 
{    
    if (pkPalette && m_pkPalette != pkPalette)
    {
        //Populate the package list:
        NiTPrimitiveSet<NiSurfacePackage*> kPackages;
        pkPalette->GetLoadedPackages(kPackages);
        String* pmTempPackageName = cbPackage->Text;
        cbPackage->Items->Clear();
        cbPackage->AutoCompleteCustomSource->Clear();
        
        for (unsigned int i = 0; i < kPackages.GetSize(); ++i)
        {
            NiSurfacePackage *pkPackage = kPackages.GetAt(i);
            String* pmPackageName = new String(pkPackage->GetName());
            cbPackage->Items->Add(pmPackageName);
            
            if (pmPackageName->CompareTo(pmCurrentPackage) == 0 && 
                pmTempPackageName->Length == 0)
            {
                pmTempPackageName = pmPackageName;
            }
            cbPackage->AutoCompleteCustomSource->Add(pmPackageName);
        }
        cbPackage->Text = pmTempPackageName;

        //Populate the Tag list:
        NiTList<NiFixedString> kTags;
        NiTListIterator kIterator;

        tscbTag->Items->Clear();
        tscbTag->AutoCompleteCustomSource->Clear();
        pkPalette->GetIndex(kTags);
        
        kIterator = kTags.GetHeadPos();
        while(kIterator != NULL)
        {
            NiFixedString kTagName = kTags.GetNext(kIterator);
            tscbTag->Items->Add(new String(kTagName));
            tscbTag->AutoCompleteCustomSource->Add(new String(kTagName));
        }
    }

    if (pkSurface)
    {
        //Check for a valid package:
        NiSurfacePackage *pkPackage = pkSurface->GetPackage();
        if (pkPackage == 0)
        {
            if (cbPackage->Text)
            {
                pkPackage = pkPalette->GetPackage
                    (MStringToCharPointer(cbPackage->Text));
            }
        }
        else
        {
            cbPackage->Text = pkPackage->GetName();
        }

        if (pkPackage == 0)
        {
            tcpTextures->Enabled = false; tcpTextures->Visible = false;
            tcpTags->Enabled = false; tcpTags->Visible = false;
            tbName->Enabled = false;
            tbDescription->Enabled = false;
        }
        else
        {
            tcpTextures->Enabled = true; tcpTextures->Visible = true;
            tcpTags->Enabled = true; tcpTags->Visible = true;
            tbName->Enabled = true;
            tbDescription->Enabled = true;

            //Populate the properties:
            tbName->Text = pkSurface->GetName();
            tbDescription->Text = pkSurface->GetDescription();

            //Figure out the package path
            NiString kPackageDir = pkPackage->GetFilename();
            NiFilename kPackagePath = kPackageDir;
            kPackageDir = kPackageDir.Left(kPackageDir.Length() - 
                (strlen(kPackagePath.GetFilename())) - 
                (strlen(kPackagePath.GetExt())));

            //Populate the textures:
            imgTextures->Images->Clear();
            lvTextures->Items->Clear();
            for (int i=0; i<NiSurface::NUM_SURFACE_MAPS; ++i)
            {
                NiSurface::SurfaceMapID kMapID = (NiSurface::SurfaceMapID)i;
                NiFixedString kMapName;
                NiSurface::GetMapName(kMapID,kMapName);
                ListViewItem *pkItem = lvTextures->Items->Add(kMapName);
                NiFixedString kMapFile;
                pkSurface->GetMapFile(kMapID,kMapFile);
                pkItem->SubItems->Add(new String(kMapFile));

                // Display the texture in the preview window:
                Image *pmSurfImage = 0;
                NiDevImageConverter kImageConverter;
                NiPixelDataPtr spSrcPixData;
                NiPixelDataPtr spPixData;

                if (kMapFile)
                {
                    kMapFile = kPackageDir + kMapFile;
                    spSrcPixData = kImageConverter.ReadImageFile(kMapFile,0);
                }
                if (spSrcPixData)
                {
                    spPixData = kImageConverter.ConvertPixelData
                        (*spSrcPixData, NiPixelFormat::BGR888,0,false);
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
                if (pmSurfImage == 0)
                {
                    //Load a default "NON" texture?
                    System::ComponentModel::ComponentResourceManager* 
                        resourceManager = MTerrainPlugin::GetResourceManager();
                    pmSurfImage = dynamic_cast<Image*>
                        (resourceManager->GetObject("SurfaceMapUnavailable"));
                }

                imgTextures->Images->Add(new Bitmap(pmSurfImage));
                pkItem->set_ImageIndex(i);
            }

            // UV Offsets:
            NiPoint2 kScale = pkSurface->GetUVScale();
            NiPoint2 kOffset = pkSurface->GetUVOffset();
            float fDistMaskStrength = pkSurface->GetDistributionMaskStrength();
            
            udScaleX->Value = kScale.x;
            udScaleY->Value = kScale.y;
            udOffsetX->Value = kOffset.x;
            udOffsetY->Value = kOffset.y;
            udDistStrength->Value = fDistMaskStrength;

            // Populate the tags:
            lbTags->Items->Clear();
            NiTObjectSet<NiFixedString> kTags;
            pkSurface->GetTags(kTags);
            
            unsigned int uiNumTags = kTags.GetSize();
            for (unsigned int ui = 0; ui < uiNumTags; ++ui)
            {
                lbTags->Items->Add(new String(kTags.GetAt(ui)));
            }

            // Populate the MetaData:
            dgMetaData->Rows->Clear();
            kTags.RemoveAll();
            pkSurface->GetMetaData().GetKeys(kTags);

            uiNumTags = kTags.GetSize();
            for (unsigned int ui = 0; ui < uiNumTags; ++ui)
            {
                System::Object *values[] = new System::Object*[3];
                NiFixedString kTag = kTags.GetAt(ui);
                NiMetaData::KeyType kType;
                
                values[0] = new String(kTag);
                pkSurface->GetMetaData().GetKeyType(kTag,kType);
                float fWeight;
                switch(kType)
                {
                    case NiMetaData::FLOAT:
                    {
                        float value;
                        pkSurface->GetMetaData().Get(kTag, value, fWeight);
                        values[2] = Convert::ToString(value);
                        values[1] = S"FLOAT";
                        break;
                    }
                    
                    case NiMetaData::INTEGER:
                    {
                        int value;
                        pkSurface->GetMetaData().Get(kTag, value, fWeight);
                        values[2] = Convert::ToString(value);
                        values[1] = S"INTEGER";
                        break;
                    }
                    
                    case NiMetaData::STRING:
                    {
                        NiFixedString value;
                        pkSurface->GetMetaData().Get(kTag, value, fWeight);
                        values[2] = Convert::ToString(value);
                        values[1] = S"STRING";
                        break;
                    }

                    case NiMetaData::FLOAT_BLENDED:
                    {
                        float value;
                        pkSurface->GetMetaData().Get(kTag, value, fWeight);
                        values[2] = Convert::ToString(value);
                        values[1] = S"FLOAT_BLENDED";                       
                        break;
                    }

                    case NiMetaData::INTEGER_BLENDED:
                    {
                        int value;
                        pkSurface->GetMetaData().Get(kTag, value, fWeight);
                        values[2] = Convert::ToString(value);
                        values[1] = S"INTEGER_BLENDED";
                        break;
                    }
                }

                dgMetaData->Rows->Add(values);
            }

        }
    }

    m_pkSurface = pkSurface;
    m_pkPalette = pkPalette;

    this->cbPreview->Checked = true;
}
//---------------------------------------------------------------------------
void DlgSurfaceEditor::ViewMapDetails(ListViewItem *pkItem)
{
    if (pkItem)
    {
        tbFilename->Text = pkItem->SubItems->get_Item(1)->Text;

        tbFilename->Enabled = true;
        cbPreview->Enabled = true;
    }
    else
    {
        tbFilename->Enabled = false;
        cbPreview->Enabled = false;

        tbFilename->Text = "";

        //Enable the preview mode....
        pbTexture->Image = 0;        
    }
 }
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::tsTagAdd_Click(System::Object*, 
    System::EventArgs*) 
{
     if (!lbTags->Items->Contains(tscbTag->Text) &&
         tscbTag->Text->Trim()->Length != 0)
     {
        lbTags->Items->Add(tscbTag->Text);
        tscbTag->Text = "";
        toolStripContainer1->Focus();
        toolStrip1->Focus();
        tscbTag->Focus();
     }
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::tsTagDelete_Click(System::Object*, 
    System::EventArgs*) 
{
     if(lbTags->SelectedIndex >= 0)
     {
        lbTags->Items->Remove(lbTags->SelectedItem);
     }
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::btCancel_Click(System::Object*, 
    System::EventArgs*) 
{
    this->DialogResult = DialogResult::Cancel;
    this->Close();
}
//---------------------------------------------------------------------------
bool DlgSurfaceEditor::IsExistingMaterialName(const char* pcExistingName,
    const char* pcMaterialName, NiSurfacePackage* pkPackage)
{
    NiSurface* pkSurface = m_pkPalette->GetSurface(pkPackage->GetName(), 
        pcMaterialName);

    if(pkSurface != NULL && strcmp(pcExistingName, pcMaterialName) != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::btApply_Click(System::Object*  sender, 
    System::EventArgs*  e) 
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);
    bool bIsValid = true;

    // Apply all the changes made in this window
    if (m_pkSurface)
    {
        // Get the new package to transfer to.
        NiSurfacePackage *pkPackage = m_pkPalette->GetPackage(
            MStringToCharPointer(cbPackage->Text));

        if (pkPackage == 0)
        {
            NiMessageBox("No package selected", "Material Error");
            return;
        }

        // Save the properties:  
        const char* kExistingName = m_pkSurface->GetName();
        const char* pcMaterialName = MStringToCharPointer(tbName->Text);
        if(strcmp(pcMaterialName, "") == 0)
        {
            NiMessageBox("No material name entered", "Material Error");
            return;
        }
        else if(IsExistingMaterialName(kExistingName, pcMaterialName, 
            pkPackage))
        {
            NiMessageBox("Material name already in use", "Material Error");
            return;
        } 

        // Verify that all metadata has names and types
        for (int i=0; i < dgMetaData->Rows->Count - 1; ++i)
        {
            DataGridViewRow* row = dgMetaData->Rows->get_Item(i);

            DataGridViewCell* namecell = row->Cells->get_Item(0);
            if (!namecell->Value)
            {
                NiMessageBox("Material meta data requires a key name", 
                    "Material Error");
                return;
            }

            DataGridViewCell *typecell = row->Cells->get_Item(1);
            if (!typecell->Value)
            {
                NiMessageBox("Material meta data requires a valid type", 
                    "Material Error");
                return;
            }
        }

        m_pkSurface->SetName(MStringToCharPointer(tbName->Text));

        if (m_pkSurface->GetPackage())
        {
            pkPackage->ClaimSurface(m_pkSurface);
        }
        else
        {
            NiSurface *pkNewSurface = m_pkPalette->CreateSurfaceFrom(
                MStringToCharPointer(cbPackage->Text), m_pkSurface->GetName(),
                m_pkSurface);
            
            if (pkNewSurface == 0)
                return;
            
            delete m_pkSurface;
            m_pkSurface = pkNewSurface;
        }
        m_pkSurface->SetDescription(MStringToCharPointer(tbDescription->Text));

        // Save the textures:
        for (int i=0; i < NiSurface::NUM_SURFACE_MAPS; ++i)
        {
            NiFixedString kMapFile = 
                MStringToCharPointer(lvTextures->Items->get_Item(i)->
                SubItems->get_Item(1)->Text);
            
            m_pkSurface->SetMapFile((NiSurface::SurfaceMapID)i,kMapFile);
        }

        // Save the UV Offsets:
        NiPoint2 kScale;
        NiPoint2 kOffset;  
        kScale.x = Convert::ToSingle(udScaleX->Value);
        kScale.y = Convert::ToSingle(udScaleY->Value);
        kOffset.x = Convert::ToSingle(udOffsetX->Value);
        kOffset.y = Convert::ToSingle(udOffsetY->Value);
        m_pkSurface->SetUVScale(kScale);
        m_pkSurface->SetUVOffset(kOffset);

        float fDistMaskValue = Convert::ToSingle(udDistStrength->Value);
        m_pkSurface->SetDistributionMaskStrength(fDistMaskValue);

        // Save the tags:
        m_pkSurface->RemoveAllTags();
        for (int i=0; i < lbTags->Items->Count; ++i)
        {
            String *pmItem = dynamic_cast<String*>(lbTags->Items->get_Item(i));
            NiFixedString kTag = MStringToCharPointer(pmItem);
            m_pkSurface->AddTag(kTag);
        }
        m_pkPalette->RefreshIndexedSurfaces();
        
        // Save the metadata:
        NiMetaData &kMetaData = m_pkSurface->GetMetaData();
        kMetaData.RemoveAllKeys();        
        for (int i=0; i < dgMetaData->Rows->Count - 1; ++i)
        {
            DataGridViewRow *row = dgMetaData->Rows->get_Item(i);
            DataGridViewCell *namecell = row->Cells->get_Item(0);
            DataGridViewCell *typecell = row->Cells->get_Item(1);
            DataGridViewCell *valuecell = row->Cells->get_Item(2);
            
            NiFixedString kMetaName = 
                MStringToCharPointer(namecell->Value->ToString());

            try
            {
                if (typecell->Value->ToString()->CompareTo(S"INTEGER") == 0)
                {
                    int value = Convert::ToInt32(valuecell->Value);
                    bIsValid &= kMetaData.Set(kMetaName,value);
                }
                else if (
                    typecell->Value->ToString()->CompareTo(S"FLOAT") == 0)
                {
                    float value = (float)Convert::ToDouble(valuecell->Value);
                    bIsValid &= kMetaData.Set(kMetaName,value);
                }
                else if (
                    typecell->Value->ToString()->CompareTo(S"STRING") == 0)
                {
                    NiFixedString value = 
                        MStringToCharPointer(valuecell->Value->ToString());
                    bIsValid &= kMetaData.Set(kMetaName,value);
                }
                else if (typecell->Value->ToString()->
                    CompareTo(S"FLOAT_BLENDED") == 0)
                {
                    float value = (float)Convert::ToDouble(valuecell->Value);
                    bIsValid &= kMetaData.Set(kMetaName, value, 
                        NiMetaData::FLOAT_BLENDED);
                }
                else if (typecell->Value->ToString()->
                    CompareTo(S"INTEGER_BLENDED") == 0)
                {
                    int value = Convert::ToInt32(valuecell->Value);
                    bIsValid &= kMetaData.Set(kMetaName,value, 
                        NiMetaData::INTEGER_BLENDED);
                } 
                else
                {
                    bIsValid = false;
                }
            }
            catch (InvalidCastException*)
            {
                bIsValid = false;
            }
            catch ( ArgumentNullException* ) 
            {                
                bIsValid = false;
            }
            catch ( FormatException* ) 
            {             
                bIsValid = false;
            }
            catch ( OverflowException* ) 
            {             
                bIsValid = false;
            }
        }

        // Tell the surface to recompile
        m_pkSurface->CompileSurface();

        if (!m_pkSurface->IsCompiled())
            bIsValid = false;

        // If we did not successfully build a terrain material, make sure to 
        // tell the user.
        if (!bIsValid)
        {
            MessageBox::Show("Unable to load one or more textures for this "
                "terrain material. Make sure the parameters are correct and "
                "any images are valid.", "Error constructing terrain material",
                MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
    this->DialogResult = DialogResult::OK;
    this->Close();     
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::DlgSurfaceEditor_Load(
    System::Object*, System::EventArgs*) 
{
       ViewMapDetails(0);
       this->toolStripContainer1->BottomToolStripPanelVisible = true;
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::groupBox4_Enter(
    System::Object*, System::EventArgs*) 
{
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::lbTags_SelectedIndexChanged(
    System::Object*, System::EventArgs*) 
{
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::toolStripContainer1_BottomToolStripPanel_Click(
    System::Object*, System::EventArgs*) 
{
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::lvTextures_SelectedIndexChanged(
    System::Object*, System::EventArgs*) 
{
    //Update the selected texture details
    ListViewItem *pkItem = 0;
    if(lvTextures->SelectedItems->Count!=0)
    {
        pkItem = lvTextures->SelectedItems->get_Item(0);
    }

    ViewMapDetails(pkItem);
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::lvTextures_ItemActivate(System::Object*  sender,
    System::EventArgs*  e)
{
    btFilename_Click(sender, e);
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::cbPreview_CheckedChanged(
    System::Object*  sender, System::EventArgs*  e) 
{
     if(cbPreview->Checked)
     {
         //Preview the selected texture:
         tbFilename_TextChanged(sender,e);
     }
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::btFilename_Click(System::Object*, 
    System::EventArgs*) 
{
    if (lvTextures->SelectedItems->Count!=0)
    {
        //Display an open file dialog window to find textures...
        OpenFileDialog* pmOpenFileDialog = new OpenFileDialog();

        pmOpenFileDialog->Filter = "Supported Texture files "
            "(*.dds;*.tga;*.bmp;*.raw;*.sgi)|*.dds;*.tga;*.bmp;*.raw;*.sgi|"
            "DirectX DDS files(*.dds)|*.dds|"
            "Targa files (*.tga)|*.tga|"
            "Bitmap files (*.bmp)|*.bmp|"
            "Raw image data (*.raw)|*.raw|"
            "Silicon graphics (*.sgi)|*.sgi|"
            "All files (*.*)|*.*";
        pmOpenFileDialog->CheckFileExists = true;
        pmOpenFileDialog->Title = "Select a Texture Map";

        if (pmOpenFileDialog->ShowDialog() == ::DialogResult::OK)
        {
            // Ok, we've selected a file. See if we can open it...
            const char *pcAbsolutePath = 
                MStringToCharPointer(pmOpenFileDialog->FileName);
            NiDevImageConverter kImgConverter;
            if (!kImgConverter.CanReadImageFile(pcAbsolutePath))
            {
                NiMessageBox("The texture file's format is not supported", 
                    "Unable to load texture");
                return;
            }

            //Figure out the package:
            NiSurfacePackage* pkPackage = m_pkPalette->GetPackage
                (MStringToCharPointer(cbPackage->Text));

            int iPathLength = 512;
            char *pcRelativePath = NiAlloc(char,iPathLength);
            const char *pcPackageDir = pkPackage->GetFilename();

            NiFilename kPackagePath = pcPackageDir;
            NiString kPackageDir = pcPackageDir;
            kPackageDir = kPackageDir.Left(kPackageDir.Length() - 
                (strlen(kPackagePath.GetFilename())) - 
                (strlen(kPackagePath.GetExt())));

            size_t stRelPathLength = NiPath::ConvertToRelative(pcRelativePath, 
                iPathLength, pcAbsolutePath, kPackageDir);
            if (stRelPathLength == 0)
            {
                //Failed to make relative path
                NiMessageBox("Texture references must be \"relative\" to " \
                    "the location of the material package.\n" \
                    "It may be that you are receiving this error because " \
                    "of an attempt to reference\n" \
                    "a texture on machine or drive that is different from " \
                    "the location of the material package.",
                    "Unable to load texture");
            }
            else
            {
                tbFilename->Text = pcRelativePath;
            }
            NiFree(pcRelativePath);
        }
    }
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::tbFilename_TextChanged(System::Object*  sender,
    System::EventArgs*  e) 
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);
    // Now update the data behind the listview
    if (lvTextures->SelectedItems->Count!=0)
    {
        ListViewItem *pkItem = lvTextures->SelectedItems->get_Item(0);
        pkItem = lvTextures->SelectedItems->get_Item(0);
        NiFixedString kMapFile = MStringToCharPointer(tbFilename->Text);
        pkItem->SubItems->get_Item(1)->Text = tbFilename->Text;

        // Display the texture in the preview window:
        Image *pmSurfImage = 0;
        NiDevImageConverter kImageConverter;
        NiPixelDataPtr spSrcPixData = NULL;
        NiPixelDataPtr spPixData = NULL;

        //Figure out the package:
        NiSurfacePackage* pkPackage = m_pkPalette->GetPackage
            (MStringToCharPointer(cbPackage->Text));

        //Figure out the package path
        NiString kPackageDir = pkPackage->GetFilename();
        NiFilename kPackagePath = kPackageDir;
        kPackageDir = kPackageDir.Left(kPackageDir.Length() - 
            (strlen(kPackagePath.GetFilename())) - 
            (strlen(kPackagePath.GetExt())));

        
        if (kMapFile)
        {
            kMapFile = kPackageDir + kMapFile;
            spSrcPixData = kImageConverter.ReadImageFile(kMapFile,0);
        }
        if (spSrcPixData)
        {
            if (!NiIsPowerOf2(spSrcPixData->GetWidth()) || !NiIsPowerOf2(
                spSrcPixData->GetHeight()))
            {
                NiMessageBox("Textures must be a power of 2 in both dimensions.",
                    "Unable to load texture");
                pmSurfImage = NULL;
            }
            else
            {
                spPixData = kImageConverter.ConvertPixelData
                    (*spSrcPixData,NiPixelFormat::BGR888,0,false);
                System::Drawing::Imaging::PixelFormat mPixFormat;
                mPixFormat = 
                    System::Drawing::Imaging::PixelFormat::Format24bppRgb;

                try
                {
                    pmSurfImage = new Bitmap(
                        spPixData->GetWidth(),
                        spPixData->GetHeight(),
                        spPixData->GetPixelStride() * spPixData->GetWidth(),
                        mPixFormat,
                        spPixData->GetPixels()
                        ); 
                }
                catch (System::Exception*)
                {
                    pmSurfImage = NULL;
                }
            }
        }
        
        if (pmSurfImage == NULL)
        {
            //Load a default "NON" texture?
            System::ComponentModel::ComponentResourceManager* resourceManager =
                MTerrainPlugin::GetResourceManager();
            pmSurfImage = dynamic_cast<Image*>(
                resourceManager->GetObject("SurfaceMapUnavailable"));

            tbFilename->Text = "";
        }

        pmSurfImage = new Bitmap(pmSurfImage);
        imgTextures->Images->set_Item(pkItem->Index,pmSurfImage);

        if (cbPreview->Checked)
        {
            pbTexture->Image = pmSurfImage;
        }
    }
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::cbPackage_SelectedIndexChanged(
   System::Object*, System::EventArgs*) 
{
    if (cbPackage->Text->Length == 0)
        return;

     // Check if the selected package is valid:
     if (!cbPackage->AutoCompleteCustomSource->Contains(cbPackage->Text))
     {
        // If not valid set to blank again or the previously selected package
        if (m_pkSurface->GetPackage())
        {
            cbPackage->Text = m_pkSurface->GetPackage()->GetName();
        }
        else
        {
            cbPackage->Text = "";
            return;
        }   
     }

    InitialiseSurface(m_pkPalette, m_pkSurface, cbPackage->Text);
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::dgMetaData_CellValidating(
    System::Object*, 
    System::Windows::Forms::DataGridViewCellValidatingEventArgs*  e) 
{
    if (e->RowIndex == dgMetaData->Rows->Count - 1)
    {
        return;
    }

    DataGridViewRow *row = dgMetaData->Rows->get_Item(e->RowIndex);
    DataGridViewCell *typecell = row->Cells->get_Item(1);

    if (typecell->Value == 0)
    {
        return;
    }

    if (e->ColumnIndex == 2)
    {
        // Validate the value in the field:
        bool valid = true;
        String *errorMessage = S"Fantastic!";
        try{
            if (typecell->Value->ToString()->CompareTo(S"INTEGER") == 0 ||
                typecell->Value->ToString()->
                CompareTo(S"INTEGER_BLENDED") == 0)
            {
                Convert::ToInt32(e->FormattedValue->ToString());
            }
            else if (typecell->Value->ToString()->CompareTo(S"FLOAT") == 0 ||
                typecell->Value->ToString()->CompareTo(S"FLOAT_BLENDED") == 0)
            {
                Convert::ToDouble(e->FormattedValue->ToString());
            }
            else if (typecell->Value->ToString()->CompareTo(S"STRING") == 0)
            {
                Convert::ToString(e->FormattedValue->ToString());
            }
            else
            {
                valid = false;
                errorMessage = S"Invalid data type specified";
            }
        }
        catch(InvalidCastException*)
        {
            // Failed to convert to what we want!
            valid = false;
            errorMessage = S"Invalid data type specified";
        }
        catch ( ArgumentNullException* ) 
        {
            // Failed to convert to what we want!
            valid = false;
            errorMessage = S"A value must be specified!";
        }
        catch ( FormatException* ) 
        {
            // Failed to convert to what we want!
            valid = false;
            errorMessage = S"Incorrect type of value entered";
        }
        catch ( OverflowException* ) 
        {
            // Failed to convert to what we want!
            valid = false;
            errorMessage = S"Value is outside the valid range";
        }

        // Revert back to the old version of the data if required
        if (!valid)
        {
            if (dgMetaData->EditingControl)
            {
                dgMetaData->EditingControl->ForeColor =
                    System::Drawing::Color::Red;
                ttTip->ShowAlways = true;
                ttTip->Active = true;
                ttTip->SetToolTip(dgMetaData->EditingControl,errorMessage);
            }
            e->Cancel = true;
        }
        else
        {
            if (dgMetaData->EditingControl)
            {
                dgMetaData->EditingControl->ForeColor = 
                    System::Drawing::SystemColors::ControlText;
                ttTip->SetToolTip(dgMetaData->EditingControl,S"");
                ttTip->ShowAlways = false;
                ttTip->Active = false;
            }
        }
    }
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::dgMetaData_CellValueChanged(
    System::Object*, 
    System::Windows::Forms::DataGridViewCellEventArgs*  e) 
{
    if (e->RowIndex >= dgMetaData->Rows->Count || e->RowIndex < 0)
    {
        return;
    }

    DataGridViewRow *row = dgMetaData->Rows->get_Item(e->RowIndex);
    DataGridViewCell *typecell = row->Cells->get_Item(1);
    DataGridViewCell *valuecell = row->Cells->get_Item(2);

    if (e->ColumnIndex == 1)
    {
        // Set the value to the default for this type:
        if (typecell->Value->ToString()->CompareTo(S"INTEGER") == 0 ||
            typecell->Value->ToString()->CompareTo(S"INTEGER_BLENDED") == 0)
        {
            valuecell->Value = S"0";
        }
        else if (typecell->Value->ToString()->CompareTo(S"FLOAT") == 0 ||
            typecell->Value->ToString()->CompareTo(S"FLOAT_BLENDED") == 0)
        {
            valuecell->Value = S"0.0";
        }
        else if (typecell->Value->ToString()->CompareTo(S"STRING") == 0)
        {
            valuecell->Value = S"";
        }
    }
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::dgMetaData_RowsRemoved(
    System::Object*  sender,
    System::Windows::Forms::DataGridViewRowsRemovedEventArgs*  e)
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::dgMetaData_KeyDown(System::Object*  sender,
    System::Windows::Forms::KeyEventArgs*  e)
{
       switch ( e->KeyCode  )
      {
       case Keys::Delete: // Enter key
           {
            e->Handled = true;
            e->SuppressKeyPress = true;
            System::Windows::Forms::DataGridView* pDgv = 
                static_cast<System::Windows::Forms::DataGridView*>(sender);
            
            if (pDgv ==  NULL)
                break;

            int iRowIndex = pDgv->CurrentCell->RowIndex;
            dgMetaData->Rows->RemoveAt(iRowIndex);
            break;
           }
         default:
            break;
      }
}
//---------------------------------------------------------------------------
System::Void DlgSurfaceEditor::tscbTag_KeyDown(System::Object*  sender,
    System::Windows::Forms::KeyEventArgs*  e)
{
    NI_UNUSED_ARG(sender);
       switch ( e->KeyCode  )
      {
       case Keys::Enter: // Enter key
            e->Handled = true;
            e->SuppressKeyPress = true;
            tsTagAdd_Click(NULL, NULL);
            break;
         default:
            break;
      }
}
//---------------------------------------------------------------------------
