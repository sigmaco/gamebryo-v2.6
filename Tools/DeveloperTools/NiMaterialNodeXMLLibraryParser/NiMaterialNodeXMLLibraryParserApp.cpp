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

#include "NiMaterialNodeXMLLibraryParser.h"

#include <ctype.h>
#include <string.h>

char* g_pcInFile = NULL;
char* g_pcOutCPPFile = NULL;
char* g_pcOutHFile = NULL;
char* g_pcClassName = NULL;
char* g_pcModifier = NULL;
char* g_pcPCH = NULL;

//---------------------------------------------------------------------------
void PrintUsage()
{
    fputs(
        "Usage: NiMaterialNodeXMLLibraryParser -in <input XML file name>\n"
        "    -class <class name>"
        "    [-modifier <storage class modifier>]\n"
        "    [-pch <PCH file name to be included>]\n"
        "    [-destdir <destination directory for generated files>]\n"
        "\n"
        "Note that <class name> must not have any whitespace in it.\n"
        "The generated source files will be named <class name>.h \n"
        "and <class name>.cpp\n", stderr);
}
//---------------------------------------------------------------------------
bool CheckArguments(int argc, char** argv)
{
    char* pcPath = NULL;
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
            g_pcInFile = new char[stLen];
            if (g_pcInFile == NULL)
                return false;
            strcpy(g_pcInFile, argv[i + 1]);
            i++;
        }

        if (!strcmp("-class", argv[i]))
        {
            if ((i + 1) >= argc)
                return false;

            size_t stLen = strlen(argv[i + 1]) + 1;
            g_pcClassName = new char[stLen];
            if (g_pcClassName == NULL)
                return false;
            strcpy(g_pcClassName, argv[i + 1]);
            i++;
        }

        if (!strcmp("-destdir", argv[i]))
        {
            if ((i + 1) >= argc)
                return false;

            size_t stLen = strlen(argv[i + 1]) + 1;
            pcPath = new char[stLen];
            if (pcPath == NULL)
                return false;
            strcpy(pcPath, argv[i + 1]);
            i++;
        }

        if (!strcmp("-modifier", argv[i]))
        {
            if ((i + 1) >= argc)
                return false;

            size_t stLen = strlen(argv[i + 1]) + 1;
            g_pcModifier = new char[stLen];
            if (g_pcModifier == NULL)
                return false;
            strcpy(g_pcModifier, argv[i + 1]);
            i++;
        }

        if (!strcmp("-pch", argv[i]))
        {
            if ((i + 1) >= argc)
                return false;

            size_t stLen = strlen(argv[i + 1]) + 1;
            g_pcPCH = new char[stLen];
            if (g_pcPCH == NULL)
                return false;
            strcpy(g_pcPCH, argv[i + 1]);
            i++;
        }
/*
        if (!strcmp("-out", argv[i]))
        {
            if ((i + 1) >= argc)
                return false;

            unsigned int uiLen = strlen(argv[i + 1]) + 1;
            g_pcOutCPPFile = new char[uiLen];
            if (g_pcOutCPPFile == NULL)
                return false;
            strcpy(g_pcOutCPPFile, argv[i + 1]);
            i++;
        }

        if (!strcmp("-header", argv[i]))
        {
            if ((i + 1) >= argc)
                return false;

            unsigned int uiLen = strlen(argv[i + 1]) + 1;
            g_pcOutHFile = new char[uiLen];
            if (g_pcOutHFile == NULL)
                return false;
            strcpy(g_pcOutHFile, argv[i + 1]);
            i++;
        }*/
    }

    // Check for well-formed class name

/*    bool bSourceFileNameOK = false;
    bool bHeaderFileNameOK = false;
    if (g_pcOutCPPFile)
    {
        unsigned int uiSourceFileLength = strlen(g_pcOutCPPFile);

        if (strncmp(g_pcOutCPPFile + (uiSourceFileLength - 4), ".cpp", 5) == 0)
        {
            bSourceFileNameOK = true;

            // Fill in header file name if it's not supplied
            if (g_pcOutHFile == NULL)
            {
                g_pcOutHFile = new char[uiSourceFileLength - 1];
                strncpy(g_pcOutHFile, g_pcOutCPPFile, uiSourceFileLength - 4);
                g_pcOutHFile[uiSourceFileLength - 4] = '.';
                g_pcOutHFile[uiSourceFileLength - 3] = 'h';
                g_pcOutHFile[uiSourceFileLength - 2] = '\0';
            }
        }
    }

    if (g_pcOutHFile)
    {
        unsigned int uiHeaderFileLength = strlen(g_pcOutHFile);

        if (strncmp(g_pcOutHFile + (uiHeaderFileLength - 2), ".h", 3) == 0)
            bHeaderFileNameOK = true;
    }*/
    bool bClassNameOK = false;
    if (g_pcClassName && isalpha(*g_pcClassName))
    {
        bClassNameOK = true;

        // Check for illegal punctuation in class name
        char* pcIter = g_pcClassName;
        while (*pcIter != '\0')
        {
            if (!isalnum(*pcIter++))
            {
                bClassNameOK = false;
                break;
            }
        }
        
        if (bClassNameOK)
        {
            size_t stClassNameLength = strlen(g_pcClassName);
            size_t stPathLength = 0;
            bool bNeedSlash = false;
            if (pcPath && *pcPath != '\0')
            {
                stPathLength = strlen(pcPath);
                if (pcPath[stPathLength -1] == '\"')
                {
                    pcPath[stPathLength - 1] = '\0';
                    stPathLength--;
                }
                
                bNeedSlash = (pcPath[stPathLength - 1] != '\\' && 
                    pcPath[stPathLength - 1] != '/');
            }
            else
            {
                size_t stStrLen = strlen(g_pcInFile);
                pcPath = new char[stStrLen + 1];
                strcpy(pcPath, g_pcInFile);

                for (int i = (int)stStrLen - 1; i >= 0; i--)
                {
                    if (pcPath[i] == '/' || pcPath[i] == '\\')
                    {
                        pcPath[i + 1] = '\0';
                        stPathLength = strlen(pcPath);
                        bNeedSlash = false;
                        break;
                    }
                }
            }
            
            // Construct source file path
            g_pcOutCPPFile = new char[stPathLength + (bNeedSlash ? 1 : 0) + 
                stClassNameLength + 4 + 1];
            g_pcOutCPPFile[0] = '\0';
            if (stPathLength != 0)
            {
                strcpy(g_pcOutCPPFile, pcPath);
                if (bNeedSlash)
                    strcat(g_pcOutCPPFile, "\\");
            }
            strcat(g_pcOutCPPFile, g_pcClassName);
            strcat(g_pcOutCPPFile, ".cpp");

            // Construct header file path
            g_pcOutHFile = new char[stPathLength + (bNeedSlash ? 1 : 0) + 
                stClassNameLength + 2 + 1];
            g_pcOutHFile[0] = '\0';
            if (stPathLength != 0)
            {
                strcpy(g_pcOutHFile, pcPath);
                if (bNeedSlash)
                    strcat(g_pcOutHFile, "\\");
            }
            strcat(g_pcOutHFile, g_pcClassName);
            strcat(g_pcOutHFile, ".h");
        }
    }

    if (g_pcInFile == NULL || !bClassNameOK)
    {
        PrintUsage();
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
void Cleanup()
{
    delete[] g_pcInFile;
    delete[] g_pcOutCPPFile;
    delete[] g_pcOutHFile;
    delete[] g_pcClassName;
    delete[] g_pcModifier;
}
//---------------------------------------------------------------------------
int Process(int argc, char** argv)
{
    if (!CheckArguments(argc, argv))
    {
        return 1;
    }

    NiMaterialNodeXMLLibraryParser kParser;
    bool bSuccess = kParser.Load(g_pcInFile, g_pcOutCPPFile, 
        g_pcOutHFile, g_pcClassName, g_pcModifier, g_pcPCH);

    return (bSuccess ? 0 : 1);
}
//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    int iRetValue = Process(argc, argv);

    Cleanup();

    return iRetValue;
}
//---------------------------------------------------------------------------
