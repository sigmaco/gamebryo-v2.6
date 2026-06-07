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

#include <NiMeshProfileProcessor.h>
#include <NiMeshProfileXMLParser.h>

#include <ctype.h>
#include <string.h>

char* g_pcInPath = NULL;
char* g_pcOutFile = NULL;

//---------------------------------------------------------------------------
void PrintUsage()
{
    fputs(
        "Usage: MeshProfileCompiler -in <input full path name> "
        "-out <output LOG file name>\n"
        "Note: If input path name is not given the default path is used.\n",
        stderr);
}
//---------------------------------------------------------------------------
bool CheckArguments(int argc, char** argv)
{
    //
    // Command line argument checking 
    // 
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            char* pc = argv[i];
            while (*pc)
            {
                *pc = (char)tolower(*pc); 
                pc++;
            }
        }

        if (!strcmp("-in", argv[i]))
        {
            if ((i + 1) >= argc)
                return false;

            size_t stLen = strlen(argv[i + 1]) + 1;
            g_pcInPath = new char[stLen];
            if (g_pcInPath == NULL)
                return false;
            NiStrcpy(g_pcInPath, stLen, argv[i + 1]);
            i++;
        }

        if (!strcmp("-out", argv[i]))
        {
            if ((i + 1) >= argc)
                return false;

            size_t stLen = strlen(argv[i + 1]) + 1;
            g_pcOutFile = new char[stLen];
            if (g_pcOutFile == NULL)
                return false;
            NiStrcpy(g_pcOutFile, stLen, argv[i + 1]);
            i++;
        }
    }

    if (g_pcOutFile == NULL)
    {
        PrintUsage();
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
void Cleanup()
{
    // Destroy the mesh profile processor class
    NiMeshProfileProcessor::DestroyMeshProfileProcessor();

    delete[] g_pcInPath;
    delete[] g_pcOutFile;

    NiShutdown();
}
//---------------------------------------------------------------------------
int Process(int argc, char** argv)
{
    if (!CheckArguments(argc, argv))
    {
        return 1;
    }

    // Create the mesh profile processor class
    NiMeshProfileProcessor::CreateMeshProfileProcessor();
    bool bSuccess =
        NiMeshProfileXMLParser::RegisterAllProfilesWithProcessor(
            g_pcInPath, true);

    // Check to see if the output file is a full path
    NiString kOutputFile = g_pcOutFile;
    if (kOutputFile.Find(':') < 0)
    {
        // Save it to a log file
        NiString kDirectories = g_pcInPath;

        // Load all the files from all the given directories
        int iIndex = kDirectories.Find(';', 0);

        if (iIndex > 0)
        {
            // Assumes first path is where the log is wanted and also 
            // assumes that working directory is set up correctly.
            kOutputFile = (const char*)kDirectories.GetSubstring(0, iIndex);
        }
        else
        {
            kOutputFile = g_pcInPath;
        }
        kOutputFile += "\\";
        kOutputFile += g_pcOutFile;

    }

    NiMeshProfileProcessor::GetErrorHandler().SaveMessages(
        (const char*)kOutputFile);

    printf("Saved log file %s.", (const char*)kOutputFile);

    NiMeshProfileProcessor::GetErrorHandler().ClearMessages();
    
    return (bSuccess ? 0 : 1);
}
//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    NiInit();

    NiSystemDesc::GetSystemDesc().SetToolMode(true);
    NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
        NiSystemDesc::RENDERER_GENERIC);

    int iRetValue = Process(argc, argv);

    Cleanup();

    return iRetValue;
}
//---------------------------------------------------------------------------
