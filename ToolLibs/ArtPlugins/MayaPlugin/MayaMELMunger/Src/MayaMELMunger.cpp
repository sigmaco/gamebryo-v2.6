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
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

#include "MayaMelMungerPCH.h"

typedef std::basic_string<TCHAR> tstring;
typedef std::basic_ifstream<TCHAR> tifstream;
typedef std::basic_ofstream<TCHAR> tofstream;

using namespace std;

tstring* g_pkFilesToReplace;

tstring g_DefaultScriptPath;
tstring g_OthersScriptPath;
tstring g_BackupPath;
tstring g_kMayaInstallPath;

tifstream g_FileStream;
tofstream g_LogFile;

const tstring PREFIX = _T("_gb_");
//just a guess
const int MAX_LINE_LENGTH = 500;
const int MAX_FILES_TO_DUPLICATE = 20;

//-----------------------------------------------------------------------------
//Function Prototypes
//-----------------------------------------------------------------------------
bool CopyFileAndRenameFunction(const tstring& kFileToCopy, 
                               const tstring& kFileToMake,
                               const tstring& kOldFunctionName,
                               const tstring& kNewFunctionName);


//-----------------------------------------------------------------------------
void Init(const tstring& kMayaInstallPath)
{
    //Setup paths
    g_kMayaInstallPath = kMayaInstallPath;
    g_DefaultScriptPath = kMayaInstallPath + _T("\\scripts\\AETemplates\\");
    g_OthersScriptPath = kMayaInstallPath + _T("\\scripts\\others\\");
    g_BackupPath =  kMayaInstallPath + _T("\\scripts\\GamebryoBackup\\");
}
//-----------------------------------------------------------------------------
bool FileExists(const tstring& filePath)
{
    bool bReturnValue = true;
    
    g_FileStream.clear();
    
    //Check to see if the file exists by trying to open it
    g_FileStream.open(filePath.c_str());
    bReturnValue = g_FileStream.fail() ? false : true;
    g_FileStream.close();

    return bReturnValue;
}
//-----------------------------------------------------------------------------
bool IsOverridenFile(const tstring& fileName)
{
    bool bReturnValue = true;

    tstring otherPath = g_OthersScriptPath + fileName + _T(".mel");
    //if the override file exists don't use the default
    bReturnValue = FileExists(otherPath);
    
    return bReturnValue;
}
//-----------------------------------------------------------------------------
bool TestForPriorAlias(const tstring& fileName)
{
    bool bReturnValue = true;

    tstring otherPath = g_OthersScriptPath + PREFIX + fileName + _T(".mel");

    bReturnValue = FileExists(otherPath);

    return bReturnValue;
}
//-----------------------------------------------------------------------------
bool MakeAliasedFile(const tstring& kFileName, 
                     const tstring& kFileToCopy, 
                     const tstring& kFileToMake)
{

    bool bReturnValue = true;

    tstring kMainFunctionName = _T("global proc ") + kFileName;
    tstring kNewMainFunctionName = _T("global proc ") + PREFIX + kFileName;

    bReturnValue = CopyFileAndRenameFunction(kFileToCopy, kFileToMake, 
        kMainFunctionName, kNewMainFunctionName);

    return bReturnValue;
}
//-----------------------------------------------------------------------------
bool CopyFileAndRenameFunction(const tstring& kFileToCopy, 
                           const tstring& kFileToMake,
                           const tstring& kOldFunctionName,
                           const tstring& kNewFunctionName)
{
    bool bReturnValue = true;
    
    if(FileExists(kFileToCopy))
    {
        tifstream oldFile(kFileToCopy.c_str());
        tofstream aliasedFile(kFileToMake.c_str());

        while(!oldFile.eof())
        {
            _TCHAR lineBuffer[MAX_LINE_LENGTH];
            oldFile.getline(lineBuffer, MAX_LINE_LENGTH);
            tstring line = lineBuffer;

            if(line.compare(0, kOldFunctionName.length(), kOldFunctionName) 
                == 0)
            {
                line.replace(0, kOldFunctionName.length(), 
                    kNewFunctionName);
            }

            aliasedFile << line << "\n";
        }

        oldFile.close();
        aliasedFile.close();
    }

    return bReturnValue;
}
//-----------------------------------------------------------------------------
bool ShouldMakeBackup(const tstring& fileName)
{
    bool bReturnValue = true;
    
    g_FileStream.clear();
    //if there is already a backup don't make another one
    tstring filePath = g_OthersScriptPath + fileName + _T(".mel");
    g_FileStream.open(filePath.c_str());

    //awesome magic line length
    _TCHAR lineBuffer[MAX_LINE_LENGTH];
    g_FileStream.getline(lineBuffer, MAX_LINE_LENGTH);

    g_FileStream.close();

    tstring firstLine(lineBuffer);

    if(firstLine.find(_T("Gamebryo Forwarding File")) == tstring::npos)
    {
        //couldn't find the "Gamebryo Forwarding File"
        bReturnValue = true;
    }
    else
    {
        //is a "Gamebryo Forwarding File" don't not make a copy
        bReturnValue = false;
    }

    return bReturnValue;
}
//-----------------------------------------------------------------------------
void GetBackupPath(const tstring& fileName, tstring& output)
{
    output = g_BackupPath + fileName + _T(".mel");
}
//-----------------------------------------------------------------------------
void MakeBackup(const tstring& filePath, const tstring& backupPath)
{
    //Log out arguments
    g_LogFile << _T("File To Backup ") << filePath.c_str() << "\n";
    g_LogFile << "Backup Path = " << backupPath.c_str() << "\n";

    //copy file to backup
    g_FileStream.clear();
    g_FileStream.open(filePath.c_str());

    tofstream backupFileStream(backupPath.c_str());
    
    if(!g_FileStream.fail())
    {
        while(!g_FileStream.eof())
        {
            _TCHAR lineBuffer[MAX_LINE_LENGTH];
            g_FileStream.getline(lineBuffer, MAX_LINE_LENGTH);
            tstring line = lineBuffer;
            backupFileStream << line << "\n";
        }
    }
    else
    {
        g_LogFile << _T("Failed to Load ") << filePath.c_str() << endl;
    }

    g_FileStream.close();
}
//-----------------------------------------------------------------------------
bool BackupExists(const tstring& fileName)
{
    bool bReturnValue = true;

    tstring backupPath;
    GetBackupPath(fileName, backupPath);

    bReturnValue = FileExists(backupPath);

    return bReturnValue;
}
//-----------------------------------------------------------------------------
bool OverrideFile(const tstring&, const tstring& fileName)
{
    bool bReturnValue = true;  

    tstring fileToCopy;
    tstring fileToMake;

    if(TestForPriorAlias(fileName))
    {
        goto exit;
    }

    //see if there is an overridden file already. This would mean that a 
    //customer has made a custom version for some reason
    if(!IsOverridenFile(fileName))
    {
        //if there is not copy the default maya one
        //and append _ on it and the main function
        fileToCopy = g_DefaultScriptPath + fileName + _T(".mel");
        fileToMake = g_OthersScriptPath + PREFIX + fileName + _T(".mel");
        MakeAliasedFile(fileName, fileToCopy, fileToMake);
    }
    else
    {
        //if there is make a backup of it
        if(ShouldMakeBackup(fileName))
        {
            tstring backupPath;
            GetBackupPath(fileName, backupPath);

            tstring filePath = g_OthersScriptPath + fileName + _T(".mel");
            MakeBackup(filePath, backupPath);
            fileToCopy = backupPath;
        }
        else
        {
            //This a weird case.There is an overriden file, but it is the 
            //Gamebryo Forwarding File. So either for some reason the aliased
            //file was removed.
            //regenerate the aliased file from either the backup copy
            //or if there is not a backup use the default file.
            if(BackupExists(fileName))
            {
                GetBackupPath(fileName, fileToCopy);
            }
            else
            {
                fileToCopy = g_DefaultScriptPath + fileName + _T(".mel");
            }
        }

        //and PREFIX on it and the main function
        fileToMake = g_OthersScriptPath + PREFIX + fileName + _T(".mel");

        MakeAliasedFile(fileName, fileToCopy, fileToMake);
    }

exit:
    return bReturnValue;
}
//-----------------------------------------------------------------------------
bool ParseArguments(int argc, _TCHAR* argv[], tstring& kMayaInstallPath, 
    tstring& kLogPath, bool& bInstall, tstring& kErrorMessage)
{
    bool bReturnValue = true;

    //setup defaults
    bInstall = true;
    kLogPath = argv[0];
    kLogPath += _T(".log");
    kErrorMessage = _T("");

    for(int argIndex = 1; argIndex < argc; argIndex++)
    {
        tstring kArg = argv[argIndex];
        
        //look at the flag and decide what it is
        if(kArg.substr(0, 4) == _T("-log"))
        {
            unsigned int uiValueIndex = (unsigned int)kArg.find(_T("="));
            if(uiValueIndex == tstring::npos)
            {
                //didn't find an equals
                bReturnValue = false;
                kErrorMessage += _T("-log flag did not have an \"=\" sign ")
                    _T("after it. No path could be extracted.");
            }

            kLogPath = kArg.substr(uiValueIndex + 1);
        }
        else if(kArg.substr(0, 5) == _T("-maya"))
        {
            unsigned int uiValueIndex = (unsigned int)kArg.find(_T("="));
            if(uiValueIndex == tstring::npos)
            {
                //didn't find an equals
                bReturnValue = 1;
                kErrorMessage += _T("-maya flag did not have an \"=\" sign ")
                    _T("after it. No path could be extracted.");
            }

            kMayaInstallPath = kArg.substr(uiValueIndex + 1);
        }
        else if(kArg.substr(0, 8) == _T("-install"))
        {
            unsigned int uiValueIndex = (unsigned int)kArg.find(_T("="));
            if(uiValueIndex == tstring::npos)
            {
                //didn't find an equals

                bReturnValue = false;
                kErrorMessage += _T("-install flag did not have an \"=\" sign")
                    _T(" after it. No value could be extracted.");
            }

            tstring kValue = kArg.substr(uiValueIndex + 1);

            if(kValue == _T("true"))
            {
                bInstall = true;
            }
            else if(kValue == _T("false"))
            {
                bInstall = false;       
            }
            else
            {
                //there was a problem
                bReturnValue = false;
                kErrorMessage += _T("Invalid value \"") + kValue + _T("\" )")
                    _T(" used with -install flag.");
            }
        }
        else
        {
            bReturnValue = false;
            kErrorMessage += _T("Invalid flag \"") + kArg + _T("\".");
        }
    }

    return bReturnValue;
}
//-----------------------------------------------------------------------------
bool RevertFile(tstring kFileNameToRevert)
{
    bool bReturnValue = true;

    tstring kFileToCopy;
    tstring kFileToMake;
    tstring kNewMainFunctionName;
    tstring kMainFunctionName;

    //delete forwarding file
    tstring kFileToDeletePath = g_OthersScriptPath + kFileNameToRevert + 
        _T(".mel");

    if(FileExists(kFileToDeletePath))
    {
        bReturnValue = (DeleteFile(kFileToDeletePath.c_str()) == 0) ? 
            false : true;
        if(!bReturnValue)
        {
            g_LogFile << _T("could not delete file ") << 
                kFileToDeletePath.c_str() << _T("\n");
            goto exit;
        }
    }


    //Copy alias file to new name
    kFileToCopy = g_OthersScriptPath + PREFIX + kFileNameToRevert + _T(".mel");
    kFileToMake = kFileToDeletePath;
    kNewMainFunctionName = _T("global proc ") + kFileNameToRevert;
    kMainFunctionName = _T("global proc ") + PREFIX + kFileNameToRevert;

    bReturnValue = CopyFileAndRenameFunction(kFileToCopy, kFileToMake, 
        kMainFunctionName, kNewMainFunctionName);

    if(!bReturnValue)
    {
        g_LogFile << _T("Copy and Renaming file ") << 
            kFileToDeletePath.c_str() << _T(" failed! \n");
        goto exit;
    }

    if(FileExists(kFileToCopy))
    {
        //delete aliased file
        bReturnValue = (DeleteFile(kFileToCopy.c_str()) == 0) ? false : true;
        if(!bReturnValue)
        {
            g_LogFile << _T("could not delete file ") << kFileToCopy.c_str() 
                << _T("\n");
            goto exit;
        }
    }


exit:
    return bReturnValue;
}

//-----------------------------------------------------------------------------
bool Install()
{
    bool bReturnValue = true;

    for(unsigned int uiFileIndex = 0; uiFileIndex < MAX_FILES_TO_DUPLICATE;
        uiFileIndex++)
    {
        //Conditionally make an override file
        bReturnValue = OverrideFile(g_kMayaInstallPath, 
            g_pkFilesToReplace[uiFileIndex]);
    }

    return bReturnValue;
}
//-----------------------------------------------------------------------------
bool Uninstall()
{
    bool bReturnValue = true;
    
    for(unsigned int uiFileIndex = 0; uiFileIndex < MAX_FILES_TO_DUPLICATE;
        uiFileIndex++)
    {
        bReturnValue = RevertFile(g_pkFilesToReplace[uiFileIndex]);
    }

    return bReturnValue;
}
//-----------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{    
    //Files that the Mel Munger will need to create aliases for.
    tstring FILES_TO_REPLACE[MAX_FILES_TO_DUPLICATE] = {
        _T("AEanisotropicTemplate"), 
        _T("AEblinnTemplate"), 
        _T("AEdirectionalLightTemplate"),
        _T("AEfileTemplate"), 
        _T("AEjointTemplate"), 
        _T("AElambertTemplate"), 
        _T("AElayeredShaderTemplate"), 
        _T("AElightTemplate"), 
        _T("AElodGroupTemplate"),
        _T("AEparticleTemplate"), 
        _T("AEphongETemplate"), 
        _T("AEphongTemplate"),
        _T("AEpointEmitterTemplate"), 
        _T("AEpointLightTemplate"), 
        _T("AErampShaderTemplate"),
        _T("AEshadingMapTemplate"), 
        _T("AEspotLightTemplate"), 
        _T("AEsurfaceShaderTemplate"),
        _T("AEtransformTemplate"), 
        _T("AEuseBackgroundTemplate")};

    g_pkFilesToReplace = FILES_TO_REPLACE;
    
    int bReturnValue = 0;
    tstring kMayaInstallPath;
    tstring kLogPath;
    bool bInstall = true;
    tstring kErrorMessage;

    bool bParseCorrectly = ParseArguments(argc, argv, kMayaInstallPath, 
        kLogPath, bInstall, kErrorMessage);

    g_LogFile.open(kLogPath.c_str());

    //make sure the arguments parsed correctly
    if(!bParseCorrectly)
    {
        g_LogFile << kErrorMessage.c_str() << "\n";
        bReturnValue = 1;
        goto exit;
    }

    //Log out parameter info
    g_LogFile << _T("Maya Install Path ") << kMayaInstallPath << "\n";
    
    {
        tstring kInstallationType;
        if(bInstall)
        {
            kInstallationType = _T("Install");
        }
        else
        {
            kInstallationType = _T("Uninstall");
        }
        g_LogFile << "Install Type: " << kInstallationType.c_str() << "\n";
    }

    Init(kMayaInstallPath);

    //Install or Uninstall
    bool bWorked = true;
    if(bInstall)
    {
        bWorked = Install();
    }
    else
    {
        bWorked = Uninstall();
    }

    bReturnValue = bWorked ? 0 : 1;

exit:
    return bReturnValue;
}

