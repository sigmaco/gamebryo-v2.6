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

#ifndef NISCENEGRAPHPRINTER_H
#define NISCENEGRAPHPRINTER_H

class NiObject;
#include <iostream>
#include <fstream>

class NiSceneGraphPrinter
{
public:

    static void PrintID(NiObject* pkObject, unsigned short usIndent);
    static void PrintAppCulled(NiObject* pkObject, unsigned short usIndent);
    static void PrintTransforms(NiObject* pkObject, unsigned short usIndent);
    static void PrintBoundingSpheres(NiObject* pkObject,
        unsigned short usIndent);
    static void PrintProperties(NiObject* pkObject, unsigned short usIndent);
    static void PrintExtraData(NiObject* pkObject, unsigned short usIndent);
    static void PrintMeshDataStreams(NiObject* pkObject,
        unsigned short usIndent);
    static void RecursivePrint(NiObject* pkObject, unsigned short usIndent);
    static void PrintTriSummary();
    static void SummaryPrint();
    static void OpenLog();
    static void OpenNewLog(const char* pcLogFile);
    static void CloseLog();
    static void InitStats();

    static bool ms_bShowAppCulled;
    static bool ms_bDropAppCulled;
    static bool ms_bShowBoundingSpheres;
    static bool ms_bShowMemoryAddress;
    static bool ms_bShowProperties;
    static bool ms_bShowTransforms;
    static bool ms_bShowExtraData;
    static bool ms_bShowMeshDataStreams;
    static bool ms_bShowMeshDataStreamsVerbose;
    static bool ms_bShowTimeControllers;
    static bool ms_bShowModifiers;
    static bool ms_bShowNodes;

    static unsigned short ms_usTabStop;
    static unsigned int ms_uiObjectCount;
    static unsigned int ms_uiDepth;

    static unsigned int ms_uiTriShapeCount;
    static unsigned int ms_uiTriShape_TriCount;
    static unsigned int ms_uiTriShape_LowTriCount;
    static unsigned int ms_uiTriShape_HighTriCount;
    static unsigned int ms_uiTriShape_ZeroTriCount;
    static unsigned int ms_uiTriStripsCount;
    static unsigned int ms_uiTriStrips_TriCount;
    static unsigned int ms_uiTriStrips_LowTriCount;
    static unsigned int ms_uiTriStrips_HighTriCount;
    static unsigned int ms_uiTriStrips_ZeroTriCount;

    static std::ostream* ms_pkOstr;

private:
    static bool ms_bFirstTriShape;
    static bool ms_bFirstTriStrips;

};

#endif // #ifndef NISCENEGRAPHPRINTER_H
