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

#include <NiFilename.h>

#include "MTerrainPlugin.h"

#include "TerrainCommandPanel.h"
#include "MBrushOperationFactory.h"
#include "MBrushElementSurface.h"
#include "MBrushOperationFactory.h"

using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

System::Void TerrainCommandPanel::OnLoad(
    System::Object* sender, System::EventArgs* e) 
{   
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);

    BrushType->SelectedIndex = 0;         
    MTerrainPlugin::GetInstance()->GetBrush()->UpdateOverlay();   
}
//---------------------------------------------------------------------------
System::Void TerrainCommandPanel::OnVisibleChanged(System::Object*, 
    System::EventArgs*)
{
    if (this->Visible)
        kBrushListElements->RedrawControl();
}
//---------------------------------------------------------------------------
System::Void TerrainCommandPanel::EnablePhysXPlugin()
{
    btSavePhysXData->Enabled = true;
}
//---------------------------------------------------------------------------
System::Void TerrainCommandPanel::SetEntityLabel(String*, Color)
{
    //lbSelectedEntity->Text = pName;
    //lbSelectedEntity->ForeColor = color;
}
//----------------------------------------------------------------------------
System::Void TerrainCommandPanel::BrushType_SelectedIndexChange(
    System::Object* sender, System::EventArgs*  e)
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);

    MBrush* pmBrush = MTerrainPlugin::GetInstance()->GetBrush();
    pmBrush->SetBrushType(BrushType->SelectedIndex, 
        MStringToCharPointer(BrushType->Text));
    
    m_pkBrushOptionPanel->SuspendLayout();
    NiUInt32 uiCount = m_pkBrushOptionPanel->Controls->Count;
    for (NiUInt32 i = 0; i < uiCount; i++)
    {
        m_pkBrushOptionPanel->Controls->RemoveAt(0);        
    }
    m_pkBrushOptionPanel->ResumeLayout(true);
    m_pkBrushOptionPanel->SuspendLayout();
    pmBrush->GetBrushType()->GetBrushOptionPanel(m_pkBrushOptionPanel);
    m_pkBrushOptionPanel->ResumeLayout(true);  
      
}
// ---------------------------------------------------------------------------
System::Void TerrainCommandPanel::btImport_Click(
    System::Object*  sender, System::EventArgs*  e)
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);

    // Enable the terrains interaction mode
    IUICommandService* pmMessageService = MGetService(IUICommandService);
    UICommand* pmCommand = pmMessageService->GetCommand("EnableTerrainBrush");
    pmCommand->DoClick(this, NULL);

    // Is a terrain selected?
    MEntity* pmTerrainEntity;
    NiEntityInterface* pkEntity;
    MTerrainPlugin::GetInstance()->GetTerrainEntity(pmTerrainEntity);
    if (!pmTerrainEntity)
    {        
        MTerrainPlugin::GetInstance()->SelectClosestTerrain();
        MTerrainPlugin::GetInstance()->GetTerrainEntity(pmTerrainEntity);
        
        if (!pmTerrainEntity)
        {   
            MessageBox::Show("No valid terrain entities found in the scene.",
                "Terrain Import Error", MessageBoxButtons::OK, 
                MessageBoxIcon::Error);

            return;
        }
    }
    if (!pmTerrainEntity->Writable)
    {
        MessageBox::Show("The terrain entity in the scene is read-only and "
            "cannot be modified.", "Cannot Import Terrain",
            MessageBoxButtons::OK, MessageBoxIcon::Error);
        return;
    }

    // Import an image as a height-map into the scene:
    OpenFileDialog* pmOpenFileDialog = new OpenFileDialog();
    static int importNumber = 0;

    pmOpenFileDialog->Filter = "Supported Heightmap files "
        "(*.bmp;*.tga;*.raw)|*.bmp;*.tga;*.raw|Bitmap files (*.bmp)|*.bmp|"
        "Targa files (*.tga)|*.tga|Raw image data (*.raw)|*.raw|"
        "All files (*.*)|*.*";
    pmOpenFileDialog->FilterIndex = 1;
    pmOpenFileDialog->CheckFileExists = true;
    pmOpenFileDialog->Title = "Import Height-map";

    if (pmOpenFileDialog->ShowDialog() != ::DialogResult::OK)
        return;

    NiInt32 iMaxHeight = INT_MAX;
    NiInt32 iMinHeight = INT_MIN;
    NiUInt32 uiNumLOD = UINT_MAX;
    NiUInt32 uiBlockSize = UINT_MAX;

    pkEntity = pmTerrainEntity->GetNiEntityInterface();
    pkEntity->GetPropertyData(NiTerrainComponent::ms_kPropBlockSize, 
        uiBlockSize, 0);
    pkEntity->GetPropertyData(NiTerrainComponent::ms_kPropNumLOD, uiNumLOD, 0);
    pkEntity->GetPropertyData(NiTerrainComponent::ms_kPropTerrainMinHeight, 
        iMinHeight);
    pkEntity->GetPropertyData(NiTerrainComponent::ms_kPropTerrainMaxHeight, 
        iMaxHeight);

    NiUInt32 uiSectorWidth = (uiBlockSize << uiNumLOD) + 1;
    NiUInt32 uiNumVal, uiImageWidth;
    float* pMap = 0;

    NiFixedString kFileName = MStringToCharPointer(pmOpenFileDialog->FileName);

    // Check that the file exists and is accessible
    if (!NiFile::Access(kFileName, NiFile::READ_ONLY))
    {        
        MessageBox::Show("Unable to access file.", "Terrain Import Error", 
            MessageBoxButtons::OK, MessageBoxIcon::Error);

        return;
    }

    char pcErrorMessage[512];
    NiSnprintf(pcErrorMessage, 512, NI_TRUNCATE, "Height-map must be of "
        "dimensions  (2^n + 1) x (2^n + 1) and be equal to the sector "
        "size of the terrain entity which is currently %d.", 
        uiSectorWidth);

    // Load the file using the image converter if possible
    NiDevImageConverter kImageConverter;
    if (kImageConverter.CanReadImageFile(kFileName))
    {
        // An image supported by the Dev Image Converter
        NiPixelDataPtr spPixelData = 
            kImageConverter.ReadImageFile(kFileName, 0);

        if (!spPixelData)
        {
            MessageBox::Show("Pixel data invalid.", "Terrain Import Error", 
                MessageBoxButtons::OK, MessageBoxIcon::Error);

            return;
        }

        NiPixelFormat pixelFormat = spPixelData->GetPixelFormat();

        // Figure out the variable specifications for the terrain 
        // Figure out the number of bits per channel:
        NiUInt16 usBitsPerChannel = (NiUInt16)
            (pixelFormat.GetBitsPerPixel() / pixelFormat.GetNumComponents());

        if (spPixelData->GetWidth() != spPixelData->GetHeight())
        {
            MessageBox::Show(pcErrorMessage, "Terrain Import Error", 
                MessageBoxButtons::OK, MessageBoxIcon::Error);
         
            return;
        }

        if (spPixelData->GetWidth() > USHRT_MAX)
        {
            MessageBox::Show(pcErrorMessage, "Terrain Import Error", 
                MessageBoxButtons::OK, MessageBoxIcon::Error);

            return;
        }

        if (spPixelData->GetWidth() != uiSectorWidth)
        {
            MessageBox::Show(pcErrorMessage, "Terrain Import Error", 
                MessageBoxButtons::OK, MessageBoxIcon::Error);

            return;
        }

        uiImageWidth = spPixelData->GetWidth();
        pMap = NiAlloc(float, uiImageWidth * uiImageWidth);

        unsigned char *buffer = spPixelData->GetPixels();
        uiNumVal = 0;

        float fMaxValueRecp = 1.0f / (float)((1 << usBitsPerChannel) - 1);
        float fMaxHeight = (float)iMaxHeight;
        NiUInt32 uiStride = pixelFormat.GetBitsPerPixel() / 8;
        for(NiUInt32 i = 0; i < uiImageWidth * uiImageWidth; i++)
        {
            float fNormalizedHeight = (float)buffer[i * uiStride] *
                fMaxValueRecp;

            pMap[i] = iMinHeight + (fNormalizedHeight * 
                (fMaxHeight - iMinHeight));
            uiNumVal++;
        }
    }
    else
    {        
        NiFilename kExt = kFileName;
        if (NiStricmp(kExt.GetExt(),".raw")==0)
        {
            NiFile* pkFile = NiNew NiFile(kFileName, NiFile::READ_ONLY);
            pMap = NiAlloc(float, uiSectorWidth * uiSectorWidth);
            bool bSuccess = LoadFromRAW(pkFile, pMap, uiNumVal, 
                (NiUInt16)uiNumLOD, uiBlockSize, iMinHeight, iMaxHeight);
            NiDelete pkFile;

            if (!bSuccess)
            {
                MessageBox::Show(pcErrorMessage, "Terrain Import Error", 
                    MessageBoxButtons::OK, MessageBoxIcon::Error);

                return;
            }
        }
        else
        {
            return;
        }
    }

    NiTerrainInteractor* pInteractor = 0;
    size_t size = 0;            
    
    pkEntity->GetPropertyData(NiTerrainComponent::ms_kPropInteractor,
        (void*&)pInteractor, size, 0);

    pInteractor->SetHeightFromMap(pMap, uiNumVal * sizeof(float), uiNumVal);

    NiDefaultErrorHandler kErrors;
    pInteractor->UpdateSectors(0.0f, &kErrors,
        MFramework::Instance->ExternalAssetManager);
    
    NiFree(pMap);
}
//---------------------------------------------------------------------------
bool TerrainCommandPanel::LoadFromRAW(NiFile* pSource, 
    float* pMap, NiUInt32& numVal, NiUInt16 numLOD, NiUInt32 uiBlockSize,
    NiInt32 iMinHeight, NiInt32 iMaxHeight)
{
    // Check to see if this raw file is appropriate
    NiUInt32 uiSectorWidth = (uiBlockSize * (1 << numLOD) + 1);
    unsigned short usBits = 8;
    unsigned int fileLength = pSource->GetFileSize();    

    if (fileLength == (uiSectorWidth * uiSectorWidth))
    {
        usBits = 8;
    }
    else if (fileLength == (uiSectorWidth * uiSectorWidth * 2))
    {
        usBits = 16;
    }
    else
    {
        return false;
    }
    
    // Line width, in bytes
    unsigned int uiLineWidth = 
        (uiBlockSize * (1 << numLOD) + 1) * (usBits >> 3);

    // How many lines to read, each of size uiLineWidth
    unsigned int uiNumLines = uiBlockSize * (1 << numLOD) + 1;

    void* buffer = NiMalloc(uiLineWidth * usBits);
    unsigned int numChars;
    unsigned int* auiComponentSizes = NiAlloc(unsigned int, 1);

    auiComponentSizes[0] = usBits; // bits per channel
    unsigned short y = 0;
    numVal = 0;

    float fMaxValueRecp = 1.0f / (float)((1 << usBits) - 1);
    float fMaxHeight = (float)iMaxHeight;
    
    NiUInt16* pusBuffer = (usBits == 16) ? (NiUInt16*)buffer : NULL;
    NiUInt8* pucBuffer = (usBits == 8) ? (NiUInt8*)buffer : NULL;
   
    while (y < uiNumLines)
    {
        // Read this line
        numChars = pSource->BinaryRead(buffer, uiLineWidth, 
            auiComponentSizes, 1);
        if (!numChars)
            break;

        if (numChars != uiLineWidth) 
        {
            char* msg = (char*)NiMalloc(255 * sizeof(char));
            NiSnprintf(msg, 255, 255, 
                "Error on line %d, only %d bytes found, expected %d",
                y, numChars, uiLineWidth);

            IMessageService* pmMessageService = MGetService(IMessageService);
            pmMessageService->AddMessage(MessageChannelType::Errors, 
                new String(msg));

            NiFree(msg);
            return false;
        }

        for (unsigned short x = 1; x <= uiNumLines; ++x) 
        {
            float fNormalizedHeight = 0.0f;

            if(usBits == 16)            
            {
                NIASSERT(pusBuffer);
                fNormalizedHeight = (float)pusBuffer[x - 1] * fMaxValueRecp;
            }
            else
            {
                NIASSERT(usBits == 8);
                NIASSERT(pucBuffer);
                fNormalizedHeight = (float)pucBuffer[x - 1] * fMaxValueRecp;
            }  

            pMap[numVal] = iMinHeight + (fNormalizedHeight * 
                (fMaxHeight - iMinHeight));
            numVal++;
        }

        y++;
    }

    NiFree(auiComponentSizes);
    NiFree(buffer);

    return numVal == (uiSectorWidth * uiSectorWidth);
}
//----------------------------------------------------------------------------
System::Void TerrainCommandPanel::btExport_Click(System::Object*, 
    System::EventArgs*)
{
    // Is a terrain selected?
    MEntity* pkEntity;
    MTerrainPlugin::GetInstance()->GetTerrainEntity(pkEntity);
    if (!pkEntity)
    {        
        MTerrainPlugin::GetInstance()->SelectClosestTerrain();
        MTerrainPlugin::GetInstance()->GetTerrainEntity(pkEntity);
        
        if (!pkEntity)
        {   
            IMessageService* pmMessageService = MGetService(IMessageService);
            pmMessageService->AddMessage(MessageChannelType::Errors, 
                new String("No valid terrains found in the scene"));
            return;
        }
    }

    NiUInt32 uiNumLOD = 0;
    NiUInt32 uiBlockSize = 0;
    pkEntity->GetNiEntityInterface()->GetPropertyData(
        NiTerrainComponent::ms_kPropBlockSize, uiBlockSize);
    pkEntity->GetNiEntityInterface()->GetPropertyData(
        NiTerrainComponent::ms_kPropNumLOD, uiNumLOD);
    
    NiUInt32 uiSectorWidth = (uiBlockSize << uiNumLOD) + 1;
    SaveFileDialog* pmSaveDialog = new SaveFileDialog();

    pmSaveDialog->Filter = "Targa files (*.tga)|*.tga|"
        "Raw image data 8bit (*.raw)|*.raw|"
        "Raw image data 16bit (*.raw)|*.raw";
    pmSaveDialog->FilterIndex = 3;
    
    pmSaveDialog->Title = "Export Height-map";

    if (pmSaveDialog->ShowDialog() != ::DialogResult::OK)
        return;

    NiFilename kFile = MStringToCharPointer(pmSaveDialog->FileName);
    NiUInt32 uiNumBits = 8;
    
    NiFile* pkFile = NiNew NiFile(MStringToCharPointer(pmSaveDialog->FileName),
        NiFile::WRITE_ONLY);

    if (NiStricmp(kFile.GetExt(),".tga")==0)
    {
        NiUInt8 bHeader = 0;
        NiUInt16 wHeader = 0;
                
        // Write header to destination file 
        pkFile->Write(&bHeader, 1);
        pkFile->Write(&bHeader, 1);
        bHeader = 3;
        pkFile->Write(&bHeader, 1);
        pkFile->Write(&wHeader, 2);
        pkFile->Write(&wHeader, 2);
        bHeader = 0;
        pkFile->Write(&bHeader, 1);
        pkFile->Write(&wHeader, 2);
        pkFile->Write(&wHeader, 2);
        wHeader = (NiUInt16)uiSectorWidth;
        pkFile->Write(&wHeader, 2);
        pkFile->Write(&wHeader, 2);
        bHeader = 8;
        pkFile->Write(&bHeader, 1);
        bHeader = 32;// Flip vertically
        pkFile->Write(&bHeader, 1);
    }
    else if (pmSaveDialog->FilterIndex == 3)
    {
        uiNumBits = 16;
    }
    
    NiFixedString fileName = MStringToCharPointer(pmSaveDialog->FileName);
    
    // Enable the terrains interaction mode
    IUICommandService* pmMessageService = MGetService(IUICommandService);
    UICommand* pmCommand = pmMessageService->GetCommand("EnableTerrainBrush");
    pmCommand->DoClick(this, NULL);
    
    NiTerrainInteractor* pkInteractor;
    size_t stSize = 0;
    NiInt32 iMinHeight = 0;
    NiInt32 iMaxHeight = 1;
    pkEntity->GetNiEntityInterface()->GetPropertyData(
        NiTerrainComponent::ms_kPropInteractor,
        (void*&)pkInteractor, stSize);
    pkEntity->GetNiEntityInterface()->GetPropertyData(
        NiTerrainComponent::ms_kPropTerrainMaxHeight, iMaxHeight);
    pkEntity->GetNiEntityInterface()->GetPropertyData(
        NiTerrainComponent::ms_kPropTerrainMinHeight, iMinHeight);
   
    if (pkInteractor)
    { 
        NiUInt32 uiNumValues = uiSectorWidth * uiSectorWidth;
        float* pfBuffer = NiAlloc(float, uiNumValues);
       
        float fMaxVal, fMinVal;
        pkInteractor->GetHeightMapValues(pfBuffer, uiNumValues * sizeof(float),
            uiNumValues, fMinVal, fMaxVal);
        fMinVal = (float)iMinHeight;
        fMaxVal = (float)iMaxHeight;

        float fPrecision = (float)(1 << uiNumBits) - 1.0f;
        float fMaxValRecip = 1.0f / (fMaxVal - fMinVal);
        for (NiUInt32 ui = 0; ui < uiNumValues; ui++)
        {
            float fNormalizedHeight = (pfBuffer[ui] - fMinVal) * fMaxValRecip;
            if (uiNumBits == 16)
            {
                NiUInt16 uiTemp = (NiUInt16)(fNormalizedHeight * fPrecision);
                pkFile->Write(&uiTemp, sizeof(NiUInt16));
            }
            else
            {
                NiUInt8 ucTemp = (NiUInt8)(fNormalizedHeight * fPrecision);
                pkFile->Write(&ucTemp, sizeof(NiUInt8));
            }
        }

        NiFree(pfBuffer);
    }
    NiDelete pkFile;
}
// ---------------------------------------------------------------------------
System::Void TerrainCommandPanel::btSavePhysXData_Click(
    System::Object*  sender, System::EventArgs*)
{
    // Trigger the save in the PhysX Plugin
    IUICommandService* pmCommandService = MGetService(IUICommandService);
    UICommand* pmCommand = 
        pmCommandService->GetCommand(COOK_TERRAIN_PHYSX_DATA);
    UICommandEventArgs* pmCommandEventArgs = new UICommandEventArgs;
    MEntity* pmEntity = 0;
    MTerrainPlugin::GetInstance()->GetTerrainEntity(pmEntity);
    if (pmEntity)
    {
        pmCommandEventArgs->SetValue(S"Entity", pmEntity->Name);
        pmCommandEventArgs->SetValue(S"Forced", S"true");
        if (pmCommand)
        {
            pmCommand->DoClick(sender, pmCommandEventArgs);
        }
    }
}
// ---------------------------------------------------------------------------
System::Void TerrainCommandPanel::cbUpdateLighting_Changed(
            System::Object*, System::EventArgs*)
{
    MBrush* pkBrush = MTerrainPlugin::GetInstance()->GetBrush();
    pkBrush->SetUpdateLightingRealtime(cbUpdateLighting->Checked);
}
// ---------------------------------------------------------------------------
System::Void TerrainCommandPanel::EnableTerrainCommandPanel(bool bEnable)
{
    gbBrushShape->Enabled = bEnable;
    m_pkElementBrowserGroup->Enabled = bEnable;
    groupBox1->Enabled = bEnable;
        
    // Update the selection hiding:
    HideSelection(m_bBrushActive && gbBrushShape->Enabled);
}
// ---------------------------------------------------------------------------
void TerrainCommandPanel::Update()
{
    __super::Update();

    Control* pkTemp = 
        m_pkElementBrowserTab->SelectedTab->Controls->get_Item(S"ButtonPanel");
    
    m_pkElementBrowserGroup->Height = pkTemp->Height + 30;
   
}
// ---------------------------------------------------------------------------
void TerrainCommandPanel::UseSurfacePalette(NiSurfacePalette* pkPalette)
{
    MDisposeRefObject(m_pkPalette);
    if (pkPalette)
    {
        m_pkPalette = pkPalette;
        MInitRefObject(m_pkPalette);
    }
}
// ---------------------------------------------------------------------------
void TerrainCommandPanel::BuildUtilityInterface()
{
    System::ComponentModel::ComponentResourceManager* pmResources = 
        MTerrainPlugin::GetInstance()->GetResourceManager();

    this->btImport->Image = 
        (__try_cast<System::Drawing::Image*>(
        pmResources->GetObject(S"TerrainImport")));
    
    this->btExport->Image = 
        (__try_cast<System::Drawing::Image*>(
        pmResources->GetObject(S"TerrainExport")));

    this->btSavePhysXData->Image = 
        (__try_cast<System::Drawing::Image*>(
        pmResources->GetObject(S"PhysxExport")));

    this->cbUpdateLighting->Image = 
        (__try_cast<System::Drawing::Image*>(
        pmResources->GetObject(S"AutoRefreshLighting")));
}
// ---------------------------------------------------------------------------
void TerrainCommandPanel::BuildElementBrowserInterface()
{
    ICollection* pmOperations = 
        MBrushOperationFactory::GetInstance()->GetTemplates();

    int iBXPos;
    int iBYPos;
    
    IEnumerator* pkEnum = pmOperations->GetEnumerator();

    while (pkEnum->MoveNext())
    {
        MBrushOperation* pmCurrent = dynamic_cast
            <MBrushOperation*>(pkEnum->Current);
        TabPage* pmTab = dynamic_cast<TabPage*>(
            m_pkElementBrowserTab->Controls->get_Item
            (pmCurrent->m_pmElement->m_pmName));

        if (!pmTab)
        {
            //add the Element tab
            pmTab = new TabPage();
            pmTab->Name = pmCurrent->m_pmElement->m_pmName;
            pmTab->Text = pmCurrent->m_pmElement->m_pmName;
            pmTab->Location = System::Drawing::Point(4, 22);
            pmTab->Padding = System::Windows::Forms::Padding(3);
            pmTab->Size = System::Drawing::Size(277, 75);
            pmTab->UseVisualStyleBackColor = true;
            pmTab->Dock = DockStyle::Fill;
            pmTab->SizeChanged += new EventHandler(this, 
                &TerrainCommandPanel::ControlSizeChanged);
            
            //Button panel:
            FlowLayoutPanel* pkPanel = new FlowLayoutPanel();
            pkPanel->AutoSize = true;
            pkPanel->FlowDirection = FlowDirection::LeftToRight;
            pkPanel->Name = S"ButtonPanel";
            pkPanel->Location = System::Drawing::Point(3, 3);
            pkPanel->Size = System::Drawing::Size(273, 60);
            pkPanel->SizeChanged += new EventHandler(this, 
                &TerrainCommandPanel::ControlSizeChanged);

            pmTab->Controls->Add(pkPanel);
            
            m_pkElementBrowserTab->Controls->Add(pmTab);
        }

        FlowLayoutPanel* pkButtonPanel = dynamic_cast<FlowLayoutPanel*>
            (pmTab->Controls->get_Item(S"ButtonPanel"));
                
        if (pkButtonPanel->Controls->Count > 0)
        {
            Control * pkItem = pkButtonPanel->Controls->get_Item
                (pkButtonPanel->Controls->Count - 1);
            
            if (pkItem->Location.X + ((pkItem->Width + 5) * 2) < 
                pkButtonPanel->Width)
            {
                iBXPos = pkItem->Location.X + pkItem->Width + 5;
                iBYPos = pkItem->Location.Y;
            }
            else
            {
                iBYPos = pkItem->Location.Y + pkItem->Height + 5;
                iBXPos = 0;
            }
        }
        else
        {
            iBXPos = 0;
            iBYPos = 10;
        }
        
        ToolTip* newTool = new ToolTip();
        newTool->AutoPopDelay = 5000;
        newTool->InitialDelay = 1000;
        newTool->ReshowDelay = 500;
        newTool->ShowAlways = true;

        // Adds the button for the operation
        Button* pkOpStarter = new Button();
        pkOpStarter->Name = pmCurrent->m_pmName;
        
        pkOpStarter->BackgroundImage = pmCurrent->m_pmOperationIcon;
        pkOpStarter->BackgroundImageLayout = 
            System::Windows::Forms::ImageLayout::Stretch;
        pkOpStarter->Location = Point(iBXPos, iBYPos);
        pkOpStarter->Size = System::Drawing::Size(47, 36);
        pkOpStarter->UseVisualStyleBackColor = true;
        pkOpStarter->Click += new EventHandler(this,
            &TerrainCommandPanel::OperationButton_Click);
        pkOpStarter->MouseDown += new MouseEventHandler(this,
            &TerrainCommandPanel::OperationButton_MouseDown);
        pkOpStarter->MouseMove += new MouseEventHandler(this,
            &TerrainCommandPanel::OperationButton_MouseMove);
        pkOpStarter->MouseUp += new MouseEventHandler(this,
            &TerrainCommandPanel::OperationButton_MouseUp);

        newTool->SetToolTip( pkOpStarter, pmCurrent->m_pmName );
        
        pkButtonPanel->Controls->Add(pkOpStarter);        
    }
}
//----------------------------------------------------------------------------
bool TerrainCommandPanel::ReadHeightMap(String* pFileName, float* pMap, 
    int& numVal, int& imageWidth, int minHeight, int maxHeight)
{
    NiFixedString kFileName = MStringToCharPointer(pFileName);
    // Check that the file exists/is accessible
    if (!NiFile::Access(kFileName, NiFile::READ_ONLY))
    {
        return false;
    }

    // Load the file using the image converter
    NiDevImageConverter kImageConverter;
    if (!kImageConverter.CanReadImageFile(kFileName))
    {
        return false;
    }
    NiPixelDataPtr spPixelData = kImageConverter.ReadImageFile(kFileName, 0);
    if (!spPixelData)
        return false;

    NiPixelFormat pixelFormat = spPixelData->GetPixelFormat();

    // Check that the file is a reasonable format
    // Must satisfy the following requirements:
    //  - Single Channel (Height data)
    //  - Dimensions must be square
    //  - Dimensions must = (2 ^ x) + 1, for some unsigned short x
    //if (pixelFormat.GetNumComponents() != 1)
    //    return false;
    if (spPixelData->GetWidth() != spPixelData->GetHeight())
        return false;
    if (spPixelData->GetWidth() > USHRT_MAX)
        return false;

    imageWidth = spPixelData->GetWidth();
   
    if (pMap)
        delete[] pMap;
    pMap = new float[imageWidth * imageWidth];

    unsigned char *buffer = spPixelData->GetPixels();
    numVal = 0;
    for (int i = 0; i < imageWidth * imageWidth; i++)
    {
        pMap[i] = (float)((maxHeight - buffer[i])/(maxHeight - minHeight));
        numVal++;
    }
    
    return true;
}
// ---------------------------------------------------------------------------
System::Void TerrainCommandPanel::OperationButton_Click(
    System::Object*  sender, System::EventArgs*  e)
{
    NI_UNUSED_ARG(e);

    Button* pkClicked = dynamic_cast<Button*>(sender);

    System::Collections::Generic::List<MBrushOperation*>* pTemp = 
        MTerrainPlugin::GetInstance()->GetBrush()->GetOperations();
    pTemp->RemoveRange(0, pTemp->Count);
    kBrushListElements->RemoveAll();

    m_SelectedElement = pkClicked->Parent->Parent->Text;
    MBrushOperation* pmToAdd = MBrushOperationFactory::GetInstance()->
        GetNewOperation(m_SelectedElement, pkClicked->Name);

    MUCBrushElement* pmNewTool = new MUCBrushElement();

    pmNewTool->Name = pkClicked->Name;
    pmNewTool->SetOperation(pmToAdd);

    kBrushListElements->AddItemToList(pmNewTool);
}
//---------------------------------------------------------------------------
System::Void TerrainCommandPanel::OperationButton_MouseDown(
    System::Object*  sender, System::Windows::Forms::MouseEventArgs*  e)
{
    Button* pkClicked = dynamic_cast<Button*>(sender);

    if (e->Button == MouseButtons::Left)
    {
        Point kLoc = Point(e->get_X(),e->get_Y());
        Point kScreenLoc = pkClicked->PointToScreen(kLoc);
        
                
        // Remember the point where the mouse down occurred. The DragSize 
        // indicates the size that the mouse can move before a drag event 
        // should be started.
        System::Drawing::Size dragSize = SystemInformation::DragSize;
        
        // Create a rectangle using the DragSize, with the mouse position being
        // at the center of the rectangle.
        m_kDragingBox = System::Drawing::Rectangle
            (Point(e->X - (dragSize.Width / 2),e->Y - 
            (dragSize.Height / 2)),dragSize);
    }


}
//---------------------------------------------------------------------------
System::Void TerrainCommandPanel::OperationButton_MouseUp(
    System::Object*  sender, System::Windows::Forms::MouseEventArgs*  e)
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);

    // Reset the drag rectangle when the mouse button is raised.
    m_kDragingBox = Rectangle::Empty;
}
//---------------------------------------------------------------------------
System::Void TerrainCommandPanel::OperationButton_MouseMove(
    System::Object*  sender, System::Windows::Forms::MouseEventArgs*  e)
{
    Button* pkClicked = dynamic_cast<Button*>(sender);

    if ( e->Button == MouseButtons::Left )
    {
            
        // If the mouse moves outside the rectangle, start the drag.
        if ( m_kDragingBox != Rectangle::Empty &&  
            !m_kDragingBox.Contains( e->X, e->Y ) )
        {           
                            
            // The screenOffset is used to account for any desktop bands
            // that may be at the top or left side of the screen when
            // determining when to cancel the drag drop operation.
            //m_kScreenOffset = SystemInformation::WorkingArea.Location;
              
            m_bDragging = true;

            m_SelectedElement = pkClicked->Parent->Parent->Text;
            MBrushOperation* pmToAdd = MBrushOperationFactory::GetInstance()->
                GetNewOperation(m_SelectedElement,pkClicked->Name);

            MUCBrushElement* pmNewTool = new MUCBrushElement();

            pmNewTool->Name = pkClicked->Name;
            pmNewTool->SetOperation(pmToAdd);
            pmNewTool->Visible = false;

            kBrushListElements->SetDraggedObject(pmNewTool);

            // Proceed with the drag-and-drop, passing in the list item.
            Object* toDrag = dynamic_cast<Object*>
                (pmNewTool);
            pkClicked->DoDragDrop(toDrag, DragDropEffects::Move );
        }
    }
}
//---------------------------------------------------------------------------
System::Void TerrainCommandPanel::ControlSizeChanged(System::Object* sender,
    System::EventArgs*)
{
    Control* pkTemp = dynamic_cast<Control*>(sender);

    if (String::Compare(pkTemp->Name, S"ButtonPanel") == 0)
    {
        m_pkElementBrowserGroup->Height = pkTemp->Height + 30;
            
        if (pkTemp->Parent->Width <= pkTemp->Width)
        {
            pkTemp->Width = pkTemp->Parent->Width - 5;
            pkTemp->Height = pkTemp->Controls->get_Item(
                pkTemp->Controls->Count-1)->Location.Y +
                pkTemp->Controls->get_Item(
                pkTemp->Controls->Count-1)->Height + 20;
        }
        
    }
    else if (String::Compare(pkTemp->Name, S"MainPanel") == 0)
    {
        
        if (pkTemp->Width < pkTemp->Height)
        {
            groupBox1->Height = 280;
            gbBrushShape->Location = Point(0, 
                m_pkDebugButtonPanel->Location.Y + 
                m_pkDebugButtonPanel->Height + 5);
            groupBox1->Location = Point(0, 
                gbBrushShape->Location.Y + gbBrushShape->Height + 5);
            m_pkElementBrowserGroup->Location = Point(0, 
                groupBox1->Location.Y + 285);;

            if (pkTemp->Width - 5 < 350 && pkTemp->Width > 260)
            {
                m_pkDebugButtonPanel->Width = pkTemp->Width - 5;
                gbBrushShape->Width = pkTemp->Width - 5;
                groupBox1->Width = pkTemp->Width - 5;
                m_pkElementBrowserGroup->Width = pkTemp->Width - 5;
            }
            else
            {
                if (pkTemp->Width - 5 >= 350)
                {
                    m_pkDebugButtonPanel->Width = 350;
                    gbBrushShape->Width = 350;
                    groupBox1->Width = 350;
                    m_pkElementBrowserGroup->Width = 350;
                }
                else if(pkTemp->Width <= 260)
                {
                    m_pkDebugButtonPanel->Width = 260;
                    gbBrushShape->Width = 260;
                    groupBox1->Width = 260;
                    m_pkElementBrowserGroup->Width = 260;
                    
                }
            }
        }
        else
        {
            
            m_pkDebugButtonPanel->Width = 205;
            gbBrushShape->Width = 260;
            groupBox1->Width = 300;
            m_pkElementBrowserGroup->Width = 300;

            gbBrushShape->Location = Point(210,5);
            groupBox1->Location = Point(480, 5);
            m_pkElementBrowserGroup->Location = Point(785,5);

            if (pkTemp->Height - 5 < 400 && pkTemp->Height > 150)
            {
                groupBox1->Height = pkTemp->Height - 5;
                m_pkElementBrowserGroup->Height = pkTemp->Height - 5;
            }
            else
            {
                if (pkTemp->Height - 5 >= 400)
                {
                    groupBox1->Height = 400;
                    m_pkElementBrowserGroup->Height = 400;
                }
                else if (pkTemp->Height <= 150)
                {
                    groupBox1->Height = 150;
                    m_pkElementBrowserGroup->Height = 150;                    
                }
            }
        }
    }
    else if (String::Compare(pkTemp->Name, S"BrushPanel") == 0)
    {
        m_pkBrushOptionPanel->Width = m_pkBrushPanel->Width - 5;
    }
    else
    {
        m_pkElementBrowserTab->Height = m_pkElementBrowserGroup->Height - 20;
        m_pkElementBrowserTab->SelectedTab->Height = 
            m_pkElementBrowserGroup->Height - 20;
        
        pkTemp->Controls->get_Item(S"ButtonPanel")->Width = pkTemp->Width - 5;
    }
}
//---------------------------------------------------------------------------
void TerrainCommandPanel::OnBrushActivated(bool bActive)
{
    m_bBrushActive = bActive;
    HideSelection(m_bBrushActive && gbBrushShape->Enabled);
}
//---------------------------------------------------------------------------
void TerrainCommandPanel::HideSelection(bool bHideSelection)
{
    //FIXME: Find a better way to stop terrain being highlighted
    //This works to stop the terrain from being highlighted, but 
    //is saved as a setting :S
    //Not approved by the designer.
    float fSelectionOpacity;
    if (bHideSelection)
    {
        fSelectionOpacity = 0.0f;
    }
    else
    {
        fSelectionOpacity = 0.25f;
    }
    
    ISettingsService *settings = MGetService(ISettingsService);
    if (settings)
    {
        settings->SetSettingsObject("Entity Highlight Transparency",
            __box(fSelectionOpacity), SettingsCategory::PerUser);
    }
}

//============================================================================
// Public Methods
//============================================================================

void TerrainCommandPanel::Surface_DoubleClick(NiSurface* pkSelection)
{

    // First search if an operation using a surface exists 
    MUCBrushElement* pmItem = dynamic_cast<MUCBrushElement*>
        (kBrushListElements->FindFirstBrushElement(S"Material", NULL));

    // Enable the terrains interaction mode
    IUICommandService* pmMessageService = MGetService(IUICommandService);
    UICommand* pmCommand = pmMessageService->GetCommand("EnableTerrainBrush");
    pmCommand->DoClick(this, NULL);

    if (pmItem)
    {
        if (String::Compare(
            pmItem->GetOperation()->m_pmElement->m_pmName, S"Material") == 0)
        {
            // Compile the surface before using it
            if (!pkSelection->IsCompiled())
                pkSelection->CompileSurface();

            dynamic_cast<MBrushElementSurface*>
                (pmItem->GetOperation()->m_pmElement)->SetSurface(pkSelection);
        }

        pmItem->Update();
        kBrushListElements->RedrawControl();
    }
    else
    {
        // No blending operation have been added yet so create a new one
        // and remove all the others
        System::Collections::Generic::List<MBrushOperation*>* pTemp = 
            MTerrainPlugin::GetInstance()->GetBrush()->GetOperations();
        pTemp->RemoveRange(0, pTemp->Count);
        kBrushListElements->RemoveAll();

        MBrushOperation* pmToAdd = MBrushOperationFactory::GetInstance()->
            GetNewOperation(S"Material",m_SelectedOperation);

        if (!pmToAdd)
        {
            pmToAdd = MBrushOperationFactory::GetInstance()->
                GetNewOperation(S"Material",S"Paint");
        }
        MUCBrushElement* pmNewTool = new MUCBrushElement();

        pmNewTool->Name = pmToAdd->m_pmName;
        pmNewTool->SetOperation(pmToAdd);

        dynamic_cast<MBrushElementSurface*>
            (pmToAdd->m_pmElement)->SetSurface(pkSelection);

        kBrushListElements->AddItemToList(pmNewTool);
    }
}
//---------------------------------------------------------------------------
void TerrainCommandPanel::Surface_Click(NiSurface* pkSelection)
{
    ICollection* pmOperations = 
        MBrushOperationFactory::GetInstance()->GetTemplates();
    
    IEnumerator* pkEnum = pmOperations->GetEnumerator();

    while (pkEnum->MoveNext())
    {
        MBrushOperation* pmCurrent = dynamic_cast
            <MBrushOperation*>(pkEnum->Current);

        if (String::Compare(pmCurrent->m_pmElement->m_pmName, 
            S"Material") == 0)
        {
            // Compile the surface before using it
            if (!pkSelection->IsCompiled())
                pkSelection->CompileSurface();

            dynamic_cast<MBrushElementSurface*>
                (pmCurrent->m_pmElement)->SetSurface(pkSelection);

            MBrushOperationFactory::GetInstance()->UpdateTemplateFrom(
                pmCurrent);
        }
    }
}
//---------------------------------------------------------------------------
void TerrainCommandPanel::Surface_Remove(NiSurface* pkSelection)
{
    NI_UNUSED_ARG(pkSelection);

    // Is a material operation in the stack?
    MUCBrushElement* pmItem = dynamic_cast<MUCBrushElement*>
        (kBrushListElements->FindFirstBrushElement(S"Material", NULL));

    if (pmItem)
    {
        MBrushElementSurface* pmElement = dynamic_cast<MBrushElementSurface*>
            (pmItem->GetOperation()->m_pmElement);

        pmElement->SetSurface(NULL);
        MBrushOperationFactory::GetInstance()->UpdateTemplateFrom(
            pmItem->GetOperation());
    }
}
//---------------------------------------------------------------------------
void TerrainCommandPanel::Palette_Update()
{
    kBrushListElements->Update();
}
//---------------------------------------------------------------------------
