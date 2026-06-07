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

#include "NiSceneGraphPrinter.h"

#include <NiMeshLib.h>
#include <NiAnimation.h>
#include <NiCollision.h>
#include <NiPortal.h>
#include <NiParticle.h>

char* gpcInFile = NULL;

#include <NiLicense.h>

NiEmbedGamebryoLicenseCode;

//---------------------------------------------------------------------------
void PrintUsage()
{
    fputs(
        "Usage: SceneGraphPrinter -in input_filename\n"
        "    [-ts integer_tabstop_width]\n"
        "    [-ac (show application culled status)]\n"
        "    [-bs (show bounding sphere data)]\n"
        "    [-mem (show memory address)]\n"
        "    [-prop (list object properties)]\n"
        "    [-trans (show transforms)]\n"
        "    [-extra (list extra data)]\n"
        "    [-mesh (show mesh data)]\n"
        "    [-meshverbose (show mesh data including stream values)]\n"
        "    [-modifiers (show modifiers)]\n"
        "    [-notimecontrollers (filters out timecontrollers)]\n"
        "    [-skipnodes (filters out nodes)]\n"
        "    [-out output_filename\n\n",
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
            gpcInFile = new char[stLen];
            if (gpcInFile == NULL)
                return false;
            NiStrcpy(gpcInFile, stLen, argv[i + 1]);
            i++;
        }

        if (!strcmp("-out", argv[i]))
        {
            if ((i + 1) >= argc)
                return false;

            size_t stLen = strlen(argv[i + 1]) + 1;
            if (stLen == 0)
                return false;

            char acOutFile[MAX_PATH];
            NiStrcpy(acOutFile, MAX_PATH, argv[i + 1]);
            i++;

            NiSceneGraphPrinter::OpenNewLog(acOutFile);
        }

        if (!strcmp("-ts", argv[i]))
        {
            if ((i + 1) >= argc)
                return false;

            int ts = atoi(argv[i + 1]);
            if (ts >= 0)
                NiSceneGraphPrinter::ms_usTabStop = (unsigned short)ts;                
            i++;
        }

        if (!strcmp("-ac", argv[i]))
            NiSceneGraphPrinter::ms_bShowAppCulled = true;

        if (!strcmp("-bs", argv[i]))
            NiSceneGraphPrinter::ms_bShowBoundingSpheres = true;

        if (!strcmp("-mem", argv[i]))
            NiSceneGraphPrinter::ms_bShowMemoryAddress = true;

        if (!strcmp("-prop", argv[i]))
            NiSceneGraphPrinter::ms_bShowProperties = true;

        if (!strcmp("-trans", argv[i]))
            NiSceneGraphPrinter::ms_bShowTransforms = true;

        if (!strcmp("-extra", argv[i]))
            NiSceneGraphPrinter::ms_bShowExtraData = true;

        if (!strcmp("-mesh", argv[i]))
            NiSceneGraphPrinter::ms_bShowMeshDataStreams = true;

        if (!strcmp("-meshverbose", argv[i]))
            NiSceneGraphPrinter::ms_bShowMeshDataStreamsVerbose= true;

        if (!strcmp("-notimecontrollers", argv[i]))
            NiSceneGraphPrinter::ms_bShowTimeControllers = false;

        if (!strcmp("-modifiers", argv[i]))
            NiSceneGraphPrinter::ms_bShowModifiers = true;

        if (!strcmp("-skipnodes", argv[i]))
            NiSceneGraphPrinter::ms_bShowNodes = false;
    }

    if (gpcInFile == NULL)
    {
        PrintUsage();
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
void Cleanup()
{
    delete[] gpcInFile;
}
//---------------------------------------------------------------------------
int Process(int argc, char** argv)
{
    if (!CheckArguments(argc, argv))
    {
        return 1;
    }

    //
    // Load nif file
    //
    NiStream kStream;

    if (!kStream.Load(gpcInFile))
    {
        fputs("Error loading stream.\n", stderr);
        return 1;
    }

    int iCount = kStream.GetObjectCount();
    
    if (iCount < 1) 
    {
        fputs("No top-level file objects.\n", stderr);
        return 0;
    }
    
    for (int i = 0; i < iCount; i++)
    {
        NiObject* pkObject = kStream.GetObjectAt(i);

        if (NiIsKindOf(NiAVObject, pkObject))
        {
            ((NiAVObject*) pkObject)->Update(0.0f);
            NiMesh::CompleteSceneModifiers(((NiAVObject*) pkObject));
        }


        NiSceneGraphPrinter::RecursivePrint(pkObject, 0);
    }

    puts("\n\n");
    puts(gpcInFile);
    size_t stLength = strlen(gpcInFile);
    for (unsigned int ui = 0; ui < stLength; ui++) 
    {
        putchar('-');
    }
    puts("\n");
    fflush(stdout);
    NiSceneGraphPrinter::SummaryPrint();

    return 0;

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

    NiShutdown();

    return iRetValue;
}
//---------------------------------------------------------------------------
