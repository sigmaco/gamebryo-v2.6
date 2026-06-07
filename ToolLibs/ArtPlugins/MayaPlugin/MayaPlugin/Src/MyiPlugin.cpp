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

//
// MyiPlugin.cpp
// =============
//
//      Handles the plugin entrant code.  Also creates a database using the
// MDtAPI which is built on Maya's more general API.  This latter part is
// done via functions which are defined in MyiDtExt.cpp.
//
// NOTE:
// The MDtAPI has been included as a separate project because specific changes
// either have or will be made for the MyImmerse application.
//

// Precompiled Headers
#include "MayaPluginPCH.h"
#include "MyiInitPlugin.h"

#include "NiShadowManager.h"

#include <NiMeshProfileProcessor.h>    // We need the mesh profiles
#include <NiMeshProfileXMLParser.h>

#include "maya/MGlobal.h"


#ifdef WIN32
#define MAXPATHLEN 512
#endif


// Initialize our static magic "number"
MString NifExportPlugin::magic ("HEADER_TITLE");

//---------------------------------------------------------------------------
NifExportPlugin::NifExportPlugin()
{ 
    NiSourceTexture::SetDestroyAppDataFlag(false);

    char acMayaLocation[MAX_PATH];

    // Get the location of maya and convert the directory seperators
#if defined(_MSC_VER) && _MSC_VER >= 1400
    unsigned int uiLen = 0;
    getenv_s(&uiLen, acMayaLocation, MAX_PATH, "MAYA_LOCATION");
    NIASSERT(uiLen <= MAX_PATH);
#else
    NiStrcpy(acMayaLocation, MAX_PATH, getenv("MAYA_LOCATION"));
#endif
    MyiTexture::ReplaceChar(acMayaLocation, '/', '\\');


    // Initialize the Framework (Make floodgate run serial)
    NiFramework::InitFramework(acMayaLocation, "Gamebryo Maya Plugin", "7.4",
        false);

    // Initialize the plug-in.
    MyiInitPlugin::Init();

    // Create the mesh profile data
    NiMeshProfileProcessor::CreateMeshProfileProcessor();
    NiMeshProfileXMLParser::RegisterAllProfilesWithProcessor();

    gExport.Initialize();

    char acPluginPath[MAX_PATH];
#if defined(_MSC_VER) && _MSC_VER >= 1400
    uiLen = 0;
    getenv_s(&uiLen, acPluginPath, MAX_PATH, "EGB_TOOL_PLUGIN_PATH");
    NIASSERT(uiLen <= MAX_PATH);
#else
    NiStrcpy(acPluginPath, MAX_PATH, getenv("EGB_TOOL_PLUGIN_PATH"));
#endif

#if _MSC_VER == 1310 //VC7.1
    NiStrcat(acPluginPath, MAX_PATH, "\\Lib\\VC71\\");
#elif _MSC_VER == 1400 //VC8.0
    NiStrcat(acPluginPath, MAX_PATH, "\\Lib\\VC80\\");
#elif _MSC_VER == 1500 //VC9.0
    NiStrcat(acPluginPath, MAX_PATH, "\\Lib\\VC90\\");
#else
    #error Unsupported version of Visual Studio
#endif

    NiPluginManager::GetInstance()->AddPluginDirectory(acPluginPath, true,
        gExport.m_bRunSilently, "XSIImport");

    NiScriptTemplateManager::GetInstance()->AddScriptDirectory(
        gExport.m_pcScriptTemplatePath, true, gExport.m_bRunSilently);

    // Create the MyImmerseShelf if it does not exists
    MStatus MayaStatus = MGlobal::executeCommand("GamebryoShelf",false,true);

    // Create the Script Job for Reseting Base Map Only
    MayaStatus = MGlobal::executeCommand("scriptJob -e SceneOpened "
        "MyImmerseResetAllNiMultiShaderBaseMapOnly", false, true);

    // Create load the shader template so we access to the Pixel Shader 
    // Mel commands
    MayaStatus = MGlobal::executeCommand(
        "source AENiMultiShaderTemplate.mel", false, true);

    // Create a script job to verify the Pixel Shaders of all the 
    // NiMultiShaders
    MayaStatus = MGlobal::executeCommand("scriptJob -e SceneOpened "
        "MyImmersePixelShaderVerifyAllNiMultiShaders", false, true);

    // Create a script job to Auto Convert ABV
    MayaStatus = MGlobal::executeCommand(
        "scriptJob -e SceneOpened MyImmerseUpgradeABVAttributes",false,true);

    // Create a script job to convert the particle systems
    MayaStatus = MGlobal::executeCommand(
        "scriptJob -e SceneOpened "
        "MyImmerseUpdateEmitterAndParticlesWithAttributes", false, true);

    //this is to convert the current scene
    MayaStatus = MGlobal::executeCommand("MyImmerseUpdateExporterOptions");

    MayaStatus = MGlobal::executeCommand(
        "scriptJob -e SceneOpened "
        "MyImmerseUpdateExporterOptions", false, true);

    MayaStatus = MGlobal::executeCommand(
        "scriptJob -e NewSceneOpened "
        "MyImmerseUpdateExporterOptions", false, true);

    // Create a script job to Unload the Plug-in when the application quits
    MayaStatus = MGlobal::executeCommand("scriptJob -e quitApplication "
        "(\"unloadPlugin -f \" + MyImmerseGetPluginName())", false, true);

    // Load some default MEL script
    MayaStatus = MGlobal::executeCommand(
        "source MyImmerseCreatekeyFrameReductionDialog.mel");

    // Starup the Pixel Shaders
    NiMaterialToolkit::CreateToolkit();

    char acShaderPath[MAX_PATH];
#if defined(_MSC_VER) && _MSC_VER >= 1400
    uiLen = 0;
    getenv_s(&uiLen, acShaderPath, MAX_PATH, "EGB_SHADER_LIBRARY_PATH");
    NIASSERT(uiLen <= MAX_PATH);
#else
    NiStrcpy(acShaderPath, MAX_PATH, getenv("EGB_SHADER_LIBRARY_PATH"));
#endif
    bool bShadersLoaded = NiMaterialToolkit::GetToolkit()->LoadFromDLL(
        acShaderPath);
    NI_UNUSED_ARG(bShadersLoaded);

    NiStrcat(acShaderPath, MAX_PATH, "\\Data");

    NiMaterialToolkit::GetToolkit()->SetMaterialDirectory(acShaderPath, true);

    LoadPixelShaderData();

    SetAvaiableViewers();

    // Set the Image Converter to use the Quantizer
    NiImageConverter::SetImageConverter(NiDevImageQuantizer::Create());
    
    NiDevImageQuantizer* pkDevImageQuantizer = 
        (NiDevImageQuantizer*)NiImageConverter::GetImageConverter();

    // Add the HDR readers
    pkDevImageQuantizer->AddReader( NiNew NiOpenEXRReader );
    pkDevImageQuantizer->AddReader( NiNew NiHDRReader );
    pkDevImageQuantizer->AddReader( NiNew NiPNGReader );
    NiTexture::SetMipmapByDefault(true);

    // Output the status and the errors for the mesh profile system.
    NiUInt32 uiCount = 
        NiMeshProfileProcessor::GetErrorHandler().GetMessageCount();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        MString theMessage = 
            NiMeshProfileProcessor::GetErrorHandler().GetMessageA(ui);
        switch (NiMeshProfileProcessor::GetErrorHandler().GetMessageType(ui))
        {
        case NiMeshProfileErrorHandler::MT_ANNOTATION:
            MGlobal::displayInfo(theMessage);
            break;
        case NiMeshProfileErrorHandler::MT_WARNING:
            MGlobal::displayWarning(theMessage);
            break;
        case NiMeshProfileErrorHandler::MT_ERROR:
            MGlobal::displayError(theMessage);
            break;
        }
    }

}
//---------------------------------------------------------------------------
NifExportPlugin::~NifExportPlugin()
{
    NiMaterialToolkit::DestroyToolkit();

    gExport.Shutdown();

    // Destory the mesh profile data
    NiMeshProfileProcessor::DestroyMeshProfileProcessor();

    // Shutdown the plug-in.
    MyiInitPlugin::Shutdown();

    // Shutdown the framework
    NiFramework::ShutdownFramework();
}
//---------------------------------------------------------------------------
void* NifExportPlugin::creator()
{
    return NiExternalNew NifExportPlugin();
}
//---------------------------------------------------------------------------
MStatus NifExportPlugin::
reader (const MFileObject & file, const MString & options,
        MPxFileTranslator::FileAccessMode mode)
{
    NI_UNUSED_ARG(options);
    NI_UNUSED_ARG(mode);
    NI_UNUSED_ARG(file);
    // The reader is not implemented yet.
    MStatus rval (MS::kSuccess);
    return rval;
}
//---------------------------------------------------------------------------
MStatus NifExportPlugin::
writer ( const MFileObject & fileObject, const MString & options,
         MPxFileTranslator::FileAccessMode mode)
{
    MStatus stat = MStatus::kSuccess;

    //_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_ALLOC_MEM_DF);
   

    const char* fileName = fileObject.fullName().asChar();
    NI_UNUSED_ARG(fileName);

    // Write method of the MayaNif translator / file exporter   


    printf("-----------------------------------------------------------\n");
    printf("Exporting Process Started...\n");
    
    if (DestinationFileReadOnly(fileObject))
    {
        printf("ERROR:: Destination File is READ ONLY\n");
        return MS::kFailure;
    }

    bool bCancelled = false;
    if(DtExt_HaveGui())
    {
        MProgressWindow::reserve();
        MProgressWindow::setInterruptable(true);
        MProgressWindow::setProgressRange(0, 100);
        MProgressWindow::setTitle("Exporting Scene");
        MProgressWindow::setProgressStatus("Starting Export");
        MProgressWindow::setProgress(0);

        MProgressWindow::startProgress();
    }

    NiShadowManager::Initialize();

    // Retrieve the per-scene options
    MStatus kStatus;
    MString strOptions;
    kStatus = MGlobal::executeCommand(MString(
        "GamebryoGetExportOptions();"), strOptions);
    if ((kStatus != MStatus::kSuccess) || (strOptions.length() <= 1))
    {
        // We do <= 1 because an empty option list just has "="
        // if we could not get any per-scene options, use the most
        // previously used ones
        strOptions = options;
    }
    // Lets strip off the known extension of .Nif if it is there.
    InitDtScene(fileObject);
    BuildDtDataBase(strOptions, mode);

    // Find out where the file is supposed to end up.
    int iLN;
    char acLTmpStr[MAXPATHLEN];
    const MString fname = fileObject.fullName ();
    MDt_GetPathName ((char *) (fname.asChar ()), acLTmpStr, MAXPATHLEN);
    iLN = (int)strlen (acLTmpStr);
    if (acLTmpStr[iLN - 1] == '/')
        acLTmpStr[iLN - 1] = '\0';

    DtSetDirectory(acLTmpStr);

    SetDtTextureSearchPaths();

    // Now we can setup the database from the wire file mesh.
    // This is where all the Maya data are retrieved, cached, and processed.
    //
    // Say that we want to have camera info
    DtExt_setOutputCameras(1);

    // Now we can setup the database. This operation may be cancelled.
    int iDBInitReturn = DtExt_dbInit();
    if (iDBInitReturn == 1)
    {
        // Cancelled
        bCancelled = true;
        kStatus = MStatus::kSuccess;
    }
    else if (iDBInitReturn == 2)
    {
        // Error
        bCancelled = true;
        kStatus = MStatus::kFailure;
    }
    else
    {
        kStatus = MStatus::kSuccess;
    }

    if (!bCancelled)
    {
        DtExt_setVertexAnimation(false);
        DtFrameSet(DtFrameGetStart());
        if(DtExt_HaveGui())
        {
            if (MProgressWindow::isCancelled())
            {
                // Cancelled
                bCancelled = true;
                kStatus = MStatus::kSuccess;
            }
        }
    }

    if (!bCancelled)
    {
        if(DtExt_HaveGui())
        {
            MProgressWindow::setProgressStatus("Starting Gamebryo Conversion");
            MProgressWindow::setProgress(0);
        }

        // Now do the export
        MyImmerse* pkMyImmerse = NiNew MyImmerse();

        pkMyImmerse->Export();

        if (pkMyImmerse->Error())
            kStatus = MStatus::kFailure;
        
        NiDelete pkMyImmerse;
        pkMyImmerse = NULL;
    }

    NiShadowManager::Shutdown();

    // Clean house.
    DtExt_CleanUp();

    if(DtExt_HaveGui())
    {
        MProgressWindow::endProgress();
    }

    return kStatus;
}
//---------------------------------------------------------------------------
bool NifExportPlugin::haveReadMethod () const
{
    return false;
}
//---------------------------------------------------------------------------
bool NifExportPlugin::haveWriteMethod () const
{
    return true;
}
//---------------------------------------------------------------------------
MString NifExportPlugin::defaultExtension () const
{
    // Whenever Maya needs to know the preferred extension of this file 
    // format, it calls this method.  For example, if the user tries to save 
    // a file called "test" using the Save As dialog, Maya will call this 
    // method and actually save it as "test.wrl2".  Note that the period 
    // should * not * be included in the extension.

    return "Nif";
}

//---------------------------------------------------------------------------
bool NifExportPlugin::canBeOpened () const
{
    // This method tells Maya whether the translator can open and import files
    // (returns true) or only import  files (returns false)
    return true;
}
//---------------------------------------------------------------------------
MPxFileTranslator::MFileKind 
NifExportPlugin::identifyFile (
          const MFileObject & fileName,
          const char *buffer,
          short size) const
{
    NI_UNUSED_ARG(fileName);
    //Check the buffer for the "Nif" magic number, the
    // string "HEADER_TITLE"

    MFileKind rval = kNotMyFileType;

    if ((size >= (short)magic.length ()) &&
        (0 == strncmp (buffer, magic.asChar (), magic.length ())))
    {
        rval = kIsMyFileType;
    }
    return rval;
}
//---------------------------------------------------------------------------
bool NifExportPlugin::DestinationFileReadOnly(const MFileObject& file)
{
    // Check to see if the file exists AND can be written to
    char acBuffer[512];
    NiStrcpy(acBuffer, 512, file.fullName().asChar());

    // Check for stripping off .nif.nif
    if (strlen(acBuffer) > strlen(".nif.nif"))
    {
        char* pTemp = acBuffer + strlen(acBuffer) - strlen(".nif.nif");

        // Check for and remove extra .nif
        if (NiStricmp(pTemp, ".nif.nif") == 0)
        {
            pTemp += strlen(".nif");
            *pTemp = '\0';
        }
    }


    // Try Opening for Reading to see if the file exists
    FILE* pCanOpen = NULL;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (fopen_s(&pCanOpen, acBuffer, "r") != 0)
        pCanOpen = NULL;
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    pCanOpen = fopen(acBuffer, "r");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    // Check if the file doesn't exists
    if (pCanOpen == NULL)
        return false;

    fclose(pCanOpen);

    // Since the file exists try opening it for writing
#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (fopen_s(&pCanOpen, acBuffer, "w") != 0)
        pCanOpen = NULL;
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    pCanOpen = fopen(acBuffer, "w");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    // Can't Open must be read only
    if (pCanOpen == NULL)
        return true;

    fclose(pCanOpen);
    return false;
}
//---------------------------------------------------------------------------
void NifExportPlugin::LoadPixelShaderData()
{
    NiMaterialToolkit* pkToolkit = NiMaterialToolkit::GetToolkit();

    char* pcShaderSep = ";";
    char* pcImpSep = "|";
    char* pcAttrSep = "#";
    char* pcShaderString = NULL;
    char* pcTemp = NULL;

    // Clear the Options Var;
    MStatus kStatus = MGlobal::executeCommand(
        MString("optionVar -rm MyImmersePixelShaderData"),true,true);

    NIASSERT(kStatus == MS::kSuccess);

    //put all the shaders in a vector
    std::vector<const NiShaderDesc*> kShaderDescriptions;
    unsigned int uiLLoop;
    for (uiLLoop = 0; uiLLoop < pkToolkit->GetLibraryCount(); uiLLoop++)
    {
        NiMaterialLibrary* pkLibrary = pkToolkit->GetLibraryAt(uiLLoop);

        // Loop through all shaders in a library

        const NiShaderDesc* pkDesc = pkLibrary->GetFirstMaterialDesc();
        while (pkDesc)
        {
            
            kShaderDescriptions.push_back(pkDesc);
            pkDesc = pkLibrary->GetNextMaterialDesc();
        }

    }

    ShaderDescriptionSorter kShaderDescriptionSorter;
    std::sort(kShaderDescriptions.begin(), kShaderDescriptions.end(), 
        kShaderDescriptionSorter);

    std::vector<const NiShaderDesc*>::iterator pkDescIterator = 
        kShaderDescriptions.begin();

    while(pkDescIterator != kShaderDescriptions.end())
    {
        const NiShaderDesc* pkDesc = *pkDescIterator;

        const char* pcShaderName = pkDesc->GetName();
        const char* pcShaderDesc = pkDesc->GetDescription();

        // Handle NULL Name
        if (pcShaderName == NULL)
            pcShaderName = "No Name";

        // Handle Null Description
        if (pcShaderDesc == NULL)
            pcShaderDesc = " ";

        unsigned int uiNumImplementations = 
            pkDesc->GetNumberOfImplementations();

        // Create the start of the shader entry 
        // NAME;Description:Num Implementations

        size_t stLen = strlen(pcShaderName) + strlen(pcShaderSep) + 
            strlen(pcShaderDesc) + strlen(pcShaderSep) + 2 + 1;
        pcShaderString = NiAlloc(char, stLen);

        NiSprintf(pcShaderString, stLen, "%s%s%s%s%d", pcShaderName, 
            pcShaderSep, pcShaderDesc, pcShaderSep, uiNumImplementations);

        // Loop through all Implementations
        unsigned int uiILoop;
        for (uiILoop = 0; uiILoop < uiNumImplementations; uiILoop++)
        {
            const NiShaderRequirementDesc* pkImpDesc = 
                pkDesc->GetImplementationDescription(uiILoop);

            const char* pcNameText = NULL;
            const char* pcDescText = NULL;

            if (pkImpDesc)
            {
                pcNameText = pkImpDesc->GetName();
                pcDescText = pkImpDesc->GetDescription();
            }
            else
            {
                // There is a problem as no shader requirement desc.
                // exists. This is probably indicative of a bogus
                // shader name in the fx itself.
                char acBuf[256];
                NiSprintf(acBuf, 256,
                    "WARNING:: [%s] No shader requirement desc found.\n",
                    pcShaderName);
                DtExt_Err(acBuf);
            }


            // Handle a blank Name
            if (pcNameText == NULL)
                pcNameText = "No Name";

            // Handle a blank description
            if (pcDescText == NULL)
                pcDescText = " ";

            // Determine the length
            stLen = strlen(pcShaderString) + strlen(pcImpSep) + 
                strlen(pcNameText) + strlen(pcImpSep) + 
                strlen(pcDescText) + 1;
            pcTemp = NiAlloc(char, stLen);

            // Create the String
            NiSprintf(pcTemp, stLen, "%s%s%s%s%s", pcShaderString, 
                pcImpSep, pcNameText, pcImpSep, pcDescText);

            // Delete the old and Save the new implementation
            NiFree(pcShaderString);
            pcShaderString = pcTemp;
        }


        int iNumUnHiddenAttributes = 0;

        // Loop trough all of the Attributes Finding the number of 
        // Unhidden ones
        unsigned int uiALoop;
        for (uiALoop = 0; uiALoop < pkDesc->GetNumberOfAttributes(); 
            uiALoop++)
        {
            const NiShaderAttributeDesc* pkAttrDesc = NULL;
            if (uiALoop == 0)
                pkAttrDesc = pkDesc->GetFirstAttribute();
            else
                pkAttrDesc = pkDesc->GetNextAttribute();

            // Only show attributes which that arn't hidden
            if (!pkAttrDesc->IsHidden())
            {
                iNumUnHiddenAttributes++;
            }
        }

        // Append on the number of Attributes
        stLen = strlen(pcShaderString) + strlen(pcShaderSep) + 4 + 1;
        pcTemp = NiAlloc(char, stLen);
        NiSprintf(pcTemp, stLen, "%s%s%d", pcShaderString, pcShaderSep, 
            iNumUnHiddenAttributes);

        NiFree(pcShaderString);
        pcShaderString = pcTemp;

        // Loop trough all of the Attributes
        for (uiALoop = 0; uiALoop < pkDesc->GetNumberOfAttributes(); 
            uiALoop++)
        {
            const NiShaderAttributeDesc* pkAttrDesc = NULL;
            if (uiALoop == 0)
                pkAttrDesc = pkDesc->GetFirstAttribute();
            else
                pkAttrDesc = pkDesc->GetNextAttribute();

            // Only show attributes which that arn't hidden
            if (!pkAttrDesc->IsHidden())
            {
                // Create the Attribute String
                char* pcAString = CreateAttributeString(pkAttrDesc);
                
                stLen = strlen(pcShaderString) + strlen(pcAttrSep) + 
                    strlen(pcAString) + 1;
                pcTemp = NiAlloc(char, stLen);
                NiSprintf(pcTemp, stLen, "%s%s%s", pcShaderString, 
                    pcAttrSep, pcAString);

                NiFree(pcAString);
                
                NiFree(pcShaderString);
                pcShaderString = pcTemp;
            }
        }


        // Remove all New Lines from the descriptions and replace 
        // with "\n"
        char* pcTemp = pcShaderString;
        while (*pcTemp != '\0')
        {
            if (((*pcTemp == 10) || (*pcTemp == 13)) &&
               ((*(pcTemp+1) == 10) || (*(pcTemp+1) == 13)))
            {
                *pcTemp++ = '\\';
                *pcTemp = 'n';
            }
            else if (((*pcTemp == 10) || (*pcTemp == 13)) &&
               ((*(pcTemp+1) != 10) && (*(pcTemp+1) != 13)))
            {
                //convert to a space.
                *pcTemp = ' ';
            }
            pcTemp++;
        }


        // Add the string to the OptionsVar
        MString MSString(pcShaderString);
        MStatus kStatus = MGlobal::executeCommand(
            (MString("optionVar -sva MyImmersePixelShaderData \"") + 
            MSString + MString("\"")),true,true);

        NIASSERT(kStatus == MS::kSuccess);

        pkDescIterator++;
        
        NiFree(pcShaderString);
    }
}
//---------------------------------------------------------------------------
char* NifExportPlugin::CreateAttributeString(
    const NiShaderAttributeDesc* pkDesc)
{
    if (pkDesc->IsHidden())
        return "";
    const char* pcName = pkDesc->GetName();
    const char* pcDescription = pkDesc->GetDescription();
    char acBuffer[2048] = "";
    char* pcSep = "|";

    // Check for Errors
    if (pcName == NULL)
        return " ";

    if (pcDescription == NULL)
        pcDescription = " ";

    switch(pkDesc->GetType())
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
        {
            bool bVal = false;
            pkDesc->GetValue_Bool(bVal);
            NiSprintf(acBuffer, 2048, "%sBoolean%s%d", pcSep, pcSep, bVal);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
        {
            const char* pcString;
            pkDesc->GetValue_String(pcString);
            NiSprintf(acBuffer, 2048, "%sString%s%s", pcSep, pcSep, pcString);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            unsigned int uiVal = 0;
            unsigned int uiMin = 0;
            unsigned int uiMax = 0;
            pkDesc->GetValue_UnsignedInt(uiVal);

            if (pkDesc->IsRanged())
            {
                pkDesc->GetRange_UnsignedInt(uiMin, uiMax);
            }

            NiSprintf(acBuffer, 2048, "%sUnsignedInt%s%d%s%d%s%d%s%d", pcSep, 
                pcSep, uiVal, pcSep, pkDesc->IsRanged(), pcSep, uiMin, pcSep, 
                uiMax);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            float fVal = 0.0f;
            float fMin = 0.0f;
            float fMax = 0.0f;

            pkDesc->GetValue_Float(fVal);

            if (pkDesc->IsRanged())
            {
                pkDesc->GetRange_Float(fMin, fMax);
            }
            NiSprintf(acBuffer, 2048, "%sFloat%s%f%s%d%s%f%s%f", pcSep, pcSep,
                fVal, pcSep, pkDesc->IsRanged(), pcSep, fMin, pcSep, fMax);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            NiPoint2 kVal = NiPoint2::ZERO;
            NiPoint2 kMin = NiPoint2::ZERO;
            NiPoint2 kMax = NiPoint2::ZERO;

            pkDesc->GetValue_Point2(kVal);

            if (pkDesc->IsRanged())
            {
                pkDesc->GetRange_Point2(kMin, kMax);
            }

            NiSprintf(acBuffer, 2048, "%sPoint2%s%f%s%f%s%d%s%f%s%f%s%f%s%f", 
                pcSep, pcSep, 
                kVal.x, pcSep, kVal.y, pcSep, 
                pkDesc->IsRanged(), pcSep, 
                kMin.x, pcSep, kMin.y, pcSep, 
                kMax.x, pcSep, kMax.y);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            NiPoint3 kVal = NiPoint3::ZERO;
            NiPoint3 kMin = NiPoint3::ZERO;
            NiPoint3 kMax = NiPoint3::ZERO;

            pkDesc->GetValue_Point3(kVal);

            if (pkDesc->IsRanged())
            {
                pkDesc->GetRange_Point3(kMin, kMax);
            }

            NiSprintf(acBuffer, 2048,
                "%sPoint3%s%f%s%f%s%f%s%d%s%f%s%f%s%f%s%f%s%f%s%f", 
                pcSep, pcSep, 
                kVal.x, pcSep, kVal.y, pcSep, kVal.z, pcSep, 
                pkDesc->IsRanged(), pcSep, 
                kMin.x, pcSep, kMin.y, pcSep, kMin.z, pcSep, 
                kMax.x, pcSep, kMax.y, pcSep, kMax.z);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            float* pfVal = NiAlloc(float, 4);
            float* pfMin = NiAlloc(float, 4);
            float* pfMax = NiAlloc(float, 4);

            for (int iLoop = 0; iLoop < 4; iLoop++)
            {
                pfVal[iLoop] = 0.0f;
                pfMin[iLoop] = 0.0f;
                pfMax[iLoop] = 0.0f;
            }


            pkDesc->GetValue_Point4(pfVal);

            if (pkDesc->IsRanged())
            {
                pkDesc->GetRange_Floats(4, (float *)pfMin, (float *)pfMax);
            }

            NiSprintf(acBuffer, 2048,
              "%sPoint4%s%f%s%f%s%f%s%f%s%d%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f",
                pcSep, pcSep, 
                pfVal[0], pcSep, pfVal[1], pcSep, pfVal[2], pcSep, pfVal[3], 
                pcSep, pkDesc->IsRanged(), pcSep, 
                pfMin[0], pcSep, pfMin[1], pcSep, pfMin[2], pcSep, pfMin[3], 
                pcSep, pfMax[0], pcSep, pfMax[1], pcSep, pfMax[2], pcSep, 
                pfMax[3]);

            NiFree(pfVal);
            NiFree(pfMin);
            NiFree(pfMax);

        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            NiMatrix3 kVal = NiMatrix3::IDENTITY;

            pkDesc->GetValue_Matrix3(kVal);
            NiSprintf(acBuffer, 2048,
                "%sMatrix%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f", 
                pcSep, pcSep,  
                kVal.GetEntry(0, 0), pcSep, kVal.GetEntry(0, 1), 
                    pcSep, kVal.GetEntry(0, 2), pcSep, 
                kVal.GetEntry(1, 0), pcSep, kVal.GetEntry(1, 1),
                    pcSep, kVal.GetEntry(1, 2), pcSep, 
                kVal.GetEntry(2, 0), pcSep, kVal.GetEntry(2, 1), 
                    pcSep, kVal.GetEntry(2, 2));
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            float afVal[16];
            float* pfVal = &afVal[0];

            int iLoop;
            for (iLoop = 0; iLoop < 16; iLoop++)
                pfVal[iLoop] = 0.0f;

            pkDesc->GetValue_Matrix4(pfVal, 16 * sizeof(float));
            NiSprintf(acBuffer, 2048, 
                "%sTransform%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s"
                "%f%s%f%s%f%s%f%s%f%s%f%s%f", pcSep, pcSep,
                pfVal[0], pcSep, pfVal[1], pcSep, pfVal[2], pcSep, pfVal[3],
                    pcSep,
                pfVal[4], pcSep, pfVal[5], pcSep, pfVal[6], pcSep, pfVal[7], 
                    pcSep,
                pfVal[8], pcSep, pfVal[9], pcSep, pfVal[10], pcSep, pfVal[11], 
                    pcSep,
                pfVal[12], pcSep, pfVal[13], pcSep, pfVal[14], pcSep, 
                    pfVal[15]);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            NiColorA kVal = NiColorA::WHITE;
            NiColorA kMin = NiColorA::WHITE;
            NiColorA kMax = NiColorA::WHITE;

            pkDesc->GetValue_ColorA(kVal);

            if (pkDesc->IsRanged())
            {
                pkDesc->GetRange_ColorA(kMin, kMax);
            }
            NiSprintf(acBuffer, 2048,
                "%sColorA%s%f%s%f%s%f%s%f%s%d%s%f%s%f%s%f%s%f"
                "%s%f%s%f%s%f%s%f", pcSep, pcSep, 
                kVal.r, pcSep, kVal.g, pcSep, kVal.b, pcSep, kVal.a, pcSep,
                pkDesc->IsRanged(), pcSep,
                kMin.r, pcSep, kMin.g, pcSep, kMin.b, pcSep, kMin.a, pcSep,
                kMax.r, pcSep, kMax.g, pcSep, kMax.b, pcSep, kMax.a, pcSep);

        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
        {
            unsigned int uiMap;
            const char* pcFileName;

            pkDesc->GetValue_Texture(uiMap, pcFileName);

            if (pcFileName == NULL)
                pcFileName = " ";

            NiSprintf(acBuffer, 2048, "%sTexture%s%d%s%s", pcSep, pcSep,
                uiMap, pcSep, pcFileName);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8:
        {
            float afVal[16];
            float* pfVal = &afVal[0];

            int iLoop;
            for (iLoop = 0; iLoop < 16; iLoop++)
                pfVal[iLoop] = 0.0f;

            // Always access the values using only the first 8
            // values will be used
            pkDesc->GetValue_Matrix4(pfVal, 16 * sizeof(float));

            NiSprintf(acBuffer, 2048, 
                "%sFloat8%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f", pcSep, pcSep,
                pfVal[0], pcSep, pfVal[1], pcSep, pfVal[2], pcSep, pfVal[3],
                pcSep, pfVal[4], pcSep, pfVal[5], pcSep, pfVal[6], pcSep, 
                pfVal[7]);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12:
        {
            float afVal[16];
            float* pfVal = &afVal[0];

            int iLoop;
            for (iLoop = 0; iLoop < 16; iLoop++)
                pfVal[iLoop] = 0.0f;

            // Always access the values using 
            pkDesc->GetValue_Matrix4(pfVal, 16 * sizeof(float));

            NiSprintf(acBuffer, 2048, 
                "%sFloat12%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s"
                "%f%s%f%s%f%s%f", pcSep, pcSep,
                pfVal[0], pcSep, pfVal[1], pcSep, pfVal[2], pcSep, pfVal[3],
                pcSep, pfVal[4], pcSep, pfVal[5], pcSep, pfVal[6], pcSep, 
                pfVal[7], pcSep, pfVal[8], pcSep, pfVal[9], pcSep, pfVal[10], 
                pcSep, pfVal[11]);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
        CreateArrayAttributeString(pkDesc, acBuffer);
        break;
    default:
        NIASSERT(0);
    }

    size_t stLen = strlen(pcName) + strlen(pcSep) + 
        strlen(pcDescription) + strlen(pcSep) + strlen(acBuffer) + 1;
    char* pcAttrString = NiAlloc(char, stLen);

    NiSprintf(pcAttrString, stLen, "%s%s%s%s", pcName, pcSep, pcDescription, 
        acBuffer);

    return pcAttrString;
}
//---------------------------------------------------------------------------
void NifExportPlugin::CreateArrayAttributeString(
    const NiShaderAttributeDesc* pkDesc, char* pcBuffer)
{
    char* pcSep = "|";
    char pcTempBuffer[2048];
    NiShaderAttributeDesc::AttributeType eSubType; 
    unsigned int uiElementSize; 
    unsigned int uiNumElements;

    if (!pkDesc->GetArrayParams(eSubType, uiElementSize, uiNumElements))
        return;

    char* pcElements = NiAlloc(char, uiElementSize * uiNumElements);
    char* pcMin = NiAlloc(char, uiElementSize * uiNumElements);
    char* pcMax = NiAlloc(char, uiElementSize * uiNumElements);

    pkDesc->GetValue_Array((void*&)pcElements, uiElementSize * uiNumElements);
    pkDesc->GetRange_Array((void*&)pcMin, (void*&)pcMax, 
        uiElementSize * uiNumElements);

    unsigned int uiLoop;

    switch(eSubType)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
        {
            NiSprintf(pcBuffer, 2048, "%sArray,Boolean,%d%", pcSep, 
                uiNumElements);

            // Add the Default Values
            for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
            {
                NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, 
                    ((bool*)pcElements)[uiLoop]);

                NiStrcat(pcBuffer, 2048, pcTempBuffer);
            }

            // Add the Ranged Flag
            NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, pkDesc->IsRanged());
            NiStrcat(pcBuffer, 2048, pcTempBuffer);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            NIASSERT(false); // Unsupported
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            NiSprintf(pcBuffer, 2048, "%sArray,UnsignedInt,%d%", pcSep, 
                uiNumElements);

            // Add the Default Values
            for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
            {
                NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, 
                    ((unsigned int*)pcElements)[uiLoop]);

                NiStrcat(pcBuffer, 2048, pcTempBuffer);
            }

            // Add the Ranged Flag
            NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, pkDesc->IsRanged());
            NiStrcat(pcBuffer, 2048, pcTempBuffer);

            if (pkDesc->IsRanged())
            {
                // Add the Min Values
                for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
                {
                    NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, 
                        ((unsigned int*)pcMin)[uiLoop]);

                    NiStrcat(pcBuffer, 2048, pcTempBuffer);
                }

                // Add the Min Values
                for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
                {
                    NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, 
                        ((unsigned int*)pcMax)[uiLoop]);

                    NiStrcat(pcBuffer, 2048, pcTempBuffer);
                }
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            NiSprintf(pcBuffer, 2048, "%sArray,Float,%d%", pcSep, 
                uiNumElements);

            // Add the Default Values
            for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
            {
                NiSprintf(pcTempBuffer, 2048, "%s%f", pcSep, 
                    ((float*)pcElements)[uiLoop]);

                NiStrcat(pcBuffer, 2048, pcTempBuffer);
            }

            // Add the Ranged Flag
            NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, pkDesc->IsRanged());
            NiStrcat(pcBuffer, 2048, pcTempBuffer);

            if (pkDesc->IsRanged())
            {
                // Add the Min Values
                for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
                {
                    NiSprintf(pcTempBuffer, 2048, "%s%f", pcSep, 
                        ((float*)pcMin)[uiLoop]);

                    NiStrcat(pcBuffer, 2048, pcTempBuffer);
                }

                // Add the Min Values
                for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
                {
                    NiSprintf(pcTempBuffer, 2048, "%s%f", pcSep, 
                        ((float*)pcMax)[uiLoop]);

                    NiStrcat(pcBuffer, 2048, pcTempBuffer);
                }
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            NiSprintf(pcBuffer, 2048, "%sArray,Point2,%d%", pcSep, 
                uiNumElements);

            // Add the Default Values
            for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
            {
                NiSprintf(pcTempBuffer, 2048, "%s%f%s%f", pcSep, 
                    ((NiPoint2*)pcElements)[uiLoop].x, pcSep, 
                    ((NiPoint2*)pcElements)[uiLoop].y);
                NiStrcat(pcBuffer, 2048, pcTempBuffer);
            }

            // Add the Ranged Flag
            NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, pkDesc->IsRanged());
            NiStrcat(pcBuffer, 2048, pcTempBuffer);

            if (pkDesc->IsRanged())
            {
                // Add the Min Values
                for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
                {
                    NiSprintf(pcTempBuffer, 2048, "%s%f%s%f", pcSep, 
                        ((NiPoint2*)pcMin)[uiLoop].x, pcSep, 
                        ((NiPoint2*)pcMin)[uiLoop].y);
                    NiStrcat(pcBuffer, 2048, pcTempBuffer);
                }

                // Add the Min Values
                for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
                {
                    NiSprintf(pcTempBuffer, 2048, "%s%f%s%f", pcSep, 
                        ((NiPoint2*)pcMax)[uiLoop].x, pcSep, 
                        ((NiPoint2*)pcMax)[uiLoop].y);
                    NiStrcat(pcBuffer, 2048, pcTempBuffer);
                }
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            NiSprintf(pcBuffer, 2048, "%sArray,Point3,%d%", pcSep, 
                uiNumElements);

            // Add the Default Values
            for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
            {
                NiSprintf(pcTempBuffer, 2048, "%s%f%s%f%s%f", pcSep, 
                    ((NiPoint3*)pcElements)[uiLoop].x, pcSep, 
                    ((NiPoint3*)pcElements)[uiLoop].y, pcSep, 
                    ((NiPoint3*)pcElements)[uiLoop].z);
                NiStrcat(pcBuffer, 2048, pcTempBuffer);
            }

            // Add the Ranged Flag
            NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, pkDesc->IsRanged());
            NiStrcat(pcBuffer, 2048, pcTempBuffer);

            if (pkDesc->IsRanged())
            {
                // Add the Min Values
                for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
                {
                    NiSprintf(pcTempBuffer, 2048, "%s%f%s%f%s%f", pcSep, 
                        ((NiPoint3*)pcMin)[uiLoop].x, pcSep, 
                        ((NiPoint3*)pcMin)[uiLoop].y, pcSep, 
                        ((NiPoint3*)pcMin)[uiLoop].z);
                    NiStrcat(pcBuffer, 2048, pcTempBuffer);
                }

                // Add the Min Values
                for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
                {
                    NiSprintf(pcTempBuffer, 2048, "%s%f%s%f%s%f", pcSep, 
                        ((NiPoint3*)pcMax)[uiLoop].x, pcSep, 
                        ((NiPoint3*)pcMax)[uiLoop].y, pcSep, 
                        ((NiPoint3*)pcMax)[uiLoop].z);
                    NiStrcat(pcBuffer, 2048, pcTempBuffer);
                }
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            NiSprintf(pcBuffer, 2048, "%sArray,Point4,%d%", pcSep, 
                uiNumElements);

            // FLOAT 4 IS JUST AN ARRAY SO MULTIPLY THE NUM ELEMENTS BY 4
            uiNumElements *= 4;

            // Add the Default Values
            for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
            {
                NiSprintf(pcTempBuffer, 2048, "%s%f", pcSep, 
                    ((float*)pcElements)[uiLoop]);

                NiStrcat(pcBuffer, 2048, pcTempBuffer);
            }

            // Add the Ranged Flag
            NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, pkDesc->IsRanged());
            NiStrcat(pcBuffer, 2048, pcTempBuffer);

            if (pkDesc->IsRanged())
            {
                // Add the Min Values
                for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
                {
                    NiSprintf(pcTempBuffer, 2048, "%s%f", pcSep, 
                        ((float*)pcMin)[uiLoop]);

                    NiStrcat(pcBuffer, 2048, pcTempBuffer);
                }

                // Add the Min Values
                for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
                {
                    NiSprintf(pcTempBuffer, 2048, "%s%f", pcSep, 
                        ((float*)pcMax)[uiLoop]);

                    NiStrcat(pcBuffer, 2048, pcTempBuffer);
                }
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            NiSprintf(pcBuffer, 2048, "%sArray,Matrix3,%d%", pcSep, 
                uiNumElements);

            // FLOAT 4 IS JUST AN ARRAY OF FLOATS MULTIPLY THE NUM ELEMENTS 
            // BY 9
            uiNumElements *= 9;

            // Add the Default Values
            for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
            {
                NiSprintf(pcTempBuffer, 2048, "%s%f", pcSep, 
                    ((float*)pcElements)[uiLoop]);

                NiStrcat(pcBuffer, 2048, pcTempBuffer);
            }

            // Add the Ranged Flag
            NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, pkDesc->IsRanged());
            NiStrcat(pcBuffer, 2048, pcTempBuffer);

            // MATRIXES DON"T SUPPRT RANGES
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            NiSprintf(pcBuffer, 2048, "%sArray,Matrix4,%d%", pcSep, 
                uiNumElements);

            // FLOAT 4 IS JUST AN ARRAY OF FLOATS MULTIPLY THE NUM ELEMENTS 
            // BY 16
            uiNumElements *= 16;

            // Add the Default Values
            for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
            {
                NiSprintf(pcTempBuffer, 2048, "%s%f", pcSep, 
                    ((float*)pcElements)[uiLoop]);

                NiStrcat(pcBuffer, 2048, pcTempBuffer);
            }

            // Add the Ranged Flag
            NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, pkDesc->IsRanged());
            NiStrcat(pcBuffer, 2048, pcTempBuffer);

            // MATRIXES DON"T SUPPRT RANGES
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            NiSprintf(pcBuffer, 2048, "%sArray,ColorA,%d%", pcSep, 
                uiNumElements);

            // Add the Default Values
            for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
            {
                NiSprintf(pcTempBuffer, 2048, "%s%f%s%f%s%f%s%f", pcSep, 
                    ((NiColorA*)pcElements)[uiLoop].r, pcSep, 
                    ((NiColorA*)pcElements)[uiLoop].g, pcSep, 
                    ((NiColorA*)pcElements)[uiLoop].b, pcSep,
                    ((NiColorA*)pcElements)[uiLoop].a);
                NiStrcat(pcBuffer, 2048, pcTempBuffer);
            }

            // Add the Ranged Flag
            NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, pkDesc->IsRanged());
            NiStrcat(pcBuffer, 2048, pcTempBuffer);

            if (pkDesc->IsRanged())
            {
                // Add the Min Values
                for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
                {
                    NiSprintf(pcTempBuffer, 2048, "%s%f%s%f%s%f%s%f", pcSep, 
                        ((NiColorA*)pcMin)[uiLoop].r, pcSep, 
                        ((NiColorA*)pcMin)[uiLoop].g, pcSep, 
                        ((NiColorA*)pcMin)[uiLoop].b, pcSep,
                        ((NiColorA*)pcMin)[uiLoop].a);
                    NiStrcat(pcBuffer, 2048, pcTempBuffer);
                }

                // Add the Min Values
                for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
                {
                    NiSprintf(pcTempBuffer, 2048, "%s%f%s%f%s%f%s%f", pcSep, 
                        ((NiColorA*)pcMax)[uiLoop].r, pcSep, 
                        ((NiColorA*)pcMax)[uiLoop].g, pcSep, 
                        ((NiColorA*)pcMax)[uiLoop].b, pcSep,
                        ((NiColorA*)pcMax)[uiLoop].a);
                    NiStrcat(pcBuffer, 2048, pcTempBuffer);
                }
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
        NIASSERT(false); // Unsupported
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8:
        {
            NiSprintf(pcBuffer, 2048, "%sArray,Float8,%d%", pcSep, 
                uiNumElements);

            // FLOAT 8 IS JUST AN ARRAY SO MULTIPLY THE NUM ELEMENTS BY 8
            uiNumElements *= 8;

            // Add the Default Values
            for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
            {
                NiSprintf(pcTempBuffer, 2048, "%s%f", pcSep, 
                    ((float*)pcElements)[uiLoop]);

                NiStrcat(pcBuffer, 2048, pcTempBuffer);
            }

            // Add the Ranged Flag
            NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, pkDesc->IsRanged());
            NiStrcat(pcBuffer, 2048, pcTempBuffer);

            // Range is Not supported
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12:
        {
            NiSprintf(pcBuffer, 2048, "%sArray,Float12,%d%", pcSep, 
                uiNumElements);

            // FLOAT 12 IS JUST AN ARRAY SO MULTIPLY THE NUM ELEMENTS BY 12
            uiNumElements *= 12;

            // Add the Default Values
            for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
            {
                NiSprintf(pcTempBuffer, 2048, "%s%f", pcSep, 
                    ((float*)pcElements)[uiLoop]);

                NiStrcat(pcBuffer, 2048, pcTempBuffer);
            }

            // Add the Ranged Flag
            NiSprintf(pcTempBuffer, 2048, "%s%d", pcSep, pkDesc->IsRanged());
            NiStrcat(pcBuffer, 2048, pcTempBuffer);

            // Range is Not supported
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
        NIASSERT(0);  // We dont support arrays of arrays
        break;
    default:
        NIASSERT(0);
    }


    // Delete the Element Arrays
    NiFree(pcElements);
    NiFree(pcMin);
    NiFree(pcMax);
}
//---------------------------------------------------------------------------
void NifExportPlugin::SetAvaiableViewers()
{
        // Clear The Run Silently flag just in case
    MGlobal::executeCommand("optionVar -rm GamebryoAvailableViewers");

    NiScriptTemplateManager* pkManager = 
        NiScriptTemplateManager::GetInstance();

    if (NiPluginManager::GetInstance()->GetPluginByClass(
        "NiSceneViewerD3DPlugin"))
    {
        MGlobal::executeCommand(
            "optionVar -sva GamebryoAvailableViewers NiSceneViewerD3DPlugin");
    }

    if (NiPluginManager::GetInstance()->GetPluginByClass("KFMViewerPlugin"))
    {
        MGlobal::executeCommand(
            "optionVar -sva GamebryoAvailableViewers KFMViewerPlugin");
    }

    if (NiPluginManager::GetInstance()->GetPluginByClass("NIFViewerPlugin"))
    {
        MGlobal::executeCommand(
            "optionVar -sva GamebryoAvailableViewers NIFViewerPlugin");
    }

    if (NiPluginManager::GetInstance()->GetPluginByClass(
        "NiXbox360ViewerPlugin"))
    {
        MGlobal::executeCommand(
            "optionVar -sva GamebryoAvailableViewers NiXbox360ViewerPlugin");
    }

    if (NiPluginManager::GetInstance()->GetPluginByClass(
        "NiPS3ViewerPlugin"))
    {
        MGlobal::executeCommand(
            "optionVar -sva GamebryoAvailableViewers NiPS3ViewerPlugin");
    }

    if (NiPluginManager::GetInstance()->GetPluginByClass(
        "NiWiiViewerPlugin"))
    {
        MGlobal::executeCommand(
            "optionVar -sva GamebryoAvailableViewers NiWiiViewerPlugin");
    }

    if (NiPluginManager::GetInstance()->GetPluginByClass(
        "NiSceneViewerD3DPlugin"))
    {
        MGlobal::executeCommand(
            "optionVar -sva GamebryoAvailableViewers NiSceneViewerD3D10Plugin");
    }

    MGlobal::executeCommand("optionVar -rm CustomNIFViewers");
    for (unsigned int ui = 0; ui < pkManager->GetScriptCount(); ui++)
    {
        NiScriptInfo* pkScript = pkManager->GetScriptAt(ui);
        if (pkScript->GetType() == "VIEWER")
        {
            char acString[1024];
            NiSprintf(acString, 1024, "optionVar -sva CustomNIFViewers %s",
                (const char*)pkScript->GetName());
            MGlobal::executeCommand(acString);
        }
    }


}
